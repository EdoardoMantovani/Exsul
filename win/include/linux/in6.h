#ifndef _LINUX_IN6_H
#define _LINUX_IN6_H

#include <linux/types.h>
#include <linux/socket.h>

#define s6_addr			in6_u.u6_addr8
#define s6_addr16		in6_u.u6_addr16
#define s6_addr32		in6_u.u6_addr32

struct in6_addr {
	union {
		__u8		u6_addr8[16];
		__be16		u6_addr16[8];
		__be32		u6_addr32[4];
	} in6_u;
};

struct sockaddr_in6 {
	__kernel_sa_family_t sin6_family;
	__be16 sin6_port;
	__be32 sin6_flowinfo;
	struct in6_addr sin6_addr;
	__u32 sin6_scope_id;
};

#endif
