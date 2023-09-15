/* vim: set tabstop=4 shiftwidth=4 softtabstop=4 expandtab smarttab : */
/**
 * @file {file}
 * @author Soo Han, Kim (princeb612.kr@gmail.com)
 * @desc
 *
 * Revision History
 * Date         Name                Description
 */

#include <hotplace/sdk/sdk.hpp>
#include <stdio.h>
#include <iostream>

using namespace hotplace;
using namespace hotplace::io;
using namespace hotplace::crypto;

test_case _test_case;

enum cose_header_t {
    // RFC 8152 Table 2: Common Header Parameters
    // RFC 8152 Table 3: Common Header Parameters
    cose_header_alg             = 1,    // int / tstr
    cose_header_crit            = 2,    // [+ label]
    cose_header_content_type    = 3,    // tstr / uint
    cose_header_kid             = 4,    // bstr
    cose_header_iv              = 5,    // bstr
    cose_header_partial_iv      = 6,    // bstr

    cose_header_counter_sig     = 7,    // COSE_Signature / [+ COSE_Signature]

    // RFC 8152 Table 27: Header Parameter for CounterSignature0
    cose_header_counter_sig0    = 9,

    // RFC 9338 Table 1: Common Header Parameters
    // RFC 9338 Table 2: New Common Header Parameters
    cose_header_counter_sig_v2  = 11,
    cose_header_counter_sig0_v2 = 12,

    // RFC 9360 Table 1: X.509 COSE Header Parameters
    cose_header_x5bag           = 32,
    cose_header_x5chain         = 33,
    cose_header_x5t             = 34,
    cose_header_x5u             = 35,
};

enum cose_key_t {
    // RFC 8152 Table 15: Direct Key
    // RFC 9053 Table 11: Direct Key
    cose_direct                 = -6,

    // RFC 8152 Table 16: Direct Key with KDF
    // RFC 9053 Table 12: Direct Key with KDF
    cose_direct_hkdf_sha_256    = -10,
    cose_direct_hkdf_sha_512    = -11,
    cose_direct_hkdf_aes_128    = -12,
    cose_direct_hkdf_aes_256    = -13,

    // RFC 8152 Table 21: Key Type Values
    // RFC 9053 Table 17: Key Type Values
    // RFC 9053 Table 22: Key Type Capabilities
    cose_key_reserved   = 0,
    cose_key_okp        = 1,
    cose_key_ec2        = 2,
    cose_key_symm       = 4,

    // RFC 8230 Table 3: Key Type Values
    // RFC 9053 Table 22: Key Type Capabilities
    cose_key_rsa        = 3,

    // RFC 9053 Table 22: Key Type Capabilities
    cose_key_hss_lms    = 5,
    cose_walnutdsa      = 6,
};

enum cose_key_lable_t {
    // RFC 8152 Table 3: Key Map Labels
    // RFC 8152 Table 4: Key Map Labels
    cose_lable_kty      = 1,
    cose_lable_kid      = 2,
    cose_lable_alg      = 3,
    cose_lable_keyops   = 4,
    cose_lable_base_iv  = 5,
};

enum cose_keyop_t {
    // RFC 8152 Table 4: Key Operation Values
    // RFC 8152 Table 5: Key Operation Values
    cose_keyop_sign         = 1,
    cose_keyop_verify       = 2,
    cose_keyop_encrypt      = 3,
    cose_keyop_decrypt      = 4,
    cose_keyop_wrap         = 5,
    cose_keyop_unwrap       = 6,
    cose_keyop_derive_key   = 7,
    cose_keyop_derive_bits  = 8,
    cose_keyop_mac_create   = 9,
    cose_keyop_mac_verify   = 10,
};

enum cose_ec_curve_t {
    // RFC 8152 Table 22: Elliptic Curves
    // RFC 9053 Table 18: Elliptic Curves
    cose_ec_p256    = 1,
    cose_ec_p384    = 2,
    cose_ec_p521    = 3,
    cose_ec_x25519  = 4,
    cose_ec_x448    = 5,
    cose_ec_ed25519 = 6,
    cose_ec_ed448   = 7,
};

enum cose_ec_key_param_t {
    // RFC 8152 Table 23: EC Key Parameters
    // RFC 9053 Table 19: EC Key Parameters
    // cose_key_t::cose_key_ec2
    cose_ec_crv = -1,
    cose_ec_x   = -2,
    cose_ec_y   = -3,
    cose_ec_d   = -4,
};
enum cose_okp_key_param_t {
    // RFC 8152 Table 24: Octet Key Pair Parameters
    // RFC 9053 Table 20: Octet Key Pair Parameters
    // cose_key_t::cose_key_okp
    cose_okp_crv    = -1,
    cose_okp_x      = -2,
    cose_okp_d      = -4,
};
enum cose_symm_key_param_t {
    // RFC 8152 Table 25: Symmetric Key Parameters
    // RFC 9053 Table 21: Symmetric Key Parameters
    cose_symm_k = -1,
};
enum cose_rsa_key_param_t {
    // RSA 8230 Table 4: RSA Key Parameters
    cose_rsa_n      = -1,
    cose_rsa_e      = -2,
    cose_rsa_d      = -3,
    cose_rsa_p      = -4,
    cose_rsa_q      = -5,
    cose_rsa_dp     = -6,
    cose_rsa_dq     = -7,
    cose_rsa_qi     = -8,
    cose_rsa_other  = -9,
    cose_rsa_ri     = -10,
    cose_rsa_di     = -11,
    cose_rsa_ti     = -12,
};

enum cose_alg_t {

    // RFC 8152 Table 17: AES Key Wrap Algorithm Values
    // RFC 9053 Table 13: AES Key Wrap Algorithm Values
    cose_a128kw                 = -3,
    cose_a192kw                 = -4,
    cose_a256kw                 = -5,

    // RFC 8152 Table 5: ECDSA Algorithm Values
    // RFC 9053 Table 1: ECDSA Algorithm Values
    cose_es256                  = -7,
    cose_es384                  = -35,
    cose_es512                  = -36,

    // RFC 8152 Table 6: EdDSA Algorithm Values
    // RFC 9053 Table 2: EdDSA Algorithm Value
    cose_eddsa                  = -8,

    // RFC 9054 Table 1: SHA-1 Hash Algorithm
    cose_sha1                   = -14,

    // RFC 9054 Table 2: SHA-2 Hash Algorithms
    cose_sha256_64              = -15,
    cose_sha256                 = -16,
    cose_sha512_256             = -17,
    cose_sha384                 = -43,
    cose_sha512                 = -44,

