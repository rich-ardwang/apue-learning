#include "apue.h"

int
main(void)
{
	char	*ptr;
	size_t	size;
	// 改变进程当前的工作目录。
	//if (chdir("/usr/spool/uucppublic") < 0)
	if (chdir("/var/mail") < 0)
		err_sys("chdir failed");
	// 使用我们自己的path_alloc函数动态分配出一块内存作为目录名称缓冲区用。
	ptr = path_alloc(&size);	/* our own function */
	// 将分配好的path buffer和其size传递给getcwd函数，
	// 该函数会获取该进程的当前工作目录，并把它写入path buffer中。
	if (getcwd(ptr, size) == NULL)
		err_sys("getcwd failed");
	// 打印出path buffer的大小。
	printf("path_alloc:%d\n", size);
	// 打印出该进程当前的工作目录。
	printf("cwd = %s\n", ptr);
	exit(0);
}

// 点评
// 因为chdir只能改变自己所在进程的当前工作目录，而我们要
// 查看该进程的当前工作目录需要使用getcwd函数，这个函数需要我们
// 传入一个缓冲区以及相应的缓冲区大小，用来存放查询结果。
// getcwd()函数相当于shell内建的pwd命令。
