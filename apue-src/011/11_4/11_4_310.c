#include "apue.h"
#include <pthread.h>

// 线程ID，只有在它所属的进程的上下文中才有意义，且是唯一的。
pthread_t	ntid;

// 格式化打印，可以输出每个线程所对应的线程编号、进程ID、线程ID等。
void
printids(const char *s)
{
	// 进程ID，在整个操作系统中唯一。
	pid_t		pid;
	// 线程ID，在它所属的进程中唯一。
	pthread_t	tid;

	// 获取进程ID
	pid = getpid();
	// 新线程使用pthread_self函数获取自己的线程ID。
	// 这种方法比通过pthread_create以共享内存方式传递过来的线程ID更为有效，
	// 当主线程在创建新线程时，它会把新线程的线程ID写入到全局变量ntid，这样
	// 新线程通过读取全局变量似乎也可以获取到线程ID，但是如果新线程在主线程
	// 调用pthread_create之前就开始运行了，那么它得到的将是未初始化的ntid。
	tid = pthread_self();
	// 输出线程编号、进程ID、线程ID（以十进制和十六进制查看）。
	printf("%s pid %lu tid %lu (0x%lx)\n", s, (unsigned long)pid,
		(unsigned long)tid, (unsigned long)tid);
}

// 线程函数，当创建一个线程时指定，此线程开始工作时就会调用这个线程函数。
void *
thr_fn(void *arg)
{
	// msg数组
	char msg[20];
	// arg是任意数据结构指针，这里转换成int指针，用于传递线程编号参数。
	// 将含有线程编号的消息写入msg数组。
	sprintf(msg, "new thread[%d]: ", *((int *)arg));
	// 调用printids对msg进行格式化打印。
	printids(msg);
	// 线程函数执行完毕后正常返回，之后线程可以正常终止。
	return ((void *)0);
}

int
main(void)
{
	// 返回值
	int		err;
	// 线程编号数组
	int nseq[10];

	// 主线程执行循环创建各个新线程
	for (int i=0; i<10; i++) {
		// 保存线程编号
		nseq[i] = i+1;
		// 创建新线程，把新线程ID写入全局变量ntid，然后将线程编号通过参数传递给线程函数。
		// 注意：创建新线程结束后，新线程立即执行线程函数，这个过程可能在pthread_create
		// 函数返回前，也可能在pthread_create返回后，这是不确定的。
		err = pthread_create(&ntid, NULL, thr_fn, (void *)&nseq[i]);
		// 返回值非0意味着创建线程失败。
		if (err != 0)
			// 出错并结束进程
			err_exit(err, "can't create thread");
	}
	// 格式化打印出主线程所对应的进程ID、线程ID等。
	printids("main thread:");
	// 这里让主线程休眠1s，是等待所有线程都创建完毕并执行完线程函数。
	// 如果不等待，主线程结束后，进程就结束了，可能其他线程还没机会被创建或执行线程函数。
	sleep(1);
	// 正常退出，冲洗IO缓冲。
	exit(0);
}

// 点评
// 创建的每个线程执行的先后顺序是不确定的，传给每个线程
// 的序号参数存储在一个固定的数组中，每个线程使用自己的专有
// 数组元素，这样不会产生race condition。

// 这里简单处理了主线程和其他子线程之间的竞争关系，是通过让主线程休眠1秒。
// 后面有更复杂的处理手段，控制各个线程之间的竞争关系，这种称为线程同步。

// 结果
// 1.每个线程对应的进程ID都相同。
// 2.每个线程自己的线程ID都不相同。
// 3.在不同的Unix平台测试，线程ID的表现是不一样的，在Linux平台它更像指针。
// 4.主线程的线程ID可能和其他子线程的线程ID分配在相同地址空间，也可能是不
// 同的地址空间，因操作系统而异。
// 5.线程执行先后的不确定性是由于操作系统调度算法等决定的。
