/* vim: set tabstop=4 shiftwidth=4 softtabstop=4 expandtab smarttab : */
/**
 * @file {file}
 * @author Soo Han, Kim (princeb612.kr@gmail.com)
 * @desc
 *
 * Revision History
 * Date         Name                Description
 *
 */

#ifndef __HOTPLACE_SDK_CRYPTO_COSE_CBOROBJECTSIGNINGENCRYPTION__
#define __HOTPLACE_SDK_CRYPTO_COSE_CBOROBJECTSIGNINGENCRYPTION__

#include <sdk/base.hpp>
#include <sdk/crypto/basic/crypto_key.hpp>
#include <sdk/crypto/cose/types.hpp>
#include <sdk/crypto/types.hpp>
#include <sdk/io/cbor/cbor_data.hpp>
#include <sdk/io/cbor/cbor_map.hpp>
#include <sdk/io/cbor/cbor_publisher.hpp>
#include <sdk/io/cbor/cbor_reader.hpp>

namespace hotplace {
using namespace io;
namespace crypto {

class cbor_object_signing_encryption {
    friend class cbor_object_encryption;
    friend class cbor_object_signing;

   public:
    cbor_object_signing_encryption();
    ~cbor_object_signing_encryption();

    /**
     * @brief   open
     * @param   cose_context_t** handle [out] call close to free
     * @return  error code (see error.hpp)
     */
    return_t open(cose_context_t** handle);
    /**
     * @brief   close
     * @param   cose_context_t* handle [in]
     * @return  error code (see error.hpp)
     */
    return_t close(cose_context_t* handle);
    /**
     * @brief   set flags
     * @param   cose_context_t* handle [in]
     * @param   uint32 flags [in] see cose_flag_t
     * @return  error code (see error.hpp)
     * @example
     *      cose.set (handle, cose_flag_t::cose_flag_auto_keygen);
     *      cose.set (handle, cose_flag_t::cose_flag_allow_debug | cose_flag_t::cose_flag_auto_keygen);
     */
    return_t set(cose_context_t* handle, uint32 flags, uint32 debug_flags = 0);
    return_t get(cose_context_t* handle, uint32& flags, uint32& debug_flags);
    /**
     * @brief   set
     * @param   cose_context_t* handle [in]
     * @param   cose_param_t id [in] cose_external, cose_public, cose_private
     * @param   binary_t const& bin [in]
     * @return  error code (see error.hpp)
     */
    return_t set(cose_context_t* handle, cose_param_t id, binary_t const& bin);

