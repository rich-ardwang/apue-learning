#include "apue.h"
#include <pthread.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/types.h>
#include <bsd/stdlib.h>

// 把800万个数平均分成8组，由8个线程分别进行堆排序，然后主线程将结果合并。
#define NTHR		8				/* number of threads */
#define NUMNUM		8000000L		/* number of numbers to sort */
#define TNUM		(NUMNUM/NTHR)	/* number to sort per thread */

// 排序前的数的数组
long nums[NUMNUM];
// 排序后的数的数组
long snums[NUMNUM];

// 屏障变量
pthread_barrier_t	b;

#ifdef SOLARIS
#define heapsort qsort
#else
// 对排序算法，在<bsd/stdlib>中实现。
extern int heapsort(void *, size_t, size_t,
					int (*)(const void *, const void *));
#endif

/*
 * Compare two long integers (helper function for heapsort)
 */
// 排序算法中需要自定义的比较方法，该方法比较两个long型变量的大小，返回3种结果。
int
complong(const void *arg1, const void *arg2)
{
	long l1 = *(long *)arg1;
	long l2 = *(long *)arg2;

	if (l1 == l2)
		return 0;
	else if (l1 < l2)
		return -1;
	else
		return 1;
}

/*
 * Worker thread to sort a portion of the set of numbers.
 */
// 线程函数
void *
thr_fn(void *arg)
{
	long idx = (long)arg;

	// 参数1：被排序的目标数组中某个索引元素的指针，参数2：要排序的元素个数，
	// 参数3：被排序元素大小，参数4：自定义比较算法。
	heapsort(&nums[idx], TNUM, sizeof(long), complong);

	// 屏障等待，即等待每个线程都完成任务后，解除阻塞。
	pthread_barrier_wait(&b);

	/*
	 * Go off and perform more work ...
	 */
	return ((void *)0);
}

/*
 * Merge the results of the individual sorted ranges.
 */
// 将各个线程的排序结果进行合并处理。
void
merge()
{
	long		idx[NTHR];
	long		i, minidx, sidx, num;

	for (i = 0; i < NTHR; i++)
		idx[i] = i * TNUM;
	for (sidx = 0; sidx < NUMNUM; sidx++) {
		num = LONG_MAX;
		for (i = 0; i < NTHR; i++) {
			if ((idx[i] < (i+1)*TNUM) && (nums[idx[i]] < num)) {
				num = nums[idx[i]];
				minidx = i;
			}
		}
		snums[sidx] = nums[idx[minidx]];
		idx[minidx]++;
	}
}

int
main()
{
	unsigned long		i;
	struct timeval		start, end;
	long long			startusec, endusec;
	double				elapsed;
	int					err;
	pthread_t			tid;

	/*
	 * Create the initial set of numbers to sort.
	 */
	// 生成800万个随机数。
	srandom(1);
	for (i = 0; i < NUMNUM; i++)
		nums[i] = random();

	/*
	 * Create 8 threads to sort the numbers.
	 */
	// 计时开始。
	gettimeofday(&start, NULL);
	// 初始化线程屏障函数。
	pthread_barrier_init(&b, NULL, NTHR+1);
	// 创建8个工作线程用于堆排序，传递每个线程用到的被排序数组的起始索引。
	for (i = 0; i < NTHR; i++) {
		err = pthread_create(&tid, NULL, thr_fn, (void *)(i * TNUM));
		if (err != 0)
			err_exit(err, "can't create thread");
	}
	// 主线程也屏障等待，等待所有工作线程完成工作后，解除阻塞。
	pthread_barrier_wait(&b);
	// 将排序结果合并。
	merge();
	// 计时结束。
	gettimeofday(&end, NULL);

	/*
	 * Print the sorted list.
	 */
	// 计算所有排序工作的用时并显示。
	startusec = start.tv_sec * 1000000 + start.tv_usec;
	endusec = end.tv_sec * 1000000 + end.tv_usec;
	elapsed = (double)(endusec - startusec) / 1000000.0;
	printf("sort took %.4f seconds\n\n", elapsed);
	// 显示部分排序后的数字。
	for (i = 0; i < 100; i++) {
		printf("%ld ", snums[i]);
		if ((i % 20) == 19)
			printf("\n");
	}
	printf("\n\n");
	for (i = 50000; i < 50100; i++) {
		printf("%ld ", snums[i]);
		if ((i % 20) == 19)
			printf("\n");
	}

	exit(0);
}

// 编译命令：
// gcc 11_6_8_337.c -o 11_6_8_337 -lapue -lpthread -lbsd
