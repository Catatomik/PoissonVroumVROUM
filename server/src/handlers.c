#include "handlers.h"
#include "config.h"
#include "fishes.h"
#include "parse_viewers_config.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

bool is_in_view(float x, float y, float w, float h,
                struct viewer_config_t *viewer_config) {
    return (x < viewer_config->x + viewer_config->width) &&
           (x + w > viewer_config->x) &&
           (y < viewer_config->y + viewer_config->height) &&
           (y + h > viewer_config->y);
}

int get_fishes(struct viewer_config_t *viewer_config, char *send_buffer,
               size_t send_buffer_capacity) {
    assert(viewer_config !=
           NULL); // cannot send to a viewer which doesn't exist

    int printed_count = snprintf(send_buffer, send_buffer_capacity, "list");
    if (printed_count > send_buffer_capacity)
        goto err;

    struct fish_t *f = next_fish(NULL);
    while (f != NULL) {
        // if any part of the fish is visible in the view window, send it
        bool fish_was_already_in_view = is_in_view(
            f->last_x, f->last_y, f->width, f->height, viewer_config);

        if (is_in_view(f->current_x, f->current_y, f->width, f->height,
                       viewer_config)) {
            float sent_x =
                (!fish_was_already_in_view) ? f->current_x : f->target_x;
            float sent_y =
                (!fish_was_already_in_view) ? f->current_y : f->target_y;

            // convert fish coordinates to percentage of view window
            float rel_x =
                ((sent_x - viewer_config->x) * 100.0) / viewer_config->width;
            float rel_y =
                ((sent_y - viewer_config->y) * 100.0) / viewer_config->height;

            printed_count += snprintf(
                send_buffer + printed_count,
                send_buffer_capacity - printed_count,
                " [%s at %.0fx%.0f,%fx%f,%f]", f->name, rel_x, rel_y, f->width,
                f->height,
                (!f->started || !fish_was_already_in_view) ? 0 : f->time_left);
            if (printed_count > send_buffer_capacity)
                goto err;
        }
        f = next_fish(f);
    }
    send_buffer[printed_count] = '\n';
    return 0;

err:
    fprintf(stderr,
            "[ERR] response would have overran send_buffer (len=%zu) because "
            "of arg len=%d\n",
            send_buffer_capacity, printed_count);
    return -1;
}

struct continuously_thread_args {
    int fd;
    struct viewer_config_t *viewer_config;
};
void *get_fishes_continuously_start(void *gargs) {
    assert(gargs != NULL); // should always be non null
    struct continuously_thread_args *args = gargs;
    printf("[LOG] starting get_fishes_continuously thread for fd %d\n",
           args->fd);

    char send_buffer[1024] = {0};
    while (write(args->fd, "", 0) != -1) {
        get_fishes(args->viewer_config, send_buffer, 1024);
        size_t send_length = strnlen(send_buffer, 1024);
        if (send_length > 0) {
            int n = write(args->fd, send_buffer, send_length);
            if (n < 0)
                fprintf(stderr, "[ERR] ERROR writing to socket");
            send_buffer[send_length] = '\0';
            printf("[DEBUG] sending '%s'\n", send_buffer);
        }
        usleep(config.fish_update_interval * 1000000);
        memset(send_buffer, 0, 1024);
    }

    free(gargs); // ugly but since the thread is detached, It's necessary
    return 0;
}

int get_fishes_continuously(int fd, struct viewer_config_t *viewer_config,
                            char *send_buffer, size_t send_buffer_capacity) {

    pthread_t thread_get_fishes_continuously;
    struct continuously_thread_args *th_args =
        malloc(sizeof(struct continuously_thread_args));
    assert(th_args != NULL); // OOM
    th_args->fd = fd;
    th_args->viewer_config = viewer_config;
    pthread_create(&thread_get_fishes_continuously, NULL,
                   get_fishes_continuously_start, th_args);
    pthread_detach(thread_get_fishes_continuously);

    return 0;
}

