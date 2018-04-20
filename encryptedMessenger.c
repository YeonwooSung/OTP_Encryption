#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    printf("encryptedMessenger: argv[0] =%s\n", argv[0]);

    int oFlag = 0; // to check if the process uses the stdout stream or not.
    FILE *out = stdout;

    if (strcmp(argv[0], "std") != 0) {
        oFlag = 1;
        out = fopen(argv[0], "wb");
    }

    char *myfifo = "./otpfifo";

    int fd;

    if ( (fd = open(myfifo, O_RDONLY | O_NONBLOCK) ) < 0) { //open the fifo with read only option.
        perror("mkfifo failed");
    }

    char *buffer = (char *) malloc(2000); //allocate 9999 bytes to buffer to avoid overflow problem
    size_t bytes = read(fd, buffer, 1900); //read encrypted text from fifo.

    printf("read %d bytes\n", bytes);
    printf("Test\n%s\nTestend\n\n", buffer);

    while (*buffer == EOF) {
        printf("%d\n", *buffer);
        buffer+=1;
    }

    fputs(buffer, out);

    if (oFlag == 1) {
        fclose(out);
    }

    close(fd);
    free(buffer);
    return 0;
}
