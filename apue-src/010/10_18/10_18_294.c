#include "apue.h"

static void
sig_int(int signo)
{
	printf("caught SIGINT\n");
}

static void
sig_chld(int signo)
{
	printf("caught SIGCHLD\n");
}

int
main(void)
{
	if (signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("signal(SIGINT) error");
	if (signal(SIGCHLD, sig_chld) == SIG_ERR)
		err_sys("signal(SIGCHLD) error");
	int sr = system("/bin/ed");
	if (sr < 0)
		err_sys("system() error");
	printf("system return value: %d\n", sr);

	exit(0);
}

// 演示了system函数调用。
// system函数会fork shell子进程，然后shell子进程再fork一个子进程来中执行命令调用，
// 这里的命令调用为/bin/ed。
// ed是一个文本编辑器，它对SIGINT的处理方式是输出？号，对SIGQUIT的处理方式是忽略。

// system函数把SIGINT信号阻塞，只会将这个信号发送给命令子进程，不会发送给其父进程，
// 所以，shell子进程和./10_18_294进程都不会收到SIGINT，我们可以看到只有？号输出。
// 这样做的目的是，让shell子进程等待交互式命令的退出，然后收集其信息，设置system函数
// 的返回值，否则shell子进程退出了，就不能收集交互式命令的推出信息了，交互式命令将变成孤儿。

// 但是system函数不阻塞SIGQUIT信号，当发送SIGQUIT时，ed会忽略它，shell子进程会采取
// 退出处理，在shell子进程退出前，它首先标记system返回值为异常，然后发送SIGCHLD给
// ./10_18_294进程，后者产生了caught SIGCHLD输出，之后./10_18_294也采取退出处理，
// 最后ed成了孤儿进程，由类似init的系统进程收养。

// 或者在ed控制台输入q，这样ed会正常退出，然后shell子进程可以正常收集到ed的退出信息，
// 并标记system函数的返回值为0，然后发送SIGCHLD给./10_18_294，后面的过程就不赘述了。
