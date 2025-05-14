#include "fishes.h"
#include "config.h"
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define FISH_SPEED 100.0

// declare struct for a head of list of elem's
TAILQ_HEAD(fishes_list_t, fish_t);

bool initialized = false;
struct fishes_list_t fishes;
pthread_mutex_t fishes_list_lock = PTHREAD_MUTEX_INITIALIZER;

int add_fish(struct fish_t fish) {
    pthread_mutex_lock(&fishes_list_lock);

    if (!initialized) {
        TAILQ_INIT(&fishes);
        initialized = true;
    }

    struct fish_t *fn = malloc(sizeof(struct fish_t));
    if (fn == NULL) {
        fprintf(stderr, "[ERR] OOM while allocating new fish\n");
        pthread_mutex_unlock(&fishes_list_lock);
        return 1;
    }
    *fn = fish;
    TAILQ_INSERT_TAIL(&fishes, fn, _next);
    pthread_mutex_unlock(&fishes_list_lock);
    return 0;
}

int remove_fish(struct fish_t *to_remove) {
    assert(to_remove != NULL);
    pthread_mutex_lock(&fishes_list_lock);
    TAILQ_REMOVE(&fishes, to_remove, _next);
    free(to_remove);
    pthread_mutex_unlock(&fishes_list_lock);
    return 0;
}

int remove_fish_by_name(char *name) {
    assert(name != NULL);
    pthread_mutex_lock(&fishes_list_lock);
    struct fish_t *pfish_node;
    TAILQ_FOREACH(pfish_node, &fishes, _next) {
        if (strcmp(pfish_node->name, name) == 0) {
            TAILQ_REMOVE(&fishes, pfish_node, _next);
            free(pfish_node);
            pthread_mutex_unlock(&fishes_list_lock);
            return 0;
        }
    }
    pthread_mutex_unlock(&fishes_list_lock);
    return 1;
}

void print_fishes() {
    pthread_mutex_lock(&fishes_list_lock);
    struct fish_t *f;
    TAILQ_FOREACH(f, &fishes, _next) { fprintf(stderr, "fish %s\n", f->name); }
    pthread_mutex_unlock(&fishes_list_lock);
}

struct fish_t *next_fish(struct fish_t *f) {
    if (f == NULL)
        return TAILQ_FIRST(&fishes);
    return TAILQ_NEXT(f, _next);
}

void remove_all_fishes() {
    while (!TAILQ_EMPTY(&fishes))
        remove_fish(next_fish(NULL));
}

void run_sea() {
    while (true) {
        pthread_mutex_lock(&fishes_list_lock);
        struct fish_t *f;
        TAILQ_FOREACH(f, &fishes, _next) {
            if (!f->started) {
                continue;
            }

            f->last_x = f->current_x;
            f->last_y = f->current_y;

            if (f->time_left > 0) {
                float vx = (f->target_x - f->current_x) / f->time_left;
                float vy = (f->target_y - f->current_y) / f->time_left;

                f->current_x += vx * FISH_UPDATE_INTERVAL;
                f->current_y += vy * FISH_UPDATE_INTERVAL;

                if (f->current_x < 0)
                    f->current_x = 0;
                if (f->current_y < 0)
                    f->current_y = 0;
                if (f->current_x > viewers_config.width - f->width)
                    f->current_x = viewers_config.width - f->width;
                if (f->current_y > viewers_config.height - f->height)
                    f->current_y = viewers_config.height - f->height;
            }

            f->time_left -= FISH_UPDATE_INTERVAL;
            if (f->time_left <= 0.) {
                // - fish size to not go out of the aquarium
                f->target_x = ((float)rand() / RAND_MAX) *
                              (viewers_config.width - f->width);
                f->target_y = ((float)rand() / RAND_MAX) *
                              (viewers_config.height - f->height);

                float dx = f->target_x - f->current_x;
                float dy = f->target_y - f->current_y;
                float distance = sqrt(dx * dx + dy * dy);
                f->time_left = distance / FISH_SPEED;
            }
        }
        pthread_mutex_unlock(&fishes_list_lock);
        // 0.1s
        usleep(FISH_UPDATE_INTERVAL * 1e6);
    }
}
