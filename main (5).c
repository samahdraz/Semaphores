#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#define THREADS_NO 5
#define BUFFER_SIZE 3

int buffer[BUFFER_SIZE];
int in = 0, out = 0;
int counter;

// First part
sem_t mutex1;
void* counter_thread(void *args)
{
    int thread_no = *(int *)args;
    free(args);

    while (1)
    {
        sleep(rand() % 5 + 1);

        printf("\nCounter thread %d: received a message",thread_no);

        //wait
        printf("\nCounter thread %d: waiting to write",thread_no);
        sem_wait(&mutex1);

        //critical section
        counter++;
        printf("\nCounter thread %d: now adding to counter, counter value = %d",thread_no,counter);

        //signal
        sem_post(&mutex1);
    }
    return NULL;
}

// Second part
sem_t mutex2,full,empty;
void* producer_thread()
{
    while (1)
    {
        sleep(rand() % 5 + 1);

        printf("\nMonitor thread: waiting to read counter");

        int value;
        sem_getvalue(&empty,&value);
        if (value = BUFFER_SIZE)
        {
            printf("Monitor thread: Buffer full!!");

        }
        // wait
        sem_wait(&empty);
        sem_wait(&mutex2);
        sem_wait(&mutex1);

        // critical section
        printf("\nMonitor thread: reading a count value of %d", counter);
        buffer[in] = counter;
        printf("\nMonitor thread: writing to buffer at position %d",in);
        counter = 0;
        in = (in+1) % BUFFER_SIZE;

        //signal
        sem_post(&mutex1);
        sem_post(&mutex2);
        sem_post(&full);
    }
    return NULL;
}
void* consumer_thread()
{
    while(1)
    {
        sleep(rand() % 5 + 1);

        int value;
        sem_getvalue(&full,&value);
        if (value = 0)
        {
            printf("Collector thread: nothing is in the buffer!");

        }
        //wait
        sem_wait(&full);
        sem_wait(&mutex2);

        //critical section
        int item = buffer[out];
        printf("\nCollector thread: reading from the buffer at position %d", out);
        out = (out+1) % BUFFER_SIZE;

        //signal
        sem_post(&mutex2);
        sem_wait(&empty);
    }
    return NULL;

}

int main()
{
    srand(time(NULL));

    // initializing semaphores
    sem_init(&mutex1, 0, 1);
    sem_init(&mutex2, 0, 1);
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, BUFFER_SIZE);

    // creating threads
    pthread_t threads[THREADS_NO] ;
    pthread_t t1,t2;
    int i =0;
    for (i=0; i<THREADS_NO; i++)
    {
        int *thread_no = malloc(sizeof(int));
        *thread_no = i;
        pthread_create(&threads[i],NULL,counter_thread,(void *)thread_no);
    }
    pthread_create(&t1, NULL, producer_thread, NULL);
    pthread_create(&t2, NULL, consumer_thread, NULL);

    // joining threads
    for (i=0; i<THREADS_NO; i++)
    {
        pthread_join(threads[i],NULL);
    }
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);

    // destroying semaphores
    sem_destroy(&mutex1);
    sem_destroy(&mutex2);
    sem_destroy(&full);
    sem_destroy(&empty);

    printf("Exit\n");
    return 0;
}
