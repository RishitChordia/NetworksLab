#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

struct sockaddr_in server, server_addr;
struct sockaddr_in client_addr;
struct hostent *host_name;
char client_string[4096];
int socket_fd, socket_fd, length, temp;

void get_messages()
{
    while (1)
    {
        bzero(client_string, 4096);
        fgets(client_string, 4095, stdin);
        client_string[strlen(client_string) - 1] = 0;
        char *domain;
        strcpy(domain, client_string);

        int length = sizeof(struct sockaddr_in);

        sendto(socket_fd, client_string, strlen(client_string), 0, &server, length);
        bzero(client_string, 4096);
        temp = recvfrom(socket_fd, client_string, 4096, 0, &server_addr, &length);
        if (strcmp(client_string, "$"))
        {
            printf("IP address for '%s' is - '%s'\n", domain, client_string);
        }
        else
        {
            printf("Domain name not found in database.\n");
        }
    }
}

int main(int argc, char *argv[])
{
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (socket_fd < 0)
    {
        printf("Couldn't open socket. Exiting.");
        exit(1);
    }

    server.sin_family = AF_INET;
    host_name = gethostbyname(argv[1]);

    if (host_name == 0)
    {
        printf("Couldn't find host. Exiting.");
        exit(1);
    }

    bcopy((char *)host_name->h_addr, (char *)&server.sin_addr, host_name->h_length);
    server.sin_port = htons(atoi(argv[2]));
    length = sizeof(struct sockaddr_in);

    while (1)
    {
        get_messages();
    }
}