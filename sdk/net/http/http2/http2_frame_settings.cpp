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
#include <sdk/io/basic/zlib.hpp>
#include <sdk/io/string/string.hpp>
#include <sdk/io/system/types.hpp>
#include <sdk/net/http/http2/http2_frame.hpp>
#include <sdk/net/http/http2/http2_protocol.hpp>
#include <sdk/net/http/http_resource.hpp>
#include <sdk/net/tls/tls.hpp>

namespace hotplace {
using namespace io;
namespace net {

http2_frame_settings::http2_frame_settings() : http2_frame(h2_frame_t::h2_frame_settings) {}

http2_frame_settings::http2_frame_settings(const http2_frame_settings& rhs) : http2_frame(rhs) { _settings = rhs._settings; }

http2_frame_settings& http2_frame_settings::add(uint16 id, uint32 value) {
    h2_setting_map_pib_t pib = _settings.insert(std::make_pair(id, value));
    if (false == pib.second) {
        pib.first->second = value;
    }
    return *this;
}

return_t http2_frame_settings::read(http2_frame_header_t const* header, size_t size) {
    return_t ret = errorcode_t::success;
    __try2 {
        if (nullptr == header) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        ret = http2_frame::read(header, size);
        if (errorcode_t::success != ret) {
            __leave2;
        }

        uint32 len = get_payload_size();
        if ((size < get_frame_size()) || (len % sizeof(http2_setting_t))) {
            ret = errorcode_t::bad_data;
            __leave2;
        }

        byte_t* payload = nullptr;
        ret = get_payload(header, size, &payload);
        if (errorcode_t::success != ret) {
            __leave2;
        }

        uint32 pos = 0;
        while (pos < len) {
            http2_setting_t* setting = (http2_setting_t*)(payload + pos);

            uint16 id = ntoh16(setting->id);
            uint32 value = ntoh32(setting->value);
            add(id, value);

            pos += sizeof(http2_setting_t);
        }
    }
    __finally2 {
        // do nothing
    }
    return ret;
}

return_t http2_frame_settings::write(binary_t& frame) {
    return_t ret = errorcode_t::success;

    uint32 len = _settings.size() * sizeof(http2_setting_t);
    ret = set_payload_size(len);

    if (errorcode_t::success == ret) {
        http2_frame::write(frame);

        // RFC 7540 Figure 10: Setting Format
        h2_setting_map_t::iterator iter;
        for (iter = _settings.begin(); iter != _settings.end(); iter++) {
            binary_append(frame, iter->first, hton16);
            binary_append(frame, iter->second, hton32);
        }
    }

    return ret;
}

void http2_frame_settings::dump(stream_t* s) {
    if (s) {
        http2_frame::dump(s);

        h2_setting_map_t::iterator iter;
        for (iter = _settings.begin(); iter != _settings.end(); iter++) {
            s->printf(" > ");
            s->printf("%s %u ", constexpr_frame_identifier, iter->first);
            s->printf("%s %u (0x%08x) ", constexpr_frame_value, iter->second, iter->second);
            s->printf("\n");
        }
    }
}

}  // namespace net
}  // namespace hotplace
