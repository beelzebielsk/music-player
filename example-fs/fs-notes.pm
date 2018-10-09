◊overview{
The disk-interface is the disk driver. This is the program that
defines software callable functions that interact with the underlying
disk device. In this instance, the underlying disk device is a file on
a disk.

The filesystem is the thing that's built on top of the disk. That's
the "thing" that users of programs and disks see. It's the thing that
defines what files, directories, and such are, as well as how to
interact with them as a user, and store/retrieve them from the
underlying disk of the filesystem.

So there's the driver layer, that specifies how to interact with a
physical disk. Then there's the filesystem layer, which builds a thing
with files and directories and such on top of the driver layer.

The read/write/open calls must be built on top of the filesystem
layer. They all operate on files, and files are entities of a
filesystem (and thus don't exist beneath it).

The goals of your implementation are:

- Conserve space. You don't want a scheme that takes up a ton of
space.
- Conserve writes. You don't want to have to do a bunch of I/O to
alter a directory if you don't have to. Though it's worth considering
if writing data to a block always takes the same amount of time
whether or not the data to write is almost the same as the original
data, or very different from original data (for a write to a single
sector).
}

A filesystem should have:

◊l{
- A superblock, which is a block in the physical disk that stores
filesystem-wide metadata.
    - location of the inode table in the disk.
    - location of the block freelist.
    - location of the inode freelist.
    - location of the start of the data blocks.
    - Number of inodes?
    - Number of total blocks?
- An inode table, which is a series of inodes.
- Freelists of blocks and inodes, for when we need to allocate a block
or an inode for a file.
}

If the filesystem has all of these things, we should be fine.

An inode stores file-based metadata. Let's make an inode 256 bytes in
length for now. That means 4 inodes to a KB, 16 inodes to a 4KB block.

◊l{
- What type of file is it? (regular file, directory, soft link)
- What's the inode number? These should be fixed-width. Large enough
to accomodate the largest inode number. 4 bytes should be fine.
- Block addresses of allocated blocks.
- Number of allocated blocks (I'd rather not have to "zero out"
deallocated blocks or something like that).
- Indirect block pointer, in case the file is large.
- File length, in bytes.
}

What's enough of an inode length? Suppose an inode number is 4 bytes.
That's ◊${2^{32}} inodes. Suppose there's 1 inode for every 16KB of
space on the disk. How large would a disk have to be to have enough
space to exhaust all inode number with that inode ratio? The ratio is
◊${1 : 2^{14}}. 1 id to every 16KB of space on the drive. If we multiply
by ◊${2^{32}} on either side, then we get
{2^{32} : 2^{46}} which is 64 TB of space. Okay. 4 bytes is completely
fine. We have more than enough inodes ids, I think.

- Regular files store arbitrary binary data in the blocks allocated to
them.
- Directories store inode->filename mappings.
    - Each entry starts with an inode number, and is then followed by
    a null-terminated string.
    - How do I remove an entry from a directory?
    - How do I add an entry to a directory?
    - How do I tell where the end of the directory information is?
    Should I use a file length?
    - If I remove a dir entry, am I going to leave empty space in the
    file? If so how do I tell where the next entry starts?
    - Would a DAWG be easier to store/read/modify than a list of
    pairs?
        - Each node is likely not a fixed length. Each node may have
        as many children as the character set used for the file names.
        Each node has to hold a reference to child nodes, and
        permitting, say 128 characters (ascii low characters, mostly)
        leads to a large size. Each address of these children is at
        least 1 byte wide, maybe 4 bytes. Either way, we're at 128 B
        minimum for each node. That's 1/8 KB. A node can grow to be
        about 1/8 KB in size, or stay that size the whole time. I'm
        not sure there's a real benefit to that as compared to just
        storing inode/name pairs.

- Overwrite the area with null bytes.
- Inodes will be wrtten in big-endian, so that MSB is 1st and
thus always non-zero.
- So an entry starts at a non-zero byte (at an inode number).
- How would you search through for a file in this scheme?
    - The 1st pairing starts at the 1st non-null byte in the file.
    - Once found, read the next 3 bytes for the inode number.
    - Then, read up until the next null byte for the file name.
    - If this is the desired file, use this inode number.
    - If not, then keep scanning until the next non-null byte and
    repeat.
- How would you add a file to the dir?
    - Calculate the size of the entry: ◊code{4 + strlen(name) + 1}.
    - Find the 1st contiguous unused region of data with this size.
    Any region starting with a non-null byte up until the next null
    byte is occupied. NOTE: This requires the data block of the dir to
    be completely zeroed out to start, because non-null bytes after
    the last entry will be interpreted as files. That's kinda
    wasteful.
    - If this region exists, write the inode number and filename in
    this region. Finished.
    - If this region does not exist, then allocate a new data block to
    the directory, zero it out, and then write an entry to this new
    data block.
- How would you remove file from dir?
    - Find the pair of interest: scan forward for a non-null byte, and
    check to see if the inode number matches the inode of the file to
    remove (or check and compare the filename, whatever)
    - Zero out the inode number and filename.

- Alternatively, pair a directory with a freelist of extents.
Initially, a directory is empty, and the freelist contains a single
extent of the whole data block of the dir.
    - Place the freelist at the start of the file, it would be hard to
    locate it elsewhere.

- Alternatively, mark a pair as unused by reserving an inode number
that will never belong to any file (such as inode 0). Include also the
length of the record in the inode.
    - How would you scan forward through the pairs?
        - The 1st byte of the file will be the start of an inode/name
        pair, so start there. Read the inode. If the inode is valid
        (not 0), then look at the name string right after the inode
        number. The next pair is located record-length bytes ahead of
        the start of the current id/name record.
        - The reason for using record length and not "scanning until a
        null byte" is that an pair may get created, deleted, and then
        reused with a smaller filename. In this case, you may get a
        situation where you have non-null bytes, terminated by a null
        byte, and then followed by old bytes from the previously
        written filename. If we're just seeking until we find a null
        byte, then we might incorrectly interpret the gibberish bytes
        after the null byte of the most recently used filename as the
        inode of a record, which is false.
    - How would you add a pair?
        - Scan forward through the pairs, looking for the 1st invalid
        pair with record length long enough for the given filename.
        - If no such pair exists, then try to create another pair at
        the end of all the pairs in the dir file.
        - If there is not enough space, then allocate a new block to
        the dir file and create a new record after the last pair. The
        new record will prob straddle the last old block and the start
        of the new block.
    - How to remove a pair?
        - Scan forward through the pairs, looking for the 1st pair
        with the correct inode/filename. Set the inode of this pair to
        0.


◊procedures{
- allocate block: Allocate a block to file. Should automatically
handle multilevel index stuff.
    - precondition: blocks allocated prior were allocated by this
    function (meaning that they folllow a multi-level index scheme).
    - If current # of allocated blocks is < 12: add a new direct
    pointer to a block.
    - If current # of allocated blocks is = 12: get a free block and
    make that an indirect block. Set the address of this block to the
    indirect pointer of the inode. Allocate another free block to the
    1st pos of the indirect block.
    - If current # of allocated blocks is < 12 + 1024: get a free
    block and put it's address in the correct spot in the indirect
    block.
    - If current # of allocated blocks is = 12 + 1024: get a free
    block and make that the indirect block of the inode. Make the
    original indirect block the 1st block address of the new indirect
    block (we just started the 2nd level of the multi-level index).
}
