#include "apue.h"

int
main(int argc, char *argv[])
{
	int			i;
	// 打印进程环境表所需的迭代器指针
	char		**ptr;
	// 外部引入进程环境表，是一个二维数组，类似于*argv[]。
	extern char	**environ;

	// 打印出所有命令行参数，argv[0]为程序名称，argv[1]...argv[n]为命令行参数。
	for (i = 0; i < argc; i++)		/* echo all command-line args */
		printf("argv[%d]: %s\n", i, argv[i]);

	// 打印出进程环境表中所有环境变量。
	for (ptr = environ; *ptr != 0; ptr++)		/* and all env strings */
		printf("%s\n", *ptr);

	// 正常退出，冲洗流。
	exit(0);
}

// 点评
// 这个程序会回显所有命令行参数和进程运行所需的环境表。
// 我们已在8_10_202中使用两个不同的exec函数对其调用了两次。
