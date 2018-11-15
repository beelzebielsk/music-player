#include "filesystem.h"
#include <stdio.h>

/* Creating a file:
 * - Allocate and initialize an Inode.
 * - Create mapping bw this new Inode and the given filename in the
 *   filename->inode mapping.
 * - Write this information to disk. Everything that is done should be
 *   persistent.
 */

/* Growing a file:
 * - Load the Inode into memory (read it from the disk).
 * - Allocate the new disk blocks necessary for the file.
 *      - Mark them as not free in the freelist.
 *      - Add them to the list of blocks maintained by the file's
 *        Inode.
 * - Flush the modifications to disk. This means writing the Inode to
 *   the disk, and writing the contents of the new blocks to the disk.
 *
 * Questions:
 * - If I store Inodes one after the other, how will I add new inodes
 *   to a disk? Will I have to shift old inodes forward to make room
 *   for a given inode?
 * - It seems that I read/write one Inode at a time. Does that mean
 *   that modifying an Inode shouldn't mean having to rearrange
 *   Inodes?
 * - How much worse would it be to use a bitvector for every file to
 *   state which blocks a file needs? Every file uses the same size
 *   block list. However, that means the smallest file's Inode takes
 *   up as much space as the largest file's Inode.
 * - What if I did something like paging for the Inode space? I can
 *   split up the initial blocks into smaller pages and maintain a
 *   pagetable or something like that.
 * - What is the size of the smallest Inode? Typical Inode? How many
 *   of them could I shove into a block?
 */

/* To shrink a file:
 * - Load the Inode into memory (read it from the disk).
 * - Deallocate the disk blocks that are no longer needed.
 *      - Mark them as free in the freelist.
 *      - Remove them from the list of blocks maintained by the file's
 *        Inode.
 * - Flush the modifications to disk. Write the Inode to the disk, and
 *   write the contents of the new blocks to the disk.
 */

/* Allocating space for persistent info:
 * - Inodes.
 * - Freelist. This is necessarily O(blocks) (worst-case) storage,
 *   because with a fresh fs, every block is free. There's no getting
 *   around this. Another option is a list of occupied blocks, but
 *   that would be inconvenient, and still have O(blocks) worst-case
 *   storage. Using a bitvector has the same worst-case storage, and
 *   is faster to check because the index of the vector can be
 *   calculated using the inode number.
 */

long lceilint(double num) {
    int oldRoundingStyle = fegetround();
    fesetround(FE_UPWARD);
    long result = lrint(num);
    fesetround(oldRoundingStyle);
    return result;
}

BitVector* createBitVector(size_t numBits) {
    BitVector* bv = malloc(sizeof(BitVector));
    if (!bv) return NULL;

    long vectorSize = lceilint((double)numBits / 8.);
    bv->length = numBits;
    bv->capacity = vectorSize * 8;
    bv->bits = malloc(sizeof(char) * vectorSize);

    if (bv->bits == NULL) {
        free(bv); return NULL;
    }
    return bv;
}

void freeBitVector(BitVector* bv) {
    free(bv->bits);
    free(bv);
}

size_t BitVectorSize(BitVector* bv) {
    if (bv == NULL) return 0;
    return bv->capacity / 8;
}

void setBitOfByte(char* v, size_t position, bool bit) {
    char setWith = 1 << position;
    if (bit) {
        *v |= setWith;
    } else {
        *v &= ~setWith;
    }
}

bool getBitOfByte(char v, size_t position) {
    return (v & (1 << position)) >> position;
}

bool setBit(BitVector* v, size_t position, bool bit) {
    size_t bytePos = position / 8;
    size_t bitPos = position % 8;
    setBitOfByte((v->bits) + bytePos, bitPos, bit);
}

bool getBit(BitVector* v, size_t position) {
    size_t bytePos = position / 8;
    size_t bitPos = position % 8;
    return getBitOfByte(*(v->bits + bytePos), bitPos);
}

void setVector(BitVector* v, bool value) {
    size_t bytes = v->capacity / 8;
    if (value) {
        memset(v->bits, (char)0, bytes);
    } else {
        memset(v->bits, (char)255, bytes);
    }
}


#define writeInfo(thing, file) fwrite(&thing, sizeof(thing), 1, file)

int getBlockSize(int bytes) {
    return lceilint((double) bytes / BLOCKSIZE);
}

