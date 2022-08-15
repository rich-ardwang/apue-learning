#include "apue.h"
#include <fcntl.h>

int
main(void)
{
	// 以读写方式打开文件tempfile。
	if (open("tempfile", O_RDWR) < 0)
		// 文件不存在会打开失败。
		err_sys("open error");
	// 解除tempfile文件的所有链接，即删除该文件。
	if (unlink("tempfile") < 0)
		err_sys("unlink error");
	// 告知文件已经解除链接，此时使用ls等命令已经看不到该文件了，
	// 但是由于该文件处于打开状态，内核并不会真正删除它，那么它所占用的空间实际上并未被释放。
	printf("file unlinked\n");
	// 延迟15秒后关闭。
	sleep(15);
	printf("done\n");
	exit(0);
}

// 点评
// 本例演示unlink函数的用法。
// 我们准备一个大一点的文件，将这个文件命名为tempfile。
// 程序会以读写方式打开它，然后使用unlink函数立即解除这个文件的所有链接。
// 之后程序睡眠15秒，在这15秒期间，我们使用ls已经看不到tempfile文件了，
// 但实际上内核还没有删除它，我们使用du /xxx命令可以看到它所占空间无变化。
// 15秒过后，程序关闭，tempfile文件的打开状态被清除，此时内核才会真正删除它，
// 我们再次使用du /xxx命令查看，发现空间已被释放。

// 扩展
// root可以使用unlink对一个目录进行解除链接，即删除目录，但是删除目录最好还是用rmdir函数。
// 删除文件除了使用unlink外，还可以使用remove函数，效果相同。

// 注意
// 如果pathname是符号链接，那么unlink删除符号链接本身，而不是它所指向的文件。
// 目前没有一个函数能直接删除符号链接所指向的文件。
