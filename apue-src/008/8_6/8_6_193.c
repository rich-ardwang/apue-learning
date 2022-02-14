#include "apue.h"
#include <sys/wait.h>

int
main(void)
{
	pid_t		pid;
	if ((pid = fork()) < 0) {
		err_sys("fork error");
	} else if (pid == 0) {			/* first child */
		if ((pid = fork()) < 0)
			err_sys("fork error");
		else if (pid > 0)
			exit(0);	/* parent from second fork == first child */

		/*
		 * We're the second child; our parent becomes init as soon
		 * as our real parent calls exit() in the statement above.
		 * Here's where we'd continue executing, knowing that when
		 * we're done, init will reap our status.
		 */
		sleep(2);
		// The parent becomes /sbin/upstart on Ubuntu1604, not init.
		// Upstart take charge in adopting orphans for the GUI of Ubuntu,
		// and if it is killed the GUI user will be kicked out.
		// If we kill the upstart on text shell, the parent will be init.
		printf("second child, parent pid = %ld\n", (long)getppid());
		exit(0);
	}

	if (waitpid(pid, NULL, 0) != pid)	/* wait for first child */
		err_sys("waitpid error");

	/*
	 * We're the parent (the original process); we continue executing,
	 * knowing that we're not the parent of the second child.
	 */
	exit(0);
}
