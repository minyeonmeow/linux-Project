#include "interval_info.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fatal("using \"%s <filename>\"", argv[0]);
    }

    list_virtual_intervals(argv[1]);

    return 0;
}