/* Writes data to a disk which should take up a certain number of
 * blocks on the disk. Writes the data, and seeks to the correct
 * position in the disk indicated by the number of blocks that the
 * data should take.
 * Parameters:
 * ===========
 * For a description of the first four parameters, see man 3 fwrite.
 * size_t numBlocks : The number of blocks that the data should take.
 *      Should be greater than getBlockSize(size * nmemb).
 */
void writeBlocks(void* ptr, size_t size, size_t nmemb, FILE* disk,
                 size_t numBlocks) {
    fpos_t* currentFilePosition;
    fgetpos(disk, currentFilePosition);
    fwrite(ptr, size, nmemb, disk);
    fsetpos(disk, currentFilePosition);
    fseek(disk, numBlocks * BLOCKSIZE, SEEK_CUR);
}

/* A struct representing the metadata written to and read from the
 * filesystem's superblock. The fields which contain "start" in their
 * name describe the disk block where that data structure starts.
 */

void writeSuperBlock(struct superBlockData* data, FILE* disk) {
    fseek(disk, 0, SEEK_SET);
    writeInfo(data->diskBlocks, disk);
    writeInfo(data->numInodes, disk);
    writeInfo(data->inodeTableStart, disk);
    writeInfo(data->inodeFreeListStart, disk);
    writeInfo(data->blockListStart, disk);
    writeInfo(data->dataBlocksStart, disk);
}

void readSuperBlock(struct superBlockData** data, FILE* disk) {
    *data = malloc(sizeof(struct superBlockData));
    if (!data) {
        return;
    }
    size_t bufSize = sizeof(size_t);
    size_t* sizetBuffer = malloc(bufSize);
    fseek(disk, 0, SEEK_SET);
    fread(sizetBuffer, bufSize, 1, disk);
    (*data)->diskBlocks = *sizetBuffer;
    fread(sizetBuffer, bufSize, 1, disk);
    (*data)->numInodes = *sizetBuffer;
    fread(sizetBuffer, bufSize, 1, disk);
    (*data)->inodeTableStart = *sizetBuffer;
    fread(sizetBuffer, bufSize, 1, disk);
    (*data)->inodeFreeListStart = *sizetBuffer;
    fread(sizetBuffer, bufSize, 1, disk);
    (*data)->blockListStart = *sizetBuffer;
    fread(sizetBuffer, bufSize, 1, disk);
    (*data)->dataBlocksStart = *sizetBuffer;
}

// TODO: Find a better calculation.
size_t getNumInodes(size_t diskBlocks) {
    return 2000;
}

FILE* mkfs(char * filename, size_t diskBlocks) {
    // Create disk file.
    FILE* disk = createDiskFile(filename, diskBlocks);
    struct superBlockData sbData;
    sbData.diskBlocks = diskBlocks;
    size_t numInodes = getNumInodes(diskBlocks);
    sbData.numInodes = numInodes;

    size_t inodeTableStart = 1; // 2nd block
    sbData.inodeTableStart = inodeTableStart;
    size_t totalInodeSpace = INODESIZE * numInodes;
    size_t inodeTableBlocks = getBlockSize(totalInodeSpace);

    size_t inodeFreeListStart = inodeTableStart + inodeTableBlocks;
    sbData.inodeFreeListStart = inodeFreeListStart;
    FreeList* inodeList = createBitVector(numInodes);
    size_t inodeFreeListBlocks = getBlockSize(BitVectorSize(inodeList));

    size_t blockListStart = inodeFreeListStart + inodeFreeListBlocks;
    sbData.blockListStart = blockListStart;
    FreeList* blockList = createBitVector(diskBlocks);
    size_t blockListBlocks = getBlockSize(BitVectorSize(blockList));

    size_t dataBlocksStart = blockListStart + blockListBlocks;
    sbData.dataBlocksStart = dataBlocksStart;

    size_t dataBlocks = diskBlocks
        - 1 // for the superblock
        - inodeFreeListBlocks
        - blockListBlocks;

    /* False for unoccupied (free), true otherwise. */
    setVector(inodeList, false);
    setVector(blockList, false);

    writeSuperBlock(&sbData, disk);
    /* no need to write anything for inode table. Allocating space for
     * it is enough, any free inode's data should be considered
     * invalid. So it's okay to have whatever gibberish there in the
     * inode table to start with.
     */
    fseek(disk, inodeFreeListStart * BLOCKSIZE, SEEK_SET);
    fwrite(inodeList->bits, 1, BitVectorSize(inodeList), disk);
    fseek(disk, blockListStart * BLOCKSIZE, SEEK_SET);
    fwrite(blockList->bits, 1, BitVectorSize(blockList), disk);
    
    return disk;
}

