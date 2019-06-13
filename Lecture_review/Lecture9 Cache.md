### Lecture9 Cache

#### Yi Zhao 11612917

#### Lecturer Bo Tang



**Cache:**

a repository for copies that can be accessed more quickly than the original



Average Access Time
$$
AAT = {Hit\_Rate*Hit\_Time} + {Miss\_Rate * Miss\_Time}
$$


Example:

![1560060419857](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560060419857.png)



Cache acceleration principle

Temporal Locality

- Keep recently accessed data items closer to processor

Spatial Locality

- Move contiguous blocks to the upper levels



Problem: Cannot afford to translate on every access

**Cache the translations**

Translation Cache: Translation Lookaside Buffer (TLB)

![1560061029300](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560061029300.png)

Can we have a TLB hierarchy?

- Sure: multiple levels at different sizes/speeds

![1560061112053](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560061112053.png)

![1560061133465](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560061133465.png)



Demand paging

- Modern programs require a lot of physical memory, but they do not use all of their memory all the time.
- **Use memory as cache for disk.**

![1560061365187](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560061365187.png)

Principle: **Transparent Level of Indirection** (page table)

- Supports flexible placement of physical data
  - Data could be on disk or somewhere across network
- Variable location of data transparent to user program
  - Performance issue, not correctness issue

How do we find a page in the cache when look for it?

- First check TLB, then page-table traversal



Effective Access Time
$$
EAT = Hit\_Time + Miss\_Rate * Miss\_Penalty
$$
（For demand paging cost model）
$$
EAT = Memory\_Access\_Time + p * Average\_Page\_Fault\_Service\_Time \\
(p: Probability\ of \ miss)
$$
Page Replacement Policies

FIFO (first in, first out)

Min (Replace page that will not be used for the longest time)

Random (Pick random page for every replacement)

LRU (Least Recently Used)

- implement LRU: on each use, remove page form list and place at head, LRU page is at tail.



Adding memory ***help*** fault rate for MIN and LRU

Adding memory ***may not help*** fault rate for FIFO

After adding memory:

- With FIFO, contents can be completely different
- In contrast, with LRU or MIN, contents of memory with X pages are a subset of contents with X+1 Page



Clock Algorithm (Not recently used)

- Approximate LRU
- replace an old page, may not be the oldest

Hardware “use” bit per physical page:

- Hardware sets use bit on each reference
- On page fault, check use bit:
  - 1 -> used recently, clear and leave alone
  - 0 -> selected candidate for replacement



Second chance List Algorithm (Yet another approximate LRU)

![1560064318764](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560064318764.png)

Page Fault

- Always move overflow page from end of Active list to front of Second-chance list (SC) and mark invalid
- Desired Page On SC List: move to front of Active list, mark RW
- Not on SC list: page in to front of Active list, mark RW; page out LRU victim at end of SC list



Do we need a reverse mapping (i.e. physical page → virtual page)?

- Yes. Clock algorithm runs through page frames. If sharing, then multiple virtual-pages per physical page
- Can’t push page out to disk without invalidating all PTEs



Direct mapped cache:

Direct mapped 2^N byte cache

The upper most 32 - N bits are always the cache tag

Block size = 2^M

The lowest M bits are the byte select

![1560065106356](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560065106356.png)

The address throws the last M bits, and  collect the last (N - M) bits, put to the right position in the cache, add the remainder of the address to the cache tag part, cache tag will be the comparing evidence.

![1560066365918](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560066365918.png)

