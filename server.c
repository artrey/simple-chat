#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void process_client(int sockfd)
{
  char buffer[100];
  do
  {
    memset(buffer, 0, sizeof(buffer));

    read(sockfd, buffer, sizeof(buffer));

    printf("From client: %s\n", buffer);
  } while (strcmp(buffer, "exit") != 0);

  close(sockfd);
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
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(9855);

  // пытаемся занять порт (квартиру в доме)
  if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
  {
    printf("Bind failed...\n");
    return 2;
  }
  else
  {
    printf("Socket successfully binded!\n");
  }

  // пытаемся настроиться на работу с клиентами.
  // второй параметр - сколько клиентов мжет быть одновременно
  if (listen(sockfd, 1) != 0)
  {
    printf("Listen failed...\n");
    return 3;
  }
  else
  {
    printf("Server listening...\n");
  }

  while (1)
  {
    // ждем клиента
    struct sockaddr_in cliaddr;
    int clilen;
    int connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
    if (connfd <= 0)
    {
      printf("Server accept failed...\n");
      return 4;
    }

    process_client(connfd);
  }

  close(sockfd);
  return 0;
}
