

#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <math.h>
#include "pme_app/pme_app.h"
#include "pcie_card.h"
#include "pcie_card_parse_bdf_thread.h"
#include "card_ipmi_msg.h"
#include "get_version.h"
#include "card_manage_module.h"
#include "pcie_card_comm.h"
#include "pme_app/common/debug_log_macro.h"
#include "../../../libs/pcie_mgmt/inc/pcie_mgmt_by_std_smbus.h"
#include "pme_app/common/public_redfish.h"


LOCAL void network_bandwidth_usage_waveform_remove(OBJ_HANDLE card_handle);
LOCAL gint32 network_bandwidth_usage_task_init(void);
LOCAL void net_card_mctp_notsupport(OBJ_HANDLE obj_handle);


GSList *g_netcard_bwu_waveform_list = NULL;
GSList *g_busiport_bwu_wavedata_list = NULL;
LOCAL GMutex g_netcard_bwu_waveform_mutex;



static GAsyncQueue *g_net_bwu_wave_process_que = NULL;
static TASKID g_net_bwu_wave_process_task = 0x00;



gint32 net_card_init()
{
    GSList *obj_list = NULL;
    GSList *node = NULL;
    guint8 cpu_id = 0;
    gint32 ret = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar referencd_object[MAX_NAME_SIZE] = {0};
    gchar referencd_property[MAX_NAME_SIZE] = {0};

    
    // 由于网卡带宽使用率波形图链表的新增和移除操作在框架回调里进行，用锁有可能导致死锁，改用队列消息实现
    if (vos_queue_create(&g_net_bwu_wave_process_que) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: g_net_bwu_wave_process_que create failed.", __FUNCTION__);
    }

    g_mutex_init(&g_netcard_bwu_waveform_mutex);
    network_bandwidth_usage_task_init();
    

    ret = dfl_get_object_list(CLASS_NETCARD_NAME, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get NetCard object list failed.");
        return RET_ERR;
    }

    for (node = obj_list; node; node = node->next) {
        obj_handle = (OBJ_HANDLE)node->data;

        
        net_card_mctp_notsupport(obj_handle);

        // 如果是引用值就不更新属性
        
        ret = dfl_get_referenced_property(obj_handle, PROPERTY_NETCARD_CPU_ID, referencd_object, referencd_property,
            MAX_NAME_SIZE, MAX_NAME_SIZE);
        if (ret == DFL_OK) {
            continue;
        }
        
        
        ret = get_card_cpu_id(obj_handle, &cpu_id);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:Get %s ResID failed! ret:%d.", __FUNCTION__, dfl_get_object_name(obj_handle),
                ret);
            continue;
        }
        ret = dal_set_property_value_byte(obj_handle, PROPERTY_NETCARD_CPU_ID, cpu_id, DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:Update %s ResID(%d) failed! ret:%d.", __FUNCTION__,
                dfl_get_object_name(obj_handle), cpu_id, ret);
            continue;
        }
        
    }

    g_slist_free(obj_list);
    return RET_OK;
}


LOCAL void net_card_mctp_notsupport(OBJ_HANDLE obj_handle)
{
    guchar me_access_type = ME_ACCESS_TYPE_SMLINK;
    gint32 ret = RET_OK;

    ret = dal_get_func_ability(CLASS_ME_INFO, PROPERTY_ME_ACCESS_TYPE, &me_access_type);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "get MeInfo.MeAccessType failed!");
        return;
    }
    if (me_access_type != ME_ACCESS_TYPE_QZ) {
        return;
    }
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_NETCARD_MCTP_SUPPORT, 0, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : set %s value %s failed, ret=%d", __FUNCTION__,
            dfl_get_object_name(obj_handle), PROPERTY_NETCARD_MCTP_SUPPORT, ret);
        return;
    }
    debug_log(DLOG_DEBUG, "set MctpSupport to 0 success!");
}


gint32 net_card_add_object_callback(OBJ_HANDLE obj_handle)
{
    
    guint8 cpu_id = 0;
    gint32 ret = 0;
    gchar referencd_object[MAX_NAME_SIZE] = {0};
    gchar referencd_property[MAX_NAME_SIZE] = {0};
    OBJ_HANDLE smbios_handle = 0;
    guint8 smbios_state = SMBIOS_WRITE_NOT_START;
    guint8 virtual_flag = 0;

    
    net_card_mctp_notsupport(obj_handle);

    
    
    ret = dal_get_object_handle_nth(CLASS_NAME_SMBIOS, 0, &smbios_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get SMBios handle failed!");
    }
    (void)dal_get_property_value_byte(smbios_handle, PROPERTY_SMBIOS_STATUS_VALUE, &smbios_state);

    if (smbios_state == SMBIOS_WRITE_FINISH) {
        ret = update_netcard_and_port_bdf_info(obj_handle, NULL);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Update NetCard rootBDF or BusinessPort BDF failed!");
        }
    }
    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_NETCARD_VIRTUAL_FLAG, &virtual_flag);
    if (ret != RET_OK) {
        return ret;
    }
    if (virtual_flag == 1) {
        return RET_OK;
    }

    // 更新当前NetCard属于哪个CPU

    // 如果是引用值就直接返回
    
    ret = dfl_get_referenced_property(obj_handle, PROPERTY_NETCARD_CPU_ID, referencd_object, referencd_property,
        MAX_NAME_SIZE, MAX_NAME_SIZE);
    if (ret == DFL_OK) {
        return RET_OK;
    }

    
    
    ret = get_card_cpu_id(obj_handle, &cpu_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Get %s ResId failed! ret:%d.", __FUNCTION__, dfl_get_object_name(obj_handle), ret);
        return RET_ERR;
    }
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_NETCARD_CPU_ID, cpu_id, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Update %s ResId(%d) failed! ret:%d.", __FUNCTION__, dfl_get_object_name(obj_handle),
            cpu_id, ret);
        return RET_ERR;
    }
    
    

    return RET_OK;
}

gint32 net_card_del_object_callback(OBJ_HANDLE object_handle)
{
    guint8 default_state = 0;

    
    network_bandwidth_usage_waveform_remove(object_handle);
    

    
    (void)net_card_recover_device_status(object_handle, &default_state);
    

    return RET_OK;
}

