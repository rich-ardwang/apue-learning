#include <stdlib.h>
#include <pthread.h>

struct foo {
	int				f_count;
	pthread_mutex_t	f_lock;
	int				f_id;
	/* ... more stuff here ... */
};

struct foo *
foo_alloc(int id)		/* allocate the object */
{
	struct foo		*fp;

	if ((fp = malloc(sizeof(struct foo))) != NULL) {
		fp->f_count = 1;
		fp->f_id = id;
		// 利用函数动态初始化结构体自己持有的mutex_t锁变量。
		if (pthread_mutex_init(&fp->f_lock, NULL) != 0) {
			free(fp);
			return (NULL);
		}
		/* ... continue initialization ... */
	}
	return (fp);
}

void
foo_hold(struct foo *fp)	/* add a reference to the object */
{
	// 增加引用计数时需要对这个对象加锁和解锁。
	pthread_mutex_lock(&fp->f_lock);
	fp->f_count++;
	pthread_mutex_unlock(&fp->f_lock);
}

void
foo_rele(struct foo *fp)	/* release a reference to the object */
{
	// 减少引用计数也需要对这个对象加锁和解锁。
	pthread_mutex_lock(&fp->f_lock);
	if (--fp->f_count == 0) {	/* last reference */
		pthread_mutex_unlock(&fp->f_lock);
		// 引用计数为零，释放对象前，销毁其mutex变量。
		pthread_mutex_destory(&fp->f_lock);
		free(&fp);
	} else {
		pthread_mutex_unlock(&fp->f_lock);
	}
}
