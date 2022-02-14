#include "apue.h"
#include <dirent.h>

int main(int argc, char *argv[])
{
	DIR					*dp;
	struct dirent		*dirp;

	if (argc != 2)
		err_quit("usage: ls directory_name");

	if((dp = opendir(argv[1])) == NULL)
		err_sys("Can't open %s", argv[1]);
	while ((dirp = readdir(dp)) != NULL) {
		printf("%s\n", dirp->d_name);
        //printf("%x\n", dirp->d_type);
        //printf("%d\n", dirp->d_reclen);
    }

	closedir(dp);
	exit(0);
}
