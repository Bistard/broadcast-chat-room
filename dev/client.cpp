#include "client.h"
#include "log.h"

using namespace std;

static void read_input(int *user_quit, string *name, 
                       int client_sock, char *msg);

int client() {
    
    // message buffer
    char buffer[MAX_BUFFER];

    // set up socket for the client
    int client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock == -1) log_error("client_sock erorr");

    // connect the server (send request)
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(6666);

    if (connect(client_sock, 
                (const struct sockaddr *) &server_addr, 
                (socklen_t) sizeof(server_addr)) == -1)
        log_error("server connection error");
    
    // read client's name and send it to the server
    string name;
    cout << "Enter your name: ";
    getline(cin, name);
    int name_size = write(client_sock, (const char *) name.data(), 
                          name.size() + 1);
    if (name_size == -1) log_error("client writing error");

    // create an epoll instance
    int epoll_fd;
    struct epoll_event events[MAX_EPOLL_EVENTS];
    
    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) log_error("epoll_fd created error");
    epoll_add(epoll_fd, client_sock, true);

    // open a new thread for user input
    int user_quit = 0;
    log_info(WELCOME_MSG, true);
    thread user(read_input, &user_quit, &name, client_sock, buffer);

    // main loop
    while (true) {
        // user quit
        if (user_quit == -1) break;

        int success_event_count = epoll_wait(epoll_fd, events, 
                                             MAX_EPOLL_EVENTS, 
                                             100);
        if (success_event_count < 0) log_error("epoll_wait error");

        // receiving msg from the server
        for (int i = 0; i < success_event_count; i++) {
            
            int sock_fd = events[i].data.fd;
            if (sock_fd == client_sock) { // request for new connection
                int msg_size = read(client_sock, buffer, MAX_BUFFER);
                if (msg_size == -1) log_error("client reading error");
                cout << "[somebody]: " << buffer << endl;
            }
        }

    }
    
    close(client_sock);
    close(epoll_fd);
    user.join();

    return 0;

}

static void read_input(int *user_quit, string *name, 
                       int client_sock, char *buffer) {
    assert(user_quit);
    assert(name);
    assert(buffer);
    
    while (true) {
        
        // read input from the user
        cout << "[" << *name << "]: ";
        cin.getline(buffer, MAX_BUFFER);

        // disconnection
        if (!strcmp(buffer, CLOSE)) {
            cout << "[" << *name << "] disconnected" << endl;
            *user_quit = -1;
            return;
        }

        // send the msg to the server
        int written_size = write(client_sock, buffer, MAX_BUFFER);
        if (written_size == -1) log_error("client writing error");
        
    }
    return;
}