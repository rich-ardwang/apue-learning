#include "apue.h"

int
main(void)
{
	int			n;
	int			fd[2];
	pid_t		pid;
	char		line[MAXLINE];

	// 先创建匿名管道，用户进程fd[1]的输出经由内核管道输入至本进程fd[0]。
	// 管道的数据传输方向是单向的，一般是半双功的，也有系统的管道是全双功。
	if (pipe(fd) < 0)
		err_sys("pipe error");
	// 然后fork进程，子进程会继承父进程的匿名管道，子进程也会
	// 拥有自己的fd[1]和fd[0]，数据传输方向与父进程的相同。
	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid > 0) {			/* parent */
		// 父进程关闭输入端fd[0]，向输出端fd[1]写数据。
		close(fd[0]);
		write(fd[1], "hello world\n", 12);
	} else {						/* child */
		// 子进程关闭输出端fd[1]，从输入端fd[0]读数据。
		close(fd[1]);
		n = read(fd[0], line, MAXLINE);
		// 子进程将读到的来自于父进程的数据输出至stdout。
		char out[20] = {"child: "};
		strcat(out, line);
		int l = strlen(out);
		write(STDOUT_FILENO, out, l);
	}
	exit(0);
}

// 演示匿名管道的基本使用方法。
// 注意：匿名管道只能用于属于同一个祖先的进程之间传输数据。
