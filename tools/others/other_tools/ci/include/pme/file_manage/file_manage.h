/******************************************************************************

                  版权所有 (C), 2013-2015, 华为技术有限公司

 ******************************************************************************
  文 件 名   : file_manage.h
  版 本 号   : 初稿
  部    门   :
  作    者   : zhouyubo@huawei.com
  生成日期   : 2013年7月23日
  最近修改   :
  功能描述   : 文件管理对外头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年7月23日
    作    者   : zhouyubo@huawei.com
    修改内容   : 创建文件

******************************************************************************/
#ifndef FILE_MANAGE_H
#define FILE_MANAGE_H

#include <pme/common/mscm_macro.h>
#include "file_manage_err.h"
#include <glib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define FM_APP_VERSION  1
#define FM_MUTEX_WAIT_KEEP          0

#define FM_MAX_PATH_LENGTH             256
#define FM_MAX_SHORT_NAME_LENGTH       64
#define FM_MAX_MODULE_NAME_LENGTH      32
#define FM_MAX_MODE_LEN                6
#define FM_MAX_CMD_LENGTH           (FM_MAX_PATH_LENGTH * 3)

#define FMAPP_IPMI_SUBCMD_FACTORY_SET   0
#define FMAPP_IPMI_SUBCMD_FACTORY_LOAD  1

#ifdef ARM_PLATFORM
/* 内存文件默认路径(可复位持久化) */
#define RAMFS_PATH      "/opt/pme/pram/"

/* 临时文件路径(复位丢失) */
#define TMPFS_PATH      "/opt/pme/pram/"

/* Flash文件默认路径 */
#define FLASHFS_PATH    "/opt/pme/save/"
#else
/* 默认 X86 */
#define RAMFS_PATH      "/tmp/ramfs/"

/* 临时文件路径 */
#define TMPFS_PATH      "/tmp/"

/* Flash文件默认路径 */
#define FLASHFS_PATH    "./flashfs/"
#endif

#define FACTORY_SETTING_FLAG_FILE   FLASHFS_PATH".LoadCfg.flag"
#define FM_FACTORY_RECOVER_POINT_FILE ".factory_recover_point.tar.gz"
#define FM_FACTORY_RECOVER_POINT_FILE1 ".factory_recover_point1.tar.gz"
#define FM_FACTORY_RECOVER_POINT_PATH FLASHFS_PATH".factory_recover_point.tar.gz"
#define FM_FACTORY_RECOVER_POINT_PATH1 FLASHFS_PATH".factory_recover_point1.tar.gz"
#define FM_FACTORY_PACKAGE_PATH     TMPFS_PATH".FmCfg"
#define FM_REBOOT_FLAG_FILE         "/tmp/.Reboot.flag" //Added by licheng, 2015/4/7 11:28:23   问题单号:DTS2015010907384
#define CFG_RECOVER_FLAG_FILE "/opt/pme/pram/.CfgRecover.flag"  //Added by licheng, 2015/8/15 20:21:7   问题单号:DTS2015073105547 */

#define FM_BACK_UP_EXTENTION    "bak"
#define FM_CHECK_SUM_EXTENTION  "md5"
#define FM_CHECK_SUM_EXTENTION_E  "sha256"


/* 配置设置标记长度 */
#define CFG_SETTING_FLAG_LEN    32

/* 出厂设置标记值 */
#define FACTORY_SETTING_FLAG "Factory"

/* 配置导入标记值 */
#define CFG_IMPORT_SETTING_FLAG "Import"

/* 配置清除标记 */
#define CFG_CLEAR_SETTING_FLAG "Cleared"

/* BEGIN: Added by licheng, 2015/5/8 16:34:1   问题单号:DTS2015050705315 */
/* 同步文件的条目名 */
#define FMAPP_SRC_ITEM_NAME "src"
#define FMAPP_DST_ITEM_NAME "dst"
#define FMAPP_BAK_ITEM_NAME "bak"
#define FMAPP_CRC_ITEM_NAME "crc" //md5
#define FMAPP_CRC2_ITEM_NAME "crc2" //sha256
#define FMAPP_BAKCRC_ITEM_NAME "bakcrc" //md5
#define FMAPP_BAKCRC2_ITEM_NAME "bakcrc2" //sha256

