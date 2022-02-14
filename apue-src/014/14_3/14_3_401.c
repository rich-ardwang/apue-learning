#include "apue.h"
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>

int
main(int argc, char *argv[])
{
	int					fd;
	pid_t				pid;
	char				buf[5];
	struct stat			statbuf;

	// 只接受2个参数，参数1为进程名称，参数2为记录锁使用的文件名称。
	if (argc != 2) {
		fprintf(stderr, "usage: %s filename\n", argv[0]);
		exit(1);
	}
	// 以读写和截断方式创建文件。
	if ((fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, FILE_MODE)) < 0)
		err_sys("open error");
	// 写入"abcdef"六个字符。
	if (write(fd, "abcdef", 6) != 6)
		err_sys("write error");

	/* turn on set-group-ID and turn off group-execute */
	// 读取文件属性。
	if (fstat(fd, &statbuf) < 0)
		err_sys("fstat error");
	// 开启设置组ID，关闭组的执行权限。
	if (fchmod(fd, (statbuf.st_mode & ~S_IXGRP) | S_ISGID) < 0)
		err_sys("fchmod error");

	// 初始化父子进程信号同步机制。
	TELL_WAIT();

	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid > 0) {		/* parent */
		/* write lock entire file */
		// 以写模式锁定整个文件记录。
		if (write_lock(fd, 0, SEEK_SET, 0) < 0)
			err_sys("write_lock error");

		// 给子进程发信号，让其解除阻塞。
		TELL_CHILD(pid);

		// 等待子进程退出，给它収尸。
		if (waitpid(pid, NULL, 0) < 0)
			err_sys("waitpid error");
	} else {					/* child */
		// 阻塞等待父进程发信号。
		WAIT_PARENT();			/* wait for parent to set lock */

		// 收到父进程发来的信号接触阻塞，
		// 设置文件IO为非阻塞模式。
		set_fl(fd, O_NONBLOCK);

		/* first let's see what error we get if region is locked */
		// 以非阻塞的方式且读模式锁定整个文件。
		if (read_lock(fd, 0, SEEK_SET, 0) !=  -1)	/* no wait */
			err_sys("child: read_lock succeeded");
		// 打印文件被其他进程锁定的信息。
		// 我们期望出错返回，EACCES或EAGAIN。
		printf("read_lock of already-locked region returns %d\n", errno);

		/* now try to read the mandatory locked file */
		// 尝试读取被写模式锁定的文件。
		// 如果系统支持强制性记录锁，则会读取失败，返回EACCES或EAGAIN。
		// 否则，读取成功，打印读出的两个字符"ab"。
		if (lseek(fd, 0, SEEK_SET) == -1)
			err_sys("lseek error");
		if (read(fd, buf, 2) < 0)
			err_ret("read failed (mandatory locking works)");
		else
			printf("read OK (no mandatory locking), buf = %2.2s\n", buf);
	}
	exit(0);
}

// 该程序确定一个系统是否支持强制性锁机制。
