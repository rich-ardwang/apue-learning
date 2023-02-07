#include "apue.h"
#include <pthread.h>

// 线程1的工作函数
void *
thr_fn1(void *arg)
{
	// 线程1打印消息
	printf("thread 1 returning\n");
	// 从线程函数返回，返回码为1。
	return ((void *)1);
}

// 线程2的工作函数
void *
thr_fn2(void *arg)
{
	// 线程2打印消息
	printf("thread 2 exiting\n");
	// 利用系统提供的函数退出线程，返回码为2。
	pthread_exit((void *)2);
}

int
main(void)
{
	// 返回值
	int			err;
	// 线程ID
	pthread_t	tid1, tid2;
	// 线程返回值
	void		*rval_ptr;

	// 创建线程1，获取其线程ID，指定其工作函数。
	err = pthread_create(&tid1, NULL, thr_fn1, NULL);
	// 如果出错的话，输出错误消息并结束进程。
	if (err != 0)
		err_exit(err, "can't create thread 1");
	// 创建线程2，获取其线程ID，指定其工作函数。
	err = pthread_create(&tid2, NULL, thr_fn2, NULL);
	// 同上
	if (err != 0)
		err_exit(err, "can't create thread 2");
	// 等待线程1退出，获取其返回值，如果对返回值不感兴趣，可以传NULL。
	err = pthread_join(tid1, &rval_ptr);
	// 如果等待操作失败，输出错误信息并结束进程。
	if (err != 0)
		err_exit(err, "can't join with thread 1");
	// 输出线程1的退出码
	printf("thread 1 exit code %ld\n", (long)rval_ptr);
	// 同上
	err = pthread_join(tid2, &rval_ptr);
	if (err != 0)
		err_exit(err, "can't join with thread 2");
	// 输出线程2的退出码
	printf("thread 2 exit code %ld\n", (long)rval_ptr);
	// 正常结束进程，冲洗IO流。
	exit(0);
}

// 点评
// 本例重点演示了线程的退出和主线程阻塞等待函数pthread_join的用法。

// 单个线程有3种退出方式
// 1.线程可以简单地从启动例程（线程工作函数）中返回，返回值是线程的退出码。
// 2.线程可以被同一进程中的其他线程取消。
// 3.线程调用pthread_exit。

// 线程等待函数pthread_join
// 线程调用这个函数后，它将一直阻塞，直到指定的线程调用pthread_exit、从启动
// 例程中返回或被其他线程取消。

// 线程分离状态
// 调用pthread_join会自动把线程置于分离状态，这样资源就可以恢复。
// 如果线程已经处于分离状态，pthread_join调用就会失败，返回EINVAL。

// rval_ptr解释
// 1.如果被等待线程从它的启动例程中返回，rval_ptr就包含返回码。
// 2.如果线程被取消，有rval_ptr指定的内存单元就设置为PTHREAD_CANCELED。
// 3.如果对线程的返回值不感兴趣，可以把rval_ptr设置为NULL，这种情况
// 下，调用pthread_join仅仅等待线程终止，并不获取其终止状态。

// 注意
// pthread_create和pthread_exit函数的void无类型指针可以传递非常复杂
// 的信息的结构的地址，但是注意，这个结构所使用的内存在调用者完成调用
// 以后必须仍然是有效的。
// 例如，在调用线程的栈上分配了该结构，那么其他线程在使用这个结构时，
// 内存内用可以已经改变了。
// 又如，线程在自己的栈上分配了一个结构，然后把这个结构的指针传递给
// pthread_exit，那么调用pthread_join的线程试图使用该结构时，这个栈
// 可能已被撤销，这块内存也已另作他用。
