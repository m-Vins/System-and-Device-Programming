#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

#define DBG 1


float *v1,*v2,**m,*tmp;
float res;
int n;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
char thread_already_done = 0;

void v_mul_m(float *v, float **M, float *res, int n, int index);
void print_array(float *array, int l);
void *thread_task(void *arg);

int main(int argc, char **argv){
    int *args;
    
    pthread_t *tids;
    
    srand(time(NULL));

    if(argc != 2){
        fprintf(stderr, "invalid number of parameters\n");
        return 1;
    }

    n = atoi(argv[1]);

    /*dinamic memory allocation*/
    args = malloc(sizeof(int)*n);
    tids = malloc(sizeof(pthread_t)*n);
    v1 = malloc(sizeof(float)*n);
    v1 = malloc(sizeof(float)*n);
    v2 = malloc(sizeof(float)*n);
    tmp = malloc(sizeof(float)*n);
    m = malloc(sizeof(float *)*n);
    for(int i=0;i<n;i++) m[i] = malloc(sizeof(float)*n);


    /*generating random values*/
    for(int i=0;i<n;i++){
        v1[i] = (float)rand()/(RAND_MAX) - 0.5;
        v2[i] = (float)rand()/(RAND_MAX) - 0.5;
        for(int j=0;j<n;j++) m[i][j] = (float)rand()/(RAND_MAX) - 0.5;
    }

    /* printing the generated values*/
    fprintf(stdout,"generating the random values..\n");
    #if DBG > 0
    fprintf(stdout,"array v1 : ");
    print_array(v1,n);
    fprintf(stdout,"array v2 : ");
    print_array(v2,n);
    fprintf(stdout,"matrix m :\n");
    for(int i=0;i<n;i++) print_array(m[i],n);
    fprintf(stdout,"\n");
    #endif


    /* computing v1'*m    */
    for(int i=0;i<n;i++){
        args[i] = i;
        pthread_create(&tids[i],NULL,thread_task,(void *)&args[i]);
    }
    
    for(int i=0;i<n;i++)    pthread_join(tids[i],NULL);
    


    fprintf(stdout,"\n\nres = %f\n",res);
    



    
    /*deallocating the memory*/
    free(args);
    free(tids);
    for(int i=0;i<n;i++) free(m[i]);
    free(m);
    free(tmp);
    free(v1);
    free(v2);
    return 0;
}

void print_array(float *array, int l){
    for(int i=0;i<l;i++) fprintf(stdout,"%f ",array[i]);
    fprintf(stdout,"\n");
}


void v_mul_m(float *v, float **M, float *res, int n, int index){
    res[index] = 0;
    for(int i=0;i<n;i++){
        res[index] += v[i] * M[index][i];
    }
}

void *thread_task(void *arg){
    int index = *((int *)arg);

    v_mul_m(v1,m,tmp,n,index);

    pthread_mutex_lock(&mutex);
    thread_already_done ++ ;
    #if DBG > 0
    fprintf(stdout,"thread %d is incrementing the counter\n",index);
    #endif
    if(thread_already_done == n){
        /*this thread is gonna compute tmp*v2  */
            
        #if DBG > 0
        print_array(tmp,n);
        #endif

        res = 0;
        for(int i=0;i<n;i++)
            res += tmp[i];
    }
    pthread_mutex_unlock(&mutex);
    

    pthread_exit(NULL);
}