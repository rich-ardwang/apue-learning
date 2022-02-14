#include "apue.h"
#include <pthread.h>

// 创建分离线程。
int
makethread(void *(*fn)(void *), void *arg)
{
	int				err;
	pthread_t		tid;
	pthread_attr_t	attr;

	// 初始化线程属性设置函数。
	err = pthread_attr_init(&attr);
	if (err != 0)
		return (err);
	// 改变线程属性为创建分离线程。
	err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (err == 0)
		// 创建分离线程。
		err = pthread_create(&tid, &attr, fn, arg);
	// 释放线程属性函数占用的资源。
	pthread_attr_destroy(&attr);
	return (err);
}
