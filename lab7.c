#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define DEFAULT_CAPACITY 100

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

char* buffMmap(int fildes, int* buffSize){
    errno = 0;
    struct stat fileSt;
    while (fstat(fildes, &fileSt) != 0){
        if(errno == EINTR){
            errno = 0;
            continue;
        }
        perror("Cannot obtain file state");
        return NULL;
    }

    *buffSize = fileSt.st_size;
    while (1){
        char* fp = mmap(NULL, *buffSize, PROT_READ,
                        MAP_PRIVATE, fildes, 0);
        if(fp == MAP_FAILED){
            if(errno == EINTR){
                errno = 0;
                continue;
            }
            perror("Cannot map file");
        }
        return fp;
    }
}

bool fillIndentTable(IndentTable* table, char const*  buff){
    char const* endLinePos = buff;

    while((endLinePos = strchr(endLinePos, '\n')) != NULL){
        pushIndent(table, endLinePos - buff);
        ++endLinePos;
    }
    return true;
}


void destroyIndentTable(IndentTable* table){
    if(table->arr != NULL){
        free(table->arr);
    }
}

void printLine(char const* buff, IndentTable* table, int lineNum){
    --lineNum;
    if(lineNum < 0 || lineNum >= table->size){
        printf("No line with such number\n");
        return;
    }

    size_t beginPos =  lineNum == 0
                       ? 0
                       : table->arr[lineNum-1] + 1; //+1 to get next pos after '\n'
    size_t length = table->arr[lineNum] - beginPos + 1; //+1 to include '\n'

    buff += beginPos;
    char* line = malloc(sizeof(char)*length);
    for(int i = 0; i < length; ++i){
        line[i] = buff[i];
    }
    printf("%s", line);
    free(line);
}

bool wait(){
    fd_set rfds;
    struct timeval tv;
    int retval;
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    retval = select(1, &rfds, NULL, NULL, &tv);
    if (retval){
        return true;
    }
    printf("Five seconds passed..:(\n");
    return false;
}

int main()
{
    int file = open("text.txt", O_RDONLY);

    if(file == -1)
    {
        perror("Cannot open file\n");
        return -1;
    }

    int bufferSize = 0;
    char* buff = buffMmap(file, &bufferSize);
    if(buff == NULL){
        if(close(file) == -1){
            perror("Cannot close file");
        }
        return 0;
    }

    if(close(file) == -1){
        perror("Cannot close file\n");
        if(munmap(buff, bufferSize) == -1){
            perror("Cannot munmap file");
        }
        return -1;
    }

    IndentTable table;

    if(!initIndentTable(&table)){
        perror("Error in initialization indent table");
        if(munmap(buff, bufferSize) == -1){
            perror("Cannot munmap file");
        }
        return -1;
    }
    if(!fillIndentTable(&table, buff)){
        destroyIndentTable(&table);
        if(munmap(buff, bufferSize) == -1){
            perror("Cannot munmap file");
        }
        perror("Error in filling indent table");
        return -1;
    }

    size_t lineNum;
    while(1)
    {
        printf("Type line num to read. Type 0 to exit: \n");

        if(wait()){
            scanf("%zd", &lineNum);
            if(lineNum != 0){
                printLine(buff, &table, lineNum);
            }
        }
        if(lineNum == 0){
            break;
        }
    }

    destroyIndentTable(&table);
    if(munmap(buff, bufferSize) == -1){
        perror("Cannot munmap file");
        return -1;
    }
    return 0;
}
