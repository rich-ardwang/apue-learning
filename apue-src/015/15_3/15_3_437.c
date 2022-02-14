#include "apue.h"
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>

/*
 * Pointer to array allocated at run-time.
 */
// 子进程的进程ID的数组。
static pid_t *childpid = NULL;

/*
 * From our open_max(), {Figure 2.17).
 */
// 最大打开文件数。
static int		maxfd;

FILE *
popen(const char *cmdstring, const char *type)
{
	int		i;
	int		pfd[2];
	pid_t	pid;
	FILE	*fp;

	/* only allow "r" or "w" */
	// 仅接受type值为'r'或'w'，其他报错并返回空指针。
	if ((type[0] != 'r' && type[0] != 'w') || type[1] != 0) {
		errno = EINVAL;
		return (NULL);
	}

	// 首次调用时会进入。
	if (childpid == NULL) {		/* first time through */
		/* allocate zeroed out array for child pids */
		// 取得该系统的最大打开文件数。
		maxfd = open_max();
		// 在堆上分配maxfd个pid_t大小的空间，也就是pid_t的数组，
		// 然后让childpid指向该数组的地一个元素。
		if ((childpid = calloc(maxfd, sizeof(pid_t))) == NULL)
			return (NULL);
	}

	// 创建匿名管道。
	if (pipe(pfd) < 0)
		return (NULL);		/* errno set by pipe() */
	// 如果管道的输入端pdf[0]和输出端pdf[1]其中有一个大于最大
	// 打开文件数，则报错并返回空指针。
	if (pfd[0] >= maxfd || pfd[1] >= maxfd) {
		close(pfd[0]);
		close(pfd[1]);
		errno = EMFILE;
		return (NULL);
	}

	// fork进程。
	if ((pid = fork()) < 0) {
		return (NULL);		/* errno set by fork() */
	} else if (pid == 0) {		/* child */
		// 在子进程中，根据type的读写模式，设置相应的管道数据传输方向。
		if (*type == 'r') {
			// 如果是读模式，子进程关闭管道输入pfd[0]，保留管道输出pfd[1]。
			close(pfd[0]);
			// 将管道输出端pfd[1]替换为stdout，然后关闭旧的文件描述符合pfd[1]。
			if (pfd[1] != STDOUT_FILENO) {
				dup2(pfd[1], STDOUT_FILENO);
				close(pfd[1]);
			}
		} else {
			// 如果是写模式，子进程关闭管道输出pfd[1]，保留管道输入pfd[0]。
			close(pfd[1]);
			// 将管道输入端pfd[0]替换为stdin，然后关闭旧的文件描述符合pfd[0]。
			if (pfd[0] != STDIN_FILENO) {
				dup2(pfd[0], STDIN_FILENO);
				close(pfd[0]);
			}
		}

		/* close all descriptors in childpid[] */
		// 根据最大打开文件数，关闭所有子进程所持有的相应的文件描述符。
		for (i = 0; i < maxfd; i++)
			if (childpid[i] > 0)
				close(i);

		// 子进程执行execl系列函数，执行shell命令建立一个全新的进程。
		execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
		// 子进程不冲刷缓冲区退出。
		_exit(127);
	}

	/* parent continues... */
	// 父进程根据type设置的读写模式，设置相应的管道的数据传输方向。
	if (*type == 'r') {
		// 如果是读模式，父进程关闭管道输出pfd[1]，保留管道输入pfd[0]。
		close(pfd[1]);
		// 父进程调用fdopen以只读模式打开文件pfd[0]。
		if ((fp = fdopen(pfd[0], type)) == NULL)
			return (NULL);
	} else {
		// 如果是写模式，父进程关闭管道输入pfd[0]，保留管道输出pfd[1]。
		close(pfd[0]);
		// 父进程调用fdopen以只写模式打开文件pfd[1]。
		if ((fp = fdopen(pfd[1], type)) == NULL)
			return (NULL);
	}

	// 将fp这个FILE*指针利用系统函数fileno转换成file编号，
	// 然后使用这个file编号为数组下标，保存相应的子进程的进程ID。
	childpid[fileno(fp)] = pid;		/* remember child pid for this fd */
	// 将FILE*指针返回。
	return (fp);
}

int
pclose(FILE *fp)
{
	int			fd, stat;
	pid_t		pid;

	// 如果子进程的pid数组为空，则报错并返回-1。
	if (childpid == NULL) {
		errno = EINVAL;
		return (-1);			/* popen() has never been called */
	}

	// 根据FILE*文件指针取得相应的文件描述符。
	fd = fileno(fp);
	// 如果文件描述符大于等于最大打开文件数，
	// 则报错并返回-1。
	if (fd >= maxfd) {
		errno = EINVAL;
		return (-1);			/* invalid file descriptor */
	}
	// 根据文件描述符索引找到相应的子进程的进程ID，
	// 如果等于0，则报错并返回-1。
	if ((pid = childpid[fd]) == 0) {
		errno = EINVAL;
		return (-1);			/* fp wan't opened by popen() */
	}

	// 将这个文件描述符对应的子进程的pid数组中的相应元素
	// 重置为0。
	childpid[fd] = 0;
	// 关闭文件，如果出错则返回-1。
	if (fclose(fp) == EOF)
		return (-1);

	// 阻塞等待该文件描述符对应的子进程退出，
	// 并取得其退出状态。
	while (waitpid(pid, &stat, 0) < 0);
		if (errno != EINTR)
			return (-1);		/* error other than EINTR from waitpid() */

	// 返回子进程的退出状态。
	return (stat);				/* return child's termination status */
}

// 点评：
// 这是作者自己实现的popen和pclose函数，并不是
// <stdio.h>提供的。
