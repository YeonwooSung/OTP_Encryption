#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// This function is used to encrypt data and write to pipe
void encryptData(FILE* input, FILE* key_file, int writePipe){
    long long file_size = 0;
    int inputByte, keyByte, encryptedByte;

    if(input == stdin) { //check if the File stream called input is the stdin stream

        int* save_std_in = (int *) malloc(sizeof(int));
        while ((inputByte = fgetc(input)) != EOF && (keyByte = fgetc(key_file)) != EOF) { // loop each byte until EOF
            encryptedByte = (inputByte ^ keyByte);
            *(save_std_in + file_size) = encryptedByte;
            file_size += 1;
            save_std_in = realloc(save_std_in, (sizeof(int)*(file_size+1))); //use the realloc to change the size of the string.
        }

        write(writePipe, &file_size, sizeof(long long)); //write the size of the file to the pipe.

        for (long long i = 0; i < file_size; i++) { //iterate the for loop to write all characters in the input string to the pipe.
            encryptedByte = *(save_std_in+i);
            write(writePipe, &encryptedByte, sizeof(int));
        }

        free(save_std_in); //free the allocated memory

    }
    else {
        fseek(input, 0L, SEEK_END); // seek to the end of the file
        file_size = ftell(input); // get the file size
        rewind(input); // return the pointer back to the beginning of the file

        write(writePipe, &file_size, sizeof(long long)); // write the length of the characters in the file

        while ((inputByte = fgetc(input)) != EOF && (keyByte = fgetc(key_file)) != EOF) { // loop each byte until EOF
            encryptedByte = (inputByte ^ keyByte); // XOR encryption
            write(writePipe, &encryptedByte, sizeof(int)); // write the output byte to pipe
        }
    }
}

//
void writeOutput(FILE* output_file, int pipe_read) {

    long file_size;
    read(pipe_read, &file_size, sizeof(long long));

    int c;
    for (long long i = 0; i < file_size; ++i) {
        read(pipe_read, &c, sizeof(int));
        fputc(c, output_file);
    }
}

void checkArguments(int argc, char *argv[], char *input_file_name, char *output_file_name, char *key_file_name){
    int opt ;
    int optionNumCounter = 0;
    while (( opt = getopt ( argc , argv , "i:o:k:" )) != -1) {
        switch ( opt )
        {
            case 'i':
                if(optarg != NULL){
                    strcpy(input_file_name, optarg);
                    optionNumCounter += 1;
                }
                break;
            case 'o':
                if(optarg != NULL){
                    strcpy(output_file_name, optarg);
                    optionNumCounter += 1;
                }
                break;
            case 'k':
                if(optarg != NULL){
                    strcpy(key_file_name, optarg);
                    optionNumCounter += 1;
                }
                break;
            default :
                printf ("usage: %s [-i input.txt] [-o output.txt] -k keyfile.key\n", argv[0]) ;
                exit(EXIT_FAILURE);
        }
    }

    if(optionNumCounter != (argc - 1) / 2) { //to check if the user input the correct number of option arguments
        //If not, print out the error message, and exit the process.
        printf ("usage: %s [-i input.txt] [-o output.txt] -k keyfile.key\n", argv[0]) ;
        exit(EXIT_FAILURE);
    }
}

//check the closing status of the child process to catch error.
void detectExitFailure(int status){
    if (WIFEXITED(status) && WEXITSTATUS(status) == 1) {
        exit(1); //exit the process when the error occurred.
    }
}

//The main function, which is the starting point of the program.
int main (int argc, char *argv[]) {

    // "Name" are used to store name of the files.
    char *inputName = (char *) malloc(sizeof(char) * 20);
    char *output_file_name = (char *) malloc(sizeof(char) * 20);
    char *key_file_name = (char *) malloc(sizeof(char) * 20);

    checkArguments(argc, &(*argv), inputName, output_file_name, key_file_name); //to validate the command line options

    pid_t child[2];

    int pd[2]; // 0 is used to read from pipe, 1 is used to write from pipe
    pipe(pd); // open pipe

    // START OF CHILD 1;
    child[0] = fork(); // fork
    if (child[0] == 0) { // when its child process
        FILE *input_file;
        FILE *key_file = fopen(key_file_name, "r"); // open key_file

        if (strcmp(inputName, "") == 0) { // if input file is not given
            input_file = stdin; // take standard input
        } else { // open file
            input_file = fopen(inputName, "r");
        }

        if (input_file == NULL) { // if input file could not be opened (maybe due to permission or not found)
            perror("Can't find [-i inputfile]");
            _exit(EXIT_FAILURE); // exit program
        }

        if (key_file == NULL) { // if key file could not be opened (maybe due to permission)
            perror("Can't find [-k keyfile]");
            _exit(EXIT_FAILURE); // exit program
        }
        encryptData(input_file, key_file, pd[1]); // encrypt file and write to pipe

        close(pd[0]); // close read pipe
        close(pd[1]); // close write pipe

        fclose(input_file); // close opened file
        fclose(key_file); // close the opened file

        free(inputName); // free used memory allocated space
        free(output_file_name); // free used memory allocated space
        free(key_file_name); // free used memory allocated space

        _exit(EXIT_SUCCESS); // exit child_one
    }
    else { // parent process && not child_one
        // START OF CHILD 2;
        child[1] = fork(); // fork second child
        if (child[1] == 0) { // if second child
            FILE *output_file;
            if (strcmp(output_file_name, "") == 0) { // when output file is not given
                output_file = stdout; // output as stdout
            } else { // when output file is given
                output_file = fopen(output_file_name, "w");
            }

            if (output_file == NULL) { // if output file could not be written (maybe due to permission)
                fprintf(stderr, "Can't write [-o outfile] in current directory"); //print the error message.
                _exit(EXIT_FAILURE); //exit the process with exit code EXIT_FAILURE.
            }

            writeOutput(output_file, pd[0]); // write to file

            close(pd[0]); // close read pipe
            close(pd[1]); // close write pipe
            //fflush(output_file);
            fclose(output_file);
            free(inputName); // free used memory allocated space
            free(output_file_name);
            free(key_file_name);

            _exit(EXIT_SUCCESS); // exit child_two
        }
    }

    int status;
    waitpid(child[1], &status, 0);
    detectExitFailure(status); //check the closing status code of the child one.

    waitpid (child[1], &status, 0);
    detectExitFailure(status); //check the closing status code of the child two.

    free(inputName); // free used memory allocated space
    free(output_file_name); //free the used memory allocated space
    free(key_file_name); //free the used memory allocated space

    return 1;
}