#include "handlers.h"
#include "config.h"
#include "parse_viewers_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// > hello in as N3
// < greeting N3 0x500+500+500
int greeting(struct viewer_config_t **assigned_config, char *args,
             size_t args_len, char *send_buffer, size_t send_buffer_capacity) {
    int requested_id;
    int matched_count = sscanf(args, "in as N%d\n", &requested_id);
    if (matched_count < 1 && (args[0] == '\0' || args[0] == '\n')) {
        // no id precised, assign first free viewer id
        for (int i = 0; i < viewers_config.viewers_count; i++) {
            if (!viewers_config.viewers_configs[i].is_in_use) {
                *assigned_config = &viewers_config.viewers_configs[i];
                printf("no id provided, assigning id N%d\n",
                       (*assigned_config)->id);
                break;
            }
        }
    } else if (matched_count == 1) {
        // if an id is specified, search it in the viewers config array
        for (int i = 0; i < viewers_config.viewers_count; i++) {
            if (viewers_config.viewers_configs[i].id == requested_id) {
                *assigned_config = &viewers_config.viewers_configs[i];
                break;
            }
        }
    } else {
        fprintf(stderr, "[WARN] client tried conecting with 'hello %s'\n",
                args);
    }
    if (*assigned_config == NULL) {
        snprintf(send_buffer, send_buffer_capacity, "no greeting\n");
        return 0;
    }

    (*assigned_config)->is_in_use = true;

    int printed_count = snprintf(
        send_buffer, send_buffer_capacity, "greting N%d %dx%d+%d+%d\n",
        (*assigned_config)->id, (*assigned_config)->x, (*assigned_config)->y,
        (*assigned_config)->width, (*assigned_config)->height);
    if (printed_count > send_buffer_capacity) {
        fprintf(
            stderr,
            "[ERR] response would have overran send_buffer (len=%zu) because "
            "of arg len=%d\n",
            send_buffer_capacity, printed_count);
        return -1;
    }
    return 0;
}

// void list(char *args, size_t args_len, char *send_buffer,
//          size_t send_buffer_capacity) {
//     strcpy(server_response, "not yet list");
// }

// void listls(char *args, size_t args_len, char *send_buffer,size_t
// send_buffer_capacity) {
//     strcpy(server_response, "not yet listls");
// }

// void bye(char *args, size_t args_len, char *send_buffer,size_t
// send_buffer_capacity) {
//     strcpy(server_response, "not yet bye");
// }

int pong(char *args, size_t args_len, char *send_buffer,
         size_t send_buffer_capacity) {
    int printed_count =
        snprintf(send_buffer, send_buffer_capacity, "pong %s\n", args);
    if (printed_count > send_buffer_capacity) {
        fprintf(
            stderr,
            "[ERR] response would have overran send_buffer (len=%zu) because "
            "of arg len=%d\n",
            send_buffer_capacity, printed_count);
        return -1;
    }
    return 0;
}

// void responseToAdd(char *args, size_t args_len, char *send_buffer,size_t
// send_buffer_capacity) {
//     strcpy(server_response, "not yet responseToAdd");
// }
// void responseToDel(char *args, size_t args_len, char *send_buffer,size_t
// send_buffer_capacity) {
//     strcpy(server_response, "not yet responseToDel");
// }
// void responseToStrat(char *args, size_t args_len, char *send_buffer,size_t
// send_buffer_capacity) {
//     strcpy(server_response, "not yet responseToStart");
// }

int handle_client_request(struct viewer_config_t **viewer_config,
                          char *receive_buffer, size_t receive_buffer_len,
                          char *send_buffer, size_t send_buffer_capacity) {
    // printf("%s\n", receive_buffer);

    /* if (strncmp(receive_buffer, "help", 4) == 0) { */
    /*   return help(parsed, server_response); */
    /* } */
    if (strncmp(receive_buffer, "hello", 5) == 0) {
        return greeting(viewer_config, receive_buffer + 6, receive_buffer_len,
                        send_buffer, send_buffer_capacity);
    }
    /* if (strncmp(receive_buffer, "getFish", 7) == 0) { */
    /*   return list(receive_buffer + 8, receive_buffer_len, send_buffer, */
    /* 		send_buffer_capacity); */
    /* } */
    /* if (strncmp(receive_buffer, "ls", 2) == 0) { */
    /*   return listls(receive_buffer + 3, receive_buffer_len, send_buffer, */
    /* 		send_buffer_capacity); */
    /* } */
    /* if (strncmp(receive_buffer, "log", 3) == 0) { */
    /*   return bye(receive_buffer + 4, receive_buffer_len, send_buffer, */
    /* 		send_buffer_capacity); */
    /* } */
    if (strncmp(receive_buffer, "ping", 4) == 0) {
        return pong(receive_buffer + 5, receive_buffer_len, send_buffer,
                    send_buffer_capacity);
    }
    /* if (strncmp(receive_buffer, "addFish", 7) == 0) { */
    /*   return responseToAdd(receive_buffer + 8, receive_buffer_len,
     * send_buffer, */
    /* 		send_buffer_capacity); */
    /* } */
    /* if (strncmp(receive_buffer, "delFish", 7) == 0) { */
    /*   return responseToDel(receive_buffer + 8, receive_buffer_len,
     * send_buffer, */
    /* 		send_buffer_capacity); */
    /* } */
    /* if (strncmp(receive_buffer, "startFish", 9) == 0) { */
    /*   return responseToStrat(receive_buffer + 10, receive_buffer_len,
     * send_buffer, */
    /* 		send_buffer_capacity); */
    /* } */

    fprintf(stderr, "[ERR] unknown protocol command\n");
    return -1;
}
