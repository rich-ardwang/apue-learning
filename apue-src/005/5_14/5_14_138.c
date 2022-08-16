#include "apue.h"

// 定义我们自己的缓冲区大小
#define BSZ 48

int
main()
{
	FILE		*fp;
	char		buf[BSZ];

	// 将缓冲区前46个字节用'a'字符填充
	memset(buf, 'a', BSZ-2);
	// 设置缓冲区倒数第二字符为终止符
	buf[BSZ-2] = '\0';
	// 设置缓冲区最后一个字符为'X'
	buf[BSZ-1] = 'X';
	// 此时缓冲区应为: aaa...aaanullX
	// fmemopen()函数将使用我们自定义的缓冲区打开内存流。
	// 打开内存流的方式为w+，即通过在内存流文件起始处设置null的方式将文件截断为0，并读写模式。
	if ((fp = fmemopen(buf, BSZ, "w+")) == NULL)
		err_sys("fmemopen failed");
	// 此时缓冲区应为: nullaaa...aaanullX, 内存流文件偏移offsite: 0, 缓冲区长度curLen: 0
	// 打印出我们自定义的缓冲区
	// 由于buf的首位置被fmemopen放置了null，所以输出为空。
	printf("initial buffer contents: %s\n", buf);
	// 将12字节的字符串写入内存流，注意此时仅仅是写入了流中，还没有被冲洗进我们的缓冲区。
	fprintf(fp, "hello, world");
	// 在冲洗内存流之前，打印出此时的缓冲区，应该还是空。
	printf("before flush: %s\n", buf);
	// 冲洗内存流中的数据，写入缓冲区。
	fflush(fp);
	// 此时缓冲区应为: hello, worldnullaaa...aaanullX, 内存流文件偏移offsite: 12, 缓冲区长度curLen: 12
	// 解释：
	// 原来缓冲区长度为0，现在要增加到12，并且满足调用了fflush()函数，所以需要在缓冲区当前位置写入null。
	// 而缓冲区当前位置已经是null了，又写入的null把原来的null给覆盖了，然后在null的左边写入12个字节的字符串。
	// 所以此时内存流文件的偏移量是12，缓冲区长度也是12。
	// 冲洗完后，打印出此时的缓冲区。
	printf("after fflush: %s\n", buf);
	// 计算缓冲区中字符串的长度，应该为12。
	printf("len of string in buf = %ld\n", (long)strlen(buf));

	// 将缓冲区前46个字节用'b'字符填充
	memset(buf, 'b', BSZ-2);
	// 设置缓冲区倒数第二字符为终止符
	buf[BSZ-2] = '\0';
	// 设置缓冲区最后一个字符为'X'
	buf[BSZ-1] = 'X';
	// 此时缓冲区应为: bbb...bbbnullX，相当于缓冲区被重新初始化了。
	// 但是，我们必须注意的是，内存流文件的偏移量没有发生变化，还是12。
	// 再将12个字节的字符串写入内存流，加上之前的内存流文件偏移量12，
	// 所以冲洗的时候需要从第13个位置开始继续写入12个字节。
	fprintf(fp, "hello, world");
	// 定位至内存流文件偏移量为0，即内存流文件的起始位置，同时引发内存流冲洗。
	// 冲洗前缓冲区大小为12，冲洗后缓冲区大小为24，增大了，再加上调用了fseek(),
	// 所以需要在原来offsite为12的位置追加null。
	fseek(fp, 0, SEEK_SET);
	// 这样，现在的缓冲区应为: bbbbbbbbbbbbhello, worldnullbbb...bbbnullX, offsite: 0, curLen: 24
	// 定位后，打印出此时的缓冲区。
	// 注意，虽然两次写入缓冲区总共写了24个字节，但是fseek强制将内存流文件偏移量重置为0，所以偏移量不是24。
	printf("after fseek: %s\n", buf);
	// 计算缓冲区字符串长度，应该为24。
	printf("len of string in buf = %ld\n", (long)strlen(buf));

	// 将缓冲区前46个字节用'c'字符填充
	memset(buf, 'c', BSZ-2);
	// 设置缓冲区倒数第二字符为终止符
	buf[BSZ-2] = '\0';
	// 设置缓冲区最后一个字符为'X'
	buf[BSZ-1] = 'X';
	// 此时缓冲区应为: ccc...cccnullX，缓冲区又被重新初始化了。
	// 此时，内存流文件偏移量为0，原缓冲区长度为24。
	// 这时，再将12个字节写入内存流，等到冲洗时，内存流偏移量为0，也就是说从头写入12个字节，
	// 这样新缓冲区大小为12，比原来长度为24的缓冲区减小了，所以就不会在缓冲区当前位置追加null了。
	fprintf(fp, "hello, world");
	// 显示关闭该内存流，这会引发冲洗内存流。
	fclose(fp);
	// 因为新写入的12个字节，比原来缓冲区长度24要小，所以不满足追加null的条件。
	// 此时缓冲区应为: hello, worldccc...cccnullX, new curLen: 12, because new curLen < old curLen(12 < 24), didn't insert null.
	// 关闭内存流后，打印出此时的缓冲区。
	printf("after fclose: %s\n", buf);
	// 计算缓冲区中字符串的长度，应该为46。
	printf("len of string in buf = %ld\n", (long)strlen(buf));

	// 正常退出。
	return (0);
}

// 点评
// 本例主要演示内存流的用法，关于内存流的理解还是有一定难度的，需要从以下几个方面梳理下：
// 1.内存流最然也使用FILE指针来操作，但是它实际上是一个虚拟的文件，它的底层并没有真实的文件存在，
// 而是通过在内存中虚拟的文件与内存中的缓冲区直接交换数据。
// 2.缓冲区的长度一般来说是固定的，由用户提供，这里我们提供的缓冲区长度为48。
// 3.在写文件时一般先将数据写入内存流虚拟文件中，而这个虚拟文件的操作是靠当前偏移量来支持的，写入
// 以后在没有调用可以引发冲洗操作的函数前，数据暂时没有写入到缓冲区。
// 4.缓冲区的长度只计算到第一个null为止。
// 5.冲洗时，会根据一定法则来确定是否在缓冲区当前位置追加写入null，然后再冲入数据，这点非常重要。

// 关于自动追加null与否的判定
// 1.满足写入缓冲区后会导致缓冲区长度增加。
// 2.满足调用fclose、fflush、fseek、feeko以及fsetpos函数时。
// 1和2必须同时满足，才会在缓冲区当前追加null，否则不追加null。
