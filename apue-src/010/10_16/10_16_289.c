#include "apue.h"

static volatile sig_atomic_t		sigflag;	/* set nonzero by sig handler */
static sigset_t						newmask, oldmask, zeromask;

static void
sig_usr(int signo)		/* one signal handler for SIGUSR1 and SIGUSR2 */
{
	sigflag = 1;
}

void
TELL_WAIT(void)
{
	// 注册信号处理函数
	if (signal(SIGUSR1, sig_usr) == SIG_ERR)
		err_sys("signal(SIGUSR1) error");
	if (signal(SIGUSR2, sig_usr) == SIG_ERR)
		err_sys("signal(SIGUSR2) error");
	// 初始化并清空信号集
	sigemptyset(&zeromask);
	sigemptyset(&newmask);
	// 在此信号集添加两个信号
	sigaddset(&newmask, SIGUSR1);
	sigaddset(&newmask, SIGUSR2);

	/* Block SIGUSR1 and SIGUSR2, and save current signal mask */
	// 设置阻塞SIGUSR1和SIGUSR2信号，并保存之前的进程屏蔽字到oldmask信号集
	if ( sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
		err_sys("SIG_BLOCK error");
}

void
TELL_PARENT(pid_t pid)
{
	// 给父进程发送SIGUSR2信号
	kill(pid, SIGUSR2);		/* tell parent we're done */
}

void
WAIT_PARENT(void)
{
	while (sigflag == 0)
		// 挂起子进程，并允许所有信号有效，sigsuspend函数返回后恢复到之前的进程信号屏蔽字状态(即继续阻塞SIGUSR1和SIGUSR2信号)
		sigsuspend(&zeromask);	/* and wait for parent */
	// 重置标志位
	sigflag = 0;

	/* Reset signal mask to original value */
	// 恢复到进程最初的信号屏蔽字状态
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		err_sys("SIG_SETMASK error");
}

// 实现方式与上面一致
void
TELL_CHILD(pid_t pid)
{
	kill(pid, SIGUSR1);		/* tell child we're done */
}

void
WAIT_CHILD(void)
{
	while (sigflag == 0)
		sigsuspend(&zeromask);	/* and wait for child */
	sigflag = 0;

	/* Reset signal mask to original value */
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		err_sys("SIG_SETMASK error");
}
