#include "apue.h"

// 从所给文件描述符读取n个字节。
ssize_t			/* Read "n" bytes from a descriptor */
readn(int fd, void *ptr, size_t n)
{
	size_t		nleft;
	ssize_t		nread;

	nleft = n;
	while (nleft > 0) {
		// 系统调用read。
		if ((nread = read(fd, ptr, nleft)) < 0) {
			// 如果nread<0，说明read出错。
			if (nleft == n)
				// 不支持读取负数字节。
				return (-1);		/* error, return -1 */
			else
				// 出错跳出循环。
				break;		/* error, return amount read so far */
		} else if (nread == 0) {
			// nread==0，说明read工作正常，但是读到了文件尾。
			break;		/* EOF */
		}
		// 如果nread>0，说明read工作正常，文件非空。
		nleft -= nread;
		ptr += nread;
	}
	// (read出错且n>=0) 或 (文件为空)，返回0。
	// 文件非空，返回>0。
	return (n - nleft);		/* return >= 0 */
}

// 向所给文件描述符写入n个字节。
ssize_t			/* Write "n" bytes to a descriptor */
writen(int fd, const void *ptr, size_t n)
{
	size_t		nleft;
	ssize_t		nwritten;

	nleft = n;
	while (nleft > 0) {
		// 系统调用write。
		if ((nwritten = write(fd, ptr, nleft)) < 0) {
			// 如果nwritten<0，说明write出错。
			if (nleft == n)
				// 不支持写入负数字节。
				return (-1);	/* error, return -1 */
			else
				// 出错跳出循环。
				break;			/* error, return amount written so far */
		} else if (nwritten == 0) {
			// nwritten==0，说明write工作正常，但是没有内容可以再写。
			break;
		}
		// 如果nwritten==0，说明write工作正常，一次没有写完。
		nleft -= nwritten;
		ptr += nwritten;
	}
	// (write出错且n>=0) 或 (写入0个字节)，返回0。
	// 正常写n>0个字节，返回>0。
	return (n - nleft);		/* return >= 0 */
}
