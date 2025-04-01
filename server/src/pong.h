#ifndef __PONG_H__
#define __PONG_H__

#include <stdbool.h>
#include <stdlib.h>

struct parse {
    int argc;
    char *argv[256];
};

// int help(char *args, char *server_response);
// int greeting(char *args, char *server_response);
// int list(char *args, char *server_response);
// int listls(char *args, char *server_response);
// int bye(char *args, char *server_response);
int pong(char *args, size_t args_len, char *send_buffer,
         size_t send_buffer_capacity);
// int responseToAdd(char *args, char *server_response);
// int responseToDel(char *args, char *server_response);
// int responseToStrat(char *args, char *server_response);

int handle_client_request(char *receive_buffer, size_t receive_buffer_len,
                          char *send_buffer, size_t send_buffer_capacity);

#endif //__PONG_H__