    // RFC 9054 Table 3: SHAKE Hash Functions
    cose_shake128               = -18,
    cose_shake256               = -45,

    // RFC 8152 Table 18: ECDH Algorithm Values
    // RFC 9053 Table 14: ECDH Algorithm Values
    cose_ecdh_es_hkdf_256       = -25,
    cose_ecdh_es_hkdf_512       = -26,
    cose_ecdh_ss_hkdf_256       = -27,
    cose_ecdh_ss_hkdf_512       = -28,

    // RFC 8152 Table 20: ECDH Algorithm Values with Key Wrap
    // RFC 9053 Table 16: ECDH Algorithm Values with Key Wrap
    cose_ecdh_es_a128kw         = -29,
    cose_ecdh_es_a192kw         = -30,
    cose_ecdh_es_a256kw         = -31,
    cose_ecdh_ss_a128kw         = -32,
    cose_ecdh_ss_a192kw         = -33,
    cose_ecdh_ss_a256kw         = -34,

    // RFC 8230 Table 1: RSASSA-PSS Algorithm Values
    cose_ps256                  = -37,
    cose_ps384                  = -38,
    cose_ps512                  = -39,

    // RFC 8230 Table 2: RSAES-OAEP Algorithm Values
    cose_rsaes_oaep_sha1        = -40,
    cose_rsaes_oaep_sha256      = -41,
    cose_rsaes_oaep_sha512      = -42,

    // RFC 8812 Table 2: ECDSA Algorithm Values
    cose_es256k                 = -47,

    // RFC 8812 Table 1: RSASSA-PKCS1-v1_5 Algorithm Values
    cose_rs256                  = -257,
    cose_rs384                  = -258,
    cose_rs512                  = -259,
    cose_rs1                    = -65535,

    // RFC 8152 Table 9: Algorithm Value for AES-GCM
    // RFC 9053 Table 5: Algorithm Values for AES-GCM
    cose_aes_128_gcm            = 1,
    cose_aes_192_gcm            = 2,
    cose_aes_256_gcm            = 3,

    // RFC 8152 Table 7: HMAC Algorithm Values
    // RFC 9053 Table 3: HMAC Algorithm Values
    cose_hmac_256_64            = 4,
    cose_hmac_256_256           = 5,
    cose_hmac_384_256           = 6,
    cose_hmac_512_512           = 7,

    // RFC 8152 Table 10: Algorithm Values for AES-CCM
    // RFC 9053 Table 6: Algorithm Values for AES-CCM
    cose_aes_ccm_16_64_128      = 10,
    cose_aes_ccm_16_64_256      = 11,
    cose_aes_ccm_64_64_128      = 12,
    cose_aes_ccm_64_64_256      = 13,
    cose_aes_ccm_16_128_128     = 30,
    cose_aes_ccm_16_128_256     = 31,
    cose_aes_ccm_64_128_128     = 32,
    cose_aes_ccm_64_128_256     = 33,

    // RFC 8152 Table 8: AES-MAC Algorithm Values
    // RFC 9053 Table 4: AES-MAC Algorithm Values
    cose_aes_cbc_mac_128_64     = 14,
    cose_aes_cbc_mac_256_64     = 15,
    cose_aes_cbc_mac_128_128    = 25,
    cose_aes_cbc_mac_256_128    = 26,

    // RFC 8152 Table 11: Algorithm Value for AES-GCM
    // RFC 9053 Table 7: Algorithm Value for ChaCha20/Poly1305
    cose_chacha20_poly1305      = 24,

    // RFC 9053 Table 23: New entry in the COSE Algorithms registry
    cose_iv_generation          = 34,
};

enum cose_alg_param_t {
    // RFC 8152 Table 19: ECDH Algorithm Parameters
    // RFC 9053 Table 15: ECDH Algorithm Parameters
    cose_ephemeral_key  = -1,
    cose_static_key     = -2,
    cose_static_key_id  = -3,

    // RFC 8152 Table 13: HKDF Algorithm Parameters
    // RFC 9053 Table 9: HKDF Algorithm Parameters
    cose_salt           = -20,

    // RFC 8152 Table 14: Context Algorithm Parameters
    // RFC 9053 Table 10: Context Algorithm Parameters
    cose_partyu_id      = -21,
    cose_partyu_nonce   = -22,
    cose_partyu_other   = -23,
    cose_partyv_id      = -24,
    cose_partyv_nonce   = -25,
    cose_partyv_other   = -26,

    // RFC 9360 Table 2: Static ECDH Algorithm Values
    cose_x5t_sender     = -27,
    cose_x5u_sender     = -28,
    cose_x5chain_sender = -29,
};

/**
 * @brief   interface prototyping
 */
typedef struct _cose_item_t {
    variant_t key;
    variant_t value;
} cose_item_t;
typedef std::list <cose_item_t> cose_list_t;

class cbor_object_signing_encryption
{
public:
    cbor_object_signing_encryption ();
    ~cbor_object_signing_encryption ();

    return_t build_protected (cbor_data** object);
    return_t build_protected (cbor_data** object, cose_list_t const& input);
    return_t build_protected (cbor_data** object, cbor_map* input);
    return_t build_unprotected (cbor_map** object, cose_list_t const& input);
    return_t build_payload (cbor_data** object, const char* payload);
    return_t build_payload (cbor_data** object, const byte_t* payload, size_t size);
    return_t build_base16str (cbor_data** object, const char* str);
};

cbor_object_signing_encryption::cbor_object_signing_encryption ()
{
    // do nothing
}
cbor_object_signing_encryption::~cbor_object_signing_encryption ()
{
    // do nothing
}

return_t cbor_object_signing_encryption::build_protected (cbor_data** object)
{
    return_t ret = errorcode_t::success;

    __try2
    {
        if (nullptr == object) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        cbor_data* part_protected = nullptr;
        binary_t dummy;
        __try_new_catch (part_protected, new cbor_data (dummy), ret, __leave2);
        *object = part_protected;
    }
    __finally2
    {
        // do nothing
    }
    return ret;
}

return_t cbor_object_signing_encryption::build_protected (cbor_data** object, cose_list_t const& input)
{
    return_t ret = errorcode_t::success;

    __try2
    {
        if (nullptr == object) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        if (0 == input.size ()) {
            cbor_data* part_protected = nullptr;
            binary_t dummy;
            __try_new_catch (part_protected, new cbor_data (dummy), ret, __leave2);
            *object = part_protected;
        } else {
            binary_t bin_protected;
            cbor_map* part_protected = nullptr;

            __try_new_catch (part_protected, new cbor_map (), ret, __leave2);

            cose_list_t::const_iterator iter;
            for (iter = input.begin (); iter != input.end (); iter++) {
                cose_item_t const& item = *iter;
                *part_protected << new cbor_pair (new cbor_data (item.key), new cbor_data (item.value));
            }

            build_protected (object, part_protected);

            part_protected->release ();
        }
    }
    __finally2
    {
        // do nothing
    }
    return ret;
}

