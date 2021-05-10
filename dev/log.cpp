#include "log.h"

void log_info(std::string msg, bool newline) {
    printf(WHT "[info]" RESET ": %s", msg.data());
    if (newline) putchar('\n');
}