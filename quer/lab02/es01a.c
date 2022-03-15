#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

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
    data_t info;
    char *f1, *f2, *f3;
    FILE *fp1, *fp3;
    int fd2;

    if (argc != 4)
    {
        fprintf(stderr, "invalid number of parameters\n");
        return 1;
    }

    f1 = argv[1];
    f2 = argv[2];
    f3 = argv[3];

    if ((fp1 = fopen(f1, "r")) == NULL)
    {
        fprintf(stderr, "cannot open file \"%s\".\n", f1);
        return 1;
    }

    if ((fd2 = open(f2, O_WRONLY)) == -1)
    {
        fprintf(stderr, "cannot open file \"%s\".\n", f2);
        return 1;
    }

    if ((fp3 = fopen(f3, "w")) == NULL)
    {
        fprintf(stderr, "cannot open file \"%s\".\n", f3);
        return 1;
    }

    while (fscanf(fp1, "%u %lu %s %s %u", &info.id, &info.reg, info.surname, info.name, &info.mark) != EOF)
    {
        printf("id:\t\t%u\nreg:\t\t%lu\nsurname:\t%s\nname:\t\t%s\nmark:\t\t%u\n\n", info.id, info.reg, info.surname, info.name, info.mark);
        write(fd2, &info, sizeof(info));
        read(fd2, &info, sizeof(info));
        fprintf(fp3, "%u %lu %s %s %u\n", info.id, info.reg, info.surname, info.name, info.mark);
    }

    fclose(fp1);
    fclose(fp3);
    close(fd2);

    return 0;
}