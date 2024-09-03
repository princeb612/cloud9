/* vim: set tabstop=4 shiftwidth=4 softtabstop=4 expandtab smarttab : */
/**
 * @file {file}
 * @author Soo Han, Kim (princeb612.kr@gmail.com)
 * @desc
 *      UDP not supported
 *
 *      bind socket and multiplexr.handle
 *                 TCP/TLS   DTLS
 *          epoll     O       O
 *          IOCP      -       O
 *      bind client socket and multiplexr.handle
 *                 TCP/TLS   DTLS
 *          epoll     O       -
 *          IOCP      O       -
 *      accept
 *                 TCP/TLS   DTLS
 *          epoll     -       -
 *          IOCP      O       -
 *
 *             mux    type     accept_queue.push
 *          1) epoll  tcp/tls  network_routine.mux_connect->accept_routine
 *          2) epoll  dtls     ...
 *          3) epoll  udp      N/A
 *          4) iocp   tcp/tls  accept_routine
 *          5) iocp   dtls     ...
 *          6) iocp   udp      N/A
 *
 * Revision History
 * Date         Name                Description
 */

#include <queue>
#include <sdk/io.hpp>
#include <sdk/net/server/network_server.hpp>

namespace hotplace {
using namespace io;
namespace net {

#define NETWORK_MULTIPLEXER_CONTEXT_SIGNATURE 0x20151127

struct _network_multiplexer_context_t;
typedef struct _accept_context_t {
    struct _network_multiplexer_context_t* mplexer_context;
    socket_t cli_socket;
    sockaddr_storage_t client_addr;
    socklen_t client_addr_len;

    _accept_context_t() : mplexer_context(nullptr), cli_socket(INVALID_SOCKET) { client_addr_len = sizeof(client_addr); }

} accept_context_t;

typedef std::queue<accept_context_t> accept_queue_t;

typedef struct _network_multiplexer_context_t {
    uint32 signature;

    multiplexer_context_t* mplexer_handle;
    uint32 concurrent;
    TYPE_CALLBACK_HANDLEREXV callback_routine;
    void* callback_param;

    socket_t listen_sock;
    server_socket* svr_socket;

    semaphore tls_accept_mutex;
    // semaphore cleanup_mutex;
    semaphore consumer_mutex;
    signalwait_threads network_threads;
#if defined _WIN32 || defined _WIN64
    signalwait_threads accept_threads;
#endif
    signalwait_threads tls_accept_threads;
    signalwait_threads consumer_threads;

    network_session_manager session_manager;
    t_mlfq<network_session> event_queue;

    network_protocol_group protocol_group;

    accept_queue_t accept_queue;
    critical_section accept_queue_lock;

    ACCEPT_CONTROL_CALLBACK_ROUTINE accept_control_handler;

    net_dgram_t dgram;

    std::function<void(stream_t*)> df;

    _network_multiplexer_context_t()
        : signature(0),
          mplexer_handle(nullptr),
          concurrent(0),
          callback_routine(nullptr),
          callback_param(nullptr),
          listen_sock(INVALID_SOCKET),
          svr_socket(nullptr),
          accept_control_handler(nullptr) {}
} network_multiplexer_context_t;

network_server::network_server() {}

network_server::~network_server() {}

return_t network_server::open(network_multiplexer_context_t** handle, unsigned int family, uint16 port, server_socket* svr_socket, server_conf* conf,
                              TYPE_CALLBACK_HANDLEREXV callback_routine, void* callback_param) {
    return_t ret = errorcode_t::success;

    network_multiplexer_context_t* context = nullptr;
    socket_t sock = INVALID_SOCKET;
    multiplexer_context_t* mplexer_handle = nullptr;

    __try2 {
        if (nullptr == handle || nullptr == callback_routine || nullptr == svr_socket) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        // TCP/TLS socket.stream-bind-listen
        // DTLS    socket.dgram-bind
        ret = svr_socket->open(&sock, family, port);
        if (errorcode_t::success != ret) {
            __leave2;
        }

        uint16 concurrent = conf ? conf->get(netserver_config_t::serverconf_concurrent_event) : 1024;
        uint16 concurrent_tls_accept = conf ? conf->get(netserver_config_t::serverconf_concurrent_tls_accept) : 1;
        uint16 concurrent_network = conf ? conf->get(netserver_config_t::serverconf_concurrent_network) : 1;
        uint16 concurrent_consume = conf ? conf->get(netserver_config_t::serverconf_concurrent_consume) : 2;

        ret = mplexer.open(&mplexer_handle, concurrent);
        if (errorcode_t::success != ret) {
            __leave2;
        }

        __try_new_catch(context, new network_multiplexer_context_t, ret, __leave2);

        int socktype = 0;
        typeof_socket(sock, socktype);
        if (SOCK_STREAM == socktype) {
#if defined __linux__
            // (epoll) bind tcp.socket
            mplexer.bind(mplexer_handle, sock, nullptr);
#endif
#if defined _WIN32 || defined _WIN64
            // use dummy signal handler ... just call CloseListener first, and signal_and_wait_all
            context->accept_threads.set(1, accept_thread, signalwait_threads::dummy_signal, context);
#endif
        }

        context->tls_accept_threads.set(concurrent_tls_accept, tls_accept_thread, tls_accept_signal, context);
        context->network_threads.set(concurrent_network, network_thread, network_signal, context);
        context->consumer_threads.set(concurrent_consume, consumer_thread, consumer_signal, context);

        context->mplexer_handle = mplexer_handle;
        context->concurrent = concurrent;
        context->callback_routine = callback_routine;
        context->callback_param = callback_param;

        context->listen_sock = sock;
        context->svr_socket = svr_socket;

        context->accept_control_handler = nullptr;

        context->signature = NETWORK_MULTIPLEXER_CONTEXT_SIGNATURE;

        if (SOCK_STREAM == socktype) {
#if defined _WIN32 || defined _WIN64
            context->accept_threads.create();
            // (iocp) and then bind client socket after accept
#endif
            if (svr_socket->support_tls()) {
                // (tls) tls_accept
                context->tls_accept_threads.create();
            }
        }

        svr_socket->addref();

        *handle = context;
    }
    __finally2 {
        if (errorcode_t::success != ret) {
            if (nullptr != mplexer_handle) {
                mplexer.close(mplexer_handle);
            }
            if (INVALID_SOCKET != sock) {
                if (svr_socket) {
                    svr_socket->close(sock, nullptr);
                }
            }
            if (nullptr != context) {
                delete context;
            }
        }
    }

    return ret;
}

return_t network_server::set_accept_control_handler(network_multiplexer_context_t* handle, ACCEPT_CONTROL_CALLBACK_ROUTINE accept_control_handler) {
    return_t ret = errorcode_t::success;

    __try2 {
        if (nullptr == handle) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        if (NETWORK_MULTIPLEXER_CONTEXT_SIGNATURE != handle->signature) {
            ret = errorcode_t::invalid_context;
            __leave2;
        }

        handle->accept_control_handler = accept_control_handler;
    }
    __finally2 {
        // do nothing
    }

    return ret;
}

return_t network_server::add_protocol(network_multiplexer_context_t* handle, network_protocol* protocol_ptr) {
    return_t ret = errorcode_t::success;

    __try2 {
        if (nullptr == handle) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        if (NETWORK_MULTIPLEXER_CONTEXT_SIGNATURE != handle->signature) {
            ret = errorcode_t::invalid_context;
            __leave2;
        }

        ret = handle->protocol_group.add(protocol_ptr);
    }
    __finally2 {
        // do nothing
    }

    return ret;
}

return_t network_server::remove_protocol(network_multiplexer_context_t* handle, network_protocol* protocol_ptr) {
    return_t ret = errorcode_t::success;

    __try2 {
        if (nullptr == handle) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        if (NETWORK_MULTIPLEXER_CONTEXT_SIGNATURE != handle->signature) {
            ret = errorcode_t::invalid_context;
            __leave2;
        }

        ret = handle->protocol_group.remove(protocol_ptr);
    }
    __finally2 {
        // do nothing
    }

    return ret;
}

return_t network_server::clear_protocols(network_multiplexer_context_t* handle) {
    return_t ret = errorcode_t::success;

    __try2 {
        if (nullptr == handle) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        if (NETWORK_MULTIPLEXER_CONTEXT_SIGNATURE != handle->signature) {
            ret = errorcode_t::invalid_context;
            __leave2;
        }

        ret = handle->protocol_group.clear();
    }
    __finally2 {
        // do nothing
    }

    return ret;
}

return_t network_server::tls_accept_loop_run(network_multiplexer_context_t* handle, uint32 concurrent_loop) {
    return_t ret = errorcode_t::success;

    __try2 {
        if (nullptr == handle) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        if (NETWORK_MULTIPLEXER_CONTEXT_SIGNATURE != handle->signature) {
            ret = errorcode_t::invalid_context;
            __leave2;
        }

        server_socket* svr_socket = handle->svr_socket;
        if (svr_socket->support_tls()) {
            for (uint32 i = 0; i < concurrent_loop; i++) {
                handle->tls_accept_threads.create();
            }
        }
    }
    __finally2 {
        // do nothing
    }

    return ret;
}

return_t network_server::tls_accept_loop_break(network_multiplexer_context_t* handle, uint32 concurrent_loop) {
    return_t ret = errorcode_t::success;

    __try2 {
        if (nullptr == handle) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        if (NETWORK_MULTIPLEXER_CONTEXT_SIGNATURE != handle->signature) {
            ret = errorcode_t::invalid_context;
            __leave2;
        }

        for (uint32 i = 0; i < concurrent_loop; i++) {
            handle->tls_accept_threads.join();
        }
    }
    __finally2 {
        // do nothing
    }

    return ret;
}

return_t network_server::event_loop_run(network_multiplexer_context_t* handle, uint32 concurrent_loop) {
    return_t ret = errorcode_t::success;

    __try2 {
        if (nullptr == handle) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        if (NETWORK_MULTIPLEXER_CONTEXT_SIGNATURE != handle->signature) {
            ret = errorcode_t::invalid_context;
            __leave2;
        }

        /* if a thread count of network_threads reachs max-concurrent, no more thread is created. */
        for (uint32 i = 0; i < concurrent_loop; i++) {
            handle->network_threads.create();
        }

        if (SOCK_DGRAM == handle->svr_socket->socket_type()) {
            // (epoll, iocp) bind udp.socket - mplexer.bind(mplexer_handle, (handle_t)sock, session_object);
            // (windows) async read
            dgram_start(handle);
        }
    }
    __finally2 {
        // do nothing
    }

    return ret;
}

return_t network_server::event_loop_break(network_multiplexer_context_t* handle, uint32 concurrent_loop) {
    return_t ret = errorcode_t::success;

    __try2 {
        if (nullptr == handle) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        if (NETWORK_MULTIPLEXER_CONTEXT_SIGNATURE != handle->signature) {
            ret = errorcode_t::invalid_context;
            __leave2;
        }

        /* stop threads */
        uint32 i = 0;
        for (i = 0; i < concurrent_loop; i++) {
            handle->network_threads.join();
        }
    }
    __finally2 {
        // do nothing
    }

    return ret;
}

return_t network_server::consumer_loop_run(network_multiplexer_context_t* handle, uint32 concurrent_loop) {
    return_t ret = errorcode_t::success;

    __try2 {
        if (nullptr == handle) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        if (NETWORK_MULTIPLEXER_CONTEXT_SIGNATURE != handle->signature) {
            ret = errorcode_t::invalid_context;
            __leave2;
        }

        /* if a thread count of consumer_threads reachs max-concurrent, no more thread is created. */
        for (uint32 i = 0; i < concurrent_loop; i++) {
            handle->consumer_threads.create();
        }
    }
    __finally2 {
        // do nothing
    }

    return ret;
}

return_t network_server::consumer_loop_break(network_multiplexer_context_t* handle, uint32 concurrent_loop) {
    return_t ret = errorcode_t::success;

    __try2 {
        if (nullptr == handle) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        if (NETWORK_MULTIPLEXER_CONTEXT_SIGNATURE != handle->signature) {
            ret = errorcode_t::invalid_context;
            __leave2;
        }

        /* stop threads */
        for (uint32 i = 0; i < concurrent_loop; i++) {
            handle->consumer_threads.join();
        }
    }
    __finally2 {
        // do nothing
    }

    return ret;
}

return_t network_server::close(network_multiplexer_context_t* handle) {
    return_t ret = errorcode_t::success;

    __try2 {
        if (nullptr == handle) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        if (NETWORK_MULTIPLEXER_CONTEXT_SIGNATURE != handle->signature) {
            ret = errorcode_t::invalid_context;
            __leave2;
        }

        server_socket* svr_socket = handle->svr_socket;
        svr_socket->close(handle->listen_sock, nullptr);

        /* stop all threads */
#if defined _WIN32 || defined _WIN64
        handle->accept_threads.signal_and_wait_all();
#endif
        cleanup_tls_accept(handle);
        handle->tls_accept_threads.signal_and_wait_all();
        handle->network_threads.signal_and_wait_all();
        handle->consumer_threads.signal_and_wait_all();

        handle->protocol_group.clear();

        svr_socket->release();

        mplexer.close(handle->mplexer_handle);

        handle->signature = 0;
        delete handle;
    }
    __finally2 {
        // do nothing
    }

    return ret;
}

return_t network_server::accept_thread(void* user_context) {
    return_t ret = errorcode_t::success;
    network_multiplexer_context_t* handle = reinterpret_cast<network_multiplexer_context_t*>(user_context);

    __try2 {
        if (nullptr == handle) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        if (NETWORK_MULTIPLEXER_CONTEXT_SIGNATURE != handle->signature) {
            ret = errorcode_t::invalid_context;
            __leave2;
        }

        network_server svr;
        do {
            ret = svr.accept_routine(handle);
        } while (errorcode_t::success == ret);
    }
    __finally2 {
        // do nothing
    }

    return ret;
}

return_t network_server::accept_routine(network_multiplexer_context_t* handle) {
    // iocp tcp only
    return_t ret = errorcode_t::success;
    network_server svr;

    __try2 {
        if (nullptr == handle) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        socket_t listen_sock = (socket_t)handle->listen_sock;
        server_socket* svr_socket = handle->svr_socket;
        if (nullptr == svr_socket) {
            __leave2;
        }

        accept_context_t accpt_ctx;
        accpt_ctx.mplexer_context = handle;

        ret = svr_socket->accept(listen_sock, &accpt_ctx.cli_socket, (struct sockaddr*)&accpt_ctx.client_addr, &accpt_ctx.client_addr_len);
        if (INVALID_SOCKET == accpt_ctx.cli_socket) {
#if defined __MINGW32__
            /* mingw environments GetLastError () return 0 */
            ret = errorcode_t::canceled;
#else
            ret = get_lasterror(ret);
#endif
            __leave2;
        }

        /* allow/deny based on a network address */
        /* if protocol upgrade needed, use accept_control_handler callback */
        if (nullptr != handle->accept_control_handler) {
            CALLBACK_CONTROL control = CONTINUE_CONTROL;
            handle->accept_control_handler(accpt_ctx.cli_socket, &accpt_ctx.client_addr, &control, handle->callback_param);
            if (STOP_CONTROL == control) {
                close_socket(accpt_ctx.cli_socket, true, 0);
                __leave2;
            }
        }

        /*
         * it can be accomplished by using follows...
         *
         * svr_socket->tls_accept(cli_socket, &tls_handle);
         * ret = svr.session_accepted(handle, tls_handle, (handle_t)cli_socket, &client_addr);
         *
         * sometimes it takes long time by calling ssl_accept
         * so, separate thread to improve accept performance
         */

        if (svr_socket->support_tls()) {
            /* prepare for ssl_accept delay */
            {
                critical_section_guard guard(handle->accept_queue_lock);
                handle->accept_queue.push(accpt_ctx);
            }

            svr.try_connect(handle, accpt_ctx.cli_socket, &accpt_ctx.client_addr);
        } else {
            ret = svr.session_accepted(handle, nullptr, (handle_t)accpt_ctx.cli_socket, &accpt_ctx.client_addr);
        }
    }
    __finally2 {
        // do nothing
    }

    return ret;
}

return_t network_server::try_connect(network_multiplexer_context_t* handle, socket_t event_socket, sockaddr_storage_t* client_addr) {
    // see accept_routine
    return_t ret = errorcode_t::success;

    __try2 {
        if (nullptr == handle) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        if (NETWORK_MULTIPLEXER_CONTEXT_SIGNATURE != handle->signature) {
            ret = errorcode_t::invalid_context;
            __leave2;
        }

        void* dispatch_data[4] = {
            nullptr,
        };
        dispatch_data[0] = (void*)(arch_t)event_socket;
        dispatch_data[1] = client_addr;
        handle->callback_routine(multiplexer_event_type_t::mux_tryconnect, 4, dispatch_data, nullptr, handle->callback_param);
    }
    __finally2 {
        // do nothing
    }
    return ret;
}

return_t network_server::tls_accept_ready(network_multiplexer_context_t* handle, bool* ready) {
    // see tls_accept_thread
    return_t ret = errorcode_t::success;

    __try2 {
        if (nullptr == handle || nullptr == ready) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        *ready = handle->accept_queue.empty() ? false : true;
    }
    __finally2 {
        // do nothing
    }
    return ret;
}

return_t network_server::tls_accept_thread(void* user_context) {
    // tls, dtls
    return_t ret = errorcode_t::success;
    network_multiplexer_context_t* handle = reinterpret_cast<network_multiplexer_context_t*>(user_context);

    __try2 {
        if (nullptr == handle) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        network_server svr;
        uint32 ret_wait = 0;
        uint32 interval = 1;
        bool ready = false;

        while (true) {
            ready = false;
            svr.tls_accept_ready(handle, &ready);

            interval = (true == ready) ? 1 : 100; /* control cpu usage */
            ret_wait = handle->tls_accept_mutex.wait(interval);
            if (0 == ret_wait) {
                break;
            }

            if (true == ready) {
                svr.tls_accept_routine(handle);
            }
        }

        // openssl_thread_end (); // ssl23_accept memory leak, call for each thread
        handle->svr_socket->tls_stop_accept();
    }
    __finally2 {
        // do nothing
    }

    return ret;
}

return_t network_server::tls_accept_routine(network_multiplexer_context_t* handle) {
    // see tls_accept_thread
    return_t ret = errorcode_t::success;

    __try2 {
        if (nullptr == handle) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        accept_context_t accpt_ctx;
        {
            critical_section_guard guard(handle->accept_queue_lock);
            if (false == handle->accept_queue.empty()) {
                accpt_ctx = handle->accept_queue.front();
                handle->accept_queue.pop();
            } else {
                ret = errorcode_t::empty;
            }
        }

        if (errorcode_t::success != ret) {
            __leave2;
        }

        socket_t listen_sock = (socket_t)handle->listen_sock;

        int socktype = 0;
        typeof_socket(listen_sock, socktype);
        bool is_stream = (SOCK_STREAM == socktype);
        bool is_dgram = (SOCK_DGRAM == socktype);

        network_server svr;
        server_socket* svr_socket = handle->svr_socket;
        tls_context_t* tls_handle = nullptr;

        return_t test = errorcode_t::success;

        test = svr_socket->tls_accept(accpt_ctx.cli_socket, &tls_handle);
        if (errorcode_t::success == test) {
            svr.session_accepted(handle, tls_handle, (handle_t)accpt_ctx.cli_socket, &accpt_ctx.client_addr);
            /* tls_handle is release in session_closed member. */
        } else {
            close_socket(accpt_ctx.cli_socket, true, 0);
        }
    }
    __finally2 {
        // do nothing
    }

    return ret;
}

return_t network_server::tls_accept_signal(void* user_context) {
    // tls, dtls
    return_t ret = errorcode_t::success;
    network_multiplexer_context_t* handle = reinterpret_cast<network_multiplexer_context_t*>(user_context);

    __try2 {
        if (nullptr == handle) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        handle->tls_accept_mutex.signal();
    }
    __finally2 {
        // do nothing
    }

    return ret;
}

return_t network_server::cleanup_tls_accept(network_multiplexer_context_t* handle) {
    // see close
    return_t ret = errorcode_t::success;

    __try2 {
        if (nullptr == handle) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        if (NETWORK_MULTIPLEXER_CONTEXT_SIGNATURE != handle->signature) {
            ret = errorcode_t::invalid_context;
            __leave2;
        }

        accept_context_t accpt_ctx;
        critical_section_guard guard(handle->accept_queue_lock);
        if (false == handle->accept_queue.empty()) {
            accpt_ctx = handle->accept_queue.front();
            handle->accept_queue.pop();
            close_socket(accpt_ctx.cli_socket, true, 0);
        }
    }
    __finally2 {
        // do nothing
    }
    return ret;
}

return_t network_server::network_thread(void* user_context) {
    return_t ret = errorcode_t::success;
    network_multiplexer_context_t* handle = reinterpret_cast<network_multiplexer_context_t*>(user_context);

    __try2 {
        if (nullptr == handle) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        if (NETWORK_MULTIPLEXER_CONTEXT_SIGNATURE != handle->signature) {
            ret = errorcode_t::invalid_context;
            __leave2;
        }

        network_server svr;
        ret = svr.mplexer.event_loop_run(handle->mplexer_handle, (handle_t)handle->listen_sock, network_routine, handle);
    }
    __finally2 {
        // do nothing
    }

    return ret;
}

return_t network_server::network_routine(uint32 type, uint32 data_count, void* data_array[], CALLBACK_CONTROL* callback_control, void* user_context) {
    // see network_thread
    return_t ret = errorcode_t::success;
    network_multiplexer_context_t* context = static_cast<network_multiplexer_context_t*>(user_context);
    network_server svr;

#if defined __linux__

    // void* handle = data_array[0];

    if (multiplexer_event_type_t::mux_connect == type) {
        svr.accept_routine(context);
    } else if (multiplexer_event_type_t::mux_read == type) {
        int sockcli = (int)(long)data_array[1];

        network_session* session_object = nullptr;
        ret = context->session_manager.find(sockcli, &session_object); /* reference increased, call release later */
        if (errorcode_t::success == ret) {
            /* consumer_routine (decrease), close_if_not_referenced (delete) */
            ret = session_object->produce(&context->event_queue, nullptr, 0);

            session_object->release(); /* find, refcount-- */

            if (errorcode_t::success == ret) {
                // do nothing
            } else {
                svr.session_closed(context, sockcli); /* call session_object->release() inside of closed */
            }
        }
    } else if (multiplexer_event_type_t::mux_dgram == type) {
        network_session* session_object = nullptr;
        ret = context->session_manager.find(context->listen_sock, &session_object); /* reference increased, call release later */
        if (errorcode_t::success == ret) {
            /* consumer_routine (decrease), close_if_not_referenced (delete) */
            ret = session_object->produce(&context->event_queue, nullptr, 0);

            session_object->release(); /* find, refcount-- */
        }
    }
    // else if (multiplexer_event_type_t::mux_disconnect == type) /* no event catchable */

#elif defined _WIN32 || defined _WIN64

    uint32 transferred = (uint32)(arch_t)data_array[1];
    network_session* session_object = (network_session*)data_array[2];
    if (multiplexer_event_type_t::mux_read == type) {
        /* consumer_routine (decrease), close_if_not_referenced (delete) */
        session_object->produce(&context->event_queue, (byte_t*)session_object->wsabuf_read()->buf, transferred);
        /* asynchronous write */
        session_object->ready_to_read();
    } else if (multiplexer_event_type_t::mux_disconnect == type) {
        svr.session_closed(context, session_object->socket_info()->event_socket);
    } else if (multiplexer_event_type_t::mux_dgram) {
        // socket_t listen_sock = context->listen_sock;
        net_dgram_wsabuf_t& wsabuf_read = context->dgram.wsabuf_pair.r;
        sockaddr_storage_t& addr = context->dgram.netsock.cli_addr;
        session_object->produce(&context->event_queue, (byte_t*)wsabuf_read.buffer, transferred, &addr);
        svr.dgram_ready_to_read(context);
    }

#endif

    return ret;
}

return_t network_server::network_signal(void* user_context) {
    return_t ret = errorcode_t::success;
    network_multiplexer_context_t* handle = reinterpret_cast<network_multiplexer_context_t*>(user_context);

    __try2 {
        if (nullptr == handle) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        network_server svr;
        svr.mplexer.event_loop_break_concurrent(handle->mplexer_handle, 1); /* call event_loop_break just 1 time */
    }
    __finally2 {
        // do nothing
    }

    return ret;
}

return_t network_server::consumer_thread(void* user_context) {
    return_t ret = errorcode_t::success;
    network_multiplexer_context_t* handle = reinterpret_cast<network_multiplexer_context_t*>(user_context);

    __try2 {
        if (nullptr == handle) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }

        network_server svr;
        uint32 ret_wait = 0;

        while (true) {
            ret_wait = handle->consumer_mutex.wait(100);
            if (0 == ret_wait) {
                break;
            }

            svr.consumer_routine(handle);
        }
    }
    __finally2 {
        // do nothing
    }

