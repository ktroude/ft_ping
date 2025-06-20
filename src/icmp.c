#include "ft_ping.h"

/*
** ICMP Echo Request Packet Structure (Type 8):
**
** An ICMP Echo Request packet consists of the following fields:
**
**   - icmp_type   : Should be set to ICMP_ECHO (value 8)
**   - icmp_code   : Always 0 for Echo Request
**   - icmp_id     : Identifier used to match requests and replies (commonly the process PID)
**   - icmp_seq    : Sequence number, usually incremented with each request
**   - icmp_cksum  : 16-bit checksum covering the entire ICMP message
**   - icmp_data   : Optional payload data (can be arbitrary, e.g. ASCII or pattern bytes)
**
** This structure is used to send a "ping" to a destination IP address.
** The corresponding Echo Reply (type 0) should return the same `id`, `seq`, and `data` values.
*/

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
