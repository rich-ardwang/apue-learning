#include <setjmp.h>
#include <signal.h>
#include <unistd.h>

static jmp_buf		env_alrm;

static void
sig_alrm(int signo)
{
	longjmp(env_alrm, 1);
}

unsigned int
sleep2(unsigned int seconds)
{
	if (signal(SIGALRM, sig_alrm) == SIG_ERR)
		return (seconds);
	if (setjmp(env_alrm) == 0) {
		alarm(seconds);			/* start the timer */
		pause();				/* next caught signal wakes us up */
	}
	// 关闭定时器，返回还没有用完的休眠时间。
	return (alarm(0));			/* turn off timer, return unslept time */
}

// sleep2函数克服了alarm和pause之间的race condition，即使pause没有被执行，
//     当longjmp被调用后，它会跳过setjmp(env_alrm) == 0代码段，返回。
// 但是如果sleep2和其他信号处理函数一起工作时就会出现问题，longjmp可能导致其他
//     信号处理函数的栈被破坏，使得其他信号处理函数没有被完全执行。
