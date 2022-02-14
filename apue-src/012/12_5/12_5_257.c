#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>

extern char **environ;

pthread_mutex_t env_mutex;

// 静态方式初始化唯一次数限制控制量。
static pthread_once_t init_done = PTHREAD_ONCE_INIT;

static void
thread_init(void)
{
	pthread_mutexattr_t attr;

	// 设置互斥锁工作在递归模式。
	pthread_mutexattr_init(&attr);	
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	// 初始化互斥量。
	pthread_mutex_init(&env_mutex, &attr);
	// 销毁互斥量属性修改函数。
	pthread_mutexattr_destroy(&attr);
}

// getenv函数的可重入版本，这个版本需要调用者提供缓冲区，
// 这样每个线程拥有自己的缓冲区，互不干扰。
int
getenv_r(const char *name, char *buf, int buflen)
{
	int i, len, olen;

	// 唯一次数限制控制量保证不管有多少个线程调用getenv_r，
	// thread_init函数只会被某个线程执行唯一的一次。
	pthread_once(&init_done, thread_init);
	len = strlen(name);
	pthread_mutex_lock(&env_mutex);
	for (i = 0; environ[i] != NULL; i++) {
		if ((strncmp(name, environ[i], len) == 0) &&
			(environ[i][len] == '=')) {
			olen = strlen(&environ[i][len+1]);
			if (olen >= buflen) {
				pthread_mutex_unlock(&env_mutex);
				return (ENOSPC);
			}
			strcpy(buf, &environ[i][len+1]);
			pthread_mutex_unlock(&env_mutex);
			return (0);
		}
	}
	pthread_mutex_unlock(&env_mutex);
	return (ENOENT);
}
