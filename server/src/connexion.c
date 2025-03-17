/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <strings.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct {
  int client_sockfd;
  int client_port;
} threads_client_args_t;

void error(char *msg)
{
    perror(msg);
    exit(1);
}
int config_socket(int portno, const char *ip_addr,struct sockaddr_in *serv_addr ){
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  
  if (sockfd < 0) 
    error("ERROR opening socket");
  
  bzero((char *) serv_addr, sizeof(serv_addr));
  serv_addr->sin_family = AF_INET;
  serv_addr->sin_addr.s_addr = inet_addr(ip_addr);
  serv_addr->sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr *) serv_addr,
	   sizeof(*serv_addr)) < 0) 
    error("ERROR on binding");
  return sockfd;
}

void *thread_function_client(void *args)
{
  threads_client_args_t *client_args = (threads_client_args_t *)args;
  int client_sockfd = client_args->client_sockfd;
  int new_port = client_args->client_port;
  
  char buffer[256];
  int n;

  // Handles socket exchange
  bzero(buffer,256);
  
  n = read(client_sockfd, buffer, 255);
  if (n < 0) error("ERROR reading from socket");


  //
  printf("The message from client %d: %s\n",client_sockfd, buffer);
  n = write(client_sockfd,"I got your message",18);
  if (n < 0) error("ERROR writing to socket");
  close(client_sockfd);

  
  return NULL;
}

int main(int argc, char *argv[])
{
  const char *controller_addr = "127.0.0.1";
  int controller_port = 12345;
  int display_timeout_value = 45;
  int fish_update_interval = 1;

  pthread_t thread_client;
  
  int sockfd, newsockfd, portno, clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr, test_addr;
  int n;

  sockfd = config_socket(controller_port, controller_addr, &serv_addr);

  // Add infinite loop to keep server running and accept new connections
  while(1){
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, 
		       (struct sockaddr *) &cli_addr, 
		       &clilen);
    if (newsockfd < 0) 
      error("ERROR on accept");

 
    // structure create for good argument in thread function
    threads_client_args_t *args = malloc(sizeof(threads_client_args_t));
    args->client_sockfd = newsockfd;
    args->client_port = controller_port;
    
    if (pthread_create(&thread_client, NULL, thread_function_client, (void *)args))
      {
        fprintf(stderr, "ERROR creation client thread\n");
	close(newsockfd);
	free(args);
	continue;
      }

    // not pthread_join because we don't wait the end of thread (parallize)
    pthread_detach(thread_client);
   
  }
}
