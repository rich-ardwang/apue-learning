#include <signal.h>
#include <unistd.h>

static void
sig_alrm(int signo)
{
	// 空函数，会立即返回，进而唤醒pause()。
	/* nothing to do, just return to wake up the pause */
}

// 这是sleep函数的一种最简单实现
unsigned int
sleep1(unsigned int seconds)
{
	// 注册需要捕获的信号SIGALRM并绑定其自定义处理函数
	if (signal(SIGALRM, sig_alrm) == SIG_ERR)
		// 如果失败就直接返回参数中提供的秒数
		return (seconds);
	// 设置SIGALRM信号多少秒后被触发
	alarm(seconds);			/* start the timer */
	// 进程休眠，等待SIGALRM信号被捕获才能返回。
	pause();				/* next caught signal wakes us up */
	// 关闭定时器，返回还没有用完的休眠时间。
	return(alarm(0));		/* turn off timer, return unslept time */
}

// 点评
// sleep1函数的缺点是：
// 1.alarm和pause之间存在race condition，如果alarm超时执行sig_alrm后，pause还未被执行，
//     之后再执行pause程序被永远挂起。
// 2.如果调用sleep1函数之前设置了alarm，那么之前的alarm将被sleep1内部的alarm覆盖。
