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

typedef std::map<int, variant_t> cose_variantmap_t;
typedef std::list<int> cose_orderlist_t;

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