/*文件后缀名类型*/
typedef enum tag_FM_SUFFIX_E
{
    FM_SUFFIX_MD5 = 0,  /* md5校验文件后缀 */
    FM_SUFFIX_SHA56,    /* sha256校验文件后缀 */
    FM_SUFFIX_BAK,      /* 备份文件后缀 */
    FM_SUFFIX_BUFF
}FM_SUFFIX_E;

#define FMAPP_PRIV_ITEM_NAME   "privilege"
/* END:   Added by licheng, 2015/5/8 16:34:5 */

/* 文件同步注册文件 */
#define FSYNC_REGISTER_FILE  TMPFS_PATH"fsync_reg.ini"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)   (sizeof(x)/sizeof(x[0]))
#endif

/* 文件管理保护等级 */
typedef enum tag_FM_PROTECT_E
{
    FM_PROTECT_RESET,       /* 复位保护 */
    FM_PROTECT_POWER_OFF,   /* 掉电保护 */
    FM_PROTECT_POWER_OFF_E, /* 增强型掉电保护（带冗余备份） */
    FM_PROTECT_POWER_OFF_S, /* 具有FM_PROTECT_POWER_OFF_E的功能，且作为出厂配置保存 */
    FM_PROTECT_PERMANENT, /* 保留flash保护（带冗余备份） */
    FM_PROTECT_FACTORY, /*仅作为出厂配置保存，不带持久化、校验和备份*/
    FM_PROTECT_BUTT
}FM_PROTECT_E;

/* 实际文件位置 */
typedef enum tag_FM_POSITION_E
{
    FM_POSITION_IN_RAM = 0,  /* 在内存中 */
    FM_POSITION_IN_FLASH,    /* 在Flash普通区中 */
    FM_POSITION_IN_FLASH_E,  /* 在Flash增强保护区中 */
    FM_POSITION_IN_RESERVE_FLASH,   /* 保留flash中 */
    FM_POSITION_BUTT
}FM_POSITION_E;

/* 文件权限 */
#define FM_PRIVILEGE_RGRP 0x0040
#define FM_PRIVILEGE_WGRP 0x0020
#define FM_PRIVILEGE_ROTH 0x0004
#define FM_PRIVILEGE_WOTH 0x0002

#define FM_PRIVILEGE_FULL (FM_PRIVILEGE_RGRP | FM_PRIVILEGE_WGRP | FM_PRIVILEGE_ROTH | FM_PRIVILEGE_WOTH)
#define FM_PRIVILEGE_LOWEST 0

#define FM_PRIVILEGE_GADMIN ((guint64)0 << 32)
#define FM_PRIVILEGE_GOPER  ((guint64)200 << 32)
#define FM_PRIVILEGE_GUSER  ((guint64)201 << 32)
#define FM_PRIVILEGE_GNOACC ((guint64)202 << 32)

/* 归档通知回调函数原型 */
typedef void (*FUNC_PACKAGE_NOTIFY_S)(gchar *);

/* 文件管理注册信息 */
typedef struct tag_FM_REGISTER_S
{
    gchar *short_name;/* 短文件名 */
    gchar *dst_name;  /* 目标文件（Flash文件）全文件名
                              （不指定的话设置为NULL，文件管理自动生成） */
    FM_PROTECT_E mode;      /* 保护等级 */
    guint32 package_size;   /* 文件归档尺寸，0表示不归档 */
    FUNC_PACKAGE_NOTIFY_S package_notify;  /* 归档通知接口。当触发归档时，通知APP */
    gchar *package_path;    /* 归档路径 */
    guint64 privilege; /* 访问权限 */
    gchar *callback_obj; /* 回调方法的类名 */
    gchar *callback_method; /* 回调的方法名 */
} FM_REGISTER_S;

typedef struct tag_FM_REG_INTER_INFO_S
{
    gchar ram_file[FM_MAX_PATH_LENGTH];
    gchar flash_file[FM_MAX_PATH_LENGTH];
    gchar flash_file_bak[FM_MAX_PATH_LENGTH];
    gchar flash_file_crc[FM_MAX_PATH_LENGTH];/*md5校验值文件*/
    gchar flash_file_crc_sha256[FM_MAX_PATH_LENGTH];/*sha256校验值文件*/
    gchar flash_file_bak_crc[FM_MAX_PATH_LENGTH];/*md5校验值文件*/
    gchar flash_file_bak_crc_sha256[FM_MAX_PATH_LENGTH];/*sha256校验值文件*/
    FM_PROTECT_E mode;
    guint64 privilege; /* 访问权限 */
    gchar *callback_obj;
    gchar *callback_method;
} FM_REG_INTER_INFO_S;

