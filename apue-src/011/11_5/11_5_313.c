#include "apue.h"
#include <pthread.h>

// 定义了一个包含四个int型成员的结构体foo
struct foo {
	int a, b, c, d;
};

// 格式化输出函数
void
printfoo(const char *s, const struct foo *fp)
{
	// 输出字符串s的值
	printf("%s", s);
	// 以16进制查看foo对象的内存地址
	printf("	structure at 0x%lx\n", (unsigned long)fp);
	// 输出结构体foo的一个实例（对象）的所有int成员变量的值
	printf("	foo.a = %d\n", fp->a);
	printf("	foo.b = %d\n", fp->b);
	printf("	foo.c = %d\n", fp->c);
	printf("	foo.d = %d\n", fp->d);
}

// 线程1的启动例程函数
void *
thr_fn1(void *arg)
{
	// 在线程1自己的栈上分配一个结构对象foo
	struct foo		foo = {1, 2, 3, 4};

	// 格式化输出结构对象foo
	printfoo("thread 1:\n", &foo);
	// 将这个在栈上分配的临时对象foo传递给pthread_exit函数
	pthread_exit((void *)&foo);
}

// 线程2的启动例程函数
void *
thr_fn2(void *arg)
{
	// 线程2调用pthread_self获取自己的线程ID，然后将其格式化输出。
	printf("thread 2: ID is %lu\n", (unsigned long)pthread_self());
	// 线程调用pthread_exit终止自己，终止状态为0。
	pthread_exit((void *)0);
}

int
main(void)
{
	// 返回值
	int			err;
	// 线程ID
	pthread_t	tid1, tid2;
	// 结构体指针
	struct foo	*fp;

	// 创建线程1，获取其线程ID，指定其启动例程。
	err = pthread_create(&tid1, NULL, thr_fn1, NULL);
	// 创建失败时输出错误信息，然后终止进程。
	if (err != 0)
		err_exit(err, "can't create thread 1");
	// 主线程阻塞等待线程1终止，并获取其终止状态的结构对象信息。
	err = pthread_join(tid1, (void *)&fp);
	// join失败时输出错误信息，然后终止进程。
	if (err != 0)
		err_exit(err, "can't join with thread 1");
	// 主线程休眠1秒，让线程1的资源完全得到释放。
	sleep(1);
	// 输出信息，提示开始创建线程2。
	printf("parent starting second thread\n");
	// 创建线程2，获取其线程ID，指定其启动例程。
	err = pthread_create(&tid2, NULL, thr_fn2, NULL);
	// 创建失败时输出错误信息，然后终止进程。
	if (err != 0)
		err_exit(err, "can't create thread 2");
	// 主线程休眠1秒，让线程2的资源完全得到释放。
	sleep(1);
	// 因为foo是在线程1的栈上被创建的，线程1退出后，这个栈就不存在了，
	// 可能被其他线程的栈(如线程2)覆盖，这样主线程访问时会出现未知结果。
	printfoo("parent:\n", fp);
	// 正常结束进程，冲洗IO流。
	exit(0);
}

// 点评
// 本程序演示了用自动变量（分配在栈上）作为pthread_exit的参数时出现的问题。

// 运行结果
// 根据各平台的内存体系结构、编译器以及线程库的实现会有所不同。
// 在Linux、Solaris、Mac OS X和FreeBSD上运行结果都不相同。
// 在tid1的栈上分配的自动变量，当主线程访问时已经改变了，tid1的线程栈可能被
// tid2的线程栈覆盖了。

// 问题解决
// 1.使用全局结构
// 2.使用malloc在堆上分配结构

// Mac OS X上会出现Segmentation fault (core dumped)
// 主线程试图访问线程1的内存时，它不再有效，这时得到SIGSEGV信号。

// FreeBSD上内存并没有被覆盖，但其他平台不会这样。
