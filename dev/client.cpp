#include "client.h"
#include "log.h"
#include "console.h"

using namespace std;

static void read_input(Chatroom *chat, int *user_quit);

int client() {
    
    // set up socket for the client
    int client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock == -1) log_error("client_sock erorr");

    // initialize chatroom settings
    Chatroom chat = Chatroom(client_sock, MAX_CONSOLE_ROW, MAX_CONSOLE_COL);
    
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
    chat.set_name(name);

    // create an epoll instance
    int epoll_fd;
    struct epoll_event events[MAX_EPOLL_EVENTS];
    
    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) log_error("epoll_fd created error");
    epoll_add(epoll_fd, client_sock, true);

    // open a new thread for user input
    int user_quit = 0;
    thread user(read_input, &chat, &user_quit);

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
            // request for new connection
            if (sock_fd == client_sock) {
                // reading message
                int msg_size = read(client_sock, chat.buffer, MAX_BUFFER);
                if (msg_size == -1) log_error("client cannot read message");
                // reading name
                char name_buffer[50];
                int name_size = read(client_sock, name_buffer, 50);
                if (name_size == -1) log_error("client cannot read name");

                // display the msg on the chat
                string name(name_buffer);
                chat.display_message(name);
            }
        }

    }
    
    close(client_sock);
    close(epoll_fd);
    user.join();

    return 0;

}

static void read_input(Chatroom *chat, int *user_quit) {
    assert(user_quit);

    set_cursor(0, 0);
    cout << WELCOME_MSG << endl;
    
    while (true) {

        // console style
        set_cursor(ENTER_BOX_COOR - 1, 0);
        cout << ENTER_MSG << endl;
        // read user's message and then display it
        chat->read_message();
        chat->display_message(*(chat->get_name()));

        // disconnection
        if (!strcmp(chat->buffer, CLOSE)) {
            set_cursor(ENTER_BOX_COOR, 0);
            cout << "[" << *(chat->get_name()) << "] disconnected" << endl;
            *user_quit = -1;
            return;
        }

        // send the msg to the server
        chat->send_message();
    }
    // should never be reached
    log_error("expected exit");
}
