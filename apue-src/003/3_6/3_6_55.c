#include "apue.h"
#include <fcntl.h>

char	buf1[] = "abcdefghij";
char	buf2[] = "ABCDEFGHIJ";

int
main(void)
{
	int	fd;

	// sizeof计算出的char数组长度包含终止null字符。
	printf("%u\n", sizeof(buf1));
	// strlen计算出的char数组长度仅为字符串长度，不
	// 包含终止null字符。
	printf("%u\n", strlen(buf1));

	// 创建名为file.hole的文件。
	// creat等价于open(path, O_WRONLY | O_CREAT | O_TRUNC, mode)。
	if ((fd = creat("file.hole", FILE_MODE)) < 0)
		err_sys("creat error");

	// 在文件开头写入10个字节的小写字母，此时offset为10。
	if (write(fd, buf1, 10) != 10)
		err_sys("buf1 write error");
	/* offset now = 10 */

	// 使用lseek设置文件偏移量为距文件开始处16384个字节。
	if (lseek(fd, 16384, SEEK_SET) == -1)
		err_sys("lseek error");
	/* offset now = 16384 */

	// 从偏移量16384开始，再写入10个字节的大写字母，此时
	// 偏移量为16394。
	// 此时文件前10个字节为小写字母，后10个字节为大写字母，
	// 但是11-16384个字节并没有写入任何内容，这样就产生了
	// 文件空洞。
	if (write(fd, buf2, 10) != 10)
		err_sys("buf2 write error");
	/* offset now = 16394 */

	// 正常退出。
	exit(0);
}

// 点评
// 本例演示利用lseek生成具有空洞的文件，具有空洞的文件比没有
// 空洞的文件占用更少的磁盘块，未写入内容的空洞部分会被设置为
// 空(null或\0)。
// 1.使用od -c file.hole，可以以字符方式查看文件内容，可以
// 看出空洞部分为\0。
// 2.使用ls -ls file.hole file.nohole，可以看出有空洞的
// 文件占用更少的磁盘块。
