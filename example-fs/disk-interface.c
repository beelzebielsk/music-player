#include "disk-interface.h"
#include <err.h>
#include <errno.h>

FILE* createDiskFile(char* filename, size_t blocks) {
    FILE* diskFile = fopen(filename, "w+");
    if (!diskFile) {
        perror("createDiskFile could not open file");
        //err(errno, "%s", "createDiskFile could not open file");
        return NULL;
    }
    size_t bytesToWrite = BLOCKSIZE * blocks;
    const void * data = (void*)"";
    size_t bytesWritten = fwrite(data, 1, bytesToWrite, diskFile);
    for (int i = 0; i < bytesToWrite; i++) {
        fputc('\0', diskFile);
    }
    /* This didn't work because you don't read the exact same
     * size-wide region of memory over and over again. You read
     * consecutive regions of memory from the buffer, each of which is
     * size-wide, where size is the 2nd argument to fwrite.
    fwrite(data, 1, bytesToWrite, diskFile); 
    if (bytesWritten < bytesToWrite) {
        perror("createDiskFile could not write to file");
        //err(errno, "%s", "createDiskFile could not open file");
        return NULL;
    }
    */
    int status = syncDisk(diskFile);
    if (status < 0) {
        perror("createDiskFile could not sync file");
        //err(errno, "%s", "createDiskFile could not open file");
        return NULL;
    }
    return diskFile;
}

FILE* openDisk(char* filename, size_t blocks) {
    FILE* diskFile = fopen(filename, "r+");
    if (!diskFile) {
        diskFile = createDiskFile(filename, blocks);
    }
    return diskFile;
}

int readBlock(FILE* disk, size_t blockNumber, void* buffer) {
    fseek(disk, BLOCKSIZE * blockNumber, SEEK_SET);
    int blocksRead = fread(buffer, BLOCKSIZE, 1, disk);
    if (blocksRead < 1) {
        perror("readBlock could not read disk");
        return -1;
    }
    return 0;
}

int writeBlock(FILE* disk, size_t blockNumber, void* buffer) {
    int status = fseek(disk, BLOCKSIZE * blockNumber, SEEK_SET);
    if (status < 0) {
        perror("Writeblock could not seek in disk.");
        return -1;
    }
    size_t amountWritten = fwrite(buffer, BLOCKSIZE, 1, disk);
    if (amountWritten < 1) {
        perror("writeBlock could not write disk");
        return -1;
    }
    return 0;
}

int syncDisk(FILE* disk) {
    int status = fflush(disk);
    if (status == EOF) {
        perror("syncDisk failed");
        //err(errno, "%s", "syncDisk failed");
        return -1;
    }
    return 0;
}
