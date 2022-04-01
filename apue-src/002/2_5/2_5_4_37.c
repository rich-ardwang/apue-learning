#include "apue.h"
#include <errno.h>
#include <limits.h>

// 声明两个静态函数，用于实现打印sysconf和pathconf的返回结果。
static void pr_sysconf(char *, int);
static void pr_pathconf(char *, char *, int);

int
main(int argc, char *argv[])
{
	// 必须提供一个路径参数，在后面pathconf调用时会用到。
	if (argc != 2)
		err_quit("usage: a.out <dirname>");

// exec函数的参数最大长度(字节)。
#ifdef ARG_MAX
	printf("ARG_MAX defined to be %ld\n", (long)ARG_MAX+0);
#else
	printf("no symbol for ARG_MAX\n");
#endif
#ifdef _SC_ARG_MAX
	pr_sysconf("ARG_MAX =", _SC_ARG_MAX);
#else
	printf("no symbol for _SC_ARG_MAX\n");
#endif
// 可用atexit函数登记的最大函数个数。
#ifdef ATEXIT_MAX
	printf("ATEXIT_MAX defined to be %ld\n", (long)ATEXIT_MAX+0);
#else
	printf("no symbol for ATEXIT_MAX\n");
#endif
#ifdef _SC_ATEXIT_MAX
	pr_sysconf("ATEXIT_MAX =", _SC_ATEXIT_MAX);
#else
	printf("no symbol for _SC_ATEXIT_MAX\n");
#endif
// 
#ifdef CHARCLASS_NAME_MAX
	printf("CHARCLASS_NAME_MAX defined to be %ld\n", (long)CHARCLASS_NAME_MAX+0);
#else
	printf("no symbol for CHARCLASS_NAME_MAX\n");
#endif
#ifdef _SC_CHARCLASS_NAME_MAX
	pr_sysconf("CHARCLASS_NAME_MAX =", _SC_CHARCLASS_NAME_MAX);
#else
	printf("no symbol for _SC_CHARCLASS_NAME_MAX\n");
#endif
// 每个实际用户ID的最大进程数。
#ifdef CHILD_MAX
	printf("CHILD_MAX defined to be %ld\n", (long)CHILD_MAX+0);
#else
	printf("no symbol for CHILD_MAX\n");
#endif
#ifdef _SC_CHILD_MAX
	pr_sysconf("CHILD_MAX =", _SC_CHILD_MAX);
#else
	printf("no symbol for _SC_CHILD_MAX\n");
#endif
// 每秒时钟滴答数。
#ifdef CLOCKTICKSPERSECOND /*clock ticks/second*/
	printf("CLOCKTICKSPERSECOND /*clock ticks/second*/ defined to be %ld\n", (long)CLOCKTICKSPERSECOND /*clock ticks/second*/+0);
#else
	printf("no symbol for CLOCKTICKSPERSECOND /*clock ticks/second*/\n");
#endif
#ifdef _SC_CLK_TCK
	pr_sysconf("CLOCKTICKSPERSECOND /*clock ticks/second*/ =", _SC_CLK_TCK);
#else
	printf("no symbol for _SC_CLK_TCK\n");
#endif
// 在本地定义文件中可以赋予LC_COLLATE顺序关键字项的最大权重数。
#ifdef COLL_WEIGHTS_MAX
	printf("COLL_WEIGHTS_MAX defined to be %ld\n", (long)COLL_WEIGHTS_MAX+0);
#else
	printf("no symbol for COLL_WEIGHTS_MAX\n");
#endif
#ifdef _SC_COLL_WEIGHTS_MAX
	pr_sysconf("COLL_WEIGHTS_MAX =", _SC_COLL_WEIGHTS_MAX);
#else
	printf("no symbol for _SC_COLL_WEIGHTS_MAX\n");
#endif
// 定时器最大超限运行次数。
#ifdef DELAYTIMER_MAX
	printf("DELAYTIMER_MAX defined to be %ld\n", (long)DELAYTIMER_MAX+0);
#else
	printf("no symbol for DELAYTIMER_MAX\n");
#endif
#ifdef _SC_DELAYTIMER_MAX
	pr_sysconf("DELAYTIMER_MAX =", _SC_DELAYTIMER_MAX);
#else
	printf("no symbol for _SC_DELAYTIMER_MAX\n");
#endif
// gethostname函数返回的主机名最大长度。
#ifdef HOST_NAME_MAX
	printf("HOST_NAME_MAX defined to be %ld\n", (long)HOST_NAME_MAX+0);
#else
	printf("no symbol for HOST_NAME_MAX\n");
#endif
#ifdef _SC_HOST_NAME_MAX
	pr_sysconf("HOST_NAME_MAX =", _SC_HOST_NAME_MAX);
#else
	printf("no symbol for _SC_HOST_NAME_MAX\n");
#endif
// readv或writev函数可以使用最多的iovec结构的个数。
#ifdef IOV_MAX
	printf("IOV_MAX defined to be %ld\n", (long)IOV_MAX+0);
#else
	printf("no symbol for IOV_MAX\n");
#endif
#ifdef _SC_IOV_MAX
	pr_sysconf("IOV_MAX =", _SC_IOV_MAX);
#else
	printf("no symbol for _SC_IOV_MAX\n");
#endif
// 实用程序输入行的最大长度。
#ifdef LINE_MAX
	printf("LINE_MAX defined to be %ld\n", (long)LINE_MAX+0);
#else
	printf("no symbol for LINE_MAX\n");
#endif
#ifdef _SC_LINE_MAX
	pr_sysconf("LINE_MAX =", _SC_LINE_MAX);
#else
	printf("no symbol for _SC_LINE_MAX\n");
#endif
// 登录名的最大长度。
#ifdef LOGIN_NAME_MAX
	printf("LOGIN_NAME_MAX defined to be %ld\n", (long)LOGIN_NAME_MAX+0);
#else
	printf("no symbol for LOGIN_NAME_MAX\n");
#endif
#ifdef _SC_LOGIN_NAME_MAX
	pr_sysconf("LOGIN_NAME_MAX =", _SC_LOGIN_NAME_MAX);
#else
	printf("no symbol for _SC_LOGIN_NAME_MAX\n");
#endif
// 每个进程同时添加的最大进程组ID数。
#ifdef NGROUPS_MAX
	printf("NGROUPS_MAX defined to be %ld\n", (long)NGROUPS_MAX+0);
#else
	printf("no symbol for NGROUPS_MAX\n");
#endif
#ifdef _SC_NGROUPS_MAX
	pr_sysconf("NGROUPS_MAX =", _SC_NGROUPS_MAX);
#else
	printf("no symbol for _SC_NGROUPS_MAX\n");
#endif
// 每个进程最大打开文件数。
#ifdef OPEN_MAX
	printf("OPEN_MAX defined to be %ld\n", (long)OPEN_MAX+0);
#else
	printf("no symbol for OPEN_MAX\n");
#endif
#ifdef _SC_OPEN_MAX
	pr_sysconf("OPEN_MAX =", _SC_OPEN_MAX);
#else
	printf("no symbol for _SC_OPEN_MAX\n");
#endif
// 系统存储页长度(字节数)。
#ifdef PAGESIZE
	printf("PAGESIZE defined to be %ld\n", (long)PAGESIZE+0);
#else
	printf("no symbol for PAGESIZE\n");
#endif
#ifdef _SC_PAGESIZE
	pr_sysconf("PAGESIZE =", _SC_PAGESIZE);
#else
	printf("no symbol for _SC_PAGESIZE\n");
#endif
// 系统存储页长度(字节数)。
#ifdef PAGE_SIZE
	printf("PAGE_SIZE defined to be %ld\n", (long)PAGE_SIZE+0);
#else
	printf("no symbol for PAGE_SIZE\n");
#endif
#ifdef _SC_PAGE_SIZE
	pr_sysconf("PAGE_SIZE =", _SC_PAGE_SIZE);
#else
	printf("no symbol for _SC_PAGE_SIZE\n");
#endif
// 当使用间隔表示法\{m,n\}时，函数regexec和regcomp允许的基本正则表达式
// 重复发生次数。
#ifdef RE_DUP_MAX
	printf("RE_DUP_MAX defined to be %ld\n", (long)RE_DUP_MAX+0);
#else
	printf("no symbol for RE_DUP_MAX\n");
#endif
#ifdef _SC_RE_DUP_MAX
	pr_sysconf("RE_DUP_MAX =", _SC_RE_DUP_MAX);
#else
	printf("no symbol for _SC_RE_DUP_MAX\n");
#endif
// 为应用程序预留的实时信号的最大个数。
#ifdef RTSIG_MAX
	printf("RTSIG_MAX defined to be %ld\n", (long)RTSIG_MAX+0);
#else
	printf("no symbol for RTSIG_MAX\n");
#endif
#ifdef _SC_RTSIG_MAX
	pr_sysconf("RTSIG_MAX =", _SC_RTSIG_MAX);
#else
	printf("no symbol for _SC_RTSIG_MAX\n");
#endif
// 一个进程可使用的信号量最大个数。
#ifdef SEM_NSEMS_MAX
	printf("SEM_NSEMS_MAX defined to be %ld\n", (long)SEM_NSEMS_MAX+0);
#else
	printf("no symbol for SEM_NSEMS_MAX\n");
#endif
#ifdef _SC_SEM_NSEMS_MAX
	pr_sysconf("SEM_NSEMS_MAX =", _SC_SEM_NSEMS_MAX);
#else
	printf("no symbol for _SC_SEM_NSEMS_MAX\n");
#endif
// 信号量的最大值。
#ifdef SEM_VALUE_MAX
	printf("SEM_VALUE_MAX defined to be %ld\n", (long)SEM_VALUE_MAX+0);
#else
	printf("no symbol for SEM_VALUE_MAX\n");
#endif
#ifdef _SC_SEM_VALUE_MAX
	pr_sysconf("SEM_VALUE_MAX =", _SC_SEM_VALUE_MAX);
#else
	printf("no symbol for _SC_SEM_VALUE_MAX\n");
#endif
// 一个进程可排队信号的最大个数。
#ifdef SIGQUEUE_MAX
	printf("SIGQUEUE_MAX defined to be %ld\n", (long)SIGQUEUE_MAX+0);
#else
	printf("no symbol for SIGQUEUE_MAX\n");
#endif
#ifdef _SC_SIGQUEUE_MAX
	pr_sysconf("SIGQUEUE_MAX =", _SC_SIGQUEUE_MAX);
#else
	printf("no symbol for _SC_SIGQUEUE_MAX\n");
#endif
// 一个_SC_STREAM_MAX进程在任意给定时刻标准I/O流的最大个数，
// 如果定义，必须与FOPEN_MAX有相同值。
#ifdef STREAM_MAX
	printf("STREAM_MAX defined to be %ld\n", (long)STREAM_MAX+0);
#else
	printf("no symbol for STREAM_MAX\n");
#endif
#ifdef _SC_STREAM_MAX
	pr_sysconf("STREAM_MAX =", _SC_STREAM_MAX);
#else
	printf("no symbol for _SC_STREAM_MAX\n");
#endif
// 在解析路径时，可遍历的符号链接数。
#ifdef SYMLOOP_MAX
	printf("SYMLOOP_MAX defined to be %ld\n", (long)SYMLOOP_MAX+0);
#else
	printf("no symbol for SYMLOOP_MAX\n");
#endif
#ifdef _SC_SYMLOOP_MAX
	pr_sysconf("SYMLOOP_MAX =", _SC_SYMLOOP_MAX);
#else
	printf("no symbol for _SC_SYMLOOP_MAX\n");
#endif
// 每个进程的最大定时器个数。
#ifdef TIMER_MAX
	printf("TIMER_MAX defined to be %ld\n", (long)TIMER_MAX+0);
#else
	printf("no symbol for TIMER_MAX\n");
#endif
#ifdef _SC_TIMER_MAX
	pr_sysconf("TIMER_MAX =", _SC_TIMER_MAX);
#else
	printf("no symbol for _SC_TIMER_MAX\n");
#endif
// 终端设备名长度，包括终止null字节。
#ifdef TTY_NAME_MAX
	printf("TTY_NAME_MAX defined to be %ld\n", (long)TTY_NAME_MAX+0);
#else
	printf("no symbol for TTY_NAME_MAX\n");
#endif
#ifdef _SC_TTY_NAME_MAX
	pr_sysconf("TTY_NAME_MAX =", _SC_TTY_NAME_MAX);
#else
	printf("no symbol for _SC_TTY_NAME_MAX\n");
#endif
// 时区名中的最大字节数。
#ifdef TZNAME_MAX
	printf("TZNAME_MAX defined to be %ld\n", (long)TZNAME_MAX+0);
#else
	printf("no symbol for TZNAME_MAX\n");
#endif
#ifdef _SC_TZNAME_MAX
	pr_sysconf("TZNAME_MAX =", _SC_TZNAME_MAX);
#else
	printf("no symbol for _SC_TZNAME_MAX\n");
#endif
// 以带符号整型值表示在指定目录中允许的普通文件最大长度所需的最小位(bit)数。
#ifdef FILESIZEBITS
	printf("FILESIZEBITS defined to be %ld\n", (long)FILESIZEBITS+0);
#else
	printf("no symbol for FILESIZEBITS\n");
#endif
#ifdef _PC_FILESIZEBITS
	pr_pathconf("FILESIZEBITS =", argv[1], _PC_FILESIZEBITS);
#else
	printf("no symbol for _PC_FILESIZEBITS\n");
#endif
// 文件链接计数的最大值。
#ifdef LINK_MAX
	printf("LINK_MAX defined to be %ld\n", (long)LINK_MAX+0);
#else
	printf("no symbol for LINK_MAX\n");
#endif
#ifdef _PC_LINK_MAX
	pr_pathconf("LINK_MAX =", argv[1], _PC_LINK_MAX);
#else
	printf("no symbol for _PC_LINK_MAX\n");
#endif
// 终端规范输入队列的最大字节数。
#ifdef MAX_CANON
	printf("MAX_CANON defined to be %ld\n", (long)MAX_CANON+0);
#else
	printf("no symbol for MAX_CANON\n");
#endif
#ifdef _PC_MAX_CANON
	pr_pathconf("MAX_CANON =", argv[1], _PC_MAX_CANON);
#else
	printf("no symbol for _PC_MAX_CANON\n");
#endif
// 终端输入队列可用空间的字节数。
#ifdef MAX_INPUT
	printf("MAX_INPUT defined to be %ld\n", (long)MAX_INPUT+0);
#else
	printf("no symbol for MAX_INPUT\n");
#endif
#ifdef _PC_MAX_INPUT
	pr_pathconf("MAX_INPUT =", argv[1], _PC_MAX_INPUT);
#else
	printf("no symbol for _PC_MAX_INPUT\n");
#endif
// 文件名的最大字节数(不包括终止null字节)。
#ifdef NAME_MAX
	printf("NAME_MAX defined to be %ld\n", (long)NAME_MAX+0);
#else
	printf("no symbol for NAME_MAX\n");
#endif
#ifdef _PC_NAME_MAX
	pr_pathconf("NAME_MAX =", argv[1], _PC_NAME_MAX);
#else
	printf("no symbol for _PC_NAME_MAX\n");
#endif
// 相对路径名的最大字节数，包括终止null字节。
#ifdef PATH_MAX
	printf("PATH_MAX defined to be %ld\n", (long)PATH_MAX+0);
#else
	printf("no symbol for PATH_MAX\n");
#endif
#ifdef _PC_PATH_MAX
	pr_pathconf("PATH_MAX =", argv[1], _PC_PATH_MAX);
#else
	printf("no symbol for _PC_PATH_MAX\n");
#endif
// 能原子地写到管道的最大字节数。
#ifdef PIPE_BUF
	printf("PIPE_BUF defined to be %ld\n", (long)PIPE_BUF+0);
#else
	printf("no symbol for PIPE_BUF\n");
#endif
#ifdef _PC_PIPE_BUF
	pr_pathconf("PIPE_BUF =", argv[1], _PC_PIPE_BUF);
#else
	printf("no symbol for _PC_PIPE_BUF\n");
#endif
// 符号链接的字节数。
#ifdef SYMLINK_MAX
	printf("SYMLINK_MAX defined to be %ld\n", (long)SYMLINK_MAX+0);
#else
	printf("no symbol for SYMLINK_MAX\n");
#endif
#ifdef _PC_SYMLINK_MAX
	pr_pathconf("SYMLINK_MAX =", argv[1], _PC_SYMLINK_MAX);
#else
	printf("no symbol for _PC_SYMLINK_MAX\n");
#endif
// 文件时间戳的纳秒精度。
#ifdef _POSIX_TIMESTAMP_RESOLUTION
	printf("_POSIX_TIMESTAMP_RESOLUTION defined to be %ld\n", (long)_POSIX_TIMESTAMP_RESOLUTION+0);
#else
	printf("no symbol for _POSIX_TIMESTAMP_RESOLUTION\n");
#endif
#ifdef _PC_TIMESTAMP_RESOLUTION
	pr_pathconf("_POSIX_TIMESTAMP_RESOLUTION =", argv[1], _PC_TIMESTAMP_RESOLUTION);
#else
	printf("no symbol for _PC_TIMESTAMP_RESOLUTION\n");
#endif
	exit(0);
}

