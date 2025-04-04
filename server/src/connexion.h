#ifndef __CONNEXION_H__
#define __CONNEXION_H__

#include "main.h"
#include "parse_cfg.h"
#include "parse_viewers_config.h"

void error(char *msg);

void *start(void *config);

#endif //__CONNEXION_H__
