/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include "connexion.h"
#include "config.h"
#include "handlers.h"
#include "parse_cfg.h"
#include "parse_viewers_config.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int is_fd_closed(int fd) {
    int flags = fcntl(fd, F_GETFD);
    if (flags == -1 && errno == EBADF) {
        return 1; // FD is closed
    }
    return 0; // FD is open
}

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

    // Make the port reusable
    int enabled = 1;
    int rc;
    rc = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&enabled,
                    sizeof(enabled));
    if (rc < 0) {
        perror("setsockopt() failed");
        close(sockfd);
        exit(-1);
    }

    if (bind(sockfd, (struct sockaddr *)serv_addr, sizeof(*serv_addr)) < 0)
        error("ERROR on binding");
    return sockfd;
}

#define RECEIVE_BUFFER_CAPACITY 256
#define SEND_BUFFER_CAPACITY 512
#define TIMEOUT_SECONDS 30

void *thread_function_client(void *args) {
    threads_client_args_t *client_args = (threads_client_args_t *)args;
    int client_sockfd = client_args->client_sockfd;
    int *nb_thread_used = client_args->nb_thread_used;
    int thread_id = client_args->thread_id;
    struct viewer_config_t *viewer_config = NULL;

    size_t receive_buffer_len = 0;
    char receive_buffer[RECEIVE_BUFFER_CAPACITY] = {0};
    char send_buffer[SEND_BUFFER_CAPACITY] = {0};

    fd_set read_fds;
    struct timeval timeout;

    while (!is_fd_closed(client_sockfd)) {
        FD_ZERO(&read_fds);
        FD_SET(client_sockfd, &read_fds);

        timeout.tv_sec = TIMEOUT_SECONDS;
        timeout.tv_usec = 0;

        int activity =
            select(client_sockfd + 1, &read_fds, NULL, NULL, &timeout);

        if (activity == 0) {
            // Timeout reached with no activity
            printf("[INFO] Timeout client %d, closing connection\n", thread_id);
            close(client_sockfd);
            break;
        }

        int n = read(client_sockfd, receive_buffer + receive_buffer_len,
                     RECEIVE_BUFFER_CAPACITY - receive_buffer_len - 1);
        if (n < 0) {
            perror("ERROR reading from socket");
            close(client_sockfd);
            break;
        }
        receive_buffer_len += n;
        receive_buffer[receive_buffer_len] = '\0';

        if (strchr(receive_buffer, '\n') != NULL) {
            // we have a full command
            printf("The message from client %d: %s\n", thread_id,
                   receive_buffer);
            memset(send_buffer, 0, SEND_BUFFER_CAPACITY);

            int res = handle_client_request(client_sockfd, &viewer_config,
                                            receive_buffer, receive_buffer_len,
                                            send_buffer, SEND_BUFFER_CAPACITY);
            if (res != 1 && res != 0) {
                fprintf(stderr, "Error handling client request\n");
                // TODO: handle it ?
            }
            size_t send_length = strnlen(send_buffer, SEND_BUFFER_CAPACITY);
            if (send_length > 0) {
                n = write(client_sockfd, send_buffer, send_length);
                if (n < 0) {
                    perror("ERROR writing to socket");
                    break;
                }
            }
            if (res == 1) { // after the last send we can close the conn
                close(client_sockfd);
            }

            receive_buffer_len = 0; // done processing this cmd
        }
    }

    if (viewer_config) {
        viewer_config->is_in_use = false;
    }
    free(client_args);
    // mark this thread as free to deal with another sockfd
    pthread_mutex_lock(&mutex);
    *nb_thread_used -= 1;
    pthread_mutex_unlock(&mutex);
    printf("[INFO] client %d disconnected\n", thread_id);
    return NULL;
}

int cli_addr_len = sizeof(struct sockaddr_in);
void *start(void *_) {
    int next_id = 0;
    int thread_id;
    int nb_thread_used = 0;
    pthread_t thread_client;
    pthread_mutex_init(&mutex, NULL); // Initialisation du mutex

    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, cli_addr;

    sockfd = config_socket(config.controller_port, "127.0.0.1", &serv_addr);
    printf("[INFO] server started! Listening to connections on port %d\n",
           config.controller_port);

    // Add infinite loop to keep server running and accept new connections
    while (true) {
        listen(sockfd, 5);
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr,
                           (unsigned int *)&cli_addr_len);
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
