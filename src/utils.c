#include "ft_ping.h"

/**
 * @brief Prints an error message using perror and exits the program with failure status.
 *
 * @param msg The message to display as a prefix to the system error.
 */
void exit_error(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

/**
 * @brief Computes the time difference between two time values in milliseconds.
 *
 * @param start The starting time (usually when a packet is sent).
 * @param end The ending time (usually when a packet is received).
 * @return The difference in milliseconds as a floating point value.
 */
double time_diff(struct timeval *start, struct timeval *end) {
    double sec = (double)(end->tv_sec - start->tv_sec);
    double usec = (double)(end->tv_usec - start->tv_usec);
    return sec * 1000.0 + usec / 1000.0;
}

/**
 * @brief Resolves a hostname or IPv4 string to a sockaddr_in structure.
 *
 * @param target The target hostname or IP address to resolve.
 * @param ping A pointer to the t_ping structure where the result will be stored.
 */
void resolve_host(const char *target, t_ping *ping) {
    struct addrinfo hints, *res;
    int ret;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;

    if ((ret = getaddrinfo(target, NULL, &hints, &res)) != 0) {
        fprintf(stderr, "ft_ping: %s: %s\n", target, gai_strerror(ret));
        exit(EXIT_FAILURE);
    }

    memcpy(&ping->addr, res->ai_addr, sizeof(struct sockaddr_in));
    inet_ntop(AF_INET, &ping->addr.sin_addr, ping->ip_str, sizeof(ping->ip_str));

    freeaddrinfo(res);
}
