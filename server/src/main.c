#include "utils.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "connexion.h"
#include "parse_cfg.h"

#define SEND_CONFIG_BUFFER_CAPACITY 1024

int help(char *send_config_buffer) {
    strcpy(send_config_buffer,
           "ERROR : command not recognized\n you can make these cmds :\n\t> "
           "load\n\t> show\n\t> add\n\t> del\n\t> save\n");
    return 0;
}

int load(input, send_config_buffer, send_config_buffer_capacity) {
    char aquaConfig[256] = {0};
    sscanf(input, "%s", &aquaConfig);
    viewers_config_from_file(viewers_config, aquaConfig);

    snprintf(send_config_buffer, sizeof(send_config_buffer),
             "aquarium loaded (%d display view!)\n",
             viewers_config.viewers_count);
    return 0;
}

int show(input, send_config_buffer, send_config_buffer_capacity) {
    char *tmp[viewers_config.viewers_count];
    snprintf(send_config_buffer, sizeof(send_config_buffer), "%dx%d\n");
    for (int i = 0; i < viewers_config.viewers_count; i++) {
        snprintf(send_config_buffer + strlen(send_config_buffer),
                 sizeof(send_config_buffer) - strlen(send_config_buffer),
                 "N%d  %dx%d+%d+%d\n", viewers_config.viewers_count[i]->id,
                 viewers_config.viewers_count[i]->x,
                 viewers_config.viewers_count[i]->y,
                 viewers_config.viewers_count[i]->width,
                 viewers_config.viewers_count[i]->height);
    }
    return 0;
}

int add(input, send_config_buffer, send_config_buffer_capacity) {
    // TO DO
    snprintf(send_config_buffer, sizeof(send_config_buffer), "view added\n");
    return 0;
}

int del(input, send_config_buffer, send_config_buffer_capacity) {
    // TO DO
    snprintf(send_config_buffer, sizeof(send_config_buffer), "view deleted\n");
    return 0;
}

int save(input, send_config_buffer, send_config_buffer_capacity) {
    // TO DO
    snprintf(send_config_buffer, sizeof(send_config_buffer),
             "Aquarium saved ! (%d display view!)\n",
             viewers_config.viewers_count);
    return 0;
}

int repl_handler(struct config_t *config, char *input, char *send_config_buffer,
                 size_t send_config_buffer_capacity) {
    printf("%s\n", input);

    if (strncmp(input, "load", 4) == 0) {
        return load(input, send_config_buffer, send_config_buffer_capacity);
    }
    if (strncmp(input, "show", 4) == 0) {
        return show(input, send_config_buffer, send_config_buffer_capacity);
    }
    if (strncmp(input, "add view", 8) == 0) {
        return add(input, send_config_buffer, send_config_buffer_capacity);
    }
    if (strncmp(input, "del view", 8) == 0) {
        return del(input, send_config_buffer, send_config_buffer_capacity);
    }
    if (strncmp(input, "save", 4) == 0) {
        return save(input, send_config_buffer, send_config_buffer_capacity);
    } else {
        return help(send_config_buffer);
    }
}

int main(int argc, char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    pthread_t thread_connexion;

    // init config structur of controller configuration file
    struct config_t *config = malloc(sizeof(struct config_t));
    char input[MAX_INPUT];
    char send_config_buffer[SEND_CONFIG_BUFFER_CAPACITY] = {0};

    printf("Bienvenue dans le REPL config. Tapez 'exit' pour quitter.\n");

    if (config_from_file(config, "./controller.cfg")) {
        pthread_create(&thread_connexion, NULL, start, (void *)config);
        pthread_detach(thread_connexion);

        while (1) {
            printf("> ");
            if (fgets(input, MAX_INPUT, stdin) == NULL) {
                break;
            }

            // Supprime le saut de ligne
            input[strcspn(input, "\n")] = '\0';

            if (strcmp(input, "exit") == 0) {
                break;
            }

            repl_handler(config, input, send_config_buffer,
                         SEND_CONFIG_BUFFER_CAPACITY);
        }

    } else {
        printf("Error while parsing config file");
        return -1;
    }

    printf("Hello, World!\n");
    return 0;
}
