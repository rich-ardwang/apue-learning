#include "apue.h"

int
main(void)
{
	char		buf[MAXLINE];
	// fgets从标准输入读入数据，一次一行，行缓冲机制。
	while (fgets(buf, MAXLINE, stdin) != NULL) {
		// fputs将从标准输入读取的数据写入到标准输出，也是一次写一行，行缓冲机制。
		if (fputs(buf, stdout) == EOF)
			err_sys("output error");
	}

	// 这里通过ferror判断fgets()在读取标准输入时是否发生错误。
	if (ferror(stdin))
		err_sys("input error");

	// 正常退出，exit()函数会自动flush冲洗缓冲区，冲洗完毕后关闭流。
	exit(0);
}

// 点评
// apue书中将标准库中的3对IO函数进行了效率对比，可以参见书中图表。
// 这三对函数是getc与putc，fgetc与fputc，以及fgets与fputs，
// 前面两对IO函数是一次读取一个字符，本例中的一对IO函数是一次读取一行。

// 关于三组IO函数的效率，可以参考书中图表，这里我们需要记住一些实验结果：
// 1.三组IO函数的系统CPU时间几乎相同，这是因为他们对内核提出的读写请求数
// 基本一致。
// 2.因为系统CPU时间几乎相同，所以他们的用户CPU时间和IO等待时间决定了他们
// 耗费时钟时间的不同。
// 3.前两组用户CPU时间基本相同，是最后一组用户CPU时间的4倍。
// 4.前两组时钟时间也基本相同，是最后一组时钟时间的3倍。
// 总结：
// 通过以上分析，就效率而言，fgets与fputs效率最高，他们是用memccpy()实现的，
// 而memccpy是用汇编语言实现的，所以效率更高。
// 前两组效率基本一致，但是比fgets与fputs要低三四倍，因为他们每次读一个字节，
// 循环次数太多了。
