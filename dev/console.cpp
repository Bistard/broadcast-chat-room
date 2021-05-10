#include "console.h"
#include "log.h"

using namespace std;

Chatroom::Chatroom(int client_sock, int rows, int cols) {
    system("clear");
    // set console size to rows and cols
    printf("\033[8;%d;%dt", rows, cols);
    this->chat_cursor = 2;
    this->client_sock = client_sock;
}

void Chatroom::set_name(std::string name) {this->user_name = name;}

int Chatroom::get_socket() {return this->client_sock;}

int Chatroom::get_cursor() {return this->chat_cursor;}

const string *Chatroom::get_name() {return (string *) &(this->user_name);}

void Chatroom::clean_enter_box() {
    set_cursor(ENTER_BOX_COOR - 1, 0);
    cout << ENTER_MSG << endl;
    
    char *buf = this->buffer;
    for (; *buf != '\0'; buf++) {
        putchar(' ');
    }
    putchar(' ');
}

void Chatroom::read_message() {
    set_cursor(ENTER_BOX_COOR, 0);
    cin.getline(this->buffer, MAX_BUFFER);
    this->clean_enter_box();
}

void Chatroom::display_message(string name) {
    set_cursor(this->chat_cursor, 0);
    cout << "[" << name << "]: " << this->buffer << endl;
    // TODO: bug when multiple lines are entered last time
    (this->chat_cursor)++;
    // set back the cursor to the enter box:
    clean_enter_box();
}

void Chatroom::send_message() {
    int written_size = write(this->get_socket(), this->buffer, MAX_BUFFER);
    if (written_size == -1) log_error("client writing error");
}

void set_cursor(int row, int col) {
    assert(row >= 0);
    assert(row <= MAX_CONSOLE_ROW);
    assert(col >= 0);
    assert(col <= MAX_CONSOLE_COL);

    printf("\033[%d;%df", row, col);
}

