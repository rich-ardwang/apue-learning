#include "apue.h"

Sigfunc *
my_signal_intr(int signo, Sigfunc *func)
{
	struct sigaction		act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	// 将所有信号中断的系统调用设置为不自动重启动。
#ifdef	SA_INTERRUPT
	ct.sa_flags |= SA_INTERRUPT;
#endif
	if (sigaction(signo, &act, &oact) < 0)
		return (SIG_ERR);
	return (oact.sa_handler);
}
