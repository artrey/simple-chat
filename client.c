#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int check_socket(int sockfd)
{
  int error_code;
  int error_len;
  getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error_code, &error_len);
  return error_code;
}

int main(int argc, char **argv)
{
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1)
  {
    printf("Socket creation failed...\n");
    return 1;
  }

  struct sockaddr_in servaddr;
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(9855);

  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
  {
    printf("Connection failed...\n");
    return 2;
  }
  else
  {
    printf("Connected successfully\n");
  }

  char user_input[100];
  while (1)
  {
    printf("Input text: ");
    int n = 0;
    while ((user_input[n++] = getchar()) != '\n')
    {
    }

    user_input[n - 1] = 0;
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
