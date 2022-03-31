#include "apue.h"

int main(void)
{
	// 打印出用户id和组id。
	printf("uid = %d, gid = %d\n", getuid(), getgid());
	exit(0);
}

// 注意
// uid为0的用户是root用户或称为超级用户，其他用户的uid都是大于0的。
// 每个用户拥有一个组id和最多16个附属组id，在/etc/group文件中记录
// 着每个组对应于哪些用户。
