#include "apue.h"
#include <sys/wait.h>

int
main(void)
{
	// 进程ID
	pid_t		pid;

	// 父进程fork子进程
	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid == 0) {			/* child */
		// 子进程调用exec系列函数执行程序testinterp，向新进程传递程序名称和2个参数。
		if (execl("./testinterp", "testinterp", "myarg1", "MY ARG2", (char *)0) < 0)
			err_sys("execl error");
	}
	// 父进程等待子进程退出并为其善后。
	if (waitpid(pid, NULL, 0) < 0)	/* parent */
		err_sys("waitpid error");
	// 父进程退出。
	exit(0);
}

// 点评
// 本小节练习使用shell解释器。
// 解释器是脚本文件中以#!开头，#!后面接可执行二进制（ELF）程序，然后在ELF程序后可直接向其传递参数。

// 实验结果
// argv[0]: ../../007/7_4/7_4_162
// argv[1]: foo
// argv[2]: ./testinterp
// argv[3]: myarg1
// argv[4]: MY ARG2

// 结果分析
// 1.子进程调用exec执行了./testinterp，并传递了程序名和2个参数，而testinterp并不是ELF文件，它只是
// 一个带有解释器的脚本文件。
// 2.testinterp脚本文件使用解释器去执行另外一个ELF文件../../007/7_4/7_4_162，并给此程序传递了foo
// 参数。
// 3.../../007/7_4/7_4_162是一个可以回显所有命令行参数的ELF程序，所以第一个参数argv[0]就是它自己的
// 程序名称。
// argv[1]是testinterp脚本文件传递给它的参数foo。
// argv[2]是bash shell调用testinterp脚本文件传递给它的脚本程序名称。
// argv[3]和argv[4]是本程序传递给脚本文件的两个参数。

// 另一个解释器的例子
// awkexample也是一个带有解释器的脚本文件，该脚本让shell解释器调用/usr/bin/awk二进制可执行文件，并
// 传递一个参数 -f 给它。
// -f 后面还需要传递另一个参数给awk程序，这个参数使用BEGIN {...}传递。
// 之后awk程序就可以执行BEGIN中的代码了，该代码回显所有命令行参数。
