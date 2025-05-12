#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

// Global variables
volatile int global_var = 0;
volatile int running = 1;

// Semaphores for synchronization
sem_t global_var_semaphore;
sem_t running_semaphore;

// Thread function: Periodically updates `global_var`
void *update_var(void *arg) {
    while (1) {
        sem_wait(&running_semaphore); // Protect `running`
        if (!running) {
            sem_post(&running_semaphore);
            break;
        }
        sem_post(&running_semaphore);

        sem_wait(&global_var_semaphore); // Protect `global_var`
        global_var++;  // Increment global_var
        sem_post(&global_var_semaphore);

        usleep(500000); // Sleep for 0.5 seconds
    }
    return NULL;
}

// Thread function: Periodically prints the value of `global_var`
void *report_var(void *arg) {
    while (1) {
        sem_wait(&running_semaphore); // Protect `running`
        if (!running) {
            sem_post(&running_semaphore);
            break;
        }
        sem_post(&running_semaphore);

        sem_wait(&global_var_semaphore); // Protect `global_var`
        printf("Current value: %d\n", global_var);
        sem_post(&global_var_semaphore);

        usleep(1000000); // Sleep for 1 second
    }
    return NULL;
}

// Main function: Creates and manages threads, waits for user input to stop execution
int main() {
    pthread_t updater, reporter;

    // Initialize semaphores
    sem_init(&global_var_semaphore, 0, 1);
    sem_init(&running_semaphore, 0, 1);

    printf("main: begin\n");

    // Create threads
    pthread_create(&updater, NULL, update_var, NULL);
    pthread_create(&reporter, NULL, report_var, NULL);

    // Wait for user input to stop execution
    printf("Press Enter to stop...\n");
    getchar();

    sem_wait(&running_semaphore);
    running = 0; // Safely update `running`
    sem_post(&running_semaphore);

    // Wait for threads to finish before exiting
    pthread_join(updater, NULL);
    pthread_join(reporter, NULL);

    // Destroy semaphores
    sem_destroy(&global_var_semaphore);
    sem_destroy(&running_semaphore);

    printf("main: done\n");

    return 0;
}
