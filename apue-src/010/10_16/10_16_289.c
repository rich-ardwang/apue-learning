#include "apue.h"

// 关于sig_atomic_t类型的介绍参考10_15_283.c。
// 这里把它用作跳出sigsuspend循环的标志位。
static volatile sig_atomic_t		sigflag;	/* set nonzero by sig handler */
// newmask是需要并入进程原来屏蔽字的信号集，
// oldmask是进程原来屏蔽字，
// zeromask仅仅是一个空信号集。
static sigset_t						newmask, oldmask, zeromask;

// SIGUSR1和SIGUSR2信号共用的自定义信号处理函数。
static void
sig_usr(int signo)		/* one signal handler for SIGUSR1 and SIGUSR2 */
{
	// 不管是SIGUSR1或SIGUSR2哪个信号，只要进入该信号处理函数，
	// 就将sigsuspend循环跳出标志置为有效。
	sigflag = 1;
}

// 父子进程同步机制的初始化函数。
void
TELL_WAIT(void)
{
	// 注册需要捕获的信号SIGUSR1和SIGUSR2，绑定它们共用的自定义信号处理函数。
	if (signal(SIGUSR1, sig_usr) == SIG_ERR)
		err_sys("signal(SIGUSR1) error");
	if (signal(SIGUSR2, sig_usr) == SIG_ERR)
		err_sys("signal(SIGUSR2) error");
	// 初始化并清空信号集
	sigemptyset(&zeromask);
	sigemptyset(&newmask);
	// 在信号集newmask添加两个信号SIGUSR1和SIGUSR2。
	sigaddset(&newmask, SIGUSR1);
	sigaddset(&newmask, SIGUSR2);

	/* Block SIGUSR1 and SIGUSR2, and save current signal mask */
	// 设置阻塞SIGUSR1和SIGUSR2信号，并保存原来的进程屏蔽字到oldmask信号集。
	if ( sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
		err_sys("SIG_BLOCK error");
}

// 子进程通知父进程自己的任务已完成，父进程可以被唤醒继续执行。
void
TELL_PARENT(pid_t pid)
{
	// 给父进程发送SIGUSR2信号
	kill(pid, SIGUSR2);		/* tell parent we're done */
}

// 子进程调用此函数进入休眠状态，让父进程先执行任务，等父进程完成任务后调用
// TELL_CHILD()，该函数才会唤醒子进程，让其继续执行。
void
WAIT_PARENT(void)
{
	while (sigflag == 0)
		// 挂起子进程，并允许所有信号有效，sigsuspend函数返回后恢复到之前
		// 的进程信号屏蔽字状态(即继续阻塞SIGUSR1和SIGUSR2信号)。
		// 只有SIGUSR1或SIGUSR2有一个被触发并处理，sigsuspend才会返回，
		// 而这意味着sigflag已经被置为1，跳出循环，即该进程被唤醒继续执行。
		sigsuspend(&zeromask);	/* and wait for parent */
	// 子进程重置标志位
	sigflag = 0;

	/* Reset signal mask to original value */
	// 子进程恢复到进程最初的信号屏蔽字状态
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		err_sys("SIG_SETMASK error");
}

// 父进程通知子进程自己的任务已完成，子进程可以被唤醒继续执行。
void
TELL_CHILD(pid_t pid)
{
	// 给父进程发送SIGUSR1信号
	kill(pid, SIGUSR1);		/* tell child we're done */
}

// 父进程调用此函数进入休眠状态，让子进程先执行任务，等子进程完成任务后调用
// TELL_PARENT()，该函数才会唤醒父进程，让其继续执行。
void
WAIT_CHILD(void)
{
	while (sigflag == 0)
		// 挂起父进程，并允许所有信号有效，sigsuspend函数返回后恢复到之前
		// 的进程信号屏蔽字状态(即继续阻塞SIGUSR1和SIGUSR2信号)。
		// 只有SIGUSR1或SIGUSR2有一个被触发并处理，sigsuspend才会返回，
		// 而这意味着sigflag已经被置为1，跳出循环，即该进程被唤醒继续执行。
		sigsuspend(&zeromask);	/* and wait for child */
	// 父进程重置标志位
	sigflag = 0;

	/* Reset signal mask to original value */
	// 父进程恢复到进程最初的信号屏蔽字状态
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		err_sys("SIG_SETMASK error");
}

// 点评
// 本例使用目前为止，我们所接触过的所有和信号概念相关的重要函数，实现了父子进程
// 的同步机制。
// 父子进程同步机制的用法参考8_9_198.c。
// 有两种使用方式都可以完成父子进程同步：
// 方式一：
// 主进程调用TELL_WAIT()初始化父子进程同步机制。
// 子进程调用WAIT_PARENT()自己阻塞，让父进程先执行任务。
// 父进程执行任务完毕后调用TELL_CHILD()，通知唤醒子进程，让子进程继续执行。
// 方式二：
// 主进程调用TELL_WAIT()初始化父子进程同步机制。
// 父进程调用WAIT_CHILD()自己阻塞，让子进程先执行任务。
// 子进程执行任务完毕后调用TELL_PARENT()，通知唤醒父进程，让父进程继续执行。
