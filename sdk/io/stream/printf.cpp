/* vim: set tabstop=4 shiftwidth=4 softtabstop=4 expandtab smarttab : */
/**
 * @file {file}
 * @author Soo han, Kim (princeb612.kr@gmail.com)
 * @desc
 *
 * Revision History
 * Date         Name                Description
 */

#include <hotplace/sdk/io/string/string.hpp>
#include <hotplace/sdk/io/string/valist.hpp>
#include <hotplace/sdk/io/basic/base16.hpp>
#include <hotplace/sdk/io/stream/stream.hpp>
#include <hotplace/sdk/io/stream/string.hpp>
#include <map>

namespace hotplace {
namespace io {

typedef struct _variant_conversion_t {
    int type;
    const char* formatter;
} variant_conversion_t;

#if __cplusplus >= 201103L    // c++11
#define VARIANT_CONVERSION_ITEM(t, f) { .type = t, .formatter = f, \
}
#else
#define VARIANT_CONVERSION_ITEM(t, f) { t, f, }
#endif

static variant_conversion_t type_formatter[] =
{
    VARIANT_CONVERSION_ITEM (TYPE_CHAR,   "%c"),
    VARIANT_CONVERSION_ITEM (TYPE_BYTE,   "%c"),
    VARIANT_CONVERSION_ITEM (TYPE_SHORT,  "%i"),
    VARIANT_CONVERSION_ITEM (TYPE_USHORT, "%i"),
    VARIANT_CONVERSION_ITEM (TYPE_INT32,  "%i"),
    VARIANT_CONVERSION_ITEM (TYPE_UINT32, "%i"),
    VARIANT_CONVERSION_ITEM (TYPE_INT64,  "%li"),
    VARIANT_CONVERSION_ITEM (TYPE_UINT64, "%li"),
    VARIANT_CONVERSION_ITEM (TYPE_POINTER, "%p"),
    VARIANT_CONVERSION_ITEM (TYPE_STRING, "%s"),
    VARIANT_CONVERSION_ITEM (TYPE_FLOAT,  "%f"),
    VARIANT_CONVERSION_ITEM (TYPE_DOUBLE, "%lf"),
};
size_t size_type_formatter = sizeof (type_formatter) / sizeof (type_formatter[0]);

return_t sprintf (stream_interface* stream, const char* fmt, valist va)
{
    return_t ret = errorcode_t::success;

    __try2
    {
        if (nullptr == stream) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        stream->flush ();

        if (nullptr == fmt) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        variant_t v;
        size_t i = 0;

        ansi_string formatter;
        formatter.write ((void *) fmt, strlen (fmt));

#if 0

        // simple implementation to support only {1} {2} {3}
        // it can't be {3} {2} {1}

        for (i = 0; i != va.size (); i++) {
            va.at (i, v);
            int order = i + 1;
            for (size_t table_index = 0; table_index != RTL_NUMBER_OF (type_formatter); table_index++) {
                if (type_formatter[table_index].type == v.type) {
                    formatter.replace (format ("{%d}", order).c_str (), type_formatter[table_index].formatter);
                    break;
                }
            }
        }

        stream->vprintf ((char *) formatter.data (), GET_VA_LIST (va.get ()));

#endif

        // Step1. check order ... build pair(fmt[?], argv[?])
        typedef std::map<size_t, size_t> va_map_t;
        typedef std::list <int> va_array_t;
        va_map_t va_map; /* pair(fmt[?], argv[?]) */
        va_array_t va_array;
        for (i = 0; i != va.size (); i++) {
            size_t key = i + 1;
            std::string find = format ("{%d}", key);
            size_t pos = 0;
            while (true) {
                pos = formatter.find_first_of (find.c_str (), pos);
                if ((size_t) -1 == pos) {
                    break;
                }
                va_map.insert (std::make_pair (pos, i));
                pos += find.size ();
            }
        }

        // Step2. relocate valist
        valist va_new;
        va_map_t::iterator iter;
        i = 0;
        for (va_map_t::iterator iter = va_map.begin (); iter != va_map.end (); iter++) {
            size_t idx = iter->second;
            va.at (idx, v);
            va_new << v;
            va_array.push_back (idx);
        }

        // Step3. replace format specifier
        typedef std::map<size_t, std::string> formatter_map_t;
        formatter_map_t formats;
        for (i = 0; i < RTL_NUMBER_OF (type_formatter); i++) {
            variant_conversion_t* item = type_formatter + i;
            formats.insert (std::make_pair (item->type, item->formatter));
        }
        va_array_t::iterator array_it;
        for (i = 0, array_it = va_array.begin (); array_it != va_array.end (); i++, array_it++) {
            size_t idx = *array_it;
            va_new.at (i, v);
            formatter_map_t::iterator fmt_it = formats.find (v.type);
            if (formats.end () != fmt_it) {
                formatter.replace (format ("{%d}", idx + 1).c_str (), fmt_it->second.c_str (), bufferio_flag_t::run_once);
            }
        }

        stream->vprintf ((char *) formatter.data (), va_new.get ());
    }
    __finally2
    {
        // do nothing
    }
    return ret;
}

return_t vtprintf (stream_interface* stream, variant_t vt)
{
    return_t ret = errorcode_t::success;

    __try2
    {
        if (nullptr == stream) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        switch (vt.type) {
            case TYPE_NULL:
                stream->printf ("null");
                break;
            case TYPE_BOOL:
                stream->printf ("%s", vt.data.b ? "true" : "false");
                break;
            case TYPE_INT8:
                stream->printf ("%i", vt.data.i8);
                break;
            case TYPE_UINT8:
                stream->printf ("%u", vt.data.ui8);
                break;
            case TYPE_INT16:
                stream->printf ("%i", vt.data.i16);
                break;
            case TYPE_UINT16:
                stream->printf ("%u", vt.data.ui16);
                break;
            case TYPE_INT32:
                stream->printf ("%i", vt.data.i32);
                break;
            case TYPE_UINT32:
                stream->printf ("%u", vt.data.ui32);
                break;
            case TYPE_INT64:
                stream->printf ("%I64i", vt.data.i64);
                break;
            case TYPE_UINT64:
                stream->printf ("%I64u", vt.data.ui64);
                break;
#if defined __SIZEOF_INT128__
            case TYPE_INT128:
                stream->printf ("%I128i", vt.data.i128);
                break;
            case TYPE_UINT128:
                stream->printf ("%I128u", vt.data.ui128);
                break;
#endif
            case TYPE_FLOAT:
                stream->printf ("%g", vt.data.f);
                break;
            case TYPE_DOUBLE:
                stream->printf ("%g", vt.data.d);
                break;
            case TYPE_POINTER:
                stream->printf ("%s", vt.data.p);
                break;
            case TYPE_STRING:
                stream->printf ("%s", vt.data.str);
                break;
            case TYPE_BINARY:
            {
                std::string temp;
                base16_encode (vt.data.bstr32.data, vt.data.bstr32.size, temp);
                stream->printf ("%s", temp.c_str ());
                break;
            }
            default:
                break;
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
