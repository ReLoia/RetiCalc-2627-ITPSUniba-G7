#include "tcp.h"

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
#include <signal.h>
#include <unistd.h>
#endif

int avvia_socket(void)
{
#ifdef _WIN32
    WSADATA dati;
    return WSAStartup(MAKEWORD(2, 2), &dati) == 0;
#else
    return signal(SIGPIPE, SIG_IGN) != SIG_ERR;
#endif
}

void termina_socket(void)
{
#ifdef _WIN32
    WSACleanup();
#endif
}

void chiudi_socket(SOCKET socket)
{
#ifdef _WIN32
    closesocket(socket);
#else
    close(socket);
#endif
}

static int chiamata_interrotta(void)
{
#ifdef _WIN32
    return WSAGetLastError() == WSAEINTR;
#else
    return errno == EINTR;
#endif
}

int invia_riga(SOCKET socket, const char *riga)
{
    char messaggio[128];
    int lunghezza = snprintf(messaggio, sizeof messaggio, "%s\n", riga);

    if (lunghezza < 0 || lunghezza >= (int)sizeof messaggio)
        return 0;

    int inviati = 0;
    while (inviati < lunghezza) {
        int n = (int)send(socket, messaggio + inviati, lunghezza - inviati, 0);
        if (n < 0 && chiamata_interrotta())
            continue;
        if (n <= 0)
            return 0;
        inviati += n;
    }
    return 1;
}

int ricevi_riga(SOCKET socket, char *riga, size_t capacita)
{
    size_t usati = 0;

    while (usati < capacita) {
        char carattere;
        int n = (int)recv(socket, &carattere, 1, 0);
        if (n < 0 && chiamata_interrotta())
            continue;
        if (n == 0 && usati == 0)
            return 0;
        if (n <= 0 || carattere == '\0')
            return -1;
        if (carattere == '\n') {
            riga[usati] = '\0';
            return 1;
        }
        riga[usati++] = carattere;
    }
    return -1;
}

static const char *salta_spazi(const char *riga)
{
    while (isspace((unsigned char)*riga))
        riga++;
    return riga;
}

int leggi_porta(const char *riga, unsigned short *porta)
{
    char *fine;
    errno = 0;
    long valore = strtol(riga, &fine, 10);

    if (fine == riga || *salta_spazi(fine) != '\0' || errno == ERANGE ||
        valore < 1 || valore > 65535)
        return 0;

    *porta = (unsigned short)valore;
    return 1;
}

int leggi_numeri(const char *riga, long long numeri[3])
{
    for (int i = 0; i < 3; i++) {
        char *fine;
        errno = 0;
        long long numero = strtoll(riga, &fine, 10);

        if (fine == riga || errno == ERANGE || numero < INT32_MIN || numero > INT32_MAX)
            return 0;
        if (i < 2 && !isspace((unsigned char)*fine))
            return 0;

        numeri[i] = numero;
        riga = fine;
    }
    return *salta_spazi(riga) == '\0';
}
