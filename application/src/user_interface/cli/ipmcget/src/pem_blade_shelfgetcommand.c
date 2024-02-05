

#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "pme_app/pme_app.h"
#include "pme_app/uip/uip_network_config.h"
#include "uip.h"
#include "ipmctypedef.h"
#include "shelfgetcommand.h"
#include "ipmcgetcommand.h"
#include "ipmc_shelf_common.h"
#include "ipmcgethelp.h"
#include "ipmc_public.h"
#include "mscm_macro.h"
#include "smm_sol_client.h"
#include "ipmcget_netconfig.h"
#include "ipmcget_user_security.h"

LOCAL gint32 foreach_pem_presence(OBJ_HANDLE object_handle, gpointer user_data);
LOCAL gint32 foreach_pem_version(OBJ_HANDLE object_handle, gpointer user_data);
LOCAL gint32 print_blade_bmc_user_info(const gchar *username, const gchar *ip, gchar *blade_name, guint8 slave_addr,
    guint8 user_id);
LOCAL gint32 ipmi_get_blade_version(guint8 index, guint8 slot_id, gchar *version, guint32 ver_buf_len,
    guint8 *msg_total_count);


LOCAL gint32 foreach_pem_presence(OBJ_HANDLE object_handle, gpointer user_data)
{
    guchar presence = 0;
    guchar slotid = 0;
    gint32 ret;

    ret = dal_get_property_value_byte(object_handle, PROTERY_IPMBETH_BLADE_PRESENCE, &presence);
    if (ret != RET_OK) {
        g_printf("Get property %s failed.\r\n", PROTERY_IPMBETH_BLADE_PRESENCE);
        return DFL_ERR;
    }
    ret = dal_get_property_value_byte(object_handle, PROPERTY_POWER_MGNT_SLOT, &slotid);
    if (ret != RET_OK) {
        g_printf("Get property %s failed.\r\n", PROPERTY_POWER_MGNT_SLOT);
        return DFL_ERR;
    }
    if (presence == SHELF_PART_PRESENCE) {
        g_printf("pem%d\r\n", slotid + 1);
    }
    return DFL_OK;
}


LOCAL gint32 foreach_pem_version(OBJ_HANDLE object_handle, gpointer user_data)
{
    gint32 ret;
    guint8 slot = 0;
    gchar hardware_version_buf[VERSION_LEN] = {0};
    gchar software_version_buf[VERSION_LEN] = {0};

    
    ret = dal_get_property_value_byte(object_handle, PROTERY_PS_INDEX, &slot);
    if (ret != RET_OK) {
        g_printf("Pem%d Hardware Version:\r\n", slot + 1);
        g_printf("Pem%d Software Version:\r\n", slot + 1);

        return RET_OK;
    }

    ret = dal_get_property_value_string(object_handle, PROTERY_PS_VERSION, hardware_version_buf,
        sizeof(hardware_version_buf));
    if (ret != RET_OK) {
        (void)memset_s(hardware_version_buf, VERSION_LEN, 0, VERSION_LEN);
    }
    ret = dal_get_property_value_string(object_handle, PROTERY_PS_FWVERSION, software_version_buf,
        sizeof(software_version_buf));
    if (ret != RET_OK) {
        (void)memset_s(software_version_buf, VERSION_LEN, 0, VERSION_LEN);
    }

    g_printf("Pem%d Hardware Version:%s\r\n", slot + 1, hardware_version_buf);
    g_printf("Pem%d Software Version:%s\r\n", slot + 1, software_version_buf);

    return RET_OK;
}

