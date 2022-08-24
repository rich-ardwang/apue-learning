#include "apue.h"

int
main(void)
{
	// 进程ID
	pid_t		pid;

	// 父进程fork子进程A
	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid != 0) {		/* parent */
		// 该父进程睡眠2秒
		sleep(2);
		// 父进程退出，设置退出状态码为2。
		exit(2);				/* terminate with exit status 2 */
	}

	// 上面的子进程A继续fork出一个子进程B
	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid != 0) {		/* first child */
		// 子进程A睡眠4秒
		sleep(4);
		// 让该子进程A异常退出（引发core dump核心转储式退出）
		abort();				/* terminate with core dump */
	}

	// 上述子进程B继续fork出一个子进程C
	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid != 0) {		/* second child */
		// 子进程B调用exec执行dd命令，把/etc/passwd复制到黑洞(/dev/null)。
		execl("/bin/dd", "dd", "if=/etc/passwd", "of=/dev/null", NULL);
		// 设置子进程B的退出状态码为7后退出。
		exit(7);				/* shouldn't get here */
	}

	// 上述子进程C继续fork出最后一个子进程D
	if ((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid != 0) {		/* third child */
		// 子进程C睡眠8秒
		sleep(8);
		// 子进程C正常退出，冲洗标准IO流。
		exit(0);				/* normal exit */
	}

	// 子进程D睡眠6秒
	sleep(6);					/* fourth child */
	// 给该子进程D发送SIGKILL信号，让其终止，不要core dump。
	kill(getpid(), SIGKILL);	/* terminate w/signal, no core dump */
	// 设置退出状态码为6后退出。
	exit(6);					/* shouldn't get here */
}

// 点评
// 本例特意为演示进程会计的概念而设计，执行本例后让其产生进程会计信息，然后使用8_14_218
// 程序去读取保存进程会计信息的文件，抽取出我们感兴趣的数据展示出来。

// 进程会计
// 进程会计功能开启时，每当一个进程结束时，内核就会写一个会计信息，记录在一个二进制文件中，
// 这些信息一般包括：
// 1.命令名称
// 2.所使用的CPU时间总量
// 3.用户ID和组ID
// 4.启动时间等。

// 进程会计安装、开启与禁用
// 1.API函数acct负责启用和禁用进程会计，唯一使用这个函数的是accton命令，只有超级用户可以
// 执行这个命令，开启后进程会计信息会写入默认指定的二进制文件中。
// 2.FreeBSD和MacOS中，该文件通常是/var/account/acct，Linux中，该文件通常是/var/account/pacct，
// Solaris中，该文件通常是/var/adm/pacct。
// 3.一般系统默认不会安装进程会计功能，需要我们手动进行安装，以Ubuntu 1604为例，我们执行
// sudo apt install acct，这样就可以完成进程会计功能的安装。
// 4.Ubuntu系统中默认保存进程会计信息的文件位置是/var/log/account/pacct。
// 5.开启：sudo accton on 或者 sudo accton /var/log/account/pacct。
// 6.禁用：sudo accton off
// 7.Dump出pacct：dump-acct /var/log/account/pacct。

// 进程会计注意事项
// 1.只有进程终止，内核才能为它写一个会计记录，对于永远不终止的进程（例如守护进程）是不能获取
// 到其进程会计信息的。
// 2.在会计文件中记录的顺序对应于进程终止的顺序，而不是它们的启动顺序。
// 3.我们通过会计记录只能知道进程的启动时间和终止顺序，并不能知道进程的终止时间。
// 4.会计纪律对应于进程，而不是程序，在fork之后，内核为子进程初始化一个记录，而不是在一个新
// 程序被执行时初始化。
// 5.fork创建会计记录，而exec不会创建会计记录，但exec后AFORK标志被清除了，这意味着，
// (A exec B, B exec C, 最后C exit)，只会写一个会计记录，在记录中的命令名是程序C，但CPU
// 时间是三个程序之和。

// 实验
// 1.使用root身份安装acct并开启进程会计功能。
// 2.执行本程序，等待一段时间，待所有子进程都退出后，此时应该至少有一个超级用户shell、一个
// 父进程和4个子进程的会计信息被记录在会计文件（二进制文件）中。
// 3.我们可以使用dump-acct /var/log/account/pacct来查看，这会看到最全面的会计信息。
// 4.我们也可以./8_14_218 /var/log/account/pacct来查看，该程序可以读取会计文件，并抽取
// 出我们感兴趣的字段。
