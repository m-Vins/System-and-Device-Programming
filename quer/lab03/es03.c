#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <sys/select.h>

#define WAIT_TIME_1 2
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

void set_fnctl (int fd, int flags) {
	int val;
	//GET FLAG
	if ((val = fcntl(fd, F_GETFL, 0)) < 0)
		{ 
            fprintf(stderr,"error getting file flags\n");
        }
	//MODIFY FLAG
	val |= flags;
	//val &= ~flags;

	//SET FLAG
	if (fcntl(fd, F_SETFL, val) < 0)
		{ 
            fprintf(stderr,"error setting file flags\n");
        }
}

int main(){
    int p1[2],p2[2];
    char buffer[STR_SIZE];

    setbuf(stdout,0);

    if(pipe(p1) != 0 || pipe(p2) != 0 ){
        fprintf(stderr,"pipe error\n");
        abort();
    }

    set_fnctl(p1[0],O_NONBLOCK);
    set_fnctl(p2[0],O_NONBLOCK);


    if(fork()){
        if(fork()){
            //P
            close(p1[1]);
            close(p2[1]);
            char buffer2[STR_SIZE];
            int n1 = 0, n2 = 0;
            int tmp;
            int nr = 0;
            int max_fd;

            if(p1[0]>p2[0]){
                max_fd = p1[0] + 1;
            }else{
                max_fd = p2[0] + 1;
            }

            fd_set rset;
            FD_ZERO(&rset);
            FD_SET(p1[0],&rset);
            FD_SET(p1[0],&rset);

            

            while(1){
                //in this way the select wait untill at least one fd in ready, then 
                //CPU is not busy in polling the two fd.
                select(max_fd,&rset,NULL,NULL,NULL);
                

                if((tmp = read(p2[0],&buffer2[n2],STR_SIZE-n2))>0){
                    n2 += tmp;
                    if(n2 == STR_SIZE){
                        str_toupper(buffer2,strlen(buffer2));
                        fprintf(stdout,buffer2,strlen(buffer2));
                        fprintf(stdout,"\n");
                        nr += n2;
                        n2 = 0;
                    }
                }

                if((tmp = read(p1[0],&buffer[n1],STR_SIZE-n1))>0){
                    n1 += tmp;
                    if(n1 == STR_SIZE){
                        str_toupper(buffer,strlen(buffer));
                        fprintf(stdout,buffer,strlen(buffer));
                        fprintf(stdout,"\n");
                        nr += n1;
                        n1 = 0;
                    }
                }


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
        close(p1[0]);
        close(p2[0]);
        close(p2[1]);
        
        int nw = 0;

        while(1){
 

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