LOCAL gint32 print_blade_bmc_user_info(const gchar *username, const gchar *ip, gchar *blade_name, guint8 slave_addr,
    guint8 user_id)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *user_name = NULL;
    gboolean callin_flag;
    gboolean link_auth_flag;
    gboolean ipmi_msg_flag;
    guint privilege;
    gsize len = 0;
    gchar name[20] = { 0 };

    input_list = g_slist_append(input_list, g_variant_new_byte(slave_addr));
    input_list = g_slist_append(input_list, g_variant_new_byte(user_id));

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_NAME_IPMBETH_BLADE,
        blade_name, METHOD_GET_BLADE_BMC_USERNAME, input_list, &output_list);
    if (ret != RET_OK) {
        print_blade_err_info(blade_name, ret);
        g_printf("Get %s user%d user name failed.\r\n", blade_name, user_id);
        uip_free_gvariant_list(input_list);
        uip_free_gvariant_list(output_list);
        return FALSE;
    }

    user_name = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), &len);
    if (len != 0) {
        strncpy_s(name, sizeof(name), user_name, sizeof(name) - 1);
    }
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
    input_list = NULL;
    output_list = NULL;

    input_list = g_slist_append(input_list, g_variant_new_byte(slave_addr));
    input_list = g_slist_append(input_list, g_variant_new_byte(user_id));

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_NAME_IPMBETH_BLADE,
        blade_name, METHOD_GET_BLADE_BMC_USERPRIVILEGE, input_list, &output_list);
    if (ret != RET_OK) {
        print_blade_err_info(blade_name, ret);
        g_printf("Get %s user%d privilege failed.\r\n", blade_name, user_id);
        uip_free_gvariant_list(input_list);
        uip_free_gvariant_list(output_list);
        return FALSE;
    }

    callin_flag = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 2));
    link_auth_flag = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 3));
    ipmi_msg_flag = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 4));
    privilege = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 5));

    g_printf("%-8d%-18s%-15s%-15s%-15s%-18s\r\n", user_id, name, (callin_flag == FALSE) ? "FALSE" : "TRUE",
        (link_auth_flag == FALSE) ? "FALSE" : "TRUE", (ipmi_msg_flag == FALSE) ? "FALSE" : "TRUE",
        val_to_str(privilege, ipmc_privlvl_vals));

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    return RET_OK;
}


LOCAL gint32 ipmi_get_blade_version(guint8 index, guint8 slot_id, gchar *version, guint32 ver_buf_len,
    guint8 *msg_total_count)
{
    errno_t safe_fun_ret = EOK;
    IPMI_REQ_MSG_HEAD_S req_msg_head;
    guint8 reqdata[128] = {0};
    guint32 manu_id = 0;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    gpointer response = NULL;
    const guint8 *resp_src_data = NULL;
    guint8 src_lenth = 0;

    (void)memset_s(version, ver_buf_len, 0, ver_buf_len);
    
    (void)dfl_get_object_handle(BMC_OBJECT_NAME, &obj_handle);
    (void)dal_get_property_value_uint32(obj_handle, BMC_MANU_ID_NAME, &manu_id);

    req_msg_head.target_type = (TARGET_TYPE_E)0x08;
    req_msg_head.target_instance = slot_id;
    req_msg_head.netfn = NETFN_OEM_REQ;
    req_msg_head.cmd = 0x15;
    req_msg_head.lun = 0;
    req_msg_head.src_len = 4;
    reqdata[0] = LONGB0(manu_id);
    reqdata[1] = LONGB1(manu_id);
    reqdata[2] = LONGB2(manu_id);
    reqdata[3] = index;

    ret = ipmi_send_request(&req_msg_head, reqdata, MAX_POSSIBLE_IPMI_FRAME_LEN, &response, TRUE);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    resp_src_data = get_ipmi_src_data(response);
    if (resp_src_data == NULL) {
        
        g_free(response);
        return RET_ERR;
    }

    if ((resp_src_data[0] == COMP_CODE_SUCCESS) && (resp_src_data[1] == LONGB0(manu_id)) &&
        (resp_src_data[2] == LONGB1(manu_id)) && (resp_src_data[3] == LONGB2(manu_id))) {
        *msg_total_count = resp_src_data[4];
        src_lenth = get_ipmi_src_data_len(response);
        if ((index == resp_src_data[5]) && (src_lenth > 6) && (ver_buf_len > 1)) {
            safe_fun_ret = memcpy_s(version, ver_buf_len - 1, &resp_src_data[6], src_lenth - 6);
            g_free(response);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                return RET_ERR;
            }
            return RET_OK;
        }
    }

    g_free(response);
    return RET_ERR;
}
gchar ipmi_get_deviceid(gchar *location)
{
    gpointer response = NULL;
    gint32 result;
    const guchar *resp_src_data = NULL;
    IPMI_REQ_MSG_HEAD_S req_msg_head;
    gchar completion_code;
    guint8 slot_id = 0;

    
    if (!strcmp(location, SHELF_MGMT_DEFAULT_NAME)) {
        location = "SMM";
        req_msg_head.target_type = IPMI_SMM;
        req_msg_head.target_instance = 0;
    } else {
        req_msg_head.target_type = (TARGET_TYPE_E)0x08;
        slot_id = get_blade_num(location);
        if (slot_id == 0xff) {
            g_printf("Usage: ipmcget -l <bladeN|swiN> -d deviceid\r\n");
            return IPMC_ERROR;
        }
        result = check_blade_present_communication_status(location);
        if (result != IPMC_OK) {
            return FALSE;
        }
        req_msg_head.target_instance = slot_id;
    }

    req_msg_head.netfn = NETFN_APP_REQ;
    req_msg_head.cmd = IPMI_GET_DEVICE_ID;
    req_msg_head.lun = 0;
    req_msg_head.src_len = 0;
    result = ipmi_send_request(&req_msg_head, NULL, MAX_POSSIBLE_IPMI_FRAME_LEN, &response, TRUE);
    if (result != RET_OK) {
        g_printf("[%s]get ipmicmd(netfn/cmd=%x/%x) respdata null.\r\n", location, req_msg_head.netfn, req_msg_head.cmd);
        return RET_ERR;
    }

    resp_src_data = (const guchar *)get_ipmi_src_data(response);
    if (resp_src_data == NULL) {
        
        g_free(response);
        g_printf("[%s]get ipmicmd(netfn/cmd=%x/%x) respdata null.\r\n", location, req_msg_head.netfn, req_msg_head.cmd);
        return RET_ERR;
    }

    completion_code = resp_src_data[0];
    if (completion_code != COMP_CODE_SUCCESS) {
        g_free(response);
        return completion_code;
    }
    smm_cli_print_deviceid(resp_src_data);
    g_free(response);

    return RET_OK;
}


