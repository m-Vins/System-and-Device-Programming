#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#define DBG 0

#define SIZE 256+1

int n_threads;
char **entries;
pthread_barrier_t *bar;
char end = 0;
pthread_t *tids;

struct arg_s {
    int thread_index;
    char *path;
} ;

void *thread_job(void *arg);
void *thread_compare(void *arg);


int main(int argc,char **argv){
    struct arg_s *args;
    pthread_t comp_tid;
    

    if(0){
        printf("insufficient number of parameters!\n");
        return 1;
    }

    n_threads = argc - 1;
    tids = malloc(sizeof(pthread_t)*n_threads);
    entries = (char **)malloc(sizeof(char *)*n_threads);
    args = malloc(sizeof(struct arg_s)*n_threads);
    bar = malloc(sizeof(pthread_barrier_t));
    pthread_barrier_init(bar,NULL,n_threads+1);


    for(int i = 0 ; i < n_threads; i++){
        args[i].path = argv[i+1];
        args[i].thread_index = i;
        pthread_create(&tids[i],NULL,thread_job,(void *)&args[i]);
    }

    pthread_create(&comp_tid,NULL,thread_compare,NULL);

    for(int i = 0 ; i < n_threads; i++) pthread_join(tids[i],NULL);

    
    printf("folders are equal\n");
    
    
    pthread_cancel(comp_tid);
    

    free(bar);
    free(args);
    free(entries);
    free(tids);
    return 0;
}


void visit_recursive(char *path, int thread_index){
    DIR *dp = opendir(path);
    struct dirent *dirp;
    struct stat tstat;
    char nextpath[SIZE];

    while((dirp = readdir(dp)) != NULL){
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0) // exclude itself and its parent
            continue;
            
        #if DBG > 0
        printf("thread %lu\tfound entry: %s/%s\n",pthread_self(),path,dirp->d_name);
        #endif
        
        entries[thread_index] = dirp->d_name;

        sprintf(nextpath, "%s/%s", path, dirp->d_name);
        stat(nextpath, &tstat);
        if (S_ISDIR(tstat.st_mode)){
            visit_recursive(nextpath,thread_index);
        }

        /* WAIT ALL READING THREAD + THE COMPARING ONE */
        pthread_barrier_wait(bar);
        pthread_barrier_wait(bar);

    }

    entries[thread_index] = NULL; /* to notify other threads about the end of the list of entries */
    pthread_barrier_wait(bar);
    pthread_barrier_wait(bar);
}


void *thread_job(void *arg){
    int thread_index = ((struct arg_s*)arg)->thread_index;
    char *path = ((struct arg_s*)arg)->path;
     
    visit_recursive(path,thread_index);
    
    pthread_exit(NULL);
}

void *thread_compare(void *arg){
    int end_of_folder;
    while(1){
        end_of_folder = 1;
        pthread_barrier_wait(bar);

        for(int i = 0 ; i < n_threads; i++) 
            if(entries[i] != NULL){
                end_of_folder = 0;
        }

        if(!end_of_folder){
            for(int i = 1 ; i < n_threads; i++) 
                if((entries[i-1] == NULL && entries[i] != NULL) || 
                        (entries[i-1] != NULL && entries[i] == NULL) ||
                        strcmp(entries[i-1],entries[i]) != 0 ){
                    
                    printf("folders are different\n");
                    for(int j = 0;j<n_threads;j++)
                        pthread_cancel(tids[j]);
                    exit(0);
                }
        }
        pthread_barrier_wait(bar);
    }
}