#ifndef TCP_H
#define TCP_H

#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET Socket;
#define SOCKET_NON_VALIDO INVALID_SOCKET
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
typedef int Socket;
#define SOCKET_NON_VALIDO (-1)
#endif

#include <stddef.h>

#define DIM_RIGA 128
#define SALUTO "connessione avvenuta"
#define QUIT "QUIT"

int avvia_socket(void);
void termina_socket(void);
void chiudi_socket(Socket socket);
int invia_riga(Socket socket, const char *riga);
/* 1: riga completa, 0: connessione chiusa, -1: errore. */
int ricevi_riga(Socket socket, char *riga, size_t capacita);
int leggi_porta(const char *riga, unsigned short *porta);
int leggi_numeri(const char *riga, long long numeri[3]);

#endif