gint32 ipmc_get_pem_presence(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    OBJ_HANDLE object_handle;
    gint32 ret;
    gint32 slot;
    guint8 presence = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    
    if (!strcmp(location, "pem")) {
        g_printf("List Present Pem:\r\n");
        if (dfl_foreach_object(CLASS_POWER_MGNT, foreach_pem_presence, NULL, NULL) != DFL_OK) {
            g_printf("Get pem presence failed.\r\n");
            return FALSE;
        }
        return TRUE;
    }

    
    slot = get_pem_num(location);
    
    ret = dal_get_specific_object_byte(CLASS_POWER_MGNT, PROPERTY_POWER_MGNT_SLOT, (guchar)slot - 1, &object_handle);
    if (ret != RET_OK) {
        g_printf("Get pem presence failed.\r\n");

        return FALSE;
    }

    
    ret = dal_get_property_value_byte(object_handle, PROPERTY_POWER_MGNT_PRESENCE, (guchar *)&presence);
    if (ret != RET_OK) {
        g_printf("Get pem presence failed.\r\n");
        return FALSE;
    }

    if (presence == SHELF_PART_PRESENCE) {
        g_printf("P%s is present.\r\n", &location[1]);
    } else {
        g_printf("P%s is not present.\r\n", &location[1]);
    }

    return TRUE;
}

gint32 smm_get_pem_health_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 iRet;
    gint32 ret;
    guint32 data[6] = { 0 };
    gchar pem_name[MAX_NAME_SIZE] = { 0 };
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget -l <pem|pemN> -d health\r\n");
        return FALSE;
    }
    iRet = snprintf_s(pem_name, sizeof(pem_name), sizeof(pem_name) - 1, "%s", target);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    pem_name[0] = g_ascii_toupper(pem_name[0]);
    data[3] = PEM_EVENT_CODE;
    data[4] = 0xff000000;
    if (strcmp(target, "pem") == 0) { // 打印所有pem健康事件
        data[5] = 0;
    } else { // 打印单个pem健康事件
        data[5] = get_pem_num(target);
        if (get_pem_presence(data[5]) != BLADE_PRESENCE) {
            g_printf("%s is not present.\r\n", pem_name);
            return TRUE;
        }
    }
    ret = foreach_smm_health_event(count_single_smm_health_event, (gpointer)data);
    if (ret != RET_OK) {
        g_printf("Get %s health status failed.\r\n", target);
        return FALSE;
    }
    if (data[0] == 0 && data[1] == 0 && data[2] == 0) {
        g_printf("%s in health state.\r\n", pem_name);
        return TRUE;
    }
    g_printf("%s Events:\r\n", pem_name);
    if (data[0] != 0) { // 返回的一般告警数目
        g_printf("%u minor events.\r\n", data[0]);
    }
    if (data[1] != 0) { // 严重告警数目
        g_printf("%u major events.\r\n", data[1]);
    }
    if (data[2] != 0) { // 致命告警数目
        g_printf("%u critical events.\r\n", data[2]);
    }

    return TRUE;
}

