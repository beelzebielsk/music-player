#include "disk-interface.h"
#include <stdio.h>
/* printf family of functions
 * perror
 * FILE
 * remove
 */
#include <string.h>
/* strlen, memcmp
 */

#include <stdlib.h>
/* malloc
 * system
 */

#include <errno.h>
/* errno
 */

#include <err.h>
/* err
 */

#include <stdbool.h>

#define testBlocks 10

int catString(char * s1, char * s2, char ** result) {
    size_t length = strlen(s1) + strlen(s2);
    char * buffer = malloc(sizeof(char) * length + 1);
    sprintf(buffer, "%s %s", s1, s2);
    *result = buffer;
    return 0;
}


int printError(char * message) {
    char* errName = strerror(errno);
    char* fullMessage = 
        malloc(sizeof(char) * (strlen(errName) + 3 + strlen(message)));
    sprintf(fullMessage, "(%s) %s", errName, message);
    perror(fullMessage);
    free(fullMessage);
}

int
main() {
    char * filename = "test.disk";
    remove(filename);

    FILE* disk = openDisk(filename, 10);
    if (!disk) {
        perror("openDisk failed in test");
        return -1;
    }

    FILE* testData = fopen("/dev/urandom", "r");
    char buffers[testBlocks][BLOCKSIZE];

    for (int i = 0; i < testBlocks; i++) {
        int amountRead = fread(buffers[i], 1, BLOCKSIZE, testData);
        if (amountRead < BLOCKSIZE) {
            perror("Reading from /dev/urandom failed");
            return -1;
        }
        int status = writeBlock(disk, i, (void*)buffers[i]);
        if (status < 0) {
            err(1, "write of block %d failed", i);
        }
    }

    char blockBuffer[BLOCKSIZE];
    bool allReadsSuccessful = true;
    for (int i = testBlocks - 1; i >= 0; i--) {
        readBlock(disk, i, (void*)blockBuffer);
        if(memcmp(buffers[i], blockBuffer, BLOCKSIZE) != 0) {
            warn("Read or write of block %d failed", i);
            allReadsSuccessful = false;
        }
    }
    if (!allReadsSuccessful) return -1;

    syncDisk(disk);
}
