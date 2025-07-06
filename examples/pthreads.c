/* Example: pthreads
 *
 * Create some threads, let them say hello, and exit.
 *
 * Compile: gcc -lpthread -o pthreads pthreads.c
 * Execute: ./pthreads
 *
 * Sven Reissmann (sven.reissmann@rz.hs-fulda.de)
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

/* The number of threads to create
 */
#define NUM_THREADS 16

/* Define a small datastructure for all the data we want to pass to a thread.
 * Remember, the function a thread gets passed can have a parameter of type
 * void *. We can use this structure to stuff in all the individual variables
 * we want to pass to a thread casting it through void *. As all threads share
 * the heap of the process, we need an instance of this struct per thread!
 */
struct tdata {
    int id;
};

/* This is the function, each thread will execute
 */
void* thread_function (void* mydata) {
    /* Cast our data back from void * to struct tdata *
     */
    struct tdata* t = (struct tdata *) mydata;

    /* Do something with the data
     */
    printf ("Hi from thread %d, (Thread-ID: %lu)\n", t->id, pthread_self());

    /* Exit thread execution
     */
    pthread_exit ((void *) EXIT_SUCCESS);
}

int main (int argc, char **argv) {
    /* This array holds the thread IDs. When creating more than one thread, it
     * makes sense to use an array to store the IDs (linux: large int numbers).
     */
    pthread_t threads[NUM_THREADS];

    /* This array gets the return values of the thread functions
     */
    int ret[NUM_THREADS];

    /* Create NUM_THREADS
     */
    for (int i = 0; i < NUM_THREADS; i++) {
        /* The thread function takes exactly one void * argument.
         * Simple usecases might just use something like a simple int pointer,
         * or don't use a parameter at all by passing (void *) NULL. We chose
         * to use a small datastructure, which we pass here.
         *
         * Try to move the malloc line before the loop, to find out what it
         * means to use parallelism with shared data structures. :)
         */
        struct tdata *mytdata = (struct tdata *) malloc (sizeof (struct tdata));
        mytdata->id = i;

        /* Start the thread
         * Out datastructure (casted to void *) is the last parameter.
         */
        if ((ret[i] = pthread_create (&threads[i], NULL, thread_function, (void *) mytdata)) != 0) {
            perror ("pthread_join");
            exit (EXIT_FAILURE);
        }
    }

    /* Now, wait for the threads to end
     */
    for (int i = 0; i < NUM_THREADS; i++) {
        if ((ret[i] = pthread_join (threads[i], NULL)) != 0) {
            perror ("pthread_join");
            exit (EXIT_FAILURE);
        }
    }

    exit (EXIT_SUCCESS);
}

