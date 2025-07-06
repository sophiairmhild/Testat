/* Exercise 4.3a - Creating a child process using fork()
 *
 * Output PID and PPID, then fork, then output PID and PPID
 * within parend process and child process.
 *
 * Sven Reissmann (sven.reissmann@rz.hs-fulda.de)
 */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

int main (void) {
    pid_t pid;
    int ret;
 
    printf ("Process: PID: %d, PPID: %d\n", getpid (), getppid ());

    /* Fork
     */
    pid = fork ();

    /* Exit, if fork did not work
     */
    if (pid == -1) {
        perror ("fork");
        exit (EXIT_FAILURE);
    }

    /* This is the child (return of fork == 0)
     * (say hello and exit)
     */
    if (pid == 0) {
        printf ("Child : PID: %d, PPID: %d\n", getpid (), getppid ());

        /* exit immediately, without flushing stdio, or calling atexit()
         * (see the exit and _exit man pages for more information)
         */
        _exit (EXIT_SUCCESS);
    }

    /* This is the parent (return of fork > 0)
     * (say hello, wait for child, and exit)
     */
    if (pid > 0) {
        printf ("Parent: PID: %d, PPID: %d\n", getpid (), getppid ());

        /* Wait for the child process to exit and get its exit value
         */
        wait (&ret);
        printf ("Child exited with statuscode %d\n", WEXITSTATUS(ret));

        /* Now exit the parent process
         */
        exit (EXIT_SUCCESS);
    }
}

