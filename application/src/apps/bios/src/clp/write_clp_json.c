

#include "write_clp_json.h"
#include "clp_config.h"

LOCAL gint32 _set_port_config_for_220_221(guint16 board_id, json_object *port_json, CLP_RESPONSE_UNIT *config_unit_p,
    json_object **error_info, gint32 port_index);
LOCAL gint32 _set_pf_config_for_220_221(json_object *pfs_json, CLP_RESPONSE_UNIT *config_unit_p,
    json_object **error_info, gint32 port_index);
LOCAL gint32 _set_boottotarget_config(gint32 target_index, json_object *target_json, CLP_RESPONSE_UNIT *config_unit_p,
    json_object **error_info, gint32 port_index, const gchar *targets_name);
LOCAL gint32 _set_port_config(guint16 board_id, json_object *port_json, CLP_RESPONSE_UNIT *config_unit_p,
    json_object **error_info, gint32 port_index);
LOCAL gint32 _set_pfs_config(json_object *pfs_json, CLP_RESPONSE_UNIT *config_unit_p, json_object **error_info,
    gint32 port_index);
LOCAL gint32 _set_pf_config(gint32 pf_index, json_object *pf_json, CLP_RESPONSE_UNIT *config_unit_p,
    json_object **error_info, gint32 port_index);
LOCAL gint32 _set_pf_config_pfid(gint32 pf_index, json_object *pf_json, CLP_RESPONSE_UNIT *config_unit_p,
    json_object **error_info, gint32 port_index);
LOCAL gint32 _set_pf_config_band(gint32 pf_index, json_object *pf_json, CLP_RESPONSE_UNIT *config_unit_p,
    json_object **error_info, gint32 port_index);
LOCAL gint32 _set_pf_config_vlan(gint32 pf_index, json_object *pf_json, CLP_RESPONSE_UNIT *config_unit_p,
    json_object **error_info, gint32 port_index);
LOCAL gint32 _set_pf_config_pfid_check(gint32 pf_index, gint32 pf_id, json_object *obj_json,
    CLP_RESPONSE_UNIT *config_unit_p, json_object **error_info, const gchar *prop_path);
LOCAL gint32 _set_pf_config_vlanid(gint32 pf_index, json_object *obj_json, CLP_RESPONSE_UNIT *config_unit_p,
    json_object **error_info, gchar *prop_path);
LOCAL gint32 _set_pf_config_vlanpriority(gint32 pf_index, json_object *obj_json, CLP_RESPONSE_UNIT *config_unit_p,
    json_object **error_info, const gchar *prop_path);
LOCAL gint32 _check_pf_trunk_range_sequence(const gchar *prop_val);


gint32 _json_set_and_rules_check(const gchar *devicename, const gchar *config_data, json_object **error_info,
    GSList **config_list, CLP_SET_S clp_set)
{
    json_object *ports_json = NULL;
    json_object *port_json = NULL;
    json_object *config_json = NULL;
    gint32 ret = 0;
    gint32 ports_num = 0;
    CLP_RESPONSE_UNIT *config_unit_p = NULL;
    gint32 i = 0;
    
    gchar prop_path[MAX_PROPERTY_PATH_LEN + 1] = {0};
    

    
    if (devicename == NULL || error_info == NULL || config_data == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }
    config_json = json_tokener_parse(config_data);
    if (config_json == NULL) {
        debug_log(DLOG_ERROR, "%s: json_tokener_parse failed", __FUNCTION__);
        return RET_ERR;
    }

    ret = json_object_object_get_ex(config_json, RFPROP_CLP_PORTS_CONFIG, &ports_json);
    if (ret) {
        
        ret = _check_ports_valid(clp_set.board_id, ports_json, error_info);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: _check_ports_valid failed", __FUNCTION__);
            goto err_out;
        }

        
        ports_num = json_object_array_length(ports_json);

        for (i = 0; i < ports_num; i++) {
            port_json = json_object_array_get_idx(ports_json, i);
            config_unit_p = (CLP_RESPONSE_UNIT *)g_malloc0(sizeof(CLP_RESPONSE_UNIT));
            if (config_unit_p == NULL) {
                ret = RET_ERR;
                debug_log(DLOG_ERROR, "%s: g_malloc0 failed", __FUNCTION__);
                goto err_out;
            }

            
            (void)_initial_config_unit(config_unit_p);
            config_unit_p->board_id = clp_set.board_id;

            (void)strncpy_s(config_unit_p->headinfo.devicename, sizeof(config_unit_p->headinfo.devicename), devicename,
                sizeof(config_unit_p->headinfo.devicename) - 1);
            
            ret = _set_header_info(clp_set.board_id, port_json, config_unit_p, i);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: _set_header_info failed", __FUNCTION__);
                goto err_out;
            }
            if (clp_set.clp_flag == CLP_520_522) {
                ret = _set_port_config(clp_set.board_id, port_json, config_unit_p, error_info, i);
            } else {
                ret = _set_port_config_for_220_221(clp_set.board_id, port_json, config_unit_p, error_info, i);
            }
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: _set_port_config failed, ret is %d", __FUNCTION__, ret);
                goto err_out;
            }

            *config_list = g_slist_append(*config_list, config_unit_p);
            config_unit_p = NULL;
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


LOCAL gint32 _set_port_config_for_220_221(guint16 board_id, json_object *port_json, CLP_RESPONSE_UNIT *config_unit_p,
    json_object **error_info, gint32 port_index)
{
    errno_t securec_rv;
    json_object *obj_json = NULL;
    gint32 ret = 0;
    const gchar *prop_val = NULL;
    
    gchar prop_path[MAX_PROPERTY_PATH_LEN + 1] = {0};
    

    
    if (port_json == NULL || config_unit_p == NULL || error_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    ret = json_object_object_get_ex(port_json, RFPROP_CLP_MULTI_MODE, &obj_json);
    if (ret && obj_json != NULL) {
        
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_MULTI_MODE);
        
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_UNKNOW;
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(port_json, RFPROP_CLP_SRIOVENABLED, &obj_json);
    if (ret && obj_json != NULL) {
        
        securec_rv = snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_SRIOVENABLED);
        if (securec_rv <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, securec_rv);
        }
        

        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_UNKNOW;
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(port_json, RFPROP_CLP_BOOT_ENABLED, &obj_json);
    if (ret && obj_json != NULL) {
        
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_BOOT_ENABLED);
        
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_UNKNOW;
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(port_json, CLP_KEY_BOOT_PROTOCOL, &obj_json);
    if (ret && obj_json != NULL) {
        
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, CLP_KEY_BOOT_PROTOCOL);
        
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_UNKNOW;
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(port_json, RFPROP_CLP_BOOTTOTARGET, &obj_json);
    if (ret && obj_json != NULL) {
        
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_BOOTTOTARGET);
        
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_UNKNOW;
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(port_json, RFPROP_CLP_SAN_BOOT_ENABLE, &obj_json);
    if (ret && obj_json != NULL) {
        config_unit_p->portconfig.SANBootEnable = json_object_get_boolean(obj_json);
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(port_json, RFPROP_CLP_LINK_CONFIG, &obj_json);
    if (ret && obj_json != NULL) {
        if (MHFA_ID == board_id) {
            prop_val = dal_json_object_get_str(obj_json);
            securec_rv = strncpy_s(config_unit_p->portconfig.LinkConfig, MAX_LEN_32 + 1, prop_val, MAX_LEN_32);
            if (securec_rv != EOK) {
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, securec_rv);
            }
        } else {
            
            (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
                MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_LINK_CONFIG);
            
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            return PROPERTY_UNKNOW;
        }
    }

    obj_json = NULL;
    prop_val = NULL;
    ret = json_object_object_get_ex(port_json, RFPROP_CLP_SERDES_CONFIG, &obj_json);
    if (ret && obj_json != NULL) {
        if (MHFA_ID == board_id) {
            prop_val = dal_json_object_get_str(obj_json);
            securec_rv = strncpy_s(config_unit_p->portconfig.SerDesConfig, MAX_LEN_32 + 1, prop_val, MAX_LEN_32);
            if (securec_rv != EOK) {
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, securec_rv);
            }
        } else {
            
            (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
                MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_SERDES_CONFIG);
            
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            return PROPERTY_UNKNOW;
        }
    }

    obj_json = NULL;

    ret = json_object_object_get_ex(port_json, RFPROP_CLP_VLANID, &obj_json);
    if (ret && obj_json != NULL) {
        
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_VLANID);
        
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_UNKNOW;
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(port_json, RFPROP_CLP_PFS_INFO, &obj_json);
    if (ret) {
        ret = _set_pf_config_for_220_221(obj_json, config_unit_p, error_info, port_index);
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

    
    
    obj_json = NULL;
    ret = json_object_object_get_ex(port_json, RFPROP_CLP_BOOT_TARGETS, &obj_json);
    if (ret) {
        ret = _set_boottotargets_config(obj_json, config_unit_p, error_info, RFPROP_CLP_BOOT_TARGETS, port_index);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: _set_boottotargets_config failed, ret is %d", __FUNCTION__, ret);
            return ret;
        }
    } else {
        obj_json = NULL;

        ret = json_object_object_get_ex(port_json, RFPROP_CLP_BOOT_TO_TARGETS, &obj_json);
        if (ret) {
            ret =
                _set_boottotargets_config(obj_json, config_unit_p, error_info, RFPROP_CLP_BOOT_TO_TARGETS, port_index);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: _set_boottotargets_config failed, ret is %d", __FUNCTION__, ret);
                return ret;
            }
        }
    }

    

    return RET_OK;
}


