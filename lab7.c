#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>

#define N 100
#define True 1

int main (int argc, char *argv[]) {
	struct timeval tv1;
	long fileOffsets[N] = {0};
	int fileDescriptor;
	int lineLength[N] = {-1};

	if ((fileDescriptor = open ("test.txt", O_RDONLY)) == -1) {
		perror ("File doesn't open");
		exit (1);
	}

	int fileSize = lseek (fileDescriptor, 0, SEEK_END);
	char *p = mmap (0, fileSize, PROT_READ, MAP_SHARED, fileDescriptor, 0);
	if (p == MAP_FAILED) {
		perror ("Error mmap");
		exit (1);
	}

	int i = 1, j = 1;
	fileOffsets[1] = p;
	for (int count = 0; count < fileSize; count++) {
		if (*(p + count) == '\n') {
			lineLength[i++] = j;
			fileOffsets[i] = count + p + 1;
			j = 1;
		} else {
			j++;
		}
	}

	fileOffsets[i] = 0;
	int lineNumber = 0;
	int retval;
	tv1.tv_sec = 5;
	tv1.tv_usec = 0;
	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(fileno (stdin), &readfds);
	while (True) {
		retval = select (fileno (stdin) + 1, &readfds, NULL, NULL, &tv1);
		if (retval == 0) {
			break;
		} else if (retval == -1) {
			fprintf (stderr, "Select error");
			if (close (fileDescriptor) != 0) {
				fprintf (stderr, "Cannot close file (descriptor=%d)\n",
						 fileDescriptor);
			}
			munmap (p, fileSize);
			exit (1);
		}
		scanf ("%d", &lineNumber);
		if (!lineNumber) {
			if (close (fileDescriptor) != 0) {
				fprintf (stderr, "Cannot close file (descriptor=%d)\n",
						 fileDescriptor);
				munmap (p, fileSize);
				exit (1);
			}
			munmap (p, fileSize);
			exit (0);
		}

		if (lineNumber < 0 || lineNumber > (N - 1) || (fileOffsets[lineNumber
			+ 1] == 0)) {
			fprintf (stderr, "wrong line number \n");
			continue;
		}

		int w = -1;
		while (w != lineLength[lineNumber]) {
			w = write (STDOUT_FILENO, fileOffsets[lineNumber],
					   lineLength[lineNumber]);
			if (w == -1) {
				if (errno == EAGAIN || errno == EINTR) {
					continue;
				} else {
					w = lineLength[lineNumber];
					fprintf (stderr, "Error write occurred\n");
				}
			}
		}
	}

	int w = -1;
	while (w != fileSize) {
		w = write (STDOUT_FILENO, p, fileSize);
		if (w == -1) {
			if (errno == EAGAIN || errno == EINTR) {
				continue;
			} else {
				w = lineLength[lineNumber];
				fprintf (stderr, "Error write occurred\n");
			}
		}
	}

	if (close (fileDescriptor) != 0) {
		fprintf (stderr, "Cannot close file (descriptor=%d)\n",
				 fileDescriptor);
		munmap (p, fileSize);
		exit (1);
	}
	munmap (p, fileSize);
	exit (0);
}
