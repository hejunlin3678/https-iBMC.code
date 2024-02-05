

#include <malloc.h>
#include "bios_settings.h"
#include "boot_options.h"
#include "bios.h"

#define ARR_ELEMENT_NUM 16
#define DPCY_MAX_DEPTH 10
#define BIOS_POLICYCFG_REQ_LEN 20
#define BIOS_POLICYCFG_REFRESHRATE "RefreshRate"
#define BIOS_POLICYCFG_PDEN "PdEn"
#define BIOS_POLICYCFG_PDPRD "PdPrd"
#define BIOS_POLICYCFG_DEMT "Demt"
#define BIOS_POLICYCFG_FUNNEL "Funnel"
#define BIOS_POLICYCFG_PSFUNNEL "PsFunnel"
#define BIOS_POLICYCFG_UNITTIME "UnitTime"
#define BIOS_POLICYCFG_CETH "CETh"
#define BIOS_POLICYCFG_PSEN "PSEn"
#define BIOS_POLICYCFG_MULFLAG "IsSupportMulParam"
#define BIOS_POLICYCFG_REQBODY "ReqBody"
#define BIOS_POLICYCFG_DEFAULT "Default"
#define BIOS_POLICYCFG_REGISTRY "Registries"
#define BIOS_POLICYCFG_ATTRIBUTE "Attributes"
#define BIOS_POLICYCFG_FUNCID "FunctionId"
#define BIOS_POLICYCFG_FUNCDETAIL "FunctionDetails"
#define BIOS_POLICYCFG_FUNCINFO "FunctionInfo"



const gchar* g_bios_config_item_set_by_ipmi[] = {
    BIOS_CONFIG_PXE_TIMEOUT_RETRY
};

typedef struct bios_control_attribute {
    gchar attribute_name[BIOS_ATTRIBUTE_NAME_LEN];
    guint8 attribute_index;
} STRUCT_BIOS_CONTROL_ATTRIBUTE;

typedef struct tag_mapfrom_item {
    const gchar *attribute;
    const gchar *property;
    const gchar *condition;
    json_object *value_jso;
    const gchar *terms;
} MAPFROM_ITEM;

typedef struct policy_config_req {
    guint32 callid;
    guint8 version;
    guint32 reserved;
    guint8 data[4];
    guint32 crc;
} POLICY_CONFIG_REQ;

typedef struct policycfg_current_value {
    POLICY_CONFIG_REQ power_down_state;
    POLICY_CONFIG_REQ funnel_state;
    guint8 power_down_state_flag;
    guint8 funnel_state_flag;
} POLICYCFG_CURRENT_VALUE;

LOCAL gint32 bios_write_setting_file(OBJ_HANDLE obj_handle, GSList *caller_info, const gchar *write_content,
    GSList **output_list);
LOCAL gint32 bios_write_configvalue_file(OBJ_HANDLE obj_handle, GSList *caller_info, const gchar *write_content,
    GSList **output_list);
LOCAL gint32 bios_write_setting_check(OBJ_HANDLE obj_handle, GSList *caller_info, const gchar *write_content,
    GSList **output_list, gboolean check_setting_flag);
LOCAL gint32 bios_write_setting_data(OBJ_HANDLE obj_handle, const gchar *write_content, json_object **out_data);
LOCAL gint32 bios_reg_check_entry_ht(OBJ_HANDLE obj_handle, json_object *object, json_object **out_err_array,
    gboolean check_setting_flag);
LOCAL gint32 ex_reg_jso(OBJ_HANDLE obj_handle, json_object **regist_data, json_object **attrs_jso,
    json_object **dpcys_jso);
LOCAL void dpcys_jso_to_hashtable(json_object *dpcysjso, GHashTable **dpcys_hashtable);
LOCAL gint32 bios_reg_check_ht(json_object *object, json_object *attrs_jso, GHashTable *dpcys_hashtable,
    json_object **out_err_array);
LOCAL gint32 bios_reg_get_curr_val_ht(OBJ_HANDLE obj_handle, const gchar *property_name, json_object *attrs_jso,
    GHashTable *dpcys_hashtable);
LOCAL void bios_check_out_err_arr_construct(json_object **out_err_array, json_object *attrName, json_object *value,
    json_object *type);
LOCAL void bios_reg_cyc_check(json_object *tmp_object, json_object *attrs_jso, GHashTable *dpcys_hashtable,
    json_object **out_err_array);
LOCAL void dpcys_ht_free_array(GArray *jso_array);
LOCAL gint32 bios_check_jso_assure(json_object *object);
LOCAL gint32 bios_reg_dpcy_response_ht(GHashTable *dpcy_hashtable, json_object *attributes,
    const gchar *changed_attribute, const char *changed_propery, json_bool flag, gint32 *depth);
LOCAL gint32 bios_reg_dpcy_change_ht(GHashTable *dependencies, json_object *dependency_jso, json_object *attributes,
    const gchar *changed_attribute, const char *changed_propery, json_bool flag, gint32 *depth);
LOCAL gint32 bios_regist_check_object_password(json_object *item_obj_jso, json_object *value);
LOCAL gint32 judge_attr_change_effect(json_object *attributes, json_object *dependency_jso,
    const gchar *changed_attribute, const char *changed_propery, gint32 *set_flag, guint8 *change_effected);
LOCAL gint32 bios_regist_check_object_array(json_object *item_obj_jso, json_object *value);
LOCAL gint32 bios_regist_check_object_string(json_object *item_obj_jso, json_object *value);
LOCAL gint32 bios_regist_check_object_integer(json_object *item_obj_jso, json_object *value);
LOCAL gint32 bios_regist_check_object_boolean(json_object *item_obj_jso, json_object *value);
LOCAL gint32 check_rules(json_object *obj_jso_a, json_object *obj_jso_b, const gchar *condtion, const gchar *mapterm,
    gint32 *set_flag);
LOCAL gint32 bios_regist_compare_object_int(const gchar *flag, gint32 number_a, gint32 number_b);
LOCAL gint32 __extract_mapfrom_prop(json_object *mapfrom_item_jso, MAPFROM_ITEM *p_mapfrom_item);


gint32 bios_set_json_file(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    const gchar *property_name = NULL;
    gsize file_name_len = 0;
    const gchar *file_content = NULL;
    gsize file_content_len = 0;

    const gchar* property_name_map[][2] =
    {
        {BIOS_FILE_REGISTRY_NAME, BIOS_JSON_FILE_REGISTRY_NAME},
        {BIOS_FILE_CURRENT_VALUE_NAME, BIOS_JSON_FILE_CURRENT_VALUE_NAME},
        {BIOS_FILE_SETTING_NAME, BIOS_JSON_FILE_SETTING_NAME},
        {BIOS_FILE_RESULT_NAME, BIOS_JSON_FILE_RESULT_NAME},
        {BIOS_FILE_CONFIGVALUE_NAME, BIOS_JSON_FILE_CONFIGVALUE_NAME},
        {NULL, NULL}
    };

    
    if (input_list == NULL || output_list == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RET_ERR;
    }

    
    const gchar *file_name = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), &file_name_len);
    if (file_name == NULL || file_name_len == 0) {
        debug_log(DLOG_ERROR, "%s: invalid file name.", __FUNCTION__);
        return ERR_INVALID_FILE_NAME;
    }

    
    for (gint32 i = 0; property_name_map[i][1] != NULL; i++) {
        if (!g_strcmp0(file_name, property_name_map[i][1])) {
            property_name = property_name_map[i][0];
            break;
        }
    }

    if (property_name == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid file name.", __FUNCTION__);
        *output_list = g_slist_append(*output_list, g_variant_new_int32(ERR_INVALID_FILE_NAME));
        return RET_OK;
    }

    if (g_strcmp0(property_name, BIOS_FILE_SETTING_NAME) && g_strcmp0(property_name, BIOS_FILE_CONFIGVALUE_NAME)) {
        debug_log(DLOG_ERROR, "%s: file not support write.", __FUNCTION__);
        *output_list = g_slist_append(*output_list, g_variant_new_int32(ERR_FILE_NOT_SUPPORT_WRITE));
        return RET_OK;
    }

    
    file_content = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 1), &file_content_len);
    if (file_content == NULL || file_content_len == 0) {
        debug_log(DLOG_ERROR, "%s: invalid write content.", __FUNCTION__);
        *output_list = g_slist_append(*output_list, g_variant_new_int32(ERR_INVALID_WRITE_CONTENT));
        return RET_OK;
    }
    if (!g_strcmp0(property_name, BIOS_FILE_CONFIGVALUE_NAME)) {
        return bios_write_configvalue_file(obj_handle, caller_info, file_content, output_list);
    }
    return bios_write_setting_file(obj_handle, caller_info, file_content, output_list);
}



LOCAL void set_current_config_value(gchar *name,
    POLICYCFG_CURRENT_VALUE *cur_value, guint32 setnum)
{
    if (!g_strcmp0(name, BIOS_POLICYCFG_PDEN)) {
        cur_value->power_down_state.data[0] = LONGB0((guint16)setnum);
        cur_value->power_down_state.data[1] = LONGB1((guint16)setnum);
        cur_value->power_down_state_flag = 1;
    } else if (!g_strcmp0(name, BIOS_POLICYCFG_PDPRD)) {
        cur_value->power_down_state.data[2] = LONGB0((guint16)setnum);          // data[2]为PdPrd的低位
        cur_value->power_down_state.data[3] = LONGB1((guint16)setnum);          // data[3]为PdPrd的高位
        cur_value->power_down_state_flag = 1;
    } else if (!g_strcmp0(name, BIOS_POLICYCFG_FUNNEL)) {
        cur_value->funnel_state.data[0] = (guint8)setnum;
        cur_value->funnel_state_flag = 1;
    } else if (!g_strcmp0(name, BIOS_POLICYCFG_PSFUNNEL)) {
        cur_value->funnel_state.data[1] = (guint8)setnum;
        cur_value->funnel_state_flag = 1;
    } else if (!g_strcmp0(name, BIOS_POLICYCFG_UNITTIME)) {
        cur_value->funnel_state.data[2] = LONGB0((guint16)setnum);              // data[2]为UnitTime的低位
        cur_value->funnel_state.data[3] = LONGB1((guint16)setnum);              // data[3]为UnitTime的高位
        cur_value->funnel_state_flag = 1;
    } else {
        return;
    }
}


LOCAL gint32 send_policycfg_ipmi_cmd(IPMI_REQ_MSG_HEAD_S req_header, guint8 *req_data, guint8 len)
{
    gpointer resp_ipmi = NULL;
    gint32 result = ipmi_send_request(&req_header, req_data, len, &resp_ipmi, TRUE);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "%s : ipmi_send_request fail", __FUNCTION__);
        return RET_ERR;
    }
    const guint8 *resp_data_buf = get_ipmi_src_data((gconstpointer)resp_ipmi);
    if (resp_data_buf == NULL) {
        debug_log(DLOG_ERROR, "%s :set policy config by imu fail", __FUNCTION__);
        g_free(resp_ipmi);
        return RET_ERR;
    }
    if (get_ipmi_src_data_len((gconstpointer)resp_ipmi) < 8) {   // 若响应长度不足8，则返回
        g_free(resp_ipmi);
        return RET_ERR;
    }
    for (guint8 i = 4; i <= 7; i++) { // 若配置成功，响应的data[4]开始到data[7]为0
        if (resp_data_buf[i] != 0) {
            g_free(resp_ipmi);
            return RET_ERR;
        }
    }
    g_free(resp_ipmi);
    return RET_OK;
}


