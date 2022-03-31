#include "apue.h"

#define		BUFFSIZE		4096

int main(void)
{
	int		n;
	char	buf[BUFFSIZE];

	// 演示不带缓冲的系统I/O：
	// read和write是不带缓冲的系统调用，直接和内核打交道。
	// 需要手动指定其缓冲区大小，实验证明缓冲区大小为4096时，
	// I/O效率最高。
	// 文件描述符STDIN_FILENO指定从标准输入读，STDOUT_FILENO
	// 指定写入标准输出。
	while ((n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0)
		if (write(STDOUT_FILENO, buf, n) != n)
			// 如果写入返回值不等于要求写入的字符数，则报系统级错误。
			err_sys("write error");

	// 当while循环退出时，意味着或者读取工作已完成，此时读到了文件末尾
	// EOF，返回值是0；或者read函数出错，此时返回值<0。
	// read出错时报系统级错误。
	if (n < 0)
		err_sys("read error");

	// 正常退出。
	exit(0);
}

// 点评
// 该程序可以实现将标准输入复制到标准输出，如果执行./1_5_7 > data，
// 则标准输入是shell终端，标准输出重定向到文件data，如果该文件不
// 存在，shell会创建它。data文件的内容是终端键入的字符，如果键入
// 了文件结束符EOF，通常是Ctrl+D，则会结束复制。
// 如果执行./1_5_7 < infile > outfile，则会把infile文件复制到
// outfile。
