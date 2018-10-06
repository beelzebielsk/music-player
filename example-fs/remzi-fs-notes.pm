◊chapter{36}

420 - 

There's multiple buses in a computer. Buses that connect to more
devices are further away from the CPU, and thus involve more latency.
It's better to put slower devices on this bus, because then the
latency will not be noticed as much.

This makes sense of M.2 drives. As the drives become faster, the
latency of the bus will be noticeable. THat's interesting. I wonder
what the latency of those buses are relative to each other, and how
much faster SSDs are getting.

422 -

Most I/O devices seem to have something like the following selection
of registers:

◊l{
- status: The status of the device. What is it doing?
- command: Write a command to this register, and the device will do
it. I guess it's always polling for changes to this register.
- data: Data to provide for a command. I guess the data needed changes
based on the command.
}

The general protocol to interact with a device:

◊l{
- Wait until it is ready.
- Write data for the command you're about to give it to the data
register.
- Write to the command register
- Wait until it's ready again.
}

There's different ways of copying a lot of memory stuff to the
registers of a device:

◊l{
- programmed I/O: Use the CPU to write to the registers directly.
- DMA: Directing some memory device to do the copying of the
information to the registers of the device. This prevents the CPU from
spending time with that when it can instead service another process.
}

425 -

There's different ways of writing to the device registers:

◊l{
- Special processor instructions. On the x86, those instructions are
called in and out.
- Memory mapped IO. Certain regions of memory are mapped to the
registers of the device.
}

427 -

The figure:

◊l{
- Device Driver: I guess this is the program that allows software to
interact with hardware.
- Generic Block Layer: I think this is the disk-interface that I wrote
as part of the lab.
- File System: I think this is the part that I'm working on right now.
Creating the file system stuff on top of the generic block layer.
- Application: I guess that's what stuff like ◊code{fopen} from C
calls upon.
}

429 -

The inb, outb and so on instructions are actually C instructions.
They're part of the header ◊header{sys/io.h}, and there's a manpage
for them.

the ◊code{in} family of functions takes only a port. So I guess that
they read from device registers. The ◊code{out} family of functions
take a value and a port, so they write to device registers. I think
that the status register has 8 bits, and those numbers 0-7 state the
purpose of each bit of the status register. That would make sense of
why the ◊code{ide_wait_ready} function uses the bitwise-and operator
to test for status.

We have to wait for the drive to be ready and not busy. Thus it is
busy while that condition is false: the driver is not ready or it is
busy. That gives us:

◊code{
int statusRegister = inb(0x1f7);
while (statusRegister & IDE_BSY || !(statusRegister & IDE_DRDY));
}

431 -

