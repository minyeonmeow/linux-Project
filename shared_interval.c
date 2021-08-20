#include "shared_info.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        fatal("using \"%s <file 1> <file 2>\"", argv[0]);
    }

    list_shared_intervals(argv[1], argv[2]);

    return 0;
}