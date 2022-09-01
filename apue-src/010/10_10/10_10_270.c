#include <setjmp.h>
#include <signal.h>
#include <unistd.h>

// 跨栈跳转机制专用的缓冲区
static jmp_buf		env_alrm;

// 闹钟信号处理函数
static void
sig_alrm(int signo)
{
	// 跨栈跳转回setjmp()函数设置的回溯点
	longjmp(env_alrm, 1);
}

// 这是sleep()函数的第二种实现
unsigned int
sleep2(unsigned int seconds)
{
	// 注册需要捕获的信号SIGALRM并绑定其自定义处理函数
	if (signal(SIGALRM, sig_alrm) == SIG_ERR)
		// 如果失败就直接返回参数中提供的秒数
		return (seconds);
	
	// setjmp()设置跨栈回溯点
	if (setjmp(env_alrm) == 0) {
		// 设置多少秒后触发SIGALRM信号
		alarm(seconds);			/* start the timer */
		// 进程休眠，等待SIGALRM信号被捕获并处理完毕后，pause返回。
		pause();				/* next caught signal wakes us up */
	}
	// 当longjmp()被调用后，将跨栈回溯到这里继续执行。
	// 关闭定时器，返回还没有用完的休眠时间。
	return (alarm(0));			/* turn off timer, return unslept time */
}

// 点评
// sleep2函数克服了alarm和pause之间的race condition，即使pause没有被执行，
//     当longjmp被调用后，它会跳过setjmp(env_alrm) == 0代码段，返回。
// 但是如果sleep2和其他信号处理函数一起工作时就会出现问题，longjmp可能导致其他
//     信号处理函数的栈被破坏，使得其他信号处理函数没有被完全执行。
