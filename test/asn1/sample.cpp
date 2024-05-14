/* vim: set tabstop=4 shiftwidth=4 softtabstop=4 expandtab smarttab : */
/**
 * @file {file}
 * @author Soo Han, Kim (princeb612.kr@gmail.com)
 * @desc
 *
 * Revision History
 * Date         Name                Description
 */

#include <algorithm>
#include <functional>
#include <sdk/sdk.hpp>

using namespace hotplace;
using namespace hotplace::io;
using namespace hotplace::net;

test_case _test_case;
t_shared_instance<logger> _logger;

// ISO/IEC 8824-1
// ITU-T X.680
// 8 Tags
// Table 1 - Universal class tag assignment

// ISO/IEC 8825-1
// X.690
// Specificaton of basic notation
// 8 Basic encoding rules
// 8.1 General rules for encoding
// 8.1.1 structure of an encoding
//  Table 1 - Encoding of class of tag
//  Figure 2 - An alternative constructed encoding
// 8.1.2 identifier octets
//  Figure 3 - Identifier octet (low tag number)
//  Figure 4 - Identifier octets (high tag number)
// 8.1.3 length octets
// 8.1.4 contents octets
// 8.1.5 end-of-contents octets
// 8.2 Encoding of a boolean value
// 8.3 Encoding of an integer value
// 8.4 Encoding of an enumerated value
// 8.5 Encoding of a real value

typedef struct _OPTION {
    int verbose;

    _OPTION() : verbose(0) {
        // do nothing
    }
} OPTION;
t_shared_instance<cmdline_t<OPTION>> _cmdline;

void test1() {
    //
}

// 216 page
// hash table

int main(int argc, char** argv) {
#ifdef __MINGW32__
    setvbuf(stdout, 0, _IOLBF, 1 << 20);
#endif

    _cmdline.make_share(new cmdline_t<OPTION>);
    *_cmdline << cmdarg_t<OPTION>("-v", "verbose", [](OPTION& o, char* param) -> void { o.verbose = 1; }).optional();
    _cmdline->parse(argc, argv);

    const OPTION& option = _cmdline->value();

    logger_builder builder;
    builder.set(logger_t::logger_stdout, option.verbose).set(logger_t::logger_flush_time, 0).set(logger_t::logger_flush_size, 0);
    _logger.make_share(builder.build());

    // studying ...
    test1();
    // test2();

    _logger->flush();

    _test_case.report(5);
    _cmdline->help();
    return _test_case.result();
}
