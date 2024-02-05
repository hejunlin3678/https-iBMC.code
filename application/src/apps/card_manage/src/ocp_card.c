

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


LOCAL gint32 get_ocp_card_bdf(guint8 cpu_num, guint8 secbus, guint8 subbus, guint8 *ocp_bus, guint8 *ocp_dev,
    guint8 *ocp_fun);
LOCAL void ocp_card_operation_log(guint8 type, guint8 slot_id, guint8 option);


LOCAL gint32 net_card_set_ocp_config(OBJ_HANDLE obj_handle, const gchar *getprop_name, const gchar *setprop_name)
{
    gint32 ret;
    guint32 thres_val = OCP_DEF_THES_TEMP;
    ret = dal_get_property_value_uint32(obj_handle, getprop_name, &thres_val);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get property %s failed.ret=%d", __FUNCTION__, getprop_name, ret);
        return RET_ERR;
    }
    ret = dal_set_property_value_uint32(obj_handle, setprop_name, thres_val, DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, set property %s failed.ret=%d", __FUNCTION__, setprop_name, ret);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL gint32 ocp_set_pcb_ver(OBJ_HANDLE object_handle)
{
    gint32 ret;
    OBJ_HANDLE com_handle = 0;
    OBJ_HANDLE fru_handle = 0;
    guint8 fru_id;

    ret = dfl_get_referenced_object(object_handle, PROPERTY_PCIE_CARD_REF_COMPONENT, &com_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get referenced obj %s failed. ret=%d", __FUNCTION__,
            PROPERTY_PCIE_CARD_REF_COMPONENT, ret);
        return ret;
    }
    ret = dal_get_property_value_byte(com_handle, PROPERTY_COMPONENT_FRUID, &fru_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get prop %s failed. ret=%d", __FUNCTION__, PROPERTY_COMPONENT_FRUID, ret);
        return ret;
    }
    ret = dal_get_specific_object_byte(CLASS_FRU, PROPERTY_FRU_ID, fru_id, &fru_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get obj by fruid %d failed. ret=%d", __FUNCTION__, fru_id, ret);
        return ret;
    }

    return dfl_call_class_method(fru_handle, METHOD_UPDATE_FRU_PCB_VERSION, NULL, NULL, NULL);
}

gint32 net_card_set_default_config(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    gint32 ret;
    OBJ_HANDLE ocp_config_handle;
    guint16 conf_val;
    guint8 ocp3_led_status;
    if (property_value == NULL) {
        return RET_ERR;
    }
    ocp3_led_status = g_variant_get_byte(property_value);
    if (ocp3_led_status != OCP_CARD_REMOVEABLE) {
        return RET_OK;
    }
    ret = ocp_set_pcb_ver(object_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, set ocp pcb version failed,ret=%d.", __FUNCTION__, ret);
    }
    ret = dfl_get_referenced_object(object_handle, PROPERTY_OCPCARD_REF_CONFIG, &ocp_config_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s, get OcpCardControl handle failed,ret=%d.", __FUNCTION__, ret);
        return RET_ERR;
    }
    ret = net_card_set_ocp_config(ocp_config_handle, PROPERTY_OCPCONTROL_THYS_CONVERT, PROPERTY_OCPCONTROL_THYS_REG);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    ret = net_card_set_ocp_config(ocp_config_handle, PROPERTY_OCPCONTROL_TOS_CONVERT, PROPERTY_OCPCONTROL_TOS_REG);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    
    ret = dal_get_property_value_uint16(ocp_config_handle, PROPERTY_OCPCONTROL_DEFAULT_CONFIG, &conf_val);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get property %s failed.ret=%d", __FUNCTION__, PROPERTY_OCPCONTROL_DEFAULT_CONFIG,
            ret);
        return RET_ERR;
    }
    ret = dal_set_property_value_uint16(ocp_config_handle, PROPERTY_OCPCONTROL_REG_CONFIG, conf_val, DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, set property %s failed.ret=%d", __FUNCTION__, PROPERTY_OCPCONTROL_REG_CONFIG, ret);
        return RET_ERR;
    }
    return RET_OK;
}

