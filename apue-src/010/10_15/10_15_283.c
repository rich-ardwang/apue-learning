#include "apue.h"
#include <setjmp.h>
#include <time.h>

// 信号SIGUSR1的自定义处理函数
static void						sig_usr1(int);
// 信号SIGALRM的自定义处理函数
static void						sig_alrm(int);
// 在信号处理程序中进行非局部转移时所使用的
// sigsetjmp和siglongjmp函数专用的缓冲区
static sigjmp_buf				jmpbuf;
// sig_atomic_t数据类型是由ISO C标准定义的变量类型，在写这种类型的
// 变量时不会被中断。
// 这意味着在具有虚拟存储器的系统上，这种变量不会跨越页边界，可以用一条
// 机器指令对其进行访问。
// sig_atomic_t总是包括volatile修饰符，因为该变量将由两个不同的控制
// 线程（main函数和异步执行的信号处理程序）访问。
static volatile sig_atomic_t	canjump;

int
main(void)
{
	// 注册需要捕捉的信号SIGUSR1，绑定其自定义信号处理函数。
	if (signal(SIGUSR1, sig_usr1) == SIG_ERR)
		err_sys("signal(SIGUSR1) error");

	// 注册需要捕捉的信号SIGALRM，绑定其自定义信号处理函数。
	if (signal(SIGALRM, sig_alrm) == SIG_ERR)
		err_sys("signal(SIGALRM) error");

	/*
	// Add br Richard Wang on 2021-12-13
	// 屏蔽SIGINT信号。
	sigset_t curset;
	sigemptyset(&curset);
	sigaddset(&curset, SIGINT);
	if (sigprocmask(SIG_SETMASK, &curset, NULL) < 0)
		err_sys("sigprocmask error");
	*/

	// 打印出当前进程屏蔽字中存在的一些信号。
	pr_mask("starting main: ");			/* Figure 10.14 */

	// 调用sigsetjmp()设置非局部转移回溯的位置。
	if (sigsetjmp(jmpbuf, 1)) {
		// 当siglongjmp()被调用后，进程将回溯到此位置继续执行，
		// 同时恢复之前的进程屏蔽字，之前的进程屏蔽字由sigsetjmp()保存。
		// 打印出进程屏蔽字中存在的一些信号，与starting main比较一下，
		// 看进程屏蔽字是否恢复为最初状态。
		pr_mask("ending main: ");
		// 正常退出进程，冲洗标准IO流。
		exit(0);
	}
	// 将sig_atomic_t类型值置为有效
	canjump = 1;	/* now sigsetjmp() is OK */

	for ( ; ; ) {
		// 打印进程ID
		printf("pid=%d\n", getpid());  // Add br Richard Wang on 2021-12-13
		// 进程休眠阻塞，直到捕获到信号并处理完毕，pause会返回，
		// 然后进入下一次循环。
		pause();
	}
}

static void
sig_usr1(int signo)
{
	// 存储该信号处理函数的开始时间点。
	time_t		starttime;

	// 如果canjump为0，则立刻返回，不进行任何处理。
	if (canjump == 0)
		return;					/* unexpected signal, ignore */

	// 打印当前进程屏蔽字中有哪些信号。
	pr_mask("starting sig_usr1: ");

	// 设置SIGALRM信号的触发时间为3秒。
	alarm(3);					/* SIGALRM in 3 seconds */
	// 记录开始时间点
	starttime = time(NULL);
	for ( ; ; )					/* busy wait for 5 seconds */
		// 不断进行for循环迭代，直到5秒过后，跳出for循环。
		if (time(NULL) > starttime + 5)
			break;

	// 再次打印当前进程信号屏蔽字中有哪些信号。
	pr_mask("finishing sig_usr1: ");

	// canjump置为无效
	canjump = 0;
	// 调用siglongjmp()非局部转移到sigsetjmp()设置的回溯位置。
	// siglongjmp()恢复sigsetjmp()之前所保存的进程屏蔽字。
	siglongjmp(jmpbuf, 1);		/* jump back to main, don't return */
}

