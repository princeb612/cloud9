/* vim: set tabstop=4 shiftwidth=4 softtabstop=4 expandtab smarttab : */
/**
 * @file {file}
 * @author Soo Han, Kim (princeb612.kr@gmail.com)
 * @desc
 *
 * Revision History
 * Date         Name                Description
 */

#include <string.h>

#include <list>
#include <sdk/base/string/string.hpp>
#include <string>

namespace hotplace {

#if defined _MBCS || defined MBCS
void replace(std::string& source, const std::string& a, const std::string& b)
#elif defined _UNICODE || defined UNICODE
void replace(std::wstring& source, const std::wstring& a, const std::wstring& b)
#endif
{
    size_t i = source.find(a);

    while (std::string::npos != i) {
        source.replace(i, a.size(), b);
        i = source.find(a, i + b.size());
    }
}

#if defined _MBCS || defined MBCS
return_t scan(const char* stream, size_t sizestream, size_t startpos, size_t* brk, int (*func)(int))
#elif defined _UNICODE || defined UNICODE
return_t scan(const wchar_t* stream, size_t sizestream, size_t startpos, size_t* brk, int (*func)(int))
#endif
{
    return_t ret = errorcode_t::success;

    __try2 {
        if (nullptr == stream || nullptr == brk || nullptr == func) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        if (startpos >= sizestream) {
            ret = errorcode_t::out_of_range;
            __leave2;
        }

        const TCHAR* pos = stream + startpos;
        const TCHAR* epos = stream + sizestream;
        const TCHAR* p = stream + startpos;

        while (0 == (*func)(*p) && p < epos) {
            p++;
        }
        *brk = startpos + p - pos + 1;
    }
    __finally2 {
        // do nothing
    }

    return ret;
}

#if defined _MBCS || defined MBCS
return_t scan(const char* stream, size_t sizestream, size_t startpos, size_t* brk, const char* match)
#elif defined _UNICODE || defined UNICODE
return_t scan(const wchar_t* stream, size_t sizestream, size_t startpos, size_t* brk, const wchar_t* match)
#endif
{
    return_t ret = errorcode_t::success;

    __try2 {
        if (nullptr == stream || nullptr == brk || nullptr == match) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        if (startpos >= sizestream) {
            ret = errorcode_t::out_of_range;
            __leave2;
        }

        const TCHAR* pos = stream + startpos;
        const TCHAR* epos = stream + sizestream;
        const TCHAR* p = stream + startpos;

#if defined _MBCS || defined MBCS
        size_t sizetoken = strlen(match);
        while ((0 != strnicmp(match, p, sizetoken)) && p < epos) {
            p++;
        }
#elif defined _UNICODE || defined UNICODE
        size_t sizetoken = wcslen(match);
        while ((0 != wcsnicmp(match, p, sizetoken)) && p < epos) {
            p++;
        }
#endif

        if (p < epos) {
            *brk = startpos + p - pos + 1;
        } else {
            *brk = sizestream;
            // ret = errorcode_t::not_found;
        }
    }
    __finally2 {
        // do nothing
    }

    return ret;
}

#if defined _MBCS || defined MBCS
return_t getline(const char* stream, size_t sizestream, size_t startpos, size_t* brk)
#elif defined _UNICODE || defined UNICODE
return_t getline(const wchar_t* stream, size_t sizestream, size_t startpos, size_t* brk)
#endif
{
    return scan(stream, sizestream, startpos, brk, _T ("\n"));
}

}  // namespace hotplace
