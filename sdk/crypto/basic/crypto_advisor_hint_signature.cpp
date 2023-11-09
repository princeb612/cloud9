/* vim: set tabstop=4 shiftwidth=4 softtabstop=4 expandtab smarttab : */
/**
 * @file {file}
 * @author Soo Han, Kim (princeb612.kr@gmail.com)
 * @desc
 *
 * Revision History
 * Date         Name                Description
 */

#include <iostream>
#include <sdk/crypto/basic/crypto_advisor.hpp>
#include <sdk/io/system/sdk.hpp>

namespace hotplace {
namespace crypto {

const hint_signature_t hint_signatures[] = {
    {
        crypt_sig_t::sig_hs256,
        jws_t::jws_hs256,
        jws_group_t::jws_group_hmac,
        crypto_kty_t::kty_oct,
        "HS256",
        hash_algorithm_t::sha2_256,
        1,
        NID_hmac,
    },
    {
        crypt_sig_t::sig_hs384,
        jws_t::jws_hs384,
        jws_group_t::jws_group_hmac,
        crypto_kty_t::kty_oct,
        "HS384",
        hash_algorithm_t::sha2_384,
        1,
        NID_hmac,
    },
    {
        crypt_sig_t::sig_hs512,
        jws_t::jws_hs512,
        jws_group_t::jws_group_hmac,
        crypto_kty_t::kty_oct,
        "HS512",
        hash_algorithm_t::sha2_512,
        1,
        NID_hmac,
    },
    {
        crypt_sig_t::sig_rs256,
        jws_t::jws_rs256,
        jws_group_t::jws_group_rsassa_pkcs15,
        crypto_kty_t::kty_rsa,
        "RS256",
        hash_algorithm_t::sha2_256,
        1,
        NID_rsaEncryption,
    },
    {
        crypt_sig_t::sig_rs384,
        jws_t::jws_rs384,
        jws_group_t::jws_group_rsassa_pkcs15,
        crypto_kty_t::kty_rsa,
        "RS384",
        hash_algorithm_t::sha2_384,
        1,
        NID_rsaEncryption,
    },
    {
        crypt_sig_t::sig_rs512,
        jws_t::jws_rs512,
        jws_group_t::jws_group_rsassa_pkcs15,
        crypto_kty_t::kty_rsa,
        "RS512",
        hash_algorithm_t::sha2_512,
        1,
        NID_rsaEncryption,
    },
    {
        crypt_sig_t::sig_rs1,
        jws_t::jws_unknown,
        jws_group_t::jws_group_rsassa_pkcs15,
        crypto_kty_t::kty_rsa,
        nullptr,
        hash_algorithm_t::sha1,
        1,
        NID_rsaEncryption,
    },
    {
        crypt_sig_t::sig_es256,
        jws_t::jws_es256,
        jws_group_t::jws_group_ecdsa,
        crypto_kty_t::kty_ec,
        "ES256",
        hash_algorithm_t::sha2_256,
        1,
        NID_X9_62_prime256v1,
    },
    {
        crypt_sig_t::sig_es384,
        jws_t::jws_es384,
        jws_group_t::jws_group_ecdsa,
        crypto_kty_t::kty_ec,
        "ES384",
        hash_algorithm_t::sha2_384,
        1,
        NID_secp384r1,
    },
    {
        crypt_sig_t::sig_es512,
        jws_t::jws_es512,
        jws_group_t::jws_group_ecdsa,
        crypto_kty_t::kty_ec,
        "ES512",
        hash_algorithm_t::sha2_512,
        1,
        NID_secp521r1,
    },
    {
        crypt_sig_t::sig_ps256,
        jws_t::jws_ps256,
        jws_group_t::jws_group_rsassa_pss,
        crypto_kty_t::kty_rsa,
        "PS256",
        hash_algorithm_t::sha2_256,
        1,
        NID_rsaEncryption,
    },
    {
        crypt_sig_t::sig_ps384,
        jws_t::jws_ps384,
        jws_group_t::jws_group_rsassa_pss,
        crypto_kty_t::kty_rsa,
        "PS384",
        hash_algorithm_t::sha2_384,
        1,
        NID_rsaEncryption,
    },
    {
        crypt_sig_t::sig_ps512,
        jws_t::jws_ps512,
        jws_group_t::jws_group_rsassa_pss,
        crypto_kty_t::kty_rsa,
        "PS512",
        hash_algorithm_t::sha2_512,
        1,
        NID_rsaEncryption,
    },
    {
        crypt_sig_t::sig_eddsa,
        jws_t::jws_eddsa,
        jws_group_t::jws_group_eddsa,
        crypto_kty_t::kty_okp,
        "EdDSA",
        hash_algorithm_t::hash_alg_unknown,
        2,
        NID_ED25519,
        NID_ED448,
    },
};

const size_t sizeof_hint_signatures = RTL_NUMBER_OF(hint_signatures);

}  // namespace crypto
}  // namespace hotplace