return_t cbor_object_signing_encryption::build_protected (cbor_data** object, cbor_map* input)
{
    return_t ret = errorcode_t::success;

    __try2
    {
        if (nullptr == object || nullptr == input) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        binary_t bin_protected;
        cbor_publisher publisher;
        publisher.publish (input, &bin_protected);

        *object = new cbor_data (bin_protected);
    }
    __finally2
    {
        // do nothing
    }
    return ret;
}

return_t cbor_object_signing_encryption::build_unprotected (cbor_map** object, cose_list_t const& input)
{
    return_t ret = errorcode_t::success;

    __try2
    {
        if (nullptr == object) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        cbor_map* part_unprotected = nullptr;

        __try_new_catch (part_unprotected, new cbor_map (), ret, __leave2);

        cose_list_t::const_iterator iter;
        for (iter = input.begin (); iter != input.end (); iter++) {
            cose_item_t const& item = *iter;
            *part_unprotected << new cbor_pair (new cbor_data (item.key), new cbor_data (item.value));
        }

        *object = part_unprotected;
    }
    __finally2
    {
        // do nothing
    }
    return ret;
}

return_t cbor_object_signing_encryption::build_payload (cbor_data** object, const char* payload)
{
    return_t ret = errorcode_t::success;

    __try2
    {
        if (nullptr == object || nullptr == payload) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        __try_new_catch (*object, new cbor_data (convert (payload)), ret, __leave2);
    }
    __finally2
    {
    }
    return ret;
}

return_t cbor_object_signing_encryption::build_payload (cbor_data** object, const byte_t* payload, size_t size)
{
    return_t ret = errorcode_t::success;

    __try2
    {
        if (nullptr == object) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        __try_new_catch (*object, new cbor_data (payload, size), ret, __leave2);
    }
    __finally2
    {
    }
    return ret;
}

return_t cbor_object_signing_encryption::build_base16str (cbor_data** object, const char* str)
{
    return_t ret = errorcode_t::success;

    __try2
    {
        if (nullptr == object || nullptr == str) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        __try_new_catch (*object, new cbor_data (base16_decode (str)), ret, __leave2);
    }
    __finally2
    {
    }
    return ret;
}

return_t cbor_dump (cbor_object* root, const char* expect_file, const char* text)
{
    return_t ret = errorcode_t::success;

    __try2
    {
        if (nullptr == root || nullptr == expect_file) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        cbor_publisher publisher;

        buffer_stream diagnostic;
        publisher.publish (root, &diagnostic);

        binary_t bin;
        publisher.publish (root, &bin);

        binary_t expect;
        {
            test_case_notimecheck notimecheck (_test_case);

            std::cout << "diagnostic\n" << diagnostic.c_str () << std::endl;

            buffer_stream bs;
            dump_memory (bin, &bs);
            std::cout << "encoded" << std::endl << bs.c_str () << std::endl;

            {
                file_stream fs;
                ret = fs.open (expect_file);
                if (errorcode_t::success != ret) {
                    __leave2;
                }
                fs.begin_mmap ();

                byte_t* file_contents = fs.data ();
                size_t file_size = fs.size ();
                expect.insert (expect.end (), file_contents, file_contents + file_size);
            }

            dump_memory (expect, &bs);
            std::cout << "test vector" << std::endl << bs.c_str () << std::endl;

            if (bin.size () == expect.size ()) {
                if (0 == memcmp (&bin[0], &expect[0], bin.size ())) {
                    // do nothing
                } else {
                    ret = errorcode_t::mismatch;
                }
            } else {
                ret = errorcode_t::mismatch;
            }
            _test_case.test (ret, __FUNCTION__, "check1.cborcheck %s", text ? text : "");
        }

        {
            test_case_notimecheck notimecheck (_test_case);
            printf ("reparse\n");

            buffer_stream diagnostic2;
            binary_t bin2;

            cbor_reader reader;
            cbor_reader_context_t* handle = nullptr;

            reader.open (&handle);
            reader.parse (handle, bin);
            reader.publish (handle, &diagnostic2);
            reader.publish (handle, &bin2);
            reader.close (handle);

            std::cout << "diagnostic\n" << diagnostic2.c_str () << std::endl;

            buffer_stream bs;
            dump_memory (bin2, &bs);
            std::cout << "encoded" << std::endl << bs.c_str () << std::endl;

            if (bin2.size () == expect.size ()) {
                if (0 == memcmp (&bin2[0], &expect[0], bin2.size ())) {
                    // do nothing
                } else {
                    ret = errorcode_t::mismatch;
                }
            } else {
                ret = errorcode_t::mismatch;
            }
            _test_case.test (ret, __FUNCTION__, "check2.cborparse %s", text ? text : "");
        }
    }
    __finally2
    {
        // do nothing
    }
    return ret;
}

/**
 * @brief   CDDL
   Headers = (
       protected : empty_or_serialized_map,
       unprotected : header_map
   )

   header_map = {
       Generic_Headers,
 * label => values
   }
 */

