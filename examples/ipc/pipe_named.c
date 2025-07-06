/* Example: Named Pipe (FIFO)
 *
 * This program creates a named pipe (fifo) and writes a message to it.
 *
 * You can connect to the fifo within the shell (i.e., "cat <pathname>"
 * to read from it), or connect another program to the fifo (i.e., by 
 * using fork, or by writing a separate program)
 *
 * Note: It might make sense to add a signal handler for the SIGINT
 *       signal, to remove the filesystem object (/tmp/fifo) before
 *       terminating the program.
 *
 * Compile: gcc -o pipe_named pipe_named.c
 * Execute: ./pipe_named
 *
 * Sven Reissmann (sven.reissmann@rz.hs-fulda.de)
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

int main (int argc, char **argv) {
    int fd;
    char* text = "Message from the pipe\n";

    /* Location of the fifo
     */
    char* pathname = "/tmp/fifo";

    /* If the fifo exists already, remove it
     */
    unlink (pathname);

    /* Create the fifo
     */
    if (mkfifo (pathname, 0660)) {
        perror ("mkfifo");
        exit (EXIT_FAILURE);
    }

    /* Open the fifo
     */
    if ((fd = open (pathname, O_WRONLY)) == -1) {
        perror ("fopen");
        exit (EXIT_FAILURE);
    }

    /* Write a message to the fifo
     */
    write (fd, text, (strlen (text) + 1));

    /* Finally remove the fifo
     */
    unlink (pathname);

    exit (EXIT_SUCCESS);
}

