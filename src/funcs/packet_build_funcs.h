#ifndef PACK_BUILD
#define PACK_BUILD

#include <netinet/ip.h>
#define IP_DF 0x4000

#include <arpa/inet.h>
#include <linux/tcp.h>
#include <netinet/in.h>
#include "get_utils.h"
void build_syn_packet(char *dst_ip, unsigned short dst_port, char *buff);
#endif