    return ret;
}

return_t network_server::consumer_routine(network_multiplexer_context_t* handle) {
    // see consumer_thread
    return_t ret = errorcode_t::success;

    if (handle->event_queue.size()) {
        int priority = 0;
        network_session* session_object = nullptr;
        ret = handle->event_queue.pop(&priority, &session_object, 1);  // session priority
        if (errorcode_t::success == ret) {
            // re-order by stream priority
            network_stream_data* buffer_object = nullptr;
            session_object->consume(&handle->protocol_group, &buffer_object);  // set stream priority while processing network_protocol::read_stream
            sockaddr_storage_t addr;

            while (buffer_object) {
                buffer_object->get_sockaddr(&addr);

                void* dispatch_data[6] = {
                    nullptr,
                };
                dispatch_data[0] = session_object->socket_info(); /* netserver_cb_type_t::netserver_cb_socket */
                dispatch_data[1] = buffer_object->content();      /* netserver_cb_type_t::netserver_cb_dataptr */
                dispatch_data[2] = (void*)buffer_object->size();  /* netserver_cb_type_t::netserver_cb_datasize */
                dispatch_data[3] = session_object;                /* netserver_cb_type_t::netserver_cb_session */
                dispatch_data[5] = &addr;                         /* netserver_cb_type_t::netserver_cb_sockaddr */

                int socktype = session_object->get_server_socket()->socket_type();
                auto muxtype = (SOCK_STREAM == socktype) ? multiplexer_event_type_t::mux_read : multiplexer_event_type_t::mux_dgram;
                handle->callback_routine(muxtype, RTL_NUMBER_OF(dispatch_data), dispatch_data, nullptr, handle->callback_param);

                network_stream_data* temp = buffer_object;
                buffer_object = buffer_object->next();
                temp->release();
            }

            session_object->release();
        }
    } else {
        msleep(10);
    }

    return ret;
}

return_t network_server::consumer_signal(void* user_context) {
    return_t ret = errorcode_t::success;
    network_multiplexer_context_t* handle = reinterpret_cast<network_multiplexer_context_t*>(user_context);

    __try2 {
        if (nullptr == handle) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        handle->consumer_mutex.signal();
    }
    __finally2 {
        // do nothing
    }

    return ret;
}

return_t network_server::session_accepted(network_multiplexer_context_t* handle, tls_context_t* tls_handle, handle_t event_socket,
                                          sockaddr_storage_t* client_addr) {
    // see accept_routine, tls_accept_routine
    return_t ret = errorcode_t::success;

    __try2 {
        if ((nullptr == handle) || (INVALID_SOCKET == (socket_t)event_socket) || (nullptr == client_addr)) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        if (NETWORK_MULTIPLEXER_CONTEXT_SIGNATURE != handle->signature) {
            ret = errorcode_t::invalid_context;
            __leave2;
        }

        network_session* session_object;
        ret = handle->session_manager.connected(event_socket, client_addr, handle->svr_socket, tls_handle, &session_object);
        if (errorcode_t::success != ret) {
            __leave2;
        }

        if (handle->df) {
            session_object->trace(handle->df);
        }

        /* associate with multiplex object (iocp, epoll) */
        mplexer.bind(handle->mplexer_handle, event_socket, session_object);
#if defined _WIN32 || defined _WIN64
        /* asynchronous */
        session_object->ready_to_read();
#endif

        void* dispatch_data[4] = {
            nullptr,
        };
        dispatch_data[0] = (void*)session_object->socket_info(); /* NET_OBJECT_SOCKET* */
        handle->callback_routine(multiplexer_event_type_t::mux_connect, 4, dispatch_data, nullptr, handle->callback_param);
    }
    __finally2 {
        // do nothing
    }

    return ret;
}

return_t network_server::dgram_start(network_multiplexer_context_t* handle) {
    return_t ret = errorcode_t::success;

    socket_t listen_sock = handle->listen_sock;
    network_session* session_object = nullptr;
    handle->session_manager.dgram_start((handle_t)listen_sock, handle->svr_socket, nullptr, &session_object);
    mplexer.bind(handle->mplexer_handle, (handle_t)listen_sock, session_object);
#if defined _WIN32 || defined _WIN64
    dgram_ready_to_read(handle);
#endif
    return ret;
}

return_t network_server::dgram_ready_to_read(network_multiplexer_context_t* handle) {
    return_t ret = errorcode_t::success;
    socket_t listen_sock = handle->listen_sock;
#if defined _WIN32 || defined _WIN64
    net_dgram_wsabuf_t& wsabuf_read = handle->dgram.wsabuf_pair.r;
    sockaddr_storage_t& addr = handle->dgram.netsock.cli_addr;
    uint32 flags = 0;
    wsabuf_read.init();
    int addrlen = sizeof(sockaddr_storage_t);
    WSARecvFrom(listen_sock, &wsabuf_read.wsabuf, 1, nullptr, &flags, (sockaddr*)&addr, &addrlen, &wsabuf_read.overlapped, nullptr);
#endif
    return ret;
}

return_t network_server::session_closed(network_multiplexer_context_t* handle, handle_t event_socket) {
    // see network_routine
    return_t ret = errorcode_t::success;

    __try2 {
        if (nullptr == handle || INVALID_SOCKET == (socket_t)event_socket) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        if (NETWORK_MULTIPLEXER_CONTEXT_SIGNATURE != handle->signature) {
            ret = errorcode_t::invalid_context;
            __leave2;
        }

        network_session* session_object = nullptr;
        /* remove from session manager, prevent double free(concurrent epoll_wait) */
        ret = handle->session_manager.ready_to_close(event_socket, &session_object);
        if (errorcode_t::success == ret) {
            /* no more associated, control_delete */
            mplexer.unbind(handle->mplexer_handle, session_object->socket_info()->event_socket, nullptr);

            void* dispatch_data[4] = {
                nullptr,
            };
            dispatch_data[0] = (void*)session_object->socket_info(); /* NET_OBJECT_SOCKET* */
            handle->callback_routine(multiplexer_event_type_t::mux_disconnect, 4, dispatch_data, nullptr, handle->callback_param);

            /* end-of-life. if reference counter is 0, close a socket and delete an instance */
            /* and release tls_handle here */
            session_object->release();
        }
    }
    __finally2 {
        // do nothing
    }

    return ret;
}

return_t network_server::trace(network_multiplexer_context_t* handle, std::function<void(stream_t*)> f) {
    return_t ret = errorcode_t::success;
    __try2 {
        if (nullptr == handle) {
            ret = errorcode_t::invalid_parameter;
            __leave2;
        }
        handle->df = f;
    }
    __finally2 {
        // do nothing
    }
    return ret;
}

}  // namespace net
}  // namespace hotplace
