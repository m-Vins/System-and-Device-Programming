/*
this version of the es02 take as parameters:
1) the folder where the input files are stored with name as 000.txt, 001.txt and so on
2) the number of input files

is also exploit synchronization to compute the total_length since tha main thread wait 
all threads reading the length of each array.
*/


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>

#define DBG 0



typedef struct {
    int length;
    int *array;
    char path[50];
    int pos;
    sem_t *sem_length;
} arg_t;

void *thread_job(void *);
void MergeSort(int *A, int N);
void print_array(int *array, int l);

int main(int argc, char **argv){
    pthread_t *tids;
    int n_threads;
    arg_t *args;
    int tot_len;
    int min;
    int *final_array;
    sem_t* sem_length = (sem_t *)malloc(sizeof(sem_t));
    


    if(argc != 3){
        fprintf(stderr,"insufficient number of parameter\n");
        return 1;
    }

    n_threads = atoi(argv[2]);
    args = (arg_t *)malloc(sizeof(arg_t) * n_threads);
    tids = (pthread_t *)malloc(sizeof(pthread_t)*n_threads);
    sem_init(sem_length,0,0);
    

    for(int i = 0; i < n_threads; i++){
        sprintf(args[i].path,"%s/%03d.txt",argv[1],i);
        args[i].sem_length = sem_length;
        pthread_create(&tids[i],NULL,thread_job,(void * )&args[i]);
        #if DBG > 0
        sleep(1);
        #endif
    }

    /*  WAIT ALL THREADS WRITING THE LENGHTS OF THE ARRAYS*/
    for(int i=0; i< n_threads; i++ ) sem_wait(sem_length);

    
    tot_len = 0;
    for(int i = 0; i< n_threads ; i++) tot_len += args[i].length;


    /*  WAIT ALL THREADS */
    for(int i=0; i< n_threads; i++ )    pthread_join(tids[i],NULL);



    /*  INITIALIZING POS TO 0*/
    #if DBG > 1
    fprintf(stdout,"initializing pos to 0\n");
    #endif
    for(int i = 0; i< n_threads ; i++) args[i].pos = 0 ;

    #if DBG > 1
    fprintf(stdout,"total length : %d\n",tot_len);
    #endif

    /*  ALLOCATING NEW ARRAY*/
    final_array = (int *)malloc(sizeof(int)*tot_len);

    for(int i = 0; i<tot_len; i++){
        min = 0;
        for(int j = 0; j < n_threads; j++){
            if(args[j].pos < args[j].length || args[j].array[args[j].pos] > args[min].array[args[min].pos]) min = j ;
        }

        final_array[i] = args[min].array[args[min].pos];
        args[min].pos ++;
    }

    print_array(final_array,tot_len);

    for(int i=0;i<n_threads;i++) free(args[i].array);
    free(final_array);
    free(sem_length);    
    free(args);
    free(tids);
    return 0;
}

void *thread_job(void *arg){
    int *array;
    int *length = &((arg_t *)arg)->length;
    int fd = open(((arg_t *)arg)->path,O_RDONLY);

    /* READING FILE */
    read(fd,length,sizeof(int));

    sem_post(((arg_t *)arg)->sem_length);

    array = (int *)malloc(sizeof(int)*(*length));
    for(int i = 0;i<*length;i++) read(fd,&array[i],sizeof(int));
    close(fd);

    #if DBG > 0
    fprintf(stdout,"array length : %d\t",*length);
    print_array(array, *length);
    #endif

    MergeSort(array,*length);

    #if DBG > 0
    fprintf(stdout,"ordered array:\t\t");
    print_array(array, *length);
    #endif

    ((arg_t *)arg)->array = array;
    pthread_exit(NULL);
}

void Merge(int *A, int *B, int l, int q, int r) {
  int i, j, k;
  i = l;
  j = q+1;
  for(k = l; k <= r; k++)
    if (i > q)
      B[k] = A[j++];
    else if (j > r)
      B[k] = A[i++];
    else if (A[i] <= A[j])
      B[k] = A[i++];
    else
      B[k] = A[j++];
  for ( k = l; k <= r; k++ )
    A[k] = B[k];
  return;
}

void MergeSortR(int *A, int *B, int l, int r) {
  int q;
  if (r <= l)
    return;
  q = (l + r)/2;
  MergeSortR(A, B, l, q);
  MergeSortR(A, B, q+1, r);
  Merge(A, B, l, q, r);
}

void MergeSort(int *A, int N) {
  int l=0, r=N-1;
  int *B = (int *)malloc(N*sizeof(int));
  if (B == NULL) {
    printf("Memory allocation error\n");
    exit(1);
  }
  MergeSortR(A, B, l, r);
  free(B);
}

void print_array(int *array, int l){
    for(int i=0;i<l;i++) fprintf(stdout,"%d4 ",array[i]);
    fprintf(stdout,"\n");
}