/* vim: set tabstop=4 shiftwidth=4 softtabstop=4 expandtab smarttab : */
/**
 * @file {file}
 * @author Soo Han, Kim (princeb612.kr@gmail.com)
 * @desc
 *
 * Revision History
 * Date         Name                Description
 * 2023.09.01   Soo Han, Kim        refactor
 */

#include <hotplace/sdk/sdk.hpp>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <deque>

using namespace hotplace;
using namespace hotplace::io;

test_case _test_case;

void encode_test (variant_t vt, binary_t& bin, std::string expect)
{
    return_t ret = errorcode_t::success;
    cbor_encode enc;

    bin.clear ();
    enc.encode (bin, vt);

    std::string hex;
    base16_encode (bin, hex);

    if (0 == stricmp (hex.c_str (), expect.c_str ())) {
        // match
    } else {
        ret = errorcode_t::mismatch;
    }

    if (1) {
        test_case_notimecheck notimecheck (_test_case);

        buffer_stream bs;

        //vtprintf (&bs, vt);
        //std::cout << bs.c_str () << std::endl;
        dump_memory (bin, &bs);
        std::cout << bs.c_str () << std::endl;
    }

    _test_case.test (ret, __FUNCTION__, "encoded %s expect %s", hex.c_str (), expect.c_str ());
}

void cbor_test (cbor_object* root, const char* expected)
{
    return_t ret = errorcode_t::success;

    __try2
    {
        if (nullptr == root || nullptr == expected) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        cbor_publisher publisher;
        binary_t bin;
        buffer_stream diagnostic;

        publisher.publish (root, &diagnostic);
        publisher.publish (root, &bin);

        std::string concise;
        base16_encode (bin, concise);
        std::string info = format ("concise: %s diagnostic: %s", concise.c_str (), diagnostic.c_str ());

        if (stricmp (concise.c_str (), expected)) {
            ret = errorcode_t::mismatch;
        }

        _test_case.test (ret, __FUNCTION__, info.c_str ());
    }
    __finally2
    {
        // do nothing
    }
}

