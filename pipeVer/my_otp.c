#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

long long encryptData(FILE* input, FILE* key, int writingPipe){
    int encrypt_byte;
    int key_byte;
    char output_byte;
    long long counter = 0;
    while( (encrypt_byte = fgetc(input)) != EOF ) {//Loop through each byte of file until EOF
        if ((key_byte = fgetc(key)) == EOF) {
            rewind(key);
            key_byte = fgetc(key);
        }
        output_byte = (char)(encrypt_byte ^ key_byte);
        write(writingPipe, &output_byte, 1);
        counter ++;
    }

    return counter;
}

void writeToFile(FILE* output_file, int pipe_read) {
    char c;
    read(pipe_read, &c, 1);
    fputc(c, output_file);
}


int main (int argc, char *argv[]) {

    FILE *input = stdin;
    FILE *output = stdout;
    FILE *keyfile = NULL;

    char kFlag = 0;
    char iFlag = 0;
    char oFlag = 0;

    int opt ;
    while (( opt = getopt ( argc , argv , "i:o:k:" )) != -1) { //check the options that the user used
        switch ( opt ) {
            case 'i':
                input = fopen(optarg, "r"); //open the input file if the user used the -i option.
                iFlag = 1;
                break;
            case 'o':
                output = fopen(optarg, "w"); //open the output file if the user used the -o option
                oFlag = 1;
                break;
            case 'k':
                keyfile = fopen(optarg, "r"); //open the key file if the user used the -k option
                kFlag = 1;
                break;
            default :
                printf ("usage: [-i input.txt] [-o output.txt] -k keyfile.key\n") ;
                exit(1);
        }
    }

    if (kFlag == 0) { //check if the user used the -k option
        printf ("usage: [-i input.txt] [-o output.txt] -k keyfile.key\n") ;
        fprintf(stderr, "The -k option is the mandatory option!");
        exit(1);
    }

    pid_t child[2]; //array to store child processes.

    int pd[2];
    pipe(pd);

    child[0] = fork ();
    if (child[0] == 0) { //check if the program is in the first child.
        long long x = encryptData(input, keyfile, pd[1]);

        child[1] = fork ();
        if (child[1] == 0){ //activate this if statement when the program is in the parent process.
            for (long long i = 0; i < x; ++i) { //use long long to allow the program process a big file.
                writeToFile(output, pd[0]);
            }
        }
    }

    for (int i = 0; i < 2; i++) { //run the for loop to wait the termination of child processes
        int status;
        waitpid(child[i], &status, 0);
        printf("The child process with pid number %d is terminated with the status num %d\n", child[i], status);
    }

    if (oFlag) { //check if the user used the -o option
        fclose(output); //close the output file.
    }

    if (iFlag) { //check if the user used the -i option
        fclose(input); //close the input file.
    }

    fclose(keyfile); //close the keyfile.

    return 0;
}
