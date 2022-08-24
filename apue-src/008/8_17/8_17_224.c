#include "apue.h"
#include <sys/times.h>

// 全局静态函数声明
static void pr_times(clock_t, struct tms *, struct tms *);
static void do_cmd(char *);

int
main(int argc, char *argv[])
{
	int		i;

	// 设置标准输出为无缓冲模式
	setbuf(stdout, NULL);
	// 本程序接受多个参数，每个参数都指定为一个可运行的命令或程序路径。
	for (i = 1; i < argc; i++)
		// 根据每个参数指定的命令或程序路径，循环来运行它们。
		do_cmd(argv[i]);	/* once for each command-line arg */
	// 正常退出，冲洗标准IO流（对标准输出无影响，它已被设置为无缓冲）。
	exit(0);
}

// 该函数根据命令名称或程序文件路径运行该命令或程序
static void
do_cmd(char *cmd)	/* execute and time the "cmd" */
{
	// 保存进程时间（用户CPU时间和系统CPU时间）
	struct tms		tmsstart, tmsend;
	// 保存墙上时钟时间
	clock_t			start, end;
	// 保存子进程返回状态
	int				status;

	// 打印命令名称或程序路径名
	printf("\ncommand: %s\n", cmd);

	// times()函数会计算出一个进程和其已终止子进程的用户CPU时间和系统CPU时间，
	// 然后将这些时间填入tms结构体。
	// times()还可以计算出墙上时钟时间，把它作为返回值返回。
	// 这里先取得墙上时钟时间、用户CPU时间和系统CPU时间的开始值。
	if ((start = times(&tmsstart)) == -1)		/* starting values */
		err_sys("times error");

	// 调用system()函数，fork子进程，然后让子进程执行命令或程序，并等待子进程
	// 执行完毕退出后，取得其退出状态。
	if ((status = system(cmd)) < 0)				/* execute command */
		err_sys("system() error");

	// 取得墙上时钟时间、用户CPU时间和系统CPU时间的结束值。
	if ((end = times(&tmsend)) == -1)			/* ending values */
		err_sys("times error");

	// 根据墙上时钟时间、用户CPU时间和系统CPU时间的开始值与结束值，计算出相对时间，
	// 然后将这些时间打印出来。
	pr_times(end-start, &tmsstart, &tmsend);
	// 打印出子进程的详细退出信息。
	pr_exit(status);
}

// 该函数格式化打印出墙上时钟时间的相对值、父子进程的用户CPU时间和系统CPU时间的开始值
// 与父子进程的用户CPU时间和系统CPU时间的结束值。
static void
pr_times(clock_t real, struct tms *tmsstart, struct tms *tmsend)
{
	// 墙上时钟时间使用的每秒时钟滴答数
	static long			clktck = 0;

	// 取得每秒时钟滴答数
	if (clktck == 0)	/* fetch clock ticks per second first time */
		// 这里实际上是调用sysconf函数来取得每秒时钟滴答数的。
		if ((clktck = sysconf(_SC_CLK_TCK)) < 0)
			err_sys("sysconf error");

	// times()函数的返回值为墙上时钟时间，但这个时间单位是时钟滴答数。
	// 两次调用times()函数，计算出相对的墙上时钟时间的总的时钟滴答数real，
	// 然后用其除以每秒时钟滴答数，这样就把单位换算成了秒。
	printf("	real:	%7.2f\n", real / (double) clktck);
	// 打印出父进程的用户CPU时间，同样利用相对的CPU时间除以每秒时钟滴答数，
	// 将其单位换算成秒。
	printf("	user:	%7.2f\n",
		(tmsend->tms_utime - tmsstart->tms_utime) / (double) clktck);
	// 打印出父进程的系统CPU时间
	printf("	sys:	%7.2f\n",
		(tmsend->tms_stime - tmsstart->tms_stime) / (double) clktck);	
	// 打印出子进程的用户CPU时间
	printf("	child user:	%7.2f\n",
		(tmsend->tms_cutime - tmsstart->tms_cutime) / (double) clktck);
	// 打印出子进程的系统CPU时间
	printf("	child sys:	%7.2f\n",
		(tmsend->tms_cstime - tmsstart->tms_cstime) / (double) clktck);	
}

// 点评
// 本例详细演示了如何对一个进程及其已终止子进程的三种时间进行获取，这三种时间为：
// 墙上时钟时间（进程实际运行时间）、用户CPU时间（用户代码耗费的CPU时间）
// 和系统CPU时间（内核代码耗费的CPU时间）。
// 我们需要理解的是times()函数的用法，这在上面代码注释中已详细说明。

// 注意
// times()函数计算出的三种时间，都是从格林威治时间至今的时钟滴答数，我们需要两次
// 调用times()，然后计算出这三种时间的相对值，注意这些相对值的单位仍为时钟滴答数，
// 最后要将时钟滴答数除以每秒时钟滴答数，将其转换为秒。
// 经过上述操作，这三种时间才能代表父子进程实际从运行到结束所经历的时间。
