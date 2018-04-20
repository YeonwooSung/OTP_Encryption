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

    char *myfifo = "/temp/otpfifo";
    mkfifo(myfifo, 0666); //make the fifo to communicate with other process.

    int fd = open(myfifo, O_RDONLY | O_NONBLOCK); //open the fifo with read only option.

    char *buffer = (char *) malloc(4500); //allocate 9999 bytes to buffer to avoid overflow problem
    read(fd, buffer, 4000); //read encrypted text from fifo.

    fputs(buffer, out);

    if (oFlag == 1) {
        fclose(out);
    }

    close(fd);
    free(buffer);
    return 0;
}
