#include<bits/stdc++.h>
#include<unistd.h>

using namespace std;

#define PROCESS_NAME_LEN 32 //进程名最大长度
#define MIN_SLICE 0 //内碎片最大大小
#define DEFAULT_MEM_SIZE 1024  //总内存大小
#define DEFAULT_MEM_START 0  //内存开始分配时的起始地址


int mem_size = DEFAULT_MEM_SIZE;
bool flag = 0; //当内存被分配了之后，不允许更改总内存大小的flag
static int pid = 0;


struct free_block {    //空闲数据块
    int size;
    int start_addr;
    struct free_block *next;
};

struct allocated_block { //已分配的数据块
    int pid;
    int size;
    int start_addr;
    char process_name[PROCESS_NAME_LEN];
    int *data;
    struct allocated_block *next;
};

free_block *free_block_head; //空闲数据块首指针
allocated_block *allocated_block_head = NULL; //分配块首指针

allocated_block *find_process(int id); //寻找pid为id的分配块
free_block *init_free_block(int mem_size); //空闲块初始化
void display_menu(); //显示选项菜单
void set_mem_size(); //设置内存大小
int allocate_mem(allocated_block *ab); //为制定块分配内存
void rearrange(); // 对块进行重新分配
int create_new_process(); //创建新的进程
int free_mem(allocated_block *ab); //释放分配块
void swap(int *p, int *q); //交换地址
int dispose(allocated_block *ab); //释放分配块结构体
void display_mem_usage(); //显示内存情况
void kill_process(); //杀死对应进程并释放其空间与结构体
void Usemy_algo(int id); //使用对应的分配算法
bool first_fit(free_block* &pre, free_block* &tmp, allocated_block *ab);
bool best_fit(free_block* &pre, free_block* &tmp, allocated_block *ab);
bool worst_fit(free_block* &pre, free_block* &tmp, allocated_block *ab);
bool (*func)(free_block* &pre, free_block* &tmp, allocated_block *ab) = first_fit;

//主函数
int main() {
    int op;
    pid = 0;
    free_block_head = init_free_block(mem_size); //初始化一个可以使用的内存块，类似与操作系统可用的总存储空间
    for (;;) {
        sleep(1);
        display_menu();
        fflush(stdin);
        scanf("%d", &op);
        switch (op) {
            case 1: {
                set_mem_size();
                break;
            }
            case 2: {
                flag = 1;
                printf("Choose the algorithm: 0 for first fit, 1 for best fit, 2 for worst fit.\n");
                int id;
                scanf("%d", &id);
                Usemy_algo(id);
                break;
            }
            case 3: {
                flag = 1;
                create_new_process();
                break;
            }
            case 4: {
                flag = 1;
                kill_process();
                break;
            }
            case 5: {
                flag = 1;
                display_mem_usage();
                break;
            }
            case 233: {
                puts("bye....");
                sleep(1);
                return 0;
            }
            default:
                printf("Please input the number between 1-5 or 233.");
                break;
        }
    }
}

allocated_block *find_process(int id) { //循环遍历分配块链表，寻找pid=id的进程所对应的块
    allocated_block *tmp;
    if (allocated_block_head == NULL) {
        return NULL;
    } else {
        tmp = allocated_block_head;
        while (tmp != NULL) {
            if (tmp->pid == id) {
                return tmp;
            }
            tmp = tmp->next;
        }
    }
    return NULL;
}

free_block *init_free_block(int mem_size) { //初始化空闲块，这里的mem_size表示允许的最大虚拟内存大小
    free_block *p;
    p = (free_block *) malloc(sizeof(free_block));
    if (p == NULL) {
        puts("No memory left");
        return NULL;
    }
    p->size = mem_size;
    p->start_addr = DEFAULT_MEM_START;
    p->next = NULL;
    return p;
}

void display_menu() {
    puts("\n\n******************menu*******************");
    printf("1) Set memory size (default = %d)\n", DEFAULT_MEM_SIZE);
    printf("2) Set memory allocation algorithm\n");
    printf("3) Create a new process\n");
    printf("4) Kill a process\n");
    printf("5) Display memory usage\n");
    printf("233) Exit\n");
}

