#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pong.h"

int greeting(char *args, size_t args_len, char *send_buffer,
             size_t send_buffer_capacity) {
    // extract id of the viewers
    char *id = malloc(6 * sizeof(char));
    if (id == NULL) {
        printf("Error allocation memory\n");
        return 1;
    }
    char *conf;
    char *start_char_id = strchr(args, 'N');

    // no id precised
    if (start_char_id == NULL) {
        memcpy(id, "N2", 2);
        id[2] = '\0';
        conf = "500x0+500+500";
    } else {
        int start_index_id = start_char_id - args;
        int end_index_id = start_index_id + 1;
        while (args[end_index_id] == '1' || args[end_index_id] == '2' ||
               args[end_index_id] == '3' || args[end_index_id] == '4' ||
               args[end_index_id] == '5' || args[end_index_id] == '6' ||
               args[end_index_id] == '7' || args[end_index_id] == '8' ||
               args[end_index_id] == '9' || args[end_index_id] == '0') {
            end_index_id += 1;
        }
        end_index_id -= start_index_id;
        memcpy(id, args + start_index_id, end_index_id);
        id[end_index_id] = '\0';

        // give them settings
        if (strncmp(id, "N1", 2) == 0)
            conf = "0x0+500+500";
        else if (strncmp(id, "N2", 2) == 0)
            conf = "500x0+500+500";
        else if (strncmp(id, "N3", 2) == 0)
            conf = "0x500+500+500";
        else if (strncmp(id, "N4", 2) == 0)
            conf = "500x500+500+500";
        else {
            snprintf(send_buffer, send_buffer_capacity, "no greeting\n");
            free(id);
            return 0;
        }
    }

    // greeting
    int printed_count = snprintf(send_buffer, send_buffer_capacity,
                                 "greting %s %s\n", id, conf);
    free(id);
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

int handle_client_request(char *receive_buffer, size_t receive_buffer_len,
                          char *send_buffer, size_t send_buffer_capacity) {
    // printf("%s\n", receive_buffer);

    /* if (strncmp(receive_buffer, "help", 4) == 0) { */
    /*   return help(parsed, server_response); */
    /* } */
    if (strncmp(receive_buffer, "hello", 4) == 0) {
        return greeting(receive_buffer + 5, receive_buffer_len, send_buffer,
                        send_buffer_capacity);
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