// 打印输出sysconf调用结果。
static void
pr_sysconf(char *mesg, int name)
{
	long val;

	// 先输出包含目标符号的部分字符串。
	fputs(mesg, stdout);
	errno = 0;
	// 调用sysconf获取目标符号被定义的值。
	// 有的符号值不会改变，一般在编译时可以确定，有的符号值需要在运行时确定，
	// 这就需要调用sysconf来动态获取。
	if ((val = sysconf(name)) < 0) {
		// sysconf返回值小于0时需要检验errno的值。
		if (errno != 0) {
			// 如果errno非0，且被设置为EINVAL，说明系统不支持该符号。
			if (errno == EINVAL)
				fputs(" ( not supported)\n", stdout);
			// 如果errno非0，且不为EINVAL，说明sysconf调用出错。
			else
				err_sys("sysconf error");
		} else {
			// 如果errno为0，也就是它没有被重置过，说明符号值无限制。
			fputs(" (no limit)\n", stdout);
		}
	 } else {
		 // 如果sysconf返回值大于等于0，说明它正常获取到符号值。
		printf(" %ld\n", val);
	}
}

// 打印输出pathconf调用结果。
static void
pr_pathconf(char *mesg, char *path, int name)
{
	long val;

	// 先输出包含目标符号的部分字符串。
	fputs(mesg, stdout);
	errno = 0;
	// 调用pathconf获取目标符号被定义的值。
	// 有的符号值不会改变，一般在编译时可以确定，有的符号值需要在运行时确定，
	// 这就需要调用pathconf来动态获取。
	if ((val = pathconf(path, name)) < 0) {
		// pathconf返回值小于0时需要检验errno的值。
		if (errno != 0) {
			// 如果errno非0，且被设置为EINVAL，说明系统不支持该符号。
			if (errno == EINVAL)
				fputs(" (not supported)\n", stdout);
			// 如果errno非0，且不为EINVAL，说明pathconf调用出错。
			else
				err_sys("pathconf error, path = %s", path);
		} else {
			// 如果errno为0，也就是它没有被重置过，说明符号值无限制。
			fputs(" (no limit)\n", stdout);
		}
	} else {
		// 如果pathconf返回值大于等于0，说明它正常获取到符号值。
		printf(" %ld\n", val);
	}
}

// 注意
// 该程序代码由2_5_4_35.sh，pathconf.sym和sysconf.sym三个文件自动
// 生成，使用了linux三剑客之awk技术，两个sym文件包含了制表符分隔的限制
// 名和符号列表。
// 并非每种平台都定义所有符号，所以每个pathconf和sysconf调用，awk程序
// 都使用了必要的#ifdef语句。
