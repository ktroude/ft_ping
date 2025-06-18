#include "ft_ping.h"

/**
 * @brief Prints an error message using perror and exits the program with failure status.
 *
 * @param msg The message to display as a prefix to the system error.
 */
void exit_error(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

/**
 * @brief Computes the time difference between two time values in milliseconds.
 *
 * @param start The starting time (usually when a packet is sent).
 * @param end The ending time (usually when a packet is received).
 * @return The difference in milliseconds as a floating point value.
 */
double time_diff(struct timeval *start, struct timeval *end) {
    double sec = (double)(end->tv_sec - start->tv_sec);
    double usec = (double)(end->tv_usec - start->tv_usec);
    return sec * 1000.0 + usec / 1000.0;
}


