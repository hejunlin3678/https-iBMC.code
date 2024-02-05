/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: app模块动态调用的框架库api函数接口定义.供app/用户进程调用
 * Author: s00120510
 * Create: 2018/12/22
 * Notes: dflapi.h
 * History:
 */

#ifndef __DFLAPI_H__
#define __DFLAPI_H__

#include "pme/dflib/dflpub.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*
 * Description: 给对象绑定别名
 *              该对象可以是属于本模块的对象，也可以是属于其他模块的对象
 *              即可以对所有无论是否属于本模块的对象绑定别名
 * Notes: 一个对象可以绑定多个别名，新的别名并不会覆盖老别名
 */
extern gint32 dfl_bind_object_alias(OBJ_HANDLE obj_handle, guint32 obj_alias);


/*
 * Description: 删除对象的某个别名
 */
extern gint32 dfl_unbind_object_alias(OBJ_HANDLE obj_handle, guint32 obj_alias);

/*
 * Description: 删除指定对象的所有别名
 */
extern void dfl_unbind_all_alias(OBJ_HANDLE obj_handle);

/*
 * Description: 根据对象别名获取对象
 */
extern gint32 dfl_get_binded_object(const gchar *class_name, guint32 obj_alias, OBJ_HANDLE *obj_handle);

/*
 * Description: 获取当前模块名称
 */
extern const gchar *dfl_get_module_name(void);


/*
 * Description: 给对象绑定一个私有成员，同一个对象只能绑定一个，后绑定的数据会替换先绑定的数据，
 *              同时，如果之前提供了释放函数，则替换时会自动调用替换函数释放原来的内存。
 *              如果没有提供释放函数，需要自己释放，注意使用时勿导致内存泄漏
 *              被绑定的数据生存期类似于全局变量，注意多线程环境访问时需要进行保护，确保线程安全性
 */
extern gint32 dfl_bind_object_data(OBJ_HANDLE obj_handle, gpointer user_data, GDestroyNotify destroy_func);


/*
 * Description: 通过对象句柄获得绑定的私有数据
 *              注意返回的数据是全局存在的，需要调用者确保多线程安全。多线程访问时需要加锁
 */
extern gint32 dfl_get_binded_data(OBJ_HANDLE obj_handle, gpointer *user_data);


/*
 * Description: 依据类名、别名获取绑定的私有数据
 *              注意返回的数据是全局存在的，需要调用者确保多线程安全。多线程访问时需要加锁
 */
extern gint32 dfl_get_data_by_alias(const gchar *class_name, guint32 obj_alias, gpointer *user_data);


/*
 * Description: 获取属性值
 *              1.如果属性本身是一个引用，则获取该引用的最终结果，
 *              要获得引用属性名称需要调用dfl_get_referenced_property
 *              2.如果属性指向硬件，且属性为基本数据类型(byte/short/int/double)，
 *              同时当前模块具有读写该硬件的权限，则从硬件获取
 *              3.如果属性指向硬件，当前模块不具有读写该硬件的权限，
 *              则从内存获取属性值，需要硬件宿主模块去扫描更新硬件值到
 *              内存中，若硬件宿主模块未能及时更新硬件值到内存，则接口获取
 *              值为内存值，很可能是无效值。
 *              DTS2016083105934: 在原来接口基础上，新增非root权限访问硬件特性，
 *              底层通过rpc调用方式，由代理完成数据访问
 * Notes: property_value(*)   函数返回成功后，需要调用g_variant_unref释放
 */
extern gint32 dfl_get_property_value(OBJ_HANDLE obj_handle, const gchar *property_name, GVariant **property_value);

/*
 * Description: 封装获取字节属性值
 *              如果属性本身是一个引用，则获取该引用的最终结果，
 *              相比原接口，不再使用gvarint变量传递属性值，减少动态申请释放。
 */
extern gint32 dal_get_property_value_byte(OBJ_HANDLE obj_handle, const gchar *property_name, guchar *out_val);

/*
 * Description: 封装获取int16属性值
 *              如果属性本身是一个引用，则获取该引用的最终结果，
 *              相比原接口，不再使用gvarint变量传递属性值，减少动态申请释放。
 */
extern gint32 dal_get_property_value_int16(OBJ_HANDLE obj_handle, const gchar *property_name, gint16 *out_val);

/*
 * Description: 封装获取uint16属性值
 *              如果属性本身是一个引用，则获取该引用的最终结果，
 *              相比原接口，不再使用gvarint变量传递属性值，减少动态申请释放。
 */
