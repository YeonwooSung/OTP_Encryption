#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "msg_queue.h"

//Enqueue the new data node to the message queue
void send_msg(MQueue *queue, int value) {

    pthread_mutex_lock(queue->mutex);

    Node *node = (Node *) malloc(sizeof(Node)); //allocate memory to create node.
    node->data = value; //set the data value of the new node
    node->next = 0; //set the pointer of the next node

    if (queue->head == NULL) { //check whether the queue contains the node or not
        queue->head = node; //set the initial head of the message queue
    } else {
        //Otherwise, link the new node to the previous tail.
        queue->tail->next = node;
    }

    queue->tail = node; //add the node to the queue

    pthread_mutex_unlock(queue->mutex);
}

//Dequeue the node from the message queue.
Node *read_msg(MQueue *queue) {
    pthread_mutex_lock(queue->mutex); //lock the mutex to ensure the thread safety.

    if (queue->head == NULL) {
        pthread_mutex_unlock(queue->mutex);
        return NULL; //return null if the message queue does not have any node in it.
    }

    Node *currentNode = queue->head;
    queue->head = currentNode->next;

    if (queue->head == NULL) { //check if the current node is the last node of the queue
        queue->tail == NULL; //set the tail of the queue as NULL as well.
    }

    pthread_mutex_unlock(queue->mutex); //unlock the mutex.

    return currentNode; //return the current node to dequeue.
}

//Initialize the message queue.
void initMQueue(MQueue *queue) {
    queue->head = NULL; //initialize the head of the queue as NULL.
    queue->tail = NULL; //initialize the tail of the queue as NULL.

    //allocate the memory for the mutex of the queue, and then initialize the member mutex of the queue.
    pthread_mutex_t *mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    int err = pthread_mutex_init(mutex, NULL); //use the init function to initialize the mutex.
    if (err != 0) {
        printf("Failed to initialize the mutex\n");
        exit(1);
    }
    queue->mutex = mutex; //set the member mutex of the message queue as a mutex pointer.
}

//Print out the user message.
void printMQueue(MQueue *queue) {

    Node *target = read_msg(queue);

    if (target == NULL) { //check if the current node is the NULL.
        return;
    }

    char content = (char) target->data;
    printf("%c", content);
    free(target);
}

//This function makes the message queue empty.
void emptyQueue(MQueue *queue) {
    while (read_msg(queue) != NULL); //dequeue until the read_msg returns the NULL.
}

//Free the given queue.
void freeMQueue(MQueue *queue) {
    emptyQueue(queue);
    free(queue->mutex);
    free(queue);
}