LOCAL gint32 get_ethernet_netcard_necessary_jso(json_object *resource_jso, json_object **huawei_jso,
    json_object **driver_info_jso, json_object **bdf_num_jso)
{
    json_object *oem_jso = NULL;
    json_bool ret_bool;

    if (resource_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:input param error.", __FUNCTION__);
        return RET_ERR;
    }

    // 获取相应json对象用于从中提取属性信息
    ret_bool = json_object_object_get_ex(resource_jso, RF_PROPERTY_OEM, &oem_jso);
    if (!ret_bool || oem_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:get oem fail.", __FUNCTION__);
        return RET_OK;
    }

    ret_bool = json_object_object_get_ex(oem_jso, RF_PROPERTY_HUAWEI, huawei_jso);
    if (!ret_bool || (*huawei_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s:get object fail.", __FUNCTION__);
        return RET_OK;
    }

    ret_bool = json_object_object_get_ex(*huawei_jso, RF_PROPERTY_DRIVER_INFO, driver_info_jso);
    if (!ret_bool || (*driver_info_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s:get DriverInfo fail.", __FUNCTION__);
    }

    ret_bool = json_object_object_get_ex(*huawei_jso, RF_PROPERTY_BDF_NUMBER, bdf_num_jso);
    if (!ret_bool || (*bdf_num_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s:json_object_object_get_ex bdfbum fail.", __FUNCTION__);
    }

    return RET_OK;
}


LOCAL gint32 get_fc_netcard_necessary_jso(json_object *resource_jso, json_object **driver_info_jso,
    json_object **bdf_num_jso)
{
    json_bool ret_bool;

    if (resource_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:input param error.", __FUNCTION__);
        return RET_ERR;
    }

    // 获取相应json对象用于从中提取属性信息
    ret_bool = json_object_object_get_ex(resource_jso, RF_PROPERTY_DRIVER_INFO, driver_info_jso);
    if (!ret_bool || (*driver_info_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s:get DriverInfo fail.", __FUNCTION__);
    }

    ret_bool = json_object_object_get_ex(resource_jso, RF_PROPERTY_BDF_NUMBER, bdf_num_jso);
    if (!ret_bool || (*bdf_num_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s:json_object_object_get_ex bdfbum fail.", __FUNCTION__);
    }

    return RET_OK;
}


LOCAL void update_ethernet_netcard_property_from_bma(json_object *resource_jso, json_object *huawei_jso,
    json_object *driver_info_jso, json_object *bdf_num_jso, OBJ_HANDLE eth_card_handle)
{
    gint32 ret = RET_OK;
    gchar port_type[PROP_VAL_MAX_LENGTH] = {0};
    gchar chip_model[PROP_VAL_MAX_LENGTH] = {0};
    gchar chip_manu[PROP_VAL_MAX_LENGTH] = {0};

    // 入参为空时不返回，用于实现恢复网卡属性默认值

    // 对比FC资源，Ethernet资源有NICName，虚拟网口
    
    dal_cache_property_string(huawei_jso, RF_PROPERTY_ETH_PORT_TYPE, port_type, sizeof(port_type));

    if (g_strcmp0(RF_PROPERTY_ETH_PORT_TYPE_VIRTUAL, port_type) != 0) {
        // 芯片型号Model，如果xml配置的原始值为空或N/A，则更新该属性，否则以BMC配置的值为准
        ret = dal_get_property_value_string(eth_card_handle, BUSY_NETCARD_MODEL, chip_model, sizeof(chip_model));
        if (ret == RET_OK && (strlen(chip_model) == 0 || g_strcmp0(chip_model, "N/A") == 0)) {
            card_update_property_string(huawei_jso, BUSY_NETCARD_MODEL, eth_card_handle, BUSY_NETCARD_MODEL);
        }

        // 芯片厂商ChipManufacturer，如果xml配置的原始值为空或N/A，则更新该属性，否则以BMC配置的值为准
        ret = dal_get_property_value_string(eth_card_handle, BUSY_NETCARD_CHIP_MANU, chip_manu, sizeof(chip_manu));
        if (ret == RET_OK && (strlen(chip_manu) == 0 || g_strcmp0(chip_manu, "N/A") == 0)) {
            card_update_property_string(resource_jso, RF_PROPERTY_MANUFACTURER, eth_card_handle,
                BUSY_NETCARD_CHIP_MANU);
        }

        // 更新OS侧网卡名称
        card_update_property_string(huawei_jso, RF_PROPERTY_NIC_NAME, eth_card_handle, BUSY_NETCARD_OSNICNAME);

        // 更新OS侧网卡固件版本
        card_update_property_string(huawei_jso, BUSY_NETCARD_FIRM_VERSION, eth_card_handle, BUSY_NETCARD_FIRM_VERSION);

        // 更新OS侧网卡驱动名称
        card_update_property_string(driver_info_jso, RF_PROPERTY_DRIVE_NAME, eth_card_handle, BUSY_NETCARD_DRIVE_NAME);

        // 更新OS侧网卡驱动版本
        card_update_property_string(driver_info_jso, RF_PROPERTY_DRIVE_VERSION, eth_card_handle,
            BUSY_NETCARD_DRIVE_VERSION);

        // 更新OS侧网卡描述信息
        card_update_property_string(resource_jso, RF_PROPERTY_DESCRIPTION, eth_card_handle, BUSY_NETCARD_DESCRIPTION);

        // 更新OS侧ROOTBDF
        
        
        card_update_rootbdf_property_string_with_limit(bdf_num_jso, RF_PROPERTY_ROOT_BDF, eth_card_handle,
            BUSY_NETCARD_ROOT_BDF);
        
    }

    
}


LOCAL void update_fc_netcard_property_from_bma(json_object *resource_jso, json_object *driver_info_jso,
    json_object *bdf_num_jso, OBJ_HANDLE fc_card_handle)
{
    gint32 ret = RET_OK;
    gchar chip_model[PROP_VAL_MAX_LENGTH] = {0};
    gchar chip_manu[PROP_VAL_MAX_LENGTH] = {0};

    // 入参为空时不返回，用于恢复网卡属性值为默认值

    // 对比Ethernet资源，FC资源没有NICName，没有虚拟网口

    // 芯片型号Model，如果xml配置的原始值为空或N/A，则更新该属性，否则以BMC配置的值为准
    ret = dal_get_property_value_string(fc_card_handle, BUSY_NETCARD_MODEL, chip_model, sizeof(chip_model));
    if (ret == RET_OK && (strlen(chip_model) == 0 || g_strcmp0(chip_model, "N/A") == 0)) {
        card_update_property_string(resource_jso, BUSY_NETCARD_MODEL, fc_card_handle, BUSY_NETCARD_MODEL);
    }

    // 芯片厂商ChipManufacturer，如果xml配置的原始值为空或N/A，则更新该属性，否则以BMC配置的值为准
    ret = dal_get_property_value_string(fc_card_handle, BUSY_NETCARD_CHIP_MANU, chip_manu, sizeof(chip_manu));
    if (ret == RET_OK && (strlen(chip_manu) == 0 || g_strcmp0(chip_manu, "N/A") == 0)) {
        card_update_property_string(resource_jso, RF_PROPERTY_MANUFACTURER, fc_card_handle, BUSY_NETCARD_CHIP_MANU);
    }

    // 更新OS侧网卡固件版本
    card_update_property_string(resource_jso, BUSY_NETCARD_FIRM_VERSION, fc_card_handle, BUSY_NETCARD_FIRM_VERSION);

    // 更新OS侧网卡驱动名称
    card_update_property_string(driver_info_jso, RF_PROPERTY_DRIVE_NAME, fc_card_handle, BUSY_NETCARD_DRIVE_NAME);

    // 更新OS侧网卡驱动版本
    card_update_property_string(driver_info_jso, RF_PROPERTY_DRIVE_VERSION, fc_card_handle, BUSY_NETCARD_DRIVE_VERSION);

    // 更新OS侧网卡描述信息
    card_update_property_string(resource_jso, RF_PROPERTY_DESCRIPTION, fc_card_handle, BUSY_NETCARD_DESCRIPTION);

    // 更新OS侧ROOTBDF
    
    
    card_update_rootbdf_property_string_with_limit(bdf_num_jso, RF_PROPERTY_ROOT_BDF, fc_card_handle,
        BUSY_NETCARD_ROOT_BDF);
    
}


gint32 card_manage_set_netcard_sn(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s : Parameter invalid.", __FUNCTION__);
        return RET_ERR;
    }
    GVariant *property_value = (GVariant *)g_slist_nth_data(input_list, 0);
    const gchar *sn = g_variant_get_string(property_value, NULL);
    if (sn == NULL) {
        debug_log(DLOG_ERROR, "%s : netcard serial number is null.", __FUNCTION__);
        return RET_ERR;
    }
    
    gint32 ret = dal_set_property_value_string(object_handle, PROPERTY_NETCARD_SERIAL_NUMBER, sn, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : Set property(%s) failed.", __FUNCTION__, PROPERTY_NETCARD_SERIAL_NUMBER);
        return ret;
    }
    return RET_OK;
}


gint32 card_manage_set_firmver_from_mctp(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    const gchar *firmware_ver = NULL;
    GVariant *property_value = NULL;
    gint32 ret = 0;

    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s : Parameter invalid.", __FUNCTION__);
        return RET_ERR;
    }
    property_value = (GVariant *)g_slist_nth_data(input_list, 0);
    firmware_ver = g_variant_get_string(property_value, NULL);
    if (firmware_ver == NULL) {
        debug_log(DLOG_ERROR, "%s : firmware_ver == null.", __FUNCTION__);
        return RET_ERR;
    }
    
    ret = dal_set_property_value_string(object_handle, BUSY_NETCARD_FIRM_VERSION, firmware_ver, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : Set property(%s) failed.", __FUNCTION__, BUSY_NETCARD_FIRM_VERSION);
        return ret;
    }
    return RET_OK;
}


gint32 card_manage_set_vid_did_from_mctp(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    guint32 ret;
    guint16 vid;
    guint16 did;
    guint16 sub_vid;
    guint16 sub_did;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s: Parameter invalid.", __FUNCTION__);
        return RET_ERR;
    }
    vid = g_variant_get_uint16((GVariant *)g_slist_nth_data(input_list, 0));
    did = g_variant_get_uint16((GVariant *)g_slist_nth_data(input_list, 1));
    sub_vid = g_variant_get_uint16((GVariant *)g_slist_nth_data(input_list, 2)); // 下标 2-Sub VendorId
    sub_did = g_variant_get_uint16((GVariant *)g_slist_nth_data(input_list, 3)); // 下标 3-Sub DeviceId

    ret = (guint32)dal_set_property_value_uint16(object_handle, PROPERTY_NETCARD_VENDER_ID, vid, DF_NONE);
    ret |= (guint32)dal_set_property_value_uint16(object_handle, PROPERTY_NETCARD_DEVICE_ID, did, DF_NONE);
    ret |= (guint32)dal_set_property_value_uint16(object_handle, PROPERTY_NETCARD_SUB_VENDER_VID, sub_vid, DF_NONE);
    ret |= (guint32)dal_set_property_value_uint16(object_handle, PROPERTY_NETCARD_SUB_DEVICE_ID, sub_did, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : Set vid: %u did: %u sub_vid: %u sub_did: %u failed.", __FUNCTION__, vid, did,
            sub_vid, sub_did);
    }

    return ret;
}


gint32 card_manage_set_net_from_bma(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = RET_OK;
    const gchar *request_str = 0;
    json_object *resource_jso = NULL;
    json_object *huawei_jso = NULL;
    json_object *driver_info_jso = NULL;
    json_object *bdf_num_jso = NULL;
    guint8 resource_type;

    if (input_list == NULL) {
        return RET_ERR;
    }

    request_str = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), NULL);
    if (request_str == NULL) {
        debug_log(DLOG_ERROR, "%s: request_str=NULL.", __FUNCTION__);
        return RET_ERR;
    }
    resource_type = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1));

    
    if (request_str != NULL && strlen(request_str) != 0) {
        resource_jso = json_tokener_parse(request_str);
        if (resource_jso == NULL) {
            debug_log(DLOG_ERROR, "%s: request_jso=NULL.", __FUNCTION__);
            return RET_ERR;
        }

        if (resource_type == UPDATE_RESOURCE_TYPE_ETHERNET) {
            ret = get_ethernet_netcard_necessary_jso(resource_jso, &huawei_jso, &driver_info_jso, &bdf_num_jso);
            if (ret != RET_OK) {
                json_object_put(resource_jso);
                debug_log(DLOG_ERROR, "%s: get_ethernet_netcard_necessary_jso fail.", __FUNCTION__);
                return RET_ERR;
            }
        } else if (resource_type == UPDATE_RESOURCE_TYPE_FC) {
            ret = get_fc_netcard_necessary_jso(resource_jso, &driver_info_jso, &bdf_num_jso);
            if (ret != RET_OK) {
                json_object_put(resource_jso);
                debug_log(DLOG_ERROR, "%s: get_fc_netcard_necessary_jso fail.", __FUNCTION__);
                return RET_ERR;
            }
        }
    }

    if (resource_type == UPDATE_RESOURCE_TYPE_ETHERNET) {
        update_ethernet_netcard_property_from_bma(resource_jso, huawei_jso, driver_info_jso, bdf_num_jso,
            object_handle);
    } else if (resource_type == UPDATE_RESOURCE_TYPE_FC) {
        update_fc_netcard_property_from_bma(resource_jso, driver_info_jso, bdf_num_jso, object_handle);
    }

    

    if (resource_jso != NULL) {
        json_object_put(resource_jso);
    }
    return ret;
}