void test1 ()
{
    _test_case.begin ("test1.encode uint, nint, float");

    binary_t bin;
    variant_t vt;

    variant_set_int8 (vt, 0);
    encode_test (vt, bin, "00");

    variant_set_int8 (vt, 1);
    encode_test (vt, bin, "01");

    variant_set_int8 (vt, 10);
    encode_test (vt, bin, "0a");

    variant_set_int8 (vt, 23);
    encode_test (vt, bin, "17");

    variant_set_int8 (vt, 24);
    encode_test (vt, bin, "1818");

    variant_set_int8 (vt, 25);
    encode_test (vt, bin, "1819");

    variant_set_int8 (vt, 100);
    encode_test (vt, bin, "1864");

    variant_set_int16 (vt, 1000);
    encode_test (vt, bin, "1903e8");

    variant_set_int32 (vt, 1000000);
    encode_test (vt, bin, "1a000f4240");

#if defined __SIZEOF_INT128__
    variant_set_int64 (vt, 1000000000000);
    encode_test (vt, bin, "1b000000e8d4a51000");

    cbor_data* cbor_1000000000000 = new cbor_data (atoi128 ("1000000000000"));
    cbor_test (cbor_1000000000000, "1b000000e8d4a51000");
    cbor_1000000000000->release ();

    variant_set_uint128 (vt, atoi128 ("18446744073709551615"));
    encode_test (vt, bin, "1bffffffffffffffff");

    cbor_data* cbor_18446744073709551615 = new cbor_data (atoi128 ("18446744073709551615"));
    cbor_test (cbor_18446744073709551615, "1bffffffffffffffff");
    cbor_18446744073709551615->release ();

    variant_set_int128 (vt, atoi128 ("18446744073709551616"));
    encode_test (vt, bin, "c249010000000000000000");

    cbor_data* cbor_18446744073709551616 = new cbor_data (atoi128 ("18446744073709551616"));
    cbor_test (cbor_18446744073709551616, "c249010000000000000000");
    cbor_18446744073709551616->release ();

    variant_set_int128 (vt, atoi128 ("-18446744073709551616"));
    encode_test (vt, bin, "3bffffffffffffffff");

    cbor_data* cbor_s18446744073709551616 = new cbor_data (atoi128 ("-18446744073709551616"));
    cbor_test (cbor_s18446744073709551616, "3bffffffffffffffff");
    cbor_s18446744073709551616->release ();

    variant_set_int128 (vt, atoi128 ("-18446744073709551617"));
    encode_test (vt, bin, "c349010000000000000000");

    cbor_data* cbor_s18446744073709551617 = new cbor_data (atoi128 ("-18446744073709551617"));
    cbor_test (cbor_s18446744073709551617, "c349010000000000000000");
    cbor_s18446744073709551617->release ();
#endif

    variant_set_int32 (vt, -1);
    encode_test (vt, bin, "20");

    variant_set_int32 (vt, -10);
    encode_test (vt, bin, "29");

    variant_set_int16 (vt, -100);
    encode_test (vt, bin, "3863");

    variant_set_int16 (vt, -1000);
    encode_test (vt, bin, "3903e7");

    /* 00000000 : 00 00 00 00 -- -- -- -- -- -- -- -- -- -- -- -- | 0x00000000 */
    variant_set_float (vt, 0.0);
    encode_test (vt, bin, "f90000"); // fa00000000

    variant_set_double (vt, 0.0);
    encode_test (vt, bin, "f90000"); // fb0000000000000000

    /* 00000000 : 00 00 00 80 -- -- -- -- -- -- -- -- -- -- -- -- | 0x80000000 */
    variant_set_float (vt, -0.0);
    encode_test (vt, bin, "f98000"); // fa80000000

    variant_set_double (vt, -0.0);
    encode_test (vt, bin, "f98000"); // fb8000000000000000

    /* 00000000 : 00 00 80 3F -- -- -- -- -- -- -- -- -- -- -- -- | 0x3f800000 */
    variant_set_float (vt, 1.0);
    encode_test (vt, bin, "f93c00"); // fa3f800000

    variant_set_double (vt, 1.0);
    encode_test (vt, bin, "f93c00"); // fb3ff0000000000000

    /* 00000000 : CD CC 8C 3F -- -- -- -- -- -- -- -- -- -- -- -- | 0x3f8ccccd */
    variant_set_float (vt, 1.1);
    encode_test (vt, bin, "fa3f8ccccd");

    variant_set_double (vt, 1.1);
    encode_test (vt, bin, "fb3ff199999999999a");

    /* 00000000 : 00 00 C0 3F -- -- -- -- -- -- -- -- -- -- -- -- | 0x3fc00000 */
    variant_set_float (vt, 1.5);
    encode_test (vt, bin, "f93e00"); // fa3fc00000

    variant_set_double (vt, 1.5);
    encode_test (vt, bin, "f93e00"); // fb3ff8000000000000

    /* 00000000 : 00 E0 7F 47 -- -- -- -- -- -- -- -- -- -- -- -- | 0x477fe000 */
    variant_set_float (vt, 65504.0);
    encode_test (vt, bin, "f97bff"); // fa477fe000

    variant_set_double (vt, 65504.0);
    encode_test (vt, bin, "f97bff"); // fb40effc0000000000

    /* 00000000 : 00 50 C3 47 -- -- -- -- -- -- -- -- -- -- -- -- | 0x47c35000 */
    variant_set_float (vt, 100000.0);
    encode_test (vt, bin, "fa47c35000");

    variant_set_double (vt, 100000.0);
    encode_test (vt, bin, "fa47c35000"); // fb40f86a0000000000

    variant_set_float (vt, 3.4028234663852886e+38);
    encode_test (vt, bin, "fa7f7fffff");

    variant_set_double (vt, 1.0e+300);
    encode_test (vt, bin, "fb7e37e43c8800759c");

    variant_set_float (vt, 5.960464477539063e-8);
    encode_test (vt, bin, "f90001"); // fa33800000

    variant_set_float (vt, 0.00006103515625);
    encode_test (vt, bin, "f90400"); // fa38800000

    variant_set_float (vt, -4.0);
    encode_test (vt, bin, "f9c400"); // fac0800000

    variant_set_float (vt, -4.1);
    encode_test (vt, bin, "fac0833333");

    variant_set_double (vt, -4.1);
    encode_test (vt, bin, "fbc010666666666666");

    variant_set_bool (vt, false);
    encode_test (vt, bin, "f4");

    cbor_simple* sample_false = new cbor_simple (cbor_simple_t::cbor_simple_false);
    cbor_test (sample_false, "f4");
    sample_false->release ();

    variant_set_bool (vt, true);
    encode_test (vt, bin, "f5");

    cbor_simple* sample_true = new cbor_simple (cbor_simple_t::cbor_simple_true);
    cbor_test (sample_true, "f5");
    sample_true->release ();

    cbor_simple* sample_null = new cbor_simple (cbor_simple_t::cbor_simple_null);
    cbor_test (sample_null, "f6");
    sample_null->release ();

    cbor_simple* sample_undef = new cbor_simple (cbor_simple_t::cbor_simple_undef);
    cbor_test (sample_undef, "f7");
    sample_undef->release ();

    cbor_simple* simple_16 = new cbor_simple (16);
    cbor_test (simple_16, "f0");
    simple_16->release ();

    cbor_simple* simple_24 = new cbor_simple (24);
    cbor_test (simple_24, "f818");
    simple_24->release ();

    cbor_simple* simple_255 = new cbor_simple (255);
    cbor_test (simple_255, "f8ff");
    simple_255->release ();

    cbor_data* data_stddatetime = new cbor_data (1363896240);
    data_stddatetime->tag (true, cbor_tag_t::cbor_tag_epoch_datetime);
    cbor_test (data_stddatetime, "c11a514b67b0");
    data_stddatetime->release ();

    cbor_data* data_stddatetime2 = new cbor_data (1363896240.5);
    data_stddatetime2->tag (true, cbor_tag_t::cbor_tag_epoch_datetime);
    cbor_test (data_stddatetime2, "c1fb41d452d9ec200000");
    data_stddatetime2->release ();

    cbor_data* data_base16 = new cbor_data (base16_decode ("01020304"));
    data_base16->tag (true, cbor_tag_t::cbor_tag_base16);
    cbor_test (data_base16, "d74401020304");
    data_base16->release ();

    cbor_data* data_encoded = new cbor_data (base16_decode ("6449455446"));
    data_encoded->tag (true, cbor_tag_t::cbor_tag_encoded);
    cbor_test (data_encoded, "d818456449455446");
    data_encoded->release ();

    cbor_data* data_wwwexamplecom = new cbor_data ("http://www.example.com");
    data_wwwexamplecom->tag (true, cbor_tag_t::cbor_tag_uri);
    cbor_test (data_wwwexamplecom, "d82076687474703a2f2f7777772e6578616d706c652e636f6d");
    data_wwwexamplecom->release ();

    cbor_data* data_b = new cbor_data (base16_decode (""));
    cbor_test (data_b, "40");
    data_b->release ();

    cbor_data* data_b01020304 = new cbor_data (base16_decode ("01020304"));
    cbor_test (data_b01020304, "4401020304");
    data_b01020304->release ();

    variant_set_str (vt, "");
    encode_test (vt, bin, "60");

    variant_set_str (vt, "a");
    encode_test (vt, bin, "6161");

    variant_set_str (vt, "IETF");
    encode_test (vt, bin, "6449455446");

    variant_set_str (vt, "\"\\");
    encode_test (vt, bin, "62225c");

    variant_set_str (vt, "\u00fc");
    encode_test (vt, bin, "62c3bc");

    variant_set_str (vt, "\u6c34");
    encode_test (vt, bin, "63e6b0b4");
}

