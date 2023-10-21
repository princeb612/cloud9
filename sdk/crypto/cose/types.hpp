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

#include <hotplace/sdk/base.hpp>
#include <hotplace/sdk/crypto/types.hpp>

namespace hotplace {
namespace crypto {

enum cose_param_t {
    cose_external = 1,
    cose_unsent_apu_id = 2,
    cose_unsent_apu_nonce = 3,
    cose_unsent_apu_other = 4,
    cose_unsent_apv_id = 5,
    cose_unsent_apv_nonce = 6,
    cose_unsent_apv_other = 7,
    cose_unsent_pub_other = 8,
    cose_unsent_priv_other = 9,
    cose_unsent_iv = 10,
    cose_unsent_alg = 11,

    // temporary
    cose_param_aad = 13,
    cose_param_cek = 14,
    cose_param_iv = 15,
};
enum code_debug_flag_t {
    // simply want to know reason why routine is failed from testcase report
    cose_debug_partial_iv = (1 << 1),
    cose_debug_aescmac = (1 << 2),
    cose_debug_chacha20_poly1305 = (1 << 3),
};

typedef std::map<int, variant_t> cose_variantmap_t;
typedef std::list<int> cose_orderlist_t;
typedef std::map<cose_param_t, binary_t> cose_binarymap_t;

typedef struct _cose_parts_t {
    // sign, verify
    uint8 tag;
    binary_t bin_protected;
    binary_t bin_data;
    EVP_PKEY* epk;
    cose_variantmap_t protected_map;
    cose_orderlist_t protected_list;
    cose_variantmap_t unprotected_map;
    cose_orderlist_t unprotected_list;

    _cose_parts_t() : tag(0), epk(nullptr) {}
    void clear_map(cose_variantmap_t& map) {
        cose_variantmap_t::iterator map_iter;
        for (map_iter = map.begin(); map_iter != map.end(); map_iter++) {
            variant_free(map_iter->second);
        }
        map.clear();
    }
    void clear() {
        tag = 0;
        bin_protected.clear();
        bin_data.clear();
        clear_map(protected_map);
        clear_map(unprotected_map);
        protected_list.clear();
        unprotected_list.clear();
        if (epk) {
            EVP_PKEY_free(epk);
            epk = nullptr;
        }
    }
} cose_parts_t;

typedef struct _cose_context_t {
    uint8 tag;
    cose_parts_t body;
    binary_t payload;
    std::list<cose_parts_t> subitems;

    cose_binarymap_t binarymap;
    uint32 debug_flag;

    _cose_context_t() : tag(0), debug_flag(0) {}
    ~_cose_context_t() { clearall(); }
    void clearall() {
        clear();
        binarymap.clear();
        debug_flag = 0;
    }
    void clear_map(cose_variantmap_t& map) {
        cose_variantmap_t::iterator map_iter;
        for (map_iter = map.begin(); map_iter != map.end(); map_iter++) {
            variant_free(map_iter->second);
        }
        map.clear();
    }
    void clear() {
        tag = 0;
        body.clear();
        payload.clear();
        std::list<cose_parts_t>::iterator iter;
        for (iter = subitems.begin(); iter != subitems.end(); iter++) {
            cose_parts_t& item = *iter;
            item.clear();
        }
        subitems.clear();
    }
} cose_context_t;

}  // namespace crypto
}  // namespace hotplace

#endif
