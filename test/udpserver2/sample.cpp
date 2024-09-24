/* vim: set tabstop=4 shiftwidth=4 softtabstop=4 expandtab smarttab : */
/**
 * @file {file}
 * @author Soo Han, Kim (princeb612.kr@gmail.com)
 * @desc
 *
 * Revision History
 * Date         Name                Description
 */

#include <stdio.h>

#include <iostream>
#include <sdk/sdk.hpp>

using namespace hotplace;
using namespace hotplace::io;
using namespace hotplace::crypto;
using namespace hotplace::net;

test_case _test_case;
t_shared_instance<logger> _logger;

typedef struct _OPTION {
    int verbose;
    uint16 port;

    _OPTION() : verbose(0), port(9000) {
        // do nothing
    }
} OPTION;
t_shared_instance<t_cmdline_t<OPTION>> _cmdline;

#define FILENAME_RUN _T (".run")

return_t consume_routine(uint32 type, uint32 data_count, void* data_array[], CALLBACK_CONTROL* callback_control, void* user_context) {
    return_t ret = errorcode_t::success;
    network_session_socket_t* session_socket = (network_session_socket_t*)data_array[0];
    byte_t* buf = (byte_t*)data_array[1];
    size_t bufsize = (size_t)data_array[2];
    network_session* session = (network_session*)data_array[3];
    sockaddr_storage_t* addr = (sockaddr_storage_t*)data_array[5];

    basic_stream bs;
    std::string message;

    switch (type) {
        case mux_dgram:
            _logger->writeln("read %d msg [%.*s]", session_socket->event_socket, (unsigned)bufsize, buf);
            // dump_memory (buf, bufsize, &bs, 16, 4);
            // std::cout << bs << std::endl;
            session->sendto((char*)buf, bufsize, addr);
            break;
    }
    return ret;
}

return_t echo_server(void* param) {
    return_t ret = errorcode_t::success;
    const OPTION& option = _cmdline->value();

    network_server network_server;
    network_multiplexer_context_t* handle_ipv4 = nullptr;
    network_multiplexer_context_t* handle_ipv6 = nullptr;
    udp_server_socket svr_sock;
    uint16 port = option.port;

    FILE* fp = fopen(FILENAME_RUN, "w");

    fclose(fp);

    __try2 {
        server_conf conf;
        conf.set(netserver_config_t::serverconf_concurrent_event, 1024)  // concurrent (linux epoll concerns, windows ignore)
            .set(netserver_config_t::serverconf_concurrent_tls_accept, 1)
            .set(netserver_config_t::serverconf_concurrent_network, 2)
            .set(netserver_config_t::serverconf_concurrent_consume, 2);

        network_server.open(&handle_ipv4, AF_INET, port, &svr_sock, &conf, consume_routine, nullptr);
        network_server.open(&handle_ipv6, AF_INET6, port, &svr_sock, &conf, consume_routine, nullptr);

        network_server.consumer_loop_run(handle_ipv4, 2);
        network_server.consumer_loop_run(handle_ipv6, 2);
        network_server.event_loop_run(handle_ipv4, 1);
        network_server.event_loop_run(handle_ipv6, 1);

        while (true) {
            msleep(1000);

#if defined __linux__
            int chk = access(FILENAME_RUN, F_OK);
            if (errorcode_t::success != chk) {
                break;
            }
#elif defined _WIN32 || defined _WIN64
            uint32 dwAttrib = GetFileAttributes(FILENAME_RUN);
            if (INVALID_FILE_ATTRIBUTES == dwAttrib) {
                break;
            }
#endif
        }

        network_server.event_loop_break(handle_ipv4, 1);
        network_server.event_loop_break(handle_ipv6, 1);
        network_server.consumer_loop_break(handle_ipv4, 2);
        network_server.consumer_loop_break(handle_ipv6, 2);
    }
    __finally2 {
        network_server.close(handle_ipv4);
        network_server.close(handle_ipv6);
    }

    return ret;
}

void run_server() {
    _test_case.begin("echo server (udp powered by network_server)");

    thread thread1(echo_server, nullptr);
    std::string result;

    __try2 { thread1.start(); }
    __finally2 { thread1.wait(-1); }
}

int main(int argc, char** argv) {
#ifdef __MINGW32__
    setvbuf(stdout, 0, _IOLBF, 1 << 20);
#endif

    _cmdline.make_share(new t_cmdline_t<OPTION>);
    *_cmdline << t_cmdarg_t<OPTION>("-v", "verbose", [](OPTION& o, char* param) -> void { o.verbose = 1; }).optional()
              << t_cmdarg_t<OPTION>("-p", "port (9000)", [](OPTION& o, char* param) -> void { o.port = atoi(param); }).optional().preced();
    _cmdline->parse(argc, argv);

    const OPTION& option = _cmdline->value();

    logger_builder builder;
    builder.set(logger_t::logger_stdout, option.verbose).set(logger_t::logger_flush_time, 0).set(logger_t::logger_flush_size, 0);
    _logger.make_share(builder.build());

#if defined _WIN32 || defined _WIN64
    winsock_startup();
#endif
    openssl_startup();
    openssl_thread_setup();

    run_server();

    openssl_thread_end();
    openssl_cleanup();

#if defined _WIN32 || defined _WIN64
    winsock_cleanup();
#endif

    _logger->flush();

    _test_case.report();
    _cmdline->help();
    return _test_case.result();
}
