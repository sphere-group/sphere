#include "unix_network.h"
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <sys/poll.h>

#define POLLSTANDARD (POLLIN|POLLPRI|POLLOUT|POLLRDNORM|POLLRDBAND|POLLWRBAND|POLLERR|POLLHUP|POLLNVAL)

#define PRINT_STANDARD_ERROR(infn, causefn) std::cerr << #infn ":" #causefn ": " << strerror(errno) << std::endl;
#define PRINT_HOST_ERROR(infn, causefn) std::cerr << #infn ":" #causefn ": " << hstrerror(h_errno) << std::endl;

const int MAX_LINE = 1024;

bool GetLocalName (char* name, int size) {
  if ((gethostname(name, size) < 0) && (errno != ENAMETOOLONG))
    return false;
  return true;
}

bool GetLocalAddress (char* name, int size) {
  const int HOSTNAME_MAX_LENGTH = 256;
  char hostname [HOSTNAME_MAX_LENGTH];
  if (!GetLocalName(hostname, HOSTNAME_MAX_LENGTH))
    return false;

  struct hostent *host = NULL;
  if ((host = gethostbyname(hostname)) == NULL)
    return false;

  char *address = NULL;
  address = inet_ntoa(*(struct in_addr*)host->h_addr);
  /* will the address fit? */
  if (strlen(address) > size) {
    free(host);
    return false;
  }
  strcpy(name, address);

  return true;
}

NSOCKET OpenAddress (const char* name, int port) {
  /* make a new client socket */
  NSOCKET sock = new NSOCKETimp;
  if (sock == NULL)
    return NULL;
  sock->is_connected = false;
  sock->is_listening = false;
  sock->socket = socket(AF_INET, SOCK_STREAM, 0);
  if (sock->socket == -1) {
    PRINT_STANDARD_ERROR(OpenAddress, socket);
    delete sock;
    return NULL;
  }

  /* make the socket non-blocking */
  fcntl(sock->socket, F_SETFL, O_NONBLOCK);

  /* figure out who we're connecting to */
  struct hostent* host;
  if ((host = gethostbyname(name)) == NULL) {
    PRINT_HOST_ERROR(OpenAddress, gethostbyname);
    delete sock;
    return NULL;
  }

  /* set destination */
  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr = *((struct in_addr *)host->h_addr);
  memset(server.sin_zero, '\0', sizeof(server.sin_zero));

  /* begin non-blocking connection attempt */
  if (connect(sock->socket, (struct sockaddr *)&server, sizeof(server)) == -1 && errno != EINPROGRESS) {
    PRINT_STANDARD_ERROR(OpenAddress, connect);
    delete sock;
    return NULL;
  }

  return sock;
}

