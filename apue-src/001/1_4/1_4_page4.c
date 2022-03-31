#include "apue.h"
#include <dirent.h>

int main(int argc, char *argv[])
{
	DIR					*dp;
	struct dirent		*dirp;

	// 仅仅接受一个参数。
	if (argc != 2)
		err_quit("usage: ls directory_name");

	// 使用传入参数打开目录。
	if((dp = opendir(argv[1])) == NULL)
		err_sys("Can't open %s", argv[1]);
	// 遍历读取目录中的文件。
	while ((dirp = readdir(dp)) != NULL) {
		// 文件名称，NAME_MAX（最长255字符）。
		printf("%s\n", dirp->d_name);
		// 文件类型。
        printf("%x\n", dirp->d_type);
		// 文件名长度。
        printf("%hu\n", dirp->d_reclen);
		// 索引节点号。
        printf("%lu\n", dirp->d_ino);
		// 在目录文件中的偏移。
        printf("%ld\n\n", dirp->d_off);
    }

	// 关闭目录。
	closedir(dp);
	// 正常退出。
	exit(0);
}
