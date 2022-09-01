#include "apue.h"

/* Reliable version of signal(), using POSIX sigaction(). */
// 利用sigaction函数重新实现了signal函数，取名为my_signal。
Sigfunc *
my_signal(int signo, Sigfunc *func)
{
	// act保存该信号需要修改的动作。
	// oact保存该信号处理动作被修改前的上一个动作。
	struct sigaction	act, oact;
	// 指定信号处理函数。
	act.sa_handler = func;
	// 取消一切信号屏蔽字。
	// 注意不可以这样做act.sa_mask = 0，只能使用sigemptyset()函数。
	sigemptyset(&act.sa_mask);
	// 初始化sa_flags
	act.sa_flags = 0;
	// 对除了被SIGALRM信号以外的所有信号所中断的系统调用，尝试使用自动重启动处理。
	// 我们希望被SIGALRM信号中断的系统调用不要自动重启的原因，是因为可以使用闹钟
	// 信号机制为那些慢速系统调用设置限制(超时)时间，使慢速系统调用不至于永远阻塞。
	if (signo == SIGALRM) {
		// 系统调用设置为不要自动重启动。
		// 早期SunOS定义了SA_INTERRUPT标志，这些系统默认是重新启动，设置为
		// SA_INTERRUPT后就不会自动重启动了。
		// Linux虽然也定义了SA_INTERRUPT标志，这是为了兼容老系统，它默认处理
		// 方式是不自动重启动，所以Linux系统设置SA_INTERRUPT与否无关紧要。
		// Single UNIX Specification的XSI扩展规定，sigaction()函数应该默认
		// 为不自动重启动，除非设置了SA_RESTART标志位。
#ifdef	SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;
#endif
	} else {
		// 系统调用设置为自动重启动
		act.sa_flags |= SA_RESTART;
	}
	// 按照act更新信号处理动作设置，取得该信号的上一个处理动作oact。
	if (sigaction(signo, &act, &oact) < 0)
		return (SIG_ERR);
	// 返回信号处理函数的函数指针。
	return (oact.sa_handler);
}

/* Add by Richard Wang
   2021-12-10
*/
// 信号的自定义处理函数
void sig_handle(int signo) {
	// 在信号处理函数中打印信号的编号
	printf("sig_handle, signo:%d.\n", signo);
	// 设置对SIGQUIT信号的处理方式为默认，即下次再收到此信号不再执行sig_handle，
	// 而是执行系统默认的处理函数。
	if (signal(SIGQUIT, SIG_DFL) == SIG_ERR)
		err_sys("can't reset SIGQUIT");
}

int main() {
	// 注册需要捕获的信号，绑定其自定义处理函数。
	if (my_signal(SIGQUIT, sig_handle) == SIG_ERR)
		err_sys("my_signal(SIGQUIT) error");
	for (;;) {
		// 通报保活信息
		printf("i am still alive...\n");
		// 进程休眠5秒后进入下一次循环
		sleep(5);
	}
}

// 点评
// 本例使用sigaction()函数实现了自己的signal()函数，也就是说如果不满足
// 系统提供的signal()函数，或者想要拓展新功能，或者为了兼容老系统的signal()
// 语义，我们可以直接调用sigaction()来自己实现signal()函数。
// sigcation()函数的功能是检查或修改（检查并修改）与指定信号相关联的处理
// 动作。
// 函数参数：
// 1.signo为信号编号
// 2.act指针非空，则要修改其动作。
// 3.oact指针非空，则系统经由oact指针返回该信号的上一个动作。
