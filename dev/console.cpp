#include "console.h"
#include "log.h"

using namespace std;

Chatroom::Chatroom(int client_sock, int rows, int cols) {
    system("clear");
    // set default color to white
    printf(WHT);
    // set console size to rows and cols
    printf("\033[8;%d;%dt", rows, cols);
    this->chat_cursor = 2;
    this->client_sock = client_sock;
    this->chat_over = false;
}

inline void 
Chatroom::set_over() {this->chat_over = true;}

inline int 
Chatroom::get_socket() {return this->client_sock;}

inline int 
Chatroom::get_cursor() {return this->chat_cursor;}

inline bool 
Chatroom::is_over() {return this->chat_over;}

inline void 
Chatroom::refresh_enterbox() {
    set_cursor(ENTER_BOX_COOR - 1, 0);
    cout << ENTER_MSG << endl;
    
    char *buf = this->buffer;
    for (; *buf != '\0'; buf++) {
        putchar(' ');
    }
    putchar(' ');
}

inline void 
Chatroom::read_message() {
    set_cursor(ENTER_BOX_COOR, 0);
    cin.getline(this->buffer, MAX_BUFFER);
    this->refresh_enterbox();
}

inline void 
Chatroom::display_message(string *name) {
    set_cursor(this->chat_cursor, 0);
    printf(GRN "[%s]:" WHT "%s\n", (*name).data(), this->buffer);
    // cout << "[" << *name << "]: " << this->buffer << endl;
    // TODO: bug when multiple lines are entered last time
    (this->chat_cursor)++;
    // set back the cursor to the enter box
    refresh_enterbox();
}

inline void 
Chatroom::send_message() {
    int written_size = write(this->get_socket(), this->buffer, MAX_BUFFER);
    if (written_size == -1) log_error("client writing error", true);
}

void set_cursor(int row, int col) {
    assert(col >= 0);
    assert(col <= MAX_CONSOLE_COL);

    printf("\033[%d;%df", row, col);
}