LOCAL gint32 _set_pf_config_for_220_221(json_object *pfs_json, CLP_RESPONSE_UNIT *config_unit_p,
    json_object **error_info, gint32 port_index)
{
    gint32 iRet = -1;
    gint32 ret = 0;
    gint32 pf_id = 0;
    gint32 pfs_num = 0;
    json_object *pf_json = NULL;
    json_object *obj_json = NULL;
    const gchar *prop_val = NULL;
    gchar  tmp_str[MAX_LEN_32 + 1] = {0};
    
    gchar prop_path[MAX_PROPERTY_PATH_LEN + 1] = {0};
    

    
    if (pfs_json == NULL || config_unit_p == NULL || error_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }
    pfs_num = json_object_array_length(pfs_json);
    
    if (pfs_num > 1) {
        iRet = snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO);
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        debug_log(DLOG_ERROR, "%s: pfs_num invalid, snprintf_s ret = %d", __FUNCTION__, iRet);
        return PORT_PF_TARGET_EXCEED;
    }
    if (pfs_num < 1) {
        iRet = snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO);
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        debug_log(DLOG_ERROR, "%s: pfs_num invalid, snprintf_s ret = %d", __FUNCTION__, iRet);
        return PORT_PF_TARGET_INSUFFICIENT;
    }
    

    pf_json = json_object_array_get_idx(pfs_json, 0);
    ret = json_object_object_get_ex(pf_json, RFPROP_CLP_PF_ID, &obj_json);

    
    (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
    (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
        MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, 0, RFPROP_CLP_PF_ID);
    

    if (ret) {
        if (json_type_int != json_object_get_type(obj_json)) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            debug_log(DLOG_ERROR, "%s: json_type error", __FUNCTION__);
            return PROPERTY_VALUE_NOT_INLIST;
        }

        pf_id = json_object_get_int(obj_json);
        if (pf_id != MIN_PF_ID && pf_id != 1) {
            debug_log(DLOG_ERROR, "%s: pf_id %d invalid.", __FUNCTION__, pf_id);
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            return PROPERTY_VALUE_NOT_INLIST;
        }

        
        if (config_unit_p->headinfo.portid == 0) {
            if ((pf_id % 2) == 0) {
                config_unit_p->portconfig.NparPF[0].pf_id = pf_id;
            } else {
                debug_log(DLOG_ERROR, "%s: pf_id %d invalid for chip%d port 0.", __FUNCTION__, pf_id,
                    config_unit_p->headinfo.chipid);
                json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
                json_object_object_add(*error_info, ERROR_VAL,
                    json_object_new_string(dal_json_object_get_str(obj_json)));
                return PROPERTY_VALUE_ERROR;
            }
        } else if (config_unit_p->headinfo.portid == 1) {
            if ((pf_id % 2) == 1) {
                config_unit_p->portconfig.NparPF[0].pf_id = pf_id;
            } else {
                debug_log(DLOG_ERROR, "%s: pf_id %d invalid for chip%d port 1.", __FUNCTION__, pf_id,
                    config_unit_p->headinfo.chipid);
                json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
                json_object_object_add(*error_info, ERROR_VAL,
                    json_object_new_string(dal_json_object_get_str(obj_json)));
                return PROPERTY_VALUE_ERROR;
            }
        }

        config_unit_p->portconfig.NparPF[0].pf_id = pf_id;
    } else {
        debug_log(DLOG_ERROR, "%s: set pf_id error.", __FUNCTION__);
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_MISSING;
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(pf_json, RFPROP_CLP_PROTOCOL, &obj_json);
    if (ret && obj_json != NULL) {
        
        (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, 0,
            RFPROP_CLP_PROTOCOL);
        
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_UNKNOW;
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(pf_json, CLP_KEY_ETH_MAC, &obj_json);
    if (ret && obj_json != NULL) {
        
        (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, 0, CLP_KEY_ETH_MAC);
        
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_UNKNOW;
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(pf_json, CLP_KEY_ISCSI_MAC, &obj_json);
    if (ret && obj_json != NULL) {
        
        (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, 0, CLP_KEY_ISCSI_MAC);
        
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_UNKNOW;
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(pf_json, CLP_KEY_FCOE_MAC, &obj_json);
    if (ret && obj_json != NULL) {
        
        (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, 0, CLP_KEY_FCOE_MAC);
        
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_UNKNOW;
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(pf_json, RFPROP_CLP_MIN_BAND, &obj_json);
    if (ret && obj_json != NULL) {
        
        (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, 0,
            RFPROP_CLP_MIN_BAND);
        
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_UNKNOW;
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(pf_json, RFPROP_CLP_MAX_BAND, &obj_json);
    if (ret && obj_json != NULL) {
        
        (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, 0,
            RFPROP_CLP_MAX_BAND);
        
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_UNKNOW;
    }

    obj_json = NULL;
    
    
    ret = json_object_object_get_ex(pf_json, CLP_KEY_WWPN, &obj_json);
    if (!ret) {
        obj_json = NULL;
        ret = json_object_object_get_ex(pf_json, CLP_KEY_FCOE_WWPN, &obj_json);
        (void)memset_s(tmp_str, MAX_LEN_32 + 1, 0, MAX_LEN_32 + 1);
        (void)strncpy_s(tmp_str, MAX_LEN_32 + 1, CLP_KEY_FCOE_WWPN, strlen(CLP_KEY_FCOE_WWPN));
    } else {
        (void)memset_s(tmp_str, MAX_LEN_32 + 1, 0, MAX_LEN_32 + 1);
        (void)strncpy_s(tmp_str, MAX_LEN_32 + 1, CLP_KEY_WWPN, strlen(CLP_KEY_WWPN));
    }

    

    
    (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
    iRet = snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
        MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, 0, tmp_str);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
    }
    

    if (ret && obj_json != NULL) {
        prop_val = dal_json_object_get_str(obj_json);
        if (strlen(prop_val) != MAX_PROP_LEN_16 && strlen(prop_val) != MIN_PROP_LEN) {
            debug_log(DLOG_ERROR, "%s: strlen error.", __FUNCTION__);
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            return PROPERTY_VALUE_ERROR;
        }

        
        ret = g_regex_match_simple(REGEX_WWPN_WWNN, prop_val, (GRegexCompileFlags)0, (GRegexMatchFlags)0);
        if (ret != TRUE && strlen(prop_val) != MIN_PROP_LEN) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            debug_log(DLOG_ERROR, "%s: g_regex_match_simple failed", __FUNCTION__);
            return PROPERTY_VALUE_ERROR;
        }
        // mezz221配置属性PermanentAddress即是redfish接口中WWPN
        (void)strncpy_s(config_unit_p->portconfig.NparPF[0].FCoEWWPN, MAX_LEN_32 + 1, prop_val, MAX_LEN_32);
    }

    obj_json = NULL;
    prop_val = NULL;

    
    
    ret = json_object_object_get_ex(pf_json, CLP_KEY_WWNN, &obj_json);
    if (!ret) {
        obj_json = NULL;
        ret = json_object_object_get_ex(pf_json, CLP_KEY_FCOE_WWNN, &obj_json);
        (void)memset_s(tmp_str, MAX_LEN_32 + 1, 0, MAX_LEN_32 + 1);
        (void)strncpy_s(tmp_str, MAX_LEN_32 + 1, CLP_KEY_FCOE_WWNN, strlen(CLP_KEY_FCOE_WWNN));
    } else {
        (void)memset_s(tmp_str, MAX_LEN_32 + 1, 0, MAX_LEN_32 + 1);
        (void)strncpy_s(tmp_str, MAX_LEN_32 + 1, CLP_KEY_WWNN, strlen(CLP_KEY_WWNN));
    }

    

    
    (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
    iRet = snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
        MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, 0, tmp_str);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
    }
    

    if (ret && obj_json != NULL) {
        prop_val = dal_json_object_get_str(obj_json);
        if (strlen(prop_val) != MAX_PROP_LEN_16 && strlen(prop_val) != MIN_PROP_LEN) {
            debug_log(DLOG_ERROR, "%s: strlen error.", __FUNCTION__);
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            return PROPERTY_VALUE_ERROR;
        }

        
        ret = g_regex_match_simple(REGEX_WWPN_WWNN, prop_val, (GRegexCompileFlags)0, (GRegexMatchFlags)0);
        if (ret != TRUE && strlen(prop_val) != MIN_PROP_LEN) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            debug_log(DLOG_ERROR, "%s: g_regex_match_simple failed", __FUNCTION__);
            return PROPERTY_VALUE_ERROR;
        }
        // mezz221配置属性NetworkAddresses[0]即是redfish接口中WWNN
        (void)strncpy_s(config_unit_p->portconfig.NparPF[0].FCoEWWNN, MAX_LEN_32 + 1, prop_val, MAX_LEN_32);
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(pf_json, RFPROP_CLP_VLAN_MODE, &obj_json);
    if (ret && obj_json != NULL) {
        
        (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, 0,
            RFPROP_CLP_VLAN_MODE);
        
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_UNKNOW;
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(pf_json, RFPROP_CLP_VLANID, &obj_json);
    if (ret && obj_json != NULL) {
        
        (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, 0, RFPROP_CLP_VLANID);
        
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_UNKNOW;
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(pf_json, RFPROP_CLP_VLAN_PRIORITY, &obj_json);
    if (ret && obj_json != NULL) {
        
        (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, 0,
            RFPROP_CLP_VLAN_PRIORITY);
        
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_UNKNOW;
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(pf_json, RFPROP_CLP_VLAN_TRUNKRANGE, &obj_json);
    if (ret && obj_json != NULL) {
        
        (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, 0,
            RFPROP_CLP_VLAN_TRUNKRANGE);
        
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_UNKNOW;
    }

    return RET_OK;
}

gint32 _set_boottotargets_config(json_object *targets_json, CLP_RESPONSE_UNIT *config_unit_p, json_object **error_info,
    const gchar *targets_name, gint32 port_index)
{
    gint32 ret = 0;
    gint32 i = 0;
    gint32 target_num = 0;
    json_object *target_json = NULL;
    
    gchar prop_path[MAX_PROPERTY_PATH_LEN + 1] = {0};
    

    
    if (targets_json == NULL || config_unit_p == NULL || error_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    target_num = json_object_array_length(targets_json);
    
    if (target_num > TARGET_MAX_NUM) {
        ret = snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, targets_name);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret);
        }

        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        debug_log(DLOG_ERROR, "%s: target_num invalid", __FUNCTION__);
        return PORT_PF_TARGET_EXCEED;
    }
    

    for (i = 0; i < target_num; i++) {
        target_json = json_object_array_get_idx(targets_json, i);

        ret = _set_boottotarget_config(i, target_json, config_unit_p, error_info, port_index, targets_name);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: _set_boottotarget_config failed, ret is %d", __FUNCTION__, ret);
            return ret;
        }
    }

    return RET_OK;
}

LOCAL gint32 _set_boottotarget_config(gint32 target_index, json_object *target_json, CLP_RESPONSE_UNIT *config_unit_p,
    json_object **error_info, gint32 port_index, const gchar *targets_name)
{
    gint32 iRet = -1;
    gint32 ret = 0;
    gint32 target_id = 0;
    json_object *obj_json = NULL;
    gint16 bootlun = 0;
    const gchar *prop_val = NULL;
    guint32 i = 0;
    gchar tmp_str[MAX_LEN_32 + 1] = {0};
    
    gchar prop_path[MAX_PROPERTY_PATH_LEN + 1] = {0};
    

    
    if (target_json == NULL || config_unit_p == NULL || error_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    ret = json_object_object_get_ex(target_json, RFPROP_CLP_TARGET_ID, &obj_json);

    
    iRet = snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
        MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, targets_name, target_index,
        RFPROP_CLP_TARGET_ID);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
    }
    

    if (ret) {
        if (json_type_int != json_object_get_type(obj_json)) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            debug_log(DLOG_ERROR, "%s: json_type error", __FUNCTION__);
            return PROPERTY_VALUE_NOT_INLIST;
        }

        target_id = json_object_get_int(obj_json);
        if (target_id < MIN_TARGET_ID || target_id > MAX_TARGET_ID) {
            debug_log(DLOG_ERROR, "%s: target_id %d invalid.", __FUNCTION__, target_id);
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            return PROPERTY_VALUE_ERROR;
        }

        // targetid重复性校验
        while (i < target_index) {
            if (target_id == config_unit_p->portconfig.FCoEBootTarget[i].target_id) {
                debug_log(DLOG_ERROR, "%s: target_id %d is the same as that of other parameters.", __FUNCTION__,
                    target_id);
                json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
                json_object_object_add(*error_info, ERROR_VAL,
                    json_object_new_string(dal_json_object_get_str(obj_json)));
                return PROPERTY_VALUE_ERROR;
            }

            i++;
        }

        config_unit_p->portconfig.FCoEBootTarget[target_index].target_id = target_id;
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(target_json, RFPROP_CLP_TARGET_ENABLED, &obj_json);
    // 221卡不支持targetenable设置
    
    if ((ret == TRUE) && (obj_json != NULL) &&
        (config_unit_p->board_id == MHFA_ID || config_unit_p->board_id == MHFB_ID)) {
        (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
        iRet = snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, targets_name, target_index,
            RFPROP_CLP_TARGET_ENABLED);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
        }
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_UNKNOW;
    }
    

    if (ret && obj_json != NULL) {
        config_unit_p->portconfig.FCoEBootTarget[target_index].TargetEnable = json_object_get_boolean(obj_json);
    }

    if (config_unit_p->portconfig.FCoEBootTarget[target_index].TargetEnable == TRUE ||
        config_unit_p->board_id == MHFA_ID || config_unit_p->board_id == MHFB_ID) {
        
        
        obj_json = NULL;
        ret = json_object_object_get_ex(target_json, CLP_KEY_WWPN, &obj_json);
        if (!ret) {
            obj_json = NULL;
            ret = json_object_object_get_ex(target_json, CLP_KEY_FCOE_WWPN, &obj_json);
            (void)memset_s(tmp_str, MAX_LEN_32 + 1, 0, MAX_LEN_32 + 1);
            (void)strncpy_s(tmp_str, sizeof(tmp_str), CLP_KEY_FCOE_WWPN, strlen(CLP_KEY_FCOE_WWPN));
        } else {
            (void)memset_s(tmp_str, MAX_LEN_32 + 1, 0, MAX_LEN_32 + 1);
            (void)strncpy_s(tmp_str, sizeof(tmp_str), CLP_KEY_WWPN, strlen(CLP_KEY_WWPN));
        }

        

        if (ret && obj_json != NULL) {
            prop_val = dal_json_object_get_str(obj_json);
            ret = g_regex_match_simple(REGEX_WWPN_WWNN, prop_val, (GRegexCompileFlags)0, (GRegexMatchFlags)0);
            
            if (ret != TRUE && strlen(prop_val) != 0) {
                (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
                iRet = snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
                    MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, targets_name, target_index,
                    tmp_str);
                if (iRet <= 0) {
                    debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
                }
                json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
                json_object_object_add(*error_info, ERROR_VAL,
                    json_object_new_string(dal_json_object_get_str(obj_json)));
                debug_log(DLOG_ERROR, "%s: g_regex_match_simple failed", __FUNCTION__);
                return PROPERTY_VALUE_ERROR;
            }
            

            (void)strncpy_s(config_unit_p->portconfig.FCoEBootTarget[target_index].FCoEWWPN, MAX_LEN_32 + 1, prop_val,
                MAX_LEN_32);
        }

        ret = json_object_object_get_ex(target_json, RFPROP_CLP_TARGET_LUN, &obj_json);
        if (ret && obj_json != NULL) {
            
            if (json_type_int != json_object_get_type(obj_json)) {
                (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
                iRet = snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
                    MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, targets_name, target_index,
                    RFPROP_CLP_TARGET_LUN);
                if (iRet <= 0) {
                    debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
                }
                json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
                json_object_object_add(*error_info, ERROR_VAL,
                    json_object_new_string(dal_json_object_get_str(obj_json)));
                debug_log(DLOG_ERROR, "%s: json_type error", __FUNCTION__);
                return PROPERTY_VALUE_NOT_INLIST;
            }
            

            bootlun = (gint16)json_object_get_int(obj_json);
            if (bootlun < MIN_LUN_NUM || bootlun > MAX_LUN_NUM) {
                debug_log(DLOG_ERROR, "%s: bootlun %d invalid.", __FUNCTION__, bootlun);
                (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
                iRet = snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
                    MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, targets_name, target_index,
                    RFPROP_CLP_TARGET_LUN);
                if (iRet <= 0) {
                    debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
                }
                json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
                json_object_object_add(*error_info, ERROR_VAL,
                    json_object_new_string(dal_json_object_get_str(obj_json)));
                return PROPERTY_VALUE_NOT_INLIST;
            }

            config_unit_p->portconfig.FCoEBootTarget[target_index].bootlun = bootlun;
        }
    }
    
    else if (config_unit_p->portconfig.FCoEBootTarget[target_index].TargetEnable == FALSE &&
        config_unit_p->board_id != MHFA_ID && config_unit_p->board_id != MHFB_ID) {
        obj_json = NULL;
        ret = json_object_object_get_ex(target_json, CLP_KEY_WWPN, &obj_json);
        if (!ret) {
            obj_json = NULL;
            ret = json_object_object_get_ex(target_json, CLP_KEY_FCOE_WWPN, &obj_json);
            (void)memset_s(tmp_str, MAX_LEN_32 + 1, 0, MAX_LEN_32 + 1);
            (void)strncpy_s(tmp_str, sizeof(tmp_str), CLP_KEY_FCOE_WWPN, strlen(CLP_KEY_FCOE_WWPN));
        } else {
            (void)memset_s(tmp_str, MAX_LEN_32 + 1, 0, MAX_LEN_32 + 1);
            (void)strncpy_s(tmp_str, sizeof(tmp_str), CLP_KEY_WWPN, strlen(CLP_KEY_WWPN));
        }

        if (ret && obj_json != NULL) {
            debug_log(DLOG_ERROR, "%s: targetenabled false when setting wwpn.", __FUNCTION__);
            
            (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
            iRet = snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
                MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, targets_name, target_index, tmp_str);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
            }
            
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_REL_PROP, json_object_new_string(RFPROP_CLP_TARGET_ENABLED));
            json_object_object_add(*error_info, ERROR_REL_VAL, json_object_new_string("false"));
            return MODE_UNSUPPORTED;
        }

        obj_json = NULL;
        ret = json_object_object_get_ex(target_json, RFPROP_CLP_TARGET_LUN, &obj_json);
        if (ret && obj_json != NULL) {
            debug_log(DLOG_ERROR, "%s: targetenabled false when setting wwpn.", __FUNCTION__);
            
            (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
            iRet = snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
                MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, targets_name, target_index,
                RFPROP_CLP_TARGET_LUN);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
            }
            
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_REL_PROP, json_object_new_string(RFPROP_CLP_TARGET_ENABLED));
            json_object_object_add(*error_info, ERROR_REL_VAL, json_object_new_string("false"));
            return MODE_UNSUPPORTED;
        }
    }

    
    return RET_OK;
}


