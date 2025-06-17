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

typedef struct s_ping {
    int sockfd;
    struct sockaddr_in addr;
    char ip_str[INET_ADDRSTRLEN];
    char *hostname;
    pid_t pid;
    int verbose;
    struct timeval send_time;
} t_ping;

void        parse_args(int argc, char **argv, t_ping *ping);
void        resolve_host(const char *target, t_ping *ping);
void        exit_error(const char *msg);
uint16_t    checksum(void *data, int len);
void        send_ping(t_ping *ping);
void        receive_ping(t_ping *ping);
double      time_diff(struct timeval *start, struct timeval *end);

#endif
