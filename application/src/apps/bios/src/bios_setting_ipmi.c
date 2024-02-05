

#include "bios_ipmi.h"
#include "bios.h"
#include "pme_app/pme_app.h"

#pragma pack(1)                             

#define SET_BIOS_CONFIG_ITEM_DATA_LEN   3   

typedef struct {
    guint8  config_item;                    
    guint16 option;                         
} CONFIG_ITEM_DATA_S;

typedef struct {
    guint8  manu_id[MANUFAC_ID_LEN];        
    guint8  sub_cmd;                        
    guint16 selector;                       
    guint16 param_len;                      
    CONFIG_ITEM_DATA_S data;                
} IPMI_SET_BIOS_CONFIG_ITEM_REQ;

typedef struct {
    guint8 comp_code;                       
    guint8 manu_id[MANUFAC_ID_LEN];        
} IPMI_SET_BIOS_CONFIG_ITEM_RESP;

typedef struct cmd_set_mem_mismatch {
    guint8 manu_id[MANUFAC_ID_LEN];
    guint8 subcmd;
    guint8 device_id;
    guint8 event_direction;
    guint8 device_cmd;
    guint8 mem_type;
} CMD_SET_MEM_MISMATCH;

typedef struct rsp_set_mem_mismatch {
    guint8 compcode;
    guint8 manu_id[MANUFAC_ID_LEN];
} RSP_SET_MEM_MISMATCH;

#pragma pack()


#define CONFIG_ITEM_FOR_PXE_TIMEOUT_RETRY   0


#define PXE_TIMEOUT_RETRY_VALUE_DISABLED    "Disabled"              
#define PXE_TIMEOUT_RETRY_VALUE_COUNT1      "TimeoutRetryCount1"    
#define PXE_TIMEOUT_RETRY_VALUE_COUNT2      "TimeoutRetryCount2"    
#define PXE_TIMEOUT_RETRY_VALUE_COUNT3      "TimeoutRetryCount3"    
#define PXE_TIMEOUT_RETRY_VALUE_COUNT4      "TimeoutRetryCount4"    

#define PXE_TIMEOUT_RETRY_VALUE_UNKOWN_STR "Unknown"


LOCAL guint8 check_bios_config_item_para(const IPMI_SET_BIOS_CONFIG_ITEM_REQ *req_data)
{
    
    if (dal_check_ipmi_req_huawei_iana(req_data->manu_id, MANUFAC_ID_LEN) == FALSE) {
        debug_log(DLOG_ERROR, "%s: manu_id is invalid", __FUNCTION__);
        return COMP_CODE_INVALID_FIELD;
    }

    
    if (req_data->param_len != SET_BIOS_CONFIG_ITEM_DATA_LEN) {
        debug_log(DLOG_ERROR, "%s: req_data->param_len (%d) is invalid, correct length: %d", __FUNCTION__,
            req_data->param_len, SET_BIOS_CONFIG_ITEM_DATA_LEN);
        return COMP_CODE_OUTOF_RANGE;
    }

    
    if (req_data->data.config_item != CONFIG_ITEM_FOR_PXE_TIMEOUT_RETRY) {
        debug_log(DLOG_ERROR, "%s: the configuration item (%d) is not supported.", __FUNCTION__,
            CONFIG_ITEM_FOR_PXE_TIMEOUT_RETRY);
        return COMP_CODE_INVALID_FIELD;
    }

    return COMP_CODE_SUCCESS;
}


LOCAL const gchar *get_pxe_timeout_config_option_str(guint16 option)
{
    const gchar *option_list[] = {
        PXE_TIMEOUT_RETRY_VALUE_DISABLED, PXE_TIMEOUT_RETRY_VALUE_COUNT1,
        PXE_TIMEOUT_RETRY_VALUE_COUNT2,   PXE_TIMEOUT_RETRY_VALUE_COUNT3,
        PXE_TIMEOUT_RETRY_VALUE_COUNT4
    };

    if (option >= sizeof(option_list) / sizeof(gchar *)) {
        return PXE_TIMEOUT_RETRY_VALUE_UNKOWN_STR;
    }

    return option_list[option];
}


