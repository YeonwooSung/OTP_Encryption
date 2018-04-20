#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// This function makes the child processes.
void forkChild(char *file, char *keyFile, int kid) {

    pid_t pid = fork(); //gets the process id.

    if (pid < 0) { //The error happened when the value of pid is less than 0.
        printf("Error occurred");
        exit(1); //terminate the programfunction
    } else if (pid == 0) { //If the value of the pid is 0, now we are in the child process.
        printf("now in the child num %d\n", kid);
        if (kid == 1) {
            execl("./otpEncryption", file, keyFile, NULL);
        } else if (kid == 2) {
            execl("./encryptedMessenger", file, NULL);
        }
    } else {
        printf("now in the parent\n");
    }
}

// The main function, which is the starting point of this program.
int main(int argc, char *argv[]) {

    if (argc < 2 && (argc % 2) == 1) { //check the number of command line argument.
        //if the user did not input the command line, print out the warning message, and exit the program.
        printf("usage: my_otp [-i infile] [-o outfile] -k keyfile\n");
        exit(1);
    }

    //Flags for the options.
    int iFlag = 0; //flag for the -i option
    int oFlag = 0; //flag for the -o option=mA.enc
    int kFlag = 0; //flag for the -k option

    int opt; //to store the result of the getopt function.

    char *inFile = (char *) malloc(30);
    char *outFile = (char *) malloc(30);
    char *keyFile = (char *) malloc(30);

    while ((opt = getopt(argc, argv, "i:o:k:")) != -1) { //TODO fill the last argument with the suitable string
        switch (opt) {
            case 'i' :
                iFlag += 1; //as we use -i option, change the value of the iFlag.
                strcpy(inFile, optarg);
                break;
            case 'o' :
                oFlag += 1; //as we use -o option, change the value of the oFlag.
                strcpy(outFile, optarg);
                break;
            case 'k' :
                kFlag = 1; //as we use -k option, change the value of the kFlag.
                strcpy(keyFile, optarg);
                break;
            default:
                printf("usage: my_otp [-i infile] [-o outfile] -k keyfile\n");
        } //switch ends

    } //while loop ends

    if (kFlag == 0) { //check if the user input the -k option.
        printf("usage: my_otp [-i infile] [-o outfile] -k keyfile\n");
        exit(1); //terminate the program
    }

    if (iFlag == 0) {
        strcpy(inFile, "std"); //if the user did not use the -i option, use the stdin to read the user input.
    }

    if (oFlag == 0) {
        strcpy(outFile, "std"); //if the user did not use the -o option, use the stdout the print out the encrypted message.
    }

    int kid = 1;

    forkChild(inFile, keyFile, kid); //use the fork() to make child processes.
    int status;
    pid_t pid = wait(&status); //wait the child process finishes
    printf("The child process with the pid %d terminated with the status %d\n", pid, status);

    kid += 1;

    forkChild(outFile, keyFile, kid); //use the fork() to make child processes.
    int status2;
    pid_t pid2 = wait(&status2); //wait the child process finishes
    printf("The child process with the pid %d terminated with the status %d\n", pid2, status2);

    return 0;
}
