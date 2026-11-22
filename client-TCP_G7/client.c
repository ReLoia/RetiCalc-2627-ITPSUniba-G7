#include "../comune/tcp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int leggi_input(const char *prompt, char riga[DIM_RIGA])
{
    for (;;) {
        fputs(prompt, stdout);
        fflush(stdout);
        if (fgets(riga, DIM_RIGA, stdin) == NULL)
            return 0;

        size_t fine = strcspn(riga, "\n");
        if (riga[fine] == '\n' || fine < DIM_RIGA - 1) {
            if (fine > 0 && riga[fine - 1] == '\r')
                fine--;
            riga[fine] = '\0';
            return 1;
        }

        int carattere;
        do {
            carattere = getchar();
        } while (carattere != '\n' && carattere != EOF);
        puts("Riga troppo lunga, riprova.");
    }
}

static int leggi_indirizzo(struct sockaddr_in *indirizzo)
{
    char riga[DIM_RIGA];

    indirizzo->sin_family = AF_INET;
    for (;;) {
        if (!leggi_input("IP del server: ", riga))
            return 0;
        if (inet_pton(AF_INET, riga, &indirizzo->sin_addr) == 1)
            break;
        puts("Inserisci un indirizzo IPv4 valido.");
    }

    for (;;) {
        unsigned short porta;
        if (!leggi_input("Porta del server: ", riga))
            return 0;
        if (leggi_porta(riga, &porta)) {
            indirizzo->sin_port = htons(porta);
            return 1;
        }
        puts("Inserisci una porta da 1 a 65535.");
    }
}

static int scambia_messaggi(Socket server)
{
    char riga[DIM_RIGA];
    if (ricevi_riga(server, riga, sizeof riga) != 1 || strcmp(riga, SALUTO) != 0) {
        fprintf(stderr, "Saluto del server mancante o non valido.\n");
        return EXIT_FAILURE;
    }
    puts(riga);

    while (leggi_input("Inserisci n1 n2 n3: ", riga)) {
        long long numeri[3];
        if (!leggi_numeri(riga, numeri)) {
            puts("Inserisci tre interi a 32 bit separati da spazi.");
            continue;
        }

        snprintf(riga, sizeof riga, "%lld %lld %lld", numeri[0], numeri[1], numeri[2]);
        if (!invia_riga(server, riga) || ricevi_riga(server, riga, sizeof riga) != 1) {
            fprintf(stderr, "Scambio con il server interrotto.\n");
            return EXIT_FAILURE;
        }

        puts(riga);
        if (strcmp(riga, QUIT) == 0)
            break;
    }
    return EXIT_SUCCESS;
}

int main(void)
{
    if (!avvia_socket()) {
        fprintf(stderr, "Impossibile inizializzare le socket.\n");
        return EXIT_FAILURE;
    }

    struct sockaddr_in indirizzo = {0};
    if (!leggi_indirizzo(&indirizzo)) {
        termina_socket();
        return EXIT_SUCCESS;
    }

    Socket server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == SOCKET_NON_VALIDO) {
        fprintf(stderr, "Impossibile creare la socket del client.\n");
        termina_socket();
        return EXIT_FAILURE;
    }

    int esito = EXIT_FAILURE;
    if (connect(server, (struct sockaddr *)&indirizzo, sizeof indirizzo) == 0)
        esito = scambia_messaggi(server);
    else
        fprintf(stderr, "Impossibile connettersi al server.\n");

    chiudi_socket(server);
    termina_socket();
    return esito;
}
