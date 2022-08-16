#include "apue.h"

// 全局函数声明如下
void	pr_stdio(const char *, FILE *);
int		is_unbuffered(FILE *);
int		is_linebuffered(FILE *);
int		buffer_size(FILE *);

int
main(void)
{
	// FILE结构体指针
	FILE		*fp;

	// fputs向标准输出写入字符串，提示用户可以输入任意字符。
	fputs("enter any character\n", stdout);
	// 使用标准库函数getchar()读取用户在标准输入的数据。
	if (getchar() == EOF)
		err_sys("getchar error");
	// fputs向标准错误写入字符串。
	fputs("one line to standard error\n", stderr);

	// 使用我们自己的pr_stdio()函数打印出标准输入、标准输出和标准错误的缓冲区。
	pr_stdio("stdin", stdin);
	pr_stdio("stdout", stdout);
	pr_stdio("stderr", stderr);

	// fopen以只读方式打开文件，并返回文件描述符。
	if ((fp = fopen("/etc/passwd", "r")) == NULL)
		err_sys("fopen error");
	// 利用文件描述符，调用getc()函数读取文件。
	if (getc(fp) == EOF)
		err_sys("getc error");
	// 打印出/etc/passwd该文件流的缓冲区。
	pr_stdio("/etc/passwd", fp);
	// 退出程序，冲洗缓冲区，关闭流。
	exit(0);
}

// 该函数可以打印出缓冲方式，以及缓冲区大小。
void
pr_stdio(const char *name, FILE *fp)
{
	// 打印出流的名称，不换行。
	printf("stream = %s, ", name);
	// 判断并打印出缓冲方式。
	// 无缓冲
	if (is_unbuffered(fp))
		printf("unbuffered");
	// 行缓冲
	else if (is_linebuffered(fp))
		printf("line buffered");
	// 全缓冲
	else	/* if neither of above */
		printf("fully buffered");
	// 打印缓冲区大小。
	printf(", buffer size = %d\n", buffer_size(fp));
}

/*
 * The following is nonportable.
 */
/* 下面的所有接口函数都是负责判断缓冲类型，与计算缓冲区大小的。
   根据各个UNIX平台的不同，使用条件编译的方法写出针对不同平台的代码。
*/

#if defined(_IO_UNBUFFERED)

int
is_unbuffered(FILE *fp)
{
	return (fp->_flags & _IO_UNBUFFERED);
}

int
is_linebuffered(FILE *fp)
{
	return (fp->_flags & _IO_LINE_BUF);
}

int
buffer_size(FILE *fp)
{
	return (fp->_IO_buf_end - fp->_IO_buf_base);
}

#elif defined(__SNBF)

int
is_unbuffered(FILE *fp)
{
	return (fp->_flags & __SNBF);
}

int
is_linebuffered(FILE *fp)
{
	return (fp->_falgs & __SLBF);
}

int
buffer_size(FILE *fp)
{
	return (fp->_bf._size);
}

#elif defined(_IONBF)

#ifdef		_LP64
#define		_flag		__pad[4]
#define		_ptr		__pad[1]
#define		_base		__pad[2]
#endif

int
is_unbuffered(FILE *fp)
{
	return (fp->_flag & _IONBF);
}

int
is_linebuffered(FILE *fp)
{
	return (fp->_flag & _IOLBF);
}

int
buffer_size(FILE *fp)
{
#ifdef _LP64
	return (fp->_base - fp->_ptr);
#else
	return (BUFSIZ);	/* just a guess */
#endif
}

#else

#error unknown stdio implementation!

#endif

// 点评
// 标准库在不同的UNIX平台上实现方式是不一样的，所以判断缓冲类型与计算缓冲区大小的代码
// 自然也不一样，需要针对各个平台进行编写，并执行条件编译。

// 实验
// 1.单纯执行本程序./5_13_131，输出结果可以看出标准输入和标准输出连接至终端时，他们
// 都是行缓冲，默认缓冲区大小为1024，标准错误无缓冲，文件流为全缓冲，默认4096。
// 2.执行重定向：./5_13_131 < /etc/group >std.out 2>std.err，查看输入结果，
// 标准输入和标准输出都被重定向到文件流，所以他们都是全缓冲，默认缓冲区大小为4096，
// 标准错误重定向为文件后，依然是无缓冲，文件流为全缓冲，默认4096。