static void
sig_alrm(int signo)
{
	// 收到闹钟信号后进入到该处理函数。
	// 打印当前进程屏蔽字中有哪些信号。
	pr_mask("in sig_alrm: ");
}

// 点评
// 7.10节说明了用于非局部转移的setjmp()和longjmp()函数，但是调用longjmp有一个问题，
// 当捕捉到信号并进入信号处理函数，此时当前信号被自动地加入到进程的信号屏蔽字中，这阻止
// 了后来产生的这种信号中断该信号处理函数。
// 如果用longjmp跳出信号处理函数，那么进程的信号屏蔽字会发生什么变化呢？
// 1).FreeBSD 8.0和Mac OS X 10.6.8中，setjmp和longjmp保存和恢复信号屏蔽字。
// 2).FreeBSD 8.0和Mac OS X 10.6.8中，_setjmp和_longjmp不保存和恢复信号屏蔽字。
// 3).Linux 3.2.0和Solaris 10不保存和恢复信号屏蔽字。(虽然Linux支持提供BSD行为的选项)

// setjmp()和longjmp()函数 与 sigsetjmp()和siglongjmp()函数 区别
// 1).sigsetjmp()增加了一个参数savemask，如果savemask非0，则sigsetjmp在env中保存
// 进程的当前信号屏蔽字。
// 调用siglongjmp时，如果之前sigsetjmp已保存进程信号屏蔽字，则siglongjmp从其env中
// 恢复进程之前的信号屏蔽字。
// 2).在信号的处理函数中鼓励使用sigsetjmp()和siglongjmp()，这在各个平台会取得一致
// 的结果。

// 实验
// 执行./10_15_283（本程序已改为前台执行，原书程序是./10_15_283 &后台执行），然后在
// 另一个shell中执行kill -USR1 <pid>给上述进程发送SIGUSR1信号。
// 结果：
/*
starting main: 
pid=4157
starting sig_usr1: 
in sig_alrm: 
finishing sig_usr1: 
ending main: 
*/
// 结果分析：
// 上述是我们在Linux平台测试的结果，可以看到信号处理函数并没有自动把该信号加入进程当前屏蔽字
// 信号集，此时如果有相同信号发出，进程扔然可以收到，并且信号处理函数可能被打断。
// 这与书中在其他Unix平台测试结果有很大不同，书中的测试结果是进入信号处理函数后，该信号会被
// 自动加入进程当前屏蔽字，这样后续发生相同信号将会被阻止，不会打断当前的信号处理，当离开信号
// 处理函数后，该信号被自动从进程屏蔽字中移除。
// 结果差异分析：
// Linux平台与原书测试的巨大差异，是由于sa_flags的选项SA_NODEFER在不同平台是否默认被设置
// 有直接关系，Linux平台默认设置了SA_NODEFER标记，那么在该信号处理函数运行时，内核将不会
// 阻塞该信号，而其他平台默认没有设置SA_NODEFER标记，那么当信号处理函数运行时，内核将阻塞
// 该给定信号。

// 其他注意
// 1).siglongjmp返回后会恢复之前sigsetjmp所保存的信号屏蔽字，这一点在Linux平台测试不出，
// 但是原书测试结果却能很明显证实。
// 但其实Linux平台也是一样的原理和效果，只不过它不阻塞信号，我们测试看不出结果而已。
// 2).在Linux平台，该程序处理第一个SIGUSR1信号期间，我们再发送一个SIGUSR1，那么进程将立即终止，
// 这是因为Linux信号处理期间内核不阻塞该信号，当再次收到该信号时，采用了对该信号的系统默认处理。
// 3).使用canjump变量是一种信号处理的防御机制，当jmpbuf被设置完毕才将canjump置为有效，然后
// 在信号处理函数中先判断canjump是否有效，有效才进行处理，否则立即返回。
// 为了防止在jmpbuf缓冲区还没有被设置好，信号就到了，然后在信号处理函数中调用siglongjmp将
// 出问题，因为调用siglongjmp之前，必须确保sigsetjmp完全设置好jmpbuf才可以。
