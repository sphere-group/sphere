#ifndef WIN32_NETWORK_HPP
#define WIN32_NETWORK_HPP
typedef struct NSOCKETimp* NSOCKET;
extern bool    GetLocalName(char* name, int size);
extern bool    GetLocalAddress(char* name, int size);
extern NSOCKET OpenAddress(const char* name, int port);
extern NSOCKET ListenOnPort(int port);
extern void    CloseSocket(NSOCKET socket);
extern bool    IsConnected(NSOCKET socket);
extern int     GetPendingReadSize(NSOCKET socket);
extern int     SocketRead(NSOCKET socket, void* buffer, int size);
extern void    SocketWrite(NSOCKET socket, void* buffer, int size);
#endif
