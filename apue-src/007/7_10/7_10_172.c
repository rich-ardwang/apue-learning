#include "apue.h"
#include <setjmp.h>

#define	TOK_ADD		5

// 跨栈跳转执行需要使用的专门的缓冲区
jmp_buf	jmpbuffer;

int
main(void)
{
	char		line[MAXLINE];

	// 使用setjmp()在这里设置跳转点，当longjmp()被调用后，
	// 进程会跳转回这里继续执行。（即实现跨栈跳转）
	if (setjmp(jmpbuffer) != 0)
		printf("error");
	while (fgets(line, MAXLINE, stdin) != NULL)
		do_line(line);
	exit(0);
}

void
cmd_add(void)
{
	int		token;

	token = get_token();
	if (token < 0)		/* an error has occurred */
		// 当token生成出错后，我们想要跳转回main()函数中setjmp()设置
		// 的跳转点处继续执行，这里需要调用longjmp()。
		longjmp(jmpbuffer, 1);
	/* rest of processing for this command */
}

// 点评
// 承接上个例子，本例只是代码片段，提示我们可以使用setjmp()和longjmp()实现
// 跨函数栈跳转执行，并演示了这两个函数的基本用法，提示出应该修改的代码位置。
