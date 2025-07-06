/* Example: Measuring real time (wall-clock time)
 *
 * Maybe you want to measure time from within your C programm
 * (i.e., to compare thread performance, or find out how long
 * a thread blocks at a specific operation).
 * Here you go ...
 *
 * Compile: gcc -o stopwatch stopwatch.c
 * Execute: ./stopwatch
 *
 * Sven Reissmann (sven.reissmann@rz.hs-fulda.de)
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <time.h>

double timediff (struct timespec *start, struct timespec *stop) {
    long seconds, nanoseconds;

    seconds = stop->tv_sec - start->tv_sec;
    nanoseconds = stop->tv_nsec - start->tv_nsec;
    if (nanoseconds < 0) {
        seconds -= 1;
        nanoseconds += 1e9;
    }
    return seconds + nanoseconds * 1e-9;
}

int main (int argc, char **argv) {
    struct timespec start, stop;
    double elapsed;

    /* Initialize the random number generator to allow to sleep for a 
     * random time between time measurements.
     * (Not a safe way for good random, but good enough for this example.
     *  -> cryptographically better random numbers: libsodium)
     */
    srand ((unsigned int) time (NULL));

    /* Start the timer
     */
    clock_gettime (CLOCK_MONOTONIC, &start);

    /* Here, you do the things that you want to mesaure the time for.
     *
     * For this example, we just sleep for a random time.
     * Note: I am using a data type of known size here, as the rand()
     * function will assume RAND_MAX depending on the datatype.
     */
    uint16_t r;
    r = rand ();
    printf ("Sleeping for %" PRIu16 " useconds\n", r);
    usleep (r);

    /* Stop the timer
     */
    clock_gettime (CLOCK_MONOTONIC, &stop);

    /* Calculate difference and print
     */
    elapsed = timediff (&start, &stop);
    printf ("Elapsed time: %f seconds\n", elapsed);

    exit (EXIT_SUCCESS);
}

