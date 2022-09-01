#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 符合POSIX.1说明的abort()函数的一种实现
void
abort(void)				/* POSIX-style abort() function */
{
	// 屏蔽信号集
	sigset_t			mask;
	// sigaction()函数使用的数据结构
	struct sigaction	action;

	/* Caller can't ignore SIGABRT, if no reset to default */
	// 获取当前对SIGABRT信号的处理方式。
	sigaction(SIGABRT, NULL, &action);
	// 如果当前对SIGABRT信号的处理方式是忽略，那么将处理方式修改为系统默认。
	if (action.sa_handler == SIG_IGN) {
		// 修改对SIGABRT信号的处理方式是系统默认。
		action.sa_handler = SIG_DFL;
		// 将修改后的action对象传入sigaction函数使修改生效。
		sigaction(SIGABRT, &action, NULL);
	}
	// 如果对SIGABRT信号的处理方式是系统默认，则冲洗所有已打开的标准IO流。
	if (action.sa_handler == SIG_DFL)
		// 冲洗所有已打开的标准IO流
		fflush(NULL);		/* flush all open stdio streams */

	/* Caller can't block SIGABRT; make sure it's unblocked */
	// 将所有种类的信号加入mask信号集
	sigfillset(&mask);
	// 从mask信号集中移除SIGABRT信号
	sigdelset(&mask, SIGABRT);		/* maske has only SIGABRT turned off */
	// 将除了SIGABRT信号以外的所有信号加入进程当前屏蔽字，即阻塞除了SIGABRT以外的
	// 所有信号。
	sigprocmask(SIG_SETMASK, &mask, NULL);
	// 向该进程发送SIGABRT信号，不管进程是否阻塞该信号，当kill返回时说明进程已经捕获
	// 并处理了该信号，但它没有自行终止。
	kill(getid(), SIGABRT);		/* send the signal */

	/* If we're here, process caught SIGABRT and returned */
	// 由于我们屏蔽了所有其他信号，所以上面的kill函数返回时说明进程一定收到并处理了
	// SIGABRT信号，这样进程可能又产生了许多输出，所以需要再次冲洗所有已打开的标准IO流。
	fflush(NULL);			/* flush all open stdio streams */
	// 修改对SIGABRT信号的处理动作是系统默认。
	action.sa_handler = SIG_DFL;
	// 将修改后的action对象传入sigaction函数使修改生效。
	sigaction(SIGABRT, &action, NULL);		/* reset to default */
	// 将除了SIGABRT信号以外的所有信号加入进程当前屏蔽字，即阻塞除了SIGABRT以外的
	// 所有信号。
	sigprocmask(SIG_SETMASK, &mask, NULL);	/* just in case ... */
	// 向该进程发送SIGABRT信号，当kill再次返回时，不管进程是否捕捉或处理，
	// 让abort()函数执行exit(1)让进程异常终止即可。
	kill(getid(), SIGABRT);					/* and one more time */
	// 终止该进程，并设置进程终止状态码为1。
	exit(1);		/* this should never ve executed ... */
}

// 点评
// POSIX.1规定不允许进程阻塞或者忽略SIGABRT信号，所以首先判断对该信号的当前处理动作，
// 如果动作是忽略，则改为系统默认。
// 如果当前动作是系统默认，则冲洗所有已经打开的标准IO流，但是不会关闭它们。
// 然后屏蔽所有信号，只放行SIGABRT信号，给进程发送这个信号，虽然进程不能阻塞或忽略它，
// 但是进程可以捕捉并处理它，这让进程可以选择自己执行一些清理动作或自行退出。
// 但是，当kill函数返回后，说明进程没有自行退出，为防止进程又产生很多输出，需要再次冲洗
// 所有已打开的标准IO流。
// 最后，将SIGABRT信号的处理动作设置为系统默认，并再次给进程发送一次该信号，这次不管进程
// 如何处理，只要kill函数返回，就让abort函数异常终止进程就可以了。
// 执行exit后，内核会把所有已经打开的标准IO流进行关闭（执行fclose()），如果不希望冲洗
// 标准IO流，可以调用_exit()或_Exit()等。
