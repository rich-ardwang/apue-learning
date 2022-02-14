#include "apue.h"
#include <ctype.h>
#include <fcntl.h>

#define BSZ		4096

unsigned char buf[BSZ];

// 使用rot-13算法加密文本文件。
unsigned char
translate(unsigned char c)
{
	if (isalpha(c)) {
		if (c >= 'n')
			c -= 13;
		else if (c >= 'a')
			c += 13;
		else if (c >= 'N')
			c -= 13;
		else
			c += 13;
	}
	return (c);
}

int
main(int argc, char *argv[])
{
	int		ifd, ofd, i, n, nw;
	if (argc != 3)
		err_quit("usage: rot13 infile outfile");
	// 只读模式打开infile。
	if ((ifd = open(argv[1], O_RDONLY)) < 0)
		err_sys("can't open %s", argv[1]);
	// 读写+截断模式创建outfile。
	if ((ofd = open(argv[2], O_RDWR|O_CREAT|O_TRUNC, FILE_MODE)) < 0)
		err_sys("can't create %s", argv[2]);

	// 系统调用read，缓冲设为4096(效率相对最佳)。
	while ((n = read(ifd, buf, BSZ)) > 0) {
		// 对这一段缓冲区内字符串进行rot-13加密。
		for (i = 0; i < n; i++)
			buf[i] = translate(buf[i]);
		// 将加密后的缓冲区内的字符串写入文件。
		if ((nw = write(ofd, buf, n)) != n) {
			if (nw < 0)
				err_sys("write failed");
			else
				err_quit("short write (%d/%d)", nw, n);
		}
	}

	// 阻塞等待内核缓冲区内所有数据(包括文件数据+文件属性)冲洗入磁盘后返回。
	fsync(ofd);
	exit(0);
}

// 使用rot-13加密后的文件，只要再调用一次translate函数即可解密。
