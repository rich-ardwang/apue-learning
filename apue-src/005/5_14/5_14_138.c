#include "apue.h"

#define BSZ 48

int
main()
{
	FILE		*fp;
	char		buf[BSZ];

	memset(buf, 'a', BSZ-2);
	buf[BSZ-2] = '\0';
	buf[BSZ-1] = 'X';
	// buf: aaa...aaa0X
	if ((fp = fmemopen(buf, BSZ, "w+")) == NULL)
		err_sys("fmemopen failed");
	// buf: nullaaa...aaa0X, offsite: 0, curLen: 0
	printf("initial buffer contents: %s\n", buf);
	fprintf(fp, "hello, world");
	printf("before flush: %s\n", buf);
	fflush(fp);
	// buf: hello, worldnullaaa...aaa0X, offsite: 12, curLen: 12
	printf("after fflush: %s\n", buf);
	printf("len of string in buf = %ld\n", (long)strlen(buf));

	memset(buf, 'b', BSZ-2);
	buf[BSZ-2] = '\0';
	buf[BSZ-1] = 'X';
	// buf: bbb...bbb0X
	fprintf(fp, "hello, world");
	fseek(fp, 0, SEEK_SET);
	// buf: bbbbbbbbbbbbhello, worldnullbbb...bbb0X, offsite: 0, curLen: 24
	printf("after fseek: %s\n", buf);
	printf("len of string in buf = %ld\n", (long)strlen(buf));

	memset(buf, 'c', BSZ-2);
	buf[BSZ-2] = '\0';
	buf[BSZ-1] = 'X';
	// buf: ccc...ccc0X
	fprintf(fp, "hello, world");
	fclose(fp);
	// buf: hello, worldccc...ccc0X, new curLen: 12, because new curLen < old curLen(12 < 24), didn't insert null.
	printf("after fclose: %s\n", buf);
	printf("len of string in buf = %ld\n", (long)strlen(buf));

	return (0);
}
