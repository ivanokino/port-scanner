#ifndef ip_checksums
#define ip_checksums
#include <stdint.h>
#include <linux/tcp.h>
#include <netinet/ip.h>
struct __attribute__((packed)) pseudo_tcp {
  uint32_t source_addr;
  uint32_t dest_addr;
  uint8_t placeholder; // alwats zero
  uint8_t protocol;    // 6, tcp
  uint16_t lenght;     // tcp header+data in bytes
};

unsigned short calc_ip_checksum(struct iphdr *ip);

unsigned short calc_tcp_checksum(struct tcphdr *tcp, struct pseudo_tcp *pseudo, char *buff);
#endif