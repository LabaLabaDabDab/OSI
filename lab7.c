#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>

#define BUFSIZE 10

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
	 struct timeval tv;
	 tv.tv_sec=5;
	 tv.tv_usec=0;
     char *displ[501];
     char *p,*f, buf[10];
     int fd1, fd2, count, i = 1, j = 1, line_no, line_ln[500]={0};
     off_t size;

     if ((fd1 = open("/dev/tty", O_RDONLY | O_NDELAY)) == -1) {
         printf("Error,file open console");
         return 0;
     }

     if ((fd2 = open(argv[1], O_RDONLY)) == -1) {
         printf("File open error,name %s",argv[1]);
		 closeFile(&fd1);
         return 0;
         }

     size = lseek(fd2, 0, SEEK_END);
	 if(size==-1){
		 closeFile(&fd1);
		 closeFile(&fd2);
		 return 0;
	 }
     if((p = mmap(0, size, PROT_READ, MAP_SHARED, fd2, 0))==MAP_FAILED){
		 printf("Error mmap");
		 closeFile(&fd1);
		 closeFile(&fd2);
		 return 0;
	 }
     displ[1] = p;
     for(count = 0; count < size; count++)
         if( *(p+count) == '\n' ) {
             line_ln[i++] = j;
             displ[i] = count+p+1;
             j = 1;
             }
         else
             j++;

     displ[i] = 0;
     while(1){
         printf("you have 5 seconds to enter a line number\n");
         if(!select(1,&fd1,NULL,NULL,&tv)){
			 writeConsole(p,size);
             break;
		 }
         else {
			 i = readFile(&fd1,&buf,BUFSIZE);
			 if(i==-1){
				 break;
			 }
             buf[i] = '\0';
             line_no = atoi(buf);
             if(line_no <= 0){
                 break;
			 }
             if(displ[line_no] != 0)
                 writeConsole(displ[line_no],line_ln[line_no]);
             else
                 printf("Bad Line Number\n");
         }
     }
	 if(!mummap(p,size)){
		 printf("Error mummap");
	 }
	 closeFile(&fd1);
	 closeFile(&fd2);
}
