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

// system()函数的一种实现
int
system(const char *cmdstring)		/* version without signal handling */
{
	// 进程ID
	pid_t		pid;
	// system函数返回值(提示当前进程或子进程的退出状态)
	int			status;

	// 输入参数检查，这个输入参数携带需要被执行的命令。
	if (cmdstring == NULL)
		return(1);		/* always a command processor with UNIX */

	// 父进程fork子进程
	if ((pid = fork()) < 0) {
		// 返回值-1，提示fork失败。
		status = -1;	/* probably out of processes */
	} else if (pid == 0) {		/* child */
		// 子进程立刻调用exec系列函数，执行/bin/sh -c <cmdstring>。
		// ”bin/sh"使用绝对路径执行程序
		// "sh"为程序文件名
		// “-c”给sh命令传入的参数1
		// "<cmdstring>"给sh命令传入的参数2
		// sh -c告诉shell读取下一个命令行参数作为命令的输入，而不是从标准输入或给定的文件中读取。
		execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
		// exec()执行成功是不返回的，这里返回说明exec()执行失败了。
		// 使用_exit()函数退出，不会引发流冲洗操作。
		// 使用_exit()的目的是为了防止父进程中某些标准IO流在子进程中被
		// 冲洗(因为子进程会拥有父进程标准IO流的副本)。
		_exit(127);		/* execl error */
	} else {			/* parent */
		// 父进程调用waitpid等待子进程退出，并获取其退出状态。
		// 如果waitpid出错，且出错原因为EINTR（中断）错误，则继续调用waitpid。
		while (waitpid(pid, &status, 0) < 0) {
			// 如果waitpid出错原因不是EINTR，则设置返回值为-1后跳出循环，不再继续调用waitpid。
			if (errno != EINTR) {
				status = -1;		/* error other than EINTR from waitpid() */
				break;
			}
		}
	}

	// system函数携带进程退出状态值返回。
	return (status);
}

int
main(void)
{
	// system函数返回值
	int		status;

	// 调用system函数，让其执行date命令。
	if ((status = system("date")) < 0)
		err_sys("system() error");
	// 打印出date进程的退出状态。
	pr_exit(status);

	// 调用system函数，让其执行一个实际上不存在的命令。
	if ((status = system("nosuchcommand")) < 0)
		err_sys("system() error");
	// 打印出上述子进程的退出状态。
	pr_exit(status);

	// 调用system函数，让其执行who命令，然后执行exit 44命令。
	if ((status = system("who; exit 44")) < 0)
		err_sys("system() error");
	// 打印出上述组合命令进程的退出状态。
	pr_exit(status);

	// 正常退出，冲洗标准IO流。
	exit(0);
}

// 点评
// 这个版本system函数的实现没有对信号进行处理，所以它是不完善的。
// 使用system而不是直接使用fork和exec函数的优点是，system进行了所需要的
// 各种出错处理以及各种信号处理。
