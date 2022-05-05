#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/ipc.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAXN 10
#define BUFFSIZE MAXN
#define DBG 1
#define FIFOPATH "fifo"


void getRandomString(char *str,int n);
void print_msg(char *msg, int n);

int main(void){
    int p1[2],p2[2];
    int n;
    int fd;
    char buffer[BUFFSIZE];

    setbuf(stdout,0);
    mkfifo(FIFOPATH,0666);
    

    if(pipe(p1) != 0 || pipe(p2)!= 0){
        fprintf(stderr,"pipe error\n");
        return 1;
    }

    if(fork()){
        //P1
        close(p1[0]);
        close(p2[1]);
        srand(getpid());
        fd = open(FIFOPATH,O_RDWR);
        
        
        while(1){
            n = rand()%MAXN;
            getRandomString(buffer,n);
            write(fd,(void *)buffer,n);
            fprintf(stderr,"write n by P1 :%d\t\tmessage:%s\n",n,buffer);
            sleep(1);
            write(p1[1],&n,sizeof(int));

            

            read(p2[0],&n,sizeof(int));
            read(fd,buffer,n);
            fprintf(stderr,"read n by P1   :%d\t\tmessage:%s\n",n,buffer);
            sleep(1);
            
            
        }

    }else{
        //P2
        close(p2[0]);
        close(p1[1]);
        srand(getpid());
        fd = open(FIFOPATH,O_RDWR);
        

        while(1){

            read(p1[0],&n,sizeof(int));
            read(fd,buffer,n);
            fprintf(stderr,"read n by P2   :%d\t\tmessage:%s\n",n,buffer);
            sleep(1);


            n = rand()%MAXN;
            getRandomString(buffer,n);
            write(fd,(void *)buffer,n);
            fprintf(stderr,"write n by P2 :%d\t\tmessage:%s\n",n,buffer);
            sleep(1);
            write(p2[1],&n,sizeof(int));
            

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
    for(int i=0;i<n-1;i++) str[i]=getRandomChar();
    str[n-1] = '\0';
}
