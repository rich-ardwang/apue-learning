#include "apue.h"

int main(void)
{
	// 输出进程id，进程id是每个进程唯一的标识。
	// 进程id是pid_t类型，可以将它转为long型。
	printf("hello world from process ID %ld\n", (long)getpid());
	exit(0);
}
