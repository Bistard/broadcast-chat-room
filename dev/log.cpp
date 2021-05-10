#include "log.h"

inline void log_info(std::string msg, bool newline) {
    printf(WHT "[info]" RESET ": %s", msg.data());
    if (newline) putchar('\n');
}

inline void log_error(std::string msg, bool newline) {
    printf(RED "[error]" RESET ": ");
    perror(msg.data());
    exit(EXIT_FAILURE);
    if (newline) putchar('\n');
}
