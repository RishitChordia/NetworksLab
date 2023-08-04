#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    struct sockaddr_in server, server_addr;
    struct hostent *host_name;
    char client_string[256];

    if (argc != 3)
    {
        printf("Incorrect arguments. Exiting.");
        exit(1);
    }

    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

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
    int length = sizeof(struct sockaddr_in);

    printf("Please enter the message: ");

    bzero(client_string, 256);
    scanf("%s", client_string);
    int temp = sendto(socket_fd, client_string, strlen(client_string), 0, &server, length);
    if (temp < 0)
    {
        printf("Couldn't write input to socket. Exiting.");
        exit(1);
    }
    printf("Sent string to server: %s\n", client_string);

    temp = recvfrom(socket_fd, client_string, 256, 0, &server_addr, &length);
    if (temp < 0)
    {
        printf("Couldn't read output from socket. Exiting.");
        exit(1);
    }
    printf("Received string from server: %s\n", client_string);
}
