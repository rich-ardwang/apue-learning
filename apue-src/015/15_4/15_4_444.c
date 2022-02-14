#include "apue.h"

int
main(void)
{
	int		int1, int2;
	char	line[MAXLINE];

	// 强制修改默认缓冲方式为行缓冲，即使数据来自于管道或其他文件，
	// 也设置为行缓冲。
	// 行缓冲就是当一行数据可用时，立即调用fflush，这样不会一直
	// 阻塞，这种方法可以解决本例作为协同进程使用时导致的死锁。
	// 注意：
	// 这段代码如果去掉，协同进程和调用进程间产生死锁，不能正常工作。
	if (setvbuf(stdin, NULL, _IOLBF, 0) != 0)
		err_sys("setvbuf error");
	if (setvbuf(stdout, NULL, _IOLBF, 0) != 0)
		err_sys("setvbuf error");
	
	// 使用标准库从管道fd1[0]读取数据，被设置为全缓冲。
	while (fgets(line, MAXLINE, stdin) != NULL) {
		if (sscanf(line, "%d%d", &int1, &int2) == 2) {
			// 使用标准库向stdout，也就是管道fd2[1]输出数据。
			if (printf("%d\n", int1 + int2) == EOF)
				err_sys("printf error");
		} else {
			if (printf("invalid args\n") == EOF)
				err_sys("printf error");
		}
	}
	// 正常退出。
	exit(0);
}

// 点评：
// 15_4_442作为15_4_442_2的协同进程使用，工作是正常的，
// 它故意使用了read和write系统调用来实现。
// 本例改写了15_4_442，使用标准库来实现，如果将本例15_4_444
// 作为15_4_442_2的协同进程使用，将不能正常工作。
// 原因：
// 标准库函数fgets中的stdin来自管道fd1[0]，管道属于文件范畴
// 的概念，所以标准库设置其默认缓冲为全缓冲，非行缓冲，这样管道
// 没有EOF时，fgets会一直阻塞，此时15_4_442_2中从管道fd2[0]
// read时也会一直阻塞，这样就发生了死锁。

// 解决死锁的办法：
// 1.改变标准库默认缓冲方式为行缓冲，这种方法需要拥有协同进程的
// 源码，在源码中修改默认缓冲方式才行。
// 2.利用伪终端技术，让本例的stdin认为数据来自于一个终端，而不是
// 一根管道，这样会设置默认缓冲方式为行缓冲。这种方法不需要修改协同
// 进程的源码。
