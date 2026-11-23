#ifndef _LINUX_BYTEORDER_LITTLE_ENDIAN_H
#define _LINUX_BYTEORDER_LITTLE_ENDIAN_H

#include <linux/types.h>

#define __LITTLE_ENDIAN_BITFIELD

#define ___constant_swab16(x) ((__u16)((((__u16)(x) & 0x00ffU) << 8) | (((__u16)(x) & 0xff00U) >> 8)))
#define ___constant_swab32(x) ((__u32)((((__u32)(x) & 0x000000ffUL) << 24) | (((__u32)(x) & 0x0000ff00UL) << 8) | (((__u32)(x) & 0x00ff0000UL) >> 8) | (((__u32)(x) & 0xff000000UL) >> 24)))
#define ___constant_swab64(x) ((__u64)((((__u64)(x) & 0x00000000000000ffULL) << 56) | (((__u64)(x) & 0x000000000000ff00ULL) << 40) | (((__u64)(x) & 0x0000000000ff0000ULL) << 24) | (((__u64)(x) & 0x00000000ff000000ULL) << 8) | (((__u64)(x) & 0x000000ff00000000ULL) >> 8) | (((__u64)(x) & 0x0000ff0000000000ULL) >> 24) | (((__u64)(x) & 0x00ff000000000000ULL) >> 40) | (((__u64)(x) & 0xff00000000000000ULL) >> 56)))

#define __swab16(x) __builtin_bswap16((__u16)(x))
#define __swab32(x) __builtin_bswap32((__u32)(x))
#define __swab64(x) __builtin_bswap64((__u64)(x))

#define __constant_htonl(x) ((__be32)___constant_swab32((x)))
#define __constant_ntohl(x) ___constant_swab32((__be32)(x))
#define __constant_htons(x) ((__be16)___constant_swab16((x)))
#define __constant_ntohs(x) ___constant_swab16((__be16)(x))
#define __constant_cpu_to_le64(x) ((__le64)(__u64)(x))
#define __constant_le64_to_cpu(x) ((__u64)(__le64)(x))
#define __constant_cpu_to_le32(x) ((__le32)(__u32)(x))
#define __constant_le32_to_cpu(x) ((__u32)(__le32)(x))
#define __constant_cpu_to_le16(x) ((__le16)(__u16)(x))
#define __constant_le16_to_cpu(x) ((__u16)(__le16)(x))
#define __constant_cpu_to_be64(x) ((__be64)___constant_swab64((x)))
#define __constant_be64_to_cpu(x) ___constant_swab64((__u64)(__be64)(x))
#define __constant_cpu_to_be32(x) ((__be32)___constant_swab32((x)))
#define __constant_be32_to_cpu(x) ___constant_swab32((__u32)(__be32)(x))
#define __constant_cpu_to_be16(x) ((__be16)___constant_swab16((x)))
#define __constant_be16_to_cpu(x) ___constant_swab16((__u16)(__be16)(x))
#define __cpu_to_le64(x) ((__le64)(__u64)(x))
#define __le64_to_cpu(x) ((__u64)(__le64)(x))
#define __cpu_to_le32(x) ((__le32)(__u32)(x))
#define __le32_to_cpu(x) ((__u32)(__le32)(x))
#define __cpu_to_le16(x) ((__le16)(__u16)(x))
#define __le16_to_cpu(x) ((__u16)(__le16)(x))
#define __cpu_to_be64(x) ((__be64)__swab64((x)))
#define __be64_to_cpu(x) __swab64((__u64)(__be64)(x))
#define __cpu_to_be32(x) ((__be32)__swab32((x)))
#define __be32_to_cpu(x) __swab32((__u32)(__be32)(x))
#define __cpu_to_be16(x) ((__be16)__swab16((x)))
#define __be16_to_cpu(x) __swab16((__u16)(__be16)(x))
#endif
