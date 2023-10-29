/* vim: set tabstop=4 shiftwidth=4 softtabstop=4 expandtab smarttab : */
/**
 * @file {file}
 * @author Soo Han, Kim (princeb612.kr@gmail.com)
 * @desc
 *  RFC 2104 HMAC: Keyed-Hashing for Message Authentication
 *  RFC 4493 The AES-CMAC Algorithm
 *
 * Revision History
 * Date         Name                Description
 */

#include <sdk/crypto/basic/crypto_advisor.hpp>
#include <sdk/crypto/basic/openssl_hash.hpp>
#include <sdk/crypto/basic/openssl_sdk.hpp>

namespace hotplace {
namespace crypto {

return_t openssl_hash::hmac(const char* alg, binary_t const& key, binary_t const& input, binary_t& output) {
    return_t ret = errorcode_t::success;
    hash_context_t* handle = nullptr;

    __try2 {
        ret = open_byname(&handle, alg, &key[0], key.size());
        if (errorcode_t::success != ret) {
            __leave2;
        }
        init(handle);
        update(handle, &input[0], input.size());
        finalize(handle, output);
    }
    __finally2 { close(handle); }

    return ret;
}

return_t openssl_hash::hmac(hash_algorithm_t alg, binary_t const& key, binary_t const& input, binary_t& output) {
    return_t ret = errorcode_t::success;
    hash_context_t* handle = nullptr;

    __try2 {
        ret = open(&handle, alg, &key[0], key.size());
        if (errorcode_t::success != ret) {
            __leave2;
        }
        init(handle);
        update(handle, &input[0], input.size());
        finalize(handle, output);
    }
    __finally2 { close(handle); }

    return ret;
}

return_t openssl_hash::cmac(const char* alg, binary_t const& key, binary_t const& input, binary_t& output) {
    return_t ret = errorcode_t::success;
    hash_context_t* handle = nullptr;

    __try2 {
        ret = open_byname(&handle, alg, &key[0], key.size());
        if (errorcode_t::success != ret) {
            __leave2;
        }
        init(handle);
        update(handle, &input[0], input.size());
        finalize(handle, output);
    }
    __finally2 { close(handle); }

    return ret;
}

return_t openssl_hash::cmac(crypt_algorithm_t alg, crypt_mode_t mode, binary_t const& key, binary_t const& input, binary_t& output) {
    return_t ret = errorcode_t::success;
    hash_context_t* handle = nullptr;

    __try2 {
        ret = open(&handle, alg, mode, &key[0], key.size());
        if (errorcode_t::success != ret) {
            __leave2;
        }
        init(handle);
        update(handle, &input[0], input.size());
        finalize(handle, output);
    }
    __finally2 { close(handle); }

    return ret;
}

}  // namespace crypto
}  // namespace hotplace
