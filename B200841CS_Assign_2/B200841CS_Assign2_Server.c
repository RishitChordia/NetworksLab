#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>

struct client_info
{
    char name[4096];
    struct sockaddr_in client_addr;
};

int find_client(struct sockaddr_in client_addr, struct client_info client_arr[], int n, char *client_string)
{
    for (int i = 0; i < n; i++)
    {
        if (client_arr[i].client_addr.sin_port == client_addr.sin_port)
        {
            return i;
        }
    }
    return n;
}

int main(int argc, char *argv[])
{
    struct sockaddr_in server;
    struct sockaddr_in client_addr;
    char client_string[4096];
    bzero(client_string, 4096);
    struct client_info client_arr[4096];
    int client_arr_index = 0, number_of_clients = 0;

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
    int n;

    while (1)
    {
        char message[4096] = "";
        bzero(&client_addr, sizeof(struct sockaddr_in));
        bzero(client_string, 4096);
        int temp = recvfrom(socket_fd, client_string, 4096, 0, (struct sockaddr *)&client_addr, &addr_size);
        int sender = find_client(client_addr, client_arr, client_arr_index, client_string);
        if (sender != client_arr_index)
        {
            if (strcmp(client_string, "/exit\n") * strcmp(client_string, "/quit\n") * strcmp(client_string, "/part\n"))
            {
                strcat(message, client_arr[sender].name);
                strcat(message, " : ");
                strcat(message, client_string);
            }
            else
            {
                number_of_clients--;
                strcat(message, client_arr[sender].name);
                strcat(message, " has left the room.\n");
            }
        }
        else
        {
            if (number_of_clients == 10)
            {
                temp = sendto(socket_fd, "Sorry, room full.", 17, 0, (struct sockaddr *)&client_addr, addr_size);
                continue;
            }
            else
            {
                bzero(&client_arr[client_arr_index].client_addr, sizeof(struct sockaddr_in));
                client_arr[client_arr_index].client_addr = client_addr;
                for (int i = 0; i < strlen(client_string) - 1; i++)
                {
                    client_arr[sender].name[i] = client_string[i];
                }
                client_arr_index++;
                number_of_clients++;
                temp = sendto(socket_fd, "Welcome!\n", 9, 0, (struct sockaddr *)&client_addr, addr_size);
                strcat(message, client_arr[sender].name);
                strcat(message, " has joined the room.\n");
            }
        }
        printf("%s", message);
        for (int i = 0; i < client_arr_index; i++)
        {
            if (i != sender)
            {
                sendto(socket_fd, message, strlen(message), 0, (struct sockaddr *)&client_arr[i].client_addr, addr_size);
            }
        }
    }

    return 0;
}