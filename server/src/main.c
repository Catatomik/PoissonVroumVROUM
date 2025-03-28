#include "utils.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#include"connexion.h"
#include "parse_cfg.h"

int main(int argc, char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    pthread_t thread_connexion;

    //init config structur of controller configuration file
    struct config_t *config = malloc(sizeof(struct config_t));
    if (config_from_file(config, "./controller.cfg")){
      printf("hey");
      //const char *controller_addr = "127.0.0.1";
      //int controller_port = config.controller_port;    
      //int display_timeout_value = config.display_timeout_value;
      //int fish_update_interval = config.fish_update_interval;

      pthread_create(&thread_connexion, NULL, start, (void*) config);
      pthread_detach(thread_connexion);
      while(1){}
    }
    else{
      printf("Error while parsing config file");
      return -1;
    }
    
    
    

    printf("Hello, World!\n");
    return 0;
}
