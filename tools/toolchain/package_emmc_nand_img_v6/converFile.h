#ifndef CONVER_FILE__H
#define CONVER_FILE__H

#include <stdio.h>

//配制文件名
const char *const CONFIG_FILE_NAME = "config.cfg";

//配制文件最大有效大小
#define MAX_CONFIG_FILE 4096

//生成最终文件名
const char * const OBJ_FILE_NAME = "ipmc.bin";

//支持最大转化文件数
#define SUPPORT_FILE_NAME 12

//支持文件名最长长度
#define FILE_MAX_LEN 64

#define  CPUTYPE_X86 0
#define  CPUTYPE_852T 1
#define VOS_LITTLE_ENDIAN 1234
#define VOS_BIG_ENDIAN 4321

/*大小字序确定*/
#ifdef  WIN32
 #define  CPUTYPE CPUTYPE_X86
#else
 #define  CPUTYPE CPUTYPE_852T
#endif
#ifndef VOS_BYTE_ORDER
 #if (CPUTYPE == CPUTYPE_X86)
  #define VOS_BYTE_ORDER VOS_LITTLE_ENDIAN
 #elif (CPUTYPE == CPUTYPE_852T)
  #define VOS_BYTE_ORDER VOS_BIG_ENDIAN
 #else
  #error "Please give the default byte order for the new VOS_HARDWARE_PLATFORM!"
 #endif
#endif

//#define FLASH_MAX_SIZE 0x1000000 * 4 /*64M*/
//#define FLASH_MAX_SIZE 0x1000000 * 8 /*128M*/
//#define FLASH_MAX_SIZE 0x1000000 * 192LL /*3072M*/
#define FLASH_MAX_SIZE 0x1000000 * 400LL /*128M*/
/*定义字序转换宏*/
#if VOS_BYTE_ORDER == VOS_BIG_ENDIAN
 #define LONG_BY_BIG_ENDIAN(x) (x)
 #define SHORT_BY_BIG_ENDIAN(x) (x)
#else
 #define LONG_BY_BIG_ENDIAN(l) (((l) >> 24) | (((l) >> 8) & 0xff00) | (((l) << 8) & 0xff0000) | ((l) << 24))
 #define SHORT_BY_BIG_ENDIAN(s) (((s) >> 8) | ((s) << 8))
#endif

#if VOS_BYTE_ORDER == VOS_LITTLE_ENDIAN
 #define LONG_BY_LITTLE_ENDIAN(x) (x)
 #define SHORT_BY_LITTLE_ENDIAN(x) (x)
#else
 #define LONG_BY_LITTLE_ENDIAN(l) (((l) >> 24) | (((l) >> 8) & 0xff00) | (((l) << 8) & 0xff0000) | ((l) << 24))
 #define SHORT_BY_LITTLE_ENDIAN(s) (((s) >> 8) | ((s) << 8))
#endif

typedef void           VOS_VOID;
typedef unsigned char  VOS_UINT8;
typedef signed char    VOS_INT8;
typedef char           VOS_CHAR;                       /* 8位ASCII值 */
typedef unsigned char  VOS_UCHAR;
typedef unsigned short VOS_UINT16;
typedef signed short   VOS_INT16;
typedef signed short   VOS_WCHAR;                  /* 16位Unicode值 */
typedef unsigned long  VOS_UINT32;
typedef unsigned long long  VOS_UINT64;
typedef signed long    VOS_INT32;
typedef VOS_UINT8      VOS_BOOL;
typedef int            VOS_INT;
typedef unsigned int   VOS_UINT;
typedef float          VOS_FLOAT;                        /* 32 bits float */
typedef double         VOS_DOUBLE;                     /* 64 bits double */
typedef unsigned long  VOS_PID;
typedef unsigned long  VOS_FID;
typedef unsigned long  VOS_SIZE_T;

#pragma pack (1)

typedef struct BACKUP_IMAGE
{
    VOS_CHAR   ucValid_Flag;          /*image有效标志*/
    VOS_CHAR   ucDowmLoad_Flag;       /*加载标志*/
    VOS_UINT16 usCopy_Version;        /*软件搬移版本号*/
    VOS_CHAR   ucMajorVersion_Num;    /*主版本号*/
    VOS_CHAR   ucMinorVersion_Num;    /*次版本号*/
    VOS_UINT16 usImage_Checksum;      /*image检验和*/
    VOS_CHAR   ucFirmewareName[16];   /*固件名*/
    VOS_UINT16 uiImage_Len;           /*image长度*/
    VOS_CHAR   ucReserved1[4];        /*保留1*/
    VOS_UINT32 uiCopy_Flag[2];        /*image向片内搬移标志*/
    VOS_CHAR   ucReserved2[88];       /*保留2*/
} BACKUP_IMAGE_S;

#pragma pack()

typedef VOS_INT32 (*ConversionFile) \
    (FILE *    pifile, \
     VOS_UINT64 ulFileSize, \
     VOS_CHAR * pDataBuffer, \
     VOS_UCHAR  ucIndex);

typedef struct tagFILETYPE
{
    VOS_CHAR       StrType[10];
    VOS_CHAR       ucFileType;
    ConversionFile pConversionFile;
} FILETYPE;

typedef struct tagConverFileListItem
{
    VOS_CHAR   FileName[FILE_MAX_LEN];
    VOS_CHAR   ucFileVersion[10];
    VOS_CHAR   ucFileType;
    VOS_UINT64 ucWriteAddress;
} CONVERFILEITEM;

#define STATE_INIT 0
#define STATE_FIRST_WORD 1
#define STATE_SECOND_WORD 2
#define STATE_THREE_WORD 3

#define true 1
#define false 0

VOS_CHAR  ExplainConfigFile();
VOS_CHAR  ExplainLine(char* line, char len);
VOS_CHAR  FindType(char* pstrtype, char* uctype);
VOS_CHAR  SortFileList();

VOS_INT32 ConversionKernelRFFile(FILE *pifile, VOS_UINT64 ulFileSize, VOS_CHAR *pDataBuffer, VOS_UCHAR ucIndex);
VOS_INT32 ConversionUBootFile(FILE *pifile, VOS_UINT64 ulFileSize, VOS_CHAR *pDataBuffer, VOS_UCHAR ucIndex);
VOS_INT32 ConversionWBSABMCFile(FILE *pifile, VOS_UINT64 ulFileSize, VOS_CHAR *pDataBuffer, VOS_UCHAR ucIndex);
VOS_INT32 Conversion852TFile(FILE *pifile, VOS_UINT64 ulFileSize, VOS_CHAR *pDataBuffer, VOS_UCHAR ucIndex);
extern CONVERFILEITEM g_FileList[];

extern FILETYPE g_FileTypeList[];

#endif