gboolean is_ocp3_card(OBJ_HANDLE card_handle)
{
    gint32 ret;
    OBJ_HANDLE com_handle = 0;
    OBJ_HANDLE fru_handle = 0;
    guint8 fru_id;
    guint8 fru_type;
    ret = dfl_get_referenced_object(card_handle, PROPERTY_PCIE_CARD_REF_COMPONENT, &com_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get referenced obj %s failed. ret=%d", __FUNCTION__,
            PROPERTY_PCIE_CARD_REF_COMPONENT, ret);
        return FALSE;
    }
    ret = dal_get_property_value_byte(com_handle, PROPERTY_COMPONENT_FRUID, &fru_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get prop %s failed. ret=%d", __FUNCTION__, PROPERTY_COMPONENT_FRUID, ret);
        return FALSE;
    }
    ret = dal_get_specific_object_byte(CLASS_FRU, PROPERTY_FRU_ID, fru_id, &fru_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get obj by fruid %d failed. ret=%d", __FUNCTION__, fru_id, ret);
        return FALSE;
    }
    ret = dal_get_property_value_byte(fru_handle, PROPERTY_FRU_TYPE, &fru_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get prop %s failed. ret=%d", __FUNCTION__, PROPERTY_FRU_TYPE, ret);
        return FALSE;
    }
    if (fru_type == FRU_TYPE_OCP3) {
        return TRUE;
    }
    return FALSE;
}
LOCAL void generate_ocp_bdf_request_info(INFO_NEED_GET_PCIE_INFO *info, guint8 cpu_num, guint32 length, guint8 bus,
    guint8 dev, guint8 function)
{
    info->cpu_num = cpu_num;
    info->is_local = 0;
    info->length = length;
    info->bus = bus;
    info->dev = dev;
    info->func = function;
    return;
}
LOCAL gint32 get_ocp_bdf_by_classnode(guint8 cpu_num, guint8 bus, guint8 dev, guint8 *ocp_bus, guint8 *ocp_dev,
    guint8 *ocp_fun)
{
    gint32 ret;

    for (guint8 function = 0; function < PCEI_FUN_MAX_NUM; function++) {
        guint8 response[ME_CMD_RESPONSE_LEN] = {0};
        INFO_NEED_GET_PCIE_INFO info;

        (void)memset_s(response, sizeof(response), 0, sizeof(response));
        generate_ocp_bdf_request_info(&info, cpu_num, sizeof(response), bus, dev, function);
        info.address = ME_CMD_GET_CLASS_CODE;
        ret = pcie_get_info_from_me(&info, response);
        debug_log(DLOG_DEBUG, "Get %x:%x:%x class code is %d, ret = %d ", info.bus, info.dev, info.func,
            response[ME_CMD_GET_CLASS_CODE_RSP_OFFSET], ret);
        if (ret != RET_OK) {
            return ME_REPONSE_ERR;
        }

        guint8 base_class_code = response[ME_CMD_GET_CLASS_CODE_RSP_OFFSET];
        if (base_class_code == ME_CMD_CLASS_CODE_NETCARD) { // 网卡设备
            *ocp_bus = bus;
            *ocp_dev = dev;
            *ocp_fun = function;
            return RET_OK;
        } else if (base_class_code == ME_CMD_CLASS_CODE_BRIDGE) { // 桥接设备
            (void)memset_s(response, sizeof(response), 0, sizeof(response));
            info.address = ME_CMD_GET_SECBUS_SUBUS;
            ret = pcie_get_info_from_me(&info, response);
            if (ret != RET_OK) {
                return ME_REPONSE_ERR;
            }
            guint8 me_secbus = response[ME_CMD_GET_SECBUS_RSP_OFFSET];
            guint8 me_subbus = response[ME_CMD_GET_SUBBUS_RSP_OFFSET];
            if (me_secbus == 0 || me_secbus < info.bus || me_subbus < me_secbus) {
                break;
            }
            return get_ocp_card_bdf(cpu_num, me_secbus, me_subbus, ocp_bus, ocp_dev, ocp_fun);
        }
    }

    return RET_ERR;
}


