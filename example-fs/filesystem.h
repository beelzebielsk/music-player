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

typedef struct {
    size_t id;
    size_t* blocks;
    InodeType type;
} Inode;

#endif