void test2 ()
{
    _test_case.begin ("test2.encode array, map");

    {
        // []
        cbor_array* root = new cbor_array ();
        cbor_test (root, "80");
        root->release ();
    }
    {
        // [1,2,3]
        cbor_array* root = new cbor_array ();
        *root << new cbor_data (1) << new cbor_data (2) << new cbor_data (3);
        cbor_test (root, "83010203");
        root->release ();
    }
    {
        // [1,[2,3],[4,5]]
        cbor_array* root = new cbor_array ();
        cbor_array* sample1 = new cbor_array ();
        *sample1 << new cbor_data (2) << new cbor_data (3);
        cbor_array* sample2 = new cbor_array ();
        *sample2 << new cbor_data (4) << new cbor_data (5);
        *root << new cbor_data (1) << sample1 << sample2;
        cbor_test (root, "8301820203820405");
        root->release ();
    }
    {
        // [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25]
        cbor_array* root = new cbor_array ();
        for (int i = 1; i <= 25; i++) {
            *root << new cbor_data (i);
        }
        cbor_test (root, "98190102030405060708090a0b0c0d0e0f101112131415161718181819");
        root->release ();
    }
    {
        // {}
        cbor_map* root = new cbor_map ();
        cbor_test (root, "a0");
        root->release ();
    }
    {
        // {1:2,3:4}
        cbor_map* root = new cbor_map ();
        *root << new cbor_pair (1, new cbor_data (2)) << new cbor_pair (3, new cbor_data (4));
        cbor_test (root, "a201020304");
        root->release ();
    }
    {
        // {"a":1,"b":[2,3]}
        cbor_map* root = new cbor_map ();
        cbor_array* sample1 = new cbor_array ();
        *sample1 << new cbor_data (2) << new cbor_data (3);
        *root << new cbor_pair ("a", new cbor_data (1)) << new cbor_pair ("b", sample1);
        cbor_test (root, "a26161016162820203");
        root->release ();
    }
    {
        // ["a",{"b":"c"}]
        cbor_array* root = new cbor_array ();
        cbor_map* sample1 = new cbor_map ();
        *sample1 << new cbor_pair ("b", new cbor_data ("c"));
        *root << new cbor_data ("a") << sample1;
        cbor_test (root, "826161a161626163");
        root->release ();
    }
    {
        // [_ ]
        cbor_array* root = new cbor_array (cbor_flag_t::cbor_indef);
        cbor_test (root, "9fff");
        root->release ();
    }
    {
        // [_ 1,[2,3],[_ 4,5]]
        cbor_array* root = new cbor_array (cbor_flag_t::cbor_indef);
        cbor_array* sample1 = new cbor_array ();
        cbor_array* sample2 = new cbor_array (cbor_flag_t::cbor_indef);
        *sample1 << new cbor_data (2) << new cbor_data (3);
        *sample2 << new cbor_data (4) << new cbor_data (5);
        *root << new cbor_data (1) << sample1 << sample2;
        cbor_test (root, "9f018202039f0405ffff");
        root->release ();
    }
    {
        // [_ 1,[2,3],[4,5]]
        cbor_array* root = new cbor_array (cbor_flag_t::cbor_indef);
        cbor_array* sample1 = new cbor_array ();
        cbor_array* sample2 = new cbor_array ();
        *sample1 << new cbor_data (2) << new cbor_data (3);
        *sample2 << new cbor_data (4) << new cbor_data (5);
        *root << new cbor_data (1) << sample1 << sample2;
        cbor_test (root, "9f01820203820405ff");
        root->release ();
    }
    {
        // [1,[2,3],[_ 4,5]]
        cbor_array* root = new cbor_array ();
        cbor_array* sample1 = new cbor_array ();
        cbor_array* sample2 = new cbor_array (cbor_flag_t::cbor_indef);
        *sample1 << new cbor_data (2) << new cbor_data (3);
        *sample2 << new cbor_data (4) << new cbor_data (5);
        *root << new cbor_data (1) << sample1 << sample2;
        cbor_test (root, "83018202039f0405ff");
        root->release ();
    }
    {
        // [1,[_ 2,3],[4,5]]
        cbor_array* root = new cbor_array ();
        cbor_array* sample1 = new cbor_array (cbor_flag_t::cbor_indef);
        cbor_array* sample2 = new cbor_array ();
        *sample1 << new cbor_data (2) << new cbor_data (3);
        *sample2 << new cbor_data (4) << new cbor_data (5);
        *root << new cbor_data (1) << sample1 << sample2;
        cbor_test (root, "83019f0203ff820405");
        root->release ();
    }
    {
        // [_ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25]
        cbor_array* root = new cbor_array (cbor_flag_t::cbor_indef);
        for (int i = 1; i <= 25; i++) {
            *root << new cbor_data (i);
        }
        cbor_test (root, "9f0102030405060708090a0b0c0d0e0f101112131415161718181819ff");
        root->release ();
    }
    {
        // {_ "a":1,"b":[_ 2,3]}
        cbor_map* root = new cbor_map (cbor_flag_t::cbor_indef);
        cbor_array* sample1 = new cbor_array (cbor_flag_t::cbor_indef);
        *sample1 << new cbor_data (2) << new cbor_data (3);
        *root << new cbor_pair ("a", new cbor_data (1)) << new cbor_pair ("b", sample1);
        cbor_test (root, "bf61610161629f0203ffff");
        root->release ();
    }
    {
        // ["a",{_ "b":"c"}]
        cbor_array* root = new cbor_array ();
        cbor_map* sample1 = new cbor_map (cbor_flag_t::cbor_indef);
        *sample1 << new cbor_pair ("b", new cbor_data ("c"));
        *root << new cbor_data ("a") << sample1;
        cbor_test (root, "826161bf61626163ff");
        root->release ();
    }
    {
        // {_ "Fun":true,"Amt":-2}
        cbor_map* root = new cbor_map (cbor_flag_t::cbor_indef);
        *root << new cbor_pair ("Fun", new cbor_data (true)) << new cbor_pair ("Amt", new cbor_data (-2));
        cbor_test (root, "bf6346756ef563416d7421ff");
        root->release ();
    }
    {
        binary_t bin;
        cbor_bstrings* root = new cbor_bstrings ();
        *root << base16_decode ("0102" ) << base16_decode ("030405");
        cbor_test (root, "5f42010243030405ff");
        root->release ();
    }
    {
        cbor_tstrings* root = new cbor_tstrings ();
        *root << "strea" << "ming";
        cbor_test (root, "7f657374726561646d696e67ff");
        root->release ();
    }
    {
        cbor_data root[3] = { -1, -2, -3 };
        cbor_test (&root[0], "20");
    }
}

