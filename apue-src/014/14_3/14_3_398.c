#include <unistd.h>
#include <fcntl.h>

// 使用记录锁的写模式，锁定整个文件。
int
lockfile(int fd)
{
	struct flock	fl;

	fl.l_type = F_WRLCK;
	// 以下三种赋值方式为锁定整个文件的所有记录。
	fl.l_start = 0;
	fl.l_whence = SEEK_SET;
	fl.l_len = 0;
	return (fcntl(fd, F_SETLK, &fl));
}
