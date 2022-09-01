#include "apue.h"
#include "10_10_270.c"

// 使用10_10_270.c中定义的sleep2()函数
unsigned int		sleep2(unsigned int);
// SIGINT信号的自定义处理函数
static void			sig_int(int);

int
main(void)
{
	// 还没用完的休眠时间
	unsigned int	unslept;
	// 注册需要捕获的信号SIGINT并绑定其自定义处理函数
	if (signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("signal(SIGINT) error");
	// 调用sleep2()休眠10秒，并记录其还未用完的休眠时间。
	unslept = sleep2(10);
	//unslept = sleep(10);
	// 打引出还未用完的休眠时间
	printf("sleep2 returned: %u\n", unslept);
	// 正常退出，冲洗标准IO流。
	exit(0);
}

static void
sig_int(int signo)
{
	// 局部变量，内外循环用。
	int				i, j;
	// 易变变量，告诉编译不要对其进行优化，每次使用它时要从地址现取。
	volatile int	k;

	/*
	 * Tune these loops to run for more than 5 seconds
	 * on whatever system this test program is run.
	 */
	// 通知sig_int内外循环已经开始。
	printf("\nsig_int starting\n");
	// 外循环
	for (i = 0; i < 300000; i++)
		// 内循环
		for (j = 0; j < 4000; j++)
			// 每次循环累加i*j的值
			k += i * j;
	// 通知sig_int内外循环已经结束。
	printf("sig_int finished\n");
}

// 点评
// 本例演示sleep2()函数和其他信号协同工作时会出现的问题，即有可能longjmp()
// 被调用跨栈跳转后，使其他信号的信号处理函数的栈空间被破坏，导致其他信号没有
// 完整被处理。

// 实验
// sig_int()函数的内外循环执行时间约为4s，我们分两种情况进行实验，一种是设定
// sleep2()的时间大于4s，即让sig_int()函数有机会执行完并返回，另一种是设定
// sleep2()的时间小于4s，即让sleep2()先执行完返回。
// 第一种情况：
// 设定sleep2(10)设置为休眠10秒。
// 执行./10_10_270_2，然后立即Ctrl+C，触发sig_int()函数被执行。
// 执行结果
// sig_int starting
// sig_int finished
// sleep2 returned: 6
// 第二种情况：
// 设定sleep2(3)设置为休眠3秒。
// 执行./10_10_270_2，然后立即Ctrl+C，触发sig_int()函数被执行。
// 执行结果
// sig_int starting
// sleep2 returned: 0

// 结果分析
// 第一种情况：
// sig_int()在4s执行完毕先返回了，这时sleep2()函数中的pause()函数被唤醒返回，
// 于是sleep2()提前结束了，返回剩余未用完的闹钟秒数为6秒。
// 第二种情况：
// 3秒时间到，sleep2()函数的longjmp()被调用，直接跳栈回溯到sig_int()函数被
// 调用前的状态，导致sig_int()函数没有执行完毕。