void test_rfc8152_c_1_1 ()
{
    _test_case.begin ("RFC 8152 C.1");

    // Signature Algorithm: ECDSA w/ SHA-256, Curve P-256

    cbor_publisher publisher;

    cbor_object_signing_encryption cose;

    cbor_data* cbor_data_protected = nullptr;
    cose.build_protected (&cbor_data_protected);

    cbor_data* cbor_data_payload = nullptr;
    cose.build_payload (&cbor_data_payload, "This is the content.");

    cbor_array* root = new cbor_array ();
    root->tag (true, cbor_tag_t::cose_tag_sign);
    *root   << cbor_data_protected  // protected, bstr
            << new cbor_map ()      // unprotected, map
            << cbor_data_payload    // payload, bstr/nil(detached)
            << new cbor_array ();   // signatures

    cbor_array* signatures = (cbor_array*) (*root)[3];

    cbor_array* signature = new cbor_array ();
    {
        cbor_data* cbor_data_signature_protected = nullptr;
        {
            cose_item_t item;
            cose_list_t list_protected;
            variant_set_int16 (item.key, cose_header_t::cose_header_alg);
            variant_set_int16 (item.value, cose_alg_t::cose_es256);
            list_protected.push_back (item);
            cose.build_protected (&cbor_data_signature_protected, list_protected);
        }

        cbor_map* cbor_data_signature_unprotected = nullptr;
        {
            cose_item_t item;
            cose_list_t list_unprotected;
            variant_set_int16 (item.key, cose_header_t::cose_header_kid);
            variant_set_bstr_new (item.value, "11", 2);
            list_unprotected.push_back (item);
            cose.build_unprotected (&cbor_data_signature_unprotected, list_unprotected);
        }

        cbor_data* cbor_data_signature_signature = nullptr;
        {
            constexpr char constexpr_sig[] = "e2aeafd40d69d19dfe6e52077c5d7ff4e408282cbefb5d06cbf414af2e19d982ac45ac98b8544c908b4507de1e90b717c3d34816fe926a2b98f53afd2fa0f30a";
            cose.build_base16str (&cbor_data_signature_signature, constexpr_sig);
        }

        *signature  << cbor_data_signature_protected
                    << cbor_data_signature_unprotected
                    << cbor_data_signature_signature;
    }
    *signatures << signature;

    cbor_dump (root, "rfc8152_c_1_1.cbor", "RFC 8152 C.1.1.  Single Signature");

    root->release ();
}

void test_rfc8152_c_1_2 ()
{
    _test_case.begin ("RFC 8152 C.1");

    // Signature Algorithm: ECDSA w/ SHA-256, Curve P-256
    // Signature Algorithm: ECDSA w/ SHA-512, Curve P-521

    cbor_publisher publisher;

    cbor_object_signing_encryption cose;

    cbor_data* cbor_data_protected = nullptr;
    cose.build_protected (&cbor_data_protected);

    cbor_data* cbor_data_payload = nullptr;
    cose.build_payload (&cbor_data_payload, "This is the content.");

    cbor_array* root = new cbor_array ();
    root->tag (true, cbor_tag_t::cose_tag_sign);
    *root   << cbor_data_protected  // protected
            << new cbor_map ()      // unprotected
            << cbor_data_payload    // payload
            << new cbor_array ();   // signatures

    cbor_array* signatures = (cbor_array*) (*root)[3];

    {
        cbor_array* signature = new cbor_array ();

        cbor_data* cbor_data_signature_protected = nullptr;
        {
            cose_item_t item;
            cose_list_t list_protected;
            variant_set_int16 (item.key, cose_header_t::cose_header_alg);
            variant_set_int16 (item.value, cose_alg_t::cose_es256);
            list_protected.push_back (item);
            cose.build_protected (&cbor_data_signature_protected, list_protected);
        }

        cbor_map* cbor_data_signature_unprotected = nullptr;
        {
            cose_item_t item;
            cose_list_t list_unprotected;
            variant_set_int16 (item.key, cose_header_t::cose_header_kid);
            variant_set_bstr_new (item.value, "11", 2);
            list_unprotected.push_back (item);
            cose.build_unprotected (&cbor_data_signature_unprotected, list_unprotected);
        }

        cbor_data* cbor_data_signature_signature = nullptr;
        {
            constexpr char constexpr_sig[] = "e2aeafd40d69d19dfe6e52077c5d7ff4e408282cbefb5d06cbf414af2e19d982ac45ac98b8544c908b4507de1e90b717c3d34816fe926a2b98f53afd2fa0f30a";
            cose.build_base16str (&cbor_data_signature_signature, constexpr_sig);
        }

        *signature  << cbor_data_signature_protected
                    << cbor_data_signature_unprotected
                    << cbor_data_signature_signature;

        *signatures << signature;
    }
    {
        cbor_array* signature = new cbor_array ();

        cbor_data* cbor_data_signature_protected = nullptr;
        {
            cose_item_t item;
            cose_list_t list_protected;
            variant_set_int16 (item.key, cose_header_t::cose_header_alg);
            variant_set_int16 (item.value, cose_alg_t::cose_es512);
            list_protected.push_back (item);
            cose.build_protected (&cbor_data_signature_protected, list_protected);
        }

        cbor_map* cbor_data_signature_unprotected = nullptr;
        {
            cose_item_t item;
            cose_list_t list_unprotected;
            variant_set_int16 (item.key, cose_header_t::cose_header_kid);
            variant_set_bstr_new (item.value, "bilbo.baggins@hobbiton.example", 30);
            list_unprotected.push_back (item);
            cose.build_unprotected (&cbor_data_signature_unprotected, list_unprotected);
        }

        cbor_data* cbor_data_signature_signature = nullptr;
        {
            constexpr char constexpr_sig[] = "00a2d28a7c2bdb1587877420f65adf7d0b9a06635dd1de64bb62974c863f0b160dd2163734034e6ac003b01e8705524c5c4ca479a952f0247ee8cb0b4fb7397ba08d009e0c8bf482270cc5771aa143966e5a469a09f613488030c5b07ec6d722e3835adb5b2d8c44e95ffb13877dd2582866883535de3bb03d01753f83ab87bb4f7a0297";
            cose.build_base16str (&cbor_data_signature_signature, constexpr_sig);
        }

        *signature  << cbor_data_signature_protected
                    << cbor_data_signature_unprotected
                    << cbor_data_signature_signature;

        *signatures << signature;
    }

    cbor_dump (root, "rfc8152_c_1_2.cbor", "RFC 8152 C.1.2.  Multiple Signers");

    root->release ();
}

