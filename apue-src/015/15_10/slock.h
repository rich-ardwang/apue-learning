#include <semaphore.h>
#include <bits/posix1_lim.h>

// 自己定义的基于POSIX信号量的锁。
struct slock {
    // POSIX信号量对象的指针。
    sem_t   *semp;
    // POSIX信号量名字的最大长度。
    char    name[_POSIX_NAME_MAX];
};
