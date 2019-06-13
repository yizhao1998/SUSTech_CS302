### Lecture6 Synchronization

#### Yi Zhao 11612917

#### Lecturer Bo Tang



Inter-process communication

**Pipe, Signal**

The pipe() system calls creates a piece of shared FIFO queue in the **kernel** space.



For the pipe, the buffer size is limited. -> Synchronization problem

**Producer-Consumer problem**

![1559976973505](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1559976973505.png)



For the shared objects, user-space memory is not sharable between processes, even parents and child. **Thus shared objects are provided at the kernel level.**

In a word, kernel provides pipe to do **one-way data flow between two processes from the same ancestor, which is restrictive.**



**Problem: race condition**

- Race condition means the outcome of an execution depends on a particular order in which the shared resource is accessed.

- It may happen whenever "shared object" + "multiple processes" + "concurrently"



**Resolve Race Condition:**

mutual exclusion

- not to share the shared object at the same time.
- make critical section.



**critical section (CS)** is the code segment that is accessing the shared object.

- When a process is entering the CS, other processes can not enter the CS that shares the same shared object.

- CS should be as tight as possible.
- One CS can be designed for accessing more than one shared objects.



**Entry and exit implementation** 

Requirement #1 Mutual exclusion

- No two processes could be simultaneously go inside their own critical sections.

Requirement #2 Bounded waiting

- Once a process trying to enter its CS, there is a bound on the number of times other processes can enter their CS.
- No process should wait for a resource for infinite amount of time.
- **the system should be able to tell the process that after a fixed time it will surely get access to the critical section**

Requirement #3 Progress

- One of the processes trying to enter will eventually get in.
- **If one process does not want to enter the critical section then it doesn't stop/ prevent another process from entering the critical section.**



**Understanding bounded waiting and progress**

- If no process can enter C.S, do not have progress
- If A waiting to enter its C.S, while B repeated leaves and re-enters its C.S and infinitum, then A does not have bounded waiting (but B is having progress).

![1559978614790](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1559978614790.png)

#0 – disabling interrupt for the whole CS

- To disable context switching when the process is inside the critical section
- When a process is in its critical section, no other processes could be able to run.

- correct and permissible in kernel: kernel tasks will be small, the tasks can be executed within limited time ticks.



#1 - Basic Spin Lock (busy waiting)

![1559985109107](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1559985109107.png)

Correct

- but it wastes CPU resources
- OK for short waiting
  - Especially these days we have multi-core
    - Will not block other irrelevant processes a lot
  - Ok when spin-time < context-switch-overhead

implementation: **pthread_mutex_lock() & pthread_mutex_unlock()**



#1: Basic Spin Lock violates progress

![1559985464467](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1559985464467.png)

在上述例子中，process1需要较长的时间执行remainder方法，但是process0为了take turn，把控制权交给了process1，此时progress 0就陷入了漫长的等待之中。

### 为什么违反progress，在progress执行完remainder之后仍然会再返回来，然后交回锁？

![1559986124812](C:\Users\Joy\AppData\Roaming\Typora\typora-user-images\1559986124812.png)

理解spin by Peterson

一个process进到critical section，那么这个process的interest是true，如果turn是他自己，说明对方现在不在interest。如果turn不是他自己，说明对方process是最后一个modify turn的process。对方进程会因为turn==process && interest[other] = true 被block掉。



 Busy waiting
+ shared variable turn for mutual exclusion
+ shared variables interest to resolve strict alternation

- **satisfy three criteria**

Peterson spinlock suffers from Priority Inversion

- A low priority process L is inside the critical region, but …
- A high priority process H gets the CPU and wants to enter the critical region.



