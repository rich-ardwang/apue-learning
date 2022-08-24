#include "apue.h"
#include <sys/wait.h>

// 自定义当前进程的运行环境。
// 每个进程在运行时需要有一个运行环境，例如bash shell的运行环境可以使用env命令查看。
// 这里我们自定义了一个进程运行环境，然后把它传递给execle()函数，让新的进程使用。
char *env_init[] = { "USER=unknown", "PATH=/tmp", NULL };

int
main(void)
{
	// 进程ID
	pid_t		pid;

	// 父进程fork子进程
	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid == 0) {		/* specify pathname, specify environment */
		// fork子进程成功后，让其立即调用exec系列函数，子进程执行完exec之后将完全变成另外
		// 一个全新的进程。
		// 这个全新的进程会从刚刚fork的子进程继承进程ID、父进程ID等一系列资源，然后用自己
		// 的正文段、数据段、堆段和栈段替换掉子进程，并从自己的main()函数开始执行。
		// execle()函数的参数：
		// 1).需要运行的新的程序的pathname，可以是相对路径，也可以是绝对路径。如果程序所在
		// 目录在当前env的PATH中，可以直接使用程序名称。
		// 2).传递新进程的程序名称给它的argv[0]。
		// 3).传递n个参数给新进程的argv[1]、argv[2]...argv[n]，这里传递了2个参数，
		// "myarg1"和"MY ARG2"。
		// 4).最后一个参数是新进程所需的运行环境。
		// [注意]：exec函数执行成功后不再返回。
		if ( execle("./8_10_203", "8_10_203", "myarg1",
				"MY ARG2", (char *)0, env_init) < 0)
			err_sys("execle error");
	}

	// 父进程调用waitpid()等待子进程执行完毕后退出并为其善后。
	// 然后父进程才继续执行后面的代码。
	if (waitpid(pid, NULL, 0) < 0)
		err_sys("wait error");

	// 父进程再次fork一个子进程
	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid == 0) {			/* specify filename, inherit environment */
		// fork子进程成功后，让其立即调用exec系列函数。
		// execlp()函数的参数：
		// 1).需要运行的新的程序的pathname，可以是相对路径，也可以是绝对路径。如果程序所在
		// 目录在当前env的PATH中，可以直接使用程序名称。
		// 2).传递新进程的程序名称给它的argv[0]。
		// 3).传递n个参数给新进程的argv[1]、argv[2]...argv[n]，这里传递了1个参数，
		// "only 1 arg"。
		// [注意]：exec函数执行成功后不再返回。
		if (execlp("./8_10_203", "8_10_203", "only 1 arg", (char *)0) < 0)
			err_sys("execlp error");
	}

	// 父进程不等待第二次fork的子进程执行完毕并为其善后，自己先退出了。
	// 第二次fork的子进程被init或upstart收养，善后的工作就交给养父了。
	exit(0);
}

// 点评
// fork子进程后，让其立即调用exec系列函数，这样子进程已经不是父进程复制出来的那个了，而是变成了
// 一个全新的进程。
// 但是这个全新的进程仍然有一部分资源是从原来的子进程中继承而来，这些资源有：
// 1.进程ID和父进程ID
// 2.实际用户ID和实际组ID
// 3.附属组ID
// 4.进程组ID
// 5.会话ID
// 6.控制终端
// 7.闹钟尚余留的时间
// 8.当前工作目录
// 9.根目录
// 10.文件模式创建屏蔽字
// 11.文件锁
// 12.进程信号屏蔽
// 13.未处理信号
// 14.资源限制
// 15.nice值（遵循XSI的系统）
// 16.tms_utime、tms_stime、tms_cutime以及tms_cstime值

// 执行exec时打开文件的处理
// 1.执行exec时是否关闭已打开文件，由文件描述符是否设置close-on-exec（执行时关闭）标志有关。
// 2.执行exec时必须关闭目录流，由opendir函数调用fcntl函数来关闭。

// 执行exec时用户ID和组ID变化
// 1.exec函数执行前后，实际用户ID和实际组ID保持不变。
// 2.exec函数执行前后，有效ID是否变化，取决于所执行程序文件的设置用户ID位和设置组ID位是否设置。
// 如果新程序的设置用户ID位已设置，则有效用户ID变为新程序文件的所有者ID，否则不变。
// 3.有效组ID的处理方式与有效用户ID的处理方式相同。