void set_mem_size() { //更改最大内存大小
    if (!flag) {
        printf("Please input the maximum memory size.\n");
        int memory;
        scanf("%d", &memory);
        flag = 1;
        mem_size = memory;
        free_block_head = init_free_block(mem_size); //初始化一个可以使用的内存块，类似与操作系统可用的总存储空间
        printf("The memory size has been changed to %d.\n", memory);
    } else {
        printf("The memory cannot be changed now.\n");
    }

}

void insert_allocated(allocated_block *ab) {
    if (allocated_block_head == NULL) {
        allocated_block_head = ab;
    }
    allocated_block *tmp = allocated_block_head;
    allocated_block *pre = allocated_block_head;
    bool is_inserted = false;
    while (tmp != NULL) {
        if (tmp->start_addr > ab->start_addr) {
            if (tmp == allocated_block_head) {
                ab->next = tmp;
                allocated_block_head = ab;
            } else {
                ab->next = tmp;
                pre->next = ab;
            }
            is_inserted = true;
            break;
        }
        if (tmp != allocated_block_head) {
            pre = pre->next;
        }
        tmp = tmp->next;
    }
    if (!is_inserted) {
        pre->next = ab;
        ab->next = NULL;
    }
}

bool first_fit(free_block* &pre, free_block* &tmp, allocated_block *ab) {
    bool has_space = false;
    while (tmp != NULL) {
        if (tmp->size >= ab->size) {
            ab->start_addr = tmp->start_addr;
            has_space = true;
            break;
        }
        if (tmp != free_block_head) {
            pre = pre->next;
        }
        tmp = tmp->next;
    }
    return has_space;
}

bool best_fit(free_block* &pre, free_block* &tmp, allocated_block *ab) {
    bool has_space = false;
    int best = INT_MAX;
    free_block *t_pre = pre, *t_tmp = tmp;
    while (tmp != NULL) {
        if (tmp->size - ab->size < best && tmp->size - ab->size >= 0) {
            t_pre = pre;
            t_tmp = tmp;
            ab->start_addr = tmp->start_addr;
            best = tmp->size - ab->size;
            has_space = true;
        }
        if (tmp != free_block_head) {
            pre = pre->next;
        }
        tmp = tmp->next;
    }
    pre = t_pre;
    tmp = t_tmp;
    return has_space;
}

bool worst_fit(free_block* &pre, free_block* &tmp, allocated_block *ab) {
    bool has_space = false;
    int worst = -1;
    free_block *t_pre = pre, *t_tmp = tmp;
    while (tmp != NULL) {
        if (tmp->size - ab->size > worst) {
            t_pre = pre;
            t_tmp = tmp;
            ab->start_addr = tmp->start_addr;
            worst = tmp->size - ab->size;
            has_space = true;
        }
        if (tmp != free_block_head) {
            pre = pre->next;
        }
        tmp = tmp->next;
    }
    pre = t_pre;
    tmp = t_tmp;
    return has_space;
}

int allocate_mem(allocated_block *ab) { //为块分配内存，真正的操作系统会在这里进行置换等操作
    if (free_block_head == NULL) {
        printf("Memory Limit Exceed!\n");
        return -1;
    }
    free_block *tmp = free_block_head;
    free_block *pre = free_block_head;
    bool has_space = func(pre, tmp, ab);
    if (has_space) {
        if (tmp->size == ab->size) {
            if (tmp == free_block_head) {
                free_block_head = tmp->next;
            } else {
                pre->next = tmp->next;
            }
        } else {
            if (tmp == free_block_head) {
                free_block *new_block = (free_block *) malloc(sizeof(free_block));
                new_block->start_addr = tmp->start_addr + ab->size;
                new_block->size = tmp->size - ab->size;
                free_block_head = new_block;
                new_block->next = tmp->next;
            } else {
                free_block *new_block = (free_block *) malloc(sizeof(free_block));
                new_block->start_addr = tmp->start_addr + ab->size;
                new_block->size = tmp->size - ab->size;
                pre->next = new_block;
                new_block->next = tmp->next;
            }
        }
        insert_allocated(ab);
        return 1;
    } else {
        printf("Memory Limit Exceed!\n");
        return -1;
    }
}

int create_new_process() { //创建新进程
    int size;
    printf("Enter the process memory size, divide them by space\n");
    scanf("%d", &size);
    if(size <= 0){
        printf("The memory of a process must be a positive number!\n");
        return -1;
    }
    allocated_block *ab = (allocated_block *) malloc(sizeof(allocated_block));
    strcpy(ab->process_name, to_string(++pid).c_str());
    ab->size = size;
    ab->pid = pid;
    allocate_mem(ab);
    return pid;
}

