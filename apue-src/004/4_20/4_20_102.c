#include "apue.h"
#include <fcntl.h>

int
main(int argc, char *argv[])
{
	int				i, fd;
	struct stat		statbuf;
	struct timespec	times[2];
	// 接受多个参数，每个参数都是文件，本程序可以将这些文件清0,
	// 但是并不改变文件的最后访问时间与修改时间。
	for (i = 1; i < argc; i++) {
		// 取得文件的详细信息，信息中包含了3种时间。
		if (stat(argv[i], &statbuf) < 0) {		/* fetch current times */
			err_ret("%s: stat error", argv[i]);
			continue;
		}
		// 以读写方式打开文件，O_TRUNC将文件截断为0，即将文件内容清空。
		if ((fd = open(argv[i], O_RDWR | O_TRUNC)) < 0) {	/* truncate */
			err_ret("%s: open error", argv[i]);
			continue;
		}
		// 保存stat取得的文件时间。
		times[0] = statbuf.st_atim;		// 文件数据的最后访问时间
		times[1] = statbuf.st_mtim;		// 文件数据的最后修改时间
		// 文件被截断后，其最后访问和修改时间必然发生变化，
		// 这里使用futimens函数将这两个时间重置为原时间。
		if (futimens(fd, times) < 0)	/* reset times */
			err_ret("%s: futimens error", argv[i]);
		// 使用文件描述符关闭文件。
		close(fd);
	}
	// 正常退出进程。
	exit (0);
}

// 点评
// 本例重点演示futimens函数的用法，该函数可以修改文件的最后访问时间和
// 最后修改时间等。

// 扩展
// ls -l <file>   #查看文件长度和最后修改时间
// ls -lu <file>  #查看最后访问时间
// ls -lc <file>  #查看文件i节点状态的最后更改时间
