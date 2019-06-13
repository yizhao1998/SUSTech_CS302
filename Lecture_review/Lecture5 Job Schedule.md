### Lecture5 Job Schedule

#### Yi Zhao 11612917

#### Lecturer Bo Tang



**context switch:** 

- scheduling is the procedure that decides which process to run next
- context switching is the actual switching procedure, from one process to another.

- whenever a process goes to blocking / waiting state

(wait() / sleep()) is called

![1560244311090](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560244311090.png)

Context switching:

Backup all current context of that process to the kernel-space's PCB.

- current register values, program counter

Then the scheduler decides to schedule another process in the ready queue, the scheduler has to load the context of that process from main memory to CPU.

![1560244772105](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560244772105.png)

![1560244784361](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560244784361.png)

**Preemptive scheduling**

When a process is chosen by the scheduler, the process would have the CPU until…

- the process voluntarily waits for I/O, or
- the process voluntarily releases the CPU, e.g., exit().
- particular kinds of interrupts (e.g., periodic clock interrupt, a new process steps in) are detected.



Algorithm evaluation:

- turnaround time: The time between the arrival of the task and the termination of the task.
- waiting time: The accumulated time that a task has waited for the CPU.



Shortest Job First (SJF)

Non-preemptive Shortest Job First

![1560245360765](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560245360765.png)

Preemptive Shortest Job First

![1560245569630](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560245569630.png)

Round-robin

- The scheduling is preemptive

- Every process is given a quantum, or the amount of time allowed to execute.
- Whenever the quantum of a process is used up (i.e., 0), the process releases the CPU and this is the preemption
- Then, the scheduler steps in and it chooses the next process which has a non-zero quantum to run.

- If all processes in the system have used up the quantum, they will be re-charged to their initial values.

![1560245788746](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560245788746.png)

Priority Scheduling

Priority + RR:

- In this course, if a task is preempted in the middle, the quantum is recharged in this course.

- process is assigned a fix priority when they are submitted to the system.

Multiple queue priority scheduling

- at different priority class, different schedulers may be deployed.



Modern process with threads

Thread: a sequential execution stream within process



Thread state

1. State shared by all threads in process / address space
   - content of memory
   - I/O state

2. State private to each thread
   - Kept in TCB(Thread Control Block)
   - CPU Registers
   - Execution Stack
3. Execution Stack
   - Parameters, temporary variables
   - Return PCs are kept while called procedures are executing

![1560247988577](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560247988577.png)\

![1560248157921](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560248157921.png)

![1560248180449](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560248180449.png)

- Process Control Block (PCBs) points to multiple Thread Control Blocks (TCBs)

Multiprocessing vs Multiprogramming

- Multiprocessing -> Multiple CPUs
- Multiprogramming -> Multiple Jobs or Processes
- Multithreading -> Multiple threads per Process

Run concurrently: 并发

parallel: 并行

![1560248674004](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560248674004.png)



Kernel versus User-Mode Threads

We have been talking about kernel threads

- Native threads supported directly by the kernel
- Every thread can run or block independently
- One process may have several threads waiting on different things



Downside of kernel threads: a bit expensive

- Need to make a crossing into kernel mode to schedule

| S.N. |                      User-Level Threads                      |                   Kernel-Level Thread                    |
| :--: | :----------------------------------------------------------: | :------------------------------------------------------: |
|  1   |     User-level threads are faster to create and manage.      |  Kernel-level threads are slower to create and manage.   |
|  2   |   Implementation is by a thread library at the user level.   |  Operating system supports creation of Kernel threads.   |
|  3   | User-level thread is generic and can run on any operating system. | Kernel-level thread is specific to the operating system. |
|  4   | Multi-threaded applications cannot take advantage of multiprocessing. |     Kernel routines themselves can be multithreaded.     |

Threading models:

Combined models of user-level threads and kernel-level threads.