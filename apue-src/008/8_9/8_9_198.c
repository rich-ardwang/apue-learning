#include "apue.h"

static void charatatime(char *);

int
main(void)
{
	pid_t		pid;

	TELL_WAIT();

	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid == 0) {  // child
		//WAIT_PARENT();			/* parent goes first */
		charatatime("output from child8888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888\n");
		TELL_PARENT(pid);
	} else {  // parent
		WAIT_CHILD();               /* child goes first */
		charatatime("output from parent999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999\n");
		//TELL_CHILD(pid);
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
