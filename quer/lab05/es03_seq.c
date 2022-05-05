#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define DBG 1


void v_mul_m(float *v, float **M, float *res, int n, int index);
void print_array(float *array, int l);

int main(int argc, char **argv){
    float *v1,*v2,**m;
    int n;
    float *tmp, res;
    
    srand(time(NULL));

    if(argc != 2){
        fprintf(stderr, "invalid number of parameters\n");
        return 1;
    }

    n = atoi(argv[1]);

    /*dinamic memory allocation*/
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
    fprintf(stdout,"array v1 : ");
    print_array(v1,n);
    fprintf(stdout,"array v2 : ");
    print_array(v2,n);
    fprintf(stdout,"matrix m :\n");
    for(int i=0;i<n;i++) print_array(m[i],n);
    fprintf(stdout,"\n");


    /* computing v1'*m    */
    for(int i=0;i<n;i++)
        v_mul_m(v1,m,tmp,n,i);
    
    #if DBG > 0
    print_array(tmp,n);
    #endif

    /* computing tmp*v2  */
    res = 0;
    for(int i=0;i<n;i++)
        res += tmp[i];
    

    fprintf(stdout,"\n\nres = %f\n",res);
    



    
    /*deallocating the memory*/
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