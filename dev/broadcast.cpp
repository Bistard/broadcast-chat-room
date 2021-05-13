#include "broadcast.h"
#include "log.h"

using namespace std;

struct Client::clnt_data {
    int fd;
    string port;
    string name;
};

Client::Client() {this->data = new clnt_data;}

void 
Client::set_fd(int fd) {
    assert(fd >= 0);
    this->data->fd = fd;
}

void 
Client::set_port(string port) {this->data->port = port;}

void 
Client::set_name(string name) {this->data->name = name;}

int 
Client::get_fd() {return this->data->fd;}

string *
Client::get_port() {return &(this->data->port);}

string *
Client::get_name() {return &(this->data->name);}

void 
Client::client_destroy() {delete this->data;}

void 
epoll_add(int epfd, int fd, bool enable_et) {
    assert(epfd >= 0);
    assert(fd >= 0);

    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    if (enable_et) event.events |= EPOLLET;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event) == -1)
        Logger::log_error("[error]: adding epoll error\n", true);
}

void 
epoll_del(int epfd, int fd, bool enable_et) {
    assert(epfd >= 0);
    assert(fd >= 0);

    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    if (enable_et) event.events |= EPOLLET;
    if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &event) == -1)
        Logger::log_error("[error]: deleting epoll error\n", true);
}

int 
accept_request(struct sockaddr_in *clnt_addr, int server_sock) {
    assert(clnt_addr);
    assert(server_sock >= 0);

    socklen_t clnt_addr_size = sizeof(*clnt_addr);
    int client_sock = accept(server_sock, 
                             (struct sockaddr *) clnt_addr,
                             &clnt_addr_size);
    if (client_sock == -1) {
        Logger::log_error("accept client request error", true);
    } else {
        Logger::log_info("client connection successed", true);
    }
    return client_sock;
}

Client *
client_initialize(int fd, uint16_t port, string name) {
    assert(fd >= 0);

    Client *client = new Client;
    client->set_fd(fd);
    client->set_port(to_string(ntohs(port)));
    client->set_name(name);
    return client;
}

void 
broadcast_client(unordered_map<int, Client *> *clients_info, 
                 char *msg, int sender, std::string *sender_name) {
    assert(clients_info);
    assert(msg);
    assert(sender >= 0);
    assert(sender_name);

    // traverse all the clients except sender
    for (pair<int, Client *> client_info : *clients_info) {
        int client_fd = client_info.first;
        if (client_fd != sender) {
            // broadcast message
            int written_size = write(client_fd, msg, BUFFER_SIZE);
            if (written_size == -1) 
                Logger::log_error("server cannot write message to client", true);
            // broadcast name of the client
            written_size = write(client_fd, sender_name->data(), 
                                 (*sender_name).size() + 1);
            if (written_size == -1) 
                Logger::log_error("server cannot write name to client", true);
        }
    }
}

void 
destroy_all_clients(std::unordered_map<int, Client *> *clients_info) {
    assert(clients_info);

    for (pair<int, Client *> client : *clients_info) {
        (client.second)->client_destroy();
    }
}