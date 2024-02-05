
#include "bios.h"
#include "set_clp.h"

LOCAL void _error_massage_info(gint32 error_code, json_object **error_info);
LOCAL gint32 check_boottype_from_file(OBJ_HANDLE obj_handle, const gchar *type_val, const gchar *file_name,
    json_object **error_info);
LOCAL guint8 get_bios_file_change(void);


gint32 Bios_Set_Clp_Config(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret = 0;
    guint16 board_id = 0;
    guint8 port_count = 0;
    guint8 bios_change_flag = 0;
    const gchar *devicename = NULL;
    const gchar *configuration = NULL;
    json_object *o_message_jso = NULL;

    
    if (caller_info == NULL || output_list == NULL || input_list == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        goto err;
    }

    
    o_message_jso = json_object_new_object();

    
    bios_change_flag = get_bios_file_change();
    if (bios_change_flag) {
        ret = check_boottype_from_file(obj_handle, BIOS_UEFI_BOOT_STR, BIOS_FILE_SETTING_NAME, &o_message_jso);
        if (ret == BOOT_TYPE_NOT_UEFI) {
            ret = MODE_UNSUPPORTED;
            goto end;
        } else if (BOOT_TYPE_NOT_FOUND == ret) {
            ret =
                check_boottype_from_file(obj_handle, BIOS_UEFI_BOOT_STR, BIOS_FILE_CURRENT_VALUE_NAME, &o_message_jso);
            if (ret == BOOT_TYPE_NOT_UEFI) {
                ret = MODE_UNSUPPORTED;
                debug_log(DLOG_ERROR, "%s: boot type is not UEFI.", __FUNCTION__);
                goto end;
            }
        }
    } else {
        ret = check_boottype_from_file(obj_handle, BIOS_UEFI_BOOT_STR, BIOS_FILE_CURRENT_VALUE_NAME, &o_message_jso);
        if (ret == BOOT_TYPE_NOT_UEFI) {
            ret = MODE_UNSUPPORTED;
            debug_log(DLOG_ERROR, "%s: boot type is not UEFI.", __FUNCTION__);
            goto end;
        }
    }

    board_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(input_list, 0));
    devicename = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 1), NULL);
    configuration = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 2), NULL);
    if (devicename == NULL || configuration == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid parameter.", __FUNCTION__);
        goto err;
    }

    
    if (!g_ascii_strncasecmp(devicename, LOM_CARD_DEVICE_NAME, strlen(LOM_CARD_DEVICE_NAME))) {
        port_count = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 3));
        ret = bios_set_json_to_clp_lom_x722(port_count, board_id, devicename, configuration, &o_message_jso);
    } else {
        if (board_id == MXEN_ID || board_id == MXEP_ID) { // 520_522
            ret = bios_set_json_to_clp_520_522(board_id, devicename, configuration, &o_message_jso);
        } else if (board_id == MHFA_ID) { // 220
            ret = bios_set_json_to_clp_220(board_id, devicename, configuration, &o_message_jso);
        } else if (board_id == MHFB_ID) { // 221
            ret = bios_set_json_to_clp_221(board_id, devicename, configuration, &o_message_jso);
        } else if (board_id == MXEM_ID || board_id == MXEK_ID || board_id == MXEL_ID) { // 310_312_912
            ret = _bios_set_json_to_clp_310_312_912(board_id, devicename, configuration, &o_message_jso);
        } else {
            ret = BOARD_UNSUPPORTED;
        }
    }

end:

    
    if (ret != RET_OK) {
        (void)_error_massage_info(ret, &o_message_jso);
        *output_list = g_slist_append(*output_list, g_variant_new_string(dal_json_object_get_str(o_message_jso)));
    }

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: bios_parse_clp_to_json failed, return value is %d", __FUNCTION__, ret);
        goto err;
    }
    json_object_put(o_message_jso);
    method_operation_log(caller_info, NULL, "BIOS Set Netcard Configuration successfully.");
    return RET_OK;

err:
    json_object_put(o_message_jso);
    method_operation_log(caller_info, NULL, "BIOS Set Netcard Configuration failed.");

    
    return (ret == RET_ERR) ? RET_ERR : RET_OK;
}