/* 文件访问句柄封装 */
typedef struct tag_FM_FILE_S
{
    FILE *fd;               /* 文件访问流指针 */
    gint handle;            /* 文件句柄 */
    gchar file_name[FM_MAX_PATH_LENGTH];       /* 文件路径(带文件名) */
    gchar simple_name[FM_MAX_PATH_LENGTH];     /* 文件名(不带路径) */
    gchar package_name[FM_MAX_PATH_LENGTH];    /* 打包文件名(不带路径) */
    gchar mode[FM_MAX_MODE_LEN];
    struct fm_reg_node_s *register_info;
} FM_FILE_S;

/* 文件写入量统计类型 */
typedef enum tag_FM_WRITE_CNT_E {
    FM_WR_CNT_SYNC,  /* 文件同步写入 */
    FM_WR_CNT_LOG,    /* LOG文件写入 */
    FM_WR_CNT_SET,  /* 其他直接写入 */
    FM_WR_CNT_INV
} FM_WR_CNT_E;

/*****************************************************************************
 函 数 名  : fm_init
 功能描述  : 文件管理初始化
 输入参数  : FM_REGISTER_S *file_cfg : 待初始化文件的管理配置信息
             gint32 count : 待初始化文件个数
 输出参数  : 无
 返 回 值  : 成功返回0。失败返回错误码。
 调用函数  :
 被调函数  :
 说明      :

 修改历史      :
  1.日    期   : 2013年7月4日
    作    者   : z00198732
    修改内容   : 新生成函数

*****************************************************************************/
gint32 fm_init(FM_REGISTER_S *file_cfg, guint32 count);

/*****************************************************************************
 函 数 名  : fm_init_cfg
 功能描述  : 文件管理初始化
 输入参数  : FM_REGISTER_S *file_cfg : 待初始化文件的管理配置信息
             gint32 count : 待初始化文件个数
             const gchar *module_name : 模块名
 输出参数  : 无
 返 回 值  : 成功返回0。失败返回错误码。
 调用函数  :
 被调函数  :
 说明      :

 修改历史      :
  1.日    期   : 2013年7月20日
    作    者   : z00198732
    修改内容   : 新生成函数

*****************************************************************************/
gint32 fm_init_cfg(FM_REGISTER_S *file_cfg, gint32 count, const gchar *module_name);

/*****************************************************************************
 函 数 名  : fm_exit
 功能描述  : 文件管理反初始化。释放之前初始化调用中分配的资源
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回0。失败返回错误码。
 调用函数  :
 被调函数  :
 说明      :

 修改历史      :
  1.日    期   : 2013年7月20日
    作    者   : z00198732
    修改内容   : 新生成函数

*****************************************************************************/
gint32 fm_exit(void);

/*****************************************************************************
 函 数 名  : fm_fopen
 功能描述  : 打开指定文件
 输入参数  : const gchar *short_name : 短文件名
             const gchar *mode : 打开模式。参考标准I/O fopen的接口定义
 输出参数  : 无
 返 回 值  : 成功返回FM_FILE_S结构体地址，失败返回NULL。
 调用函数  :
 被调函数  :
 说明      :

 修改历史      :
  1.日    期   : 2013年7月20日
    作    者   : z00198732
    修改内容   : 新生成函数

*****************************************************************************/
FM_FILE_S * fm_fopen ( const gchar * short_name, const gchar *mode);
FM_FILE_S * fm_fopen_file( const gchar * short_name, const gchar *module_name, const gchar *mode );
FM_FILE_S *fm_fopen_log_file(const FM_REGISTER_S *fm_info, const gchar *mode);

/*****************************************************************************
 函 数 名  : fm_fclose
 功能描述  : 关闭指定文件
 输入参数  : FM_FILE_S * stream : FM_FILE_S结构体地址
 输出参数  : 无
 返 回 值  : 0 ---关闭成功；非0---关闭失败
 调用函数  :
 被调函数  :
 说明      :

 修改历史      :
  1.日    期   : 2013年7月20日
    作    者   : z00198732
    修改内容   : 新生成函数

*****************************************************************************/
void fm_fclose(FM_FILE_S *stream);
void fm_fclose_log_file(FM_FILE_S *stream);

