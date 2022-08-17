#include "apue.h"

// 定义操作标识，表示进行存储token的操作。
#define TOK_ADD		5

// 全局函数声明
void	do_line(char *);
void	cmd_add(void);
int		get_token(void);

int
main(void)
{
	// 指定fgets读取一行的最大长度
	char		line[MAXLINE];

	// fgets()函数从标准输入流读取数据，每次读取一行。
	while (fgets(line, MAXLINE, stdin) != NULL)
		// 读完一行数据后立即送到do_line()函数去执行。
		do_line(line);
	exit(0);
}

// 全局指针变量，存储输入参数，该参数应该参与生成token的计算。
char *tok_ptr;			/* global pointer for get_token() */

void
do_line(char *ptr)		/* process one line of input */
{
	int		cmd;

	// 保存输入参数，该参数参与生成token的计算。
	tok_ptr = ptr;
	// 根据输入参数的个数，循环计算生成token并保存。
	while ((cmd = get_token()) > 0) {
		switch (cmd) {		/* one case for each command */
		// 保存生成好的token。
		case TOK_ADD:
			cmd_add();
			break;
		}
	}
}

// 该函数负责存储token
void
cmd_add(void)
{
	int		token;

	// 假设可以利用get_token()函数取得token。
	token = get_token();
	/* rest of processding for this command */
	// 取得token后可以将tok_ptr置空了。
	tok_ptr = NULL;
	// 下面操作应该是将取得的token存储起来，这里并未实现，仅仅是打印一条语句。
	printf("token stored.\n");
}

// 该函数可以使用一定的算法计算出token
int
get_token(void)
{
	/* fetch next token from line pointed to by tok_ptr */
	// 如果tok_ptr有效，则进行token的生成操作。
	if (tok_ptr) {
		// 下面应该通过一系列算法生成token，这里没有实现，
		// 仅仅打印出一条语句。
		printf("token generated.\n");
		// 返回操作标识，指定进行存储token的操作。
		return TOK_ADD;
	}
	else
		return -1;
}

// 点评
// 该例子完全是一个程序框架，并没有实现更多业务细节，而仅仅是打印出语句提示业务步骤。
// 作者写该程序的目的仅仅是向大家展示程序调用栈，该程序执行时在高地址处main()函数先压入栈，
// 然后向低地址方向扩展，get_token()函数入栈，然后cmd_add()函数入栈。
// 此时，如果我们想要从栈顶的cmd_add()函数立即跳入栈底的main()函数，靠goto语句是不行的，
// 因为c语言的goto语句不能跨越函数，它只能在一个函数栈内跳来跳去。

// 基于上面的分析，通过这个例子，可以引出setjmp()和longjmp()函数，因为这两个函数可以实现
// 跨越函数栈的跳转执行，它可以从栈顶跳到多层嵌套的很深的栈底去执行，即可以在函数栈之间自由
// 跳跃执行。
