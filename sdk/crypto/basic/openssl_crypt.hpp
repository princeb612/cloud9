/* vim: set tabstop=4 shiftwidth=4 softtabstop=4 expandtab smarttab : */
/**
 * @file {file}
 * @author Soo Han, Kim (princeb612.kr@gmail.com)
 * @desc
 *  RFC 3394 Advanced Encryption Standard (AES) Key Wrap Algorithm (September 2002)
 *  RFC 5649 Advanced Encryption Starndard (AES) Key Wrap with Padding Algorithm (September 2009)
 *  RFC 8017 PKCS #1: RSA Cryptography Specifications Version 2.2
 *
 * Revision History
 * Date         Name                Description
 * 2009.06.18   Soo Han, Kim        implemented (codename.merlin)
 */

#ifndef __HOTPLACE_SDK_CRYPTO_OPENSSL_CRYPT__
#define __HOTPLACE_SDK_CRYPTO_OPENSSL_CRYPT__

#include <sdk/crypto/basic/types.hpp>
#include <sdk/crypto/crypto.hpp>

namespace hotplace {
namespace crypto {

typedef struct _encrypt_option_t {
    crypt_ctrl_t ctrl;
    uint16 value;
} encrypt_option_t;

/**
 * @brief openssl_crypt
 * @example
 *          constexpr byte_t data_plain[] = "still a man hears what he wants to hear and disregards the rest";
 *          size_t size_plain = RTL_NUMBER_OF (data_plain);
 *
 *          openssl_crypt crypt;
 *          crypt_context_t* handle = nullptr;
 *          binary_t data_encrypted;
 *          binary_t data_decrypted;
 *
 *          // key
 *          binary_t key;
 *          binary_t iv;
 *          key.resize (32);
 *          iv.resize (32);
 *          for (int i = 0; i < 32; i++) {
 *              key[i] = i;
 *              iv[i] = i;
 *          }
 *
 *          // block cipher
 *          {
 *              crypt.open (&handle, crypt_algorithm_t::aes256, crypt_mode_t::cbc, &key[0], key.size (), &iv[0], iv.size ());
 *              crypt.encrypt (handle, data_plain, size_plain, data_encrypted);
 *              crypt.decrypt (handle, &data_encrypted[0], data_encrypted.size (), data_decrypted);
 *              crypt.close (handle);
 *          }
 *
 *          // AEAD
 *          {
 *              binary_t aad;
 *              binary_t tag;
 *              openssl_prng rand;
 *              rand.random (aad, 32);
 *              crypt.open (&handle, crypt_algorithm_t::aes256, crypt_mode_t::gcm, &key[0], key.size (), &iv[0], iv.size ());
 *              crypt.encrypt2 (handle, data_plain, size_plain, data_encrypted, &aad, &tag);
 *              crypt.decrypt2 (handle, &data_encrypted[0], data_encrypted.size (), data_decrypted, &aad, &tag);
 *              crypt.close (handle);
 *          }
 */
class openssl_crypt : public crypt_t {
   public:
    /**
     * @brief constructor
     */
    openssl_crypt();
    /**
     * @brief destructor
     */
    virtual ~openssl_crypt();

