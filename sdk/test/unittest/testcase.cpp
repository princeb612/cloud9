/* vim: set tabstop=4 shiftwidth=4 softtabstop=4 expandtab smarttab : */
/**
 * @file {file}
 * @author Soo Han, Kim (princeb612.kr@gmail.com)
 * @desc
 *
 * Revision History
 * Date         Name                Description
 * 2023.08.15   Soo Han, Kim        elapsed time
 */

#include <hotplace/sdk/io/system/datetime.hpp>
#include <hotplace/sdk/io/stream/console_color.hpp>
#include <hotplace/sdk/io/stream/string.hpp>
#include <hotplace/sdk/io/string/string.hpp>
#include <hotplace/sdk/test/unittest/testcase.hpp>
#include <fstream>
#include <iostream>

namespace hotplace {
using namespace io;
namespace test {

test_case::test_case ()
    : _count_success (0),
    _count_fail (0),
    _count_not_supported (0),
    _count_low_security (0)
{
    // do nothing
    stopwatch::read (_timestamp);
}

void test_case::begin (const char* case_name, const char* file_path)
{
    if (nullptr != case_name) {
        _current_case_name = case_name;
        console_color col;
        /* "test case" */
        char STRING_TEST_CASE[] = { '[', '*', ' ', 't', 'e', 's', 't', ' ', 'c', 'a', 's', 'e', ' ', '-', ' ', 0, };
        std::cout   << col.set_fgcolor (console_color_t::magenta).turnon ()
                    << STRING_TEST_CASE << case_name << " ]" << col.turnoff () << std::endl;
    }
    if (nullptr != file_path) {
        _current_file_path = file_path;
        unittest_file_t::iterator iter = _test_file.find (_current_case_name);
        if (_test_file.end () == iter) {
            _test_file.insert (std::make_pair (_current_case_name, _current_file_path));
        }
    }
    stopwatch::read (_timestamp);
}

void test_case::start ()
{
    stopwatch::read (_timestamp);
}

void test_case::assert (bool expect, const char* test_function, const char* message)
{
    return_t ret = errorcode_t::success;

    if (false == expect) {
        ret = errorcode_t::unexpected;
    }
    test (ret, test_function, message);
}

void test_case::test (return_t result, const char* test_function, const char* message)
{
    struct timespec now, diff;

    __try2
    {
        stopwatch::read (now);
        stopwatch::diff (diff, _timestamp, now);

        _lock.enter ();

        if (errorcode_t::success == result) {
            _count_success++;
        } else if (errorcode_t::not_supported == result) {
            _count_not_supported++;
        } else if (errorcode_t::low_security == result) {
            _count_low_security++;
        } else {
            _count_fail++;
        }

        unittest_map_t::iterator iter = _test_map.find (_current_case_name);


        unittest_item_t item;
        memcpy (&item._time, &diff, sizeof (diff));
        item._result = result;
        if (nullptr != test_function) {
            item._test_function = test_function;
        }
        if (nullptr != message) {
            item._message = message;
        }

        if (_test_map.end () == iter) {
            test_status_t status;
            if (errorcode_t::success == result) {
                status._count_success++;
            } else if (errorcode_t::not_supported == result) {
                status._count_not_supported++;
            } else if (errorcode_t::low_security == result) {
                status._count_low_security++;
            } else {
                status._count_fail++;
            }

            status._test_list.push_back (item);                                 /* append a unittest_item_t */
            _test_map.insert (std::make_pair (_current_case_name, status));     /* insert a new test_status_t */
        } else {
            test_status_t& status = iter->second;
            if (errorcode_t::success == result) {
                status._count_success++;
            } else if (errorcode_t::not_supported == result) {
                status._count_not_supported++;
            } else if (errorcode_t::low_security == result) {
                status._count_low_security++;
            } else {
                status._count_fail++;
            }

            status._test_list.push_back (item); /* append a unittest_item_t */
        }

        console_color col;
        ansi_string buf;
        buf << col.set_fgcolor (console_color_t::yellow).turnon ()
            << format ("[%08x][%s] %s", result, test_function ? test_function : "", message ? message : "").c_str ()
            << col.turnoff ();

        std::cout << buf.c_str ()  << std::endl;
    }
    __finally2
    {
        _lock.leave ();

        stopwatch::read (_timestamp);
    }
}

void test_case::report ()
{
    ansi_string stream;
    console_color col;
    console_color_t fgcolor = console_color_t::white;

    _lock.enter ();

    /* "report success" */
    char STRING_REPORT[] = { 'r', 'e', 'p', 'o', 'r', 't', 0, };
    /* "success" */
    char STRING_SUCCESS[] = { 's', 'u', 'c', 'c', 'e', 's', 's', 0, };
    /* "pass" */
    char STRING_PASS[] = { 'p', 'a', 's', 's', 0, };
    /* "fail" */
    char STRING_FAIL[] = { 'f', 'a', 'i', 'l', ' ', 0, };
    /* "skip" */
    char STRING_NOT_SUPPORTED[] = { 's', 'k', 'i', 'p', ' ', 0, };
    /* "low" */
    char STRING_LOW_SECURITY[] = { 'l', 'o', 'w', ' ', ' ', 0, };
    /* "test case" */
    char STRING_TEST_CASE[] = { 't', 'e', 's', 't', ' ', 'c', 'a', 's', 'e', 0, };
    /* "result" */
    char STRING_RESULT[] = { 'r', 'e', 's', 'u', 'l', 't', 0, };
    /* "errorcode" */
    char STRING_ERRORCODE[] = { 'e', 'r', 'r', 'o', 'r', 'c', 'o', 'd', 'e', 0, };
    /* "test function" */
    char STRING_TEST_FUNCTION[] = { 't', 'e', 's', 't', ' ', 'f', 'u', 'n', 'c', 't', 'i', 'o', 'n', 0, };
    /* "time" */
    char STRING_TIME[] = { 't', 'i', 'm', 'e', 0, };
    /* "message" */
    char STRING_MESSAGE[] = { 'm', 'e', 's', 's', 'a', 'g', 'e', 0, };
    char STRING_UPPERCASE_TEST_FAILED[] = { 'T', 'E', 'S', 'T', ' ', 'F', 'A', 'I', 'L', 'E', 'D', 0, };

#define PRINT_STRING_SUCCESS col.set_fgcolor (console_color_t::green) << STRING_SUCCESS << col.set_fgcolor (fgcolor)
#define PRINT_STRING_FAIL col.set_fgcolor (console_color_t::red) << STRING_FAIL << col.set_fgcolor (fgcolor)
#define PRINT_STRING_NOT_SUPPORTED col.set_fgcolor (console_color_t::cyan) << STRING_NOT_SUPPORTED << col.set_fgcolor (fgcolor)
#define PRINT_STRING_LOW_SECURITY col.set_fgcolor (console_color_t::yellow) << STRING_LOW_SECURITY  << col.set_fgcolor (fgcolor)

    //
    // compose
    //

    stream << col.set_style (console_style_t::bold).set_fgcolor (fgcolor).turnon () << STRING_REPORT << "\n";

    for (unittest_map_t::iterator iter = _test_map.begin (); iter != _test_map.end (); iter++) {
        test_status_t status = iter->second;

        std::string file_path;
        unittest_file_t::iterator iter_file = _test_file.find (iter->first);

        if (_test_file.end () != iter_file) {
            file_path = iter_file->second;
        } else {
            // not found
            file_path.clear ();
        }

        stream  << "@ "
                << STRING_TEST_CASE << " \"" << iter->first.c_str () << "\" "
                << PRINT_STRING_SUCCESS << " " << status._count_success;
        if (status._count_fail) {
            stream << " " << PRINT_STRING_FAIL << " " << status._count_fail;
        }
        if (status._count_not_supported) {
            stream << " " << PRINT_STRING_NOT_SUPPORTED << " " << status._count_not_supported;
        }
        if (status._count_low_security) {
            stream << " " << PRINT_STRING_LOW_SECURITY << " " << status._count_low_security;
        }
        stream << file_path.c_str () << "\n";

        stream.fill (80, '-');
        stream << "\n";
        stream.printf ("%-6s | %-10s | %-20s | %-10s | %s\n", STRING_RESULT, STRING_ERRORCODE, STRING_TEST_FUNCTION, STRING_TIME, STRING_MESSAGE);

        for (unittest_list_t::iterator list_iterator = status._test_list.begin (); list_iterator != status._test_list.end (); list_iterator++) {
            unittest_item_t item = *list_iterator;

            ansi_string error_message;
            switch (item._result) {
                case errorcode_t::success:       error_message << STRING_PASS; break;
                case errorcode_t::not_supported: error_message << PRINT_STRING_NOT_SUPPORTED; break;
                case errorcode_t::low_security:  error_message << PRINT_STRING_LOW_SECURITY; break;
                default:                     error_message << PRINT_STRING_FAIL; break;
            }

            stream.printf (" %-5s | 0x%08x | %-20s | %-10s | %s\n",
                           error_message.c_str (), item._result, item._test_function.c_str (),
                           format ("%lld.%07ld", item._time.tv_sec, item._time.tv_nsec / 100).c_str (),
                           item._message.c_str ());
        }
        stream.fill (80, '-');
        stream << "\n";
    }

    stream << "# " << PRINT_STRING_SUCCESS << " " << _count_success;
    if (_count_fail) {
        stream << " " << PRINT_STRING_FAIL << " " << _count_fail;
    }
    if (_count_not_supported) {
        stream << " " << PRINT_STRING_NOT_SUPPORTED << " " << _count_not_supported;
    }
    if (_count_low_security) {
        stream << " " << PRINT_STRING_LOW_SECURITY << " " << _count_low_security;
    }
    stream << col.turnoff () << "\n";

    _lock.leave ();

    //
    // print
    //
    ansi_string line;

    line.fill (80, '=');
    line << "\n";
    std::cout << line.c_str ();
    printf ("%s", stream.c_str ());
    if (_count_fail) {
        std::cout << col.set_fgcolor (console_color_t::red).turnon () << STRING_UPPERCASE_TEST_FAILED << col.turnoff () << std::endl;
    }
    std::cout << line.c_str ();

#if 1
    std::ofstream file (STRING_REPORT, std::ios::trunc);
    file << stream.c_str ();
    file.close ();
#endif
}

return_t test_case::result ()
{
    return _count_fail > 0 ? errorcode_t::internal_error : errorcode_t::success;
}

}
}  // namespace
