#include<bits/stdc++.h>
#include<unistd.h>

using namespace std;

#define PROCESS_NAME_LEN 32 //进程名最大长度
#define DEFAULT_MEM_SIZE 1024  //总内存大小
#define DEFAULT_MEM_START 0  //内存开始分配时的起始地址
#define DEFAULT_PAGE_SIZE 4


int mem_size = DEFAULT_MEM_SIZE;
int page_size = DEFAULT_PAGE_SIZE;
bool flag = 0; //当内存被分配了之后，不允许更改总内存大小的flag
bool page_flag = 0;
static int pid = 0;


struct free_block {    //空闲数据块
    int start_addr;
    struct free_block *next;
};

struct allocated_block { //已分配的数据块
    int pid;
    int start_addr;
    char process_name[PROCESS_NAME_LEN];
    struct allocated_block *next;
};

free_block *free_block_head; //空闲数据块首指针
allocated_block *allocated_block_head = NULL; //分配块首指针

free_block *init_free_block(int mem_size); //空闲块初始化
void display_menu(); //显示选项菜单
void set_mem_size(); //设置内存大小
void set_page_size(); //设置页大小
int allocate_mem(int cnt, allocated_block *ab); //为制定块分配内存
void rearrange(); // 对块进行重新分配
int create_new_process(); //创建新的进程
int free_mem(int id); //释放分配块
void swap(int *p, int *q); //交换地址
void display_mem_usage(); //显示内存情况
void kill_process(); //杀死对应进程并释放其空间与结构体

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
                set_page_size();
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

free_block *init_free_block(int mem_size) { //初始化空闲块，这里的mem_size表示允许的最大虚拟内存大小
    int cnt = mem_size / page_size;
    if(mem_size % page_size != 0){
        cnt += 1;
    }
    free_block *p;
    p = (free_block *) malloc(sizeof(free_block));
    if (p == NULL) {
        puts("No memory left");
        return NULL;
    }
    p->start_addr = DEFAULT_MEM_START;
    p->next = NULL;
    free_block *tmp = p;
    for (int i = 0; i < cnt - 1; i++) {
        free_block *new_block = (free_block *) malloc(sizeof(free_block));
        if (new_block == NULL) {
            puts("No memory left");
            return NULL;
        }
        new_block->start_addr = tmp->start_addr + page_size;
        tmp->next = new_block;
        tmp = new_block;
    }

    return p;
}

void display_menu() {
    puts("\n\n******************menu*******************");
    printf("1) Set memory size (default = %d)\n", DEFAULT_MEM_SIZE);
    printf("2) Set page size (default = %d)\n", DEFAULT_PAGE_SIZE);
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
        if(memory <= 0){
            printf("The assigned memory must be positive,");
            return;
        }
        flag = 1;
        page_flag = 1;
        mem_size = memory;
        free_block_head = init_free_block(mem_size); //初始化一个可以使用的内存块，类似与操作系统可用的总存储空间
        printf("The memory size has been changed to %d.\n", memory);
    } else {
        printf("The memory cannot be changed now.\n");
    }

}

void set_page_size() { //更改最大内存大小
    if (!page_flag) {
        printf("Please input the page size.\n");
        int size;
        scanf("%d", &size);
        if(size <= 0){
            printf("The assigned page size must be positive,");
            return;
        }
        page_flag = 1;
        printf("The page size has been changed to %d.\n", page_size);
    } else {
        printf("The page size cannot be changed now.\n");
    }

}

int allocate_mem(int cnt, allocated_block *ab) { //为块分配内存，真正的操作系统会在这里进行置换等操作
    if (free_block_head == NULL) {
        printf("Memory Limit Exceed!\n");
        return -1;
    }
    int block_cnt = 0;
    free_block *tmp = free_block_head;
    free_block **recoder = (free_block **) malloc(sizeof(free_block *) * mem_size / page_size);
    while (tmp != NULL) {
        recoder[block_cnt++] = tmp;
        if (block_cnt == cnt) {
            break;
        }
        tmp =  tmp -> next;
    }
    if (block_cnt == cnt) {
        if (allocated_block_head == NULL) {
            allocated_block_head = (allocated_block *) malloc(sizeof(allocated_block));
            allocated_block_head->start_addr = recoder[0]->start_addr;
            allocated_block_head->pid = ab->pid;
            strcpy(allocated_block_head->process_name, ab->process_name);
            allocated_block *tmp = allocated_block_head;
            for (int i = 1; i < cnt; i++) {
                allocated_block *new_block = (allocated_block *) malloc(sizeof(allocated_block));
                new_block->start_addr = recoder[i]->start_addr;
                strcpy(new_block->process_name, ab->process_name);
                new_block->pid = ab->pid;
                tmp -> next = new_block;
                tmp = tmp -> next;
            }
        } else {
            allocated_block *tmp = allocated_block_head;
            allocated_block *next_tmp = allocated_block_head -> next;
            for (int i = 0; i < cnt; i++) {
                allocated_block *new_block = (allocated_block *) malloc(sizeof(allocated_block));
                new_block->start_addr = recoder[i]->start_addr;
                strcpy(new_block->process_name, ab->process_name);
                new_block -> pid = ab -> pid;
                tmp -> next = new_block;
                tmp = tmp -> next;
            }
            tmp -> next = next_tmp;
        }
        free_block *tmp = free_block_head;
        for(int i=0;i<cnt;i++){
            tmp = tmp -> next;
        }
        free_block_head = tmp;
        rearrange();
    } else {
        printf("Memory Limit Exceed!\n");
        return -1;
    }
}

