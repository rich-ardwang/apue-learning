#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>
#include "apue.h"

// 打印进程退出状态的详细信息
// 详细注释参考8_6_191.c
void
pr_exit(int status)
{
	if (WIFEXITED(status))
		printf("normal termination, exit status = %d\n",
			WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		printf("abnormal termination, signal number = %d%s\n",
			WTERMSIG(status),
#ifdef  WCOREDUMP
			WCOREDUMP(status) ? " (core file generated)" : "");
#else
			"");
#endif
	else if (WIFSTOPPED(status))
		printf("child stopped, signal number = %d\n",
 			WSTOPSIG(status));
}

// 同8_13_212.c
int
system(const char *cmdstring)		/* version without signal handling */
{
	pid_t		pid;
	int			status;

	if (cmdstring == NULL)
		return(1);		/* always a command processor with UNIX */

	if ((pid = fork()) < 0) {
		status = -1;	/* probably out of processes */
	} else if (pid == 0) {		/* child */
		execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
		_exit(127);		/* execl error */
	} else {			/* parent */
		while (waitpid(pid, &status, 0) < 0) {
			if (errno != EINTR) {
				status = -1;		/* error other than EINTR from waitpid() */
				break;
			}
		}
	}

	return (status);
}

int
main(int argc, char *argv[])
{
	// 进程返回状态
	int		status;

	// 接受一个参数，实际上是接受大于一个参数，但本程序仅仅使用第一参数。
	if (argc < 2)
		err_quit("command-line argument required");

	// 调用system执行第一参数携带的命令。
	if ((status = system(argv[1])) < 0)
		err_sys("system() error");
	// 打印进程的退出状态。
	pr_exit(status);

	// 正常退出，冲洗标准IO流。
	exit(0);
}

// 点评
// 本程序可以使用命令行传参的方式执行任何命令，如果本程序被设置了设置用户ID，
// 尤其程序文件的拥有者是root时，则会引起严重的安全漏洞。
// 如果Unix机器上的任一普通用户被黑客控制，则黑客可以利用该程序以root身份
// 执行任何命令。
// 在编程时要注意，不能让拥有设置用户ID的程序调用system执行任意命令，这会是
// 高危安全漏洞。
