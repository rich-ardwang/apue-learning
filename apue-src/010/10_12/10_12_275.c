#include "apue.h"
#include <errno.h>

// 取得进程当前信号屏蔽字的信号集，打印出某些存在其中的信号名称。
void
pr_mask(const char *str)
{
	// 信号集
	sigset_t		sigset;
	// 保存errno
	int				errno_save;

	// 因为在执行本函数下面的某些代码后，errno的值可能被重新设置而发生变化，
	// 为了使本函数不影响errno，我们将其暂存在栈上，待函数返回前再将errno还原。
	errno_save = errno;			/* we can be called by signal handlers */
	// 不改变进程当前信号屏蔽字，取得进程当前信号屏蔽字放在sigset信号集中。
	if (sigprocmask(0, NULL, &sigset) < 0) {
		err_sys("sigprocmask error");
	} else {
		// 打印出pr_mask传递的参数值。
		printf("%s", str);
		// 判断SIGINT信号是否在信号集中，如果在则打印出信号名称。
		if (sigismember(&sigset, SIGINT))
			printf(" SIGINT");
		// 判断SIGQUIT信号是否在信号集中，如果在则打印出信号名称。
		if (sigismember(&sigset, SIGQUIT))
			printf(" SIGQUIT");
		// 判断SIGUSR1信号是否在信号集中，如果在则打印出信号名称。
		if (sigismember(&sigset, SIGUSR1))
			printf(" SIGUSR1");
		// 判断SIGALRM信号是否在信号集中，如果在则打印出信号名称。
		if (sigismember(&sigset, SIGALRM))
			printf(" SIGALRM");

		/* remaining signals can go here */
		// \n会冲洗标准输出流。
		printf("\n");
	}

	// 还原errno的值，这样不影响外层调用者的代码。
	errno = errno_save;		/* restore errno */
}

/* Add by Richard Wang
   2021-12-10
*/
int main() {
	// main函数的信号集
	sigset_t main_sigset;
	// 设置main_sigset包含所有信号。
	// sigfillset(&main_sigset);
	// 初始化空信号集。
	sigemptyset(&main_sigset);
	// 向main_sigset信号集中加入信号。
	sigaddset(&main_sigset, SIGUSR1);
	sigaddset(&main_sigset, SIGINT);
	// 设置进程新的屏蔽字设置为main_sigset集，不取得进程之前的信号屏蔽字。
	if (sigprocmask(SIG_SETMASK, &main_sigset, NULL) < 0)
		err_sys("sig block err.");
	pr_mask("hello, world.\n");
	exit(0);
}

// 点评
// 这个例子主要练习sigprocmask()和sigismember()函数的使用，原书仅仅实现了
// 一个函数pr_mask()，我们自己为这个函数添加了测试代码，而且在测试代码中加入
// sigfillset()、sigemptyset()和sigaddset()等函数，进一步演示了和信号集
// 相关的一些函数的用法。

// 实验
// 分为两种情况进行编译和执行：

// 情况一：新建一个信号集，直接将其中的信号填满（即加入所有种类的信号），然后将
// 这个信号集设置为当前进程的信号屏蔽字，最后调用pr_mask()打印出信号。
// 结果：
// hello, world.
// SIGINT SIGQUIT SIGUSR1 SIGALRM
// 分析：
// pr_mask()打印出了所有四种信号，和我们预期相符合，因为当前进程的信号屏蔽字为
// 包含所有种类的信号。
// 注意：
// 直接调用sigfillset(&main_sigset)即可，不需要初始化信号集。

// 情况二：新建一个空信号集，然后将两种信号SIGUSR1和SIGINT保存到其中，再将这个
// 信号集设置为当前进程的信号屏蔽字，最后调用pr_mask()打印出信号。
// 结果：
// hello, world.
// SIGINT SIGUSR1
// 分析：
// 程序如期的打印出了我们添加入信号屏蔽字的两种信号。
// 注意：
// 初始化新的空信号集时，需要调用sigemptyset()，然后可以向其中加入某些种类的信号。