LOCAL gint32 _set_port_config(guint16 board_id, json_object *port_json, CLP_RESPONSE_UNIT *config_unit_p,
    json_object **error_info, gint32 port_index)
{
    gint32 iRet = -1;
    errno_t securec_rv;
    json_object *obj_json = NULL;
    gint32 ret = 0;
    gint32 vlanid = 0;
    const gchar *prop_val = NULL;
    gchar tmp_str[MAX_LEN_32 + 1] = {0};
    gchar prop_path[MAX_PROPERTY_PATH_LEN + 1] = {0};

    
    if (port_json == NULL || config_unit_p == NULL || error_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    ret = json_object_object_get_ex(port_json, RFPROP_CLP_MULTI_MODE, &obj_json);
    if (ret && obj_json != NULL) {
        
        prop_val = dal_json_object_get_str(obj_json);
        securec_rv = strncpy_s(config_unit_p->chipconfig.MultifunctionMode, MAX_LEN_32, prop_val, MAX_LEN_32 - 1);
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, securec_rv);
        }

        
        if ((MXEP_ID == board_id) || (MXEN_ID == board_id)) {
            if (g_ascii_strcasecmp(config_unit_p->chipconfig.MultifunctionMode, CLP_VAL_NPAR) == 0) {
                config_unit_p->cardinfo.vid = MXEP_VID_NPAR;
                config_unit_p->cardinfo.did = MXEP_DID_NPAR;
            } else {
                config_unit_p->cardinfo.vid = MXEP_VID;
                config_unit_p->cardinfo.did = MXEP_DID;
            }
        }
    } else {
        
        debug_log(DLOG_ERROR, "%s:  get MultifunctionMode failed", __FUNCTION__);
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_MULTI_MODE);
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        
        return PROPERTY_MISSING;
    }

    obj_json = NULL;
    prop_val = NULL;
    
    ret = json_object_object_get_ex(port_json, RFPROP_CLP_SRIOVENABLED, &obj_json);
    if (ret) {
        
        if (json_object_get_boolean(obj_json) == TRUE &&
            g_ascii_strcasecmp(config_unit_p->chipconfig.MultifunctionMode, CLP_VAL_NPAR) == 0) {
            debug_log(DLOG_ERROR, "%s: SRIOV not be setting true ", __FUNCTION__);
            
            (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
            (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
                MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_SRIOVENABLED);
            
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_REL_PROP, json_object_new_string(RFPROP_CLP_MULTI_MODE));
            json_object_object_add(*error_info, ERROR_REL_VAL, json_object_new_string(CLP_VAL_NPAR));
            return MODE_UNSUPPORTED;
        }

        if (json_type_null != json_object_get_type(obj_json)) {
            config_unit_p->portconfig.SRIOVEnabled = json_object_get_boolean(obj_json);
        }
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(port_json, RFPROP_CLP_BOOT_ENABLED, &obj_json);
    if (ret && obj_json != NULL) {
        config_unit_p->portconfig.BootEnable = json_object_get_boolean(obj_json);
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(port_json, CLP_KEY_BOOT_PROTOCOL, &obj_json);
    if (ret && obj_json != NULL) {
        prop_val = dal_json_object_get_str(obj_json);
        securec_rv = strncpy_s(config_unit_p->portconfig.BootProtocol, MAX_LEN_32 + 1, prop_val, MAX_LEN_32);
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, securec_rv);
        }
    } else {
        debug_log(DLOG_ERROR, "%s:  get BootProtocol failed", __FUNCTION__);
        
        (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, CLP_KEY_BOOT_PROTOCOL);
        
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_MISSING;
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(port_json, RFPROP_CLP_BOOTTOTARGET, &obj_json);
    if (ret && obj_json != NULL) {
        config_unit_p->portconfig.BootToTarget = json_object_get_boolean(obj_json);

        // 当bootprotocol不为fcoe时boot to target不允许设置为true
        if (config_unit_p->portconfig.BootToTarget &&
            g_ascii_strncasecmp(config_unit_p->portconfig.BootProtocol, CLP_VAL_FCOE, strlen(CLP_VAL_FCOE)) != 0) {
            debug_log(DLOG_ERROR, "%s:  FCOE not support", __FUNCTION__);
            
            (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
            (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
                MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_BOOTTOTARGET);
            
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_REL_PROP, json_object_new_string(CLP_KEY_BOOT_PROTOCOL));
            json_object_object_add(*error_info, ERROR_REL_VAL,
                json_object_new_string(config_unit_p->portconfig.BootProtocol));
            return MODE_UNSUPPORTED;
        }
    }

    obj_json = NULL;

    ret = json_object_object_get_ex(port_json, RFPROP_CLP_VLANID, &obj_json);
    
    (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
    (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
        MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_VLANID);
    

    if (ret && obj_json != NULL) {
        if (json_type_int != json_object_get_type(obj_json)) {
            debug_log(DLOG_ERROR, "%s: json_type error", __FUNCTION__);
            return RET_ERR;
        }

        
        
        
        if (g_ascii_strcasecmp(config_unit_p->portconfig.BootProtocol, CLP_VAL_PXE) != 0) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_REL_PROP, json_object_new_string(CLP_KEY_BOOT_PROTOCOL));
            json_object_object_add(*error_info, ERROR_REL_VAL,
                json_object_new_string(config_unit_p->portconfig.BootProtocol));
            debug_log(DLOG_ERROR, "%s: Bootprotocol is not PXE when set vlanid", __FUNCTION__);
            return MODE_UNSUPPORTED;
        }

        vlanid = json_object_get_int(obj_json);
        if (vlanid < MIN_VLAN_ID || vlanid > MAX_VLAN_ID) {
            debug_log(DLOG_ERROR, "%s: vlanid %d  invalid.", __FUNCTION__, vlanid);
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            return PROPERTY_VALUE_ERROR;
        }

        config_unit_p->portconfig.VlanId = vlanid;
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(port_json, RFPROP_CLP_SAN_BOOT_ENABLE, &obj_json);
    if (ret && obj_json != NULL) {
        
        (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_SAN_BOOT_ENABLE);
        
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_UNKNOW;
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(port_json, RFPROP_CLP_LINK_CONFIG, &obj_json);
    if (ret && obj_json != NULL) {
        
        (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_LINK_CONFIG);
        
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_UNKNOW;
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(port_json, RFPROP_CLP_SERDES_CONFIG, &obj_json);
    if (ret && obj_json != NULL) {
        
        (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_SERDES_CONFIG);
        
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_UNKNOW;
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(port_json, RFPROP_CLP_PFS_INFO, &obj_json);
    if (ret) {
        ret = _set_pfs_config(obj_json, config_unit_p, error_info, port_index);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: _set_pfs_config failed", __FUNCTION__);
            return ret;
        }
    } else {
        debug_log(DLOG_ERROR, "%s:  get PFSConfig failed", __FUNCTION__);
        
        (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO);
        
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_MISSING;
    }

    obj_json = NULL;
    
    
    ret = json_object_object_get_ex(port_json, RFPROP_CLP_BOOT_TARGETS, &obj_json);
    if (!ret) {
        obj_json = NULL;
        ret = json_object_object_get_ex(port_json, RFPROP_CLP_BOOT_TO_TARGETS, &obj_json);
        (void)strncpy_s(tmp_str, sizeof(tmp_str), RFPROP_CLP_BOOT_TO_TARGETS, strlen(RFPROP_CLP_BOOT_TO_TARGETS));
    } else {
        (void)strncpy_s(tmp_str, sizeof(tmp_str), RFPROP_CLP_BOOT_TARGETS, strlen(RFPROP_CLP_BOOT_TARGETS));
    }

    

    if (ret) {
        if (config_unit_p->portconfig.BootToTarget == TRUE) {
            ret = _set_boottotargets_config(obj_json, config_unit_p, error_info, tmp_str, port_index);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: _set_boottotargets_config failed, ret is %d", __FUNCTION__, ret);
                return ret;
            }
        } else {
            debug_log(DLOG_ERROR, "%s:  set boottotargets failed", __FUNCTION__);
            
            (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
            iRet = snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
                MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, tmp_str);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
            }
            
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_REL_PROP, json_object_new_string(RFPROP_CLP_BOOTTOTARGET));
            json_object_object_add(*error_info, ERROR_REL_VAL, json_object_new_string("false"));
            return MODE_UNSUPPORTED;
        }
    }

    return RET_OK;
}