LOCAL gint32 construct_ipmi_cmd(POLICY_CONFIG_REQ req)
{
    IPMI_REQ_MSG_HEAD_S req_header;
    guint8 req_data[BIOS_POLICYCFG_REQ_LEN] = {0};
    const guint32 huawei_iana_id = HUAWEI_MFG_ID;

    // 构造ipmi命令
    req_header.target_type = 1;                   // IPMIChannel：ME
    req_header.target_instance = 0;
    req_header.lun = 0;
    req_header.netfn = BIOS_IPMI_CMD_NETNUM;      // 0x30
    req_header.cmd = BIOS_IPMI_CMD_BIOS_CMDNUM;   // 0x98
    req_header.src_len = BIOS_POLICYCFG_REQ_LEN;

    req_data[0] = LONGB0(huawei_iana_id);         // 0:0xdb
    req_data[1] = LONGB1(huawei_iana_id);         // 1:0x07
    req_data[2] = LONGB2(huawei_iana_id);         // 2:0x00
    req_data[3] = 0x23;                           // 3:子命令字0x23
    req_data[4] = LONGB0(req.callid);             // 4:callid
    req_data[5] = LONGB1(req.callid);             // 5:callid
    req_data[6] = LONGB2(req.callid);             // 6:callid
    req_data[7] = LONGB3(req.callid);             // 7:callid
    req_data[8] = req.version;                    // 8:version
    req_data[9] = LONGB0(req.reserved);           // 9:reserved
    req_data[10] = LONGB1(req.reserved);          // 10:reserved
    req_data[11] = LONGB2(req.reserved);          // 11:reserved
    req_data[12] = req.data[0];                   // 12:配置项状态data[0]
    req_data[13] = req.data[1];                   // 13:配置项状态data[1]
    req_data[14] = req.data[2];                   // 14:配置项状态data[2]
    req_data[15] = req.data[3];                   // 15:配置项状态data[3]

    guint8 req_crc32_data[8] = {0};
    for (guint i = 0; i < sizeof(req_crc32_data); i++) {
        req_crc32_data[i] = req_data[i + 8];      // 从req_data[8]开始计算校验和
    }
    guint32 crc32 = arith_make_crc32(0, (const char*)req_crc32_data, sizeof(req_crc32_data));

    req_data[16] = LONGB0(crc32);                 // 16:CRC32校验和
    req_data[17] = LONGB1(crc32);                 // 17:CRC32校验和
    req_data[18] = LONGB2(crc32);                 // 18:CRC32校验和
    req_data[19] = LONGB3(crc32);                 // 19:CRC32校验和

    return send_policycfg_ipmi_cmd(req_header, req_data, sizeof(req_data));
}


LOCAL void parse_json_to_ipmi(json_object *transfunc, gchar *name, json_object *val,
    POLICYCFG_CURRENT_VALUE *cur_value, json_object **cur_file_obj)
{
    // 从请求体中找到Default值
    json_object *ismulparam = NULL;
    json_object *reqbody = NULL;
    json_object *callid = NULL;
    json_object *version = NULL;
    json_object *reserved = NULL;

    json_object_object_get_ex(transfunc, BIOS_POLICYCFG_MULFLAG, &ismulparam);
    json_object_object_get_ex(transfunc, BIOS_POLICYCFG_REQBODY, &reqbody);

    json_object *callid_obj = json_object_array_get_idx(reqbody, 0);
    json_object *version_obj = json_object_array_get_idx(reqbody, 1);
    json_object *reserved_obj = json_object_array_get_idx(reqbody, 2);

    json_object_object_get_ex(callid_obj, BIOS_POLICYCFG_DEFAULT, &callid);
    json_object_object_get_ex(version_obj, BIOS_POLICYCFG_DEFAULT, &version);
    json_object_object_get_ex(reserved_obj, BIOS_POLICYCFG_DEFAULT, &reserved);

    guint8 mulflag = (guint8)json_object_get_uint64(ismulparam);
    guint32 setnum = (guint32)json_object_get_uint64(val);

    // 如果不支持多种属性发送，则直接获取data，传进ipmi命令中
    if (!mulflag) {
        POLICY_CONFIG_REQ req = {0};
        req.callid = (guint32)json_object_get_uint64(callid);
        req.version = (guint8)json_object_get_uint64(version);
        req.reserved = (guint32)json_object_get_uint64(reserved);
        req.data[0] = LONGB0(setnum);
        req.data[1] = LONGB1(setnum);
        req.data[2] = LONGB2(setnum);       // data[2]为属性的高位
        req.data[3] = LONGB3(setnum);       // data[3]为属性的最高位
        if (!construct_ipmi_cmd(req)) {
            json_object *cfg_val = NULL;
            json_object_object_get_ex(*cur_file_obj, name, &cfg_val);
            json_object_set_int(cfg_val, setnum);
        }
    } else {
        // 修改当前值文件为下发配置值，设置改变标志
        set_current_config_value(name, cur_value, setnum);

        if (cur_value->power_down_state_flag && (!cur_value->power_down_state.callid)) {
            cur_value->power_down_state.callid = (guint32)json_object_get_uint64(callid);
            cur_value->power_down_state.version = (guint8)json_object_get_uint64(version);
            cur_value->power_down_state.reserved = (guint32)json_object_get_uint64(reserved);
        }
        if (cur_value->funnel_state_flag && (!cur_value->funnel_state.callid)) {
            cur_value->funnel_state.callid = (guint32)json_object_get_uint64(callid);
            cur_value->funnel_state.version = (guint8)json_object_get_uint64(version);
            cur_value->funnel_state.reserved = (guint32)json_object_get_uint64(reserved);
        }
    }
}


LOCAL void policy_config_according_items(json_object **cur_file_obj, gchar *name,
    json_object *val, POLICYCFG_CURRENT_VALUE *cur_value)
{
    // 将配置文件转为json对象
    OBJ_HANDLE bios_handle = 0;
    gchar registry_filename[REGRIST_FILE_NAME_LENGTH] = {0};

    (void)dal_get_object_handle_nth(CLASS_NAME_BIOS, 0, &bios_handle);
    (void)dal_get_property_value_string(bios_handle, BIOS_FILE_POLICYCONFIGREGISTRY_NAME,
        registry_filename, REGRIST_FILE_NAME_LENGTH);

    json_object *file_obj = json_object_from_file(registry_filename);
    if (file_obj == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_from_file fail", __FUNCTION__);
        return;
    }

    // 根据配置项名找到对应的FunctionID
    json_object *registries = NULL;
    json_object *attributes = NULL;
    json_object *settings = NULL;
    json_object *functionid = NULL;

    json_object_object_get_ex(file_obj, BIOS_POLICYCFG_REGISTRY, &registries);
    json_object_object_get_ex(registries, BIOS_POLICYCFG_ATTRIBUTE, &attributes);
    json_object_object_get_ex(attributes, name, &settings);
    json_object_object_get_ex(settings, BIOS_POLICYCFG_FUNCID, &functionid);
    const gchar *funcid = json_object_get_string(functionid);

    // 根据FunctionID找到转换函数
    json_object *functiondetails = NULL;
    json_object *functionInfo = NULL;
    json_object *transfunc = NULL;

    json_object_object_get_ex(file_obj, BIOS_POLICYCFG_FUNCDETAIL, &functiondetails);
    json_object_object_get_ex(functiondetails, BIOS_POLICYCFG_FUNCINFO, &functionInfo);
    json_object_object_get_ex(functionInfo, funcid, &transfunc);
    
    parse_json_to_ipmi(transfunc, name, val, cur_value, cur_file_obj);
    json_object_put(file_obj);
}


LOCAL gint32 get_current_config_value(json_object **file_obj, POLICYCFG_CURRENT_VALUE *cur_value)
{
    OBJ_HANDLE bios_handle = 0;
    gchar config_filename[REGRIST_FILE_NAME_LENGTH] = {0};

    (void)dal_get_object_handle_nth(CLASS_NAME_BIOS, 0, &bios_handle);
    (void)dal_get_property_value_string(bios_handle, BIOS_FILE_CONFIGVALUE_NAME,
        config_filename, REGRIST_FILE_NAME_LENGTH);

    *file_obj = json_object_from_file(config_filename);
    if (*file_obj == NULL) {
        debug_log(DLOG_ERROR, "[%s]json_object_from_file fail", __FUNCTION__);
        return RET_ERR;
    }

    json_object *pden = NULL;
    json_object *pdprd = NULL;
    json_object *funnel = NULL;
    json_object *psfunnel = NULL;
    json_object *unittime = NULL;

    json_object_object_get_ex(*file_obj, BIOS_POLICYCFG_PDEN, &pden);
    json_object_object_get_ex(*file_obj, BIOS_POLICYCFG_PDPRD, &pdprd);
    json_object_object_get_ex(*file_obj, BIOS_POLICYCFG_FUNNEL, &funnel);
    json_object_object_get_ex(*file_obj, BIOS_POLICYCFG_PSFUNNEL, &psfunnel);
    json_object_object_get_ex(*file_obj, BIOS_POLICYCFG_UNITTIME, &unittime);

    cur_value->power_down_state_flag = 0;
    cur_value->funnel_state_flag = 0;
    cur_value->power_down_state.data[0] = LONGB0((guint16)json_object_get_uint64(pden));
    cur_value->power_down_state.data[1] = LONGB1((guint16)json_object_get_uint64(pden));
    cur_value->power_down_state.data[2] = LONGB0((guint16)json_object_get_uint64(pdprd));       // data[2]为PdPrd的低位
    cur_value->power_down_state.data[3] = LONGB1((guint16)json_object_get_uint64(pdprd));       // data[3]为PdPrd的高位
    cur_value->funnel_state.data[0] = (guint8)json_object_get_uint64(funnel);
    cur_value->funnel_state.data[1] = (guint8)json_object_get_uint64(psfunnel);
    cur_value->funnel_state.data[2] = LONGB0((guint16)json_object_get_uint64(unittime));        // data[2]为UnitTime的低位
    cur_value->funnel_state.data[3] = LONGB1((guint16)json_object_get_uint64(unittime));        // data[3]为UnitTime的高位

    return RET_OK;
}


LOCAL void send_muti_policycfg_cmd(POLICYCFG_CURRENT_VALUE cur_value, json_object **cur_file_obj)
{
    gint32 ret = RET_ERR;
    if (cur_value.power_down_state_flag) {
        ret = construct_ipmi_cmd(cur_value.power_down_state);
        if (ret == RET_OK) {
            json_object *pden = NULL;
            json_object *pdprd = NULL;
            json_object_object_get_ex(*cur_file_obj, BIOS_POLICYCFG_PDEN, &pden);
            json_object_object_get_ex(*cur_file_obj, BIOS_POLICYCFG_PDPRD, &pdprd);
            guint16 pden_val = MAKE_WORD(cur_value.power_down_state.data[1],
                cur_value.power_down_state.data[0]);
            guint16 pdprd_val = MAKE_WORD(cur_value.power_down_state.data[3],
                cur_value.power_down_state.data[2]);
            json_object_set_int(pden, pden_val);
            json_object_set_int(pdprd, pdprd_val);
        }
    }
    if (cur_value.funnel_state_flag) {
        ret = construct_ipmi_cmd(cur_value.funnel_state);
        if (ret == RET_OK) {
            json_object *funnel = NULL;
            json_object *psfunnel = NULL;
            json_object *unittime = NULL;
            json_object_object_get_ex(*cur_file_obj, BIOS_POLICYCFG_FUNNEL, &funnel);
            json_object_object_get_ex(*cur_file_obj, BIOS_POLICYCFG_PSFUNNEL, &psfunnel);
            json_object_object_get_ex(*cur_file_obj, BIOS_POLICYCFG_UNITTIME, &unittime);
            guint16 unittime_val = MAKE_WORD(cur_value.funnel_state.data[3],
                cur_value.funnel_state.data[2]);
            json_object_set_int(funnel, cur_value.funnel_state.data[0]);
            json_object_set_int(psfunnel, cur_value.funnel_state.data[1]);
            json_object_set_int(unittime, unittime_val);
        }
    }
}