extern gint32 dal_get_property_value_uint16(OBJ_HANDLE obj_handle, const gchar *property_name, guint16 *out_val);

/*
 * Description: 封装获取int32属性值
 *              如果属性本身是一个引用，则获取该引用的最终结果，
 *              相比原接口，不再使用gvarint变量传递属性值，减少动态申请释放。
 */
extern gint32 dal_get_property_value_int32(OBJ_HANDLE obj_handle, const gchar *property_name, gint32 *out_val);

/*
 * Description: 封装获取uint32属性值
 *              如果属性本身是一个引用，则获取该引用的最终结果，
 *              相比原接口，不再使用gvarint变量传递属性值，减少动态申请释放。
 */
extern gint32 dal_get_property_value_uint32(OBJ_HANDLE obj_handle, const gchar *property_name, guint32 *out_val);

/*
 * Description: 封装获取double属性值
 *              如果属性本身是一个引用，则获取该引用的最终结果，
 *              相比原接口，不再使用gvarint变量传递属性值，减少动态申请释放。
 */
extern gint32 dal_get_property_value_double(OBJ_HANDLE obj_handle, const gchar *property_name, gdouble *out_val);
/*
 * Description: 封装获取string属性值
 *              如果属性本身是一个引用，则获取该引用的最终结果，
 *              相比原接口，不再使用gvarint变量传递属性值，减少动态申请释放。
 */
extern gint32 dal_get_property_value_string(OBJ_HANDLE obj_handle, const gchar *property_name, gchar *out_val,
    guint32 out_len);
/*
 * Description: 封装根据byte属性值查找对象的接口
 *              相比原接口，不再使用gvarint变量传递属性值，减少动态申请释放。
 */
extern gint32 dal_get_specific_object_byte(const gchar *class_name, const gchar *property_name, guchar in_prop_val,
    OBJ_HANDLE *obj_handle);
/*
 * Description: 封装根据int16属性值查找对象的接口
 *              相比原接口，不再使用gvarint变量传递属性值，减少动态申请释放。
 */
extern gint32 dal_get_specific_object_int16(const gchar *class_name, const gchar *property_name, gint16 in_prop_val,
    OBJ_HANDLE *obj_handle);
/*
 * Description: 封装根据uint16属性值查找对象的接口
 *              相比原接口，不再使用gvarint变量传递属性值，减少动态申请释放。
 */
extern gint32 dal_get_specific_object_uint16(const gchar *class_name, const gchar *property_name, guint16 in_prop_val,
    OBJ_HANDLE *obj_handle);
/*
 * Description: 封装根据int32属性值查找对象的接口
 *              相比原接口，不再使用gvarint变量传递属性值，减少动态申请释放。
 */
extern gint32 dal_get_specific_object_int32(const gchar *class_name, const gchar *property_name, gint32 in_prop_val,
    OBJ_HANDLE *obj_handle);
/*
 * Description: 封装根据uint32属性值查找对象的接口
 *              相比原接口，不再使用gvarint变量传递属性值，减少动态申请释放。
 */
extern gint32 dal_get_specific_object_uint32(const gchar *class_name, const gchar *property_name, guint32 in_prop_val,
    OBJ_HANDLE *obj_handle);
/*
 * Description: 封装根据double属性值查找对象的接口
 *              相比原接口，不再使用gvarint变量传递属性值，减少动态申请释放。
 */
extern gint32 dal_get_specific_object_double(const gchar *class_name, const gchar *property_name, gdouble in_prop_val,
    OBJ_HANDLE *obj_handle);
/*
 * Description: 封装根据string属性值查找对象的接口
 *              相比原接口，不再使用gvarint变量传递属性值，减少动态申请释放。
 */
extern gint32 dal_get_specific_object_string(const gchar *class_name, const gchar *property_name,
    const gchar *in_prop_val, OBJ_HANDLE *obj_handle);
/*
 * Description: 新增根据某个对象查找同级position指定类的第一个对象接口
 */
extern gint32 dal_get_specific_object_position(OBJ_HANDLE obj_in, const gchar *class_name, OBJ_HANDLE *obj_out);
/*
 * Description: 新增根据某个对象查找同级position指定类的所有对象接口
 * Notes: obj_handle 输入对象
 *        class_name 指定类名
 *        obj_list   输出对象链表，调用者需释放链表，内容无需释放
 */
extern gint32 dal_get_object_list_position(OBJ_HANDLE obj_handle, const gchar *class_name, GSList **obj_list);

