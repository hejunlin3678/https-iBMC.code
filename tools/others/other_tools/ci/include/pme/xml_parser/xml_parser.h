/******************************************************************************

                  版权所有 (C), 2008-2009, 华为技术有限公司

 ******************************************************************************
  文 件 名   : xml_parser.h
  版 本 号   : 初稿
  部    门   :
  作    者   : zhouyubo@huawei.com
  生成日期   : 2013年7月20日
  最近修改   :
  功能描述   : XML文件解析对外头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年7月20日
    作    者   : zhouyubo@huawei.com
    修改内容   : 创建文件

******************************************************************************/
#ifndef XML_PARSER_H
#define XML_PARSER_H

#include <pme/common/mscm_macro.h>
#include <glib.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */
/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
#define XML_MAX_NAME_LEN            32
#define XML_MAX_OBJECT_NAME_LEN     XML_MAX_NAME_LEN
#define XML_MAX_CLASS_NAME_LEN      XML_MAX_NAME_LEN
#define XML_MAX_TYPE_NAME_LEN       XML_MAX_NAME_LEN
#define XML_MAX_PROPERTY_NAME_LEN   XML_MAX_NAME_LEN
#define XML_MAX_METHOD_NAME_LEN     XML_MAX_NAME_LEN
#define XML_MAX_PARA_NAME_LEN       XML_MAX_NAME_LEN
#define XML_MAX_ACCESSOR_NAME_LEN   XML_MAX_NAME_LEN
#define XML_MAX_MODE_NAME_LEN       4

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
typedef struct dynamic_xml_object{
    gchar  class_name[XML_MAX_CLASS_NAME_LEN];
    gchar  obj_name[XML_MAX_OBJECT_NAME_LEN];
    GSList* property_list;
}DYNAMIC_XML_OBJECT_S;

typedef struct dynamic_xml_object_property{
    gchar   name[XML_MAX_NAME_LEN];
    gchar   type[XML_MAX_NAME_LEN];
    gchar* value_list;
}DYNAMIC_XML_OBJECT_PROPERTY_S;


typedef struct tag_xml_object_file
{
    GSList *object_list;
}XML_OBJECT_FILE_S;

typedef struct tag_xml_object
{
    gchar name[XML_MAX_OBJECT_NAME_LEN];
    gchar class_name[XML_MAX_CLASS_NAME_LEN];
    gchar type[XML_MAX_TYPE_NAME_LEN];
    gchar ref_obj[XML_MAX_OBJECT_NAME_LEN];
    gchar mutex_obj[XML_MAX_OBJECT_NAME_LEN];
    GSList *property_list;
}XML_OBJECT_S;

typedef struct tag_xml_object_property
{
    gchar name[XML_MAX_PROPERTY_NAME_LEN];
    gchar type[XML_MAX_TYPE_NAME_LEN];
    GSList *value_list;
    glong size;
    gchar relation_accessor[XML_MAX_ACCESSOR_NAME_LEN];
    gchar mode[XML_MAX_MODE_NAME_LEN];
}XML_OBJECT_PROPERTY_S;

typedef struct tag_xml_method_property
{
    gchar name[XML_MAX_PROPERTY_NAME_LEN];
    gchar type[XML_MAX_TYPE_NAME_LEN];
    guint16 id;                     /* method id */
    GSList *value_list;
}XML_METHOD_PROPERTY_S;

typedef struct tag_xml_class_property
{
    gchar name[XML_MAX_PROPERTY_NAME_LEN];
    gchar type[XML_MAX_TYPE_NAME_LEN];
    guint16 id;                     /* property id */
    guint8 exported;        /* property export info */
    guint8 key;           /* property key info */
}XML_CLASS_PROPERTY_S;

typedef enum tag_xml_class_method_para_dir
{
    XML_CLASS_METHOD_PARA_IN = 0,   /* The input parameter */
    XML_CLASS_METHOD_PARA_OUT       /* The output parameter */
}XML_CLASS_METHOD_PARA_DIR_E;

