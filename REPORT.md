# Project #4 - ECS 150 File System


This project was extremely interesting to implement since it emulated an actual
file system similar to the ones we are used to using within the various operating
systems of our choice. The file system API was implemented using the existing
disk API provided to us. The fs library functions were built using these disk
API functions and a combination of file system techniques and memory allocation.


## High Level Design Choices

Given that the specifications were already laid out for this project in the form
of different function definitions and headers, the major design choices that had
to be made centered around making sure memory management and function
specifications were met adequately. Due to this, a simple global structure
`struct` for each layer of the file system (Superblock, FAT, Root Directory,
File Descriptor Table) was used. These were all declared globally since the
values of these structures would definitely need to be shared and carried across
all sections of the file system API. Helper functions that were separately
defined across the program to perform basic error checking were of course,
declared statically (`static`) to avoid any global declaration transparency or
concurrency issues. `__attribute__` details were added to all structures in the
program to make them aligned and packed into one contiguous chunk of memory for
easier and cleaner system access. 



## Implementation 

The implementation for this `ECS150FS` file system will now be discussed in
sequential order of file organization and memory allocation too- basically from
the perspective of the file system in its execution itself:-

### Drive Mounting 

The virtually formatted partition drive that was to be 'mounted' into the file
system was a challenging part to implement. This was done by using the disk API
block functions provided to us. Since the file system was to be implemented
by asssuming input memory to be properly aligned and arranged into equally
partitioned blocks of size `4096`, it was easy to determine that the
`block_disk_open()` and `block_read()` functions would have to be used in order to
open the given virtual disk for reading and finally read the respective blocks
of memory into the different structures used to organize relevant file system
information.

### File creation and deletion

Files were created and deleted using a combination of various API functions
which were to be built. File addition was a combination of `fs_create()` and
`fs_write()` which would basically add the file name, size, offset and other index
information to the file descriptor `struct` array and the root directory, and
eventually `block_write()` the information into the respective memory block from
which an `fs_read()` would help to display the file's contents.

Deletion worked slightly similar- the file would have to be located specifically
at the file descriptor index within the root directory array and would then be
'deleted' or would have it's contents freed from the FAT array. This was carried
out using a simple deletion/nullification of a
`Fatarray[root[i].dataIndex].wordFat` element along with setting the index to
`FAT_EOC` or `0xFFFF` that helped with removal of the
file.

### Drive unmounting

The virtual drive was unmounted by writing back all the memory that was
manipulated, set, stored or freed within the disk operations using
`block_write()`. `block_disk_close()` was a disk block API function that was
used to finally 'unmount' the disk and the file system along with it.



## Algorithm Design

For this FS API, there were two important functions in particular that required
specific algorithm design. These functions were part of 'Phase 4' of the
project, which are `fs_read()` and `fs_write()`. 

These functions were implemented using a dynamic algorithm that would
constantly check for the offset position and the count value supplied to the
function, in reference to the `BLOCK_SIZE` too. The algorithm would loop until
it found an instance where count was NOT greater than the remaining block size
in relation to the offset, upon which a `block_read()` would take place and
copy the block into the buffer to be displayed in the case of `fs_read()` and
out of the buffer into the block in the case of `fs_write()`. A bounce buffer
was also used to account for the case where the file size or count bytes that
needed to be read/writen from/to the file was more than a single `BLOCK_SIZE`
or `4096` bytes of memory. This buffer would help keep track of the bits of
data from the offset to the next block's start index by performing a `memcpy()`
into it, thus saving the value of the buffer for use in the next block. The
relative offset was also calculated by performing `(offset%BLOCK_SIZE)` to
account for the high offset value in the case that the file itself spanned
across various blocks in the data block/FAT array entries.   


## Conclusion

Overall, this program was extremely interesting and enjoyable to code- the
Phase 4 algorithm bits were a little tricky to handle ( `fs_write()` does not
completely handle and work for all edge cases either) but it was a great
learning experience and an extremely fun way to sign off of the programming
aspect for this course! 





### Sources
[File System Guidelines and Testing cases](https://canvas.ucdavis.edu/courses/300146/files/folder/discussions?preview=5360020)

[DOS File System Reference Page] (https://www.win.tue.nl/~aeb/linux/fs/fat/fat-1.html)

[Void pointers] (https://stackoverflow.com/questions/1776141/passing-void-type-parameter-in-c)

[Memcpy() Cstdlib Page] (https://www.tutorialspoint.com/c_standard_library/c_function_memcpy.htm)

[GDB Execution Tutorial](https://www.cs.cmu.edu/~gilpin/tutorial/)

[Valgrind Memory Testing] (http://valgrind.org/docs/manual/quick-start.html)
