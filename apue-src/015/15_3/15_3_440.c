#include "apue.h"
#include <ctype.h>

int
main(void)
{
	int		c;

	while ((c = getchar()) != EOF) {
		if (isupper(c))
			c = tolower(c);
		if (putchar(c) == EOF)
			err_sys("output error");
		if (c == '\n')
			fflush(stdout);
	}
	exit(0);
}

// 点评：
// 该程序的主要任务是从stdin读入字符，
// 将其转换成小写字母，并写入stdout。
