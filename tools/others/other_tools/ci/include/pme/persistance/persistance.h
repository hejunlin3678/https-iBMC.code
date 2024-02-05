/******************************************************************************

                  版权所有 (C), 2008-2009, 华为技术有限公司

 ******************************************************************************
  文 件 名   : persistance.h
  版 本 号   : 初稿
  部    门   :
  作    者   : zhouyubo@huawei.com
  生成日期   : 2013年7月24日
  最近修改   :
  功能描述   : 持久化对外头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年7月24日
    作    者   : zhouyubo@huawei.com
    修改内容   : 创建文件

******************************************************************************/
#ifndef PERSISTANCE_H
#define PERSISTANCE_H

#include <pme/common/mscm_macro.h>
#include "persistance_err.h"
#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define PER_VERSION 1

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)   (sizeof(x)/sizeof(x[0]))
#endif

#define PER_MAX_KEY_LEN 32
#define CFG_KEY_LEN     32
#define CFG_OBJECT_LEN  32
#define CFG_TYPE_LEN    32
#define CFG_VALUE_LEN   256

#define PER_MODULE_NAME "per"

#define PER_CONFIG_IN_SHORT_FILE    "per_config_in.ini"
#define PER_FACTORY_SET_SHORT_FILE  "factory_settings.ini"

#define PER_CONFIG_INPORT_FILE      RAMFS_PATH"per_config_in.ini"
#define PER_CONFIG_FACTORY_FILE     RAMFS_PATH"per_factory_settings.ini"

#define PER_CONFIG_SHORT_FILE               "config.ini"
#define PER_CFG_NODEF_RESET_SHORT_FILE      "config_reset.ini"
#define PER_CFG_NODEF_PERMANENT_SHORT_FILE  "config_permanent.ini"
#define PER_DEF_CONFIG_SHORT_FILE           "def_config.ini"
#define PER_CFG_DEF_RESET_SHORT_FILE        "def_config_reset.ini"
#define PER_CFG_DEF_PERMANENT_SHORT_FILE    "def_config_permanent.ini"
#define PER_CFG_STATUS_SHORT_FILE               "config_status.ini"

#define PER_CONFIG_FILE                 RAMFS_PATH"per_config.ini"
#define PER_CFG_NODEF_RESET_FILE        RAMFS_PATH"per_config_reset.ini"
#define PER_CFG_NODEF_PERMANENT_FILE    RAMFS_PATH"per_config_permanent.ini"
#define PER_CFG_DEF_PERMANENT_FILE      RAMFS_PATH"per_def_config_permanent.ini"
#define PER_CONFIG_DEF_FILE             RAMFS_PATH"per_def_config.ini"
#define PER_CFG_DEF_RESET_FILE          RAMFS_PATH"per_def_config_reset.ini"
#define PER_CFG_STATUS_FILE            RAMFS_PATH"per_config_status.ini"

#define PER_CONFIG_FLASH_FILE       FLASHFS_PATH"per_config.ini"

#define GLOBAL_NV_STORAGE_NAME      "GlobalNVStorage"

typedef enum tag_PER_MODE_E
{
    PER_MODE_NOSUPPORT = -1,/* 无需持久化 */
    PER_MODE_POWER_OFF = 0, /* 掉电持久化 */
    PER_MODE_RESET,       /* 复位持久化 */
    PER_MODE_PERMANENT,   /* 永久持久化 */
    PER_MODE_BUTT
}PER_MODE_E;

typedef struct tag_PER_S
{
    gchar *key;     /* 属性名           */
    PER_MODE_E mode;/* 持久化模式       */
    guint8 *data;   /* 属性值变量地址   */
    gint len;       /* 属性值长度       */
}PER_S;

typedef struct tag_CFG_READ_S
{
    gchar *type;        /* 类型 */
    guint32 size;       /* 数据字节长度 */
    guint8 *data_out;   /* 数据 */
}CFG_READ_S;

typedef gint32 (*CFG_READ_FUNC_S)(const gchar *obj_name, const gchar *property, const gchar *type, guint32 size, const guint8 *value);
typedef gint32 (*CFG_FOREACH_FUNC_S)(const gchar *obj_name, const gchar *property, const gchar *type, guint32 size, const guint8 *value, gpointer user_data);
typedef gint32 (*CFG_STA_FOREACH_FUNC_S)(const gchar *obj_name, const gchar *property, guint32 status, gpointer user_data);

