/* vim: set tabstop=4 shiftwidth=4 softtabstop=4 expandtab smarttab : */
/**
 * @file {file}
 * @author Soo Han, Kim (princeb612.kr@gmail.com)
 * @desc
 *          RFC 8446 The Transport Layer Security (TLS) Protocol Version 1.3
 *          RFC 6066 Transport Layer Security (TLS) Extensions: Extension Definitions
 *          RFC 5246 The Transport Layer Security (TLS) Protocol Version 1.2
 *
 * Revision History
 * Date         Name                Description
 */

#include <sdk/base/basic/dump_memory.hpp>
#include <sdk/base/unittest/trace.hpp>
#include <sdk/crypto/crypto/cipher_encrypt.hpp>
#include <sdk/io/basic/payload.hpp>
#include <sdk/net/tls1/record/dtls13_ciphertext.hpp>
#include <sdk/net/tls1/record/tls_record_ack.hpp>
#include <sdk/net/tls1/record/tls_record_alert.hpp>
#include <sdk/net/tls1/tls_advisor.hpp>
#include <sdk/net/tls1/tls_protection.hpp>
#include <sdk/net/tls1/tls_session.hpp>

namespace hotplace {
namespace net {

constexpr char constexpr_group_c[] = "group c";       // connection id
constexpr char constexpr_group_s16[] = "group s 16";  // sequence
constexpr char constexpr_group_s8[] = "group s 8";    // sequence
constexpr char constexpr_group_l[] = "group l";       // length
constexpr char constexpr_group_e[] = "group e";       // epoch

constexpr char constexpr_unified_header[] = "unified header";
constexpr char constexpr_connection_id[] = "connection id";
constexpr char constexpr_sequence16[] = "sequence 16";
constexpr char constexpr_sequence8[] = "sequence 8";
constexpr char constexpr_len[] = "len";
constexpr char constexpr_encdata[] = "enc data + tag";
constexpr char constexpr_sequence[] = "sequence";
constexpr char constexpr_recno[] = "record no";

dtls13_ciphertext::dtls13_ciphertext(uint8 type, tls_session* session) : tls_record(type, session), _sequence(0), _sequence_len(0), _offset_encdata(0) {}

tls_handshakes& dtls13_ciphertext::get_handshakes() { return _handshakes; }

return_t dtls13_ciphertext::do_read_header(tls_direction_t dir, const byte_t* stream, size_t size, size_t& pos) {
    return_t ret = errorcode_t::success;
    __try2 {
        size_t recpos = pos;

        uint8 uhdr = 0;
        binary_t connection_id;
        uint16 sequence = 0;
        uint8 sequence_len = 0;
        size_t offset_sequence = 0;
        uint16 len = 0;
        binary_t encdata;
        size_t offset_encdata = 0;
        {
            payload pl;
            pl << new payload_member(uint8(0), constexpr_unified_header)                          //
               << new payload_member(binary_t(), constexpr_connection_id, constexpr_group_c)      // cid
               << new payload_member(uint16(0), true, constexpr_sequence16, constexpr_group_s16)  // seq 16
               << new payload_member(uint8(0), constexpr_sequence8, constexpr_group_s8)           // seq 8
               << new payload_member(uint16(0), true, constexpr_len, constexpr_group_l)           // l
               << new payload_member(binary_t(), constexpr_encdata);

            /**
             * 0 1 2 3 4 5 6 7
             * +-+-+-+-+-+-+-+-+
             * |0|0|1|C|S|L|E E|
             * +-+-+-+-+-+-+-+-+
             */

            auto lambda_condition = [&](payload* pl, payload_member* item) -> void {
                auto uhdr = pl->t_value_of<uint8>(item);
                pl->set_group(constexpr_group_c, (0x10 & uhdr));
                pl->set_group(constexpr_group_s16, 0 != (0x08 & uhdr));
                pl->set_group(constexpr_group_s8, 0 == (0x08 & uhdr));
                pl->set_group(constexpr_group_l, (0x04 & uhdr));
                if (0x04 & uhdr) {
                    pl->set_reference_value(constexpr_encdata, constexpr_len);
                }
            };
            pl.set_condition(constexpr_unified_header, lambda_condition);
            pl.read(stream, size, pos);

            uhdr = pl.t_value_of<uint8>(constexpr_unified_header);
            if (pl.get_group_condition(constexpr_group_c)) {
                pl.select(constexpr_connection_id)->get_variant().to_binary(connection_id);
            }
            if (pl.get_group_condition(constexpr_group_s16)) {
                sequence = pl.t_value_of<uint16>(constexpr_sequence16);
                sequence_len = 2;
                offset_sequence = pl.offset_of(constexpr_group_s16);
            }
            if (pl.get_group_condition(constexpr_group_s8)) {
                sequence = pl.t_value_of<uint16>(constexpr_sequence8);
                sequence_len = 1;
                offset_sequence = pl.offset_of(constexpr_group_s8);
            }
            if (pl.get_group_condition(constexpr_group_l)) {
                len = pl.t_value_of<uint16>(constexpr_len);
            }
            pl.select(constexpr_encdata)->get_variant().to_binary(encdata);
            offset_encdata = pl.offset_of(constexpr_encdata);
        }

        if (istraceable()) {
            basic_stream dbs;
            dbs.printf("> %s %02x (C:%i S:%i L:%i E:%x)\n", constexpr_unified_header, uhdr, (uhdr & 0x10) ? 1 : 0, (uhdr & 0x08) ? 1 : 0, (uhdr & 0x04) ? 1 : 0,
                       (uhdr & 0x03));
            if (connection_id.size()) {
                dbs.printf("> %s %s\n", constexpr_connection_id, base16_encode(connection_id).c_str());
            }
            dbs.printf("> %s %04x\n", constexpr_sequence, sequence);
            dbs.printf("> %s %04x\n", constexpr_len, len);
            dbs.printf("> %s\n", constexpr_encdata);
            dump_memory(encdata, &dbs, 16, 3, 0x0, dump_notrunc);

            trace_debug_event(category_tls1, tls_event_read, &dbs);
        }

        {
            _content_type = uhdr;
            _bodysize = len;

            _range.begin = recpos;
            _range.end = pos;

            _sequence = sequence;
            _sequence_len = sequence_len;
            _offset_encdata = offset_encdata;
        }
    }
    __finally2 {
        // do nothing
    }
    return ret;
}

return_t dtls13_ciphertext::do_read_body(tls_direction_t dir, const byte_t* stream, size_t size, size_t& pos) {
    return_t ret = errorcode_t::success;
    __try2 {
        auto recpos = offsetof_header();
        auto sequence = _sequence;
        auto sequence_len = _sequence_len;
        auto offset_encdata = _offset_encdata;

        uint16 recno = 0;
        uint16 rec_enc = 0;
        binary_t ciphertext;
        tls_secret_t sn_key;
        auto session = get_session();
        auto& protection = session->get_tls_protection();
        auto hsstatus = session->get_session_info(dir).get_status();
        {
            cipher_encrypt_builder builder;
            auto cipher = builder.set(aes128, ecb).build();
            size_t blocksize = 16;  // minimal block
            if (cipher) {
                if (from_server == dir) {
                    if (tls_hs_finished == hsstatus) {
                        sn_key = tls_secret_application_server_sn_key;
                    } else {
                        sn_key = tls_secret_handshake_server_sn_key;
                    }
                } else {
                    if (tls_hs_finished == hsstatus) {
                        sn_key = tls_secret_application_client_sn_key;
                    } else {
                        sn_key = tls_secret_handshake_client_sn_key;
                    }
                }
                cipher->encrypt(protection.get_item(sn_key), binary_t(), stream + offset_encdata, blocksize, ciphertext);
                cipher->release();
            }

            // recno
            if (2 == sequence_len) {
                rec_enc = t_binary_to_integer<uint16>(ciphertext);
            } else {
                rec_enc = t_binary_to_integer<uint8>(ciphertext);
            }
            recno = sequence ^ rec_enc;
        }

        binary_t aad;
        {
            binary_append(aad, stream + recpos, offset_encdata);
            for (auto i = 0; i < sequence_len; i++) {
                aad[1 + i] ^= ciphertext[i];
            }
        }

        if (istraceable()) {
            basic_stream dbs;
            dbs.printf("> record number key %s\n", base16_encode(protection.get_item(sn_key)).c_str());

            // s->printf("> %s %04x\n", constexpr_recno, recno);
            dbs.printf("> %s %04x (%04x XOR %s)\n", constexpr_recno, recno, sequence, base16_encode(ciphertext).substr(0, sequence_len << 1).c_str());
            dump_memory(ciphertext, &dbs, 16, 3, 0x0, dump_notrunc);

            trace_debug_event(category_tls1, tls_event_read, &dbs);
        }

        binary_t plaintext;
        {
            // decryption
            ret = protection.decrypt(session, dir, stream, size - aad.size(), recpos, plaintext, aad);
        }

        if (istraceable()) {
            basic_stream dbs;
            dbs.printf("> aad\n");
            dump_memory(aad, &dbs, 16, 3, 0x0, dump_notrunc);
            dbs.printf("> plaintext\n");
            dump_memory(plaintext, &dbs, 16, 3, 0x0, dump_notrunc);

            trace_debug_event(category_tls1, tls_event_read, &dbs);
        }

        // record
        if (errorcode_t::success == ret) {
            uint8 hstype = *plaintext.rbegin();
            size_t tpos = 0;

            switch (hstype) {
                case tls_content_type_alert: {
                    tls_record_alert alert(session);
                    ret = alert.read_plaintext(dir, &plaintext[0], plaintext.size() - 1, tpos);
                } break;
                case tls_content_type_handshake: {
                    auto handshake = tls_handshake::read(session, dir, &plaintext[0], plaintext.size() - 1, tpos);
                    get_handshakes().add(handshake);
                } break;
                case tls_content_type_application_data: {
                    if (istraceable()) {
                        basic_stream dbs;
                        dbs.printf("> application data\n");
                        dump_memory(&plaintext[0], plaintext.size() - 1, &dbs, 16, 3, 0x0, dump_notrunc);

                        trace_debug_event(category_tls1, tls_event_read, &dbs);
                    }
                } break;
                case tls_content_type_ack: {
                    tls_record_ack ack(session);
                    ret = ack.do_read_body(dir, &plaintext[0], plaintext.size() - 1, tpos);
                } break;
            }
        }
    }
    __finally2 {
        // do nothing
    }
    return ret;
}

return_t dtls13_ciphertext::do_write_header(tls_direction_t dir, binary_t& bin, const binary_t& body) {
    return_t ret = errorcode_t::success;
    return ret;
}

return_t dtls13_ciphertext::do_write_body(tls_direction_t dir, binary_t& bin) {
    return_t ret = errorcode_t::success;
    return ret;
}

}  // namespace net
}  // namespace hotplace
