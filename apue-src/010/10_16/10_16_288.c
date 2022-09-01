#include "apue.h"

// 关于sig_atomic_t类型的介绍参考10_15_283.c。
// 这里把它用作退出标志符。
volatile sig_atomic_t	quitflag;		/* set nonzero by signal handler */

// SIGINT和SIGQUIT信号的自定义处理函数
static void
sig_int(int signo)		/* one signal handler for SIGINT and SIGQUIT */
{
	// 如果是SIGINT信号
	if (signo == SIGINT) {
		// 通知进程被中断
		printf("\ninterrupt\n");
		// Add by Richard on 2021-12-13
		// 再次注册需要捕获SIGINT，并绑定自定义处理函数，这样下次再捕获到
		// 此信号时，还是回调该自定义信号处理函数。
		if (signal(SIGINT, sig_int) == SIG_ERR)
			err_sys("signal(SIGINT) error");
	}
	// 如果是SIGQUIT信号
	else if (signo == SIGQUIT)
		// 把退出标志符置为有效
		quitflag = 1;	/* set flag for main loop */
}

int
main(void)
{
	// newmask是需要并入进程原来屏蔽字的信号集，
	// oldmask是进程原来屏蔽字，
	// zeromask仅仅是一个空信号集。
	sigset_t	newmask, oldmask, zeromask;

	// 注册需要捕获的信号SIGINT和SIGQUIT，绑定它们共用的自定义信号处理函数。
	if (signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("signal(SIGINT) error");
	if (signal(SIGQUIT, sig_int) == SIG_ERR)
		err_sys("signal(SIGQUIT) error");

	// 初始化空信号集
	sigemptyset(&zeromask);
	sigemptyset(&newmask);
	// 将SIGQUIT信号加入信号集newmask
	sigaddset(&newmask, SIGQUIT);

	/*
	* Block SIGQUIT and save current signal mask.
	 */
	// 阻塞信号SIGQUIT，并保存进程原来的屏蔽字。
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
		err_sys("SIG_BLOCK error");

	// 打印出退出标志符
	printf("quitflag:%d\n",quitflag);
	// 如果退出标志符为0（不退出进程），则不阻塞任何信号。
	while (quitflag == 0)
		// 注意：上面被阻塞的SIGQUIT信号，在sigsuspend被调用后会被解
		// 除阻塞。
		// 不阻塞任何信号，放行所有信号，阻塞等待有信号发生并被处理完毕，
		// sigsuspend才会返回，并恢复进程屏蔽字至调用它之前的状态。
		sigsuspend(&zeromask);

	/*
	 * SIGQUIT has been caught and is now blocked; do whatever.
	 */
	// 既然能够跳出上面的while循环，说明quitflag的值是1，将其重新置0。
	quitflag = 0;

	/*
	 * Reset signal mask which unblocks SIQQUIT.
	 */
	// 恢复进程原来的屏蔽字olamask。
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		err_sys("SIG_SETMASK error");

	// 正常退出，冲洗标准IO流。
	exit(0);
}

// 点评
// sigsuspend除了可以保护临界区内代码不被某些信号中断外，另一种应用是等待一个信号
// 处理函数设置一个全局变量。
// 本程序捕捉中断信号和退出信号，但是希望仅当捕捉到退出信号时，才让进程终止。

// 实验
// 结果如下：
/*
quitflag:0
^C
interrupt
^C
interrupt
^C
interrupt
^C
interrupt
*/
// 分析：
// 程序运行后，我们多次输入Ctrl+C（产生中断信号），进程仅仅通知我们它收到了该信号，
// 而只有我们键入Ctrl+\（产生退出信号），信号处理程序会将quitflag设置为有效，进而
// 让进程终止。

// 与原书不一致
// 我们在自定义信号处理函数增加了如下代码：
/*
if (signal(SIGINT, sig_int) == SIG_ERR)
	err_sys("signal(SIGINT) error");
*/
// 这样，才会每次键入Ctrl+C，进程都会通知收到中断信号，而不会终止程序，否则当我们第
// 二次键入Ctrl+C时，进程对中断信号使用默认处理方式，这样程序就会被终止。
// 这一点与书中是不一致的，原书中没有对SIGINT信号再次注册并绑定，依然可以多次键入
// Ctrl+C，而程序不终止，这是我们疑惑的地方，难道其他Unix平台对中断信号的默认处理方式
// 是不终止进程？