int create_new_process() { //创建新进程
    int size;
    printf("Enter the process memory size, divide them by space\n");
    scanf("%d", &size);
    if (size <= 0) {
        printf("The memory of a process must be a positive number!\n");
        return -1;
    }
    int cnt = size / page_size;
    if(size % page_size != 0){
        cnt ++;
    }
    allocated_block *ab = (allocated_block *) malloc(sizeof(allocated_block));
    strcpy(ab->process_name, to_string(++pid).c_str());
    ab->pid = pid;
    allocate_mem(cnt, ab);
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
            }
            tmpx = tmpx->next;
        }
        tmp = tmp->next;
    }
    allocated_block *atmp, *atmpx;
    atmp = allocated_block_head;
    while (atmp != NULL) {
        atmpx = atmp->next;
        while (atmpx != NULL) {
            if (atmpx->start_addr < atmp->start_addr) {
                swap(&atmp->start_addr, &atmpx->start_addr);
                swap(&atmp->pid, &atmpx->pid);
                char process_name[PROCESS_NAME_LEN];
                strcpy(process_name, atmp -> process_name);
                strcpy(atmp -> process_name, atmpx -> process_name);
                strcpy(atmpx -> process_name, process_name);
            }
            atmpx = atmpx->next;
        }
        atmp = atmp->next;
    }
    usleep(500);
    puts("Rearrange Done.");
}


int free_mem(int id) { //释放某一块的内存
    allocated_block *pre = allocated_block_head;
    allocated_block *tmp = allocated_block_head;
    int cnt = 0;
    allocated_block **recorder = (allocated_block **) malloc(sizeof(allocated_block *) * mem_size / page_size);
    while(tmp != NULL){
        if(tmp -> pid == id){
            if(tmp == allocated_block_head){
               allocated_block_head = tmp -> next;
            }else{
                pre -> next = tmp -> next;
            }
            recorder[cnt++] = tmp;
        }
        else{
            if(tmp != allocated_block_head){
                pre = pre -> next;
            }
        }
        tmp = tmp -> next;
    }
    if(cnt){
        if (free_block_head == NULL) {
            free_block_head = (free_block *) malloc(sizeof(free_block));
            free_block_head->start_addr = recorder[0]->start_addr;
            free_block *tmp = free_block_head;
            for (int i = 1; i < cnt; i++) {
                free_block *new_block = (free_block *) malloc(sizeof(free_block));
                new_block->start_addr = recorder[i]->start_addr;
                tmp -> next = new_block;
                tmp = tmp -> next;
            }
        } else {
            free_block *tmp = free_block_head;
            free_block *tmp_next = tmp -> next;
            for (int i = 0; i < cnt; i++) {
                free_block *new_block = (free_block *) malloc(sizeof(free_block));
                new_block->start_addr = recorder[i]->start_addr;
                tmp -> next = new_block;
                tmp = tmp -> next;
            }
            tmp -> next = tmp_next;
        }
        rearrange();
        return 1;
    }else{
        return 0;
    }
}

void display_mem_usage() {
    free_block *fb = free_block_head;
    allocated_block *ab = allocated_block_head;
    puts("*********************Free Memory*********************");
    printf("%20s\n", "start_addr");
    int cnt = 0;
    while (fb != NULL) {
        cnt++;
        printf("%20d\n", fb->start_addr);
        fb = fb->next;
    }
    if (!cnt) puts("No Free Memory");
    else printf("Totally %d free blocks\n", cnt);
    puts("");
    puts("*******************Used Memory*********************");
    printf("%10s %20s %10s\n", "PID", "ProcessName", "start_addr");
    cnt = 0;
    while (ab != NULL) {
        cnt++;
        printf("%10d %20s %10d\n", ab->pid, ab->process_name, ab->start_addr);
        ab = ab->next;
    }
    if (!cnt) puts("No allocated block");
    else printf("Totally %d allocated blocks\n", cnt);
    return;
}

void kill_process() { //杀死某个进程
    int pid;
    puts("Please input the pid of Killed process");
    scanf("%d", &pid);
    int ans = free_mem(pid);
    if(!ans){
        printf("Process not found, kill fails.\n");
    }
}
