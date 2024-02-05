
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "pme_app/pme_app.h"
#include "pme_app/uip/uip_network_config.h"
#include "uip.h"
#include "ipmctypedef.h"
#include "ipmcgetcommand.h"
#include "ipmc_shelf_common.h"
#include "ipmcgethelp.h"
#include "ipmc_public.h"
#include "mscm_macro.h"
#include "smm_sol_client.h"
#include "ipmcget_sys_mgmt.h"
#include "shelfgetcommand.h"


LOCAL gint32 foreach_pem_health(OBJ_HANDLE object_handle, gpointer user_data)
{
    guint8 health = 0;
    guint8 slotid = 0;
    gint32 ret;
    guint8 presence = 0;

    ret = dal_get_property_value_byte(object_handle, PROPERTY_POWER_MGNT_PS_HEALTH, &health);
    if (ret != RET_OK) {
        return DFL_ERR;
    }
    ret = dal_get_property_value_byte(object_handle, PROPERTY_POWER_MGNT_SLOT, &slotid);
    if (ret != RET_OK) {
        return DFL_ERR;
    }

    
    
    ret = dal_get_property_value_byte(object_handle, PROPERTY_POWER_MGNT_PRESENCE, (guchar *)&presence);
    if (ret != RET_OK) {
        return DFL_ERR;
    }
    

    if (presence == SHELF_PART_PRESENCE) {
        if (health == 1) { 
            g_printf("Pem%d has problem.\r\n", slotid + 1);
            *(guint8 *)user_data = *(guint8 *)user_data + 1;
        }
    }

    return DFL_OK;
}


gint32 ipmc_get_pem_health(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    OBJ_HANDLE object_handle;
    gint32 ret;
    gint32 slot;
    guint8 presence = 0;
    guint8 health = 0;
    guint8 pem_has_problem_count = 0; 

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    
    if (!strcmp(location, "pem")) {
        ret = dfl_foreach_object(CLASS_POWER_MGNT, foreach_pem_health, (gpointer)&pem_has_problem_count, NULL);
        if (ret != DFL_OK) {
            g_printf("Get pem health failed.\r\n");
            return FALSE;
        }

        if (pem_has_problem_count == 0) {
            g_printf("Pem has no problem.\r\n");
        }

        return TRUE;
    }

    
    slot = get_pem_num(location);
    
    ret = dal_get_specific_object_byte(CLASS_POWER_MGNT, PROPERTY_POWER_MGNT_SLOT, (guchar)(slot - 1), &object_handle);
    if (ret != RET_OK) {
        g_printf("Get pem health failed.\r\n");
        return FALSE;
    }

    
    ret = dal_get_property_value_byte(object_handle, PROPERTY_POWER_MGNT_PRESENCE, (guchar *)&presence);
    if (ret != RET_OK) {
        g_printf("Get pem health failed.\r\n");
        return FALSE;
    }

    if (presence == 0) {
        g_printf("P%s is not present.\r\n", &location[1]);
        return FALSE;
    }

    
    ret = dal_get_property_value_byte(object_handle, PROPERTY_POWER_MGNT_PS_HEALTH, (guchar *)&health);
    if (ret != RET_OK) {
        g_printf("Get pem health failed.\r\n");
        return FALSE;
    }

    if (health == 0) {
        g_printf("P%s has no problem.\r\n", &location[1]);
    } else {
        g_printf("P%s has problem.\r\n", &location[1]);
    }

    return TRUE;
}


LOCAL gint32 get_pem_fru_object(guint32 slot, OBJ_HANDLE *fru_handle)
{
    gint32 ret;
    OBJ_HANDLE object_handle[PS_DEV_CNT_MAX] = {0};
    guint8 pem_num = 0;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    guchar fru_type = 0;
    guint32 position = 0;
    gint32 i;

    ret = dfl_get_object_list(CLASS_FRU, &obj_list);
    if (ret != RET_OK) {
        g_printf("Get FRU object list failed.\r\n");
        return FALSE;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROPERTY_FRU_TYPE, (guchar *)&fru_type);
        if (ret != RET_OK) {
            g_printf("Get FRU type failed.\r\n");
            g_slist_free(obj_list);
            return FALSE;
        }

        if (fru_type == FRU_TYPE_POWER) {
            object_handle[pem_num++] = (OBJ_HANDLE)obj_note->data;
        }
    }

    for (i = 0; i < pem_num; i++) {
        ret = dal_get_property_value_uint32(object_handle[i], PROPERTY_FRU_POSITION, (guint32 *)&position);
        if (ret != RET_OK) {
            g_printf("Get FRU position failed.\r\n");
            g_slist_free(obj_list);
            return FALSE;
        }

        
        if (slot == position - 19) {
            *fru_handle = object_handle[i];
            break;
        }
        
    }

    g_slist_free(obj_list);

    return RET_OK;
}


gint32 smm_get_pem_fru(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target,
    gchar *location)
{
    guint32 slot;
    gint32 ret;
    OBJ_HANDLE object_handle = 0;
    guint8 presence = 0;
    guint8 comm_status = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -l pemN -t fru -d all\r\n");
        return FALSE;
    }

    
    if (!strcmp(location, "pem")) {
        g_printf("Please Operate:\r\n");
        g_printf("pem1 ~ pem6\r\n");

        return FALSE;
    }
    

    
    slot = get_pem_num(location);
    
    ret = dal_get_specific_object_byte(CLASS_POWER_MGNT, PROPERTY_POWER_MGNT_SLOT, (guchar)(slot - 1), &object_handle);
    if (ret != RET_OK) {
        g_printf("Get pem fru failed.\r\n");

        return FALSE;
    }

    
    ret = dal_get_property_value_byte(object_handle, PROPERTY_POWER_MGNT_PRESENCE, (guchar *)&presence);
    if (ret != RET_OK) {
        g_printf("Get pem fru failed.\r\n");

        return FALSE;
    }

    if (presence == 0) {
        g_printf("P%s is not present.\r\n", &location[1]);
        return FALSE;
    }

    
    ret = dal_get_specific_object_byte(CLASS_NAME_PS, PROTERY_PS_INDEX, (guchar)(slot - 1), &object_handle);
    if (ret != RET_OK) {
        g_printf("Get pem fru failed.\r\n");

        return FALSE;
    }
    
    ret = dal_get_property_value_byte(object_handle, PROTERY_PS_COMMU_STATE, (guchar *)&comm_status);
    if (ret != RET_OK) {
        g_printf("Get pem fru failed.\r\n");
        return FALSE;
    }

    if (comm_status == 1) {
        g_printf("P%s communication lost.\r\n", &location[1]);
        return FALSE;
    }

    
    ret = get_pem_fru_object(slot, &object_handle);
    if (ret != RET_OK) {
        return FALSE;
    }

    ret = print_fru_info(object_handle);
    if (ret != RET_OK) {
        g_printf("Get pem fru failed.\r\n");
        return FALSE;
    }

    return TRUE;
}


gint32 ipmc_get_psu_sleep_config(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    gint32 ret;
    OBJ_HANDLE object_handle = 0;
    guint8 sleep_enable = 0;
    guint8 mode = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -l shelf -d psusleepconfig\r\n");
        return FALSE;
    }

    ret = dal_get_object_handle_nth(CLASS_POWER_SLEEP, 0, &object_handle);
    if (ret != RET_OK) {
        g_printf("Get psu sleep config failed.\r\n");
        return FALSE;
    }

    ret = dal_get_property_value_byte(object_handle, PROPERTY_POWER_SLEEP_ENABLE, &sleep_enable);
    if (ret != RET_OK) {
        g_printf("Get psu sleep config failed.\r\n");
        return FALSE;
    }

    if (sleep_enable == 0) { 
        g_printf("PSU Sleep is disabled.\r\n");
    } else { 
        ret = dal_get_property_value_byte(object_handle, ROPERTY_POWER_SLEEP_MODE, &mode);
        if (ret != RET_OK) {
            g_printf("Get psu sleep config failed.\r\n");
            return FALSE;
        }

        g_printf("PSU Sleep is enabled.\r\n");

        if (mode == 0) {
            g_printf("PSU Sleep Mode is N+1.\r\n");
        } else if (mode == 1) {
            g_printf("PSU Sleep Mode is N+N.\r\n");
        } else {
        }
    }

    return TRUE;
}


