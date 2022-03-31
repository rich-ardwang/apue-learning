#include "apue.h"
#include <sys/wait.h>

int main(void)
{
	char		buf[MAXLINE];   /* from apue.h */
	pid_t		pid;
	int			status;

	// 进程在终端打印出%+space字符。
	printf("%% ");   /* print prompt (printf requires %% to print %) */
	// fgets是自带缓冲的标准I/O函数，一次从标准输入读取一行，执行每行字符数最大为
	// MAXLINE。
	// 当fgets读取到文件结束符EOF，或出错时，它会返回NULL结束while循环。
	while (fgets(buf, MAXLINE, stdin) != NULL) {
		// fgets返回读取的每行字符串是以\n+null结尾的，使用strlen取得其长度，
		// 注意长度值不包含空字符null，但是包含换行符\n。
		// 将换行符\n替换成空字符null，这样做是因为后面即将调用的函数execlp需要以
		// null结尾的字符串。
		if (buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = 0;   /* replace newline with null */
		// fork进程，即实现进程复制，创建调用进程的一个副本，我们将调用进程称为父
		// 进程，将新创建的进程副本称为子进程。
		// fork被调用一次，但是返回两次，在父子进程中各返回一次，给父进程返回的pid
		// 是一个非负整数，也就是其子进程id，这是父进程唯一可以获取到子进程id的方式。
		// 给子进程返回的pid为0，因为子进程可以通过getppid随时获取到父进程id。
		// fork返回小于0时说明出错。
		if ((pid = fork()) < 0) {
			err_sys("fork error");
		} else if (pid == 0) {   /* child */
			// 在子进程中调用execlp执行fgets读取到的命令，这里只能简单的执行命令，
			// 而没有实现向命令传递参数。
			// fork+exec系函数，这就用新的程序文件替换了原来子进程执行的程序文件，
			// 也就是说子进程的进程空间完全改变了，变成一个全新的进程，和原来被复制
			// 的那个进程完全不一样了。
			// fork+exec系函数的这种组合调用，就是某些系统的spawn一个新进程，在unix
			// 系统将spawn调用分离为两个独立的函数调用。
			execlp(buf, buf, (char *)0);
			// execlp执行成功后不会返回，如果返回就说明执行失败了。
			err_ret("couldn't execute: %s", buf);
			// 标识异常退出。
			exit(127);
		}

		/* parent */
		// 父进程等待子进程退出，并获取其退出状态status。
		if ((pid = waitpid(pid, &status, 0)) < 0)
			err_sys("waitpid error");
		// 子进程执行完毕后，父进程在控制终端打印%符号，为下次循环做准备。
		printf("%% ");
	}
	// 当fgets读取结束或出错时，均按照正常退出处理。
	exit(0);
}
