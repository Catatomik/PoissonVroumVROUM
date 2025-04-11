#ifndef __HANDLERS_H__
#define __HANDLERS_H__

#include "parse_viewers_config.h"
#include <stdbool.h>
#include <stdlib.h>

struct parse {
    int argc;
    char *argv[256];
};

int handle_client_request(struct viewer_config_t **viewer_config,
                          char *receive_buffer, size_t receive_buffer_len,
                          char *send_buffer, size_t send_buffer_capacity);

#endif //__HANDLERS_H__
