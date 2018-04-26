#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "msg_queue.h"

// Gets the input message from the stdin stream.
int getInputsToStream(MQueue *q) {

    char *buffer = (char *) malloc(256);
    char *savePoint = buffer; //will use this to free the allocated memory

    int totalNumOfChars = 0;

    while (fgets(buffer, 256, stdin) != NULL) {
        int counter = 0;

        while (*buffer != '\n' && *buffer != '\0' && counter < 256) {
            //
            int input = *buffer;
            send_msg(q, input);
            buffer += 1;
            counter += 1;
            totalNumOfChars += 1;
        }

        if (*buffer == '\n') { //check if the current character is the next line character.
            int input = *buffer;
            send_msg(q, input);
            totalNumOfChars += 1;
        }

        free(savePoint); //free the allocated memory

        buffer = (char *) malloc(256);

        savePoint = buffer;
    }

    free(savePoint);

    return totalNumOfChars;
}

// Prints the output message through the stdout stream.
void *writeOutputsToStream(void *queue) {
    MQueue *q = (MQueue *)queue;

    printMQueue(q);
}

// The main function.
int main(void) {

    MQueue *queue = (MQueue *) malloc(sizeof(MQueue));
    initMQueue(queue);

    int num = getInputsToStream(queue); //get the user input

    pthread_t writer[num];

    int i, err;
    for (i = 0; i < num; i++) {
        err = pthread_create(&writer[i], NULL, writeOutputsToStream, (void *)queue); //create the new thread that writes output messages.
        if (err != 0) {
            fprintf(stderr, "Failed to make a thread\n");
            exit(1);
        }
    }

    int j, status;
    for (j = 0; j < num; j++) {
        pthread_join(writer[j], (void *)&status);
    }

    freeMQueue(queue);

    return 1;
}