/* ****************************************************************
     设置/查询属性选项
**************************************************************** */
/* --- 仅设置，无其他效果 */
#define DF_NONE 0u
/* --- 设置属性时同时持久化(掉电持久化) */
#define DF_SAVE 1u
/* --- 设置属性时同时持久化(永久持久化，MAC地址保存等) */
#define DF_SAVE_PERMANENT 2u
/* --- 设置属性时同时持久化(复位持久化，掉电后丢失) */
#define DF_SAVE_TEMPORARY 4u
/* -- 查询时从内存读取 */
#define DF_MEM 8u
/* --- 查询时从硬件读取/设置属性时同时生效到硬件 */
#define DF_HW 16u
/* -- 查询/设置自动依据xml来决定操作内存还是硬件 */
#define DF_AUTO 32u
/* ----查询值(硬件、持久化)的时候复制到当前值 */
#define DF_COPY 64u
/* ----强制修改属性值 用于调试 */
#define DF_OVERRIDE 128u
/* ----取消强制修改状态 */
#define DF_CANCEL_OVERRIDE 256u
/* ----设置属性持久化并加上static标记(卸载xml删除对象持久化不丢失) */
#define DF_SAVE_STATIC 512u

typedef guint32 DF_OPTIONS;

/*
 * Description: 设置对象的属性并广播属性变更事件
 *              1. 如果属性是引用其他属性，且被引用对象是属于当前模块的，则设置最终引用属性值。
 *              如果被引用对象不属于当前模块，返回只读错误
 *              2. 如果options包含DF_HW，
 *              (1) 此时若属性是指向硬件的(xml中指定)，同时当前模块拥有该硬件的读写权限，则会将值写入硬件
 *              (2) 如果属性没有指向硬件，会返回错误
 *              3. 如果options包含DF_SAVE，则属性值会保存在Flash文件系统中，确保掉电不丢失
 *              如果options包含DF_SAVE_PERMANENT，则属性值会保存在Flash保留分区，确保掉电、
 *              文件系统损坏不丢失(仅用于存出厂设置的MAC/UUID)，保留分区空间有限，不能保存过多内容
 *              如果options包含DF_SAVE_TEMPORARY，则属性值会保存在保留内存中，确保复位不丢失，但是掉电会丢失
 * Notes: 不管options是什么，始终会修改属性值
 */
extern gint32 dfl_set_property_value(OBJ_HANDLE obj_handle, const gchar *property_name, GVariant *property_value,
    DF_OPTIONS options);

/*
 * Description: 获取上一次保存(持久化)的属性值，和当前的属性值可能不一样
 *              仅获取掉电保存的属性值
 * Notes: property_value(*)  返回的GVariant*需要释放
 */
extern gint32 dfl_get_saved_value(OBJ_HANDLE obj_handle, const gchar *property_name, GVariant **property_value,
    DF_OPTIONS options);

/*
 * Description: DTS2015073105547
 *              获取上一次保存(持久化)的属性值，和当前的属性值可能不一样
 *              dfl_get_saved_value: 进程直接调用动态库访问持久化数据
 *              DTS2016083105934
 *              dfl_get_saved_value_v2: 除了兼容dfl_get_saved_value接口功能之外，
 *              还支持进程无root权限无法访问持久化数据的场景下使用，低层通过rpc
 *              调用方式，由代理完成数据访问(dfl_get_saved_value_remote);
 *              dfl_get_saved_value接口的options只支持PER_MODE_POWER_OFF，
 *              而v2接口的options可以支持PER_MODE_POWER_OFF
 *              /PER_MODE_POWER_OFF/PER_MODE_RESET三种情况
 * Notes: property_value(*)  返回的GVariant*需要释放
 */
extern gint32 dfl_get_saved_value_remote(OBJ_HANDLE obj_handle, const gchar *property_name, GVariant **property_value,
    DF_OPTIONS options);
extern gint32 dfl_get_saved_value_v2(OBJ_HANDLE obj_handle, const gchar *property_name, GVariant **property_value,
    DF_OPTIONS options);

/*
 * Description: 为避免对未做持久化的属性直接调用get_saved_value导致框架报错，
 *              提供接口给调用者先判断属性值是否做了持久化
 * Notes: 入参options: 默认填0
 *        返回值: true--属性值已做持久化保存，false--执行失败或属性值未持久化保存
 */
extern gboolean dfl_is_property_value_saved(OBJ_HANDLE obj_handle, const gchar *property_name, DF_OPTIONS options);

