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

struct sockaddr_in server, server_addr;
struct sockaddr_in client_addr;
struct hostent *host_name;
char client_string[4096];
int socket_fd, socket_fd, length, temp;
struct stack cache;
int MAX_CACHE_SIZE = 30;

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

void *get_messages(void *data)
{
    while (1)
    {
        bzero(client_string, 4096);
        length = sizeof(struct sockaddr_in);
        temp = recvfrom(socket_fd, client_string, 4096, 0, &client_addr, &length);
        if (temp < 0)
        {
            printf("Couldn't read output from socket. Exiting.");
            exit(1);
        }
        char *ip = get_ip_address(client_string);
        if (strcmp(ip, "$"))
        {
            printf("Found IP for '%s' in database. Returning.\n", client_string);
            sendto(socket_fd, ip, strlen(ip), 0, (struct sockaddr *)&client_addr, length);
        }
        else
        {
            printf("Domain name '%s' not present in database. Returning.\n", client_string);
            temp = sendto(socket_fd, "$", 1, 0, (struct sockaddr *)&client_addr, length);
        }
    }
}

void create_database()
{
    bzero(cache.records[cache.size].domain_name, 50);
    bzero(cache.records[cache.size].ip_address, 20);
    strcpy(cache.records[cache.size].domain_name, "www.google.com");
    strcpy(cache.records[cache.size++].ip_address, "209.85.231.104");

    bzero(cache.records[cache.size].domain_name, 50);
    bzero(cache.records[cache.size].ip_address, 20);
    strcpy(cache.records[cache.size].domain_name, "www.microsoft.com");
    strcpy(cache.records[cache.size++].ip_address, "207.46.170.123");

    bzero(cache.records[cache.size].domain_name, 50);
    bzero(cache.records[cache.size].ip_address, 20);
    strcpy(cache.records[cache.size].domain_name, "www.yahoo.com");
    strcpy(cache.records[cache.size++].ip_address, "72.30.2.43");

    bzero(cache.records[cache.size].domain_name, 50);
    bzero(cache.records[cache.size].ip_address, 20);
    strcpy(cache.records[cache.size].domain_name, "www.facebook.com");
    strcpy(cache.records[cache.size++].ip_address, "66.220.149.25");

    bzero(cache.records[cache.size].domain_name, 50);
    bzero(cache.records[cache.size].ip_address, 20);
    strcpy(cache.records[cache.size].domain_name, "www.wikipedia.com");
    strcpy(cache.records[cache.size++].ip_address, "208.80.152.2");

    bzero(cache.records[cache.size].domain_name, 50);
    bzero(cache.records[cache.size].ip_address, 20);
    strcpy(cache.records[cache.size].domain_name, "www.twitter.com");
    strcpy(cache.records[cache.size++].ip_address, "128.242.245.116");

    bzero(cache.records[cache.size].domain_name, 50);
    bzero(cache.records[cache.size].ip_address, 20);
    strcpy(cache.records[cache.size].domain_name, "www.gmail.com");
    strcpy(cache.records[cache.size++].ip_address, "142.251.33.69");

    bzero(cache.records[cache.size].domain_name, 50);
    bzero(cache.records[cache.size].ip_address, 20);
    strcpy(cache.records[cache.size].domain_name, "www.youtube.com");
    strcpy(cache.records[cache.size++].ip_address, "216.58.206.238");

    bzero(cache.records[cache.size].domain_name, 50);
    bzero(cache.records[cache.size].ip_address, 20);
    strcpy(cache.records[cache.size].domain_name, "www.eduserver.nitc.ac.in");
    strcpy(cache.records[cache.size++].ip_address, "103.241.136.151");
}

int main(int argc, char *argv[])
{
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

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

    cache.size = 0;
    cache.records = malloc(sizeof(struct record) * MAX_CACHE_SIZE);
    create_database();

    pthread_t thread_id2;
    // int return_value1 = pthread_create(&thread_id1, NULL, send_message, NULL);
    int return_value2 = pthread_create(&thread_id2, NULL, get_messages, NULL);
    pthread_join(thread_id2, NULL);
    pthread_exit(NULL);
}