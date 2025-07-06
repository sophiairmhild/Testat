/* UNIX/Linux signal example
 *
 * This example program just goes to sleep for 300 seconds.
 * However, if any signal arrives, the sleep function call gets unblocked,
 * to allow the scheduler to assign the CPU and call the processes signal
 * handler for the received signal.
 *
 * If implemented in this way, the call to sleep does not guarantee that
 * it actually blocks for the requested amount of time.
 *
 * Compile: gcc -o sleep sleep.c
 * Execute: ./sleep
 * Actions: After start, send a signal from another terminal
 *          i.e., kill -SIGUSR1 <pid>
 *
 * Sven Reißmann (sven.reissmann@rz.hs-fulda.de)
 */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    printf ("My PID: %d\nNow sleeping for 300 seconds\n", getpid ());
    sleep (300);

    exit (EXIT_SUCCESS);
}