The references for this chapter seem pretty interesting. Especially
the smotherman document 
◊link{http://people.cs.clemson.edu/~mark/interrupts.html}.
Um. It's not quite what I expected. It seems to say more about who did
what and when and less about the evolution of ideas in this area.

◊link{http://eecs.wsu.edu/˜cs460/cs560/HDdriver.html}

◊chapter{37}

433 -

Come back to the following and lay out the answers to these questions:

◊l{
- How do modern hard-disk drives store data? 
    - on disks with a magnetically coated surface. Data is stored in
    sectors, organized into concentric circles of sectors called
    tracks. These tracks are on surfaces, and there's two surfaces to
    a platter.
- What is the interface?
    - Uh... did they answer that here? Isn't this a question for the
    next chapters?
- How is the data actually laid out and accessed? 
    - See answer to question 1.
- How does disk scheduling improve performance?
    - You try and get jobs done faster by taking advantage of the fact
    that some regions of the disk are easier to access from the
    current location than others.
}

The address space of a hard disk is the addresses of the blocks.

This explains how an hard ◊em{disk} works. It's assuming platter
drives.

I thought that a disk would have a head per track, so as to allow for
highly-parallel reading/writing. I guess not. Oh, wait. No. I thought
it would read across all of the different tracks on the difference
surfaces of all the platters.

436 -

I can see why the settling time is important. Earlier, the book said
that many hundreds of tracks could fit into the size of a human hair.
So the disk head has to land in VERY precise locations. Like, if a
human hair is .1 mm wide, and there's around 100 tracks in that width,
then the tracs are on the scale of micrometers.

So the time to read/write a single sector is:

◊l{
- Time to seek (time to place the head on the correct track).
- Time to wait for the correct sector to appear under the head.
- Time to read/write info to the sector (called a transfer).
}

438 -

The last part of the aside is a little confusing.

We're trying to figure out how long a single sector write/read would
take if it writes/reads at 100MB/second.

100 MiB / second -> 512 KiB / some amount of time

That's what we're after. We know the size of the transfer, but not the
time.

100 MiB / second

Honestly, though, you can just divide through until you get to 512 KB
on the top.

1 MB / 10 ms
512 KB / 5 ms

439 -

Let's double-check the IO math that the author did for the Cheetah:

T_{seek} = 4 ms is the average seek time. Fine
◊eql{
T_{rotation} = 15K rotations/minute 
    = 15K rotations / 15 * 4 seconds
    = 1K rotations / 4 seconds
    = 1 rotation / 4 ms
}

My time for rotation isn't matching. Later on in text, author explains
that we're calculating average times for seek, rotation, and transfer.
The average rotation is not a full rotation. It's closer to half, so
we half the full rotation time of 4ms, resulting in 2ms.

For transfer rate, we want to figure out 4Kb / some amount of time
given that we transfer at 125MB/s

◊eql{
T_{transfer} = 4K125 MB/s = 1 MB / .008 s = 1M / 8 ms = 512 KB / 4 ms
    = 128 * 4 KB / 4ms
    = 32 * 4 * 4 KB / 4 ms
    = 32 * 4 KB / 1 ms
    = 4 KB / .03125 ms
    \approx 4 KB / 30 microseconds
}

Interesting. The seek and rotation delays are most costly. They're 3
orders of magnitude worse than the transfer time of a 4 KB
transfer.

441 -

Hm... that's an interesting calculations. It assumes all seeks are
equally likely. Is that really a good assumption? Maybe. Average time
would be worse if larger seeks are more likely. Average time would be
better if smaller seeks are more likely.

442 -

Is that an optimal choice, shortest seek time 1st? Wouldn't we want
the overall shortest time for all the jobs? I dunno. This doesn't
necessarily guarantee that the total time of all the seeks would be
shortest. The head keeps moving from job to job. Maybe it determines
shorest seek time after each job is done.

444 -

I think Shortest access time first still requires some kind of scan
algorithm so that we don't ignore further away sectors forever. 

It's interesting that disk schedulers are often implemented in the
disk. I can understand that, though. The disk knows more about the
information that might make one job better to schedule than another. I
suppose this is where an OS really shines. The programmer just issues
reads/writes. The OS can try and stick to some rules of thumb about
what ought to make operations generally faster.

I wonder if there's a way to get around this, though, in case you
really know better than the OS for your particular application.

◊todo{Come back and do the exercises for this chapter}.

◊chapter{39}

Interesting. Here, directories are responsible for mapping
user-readable names to inodes.

operations

◊l{
- open: Creates a new file. Can specify an open mode for the file and
permissions
- read: Reads data from a file, which is identified by a file
descriptor.
- write: writes data from a file, which is identified by a file
descriptor.
- lseek: similar to fseek, except uses an FD. The offset has a
different type: ◊code{off_t}. Not sure what that is or means.
- fsync: Syncs unwritten data to the disk that the data lives on.
- rename: In ◊header{stdio}. Renames a file from one name to another.
Apparently replaces a file if a file of that name already exists.
- mkdir: Creates a directory. Contained in ◊header{sys/stat}.
- opendir: Opens a directory, returning a pointer to the directory.
Similar to FILE pointers, there are DIR pointers. They're both opaque
types that I know nothing about.
- readdir: This is like the analogue of read, but for directories. You
read the next file in a directory. I guess the "current" file is
stored somewhere that's pointed to by the directory pointer. Whenever
readdir is called, that current file pointer is updated.
- link: Takes in an old and new pathname. Creates a new file at the
new pathname which has the same inode as the file from the old
pathname. So, now there's two human-readable names you can use to
refer to the same underlying file. The file is identified by it's
inode number, because the inode is the data structure that keeps track
of file information.
- unlink: Takes in a pathname and "deletes" the file. In truth, what
it does is unlinks the human-readable name from the inode related to
that name. I assume that if there are no other files related to that
given inode, then the inode is deleted too, and as such the whole file
is actually deleted. Wait, inodes aren't deleted. They're deallocated
from the given file.
}

file pointers are like pointers to files. IIRC, each process's process
control block maintains a table of file info, indexed by file
pointers. FDs are local to each process because of that.

FDs are associated with an offset into a file. This offset specifies
where in the file you "are": where the next read/write will take
place. The offset gets advanced by ◊code{reads} and ◊code{writes}, or
can be changed to a chosen location via ◊code{lseek}.

◊note{filesystems must maintain some record of dirty blocks. Blocks
with modifications made to them.

◊todo{Check out references P+13 and P+14. I never knew that fsync
should be applied to the parent dir, too.}

477 -

◊q{
What's the difference between the st_dev and st_ino fields? Isn't an
inode number essentially an id number?
}

◊todo{Check out the system calls used to report file metadata using
◊code{stat}}

Apparently, directories actually contain references to themselves. I
suppose that, if directories are mappings from human-readable names to
inodes, then each directory maps ◊code{.} to the inode representing
itself and ◊code{..} to the inode representing it's parent.

480 -

◊q{
What's the "offset to the next dirent" mean here? Offset in size? In
inode? Offset relative to what? Moreover, why would different files
have different offsets? If each inode is of a fixed size, and 
}

482 -

So, hardlinks map a second human-readable name onto the inode of
another file. softlinks map a second human-readable name onto a 1st
human-readable name.

softlink files hold the path that they link to as their data. So
softlinks to larger pathnames (as in more characters) are larger than
softlinks to shorter pathnames.

◊chapter{40}

488 -

Keep track of the answers to the following two questions:

◊l{
- What are the data structures of the file system?
- How do the common system calls interact with/modify the data
structures of the file system?
}
