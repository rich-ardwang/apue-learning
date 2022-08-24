#include "apue.h"
#include <sys/wait.h>

// 打印进程退出状态的详细信息
void
pr_exit(int status)
{
	// WIFEXITED宏判断status如果是正常终止状态，则返回真。
	if (WIFEXITED(status))
		// 打印正常终止状态，WEXITSTATUS(status)获取子进程传送给exit或_exit参数的低8位。
		printf("normal termination, exit status = %d\n",
			WEXITSTATUS(status));
	// WIFSIGNALED宏判断status如果是异常终止状态，则返回真(接到一个不捕捉的信号)。
	else if (WIFSIGNALED(status))
		// 打印异常终止状态，WTERMSIG(status)获取使子进程终止的信号编号。
		printf("abnormal termination, signal number = %d%s\n",
			WTERMSIG(status),
#ifdef	WCOREDUMP
			// 有些实现会定义WCOREDUMP(status)，若已产生终止进程的core文件，则返回真。
			WCOREDUMP(status) ? " (core file generated)" : "");
#else
			"");
#endif
	// WIFSTOPPED宏判断status如果是当前暂停子进程的返回状态，则为真。
	else if (WIFSTOPPED(status))
		// 打印子进程暂停状态，WSTOPSIG(status)获取使子进程暂停的信号编号。
		printf("child stopped, signal number = %d\n",
			WSTOPSIG(status));
	// 下面代码是我加的，时间：2022-8-18。
	// WIFCONTINUED宏判断status如果是作业控制暂停后已经继续的子进程返回的状态，则为真。
	else if (WIFCONTINUED(status))
		// 打印作业控制暂停的子进程又继续了。
		printf("child continued.\n");
}

int
main(void)
{
	// 进程ID
	pid_t		pid;
	// 子进程退出状态
	int			status;

	// 父进程fork子进程
	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0)			/* child */
		// 指定子进程退出状态为7
		exit(7);
	// 父进程调用wait()获取子进程退出状态。
	if (wait(&status) != pid)	/* wait for child */
		err_sys("wait error");
	// 父进程将获取到的子进程退出状态打印出来。
	pr_exit(status);			/* and print its status */

	// 父进程fork子进程
	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0)			/* child */
		// 让子进程以abort()方式（异常终止）退出
		abort();				/* generates SIGABRT */
	// 父进程调用wait()获取子进程退出状态。
	if (wait(&status) != pid)	/* wait for child */
		err_sys("wait error");
	// 父进程将获取到的子进程退出状态打印出来。
	pr_exit(status);			/* and print its status */

	// 父进程fork子进程
	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0)			/* child */
		// 让子进程计算除以0，必然产生非正常状态终止。
		status /= 0;			/* divide by 0 generates SIGFPE */
	// 父进程调用wait()获取子进程退出状态。
	if (wait(&status) != pid)	/* wait for child */
		err_sys("wait error");
	// 父进程将获取到的子进程退出状态打印出来。
	pr_exit(status);			/* and print its status */

	// 父进程正常退出。
	exit(0);
}

// 点评
// 父进程调用wait()阻塞等待获取子进程退出状态，一个进程有几个子进程，只要有一个子进程终止，
// wait就返回。
// wait()返回值为子进程的pid，输出参数status为子进程的退出状态。
// 本例使用WIFXXXX系列宏判断子进程退出状态，以及获取子进程的退出状态码，获取子进程被哪些信号
// 终止的信号编码等。

// 实验结果
// normal termination, exit status = 7
// abnormal termination, signal number = 6
// abnormal termination, signal number = 8

// 结果分析
// 1.子进程退出状态码被获取到。
// 2.子进程异常退出时，它被哪个信号终止的信号编码也被获取到。
// 信号编码：
//     6：SIGABRT
//     8：SIGFPE
