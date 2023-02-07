#include "apue.h"
#include <pthread.h>

// 格式化输出函数，仅仅通知清理了哪个线程。
void
cleanup(void *arg)
{
	// 格式化输出
	printf("cleanup: %s\n", (char *)arg);
}

// 线程1的启动例程
void *
thr_fn1(void *arg)
{
	// 输出线程1启动消息
	printf("thread 1 start\n");
	// 注册线程1退出时需要调用的自定义清理函数，向次函数传递void*参数1。
	pthread_cleanup_push(cleanup, "thread 1 first handler");
	// 注册线程1退出时需要调用的自定义清理函数，向次函数传递void*参数2。
	pthread_cleanup_push(cleanup, "thread 1 second handler");
	// 输出信息，通知线程1的自定义清理函数push完毕。
	printf("thread 1 push complete\n");
	// 如果参数有效，则退出线程，退出状态为1。
	if (arg)
		return ((void *)1);
	// 以下代码仅仅是为了通过编译
	pthread_cleanup_pop(0);
	pthread_cleanup_pop(0);
	return ((void *)1);
}

// 线程2的启动例程
void *
thr_fn2(void *arg)
{
	// 输出线程2启动消息
	printf("thread 2 start\n");
	// 注册线程2退出时需要调用的自定义清理函数，向次函数传递void*参数1。
	pthread_cleanup_push(cleanup, "thread 2 first handler");
	// 注册线程2退出时需要调用的自定义清理函数，向次函数传递void*参数2。
	pthread_cleanup_push(cleanup, "thread 2 second handler");
	// 输出信息，通知线程2的自定义清理函数push完毕。
	printf("thread 2 push complete\n");
	// 如果参数有效，则退出线程，退出状态为2。
	if (arg)
		pthread_exit((void *)2);
	// 以下代码仅仅是为了通过编译
	pthread_cleanup_pop(0);
	pthread_cleanup_pop(0);
	pthread_exit((void *)2);
}

int
main(void)
{
	// 返回值
	int				err;
	// 线程ID
	pthread_t		tid1, tid2;
	// 线程的退出状态
	void			*tret;

	// 创建线程1，获取其线程ID，指定其启动例程，向启动例程传递传递参数1。
	err = pthread_create(&tid1, NULL, thr_fn1, (void *)1);
	// 如果创建线程失败，则输出错误信息，退出进程。
	if (err != 0)
		err_exit(err, "can't create thread 1");
	// 创建线程2，获取其线程ID，指定其启动例程，向启动例程传递传递参数1。
	err = pthread_create(&tid2, NULL, thr_fn2, (void *)1);
	// 如果创建线程失败，则输出错误信息，退出进程。
	if (err != 0)
		err_exit(err, "can't create thread 2");
	// 主线程阻塞等待线程1退出，并获取其退出状态。
	err = pthread_join(tid1, &tret);
	// 如果等待失败，则输出错误信息，退出进程。
	if (err != 0)
		err_exit(err, "can't join with thread 1");
	// 输出消息，通知线程1退出状态码。
	printf("thread 1 exit code %ld\n", (long)tret);
	// 主线程阻塞等待线程2退出，并获取其退出状态。
	err = pthread_join(tid2, &tret);
	// 如果等待失败，则输出错误信息，退出进程。
	if (err != 0)
		err_exit(err, "can't join with thread 2");
	// 输出消息，通知线程2退出状态码。
	printf("thread 2 exit code %ld\n", (long)tret);
	// 正常结束进程，冲洗IO流。
	exit(0);
}

// 点评
// 本例给出了一个如何使用线程清理处理程序的例子，展示了清理机制。
// 虽然并不想要传一个参数0给线程的启动例程，但还是需要把pthread_cleanup_push
// 和pthread_cleanup_pop两个调用匹配使用，否则不能通过编译。

// 试验结果
// 1.两个线程都正确地启动和退出了。
// 2.只有thread2的cleanup函数被执行了，说明线程必须通过pthread_exit函数
// 退出，其cleanup函数才能被执行，从线程函数返回是不行的。
// 3.清理处理程序是按照与它们安装时相反的顺序被调用的。

// 跨平台
// 在FreeBSD或Mac OS X上运行时会由于段错误core dump，因为在这两个平台上，
// pthread_cleanup_push使用宏实现的，而宏把某些上下文存放在栈上，当线程1
// 在调用pthread_cleanup_push和调用pthread_cleanup_pop之间返回时，栈已
// 被改写，而调用清理处理程序时就用了这个被改写的上下文。

// 可移植
// pthread_exit函数是可以被移植的。

// 进程和线程函数的对比
// 进程原语       线程原语                  描述
// fork       pthread_create         创建新的控制流
// exit       pthread_exit           从现有的控制流中退出
// waitpid    pthread_join           从控制流中得到退出状态
// atexit     pthread_cancel_puth    注册在退出控制流时调用的函数
// getpid     pthread_self           获取控制流的ID
// abort      pthread_cancel         请求控制流的非正常退出

// 注意
// pthread_join只能等待未分离状态的线程并获取其退出信息，对分离状态的线程调用
// 会产生未定义行为。

// 分离线程
// pthread_detach函数可以分离线程。
