/* vim: set tabstop=4 shiftwidth=4 softtabstop=4 expandtab smarttab : */
/**
 * @file {file}
 * @author Soo Han, Kim (princeb612.kr@gmail.com)
 * @desc
 *
 * Revision History
 * Date         Name                Description
 */

#include <sdk/base/system/critical_section.hpp>
#include <sdk/crypto/basic/crypto_advisor.hpp>
#include <sdk/crypto/basic/openssl_hash.hpp>
#include <sdk/crypto/basic/openssl_prng.hpp>
#include <sdk/io/basic/zlib.hpp>
#include <sdk/io/string/string.hpp>
#include <sdk/net/basic/sdk.hpp>
#include <sdk/net/http/http.hpp>
#include <sdk/net/http/http_authentication_resolver.hpp>
#include <sdk/net/server/network_session.hpp>
#include <sdk/net/tls/tls.hpp>

namespace hotplace {
using namespace crypto;
using namespace io;
namespace net {

http_authentication_resolver::http_authentication_resolver() : _basic_resolver(nullptr) {}

bool http_authentication_resolver::resolve(http_authenticate_provider* provider, network_session* session, http_request* request, http_response* response) {
    return provider->try_auth(this, session, request, response);
}

http_authentication_resolver& http_authentication_resolver::basic_resolver(authenticate_handler_t resolver) {
    _basic_resolver = resolver;
    return *this;
}

bool http_authentication_resolver::basic_authenticate(http_authenticate_provider* provider, network_session* session, http_request* request,
                                                      http_response* response) {
    bool ret_value = false;
    __try2 {
        if (_basic_resolver) {
            ret_value = _basic_resolver(provider, session, request, response);
        } else {
            std::string challenge = provider->get_challenge(request);

            size_t pos = 0;
            tokenize(challenge, " ", pos);                           // Basic
            std::string credential = tokenize(challenge, " ", pos);  // base64(user:password)

            ret_value = get_basic_credentials().verify(provider, credential);
        }
    }
    __finally2 {
        // do nothing
    }
    return ret_value;
}

http_authentication_resolver& http_authentication_resolver::digest_resolver(authenticate_handler_t resolver) {
    _digest_resolver = resolver;
    return *this;
}

bool http_authentication_resolver::digest_authenticate(http_authenticate_provider* provider, network_session* session, http_request* request,
                                                       http_response* response) {
    bool ret_value = false;
    __try2 {
        if (_digest_resolver) {
            ret_value = _digest_resolver(provider, session, request, response);
        } else {
            return_t ret = errorcode_t::success;
            digest_access_authentication_provider* digest_provider = (digest_access_authentication_provider*)provider;
            key_value kv;

            ret = digest_provider->prepare_digest_access(session, request, response, kv);
            if (errorcode_t::success == ret) {
                // get username from kv.get("username"), and then read password (cache, in-memory db)
                // and then call provider->auth_digest_access

                get_digest_credentials().verify(provider, kv);

                ret = digest_provider->auth_digest_access(session, request, response, kv);
                if (errorcode_t::success == ret) {
                    ret_value = true;
                }
            }
        }
    }
    __finally2 {
        // do nothing
    }
    return ret_value;
}

http_authentication_resolver& http_authentication_resolver::bearer_resolver(authenticate_handler_t resolver) {
    _bearer_resolver = resolver;
    return *this;
}

bool http_authentication_resolver::bearer_authenticate(http_authenticate_provider* provider, network_session* session, http_request* request,
                                                       http_response* response) {
    bool ret_value = false;
    if (_bearer_resolver) {
        ret_value = _bearer_resolver(provider, session, request, response);
    } else {
        std::string challenge = provider->get_challenge(request);
        std::string token;

        if (0 == strncmp("Bearer", challenge.c_str(), 6)) {
            size_t pos = 6;
            token = tokenize(challenge, " ", pos);

            ret_value = get_bearer_credentials().verify(provider, token);
        }
    }
    return ret_value;
}

http_authentication_resolver& http_authentication_resolver::oauth2_resolver(authenticate_handler_t resolver) {
    _oauth2_resolver = resolver;
    return *this;
}

bool http_authentication_resolver::oauth2_authenticate(http_authenticate_provider* provider, network_session* session, http_request* request,
                                                       http_response* response) {
    bool ret_value = false;
    return ret_value;
}

basic_credentials& http_authentication_resolver::get_basic_credentials() { return _basic_credentials; }

digest_credentials& http_authentication_resolver::get_digest_credentials() { return _digest_credentials; }

bearer_credentials& http_authentication_resolver::get_bearer_credentials() { return _bearer_credentials; }

oauth2_credentials& http_authentication_resolver::get_oauth2_credentials() { return _oauth2_credentials; }

}  // namespace net
}  // namespace hotplace
