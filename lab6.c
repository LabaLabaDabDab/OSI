#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define DEFAULT_CAPACITY 100
#define BUFF_SIZE 16

typedef struct _IndentTable{
    size_t* arr;
    size_t size;
} IndentTable;

bool initIndentTable(IndentTable* table){
    table->arr = calloc(DEFAULT_CAPACITY, sizeof(size_t));
    if(table->arr == NULL){
        perror("Cannot allocate memory for indent array\n");
        return false;
    }

    table->size = 0;
    return true;
}

void pushIndent(IndentTable* table, size_t val){
    table->arr[table->size] = val;
    ++table->size;
}

bool fillIndentTable(IndentTable* table, int file){
    char buff[BUFF_SIZE + 1];
    size_t currPos = 0;
    ssize_t readCount = 0;
    
    while((readCount = read(file, buff, BUFF_SIZE)) != 0){
        if(readCount == -1){
            if(errno != EAGAIN && errno != EINTR){
                perror("Error in reading file\n");
                return false;
            }
            continue;
        }

        buff[readCount] = '\0';

        char* endLinePos = buff;
        
        while((endLinePos = strchr(endLinePos, '\n')) != NULL){
            pushIndent(table, currPos + (endLinePos - buff));
            ++endLinePos;
        }
        currPos += readCount;
    }

    return true;
}

void destroyIndentTable(IndentTable* table){
    if(table->arr != NULL){
        free(table->arr);
    }
}

void printLine(int fildes, IndentTable* table, int lineNum){
    --lineNum;
    if(lineNum < 0 || lineNum >= table->size){
        printf("No line with such number\n");
        return;
    }

    size_t beginPos =  lineNum == 0
                       ? 0
                       : table->arr[lineNum-1] + 1; //+1 to get next pos after '\n'
    size_t length = table->arr[lineNum] - beginPos + 1; //+1 to include '\n'

    char* line = calloc(length + 1, sizeof(char));
    if(line == NULL){
        perror("Cannot allocate memory to printLine buffer\n");
        return;
    }

    lseek(fildes, beginPos, SEEK_SET);
    read(fildes, line, length);
    printf("%s\n", line);
    free(line);

}

bool wait(int file){
    fd_set rfds;
    struct timeval tv;
    int retval;
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    retval = select(file + 1, &rfds, NULL, NULL, &tv);
    if (retval){
        return true;
    }
    printf("Five seconds passed..:(\n");
    return false;
}

int main(){
    int file = open("text.txt", O_RDONLY);
    if(file == -1){
        perror("Cannot open file\n");
        return -1;
    }

    IndentTable table;

    if(!initIndentTable(&table)){
        if(close(file)){
            perror("Error in closing file");
        }
        return -1;
    }
    
    if(!fillIndentTable(&table, file)){
        destroyIndentTable(&table);
        if(close(file)){
            perror("Error in closing file");
        }
        return -1;
    }
    
    size_t lineNum;
    
    while(1){
        printf("Type line num to read. Type 0 to exit: \n");
        if(wait(file)){
            scanf("%ld", &lineNum);
            if (lineNum != 0){
                printLine(file, &table, lineNum);
            }
        }
        if(lineNum == 0){
            break;
        }
    }

    destroyIndentTable(&table);
    
    if(close(file)){
        perror("Error in closing file");
    }
    
    return 0;
}
