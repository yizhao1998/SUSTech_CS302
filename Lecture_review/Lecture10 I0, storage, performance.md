### Lecture10 I/0, storage, performance

#### Yi Zhao 11612917

#### Lecturer Bo Tang



![1560066728986](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560066728986.png)

![1560066769027](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560066769027.png)



Page fault: 将miss的page放入memory

![1560066920393](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560066920393.png)



**I/O**

- I/O devices are supported by I/O Controllers

- Processors accesses them by reading and writing IO registers as if they were memory



operational parameters

- Data granularity: Byte (keyboard) vs. Block (disks, networks)



**Transfer Mechanism: Programmed IO and DMA**

The goal of IO subsystem:

- Provide uniform interfaces, despite wide range of different devices

**code that controls devices (“device driver”) implements standard interface**



How does the processor talk to the device?

CPU interacts with a Controller

- Contains a set of registers that can be read and written

- May contain memory for request queues or bit-mapped images 

Regardless of the complexity of the connections and buses, processor accesses registers in two ways:

- I/O instructions: in/out instructions
  - Example from the Intel architecture: out 0x21,AL
- Memory mapped I/O: load/store instructions
  - Registers/memory appear in physical address space
  - I/O accomplished with load and store instructions



**Memory mapped example**

![1560068388561](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560068388561.png)

**Programmed I/O:**

- Each byte transferred via processor in/out or load/store
   Pro: Simple hardware, easy to program
   Con: Consumes processor cycles proportional to data size

**Direct Memory Access:**
 Give controller access to memory bus
 Ask it to transfer data blocks to/from memory directly

![1560068748037](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560068748037.png)



**I/O Device Notifying the OS**

- I/O interrupt
- polling



**Device Driver**: Device-specific code in the kernel that interacts directly with the device hardware

generally divided into two pieces

- Top half: accessed in call path from system calls
- Bottom half: run as interrupt routine



![1560069196883](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560069196883.png)

Most I/O operations are roughly linear in n bytes

- Latency(n) = Overhead (time to initiate an operation) + n/TransferCapacity
- Latency = S + n / B
- Bandwidth = n / (S + n / B)



What Determines Peak BW for I/O ?

Bus speed, Device Transfer Bandwidth



Magnetic Disk:

- Unit of transfer: Sector
- Disk tracks



Read/write data is a three-stage process:

- Seek time: position the head/arm over the proper track
- Rotational latency: wait for desired sector to rotate under r/w head
- Transfer time: transfer a block of bits (sector) under r/w head

$$
Disk Latency\\  = Queueing Time + Controller time +
Seek Time + Rotation Time + Xfer Time
$$

Example

![1560070439885](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560070439885.png)

![1560070485254](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560070485254.png)

Seek time: look for the cylinder (track)

Rotation time: look for the sector in the corresponding track



SSD (Solid State Disk)
$$
Latency = Queuing Time + Controller time + Xfer Time
$$




**I/O Performance**

Response Time = Queue + I/O device service time



Disk scheduling

FIFO

SSTF (Shortest Seek Time First selects the request with the minimum seek time from the current head position)

SCAN (The disk arm starts at one end of the disk, and moves toward the other end, servicing requests until it gets to the other end of the disk, where the head movement is reversed and
servicing continues.)

![1560071172167](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560071172167.png)

C-SCAN: When it reaches the other end, however, it immediately returns to the beginning of the disk, without servicing any requests on the return trip

![1560071262270](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560071262270.png)

LOOK & C-LOOK: Arm only goes as far as the last request in each direction, then reverses direction immediately, without first going all the way to the end of the disk (走到最远的轨道号就回头，不一定要走到最大或最小的磁轨处再回头) 

![1560071386424](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560071386424.png)

![1560071493808](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560071493808.png)

![1560071543217](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560071543217.png)

![1560071552336](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1560071552336.png)

