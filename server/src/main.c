#include "connexion.h"
#include "parse_cfg.h"
#include "parse_viewers_config.h"
#include "utils.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct viewers_config_t viewers_config;
struct config_t config;

int main(int argc, char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    // TODO: parse configs
    pthread_t thread_connexion;

    if (config_from_file(&config, "./controller.cfg") == 0 &&
        viewers_config_from_file(&viewers_config, "./viewers.config") == 0) {

        // const char *controller_addr = "127.0.0.1";
        // int controller_port = config.controller_port;
        // int display_timeout_value = config.display_timeout_value;
        // int fish_update_interval = config.fish_update_interval;

        pthread_create(&thread_connexion, NULL, start, NULL);
        pthread_detach(thread_connexion);
        while (1) {
        }
    } else {
        printf("Error while parsing configs files");
        return -1;
    }

    printf("Hello, World!\n");
    return 0;
}