LOCAL gint32 get_ocp_card_bdf(guint8 cpu_num, guint8 secbus, guint8 subbus, guint8 *ocp_bus, guint8 *ocp_dev,
    guint8 *ocp_fun)
{
    gint32 ret;
    guint8 bus;
    guint8 dev;

    if (cpu_num == INVALID_UINT8 || secbus == INVALID_UINT8 || subbus == INVALID_UINT8 || ocp_bus == NULL ||
        ocp_dev == NULL || ocp_fun == NULL) {
        debug_log(DLOG_DEBUG, "Internal error, parameter is error");
        return RET_ERR;
    }
    for (bus = secbus; bus <= subbus; bus++) {
        for (dev = 0; dev < PCEI_DEV_MAX_NUM; dev++) {
            ret = get_ocp_bdf_by_classnode(cpu_num, bus, dev, ocp_bus, ocp_dev, ocp_fun);
            if (ret == ME_REPONSE_ERR) {
                break;
            } else if (ret == RET_OK) {
                return RET_OK;
            }
        }
    }

    return RET_ERR;
}


gint32 ocp_card_update_root_port_bdf(guint8 pcie_type, guint8 pcie_slot, guint8 *ocp_bus, guint8 *ocp_dev,
    guint8 *ocp_fun)
{
    gint32 ret = RET_ERR;
    OBJ_HANDLE pcie_addr_handle = 0;
    INFO_NEED_GET_PCIE_INFO info;
    guint8 response[ME_CMD_RESPONSE_LEN] = {0};

    if (pcie_type == OCPCARD_CONNECTOR) {
        ret = get_pcieaddr_handle_by_type_slot(COMPONENT_TYPE_OCP, pcie_slot, &pcie_addr_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "Get PcieAddrInfo failed, ret = %d", ret);
            return RET_ERR;
        }

        ret = get_bdf_property_value(pcie_addr_handle, &info);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "Get %s property value failed, ret = %d", dfl_get_object_name(pcie_addr_handle), ret);
            return RET_ERR;
        }

        info.is_local = 0;
        info.address = ME_CMD_GET_SECBUS_SUBUS;
        info.length = (guint8)sizeof(response);

        ret = pcie_get_info_from_me(&info, response);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "Me get pcie addr info faild, ret = %d", ret);
            return RET_ERR;
        }
        guint8 me_secbus = response[ME_CMD_GET_SECBUS_RSP_OFFSET];
        guint8 me_subbus = response[ME_CMD_GET_SUBBUS_RSP_OFFSET];

        ret = set_pcie_addr_info(pcie_addr_handle, info.bus, me_secbus, me_subbus);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "Set pcie addr info faild, ret = %d", ret);
            return RET_ERR;
        }
        ret = get_ocp_card_bdf(info.cpu_num, me_secbus, me_subbus, ocp_bus, ocp_dev, ocp_fun);
        debug_log(DLOG_DEBUG, "Ocp bdf=%02x:%02x:%x, ret = %d", *ocp_bus, *ocp_dev, *ocp_fun, ret);
        ;
    }

    return ret;
}

