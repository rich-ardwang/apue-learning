#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define NHASH        29
#define HASH(id)    (((unsigned long)id)%NHASH)

struct foo *fh[NHASH];

// 分配全局静态mutex_t变量，用于锁定fh[NHASH]这个全局哈希桶。
pthread_mutex_t hashlock = PTHREAD_MUTEX_INITIALIZER;

struct foo {
    int                f_count;
    pthread_mutex_t    f_lock;
    int                f_id;
    int                f_idx;
    struct foo        *f_next;    /* protected by hashlock */
    /* ... more stuff here ... */
};

struct foo *
foo_alloc(int id)        /* allocate the object */
{
    struct foo            *fp;
    int                    idx;

    if ((fp = malloc(sizeof(struct foo))) != NULL) {
        fp->f_count = 1;
        fp->f_id = id;
        // 动态分配mutex_t变量，用于保护每个哈希元素。
        if (pthread_mutex_init(&fp->f_lock, NULL) != 0) {
            free(fp);
            return (NULL);
        }
        idx = HASH(id);
        fp->f_idx = idx;
        pthread_mutex_lock(&hashlock);
        fp->f_next = fh[idx];
        fh[idx] = fp;
        pthread_mutex_lock(&fp->f_lock);
        pthread_mutex_unlock(&hashlock);
        /* ... continue initialization ... */
        pthread_mutex_unlock(&fp->f_lock);
    }
    return (fp);
}

void
foo_hold(struct foo *fp)    /* add a reference to the object */
{
    pthread_mutex_lock(&fp->f_lock);
    fp->f_count++;
    pthread_mutex_unlock(&fp->f_lock);
}

struct foo *
foo_find(int id)    /* find an existing object */
{
    struct foo        *fp;

    pthread_mutex_lock(&hashlock);
    for (fp = fh[HASH(id)]; fp != NULL; fp = fp->f_next) {
        if (fp->f_id == id) {
            foo_hold(fp);
            break;
        }
    }
    pthread_mutex_unlock(&hashlock);
    return (fp);
}

void
foo_rele(struct foo *fp)    /* release a reference to the object */
{
    struct foo        *tfp;
    int                idx;

    pthread_mutex_lock(&fp->f_lock);
    if (fp->f_count == 1) {    /* last reference */
        pthread_mutex_unlock(&fp->f_lock);
        pthread_mutex_lock(&hashlock);
        pthread_mutex_lock(&fp->f_lock);
        /* need to recheck the condition */
        if (fp->f_count != 1) {
            fp->f_count--;
            pthread_mutex_unlock(&fp->f_lock);
            pthread_mutex_unlock(&hashlock);
            return;
        }
        /* remove from list */
        idx = HASH(fp->f_id);
        tfp = fh[idx];
        if (tfp == fp) {
            fh[idx] = fp->f_next;
        } else {
            while (tfp->f_next != fp)
                tfp = tfp->f_next;
            tfp->f_next = fp->f_next;
        }
        pthread_mutex_unlock(&hashlock);
        pthread_mutex_unlock(&fp->f_lock);
        pthread_mutex_destroy(&fp->f_lock);
        free(fp);
    } else {
        fp->f_count--;
        pthread_mutex_unlock(&fp->f_lock);
    }
}

// Add by Richard Wang
// 2021-12-30
void release_all(int nmax) {
    int i;
    struct foo *pf;
    for (i=0; i<nmax; i++) {
        pf = foo_find(i+1);
        foo_rele(pf);
        foo_rele(pf);
    }
}

int main() {
    int        nMax = 1000;
    int     i;
    for (i=0; i<nMax; i++) {
        foo_alloc(i+1);
    }

    // 显示1-100
    struct foo    *pf = NULL;
    for (i=0; i<100; i++) {
        pf = foo_find(i+1);
        printf("f_id: %d, f_idx: %d, f_count: %d, f_next:%p, address:%p\n", pf->f_id,
            pf->f_idx, pf->f_count, pf->f_next, pf);
        foo_rele(pf);
    }

    release_all(nMax);
    exit(0);
}
