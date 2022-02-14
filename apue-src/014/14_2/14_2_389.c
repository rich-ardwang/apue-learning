#include "apue.h"
#include <errno.h>
#include <fcntl.h>

char		buf[5000000];

int
main(void)
{
	int		ntowrite, nwrite;
	char	*ptr;

	// 从stdin读入500万字节。
	ntowrite = read(STDIN_FILENO, buf, sizeof(buf));
	fprintf(stderr, "read %d bytes\n", ntowrite);

	// 设置stdout文件的IO模式为非阻塞。
	set_fl(STDOUT_FILENO, O_NONBLOCK);		/* set nonblocking */

	// ptr指针指向buf数组首个元素的地址。
	ptr = buf;
	// 如果未到达文件末尾，则持续循环。
	while (ntowrite > 0) {
		errno = 0;
		// 从缓冲区buf的第一个元素开始写入，返回实际写入的字节数nwrite。
		nwrite = write(STDOUT_FILENO, ptr, ntowrite);
		fprintf(stderr, "nwrite = %d, errno = %d\n", nwrite, errno);

		if (nwrite > 0) {
			// 写操作指针移动到nwrite位置。
			ptr += nwrite;
			// 要求写入字节数减去已经写入的字节数。
			ntowrite -= nwrite;
		}
	}

	// 移除stdout文件的IO模式，恢复默认。
	clr_fl(STDOUT_FILENO, O_NONBLOCK);		/* clear nonblocking */

	exit(0);
}

// 测试命令
// # 从stdin读取/etc/servies文件，然后写入文件temp.file。
// ./14_2_389 < /etc/services > temp.file
// # 从stdin读取/etc/services文件，然后输出到stdout，并将stderr写入文件stderr.out。
// ./14_2_389 < /etc/services 2>stderr.out
