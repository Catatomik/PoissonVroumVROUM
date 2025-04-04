#include "parse_cfg.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MAX_LEN 256

int config_from_file(struct config_t *config, const char *cfgpath) {
    bool controller_port_found = false;
    bool display_timeout_value_found = false;
    bool fish_update_interval_found = false;

    FILE *fp;
    fp = fopen(cfgpath, "r");
    if (fp == NULL) {
        fprintf(stderr, "[ERR][%s:%s] failed to open file\n", __FILE__,
                __FUNCTION__);
        return 1;
    }

    char buffer[MAX_LEN];
    while (fgets(buffer, MAX_LEN, fp)) {
        // fprintf(stderr, "'%s'", buffer);
        // Remove trailing newline
        buffer[strcspn(buffer, "\n")] = 0;
        char *first_equal_pos = strchr(buffer, '=');
        char *value_start = first_equal_pos + 1;

        if (strncmp(buffer, "controller-port", 15) == 0) {
            sscanf(value_start, "%d", &config->controller_port);
            controller_port_found = true;
        }
        if (strncmp(buffer, "display-timeout-value", 21) == 0) {
            sscanf(value_start, "%d", &config->display_timeout_value);
            display_timeout_value_found = true;
        }
        if (strncmp(buffer, "fish-update-interval", 20) == 0) {
            sscanf(value_start, "%d", &config->fish_update_interval);
            fish_update_interval_found = true;
        }
    }

    fclose(fp);
    bool success = controller_port_found && display_timeout_value_found &&
                   fish_update_interval_found;
    return !success;
}
