#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "utils.h"

void read_data(int sockfd)
{
  char buffer[100];
  do
  {
    memset(buffer, 0, sizeof(buffer));

    int result = read(sockfd, buffer, sizeof(buffer));
    if (result == 0)
    {
      break;
    }

    printf("%s\n", buffer);
  } while (1);
}

void *thread_func(void *arguments)
{
  int *sockfd = (int *)arguments;
  read_data(*sockfd);
}

int main(int argc, char **argv)
{
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1)
  {
    printf("Socket creation failed...\n");
    return 1;
  }

  struct sockaddr_in servaddr = configure_socket(inet_addr("127.0.0.1"), 9855);

  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
  {
    printf("Connection failed...\n");
    return 2;
  }
  else
  {
    printf("Connected successfully\n");
  }

  pthread_t thread;
  pthread_create(&thread, NULL, thread_func, &sockfd);

  char user_input[100];
  while (1)
  {
    // printf("Input text: ");
    int n = 0;
    while ((user_input[n++] = getchar()) != '\n')
    {
    }

    user_input[n - 1] = 0;

    if (n == 1)
      continue;
    write(sockfd, user_input, n);

    if (strcmp(user_input, "exit") == 0)
    {
      break;
    }

    if (check_socket(sockfd) != 0)
    {
      printf("Connection was closed...\n");
      return 3;
    }
  }

  close(sockfd);
  return 0;
}
