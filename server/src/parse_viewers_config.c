#include "parse_viewers_config.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 256

void add_viewer_config(struct viewers_config_t *config, int id, int x, int y,
                       int width, int height) {
    assert(config != NULL);
    int config_index = config->viewers_count;
    if (config->viewers_configs == NULL) {
        assert(config->viewers_count == 0);
        config->viewers_configs = malloc(sizeof(struct viewer_config_t));
        config->viewers_count = 1;
    } else {
        config->viewers_count++;
        config->viewers_configs =
            realloc(config->viewers_configs,
                    sizeof(struct viewer_config_t) * config->viewers_count);
    }
    config->viewers_configs[config_index].id = id;
    config->viewers_configs[config_index].x = x;
    config->viewers_configs[config_index].y = y;
    config->viewers_configs[config_index].width = width;
    config->viewers_configs[config_index].height = height;
}

// Example viewers.config:
//
// {aquarium_width} x {aquarium_height}
// N1 {width}x{height}+{x}+{y}
// N2 {width}x{height}+{x}+{y}
// N3 {width}x{height}+{x}+{y}
// N4 {width}x{height}+{x}+{y}
int viewers_config_from_file(struct viewers_config_t *config,
                             const char *viewers_config_path) {
    config->viewers_configs = NULL;
    config->viewers_count = 0;

    FILE *fp;
    fp = fopen(viewers_config_path, "r");
    if (fp == NULL) {
        perror("Failed: ");
        return 1;
    }

    int i = 0;
    char buffer[MAX_LEN];
    while (fgets(buffer, MAX_LEN, fp)) {
        // Remove trailing newline
        buffer[strcspn(buffer, "\n")] = 0;
        if (i == 0) {
            // header is "{aquarium_width} x {aquarium_height}"
            sscanf(buffer, "%d x %d", &config->width, &config->height);
        } else {
            if (buffer[0] != 'N') {
                return 1; // Invalid format, viewers should be in the format
                          // "N{viewer_id} {width}x{height}+{x}+{y}"
            }
            int viewer_id;
            int x, y;
            int width, height;
            sscanf(buffer, "N%d %dx%d+%d+%d", &viewer_id, &x, &y, &width,
                   &height);
            add_viewer_config(config, viewer_id, width, height, x, y);
        }
        i++;
    }

    fclose(fp);
    return 0;
}

void viewers_config_free_internals(struct viewers_config_t *config) {
    if (config == NULL)
        return;
    if (config->viewers_configs == NULL)
        return;
    free(config->viewers_configs);
}
