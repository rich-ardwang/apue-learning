#include "apue.h"
#include <pwd.h>

static void
my_alarm(int signo)
{
	struct passwd		*rootptr;

	printf("in signal handler\n");
	// getpwnam函数不可重入，不应在信号处理函数中调用，
	// 其内部有锁，如果此时进入信号处理函数内部，并执行如下代码，
	// 则getpwnam("root")不会返回，会一直阻塞，发生死锁。
	if ((rootptr = getpwnam("root")) == NULL)
		err_sys("getpwnam(root) error");
	// 重新注册信号处理函数并设定闹钟为1秒，如果不重新注册
	// 则再次收到闹钟信号时程序会终止，无法继续执行。
	signal(SIGALRM, my_alarm);
	alarm(1);
}

int
main(void)
{
	struct passwd		*ptr;

	signal(SIGALRM, my_alarm);
	alarm(1);
	for ( ; ; ) {
		if ((ptr = getpwnam("sar")) == NULL)
			err_sys("getpwnam error");
		if (strcmp(ptr->pw_name, "sar") != 0)
			printf("return value corrupted!, pw_name = %s\n", ptr->pw_name);
		else
			printf("pw_name = %s\n", ptr->pw_name);
	}
}