void test_parse (const char* input, const char* diagnostic, const char* diagnostic2 = nullptr)
{
    cbor_reader reader;
    cbor_reader_context_t* handle = nullptr;
    ansi_string bs;
    binary_t bin;

    reader.open (&handle);
    reader.parse (handle, input);
    reader.publish (handle, &bs);
    reader.publish (handle, bin);
    reader.close (handle);

    bool test = false;
    {
        test_case_notimecheck notimecheck (_test_case);

        printf ("diagnostic %s\n", bs.c_str ());
        std::string b16;
        base16_encode (bin, b16);
        printf ("cbor       %s\n", b16.c_str ());

        test = (0 == stricmp (input, b16.c_str ()));
    }

    bool test2 = false;
    {
        test_case_notimecheck notimecheck (_test_case);

        ansi_string bs_diagnostic;
        ansi_string bs_diagnostic2;
        bs_diagnostic = diagnostic;

        bs.replace  ("_ ", "__");
        bs.replace  (" ", "");
        bs.replace  ("__", "_ ");
        bs_diagnostic.replace  ("_ ", "__");
        bs_diagnostic.replace  (" ", "");
        bs_diagnostic.replace  ("__", "_ ");

        if (diagnostic2) {
            bs_diagnostic2 = diagnostic2;
            test2 = ((bs == bs_diagnostic) || (bs == bs_diagnostic2));
        } else {
            test2 = (bs == bs_diagnostic);
        }
    }

    _test_case.assert (test && test2, __FUNCTION__, "decode input %s diagnostic %s", input, diagnostic ? diagnostic : "");
}

