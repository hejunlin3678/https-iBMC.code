#ifndef __UIP_OBJECT_INTERFACE_H__
#define __UIP_OBJECT_INTERFACE_H__

#include <stdarg.h>
#include "pme_app/common/cleanup_util.h"
#define NO_SESSION_METHMOD_NUM 11
#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 函 数 名  : uip_get_object_property
 功能描述  : 提供除web之外的接口, 参数value需由调用者通过g_variant_unref()函数释放

 输入参数  : gchar * class_name
             gchar * obj_name
             gchar * property_name
 输出参数  : GVariant ** value
 返 回 值  : gint32
*****************************************************************************/
extern gint32 uip_get_object_property(const gchar* class_name, const gchar* obj_name, const gchar* property_name, GVariant** value);

/*****************************************************************************
 函 数 名  : uip_multiget_object_property
 功能描述  : 获取对象的多个属性. 链表 property_name_list 和 property_value都由调用者释放

 输入参数  : gchar * class_name
             gchar * obj_name
             GSList * property_name_list
 输出参数  : GSList ** property_value
 返 回 值  : gint32
*****************************************************************************/
extern gint32 uip_multiget_object_property(const gchar* class_name, const gchar* obj_name, GSList* property_name_list, GSList** property_value);

/*****************************************************************************
 函 数 名  : uip_set_object_property
 功能描述  : 设置单个对象属性。value 由调用者释放。

 输入参数  : gchar * class_name
             gchar * obj_name
             GSList * property_name_list
 输出参数  : GSList ** property_value
 返 回 值  : gint32
*****************************************************************************/
extern gint32 uip_set_object_property(const gchar* class_name, const gchar* obj_name, const gchar* method_name, GSList* input_list, GSList** output_list);


/*****************************************************************************
 函 数 名  : uip_free_gvariant_list
 功能描述  : 释放GVariant链表，同时释放每个节点的GVariant内存

 输入参数  : gchar * class_name
             gchar * obj_name
             GSList * property_name_list
 输出参数  : GSList ** property_value
 返 回 值  : gint32
*****************************************************************************/
void uip_free_gvariant_list(GSList* list);

DEFINE_POINTER_CLEANUP_FUNCTION(GSList, uip_free_gvariant_list)
#define _cleanup_uip_gvariant_list_ _cleanup_(uip_free_gvariant_listp)

extern gint32 uip_call_class_method_web(const gchar* interfaces, const gchar* username, const gchar* client,
                                        const gchar* session, const gchar* class_name, const gchar* obj_name,
                                        const gchar* method_name,
                                        GSList* input_list,
                                        GSList** output_list);
gint32 uip_call_class_method_with_handle_web(const gchar* interfaces, const gchar* username, const gchar* client,
        const gchar* session, const gchar* class_name, OBJ_HANDLE obj_handle,
        const gchar* method_name,
        GSList* input_list,
        GSList** output_list);
extern gint32 uip_call_class_method(const gchar* interface, const gchar* username, const gchar* client, const gchar* class_name, const gchar* obj_name, const gchar* method_name, GSList* input_list, GSList** output_list);
gint32 uip_call_class_method_user_info(const gchar *interface, const gchar *username, const gchar *client,
    const gboolean is_local_user, const gchar *role_id, const gchar *class_name, const gchar *obj_name,
    const gchar *method_name, GSList *input_list, GSList **output_list);
extern gint32 uip_call_class_method_with_handle(const gchar* interface, const gchar* username, const gchar* client, const gchar* class_name, OBJ_HANDLE obj_handle, const gchar* method_name, GSList* input_list, GSList** output_list);
extern gint32 uip_call_class_method_redfish(guchar       from_webui_flag,
                                                      const gchar* username,
                                                      const gchar* client,
                                                      OBJ_HANDLE obj_handle,
                                                      const gchar* class_name,
                                                      const gchar* method_name,
                                                      guint32 auth_flag,
                                                      guint8 user_role_pri,
                                                      GSList* input_list,
                                                      GSList** output_list);
gint32 uip_redfish_call_class_method_with_userinfo(guchar from_webui_flag, const gchar *username, const gchar *client,
    const gboolean is_local_user, const gchar *role_id, OBJ_HANDLE obj_handle, const gchar *class_name,
    const gchar *method_name, guint32 auth_flag, guint8 user_role_pri, GSList *input_list, GSList **output_list);


#define uip_get_object_property_func_def(gtype, gvariant_get_func, gv_type) \
    gint32 uip_get_object_property_##gtype(const gchar* obj_name, const gchar* property_name, gtype * value)\
    {\
        GVariant * property_value = NULL;\
        gint32 ret = 0; \
        ret = uip_get_object_property(NULL, obj_name, property_name, &property_value);\
        uip_return_val_if_fail(RET_OK == ret, ret);\
        if (gv_type != g_variant_classify(property_value)) \
        { \
            uip_log(DLOG_ERROR, "get_property: %s.%s type is '%c', but except type is '%c'.", obj_name, property_name, (gchar)g_variant_classify(property_value), (gchar)gv_type);\
            g_variant_unref(property_value);\
            return UIP_ERR_GV_TYPE_MISMATCH;\
        } \
        *value = gvariant_get_func(property_value);\
        g_variant_unref(property_value);\
        return RET_OK;\
    }

extern gint32 uip_get_object_property_guchar(const gchar* obj_name, const gchar* property_name, guchar* value);
extern gint32 uip_get_object_property_guint32(const gchar* obj_name, const gchar* property_name, guint32* value);
extern gint32 uip_get_object_property_gdouble(const gchar* obj_name, const gchar* property_name, gdouble* value);
extern gint32 uip_get_object_property_guint16(const gchar* obj_name, const gchar* property_name, guint16* value);
extern gint32 uip_get_object_property_gint16(const gchar* obj_name, const gchar* property_name, gint16* value);
extern gint32 uip_get_object_property_gint32(const gchar* obj_name, const gchar* property_name, gint32* value);
extern gint32 uip_replace_bmc_version_ibmc_string(gchar *custom_special_name, gint16 len);

//注意，内存需要调用者 g_free 释放
extern gint32 uip_get_object_property_string(const gchar* obj_name, const gchar* property_name, gchar** value);

extern gint32 uip_get_object_property_by_handle_guchar(OBJ_HANDLE obj_handle, const gchar* property_name, guchar* value);
extern gint32 uip_get_object_property_by_handle_guint32(OBJ_HANDLE obj_handle, const gchar* property_name, guint32* value);
extern gint32 uip_get_object_property_by_handle_gdouble(OBJ_HANDLE obj_handle, const gchar* property_name, gdouble* value);
extern gint32 uip_get_object_property_by_handle_guint16(OBJ_HANDLE obj_handle, const gchar* property_name, guint16* value);
//注意，内存需要调用者 g_free 释放
extern gint32 uip_get_object_property_by_handle_string(OBJ_HANDLE obj_handle, const gchar* property_name, gchar** value);


#ifdef __cplusplus
}
#endif
#endif

