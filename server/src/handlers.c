#include "handlers.h"
#include "config.h"
#include "fishes.h"
#include "parse_viewers_config.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SEND_BUFFER_CAPACITY 512

struct config_and_send_t {
    struct viewer_config_t **assigned_config;
    char *send_buffer;
};

/**
 * write the response to given hello command in send_buffer
 *
 * @param args string with client command (without hello)
 * @param send_buffer response for client
 *
 */
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
        send_buffer, send_buffer_capacity, "greeting N%d %dx%d+%d+%d\n",
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

int list(struct viewer_config_t **assigned_config, char *send_buffer,
         size_t send_buffer_capacity) {
    struct fish_t *fish = next_fish(NULL);

    int x_min = 0;
    int y_min = 0;
    int x_max = 0;
    int y_max = 0;

    if (*assigned_config != NULL) {
        x_min = (*assigned_config)->x;
        y_min = (*assigned_config)->y;
        x_max = (*assigned_config)->x + (*assigned_config)->width;
        y_max = (*assigned_config)->y + (*assigned_config)->height;
    }

    while (fish != NULL) {
        // should check if fish is in the view
        if (fish->current_x >= x_min && fish->current_x <= x_max &&
            fish->current_y >= y_min && fish->current_y <= y_max) {
            snprintf(send_buffer + strlen(send_buffer),
                     send_buffer_capacity - strlen(send_buffer),
                     "%s at [%dx%d,%dx%d,%d]", fish->name, (int)fish->target_x,
                     (int)fish->target_y, (int)fish->width, (int)fish->height,
                     (int)fish->time_left);
        }

        fish = next_fish(fish);
    }
    snprintf(send_buffer + strlen(send_buffer),
             send_buffer_capacity - strlen(send_buffer), "\n");
    return 0;
}

// void listls(char *args, size_t args_len, char *send_buffer,size_t
// send_buffer_capacity) {
//     strcpy(server_response, "not yet listls");
// }

void *actualList(void *args) {
    struct config_and_send_t *arguments = (struct config_and_send_t *)args;
    char *send_buffer = arguments->send_buffer;
    struct viewer_config_t **assigned_config = arguments->assigned_config;
    while (true) {
        list(assigned_config, send_buffer, SEND_BUFFER_CAPACITY);
    }
    return NULL;
}

int bye(char *args, size_t args_len, char *send_buffer,
        size_t send_buffer_capacity) {
    // should stop the thread and while loop in connexion
    snprintf(send_buffer, send_buffer_capacity, "bye\n");
    return 0;
}

/**
 * write the response to given ping command in send_buffer
 *
 * @param args string with client command (without ping)
 * @param send_buffer response for client
 *
 */
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

/**
 * add the fish by calling add_fish
 */
int responseToAdd(char *args, size_t args_len, char *send_buffer,
                  size_t send_buffer_capacity) {
    if (args_len <= 0) {
        snprintf(send_buffer, send_buffer_capacity,
                 "error no fish details described\n");
        return 1;
    }
    struct fish_t newFish;
    if (sscanf(args, "%s at %fx%f,%fx%f,RandomWayPoint", newFish.name,
               &newFish.current_x, &newFish.current_y, &newFish.width,
               &newFish.height) < 5) {
        printf("command unrecognized\n");
        return 1;
    }

    newFish.target_x = newFish.current_x;
    newFish.target_y = newFish.current_y;
    newFish.time_left = 0;

    struct fish_t *existedFish = next_fish(NULL);
    while (existedFish != NULL) {
        if (strcmp(existedFish->name, newFish.name) == 0) {
            snprintf(send_buffer, send_buffer_capacity, "NOK\n");
            return 1;
        }
        existedFish = next_fish(existedFish);
    }

    add_fish(newFish);
    snprintf(send_buffer, send_buffer_capacity, "OK\n");
    return 0;
}

/**
 * delet the fish by calling remove_fish
 */
