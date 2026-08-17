
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>

int get_local_ip(char *dst_ip, char *buff) {

  int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  struct sockaddr_in sin;
  inet_pton(AF_INET, dst_ip, &sin.sin_addr);
  uint32_t ip_int = ntohl(sin.sin_addr.s_addr);
  sin.sin_family = AF_INET;
  sin.sin_port = htons(8080); // rand port
  socklen_t sin_len = sizeof(sin);
  int res = connect(fd, (struct sockaddr *)&sin, sin_len);
  if (res < 0) {
    close(fd);
    return -1;
  };

  getsockname(fd, (struct sockaddr *)&sin, &sin_len);

  inet_ntop(AF_INET, &sin.sin_addr, buff, INET_ADDRSTRLEN);
  close(fd);
  return 0;
}