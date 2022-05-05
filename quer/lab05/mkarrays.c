#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

/**
 * @brief 
 * 
 * @param argc 3
 * @param argv [number of files] [target directory]
 * @return 0
 */


int main(int argc, char **argv){
    int n;
    char path[50];
    
    if(argc != 3){
        fprintf(stderr,"invalid parameters number");
        return 1;
    }

    n = atoi(argv[1]); 
    srand(time(NULL));
    if(n > 999) return 1;
    
    for(int i = 0; i < n; i++){
        /* CREATING FILE */
        sprintf(path,"%s/%03d.txt",argv[2],i);

        int fd = open(path,O_CREAT | O_RDWR);
        if(fd < 0){
            fprintf(stderr,"error creating the file");
            return 1;
        }

        int l = rand()%20;
        int tmp;
        write(fd,(void *)&l,sizeof(int));
        fprintf(stdout,"file %s\t-> len %d\t:\t",path,l);
        for(int j = 0; j < l; j++){
            tmp = (int)rand()%100;
            write(fd,(void *)&tmp,sizeof(int));
            fprintf(stdout," %d",tmp);
        }
        fprintf(stdout,"\n");
        close(fd);
    }
    return 0;
    
}