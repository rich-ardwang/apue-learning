#include "apue.h"

static int		pfd1[2], pfd2[2];

void
TELL_WAIT(void)
{
	// 初始化父子进程同步机制时创建两条管道。
	if (pipe(pfd1) < 0 || pipe(pfd2) < 0)
		err_sys("pipe error");
}

void
TELL_PARENT(pid_t pid)
{
	// 子进程通知父进程。
	// 子进程向管道2的输出端pfd2[1]写入字符'c'。
	if (write(pfd2[1], "c", 1) != 1)
		err_sys("write error");
}

void
WAIT_PARENT(void)
{
	char	c;

	// 子进程阻塞等待父进程。
	// 子进程阻塞等待从管道1的输入端pfd1[0]读取字符'p'。
	if (read(pfd1[0], &c, 1) != 1)
		err_sys("read error");

	if (c != 'p')
		err_sys("WAIT_PARENT: incorrect data");
}

void
TELL_CHILD(pid_t pid)
{
	// 父进程通知子进程。
	// 父进程向管道1的输出端pfd1[1]写入字符'p'。
	if (write(pfd1[1], "p", 1) != 1)
		err_sys("write error");
}

void
WAIT_CHILD(void)
{
	char	c;

	// 父进程阻塞等待子进程。
	// 父进程阻塞等待从管道2的输入端pfd2[0]读取字符'c'。
	if (read(pfd2[0], &c, 1) != 1)
		err_sys("read error");

	if (c != 'c')
		err_quit("WAIT_CHILD: incorrect data");
}

// 点评：
// 之前的这套父子进程同步是通过Unix信号机制实现的，
// 现在用匿名管道的方式来重新实现它。
