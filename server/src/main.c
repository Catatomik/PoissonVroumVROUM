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

#define MAX_INPUT 256

int help() {
    printf("ERROR : command not recognized\n you can make these cmds :\n\t> "
           "load <viewers config filepaths>\n\t> show aquarium\n\t>status "
           "\n\t> add view "
           "<Nid XxY+width+height>\n\t> del view <id>\n\t> save <file name>\n");
    return 0;
}

int load(char *input) {
    if (strlen(input) > 5) {
        if (viewers_config_from_file(&viewers_config, input + 5) != 0)
            return 1;

        printf("aquarium loaded (%d display view!)\n",
               viewers_config.viewers_count);
        return 0;
    }
    printf("No path to viewer config given\n");
    return 1;
}

int show() {
    printf("%dx%d\n", viewers_config.width, viewers_config.height);
    for (int i = 0; i < viewers_config.viewers_count; i++) {
        printf("N%d  %dx%d+%d+%d\n", viewers_config.viewers_configs[i].id,
               viewers_config.viewers_configs[i].x,
               viewers_config.viewers_configs[i].y,
               viewers_config.viewers_configs[i].width,
               viewers_config.viewers_configs[i].height);
    }
    return 0;
}

int add(char *input) {
    struct viewer_config_t newViewer;
    sscanf(input, "N%d %dx%d+%d+%d", &newViewer.id, &newViewer.x, &newViewer.y,
           &newViewer.width, &newViewer.height);

    for (int i=0 ; i< viewers_config.viewers_count; i++){
        if (viewers_config.viewers_configs[i].id == newViewer.id){
            printf("this id is already used for an other view");
            return 1;
        }
    }
    viewers_config.viewers_configs[viewers_config.viewers_count] = newViewer;
    viewers_config.viewers_count += 1;

    printf("view added\n");
    return 0;
}

int overwrite(int i) {
    for (int j = i; j < viewers_config.viewers_count - 1; j++) {
        viewers_config.viewers_configs[j] =
            viewers_config.viewers_configs[j + 1];
    }
    viewers_config.viewers_count -= 1;
    return 0;
}

int del(char *input) {
    int id = 0;
    sscanf(input, "N%d", &id);
    for (int i = 0; i < viewers_config.viewers_count; i++) {
        if (viewers_config.viewers_configs[i].id == id) {
            if (overwrite(i) != 0)
                return 1;
            printf("view N%d deleted\n", id);
            return 0;
        }
    }
    printf("view N%d not found\n", id);
    return 1;
}

int save(char *input) {
    FILE *filefd = fopen(input, "w");

    if (filefd == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return 1;
    }

    fprintf(filefd, "%dx%d\n", viewers_config.width, viewers_config.height);
    for (int i = 0; i < viewers_config.viewers_count; i++) {
        fprintf(filefd, "N%d  %dx%d+%d+%d\n",
                viewers_config.viewers_configs[i].id,
                viewers_config.viewers_configs[i].x,
                viewers_config.viewers_configs[i].y,
                viewers_config.viewers_configs[i].width,
                viewers_config.viewers_configs[i].height);
    }
    fclose(filefd);

    printf("Aquarium saved ! (%d display view!)\n",
           viewers_config.viewers_count);
    return 0;
}

int repl_handler(char *input) {
    printf("your command is %s\n", input);

    if (strncmp(input, "load ", 5) == 0) {
        return load(input);
    }
    if (strncmp(input, "show aquarium", 13) == 0) {
        return show(input);
    }
    if (strncmp(input, "add view", 8) == 0) {
        return add(input);
    }
    if (strncmp(input, "del view", 8) == 0) {
        return del(input);
    }
    if (strncmp(input, "save", 4) == 0) {
        return save(input);
    } else {
        return help();
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

        repl_handler(input);
    }

    printf("Hello, World!\n");
    return 0;
}
