#include <stdio.h>

// 该函数打开一个普通的IO文件流，然后将文件流指针返回。
FILE *
open_data(void)
{
	// 文件流指针
	FILE *fp;
	// 我们自己规定IO缓冲区的大小
	char databuf[BUFSIZ];		/* setvbuf makes this the stdio buffer */

	// 使用标准库IO函数fopen()以只读方式打开一个普通的IO文件流。
	if ((fp = fopen("datafile", "r")) == NULL)
		return (NULL);
	// 我们自定义标准IO库使用的缓冲区大小，以及设置缓冲方式为行缓冲。
	if (setvbuf(fp, databuf, _IOLBF, BUFSIZ) != 0)
		return (NULL);
	// 将已经打开的IO文件流指针返回。
	return (fp);		/* error */
}

// 点评
// 本例就一个函数讲解一些需要注意的问题，并不一定需要编译后演示。
// 这个函数看起似乎没什么问题，但很遗憾这是一个错误的例子。
// 因为open_data()函数设置缓冲区databuf是在它自己的栈上分配的，
// 然后它把这段内存空间派给标准IO库作为自定义缓冲区使用，当open_data()
// 函数返回后，它的栈将被其他函数所使用，而标准IO库也会用到其中的一段，
// 这样就产生冲突和错误。

// 解决办法
// 1.将自定义缓冲区databuf[BUFSIZ]的类型改为static或extern。
// 2.在堆上为自定义缓冲区动态申请内存，alloc()或malloc()等。