void net_card_set_device_status_default_value(void)
{
    gint32 ret = 0;
    guint8 default_state = 0;

    ret = dfl_foreach_object(CLASS_NETCARD_NAME, net_card_recover_device_status, &default_state, NULL);
    if (ret != DFL_OK) {
        if (ret != ERRCODE_OBJECT_NOT_EXIST) {
            debug_log(DLOG_ERROR, "Recover net card default value failed!");
        }
    }

    return;
}

gint32 ipmi_get_net_card_chip_info(const void *msg_data, gpointer user_data)
{
    errno_t safe_fun_ret = EOK;
    const GET_NETCARD_CHIP_INFO_REQ_S *req_data = NULL;
    GET_NETCARD_CHIP_INFO_RSP_S resp_data = { 0 };
    OBJ_HANDLE netcard_obj_handle = 0;
    GVariant *array_netcard_chip = NULL;
    GVariant *chip_obj_gvar = NULL;
    GVariantIter iter;
    const gchar *string = NULL;
    OBJ_HANDLE netcard_chip_obj = 0;
    guint8 chip_port_num = 0;
    gchar chip_model[32] = {0};
    guint8 chip_num = 0;
    guint8 *data = NULL;
    guint32 result = RET_OK;

    resp_data.manufacturer = HUAWEI_MFG_ID;
    req_data = (const GET_NETCARD_CHIP_INFO_REQ_S *)get_ipmi_src_data(msg_data);
    if (req_data == NULL) {
        resp_data.comp_code = COMP_CODE_UNKNOWN;
        return ipmi_send_response(msg_data, 4, (guint8 *)&resp_data);
    }

    
    if (req_data->manufacturer != HUAWEI_MFG_ID) {
        resp_data.comp_code = COMP_CODE_INVALID_FIELD;
        debug_log(DLOG_ERROR, "%s:req_data->manufacturer:%x ", __FUNCTION__, req_data->manufacturer);
        return ipmi_send_response(msg_data, 4, (guint8 *)&resp_data);
    }

    
    if (req_data->offset != 0) {
        resp_data.comp_code = COMP_CODE_OUTOF_RANGE;
        return ipmi_send_response(msg_data, 4, (guint8 *)&resp_data);
    }

    
    result =
        dal_get_specific_object_byte(CLASS_NETCARD_NAME, PROPERTY_NETCARD_CARDTYPE, NET_TYPE_LOM, &netcard_obj_handle);
    if (result != RET_OK) {
        resp_data.comp_code = COMP_CODE_INVALID_FIELD;
        debug_log(DLOG_ERROR, "%s:result:%x ", __FUNCTION__, result);
        return ipmi_send_response(msg_data, 4, (guint8 *)&resp_data);
    }

    data = &(resp_data.data[1]);
    (void)dfl_get_property_value(netcard_obj_handle, PROPERTY_REF_NETCARD_CHIP_NAME, &array_netcard_chip);
    (void)g_variant_iter_init(&iter, array_netcard_chip);

    while ((chip_obj_gvar = g_variant_iter_next_value(&iter)) != NULL) {
        string = g_variant_get_string(chip_obj_gvar, NULL);
        result = dfl_get_object_handle(string, &netcard_chip_obj);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "%s:get the action handle(%s) result=%d ", __FUNCTION__, string, result);
            g_variant_unref(chip_obj_gvar);
            chip_obj_gvar = NULL;
            continue;
        }

        chip_num++;
        (void)dal_get_property_value_byte(netcard_chip_obj, PROPERTY_NETCARD_CHIP_PORT_NUM, &chip_port_num);
        (void)dal_get_property_value_string(netcard_chip_obj, PROPERTY_NETCARD_CHIP_MODEL, chip_model,
            sizeof(chip_model));

        g_variant_unref(chip_obj_gvar);
        chip_obj_gvar = NULL;

        *data++ = 2;                     // 第n个芯片的信息数量（目前是2个）
        *data++ = (guint8)sizeof(chip_port_num); // 第n个芯片的端口数量信息长度（L）
        *data++ = chip_port_num;         // 第n个芯片的端口数量（V）
        *data++ = (guint8)strlen(chip_model);    // 第n个芯片的芯片型号信息长度（L）
        safe_fun_ret = memcpy_s(data, strlen(chip_model), chip_model, strlen(chip_model)); // 第n个芯片的芯片型号（V）
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }

        data = data + strlen(chip_model);
    }

    g_variant_unref(array_netcard_chip);

    resp_data.end_flag = 0;
    resp_data.data[0] = chip_num;
    

    return ipmi_send_response(msg_data, 5 + (data - &resp_data.data[0]), (guint8 *)&resp_data);
}

LOCAL gint32 card_manage_set_bwu_wave_title(OBJ_HANDLE object_handle)
{
    gint32 ret = RET_OK;
    OBJ_HANDLE com_handle;
    gchar card_desc[PROP_VAL_LENGTH] = {0};
    gchar productname[PROP_VAL_LENGTH] = {0};
    gchar location[PROP_VAL_LENGTH] = {0};
    gchar devicename[PROP_VAL_LENGTH] = {0};
    gchar wave_title[PROP_VAL_MAX_LENGTH] = {0};
    guint8 retry = SET_BWU_WAVE_TITLE_RETRY;

    // 获取网卡的CardDesc和ProductName表征网卡特性
    ret = dal_get_property_value_string(object_handle, PROPERTY_NETCARD_DESC, card_desc, sizeof(card_desc));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get card_desc fail", __FUNCTION__);
        return RET_ERR;
    }

    ret = dal_get_property_value_string(object_handle, PROPERTY_NETCARD_NAME, productname, sizeof(productname));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get product name fail", __FUNCTION__);
        return RET_ERR;
    }

    ret = dfl_get_referenced_object(object_handle, PROPERTY_NETCARD_REF_COMPONENT, &com_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:get com_handle fail", __FUNCTION__);
        return RET_ERR;
    }

    // 获取Component对象的location和devicename表征网卡的位置信息
    ret = dal_get_property_value_string(com_handle, PROPERTY_COMPONENT_LOCATION, location, sizeof(location));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get location fail", __FUNCTION__);
        return RET_ERR;
    }

    while (retry--) {
        ret = dal_get_property_value_string(com_handle, PROPERTY_COMPONENT_DEVICE_NAME, devicename, sizeof(devicename));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:get device name fail", __FUNCTION__);
            return RET_ERR;
        }
        debug_log(DLOG_DEBUG, "Device name is %s.", devicename);

        if (!strstr(devicename, DEVICE_NAME_SPLIT_FLAG) && !strstr(devicename, DEVICE_NAME_ADD_LOCATION_FLAG)) {
            debug_log(DLOG_DEBUG, "Device name(%s) is init ok.", devicename);
            break;
        }
        vos_task_delay(SET_BWU_WAVE_TITLE_DELAY);
    }
    if (g_ascii_strcasecmp(location, "mainboard") == 0) { // mainboard不需要对外呈现
        ret = snprintf_s(wave_title, sizeof(wave_title), sizeof(wave_title) - 1, "%s - %s(%s)", devicename, productname,
            card_desc);
    } else {
        ret = snprintf_s(wave_title, sizeof(wave_title), sizeof(wave_title) - 1, "%s %s - %s(%s)", location, devicename,
            productname, card_desc);
    }

    if (ret <= 0) {
        debug_log(DLOG_INFO, "%s:get wave_title NULL", __FUNCTION__);
    }

    ret = dal_set_property_value_string(object_handle, BUSY_NETCARD_BWU_WAVE_TITLE, wave_title, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:set netcard bandwidth usage waveforms title(%s) fail.", __FUNCTION__, wave_title);
    }

    return ret;
}


gint32 netcard_bandwidth_usage_rollback(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    GSList *wave_node = NULL;
    NETCARD_BWU_WAVE_INFO *wave_info = NULL;

    gint32 ret;

    g_mutex_lock(&g_netcard_bwu_waveform_mutex);

    // 遍历g_netcard_bwu_waveforms_list，通过card_handle找到当前网卡的波形图信息
    for (wave_node = g_netcard_bwu_waveform_list; wave_node; wave_node = wave_node->next) {
        wave_info = (NETCARD_BWU_WAVE_INFO *)wave_node->data;

        if (wave_info->port_num == 0) {
            debug_log(DLOG_ERROR, "%s:card_wave_info->port_num invalid.", __FUNCTION__);
            continue;
        }

        ret = wave_record_rollback(wave_info->wave_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: port %d wave record rollback failed", __FUNCTION__, wave_info->port_num);
        }
    }

    g_mutex_unlock(&g_netcard_bwu_waveform_mutex);

    return RET_OK;
}


LOCAL void get_port_min_silknum(OBJ_HANDLE card_handle, guint8 *silk_num)
{
    GSList *port_list = NULL;
    GSList *node = NULL;
    guint8 silk;
    guint8 min_silk = 0xff; 
    OBJ_HANDLE card_obj;
    OBJ_HANDLE port_obj;
    gint32 ret;

    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &port_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get business port list failed, ret=%d.", ret);
        return;
    }

    for (node = port_list; node; node = node->next) {
        port_obj = (OBJ_HANDLE)node->data;
        ret = dfl_get_referenced_object(port_obj, BUSY_ETH_ATTRIBUTE_REF_NETCARD, &card_obj);
        if (ret != RET_OK) {
            break;
        }

        if (card_obj != card_handle) {
            continue;
        }

        ret = dal_get_property_value_byte(port_obj, BUSY_ETH_ATTRIBUTE_SILK_NUM, &silk);
        if (ret != RET_OK) {
            break;
        }
        min_silk = MIN(min_silk, silk);
    }

    g_slist_free(port_list);
    if (ret == RET_OK) { 
        *silk_num = min_silk;
    } else {
        *silk_num = 1; 
    }
}


