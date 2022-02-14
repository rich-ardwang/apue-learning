#include "apue.h"
#include <poll.h>
#include <pthread.h>
#include <sys/msg.h>
#include <sys/socket.h>

// XSI消息队列的个数。
#define NQ			3		/* number of queues */
// XSI消息队列中每条消息的最大字节数。
#define MAXMSZ		512		/* maximum message size */
// 首个XSI消息队列的键值。
#define KEY			0x123	/* key for first message queue */

// 线程信息，保存fd与qid的映射关系。
struct threadinfo {
	int		qid;	// XSI消息队列ID。
	int		fd;		// 文件描述符。
};

// XSI消息结构。
struct mymesg {
	long	mtype;			// XSI消息类型。
	char	mtext[MAXMSZ];	// XSI消息内容。
};

// 线程函数。
void *
helper(void *arg)
{
	int					n;
	struct mymesg		m;
	struct threadinfo	*tip = arg;

	for (;;) {
		memset(&m, 0, sizeof(m));
		if ((n = msgrcv(tip->qid, &m, MAXMSZ, 0, MSG_NOERROR)) < 0)
			err_sys("msgrcv error");
		if (write(tip->fd, m.mtext, n) < 0)
			err_sys("write error");
	}
}

int
main()
{
	int					i, n, err;
	// UNIX域套接字使用的全双工管道描述符。
	int					fd[2];
	// XSI消息队列数组，每个元素代表一个消息队列。
	int					qid[NQ];
	// IO多路转接Poll技术使用的结构体。
	struct pollfd		pfd[NQ];
	// 线程信息。
	struct threadinfo	ti[NQ];
	// 线程ID，每根线程负责一个XSI消息队列。
	pthread_t			tid[NQ];
	// 系统调用read使用的缓冲区。
	char				buf[MAXMSZ];

	// 循环创建三个XSI消息队列和三个线程，每个队列对应一组UNIX域套接
	// 字的全双工管道，然后把消息队列ID和UNIX域套接字的管道输出端相关
	// 联，再把这些关联信息传入线程函数。
	// 每个线程负责一个XSI消息队列，它们阻塞等待从自己负责的消息队列中
	// 读取消息，有消息到来后解除阻塞并将消息写入UNIX域套接字的输出端
	// fd[1]。
	for (i = 0; i < NQ; i++) {
		// 创建3个XSI消息队列并保存在数组中。
		// 参数1：消息队列的键值。
		// 参数2：消息队列标志位，IPC_CREAT表示创建，0666是设置消息
		// 队列权限位，表示用户读写、组读写、其他读写。
		if ((qid[i] = msgget((KEY+i), IPC_CREAT|0666)) < 0)
			err_sys("msgget error");

		// 打印出队列的索引ID和msgget分配的队列编号。
		printf("queue ID %d is %d\n", i, qid[i]);

		// 创建UNIX域套接字，使用数据报类型而不是流套接字，这样做是为了保
		// 持消息边界，一次从套接字中读取一条消息。
		if (socketpair(AF_UNIX, SOCK_DGRAM, 0, fd) < 0)
			err_sys("socketpair error");
		// 将poll中的fd指定为UNIX域套接字管道的输入端fd[0]。
		pfd[i].fd = fd[0];
		// 设置poll的输入事件为POLLIN，告诉内核我们感兴趣的事件是POLLIN。
		// POLLIN指示可以不阻塞的读高优先级以外的数据，即普通数据和优先级
		// 数据。
		pfd[i].events = POLLIN;
		// 将UNIX域套接字管道的输出端fd[1]和XSI消息队列编号相关联。
		ti[i].qid = qid[i];
		ti[i].fd = fd[1];
		// 创建线程，将threadinfo传入线程函数。
		if ((err = pthread_create(&tid[i], NULL, helper, &ti[i])) != 0)
			err_exit(err, "ptherad_create error");
	}

	for (;;) {
		// poll阻塞等待有符合要求的事件发生，一旦poll返回，如果返回值n大于0,
		// 意味着pfd数组中有n个描述符已经准备好，可以进行读取。
		if (poll(pfd, NQ, -1) < 0)
			err_sys("poll error");
		// poll函数返回值非负，意味着已经有0～3个fd准备好，可以进行读取。
		// 对这些描述符进行轮询。
		for (i = 0; i < NQ; i++) {
			// 一旦有events中设置的flag事件发生，poll也会将这些事件放在
			// 输出事件revents中，我们只要让revents与感兴趣的flag进行按
			// 位与操作，即可进行判断这个flag是否发生。
			if (pfd[i].revents & POLLIN) {
				// 文件描述符已经可以被读取，将其读取到缓冲区中。
				if ((n = read(pfd[i].fd, buf, sizeof(buf))) < 0)
					err_sys("read error");
				// 设置消息尾部\0。
				buf[n] = 0;
				// 将队列编号和消息输出到stdout。
				printf("queue id %d, message %s\n", qid[i], buf);
			}
		}
	}

	// 正常退出进程。
	exit(0);
}
