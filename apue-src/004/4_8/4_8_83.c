#include "apue.h"
#include <fcntl.h>

#define RWRWRW		(S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

int
main(void)
{
	// 将屏蔽字设为000，即不屏蔽。
	umask(0);
	// 创建文件，权限设为用户读写-组读写-其他读写。
	if (creat("foo", RWRWRW) < 0)
		err_sys("creat error for foo");
	// 设置屏蔽字，屏蔽组读写和其他读写。
	umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	// 创建文件，权限设为用户读写-组读写-其他读写，但由于设置了屏蔽字，
	// 组读写和其他读写被屏蔽，即被禁止了。
	if (creat("bar", RWRWRW) < 0)
		err_sys("creat error for bar");
		exit(0);
}

// 点评
// 本例主要演示屏蔽字设置函数umask用法，通过实验我们可以看出，
// umask函数只能改变进程的屏蔽字，即该屏蔽字只能在进程运行期间有效，
// 进程运行结束后无效。
// 该程序运行前使用命令umask查看当前屏蔽字，运行后再使用命令umask查看，
// 发现屏蔽字无变化，这是因为使用umask命令查看的屏蔽字是bash shell自己的，
// 我们程序中的unask函数不能改变它。
