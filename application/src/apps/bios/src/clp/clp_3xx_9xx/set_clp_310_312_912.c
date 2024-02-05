

#include "set_clp_310_312_912.h"

LOCAL gint32 _json_set_and_rules_check_for_310_312_912(guint16 board_id, const gchar *devicename,
    const gchar *config_data, json_object **error_info, GSList **config_list);
LOCAL gint32 _parse_set_file_content_for_310_312_912(GSList *config_list, gchar *content, guint32 size);
LOCAL gint32 _set_port_config_for_310_312_912(guint16 board_id, json_object *port_json,
    CLP_RESPONSE_UNIT *config_unit_p, CLP_RESPONSE_UNIT *config_unit_p_bak, json_object **error_info,
    gint32 port_index);
LOCAL gint32 _set_pf_config_for_310_312_912(json_object *pfs_json, CLP_RESPONSE_UNIT *config_port_unit,
    CLP_RESPONSE_UNIT *config_port_unit_bak, json_object **error_info, gint32 port_index);
LOCAL gint32 _set_pf_config_pfid_for_310_312_912(json_object *pf_id_json, CLP_RESPONSE_UNIT *config_port_unit,
    json_object **error_info, gint32 port_index);


gint32 _bios_set_json_to_clp_310_312_912(guint16 board_id, const gchar *devicename, const gchar *config_data,
    json_object **error_info)
{
    gint32 ret = 0;
    GSList *config_list = NULL;
    gchar *content = NULL;

    
    if (config_data == NULL || error_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    
    ret = _json_set_and_rules_check_for_310_312_912(board_id, devicename, config_data, error_info, &config_list);
    if (ret != RET_OK) {
        if (config_list) {
            g_slist_free_full(config_list, (GDestroyNotify)g_free);
        }
        debug_log(DLOG_ERROR, "%s:_json_set_and_rules_check_for_310_312_912 failed, ret is %d", __FUNCTION__, ret);
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

    
    ret = _parse_set_file_content_for_310_312_912(config_list, content, SMBIOS_DATA_SIZE);
    if (ret != RET_OK) {
        g_free(content);
        if (config_list) {
            g_slist_free_full(config_list, (GDestroyNotify)g_free);
        }
        debug_log(DLOG_ERROR, "%s:_parse_set_file_content_for_310_312_912 failed", __FUNCTION__);
        return RET_ERR;
    }

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


LOCAL gint32 _json_set_and_rules_check_for_310_312_912(guint16 board_id, const gchar *devicename,
    const gchar *config_data, json_object **error_info, GSList **config_list)
{
    gint32 ret = RET_OK;
    gint32 retv = EOK;
    gint32 ports_num = 0;
    gint32 i = 0;
    json_object *ports_json = NULL;
    json_object *port_json = NULL;
    json_object *p_config_json = NULL;
    CLP_RESPONSE_UNIT *config_port_unit = NULL;
    CLP_RESPONSE_UNIT *config_port_unit_bak = NULL;
    gchar               prop_path[MAX_PROPERTY_PATH_LEN + 1] = {0};

    if (config_data == NULL || devicename == NULL || config_list == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RET_ERR;
    }

    p_config_json = json_tokener_parse(config_data);
    if (p_config_json == NULL) {
        debug_log(DLOG_ERROR, "%s: json_tokener_parse failed", __FUNCTION__);
        return RET_ERR;
    }

    ret = json_object_object_get_ex(p_config_json, RFPROP_CLP_PORTS_CONFIG, &ports_json);
    if (ret) {
        
        ret = _check_ports_valid(board_id, ports_json, error_info);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: _check_ports_valid_for_310_312_912 failed", __FUNCTION__);
            goto err_out;
        }

        
        
        config_port_unit_bak = (CLP_RESPONSE_UNIT *)g_malloc0(sizeof(CLP_RESPONSE_UNIT));
        if (config_port_unit_bak == NULL) {
            ret = RET_ERR;
            debug_log(DLOG_ERROR, "%s: g_malloc0 for config_unit_p failed", __FUNCTION__);
            goto err_out;
        }
        
        
        ports_num = json_object_array_length(ports_json);
        for (i = 0; i < ports_num; i++) {
            port_json = json_object_array_get_idx(ports_json, i);
            config_port_unit = (CLP_RESPONSE_UNIT *)g_malloc0(sizeof(CLP_RESPONSE_UNIT));
            if (config_port_unit == NULL) {
                ret = RET_ERR;
                debug_log(DLOG_ERROR, "%s: g_malloc0 for config_unit_p failed", __FUNCTION__);
                goto err_out;
            }
            
            (void)_initial_config_unit(config_port_unit);
            config_port_unit->board_id = board_id;

            retv = strncpy_s(config_port_unit->headinfo.devicename, sizeof(config_port_unit->headinfo.devicename),
                devicename, sizeof(config_port_unit->headinfo.devicename) - 1);
            if (retv != EOK) {
                debug_log(DLOG_ERROR, "%s: strncpy_s failed", __FUNCTION__);
                goto err_out;
            }

            ret = _set_header_info(board_id, port_json, config_port_unit, i);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: _set_header_info failed", __FUNCTION__);
                goto err_out;
            }
            
            ret = _set_port_config_for_310_312_912(board_id, port_json, config_port_unit, config_port_unit_bak,
                error_info, i);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: _set_port_config failed, ret is %d", __FUNCTION__, ret);
                goto err_out;
            }

            *config_list = g_slist_append(*config_list, config_port_unit);
            config_port_unit = NULL;
        }
    } else {
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG);
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        ret = PROPERTY_MISSING;
        goto err_out;
    }
    json_object_put(p_config_json);
    if (config_port_unit_bak != NULL) {
        g_free(config_port_unit_bak);
    }
    return RET_OK;