gint32 ipmc_get_psu_sleep_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *location)
{
    gint32 ret;
    OBJ_HANDLE object_handle = 0;
    guint8 sleep_enable = 0;
    guint16 sleep_bitmap = 0;
    guint32 i = 0;
    guint8 sleep_psu_count = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -l shelf -d psusleepstatus\r\n");
        return FALSE;
    }

    ret = dal_get_object_handle_nth(CLASS_POWER_SLEEP, 0, &object_handle);
    if (ret != RET_OK) {
        g_printf("Get psu sleep status failed.\r\n");
        return FALSE;
    }

    ret = dal_get_property_value_byte(object_handle, PROPERTY_POWER_SLEEP_ENABLE, &sleep_enable);
    if (ret != RET_OK) {
        g_printf("Get psu sleep status failed.\r\n");
        return FALSE;
    }

    if (sleep_enable == 0) { 
        g_printf("Sleep PSU Count is:0\r\n");
    } else { 
        ret = dal_get_property_value_uint16(object_handle, ROPERTY_POWER_SLEEP_SLEEP_STATUS_BITMAP, &sleep_bitmap);
        if (ret != RET_OK) {
            g_printf("Get psu sleep status failed.\r\n");
            return FALSE;
        }

        if (sleep_bitmap == 0) {
            g_printf("Sleep PSU Count is:0\r\n");
        } else {
            
            for (i = 0; i < 16; i++) {
                if (((sleep_bitmap >> i) & 0x0001) == 0x0001) {
                    sleep_psu_count++;
                }
            }
            g_printf("Sleep PSU Count is:%d\r\n", sleep_psu_count);

            g_printf("Sleep PSU Index is:");
            for (i = 0; i < 16; i++) {
                if (((sleep_bitmap >> i) & 0x0001) == 0x0001) {
                    g_printf("%d ", i + 1);
                }
            }

            g_printf("\r\n");
        }
    }

    return TRUE;
}


gint32 smm_get_power_capping_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target,
    gchar *location)
{
    gint32 ret;

    ret = ipmc_get_powercapping_info(privilege, rolepriv, argc, argv, target);
    return ret;
}


gint32 smm_get_power_capping_value_range(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target, gchar *location)
{
    gint32 ret;
    OBJ_HANDLE handle = 0;
    GSList *output_list = NULL;
    guint16 value_base;
    guint16 value_top;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    
    ret = dal_get_object_handle_nth(CLASS_NAME_SHELF_POWER_CAPPING, 0, &handle);
    if (ret != RET_OK) {
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_NAME_SHELF_POWER_CAPPING, dfl_get_object_name(handle), METHOD_GET_SHELF_POWER_CAPPING_VALUE_RANGE, NULL,
        &output_list);
    if (ret != RET_OK) {
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        g_printf("Get shelf power capping value range failed.\r\n");
        return FALSE;
    }

    value_base = g_variant_get_uint16((GVariant *)g_slist_nth_data(output_list, 0));
    value_top = g_variant_get_uint16((GVariant *)g_slist_nth_data(output_list, 1));

    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    
    if (value_top > value_base) {
        g_printf("Shelf power capping value range is [%d %d].\r\n", value_base, value_top);
    } else {
        g_printf("The max settable shelf capping power is %d Watts.\r\n", value_top);
    }

    return TRUE;
}


gint32 smm_sol_get_com_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target,
    gchar *location)
{
    gint32 blade_number;
    gint32 ret;
    gchar *usage = "Usage: ipmcget -l <bladeN|swiN> -t sol -d cominfo\r\n";
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("%s", usage);
        return FALSE;
    }
    blade_number = get_blade_num(location);
    if (blade_number == 0xff) {
        g_printf("%s", usage);
        return FALSE;
    }
    ret = smm_sol_start_process(COM_INFO, (guint32)blade_number, 0, NULL, NULL);
    if (ret != RET_OK) {
        return FALSE;
    }
    return TRUE;
}


#define DEFAULT_MAX_FAN_SLOT 14
static gint32 get_max_fan_slot(void)
{
    guint32 slot = DEFAULT_MAX_FAN_SLOT;

    if (!dfl_get_object_count("SMMFANClass", &slot)) {
        return slot;
    }
    return DEFAULT_MAX_FAN_SLOT;
}


static gint32 get_fan_state(gint32 slot, GSList **output)
{
    gint32 ret;
    GSList *input = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    input = g_slist_append(input, g_variant_new_int32(slot));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, username, ip, COOLINGCLASS, CLASS_NAME_COOLING_APP,
        METHOD_COOLING_FAN_STATE, input, output);

    uip_free_gvariant_list(input);

    return ret;
}

static gint32 get_fan_speed(gint32 slot)
{
    GSList *output = NULL;
    gint32 ret;
    gint32 speed = 0;
    gint32 present = 0;

    ret = get_fan_state(slot, &output);
    if (ret == RET_OK) {
        speed = g_variant_get_int32((GVariant *)g_slist_nth_data(output, 1));
        present = g_variant_get_int32((GVariant *)g_slist_nth_data(output, 2));

        g_printf("fantray%d:", slot);
        if (present) {
            if (speed >= 0 && speed <= 100) {
                g_printf("The speed of fan is %d%%.\n", speed);
            } else {
                g_printf("Get fan speed failed.\n");
            }
        } else {
            g_printf("Fantray is not present.\n");
        }
    } else {
        g_printf("Failed to get fan state of fantray%d.\n", slot);
    }

    uip_free_gvariant_list(output);

    return RET_OK;
}

gint32 get_pem_presence(gint32 slot)
{
    gint32 ret;
    OBJ_HANDLE object_handle = 0;
    guint8 presence = 0;

    ret = dal_get_specific_object_byte(CLASS_POWER_MGNT, PROPERTY_POWER_MGNT_SLOT, (guchar)(slot - 1), &object_handle);
    if (ret != RET_OK) {
        g_printf("Get pem presence failed.\r\n");
        return 0;
    }

    
    ret = dal_get_property_value_byte(object_handle, PROPERTY_POWER_MGNT_PRESENCE, (guchar *)&presence);
    if (ret != RET_OK) {
        return 0;
    }
    return presence;
}

static gint32 get_fan_presence(gint32 slot)
{
    GSList *output = NULL;
    gint32 ret;
    gint32 present = 0;

    ret = get_fan_state(slot, &output);
    if (ret == RET_OK) {
        present = g_variant_get_int32((GVariant *)g_slist_nth_data(output, 2));
        if (present) {
            g_printf("fantray%d is present.\n", slot);
        } else {
            g_printf("fantray%d is not present.\n", slot);
        }
    } else {
        g_printf("Failed to get fan state of fantray%d.\n", slot);
    }

    uip_free_gvariant_list(output);

    return RET_OK;
}


static gint32 get_shelf_fan_speed(gint32 max_fan_slot)
{
    gint32 i;

    for (i = 1; i <= max_fan_slot; i++) {
        get_fan_speed(i);
    }
    return RET_OK;
}


gint32 smm_get_fan_speed(guchar priv, const gchar *role, guchar argc, gchar **argv, gchar *target)
{
    glong slot = 0;
    gint32 max_fan_slot = get_max_fan_slot();

    if ((argc != 0)) {
        g_printf("Usage: ipmcget -l fantray/fantrayN -d fanspeed \r\n");
        return FALSE;
    }

    if (ipmc_privilege_judge(priv, role) == FALSE) {
        g_printf("permission denied.\n");
        return FALSE;
    }
    // Get shelf fan speed.
    if (!strcmp(target, "fantray")) {
        if (get_shelf_fan_speed(max_fan_slot)) {
            g_printf("Failed to get shelf fan state.\n");
            return FALSE;
        } else {
            return TRUE;
        }
        // Get fan speed by slot.
    } else if (!strncmp(target, "fantray", strlen("fantray"))) {
        if (str_to_long(&slot, target + strlen("fantray"), 1, max_fan_slot)) {
            g_printf("Invalid fantray.\n");
            return FALSE;
        }
        if (get_fan_speed(slot)) {
            return FALSE;
        } else {
            return TRUE;
        }
    } else {
        g_printf("Parameter error.\n");
        return FALSE;
    }
}

gint32 smm_get_fan_log(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 iRet;
    gchar file_path[MAX_FILE_NAME_LEN + 1] = { 0 };
    gchar file_name[MAX_FILE_NAME_LEN + 1] = { 0 };
    gchar real_path[MAX_FILE_NAME_LEN + 1] = { 0 };
    OBJ_HANDLE obj_handle = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    if (dal_get_object_handle_nth(CLASS_LOG, 0, &obj_handle) != RET_OK) {
        g_printf("Get Log object failed.\r\n");
        return FALSE;
    }

    // 读取文件路径
    (void)dal_get_property_value_string(obj_handle, PROPERTY_LOG_DIR, file_path, sizeof(file_path));
    (void)dal_get_property_value_string(obj_handle, PROPERTY_LOG_OPERATELOG_FILE, file_name, sizeof(file_name));

    if (strlen(file_path) + strlen(file_name) >= MAX_FILE_NAME_LEN - 2) {
        return FALSE;
    }

    
    iRet = snprintf_s(real_path, sizeof(real_path), sizeof(real_path) - 1, "%s/%s", file_path, FAN_LOG_NAME);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    
    return ipmc_print_log(real_path);
}

static gint32 presence_counter;
static gint32 foreach_get_blade_presence(OBJ_HANDLE object_handle, gpointer user_data)
{
    guchar presence = 0;
    gint32 ret;
    ret = dal_get_property_value_byte(object_handle, PROPERTY_FAN_PRESENT, &presence);
    if (ret != RET_OK) {
        g_printf("get fantray%d present status fail. ret:%d.\r\n", presence_counter, ret);
        presence_counter++;
        return DFL_OK;
    }
    if (presence) {
        g_printf("fantray%d\r\n", presence_counter);
    }
    presence_counter++;
    return DFL_OK;
}