void test3 ()
{
    _test_case.begin ("test3.parse");
    // 0
    test_parse ("00", "0");
    // 1
    test_parse ("01", "1");
    // 10
    test_parse ("0a", "10");
    // 23
    test_parse ("17", "23");
    // 24
    test_parse ("1818", "24");
    // 25
    test_parse ("1819", "25");
    // 100
    test_parse ("1864", "100");
    // 1000
    test_parse ("1903e8", "1000");
    // 1000000
    test_parse ("1a000f4240", "1000000");
    // 1000000000000
    test_parse ("1b000000e8d4a51000", "1000000000000");
    // 18446744073709551615
    test_parse ("1bffffffffffffffff", "18446744073709551615");
    // 18446744073709551616
    test_parse ("c249010000000000000000", "18446744073709551616", "2(18446744073709551616)");
    // -18446744073709551615
    test_parse ("3bfffffffffffffffe", "-18446744073709551615");
    // -18446744073709551616
    test_parse ("3bffffffffffffffff", "-18446744073709551616");
    // -18446744073709551617
    test_parse ("c349010000000000000000", "-18446744073709551617", "3(-18446744073709551617)");
    // -1
    test_parse ("20", "-1");
    // -10
    test_parse ("29", "-10");
    // -100
    test_parse ("3863", "-100");
    // -1000
    test_parse ("3903e7", "-1000");
    // false
    test_parse ("f4", "false");
    // true
    test_parse ("f5", "true");
    // null
    test_parse ("f6", "null");
    // undefined
    test_parse ("f7", "undefined");
    // []
    test_parse ("80", "[]");
    // [1,2,3]
    test_parse ("83010203", "[1,2,3]");
    // [1,[2,3],[4,5]]
    test_parse ("8301820203820405", "[1,[2,3],[4,5]]");
    // [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25]
    test_parse ("98190102030405060708090a0b0c0d0e0f101112131415161718181819", "[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25]");
    // {}
    test_parse ("a0", "{}");
    // {1:2,3:4}
    test_parse ("a201020304", "{1:2,3:4}");
    // {"a":1,"b":[2,3]}
    test_parse ("a26161016162820203", "{\"a\":1,\"b\":[2,3]}");
    // ["a",{"b":"c"}]
    test_parse ("826161a161626163", "[\"a\",{\"b\":\"c\"}]");
    // {"a": "A", "b": "B", "c": "C", "d": "D", "e": "E"}
    test_parse ("a56161614161626142616361436164614461656145", "{\"a\": \"A\", \"b\": \"B\", \"c\": \"C\", \"d\": \"D\", \"e\": \"E\"}");
    // (_ h'0102', h'030405')
    test_parse ("5f42010243030405ff", "(_ h'0102', h'030405')");
    // (_ "strea", "ming")
    test_parse ("7f657374726561646d696e67ff", "(_ \"strea\", \"ming\")");
    // [_ ]
    test_parse ("9fff", "[_ ]");
    // [_ 1,[2,3],[_ 4,5]]
    test_parse ("9f018202039f0405ffff", "[_ 1,[2,3],[_ 4,5]]");
    // [_ 1,[2,3],[4,5]]
    test_parse ("9f01820203820405ff", "[_ 1,[2,3],[4,5]]");
    // [1,[2,3],[_ 4,5]]
    test_parse ("83018202039f0405ff", "[1,[2,3],[_ 4,5]]");
    // [1,[_ 2,3],[4,5]]
    test_parse ("83019f0203ff820405", "[1,[_ 2,3],[4,5]]");
    // [_ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25]
    test_parse ("9f0102030405060708090a0b0c0d0e0f101112131415161718181819ff", "[_ 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25]");
    // {_ "a":1,"b":[_ 2,3]}
    test_parse ("bf61610161629f0203ffff", "{_ \"a\":1,\"b\":[_ 2,3]}");
    // ["a",{_ "b":"c"}]
    test_parse ("826161bf61626163ff", "[\"a\",{_ \"b\":\"c\"}]");
    // {_ "Fun":true,"Amt":-2}
    test_parse ("bf6346756ef563416d7421ff", "{_ \"Fun\":true,\"Amt\":-2}");
}

int main ()
{
    test1 ();
    test2 ();
    test3 ();

    _test_case.report (5);
    return _test_case.result ();
}
