

#include <netinet/ip.h>

#include "checksum_funcs.h"

unsigned short calc_ip_checksum(struct iphdr *ip) {

  int ihl_len = ip->ihl * 4;
  int sum = 0;
  unsigned short *ptr = (unsigned short *)ip;
  while (ihl_len > 1) {
    sum += *ptr;
    ptr++;
    ihl_len -= 2;
  }
  // ipv4 len is always /4
  while ((sum >> 16) != 0) {
    sum = (sum & 0xffff) + (sum >> 16);
  }

  return (unsigned short)~sum;
}

unsigned short calc_tcp_checksum(struct tcphdr *tcp, struct pseudo_tcp *pseudo,
                                 char *buff) {

  u_int16_t *ptr = (u_int16_t *)pseudo;
  int all_len = tcp->doff * 4;
  int sum = 0;

  for (int i = sizeof(struct pseudo_tcp); i > 1; i -= 2) {
    sum += *ptr;
    ptr++;
  }
  ptr = (unsigned short *)(buff + sizeof(struct iphdr));

  while (all_len > 1) {
    sum += *ptr;
    ptr++;
    all_len -= 2;
  }

  if (all_len == 1) { // data is uneven
    uint8_t *last_byte = (uint8_t *)ptr;
    sum += (uint16_t)(*last_byte);
  }
  while ((sum >> 16) != 0) {
    sum = (sum & 0xffff) + (sum >> 16);
  }

  return (unsigned short)~sum;
}