err_out:
    debug_log(DLOG_ERROR, "%s: error failed", __FUNCTION__);
    if (config_port_unit_bak != NULL) {
        g_free(config_port_unit_bak);
    }
    if (config_port_unit != NULL) {
        g_free(config_port_unit);
    }
    json_object_put(p_config_json);
    return ret;
}


LOCAL gint32 _parse_set_file_content_for_310_312_912(GSList *config_list, gchar *content, guint32 size)
{
    GSList *obj_node = NULL;
    CLP_RESPONSE_UNIT *node_p = NULL;
    gchar prop_value[LINE_MAX_LEN + 1] = {0};
    gchar *devicename = NULL;
    gchar *vid_did = NULL;
    gint32 i_index = 0;
    gint32 i_ret;

    if (config_list == NULL || content == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RET_ERR;
    }

    vid_did = MXEK_VID_DID;
    for (obj_node = config_list; obj_node; obj_node = obj_node->next) {
        node_p = (CLP_RESPONSE_UNIT *)obj_node->data;
        errno_t safe_fun_ret = strncat_s(content, size, "{\r\n", strlen("{\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }

        devicename = g_ascii_strdown(node_p->headinfo.devicename, strlen(node_p->headinfo.devicename));
        if (devicename != NULL) {
            i_ret = snprintf_s(prop_value, LINE_MAX_LEN + 1, LINE_MAX_LEN, "#%s.%d.%d\r\n", devicename,
                node_p->headinfo.chipid, node_p->headinfo.portid);
            if (i_ret <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, i_ret);
            }

            _data_string_concat(prop_value, size, content);
            (void)memset_s(prop_value, sizeof(prop_value), 0, sizeof(prop_value));
            i_ret = snprintf_s(prop_value, LINE_MAX_LEN + 1, LINE_MAX_LEN, "%s.%d.%d\r\n\r\n", devicename,
                node_p->headinfo.chipid, node_p->headinfo.portid);
            if (i_ret <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, i_ret);
            }

            _data_string_concat(prop_value, size, content);
            g_free(devicename);
        }
        safe_fun_ret = strncat_s(content, size, "#board_id\r\n", strlen("#board_id\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }

        (void)memset_s(prop_value, sizeof(prop_value), 0, sizeof(prop_value));
        (void)snprintf_s(prop_value, LINE_MAX_LEN + 1, LINE_MAX_LEN, "board_id=%u\r\n\r\n", node_p->board_id);
        _data_string_concat(prop_value, size, content);
        safe_fun_ret = strncat_s(content, size, "#netcard\r\n", strlen("#netcard\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }

        (void)memset_s(prop_value, sizeof(prop_value), 0, sizeof(prop_value));
        i_ret = snprintf_s(prop_value, LINE_MAX_LEN + 1, LINE_MAX_LEN, "netcard=%s\r\n", vid_did);
        if (i_ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, i_ret);
        }
        _data_string_concat(prop_value, size, content);

        safe_fun_ret = strncat_s(content, size, "#reset\r\n", strlen("#reset\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }

        safe_fun_ret = strncat_s(content, size, "#CMD\r\n", strlen("#CMD\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }

        (void)memset_s(prop_value, sizeof(prop_value), 0, sizeof(prop_value));
        i_ret = snprintf_s(prop_value, LINE_MAX_LEN + 1, LINE_MAX_LEN, "set netport1 %s=%s\r\n", CLP_KEY_ETH_MAC,
            node_p->portconfig.NparPF[i_index].PermanentAddress);
        if (i_ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, i_ret);
        }
        _data_string_concat(prop_value, size, content);

        safe_fun_ret = strncat_s(content, size, "exit\r\n", strlen("exit\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }

        (void)memset_s(prop_value, sizeof(prop_value), 0, sizeof(prop_value));
        (void)snprintf_s(prop_value, LINE_MAX_LEN + 1, LINE_MAX_LEN, "show netport1 %s\r\n", CLP_KEY_ETH_MAC);

        safe_fun_ret = strncat_s(content, size, prop_value, strlen(prop_value));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }

        safe_fun_ret = strncat_s(content, size, "#Oprom\r\n}\r\n", strlen("#Oprom\r\n}\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }

        i_index++;
    }

    return RET_OK;
}


LOCAL gint32 _set_port_config_for_310_312_912(guint16 board_id, json_object *port_json,
    CLP_RESPONSE_UNIT *config_unit_p, CLP_RESPONSE_UNIT *config_unit_p_bak, json_object **error_info, gint32 port_index)
{
    gint32 ret = 0;
    json_object *obj_json = NULL;
    gchar prop_path[MAX_PROPERTY_PATH_LEN + 1] = {0};

    if (port_json == NULL || config_unit_p == NULL || config_unit_p_bak == NULL || error_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    if (json_object_object_get_ex(port_json, RFPROP_CLP_PFS_INFO, &obj_json)) {
        ret = _set_pf_config_for_310_312_912(obj_json, config_unit_p, config_unit_p_bak, error_info, port_index);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: _set_pf_config_for_221 failed", __FUNCTION__);
            return ret;
        }
    } else {
        debug_log(DLOG_ERROR, "%s:  get PFSConfig failed", __FUNCTION__);
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO);
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_MISSING;
    }

    return RET_OK;
}


LOCAL gint32 _set_pf_config_for_310_312_912(json_object *pfs_json, CLP_RESPONSE_UNIT *config_port_unit,
    CLP_RESPONSE_UNIT *config_port_unit_bak, json_object **error_info, gint32 port_index)
{
    gint32 pfs_num = 0;
    gint32 ret = 0;
    gint32 retv = EOK;
    gint32 i = 0;
    json_object *pf_json = NULL;
    json_object *prop_json = NULL;
    const gchar *prop_val = NULL;
    gchar prop_path[MAX_PROPERTY_PATH_LEN + 1] = {0};

    if (pfs_json == NULL || config_port_unit == NULL || config_port_unit_bak == NULL || error_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL", __FUNCTION__);
        return RET_ERR;
    }

    pfs_num = json_object_array_length(pfs_json);
    if (pfs_num != 1) {
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO);
        (void)json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        debug_log(DLOG_ERROR, "%s: pfs_num invalid", __FUNCTION__);
        if (pfs_num > 1) {
            return PORT_PF_TARGET_EXCEED;
        } else {
            return PORT_PF_TARGET_INSUFFICIENT;
        }
    }

    pf_json = json_object_array_get_idx(pfs_json, 0);
    
    (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
    (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
        MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, 0, RFPROP_CLP_PF_ID);
    
    
    if (json_object_object_get_ex(pf_json, RFPROP_CLP_PF_ID, &prop_json)) {
        if (json_type_int != json_object_get_type(prop_json)) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(prop_json)));
            debug_log(DLOG_ERROR, "%s: json_type error", __FUNCTION__);
            return PROPERTY_VALUE_NOT_INLIST;
        }
        
        ret = _set_pf_config_pfid_for_310_312_912(prop_json, config_port_unit, error_info, port_index);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: set pf_id error.", __FUNCTION__);
            return ret;
        }
    } else {
        debug_log(DLOG_ERROR, "%s: set pf_id error.", __FUNCTION__);
        (void)json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_MISSING;
    }

    
    prop_json = NULL;
    ret = json_object_object_get_ex(pf_json, CLP_KEY_ETH_MAC, &prop_json);
    
    (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
    (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
        MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, 0, CLP_KEY_ETH_MAC);
    
    if (ret && prop_json != NULL) {
        prop_val = dal_json_object_get_str(prop_json);

        
        ret = g_regex_match_simple(REGEX_MAC, prop_val, (GRegexCompileFlags)0, (GRegexMatchFlags)0);
        if (ret != TRUE && strlen(prop_val) != MIN_PROP_LEN) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(prop_json)));
            debug_log(DLOG_ERROR, "%s: g_regex_match_simple failed", __FUNCTION__);
            return PROPERTY_VALUE_ERROR;
        }

        if (config_port_unit != NULL && config_port_unit_bak != NULL) {
            // mac地址重复性校验
            
            (void)strncpy_s(config_port_unit_bak->portconfig.NparPF[port_index].PermanentAddress, MAX_LEN_32 + 1,
                prop_val, MAX_LEN_32);
            for (i = port_index; i > 0; i--) {
                if (strcmp(prop_val, config_port_unit_bak->portconfig.NparPF[i - 1].PermanentAddress) == 0) {
                    json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
                    json_object_object_add(*error_info, ERROR_VAL,
                        json_object_new_string(dal_json_object_get_str(prop_json)));
                    debug_log(DLOG_ERROR, "%s: PermanentAddress value is conflicting", __FUNCTION__);
                    return PROPERTY_VALUE_ERROR;
                }
            }
            
            retv = strncpy_s(config_port_unit->portconfig.NparPF[port_index].PermanentAddress, MAX_LEN_32 + 1, prop_val,
                MAX_LEN_32);
            if (retv != EOK) {
                debug_log(DLOG_ERROR, "%s: strncpy_s for CLP_KEY_ETH_MAC failed.", __FUNCTION__);
                return RET_ERR;
            }
        }
    } else if (ret && prop_json == NULL) {
        debug_log(DLOG_ERROR, "%s: property %s is null.", __FUNCTION__, CLP_KEY_ETH_MAC);
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        json_object_object_add(*error_info, ERROR_VAL, json_object_new_string("null"));
        return PROPERTY_TYPE_ERROR;
    } else {
        debug_log(DLOG_ERROR, "%s: get property CLP_KEY_ETH_MAC failed.", __FUNCTION__);
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_MISSING;
    }

    return RET_OK;
}


LOCAL gint32 _set_pf_config_pfid_for_310_312_912(json_object *pf_id_json, CLP_RESPONSE_UNIT *config_port_unit,
    json_object **error_info, gint32 port_index)
{
    gint32 pf_id = 0;
    gchar pro_path[MAX_PROPERTY_PATH_LEN + 1] = {0};

    if (pf_id_json == NULL || config_port_unit == NULL || error_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL", __FUNCTION__);
        return RET_ERR;
    }

    (void)memset_s(pro_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
    (void)snprintf_s(pro_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
        MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, 0, RFPROP_CLP_PF_ID);

    pf_id = json_object_get_int(pf_id_json);
    if (pf_id != MIN_PF_ID && pf_id != 1) {
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(pro_path));
        json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(pf_id_json)));
        debug_log(DLOG_ERROR, "%s: pf_id %d invalid.", __FUNCTION__, pf_id);
        return PROPERTY_VALUE_NOT_INLIST;
    }
    
    if (config_port_unit != NULL) {
        if (config_port_unit->headinfo.portid == 0) {
            if ((pf_id % 2) == 0) {
                config_port_unit->portconfig.NparPF[port_index].pf_id = pf_id;
            } else {
                json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(pro_path));
                json_object_object_add(*error_info, ERROR_VAL,
                    json_object_new_string(dal_json_object_get_str(pf_id_json)));
                debug_log(DLOG_ERROR, "%s: pf_id %d invalid for chip%d port 0.", __FUNCTION__, pf_id,
                    config_port_unit->headinfo.chipid);
                return PROPERTY_VALUE_ERROR;
            }
        } else if (config_port_unit->headinfo.portid == 1) {
            if ((pf_id % 2) == 1) {
                config_port_unit->portconfig.NparPF[port_index].pf_id = pf_id;
            } else {
                json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(pro_path));
                json_object_object_add(*error_info, ERROR_VAL,
                    json_object_new_string(dal_json_object_get_str(pf_id_json)));
                debug_log(DLOG_ERROR, "%s: pf_id %d invalid for chip%d port 1.", __FUNCTION__, pf_id,
                    config_port_unit->headinfo.chipid);
                return PROPERTY_VALUE_ERROR;
            }
        }

        config_port_unit->portconfig.NparPF[port_index].pf_id = pf_id;
    }

    return RET_OK;
}