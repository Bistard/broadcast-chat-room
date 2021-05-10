#include "client.h"
#include "log.h"

using namespace std;

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

    // main loop
    log_info(WELCOME_MSG, true);
    while (true) {
        
        // read input from the user
        cout << "[" << name << "]: "; 
        cin.getline(buffer, MAX_BUFFER);

        // disconnection
        if (!strcmp(buffer, CLOSE)) {
            cout << "[" << name << "] disconnected" << endl;
            break;
        }

        // send the msg to the server, then read from it again
        int written_size = write(client_sock, buffer, MAX_BUFFER);
        if (written_size == -1) log_error("client writing error");
        
    }
    
    close(client_sock);

    return 0;

}