mode_t fm_create_privilege(guint32 privilege);

/*****************************************************************************
 函 数 名  : fm_fwrite
 功能描述  : 写某个文件
 输入参数  : const void * ptr : 待写入数据地址
             size_t size : 待写入数据块长度
             size_t count : 待写入数据块个数
 输出参数  : FM_FILE_S * stream : FM_FILE_S结构体地址
 返 回 值  : 成功返回实际写入数据块个数；失败返回错误码
 调用函数  :
 被调函数  :
 说明      :

 修改历史      :
  1.日    期   : 2013年7月20日
    作    者   : z00198732
    修改内容   : 新生成函数

*****************************************************************************/
size_t fm_fwrite ( const void * ptr, size_t size, size_t count, FM_FILE_S * stream );
size_t fm_fwrite_log(const void *ptr, size_t size, size_t count, FM_FILE_S *stream);

/*****************************************************************************
 函 数 名  : fm_fputc
 功能描述  : 插入一个字符
 输入参数  : int c
             FM_FILE_S *stream
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月20日
    作    者   : licheng
    修改内容   : 新生成函数

*****************************************************************************/
gint fm_fputc(int c, FM_FILE_S *stream);

/*****************************************************************************
 函 数 名  : fm_set_module_name
 功能描述  : 初始化模块名
 输入参数  : const gchar *module_name :模块名
 输出参数  : 无
 返 回 值  : 成功返回0。失败返回错误码。
 调用函数  :
 被调函数  :
 说明      :

 修改历史      :
  1.日    期   : 2013年7月4日
    作    者   : z00198732
    修改内容   : 新生成函数

*****************************************************************************/
gint32 fm_set_module_name( const gchar *module_name );

/*****************************************************************************
 函 数 名  : fm_get_module_name
 功能描述  : 获取模块名
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 模块名
 调用函数  :
 被调函数  :
 说明      :

 修改历史      :
  1.日    期   : 2013年8月21日
    作    者   : z00198732
    修改内容   : 新生成函数

*****************************************************************************/
const gchar *fm_get_module_name(void);

/*****************************************************************************
 函 数 名  : fm_clear_file
 功能描述  : 文件长度清0
 输入参数  : const gchar *short_name 短文件名
 输出参数  : 无
 返 回 值  : 成功返回FM_OK；失败返回错误码
 调用函数  :
 被调函数  :
 说明      :

 修改历史      :
  1.日    期   : 2013年9月6日
    作    者   : z00198732
    修改内容   : 新生成函数

*****************************************************************************/
gint32 fm_clear_file( const gchar *short_name );

/*****************************************************************************
 函 数 名  : fm_clear_module_file
 功能描述  : 文件长度清0
 输入参数  : const gchar *short_name
 输出参数  : 无
 返 回 值  : 成功返回FM_OK；失败返回错误码
 调用函数  :
 被调函数  :
 说明      :

 修改历史      :
  1.日    期   : 2015年3月21日
    作    者   : l00164716
    修改内容   : 新生成函数

*****************************************************************************/
gint32 fm_clear_module_file(const gchar *short_name, const gchar *module_name);
gint32 fm_clear_log_file(const FM_REGISTER_S *fm_info);

/*****************************************************************************
 函 数 名  : fm_get_new_file_path
 功能描述  : 将短文件名转换成带路径信息的实际文件名
 输入参数  : short_name :短文件名
             const gchar *module_name :模块名
             position : 实际文件位置，参考FM_POSITION_E类型定义
 输出参数  : 无
 返 回 值  : 实际文件名，失败返回NULL
 调用函数  :
 被调函数  :
 说明      : 该函数返回结果需要调用gfree释放

 修改历史      :
  1.日    期   : 2013年7月4日
    作    者   : z00198732
    修改内容   : 新生成函数

*****************************************************************************/
void fm_get_file_path(const gchar *short_name,
                        const gchar *module_name,
                        FM_POSITION_E position,
                        gchar *file_nm_buf,
                        gsize buf_len);

