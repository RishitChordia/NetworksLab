#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

void reverse_string(char string[], int size)
{
    for (int i = 0; i < size / 2; i++)
    {
        // printf("%d - %c", i, string[i]);
        char temp = string[i];
        string[i] = string[size - 1 - i];
        string[size - 1 - i] = temp;
    }
}

int main(int argc, char *argv[])
{

    char client_string[256];

    if (argc < 2)
    {
        printf("Port not provided. Exiting.");
        exit(1);
    }

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_fd < 0)
    {
        printf("Couldn't open socket. Exiting.");
        exit(1);
    }
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

    int newsocket_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_size);

    if (newsocket_fd < 0)
    {
        printf("Couldn't accept client. Exiting.");
        exit(1);
    }

    bzero(client_string, 256);

    int temp = read(newsocket_fd, client_string, 255);

    if (temp < 0)
    {
        printf("Couldn't read input from socket. Exiting.");
        exit(1);
    }

    printf("Received string from client: %s\n", client_string);

    int input_size = strlen(client_string);
    reverse_string(client_string, input_size);

    temp = write(newsocket_fd, client_string, input_size);

    if (temp < 0)
    {
        printf("Couldn't write output to socket. Exiting.");
        exit(1);
    }

    printf("Returned string to client: %s\n", client_string);

    return 0;
}