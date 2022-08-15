#include "apue.h"
#include <dirent.h>
#include <limits.h>

/* function type that is called for each filename */
// 知识点：定义函数类型 & 定义函数指针类型
// 定义一个函数类型为Myfunc1，当使用函数指针时用法为Myfunc1 *p1，
// 其中p1为Myfunc1类型的函数指针。
// 容易混淆的是，定义一个函数类型与定义一个函数指针类型是不一样的。
// typedef int (* Myfunc2)(const char*, const struct stat *, int)，
// 上面是定义了一个函数指针类型为Myfunc2，当使用函数指针时用法为Myfunc2 p2，
// 其中p2为Myfunc2类型的函数指针。
// p1前面有一个*才能成为函数指针，这是因为Myfunc1被定义为函数类型，
// 而p2前面不需要*，因为Myfunc2本身就是被定义为函数指针类型。
typedef int Myfunc(const char *, const struct stat *, int);

static Myfunc myfunc;  // 声明一个全局静态函数，myfunc。
static int		myftw(char *, Myfunc *);  // 声明全局静态函数myftw，其中使用Myfunc函数指针作为参数。
static int		dopath(Myfunc *);  // 声明全局静态函数dopath，其中使用Myfunc函数指针作为参数。
static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;  // 声明工作时使用的静态变量。

int
main(int argc, char *argv[])
{
	int			ret;
	// 接受一个目录参数
	if (argc != 2)
		err_quit("usage: ftw <starting-pathname>");
	// 使用myftw函数对指定目录进行遍历。
	ret = myftw(argv[1], myfunc);		/* does it all */
	// 所有类型文件的总和
	ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock;
	// 避免被除数为0的错误
	if (ntot == 0)
		ntot = 1;		/* avoid divide by 0; print 0 for all counts */
	// 输出统计结果，将7种类型文件的个数，以及所占百分比输出。
	printf("regular files = %7ld, %5.2f %%\n", nreg, nreg*100.0/ntot);
	printf("directories = %7ld, %5.2f %%\n", ndir, ndir*100.0/ntot);
	printf("block special = %7ld, %5.2f %%\n", nblk, nblk*100.0/ntot);
	printf("char special = %7ld, %5.2f %%\n", nchr, nchr*100.0/ntot);
	printf("FIFOs = %7ld, %5.2f %%\n", nfifo, nfifo*100.0/ntot);
	printf("symbolic links = %7ld, %5.2f %%\n", nslink, nslink*100.0/ntot);
	printf("sockets = %7ld, %5.2f %%\n", nsock, nsock*100.0/ntot);
	exit(ret);
}

/*
 * Descend through the hierarchy, starting at "pathname".
 * The caller's func() is called for every file.
 */
// 指明是文件，不是目录
#define FTW_F		1		/* file other than directory */
// 指明是目录
#define FTW_D		2		/* directory */
// 调用stat函数会出错的目录
#define FTW_DNR		3		/* directory that can't be read */
// 调用stat函数会出错的文件
#define FTW_NS		4		/* file that we can't stat */

// 文件名绝对路径
static char *fullpath;		/* contains full pathname for every file */
// 文件名长度
static size_t pathlen;

// 处理目录遍历的主函数
static int		/* we return whatever func() returns */
myftw(char *pathname, Myfunc *func)
{
	// 为文件名动态分配内存，在path_alloc函数中会PATH_MAX+1。
	fullpath = path_alloc(&pathlen);	/* malloc PATH_MAX+1 bytes */
										/* ({Flgure 2.16}) */
	// 如果path_alloc分配的内存数量小于等于命令行参数传进来的文件名长度，
	// 那么重新分配2倍于命令行参数传进来的文件名长度的内存空间。
	if (pathlen <= strlen(pathname)) {
		pathlen = strlen(pathname) * 2;
		if ((fullpath = realloc(fullpath, pathlen)) == NULL)
			err_sys("realloc failed");
	}
	// 将命令行传进来的文件名拷贝至fullpath
	strcpy(fullpath, pathname);
	// 调用工作函数开始遍历目录
	return (dopath(func));
}

/*
 * Descend through the hierarchy, starting at "fullpath".
 * If "fullpath" is anything other than a a directory, we lstat() it,
 * call func(), and return. For a directory, we call ourself
 * recursively for each name in the directory.
 */

