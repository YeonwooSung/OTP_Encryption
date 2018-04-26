#include <pthread.h>

#ifndef MSG_QUEUE_H
#define MSG_QUEUE_H


/* Interface for integer MQueue */
typedef struct node{
	int data; //the message data
	struct node *next; //pointer to the next node to implement the linked list
} Node;

/* The multi-threaded version contain a mutex 'access */
typedef struct {
	Node *head; //the first node of the queue.
	Node *tail; //the last node of the queue.
	pthread_mutex_t *mutex; //this will be used to implement the thread safe queue.
} MQueue;

void send_msg( MQueue *s, int value ); // enqueue
Node *read_msg( MQueue *s );           // dequeue
void initMQueue(MQueue *);             // initializer
void printMQueue(MQueue *);            // print the message.
void freeMQueue(MQueue *);           // free the message queue.

#endif //MSG_QUEUE_H
