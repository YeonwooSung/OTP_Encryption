#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    printf("encryptedMessenger: argv[0] =%s\n", argv[0]);

    if (argc == 1) {
        printf("otpEncryption error!\n");
        printf("usage: my_otp [-i infile] [-o outfile] -k keyfile\n");
        exit(1);
    }

    int oFlag = 0; // to check if the process uses the stdout stream or not.
    FILE *out = stdout;

    if (strcmp(argv[0], "std") != 0) {
        oFlag = 1;
        out = fopen(argv[0], "wb");
    }

    char *myfifo = "otpfifo";
    mkfifo(myfifo, 0666); //make the fifo to communicate with other process.

    int fd = open(myfifo, O_RDONLY); //open the fifo with read only option.

    char *buffer = (char *) malloc(9999); //allocate 9999 bytes to buffer to avoid overflow problem
    read(fd, buffer, 9999); //read encrypted text from fifo.

    fputs(buffer, out);

    if (oFlag == 1) {
        fclose(out);
    }

    close(fd);
    free(buffer);
    return 0;
}
