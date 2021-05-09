#include "broadcast.h"

using namespace std;

struct Client::clnt_data {
    int fd;
    string port;
    string name;
};

Client::Client() {this->data = new clnt_data;};

void Client::set_fd(int fd) {
    assert(fd >= 0);

    this->data->fd = fd;
}

void Client::set_port(string port) {this->data->port = port;}

void Client::set_name(string name) {this->data->name = name;}

int Client::get_fd() {return this->data->fd;}

string Client::get_port() {return this->data->port;}

string Client::get_name() {return this->data->name;}

void Client::client_destroy() {delete this->data;}

void epoll_add(int epfd, int fd, bool enable_et) {
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    if (enable_et) event.events |= EPOLLET;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event) == -1)
        log_error("[error]: adding epoll error\n");
}

void epoll_del(int epfd, int fd, bool enable_et) {
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    if (enable_et) event.events |= EPOLLET;
    if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &event) == -1)
        log_error("[error]: deleting epoll error\n");
}

int accept_request(struct sockaddr_in *clnt_addr, int server_sock) {
    assert(clnt_addr);
    assert(server_sock >= 0);

    socklen_t clnt_addr_size = sizeof(*clnt_addr);
    int client_sock = accept(server_sock, 
                             (struct sockaddr *) clnt_addr,
                             &clnt_addr_size);
    if (client_sock == -1) {
        log_error("accept client request error");
    } else {
        log_info("[client connection successed]", true);
    }
    return client_sock;
}

Client *client_initialize(int fd, uint16_t port, string name) {
    assert(fd >= 0);

    Client *client = new Client;
    client->set_fd(fd);
    client->set_port(to_string(ntohs(port)));
    client->set_name(name);
    return client;
}

void broadcast_client(unordered_map<int, Client *> *clients_info, 
                      char *msg, int sender) {
    assert(clients_info);
    assert(msg);
    assert(sender >= 0);

    // traverse all the clients except sender
    for (pair<int, Client *> client_info : *clients_info) {
        int client_fd = client_info.first;
        Client *client = client_info.second;
        if (client_fd != sender) {
            int written_size = write(client_fd, msg, MAX_BUFFER);
            if (written_size == -1) log_error("server writing to client error");
        }
    }
}

void destroy_all_clients(std::unordered_map<int, Client *> *clients_info) {
    assert(clients_info);

    for (pair<int, Client *> client : *clients_info) {
        (client.second)->client_destroy();
    }
}