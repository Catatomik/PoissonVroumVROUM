#include "fishes.h"
#include "parse_cfg.h"
#include "parse_viewers_config.h"
#include "utils.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct viewers_config_t viewers_config;
struct config_t config;

int main(int argc, char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    struct fish_t f1 = {.name = "blublu",
                        .current_x = 5,
                        .current_y = 3,
                        .width = 4,
                        .height = 4,
                        .target_x = 8,
                        .target_y = 9,
                        .time_left = 3};

    // INIT
    {
        // there shouldn't be any fishes initially
        assert(next_fish(NULL) == NULL);
    }

    // ADDING
    struct fish_t *fp;
    {
        add_fish(f1);
        fp = next_fish(NULL);
        assert(fp != NULL); // added a fish so there is a fish

        // check that the added fish is the same
        assert((strcmp(fp->name, f1.name) == 0) &&
               (fp->current_x == f1.current_x) &&
               (fp->current_y == f1.current_y) && (fp->width == f1.width) &&
               (fp->height == f1.height) && (fp->target_x == f1.target_x) &&
               (fp->target_y == f1.target_y) &&
               (fp->time_left == f1.time_left));
    }

    // REMOVAL
    {
        assert(remove_fish(fp) == 0); // should go well
        // there shouldn't be any more fishes
        assert(next_fish(NULL) == NULL);
    }

    // REMOVAL BY NAME
    {
        add_fish(f1);
        // there should be a fish now
        assert(next_fish(NULL) != NULL);

        assert(remove_fish_by_name("blabla") !=
               0); // removing a fish which isn't here should return an error
        // there should still be a fish
        assert(next_fish(NULL) != NULL);

        assert(remove_fish_by_name(f1.name) ==
               0); // try removing a fish which is here
        // there shouldn't be any more fishes
        assert(next_fish(NULL) == NULL);
    }

    return 0;
}
