#include "apue.h"
#include <errno.h>
#include <limits.h>

// 最大打开文件数。
#ifdef OPEN_MAX
static long openmax = OPEN_MAX;
#else
static long openmax = 0;
#endif

/*
 * If OPEN_MAX is indeterminate, this might be inadequate.
 */
// 如果OPEN_MAX是未定义的，这个自定义值可能不是足够大。
#define OPEN_MAX_GUESS 256

long
open_max(void)
{
	// 如果OPEN_MAX是未定义的，这会首次进入执行。
	if (openmax == 0) {		/* first time through */
		errno = 0;
		// 调用sysconf获取运行时的值。
		// 如果返回值小于0，需要检验errno的值。
		// 反之，说明正确取得OPEN_MAX的值。
		if ((openmax = sysconf(_SC_OPEN_MAX)) < 0) {
			// 如果errno为0，则说明系统对于OPEN_MAX是未定义的，
			// 需要使用自定义值。
			if (errno == 0)
				openmax = OPEN_MAX_GUESS;	/* it's indeterminate */
			else
				// 否则说明sysconf调用出错。
				err_sys("sysconf error for _SC_OPEN_MAX");
		}
	}
	// 返回openmax值。
	return (openmax);
}

/* Add by Richard Wang
   2021-12-8
*/
int main() {
	printf("The max count of file descripotors is %ld.\n", open_max());
	return 0;
}

// 点评
// 本例演示了动态获取最大打开文件数的方法。
// 有的守护进程需要关闭所有已经打开的文件，如果最大打开文件数
// 有定义，这是没有问题的，只要编写循环逐一关闭即可。
// 但是，如果OPEN_MAX未定义，直接编写循环将不会被执行。
// 1.采用逐一调用close，然后判断出错返回也不是一个好办法，因为
// close的出错返回并不区分无效描述符和未打开描述符，如果5未打
// 开，6打开，使用这种技术会导致停止在5上，而6不会被关闭。
// 2.dup函数在超过OPEN_MAX时确实会返回一个特定的出错信息，但是
// 这样做往往需要复制一个描述符几百次才能知道结果。
// 3.如果使用LONG_MAX的值作为限定，其值为2147483647，这会使
// 程序运行非常糟糕。
// 以上三种方法均不可行，所以我们编写了本例，这是一种相对不错的
// 解决方案。
