#include "apue.h"
#include <sys/shm.h>

#define ARRAY_SIZE		40000
#define MALLOC_SIZE		100000
#define SHM_SIZE		100000
#define SHM_MODE		0600	/* usr read/write */

// 4万个char元素的数组，未初始化，在BSS段。
char array[ARRAY_SIZE];		/* uninitiallized data = bss */

int
main(void)
{
	// 函数内自动变量，在栈段分配。
	int		shmid;
	char	*ptr, *shmptr;

	// 可以查看4万个char元素的数组在BSS段的地址范围。
	printf("array[] from %p to %p\n", (void *)&array[0],
		(void *)&array[ARRAY_SIZE]);
	// 查看自动变量在栈段的地址。
	printf("stack around %p\n", (void *)&shmid);

	// 在堆上分配10万个字节的空间。
	if ((ptr = malloc(MALLOC_SIZE)) == NULL)
		err_sys("malloc error");
	// 查看堆段的地址范围。
	printf("malloced from %p to %p\n", (void *)ptr,
		(void *)ptr+MALLOC_SIZE);

	// 在共享内存段分配10万个字节的空间。
	if ((shmid = shmget(IPC_PRIVATE, SHM_SIZE, SHM_MODE)) < 0)
		err_sys("shmget error");
	// 取得共享内存段的首地址。
	if ((shmptr = shmat(shmid, 0, 0)) == (void *)-1)
		err_sys("shmat error");
	// 查看共享内存段的地址范围。
	printf("shared memory attched from %p to %p\n", (void *)shmptr,
		(void *)shmptr+SHM_SIZE);
	// 释放已分配的10万个字节的共享内存。
	if (shmctl(shmid, IPC_RMID, 0) < 0)
		err_sys("shmctl error");
	
	// 释放堆内存。
	if (ptr)
		free(ptr);

	exit(0);
}

// 本例主要打印各种类型的数据存放的位置。