LOCAL gint32 _set_pfs_config(json_object *pfs_json, CLP_RESPONSE_UNIT *config_unit_p, json_object **error_info,
    gint32 port_index)
{
    gint32 ret = 0;
    json_object *pf_json = NULL;
    gint32 pfs_num = 0;
    gint32 i = 0;
    
    gchar prop_path[MAX_PROPERTY_PATH_LEN + 1] = {0};
    

    
    if (pfs_json == NULL || config_unit_p == NULL || error_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    
    (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
        MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO);

    pfs_num = json_object_array_length(pfs_json);
    if (pfs_num > PORT_PF_MAX) { // pfs_num将用于长度为PORT_PF_MAX数组的下标
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        debug_log(DLOG_ERROR, "%s: pfs_num (%d) is invalid, should less than %d", __FUNCTION__, pfs_num, PORT_PF_MAX);
        return PORT_PF_TARGET_EXCEED;
    }

    if (g_ascii_strcasecmp(config_unit_p->chipconfig.MultifunctionMode, CLP_VAL_NPAR) == 0) {
        if (pfs_num < PORT_PF_MAX) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            debug_log(DLOG_ERROR, "%s: pfs_num insufficient", __FUNCTION__);
            return PORT_PF_TARGET_INSUFFICIENT;
        }
    } else if (g_ascii_strcasecmp(config_unit_p->chipconfig.MultifunctionMode, CLP_VAL_SF) == 0) {
        if (pfs_num > 1) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            debug_log(DLOG_ERROR, "%s: pfs_num invalid", __FUNCTION__);
            return PORT_PF_TARGET_EXCEED;
        }
        if (pfs_num < 1) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            debug_log(DLOG_ERROR, "%s: pfs_num insufficient", __FUNCTION__);
            return PORT_PF_TARGET_INSUFFICIENT;
        }
    }
    

    for (i = 0; i < pfs_num; i++) {
        pf_json = json_object_array_get_idx(pfs_json, i);
        ret = _set_pf_config(i, pf_json, config_unit_p, error_info, port_index);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: _set_pf_config failed", __FUNCTION__);
            return ret;
        }
    }

    
    if ((g_ascii_strcasecmp(config_unit_p->portconfig.BootProtocol, CLP_VAL_ISCSI) == 0 &&
        config_unit_p->portconfig.iSCSI_count == 0) ||
        (g_ascii_strcasecmp(config_unit_p->portconfig.BootProtocol, CLP_VAL_FCOE) == 0 &&
        config_unit_p->portconfig.FCoE_count == 0)) {
        debug_log(DLOG_ERROR, "%s: BootProtocol check error.", __FUNCTION__);
        (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, CLP_KEY_BOOT_PROTOCOL);
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return BOOTPROTOCOL_NOT_FOUND_IN_PFS;
    }
    

    return RET_OK;
}


