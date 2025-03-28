#include "parse_viewers_config.h"
#include <assert.h>

int main() {
    struct viewers_config_t invalid_config;
    assert(viewers_config_from_file(&invalid_config,
                                    "tests/assets/no_x_viewers.config"));
    viewers_config_free_internals(&invalid_config);

    struct viewers_config_t valid_config;
    assert(viewers_config_from_file(&valid_config,
                                    "tests/assets/valid_viewers.config") == 0);
    viewers_config_free_internals(&valid_config);
}
