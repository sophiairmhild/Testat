/* Example: Measuring CPU time
 *
 * Maybe you want to measure time from within your C programm
 * (i.e., to compare thread performance, or find out how long
 * a thread blocks at a specific operation).
 * Here you go ...
 *
 * Compile: gcc -o cputime cputime.c
 * Execute: ./cputime
 *
 * Sven Reissmann (sven.reissmann@rz.hs-fulda.de)
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <time.h>

int main (int argc, char **argv) {
    clock_t start, end;
    double elapsed;

    /* Initialize the random number generator to allow to sleep for a 
     * random time between time measurements.
     * (Not a safe way for good random, but good enough for this example.
     *  -> cryptographically better random numbers: libsodium)
     */
    srand ((unsigned int) time (NULL));

    /* Start the timer
     */
    start = clock ();

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
    end = clock ();

    /* Calculate difference and print
     */
    elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf ("Elapsed time: %f seconds\n", elapsed);

    exit (EXIT_SUCCESS);
}

