#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <string.h>

#define WAIT_TIME_1 3
#define WAIT_TIME_2 1
#define STR_SIZE 16
#define STR_NUM 32


void str_toupper(char *str, int l){
    for(int i=0;i<l;i++){
        if(str[i] == '\0')
            return;
        str[i] = toupper(str[i]);
    }
}

int main(){
    int p1[2],p2[2];
    char buffer[STR_SIZE];

    if(pipe(p1) != 0 || pipe(p2) != 0 ){
        fprintf(stderr,"pipe error\n");
        abort();
    }

    if(fork()){
        if(fork()){
            //P
            close(p1[1]);
            close(p2[1]);

            int nr = 0;

            while(1){
                nr += read(p1[0],buffer,STR_SIZE);
                str_toupper(buffer,strlen(buffer));
                fprintf(stdout,buffer,STR_SIZE);
                fprintf(stdout,"\n");

                nr += read(p2[0],buffer,STR_SIZE);
                str_toupper(buffer,strlen(buffer));
                fprintf(stdout,buffer,STR_SIZE);
                fprintf(stdout,"\n");
                

                if(nr >= 2*STR_NUM)
                    exit(0);
            }



        }else{
            //C2
            close(p2[0]);
            close(p1[0]);
            close(p1[1]);

            int nw = 0;
            
            while(1){
                sprintf(buffer,"A string [p2]");
                sleep(WAIT_TIME_2);
                nw += write(p2[1],buffer,STR_SIZE);

                if(nw >= STR_NUM){
                    exit(0);
            }
            }
        }
    }else{
        //C1
        int nw = 0;

        while(1){
            close(p1[0]);
            close(p2[0]);
            close(p2[1]);

            sprintf(buffer,"A string [p1]");
            sleep(WAIT_TIME_1);
            nw += write(p1[1],buffer,STR_SIZE);

            if(nw >= STR_NUM){
                exit(0);
            }
        }
    }

    return 0;
}