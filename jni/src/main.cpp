#include <unistd.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 
#include "tool.h"
#include"config.h"
int main() {
	printf("[INFO]运行中,pid=[%d]\n",getpid());
	while (1) {
		bool result = false;
		result = check_pagefault();
		if (result) {
			print_error("[ERROR]check_pagefault\n");
			exit(0);
		}
		check_inotify();
		result = check_tracepid();
		if (result) {
			print_error("[ERROR]check_tracepid\n");
			exit(0);
		}
		sleep(1);
	}
	return 0;
}