/*
 * Description: 保存属性，用于配置保存等功能，仅能保存自己管理的对象属性
 *              仅用于掉电保存
 */
extern gint32 dfl_save_property_value(OBJ_HANDLE obj_handle, const gchar *property_name);

/*
 * Description: DTS2015073105547
 *              保存属性，用于配置保存等功能，仅能保存自己管理的对象属性
 *              dfl_save_property_value: 进程直接调用动态库访问持久化数据
 *              DTS2016083105934
 *              dfl_save_property_value_v2: 在dfl_save_property_value接口
 *              基础上，新增一个DF_OPTIONS选项，而且在进程无root权限无法访
 *              问持久化数据的场景下使用，低层通过rpc调用方式，由代理完成
 *              数据访问(dfl_save_property_value_remote);新增options选项，
 *              原来dfl_save_property_value接口只针对PER_MODE_POWER_OFF情
 *              况默认进行处理，新的v2接口支持PER_MODE_POWER_OFF/
 *              PER_MODE_POWER_OFF/PER_MODE_RESET三种情况
 */
extern gint32 dfl_save_property_value_remote(OBJ_HANDLE obj_handle, const gchar *property_name, DF_OPTIONS options);
extern gint32 dfl_save_property_value_v2(OBJ_HANDLE obj_handle, const gchar *property_name, DF_OPTIONS options);

/*
 * Description: 同时获取对象的多个属性值
 *              本接口同时获得多个属性的快照，确保属性值是同一个状态下同步获取的。
 *              它和多次调用dfl_get_property_value的区别在于: 后者是在不同时刻获得属性值，
 *              这些属性值可能在状态上是不同步的
 * Notes: property_value_list(*) 输出list<GVariant*>   函数返回成功后，需要调用g_slist_free_full释放
 */
extern gint32 dfl_multiget_property_value(OBJ_HANDLE obj_handle, GSList *property_name_list,
    GSList **property_value_list);

/*
 * Description: 设置属性值的默认值
 *              有些属性需要配置默认值，例如IP地址。本接口用于在运行过程中修改默认值。
 *              通常应用于生产定制化场景
 */
extern gint32 dfl_set_property_default(OBJ_HANDLE obj_handle, const gchar *property_name, GVariant *property_value);

/*
 * Description: 获取属性的accessor名称
 *              查询属性关联的硬件accessor对象
 *              用户在不确定属性是否指向了硬件时，可以通过此接口来进行判断。
 *              若存在accessor，说明属性指向了硬件。
 */
extern gint32 dfl_get_property_accessor(OBJ_HANDLE object_handle, const gchar *property_name,
    gchar accessor[MAX_NAME_SIZE], gint32 length);

/*
 * Description: 获取对象属性的默认值
 *              部分属性存在默认值，如IP地址。
 *              本接口获得默认属性值，注意默认属性和当前属性不是同一个属性，
 *              它们的值可能是不一样的
 *              例如，对于IPAddr属性，用接口dfl_get_property_value获取的是当前IP地址的实际值，
 *              而用此接口获得的是IP地址的默认值
 * Notes: property_value(*) 函数返回成功后，需要调用g_variant_unref释放
 */
extern gint32 dfl_get_property_default(OBJ_HANDLE obj_handle, const gchar *property_name, GVariant **property_value);

/*
 * Description: 修改属性持久化类型true--静态持久化，false--动态持久化。
 */
extern gint32 dfl_set_property_save_type(OBJ_HANDLE obj_handle, const gchar *property_name, gboolean is_static);

/*
 * Description: 获取属性的默认属性名称
 *              获取某个属性的默认属性名称
 */
extern gint32 dfl_get_default_property_name(const gchar *property_name, gchar name[MAX_NAME_SIZE]);

/*
 * Description: 获取属性引用的目标属性，即仅获取属性的当前值(名称)而不获取最终结果
 * Notes: referencd_object[MAX_NAME_SIZE] --- 用数组是明确告知调用者所需内存空间大小
 */
extern gint32 dfl_get_referenced_property(OBJ_HANDLE obj_handle, const gchar *property_name,
    gchar referencd_object[MAX_NAME_SIZE], gchar referencd_property[MAX_NAME_SIZE],
    gint32 obj_length, gint32 prop_length);

/*
 * Description: 一个对象的某个属性可以引用另一个对象(名称)，通过自己的句柄和引用属性名称返回被引用的对象句柄
 */
