#ifndef __PONG_H__
#define __PONG_H__

#include <stdbool.h>
#include <stdlib.h>

struct parse {
    int argc;
    char *argv[256];
};

// int help(char *args, size_t args_len, char *send_buffer,
//          size_t send_buffer_capacity);
// int greeting(char *args, size_t args_len, char *send_buffer,
//          size_t send_buffer_capacity);
// int list(char *args, size_t args_len, char *send_buffer,
//          size_t send_buffer_capacity);
// int listls(char *args, size_t args_len, char *send_buffer,
//          size_t send_buffer_capacity);
// int bye(char *args, size_t args_len, char *send_buffer,
//          size_t send_buffer_capacity);
int pong(char *args, size_t args_len, char *send_buffer,
         size_t send_buffer_capacity);
// int responseToAdd(char *args, size_t args_len, char *send_buffer,
//          size_t send_buffer_capacity);
// int responseToDel(char *args, size_t args_len, char *send_buffer,
//          size_t send_buffer_capacity);
// int responseToStrat(char *args, size_t args_len, char *send_buffer,
//          size_t send_buffer_capacity);

int handle_client_request(char *receive_buffer, size_t receive_buffer_len,
                          char *send_buffer, size_t send_buffer_capacity);

#endif //__PONG_H__
