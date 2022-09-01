#include "apue.h"
#include <setjmp.h>

// SIGALRM信号的自定义处理函数
static void			sig_alrm(int);
// 跨栈跳转专用的缓冲区
static jmp_buf		env_alrm;

int
main(void)
{
	// 记录读写字节数
	int		n;
	// 设置read和write系统调用所使用的行缓冲区
	char	line[MAXLINE];

	// 注册需要捕获的信号SIGALRM并绑定其自定义处理函数
	if (signal(SIGALRM, sig_alrm) == SIG_ERR)
		err_sys("signal(SIGALRM) error");
	// 设置跨栈跳转的回溯位置。
	if (setjmp(env_alrm) != 0)
		// 当longjmp()被调用时，将回溯到此位置继续执行。
		// 报出read函数超时的错误，然后终止进程。
		err_quit("read timeout");
	// 设置SIGALRM信号的触发时间为10秒
	alarm(10);
	// 开始执行read()慢速系统调用，从标准输入中读入一行。
	// 如果alarm超时，read将被信号中断而出错返回。
	if ((n = read(STDIN_FILENO, line, MAXLINE)) < 0)
		err_sys("read error");
	//alarm(0);

	// 将从标准输入读入的一行数据写入标准输出。
	write(STDOUT_FILENO, line, n);
	// 正常退出，冲洗标准IO流。
	exit(0);
}

static void
sig_alrm(int signo)
{
	// 调用longjmp()跨栈跳回到setjmp()设置的回溯位置。
	longjmp(env_alrm, 1);
}

// 点评
// 这个案例是10_10_271的改进版，解决了271的两个问题，不论sig_alrm()先于read()阻塞前
// 返回，还是系统调用是否是自动重启动，只要longjmp返回后都将执行err_quit，强行结束read
// 这个慢速系统调用。
// 但本例的缺陷是仍然会有10_10_270_2那样的问题，即和其他信号协同工作时会出现的问题。
// 总结
// 如果要对IO操作设置时间限制，可以使用longjmp，当然也要清楚它可能有与其他信号处理程序交互
// 的问题，另一种选择是使用select或poll函数。