LOCAL gint32 save_policycfg_file(json_object *cur_file_obj)
{
    gint32 ret = json_object_to_file(BIOS_CONFIGVALUE_JSON_FILE_NAME, cur_file_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "json_object_to_file failed, ret=%d", ret);
        return RET_ERR;
    }
    ret = json_object_to_file(BIOS_CLASS_CONFIGVALUE_JSON_FILE_NAME, cur_file_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "json_object_to_bios_class_file failed, ret=%d", ret);
        return RET_ERR;
    }
    (void)chmod(BIOS_CONFIGVALUE_JSON_FILE_NAME, (S_IRUSR | S_IWUSR));
    (void)chmod(BIOS_CLASS_CONFIGVALUE_JSON_FILE_NAME, (S_IRUSR | S_IWUSR));
    return RET_OK;
}


LOCAL gint32 bios_write_configvalue_file(OBJ_HANDLE obj_handle, GSList *caller_info, const gchar *write_content,
    GSList **output_list)
{
    if (output_list == NULL) {
        debug_log(DLOG_ERROR, "Bios_write_setting_file failed, output_list can not be NULL.");
        return RET_ERR;
    }

    if (write_content == NULL) {
        debug_log(DLOG_ERROR, "%s: write_content is NULL", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Failed to issue BIOS Setup configuration");
        return RET_ERR;
    }

    // 获得策略配置项当前值
    json_object *cur_file_obj = NULL;
    POLICYCFG_CURRENT_VALUE cur_value = {0};
    gint32 ret = get_current_config_value(&cur_file_obj, &cur_value);
    if (ret != RET_OK) {
        method_operation_log(caller_info, NULL, "Failed to issue BIOS Setup configuration");
        return RET_ERR;
    }

    json_object *setting_jso = json_tokener_parse(write_content);

    // 此处用于消除foreach编译告警"assignment discards 'const' qualifier from pointer target type"
#pragma GCC diagnostic push // require GCC 4.6
#pragma GCC diagnostic ignored "-Wcast-qual"
    json_object_object_foreach0(setting_jso, name, val)
    {
        policy_config_according_items(&cur_file_obj, name, val, &cur_value);
    }
#pragma GCC diagnostic pop // require GCC 4.6

    send_muti_policycfg_cmd(cur_value, &cur_file_obj);

    ret = save_policycfg_file(cur_file_obj);
    if (ret != RET_OK) {
        json_object_put(cur_file_obj);
        json_object_put(setting_jso);
        return RET_ERR;
    }

    json_object_put(cur_file_obj);
    json_object_put(setting_jso);

    *output_list = g_slist_append(*output_list, g_variant_new_int32(0));
    method_operation_log(caller_info, NULL, "Issue BIOS Setup Configuration successfully");

    return RET_OK;
}


LOCAL gint32 bios_write_setting_file(OBJ_HANDLE obj_handle, GSList *caller_info, const gchar *write_content,
    GSList **output_list)
{
    FM_FILE_S *file_handle = NULL;
    json_object *write_obj = NULL;
    const gchar *write_str = NULL;
    guint32 file_actual_size = 0;
    guint8 file_change_flag = g_bios_file_change[0].file_change_flag;
    guchar arm_enable = 0;
    guint32 product_version = 0;

    
    if (output_list == NULL) {
        debug_log(DLOG_ERROR, "Bios_write_setting_file failed, output_list can not be NULL.\r\n");
        return RET_ERR;
    }

    

    
    // 这里返回RET_OK,错误码在错误属性在从output_list中获取
    if (bios_write_setting_check(obj_handle, caller_info, write_content, output_list,
        (file_change_flag == BIOS_SETTING_FILE_CHANGED) ? TRUE : FALSE) != RET_OK) {
        return RET_OK;
    }

    
    if (file_change_flag == BIOS_SETTING_FILE_CHANGED) {
        if (bios_write_setting_data(obj_handle, write_content, &write_obj) != RET_OK) {
            debug_log(DLOG_ERROR, "%s: bios_write_setting_data failed", __FUNCTION__);
            goto err_out;
        }

        write_str = dal_json_object_get_str(write_obj);
    } else {
        
        write_str = write_content;
    }
    if (write_str == NULL) {
        if (write_obj != NULL) {
            json_object_put(write_obj);
        }
        debug_log(DLOG_ERROR, "%s: write_str is NULL", __FUNCTION__);
        goto err_out;
    }
    
    
    file_handle = fm_fopen(BIOS_FILE_SETTING_NAME, "wb");
    if (file_handle == NULL) {
        json_object_put(write_obj);
        debug_log(DLOG_ERROR, "%s: open file %s failed", __FUNCTION__, BIOS_FILE_SETTING_NAME);
        goto err_out;
    }

    file_actual_size = fm_fwrite(write_str, BIOS_FILE_STREAM_SIZE, strlen(write_str), file_handle);
    if (strlen(write_str) != file_actual_size) {
        json_object_put(write_obj);
        debug_log(DLOG_ERROR, "%s: write file(InLen:%zu,OutLen:%d) failed", __FUNCTION__, strlen(write_str),
            file_actual_size);
        goto err_out;
    }
    json_object_put(write_obj);
    fm_fclose(file_handle);
    file_handle = NULL;

    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);
    (void)dal_get_product_version_num(&product_version);
    if ((product_version <= PRODUCT_VERSION_V5) && (arm_enable == ENABLE)) {
        if (vos_get_file_accessible(BIOS_CHANGE_INI_FILE_NAME) == TRUE) {
            
            if (fm_clear_file(BIOS_FILE_CHANGE_NAME) != RET_OK) {
                debug_log(DLOG_ERROR, "%s: fm_clear_file %s failed", __FUNCTION__, BIOS_FILE_CHANGE_NAME);
                goto err_out;
            }
        }

        g_bios_config_format = 1;
        per_save((guint8 *)&g_bios_config_format);
        debug_log(DLOG_DEBUG, "BIOS config is json format.\r\n");
    }
    

    
    set_bios_setting_file_change_flag(BIOS_SETTING_FILE_CHANGED, 0);

    
    *output_list = g_slist_append(*output_list, g_variant_new_int32(0));

    
    method_operation_log(caller_info, NULL,
        "BIOS Setup configuration issued successfully and will take effect upon the next startup.");
    

    debug_log(DLOG_MASS, "%s: call malloc_trim result :%d", __FUNCTION__, malloc_trim(0));
    return RET_OK;

err_out:
    if (file_handle != NULL) {
        fm_fclose(file_handle);
    }

    
    method_operation_log(caller_info, NULL, "Failed to issue BIOS Setup configuration.");
    

    debug_log(DLOG_MASS, "%s: call malloc_trim result :%d", __FUNCTION__, malloc_trim(0));
    return RET_ERR;
}


LOCAL gboolean is_need_simple_check(GSList *caller_info, json_object *input_json, const gchar **config_item)
{
    GVariant *tmp_gv = NULL;
    const gchar *interface_str = NULL;
    json_bool ret_bool;
    json_object *json_config = NULL;
    gsize config_size;
    gsize idx;

    tmp_gv = (GVariant *)g_slist_nth_data(caller_info, 0);
    if (tmp_gv == NULL) {
        debug_log(DLOG_ERROR, "%s: g_slist_nth_data failed", __FUNCTION__);
        return FALSE;
    }

    interface_str = g_variant_get_string(tmp_gv, NULL);
    if (interface_str == NULL) {
        debug_log(DLOG_ERROR, "%s: g_variant_get_string failed", __FUNCTION__);
        return FALSE;
    }

    
    if (strcmp(interface_str, "IPMI") != 0 || json_object_object_length(input_json) != 1) {
        return FALSE;
    }

    config_size = sizeof(g_bios_config_item_set_by_ipmi) / sizeof(gchar *);
    for (idx = 0; idx < config_size; idx++) {
        ret_bool = json_object_object_get_ex(input_json, g_bios_config_item_set_by_ipmi[idx], &json_config);
        if (ret_bool == TRUE) {
            *config_item = g_bios_config_item_set_by_ipmi[idx];
            return TRUE;
        }
    }

    return FALSE;
}


LOCAL gint32 bios_regist_simple_check_entry(const gchar *config_item)
{
#define BIOS_FILE_REGISTRY_MAX_SIZE (5 * 1024 * 1024)
    gint32 ret;
    glong file_len;
    glong file_read_len;
    FILE *file_handle = NULL;
    gchar *data_buf = NULL;

    debug_log(DLOG_DEBUG, "%s: %s do simple check", __FUNCTION__, config_item);

    file_len = vos_get_file_length(BIOS_REGISTRY_JSON_FILE_NAME);
    if (file_len <= 0 || file_len > BIOS_FILE_REGISTRY_MAX_SIZE) {
        debug_log(DLOG_ERROR, "%s: The file is empty or too large, file length = %ld", __FUNCTION__, file_len);
        goto EXIT;
    }

    file_handle = g_fopen(BIOS_REGISTRY_JSON_FILE_NAME, "r");
    if (file_handle == NULL) {
        debug_log(DLOG_ERROR, "%s: g_fopen failed", __FUNCTION__);
        goto EXIT;
    }

    data_buf = (gchar *)g_malloc0(file_len + 1);
    if (data_buf == NULL) {
        debug_log(DLOG_ERROR, "%s: g_malloc0 failed", __FUNCTION__);
        goto EXIT;
    }

    file_read_len = fread(data_buf, BIOS_FILE_STREAM_SIZE, file_len, file_handle);
    if (file_read_len != file_len) {
        debug_log(DLOG_ERROR, "%s: fread failed", __FUNCTION__);
        goto EXIT;
    }
    data_buf[file_read_len] = '\0';

    if (strstr(data_buf, config_item) != NULL) {
        ret = RET_OK;
    } else {
        debug_log(DLOG_DEBUG, "%s: the BIOS Setup configuration item (%s) does not exist", __FUNCTION__, config_item);
        ret = REGRIST_TYPE_PROP_UNKNOWN;
    }

    g_free(data_buf);
    (void)fclose(file_handle);
    return ret;

EXIT:
    if (file_handle != NULL) {
        (void)fclose(file_handle);
    }

    if (data_buf != NULL) {
        g_free(data_buf);
    }

    return RET_ERR;
}

LOCAL void parse_out_err_array(json_object *output_json, GSList **output_list)
{
    gint32 int32_value = 0;
    json_object *elem_0 = NULL;
    json_object *property_name = NULL;
    json_object *property_value = NULL;
    json_object *err_type = NULL;
    const gchar *str = NULL;

    elem_0 = json_object_array_get_idx(output_json, 0);
    if (elem_0 != NULL) {
        (void)json_object_object_get_ex(elem_0, REGRIST_PROP_ATTRIBUTENAME, &property_name);
        (void)json_object_object_get_ex(elem_0, REGRIST_PROP_VALUE, &property_value);
        (void)json_object_object_get_ex(elem_0, REGRIST_PROP_TYPE, &err_type);

        if (err_type != NULL) {
            int32_value = json_object_get_int(err_type);
            *output_list = g_slist_append(*output_list, g_variant_new_int32(int32_value));
        }

        if (property_name != NULL) {
            str = dal_json_object_get_str(property_name);
            *output_list = g_slist_append(*output_list, g_variant_new_string(str));
        }

        if (property_value != NULL) {
            str = dal_json_object_get_str(property_value);
            *output_list = g_slist_append(*output_list, g_variant_new_string(str));
        }
    }
}


