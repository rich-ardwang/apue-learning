#include "apue.h"

#define BUFFSIZE	4096

int
main(void)
{
	int 	n;
	char	buf[BUFFSIZE];

	// 从标准输入读取文件，写入到标准输出。
	// 设置read缓冲区大小为4096。
	while ((n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0)
		if (write(STDOUT_FILENO, buf, n) != n)
			err_sys("write error");

	// while循环跳出后，可能正常读取到文件EOF，也可能read执行异常，
	// 所以需要判断read返回值是否小于0，如果出错需要报系统级错误。
	if (n < 0)
		err_sys("read error");

	// 正常退出。
	exit(0);
}

// 点评
// 使用这个程序，我们可以通过修改缓冲区大小来测试不带缓冲的
// read和write系统调用的执行效率。
// APUE的作者做了一系列实验，并把这些实验整理成表格，从实验
// 数据可以看出，缓冲区大小设置为4096时效率最高。
