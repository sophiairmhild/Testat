/* Example: Anonymous Pipe
 *
 * Create a pipe, fork, and send a message from parent to child.
 *
 * Compile: gcc -o pipe pipe.c
 * Execute: ./pipe
 *
 * Sven Reissmann (sven.reissmann@rz.hs-fulda.de)
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main (int argc, char **argv) {
    pid_t pid;
    int nbytes;

    char* text = "Message from the pipe\n";
    char readbuffer[128];

    /* Array for storing the pipes file descriptors
     * (fd[0] = read, fd[1] = write)
     */
    int fd[2];

    /* Create the pipe
     */
    if (pipe (fd) == -1) {
        perror ("pipe");
        exit (EXIT_FAILURE);
    }

    /* fork
     */
    if ((pid = fork ()) == -1) {
        perror ("fork");
        exit (EXIT_FAILURE);
    }

    /* Parent
     */
    if (pid > 0) {
        /* Close the read end of the pipe, as we only want to write
         */
        close (fd[0]);

        /* Write a message to the pipe
         */
        write (fd[1], text, (strlen (text) + 1));
    }

    /* Child
     */
    if (pid == 0) {
        /* Close the write end of the pipe, as we only want to read
         */
        close (fd[1]);

        /* Read a message from the pipe
         */
        nbytes = read (fd[0], readbuffer, sizeof (readbuffer));
        printf ("Read %d bytes from pipe: %s", nbytes, readbuffer);
    }

    exit (EXIT_SUCCESS);
}

