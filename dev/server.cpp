#include "server.h"
#include "console.h"
#include "log.h"

using namespace std;


int server() {
    // console settings
    system("clear");
    printf("\033[8;%d;%dt", MAX_CONSOLE_ROW, MAX_CONSOLE_COL);

    // set up socket for the server
    int server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_sock == -1) 
        Logger::log_error("server_sock error", true);

    // bind the socket with the IP & port
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(6666);

    bind(server_sock, 
         (const struct sockaddr *) &server_addr, 
         (socklen_t) sizeof(server_addr));

    // listen to the client
    if (listen(server_sock, 20) == -1) {
        Logger::log_error("listen to the client error", true);
    } else {
        Logger::log_info("[start listening...]", true);
    }

    // create an epoll instance
    int epoll_fd;
    struct epoll_event events[MAX_EPOLL_EVENTS];
    char buffer[BUFFER_SIZE];
    unordered_map<int, Client *> clients_info;
    
    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) 
        Logger::log_error("epoll_fd created error", true);
    
    epoll_add(epoll_fd, server_sock, true);

    // main loop
    while (true) {

        // epoll events
        int success_event_count = 
            epoll_wait(epoll_fd, events, MAX_EPOLL_EVENTS, INFINITE_WAIT_TIME);
        if (success_event_count < 0) 
            Logger::log_error("epoll_wait error", true);
        
        // receiving some clients
        for (int i = 0; i < success_event_count; i++) {
            
            int sock_fd = events[i].data.fd;
            if (sock_fd == server_sock) { // request for new connection
                
                // accept the client request
                struct sockaddr_in clnt_addr;
                int client_sock = accept_request(&clnt_addr, server_sock);
                epoll_add(epoll_fd, client_sock, true);
                
                // read the name of the client
                int name_size = read(client_sock, buffer, BUFFER_SIZE);
                if (name_size == -1) 
                    Logger::log_error("server reading error", true);
                string name(buffer);
                
                // then create an Client instance
                Client *client = client_initialize(client_sock, 
                                                   clnt_addr.sin_port, 
                                                   name);
                clients_info[client_sock] = client;
                
                // output the client info
                Logger::log_info("", false);
                cout << "welcome new guest " BLU "[" << *(client->get_name()) 
                     << "]" RESET << endl;
                Logger::log_info("", false);
                cout << "<IP [" << inet_ntoa(clnt_addr.sin_addr)
                     << "] port [" << *(client->get_port())
                     << "] clientfd = " << client->get_fd() << ">" << endl;

            } else { // data from existing connection, process it
                
                Client *client = clients_info[sock_fd];

                // read the msg from the client
                int msg_size = read(sock_fd, buffer, BUFFER_SIZE);
                if (msg_size == -1) 
                    Logger::log_error("server reading message error", true);
                
                // client disconnected
                if (!msg_size) { 
                    epoll_del(epoll_fd, sock_fd, true);
                    close(sock_fd);

                    Logger::log_info(BLU "[" + *(client->get_name()) 
                                             + "]" RESET " disconneted.",
                                     true);
                    
                    clients_info[sock_fd]->client_destroy();
                    clients_info.erase(sock_fd);
                    
                } else { // successfully read the data, and broadcast to others
                    // TODO: pass a Server Object
                    broadcast_client(&clients_info, buffer, 
                                     client->get_fd(), client->get_name());                    
                    cout << BLU "[" << *(client->get_name()) 
                         << "]" RESET ": " << buffer << endl;
                }
            }
        }
    }

    // close the connection
    destroy_all_clients(&clients_info);
    close(server_sock);
    close(epoll_fd);
    
    return 0;
}
