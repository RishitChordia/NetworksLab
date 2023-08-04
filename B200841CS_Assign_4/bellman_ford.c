#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>

int min(int a, int b)
{
    if (a < b)
    {
        return a;
    }
    return b;
}

int NUMBER_OF_ROUTERS;
struct router_data *router_matrix;

struct router_data
{
    char router_name;
    int *delays;
    char *next_router_arr;
    int *neighbors;
    // 0 for non neighbors, 1 for neighbors
    // easier having fixed size than having to make variable sized arrays
};

void initialize_router_matrix()
{

    router_matrix = (struct router_data *)malloc(sizeof(struct router_data) * NUMBER_OF_ROUTERS);

    for (int i = 0; i < NUMBER_OF_ROUTERS; i++)
    {

        router_matrix[i].delays = (int *)malloc(NUMBER_OF_ROUTERS * sizeof(int));
        router_matrix[i].next_router_arr = (char *)malloc(NUMBER_OF_ROUTERS * sizeof(char));

        for (int j = 0; j < NUMBER_OF_ROUTERS; j++)
        {
            router_matrix[i].delays[j] = 999999;
            router_matrix[i].next_router_arr[j] = 0;
        }
        router_matrix[i].delays[i] = 0;

        router_matrix[i].neighbors = (int *)calloc(NUMBER_OF_ROUTERS, sizeof(int));
        router_matrix[i].router_name = 'A' + i;
    }
}

void print_router_matrix()
{

    printf("\t");

    for (int i = 0; i < NUMBER_OF_ROUTERS; i++)
    {
        printf("%c\t", router_matrix[i].router_name);
    }

    for (int i = 0; i < NUMBER_OF_ROUTERS; i++)
    {
        printf("\n%c\t", router_matrix[i].router_name);
        for (int j = 0; j < NUMBER_OF_ROUTERS; j++)
        {
            printf("%d\t", router_matrix[j].delays[i]);
        }
    }

    printf("\n\n");
}

void append_router_connection(char src, char dst, int delay)
{
    router_matrix[src - 'A'].delays[dst - 'A'] = min(router_matrix[src - 'A'].delays[dst - 'A'], delay);
    router_matrix[src - 'A'].neighbors[dst - 'A'] = 1;
}

int bellman_ford_iteration()
{
    int updation_count = 0;
    for (int i = 0; i < NUMBER_OF_ROUTERS; i++)
    {
        for (int j = 0; j < NUMBER_OF_ROUTERS; j++)
        {
            // updating data for path from i to j
            int min_delay = 999999;
            char min_hop = 0;
            for (int k = 0; k < NUMBER_OF_ROUTERS; k++)
            {
                if (router_matrix[i].neighbors[k])
                {
                    int delay_through_k = router_matrix[i].delays[k] + router_matrix[k].delays[j];
                    if (delay_through_k < min_delay)
                    {
                        min_delay = delay_through_k;
                        min_hop = 'A' + k;
                    }
                }
            }
            if (min_delay < router_matrix[i].delays[j])
            {
                updation_count += 1;
                router_matrix[i].delays[j] = min_delay;
                router_matrix[i].next_router_arr[j] = min_hop;
            }
        }
    }
    return updation_count;
}

void bellman_ford()
{
    for (int iterations = 1;; iterations++)
    {
        int updations = bellman_ford_iteration();
        if (updations == 0)
        {
            printf("No further updates possible. Terminating.\n\n");
            break;
        }
        else
        {
            printf("\n%d values updated in iteration #%d. Matrix is as follows:\n\n", updations, iterations);
            print_router_matrix();
        }
    }
}

int main()
{
    printf("Enter number of routers: ");
    scanf("%d", &NUMBER_OF_ROUTERS);
    initialize_router_matrix();
    print_router_matrix();
    char user_inp;
    printf("Enter e to add a new edge, t to finish graph creation.\n");
    while (1)
    {
        scanf("%c", &user_inp);
        if (user_inp == 't')
        {
            break;
        }
        if (user_inp == 'e')
        {
            char src, dst;
            int delay;
            printf("Enter source, destination and delay separated by spaces: ");
            scanf(" %c %c %d", &src, &dst, &delay);
            append_router_connection(src, dst, delay);
        }
    }
    print_router_matrix();
    bellman_ford();
    return 0;
}