

#include "set_clp_lom.h"

LOCAL gint32 _parses_json_objects_for_lom_x722(guint8 portnum, const gchar *devicename, const gchar *config_data,
    json_object **error_info, GSList **config_list);
LOCAL gint32 _parse_set_file_content_lom(GSList *config_list, gchar *content, guint32 size);
LOCAL gint32 _check_ports_valid_for_lom(guint8 portnum, json_object *ports_json, json_object **error_info);
LOCAL gint32 _convert_setting_mac_format(gchar *mac_addr);

gint32 bios_set_json_to_clp_lom_x722(guint8 portnum, guint16 board_id, const gchar *devicename,
    const gchar *config_data, json_object **error_info)
{
    gint32 ret = RET_OK;
    GSList *config_list = NULL;
    gchar *content = NULL;

    
    if (config_data == NULL || error_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    ret = _parses_json_objects_for_lom_x722(portnum, devicename, config_data, error_info, &config_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: _parses_json_objects_for_lom_x722 failed.", __FUNCTION__);
        if (config_list != NULL) {
            g_slist_free_full(config_list, (GDestroyNotify)g_free);
        }
        return ret;
    }

    content = (gchar *)g_malloc0(SMBIOS_DATA_SIZE);
    if (content == NULL) {
        if (config_list) {
            g_slist_free_full(config_list, (GDestroyNotify)g_free);
        }
        debug_log(DLOG_ERROR, "%s: g_malloc0 failed", __FUNCTION__);
        return RET_ERR;
    }
    (void)_parse_set_file_content_lom(config_list, content, SMBIOS_DATA_SIZE);

    ret = _write_tmp_file_and_copy(content, strlen(content), devicename);
    g_free(content);
    if (config_list) {
        g_slist_free_full(config_list, (GDestroyNotify)g_free);
    }

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:_write_tmp_file_for_copy failed", __FUNCTION__);
        return ret;
    }

    return RET_OK;
}