typedef struct tag_xml_class_tree
{
    GNode *class_node;              /* The root of class-tree */
}XML_CLASS_TREE_S;

typedef struct tag_xml_class_data
{
    gchar name[XML_MAX_CLASS_NAME_LEN];
    guint16 id;                     /* class id */
    GSList *property_list;
    GSList *method_list;
    guint32 class_pos;
}XML_CLASS_DATA_S;

typedef struct tag_xml_class_method
{
    gchar name[XML_MAX_METHOD_NAME_LEN];
    guint16 id;                     /* method id */
    GSList *para_list;
    GSList *property_list;
}XML_CLASS_METHOD_S;

typedef struct tag_xml_class_method_para
{
    gchar name[XML_MAX_PARA_NAME_LEN];
    gchar type[XML_MAX_TYPE_NAME_LEN];
    guint16 id;                     /* parameter id */
    XML_CLASS_METHOD_PARA_DIR_E dir;
}XML_CLASS_METHOD_PARA_S;

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/
 /*****************************************************************************
 函 数 名  : xml_create_object_file
 功能描述  : 创建动态添加对象的XML文件
 输入参数  : GSList* dynamic_object_list  : 动态添加对象链表
             const gchar* filename        : 创建的XML文件名(全路径)
 输出参数  : 无
 返 回 值  : gint32     创建成功返回0，失败返回-1
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2015年10月10日
    作    者   : lingmingqiang
    修改内容   : 新生成函数

*****************************************************************************/
gint32 xml_create_object_file( GSList* dynamic_object_list, const gchar* filename);

/*****************************************************************************
 函 数 名  : xml_open_object_file
 功能描述  : 打开对象XML文件
 输入参数  : const gchar *file_name：对象文件名
 输出参数  : 无
 返 回 值  : 对象XML文件访问句柄
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月20日
    作    者   : zhouyubo@huawei.com
    修改内容   : 新生成函数

*****************************************************************************/
XML_OBJECT_FILE_S *xml_open_object_file(const gchar *file_name);

/*****************************************************************************
 函 数 名  : xml_close_object_file
 功能描述  : 关闭对象XML文件
 输入参数  : XML_OBJECT_FILE_S *fp：对象XML文件访问句柄
 输出参数  : 无
 返 回 值  : 0--成功；其他--失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月20日
    作    者   : zhouyubo@huawei.com
    修改内容   : 新生成函数

*****************************************************************************/
gint32 xml_close_object_file(XML_OBJECT_FILE_S *fp);

/*****************************************************************************
 函 数 名  : xml_open_class_tree
 功能描述  : 打开类树XML文件
 输入参数  : const gchar *file_name :文件名
 输出参数  : 无
 返 回 值  : 成功返回类树指针；失败返回NULL
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月20日
    作    者   : zhouyubo@huawei.com
    修改内容   : 新生成函数

*****************************************************************************/
XML_CLASS_TREE_S *xml_open_class_tree(const gchar *file_name);

/*****************************************************************************
 函 数 名  : xml_close_class_tree
 功能描述  : 关闭类树XML文件
 输入参数  : XML_CLASS_TREE_S *fp : 类树指针
 输出参数  : 无
 返 回 值  : 0--成功；非0--失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月20日
    作    者   : zhouyubo@huawei.com
    修改内容   : 新生成函数

*****************************************************************************/
gint32 xml_close_class_tree(XML_CLASS_TREE_S *fp);

/*****************************************************************************
 函 数 名  : xml_append_property
 功能描述  : 添加属性
 输入参数  :
             GSList* property_list
             const gchar* new_property_name
             const gchar type
             glong size
             const gchar* vulue_string
 输出参数  : 无
 返 回 值  : void

 修改历史      :
  1.日    期   : 2014年10月20日
    作    者   : hejun 00168999
    修改内容   : 新生成函数

*****************************************************************************/
void xml_append_property(GSList* property_list,
                                       const gchar* new_property_name, const gchar type,
                                       glong size, const gchar* value_string);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
