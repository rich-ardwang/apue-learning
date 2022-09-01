#include "apue.h"

// 闹钟信号自定义处理函数
static void		sig_alrm(int);

int
main(void)
{
	// 记录读写字节数
	int			n;
	// 设置read和write系统调用所使用的行缓冲区，前面已经实验证明
	// 4096效率最佳。
	char		line[MAXLINE];

	// 注册需要捕获的信号SIGALRM并绑定其自定义处理函数
	if (signal(SIGALRM, sig_alrm) == SIG_ERR)
		err_sys("signal(SIGALRM) error");

	// 设置闹钟信号10秒后发出
	alarm(10);
	// 从标准输入读取一行数据，并记录读到的字节数。
	if ((n = read(STDIN_FILENO, line, MAXLINE)) < 0)
		err_sys("read error");
	//alarm(0);

	// 将从标准输入读到的字节写入标准输出。
	write(STDOUT_FILENO, line, n);
	// 正常退出，冲洗标准IO(本例未使用标准IO)。
	exit(0);
}

static void
sig_alrm(int signo)
{
	// 不进行任何处理，直接返回，read()函数会被系统调用中断。
	/* nothing to do, just return to interrupt the read */
}

// 点评
// SIGALRM闹钟信号除了可以实现sleep()函数外，还有中断慢速系统调用的用途，alarm设
// 置时间限制超时后，使得read()函数等慢速系统调用被中断。
// 但是这里存在两个问题：
// 1.alarm和read之间存在race condition，如果alarm时间足够小，可能它会提前超时，
//    这将导致read被永远阻塞。
// 2.如果系统调用是自动重启动的，当从sig_alrm返回时，read不会被其中断，在这种情况
//    下设置时间限制不起作用。

// 低速设备
// read, readv, write, writev和ioctl被称为“低速”设备，所谓的“低速”设备，是指
// I/O调用可能会被永远阻塞。
// 例如，终端，管道或者套接字。如果低速设备上的I/O调用正在传输数据的过程中被信号打断，
// 则返回传输的字节数。

// 慢速系统调用
// 只要可能导致I/O永远阻塞的，就是慢速系统调用。
// 按照定义，pause()函数是慢速的，而sleep()不是（仔细体会）。

// 慢速系统调用被信号中断
// 有两种情况：
// 1).慢速系统调用已经收到n字节的数据时被信号中断，按照POSIX语义，成功返回已读取的
// 字节数n（System V语义是返回错误，而Linux是遵守POSIX标准的）。
// 2).慢速系统调用尚未收到数据被信号中断，返回错误(-1)，同时errno变量置为EINTR 
// (error interrupt)。

// 系统调用自动重启
// 有些慢速系统调用被信号中断后，本应该返回错误的，但是通过开启struct sigaction成员
// sa_flags的SA_RESTART选项，这些慢速系统调用就不会返回错误，而是重新执行一次。
// 如果你使用了signal信号注册函数，SA_RESTART选项默认就是开启的，而大多数时候，我们
// 并不希望开启此选项。
