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
    size_t fileSize = ftell(in); //gets the size of the file.File descriptor is integer that uniquely identifies an open file of the process.

    rewind(in);

    char *buffer = (char *) malloc(fileSize); //buffer to store the encrypted texts.
    char *savePoint = buffer;

    FILE *key = fopen(keyFile, "rb");

    char inputChar;
    char keyChar;

    char *myfifo = "./otpfifo"; //the file path of the FIFO
    if (mkfifo(myfifo, 0666) < 0) { //creating the named pipe FIFO with the permission 0666.
        perror("mkfifo failed");
    }

    while ((inputChar = fgetc(in)) != EOF) { //read the inputfclose(in); //close the opened file until the fgetc finish reading all inputs

        if ( (keyChar = fgetc(key)) == EOF ) { //check if the program reaches to the end of the key file.
            rewind(key); //rewind to the beginning of the key file.
            keyChar = fgetc(key); //read the character once more.
        }

        char encryptedChar = (inputChar ^ keyChar); //do the xor operation to encrypt the text.

        //*buffer++ = encryptedChar;
        *buffer++ = 'a';
    }

    mode_t filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    int fd;
    fd = open(myfifo, O_RDWR | O_CREAT, filePerms); //open the fifo to write only.

    int checker;
    checker = write(fd, buffer, fileSize); //write the encrypted text to the fifo.
    printf("checker = %d\n", checker);
    if (checker < 0) {
        printf("ERROR\n");
    }

    free(savePoint); //free the allocated memory.

    fclose(key); //close the opened file

    if (iFlag == 1) {
        fclose(in); //close the opened file.
    }

    char *buffer2 = (char *) malloc(fileSize); //allocate 9999 bytes to buffer to avoid overflow problem
    size_t bytes = read(fd, buffer2, fileSize); //read encrypted text from fifo.
    printf("Test\n\n%sTestENDED\nread %d bytes\n", buffer2, bytes);

    close(fd);
}

// The main function, which is the starting point of this process.
int main(int argc, char *argv[]) {
    printf("otpEncryption: argv[0] = %s\n", argv[0]);

    char *keyFile = (char *) malloc(30);
    char *inFile = (char *) malloc(30);

    strcpy(inFile, argv[0]);
    strcpy(keyFile, argv[1]);

    printf("main of otpEncryption testing\n");

    readAndEncryptTheUserInput(inFile, keyFile); //reads the input and encrypt it.

    printf("main of otpEncryption testing end\n");

    free(keyFile);
    free(inFile);

    return 0;
}
