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

struct record
{
    char domain_name[50];
    char ip_address[20];
};

struct stack
{
    int size;
    struct record *records;
};

struct sockaddr_in server, server_addr, server_self, server_addr_self;
struct sockaddr_in client_addr;
struct hostent *host_name;
struct hostent *host_name_self;
char client_string[4096];
int socket_fd, socket_fd_self, length, temp;
struct stack cache;
int MAX_CACHE_SIZE = 3;

char *get_ip_address(char domain_name[])
{
    for (int i = 0; i < cache.size; i++)
    {
        if (!(strcmp(cache.records[i].domain_name, domain_name)))
        {
            return cache.records[i].ip_address;
        }
    }
    return "$";
}

void append_to_stack(char domain_name[], char ip_address[])
{
    if (cache.size < MAX_CACHE_SIZE)
    {
        bzero(cache.records[cache.size].domain_name, 50);
        bzero(cache.records[cache.size].ip_address, 20);

        strcpy(cache.records[cache.size].domain_name, domain_name);
        strcpy(cache.records[cache.size].ip_address, ip_address);

        cache.size++;
    }
    else
    {
        for (int i = 0; i < MAX_CACHE_SIZE - 1; i++)
        {
            bzero(cache.records[i].domain_name, 50);
            bzero(cache.records[i].ip_address, 20);

            strcpy(cache.records[i].domain_name, cache.records[i + 1].domain_name);
            strcpy(cache.records[i].ip_address, cache.records[i + 1].ip_address);
        }
        bzero(cache.records[MAX_CACHE_SIZE - 1].domain_name, 50);
        bzero(cache.records[MAX_CACHE_SIZE - 1].ip_address, 20);

        strcpy(cache.records[MAX_CACHE_SIZE - 1].domain_name, domain_name);
        strcpy(cache.records[MAX_CACHE_SIZE - 1].ip_address, ip_address);
    }
}

void get_messages()
{
    while (1)
    {
        bzero(client_string, 4096);
        length = sizeof(struct sockaddr_in);
        temp = recvfrom(socket_fd_self, client_string, 4096, 0, &client_addr, &length);
        if (temp < 0)
        {
            printf("Couldn't read output from socket. Exiting.");
            exit(1);
        }
        char *ip = get_ip_address(client_string);
        if (strcmp(ip, "$"))
        {
            printf("Found IP for '%s' here. Returning.\n", client_string);
            sendto(socket_fd_self, ip, strlen(ip), 0, (struct sockaddr *)&client_addr, length);
        }
        else
        {
            printf("Couldn't find IP for '%s' here. Sending further.\n", client_string);
            sendto(socket_fd, client_string, strlen(client_string), 0, &server, length);
            char domain[50];
            strcpy(domain, client_string);
            bzero(client_string, 4096);
            temp = recvfrom(socket_fd, client_string, 4096, 0, &server_addr, &length);
            if (strcmp(client_string, "$"))
            {
                append_to_stack(domain, client_string);
            }
            sendto(socket_fd_self, client_string, strlen(client_string), 0, (struct sockaddr *)&client_addr, length);
        }
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

    socket_fd_self = socket(AF_INET, SOCK_DGRAM, 0);

    if (socket_fd_self < 0)
    {
        printf("Couldn't open socket. Exiting.");
        exit(1);
    }

    length = sizeof(server_self);
    bzero(&server_self, length);
    server_self.sin_family = AF_INET;
    server_self.sin_addr.s_addr = INADDR_ANY;
    server_self.sin_port = htons(atoi(argv[3]));

    if (bind(socket_fd_self, (struct sockaddr *)&server_self, length) < 0)
    {
        printf("Couldn't bind. Exiting.");
        exit(1);
    }

    cache.size = 0;
    cache.records = malloc(sizeof(struct record) * MAX_CACHE_SIZE);
    get_messages();
}