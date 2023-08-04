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

    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;

    host_name = gethostbyname(argv[1]);

    bcopy((char *)host_name->h_addr, (char *)&server.sin_addr, host_name->h_length);

    server.sin_port = htons(atoi(argv[2]));

    int length = sizeof(struct sockaddr_in);
    bzero(client_string, 256);
    scanf("%s", client_string);

    int temp = sendto(socket_fd, client_string, strlen(client_string), 0, &server, length);
    temp = recvfrom(socket_fd, client_string, 256, 0, &server_addr, &length);
}
