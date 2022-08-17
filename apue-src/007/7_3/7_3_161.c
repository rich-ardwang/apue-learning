#include "apue.h"

// 静态函数声明，这些函数将注册到atexit，然后在main()函数退出后逐一被执行。
static void		my_exit1(void);
static void		my_exit2(void);

int
main(void)
{
	// 将my_exit2()注册到atexit()一次
	if (atexit(my_exit2) != 0)
		err_sys("can't register my_exit2");

	// 将my_exit2()注册到atexit()两次
	if (atexit(my_exit1) != 0)
		err_sys("can't register my_exit1");
	if (atexit(my_exit1) != 0)
		err_sys("can't register my_exit1");

	// 提示main函数退出
	printf("main is done\n");
	// 和exit(0)是等价的
	return (0);
}

static void
my_exit1(void)
{
	printf("first exit handler\n");
}

static void
my_exit2(void)
{
	printf("second exit handler\n");
}

// 点评
// 本例说明如何使用atexit()函数，自定义函数可以注册到atexit，注册次数可以多次，
// 当main函数正常结束后，注册的自定义函数就会被执行，注册几次就会被执行几次。
