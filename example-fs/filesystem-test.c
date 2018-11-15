#include <stdio.h>
#include "filesystem.h"

#define assert(test, message) {if (!(test)) fprintf(stderr, message);} while(0)
#define asserteq(actual, expected, message) {\
    if (actual != expected) {\
        fprintf(stderr, #message ": actual = %d, expected = %d\n",\
                actual, expected);}} while(0)

int
main() {
    size_t diskBlocks = 10000;
    size_t expectedDiskSize = diskBlocks * BLOCKSIZE;
    char * fsName = "filesystem-disk";
    remove(fsName);
    FILE* disk = mkfs(fsName, diskBlocks);

    // Test disk file size:
    fseek(disk, 0, SEEK_END);
    size_t actualDiskSize = ftell(disk);
    asserteq(actualDiskSize, expectedDiskSize, "Disk size test failed");

    // Test superblock reading:
    struct superBlockData* sbData;
    readSuperBlock(&sbData, disk);
    asserteq(sbData->diskBlocks, diskBlocks,
             "superblock diskBlocks");
    assert(sbData->inodeTableStart < sbData->inodeFreeListStart,
            "superblock inode table comes before inode freelist failed\n");
    assert(sbData->inodeFreeListStart < sbData->blockListStart,
            "superblock inode freelist comes before block freelist failed\n");
    assert(sbData->blockListStart < sbData->dataBlocksStart,
            "superblock block freelist comes before datablocks failed\n");

    // Test correctness(ish) of disk metadata:
    // Test if data structures are large enough.

    size_t inodeTableBlocks =
        sbData->inodeFreeListStart - sbData->inodeTableStart;
    size_t inodeFreeListBlocks =
        sbData->blockListStart - sbData->inodeFreeListStart;
    size_t blockFreeListBlocks = 
        sbData->dataBlocksStart - sbData->blockListStart;
    size_t inodeTableBytes = sbData->numInodes * INODESIZE;
    size_t inodeFreeListBytes =
        lceilint((double) sbData->numInodes / 8.);
    size_t blockFreeListBytes =
        lceilint((double) sbData->diskBlocks / 8.);
    assert(inodeTableBytes <= inodeTableBlocks * BLOCKSIZE,
            "inode table is large enough failed\n");
    assert(inodeFreeListBytes <= inodeFreeListBlocks * BLOCKSIZE,
            "inode freelist is large enough failed\n");
    assert(blockFreeListBytes <= blockFreeListBlocks * BLOCKSIZE,
            "inode table is large enough failed\n");
    printf("inode table blocks: %d\n"
           "inode freelist blocks: %d\n"
           "block freelist blocks: %d\n",
           inodeTableBlocks, inodeFreeListBlocks,
           blockFreeListBlocks);
    printf("inode table bytes: %d\n"
           "inode freelist bytes: %d\n"
           "block freelist bytes: %d\n",
            inodeTableBytes, inodeFreeListBytes,
            blockFreeListBytes);
    printf("%d\n", sbData->inodeTableStart);
    printf("%d\n", sbData->inodeFreeListStart);
    return 0;
}

/* Tests:
 * - While your file system does not have to survive random crashes at
 *   this stage, it does have to maintain state persistently. In
 *   particular, it should be able to run, be powered down, and then
 *   be ``rebooted'' in the state that it was in when it was shutdown
 *   (i.e., all files that were written at shutdown time should still
 *   be there, and no garbage files should suddenly appear).
 */