gint32 smm_get_pem_health_event(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    errno_t safe_fun_ret;
    gint32 ret;
    guint32 data[6] = { 0 };
    guint32 id;
    gchar pem_name[MAX_NAME_SIZE] = { 0 };

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget -l <pem|pemN> -d healthevents\r\n");
        return FALSE;
    }
    safe_fun_ret = strncpy_s(pem_name, sizeof(pem_name), target, sizeof(pem_name) - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    pem_name[0] = g_ascii_toupper(pem_name[0]);
    if (strcmp(target, "pem") == 0) { // 打印所有pem健康事件
        id = 0;
    } else { // pemN健康事件
        id = get_pem_num(target);
        if (get_pem_presence(id) != BLADE_PRESENCE) {
            g_printf("%s is not present.\r\n", pem_name);
            return TRUE;
        }
    }
    
    data[3] = PEM_EVENT_CODE;
    data[4] = 0xff000000;
    data[5] = id;
    ret = foreach_smm_health_event(count_single_smm_health_event, (gpointer)data);
    if (ret != RET_OK) {
        g_printf("Get %s health event failed.\r\n", target);
        return FALSE;
    }
    if (data[0] == 0 && data[1] == 0 && data[2] == 0) {
        g_printf("%s in health state.\r\n", pem_name);
        return TRUE;
    }
    
    g_printf("%-10s | %-20s | %-12s | %-12s | %s\r\n", "Event Num", "Event Time", "Alarm Level", "Event Code",
        "Event Description");
    data[0] = 0;
    data[1] = PEM_EVENT_CODE;
    data[2] = 0xff000000;
    data[3] = id;
    ret = foreach_smm_health_event(print_single_smm_health_event, (gpointer)data);
    if (ret != RET_OK) {
        g_printf("Get %s health event failed.\r\n", target);
        return FALSE;
    }
    return TRUE;
}


gint32 ipmc_get_pem_version(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    OBJ_HANDLE power_mgnt_object_handle = 0;
    OBJ_HANDLE onepower_object_handle = 0;
    gint32 ret;
    gint32 slot;
    guint8 presence = 0;
    guint8 communication = 0;
    gchar hardware_version_buf[VERSION_LEN] = {0};
    gchar software_version_buf[VERSION_LEN] = {0};

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    
    if (!strcmp(location, "pem")) {
        if (dfl_foreach_object(CLASS_NAME_PS, foreach_pem_version, NULL, NULL) != DFL_OK) {
            g_printf("Usage: ipmcget -l <pem|pemN> -d version\r\n");
            return FALSE;
        }

        return TRUE;
    }

    
    slot = get_pem_num(location);
    
    ret = dal_get_specific_object_byte(CLASS_POWER_MGNT, PROPERTY_POWER_MGNT_SLOT, (guchar)(slot - 1),
        &power_mgnt_object_handle);
    if (ret != RET_OK) {
        g_printf("Get pem version failed.\r\n");
        return FALSE;
    }

    
    (void)dal_get_property_value_byte(power_mgnt_object_handle, PROPERTY_POWER_MGNT_PRESENCE, &presence);
    if (presence == 0) {
        g_printf("P%s is not present.\r\n", &location[1]);
        return FALSE;
    }

    ret = dal_get_specific_object_byte(CLASS_NAME_PS, PROTERY_PS_INDEX, (guchar)slot - 1, &onepower_object_handle);
    if (ret != RET_OK) {
        g_printf("Get pem version failed.\r\n");
        return FALSE;
    }

    (void)dal_get_property_value_byte(onepower_object_handle, PROTERY_PS_COMMU_STATE, &communication);
    if (communication == 1) {
        g_printf("P%s communication lost.\r\n", &location[1]);
        return FALSE;
    }

    ret = dal_get_property_value_string(onepower_object_handle, PROTERY_PS_VERSION, hardware_version_buf,
        sizeof(hardware_version_buf));
    if (ret != RET_OK) {
        (void)memset_s(hardware_version_buf, VERSION_LEN, 0, VERSION_LEN);
    }
    ret = dal_get_property_value_string(onepower_object_handle, PROTERY_PS_FWVERSION, software_version_buf,
        sizeof(software_version_buf));
    if (ret != RET_OK) {
        (void)memset_s(software_version_buf, VERSION_LEN, 0, VERSION_LEN);
    }

    g_printf("Hardware Version:%s\r\n", hardware_version_buf);
    g_printf("Software Version:%s\r\n", software_version_buf);

    return TRUE;
}


