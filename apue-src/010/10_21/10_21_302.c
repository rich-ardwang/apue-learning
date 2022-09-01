#include "apue.h"

// 指定系统调用读写的缓冲区长度，1024为效率最佳。
#define BUFFSIZE	1024

// 信号SIGTSTP的自定义信号处理函数
static void
sig_tstp(int signo)		/* signal handler for SIGTSTP */
{
	// 通知收到信号SIGTSTP
	printf("Reveived SIGTSTP.\n");

	// 信号屏蔽字（信号集）
	sigset_t		mask;

	/* ... move cursor to lower left corner, reset tty mode ... */
	// 移动光标至左下角，重新设置tty模式，将终端恢复至打开vi之前的状态。

	/*
	 * Unblock SIGTSTP, since it's blocked while we're handling it.
	 */
	// 清空信号集mask
	sigemptyset(&mask);
	// 将信号SIGTSTP加入信号屏蔽字，此时该屏蔽字中只有这一个信号。
	sigaddset(&mask, SIGTSTP);
	// 将mask信号集的补集与进程当前信号屏蔽字取交集，即解除对mask中所含信号的阻塞。
	// 这里解除对SIGTSTP阻塞的原因是，在处理该信号期间，系统自动阻塞该信号，而我们
	// 的目的是，在处理该信号期间，进程也能再次收到该信号。
	sigprocmask(SIG_UNBLOCK, &mask, NULL);

	// 设置信号SIGTSTP为默认处理方式，这个信号的默认处理方式就是停止进程。
	signal(SIGTSTP, SIG_DFL);		/* reset disposition to default */

	// 告知进程处于给自己发送SIGTSTP信号之前的状态。
	printf("Before send SIGTSTP to myself.\n");

	// 进程调用kill发送SIGTSTP给自己，这次直接使用SIGTSTP信号的默认处理动作来停止
	// 进程，因此kill函数不会返回，它后面的代码暂时不会得到执行。
	// 直到作业控制shell使用bg % <jobid>来恢复进程，这相当于给进程发送了SIGCONT
	// 信号来继续一个停止的进程。
	// 或者可以使用kill -CONT % <jobid>直接发送SIGCONT信号来恢复已停止的进程。
	// 然后，kill函数才返回，进程继续执行后面的代码。
	kill(getpid(), SIGTSTP);		/* and send the signal to ourself */

	// 告知进程处于给自己发送SIGTSTP信号之后的状态。
	printf("After send SIGTSTP to myself.\n");

	/* we don't return from the kill until we're continued */

	// 重新注册需要捕获的信号，绑定该信号处理函数。
	signal(SIGTSTP, sig_tstp);		/* reestablish signal handler */

	/* ... reset tty mode, redraw screen ... */
	// 重置tty模式，重新绘制屏幕等操作。
}

int
main(void)
{
	// 系统调用读一次返回的字节数
	int			n;
	// 系统调用读写函数使用的缓冲区
	char		buf[BUFFSIZE];

	/*
	 * Only catch SIGTSTP if we're running with a job-control shell.
	 */
	// 将SIGTSTP信号的处理动作修改为SIGIGN（忽略），并判断其返回值。
	// 在支持作业控制的shell中（如/bin/bash），它会返回SIG_DFL（默认），此时
	// 我们可以设置捕捉该信号，并绑定该信号的自定义处理函数。
	// 如果在不支持作业控制的shell中（如/bin/sh），signal会返回SIG_IGN，此时
	// 捕捉该信号没有意义。
	if (signal(SIGTSTP, SIG_IGN) == SIG_DFL)
		// 注册需要捕获的信号，绑定其自定义信号处理函数。
		signal(SIGTSTP, sig_tstp);

	// 系统调用read函数从标准输入读，每次读取n个字节。
	while ((n= read(STDIN_FILENO, buf, BUFFSIZE)) > 0)
		// 系统调用write函数将read读取的n个字节写入到标准输出。
		if (write(STDOUT_FILENO, buf, n) != n)
			err_sys("write error");

	// while循环结束后，判断read是否出错。
	if (n < 0)
		err_sys("read error");

	// 正常退出进程，冲洗已经打开的所有标准IO流。
	exit(0);
}

// 点评
// 这个例子目的是让我们熟悉shell作业控制中需要使用的几种信号，它们的用法与原理，
// 它重点阐述了SIGTSTP信号，举了一个类似vi编辑器的例子，当用户要挂起它时，它需
// 要移动光标至左下角，重新设置tty模式，以便将终端状态恢复到vi启动时的情况，而
// 当在前台恢复它时，它要重置tty模式，重新绘制屏幕等，以便将终端状态恢复回vi编
// 辑时的情况。

// POSIX.1认为有以下6种信号与作业控制有关：
// 1).SIGCHLD，子进程已停止或终止，将该信号发送给其父进程。
// 一般非交互式shell是不处理下面的信号的，只有交互式shell会处理2-6所有信号。
// 2).SIGCONT，默认处理动作是，如果进程已停止，则使其继续运行，即使该信号被阻塞
// 或忽略，也要使其继续运行，否则忽略。当我们通知shell在前台或后台恢复运行一个作
// 业时，shell向该作业中的所有进程发送SIGCONT信号。
// 3).SIGSTOP，这是一个作业控制信号，它停止一个进程，类似于SIGTSTP，但是这个
// 信号不能被捕捉或忽略。
// 4).SIGTSTP，交互式停止信号，键盘键入Ctrl+Z时，终端驱动程序产生此信号，发送
// 至前台进程组中所有进程。
// 5).SIGTTIN，后台进程组成员试图读控制终端时产生该信号，默认处理动作是停止进程。
// 6).SIGTTOU，后台进程组成员试图写控制终端时产生该信号，默认处理动作是停止进程。
// [注意]
// shell并不显式忽略SIGTSTP信号，而是由init将4-6这3个作业控制信号设置为SIG_IGN，
// 然后这种配置被所有登录shell继承，只有作业控制shell才应将这3个信号重新设置为SIG_DFL。

// 作业控制信号间交互
// 当对一个进程产生4种停止信号（SIGTSTP、SIGSTOP、SIGTTIN或SIGTTOU）中的任意
// 一种时，对该进程的任一未决SIGCONT信号就被丢弃。
// 反之，当对一个进程产生SIGCONT信号时，对同一进程的任一未决停止信号被丢弃。

// 不能被捕捉或忽略的两种信号
// SIGKILL和SIGSTOP

// 常见的键盘输出终端驱动所产生的信号
// Ctrl+C，产生SIGINT信号。
// Ctrl+\，产生SIGQUIT信号。
// Ctrl+Z，产生SIGTSTP信号。
// Ctrl+D，不产生信号，当前行有输入时，shell删除光标前所有的输入字符，当前行
// 没有输入字符时，shell把它当作EOF，做结束并退出进程处理。