    /**
     * @brief create a context handle (symmetric)
     * @param crypt_context_t** handle [out]
     * @param crypt_algorithm_t algorithm [in]
     * @param crypt_mode_t mode [in]
     * @param const unsigned char* key [in]
     * @param unsigned size_key [in]
     * @param const unsigned char* iv [in] see openssl_chacha20_iv in case of crypt_algorithm_t::chacha20
     * @param unsigned size_iv [in]
     * @return error code (see error.hpp)
     * @example
     *        openssl_crypt crypt;
     *        crypt_context_t* handle = nullptr;
     *        crypt.open(&handle, crypt_algorithm_t::aes256, crypt_mode_t::cbc, key, size_key, iv, size_iv);
     *        crypt.close(handle);
     */
    virtual return_t open(crypt_context_t** handle, crypt_algorithm_t algorithm, crypt_mode_t mode, const unsigned char* key, unsigned size_key,
                          const unsigned char* iv, unsigned size_iv);
    /**
     * @brief create a context handle (symmetric)
     * @param crypt_context_t** handle [out]
     * @param crypt_algorithm_t algorithm [in]
     * @param crypt_mode_t mode [in]
     * @param binary_t const& key [in]
     * @param binary_t const& iv [in]
     * @return error code (see error.hpp)
     */
    virtual return_t open(crypt_context_t** handle, crypt_algorithm_t algorithm, crypt_mode_t mode, binary_t const& key, binary_t const& iv);
    /**
     * @brief create a context handle (symmetric)
     * @param crypt_context_t** handle [out]
     * @param const char* cipher [in] ex. "aes-128-cbc"
     * @param binary_t const& key [in]
     * @param binary_t const& iv [in]
     * @return error code (see error.hpp)
     */
    virtual return_t open(crypt_context_t** handle, const char* cipher, binary_t const& key, binary_t const& iv);
    /**
     * @brief destroy a context handle
     * @param crypt_context_t* handle [in]
     * @return error code (see error.hpp)
     */
    virtual return_t close(crypt_context_t* handle);
    /**
     * @brief set
     * @param crypt_context_t* handle [in]
     * @param crypt_ctrl_t id [in]
     * @param uint16 param [in]
     */
    virtual return_t set(crypt_context_t* handle, crypt_ctrl_t id, uint16 param);

    /**
     * @brief symmetric encrypt
     * @param crypt_context_t* handle [in]
     * @param const unsigned char* data_plain [in]
     * @param size_t size_plain [in]
     * @param unsigned char** data_encrypted [out]
     * @param size_t* size_encrypted [out]
     * @return error code (see error.hpp)
     * @example
     *        crypt.encrypt(handle, data_plain, size_plain, &data_encrypted, &size_encrypted);
     *        crypt.free_data(data_encrypted);
     */
    virtual return_t encrypt(crypt_context_t* handle, const unsigned char* data_plain, size_t size_plain, unsigned char** data_encrypted,
                             size_t* size_encrypted);
    /**
     * @brief symmetric encrypt
     * @param crypt_context_t* handle [in]
     * @param const unsigned char* data_plain [in]
     * @param size_t size_plain [in]
     * @param binary_t& out_encrypted [out]
     * @return error code (see error.hpp)
     * @example
     *        crypt.encrypt(handle, data_plain, size_plain, data_encrypted);
     */
    virtual return_t encrypt(crypt_context_t* handle, const unsigned char* data_plain, size_t size_plain, binary_t& out_encrypted);
    /**
     * @brief encrypt
     * @param crypt_context_t* handle [in]
     * @param binary_t const& input [in]
     * @param binary_t& out [out]
     * @return error code (see error.hpp)
     * @example
     */
    virtual return_t encrypt(crypt_context_t* handle, binary_t const& input, binary_t& out);
    /**
     * @brief encrypt (GCM/CCM)
     * @param crypt_context_t* handle [in]
     * @param const unsigned char* data_plain
     * @param size_t size_plain [in]
     * @param binary_t& out_encrypte [out]
     * @param binary_t* aad [inopt]
     * @param binary_t* tag [outopt]
     */
    virtual return_t encrypt2(crypt_context_t* handle, const unsigned char* data_plain, size_t size_plain, binary_t& out_encrypted,
                              const binary_t* aad = nullptr, binary_t* tag = nullptr);
    /**
     * @brief encrypt (GCM/CCM)
     * @param crypt_context_t* handle [in]
     * @param binary_t const& plain [in]
     * @param binary_t& out_encrypte [out]
     * @param binary_t* aad [inopt]
     * @param binary_t* tag [outopt]
     */
    virtual return_t encrypt2(crypt_context_t* handle, binary_t const& plain, binary_t& out_encrypted, const binary_t* aad = nullptr, binary_t* tag = nullptr);
    /**
     * @brief encrypt
     * @param crypt_context_t* handle [in]
     * @param const unsigned char* data_plain [in]
     * @param size_t size_plain [in]
     * @param unsigned char* out_encrypted [out] allocated buffer
     * @param size_t* size* size_encrypted [inout] should be at least size_encrypted + EVP_MAX_BLOCK_LENGTH
     * @param binary_t* aad [inopt]
     * @param binary_t* tag [inopt]
     */
    return_t encrypt2(crypt_context_t* handle, const unsigned char* data_plain, size_t size_plain, unsigned char* out_encrypted, size_t* size_encrypted,
                      const binary_t* aad = nullptr, binary_t* tag = nullptr);
    /**
     * @brief symmetric decrypt
     * @param crypt_context_t* handle [in]
     * @param const unsigned char* data_encrypted [in]
     * @param size_t size_encrypted [in]
     * @param unsigned char** data_plain [out]
     * @param size_t* size_plain [out]
     * @return error code (see error.hpp)
     * @example
     *        crypt.decrypt(handle, data_encrypted, size_encrypted, &data_decrypted, &size_decrypted);
     *        crypt.free_data(data_decrypted);
     */
    virtual return_t decrypt(crypt_context_t* handle, const unsigned char* data_encrypted, size_t size_encrypted, unsigned char** data_plain,
                             size_t* size_plain);
    /**
     * @brief symmetric decrypt
     * @param crypt_context_t* handle [in]
     * @param const unsigned char* data_encrypted [in]
     * @param size_t size_encrypted [in]
     * @param binary_t& out_decrypted [out]
     * @return error code (see error.hpp)
     * @example
     *        crypt.decrypt(handle, data_encrypted, size_encrypted, data_decrypted);
     */
    virtual return_t decrypt(crypt_context_t* handle, const unsigned char* data_encrypted, size_t size_encrypted, binary_t& out_decrypted);
    /**
     * @brief decrypt
     * @param crypt_context_t* handle [in]
     * @param binary_t const& input [in]
     * @param binary_t& out [out]
     * @return error code (see error.hpp)
     */
    virtual return_t decrypt(crypt_context_t* handle, binary_t const& input, binary_t& out);