gint32 ipmc_get_pem_fault_reason(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    OBJ_HANDLE power_mgnt_object_handle = 0;
    OBJ_HANDLE onepower_object_handle = 0;
    gint32 ret;
    gint32 slot;
    guint8 presence = 0;
    guint8 communication = 0;
    gchar faultreason_buf[1024] = {0};

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    
    slot = get_pem_num(location);
    
    ret = dal_get_specific_object_byte(CLASS_POWER_MGNT, PROPERTY_POWER_MGNT_SLOT, (guchar)(slot - 1),
        &power_mgnt_object_handle);
    if (ret != RET_OK) {
        g_printf("Get pem version failed.\r\n");
        return FALSE;
    }

    
    (void)dal_get_property_value_byte(power_mgnt_object_handle, PROPERTY_POWER_MGNT_PRESENCE, &presence);
    if (presence == 0) {
        g_printf("P%s is not present.\r\n", &location[1]);
        return FALSE;
    }

    ret = dal_get_specific_object_byte(CLASS_NAME_PS, PROTERY_PS_INDEX, (guchar)slot - 1, &onepower_object_handle);
    if (ret != RET_OK) {
        g_printf("Get pem version failed.\r\n");
        return FALSE;
    }

    (void)dal_get_property_value_byte(onepower_object_handle, PROTERY_PS_COMMU_STATE, &communication);
    if (communication == 1) {
        g_printf("P%s communication lost.\r\n", &location[1]);
        return FALSE;
    }

    ret = dal_get_property_value_string(power_mgnt_object_handle, PROPERTY_POWER_MGNT_FAULT_REASON, faultreason_buf,
        sizeof(faultreason_buf));
    if (ret != RET_OK) {
        g_printf("Get pem fault reason failed.\r\n");
        return RET_OK;
    }

    if (strlen(faultreason_buf) > 0) {
        g_printf("%s", faultreason_buf);
    } else {
        g_printf("Pem has no problem.\r\n");
    }

    return TRUE;
}
LOCAL gint32 get_pem_type_handler(guint8 *pem_type_value, gchar *location, gint32 slot)
{
    gint32 ret;
    OBJ_HANDLE object_handle;
    guint8 presence = 0;
    guint8 comm_status = 0;
    guchar protocol = PROTOCOL_INVALID;
    guint8 pem_type = 0;

    
    ret = dal_get_specific_object_byte(CLASS_POWER_MGNT, PROPERTY_POWER_MGNT_SLOT, (guchar)(slot - 1), &object_handle);
    if (ret != RET_OK) {
        goto ERR_EXIT;
    }

    
    ret = dal_get_property_value_byte(object_handle, PROPERTY_POWER_MGNT_PRESENCE, (guchar *)&presence);
    if (ret != RET_OK) {
        goto ERR_EXIT;
    }
    if (presence == 0) {
        g_printf("P%s is not present.\r\n", &location[1]);
        return RET_ERR;
    }

    
    ret = dal_get_specific_object_byte(CLASS_NAME_PS, PROTERY_PS_INDEX, (guchar)(slot - 1), &object_handle);
    if (ret != RET_OK) {
        goto ERR_EXIT;
    }

    
    ret = dal_get_property_value_byte(object_handle, PROTERY_PS_COMMU_STATE, (guchar *)&comm_status);
    if (ret != RET_OK) {
        goto ERR_EXIT;
    }
    if (comm_status == 1) {
        g_printf("P%s communication lost.\r\n", &location[1]);
        return RET_ERR;
    }

    
    ret = dal_get_property_value_byte(object_handle, PROTERY_PS_INPUTMODE, (guchar *)&pem_type);
    if (ret != RET_OK) {
        goto ERR_EXIT;
    }

    ret = dal_get_property_value_byte(object_handle, PROTERY_PS_PROTOCAL, &protocol);
    if (ret != DFL_OK) {
        goto ERR_EXIT;
    }
    
    if (protocol == PROTOCOL_TCE_PMBUS) {
        pem_type = (pem_type == PEM_TYPE_AC) ? PEM_TYPE_DC : ((pem_type == PEM_TYPE_DC) ? PEM_TYPE_AC : pem_type);
    }
    *pem_type_value = pem_type;
    return RET_OK;

ERR_EXIT:
    g_printf("Get pem type failed.\r\n");
    return RET_ERR;
}

gint32 ipmc_get_pem_type(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    gint32 ret;
    guint8 pem_type = 0;
    gint32 slot;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    
    slot = get_pem_num(location);
    ret = get_pem_type_handler(&pem_type, location, slot);
    if (ret != RET_OK) {
        return FALSE;
    }

    if (pem_type == PEM_TYPE_AC) {
        g_printf("Pem type: AC.\r\n");
    } else if (pem_type == PEM_TYPE_DC) {
        g_printf("Pem type: DC.\r\n");
    } else if (pem_type == PEM_TYPE_AC_AND_DC) {
        g_printf("Pem type: AC and DC.\r\n");
    }

    return TRUE;
}