LOCAL gint32 _set_pf_config(gint32 pf_index, json_object *pf_json, CLP_RESPONSE_UNIT *config_unit_p,
    json_object **error_info, gint32 port_index)
{
    gint32 iRet = -1;
    gint32 ret = 0;
    json_object *obj_json = NULL;
    const gchar *prop_val = NULL;
    
    gchar tmp_str[MAX_LEN_32 + 1] = {0};
    
    gchar prop_path[MAX_PROPERTY_PATH_LEN + 1] = {0};

    
    if (pf_json == NULL || config_unit_p == NULL || error_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }
    
    ret = _set_pf_config_pfid(pf_index, pf_json, config_unit_p, error_info, port_index);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: _set_pf_config_pfid fail.", __FUNCTION__);
        return ret;
    }
    
    ret = json_object_object_get_ex(pf_json, RFPROP_CLP_PROTOCOL, &obj_json);

    
    (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
    (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
        MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, pf_index,
        RFPROP_CLP_PROTOCOL);
    

    if (ret && obj_json != NULL) {
        prop_val = dal_json_object_get_str(obj_json);
        if (prop_val == NULL) {
            return RET_ERR;
        }

        if (g_ascii_strcasecmp(prop_val, "ETHERNET") == 0) {
            (void)strncpy_s(config_unit_p->portconfig.NparPF[pf_index].Protocol, MAX_LEN_32 + 1, prop_val, MAX_LEN_32);
        } else if (g_ascii_strcasecmp(prop_val, "FCoE") == 0) {
            (void)strncpy_s(config_unit_p->portconfig.NparPF[pf_index].Protocol, MAX_LEN_32 + 1, "ETHERNET:FCOE",
                strlen("ETHERNET:FCOE"));
            config_unit_p->portconfig.FCoE_count++;
        } else if (!g_ascii_strcasecmp(prop_val, "iSCSI")) {
            (void)strncpy_s(config_unit_p->portconfig.NparPF[pf_index].Protocol, MAX_LEN_32 + 1, "ETHERNET:ISCSI",
                strlen("ETHERNET:ISCSI"));
            config_unit_p->portconfig.iSCSI_count++;
        } else {
            return RET_ERR;
        }
    } else {
        debug_log(DLOG_ERROR, "%s: set pf_id error.", __FUNCTION__);
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_MISSING;
    }

    
    if (config_unit_p->portconfig.FCoE_count > MAX_FCOE_COUNT ||
        config_unit_p->portconfig.iSCSI_count > MAX_ISCSI_COUNT ||
        (config_unit_p->portconfig.FCoE_count == MAX_FCOE_COUNT &&
        config_unit_p->portconfig.iSCSI_count > MAX_FCOE_COUNT)) {
        debug_log(DLOG_ERROR, "%s: pf config: protocol error.", __FUNCTION__);
        return PROTOCOLS_CHECK_FAIL;
    }

    obj_json = NULL;
    prop_val = NULL;
    ret = json_object_object_get_ex(pf_json, CLP_KEY_ETH_MAC, &obj_json);

    
    (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
    (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
        MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, pf_index,
        CLP_KEY_ETH_MAC);
    

    if (ret && obj_json != NULL) {
        prop_val = dal_json_object_get_str(obj_json);
        if (prop_val == NULL) {
            return RET_ERR;
        }

        if (strlen(prop_val) != MAX_PROP_LEN && strlen(prop_val) != MIN_PROP_LEN) {
            debug_log(DLOG_ERROR, "%s: strlen error.", __FUNCTION__);
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            return PROPERTY_VALUE_ERROR;
        }

        
        ret = g_regex_match_simple(REGEX_MAC, prop_val, (GRegexCompileFlags)0, (GRegexMatchFlags)0);
        if (ret != TRUE && strlen(prop_val) != MIN_PROP_LEN) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            debug_log(DLOG_ERROR, "%s: g_regex_match_simple failed", __FUNCTION__);
            return PROPERTY_VALUE_ERROR;
        }
        (void)strncpy_s(config_unit_p->portconfig.NparPF[pf_index].PermanentAddress,
            sizeof(config_unit_p->portconfig.NparPF[pf_index].PermanentAddress), prop_val,
            sizeof(config_unit_p->portconfig.NparPF[pf_index].PermanentAddress) - 1);
    } else if (ret && obj_json == NULL) {
        debug_log(DLOG_ERROR, "%s: %s is null.", __FUNCTION__, CLP_KEY_ETH_MAC);
        
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        json_object_object_add(*error_info, ERROR_VAL, json_object_new_string("null"));
        
        return PROPERTY_TYPE_ERROR;
    } else {
        debug_log(DLOG_ERROR, "%s: set pf_id error.", __FUNCTION__);
        
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        
        return PROPERTY_MISSING;
    }

    obj_json = NULL;
    prop_val = NULL;
    ret = json_object_object_get_ex(pf_json, CLP_KEY_ISCSI_MAC, &obj_json);

    
    (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
    (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
        MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, pf_index,
        CLP_KEY_ISCSI_MAC);
    

    if (ret && obj_json != NULL) {
        prop_val = dal_json_object_get_str(obj_json);
        if (prop_val == NULL) {
            return RET_ERR;
        }

        if (strlen(prop_val) != MAX_PROP_LEN && strlen(prop_val) != MIN_PROP_LEN) {
            debug_log(DLOG_ERROR, "%s: strlen error.", __FUNCTION__);
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            return PROPERTY_VALUE_ERROR;
        }

        
        ret = g_regex_match_simple(REGEX_MAC, prop_val, (GRegexCompileFlags)0, (GRegexMatchFlags)0);
        if (ret != TRUE && strlen(prop_val) != MIN_PROP_LEN) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            debug_log(DLOG_ERROR, "%s: g_regex_match_simple failed", __FUNCTION__);
            return PROPERTY_VALUE_ERROR;
        }
        (void)strncpy_s(config_unit_p->portconfig.NparPF[pf_index].iScsiAddress,
            sizeof(config_unit_p->portconfig.NparPF[pf_index].iScsiAddress), prop_val,
            sizeof(config_unit_p->portconfig.NparPF[pf_index].iScsiAddress) - 1);
    }

    obj_json = NULL;
    prop_val = NULL;
    ret = json_object_object_get_ex(pf_json, CLP_KEY_FCOE_MAC, &obj_json);

    
    (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
    (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
        MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, pf_index,
        CLP_KEY_FCOE_MAC);
    

    if (ret && obj_json != NULL) {
        prop_val = dal_json_object_get_str(obj_json);
        if (prop_val == NULL) {
            return RET_ERR;
        }

        if (strlen(prop_val) != MAX_PROP_LEN && strlen(prop_val) != MIN_PROP_LEN) {
            debug_log(DLOG_ERROR, "%s: strlen error.", __FUNCTION__);
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            return PROPERTY_VALUE_ERROR;
        }

        
        ret = g_regex_match_simple(REGEX_MAC, prop_val, (GRegexCompileFlags)0, (GRegexMatchFlags)0);
        if (ret != TRUE && strlen(prop_val) != MIN_PROP_LEN) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            debug_log(DLOG_ERROR, "%s: g_regex_match_simple failed", __FUNCTION__);
            return PROPERTY_VALUE_ERROR;
        }
        (void)strncpy_s(config_unit_p->portconfig.NparPF[pf_index].FCoEFipMACAddress,
            sizeof(config_unit_p->portconfig.NparPF[pf_index].FCoEFipMACAddress), prop_val,
            sizeof(config_unit_p->portconfig.NparPF[pf_index].FCoEFipMACAddress) - 1);
    }

    
    ret = _set_pf_config_band(pf_index, pf_json, config_unit_p, error_info, port_index);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: _set_pf_config_band fail.", __FUNCTION__);
        return ret;
    }
    
    obj_json = NULL;
    prop_val = NULL;
    
    
    ret = json_object_object_get_ex(pf_json, CLP_KEY_WWPN, &obj_json);
    if (!ret) {
        obj_json = NULL;
        ret = json_object_object_get_ex(pf_json, CLP_KEY_FCOE_WWPN, &obj_json);
        (void)memset_s(tmp_str, MAX_LEN_32 + 1, 0, MAX_LEN_32 + 1);
        (void)strncpy_s(tmp_str, sizeof(tmp_str), CLP_KEY_FCOE_WWPN, strlen(CLP_KEY_FCOE_WWPN));
    } else {
        (void)memset_s(tmp_str, MAX_LEN_32 + 1, 0, MAX_LEN_32 + 1);
        (void)strncpy_s(tmp_str, sizeof(tmp_str), CLP_KEY_WWPN, strlen(CLP_KEY_WWPN));
    }

    

    
    (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
    iRet = snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
        MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, pf_index, tmp_str);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
    }
    

    if (ret && obj_json != NULL) {
        prop_val = dal_json_object_get_str(obj_json);
        if (prop_val == NULL) {
            return RET_ERR;
        }

        if (strlen(prop_val) != MAX_PROP_LEN_16 && strlen(prop_val) != MIN_PROP_LEN) {
            debug_log(DLOG_ERROR, "%s: strlen error.", __FUNCTION__);
            
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            return PROPERTY_VALUE_ERROR;
        }

        
        ret = g_regex_match_simple(REGEX_WWPN_WWNN, prop_val, (GRegexCompileFlags)0, (GRegexMatchFlags)0);
        
        
        if (ret != TRUE && strlen(prop_val) != MIN_PROP_LEN) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            debug_log(DLOG_ERROR, "%s: g_regex_match_simple failed", __FUNCTION__);
            return PROPERTY_VALUE_ERROR;
        }
        (void)strncpy_s(config_unit_p->portconfig.NparPF[pf_index].FCoEWWPN,
            sizeof(config_unit_p->portconfig.NparPF[pf_index].FCoEWWPN), prop_val,
            sizeof(config_unit_p->portconfig.NparPF[pf_index].FCoEWWPN) - 1);
    }

    obj_json = NULL;
    prop_val = NULL;
    
    
    ret = json_object_object_get_ex(pf_json, CLP_KEY_WWNN, &obj_json);
    if (!ret) {
        obj_json = NULL;
        ret = json_object_object_get_ex(pf_json, CLP_KEY_FCOE_WWNN, &obj_json);
        (void)memset_s(tmp_str, MAX_LEN_32 + 1, 0, MAX_LEN_32 + 1);
        (void)strncpy_s(tmp_str, sizeof(tmp_str), CLP_KEY_FCOE_WWNN, strlen(CLP_KEY_FCOE_WWNN));
    } else {
        (void)memset_s(tmp_str, MAX_LEN_32 + 1, 0, MAX_LEN_32 + 1);
        (void)strncpy_s(tmp_str, sizeof(tmp_str), CLP_KEY_WWNN, strlen(CLP_KEY_WWNN));
    }

    

    
    (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
    iRet = snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
        MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, pf_index, tmp_str);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
    }
    

    if (ret && obj_json != NULL) {
        prop_val = dal_json_object_get_str(obj_json);
        if (prop_val == NULL) {
            return RET_ERR;
        }

        if (strlen(prop_val) != MAX_PROP_LEN_16 && strlen(prop_val) != MIN_PROP_LEN) {
            debug_log(DLOG_ERROR, "%s: strlen error.", __FUNCTION__);
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            return PROPERTY_VALUE_ERROR;
        }

        
        ret = g_regex_match_simple(REGEX_WWPN_WWNN, prop_val, (GRegexCompileFlags)0, (GRegexMatchFlags)0);
        if (ret != TRUE && strlen(prop_val) != MIN_PROP_LEN) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            debug_log(DLOG_ERROR, "%s: g_regex_match_simple failed", __FUNCTION__);
            return PROPERTY_VALUE_ERROR;
        }
        (void)strncpy_s(config_unit_p->portconfig.NparPF[pf_index].FCoEWWNN,
            sizeof(config_unit_p->portconfig.NparPF[pf_index].FCoEWWNN), prop_val,
            sizeof(config_unit_p->portconfig.NparPF[pf_index].FCoEWWNN) - 1);
    }

    
    ret = _set_pf_config_vlan(pf_index, pf_json, config_unit_p, error_info, port_index);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: _set_pf_config_vlan fail.", __FUNCTION__);
        return ret;
    }
    
    return RET_OK;
}


