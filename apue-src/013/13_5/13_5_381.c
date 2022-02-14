#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>

// 指定被加文件锁的文件的目录和名称，一般守护进程的加锁文件都放在/var/run目录下。
#define LOCKFILE	"/var/run/daemon.pid"
// 指定文件权限模式。
#define LOCKMODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

// 引入外部方法：锁定文件的函数。
extern int lockfile(int);

// 使用对单一文件全局加锁的方式，保证守护进程运行在单实例模式。
int
already_running(void)
{
	int		fd;
	char	buf[16];

	// 使用open系统调用打开目标文件。
	fd = open(LOCKFILE, O_RDWR|O_CREAT, LOCKMODE);
	if (fd < 0)	{
		syslog(LOG_ERR, "can't open %s: %s", LOCKFILE, strerror(errno));
		exit(1);
	}
	// 对目标文件加全局锁。
	if (lockfile(fd) < 0) {
		// 加锁失败，说明已经有进程对这个文件加了锁。
		if (errno ==EACCES || errno == EAGAIN) {
			// 关闭文件描述符，并返回文件已被占用的通知。
			close(fd);
			return(1);
		}
		// 加锁失败，但是并不是由于文件被占用的原因，而是其他原因导致的失败，
		// 这时需要做进程退出处理。
		syslog(LOG_ERR, "can't lock %s: %s", LOCKFILE, strerror(errno));
		exit(1);
	}
	// 对目标文件加锁成功。
	// 截断文件长度为0，及清空文件的所有旧的内容。
	ftruncate(fd, 0);
	// 将进程ID写入目标文件。
	sprintf(buf, "%ld", (long)getpid());
	write(fd, buf, strlen(buf)+1);
	return (0);
}
