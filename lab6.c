#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define BUFSIZE 257
#define BUFSUZE_C 100

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



int writeConsole(char *buf,int whence){
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
     long displ[500]={0};
	 int countAll=1;
     int fd1, fd2, i = 1, j = 0, line_no, line_ln[500]={0};
     char c[BUFSUZE_C], buf[BUFSIZE];

     if ((fd1 = open("/dev/tty", O_RDONLY | O_NDELAY)) == -1) {
         printf("Error,open file console");
         return 1;
     }

     if ((fd2 = open(argv[1], O_RDONLY)) == -1) {
         printf("Error open -",argv[1]);
		 closeFile(&fd1);
         return 1;
     }
	 int maxfdid;
	 if(fd1>fd2){
		 maxfdid=fd1;
	 }ekse{
		 maxfdid=fd2;
	 }
	 int count;
     while(count=readFile(&fd2,&c,BUFSUZE_C))
		for(int k=0;k<count;k++){
			 if( c[k] == '\n' ) {
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
		
	 if(count==-1){
        closeFile(&fd1);
		closeFile(&fd2);
        return 0;
     }

     while(1){
         printf("you have 5 seconds to enter a line number\n");
         if (!select(maxfdid+1,&fd1,NULL,NULL,&tv)) {
             if(lseek(fd2, SEEK_SET, 0)==-1){
				 break;
			 }
             while((i = readFile(&fd2,&buf,BUFSIZE)) > 0){
                 if(writeConsole(buf,i)){
					 break;
				 }
			 }
			 break;
         }
         else {
			 i = readFile(&fd1,&buf,BUFSIZE);
			 if(i==-1){
				 break;
			 }
             buf[i] = '\0';
             line_no = atoi(buf);
             if(line_no <= 0)
                 break;
			 if(lseek(fd2, displ[line_no], 0)==-1)
				 break;
             if(count=readFile(&fd2,&buf,line_ln[line_no])){
                 if(writeConsole(buf,line_ln[line_no])){
					 break;
				 }
			 }
             else
				 if(count==0){
                 printf("Bad Line Number\n");
				 }
				 else{
					 break;
				 }
         }
     }
	 closeFile(&fd1);
	 closeFile(&fd2);
	 return 0;
}
