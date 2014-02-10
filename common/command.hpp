#ifndef _COMMAND_HPP
#define _COMMAND_HPP

struct __attribute__((__packed__)) Command {
    char format[4];
    char control[16];
    int size;
    char *content;
};

#endif
