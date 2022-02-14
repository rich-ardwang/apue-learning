#include "apue.h"

static void charatatime(char *);

int
main(void)
{
	pid_t		pid;

	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid == 0) {
		charatatime("output from child8888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888\n");
	} else {
		charatatime("output from parent999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999\n");
	}
	exit(0);
}

static void
charatatime(char *str)
{
	char		*ptr;
	int			c;

	setbuf(stdout, NULL);		/* set unbuffered */
	for (ptr = str; (c = *ptr++) != 0; )
		putc(c, stdout);
}
