#include "apue.h"

int
main(void)
{
	struct stat		statbuf;

	/* turn on set-group-ID and turn off group-execute */
	// 在foo文件原来权限的基础上更改权限，进行相对值设置，即开启或关闭某些权限位。
	// 获取foo文件信息。
	if (stat("foo", &statbuf) < 0)
		err_sys("stat error for foo");
	// 改变foo文件权限，将组执行关闭（使用statbuf.st_mode和S_IXGRP与非操作），
	// 将设置组ID打开（使用statbuf.st_mode和S_ISGID或操作）。
	if (chmod("foo", (statbuf.st_mode & ~S_IXGRP) | S_ISGID) < 0)
		err_sys("chmod error for foo");

	/* set absolute mode to "rw-r--r--" */
	// 不管bar文件原来的权限如何，将其权限设为绝对值：用户读写-组读-其他读。
	if (chmod("bar", S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0)
		err_sys("chmod error for bar");
	exit(0);
}

// 点评
// 本例演示chmod函数的用法，该函数可以改变一个文件的权限，当然可以使用两种方式：
// 一种是在保留文件原来权限的基础上，进行某些权限位的开启与关闭，
// 另一种是无视文件原来的权限，直接将其修改为新的权限。
