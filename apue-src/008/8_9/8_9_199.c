#include "apue.h"

static void charatatime(char *);

int
main(void)
{
	pid_t		pid;

	// 初始化父子进程同步机制
	TELL_WAIT();

	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid == 0) {
		// 子进程先将如下字符串写入标准输出。
		charatatime("output from child\n");
		// 写入完成后通知父进程。
		TELL_PARENT(getppid());
	} else {
		// 让子进程先执行，父进程阻塞等待。
		// 当收到子进程已完成的通知后，解除阻塞。
		WAIT_CHILD();			/* child goes first */
		// 此时子进程已经写完了，父进程将如下字符串写入标准输出。
		charatatime("output from parent\n");
	}

	exit(0);
}

static void
charatatime(char *str)
{
	char		*ptr;
	int			c;

	setbuf(stdout, NULL);		/* set unbuffered */
	for (ptr = str; (c = *ptr++) != 0; )
		putc(c, stdout);
}

// 点评
// 本例进程同步原理与8_9_198是一样的，但是操作顺序与其刚好相反。
// 子进程先输出，然后父进程再输出。
