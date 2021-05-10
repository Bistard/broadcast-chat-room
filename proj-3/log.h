#ifndef LOG_H_
#define LOG_H_

#include "library.h"

#define CLOSE        "CLOSE"
#define NO_MSG       "NO_MSG"
#define WELCOME_MSG  "<----------------chat room---------------->"

#define log_error(msg) {perror(msg); exit(EXIT_FAILURE);}

void log_info(std::string msg, bool newline);

#endif // LOG_H_