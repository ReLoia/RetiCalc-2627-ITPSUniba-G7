#include "../comune/tcp.h"

#include <stdio.h>
#include <stdlib.h>

static void gestisci_client(Socket client)
{
    char riga[DIM_RIGA];

    if (!invia_riga(client, SALUTO))
        return;

    while (ricevi_riga(client, riga, sizeof riga) == 1) {
        long long numeri[3];
        if (!leggi_numeri(riga, numeri)) {
            fprintf(stderr, "Il client non ha inviato tre interi validi.\n");
            return;
        }

        long long risultato = numeri[0] + numeri[1] - numeri[2];
        printf("la somma di %lld+%lld-%lld=%lld\n",
               numeri[0], numeri[1], numeri[2], risultato);
        fflush(stdout);

        if (risultato == 100) {
            invia_riga(client, QUIT);
            return;
        }

        snprintf(riga, sizeof riga, "%lld", risultato);
        if (!invia_riga(client, riga))
            return;
    }
}

int main(int argc, char *argv[])
{
    unsigned short porta;
    if (argc != 2 || !leggi_porta(argv[1], &porta)) {
        fprintf(stderr, "Uso: %s <porta da 1 a 65535>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (!avvia_socket()) {
        fprintf(stderr, "Impossibile inizializzare le socket.\n");
        return EXIT_FAILURE;
    }

    Socket listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == SOCKET_NON_VALIDO) {
        fprintf(stderr, "Impossibile creare la socket del server.\n");
        termina_socket();
        return EXIT_FAILURE;
    }

#ifndef _WIN32
    int riuso = 1;
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &riuso, sizeof riuso);
#endif

    struct sockaddr_in indirizzo = {0};
    indirizzo.sin_family = AF_INET;
    indirizzo.sin_port = htons(porta);
    indirizzo.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listener, (struct sockaddr *)&indirizzo, sizeof indirizzo) != 0 ||
        listen(listener, SOMAXCONN) != 0) {
        fprintf(stderr, "Impossibile mettersi in ascolto sulla porta %u.\n", (unsigned int)porta);
        chiudi_socket(listener);
        termina_socket();
        return EXIT_FAILURE;
    }

    printf("Server in ascolto sulla porta %u\n", (unsigned int)porta);
    fflush(stdout);

    for (;;) {
        struct sockaddr_in indirizzo_client;
#ifdef _WIN32
        int dimensione = sizeof indirizzo_client;
#else
        socklen_t dimensione = sizeof indirizzo_client;
#endif
        Socket client = accept(listener, (struct sockaddr *)&indirizzo_client, &dimensione);
        if (client == SOCKET_NON_VALIDO) {
            fprintf(stderr, "Errore durante l'accettazione del client.\n");
            continue;
        }

        char ip[INET_ADDRSTRLEN];
        const char *ip_client = inet_ntop(AF_INET, &indirizzo_client.sin_addr, ip, sizeof ip);
        printf("connesso con il client il cui nome IP: %s\n", ip_client ? ip_client : "sconosciuto");
        fflush(stdout);

        gestisci_client(client);
        chiudi_socket(client);
    }
}
