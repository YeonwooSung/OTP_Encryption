#include <stdio.h>
#include <string.h>

//Reads the user input and encrypts it.
void readInputAndEncrypt(FILE *inFile, FILE *key) {
    char buffer[256];
    while (fgets(buffer, 255, inFile)) { //read the input until the fgets finish reading all inputs
        //
    }
}

// The main function, which is the starting point of this process.
int main(int argc, char *argv[]) {
    FILE *inputStream = stdin;
    FILE *keyStream = stdin;

    //

    readInputAndEncrypt(inputStream, keyStream); //reads the input and encrypt it.
}