LOCAL guint8 add_config_item_to_gslist(const gchar *config_item, guint16 option, GSList **gs_list)
{
    const gchar *option_str = NULL;
    struct json_object *json_config = NULL;
    const gchar *json_config_str = NULL;

    
    option_str = get_pxe_timeout_config_option_str(option);
    if (g_strcmp0(option_str, PXE_TIMEOUT_RETRY_VALUE_UNKOWN_STR) == 0) {
        debug_log(DLOG_ERROR, "%s: incorrect configuration option", __FUNCTION__);
        return COMP_CODE_OUTOF_RANGE;
    }

    json_config = json_object_new_object();
    if (json_config == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        return COMP_CODE_UNKNOWN;
    }

    json_object_object_add(json_config, config_item, json_object_new_string(option_str));

    json_config_str = dal_json_object_to_json_string(json_config);
    if (json_config_str == NULL) {
        debug_log(DLOG_ERROR, "%s: dal_json_object_to_json_string failed", __FUNCTION__);
        json_object_put(json_config);
        json_config = NULL;
        return COMP_CODE_UNKNOWN;
    }

    *gs_list = g_slist_append(*gs_list, (gpointer)g_variant_new_string(json_config_str));
    json_object_put(json_config);
    json_config = NULL;

    return COMP_CODE_SUCCESS;
}


LOCAL guint8 bios_setting_err_trans(GSList *output_list)
{
    gint32 ret_type;

    if (output_list == NULL) {
        return COMP_CODE_UNKNOWN;
    }

    ret_type = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    switch (ret_type) {
        case VOS_OK:
            return COMP_CODE_SUCCESS;
        case REGRIST_TYPE_PROP_UNKNOWN:
            return COMP_CODE_INVALID_CMD;
        case VOS_ERR:
        default:
            return COMP_CODE_UNKNOWN;
    }
}


LOCAL guint8 set_bios_config_item(const void *req_msg, const gchar *config_item, guint16 option)
{
    gint32 ret;
    guint8 resp_code;
    OBJ_HANDLE obj_handle = 0;
    GSList *caller_info = NULL;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    ret = dfl_get_object_handle(OBJ_NAME_BIOS, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get obj_handle for OBJ_NAME_BIOS failed", __FUNCTION__);
        return COMP_CODE_UNKNOWN;
    }

    caller_info = ipmimsg_to_callerinfo(req_msg);
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_string(BIOS_JSON_FILE_SETTING_NAME));

    
    resp_code = add_config_item_to_gslist(config_item, option, &input_list);
    if (resp_code != COMP_CODE_SUCCESS) {
        goto EXIT;
    }

    
    ret = dfl_call_class_method(obj_handle, METHOD_BIOS_SET_JSON_FILE, caller_info, input_list, &output_list);
    if (ret == DFL_OK) {
        resp_code = bios_setting_err_trans(output_list); 
    } else {
        resp_code = COMP_CODE_UNKNOWN;
    }

EXIT:
    g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    return resp_code;
}


gint32 ipmi_set_bios_config_item(const void *req_msg, gpointer user_data)
{
    guint8 resp_code;
    const IPMI_SET_BIOS_CONFIG_ITEM_REQ *req_data = NULL;
    IPMI_SET_BIOS_CONFIG_ITEM_RESP resp_data = { 0 };

    if (req_msg == NULL) {
        debug_log(DLOG_ERROR, "%s: input NULL pointer.", __FUNCTION__);
        return RET_ERR;
    }

    req_data = (const IPMI_SET_BIOS_CONFIG_ITEM_REQ *)get_ipmi_src_data(req_msg);
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "%s: get_ipmi_src_data failed", __FUNCTION__);
        ipmi_operation_log(req_msg, "Set BIOS Setup configuration failed");
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    resp_code = check_bios_config_item_para(req_data);
    if (resp_code != COMP_CODE_SUCCESS) {
        ipmi_operation_log(req_msg, "Set BIOS Setup configuration failed");
        return ipmi_send_simple_response(req_msg, resp_code);
    }

    resp_code = set_bios_config_item(req_msg, BIOS_CONFIG_PXE_TIMEOUT_RETRY, req_data->data.option);
    if (resp_code == COMP_CODE_SUCCESS) {
        ipmi_operation_log(req_msg, "Set BIOS Setup configuration (%s) to %s successfully",
            BIOS_CONFIG_PXE_TIMEOUT_RETRY, get_pxe_timeout_config_option_str(req_data->data.option));
        resp_data.comp_code = COMP_CODE_SUCCESS;
        (void)dal_set_ipmi_resp_huawei_iana(resp_data.manu_id, MANUFAC_ID_LEN);
        return ipmi_send_response(req_msg, sizeof(resp_data), (guint8 *)&resp_data);
    } else {
        ipmi_operation_log(req_msg, "Set BIOS Setup configuration (%s) failed", BIOS_CONFIG_PXE_TIMEOUT_RETRY);
        return ipmi_send_simple_response(req_msg, resp_code);
    }
}
#define ONE_FOR_GUINT32 0x00000001 // guint32类型的数字1
#define BIOS_MISMATCH_MEM_TYPE_AEP 0x00

