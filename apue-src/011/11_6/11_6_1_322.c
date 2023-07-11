#include <stdlib.h>
#include <pthread.h>

// 结构定义
struct foo {
    int                f_count;  // 计数器
    pthread_mutex_t    f_lock;   // 互斥量，也称互斥锁对象
    int                f_id;     // 对象ID
    /* ... more stuff here ... */
};

// 在heap上分配foo对象，返回指向对象的指针。
struct foo *
foo_alloc(int id)        /* allocate the object */
{
    // 结构体foo的对象的指针
    struct foo        *fp;

    // sizeof取得结构总字节数
    // 调用malloc在堆上分配对象，取得该对象的指针。
    if ((fp = malloc(sizeof(struct foo))) != NULL) {
        // 初始化对象
        // 计数器初始化为1
        fp->f_count = 1;
        // 保存对象ID
        fp->f_id = id;
        // 利用pthread_mutex_init函数动态初始化结构体自己持有的mutex_t锁变量。
        if (pthread_mutex_init(&fp->f_lock, NULL) != 0) {
            // 初始化互斥锁对象失败
            // 在heap上释放该对象
            free(fp);
            // 把指针指向NULL，避免出现野指针。
            return (NULL);
        }
        /* ... continue initialization ... */
    }
    // 处理完毕后，返回指向已分配对象的指针。
    return (fp);
}

// 线程安全地递增计数器
// 该函数持有互斥锁，之后释放互斥锁。
void
foo_hold(struct foo *fp)    /* add a reference to the object */
{
    // 增加引用计数时需要对这个对象加锁和解锁。
    // 持有互斥锁时，其他线程不能更改共享对象数据。
    pthread_mutex_lock(&fp->f_lock);
    // 持有互斥锁的线程，可安全地更改共享对象的数据，
    // 这里将引用计数器+1
    fp->f_count++;
    // 释放互斥锁
    pthread_mutex_unlock(&fp->f_lock);
}

// 引用计数减1，当引用计数为0时，该函数销毁互斥锁对象。
void
foo_rele(struct foo *fp)    /* release a reference to the object */
{
    // 减少引用计数也需要对这个对象加锁和解锁。
    // 线程持有互斥锁对象
    pthread_mutex_lock(&fp->f_lock);
    // 引用计数减1，并判断是否归零。
    if (--fp->f_count == 0) {    /* last reference */
        // 引用计数为0时需要释放并销毁互斥锁对象
        // 释放互斥锁对象
        pthread_mutex_unlock(&fp->f_lock);
        // 释放互斥锁对象后销毁它
        pthread_mutex_destory(&fp->f_lock);
        // 释放在堆上分配的对象所占用的空间
        free(&fp);
    } else {
        // 如果引用计数没有归零，需要立即释放互斥锁给其他线程再利用。
        pthread_mutex_unlock(&fp->f_lock);
    }
}

// 点评
// 本例描述了用于保护某个数据结构的互斥量。当一个以上的线程需要访问动态分配的对象时，
// 可以在对象中嵌入引用计数，确保在所有使用该对象的线程完成数据访问前，该对象内存空
// 间不会被释放。

// 对引用计数的操作需要锁住互斥量，foo_alloc中引用计数初始化为1没有加锁，因为此时是
// 单线程操作的，foo对象创建完毕后，可能就会被多个线程访问，就需要加锁了。

// 使用对象
// 使用对象前线程需要调用foo_hold对这个对象引用计数加1，使用完毕后线程调用foo_rele
// 引用计数减1，如果最后一个引用计数被释放后，对象所占内存空间才会被释放。

// 有问题
// 如果一个线程在调用foo_hold时阻塞等待互斥锁，这时即使对象的引用计数为0，foo_rele
// 释放对象的内存仍然是不正确的，应确保对象在释放内存前不会被调用foo_hold（被找到）。
