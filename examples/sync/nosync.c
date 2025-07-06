/* Example: Synchronization problem
 *
 * For this example, let's assume we have a bank account. A global variable
 * 'balance' keeps track of the credit balance. The bank is not willing to
 * let our account balance to go negative.
 *
 * Several threads try to spend money by chosing a random amount, checking
 * the balance, and subtracting the amount - iff the account has funds!
 *
 * For comparison, first run this program several times with only one
 * thread, and then try again several times with multiple threads (two
 * threads are enough to finish with a negative balance eventually. More
 * threads will speed up the process).
 *
 * Compile: gcc -lpthread -o nosync nosync.c
 * Execute:
 *    Start with one thread: ./nosync
 *    Use more threads     : ./nosync <num_threads> 
 *
 * Sven Reissmann (sven.reissmann@rz.hs-fulda.de)
 */
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>

/* Configuration
 */
#define INITIAL_BALANCE 1000
#define MAX_PER_TRANSACTION 100

/* Global variable for keeping track of the accounts balance
 */
long int balance = INITIAL_BALANCE;

/* The function executed by each thread
 */
void* tfunc (void*) {
    int spend;

    while (1) {
        /* Choose a random amount to spend
         */
        spend = rand () % MAX_PER_TRANSACTION + 1;

        /* If account has enough funds, spend the chosen amount
         */
        if (balance >= spend) {
            printf ("Thread %16ld: Balance = %6ld, Spend %4d, Result %6ld\n",
                    pthread_self(), balance, spend, balance - spend);

            balance -= spend;

        /* Otherwise, exit the thread
         */
        } else {
            printf ("Thread %16ld: Balance = %6ld, Spend %4d -> balance too low!\n",
                    pthread_self(), balance, spend);

            pthread_exit ((void *) EXIT_SUCCESS);
        }
    }
}

int main (int argc, char **argv) {
    /* Get num_threads from cli, or set to 1 none was provided
     */
    long int num_threads = 1;
    if (argc > 1) {
        errno = 0;
        char *end;
        num_threads = strtol (argv[1], &end, 10);
        if (errno != 0 || *end != '\0') {
            perror ("strtol");
            exit (EXIT_FAILURE);
        }
    }

    pthread_t threads[num_threads];
    int ret[num_threads];

    printf ("Running with %ld threads\n\n", num_threads);

    /* Initialize the random number generator 
     * (Note: This is not a safe way to initialize the RNG!)
     */
    srand ((unsigned int) time (NULL));

    /* Create NUMBER_OF_THREADS
     */
    for (int i = 0; i < num_threads; i++) {
        if ((ret[i] = pthread_create (&threads[i], NULL, &tfunc, NULL)) != 0) {
            perror ("pthread_join");
            exit (EXIT_FAILURE);
        }
    }

    /* Wait for the threads to end
     */
    for (int i = 0; i < num_threads; i++) {
        if ((ret[i] = pthread_join (threads[i], NULL)) != 0) {
            perror ("pthread_join");
            exit (EXIT_FAILURE);
        }
    }

    /* Print the final balance (should be > 0), and exit
     */
    printf ("\nAll threads exited. Final balance: %ld\n", balance);
    exit (EXIT_SUCCESS);
}

