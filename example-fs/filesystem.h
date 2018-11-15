#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
// memset
#include <math.h>
#include <fenv.h>
#include "disk-interface.h"

/* - Naming. Human-readable file names must be mapped to the disk
 *   blocks that they are associated with.
 * - Organization. File systems (typically) have mechanisms humans can
 *   use to organize information. Computer scientists like
 *   hierarchies, as a result, the tree directory structure has become
 *   the dominant mechanism for organization.
 * - Persistence. Data is entrusted to the file system and is expected
 *   to remain there (uncorrupted and undeleted) until an explicit
 *   request is made to delete it. This aspect includes support for
 *   allocation and deletion of files; as syntactic sugar, file
 *   systems also provide mechanisms to modify existing files.
 */

/* - I have to map a name to a series of blocks. I suppose that the
 *   bytes of the file ought to start on the 1st listed block and end
 *   in the last listed block. The blocks do not have to be in any
 *   particular order (based on block number). Just ordered by
 *   file-block number (1st block of file, 2nd block of file, ...).
 * - Creating different entities to live on the file system:
 *      - regular files
 *      - directories
 *      - maybe links?
 * - Keeping track of the free blocks. This data structure must be
 *   stored on the disk, because the data must be available once we
 *   shut off the computer (stop running the disk program).
 * - Mappings names to fs entities.
 */

/* - 1st step is to figure out how large the file system's accessible
 *   storage space should be. Then, figure out how much space (in
 *   blocks) would be necessary to store persistent data structures.
 *   The total disk size will be requested blocks + extra blocks.
 * - Allocate the blocks to the different data structures. Make
 *   procedures for reading the correct data structures from the disk.
 *   One option is to just note where the data structures are, what
 *   blocks they reside on, and prcoedures to serialize the DSes to
 *   the blocks, and read them from a series of blocks. Another option
 *   is to just write them into blocks one after the other, create
 *   ways to figure out where one DS ends and another begins, and then
 *   write a procedure to read all of the persistent DSes.
 */

/* BitVector: A struct representing a vector of bits.
 *
 * Members:
 * ========
 * length, size_t : The current length of the bit vector, in bits.
 * capacity, size_t : The current capacity of the bit vector, in bits.
 *      This is how many bits were actually allocated to the bit
 *      vector. Is always at least the length of the bv, and a
 *      multiple of 8 (since space is allocated in bytes).
 * bits, char * : A pointer to an array of bytes. The bits of each
 *      byte are the bits of the bitvector.
 */
typedef struct {
    size_t length;
    size_t capacity;
    char * bits; 
} BitVector;

long lceilint(double num);

BitVector* createBitVector(size_t numBits);
void freeBitVector(BitVector* bv);

/* Returns the amount of space the bit vector will take on disk. This
 * assumes that the bitvector's capacity is written out, and then the
 * series of bits (all capacity number of them) are written out. 
 *
 * Parameters:
 * ===========
 * bv, BitVector* : Pointer to the bitvector to measure.
 */
size_t BitVectorSize(BitVector* bv);

void setBitOfByte(char* v, size_t position, bool bit) ;

bool getBitOfByte(char v, size_t position) ;

bool setBit(BitVector* v, size_t position, bool bit) ;

bool getBit(BitVector* v, size_t position) ;

/* Sets the values of all the bits of the bitvector.
 * Parameters:
 * ===========
 * BitVector* v : The vector to set the values for.
 * bool value : The value to set all the bits to.
 */
void setVector(BitVector* v, bool value) ;

typedef BitVector FreeList;

typedef enum {file, directory} InodeType;

#define BLOCKSPERINODE 12

/* The information contained in an inode should probably be different
 * for directories and files. Directories should contain other Inodes,
 * I guess. Files should contain blocks. I could include a pointer to
 * both here. If I have a directory, then the InodeType is directory.
 * If I have a file, then the InodeType is file. For files, the block
 * pointer is non-null. For directories, the inodes pointer is
 * non-null. When I serialize, I need only write out the correct
 * pointer. Thus size in memory doesn't have to translate to size in
 * serialization.
 */
typedef struct {
    InodeType type; 
    size_t blocks[BLOCKSPERINODE]; // Data block locations
    size_t indirectBlock;
    size_t length; // Bytes in the file
    size_t numBlocks; // number of allocated blocks
    size_t linksCount;
} Inode;

#define INODESIZE 256
#define NULL_INODE 0

/* Storage of inodes:
 * - In order to make a reasonable filesystem, I have to be able to
 *   grow and shrink the storage space of an inode while doing the
 *   minimum amount of writing. The more I have to write to disk to
 *   update Inodes, the longer disk operations take.
 * 
 * Issues:
 * - In order to grow an inode, I may have to shift later inodes
 *   upward in the file.
 *      - Fixed-length inodes is a possibility (maximum file size,
 *        maximum directory size).
 * - There is not technically a finite number of inodes that can be
 *   created. There are links, there are directories, there are empty
 *   files. Empty files are the most pernicious because I ought to be
 *   able to make infinity of them. They don't take up any real space,
 *   aside from their inodes. So I can't just make an assumption about
 *   how many inodes there will be:
 *      - Decide on an inode cap. Create enough space for those. Mixed
 *        with fixed-length inodes, this gives you an upper-bound on
 *        the size of the inodes, and I can now allocate physical
 *        space for them.
 * 
 * Interestingly, the ext4 file system seems to do exactly what I've
 * done. It has a max number of inodes that the user sets, and there's
 * a pre-determined amount of space allocated to each inode. The
 * defaults of my file systems is located in /etc/mke2fs.conf:
 * - block size of 4KiB.
 * - inode size of 256 bytes.
 * - inode ratio of 16384 (one inode per 16 KB of disk space). I'm not
 *   sure what this is. For mkfs.ext4, it's bytes per inode. I guess
 *   that's the inode size? Nope. Create one inode for every so many
 *   bytes on the disk. What the fuck?  Each inode is 256 bytes of
 *   size. 16384 = 2^14. So for every 2^14 bytes of space on the disk,
 *   there's an inode of 2^8 bytes of size. How much space is left on
 *   the disk for FILES? How does that ratio even work? Is that for
 *   USABLE bytes, or even for the bytes on which an inode would
 *   reside (and other FS information?). The manpage says not to make
 *   the ratio smaller than the blocksize. If we did, then there'd be
 *   more inodes than blocks "since in that case more inodes would be
 *   made than can ever be used". I wonder why that is, exactly.
 */

/* Will create a filesystem whose total disk size is diskBlocks. From
 * those blocks, one will be put aside for a superblock, and space for
 * inodes, and space for data. 
 *
 * Parameters:
 * ===========
 * diskBlocks, size_t : The number of blocks that the disk will use,
 *      in total.
 */
FILE* mkfs(char * filename, size_t diskBlocks);

/* - Create a file
 * - Create a directory
 * - Allocate a block to a file.
 * - Allocate an inode to a file.
 * - Add a new name/inode pair to a directory.
 * - Read from a regular file.
 * - Write to a regular file.
 * - Seek in a regular file.
 * - open a file.
 * - close a file.
 */

/* A struct representing the metadata written to and read from the
 * filesystem's superblock. The fields which contain "start" in their
 * name describe the disk block where that data structure starts.
 */
struct superBlockData {
    size_t diskBlocks;
    size_t numInodes;
    size_t inodeTableStart;
    size_t inodeFreeListStart;
    size_t blockListStart;
    size_t dataBlocksStart;
};

void readSuperBlock(struct superBlockData** data, FILE* disk);

#endif
