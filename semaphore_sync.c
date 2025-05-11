#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h> 

// Global variable
volatile int global_var = 0;
volatile int running = 1; 

// Semaphore for synchronization
sem_t semaphore; 

// Thread function: Periodically updates `global_var`
void *update_var(void *arg) {
    while (running) {
        sem_wait(&semaphore);  // Wait (lock)
        global_var++;          // Increment global_var
        sem_post(&semaphore);  // Signal (unlock)
        usleep(500000);        // Sleep for 0.5 seconds
    }
    return NULL;
} 

// Thread function: Periodically prints the value of `global_var`
void *report_var(void *arg) {
    while (running) {
        sem_wait(&semaphore);  // Wait (lock)
        printf("Current value: %d\n", global_var);  // Print current value
        sem_post(&semaphore);  // Signal (unlock)
        usleep(1000000);        // Sleep for 1 second
    }
    return NULL;
}

// Main function: Creates and manages threads, waits for user input to stop execution
int main() {
    pthread_t updater, reporter; 

    // Initialize semaphore with a value of 1
    sem_init(&semaphore, 0, 1); 

    printf("main: begin\n"); 

    // Create threads

    pthread_create(&updater, NULL, update_var, NULL);
    pthread_create(&reporter, NULL, report_var, NULL); 

    // Wait for user input to stop execution
    printf("Press Enter to stop...\n");
    getchar();
    running = 0;  // Stop flag 

    // Wait for threads to finish before exiting
    pthread_join(updater, NULL);
    pthread_join(reporter, NULL); 

    // Destroy semaphore
    sem_destroy(&semaphore);
    printf("main: done\n");
    return 0;
}
