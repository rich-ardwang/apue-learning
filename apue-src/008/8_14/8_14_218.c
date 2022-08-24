#include "apue.h"
#include <sys/acct.h>

// 如下是针对FreeBSD平台的定义
#if defined(BSD)	/* different structure in FreeBSD */
// acct版本为acctv2
#define acct acctv2
// ac_flag定义为ac_trailer.ac_flag
#define ac_flag ac_trailer.ac_flag
// 输出格式如下
#define FMT "%-*.*s e = %.0f, chars = %.0f, %c %c %c %c \n"
// 非FreeBSD平台，查看stat信息需要编译时定义HAS_AC_STAT
#elif defined(HAS_AC_STAT)
#define FMT "%-*.*s e = %6ld, chars = %7ld, stat = %3u: %c %c %c %c \n"
// 所有非FreeBSD平台
#else
// 所有非FreeBSD平台只定义了输出格式
#define FMT "%-*.*s e = %6ld, chars = %7ld, %c %c %c %c \n"
#endif

// 如下是针对Linux平台的定义
#if defined(LINUX)
// acct版本为acct_v3
#define acct acct_v3	/* different structure in Linux */
#endif

// 如果没有定义HAS_ACORE，那么定义ACORE为0。
#if !defined(HAS_ACORE)
#define ACORE 0
#endif
// 如果没有定义HAS_AXSIG，那么定义AXSIG为0。
#if !defined(HAS_AXSIG)
#define AXSIG 0
#endif

// 如下函数是给所有非FreeBSD平台使用的
#if !defined(BSD)
static unsigned long
// 这个函数将comp_t类型转换为unsigned long
compt2ulong(comp_t comptime)	/* convert comp_t to unsigned long */
{
	unsigned long		val;
	int					exp;

	val = comptime & 0x1fff;	/* 13-bit fraction */
	exp = (comptime >> 13) & 7;	/* 3-bit exponent (0-7) */
	while (exp-- > 0)
		val *= 8;
	return (val);
}
#endif

int
main(int argc, char *argv[])
{
	// 进程会计信息结构体
	struct acct			acdata;
	// 标准IO的文件指针
	FILE				*fp;

	// 只接受一个参数，该参数为pacct文件pathname。
	if (argc != 2)
		err_quit("usage: pracct filename");
	// 调用fopen()以只读方式打开pacct文件，该函数返回pacct的文件指针。
	if ((fp = fopen(argv[1], "r")) == NULL)
		// 处理文件打开失败。
		err_sys("can't open %s", argv[1]);
	// 调用fread()函数开始循环读取pacct这个二进制文件。
	// 参数：
	// 1.该二进制文件的解释方式（结构体填充）。
	// 2.一个结构体的大小。
	// 3.每次循环读取一个完整的结构体信息。
	// 4.文件IO流指针。
	while (fread(&acdata, sizeof(acdata), 1, fp) == 1) {
		// 格式化输出抽取出来的我们感兴趣的信息。
		// 下面是对进程名的格式化输出。
		printf(FMT, (int)sizeof(acdata.ac_comm),
			(int)sizeof(acdata.ac_comm), acdata.ac_comm,
			// 下面是对ac_etime和ac_io的格式化输出，对于FreeBSD和其他分别定义。
#if defined(BSD)
			acdata.ac_etime, acdata.ac_io,
#else
			compt2ulong(acdata.ac_etime), compt2ulong(acdata.ac_io),
#endif
			// 下面是针对ac_stat的格式化输出。
#if defined(HAS_AC_STAT)
			(unsigned char) acdata.ac_stat,
#endif
			// 下面是输出ac_flag的不同类型。
			acdata.ac_flag & ACORE ? 'D' : ' ',
			acdata.ac_flag & AXSIG ? 'X' : ' ',
			acdata.ac_flag & AFORK ? 'F' : ' ',
			acdata.ac_flag & ASU ? 'S' : ' ');
	}
	// 调用ferror检查文件读取是否出错并处理。
	if (ferror(fp))
		err_sys("read error");
	// 正常退出，冲洗标准IO流。
	exit(0);
}

// 点评

// 编译本程序注意事项
// 需要加入-DLINUX这个定义，否则由于acct的版本信息不一致，会导致文件读取错乱，
// Linux系统中acct的版本是acct_v3。
// 编译命令：
// gcc -DLINUX 8_14_218.c -o 8_14_218 -lapue

// 实验结果
/*
accton           e =      0, chars =       0,       S
sh               e =    102, chars =       0, 
dd               e =      0, chars =       0,
8_14_217         e =    200, chars =       0,
8_14_217         e =    421, chars =       0,     F   
8_14_217         e =    600, chars =       0,     F   
8_14_217         e =    800, chars =       0,     F
8_14_218         e =      0, chars =       0,
*/
