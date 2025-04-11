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

    if (config_from_file(&config, "./controller.cfg") != 0) {
        printf("Error while parsing controller.cfg");
        return -1;
    }
    if (viewers_config_from_file(&viewers_config, "./viewers.config") != 0) {
        printf("Error while parsing viewers.config");
        return -1;
    }

    pthread_create(&thread_connexion, NULL, start, NULL);
    pthread_detach(thread_connexion);

    // CLI
    while (1) {
    }

    printf("Hello, World!\n");
    return 0;
}