LOCAL gint32 _parses_json_objects_for_lom_x722(guint8 portnum, const gchar *devicename, const gchar *config_data,
    json_object **error_info, GSList **config_list)
{
    gint32 ret = RET_OK;
    gint32 ports_num = 0;
    gint32 i = 0;
    json_object *ports_json = NULL;
    json_object *port_json = NULL;
    json_object *config_json = NULL;
    json_object *pfs_json = NULL;
    json_object *pf_json = NULL;
    json_object *obj_json = NULL;
    CLP_RESPONSE_UNIT *config_unit_p = NULL;
    errno_t safe_fun_ret = EOK;
    
    gchar               prop_path[MAX_PROPERTY_PATH_LEN + 1] = {0};
    

    if (portnum == 0 || devicename == NULL || config_data == NULL || config_list == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RET_ERR;
    }

    config_json = json_tokener_parse(config_data);
    if (config_json == NULL) {
        debug_log(DLOG_ERROR, "%s: json_tokener_parse failed", __FUNCTION__);
        return RET_ERR;
    }

    ret = json_object_object_get_ex(config_json, RFPROP_CLP_PORTS_CONFIG, &ports_json);
    if (ret) {
        
        ret = _check_ports_valid_for_lom(portnum, ports_json, error_info);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: _check_ports_valid failed", __FUNCTION__);
            goto err_out;
        }

        
        ports_num = json_object_array_length(ports_json);

        for (i = 0; i < ports_num; i++) {
            port_json = json_object_array_get_idx(ports_json, i);
            config_unit_p = (CLP_RESPONSE_UNIT *)g_malloc0(sizeof(CLP_RESPONSE_UNIT));
            if (config_unit_p == NULL) {
                (ret = RET_ERR);
                debug_log(DLOG_ERROR, "%s: g_malloc0 failed", __FUNCTION__);
                goto err_out;
            }

            
            (void)_initial_config_unit(config_unit_p);
            ret = _set_header_info(0, port_json, config_unit_p, i);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: _set_header_info failed", __FUNCTION__);
                goto err_out;
            }

            (void)strncpy_s(config_unit_p->headinfo.devicename, sizeof(config_unit_p->headinfo.devicename), devicename,
                sizeof(config_unit_p->headinfo.devicename) - 1);

            if (json_object_object_get_ex(port_json, RFPROP_CLP_PFS_INFO, &pfs_json)) {
                pf_json = json_object_array_get_idx(pfs_json, 0);
                if (json_object_object_get_ex(pf_json, RFPROP_CLP_PF_ID, &obj_json)) {
                    if (json_type_int != json_object_get_type(obj_json)) {
                        ret = PROPERTY_VALUE_NOT_INLIST;
                        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN,
                            "%s/%s/%d/%s/%d/%s", MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, i,
                            RFPROP_CLP_PFS_INFO, 0, RFPROP_CLP_PF_ID);
                        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
                        json_object_object_add(*error_info, ERROR_VAL,
                            json_object_new_string(dal_json_object_get_str(obj_json)));
                        debug_log(DLOG_ERROR, "%s: json_type error", __FUNCTION__);
                        goto err_out;
                    }

                    config_unit_p->portconfig.NparPF[0].pf_id = json_object_get_int(obj_json);

                    if (!config_unit_p->headinfo.portid) {
                        if (config_unit_p->portconfig.NparPF[0].pf_id != 0) {
                            ret = PROPERTY_VALUE_NOT_INLIST;
                            debug_log(DLOG_ERROR, "%s: pf id error.", __FUNCTION__);
                            (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN,
                                "%s/%s/%d/%s/%d/%s", MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, i,
                                RFPROP_CLP_PFS_INFO, 0, RFPROP_CLP_PF_ID);
                            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
                            json_object_object_add(*error_info, ERROR_VAL,
                                json_object_new_string(dal_json_object_get_str(obj_json)));
                            goto err_out;
                        }
                    } else {
                        if (config_unit_p->portconfig.NparPF[0].pf_id != 1) {
                            ret = PROPERTY_VALUE_NOT_INLIST;
                            debug_log(DLOG_ERROR, "%s: pf id error.", __FUNCTION__);
                            (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN,
                                "%s/%s/%d/%s/%d/%s", MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, i,
                                RFPROP_CLP_PFS_INFO, 0, RFPROP_CLP_PF_ID);
                            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
                            json_object_object_add(*error_info, ERROR_VAL,
                                json_object_new_string(dal_json_object_get_str(obj_json)));
                            goto err_out;
                        }
                    }
                }

                obj_json = NULL;

                
                if (json_object_object_get_ex(pf_json, CLP_KEY_ETH_MAC, &obj_json) && obj_json != NULL) {
                    
                    safe_fun_ret = strncpy_s(config_unit_p->portconfig.NparPF[0].PermanentAddress,
                        sizeof(config_unit_p->portconfig.NparPF[0].PermanentAddress), dal_json_object_get_str(obj_json),
                        sizeof(config_unit_p->portconfig.NparPF[0].PermanentAddress) - 1);
                    if (safe_fun_ret != EOK) {
                        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                    }
                    
                    ret = g_regex_match_simple(REGEX_MAC, config_unit_p->portconfig.NparPF[0].PermanentAddress,
                        (GRegexCompileFlags)0, (GRegexMatchFlags)0);
                    if (ret != TRUE && strlen(config_unit_p->portconfig.NparPF[0].PermanentAddress) != 0) {
                        ret = PROPERTY_VALUE_ERROR;
                        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN,
                            "%s/%s/%d/%s/%d/%s", MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, i,
                            RFPROP_CLP_PFS_INFO, 0, CLP_KEY_ETH_MAC);
                        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
                        json_object_object_add(*error_info, ERROR_VAL,
                            json_object_new_string(dal_json_object_get_str(obj_json)));
                        debug_log(DLOG_ERROR, "%s: g_regex_match_simple failed", __FUNCTION__);
                        goto err_out;
                    }
                }

                *config_list = g_slist_append(*config_list, config_unit_p);
                config_unit_p = NULL;
            } else {
                if (config_unit_p != NULL) {
                    g_free(config_unit_p);
                }
            }
        }
    } else {
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG);
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        ret = PROPERTY_MISSING;
        goto err_out;
    }

    json_object_put(config_json);
    return RET_OK;

err_out:
    debug_log(DLOG_ERROR, "%s: error failed", __FUNCTION__);
    if (config_unit_p != NULL) {
        g_free(config_unit_p);
    }
    json_object_put(config_json);
    return ret;
}

