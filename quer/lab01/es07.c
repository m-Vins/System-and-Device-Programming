#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define DBG 0

typedef struct arg
{
    long int *tree_generation;
    int actual_depth;
    int max_depth;
} arg_t;

void *thread_routine(void *argp)
{
    arg_t arguments[2];
    pthread_t tids[2];
    arg_t *arg = (arg_t *)argp;

#if DBG
    printf("thread routine with argument: ad %d md %d\n", arg->actual_depth, arg->max_depth);
    for (int i = 0; i < arg->actual_depth + 1; i++)
        printf("%ld\t", arg->tree_generation[i]);
    printf("\n");
#endif

    if (arg->actual_depth + 1 == arg->max_depth)
    {
        // LEAF NODE OF THE TREE
        for (int i = 0; i < arg->max_depth; i++)
            printf("%ld\t", arg->tree_generation[i]);
        printf("\n");
        pthread_exit(NULL);
    }

    for (int i = 0; i < 2; i++)
    {
        // GENERATING THE NEW ARGUMENT FOR THE THREAD
        arguments[i].actual_depth = arg->actual_depth + 1;
        arguments[i].tree_generation = (long int *)malloc((arguments[i].actual_depth + 1) * sizeof(long int));

        // COPYING THE LIST OF THE ANCESTOR THREADS
        for (int j = 0; j < arguments[i].actual_depth; j++)
        {
            arguments[i].tree_generation[j] = arg->tree_generation[j];
        }
        arguments[i].tree_generation[arguments[i].actual_depth] = pthread_self();
        arguments[i].max_depth = arg->max_depth;
        // THROWING THE NEW THREAD
        pthread_create(&tids[i], NULL, thread_routine, (void *)&arguments[i]);
    }

    // WAITING FOR THE THREADS
    for (int t = 0; t < 2; t++)
    {
        pthread_join(tids[t], NULL);
        free(arguments[t].tree_generation);
    }

    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    arg_t arguments[2];
    pthread_t tids[2];
    int n;
    setbuf(stdout, 0);

    n = atoi(argv[1]);
    printf("n = %d\n", n);

    for (int i = 0; i < 2; i++)
    {
        arguments[i].actual_depth = 0;
        arguments[i].tree_generation = (long int *)malloc((arguments[i].actual_depth + 1) * sizeof(long int));
        arguments[i].tree_generation[arguments[i].actual_depth] = pthread_self();
        arguments[i].max_depth = n;
        sleep(i);
        pthread_create(&tids[i], NULL, thread_routine, (void *)&arguments[i]);
    }
    for (int t = 0; t < 2; t++)
    {
        pthread_join(tids[t], NULL);
        free(arguments[t].tree_generation);
    }

    return 0;
}