void swap(int *p, int *q) {
    int tmp = *p;
    *p = *q;
    *q = tmp;
    return;
}

void rearrange() { //将块按照地址大小进行排序
    free_block *tmp, *tmpx;
    puts("Rearrange begins...");
    puts("Rearrange by address...");
    tmp = free_block_head;
    while (tmp != NULL) {
        tmpx = tmp->next;
        while (tmpx != NULL) {
            if (tmpx->start_addr < tmp->start_addr) {
                swap(&tmp->start_addr, &tmpx->start_addr);
                swap(&tmp->size, &tmpx->size);
            }
            tmpx = tmpx->next;
        }
        tmp = tmp->next;
    }
    tmp = free_block_head;
    while (tmp != NULL && tmp->next != NULL) {
        if (tmp->start_addr + tmp->size == tmp->next->start_addr) {
            tmp->size = tmp->size + tmp->next->size;
            tmp->next = tmp->next->next;
        }
        tmp = tmp->next;
    }
    usleep(500);
    puts("Rearrange Done.");
}


int free_mem(allocated_block *ab) { //释放某一块的内存
    free_block *new_block = (free_block *) malloc(sizeof(free_block));
    new_block->start_addr = ab->start_addr;
    new_block->next = NULL;
    new_block->size = ab->size;
    free_block *pre = free_block_head;
    free_block *tmp = free_block_head;
    while (tmp != NULL) {
        if (tmp->start_addr > ab->start_addr) {
            break;
        }
        if (tmp != free_block_head) {
            pre = pre->next;
        }
        tmp = tmp->next;
    }
    allocated_block *apre = allocated_block_head;
    allocated_block *atmp = allocated_block_head;
    while (atmp != NULL) {
        if (atmp == ab) {
            break;
        }
        if (atmp != allocated_block_head) {
            apre = apre->next;
        }
        atmp = atmp->next;
    }
    // no free block
    if (free_block_head == NULL) {
        free_block_head = new_block;
    }
        // get the last free block before ab
    else {
        free_block *n = free_block_head->next;
        free_block_head->next = new_block;
        new_block->next = n;
        rearrange();
    }
}

int dispose(allocated_block *fab) { //释放结构体所占的内存
    allocated_block *pre, *ab;
    if (fab == allocated_block_head) {
        allocated_block_head = allocated_block_head->next;
        free(fab);
        return 1;
    }
    pre = allocated_block_head;
    ab = allocated_block_head->next;
    while (ab != fab) {
        pre = ab;
        ab = ab->next;
    }
    pre->next = ab->next;
    free(ab);
    return 2;
}

void display_mem_usage() {
    free_block *fb = free_block_head;
    allocated_block *ab = allocated_block_head;
    puts("*********************Free Memory*********************");
    printf("%20s %20s\n", "start_addr", "size");
    int cnt = 0;
    while (fb != NULL) {
        cnt++;
        printf("%20d %20d\n", fb->start_addr, fb->size);
        fb = fb->next;
    }
    if (!cnt) puts("No Free Memory");
    else printf("Totaly %d free blocks\n", cnt);
    puts("");
    puts("*******************Used Memory*********************");
    printf("%10s %20s %10s %20s\n", "PID", "ProcessName", "start_addr", "size");
    cnt = 0;
    while (ab != NULL) {
        cnt++;
        printf("%10d %20s %10d %20d\n", ab->pid, ab->process_name, ab->start_addr, ab->size);
        ab = ab->next;
    }
    if (!cnt) puts("No allocated block");
    else printf("Totaly %d allocated blocks\n", cnt);
    return;
}

void kill_process() { //杀死某个进程
    allocated_block *ab;
    int pid;
    puts("Please input the pid of Killed process");
    scanf("%d", &pid);
    ab = find_process(pid);
    if (ab != NULL) {
        free_mem(ab);
        dispose(ab);
    } else {
        printf("Process not found, kill fails.\n");
    }
}

void Usemy_algo(int id) {
    if (id == 0) {
        func = first_fit;
        printf("");
    } else if (id == 1) {
        func = best_fit;
    } else if (id == 2) {
        func = worst_fit;
    } else {
        printf("No corresponding algorithm.\n");
    }
}

