#include "apue.h"
#include <sys/socket.h>

// 休眠阀值，最大延迟时间2分钟左右。
#define MAXSLEEP	128

// Socket重连接实现，使用了指数补偿算法(exponential backoff)。
int
connect_retry(int sockfd, const struct sockaddr *addr, socklen_t alen)
{
	int		numsec;

	/*
	 * Try to connect with exponential backoff.
	 */
	// numsec左移1位相当于乘以2，即扩大1倍。
	for (numsec = 1; numsec <= MAXSLEEP; numsec <<= 1) {
		if (connect(sockfd, addr, alen) == 0) {
			/*
			 * Connection accepted.
			 */
			// 表示连接成功，该连接已被服务端接受。
			return (0);
		}

		/*
		 * Delay befor trying again.
		 */
		// 休眠一定秒数然后重试。
		if (numsec <= MAXSLEEP/2)
			sleep(numsec);
	}
	// 跳出循环意味着超过休眠阀值，最终连接失败。
	return (-1);
}

// 点评：
// 这个方法移植性差，在Linux和Solaris上正常工作，
// 但是在FreeBSD和MacOS X上却不能按预期工作。因
// 为后两者在connect失败时必须先关闭socket，然后
// 再重新调用，否则第一次connect失败，后面连续调用
// connect都会失败。
