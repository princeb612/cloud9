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

namespace hotplace {
namespace crypto {

const openssl_evp_cipher_method_t evp_cipher_methods[] = {
    {
        crypt_algorithm_t::aes128,
        crypt_mode_t::cbc,
        "aes-128-cbc",
    },
    {
        crypt_algorithm_t::aes128,
        crypt_mode_t::ccm,
        "aes-128-ccm",
    },
    {
        crypt_algorithm_t::aes128,
        crypt_mode_t::cfb,
        "aes-128-cfb",
    },
    {
        crypt_algorithm_t::aes128,
        crypt_mode_t::cfb1,
        "aes-128-cfb1",
    },
    {
        crypt_algorithm_t::aes128,
        crypt_mode_t::cfb8,
        "aes-128-cfb8",
    },
    {
        crypt_algorithm_t::aes128,
        crypt_mode_t::ctr,
        "aes-128-ctr",
    },
    {
        crypt_algorithm_t::aes128,
        crypt_mode_t::ecb,
        "aes-128-ecb",
    },
    {
        crypt_algorithm_t::aes128,
        crypt_mode_t::gcm,
        "aes-128-gcm",
    },
    {
        crypt_algorithm_t::aes128,
        crypt_mode_t::ofb,
        "aes-128-ofb",
    },
    {
        crypt_algorithm_t::aes128,
        crypt_mode_t::wrap,
        "aes-128-wrap",
    },

    {
        crypt_algorithm_t::aes192,
        crypt_mode_t::cbc,
        "aes-192-cbc",
    },
    {
        crypt_algorithm_t::aes192,
        crypt_mode_t::ccm,
        "aes-192-ccm",
    },
    {
        crypt_algorithm_t::aes192,
        crypt_mode_t::cfb,
        "aes-192-cfb",
    },
    {
        crypt_algorithm_t::aes192,
        crypt_mode_t::cfb1,
        "aes-192-cfb1",
    },
    {
        crypt_algorithm_t::aes192,
        crypt_mode_t::cfb8,
        "aes-192-cfb8",
    },
    {
        crypt_algorithm_t::aes192,
        crypt_mode_t::ctr,
        "aes-192-ctr",
    },
    {
        crypt_algorithm_t::aes192,
        crypt_mode_t::ecb,
        "aes-192-ecb",
    },
    {
        crypt_algorithm_t::aes192,
        crypt_mode_t::gcm,
        "aes-192-gcm",
    },
    {
        crypt_algorithm_t::aes192,
        crypt_mode_t::ofb,
        "aes-192-ofb",
    },
    {
        crypt_algorithm_t::aes192,
        crypt_mode_t::wrap,
        "aes-192-wrap",
    },

    {
        crypt_algorithm_t::aes256,
        crypt_mode_t::cbc,
        "aes-256-cbc",
    },
    {
        crypt_algorithm_t::aes256,
        crypt_mode_t::ccm,
        "aes-256-ccm",
    },
    {
        crypt_algorithm_t::aes256,
        crypt_mode_t::cfb,
        "aes-256-cfb",
    },
    {
        crypt_algorithm_t::aes256,
        crypt_mode_t::cfb1,
        "aes-256-cfb1",
    },
    {
        crypt_algorithm_t::aes256,
        crypt_mode_t::cfb8,
        "aes-256-cfb8",
    },
    {
        crypt_algorithm_t::aes256,
        crypt_mode_t::ctr,
        "aes-256-ctr",
    },
    {
        crypt_algorithm_t::aes256,
        crypt_mode_t::ecb,
        "aes-256-ecb",
    },
    {
        crypt_algorithm_t::aes256,
        crypt_mode_t::gcm,
        "aes-256-gcm",
    },
    {
        crypt_algorithm_t::aes256,
        crypt_mode_t::ofb,
        "aes-256-ofb",
    },
    {
        crypt_algorithm_t::aes256,
        crypt_mode_t::wrap,
        "aes-256-wrap",
    },

    {
        crypt_algorithm_t::aria128,
        crypt_mode_t::cbc,
        "aria-128-cbc",
    },
    {
        crypt_algorithm_t::aria128,
        crypt_mode_t::ccm,
        "aria-128-ccm",
    },
    {
        crypt_algorithm_t::aria128,
        crypt_mode_t::cfb,
        "aria-128-cfb",
    },
    {
        crypt_algorithm_t::aria128,
        crypt_mode_t::cfb1,
        "aria-128-cfb1",
    },
    {
        crypt_algorithm_t::aria128,
        crypt_mode_t::cfb8,
        "aria-128-cfb8",
    },
    {
        crypt_algorithm_t::aria128,
        crypt_mode_t::ctr,
        "aria-128-ctr",
    },
    {
        crypt_algorithm_t::aria128,
        crypt_mode_t::ecb,
        "aria-128-ecb",
    },
    {
        crypt_algorithm_t::aria128,
        crypt_mode_t::gcm,
        "aria-128-gcm",
    },
    {
        crypt_algorithm_t::aria128,
        crypt_mode_t::ofb,
        "aria-128-ofb",
    },

    {
        crypt_algorithm_t::aria192,
        crypt_mode_t::cbc,
        "aria-192-cbc",
    },
    {
        crypt_algorithm_t::aria192,
        crypt_mode_t::ccm,
        "aria-192-ccm",
    },
    {
        crypt_algorithm_t::aria192,
        crypt_mode_t::cfb,
        "aria-192-cfb",
    },
    {
        crypt_algorithm_t::aria192,
        crypt_mode_t::cfb1,
        "aria-192-cfb1",
    },
    {
        crypt_algorithm_t::aria192,
        crypt_mode_t::cfb8,
        "aria-192-cfb8",
    },
    {
        crypt_algorithm_t::aria192,
        crypt_mode_t::ctr,
        "aria-192-ctr",
    },
    {
        crypt_algorithm_t::aria192,
        crypt_mode_t::ecb,
        "aria-192-ecb",
    },
    {
        crypt_algorithm_t::aria192,
        crypt_mode_t::gcm,
        "aria-192-gcm",
    },
    {
        crypt_algorithm_t::aria192,
        crypt_mode_t::ofb,
        "aria-192-ofb",
    },

    {
        crypt_algorithm_t::aria256,
        crypt_mode_t::cbc,
        "aria-256-cbc",
    },
    {
        crypt_algorithm_t::aria256,
        crypt_mode_t::ccm,
        "aria-256-ccm",
    },
    {
        crypt_algorithm_t::aria256,
        crypt_mode_t::cfb,
        "aria-256-cfb",
    },
    {
        crypt_algorithm_t::aria256,
        crypt_mode_t::cfb1,
        "aria-256-cfb1",
    },
    {
        crypt_algorithm_t::aria256,
        crypt_mode_t::cfb8,
        "aria-256-cfb8",
    },
    {
        crypt_algorithm_t::aria256,
        crypt_mode_t::ctr,
        "aria-256-ctr",
    },
    {
        crypt_algorithm_t::aria256,
        crypt_mode_t::ecb,
        "aria-256-ecb",
    },
    {
        crypt_algorithm_t::aria256,
        crypt_mode_t::gcm,
        "aria-256-gcm",
    },
    {
        crypt_algorithm_t::aria256,
        crypt_mode_t::ofb,
        "aria-256-ofb",
    },

    {
        crypt_algorithm_t::blowfish,
        crypt_mode_t::cbc,
        "bf-cbc",
    },
    {
        crypt_algorithm_t::blowfish,
        crypt_mode_t::cfb,
        "bf-cfb",
    },
    //{ crypt_algorithm_t::blowfish, crypt_mode_t::ctr, "bf-ctr", },
    {
        crypt_algorithm_t::blowfish,
        crypt_mode_t::ecb,
        "bf-ecb",
    },
    {
        crypt_algorithm_t::blowfish,
        crypt_mode_t::ofb,
        "bf-ofb",
    },

    {
        crypt_algorithm_t::camellia128,
        crypt_mode_t::cbc,
        "camellia-128-cbc",
    },
    //{ crypt_algorithm_t::camellia128, crypt_mode_t::ccm, "camellia-128-ccm", },
    {
        crypt_algorithm_t::camellia128,
        crypt_mode_t::cfb,
        "camellia-128-cfb",
    },
    {
        crypt_algorithm_t::camellia128,
        crypt_mode_t::cfb1,
        "camellia-128-cfb1",
    },
    {
        crypt_algorithm_t::camellia128,
        crypt_mode_t::cfb8,
        "camellia-128-cfb8",
    },
    {
        crypt_algorithm_t::camellia128,
        crypt_mode_t::ctr,
        "camellia-128-ctr",
    },
    {
        crypt_algorithm_t::camellia128,
        crypt_mode_t::ecb,
        "camellia-128-ecb",
    },
    //{ crypt_algorithm_t::camellia128, crypt_mode_t::gcm, "camellia-128-gcm", },
    {
        crypt_algorithm_t::camellia128,
        crypt_mode_t::ofb,
        "camellia-128-ofb",
    },

    {
        crypt_algorithm_t::camellia192,
        crypt_mode_t::cbc,
        "camellia-192-cbc",
    },
    //{ crypt_algorithm_t::camellia192, crypt_mode_t::ccm, "camellia-192-ccm", },
    {
        crypt_algorithm_t::camellia192,
        crypt_mode_t::cfb,
        "camellia-192-cfb",
    },
    {
        crypt_algorithm_t::camellia192,
        crypt_mode_t::cfb1,
        "camellia-192-cfb1",
    },
    {
        crypt_algorithm_t::camellia192,
        crypt_mode_t::cfb8,
        "camellia-192-cfb8",
    },
    {
        crypt_algorithm_t::camellia192,
        crypt_mode_t::ctr,
        "camellia-192-ctr",
    },
    {
        crypt_algorithm_t::camellia192,
        crypt_mode_t::ecb,
        "camellia-192-ecb",
    },
    //{ crypt_algorithm_t::camellia192, crypt_mode_t::gcm, "camellia-192-gcm", },
    {
        crypt_algorithm_t::camellia192,
        crypt_mode_t::ofb,
        "camellia-192-ofb",
    },

    {
        crypt_algorithm_t::camellia256,
        crypt_mode_t::cbc,
        "camellia-256-cbc",
    },
    //{ crypt_algorithm_t::camellia256, crypt_mode_t::ccm, "camellia-256-ccm", },
    {
        crypt_algorithm_t::camellia256,
        crypt_mode_t::cfb,
        "camellia-256-cfb",
    },
    {
        crypt_algorithm_t::camellia256,
        crypt_mode_t::cfb1,
        "camellia-256-cfb1",
    },
    {
        crypt_algorithm_t::camellia256,
        crypt_mode_t::cfb8,
        "camellia-256-cfb8",
    },
    {
        crypt_algorithm_t::camellia256,
        crypt_mode_t::ctr,
        "camellia-256-ctr",
    },
    {
        crypt_algorithm_t::camellia256,
        crypt_mode_t::ecb,
        "camellia-256-ecb",
    },
    //{ crypt_algorithm_t::camellia256, crypt_mode_t::gcm, "camellia-256-gcm", },
    {
        crypt_algorithm_t::camellia256,
        crypt_mode_t::ofb,
        "camellia-256-ofb",
    },

    {
        crypt_algorithm_t::cast,
        crypt_mode_t::cbc,
        "cast5-cbc",
    },
    {
        crypt_algorithm_t::cast,
        crypt_mode_t::cfb,
        "cast5-cfb",
    },
    {
        crypt_algorithm_t::cast,
        crypt_mode_t::ecb,
        "cast5-ecb",
    },
    {
        crypt_algorithm_t::cast,
        crypt_mode_t::ofb,
        "cast5-ofb",
    },

    {
        crypt_algorithm_t::idea,
        crypt_mode_t::cbc,
        "idea-cbc",
    },
    {
        crypt_algorithm_t::idea,
        crypt_mode_t::cfb,
        "idea-cfb",
    },
    {
        crypt_algorithm_t::idea,
        crypt_mode_t::ecb,
        "idea-ecb",
    },
    {
        crypt_algorithm_t::idea,
        crypt_mode_t::ofb,
        "idea-ofb",
    },

    {
        crypt_algorithm_t::rc2,
        crypt_mode_t::cbc,
        "rc2-cbc",
    },
    {
        crypt_algorithm_t::rc2,
        crypt_mode_t::cfb,
        "rc2-cfb",
    },
    {
        crypt_algorithm_t::rc2,
        crypt_mode_t::ecb,
        "rc2-ecb",
    },
    {
        crypt_algorithm_t::rc2,
        crypt_mode_t::ofb,
        "rc2-ofb",
    },

    {
        crypt_algorithm_t::rc5,
        crypt_mode_t::cbc,
        "rc5-cbc",
    },
    {
        crypt_algorithm_t::rc5,
        crypt_mode_t::cfb,
        "rc5-cfb",
    },
    {
        crypt_algorithm_t::rc5,
        crypt_mode_t::ecb,
        "rc5-ecb",
    },
    {
        crypt_algorithm_t::rc5,
        crypt_mode_t::ofb,
        "rc5-ofb",
    },

    {
        crypt_algorithm_t::sm4,
        crypt_mode_t::cbc,
        "sm4-cbc",
    },
    {
        crypt_algorithm_t::sm4,
        crypt_mode_t::cfb,
        "sm4-cfb",
    },
    {
        crypt_algorithm_t::sm4,
        crypt_mode_t::ecb,
        "sm4-ecb",
    },
    {
        crypt_algorithm_t::sm4,
        crypt_mode_t::ofb,
        "sm4-ofb",
    },
    {
        crypt_algorithm_t::sm4,
        crypt_mode_t::ctr,
        "sm4-ctr",
    },

    {
        crypt_algorithm_t::seed,
        crypt_mode_t::cbc,
        "seed-cbc",
    },
    {
        crypt_algorithm_t::seed,
        crypt_mode_t::cfb,
        "seed-cfb",
    },
    {
        crypt_algorithm_t::seed,
        crypt_mode_t::ecb,
        "seed-ecb",
    },
    {
        crypt_algorithm_t::seed,
        crypt_mode_t::ofb,
        "seed-ofb",
    },

    {
        crypt_algorithm_t::rc4,
        crypt_mode_t::stream_cipher,
        "rc4",
    },
    {
        crypt_algorithm_t::chacha20,
        crypt_mode_t::stream_cipher,
        "chacha20",
    },
    {
        crypt_algorithm_t::chacha20,
        crypt_mode_t::stream_aead,
        "chacha20-poly1305",
    },
};

const size_t sizeof_evp_cipher_methods = RTL_NUMBER_OF(evp_cipher_methods);

}  // namespace crypto
}  // namespace hotplace
