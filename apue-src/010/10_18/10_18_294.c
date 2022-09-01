#include "apue.h"

// SIGINT信号的自定义信号处理函数
static void
sig_int(int signo)
{
	// 通知已捕捉到SIGINT信号
	printf("caught SIGINT\n");
}

// SIGCHLD信号的自定义信号处理函数
static void
sig_chld(int signo)
{
	// 通知已捕捉到SIGCHLD信号
	printf("caught SIGCHLD, pid=%d\n", getpid());
}

int
main(void)
{
	// 注册需要捕获的信号SIGINT，绑定其自定义信号处理函数。
	if (signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("signal(SIGINT) error");
	// 注册需要捕获的信号SIGCHLD，绑定其自定义信号处理函数。
	if (signal(SIGCHLD, sig_chld) == SIG_ERR)
		err_sys("signal(SIGCHLD) error");
	// 调用system函数执行程序/bin/ed，该程序是一个著名的Unix编辑器。
	int sr = system("/bin/ed");
	// 如果system返回值小于0，说明执行出错。
	if (sr < 0)
		err_sys("system() error");
	// Add by Richard Wang on 2022-8-29
	// 如果system函数工作正常，打印出其返回值。
	printf("system return value: %d\n", sr);

	// 正常退出进程，冲洗标准IO流。
	exit(0);
}

// 点评
// 本例重点演示system函数的用法和一些注意事项。
// system函数会fork子进程，然后让子进程调用exec系列函数执行/bin/sh，然后让sh再次
// fork出一个子进程，最后让该子进程调用exec函数执行system参数传进去的命令或程序，
// 这里是/bin/ed命令。
// 相当于10_18_294、sh和ed组成了一个前台进程组，bash自己是后台进程组，它们构成了
// 一个完整的会话，该会话进程关系如下：
// bash        #登录bash，10_18_294的父进程。
// 10_18_294   #本程序，sh的父进程
// sh          #/bin/sh，/bin/ed的父进程
// ed          #/bin/ed，sh的子进程
// ed是一个文本编辑器，很早就是Unix的组成部分，它对SIGINT信号的处理方式是输出?号，对
// SIGQUIT信号的处理方式是忽略。

// system函数忽略SIGINT和SIGQUIT信号，执行时阻塞SIGCHLD信号，返回后解除对SIGCHLD信
// 号的阻塞，那么SIGINT会发送给前台进程组的所有进程，10_18_294收不到该信号，因为该信号
// 被system函数忽略了，sh默认忽略SIGINT，只有ed进程才能捕获并处理，我们可以看到只有?号
// 输出，而没有"caught SIGINT\n"就说明了这一点。
// 这样做的原因如下：
// 1).调用system的进程可以调用多个system产生多个终端来执行命令，所以它不应该收到来自终
// 端的两个信号SIGINT和SIGQUIT，也就是说终端的命令状态不能影响到system调用者。
// 2).system调用者在调用system之前可能已经fork出多个子进程，如果在system执行期间发送
// SIGCHLD信号给它，调用者可能会误以为自己的子进程终止了，从而调用waitpid获取其状态，这
// 样就会影响到system函数内部sh调用waitpid对子进程状态的收集，从而影响system函数的返回。
// 3).而system返回后就可以解除对SIGCHLD信号的阻塞了，这是因为system的调用者也需要知道
// system内部sh子进程的退出状态。

// 实验分析
// 1).运行./10_18_294后，我们键入多个Ctrl+C，终端都会输出?号，上面已经解释过了。
// 2).运行./10_18_294后，我们键入Ctrl+\，产生的SIGQUIT信号被system忽略，10_18_294
// 收不到此信号，ed对此信号是忽略的，但是sh收到此信号后退出了，system返回值为131，因为
// sh终端进程终止了，那么它运行的子进程ed自然就强行被终止，sh终止后system返回，此时可以
// 给父进程10_18_294发送SIGCHLD信号了。
// 3).运行./10_18_294后，我们键入q，ed进程对q命令的处理是正常终止，于是sh也正常终止，
// system函数返回，给父进程10_18_294发送SIGCHLD信号。

// system返回值
// system的返回值是128+sh被哪个信号终止的那个信号编码，如sh被SIGQUIT终止，所以system
// 返回值是128+3，也就是131。
// SIGQUIT信号编码为3，SIGINT为2。
