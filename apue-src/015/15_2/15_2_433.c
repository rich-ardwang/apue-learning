#include "apue.h"
#include <sys/wait.h>

// 默认分页程序pathname，使用more这个分页程序。
#define DEF_PAGER		"/bin/more"			/* default pager program */

int
main(int argc, char *argv[])
{
	int			n;
	int			fd[2];
	pid_t		pid;
	char		*pager, *argv0;
	char		line[MAXLINE];
	FILE		*fp;

	// <pathname>为要读取的目标文件。
	if (argc != 2)
		err_quit("usage: a.out <pathname>");

	// 只读模式打开目标文件。
	if ((fp = fopen(argv[1], "r")) == NULL)
		err_sys("can't open %s", argv[1]);
	// 创建匿名管道。
	if (pipe(fd) < 0)
		err_sys("pipe error");

	// fork进程。
	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid > 0) {		/* parent */
		// 父进程关闭输入端fd[0]，保留fd[1]用于输出。
		close(fd[0]);		/* close read end */

		/* parent copies argv[1] to pipe */
		// 父进程把目标文件的内容拷贝至管道输出端fd[1]。
		while (fgets(line, MAXLINE, fp) != NULL) {
			n = strlen(line);
			if (write(fd[1], line, n) != n)
				err_sys("write error to pipe");
		}
		// 考察fgets是否出错。
		if (ferror(fp))
			err_sys("fgets error");

		// 父进程关闭工作在写模式的管道输出端fd[1]。
		close(fd[1]);	/* close write end of pipe for reader */

		// 父进程阻塞等待给子进程收尸。
		if (waitpid(pid, NULL, 0) < 0)
			err_sys("waitpid error");
		exit(0);
	} else {		/* child */
		// 子进程关闭管道的输出端fd[1]，保留输入端fd[0]用于读取接收数据。
		close(fd[1]);		/* close write end */
		if (fd[0] != STDIN_FILENO) {
			// 子进程通过复制文件描述符把管道输入端fd[0]指定为stdin。
			if (dup2(fd[0], STDIN_FILENO) != STDIN_FILENO)
				err_sys("dup2 error to stdin");
			// 这样新的管道输入端为stdin，fd[0]就不再需要了，
			// 可以将其关闭。
			close(fd[0]);	/* don't need this after dup2 */
		}

		/* get arguments for execl() */
		// 读取环境变量"PAGER"，取得系统默认的分页程序pathname，
		// 如果失败，就使用/bin/more程序。
		if ((pager = getenv("PAGER")) == NULL)
			pager = DEF_PAGER;
		// 提取程序名称，去掉所有/和路径，如：/bin/more提取后是more。
		if ((argv0 = strrchr(pager, '/')) != NULL)
			argv0++;		/* step past rightmost slash */
		else
			argv0 = pager;	/* no slash in pager */
		printf("[child] pager: %s\n", argv0);

		// 子进程调用execl系列函数生成完全全新的工作进程，
		// 这里的进程为分页进程，如more。
		if (execl(pager, argv0, (char *)0) < 0)
			err_sys("execl error for %s", pager);
	}
	exit(0);
}

// 本程序读取某个文件内容，将其发送至分页程序。
