#include <signal.h>
#include <unistd.h>

static void
sig_alrm(int signo)
{
	/* nothing to do, just return to wake up the pause */
}

unsigned int
sleep1(unsigned int seconds)
{
	if (signal(SIGALRM, sig_alrm) == SIG_ERR)
		return (seconds);
	alarm(seconds);			/* start the timer */
	pause();				/* next caught signal wakes us up */
	return(alarm(0));		/* turn off timer, return unslept time */
}

// sleep1函数的缺点是：
// 1.alarm和pause之间存在race condition，如果alarm超时执行sig_alrm后，pause还未被执行，
//     之后再执行pause程序被永远挂起。
// 2.如果调用sleep1函数之前设置了alarm，那么之前的alarm将被sleep1内部的alarm覆盖。
