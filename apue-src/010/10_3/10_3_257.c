#include "apue.h"

// 自定义信号处理程序，当捕捉到SIGUSR1和SIGUSR2信号时，用它来处理。
static void sig_usr(int);	/* one handler for both signals */

// 添加者：Richard Wang
// 日期：2022-8-24
// 将原书中注册信号处理函数的部分封装了一下，使用更方便。
void reg_sig(int, __sighandler_t, char *);

int
main(void)
{
	// 修改者：Richard Wang
	// 日期：2022-8-24
	// 使用我们自己封装的函数，两种信号绑定同一个信号处理方法。
	reg_sig(SIGUSR1, sig_usr, "can't catch SIGUSR1");
	reg_sig(SIGUSR2, sig_usr, "can't catch SIGUSR2");
	for ( ; ; ) {
		// 添加者：Richard Wang
		// 日期：2022-8-24
		// 打印进程ID，通知该进程活着。
		// 注意：这里进程试图向终端写数据，所以不能./10_3_257 &使用后台作业方式，
		// 这样会收到SIGTTOU信号，导致进程终止。
		printf("i am still alive...pid:%ld\n", (long)getpid());
		// 让进程阻塞，等待接收SIGUSR1和SIGUSR2信号。
		// 收到信号后，pause()函数返回，进入下一次循环。
		pause();
	}
}

static void
sig_usr(int signo)		/* argument is signal number */
{
	// 捕捉到SIGUSR1信号
	if (signo == SIGUSR1) {
		// 通知此信号已收到
		printf("received SIGUSR1\n");
		// 再次捕捉该信号，绑定自定义处理函数。
		// 注意：注册捕捉信号并绑定信号处理方法后，如果进程捕捉到了该信号，
		// 并已经使用自定义方法处理过，则下次不再捕捉该信号，除非再次调用
		// signal()注册捕捉信号并绑定信号处理方法。
		reg_sig(SIGUSR1, sig_usr, "can't catch SIGUSR1");
	}
	// 捕捉到SIGUSR2信号
	else if (signo == SIGUSR2)
		// 通知此信号已收到
		printf("received SIGUSR2\n");
	else
		// 异常处理，收到其他类型信号。
		// 当收到其他类型的信号时，结束进程。
		err_dump("received signal %d\n", signo);
}

// 添加者：Richard Wang
// 日期：2022-8-24
void reg_sig(int signo, __sighandler_t handler, char *errmsg) {
	// 捕捉特定信号，绑定自定义的信号处理方法。
	if (signal(signo, handler) == SIG_ERR)
		err_sys(errmsg);
}

// 点评
// 本程序演示进程调用signal()注册需要捕捉的信号，并绑定自定义的信号处理方法。
// 可以对多个不同的信号进程捕捉，并设定同一个自定义的信号处理方法，当然也可以
// 对不同信号设定不同的自定义信号处理方法。
// 注册完毕后，当进程捕捉到这些信号时，自定义的信号处理方法将被回调。

// 注意
// signal()函数执行几次，捕捉到该信号时，自定义信号处理方法就会被回调几次。

// 实验
// 执行./10_3_257，进程打印出pid后阻塞了。
// 在另一个终端向上面的进程发送信号即可。
// kill -USR1 <pid>   #给进程发送SIGUSR1信号
// kill -USR2 <pid>   #给进程发送SIGUSR2信号
// 发送其他信号时，进程会终止，一直发送送SIGUSR1信号，进程会一直通知收到，
// 这是因为进程每次收到该信号后，都会重新对此信号进行注册。
// 而发送SIGUSR2后，再此发送此信号，进程会退出，这是因为第二次发送SIGUSR2
// 信号使用的是系统默认处理（进程退出）。
