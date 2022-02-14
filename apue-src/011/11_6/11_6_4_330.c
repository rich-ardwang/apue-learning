#include <stdlib.h>
#include <pthread.h>

// job是一个双向链表。
struct job {
	struct job		*j_next;
	struct job		*j_prev;
	pthread_t		j_id;		/* tells which thread handles this job */
	/* ... more stuff here ... */
};

// queue是一个工作队列，把job放在其中。
struct queue {
	struct job			*q_head;
	struct job			*q_tail;
	pthread_rwlock_t	q_lock;
};

/*
 * Initialize a queue.
 */
int
queue_init(struct queue *qp)
{
	int		err;

	qp->q_head = NULL;
	qp->q_tail = NULL;
	// 在队列初始化函数中，对工作队列的读写锁进行初始化。
	err = pthread_rwlock_init(&qp->q_lock, NULL);
	if (err != 0)
		return (err);
	/* ... continue initialization ... */
	return (0);
}

/*
 * Insert a job at the head of the queue.
 */
void
job_insert(struct queue *qp, struct job *jp)
{
	// 从工作队列头部插入元素，使用写锁进行锁定。
	pthread_rwlock_wrlock(&qp->q_lock);
	jp->j_next = qp->q_head;
	jp->j_prev = NULL;
	if (qp->q_head != NULL)
		qp->q_head->j_prev = jp;
	else
		qp->q_tail = jp;		/* list was empty */
	qp->q_head = jp;
	// 写锁解锁。
	pthread_rwlock_unlock(&qp->q_lock);
}

/*
 * Append a job on the tail of the queue.
 */
void
job_append(struct queue *qp, struct job *jp)
{
	// 从工作队列尾部插入元素，使用写锁进行锁定。
	pthread_rwlock_wrlock(&qp->q_lock);
	jp->j_next = NULL;
	jp->j_prev = qp->q_tail;
	if (qp->q_tail != NULL)
		qp->q_tail->j_next = jp;
	else
		qp->q_head = jp;		/* list was empty */
	qp->q_tail = jp;
	pthread_rwlock_unlock(&qp->q_lock);
}

/*
 * Remove the giver job from a queue.
 */
void
job_remove(struct queue *qp, struct job *jp)
{
	// 从工作队列中移除元素，使用写锁进行锁定。
	pthread_rwlock_wrlock(&qp->q_lock);
	if (jp == qp->q_head) {
		qp->q_head = jp->j_next;
		if (qp->q_tail == jp)
			qp->q_tail = NULL;
		else
			jp->j_next->j_prev = jp->j_prev;
	} else if (jp == qp->q_tail) {
		qp->q_tail = jp->j_prev;
		jp->j_prev->j_next = jp->j_next;
	} else {
		jp->j_prev->j_next = jp->j_next;
		jp->j_next->j_prev = jp->j_prev;
	}
	pthread_rwlock_unlock(&qp->q_lock);
}

/*
 * Find a job for the given thread ID.
 */
struct job *
job_find(struct queue *qp, pthread_t id)
{
	struct job		*jp;

	// 从工作队列中查找元素，使用读锁进行锁定。
	if (pthread_rwlock_rdlock(&qp->q_lock) != 0)
		return (NULL);

	for (jp = qp->q_head; jp != NULL; jp = jp->j_next)
		if (pthread_equal(jp->j_id, id))
			break;

	// 读锁解锁。
	pthread_rwlock_unlock(&qp->q_lock);
	return (jp);
}

// 读写锁有三种状态：
// 1.写模式上锁，2.读模式上锁，3.解除锁定。
// 规则如下：
// 写模式只允许唯一线程对其上锁，独占使用，锁定中会阻塞其他线程。
// 读模式为共享模式，可以有多个线程对其上锁，读取完毕后解锁。
// 当至少有一把读锁存在时，不允许上写锁，申请上写锁的线程等待。
// 有申请对写锁上锁的线程在等待时，所有申请上读锁的线程会被阻塞，待写锁上锁完毕后，才允许继续申请读锁。
