#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>

#define MAX 20      // max number of entries
#define MAXLEN 1024 // max len of each path

void *visit(void *path_arg)
{
    char *path = (char *)path_arg;
    struct stat stat_v;
    DIR *dp;
    struct dirent *dirp;
    char buff[MAXLEN];

    char **array_next_paths = (char **)malloc(MAX * sizeof(char *)); //  devo allocare un array di path dinamico per ogni ricorsione.
    int n_next_paths = 0;

    // TO DO:
    // check if this path point to a directory
    dp = opendir(path); // open the current directory

    while ((dirp = readdir(dp)) != NULL) // list all the entries
    {
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0) // exclude itself and its parent
            continue;

        sprintf(buff, "%s/%s", path, dirp->d_name); // path of the current file/subdir
        stat(buff, &stat_v);

        if (S_ISDIR(stat_v.st_mode))                                                // if subdir -> implement recursion
        {                                                                           // it is a subdirectory --> recursion
            array_next_paths[n_next_paths] = (char *)malloc(MAXLEN * sizeof(char)); // allocate the argument
            strcpy(array_next_paths[n_next_paths], buff);
            visit((void *)array_next_paths[n_next_paths]);

            // just a check whether too much entries in the array
            if (++n_next_paths == MAX)
            {
                fprintf(stderr, "error: too much entries in the array\n");
                exit(0);
            }
        }
        else
        {
            fprintf(stdout, "%ld :\t%s/%s\n", pthread_self(), path, dirp->d_name);
        }
    }

    for (int i = 0; i < n_next_paths; i++)
        free(array_next_paths[i]);
    free(array_next_paths);
}

int main(int argc, char **argv)
{
    int N = argc - 1;
    pthread_t tids[N];

    for (int i = 0; i < N; i++)
    {
        pthread_create(&tids[i], NULL, visit, (void *)argv[i + 1]);
    }

    for (int t = 0; t < N; t++)
    {
        pthread_join(tids[t], NULL);
    }
}