LOCAL gint32 bios_write_setting_check(OBJ_HANDLE obj_handle, GSList *caller_info, const gchar *write_content,
    GSList **output_list, gboolean check_setting_flag)
{
    gint32 ret = RET_OK;
    json_object *output_json = NULL;
    json_object *input_json = NULL;
    const gchar *config_item = NULL;

    input_json = json_tokener_parse(write_content);
    if (input_json == NULL) {
        return RET_ERR;
    }

    if (output_list == NULL) {
        json_object_put(input_json);
        debug_log(DLOG_ERROR, "Bios_write_setting_check failed, output_list can not be NULL.");
        return RET_ERR;
    }

    
    if (is_need_simple_check(caller_info, input_json, &config_item)) {
        json_object_put(input_json);
        ret = bios_regist_simple_check_entry(config_item);
        *output_list = g_slist_append(*output_list, g_variant_new_int32(ret));
        return ret;
    }

    ret = bios_reg_check_entry_ht(obj_handle, input_json, &output_json, check_setting_flag);
    if (ret != RET_OK) {
        if (output_json == NULL) {
            json_object_put(input_json);
            *output_list = g_slist_append(*output_list, g_variant_new_int32(RET_ERR));
            return RET_ERR;
        }

        parse_out_err_array(output_json, output_list);
        json_object_put(input_json);
        json_object_put(output_json);

        return RET_ERR;
    }

    json_object_put(input_json);

    return RET_OK;
}


LOCAL gint32 bios_write_setting_data(OBJ_HANDLE obj_handle, const gchar *write_content, json_object **out_data)
{
    gchar  setting_filename[REGRIST_FILE_NAME_LENGTH] = {0};
    json_object *file_obj = NULL;
    json_object *write_obj_json = NULL;

    if (out_data == NULL || write_content == NULL) {
        return RET_ERR;
    }

    (void)dal_get_property_value_string(obj_handle, BIOS_FILE_SETTING_NAME, setting_filename, REGRIST_FILE_NAME_LENGTH);

    
    if (vos_get_file_accessible(setting_filename) == TRUE &&
        vos_get_file_length(setting_filename) != 0) { // 判断setting_filename是否存在,并且不为空文件
        
        file_obj = json_object_from_file(setting_filename);
    } else {
        file_obj = json_object_new_object();
    }

    write_obj_json = json_tokener_parse(write_content);
    if (write_obj_json == NULL) {
        debug_log(DLOG_ERROR, "%s: json_tokener_parse failed", __FUNCTION__);
        if (file_obj != NULL) {
            json_object_put(file_obj);
            file_obj = NULL;
        }
        return RET_ERR;
    }

    // 此处用于消除foreach编译告警"assignment discards 'const' qualifier from pointer target type"
#pragma GCC diagnostic push // require GCC 4.6
#pragma GCC diagnostic ignored "-Wcast-qual"

    
    json_object_object_foreach0(write_obj_json, name, val)
    {
        json_object_object_del(file_obj, name);

        json_object_object_add(file_obj, name, json_object_get(val));
    }
#pragma GCC diagnostic pop // require GCC 4.6

    json_object_put(write_obj_json);

    *out_data = file_obj;
    return RET_OK;
}


LOCAL gint32 bios_reg_check_entry_ht(OBJ_HANDLE obj_handle, json_object *object, json_object **out_err_array,
    gboolean check_setting_flag)
{
    gint32 ret;
    json_object *regist_data = NULL;
    json_object *attrs_jso = NULL;
    json_object *dpcys_jso = NULL;
    GHashTable *dpcys_hashtable = NULL;
    // 入参判断
    if (object == NULL || out_err_array == NULL) {
        debug_log(DLOG_ERROR, "[%s] parameter error", __FUNCTION__);
        return RET_ERR;
    }
    // 校验系统启动项顺序
    ret = bios_regist_check_boottypeorder(object);
    // 构造out_err_array
    if (ret != RET_OK) {
        *out_err_array = json_object_new_array();
        bios_check_out_err_arr_construct(out_err_array, json_object_new_string(REGRIST_PROP_BOOTTYPEORDER),
            json_object_new_string("N/A"), json_object_new_int(REGRIST_TYPE_PROP_BOOTTYPEORDER_ERROR));
        return REGRIST_TYPE_PROP_BOOTTYPEORDER_ERROR;
    }
    // 提取regist对象的attrs对象和dpcys对象
    ret = ex_reg_jso(obj_handle, &regist_data, &attrs_jso, &dpcys_jso);
    if (ret == RET_ERR) {
        goto ERR_EXIT;
    }

    dpcys_jso_to_hashtable(dpcys_jso, &dpcys_hashtable);
    // 刷新registry文件attrs的属性值
    ret = bios_reg_get_curr_val_ht(obj_handle, BIOS_FILE_CURRENT_VALUE_NAME, attrs_jso, dpcys_hashtable);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] bios_reg_get_curr_val_ht for attrs error", __FUNCTION__);
        ret = RET_ERR;
        goto ERR_EXIT;
    }
    // 刷新setting文件value值
    if (check_setting_flag == TRUE) {
        ret = bios_reg_get_curr_val_ht(obj_handle, BIOS_FILE_SETTING_NAME, attrs_jso, dpcys_hashtable);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "[%s] bios_reg_get_curr_val_ht for value error \n", __FUNCTION__);
            ret = RET_ERR;
            goto ERR_EXIT;
        }
    }
    // 检查setting文件设置项的值是否符合规则
    ret = bios_reg_check_ht(object, attrs_jso, dpcys_hashtable, out_err_array);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] setting check failed", __FUNCTION__);
        goto ERR_EXIT;
    }

ERR_EXIT:
    json_object_put(regist_data);
    g_hash_table_destroy(dpcys_hashtable);
    return ret;
}


gint32 bios_regist_check_boottypeorder(json_object *object)
{
    json_object* order[REGRIST_PROP_BOOTTYPEORDERNUM] = {NULL, NULL, NULL, NULL};

    gint32 count = 0;
    gint32 index_id = 0;
    gint32 index_base = 0;

    if (object == NULL) {
        return RET_ERR;
    }

    if (json_object_object_get_ex(object, REGRIST_PROP_BOOTTYPEORDER0, &order[0]) == TRUE) {
        count++;
    }

    if (json_object_object_get_ex(object, REGRIST_PROP_BOOTTYPEORDER1, &order[1]) == TRUE) {
        count++;
    }

    if (json_object_object_get_ex(object, REGRIST_PROP_BOOTTYPEORDER2, &order[2]) == TRUE) {
        count++;
    }

    if (json_object_object_get_ex(object, REGRIST_PROP_BOOTTYPEORDER3, &order[3]) == TRUE) {
        count++;
    }

    if (count == 0) {
        
        return RET_OK;
    } else if (count < REGRIST_PROP_BOOTTYPEORDERNUM) {
        
        return RET_ERR;
    } else if (count == REGRIST_PROP_BOOTTYPEORDERNUM) {
        for (index_base = 0; index_base < REGRIST_PROP_BOOTTYPEORDERNUM; index_base++) {
            for (index_id = index_base + 1; index_id < REGRIST_PROP_BOOTTYPEORDERNUM; index_id++) {
                if (g_strcmp0(dal_json_object_get_str(order[index_base]), dal_json_object_get_str(order[index_id])) ==
                    0) {
                    return RET_ERR;
                }
            }
        }
    }

    return RET_OK;
}


LOCAL gint32 ex_reg_jso(OBJ_HANDLE obj_handle, json_object **regist_data, json_object **attrs_jso,
    json_object **dpcys_jso)
{
    gint32 ret;
    gchar regist_filename[REGRIST_FILE_NAME_LENGTH] = {0};
    json_object *regist_data_entries = NULL;

    dal_get_property_value_string(obj_handle, BIOS_FILE_REGISTRY_NAME, regist_filename, REGRIST_FILE_NAME_LENGTH);
    *regist_data = json_object_from_file(regist_filename);
    ret = json_object_object_get_ex(*regist_data, REGRIST_PROP_REGISTRYENTRIES, &regist_data_entries);
    if (ret != TRUE) {
        debug_log(DLOG_ERROR, "[%s] json_object_object_get_ex error", __FUNCTION__);
        return RET_ERR;
    }
    json_object_object_get_ex(regist_data_entries, REGRIST_PROP_ATTRIBUTES, attrs_jso);
    json_object_object_get_ex(regist_data_entries, REGRIST_PROP_DEPENDENCIES, dpcys_jso);
    return RET_OK;
}


LOCAL void dpcys_jso_to_hashtable(json_object *dpcysjso, GHashTable **dpcys_hashtable)
{
    gint32 dpcys_length;
    gint32 idx;
    const gchar *dpcy_str = NULL;
    GArray *map_value = NULL;
    json_object *dpcyitem_jso = NULL;
    json_object *dpcyfor_jso = NULL;
    json_object *dpcy_jso = NULL;

    *dpcys_hashtable = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, (GDestroyNotify)dpcys_ht_free_array);
    if (*dpcys_hashtable == NULL) {
        debug_log(DLOG_ERROR, "%s: generate dpcys_hashtable fail", __FUNCTION__);
        return;
    }
    
    dpcys_length = json_object_array_length(dpcysjso);
    for (idx = 0; idx < dpcys_length; ++idx) {
        dpcyitem_jso = NULL;
        dpcyitem_jso = json_object_array_get_idx(dpcysjso, idx);

        json_object_object_get_ex(dpcyitem_jso, REGRIST_PROP_DEPENDENCYFOR, &dpcyfor_jso);
        dpcy_str = dal_json_object_get_str(dpcyfor_jso);

        json_object_object_get_ex(dpcyitem_jso, REGRIST_PROP_DEPENDENCY, &dpcy_jso);

        if (g_hash_table_contains(*dpcys_hashtable, dpcy_str)) {
            map_value = (GArray *)g_hash_table_lookup(*dpcys_hashtable, dpcy_str);
            g_array_append_val(map_value, dpcy_jso);
        } else {
            map_value = g_array_sized_new(FALSE, TRUE, sizeof(json_object *), ARR_ELEMENT_NUM);
            g_array_append_val(map_value, dpcy_jso);
#pragma GCC diagnostic push // require GCC 4.6
#pragma GCC diagnostic ignored "-Wcast-qual"
            g_hash_table_insert(*dpcys_hashtable, (gpointer)dpcy_str, map_value);
#pragma GCC diagnostic pop // require GCC 4.6
        }
        map_value = NULL;
    }

    return;
}


LOCAL gint32 bios_reg_check_ht(json_object *object, json_object *attrs_jso, GHashTable *dpcys_hashtable,
    json_object **out_err_array)
{
    json_object *tmp_object = NULL;
    const gchar *object_resource = NULL;
    gint32 ret;

    // 入参判断
    if (object == NULL || attrs_jso == NULL || dpcys_hashtable == NULL || out_err_array == NULL) {
        debug_log(DLOG_ERROR, "[%s] parameter error", __FUNCTION__);
        return RET_ERR;
    }
    ret = bios_check_jso_assure(object);
    if (ret == RET_ERR) {
        return RET_ERR;
    }
    // 创建错误属性数组
    *out_err_array = json_object_new_array();
    if (*out_err_array == NULL) {
        debug_log(DLOG_ERROR, "[%s] json_object_new_array error", __FUNCTION__);
        return RET_ERR;
    }
    // 复制object
    object_resource = dal_json_object_get_str(object);
    tmp_object = json_tokener_parse(object_resource);

    bios_reg_cyc_check(tmp_object, attrs_jso, dpcys_hashtable, out_err_array);
    // 循环检验
    json_object_put(tmp_object);
    // 判断 *out_err_array
    if (json_object_array_length(*out_err_array) != 0) {
        return RET_ERR;
    }
    json_object_put(*out_err_array);
    *out_err_array = NULL;
    return RET_OK;
}


