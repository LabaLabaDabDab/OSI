#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define BUFSIZE_C 100

void closeFile(int *fd){
    if(close(*fd)==-1){
        if(errno==EINTR){
            if(close(*fd)==-1&&errno!=EINTR){;
                printf("Error close file");
            }
        }else{
            printf("Error close file");
        }
    }
}

int writeConsole(char buf[],int whence){
	if(write(1,buf,whence)==-1){
		if(errno==EINTR){
			if(write(1,buf,whence)==-1&&errno!=EINTR){
				printf("Error write");
				return 1;
			}
		}else{
			printf("Error write");
			return 1;
		}
	}
	return 0;
}

int readFile(int *fd,char *buf,int size_buf){
    int count;
	if((count=read(*fd,buf,size_buf))==-1){
		if(errno==EINTR){
			if((count=read(*fd,buf,size_buf))==-1&&errno!=EINTR){
				printf("Error read");
				return -1;
			}
		}else{
			printf("Error read");
			return -1;
		}
	}
	return count;
}

int main(int argc, char *argv[]){
     int fd;
     long displ[500]={0};
     int countAll=1;
     int i = 1, j = 0, line_no, line_ln[500]={0};
     char c[BUFSIZE_C], buf[257];
     if(( fd =  open(argv[1], O_RDONLY)) == -1){
         printf("No find file- %s",argv[1]);
         return 1;
     }
	 int count;
     while(count=readFile(&fd,&c,BUFSIZE_C)){
		 for(int k=0;k<count;k++){
             if( c[k] == '\n') {
                 j++;
                 line_ln[i++] = j;
                 displ[i] = countAll;
                 j = 0;
             }
             else
                 if(c[k]!='\0'){
                 j++;
                 }
             if(c[k]!='\0'){
                countAll++;
             }
		 }
     }
     if(count==-1){
        closeFile(&fd);
        return 0;
     }
     while( printf("Line number : ") && scanf("%d", &line_no)) {
         if(line_no <= 0)
             break;
         if(lseek(fd, displ[line_no], 0)==-1){
			 break;
		 }
         if(count=readFile(&fd,&buf,line_ln[line_no])){
            if(writeConsole(buf,line_ln[line_no])){
				break;
			}
         }
         else
            if(count==0){
              printf("Bad Line Number\n");
            }else{
                break;
            }
         }
     closeFile(&fd);
}
