#ifndef TCP_H
#define TCP_H

#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
/* Su POSIX le socket sono descrittori interi e -1 segnala un errore. */
typedef int SOCKET;
#define INVALID_SOCKET (-1)
#endif

#include <stddef.h>

int avvia_socket(void);
void termina_socket(void);
void chiudi_socket(SOCKET socket);
int invia_riga(SOCKET socket, const char *riga);
/* 1: riga completa, 0: connessione chiusa, -1: errore. */
int ricevi_riga(SOCKET socket, char *riga, size_t capacita);
int leggi_porta(const char *riga, unsigned short *porta);
int leggi_numeri(const char *riga, long long numeri[3]);

#endif
