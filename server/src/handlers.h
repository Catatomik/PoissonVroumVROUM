#ifndef __HANDLERS_H__
#define __HANDLERS_H__

#include "parse_viewers_config.h"
#include <stdbool.h>
#include <stdlib.h>

struct parse {
    int argc;
    char *argv[256];
};

/**
 *
 * this function takes the client configuration, the client's command and the
 * server's return buffer. It returns the appropriate response to the received
 * client command
 *
 * @param viewer_config config of client (x,y, size ...)
 * @param receive_buffer command from client
 * @param send_buffer response for client
 *
 * returns 1 if con to close, 0 if ok and something else if an issue arised
 */
int handle_client_request(int fd, struct viewer_config_t **viewer_config,
                          char *receive_buffer, size_t receive_buffer_len,
                          char *send_buffer, size_t send_buffer_capacity);

#endif //__HANDLERS_H__