gint32 ipmc_get_pem_rate_power(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    OBJ_HANDLE object_handle;
    gint32 ret;
    gint32 slot;
    guint8 presence = 0;
    guint16 rate_power = 0;
    guint8 comm_status = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    
    slot = get_pem_num(location);
    
    ret = dal_get_specific_object_byte(CLASS_POWER_MGNT, PROPERTY_POWER_MGNT_SLOT, (guchar)(slot - 1), &object_handle);
    if (ret != RET_OK) {
        g_printf("Get pem rating power failed.\r\n");
        return FALSE;
    }

    
    ret = dal_get_property_value_byte(object_handle, PROPERTY_POWER_MGNT_PRESENCE, (guchar *)&presence);
    if (ret != RET_OK) {
        g_printf("Get pem rating power failed.\r\n");
        return FALSE;
    }

    if (presence == 0) {
        g_printf("P%s is not present.\r\n", &location[1]);
        return FALSE;
    }

    
    ret = dal_get_specific_object_byte(CLASS_NAME_PS, PROTERY_PS_INDEX, (guchar)(slot - 1), &object_handle);
    if (ret != RET_OK) {
        g_printf("Get pem rating power failed.\r\n");
        return FALSE;
    }

    
    ret = dal_get_property_value_byte(object_handle, PROTERY_PS_COMMU_STATE, (guchar *)&comm_status);
    if (ret != RET_OK) {
        g_printf("Get pem rating power failed.\r\n");
        return FALSE;
    }

    if (comm_status == 1) {
        g_printf("P%s communication lost.\r\n", &location[1]);
        return FALSE;
    }
    
    ret = dal_get_property_value_uint16(object_handle, PROTERY_PS_RATING, &rate_power);
    if (ret != RET_OK) {
        g_printf("Get pem rating power failed.\r\n");
        return FALSE;
    }

    g_printf("%d Watts.\r\n", (guint32)rate_power);

    return TRUE;
}


gint32 ipmc_get_pem_real_power(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    OBJ_HANDLE object_handle;
    gint32 ret;
    gint32 slot;
    guint8 presence = 0;
    double real_power = 0;
    guint8 comm_status = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    
    slot = get_pem_num(location);
    
    ret = dal_get_specific_object_byte(CLASS_POWER_MGNT, PROPERTY_POWER_MGNT_SLOT, (guchar)(slot - 1), &object_handle);
    if (ret != RET_OK) {
        g_printf("Get pem runtime power failed.\r\n");
        return FALSE;
    }

    
    ret = dal_get_property_value_byte(object_handle, PROPERTY_POWER_MGNT_PRESENCE, (guchar *)&presence);
    if (ret != RET_OK) {
        g_printf("Get pem runtime power failed.\r\n");
        return FALSE;
    }

    if (presence == 0) {
        g_printf("P%s is not present.\r\n", &location[1]);
        return FALSE;
    }

    
    ret = dal_get_specific_object_byte(CLASS_NAME_PS, PROTERY_PS_INDEX, (guchar)(slot - 1), &object_handle);
    if (ret != RET_OK) {
        g_printf("Get pem runtime power failed.\r\n");
        return FALSE;
    }

    
    ret = dal_get_property_value_byte(object_handle, PROTERY_PS_COMMU_STATE, (guchar *)&comm_status);
    if (ret != RET_OK) {
        g_printf("Get pem runtime power failed.\r\n");
        return FALSE;
    }

    if (comm_status == 1) {
        g_printf("P%s communication lost.\r\n", &location[1]);
        return FALSE;
    }

    
    ret = dal_get_property_value_double(object_handle, PROTERY_PS_INPUTPOWER, (double *)&real_power);
    if (ret != RET_OK) {
        g_printf("Get pem runtime power failed.\r\n");
        return FALSE;
    }

    g_printf("%d Watts.\r\n", (guint32)real_power);

    return TRUE;
}

gint32 smm_get_blade_cooling_medium(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    OBJ_HANDLE object_handle;
    gint32 ret;
    gint32 presence;
    guint8 medium;
    gint32 medium_param = 0;
    guchar board_num;
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget -l bladeN -d coolingmode\r\n");
        return FALSE;
    }

    board_num = get_blade_num(location);
    if (board_num == 0xff) {
        g_printf("Usage: ipmcget -l bladeN -d coolingmode\r\n");
        return FALSE;
    }
    
    ret = dfl_get_object_handle(location, &object_handle);
    if (ret != DFL_OK) {
        g_printf("get %s presence state failed.\r\n", location);
        return FALSE;
    }
    
    location[0] = location[0] - ('a' - 'A');
    presence = get_blade_presence(object_handle);
    if (presence != SHELF_PART_PRESENCE) {
        g_printf("%s is not present.\r\n", location);
        return FALSE;
    }

    
    ret = ipmi_get_cooling_medium(board_num, &medium_param);
    if (ret != RET_OK) {
        g_printf("Get cooling mode failed.\r\n");
        return FALSE;
    }
    medium = (guint8)medium_param;
    if (medium == 0) {
        g_printf("%s is air-cooled.\r\n", location);
    } else if (medium == 1) {
        g_printf("%s is liquid-cooled.\r\n", location);
    } else {
        g_printf("%s cooling medium(%d) error.\r\n", location, medium);
    }

    return TRUE;
}

