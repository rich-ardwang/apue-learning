#include "apue.h"
#include <fcntl.h>
#include <sys/mman.h>

// 设置循环次数。
#define NLOOPS		1000
// 用于内存映射，仅分配一个long型大小的空间。
#define SIZE		sizeof(long)	/* size of shared memory area */

// 每次加1的自增函数。
static int
update(long *ptr)
{
	return ((*ptr)++);		/* return value before increment */
}

int
main(void)
{
	int		fd, i, counter;
	pid_t	pid;
	void	*area;

	// 以读写模式打开文件/dev/zero，这其实是一个字符设备文件，
	// 使用它来进行内存映射(memory map)的目的是不必存在一个
	// 真实的文件，即可完成内存映射的功能。
	if ((fd = open("/dev/zero", O_RDWR)) < 0)
		err_sys("open error");
	// 使用mmap在内存映射区域申请一块空间与/dev/zero关联，空间
	// 大小为一个long型尺寸，读写模式，进程共享。
	if ((area = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
		fd, 0)) == MAP_FAILED)
		err_sys("mmap error");
	// 内存映射成功后可以将文件关闭。
	close(fd);		/* can close /dev/zero now that it's mapped */

	// 初始化父子进程同步机制。
	TELL_WAIT();

	// fork进程。
	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid > 0) {			/* parent */
		// 父进程从0开始，步长为2循环。
		for (i = 0; i < NLOOPS; i += 2) {
			// 父进程对内存映射区存储的long型数据加1。
			if ((counter = update((long *)area)) != i)
				err_quit("parent: expected %d, got %d", i, counter);
			// 给子进程发信号，通知它动作。
			TELL_CHILD(pid);
			// 阻塞等待子进程，若收到信号解除阻塞。
			WAIT_CHILD();
		}
	} else {						/* child */
		// 子进程从1开始，步长为2循环。
		for (i = 1; i < NLOOPS + 1; i += 2) {
			// 阻塞等待父进程，若收到信号解除阻塞。
			WAIT_PARENT();
			// 子进程对内存映射区存储的long型数据加1。	
			if ((counter = update((long *)area)) != i)
				err_quit("child: expected %d, got %d", i, counter);
			// 给父进程发信号，通知它动作。
			TELL_PARENT(getppid());
		}
		// 子进程退出，不冲洗缓冲区。
		_exit(0);
	}

	// 父进程打印出最终的结果。
	printf("[parent] memroy map:[ value:%ld, address:%p ]\n", *(long *)area, (long *)area);
	exit(0);
}
