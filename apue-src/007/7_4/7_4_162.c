#include "apue.h"

int
main(int argc, char *argv[])
{
	int			i;

	for (i = 0; i < argc; i++)		/* echo all command-line args */
		printf("argv[%d]: %s\n", i, argv[i]);
	exit(0);
}

// 点评
// 本例演示将所有命令参数输出，argc是命令行参数的个数，**argv与*argv[]等效，表示命令行
// 参数的内容。
// 第0个参数是应用程序名称，一般不会被用到，其他参数为用户传递的参数。

// 如：
// ./7_4_162 p1 p2 p3，则输出结果：
// argv[0]: ./7_4_162
// argv[1]: p1
// argv[2]: p2
// argv[3]: p3

// 扩展
// 其实main()函数还有第3个参数: char *envp[]或者char **envp，这个参数储存一张环境表，
// 每个应用程序都有一张环境表，该表中字段如HOME、PATH、SHELL、USER和LOGNAME等。
