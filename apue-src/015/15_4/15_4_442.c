#include "apue.h"

int
main(void)
{
	int		n, int1, int2;
	char	line[MAXLINE];

	// 从标准输入按行读取数据。
	while ((n = read(STDIN_FILENO, line, MAXLINE)) > 0) {
		line[n] = 0;		/* null terminate */
		// 将从标准输入读取的数据转换成两个int型。
		if (sscanf(line, "%d%d", &int1, &int2) == 2) {
			// 计算两个int数字的和并将这个和转换成字符串。
			sprintf(line, "%d\n", int1 + int2);
			n = strlen(line);
			// 将结果字符串写入到标准输出。
			if (write(STDOUT_FILENO, line, n) != n)
				err_sys("write error");
		} else {
			// 如果scanf扫描出的参数个数不是2，则输出无效参数错误。
			if (write(STDOUT_FILENO, "invalid args\n", 13) != 13)
				err_sys("write error");
		}
	}
	exit(0);
}

// 15_4_442编译为独立程序后，将作为协同进程被调用。
// 该协同进程从stdin读入两个数，然后计算它们的和，
// 最后写至stdout。