gint32 smm_get_fan_presence(guchar priv, const gchar *role, guchar argc, gchar **argv, gchar *target)
{
    glong slot = 0;
    gint32 max_fan_slot = get_max_fan_slot();

    if ((argc != 0)) {
        g_printf("Usage: ipmcget -l fantrayN -d presence \r\n");
        return FALSE;
    }

    if (ipmc_privilege_judge(priv, role) == FALSE) {
        g_printf("permission denied.\n");
        return FALSE;
    }
    // Get shelf fan speed.
    if (!strcmp(target, "fantray")) {
        presence_counter = 1;
        g_printf("List Present Fan:\r\n");
        (void)dfl_foreach_object(CLASS_NAME_SMM_FAN, foreach_get_blade_presence, NULL, NULL);
    }

    if (!strncmp(target, "fantray", strlen("fantray"))) {
        if (str_to_long(&slot, target + strlen("fantray"), 1, max_fan_slot)) {
            return FALSE;
        }
        if (get_fan_presence(slot)) {
            return FALSE;
        } else {
            return TRUE;
        }
    } else {
        g_printf("Parameter error.\n");
        return FALSE;
    }
}

LOCAL gint32 foreach_print_fan_version(OBJ_HANDLE object_handle, gpointer user_data)
{
    guchar status = 0;
    guchar type = 0;
    guchar PCB_version = 0;
    guchar soft_version = 0;

    (void)dal_get_property_value_byte(object_handle, FAN_F_PROPERTY_STATUS, &status);
    (void)dal_get_property_value_byte(object_handle, FAN_PROPERTY_TYPE, &type);
    (void)dal_get_property_value_byte(object_handle, PROPERTY_FAN_PCB_VERSION, &PCB_version);
    (void)dal_get_property_value_byte(object_handle, PROPERTY_FAN_SOFT_VERSION, &soft_version);

    if (status == 6) { // refer to RF_STATE_ABSENT
        g_printf("%s: Not present\r\n", dfl_get_object_name(object_handle));
    } else if (status == 11) { // refer to RF_STATE_UNAVAILABLE_OFFLINE
        g_printf("%s: Error connection\r\n", dfl_get_object_name(object_handle));
    } else {
        g_printf("%s:\r\n", dfl_get_object_name(object_handle));
        g_printf("    PCB Version       :%s \r\n",
            PCB_version == 0x00 ? "A" : (PCB_version == 0x01 ? "B" : (PCB_version == 0x02 ? "C" : "unknown")));
        g_printf("    Software Version  :%u \r\n", soft_version);
        g_printf("    Type              :%s \r\n",
            type == 0x00 ? "2U" : (type == 0x01 || type == 0x11 ? "1U" : "unknown"));
    }

    return DFL_OK;
}

gint32 smm_get_fan_version(guchar priv, const gchar *role, guchar argc, gchar **argv, gchar *target)
{
    if ((argc != 0)) {
        g_printf("Usage: ipmcget -l fantray -d version \r\n");
        return FALSE;
    }

    if (ipmc_privilege_judge(priv, role) == FALSE) {
        g_printf("Permission denied.\n");
        return FALSE;
    }
    // Get shelf fan speed.
    if (!strcmp(target, "fantray")) {
        presence_counter = 1;
        g_printf("Fantray Version Information:\r\n");
        (void)dfl_foreach_object(CLASS_NAME_SMM_FAN, foreach_print_fan_version, NULL, NULL);
    } else {
        g_printf("Usage: ipmcget -l fantray -d version \r\n");
    }

    return TRUE;
}


gint32 smm_get_fan_ctl_mode(guchar priv, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 mode = 0;
    gchar username[PARAMETER_LEN + 1] = { 0 };
    gchar ip[PARAMETER_LEN + 1] = { 0 };

    if ((argc != 0)) {
        g_printf("Usage: ipmcget -l fantray -d fancontrolmode \r\n");
        return FALSE;
    }

    if (strcmp(target, "fantray")) {
        g_printf("Usage: ipmcget -l fantray -d fancontrolmode \r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    ret = dal_get_object_handle_nth(COOLINGCLASS, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get object handle failed: ret %d.", ret);
        g_printf("get fancontrolmode fail.\r\n");
        return ret;
    }

    ret = dal_get_property_value_byte(obj_handle, COOLING_PROPERTY_MODE, &mode);
    if (ret == RET_OK) {
        g_printf("%s\n", (mode == AUTO_FAN_MODE) ? "Auto" : "Manual");
    } else {
        debug_log(DLOG_ERROR, "get object handle failed: ret %d.", ret);
        g_printf("get fancontrolmode fail.\r\n");
    }
    return ret;
}

LOCAL gint32 foreach_print_fan_fru_info(OBJ_HANDLE object_handle, gpointer user_data)
{
    gint32 ret;
    guint8 type = 0;
    glong slot = *(glong *)user_data;
    guint32 read_position = 0;

    ret = dal_get_property_value_byte(object_handle, PROPERTY_FRU_TYPE, &type);
    if (ret != RET_OK) {
        g_printf("Get Fan%ld FRU type error.\r\n", slot);
        return DFL_OK;
    }

    ret = dal_get_property_value_uint32(object_handle, PROPERTY_FRU_POSITION, &read_position);
    if (ret != RET_OK) {
        g_printf("Get Fan%ld FRU position error.\r\n", slot);
        return DFL_OK;
    }
    
    if (type == DEVICE_TYPE_FAN_BOARD && (read_position - 1 == slot || slot == 0)) {
        (void)print_fru_info(object_handle);
    }
    return DFL_OK;
}


LOCAL gint32 print_fan_fru_info(glong slot)
{
    (void)dfl_foreach_object(CLASS_FRU, foreach_print_fan_fru_info, &slot, NULL);
    return DFL_OK;
}

gint32 smm_get_fan_fru(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target,
    gchar *location)
{
    gint32 ret;
    glong slot = 0;
    OBJ_HANDLE fan_object = 0;
    guchar presence = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget -l fantray[1-N] -t fru -d show \r\n");
        return FALSE;
    }

    if (!strcmp(location, "fantray")) {
        presence_counter = 1;
        g_printf("List Fan Fru info:\r\n");

        ret = print_fan_fru_info(0);
        if (ret != RET_OK) {
            g_printf("Get shelf fru information failed\r\n");
            return FALSE;
        }
    } else if (!strncmp(location, "fantray", strlen("fantray"))) {
        if (str_to_long(&slot, location + strlen("fantray"), 1, get_max_fan_slot())) {
            return FALSE;
        }

        
        ret = dal_get_specific_object_byte(CLASS_NAME_SMM_FAN, PROPERTY_FAN_SLOT, (guint8)slot, &fan_object);
        if (ret != RET_OK) {
            g_printf("Get Fan%ld object error.\r\n", slot);
            return DFL_OK;
        }

        ret = dal_get_property_value_byte(fan_object, PROPERTY_FAN_PRESENT, &presence);
        if (ret != RET_OK) {
            g_printf("Get Fan%ld presence error.\r\n", slot);
            return DFL_OK;
        }

        if (!presence) {
            
            g_printf("Fan%ld is not present\r\n", slot);
            return DFL_OK;
        }

        ret = print_fan_fru_info(slot);
        if (ret != RET_OK) {
            g_printf("Get Fan fru information failed\r\n");
            return FALSE;
        }
    } else {
        g_printf("Usage: ipmcget -l fantray[1-N] -t fru -d show \r\n");
        return FALSE;
    }
    return TRUE;
}

gint32 smm_get_shelf_fru_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target,
    gchar *location)
{
    gint32 ret;
    OBJ_HANDLE handle;
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget -l shelf -t fru -d shelfall\r\n");
        return FALSE;
    }
    ret = dal_get_specific_object_uint32(CLASS_FRU, PROPERTY_POSITION, SMM_BACK_PLANE_POSITION, &handle);
    if (ret != RET_OK) {
        g_printf("Get shelf fru information failed.\r\n");
        return FALSE;
    }
    ret = print_fru_info(handle);
    if (ret != RET_OK) {
        g_printf("Get shelf fru information failed\r\n");
        return FALSE;
    }
    return TRUE;
}

gint32 smm_get_chassis_id(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE handle;
    guint32 id;
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget -l shelf -d chassisid\r\n");
        return FALSE;
    }
    ret = dfl_get_object_handle(OBJECT_NAME_CHASSIS, &handle);
    if (ret != DFL_OK) {
        g_printf("Get chassis id failed.\r\n");
        return FALSE;
    }
    ret = dal_get_property_value_uint32(handle, PROPERTY_CHASSIS_CHASSIS_NUM, &id);
    if (ret != RET_OK) {
        g_printf("Get chassis id failed.\r\n");
        return FALSE;
    }
    g_printf("Shelf Number : %u\r\n", id);
    return TRUE;
}
gint32 smm_get_chassis_name(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE handle;
    gchar name[CHASSIS_NAME_LEN + 1] = { 0 };
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget -l shelf -d chassisname\r\n");
        return FALSE;
    }
    ret = dfl_get_object_handle(OBJECT_NAME_CHASSIS, &handle);
    if (ret != DFL_OK) {
        g_printf("Get chassis name failed.\r\n");
        return FALSE;
    }
    ret = dal_get_property_value_string(handle, PROPERTY_CHASSIS_CHASSIS_NAME, name, CHASSIS_NAME_LEN + 1);
    if (ret != RET_OK) {
        g_printf("Get chassis name failed.\r\n");
        return FALSE;
    }
    g_printf("Shelf Name : %s\r\n", name);
    return TRUE;
}
gint32 smm_get_chassis_location(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE handle;
    gchar location[CHASSIS_LOCATION_LEN + 1] = { 0 };
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget -l shelf -d location\r\n");
        return FALSE;
    }
    ret = dfl_get_object_handle(OBJECT_NAME_CHASSIS, &handle);
    if (ret != DFL_OK) {
        g_printf("Get chassis location failed.\r\n");
        return FALSE;
    }
    ret = dal_get_property_value_string(handle, PROPERTY_CHASSIS_LOCATION, location, CHASSIS_LOCATION_LEN + 1);
    if (ret != RET_OK) {
        g_printf("Get chassis location failed.\r\n");
        return FALSE;
    }
    g_printf("Shelf Location : %s\r\n", location);
    return TRUE;
}



