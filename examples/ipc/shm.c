/* Example: Shared memory
 *
 * Create an anonymous shared memory segment, copy an initial message
 * to it, read it from both, a parent and a child process, then modify
 * the message and read it again.
 *
 * Note: Using MAP_ANONYMOUS creates a shared memory segment, which is
 * NOT backed by a file. This is more simple, but might be less portable
 * as it is not specified by POSIX (see man 2 mmap).
 *
 * Please note: It makes more sense, to map a datastructure, of course.
 * Also note: When working with shared memory, syncronization is needed!
 *            We will have a look at this in the next lecture.
 *
 * Compile: gcc -Wall -o shm shm.c
 * Execute: ./shm
 *
 * Sven Reissmann (sven.reissmann@rz.hs-fulda.de)
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>

#define SHM_SIZE 128

int main() {
    pid_t pid;

    char msg1[] = "Initial message (set by parent before fork)";
    char msg2[] = "Message set by the child process";

    /* Configuration for the mmap() protection parameter.
     * The protection determines whether read, write, execute, or some
     * combination of accesses are permitted to the data being mapped.
     */
    int protection = PROT_READ | PROT_WRITE;

    /* Configuration for the mmap() flags parameter.
     * The flags argument determines whether updates to the mapping are
     * visible to other processes mapping the same region, and whether
     * updates are carried through to the underlying file.
     */
    int flags = MAP_SHARED | MAP_ANONYMOUS;

    /* Create the shared memory segment.
     * Due to MAP_ANONYMOUS, we can just pass -1 and 0 for the last two
     * arguments, which would otherwise be a file descriptor and size.
     */
    void* shmem = mmap (NULL, SHM_SIZE, protection, flags, -1, 0);
    if (shmem == MAP_FAILED) {
        perror ("mmap");
        exit (EXIT_FAILURE);
    }

    /* memcopy an initial message into the shared memory segment
     */
    memcpy (shmem, msg1, sizeof (msg1));

    /* fork
     */
    if ((pid = fork ()) == -1) {
        perror ("fork");
        exit (EXIT_FAILURE);
    }

    /* child
     */
    if (pid == 0) {
        /* Read the current message from the shared memory segment
         */
        printf ("Child (%d) read: %s\n", getpid (), (char *) shmem);

        /* Modify the shared memory by copying the contents of the
         * variable msg2 into it. 
         */
        memcpy (shmem, msg2, sizeof (msg2));

        /* Read from shared memory, again.
         */
        printf ("Child (%d) wrote: %s\n", getpid (), (char *) shmem);

        _exit (EXIT_SUCCESS);
    }

    /* parent
     */
    if (pid > 0) {
        /* Read the current message from the shared memory segment
         */
        printf ("Parent (%d) read: %s\n", getpid (), (char *) shmem);

        /* wait for the child to exit (= the child changed the message),
         * then read from the shared memory again
         */
        wait (NULL);
        printf ("Parent (%d) read: %s\n", getpid (), (char *) shmem);

        /* Unmap the shm segment after it is no longer needed
         */
        munmap (shmem, SHM_SIZE);

        exit (EXIT_SUCCESS);
    }
}

