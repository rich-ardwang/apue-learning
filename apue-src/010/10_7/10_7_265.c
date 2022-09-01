#include "apue.h"
#include <sys/wait.h>

// 自定义信号处理函数
static void		sig_cld(int);

int
main()
{
	// 进程ID
	pid_t		pid;

	// 注册需要捕获的信号SIGCLD，绑定其自定义处理函数。
	if (signal(SIGCLD, sig_cld) == SIG_ERR)
		perror("signal error");
	// 父进程fork子进程
	if ((pid = fork()) < 0) {
		perror("fork error");
	} else if (pid == 0) {		/* child */
		// 子进程休眠2秒
		sleep(2);
		// 正常退出，不冲洗标准IO流。
		_exit(0);
	}

	// 父进程休眠阻塞，直到进程捕获到信号后pause返回。
	pause();	/* parent */
	// 父进程正常退出，冲洗标准IO流。
	exit(0);
}

// 自定义SIGCLD信号的处理函数
static void
sig_cld(int signo)		/* interrupts pause() */
{
	// 进程ID
	pid_t		pid;
	// 保存子进程返回状态
	int			status;

	// 通知收到SIGCLD信号
	printf("SIGCLD received\n");

	// 再次注册，否则不能再次捕获并自定义处理该信号。
	if (signal(SIGCLD, sig_cld) == SIG_ERR)		/* reestablish handler */
		perror("signal error");

	// 父进程调用wait等待子进程退出，并获取其退出状态。
	if ((pid = wait(&status)) < 0)		/* fetch child status */
		perror("wait error");

	// 打印出已退出的子进程ID。
	printf("pid = %d\n", pid);
}

// 点评
// 本节重点讲述SIGCLD与SIGCHLD信号的一些差别，以及如何产生并处理SIGCLD信号。
// 本例在System V系统中是不能正常运行的，需要加以修改，后面会详细说明，而在其他Unix系统
// 中运行是没有问题的。

// SIGCLD与SIGCHLD
// SIGCLD是System V系统定义的，并且提供了SIGCLD的语义，与其他信号处理方式不同，后面要
// 重点介绍一下。
// SIGCHLD是BSD系统定义的，并且提供了SIGCHLD的语义，与其他信号处理类似，一个子进程终止
// 后，该信号被发送给其父进程。
// 这相当于同一个事情存在两种不同的做法，但是后面要做标准化，究竟选用哪一个呢？最后标准化
// 组织决定采用BSD的语义。
// 在Linux平台SIGCLD被定义为SIGCHLD，也就是说它们都是一样的，都采用SIGCHLD语义。
// 而其他Unix平台，SIGCLD也可能被定义为与SIGCHLD完全不一样，这点需要注意一下。

// 古老的System V对SIGCLD的处理方法：
// 1.不去管SIGCLD信号，相当于主动调用signal(SIGCLD, SIG_DFL)，也就是对此信号采用默认
// 处理方式。
// 这需要父进程调用wait或waitpid来给子进程善后，否则会产生僵尸进程。
// 2.主动调用signal(SIGCLD, SIG_IGN)，也就是父进程忽略该信号。
// 子进程状态变化正常或异常退出时，内核会将子进程状态信息等丢弃，这样父进程不需要调用wait
// 或waitpid来给子进程善后，不会产生僵尸进程。
// 这种情况下，如果父进程调用wait或waitpid反而会出现问题，如一直阻塞等不到子进程状态信息
// 等，因为子进程退出状态等信息已被内核丢弃了。
// 3.主动调用signal(SIGCLD, sig_cld)，自定义处理该信号。
// 子进程状态变化（正常或异常退出）后，系统立刻检查是否有子进程状态信息处于被等待，如果是
// 内核就给其父进程发送SIGCLD信号，当再次调用signal(SIGCLD, sig_cld)时会引发系统再次
// 检查是否有子进程状态信息处于被等待。

// 其他Unix系统对SIGCLD的处理方法：
// 1和2与上述基本一致。
// 3.子进程状态变化（正常或异常退出）后，内核就给其父进程发送SIGCLD信号，而不会因为调用
// signal(SIGCLD, sig_cld)引发系统检查是否有子进程状态处于被等待。

// 为什么本例不能在System V平台正常运行
// 子进程退出后系统检测出有子进程状态信息处于被等待，于是发送了SIGCLD信号，然后进入信号处理
// 函数后，又再次调用了signal(SIGCLD, sig_cld)重新注册和绑定，这又会引起系统检测是否有
// 子进程状态信息处于被等待，因为此时wait或waitpid还没有被执行，于是系统又发送SIGCLD信号，
// 这样就会不断执行新的信号处理函数，引起死循环，直到栈空间被耗尽，程序退出为止。
// 修改方法
// 在信号处理函数中，应该先调用wait或waitpid，父进程将子进程退出状态信息收割以后，再调用
// signal(SIGCLD, sig_cld)重新注册和绑定，就不会引起死循环了。

// 本例在其他平台正常运行
// 其他平台采用SIGCHLD的语义，当子进程终止后，发送SIGCLD给父进程，并不会因为父进程调用
// signal(SIGCLD, sig_cld)而引发系统检查是否有子进程状态信息处于被等待，所以SIGCLD
// 信号不会被再次发送，也就不会引起信号处理死循环。

// 僵尸进程产生与消亡
// 1.父进程先于子进程死亡，子进程会被init或upstart等收养，不会产生僵尸进程。
// 2.父进程活着，但是它不调用wait或waitpid给子进程善后，也不设置signal(SIGCLD, SIG_IGN)
// 信号忽略，这种情况子进程终止后会变成僵尸进程。
// 3.父进程死后，它名下所有僵尸进程将被内核清理。
// 总结
// 对现代Unix操作系统而言，无需担心僵尸进程会占用资源的问题，除非父进程一直活着，然后它名下
// 不断产生僵尸进程，这种属于应用程序本身设计有问题。
