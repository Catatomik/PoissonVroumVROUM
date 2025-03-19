#include "parse_viewers_config.h"
#include <stdio.h>

int main() {
    struct viewers_config_t config;
    viewers_config_from_file(&config, "viewers.config");

    printf("Aquarium width=%d, Aquarium height=%d\n", config.width,
           config.height);
    printf("Viewer count=%d\n", config.viewers_count);
    printf("Viewers configs: \n");
    for (int i = 0; i < config.viewers_count; i++) {
        printf("\tViewer %d: width=%d height=%d x=%d y=%d\n",
               config.viewers_configs[i].id, config.viewers_configs[i].width,
               config.viewers_configs[i].height, config.viewers_configs[i].x,
               config.viewers_configs[i].y);
    }

    viewers_config_free_internals(&config);
}