LOCAL gint32 bios_reg_get_curr_val_ht(OBJ_HANDLE obj_handle, const gchar *property_name, json_object *attrs_jso,
    GHashTable *dpcys_hashtable)
{
    gint32 ret = RET_OK;
    gint32 index_id;
    gint32 depth;
    gchar  current_filename[REGRIST_FILE_NAME_LENGTH] = {0};
    json_object *file_obj = NULL;
    json_object *name_value_jso = NULL;
    json_object *item_value_jso = NULL;
    json_object *current_jso = NULL;

    if (attrs_jso == NULL) {
        debug_log(DLOG_ERROR, "[%s]  parameter error", __FUNCTION__);
        return RET_ERR;
    }
    // update Attributes array
    gint32 length = json_object_array_length(attrs_jso);

    (void)dal_get_property_value_string(obj_handle, property_name, current_filename, REGRIST_FILE_NAME_LENGTH);
    if (vos_get_file_accessible(current_filename) != TRUE || vos_get_file_length(current_filename) == 0) {
        ret = (g_strcmp0(property_name, BIOS_FILE_SETTING_NAME)) == 0 ? RET_OK : RET_ERR;
        debug_log(DLOG_DEBUG, "[%s]  %s not exist", __FUNCTION__, property_name);
        return ret;
    }
    file_obj = json_object_from_file(current_filename);
    if (file_obj == NULL) {
        debug_log(DLOG_ERROR, "[%s] json_object_from_file error", __FUNCTION__);
        return RET_ERR;
    }
    for (index_id = 0; index_id < length; index_id++) {
        item_value_jso = json_object_array_get_idx(attrs_jso, index_id);
        if (item_value_jso == NULL) {
            continue;
        }
        (void)json_object_object_get_ex(item_value_jso, REGRIST_PROP_ATTRIBUTENAME, &name_value_jso);
        if (name_value_jso == NULL) {
            continue;
        }
        (void)json_object_object_get_ex(file_obj, dal_json_object_get_str(name_value_jso), &current_jso);
        if (current_jso == NULL) {
            continue;
        }
        json_object_object_add(item_value_jso, REGRIST_PROP_CURRENTVALUE, json_object_get(current_jso));
        debug_log(DLOG_DEBUG, "[%s] name %s  value %s", __FUNCTION__, dal_json_object_get_str(name_value_jso),
            dal_json_object_get_str(current_jso));
        depth = DPCY_MAX_DEPTH;
        ret = bios_reg_dpcy_response_ht(dpcys_hashtable, attrs_jso, dal_json_object_get_str(name_value_jso),
            "CurrentValue", FALSE, &depth);

        item_value_jso = NULL;
        name_value_jso = NULL;
        current_jso = NULL;
    }
    json_object_put(file_obj);
    return ret;
}


LOCAL void bios_check_out_err_arr_construct(json_object **out_err_array, json_object *attrName, json_object *value,
    json_object *type)
{
    gint32 ret;
    json_object *array_item_obj = json_object_new_object();

    json_object_object_add(array_item_obj, REGRIST_PROP_ATTRIBUTENAME, attrName);
    json_object_object_add(array_item_obj, REGRIST_PROP_VALUE, value);
    json_object_object_add(array_item_obj, REGRIST_PROP_TYPE, type);
    // *out_err_array 需保证已申请内存
    ret = json_object_array_add(*out_err_array, array_item_obj);
    if (ret != RET_OK) {
        json_object_put(array_item_obj);
        debug_log(DLOG_ERROR, "[%s] json_object_array_add error", __FUNCTION__);
    }
}


LOCAL void bios_reg_cyc_check(json_object *tmp_object, json_object *attrs_jso, GHashTable *dpcys_hashtable,
    json_object **out_err_array)
{
    json_bool loop_flag = TRUE;
    gint32 type_ret = 0;
    json_bool error_info_flag = FALSE;
    gint32 depth;

    // 循环检验，直到无可以设置属性(有些属性因后面属性影响，第二次循环中可以支持设置)
    while (loop_flag == TRUE) {
        loop_flag = FALSE;
        // 此处用于消除foreach编译告警"assignment discards 'const' qualifier from pointer target type"
#pragma GCC diagnostic push // require GCC 4.6
#pragma GCC diagnostic ignored "-Wcast-qual"
        // 遍历check属性
        json_object_object_foreach0(tmp_object, name, val)
        {
            type_ret = bios_regist_check_object(attrs_jso, name, val, EXPORT_FILE_TRUE);
            debug_log(DLOG_DEBUG, "[%s]name:[%s], type_ret:[%d]", __FUNCTION__, name, type_ret);
            if ((type_ret != RET_OK) && (error_info_flag == TRUE)) {
                json_object_get(val);
                bios_check_out_err_arr_construct(out_err_array, json_object_new_string(name), val,
                    json_object_new_int(type_ret));
                return;
            } else if (type_ret == RET_OK) {
                // 有检测可以设置成功的，则需对设置不成功的重新校验
                loop_flag = TRUE;
                // 根据dependencies更新attributes值
                depth = DPCY_MAX_DEPTH;
                bios_reg_dpcy_response_ht(dpcys_hashtable, attrs_jso, name, REGRIST_PROP_CURRENTVALUE, TRUE, &depth);
                json_object_object_del(tmp_object, name);
            }
        }
        // 如果 FALSE == loop_flag&&error_info_flag == FALSE说明，已经无可以设置属性，再循环一次生成错误消息
        if (loop_flag == FALSE && error_info_flag == FALSE) {
            loop_flag = TRUE;
            // error_info_flag 为TRUE;去尝试生成消息体
            error_info_flag = TRUE;
        }
#pragma GCC diagnostic pop // require GCC 4.6
    }
}


LOCAL void dpcys_ht_free_array(GArray *jso_array)
{
    g_array_free(jso_array, TRUE);
    return;
}