LOCAL void create_netcard_waveform(OBJ_HANDLE card_handle, gchar *wave_filename, guint8 port_num)
{
#define PORT_STR_MAX_LEN 8 // BMC配置xml的网卡网口数量最大值是Port255

    
    WAVE_CFG_S card_wave_cfg = { 0 };
    BPORT_BWU_WAVE_INFO *bport_info = NULL;
    gchar wave_name_buff[MAX_FILE_NAME_LEN] = {0};
    guint8 min_silk_num = 1; 
    guint32 i = 0;

    if (port_num == 0 || wave_filename == NULL) {
        debug_log(DLOG_ERROR, "%s: input para error", __FUNCTION__);
        return;
    }

    gchar **label = (gchar **)g_malloc0(port_num * sizeof(gchar *));
    if (label == NULL) {
        debug_log(DLOG_ERROR, "%s: g_malloc0 for label failed", __FUNCTION__);
        return;
    }

    // 波形图配置
    for (i = 0; i < port_num; i++) {
        label[i] = (gchar *)g_malloc0(PORT_STR_MAX_LEN);
        if (label[i] == NULL) {
            debug_log(DLOG_ERROR, "%s: g_malloc0 for label[%d] failed", __FUNCTION__, i);
            goto exit;
        }

        if (snprintf_s(label[i], PORT_STR_MAX_LEN, PORT_STR_MAX_LEN - 1, "Port%u", i + 1) <= 0) {
            debug_log(DLOG_ERROR, "%s: set (%s) bandwidth usage waveforms labels failed", __FUNCTION__, wave_filename);
        }
    }

    (void)memset_s(&card_wave_cfg, sizeof(WAVE_CFG_S), 0, sizeof(WAVE_CFG_S));
    
    // 数据会先写到缓存文件wave_name
    card_wave_cfg.wave_name = wave_name_buff;

    
    
    if (snprintf_s(wave_name_buff, sizeof(wave_name_buff), sizeof(wave_name_buff) - 1, "%s_nwbwusage_1",
        dfl_get_object_name(card_handle)) <= 0) {
        debug_log(DLOG_ERROR, "%s: set wave_name failed", __FUNCTION__);
    }

    

    
    // 先从缓存文件wave_name加载历史数据，再更新值到wave_name，最后把wave_name的数据按格式刷到web_view_path
    card_wave_cfg.web_view_path = (gchar *)g_malloc0(MAX_FILE_NAME_LEN);
    if (card_wave_cfg.web_view_path == NULL) {
        debug_log(DLOG_ERROR, "%s: g_malloc0 for web_view_path failed", __FUNCTION__);
        goto exit;
    }
    if (snprintf_s(card_wave_cfg.web_view_path, MAX_FILE_NAME_LEN, MAX_FILE_NAME_LEN - 1, "%s%s", TMPFS_PATH,
        wave_filename) <= 0) {
        debug_log(DLOG_ERROR, "%s: set web_view_path failed", __FUNCTION__);
    }

    card_wave_cfg.smm_view_path = NULL;
    card_wave_cfg.max_interval = 60;
    card_wave_cfg.max_item_cnt = port_num;
    card_wave_cfg.max_record_cnt = 60;
    card_wave_cfg.item_label = label;

    // 卡的波形图信息初始化
    NETCARD_BWU_WAVE_INFO *card_info = (NETCARD_BWU_WAVE_INFO *)g_malloc0(sizeof(NETCARD_BWU_WAVE_INFO));
    if (card_info == NULL) {
        debug_log(DLOG_ERROR, "%s: g_malloc0 for card_info failed", __FUNCTION__);
        goto exit;
    }

    card_info->port_num = port_num;
    card_info->card_handle = card_handle;
    card_info->wave_handle = wave_init(&card_wave_cfg);
    if (card_info->wave_handle == NULL) {
        debug_log(DLOG_ERROR, "init %s card_info failed.", wave_filename);
    }

    // 将初始化完毕的波形图信息加到链表
    g_netcard_bwu_waveform_list = g_slist_append(g_netcard_bwu_waveform_list, (gpointer)card_info);
    get_port_min_silknum(card_handle, &min_silk_num);

    // 进行网口波形数据初始化，通过保存的NetCard句柄和SilkNum确定当前初始化的是哪张网卡的哪个网口的波形数据
    for (i = 0; i < port_num; i++) {
        bport_info = (BPORT_BWU_WAVE_INFO *)g_malloc0(sizeof(BPORT_BWU_WAVE_INFO));
        if (bport_info == NULL) {
            continue;
        }
        bport_info->stat_whole_life = stat_init();
        bport_info->stat_one_cycle = stat_init();
        bport_info->silk_num = i + min_silk_num;
        bport_info->card_handle = card_handle;

        if (bport_info->stat_whole_life == NULL || bport_info->stat_one_cycle == NULL) {
            debug_log(DLOG_ERROR, "init %s bport_info->silk_num(%d) failed.", wave_filename, bport_info->silk_num);
            // 释放结构体成员函数申请的空间
            g_free(bport_info->stat_whole_life);
            g_free(bport_info->stat_one_cycle);
            g_free(bport_info);
        } else {
            g_busiport_bwu_wavedata_list = g_slist_append(g_busiport_bwu_wavedata_list, (gpointer)bport_info);
        }

        bport_info = NULL;
    }

exit:

    for (i = 0; i < port_num; i++) {
        if (label[i] != NULL) {
            g_free(label[i]);
        }
    }

    if (label != NULL) {
        g_free(label);
    }
    if (card_wave_cfg.web_view_path != NULL) {
        g_free(card_wave_cfg.web_view_path);
    }

    return;
}


LOCAL guint32 check_netcard_busiport_type(OBJ_HANDLE card_handle)
{
    gint32 ret = RET_OK;
    GSList *busi_list = NULL;
    GSList *busi_node = NULL;
    OBJ_HANDLE busi_handle;
    OBJ_HANDLE ref_handle;
    guint32 func_type;
    guint32 card_busi_type = NETCARD_BUSIPORT_TYPE_UNKNOWN;

    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &busi_list);
    if (ret != DFL_OK || busi_list == NULL) {
        debug_log(DLOG_DEBUG, "%s:get busi_list failed", __FUNCTION__);
        return card_busi_type;
    }

    for (busi_node = busi_list; busi_node; busi_node = busi_node->next) {
        busi_handle = (OBJ_HANDLE)busi_node->data;

        ret = dfl_get_referenced_object(busi_handle, BUSY_ETH_ATTRIBUTE_REF_NETCARD, &ref_handle);
        // 过滤掉不是该网卡的网口
        if (ret != DFL_OK || card_handle != ref_handle) {
            continue;
        }

        // 考虑网口可以切换形态FC-FCOE-ETH，所以根据support_functype来判断
        ret = dal_get_property_value_uint32(busi_handle, BUSY_ETH_ATTRIBUTE_SUPPORT_FUNCTYPE, &func_type);
        if (ret != RET_OK) {
            continue;
        }
        if (func_type & NETDEV_FUNCTYPE_FC) {
            (card_busi_type |= NETCARD_BUSIPORT_TYPE_FC);
        } // FC
        if (func_type & NETDEV_FUNCTYPE_ETHERNET) {
            (card_busi_type |= NETCARD_BUSIPORT_TYPE_ETH);
        } // ETH
        
        if (func_type & NETDEV_FUNCTYPE_IB) {
            (card_busi_type |= NETCARD_BUSIPORT_TYPE_IB);
        } // IB
        
    }

    g_slist_free(busi_list);
    return card_busi_type;
}


LOCAL gint32 check_netcard_state_for_waveform_init(OBJ_HANDLE card_handle)
{
    gint32 ret = RET_OK;
    guchar virtual_flag = 0;
    GSList *wave_node = NULL;
    NETCARD_BWU_WAVE_INFO *wave_info = NULL;

    // 虚拟网卡不需要生成波形图
    if (g_strrstr(dfl_get_object_name(card_handle), DYNAMIC_OBJ_ETH_CARD) != NULL) {
        debug_log(DLOG_DEBUG, "%s: this is dynamic object eth card.", __FUNCTION__);
        return RET_ERR;
    }

    // 已生成波形图的网卡不需要再生成
    for (wave_node = g_netcard_bwu_waveform_list; wave_node; wave_node = wave_node->next) {
        wave_info = (NETCARD_BWU_WAVE_INFO *)wave_node->data;

        // 波形图标题包含Component下的属性，跨进程加载慢，为免出现PcieCard$的标题，每次检查到重复初始化的波形图都再刷一次标题
        if (card_handle == wave_info->card_handle) {
            (void)card_manage_set_bwu_wave_title(card_handle);
        }

        if (card_handle == wave_info->card_handle) {
            debug_log(DLOG_DEBUG, "%s: this card waveforms has been created.", __FUNCTION__);
            return RET_ERR;
        }
    }

    
    ret = dal_get_property_value_byte(card_handle, PROPERTY_NETCARD_VIRTUAL_FLAG, &virtual_flag);
    if ((ret == RET_OK) && (virtual_flag == AVAILABLE)) {
        debug_log(DLOG_DEBUG, "%s: this is virtual eth card.", __FUNCTION__);
        return RET_ERR;
    }

    // 网口类型全为FC或未知，不需要创建波形图
    guint32 type = check_netcard_busiport_type(card_handle);
    if (type == NETCARD_BUSIPORT_TYPE_FC || type == NETCARD_BUSIPORT_TYPE_UNKNOWN) {
        return RET_ERR;
    } else {
        return RET_OK;
    }
}


