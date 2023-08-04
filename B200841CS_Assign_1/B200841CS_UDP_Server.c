#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

void reverse_string(char string[], int size)
{
    for (int i = 0; i < size / 2; i++)
    {
        char temp = string[i];
        string[i] = string[size - 1 - i];
        string[size - 1 - i] = temp;
    }
}

int main(int argc, char *argv[])
{
    struct sockaddr_in server;
    struct sockaddr_in client_addr;
    char client_string[1024];
    bzero(client_string, 1024);

    if (argc < 2)
    {
        printf("Port not provided. Exiting.");
        exit(1);
    }

    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0)
    {
        printf("Couldn't open socket. Exiting.");
        exit(1);
    }
    int length = sizeof(server);
    bzero(&server, length);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(atoi(argv[1]));
    if (bind(socket_fd, (struct sockaddr *)&server, length) < 0)
    {
        printf("Couldn't bind. Exiting.");
        exit(1);
    }
    int addr_size = sizeof(struct sockaddr_in);

    int temp = recvfrom(socket_fd, client_string, 1024, 0, (struct sockaddr *)&client_addr, &addr_size);
    if (temp < 0)
    {
        printf("Couldn't read input from socket. Exiting.");
        exit(1);
    }
    printf("Received string from client: %s\n", client_string);
    int input_size = strlen(client_string);
    reverse_string(client_string, input_size);

    temp = sendto(socket_fd, client_string, input_size,
                  0, (struct sockaddr *)&client_addr, addr_size);
    if (temp < 0)
    {
        printf("Couldn't write output to socket. Exiting.");
        exit(1);
    }
    printf("Returned string to client: %s\n", client_string);
}