#ifndef __PONG_H__
#define __PONG_H__

#include "main.h"
#include "parse_viewers_config.h"
#include <stdbool.h>
#include <stdlib.h>

struct parse {
    int argc;
    char *argv[256];
};

int handle_client_request(char *receive_buffer, size_t receive_buffer_len,
                          char *send_buffer, size_t send_buffer_capacity);

#endif //__PONG_H__
