#include "apue.h"
#include <fcntl.h>
#include <sys/mman.h>

// 拷贝时缓冲区最大1G。
#define COPYINCR		(1024*1024*1024)	/* 1 GB */

int
main(int argc, char *argv[])
{
	int					fdin, fdout;
	void				*src, *dst;
	size_t				copysz;
	struct stat			sbuf;
	off_t				fsz = 0;

	// 该程序只接受3个参数，将fromfile拷贝至tofile。
	if (argc != 3)
		err_quit("usage: %s <fromfile> <tofile>", argv[0]);

	// 以只读方式打开fromfile。
	if ((fdin = open(argv[1], O_RDONLY)) < 0)
		err_sys("can't open %s for reading", argv[1]);

	// 以读写+截断方式创建文件tofile。
	if ((fdout = open(argv[2], O_RDWR | O_CREAT |O_TRUNC, FILE_MODE)) < 0)
		err_sys("can't creat %s for writing", argv[2]);

	// 读取文件属性。
	if (fstat(fdin, &sbuf) < 0)		/* need size of input file */
		err_sys("fstat error");

	// 设置输出文件size为输入文件size。
	if (ftruncate(fdout, sbuf.st_size) < 0)		/* set output file size */
		err_sys("ftruncate error");

	// 从0偏移量开始循环拷贝文件，到达输入文件size结束。
	while (fsz < sbuf.st_size) {
		// 控制每次循环拷贝最大字节数为1G。
		if ((sbuf.st_size - fsz) > COPYINCR)
			copysz = COPYINCR;
		else
			copysz = sbuf.st_size - fsz;

		// 使用mmap内存映射方式拷贝文件。
		// 先将输入文件的描述符与位于堆栈之间内存映射区域建立映射关系。
		if ((src = mmap(0, copysz, PROT_READ, MAP_SHARED, fdin, fsz)) == MAP_FAILED)
			err_sys("mmap error for input");
		// 再将输出文件的描述符与内存映射区域建立映射关系。
		if ((dst = mmap(0, copysz, PROT_READ | PROT_WRITE,
				MAP_SHARED, fdout, fsz)) == MAP_FAILED)
			err_sys("mmap error for output");

		// 将输入文件在内存映射区域中的数据拷贝至输出文件内存映射区域，
		// 进而再拷贝至输出文件。
		memcpy(dst, src, copysz);			/* does the file copy */
		// 释放输入与输出文件的内存映射区域。
		munmap(src, copysz);
		munmap(dst, copysz);
		// 累加已拷贝的字节数。
		fsz += copysz;
	}
	exit(0);
}

// 利用内存映射mmap技术拷贝文件。
