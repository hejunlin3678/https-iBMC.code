/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : xmlparser.h
  版 本 号   : 初稿
  作    者   : chenzhan
  生成日期   : 2016年6月8日
  最近修改   :
  功能描述   : xmlparser.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2016年6月8日
    作    者   : chenzhan
    修改内容   : 创建文件

******************************************************************************/
#ifndef __XMLPARSER_H__
#define __XMLPARSER_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

typedef struct property_record
{
    gchar *  classname;                      /* 类名 */
    guint32  classid;                          /* 类ID */
    gchar *  objectname;                     /* 对象名 */
    gchar *  propertyname;                   /* 属性名 */
    guint32  propertyid;                /* 属性信息号 */
    gchar *  propertyvalue;                  /* 属性值 */
    gchar *  specifyvalue;
    guint8   comment;                       /*是否注释掉*/
    struct property_record * pnextproperty;    /* 下一个属性 */
}STRUCT_PROPERTY_RECORD;

typedef struct head_record
{
    gchar *  productname;                      /* 产品名称 */
    gchar *  productsn;                     /* sn */
    gchar *  productid;                      /*产品ID*/
    gchar*  sysconfigversion;                /*系统配置文件版本号*/
    gchar*  systemmode;                      /*系统模式是单系统还是双系统*/
    gchar*  nodemode;                        /*区分8100在双系统模式下的主从BMC*/
    gchar*  timestamp;                               /* 时间 */
}STRUCT_HEAD_RECORD;

typedef struct component_record
{
    gchar *  componentname;                      /* 组件名 */
    gchar *  componetsoftver;
    gchar *  componetconfigver;
    guint32 componentid;
    struct property_record * ppropertylist;     /* 属性列表 */
    struct component_record * pnextcomponent;
}STRUCT_COMPONENT_RECORD;

typedef struct config_record
{
    gchar *  XMLname;                      /* 文件名 */
    struct head_record * phead;    /* 属性列表 */
    struct component_record * pcomponentlist;
}STRUCT_CONFIG_RECORD;


#define MAX_FILE_LEN  256
#define XML_ROOT_STRING "SystemConfiguration"
#define XML_COMPONENT_STRING "Component"
#define XML_PRPPERTY_STRING "Attribute"

#define XML_ATTRIBUTE_NAME "Name"
#define XML_ATTRIBUTE_SOFTWARE_VERSION "SoftwareVersion"
#define XML_ATTRIBUTE_CONFIG_VERSION "ConfigVersion"
#define XML_ATTRIBUTE_CLASS "Class"
#define XML_ATTRIBUTE_SPECIFICPROPERTY "Key"

#define PRODUCT_NAME_STRING  "ProductName"
#define PRODUCT_SN_STRING  "ProductSN"
#define PRODUCT_UNIQUE_ID  "ProductUniqueID"
#define BMC_SYSTEM_MODE        "SystemMode"
#define BMC_NODE_MODE          "Master"
#define TIME_STAMP_STRING "TimeStamp"
#define SYS_CONFIG_VER_STRING "ConfigVersion"

extern gint32 xml_debug_printf_config(struct config_record* ppropertyList);
extern gint32 xml_free_all_config_struct(struct config_record* ppropertyList);
extern gint32 xml_free_component_struct(struct component_record* pcomponent);
extern gint32 xml_free_property_struct(struct property_record* pproperty);
extern gint32 xml_free_property_struct_list(struct property_record* ppropertylist);
extern gint32 xml_read_file(const gchar* filename, struct config_record** ppropertyList);
extern gint32 xml_write_file(struct config_record *pconfig, const gchar* filename);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __XMLPARSER_H__ */


