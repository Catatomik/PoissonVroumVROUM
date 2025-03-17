#ifndef _PARSE_CFG_H_
#define _PARSE_CFG_H_

struct config_t {
    int controller_port;
    int display_timeout_value;
    int fish_update_interval;
};

int config_from_file(struct config_t *config, const char *cfgpath);

#endif /* _PARSE_CFG_H_ */
