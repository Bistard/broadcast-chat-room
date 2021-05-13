#include "log.h"

namespace Logger {

Log & 
Log::get_instance() {
    static Log instance; /* in C++11, it is thread safe
                            in C++03, it is NOT thread safe
                          */
    return instance;
}

void
Log::log(Level lv, std::string msg, bool newline) {
    switch (lv)
    {
    case INFO:
        std::cout << WHT "[info]: " RESET; 
        break;
    case WARN:
        std::cout << YEL "[warn]: " RESET;
        break;
    case ERROR:
        std::cout << RED "[error]: " RESET;
        break;
    case FATAL:
        std::cout << MAG "[fatal]: " RESET;
        break;
    }
    std::cout << msg;
    if (newline) 
        std::cout << std::endl;
    if (lv == ERROR || lv == FATAL) {
        exit(EXIT_FAILURE);
    }
}

void
log_info(std::string msg, bool newline) { 
   Log::get_instance().log(INFO, msg, newline);
}

void
log_warn(std::string msg, bool newline) { 
    Log::get_instance().log(WARN, msg, newline); 
}

void
log_error(std::string msg, bool newline) { 
    Log::get_instance().log(ERROR, msg, newline); 
}

void
log_fatal(std::string msg, bool newline) { 
    Log::get_instance().log(FATAL, msg, newline); 
}

}
