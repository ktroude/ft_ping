#ifndef FT_PING_H
#define FT_PING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>

#define PACKET_SIZE 64

extern int g_running;

/**
 * @brief Stores all runtime state and metrics for the ft_ping program.
 */
typedef struct s_ping {
    int sockfd;                        /**< Socket file descriptor used to send/receive ICMP packets */
    struct sockaddr_in addr;           /**< Destination IPv4 address in sockaddr format */
    char ip_str[INET_ADDRSTRLEN];      /**< Human-readable IP address as a string */
    char *hostname;                    /**< Hostname provided by the user (e.g. "google.com") */
    pid_t pid;                         /**< Process ID, used to identify ICMP requests */
    int verbose;                       /**< Verbose mode flag, set by -v option */

    int sent;                          /**< Number of ICMP Echo Requests sent */
    int received;                      /**< Number of ICMP Echo Replies received */

    struct timeval send_time;          /**< Timestamp when the last packet was sent */
    struct timeval start_time;         /**< Timestamp when the ping session started */

    double rtt_min;                    /**< Minimum round-trip time observed (ms) */
    double rtt_max;                    /**< Maximum round-trip time observed (ms) */
    double rtt_total;                  /**< Sum of all round-trip times, for average calculation */
    int    rtt_count;                  /**< Number of RTTs measured (i.e., responses received) */
} t_ping;

void        parse_args(int argc, char **argv, t_ping *ping);
void        resolve_host(const char *target, t_ping *ping);
void        exit_error(const char *msg);
uint16_t    checksum(void *data, int len);
void        send_ping(t_ping *ping, int seq);
void        handle_icmp_reply(t_ping *ping, struct icmp *icmp, int bytes, int ip_hdr_len, struct ip *ip_hdr, struct timeval *recv_time);
void        build_icmp_packet(char *packet, t_ping *ping, int seq);
void        receive_ping(t_ping *ping);
double      time_diff(struct timeval *start, struct timeval *end);
void        int_handler(int sig);
void        update_rtt_stats(t_ping *ping, double rtt);
void        print_statistics(t_ping *ping);
void        setup_socket(t_ping *ping);

#endif
