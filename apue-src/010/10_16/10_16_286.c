#include "apue.h"

// 信号SIGINT的自定义处理函数
static void		sig_int(int);

int
main(void)
{
	// newmask是需要并入进程原来的屏蔽字的信号集
	// oldmask是进程原来的信号屏蔽字
	// waitmask是需要被内核阻塞的信号集
	sigset_t	newmask, oldmask, waitmask;

	// 打印当前进程信号屏蔽字中有哪些信号
	// 应该没有信号被阻塞（屏蔽）才对
	pr_mask("program start: ");

	// 注册需要捕获的信号，绑定其自定义信号处理函数。
	if (signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("signal(SIGINT) error");
	// 初始化空信号集waitmask
	sigemptyset(&waitmask);
	// 在信号集waitmask中加入SIGUSR1信号
	sigaddset(&waitmask, SIGUSR1);
	// 初始化空信号集newmask
	sigemptyset(&newmask);
	// 在信号集newmask中加入SIGINT信号
	sigaddset(&newmask, SIGINT);

	/*
	 * Block SIGINT and save current signal mask.
	 */
	// 将进程当前信号屏蔽字与newmask取并集，即阻塞SIGINT信号，
	// 并保存旧的信号屏蔽字oldmask。
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
		err_sys("SIG_BLOCK error");

	/*
	 * Critical region of code.
	 */
	// 这部分属于临界区代码段，在这部分编写的代码是不会被SIGINT信号中断的，因为上面
	// 的sigprocmask已经阻塞了这个信号。

	// 打印当前进程信号屏蔽字中有哪些信号
	// 当前进程屏蔽字中应该包含SIGINT信号
	pr_mask("in critical region: ");

	/*
	 * Pause, allowing all signals except SIGUSR1.
	 */
	// 阻塞SIGUSR1信号，放行所有其他信号，然后进入阻塞状态等待除了SIGUSR1外的其他
	// 信号被触发，一旦有信号被触发，则信号处理函数执行完毕后，sigsuspend返回。
	// 也就是说sigsuspend阻塞了SIGUSR1信号，然后把所有其他信号放行，那么上面被阻塞
	// 的SIGINT信号也被放行了。
	// 如果用户按下Ctrl+C，SIGINT被触发，然后进入自定义信号处理函数sig_int，处理完
	// 后sig_int返回，然后sigsuspend返回。
	// sigsuspend返回后不再阻塞SIGUSR1信号，并恢复信号屏蔽字为调用sigsuspend之
	// 前的状态。
	if (sigsuspend(&waitmask) != -1)
		err_sys("sigsuspend error");

	// 打印当前进程信号屏蔽字中有哪些信号
	// 进程屏蔽字被恢复为调用sigsuspend之前的状态，所以应该包含SIGINT信号。
	pr_mask("after return from sigsuspend: ");

	// 在这里SIGINT信号又再次处于被阻塞的状态了。

	/*
	 * Reset signal mask which unblocks SIGINT.
	 */
	// 恢复最原始的进程信号屏蔽字，在这里SIGINT信号被解除阻塞了。
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		err_sys("SIG_SETMASK error");

	/*
	 * And continue processing ...
	 */
	// 最后再打印出进程屏蔽字中有哪些信号看一下
	// 此时信号屏蔽字应该和开始时一样，不阻塞任何信号。
	pr_mask("program exit: ");

	// 进程正常退出，冲洗标准IO流。
	exit(0);
}

static void
sig_int(int signo)
{
	// 打印当前进程信号屏蔽字中有哪些信号
	// 处理SIGINT信号期间，应该只有SIGUSR1信号被阻塞才对
	pr_mask("\nin sig_int: ");
}

// 点评
// 这个例子引入了sigsuspend函数，这样信号处理机制变得更加的复杂有趣。
// 正常来说我们要避免临界区一段代码被某个信号打断需要如下步骤：
// 1).调用sigprocmask阻塞该信号，如SIGINT。
// 2).在临界区内编写代码，这些代码不会被SIGINT信号中断。
// 3).调用sigprocmask恢复oldmask，解除对该信号的阻塞。
// 4).调用pause，进程休眠等待该信号如SIGINT被触发后pause返回，离开临界区。
// 但是，上述步骤在调用sigprocmask恢复oldmask和调用pause之间存在一个时间窗口，
// 如果在这个时间窗口期间，发生了SIGINT信号，那么pause可能永远被阻塞。
// 所以，需要一个可以使用原子操作的函数sigsuspend，它在阻塞一个信号的同时，让其它
// 信号顺利通过，然后让进程陷入休眠等待其它信号被触发并处理完毕，之后它解除对该信号
// 的阻塞，并恢复进程屏蔽字到sigsuspend被调用之前的状态。

// 实验
// 结果如下：
/*
program start: 
in critical region:  SIGINT
^C
in sig_int:  SIGUSR1
after return from sigsuspend:  SIGINT
program exit: 
*/
// 分析
// 可以看到实验结果的各个部分是我们所预期的，本例主要理解sigsuspend函数的用法。