NSOCKET ListenOnPort (int port) {
  /* make a new port-listening server socket */
  NSOCKET sock = new NSOCKETimp;
  if (sock == NULL)
    return NULL;
  sock->is_connected = false;
  sock->is_listening = false;
  sock->socket = socket(PF_INET, SOCK_STREAM, 0);
  if (sock->socket == -1) {
    PRINT_STANDARD_ERROR(ListenOnPort, socket);
    delete sock;
    return NULL;
  }

  /* make the socket non-blocking */
  fcntl(sock->socket, F_SETFL, O_NONBLOCK);

  /* allow port reuse */
  int yes = 1;
  if (setsockopt(sock->socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
    PRINT_STANDARD_ERROR(ListenOnPort, setsockopt);
    delete sock;
    return NULL;
  }

  /* set our listening port */
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = htonl(INADDR_ANY);
  memset(address.sin_zero, '\0', sizeof(address.sin_zero));

  /* bind to the port */
  if (bind(sock->socket, (struct sockaddr*)&address, sizeof(address)) == -1) {
    PRINT_STANDARD_ERROR(ListenOnPort, bind);
    delete sock;
    return NULL;
  }

  /* start listening on the port */
  const int BACKLOG_SIZE = 16; /* not sure about an optimum backlog value */
  if (listen(sock->socket, BACKLOG_SIZE) == -1) {
    PRINT_STANDARD_ERROR(ListenOnPort, listen);
    delete sock;
    return NULL;
  } else {
    sock->is_listening = true;
  }

  return sock;
}

void CloseSocket (NSOCKET socket) {
  close(socket->socket);
  delete socket;
}

bool IsConnected (NSOCKET socket) {
  /* poll on the one socket we have */
  struct pollfd sock_evs = {socket->socket, POLLSTANDARD, 0};
  int connection = poll(&sock_evs, 1, 0);
  if (connection == -1) {
    PRINT_STANDARD_ERROR(IsConnected, poll);
    socket->is_connected = false;
  }

  /* check if poll found anything interesting */
  if (connection > 0) {

    if (sock_evs.revents & POLLOUT && !socket->is_connected) {
      /* client signals ready with POLLOUT */

      /* client sockets need to see if their connect() succeeded
         method: 2nd last of http://cr.yp.to/docs/connect.html */
      struct sockaddr_in server;
      socklen_t len = sizeof(server);
      if (getpeername(socket->socket, (struct sockaddr *)&server, &len) == -1) {
        /* too bad */
        if (errno == ENOTCONN)
          std::cerr << "IsConnected: not connected, " << std::flush;
        char dummy;
        read(socket->socket, &dummy, 1);
        PRINT_STANDARD_ERROR(IsConnected, read);
        socket->is_connected = false;
      } else {
        /* we're in! */
        socket->is_connected = true;
      }

    } else if (sock_evs.revents & POLLIN && socket->is_listening) {
      /* server detects incoming connections with POLLIN */

      /* switch out the port listener for a client handling socket */
      struct sockaddr_in client;
      size_t size;
      int new_client = accept(socket->socket, (struct sockaddr *)&client, &size);
      if (new_client == -1) {
        PRINT_STANDARD_ERROR(IsConnected, accept);
        socket->is_connected = false;
      } else {
        /* success! */
        close(socket->socket);
        socket->socket = new_client;
        socket->is_connected = true;
        socket->is_listening = false;
      }

    } else if (socket->is_connected && (sock_evs.revents & POLLIN)) {
      /* are we dead yet? */

      char dummy;
      int num_bytes = recv(socket->socket, &dummy, 1, MSG_PEEK);
      if (num_bytes == 0 || (num_bytes == -1 && errno != EAGAIN)) {
        /* socket is now officially closed */
        close(socket->socket);
        socket->is_connected = false;
      }
      
    } else if (sock_evs.revents & (POLLERR | POLLHUP)) {
      /* some sort of failure */

      socket->is_connected = false;

    }

  }

  return socket->is_connected;
}

int GetPendingReadSize (NSOCKET socket) {
  if (!IsConnected(socket))
    return -1;

  /* We don't have ioctl's FIONREAD in POSIX or GNU, so instead we'll peek at the data.  Note that this function probably will perform a little bit differently than the win32 version, but won't affect program correctness. */
  char buffer[MAX_LINE];
  int num_bytes;
  num_bytes = recv(socket->socket, buffer, MAX_LINE, MSG_PEEK);
  if (num_bytes == -1) {
    if (errno == EWOULDBLOCK) {
      return 0;
    } else {
      PRINT_STANDARD_ERROR(GetPendingReadSize, recv);
      return -1;
    }
  }
  return num_bytes;
}

int SocketRead (NSOCKET socket, void* buffer, int size) {
  if (!IsConnected(socket))
    return -1;

  int num_read = 0;
  int total_read = 0;
  char* read_ptr = (char*)buffer;
  while (((num_read = read(socket->socket, read_ptr, size - total_read)) != 0) && (total_read < size)) {
    if (num_read == -1) {
      if (errno == EWOULDBLOCK) {
        return 0;
      } else {
        PRINT_STANDARD_ERROR(SocketRead, read);
        return -1;
      }
    }
    total_read += num_read;
    read_ptr += num_read;
  }
  return total_read;
}

void SocketWrite (NSOCKET socket, void* buffer, int size) {
  if (IsConnected(socket))
    if (write(socket->socket, buffer, size) == -1)
      PRINT_STANDARD_ERROR(SocketWrite, write);
}

#undef PRINT_STANDARD_ERROR
#undef PRINT_HOST_ERROR