    /**
     * @brief decrypt (GCM/CCOM)
     * @param crypt_context_t* handle [in]
     * @param const unsigned char* data_encrypted [in]
     * @param size_t size_encrypted [in]
     * @param binary_t& out_decrypted [out]
     * @param binary_t* aad [inpot]
     * @param binary_t* tag [inopt]
     */
    virtual return_t decrypt2(crypt_context_t* handle, const unsigned char* data_encrypted, size_t size_encrypted, binary_t& out_decrypted,
                              const binary_t* aad = nullptr, const binary_t* tag = nullptr);
    /**
     * @brief decrypt (GCM/CCOM)
     * @param crypt_context_t* handle [in]
     * @param binary_t const& data_encrypted [in]
     * @param binary_t& out_decrypted [out]
     * @param binary_t* aad [inpot]
     * @param binary_t* tag [inopt]
     */
    virtual return_t decrypt2(crypt_context_t* handle, binary_t const& data_encrypted, binary_t& out_decrypted, const binary_t* aad = nullptr,
                              const binary_t* tag = nullptr);
    /**
     * @brief decrypt
     * @param crypt_context_t* handle [in]
     * @param const unsigned char* data_encrypted [in]
     * @param size_t size_encrypted [in]
     * @param byte_t* out_decrypted [out] allocated buffer
     * @param size_t* size_decrypted [inout] should be at least size_encrypted + EVP_MAX_BLOCK_LENGTH
     * @param binary_t* aad [inopt]
     * @param binary_t* tag [inopt]
     */
    return_t decrypt2(crypt_context_t* handle, const unsigned char* data_encrypted, size_t size_encrypted, byte_t* out_decrypted, size_t* size_decrypted,
                      const binary_t* aad = nullptr, const binary_t* tag = nullptr);
    /**
     * @brief free memory
     * @remarks see encrypt, decrypt
     */
    virtual return_t free_data(unsigned char* data);

    /**
     * @biref asymmetric encrypt
     * @param const EVP_PKEY* pkey [in]
     * @param binary_t const& input [in]
     * @param binary_t& output [out]
     * @param crypt_enc_t mode [in]
     */
    return_t encrypt(const EVP_PKEY* pkey, binary_t const& input, binary_t& output, crypt_enc_t mode);
    /**
     * @biref asymmetric decrypt
     * @param const EVP_PKEY* pkey [in]
     * @param binary_t const& input [in]
     * @param binary_t& output [out]
     * @param crypt_enc_t mode [in]
     */
    return_t decrypt(const EVP_PKEY* pkey, binary_t const& input, binary_t& output, crypt_enc_t mode);

