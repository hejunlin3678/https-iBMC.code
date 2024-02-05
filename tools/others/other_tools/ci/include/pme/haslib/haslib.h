/******************************************************************************

                  版权所有 (C), 2008-2009, 华为技术有限公司

 ******************************************************************************
  文 件 名   : haslib.h
  版 本 号   : 初稿
  部    门   :
  作    者   : s00120510
  生成日期   : 2013年9月10日
  最近修改   :
  功能描述   : haslib提供给dflib库使用的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年9月10日
    作    者   : s00120510
    修改内容   : 创建文件

******************************************************************************/

#ifndef __HASLIB_H__
#define __HASLIB_H__

#include "pme/common/mscm_vos.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


// 等vos库确定后替换
#define HAS_OK    VOS_OK
#define HAS_ERR   VOS_ERR

#define HAS_RUN_ERR                  (-2100)
#define HAS_WRONG_PARAMETER          (-2101)
#define HAS_OBJECT_NOT_EXIST         (-2109)
#define HAS_TYPE_MISMATCH            (-2112)
#define HAS_NOT_INITED               (-2118)
#define HAS_TIME_OUT               (-2119)
#define HAS_HARDWARE_ERR      (-2120)
#define HAS_DATA_CHECK_ERR    (-2121)

#define HAS_EXTEND_CHIP_READ_MODE    0xffffffff  /* 将最后一个offset值作为扩展的芯片读模式 */

/* fanchip的对外宏定义 */
#define FANCHIP_LOAD_FLAG            0xfffffffe


typedef struct tag_extend_chip_read_s
{
    gint32 input_length;
    gconstpointer input_data;
    gint32 output_length;
    gpointer output_data;
}EXTEND_CHIP_READ_S;

// 物理对象哈希表key分隔符
#define KEY_SPLIT_FLAG   "."

// 下面的结构体依赖xml解析模块，以该模块的定义为准
typedef struct tag_obj_property
{
   gchar      *name;
   GVariant   *value;   // 类型已经在value中了
}OBJ_PROPERTY_S;

//bus over lan 命令结构
typedef struct tag_remote_bus_cmd
{
    guint16 bus_type;         /*bus类型，i2c-0*/
    guint16 bus_id;             /*bus通道号*/
    gint32 op_type;            /*操作类型0-读，1-写*/
    guint32 tx_len;             /*发送命令中数据长度( 地址 + 其他数据 的字节数 )*/
    guint32 rx_len;             /*读出数据的长度*/
    guint32 time_out_cnt;   /*读超时时间 ms*/
    guint32 retry_cnt;    /*重读次数 默认为3，失败时重读三次*/
    gchar tx_buffer[0];     /*读操作发送的数据 (地址(1byte高7位) + 其他数据),其他数据可以没有*/
}REMOTE_BUS_CMD_S;

typedef struct tag_remote_bus_buf
{
    gint32 access_type;      /*字节访问还是块访问*/
    guint32 mask;              /*数据掩码，字节访问时有效*/
    guint32 size;                /*数据buffer size*/
    guint8* buf;                 /*数据buffer*/
}REMOTE_BUS_BUF_S;

typedef enum tag_remote_bus_type
{
    REMOTE_BUS_I2C,
    REMOTE_BUS_INV,
}REMOTE_BUS_TYPE_E;

extern OBJ_PROPERTY_S* create_has_property( const gchar* name, GVariant* variant );
extern gint32 create_physical_object( GHashTable* hashtable );
extern gint32 delete_physical_object(guint32 position);
extern void dump_physical_object(FILE *stream);
extern void free_has_property( OBJ_PROPERTY_S* property );
extern gint32 accessor_read(const gchar *accessor_name,
    gint32 offset, gsize length, gpointer out_data);
extern gint32 accessor_write(const gchar *accessor_name,
    gint32 offset, gsize length, gconstpointer data_in);

extern gint32 chip_block_read(const gchar *chip_name, gint32 offset, gsize length, gpointer out_data);
extern gint32 chip_block_write(const gchar *chip_name, gint32 offset, gsize length, gpointer data_in);
extern gint32 chip_byte_read(const gchar *chip_name, gint32 offset, guint32 mask, gpointer out_data);
extern gint32 chip_byte_write(const gchar *chip_name, gint32 offset, guint32 mask, gpointer data_in);
extern gint32 chip_init(const gchar *chip_name);
extern gint32 chip_reset(const gchar *chip_name);
extern gint32 chip_test(const gchar *chip_name, gint32 *result);
extern gint32 dfl_chip_blk_write_read(const gchar* chip_name, gint32 input_length,
                                    gconstpointer input_data, gint32 output_length,
                                    gpointer output_data);
extern void error_print(const gchar* fmt, ...);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __HASLIB_H__ */


