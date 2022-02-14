#include "apue.h"
#include <pthread.h>

// 静态方式初始化两个全局互斥量。
pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

// 父进程fork创建子进程前调用。
// 获取父进程定义的所有锁。
void
prepare(void)
{
	int	err;

	printf("preparing locks...\n");
	if ((err = pthread_mutex_lock(&lock1)) != 0)
		err_cont(err, "can't lock lock1 in prepare handler");
	if ((err = pthread_mutex_lock(&lock2)) != 0)
		err_cont(err, "can't lock lock2 in prepare handler");
}

// 父进程fork创建子进程后返回前，在父进程的上下文中执行。
// 对prepare获取的所有锁进行解锁。
void
parent(void)
{
	int	err;

	printf("parent unlocking locks...\n");
	if ((err = pthread_mutex_unlock(&lock1)) != 0)
		err_cont(err, "can't unlock lock1 in parent handler");
	if ((err = pthread_mutex_unlock(&lock2)) != 0)
		err_cont(err, "can't unlock lock2 in parent handler");
}

// 在fork返回前，在子进程的上下文中执行。
// 对prepare获取的所有锁进行解锁。
void
child(void)
{
	int	err;

	printf("child unlocking locks...\n");
	if ((err = pthread_mutex_unlock(&lock1)) != 0)
		err_cont(err, "can't unlock lock1 in child handler");
	if ((err = pthread_mutex_unlock(&lock2)) != 0)
		err_cont(err, "can't unlock lock2 in child handler");
}

// 线程函数
void *
thr_fn(void *arg)
{
	printf("thread started...\n");
	// 线程阻塞
	pause();
	return (0);
}

int
main(void)
{
	int			err;
	pid_t		pid;
	pthread_t	tid;

	// 因为要多线程和多进程混用，所以这里注册多线程存在，
	// 并且fork进程前后需要处理共享锁的三个函数。
	if ((err = pthread_atfork(prepare, parent, child)) != 0)
		err_exit(err, "can't install fork handlers");
	// 创建一个线程并让其阻塞。
	if ((err = pthread_create(&tid, NULL, thr_fn, 0)) != 0)
		err_exit(err, "can't create thread");

	// 主线程等待2秒，保证让子线程成功创建并执行。
	sleep(2);
	printf("parent about to fork...\n");

	// 准备fork子进程。
	if ((pid = fork()) < 0)
		err_quit("fork failed");
	else if (pid == 0)		/* child */
		printf("child returned from fork\n");
	else		/* parent */
		printf("parent returned from fork\n");
	exit(0);
}