extern gint32 dfl_get_referenced_object(OBJ_HANDLE obj_handle, const gchar *referenced_object_property,
    OBJ_HANDLE *referencd_handle);

/*
 * Description: 根据类名、属性值获得对象句柄
 *              当同时有多个对象匹配时，只返回找到第一个符合条件的对象。
 *              第一个符合条件的对象并不是名称排在最前面的对象，事实上这里没有任何排序。
 */
extern gint32 dfl_get_specific_object(const gchar *class_name, const gchar *property_name, GVariant *property_value,
    OBJ_HANDLE *obj_handle);

/*
 * Description: 调用其他模块管理的类的方法
 *              输入参数需要和方法的参数定义匹配，如果不匹配会返回"参数不匹配"错误
 *              对于具有变参的方法，当前接口不进行参数检查。需要由方法提供者来进行参数的处理。
 */
extern gint32 dfl_call_class_method(OBJ_HANDLE obj_handle, const gchar *method_name, GSList *user_data_list,
    GSList *input_list, GSList **output_list);

/*
 * Description: 查询类方法的属性
 *              方法也可以具备属性，例如方法的权限级别等。
 * Notes: property_value(*)   需要调用者释放
 */
extern gint32 dfl_get_method_property(const gchar *class_name, const gchar *method_name, const gchar *property_name,
    GVariant **property_value);

/*
 * Description: 远程调用其他节点的类方法
 *              本接口基于ip(10.0.0.1 10.0.0.2)地址远程调用其他节点的方法，用于8P等多BMC系统下的BMC之间的方法调用
 */
extern gint32 dfl_call_remote_class_method(const gchar *object_name, const gchar *method_name, GSList *user_data_list,
    GSList *input_list, GSList **output_list);

/*
 * Description: 远程调用其他节点的类方法
 *              本接口基于非固定ip地址远程调用其他节点的方法，用于9032等多BMC系统下的BMC之间的方法调用
 */
extern gint32 dfl_call_remote_class_method2(const gchar *ip, gint32 port, const gchar *object_name,
    const gchar *method_name, GSList *user_data_list, GSList *input_list, GSList **output_list);

/*
 * Description: 获取远程节点的对象属性值
 *              本接口基于ip(10.0.0.1 10.0.0.2)地址远程获取其他节点的属性值，用于8P等多BMC系统下的BMC之间的属性值查询
 */
extern gint32 dfl_get_remote_property_value(const gchar *object_name, GSList *input_list, GSList **output_list);

/*
 * Description: 获取远程节点的对象属性值
 *              本接口基于ip地址远程获取其他节点的属性值，用于9032等多BMC系统下的BMC之间的属性值查询
 */
extern gint32 dfl_get_remote_property_value2(const gchar *ip, gint32 port, const gchar *object_name, GSList *input_list,
    GSList **output_list);


/*
 * Description: 依据类名返回对象个数
 */
extern gint32 dfl_get_object_count(const gchar *class_name, guint32 *count);

/*
 * Description: 依据类名查询object列表，返回handle列表
 *              注意，此处返回的列表是静态的，当调用者获得列表之后，如果单板被拔走，
 *              部分对象可能会被删除，此时链表中部分handle就会失效。
 */
extern gint32 dfl_get_object_list(const gchar *class_name, GSList **list);

/* 遍历对象。获取同一类名的下一个对象。确保回调函数handler中不能挂起 */
/* 异常处理函数将获得遍历过程中错误返回值，注意，异常处理函数输入的object handle可能为空 */
typedef gint32 (*OBJECT_FOREACH_FUNC)(OBJ_HANDLE object_handle, gpointer user_data);
typedef void (*FOREACH_EXCEPTION_FUNC)(const gchar *class_name, OBJ_HANDLE object_handle, gpointer user_data,
    gint32 result);
/*
 * Description: 依据类名遍历回调对象处理，依次调用回调函数并输入对象句柄
 *              框架负责维护对象的动态添加删除
 */
extern gint32 dfl_foreach_object(const gchar *class_name, OBJECT_FOREACH_FUNC handler, gpointer user_data,
    FOREACH_EXCEPTION_FUNC exception_func);

/*
 * Description: 依据类名遍历回调对象处理，依次调用回调函数并输入对象句柄
 *              框架负责维护对象的动态添加删除, 会遍历子类对象
 * History: 1.2015年5月25日, hejun 00168999
 *          新生成函数
 */
extern gint32 dfl_foreach_object_family(const gchar *class_name, OBJECT_FOREACH_FUNC handler, gpointer user_data,
    FOREACH_EXCEPTION_FUNC exception_func);


