#include "apue.h"

int
main(void)
{
	char	name[L_tmpnam], line[MAXLINE];
	FILE	*fp;
	// tmpnam(NULL)将生成一个不同于现有文件的随机的文件路径名，然后将其放在静态区域中，
	// 返回这个静态区域的指针。
	printf("%s\n", tmpnam(NULL));		/* first temp name */

	// 给tmpnam()函数传一个输出参数，则它生成一个不同于现有文件的随机的文件路径名后，
	// 会把其写入到输出参数中。
	tmpnam(name);						/* second temp name */
	// 打印出第二个tmpnam生成的临时文件路径名。
	printf("%s\n", name);

	// 调用tmpfile()函数创建一个临时文件，然后返回该临时文件的文件描述符。
	// 这个函数首先调用tmpnam()函数生成一个不同于现有文件的随机的文件路径名，
	// 然后使用这个路径名创建文件，之后立即unlink()，所以ls -la是看不到它的。
	if ((fp = tmpfile()) == NULL)		/* create temp file */
		err_sys("tmpfile error");
	// fputs()将一个字符串写入到由tmpfile创建的临时文件中。
	fputs("one line of output\n", fp);	/* write to temp file */
	// rewind()将一个文件流定位到该临时文件的起始位置。
	rewind(fp);							/* then read it back */
	// fgets()读取文件流中的数据，也就是从该临时文件的开始位置读取。
	if (fgets(line, sizeof(line), fp) == NULL)
		err_sys("fgets error");
	// 最后调用fputs()函数将数据写入到标准输出流。
	fputs(line, stdout);				/* print the line we wrote */

	// 正常退出，冲洗缓冲区，关闭IO流。
	exit(0);
}

/* 点评
1).每次调用tmpnam()函数都会先生成与现有文件不同的随机文件路径名，然后将创建的临时文件名返回，或者写到输出参数中。
2).tmpnam()函数创建的临时文件名格式：file+<6位随机字母和数字>，如：/tmp/fileky6gb6。
3).tmpnam()函数仅仅会生成一个每次调用都不同的临时文件路径名，它并不会去创建文件。
4).tmpfile()函数会创建一个临时的二进制文件，模式为wb+，但是内核并不区分文本文件，还是二进制文件。
5).tmpfile()函数利用tmpnam()函数生成一个唯一的文件路径名，然后用它创建临时文件，之后立即unlink()，
因此ls -la是无法看到它的。
6).临时文件创建完毕并unlink()后，虽然ls看不见，但是它仍然是存在的，只有当显示关闭该文件，或者当进程退出后，
内核才会销毁并释放它占用的内存空间。
7).一般仅仅是通过临时文件做事情，而不希望落盘，那么调用tmpfile()是个不错的选择，操作完毕后文件自动unlink()掉，也不会泄密。
8).如果希望临时文件保存一段时间，可以通过tmpnam()生成临时文件名，然后fopen()打开，之后如果不手动unlink()，
则该文件会保存在磁盘上，这个效果与mkstemp()函数差不多，后者就是先生成临时文件名，然后创建文件，需要手动unlink才能删除。
*/
