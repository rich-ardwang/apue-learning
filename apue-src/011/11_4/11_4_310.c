#include "apue.h"
#include <pthread.h>

pthread_t	ntid;

void
printids(const char *s)
{
	pid_t		pid;
	pthread_t	tid;

	pid = getpid();
	tid = pthread_self();
	printf("%s pid %lu tid %lu (0x%lx)\n", s, (unsigned long)pid,
		(unsigned long)tid, (unsigned long)tid);
}

void *
thr_fn(void *arg)
{
	char msg[20];
	sprintf(msg, "new thread[%d]: ", *((int *)arg));
	printids(msg);
	return ((void *)0);
}

int
main(void)
{
	int			err;
	int nseq[10];

	for (int i=0; i<10; i++) {
		nseq[i] = i+1;
		err = pthread_create(&ntid, NULL, thr_fn, (void *)&nseq[i]);
		if (err != 0)
			err_exit(err, "can't create thread");
	}
	printids("main thread:");
	sleep(1);
	exit(0);
}

// 创建的每个线程执行的先后顺序是不确定的，传给每个线程
// 的序号参数存储在一个固定的数组中，每个线程使用自己的专有
// 数组元素，这样不会产生race condition。