LOCAL void network_bandwidth_usage_waveform_add(void)
{
    gint32 ret;
    GSList *netcard_list = NULL;
    GSList *netcard_node = NULL;
    OBJ_HANDLE card_handle;
    guint8 port_num;
    gchar  wave_filename[PROP_VAL_LENGTH + 20] = {0}; // 文件名是NetCard对象名和_bwu_webview.dat的拼接，20长度为后半段字符串预留

    ret = dfl_get_object_list(CLASS_NETCARD_NAME, &netcard_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:get netcard_list fail.", __FUNCTION__);
        return;
    }

    g_mutex_lock(&g_netcard_bwu_waveform_mutex);

    for (netcard_node = netcard_list; netcard_node; netcard_node = netcard_node->next) {
        card_handle = (OBJ_HANDLE)netcard_node->data;

        // 屏蔽虚拟网卡、FC卡、已创建波形图的网卡
        if (check_netcard_state_for_waveform_init(card_handle) != RET_OK) {
            continue;
        }

        // 获取网卡网口数，用于生成dat文件首行"id": "1","label": "Port1"#"id": "2","label": "Port2"#
        ret = dal_get_property_value_byte(card_handle, PROPERTY_NETCARD_PORTNUM, &port_num);
        if (ret != RET_OK) {
            continue;
        }

        // 获取卡对象名拼接成文件名称，写入BWUWaveFileName，用于生成dat文件名
        (void)memset_s(wave_filename, sizeof(wave_filename), 0, sizeof(wave_filename));

        
        
        if (snprintf_s(wave_filename, sizeof(wave_filename), sizeof(wave_filename) - 1, "%s_bwu_webview_1.dat",
            dfl_get_object_name(card_handle)) <= 0) {
            continue;
        }
        

        // 写入文件名，供前台调用
        ret = dal_set_property_value_string(card_handle, BUSY_NETCARD_BWU_WAVE_FILENAME, wave_filename, DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:set netcard bandwidth usage waveforms filename(%s) fail.", __FUNCTION__,
                wave_filename);
        }

        // 写入图表标题，供前台调用
        ret = card_manage_set_bwu_wave_title(card_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:set netcard bandwidth usage waveforms title fail.", __FUNCTION__);
        }

        // 初始化录波数据接口
        create_netcard_waveform(card_handle, wave_filename, port_num);
    }

    g_mutex_unlock(&g_netcard_bwu_waveform_mutex);

    g_slist_free(netcard_list);

    return;
}


LOCAL gint32 get_netcard_bandwidth_usage_wave_node(OBJ_HANDLE card_handle, NETCARD_BWU_WAVE_INFO **o_wave_info)
{
    GSList *wave_node = NULL;
    NETCARD_BWU_WAVE_INFO *wave_info = NULL;

    if (o_wave_info == NULL) {
        debug_log(DLOG_INFO, "%s:input para NULL", __FUNCTION__);
        return RET_ERR;
    }

    for (wave_node = g_netcard_bwu_waveform_list; wave_node; wave_node = wave_node->next) {
        wave_info = (NETCARD_BWU_WAVE_INFO *)wave_node->data;

        // 根据传入网卡句柄和带宽使用率波形图链表保存的网卡句柄匹配
        if (card_handle == wave_info->card_handle) {
            *o_wave_info = wave_info;
            return RET_OK;
        }
    }

    return RET_ERR;
}


LOCAL void bport_bandwidth_usage_wave_stat_clear(OBJ_HANDLE card_handle)
{
    GSList *bport_wave_node = NULL;
    BPORT_BWU_WAVE_INFO *bport_wave_info = NULL;

    // 遍历g_busiport_bwu_wavedata_list，匹配网卡句柄
    for (bport_wave_node = g_busiport_bwu_wavedata_list; bport_wave_node; bport_wave_node = bport_wave_node->next) {
        bport_wave_info = (BPORT_BWU_WAVE_INFO *)bport_wave_node->data;

        // 根据card_handle匹配，匹配成功则清除该网口one_cycle数据
        if (card_handle == bport_wave_info->card_handle) {
            stat_clear(bport_wave_info->stat_one_cycle);
        }
    }

    return;
}


gint32 card_manage_update_bandwidth_usage_wave(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    GSList *busi_port_list = NULL;
    GSList *node = NULL;
    OBJ_HANDLE busi_handle;
    OBJ_HANDLE refcard_handle;
    guint8 silk_num = 0;
    guint32 cur_time = 0;
    NETCARD_BWU_WAVE_INFO *card_wave_info = NULL;
    guint16 bandwidth_usage;
    WAVE_RECORD_S record_info;
    GSList *bport_wave_node = NULL;
    BPORT_BWU_WAVE_INFO *bport_wave_info = NULL;
    guint32 record_data[MAX_NETCARD_PORT];

    g_mutex_lock(&g_netcard_bwu_waveform_mutex);
    // 遍历g_netcard_bwu_waveforms_list，通过card_handle找到当前网卡的波形图信息
    gint32 ret = get_netcard_bandwidth_usage_wave_node(object_handle, &card_wave_info);
    if (ret != RET_OK || card_wave_info == NULL) {
        g_mutex_unlock(&g_netcard_bwu_waveform_mutex);
        debug_log(DLOG_INFO, "%s:get card_wave_info failed.", __FUNCTION__);
        return RET_ERR;
    }

    if (card_wave_info->port_num == 0 || card_wave_info->port_num > MAX_NETCARD_PORT) {
        g_mutex_unlock(&g_netcard_bwu_waveform_mutex);
        debug_log(DLOG_INFO, "%s:card_wave_info->port_num invalid.", __FUNCTION__);
        return RET_ERR;
    }

    cur_time = (guint32)vos_get_cur_time_stamp();

    // 遍历BusinessPort，匹配网卡句柄，获取到该网卡各个网口带宽使用率、网口丝印信息
    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &busi_port_list);
    if (ret != DFL_OK) {
        g_mutex_unlock(&g_netcard_bwu_waveform_mutex);
        debug_log(DLOG_ERROR, "%s:get busi_port_list fail.", __FUNCTION__);
        return RET_ERR;
    }

    for (node = busi_port_list; node; node = node->next) {
        busi_handle = (OBJ_HANDLE)node->data;
        // 网口不是同一张网卡不处理
        ret = dfl_get_referenced_object(busi_handle, BUSY_ETH_ATTRIBUTE_REF_NETCARD, &refcard_handle);
        if (ret != DFL_OK || refcard_handle != object_handle) {
            continue;
        }

        
        ret = dal_get_property_value_byte(busi_handle, BUSY_ETH_ATTRIBUTE_SILK_NUM, &silk_num);
        if (ret != RET_OK || silk_num < 1 || silk_num > MAX_NETCARD_PORT) {
            debug_log(DLOG_DEBUG, "%s: get silk_num failed, ret = %d, silk_num = %d", __FUNCTION__, ret, silk_num);
            continue;
        }

        // 使用率为无效值则描点0
        ret = dal_get_property_value_uint16(busi_handle, BUSY_ETH_ATTRIBUTE_BANDWIDTH_USAGE, &bandwidth_usage);
        
        
        if (ret != RET_OK || bandwidth_usage > 10000) {
            record_data[silk_num - 1] = 0;
            continue;
        }
        

        // 遍历g_busiport_bwu_wavedata_list，匹配网卡句柄和网口丝印，计算数据
        for (bport_wave_node = g_busiport_bwu_wavedata_list; bport_wave_node; bport_wave_node = bport_wave_node->next) {
            bport_wave_info = (BPORT_BWU_WAVE_INFO *)bport_wave_node->data;

            // 根据silknum和card_handle匹配，匹配成功则将带宽使用率的值计算保存到网口信息链表
            if (object_handle == bport_wave_info->card_handle && silk_num == bport_wave_info->silk_num) {
                stat_caculate(bport_wave_info->stat_whole_life, (guint32)bandwidth_usage, cur_time);
                stat_caculate(bport_wave_info->stat_one_cycle, (guint32)bandwidth_usage, cur_time);

                
                if (silk_num <= card_wave_info->port_num) {
                    record_data[silk_num - 1] = stat_get_aver(bport_wave_info->stat_one_cycle);
                }
            }
        }
    }

    g_slist_free(busi_port_list);

    record_info.cur_time = cur_time;
    record_info.item_cnt = card_wave_info->port_num;
    record_info.record_data = record_data;

    // 将数据录入dat文件，并清除one cycle数据
    if (wave_record(card_wave_info->wave_handle, &record_info) == TRUE) {
        bport_bandwidth_usage_wave_stat_clear(object_handle);
    }

    g_mutex_unlock(&g_netcard_bwu_waveform_mutex);

    return RET_OK;
}


