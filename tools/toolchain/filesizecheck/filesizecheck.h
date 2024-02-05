#ifndef _CFGFILE_H_
#define _CFGFILE_H_

#include <stdio.h>

#define MAX_LINE_SIZE       512
#define MAX_FILE_NAME_SIZE  256
#define MAX_VALUE_NUM       32

#define UPGRADE_CFGFILEPATH     "CfgFileList.conf"
/*Size=xxxx\n 4+1+4+1 size占4字节,=一个字节,大小占4字节,换行1字节*/
#define MAX_COUPLE_OFFSET   10

typedef struct tg_CfgParseCB
{
    FILE*        fp;
    char         file_path[MAX_LINE_SIZE];
    char*        value[MAX_VALUE_NUM];
} CFGPARSE_CB_T, *PCFGPARSE_CB_T;

#endif /*_CFGFILE_H_*/
