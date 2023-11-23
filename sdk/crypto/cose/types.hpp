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

#ifndef __HOTPLACE_SDK_CRYPTO_COSE_TYPES__
#define __HOTPLACE_SDK_CRYPTO_COSE_TYPES__

#include <sdk/base.hpp>
#include <sdk/crypto/basic/crypto_key.hpp>
#include <sdk/crypto/types.hpp>
#include <sdk/io/cbor/cbor_array.hpp>
#include <sdk/io/cbor/cbor_data.hpp>
#include <sdk/io/cbor/cbor_encode.hpp>
#include <sdk/io/cbor/cbor_map.hpp>
#include <sdk/io/cbor/cbor_publisher.hpp>
#include <sdk/io/cbor/cbor_reader.hpp>
#include <sdk/io/cbor/concise_binary_object_representation.hpp>

namespace hotplace {
using namespace io;
namespace crypto {

typedef struct _cose_context_t cose_context_t;
// typedef struct _cose_structure_t cose_structure_t;

enum cose_param_t {
    cose_param_base = 0x1000,

    cose_external = cose_param_base + 1,
    cose_unsent_apu_id = cose_param_base + 2,
    cose_unsent_apu_nonce = cose_param_base + 3,
    cose_unsent_apu_other = cose_param_base + 4,
    cose_unsent_apv_id = cose_param_base + 5,
    cose_unsent_apv_nonce = cose_param_base + 6,
    cose_unsent_apv_other = cose_param_base + 7,
    cose_unsent_pub_other = cose_param_base + 8,
    cose_unsent_priv_other = cose_param_base + 9,
    cose_unsent_iv = cose_param_base + 10,
    cose_unsent_alg = cose_param_base + 11,

    cose_param_aad = cose_param_base + 13,
    cose_param_cek = cose_param_base + 14,
    cose_param_context = cose_param_base + 15,
    cose_param_iv = cose_param_base + 16,
    cose_param_kek = cose_param_base + 17,
    cose_param_salt = cose_param_base + 18,
    cose_param_secret = cose_param_base + 19,
    cose_param_tobesigned = cose_param_base + 20,
    cose_param_tomac = cose_param_base + 21,
    cose_param_apu_id = cose_param_base + 22,
    cose_param_apu_nonce = cose_param_base + 23,
    cose_param_apu_other = cose_param_base + 24,
    cose_param_apv_id = cose_param_base + 25,
    cose_param_apv_nonce = cose_param_base + 26,
    cose_param_apv_other = cose_param_base + 27,
    cose_param_pub_other = cose_param_base + 28,
    cose_param_priv_other = cose_param_base + 29,
    cose_param_ciphertext = cose_param_base + 30,
};

enum cose_flag_t {
    cose_flag_allow_debug = (1 << 1),
    cose_flag_auto_keygen = (1 << 2),

    // debug
    cose_debug_notfound_key = (1 << 16),
    cose_debug_partial_iv = (1 << 17),
};

typedef std::list<int> cose_orderlist_t;
typedef std::map<cose_param_t, binary_t> cose_binarymap_t;
typedef std::map<int, variant_t> cose_variantmap_t;

static inline void cose_variantmap_copy(cose_variantmap_t& target, cose_variantmap_t& source) {
    variant_t vt;
    cose_variantmap_t::iterator map_iter;
    for (map_iter = source.begin(); map_iter != source.end(); map_iter++) {
        int key = map_iter->first;
        variant_t& value = map_iter->second;
        variant_copy(vt, value);
        target.insert(std::make_pair(key, vt));
    }
}

static inline void cose_variantmap_move(cose_variantmap_t& target, cose_variantmap_t& source) {
    variant_t vt;
    cose_variantmap_t::iterator map_iter;
    for (map_iter = source.begin(); map_iter != source.end(); map_iter++) {
        int key = map_iter->first;
        variant_t& value = map_iter->second;
        variant_move(vt, value);
        target.insert(std::make_pair(key, vt));
    }
    source.clear();
}

static inline void cose_variantmap_free(cose_variantmap_t& map) {
    cose_variantmap_t::iterator map_iter;
    for (map_iter = map.begin(); map_iter != map.end(); map_iter++) {
        variant_t& value = map_iter->second;
        variant_free(value);
    }
    map.clear();
}

class cose_composer;
struct _cose_context_t {
    uint32 flags;
    uint32 debug_flags;
    basic_stream debug_stream;

    // restructuring in progress
    cose_composer* composer;

    _cose_context_t() : flags(0), debug_flags(0) {
        // composer = new cose_composer;
    }
    ~_cose_context_t() {
        clearall();
        // delete composer;
    }
    void clearall() {
        clear();
        flags = 0;
        debug_flags = 0;
        debug_stream.clear();
    }
    void clear() {
        // do nothing
    }
};

}  // namespace crypto
}  // namespace hotplace

#endif