/*****************************************************************************
 函 数 名  : fm_insert/fm_delete/fm_modify
 功能描述  : 提供对文件内容的增删改操作
 输入参数  : gchar *contents
             gint32 insert_len
             gint32 offset
             FM_FILE_S *stream
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2014年11月20日
    作    者   : licheng
    修改内容   : 新生成函数

*****************************************************************************/
gint32 fm_insert(const gchar *contents, gsize insert_len, gsize offset, FM_FILE_S *stream);
gint32 fm_delete(gsize del_len, gsize offset, FM_FILE_S *stream);
gint32 fm_modify(const gchar *contents, gsize contents_len, gsize mod_offset, gsize mod_len, FM_FILE_S *stream);

gint32 fm_factory_load_req(gboolean set);
gint32 fm_recover_factory_cfg(const gchar *path);
gint32 fm_set_content(const gchar *file_name, const gchar *buf, gsize len);
void fm_set_privilege(const gchar *file_name, guint64 privilege);
/* BEGIN: Added by licheng, 2015/8/27 15:8:59   问题单号:DTS2015082704966 */
gint fm_sync_privilege(const gchar *src_file, const gchar *dst_file);
/* END:   Added by licheng, 2015/8/27 15:9:3 */

GKeyFile *fm_open_key_file( const char *file_name, gint fd, gboolean hold_lock);
gint32 fm_close_key_file( GKeyFile *keyfile, gint fd, gboolean hold_lock);
gint32 fm_writeback_key_file ( GKeyFile *key_file, const gchar *file_name );
gboolean fm_key_file_string_valid(const gchar *key);
void fm_file_lock(gint handle);
void fm_file_unlock(gint handle);
/* BEGIN: Added by licheng, 2015/5/8 16:34:27   问题单号:DTS2015050705315 */
gboolean is_ram_file_valid(const gchar *ram_file);
gboolean is_file_valid(const gchar *src_file, const gchar *ck_file);
/* END:   Added by licheng, 2015/5/8 16:34:30 */
void fm_wirte_cnt_set(FM_WR_CNT_E type, gsize len);
guint64 fm_wirte_cnt_get(FM_WR_CNT_E type);

/*****************************************************************************
 函 数 名  : fm_check_recovery_rambak
 功能描述  : 检测保留内存中的备份文件是否损坏，若损坏则进行恢复
 输入参数  : const gchar *src_name : 保留内存主文件名(全路径)
 输出参数  : 无
 返 回 值  : gint32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年1月26日
    作    者   : lingmingqiang
    修改内容   : DTS2016012802702  添加复位持久化文件操作前主备校验接口

*****************************************************************************/
gint32 fm_check_recovery_rambak(const gchar *src_name);
/*****************************************************************************
 函 数 名  : fm_recovery_flash_bak
 功能描述  : 恢复flash中备份文件的内容
 输入参数  : const gchar *src_name
 输出参数  : 无
 返 回 值  : gint32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年3月29日
    作    者   : lingmingqiang
    修改内容   : 新生成函数

*****************************************************************************/
gint32 fm_recovery_flash_bak(const gchar *src_name);

/*****************************************************************************
 函 数 名  : fm_get_package_size
 功能描述  : 获取日志压缩容量
 输入参数  : gchar* log_name: 日志的short_name
 输出参数  : guint32 * package_size 归档尺寸
 返 回 值  : gint32
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年4月11日
    作    者   : lingmingqiang
    修改内容   : 新生成函数

*****************************************************************************/
gint32 fm_get_package_size(gchar* log_name, guint32* package_size);


/* 以下宏函数封装了标准I/O，需要调用方保证入参检查.
   注意: 对文件的修改操作不能封装成简单的宏，必须要加锁保护 */
#define fm_fread(ptr, size, count, stream)  fread(ptr, size, count, stream->fd)
#define fm_fseek(stream, offset, origin )   fseek(stream->fd, offset, origin)
#define fm_fflush(stream)                   fflush(stream->fd)
#define fm_ftell(stream)                    ftell(stream->fd)
#define fm_fgetc(stream)                    fgetc(stream->fd)
#define fm_fgets(string, n, stream)         fgets(string, n, stream->fd)
#define fm_fileno(stream)                   fileno(stream->fd)
#define fm_fsync(stream)                    fsync(fileno(stream->fd))

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

