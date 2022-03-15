#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define DBG 1

#define MAXLEN 20

typedef struct
{
    unsigned int id;
    unsigned long int reg;
    char surname[MAXLEN];
    char name[MAXLEN];
    unsigned int mark;
} data_t;

int main(int argc, char **argv)
{
    int fd; // file descriptor
    char cmd;
    int n;
    data_t buff;

    if (argc != 2)
    {
        fprintf(stderr, "invalid number of parameters\n");
        return 1;
    }

    while (1)
    {
        fscanf(stdin, "%c", &cmd);
        if (cmd == 'E')
            break;

        if (cmd == 'W' || cmd == 'R')
        {
            fscanf(stdin, " %d", &n);
#if DBG
            printf("cmd : %c\t\tn: %d\n\n", cmd, n);
#endif

            switch (cmd)
            {
            case ('R'):
                if ((fd = open(argv[1], O_RDONLY)) == -1)
                {
                    fprintf(stderr, "cannot open file \"%s\".\n", argv[1]);
                    return 1;
                }

                lseek(fd, (n - 1) * sizeof(data_t), SEEK_SET); // setting the pointer at the right position

                if (read(fd, &buff, sizeof(data_t)) <= 0)
                {
                    fprintf(stderr, "error in the reading\n");
                    return 1;
                }
                printf("id:\t\t%u\nreg:\t\t%lu\nsurname:\t%s\nname:\t\t%s\nmark:\t\t%u\n\n", buff.id, buff.reg, buff.surname, buff.name, buff.mark);

                close(fd);
                break;
            case ('W'):
                if ((fd = open(argv[1], O_WRONLY)) == -1)
                {
                    fprintf(stderr, "cannot open file \"%s\".\n", argv[1]);
                    return 1;
                }

                lseek(fd, (n - 1) * sizeof(data_t), SEEK_SET); // setting the pointer at the right position
                fscanf(stdin, "%u %lu %s %s %u", &buff.id, &buff.reg, buff.surname, buff.name, &buff.mark);
                printf("id:\t\t%u\nreg:\t\t%lu\nsurname:\t%s\nname:\t\t%s\nmark:\t\t%u\n\n", buff.id, buff.reg, buff.surname, buff.name, buff.mark);
                write(fd, &buff, sizeof(data_t));

                close(fd);
                break;
            default:
                break;
            }
        }
    };

    return 0;
}