#include <unistd.h> 
#import <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include<pthread.h>
#include <sys/ptrace.h>
#include<sys/inotify.h>
#include <string.h>
#include <stdlib.h> 
#include <stdio.h> 
#include "tool.h"
#include"config.h"
char* memory = nullptr;
bool check_pagefault() {

	static bool is_first = true;
	if (is_first) {
		memory = (char*)mmap(nullptr, 0x4000, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
        print("[INFO]缺页Trap[%p]\n", memory);
		is_first = false;
	}
	int pageSize = getpagesize();
	unsigned char vec = 0;
	unsigned long addr = reinterpret_cast<unsigned long>(memory);
	unsigned long start = addr & (~(pageSize - 1));
	mincore((void*)start, pageSize, &vec);
	return vec == 1;
}
 static void * pthread_inotify(void *arg)
 {
     const int MAXLEN = 2048;
     char readbuf[MAXLEN];
     int wd[4];
     int ret, len, i;
     int fd;
     fd_set readfds;
     fd = inotify_init();
     wd[0] = inotify_add_watch(fd, "/proc/self/mem", IN_ALL_EVENTS);    if (wd[0] < 0) {print("inotify_add_watch error1\n");return NULL;}
     wd[1] = inotify_add_watch(fd, "/proc/self/pagemap", IN_ALL_EVENTS); if (wd[1] < 0) { print("inotify_add_watch error2\n"); return NULL; }
     wd[2] = inotify_add_watch(fd, "/proc/self/maps", IN_ALL_EVENTS); if (wd[2] < 0) { print("inotify_add_watch error3\n"); return NULL; }
     wd[3] = inotify_add_watch(fd, "/proc/self/smaps", IN_ALL_EVENTS); if (wd[3] < 0) { print("inotify_add_watch error4\n"); return NULL; }
 
     while (1) {
         i = 0;
         //注意要对fd_set进行初始化
         FD_ZERO(&readfds);
         FD_SET(fd, &readfds);
         //第一个参数固定要+1，第二个参数是读的fdset，第三个是写的fdset，最后一个是等待的时间
         //最后一个为NULL则为阻塞
         //select系统调用是用来让我们的程序监视多个文件句柄的状态变化
         ret = select(fd + 1, &readfds, 0, 0, 0);
         if (ret == -1) {
             print("inotify_add_watch error\n");
             inotify_rm_watch(fd, wd[0]);
             inotify_rm_watch(fd, wd[1]);
             inotify_rm_watch(fd, wd[2]);
             inotify_rm_watch(fd, wd[3]);
             close(fd);
             return NULL;
         }
         if (ret) {
             len = read(fd, readbuf, MAXLEN);
             while (i < len) {
                 //返回的buf中可能存了多个inotify_event
                 struct inotify_event* event = (struct inotify_event*)&readbuf[i];
                 //print("event mask %d\n", (event->mask & IN_ACCESS) || (event->mask & IN_OPEN));
                 //这里监控读和打开事件
                 if ((event->mask & IN_ACCESS) || (event->mask & IN_OPEN)) {
                     //事件出现则杀死父进程
                     print_error("[ERROR]check_inotify\n");
                     exit(0);
                 }
                 i += sizeof(struct inotify_event) + event->len;
             }
         }
     }
     return NULL;
 }
void check_inotify() {

    static bool is_first = true;
    if (is_first) {
        is_first = false;
        pthread_t tidp;
        if ((pthread_create(&tidp, NULL, pthread_inotify, nullptr)) == -1)
        {
            print("pthread_create error\n");
            return;
        }

    }   

}

bool check_tracepid() {
    FILE* fd;
    char context[128];
    char pidname[128];
    int pid = getpid();
    sprintf(pidname, "/proc/%d/status", pid);
    fd = fopen(pidname, "r");
    bool result = false;
    while (fgets(context, 128, fd)) {
        if (strncmp(context, "TracerPid", 9) == 0) {
            int status = atoi(&context[10]);
            fclose(fd);
            if (status != 0) {
                result =  true;
            }
            break;
        }
    }
    return result;
}

