#ifndef _PARSE_VIEWERS_CONFIG_H_
#define _PARSE_VIEWERS_CONFIG_H_

struct viewers_config_t {
    int width;
    int height;
    struct viewer_config_t *viewers_configs;
    int viewers_count;
};

struct viewer_config_t {
    int id;
    int x;
    int y;
    int width;
    int height;
};

int viewers_config_from_file(struct viewers_config_t *config,
                             const char *filename);

void viewers_config_free_internals(struct viewers_config_t *config);

#endif /* _PARSE_VIEWERS_CONFIG_H_ */
