#include "apue.h"
#include <pthread.h>

int			quitflag;		/* set nonzero by thread */
sigset_t	mask;  // 全局信号集，将需要等待的信号放在其中。

// 静态方式初始化互斥锁。
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
// 静态方式初始化条件变量。
pthread_cond_t waitloc = PTHREAD_COND_INITIALIZER;

// 线程函数
void *
thr_fn(void *arg)
{
	int		err, signo;

	for ( ; ; ) {
		// 阻塞等待信号发生。
		err = sigwait(&mask, &signo);
		if (err != 0)
			err_exit(err, "sigwait failed");
		switch (signo) {
		case SIGINT:
			// 如果是ctrl+c，我们仅仅打印一条信息，不执行中断。
			printf("\ninterrupt\n");
			break;
		case SIGQUIT:
			// 如果是ctrl+\，修改quitflag，然后发送条件变量已满足的信号。
			pthread_mutex_lock(&lock);
			quitflag = 1;
			pthread_mutex_unlock(&lock);
			pthread_cond_signal(&waitloc);
			return (0);
		default:
			// 其他信号不处理，直接退出。
			printf("unexpected signal %d\n", signo);
			exit(1);
		}
	}
}

int
main(void)
{
	int			err;
	sigset_t	oldmask;
	pthread_t	tid;

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGQUIT);
	// 让进程当前信号屏蔽字与mask中的取并集，保存老的信号屏蔽字。
	if ((err = pthread_sigmask(SIG_BLOCK, &mask, &oldmask)) != 0)
		err_exit(err, "SIG_BLOCK error");

	// 创建一个线程。
	err = pthread_create(&tid, NULL, thr_fn, 0);
	if (err != 0)
		err_exit(err, "can't create thread");

	// 主线程对互斥量加锁。
	pthread_mutex_lock(&clock);
	while (quitflag == 0)
		// 阻塞等待条件变量满足后发来的信号。
		// 互斥量保护条件变量。
		pthread_cond_wait(&waitloc, &lock);
	// 条件满足后跳出循环，解锁。
	pthread_mutex_unlock(&lock);

	/* SIGFQUIT has been caught and is now blocked; do whatever */
	// 此时quitflag的值为1，将它重置为。
	quitflag = 0;

	/* reset signal mask which unblocks SIGQUIT */
	// 恢复进程原始屏蔽字，然后退出。
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		err_sys("SIG_SETMASK error");
	exit(0);
}
