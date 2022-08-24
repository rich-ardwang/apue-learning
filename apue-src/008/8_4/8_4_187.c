#include "apue.h"

// 全局变量声明并初始化
int		globvar = 6;		/* external variable in initialized data */

int
main(void)
{
	// 局部变量声明
	int		var;		/* automatic variable on the stack */
	pid_t	pid;  // 进程ID

	// 局部变量赋值
	var = 88;
	// 向标准输出写入字符串，注意如果是行缓冲，则换行符会立即冲洗IO流，而如果是
	// 全缓冲，则不会冲洗。
	printf("before vfork\n");	/* we don't flush stdio */
	// 调用vfork()，这个函数与fork()的区别会在点评中说明。
	if ((pid = vfork()) < 0) {
		err_sys("vfork error");
	} else if (pid == 0) {		/* child */
		// 子进程会修改全局变量和局部变量的值。
		globvar++;				/* modify parent's variabels */
		var++;
		// _exit(0)不会引发冲洗操作。
		//_exit(0);				/* child terminates, do not flash std IO stream.*/
		// 子进程退出，引发标准输出流被冲洗至标准输出。
		exit(0);   /* child exit and flash std IO stream. */
	}

	/* parent continues here */
	// 等待子进程调用exit(0)后，父进程恢复调度，打印出全局变量和局部变量的值。
	printf("pid = %ld, glob = %d, var = %d\n", (long)getpid(), globvar, var);
	// 父进程退出，引发冲洗操作。
	exit(0);
}

// 点评
// 本例主要演示vfork()函数的用法，我们将通过这个例子体会vfork与fork的区别。

// vfork()与fork()区别
// 1.vfork后内核可以保证子进程优先运行，并等待子进程调用exec类函数或exit类函数后，
// 父进程才能恢复调度，继续执行。
// 2.设计vfork的主要目的是用于fork后子进程立即调用exec类函数的使用场景，即使让子进程
// 继承父进程的存储空间，子进程调用了exec并不会去引用那个副本，所以索性就让子进程在父
// 进程的地址空间中运行，不必继承拥有自己的副本。

// 实验
// 1.由于vfork的特性，子进程修改的全局变量和局部变量都是属于父进程的，而不是自己拥有的
// 副本，所以父进程打印出的变量是被改变了的。
// 2.使用./8_4_187 > temp.out的用法，"before vfork"并不会被打印出两遍，可见子进程
// 并未继承父进程的IO缓冲区。