LOCAL gint32 _set_pf_config_pfid(gint32 pf_index, json_object *pf_json, CLP_RESPONSE_UNIT *config_unit_p,
    json_object **error_info, gint32 port_index)
{
    gint32 ret = 0;
    gint32 pf_id = 0;
    json_object *obj_json = NULL;
    gchar prop_path[MAX_PROPERTY_PATH_LEN + 1] = {0};

    
    if (pf_json == NULL || config_unit_p == NULL || error_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    ret = json_object_object_get_ex(pf_json, RFPROP_CLP_PF_ID, &obj_json);

    
    (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
        MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, pf_index,
        RFPROP_CLP_PF_ID);

    
    if (ret) {
        if (json_type_int != json_object_get_type(obj_json)) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            debug_log(DLOG_ERROR, "%s: json_type error", __FUNCTION__);
            return PROPERTY_VALUE_NOT_INLIST;
        }

        pf_id = json_object_get_int(obj_json);
        if (pf_id < MIN_PF_ID || pf_id > MAX_PF_ID) {
            debug_log(DLOG_ERROR, "%s: pf_id %d invalid.", __FUNCTION__, pf_id);
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            return PROPERTY_VALUE_NOT_INLIST;
        }

        ret = _set_pf_config_pfid_check(pf_index, pf_id, obj_json, config_unit_p, error_info, prop_path);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: _set_pf_config_pfid_check fail.", __FUNCTION__);
            return ret;
        }

        if (config_unit_p->headinfo.portid == 0) {
            if ((pf_id % 2) == 0) {
                config_unit_p->portconfig.NparPF[pf_index].pf_id = pf_id;
            } else {
                debug_log(DLOG_ERROR, "%s: pf_id %d invalid for chip%d port 0.", __FUNCTION__, pf_id,
                    config_unit_p->headinfo.chipid);
                json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
                json_object_object_add(*error_info, ERROR_VAL,
                    json_object_new_string(dal_json_object_get_str(obj_json)));
                return PROPERTY_VALUE_ERROR;
            }
        } else if (config_unit_p->headinfo.portid == 1) {
            if ((pf_id % 2) == 1) {
                config_unit_p->portconfig.NparPF[pf_index].pf_id = pf_id;
            } else {
                debug_log(DLOG_ERROR, "%s: pf_id %d invalid for chip%d port 1.", __FUNCTION__, pf_id,
                    config_unit_p->headinfo.chipid);
                json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
                json_object_object_add(*error_info, ERROR_VAL,
                    json_object_new_string(dal_json_object_get_str(obj_json)));
                return PROPERTY_VALUE_ERROR;
            }
        }
    } else {
        debug_log(DLOG_ERROR, "%s: set pf_id error.", __FUNCTION__);
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_MISSING;
    }

    return RET_OK;
}


