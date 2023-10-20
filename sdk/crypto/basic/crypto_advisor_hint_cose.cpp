/* vim: set tabstop=4 shiftwidth=4 softtabstop=4 expandtab smarttab : */
/**
 * @file {file}
 * @author Soo Han, Kim (princeb612.kr@gmail.com)
 * @desc
 *
 * Revision History
 * Date         Name                Description
 */

#include <hotplace/sdk/crypto/basic/crypto_advisor.hpp>
#include <hotplace/sdk/io/system/sdk.hpp>
#include <iostream>

namespace hotplace {
namespace crypto {

const hint_cose_algorithm_t hint_cose_algorithms[] = {
    {
        cose_alg_t::cose_aes128kw,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_aeskw,
    },
    {
        cose_alg_t::cose_aes192kw,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_aeskw,
    },
    {
        cose_alg_t::cose_aes256kw,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_aeskw,
    },
    {
        cose_alg_t::cose_direct,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_direct,
    },
    {
        cose_alg_t::cose_es256,
        crypto_kty_t::kty_ec,
        cose_group_t::cose_group_ecdsa,
    },
    {
        cose_alg_t::cose_es384,
        crypto_kty_t::kty_ec,
        cose_group_t::cose_group_ecdsa,
    },
    {
        cose_alg_t::cose_es512,
        crypto_kty_t::kty_ec,
        cose_group_t::cose_group_ecdsa,
    },
    {
        cose_alg_t::cose_eddsa,
        crypto_kty_t::kty_okp,
        cose_group_t::cose_group_eddsa,
    },
    {
        cose_alg_t::cose_hkdf_hmac_sha256,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_hkdf_hmac,
        {
            "sha256",
            16,
        },
    },
    {
        cose_alg_t::cose_hkdf_hmac_sha512,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_hkdf_hmac,
        {
            "sha512",
            32,
        },
    },
    {
        cose_alg_t::cose_hkdf_aescmac128,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_hkdf_aescmac,
        {
            "aes-128-cbc",
            16,
        },
        {
            "aes-128-wrap",
        },
    },
    {
        cose_alg_t::cose_hkdf_aescmac256,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_hkdf_aescmac,
        {
            "aes-256-cbc",
            32,
        },
        {
            "aes-256-wrap",
        },
    },
    {
        cose_alg_t::cose_sha1,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_sha,
    },
    {
        cose_alg_t::cose_sha256_64,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_sha,
    },
    {
        cose_alg_t::cose_sha256,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_sha,
    },
    {
        cose_alg_t::cose_sha512_256,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_sha,
    },
    {
        cose_alg_t::cose_sha384,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_sha,
    },
    {
        cose_alg_t::cose_sha512,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_sha,
    },
    {
        cose_alg_t::cose_shake128,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_sha,
    },
    {
        cose_alg_t::cose_shake256,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_sha,
    },
    {
        cose_alg_t::cose_ecdhes_hkdf_256,
        crypto_kty_t::kty_ec,
        cose_group_t::cose_group_ecdhes_hkdf,
        {
            "sha256",
            16,
        },
    },
    {
        cose_alg_t::cose_ecdhes_hkdf_512,
        crypto_kty_t::kty_ec,
        cose_group_t::cose_group_ecdhes_hkdf,
        {
            "sha512",
            32,
        },
    },
    {
        cose_alg_t::cose_ecdhss_hkdf_256,
        crypto_kty_t::kty_ec,
        cose_group_t::cose_group_ecdhss_hkdf,
        {
            "sha256",
            16,
        },
    },
    {
        cose_alg_t::cose_ecdhss_hkdf_512,
        crypto_kty_t::kty_ec,
        cose_group_t::cose_group_ecdhss_hkdf,
        {
            "sha512",
            32,
        },
    },
    {
        cose_alg_t::cose_ecdhes_a128kw,
        crypto_kty_t::kty_ec,
        cose_group_t::cose_group_ecdhes_aeskw,
        {
            "sha256",
            16,
        },
        {
            "aes-128-wrap",
        },
    },
    {
        cose_alg_t::cose_ecdhes_a192kw,
        crypto_kty_t::kty_ec,
        cose_group_t::cose_group_ecdhes_aeskw,
        {
            "sha384",
            24,
        },
        {
            "aes-192-wrap",
        },
    },
    {
        cose_alg_t::cose_ecdhes_a256kw,
        crypto_kty_t::kty_ec,
        cose_group_t::cose_group_ecdhes_aeskw,
        {
            "sha512",
            32,
        },
        {
            "aes-256-wrap",
        },
    },
    {
        cose_alg_t::cose_ecdhss_a128kw,
        crypto_kty_t::kty_ec,
        cose_group_t::cose_group_ecdhss_aeskw,
        {
            "sha256",
            16,
        },
        {
            "aes-128-wrap",
            16,
        },
    },
    {
        cose_alg_t::cose_ecdhss_a192kw,
        crypto_kty_t::kty_ec,
        cose_group_t::cose_group_ecdhss_aeskw,
        {
            "sha384",
            24,
        },
        {
            "aes-192-wrap",
            24,
        },
    },
    {
        cose_alg_t::cose_ecdhss_a256kw,
        crypto_kty_t::kty_ec,
        cose_group_t::cose_group_ecdhss_aeskw,
        {
            "sha512",
            32,
        },
        {
            "aes-256-wrap",
            32,
        },
    },
    {
        cose_alg_t::cose_ps256,
        crypto_kty_t::kty_rsa,
        cose_group_t::cose_group_rsassa_pss,
    },
    {
        cose_alg_t::cose_ps384,
        crypto_kty_t::kty_rsa,
        cose_group_t::cose_group_rsassa_pss,
    },
    {
        cose_alg_t::cose_ps512,
        crypto_kty_t::kty_rsa,
        cose_group_t::cose_group_rsassa_pss,
    },
    {
        cose_alg_t::cose_rsaes_oaep_sha1,
        crypto_kty_t::kty_rsa,
        cose_group_t::cose_group_rsa_oaep,
    },
    {
        cose_alg_t::cose_rsaes_oaep_sha256,
        crypto_kty_t::kty_rsa,
        cose_group_t::cose_group_rsa_oaep,
    },
    {
        cose_alg_t::cose_rsaes_oaep_sha512,
        crypto_kty_t::kty_rsa,
        cose_group_t::cose_group_rsa_oaep,
    },
    {
        cose_alg_t::cose_es256k,
        crypto_kty_t::kty_ec,
        cose_group_t::cose_group_ecdsa,
    },
    {
        cose_alg_t::cose_rs256,
        crypto_kty_t::kty_rsa,
        cose_group_t::cose_group_rsassa_pkcs15,
    },
    {
        cose_alg_t::cose_rs384,
        crypto_kty_t::kty_rsa,
        cose_group_t::cose_group_rsassa_pkcs15,
    },
    {
        cose_alg_t::cose_rs512,
        crypto_kty_t::kty_rsa,
        cose_group_t::cose_group_rsassa_pkcs15,
    },
    {
        cose_alg_t::cose_rs1,
        crypto_kty_t::kty_rsa,
        cose_group_t::cose_group_rsassa_pkcs15,
    },
    {
        cose_alg_t::cose_aes128gcm,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_aesgcm,
        {},
        {
            "aes-128-gcm",
            16,
            16,
        },
    },
    {
        cose_alg_t::cose_aes192gcm,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_aesgcm,
        {},
        {
            "aes-192-gcm",
            24,
            16,
        },
    },
    {
        cose_alg_t::cose_aes256gcm,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_aesgcm,
        {},
        {
            "aes-256-gcm",
            32,
            16,
        },
    },
    {
        cose_alg_t::cose_hs256_64,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_hmac,
    },
    {
        cose_alg_t::cose_hs256,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_hmac,
    },
    {
        cose_alg_t::cose_hs384,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_hmac,
    },
    {
        cose_alg_t::cose_hs512,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_hmac,
    },
    {
        cose_alg_t::cose_aesccm_16_64_128,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_aesccm,
        {},
        {
            "aes-128-ccm",
            16,
            8,
            2,
        },
    },
    {
        cose_alg_t::cose_aesccm_16_64_256,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_aesccm,
        {},
        {
            "aes-256-ccm",
            32,
            8,
            2,
        },
    },
    {
        cose_alg_t::cose_aesccm_64_64_128,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_aesccm,
        {},
        {
            "aes-128-ccm",
            16,
            8,
            8,
        },
    },
    {
        cose_alg_t::cose_aesccm_64_64_256,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_aesccm,
        {},
        {
            "aes-256-ccm",
            32,
            8,
            8,
        },
    },
    {
        cose_alg_t::cose_aesccm_16_128_128,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_aesccm,
        {},
        {
            "aes-128-ccm",
            16,
            16,
            2,
        },
    },
    {
        cose_alg_t::cose_aesccm_16_128_256,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_aesccm,
        {},
        {
            "aes-256-ccm",
            32,
            16,
            2,
        },
    },
    {
        cose_alg_t::cose_aesccm_64_128_128,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_aesccm,
        {},
        {
            "aes-128-ccm",
            16,
            16,
            8,
        },
    },
    {
        cose_alg_t::cose_aesccm_64_128_256,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_aesccm,
        {},
        {
            "aes-256-ccm",
            32,
            16,
            8,
        },
    },
    {
        cose_alg_t::cose_aescmac_128_64,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_aescmac,
    },
    {
        cose_alg_t::cose_aescmac_256_64,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_aescmac,
    },
    {
        cose_alg_t::cose_aescmac_128_128,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_aescmac,
    },
    {
        cose_alg_t::cose_aescmac_256_128,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_aescmac,
    },
    {
        cose_alg_t::cose_chacha20_poly1305,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_chacha20_poly1305,
        {},
        {
            "chacha20-poly1305",
            32,
            16,
            3,
        },
    },
    {
        cose_alg_t::cose_iv_generation,
        crypto_kty_t::kty_hmac,
        cose_group_t::cose_group_iv,
    },
};

const size_t sizeof_hint_cose_algorithms = RTL_NUMBER_OF(hint_cose_algorithms);

}  // namespace crypto
}  // namespace hotplace