gint32 method_netcard_bandwidth_usage_wave_record_clear(const OBJ_HANDLE object_handle, GSList *caller_info,
    GSList *input_list, GSList **output_list)
{
    gint32 ret = RET_OK;
    NETCARD_BWU_WAVE_INFO *card_wave_info = NULL;
    gchar wave_title[PROP_VAL_MAX_LENGTH] = {0};

    ret = dal_get_property_value_string(object_handle, BUSY_NETCARD_BWU_WAVE_TITLE, wave_title, sizeof(wave_title));
    if (ret != RET_OK) {
        method_operation_log(caller_info, NULL, "Clear network bandwidth usage history record failed");
        debug_log(DLOG_ERROR, "%s:get wave_title fail", __FUNCTION__);
        return RET_ERR;
    }

    g_mutex_lock(&g_netcard_bwu_waveform_mutex);
    // 遍历g_netcard_bwu_waveforms_list，通过card_handle找到当前网卡的波形图信息
    ret = get_netcard_bandwidth_usage_wave_node(object_handle, &card_wave_info);
    if (ret != RET_OK || card_wave_info == NULL) {
        g_mutex_unlock(&g_netcard_bwu_waveform_mutex);
        method_operation_log(caller_info, NULL, "Clear %s network bandwidth usage history record failed", wave_title);
        debug_log(DLOG_ERROR, "%s:get card_wave_info failed.", __FUNCTION__);
        return RET_ERR;
    }

    ret = wave_clear(card_wave_info->wave_handle);
    g_mutex_unlock(&g_netcard_bwu_waveform_mutex);
    if (ret == RET_OK) {
        method_operation_log(caller_info, NULL, "Clear %s network bandwidth usage history record successfully",
            wave_title);
    } else {
        debug_log(DLOG_ERROR, "%s:wave_clear failed, ret %d.", __FUNCTION__, ret);
        method_operation_log(caller_info, NULL, "Clear %s network bandwidth usage history record failed", wave_title);
    }

    return ret;
}


LOCAL void network_bandwidth_usage_waveform_remove(OBJ_HANDLE card_handle)
{
    NET_BWU_WAVE_QUE_MSG *bwu_msg = NULL;
    gint32 ret = RET_ERR;

    if (card_handle == 0) {
        debug_log(DLOG_ERROR, "%s: input para card_handle error", __FUNCTION__);
        return;
    }

    
    // 判断消息队列是否为空
    if (g_net_bwu_wave_process_que == NULL) {
        debug_log(DLOG_ERROR, "%s:g_net_bwu_wave_process_que is NULL", __FUNCTION__);
        return;
    }

    

    bwu_msg = (NET_BWU_WAVE_QUE_MSG *)g_malloc0(sizeof(NET_BWU_WAVE_QUE_MSG));
    if (bwu_msg == NULL) {
        debug_log(DLOG_ERROR, "%s: g_malloc0 for bwu_msg failed", __FUNCTION__);
        return;
    }

    bwu_msg->que_type = BWU_WAVE_QUE_TYPE_DEL;
    bwu_msg->card_handle = card_handle; // 移除卡波形图对象需要指定card_handle

    ret = vos_queue_send(g_net_bwu_wave_process_que, (gpointer)bwu_msg);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: g_net_bwu_wave_process_que send fail, ret:%d", __FUNCTION__, ret);
        
        g_free(bwu_msg);
        
    } else {
        debug_log(DLOG_DEBUG, "%s: g_net_bwu_wave_process_que msg send ok", __FUNCTION__);
    }

    return;
}


LOCAL void network_bandwidth_usage_waveform_del(OBJ_HANDLE card_handle)
{
    GSList *wave_node = NULL;
    GSList *bport_node = NULL;
    NETCARD_BWU_WAVE_INFO *wave_info = NULL;
    BPORT_BWU_WAVE_INFO *bport_info = NULL;

    if (card_handle == 0) {
        debug_log(DLOG_INFO, "%s:input para error", __FUNCTION__);
        return;
    }

    g_mutex_lock(&g_netcard_bwu_waveform_mutex);
    // 先释放波形图的网口节点，由于网口可能有多个，找到匹配的节点释放后，还得重新找直到链表从头遍历完都找不到
    bport_node = g_busiport_bwu_wavedata_list;

    while (bport_node != NULL) {
        bport_info = (BPORT_BWU_WAVE_INFO *)bport_node->data;

        if (card_handle == bport_info->card_handle) {
            if (bport_info->stat_one_cycle != NULL) {
                g_free(bport_info->stat_one_cycle);
            }
            if (bport_info->stat_whole_life != NULL) {
                g_free(bport_info->stat_whole_life);
            }

            debug_log(DLOG_INFO, "%s:the port silknum(%d) waveform data of card(%s) is removed.", __FUNCTION__,
                bport_info->silk_num, dfl_get_object_name(card_handle));
            g_busiport_bwu_wavedata_list = g_slist_remove(g_busiport_bwu_wavedata_list, (gpointer)bport_info);
            g_free(bport_info);
            bport_node = g_busiport_bwu_wavedata_list;
            continue;
        }

        bport_node = g_slist_next(bport_node);
    }

    // 释放波形图网卡节点，只会有一个匹配，找到释放即退出
    for (wave_node = g_netcard_bwu_waveform_list; wave_node; wave_node = wave_node->next) {
        wave_info = (NETCARD_BWU_WAVE_INFO *)wave_node->data;

        if (card_handle == wave_info->card_handle) {
            wave_remove(wave_info->wave_handle);

            debug_log(DLOG_INFO, "%s:the card(%s) waveform is removed.", __FUNCTION__,
                dfl_get_object_name(card_handle));
            g_netcard_bwu_waveform_list = g_slist_remove(g_netcard_bwu_waveform_list, (gpointer)wave_info);
            g_free(wave_info);

            break;
        }
    }

    g_mutex_unlock(&g_netcard_bwu_waveform_mutex);

    return;
}


void network_bandwidth_usage_waveform_init(void)
{
    NET_BWU_WAVE_QUE_MSG *bwu_msg = NULL;
    gint32 ret = RET_ERR;

    
    // 判断消息队列是否为空
    if (g_net_bwu_wave_process_que == NULL) {
        debug_log(DLOG_ERROR, "%s:g_net_bwu_wave_process_que is NULL", __FUNCTION__);
        return;
    }

    

    bwu_msg = (NET_BWU_WAVE_QUE_MSG *)g_malloc0(sizeof(NET_BWU_WAVE_QUE_MSG));
    if (bwu_msg == NULL) {
        debug_log(DLOG_ERROR, "%s: g_malloc0 for bwu_msg failed", __FUNCTION__);
        return;
    }

    bwu_msg->que_type = BWU_WAVE_QUE_TYPE_ADD;
    bwu_msg->card_handle = 0; // 新增波形图不依赖card_handle

    ret = vos_queue_send(g_net_bwu_wave_process_que, (gpointer)bwu_msg);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: g_net_bwu_wave_process_que send fail, ret:%d", __FUNCTION__, ret);
        
        g_free(bwu_msg);
        
    } else {
        debug_log(DLOG_DEBUG, "%s: g_net_bwu_wave_process_que msg send ok", __FUNCTION__);
    }

    return;
}


LOCAL void network_bandwidth_usage_task(gpointer data)
{
    gint32 ret = RET_OK;
    NET_BWU_WAVE_QUE_MSG *bwu_msg = NULL;

    (void)prctl(PR_SET_NAME, (gulong) "BandwithUsage");

    while (1) {
        
        ret = vos_queue_receive(g_net_bwu_wave_process_que, (gpointer *)&bwu_msg, QUE_WAIT_FOREVER);
        if ((ret != RET_OK) || (bwu_msg == NULL)) {
            continue;
        }

        if (bwu_msg->que_type == BWU_WAVE_QUE_TYPE_ADD) {
            network_bandwidth_usage_waveform_add();
        } else if (bwu_msg->que_type == BWU_WAVE_QUE_TYPE_DEL) {
            network_bandwidth_usage_waveform_del(bwu_msg->card_handle);
        }

        if (bwu_msg != NULL) {
            g_free(bwu_msg);
        }
        bwu_msg = NULL;
    }
}


LOCAL gint32 network_bandwidth_usage_task_init(void)
{
    gint32 ret = RET_ERR;

    if (g_net_bwu_wave_process_task == 0) {
        ret = vos_task_create(&g_net_bwu_wave_process_task, "BWUWaveScan", (TASK_ENTRY)network_bandwidth_usage_task,
            (void *)NULL, DEFAULT_PRIORITY);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: create task fail", __FUNCTION__);
            return RET_ERR;
        }
    }

    return RET_OK;
}

gint32 net_card_set_eid(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret = 0;
    guint8 eid = 0;
    guint16 phy_addr = 0;
    guint8 is_support = 0;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input param null.", __FUNCTION__);
        return RET_ERR;
    }

    (void)dal_get_property_value_byte(object_handle, PROPERTY_NETCARD_MCTP_SUPPORT, &is_support);
    if (is_support == 0) {
        debug_log(DLOG_ERROR, "%s, bmc do not support net card %s mctp function currently.", __FUNCTION__,
            dfl_get_object_name(object_handle));
        return RET_OK;
    }
    eid = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    phy_addr = g_variant_get_uint16((GVariant *)g_slist_nth_data(input_list, 1));

    ret = dal_set_property_value_byte(object_handle, PROPERTY_NETCARD_EID, eid, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, Set net card eid failed.", __FUNCTION__);
        return RET_ERR;
    }

    ret = dal_set_property_value_uint16(object_handle, PROPERTY_NETCARD_PHY_ADDR, phy_addr, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, Set net card phy addr failed.", __FUNCTION__);
        return RET_ERR;
    }
    return RET_OK;
}


