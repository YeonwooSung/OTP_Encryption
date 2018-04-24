#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "msg_queue.h"

// Gets the input message from the stdin stream.
void getInputsToStream(MQueue *q) {

    char *buffer = (char *) malloc(256);
    char *savePoint = buffer; //will use this to free the allocated memory

    fgets(buffer, 256, stdin);

    int counter = 0;

    while (*buffer != '\n' && *buffer != '\0' && counter < 256) {
        //
        int input = *buffer;
        send_msg(q, input);
        buffer += 1;
        counter += 1;
    }

    free(savePoint); //free the allocated memory

}

// Prints the output message through the stdout stream.
void *writeOutputsToStream(void *queue) {
    MQueue *q = (MQueue *)queue;

    printMQueue(q);
}

// The main function.
int main(int argc, char *argv[]) {

    int doJoin;

    if (argc > 1 && strcmp(argv[1], "join") == 0) { //check the command line argument
        doJoin = 1; //set the value of doJoin as 1.
    } else {
        //Otherwise, set the value of doJoin as 0.
        doJoin = 0;
    }

    pthread_t writer[10];

    MQueue *queue = (MQueue *) malloc(sizeof(MQueue));
    initMQueue(queue);

    getInputsToStream(queue); //get the user input

    int i, err;
    for (i = 0; i < 10; i++) {
        err = pthread_create(&writer[i], NULL, writeOutputsToStream, (void *)queue); //create the new thread that writes output messages.
        if (err != 0) {
            fprintf(stderr, "Failed to make a thread\n");
            exit(1);
        }
        //printf("Finish the %02d turn\n", i);
    }

    nanosleep((const struct timespec[]){{0, 50000L}}, NULL); //use the nanosleep to sleep the main thread

    printf("sleep finish\n");

    if (doJoin) {
        int j, status;
        for (j = 0; j < 10; j++) {
            pthread_join(writer[j], (void *)&status);
        }
    }

    freeMQueue(queue);
}
