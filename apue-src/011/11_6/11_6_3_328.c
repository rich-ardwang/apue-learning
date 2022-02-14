#include "apue.h"
#include <pthread.h>

// Add by Richard Wang
// 2021-12-30
void *thr_fn(void *arg) {
	// 7秒后解锁。
	sleep(11);
	pthread_mutex_t *plock = (pthread_mutex_t *)arg;
	pthread_mutex_unlock(plock);
}

int
main(void)
{
	int					err;
	struct timespec		tout;
	struct tm			*tmp;
	char				buf[64];
	pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

	pthread_mutex_lock(&lock);
	printf("mutex is locked\n");

	// Add by Richard Wang at 2021-12-30。
	// 把互斥锁传给线程，让它在规定的秒数后解锁。
	pthread_t ntid;
	err = pthread_create(&ntid, NULL, thr_fn, (void *)&lock);
	if (err != 0)
		err_exit(err, "can't create thread");

	clock_gettime(CLOCK_REALTIME, &tout);
	tmp = localtime(&tout.tv_sec);
	strftime(buf, sizeof(buf), "%r", tmp);
	printf("current time is %s\n", buf);
	tout.tv_sec += 10;	/* 10 seconds from now */
	/* caution: this could lead to deadlock */
	err = pthread_mutex_timedlock(&lock, &tout);
	clock_gettime(CLOCK_REALTIME, &tout);
	tmp = localtime(&tout.tv_sec);
	strftime(buf, sizeof(buf), "%r", tmp);
	printf("the time is now %s\n", buf);
	if (err == 0)
		printf("mutex locked again!\n");
	else
		printf("can't lock mvtex again:%s\n", strerror(err));
	exit(0);
}

// 如果10秒内解锁，pthread_mutex_timedlock会再次上锁，
// 否则它超时退出阻塞状态。
