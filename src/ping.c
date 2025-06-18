#include "ft_ping.h"

/**
 * @brief Global flag used to control the ping loop.
 *        Set to 0 on SIGINT to gracefully exit.
 */
int g_running = 1;

/**
 * @brief Constructs an ICMP Echo Request packet with predefined payload.
 *
 * @param packet Pointer to the buffer where the packet will be built.
 * @param ping Pointer to the t_ping structure containing process ID.
 * @param seq ICMP sequence number for the current request.
 */
void build_icmp_packet(char *packet, t_ping *ping, int seq) {
    struct icmp *icmp = (struct icmp *)packet;

    memset(packet, 0, PACKET_SIZE);
    memset(icmp->icmp_data, 42, 56);

    icmp->icmp_type = ICMP_ECHO;
    icmp->icmp_code = 0;
    icmp->icmp_id = htons(ping->pid);
    icmp->icmp_seq = htons(seq);

    icmp->icmp_cksum = 0;
    icmp->icmp_cksum = checksum(packet, 64);
}

/**
 * @brief Sends an ICMP Echo Request to the target IP address.
 *
 * @param ping Pointer to the t_ping structure with destination info.
 * @param seq Sequence number of the current ping request.
 */
void send_ping(t_ping *ping, int seq) {
    char packet[PACKET_SIZE];
    build_icmp_packet(packet, ping, seq);

    gettimeofday(&ping->send_time, NULL);

    ssize_t sent = sendto(ping->sockfd, packet, PACKET_SIZE, 0,
                          (struct sockaddr *)&ping->addr, sizeof(ping->addr));
    if (sent < 0)
        perror("sendto");
    else {
        ping->sent++;
        printf("ICMP Echo Request sent to %s\n", ping->ip_str);
    }
}

/**
 * @brief Calculates the ICMP checksum using the standard one's complement algorithm.
 *
 * @param data Pointer to the packet data.
 * @param len Length of the data to checksum.
 * @return 16-bit checksum in network byte order.
 */
uint16_t checksum(void *data, int len) {
    uint32_t sum = 0;
    uint16_t *ptr = data;

    while (len > 1) {
        sum += *ptr++;
        len -= 2;
    }

    if (len == 1) {
        uint16_t leftover = 0;
        *(uint8_t *)&leftover = *(uint8_t *)ptr;
        sum += leftover;
    }

    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    return ~sum;
}

/**
 * @brief Receives an ICMP packet and delegates response handling.
 *
 * @param ping Pointer to the main t_ping structure.
 */
void receive_ping(t_ping *ping) {
    char buffer[1024];
    struct sockaddr_in from;
    socklen_t addr_len = sizeof(from);
    struct timeval recv_time;

    ssize_t bytes = recvfrom(ping->sockfd, buffer, sizeof(buffer), 0,
                             (struct sockaddr *)&from, &addr_len);

    if (bytes < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            if (ping->verbose)
                printf("Request timeout for icmp_seq=%d\n", ping->rtt_count + 1);
        } else {
            fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
        }
        return;
    }

    gettimeofday(&recv_time, NULL);

    struct ip *ip_hdr = (struct ip *)buffer;
    int ip_hdr_len = ip_hdr->ip_hl << 2;
    struct icmp *icmp = (struct icmp *)(buffer + ip_hdr_len);

    handle_icmp_reply(ping, icmp, bytes, ip_hdr_len, ip_hdr, &recv_time);
}

/**
 * @brief Parses and validates the received ICMP response.
 *        Updates stats and prints response if valid.
 *
 * @param ping Pointer to main ping data.
 * @param icmp Pointer to the ICMP header from the received packet.
 * @param bytes Total bytes received.
 * @param ip_hdr_len Length of the IP header in the received packet.
 * @param ip_hdr Pointer to the IP header.
 * @param recv_time Timestamp when the packet was received.
 */
void handle_icmp_reply(t_ping *ping, struct icmp *icmp, int bytes, int ip_hdr_len, struct ip *ip_hdr, struct timeval *recv_time) {
    if (icmp->icmp_type == ICMP_ECHOREPLY &&
        ntohs(icmp->icmp_id) == ping->pid) {

        double rtt = time_diff(&ping->send_time, recv_time);
        update_rtt_stats(ping, rtt);

        printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.1f ms\n",
               bytes - ip_hdr_len,
               ping->ip_str,
               ntohs(icmp->icmp_seq),
               ip_hdr->ip_ttl,
               rtt);
    }
    else if (ping->verbose) {
        printf("Received ICMP type %d code %d (not echo reply)\n",
               icmp->icmp_type,
               icmp->icmp_code);
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

/**
 * @brief Signal handler for SIGINT (Ctrl+C).
 *        Stops the main ping loop gracefully.
 *
 * @param sig Unused.
 */
void int_handler(int sig) {
    (void)sig;
    g_running = 0;
}

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
