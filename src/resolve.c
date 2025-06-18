#include "ft_ping.h"

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