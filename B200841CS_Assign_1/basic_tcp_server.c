#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    char client_string[256];

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;

    bzero((char *)&serv_addr, sizeof(serv_addr));

    int port_num = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_num);

    if (bind(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Couldn't bind. Exiting.");
        exit(1);
    }

    listen(socket_fd, 1);
    struct sockaddr_in client_addr;
    int client_size = sizeof(client_addr);
    // int client_size = sizeof(client_addr);

    int newsocket_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_size);

    bzero(client_string, 256);

    int temp = read(newsocket_fd, client_string, 255);

    temp = write(newsocket_fd, client_string, 255);

    return 0;
}