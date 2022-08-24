#include "apue.h"
#include <errno.h>
#include <sys/time.h>

// 针对不同Unix平台使用不同的系统限制
#if defined(MACOS)
#include <sys/syslimits.h>
#elif defined(SOLARIS)
#include <limits.h>
#elif defined(BSD)
#include <sys/param.h>
#endif

// 全局变量声明
unsigned long long count;  // 计数器，默认被初始化为0
struct timeval end;  // 保存程序运行结束时间

// 检查程序结束时间，如果结束时间到，则打印出count值，然后退出。
void
checktime(char *str)
{
	// 该结构体有秒和微秒两个成员变量
	struct timeval		tv;

	// gettimeofday()现已被弃用，但有的程序还是要使用的，因为它比time()
	// 函数提供了更高的精度，达到微秒级。
	// 该函数将从格林威治时间1970年1月1日00:00:00至今经历的秒数和微秒数
	// 填入timeval中。
	// 第二个参数最好为NULL，其他的话可能产生不确定结果。
	gettimeofday(&tv, NULL);
	// 同时比较秒和微秒（精度较高），判断是否到程序结束时间。
	if (tv.tv_sec >= end.tv_sec && tv.tv_usec >= end.tv_usec) {
		// 打印我们给父子进程定义的名称和其相应的count值
		printf("%s count = %lld\n", str, count);
		// 正常退出，冲洗标准IO流（对标准输出无影响，它已被设置为无缓冲）。
		exit(0);
	}
}

int
main(int argc, char *argv[])
{
	pid_t		pid;  // 进程ID
	char		*s;   // 我们给父子进程定义的名称
	int			nzero, ret;  // nzero保存当前平台默认nice值，ret保存nice()函数返回值。
	int			adj = 0;  // 保存输入参数

	// 设置标准输出为无缓冲模式
	setbuf(stdout, NULL);

// 获取当前平台默认的nice值，nice值的范围为0～(2*NZERO-1)或0～2*NZERO。
// nice值越高，进程优先级越低。
// 如果平台定义了NZERO，那么直接取其值，否则调用sysconf()去取。
#if defined(NZERO)
	nzero = NZERO;
#elif defined(_SC_NZERO)
	nzero = sysconf(_SC_NZERO);
#else
#error NZERO undefined
#endif
	// 打印当前平台的默认nice值。
	printf("NZERO = %d\n", nzero);

	// 该程序接受一个输入参数，该参数为用户希望增加的子进程nice值。
	if (argc == 2)
		// 将输入参数转为10进制long类型
		adj = strtol(argv[1], NULL, 10);
	
	// 取得当前时间的秒和微秒数
	gettimeofday(&end, NULL);
	// 我们设置程序结束时间为当前时间+10秒，也就是仅让程序运行10秒。
	end.tv_sec += 10;	/* run for 10 seconds */

	// 父进程fork子进程
	if ((pid = fork()) < 0) {
		err_sys("fork failed");
	} else if (pid == 0) {		/* child */
		// 给子进程定义名称
		s = "child";
		// nice()函数可以设置当前进程的nice值，然后返回设置后的nice值。
		// nice()函数设置的nice只对调用进程自己有效，不影响其他进程。
		// 打印出子进程默认的nice值和希望调升的数量。
		printf("current nice value in child is %d, adjusting by %d \n",
			nice(0)+nzero, adj);
		// 重置一下errno
		errno = 0;
		// 提升子进程nice值，并判断是否出错。
		if ((ret = nice(adj)) == -1 && errno != 0)
			err_sys("child set scheduling priority");
		// 打印出子进程默认nice值+提升nice值。
		printf("now child nice value is %d\n", ret+nzero);
	} else {		/* parent */
		// 给父进程定义名称
		s = "parent";
		// 打印出父进程默认的nice值。
		printf("current nice value in parent is %d\n", nice(0)+nzero);
	}
	// 父子进程都在10秒内不断累加count
	for (;;) {
		// count初始值为0，先将其+1。
		if (++count == 0)
			// 如果count值+1后还是为0，说明进程出错。
			err_quit("%s counter wrap", s);
		// 每次循环都要检查程序结束时间。
		checktime(s);
	}
}

// 点评
// 本程序设计运行10秒，在10秒内父子进程都进行累加计数，通过给子进程设置不同的nice值，
// 然后打印出父子进程的计数值进行观察，我们看出nice对进程优先级的影响。
// 一般是nice值越高，表示进程越友好，那么它的优先级就越低。

// 实验
// 调用./8_16_222和./8_16_222 20，两次调用打印结果如下：
/*
NZERO = 20
current nice value in parent is 20
current nice value in child is 20, adjusting by 0 
now child nice value is 20
parent count = 317136147
child count = 317726588

NZERO = 20
current nice value in parent is 20
current nice value in child is 20, adjusting by 20 
now child nice value is 39
child count = 313969720
parent count = 314290914
*/

// 结果分析
// 可以看到，子进程已经最高nice值了，但是父子进程的优先级似乎没有受到影响，
// 内核调用它们还是几乎平均的(一般不会一致，因为内核调度不是精确的)。
// 这是因为，我们实验的机器为多核CPU，父子进程并不共享一个CPU，所以内核调度
// 上闲置资源较多，子进程优先级也就没有被压缩。
// 如果该单核CPU来运行本程序，就能很明显看出子进程nice越大，被内核执行的
// 时间就越少。
