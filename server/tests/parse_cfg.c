#include "parse_cfg.h"
#include "parse_viewers_config.h"
#include "utils.h"
#include <assert.h>
#include <stdio.h>

void test_config_parsing() {
    struct config_t cfg;
    assert(config_from_file(&cfg, "./controller.cfg") == 0);
}

struct viewers_config_t viewers_config;
struct config_t config;

int main(int argc, char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    test_config_parsing();

    return 0;
}
