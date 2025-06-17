#include "ft_ping.h"

void help() {
    printf("Usage\n");
    printf("ping [options] <destination>\n");
    printf("\n");
    printf("Options:\n");
    printf("  <destination>      dns name or ip address\n");
    printf("  -v                 verbose output\n");
    
}

void parse_args(int argc, char **argv, t_ping *ping) {
    int i = 1;

    if (argc < 2) {
        help();
        exit(EXIT_FAILURE);
    }

    while(i < argc) {
        if (strcmp(argv[i], "-v") == 0) {
            ping->verbose = 1;
        }
        else if (strcmp(argv[i], "-?") == 0 ||
            strcmp(argv[i], "-h") == 0 ||
            strcmp(argv[i], "--help") == 0
        ) {
            help();
            exit(EXIT_SUCCESS);
        }
        else if (argv[i][0] == '-') {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }
        else {
            ping->hostname = argv[i];
        }

        i++;
    }

    if (!ping->hostname) {
        help();
        exit(EXIT_FAILURE);
    }
}
