#include "apue.h"

int
main(void)
{
	// 改变进程当前工作目录。
	if (chdir("/tmp") < 0)
		err_sys("chdir failed");
	printf("chdir to /tmp succeeded\n");
	//sleep(20);
	exit(0);
}

// 点评
// 本例演示chdir()函数的用法，该函数可以改变进程当前的工作目录。
// 实验可以发现，shell的pwd当前工作目录并没有发生变化，这是因为
// chdir()函数只能改变自己进程的当前工作目录，而shell进程的当前
// 工作目录需要使用其内建的cd命令来实现。
