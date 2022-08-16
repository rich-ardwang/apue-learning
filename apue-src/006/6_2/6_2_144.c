#include <pwd.h>
#include <stddef.h>
#include <string.h>
#include "apue.h"

// 本例给出了getpwnam()的一个实现。
// getpwnam()函数通过登录名获取其在/etc/passwd文件中的详细信息，该函数被login程序使用。
struct passwd *
getpwnam(const char *name)
{
	// 密码文件结构体指针
	struct passwd	*ptr;

	// setpwent()用来将getpwent()的读写地址指回密码文件开头。
	// 这里起着保护作用，如果之前用户已经调用了getpwent()，这里的调用将确保读写地址定位到密码文件开头。
	setpwent();
	// 调用getpwent()函数遍历读取密码文件。
	while ((ptr = getpwent()) != NULL)
		// 找到和目标登录名一致的条目后跳出循环。
		if (strcmp(name, ptr->pw_name) == 0)
			break;		/* found a match */
	// getpwent()和setpwent()函数会打开passwd等相关文件，查询结束后必须调用endpwent()函数关闭这些文件。
	endpwent();
	// 将读到的信息指针返回给用户，如果未找到信息，则返回NULL。
	return (ptr);		/* ptr is NULL if no match found */
}

// Add by Richard Wang
// 2021-12-07
// 测试程序
int main(int argc, char **argv) {
	// 仅接受1个参数，参数为登录名。
	if (argc != 2) {
		printf("Usage: exc <user_name>\n");
		exit(0);
	}
	// 调用我们自己实现的getpwnam()函数查询密码结构信息。
	struct passwd *pwd = getpwnam(argv[1]);
	// 未查到信息。
	if (NULL == pwd) {
		err_ret("getpwnam error");
		exit(0);
	}
	// 查到信息后输出查询结果。
	printf("name:%s, pass:%s, uid:%u, gid:%u, rname:%s, dir:%s, shell:%s\n",
		pwd->pw_name, pwd->pw_passwd, pwd->pw_uid, pwd->pw_gid,
		pwd->pw_gecos, pwd->pw_dir, pwd->pw_shell);
	
	exit(0);
}

// 扩展
// 还有一个函数根据uid来查询passwd的详细信息，这个函数是getpwuid(uid_t uid)。
