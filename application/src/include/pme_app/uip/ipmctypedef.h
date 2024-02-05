#ifndef TYPEDEF_H
#define TYPEDEF_H
#include <stdio.h>
#include <string.h>

#define IPMC_TIMEOUT_ERR (-2)
#define IPMC_ERROR (-1)
#define IPMC_OK 0
#define CPU_BIGENDIAN 0x8888
#define IPMC_BYTE_ORDER CPU_BIGENDIAN

#if IPMC_BYTE_ORDER == CPU_BIGENDIAN
 #define LONG_BY_LITTLE_ENDIAN(l) (((l) >> 24) | (((l) >> 8) & 0xff00) | (((l) << 8) & 0xff0000) | ((l) << 24))
 #define SHORT_BY_LITTLE_ENDIAN(s) (((s) >> 8) | ((s) << 8))
#else
 #define LONG_BY_LITTLE_ENDIAN(x) (x)
 #define SHORT_BY_LITTLE_ENDIAN(x) (x)
#endif

#define BSWAP_16(x) ((((x) & 0xff00) >> 8) | (((x) & 0x00ff) << 8))
#define BSWAP_32(x) ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >> 8) | \
                     (((x) & 0x0000ff00) << 8) | (((x) & 0x000000ff) << 24))

#endif