    /**
     * @brief   encrypt ("Encrypt0")
     * @param   cose_context_t* handle [in]
     * @param   crypto_key* key [in]
     * @param   cose_alg_t method [in] must specify an encryption algoritm (see cose_group_enc_aesgcm/cose_group_enc_aesccm)
     * @param   binary_t const& input [in]
     * @param   binary_t& output [out]
     * @return  error code (see error.hpp)
     * @example
     *          encrypt (handle, key, cose_aes128gcm, input, output);
     */
    return_t encrypt(cose_context_t* handle, crypto_key* key, cose_alg_t method, binary_t const& input, binary_t& output);
    /**
     * @brief   encrypt ("Encrypt")
     * @param   cose_context_t* handle [in]
     * @param   crypto_key* key [in]
     * @param   std::list<cose_alg_t> methods [in] at least one encryption algorithm
     * @param   binary_t const& input [in]
     * @param   binary_t& output [out]
     * @return  error code (see error.hpp)
     * @example
     *          algs.push_back(cose_aes256gcm); // one of cose_group_enc_xxx
     *          algs.push_back(cose_group_key_ecdhss_hmac); // cose_group_key_xxx
     *          encrypt (handle, key, algs, input, output);
     */
    return_t encrypt(cose_context_t* handle, crypto_key* key, std::list<cose_alg_t> methods, binary_t const& input, binary_t& output);
    /**
     * @brief   encrypt
     * @param   cose_context_t* handle [in]
     * @param   crypto_key* key [in]
     * @param   cose_alg_t* methods [in]
     * @param   size_t size_method [in]
     * @param   binary_t const& input [in]
     * @param   binary_t& output [out]
     * @return  error code (see error.hpp)
     * @example
     *          cose_alg_t algs[] = { cose_aesccm_16_64_256 };
     *          cose.encrypt (handle, key, algs, 1, input, output);
     *          cose_alg_t algs2[] = { cose_aesccm_64_64_256, cose_group_key_ecdhss_hmac, cose_group_key_hkdf_aes, };
     *          encrypt (handle, key, algs2, 2, input, output);
     */
    return_t encrypt(cose_context_t* handle, crypto_key* key, cose_alg_t* methods, size_t size_method, binary_t const& input, binary_t& output);
    /**
     * @brief   decrypt
     * @param   cose_context_t* handle [in]
     * @param   crypto_key* key [in]
     * @param   binary_t const& input [in]
     * @param   bool& result [out]
     * @return  error code (see error.hpp)
     * @remarks see json_object_encryption::decrypt
     */
    return_t decrypt(cose_context_t* handle, crypto_key* key, binary_t const& input, binary_t& output, bool& result);
    /**
     * @brief   sign
     * @param   cose_context_t* handle [in]
     * @param   crypto_key* key [in]
     * @param   cose_alg_t method [in]
     * @param   binary_t const& input [in]
     * @param   binary_t& output [out]
     * @return  error code (see error.hpp)
     * @remarks see json_object_signing::sign
     */
    return_t sign(cose_context_t* handle, crypto_key* key, cose_alg_t method, binary_t const& input, binary_t& output);
    /**
     * @brief   sign
     * @param   cose_context_t* handle [in]
     * @param   crypto_key* key [in]
     * @param   std::list<cose_alg_t> methods [in]
     * @param   binary_t const& input [in]
     * @param   binary_t& output [out]
     * @return  error code (see error.hpp)
     * @remarks see json_object_signing::sign
     */
    return_t sign(cose_context_t* handle, crypto_key* key, std::list<cose_alg_t> methods, binary_t const& input, binary_t& output);
    /**
     * @brief   mac
     * @param   cose_context_t* handle [in]
     * @param   crypto_key* key [in]
     * @param   cose_alg_t method [in]
     * @param   binary_t const& input [in]
     * @param   binary_t& output [out]
     * @return  error code (see error.hpp)
     */
    return_t mac(cose_context_t* handle, crypto_key* key, cose_alg_t method, binary_t const& input, binary_t& output);
    /**
     * @brief   mac
     * @param   cose_context_t* handle [in]
     * @param   crypto_key* key [in]
     * @param   std::list<cose_alg_t> methods [in]
     * @param   binary_t const& input [in]
     * @param   binary_t& output [out]
     * @return  error code (see error.hpp)
     */
    return_t mac(cose_context_t* handle, crypto_key* key, std::list<cose_alg_t> methods, binary_t const& input, binary_t& output);
    /**
     * @brief   verify with kid
     * @param   cose_context_t* handle [in]
     * @param   crypto_key* key [in]
     * @param   binary_t const& input [in]
     * @param   bool& result [out]
     * @return  error code (see error.hpp)
     * @remarks see json_object_signing::verify
     */
    return_t verify(cose_context_t* handle, crypto_key* key, binary_t const& input, bool& result);
    /**
     * @brief   clear
     * @param   cose_context_t* handle [in]
     * @return  error code (see error.hpp)
     */
    static return_t clear_context(cose_context_t* handle);

    /**
     * @brief   parser
     */
    class parser {
       public:
        parser();
        ~parser();

