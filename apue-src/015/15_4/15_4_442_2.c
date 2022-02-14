#include "apue.h"

// 管道信号处理函数。
static void		sig_pipe(int);		/* our signal handler */

int
main(void)
{
	int		n, fd1[2], fd2[2];
	pid_t	pid;
	char	line[MAXLINE];

	// 当对没有读进程的管道进行写操作，或对没有写进程的管道进行读
	// 操作时，会产生SIGPIPE信号。
	if (signal(SIGPIPE, sig_pipe) == SIG_ERR)
		err_sys("signal error");
	
	// 创建两根管道fd1和fd2，为了后面把子进程变成协同进程做准备。
	if (pipe(fd1) < 0 || pipe(fd2) < 0)
		err_sys("pipe error");

	// fork进程。
	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid > 0) {			/* parent */
		// 父进程关闭fd1管道输入端，保留其输出端。
		close(fd1[0]);
		// 父进程关闭fd2管道输出端，保留其输入端。
		close(fd2[1]);

		// 使用标准库从stdin读取数据，此时stdin是终端，
		// 标准库被设置为行缓冲。
		while (fgets(line, MAXLINE, stdin) != NULL) {
			n = strlen(line);
			// 使用系统调用write向管道输出端fd1[1]写入从终端stdin读取的数据。
			if (write(fd1[1], line, n) != n)
				err_sys("write error to pipe");
			// 从管道输入端fd2[0]读取管道输入的数据，这个数据来自于协同进程。
			if ((n = read(fd2[0], line, MAXLINE)) < 0)
				err_sys("read error from pipe");
			// 如果n为0，说明协同进程关闭了管道fd2[1]。
			if (n == 0) {
				err_msg("child closed pipe");
				break;
			}
			// 在数据尾部加入NULL，引起标准库缓冲区冲洗。
			line[n] = 0;	/* null terminate */
			// 使用标准库将从管道fd2[0]接收的数据写入终端stdout。
			if (fputs(line, stdout) == EOF)
				err_sys("fputs error");
		}

		// 检查标准库函数fgets是否工作正常。
		if (ferror(stdin))
			err_sys("fgets error on stdin");
		// 正常退出。
		exit(0);
	} else {						/* child */
		// 子进程关闭fd1管道输出端，保留其输入端。
		close(fd1[1]);
		// 子进程关闭fd2管道输入端，保留其输出端。
		close(fd2[0]);
		// 将子进程的fd1[0]管道输入端指向stdin，之后关闭fd1[0]。
		if (fd1[0] != STDIN_FILENO) {
			if (dup2(fd1[0], STDIN_FILENO) != STDIN_FILENO)
				err_sys("dup2 error to stdin");
			close(fd1[0]);
		}
		// 将子进程的fd2[1]管道输出端指向stdout，之后关闭fd2[1]。
		if (fd2[1] != STDOUT_FILENO) {
			if (dup2(fd2[1], STDOUT_FILENO) != STDOUT_FILENO)
				err_sys("dup2 error to stdout");
			close(fd2[1]);
		}
		// 至此，子进程可以以协同进程的身份运行shell命令，成为协同进程。
		// 这里参数2可以随便指定，15_4_442不要接收参数。
		if (execl("./15_4_444", "add2", (char *)0) < 0)
			err_sys("execl error");
	}
	exit(0);
}

static void
sig_pipe(int signo)
{
	printf("SIGPIPE caught\n");
	exit(1);
}

// 点评：
// 本例主要演示协同进程是如何工作的。
// 将15_4_442作为协同进程来调用，这个协同进程从stdin读入两个数，
// 然后计算它们的和，最后将结果写入stdout。
// 15_4_442_2进程作为调用进程，它首先创建两根管道，然后fork进程，
// 让子进程调用协同进程15_4_442，父进程与子进程之间有两根管道建立
// 双向连接，全双工的工作方式。

// 概念：
// 如果父子进程间只有一根管道，则子进程可能是过滤程序所使用的进程，
// 这种数据传输是单向的，半双工的。
// 如果父子进程间有两根管道，且两根管道的数据传输方向相反，则子进程
// 执行execl系列函数所产生的进程称为协同进程，工作方式为全双工。
