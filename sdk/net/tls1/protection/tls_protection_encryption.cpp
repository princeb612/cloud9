/* vim: set tabstop=4 shiftwidth=4 softtabstop=4 expandtab smarttab : */
/**
 * @file {file}
 * @author Soo Han, Kim (princeb612.kr@gmail.com)
 * @desc
 *
 * Revision History
 * Date         Name                Description
 */

#include <sdk/base/basic/binary.hpp>
#include <sdk/base/unittest/trace.hpp>
#include <sdk/crypto/basic/crypto_advisor.hpp>
#include <sdk/crypto/crypto/cipher_encrypt.hpp>
#include <sdk/io/asn.1/types.hpp>
#include <sdk/io/basic/payload.hpp>
#include <sdk/net/tls1/tls_advisor.hpp>
#include <sdk/net/tls1/tls_protection.hpp>
#include <sdk/net/tls1/tls_session.hpp>
// debug
#include <sdk/base/basic/dump_memory.hpp>
#include <sdk/base/stream/basic_stream.hpp>

namespace hotplace {
namespace net {

return_t tls_protection::get_cipher_info(tls_session *session, crypt_algorithm_t &alg, crypt_mode_t &mode) {
    return_t ret = errorcode_t::success;
    __try2 {
        if (nullptr == session) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        alg = crypt_alg_unknown;
        mode = mode_unknown;

        auto session_type = session->get_type();
        tls_advisor *tlsadvisor = tls_advisor::get_instance();

        auto cs = get_cipher_suite();

        if ((0 == cs) && (session_quic == session_type)) {
            cs = 0x1301;  // TLS_AES_128_GCM_SHA256
        }

        const tls_cipher_suite_t *hint = tlsadvisor->hintof_cipher_suite(cs);
        if (nullptr == hint) {
            ret = errorcode_t::not_supported;
            __leave2;
        }

        alg = hint->cipher;
        mode = hint->mode;
    }
    __finally2 {}
    return ret;
}

return_t tls_protection::build_iv(tls_session *session, tls_secret_t type, binary_t &iv, uint64 recordno) {
    return_t ret = errorcode_t::success;
    __try2 {
        if (nullptr == session) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        iv = get_item(type);
        if (iv.empty()) {
            ret = errorcode_t::bad_data;
            __leave2;
        }

        for (uint64 i = 0; i < 8; i++) {
            iv[12 - 1 - i] ^= ((recordno >> (i * 8)) & 0xff);
        }
    }
    __finally2 {
        // do nothing
    }
    return ret;
}

uint8 tls_protection::get_tag_size() {
    uint8 ret_value = 0;
    crypto_advisor *advisor = crypto_advisor::get_instance();
    tls_advisor *tlsadvisor = tls_advisor::get_instance();
    const tls_cipher_suite_t *hint = tlsadvisor->hintof_cipher_suite(get_cipher_suite());
    if (hint) {
        auto hmac_alg = hint->mac;
        auto hint_digest = advisor->hintof_digest(hmac_alg);

        auto cipher = hint->cipher;
        auto mode = hint->mode;
        auto dlen = sizeof_digest(hint_digest);

        switch (mode) {
            case gcm:
            case mode_poly1305:
                ret_value = 16;
                break;
            case ccm:
                ret_value = 14;
                break;
            case ccm8:
                ret_value = 8;
                break;
            default:
                ret_value = dlen;
                break;
        }
    }
    return ret_value;
}

return_t tls_protection::get_aead_key(tls_session *session, tls_direction_t dir, tls_secret_t &secret_key, tls_secret_t &secret_iv) {
    return_t ret = errorcode_t::success;
    __try2 {
        if (nullptr == session) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        auto session_type = session->get_type();
        auto hsstatus = session->get_session_info(dir).get_status();
        if (session_tls == session_type) {
            // TLS, DTLS

            if (from_client == dir) {
                auto flow = get_flow();
                if (tls_1_rtt == flow || tls_hello_retry_request == flow) {
                    if (tls_hs_finished == hsstatus) {
                        secret_key = tls_secret_application_client_key;
                        secret_iv = tls_secret_application_client_iv;
                    } else {
                        secret_key = tls_secret_handshake_client_key;
                        secret_iv = tls_secret_handshake_client_iv;
                    }
                } else {
                    // 1-RTT
                    // 0-RTT
                    // client_hello         c e traffic
                    // end_of_early_data    c hs traffic
                    // finished             c ap traffic
                    switch (hsstatus) {
                        case tls_hs_end_of_early_data: {
                            secret_key = tls_secret_handshake_client_key;
                            secret_iv = tls_secret_handshake_client_iv;
                        } break;
                        case tls_hs_finished: {
                            secret_key = tls_secret_application_client_key;
                            secret_iv = tls_secret_application_client_iv;
                        } break;
                        case tls_hs_client_hello:
                        default: {
                            // use early traffic
                            secret_key = tls_secret_c_e_traffic_key;
                            secret_iv = tls_secret_c_e_traffic_iv;
                        } break;
                    }
                }
            } else {
                // from_server
                if (tls_hs_finished == hsstatus) {
                    secret_key = tls_secret_application_server_key;
                    secret_iv = tls_secret_application_server_iv;
                } else {
                    secret_key = tls_secret_handshake_server_key;
                    secret_iv = tls_secret_handshake_server_iv;
                }
            }
        } else if (session_quic == session_type) {
            // QUIC
            if (from_client == dir) {
                if (tls_hs_finished == hsstatus) {
                    secret_key = tls_secret_application_quic_client_key;
                    secret_iv = tls_secret_application_quic_client_iv;
                } else if (tls_hs_server_hello == hsstatus) {
                    secret_key = tls_secret_handshake_quic_client_key;
                    secret_iv = tls_secret_handshake_quic_client_iv;
                } else {
                    secret_key = tls_context_quic_initial_client_key;
                    secret_iv = tls_context_quic_initial_client_iv;
                }
            } else {
                if (tls_hs_finished == hsstatus) {
                    secret_key = tls_secret_application_quic_server_key;
                    secret_iv = tls_secret_application_quic_server_iv;
                } else if (tls_hs_server_hello == hsstatus) {
                    secret_key = tls_secret_handshake_quic_server_key;
                    secret_iv = tls_secret_handshake_quic_server_iv;
                } else {
                    secret_key = tls_context_quic_initial_server_key;
                    secret_iv = tls_context_quic_initial_server_iv;
                }
            }
        }
    }
    __finally2 {
        // do nothing
    }
    return ret;
}

return_t tls_protection::get_cbc_hmac_key(tls_session *session, tls_direction_t dir, tls_secret_t &secret_key, tls_secret_t &secret_mac_key) {
    return_t ret = errorcode_t::success;
    __try2 {
        if (from_client == dir) {
            secret_key = tls_secret_client_key;
            secret_mac_key = tls_secret_client_mac_key;
        } else {
            secret_key = tls_secret_server_key;
            secret_mac_key = tls_secret_server_mac_key;
        }
    }
    __finally2 {
        // do nothing
    }
    return ret;
}

// RFC 2246 6.2.3 Record payload protection
// length of TLSCiphertext.fragment may not exceed 2^14 + 2048
#define TLS_CIPHERTEXT_MAXSIZE ((2 << 14) + 2048)

return_t tls_protection::encrypt(tls_session *session, tls_direction_t dir, const binary_t &plaintext, binary_t &ciphertext, const binary_t &additional,
                                 binary_t &tag) {
    return_t ret = errorcode_t::success;
    __try2 {
        if (nullptr == session) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        auto record_version = get_lagacy_version();
        size_t content_header_size = 0;
        tls_advisor *tlsadvisor = tls_advisor::get_instance();

        auto mode = mode_unknown;

        {
            const tls_cipher_suite_t *hint = tlsadvisor->hintof_cipher_suite(get_cipher_suite());
            if (nullptr == hint) {
                ret = errorcode_t::not_supported;
                __leave2;
            }
            mode = hint->mode;
        }
        /**
         * RFC 7366
         * If a server receives an encrypt-then-MAC request extension from a client
         * and then selects a stream or Authenticated Encryption with Associated
         * Data (AEAD) ciphersuite, it MUST NOT send an encrypt-then-MAC
         * response extension back to the client.
         */
        switch (mode) {
            // AEAD
            case gcm:
            case ccm:
            case ccm8:
            case mode_poly1305: {
                ret = encrypt_aead(session, dir, plaintext, ciphertext, additional, tag);
            } break;
            // encrypt-then-MAC
            case cbc: {
                ret = encrypt_cbc_hmac(session, dir, plaintext, ciphertext, additional, tag);
            } break;
        }
        if (errorcode_t::success != ret) {
            __leave2;
        }
        if (ciphertext.size() > TLS_CIPHERTEXT_MAXSIZE) {
            ret = errorcode_t::exceed;
            __leave2;
        }
    }
    __finally2 {}
    return ret;
}

return_t tls_protection::encrypt_aead(tls_session *session, tls_direction_t dir, const binary_t &plaintext, binary_t &ciphertext, const binary_t &aad,
                                      binary_t &tag) {
    return_t ret = errorcode_t::success;
    __try2 {
        if (nullptr == session) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        auto record_version = get_lagacy_version();
        size_t content_header_size = 0;
        tls_advisor *tlsadvisor = tls_advisor::get_instance();

        auto cipher = crypt_alg_unknown;
        auto mode = mode_unknown;

        ret = get_cipher_info(session, cipher, mode);
        if (errorcode_t::success != ret) {
            __leave2;
        }

        crypt_context_t *handle = nullptr;
        openssl_crypt crypt;

        tls_secret_t secret_key;
        tls_secret_t secret_iv;
        get_aead_key(session, dir, secret_key, secret_iv);

        uint64 record_no = 0;
        record_no = session->get_recordno(dir, true);

        auto const &key = get_item(secret_key);
        auto const &iv = get_item(secret_iv);
        binary_t nonce = iv;
        build_iv(session, secret_iv, nonce, record_no);
        ret = crypt.encrypt(cipher, mode, key, nonce, plaintext, ciphertext, aad, tag);

        if (istraceable()) {
            basic_stream dbs;
            dbs.printf("> encrypt\n");
            dbs.printf(" > key[%08x] %s\n", secret_key, base16_encode(key).c_str());
            dbs.printf(" > iv [%08x] %s\n", secret_iv, base16_encode(iv).c_str());
            dbs.printf(" > record no %i\n", record_no);
            dbs.printf(" > nonce %s\n", base16_encode(nonce).c_str());
            dbs.printf(" > aad %s\n", base16_encode(aad).c_str());
            dbs.printf(" > tag %s\n", base16_encode(tag).c_str());
            dbs.printf(" > plaintext\n");
            dump_memory(plaintext, &dbs, 16, 3, 0x0, dump_notrunc);
            dbs.printf(" > ciphertext\n");
            dump_memory(ciphertext, &dbs, 16, 3, 0x0, dump_notrunc);

            trace_debug_event(category_tls1, tls_event_write, &dbs);
        }
    }
    __finally2 {
        // do nothing
    }
    return ret;
}

return_t tls_protection::encrypt_cbc_hmac(tls_session *session, tls_direction_t dir, const binary_t &plaintext, binary_t &ciphertext,
                                          const binary_t &additional, binary_t &maced) {
    return_t ret = errorcode_t::success;
    __try2 {
        if (nullptr == session) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        crypto_advisor *advisor = crypto_advisor::get_instance();
        tls_advisor *tlsadvisor = tls_advisor::get_instance();
        const tls_cipher_suite_t *hint = tlsadvisor->hintof_cipher_suite(get_cipher_suite());
        if (nullptr == hint) {
            ret = errorcode_t::not_supported;
            __leave2;
        }
        auto hint_cipher = advisor->hintof_blockcipher(hint->cipher);
        if (nullptr == hint_cipher) {
            ret = errorcode_t::not_supported;
            __leave2;
        }
        auto ivsize = sizeof_iv(hint_cipher);
        size_t content_header_size = get_header_size();
        binary_t iv;
        binary_append(iv, &additional[content_header_size], ivsize);

        openssl_crypt crypt;

        tls_secret_t secret_key;
        tls_secret_t secret_mac_key;
        get_cbc_hmac_key(session, dir, secret_key, secret_mac_key);

        uint64 record_no = 0;
        record_no = session->get_recordno(dir, true);

        const binary_t &enckey = get_item(secret_key);
        auto enc_alg = hint->cipher;
        auto hmac_alg = hint->mac;  // do not promote insecure algorithm
        const binary_t &mackey = get_item(secret_mac_key);

        /**
         * RFC 7366 3.  Applying Encrypt-then-MAC
         *   -- for TLS 1.1 and greater with an explicit IV
         *   MAC(MAC_write_key, seq_num +
         *       TLSCipherText.type +
         *       TLSCipherText.version +
         *       TLSCipherText.length +
         *       IV +
         *       ENC(content + padding + padding_length));
         */
        binary_t verifydata;
        binary_t aad;
        binary_append(aad, uint64(record_no), hton64);  // sequence
        binary_append(aad, &additional[0], 3);          // rechdr (content_type, version)
        size_t plainsize = 0;

        ret = crypt.cbc_hmac_tls_encrypt(enc_alg, hmac_alg, enckey, mackey, iv, aad, plaintext, ciphertext);

        if (istraceable()) {
            basic_stream dbs;
            dbs.printf("> encrypt\n");
            dbs.printf(" > aad %s\n", base16_encode(aad).c_str());
            dbs.printf(" > enc %s\n", advisor->nameof_cipher(enc_alg, cbc));
            dbs.printf(" > enckey[%08x] %s\n", secret_key, base16_encode(enckey).c_str());
            dbs.printf(" > iv %s\n", base16_encode(iv).c_str());
            dbs.printf(" > mac %s\n", advisor->nameof_md(hmac_alg));
            dbs.printf(" > mackey[%08x] %s\n", secret_mac_key, base16_encode(mackey).c_str());
            dbs.printf(" > record no %i\n", record_no);
            dbs.printf(" > plaintext\n");
            dump_memory(plaintext, &dbs, 16, 3, 0x0, dump_notrunc);
            dbs.printf(" > ciphertext\n");
            dump_memory(ciphertext, &dbs, 16, 3, 0x0, dump_notrunc);

            trace_debug_event(category_tls1, tls_event_read, &dbs);
        }
    }
    __finally2 {
        // do nothing
    }
    return ret;
}

return_t tls_protection::decrypt(tls_session *session, tls_direction_t dir, const byte_t *stream, size_t size, size_t pos, binary_t &plaintext) {
    return_t ret = errorcode_t::success;
    __try2 {
        if (nullptr == session || nullptr == stream) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        if (size > TLS_CIPHERTEXT_MAXSIZE) {
            ret = errorcode_t::exceed;
            __leave2;
        }

        tls_advisor *tlsadvisor = tls_advisor::get_instance();
        const tls_cipher_suite_t *hint = tlsadvisor->hintof_cipher_suite(get_cipher_suite());
        if (nullptr == hint) {
            ret = errorcode_t::not_supported;
            __leave2;
        }
        /**
         * RFC 7366
         * If a server receives an encrypt-then-MAC request extension from a client
         * and then selects a stream or Authenticated Encryption with Associated
         * Data (AEAD) ciphersuite, it MUST NOT send an encrypt-then-MAC
         * response extension back to the client.
         */
        auto mode = hint->mode;
        switch (mode) {
            // AEAD
            case gcm:
            case ccm:
            case ccm8:
            case mode_poly1305: {
                ret = decrypt_aead(session, dir, stream, size, pos, plaintext);
            } break;
            // encrypt-then-MAC
            case cbc: {
                ret = decrypt_cbc_hmac(session, dir, stream, size, pos, plaintext);
            } break;
        }
    }
    __finally2 {}
    return ret;
}

return_t tls_protection::decrypt(tls_session *session, tls_direction_t dir, const byte_t *stream, size_t size, size_t pos, binary_t &plaintext, binary_t &aad) {
    return_t ret = errorcode_t::success;
    __try2 {
        if (nullptr == session || nullptr == stream) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        auto session_type = session->get_type();
        if (session_tls == session_type) {
            tls_advisor *tlsadvisor = tls_advisor::get_instance();
            const tls_cipher_suite_t *hint = tlsadvisor->hintof_cipher_suite(get_cipher_suite());
            if (nullptr == hint) {
                ret = errorcode_t::not_supported;
                __leave2;
            }
            auto mode = hint->mode;
            switch (mode) {
                case gcm:
                case ccm:
                case ccm8:
                case mode_poly1305: {
                    auto aadlen = aad.size();
                    auto tagsize = get_tag_size();
                    binary_t tag;

                    // ... aad(aadlen) encdata tag(tagsize)
                    //     \_ pos
                    binary_append(tag, stream + pos + aadlen + size - tagsize, tagsize);
                    ret = decrypt_aead(session, dir, stream, size - tagsize, pos + aadlen, plaintext, aad, tag);
                } break;
                case cbc: {
                    ret = decrypt_cbc_hmac(session, dir, stream, size, pos, plaintext);
                } break;
            }
        } else if (session_quic == session_type) {
            ret = errorcode_t::not_supported;
        }
    }
    __finally2 {}
    return ret;
}

return_t tls_protection::decrypt(tls_session *session, tls_direction_t dir, const byte_t *stream, size_t size, size_t pos, binary_t &plaintext,
                                 const binary_t &aad, const binary_t &tag) {
    return_t ret = errorcode_t::success;
    __try2 {
        if (nullptr == session || nullptr == stream) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        crypt_algorithm_t alg = crypt_alg_unknown;
        crypt_mode_t mode = mode_unknown;

        ret = get_cipher_info(session, alg, mode);
        if (errorcode_t::success != ret) {
            __leave2;
        }

        switch (mode) {
            case gcm:
            case ccm:
            case ccm8:
            case mode_poly1305: {
                ret = decrypt_aead(session, dir, stream, size, pos, plaintext, aad, tag);
                break;
            }
            default: {
                ret = errorcode_t::not_supported;
                break;
            } break;
        }
    }
    __finally2 {}
    return ret;
}

return_t tls_protection::decrypt_aead(tls_session *session, tls_direction_t dir, const byte_t *stream, size_t size, size_t pos, binary_t &plaintext) {
    return_t ret = errorcode_t::success;
    __try2 {
        if (nullptr == session || nullptr == stream) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        size_t aadlen = get_header_size();

        binary_t aad;
        binary_append(aad, stream + pos, aadlen);

        binary_t tag;
        uint8 tagsize = get_tag_size();

        // ... aad(aadlen) encdata tag(tagsize)
        //     \_ pos
        binary_append(tag, stream + (pos + aadlen) + size - tagsize, tagsize);

        ret = decrypt_aead(session, dir, stream, size - tagsize, pos + aadlen, plaintext, aad, tag);
    }
    __finally2 {
        // do nothing
    }
    return ret;
}

return_t tls_protection::decrypt_aead(tls_session *session, tls_direction_t dir, const byte_t *stream, size_t size, size_t pos, binary_t &plaintext,
                                      const binary_t &aad, const binary_t &tag) {
    return_t ret = errorcode_t::success;
    __try2 {
        if (nullptr == session || nullptr == stream) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        auto session_type = session->get_type();
        auto cipher = crypt_alg_unknown;
        auto mode = mode_unknown;

        auto record_version = get_lagacy_version();

        crypt_context_t *handle = nullptr;
        openssl_crypt crypt;

        ret = get_cipher_info(session, cipher, mode);
        if (errorcode_t::success != ret) {
            __leave2;
        }

        tls_secret_t secret_key;
        tls_secret_t secret_iv;
        get_aead_key(session, dir, secret_key, secret_iv);

        uint64 record_no = 0;
        record_no = session->get_recordno(dir, true);

        auto const &key = get_item(secret_key);
        auto const &iv = get_item(secret_iv);
        binary_t nonce = iv;
        build_iv(session, secret_iv, nonce, record_no);
        ret = crypt.decrypt(cipher, mode, key, nonce, stream + pos, size, plaintext, aad, tag);

        if (istraceable()) {
            basic_stream dbs;
            dbs.printf("> decrypt\n");
            dbs.printf(" > key[%08x] %s\n", secret_key, base16_encode(key).c_str());
            dbs.printf(" > iv [%08x] %s\n", secret_iv, base16_encode(iv).c_str());
            dbs.printf(" > record no %i\n", record_no);
            dbs.printf(" > nonce %s\n", base16_encode(nonce).c_str());
            dbs.printf(" > aad %s\n", base16_encode(aad).c_str());
            dbs.printf(" > tag %s\n", base16_encode(tag).c_str());
            dbs.printf(" > ciphertext\n");
            dump_memory(stream + pos, size, &dbs, 16, 3, 0x0, dump_notrunc);
            dbs.printf(" > plaintext\n");
            dump_memory(plaintext, &dbs, 16, 3, 0x0, dump_notrunc);

            trace_debug_event(category_tls1, tls_event_read, &dbs);
        }
    }
    __finally2 {
        // do nothing
    }
    return ret;
}

return_t tls_protection::decrypt_cbc_hmac(tls_session *session, tls_direction_t dir, const byte_t *stream, size_t size, size_t pos, binary_t &plaintext) {
    return_t ret = errorcode_t::success;
    __try2 {
        if (nullptr == session || nullptr == stream) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        // stream = unprotected(content header + iv) + protected(ciphertext)
        // ciphertext = enc(plaintext + tag)

        crypto_advisor *advisor = crypto_advisor::get_instance();
        tls_advisor *tlsadvisor = tls_advisor::get_instance();
        const tls_cipher_suite_t *hint = tlsadvisor->hintof_cipher_suite(get_cipher_suite());
        if (nullptr == hint) {
            ret = errorcode_t::not_supported;
            __leave2;
        }
        auto hint_cipher = advisor->hintof_blockcipher(hint->cipher);
        if (nullptr == hint_cipher) {
            ret = errorcode_t::not_supported;
            __leave2;
        }
        auto ivsize = sizeof_iv(hint_cipher);
        size_t content_header_size = get_header_size();
        binary_t iv;
        binary_append(iv, stream + content_header_size, ivsize);
        size_t bpos = content_header_size + ivsize;

        openssl_crypt crypt;

        tls_secret_t secret_key;
        tls_secret_t secret_mac_key;
        get_cbc_hmac_key(session, dir, secret_key, secret_mac_key);

        uint64 record_no = 0;
        record_no = session->get_recordno(dir, true);

        const binary_t &enckey = get_item(secret_key);
        auto enc_alg = hint->cipher;
        auto hmac_alg = hint->mac;  // do not promote insecure algorithm
        const binary_t &mackey = get_item(secret_mac_key);

        binary_t verifydata;
        binary_t aad;
        binary_t tag;
        binary_append(aad, uint64(record_no), hton64);  // sequence
        binary_append(aad, stream, 3);                  // rechdr (content_type, version)
        size_t plainsize = 0;

        // plaintext || tag
        //          \- plainsize
        ret = crypt.cbc_hmac_tls_decrypt(enc_alg, hmac_alg, enckey, mackey, iv, aad, stream + bpos, size - bpos, plaintext, tag);

        if (istraceable()) {
            basic_stream dbs;
            dbs.printf("> decrypt\n");
            dbs.printf(" > aad %s\n", base16_encode(aad).c_str());
            dbs.printf(" > enc %s\n", advisor->nameof_cipher(enc_alg, cbc));
            dbs.printf(" > enckey[%08x] %s\n", secret_key, base16_encode(enckey).c_str());
            dbs.printf(" > iv %s\n", base16_encode(iv).c_str());
            dbs.printf(" > mac %s\n", advisor->nameof_md(hmac_alg));
            dbs.printf(" > mackey[%08x] %s\n", secret_mac_key, base16_encode(mackey).c_str());
            dbs.printf(" > record no %i\n", record_no);
            dbs.printf(" > ciphertext\n");
            dump_memory(stream + bpos, size - bpos, &dbs, 16, 3, 0x0, dump_notrunc);
            dbs.printf(" > plaintext 0x%x(%i)\n", plainsize, plainsize);
            dump_memory(plaintext, &dbs, 16, 3, 0x0, dump_notrunc);

            trace_debug_event(category_tls1, tls_event_read, &dbs);
        }
    }
    __finally2 {
        // do nothing
    }
    return ret;
}

return_t tls_protection::protection_mask(tls_session *session, tls_direction_t dir, const byte_t *stream, size_t size, binary_t &mask, size_t masklen) {
    return_t ret = errorcode_t::success;
    cipher_encrypt *cipher = nullptr;

    __try2 {
        if (nullptr == session || nullptr == stream) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        auto session_type = session->get_type();
        auto &protection = session->get_tls_protection();

        crypto_advisor *advisor = crypto_advisor::get_instance();
        auto hint = advisor->hintof_blockcipher(aes128);
        uint16 blocksize = sizeof_block(hint);
        if (masklen > blocksize) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        uint16 recno = 0;
        uint16 rec_enc = 0;
        tls_secret_t secret_key;
        auto hsstatus = session->get_session_info(dir).get_status();
        {
            cipher_encrypt_builder builder;
            cipher = builder.set(aes128, ecb).build();
            if (cipher) {
                if (is_kindof_dtls()) {
                    if (from_server == dir) {
                        if (tls_hs_finished == hsstatus) {
                            secret_key = tls_secret_application_server_sn_key;
                        } else {
                            secret_key = tls_secret_handshake_server_sn_key;
                        }
                    } else {
                        if (tls_hs_finished == hsstatus) {
                            secret_key = tls_secret_application_client_sn_key;
                        } else {
                            secret_key = tls_secret_handshake_client_sn_key;
                        }
                    }
                } else if (session_quic == session_type) {
                    if (from_server == dir) {
                        secret_key = tls_context_quic_initial_server_hp;
                    } else {
                        secret_key = tls_context_quic_initial_client_hp;
                    }
                } else {
                    ret = errorcode_t::not_supported;
                    __leave2;
                }

                auto const &key = get_item(secret_key);
                ret = cipher->encrypt(key, binary_t(), stream, (size > blocksize) ? blocksize : size, mask);
            }
        }

        mask.resize(masklen);
    }
    __finally2 {
        if (cipher) {
            cipher->release();
        }
    }

    return ret;
}

}  // namespace net
}  // namespace hotplace
