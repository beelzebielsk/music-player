#include "filesystem.h"

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
