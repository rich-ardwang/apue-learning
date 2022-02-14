#include "apue.h"
#include <syslog.h>
#include <errno.h>
#include "../13_5/13_5_381.c"
#include "../../014/14_3/14_3_398.c"

extern int lockfile(int);
extern int already_running(void);

void
reread(void)
{
	/* ... */
	syslog(LOG_INFO, "Reload config file.\n");
}

// SIGTERM信号处理函数。
void
sigterm(int signo)
{
	syslog(LOG_INFO, "got SIGTERM; exiting");
	exit(0);
}

// SIGHUP信号处理函数。
void
sighup(int signo)
{
	syslog(LOG_INFO, "Re-reading configuration file");
	reread();
}

// Add by Richard Wang at 2021-1-4.
// 可以持续处理信号。
struct sigaction	sa;
void reg_sig_term(struct sigaction *psa) {
	// 设置SIGTERM信号的处理方法为自定义函数sigterm。
	psa->sa_handler = sigterm;
	// 屏蔽SIGHUP，接收SIGTERM。
	sigemptyset(&psa->sa_mask);
	sigaddset(&psa->sa_mask, SIGHUP);
	psa->sa_flags = 0;
	if (sigaction(SIGTERM, psa, NULL) < 0) {
		syslog(LOG_ERR, "can't catch SIGTERM: %s", strerror(errno));
		exit(1);
	}
}
void reg_sig_hup(struct sigaction *psa) {
	// 设置SIGHUP信号的处理方法为自定义函数sighup。
	psa->sa_handler = sighup;
	// 屏蔽SIGTERM，接收SIGHUP。
	sigemptyset(&psa->sa_mask);
	sigaddset(&psa->sa_mask, SIGTERM);
	psa->sa_flags = 0;
	if (sigaction(SIGHUP, psa, NULL) < 0) {
		syslog(LOG_ERR, "can't catch SIGHUP: %s", strerror(errno));
		exit(1);
	}
}

int
main(int argc, char *argv[])
{
	char				*cmd;

	if ((cmd = strrchr(argv[0], '/')) == NULL)
		cmd = argv[0];
	else
		cmd++;

	/*
	 * Become a daemon.
	 */
	daemonize(cmd);

	/*
	 * Make sure only one copy of the daemon is running.
	 */
	if (already_running()) {
		syslog(LOG_ERR, "daemon already running");
		exit(1);
	}

	/*
	 * Handle signals of interest.
	 */
	// 进程直接接收并处理信号，而不是由创建的线程来处理。
	reg_sig_term(&sa);
	reg_sig_hup(&sa);

	/*
	 * Proceed with the rest of the daemon.
	 */
	/* ... */
	// 为演示进程处理信号，我们让其无线循环式阻塞，
	// 每当信号处理函数被执行，pause会解除阻塞，
	// 然后进入下一个循环后再次阻塞。
	// 我们通过发送SIGTERM信号让程序退出即可。
	syslog(LOG_INFO, "pid:%d\n", getpid());
	while (1) {
		pause();
	}
	
	// 这里永远不会被执行到。
	syslog(LOG_INFO, "exit process!");
	exit(0);
}