gint32 smm_get_chassis_version(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    OBJ_HANDLE fru_handle = 0;
    OBJ_HANDLE connect_handle = 0;
    guint32 board_id = 0;
    guint8 pcb_id = 0;
    gint32 ret;
    gchar *board_type[] = {
        "IT11BKPA",
        "IT11BKPB",
        "IT11TBPA"
    };
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget -l shelf -d version\r\n");
        return FALSE;
    }
    
    ret = dfl_get_object_handle(OBJECT_NAME_BACK_PLANE_CONNECT, &connect_handle);
    if (ret != DFL_OK) {
        g_printf("Get shelf version failed.\r\n");
        return FALSE;
    }
    ret = dal_get_property_value_uint32(connect_handle, PROPERTY_ID, &board_id);
    if (ret != RET_OK) {
        g_printf("Get shelf version failed.\r\n");
        return FALSE;
    }
    
    ret = dal_get_specific_object_uint32(CLASS_FRU, PROPERTY_POSITION, SMM_BACK_PLANE_POSITION, &fru_handle);
    if (ret != RET_OK) {
        g_printf("Get shelf version failed.\r\n");
        return FALSE;
    }
    ret = dal_get_property_value_byte(fru_handle, PROPERTY_PCB_ID, &pcb_id);
    if (ret != RET_OK) {
        g_printf("Get shelf version failed.\r\n");
        return FALSE;
    }
    
    switch (board_id) {
        case 0x00:
            board_id = 0;
            break;
        case 0x01:
            board_id = 1;
            break;
        case 0x03:
            board_id = 2;
            break;
        default:
            board_id = 0;
            break;
    }
    g_printf("Shelf Version Information : \r\nBkplane type\t:%s\r\nPCB Version\t:.%c\r\n", board_type[board_id],
        pcb_id + 0x40);
    return TRUE;
}


gint32 print_single_smm_health_event(GSList *event_msg, gpointer data)
{
    errno_t safe_fun_ret = EOK;
    guint32 event_time;
    guint8 event_level;
    guint32 event_code;
    guint16 subject_id;
    const gchar *event_desc_ptr = NULL;
    gint32 ret;
    guint32 *index = (guint32 *)data;
    guint32 code_filter = ((guint32 *)data)[1];
    guint32 code_mask = ((guint32 *)data)[2];
    guint32 id_filter = ((guint32 *)data)[3];

    gchar event_code_str[256] = { 0 };
    gchar event_desc[EVENT_DESC_LENGTH] = { 0 };
    gchar event_time_str[MAX_SEL_TIME_LEN] = { 0 };

    
    event_code = g_variant_get_uint32((GVariant *)g_slist_nth_data(event_msg, 5));
    if (code_filter == 0) { // 选出本板事件
        if ((event_code & code_mask) == PEM_EVENT_CODE ||
            (event_code & code_mask) == FANTRAY_EVENT_CODE) { // 筛选掉电源风扇事件
            return RET_OK;
        }
    } else if ((event_code & code_mask) != code_filter) { // 只选择风扇 或 电源事件
        return RET_OK;
    }
    
    subject_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(event_msg, 15));
    if (id_filter != 0 && id_filter != subject_id) {
        return RET_OK;
    }
    
    event_time = g_variant_get_uint32((GVariant *)g_slist_nth_data(event_msg, 3));
    event_level = g_variant_get_byte((GVariant *)g_slist_nth_data(event_msg, 4));
    event_desc_ptr = g_variant_get_string((GVariant *)g_slist_nth_data(event_msg, 6), NULL);
    if (event_desc_ptr != NULL) {
        safe_fun_ret = strncpy_s(event_desc, sizeof(event_desc), event_desc_ptr, sizeof(event_desc) - 1);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    } else {
        strncpy_s(event_desc, sizeof(event_desc), "Unknown", strlen("Unknown"));
    }

    memset_s(event_time_str, MAX_SEL_TIME_LEN, 0, MAX_SEL_TIME_LEN);
    ret = uip_parse_sel_time(event_time, event_time_str, sizeof(event_time_str));
    if (ret != RET_OK) {
        g_printf("Parse event time failed.\r\n ");
        return RET_ERR;
    }
    snprintf_s(event_code_str, sizeof(event_code_str), sizeof(event_code_str) - 1, "0x%08X", event_code);
    g_printf("%-10d | %-20s | %-12s | %-12s | %s\r\n", ++(*index), event_time_str,
        (event_level > EVENT_ALARM_LEVEL_NUM - 1) ? "Unknown" : event_alarm_level[event_level], event_code_str,
        event_desc);
    return RET_OK;
}


gint32 print_single_blade_health_event(const ALARM_EVENT_MSG *msg, const guint8 *msg_data, gpointer data)
{
    errno_t safe_fun_ret;
    guint32 *index = (guint32 *)data;
    gint32 ret;

    guint32 msg_data_length = msg->data_length;
    gchar event_time_str[MAX_SEL_TIME_LEN] = { 0 };
    gchar event_code_str[256] = { 0 };
    gchar event_entity_name[MAX_NAME_SIZE] = { 0 };
    guint8 event_entity_name_length;
    gchar event_desc[EVENT_DESC_LENGTH] = { 0 };
    gchar *event_desc_formated = NULL;
    guint16 event_desc_length;
    // 事件主体
    event_entity_name_length = msg_data[0];
    msg_data_length -= sizeof(event_entity_name_length);
    msg_data += sizeof(event_entity_name_length);
    
    safe_fun_ret = memcpy_s(event_entity_name, sizeof(event_entity_name) - 1, msg_data, event_entity_name_length);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    
    msg_data_length -= event_entity_name_length;
    msg_data += event_entity_name_length;
    // 事件描述
    event_desc_length = msg_data[0];
    event_desc_length += (msg_data[1] << 8);
    msg_data_length -= sizeof(event_desc_length);
    msg_data += sizeof(event_desc_length);
    
    safe_fun_ret = memcpy_s(event_desc, sizeof(event_desc) - 1, msg_data, event_desc_length);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    
    // 格式化事件描述
    alarm_event_msg_format(event_desc, &event_desc_formated);
    // 产生时间
    ret = uip_parse_sel_time(msg->event_timestamp, event_time_str, sizeof(event_time_str));
    if (ret != RET_OK) {
        g_printf("Parse event time failed.\r\n");
        g_free(event_desc_formated);
        return FALSE;
    }
    // 事件码
    snprintf_s(event_code_str, sizeof(event_code_str), sizeof(event_code_str) - 1, "0x%08X", msg->event_code);
    // 打印事件信息
    g_printf("%-10d | %-20s | %-12s | %-12s | %s\r\n", ++(*index), event_time_str,
        (msg->event_severity >= EVENT_ALARM_LEVEL_NUM) ? "Unknown" : event_alarm_level[msg->event_severity],
        event_code_str, event_desc_formated);
    g_free(event_desc_formated);

    return RET_OK;
}

LOCAL gint32 print_each_blade_health_event(OBJ_HANDLE handle, gpointer data)
{
    guint8 blade_type = 0;
    guint8 ipmb_addr = 0;
    gint32 ret = 0;
    guint32 *index = (guint32 *)data;
    guint32 *has_msg = &((guint32 *)data)[1];
    guint32 tmp_data[3] = { 0 };
    const gchar *blade_name = NULL;

    (void)dal_get_property_value_byte(handle, PROTERY_IPMBETH_BLADE_BLADETYPE, &blade_type);
    if (blade_type == 0 || blade_type == 0xc0) { // blade or switch
        (void)dal_get_property_value_byte(handle, IPMB_PROPERTY_SLAVE_ADDR, &ipmb_addr);
        blade_name = dfl_get_object_name(handle);
        *index = 0;
        tmp_data[0] = 0;
        tmp_data[1] = 0;
        tmp_data[2] = 0;
        ret = foreach_blade_health_event(count_single_blade_health_event, ipmb_addr, tmp_data);
        if (ret != RET_OK) {
            g_printf("Get %s health status failed.\r\n", blade_name);
            return RET_OK;
        }
        if (tmp_data[0] != 0 || tmp_data[1] != 0 || tmp_data[2] != 0) { // 存在告警
            *has_msg = 1;
            g_printf("%s:\r\n", blade_name);
            g_printf("%-10s | %-20s | %-12s | %-12s | %s\r\n", "Event Num", "Event Time", "Alarm Level", "Event Code",
                "Event Description");
            ret = foreach_blade_health_event(print_single_blade_health_event, ipmb_addr, data);
            if (ret != RET_OK) {
                return RET_ERR;
            }
            g_printf("\r\n");
        }
    }
    return RET_OK;
}