/*****************************************************************************
 函 数 名  : cfg_init
 功能描述  : 初始化配置项
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 初始化成功返回PER_OK；失败返回<0的错误码
 调用函数  :
 被调函数  :
 说明      :

 修改历史      :
  1.日    期   : 2013年7月24日
    作    者   : z00198732
    修改内容   : 新生成函数
  1.日    期   : 2013年8月10日
    作    者   : z00198732
    修改内容   : 变更接口，只供框架调用

*****************************************************************************/
gint32 cfg_init(void);

/*****************************************************************************
 函 数 名  : cfg_save
 功能描述  : 持久化指定对象的属性
 输入参数  : const gchar *obj_name: 对象名
             const gchar *property: 属性名
             const gchar *type : 类型
             guint32 size : 属性长度
             const guint8 *data : 属性数据
 输出参数  : 无
 返 回 值  : 初始化成功返回PER_OK；失败返回<0的错误码
 调用函数  :
 被调函数  :
 说明      :

 修改历史      :
  1.日    期   : 2013年7月24日
    作    者   : z00198732
    修改内容   : 新生成函数
  1.日    期   : 2013年8月10日
    作    者   : z00198732
    修改内容   : 变更接口，只供框架调用

*****************************************************************************/
gint32 cfg_save(const gchar *obj_name,
                const gchar *property,
                const gchar *type,
                guint32 size,
                const guint8 *data);

/*****************************************************************************
 函 数 名  : cfg_read
 功能描述  : 持久化某个已注册的内存块
 输入参数  : const gchar *obj_name: 对象名
             const gchar *property: 属性名
 输出参数  : CFG_READ_S *data: 属性数据
 返 回 值  : 获取成功返回PER_OK；失败返回错误码
 调用函数  :
 被调函数  :
 说明      :

 修改历史      :
  1.日    期   : 2013年7月24日
    作    者   : z00198732
    修改内容   : 新生成函数
  1.日    期   : 2013年8月10日
    作    者   : z00198732
    修改内容   : 变更接口，只供框架调用

*****************************************************************************/
gint32 cfg_read(const gchar *obj_name,
                const gchar *property,
                CFG_READ_S *data);

/*****************************************************************************
 函 数 名  : cfg_read_free
 功能描述  : 释放读取的数据
 输入参数  : CFG_READ_S *data
 返 回 值  : 获取成功返回PER_OK；失败返回错误码
 调用函数  :
 被调函数  :
 说明      :

 修改历史      :
  1.日    期   : 2013年8月10日
    作    者   : z00198732
    修改内容   : 新建接口，只供框架调用

*****************************************************************************/
gint32 cfg_read_free(CFG_READ_S *data);

/*****************************************************************************
 函 数 名  : cfg_read_each
 功能描述  : 遍历所有对象属性
 输入参数  : CFG_READ_FUNC_S func: 遍历回调函数
 输出参数  : 无
 返 回 值  : 获取成功返回PER_OK；失败返回错误码
 调用函数  :
 被调函数  :
 说明      :

 修改历史      :
  1.日    期   : 2013年8月20日
    作    者   : z00198732
    修改内容   : 新建函数

*****************************************************************************/
gint32 cfg_read_each(CFG_READ_FUNC_S func);

gint32 cfg_save_v2(const gchar *obj_name,
                const gchar *property,
                const gchar *type,
                guint32 size,
                const guint8 *data,
                gboolean is_default,
                PER_MODE_E per_mode);

gint32 cfg_read_v2(const gchar *obj_name,
                const gchar *property,
                CFG_READ_S *data,
                gboolean is_default,
                PER_MODE_E per_mode);

gint32 cfg_is_saved(const gchar *obj_name,
                const gchar *property,
                gboolean is_default,
                PER_MODE_E per_mode,
                gboolean *saved);

#define cfg_read_free_v2 cfg_read_free
gint32 cfg_read_each_v2(CFG_READ_FUNC_S func, gboolean is_default, PER_MODE_E per_mode);
gint32 cfg_foreach(CFG_FOREACH_FUNC_S func, gboolean is_default, PER_MODE_E per_mode, gpointer user_data);

gint32 cfg_set_nvdata(gpointer nvdata, gsize size);
gint32 cfg_exit(void);
gint32 cfg_remove(GHashTable* hashtable, gboolean is_default, PER_MODE_E per_mode);
gint32 cfg_status_save(const gchar *obj_name, const gchar *property, guint32 status);
gint32 cfg_status_remove(const gchar *obj_name, const gchar *property);
gint32 cfg_status_foreach(CFG_STA_FOREACH_FUNC_S func, gpointer user_data);

