#ifndef __DAL_SET_AGENT_OBJECT_INTERFACE_H__
#define __DAL_SET_AGENT_OBJECT_INTERFACE_H__
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "json/json.h"

gint32 dal_update_property_string_try_agent(json_object* resource_jso,
                                            gchar* redfish_prop,
                                            OBJ_HANDLE pme_handle,
                                            gchar* pme_prop);

gint32 dal_update_property_uint16_try_agent(json_object* resource_jso,
                                            gchar* redfish_prop,
                                            OBJ_HANDLE pme_handle,
                                            gchar* pme_prop);
gint32 dal_update_property_byte_try_agent(json_object* resource_jso,
                                          gchar* redfish_prop,
                                          OBJ_HANDLE pme_handle,
                                          gchar* pme_prop);
gint32 dal_set_property_string_try_agent(OBJ_HANDLE obj_handle,
                                         const gchar* property_name,
                                         const gchar* in_prop_val,
                                         DF_OPTIONS options);
gint32 dal_set_property_double_try_agent(OBJ_HANDLE obj_handle,
                                         const gchar* property_name,
                                         gdouble in_prop_val,
                                         DF_OPTIONS options);
gint32 dal_set_property_uint16_try_agent(OBJ_HANDLE obj_handle,
                                         const gchar* property_name,
                                         guint16 in_prop_val,
                                         DF_OPTIONS options);
gint32 dal_set_property_byte_try_agent(OBJ_HANDLE obj_handle,
                                       const gchar* property_name,
                                       guchar in_prop_val,
                                       DF_OPTIONS options);
gint32 dal_set_agent_object_property_method(const OBJ_HANDLE object_handle,
                                            GSList* caller_info,
                                            GSList* input_list,
                                            GSList** output_list);

// 以下这些接口函数从未被使用过
#define __UN_USED_FUNCTION
#ifdef __UN_USED_FUNCTION
gint32 dal_update_property_double_try_agent(json_object* resource_jso,
                                            gchar* redfish_prop,
                                            OBJ_HANDLE pme_handle,
                                            gchar* pme_prop);
gint32 dal_update_property_uint32_try_agent(json_object* resource_jso,
                                            gchar* redfish_prop,
                                            OBJ_HANDLE pme_handle,
                                            gchar* pme_prop);
gint32 dal_set_property_int32_try_agent(OBJ_HANDLE obj_handle, const gchar *property_name, gint32 in_prop_val,
    DF_OPTIONS options);
gint32 dal_set_property_int16_try_agent(OBJ_HANDLE obj_handle, const gchar *property_name, gint16 in_prop_val,
    DF_OPTIONS options);
gint32 dal_set_property_uint32_try_agent(OBJ_HANDLE obj_handle, const gchar *property_name, guint32 in_prop_val,
    DF_OPTIONS options);
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* __DAL_SET_AGENT_OBJECT_INTERFACE_H__ */