#include "apue.h"
#include <setjmp.h>

// 自定义全局静态函数声明
static void		f1(int, int, int, int);
static void		f2(void);

// 跨栈跳转(或跨栈回滚)所必需的专用缓冲区
static jmp_buf	jmpbuffer;
// 全局静态变量
static int		globval;

int
main(void)
{
	int				autoval;  // 自动变量
	register int	regival;  // 寄存器变量
	volatile int	volaval;  // 易失变量
	static int		statval;  // 静态变量

	// 初始化5种不同类型变量的值
	globval = 1; autoval = 2; regival = 3; volaval = 4; statval = 5;

	// 设置跨栈回滚点
	if (setjmp(jmpbuffer) != 0) {
		// 当longjmp()被调用后，进程栈会回滚到这里继续执行。
		printf("after longjmp:\n");
		// 打印出5种不同类型变量的值，观察它们值的变化。
		printf("globval = %d, autoval = %d, regival = %d,"
			" volaval = %d, statval = %d\n",
			globval, autoval, regival, volaval, statval);
		// 正常退出
		exit(0);
	}

	/*
	 * Change variables after setjmp, but before longjmp.
	 */
	// 当setjmp()被调用后，改变5种不同类型变量的值，使它们和初始值不一样。
	globval = 95; autoval = 96; regival = 97; volaval = 98;
	statval = 99;

	// 执行f1()函数，进入main()的下一个函数栈f1()。
	f1(autoval, regival, volaval, statval);		/* never returns */
	// 正常退出
	exit(0);
}

static void
f1(int i, int j, int k, int l)
{
	// 报告在函数f1()中，并打印出5中不同类型变量的值。
	printf("in f1():\n");
	printf("globval = %d, autoval = %d, regival = %d,"
		" volaval = %d, statval = %d\n", globval, i, j, k, l);
	// 调用f2()，进入f1()的下一个函数栈f2()。
	f2();
}

static void
f2(void)
{
	// 在f2()函数栈中调用longjmp()，回滚到main()函数栈的setjmp()处。
	longjmp(jmpbuffer, 1);
}

// 点评
// 本程序的目的是考察不同类型的变量值是否受跨栈回滚影响，在调用setjmp()之前首先
// 初始化各类变量的值，在调用setjmp()之后我们为各种类型的变量赋值，然后在调用
// longjmp()之前将这些值打印出来观察是否变化，在调用longjmp()后，跨栈回滚到
// setjmp()设置的回滚点处，最后在回滚点处再打印出各类变量的值进行观察。

// 实验
// 1.首先gcc普通编译，然后执行本程序，输出如下：
// in f1():
// globval = 95, autoval = 96, regival = 97, volaval = 98, statval = 99
// after longjmp:
// globval = 95, autoval = 96, regival = 97, volaval = 98, statval = 99
// 可见各类变量的值在跨栈回滚后并未改变，也就是说变量值没有回滚。
// 2.进行优化编译gcc -O 7_10_173.c -o 7_10_173 -lapue，然后执行程序，输出如下：
// in f1():
// globval = 95, autoval = 96, regival = 97, volaval = 98, statval = 99
// after longjmp:
// globval = 95, autoval = 2, regival = 3, volaval = 98, statval = 99

// 实验结果
// 1.在没有优化编译的情况下，各种类型的变量的值不受跨栈回滚影响，也就是说跨栈回滚发生
// 后，这些类型变量的值不会回滚，仍然保持之前的设置。
// 2.在优化编译后，自动变量和寄存器变量会受到跨栈回滚的影响，当跨栈回滚发生后，自动变量
// 和寄存器变量回滚到setjmp()被调用前的状态。

// 实验结果分析
// 1.自动变量和寄存器变量在优化编译时会被放到寄存器中存储，这样跨栈回滚后其值也会回滚，
// 但是非优化编译时，这两种变量会被放在存储器中保存，跨栈回滚后其值不受影响。
// 2.全局变量、全局静态变量、易失变量和静态变量不管是否优化编译，都会被放在存储器中保存，因此跨栈
// 回滚对他们不产生影响。
