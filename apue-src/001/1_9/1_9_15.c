#include "apue.h"
#include <sys/wait.h>

// 声明静态的自定义信号处理函数。
static void sig_int(int);   /* our signal-catching function */

int main(void)
{
	char		buf[MAXLINE];   /* from apue.h */
	pid_t		pid;
	int			status;

	// signal是信号捕捉函数，参数1指定需要捕捉的信号，参数2设置自定义的
	// 信号处理函数，用于处理参数1捕捉到的信号。
	if (signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("signal error");

	// 控制终端打印%提示符。
	printf("%% ");   /* print prompt (printf requires %% to print %) */
	// fgets每次从stdin读取一行，最大字符数4096。
	while (fgets(buf, MAXLINE, stdin) != NULL) {
		// 将读取的命令处理成以null结尾，作为后续execlp的输入参数。
		if (buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = 0;   /* replace newline with null */

		// fork进程。
		if((pid = fork()) < 0) {
			err_sys("fork error");
		} else if (pid == 0) {   /* child */
			// 子进程执行命令，将自己更新为全新的进程，execlp执行成功后不返回。
			execlp(buf, buf, (char *)0);
			// 设置出错信息。
			err_ret("couldn't execute: %s", buf);
			exit(127);
		}

		/* parent */
		// 父进程等待子进程退出并获取其退出状态。
		if ((pid = waitpid(pid, &status, 0)) < 0)
			err_sys("waitpid error");
		// 父进程打印用于下一轮循环的提示符。
		printf("%% ");
	}
	// 正常退出。
	exit(0);
}

// 定义静态的自定义信号处理函数。
void sig_int(int signo)
{
	// 打印一条中断信息，并打印一个新的提示符%。
	printf("interrupt\n%% ");
}

// 点评
// 该例子演示了信号处理的基本用法，信号由内核产生，根据不同
// 的实际情况，内核产生不同类型的信号发送给用户进程。
// 用户进程对信号一般有三种处理办法：
// 1.忽略信号。如有些硬件异常的信号不需要处理，应该采用忽略
// 的手段。
// 2.按系统默认方式处理。有的信号默认处理为报某种错误信息，
// 有的信号默认处理方式为终止进程等。
// 3.提供一个自定义信号处理函数，专门处理某个信号。本例就是
// 捕捉SIGINT信号，然后提供自定义函数专门处理该信号。
