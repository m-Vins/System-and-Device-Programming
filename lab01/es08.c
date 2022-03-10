#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

typedef struct arg
{
    int **A;
    int **B;
    int **C;
    int l;
    int x;
    int y;
} arg_t;

void mat_mul(int **A, int **B, int r, int x, int c, int **C);
int **allocate_matrix(int r, int c);
void free_matrix(int **M, int rows);
void read_matrix(FILE *fp, int **M, int r, int c);

int main(int argc, char **argv)
{
    int r, x, c;
    int **A;
    int **B;
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

    free_matrix(A, r);
    free_matrix(B, x);
    return 0;
}

void thread_function(void *arg)
{
    arg_t *data = (arg_t *)arg;
    int sum = 0;

    for (int i = 0; i < data->l; i++)
    {
        sum += data->A[data->x][i] * data->B[i][data->y];
    }
    data->C[data->x][data->y] = sum;
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
            printf("%d\t", M[i][j]);
        }
        fscanf(fp, "\n");
        printf("\n");
    }
    printf("\n");
    printf("\n");
}

void mat_mul(int **A, int **B, int r, int x, int c, int **C)
{
}