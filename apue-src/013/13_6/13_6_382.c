#include "apue.h"
#include <pthread.h>
#include <syslog.h>
#include "../13_5/13_5_381.c"
#include "../../014/14_3/14_3_398.c"

// 全局信号屏蔽字。
sigset_t mask;

// 引入外部定义的保证守护进程单例运行的函数。
extern int already_running(void);

// 模拟重新读取配置文件。
void
reread(void)
{
	/* ... */
	syslog(LOG_INFO, "Reload config file.\n");
}

// 线程函数。
void *
thr_fn(void *arg)
{
	int err, signo;

	for (;;) {
		// 阻塞等待信号发生。
		err = sigwait(&mask, &signo);
		if (err != 0) {
			syslog(LOG_ERR, "sigwait failed");
			exit(1);
		}
		switch (signo) {
		case SIGHUP:
			// 收到SIGHUP信号时，演示重新读取配置文件。
			syslog(LOG_INFO, "Re-reading configuration file");
			reread();
			break;
		case SIGTERM:
			// 收到SIGTERM信号时，正常结束进程。
			syslog(LOG_INFO, "got SIGTERM; exiting");
			exit(0);
		default:
			syslog(LOG_INFO, "unexpected signal %d\n", signo);
		}
	}
	return (0);
}

int
main(int argc, char *argv[])
{
	int					err;
	pthread_t			tid;
	char				*cmd;
	struct sigaction	sa;

	// 提取程序名称，如：提取./test_name为test_name。
	if ((cmd = strrchr(argv[0], '/')) == NULL)
		cmd = argv[0];
	else
		cmd++;

	/*
	 * Become a daemon.
	 */
	// 将程序名称作为实参，调用daemonize，把进程变为守护进程。
	// 或者说将某某进程守护进程化。
	daemonize(cmd);

	/*
	 * Make sure only one copy of the daemon is running.
	 */
	// 以单实例运行守护进程，即如果一个守护进程实例正在运行，则
	// 报错并结束进程。
	if (already_running()) {
		syslog(LOG_ERR, "daemon already running");
		exit(1);
	}

	/*
	 * Restore SIGHUP default and block all signals.
	 */
	// 改变信号处理方法为默认处理方式，因为daemonize中已将SIGHUP
	// 处理方式设置为忽略，这里必须更改，否则线程将永远也不能捕获到SIGHUP信号。
	sa.sa_handler = SIG_DFL;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0)
		err_quit("%s: can't restore SIGHUP default");
	// 主线程阻塞所有信号，让后面新创建的线程等待并处理SIGHUP
	// 和SIGTERM信号。
	sigfillset(&mask);
	if ((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0)
		err_exit(err, "SIG_BLOCK error");

	/*
	 * Create a thread to handle SIGHUP and SIGTERM.
	 */
	// 创建一个默认属性的线程。
	err = pthread_create(&tid, NULL, thr_fn, 0);
	if (err != 0)
		err_exit(err, "can't create thread");

	/*
	 * Proceed with the rest of the daemon.
	 */
	/* ... */
	// 为演示线程处理信号，让主线程睡眠600秒，之后再结束程序。
	syslog(LOG_INFO, "pid:%d\n", getpid());
	sleep(600);
	exit(0);
}

// 注意：
// 因为守护进程没有终端，所以需要使用系统日志接口来输出信息，
// 查看方式为：tail -f /var/log/syslog。
// 如果调用printf，则是无效，任何0, 1和2都被指向/dev/null。