// 遍历目录的工作函数，需要Myfunc函数指针作为参数。
static int						/* we return whatever func() returns */
dopath(Myfunc *func)
{
	struct stat			statbuf;
	struct dirent		*dirp;
	DIR					*dp;
	int					ret, n;
	// lstat函数不跟随符号链接，而是直接把符号链接文件本身的详细信息获取到。
	if (lstat(fullpath, &statbuf) < 0)	/* stat error */
		// 处理lstat会失败的目录
		return (func(fullpath, &statbuf, FTW_NS));
	if(S_ISDIR(statbuf.st_mode) == 0)	/* not a directory */
		// 处理除了目录类型以外的文件类型
		return (func(fullpath, &statbuf, FTW_F));
	/*
	 * It's a directory. First call func() for the directory,
	 * then process each filename in the directory.
	 */
	// 处理目录类型
	if ((ret = func(fullpath, &statbuf, FTW_D)) != 0)
		return (ret);
	n = strlen(fullpath);
	// 如果超过文件名长度缓冲区，那么就重新申请加倍长度的空间。
	if (n + NAME_MAX + 2 > pathlen) {		/* expand path buffer */
		pathlen *= 2;
		if ((fullpath = realloc(fullpath, pathlen)) == NULL)
			err_sys("realloc failed");
	}
	// 在文件名末尾追加'/'
	fullpath[n++] = '/';
	// 在文件名末尾追加终止符
	fullpath[n] = 0;
	// 尝试打开目录
	if ((dp = opendir(fullpath)) == NULL)		/* can't read directory */
		// 处理所有打开目录会失败的目录文件
		return (func(fullpath, &statbuf, FTW_DNR));
	// 遍历读取目录
	while ((dirp = readdir(dp)) != NULL) {
		// 忽略'.'和'..'目录
		if (strcmp(dirp->d_name, ".") == 0 ||
			strcmp(dirp->d_name, "..") == 0)
				continue;		/* ignore dot and dot-dot */
		// 在'/'后面追加读到的目录名
		strcpy(&fullpath[n], dirp->d_name);		/* append name after "/" */
		// 递归调用工作函数，处理子目录的遍历。
		if ((ret = dopath(func)) != 0)		/* recursive */
			break;		/* time to leave */
	}
	// 将'/'替换为终止符，这样可以抹去'/'后面的一切内容。
	fullpath[n-1] = 0;		/* erase everything from slash onward */
	// 关闭目录
	if (closedir(dp) < 0)
		err_ret("can't close directory %s", fullpath);
	return (ret);
}

// 自定义函数，用于处理各种不同类别的功能。
static int
myfunc(const char *pathname, const struct stat *statptr, int type)
{
	switch (type) {
	// 处理除了目录以外的所有文件
	// 这里仅仅统计各种类型文件的个数
	case FTW_F:
		switch (statptr->st_mode & S_IFMT) {
		case S_IFREG:	nreg++;		break;
		case S_IFBLK:	nblk++;		break;
		case S_IFCHR:	nchr++;		break;
		case S_IFIFO:	nfifo++;	break;
		case S_IFLNK:	nslink++;	break;
		case S_IFSOCK:	nsock++;	break;
		case S_IFDIR:	/* directories should have type = FTW_D */
			err_dump("for S_IFDIR for %s", pathname);
		}
		break;
	// 处理目录类型的文件
	// 统计目录个数
	case FTW_D:
		ndir++;
		break;
	// 处理所有不能读取的目录文件，报错即可。
	case FTW_DNR:
		err_ret("can't read directory %s", pathname);
		break;
	// 处理stat会出错的文件，报错即可。
	case FTW_NS:
		err_ret("stat error for %s", pathname);
		break;
	// 未知情形
	default:
		err_dump("unknown type %d for pathname %s", type, pathname);
	}
	return (0);
}

// 点评
// 使用符号链接（软链接）会产生目录遍历无限循环的问题，当遍历文件时产生无限循环后，如果可以控制不跟随
// 符号链接，那么将会很轻松的避免这种问题。
// 但是，如果使用硬链接，当遍历文件时产生无限循环后，则没有什么办法能够控制不跟随硬链接，因为硬链接本身
// 就是一个和原文件共享iNode节点的新文件，所以这会破坏文件系统，因此只允许root用户对目录进行硬链接。

// Solaris提供了一个ftw()函数，该函数可以遍历目录，当目录中有特殊的符号链接，进而产生目录循环后，
// 该函数会一直执行，直至出错返回errno值为ELOOP。
// Solaris还提供了另一nftw()函数，该函数可以遍历目录时指定不跟随符号链接，从而完全避免无限目录循环的问题。

// 本例编写了一个简单的目录遍历程序，可以在目录遍历时不跟随符号链接，效果和nftw()函数类似。

