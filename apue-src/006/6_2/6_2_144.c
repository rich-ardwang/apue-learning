#include <pwd.h>
#include <stddef.h>
#include <string.h>
#include "apue.h"

struct passwd *
getpwnam(const char *name)
{
	struct passwd	*ptr;

	setpwent();
	while ((ptr = getpwent()) != NULL)
		if (strcmp(name, ptr->pw_name) == 0)
			break;		/* found a match */
	endpwent();
	return (ptr);		/* ptr is NULL if no match found */
}

// Add by Richard Wang
// 2021-12-07
int main(int argc, char **argv) {
	if (argc < 2) {
		printf("Usage: exc <user_name>\n");
		exit(0);
	}
	struct passwd *pwd = getpwnam(argv[1]);
	if (NULL == pwd)
		err_ret("getpwnam error");
	printf("name:%s, pass:%s, uid:%u, gid:%u, rname:%s, dir:%s, shell:%s\n",
		pwd->pw_name, pwd->pw_passwd, pwd->pw_uid, pwd->pw_gid,
		pwd->pw_gecos, pwd->pw_dir, pwd->pw_shell);
	
	exit(0);
}