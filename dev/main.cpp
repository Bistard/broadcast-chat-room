#include <iostream>
#include <cstring>
#include "server.h"
#include "client.h"
#include "log.h"

#define SERVER "server"
#define CLIENT "client"

int main(int argc, char *argv[]) {
    if (argc == 1) { // running server as default
        log_info("[running SERVER...]", true);
        server();
    } else if (argc == 2 && !strcmp(argv[1], SERVER)) { // running server
        log_info("[running SERVER...]", true);
        server();
    } else if (argc == 2 && !strcmp(argv[1], CLIENT)) { // running client
        log_info("[running CLIENT...]", true);
        client();
    } else {
        log_error("wrong arguments", true);
    }
}