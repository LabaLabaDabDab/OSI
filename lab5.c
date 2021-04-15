#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
     long fileOffsets[101];
     int fd = 0, i = 1, j = 0, lineNumber = 0, lineLength[101] = { -1 };
     char c = 0;
     char *buf = (char*)malloc (sizeof(char) * 257);
     int bufSize = 257;
     static char err_msg[32] = "Input file doesn`t exist -  ";
     if (argc < 2) {
         perror("Usage: filename as argument");
         exit(2);
     }
     if((fd = open(argv[1], O_RDONLY)) == -1) {
         perror(strcat(err_msg, argv[1]));
         exit(1);
     }
     fileOffsets[0] = 0L;
     fileOffsets[1] = 0L;
     while(read(fd, &c, 1)){
         if(c == '\n') {
             j++;
             lineLength[i++] = j;
             fileOffsets[i] = lseek(fd, 0L, SEEK_CUR);
             j = 0;
         }
         else
             j++;
     }

     while(printf("enter line number : ") && scanf("%d", &lineNumber)) {
         if(lineNumber == 0)
             exit(0);
         if(lineNumber < 0 || lineNumber > 100 || (lineLength[lineNumber] == -1)) {
	     fprintf(stderr, "wrong line number \n");
             continue;
         }
	 lseek(fd, fileOffsets[lineNumber], SEEK_SET);
	 if (lineLength[lineNumber] > bufSize) {
	     realloc (buf, lineLength[lineNumber] * sizeof(char));
             bufSize = lineLength[lineNumber];
         } 
         if(read(fd, buf, lineLength[lineNumber]))
             write(1, buf, lineLength[lineNumber]);
         else
             fprintf(stderr, "wrong line number\n");
     }
     free(buf);
     return 0;
 }
