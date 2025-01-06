/* vim: set tabstop=4 shiftwidth=4 softtabstop=4 expandtab smarttab : */
/**
 * @file {file}
 * @author Soo Han, Kim (princeb612.kr@gmail.com)
 * @desc
 *
 * Revision History
 * Date         Name                Description
 */

#include <sdk/net/tls1/tls.hpp>
#include <sdk/net/tls1/tls_advisor.hpp>
#include <sdk/net/tls1/tls_record.hpp>

namespace hotplace {
namespace net {

tls_handshake::tls_handshake(tls_session* session) : tls_record(tls_content_type_handshake, session) {}

return_t tls_handshake::read_data(tls_direction_t dir, const byte_t* stream, size_t size, size_t& pos, stream_t* debugstream) {
    return_t ret = errorcode_t::success;
    __try2 {
        uint16 len = get_length();

        {
            auto session = get_session();
            size_t tpos = 0;
            size_t recpos = get_header_range().begin;
            tls_advisor* tlsadvisor = tls_advisor::get_instance();

            auto session_info = session->get_session_info(dir);
            if (session_info.doprotect()) {
                /**
                 * RFC 2246 6.2.3. Record payload protection
                 *     struct {
                 *         ContentType type;
                 *         ProtocolVersion version;
                 *         uint16 length;
                 *         select (CipherSpec.cipher_type) {
                 *             case stream: GenericStreamCipher;
                 *             case block: GenericBlockCipher;
                 *         } fragment;
                 *     } TLSCiphertext;
                 * RFC 2246 6.2.3.1. Null or standard stream cipher
                 *     stream-ciphered struct {
                 *         opaque content[TLSCompressed.length];
                 *         opaque MAC[CipherSpec.hash_size];
                 *     } GenericStreamCipher;
                 *     HMAC_hash(MAC_write_secret, seq_num + TLSCompressed.type +
                 *                   TLSCompressed.version + TLSCompressed.length +
                 *                   TLSCompressed.fragment));
                 * RFC 2246 6.2.3.2. CBC block cipher
                 *     block-ciphered struct {
                 *         opaque content[TLSCompressed.length];
                 *         opaque MAC[CipherSpec.hash_size];
                 *         uint8 padding[GenericBlockCipher.padding_length];
                 *         uint8 padding_length;
                 *     } GenericBlockCipher;
                 */
                tls_protection& protection = session->get_tls_protection();
                binary_t plaintext;
                binary_t tag;
                auto tlsversion = protection.get_tls_version();
                if (is_basedon_tls13(tlsversion)) {
                    ret = protection.decrypt_tls13(session, dir, stream, len, recpos, plaintext, tag, debugstream);
                } else {
                    ret = protection.decrypt_tls1(session, dir, stream, size, recpos, plaintext, debugstream);
                }
                if (errorcode_t::success == ret) {
                    tpos = 0;
                    ret = tls_dump_handshake(debugstream, session, &plaintext[0], plaintext.size(), tpos, dir);
                }
            } else {
                tpos = pos;
                ret = tls_dump_handshake(debugstream, session, stream, pos + len, tpos, dir);
            }
        }
    }
    __finally2 {
        // do nothing
    }
    return ret;
}

return_t tls_handshake::write(tls_direction_t dir, binary_t& bin, stream_t* debugstream) { return errorcode_t::not_supported; }

}  // namespace net
}  // namespace hotplace