LOCAL guint8 set_memory_mismatch(guint8 event_asserted)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint32 mem_ver_mismatch = 0;

    ret = dfl_get_binded_object(BIOS_CLASS_NAME, BIOS_OBJECT_HANDLE_NAME, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Get Bios object handle failed, ret = %d", __FUNCTION__, ret);
        return COMP_CODE_UNKNOWN;
    }

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_BIOS_MEM_VER_MISMATCH, &mem_ver_mismatch);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_uint32 failed. ret = %d", __FUNCTION__, ret);
        return COMP_CODE_UNKNOWN;
    }

    
    if (event_asserted == 1) {
        mem_ver_mismatch |= ONE_FOR_GUINT32;
    } else {
        mem_ver_mismatch &= ~(ONE_FOR_GUINT32);
    }

    ret =
        dal_set_property_value_uint32(obj_handle, PROPERTY_BIOS_MEM_VER_MISMATCH, mem_ver_mismatch, DF_SAVE_TEMPORARY);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dal_set_property_value_uint32 failed, ret = %d", __FUNCTION__, ret);
        return COMP_CODE_UNKNOWN;
    }

    return COMP_CODE_SUCCESS;
}


gint32 bios_set_memory_mismatch(const void *req_msg, gpointer user_data)
{
    const CMD_SET_MEM_MISMATCH *src_data = NULL;
    RSP_SET_MEM_MISMATCH resp_data;
    guint8 ipmi_cc;

    if (req_msg == NULL) {
        debug_log(DLOG_ERROR, "%s: Param pointer is null.", __FUNCTION__);
        return RET_ERR;
    }

    
    src_data = (const CMD_SET_MEM_MISMATCH *)get_ipmi_src_data(req_msg);
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, "%s: get_ipmi_src_data failed", __FUNCTION__);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    
    if (!dal_check_ipmi_req_huawei_iana(src_data->manu_id, sizeof(src_data->manu_id))) {
        debug_log(DLOG_ERROR, "%s: check request manufacture ID failed.", __FUNCTION__);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    
    if (src_data->mem_type != BIOS_MISMATCH_MEM_TYPE_AEP) {
        debug_log(DLOG_ERROR, "%s: The memory type(%u) is invalid", __FUNCTION__, src_data->mem_type);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    ipmi_cc = set_memory_mismatch(!src_data->event_direction); 
    if (ipmi_cc != COMP_CODE_SUCCESS) {
        return ipmi_send_simple_response(req_msg, ipmi_cc);
    }

    resp_data.compcode = COMP_CODE_SUCCESS;
    (void)dal_set_ipmi_resp_huawei_iana(resp_data.manu_id, sizeof(resp_data.manu_id));
    return ipmi_send_response(req_msg, sizeof(resp_data), (guint8 *)&resp_data);
}


// 最低版本号 0.31
#define UMS_BIOS_NOSUPPORT_VER_MAIN 0
#define UMS_BIOS_NOSUPPORT_VER_SUB 37


LOCAL gint32 bios_support_sp_judge(const gchar *bios_ver)
{
    GSList *input_list = NULL;
    gint32 ret = RET_OK;
    guint32 product_num = 0;
    const gchar *s_bios_ver_num = NULL;
    glong main_bios_ver = 0; // bios主版本号
    glong sub_bios_ver = 0;  // bios子版本号
    guint8 bios_support_sp_flag = 0;
    guint8 old_bios_support_sp_flag = 0;
    OBJ_HANDLE ums_obj_handle = 0;

    (void)dal_get_product_version_num(&product_num);

    if (product_num != PRODUCT_VERSION_V5) {
        return RET_OK;
    }

    if (bios_ver == NULL) {
        return RET_ERR;
    }

    main_bios_ver = strtol(bios_ver, NULL, 10); 
    if (main_bios_ver > UMS_BIOS_NOSUPPORT_VER_MAIN) {
        bios_support_sp_flag = 1;
        goto BIOS_SUPPORT_JUDGE;
    }

    s_bios_ver_num = strchr(bios_ver, '.');
    if (!s_bios_ver_num) {
        sub_bios_ver = 0;
    } else {
        sub_bios_ver = strtol(s_bios_ver_num + 1, NULL, 10); 
    }

    if ((main_bios_ver == UMS_BIOS_NOSUPPORT_VER_MAIN) && (sub_bios_ver > UMS_BIOS_NOSUPPORT_VER_SUB)) {
        bios_support_sp_flag = 1;
    }

BIOS_SUPPORT_JUDGE:
    ret = dal_get_object_handle_nth(SP_UMS_CLASS_NAME, 0, &ums_obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : get ums object handle  failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    (void)dal_get_property_value_byte(ums_obj_handle, UMS_PROPERTY_BIOS_SUPPORT, (guchar *)(&old_bios_support_sp_flag));

    if (old_bios_support_sp_flag != bios_support_sp_flag) {
        // 状态变化
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(bios_support_sp_flag));
        ret = dfl_call_class_method(ums_obj_handle, UMS_METHOD_SET_BIOS_SUPPORT, NULL, input_list, NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s : ums method (%s) return failed, ret=%d", __FUNCTION__,
                UMS_METHOD_SET_BIOS_SUPPORT, ret);
            return RET_ERR;
        }
    }

    return RET_OK;
}


