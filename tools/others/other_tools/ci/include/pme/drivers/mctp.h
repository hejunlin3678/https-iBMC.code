/******************************************************************************

                  版权所有 (C), 2001-2013, 华为技术有限公司

******************************************************************************
  文 件 名   : mctp.h
  版 本 号   : 初稿
  作    者   : l00354559
  生成日期   : 2016年12月10日
  最近修改   :
  功能描述   : hi1710平台SDK用户程序接口
  函数列表   :
  修改历史   :
  1.日    期   : 2016年12月10日
    作    者   : l00354559
    修改内容   : 创建文件

******************************************************************************/

#ifndef _MCTP_API_H_
#define _MCTP_API_H_

#include "glib.h"

//读数据的最大超时时间，单位us
#define MCTP_TIMEOUT_MAX  (10 * 60 * 1000 *1000)
#define MCTP_TIMEOUT_NONE  (~0U)

#define MCTP_MSG_HEAD_LEN 16
#define MCTP_MAX_PAYLOAD_LEN 256
#define MCTP_MIN_PAYLOAD_LEN 4

/*最小包大小*/
#define MCTP_MIN_PACKET_SIZE (MCTP_MIN_PAYLOAD_LEN + MCTP_MSG_HEAD_LEN)
/*最大包大小*/
#define MCTP_MAX_PACKET_SIZE (MCTP_MAX_PAYLOAD_LEN + MCTP_MSG_HEAD_LEN)


#pragma pack(1)
typedef struct
{
    //此次操作的超时时间，单位为us(毫秒)，只在读数据时有效
    //取值范围为0~MCTP_TIMEOUT_MAX
    //MCTP_TIMEOUT_NONE表示此次操作一定要读取到数据才会返回
    unsigned int timeout;
    char* data;
    //在发送时，至少为MCTP_MIN_PACKET_SIZE，最多为MCTP_MAX_PACKET_SIZE
    //在接收时，至少为MCTP_MAX_PACKET_SIZE
    unsigned int data_len;
    unsigned char data_type;
} MCTP_DATA;
#pragma pack()

extern gint32 mctp_read(MCTP_DATA* mctp_read);
extern gint32 mctp_write(MCTP_DATA* mctp_write);

#endif
