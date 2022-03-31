#include "apue.h"
#include <errno.h>

int main(int argc, char *argv[])
{
	// strerror可以打印出错误编号所对应提示字符串。
	fprintf(stderr, "EACCES: %s\n", strerror(EACCES));
	// 设置errno为不同的错误编号，perror会报出对应错误编号的提示字符串。
	errno = ENOENT;
	// perror接受一个参数，并将此参数作为错误提示的冒号前半部分。
	perror(argv[0]);
	exit(0);
}

// 注意
// errno的值永远为负值，不会大于等于0，在errno.h中定义了常见的错误编号
// 和对应的出错信息。
// 当没有出错时，errno的值不会被重置或清除，因此只在函数的返回值指明其出错
// 时，才会验证errno的值。
// errno支持多线程，多线程共享同一个进程空间，但它们拥有自己局部的errno，
// 不会彼此互相干扰。
