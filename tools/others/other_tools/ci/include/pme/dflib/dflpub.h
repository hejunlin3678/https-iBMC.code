/*******************************************************************
              Copyright 2008 - 2013, Huawei Tech. Co., Ltd.
			          ALL RIGHTS RESERVED
						  
Filename      : dflpub.h
Author        : s00120510
Creation time : 2014/1/9
Description   : 框架库对外部提供的公共定义 (同时内部也要使用)

Version       : 1.0
********************************************************************/
#ifndef __DFLPUB_H__
#define __DFLPUB_H__


// 类名、对象名、属性名称最大长度限制
// 对象名用户可输入长度为22字节(预留10字节作为前缀/后缀，由工具控制)
#define MAX_NAME_SIZE              32

// 表示最大的初始化顺序号
#define MAX_ORDER_INIT_NUM         (-1)

// 操作已加入到容器(哈希表)中的对象必须用句柄
typedef gint32 OBJ_HANDLE;
typedef gint32 OBJECT_FILE;
typedef gint32 OBJECT_ITEM;


typedef void (*PropertyForeachFunc)(OBJ_HANDLE object_handle, 
    const gchar* property_name, GVariant* value, gpointer user_data);


#define MAX_IPMICMD_FILTER_SIZE    256
typedef struct tag_ipmi_reg_info
{
    // table index
    guint8 index;
    // ipc 队列id
    guint32 ipc_id;
    // net function    
    guint8 netfn;
    // ipmi命令字
    guint8 cmd;   
    // 命令权限
    guint8 privilege;
    // 命令属于哪个功能
    gchar  role_func[MAX_NAME_SIZE];
    // ipmi命令数据区(data)最大长度
    guint8 max_length;
    // ipmi命令数据区最小长度
    guint8 min_length;
     // 过滤字符串
    gchar  filter[MAX_IPMICMD_FILTER_SIZE];
}IPMI_REG_DATA_S;




#endif /* __DFLPUB_H__ */

