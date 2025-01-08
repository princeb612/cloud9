/* vim: set tabstop=4 shiftwidth=4 softtabstop=4 expandtab smarttab : */
/**
 * @file {file}
 * @author Soo Han, Kim (princeb612.kr@gmail.com)
 * @desc
 *
 * Revision History
 * Date         Name                Description
 */

#include <sdk/base/basic/dump_memory.hpp>
#include <sdk/io/basic/payload.hpp>
#include <sdk/net/tls1/tls.hpp>
#include <sdk/net/tls1/tls_advisor.hpp>
#include <sdk/net/tls1/tls_handshake.hpp>

namespace hotplace {
namespace net {

tls_handshake_server_hello::tls_handshake_server_hello(tls_session* session) : tls_handshake(tls_hs_server_hello, session) {}

return_t tls_handshake_server_hello::do_handshake(tls_direction_t dir, const byte_t* stream, size_t size, size_t& pos, stream_t* debugstream) {
    return_t ret = errorcode_t::success;
    __try2 {
        auto session = get_session();
        if (nullptr == session) {
            ret = errorcode_t::invalid_context;
            __leave2;
        }
        auto hspos = get_header_range().begin;
        auto hdrsize = get_header_size();
        auto& protection = session->get_tls_protection();

        {
            ret = do_read(dir, stream, size, pos, debugstream);

            // calculates the hash of all handshake messages to this point (ClientHello and ServerHello).
            binary_t hello_hash;
            if (tls_1_rtt == protection.get_flow()) {
                const binary_t& client_hello = protection.get_item(tls_context_client_hello);
                protection.calc_transcript_hash(session, &client_hello[0], client_hello.size());  // client_hello
            }

            protection.calc_transcript_hash(session, stream + hspos, hdrsize, hello_hash);  // server_hello
            ret = protection.calc(session, tls_hs_server_hello, dir);
            session->get_session_info(dir).set_status(get_type());
            if (errorcode_t::success != ret) {
                protection.set_flow(tls_hello_retry_request);

                /**
                 *    RFC 8446 4.4.1.  The Transcript Hash
                 *
                 *       As an exception to this general rule, when the server responds to a
                 *       ClientHello with a HelloRetryRequest, the value of ClientHello1 is
                 *       replaced with a special synthetic handshake message of handshake type
                 *       "message_hash" containing Hash(ClientHello1).  I.e.,
                 *
                 *       Transcript-Hash(ClientHello1, HelloRetryRequest, ... Mn) =
                 *           Hash(message_hash ||        // Handshake type
                 *                00 00 Hash.length  ||  // Handshake message length (bytes)
                 *                Hash(ClientHello1) ||  // Hash of ClientHello1
                 *                HelloRetryRequest  || ... || Mn)
                 */
                binary_t handshake_hash;
                const binary_t& client_hello = protection.get_item(tls_context_client_hello);
                protection.reset_transcript_hash(session);
                protection.calc_transcript_hash(session, &client_hello[0], client_hello.size(), handshake_hash);

                binary message_hash;
                message_hash << uint8(tls_hs_message_hash) << uint16(0) << byte_t(handshake_hash.size()) << handshake_hash;
                const binary_t& synthetic_handshake_message = message_hash.get();

                protection.reset_transcript_hash(session);
                protection.calc_transcript_hash(session, &synthetic_handshake_message[0], synthetic_handshake_message.size());
                protection.calc_transcript_hash(session, stream + hspos, hdrsize, hello_hash);
            }

            protection.clear_item(tls_context_client_hello);
        }
    }
    __finally2 {
        // do nothing
    }
    return ret;
}

return_t tls_handshake_server_hello::do_read(tls_direction_t dir, const byte_t* stream, size_t size, size_t& pos, stream_t* debugstream) {
    return_t ret = errorcode_t::success;
    __try2 {
        auto session = get_session();
        if (nullptr == session) {
            ret = errorcode_t::invalid_context;
            __leave2;
        }
        if (nullptr == stream) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        {
            tls_advisor* tlsadvisor = tls_advisor::get_instance();

            constexpr char constexpr_version[] = "version";
            constexpr char constexpr_random[] = "random";
            constexpr char constexpr_session_id_len[] = "session id len";
            constexpr char constexpr_session_id[] = "session id";
            constexpr char constexpr_cipher_suite[] = "cipher suite";
            constexpr char constexpr_compression_method[] = "compression method";
            constexpr char constexpr_extension_len[] = "extension len";
            constexpr char constexpr_extension[] = "extension";

            constexpr char constexpr_group_dtls[] = "dtls";
            constexpr char constexpr_cookie_len[] = "cookie len";
            constexpr char constexpr_cookie[] = "cookie";

            /* RFC 8446 4.1.3.  Server Hello */

            auto& protection = session->get_tls_protection();
            uint16 record_version = protection.get_record_version();
            uint16 version = 0;

            binary_t random;
            binary_t session_id;
            uint8 session_ids = 0;
            uint16 cipher_suite = 0;
            uint8 compression_method = 0;
            uint8 extension_len = 0;

            binary_t bin_server_hello;

            {
                payload pl;
                pl << new payload_member(uint16(0), true, constexpr_version) << new payload_member(binary_t(), constexpr_random)
                   << new payload_member(uint8(0), constexpr_session_id_len) << new payload_member(binary_t(), constexpr_session_id)
                   << new payload_member(uint16(0), true, constexpr_cipher_suite) << new payload_member(uint8(0), constexpr_compression_method)
                   << new payload_member(uint16(0), true, constexpr_extension_len);

                pl.set_group(constexpr_group_dtls, (record_version >= dtls_12));

                pl.select(constexpr_random)->reserve(32);
                pl.set_reference_value(constexpr_session_id, constexpr_session_id_len);
                pl.read(stream, size, pos);

                // RFC 8446 4.1.1.  Cryptographic Negotiation
                // If PSK is being used, ... "pre_shared_key" extension indicating the selected key
                // When (EC)DHE is in use, ... "key_share" extension
                // When authenticating via a certificate, ... Certificate (Section 4.4.2) and CertificateVerify (Section 4.4.3)

                version = pl.t_value_of<uint16>(constexpr_version);

                pl.get_binary(constexpr_random, random);
                session_ids = pl.t_value_of<uint8>(constexpr_session_id_len);
                pl.get_binary(constexpr_session_id, session_id);
                cipher_suite = pl.t_value_of<uint16>(constexpr_cipher_suite);
                compression_method = pl.t_value_of<uint8>(constexpr_compression_method);
                extension_len = pl.t_value_of<uint16>(constexpr_extension_len);
            }

            if (debugstream) {
                debugstream->autoindent(1);
                debugstream->printf(" > %s 0x%04x (%s)\n", constexpr_version, version, tlsadvisor->tls_version_string(version).c_str());
                debugstream->printf(" > %s\n", constexpr_random);
                if (random.size()) {
                    // dump_memory(random, s, 16, 3, 0x0, dump_notrunc);
                    debugstream->printf("   %s\n", base16_encode(random).c_str());
                }
                debugstream->printf(" > %s\n", constexpr_session_id);
                if (session_id.size()) {
                    debugstream->printf("   %s\n", base16_encode(session_id).c_str());
                }
                debugstream->printf(" > %s 0x%04x %s\n", constexpr_cipher_suite, cipher_suite, tlsadvisor->cipher_suite_string(cipher_suite).c_str());
                debugstream->printf(" > %s %i %s\n", constexpr_compression_method, compression_method,
                                    tlsadvisor->compression_method_string(compression_method).c_str());
                debugstream->printf(" > %s 0x%02x(%i)\n", constexpr_extension_len, extension_len, extension_len);
                debugstream->autoindent(0);
            }

            for (return_t test = errorcode_t::success;;) {
                test = tls_dump_extension(tls_hs_server_hello, session, stream, size, pos, debugstream);
                if (errorcode_t::no_more == test) {
                    break;
                } else if (errorcode_t::success == test) {
                    continue;
                } else {
                    ret = test;
                    break;
                }
            }

            // cipher_suite
            protection.set_cipher_suite(cipher_suite);

            // server_key_update
            session->get_tls_protection().set_item(tls_context_server_hello_random, random);
        }
    }
    __finally2 {
        // do nothing
    }
    return ret;
}

}  // namespace net
}  // namespace hotplace