    /**
     * @brief simple api
     * @example
     *      encrypt_option_t options[] = {
     *          { crypt_ctrl_padding, 0 }, { },
     *      };
     *      encrypt("aes-128-cbc", cek, iv, plaintext, ciphertext, options);
     */
    return_t encrypt(const char* alg, binary_t const& key, binary_t const& iv, binary_t const& plaintext, binary_t& ciphertext,
                     encrypt_option_t* options = nullptr);
    return_t encrypt(crypt_algorithm_t algorithm, crypt_mode_t mode, binary_t const& key, binary_t const& iv, binary_t const& plaintext, binary_t& ciphertext,
                     encrypt_option_t* options = nullptr);
    return_t encrypt(const char* alg, binary_t const& key, binary_t const& iv, binary_t const& plaintext, binary_t& ciphertext, binary_t const& aad,
                     binary_t& tag, encrypt_option_t* options = nullptr);
    return_t encrypt(crypt_algorithm_t algorithm, crypt_mode_t mode, binary_t const& key, binary_t const& iv, binary_t const& plaintext, binary_t& ciphertext,
                     binary_t const& aad, binary_t& tag, encrypt_option_t* options = nullptr);
    return_t decrypt(const char* alg, binary_t const& key, binary_t const& iv, binary_t const& ciphertext, binary_t& plaintext,
                     encrypt_option_t* options = nullptr);
    return_t decrypt(crypt_algorithm_t algorithm, crypt_mode_t mode, binary_t const& key, binary_t const& iv, binary_t const& ciphertext, binary_t& plaintext,
                     encrypt_option_t* options = nullptr);
    return_t decrypt(const char* alg, binary_t const& key, binary_t const& iv, binary_t const& ciphertext, binary_t& plaintext, binary_t const& aad,
                     binary_t const& tag, encrypt_option_t* options = nullptr);
    return_t decrypt(crypt_algorithm_t algorithm, crypt_mode_t mode, binary_t const& key, binary_t const& iv, binary_t const& ciphertext, binary_t& plaintext,
                     binary_t const& aad, binary_t const& tag, encrypt_option_t* options = nullptr);

    /**
     * @brief deprecated - expect block operation size
     * @param crypt_context_t* handle [in]
     * @param size_t size_data [in]
     * @param size_t* size_expect [out]
     * @return error code (see error.hpp)
     */
    // virtual return_t expect(crypt_context_t* handle, size_t size_data, size_t* size_expect);
    /**
     * @brief crypt_poweredby_t
     * @return see crypt_poweredby_t
     */
    virtual crypt_poweredby_t get_type();