gint32 card_manage_set_lldp_enable(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = 0;
    guint8 lldp_flag = 0;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input param null.", __FUNCTION__);
        return RET_ERR;
    }

    lldp_flag = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    if (lldp_flag != 0 && lldp_flag != 1) {
        debug_log(DLOG_ERROR, "%s: lldp flag invalid, flag value is %d", __FUNCTION__, lldp_flag);
        return RET_ERR;
    }

    ret = dal_set_property_value_byte(object_handle, PROPERTY_NETCARD_LLDP_ENABLE, lldp_flag, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set netcard lldp flag failed, ret = %d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 card_manage_set_lldp_over_ncsi_enable(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret;
    guint8 lldp_over_ncsi;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s : input param null.", __FUNCTION__);
        return RET_ERR;
    }

    lldp_over_ncsi = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    if ((lldp_over_ncsi != DISABLE) && (lldp_over_ncsi != ENABLE)) {
        debug_log(DLOG_ERROR, "%s : lldp_over_ncsi[%d] is invalid", __FUNCTION__, lldp_over_ncsi);
        return RET_ERR;
    }

    ret = dal_set_property_value_byte(object_handle, PROPERTY_NETCARD_LLDPOVERNCSI_ENABLE, lldp_over_ncsi, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : Set LLDP over NCSI status failed, ret = %d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 card_manage_set_netcard_chip_temp(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    guint16 chip_temp;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input param null.", __FUNCTION__);
        return RET_ERR;
    }

    chip_temp = g_variant_get_uint16((GVariant *)g_slist_nth_data(input_list, 0));

    (void)dal_set_property_value_uint16(object_handle, PROPERTY_NETCARD_CHIP_TEMP, chip_temp, DF_NONE);

    return RET_OK;
}


gint32 card_manage_set_netcard_manu_id(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    guint32 manufacturer_id;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input param null.", __FUNCTION__);
        return RET_ERR;
    }
    
    manufacturer_id = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 0));

    (void)dal_set_property_value_uint32(object_handle, PROPERTY_NETCARD_MANU_ID, manufacturer_id, DF_NONE);

    return RET_OK;
}


LOCAL gint32 set_ocp_pre_plug_property(OBJ_HANDLE object_handle, GSList *caller_info, guint8 pre_plug_state)
{
    gint32 ret;
    guint8 bios_status = 0;

    (void)dal_get_func_ability(CLASS_NAME_BIOS, PROPERTY_BIOS_STARTUP_STATE, &bios_status);
    
    if ((dal_match_product_id(PRODUCT_ID_TIANCHI) == TRUE && pre_plug_state == 1) ||
        bios_status != BIOS_STARTUP_POST_STAGE_FINISH) {
        method_operation_log(caller_info, NULL, "Set OCP card HotPlugAttention state failed.");
        return RET_ERR;
    }
    ret = dal_set_property_value_byte(object_handle, PROPERTY_NETCARD_PREPLUG_STATE, 1, DF_AUTO);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, Set OCP card pre plug state failed.ret=%d", __FUNCTION__, ret);
        method_operation_log(caller_info, NULL, "Set OCP card HotPlugAttention state failed.");
        return RET_ERR;
    }
    vos_task_delay(DELAY_ONE_SECOND);
    ret = dal_set_property_value_byte(object_handle, PROPERTY_NETCARD_PREPLUG_STATE, 0, DF_AUTO);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, Set OCP card pre plug state failed.ret=%d", __FUNCTION__, ret);
        method_operation_log(caller_info, NULL, "Set OCP card HotPlugAttention state failed.");
        return RET_ERR;
    }
    method_operation_log(caller_info, NULL, "Set OCP card HotPlugAttention state successfully.");
    return RET_OK;
}


gint32 net_card_set_pre_plug(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    guint8 pre_plug_state;
    guint8 is_support = 0;
    OBJ_HANDLE product_handle;
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input param null.", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Set the OCP card to preinstalled state failed.");
        return RET_ERR;
    }
    ret = dal_get_object_handle_nth(CLASS_NAME_PME_PRODUCT, 0, &product_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get class handle of PRODUCT failed,ret=%d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    ret = dal_get_property_value_byte(product_handle, PROTERY_PRODUCT_SUPPORT_PCIE_HOTPLUG, &is_support);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get property value %s failed.", __FUNCTION__, PROTERY_PRODUCT_SUPPORT_PCIE_HOTPLUG);
        method_operation_log(caller_info, NULL, "Set OCP card SupportPcieHotPlug state failed.");
    }
    if (is_support == 0) {
        debug_log(DLOG_ERROR, "%s, server do not support pcie hotplug currently.", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Set OCP card HotPlugAttention state failed.");
        return RET_ERR;
    }
    (void)dal_get_property_value_byte(object_handle, PROPERTY_NETCARD_HOTPLUG_SUPPORT, &is_support);
    if (is_support == 0) {
        debug_log(DLOG_ERROR, "%s, OCP card %s do not support hotplug currently.", __FUNCTION__,
            dfl_get_object_name(object_handle));
        method_operation_log(caller_info, NULL, "Set OCP card HotPlugAttention state failed.");
        return RET_ERR;
    }
    pre_plug_state = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    if (pre_plug_state > 1) {
        return RET_ERR;
    }
    ret = set_ocp_pre_plug_property(object_handle, caller_info, pre_plug_state);
    return ret;
}

gint32 net_card_set_ncsi_package_id(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret;
    guint8 ncsi_package_id;
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input param null.", __FUNCTION__);
        return RET_ERR;
    }
    ncsi_package_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));

    ret = dal_set_property_value_byte(object_handle, PROPERTY_NETCARD_NCSI_PACKAGE_ID, ncsi_package_id, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, Set %s failed.ret=%d", __FUNCTION__, dfl_get_object_name(object_handle), ret);
        return RET_ERR;
    }
    return RET_OK;
}

void pcie_card_update_netcard_mac_address_by_smbus(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info, guint8 portnum)
{
    gint32 ret = RET_OK;
    guint8 *netcard_mac_adress = NULL;
    guint32 netcard_mac_address_total_len = 0;
    OBJ_HANDLE business_port_handle = 0;
    guint8 channal_index = 0;
    GSList *input_list = NULL;
    gchar mac_addr[NETCARD_MAC_ADDRESS_LENGTH] = {0};
    gint32 mac_i = 0;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s (%d): Input parameter pm_chip_info is NULL.", __FUNCTION__, __LINE__);
        return;
    }

    
    if (pcie_card_mgnt_ready() != TRUE && !is_ocp3_card(obj_handle)) {
        return;
    }
    ret = pm_get_netcard_mac_address(pm_chip_info, (guint8 **)&netcard_mac_adress, &netcard_mac_address_total_len);
    // 获取MAC地址
    if ((ret != RET_OK) || (netcard_mac_adress == NULL)) {
        debug_log(DLOG_INFO, "%s Get netcard MAC address failed, ret = 0x%4x", __FUNCTION__, ret);

        if (netcard_mac_adress != NULL) {
            g_free(netcard_mac_adress);
        }

        return;
    }

    // 校验通过SMBus获取的MAC地址长度参数，至少要有port_num个MAC地址
    if ((netcard_mac_address_total_len / NETCARD_MAC_ADDRESS_RESPONSE_LENGTH) < portnum) {
        debug_log(DLOG_ERROR, "invalid mac len 0x%4x, expect port num %d", netcard_mac_address_total_len, portnum);
        g_free(netcard_mac_adress);
        return;
    }

    // 获取MAC地址成功
    for (channal_index = 0; channal_index < portnum; channal_index++) {
        // 获取1822网卡各端口的句柄
        ret = pcie_card_get_business_port_handle_by_channel(obj_handle, channal_index, &business_port_handle);
        // 支持SMBus访问的PCIe网卡不一定配置有BusinessPort对象（如，SDI的扩展网卡），优化该场景日志打印
        if (ret == ERRCODE_OBJECT_NOT_EXIST) {
            continue;
        }

        if ((ret != RET_OK) || (business_port_handle == 0)) {
            debug_log(DLOG_ERROR, "%s get mac address handle by channel%d fail.", __FUNCTION__, channal_index);
            continue;
        }

        // 将mac地址拼接成为字符串
        (void)memset_s(mac_addr, NETCARD_MAC_ADDRESS_LENGTH, 0, NETCARD_MAC_ADDRESS_LENGTH);

        for (mac_i = 0; mac_i < MAC_ADDRESS_LENGTH; ++mac_i) {
            ret = snprintf_truncated_s((mac_addr + 3 * mac_i), (NETCARD_MAC_ADDRESS_LENGTH - 3 * mac_i),
                "%02x:", netcard_mac_adress[3 + mac_i + 9 * channal_index]);
            if (ret <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_truncated_s fail, ret = %d", __FUNCTION__, ret);
            }
        }

        mac_addr[NETCARD_MAC_ADDRESS_LENGTH - 1] = '\0';

        debug_log(DLOG_INFO, "%s i = %d: mac_addr = %s", __FUNCTION__, channal_index, mac_addr);

        input_list = g_slist_append(input_list, g_variant_new_string(mac_addr));
        ret = dfl_call_class_method(business_port_handle, BUSY_ETH_METHOD_SET_MAC, NULL, input_list, NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        input_list = NULL;
        business_port_handle = 0;
        if (ret != DFL_OK) {
            debug_log(DLOG_DEBUG, "%s:Set channel%d mac address fail, ret %d.", __FUNCTION__, channal_index, ret);
            continue;
        }
    }

    g_free(netcard_mac_adress);
}


