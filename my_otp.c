#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

// This function makes the child processes.
void forkChildren() {
    for (int kid = 0; kid < 2; kid++) {
        pid_t pid = fork();

        if (pid < 0) { //The error happened when the value of pid is less than 0.
            printf("Error occurred");
            exit(1); //terminate the program
        } else if (pid == 0) { //If the value of the pid is 0, now we are in the child process.
            //TODO execute the suitable exe file.
        } else { //If the value of the pid is greater than 0, we are in the parent process.
            printf("This is the parent process!");
            exit(1); //terminate the program
        }
    }
}

// This function waits all child processes terminate.
void waitAllChildrenTerminates() {
    for (int kid = 0; kid < 2; kid++) {
        int status;
        pid_t pid = wait(&status); //wait the child process finishes
        printf("The child process with the pid %d terminated with the status %d\n", pid, status);
    }
}

// The main function, which is the starting point of this program.
int main(int argc, char *argv[]) {
    //Flags for the options.
    char iFlag = 0; //flag for the -i option
    char oFlag = 0; //flag for the -o option
    char kFlag = 0; //flag for the -k option

    int opt; //to store the result of the getopt function.

    while ((opt = getopt(argc, argv, "iok")) != -1) { //TODO fill the last argument with the suitable string
        switch (opt) {
            case 'k' :
                if (kFlag) { //check if the user input multiple -k option
                    printf("usage: my_otp [-i infile] [-o outfile] -k keyfile\n");
                    exit(1); //terminate the program
                }
                kFlag = 1;
                break;
            case 'i' :
                if (iFlag) { //check if the user input more than 1 -i option
                    printf("usage: my_otp [-i infile] [-o outfile] -k keyfile\n");
                    exit(1); //terminate the program
                }
                iFlag = 1; //change the value of the iFlag to 1 to change the stream input from stdin to file stream.
                break;
            case 'o' :
                if (oFlag) { //check if the user input more than 1 -i option
                    printf("usage: my_otp [-i infile] [-o outfile] -k keyfile\n");
                    exit(1); //terminate the program
                }
                oFlag = 1;
                break;
            default:
                printf("usage: my_otp [-i infile] [-o outfile] -k keyfile\n");
        } //switch ends
    } //while loop ends

    if (kFlag == 0) {
        printf("usage: my_otp [-i infile] [-o outfile] -k keyfile\n");
        exit(1); //terminate the program
    }

    //TODO give suitable arguments by checking the flags
    forkChildren(); //use the fork() to make child processes.

    waitAllChildrenTerminates();

    return 0;
}