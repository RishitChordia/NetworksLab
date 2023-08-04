#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    struct sockaddr_in server;
    struct sockaddr_in client_addr;
    char client_string[1024];
    bzero(client_string, 1024);

    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

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


    temp = sendto(socket_fd, client_string, 1024,
                  0, (struct sockaddr *)&client_addr, addr_size);
}