void pcie_card_update_netcard_link_status_by_smbus(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info, guint8 portnum)
{
#define LINK_STATUS_STR_LENGTH 9
    gint32 ret = RET_OK;
    guint8 *netcard_link_status = NULL;
    guint32 netcard_link_status_len = 0;
    OBJ_HANDLE business_port_handle = 0;
    GSList *input_list = NULL;
    guint8 channal_index = 0;
    guint8 old_link_status = 0;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s: Input parameter pm_chip_info is NULL", __FUNCTION__);
        return;
    }
    
    if (pcie_card_mgnt_ready() != TRUE && !is_ocp3_card(obj_handle)) {
        return;
    }

    ret = pm_get_netcard_link_status(pm_chip_info, (guint8 **)&netcard_link_status, &netcard_link_status_len);
    if ((ret != RET_OK) || (netcard_link_status == NULL)) {
        debug_log(DLOG_INFO, "%s Get netcard link status failed, ret = 0x%4x", __FUNCTION__, ret);

        if (netcard_link_status != NULL) {
            g_free(netcard_link_status);
        }

        return;
    }

    // 获取link状态成功
    for (channal_index = 0; channal_index < netcard_link_status_len; channal_index++) {
        
        if (channal_index >= portnum) {
            debug_log(DLOG_ERROR, "%s: invalid buffer len %u", __FUNCTION__, netcard_link_status_len);
            break;
        }

        ret = pcie_card_get_business_port_handle_by_channel(obj_handle, channal_index, &business_port_handle);
        // 支持SMBus访问的PCIe网卡不一定配置有BusinessPort对象（如，SDI的扩展网卡），优化该场景日志打印
        if (ret == ERRCODE_OBJECT_NOT_EXIST) {
            continue;
        }

        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "get link status handle by channel fail.");
            continue;
        }

        // 优化代码逻辑，仅在当前link状态与预期不一致时才打印日志和调用方法设置link状态
        ret = dal_get_property_value_byte(business_port_handle, BUSY_ETH_ATTRIBUTE_LINK_STATUS, &old_link_status);
        if ((ret == RET_OK) && (old_link_status == netcard_link_status[channal_index])) {
            continue;
        }

        debug_log(DLOG_INFO, "%s link_status(smbus) changed from %d into %d", dfl_get_object_name(business_port_handle),
            old_link_status, netcard_link_status[channal_index]);

        // 设置LinkStatus属性值
        input_list = g_slist_append(input_list, g_variant_new_byte(netcard_link_status[channal_index]));
        ret = dfl_call_class_method(business_port_handle, BUSY_ETH_METHOD_SET_LINK_STATUS, NULL, input_list, NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        input_list = NULL;

        if (ret != DFL_OK) {
            debug_log(DLOG_DEBUG, "%s:Set channel%d link status fail, ret %d.", __FUNCTION__, channal_index, ret);
            continue;
        }
    }

    g_free(netcard_link_status);
}

void get_pcie_netcard_port_num(OBJ_HANDLE pcie_obj_handle, guint8 *portnum)
{
    gint32 ret;
    OBJ_HANDLE netcard_obj;

    ret = dal_get_specific_object_position(pcie_obj_handle, CLASS_NETCARD_NAME, &netcard_obj);
    if (ret != RET_OK) {
        *portnum = 0;
        return;
    }

    ret = dal_get_property_value_byte(netcard_obj, PROPERTY_NETCARD_PORTNUM, portnum);
    if (ret != RET_OK) {
        *portnum = 0;
    }

    return;
}


void sync_local_pcie_card_mac_addr_by_port(OBJ_HANDLE local_pcie_card, guint8 port_index)
{
    gint32 ret;
    OBJ_HANDLE hd_eth_group;
    gchar mac_addr[NETCARD_MAC_ADDRESS_LENGTH] = {0};
    gchar cur_mac_addr[NETCARD_MAC_ADDRESS_LENGTH] = {0};
    OBJ_HANDLE hd_port;
    GSList *input_list = NULL;

    ret = dal_get_specific_object_byte(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_GROUP_ID, port_index + 1,
        &hd_eth_group);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get EthGroup by GroupId=%d failed, ret=%d", __FUNCTION__, port_index + 1, ret);
        return;
    }

    ret = dal_get_specific_position_object_byte(local_pcie_card, ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_ATTRIBUTE_PORT_NUM,
        port_index, &hd_port);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get BusinessPort by PortNum=%d failed, ret=%d", __FUNCTION__, port_index, ret);
        return;
    }

    (void)dal_get_property_value_string(hd_eth_group, ETH_GROUP_ATTRIBUTE_MAC, mac_addr, sizeof(mac_addr));
    (void)dal_get_property_value_string(hd_port, BUSY_ETH_GROUP_ATTRIBUTE_MAC, cur_mac_addr, sizeof(cur_mac_addr));

    if (g_strcmp0(mac_addr, cur_mac_addr) == 0) {
        return;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(mac_addr));
    ret = dfl_call_class_method(hd_port, BUSY_ETH_METHOD_SET_MAC, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call method %s failed, ret=%d", __FUNCTION__, BUSY_ETH_METHOD_SET_MAC, ret);
    }
}

gint32 pcie_find_netcard_related_pcieaddrinfo_handle(OBJ_HANDLE netcard_handle, guint8 bdfno,
    OBJ_HANDLE *out_pcieaddrinfo_handle)
{
    gint32 ret = RET_OK;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE ref_component_handle = 0;
    guint8 com_groupid = 0;
    guint8 com_devicetype = 0;
    guint8 com_devicenum = 0;
    OBJ_HANDLE pcieaddr_handle = 0;
    guint8 paddr_groupid = 0;
    guint8 paddr_componenttype = 0;
    guint8 paddr_slotid = 0;
    guint8 paddr_bdfno = 0;

    if (netcard_handle == 0 || NULL == out_pcieaddrinfo_handle) {
        debug_log(DLOG_ERROR, "%s: Invalid parameter.", __FUNCTION__);
        return RET_ERR;
    }

    ret = dfl_get_referenced_object(netcard_handle, PROPERTY_NETCARD_REF_COMPONENT, &ref_component_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:%s get %s failed, ret:%d.", __FUNCTION__, dfl_get_object_name(netcard_handle),
            PROPERTY_NETCARD_REF_COMPONENT, ret);
        return ret;
    }

    ret = dal_get_property_value_byte(ref_component_handle, PROPERTY_COMPONENT_GROUPID, &com_groupid);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get %s failed, ret:%d.", __FUNCTION__, PROPERTY_COMPONENT_GROUPID, ret);
        return ret;
    }
    ret = dal_get_property_value_byte(ref_component_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &com_devicetype);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get %s failed, ret:%d.", __FUNCTION__, PROPERTY_COMPONENT_DEVICE_TYPE, ret);
        return ret;
    }
    ret = dal_get_property_value_byte(ref_component_handle, PROPERTY_COMPONENT_DEVICENUM, &com_devicenum);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get %s failed, ret:%d.", __FUNCTION__, PROPERTY_COMPONENT_DEVICENUM, ret);
        return ret;
    }

    ret = dfl_get_object_list(CLASS_PCIE_ADDR_INFO, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:get %s failed, ret:%d.", __FUNCTION__, CLASS_PCIE_ADDR_INFO, ret);
        return ret;
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        pcieaddr_handle = (OBJ_HANDLE)obj_node->data;

        ret = dal_get_property_value_byte(pcieaddr_handle, PROPERTY_GROUP_ID, &paddr_groupid);
        if (ret != RET_OK) {
            g_slist_free(obj_list);
            debug_log(DLOG_ERROR, "%s:get %s GroupId failed, ret = %d.", __FUNCTION__,
                dfl_get_object_name(pcieaddr_handle), ret);
            return ret;
        }
        ret = dal_get_property_value_byte(pcieaddr_handle, PROPERTY_COMPONENT_TYPE, &paddr_componenttype);
        if (ret != RET_OK) {
            g_slist_free(obj_list);
            debug_log(DLOG_ERROR, "%s:get %s ComponentType failed, ret = %d.", __FUNCTION__,
                dfl_get_object_name(pcieaddr_handle), ret);
            return ret;
        }
        ret = dal_get_property_value_byte(pcieaddr_handle, PROPERTY_PCIE_SLOT_ID, &paddr_slotid);
        if (ret != RET_OK) {
            g_slist_free(obj_list);
            debug_log(DLOG_ERROR, "%s:get %s SlotId failed, ret = %d.", __FUNCTION__,
                dfl_get_object_name(pcieaddr_handle), ret);
            return ret;
        }
        ret = dal_get_property_value_byte(pcieaddr_handle, PROPERTY_BDF_NO, &paddr_bdfno);
        if (ret != RET_OK) {
            g_slist_free(obj_list);
            debug_log(DLOG_ERROR, "%s:get %s BDFNo failed, ret = %d.", __FUNCTION__,
                dfl_get_object_name(pcieaddr_handle), ret);
            return ret;
        }

        if ((com_groupid == paddr_groupid) && (com_devicetype == paddr_componenttype) &&
            (com_devicenum == paddr_slotid) && (bdfno == paddr_bdfno)) {
            debug_log(DLOG_INFO, "NetCard:%s find related pcieaddrinfo handle:%s. ",
                dfl_get_object_name(netcard_handle), dfl_get_object_name(pcieaddr_handle));
            *out_pcieaddrinfo_handle = pcieaddr_handle;
            g_slist_free(obj_list);
            return RET_OK;
        }
    }

    g_slist_free(obj_list);
    return RET_ERR;
}

gint32 net_card_set_rootbdf(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    const gchar *bdf_str = NULL;
    gsize bdf_str_len;

    if (input_list == NULL || g_slist_length(input_list) != 1) {
        debug_log(DLOG_ERROR, "Invalid input parameter!");
        return RET_ERR;
    }

    bdf_str = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), &bdf_str_len);
    if (bdf_str == NULL) {
        debug_log(DLOG_ERROR, "%s Port:%s,set RootBDF failed,input=null", __FUNCTION__,
            dfl_get_object_name(object_handle));
        return RET_ERR;
    }
    debug_log(DLOG_INFO, "%s Port:%s,RootBDF:%s", __FUNCTION__, dfl_get_object_name(object_handle), bdf_str);
    ret = dal_set_property_value_string(object_handle, PROPERTY_NETCARD_ROOT_BDF, bdf_str, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set %s RootBDF property failed, ret=%d.", dfl_get_object_name(object_handle), ret);
        return RET_ERR;
    }

    return RET_OK;
}
