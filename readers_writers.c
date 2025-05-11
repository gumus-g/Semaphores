#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t writer_lock, reader_lock;
int reader_count = 0;
int shared_data = 0;

void *reader(void *arg) {
    int id = *(int *)arg;

    printf("[Reader %d] Trying to obtain reader lock...\n", id);
    sem_wait(&reader_lock);
    reader_count++;
    if (reader_count == 1) {
        printf("[Reader %d] First reader, obtaining writer lock...\n", id);
        sem_wait(&writer_lock);
    }
    printf("[Reader %d] Released reader lock.\n", id);
    sem_post(&reader_lock);

    // Reading section
    printf("[Reader %d] Reading shared data: %d\n", id, shared_data);
    usleep(100);  // Simulating reading delay

    printf("[Reader %d] Trying to obtain reader lock...\n", id);
    sem_wait(&reader_lock);
    reader_count--;
    if (reader_count == 0) {
        printf("[Reader %d] Last reader, releasing writer lock...\n", id);
        sem_post(&writer_lock);
    }
    printf("[Reader %d] Released reader lock.\n", id);
    sem_post(&reader_lock);

    return NULL;
}

void *writer(void *arg) {
    int id = *(int *)arg;

    printf("[Writer %d] Trying to obtain writer lock...\n", id);
    sem_wait(&writer_lock);
    printf("[Writer %d] Obtained writer lock.\n", id);
    
    // Writing section
    shared_data = rand() % 100;
    printf("[Writer %d] Writing new shared data: %d\n", id, shared_data);
    usleep(300);  // Simulating writing delay

    printf("[Writer %d] Releasing writer lock.\n", id);
    sem_post(&writer_lock);
    
    return NULL;
}

int main() {
    sem_init(&writer_lock, 0, 1);
    sem_init(&reader_lock, 0, 1);

    pthread_t readers[5], writers[2];
    int id[7];

    for (int i = 0; i < 5; i++) {
        id[i] = i + 1;
        pthread_create(&readers[i], NULL, reader, &id[i]);
    }
    for (int i = 0; i < 2; i++) {
        id[i + 5] = i + 1;
        pthread_create(&writers[i], NULL, writer, &id[i + 5]);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(readers[i], NULL);
    }
    for (int i = 0; i < 2; i++) {
        pthread_join(writers[i], NULL);
    }

    sem_destroy(&writer_lock);
    sem_destroy(&reader_lock);
    
    return 0;
}