LOCAL gint32 count_each_blade_health_event(OBJ_HANDLE handle, gpointer data)
{
    guint8 blade_type = 0;
    guint8 ipmb_addr = 0;
    gint32 ret = 0;

    (void)dal_get_property_value_byte(handle, PROTERY_IPMBETH_BLADE_BLADETYPE, &blade_type);
    if (blade_type == 0 || blade_type == 0xc0) { // blade or switch
        (void)dal_get_property_value_byte(handle, IPMB_PROPERTY_SLAVE_ADDR, &ipmb_addr);
        ret = foreach_blade_health_event(count_single_blade_health_event, ipmb_addr, (gpointer)data);
        if (ret != RET_OK) {
            return RET_ERR;
        }
    }
    return RET_OK;
}

gint32 smm_get_fantray_health_event(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    errno_t safe_fun_ret;
    gint32 ret;
    guint32 data[6] = { 0 };
    guint32 id;
    GSList *output_list = NULL;
    gint32 presence = 0;
    gchar fantray_name[MAX_NAME_SIZE] = { 0 };

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget -l <fantray|fantrayN> -d healthevents\r\n");
        return FALSE;
    }

    safe_fun_ret = strncpy_s(fantray_name, sizeof(fantray_name), target, sizeof(fantray_name) - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    fantray_name[0] = g_ascii_toupper(fantray_name[0]);
    if (strcmp(target, "fantray") == 0) { // 打印所有fantray健康事件
        id = 0;
    } else { // fantrayN健康事件
        id = get_fantray_num(target);
        ret = get_fan_state(id, &output_list);
        if (ret != RET_OK) {
            g_printf("Get %s present state failed.\r\n", target);
            uip_free_gvariant_list(output_list);
            return FALSE;
        }
        presence = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 2));
        uip_free_gvariant_list(output_list);
        if (presence != BLADE_PRESENCE) {
            g_printf("%s is not present.\r\n", fantray_name);
            return TRUE;
        }
    }
    
    data[3] = FANTRAY_EVENT_CODE;
    data[4] = 0xff000000;
    data[5] = id;
    ret = foreach_smm_health_event(count_single_smm_health_event, (gpointer)data);
    if (ret != RET_OK) {
        g_printf("Get %s health status failed.\r\n", target);
        return FALSE;
    }
    if (data[0] == 0 && data[1] == 0 && data[2] == 0) {
        g_printf("%s in health state.\r\n", fantray_name);
        return TRUE;
    }
    
    g_printf("%-10s | %-20s | %-12s | %-12s | %s\r\n", "Event Num", "Event Time", "Alarm Level", "Event Code",
        "Event Description");
    data[1] = FANTRAY_EVENT_CODE;
    data[2] = 0xff000000;
    data[3] = id;
    ret = foreach_smm_health_event(print_single_smm_health_event, (gpointer)data);
    if (ret != RET_OK) {
        g_printf("Get %s health event failed.\r\n", target);
        return FALSE;
    }
    return TRUE;
}

gint32 smm_get_fantray_health_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    errno_t safe_fun_ret;
    gint32 ret;
    guint32 data[6] = { 0 };
    GSList *output_list = NULL;
    gint32 presence = 0;
    gchar fantray_name[MAX_NAME_SIZE] = { 0 };

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget -l <fantray|fantrayN> -d health\r\n");
        return FALSE;
    }
    safe_fun_ret = strncpy_s(fantray_name, sizeof(fantray_name), target, sizeof(fantray_name) - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    fantray_name[0] = g_ascii_toupper(fantray_name[0]);
    data[3] = FANTRAY_EVENT_CODE;
    data[4] = 0xff000000;
    if (strcmp(target, "fantray") == 0) { // 打印所有fantray健康事件
        data[5] = 0;
    } else { // 打印单个fantray健康事件
        data[5] = get_fantray_num(target);
        ret = get_fan_state(data[5], &output_list);
        if (ret != RET_OK) {
            g_printf("Get %s present state failed.\r\n", target);
            uip_free_gvariant_list(output_list);
            return FALSE;
        }
        presence = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 2));
        uip_free_gvariant_list(output_list);
        if (presence != BLADE_PRESENCE) {
            g_printf("%s is not present.\r\n", fantray_name);
            return TRUE;
        }
    }
    
    ret = foreach_smm_health_event(count_single_smm_health_event, (gpointer)data);
    if (ret != RET_OK) {
        g_printf("Get %s health status failed.\r\n", target);
        return FALSE;
    }
    if (data[0] == 0 && data[1] == 0 && data[2] == 0) {
        g_printf("%s in health state.\r\n", fantray_name);
        return TRUE;
    }
    g_printf("%s Events:\r\n", fantray_name);
    if (data[0] != 0) { // 一般告警数目
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

gint32 smm_get_shelf_health_event(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    guint8 ipmb_addr = 0;
    gint32 ret;
    guint32 data[6] = { 0 };
    OBJ_HANDLE warning_handle = 0;
    OBJ_HANDLE shelf_manage_handle = 0;
    guint8 health = 0;
    gboolean has_msg = FALSE;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget -l shelf -d healthevents\r\n");
        return FALSE;
    }
    
    ret = dal_get_object_handle_nth(CLASS_NAME_SHELF_MANAGE, 0, &shelf_manage_handle);
    if (ret != RET_OK) {
        g_printf("Get chassis health state failed.\r\n");
        return FALSE;
    }
    ret = dal_get_object_handle_nth(CLASS_WARNING, 0, &warning_handle);
    if (ret != RET_OK) {
        g_printf("Get chassis health state failed.\r\n");
        return FALSE;
    }
    (void)dal_get_property_value_byte(shelf_manage_handle, PROPERTY_SHELF_HEALTH, &health);
    if (health == 0) {
        (void)dal_get_property_value_byte(warning_handle, PROPERTY_WARNING_HEALTH_STATUS, &health);
    }
    if (health == 0) {
        g_printf("Chassis in health state.\r\n");
        return TRUE;
    }

    // Smm
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0xff000000;
    data[5] = 0;
    ret = foreach_smm_health_event(count_single_smm_health_event, data);
    if (ret != RET_OK) {
        g_printf("Get smm health status failed.\r\n");
        return FALSE;
    }
    if (data[0] != 0 || data[1] != 0 || data[2] != 0) {
        has_msg = TRUE;
        data[0] = 0;
        data[1] = 0;          // code_filter
        data[2] = 0xff000000; // code_mask
        data[3] = 0;
        g_printf("SMM:\r\n");
        g_printf("%-10s | %-20s | %-12s | %-12s | %s\r\n", "Event Num", "Event Time", "Alarm Level", "Event Code",
            "Event Description");
        ret = foreach_smm_health_event(print_single_smm_health_event, &data);
        if (ret != RET_OK) {
            g_printf("Get smm health event failed.\r\n");
            return FALSE;
        }
        g_printf("\r\n");
    }
    // OtherSmm
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    ret = get_other_smm_slave_addr(&ipmb_addr);
    if (ret != RET_OK) {
        g_printf("Get other smm health evetn failed.\r\n");
        return FALSE;
    }
    ret = foreach_blade_health_event(count_single_blade_health_event, ipmb_addr, data);
    if (ret != RET_OK) {
        g_printf("Get other smm health status failed.\r\n");
        return FALSE;
    }
    if (data[0] != 0 || data[1] != 0 || data[2] != 0) {
        has_msg = TRUE;
        data[0] = 0;
        g_printf("otherSMM:\r\n");
        g_printf("%-10s | %-20s | %-12s | %-12s | %s\r\n", "Event Num", "Event Time", "Alarm Level", "Event Code",
            "Event Description");
        ret = foreach_blade_health_event(print_single_blade_health_event, ipmb_addr, &data);
        if (ret != RET_OK) {
            g_printf("Get other smm health event failed.\r\n");
            return FALSE;
        }
        g_printf("\r\n");
    }
    // pem
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = PEM_EVENT_CODE;
    data[4] = 0xff000000;
    data[5] = 0;
    ret = foreach_smm_health_event(count_single_smm_health_event, data);
    if (ret != RET_OK) {
        g_printf("Get pem health status failed.\r\n");
        return FALSE;
    }
    if (data[0] != 0 || data[1] != 0 || data[2] != 0) {
        has_msg = TRUE;
        data[0] = 0;
        data[1] = PEM_EVENT_CODE; // code_filter
        data[2] = 0xff000000;     // code_mask
        data[3] = 0;
        g_printf("pem:\r\n");
        g_printf("%-10s | %-20s | %-12s | %-12s | %s\r\n", "Event Num", "Event Time", "Alarm Level", "Event Code",
            "Event Description");
        ret = foreach_smm_health_event(print_single_smm_health_event, &data);
        if (ret != RET_OK) {
            g_printf("Get pem health event failed.\r\n");
            return FALSE;
        }
        g_printf("\r\n");
    }
    // fantray
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = FANTRAY_EVENT_CODE;
    data[4] = 0xff000000;
    data[5] = 0;
    ret = foreach_smm_health_event(count_single_smm_health_event, data);
    if (ret != RET_OK) {
        g_printf("Get fantray health status failed.\r\n");
        return FALSE;
    }
    if (data[0] != 0 || data[1] != 0 || data[2] != 0) {
        has_msg = TRUE;
        data[0] = 0;
        data[1] = FANTRAY_EVENT_CODE; // code_filter
        data[2] = 0xff000000;         // code_mask
        data[3] = 0;
        g_printf("fantray:\r\n");
        g_printf("%-10s | %-20s | %-12s | %-12s | %s\r\n", "Event Num", "Event Time", "Alarm Level", "Event Code",
            "Event Description");
        ret = foreach_smm_health_event(print_single_smm_health_event, &data);
        if (ret != RET_OK) {
            g_printf("Get fantray health event failed.\r\n");
            return FALSE;
        }
        g_printf("\r\n");
    }
    // blade
    data[1] = 0;
    ret = dfl_foreach_object(CLASS_NAME_IPMBETH_BLADE, print_each_blade_health_event, (gpointer)&data, NULL);
    if (ret != DFL_OK) {
        g_printf("Get blade health event failed.\r\n");
        return FALSE;
    }
    if (data[1]) {
        has_msg = TRUE;
    }
    if (!has_msg) { // 若遍历完，没有消息可打印
        g_printf("Chassis in health state.\r\n");
    }
    return TRUE;
}

