#include "apue.h"
#include <fcntl.h>

int
main(int argc, char *argv[])
{
	// 接受一个参数（文件名称）。
	if (argc != 2)
		err_quit("usage: a.out <pathname>");
	// 按调用进程的实际用户ID和实际组ID进行文件访问权限测试。
	if (access(argv[1], R_OK) < 0)
		// 测试失败
		err_ret("access error for %s", argv[1]);
	else
		// 可以读取文件
		printf("read access OK\n");
	// 测试能否打开文件
	if (open(argv[1], O_RDONLY) < 0)
		err_ret("open error for %s", argv[1]);
	else
		printf("open for reading OK\n");
	exit(0);
}

// 点评
// 本例主要演示access函数的用法，该函数按实际用户ID和实际组ID进行文件访问权限测试，
// 而不是按有效用户ID和有效组ID。
// faccessat()函数：默认工作方式与access函数一致，但是当flag设置为AT_EACCESS时，
// 则会按调用进程的有效用户ID和有效组ID进行权限测试/
