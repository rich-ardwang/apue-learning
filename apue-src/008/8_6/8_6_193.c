#include "apue.h"
#include <sys/wait.h>

int
main(void)
{
	// 进程ID
	pid_t		pid;

	// 父进程fork儿子进程
	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid == 0) {			/* first child */
		// 儿子进程再fork孙子进程
		if ((pid = fork()) < 0)
			err_sys("fork error");
		else if (pid > 0)
			// 儿子进程正常退出
			exit(0);	/* parent from second fork == first child */

		/*
		 * We're the second child; our parent becomes init as soon
		 * as our real parent calls exit() in the statement above.
		 * Here's where we'd continue executing, knowing that when
		 * we're done, init will reap our status.
		 */
		// 孙子进程睡眠2秒，这是为了保证他的父进程已经调用exit(0)退出。
		// 当孙子进程的父亲，也就是儿子进程调用exit(0)退出时，孙子进程立即变成
		// 孤儿，此时这个进程会被Unix系统的init进程收养。
		sleep(2);
		// The parent becomes /sbin/upstart on Ubuntu1604, not init.
		// Upstart take charge in adopting orphans for the GUI of Ubuntu,
		// and if it is killed the GUI user will be kicked out.
		// If we kill the upstart on text shell, the parent will be init.
		// 孙子进程打印出自己的父进程ID。
		// 这里可以看到孙子进程的父进程ID为1841，也就是Ubuntu1604 GUI的/sbin/upstart
		// 进程，说明它被upstart进程收养了，如果把upstart杀掉，所有GUI用户将被踢出系统，
		// 如果切换为tty终端界面，那么收养者将变成init进程。
		printf("second child, parent pid = %ld\n", (long)getppid());
		// 孙子进程正常退出。
		exit(0);
	}

	// 父进程调用waitpid专门阻塞等待儿子进程退出。
	if (waitpid(pid, NULL, 0) != pid)	/* wait for first child */
		err_sys("waitpid error");

	/*
	 * We're the parent (the original process); we continue executing,
	 * knowing that we're not the parent of the second child.
	 */
	// 儿子进程退出后，父进程正常退出，不管其孙子进程是否退出。
	exit(0);
}

// 点评
// 这个程序试图让我们熟悉僵尸进程，孤儿进程，进程收养等概念。

// 僵尸进程
// 一般父进程fork子进程后，要调用wait()或waitpid()等待子进程退出，
// 然后收集子进程的退出信息（俗语说为子进程收尸），之后内核就可以完全
// 释放由子进程所占的资源。
// 但是，如果父进程不调用wait()或waitpid()等函数，也就是他不给子进程
// 善后，那么子进程退出后将变成僵尸进程，退出状态为Z（使用top或ps等可查看），
// 内核会保留一些僵尸进程所占的资源，并不会完全释放，而我们又无法使用kill等
// 方式杀死该进程，因为理论上说这个进程已经是死亡了的。

// 进程收养
// 如果父进程fork子进程后先于子进程退出（或者俗话说父进程先死亡了），那么当
// 父进程调用exit后，子进程变成孤儿进程，之后将会被Unix系统进程init进程收养，
// 如果是GUI界面的话，可能会有其他专门负责收养孤儿进程的进程，如upstart等。
// 子进程被收养后就不会再变成僵尸进程了，因为子进程退出后，init进程一定会调用
// wait()为其善后。

// wait和waitpid
// wait()是阻塞式等待子进程退出，然后获取其退出状态，有任何一个子进程退出后，
// wait就会返回，然后调用者需要判断其pid是不是自己要等待的子进程。
// waitpid()提供了wait函数没有提供的3个功能：
// 1).waitpid可等待一个特定的进程，而wait则返回任一终止子进程的状态。
// 2).waitpid提供了一个wait的非阻塞版本，有时希望获取一个子进程的状态，
// 但不想阻塞。
// 3).waitpid通过WUNTRACED和WCONTINUED选项支持作业控制。

// waitid、wait3和wait4
// waitid()类似与waitpid，但提供了更多的灵活性。
// wait3和wait4源自UNIX BSD分支，它们提供的功能比wait、waitpid、waitid
// 所提供的功能要多一个，这与附加参数有关，该参数允许内核返回由终止进程及其所有
// 子进程使用的资源概况。
