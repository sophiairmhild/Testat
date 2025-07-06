/* Sample program to demonstrate virtual-physical mapping of memory.
 *
 * This program reserves x GB of memory. It does this by reserving the needed
 * amount of virtual pages using the mmap() function.
 *
 * The program takes two arguments: The first argument is an integer (size of
 * memory in gigabyte to reserve), while the secont parameter controls whether
 * the memory actually gets mapped into physical memory by writing to it.
 *
 * The program is intended to play around with. Some thoughts:
 *   - Map the memory to physical or do not map it (second parameter)
 *   - Only write to every second page while monitoring the results
 *   - Use malloc() or posix_memalign() instead of mmap()
 *   - Try to allocate more memory than there is physical memory available
 *
 * Compile: gcc -o mem mem.c
 * Execute: ./mem 4 0
 *          ./mem 4 1
 * Monitor: By using pmap, /proc/<pid>/status, /proc/<pid>/maps
 *
 * Sven Reissmann (sven.reissmann@rz.hs-fulda.de)
 */
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>

int main (int argc, char **argv) {
    int i, gbytes, pagesize;
    unsigned long npages;
    int do_mapping;

    /* Get cli parameters
     */
    if (argc != 3) {
        printf ("Usage: %s <gigabyte> [0|1]\n", argv[0]);
        exit (EXIT_FAILURE);
    }
    gbytes = atoi (argv[1]);
    do_mapping = atoi (argv[2]);

    /* Print PID and wait to allow setup of monitoring
     */
    printf ("My PID: %d\n", getpid());
    printf ("Hit Enter to start\n");
    getchar();

    /* Get the memory pagesize of the system
     */
    pagesize = sysconf (_SC_PAGESIZE);

    /* Calculate how many pages to allocate
     */
    npages = ((unsigned long) gbytes << 30) / pagesize;
    printf ("Will now allocate %ld pages of %d byte\n", npages, pagesize);

    /* Go for it
     */
    for (i = 0; i < npages; ++i) {
        /* Allocate one page of memory.
         * Not keeping track of the pointers means, we will not be able to
         * free() the memory. The OS will take care when calling exit()
         */
        void *m = mmap (NULL, pagesize, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        if (m == MAP_FAILED) {
            perror ("mmap");
            break;
        }

        /* Write one byte to the allocated page, otherwise it won't
         * get mapped to physical memory by the operating system
         */
        if (do_mapping)
            memset (m, 0, 1);
    }
    printf("Done. Allocated %d pages (%lu MB)\n", i, ((unsigned long) i * pagesize) >> 20);

    printf ("Hit Enter to exit\n");
    getchar();

    exit (EXIT_SUCCESS);
}

