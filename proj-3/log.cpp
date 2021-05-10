#include "log.h"

void log_info(std::string msg, bool newline) {
    std::cout << msg;
    if (newline) {
        std::cout << std::endl;
    }
}