/*
 * Description: 遍历一个对象的属性
 */
extern gint32 dfl_foreach_property(OBJ_HANDLE obj_handle, PropertyForeachFunc handler, gpointer user_data);


/*
 * Description: 模块定时向管理进程发送心跳
 */
extern gint32 dfl_heartbeat(void);

/*
 * Description: 根据句柄取类名
 */
extern gint32 dfl_get_class_name(OBJ_HANDLE obj_handle, gchar class_name[MAX_NAME_SIZE], gint32 length);

/*
 * Description: 根据句柄取对象名
 */
extern const gchar *dfl_get_object_name(OBJ_HANDLE obj_handle);

/*
 * Description: 根据对象名称获取对象句柄
 */
extern gint32 dfl_get_object_handle(const gchar *obj_name, OBJ_HANDLE *obj_handle);

/*
 * Description: 获取对象位置号
 *              位置号表示单板的物理位置。位置号是一个32位整数，
 *              每8bit表示一层，最多4层。最低8位表示当前层
 *              例如，0x203 表示当前板卡位于 母板 的第二个扣板上的第三个槽位
 *
 *                          /|---->position = 0x203
 *                 _________||_________
 *                /  _______||__       /
 *               /  / // // |/ /|     /
 *              /  /__________/ /--------> position = 2
 *             /   |__________|/    /
 *            /   ____________     /
 *           /   /           /|   /
 *          /   /__________ //--------> position = 1
 *         /    |__________|/   /
 *        /                    /---> position = 0
 *       /____________________/
 */
extern guint32 dfl_get_position(OBJ_HANDLE obj_handle);

/*
 * Description: 通知管理进程板卡的在位状态
 *              有些单板无法通过规范的手段判断在位状态，此时依赖于app检测并通知，例如(PCIe卡)
 */
extern gint32 dfl_notify_presence(OBJ_HANDLE obj_handle, guint32 identifier, guint8 state);


/*
 * Description: (增加一个属性,auxid)通知管理进程板卡的在位状态
 *              有些单板无法通过规范的手段判断在位状态，此时依赖于app检测并通知，例如(PCIe卡)
 */
extern gint32 dfl_notify_presence_v2(OBJ_HANDLE obj_handle, guint32 identifier1, guint32 identifier2, guint8 state);


/*
 * Description: APP发送请求消息到IPMI模块
 */
extern gint32 dfl_send_ipmi_request(gsize req_size, gconstpointer request, gsize resp_size, gpointer response,
    gboolean wait_response);

/*
 * Description: 模块收到ipmi命令后再转发给其他控制器处理
 */
extern gint32 dfl_proxy_ipmi(gsize req_size, gconstpointer request);

/*
 * Description: APP发送响应消息到IPMI模块
 */
extern gint32 dfl_ipmi_response(gsize resp_size, gconstpointer response);

/*
 * Description: 监听类属性变化事件
 */
