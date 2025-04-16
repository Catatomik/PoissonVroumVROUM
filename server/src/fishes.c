#include "fishes.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// declare struct for a head of list of elem's
TAILQ_HEAD(fishes_list_t, fish_t);

bool initialized = false;
struct fishes_list_t fishes;

int add_fish(struct fish_t fish) {
    if (!initialized)
        TAILQ_INIT(&fishes);

    struct fish_t *fn = malloc(sizeof(struct fish_t));
    if (fn == NULL) {
        fprintf(stderr, "[ERR] OOM while allocating new fish\n");
        return 1;
    }
    *fn = fish;
    TAILQ_INSERT_TAIL(&fishes, fn, _next);
    return 0;
}

int remove_fish(struct fish_t *to_remove) {
    assert(to_remove != NULL);
    TAILQ_REMOVE(&fishes, to_remove, _next);
    free(to_remove);
    return 0;
}

int remove_fish_by_name(char *name) {
    assert(name != NULL);
    struct fish_t *pfish_node;
    TAILQ_FOREACH(pfish_node, &fishes, _next) {
        if (strcmp(pfish_node->name, name) == 0) {
            TAILQ_REMOVE(&fishes, pfish_node, _next);
            free(pfish_node);
            return 0;
        }
    }
    return 1;
}

struct fish_t *next_fish(struct fish_t *f) {
    if (f == NULL)
        return TAILQ_FIRST(&fishes);
    return TAILQ_NEXT(f, _next);
}
