#include "apue.h"

// 信号SIGQUIT的自定义处理函数
static void		sig_quit(int);

int
main(void)
{
	// newmask为需要并入当前进程信号屏蔽字的信号集。
	// oldmask为被并入新的信号集之前的当前进程的信号屏蔽字。
	// pendmask为当前进程所有未决信号集。
	sigset_t newmask, oldmask, pendmask;

	// 注册需要捕获的信号SIGQUIT，绑定其自定义处理函数。
	if (signal(SIGQUIT, sig_quit) == SIG_ERR)
		err_sys("can't catch SIGQUIT");

	/*
	 * Block SIGQUIT and save current signal mask.
	 */
	// 初始化一个空信号集
	sigemptyset(&newmask);
	// 将信号SIGQUIT加入这个空信号集
	sigaddset(&newmask, SIGQUIT);
	// 进程当前信号屏蔽字与newmask取并集，返回进程旧的信号屏蔽字oldmask。
	// 也就是这里想要让进程阻塞SIGQUIT信号。
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
		err_sys("SIG_BLOCK error");

	// 当前进程休眠5秒
	sleep(5);		/* SIGQUIT here will remain pending */
	// 5秒之前，所有SIGQUIT信号都被阻塞，不会发送到该进程。

	// 取得当前进程的所有未决信号集。
	if (sigpending(&pendmask) < 0)
		err_sys("sigpending error");
	// 检查SIGQUIT信号是否是未决的，即检查5秒前是否有SIGQUIT信号产生，
	// 但是没有发送给该进程。
	if (sigismember(&pendmask, SIGQUIT))
		printf("\nSIGQUIT pending\n");

	/*
	 * Restore signal mask which unblocks SIGQUIT.
	 */
	// 将保存的旧的进程屏蔽字恢复，即不再阻塞SIGQUIT信号。
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		err_sys("SIG_SETMASK error");
	// 通知不再阻塞SIGQUIT信号
	printf("SIGQUIT unblocked\n");

	// 进程再次休眠5秒
	sleep(5);		/* SIGQUIT here will terminate with core file */
	// 正常退出，冲洗标准IO流。
	exit(0);
}

static void
sig_quit(int signo)
{
	// 通知已捕获到SIGQUIT信号
	printf("caught SIGQUIT\n");
	// 设置下一次捕获SIGQUIT信号时的处理方式为系统默认方式。
	// SIGQUIT信号的系统默认处理方式就是终止进程。
	if (signal(SIGQUIT, SIG_DFL) == SIG_ERR)
		err_sys("can't reset SIGQUIT");
}

// 点评
// 本例主要演示sigpending()函数的使用方法，该函数可以取得当前进程的所有未决
// 信号的集合。

// 实验
// ./10_13_276，然后在5秒内Ctrl+\，在下一个休眠5秒期间，再次Ctrl+\。
// 结果1：
/*
^\
SIGQUIT pending
caught SIGQUIT
SIGQUIT unblocked
^\退出 (核心已转储)
*/
// 分析：
// 进程休眠的第一个5秒内，我们已经设置阻塞了信号SIGQUIT，因此Ctrl+\所产生的
// SIGQUIT信号不能被发送到进程。
// 5秒过后，可见SIGQUIT确实在当前进程的所有未决信号集中。
// 然后，我们解除对SIGQUIT信号的阻塞，再次让进程休眠5秒。
// 也就是在第二个5秒内，我们执行Ctrl+\发送SIGQUIT信号，进程收到此信号后采取
// 系统默认的处理方式，也就是core dump终止。

// 结果2：
/*
^\^\^\^\^\
SIGQUIT pending
caught SIGQUIT
SIGQUIT unblocked
^\退出 (核心已转储)
*/
// 分析：
// 这一次在进程睡眠的第一个5秒期间，我们产生了5个SIGQUIT未决信号，5秒过后我们
// 解除了对该类型信号的阻塞，但是仅有一个SIGQUIT被捕获了，这说明系统并没有让未决
// 信号排队，而是同一种未决信号，解除阻塞后，仅发送一次给进程。

// 解除信号阻塞的另一种方法
// 可以使用SIG_UNBLOCK解除对某种信号（如SIGQUIT）的阻塞，但是不建议使用这种
// 方法。
// 如果我们编写的函数给他人调用，调用者在调用我们函数前就阻塞了同样的信号，显然
// 在我们函数内部简单粗暴的使用SIG_UNBLOCK解除对这种信号的阻塞是不合适的，还是
// 应该将当前进程的信号屏蔽字恢复为已保存的旧屏蔽字比较稳妥，这可以使该信号被继续
// 阻塞住，不会影响调用者的代码。
