#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define SHELF_SIZE 5
#define NUM_LIBRARIANS 3
#define NUM_READERS 3
#define NUM_BOOKS 9

int shelf[SHELF_SIZE];
int in = 0, out = 0;

sem_t empty; // Counts the number of empty slots on the shelf
sem_t full;  // Counts the number of filled slots on the shelf
sem_t mutex; // Protects access to the shelf

void *librarian(void *arg)
{
    int librarian_id = *(int *)arg;
    for (int i = 0; i < NUM_BOOKS; i++)
    {
        int book = rand() % 100;
        sleep(rand() % 2);
        sem_wait(&empty);
        sem_wait(&mutex);

        shelf[in] = book;
        printf("Librarian %d placed book %d on shelf slot %d\n", librarian_id, book, in);
        in = (in + 1) % SHELF_SIZE;

        sem_post(&mutex);
        sem_post(&full);
    }
    printf("Librarian %d finished restocking books.\n", librarian_id);
    pthread_exit(NULL);
}

void *reader(void *arg)
{
    int reader_id = *(int *)arg;
    for (int i = 0; i < NUM_BOOKS; i++)
    {
        sem_wait(&full);
        sem_wait(&mutex);

        int book = shelf[out];
        printf("Reader %d borrowed book %d from shelf slot %d\n", reader_id, book, out);
        out = (out + 1) % SHELF_SIZE;

        sem_post(&mutex);
        sem_post(&empty);

        sleep(rand() % 3);
    }
    printf("Reader %d finished borrowing books.\n", reader_id);
    pthread_exit(NULL);
}

int main()
{
    pthread_t librarians[NUM_LIBRARIANS];
    pthread_t readers[NUM_READERS];
    int librarian_ids[NUM_LIBRARIANS];
    int reader_ids[NUM_READERS];

    sem_init(&empty, 0, SHELF_SIZE);
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 1);

    for (int i = 0; i < NUM_LIBRARIANS; i++)
    {
        librarian_ids[i] = i + 1;
        pthread_create(&librarians[i], NULL, librarian, &librarian_ids[i]);
    }

    for (int i = 0; i < NUM_READERS; i++)
    {
        reader_ids[i] = i + 1;
        pthread_create(&readers[i], NULL, reader, &reader_ids[i]);
    }

    for (int i = 0; i < NUM_LIBRARIANS; i++)
    {
        pthread_join(librarians[i], NULL);
    }

    for (int i = 0; i < NUM_READERS; i++)
    {
        pthread_join(readers[i], NULL);
    }

    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);

    printf("Library activities have concluded.\n");
    return 0;
}
