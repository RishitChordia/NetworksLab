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
struct hostent *host_name;
char client_string[4096];
int socket_fd, length, temp;

void enter_chatroom()
{
    int length = sizeof(struct sockaddr_in);
    printf("Please enter your name: ");

    bzero(client_string, 4096);
    fgets(client_string, 4095, stdin);

    int temp = sendto(socket_fd, client_string, strlen(client_string), 0, &server, length);

    if (temp < 0)
    {
        printf("Couldn't write input to socket. Exiting.");
        exit(1);
    }

    temp = recvfrom(socket_fd, client_string, 4096, 0, &server_addr, &length);
    if (temp < 0)
    {
        printf("Couldn't read output from socket. Exiting.");
        exit(1);
    }
    printf("%s", client_string);
    if (!strcmp(client_string, "Sorry, room full."))
    {
        exit(0);
    }
}

void *send_message(void *data)
{
    while (1)
    {
        int length = sizeof(struct sockaddr_in);

        bzero(client_string, 4096);
        fgets(client_string, 4095, stdin);

        int temp = sendto(socket_fd, client_string, strlen(client_string), 0, &server, length);
        if (!(strcmp(client_string, "/exit\n") * strcmp(client_string, "/quit\n") * strcmp(client_string, "/part\n")))
        {
            exit(0);
        }

        if (temp < 0)
        {
            printf("Couldn't write input to socket. Exiting.");
            exit(1);
        }
    }
}

void *get_messages(void *data)
{
    while (1)
    {
        bzero(client_string, 4096);

        int length = sizeof(struct sockaddr_in);
        temp = recvfrom(socket_fd, client_string, 4096, 0, &server_addr, &length);
        if (temp < 0)
        {
            printf("Couldn't read output from socket. Exiting.");
            exit(1);
        }
        printf("%s", client_string);
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

    enter_chatroom();

    pthread_t thread_id1;
    pthread_t thread_id2;
    int return_value1 = pthread_create(&thread_id1, NULL, send_message, NULL);
    int return_value2 = pthread_create(&thread_id2, NULL, get_messages, NULL);
    pthread_join(thread_id1, NULL);
    pthread_join(thread_id2, NULL);
    pthread_exit(NULL);
}