gint32 smm_get_blade_user_list(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    gint32 ret_val;
    guint8 board_num;
    gchar username[PARAMETER_LEN + 1] = { 0 };
    gchar ip[PARAMETER_LEN + 1] = { 0 };
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    guint8 user_count;
    gint32 i;
    guint slave_addr;
    
    gchar blade_name[OBJ_NAME_MAX_LEN] = { 0 };
    

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if ((argc != 0)) {
        g_printf("Usage: ipmcget -l <bladeN|swiN> -d userlist\r\n");
        return FALSE;
    }

    
    board_num = get_blade_num(location);
    if (board_num == 0xff) {
        g_printf("Usage: ipmcget -l <bladeN|swiN> -d userlist\r\n");
        return FALSE;
    }

    slave_addr = get_blade_ipmbaddr(board_num);
    if (get_ipmbeth_blade_name(slave_addr, blade_name, sizeof(blade_name)) != RET_OK) {
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(slave_addr));
    input_list = g_slist_append(input_list, g_variant_new_byte(1));
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }
    ret_val = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_NAME_IPMBETH_BLADE, blade_name, METHOD_GET_BLADE_BMC_USERPRIVILEGE, input_list, &output_list);
    if (ret_val != RET_OK) {
        print_blade_err_info(location, ret_val);
        g_printf("Get blade user list failed.\r\n");
        uip_free_gvariant_list(input_list);
        uip_free_gvariant_list(output_list);
        return FALSE;
    }

    user_count = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    g_printf("%-8s%-18s%-15s%-15s%-15s%-18s\r\n", "ID", "Name", "Callin", "Link Auth", "IPMI Msg",
        "Channel Priv Limit");

    for (i = MIN_USER_ID; i <= user_count; i++) {
        print_blade_bmc_user_info((const gchar *)username, (const gchar *)ip, blade_name, slave_addr, i);
    }
    return TRUE;
}


gint32 smm_get_blade_real_time_power(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location)
{
    guint8 board_num;
    guint8 slave_addr;
    OBJ_HANDLE obj_handle = 0;
    guint16 current_power = 0;
    gint32 ret;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if ((argc != 0)) {
        g_printf("Usage: ipmcget -l bladeN -d realtimepower\r\n");
        return FALSE;
    }

    
    board_num = get_blade_num(location);
    if (board_num == 0xff) { 
        g_printf("Usage: ipmcget -l bladeN -d realtimepower\r\n");
        return FALSE;
    }

    ret = check_blade_present_communication_status(location);
    if (ret != IPMC_OK) {
        return FALSE;
    }

    slave_addr = get_blade_ipmbaddr(board_num);
    
    (void)dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROTERY_IPMBETH_BLADE_SLAVEADDR, slave_addr,
        &obj_handle);

    
    (void)dal_get_property_value_uint16(obj_handle, PROTERY_IPMBETH_POWER_CAPPING_CURRENT_POWER, &current_power);

    location[0] = location[0] - ('a' - 'A');
    g_printf("%s real-time power is %d Watts.\r\n", location, current_power);

    return TRUE;
}


gint32 smm_get_blade_power_capping_value_range(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *location)
{
    guint8 board_num;
    guint8 slave_addr;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    guint16 value_base;
    guint16 value_top;
    GSList *output_list = NULL;
    guint8 presence = 0;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if ((argc != 0)) {
        g_printf("Usage: ipmcget -l bladeN -d powercappingvaluerange\r\n");
        return FALSE;
    }

    
    board_num = get_blade_num(location);
    if (board_num > 32) { 
        g_printf("Usage: ipmcget -l bladeN -d powercappingvaluerange\r\n");
        return FALSE;
    }
    slave_addr = get_blade_ipmbaddr(board_num);
    
    (void)dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROTERY_IPMBETH_BLADE_SLAVEADDR, slave_addr,
        &obj_handle);

    
    (void)dal_get_property_value_byte(obj_handle, PROTERY_IPMBETH_BLADE_PRESENCE, &presence);

    if (!presence) {
        g_printf("Blade%d is not present.\r\n", board_num);
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_NAME_IPMBETH_BLADE,
        dfl_get_object_name(obj_handle), METHOD_IPMBETH_BLADE_GET_CAPPING_VALUE_RANGE, NULL, &output_list);
    if (ret != RET_OK) {
        g_printf("Get blade%d power capping value range failed.\r\n", board_num);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

        return FALSE;
    }

    value_base = g_variant_get_uint16((GVariant *)g_slist_nth_data(output_list, 0));
    value_top = g_variant_get_uint16((GVariant *)g_slist_nth_data(output_list, 1));
    
    if (value_top & 0x8000) {
        g_printf("There is no remaining power capping value for the blade%d.\r\n", board_num);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

        return FALSE;
    }

    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    if (value_base <= value_top) {
        
        if (value_top > 9999) {
            value_top = 9999; 
        }
        g_printf("Blade%d power capping value range is [%d %d].\r\n", board_num, value_base, value_top);
    } else {
        g_printf("The max remaining capping power for the blade%d is %d Watts.\r\n", board_num, value_top);
    }

    return TRUE;
}


