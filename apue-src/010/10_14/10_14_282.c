#include "apue.h"

Sigfunc *
my_signal_intr(int signo, Sigfunc *func)
{
	struct sigaction		act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	// 将系统调用设置为不自动重启动，它可以被所有类型的信号中断。
#ifdef	SA_INTERRUPT
	ct.sa_flags |= SA_INTERRUPT;
#endif
	if (sigaction(signo, &act, &oact) < 0)
		return (SIG_ERR);
	return (oact.sa_handler);
}

// 点评
// 这是使用sigaction()函数实现的另一个代替signal()函数的自定义实现，
// 这个实现与10_14_280的区别仅仅是将sa_flags打开SA_INTERRUPT标志
// 位，如果是古老的系统，则会定义SA_INTERRUPT，那么就打开SA_INTERRUPT
// 标志位，如果是现代Unix操作系统，几乎默认都是SA_INTERRUPT模式。
// 所以，几乎任何平台，任何类型的信号都可以中断系统调用，而系统调用被中
// 断后不会自动重启动。
