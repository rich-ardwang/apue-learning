#include "apue.h"
#include <errno.h>
#include <limits.h>

#ifdef PATH_MAX
static long pathmax = PATH_MAX;
#else
static long pathmax = 0;
#endif

static long posix_version = 0;
static long xsi_version = 0;

/* If PATH_MAX is indeterminate, no guarantee this is adequate */
// 如果PATH_MAX是不确定的，不能保证如下自定义的值是足够的。
#define PATH_MAX_GUESS 1024

char *
path_alloc(size_t *sizep)	/* also return allocated size, if nonnull */
{
	char		*ptr;
	size_t		size;

	// 取得POSIX的版本。
	if (posix_version == 0)
		posix_version = sysconf(_SC_VERSION);

	// 取得XSI的版本。
	if (xsi_version == 0)
		xsi_version = sysconf(_SC_XOPEN_VERSION);

	// 如果PATH_MAX未定义，则会首次进入。
	if (pathmax == 0) {		/* first time through */
		errno = 0;
		// 将根目录作为参数调用pathconf。
		if ((pathmax = pathconf("/", _PC_PATH_MAX)) < 0) {
			// 如果pathconf返回值小于0，需要检查errno。
			// 如果errno未被重置，则说明_PC_PATH_MAX未定义，
			// 使用自定义值即可。
			if (errno == 0)
				pathmax = PATH_MAX_GUESS;	/* it's indeterminate */
			else
				// 其他情况说明pathconf调用出错。
				err_sys("pathconf error for _PC_PATH_MAX");
		} else {
			// 如果pathconf返回值大于等于0，说明已定义。
			pathmax++;		/* add one since it's relative to root */
		}
	}

	/*
	 * Before POSIX.1-2001, we aren't guaranteed that PATH_MAX includes
	 * the ternminating null byte. Same goes for XPG3.
	 */
	// 在POSIX.1-2001版本之前，我们不能保证PATH_MAX包含终止null字符，所以需要加1
	// 个字节给null。
	// 同理XPG3和其之前的版本也是如此。
	if ((posix_version < 200112L) && (xsi_version < 4))
		size = pathmax + 1;
	else
		size = pathmax;

	// 调用malloc在heap上分配存储空间。
	if ((ptr = malloc(size)) == NULL)
		err_sys("malloc error for pathname");

	// 如果输入输出参数sizep有效，可以把分配的size传递出去。
	if (sizep != NULL)
		*sizep = size;
	// 返回已分配的内存空间。
	return (ptr);
}

// 点评
// 本例演示了为路径名的动态分配存储空间，因为PATH_MAX的值不总是
// 固定不变的，这意味着不能编译时确定，那么也就不能保证运行时仍然
// 可用，所以需要编写动态分配函数，当运行时也不可用时，可以指定我们
// 自定义的值对其进行分配。
