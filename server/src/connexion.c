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
  serv_addr->sin_addr.s_addr = INADDR_ANY;//inet_addr(ip_addr);
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
  
  struct sockaddr_in serv_addr,cli_addr;
  int clilen;
  int sockfd =  config_socket(new_port, "127.0.0.1", &serv_addr);
  char buffer_reader[256];
  char buffer_change_port[256];
  int n;

  // Ask to client to connect to his port (manually)
  n = sprintf(buffer_change_port,"Please reconnect : your port is now %d",new_port);
  if (n < 0) error("ERROR formating string");
  n = write(client_sockfd, buffer_change_port, n);
  if (n < 0) error("ERROR writing to socket");
  close(client_sockfd);

  // Open listener on the client assigned port
  listen(sockfd,5);
  clilen = sizeof(cli_addr);
  client_sockfd = accept(sockfd, 
		     (struct sockaddr *) &cli_addr, 
		     &clilen);
   if (client_sockfd < 0) 
      error("ERROR on accept");

  // Handles socket exchange
  bzero(buffer_reader,256);
  
  n = read(client_sockfd, buffer_reader, 255);
  if (n < 0) error("ERROR reading from socket");
  
  printf("The message from client %d: %s\n",client_sockfd, buffer_reader);
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
  int n, test_port, test_sock;
  int next_port = controller_port;
  int port_found = 0;

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

    // ISSUE : 12345+i is port for next connection but don't check if it's free
    // ISSUE : 12345+i impossible to use again a old port closed
    /*next_port = 12345;
    port_found = 0;
    
    while(port_found == 0)
      {
	next_port += 1;
	test_sock = config_socket(next_port, controller_addr, &test_addr);
	if (bind(test_sock, (struct sockaddr *) &test_addr, sizeof(test_addr)) > 0){
	  close(test_sock);
	  printf("test_sock :%d\n", test_sock);
	  printf("test_port :%d\n", next_port);
	}
	else{
	  port_found = 1;
	  close(test_sock);
	}
	sleep(1);
	}*/

    next_port += 1;
    // structure create for good argument in thread function
    threads_client_args_t *args = malloc(sizeof(threads_client_args_t));
    args->client_sockfd = newsockfd;
    args->client_port = next_port;
    
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
