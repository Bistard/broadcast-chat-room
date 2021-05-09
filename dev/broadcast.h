#ifndef BROADCAST_H_
#define BROADCAST_H_

#include "library.h"
#include "log.h"

#define SERVER_IP             "127.0.0.1"
#define MAX_EPOLL_EVENTS      30
#define MAX_BUFFER            500
#define INFINITE_WAIT_TIME    -1

class Client {
    private:
        struct clnt_data;
        struct clnt_data *data;
    
    public:
        Client();
        void set_fd(int fd);
        void set_port(std::string port);
        void set_name(std::string name);
        int get_fd();
        std::string get_port();
        std::string get_name();
        void client_destroy();
};

void epoll_add(int epfd, int fd, bool enable_et);
void epoll_del(int epfd, int fd, bool enable_et);

int accept_request(struct sockaddr_in *clnt_addr, int server_sock);

Client *client_initialize(int fd, uint16_t port, std::string name);

void broadcast_client(std::unordered_map<int, Client *> *clients_info,
                      char *msg, int sender);

void destroy_all_clients(std::unordered_map<int, Client *> *clients_info);

#endif // BROADCAST_H_