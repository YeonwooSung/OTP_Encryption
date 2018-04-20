#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Reads the user input and encrypts it.
void readAndEncryptTheUserInput(char *inFile, char *keyFile) {
    FILE *in = stdin;
    printf("readInputAndEncrypt function starts\n");

    int iFlag = 0; // to check if the process uses the stdin stream or not.

    if (strcmp(inFile, "std") != 0) {
        in = fopen(inFile, "rb");
        iFlag = 1;
    }

    //Get the file size to avoid the overflow.
    fseek(in, 0, SEEK_END); //seeks the end of the file.status
    int fileSize = ftell(in); //gets the size of the file.File descriptor is integer that uniquely identifies an open file of the process.


    rewind(in);

    char *buffer = (char *) malloc(fileSize); //buffer to store the encrypted texts.

    FILE *key = fopen(keyFile, "rb");

    char inputChar;
    char keyChar;

    char *myfifo = "./otpfifo"; //the file path of the FIFO
    mkfifo(myfifo, 0666); //creating the named pipe FIFO with the permission 0666.

    int fd;
    fd = open(myfifo, O_WRONLY); //open the fifo to write only.

    printf("1\n");

    while ((inputChar = fgetc(in)) != EOF) { //read the inputfclose(in); //close the opened file until the fgetc finish reading all inputs

        if ( (keyChar = fgetc(key)) == EOF ) { //check if the program reaches to the end of the key file.
            rewind(key); //rewind to the beginning of the key file.
            keyChar = fgetc(key); //read the character once more.
        }

        char encryptedChar = (inputChar ^ keyChar); //do the xor operation to encrypt the text.
        printf("%c\n", inputChar);

        *buffer++ = encryptedChar;
    }
    printf("2\n");

    write(fd, buffer, (fileSize + 1)); //write the encrypted text to the fifo.

    free(buffer-fileSize); //free the allocated memory.

    fclose(key); //close the opened file

    if (iFlag == 1) {
        fclose(in); //close the opened file.
    }

    close(fd);
}

// The main function, which is the starting point of this process.
int main(int argc, char *argv[]) {
    printf("otpEncryption: argv[0] = %s\n", argv[0]);
    if (argc == 0) {
        printf("otpEncryption error!\n");
        printf("usage: my_otp [-i infile] [-o outfile] -k keyfile\n");
        exit(1);
    }
    char *keyFile = (char *) malloc(30);
    char *inFile = (char *) malloc(30);

    strcpy(inFile, argv[0]);
    strcpy(keyFile, argv[1]);

    readAndEncryptTheUserInput(inFile, keyFile); //reads the input and encrypt it.

    return 0;
}
