#ifndef __PONG_H__
#define __PONG_H__

struct parse {
    int argc;
    char *argv[256];
};

void help(struct parse *parsed, char *server_response);
void greeting(struct parse *parsed, char *server_response);
void list(struct parse *parsed, char *server_response);
void listls(struct parse *parsed, char *server_response);
void bye(struct parse *parsed, char *server_response);
void pong(struct parse *parsed, char *server_response);
void responseToAdd(struct parse *parsed, char *server_response);
void responseToDel(struct parse *parsed, char *server_response);
void responseToStrat(struct parse *parsed, char *server_response);
void errorParsing(char *server_response);

void parse_client_request(char *buffer_client, struct parse *parsed);
void client_request(char *buffer_client, struct parse *parsed,
                    char *server_response);

#endif //__PONG_H__
