#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    char client_string[256];
    int portno = atoi(argv[2]);
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (argc != 3)
    {
        printf("Incorrect arguments. Exiting.");
        exit(1);
    }

    if (socket_fd < 0)
    {
        printf("Couldn't open socket. Exiting.");
        exit(1);
    }

    struct hostent *server;
    server = gethostbyname(argv[1]);

    if (server == NULL)
    {
        printf("Couldn't find host. Exiting.");
        exit(1);
    }

    struct sockaddr_in serv_addr;
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr, server->h_length);
    // bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    serv_addr.sin_port = htons(portno);

    if (connect(socket_fd, &serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Couldn't connect. Exiting.");
        exit(1);
    }

    printf("Please enter the message: ");
    bzero(client_string, 256);

    scanf("%s", client_string);
    int temp = write(socket_fd, client_string, strlen(client_string));
    if (temp < 0)
    {
        printf("Couldn't write input to socket. Exiting.");
        exit(1);
    }
    printf("Sent string to server: %s\n", client_string);

    bzero(client_string, 256);
    temp = read(socket_fd, client_string, 255);
    if (temp < 0)
    {
        printf("Couldn't read output from socket. Exiting.");
        exit(1);
    }
    printf("Received string from server: %s\n", client_string);

    return 0;
}