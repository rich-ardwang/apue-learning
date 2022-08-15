#include "apue.h"

int main(int argc, char *argv[])
{
	int			i;
	struct stat buf;
	char		*ptr;

	// 接受一组参数，这组参数可以是目录、普通文件、FIFO等任意文件。
	for (i = 1; i < argc; i++) {
		// 打印出参数名，注意没有换行符。
		printf("%s: ", argv[i]);
		// 调用lstat函数取得文件详细信息，该函数最大的特点是当文件是符号链接类型时，
		// 它不会跟随符号链接，几它会取得符号链接本身的信息，而不是符号链接指向的文件。
		if (lstat(argv[i], &buf) < 0) {
			err_ret("lstat error");
			continue;
		}
		// S_ISREG宏判断该文件类型是否是普通文件。
		// 普通文件可以是文本文件或二进制文件，内核不区分它们，解释权由应用程序负责。
		// 但是可执行二进制文件（ELF文件）必须符合一定规则，内核才能知道怎样加载它（数据段、代码段等）。
		if (S_ISREG(buf.st_mode))
			ptr = "regular";
		// S_ISDIR宏判断该文件类型是否是目录文件。
		// 目录文件包含了其他文件的名字以及指向这些文件有关信息的指针。
		// 对目录具有读权限，才能读取它包含的文件以及这些文件的信息。
		// 对目录具有写和执行权限，才拥有在目录下执行修改、新建、删除文件等操作的前提，
		// 当然是否能操作还要看对具体文件所拥有的权限是什么。
		else if (S_ISDIR(buf.st_mode))
			ptr = "directory";
		// S_ISCHR宏判断该文件类型是否是字符特殊文件（shell、其他字符型终端设备等）。
		// 这种类型文件提供对设备不带缓冲的访问，每次访问长度可变。
		else if (S_ISCHR(buf.st_mode))
			ptr = "character special";
		// S_ISBLK宏判断该文件类型是否是快特殊文件（光盘、磁盘等设备）。
		// 这种类型文件提供对设备带缓冲的访问，每次访问以固定长度为单位进行。
		else if (S_ISBLK(buf.st_mode))
			ptr = "block special";
		// S_ISFIFO宏判断该文件类型是否是FIFO文件。
		// 这种类型文件称为命名管道（named pipe），用于进程间通信。
		else if (S_ISFIFO(buf.st_mode))
			ptr = "fifo";
		// S_ISLNK宏判断该文件类型是否是符号链接文件。
		// 这种类型文件指向另一个文件。
		else if (S_ISLNK(buf.st_mode))
			ptr = "symbolic link";
		// S_ISSOCK宏判断该文件类型是否是socket文件。
		// 套接字类型的文件用于进程间的网络通信，也可用于同一台宿主机各个进程间的非网络通信。
		else if (S_ISSOCK(buf.st_mode))
			ptr = "socket";
		else
			ptr = "** unknown mode **";
		printf("%s\n", ptr);
	}
	exit(0);
}

// 点评
// 本例使用lstat是为了检测符号链接，如果使用stat则不会观察到符号链接。
// 介绍了文件类型判断宏的用法，这些宏一般在<sys/stat.h>中定义，如：
// #define S_ISDIR (mode)	((mode) & S_IFMT) == S_IFDIR)
// 最后，统计一下系统中各种类型文件所占的比例：
// 普通文件：79.77%
// 目录：11.93%
// 符号链接：8.25%
// 字符特殊：0.03%
// 快特殊：0.01%
// 套接字：0.01%
// FIFO：0.00%
