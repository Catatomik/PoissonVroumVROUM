#include "connexion.h"
#include "parse_cfg.h"
#include "parse_viewers_config.h"
#include "utils.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct viewers_config_t viewers_config;
struct config_t config;

#define SEND_CONFIG_BUFFER_CAPACITY 1024
#define MAX_INPUT 256

int help(char *send_config_buffer) {
    strcpy(send_config_buffer,
           "ERROR : command not recognized\n you can make these cmds :\n\t> "
           "load\n\t> show\n\t> add\n\t> del\n\t> save\n");
    return 0;
}

int load(char *input, char *send_config_buffer,
         size_t send_config_buffer_capacity) {
    char aquaConfig[256] = {0};
    sscanf(input, "%s", aquaConfig);
    viewers_config_from_file(&viewers_config, aquaConfig);

    snprintf(send_config_buffer, send_config_buffer_capacity,
             "aquarium loaded (%d display view!)\n",
             viewers_config.viewers_count);
    return 0;
}

int show(char *input, char *send_config_buffer,
         size_t send_config_buffer_capacity) {
    snprintf(send_config_buffer, send_config_buffer_capacity, "%dx%d\n",
             viewers_config.width, viewers_config.height);
    for (int i = 0; i < viewers_config.viewers_count; i++) {
        snprintf(send_config_buffer + strlen(send_config_buffer),
                 sizeof(send_config_buffer) - strlen(send_config_buffer),
                 "N%d  %dx%d+%d+%d\n", viewers_config.viewers_configs[i].id,
                 viewers_config.viewers_configs[i].x,
                 viewers_config.viewers_configs[i].y,
                 viewers_config.viewers_configs[i].width,
                 viewers_config.viewers_configs[i].height);
    }
    return 0;
}

int add(char *input, char *send_config_buffer,
        size_t send_config_buffer_capacity) {
    // TO DO
    snprintf(send_config_buffer, send_config_buffer_capacity, "view added\n");
    return 0;
}

int del(char *input, char *send_config_buffer,
        size_t send_config_buffer_capacity) {
    // TO DO
    snprintf(send_config_buffer, send_config_buffer_capacity, "view deleted\n");
    return 0;
}

int save(char *input, char *send_config_buffer,
         size_t send_config_buffer_capacity) {
    // TO DO
    snprintf(send_config_buffer, send_config_buffer_capacity,
             "Aquarium saved ! (%d display view!)\n",
             viewers_config.viewers_count);
    return 0;
}

int repl_handler(char *input, char *send_config_buffer,
                 size_t send_config_buffer_capacity) {
    printf("stupid, %s\n", input);

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

    printf("Bienvenue dans le REPL config. Tapez 'exit' pour quitter.\n");
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
    // pthread_create(&thread_connexion, NULL, start, (void *)config);

    // init config structur of controller configuration file
    char input[MAX_INPUT];
    char send_config_buffer[SEND_CONFIG_BUFFER_CAPACITY] = {0};
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

        repl_handler(input, send_config_buffer, SEND_CONFIG_BUFFER_CAPACITY);
        printf("%s\n", send_config_buffer);
    }

    printf("Hello, World!\n");
    return 0;
}
