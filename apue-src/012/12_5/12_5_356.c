#include <limits.h>
#include <string.h>

#define MAXSTRINGSZ		4096

static char envbuf[MAXSTRINGSZ];

extern char **environ;

// getenv不可重入函数，不是线程安全的，不能多线程使用。
char *
getenv(const char *name)
{
	int i, len;

	len = strlen(name);
	for (i = 0; environ[i] != NULL; i++) {
		if ((strncmp(name, environ[i], len) == 0) &&
			(environ[i][len] == '=')) {
			strncpy(envbuf, &environ[i][len+1], MAXSTRINGSZ-1);
		}
	}
	return (NULL);
}
