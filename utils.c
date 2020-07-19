#include "utils.h"
#include <sys/socket.h>

int check_socket(int sockfd)
{
  int error_code;
  int error_len;
  getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error_code, &error_len);
  return error_code;
}

struct sockaddr_in configure_socket(uint32_t ip_addr, int port)
{
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = ip_addr;
  addr.sin_port = htons(port);
  return addr;
}
