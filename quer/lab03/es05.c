#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#define FILENAME "file.txt"
#define MAXLEN 20
#define DBG

typedef struct
{
    unsigned int id;
    unsigned long int reg;
    char surname[MAXLEN];
    char name[MAXLEN];
    unsigned int mark;
} data_t;


void *thread_increment_id(void* arg);
void *thread_decrement_mark(void* arg);

void *memmap;
int n_lines;

int main(void){
    int fd;
    struct stat sbuf;
#ifdef DBG
    data_t buff;
#endif 
    pthread_t tids[2];

    

    fd = open(FILENAME,O_RDWR);
    if(fd < 0){
        fprintf(stderr,"error encoutered while opening the file\n");
        abort();
    }

    if(fstat(fd,&sbuf) < 0){
        fprintf(stderr,"error fstat\n");
        abort();
    }

    

    n_lines = sbuf.st_size / sizeof(data_t) ; 
    
    fprintf(stdout,"the file contains %d lines\n",n_lines);

    if((memmap = mmap(0,sbuf.st_size,PROT_READ | PROT_WRITE,MAP_SHARED, fd, 0)) == MAP_FAILED){
        fprintf(stderr,"error encoutered while mapping the file\n");
        abort();
    } 

    pthread_create(&tids[0],NULL,thread_decrement_mark,NULL);
    pthread_create(&tids[1],NULL,thread_increment_id,NULL);

    for(int i=0 ; i<2 ; i++) pthread_join(tids[i],NULL);

    munmap(memmap,sbuf.st_size);

    close(fd);

#ifdef DBG
    if ((fd = open(FILENAME, O_RDONLY)) == -1)
    {
        fprintf(stderr, "cannot open file \"%s\".\n",FILENAME);
        return 1;
    }
    for(int i = 0;i < n_lines ;i++){
        read(fd, &buff, sizeof(data_t));
        printf("id:\t\t%u\nreg:\t\t%lu\nsurname:\t%s\nname:\t\t%s\nmark:\t\t%u\n\n", buff.id, buff.reg, buff.surname, buff.name, buff.mark);
    }
    close(fd);
#endif

    return 0;
}


void* thread_increment_id(void* arg){
    for(int i=0; i < n_lines ;i++)
        ((data_t *)memmap)[i].reg++;
    pthread_exit(0);
}

void* thread_decrement_mark(void* arg){
    for(int i = n_lines - 1; i >= 0; i--)
        ((data_t *)memmap)[i].mark--;
    pthread_exit(0);
}