#include "apue.h"
#include <sys/wait.h>

int
main(void)
{
	char		line[MAXLINE];
	FILE		*fpin;

	// 将15_3_440作为一个过滤程序来调用，这个过滤程序可以将
	// 大写字母转换为小写字母。
	// popen在"r"模式下会将子进程的管道pfd[1]指向stdout，
	// 将父进程的管道pfd[0]已只读方式读取子进程的stdout，所以
	// 对于父进程来说接收管道信息就相当于文件输入端(fpin)，而子
	// 进程向管道发送信息就是输出端(stdout)。
	if ((fpin = popen("./15_3_440", "r")) == NULL)
		err_sys("popen error");
	for (;;) {
		// 向stdout输出提示字符串。
		fputs("prompt> ", stdout);
		// 因为向stdout输出的提示字符串中没有\n，不会引起行缓冲冲洗，
		// 所以这里要手动冲洗内核缓冲区。
		fflush(stdout);
		// 读取过滤程序发来的数据，将其写入stdout。
		if (fgets(line, MAXLINE, fpin) == NULL)		/* read from pipe */
			break;
		if (fputs(line, stdout) == EOF)
			err_sys("fputs error to pipe");
	}
	// 关闭文件，阻塞等待popen的子进程退出。
	if (pclose(fpin) == -1)
		err_sys("pclose error");
	// 冲洗内核缓冲区。
	putchar('\n');
	exit(0);
}