#if (PER_VERSION >= 2)
/*****************************************************************************
 函 数 名  : cfg_export_single
 功能描述  : 配置导出接口
 输入参数  : const gchar *obj_name: 对象名
             const gchar *property: 属性名
             const gchar *file_name : 导出文件名(完整路径)
 输出参数  : 无
 返 回 值  : 获取成功返回PER_OK；失败返回错误码
 调用函数  :
 被调函数  :
 说明      :

 修改历史      :
  1.日    期   : 2013年8月19日
    作    者   : z00198732
    修改内容   : 新生成函数

*****************************************************************************/
gint32 cfg_export_single(const gchar *obj_name, const gchar *property, const gchar *file_name);

/*****************************************************************************
 函 数 名  : cfg_export_all
 功能描述  : 全部配置导出
 输入参数  : const gchar *file_name : 导出文件名(完整路径)
 输出参数  : 无
 返 回 值  : 获取成功返回PER_OK；失败返回错误码
 调用函数  :
 被调函数  :
 说明      :

 修改历史      :
  1.日    期   : 2013年8月19日
    作    者   : z00198732
    修改内容   : 新生成函数

*****************************************************************************/
gint32 cfg_export_all(const gchar *file_name);

/*****************************************************************************
 函 数 名  : cfg_import_signgle
 功能描述  : 单项配置导出
 输入参数  : const gchar *obj_name: 对象名
             const gchar *property: 属性名
             const gchar *file_name : 导出文件名(完整路径)
 输出参数  : 无
 返 回 值  : 获取成功返回PER_OK；失败返回错误码
 调用函数  :
 被调函数  :
 说明      :

 修改历史      :
  1.日    期   : 2013年8月19日
    作    者   : z00198732
    修改内容   : 新生成函数

*****************************************************************************/
gint32 cfg_import_signgle(const gchar *obj_name, const gchar *property, const gchar *file_name);

/*****************************************************************************
 函 数 名  : cfg_import_all
 功能描述  : 所有配置导出
 输入参数  : const gchar *file_name : 导出文件名(完整路径)
 输出参数  : 无
 返 回 值  : 获取成功返回PER_OK；失败返回错误码
 调用函数  :
 被调函数  :
 说明      :

 修改历史      :
  1.日    期   : 2013年8月19日
    作    者   : z00198732
    修改内容   : 新生成函数

*****************************************************************************/
gint32 cfg_import_all(const gchar *file_name);
#endif

#ifdef UNIT_TEST
/*****************************************************************************
 函 数 名  : per_exit
 功能描述  : 清除已初始化的持久化项。该接口主要在调试时使用
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 初始化成功返回0；失败返回<0的错误码
 调用函数  :
 被调函数  :
 说明      :

 修改历史      :
  1.日    期   : 2013年7月24日
    作    者   : z00198732
    修改内容   : 新生成函数

*****************************************************************************/
gint32 per_exit(void);
#endif

/*****************************************************************************
 函 数 名  : per_init
 功能描述  : 数据持久化初始化
 输入参数  : PER_S *per_tbl : 持久化项目配置表
             guint32 count : 待初始化的持久化项目个数
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
gint32 per_init(PER_S *cfg_tbl, guint32 count);

/*****************************************************************************
 函 数 名  : per_save
 功能描述  : 持久化某个已注册的配置内存块
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 初始化成功返回0；失败返回<0的错误码
 调用函数  :
 被调函数  :
 说明      :

 修改历史      :
  1.日    期   : 2013年7月24日
    作    者   : z00198732
    修改内容   : 新生成函数

*****************************************************************************/
gint32 per_save(guint8 *data);

/*****************************************************************************
 函 数 名  : per_read
 功能描述  : 获取持久化的值，输出到指定区域
 输入参数  : guint8 *data : 注册变量地址
 输出参数  : guint8 *data_out : 输出地址
 返 回 值  : 获取成功返回0；失败返回错误码
 调用函数  :
 被调函数  :
 说明      :

 修改历史      :
  1.日    期   : 2013年7月29日
    作    者   : z00198732
    修改内容   : 新生成函数

*****************************************************************************/
gint32 per_read(guint8 *data, guint8 *data_out, gint32 len, gint32 out_len);

gint32 per_exit(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

