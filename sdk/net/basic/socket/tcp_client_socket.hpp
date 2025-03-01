/* vim: set tabstop=4 shiftwidth=4 softtabstop=4 expandtab smarttab : */
/**
 * @file {file}
 * @author Soo Han, Kim (princeb612.kr@gmail.com)
 * @desc
 *
 * Revision History
 * Date         Name                Description
 */

#ifndef __HOTPLACE_SDK_NET_BASIC_TCPCLIENTSOCKET__
#define __HOTPLACE_SDK_NET_BASIC_TCPCLIENTSOCKET__

#include <sdk/net/basic/socket/client_socket.hpp>

namespace hotplace {
namespace net {

/**
 * @brief   client socket
 * @sa      class tls_client_socket : public tcp_client_socket
 */
class tcp_client_socket : public client_socket {
   public:
    tcp_client_socket();

    /**
     * @brief   open (tcp/udp)
     */
    virtual return_t open(socket_t* sock, const char* address, uint16 port);
    /**
     * @brief   connect
     * @param   socket_t*       sock            [OUT]
     * @param   tls_context_t** tls_handle      [OUT] ignore, see tls_client_socket
     * @param   const char*     address         [IN]
     * @param   uint16          port            [IN]
     * @param   uint32          timeout         [IN] second
     * @return  error code (see error.hpp)
     */
    virtual return_t connect(socket_t* sock, tls_context_t** tls_handle, const char* address, uint16 port, uint32 timeout);
    /**
     * @brief   connect
     * @oaram   socket_t sock [in]
     * @oaram   tls_context_t** tls_handle [out]
     * @oaram   const char* address [in]
     * @oaram   uint16 port [in]
     * @oaram   uint32 timeout [in[
     */
    virtual return_t connectto(socket_t sock, tls_context_t** tls_handle, const char* address, uint16 port, uint32 timeout);
    /**
     * @brief   connect
     * @param   socket_t sock [in]
     * @param   tls_context_t** tls_handle [out]
     * @param   const sockaddr* addr [in]
     * @param   socklen_t addrlen [in]
     * @param   uint32 timeout [in]
     */
    virtual return_t connectto(socket_t sock, tls_context_t** tls_handle, const sockaddr* addr, socklen_t addrlen, uint32 timeout);

    /**
     * @brief   read
     * @param   socket_t        sock            [IN]
     * @param   tls_context_t*  tls_handle      [IN] ignore, see tls_client_socket
     * @param   char*           ptr_data        [OUT]
     * @param   size_t          size_data       [IN]
     * @param   size_t*         cbread          [OUT]
     * @return  error code (see error.hpp)
     */
    virtual return_t read(socket_t sock, tls_context_t* tls_handle, char* ptr_data, size_t size_data, size_t* cbread);
    /**
     * @brief   more (tcp)
     */
    virtual return_t more(socket_t sock, tls_context_t* tls_handle, char* ptr_data, size_t size_data, size_t* cbread);
    /**
     * @brief   send
     * @param   socket_t        sock            [IN]
     * @param   tls_context_t*  tls_handle      [IN] ignore, see tls_client_socket
     * @param   const char*     ptr_data        [IN]
     * @param   size_t          size_data       [IN]
     * @param   size_t*         size_sent       [OUT]
     * @return  error code (see error.hpp)
     */
    virtual return_t send(socket_t sock, tls_context_t* tls_handle, const char* ptr_data, size_t size_data, size_t* size_sent);

    virtual int socket_type();
};

}  // namespace net
}  // namespace hotplace

#endif
