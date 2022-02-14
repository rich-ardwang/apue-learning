#include "slock.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

// 为基于POSIX信号量的自定义互斥锁分配资源。
struct slock *
s_alloc()
{
	struct slock	*sp;
	static int		cnt;

	// 在堆上为slock结构体分配内存。
	if ((sp = malloc(sizeof(struct slock))) == NULL)
		return (NULL);
	do {
		// 使用/+进程ID+cnt的值组合成POSIX信号量的名字。
		snprintf(sp->name, sizeof(sp->name), "/%ld.%d", (long)getpid(),
			cnt++);
		// 创建一个信号量并保存其对象指针。
		// 参数1：信号量名字。
		// 参数2：如果信号量存在即创建失败，如果失败则while循环尝试
		// 再次创建。
		// 参数3：用户读写执行。
		// 参数4：指定信号量的初始值，在0～SEM_VALUE_MAX之间。
		sp->semp = sem_open(sp->name, O_CREAT|O_EXCL, S_IRWXU, 1);
	} while ((sp->semp == SEM_FAILED) && (errno == EEXIST));
	// 最终创建POSIX信号量失败时，释放资源返回NULL。
	if (sp->semp == SEM_FAILED) {
		free(sp);
		return (NULL);
	}
	// 销毁一个命名的POSIX信号量。
	// 如果没有打开的信号量引用，则信号量被销毁，
	// 否则销毁会延迟到最后一个引用的信号量关闭。
	sem_unlink(sp->name);
	// 返回自定义锁对象指针。
	return (sp);
}

// 释放自定义互斥锁资源。
void
s_free(struct slock *sp)
{
	// 关闭对信号量的引用。
	sem_close(sp->semp);
	// 释放自定义锁资源。
	free(sp);
}

int
s_lock(struct slock *sp)
{
	// 如果信号量是0，则阻塞。
	// 否则使信号量减1。
	return (sem_wait(sp->semp));
}

int
s_trylock(struct slock *sp)
{
	// 如果信号量是0，不会阻塞，返回-1并设置errno为EAGAIN。
	// 否则使信号量减1。
	return (sem_trywait(sp->semp));
}

int
s_unlock(struct slock *sp)
{
	// 使信号量加1，如果调用sem_post前，sem_wait
	// 或sem_timedwait正处于阻塞状态，则调用sem_post
	// 后阻塞状态解除，被sem_post加1的信号量会再次被
	// sem_wait或sem_timedwait减1。
	return (sem_post(sp->semp));
}

// 本例演示了基于信号量的互斥原语的实现。
