#include "packet_build_funcs.h"
#include "checksum_funcs.h"

void build_syn_packet(char *dst_ip, unsigned short dst_port, char *buff) {

  char local_ip[INET_ADDRSTRLEN] = {0};
  get_local_ip(dst_ip, local_ip);

  struct pseudo_tcp pseudo = {0};
  struct iphdr *ip = (struct iphdr *)buff;
  struct tcphdr *tcp = (struct tcphdr *)(buff + sizeof(struct iphdr));

  ip->ihl = 5;     // ip head length dont need htons cuz 1 bite
  ip->version = 4; // dont need htons cuz 1 bites
  ip->tos = 16;    // low delay
  ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct tcphdr));
  ip->id = 0;                  // dont need cuz only 1 pockets
  ip->frag_off = htons(IP_DF); // flags + frag offset
  ip->ttl = 64;                // count of routers what he can jump from
  ip->protocol = IPPROTO_TCP;
  ip->check = 0;
  inet_pton(AF_INET, local_ip, &ip->saddr);
  inet_pton(AF_INET, dst_ip, &ip->daddr);

  // PSEUDO TCP
  inet_pton(AF_INET, local_ip, &pseudo.source_addr);
  inet_pton(AF_INET, dst_ip, &pseudo.dest_addr);

  ip->check = calc_ip_checksum(ip);

  pseudo.protocol = 6;
  pseudo.lenght = htons(sizeof(struct tcphdr));
  pseudo.placeholder = 0;

  tcp->source = htons((unsigned short)8080);
  tcp->dest = htons((unsigned short)dst_port);
  tcp->seq = htonl((unsigned int)2374); // rand
  tcp->ack_seq = 0;
  tcp->doff = 5; // len /4, count of 32bit segs
  tcp->res1 = 0;
  tcp->syn = 1; // TH_SYN
  tcp->window = htons(2000);
  tcp->check = 0;

  tcp->urg_ptr = 0; // pointer on a urgent data

  tcp->check = calc_tcp_checksum(tcp, (struct pseudo_tcp *)&pseudo, buff);
}