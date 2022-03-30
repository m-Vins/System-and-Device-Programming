#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
#include <aio.h>
#include <signal.h>


#define WAIT_TIME_1 1
#define WAIT_TIME_2 5
#define STR_SIZE 16
#define STR_NUM 48

void fill_aio_struct(int fd,char *buf, struct aiocb *aio_arg,void *signal_routine);
void signal_routine1(sigval_t sigval);
void signal_routine2(sigval_t sigval);
void str_toupper(char *str, int l);

//global variables
int p1[2],p2[2];
int nr1 = 0,nr2 = 0;
char buffer_1[STR_SIZE];
char buffer_2[STR_SIZE];
struct aiocb *aiostruct_1,*aiostruct_2;

int main(){
    

    if(pipe(p1) != 0 || pipe(p2) != 0 ){
        fprintf(stderr,"pipe error\n");
        abort();
    }

    if(fork()){
        if(fork()){
            //P
            close(p1[1]);
            close(p2[1]);
 
            aiostruct_1 = calloc(1,sizeof(struct aiocb));
            aiostruct_2 = calloc(1,sizeof(struct aiocb));
            fill_aio_struct(p1[0],buffer_1,aiostruct_1,signal_routine1);
            fill_aio_struct(p2[0],buffer_2,aiostruct_2,signal_routine2);
            aio_read(aiostruct_1);
            aio_read(aiostruct_2);
            sleep(20);

        }else{
            //C2
            close(p2[0]);
            close(p1[0]);
            close(p1[1]);

            int nw = 0;
            
            while(1){
                sprintf(buffer_2,"A string [p2]");
                sleep(WAIT_TIME_2);
                nw += write(p2[1],buffer_2,STR_SIZE);

                if(nw >= STR_NUM){
                    exit(0);
            }
            }
        }
    }else{
        //C1
        close(p1[0]);
        close(p2[0]);
        close(p2[1]);
        
        int nw = 0;

        while(1){


            sprintf(buffer_1,"A string [p1]");
            sleep(WAIT_TIME_1);
            nw += write(p1[1],buffer_1,STR_SIZE);
            
            if(nw >= STR_NUM){
                exit(0);
            }
        }
    }

    return 0;
}



void signal_routine1(sigval_t sigval){
    nr1 += STR_SIZE;
    
    str_toupper(buffer_1,strlen(buffer_1));
    fprintf(stdout,buffer_1,STR_SIZE);
    fprintf(stdout,"\n");

    if(nr1<STR_NUM) {   
        fill_aio_struct(p1[0],buffer_1,aiostruct_1,signal_routine1);
        aio_read(aiostruct_1);
    }
}

void signal_routine2(sigval_t sigval){
    nr2 += STR_SIZE;
    
    str_toupper(buffer_2,strlen(buffer_2));
    fprintf(stdout,buffer_2,STR_SIZE);
    fprintf(stdout,"\n");

    if(nr2<STR_NUM) {    
        fill_aio_struct(p2[0],buffer_2,aiostruct_2,signal_routine2);    
        aio_read(aiostruct_2);
    }
}


void str_toupper(char *str, int l){
    for(int i=0;i<l;i++){
        if(str[i] == '\0')
            return;
        str[i] = toupper(str[i]);
    }
}


void fill_aio_struct(int fd,char *buf, struct aiocb *aio_arg,void *signal_routine){
    aio_arg->aio_fildes = fd;
    aio_arg->aio_offset = 0;
    aio_arg->aio_buf = (void *)buf;
    aio_arg->aio_nbytes = STR_SIZE;
    aio_arg->aio_sigevent.sigev_notify = SIGEV_THREAD;
    aio_arg->aio_sigevent.sigev_notify_function = signal_routine;
    aio_arg->aio_sigevent.sigev_notify_attributes = NULL ;
    aio_arg->aio_sigevent.sigev_value.sival_ptr = aio_arg;

}