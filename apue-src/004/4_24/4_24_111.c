#include "apue.h"
#ifdef SOLARIS
#include <sys/mkdev.h>
#endif
#include <sys/sysmacros.h>

int
main(int argc, char *argv[])
{
	int				i;
	struct stat		buf;
	// 接受任意多参数，每个参数都是一个文件，本程序可以将这些文件的主次设备号打印出来。
	// 如果是字符特殊和块特殊文件，还能打印出其实际设备的主次设备号。
	for (i = 1; i < argc; i++) {
		// 先打印出文件名称（不换行）
		printf("%s: ", argv[i]);
		// 获取该文件的详细属性信息
		if (stat(argv[i], &buf) < 0) {
			// 如果stat该文件时出错，则报错后继续执行，不影响后续文件。
			err_ret("stat error");
			continue;
		}
		// 在文件详细属性信息中有成员st_dev，这是系统中与该文件名相关联的文件系统设备号。
		// 可以使用major和minor这两个宏，分别获取到文件系统的主设备号和次设备号。
		printf("dev = %d/%d", major(buf.st_dev), minor(buf.st_dev));
		// 只有字符特殊和块特殊文件才有st_rdev属性，这个是它们的实际设备号。
		if (S_ISCHR(buf.st_mode) || S_ISBLK(buf.st_mode)) {
			// 同理，可以打印出字符特殊和块特殊文件的实际设备的主次设备号。
			printf(" (%s) rdev = %d/%d",
					(S_ISCHR(buf.st_mode)) ? "character" : "block",
					major(buf.st_rdev), minor(buf.st_rdev));
		}
		printf("\n");
	}
	exit(0);
}

// 点评
// 在文件系统中，任何文件和设备都拥有一个与之对应的文件系统设备号，
// 我们可以使用major和minor两个宏获取到主次设备号，然后通过观察
// 主次设备号，即可判断某些文件是否在同一个或不同的文件系统中。

// 只有字符特殊和块特殊文件拥有实际设备号，即使这个实际设备可能是被
// 虚拟出来的。
