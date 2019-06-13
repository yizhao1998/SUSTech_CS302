### Lecture1 Introduction

#### Yi Zhao 11612917

#### Lecturer Bo Tang



**What is an OS:**

OS is a special layer of software that provides application software access to hardware resources.

- Convenient abstraction of complex hardware device
- Protected access to shared resources
- Security and authentication
- Communication among logical entities



An OS

Include a program **"kernel"**. 

- manages all physical devices
- expose syscalls for others to configure the kernel or build things on top.

Include some more programs

- drivers, handles the interaction between the kernel and external devices.
- shell, renders a simple command-line user interface.

Include optional programs.



![1560226206716](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560226206716.png)

进程是操作系统资源分配的基本单位,而线程是任务调度和执行的基本单位

An process: an execution instance of a program

Process has states concerning the execution. (running code line number, time left of returning CPU to others)

![1560226508718](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560226508718.png)

A shell is a program, use getchar() to get the commands.

- invoke a function fork() to create a new process, which becomes a **child process** of the **shell**. Ask the child process to **exec() certain program**, for example "ps".



**System call is a function call that is exposed by the kernel, it abstract away most low-level details.**



Library functions are usually compiled and packed inside an object called the library file.

![1560226973027](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560226973027.png)

The following arrangement is called **segmentation.**

![1560227174081](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560227174081.png)

