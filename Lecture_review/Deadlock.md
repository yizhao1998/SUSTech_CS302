### Deadlock

#### Yi Zhao 11612917

#### Lecturer Bo Tang



Starvation vs Deadlock

- Starvation: thread waits indefinitely
- Deadlock: circular waiting for resources

![1560014296045](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560014296045.png)

Four requirements for deadlock:

- Mutual exclusion
  - only one thread at a time can use a resource.
- Hold and wait
  - Thread holding at least one resource is waiting to acquire additional resources held by other threads.
- No preemption
  - Resources are released only voluntarily by the thread holding the resource, after thread is finished with it.
- Circular wait
  - There exists a set {T1, …, Tn } of waiting threads that wait each other circularly.

![1560043483088](C:\Users\11612\AppData\Roaming\Typora\typora-user-images\1560043483088.png)



**Deadlock handling methods:**

1. allow system to enter deadlock and recover
   - deadlock detection
2. ensure system will never enter a deadlock
   - deadlock avoidance

3. Ignore deadlock



1. deadlock detection algorithm

```cpp
[Avail] = [FreeResources]
Add all nodes to UNFINISHED
do {
    done = true
    Foreach node in UNFINISHED {
        if ([Requestnode] <= [Avail]) { // 可获取的资源比自己想要请求的资源多
            remove node from UNFINISHED
            [Avail] = [Avail] + [Allocnode] // 请求结束，将可获取的资源数加上自己已经被分配的资源数
            done = false
        }
    }
} until(done)
```



2. preventing deadlock

- Infinite resources
- Not sharing of resources
- do not allow waiting
- make all threads request everything that they will need at the beginning
- force all threads to request resources in a particular order preventing any cyclic use of resources



Banker's algorithm (银行家算法)

- State maximum (max) resource needs in advance
- Allow particular thread to proceed if:
  - (available resources - #requested) >= max remaining that might be needed by any thread

```cpp
[Avail] = [FreeResources]
Add all nodes to UNFINISHED
do {
done = true
Foreach node in UNFINISHED {
        if ([Maxnode]-[Allocnode] <= [Avail]) { // 最多还要请求的资源小于可分配的资源
            remove node from UNFINISHED
            [Avail] = [Avail] + [Allocnode] // 请求结束，将可获取的资源数加上自己已经被分配的资源数
            done = false
        }
    }
} until(done)
```

Keep system in a safe state,  i.e. there exists a sequence {T1 , T2 , … Tn } with T1 requesting all remaining resources, finishing, then T2 requesting all remaining resources, etc..



Banker’s algorithm with dining philosophers

- “Safe” (will not cause deadlock) if when try to grab chopstick
  either:
  - Not last chopstick
  - Is last chopstick but someone will have two afterwards



