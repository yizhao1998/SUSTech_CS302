### Lecture2 OS concepts

#### Yi Zhao 11612917

#### Lecturer Bo Tang



**Four fundamental OS concepts**

- Thread
  - Single unique execution context: fully describe program state
  - Program counter, registers, execution flags, stack
- Address space (with translation)
  - Program execute in an address apace that is distinct from the memory space of physical machine
- Process
  - An instance of an executing program is a process consisting of an address space and one or more threads of control
- Dual mode operation / protection
  - only the system has the ability to access certain resources.
  - The OS and the hardware are protected from user programs and user programs are isolated from one another by **controlling the address translation** from virtual address to physical address.

Four components of a computer system:

User -> System and application programs -> operating systems -> computer hardware



Computer-system operation

- One or more CPUs, device controllers connect through common bus providing access to shared memory.
- Concurrent execution of CPUs and devices competing for memory cycles.

![1560228335016](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560228335016.png)

Registers -> cache -> main memory -> solid-state disk -> hard disk -> optical disk -> magnetic tapes

![1560228443642](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560228443642.png)

Run programs:

Load instruction and data segments of executable file into memory.

Create stack and heap.

Transfer control to program.

provide services to program.

protecting OS and program.



Instruction **fetch / decode / execute**

![1560228992979](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560228992979.png)



thread:

- Certain registers hold the address of the context of the thread
  - Stack pointer holds the address of the top of the stack
- A thread is executing on a processor when it is resident in the processor registers.
- PC register holds the address of executing instructions in the thread.
- Registers hold the root state of the thread, and the rest is in memory.



Program's Address Space:

- Address Space: the set of accessible addresses + state associated with them.
- 32 bit processor -> 2^32 byte = 4GB



Multiple processors:

virtual CPU multiplex in time.

Each virtual CPU needs a structure to hold 

- PC, SP, Registers

Switch between vCPU:

- save PC, SP, registers in current state block 
- load PC, SP, registers in new state block



Virtual Machine Translation 

- simple abstraction for processes

- multiplex these abstract machines
- all virtual CPU has same non-CPU resources

- good for sharing, bad for protection



Protection

- Reliability, Security, Privacy, Fairness

- Primary mechanism: limit the translation from program address space to physical memory space.



Process

- Process: execution environment wit one or more threads
- owns memory, file descriptor, file system context
- encapsulate one or more threads sharing process resources

Application instance consists of one or more processes



![1560234243175](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560234243175.png)

Dual mode operation / protection

Hardware provide at least two modes:

- Kernel mode
- User mode

Reason:

- bit of state (user/system mode bit)

- certain operations / actions are only permitted in system / kernel mode
- User -> kernel transition sets system mode and saves user PC
- kernel -> user transition clears system mode and restores appropriate user PC



![1560234537489](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560234537489.png)

Mode transfer:

- Syscall (process request system service)
- Interrupt (external asynchronous event triggers context switch)
- Exception or Trap (internal synchronous event triggers context switch)



Get system target address of the "unprogrammed control transfer":

Interrupt vector (contains addresses and properties of each interrupt handler)



User => Kernel:

save PC to uPC, load interrupt vector in PC, save user registers and set up system stack by updating base and bound.



Kernel => User

transfer uPC to PC.