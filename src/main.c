#include "ft_ping.h"

int main(int argc, char** argv) {
    t_ping ping;
    memset(&ping, 0, sizeof(ping));
    ping.pid = getpid();
    

    parse_args(argc, argv, &ping);
    resolve_host(ping.hostname, &ping);

    printf("PING %s (%s) %d bytes of data.\n", ping.hostname, ping.ip_str, PACKET_SIZE);

    ping.sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (ping.sockfd < 0) {
        perror("socket");
        exit(1);
    }

    send_ping(&ping);
}

