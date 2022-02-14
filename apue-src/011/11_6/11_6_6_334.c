#include <pthread.h>

// 存储消息的链表结构
struct msg {
	struct msg		*m_next;
	/* ... more stuff here ... */
};

// 简化的消息队列
struct msg		*workq;

// 静态方式初始化条件变量
pthread_cond_t qready = PTHREAD_COND_INITIALIZER;
// 静态方式初始化互斥量
pthread_mutex_t qlock = PTHREAD_MUTEX_INITIALIZER;

void
process_msg(void)
{
	struct msg		*mp;

	for ( ; ; ) {
		// 互斥锁锁定消息队列
		pthread_mutex_lock(&qlock);
		while (workq == NULL)
			// 阻塞等待接收条件变量满足后发过来的通知，
			// 条件变量由互斥锁保护。
			pthread_cond_wait(&qready, &qlock);
		// 从队列中取出消息
		mp = workq;
		workq = mp->m_next;
		// 解锁
		pthread_mutex_unlock(&qlock);
		/* now process the message mp */
	}
}

void
enqueue_msg(struct msg *mp)
{
	// 互斥锁加锁后向消息队列中加入消息。
	pthread_mutex_lock(&qlock);
	mp->m_next = workq;
	workq = mp;
	pthread_mutex_unlock(&qlock);
	// 消息队列操作完毕，发送条件变量通知。
	pthread_cond_signal(&qready);
}
