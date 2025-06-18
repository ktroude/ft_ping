#include "ft_ping.h"

/**
 * @brief Prints the final summary after pinging is done.
 *
 * @param ping Pointer to structure containing stats.
 */
void print_statistics(t_ping *ping) {
    printf("\n--- %s ping statistics ---\n", ping->hostname);
    printf("%d packets transmitted, %d received, %.0f%% packet loss\n",
           ping->sent,
           ping->received,
           ping->sent > 0 ? 100.0 * (ping->sent - ping->received) / ping->sent : 0.0);
    if (ping->rtt_count > 0) {
        double avg = ping->rtt_total / ping->rtt_count;
        printf("rtt min/avg/max = %.3f/%.3f/%.3f ms\n",
               ping->rtt_min, avg, ping->rtt_max);
    }
}

/**
 * @brief Updates the round-trip time statistics.
 *
 * @param ping Pointer to the main structure holding stats.
 * @param rtt Last measured round-trip time.
 */
void update_rtt_stats(t_ping *ping, double rtt) {
    ping->received++;
    ping->rtt_total += rtt;
    ping->rtt_count++;

    if (ping->rtt_min == 0 || rtt < ping->rtt_min)
        ping->rtt_min = rtt;
    if (rtt > ping->rtt_max)
        ping->rtt_max = rtt;
}
