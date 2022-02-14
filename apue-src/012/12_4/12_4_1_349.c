#include "apue.h"
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include "../12_3/12_3_343.c"

extern int makethread(void *(*)(void *), void *);

struct to_info {
	void			(*to_fn)(void *);		/* function 函数指针，保存具体的工作函数。 */
	void			*to_arg;				/* argument 工作函数需要使用的参数。*/
	struct timespec	to_wait;				/* time to wait 等待多长时间执行工作函数。*/
};

#define SECTONSEC		1000000000		/* seconds to nanoseconds */

// BSD系统使用clock_nanosleep纳秒级函数。
#if !defined(CLOCK_REALTIME) || defined(BSD)
#define clock_nanosleep(ID, FL, REQ, REM)	nanosleep((REQ), (REM))
#endif

// Linux等系统使用微秒级时钟函数。
#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME		0
#define USECTONSEC			1000	/* microseconds to nanoseconds */

void
clock_gettime(int id, struct timespec *tsp)
{
	struct timeval		tv;

	gettimeofday(&tv, NULL);
	tsp->tv_sec = tv.tv_sec;
	tsp->tv_nsec = tv.tv_usec * USECTONSEC;
}
#endif

// 分离线程所使用的线程函数，在该函数中延时执行
// to_info中保存的目标工作函数。
void *
timeout_helper(void *arg)
{
	struct to_info		*tip;

	tip = (struct to_info *)arg;
	// 线程休眠to_wait指定的秒数。
	clock_nanosleep(CLOCK_REALTIME, 0, &tip->to_wait, NULL);
	// 使用函数指针执行具体的工作函数，
	// 这个例子的工作函数就是retry。
	(*tip->to_fn)(tip->to_arg);
	// 释放arg参数并返回。
	free(arg);
	return (0);
}

// 超时函数，可以安排一个线程函数func在未来的某个时间执行。
void
timeout(const struct timespec *when, void (*func)(void *), void *arg)
{
	struct timespec		now;
	struct to_info		*tip;
	int					err;

	clock_gettime(CLOCK_REALTIME, &now);
	if ((when->tv_sec > now.tv_sec) ||
		(when->tv_sec == now.tv_sec && when->tv_nsec > now.tv_nsec)) {
		// 未超时的情况，创建分离式线程，让其在规定秒数后执行retry函数。
		printf("Time is not out, we create a thread and do the job.\n");
		tip = malloc(sizeof(struct to_info));
		if (tip != NULL) {
			tip->to_fn = func;
			tip->to_arg = arg;
			tip->to_wait.tv_sec = when->tv_sec - now.tv_sec;
			if (when->tv_nsec >= now.tv_nsec) {
				tip->to_wait.tv_nsec = when->tv_nsec - now.tv_nsec;
			} else {
				tip->to_wait.tv_sec--;
				tip->to_wait.tv_nsec = SECTONSEC - now.tv_nsec + when->tv_nsec;
			}
			// 创建分离线程，让它执行timeout_helper函数，
			// 然后在这个函数中延时执行tip中存储的工作函数，即retry。
			err = makethread(timeout_helper, (void *)tip);
			if (err == 0)
				// 成功创建线程后直接返回。
				return;
			else
				// 创建线程失败，释放tip结构。
				free(tip);
		}
	}

	/*
	 * We get here if (a) when <= now, or (b) malloc fails, or
	 * (c) we can't make a thread, so we just call the function now.
	 */
	// 这是超时的情况，主线程直接调用retry函数。
	printf("Time out, we run the work function directly.\n");
	(*func)(arg);
}

pthread_mutexattr_t		attr;
pthread_mutex_t			mutex;

// 模拟实际项目中的工作函数。
void
retry(void *arg)
{
	// A.在默认互斥锁模式下：
	// - retry调用者如果锁定了这个互斥量，这里会阻塞，
	// - 因为调用者是希望此函数返回再解锁，
	// - 而此函数在等调用者解锁，最终导致死锁。
	// B.在递归互斥锁模式下：
	// - retry可以对mutex重复上锁，不会导致死锁。
	pthread_mutex_lock(&mutex);

	/* perform retry steps ... */
	printf("function retry is invoked.\n");

	pthread_mutex_unlock(&mutex);
}

int
main(void)
{
	int					err, condition=1, arg;
	struct timespec		when;

	// 初始化互斥量属性修改函数。
	if ((err = pthread_mutexattr_init(&attr)) != 0)
		err_exit(err, "pthread_mutexattr_init failed");
	// 设置互斥量工作在递归模式。
	if ((err = pthread_mutexattr_settype(&attr,
		PTHREAD_MUTEX_RECURSIVE)) != 0)
		err_exit(err, "can't set recursive type");
	// 初始化互斥量。
	if ((err = pthread_mutex_init(&mutex, &attr)) != 0)
		err_exit(err, "can't create recursive mutex");

	/* continue processing ... */

	// retry调用者对互斥量加锁。
	pthread_mutex_lock(&mutex);

	/*
	 * Check the condition under the protection of a lock to
	 * make the check and the call to timeout atomic.
	 */
	printf("condition: %d\n", condition);
	if (condition) {
		/*
		 * Calculate the absolute time when we want to retry.
		 */
		clock_gettime(CLOCK_REALTIME, &when);
		// 这会导致超时，timeout函数(即主线程)直接执行retry函数。
		when.tv_sec -= 10;		/* 10 seconds from now */
		// 这会在10秒之后由新创建的分离式线程执行retry函数。
		when.tv_sec += 10;		/* 10 seconds from now */
		timeout(&when, retry, (void *)((unsigned long)arg));
	}
	// 让主线程等待15秒，然后解锁退出。
	// 等待15秒足够描述这个案例了，
	// 给retry函数留出了充分时间可以执行。
	sleep(15);
	// 在默认模式下会死锁，递归模式下不会死锁。
	pthread_mutex_unlock(&mutex);
	
	/* continue processing ... */
	printf("function main terminited.\n");

	exit(0);
}
