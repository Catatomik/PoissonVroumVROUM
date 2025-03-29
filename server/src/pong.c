#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pong.h"

void help(struct parse *parsed, char *server_response) {
    // Je suis trop bete ca ecrit du coté du server
    printf("list of commandes:\n");

    // -------------------------------------------
    printf("hello\n");
    printf("Description: for a client to identify itself to the server and "
           "retrieve its settings\n");
    printf("Exemple request: > hello in as N3\n");
    printf("Exemple response: < greeting N3 0x500+500+500");

    // -------------------------------------------
    printf("getFish\n");
    printf("Description: controller returns the list of fish to be managed by "
           "the display program\n");
    printf("Exemple request: > getFishes\n");
    printf("Exemple response:  <list [PoissonRouge at 90x4,10x4,5] "
           "[PoissonClown at 20x80,12x6,5]");

    // -------------------------------------------
    printf("ls\n");
    printf("Description: controller returns the list of fish to be "
           "continuously managed by the display program\n");
    printf("Exemple request: > ls\n");
    printf("Exemple response: < list [PoissonRouge at 92x40,10x4,5] "
           "[PoissonClown at 22x80,12x6,5]\n< list [PoissonRouge at "
           "70x40,10x4,15] [PoissonClown at 10x90,12x6,6]\n< list "
           "[PoissonRouge at 30x30,10x4,8] [PoissonClown at 30x30,12x6,11]");

    //-------------------------------------------
    printf("log\n");
    printf("Description: deconnexion\n");
    printf("Exemple request: > log out\n");
    printf("Exemple response: < bye");

    //-------------------------------------------
    printf("ping\n");
    printf("Description: Keep connexion alive\n");
    printf("Exemple request: > ping 12345\n");
    printf("Exemple response: < pong 12345");

    //...
}
void greeting(struct parse *parsed, char *server_response) {
    strcpy(server_response, "not yet greeting");
}
void list(struct parse *parsed, char *server_response) {
    strcpy(server_response, "not yet list");
}
void listls(struct parse *parsed, char *server_response) {
    strcpy(server_response, "not yet listls");
}
void bye(struct parse *parsed, char *server_response) {
    strcpy(server_response, "not yet bye");
}

void pong(struct parse *parsed, char *server_response) {
    char res[20];
    strcpy(res, "pong");
    strncpy(res + strlen(res), parsed->argv[1], 5);
    strcpy(server_response, res);
}

void responseToAdd(struct parse *parsed, char *server_response) {
    strcpy(server_response, "not yet responseToAdd");
}
void responseToDel(struct parse *parsed, char *server_response) {
    strcpy(server_response, "not yet responseToDel");
}
void responseToStrat(struct parse *parsed, char *server_response) {
    strcpy(server_response, "not yet responseToStart");
}
void errorParsing(char *server_response) {
    strcpy(server_response,
           "error request not understand, do help for more information");
}

void parse_client_request(char *buffer_client, struct parse *parsed) {
    int letter = 0;
    int i = 0;
    int j = 0;
    while (buffer_client[letter] != '\0') {
        if (buffer_client[letter] == ' ') {
            j = 0;
            i++;
            parsed->argc++;
        }
        parsed->argv[i][j++] = buffer_client[letter];
        letter++;
    }
}

void client_request(char *buffer_client, struct parse *parsed,
                    char *server_response) {
    parse_client_request(buffer_client, parsed);

    if (strcmp(parsed->argv[0], "help") == 0) {
        return help(parsed, server_response);
    }
    if (strcmp(parsed->argv[0], "hello") == 0) {
        return greeting(parsed, server_response);
    }
    if (strcmp(parsed->argv[0], "getFish") == 0) {
        return list(parsed, server_response);
    }
    if (strcmp(parsed->argv[0], "ls") == 0) {
        return listls(parsed, server_response);
    }
    if (strcmp(parsed->argv[0], "log") == 0) {
        return bye(parsed, server_response);
    }
    if (strcmp(parsed->argv[0], "ping") == 0) {
        return pong(parsed, server_response);
    }
    if (strcmp(parsed->argv[0], "addFish") == 0) {
        return responseToAdd(parsed, server_response);
    }
    if (strcmp(parsed->argv[0], "delFish") == 0) {
        return responseToDel(parsed, server_response);
    }
    if (strcmp(parsed->argv[0], "startFish") == 0) {
        return responseToStrat(parsed, server_response);
    }
    return errorParsing(server_response);
}
