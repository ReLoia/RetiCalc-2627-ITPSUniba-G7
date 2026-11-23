# Esonero Reti di Calcolatori 2026–27 · Gruppo G7

Prima assegnazione di laboratorio sulle socket TCP. Sistema operativo usato per la prova: Arch Linux.

Il sorgente del client è in `client-TCP_G7`, quello del server in `serverTCP_G7`. Entrambi usano `comune/tcp.h` e `comune/tcp.c`.

## Compilazione

Dalla cartella principale, su Linux o macOS:

```sh
cc -std=c11 -Wall -Wextra -Wpedantic serverTCP_G7/server.c comune/tcp.c -o server
cc -std=c11 -Wall -Wextra -Wpedantic client-TCP_G7/client.c comune/tcp.c -o client
```

Su Windows, con GCC (MinGW-w64):

```powershell
gcc -std=c11 -Wall -Wextra -Wpedantic serverTCP_G7/server.c comune/tcp.c -o server.exe -lws2_32
gcc -std=c11 -Wall -Wextra -Wpedantic client-TCP_G7/client.c comune/tcp.c -o client.exe -lws2_32
```

## Esecuzione

1. Avvia il server con `./server 5000` (su Windows `./server.exe 5000`). Puoi scegliere un'altra porta libera.
2. In un altro terminale, avvia `./client` (su Windows `./client.exe`) e inserisci l'indirizzo IPv4 del server e la porta. Usa `127.0.0.1` se entrambi sono sulla stessa macchina.
3. Dopo il messaggio `connessione avvenuta`, inserisci tre interi a 32 bit separati da spazi sulla stessa riga. Per esempio, `20 30 10` dà `40`.
4. Con `60 50 10` il risultato è 100. Il server risponde `QUIT`, il client termina e il server può accettare la connessione successiva. Per fermare il server usa `Ctrl+C`.

Ogni messaggio TCP è una riga di testo terminata da `\n`. Il risultato è calcolato come `n1 + n2 - n3`.