int responseToDel(char *args, size_t args_len, char *send_buffer,
                  size_t send_buffer_capacity) {
    if (args_len <= 0) {
        snprintf(send_buffer, send_buffer_capacity,
                 "error no fish name gave\n");
        return 1;
    }
    char name[FISH_NAME_MAX_LENGTH];
    if (sscanf(args, "%s", name) < 1) {
        printf("command unrecognized\n");
        return 1;
    }

    struct fish_t *existedFish = next_fish(NULL);
    while (existedFish != NULL) {
        if (strcmp(existedFish->name, name) == 0) {
            remove_fish(existedFish);
            snprintf(send_buffer, send_buffer_capacity, "OK\n");
            return 0;
        }
        existedFish = next_fish(existedFish);
    }
    snprintf(send_buffer, send_buffer_capacity, "NOK\n");
    return 0;
}

/**
 * start the fish by given an target and a time left
 */
int responseToStrat(char *args, size_t args_len, char *send_buffer,
                    size_t send_buffer_capacity) {
    if (args_len <= 0) {
        snprintf(send_buffer, send_buffer_capacity,
                 "error no fish name gave\n");
        return 1;
    }
    char name[FISH_NAME_MAX_LENGTH];
    if (sscanf(args, "%s", name) < 1) {
        printf("command unrecognized\n");
        return 1;
    }

    struct fish_t *existedFish = next_fish(NULL);
    while (existedFish != NULL) {
        if (strcmp(existedFish->name, name) == 0) {
            existedFish->target_x = rand() % viewers_config.width;
            existedFish->target_y = rand() % viewers_config.height;
            existedFish->time_left = rand() % 10;
            snprintf(send_buffer, send_buffer_capacity, "OK\n");
            return 0;
        }
        existedFish = next_fish(existedFish);
    }
    snprintf(send_buffer, send_buffer_capacity, "NOK\n");
    return 0;
}

int handle_client_request(struct viewer_config_t **viewer_config,
                          char *receive_buffer, size_t receive_buffer_len,
                          char *send_buffer, size_t send_buffer_capacity) {
    pthread_t thread_getFish_constinously;
    // printf("%s\n", receive_buffer);

    /* if (strncmp(receive_buffer, "help", 4) == 0) { */
    /*   return help(parsed, server_response); */
    /* } */
    if (strncmp(receive_buffer, "hello", 5) == 0) {
        return greeting(viewer_config, receive_buffer + 6, receive_buffer_len,
                        send_buffer, send_buffer_capacity);
    }
    if (strncmp(receive_buffer, "getFishes", 7) == 0) {
        return list(viewer_config, send_buffer, send_buffer_capacity);
    }
    /* if (strncmp(receive_buffer, "ls", 2) == 0) { */
    /*   return listls(viewer_config, receive_buffer + 3, receive_buffer_len,
     * send_buffer, */
    /* 		send_buffer_capacity); */
    /* } */
    if (strncmp(receive_buffer, "getFishesContinuously", 21) == 0) {
        struct config_and_send_t args = {.assigned_config = viewer_config,
                                         .send_buffer = send_buffer};
        struct config_and_send_t *pargs = &args;
        pthread_create(&thread_getFish_constinously, NULL, actualList,
                       (void *)pargs);
        pthread_detach(thread_getFish_constinously);
        // return actualList( send_buffer,send_buffer_capacity);
    }
    if (strncmp(receive_buffer, "log", 3) == 0) {
        return bye(receive_buffer + 4, receive_buffer_len, send_buffer,
                   send_buffer_capacity);
    }
    if (strncmp(receive_buffer, "ping", 4) == 0) {
        return pong(receive_buffer + 5, receive_buffer_len, send_buffer,
                    send_buffer_capacity);
    }
    if (strncmp(receive_buffer, "addFish", 7) == 0) {
        return responseToAdd(receive_buffer + 8, receive_buffer_len,
                             send_buffer, send_buffer_capacity);
    }
    if (strncmp(receive_buffer, "delFish", 7) == 0) {
        return responseToDel(receive_buffer + 8, receive_buffer_len,
                             send_buffer, send_buffer_capacity);
    }
    if (strncmp(receive_buffer, "startFish", 9) == 0) {
        return responseToStrat(receive_buffer + 10, receive_buffer_len,
                               send_buffer, send_buffer_capacity);
    }

    fprintf(stderr, "[ERR] unknown protocol command\n");
    return -1;
}
