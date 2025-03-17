#include "parse_cfg.h"
#include <stdio.h>

void test_config_parsing() {
    struct config_t cfg;
    config_from_file(&cfg, "./controller.cfg");
    printf("config (controller_port=%d, display_timeout_value=%d, "
           "fish_update_interval=%d)\n",
           cfg.controller_port, cfg.display_timeout_value,
           cfg.fish_update_interval);
}

int main(int argc, char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    test_config_parsing();

    return 0;
}
