#include "apue.h"
#include <fcntl.h>

static void
lockabyte(const char *name, int fd, off_t offset)
{
	// 阻塞等待的方式设置写模式的记录锁，锁定从文件开始的1个字节。
	if (writew_lock(fd, offset, SEEK_SET, 1) < 0)
		err_sys("%s: writew_lock error", name);
	printf("%s: got the lock, byte %lld\n", name, (long long)offset);
}

int
main(void)
{
	int		fd;
	pid_t	pid;

	/*
	 * Create a file and write two bytes to it.
	 */
	// 创建临时文件templock。
	if ((fd = creat("templock", FILE_MODE)) < 0)
		err_sys("creat error");
	// 在该文件中写入"ab"两个字符。
	if (write(fd, "ab", 2) != 2)
		err_sys("write error");

	// 初始化父子进程的信号同步功能。
	TELL_WAIT();
	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid == 0) {			/* child */
		// 子进程写模式锁定从该文件开始的第一个字节。
		lockabyte("child", fd, 0);
		// 发信号通知父进程，可以开始下面动作。
		TELL_PARENT(getppid());
		// 阻塞等待父进程信号。
		WAIT_PARENT();
		// 子进程收到父进程发来的信号，它写模式锁定从该文件开始的第二个字节。
		lockabyte("child", fd, 1);
	} else {						/* parent */
		// 父进程写模式锁定从该文件开始的第二个字节。
		lockabyte("parent", fd, 1);
		// 发信号通知子进程，可以开始下面的动作。
		TELL_CHILD(pid);
		// 阻塞等待子进程发信号。
		WAIT_CHILD();
		// 父进程收到子进程发来的信号，它写模式锁定从该文件开始的第一个字节。
		lockabyte("parent", fd, 0);
	}
	exit(0);
}

// 模拟记录锁发生死锁的情况，检测到死锁发生时，
// 内核必须选择一个进程出错返回，这里选择的是父进程。