typedef gint32 (*PropertyEventFunc)(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
/* 注册类属性变更事件的回调处理函数 */
extern gint32 dfl_class_event_handler(const gchar *class_name, const gchar *property_name, PropertyEventFunc handler,
    gpointer user_data);

/*
 * Description: 取消类属性事件的监听
 */
extern gint32 dfl_class_event_cancel(const gchar *class_name, const gchar *property_name);

extern void dfl_update_proxy_flag(void);

/*
 * Description: 用于非常驻进程(如cli)模块的初始化
 */
extern gint32 dfl_init(const gchar *module_name);

/*
 * Description: 获取类方法的入参参数列表
 */
extern gint32 dfl_get_class_method_paralist(const gchar *class_name, const gchar *method_name, GSList **para_list);

/*
 * Description: 调用框架复位BMC
 */
extern gint32 dfl_reboot(void);

/*
 * Description: 查询当前系统reboot控制状态
 */
extern gint32 dfl_get_reboot_state(void);


#define DFT_RESULT_STR_LEN 64
typedef struct tag_dft_result_data {
    guint8 status;
    guint8 result;
    gchar result_str[DFT_RESULT_STR_LEN];
} DFT_RESULT_S;

typedef gint32 (*DftCtrlFunc)(guint32 command, gsize para_size, gconstpointer para_data, DFT_RESULT_S *dft_data,
    gpointer user_data);

/*
 * Description: app模块注册dft测试项
 */
extern gint32 dfl_register_dft_item(guint32 dft_id, guint32 slot, guint32 dev_id, DftCtrlFunc dft_func,
    gpointer user_data);


/*
 * Description: 块设备读 (EEPROM等)
 */
extern gint32 dfl_block_read(OBJ_HANDLE obj_handle, const gchar *property_name, gint32 offset, gsize length,
    gpointer out_data);

/*
 * Description: 块设备写
 */
extern gint32 dfl_block_write(OBJ_HANDLE obj_handle, const gchar *property_name, gint32 offset, gsize length,
    gconstpointer in_data);
/*
 * Description: 通过属性名称获得chip名称
 */
extern gint32 dfl_get_chip(OBJ_HANDLE obj_handle, const gchar *property_name,
    gchar chip_name[MAX_NAME_SIZE], gint32 length);

/*
 * Description: 直接操作器件接口: 初始化器件
 */
extern gint32 dfl_chip_init(const gchar *chip_name);

/*
 * Description: 直接操作器件接口: 从器件读数据，以byte为单位
 *              这个接口可以访问块设备(SRAM、EEPROM等)
 *              如果用来访问cpld等器件，上层软件可能并不清楚具体寄存器
 *              每个bit的含义(不能与寄存器耦合)，因此只能做一些通用处理，
 *              如显示寄存器内容，记录寄存器内容等
 *              不允许bit操作
 *              读出一个字节，与或运算后再写入，无法确保多线程/多进程安全
 *              bit 操作请使用byte读写接口
 */
extern gint32 dfl_chip_blkread(const gchar *chip_name, gint32 offset, gsize length, gpointer out_data);

/*
 * Description: 器件按字节 读，允许bit操作
 */
extern gint32 dfl_chip_byteread(const gchar *chip_name, gint32 offset, guint32 mask, gpointer out_data);

/*
 * Description: 器件按字节写 允许bit操作
 */
extern gint32 dfl_chip_bytewrite(const gchar *chip_name, gint32 offset, guint32 mask, gpointer in_data);

/*
 * Description: 直接操作器件接口:  复位器件
 */
extern gint32 dfl_chip_reset(const gchar *chip_name);

/*
 * Description: 直接操作器件接口:  器件自检
 */
extern gint32 dfl_chip_test(const gchar *chip_name, gint32 *result);

/*
 * Description: 直接操作器件接口: 写入数据，以byte为单位，不允许bit操作
 */
extern gint32 dfl_chip_blkwrite(const gchar *chip_name, gint32 offset, gsize length, gpointer in_data);

/*
 * Description: 读取硬件，获得属性对应的硬件的值，依据options选项确定是否同步到当前属性值
 *              本接口默认属性对应硬件1~4个字节。如果超过4个字节请使用块读写接口访问
 * Notes:  除非你非常确信这个值一定在所有产品上都必须从硬件上获取，你才需要调用此接口
 *         推荐使用dfl_get_extern_value接口替代
 *         value(*) 返回的GVariant*需要释放
 */
extern gint32 dfl_get_hardware_value(OBJ_HANDLE obj_handle, const gchar *property_name, GVariant **value,
    DF_OPTIONS options);

/*
 * Description: 灵活接口。
 *              DF_MEM  查询时从内存读取
 *              DF_HW   查询时从硬件或公式读取
 *              DF_AUTO 查询时自动依据xml来决定从内存还是硬件/公式读取
 * Notes: value(*) 返回的GVariant*外部需要释放
 */
extern gint32 dfl_get_extern_value(OBJ_HANDLE obj_handle, const gchar *property_name, GVariant **value,
    DF_OPTIONS options);


/*
 * Description: 测试自己的rpc服务是否正常
 */
extern gint32 dfl_rpc_selftest(void);

/*
 * Description: 根据功能号获取类名称
 */
extern gint32 dfl_get_class_info_by_id(guint16 class_id, gchar class_name[MAX_NAME_SIZE]);


/*
 * Description: 根据功能号、信息号获取类名称、属性名称及属性类型
 */
extern GVariantType *dfl_get_property_info_by_id(guint16 class_id, guint16 property_id, gchar class_name[MAX_NAME_SIZE],
    gchar property_name[MAX_NAME_SIZE]);


/*
 * Description: 根据功能号、方法号获取类名称、方法名称
 */
extern gint32 dfl_get_method_info_by_id(guint16 class_id, guint16 method_id, gchar class_name[MAX_NAME_SIZE],
    gchar method_name[MAX_NAME_SIZE]);


/*
 * Description: 动态创建对象
 * Notes:  1.单个模块只进行一次性创建，如果创建对象时，已存在动态创建对象
 *         则返回失败,返回ERRCODE_OBJECT_CREATED，表示Connector已有动态对象创建过
 *         2.动态创建不能创建物理对象，默认支持复位持久化,复位后若存在动态创建xml文件，自动加载动态创建xml文件，
 *         若不存在，则由调用者决定是否调用接口创建。
 *         3.动态创建对象属性不支持持久化，使用时建议先调用动态卸载接口，再调用动态添加对象接口
 *         4.Connector中的BOM/ID需手动设置一下，用于动态创建xml文件命名(BOM_ID.xml)，
 *         为避免xml重名，建议将BOM设置成有意义的字符串,ID要确保是全局唯一的，来避免动态创建文件名的重名问题'
 *         使用示例  :
 *             OBJECT_FILE  of = dfl_dynamic_create_object_file();
 *             for ( xx ; xx;  xx)
 *             {
 *                 OBJECT_ITEM oi = dfl_dynamic _create_object_item(of, "object1","class1");
 *                 for( xx; xx; xx)
 *                 { dfl_dynamic _add_property(oi, "a","y","1");}
 *             }
 *             dfl_dynamic_create_object(of);
 * History: 1.2015年10月10日, lingmingqiang
 *          新生成函数
 */
extern gint32 dfl_dynamic_create_object(const gchar *connector_name, OBJECT_FILE object_file);

/*
 * Description: 一次性删除动态创建的对象
 * History: 1.2015年10月13日, lingmingqiang
 *          新生成函数
 */
extern gint32 dfl_dynamic_del_object(const gchar *connector_name);

/*
 * Description: 创建对象文件句柄
 * History: 1.2015年12月8日, lingmingqiang
 *          新生成函数
 */
extern OBJECT_FILE dfl_dynamic_create_object_file(void);

/*
 * Description: 创建对象文件单项条目
 * History: 1.2015年12月8日, lingmingqiang
 *          新生成函数
 */
extern OBJECT_ITEM dfl_dynamic_create_object_item(OBJECT_FILE obj_file, const gchar *object_name,
    const gchar *class_name);

/*
 * Description: 添加对象的属性内容
 * History: 1.2015年12月8日, lingmingqiang
 *          新生成函数
 */
extern gint32 dfl_dynamic_add_property(OBJECT_ITEM object_item, const gchar *property_name, const gchar *property_type,
    const gchar *property_value);

#define DATA_TYPE_SIZE 6
typedef struct tag_property_info {
    gchar name[MAX_NAME_SIZE]; /* property name */
    guint16 id;                /* property id */
    guint8 exported;           /* property export info */
    guint8 key;                /* property key info */
    gchar type[DATA_TYPE_SIZE];
} PROPERTY_INFO_S;

typedef void (*ClassForeachFunc)(const gchar *class_name, guint16 class_id);
/*
 * Description: 获取属性信息
 * History: 1.2016年12月20日, hucheng
 *          新生成函数
 */
extern gint32 dfl_get_property_info(const gchar *class_name, guint32 *property_num,
    PROPERTY_INFO_S **properity_info_list);

/*
 * Description: 遍历类
 * History: 1.2016年12月20日, hucheng
 *          新生成函数
 */
extern gint32 dfl_foreach_class(ClassForeachFunc handler);

typedef struct tag_class_info {
    gchar name[MAX_NAME_SIZE]; /* class name */
    guint16 id;                /* class id */
} CLASS_INFO_S;

/*
 * Description: 获取类列表
 * History: 1.2016年12月20日, hucheng
 *          新生成函数
 */
extern gint32 dfl_get_class_list(gint32 *number, CLASS_INFO_S **list);
/*
 * Description: 获取类列表
 * History: 1.2016年12月20日, hucheng
 *          新生成函数
 */
extern gint32 dfl_get_class_list_v2(gint32 *number, GSList **list);

/*
 * Description: 模块管理接口
 * Notes: options目前只支持0和1两种取值；模块disable主要对进程状态进行修改，
 *        模块异常退出，不进行拉起动作，用于升级场景，杀进程节省内存空间
 *        其他场景慎用，目前建议只在同一个进程进行使用，多进程使用可能导致
 *        乱序等问题。
 * History: 1.2017年1月11日, lingmingqiang
 *          新生成函数
 */
extern gint32 dfl_manage_module(const gchar *module_name, guint32 options);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DFLAPI_H__ */
