
#include <stdio.h>
#include <winsock2.h>
#include "win32_network.hpp"

struct NSOCKETimp
{
    SOCKET socket;
    bool is_connected;
    bool is_listening;
};
#ifdef CYGWIN
#define _snprintf snprintf
#endif
static bool s_NetworkInitialized = false;
////////////////////////////////////////////////////////////////////////////////
bool InitNetworkSystem()
{
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(1, 1), &wsadata) != 0)
    {

        return false;
    }
    s_NetworkInitialized = true;
    return true;
}
////////////////////////////////////////////////////////////////////////////////
bool CloseNetworkSystem()
{
    WSACleanup();
    s_NetworkInitialized = false;
    return true;
}
////////////////////////////////////////////////////////////////////////////////
bool GetLocalName(char* name, int size)
{
    if (s_NetworkInitialized)
    {

        name[size - 1] = 0;
        return (gethostname(name, size - 1) == 0);
    }
    else
    {

        return false;
    }
}
////////////////////////////////////////////////////////////////////////////////
bool GetLocalAddress(char* name, int size)
{
    if (s_NetworkInitialized)
    {

        hostent* he = gethostbyname(NULL);
        if (he == NULL)
        {

            return false;
        }
        else
        {

            name[size - 1] = 0;
            in_addr* addr = (in_addr*)he->h_addr;
            _snprintf(name, size - 1, "%d.%d.%d.%d",
                      addr->S_un.S_un_b.s_b1,
                      addr->S_un.S_un_b.s_b2,
                      addr->S_un.S_un_b.s_b3,
                      addr->S_un.S_un_b.s_b4);
            return true;
        }
    }
    else
    {

        return false;
    }
}
////////////////////////////////////////////////////////////////////////////////
NSOCKET OpenAddress(const char* name, int port)
{
    if (!s_NetworkInitialized)
    {

        return NULL;
    }
    // lookup address
    struct hostent* he = gethostbyname(name);
    if (he == NULL)
    {

        return NULL;
    }
    in_addr address = *(in_addr*)(he->h_addr);
    NSOCKET s = new NSOCKETimp;
    if (!s)
    {

        return NULL;
    }
    s->socket = INVALID_SOCKET;
    s->is_connected = false;
    s->is_listening = false;
    // create socket
    s->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s->socket == INVALID_SOCKET)
    {

        delete s;
        return NULL;
    }
    // set nonblocking mode
    u_long blocking = 1;
    if (ioctlsocket(s->socket, FIONBIO, &blocking) != 0)
    {

        closesocket(s->socket);
        delete s;
        return NULL;
    }
    // connect
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr   = address;
    addr.sin_port   = htons(port);
    if (connect(s->socket, (sockaddr*)&addr, sizeof(addr)) != 0)
    {

        if (WSAGetLastError() != WSAEWOULDBLOCK)
        {

            closesocket(s->socket);
            delete s;
            return NULL;
        }
    }
    return s;
}
////////////////////////////////////////////////////////////////////////////////
NSOCKET ListenOnPort(int port)
{
    if (!s_NetworkInitialized)
    {

        return NULL;
    }
    NSOCKET s = new NSOCKETimp;
    if (!s)
        return NULL;
    s->socket = INVALID_SOCKET;
    s->is_connected = false;
    s->is_listening = false;
    // create socket
    s->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s->socket == INVALID_SOCKET)
    {

        delete s;
        return NULL;
    }
    // set nonblocking mode
    u_long blocking = 1;
    if (ioctlsocket(s->socket, FIONBIO, &blocking) != 0)
    {

        closesocket(s->socket);
        delete s;
        return NULL;
    }
    // bind to any address and an incoming port
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(port);
    if (bind(s->socket, (sockaddr*)&addr, sizeof(addr)) != 0)
    {

        closesocket(s->socket);
        delete s;
        return NULL;
    }
    // enable listening state
    if (listen(s->socket, 1) != 0)
    {

        closesocket(s->socket);
        delete s;
        return NULL;
    }
    s->is_listening = true;
    return s;
}
////////////////////////////////////////////////////////////////////////////////
void CloseSocket(NSOCKET socket)
{
    closesocket(socket->socket);
    delete socket;
    socket = NULL;
}
////////////////////////////////////////////////////////////////////////////////
bool IsConnected(NSOCKET socket)
{
    if (socket->is_connected)
    {

        // check
        return true;
    }
    else
    {

        // check to see if we're connected
        if (socket->is_listening)
        {

            sockaddr addr;
            memset(&addr, 0, sizeof(addr));
            int addr_len = sizeof(addr);
            SOCKET connection = accept(socket->socket, &addr, &addr_len);
            if (connection != INVALID_SOCKET)
            {

                closesocket(socket->socket);
                socket->socket = connection;
                socket->is_connected = true;
                return true;
            }
            return false;
        }
        else
        {

            fd_set writefds;
            FD_ZERO(&writefds);
            FD_SET(socket->socket, &writefds);
            // only wait a millisecond at most
            timeval tv;
            tv.tv_sec  = 0;
            tv.tv_usec = 1000;
            if (select(0, NULL, &writefds, NULL, &tv) == SOCKET_ERROR)
            {

                return false;
            }
            if (FD_ISSET(socket->socket, &writefds))
            {

                socket->is_connected = true;
                return true;
            }
            return false;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////
int GetPendingReadSize(NSOCKET socket)
{
    if (IsConnected(socket))
    {

        u_long size = 0;
        if (ioctlsocket(socket->socket, FIONREAD, &size) == 0)
        {

            return size;
        }
        else
        {

            return -1;
        }
    }
    else
    {

        return -1;
    }
}
////////////////////////////////////////////////////////////////////////////////
int SocketRead(NSOCKET socket, void* buffer, int size)
{
    if (IsConnected(socket))
    {

        return recv(socket->socket, (char*)buffer, size, 0);
    }
    return -1;
}
////////////////////////////////////////////////////////////////////////////////
void SocketWrite(NSOCKET socket, void* buffer, int size)
{
    if (IsConnected(socket))
    {

        send(socket->socket, (char*)buffer, size, 0);
    }
}
////////////////////////////////////////////////////////////////////////////////