gint32 smm_get_shelf_health_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    guint8 ipmb_addr = 0;
    gint32 ret;
    guint32 data[6] = { 0 };

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget -l shelf -d health\r\n");
        return FALSE;
    }
    // Smm
    data[3] = 0;
    data[4] = 0;
    data[5] = 0;
    ret = foreach_smm_health_event(count_single_smm_health_event, data);
    if (ret != RET_OK) {
        g_printf("Get smm health status failed.\r\n");
        return FALSE;
    }
    // OtherSmm
    ret = get_other_smm_slave_addr(&ipmb_addr);
    if (ret != RET_OK) {
        g_printf("Get other smm health status failed.\r\n");
        return FALSE;
    }
    ret = foreach_blade_health_event(count_single_blade_health_event, ipmb_addr, data);
    if (ret != RET_OK) {
        g_printf("Get other smm health status failed.\r\n");
        return FALSE;
    }
    // Blade
    ret = dfl_foreach_object(CLASS_NAME_IPMBETH_BLADE, count_each_blade_health_event, (gpointer)data, NULL);
    if (ret != DFL_OK) {
        g_printf("Get blade health status failed.\r\n");
        return FALSE;
    }
    // 打印
    if (data[0] == 0 && data[1] == 0 && data[2] == 0) {
        g_printf("Chassis in health state.\r\n");
        return TRUE;
    }
    g_printf("Chassis Events:\r\n");
    if (data[0] != 0) {
        g_printf("%u minor events.\r\n", data[0]);
    }
    if (data[1] != 0) {
        g_printf("%u major events.\r\n", data[1]);
    }
    if (data[2] != 0) {
        g_printf("%u critical events.\r\n", data[2]);
    }
    return TRUE;
}


LOCAL void add_unhealthy_location_to_list(guint32 health_data[], size_t data_len, GSList *unhealthy_location[],
    const gchar *location)
{
    guint32 i;
    if (location == NULL) {
        return;
    }
    if (data_len == 0) {
        return;
    }
    for (i = 0; i < EVENT_ALARM_LEVEL_COUNT; i++) {
        if (health_data[i] != 0) {
            unhealthy_location[i] = g_slist_append(unhealthy_location[i], g_variant_new_string(location));
        }
    }
}

LOCAL gint32 check_each_blade_health_state(OBJ_HANDLE handle, gpointer data)
{
    GSList **unhealthy_location = (GSList **)data;
    guint8 blade_type = 0;
    guint8 ipmb_addr = 0;
    gint32 ret = 0;
    guint32 count_data[3] = { 0 };
    size_t len = 0;

    (void)dal_get_property_value_byte(handle, PROTERY_IPMBETH_BLADE_BLADETYPE, &blade_type);
    if (blade_type == 0 || blade_type == 0xc0) { // blade or switch
        (void)dal_get_property_value_byte(handle, IPMB_PROPERTY_SLAVE_ADDR, &ipmb_addr);
        ret = foreach_blade_health_event(count_single_blade_health_event, ipmb_addr, (gpointer)count_data);
        if (ret != RET_OK) {
            return RET_ERR;
        }
        len = sizeof(count_data) / sizeof(guint32);
        add_unhealthy_location_to_list(count_data, len, unhealthy_location, dfl_get_object_name(handle));
    }
    return RET_OK;
}

gint32 smm_get_shelf_unhealthy_location(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    guint32 data[6] = { 0 };
    gint32 ret;
    guint8 ipmb_addr = 0;
    GSList *unhealthy_location[EVENT_ALARM_LEVEL_COUNT] = { 0 }; // 0 minor; 1 major; 2 critical
    GSList *cur_node = NULL;
    guint32 i;
    size_t len;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget -l shelf -d unhealthylocations\r\n");
        return FALSE;
    }
    // Smm
    data[3] = 0;
    data[4] = 0xff000000;
    data[5] = 0;
    ret = foreach_smm_health_event(count_single_smm_health_event, data);
    if (ret != RET_OK) {
        g_printf("Get smm health event failed.\r\n");
        return FALSE;
    }
    len = sizeof(data) / sizeof(guint32);
    add_unhealthy_location_to_list(data, len, unhealthy_location, "SMM");
    // OtherSmm
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = 0;
    ret = get_other_smm_slave_addr(&ipmb_addr);
    if (ret != RET_OK) {
        g_printf("Get other smm health event failed.\r\n");
        return FALSE;
    }
    ret = foreach_blade_health_event(count_single_blade_health_event, ipmb_addr, data);
    if (ret != RET_OK) {
        g_printf("Get other smm health event failed.\r\n");
        return FALSE;
    }
    add_unhealthy_location_to_list(data, len, unhealthy_location, "otherSMM");
    // Pem
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = PEM_EVENT_CODE;
    data[4] = 0xff000000;
    data[5] = 0;
    ret = foreach_smm_health_event(count_single_smm_health_event, data);
    if (ret != RET_OK) {
        g_printf("Get pem health event failed.\r\n");
        return FALSE;
    }
    add_unhealthy_location_to_list(data, len, unhealthy_location, "pem");
    // Fantray
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = FANTRAY_EVENT_CODE;
    data[4] = 0xff000000;
    data[5] = 0;
    ret = foreach_smm_health_event(count_single_smm_health_event, data);
    if (ret != RET_OK) {
        g_printf("Get fantray health event failed.\r\n");
        return FALSE;
    }
    add_unhealthy_location_to_list(data, len, unhealthy_location, "fantray");
    // Blade
    (void)dfl_foreach_object(CLASS_NAME_IPMBETH_BLADE, check_each_blade_health_state, (gpointer)unhealthy_location,
        NULL);
    // 打印
    if (unhealthy_location[0] == NULL && unhealthy_location[1] == NULL && unhealthy_location[2] == NULL) {
        g_printf("Chassis in health state.\r\n");
        return RET_OK;
    }
    for (i = 0; i < EVENT_ALARM_LEVEL_COUNT; i++) {
        if (unhealthy_location[i] != NULL) {
            g_printf("%-10s:", event_alarm_level[i + 1]);
            cur_node = unhealthy_location[i];
            while (cur_node != NULL) {
                g_printf(" %s", g_variant_get_string(cur_node->data, NULL));
                cur_node = cur_node->next;
            }
            g_printf("\r\n");
        }
    }
    // 释放
    for (i = 0; i < EVENT_ALARM_LEVEL_COUNT; i++) {
        g_slist_free_full(unhealthy_location[i], (GDestroyNotify)g_variant_unref);
    }
    return TRUE;
}


gint32 smm_get_shelf_realtime_power(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE object_handle = 0;
    guint32 current_power = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -l shelf -d currentpower\r\n");
        return FALSE;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_AMMETER, 0, &object_handle);
    if (ret != RET_OK) {
        g_printf("Get shelf current power failed.\r\n");
        return FALSE;
    }

    ret = dal_get_property_value_uint32(object_handle, PROTERY_PSTOTAL_POWER, &current_power);
    if (ret != RET_OK) {
        g_printf("Get shelf current power failed.\r\n");
        return FALSE;
    }

    g_printf("Shelf current power is %d Watts.\r\n", current_power);

    return TRUE;
}


