// RCS DATA AT BOTTOM OF FILE


#include <stdio.h>                               // printf()
#include <stdlib.h>                              // exit()
#include <errno.h>                               // perror(), etc.
#include <sys/ipc.h>                             // Inter-process communication
#include <sys/shm.h>                             // Shared memory
#include "ipc_data.h"                            // Shared memory data structures

#define IPCD_SZ sizeof(ipcd_t)                   // Size of the IPC data structure

// MAIN
void adv_clock(int sec, int msec, int nsec, char *caller) {

    // Local variables
    char msgerr[50] = "";                        // Hold a message to be passed to perror()
    int shmid_ipcd;                              // Shared memory ID
    ipcd_t *ipcd;                                // Pointer for shared memory
    int SHMKEY;                                  // Shared memory key

    /*
        SHARED MEMORY ATTACHMENT
    */

    // Generate a shared memory key
    if ((SHMKEY = ftok(".", 47604000)) == -1) {
        sprintf(msgerr, "%s: ftok (SHMKEY)", caller);
        perror(msgerr);
        exit(1);
    }

    // Get the ID of the shared IPC data
    if ((shmid_ipcd = shmget(SHMKEY, IPCD_SZ, 0600)) == -1) {
        sprintf(msgerr, "%s: shmget", caller);
        perror(msgerr);
        exit(1);
    }

    // Get a pointer to the shared IPC data segment
    if ( ! (ipcd = (ipcd_t *)(shmat(shmid_ipcd, 0, 0)))) {
        sprintf(msgerr, "%s: shmat", caller);
        perror(msgerr);
        exit(1);
    }


    /*
        ADVANCE THE CLOCK
    */

    // Handle seconds
    ipcd->clock_sec += sec;

    // Handle milliseconds
    ipcd->clock_milli += msec;
    if ( ipcd->clock_milli >= 1000 ) {
        ipcd->clock_sec++;
        ipcd->clock_milli -= 1000;
    }

    // Handle nanoseconds
    ipcd->clock_nano += nsec;
    if ( ipcd->clock_nano >= 1000000 ) {
        ipcd->clock_milli++;
        ipcd->clock_nano -= 1000000;
    }

    return;
}