gint32 ocp_card_update_info(OBJ_HANDLE obj_handle, gpointer user_data)
{
    gint32 ret;
    guint8 ocp_state = 0;
    OBJ_HANDLE netcard_obj = 0;

    ret = dal_get_specific_object_position(obj_handle, CLASS_NETCARD_NAME, &netcard_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "Get %s netcard object handle failed, ret = %d", dfl_get_object_name(obj_handle), ret);
        
        return RET_OK;
    }

    ret = dal_get_property_value_byte(netcard_obj, PROPERTY_NETCARD_HOTPLUG_CONTROL, &ocp_state);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "Get %s property HotplugCtrlStatus failed, ret = %d", dfl_get_object_name(netcard_obj),
            ret);
        
        return RET_OK;
    }

    if (ocp_state == OCP_CARD_REMOVEABLE) {
        return pcie_card_update_info(obj_handle, user_data);
    }

    return RET_OK;
}
static void ocp_card_init_obj(OBJ_HANDLE obj_handle, guint8 slot_id)
{
    
    (void)dal_set_property_value_uint16(obj_handle, PROPERTY_PCIECARD_DID, g_ocp_group.info[slot_id - 1].device_id,
        DF_NONE);
    (void)dal_set_property_value_uint16(obj_handle, PROPERTY_PCIECARD_VID, g_ocp_group.info[slot_id - 1].vender_id,
        DF_NONE);

    
    (void)dal_set_property_value_uint16(obj_handle, PROPERTY_PCIECARD_SUBDID,
        g_ocp_group_sub_info.sub_info[slot_id - 1].sub_device_id, DF_NONE);
    (void)dal_set_property_value_uint16(obj_handle, PROPERTY_PCIECARD_SUBVID,
        g_ocp_group_sub_info.sub_info[slot_id - 1].sub_vender_id, DF_NONE);

    
    (void)dal_set_property_value_byte(obj_handle, PROPERTY_PCIECARD_BUS_NUM,
        g_ocp_bdf_info.bdf_info[slot_id - 1].bus_num, DF_NONE);
    (void)dal_set_property_value_byte(obj_handle, PROPERTY_PCIECARD_DEV_NUM,
        g_ocp_bdf_info.bdf_info[slot_id - 1].device_num, DF_NONE);
    (void)dal_set_property_value_byte(obj_handle, PROPERTY_PCIECARD_FUN_NUM,
        g_ocp_bdf_info.bdf_info[slot_id - 1].function_num, DF_NONE);

    return;
}


gint32 ocp_card_add_object_callback(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    guint8 slot_id = 0;
    gchar card_desc[MAX_INFO_LEN] = {0};
    gchar manufacturer[MAX_INFO_LEN] = {0};
    guint8 pciids_support = 1;
    gchar referencd_object[MAX_NAME_SIZE] = {0};
    gchar referencd_property[MAX_NAME_SIZE] = {0};

    ret = dal_get_property_value_byte(obj_handle, PROPETRY_PCIECARD_SLOT, &slot_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get ocp card slot id failed!");
        return RET_ERR;
    }

    if (slot_id <= 0 || slot_id > MAX_PCIE_CARD) {
        return RET_ERR;
    }

    debug_log(DLOG_ERROR, "Add ocp card%d object", slot_id);

    ocp_card_init_obj(obj_handle, slot_id);

    recover_saved_value(obj_handle, PROPERTY_PCIECARD_DECREASE_BANDWIDTH, DF_SAVE_TEMPORARY);
    ocp_card_update_health_info(obj_handle);
    pcie_card_update_version_info(obj_handle);
    pcie_card_update_resId_info(obj_handle, slot_id);

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_PCIECARD_SUPPORT_PCIIDS, &pciids_support);
    if (pciids_support) {
        ret = pcie_card_lookup_name(g_ocp_group.info[slot_id - 1].vender_id, g_ocp_group.info[slot_id - 1].device_id,
            g_ocp_group_sub_info.sub_info[slot_id - 1].sub_vender_id,
            g_ocp_group_sub_info.sub_info[slot_id - 1].sub_device_id, manufacturer, sizeof(manufacturer), card_desc,
            sizeof(card_desc));
        if (ret == RET_OK) {
            
            if (g_ocp_group_sub_info.sub_info[slot_id - 1].sub_vender_id == PCIE_HUAWEI_SUBVID) {
                strcpy_s(manufacturer, sizeof(manufacturer), PCIE_HUAWEI_MANU);
            }

            if (dfl_get_referenced_property(obj_handle, PROPETRY_PCIECARD_MANUFACTURER, referencd_object,
                referencd_property, MAX_NAME_SIZE, MAX_NAME_SIZE) != DFL_OK) {
                (void)dal_set_property_value_string(obj_handle, PROPETRY_PCIECARD_MANUFACTURER, manufacturer, DF_NONE);
            }

            (void)dal_set_property_value_string(obj_handle, PROPERTY_PCIECARD_DESC, card_desc, DF_NONE);
        }
    }
    (void)pcie_card_change_notify_mctp();

    info_pcie_card_change();

    return RET_OK;
}

