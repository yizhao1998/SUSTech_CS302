### Lecture13 File System (iNode)

### Yi Zhao 11612917

#### Lecturer Bo Tang



File number is index into iNode arrays.

**iNode**

- all pointers of a file are located together
- one directory / file has one iNode



iNode table is an array of iNodes

pointers are unbalanced tree-based

![1560082753456](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560082753456.png)

File attributes: iNode Metadata

![1560082804022](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560082804022.png)

Storage: 

- 12 pointers direct to data blocks

  4KB * 12  = 48 KB

- one indirect pointer

  (4KB / 4B) * 4KB = 4MB

- one double indirect pointer 

  (4KB / 4B) ^ 2 * 4KB = 4GB

- one triple indirect pointer 

  (4KB / 4B) ^ 3 * 4KB = 4TB



File system Ext. (Extended file system)

![1560083112732](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560083112732.png)

**File system size = block number (2 ^ (address bit length)) * block size**



Ext 2/3 - Block groups

The file system is divided into block groups and every block group has the same structure.

![1560083249702](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560083249702.png)

![1560083260151](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560083260151.png)

Block bitmap: which data block is allocated

Inode bitmap: A bit string that represents if an iNode (index-node) is allocated or not.

The number of files in the file system is fixed.



**Why having groups?** 

1. performance: spatial locality
2. Reliability: superblock and GDT are replicated in each block group.

![1560083645331](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560083645331.png)

首先在根目录下找到dir1，再在dir1下创建文件



Directory entry in a directory block

![1560083956164](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560083956164.png)

File deletion is just an update of the entry length of the previous entry.



**A hard link is a directory entry pointing to the iNode of an existing file.**

That file can accessed through two different pathnames.

![1560088099166](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560088099166.png)

每个子目录都有一个'..'链接，指向root，同时，root下有'.'和'..'指向自己，所以有22个链接指向root



![1560088191451](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560088191451.png)

当链接数量为0，iNode就被解分配了。



Symbolic link （soft link）

A symbolic link creates a new iNode.

- Symbolic link is pointing to a new iNode whose target’s pathname are stored using the space originally designed for 12 direct block and the 3 indirect block pointers if the pathname is shorter than 60 characters

- Use back a normal inode + one direct data block to hold the long pathname otherwise

- Map one name to another name

![1560096463401](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560096463401.png)



NFTS:

**Master File Table (MFT)**

-  Database with Flexible 1KB entries for metadata/data



Memory mapped files

- What if we could “map” the file directly into an empty region of our address space

- Executable files are treated this way when we exec the process!!