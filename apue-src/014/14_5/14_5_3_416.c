#include "apue.h"
#include <ctype.h>
#include <fcntl.h>
#include <aio.h>
#include <errno.h>

// 每个缓冲区大小，4096相对读写效率比较高。
#define BSZ		4096
// 8个非阻塞异步IO使用的缓冲区。
#define NBUF	8

// 指示读写状态的枚举。
enum rwop {
	// 未使用
	UNUSED = 0,
	// 读等待
	READ_PENDING = 1,
	// 写等待
	WRITE_PENDING = 2
};

// 非阻塞异步IO缓冲区。
struct buf {
	// 读写状态
	enum rwop		op;
	// 指示到达文件末尾
	int				last;
	// POSIX异步IO控制块
	struct aiocb	aiocb;
	// 缓冲区
	unsigned char	data[BSZ];
};

// 定义8个非阻塞异步IO缓冲区。
struct buf bufs[NBUF];

// rot-13算法
unsigned char
translate(unsigned char c)
{
	/* same as before */
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
	int					ifd, ofd, i, j, n, err, numop;
	struct stat			sbuf;
	const struct aiocb	*aiolist[NBUF];
	off_t				off = 0;

	// 该程序只接受3个参数。
	if (argc != 3)
		err_quit("usage: rot13 infile outfile");
	// 只读模式打开infile。
	if ((ifd = open(argv[1], O_RDONLY)) < 0)
		err_sys("can't open %s", argv[1]);
	// 读写+截断模式创建outfile。
	if ((ofd = open(argv[2], O_RDWR|O_CREAT|O_TRUNC, FILE_MODE)) < 0)
		err_sys("can't create %s", argv[2]);
	// 读取文件属性。
	if (fstat(ifd, &sbuf) < 0)
		err_sys("fstat failed");

	/* initialize the buffers */
	// 初始化8个非阻塞异步IO缓冲区。
	for (i = 0; i < NBUF; i++) {
		bufs[i].op = UNUSED;
		bufs[i].aiocb.aio_buf = bufs[i].data;
		// 设置异步IO请求完成后不通知进程。
		bufs[i].aiocb.aio_sigevent.sigev_notify = SIGEV_NONE;
		// 基于列表的异步IO操作。
		aiolist[i] = NULL;
	}

	// 操作次数。
	numop = 0;
	for (;;) {
		// 无限循环，对8个异步IO缓冲区轮询。
		for (i = 0; i < NBUF; i++) {
			switch (bufs[i].op) {
			// 测试异步IO未使用状态。
			case UNUSED:
				/*
				 * Read from the input file if more data
				 * remains unread.
				 */
				if (off < sbuf.st_size) {
					// 修改状态为"正在读"。
					bufs[i].op = READ_PENDING;
					// 设置异步IO操作目标文件描述符。
					bufs[i].aiocb.aio_fildes = ifd;
					// 设置异步IO读取的偏移量。
					bufs[i].aiocb.aio_offset = off;
					// 累加偏移量。
					off += BSZ;
					if (off >= sbuf.st_size)
						// 设置到达文件末尾标志。
						bufs[i].last = 1;
					// 设置异步IO读取的字节数。
					bufs[i].aiocb.aio_nbytes = BSZ;
					// 申请POSIX异步IO读操作。
					if (aio_read(&bufs[i].aiocb) < 0)
						err_sys("aio_read failed");
					// 将异步IO控制块保存在列表中。
					aiolist[i] = &bufs[i].aiocb;
					numop++;
				}
				break;
			// 测试异步IO读状态。
			case READ_PENDING:
				// 测试读状态有无异常，是否完成。
				// 读未完成，则继续下次循环。
				// 读出错则报错退出进程。
				if ((err = aio_error(&bufs[i].aiocb)) == EINPROGRESS)
					continue;
				if (err != 0) {
					if (err == -1)
						err_sys("aio_error failed");
					else
						err_exit(err, "read failed");
				}

				/*
				 * A read is complete; translate the buffer
				 * and write it.
				 */
				// 读状态完成，调用aio_return取得返回值。
				if ((n = aio_return(&bufs[i].aiocb)) < 0)
					err_sys("aio_return failed");
				// 如果没有读到缓冲区长度，且未到达文件尾，
				// 则报错退出进程。
				if (n != BSZ && !bufs[i].last)
					err_quit("short read (%d/%d)", n, BSZ);
				// 正常读取的情况，逐个字节进行rot-13加密。
				for (j = 0; j < n; j++)
					bufs[i].data[j] = translate(bufs[i].data[j]);
				// 设置异步IO为"正在写"状态。
				bufs[i].op = WRITE_PENDING;
				// 设置异步IO操作所需文件描述符。
				bufs[i].aiocb.aio_fildes = ofd;
				// 设置异步IO写操作写入多少字节。
				bufs[i].aiocb.aio_nbytes = n;
				// 申请POSIX异步IO写操作。
				if (aio_write(&bufs[i].aiocb) < 0)
					err_sys("aio_write failed");
				/* retain our spot in aiolist */
				break;
			// 测试异步IO写状态。
			case WRITE_PENDING:
				// 测试写状态有无异常，是否完成。
				// 写未完成，则继续下次循环。
				// 写出错则报错退出进程。
				if ((err = aio_error(&bufs[i].aiocb)) == EINPROGRESS)
					continue;
				if (err != 0) {
					if (err == -1)
						err_sys("aio_error failed");
					else
						err_exit(err, "write failed");
				}

				/*
				 * A write is complete; mark the buffer as unused.
				 */
				// 异步IO写操作完成后，调用aio_return取得返回值。
				if ((n = aio_return(&bufs[i].aiocb)) < 0)
					err_sys("aio_return failed");
				// 写出错，报错退出进程。
				if (n != bufs[i].aiocb.aio_nbytes)
					err_quit("short write (%d/%d)", n, BSZ);
				// 重置aio状态。
				aiolist[i] = NULL;
				bufs[i].op = UNUSED;
				// 一次异步读写完成，将操作计数减1。
				numop--;
				break;
			}
		}
		// 操作计数为0意味着所有异步读写已经完成。
		if (numop == 0) {
			// 偏移量到达文件尾跳出无限循环。
			if (off >= sbuf.st_size)
				break;
		} else {
			// 操作计数非0意味着异步读写未完成，此时所有已完成的异步读写操作
			// 需要阻塞等待所有未完成的异步读写操作。
			if (aio_suspend(aiolist, NBUF, NULL) < 0)
				err_sys("aio_suspend failed");
		}
	}
	// 所有POXIS异步IO读写操作正确完成，
	// 需要阻塞式冲洗内核缓冲区，待内核缓冲区
	// 所有文件数据和文件属性写入磁盘后返回。
	bufs[0].aiocb.aio_fildes = ofd;
	if (aio_fsync(O_SYNC, &bufs[0].aiocb) < 0)
		err_sys("aio_fsync failed");
	exit(0);
}

// gcc 14_5_3_416.c -o 14_5_3_416 -lapue -lrt
// 在编译链接时必须添加-lrt命令选项，因为rt库才包含了系统底层的API。