void test_rfc8152_c_1_3 ()
{
    _test_case.begin ("RFC 8152 C.1");

    // Signature Algorithm: ECDSA w/ SHA-256, Curve P-256
    // The same parameters are used for both the signature and the counter signature.

    cbor_publisher publisher;

    cbor_object_signing_encryption cose;

    cbor_data* cbor_data_protected = nullptr;
    cose.build_protected (&cbor_data_protected);

    cbor_data* cbor_data_payload = nullptr;
    cose.build_payload (&cbor_data_payload, "This is the content.");

    cbor_array* root = new cbor_array ();
    root->tag (true, cbor_tag_t::cose_tag_sign);
    *root   << cbor_data_protected  // protected
            << new cbor_map ()      // unprotected
            << cbor_data_payload    // payload
            << new cbor_array ();   // signatures

    cbor_map* header_unprotected = (cbor_map*) (*root)[1];
    {
        cbor_array* countersign = new cbor_array ();
        *header_unprotected << new cbor_pair (cose_header_t::cose_header_counter_sig, countersign);

        cbor_data* cbor_data_countersignature_protected = nullptr;
        {
            cose_item_t item;
            cose_list_t list_protected;
            variant_set_int16 (item.key, cose_header_t::cose_header_alg);
            variant_set_int16 (item.value, cose_alg_t::cose_es256);
            list_protected.push_back (item);
            cose.build_protected (&cbor_data_countersignature_protected, list_protected);
        }

        cbor_map* cbor_data_countersignature_unprotected = nullptr;
        {
            cose_item_t item;
            cose_list_t list_unprotected;
            variant_set_int16 (item.key, cose_header_t::cose_header_kid);
            variant_set_bstr_new (item.value, "11", 2);
            list_unprotected.push_back (item);
            cose.build_unprotected (&cbor_data_countersignature_unprotected, list_unprotected);
        }

        cbor_data* cbor_data_countersignature_signature = nullptr;
        {
            constexpr char constexpr_sig[] = "5ac05e289d5d0e1b0a7f048a5d2b643813ded50bc9e49220f4f7278f85f19d4a77d655c9d3b51e805a74b099e1e085aacd97fc29d72f887e8802bb6650cceb2c";
            cose.build_base16str (&cbor_data_countersignature_signature, constexpr_sig);
        }

        *countersign    << cbor_data_countersignature_protected
                        << cbor_data_countersignature_unprotected
                        << cbor_data_countersignature_signature;
    }

    cbor_array* signatures = (cbor_array*) (*root)[3];

    cbor_array* signature = new cbor_array ();
    {
        cbor_data* cbor_data_signature_protected = nullptr;
        {
            cose_item_t item;
            cose_list_t list_protected;
            variant_set_int16 (item.key, cose_header_t::cose_header_alg);
            variant_set_int16 (item.value, cose_alg_t::cose_es256);
            list_protected.push_back (item);
            cose.build_protected (&cbor_data_signature_protected, list_protected);
        }

        cbor_map* cbor_data_signature_unprotected = nullptr;
        {
            cose_item_t item;
            cose_list_t list_unprotected;
            variant_set_int16 (item.key, cose_header_t::cose_header_kid);
            variant_set_bstr_new (item.value, "11", 2);
            list_unprotected.push_back (item);
            cose.build_unprotected (&cbor_data_signature_unprotected, list_unprotected);
        }

        cbor_data* cbor_data_signature_signature = nullptr;
        {
            constexpr char constexpr_sig[] = "e2aeafd40d69d19dfe6e52077c5d7ff4e408282cbefb5d06cbf414af2e19d982ac45ac98b8544c908b4507de1e90b717c3d34816fe926a2b98f53afd2fa0f30a";
            cose.build_base16str (&cbor_data_signature_signature, constexpr_sig);
        }

        *signature  << cbor_data_signature_protected
                    << cbor_data_signature_unprotected
                    << cbor_data_signature_signature;

        *signatures << signature;
    }

    cbor_dump (root, "rfc8152_c_1_3.cbor", "RFC 8152 C.1.3.  Counter Signature");

    root->release ();
}

void test_rfc8152_c_1_4 ()
{
    _test_case.begin ("RFC 8152 C.1");

    // Signature Algorithm: ECDSA w/ SHA-256, Curve P-256
    // There is a criticality marker on the "reserved" header parameter

    cbor_publisher publisher;

    cbor_object_signing_encryption cose;

    cbor_array* root = new cbor_array ();
    root->tag (true, cbor_tag_t::cose_tag_sign);

    cbor_data* cbor_data_protected = nullptr;
    {
        cbor_map* cbor_map_protected = new cbor_map ();

        cbor_array* crit = new cbor_array ();
        *crit << new cbor_data ("reserved"); // [+ label]

        *cbor_map_protected << new cbor_pair ("reserved", new cbor_data (false))
                            << new cbor_pair (cose_header_t::cose_header_crit, crit);

        cose.build_protected (&cbor_data_protected, cbor_map_protected);

        cbor_map_protected->release ();
    }

    cbor_data* cbor_data_payload = nullptr;
    cose.build_payload (&cbor_data_payload, "This is the content.");

    *root   << cbor_data_protected  // protected
            << new cbor_map ()      // unprotected
            << cbor_data_payload    // payload
            << new cbor_array ();   // signatures

    cbor_array* signatures = (cbor_array*) (*root)[3];

    cbor_array* signature = new cbor_array ();
    {
        cbor_data* cbor_data_signature_protected = nullptr;
        {
            cose_item_t item;
            cose_list_t list_protected;
            variant_set_int16 (item.key, cose_header_t::cose_header_alg);
            variant_set_int16 (item.value, cose_alg_t::cose_es256);
            list_protected.push_back (item);
            cose.build_protected (&cbor_data_signature_protected, list_protected);
        }

        cbor_map* cbor_data_signature_unprotected = nullptr;
        {
            cose_item_t item;
            cose_list_t list_unprotected;
            variant_set_int16 (item.key, cose_header_t::cose_header_kid);
            variant_set_bstr_new (item.value, "11", 2);
            list_unprotected.push_back (item);
            cose.build_unprotected (&cbor_data_signature_unprotected, list_unprotected);
        }

        cbor_data* cbor_data_signature_signature = nullptr;
        {
            constexpr char constexpr_sig[] = "3fc54702aa56e1b2cb20284294c9106a63f91bac658d69351210a031d8fc7c5ff3e4be39445b1a3e83e1510d1aca2f2e8a7c081c7645042b18aba9d1fad1bd9c";
            cose.build_base16str (&cbor_data_signature_signature, constexpr_sig);
        }

        *signature  << cbor_data_signature_protected
                    << cbor_data_signature_unprotected
                    << cbor_data_signature_signature;

        *signatures << signature;
    }

    cbor_dump (root, "rfc8152_c_1_4.cbor", "RFC 8152 C.1.4.  Signature with Criticality");

    root->release ();
}

