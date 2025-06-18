#include "ft_ping.h"

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
