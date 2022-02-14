#include "apue.h"
#include <sys/wait.h>

// 这条shell命令的意思是，如果shell变量PAGER已经定义且非空，
// 则使用其值，否则使用"more"。
#define PAGER		"${PAGER:-more}"	/* environment variable, or default */

int
main(int argc, char *argv[])
{
	char line[MAXLINE];
	FILE *fpin, *fpout;

	if (argc != 2)
		err_quit("usage: a.out <pathname>");
	
	// 只读模式打开目标文件，返回FILE*指针。
	if ((fpin = fopen(argv[1], "r")) == NULL)
		err_sys("can't open %s", argv[1]);

	// popen会先创建一根管道，然后fork进程，让子进程
	// 执行sh -c <cmdstring>这条shell命令建立一个全新
	// 的子进程。
	// popen执行fork进程后会根据读写模式，设置不同管道数据
	// 传输方向：
	// "w"模式，设置父进程的fpout为管道输出端，子进程的stdin为管道输入端。
	// "r"模式，设置父进程的fpout为管道输入端，子进程的stdout为管道输出端。
	if ((fpout = popen(PAGER, "w")) == NULL)
		err_sys("popen error");

	/* copy argv[1] to pager */
	// 将目标文件内容拷贝至父进程的fpout管道输出端，
	// 这样数据就会传递给子进程的stdin管道输入端。
	while (fgets(line, MAXLINE, fpin) != NULL) {
		if (fputs(line, fpout) == EOF)
			err_sys("fputs error to pipe");
	}
	// 检查fgets工作是否出错。
	if (ferror(fpin))
		err_sys("fgets error");
	// pclose函数关闭标准IO流，等待命令终止，然后返回shell
	// 的终止状态。
	if (pclose(fpout) == -1)
		err_sys("pclose error");

	exit(0);
}
