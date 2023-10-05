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
#include <hotplace/sdk/crypto/basic/crypto_key.hpp>
#include <hotplace/sdk/crypto/basic/openssl_sdk.hpp>
#include <hotplace/sdk/io/string/string.hpp>

namespace hotplace {
using namespace io;
namespace crypto {

return_t nidof_evp_pkey (EVP_PKEY* pkey, uint32& nid)
{
    return_t ret = errorcode_t::success;

    __try2
    {
        nid = 0;

        if (nullptr == pkey) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        nid = EVP_PKEY_id ((EVP_PKEY *) pkey);
        if (EVP_PKEY_EC == nid) {
            EC_KEY* ec = EVP_PKEY_get1_EC_KEY ((EVP_PKEY*) pkey);
            if (ec) {
                const EC_GROUP* group = EC_KEY_get0_group (ec);
                nid = EC_GROUP_get_curve_name (group);
                EC_KEY_free (ec);
            }
        }
        if (0 == nid) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
    }
    __finally2
    {
        // do nothing
    }
    return ret;
}

bool kindof_ecc (EVP_PKEY* pkey)
{
    bool test = false;

    if (pkey) {
        int type = EVP_PKEY_id (pkey);
        test = ((EVP_PKEY_EC == type) || (EVP_PKEY_ED25519 == type) || (EVP_PKEY_ED448 == type)
                || (EVP_PKEY_X25519 == type) || (EVP_PKEY_X448 == type));
    }
    return test;
}

bool kindof_ecc (crypto_key_t type)
{
    return (crypto_key_t::kty_ec == type) || (crypto_key_t::kty_okp == type);
}

const char* nameof_key_type (crypto_key_t type)
{
    const char* name = "";

    if (crypto_key_t::kty_hmac == type) {
        name = "oct";
    } else if (crypto_key_t::kty_rsa == type) {
        name = "RSA";
    } else if (crypto_key_t::kty_ec == type) {
        name = "EC";
    } else if (crypto_key_t::kty_okp == type) {
        name = "OKP";
    }
    return name;
}

crypto_key_t typeof_crypto_key (EVP_PKEY* pkey)
{
    crypto_key_t kty = crypto_key_t::kty_unknown;
    int type = EVP_PKEY_id ((EVP_PKEY *) pkey);

    switch (type) {
        case EVP_PKEY_HMAC:
            kty = crypto_key_t::kty_hmac;
            break;
        case EVP_PKEY_RSA:
            kty = crypto_key_t::kty_rsa;
            break;
        case EVP_PKEY_EC:
            kty = crypto_key_t::kty_ec;
            break;
        case EVP_PKEY_X25519:
        case EVP_PKEY_X448:
        case EVP_PKEY_ED25519:
        case EVP_PKEY_ED448:
            kty = crypto_key_t::kty_okp;
            break;
        default:
            break;
    }
    return kty;
}

return_t is_private_key (EVP_PKEY* pkey, bool& result)
{
    return_t ret = errorcode_t::success;

    __try2
    {
        result = false;

        if (nullptr == pkey) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        EVP_PKEY* key = (EVP_PKEY*) (pkey);
        int type = EVP_PKEY_id (key);

        switch (type) {
            case EVP_PKEY_HMAC:
                result = true;
                break;
            case EVP_PKEY_RSA:
                if (nullptr != RSA_get0_d (EVP_PKEY_get0_RSA (key))) {
                    result = true;
                }
                break;
            case EVP_PKEY_EC:
            {
                const BIGNUM* bn = EC_KEY_get0_private_key (EVP_PKEY_get0_EC_KEY (key));
                if (nullptr != bn) {
                    result = true;
                }
                break;
            }
            case EVP_PKEY_X25519:
            case EVP_PKEY_X448:
            case EVP_PKEY_ED25519:
            case EVP_PKEY_ED448:
            {
                binary_t bin_d;
                size_t len_d = 256;
                bin_d.resize (len_d);
                int check = EVP_PKEY_get_raw_private_key ((EVP_PKEY*) pkey, &bin_d[0], &len_d);
                bin_d.resize (len_d);
                if (1 == check) {
                    result = true;
                }
                break;
            }
            default:
                ret = errorcode_t::not_supported;
                break;
        }
    }
    __finally2
    {
        // do nothing
    }
    return ret;
}

}
}  // namespace
