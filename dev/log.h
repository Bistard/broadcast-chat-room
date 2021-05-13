#ifndef LOG_H_
#define LOG_H_

#include "library.h"

#define CLOSE        "CLOSE"
#define WELCOME_MSG  "<-----------------------------------chatroom------------"\
                     "----------------------->"
#define ENTER_MSG    "<-----------------------------------enterbox------------"\
                     "----------------------->"

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\033[0m"

namespace Logger 
{
    enum Level {INFO, WARN, ERROR, FATAL};

    /* Singleton Logger Class */
    class Log 
    {
    public:
        static Log &get_instance();
    private:
        Log() {}
        Log(const Log &);
        Log &operator=(const Log &);
    public:
        void log(Level lv, std::string msg, bool newline);
    };

    void log_info (std::string msg, bool newline);
    void log_warn (std::string msg, bool newline);
    void log_error(std::string msg, bool newline);
    void log_fatal(std::string msg, bool newline);

}   

#endif // LOG_H_