    /**
     * @brief query
     * @param crypt_context_t* handle [in]
     * @param size_t cmd [in] 1 key size, 2 iv size
     * @param size_t& value [out]
     */
    virtual return_t query(crypt_context_t* handle, size_t cmd, size_t& value);
};

/**
 * https://www.ietf.org/archive/id/draft-mcgrew-aead-aes-cbc-hmac-sha2-05.txt
 * 2.4 AEAD_AES_128_CBC_HMAC_SHA_256 AES-128 SHA-256 K 32 MAC_KEY_LEN 16 ENC_KEY_LEN 16 T_LEN=16
 * 2.5 AEAD_AES_192_CBC_HMAC_SHA_384 AES-192 SHA-384 K 48 MAC_KEY_LEN 24 ENC_KEY_LEN 24 T_LEN=24
 * 2.6 AEAD_AES_256_CBC_HMAC_SHA_384 AES-256 SHA-384 K 56 MAC_KEY_LEN 32 ENC_KEY_LEN 24 T_LEN=24
 * 2.7 AEAD_AES_256_CBC_HMAC_SHA_512 AES-256 SHA-512 K 64 MAC_KEY_LEN 32 ENC_KEY_LEN 32 T_LEN=32
 */

/**
 * @brief   Authenticated Encryption with AES-CBC and HMAC-SHA
 * @param   const char* enc_alg [in] "aes-128-cbc"
 * @param   const char* mac_alg [in] "sha256"
 * @param   binary_t const& k [in] MAC_KEY || ENC_KEY
 * @param   binary_t const& iv [in] iv
 * @param   binary_t const& a [in] aad
 * @param   binary_t const& p [in] plaintext
 * @param   binary_t& q [out] ciphertext
 * @param   binary_t& t [out] AE tag
 * @desc
 *
 *          K = MAC_KEY || ENC_KEY
 *          MAC_KEY = initial MAC_KEY_LEN bytes of K
 *          ENC_KEY = final ENC_KEY_LEN bytes of K
 *
 * @sa      RFC 7516 Appendix B.  Example AES_128_CBC_HMAC_SHA_256 Computation
 */
return_t aes_cbc_hmac_sha2_encrypt(const char* enc_alg, const char* mac_alg, binary_t const& k, binary_t const& iv, binary_t const& a, binary_t const& p,
                                   binary_t& q, binary_t& t);
return_t aes_cbc_hmac_sha2_encrypt(crypt_algorithm_t enc_alg, crypt_mode_t enc_mode, hash_algorithm_t mac_alg, binary_t const& k, binary_t const& iv,
                                   binary_t const& a, binary_t const& p, binary_t& q, binary_t& t);

/**
 * @brief   Authenticated Encryption with AES-CBC and HMAC-SHA
 * @desc    each ENC_KEY, MAC_KEY
 */
return_t aes_cbc_hmac_sha2_encrypt(const char* enc_alg, const char* mac_alg, binary_t const& enc_k, binary_t const& mac_k, binary_t const& iv,
                                   binary_t const& a, binary_t const& p, binary_t& q, binary_t& t);
return_t aes_cbc_hmac_sha2_encrypt(crypt_algorithm_t enc_alg, crypt_mode_t enc_mode, hash_algorithm_t mac_alg, binary_t const& enc_k, binary_t const& mac_k,
                                   binary_t const& iv, binary_t const& a, binary_t const& p, binary_t& q, binary_t& t);
/**
 * @brief   Authenticated Encryption with AES-CBC and HMAC-SHA
 * @param   const char* enc_alg [in] "aes-128-cbc"
 * @param   const char* mac_alg [in] "sha256"
 * @param   binary_t const& k [in] MAC_KEY || ENC_KEY
 * @param   binary_t const& iv [in] iv
 * @param   binary_t const& a [in] aad
 * @param   binary_t const& q [in] ciphertext
 * @param   binary_t& p [out] plaintext
 * @param   binary_t& t [in] AE tag
 * @desc
 *          K = MAC_KEY || ENC_KEY
 *          MAC_KEY = initial MAC_KEY_LEN bytes of K
 *          ENC_KEY = final ENC_KEY_LEN bytes of K
 * @sa      RFC 7516 Appendix B.  Example AES_128_CBC_HMAC_SHA_256 Computation
 */
return_t aes_cbc_hmac_sha2_decrypt(const char* enc_alg, const char* mac_alg, binary_t const& k, binary_t const& iv, binary_t const& a, binary_t const& q,
                                   binary_t& p, binary_t const& t);
return_t aes_cbc_hmac_sha2_decrypt(crypt_algorithm_t enc_alg, crypt_mode_t enc_mode, hash_algorithm_t mac_alg, binary_t const& k, binary_t const& iv,
                                   binary_t const& a, binary_t const& q, binary_t& p, binary_t const& t);
/**
 * @brief   Authenticated Encryption with AES-CBC and HMAC-SHA
 * @desc    each ENC_KEY, MAC_KEY
 */
return_t aes_cbc_hmac_sha2_decrypt(const char* enc_alg, const char* mac_alg, binary_t const& enc_k, binary_t const& mac_k, binary_t const& iv,
                                   binary_t const& a, binary_t const& q, binary_t& p, binary_t const& t);
return_t aes_cbc_hmac_sha2_decrypt(crypt_algorithm_t enc_alg, crypt_mode_t enc_mode, hash_algorithm_t mac_alg, binary_t const& enc_k, binary_t const& mac_k,
                                   binary_t const& iv, binary_t const& a, binary_t const& q, binary_t& p, binary_t const& t);

}  // namespace crypto
}  // namespace hotplace

#endif
