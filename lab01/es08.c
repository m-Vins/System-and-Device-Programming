#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

typedef struct arg
{
    int **A;
    int **B;
    int **C; // result matrix
    int l;   // number of column of the first matrix
    int x;   // target row
    int y;   // target column
} arg_t;

void mat_mul(int **A, int **B, int r, int x, int c, int **C);
int **allocate_matrix(int r, int c);
void free_matrix(int **M, int rows);
void read_matrix(FILE *fp, int **M, int r, int c);
void print_matrix(int **M, int r, int c);

int main(int argc, char **argv)
{
    int r, x, c;
    int **A;
    int **B;
    int **C;
    FILE *fp;

    /*********************************
     * READING THE FILE
     ********************************/
    fp = fopen("es08.txt", "r");
    fscanf(fp, "%d\n%d\n%d\n", &r, &x, &c);
    printf("r : %d\nx : %d\nc : %d\n\n", r, x, c);

    /*******************************
     * MATRIX ALLOCATION
     ******************************/
    A = allocate_matrix(r, x);
    B = allocate_matrix(x, c);

    read_matrix(fp, A, r, x);
    read_matrix(fp, B, x, c);

    print_matrix(A, r, x);
    print_matrix(B, x, c);

    C = allocate_matrix(r, c);

    print_matrix(C, r, c);
    mat_mul(A, B, r, x, c, C);

    print_matrix(C, r, c);

    free_matrix(A, r);
    free_matrix(B, x);
    free_matrix(C, c);
    return 0;
}

void *thread_function(void *arg)
{
    arg_t *data = (arg_t *)arg;
    int sum = 0;

    for (int i = 0; i < data->l; i++)
    {
        sum += data->A[data->x][i] * data->B[i][data->y];
    }
    data->C[data->x][data->y] = sum;
    pthread_exit(NULL);
}

int **allocate_matrix(int r, int c)
{
    int **M = (int **)malloc(r * sizeof(int *));
    for (int j = 0; j < c; j++)
    {
        M[j] = (int *)malloc(sizeof(int));
    }
    return M;
}

void free_matrix(int **M, int rows)
{
    for (int j = 0; j < rows; j++)
    {
        M[j] = (int *)malloc(sizeof(int));
    }
    free(M);
}

void read_matrix(FILE *fp, int **M, int r, int c)
{
    for (int i = 0; i < r; i++)
    {
        for (int j = 0; j < c; j++)
        {
            fscanf(fp, "%d", &M[i][j]);
        }
        fscanf(fp, "\n");
    }
}

void print_matrix(int **M, int r, int c)
{
    for (int i = 0; i < r; i++)
    {
        for (int j = 0; j < c; j++)
        {
            printf("%d\t", M[i][j]);
        }
        printf("\n");
    }
    printf("\n");
    printf("\n");
}

void mat_mul(int **A, int **B, int r, int x, int c, int **C)
{
    pthread_t tids[r * c];
    arg_t arguments[r * c];

    printf("calculating A*B with r: %d, c: %d, x:%d\n", r, c, x);

    for (int i = 0; i < r * c; i++)
    {
        arguments[i].A = A;
        arguments[i].B = B;
        arguments[i].C = C;
        arguments[i].l = x;
        arguments[i].x = i / c;
        arguments[i].y = i % c;
        printf("throwing a thread with for row: %d, column: %d\n", i / c, i % c);
        pthread_create(&tids[i], NULL, thread_function, (void *)&arguments[i]);
    }
    for (int t = 0; t < r * c; t++)
    {
        pthread_join(tids[t], NULL);
    }
}