#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int
main(void)
{
	// time_t用于记录从1970年1月1日00:00:00（格林威治时间）至今所经过的秒数，
	// 我们称之为日历时间。
	time_t			t;
	// 一个专门用于描述时间、星期和年月日等详细信息的结构体。
	struct tm		*tmp;
	// 下面两个缓冲区给strftime()函数使用。
	char			buf1[16];
	char			buf2[64];

	// 计算格林威治时间至今所经过的秒数，存储为日历时间。
	time(&t);
	// localtime()函数将日历时间转换为本地时间(考虑到本地时区和夏令时标志)。
	// 将转换后的时间存储到tm结构体，将其指针返回。
	tmp = localtime(&t);
	// strftime()函数是时间格式化输出函数，它可以把tm时间以任意我们给定的格式进行输出。
	// 我们使用了两个不同大小的缓冲区，为了演示16个字节的小缓冲区无法容纳输出信息。
	if (strftime(buf1, 16, "time and date: %r, %a %b %d, %Y", tmp) == 0)
		printf("buffer length 16 is too small\n");
	else
		printf("%s\n", buf1);
	if (strftime(buf2, 64, "time and date: %r, %a %b %d, %Y", tmp) == 0)
		printf("buffer length 64 is too small\n");
	else
		printf("%s\n", buf2);
	exit(0);
}

// 扩展
// 1.有一个和localtime()相类似的函数gmtime()，该函数会将日历时间转换为UTC(协调统一时间)
// 的年、月、日、时、分、秒、周日分解结构。
// 2.strptime()函数是strftime()函数反过来的版本，它可以把字符串时间转换为分解时间（tm结构体）。
