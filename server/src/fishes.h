#ifndef __FISHES_H__
#define __FISHES_H__
#include <stdbool.h>
#include <sys/queue.h>

#define FISH_NAME_MAX_LENGTH 32

struct fish_t {
    char name[FISH_NAME_MAX_LENGTH];
    float current_x, current_y;
    float width, height;
    float target_x, target_y;
    float time_left;
    bool started;
    TAILQ_ENTRY(fish_t) _next;
};

// makes a copy of the current fish and adds it to the fishes
// returns != 0 if an error occured (eg: OOM)
int add_fish(struct fish_t fish);

// removes the given fish ptr from the fishes
// returns != 0 if an error occured
int remove_fish(struct fish_t *to_remove);

// removes the first fish with this name from the fishes
// returns != 0 if no fish was found and removed with this name
// warning: this is O(n)
int remove_fish_by_name(char *name);

// returns a ptr to the next fish in the iterator
// if called with f == NULL, returns the first fish
// if no next fish, returns NULL
struct fish_t *next_fish(struct fish_t *f);

// clears all the fishes
void remove_all_fishes();

// moves the fishes
void run_sea();

// for debugging purposes
void print_fishes();

#endif //__FISHES_H__
