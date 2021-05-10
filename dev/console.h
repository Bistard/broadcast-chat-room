#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "library.h"
#include "broadcast.h"


#define MAX_CONSOLE_ROW  30
#define MAX_CONSOLE_COL  80

#define ENTER_BOX_HEIGHT 3
#define ENTER_BOX_COOR   MAX_CONSOLE_ROW - ENTER_BOX_HEIGHT + 1

class Chatroom {
    public:
        char buffer[MAX_BUFFER];
    private:
        int client_sock;
        std::string user_name;
        int chat_cursor;
    public:
        Chatroom(int client_sock, int rows, int cols);
        void set_name(std::string name);
        
        int get_socket();
        int get_cursor();
        const std::string *get_name();
        
        void clean_enter_box();

        void read_message();
        void display_message(std::string name);
        void send_message();
};

void set_cursor(int row, int col);


#endif