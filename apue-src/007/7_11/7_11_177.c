#include "apue.h"
#include <sys/resource.h>

// #name convert int to char *
// doit(name)中的name为int类型的资源标识符，本定义将这个资源标识符
// 一分为二，用#name将int型资源标识符转换为char *，作为pr_limits()
// 的第一个参数，用name这个int类型作为pr_limits()的第二个参数。
// 例如：
// doit(RLIMIT_AS)转换为pr_limits("RLIMIT_AS", RLIMIT_AS)
#define	doit(name)		pr_limits(#name, name)

// 全局静态函数声明
static void		pr_limits(char *, int);

int
main(void)
{
// 进程总的可用存储空间的最大长度（字节）。
// 这影响到sbrk函数和mmap函数。
#ifdef	RLIMIT_AS
	doit(RLIMIT_AS);
#endif

	// core文件的最大字节数，若其值为0则阻止创建core文件。
	doit(RLIMIT_CORE);
	// CPU时间的最大量值（秒），当超过此限制时，向进程发送SIGXCPU信号。
	doit(RLIMIT_CPU);
	// 数据段的最大字节长度。数据段是初始化数据、非初始以及堆的总和。
	doit(RLIMIT_DATA);
	// 可以创建的文件的最大字节长度。当超过此限制时，则向进程发送SIGXFSZ信号。
	doit(RLIMIT_FSIZE);

// 一个进程使用mlock()能够锁定在存储空间中的最大字节长度。
#ifdef	RLIMIT_MEMLOCK
	doit(RLIMIT_MEMLOCK);
#endif

// 进程为POSIX消息队列可分配的最大存储字节数。
#ifdef	RLIMIT_MSGQUEUE
	doit(RLIMIT_MSGQUEUE);
#endif

// 为了影响进程的调度优先级，nice值可设置的最大限制。
#ifdef	RLIMIT_NICE
	doit(RLIMIT_NICE);
#endif

	// 每个进程能打开的最多文件数。
	// 更改此限制将影响到sysconf函数在参数_SC_OPEN_MAX中返回的值。
	doit(RLIMIT_NOFILE);

// 每个实际用户ID可拥有的最大子进程数。
// 更改此限制将影响到sysconf函数在参数_SC_CHILD_MAX中返回的值。
#ifdef	RLIMIT_NPROC
	doit(RLIMIT_NPROC);
#endif

// 用户可同时打开的伪终端的最大数量。
#ifdef	RLIMIT_NPTS
	doit(RLIMIT_NPTS);
#endif

// 最大驻内存集字节长度（resident set size in bytes, RSS）。
// 如果可用的物理存储器非常少，则内核将从进程处取回超过RSS的部分。
#ifdef	RLIMIT_RSS
	doit(RLIMIT_RSS);
#endif

// 在任意给定时刻，一个用户可以占用的套接字缓冲区的最大长度（字节）。
#ifdef	RLIMIT_SBSIZE
	doit(RLIMIT_SBSIZE);
#endif

// 一个进程可排队的信号最大数量。
// 这个限制是sigqueue函数实施的。
#ifdef	RLIMIT_SIGPENDING
	doit(RLIMIT_SIGPENDING);
#endif

	// 栈的最大字节长度。
	doit(RLIMIT_STACK);

// 用户可消耗的交换空间的最大字节数。
#ifdef	RLIMIT_SWAP
	doit(RLIMIT_SWAP);
#endif

// 这是RLIMIT_AS的同义词。
#ifdef	RLIMIT_VMEM
	doit(RLIMIT_VMEM);
#endif

	exit(0);
}

// 该函数打印出资源标识符的名称和其相应的限制值
static void
pr_limits(char *name, int resource)
{
	// 资源限制专用的结构体，只有软限制和硬限制这两个成员。
	struct rlimit		limit;
	// 软限制和硬限制的类型有的平台是unsigned long，有的平台
	// 是unsigned long long，这里为了兼容平台。
	unsigned long long	lim;

	// 调用getrlimit()函数取得该资源标识符相应的资源限制信息。
	if (getrlimit(resource, &limit) < 0)
		err_sys("getrlimit error for %s", name);
	// 打印出资源标识符的名称
	printf("%-14s ", name);
	if (limit.rlim_cur == RLIM_INFINITY) {
		// 该资源的软限制为不受限制
		printf("(infinite) ");
	} else {
		// 打印出该资源的软限制值
		lim = limit.rlim_cur;
		printf("%10lld ", lim);
	}
	if (limit.rlim_max == RLIM_INFINITY) {
		// 该资源的硬限制为不受限制
		printf("(infinite)");
	} else {
		// 打印出该资源的硬限制值
		lim = limit.rlim_max;
		printf("%10lld", lim);
	}
	// 向标准输出流写入换行符
	putchar((int)'\n');
}

// 点评
// 本例重点讨论资源限制，系统中针对资源限制分为软限制和硬限制两种。
// 资源限制影响到调用进程并由其子进程继承，为了影响一个用户的所有
// 后续进程，需要将资源限制的设置构造的shell中。
// 确实，Bourne shell、bash和Kom shell内置了ulimit命令，C shell
// 内置了limit命令。

// 可以用setrlimit()函数更改资源限制，必须遵循下面3条原则：
// 1).任何一个进程都可将一个软限制值更改为小于或等于其硬限制值。
// 2).任何一个进程都可降低其硬限制值，但它必须大于或等于其软限制值。
// 这种降低，对普通用户而言是不可逆的。
// 3).只有超级用户进程可以提高硬限制值。
