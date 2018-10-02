#include "disk-interface.h"
#include <stdio.h>
/* printf family of functions
 * perror
 * FILE
 */
#include <string.h>
/* strlen
 */

#include <stdlib.h>
/* malloc
 * system
 */

#define testBlocks 10

int deleteFile(char * filename) {
    // "rm "
    char * command = "rm -f";
    size_t length = strlen(command) + 1 + strlen(filename);
    char * buffer = malloc(sizeof(char) * (length + 3) + 1);
    sprintf(buffer, "%s %s", command, filename);
    system(buffer);
    free(buffer);
}

int
main() {
    char * filename = "test.disk";
    deleteFile(filename);

    FILE* disk = openDisk(filename, 10);
    if (!disk) {
        perror("openDisk failed in test");
        return -1;
    }

    /*
    FILE* testData = fopen("/dev/urandom", "r");
    char buffers[testBlocks][BLOCKSIZE];

    for (int i = 0; i < testBlocks; i++) {
        fread(buffers[i], 1, BLOCKSIZE, testData);
        writeBlock(disk, i, (void*)buffers[i]);
    }

    syncDisk(disk);
    */
}
