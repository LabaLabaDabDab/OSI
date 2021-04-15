#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
 
#define BUFSIZE 257
#define NOT_TIMEOUT 1

 
int main(int argc, char *argv[])
 {
      struct timeval currentTime;      
      long fileOffsets[101];
      int fd1, fd2, i = 1, j = 0, lineNumber, lineLength[101] = { -1 };
      char c;
      char *buf = (char*)malloc (sizeof(char) * 257);
      int bufSize = 257;
      static char err_msg[32] = "Input file - ";
 
      if ((fd1 = open("/dev/tty", O_RDONLY | O_NDELAY)) == -1) {
          perror("/dev/tty");
          exit(1);
      }
 
      if ((fd2 = open(argv[1], O_RDONLY)) == -1) {
          perror(strcat(err_msg, argv[1]));
          exit(1);
          }
 
      fileOffsets[1] = 0L;
      while(read(fd2, &c, 1)){
          if( c == '\n' ) {
              j++;
              lineLength[i++] = j;
              fileOffsets[i] = lseek(fd2, 0L, 1);
              j = 0;
              }
          else
              j++;
	}
	printf("five seconds to write a line number\n");
	int timeCountedFrom = 0;
	gettimeofday(&currentTime, NULL);
	timeCountedFrom = currentTime.tv_sec;
	while (NOT_TIMEOUT) {
		gettimeofday(&currentTime, NULL);
		if((currentTime.tv_sec - timeCountedFrom) >= 5){
			lseek(fd2, 0, SEEK_SET);
             		while((i = read(fd2, buf, BUFSIZE)) > 0)
             			write(1, buf, i);
			break; 
		}
		if ((i = read (fd1, buf, BUFSIZE)) != 0) {
			buf[i] = '\0'; 
			lineNumber = atoi(buf);
			if(lineNumber == 0)
				exit(0);
			if(lineNumber < 0 || lineNumber > 100 || lineLength[lineNumber] == -1){
				fprintf(stderr, "wrong line number\n");
				continue;
			}
			lseek(fd2, fileOffsets[lineNumber], SEEK_SET);
			if (lineLength[lineNumber] > bufSize) {
				realloc(buf, lineLength[lineNumber] * sizeof(char));
				bufSize = lineLength[lineNumber];
			}
			if(read(fd2, buf, lineLength[lineNumber]) > 0){
				write(1, buf, lineLength[lineNumber]);
			}
			else {
				fprintf(stderr, "wrong line number\n");
			}
			gettimeofday(&currentTime, NULL);
			timeCountedFrom = currentTime.tv_sec;
		}
	}
      return 0;
}
