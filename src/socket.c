#include "ft_ping.h"

/**
 * @brief Initializes the raw socket for sending and receiving ICMP packets.
 *
 * Creates a socket using `AF_INET`, `SOCK_RAW`, and `IPPROTO_ICMP`, which allows the
 * program to send ICMP Echo Requests (type 8) and receive Echo Replies (type 0).
 *
 * @param ping Pointer to the main ping context (t_ping struct).
 *
 * @note Exits the program using `exit_error()` if socket creation or option setup fails.
 */
void setup_socket(t_ping *ping) {
    ping->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (ping->sockfd < 0)
        exit_error("socket");

    struct timeval timeout = {1, 0};
    if (setsockopt(ping->sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
        exit_error("setsockopt");
}
