#include <stdio.h>
#include "filesystem.h"

#define assert(test, message) {if (!test) fprintf(stderr, message);} while(0)

int
main() {
    size_t diskBlocks = 4000;
    size_t expectedDiskSize = 4000 * BLOCKSIZE;
    FILE* disk = mkfs(diskBlocks);

    // Test disk file size:
    fseek(disk, 0, SEEK_END);
    size_t actualDiskSize = ftell(disk);
    assert(expectedDiskSize == actualDiskSize, "Disk size test");
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
