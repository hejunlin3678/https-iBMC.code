
#include "redfish_provider.h"
#include "redfish_provider_system.h"

#define RF_PROCESSORS_URI_LEN 100


LOCAL gint32 get_system_processors_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_system_processors_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_processors_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_processors_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_cpu_rsc_link(OBJ_HANDLE obj_handle, char *rsc_link, size_t link_len, const gchar* slot_id);

LOCAL PROPERTY_PROVIDER_S g_system_processors_provider[] = {
    {
        RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_processors_odata_id,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_processors_count,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_MEMBERS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_processors_members,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_processors_odata_context,
        NULL, NULL, ETAG_FLAG_ENABLE
    }
};

typedef gboolean (*processor_is_present)(OBJ_HANDLE obj_handle);
typedef gint32 (*processor_rsc_link)(OBJ_HANDLE obj_handle,
    char *rsc_link, size_t link_len, const gchar* slot_id);

typedef struct {
    const char processor_type[MAX_CLASS_NAME];  
    processor_is_present is_present;            
    processor_rsc_link rsc_link;                
} PROCESSOR_INFO_S;

LOCAL PROCESSOR_INFO_S g_processor_info[] = {
    {CLASS_CPU, cpu_is_present, get_cpu_rsc_link},
    {CLASS_NPU, npu_is_present, get_npu_rsc_link},
    {CLASS_GPU_CARD, gpu_is_present, get_gpu_rsc_link}
};


LOCAL gint32 get_system_processors_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;

    gint32 ret;
    gchar odata_context[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    iRet =
        snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, SYSTEMS_PROCES_METADATA, slot_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string(odata_context);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_system_processors_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, sizeof(slot));

    
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));
    

    iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, RFPROP_SYSTEM_REPROCESSOR, slot);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string((const char *)uri);

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

gboolean cpu_is_present(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    guchar presence = 0;
    guchar cpu_id = 0;
    guint8 invisible = 0; 

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_CPU_PRESENCE, &presence);
    if (ret != RET_OK || presence == 0) {
        return FALSE;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_CPU_PHYSIC_ID, &cpu_id);
    if (ret != RET_OK || cpu_id == 0) {
        return FALSE;
    }
    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_CPU_INVISIBLE, &invisible);
    if (invisible == 1) {
        return FALSE;
    }
    return TRUE;
}

gboolean npu_is_present(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    guchar presence = 0;

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_NPU_PRESENCE, &presence);
    if (ret != DFL_OK || presence == 0) {
        return FALSE;
    }

    return TRUE;
}

gboolean gpu_is_present(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    guchar power_status = 0;

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_GPU_CARD_POWER_STATE, &power_status);
    if (ret != DFL_OK || power_status == 0) {
        return FALSE;
    }

    return TRUE;
}


gint32 get_cpu_rsc_link(OBJ_HANDLE obj_handle, char *rsc_link, size_t link_len, const gchar* slot_id)
{
    gint32 ret;
    guint8 cpu_id = 0;
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_CPU_PHYSIC_ID, &cpu_id);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get cpu id failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    ret = sprintf_s(rsc_link, link_len, URI_FORMAT_SYSTEM_REPROCESSORS, slot_id, cpu_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s:sprintf_s failed, ret:%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 get_npu_rsc_link(OBJ_HANDLE obj_handle, char *rsc_link, size_t link_len, const gchar* slot_id)
{
    gint32 ret;
    gchar npu_name[MAX_RSC_NAME_LEN] = {0};
    if (dal_is_pcie_npu() == TRUE) {
        ret = construct_npu_name(obj_handle, npu_name, sizeof(npu_name));
    } else {
        ret = dal_get_property_value_string(obj_handle, PROPERTY_NPU_NAME, npu_name, sizeof(npu_name));
    }
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get npu name failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    
    format_xpu_member_id(npu_name, sizeof(npu_name));

    ret = sprintf_s(rsc_link, link_len, URI_FORMAT_SYSTEM_REPROCESSORS_XPU, slot_id, npu_name);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s:snprintf_s failed, ret:%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 get_gpu_rsc_link(OBJ_HANDLE obj_handle, char *rsc_link, size_t link_len, const gchar* slot_id)
{
    gint32 ret;
    guchar power_status = 0;
    guchar gpu_slot = 0;
    gchar gpu_member_id[MAX_RSC_NAME_LEN] = {0};

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_GPU_CARD_POWER_STATE, &power_status);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte failed(%s), ret = %d",
            __FUNCTION__, PROPERTY_GPU_CARD_POWER_STATE, ret);
        return RET_ERR;
    }
    if (power_status == 0) {
        return RET_ERR;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_GPU_CARD_SLOT, &gpu_slot);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte failed(%s), ret = %d",
            __FUNCTION__, PROPERTY_GPU_CARD_SLOT, ret);
        return RET_ERR;
    }

    
    ret = sprintf_s(gpu_member_id, sizeof(gpu_member_id),
        MEMBER_ID_FORMAT_SYSTEM_REPROCESSORS_GPU, gpu_slot);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: sprintf_s failed, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    ret = sprintf_s(rsc_link, link_len, URI_FORMAT_SYSTEM_REPROCESSORS_XPU, slot_id, gpu_member_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: sprintf_s failed, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL guint32 processor_obj_present_calc(OBJ_HANDLE obj_handle, const gchar *class_name)
{
    gsize idx;

    for (idx = 0; idx < ARRAY_SIZE(g_processor_info); idx++) {
        if (g_strcmp0(class_name, g_processor_info[idx].processor_type) == 0) {
            return g_processor_info[idx].is_present(obj_handle) ? 1 : 0;
        }
    }
    return 0;
}


LOCAL void get_present_processors_cnt(const gchar *class_name, guint32 *cnt)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    return_do_info_if_expr(class_name == NULL || cnt == NULL, debug_log(DLOG_ERROR, "%s:NULL pointer", __FUNCTION__));

    *cnt = 0;
    ret = dfl_get_object_list(class_name, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: get object list of class %s failed, ret:%d", __FUNCTION__, class_name, ret);
        return;
    }

    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        *cnt += processor_obj_present_calc((OBJ_HANDLE)(intptr_t)(obj_node->data), class_name);
    }

    g_slist_free(obj_list);
}