gint32 ocp_card_del_object_callback(OBJ_HANDLE object_handle)
{
    gint32 ret;
    guint8 slot_id = 0;

    ret = dal_get_property_value_byte(object_handle, PROPETRY_PCIECARD_SLOT, &slot_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get ocp card slot id failed!");
        return RET_ERR;
    }

    if (slot_id <= 0 || slot_id > MAX_PCIE_CARD) {
        return RET_ERR;
    }

    debug_log(DLOG_ERROR, "Delete ocp card%d object", slot_id);

    g_ocp_group.info[slot_id - 1].device_id = PCIE_INVALID_DID;
    g_ocp_group.info[slot_id - 1].vender_id = PCIE_INVALID_VID;
    g_ocp_group_sub_info.sub_info[slot_id - 1].sub_device_id = PCIE_INVALID_SUBDID;
    g_ocp_group_sub_info.sub_info[slot_id - 1].sub_vender_id = PCIE_INVALID_SUBVID;

    per_save((guint8 *)&g_ocp_group);
    per_save((guint8 *)&g_ocp_group_sub_info);

    (void)pcie_card_change_notify_mctp();
    return RET_OK;
}

void ocp_card_set_device_status_default_value(void)
{
    gint32 ret;

    ret = dfl_foreach_object(CLASS_OCP_CARD, ocp_card_recover_device_status, NULL, NULL);
    if (ret != DFL_OK && ret != ERRCODE_OBJECT_NOT_EXIST) {
        debug_log(DLOG_ERROR, "Recover ocp card default value failed!");
    }

    return;
}


gint32 ocp_card_ipmi_set_vid_did_subvid_subdid_info(const void* msg_data, gpointer user_data)
{
#define ONE_OCP_CARD_INFO_SIZE 8
#define TASK_DELAY_PCIE_RELOAD_INTERVAL   3000
    OCP_CARD_IPMI_INFO_S ocp_card_data;
    const guint8 *req_data_buf = NULL;
    
    if (msg_data == NULL) {
        return VOS_ERR;
    }
    if (filter_msg_by_sys_channel(msg_data)) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_STATUS_INVALID);
    }
    req_data_buf = get_ipmi_src_data(msg_data);
    if (req_data_buf == NULL) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }
    (void)memset_s(&ocp_card_data, sizeof(OCP_CARD_IPMI_INFO_S), 0, sizeof(OCP_CARD_IPMI_INFO_S));
    ocp_card_data.ocp_card_info.count = *(req_data_buf + 1);
    debug_log(DLOG_ERROR, "Ocp card number: %d", ocp_card_data.ocp_card_info.count);
    
    if (ocp_card_data.ocp_card_info.count > OCPCARD_CONNECTOR) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_PARA_NOT_SUPPORT);
    }
    
    if (ocp_card_data.ocp_card_info.count * ONE_OCP_CARD_INFO_SIZE + 1 >= get_ipmi_src_data_len(msg_data)) {
        debug_log(DLOG_ERROR, "%s: ocp_card_info count %d size > datalen %d", __FUNCTION__,
            ocp_card_data.ocp_card_info.count, get_ipmi_src_data_len(msg_data));
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }
    
    ipmi_send_simple_response(msg_data, COMP_CODE_SUCCESS);
    
    ocp_card_ipmi_get_pcie_vid_did_subvid_subdid(&ocp_card_data, req_data_buf);
    
    ocp_card_xml_unload_for_bios(&ocp_card_data);
    vos_task_delay(TASK_DELAY_PCIE_RELOAD_INTERVAL);
    
    ocp_card_xml_load_for_bios(&ocp_card_data);
    ocp_card_info_per_save(&ocp_card_data);
    return RET_OK;
}