LOCAL void _error_massage_info(gint32 error_code, json_object **error_info)
{
    gchar message_id[MAX_LEN_64 + 1] = {0};
    
    if (error_info == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return;
    }

    switch (error_code) {
        case BOARD_UNSUPPORTED:
            (void)strncpy_s(message_id, MAX_LEN_64, BOARD_UNSUPPORTED_ID, strlen(BOARD_UNSUPPORTED_ID));
            break;

        case PROPERTY_MISSING:
            (void)strncpy_s(message_id, MAX_LEN_64, PROPERTY_MISSING_ID, strlen(PROPERTY_MISSING_ID));
            break;

        case PORT_PF_TARGET_EXCEED:
            (void)strncpy_s(message_id, MAX_LEN_64, PORT_PF_TARGET_EXCEED_ID, strlen(PORT_PF_TARGET_EXCEED_ID));
            break;

        case PROPERTY_VALUE_NOT_INLIST:
            (void)strncpy_s(message_id, MAX_LEN_64, PROPERTY_VALUE_NOT_INLIST_ID, strlen(PROPERTY_VALUE_NOT_INLIST_ID));
            break;

        case MODE_UNSUPPORTED:
            (void)strncpy_s(message_id, MAX_LEN_64, MODE_UNSUPPORTED_ID, strlen(MODE_UNSUPPORTED_ID));
            break;

        case PROPERTY_VALUE_ERROR:
            (void)strncpy_s(message_id, MAX_LEN_64, PROPERTY_VALUE_ERROR_ID, strlen(PROPERTY_VALUE_ERROR_ID));
            break;

        case BANDWITH_ERROR:
            (void)strncpy_s(message_id, MAX_LEN_64, BANDWITH_ERROR_ID, strlen(BANDWITH_ERROR_ID));
            break;

        case VLANID_CONFLICT:
            (void)strncpy_s(message_id, MAX_LEN_64, VLANID_CONFLICT_ID, strlen(VLANID_CONFLICT_ID));
            break;

        case TRUNK_RANGE_CHECK_FAIL:
            (void)strncpy_s(message_id, MAX_LEN_64, TRUNK_RANGE_CHECK_FAIL_ID, strlen(TRUNK_RANGE_CHECK_FAIL_ID));
            break;

        case COMP_CODE_STATUS_INVALID:
            (void)strncpy_s(message_id, MAX_LEN_64, CONFIG_BUSY, strlen(CONFIG_BUSY));
            break;

        case PROPERTY_UNKNOW:
            (void)strncpy_s(message_id, MAX_LEN_64, PROPERTY_UNKNOW_ID, strlen(PROPERTY_UNKNOW_ID));
            break;

        case PORT_PF_TARGET_INSUFFICIENT:
            (void)strncpy_s(message_id, MAX_LEN_64, PORT_PF_TARGET_INSUFFICIENT_ID,
                strlen(PORT_PF_TARGET_INSUFFICIENT_ID));
            break;

            
        case CHIP_RULE_CHECK_FAIL:
            (void)strncpy_s(message_id, MAX_LEN_64, CHIP_RULE_CHECK_FAIL_ID, strlen(CHIP_RULE_CHECK_FAIL_ID));
            break;
            

            
        case PROTOCOLS_CHECK_FAIL:
            (void)strncpy_s(message_id, MAX_LEN_64, PROTOCOLS_CHECK_FAIL_ID, strlen(PROTOCOLS_CHECK_FAIL_ID));
            break;
            

            
        case PROPERTY_TYPE_ERROR:
            (void)strncpy_s(message_id, MAX_LEN_64, PROPERTY_TYPE_ERROR_ID, strlen(PROPERTY_TYPE_ERROR_ID));
            break;
            

            
        case PORT_ID_SEQUENCE_ERROR:
            (void)strncpy_s(message_id, MAX_LEN_64, PORT_ID_SEQUENCE_ERROR_ID, strlen(PORT_ID_SEQUENCE_ERROR_ID));
            break;

            
            
        case QINQ_VLANID_ERROR:
            (void)strncpy_s(message_id, MAX_LEN_64, MSGID_QINQ_VLANID_ERROR, strlen(MSGID_QINQ_VLANID_ERROR));
            break;

            
        default:
            
            if (error_code == BOOTPROTOCOL_NOT_FOUND_IN_PFS) {
                (void)strncpy_s(message_id, MAX_LEN_64, BOOTPROTOCOL_NOT_FOUND_IN_PFS_ID,
                    sizeof(BOOTPROTOCOL_NOT_FOUND_IN_PFS_ID) - 1);
                json_object_object_add(*error_info, ERROR_MSGID_KEY, json_object_new_string(message_id));
                return;
            }
            
            return;
    }

    json_object_object_add(*error_info, ERROR_MSGID_KEY, json_object_new_string(message_id));

    return;
}


LOCAL gint32 check_boottype_from_file(OBJ_HANDLE obj_handle, const gchar *type_val, const gchar *file_name,
    json_object **error_info)
{
    gchar           filename[REGRIST_FILE_NAME_LENGTH] = {0};
    json_object *file_obj = NULL;
    json_object *type_obj = NULL;
    const gchar *type_str = NULL;
    gint32 ret = BOOT_TYPE_NOT_FOUND;

    (void)dal_get_property_value_string(obj_handle, file_name, filename, REGRIST_FILE_NAME_LENGTH);

    if (vos_get_file_accessible(filename) == TRUE && vos_get_file_length(filename) != 0) {
        file_obj = json_object_from_file(filename);
        if (json_object_object_get_ex(file_obj, BIOS_BOOT_TYPE_STR, &type_obj)) {
            type_str = dal_json_object_get_str(type_obj);
            if (!g_ascii_strncasecmp(type_str, type_val, strlen(type_val))) {
                ret = BOOT_TYPE_IS_UEFI;
            } else {
                ret = BOOT_TYPE_NOT_UEFI;
                json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(MESSAGE_PROPERTY_PATH_HEAD));
                json_object_object_add(*error_info, ERROR_REL_PROP, json_object_new_string(BIOS_BOOT_TYPE_STR));
                json_object_object_add(*error_info, ERROR_REL_VAL, json_object_new_string(type_str));
            }
        }
    }

    if (file_obj != NULL) {
        json_object_put(file_obj);
    }

    return ret;
}


LOCAL guint8 get_bios_file_change(void)
{
    return g_bios_file_change[0].file_change_flag;
}