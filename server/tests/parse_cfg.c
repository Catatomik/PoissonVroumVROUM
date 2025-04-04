#include "parse_cfg.h"
#include "utils.h"
#include <assert.h>
#include <stdio.h>

void test_config_parsing() {
    struct config_t cfg;
    assert(config_from_file(&cfg, "./controller.cfg") == 0);
}

int main(int argc, char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    test_config_parsing();

    return 0;
}