void test_rfc8152_c_2_1 ()
{
    _test_case.begin ("RFC 8152 C.2");
    cbor_publisher publisher;

    cbor_object_signing_encryption cose;

    cbor_array* root = new cbor_array ();
    root->tag (true, cbor_tag_t::cose_tag_sign1);

    cbor_data* cbor_data_protected = nullptr;
    {
        cose_item_t item;
        cose_list_t list_protected;
        variant_set_int16 (item.key, cose_header_t::cose_header_alg);
        variant_set_int16 (item.value, cose_alg_t::cose_es256);
        list_protected.push_back (item);
        cose.build_protected (&cbor_data_protected, list_protected);
    }

    cbor_map* cbor_data_unprotected = nullptr;
    {
        cose_item_t item;
        cose_list_t list_unprotected;
        variant_set_int16 (item.key, cose_header_t::cose_header_kid);
        variant_set_bstr_new (item.value, "11", 2);
        list_unprotected.push_back (item);
        cose.build_unprotected (&cbor_data_unprotected, list_unprotected);
    }

    cbor_data* cbor_data_payload = nullptr;
    cose.build_payload (&cbor_data_payload, "This is the content.");

    cbor_data* cbor_data_signature = nullptr;
    cose.build_base16str (&cbor_data_signature, "8eb33e4ca31d1c465ab05aac34cc6b23d58fef5c083106c4d25a91aef0b0117e2af9a291aa32e14ab834dc56ed2a223444547e01f11d3b0916e5a4c345cacb36");

    *root   << cbor_data_protected
            << cbor_data_unprotected
            << cbor_data_payload
            << cbor_data_signature;

    cbor_dump (root, "rfc8152_c_2_1.cbor", "RFC 8152 C.2.1.  Single ECDSA Signature");

    root->release ();
}

void test_rfc8152_c_3_1 ()
{
    _test_case.begin ("RFC 8152 C.3");
    cbor_publisher publisher;

    cbor_object_signing_encryption cose;

    cbor_array* root = new cbor_array ();
    root->tag (true, cbor_tag_t::cose_tag_encrypt);

    cbor_data* cbor_data_protected = nullptr;
    {
        cose_item_t item;
        cose_list_t list_protected;
        variant_set_int16 (item.key, cose_header_t::cose_header_alg);
        variant_set_int16 (item.value, cose_alg_t::cose_aes_128_gcm);
        list_protected.push_back (item);
        cose.build_protected (&cbor_data_protected, list_protected);
    }

    cbor_data* cbor_data_ciphertext = nullptr;
    cose.build_base16str (&cbor_data_ciphertext, "7adbe2709ca818fb415f1e5df66f4e1a51053ba6d65a1a0c52a357da7a644b8070a151b0");

    *root   << cbor_data_protected  // protected
            << new cbor_map ()      // unprotected
            << cbor_data_ciphertext // ciphertext
            << new cbor_array ();   // recipients

    cbor_map* header_unprotected = (cbor_map*) (*root)[1];
    {
        *header_unprotected << new cbor_pair (cose_header_t::cose_header_iv, new cbor_data (base16_decode ("c9cf4df2fe6c632bf7886413")));
    }

    cbor_array* recipients = (cbor_array*) (*root)[3];

    cbor_array* recipient = new cbor_array ();
    {
        cbor_data* cbor_data_recipient_protected = nullptr;
        {
            cose_item_t item;
            cose_list_t list_protected;
            variant_set_int16 (item.key, cose_header_t::cose_header_alg);
            variant_set_int16 (item.value, cose_alg_t::cose_ecdh_es_hkdf_256);
            list_protected.push_back (item);
            cose.build_protected (&cbor_data_recipient_protected, list_protected);
        }

        cbor_map* cbor_data_recipient_unprotected = new cbor_map ();
        {
            constexpr char constexpr_x [] = "98f50a4ff6c05861c8860d13a638ea56c3f5ad7590bbfbf054e1c7b4d91d6280";
            constexpr char constexpr_kid [] = "meriadoc.brandybuck@buckland.example";

            cbor_map* ephemeral = new cbor_map ();
            *ephemeral
                << new cbor_pair (cose_key_lable_t::cose_lable_kty, new cbor_data (cose_key_t::cose_key_ec2))       // kty
                << new cbor_pair (cose_ec_key_param_t::cose_ec_crv, new cbor_data (cose_ec_curve_t::cose_ec_p256))  // crv
                << new cbor_pair (cose_ec_key_param_t::cose_ec_x, new cbor_data (base16_decode (constexpr_x)))      // x
                << new cbor_pair (cose_ec_key_param_t::cose_ec_y, new cbor_data (true));                            // y

            *cbor_data_recipient_unprotected
                << new cbor_pair (cose_alg_param_t::cose_ephemeral_key, ephemeral)                              // epk
                << new cbor_pair (cose_header_t::cose_header_kid, new cbor_data (convert (constexpr_kid)));     // kid
        }

        cbor_data* cbor_data_recipient_ciphertext = nullptr;
        cose.build_base16str (&cbor_data_recipient_ciphertext, "");

        *recipient  << cbor_data_recipient_protected    // protected
                    << cbor_data_recipient_unprotected  // unprotected
                    << cbor_data_recipient_ciphertext;  // ciphertext
    }
    *recipients << recipient;

    cbor_dump (root, "rfc8152_c_3_1.cbor", "RFC 8152 C.3.1.  Direct ECDH");

    root->release ();
}

void test_rfc8152_c_3_2 ()
{
    _test_case.begin ("RFC 8152 C.3");
    cbor_publisher publisher;

    cbor_object_signing_encryption cose;

    cbor_array* root = new cbor_array ();
    root->tag (true, cbor_tag_t::cose_tag_encrypt);

    cbor_data* cbor_data_protected = nullptr;
    {
        cose_item_t item;
        cose_list_t list_protected;
        variant_set_int16 (item.key, cose_header_t::cose_header_alg);
        variant_set_int16 (item.value, cose_alg_t::cose_aes_ccm_16_64_128);
        list_protected.push_back (item);
        cose.build_protected (&cbor_data_protected, list_protected);
    }

    cbor_map* cbor_data_unprotected = nullptr;
    {
        cose_item_t item;
        cose_list_t list_protected;
        variant_set_int16 (item.key, cose_header_t::cose_header_iv);
        binary_t b16;
        b16 = base16_decode ("89f52f65a1c580933b5261a76c");
        variant_set_bstr_new (item.value, &b16[0], b16.size ());
        list_protected.push_back (item);
        cose.build_unprotected (&cbor_data_unprotected, list_protected);
    }

    cbor_data* cbor_data_ciphertext = nullptr;
    cose.build_base16str (&cbor_data_ciphertext, "753548a19b1307084ca7b2056924ed95f2e3b17006dfe931b687b847");

    *root   << cbor_data_protected      // protected
            << cbor_data_unprotected    // unprotected
            << cbor_data_ciphertext     // ciphertext
            << new cbor_array ();       // recipients

    cbor_array* recipients = (cbor_array*) (*root)[3];

    {
        cbor_array* recipient = new cbor_array ();
        cbor_data* cbor_data_recipient_protected = nullptr;
        {
            cose_item_t item;
            cose_list_t list_protected;
            variant_set_int16 (item.key, cose_header_t::cose_header_alg);
            variant_set_int16 (item.value, -10);
            list_protected.push_back (item);
            cose.build_protected (&cbor_data_recipient_protected, list_protected);
        }
        cbor_map* cbor_data_recipient_unprotected = nullptr;
        {
            cose_item_t item1;
            cose_item_t item2;
            cose_list_t list_unprotected;
            variant_set_int16 (item1.key, -20);
            variant_set_bstr_new (item1.value, "aabbccddeeffgghh", 16);
            list_unprotected.push_back (item1);
            variant_set_int16 (item2.key, cose_header_t::cose_header_kid);
            variant_set_bstr_new (item2.value, "our-secret", 10);
            list_unprotected.push_back (item2);
            cose.build_unprotected (&cbor_data_recipient_unprotected, list_unprotected);
        }

        *recipient  << cbor_data_recipient_protected        // protected
                    << cbor_data_recipient_unprotected      // unprotected
                    << new cbor_data (base16_decode (""));  // ciphertext

        *recipients << recipient;
    }

    cbor_dump (root, "rfc8152_c_3_2.cbor", "RFC 8152 C.3.2.  Direct Plus Key Derivation");

    root->release ();
}