LOCAL gint32 _set_pf_config_band(gint32 pf_index, json_object *pf_json, CLP_RESPONSE_UNIT *config_unit_p,
    json_object **error_info, gint32 port_index)
{
    gint32 ret = 0;
    json_object *obj_json = NULL;
    gint32 min_band = 0;
    gint32 max_band = 0;
    gchar prop_path[MAX_PROPERTY_PATH_LEN + 1] = {0};

    
    if (pf_json == NULL || config_unit_p == NULL || error_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    ret = json_object_object_get_ex(pf_json, RFPROP_CLP_MIN_BAND, &obj_json);

    
    (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
    (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
        MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, pf_index,
        RFPROP_CLP_MIN_BAND);
    
    
    if ((!g_ascii_strncasecmp((const gchar *)config_unit_p->chipconfig.MultifunctionMode, CLP_VAL_NPAR,
        strlen(CLP_VAL_NPAR))) &&
        ret == FALSE) {
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        debug_log(DLOG_ERROR, "%s: set MinBandwidth error.", __FUNCTION__);
        return PROPERTY_MISSING;
    }

    
    if (ret) {
        if (json_type_int != json_object_get_type(obj_json) && json_type_null != json_object_get_type(obj_json)) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            debug_log(DLOG_ERROR, "%s: json_type error", __FUNCTION__);
            return PROPERTY_VALUE_NOT_INLIST;
        }
        min_band = json_object_get_int(obj_json);
        
        if (min_band < MIN_BAND || min_band > MAX_BAND ||
            (!g_ascii_strncasecmp((const gchar *)config_unit_p->chipconfig.MultifunctionMode, CLP_VAL_SF,
            strlen(CLP_VAL_SF)) &&
            min_band != 0)) {
            debug_log(DLOG_ERROR, "%s: min_band invalid .", __FUNCTION__);
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            return PROPERTY_VALUE_ERROR;
        }

        config_unit_p->portconfig.NparPF[pf_index].MinBandwidth = min_band;
    }

    config_unit_p->portconfig.Band_sum += min_band;

    if (config_unit_p->portconfig.Band_sum > MAX_BAND) {
        debug_log(DLOG_ERROR, "%s: min_band out of range.", __FUNCTION__);
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
        return PROPERTY_VALUE_ERROR;
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(pf_json, RFPROP_CLP_MAX_BAND, &obj_json);

    
    (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
    (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
        MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, pf_index,
        RFPROP_CLP_MAX_BAND);
    

    
    if ((!g_ascii_strncasecmp((const gchar *)config_unit_p->chipconfig.MultifunctionMode, CLP_VAL_NPAR,
        strlen(CLP_VAL_NPAR))) &&
        ret == FALSE) {
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        debug_log(DLOG_ERROR, "%s: set MaxBandwidth error.", __FUNCTION__);
        return PROPERTY_MISSING;
    }
    

    if (ret) {
        
        if (json_type_int != json_object_get_type(obj_json) && json_type_null != json_object_get_type(obj_json)) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            debug_log(DLOG_ERROR, "%s: json_type error", __FUNCTION__);
            return PROPERTY_VALUE_NOT_INLIST;
        }
        max_band = json_object_get_int(obj_json);
        
        if (MIN_BAND > max_band || MAX_BAND < max_band ||
            (!g_ascii_strncasecmp((const gchar *)config_unit_p->chipconfig.MultifunctionMode, CLP_VAL_SF,
            strlen(CLP_VAL_SF)) &&
            max_band != 0)) {
            debug_log(DLOG_ERROR, "%s: max_band invalid .", __FUNCTION__);
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            return PROPERTY_VALUE_ERROR;
        }

        config_unit_p->portconfig.NparPF[pf_index].MaxBandwidth = max_band;
    }

    if (max_band < min_band) {
        debug_log(DLOG_ERROR, "%s:min_band more than max_band.", __FUNCTION__);
        json_object_object_add(*error_info, ERROR_REL_PROP, json_object_new_string(prop_path));
        
        (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, pf_index,
            RFPROP_CLP_MIN_BAND);
        
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return BANDWITH_ERROR;
    }

    return RET_OK;
}
LOCAL gint32 check_pf_vlan(const gchar *prop_val, gint32 pf_index, CLP_RESPONSE_UNIT *config_unit_p)
{
    gint32 ret;
    if (strlen(prop_val) >= sizeof(config_unit_p->portconfig.NparPF[pf_index].QinqVlanTrunkRange)) {
        debug_log(DLOG_ERROR, "%s: length of QinqVlanTrunkRange %zu exceeds than %zu", __func__,
            strlen(prop_val), sizeof(config_unit_p->portconfig.NparPF[pf_index].QinqVlanTrunkRange) - 1);
        return TRUNK_RANGE_CHECK_FAIL;
    }
    
    ret = g_regex_match_simple(REGEX_TRUNK_RANGE, prop_val, (GRegexCompileFlags)0, (GRegexMatchFlags)0);
    if (ret != TRUE && strlen(prop_val) != 0) {
        debug_log(DLOG_ERROR, "%s: g_regex_match_simple failed", __FUNCTION__);
        return TRUNK_RANGE_CHECK_FAIL;
    }
    ret = _check_pf_trunk_range_sequence(prop_val);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: _check_pf_trunk_range_sequence failed", __FUNCTION__);
        return TRUNK_RANGE_CHECK_FAIL;
    }
    ret = _check_pf_vlan_trunkrange(prop_val, TRUE); // 校验单个pf里trunkrange是否重复
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: _check_pf_vlan_trunkrange failed", __FUNCTION__);
        return ret;
    }
    return RET_OK;
}

LOCAL gint32 _set_pf_config_vlan(gint32 pf_index, json_object *pf_json, CLP_RESPONSE_UNIT *config_unit_p,
    json_object **error_info, gint32 port_index)
{
    gint32 ret = 0;
    json_object *obj_json = NULL;
    const gchar *prop_val = NULL;
    gchar prop_path[MAX_PROPERTY_PATH_LEN + 1] = {0};
    errno_t securec_rv;

    
    if (pf_json == NULL || config_unit_p == NULL || error_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    ret = json_object_object_get_ex(pf_json, RFPROP_CLP_VLAN_MODE, &obj_json);
    if (ret && obj_json != NULL) {
        prop_val = dal_json_object_get_str(obj_json);
        securec_rv = strncpy_s(config_unit_p->portconfig.NparPF[pf_index].QinqVlanMode,
            sizeof(config_unit_p->portconfig.NparPF[pf_index].QinqVlanMode), prop_val,
            sizeof(config_unit_p->portconfig.NparPF[pf_index].QinqVlanMode) - 1);
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, securec_rv);
        }
    }

    
    if (strlen(config_unit_p->portconfig.NparPF[pf_index].QinqVlanMode)) {
        obj_json = NULL;
        ret = json_object_object_get_ex(pf_json, RFPROP_CLP_VLANID, &obj_json);

        
        (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, pf_index,
            RFPROP_CLP_VLANID);
        

        if (ret && obj_json != NULL) {
            ret = _set_pf_config_vlanid(pf_index, obj_json, config_unit_p, error_info, prop_path);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: _set_pf_config_vlanid fail.", __FUNCTION__);
                return ret;
            }
        }

        
        if (g_ascii_strcasecmp(config_unit_p->portconfig.NparPF[pf_index].QinqVlanMode, "QINQ") == 0 &&
            config_unit_p->portconfig.NparPF[pf_index].QinqVlanId == INVALID_INT_VAL) {
            
            if (ret && obj_json == NULL) {
                json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
                json_object_object_add(*error_info, ERROR_VAL, json_object_new_string("null"));
                return PROPERTY_TYPE_ERROR;
            }

            

            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            return PROPERTY_MISSING;
        }

        obj_json = NULL;
        (void)json_object_object_get_ex(pf_json, RFPROP_CLP_VLAN_PRIORITY, &obj_json);

        
        (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, pf_index,
            RFPROP_CLP_VLAN_PRIORITY);
        
        ret = _set_pf_config_vlanpriority(pf_index, obj_json, config_unit_p, error_info, prop_path);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: _set_pf_config_vlanpriority fail.", __FUNCTION__);
            return ret;
        }

        obj_json = NULL;
        prop_val = NULL;
        ret = json_object_object_get_ex(pf_json, RFPROP_CLP_VLAN_TRUNKRANGE, &obj_json);

        
        (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, port_index, RFPROP_CLP_PFS_INFO, pf_index,
            RFPROP_CLP_VLAN_TRUNKRANGE);
        

        if (ret && obj_json != NULL) {
            
            if (g_ascii_strcasecmp(config_unit_p->portconfig.NparPF[pf_index].QinqVlanMode, "FILTERING")) {
                debug_log(DLOG_ERROR, "%s: vlanmode error when setting vlan_trunkrange.", __FUNCTION__);
                json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
                json_object_object_add(*error_info, ERROR_REL_PROP, json_object_new_string(RFPROP_CLP_VLAN_MODE));
                json_object_object_add(*error_info, ERROR_REL_VAL,
                    json_object_new_string(config_unit_p->portconfig.NparPF[pf_index].QinqVlanMode));
                return MODE_UNSUPPORTED;
            }

            prop_val = dal_json_object_get_str(obj_json);
            ret = check_pf_vlan(prop_val, pf_index, config_unit_p);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: check_pf_vlan failed", __func__);
                return ret;
            }

            securec_rv = strncpy_s(config_unit_p->portconfig.NparPF[pf_index].QinqVlanTrunkRange,
                sizeof(config_unit_p->portconfig.NparPF[pf_index].QinqVlanTrunkRange), prop_val,
                sizeof(config_unit_p->portconfig.NparPF[pf_index].QinqVlanTrunkRange) - 1);
            if (securec_rv != EOK) {
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, securec_rv);
            }
        }
        
        else if (g_ascii_strcasecmp(config_unit_p->portconfig.NparPF[pf_index].QinqVlanMode, "FILTERING") == 0) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            return PROPERTY_MISSING;
        }

        
    }

    return RET_OK;
}


LOCAL gint32 _set_pf_config_pfid_check(gint32 pf_index, gint32 pf_id, json_object *obj_json,
    CLP_RESPONSE_UNIT *config_unit_p, json_object **error_info, const gchar *prop_path)
{
    
    if (obj_json == NULL || prop_path == NULL || config_unit_p == NULL || error_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    if (g_ascii_strcasecmp(CLP_VAL_SF, config_unit_p->chipconfig.MultifunctionMode) == 0) {
        if (pf_id != 0 && pf_id != 1) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            debug_log(DLOG_ERROR, "%s:pf_id %d ERROR when MultifunctionMode SF ", __FUNCTION__, pf_id);
            return PROPERTY_VALUE_ERROR;
        }
    } else {
        
        while ((pf_index - 1 >= 0) && (pf_index - 1 < PORT_PF_MAX)) {
            if (pf_id == config_unit_p->portconfig.NparPF[pf_index - 1].pf_id) {
                json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
                json_object_object_add(*error_info, ERROR_VAL,
                    json_object_new_string(dal_json_object_get_str(obj_json)));
                debug_log(DLOG_ERROR, "%s:pf_id %d is the same as that of other parameters.", __FUNCTION__, pf_id);
                return PROPERTY_VALUE_ERROR;
            }
            pf_index--;
        }
    }

    return RET_OK;
}


