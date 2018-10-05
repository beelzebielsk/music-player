#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdbool.h>
#include <stdlib.h>
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

typedef struct {
    size_t length;
    bool * bits; 
} BitVector;

BitVector* createBitVector(size_t numBits) {
    BitVector* bv = malloc(sizeof(BitVector));
    bv->length = numBits;
    bv->bits = malloc(sizeof(bool) * numBits);
    return bv;
}

void freeBitVector(BitVector* bv) {
    free(bv->bits);
    free(bv);
}

typedef BitVector FreeList;

typedef enum {file, directory} InodeType;

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
    size_t id;
    InodeType type;
    size_t* blocks;
    Inode* containedNodes;
} Inode;

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
 * - inode ratio of 16384. I'm not sure what this is. For mkfs.ext4,
 *   it's bytes/per/inode. I guess that's the inode size? Nope. Create
 *   one inode for every so many bytes on the disk. What the fuck?
 *   Each inode is 256 bytes of size. 16384 = 2^14. So for every 2^14
 *   bytes of space on the disk, there's an inode of 2^8 bytes of
 *   size. How much space is left on the disk for FILES? How does that
 *   ratio even work? Is that for USABLE bytes, or even for the bytes
 *   on which an inode would reside (and other FS information?). The
 *   manpage says not to make the ratio smaller than the blocksize. If
 *   we did, then there'd be more inodes than blocks "since in that
 *   case more inodes would be made than can ever be used". I wonder
 *   why that is, exactly.
 */

#endif
