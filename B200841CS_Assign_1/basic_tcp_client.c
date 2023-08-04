#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    char client_string[256];
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct hostent *server;
    server = gethostbyname(argv[1]);

    struct sockaddr_in serv_addr;
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    // bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(socket_fd, &serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Couldn't connect. Exiting.");
        exit(1);
    }

    bzero(client_string, 256);

    scanf("%s", client_string);
    int temp = write(socket_fd, client_string, strlen(client_string));

    bzero(client_string, 256);
    temp = read(socket_fd, client_string, 255);

    return 0;
}