#include "page_info.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fatal("using \"%s <filename>\"", argv[0]);
    }

    show_assigned_percentage(argv[1]);

    return 0;
}