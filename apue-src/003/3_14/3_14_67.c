#include "apue.h"
#include <fcntl.h>

int
main(int argc, char *argv[])
{
	int		val;

	// 只接受一个文件描述符参数。
	if (argc != 2)
		err_quit("usage: a.out <descriptor#>");

	// F_GETFL为获取文件状态标志，此时第三个参数没有用。
	if ((val = fcntl(atoi(argv[1]), F_GETFL, 0)) < 0)
		err_sys("fcntl error for fd %d", atoi(argv[1]));

	// 5个访问方式标志(O_RDONLY,O_WRONLY,O_RDWR,O_EXEC和O_SEARCH)
	// 并不各占1位，前3个分别是0,1,2，这5个值互斥，一个文件的访问方式只能
	// 取这5个值之一。因此必须先用屏蔽字O_ACCMODE取得访问方式位，然后将结
	// 果与这5个值逐一比较。
	switch (val & O_ACCMODE) {
	case O_RDONLY:
		printf("read only");
		break;

	case O_WRONLY:
		printf("write only");
		break;

	case O_RDWR:
		printf("read write");
		break;

	default:
		err_dump("unknown access mode");
	}
	// 追加写。
	if (val & O_APPEND)
		printf(", append");
	// 非阻塞模式。
	if (val & O_NONBLOCK)
		printf(", nonblocking");
	// 等待写完成(数据和属性)。
	if (val & O_SYNC)
		printf(", synchronous writes");

#if !defined(_POSIX_C_SOURCE) && defined(O_FSYNC) && (O_FSYNC != O_SYNC)
	// 等待写完成(进FreeBSD和Mac OS X)
	if (val & O_FSYNC)
		printf(", synchronous writes");
#endif

	// 输出一个换行符。
	putchar('\n');
	exit(0);
}

// 点评
// 本例使用了功能测试宏_POSIX_C_SOURCE，并且条件编译了POSIX.1中没有
// 定义的文件访问标志。
// 下面显示了bash(Bourne-again shell)调用该程序时的几种情况，当使用
// 不同shell时结果可能会有所不同。
// $./3_14_67 0 < /dev/tty
// read only
// $./3_14_67 1 > temp.foo
// $cat temp.foo
// write only
// $./3_14_67 2 2>>temp.foo
// write only, append
// $./3_14_67 5 5<>temp.foo
// read write
// 5<>temp.foo表示在文件描述符5上打开文件以供读写。