LOCAL void print_fru_extend_label(const gchar *extend_info)
{
    errno_t safe_fun_ret = EOK;
    gchar *p = NULL;
    gchar *oem_tag = NULL;
    gchar *value = NULL;
    gsize buf_len = 0;
    gsize tag_len = 0;
    gsize value_len = 0;
    const gchar *buff = extend_info;

    p = strchr(buff, '=');
    if (p == NULL) {
        g_printf("%s\r\n", buff);
    } else {
        buf_len = strlen(extend_info);
        tag_len = p - buff;
        value_len = buf_len - tag_len - 1;

        oem_tag = (gchar *)g_malloc0((tag_len + 1) * sizeof(gchar));
        if (oem_tag == NULL) {
            return;
        }
        memset_s(oem_tag, tag_len + 1, 0, tag_len + 1);
        safe_fun_ret = memcpy_s(oem_tag, tag_len + 1, buff, tag_len);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }

        value = (gchar *)g_malloc0((value_len + 1) * sizeof(gchar));
        return_do_info_if_expr(value == NULL, g_free(oem_tag));
        memset_s(value, value_len + 1, 0, value_len + 1);
        safe_fun_ret = memcpy_s(value, value_len + 1, p + 1, value_len);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }

        g_printf("%-30s: %s\r\n", oem_tag, value);

        g_free(oem_tag);
        g_free(value);
    }
}


gint32 print_blade_info(guint8 slave_addr)
{
    gint32 iRet;
    gint32 ret_val;
    gchar username[PARAMETER_LEN + 1] = { 0 };
    gchar ip[PARAMETER_LEN + 1] = { 0 };
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *board_name = NULL;
    const gchar *serial_number = NULL;
    const gchar *product_name = NULL;
    const gchar *product_sn = NULL;
    const gchar *product_version = NULL;
    const gchar *board_part_number = NULL;
    const gchar *board_fru_file_id = NULL;
    const gchar *board_mfg = NULL;
    const gchar *pro_fru_file_id = NULL;
    const gchar *product_mfg = NULL;
    const gchar *product_part_number = NULL;
    const gchar *product_asset_tag = NULL;
    const gchar **product_oem_label = NULL;
    gchar board_mfg_date[SHORT_STRING_LEN] = {0};
    gsize len = 0;
    
    gchar blade_name[OBJ_NAME_MAX_LEN] = { 0 };
    
    guint32 seconds;
    guint32 minutes;
    RTC_TIME_S date = { 0 };
    const gchar *wday[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
    guint8 i = 0;
    guint8 extend_flag = 0;
    OBJ_HANDLE obj_handle = 0;

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    if (get_ipmbeth_blade_name(slave_addr, blade_name, sizeof(blade_name)) != RET_OK) {
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(slave_addr));

    ret_val = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_NAME_IPMBETH_BLADE, blade_name, METHOD_SHELF_GET_BLADE_INFO, input_list, &output_list);
    if (ret_val != RET_OK) {
        print_blade_err_info(blade_name, ret_val);
        g_printf("Get fru info failed.\r\n");
        uip_free_gvariant_list(input_list);
        uip_free_gvariant_list(output_list);
        return FALSE;
    }

    board_name = g_variant_get_bytestring((GVariant *)g_slist_nth_data(output_list, 1));
    serial_number = g_variant_get_bytestring((GVariant *)g_slist_nth_data(output_list, 2));
    product_name = g_variant_get_bytestring((GVariant *)g_slist_nth_data(output_list, 3));
    product_sn = g_variant_get_bytestring((GVariant *)g_slist_nth_data(output_list, 4));
    product_version = g_variant_get_bytestring((GVariant *)g_slist_nth_data(output_list, 5));
    board_part_number = g_variant_get_bytestring((GVariant *)g_slist_nth_data(output_list, 6));
    
    minutes = g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, 7));
    seconds = minutes * 60;
    
    date = vos_gettime_bysecond(1996, seconds);
    iRet = snprintf_s(board_mfg_date, SHORT_STRING_LEN, SHORT_STRING_LEN - 1, "%04d/%02d/%02d %s %02d:%02d:%02d",
        date.wYear, date.ucMonth, date.ucDate, wday[date.ucDay], date.ucHours, date.ucMinutes, date.ucSecond);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    

    board_fru_file_id = g_variant_get_bytestring((GVariant *)g_slist_nth_data(output_list, 8));
    board_mfg = g_variant_get_bytestring((GVariant *)g_slist_nth_data(output_list, 9));
    pro_fru_file_id = g_variant_get_bytestring((GVariant *)g_slist_nth_data(output_list, 10));
    product_mfg = g_variant_get_bytestring((GVariant *)g_slist_nth_data(output_list, 11));
    product_part_number = g_variant_get_bytestring((GVariant *)g_slist_nth_data(output_list, 12));
    product_asset_tag = g_variant_get_bytestring((GVariant *)g_slist_nth_data(output_list, 13));
    product_oem_label = g_variant_get_bytestring_array((GVariant *)g_slist_nth_data(output_list, 14), &len);

    g_printf("FRU Name                      :  %s\r\n", board_name);
    g_printf("Board FRU File ID             :  %s\r\n", board_fru_file_id);
    g_printf("Board Manufacturer            :  %s\r\n", board_mfg);
    g_printf("Board Description             :  %s\r\n", board_name);
    g_printf("Board Serial Number           :  %s\r\n", serial_number);
    g_printf("Board Part Number             :  %s\r\n", board_part_number);
    g_printf("Board Mfg. Date               :  %s\r\n", board_mfg_date);
    g_printf("Product FRU File ID           :  %s\r\n", pro_fru_file_id);
    g_printf("Product Manufacturer          :  %s\r\n", product_mfg);
    g_printf("Product Description           :  %s\r\n", product_name);
    g_printf("Product Part/Model Number     :  %s\r\n", product_part_number);
    g_printf("Product Revision              :  %s\r\n", product_version);
    g_printf("Product Serial Number         :  %s\r\n", product_sn);
    g_printf("Asset Tag                     :  %s\r\n", product_asset_tag);

    if (product_oem_label != NULL) {
        for (i = 0; i < len; i++) {
            if (strlen(product_oem_label[i]) > 0) {
                extend_flag++;
                (void)print_fru_extend_label(product_oem_label[i]);
            }
        }
    }

    if (extend_flag == 0) {
        (void)dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROTERY_IPMBETH_BLADE_SLAVEADDR, slave_addr,
            &obj_handle);
        g_printf("%s has no label.\r\n", dfl_get_object_name(obj_handle));
    }

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
    return TRUE;
}


gint32 smm_get_node_fru_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target,
    gchar *location)
{
    guint8 board_num;
    guint8 slave_addr;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("ipmcget -l <bladeN|swiN> -t fru -d fruinfo\r\n");
        return FALSE;
    }

    board_num = get_blade_num(location);
    if (board_num == 0xff) {
        g_printf("ipmcget -l <bladeN|swiN> -t fru -d fruinfo\r\n");
        return FALSE;
    }

    
    slave_addr = get_blade_ipmbaddr(board_num);
    return print_blade_info(slave_addr);
}