void ocp_card_ipmi_get_pcie_vid_did_subvid_subdid(OCP_CARD_IPMI_INFO_S *ocp_card_data, const guint8 *req_data_buf)
{
    guint8 i;
    guint8 slot_id;
    for (slot_id = 0, i = 0; slot_id < ocp_card_data->ocp_card_info.count; slot_id++, i++) {
        
        ocp_card_data->ocp_card_info.info[i].vender_id = MAKE_WORD(*(req_data_buf + 3 + slot_id * 8),
            *(req_data_buf + 2 + slot_id * 8)); // 小端在前 vid data2-3 Subdid data8-9
        
        ocp_card_data->ocp_card_info.info[i].device_id = MAKE_WORD(*(req_data_buf + 5 + slot_id * 8),
            *(req_data_buf + 4 + slot_id * 8)); // 小端在前 did data4-5 Subdid data8-9
        
        ocp_card_data->ocp_card_sub_info.sub_info[i].sub_vender_id =  MAKE_WORD(*(req_data_buf + 7 + slot_id * 8),
            *(req_data_buf + 6 + slot_id * 8)); // 小端在前 Subvid data6-7 Subdid data8-9
        
        ocp_card_data->ocp_card_sub_info.sub_info[i].sub_device_id =  MAKE_WORD(*(req_data_buf + 9 + slot_id * 8),
            *(req_data_buf + 8 + slot_id * 8)); // 小端在前 Subdid data8-9
        debug_log(DLOG_ERROR, "Ocp card%d, Vender id: %4x, Device id: %4x, Sub Vender id: %4x, Sub Device id: %4x",
            i + 1,
            ocp_card_data->ocp_card_info.info[i].vender_id,
            ocp_card_data->ocp_card_info.info[i].device_id,
            ocp_card_data->ocp_card_sub_info.sub_info[i].sub_vender_id,
            ocp_card_data->ocp_card_sub_info.sub_info[i].sub_device_id);
    }
    ocp_card_data->slot_id = i;
}


LOCAL void ocp_card_operation_log(guint8 type, guint8 slot_id, guint8 option)
{
    GSList* caller_info_list = NULL;

    caller_info_list = g_slist_append(caller_info_list, g_variant_new_string("N/A"));
    caller_info_list = g_slist_append(caller_info_list, g_variant_new_string("N/A"));
    caller_info_list = g_slist_append(caller_info_list, g_variant_new_string("127.0.0.1"));

    method_operation_log(caller_info_list, NULL,
        "%s card%d(vid: 0x%04x, did: 0x%04x, sub_vid: 0x%04x, sub_did: 0x%04x) %s successfully",
        (OCPCARD_CONNECTOR == type) ? "OCP" : "PCIe", slot_id + 1,
        g_ocp_group.info[slot_id].vender_id, g_ocp_group.info[slot_id].device_id,
        g_ocp_group_sub_info.sub_info[slot_id].sub_vender_id,
        g_ocp_group_sub_info.sub_info[slot_id].sub_device_id,
        (PCIE_CARD_XML_LOAD == option) ? "plug in" : "plug out");
    g_slist_free_full(caller_info_list, (GDestroyNotify)g_variant_unref);
    return;
}


void ocp_card_xml_unload_for_bios(OCP_CARD_IPMI_INFO_S *ocp_card_data)
{
    guint8 slot_id;
    guint8 ret;
    guint8 i = ocp_card_data->slot_id;
    volatile guint32 old_boardid;
    volatile guint32 new_boardid;
    volatile guint32 old_auxid;
    OBJ_HANDLE obj_handle = 0;
    
    for (slot_id = 0; slot_id < i; slot_id++) {
        old_boardid = g_ocp_group.info[slot_id].vender_id * 0x10000 + g_ocp_group.info[slot_id].device_id;
        old_auxid = g_ocp_group_sub_info.sub_info[slot_id].sub_vender_id * 0x10000 +
            g_ocp_group_sub_info.sub_info[slot_id].sub_device_id;
        new_boardid = ocp_card_data->ocp_card_info.info[slot_id].vender_id * 0x10000 +
            ocp_card_data->ocp_card_info.info[slot_id].device_id;
        if (old_boardid == new_boardid) {
            continue;
        }
        if (old_boardid != PCIE_NO_CARD && old_auxid != PCIE_NO_CARD) {
            debug_log(DLOG_ERROR, "OCP PullOut Slot %d, VID=%04x DID=%04x SubVID=%04x SubDID=%04x",
                (slot_id + 1),
                g_ocp_group.info[slot_id].vender_id, g_ocp_group.info[slot_id].device_id,
                g_ocp_group_sub_info.sub_info[slot_id].sub_vender_id,
                g_ocp_group_sub_info.sub_info[slot_id].sub_device_id);
            
            ret = pcie_card_get_connector_handle(slot_id + 1, &obj_handle, OCPCARD_CONNECTOR);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "Get ocp card%d connector handle fail!", slot_id + 1);
                continue;
            }
            
            (void)pcie_card_load_xml(obj_handle, old_boardid, old_auxid, PCIE_CARD_XML_UNLOAD,
                PCIE_AND_PCIEPLUS_CONNECTOR);
            ocp_card_operation_log(PCIE_AND_PCIEPLUS_CONNECTOR, slot_id, PCIE_CARD_XML_UNLOAD);
        }
    }
}