void test_rfc8152_c_3_3 ()
{
    _test_case.begin ("RFC 8152 C.3");
    cbor_publisher publisher;

    cbor_object_signing_encryption cose;

    cbor_array* root = new cbor_array ();
    root->tag (true, cbor_tag_t::cose_tag_encrypt);

    cbor_data* cbor_data_protected = nullptr;
    {
        cose_item_t item;
        cose_list_t list_protected;
        variant_set_int16 (item.key, cose_header_t::cose_header_alg);
        variant_set_int16 (item.value, 1);
        list_protected.push_back (item);
        cose.build_protected (&cbor_data_protected, list_protected);
    }

    constexpr char constexpr_ciphertext[] = "7adbe2709ca818fb415f1e5df66f4e1a51053ba6d65a1a0c52a357da7a644b8070a151b0";
    cbor_data* cbor_data_ciphertext = nullptr;
    cose.build_base16str (&cbor_data_ciphertext, constexpr_ciphertext);

    *root   << cbor_data_protected      // protected
            << new cbor_map ()          // unprotected
            << cbor_data_ciphertext     // ciphertext
            << new cbor_array ();       // recipients

    cbor_map* header_unprotected = (cbor_map*) (*root)[1];
    {
        cbor_array* countersign = new cbor_array ();
        *header_unprotected << new cbor_pair (cose_header_t::cose_header_iv, new cbor_data (base16_decode ("c9cf4df2fe6c632bf7886413")));
        *header_unprotected << new cbor_pair (cose_header_t::cose_header_counter_sig, countersign);

        cbor_data* cbor_data_countersignature_protected = nullptr;
        {
            cose_item_t item;
            cose_list_t list_protected;
            variant_set_int16 (item.key, cose_header_t::cose_header_alg);
            variant_set_int16 (item.value, -36);
            list_protected.push_back (item);
            cose.build_protected (&cbor_data_countersignature_protected, list_protected);
        }

        cbor_map* cbor_data_countersignature_unprotected = nullptr;
        {
            cose_item_t item;
            cose_list_t list_unprotected;
            variant_set_int16 (item.key, cose_header_t::cose_header_kid);
            variant_set_bstr_new (item.value, "bilbo.baggins@hobbiton.example", 30);
            list_unprotected.push_back (item);
            cose.build_unprotected (&cbor_data_countersignature_unprotected, list_unprotected);
        }

        constexpr char constexpr_signature[] = "00929663c8789bb28177ae28467e66377da12302d7f9594d2999afa5dfa531294f8896f2b6cdf1740014f4c7f1a358e3a6cf57f4ed6fb02fcf8f7aa989f5dfd07f0700a3a7d8f3c604ba70fa9411bd10c2591b483e1d2c31de003183e434d8fba18f17a4c7e3dfa003ac1cf3d30d44d2533c4989d3ac38c38b71481cc3430c9d65e7ddff";
        cbor_data* cbor_data_countersignature_signature = nullptr;
        cose.build_base16str (&cbor_data_countersignature_signature, constexpr_signature);

        *countersign    << cbor_data_countersignature_protected     // protected
                        << cbor_data_countersignature_unprotected   // unprotected
                        << cbor_data_countersignature_signature;    // signature

    }

    cbor_array* recipients = (cbor_array*) (*root)[3];
    {
        cbor_array* recipient = new cbor_array ();

        cbor_data* cbor_data_recipient_protected = nullptr;
        {
            cose_item_t item;
            cose_list_t list_protected;
            variant_set_int16 (item.key, cose_header_t::cose_header_alg);
            variant_set_int16 (item.value, -25);
            list_protected.push_back (item);
            cose.build_protected (&cbor_data_recipient_protected, list_protected);
        }

        cbor_data* cbor_data_recipient_ciphertext = nullptr;
        cose.build_base16str (&cbor_data_recipient_ciphertext, "");

        *recipient  << cbor_data_recipient_protected    // protected (alg)
                    << new cbor_map ()                  // unprotected (epk, kid)
                    << cbor_data_recipient_ciphertext;  // ciphertext

        cbor_map* recipient_unprotected = (cbor_map*) (*recipient)[1];
        {
            constexpr char constexpr_x[] = "98f50a4ff6c05861c8860d13a638ea56c3f5ad7590bbfbf054e1c7b4d91d6280";
            constexpr char constexpr_kid[] = "meriadoc.brandybuck@buckland.example";
            cbor_map* ephemeral = new cbor_map ();
            *ephemeral
                << new cbor_pair (1, new cbor_data (2))                             // kty
                << new cbor_pair (-1, new cbor_data (1))                            // crv
                << new cbor_pair (-2, new cbor_data (base16_decode (constexpr_x)))  // x
                << new cbor_pair (-3, new cbor_data (true));                        // y

            *recipient_unprotected
                << new cbor_pair (cose_alg_param_t::cose_ephemeral_key, ephemeral)                              // epk
                << new cbor_pair (cose_header_t::cose_header_kid, new cbor_data (convert (constexpr_kid)));     // kid
        }

        *recipients << recipient;
    }

    cbor_dump (root, "rfc8152_c_3_3.cbor", "RFC 8152 C.3.3.  Counter Signature on Encrypted Content");

    root->release ();
}

void test_rfc8152_c_3_4 ()
{
    _test_case.begin ("RFC 8152 C.3");
}

