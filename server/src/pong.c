#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pong.h"

// void help(char *args, char *server_response) {
//     // general help
//     if (parsed->argc == 1) {
//         char *intro =
//             "For more information about a cmd, do help <cmd>\nlist of "
//             "commandes:\n\t> hello\n\t> getFish\n\t> ls\n\t> log\n\t>
//             ping\n";
//         strcpy(server_response, intro);
//     }

//     // help hello
//     if (strcmp(parsed->argv[1], "hello") == 0) {
//         char *hello = "hello------------------------------\nDescription: for
//         a "
//                       "client to identify itself to the server and retrieve "
//                       "its settings\nExemple request: > hello in as "
//                       "N3\nExemple response: < greeting N3 0x500+500+500\n";
//         strcpy(server_response, hello);
//     }

//     // help getFish
//     if (strcmp(parsed->argv[1], "getFish") == 0) {
//         char *getFish =
//             "getFish-----------------------------\nDescription: controller "
//             "returns the list of fish to be managed by the display "
//             "program\nExemple request: > getFishes\nExemple response:  <list
//             "
//             "[PoissonRouge at 90x4,10x4,5] [PoissonClown at 20x80,12x6,5]\n";
//         strcpy(server_response, getFish);
//     }

//     // help ls
//     if (strcmp(parsed->argv[1], "ls") == 0) {
//         char *ls =
//             "ls--------------------------------\nDescription: controller "
//             "returns the list of fish to be continuously managed by the "
//             "display program\nExemple request: > ls\nExemple response: < list
//             "
//             "[PoissonRouge at 92x40,10x4,5] [PoissonClown at 22x80,12x6,5]\n<
//             " "list [PoissonRouge at 70x40,10x4,15] [PoissonClown at "
//             "10x90,12x6,6]\n< list [PoissonRouge at 30x30,10x4,8] "
//             "[PoissonClown at 30x30,12x6,11]";
//         strcpy(server_response, ls);
//     }

//     // help log
//     if (strcmp(parsed->argv[1], "log") == 0) {
//         char *log =
//             "log------------------------------\nDescription: "
//             "deconnexion\nExemple request: > log out\nExemple response: <
//             bye";
//         strcpy(server_response, log);
//     }

//     //-------------------------------------------
//     if (strcmp(parsed->argv[1], "ping") == 0) {
//         char *ping = "ping----------------------------\nDescription: Keep "
//                      "connexion alive\nExemple request: > ping 12345\nExemple
//                      " "response: < pong 12345";
//         strcpy(server_response, ping);
//     }

//     else {
//         char *error = "no cmd corespond to what you wrote, maybe you
//         miswrite!"; strcpy(server_response, error);
//     }

//     //...
// }
// void greeting(char *args, char *server_response) {
//     strcpy(server_response, "not yet greeting");
// }
// void list(char *args, char *server_response) {
//     strcpy(server_response, "not yet list");
// }
// void listls(char *args, char *server_response) {
//     strcpy(server_response, "not yet listls");
// }
// void bye(char *args, char *server_response) {
//     strcpy(server_response, "not yet bye");
// }

int pong(char *args, size_t args_len, char *send_buffer,
         size_t send_buffer_capacity) {
    int printed_count =
        snprintf(send_buffer, send_buffer_capacity, "pong %s\n", args);
    if (printed_count > send_buffer_capacity) {
        fprintf(
            stderr,
            "[ERR] response would have overran send_buffer (len=%zu) because "
            "of arg len=%d\n",
            send_buffer_capacity, printed_count);
        return -1;
    }
    return 0;
}

// void responseToAdd(char *args, char *server_response) {
//     strcpy(server_response, "not yet responseToAdd");
// }
// void responseToDel(char *args, char *server_response) {
//     strcpy(server_response, "not yet responseToDel");
// }
// void responseToStrat(char *args, char *server_response) {
//     strcpy(server_response, "not yet responseToStart");
// }

int handle_client_request(char *receive_buffer, size_t receive_buffer_len,
                          char *send_buffer, size_t send_buffer_capacity) {
    // printf("%s\n", receive_buffer);

    // if (strcmp(parsed->argv[0], "help") == 0) {
    //     return help(parsed, server_response);
    // }
    // if (strcmp(parsed->argv[0], "hello") == 0) {
    //     return greeting(parsed, server_response);
    // }
    // if (strcmp(parsed->argv[0], "getFish") == 0) {
    //     return list(parsed, server_response);
    // }
    // if (strcmp(parsed->argv[0], "ls") == 0) {
    //     return listls(parsed, server_response);
    // }
    // if (strcmp(parsed->argv[0], "log") == 0) {
    //     return bye(parsed, server_response);
    // }
    if (strncmp(receive_buffer, "ping", 4) == 0) {
        return pong(receive_buffer + 4, receive_buffer_len, send_buffer,
                    send_buffer_capacity);
    }
    // if (strcmp(parsed->argv[0], "addFish") == 0) {
    //     return responseToAdd(parsed, server_response);
    // }
    // if (strcmp(parsed->argv[0], "delFish") == 0) {
    //     return responseToDel(parsed, server_response);
    // }
    // if (strcmp(parsed->argv[0], "startFish") == 0) {
    //     return responseToStrat(parsed, server_response);
    // }

    fprintf(stderr, "[ERR] unknown protocol command\n");
    return -1;
}