LOCAL gint32 _set_pf_config_vlanid(gint32 pf_index, json_object *obj_json, CLP_RESPONSE_UNIT *config_unit_p,
    json_object **error_info, gchar *prop_path)
{
    gint32 qinq_vlanid = 0;
    gint32 index_id = 0;

    
    if (obj_json == NULL || config_unit_p == NULL || error_info == NULL || prop_path == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    if (json_type_int != json_object_get_type(obj_json)) {
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
        debug_log(DLOG_ERROR, "%s: json_type error", __FUNCTION__);
        return PROPERTY_VALUE_ERROR;
    }
    qinq_vlanid = json_object_get_int(obj_json);
    if (qinq_vlanid < MIN_VLAN_ID || qinq_vlanid > MAX_VLAN_ID) {
        debug_log(DLOG_ERROR, "%s: vlanid %d  invalid.", __FUNCTION__, qinq_vlanid);
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
        return PROPERTY_VALUE_ERROR;
    }

    
    if (g_ascii_strcasecmp(config_unit_p->portconfig.NparPF[pf_index].QinqVlanMode, "QINQ") == 0) {
        for (index_id = 0; index_id < pf_index; index_id++) {
            if (qinq_vlanid == config_unit_p->portconfig.NparPF[index_id].QinqVlanId &&
                INVALID_INT_VAL != config_unit_p->portconfig.NparPF[index_id].QinqVlanId) {
                debug_log(DLOG_ERROR, "%s: vlanid %d  conflict.", __FUNCTION__, qinq_vlanid);
                return VLANID_CONFLICT;
            }
        }
    }
    
    else if (g_ascii_strcasecmp(config_unit_p->portconfig.NparPF[pf_index].QinqVlanMode, "NORMAL") == 0) {
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        json_object_object_add(*error_info, ERROR_REL_PROP, json_object_new_string(RFPROP_CLP_VLAN_MODE));
        json_object_object_add(*error_info, ERROR_REL_VAL, json_object_new_string("NORMAL"));
        return MODE_UNSUPPORTED;
    }

    

    config_unit_p->portconfig.NparPF[pf_index].QinqVlanId = qinq_vlanid;

    
    if (g_ascii_strcasecmp(config_unit_p->portconfig.NparPF[pf_index].QinqVlanMode, "QINQ") == 0 &&
        (config_unit_p->portconfig.NparPF[pf_index].QinqVlanId == 0)) {
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
        json_object_object_add(*error_info, ERROR_REL_VAL,
            json_object_new_string(config_unit_p->portconfig.NparPF[pf_index].QinqVlanMode));
        return QINQ_VLANID_ERROR;
    }

    return RET_OK;
}


LOCAL gint32 _set_pf_config_vlanpriority(gint32 pf_index, json_object *obj_json, CLP_RESPONSE_UNIT *config_unit_p,
    json_object **error_info, const gchar *prop_path)
{
    gint32 vlan_Priority = 0;

    
    if (config_unit_p == NULL || error_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    if (obj_json != NULL) {
        if (json_type_int != json_object_get_type(obj_json)) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            debug_log(DLOG_ERROR, "%s: json_type error", __FUNCTION__);
            return PROPERTY_VALUE_ERROR;
        }
        vlan_Priority = json_object_get_int(obj_json);
        if (vlan_Priority < MIN_VLAN_PRIORITY || vlan_Priority > MAX_VLAN_PRIORITY) {
            debug_log(DLOG_ERROR, "%s: vlan_Priority %d  invalid .", __FUNCTION__,
                config_unit_p->portconfig.NparPF[pf_index].QinqVlanId);
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_VAL, json_object_new_string(dal_json_object_get_str(obj_json)));
            return PROPERTY_VALUE_ERROR;
        }

        
        if (g_ascii_strcasecmp(config_unit_p->portconfig.NparPF[pf_index].QinqVlanMode, "NORMAL") == 0) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            json_object_object_add(*error_info, ERROR_REL_PROP, json_object_new_string(RFPROP_CLP_VLAN_MODE));
            json_object_object_add(*error_info, ERROR_REL_VAL, json_object_new_string("NORMAL"));
            return MODE_UNSUPPORTED;
        }

        
        config_unit_p->portconfig.NparPF[pf_index].QinqVlanPriority = json_object_get_int(obj_json);
    }
    
    else if (g_ascii_strcasecmp(config_unit_p->portconfig.NparPF[pf_index].QinqVlanMode, "QINQ") == 0) {
        json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
        return PROPERTY_MISSING;
    }

    
    return RET_OK;
}


LOCAL gint32 _check_pf_trunk_range_sequence(const gchar *prop_val)
{
    gint32 ret = RET_OK;
    gchar **range_arry = NULL;
    gchar **sub_range_arry = NULL;
    guint16 range_index = 0;
    guint16 n_index = 0;
    gint32 idx = 0;
    guint16 pf_range[4096] = {0};

    if (prop_val == NULL || strlen(prop_val) == 0) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RET_ERR;
    }

    
    
    if (strlen(prop_val) == 1 && g_ascii_strncasecmp(prop_val, "0", strlen("0")) == 0) {
        return RET_OK;
    }
    

    range_arry = g_strsplit_set(prop_val, ",", 0);
    if (range_arry == NULL) {
        debug_log(DLOG_ERROR, "%s:g_strsplit_set fail", __FUNCTION__);
        return RET_ERR;
    }

    for (range_index = 0; range_arry[range_index] != NULL; range_index++) {
        if (g_strrstr(range_arry[range_index], "-")) {
            sub_range_arry = g_strsplit_set(range_arry[range_index], "-", 0);
            if (sub_range_arry == NULL || g_strv_length(sub_range_arry) != 2) {
                if (sub_range_arry != NULL) {
                    g_strfreev(sub_range_arry);
                }
                g_strfreev(range_arry);
                debug_log(DLOG_ERROR, "%s:g_strsplit_set fail", __FUNCTION__);
                return RET_ERR;
            }
            debug_log(DLOG_ERROR, "%s: range = %s", __FUNCTION__, sub_range_arry[0]);
            ret = vos_str2int(sub_range_arry[0], 10, &pf_range[n_index++], NUM_TPYE_USHORT);
            if (ret != RET_OK) {
                g_strfreev(sub_range_arry);
                g_strfreev(range_arry);
                debug_log(DLOG_ERROR, "%s:vos_str2int , ret = %d", __FUNCTION__, ret);
                return RET_ERR;
            }
            
            if (pf_range[n_index - 1] == 0) {
                g_strfreev(sub_range_arry);
                g_strfreev(range_arry);
                debug_log(DLOG_ERROR, "%s:Trunkrange is 0 , ret = %d", __FUNCTION__, ret);
                return RET_ERR;
            }

            debug_log(DLOG_ERROR, "%s: range = %s", __FUNCTION__, sub_range_arry[1]);
            ret = vos_str2int(sub_range_arry[1], 10, &pf_range[n_index++], NUM_TPYE_USHORT);
            if (ret != RET_OK) {
                g_strfreev(sub_range_arry);
                g_strfreev(range_arry);
                debug_log(DLOG_ERROR, "%s:vos_str2int , ret = %d", __FUNCTION__, ret);
                return RET_ERR;
            }
            
            if (pf_range[n_index - 1] == 0) {
                g_strfreev(sub_range_arry);
                g_strfreev(range_arry);
                debug_log(DLOG_ERROR, "%s:Trunkrange is 0 , ret = %d", __FUNCTION__, ret);
                return RET_ERR;
            }
            if (sub_range_arry != NULL) {
                g_strfreev(sub_range_arry);
                sub_range_arry = NULL;
            }
        } else {
            debug_log(DLOG_ERROR, "%s: range = %s", __FUNCTION__, range_arry[range_index]);
            ret = vos_str2int(range_arry[range_index], 10, &pf_range[n_index++], NUM_TPYE_USHORT);
            if (ret != RET_OK) {
                g_strfreev(range_arry);
                debug_log(DLOG_ERROR, "%s:vos_str2int , ret = %d", __FUNCTION__, ret);
                return RET_ERR;
            }
            
            if (pf_range[n_index - 1] == 0) {
                g_strfreev(range_arry);
                debug_log(DLOG_ERROR, "%s:Trunkrange is 0 , ret = %d", __FUNCTION__, ret);
                return RET_ERR;
            }
        }
    }

    g_strfreev(range_arry);

    
    while (idx < 4095) {
        if (pf_range[idx] > 4094 && pf_range[idx] != 0) {
            debug_log(DLOG_DEBUG, "%s:pf continuous trunk range error", __FUNCTION__);
            return RET_ERR;
        } else if (pf_range[idx + 1] == 0) {
            break;
        }

        if (pf_range[idx] >= pf_range[idx + 1]) {
            debug_log(DLOG_ERROR, "%s: pf continuous trunk range error.", __FUNCTION__);
            return RET_ERR;
        }

        idx++;
    }

    return RET_OK;
}