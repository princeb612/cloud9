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
#include <hotplace/sdk/crypto/basic/crypto_keychain.hpp>
#include <hotplace/sdk/crypto/basic/openssl_sdk.hpp>
#include <hotplace/sdk/crypto/jose/json_web_key.hpp>
#include <hotplace/sdk/io/basic/base64.hpp>
#include <hotplace/sdk/io/basic/json.hpp>
#include <fstream>

namespace hotplace {
using namespace io;
namespace crypto {

json_web_key::json_web_key () : crypto_json_key ()
{
    // do nothing
}

json_web_key::~json_web_key ()
{
    // do nothing
}

return_t json_web_key::load (crypto_key* crypto_key, const char* buffer, int flags)
{
    return_t ret = errorcode_t::success;
    json_t* root = nullptr;

    __try2
    {
        if (nullptr == buffer) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        ret = json_open_stream (&root, buffer);
        if (errorcode_t::success != ret) {
            __leave2;
        }

        json_t* keys_node = json_object_get (root, "keys");
        if (nullptr != keys_node) {
            if (JSON_ARRAY != json_typeof (keys_node)) {
                ret = errorcode_t::bad_data;
                __leave2;
            }

            size_t size = json_array_size (keys_node);
            for (size_t i = 0; i < size; i++) {
                json_t* temp = json_array_get (keys_node, i);
                read (crypto_key, temp);
            } // json_array_size
        } else {
            read (crypto_key, root);
        }
    }
    __finally2
    {
        if (root) {
            json_decref (root);
        }
    }
    return ret;
}

typedef struct _json_mapper_item_t {
    EVP_PKEY* pkey;
    crypto_key_t type;
    std::string kid;
    int use; // crypto_use_t
    std::string alg;
    binary_t pub1;
    binary_t pub2;
    binary_t priv;
} json_mapper_item_t;

typedef std::list <json_mapper_item_t> json_mapper_items_t;

typedef struct _json_mapper_t {
    int flag;
    json_mapper_items_t items;
} json_mapper_t;

static void jwk_serialize_item (int flag, json_mapper_item_t item, json_t* json_item)
{
    crypto_advisor* advisor = crypto_advisor::get_instance ();

    /* kty */
    json_object_set_new (json_item, "kty", json_string (nameof_key_type (item.type)));

    /* kid */
    if (item.kid.size ()) {
        json_object_set_new (json_item, "kid", json_string (item.kid.c_str ()));
    }

    /* use */
    if (crypto_use_t::use_sig == item.use) {
        json_object_set_new (json_item, "use", json_string ("sig"));
    }
    if (crypto_use_t::use_enc == item.use) {
        json_object_set_new (json_item, "use", json_string ("enc"));
    }

    if (item.alg.size ()) {
        json_object_set_new (json_item, "alg", json_string (item.alg.c_str ()));
    }

    std::string curve_name;

    if (kindof_ecc (item.type)) {
        advisor->nameof_ec_curve (item.pkey, curve_name);
    }

    /* param */
    if (crypto_key_t::hmac_key == item.type) {
        json_object_set_new (json_item, "k", json_string (base64_encode (item.priv, base64_encoding_t::base64url_encoding).c_str ()));
    } else if (crypto_key_t::rsa_key == item.type) {
        json_object_set_new (json_item, "n", json_string (base64_encode (item.pub1, base64_encoding_t::base64url_encoding).c_str ()));
        json_object_set_new (json_item, "e", json_string (base64_encode (item.pub2, base64_encoding_t::base64url_encoding).c_str ()));
        if (flag) {
            json_object_set_new (json_item, "d", json_string (base64_encode (item.priv, base64_encoding_t::base64url_encoding).c_str ()));
        }
    } else if (crypto_key_t::ec_key == item.type) {
        json_object_set_new (json_item, "crv", json_string (curve_name.c_str ()));
        json_object_set_new (json_item, "x", json_string (base64_encode (item.pub1, base64_encoding_t::base64url_encoding).c_str ()));
        json_object_set_new (json_item, "y", json_string (base64_encode (item.pub2, base64_encoding_t::base64url_encoding).c_str ()));
        if (flag) {
            json_object_set_new (json_item, "d", json_string (base64_encode (item.priv, base64_encoding_t::base64url_encoding).c_str ()));
        }
    } else if (crypto_key_t::okp_key == item.type) {
        json_object_set_new (json_item, "crv", json_string (curve_name.c_str ()));
        json_object_set_new (json_item, "x", json_string (base64_encode (item.pub1, base64_encoding_t::base64url_encoding).c_str ()));
        if (flag) {
            json_object_set_new (json_item, "d", json_string (base64_encode (item.priv, base64_encoding_t::base64url_encoding).c_str ()));
        }
    }
}

static return_t jwk_serialize (json_mapper_t mapper, std::string& buffer)
{
    return_t ret = errorcode_t::success;

    __try2
    {
        buffer.clear ();

        size_t size = mapper.items.size ();
        if (0 == size) {
            ret = errorcode_t::no_data;
            __leave2;
        }

        json_t* json_root = json_object ();
        if (json_root) {
            if (1 == size) {
                json_mapper_item_t& item = mapper.items.front ();
                jwk_serialize_item (mapper.flag, item, json_root);
            } else {
                json_t* json_keys = json_array ();
                if (json_keys) {
                    for (json_mapper_items_t::iterator iter = mapper.items.begin (); iter != mapper.items.end (); iter++) {
                        json_mapper_item_t item = *iter;

                        json_t* json_key = json_object ();

                        if (json_key) {
                            jwk_serialize_item (mapper.flag, item, json_key);
                            json_array_append_new (json_keys, json_key);
                        }
                    }
                    json_object_set_new (json_root, "keys", json_keys);
                }
            }
            char* contents = json_dumps (json_root, JOSE_JSON_FORMAT);
            if (contents) {
                buffer = contents;
                free (contents);
            }
            json_decref (json_root);
        }
    }
    __finally2
    {
        // do nothing
    }
    return ret;
}

static void json_writer (crypto_key_object_t* key, void* param)
{
    json_mapper_t* mapper = (json_mapper_t*) param;

    __try2
    {
        if (nullptr == key || nullptr == param) {
            __leave2;
        }

        json_mapper_item_t item;
        item.pkey = key->pkey;
        item.kid = key->kid;
        item.use = key->use;
        item.alg = key->alg;
        crypto_key::get_key (key->pkey, mapper->flag, item.type, item.pub1, item.pub2, item.priv);
        mapper->items.push_back (item);
    }
    __finally2
    {
        // do nothing
    }
    // do not return
}

return_t json_web_key::write (crypto_key* crypto_key, char* buf, size_t* buflen, int flags)
{
    return_t ret = errorcode_t::success;

    __try2
    {
        if (nullptr == crypto_key || nullptr == buflen) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        size_t size_request = *buflen;
        std::string buffer;
        json_mapper_t mapper;

        mapper.flag = flags;
        crypto_key->for_each (json_writer, &mapper);

        jwk_serialize (mapper, buffer);

        *buflen = buffer.size () + 1;
        if (buffer.size () + 1 > size_request) {
            ret = errorcode_t::insufficient_buffer;
            __leave2;
        } else {
            if (buf) {
                memcpy (buf, buffer.c_str (), buffer.size ());
                *(buf + buffer.size ()) = 0;
            } else {
                ret = errorcode_t::invalid_parameter;
                __leave2;
            }
        }
    }
    __finally2
    {
        // do nothing
    }
    return ret;
}

return_t json_web_key::write_json (crypto_key* crypto_key, const char* file, int flags)
{
    return_t ret = errorcode_t::success;

    __try2
    {
        if (nullptr == crypto_key || nullptr == file) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        std::string buffer;
        json_mapper_t mapper;
        mapper.flag = flags;
        crypto_key->for_each (json_writer, &mapper);

        jwk_serialize (mapper, buffer);

        FILE* fp = fopen (file, "wt");
        if (fp) {
            fwrite (buffer.c_str (), 1, buffer.size (), fp);
            fclose (fp);
        }
    }
    __finally2
    {
        // do nothing
    }
    return ret;
}

return_t json_web_key::read (crypto_key* crypto_key, json_t* json)
{
    return_t ret = errorcode_t::success;
    json_t* temp = json;
    crypto_keychain keyset;

    __try2
    {
        if (nullptr == crypto_key || nullptr == temp) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        const char* kty = nullptr;
        const char* kid = nullptr;
        const char* use = nullptr;
        const char* alg = nullptr;
        json_unpack (temp, "{s:s}", "kty", &kty);
        json_unpack (temp, "{s:s}", "kid", &kid);
        json_unpack (temp, "{s:s}", "use", &use);
        json_unpack (temp, "{s:s}", "alg", &alg);

        crypto_use_t usage = crypto_use_t::use_any;
        if (nullptr != use) {
            if (0 == strcmp (use, "sig")) {
                usage = crypto_use_t::use_sig;
            } else if (0 == strcmp (use, "enc")) {
                usage = crypto_use_t::use_enc;
            }
        }

        if (nullptr != kty) {
            if (0 == strcmp (kty, "oct")) {
                const char* k_value = nullptr;
                json_unpack (temp, "{s:s}", "k", &k_value);

                add_oct (crypto_key, kid, alg, k_value, usage);
            } else if (0 == strcmp (kty, "RSA")) {
                const char* n_value = nullptr;
                const char* e_value = nullptr;
                const char* d_value = nullptr;
                json_unpack (temp, "{s:s,s:s,s:s}", "n", &n_value, "e", &e_value, "d", &d_value);

                const char* p_value = nullptr;
                const char* q_value = nullptr;
                const char* dp_value = nullptr;
                const char* dq_value = nullptr;
                const char* qi_value = nullptr;
                json_unpack (temp, "{s:s,s:s,s:s,s:s,s:s}",
                             "p", &p_value, "q", &q_value, "dp", &dp_value, "dq", &dq_value, "qi", &qi_value);

                add_rsa (crypto_key, kid, alg, n_value, e_value, d_value, p_value, q_value, dp_value, dq_value, qi_value, usage);
            } else if (0 == strcmp (kty, "EC")) {
                const char* crv_value = nullptr;
                const char* x_value = nullptr;
                const char* y_value = nullptr;
                const char* d_value = nullptr;
                json_unpack (temp, "{s:s,s:s,s:s,s:s}", "crv", &crv_value, "x", &x_value, "y", &y_value, "d", &d_value);

                add_ec (crypto_key, kid, alg, crv_value, x_value, y_value, d_value, usage);
            } else if (0 == strcmp (kty, "OKP")) {
                const char* crv_value = nullptr;
                const char* x_value = nullptr;
                const char* d_value = nullptr;
                json_unpack (temp, "{s:s,s:s,s:s}", "crv", &crv_value, "x", &x_value, "d", &d_value);

                add_ec (crypto_key, kid, alg, crv_value, x_value, nullptr, d_value, usage);
            } else {
                // do nothing
            }
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
