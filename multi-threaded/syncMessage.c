#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "msg_queue.h"

// Gets the input message from the stdin stream.
int getInputsToStream(MQueue *q) {

    char *buffer = (char *) malloc(300);
    char *savePoint = buffer; //will use this to free the allocated memory

    int totalNum = 0;

    while (fgets(buffer, 299, stdin) != NULL) {

        send_msg(q, buffer);
        totalNum += 1;

    }

    free(savePoint);

    return totalNum;
}

// Prints the output message through the stdout stream.
void *writeOutputsToStream(void *queue) {
    MQueue *q = (MQueue *)queue;

    printMQueue(q);
}

// The main function.
int main(void) {

    char *keyFileName = (char *) malloc(40);
    strcpy(keyFileName, "keyfile16K.key");

    MQueue *queue = (MQueue *) malloc(sizeof(MQueue));
    initMQueue(queue, keyFileName);

    int num = getInputsToStream(queue); //get the user input
    printf("num of lines: %d\n", num);

    pthread_t writer[num];

    int i, err;
    for (i = 0; i < num; i++) {
        err = pthread_create(&writer[i], NULL, writeOutputsToStream, (void *)queue); //create the new thread that writes output messages.
        if (err != 0) {
            fprintf(stderr, "Failed to make a thread\n");
            exit(1);
        }
    }

    //nanosleep((const struct timespec[]){{0, 50000L}}, NULL);


    int j, status;
    for (j = 0; j < num; j++) {
        pthread_join(writer[j], (void *)&status);
    }


    freeMQueue(queue);
}
