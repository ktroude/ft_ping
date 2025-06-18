# ft_ping – A Network Diagnostic Utility in C

## Overview

`ft_ping` is a low-level reimplementation of the classic `ping` utility, written entirely in C. This tool allows users to test the reachability of a host (IPv4 address or hostname) and measure the round-trip time (RTT) of ICMP Echo Requests.

The project strictly adheres to the behavior of the original `ping` as implemented in `inetutils-2.0`, with specific constraints prohibiting the use of the system `ping` or copying any existing source code.

## Features

- ICMP Echo Request/Reply packet construction and checksum calculation  
- Hostname and IP resolution (IPv4 only) using `getaddrinfo()`  
- Statistics display on termination (packet loss, RTT min/avg/max)  
- Error handling for timeouts and unreachable hosts  
- Verbose mode (-v) for debugging and diagnostics  
- Help/usage option (-?)

## Technologies and Concepts

- Language: C (using the standard C library)
- Protocols: IPv4, ICMP
- System Calls: `socket()`, `sendto()`, `recvfrom()`, `setsockopt()`, `gettimeofday()`
- Signal Handling: Clean shutdown with `SIGINT`
- DNS Resolution: via `getaddrinfo()` (without relying on reverse DNS in response)
- Makefile: Builds only what's necessary

## Project Structure

ft_ping/
├── include/
│ └── ft_ping.h # Global declarations, struct definitions, function prototypes
├── src/
│ ├── main.c    # Main entry point: initialization, signal setup, main loop
│ ├── socket.c  # Raw socket setup (setup_socket function)
│ ├── ping.c    # Sending and receiving packets, response processing
│ ├── icmp.c    # ICMP packet construction and checksum calculation
│ ├── resolve.c # Hostname and IPv4 address resolution
│ ├── args.c    # Command-line argument parsing and usage/help display
│ ├── stats.c   # RTT statistics tracking and final summary display
│ └── utils.c   # Generic utilities
├── Makefile    # Build rules for compiling the project
└── README.md

## Usage

```bash
sudo ./ft_ping [options] <destination>
```

### Example

```bash
sudo ./ft_ping -v google.com
```

### Options

- `-v` : Enable verbose output (e.g., timeouts, ICMP error codes)
- `-?`, `--help`, `-h` : Display help message

## Installation and Build

### Prerequisites

- A Linux VM (Debian ≥ 7.0 recommended)
- GCC or Clang
- make

### Build the Project

```bash
make
```

### Run

Note: ICMP requires root privileges, so use `sudo`.
```bash
sudo ./ft_ping 8.8.8.8
```

## Output Example

```bash
sudo ./ft_ping google.com
PING google.com (172.217.21.14) 64 bytes of data.
ICMP Echo Request sent to 172.217.21.14
64 bytes from 172.217.21.14: icmp_seq=1 ttl=255 time=3.2 ms
ICMP Echo Request sent to 172.217.21.14
64 bytes from 172.217.21.14: icmp_seq=2 ttl=255 time=3.3 ms
ICMP Echo Request sent to 172.217.21.14
64 bytes from 172.217.21.14: icmp_seq=3 ttl=255 time=3.4 ms
^C
--- google.com ping statistics ---
3 packets transmitted, 3 received, 0% packet loss
rtt min/avg/max = 3.170/3.291/3.364 ms
```

## Restrictions

- No use of the system `ping` binary or its source code
- No third-party libraries
- Only standard C library functions are allowed

## Notes

- The program mimics `ping` from `inetutils-2.0`
- A delay of ±30 ms is tolerated when matching RTT
- Reverse DNS lookup is not required for ICMP replies

## Author

This project was developed as part of a networking module and demonstrates low-level network programming skills and raw socket manipulation.
