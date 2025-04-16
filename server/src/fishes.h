#ifndef __FISHES_H__
#define __FISHES_H__
#include <sys/queue.h>

struct fish_t {
    char *name;
    float current_x, current_y;
    float target_x, target_y;
    float time_left;
    TAILQ_ENTRY(fish_t) _next;
};

int add_fish(struct fish_t fish);

int remove_fish(struct fish_t *to_remove);

// warning: this is O(n)
int remove_fish_by_name(char *name);

// returns a ptr to the next fish in the iterator
// if called with f == NULL, returns the first fish
// if no next fish, returns NULL
struct fish_t *next_fish(struct fish_t *f);

#endif //__FISHES_H__
