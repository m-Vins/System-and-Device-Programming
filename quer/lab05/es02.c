#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>

#define DBG 0


/**
 * each thread when has been done the work, increment the thread_done, 
 * and write into thread_done_queue its thread_index.
 * In this way the main thread wait for the thread_done to be greater than
 * a local variable array_merged incremented each time an array is merged.
 * 
 */
int *thread_done_queue;
int thread_done = 0;

typedef struct {
    int thread_index;
    int length;
    int *array;
    char *path;
    pthread_cond_t *cond;
    pthread_mutex_t *mutex;
} arg_t;

void *thread_job(void *);
void MergeSort(int *A, int N);
void print_array(int *array, int l);
void merge2array(int *srcA, int *srcB,int *ret, int lA, int lB);

int main(int argc, char **argv){
    pthread_t *tids;
    int n_threads;
    arg_t *args;
    int tot_len;
    int *final_array,*tmp;
    pthread_cond_t *cond;
    pthread_mutex_t *mutex;
    int array_merged = 0;
    
    
    setbuf(stdout,0);
    srand(getpid());
    


    if(argc <= 2){
        fprintf(stderr,"insufficient number of parameter\n");
        return 1;
    }

    n_threads = argc - 2 ;
    args = (arg_t *)malloc(sizeof(arg_t) * n_threads);
    tids = (pthread_t *)malloc(sizeof(pthread_t)*n_threads);
    thread_done_queue = (int *)malloc(sizeof(int)*n_threads);
    cond = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
    mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));

    pthread_cond_init(cond,NULL);
    pthread_mutex_init(mutex,NULL);
    


    for(int i = 0; i < n_threads; i++){
        args[i].cond = cond;
        args[i].mutex = mutex;
        args[i].path = argv[i+1];
        args[i].thread_index = i;
        pthread_create(&tids[i],NULL,thread_job,(void * )&args[i]);
        // #if DBG > 0
        // sleep(rand()%3);
        // #endif
    }

    tot_len = 0;
    tmp = NULL;
    

    for(int i=0; i< n_threads; i++ ){

      pthread_mutex_lock(mutex);
      while(array_merged >= thread_done) 
        pthread_cond_wait(cond,mutex);
      pthread_mutex_unlock(mutex);

      int curr_thread_arg = thread_done_queue[array_merged];

      #if DBG > 0
      fprintf(stdout,"main thread is merging the thread: %d\n",curr_thread_arg);
      #endif

      final_array = malloc(sizeof(int)*(tot_len + args[curr_thread_arg].length));



      merge2array(tmp,args[curr_thread_arg].array,final_array,tot_len,args[curr_thread_arg].length); 

      if(tot_len == 0) tmp = malloc(sizeof(int)*(tot_len + args[curr_thread_arg].length));
      else tmp = realloc(tmp,sizeof(int)*(tot_len + args[curr_thread_arg].length));
      
      tot_len += args[curr_thread_arg].length;
      memcpy(tmp,final_array,tot_len*sizeof(int));

      #if DBG > 0
      fprintf(stdout,"final ordered array:\t\t");
      print_array(final_array, tot_len);
      #endif
            
      
      array_merged++;
    }

    print_array(final_array, tot_len);


    /*  WAIT ALL THREADS */
    for(int i=0; i< n_threads; i++ )    pthread_join(tids[i],NULL);

    for(int i=0;i<n_threads;i++) free(args[i].array);
    free(tmp);
    free(final_array);   
    free(args);
    free(tids);
    return 0;
}

void *thread_job(void *arg){
    int *array;
    int *length = &((arg_t *)arg)->length;
    int fd = open(((arg_t *)arg)->path,O_RDONLY);
    pthread_cond_t *cond = ((arg_t *)arg)->cond;
    pthread_mutex_t *mutex = ((arg_t *)arg)->mutex;

    #if DBG > 0
    sleep(3+rand()%3);
    #endif

    /* READING FILE */
    read(fd,length,sizeof(int));
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

    pthread_mutex_lock(mutex);
    thread_done_queue[thread_done++] = ((arg_t *)arg)->thread_index;
    #if DBG > 0
    fprintf(stdout,"thread %d is incrementing the condition variable to: %d\n",((arg_t *)arg)->thread_index, thread_done);
    #endif
    pthread_cond_signal(cond);
    pthread_mutex_unlock(mutex);

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
    for(int i=0;i<l;i++) fprintf(stdout,"%d ",array[i]);
    fprintf(stdout,"\n");
}

void merge2array(int *srcA, int *srcB,int *ret, int lA, int lB){
  int posA = 0, posB = 0;

  while(posA + posB < lA + lB){
    if(posA >= lA){
      ret[posA+posB] = srcB[posB];
      posB ++;
    }else if(posB >= lB)   {
      ret[posA+posB] = srcA[posA];
      posA++;
    }else if(srcA[posA] < srcB[posB])  {
      ret[posA+posB] = srcA[posA];
      posA++;
    }else if(srcA[posA] >= srcB[posB])   {
      ret[posA+posB] = srcB[posB];
      posB++;
    }
  }


} 