void ocp_card_xml_load_for_bios(OCP_CARD_IPMI_INFO_S *ocp_card_data)
{
    guint8 slot_id;
    guint8 ret;
    guint8 i = ocp_card_data->slot_id;
    volatile guint32 old_boardid;
    volatile guint32 new_boardid;
    volatile guint32 new_auxid;
    OBJ_HANDLE obj_handle = 0;
    for (slot_id = 0; slot_id < i; slot_id++) {
        old_boardid = g_ocp_group.info[slot_id].vender_id * 0x10000 + g_ocp_group.info[slot_id].device_id;
        new_boardid = ocp_card_data->ocp_card_info.info[slot_id].vender_id * 0x10000 +
            ocp_card_data->ocp_card_info.info[slot_id].device_id;
        new_auxid = ocp_card_data->ocp_card_sub_info.sub_info[slot_id].sub_vender_id * 0x10000 +
            ocp_card_data->ocp_card_sub_info.sub_info[slot_id].sub_device_id;
        if (old_boardid == new_boardid) {
            continue;
        }
        if (new_boardid != PCIE_NO_CARD && new_auxid != PCIE_NO_CARD) {
            debug_log(DLOG_ERROR, "OCP PullIn Slot %d, VID=%04x DID=%04x SubVID=%04x SubDID=%04x",
                (slot_id + 1),
                ocp_card_data->ocp_card_info.info[slot_id].vender_id,
                ocp_card_data->ocp_card_info.info[slot_id].device_id,
                ocp_card_data->ocp_card_sub_info.sub_info[slot_id].sub_vender_id,
                ocp_card_data->ocp_card_sub_info.sub_info[slot_id].sub_device_id);
            
            ret = pcie_card_get_connector_handle(slot_id + 1, &obj_handle, OCPCARD_CONNECTOR);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "Get ocp card%d connector handle fail!", slot_id);
                continue;
            }
            g_ocp_group.info[slot_id].vender_id = ocp_card_data->ocp_card_info.info[slot_id].vender_id;
            g_ocp_group.info[slot_id].device_id = ocp_card_data->ocp_card_info.info[slot_id].device_id;
            guint16 tmp_sub_vender_id = ocp_card_data->ocp_card_sub_info.sub_info[slot_id].sub_vender_id;
            guint16 tmp_sub_device_id = ocp_card_data->ocp_card_sub_info.sub_info[slot_id].sub_device_id;
            g_ocp_group_sub_info.sub_info[slot_id].sub_vender_id = tmp_sub_vender_id;
            g_ocp_group_sub_info.sub_info[slot_id].sub_device_id = tmp_sub_device_id;
            
            (void)pcie_card_load_xml(obj_handle, new_boardid, new_auxid, PCIE_CARD_XML_LOAD,
                OCPCARD_CONNECTOR);
            ocp_card_operation_log(OCPCARD_CONNECTOR, slot_id, PCIE_CARD_XML_LOAD);
        }
    }
}

void ocp_card_info_per_save(OCP_CARD_IPMI_INFO_S *ocp_card_data)
{
    memcpy_s(&g_ocp_group, sizeof(g_ocp_group), &ocp_card_data->ocp_card_info, sizeof(PCIE_CARD_GROUP_INFO_S));
    per_save((guint8*)&g_ocp_group);
    memcpy_s(&g_ocp_group_sub_info, sizeof(g_ocp_group_sub_info), &ocp_card_data->ocp_card_sub_info,
        sizeof(PCIE_CARD_GROUP_SUB_INFO_S));
    per_save((guint8*)&g_ocp_group_sub_info);
}