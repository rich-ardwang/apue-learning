#include "apue.h"

// SIGALRM信号的自定义信号处理函数
static void
sig_alrm(int signo)
{
	/* nothing to do, just returning wakes up sigsuspend() */
	// 什么都不需要做，只是让它返回即可唤醒sigsuspend。
}

// 符合POSIX.1的一种sleep函数实现。
unsigned int
sleep(unsigned int seconds)
{
	// newact修改后的信号处理动作，
	// oldact修改前的信号处理动作。
	struct sigaction		newact, oldact;
	// newmask需要屏蔽的信号集，
	// oldmask进程原来的信号屏蔽字，
	// suspmask需要被sigsuspend挂起的信号集，在其他信号被触发且被处理完毕后，
	// 才能解除对它们的阻塞。
	sigset_t				newmask, oldmask, suspmask;
	// 距离sleep超时未用完的秒数。
	unsigned int			unslept;

	/* set out handler, save prevoius information */
	// 设置SIGALRM信号新的处理动作，保存其原来的处理动作。
	newact.sa_handler = sig_alrm;
	sigtemptyset(&newact.sa_mask);
	newact.sa_flags = 0;
	sigaction(SIGALRM, &newact, &oldact);

	/* block SIGALRM and save current signal mask */
	// 阻塞（屏蔽）信号SIGALRM，保存原来的进程屏蔽字。
	// 防止在处理SIGALRM信号期间，有新的SIGALRM信号打断它。
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGALRM);
	sigprocmask(SIG_BLOCK, &newmask, &oldmask);

	// 设置SIGALRM信号的触发时间（秒数）
	alarm(seconds);
	// 将进程原来的屏蔽字设置为要挂起的信号集
	suspmask = oldmask;

	/* make sure SIGALRM isn't bolcked */
	// 从要挂起的信号集中移除信号SIGALRM，确保只有它被放行。
	sigdelset(&suspmask, SIGALRM);

	/* wait for any signal to be caught */
	// 阻塞进程当前屏蔽字中的所有信号，仅仅放行SIGALRM信号和其他未在进程屏蔽字
	// 中的信号，然后进程休眠，等待SIGALRM信号或其他信号被触发并处理完成，之后
	// sigsuspend返回，解除对进程屏蔽字中所有信号的阻塞，将进程屏蔽字恢复至调
	// 用sigsuspend之前的状态。
	sigsuspend(&suspmask);

	/* some signal has been caught, SIGALRM is now blocked */
	// 此时SIGALRM信号处于被阻塞状态。

	// 取得闹钟信号被触发前剩余的秒数，这就是sleep超时前未用完的秒数。
	unslept = alarm(0);

	/* reset previous action */
	// 恢复对SIGALRM信号的处理动作。
	sigaction(SIGALRM, &oldact, NULL);

	/* reset signal mask, which unblocks SIGALRM */
	// 恢复进程原来的信号屏蔽字。
	sigprocmask(SIG_SETMASK, &oldmask, NULL);
	// 返回未用完的秒数。
	return (unslept);
}

// 点评
// 这是POSIX.1 sleep函数的实现，它克服了与其他信号处理函数交互使用时出现的问题，
// 能够可靠的处理信号，从而避免在alarm和pause之间的race condition，对处理
// SIGALRM信号期间可能执行的其他信号处理函数没有任何影响。
// 但是，如果调用者在调用sleep之前，已经调用alarm()，那么这个alarm的秒数会被
// sleep函数内部调用的alarm()中的秒数所覆盖。
// POSIX.1没有明确对sleep与其它alarm交互使用时出现的问题做明确规定。

// 与sleep相关的其他函数
// nanosleep()，提供了纳秒级的精度，如果系统不支持纳秒精度，要求的时间就会取整，
// 该函数不涉及产生任何信号，无需担心与其他函数的交互。
// clock_nanosleep()，可以指定计算延迟时间所基于的时钟，针对该种时钟的延迟时间
// 来挂起调用线程。
