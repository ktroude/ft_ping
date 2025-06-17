#include "ft_ping.h"

int g_running = 1;

uint16_t checksum(void *data, int len) {
    uint32_t sum = 0;
    uint16_t *ptr = data;

    while(len > 1) {
        sum += *ptr++;
        len -=2;
    }

    if (len == 1) {
        uint16_t leftover =  0;
        *(uint8_t *)&leftover = *(u_int8_t *)ptr;

        sum += leftover;
    }

    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return ~sum;
}

void send_ping(t_ping *ping, int seq) {
    char packet[PACKET_SIZE];
    struct icmp *icmp_header = (struct icmp *)packet;

    memset(packet, 0, PACKET_SIZE);
    memset(icmp_header->icmp_data, 42, 56);

    icmp_header->icmp_type = ICMP_ECHO;
    icmp_header->icmp_code = 0;
    icmp_header->icmp_id = htons(ping->pid);
    icmp_header->icmp_seq = htons(seq);
    strcpy((char *)icmp_header->icmp_data, "Hello from ft_ping !!");

    icmp_header->icmp_cksum = 0;
    icmp_header->icmp_cksum = checksum(packet, PACKET_SIZE);

    gettimeofday(&ping->send_time, NULL);

    ssize_t sent = sendto(ping->sockfd, packet, PACKET_SIZE, 0, (struct sockaddr *)&ping->addr, sizeof(ping->addr));

    if (sent < 0) {
        perror("sendto");
    }
    else {
        ping->sent++;
        printf("ICMP Echo Request sent to %s\n", ping->ip_str);
    }
}

double time_diff(struct timeval *start, struct timeval *end) {
    double sec = (double)(end->tv_sec - start->tv_sec);
    double usec = (double)(end->tv_usec - start->tv_usec);
    return sec * 1000.0 + usec / 1000.0;
}

void receive_ping(t_ping *ping) {
    char buffer[1024];
    struct sockaddr_in from;
    socklen_t addr_len = sizeof(from);
    struct timeval recv_time;

    ssize_t bytes = recvfrom(ping->sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&from, &addr_len);

    if (bytes < 0) {
        perror("recvfrom");
        return;
    }

    gettimeofday(&recv_time, NULL);

    struct ip* ip_hdr = (struct ip *)buffer;
    int ip_hdr_len = ip_hdr->ip_hl << 2;

    struct icmp* icmp_hdr = (struct icmp *)(buffer + ip_hdr_len);

    if (icmp_hdr->icmp_type == ICMP_ECHOREPLY && ntohs(icmp_hdr->icmp_id) == ping->pid) {
        double rtt = time_diff(&ping->send_time, &recv_time);

        ping->received++;
        ping->rtt_total += rtt;
        ping->rtt_count++;

        if (ping->rtt_min == 0 || rtt < ping->rtt_min) {
            ping->rtt_min = rtt;
        }
        if (rtt > ping->rtt_max) {
            ping->rtt_max = rtt;
        }


        printf("%zd bytes from %s: icmp_seq=%d ttl=%d time=%.1f ms\n",
            bytes - ip_hdr_len,
            ping->ip_str,
            ntohs(icmp_hdr->icmp_seq),
            ip_hdr->ip_ttl,
            rtt
        );
    }
    else if (ping->verbose) {
        printf("Received ICMP type %d code %d (not echo reply)\n",
               icmp_hdr->icmp_type,
               icmp_hdr->icmp_code
        );
    }
}

void int_handler(int sig) {
    (void)sig;
    g_running = 0;
}

void print_statistics(t_ping *ping) {
    printf("\n--- %s ping statistics ---\n", ping->hostname);
    printf("%d packets transmitted, %d received, %.0f%% packet loss\n",
           ping->sent,
           ping->received,
           ping->sent > 0 ? 100.0 * (ping->sent - ping->received) / ping->sent : 0.0
    );
    if (ping->rtt_count > 0) {
        double avg = ping->rtt_total / ping->rtt_count;
        printf("rtt min/avg/max = %.3f/%.3f/%.3f ms\n",
               ping->rtt_min, avg, ping->rtt_max);
    }
}
