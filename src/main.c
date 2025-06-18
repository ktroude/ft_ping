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
static void setup_socket(t_ping *ping) {
    ping->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (ping->sockfd < 0)
        exit_error("socket");

    struct timeval timeout = {1, 0};
    if (setsockopt(ping->sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
        exit_error("setsockopt");
}

/**
 * @brief Starts the main ping loop, sending and receiving ICMP packets continuously.
 *
 * Sends one ICMP Echo Request per second and waits for the corresponding Echo Reply.
 * If a SIGINT is received (e.g. Ctrl+C), the loop exits cleanly and statistics are printed.
 *
 * @param ping Pointer to the ping context, containing socket info and statistics.
 *
 * @note Uses global `g_running` as the loop condition, modified by the signal handler.
 */
static void run_ping_loop(t_ping *ping) {
    int seq = 1;

    while (g_running) {
        send_ping(ping, seq);
        receive_ping(ping);
        seq++;
        sleep(1);
    }
}

/**
 * @brief Entry point of the ft_ping program.
 *
 * Performs all initializations:
 * - Parses command-line arguments
 * - Resolves the target host to an IPv4 address
 * - Prints the initial PING header line
 * - Sets up the raw socket and signal handler
 * - Starts the ping loop
 * - Prints final statistics on exit
 *
 * @param argc Argument count.
 * @param argv Argument vector (e.g. command-line arguments).
 * @return Exit status (0 on success).
 */
int main(int argc, char **argv) {
    t_ping ping;
    memset(&ping, 0, sizeof(t_ping));
    ping.pid = getpid();

    parse_args(argc, argv, &ping);
    resolve_host(ping.hostname, &ping);

    printf("PING %s (%s) %d bytes of data.\n", ping.hostname, ping.ip_str, PACKET_SIZE);

    setup_socket(&ping);

    signal(SIGINT, int_handler);
    gettimeofday(&ping.start_time, NULL);

    run_ping_loop(&ping);
    print_statistics(&ping);

    return 0;
}
