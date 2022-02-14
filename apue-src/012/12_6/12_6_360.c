#include <limits.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>

#define MAXSTRINGSZ			4096

// 静态初始化线程特定数据键，该key可以被进程中的所有线程使用，
// 但是每个线程会把该key与自己的特定数据相关联。
static pthread_key_t key;
// 静态方式初始化唯一次数控制量。
static pthread_once_t init_done = PTHREAD_ONCE_INIT;
// 静态方式初始化互斥量。
pthread_mutex_t env_mutex = PTHREAD_MUTEX_INITIALIZER;

extern char **environ;

static void
thread_init(void)
{
	// 让每个线程使用该key与自己的特定数据相关联，
	// 这样每个线程拥有自己的特定数据。
	// 参数2：可以自定义析构函数，这里执行为系统函数free。
	pthread_key_create(&key, free);
}

// getenv的另一个可重入版本，线程安全，并且不需要调用者提供缓冲区，
// 它是利用pthread_key_create和pthread_setspecific在每个线程
// 创建一块私有的数据区域作为缓冲区，各个线程互不干扰。
char *
getenv(const char *name)
{
	int		i, len;
	char	*envbuf;

	// 保证不管有多少个线程调用getenv，thread_init只会被执行唯一一次。
	pthread_once(&init_done, thread_init);
	pthread_mutex_lock(&env_mutex);
	// 每个线程利用key创建自己的特定数据缓冲区，
	// 该缓冲区别的线程不能访问。
	envbuf = (char *)pthread_getspecific(key);
	if (envbuf == NULL) {
		envbuf = malloc(MAXSTRINGSZ);
		if (envbuf == NULL) {
			pthread_mutex_unlock(&env_mutex);
			return (NULL);
		}
		pthread_setspecific(key, envbuf);
	}
	len = strlen(name);
	for (i = 0; environ[i] != NULL; i++) {
		if ((strncmp(name, environ[i], len) == 0) &&
			(environ[i][len] == '=')) {
			strncpy(envbuf, &environ[i][len+1], MAXSTRINGSZ-1);
			pthread_mutex_unlock(&env_mutex);
			return (envbuf);
		}
	}
	pthread_mutex_unlock(&env_mutex);
	return (NULL);
}
