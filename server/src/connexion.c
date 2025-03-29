/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "connexion.h"
#include "pong.h"

pthread_mutex_t mutex;

typedef struct {
    int client_sockfd;
    int *nb_thread_used;
    int thread_id;
} threads_client_args_t;

void error(char *msg) {
    perror(msg);
    exit(1);
}

int config_socket(int portno, const char *ip_addr,
                  struct sockaddr_in *serv_addr) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
        error("ERROR opening socket");

    bzero((char *)serv_addr, sizeof(*serv_addr));
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_addr.s_addr = inet_addr(ip_addr);
    serv_addr->sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *)serv_addr, sizeof(*serv_addr)) < 0)
        error("ERROR on binding");
    return sockfd;
}

void *thread_function_client(void *args) {
    threads_client_args_t *client_args = (threads_client_args_t *)args;
    int client_sockfd = client_args->client_sockfd;
    int *nb_thread_used = client_args->nb_thread_used;
    int thread_id = client_args->thread_id;

    char buffer_read[256];
    char buffer_write[256];
    int n;

    // Handles socket exchange
    bzero(buffer_read, 256);
    bzero(buffer_write, 256);

    n = read(client_sockfd, buffer_read, 255);
    if (n < 0)
        error("ERROR reading from socket");

    // printf("The message from client %d: %s\n", thread_id, buffer_read);
    // n = write(client_sockfd, "I got your message", 18);

    struct parse parsed = {};
    for (int i = 0; i < 256; i++) {
        parsed.argv[i] = malloc(sizeof(char) * 256);
    }
    printf("The message from client %d: %s\n", thread_id, buffer_read);
    client_request(buffer_read, &parsed, buffer_write);
    n = write(client_sockfd, buffer_write, sizeof(buffer_write));
    if (n < 0)
        error("ERROR writing to socket");
    close(client_sockfd);

    // mark this thread as free to deal with another sockfd
    pthread_mutex_lock(&mutex);
    *nb_thread_used -= 1;
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void *start(void *config) {

    int next_id = 0;
    int thread_id;
    int nb_thread_used = 0;
    pthread_t thread_client;
    pthread_mutex_init(&mutex, NULL); // Initialisation du mutex

    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, cli_addr;

    sockfd = config_socket(((struct config_t *)config)->controller_port,
                           "127.0.0.1", &serv_addr);
    int clilen = (sizeof(cli_addr));

    // Add infinite loop to keep server running and accept new connections
    while (1) {
        listen(sockfd, 5);
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr,
                           (unsigned int *restrict)&clilen);
        if (newsockfd < 0)
            error("ERROR on accept");

        if (nb_thread_used < 5) {
            pthread_mutex_lock(&mutex);
            nb_thread_used += 1;
            pthread_mutex_unlock(&mutex);

            thread_id = next_id++;
            printf("nb_thread_used %d\n", nb_thread_used);

            // structure create for good argument in thread function
            threads_client_args_t *args = malloc(sizeof(threads_client_args_t));
            args->client_sockfd = newsockfd;
            args->nb_thread_used = &nb_thread_used;
            args->thread_id = thread_id;

            if (pthread_create(&thread_client, NULL, thread_function_client,
                               (void *)args)) {
                fprintf(stderr, "ERROR creation client thread\n");
                close(newsockfd);
                free(args);
                continue;
            }
        } else {
            usleep(3);
        }

        // not pthread_join because we don't wait the end of thread (parallize)
        pthread_detach(thread_client);
    }

    pthread_mutex_destroy(&mutex);
    return NULL;
}
