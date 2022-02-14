#include "apue.h"
#if defined(SOLARIS)
#include <netinet/in.h>
#endif
#include <netdb.h>
#include <arpa/inet.h>
#if defined(BSD)
#include <sys.socket.h>
#include <netinet/in.h>
#endif

// 打印地址族(Address Family)。
void
print_family(struct addrinfo *aip)
{
	printf(" family:");
	switch (aip->ai_family) {
	case AF_INET:
		printf("inet");
		break;
	case AF_INET6:
		printf("inet6");
		break;
	case AF_UNIX:
		printf("unix");
		break;
	case AF_UNSPEC:
		printf("unspecified");
		break;
	default:
		printf("unknown");
	}
}

// 打印套接字类型。
void
print_type(struct addrinfo *aip)
{
	printf(" type:");
	switch (aip->ai_socktype) {
	case SOCK_STREAM:
		printf("stream");
		break;
	case SOCK_DGRAM:
		printf("datagram");
		break;
	case SOCK_SEQPACKET:
		printf("seqpacket");
		break;
	case SOCK_RAW:
		printf("raw");
		break;
	default:
		printf("unknown (%d)", aip->ai_socktype);
	}
}

// 打印套接字协议。
void
print_protocol(struct addrinfo *aip)
{
	printf(" protocol:");
	switch (aip->ai_protocol) {
	case 0:
		printf("default");
		break;
	case IPPROTO_TCP:
		printf("TCP");
		break;
	case IPPROTO_UDP:
		printf("UDP");
		break;
	case IPPROTO_RAW:
		printf("raw");
		break;
	default:
		printf("unknown (%d)", aip->ai_protocol);
	}
}

// 打印套接字行为标志。
void
print_flags(struct addrinfo *aip)
{
	printf(" flags:");
	if (aip->ai_flags == 0) {
		printf(" 0");
	} else {
		// 套接字地址用于监听绑定。
		if (aip->ai_flags & AI_PASSIVE)
			printf("passive");
		// 需要一个规范的名字(与别名相对)。
		if (aip->ai_flags & AI_CANONNAME)
			printf("canon");
		// 以数字格式指定主机地址，不翻译。
		if (aip->ai_flags & AI_NUMERICHOST)
			printf("numhost");
		// 将服务指定为数字端口号，不翻译。
		if (aip->ai_flags & AI_NUMERICSERV)
			printf("numserv");
		// 如果没有找到IPv6地址，返回映射到IPv6格式的IPv4地址。
		if (aip->ai_flags & AI_V4MAPPED)
			printf("v4mapped");
		// 查找IPv4和IPv6地址(仅用于AI_V4MAPPED)。
		if (aip->ai_flags & AI_ALL)
			printf("all");
	}
}

int
main(int argc, char *argv[])
{
	struct addrinfo			*ailist, *aip;
	struct addrinfo			hint;
	struct sockaddr_in		*sinp;
	const char				*addr;
	int						err;
	// INET_ADDRSTRLEN定义了足够容纳IPv4地址字符串的长度，
	// INET6_ADDRSTRLEN定义了足够容纳IPv6地址字符串的长度。
	char					abuf[INET_ADDRSTRLEN];

	// 只接受3个参数，其中2个交互式输入参数。
	if (argc != 3)
		err_quit("usage: %s nodename service", argv[0]);
	// 提供一个hint模板，在这里初始化它。
	hint.ai_flags = AI_CANONNAME;
	hint.ai_family = 0;
	hint.ai_socktype = 0;
	hint.ai_protocol = 0;
	hint.ai_addrlen = 0;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;
	// getaddrinfo函数允许将一个主机名和一个服务名映射到一个地址。
	// 参数1：主机名，可以是一个节点名，也可以是点分格式的主机地址。
	// 参数2：服务名，如nfs、ssh、telnet等。
	// 参数3：可以提供一个可选的hint来选择符合特定条件的地址，hint
	// 是一个用于过滤地址的模板。
	// 参数4：函数返回一个链表结构的结果集。
	if ((err = getaddrinfo(argv[1], argv[2], &hint, &ailist)) != 0)
		err_quit("getaddrinfo error: %s", gai_strerror(err));
	// aip是一个addrinfo结构的指针，相当于一个迭代器，使用它来遍历
	// 结果集链表。
	for (aip = ailist; aip != NULL; aip = aip->ai_next) {
		// 将我们感兴趣的各种信息打印出来。
		print_flags(aip);
		print_family(aip);
		print_type(aip);
		print_protocol(aip);
		printf("\n\thost %s", aip->ai_canonname ? aip->ai_canonname : "-");
		// 针对IPv4特殊处理。
		if (aip->ai_family == AF_INET) {
			// 将sockaddr *转换为sockaddr_in *。
			sinp = (struct sockaddr_in *)aip->ai_addr;
			// 将网络字节序的二进制地址转换成文本字符串格式。
			addr = inet_ntop(AF_INET, &sinp->sin_addr, abuf,
					INET_ADDRSTRLEN);
			// 打印IPv4地址字符串。
			printf(" address %s", addr ? addr : "unknown");
			// 打印服务映射的端口号。
			printf(" port %d", ntohs(sinp->sin_port));
		}
		printf("\n");
	}
	exit(0);
}