LOCAL gint32 _parse_set_file_content_lom(GSList *config_list, gchar *content, guint32 size)
{
    gint32 iRet = -1;
    errno_t safe_fun_ret = EOK;
    GSList *obj_node = NULL;
    CLP_RESPONSE_UNIT *node_p = NULL;
    gchar str_val[LINE_MAX_LEN + 1] = {0};
    gchar *devicename = NULL;
    gchar *vid_did = NULL;

    if (config_list == NULL || content == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RET_ERR;
    }

    vid_did = "8086:37ce";

    for (obj_node = config_list; obj_node; obj_node = obj_node->next) {
        node_p = (CLP_RESPONSE_UNIT *)obj_node->data;
        safe_fun_ret = strncat_s(content, size, "{\r\n", strlen("{\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        devicename = g_ascii_strdown(node_p->headinfo.devicename, strlen(node_p->headinfo.devicename));
        if (devicename != NULL) {
            iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "#%s%d.%d\r\n", devicename,
                node_p->headinfo.chipid, node_p->headinfo.portid + 1);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
            }
            _data_string_concat(str_val, size, content);

            (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
            iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "%s%d.%d\r\n\r\n", devicename,
                node_p->headinfo.chipid, node_p->headinfo.portid + 1);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
            }
            _data_string_concat(str_val, size, content);
            g_free(devicename);
        }

        safe_fun_ret = strncat_s(content, size, "#board_id\r\n", strlen("#board_id\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "board_id=X722\r\n\r\n");
        _data_string_concat(str_val, size, content);

        safe_fun_ret = strncat_s(content, size, "#netcard\r\n", strlen("#netcard\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "netcard=%s\r\n", vid_did);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
        }
        _data_string_concat(str_val, size, content);

        safe_fun_ret = strncat_s(content, size, "#reset\r\n", strlen("#reset\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        safe_fun_ret = strncat_s(content, size, "#CMD\r\n", strlen("#CMD\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }

        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);

        if (strlen(node_p->portconfig.NparPF[0].PermanentAddress)) {
            _convert_setting_mac_format(node_p->portconfig.NparPF[0].PermanentAddress);
            iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "set permanentaddress=%s\r\n\r\n",
                node_p->portconfig.NparPF[0].PermanentAddress);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
            }
            _data_string_concat(str_val, size, content);
        }

        safe_fun_ret = strncat_s(content, size, "#Oprom\r\n}\r\n", strlen("#Oprom\r\n}\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    }

    return RET_OK;
}

LOCAL gint32 _check_ports_valid_for_lom(guint8 portnum, json_object *ports_json, json_object **error_info)
{
    gint32 ret = RET_OK;
    gint32 port_count = 0;
    guint8 i = 0;
    gint32 portid = 0;
    json_object *object = NULL;
    json_object *obj_json = NULL;
    gint32          portid_array[MXEP_PORT_NUM + 1] = {0};
    
    gchar           prop_path[MAX_PROPERTY_PATH_LEN + 1] = {0};
    

    if (portnum == 0 || ports_json == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RET_ERR;
    }

    port_count = json_object_array_length(ports_json);
    if (port_count > portnum) {
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG);
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        debug_log(DLOG_ERROR, "%s: Lom card excessive number of port.", __FUNCTION__);
        return PORT_PF_TARGET_EXCEED;
    }

    for (i = 0; i < port_count; i++) {
        object = json_object_array_get_idx(ports_json, i);
        ret = json_object_object_get_ex(object, RFPROP_CLP_PORT_ID, &obj_json);
        if (ret) {
            if (json_type_int != json_object_get_type(obj_json)) {
                (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%u/%s",
                    MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, i, RFPROP_CLP_PORT_ID);
                json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
                json_object_object_add(*error_info, ERROR_VAL,
                    json_object_new_string(dal_json_object_get_str(obj_json)));
                debug_log(DLOG_ERROR, "%s: json_type error", __FUNCTION__);
                return PROPERTY_VALUE_NOT_INLIST;
            }

            portid = json_object_get_int(obj_json);
            if (portid <= 0 || portnum < portid) {
                (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%u/%s",
                    MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, i, RFPROP_CLP_PORT_ID);
                json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
                json_object_object_add(*error_info, ERROR_VAL,
                    json_object_new_string(dal_json_object_get_str(obj_json)));
                debug_log(DLOG_ERROR, "%s: portid invalid.", __FUNCTION__);
                return PROPERTY_VALUE_NOT_INLIST;
            }

            // portid重复性检查
            if (portid_array[portid] == 1) {
                (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%u/%s",
                    MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, i, RFPROP_CLP_PORT_ID);
                json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
                json_object_object_add(*error_info, ERROR_VAL,
                    json_object_new_string(dal_json_object_get_str(obj_json)));
                debug_log(DLOG_ERROR, "%s: The value of portid is the same as that of other parameters.", __FUNCTION__);
                return PROPERTY_VALUE_ERROR;
            } else {
                portid_array[portid] = 1;
            }
        }
    }

    return RET_OK;
}


LOCAL gint32 _convert_setting_mac_format(gchar *mac_addr)
{
    errno_t safe_fun_ret = EOK;
    gint32 i = 0;
    gint32 j = 0;
    guint8 current_num = 0;
    gchar   tmp_str[MAX_LEN_128] = {0};

    if (mac_addr == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RET_ERR;
    }

    for (i = 0; *(mac_addr + i); i++) {
        tmp_str[j++] = *(mac_addr + i);
        current_num++;

        if (current_num == 2 && *(mac_addr + i + 1)) {
            tmp_str[j++] = '-';
            current_num = 0;
        }
    }

    safe_fun_ret = strncpy_s(mac_addr, MAX_LEN_32, tmp_str, MAX_LEN_32 - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    return RET_OK;
}