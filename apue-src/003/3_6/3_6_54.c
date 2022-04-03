#include "apue.h"
int
main(void)
{
	if (lseek(STDIN_FILENO, 0, SEEK_CUR) == -1)
		printf("cannot seek\n");
	else
		printf("seek OK\n");
	exit(0);
}

// 点评
// 本例演示lseek函数的用法。
// 参数1：文件描述符，这里指定为标准输入。
// 参数2：偏移量，类型为off_t。
// 参数3：
//       SEEK_CUR:将该文件的偏移量设置为其当前值+offset，offset可正或负。
//       SEEK_SET:将该文件的偏移量设置为距文件开始处offset个字节。
//       SEEK_END:将该文件的偏移量设置为文件长度+offset，offset可正或负。
// 返回值：
//		-1:当文件描述符指向一个管道，FIFO或套接字时，返回-1，并设置errno为
//			ESPIPE。
//		其他:正常返回offset计算后的偏移量，可能为正，也可能为负，或0等。
// 其他注意点：
//		1.lseek仅将文件当前偏移量记录在内核中，并不会引起任何I/O操作，然后该偏
//        移量用于下一个读或写操作。
//      2.当打开一个文件时，除非指定O_APPEND选项，否则该偏移量被设置为0。
