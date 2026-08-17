#define _POSIX_C_SOURCE 199309L
#include "funcs/checksum_funcs.h"
#include "funcs/packet_build_funcs.h"
#include "time.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SOURCE_PORT 8080

double get_time(void) {
  struct timespec ts;

  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (double)(ts.tv_sec) + (double)(ts.tv_nsec / 1e9);
}

int main(int argc, char *argv[]) {

  char *dst_ip = argv[1];

  char buff[4096] = {0};

  unsigned short start_port = (unsigned short)strtol(argv[2], NULL, 10);
  unsigned short end_port = (unsigned short)strtol(argv[3], NULL, 10);

  // struct timespec ts = {0};
  // ts.tv_nsec = 100000000;

  int sock_fd = socket(PF_INET, SOCK_RAW | SOCK_NONBLOCK, IPPROTO_TCP);
  if (sock_fd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  struct sockaddr_in sin = {0};
  const struct sockaddr *sin_ptr = (const struct sockaddr *)&sin;
  struct iphdr *ip = (struct iphdr *)buff;
  struct pseudo_tcp pseudo = {0};
  struct tcphdr *tcp = (struct tcphdr *)(buff + sizeof(struct iphdr));

  int one = 1;
  int bs = 100 * 1024 * 1024; // big buffer
  setsockopt(sock_fd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(int));
  setsockopt(sock_fd, SOL_SOCKET, SO_RCVBUF, &bs, sizeof(bs));

  sin.sin_family = AF_INET;
  sin.sin_port = htons(start_port);
  sin.sin_addr.s_addr = inet_addr(dst_ip);

  unsigned char resp_buff[1024] = {0};
  socklen_t sinlen = sizeof(sin);

  struct pollfd fds[1];
  fds[0].fd = sock_fd;

  fds[0].events = POLLIN; // got data

  int current_port = start_port;
  double last = 0;
  double now = 0;
  struct sockaddr saddr_dst = {0};


  char is_printed[65535 + 1] = {0};

  while (1) {

    if (current_port <= end_port) {
      build_syn_packet(dst_ip, current_port, buff);
      sendto(sock_fd, buff, ntohs(ip->tot_len), 0, sin_ptr, sizeof(sin));
      last = get_time();
      current_port++;
    }

    int ret = poll(fds, 1, 1);
    if (ret > 0 && (fds[0].revents & POLLIN)) {
      while (1) {

        ssize_t data = recvfrom(sock_fd, resp_buff, 1024, 0,
                                (struct sockaddr *)&saddr_dst, &sinlen);
        if (data < 0) {
          if (errno == EWOULDBLOCK || errno == EAGAIN) {
            break;
          } else if (errno == EINTR) {
            continue;
          } else
            return 0;
          break;
        }

      
        struct iphdr *recv_ip = (struct iphdr *)resp_buff;
        struct tcphdr *recv_tcp =
            (struct tcphdr *)(resp_buff + recv_ip->ihl * 4);
        if (recv_tcp->dest == htons(SOURCE_PORT) &&
            recv_tcp->syn == 1 && // syn - synchronize, ack - succes got packet,
            recv_tcp->ack == 1 && recv_ip->saddr == ip->daddr &&
            recv_ip->protocol == IPPROTO_TCP

        ) { // their size is 1 bit
          if (is_printed[ntohs(recv_tcp->source)] != 1) {
            printf("port %d is open\n", ntohs(recv_tcp->source));
            is_printed[ntohs(recv_tcp->source)] = 1;
          }
        }

        // else printf("port %d is close\n", ntohs(recv_tcp->source));
        memset(resp_buff, 0, 1024);
        memset(&saddr_dst, 0, sizeof(saddr_dst));
      }
    }

    now = get_time();
    if (current_port > end_port &&
        (now - last) >
            5) { // wait for 5 second to exit if no packets more got recv
      break;
    }
  }

  return 0;
}
