#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

int read_file(int fileDescriptor, char* p, size_t size) {
    int number = 0;
    while(number == 0) {
        number = read(fileDescriptor, p, size);
        if (errno == EAGAIN || errno == EINTR) {
            number = 0;
        } else {
            return number;
        }
    }
}

int main() {
    long fileOffsets[101];
    int fileDescriptor;
    int line_number = 0;
    int line_length[101] = {-1};
    int buffer_size = 257;
    char* buffer = (char*)malloc(sizeof(char) * buffer_size);

    if(!buffer) {
        fprintf(stderr, "memory allocation error\n");
        return 0;
    }

    char* file = (char*)malloc(sizeof(char) * buffer_size);

    if (!file){
        free(buffer);
        fprintf(stderr, "memory allocation error \n");
        return 0;
    }

    if ((fileDescriptor = open("test.txt", O_RDONLY)) == -1) {
        fprintf(stderr, "File doesn't open \n");
        free(buffer);
        return 0;
    }
    fileOffsets[0] = 0;
    fileOffsets[1] = 0;
    int old_offset = 0, i = 1, j = 0;
    int number = read_file(fileDescriptor, file, buffer_size);

    for (int k = 0; k < number; ++k) {
        j++;
        if (file[k] == '\n') {
            line_length[i] = j;
            fileOffsets[++i] = j + old_offset;
            old_offset += j;
            j = 0;
        }
    }

    while (1) {
        printf("line number: ");
        scanf("%d", &line_number);

        if (line_number == 0) {
            break;
        }

        if (line_number < 0 || line_number > 100 || (line_length[line_number] == -1)) {
            fprintf(stderr, "wrong line number \n");
            continue;
        }

        lseek(fileDescriptor, fileOffsets[line_number], SEEK_SET);

        if (line_length[line_number] > buffer_size) {
            realloc(buffer, line_length[line_number] * sizeof(char));
            buffer_size = line_length[line_number];
        }

        if (read(fileDescriptor, buffer, line_length[line_number])) {
            write(1, buffer, line_length[line_number]);
        } else {
            fprintf(stderr, "wrong line number\n");
        }
    }
    free(buffer);

    if (close(fileDescriptor) == -1) {
        fprintf(stderr, "error with closing of session descriptor\n");
    }
    return 0;
}
