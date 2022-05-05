#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <ctype.h>

#define MAXN 10
#define SHMEMSIZE MAXN


void getRandomString(char *str,int n);
void print_msg(char *msg, int n);

int main(void){
    key_t key;
    int p1[2],p2[2];
    int shmid;
    char *data;
    int n;

    setbuf(stdout,0);
    
    
    key = ftok("es01",13);
    

    if(pipe(p1) != 0 || pipe(p2)!= 0){
        fprintf(stderr,"pipe error\n");
        return 1;
    }

    if(fork()){
        //P1
        close(p1[0]);
        close(p2[1]);
        srand(getpid());
        shmid = shmget(key,SHMEMSIZE,IPC_CREAT | 0644);
        data = shmat(shmid,NULL,0);
        
        while(1){
            n = rand()%MAXN;
            getRandomString(data,n);
            fprintf(stderr,"write n by P1 :%d\t\tmessage:",n);
            print_msg(data,n);
            //sleep(1);
            write(p1[1],&n,sizeof(int));
            if(n==0) return 0;

            

            read(p2[0],&n,sizeof(int));
            if(n==0) return 0;
            fprintf(stderr,"read n by P1   :%d\t\tmessage:",n);
            print_msg(data,n);
            //sleep(1);
            
            
        }

    }else{
        //P2
        close(p2[0]);
        close(p1[1]);
        srand(getpid());
        shmid = shmget(key,SHMEMSIZE,IPC_CREAT | 0644);
        data = shmat(shmid,NULL,0);
        

        while(1){

            read(p1[0],&n,sizeof(int));
            if(n==0) return 0;
            fprintf(stderr,"read n by P2   :%d\t\tmessage:",n);
            print_msg(data,n);
            //sleep(1);


            n = rand()%MAXN;
            getRandomString(data,n);
            fprintf(stderr,"write n by P2 :%d\t\tmessage:",n);
            print_msg(data,n);
            //sleep(1);
            write(p2[1],&n,sizeof(int));
            if(n==0) return 0;
            

        }
        

    }

    return 0;
}

char getRandomChar(void){
    char c = rand()%28;
    if(c==26) return ' ';
    if(c==27) return '\n';
    return c+'a';
}


void getRandomString(char *str,int n){
    for(int i=0;i<n;i++) str[i]=getRandomChar();

}


void print_msg(char *msg, int n){
    for(int i=0;i<n;i++) fprintf(stdout,"%c",msg[i]);
    fprintf(stdout,"\n");
}