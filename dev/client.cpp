#include "client.h"
#include "log.h"
#include "console.h"

using namespace std;

static void read_input(Chatroom *);

int client() {
    
    // set up socket for the client
    int client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock == -1) log_error("client_sock erorr", true);

    // initialize chatroom settings
    Chatroom chat = Chatroom(client_sock, MAX_CONSOLE_ROW, MAX_CONSOLE_COL);
    
    // connect the server (send request)
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(6666);

    if (connect(client_sock, (const struct sockaddr *) &server_addr, 
                (socklen_t) sizeof(server_addr)) == -1) {
        log_error("server connection error", true);
    }

    // read client's name and send it to the server
    cout << "Enter your name: ";
    getline(cin, chat.user_name);
    int name_size = write(client_sock, (const char *) chat.user_name.data(), 
                          chat.user_name.size() + 1);
    if (name_size == -1) log_error("client writing error", true);

    // create an epoll instance
    int epoll_fd;
    struct epoll_event events[MAX_EPOLL_EVENTS];
    
    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) log_error("epoll_fd created error", true);
    epoll_add(epoll_fd, client_sock, true);

    // new thread for user input
    thread user(read_input, &chat);

    // main loop
    while (true) {
        // user quit
        if (chat.is_over()) break;

        // epoll event
        int success_event_count = 
            epoll_wait(epoll_fd, events, MAX_EPOLL_EVENTS, 100);
        if (success_event_count < 0) 
            log_error("epoll_wait error", true);

        // receive message from the server
        for (int i = 0; i < success_event_count; i++) {
            
            int sock_fd = events[i].data.fd;
            // request for new connection
            if (sock_fd == client_sock) {
                
                // reading message
                int msg_size = read(client_sock, chat.buffer, BUFFER_SIZE);
                if (msg_size == -1) 
                    log_error("client cannot read message", true);
                
                // reading name
                char name_buffer[50];
                int name_size = read(client_sock, name_buffer, 50);
                if (name_size == -1) 
                    log_error("client cannot read name", true);

                // display the msg on the chat
                string name(name_buffer);
                chat.display_message(&name);
            }
        }
    }
    
    // close the connection
    close(client_sock);
    close(epoll_fd);
    user.join();

    return 0;

}

static void read_input(Chatroom *chat) {
    assert(chat);

    set_cursor(0, 0);
    cout << WELCOME_MSG << endl;
    
    while (true) {

        // console style
        set_cursor(ENTER_BOX_COOR - 1, 0);
        cout << ENTER_MSG << endl;
        
        // read user's message and then display it
        chat->read_message();
        chat->display_message(&(chat->user_name));

        // disconnection
        if (!strcmp(chat->buffer, CLOSE)) {
            set_cursor(ENTER_BOX_COOR, 0);
            cout << "[" << chat->user_name << "] disconnected" << endl;
            chat->set_over();
            return;
        }

        // send the msg to the server
        chat->send_message();
    }

    // should never be reached
    log_error("expected exit", true);
}