void smm_cli_print_deviceid(const guchar *resp_src_data)
{
    errno_t safe_fun_ret;
    guchar SDRspt;
    guchar DeviceRvs;
    guchar DeviceAvb;
    guchar FirewareMRev;
    guchar FirewareSRev;
    guchar IPMIMVer;
    guchar IPMISVer;
    guchar ChassisSpt = 0;
    guchar BridgeSpt = 0;
    guchar IPMBEventGSpt = 0;
    guchar IPMBEventRSpt = 0;
    guchar FRUISpt = 0;
    guchar SELSpt = 0;
    guchar SDRSpt = 0;
    guchar SensorSpt = 0;
    guint32 ulManufacturer;
    guint32 ProductID;
    guchar AuxFireware;
    guchar uiturn;
    gchar     cAuxFireware[25] = {0};
    gchar     cturn[5] = {0};
    guchar    udeviceid[GETDEVICEID_MAXLEN] = {0};

    const OUTPUT_GETDEVICEID_T *output_value = NULL;

    output_value = (const OUTPUT_GETDEVICEID_T *)resp_src_data;
    memcpy_s(udeviceid, sizeof(udeviceid), output_value->arrDeviceid, sizeof(output_value->arrDeviceid));
    if (udeviceid[2] & 0x80) {
        SDRspt = 1;
    } else {
        SDRspt = 0;
    }
    DeviceRvs = udeviceid[2] & 0x0F;
    if (udeviceid[3] & 0x80) {
        DeviceAvb = 1;
    } else {
        DeviceAvb = 0;
    }
    FirewareMRev = udeviceid[3] & 0x7F;
    FirewareSRev = udeviceid[4];
    IPMIMVer = udeviceid[5] & 0x0F;
    IPMISVer = (udeviceid[5] & 0xF0) >> 4;
    processDeviceID((udeviceid[6] & 0x80), &ChassisSpt);
    processDeviceID((udeviceid[6] & 0x40), &BridgeSpt);
    processDeviceID((udeviceid[6] & 0x20), &IPMBEventGSpt);
    processDeviceID((udeviceid[6] & 0x10), &IPMBEventRSpt);
    processDeviceID((udeviceid[6] & 0x08), &FRUISpt);
    processDeviceID((udeviceid[6] & 0x04), &SELSpt);
    processDeviceID((udeviceid[6] & 0x02), &SDRSpt);
    processDeviceID((udeviceid[6] & 0x01), &SensorSpt);

    ulManufacturer = udeviceid[9];
    ulManufacturer = ulManufacturer << 8;
    ulManufacturer += udeviceid[8];
    ulManufacturer = ulManufacturer << 8;
    ulManufacturer += udeviceid[7];
    ProductID = udeviceid[11];
    ProductID = ProductID << 8;
    ProductID += udeviceid[10];
    AuxFireware = udeviceid[12];
    uiturn = AuxFireware >> 4;
    (void)snprintf_s(cturn, sizeof(cturn), sizeof(cturn) - 1, "%x", uiturn);
    safe_fun_ret = strncat_s(cAuxFireware, sizeof(cAuxFireware), cturn, sizeof(cturn));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return;
    }

    uiturn = AuxFireware & 0x0F;
    (void)snprintf_s(cturn, sizeof(cturn), sizeof(cturn) - 1, "%x", uiturn);
    safe_fun_ret = strncat_s(cAuxFireware, sizeof(cAuxFireware), cturn, sizeof(cturn));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return;
    }

    AuxFireware = udeviceid[13];
    uiturn = AuxFireware >> 4;
    (void)snprintf_s(cturn, sizeof(cturn), sizeof(cturn) - 1, "%x", uiturn);
    safe_fun_ret = strncat_s(cAuxFireware, sizeof(cAuxFireware), cturn, sizeof(cturn));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return;
    }
    uiturn = AuxFireware & 0x0F;
    (void)snprintf_s(cturn, sizeof(cturn), sizeof(cturn) - 1, "%x", uiturn);
    safe_fun_ret = strncat_s(cAuxFireware, sizeof(cAuxFireware), cturn, sizeof(cturn));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return;
    }
    AuxFireware = udeviceid[14];
    uiturn = AuxFireware >> 4;
    (void)snprintf_s(cturn, sizeof(cturn), sizeof(cturn) - 1, "%x", uiturn);
    safe_fun_ret = strncat_s(cAuxFireware, sizeof(cAuxFireware), cturn, sizeof(cturn));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return;
    }
    uiturn = AuxFireware & 4;
    (void)snprintf_s(cturn, sizeof(cturn), sizeof(cturn) - 1, "%x", uiturn);
    safe_fun_ret = strncat_s(cAuxFireware, sizeof(cAuxFireware), cturn, sizeof(cturn));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return;
    }
    AuxFireware = udeviceid[15];
    uiturn = AuxFireware >> 4;
    (void)snprintf_s(cturn, sizeof(cturn), sizeof(cturn) - 1, "%x", uiturn);
    safe_fun_ret = strncat_s(cAuxFireware, sizeof(cAuxFireware), cturn, sizeof(cturn));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return;
    }
    uiturn = AuxFireware & 0x0F;
    (void)snprintf_s(cturn, sizeof(cturn), sizeof(cturn) - 1, "%x", uiturn);
    safe_fun_ret = strncat_s(cAuxFireware, sizeof(cAuxFireware), cturn, sizeof(cturn));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return;
    }

    
    
    
    g_printf("\rDevice ID                    = 0x%02x"
        "\n\rSDR Support                  = %d"
        "\n\rDevice Revision              = %d"
        "\n\rDevice Available             = %d"
        "\n\rFirmware Revision            = %d.%02x"
        "\n\rIPMI Version                 = %d.%d"
        "\n\rShelf Support                = %d"
        "\n\rBridge Support               = %d"
        "\n\rIPMB Event Generator Support = %d"
        "\n\rIPMB Event Receiver Support  = %d"
        "\n\rFRU Inventory Support        = %d"
        "\n\rSEL Support                  = %d"
        "\n\rSDR Repository Support       = %d"
        "\n\rSensor Support               = %d"
        "\n\rManufacturer ID              = 0x%04x"
        "\n\rProduct ID                   = 0x%04x"
        "\n\rAux Firmware Revision        = %s",
        udeviceid[1], SDRspt, DeviceRvs, DeviceAvb, FirewareMRev, FirewareSRev, IPMIMVer, IPMISVer, ChassisSpt,
        BridgeSpt, IPMBEventGSpt, IPMBEventRSpt, FRUISpt, SELSpt, SDRSpt, SensorSpt, ulManufacturer, ProductID,
        cAuxFireware);
    g_printf("\n");
}
void processDeviceID(guchar cmp, guchar *valueSpt)
{
    if (cmp) {
        *valueSpt = 1;
    } else {
        *valueSpt = 0;
    }
}


gint32 smm_get_inner_subnet(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE group_obj = 0;
    gchar username[PARAMETER_LEN + 1] = { 0 };
    gchar ip[PARAMETER_LEN + 1] = { 0 };
    gchar inner_ip_addr[IPV4_IP_STR_SIZE + 1] = { 0 };
    guint32 ip_digit = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    
    if (dal_is_support_eth_define_specific_propery(PROPERTY_SUPPORT_MULTI_VLAN)) {
        g_printf("Do not support get inner subnet segment.\r\n");
        return FALSE;
    }

    if ((argc != 0)) {
        g_printf("Usage: ipmcget -l shelf -d subnet\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    ret = dal_get_specific_object_byte(ETHGROUP_CLASS1_NAME, ETH_GROUP_ATTRIBUTE_OUT_TYPE, INNER_ETHERNET, &group_obj);
    if (ret != RET_OK) {
        g_printf("Get inner ethgroup object failed.ret:%d\n", ret);
        return FALSE;
    }

    (void)dal_get_property_value_string(group_obj, ETH_GROUP_ATTRIBUTE_INNER_NETWORK, inner_ip_addr,
        sizeof(inner_ip_addr));
    (void)inet_pton(AF_INET, inner_ip_addr, (void *)&ip_digit);

    g_printf("\rThe subnet address:%d.%d.0.0\r\n", LONGB0(ip_digit), LONGB1(ip_digit));

    return TRUE;
}

guchar ipmc_get_local_data_powerstate(void)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar powerstate = PAYLOAD_POWER_STATE_UNSPECIFIED;

    ret = dal_get_object_handle_nth(CLASS_PAYLOAD, 0, &obj_handle);
    if (ret != VOS_OK) {
        g_printf("get obj_handle fail ret=%d.\r\n", ret);
        return powerstate;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_PAYLOAD_CHASSPWR_STATE, &powerstate);
    if (ret != VOS_OK) {
        g_printf("get property_data fail ret=%d.\r\n", ret);
        return powerstate;
    }

    return powerstate;
}

gint32 ipmc_preconditions_no_argc_judge(guchar privilege, const gchar* rolepriv, guchar argc)
{
    
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    return TRUE;
}


gint32 ipmc_get_om_channel_mode(guchar privilege, const gchar* rolepriv, guchar argc, gchar** argv, gchar* target)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList* output_list = NULL;
    guint32 current_mode;
    guint32 remain_time;
    
    gchar *current_mode_description[OM_CHANNEL_MODE_NUM] = {
        "0(default mode)",
        "1(OM PXE install mode)",
        "2(OM maintenance mode)",
    };

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        g_printf("Permission denied.\r\n");
        return FALSE;
    }
    if (argc != 0) {
        if (strcmp("smm", target) == 0) {
            g_printf("Usage: ipmcget [-l smm] -d omchannel\r\n");
        } else {
            g_printf("Usage: ipmcget -t lsw -d omchannel\r\n");
        }
        return FALSE;
    }
    ret = dal_get_object_handle_nth(CLASS_LSW_PORT_ATTR_CENTER, 0, &obj_handle);
    if (ret != VOS_OK) {
        g_printf("Get object handle fail!\r\n");
        return FALSE;
    }

    ret = dfl_call_class_method(obj_handle, METHOD_LSW_GET_OM_CHANNEL, NULL, NULL, &output_list);
    if (ret != DFL_OK) {
        g_printf("Get omchannel fail! ret(%d)\r\n", ret);
        return FALSE;
    }
    current_mode = g_variant_get_uint32((GVariant*)g_slist_nth_data(output_list, 0));
    remain_time = g_variant_get_uint32((GVariant*)g_slist_nth_data(output_list, 1));
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    if (current_mode >= OM_CHANNEL_MODE_NUM) {
        g_printf("Current mode(%u) is invalid!\r\n", current_mode);
        return FALSE;
    }
    g_printf("Current mode : %s\r\n", current_mode_description[current_mode]);
    if (current_mode != 0) {
        g_printf("Remain time : %u hour %u min\r\n", (remain_time / HOUR_TO_MINUTE), (remain_time % HOUR_TO_MINUTE));
    }

    return TRUE;
}


gint32 ipmc_get_cmesh_mode(guchar privilege, const gchar* rolepriv, guchar argc, gchar** argv, gchar* target)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 state = 0;

    
    gchar *current_mode_description[] = {
        "0: Supports BMC communication only",
        "1: VLAN all-pass mode",
    };

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        g_printf("Permission denied.\r\n");
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget -t lsw -d cmeshmode\r\n");
        return FALSE;
    }

    ret = dfl_get_object_handle(OBJ_LSW_PUBLIC_ATTR_LOCAL, &obj_handle);
    if (ret != DFL_OK) {
        g_printf("get obj fail\r\n");
        return FALSE;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_LSW_PUBLIC_CMESH_MODE, &state);
    if (ret != RET_OK) {
        g_printf("get property value fail\r\n");
        return FALSE;
    }
    if (state < ARRAY_SIZE(current_mode_description)) {
        g_printf("Current mode : %s\r\n", current_mode_description[state]);
    } else {
        g_printf("XML data exception! mode = %u\r\n", state);
        return FALSE;
    }

    return TRUE;
}
