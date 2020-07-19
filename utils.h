#ifndef SIMPLE_CHAT_UTILS_H
#define SIMPLE_CHAT_UTILS_H

#include <netinet/in.h>

int check_socket(int sockfd);

struct sockaddr_in configure_socket(uint32_t ip_addr, int port);

#endif
