#include <signal.h>
#include "ft_ping.h"

int main(int argc, char **argv) {
    t_ping ping;
    memset(&ping, 0, sizeof(t_ping));
    ping.pid = getpid();

    parse_args(argc, argv, &ping);
    resolve_host(ping.hostname, &ping);

    printf("PING %s (%s) %d bytes of data.\n", ping.hostname, ping.ip_str, PACKET_SIZE);

    ping.sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (ping.sockfd < 0) {
        perror("socket");
        exit(1);
    }

    signal(SIGINT, int_handler);
    gettimeofday(&ping.start_time, NULL);

    int seq = 1;
    while (g_running) {
        send_ping(&ping, seq);
        receive_ping(&ping);
        seq++;
        sleep(1);
    }

    print_statistics(&ping);
    return 0;
}
