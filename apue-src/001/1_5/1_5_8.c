#include "apue.h"

int main(void)
{
	int		c;

	// 演示自带缓冲的标准I/O:
	// 标准I/O函数自带缓冲区，无需关心缓冲区的大小。
	// getc是一次只读取一个字符的标准I/O函数，当读取
	// 到字符串尾部时返回EOF，标识读取工作结束。
	// stdin指定从标准输入读取字符。
	while ((c = getc(stdin)) != EOF)
		// putc是一次只写入一个字符的标准I/O函数，当
		// 其返回EOF时，说明写入出错。
		// stdout指定写入字符至标准输出。
		if (putc(c, stdout) == EOF)
			// putc返回EOF意味着写入字符时出错。
			err_sys("output error");

	// while循环退出后，意味着或者读取到EOF，工作结束，
	// 或者getc出错。
	// 需要使用ferror，并传入stdin，来捕捉是否出错。
	if (ferror(stdin))
		err_sys("input error");

	// 正常退出。
	exit(0);
}

// 点评
// 功能与1_5_7一致，只是实现方式不同，后者使用read和
// write不带缓冲的系统调用，而本例使用标准I/O库函数实现。
