#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    //Flags for the options.
    char iFlag = 0; //flag for the -i option
    char oFlag = 0; //flag for the -o option
    char kFlag = 0; //flag for the -k option

    FILE *input = stdin;
    FILE *output = stdout;

    int opt; //to store the result of the getopt function.

    while ((opt = getopt(argc, argv, "iok")) != -1) { //TODO fill the last argument with the suitable string
        switch (opt) {
            case 'k' :
                if (kFlag) { //check if the user input multiple -k option
                    printf("Only one 'k' option is required!");
                    printf("usage: my_otp [-i infile] [-o outfile] -k keyfile");
                    return 0; //terminate the program
                }
                kFlag = 1;
                break;
            case 'i' :
                if (iFlag) { //check if the user input more than 1 -i option
                    printf("Only one 'i' option!");
                    printf("usage: my_otp [-i infile] [-o outfile] -k keyfile");
                    return 0; //terminate the program
                }
                iFlag = 1; //change the value of the iFlag to 1 to change the stream input from stdin to file stream.
                break;
            case 'o' :
                if (oFlag) { //check if the user input more than 1 -i option
                    printf("Only one 'o' option!");
                    printf("usage: my_otp [-i infile] [-o outfile] -k keyfile");
                    return 0; //terminate the program
                }
                oFlag = 1;
                break;
            default:
                printf("usage: my_otp [-i infile] [-o outfile] -k keyfile");
        } //switch ends
    } //while loop ends

    if (kFlag == 0) {
        printf("usage: my_otp [-i infile] [-o outfile] -k keyfile");
        return 0;
    }
    return 0;
}