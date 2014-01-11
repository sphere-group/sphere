#ifndef UNIX_NETWORK_H
#define UNIX_NETWORK_H

struct NSOCKETimp {
  int socket;
  bool is_connected;
  bool is_listening;
};
typedef struct NSOCKETimp* NSOCKET;

bool    GetLocalName(char* name, int size);
bool    GetLocalAddress(char* name, int size);
NSOCKET OpenAddress(const char* name, int port);
NSOCKET ListenOnPort(int port);
void    CloseSocket(NSOCKET socket);
bool    IsConnected(NSOCKET socket);
int     GetPendingReadSize(NSOCKET socket);
int     SocketRead(NSOCKET socket, void* buffer, int size);
void    SocketWrite(NSOCKET socket, void* buffer, int size);

#endif /* UNIX_NETWORK_H */
