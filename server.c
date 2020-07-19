#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "utils.h"

#define MAX_CLIENTS 20

uint8_t CLIENT_BUSY[MAX_CLIENTS] = {0};

int find_free_id()
{
  for (int i = 0; i < MAX_CLIENTS; i++)
  {
    if (CLIENT_BUSY[i] == 0)
      return i;
  }
  return -1;
}

void send_to_all(char *buffer, int len)
{
  for (int i = 0; i < MAX_CLIENTS; i++)
  {
    if (CLIENT_BUSY[i] != 0)
    {
      write(CLIENT_BUSY[i], buffer, len);
    }
  }
}

typedef struct client_info
{
  int sockfd;
  int id;
} client_info_t;

void process_client(int sockfd, int client_id)
{
  printf("Client %02d connected\n", client_id);

  char buffer[100];
  do
  {
    memset(buffer, 0, sizeof(buffer));

    int result = read(sockfd, buffer, sizeof(buffer));
    if (result == 0)
    {
      break;
    }

    printf("From client %02d: %s\n", client_id, buffer);
    send_to_all(buffer, result);
  } while (strcmp(buffer, "exit") != 0);

  close(sockfd);

  CLIENT_BUSY[client_id] = 0;
  printf("Client %02d disconnected\n", client_id);
}

void *thread_func(void *arguments)
{
  client_info_t *info = (client_info_t *)arguments;
  process_client(info->sockfd, info->id);
}

int main(int argc, char **argv)
{
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1)
  {
    printf("Socket creation failed...\n");
    return 1;
  }

  struct sockaddr_in servaddr = configure_socket(htonl(INADDR_ANY), 9855);

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
  if (listen(sockfd, MAX_CLIENTS + 1) != 0)
  {
    printf("Listen failed...\n");
    close(sockfd);
    return 3;
  }
  else
  {
    printf("Server listening...\n");
  }

  pthread_t threads[MAX_CLIENTS];
  while (1)
  {
    // ждем клиента
    struct sockaddr_in cliaddr;
    int clilen;
    int connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
    if (connfd <= 0)
    {
      printf("Server accept failed...\n");
      close(sockfd);
      return 4;
    }

    int free_id = find_free_id();
    if (free_id == -1)
    {
      printf("All client workers are busy\n");
      write(connfd, "Server busy. Try again later", 29);
      close(connfd);
      continue;
    }

    client_info_t info;
    info.sockfd = connfd;
    info.id = free_id;
    CLIENT_BUSY[free_id] = connfd;
    pthread_create(&threads[info.id], NULL, thread_func, &info);
  }

  close(sockfd);
  return 0;
}