LOCAL gint32 get_system_processors_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint32 cpu_cnt = 0;
    guint32 npu_cnt = 0;
    guint32 gpu_cnt = 0;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    get_present_processors_cnt(CLASS_CPU, &cpu_cnt);
    get_present_processors_cnt(CLASS_NPU, &npu_cnt);
    get_present_processors_cnt(CLASS_GPU_CARD, &gpu_cnt);

    *o_result_jso = json_object_new_int(cpu_cnt + npu_cnt + gpu_cnt);
    return HTTP_OK;
}


void format_xpu_member_id(gchar *member_id, gsize str_len)
{
    gsize i;
    
    if (str_len > 0) {
        member_id[0] = g_ascii_toupper(member_id[0]);
    }
    
    for (i = 1; i < str_len; i++) {
        member_id[i] = g_ascii_tolower(member_id[i]);
    }
}

LOCAL gint32 get_processor_rsc_link(const gchar *class_name,
    OBJ_HANDLE obj_handle, char *rsc_link, size_t link_len, const gchar* slot_id)
{
    gsize idx;
    gint32 ret;

    for (idx = 0; idx < ARRAY_SIZE(g_processor_info); idx++) {
        if (g_strcmp0(class_name, g_processor_info[idx].processor_type) != 0) {
            continue;
        }

        if (!g_processor_info[idx].is_present(obj_handle)) {
            return RET_ERR;
        }
    
        ret = g_processor_info[idx].rsc_link(obj_handle, rsc_link, link_len, slot_id);
        if (ret != RET_OK) {
            return RET_ERR;
        }
        break;
    }

    return RET_OK;
}


LOCAL void get_processors_rsc_links(json_object *obj_array, const gchar *class_name, const gchar *slot_id)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    
    gchar rsc_link[MAX_RSC_URI_LEN] = {0};
    json_object *obj_jso = NULL;
    OBJ_HANDLE obj_handle;

    return_do_info_if_expr(obj_array == NULL || class_name == NULL || slot_id == NULL,
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__));

    ret = dfl_get_object_list(class_name, &obj_list);
    return_do_info_if_expr(ret != VOS_OK,
        debug_log(DLOG_DEBUG, "%s: get object list of class %s failed, ret:%d", __FUNCTION__, class_name, ret));

    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        (void)memset_s(rsc_link, sizeof(rsc_link), 0, sizeof(rsc_link));
        obj_handle = (OBJ_HANDLE)obj_node->data;

        ret = get_processor_rsc_link(class_name, obj_handle, rsc_link, sizeof(rsc_link), slot_id);
        if (ret != RET_OK) {
            continue;
        }

        obj_jso = json_object_new_object();
        json_object_object_add(obj_jso, RFPROP_ODATA_ID, json_object_new_string((const gchar *)rsc_link));

        json_object_array_add(obj_array, obj_jso);
        obj_jso = NULL;
    }
    g_slist_free(obj_list);
}


LOCAL gint32 get_system_processors_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gint32 ret;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_array failed", __FUNCTION__));

    // 分别获取CPU和NPU的资源链接
    get_processors_rsc_links(*o_result_jso, CLASS_CPU, (const gchar *)slot);
    get_processors_rsc_links(*o_result_jso, CLASS_NPU, (const gchar *)slot);
    get_processors_rsc_links(*o_result_jso, CLASS_GPU_CARD, (const gchar *)slot);

    return HTTP_OK;
}

gint32 system_processors_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;
    guchar board_type = 0;

    
    ret = redfish_get_x86_enable_type(&board_type);
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);
    return_val_if_fail(DISABLE != board_type, HTTP_NOT_FOUND);
    

    
    gboolean bool_ret;

    bool_ret = redfish_check_system_uri_valid(i_paras->uri);
    if (!bool_ret) {
        return HTTP_NOT_FOUND;
    }

    

    *prop_provider = g_system_processors_provider;
    *count = sizeof(g_system_processors_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}
