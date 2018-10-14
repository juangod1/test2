#ifndef PROXYCOMMUNICATION_H
#define PROXYCOMMUNICATION_H

#include <unistd.h>

#define NUM_PIPES          2

#define PROXY_READ_PIPE   0
#define PROXY_WRITE_PIPE  1

#define READ_FD  0
#define WRITE_FD 1

#define PROXY_READ_FD  ( pipes[PROXY_READ_PIPE][READ_FD]   )
#define PARSER_WRITE_FD ( pipes[PROXY_READ_PIPE][WRITE_FD] )

#define PARSER_READ_FD  ( pipes[PROXY_WRITE_PIPE][READ_FD]   )
#define PROXY_WRITE_FD ( pipes[PROXY_WRITE_PIPE][WRITE_FD] )

int start_parser(char * cmd,char * msg, size_t size);
size_t read_parser(char * buffer, size_t size);

#endif