void test_rfc8152_c_4_1 ()
{
    _test_case.begin ("RFC 8152 C.4");
}

void test_rfc8152_c_4_2 ()
{
    _test_case.begin ("RFC 8152 C.4");
}

void test_rfc8152_c_5_1 ()
{
    _test_case.begin ("RFC 8152 C.5");
}

void test_rfc8152_c_5_2 ()
{
    _test_case.begin ("RFC 8152 C.5");
}

void test_rfc8152_c_5_3 ()
{
    _test_case.begin ("RFC 8152 C.5");
}

void test_rfc8152_c_5_4 ()
{
    _test_case.begin ("RFC 8152 C.5");
}

void test_rfc8152_c_6_1 ()
{
    _test_case.begin ("RFC 8152 C.6");
    // C.6.1.  Shared Secret Direct MAC
    cbor_publisher publisher;

    cbor_object_signing_encryption cose;

    cbor_array* root = new cbor_array ();
    root->tag (true, cbor_tag_t::cose_tag_mac0);

    cbor_data* cbor_data_protected = nullptr;
    {
        cose_item_t item;
        cose_list_t list_protected;
        variant_set_int16 (item.key, cose_header_t::cose_header_alg);
        variant_set_int16 (item.value, cose_alg_t::cose_aes_cbc_mac_256_64);
        list_protected.push_back (item);
        cose.build_protected (&cbor_data_protected, list_protected);
    }

    cbor_data* cbor_data_payload = nullptr;
    cose.build_payload (&cbor_data_payload, "This is the content.");

    cbor_data* cbor_data_tag = nullptr;
    cose.build_base16str (&cbor_data_tag, "726043745027214f");

    *root   << cbor_data_protected  // protected
            << new cbor_map ()      // unprotected
            << cbor_data_payload    // payload
            << cbor_data_tag;       // tag

    cbor_dump (root, "rfc8152_c_6_1.cbor", "RFC 8152 C.6.1.  Shared Secret Direct MAC");

    root->release ();
}

void test_rfc8152_c_7_1 ()
{
    _test_case.begin ("RFC 8152 C.7");
}

void test_rfc8152_c_7_2 ()
{
    _test_case.begin ("RFC 8152 C.7");
}

return_t BIO_to_string (BIO* bio, std::string& data)
{
    return_t ret = errorcode_t::success;

    __try2
    {
        data.clear ();

        if (nullptr == bio) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        std::vector<char> buf;
        char temp[16];
        int l = 0;
        while (1) {
            l = BIO_read (bio, temp, sizeof (temp));
            if (0 >= l) {
                break;
            }
            buf.insert (buf.end (), temp, temp + l);
        }
        data.append (&buf[0], buf.size ());
    }
    __finally2
    {
        // do nothing
    }
    return ret;
}

return_t EVP_PKEY_public_to_string (EVP_PKEY* pkey, std::string& data, int indent)
{
    return_t ret = errorcode_t::success;
    BIO* bio = nullptr;

    __try2
    {
        data.clear ();

        if (nullptr == pkey) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        bio = BIO_new (BIO_s_mem ());
        EVP_PKEY_print_public (bio, pkey, indent, nullptr);

        BIO_to_string (bio, data);
    }
    __finally2
    {
        if (nullptr != bio) {
            BIO_free (bio);
        }
    }

    return ret;
}

void dump_crypto_key (crypto_key_object_t* key, void*)
{
    uint32 nid = 0;
    std::string temp;

    nidof_evp_pkey (key->pkey, nid);
    printf ("nid %i kid %s alg %s use %i\n", nid, key->kid.c_str (), key->alg.c_str (), key->use);
    EVP_PKEY_public_to_string (key->pkey, temp, 0);
    printf ("%s\n", temp.c_str ());
}

void try_refactor_jose_sign ()
{
    crypto_key key;

    key.generate (crypto_key_t::hmac_key, 256, "sample");
    key.generate (crypto_key_t::rsa_key, 2048, "sample");
    key.generate (crypto_key_t::ec_key, 256, "sample");

    key.generate (crypto_key_t::hmac_key, 256, "HS256");
    key.generate (crypto_key_t::rsa_key, 2048, "RS256");
    key.generate (crypto_key_t::rsa_key, 2048, "RS384");
    key.generate (crypto_key_t::rsa_key, 2048, "RS512");
    key.generate (crypto_key_t::ec_key, 256, "ES256");
    key.generate (crypto_key_t::ec_key, 384, "ES384");
    key.generate (crypto_key_t::ec_key, 521, "ES512");

    // dump
    key.for_each (dump_crypto_key, nullptr);
    json_web_key jwk;
    size_t size = 0;
    std::vector<char> bin;
    jwk.write (&key, &bin[0], &size);
    bin.resize (size);
    jwk.write (&key, &bin[0], &size);
    printf ("%.*s\n", size, &bin[0]);

    // JWS
    constexpr char contents[] = "This is the content.";
    std::string jws;

    jose_context_t* handle = nullptr;
    json_object_signing_encryption jose;
    jose.open (&handle, &key);
    jose.sign (handle, jws_t::jws_es256, contents, jws);
    jose.close (handle);

    printf ("contents %s\njws      %s\n", contents, jws.c_str ());
}

int main ()
{
    // check format
    // install
    //      pacman -S rubygems (MINGW)
    //      yum install rubygems (RHEL)
    //      gem install cbor-diag
    // diag2cbor.rb < inputfile > outputfile
    // compare
    //      cat outputfile | xxd
    //      xxd -ps outputfile

    // interface design
    // what kind of member methods required ?
    // need more simple ones
    test_rfc8152_c_1_1 ();
    test_rfc8152_c_1_2 ();
    test_rfc8152_c_1_3 ();
    test_rfc8152_c_1_4 ();
    test_rfc8152_c_2_1 ();
    test_rfc8152_c_3_1 ();
    test_rfc8152_c_3_2 ();
    test_rfc8152_c_3_3 ();
    test_rfc8152_c_3_4 ();
    test_rfc8152_c_4_1 ();
    test_rfc8152_c_4_2 ();
    test_rfc8152_c_5_1 ();
    test_rfc8152_c_5_2 ();
    test_rfc8152_c_5_3 ();
    test_rfc8152_c_5_4 ();
    test_rfc8152_c_6_1 ();
    test_rfc8152_c_7_1 ();
    test_rfc8152_c_7_2 ();

    // and then refactor JOSE
    // try_refactor_jose_sign ();

    _test_case.report (5);
    return _test_case.result ();
}
