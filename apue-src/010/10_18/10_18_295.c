#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

// 符合POSIX.1要求的一种system函数实现。
int
system(const char *cmdstring)		/* with appropriate signal handling */
{
	// 进程ID
	pid_t				pid;
	// system返回值
	int					status;
	// ignore将信号动作修改为忽略，
	// saveintr保存SIGINT信号动作被修改之前的动作，
	// savequit保存SIGQUIT信号动作被修改之前的动作。
	struct sigaction	ignore, saveintr, savequit;
	// chldmask屏蔽SIGCHLD信号用的信号集，
	// savemask保存进程原来的屏蔽字。
	sigset_t			chldmask, savemask;

	// 如果system参数指针为空，说明没有任何意义，直接返回1。
	if (cmdstring == NULL)
		return (1);			/* always a command processor with UNIX */

	// 设置对SIGINT和SIGQUIT这两个信号的处理动作为忽略。
	ignore.sa_handler = SIG_IGN;		/* ignore SIGINT and SIGQUIT */
	sigemptyset(&ignore.sa_mask);
	ignore.sa_flags = 0;
	// 使设置生效，并保存SIGINT信号之前的处理动作。
	if (sigaction(SIGINT, &ignore, &saveintr) < 0)
		return (-1);
	// 使设置生效，并保存SIGQUIT信号之前的处理动作。
	if (sigaction(SIGQUIT, &ignore, &savequit) < 0)
		return (-1);
	// system执行期间，阻塞SIGCHLD信号，保存进程原来的屏蔽字。
	sigempryset(&chldmask);		/* now block SIGCHLD */
	sigaddset(&chldmask, SIGCHLD);
	if (sigprocmask(SIG_BLOCK, &chldmask, &savemask) < 0)
		return (-1);

	// 父进程fork子进程
	if ((pid = fork()) < 0) {
		status = -1;		/* probably out of processes */
	} else if (pid == 0) {		/* child */
		/* restore previous signal actions & reset signal mask */
		// 子进程不再忽略SIGINT和SIGQUIT这两个信号，而是恢复到它们之前处理动作。
		sigaction(SIGINT, &saveintr, NULL);
		sigaction(SIGQUIT, &savequit, NULL);
		// 子进程不阻塞SIGCHLD信号，而是恢复到原来的进程屏蔽字。
		sigprocmask(SIG_SETMASK, &savemask, NULL);

		// 子进程执行exec系列函数执行命令或程序。
		// 这里执行/bin/sh -c <cmdstring>，也就是让sh再fork子进程，然后子进程调用
		// exec函数执行<cmdstring>。
		execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
		// execl成功后不会返回，如果返回说明出错，调用_exit()终止sh进程，终止码127。
		// 不冲洗标准IO流，因为不能影响system调用者。
		_exit(127);			/* exec error */
	} else {			/* parent */
		// sh进程调用waitpid等待cmd子进程终止，然后获取其状态为其善后。
		while (waitpid(pid, &status, 0) < 0)
			// waitpid返回值小于0说明出错了，如果出错原因不是EINTR，
			// 则system返回值为-1，跳出循环，否则继续waitpid等待。
			if (errno != EINTR) {
				status = -1;		/* error other than EINTR from waitpid() */
				break;
			}
	}

	/* restore previous signal actions & reset signal mask */
	// 恢复对SIGINT和SIGQUIT这两个信号的处理动作，使它们不再被忽略。
	if (sigaction(SIGINT, &saveintr, NULL) < 0)
		return (-1);
	if (sigaction(SIGQUIT, &savequit, NULL) < 0)
		return (-1);
	// 恢复进程原来的屏蔽字，也就是解除对信号SIGCHLD的阻塞。
	if (sigprocmask(SIG_SETMASK, &savemask, NULL) < 0)
		return (-1);

	// 返回最终system状态码。
	return (status);
}

// 点评
// POSIX.1要求system忽略信号SIGINT和SIGQUIT，在执行期间阻塞SIGCHLD，执行结束返回
// 后解除对SIGCHLD信号的阻塞。
// 本例给出符合该要求的一个system函数实现，它fork出子进程sh之前，将SIGINT和SIGQUIT
// 两种信号的处理动作设置为忽略，fork成功后让子进程处理命令，并且SIGINT和SIGQUIT仅仅
// 发送给cmd子进程，然后父进程sh调用waitpid等待cmd子进程终止后收集其退出信息，最后在
// 返回前将SIGINT和SIGQUIT的处理动作恢复为默认（不再忽略），并解除阻塞SIGCHLD信号。