gint32 bios_set_bios_ver(const void *req_msg, gpointer user_data)
{
    errno_t safe_fun_ret = EOK;
    OBJ_HANDLE obj_handle = 0;
    guint32 valid_len = 0;
    guint8 resp_data[BIOS_MSG_HEAD_MIN_LEN];
    gint32 ret_val = 0;
    const guint8 *src_data = NULL;
    GVariant *property_data = NULL;
    gchar bios_version[BIOS_VERSION_MAX_LEN + 1] = {0};

    if (req_msg == NULL) {
        
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    
    valid_len = get_ipmi_src_data_len(req_msg) - BIOS_MSG_HEAD_MIN_LEN;
    if ((valid_len == 0) || (valid_len > BIOS_VERSION_MAX_LEN)) {
        
        debug_log(DLOG_ERROR, "Request msg len:%d is invalid!\n", valid_len);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }

    src_data = get_ipmi_src_data(req_msg);
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, DB_STR_GET_SRC_DATA_FAIL);
        
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
        
    }

    
    ret_val = judge_manu_id_valid_bios(src_data);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, DB_STR_MUNU_ID_ERR, ret_val);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }

#ifdef ARM64_HI1711_ENABLED
    set_bios_reported_flag(RET_OK);
#endif

    
    safe_fun_ret =
        memmove_s(bios_version, sizeof(bios_version), (const gchar *)&(src_data[BIOS_MSG_HEAD_MIN_LEN]), valid_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }

    
    bios_version[valid_len] = '\0';
    
    ret_val = bios_support_sp_judge(bios_version);
    if (ret_val != RET_OK) {
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }

    

    debug_log(DLOG_DEBUG, "InputLen:%d!\r\n", valid_len);

    
    ret_val = dfl_get_binded_object(BIOS_CLASS_NAME, BIOS_OBJECT_HANDLE_NAME, &obj_handle);
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        
        return ipmi_send_simple_response(req_msg, COMP_CODE_CANNOT_RESPONSE);
        
    }

    property_data = g_variant_new_string(bios_version);
    
    if (property_data == NULL) {
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return ipmi_send_simple_response(req_msg, COMP_CODE_CANNOT_RESPONSE);
    }
    
    ret_val = dfl_set_property_value(obj_handle, BMC_BIOS_VER_NAME, property_data, DF_SAVE);
    if (ret_val != DFL_OK) {
        g_variant_unref(property_data);
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        
        return ipmi_send_simple_response(req_msg, COMP_CODE_CANNOT_RESPONSE);
        
    }

    g_variant_unref(property_data);

    
    debug_log(DLOG_ERROR, "Set BIOS version to (%s)", bios_version);
    

    
    resp_data[0] = COMP_CODE_SUCCESS;

    
    ret_val = construt_and_respond_msg(req_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    if (ret_val != RET_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
    }

    return ret_val;
}
