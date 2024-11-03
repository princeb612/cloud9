/* vim: set tabstop=4 shiftwidth=4 softtabstop=4 expandtab smarttab : */
/**
 * @file {file}
 * @author Soo Han, Kim (princeb612.kr@gmail.com)
 * @desc
 *
 * Revision History
 * Date         Name                Description
 */

#include <sdk/io/basic/payload.hpp>

namespace hotplace {
namespace io {

payload_member::payload_member(uint8 value, const char* name, const char* group) : _change_endian(false), _ref(nullptr), _vl(nullptr), _reserve(0) {
    set_name(name).set_group(group);
    get_variant().set_uint8(value);
}

payload_member::payload_member(uint8 value, uint16 repeat, const char* name, const char* group)
    : _change_endian(false), _ref(nullptr), _vl(nullptr), _reserve(repeat) {
    set_name(name).set_group(group);
    binary_t bin;
    uint16 temp = repeat;
    while (temp--) {
        bin.insert(bin.end(), value);
    }
    get_variant().set_binary_new(bin);
}

payload_member::payload_member(uint16 value, bool change_endian, const char* name, const char* group)
    : _change_endian(change_endian), _ref(nullptr), _vl(nullptr), _reserve(0) {
    set_name(name).set_group(group);
    get_variant().set_uint16(value);
}

payload_member::payload_member(uint32_24_t value, const char* name, const char* group) : _change_endian(true), _ref(nullptr), _vl(nullptr), _reserve(0) {
    set_name(name).set_group(group);
    get_variant().set_uint24(value.get());
}

payload_member::payload_member(uint32 value, bool change_endian, const char* name, const char* group)
    : _change_endian(change_endian), _ref(nullptr), _vl(nullptr), _reserve(0) {
    set_name(name).set_group(group);
    get_variant().set_uint32(value);
}

payload_member::payload_member(uint64 value, bool change_endian, const char* name, const char* group)
    : _change_endian(change_endian), _ref(nullptr), _vl(nullptr), _reserve(0) {
    set_name(name).set_group(group);
    get_variant().set_uint64(value);
}

payload_member::payload_member(uint128 value, bool change_endian, const char* name, const char* group)
    : _change_endian(change_endian), _ref(nullptr), _vl(nullptr), _reserve(0) {
    set_name(name).set_group(group);
    get_variant().set_uint128(value);
}

payload_member::payload_member(const binary_t& value, const char* name, const char* group) : _change_endian(false), _ref(nullptr), _vl(nullptr), _reserve(0) {
    set_name(name).set_group(group);
    get_variant().set_binary_new(value);
}

payload_member::payload_member(const std::string& value, const char* name, const char* group)
    : _change_endian(false), _ref(nullptr), _vl(nullptr), _reserve(0) {
    set_name(name).set_group(group);
    get_variant().set_str_new(value);
}

payload_member::payload_member(const stream_t* value, const char* name, const char* group) : _change_endian(false), _ref(nullptr), _vl(nullptr), _reserve(0) {
    set_name(name).set_group(group);
    get_variant().set_bstr_new(value);
}

payload_member::payload_member(payload_encoded* value, const char* name, const char* group) : _change_endian(false), _ref(nullptr), _vl(value), _reserve(0) {
    set_name(name).set_group(group);
}

payload_member::~payload_member() {
    if (_vl) {
        _vl->release();
    }
}

bool payload_member::get_change_endian() { return _change_endian; }

std::string payload_member::get_name() const { return _name; }

std::string payload_member::get_group() const { return _group; }

bool payload_member::encoded() const { return nullptr != _vl; }

payload_member& payload_member::set_change_endian(bool enable) {
    _change_endian = enable;
    return *this;
}

payload_member& payload_member::set_name(const char* name) {
    if (name) {
        _name = name;
    }
    return *this;
}

payload_member& payload_member::set_group(const char* group) {
    if (group) {
        _group = group;
    }
    return *this;
}

variant& payload_member::get_variant() { return _vt; }

size_t payload_member::get_space() {
    size_t space = 0;
    if (_vl) {
        space = _vl->lsize();
    } else if (_reserve) {
        space = _reserve;
    } else if (variant_flag_t::flag_int == get_variant().flag()) {
        space = get_variant().size();
    } else if (_ref) {
        space = t_to_int<size_t>(_ref);
    }
    return space;
}

size_t payload_member::get_capacity() {
    size_t space = 0;
    if (_vl) {
        space = _vl->lsize();
    } else if (_reserve) {
        space = _reserve;
    } else if (_ref) {
        space = t_to_int<size_t>(_ref);
    } else {
        space = get_variant().size();
    }
    return space;
}

size_t payload_member::get_reference_value() {
    size_t size = 0;
    if (_vl) {
        size = _vl->value();
    } else if (_reserve) {
        size = _reserve;
    } else if (_ref) {
        size = t_to_int<size_t>(_ref);
    } else {
        size = t_to_int<size_t>(this);
    }
    return size;
}

payload_member* payload_member::get_reference_of() { return _ref; }

payload_member& payload_member::set_reference_of(payload_member* member) {
    _ref = member;
    return *this;
}

payload_member& payload_member::write(binary_t& bin) {
    uint32 flags = 0;
    if (_vl) {
        _vl->write(bin);  // delegate
    } else {
        if (get_change_endian()) {
            flags |= variant_convendian;
        }
        get_variant().to_binary(bin, flags);
    }
    return *this;
}

return_t payload_member::doread(const byte_t* ptr, size_t size_ptr, size_t* size_read) {
    return_t ret = errorcode_t::success;
    __try2 {
        if (nullptr == ptr && nullptr == size_read) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        variant& v = get_variant();
        vartype_t type = v.type();
        if (variant_flag_t::flag_int == v.flag()) {
            uint16 size = v.size();
            if (size_ptr >= size) {
                switch (type) {
                    case TYPE_INT8:
                    case TYPE_UINT8: {
                        v.set_uint8(*(uint8*)ptr);
                        *size_read = size;
                    } break;
                    case TYPE_INT16:
                    case TYPE_UINT16: {
                        uint16 temp = *(uint16*)ptr;
                        if (get_change_endian()) {
                            temp = ntoh16(temp);
                        }
                        v.set_uint16(temp);
                        *size_read = size;
                    } break;
                    case TYPE_INT24:
                    case TYPE_UINT24: {
                        uint32 temp = 0;
                        b24_i32(ptr, size_ptr, temp);
                        v.set_uint24(temp);
                        *size_read = size;
                    } break;
                    case TYPE_INT32:
                    case TYPE_UINT32: {
                        uint32 temp = *(uint32*)ptr;
                        if (get_change_endian()) {
                            temp = ntoh32(temp);
                        }
                        v.set_uint32(temp);
                        *size_read = size;
                    } break;
                    case TYPE_INT64:
                    case TYPE_UINT64: {
                        uint64 temp = *(uint64*)ptr;
                        if (get_change_endian()) {
                            temp = ntoh64(temp);
                        }
                        v.set_uint64(temp);
                        *size_read = size;
                    } break;
                    case TYPE_INT128:
                    case TYPE_UINT128: {
                        uint128 temp = *(uint128*)ptr;
                        if (get_change_endian()) {
                            temp = ntoh128(temp);
                        }
                        v.set_uint64(temp);
                        *size_read = size;
                    } break;
                    default:
                        break;
                }
            }
        } else {
            size_t size = 0;
            payload_member* ref = get_reference_of();
            if (_reserve) {
                size = _reserve;
            } else if (ref) {
                size = t_to_int<size_t>(ref);
            }

            if (size_ptr >= size) {
                switch (type) {
                    case TYPE_STRING:
                        v.clear().set_strn_new((char*)ptr, size);
                        *size_read = size;
                        break;
                    case TYPE_BINARY:
                        v.clear().set_bstr_new(ptr, size);
                        *size_read = size;
                        break;
                    default:
                        break;
                }
            }
        }
    }
    __finally2 {
        // do nothing
    }
    return ret;
}

return_t payload_member::doread_encoded(const byte_t* ptr, size_t size_ptr, size_t* size_read) {
    return_t ret = errorcode_t::success;
    __try2 {
        if (nullptr == ptr && nullptr == size_read) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        size_t pos = 0;
        _vl->read(ptr, size_ptr, pos);  // delegate
        *size_read = pos;
    }
    __finally2 {
        // do nothing
    }
    return ret;
}

payload_member& payload_member::read(const byte_t* ptr, size_t size_ptr, size_t* size_read) {
    __try2 {
        if (nullptr == ptr || 0 == size_ptr || nullptr == size_read) {
            __leave2;
        }

        if (get_payload_encoded()) {
            doread_encoded(ptr, size_ptr, size_read);
        } else {
            doread(ptr, size_ptr, size_read);
        }
    }
    __finally2 {
        // do nothing
    }
    return *this;
}

payload_member& payload_member::reserve(uint16 size) {
    _reserve = size;
    return *this;
}

payload_encoded* payload_member::get_payload_encoded() { return _vl; }

}  // namespace io
}  // namespace hotplace