gint32 smm_get_blade_list_frus(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    guint8 slave_addr;
    gint32 ret;
    guint8 slot_id;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gchar username[PARAMETER_LEN + 1] = { 0 };
    gchar ip[PARAMETER_LEN + 1] = { 0 };
    
    gchar blade_name[OBJ_NAME_MAX_LEN] = { 0 };
    
    GSList *fru_node = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -l <bladeN|swiN> -d listpresentfrus\r\n");
        return FALSE;
    }

    slot_id = get_blade_num(location);
    if (slot_id == 0xff) {
        g_printf("Usage: ipmcget -l <bladeN|swiN> -d listpresentfrus\r\n");
        return IPMC_ERROR;
    }

    ret = check_blade_present_communication_status(location);
    if (ret != IPMC_OK) {
        return FALSE;
    }

    slave_addr = get_blade_ipmbaddr(slot_id);
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    if (get_ipmbeth_blade_name(slave_addr, blade_name, sizeof(blade_name)) != RET_OK) {
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(slave_addr));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_NAME_IPMBETH_BLADE,
        blade_name, METHOD_GET_BLADE_PRESENT_FRU, input_list, &output_list);
    if (ret != RET_OK) {
        g_printf("Get blade list present frus info failed.\r\n");
        uip_free_gvariant_list(input_list);
        uip_free_gvariant_list(output_list);
        return FALSE;
    }

    g_printf("List Present Information:\r\n");

    for (fru_node = output_list; fru_node; fru_node = fru_node->next) {
        g_printf("%s:%d\r\n", location, g_variant_get_byte((GVariant *)fru_node->data));
    }

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
    return TRUE;
}


gint32 smm_get_blade_version(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret;
    guint8 slot_id;
    gchar blade_version[BLADE_VERSION_MAX_LEN] = {0};
    gchar blade_version_tmp[256] = {0};
    guint8 msg_total_count = 0;
    guint8 msg_index = 1;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -l <bladeN|swiN> -d version\r\n");
        return IPMC_ERROR;
    }

    slot_id = get_blade_num(location);
    if (slot_id == 0xff) {
        g_printf("Usage: ipmcget -l <bladeN|swiN> -d version\r\n");
        return IPMC_ERROR;
    }

    ret = check_blade_present_communication_status(location);
    if (ret != IPMC_OK) {
        return FALSE;
    }

    while (1) {
        memset_s(blade_version_tmp, sizeof(blade_version_tmp), 0, sizeof(blade_version_tmp));
        ret =
            ipmi_get_blade_version(msg_index, slot_id, blade_version_tmp, sizeof(blade_version_tmp), &msg_total_count);
        if (ret != RET_OK) {
            g_printf("Get %s version failed.\r\n", location);
            return FALSE;
        }
        safe_fun_ret = strncat_s(blade_version, sizeof(blade_version), blade_version_tmp, sizeof(blade_version_tmp));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }

        if (msg_index >= msg_total_count) {
            g_printf("%s\r\n", blade_version);
            return TRUE;
        }
        msg_index++;
    }
}


LOCAL void print_get_ip_helpinfo(gchar *target)
{
    guint8 software_type = 0;

    (void)dal_get_software_type(&software_type);

    if (software_type == MGMT_SOFTWARE_TYPE_EM) {
        g_printf("Usage: ipmcget -l <bladeN|swiN> -d ipinfo\r\n");
    } else {
        g_printf("Usage: ipmcget -l bladeN -d ipinfo\r\n");
    }
}


gint32 smm_get_blade_ip_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    guint8 slave_addr;
    gint32 ret;
    guint8 slot_id;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    
    
    if (argc != 0) {
        print_get_ip_helpinfo(location);
        return FALSE;
    }
    
    

    slot_id = ipmc_get_blade_num(location);
    if (slot_id == 0xff) {
        print_get_ip_helpinfo(location);
        return IPMC_ERROR;
    }

    ret = check_blade_present_communication_status(location);
    if (ret != IPMC_OK) {
        return FALSE;
    }

    slave_addr = ipmc_get_blade_ipmbaddr(slot_id);
    return print_blade_bmc_ip_info(slave_addr);
}
