#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "msg_queue.h"

char *encryptMessageWithXOR(FILE *key, char *value, int count) {
    char *encryptedString = value; //save the memory address of the value.

    char keyByte;

    while (count-- > 0) {
        if ( (keyByte = fgetc(key)) == EOF ) {
            rewind(key); //rewind the key stream
            keyByte = fgetc(key);
        }
        *value = keyByte ^ (*value); //encrypt the input string by using XOR operation.
        value += 1;
    }

    return encryptedString;
}

//Enqueue the new data node to the message queue
void send_msg(MQueue *queue, char *value) {

    pthread_mutex_lock(queue->mutex);

    FILE *key = fopen(queue->keyFile, "r");
    char *val = (char *) malloc(300);

    strcpy(val, value);

    int count = strlen(value);

    Node *node = (Node *) malloc(sizeof(Node)); //allocate memory to create node.
    node->data = encryptMessageWithXOR(key, val, count); //set the data value of the new node
    node->next = 0; //set the pointer of the next node
    node->count = count;

    if (queue->head == NULL) { //check whether the queue contains the node or not
        queue->head = node; //set the initial head of the message queue
    } else {
        //Otherwise, link the new node to the previous tail.
        queue->tail->next = node;
    }

    queue->tail = node; //add the node to the queue

    fclose(key);

    pthread_mutex_unlock(queue->mutex);
}

//Dequeue the node from the message queue.
Node *read_msg(MQueue *queue) {
    if (queue->head == NULL) {
        return NULL; //return null if the message queue does not have any node in it.
    }

    Node *currentNode = queue->head; //gets the first in node from the message queue.
    queue->head = currentNode->next;

    if (queue->head == NULL) { //check if the current node is the last node of the queue
        queue->tail == NULL; //set the tail of the queue as NULL as well.
    }
    return currentNode; //return the current node to dequeue.
}

//Initialize the message queue.
void initMQueue(MQueue *queue, char *keyFile) {
    queue->head = NULL; //initialize the head of the queue as NULL.
    queue->tail = NULL; //initialize the tail of the queue as NULL.

    queue->keyFile = keyFile; //initialize the path name of the key file that the thread safe queue uses for encryption.

    //allocate the memory for the mutex of the queue, and then initialize the member mutex of the queue.
    pthread_mutex_t *mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));

     //use the init function to initialize the mutex.
    int err = pthread_mutex_init(mutex, NULL);
    if (err != 0) {
        printf("Failed to initialize the mutex\n");
        exit(1);
    }
    queue->mutex = mutex; //set the member mutex of the message queue as a mutex pointer.
}

//Print out the user message.
void printMQueue(MQueue *queue) {
    pthread_mutex_lock(queue->mutex);
    Node *target = read_msg(queue); //dequeue the node from the message queue.

    if (target == NULL) { //check if the current node is the NULL.
        return;
    }

    FILE *key = fopen(queue->keyFile, "r");

    char *content = encryptMessageWithXOR(key, target->data, target->count); //get the data from the message node.
    printf("%s", content); //print out the message data.
    free(target->data);
    free(target);
    fclose(key);
    pthread_mutex_unlock(queue->mutex);
}


//This function makes the message queue empty.
void emptyQueue(MQueue *queue) {
    while (read_msg(queue) != NULL); //dequeue until the read_msg returns the NULL.
}

//Free the given queue.
void freeMQueue(MQueue *queue) {
    emptyQueue(queue); //free the allocated memory of all nodes in the message queue
    free(queue->keyFile);
    free(queue->mutex); //free the allocated memory of the mutex
    free(queue); //free the queue.
}
