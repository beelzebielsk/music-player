#ifndef DISK_INTERFACE_H
#define DISK_INTERFACE_H

#define BLOCKSIZE 4096
#include <stdio.h>
/* createDiskFile: Creates a file to be used as a disk. The file size
 * should be BLOCKSIZE * blocks bytes.
 *
 * If unable to open the file, or unable to write the requested amount
 * of space, then returns NULL pointer. Returns FILE pointer to the
 * created disk file on success.
 * Parameters:
 * ===========
 * filename, char * : The name of the file to create.
 * BLOCKSIZE, size_t : The size of a disk block in bytes.
 * blocks, size_t : The number of blocks in the disk.
 */
FILE* createDiskFile(char *filename, size_t blocks); 

/* openDiskFile: Opens an existing disk file, if existing. Otherwise
 * creates a new diskFile. Same return values as createDiskFile.
 *
 * Parameters:
 * ===========
 * filename, char * : The name of the file to create.
 * BLOCKSIZE, size_t : The size of a disk block in bytes.
 * blocks, size_t : The number of blocks in the disk.
 */
FILE* openDisk(char *filename, size_t blocks); 

/* readBlock: Reads a block from a disk, identified by it's file
 * pointer.
 * Parameters:
 * ===========
 * disk, FILE * : File pointer to the disk file, obtained using
 *      openDiskFile.
 * blockNumber, size_t : The block to read from the disk. 0 reads the
 *      1st block.
 * buffer, void * : The buffer to read data from the block into.
 */
int readBlock(FILE *disk, size_t blockNumber, void * buffer); 


/* writeBlock: writes a block from a disk, identified by it's file
 * pointer.
 * Parameters:
 * ===========
 * disk, FILE * : File pointer to the disk file, obtained using
 *      openDiskFile.
 * blockNumber, size_t : The block to write from the disk. 0 writes the
 *      1st block.
 * buffer, void * : The buffer to write data from the block into.
 */
int writeBlock(FILE *disk, size_t blockNumber, void * buffer);

/* syncDisk forces all outstanding writes to the disk file.
 * Parameters:
 * ===========
 * disk, FILE* : The disk to force writes to.
 */
int syncDisk(FILE *disk);
#endif
