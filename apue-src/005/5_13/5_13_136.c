#include "apue.h"
#include <errno.h>

// 全局函数声明
void make_temp(char *template);

int
main()
{
	// good_template使用数组来存放临时文件路径名，数组是在栈上分配的，
	// 其内容可以被读写。
	char		good_template[] = "/tmp/dirXXXXXX";		/* right way */
	// bad_template使用指针来存放临时文件路径名，指针本身是在栈上分配的，
	// 但是编译器把字面字符串放在只读区域，这意味着bad_template指向
	// 的内容不能被更改，否则会出现段错误。
	char		*bad_template = "/tmp/dirXXXXXX";		/* wrong way */

	// 通知并真正去创建第一个临时文件
	printf("trying to create first temp file...\n");
	make_temp(good_template);
	// 通知并真正去创建第二个临时文件
	printf("trying to create second temp file...\n");
	make_temp(bad_template);
	// 正常退出
	exit(0);
}

// 创建临时文件
void
make_temp(char *template)
{
	int			fd;
	struct stat	sbuf;

	// mkstemp()函数利用我们传进去的文件路径，对其文件名部分加以修改生成新的随机的文件名，
	// 然后使用它创建一个文件，并返回这个文件的描述符。
	// 注意：mkstemp()创建文件后，并不会unlink()，因此ls -la可以看到它，这个文件也不会被自动删除，
	// 除非用户手动unlink()。
	if ((fd = mkstemp(template)) < 0)
		err_sys("can't create temp file");
	// 打印出mkstemp()创建的临时文件的文件名，如：/tmp/dirxEXpqM，可见该函数把/tmp/dirXXXXXX
	// 这个路径dir后面的XXXXXX部分替换为随机文件名了。
	printf("temp name = %s\n", template);
	// 显示关闭该文件，如果之前unlink()过该文件，那么当文件被关闭后，该文件会被内核立即清除并释放掉。
	// 如果没有unlink()过，则该文件会继续存在在硬盘上。
	close(fd);
	// 获取该临时文件的详细属性信息。
	if (stat(template, &sbuf) < 0) {
		// 如果ENOENT被设置，说明该文件不存在，否则说明stat()函数出错。
		if (errno == ENOENT)
			printf("file doesn't exist\n");
		else
			err_sys("stat failed");
	} else {
		// 说明该文件存在
		printf("file exists\n");
		// 我们可以在这里选择是否手动unlink()掉该文件。
		//unlink(template);
	}
}

// 点评
// 实验中可以发现，创建第一个临时文件的全过程是顺利的，而且被创建的临时文件在程序退出后仍然存在，
// 这是因为mkstemp()函数并不会unlink()它创建的临时文件。
// 但是创建第二个临时文件时出现了段错误，导致core dump(核心转储)，这是因为指针指向了字面量字符串
// 所在的只读区域，当mkstemp()函数试图修改指针所指向的只读区域时就会出现段错误。

// mkdtemp()和mkstemp()两个函数是XSI的扩展部分，mkdtemp()函数可以生成一个唯一的随机路径名，
// mkstemp()函数会使用这个路径名继续创建一个随机的文件名，然后真正把这个文件创建出来。

// 使用tmpnam()有一个缺点，当tmpnam()创建一个文件路径名和fopen()使用这个文件路径名创建文件
// 之间会有一个时间窗口，在这个窗口期内其他进程可能会使用相同的文件名来创建文件。
// 而tempfile()和mkstemp()不会存在这个问题。
