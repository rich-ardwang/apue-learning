#include "apue.h"
#include <pwd.h>

// 自定义SIGALRM信号处理函数
static void
my_alarm(int signo)
{
	// 用于接收getpwnam()返回值
	struct passwd		*rootptr;

	// 通知在信号处理函数中
	printf("in signal handler\n");
	// getpwnam函数不可重入，不应在信号处理函数中调用，
	// 其内部有锁，如果此时进入信号处理函数内部，并执行如下代码，
	// 则getpwnam("root")不会返回，会一直阻塞，发生死锁。
	if ((rootptr = getpwnam("root")) == NULL)
		err_sys("getpwnam(root) error");
	// 重新注册信号处理函数，如果不重新注册则再次收到闹钟信号时
	// 程序会终止，无法继续执行。
	signal(SIGALRM, my_alarm);  // Add by Richard Wang on 2022-8-25
	// 重新设定闹钟信号触发时间为1秒
	alarm(1);
}

int
main(void)
{
	// 用于接收getpwnam()返回值
	struct passwd		*ptr;

	// 注册需要捕获的信号SIGALRM，自定义处理函数。
	signal(SIGALRM, my_alarm);
	// 设置闹钟信号每秒产生1次
	alarm(1);
	for ( ; ; ) {
		// 调用不可重入函数getpwnam，获取sar用户的passwd信息，
		// 该信息写在文件/etc/passwd中。
		if ((ptr = getpwnam("sar")) == NULL)
			err_sys("getpwnam(sar) error");
		// 校验getpwnam()获取的用户passwd信息是否为sar
		if (strcmp(ptr->pw_name, "sar") != 0)
			// 如果getpwnam()取得的用户passwd不是sar，而是其他用户的，那么提示用户名称
			// 被占用，打印出用户名。
			// 这是非常可能发生的，因为我们在SIGALRM信号处理函数中也调用了getpwnam这个
			// 不可重入函数，如果在main函数中的getpwnam刚取得sar用户信息，还没来得及返回，
			// 此时收到SIGALRM信号到信号处理函数中执行getpwnam获取root用户信息，这样main
			// 函数的getpwnam之前获取的sar信息就被root信息覆盖，因这个函数返回值放在静态区域。
			printf("return value corrupted!, pw_name = %s\n", ptr->pw_name);
		// Add by Richard Wang on 2022-8-25
		else
			// getpwnam()成功取得sar用户的passwd信息，打印用户名。
			printf("pw_name = %s\n", ptr->pw_name);
	}
}

// 点评
// 本小节主要讲可重入与不可重入函数的相关概念，这个程序演示了在信号处理函数中调用不可重入函数
// getpwnam的错误用法。

// 重入相关概念
// 重入：同一个函数被不同的执行流调用，当前流程还没有执行完，就被信号等打断，然后在其他执行流
// （如信号处理函数）中被再次调用，或在执行流之间相互嵌套执行。
// 可重入：多个执行流反复执行同一段代码，其结果不会发生改变，通常访问的都是各自的私有栈资源。
// 不可重入：多个执行流反复执行同一段代码时，其结果会发生改变。
// 可重入函数：函数被放在多个执行流中执行，可以任意切换或打断，并且在每个执行流中最终会返回正确
// 结果，这种函数就是可重入的，否则是不可重入函数。

// 可重入函数满足条件
// (1)不使用全局变量或静态变量。
// (2)不使用用malloc或者new开辟出的空间。
// (3)不调用不可重入函数。
// (4)不返回静态或全局数据，所有数据都由函数的调用者提供。
// (5)使用本地数据，或者通过制作全局数据的本地副本来保护全局数据。

// 不可重入函数符合以下条件之一
// (1)调用了malloc/free函数，因为malloc函数是用全局链表来管理堆的。
// (2)调用了标准I/O库函数，标准I/O库的很多实现都以不可重入的方式使用全局数据结构。
// (3)可重入体内使用了静态的数据结构。

// 实验
// 在信号处理函数中调用非可重入函数，则其结果是不可预知的。
// 本书描述：
// 因为main函数和信号处理函数中都调用getpwnam函数，而在getpwnam函数中会调用malloc和free，
// 如果主函数malloc之后收到SIGALRM信号进入信号处理函数，在信号处理函数里malloc然后free，
// 而在信号处理函数调用free和main函数也在调用free时，malloc和free维护的数据结构就出现了损坏。
// 书中描述的是本实验的一种可能结果之一，此外还有几种可能结果：
// 1.main函数中getpwnam函数取得的信息被信号处理函数中getpwnam取得的信息所覆盖，因为它们共用
// 了同样的静态区域。
// 2.在信号处理函数中getpwnam被调用后发生死锁，我们在Linux环境中测试就是这种结果，很可能Linux
// 的getpwnam实现拥有锁机制，在main中的getpwnam执行时加了锁，还没来得及释放，就执行了信号处理
// 函数，然后在信号处理函数中调用getpwnam时等待锁被释放，这样就发生了死锁。

// 总结
// 总之，一定不能在不同的执行流中去调用不可重入函数，否则根据实现的不同，会有不可预测的结果发生。
