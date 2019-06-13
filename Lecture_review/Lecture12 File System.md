### Lecture12 File System

### Yi Zhao 11612917

#### Lecturer Bo Tang



File System: Layer of OS that transforms block interface of disks (or other block devices) into files, Directories, etc.



Block size >= sector size, in UNIX, block size is 4KB.

- Everything inside File System is in whole size blocks
  - For example, getc(), putc()  buffers something like 4096 bytes, even if interface is one byte at a time



**Directory:**

- Basically a hierarchical structure
- Each directory is a collection of
  - Files
  - Directories
- Each has a name and attributes



**Files**

- Named permanent storage

- Contains:
  - Data: Blocks on disk somewhere
  - Metadata(attributes)
    - owner, size, last opened..
    - Access rights (R, W, X)



Basic entities on a disk:

- File: user-visible group of blocks arranged sequentially in logical space
- Directory: user-visible index mapping names to files



Access disk as linear array of sectors.

- Identify sectors as vectors [cylinder, surface, sector], sort in cylindermajor order, not used anymore
- **Logical Block Addressing (LBA): Every sector has integer address from**
  **zero up to max number of sectors**



**track free disk blocks -> using bitmap**



File system Layout:

Contiguous allocation

![1560073656745](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560073656745.png)

**File deletion:** Space de-allocation is the same as updating the root directory!

**File creation:** File creation is really bad as there exists problem of bunch of external fragmentation. Defragmentation process may help, but it is expensive as working on disks.



Linked Allocation:

Chop the storage device and data into equal sized blocks.

Fill the empty space in a block-by-block manner.

![1560073882385](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560073882385.png)

Leave 4 byte from each block as the pointer, to write the block # of the next block into the first 4 bytes of each block.

![1560073968907](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560073968907.png)

Also the file size should be kept in the root directory table.

- To facilitate “ls –l” that lists the file size of each file

- (otherwise needs to live counting how many blocks each file has)



Disadvantage:

- There will be still Internal Fragmentation.



Centralize all the block links as **FILE ALLOCATION TABLE (FAT)**

Step1: Read the root directory and retrieve the first block number.

Step2: Read the FAT to determine the location of the block.

Step 3. After reading the 2nd block, the process continues. Note that the blocks may not be contiguously allocated.
Step 4. The process stops until the FAT says the next block # is -1.

![1560079049067](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560079049067.png)

![1560079113383](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560079113383.png)

If the block size is 32KB, then the file system size is 
$$
(32 * 2^{10}) * 2^{28} = 2^{5} * 2^{10} * 2^{28} = 2 ^ {43} = 8TB
$$
FAT PARTITION:

![1560079281200](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560079281200.png)

In MS-DOS, a block is called a cluster.

directory traversal:

Step1: Read the directory file of the root directory.

Step2: Read the directory file of the subdirectory.

Directory entry:

- A 32-byte directory entry in a directory file

- A directory entry is describing a file (or a sub-directory) under a particular directory

![1560079852739](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560079852739.png)

The maximum size of a FAT32 file:

2^32 - 1 = 4G-1 bytes



LFN - Long File Name directory entry

![1560080710945](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560080710945.png)

![1560080834503](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560080834503.png)

![1560081027908](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560081027908.png)

Since the FAT has marked “EOF”, we have reached the last cluster of that file.
Note. The file size help determining how many bytes to read from the last cluster

 **When expanding file, the FAT and FSINFO need be updated**

![1560081130685](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560081130685.png)

Delete a file: 

Step1: De-allocate all the blocks involved. Update the FSINFO and FATs.

Step2: Change the first byte of the directory entry to _ (0xE5).

(0xE5 or 0x00 means unallocated.)



FAT: 

Space efficient

Delete: Lazy delete efficient but insecure