        /**
         * @brief   parse
         * @param   cose_context_t* handle [in]
         * @param   binary_t const& input [in]
         * @return  error code (see error.hpp)
         */
        return_t parse(cose_context_t* handle, binary_t const& input);
        /**
         * @brief   find
         * @param   int key [in]
         * @param   int& value [out]
         * @param   cose_variantmap_t const& from [in]
         */
        bool exist(int key, cose_variantmap_t const& from);
        /**
         * @brief   find
         * @param   int key [in]
         * @param   int& value [out]
         * @param   cose_variantmap_t const& from [in]
         * @return  error code (see error.hpp)
         */
        return_t finditem(int key, int& value, cose_variantmap_t const& from);
        /**
         * @brief   find
         * @param   int key [in]
         * @param   std::string& value [out]
         * @param   cose_variantmap_t const& from [in]
         * @return  error code (see error.hpp)
         */
        return_t finditem(int key, std::string& value, cose_variantmap_t const& from);
        /**
         * @brief   find
         * @param   int key [in]
         * @param   binary_t& value [out]
         * @param   cose_variantmap_t const& from [in]
         * @return  error code (see error.hpp)
         */
        return_t finditem(int key, binary_t& value, cose_variantmap_t const& from);
        /**
         * @brief   compose Enc_structure
         * @param   cose_context_t* handle [in]
         * @param   binary_t& authenticated_data [out]
         * @return  error code (see error.hpp)
         */
        return_t compose_enc_structure(cose_context_t* handle, binary_t& authenticated_data);
        /**
         * @brief   compose the COSE_KDF_Context
         * @param   cose_context_t* handle [in]
         * @param   cose_structure_t& item [in]
         * @param   binary_t& kdf_context [out]
         * @return  error code (see error.hpp)
         * @desc    AlgorithmID: ... This normally is either a key wrap algorithm identifier or a content encryption algorithm identifier.
         */
        return_t compose_kdf_context(cose_context_t* handle, cose_structure_t& item, binary_t& kdf_context);
        /**
         * @brief   compose the ToBeSigned
         * @param   cose_context_t* handle [in]
         * @param   cose_structure_t& item [in]
         * @param   binary_t& tobesigned [out]
         * @return  error code (see error.hpp)
         */
        return_t compose_sig_structure(cose_context_t* handle, cose_structure_t& item, binary_t& tobesigned);
        /**
         * @brief   compose the ToMac
         * @param   cose_context_t* handle [in]
         * @param   binary_t& tomac [out]
         * @return  error code (see error.hpp)
         */
        return_t compose_mac_structure(cose_context_t* handle, binary_t& tomac);

       protected:
        cbor_data* docompose_kdf_context_item(cose_context_t* handle, cose_structure_t& item, cose_key_t key, cose_param_t shared);

        return_t doparse_protected(cose_context_t* handle, cbor_object* object);
        return_t doparse_unprotected(cose_context_t* handle, cbor_object* object);
        return_t doparse_payload(cose_context_t* handle, cbor_object* object);
        return_t doparse_singleitem(cose_context_t* handle, cbor_object* object);
        return_t doparse_multiitems(cose_context_t* handle, cbor_object* object);
        return_t doparse_multiitem(cbor_array* object, cose_structure_t& body);
        /**
         * @brief   read unprotected (cbor_map) to context
         * @param   cbor_map* data [in]
         * @param   cose_structure_t& item [out]
         * @return  error code (see error.hpp)
         */
        return_t doparse_unprotected(cbor_map* data, cose_structure_t& item);
        /**
         * @brief   read bstr of protected (cbor_data) to list
         * @param   binary_t const& data [in]
         * @param   cose_variantmap_t& vtl [out]
         * @return  error code (see error.hpp)
         */
        return_t doparse_binary(binary_t const& data, cose_variantmap_t& vtl);
        /**
         * @brief   read unprotected (cbor_map) to list
         * @param   cbor_map* data [in]
         * @param   cose_variantmap_t& vtl [out]
         * @return  error code (see error.hpp)
         */
        return_t doparse_map(cbor_map* data, cose_variantmap_t& vtl);
    };

   protected:
    /**
     * @brief cek into handle->binarymap[cose_param_t::cose_param_cek]
     * @param cose_context_t* handle [in]
     * @param crypto_key* key [in]
     * @param cose_structure_& item [in] compute cek
     */
    return_t process_keyagreement(cose_context_t* handle, crypto_key* key, cose_structure_t& item, bool do_encrypt);
    return_t preprocess_keyagreement(cose_context_t* handle, crypto_key* key, cose_structure_t& item);

    return_t info_of_strcuture(cose_context_t* handle, cose_structure_t& item, int& alg, std::string& kid);
};

typedef cbor_object_signing_encryption COSE;

}  // namespace crypto
}  // namespace hotplace

#endif