int bye(int fd, char *send_buffer, size_t send_buffer_capacity) {
    // should stop the thread and while loop in connexion
    snprintf(send_buffer, send_buffer_capacity, "bye\n");
    return 1;
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
int responseToAdd(struct viewer_config_t *viewer_config, char *args,
                  size_t args_len, char *send_buffer,
                  size_t send_buffer_capacity) {
    if (args_len <= 0) {
        snprintf(send_buffer, send_buffer_capacity,
                 "error no fish details described\n");
        return -1;
    }
    struct fish_t newFish;
    if (sscanf(args, "%s at %fx%f,%fx%f,RandomWayPoint", newFish.name,
               &newFish.current_x, &newFish.current_y, &newFish.width,
               &newFish.height) < 5) {
        printf("command unrecognized\n");
        return -1;
    }

    newFish.current_x =
        viewer_config->x + viewer_config->width * newFish.current_x / 100;
    newFish.current_y =
        viewer_config->y + viewer_config->height * newFish.current_y / 100;

    newFish.last_x = newFish.current_x;
    newFish.last_y = newFish.current_y;

    newFish.target_x = newFish.current_x;
    newFish.target_y = newFish.current_y;
    newFish.started = false;
    newFish.time_left = 0;

    struct fish_t *existedFish = next_fish(NULL);
    while (existedFish != NULL) {
        if (strcmp(existedFish->name, newFish.name) == 0) {
            snprintf(send_buffer, send_buffer_capacity, "NOK\n");
            return -1;
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
        return -1;
    }
    char name[FISH_NAME_MAX_LENGTH];
    if (sscanf(args, "%s", name) < 1) {
        printf("command unrecognized\n");
        return -1;
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
int responseToStart(char *args, size_t args_len, char *send_buffer,
                    size_t send_buffer_capacity) {
    if (args_len <= 0) {
        snprintf(send_buffer, send_buffer_capacity,
                 "error no fish name gave\n");
        return -1;
    }
    char name[FISH_NAME_MAX_LENGTH];
    if (sscanf(args, "%s", name) < 1) {
        printf("command unrecognized\n");
        return -1;
    }

    struct fish_t *f = next_fish(NULL);
    while (f != NULL) {
        if (strcmp(f->name, name) == 0) {
            f->started = true;
            snprintf(send_buffer, send_buffer_capacity, "OK\n");
            return 0;
        }
        f = next_fish(f);
    }
    snprintf(send_buffer, send_buffer_capacity, "NOK\n");
    return 0;
}

int handle_client_request(int fd, struct viewer_config_t **viewer_config,
                          char *receive_buffer, size_t receive_buffer_len,
                          char *send_buffer, size_t send_buffer_capacity) {
    if (strncmp(receive_buffer, "hello", 5) == 0) {
        return greeting(viewer_config, receive_buffer + 6, receive_buffer_len,
                        send_buffer, send_buffer_capacity);
    }
    if (strncmp(receive_buffer, "getFishes\n", 10) == 0) {
        return get_fishes(*viewer_config, send_buffer, send_buffer_capacity);
    }
    if (strncmp(receive_buffer, "getFishesContinuously\n", 22) == 0) {
        return get_fishes_continuously(fd, *viewer_config, send_buffer,
                                       send_buffer_capacity);
    }
    if (strncmp(receive_buffer, "log out\n", 8) == 0) {
        return bye(fd, send_buffer, send_buffer_capacity);
    }
    if (strncmp(receive_buffer, "ping", 4) == 0) {
        return pong(receive_buffer + 5, receive_buffer_len, send_buffer,
                    send_buffer_capacity);
    }
    if (strncmp(receive_buffer, "addFish", 7) == 0) {
        return responseToAdd(*viewer_config, receive_buffer + 8,
                             receive_buffer_len, send_buffer,
                             send_buffer_capacity);
    }
    if (strncmp(receive_buffer, "delFish", 7) == 0) {
        return responseToDel(receive_buffer + 8, receive_buffer_len,
                             send_buffer, send_buffer_capacity);
    }
    if (strncmp(receive_buffer, "startFish", 9) == 0) {
        return responseToStart(receive_buffer + 10, receive_buffer_len,
                               send_buffer, send_buffer_capacity);
    }

    fprintf(stderr, "[ERR] unknown protocol command\n");
    return -1;
}
