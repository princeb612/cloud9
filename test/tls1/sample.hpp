#ifndef __HOTPLACE_TEST_TLS13__
#define __HOTPLACE_TEST_TLS13__

#include <stdio.h>

#include <iostream>
#include <sdk/sdk.hpp>

using namespace hotplace;
using namespace hotplace::crypto;
using namespace hotplace::io;
using namespace hotplace::net;

struct OPTION {
    int verbose;
    int debug;
    int log;
    int time;
    std::string cipher_suite;
    uint16 version;

    OPTION() : verbose(0), debug(0), log(0), time(0), version(tls_13) {
        // do nothing
    }
    void enable_debug() {
        verbose = 1;
        debug = 1;
    }
    void set_cipher_suite(const char* cs) { cipher_suite = cs; }
};

extern test_case _test_case;
extern t_shared_instance<logger> _logger;
extern t_shared_instance<t_cmdline_t<OPTION>> _cmdline;

extern tls_session rfc8448_session;
extern tls_session rfc8448_session2;

return_t dump_record(const char* text, tls_session* session, const binary_t& bin, tls_direction_t dir = from_server, bool expect = true);
return_t dump_handshake(const char* text, tls_session* session, const binary_t& bin, tls_direction_t dir = from_server);
void test_keycalc(tls_session* session, tls_secret_t tls_secret, binary_t& secret, const char* text, const char* expect);
void test_transcript_hash(tls_session* session, const binary_t& expect);

void test_tls13_xargs_org();
void test_tls12_xargs_org();
void test_dtls_xargs_org();

void test_rfc8448_2();
void test_rfc8448_3();
void test_rfc8448_4();
void test_rfc8448_5();
void test_rfc8448_6();
void test_rfc8448_7();

void test_use_pre_master_secret();

void test_construct();
void test_validate();

#endif
