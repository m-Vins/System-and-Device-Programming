#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int last1 = -1;
int last2 = -1;
int finish = 0;

void manager(int sig)
{
    switch (sig)
    {
    case SIGUSR1:
        if (last1 == 1)
        {
            if (last2 == 1)
            {
                finish = 1;
                return;
            }
            printf("error\n");
        }
        else if (last1 == 2)
        {
            printf("success\n");
        }
        last2 = last1;
        last1 = 1;
        break;
    case SIGUSR2:
        if (last1 == 1)
        {
            printf("success\n");
        }
        else if (last1 == 2)
        {
            if (last2 == 2)
            {
                finish = 1;
                return;
            }
            printf("error\n");
        }
        last2 = last1;
        last1 = 2;
        break;
    default:
        break;
    }
    return;
}

int main(int argc, char **argv)
{
    signal(SIGUSR1, manager);
    signal(SIGUSR2, manager);

    while (!finish)
        ;
    return 0;
}

/*
SIGUSR1 10
SIGUSR2 12 --> kill -12 [pid]
*/