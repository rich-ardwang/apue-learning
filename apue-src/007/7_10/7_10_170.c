#include "apue.h"

#define TOK_ADD		5

void	do_line(char *);
void	cmd_add(void);
int		get_token(void);

int
main(void)
{
	char		line[MAXLINE];

	while (fgets(line, MAXLINE, stdin) != NULL)
		do_line(line);
	exit(0);
}

char *tok_ptr;			/* global pointer for get_token() */

void
do_line(char *ptr)		/* process one line of input */
{
	int		cmd;

	tok_ptr = ptr;
	while ((cmd = get_token()) > 0) {
		switch (cmd) {		/* one case for each command */
		case TOK_ADD:
			cmd_add();
			break;
		}
	}
}

void
cmd_add(void)
{
	int		token;

	token = get_token();
	/* rest of processding for this command */
	tok_ptr = NULL;
	printf("token stored.\n");
}

int
get_token(void)
{
	/* fetch next token from line pointed to by tok_ptr */
	if (tok_ptr) {
		printf("token generated.\n");
		return TOK_ADD;
	}
	else
		return -1;
}