LOCAL gint32 bios_check_jso_assure(json_object *object)
{
    gint32 object_length;

    // 输入对象类型判断
    if (json_type_object != json_object_get_type(object)) {
        debug_log(DLOG_ERROR, "[%s] type error", __FUNCTION__);
        return RET_ERR;
    }
    // 输入对象长度判断
    object_length = json_object_object_length(object);
    if (object_length == 0) {
        debug_log(DLOG_ERROR, "[%s] object_length error", __FUNCTION__);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 bios_reg_dpcy_response_ht(GHashTable *dpcy_hashtable, json_object *attributes,
    const gchar *changed_attribute, const char *changed_propery, json_bool flag, gint32 *depth)
{
    guint32 index_id;
    GArray *map_value = NULL;
    json_object *dpcy_jso = NULL;
    int ret;

    if ((*depth) <= 0) {
        debug_log(DLOG_ERROR, "%s: beyond max depth.", __FUNCTION__);
        return RET_OK;
    }

    if (dpcy_hashtable == NULL || attributes == NULL || changed_attribute == NULL || changed_propery == NULL) {
        debug_log(DLOG_ERROR, "[%s]  parameter error", __FUNCTION__);
        return RET_ERR;
    }

    map_value = (GArray *)g_hash_table_lookup(dpcy_hashtable, changed_attribute);
    if (map_value == NULL) {
        return RET_OK;
    }

    --(*depth);
    for (index_id = 0; index_id < map_value->len; ++index_id) {
        dpcy_jso = g_array_index(map_value, json_object *, index_id);
        ret = bios_reg_dpcy_change_ht(dpcy_hashtable, dpcy_jso, attributes, changed_attribute, changed_propery, flag,
            depth);
        if (ret == RET_ERR) {
            debug_log(DLOG_ERROR, "[%s] bios_reg_dpcy_change_ht error", __FUNCTION__);
        }
    }

    ++(*depth);
    return RET_OK;
}


LOCAL gint32 bios_reg_dpcy_change_ht(GHashTable *dependencies, json_object *dependency_jso, json_object *attributes,
    const gchar *changed_attribute, const char *changed_propery, json_bool flag, gint32 *depth)
{
    gint32 ret;
    gint32 set_flag = 0;

    const gchar *set_map_name = NULL;
    const gchar *set_map_prop = NULL;
    json_object *mapfrom_item_prop_jso = NULL;
    json_object *set_object = NULL;
    json_object *attributes_item_obj = NULL;
    guint8 change_effected = FALSE; // 变更的数据是否影响依赖项

    if (dependency_jso == NULL || attributes == NULL || changed_attribute == NULL || changed_propery == NULL) {
        debug_log(DLOG_ERROR, "[%s]  parameter error", __FUNCTION__);
        return RET_ERR;
    }

    ret = judge_attr_change_effect(attributes, dependency_jso, changed_attribute, changed_propery, &set_flag,
        &change_effected);
    if (ret == RET_ERR) {
        return RET_ERR;
    }
    // 只有当前面条件判断成功，并且修改的属性对依赖项产生影响时，才需要修改设置属性
    if (set_flag == RET_OK && change_effected == TRUE) {
        // 获取Mapfrom 的MaptoAttribute
        ret = json_object_object_get_ex(dependency_jso, REGRIST_PROP_MAPTOATTRIBUTE, &mapfrom_item_prop_jso);
        set_map_name = dal_json_object_get_str(mapfrom_item_prop_jso);
        if (ret == FALSE || set_map_name == NULL) {
            debug_log(DLOG_ERROR, "%s: get MaptoAttribute value failed, ret = %d", __FUNCTION__, ret);
            return RET_ERR;
        }
        mapfrom_item_prop_jso = NULL;
        // 获取Mapfrom 的MaptoProperty
        ret = json_object_object_get_ex(dependency_jso, REGRIST_PROP_MAPTOPROPERTY, &mapfrom_item_prop_jso);
        set_map_prop = dal_json_object_get_str(mapfrom_item_prop_jso);
        if (ret == FALSE || set_map_prop == NULL) {
            debug_log(DLOG_ERROR, "%s: get MaptoProperty value failed, ret = %d", __FUNCTION__, ret);
            return RET_ERR;
        }
        // 获取MaptoValue属性当前值
        ret = bios_regist_get_value(attributes, set_map_name, set_map_prop, NULL, &attributes_item_obj);
        if (ret == RET_ERR) {
            debug_log(DLOG_ERROR, "[%s] bios_regist_get_value error", __FUNCTION__);
            return RET_ERR;
        }
        // 获取Mapfrom 的MapToValue
        ret = json_object_object_get_ex(dependency_jso, REGRIST_PROP_MAPTOVALUE, &set_object);
        if (ret == FALSE) {
            debug_log(DLOG_ERROR, "[%s] MapToValue: json_object_object_get_ex error", __FUNCTION__);
            return RET_ERR;
        }
        // 将对象自己的一个属性value加在另一个属性下面，引用计数要+1
        json_object_object_add(attributes_item_obj, set_map_prop, json_object_get(set_object));

        // 刷新依赖项产生的影响
        if (flag == TRUE) {
            debug_log(DLOG_ERROR, "[%s]", __FUNCTION__);
            bios_reg_dpcy_response_ht(dependencies, attributes, set_map_name, set_map_prop, TRUE, depth);
        }
    } else {
        debug_log(DLOG_MASS, "[%s] %s.%s is changed but has no effect. set_flag=%d, effect_flag=%d ", __FUNCTION__,
            changed_attribute, changed_propery, set_flag, change_effected);
    }

    return RET_OK;
}


LOCAL gint32 compare_bios_input_current_value(gchar *name, json_object *value)
{
    json_object *cur_jso = NULL;
    json_bool bool_cur_jso;
    json_object *cur_jso_item = NULL;
    const gchar *prop_str = NULL;
    const gchar *value_str = NULL;

    value_str = dal_json_object_get_str(value);
    if (value_str == NULL) {
        debug_log(DLOG_ERROR, "%s: dal_json_object_get_str error.", __FUNCTION__);
        return RET_ERR;
    }
    cur_jso = json_object_from_file(BIOS_FILE_CURRENTVALUE_PATH);
    if (cur_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json file parse fail", __FUNCTION__);
        return RET_ERR;
    }

    bool_cur_jso = json_object_object_get_ex(cur_jso, name, &cur_jso_item);
    if (bool_cur_jso != TRUE) {
        (void)json_object_put(cur_jso);
        debug_log(DLOG_ERROR, "%s: json_object_object_get_ex %s failed", __FUNCTION__, name);
        return RET_ERR;
    }

    prop_str = dal_json_object_get_str(cur_jso_item);
    if (NULL == prop_str) {
        (void)json_object_put(cur_jso);
        debug_log(DLOG_ERROR, "%s: dal_json_object_get_str fail", __FUNCTION__);
        return RET_ERR;
    }

    if (g_strcmp0(value_str, prop_str) == 0) {
        (void)json_object_put(cur_jso);
        return RET_OK;
    }
    (void)json_object_put(cur_jso);
    debug_log(DLOG_ERROR, "%s: inputValue=%s, currentValue=%s.", __FUNCTION__, value_str, prop_str);
    return RET_ERR;
}


LOCAL gint32 judge_attribute_value_equal(gchar *name, json_object *value, gboolean export_flag, gboolean *equal_flag)
{
    gint32 ret = 0;
    if (export_flag == TRUE) {
        return REGRIST_TYPE_PROP_IMMUTABLE;
    }
    ret = compare_bios_input_current_value(name, value);
    if (ret != RET_OK) {
        *equal_flag = FALSE;
        debug_log(DLOG_ERROR, "%s: judge_attribute_value_equal IMMUTABLE.", __FUNCTION__);
        return REGRIST_TYPE_PROP_IMMUTABLE;
    }
    *equal_flag = TRUE;
    return RET_OK;
}


LOCAL gint32 check_attributes_status(json_object *item_obj_jso, json_object *prop_value_jso, gchar *name,
    json_object *value, gboolean export_flag)
{
    gboolean equal_flag = FALSE;
    json_bool bool_value;
    gint32 ret = 0;
    int i;
    STRUCT_BIOS_CONTROL_ATTRIBUTE bios_attribute_prop_array[] = {
        {REGRIST_PROP_IMMUTABLE, REGRIST_TYPE_PROP_IMMUTABLE},
        {REGRIST_PROP_HIDDEN, REGRIST_TYPE_PROP_HIDE},
        {REGRIST_PROP_READONLY, REGRIST_TYPE_PROP_READONLY}
    };

    for (i = 0; i < sizeof(bios_attribute_prop_array) / sizeof(bios_attribute_prop_array[0]); i++) {
        ret = json_object_object_get_ex(item_obj_jso, bios_attribute_prop_array[i].attribute_name, &prop_value_jso);
        if (ret == FALSE || json_object_get_type(prop_value_jso) != json_type_boolean) {
            debug_log(DLOG_ERROR, "%s: json_object_object_get_ex error. property=%s ", __FUNCTION__,
                bios_attribute_prop_array[i].attribute_name);
            return RET_ERR;
        }

        bool_value = json_object_get_boolean(prop_value_jso);
        if (bool_value == TRUE) {
            if (export_flag == TRUE) {
                return bios_attribute_prop_array[i].attribute_index;
            }

            ret = judge_attribute_value_equal(name, value, export_flag, &equal_flag);
            if (ret != RET_OK) {
                equal_flag = FALSE;
                return bios_attribute_prop_array[i].attribute_index;
            } else {
                equal_flag = TRUE;
            }
        }
    }
    return RET_OK;
}


gint32 bios_regist_check_object(json_object *attributes, gchar *name, json_object *value, gboolean export_flag)
{
    gint32 ret = 0;

    json_object *item_obj_jso = NULL;
    json_object *prop_value_jso = NULL;
    const gchar *value_str = NULL;
    gint32 length = 0;
    gint32 index_id = 0;

    if (attributes == NULL || name == NULL) {
        debug_log(DLOG_ERROR, "[%s][%d]  parameter error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    if (value == NULL) {
        return REGRIST_TYPE_PROP_TYPE_ERROR;
    }

    
    length = json_object_array_length(attributes);

    for (index_id = 0; index_id < length; index_id++) {
        item_obj_jso = NULL;
        item_obj_jso = json_object_array_get_idx(attributes, index_id);
        if (item_obj_jso == NULL) {
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }

        ret = json_object_object_get_ex(item_obj_jso, REGRIST_PROP_ATTRIBUTENAME, &prop_value_jso);
        if (ret == FALSE) {
            debug_log(DLOG_ERROR, "[%s][%d] name error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }

        value_str = dal_json_object_get_str(prop_value_jso);
        if (value_str == NULL) {
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }

        
        if (g_strcmp0(value_str, name) == 0) {
            prop_value_jso = NULL;
            value_str = NULL;
            break;
        }
    }

    
    if (index_id >= length) {
        debug_log(DLOG_ERROR, "[%s][%d] Can't find attributes %s in current BIOS version", __FUNCTION__, __LINE__,
            name);
        return REGRIST_TYPE_PROP_UNKNOWN;
    }
    

    ret = check_attributes_status(item_obj_jso, prop_value_jso, name, value, export_flag);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] check_attributes_status name=%s.", __FUNCTION__, name);
        return ret;
    }

    
    ret = json_object_object_get_ex(item_obj_jso, REGRIST_PROP_TYPE, &prop_value_jso);
    if (ret == FALSE) {
        debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }
    if (json_type_string != json_object_get_type(prop_value_jso)) {
        debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    value_str = dal_json_object_get_str(prop_value_jso);
    if (value_str == NULL) {
        debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    if (g_strcmp0(value_str, REGRIST_PROP_TYPE_ENUM) == 0) {
        ret = bios_regist_check_object_array(item_obj_jso, value);
    } else if (g_strcmp0(value_str, REGRIST_PROP_TYPE_STRING) == 0) {
        ret = bios_regist_check_object_string(item_obj_jso, value);
    } else if (g_strcmp0(value_str, REGRIST_PROP_TYPE_INTEGER) == 0) {
        ret = bios_regist_check_object_integer(item_obj_jso, value);
    } else if (g_strcmp0(value_str, REGRIST_PROP_TYPE_BOOLEAN) == 0) {
        ret = bios_regist_check_object_boolean(item_obj_jso, value);
    } else if (g_strcmp0(value_str, REGRIST_PROP_TYPE_PASSWORD) == 0) {
        ret = bios_regist_check_object_password(item_obj_jso, value);
    } else {
        ret = RET_ERR;
        debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
    }

    if (ret == RET_OK) {
        
        json_object_object_add(item_obj_jso, REGRIST_PROP_CURRENTVALUE, json_object_get(value));
        debug_log(DLOG_DEBUG, "[%s][%d] setting [%s]:[%s]  success \n", __FUNCTION__, __LINE__, name,
            dal_json_object_get_str(value));
    }

    return ret;
}


LOCAL gint32 bios_regist_check_object_password(json_object *item_obj_jso, json_object *value)
{
    if (item_obj_jso == NULL || value == NULL) {
        debug_log(DLOG_ERROR, "[%s][%d] parameter error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    if (json_type_string != json_object_get_type(value)) {
        return REGRIST_TYPE_PROP_TYPE_ERROR;
    }

    return RET_OK;
}


gint32 bios_regist_get_value(json_object *attributes, const gchar *name, const gchar *poperty,
    json_object **name_value_jso, json_object **item_jso)
{
    gint32 length = 0;
    gint32 ret;
    gint32 index_id = 0;
    json_object *item_obj_jso = NULL;
    json_object *prop_value_jso = NULL;
    const gchar *value_str = NULL;

    if (attributes == NULL || name == NULL || poperty == NULL) {
        debug_log(DLOG_ERROR, "[%s][%d]  parameter error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    length = json_object_array_length(attributes);

    for (index_id = 0; index_id < length; index_id++) {
        item_obj_jso = NULL;
        item_obj_jso = json_object_array_get_idx(attributes, index_id);
        if (item_obj_jso == NULL) {
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }

        ret = json_object_object_get_ex(item_obj_jso, REGRIST_PROP_ATTRIBUTENAME, &prop_value_jso);
        if (ret == FALSE) {
            debug_log(DLOG_ERROR, "[%s][%d] name error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }

        value_str = dal_json_object_get_str(prop_value_jso);
        if (value_str == NULL) {
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }

        
        if (g_strcmp0(value_str, name) == 0) {
            if (item_jso != NULL) {
                (*item_jso = item_obj_jso);
            }

            if (name_value_jso != NULL) {
                ret = json_object_object_get_ex(item_obj_jso, poperty, name_value_jso);
                if (ret == FALSE) {
                    debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
                    return RET_ERR;
                }
            }

            return RET_OK;
        }
    }

    return RET_ERR;
}


LOCAL gint32 bios_regist_check_object_array(json_object *item_obj_jso, json_object *value)
{
    json_object *array_value_jso = NULL;
    json_object *prop_value_jso = NULL;
    json_object *prop_name_jso = NULL;
    const gchar *value_str_input = NULL;
    const gchar *value_str = NULL;

    
    if (item_obj_jso == NULL || value == NULL) {
        debug_log(DLOG_ERROR, "[%s][%d] parameter error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    
    if (json_type_string != json_object_get_type(value)) {
        return REGRIST_TYPE_PROP_TYPE_ERROR;
    }

    
    gint32 ret = json_object_object_get_ex(item_obj_jso, REGRIST_PROP_VALUE, &array_value_jso);
    if (ret == FALSE) {
        debug_log(DLOG_ERROR, "[%s][%d] name error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }
    if (array_value_jso == NULL) {
        debug_log(DLOG_ERROR, "[%s] null pointer", __FUNCTION__);
        return RET_ERR;
    }
    gint32 length = json_object_array_length(array_value_jso);

    for (gint32 index_id = 0; index_id < length; index_id++) {
        prop_value_jso = NULL;
        value_str = NULL;
        prop_value_jso = json_object_array_get_idx(array_value_jso, index_id);
        if (array_value_jso == NULL) {
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }

        
        ret = json_object_object_get_ex(prop_value_jso, REGRIST_PROP_VALUENAME, &prop_name_jso);
        if (ret == FALSE || prop_name_jso == NULL) {
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }

        value_str = dal_json_object_get_str(prop_name_jso);
        if (value_str == NULL) {
            debug_log(DLOG_ERROR, "[%s][%d] dal_json_object_get_str error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }
        value_str_input = dal_json_object_get_str(value);
        if (g_strcmp0(value_str, value_str_input) == 0) {
            return RET_OK;
        }
    }

    debug_log(DLOG_DEBUG, "[%s][%d] not find  \n", __FUNCTION__, __LINE__);

    return REGRIST_TYPE_PROP_NOT_LIST;
}


LOCAL gint32 bios_regist_check_object_string(json_object *item_obj_jso, json_object *value)
{
    gint32 ret = 0;
    gint32 max_length = 0;
    gint32 min_length = 0;
    gint32 input_length = 0;
    json_object *valueexpression_jso = NULL;
    json_object *prop_value_jso = NULL;

    
    if (item_obj_jso == NULL || value == NULL) {
        debug_log(DLOG_ERROR, "[%s][%d] parameter error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    
    if (json_type_string != json_object_get_type(value)) {
        return REGRIST_TYPE_PROP_TYPE_ERROR;
    }

    
    ret = json_object_object_get_ex(item_obj_jso, REGRIST_PROP_MAXLENGTH, &prop_value_jso);
    if (ret == FALSE) {
        debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    if (prop_value_jso != NULL) {
        max_length = json_object_get_int(prop_value_jso);
        prop_value_jso = NULL;
    }

    ret = json_object_object_get_ex(item_obj_jso, REGRIST_PROP_MINLENGTH, &prop_value_jso);
    if (ret == FALSE) {
        debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    if (prop_value_jso != NULL) {
        min_length = json_object_get_int(prop_value_jso);
    }

    input_length = strlen(dal_json_object_get_str(value));
    
    if (max_length != 0 && (input_length < min_length || input_length > max_length)) {
        debug_log(DLOG_ERROR, "[%s][%d]  error \n", __FUNCTION__, __LINE__);
        return REGRIST_TYPE_PROP_LENGTH_ERR;
    }

    
    ret = json_object_object_get_ex(item_obj_jso, REGRIST_PROP_VALUEEXPRESSION, &valueexpression_jso);
    if (ret == FALSE) {
        debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    if (json_type_null == json_object_get_type(valueexpression_jso)) {
        debug_log(DLOG_ERROR, "[%s][%d]  valueexpression_jso is null!\n", __FUNCTION__, __LINE__);
        ret = RET_OK;
    } else if (json_type_string == json_object_get_type(valueexpression_jso)) {
        ret = g_regex_match_simple(dal_json_object_get_str(valueexpression_jso), dal_json_object_get_str(value),
            G_REGEX_OPTIMIZE, (GRegexMatchFlags)0);
        ret = (ret == TRUE ? RET_OK : REGRIST_TYPE_PROP_FORMAT_ERR);
    } else {
        ret = RET_ERR;
        debug_log(DLOG_ERROR, "[%s][%d]  error \n", __FUNCTION__, __LINE__);
    }

    return ret;
}


LOCAL gint32 bios_regist_check_object_integer(json_object *item_obj_jso, json_object *value)
{
    gint32 ret = 0;
    gint32 max_number = 0;
    gint32 min_number = 0;
    gint32 input_number = 0;

    gint32 scalarincrement = 0;
    gint32 currentvalue = 0;
    gdouble double_value;

    json_object *valueexpression_jso = NULL;
    json_object *scalarincrement_jso = NULL;
    json_object *prop_value_jso = NULL;

    
    if (item_obj_jso == NULL || value == NULL) {
        debug_log(DLOG_ERROR, "[%s][%d] parameter error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    
    if (json_type_int != json_object_get_type(value)) {
        return REGRIST_TYPE_PROP_TYPE_ERROR;
    }

    
    ret = json_object_object_get_ex(item_obj_jso, REGRIST_PROP_UPPERBOUND, &prop_value_jso);
    if (ret == FALSE) {
        debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    if (prop_value_jso != NULL) {
        max_number = json_object_get_int(prop_value_jso);
        prop_value_jso = NULL;
    }

    ret = json_object_object_get_ex(item_obj_jso, REGRIST_PROP_LOWERBOUND, &prop_value_jso);
    if (ret == FALSE) {
        debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    if (prop_value_jso != NULL) {
        min_number = json_object_get_int(prop_value_jso);
        prop_value_jso = NULL;
    }

    input_number = json_object_get_int(value);
    
    if (max_number && (input_number < min_number || input_number > max_number) != 0) {
        debug_log(DLOG_ERROR, "[%s][%d]  error \n", __FUNCTION__, __LINE__);
        return REGRIST_TYPE_PROP_NOT_RANGE;
    }

    
    ret = json_object_object_get_ex(item_obj_jso, REGRIST_PROP_VALUEEXPRESSION, &valueexpression_jso);
    if (ret == FALSE) {
        debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    if (json_type_null == json_object_get_type(valueexpression_jso)) {
        debug_log(DLOG_ERROR, "[%s][%d]  valueexpression_jso is null \n", __FUNCTION__, __LINE__);
        ret = RET_OK;
    } else if (json_type_string == json_object_get_type(valueexpression_jso)) {
        ret = g_regex_match_simple(dal_json_object_get_str(valueexpression_jso), dal_json_object_get_str(value),
            G_REGEX_OPTIMIZE, (GRegexMatchFlags)0);
        ret = (ret == TRUE ? RET_OK : REGRIST_TYPE_PROP_FORMAT_ERR);
    } else {
        ret = RET_ERR;
        debug_log(DLOG_ERROR, "[%s][%d]  error \n", __FUNCTION__, __LINE__);
    }

    
    if (ret == RET_OK) {
        ret = json_object_object_get_ex(item_obj_jso, REGRIST_PROP_SCALARINCREMET, &scalarincrement_jso);
        if (ret == FALSE) {
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }

        if (scalarincrement_jso != NULL) {
            (scalarincrement = json_object_get_int(scalarincrement_jso));
        }

        ret = json_object_object_get_ex(item_obj_jso, REGRIST_PROP_CURRENTVALUE, &prop_value_jso);
        if (ret == FALSE) {
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }

        if (prop_value_jso != NULL) {
            (currentvalue = json_object_get_int(prop_value_jso));
        }

        if (scalarincrement == 0) {
            ret = RET_OK;
        } else {
            double_value = (input_number - currentvalue) / (scalarincrement * 1.0);
            if (double_value == (gint32)double_value) {
                ret = RET_OK;
            } else {
                ret = REGRIST_TYPE_PROP_SCALAR_INCREMENT;
            }
        }
    }

    return ret;
}


LOCAL gint32 bios_regist_check_object_boolean(json_object *item_obj_jso, json_object *value)
{
    if (item_obj_jso == NULL || value == NULL) {
        debug_log(DLOG_ERROR, "[%s][%d] parameter error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    if (json_type_boolean != json_object_get_type(value)) {
        return REGRIST_TYPE_PROP_TYPE_ERROR;
    }

    return RET_OK;
}

LOCAL gint32 __extract_mapfrom_prop(json_object *mapfrom_item_jso, MAPFROM_ITEM *p_mapfrom_item)
{
    // 获取Mapfrom 的 MapFromAttribute
    json_object *tmp_jso = NULL;
    (void)json_object_object_get_ex(mapfrom_item_jso, REGRIST_PROP_MAPFORMATTRIBUTE, &tmp_jso);
    p_mapfrom_item->attribute = dal_json_object_get_str(tmp_jso);
    if (p_mapfrom_item->attribute == NULL) {
        debug_log(DLOG_ERROR, "%s: get MapFromAttribute failed.", __FUNCTION__);
        return RET_ERR;
    }

    // 获取Mapfrom 的 MapFromProperty
    tmp_jso = NULL;
    (void)json_object_object_get_ex(mapfrom_item_jso, REGRIST_PROP_MAPFROMPROPERTY, &tmp_jso);
    p_mapfrom_item->property = dal_json_object_get_str(tmp_jso);
    if (p_mapfrom_item->property == NULL) {
        debug_log(DLOG_ERROR, "%s: get MapFromProperty failed.", __FUNCTION__);
        return RET_ERR;
    }

    // 获取Mapfrom 的 MapFromCondition
    tmp_jso = NULL;
    (void)json_object_object_get_ex(mapfrom_item_jso, REGRIST_PROP_MAPFROMCONDITION, &tmp_jso);
    p_mapfrom_item->condition = dal_json_object_get_str(tmp_jso);
    if (p_mapfrom_item->condition == NULL) {
        debug_log(DLOG_ERROR, "%s: get MapFromCondition failed.", __FUNCTION__);
        return RET_ERR;
    }

    // 获取Mapfrom 的 MapFromValue
    (void)json_object_object_get_ex(mapfrom_item_jso, REGRIST_PROP_MAPFROMVALUE, &(p_mapfrom_item->value_jso));

    // 获取Mapfrom 的 MapTerm
    tmp_jso = NULL;
    (void)json_object_object_get_ex(mapfrom_item_jso, REGRIST_PROP_MAPTERMS, &tmp_jso);
    p_mapfrom_item->terms = dal_json_object_get_str(tmp_jso);
    if (p_mapfrom_item->terms == NULL) {
        debug_log(DLOG_ERROR, "%s: get MapTerm failed.", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 judge_attr_change_effect(json_object *attributes, json_object *dependency_jso,
    const gchar *changed_attribute, const char *changed_propery, gint32 *set_flag, guint8 *change_effected)
{
    json_object *mapfrom_item_jso = NULL;
    json_object *compare_prop_jso = NULL;
    json_object *prop_value_jso = NULL;
    MAPFROM_ITEM mapfrom_item;

    mapfrom_item.value_jso = NULL;

    gint32 ret = json_object_object_get_ex(dependency_jso, REGRIST_PROP_MAPFORM, &prop_value_jso);
    if (ret == FALSE) {
        debug_log(DLOG_ERROR, "[%s] MapFrom: json_object_object_get_ex error", __FUNCTION__);
        return RET_ERR;
    }

    gint32 mapfrom_length = json_object_array_length(prop_value_jso);
    for (gint32 index_id = 0; index_id < mapfrom_length; index_id++) {
        mapfrom_item_jso = NULL;
        mapfrom_item_jso = json_object_array_get_idx(prop_value_jso, index_id);
        if (mapfrom_item_jso == NULL) {
            debug_log(DLOG_ERROR, "%s: get mapfrom_item_jso failed", __FUNCTION__);
            return RET_ERR;
        }

        if (__extract_mapfrom_prop(mapfrom_item_jso, &mapfrom_item) != RET_OK) {
            return RET_ERR;
        }

        if (strcmp(mapfrom_item.attribute, changed_attribute) == 0 &&
            strcmp(mapfrom_item.property, changed_propery) == 0) {
            *change_effected = TRUE;
        }

        // 获取 MapFromAttribute 属性当前值
        ret = bios_regist_get_value(attributes, mapfrom_item.attribute, mapfrom_item.property, &compare_prop_jso, NULL);

        ret =
            check_rules(mapfrom_item.value_jso, compare_prop_jso, mapfrom_item.condition, mapfrom_item.terms, set_flag);
        if (ret == TRUE) {
            break;
        }
    }
    return RET_OK;
}


LOCAL gint32 check_rules(json_object *obj_jso_a, json_object *obj_jso_b, const gchar *condtion, const gchar *mapterm,
    gint32 *set_flag)
{
    gint32 ret;
    // 比较值
    ret = bios_regist_compare_object(obj_jso_a, obj_jso_b, condtion);
    if (g_ascii_strcasecmp(mapterm, REGRIST_STRING_AND) == 0) {
        *set_flag = RET_OK;
        if (ret != RET_OK) {
            *set_flag = RET_ERR;
            return TRUE;
        }
    } else if ((g_ascii_strcasecmp(mapterm, REGRIST_STRING_OR) == 0)) {
        *set_flag = RET_ERR;
        if (ret == RET_OK) {
            *set_flag = RET_OK;
            return TRUE;
        }
    }
    return FALSE;
}


gint32 bios_regist_compare_object(json_object *obj_jso_a, json_object *obj_jso_b, const gchar *flag)
{
    gint32 ret = RET_ERR;
    gint32 type_a = 0;
    gint32 type_b = 0;
    json_bool bool_a = 0;
    json_bool bool_b = 0;
    gint32 number_a = 0;
    gint32 number_b = 0;
    const gchar *string_a = NULL;
    const gchar *string_b = NULL;

    if (flag == NULL) {
        debug_log(DLOG_ERROR, "[%s][%d]  parameter error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    type_a = json_object_get_type(obj_jso_a);
    type_b = json_object_get_type(obj_jso_b);
    if ((type_a == json_type_null) && (type_a == type_b) && (g_strcmp0(flag, REGRIST_PROP_COMPARE_EQU) == 0)) {
        ret = RET_OK;
    } else if ((type_a == json_type_boolean) && (type_a == type_b)) {
        bool_a = json_object_get_boolean(obj_jso_a);
        bool_b = json_object_get_boolean(obj_jso_b);
        if (g_strcmp0(flag, REGRIST_PROP_COMPARE_EQU) == 0 && bool_a == bool_b) {
            ret = RET_OK;
        }
        if (g_strcmp0(flag, REGRIST_PROP_COMPARE_NEQ) == 0 && bool_a != bool_b) {
            ret = RET_OK;
        }
    } else if ((type_a == json_type_int) && (type_a == type_b)) {
        number_a = json_object_get_int(obj_jso_a);
        number_b = json_object_get_int(obj_jso_b);
        ret = bios_regist_compare_object_int(flag, number_a, number_b);
    } else if ((type_a == json_type_string) && (type_a == type_b)) {
        string_a = dal_json_object_get_str(obj_jso_a);
        string_b = dal_json_object_get_str(obj_jso_b);
        if (g_strcmp0(flag, REGRIST_PROP_COMPARE_EQU) == 0 && g_strcmp0(string_a, string_b) == 0) {
            ret = RET_OK;
        }
        if (g_strcmp0(flag, REGRIST_PROP_COMPARE_NEQ) == 0 && g_strcmp0(string_a, string_b) != 0) {
            ret = RET_OK;
        }
    } else {
        ret = RET_ERR;
    }

    debug_log(DLOG_DEBUG, "[%s][%d]  [%s]***[%s] ***ret=%d \n", __FUNCTION__, __LINE__,
        dal_json_object_get_str(obj_jso_a), dal_json_object_get_str(obj_jso_b), ret);

    return ret;
}


LOCAL gint32 bios_regist_compare_object_int(const gchar *flag, gint32 number_a, gint32 number_b)
{
    gint32 ret = RET_ERR;
    if ((g_strcmp0(flag, REGRIST_PROP_COMPARE_EQU) == 0) && (number_a == number_b)) {
        ret = RET_OK;
    } else if ((g_strcmp0(flag, REGRIST_PROP_COMPARE_NEQ) == 0) && (number_a != number_b)) {
        ret = RET_OK;
    } else if ((g_strcmp0(flag, REGRIST_PROP_COMPARE_GTR) == 0) && (number_a > number_b)) {
        ret = RET_OK;
    } else if ((g_strcmp0(flag, REGRIST_PROP_COMPARE_GEQ) == 0) && (number_a >= number_b)) {
        ret = RET_OK;
    } else if ((g_strcmp0(flag, REGRIST_PROP_COMPARE_LSS) == 0) && (number_a < number_b)) {
        ret = RET_OK;
    } else if ((g_strcmp0(flag, REGRIST_PROP_COMPARE_LEQ) == 0) && (number_a <= number_b)) {
        ret = RET_OK;
    }
    return ret;
}


gint32 bios_get_json_file(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    const gchar *property_name = NULL;
    gsize file_name_len = 0;
    FM_FILE_S *file_handle = NULL;
    gchar *data_buf = NULL;

    const gchar* property_name_map[][2] =
    {
        {BIOS_FILE_REGISTRY_NAME, BIOS_JSON_FILE_REGISTRY_NAME},
        {BIOS_FILE_CURRENT_VALUE_NAME, BIOS_JSON_FILE_CURRENT_VALUE_NAME},
        {BIOS_FILE_SETTING_NAME, BIOS_JSON_FILE_SETTING_NAME},
        {BIOS_FILE_RESULT_NAME, BIOS_JSON_FILE_RESULT_NAME},
        {BIOS_FILE_CONFIGVALUE_NAME, BIOS_JSON_FILE_CONFIGVALUE_NAME},
        {BIOS_FILE_POLICYCONFIGREGISTRY_NAME, BIOS_JSON_FILE_POLICYONFIGREGISTRY_NAME},
        {BIOS_FILE_COMPONENTVERSION_NAME, BIOS_JSON_FILE_COMPONENTVERSION_NAME},
        {NULL, NULL}
    };

    
    if (input_list == NULL || output_list == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        goto err_out;
    }

    
    const gchar *file_name =
        g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), &file_name_len);
    if (file_name == NULL || file_name_len == 0) {
        debug_log(DLOG_ERROR, "%s: invalid file name.", __FUNCTION__);
        goto err_out;
    }

    
    for (gint32 i = 0; property_name_map[i][1] != NULL; i++) {
        if (!g_strcmp0(file_name, property_name_map[i][1])) {
            property_name = property_name_map[i][0];
            break;
        }
    }

    if (property_name == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid file name.", __FUNCTION__);
        goto err_out;
    }

    
    if (dal_get_fm_file_accessible(property_name, fm_get_module_name(), FM_POSITION_IN_RAM) != TRUE) {
        debug_log(DLOG_DEBUG, "%s: file %s not exist", __FUNCTION__, property_name);
        goto err_out;
    }

    
    file_handle = fm_fopen(property_name, "rb");
    if (file_handle == NULL) {
        debug_log(DLOG_ERROR, "%s: open file %s failed", __FUNCTION__, property_name);
        goto err_out;
    }

    
    gint32 fm_fseek_back = fm_fseek(file_handle, 0, SEEK_END);  // 返回成功是0，不成功非0
    if (fm_fseek_back != 0) {
        debug_log(DLOG_ERROR, "%s: file seek %s failed", __FUNCTION__, property_name);
        goto err_out;
    }

    gint32 file_size = fm_ftell(file_handle);
    if (file_size <= 0) {
        debug_log(DLOG_DEBUG, "%s: file tell %s failed", __FUNCTION__, property_name);
        goto err_out;
    }

    
    data_buf = (gchar *)g_malloc0(file_size + 1);
    if (data_buf == NULL) {
        debug_log(DLOG_ERROR, "g_malloc0 len:%d fail!\n", file_size + 1);
        goto err_out;
    }

    
    fm_fseek_back = fm_fseek(file_handle, 0, SEEK_SET);
    if (fm_fseek_back != 0) {
        debug_log(DLOG_ERROR, "%s: file seek %s failed", __FUNCTION__, property_name);
        goto err_out;
    }

    guint32 file_actual_size = fm_fread(data_buf, BIOS_FILE_STREAM_SIZE, (guint32)file_size, file_handle);
    if (file_size != file_actual_size) {
        debug_log(DLOG_ERROR, "read file(InLen:%d,OutLen:%d) failed", file_size, file_actual_size);
        goto err_out;
    }

    *output_list = g_slist_append(*output_list, g_variant_new_string(data_buf));

    
    fm_fclose(file_handle);
    g_free(data_buf);

    return RET_OK;

err_out:
    if (file_handle != NULL) {
        fm_fclose(file_handle);
    }
    if (data_buf != NULL) {
        g_free(data_buf);
    }

    return RET_ERR;
}


gint32 bios_set_print_flag(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    GVariant *property_value = NULL;
    gint8 Flag = -1;
    gint32 ret;

    if (input_list == NULL) {
        method_operation_log(caller_info, NULL, "Set BIOS debug info failed");
        return RET_ERR;
    }

    if (caller_info == NULL) {
        method_operation_log(caller_info, NULL, "Set BIOS debug info failed");
        return RET_ERR;
    }

    if (g_slist_length(input_list) != 1) {
        debug_log(DLOG_ERROR, "[%s][%d] failed with input_list length is not 1\n", __FUNCTION__, __LINE__);
        method_operation_log(caller_info, NULL, "Set BIOS debug info failed");
        return RET_ERR;
    }

    property_value = (GVariant *)g_slist_nth_data(input_list, 0);
    Flag = g_variant_get_byte(property_value);
    if (Flag) {
        ret = dal_set_property_value_byte(obj_handle, BIOS_PRINT_FLAG_NAME, Flag, DF_SAVE | DF_HW);
        if (ret != RET_OK) {
            method_operation_log(caller_info, NULL, "Set BIOS debug info enable failed");
            return RET_ERR;
        } else {
            method_operation_log(caller_info, NULL, "Set BIOS debug info enable successfully");
        }
    } else {
        ret = dal_set_property_value_byte(obj_handle, BIOS_PRINT_FLAG_NAME, Flag, DF_SAVE | DF_HW);
        if (ret != RET_OK) {
            method_operation_log(caller_info, NULL, "Set BIOS debug info enabled/disabled by BIOS setup menu failed");
            return RET_ERR;
        } else {
            method_operation_log(caller_info, NULL,
                "Set BIOS debug info enabled/disabled by BIOS setup menu successfully");
        }
    }

    return RET_OK;
}


gint32 bios_reset(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;

    if (vos_get_file_accessible(BIOS_FILE_SETTING_PATH) == TRUE) {
        
        ret = fm_clear_file(BIOS_FILE_SETTING_NAME);
        
        if ((gint32)ret != FM_OK) {
            debug_log(DLOG_ERROR, "%s: fm_clear_file %s failed", __FUNCTION__, BIOS_FILE_SETTING_NAME);
            return RET_ERR;
        }
        
        
        set_bios_setting_file_change_flag(BIOS_SETTING_FILE_UNCHANGED, 0);
    }

    return bios_clear_cmos(obj_handle, caller_info, input_list, output_list);
}


gint32 clear_bios_setting_file(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret = RET_OK;

    if (caller_info == NULL) {
        method_operation_log(caller_info, NULL, "Revoke BIOS settings Failed.");
        return RET_ERR;
    }

    if (vos_get_file_accessible(BIOS_FILE_SETTING_PATH) == TRUE) {
        
        ret = fm_clear_file(BIOS_FILE_SETTING_NAME);
        if (ret != RET_OK) {
            method_operation_log(caller_info, NULL, "Revoke BIOS settings Failed.");
            debug_log(DLOG_ERROR, "%s: fm_clear_file %s failed", __FUNCTION__, BIOS_FILE_SETTING_NAME);
            return RET_ERR;
        }
        
        set_bios_setting_file_change_flag(BIOS_SETTING_FILE_UNCHANGED, 0);

        method_operation_log(caller_info, NULL, "Revoke BIOS settings successfully.");
    }

    return RET_OK;
}


gint32 bios_get_setting_effective_status(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    if (output_list == NULL) {
        debug_log(DLOG_ERROR, "Input parameter is error \r\n");
        return RET_ERR;
    }

    *output_list = g_slist_append(*output_list, g_variant_new_byte(g_bios_setting_file_state));

    debug_log(DLOG_DEBUG, "%s : g_bios_setting_file_state = %d", __FUNCTION__, g_bios_setting_file_state);

    return RET_OK;
}


gint32 bios_get_server_type(guint8 *server_type)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret_val = RET_ERR;
    GVariant *property_data = NULL;

    ret_val = dfl_get_object_handle(BMC_PRODUCT_OBJ_NAME_APP, &obj_handle);
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return RET_ERR;
    }

    ret_val = dfl_get_property_value(obj_handle, PROPERTY_SERVER_TYPE, &property_data);
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return RET_ERR;
    }

    *server_type = g_variant_get_byte(property_data);
    g_variant_unref(property_data);

    return RET_OK;
}

gint32 set_flash_id(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    GVariant *property_data = NULL;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s input list is NULL", __FUNCTION__);
        return RET_ERR;
    }

    property_data = (GVariant *)(g_slist_nth_data(input_list, 0));
    if (property_data != NULL) {
        (void)dfl_set_property_value(obj_handle, PROPERTY_SPI_FLASH_ID, property_data, DF_SAVE);
    }
    return RET_OK;
}