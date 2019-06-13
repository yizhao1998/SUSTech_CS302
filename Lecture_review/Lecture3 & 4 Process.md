### Lecture3 & 4 Process

#### Yi Zhao 11612917

#### Lecturer Bo Tang



**Process is a program in execution**

- contains every accounting information of that running program.
  - Current program counter
  - accumulated running time
  - the list of files that are currently opened by that program
  - the page table

**Process Identification:**

- Each process is given an unique ID number, PID.

**Process Creation:**

- fork() creates a process

- both the parent and the child execute the same program
- the child process starts its execution at the location that fork() is returned,  not from the beginning of the program

![1560237041100](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560237041100.png)

![1560237057345](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560237057345.png)

fork() creates child process by cloning from the parent process, including all user-space data.

![1560237414914](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560237414914.png)

fork() does not clone the following

![1560237471757](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560237471757.png)



execl() - a member of exec system call family

![1560237586547](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560237586547.png)

(NULL states the end of the program argument list)

The return or the "exit 0" in the invoked program will terminate the process, so **the statements in the original program after the exex() statement will never be executed.**

The process is changing the code that is executing and never returns to the original code.

The process calls and exec* system will replace user-space info,

- program code, memory, register value

the kernel space info is preserved

- PID, process relationship



Suspend the execution of the parent process:

**wait()**

- suspend the calling process to waiting state and return (wakes up) when 
  - one of its child process changes from running to terminated
  - a signal is received
- return immediately if
  - it has no children
  - a child terminates before the parent calls wait for

![1560238441558](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560238441558.png)

A process is a program brought by exec to the memory

- system() = fork() + exec() + wait()

![1560238653603](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560238653603.png)

When invoking a system call

![1560238746045](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560238746045.png)

Time: 

Real time , user time, system time

Real > user + sys: context switch

Real < user + sys: parallel, the time is calculated for every thread

![1560239027531](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560239027531.png)

Fork() inside kernel:

![1560239116616](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560239116616.png)

Fork() inside User:

![1560239146380](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560239146380.png)

Finish: Change the **return value in kernel space.**

![1560239233083](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560239233083.png)

![1560239353386](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560239353386.png)

**That's why a parent process shares the same terminal output stream as the child process.**



Exec()

![1560239505024](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560239505024.png)



wait() and exit()

![1560239570546](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560239570546.png)

![1560239611739](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560239611739.png)

![1560239638053](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560239638053.png)



Process ID still in the kernel's process table

- This entry is still need to allow the process that started the (now zombie) process to read its exit status.

- The status of the child is now called zombie ("terminated").

![1560239836691](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560239836691.png)



What kernel does for exit():

1. Clean up most of the allocated kernel-space memory

2. clean up the exit process's user-space memory.

3. notify the parent with SIGCHLD

![1560240015782](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560240015782.png)

![1560240040626](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560240040626.png)

![1560240078767](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560240078767.png)

Orphan process

- the init process (with pid = 1) will become step mother of all orphans, it is called re-parenting.

Procedure:

![1560241948596](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560241948596.png)

![1560241966197](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560241966197.png)

Process lifecycle

![1560242085398](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560242085398.png)

Birth: except the first process "init", every process is created using fork()

Ready:

- just created by fork()
- has been running on the CPU for some time and the OS chooses another process to run
- returning from blocked states

Run:

The OS chooses this process to be running on the CPU and changes its state to "Running"

Block:

While the process is running, it may wait form something (getc(), wait())

Interruptible wait: Reading a file

Uninterruptible wait: Some syscalls that are believed to be fast and robust.

Return to ready:

When response arrives, the status of the process changes back to ready.

Die:

The process may choose to terminate it self or force to be terminated.