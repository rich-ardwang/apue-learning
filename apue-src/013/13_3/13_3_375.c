#include "apue.h"
#include <syslog.h>
#include <fcntl.h>
#include <sys/resource.h>

// 实现守护进程的函数。
// 注意：
// 虽然参数cmd指定守护进程的名字，但是实际上
// 守护进程的真正名字是编译时决定的，这个函数
// 并不能真正改变守护进程的名字。
// 如果cmd与编译时指定的程序名称不一致，则守护
// 进程名字以后者为准。
void
daemonize(const char *cmd)
{
	int					i, fd0, fd1, fd2;
	pid_t				pid;
	struct rlimit		rl;
	struct sigaction	sa;

	/*
	 * Clear file creation mask.
	 */
	// 文件掩码设置为0，这样由继承得来的文件模式，
	// 创建文件时其权限不会受到任何影响。
	umask(0);

	/*
	 * Get maximum number of file descriptors.
	 */
	// 取得最大的文件描述符。
	if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
		err_quit("%s: can't get file limit", cmd);

	/*
	 * Become a session leader to lose controlling TTY.
	 */
	// fork进程后父进程立即退出，这样做有两点作用：
	// 1.如果是从shell中以命令行方式执行的，可以让shell认为命令已经执行完毕。
	// 2.虽然子进程继承了父进程的进程组ID，但是它获得了新的进程ID，
	// 这样保证子进程不是一个进程组的组长，为setsid调用提供先决条件。
	if ((pid = fork()) < 0)
		err_quit("%s: can't fork", cmd);
	else if (pid != 0)		/* parent */
		exit(0);
	// setid调用后意味着做了三件事：
	// 1.创建一个新的会话，调用进程(子进程)成为新会话的首进程，该进程是新会话中的唯一进程。
	// 2.该进程成为一个新的进程组的组长进程，新进程组的ID就是该进程的进程ID。
	// 3.该进程没有控制终端，如果调用setsid前该进程有控制终端，那么这个控制终端会被切断。
	// 注意：必须保证setsid的调用进程不是一个进程组的组长，否则setsid出错返回。
	setsid();

	/*
	 * Ensure future opens won't allocate controlling TTYs.
	 */
	// 设置信号处理方法为忽略（SIG_IGN）。
	sa.sa_handler = SIG_IGN;
	// 清空信号屏蔽字。
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	// 设置对信号SIGHUP的处理方式为忽略。
	if (sigaction(SIGHUP, &sa, NULL) < 0)
		err_quit("%s: can't ignore SIGHUP", cmd);
	// 再次fork进程，之后父进程立即退出，这样子进程继承了父进程的进程组ID，
	// 因为父进程已经是会话的首进程，并且是进程组的组长，并且无终端，所以
	// 子进程不是进程组的组长，也不是会话的首进程，更没有终端。
	// 这个新的子进程成为了孤儿进程组的进程，即守护进程在一个孤儿进程组中。
	if ((pid = fork()) < 0)
		err_quit("%s: can't fork", cmd);
	else if (pid != 0)		/* parent */
		exit(0);

	/*
	 * Change the current working directory to the root so
	 * we won't prevent file systems from being unmounted.
	 */
	// 改变进程的当前工作目录为"/"。
	if (chdir("/") < 0)
		err_quit("%s: can't change directory to /", cmd);

	/*
	 * Close all open file descriptors.
	 */
	// 根据之前取得的最大文件描述符，关闭所有文件描述符。
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for (i = 0; i < rl.rlim_max; i++)
		close(i);

	/*
	 * Attach file descriptors 0, 1, and 2 to /dev/null.
	 */
	// 将文件描述符0, 1和2指向黑洞，这样所有试图从标准输入读，
	// 写入标准输出和标准错误的行为，都将无效。
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);

	/*
	 * Initialize the log file.
	 */
	// 初始化系统级别日志文件记录模块。
	// 参数1：进程名称，参数2：见APUE图13-3，参数3：见APUE图13-4。
	openlog(cmd, LOG_CONS, LOG_DAEMON);
	if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
		syslog(LOG_ERR, "unexpected file descriptors %d %d %d",
			fd0, fd1, fd2);
		exit(1);
	}
}

int
main(void)
{
	daemonize("test");
	sleep(60);
}
