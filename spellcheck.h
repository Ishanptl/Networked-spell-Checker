#ifndef SPELLCHECK_H
#define SPELLCHECK_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>



#define BUF_SIZE 256
#define _XOPEN_SOURCE 600

#define READ 0
#define WRITE 1

#define TRUE 1
#define FALSE 0

#define BACKLOG 20
#define DEFAULT_PORT_STR "9999"

#define DEFAULT_DICTIONARY "words.txt"
#define DICTIONARY_SIZE 99172

#define EXIT_USAGE_ERROR 1
#define EXIT_GETADDRINFO_ERROR 2
#define EXIT_BIND_FAILURE 3
#define EXIT_LISTEN_FAILURE 4

#define NUM_WORKERS 3

#define MAX_LINE 64


typedef struct {
    // buffer array
    int *buf;
    // maximum number of slots
    int capacity;
    // first item
    int front;
    // last item
    int rear;
    // number of items
    int size;
    pthread_mutex_t mutex;
pthread_cond_t cv;
} queue;

void q_init(queue *q, int n) {
    q->buf = calloc(n, sizeof(int));
    // capacity is n
    q->capacity = n;
    // size is 0
    q->size = 0;
    // empty buffer if front == rear
    q->front = q->rear = 0;
  q->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
  q->cv= (pthread_cond_t)PTHREAD_COND_INITIALIZER; 
}

void q_deinit(queue *q) {
    free(q->buf);
}

void q_insert(queue *q, int item) {
     pthread_mutex_lock(&q->mutex);
	while(q->size == q->capacity){
	pthread_cond_wait(&q->cv, &q->mutex);}  
    q->buf[(++q->rear) % (q->capacity)] = item;
    q->size++;
    
  pthread_cond_signal(&q->cv);
pthread_mutex_unlock(&q->mutex);
}

int q_remove(queue *q) {
    int item;
    pthread_mutex_lock(&q->mutex);
if (q->size > 0){
    item = q->buf[(++q->front) % (q->capacity)];
    q->size--;
    }else{
	pthread_cond_wait(&q->cv, &q->mutex);
}
	pthread_mutex_unlock(&q->mutex);
    return item;
}

typedef struct thread_args {
    queue *qu;
    char **words;
} thread_args;

typedef struct addrinfo addrinfo;
typedef struct sockaddr_storage sockaddr_storage;
typedef struct sockaddr sockaddr;
int getlistenfd(char *);
void *request_handle(void *);
ssize_t readLine(int , void *, size_t);
char **load_dict(char *);

#endif
