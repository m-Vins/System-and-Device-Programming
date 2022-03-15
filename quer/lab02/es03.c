#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>

#define DBG 0

#define MAX 20      // max number of entries
#define MAXLEN 1024 // max len of each path

void copy_file(const char *src, const char *dst, const char *name)
{
    char buff[MAXLEN];
    int fdin, fdout; // file descriptor
    int nr, nw;
    int size = 0;

#if DBG
    printf("opening file %s\n", src);
#endif
    if ((fdin = open(src, O_RDONLY)) == -1)
    {
        fprintf(stderr, "cannot open file \"%s\".\n", src);
        return;
    }
#if DBG
    printf("opening file %s\n", dst);
#endif
    if ((fdout = open(dst, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IROTH)) == -1)
    {
        fprintf(stderr, "cannot open file \"%s\".\n", dst);
        return;
    }
#if DBG
    printf("reading src file dimension\n");
#endif
    while ((nr = read(fdin, buff, MAXLEN)) > 0)
    {
        size += nr;
    }
#if DBG
    printf("dimension of the file : %d\n", size);
    printf("name of the file: %s\n", name);
#endif
    lseek(fdin, 0, SEEK_SET);

    write(fdout, (void *)name, strlen(name));
    write(fdout, (void *)&size, sizeof(int));

    while ((nr = read(fdin, buff, MAXLEN)) > 0)
    {
        nw = write(fdout, buff, nr);
        if (nr != nw)
            fprintf(stderr, "write error\n");
    }

    close(fdout);
    close(fdin);
}

void visit(const char *path, const char *target_path)
{
    struct stat tmp_stat;
    DIR *dp;
    struct dirent *dirp;
    char tmp_src[MAXLEN], tmp_target[MAXLEN];
    char **array_next_paths = (char **)malloc(MAX * sizeof(char *)); //  devo allocare un array di path dinamico per ogni ricorsione.
    char **array_next_target_paths = (char **)malloc(MAX * sizeof(char *));
    int n_next_paths = 0;

    dp = opendir(path); // open the current directory

    while ((dirp = readdir(dp)) != NULL) // list all the entries
    {
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0) // exclude itself and its parent
            continue;

        sprintf(tmp_src, "%s/%s", path, dirp->d_name);           // path of the current file/subdir
        sprintf(tmp_target, "%s/%s", target_path, dirp->d_name); // path of the target file/subdir

        stat(tmp_src, &tmp_stat);

        if (S_ISDIR(tmp_stat.st_mode)) // if subdir -> implement recursion
        {
#if DBG
            printf("D : %s\n", tmp_target);
#endif
            mkdir(tmp_target, O_RDWR);

            array_next_paths[n_next_paths] = (char *)malloc(MAXLEN * sizeof(char));
            array_next_target_paths[n_next_paths] = (char *)malloc(MAXLEN * sizeof(char));
            strcpy(array_next_paths[n_next_paths], tmp_src);
            strcpy(array_next_target_paths[n_next_paths], tmp_target);
            visit(array_next_paths[n_next_paths], array_next_target_paths[n_next_paths]);
            if (++n_next_paths == MAX)
            {
                fprintf(stderr, "error: too much entries in the array\n");
                return;
            }
        }
        else
        {
#if DBG
            printf("F : %s\n", tmp_target);
#endif
            copy_file(tmp_src, tmp_target, dirp->d_name);
        }
    }

    for (int i = 0; i < n_next_paths; i++)
        free(array_next_paths[i]);
    free(array_next_paths);
}

int main(int argc, char **argv)
{
    char *src_dir, *target_dir;
    setbuf(stdout, 0);

    if (argc != 3)
    {
        fprintf(stderr, "invalid number of parameters\n");
        return 1;
    }

    src_dir = argv[1];
    target_dir = argv[2];

    visit(src_dir, target_dir);
    return 0;
}