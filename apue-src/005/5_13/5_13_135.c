#include "apue.h"

int
main(void)
{
	char	name[L_tmpnam], line[MAXLINE];
	FILE	*fp;

	printf("%s\n", tmpnam(NULL));		/* first temp name */

	tmpnam(name);						/* second temp name */
	printf("%s\n", name);

	if ((fp = tmpfile()) == NULL)		/* create temp file */
		err_sys("tmpfile error");
	fputs("one line of output\n", fp);	/* write to temp file */
	rewind(fp);							/* then read it back */
	if (fgets(line, sizeof(line), fp) == NULL)
		err_sys("fgets error");
	fputs(line, stdout);				/* print the line we wrote */

	exit(0);
}

/* 注意
Unix Book Advanced书中这部分可能讲解有误，实际测试下来结果如下：
1).tmpnam()函数仅仅返回一个/tmp目录下不存在的临时文件名，并不是真正创建了文件，也不是创建临时文件后立即unlink。因为ls无法看到它创建的文件，假如它创建了临时文件后立即unlink，那这样做是没有任何意义的。
2).tmpfile()函数并不是在tmpnam()函数生成的临时文件名上创建的临时文件，而是创建了一个匿名的临时文件，这个文件在磁盘上本来就无法查看到。实际上这两个函数没有什么必然联系，各自都是独立的。
3).一般仅仅是通过临时文件做事情，而不希望落盘，那么调用tmpfile()是个不错的选择，操作完毕后文件自动unlink()掉，也不会泄密。
4).如果希望临时文件保存一段时间，可以通过tmpnam()生成临时文件名，然后fopen打开，之后如果不手动unlink，则该文件会保存在磁盘上，这个效果与mkstemp()函数差不多，后者就是先生成临时文件名，然后创建文件，需要手动unlink才能删除。
*/