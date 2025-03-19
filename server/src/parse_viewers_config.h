#ifndef _PARSE_VIEWERS_CONFIG_H_
#define _PARSE_VIEWERS_CONFIG_H_

// config of an aquarium
struct viewers_config_t {
    int width;                               // width of the aquarium
    int height;                              // height of the aquarium
    struct viewer_config_t *viewers_configs; // array of configs (each viewer's)
    int viewers_count; // length of the viewers_configs array
};

// description of a viewer's config
struct viewer_config_t {
    int id;     // id of the viewer
    int x;      // x location of the viewer window in the aquarium
    int y;      // y location of the viewer window in the aquarium
    int width;  // width of the viewer window
    int height; // height of the viewer window
};

// parses a viewers config file at given path and assigns the parsed values to
// the config object
int viewers_config_from_file(struct viewers_config_t *config,
                             const char *viewers_config_path);

// free the internals of the viewers_config_t
void viewers_config_free_internals(struct viewers_config_t *config);

#endif /* _PARSE_VIEWERS_CONFIG_H_ */
