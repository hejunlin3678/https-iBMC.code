

#include "get_clp.h"

LOCAL gint32 bios_get_lom_card_mac(json_object **out_data, const gchar *object_name);
LOCAL gint32 bios_parse_clp_to_json(guint16 board_id, const gchar *devicename, json_object **out_data);
LOCAL gint32 _read_mac_addr(json_object **out_data, const gchar *object_name);
LOCAL gint32 json_data_initial(guint16 board_id, json_object **out_data);
LOCAL void create_json_data(GSList *config_list, guint16 board_id, const gchar *devicename, json_object *out_data);
LOCAL gint32 _parse_response(GSList **config_list);
LOCAL gint32 create_lom_json_data(json_object *port_data, gchar *mac_address, guchar port_id);
LOCAL gint32 _if_netcard_supported(guint16 board_id);
LOCAL void _count_max_portid_prechip(GSList *config_list, guint16 board_id, const gchar *devicename,
    guint8 *max_port_num);
LOCAL gint32 _parse_response_cmd_show(gchar *data_buf, guint32 data_buf_size, FM_FILE_S *file_handle,
    CLP_RESPONSE_UNIT *response_unit_p, CLP_RESPONSE_UNIT *response_unit_p_1);
LOCAL gint32 _convert_display_mac_format(gchar *mac_addr);
LOCAL gint32 _parse_response_for_221(const gchar *data_buf, FM_FILE_S *file_handle, CLP_RESPONSE_UNIT *response_unit_p);
LOCAL gint32 _parse_response_for_220(const gchar *data_buf, FM_FILE_S *file_handle, CLP_RESPONSE_UNIT *response_unit_p,
    CLP_RESPONSE_UNIT *response_unit_p_1);
LOCAL gint32 _parse_response_show_for_520_or_522(gchar *data_buf, FM_FILE_S *file_handle,
    CLP_RESPONSE_UNIT *response_unit_p);
LOCAL gint32 _parse_response_show_for_310_312_912(gchar *data_buf, FM_FILE_S *file_handle,
    CLP_RESPONSE_UNIT *response_unit_p);
LOCAL gint32 _parse_response_card_did_vid(CLP_RESPONSE_UNIT *response_unit_p, gchar *data, guint16 data_len);
LOCAL gint32 _parse_response_board_id(CLP_RESPONSE_UNIT *response_unit_p, const gchar *data);
LOCAL gint32 _parse_response_head_info(CLP_RESPONSE_UNIT *response_unit_p, const gchar *data);
LOCAL void _fill_2xx_portconfig_bootenable(const gchar *ini_val, guchar *bootenable);
LOCAL gint32 _parse_reponse_unit_show(CLP_RESPONSE_UNIT *response_unit_p, gchar *data_buf, guchar pf_id,
    gchar *show_cmd);
LOCAL gint32 _update_reponse_unit_show_config(CLP_RESPONSE_UNIT *response_unit_p, gchar *prop_key, gchar *prop_val,
    guchar pf_id);
LOCAL gint32 _parse_pf_vlan_info(CLP_RESPONSE_UNIT *response_unit_p, guint8 keyid, gchar *prop_key, gchar *prop_val);
LOCAL gint32 _parse_port_info(CLP_RESPONSE_UNIT *response_unit_p, guchar pf_id, gchar *prop_key, gchar *prop_val);
LOCAL gint32 _parse_pf_info(CLP_RESPONSE_UNIT *response_unit_p, guint8 keyid, gchar *prop_val);
LOCAL gint32 _parse_fcoeboottargets_info(CLP_RESPONSE_UNIT *response_unit_p, gchar *prop_key, gchar *prop_val);
LOCAL gchar *_trunk_range_replace(gchar *prop_val);
LOCAL gint32 _update_fcoeboottarget_info(CLP_RESPONSE_UNIT *response_unit_p, gchar *prop_key, gchar *prop_val,
    guint32 target_id);


gint32 Bios_Get_Clp_Response(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret = 0;
    json_object *out_json_data = NULL;
    guint16 board_id = 0;
    const gchar *devicename = NULL;

    
    if (caller_info == NULL || output_list == NULL || input_list == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        goto err;
    }

    board_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(input_list, 0));
    devicename = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 1), NULL);
    if (devicename == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid devicename.", __FUNCTION__);
        goto err;
    }

    if (!g_ascii_strncasecmp(devicename, "lom", strlen("lom"))) {
        bios_get_lom_card_mac(&out_json_data, g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 2), NULL));
    } else {
        
        ret = bios_parse_clp_to_json(board_id, devicename, &out_json_data);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "bios_parse_clp_to_json failed");
            goto err;
        }
    }

    *output_list = g_slist_append(*output_list, g_variant_new_string(dal_json_object_get_str(out_json_data)));
    json_object_put(out_json_data);
    return RET_OK;

err:
    return RET_ERR;
}

LOCAL gint32 bios_get_lom_card_mac(json_object **out_data, const gchar *object_name)
{
    gint32 ret = 0;
    OBJ_HANDLE obj_handle = 0;
    guchar change_flag = 0;

    if (out_data == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    
    ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_object_handle_nth fail.\n", __FUNCTION__);
        return RET_ERR;
    }
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_BIOS_CLP_CHANGE_FLAG, &change_flag);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_object_handle_nth fail.\n", __FUNCTION__);
        return RET_ERR;
    }

    *out_data = json_object_new_object();
    if (!(*out_data)) {
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail.\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }
    json_object_object_add(*out_data, RFPROP_CLP_EFFECTIVE, json_object_new_boolean(!change_flag));

    json_object_object_add(*out_data, RFPROP_CLP_PORTS_CONFIG, NULL);

    
    ret = _read_mac_addr(out_data, object_name);

    return ret;
}


LOCAL gint32 bios_parse_clp_to_json(guint16 board_id, const gchar *devicename, json_object **out_data)
{
    GSList *config_list = NULL;
    gint32 ret = 0;

    
    if (devicename == NULL || out_data == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    
    ret = json_data_initial(board_id, out_data);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s:json_data_initial fail", __FUNCTION__);
        return RET_ERR;
    }

    
    ret = _parse_response(&config_list);
    
    if (ret != RET_OK) {
        json_object_put(*out_data);
        if (config_list) {
            g_slist_free_full(config_list, (GDestroyNotify)g_free);
        }
        debug_log(DLOG_INFO, "%s:_parse_response fail", __FUNCTION__);
        return RET_ERR;
    }

    
    (void)create_json_data(config_list, board_id, devicename, *out_data);

    
    if (config_list) {
        g_slist_free_full(config_list, (GDestroyNotify)g_free);
    }

    return RET_OK;
}

LOCAL gint32 _read_mac_addr(json_object **out_data, const gchar *object_name)
{
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    gchar           ref_nercard[MAX_LEN_128] = {0};
    gchar           addresses[MAX_LEN_128] = {0};
    guint32 func_type = 0;
    guchar portnum = 0xff;
    guchar port_count = 0;
    guchar index_port = 0;
    OBJ_HANDLE card_handle = 0;
    json_object *ports_obj = NULL;
    json_object *port_obj = NULL;

    if (out_data == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    gint32 ret = dfl_get_object_handle(object_name, &card_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s, %d:  get card handle fail\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    ret = dal_get_property_value_byte(card_handle, PROPERTY_NETCARD_PORTNUM, &port_count);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d:  get port number fail\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s, %d:  get business port fail\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    ports_obj = json_object_new_array();
    json_object_object_add(*out_data, RFPROP_CLP_PORTS_CONFIG, ports_obj);

    for (index_port = 0; index_port <= port_count; index_port++) {
        for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
            ret = dal_get_property_value_string((OBJ_HANDLE)obj_node->data, BUSY_ETH_ATTRIBUTE_REF_NETCARD, ref_nercard,
                MAX_LEN_128);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s, %d:  get RefNetCard fail\n", __FUNCTION__, __LINE__);
                g_slist_free(obj_list);
                return RET_ERR;
            }
            if (g_ascii_strcasecmp(ref_nercard, object_name) != 0) {
                continue;
            }

            ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, BUSY_ETH_ATTRIBUTE_PORT_NUM, &portnum);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s, %d:  get SilkNum fail\n", __FUNCTION__, __LINE__);
                g_slist_free(obj_list);
                return RET_ERR;
            }
            if (index_port != portnum) {
                continue;
            }

            ret = dal_get_property_value_uint32((OBJ_HANDLE)obj_node->data, BUSY_ETH_ATTRIBUTE_NETDEV_FUNCTYPE,
                &func_type);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s, %d:  get NetDevFuncType fail\n", __FUNCTION__, __LINE__);
                g_slist_free(obj_list);
                return RET_ERR;
            }

            if ((func_type & NETDEV_FUNCTYPE_ETHERNET) != 0) {
                ret = dal_get_property_value_string((OBJ_HANDLE)obj_node->data, BUSY_ETH_GROUP_ATTRIBUTE_MAC, addresses,
                    MAX_LEN_128);
                if (ret != RET_OK) {
                    debug_log(DLOG_ERROR, "%s, %d: get address error.", __FUNCTION__, __LINE__);
                    g_slist_free(obj_list);
                    return RET_ERR;
                }
                break;
            }
        }

        port_obj = json_object_new_object();
        if (port_obj == NULL) {
            debug_log(DLOG_ERROR, "%s: new port_obj error.", __FUNCTION__);
            g_slist_free(obj_list);
            return RET_ERR;
        }
        create_lom_json_data(port_obj, addresses, index_port);
        json_object_array_add(ports_obj, port_obj);
    }

    g_slist_free(obj_list);
    return RET_OK;
}


LOCAL gint32 json_data_initial(guint16 board_id, json_object **out_data)
{
    guchar change_flag = 0;
    OBJ_HANDLE obj_handle = 0;
    gint32 result = 0;

    
    if (out_data == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    
    result = _if_netcard_supported(board_id);
    if (result != RET_OK) {
        *out_data = NULL;
        debug_log(DLOG_MASS, "%s:Board id is not match%d.", __FUNCTION__, board_id);
        return RET_ERR;
    }
    

    
    result = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &obj_handle);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_object_handle_nth fail.\n", __FUNCTION__);
        return RET_ERR;
    }
    result = dal_get_property_value_byte(obj_handle, PROPERTY_BIOS_CLP_CHANGE_FLAG, &change_flag);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_object_handle_nth fail.\n", __FUNCTION__);
        return RET_ERR;
    }

    *out_data = json_object_new_object();
    if (!(*out_data)) {
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail.\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }
    json_object_object_add(*out_data, RFPROP_CLP_EFFECTIVE, json_object_new_boolean(!change_flag));

    json_object_object_add(*out_data, RFPROP_CLP_PORTS_CONFIG, NULL);
    return RET_OK;
}


LOCAL void create_json_data(GSList *config_list, guint16 board_id, const gchar *devicename, json_object *out_data)
{
    GSList *obj_node = NULL;
    CLP_RESPONSE_UNIT *node_p = NULL;
    json_object *portsconfig = NULL;
    json_object *port = NULL;
    json_object *port_pf = NULL;
    guint8 id_index = 0;
    json_object *target = NULL;
    json_object *pfs_info = NULL;
    json_object *boot_to_targets = NULL;
    guint8 max_fp_num = 1;
    guint8 rf_port_id = 0;
    guint8 max_port_num = 0;
    
    if (config_list == NULL || devicename == NULL || out_data == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return;
    }

    
    
    (void)_count_max_portid_prechip(config_list, board_id, devicename, &max_port_num);
    

    for (obj_node = config_list; obj_node; obj_node = obj_node->next) {
        node_p = (CLP_RESPONSE_UNIT *)obj_node->data;

        
        if (config_list == obj_node) {
            portsconfig = json_object_new_array();
            if (portsconfig == NULL) {
                debug_log(DLOG_ERROR, "%s, %d: json_object_new_array fail.\n", __FUNCTION__, __LINE__);
                (void)json_object_put(out_data);
                return;
            }

            json_object_object_add(out_data, RFPROP_CLP_PORTS_CONFIG, portsconfig);
        }

        
        if (board_id == node_p->board_id && g_ascii_strcasecmp(node_p->headinfo.devicename, devicename) == 0) {
            
            port = json_object_new_object();
            if (port == NULL) {
                debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail.\n", __FUNCTION__, __LINE__);
                (void)json_object_put(out_data);
                return;
            }

            json_object_array_add(portsconfig, port);

            
            
            rf_port_id = max_port_num * (node_p->headinfo.chipid - 1) + node_p->headinfo.portid + 1;
            
            json_object_object_add(port, RFPROP_CLP_PORT_ID, json_object_new_int(rf_port_id));
            json_object_object_add(port, CLP_KEY_MULTI_MODE,
                strlen(node_p->chipconfig.MultifunctionMode) != 0 ?
                json_object_new_string(node_p->chipconfig.MultifunctionMode) :
                NULL);
            json_object_object_add(port, RFPROP_CLP_SRIOVENABLED,
                INVALID_UCHAR != node_p->portconfig.SRIOVEnabled ?
                json_object_new_boolean(node_p->portconfig.SRIOVEnabled) :
                NULL);

            json_object_object_add(port, CLP_KEY_BOOT_PROTOCOL,
                strlen(node_p->portconfig.BootProtocol) != 0 ? json_object_new_string(node_p->portconfig.BootProtocol) :
                                                               NULL);

            if (node_p->board_id == MHFA_ID || node_p->board_id == MHFB_ID) {
                json_object_object_add(port, RFPROP_CLP_BOOTTOTARGET, NULL);
            } else {
                json_object_object_add(port, RFPROP_CLP_BOOTTOTARGET,
                    INVALID_UCHAR != node_p->portconfig.BootToTarget ?
                    json_object_new_boolean(node_p->portconfig.BootToTarget) :
                    NULL);
            }

            json_object_object_add(port, RFPROP_CLP_VLANID,
                INVALID_INT_VAL != node_p->portconfig.VlanId ? json_object_new_int(node_p->portconfig.VlanId) : NULL);

            
            json_object_object_add(port, RFPROP_CLP_SAN_BOOT_ENABLE,
                INVALID_UCHAR != node_p->portconfig.SANBootEnable ?
                json_object_new_boolean(node_p->portconfig.SANBootEnable) :
                NULL);
            json_object_object_add(port, RFPROP_CLP_LINK_CONFIG,
                strlen(node_p->portconfig.LinkConfig) != 0 ? json_object_new_string(node_p->portconfig.LinkConfig) :
                                                             NULL);
            json_object_object_add(port, RFPROP_CLP_SERDES_CONFIG,
                strlen(node_p->portconfig.SerDesConfig) != 0 ? json_object_new_string(node_p->portconfig.SerDesConfig) :
                                                               NULL);
            
            
            pfs_info = json_object_new_array();
            if (pfs_info == NULL) {
                debug_log(DLOG_ERROR, "%s, %d: json_object_new_array fail.\n", __FUNCTION__, __LINE__);
                (void)json_object_put(out_data);
                return;
            }
            json_object_object_add(port, RFPROP_CLP_PFS_INFO, pfs_info);

            
            if (g_ascii_strcasecmp(node_p->chipconfig.MultifunctionMode, CLP_VAL_NPAR) == 0) {
                max_fp_num = PORT_PF_MAX;
            }
            if (g_ascii_strcasecmp(node_p->chipconfig.MultifunctionMode, CLP_VAL_SF) == 0) {
                max_fp_num = 1;
            }

            for (id_index = 0; id_index < max_fp_num; id_index++) {
                if (INVALID_UCHAR != node_p->portconfig.NparPF[id_index].pf_id) {
                    port_pf = json_object_new_object();
                    if (port_pf == NULL) {
                        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail.\n", __FUNCTION__, __LINE__);
                        (void)json_object_put(out_data);
                        return;
                    }
                    json_object_array_add(pfs_info, port_pf);
                    json_object_object_add(port_pf, RFPROP_CLP_PF_ID,
                        json_object_new_int(node_p->portconfig.NparPF[id_index].pf_id));
                    json_object_object_add(port_pf, RFPROP_CLP_PROTOCOL,
                        strlen(node_p->portconfig.NparPF[id_index].Protocol) != 0 ?
                        json_object_new_string(node_p->portconfig.NparPF[id_index].Protocol) :
                        NULL);
                    json_object_object_add(port_pf, CLP_KEY_ETH_MAC,
                        strlen(node_p->portconfig.NparPF[id_index].PermanentAddress) != 0 ?
                        json_object_new_string(node_p->portconfig.NparPF[id_index].PermanentAddress) :
                        NULL);
                    json_object_object_add(port_pf, CLP_KEY_ISCSI_MAC,
                        strlen(node_p->portconfig.NparPF[id_index].iScsiAddress) != 0 ?
                        json_object_new_string(node_p->portconfig.NparPF[id_index].iScsiAddress) :
                        NULL);
                    json_object_object_add(port_pf, CLP_KEY_FCOE_MAC,
                        strlen(node_p->portconfig.NparPF[id_index].FCoEFipMACAddress) != 0 ?
                        json_object_new_string(node_p->portconfig.NparPF[id_index].FCoEFipMACAddress) :
                        NULL);
                    json_object_object_add(port_pf, RFPROP_CLP_MIN_BAND,
                        INVALID_UCHAR != node_p->portconfig.NparPF[id_index].MinBandwidth ?
                        json_object_new_int(node_p->portconfig.NparPF[id_index].MinBandwidth) :
                        NULL);
                    json_object_object_add(port_pf, RFPROP_CLP_MAX_BAND,
                        INVALID_UCHAR != node_p->portconfig.NparPF[id_index].MaxBandwidth ?
                        json_object_new_int(node_p->portconfig.NparPF[id_index].MaxBandwidth) :
                        NULL);
                    
                    json_object_object_add(port_pf, CLP_KEY_WWPN,
                        strlen(node_p->portconfig.NparPF[id_index].FCoEWWPN) != 0 ?
                        json_object_new_string(node_p->portconfig.NparPF[id_index].FCoEWWPN) :
                        NULL);
                    json_object_object_add(port_pf, CLP_KEY_WWNN,
                        strlen(node_p->portconfig.NparPF[id_index].FCoEWWNN) != 0 ?
                        json_object_new_string(node_p->portconfig.NparPF[id_index].FCoEWWNN) :
                        NULL);
                    json_object_object_add(port_pf, CLP_KEY_FCOE_WWPN,
                        strlen(node_p->portconfig.NparPF[id_index].FCoEWWPN) != 0 ?
                        json_object_new_string(node_p->portconfig.NparPF[id_index].FCoEWWPN) :
                        NULL);
                    json_object_object_add(port_pf, CLP_KEY_FCOE_WWNN,
                        strlen(node_p->portconfig.NparPF[id_index].FCoEWWNN) != 0 ?
                        json_object_new_string(node_p->portconfig.NparPF[id_index].FCoEWWNN) :
                        NULL);
                    
                    json_object_object_add(port_pf, RFPROP_CLP_VLAN_MODE,
                        strlen(node_p->portconfig.NparPF[id_index].QinqVlanMode) != 0 ?
                        json_object_new_string(node_p->portconfig.NparPF[id_index].QinqVlanMode) :
                        NULL);
                    json_object_object_add(port_pf, RFPROP_CLP_VLANID,
                        INVALID_INT_VAL != node_p->portconfig.NparPF[id_index].QinqVlanId ?
                        json_object_new_int(node_p->portconfig.NparPF[id_index].QinqVlanId) :
                        NULL);
                    json_object_object_add(port_pf, RFPROP_CLP_VLAN_PRIORITY,
                        INVALID_UCHAR != node_p->portconfig.NparPF[id_index].QinqVlanPriority ?
                        json_object_new_int(node_p->portconfig.NparPF[id_index].QinqVlanPriority) :
                        NULL);
                    json_object_object_add(port_pf, RFPROP_CLP_VLAN_TRUNKRANGE,
                        strlen(node_p->portconfig.NparPF[id_index].QinqVlanTrunkRange) != 0 ?
                        json_object_new_string(node_p->portconfig.NparPF[id_index].QinqVlanTrunkRange) :
                        NULL);
                }
            }

            
            boot_to_targets = json_object_new_array();
            if (boot_to_targets == NULL) {
                debug_log(DLOG_ERROR, "%s, %d: json_object_new_array fail.\n", __FUNCTION__, __LINE__);
                (void)json_object_put(out_data);
                return;
            }
            
            json_object_object_add(port, RFPROP_CLP_BOOT_TO_TARGETS, boot_to_targets);
            
            json_object_object_add(port, RFPROP_CLP_BOOT_TARGETS, json_object_get(boot_to_targets));
            
            

            if (TRUE == node_p->portconfig.BootToTarget) {
                for (id_index = 0; id_index < TARGET_MAX_NUM; id_index++) {
                    target = json_object_new_object();
                    if (target == NULL) {
                        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail.\n", __FUNCTION__, __LINE__);
                        (void)json_object_put(out_data);
                        return;
                    }
                    json_object_array_add(boot_to_targets, target);
                    json_object_object_add(target, RFPROP_CLP_TARGET_ID,
                        json_object_new_int(node_p->portconfig.FCoEBootTarget[id_index].target_id));
                    json_object_object_add(target, RFPROP_CLP_TARGET_ENABLED,
                        INVALID_UCHAR != node_p->portconfig.FCoEBootTarget[id_index].TargetEnable ?
                        json_object_new_boolean(node_p->portconfig.FCoEBootTarget[id_index].TargetEnable) :
                        NULL);
                    
                    json_object_object_add(target, CLP_KEY_WWPN,
                        strlen(node_p->portconfig.FCoEBootTarget[id_index].FCoEWWPN) != 0 ?
                        json_object_new_string(node_p->portconfig.FCoEBootTarget[id_index].FCoEWWPN) :
                        NULL);
                    json_object_object_add(target, CLP_KEY_FCOE_WWPN,
                        strlen(node_p->portconfig.FCoEBootTarget[id_index].FCoEWWPN) != 0 ?
                        json_object_new_string(node_p->portconfig.FCoEBootTarget[id_index].FCoEWWPN) :
                        NULL);
                    
                    json_object_object_add(target, RFPROP_CLP_TARGET_LUN,
                        INVALID_INT_VAL != node_p->portconfig.FCoEBootTarget[id_index].bootlun ?
                        json_object_new_int(node_p->portconfig.FCoEBootTarget[id_index].bootlun) :
                        NULL);
                }
            }
        }
    }
}


LOCAL gint32 _parse_response(GSList **config_list)
{
    gchar data_buf[LINE_MAX_LEN + 1] = {0};
    FM_FILE_S *file_handle = NULL;
    gint32 fm_fseek_start = 0; // 返回成功为0，不成功非0
    gint32 file_start = 0;
    gint32 ret = 0;
    CLP_RESPONSE_UNIT *response_unit_p = NULL;
    gchar *fm_ret = NULL;
    gint32 id_index = 0;
    gchar card_buf[LINE_MAX_LEN + 1] = {0};
    gchar head_buf[LINE_MAX_LEN + 1] = {0};
    CLP_RESPONSE_UNIT *response_unit_p_1 = NULL;

    if (config_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input parameter is NULL", __FUNCTION__);
        return RET_ERR;
    }

    
    file_handle = fm_fopen(PROPERTY_BIOS_CLP_RESP_NAME, "rb");
    if (file_handle == NULL) {
        debug_log(DLOG_ERROR, "%s: open file %s failed", __FUNCTION__, PROPERTY_BIOS_CLP_RESP_NAME);
        goto err_out;
    }

    
    fm_fseek_start = fm_fseek(file_handle, 0, SEEK_SET);
    if (fm_fseek_start != 0) {
        debug_log(DLOG_ERROR, "%s: file seek %s failed", __FUNCTION__, PROPERTY_BIOS_CLP_RESP_NAME);
        goto err_out;
    }

    file_start = fm_ftell(file_handle);
    if (file_start < 0) {
        debug_log(DLOG_ERROR, "%s: file tell %s failed", __FUNCTION__, PROPERTY_BIOS_CLP_RESP_NAME);
        goto err_out;
    }

    
    
    while (fm_fgets(data_buf, LINE_MAX_LEN, file_handle) != NULL) {
        // port 分配内存空间
        if (g_ascii_strncasecmp(data_buf, "{", strlen("{")) == 0) {
            if (response_unit_p != NULL) {
                g_free(response_unit_p);
                response_unit_p = NULL;
            }

            response_unit_p = (CLP_RESPONSE_UNIT *)g_malloc0(sizeof(CLP_RESPONSE_UNIT));
            if (response_unit_p == NULL) {
                debug_log(DLOG_ERROR, "%s: g_malloc0 failed", __FUNCTION__);
                goto err_out;
            }

            
            for (id_index = 0; id_index < PORT_PF_MAX; id_index++) {
                response_unit_p->portconfig.NparPF[id_index].pf_id = INVALID_UCHAR;
                response_unit_p->portconfig.NparPF[id_index].MinBandwidth = INVALID_UCHAR;
                response_unit_p->portconfig.NparPF[id_index].MaxBandwidth = INVALID_UCHAR;
                response_unit_p->portconfig.NparPF[id_index].QinqVlanId = INVALID_INT_VAL;
                response_unit_p->portconfig.NparPF[id_index].QinqVlanPriority = INVALID_UCHAR;
            }

            
            
            for (id_index = 0; id_index < TARGET_MAX_NUM; id_index++) {
                response_unit_p->portconfig.FCoEBootTarget[id_index].bootlun = INVALID_INT_VAL;
                response_unit_p->portconfig.FCoEBootTarget[id_index].TargetEnable = INVALID_UCHAR;
            }

            response_unit_p->portconfig.VlanId = INVALID_INT_VAL;
            response_unit_p->portconfig.SANBootEnable = INVALID_UCHAR;
            response_unit_p->portconfig.SRIOVEnabled = INVALID_UCHAR;
            
        }

        // 解析mezz head 信息
        else if (g_ascii_strncasecmp(data_buf, CLP_HEAD_MEZZ, strlen(CLP_HEAD_MEZZ)) == 0) {
            (void)memset_s(head_buf, sizeof(head_buf), 0, sizeof(head_buf));
            fm_ret = fm_fgets(head_buf, LINE_MAX_LEN, file_handle);
            
            if (fm_ret == NULL) {
                (void)memset_s(data_buf, sizeof(data_buf), 0, sizeof(data_buf));
                debug_log(DLOG_MASS, "%s: fm_fgets fail.", __FUNCTION__);
                continue;
            }
            if (response_unit_p == NULL) {
                (void)memset_s(data_buf, sizeof(data_buf), 0, sizeof(data_buf));
                debug_log(DLOG_MASS, "%s: response_unit_p is NULL.", __FUNCTION__);
                continue;
            }
            ret = _parse_response_head_info(response_unit_p, head_buf);
            if (ret != RET_OK) {
                (void)memset_s(data_buf, sizeof(data_buf), 0, sizeof(data_buf));
                debug_log(DLOG_MASS, "%s: _parse_response_head_info fail.", __FUNCTION__);
                continue;
            }
        }
        // 解析boardid信息
        else if (g_ascii_strncasecmp(data_buf, CLP_HEAD_BOARD_ID, strlen(CLP_HEAD_BOARD_ID)) == 0) {
            (void)memset_s(card_buf, sizeof(card_buf), 0, sizeof(card_buf));
            fm_ret = fm_fgets(card_buf, LINE_MAX_LEN, file_handle);
            if (fm_ret == NULL) {
                (void)memset_s(data_buf, sizeof(data_buf), 0, sizeof(data_buf));
                debug_log(DLOG_MASS, "%s: fm_fgets fail.", __FUNCTION__);
                continue;
            }
            if (response_unit_p == NULL) {
                (void)memset_s(data_buf, sizeof(data_buf), 0, sizeof(data_buf));
                debug_log(DLOG_MASS, "%s: response_unit_p is NULL.", __FUNCTION__);
                continue;
            }
            ret = _parse_response_board_id(response_unit_p, card_buf);
            if (ret != RET_OK) {
                (void)memset_s(data_buf, sizeof(data_buf), 0, sizeof(data_buf));
                debug_log(DLOG_MASS, "%s: _parse_response_board_id fail.", __FUNCTION__);
                continue;
            }
            
            if (response_unit_p->board_id == MHFA_ID || response_unit_p->board_id == MHFB_ID) {
                response_unit_p->portconfig.BootToTarget = TRUE;
            }
            
        }
        // 解析mezz  信息
        else if (g_ascii_strncasecmp(data_buf, CLP_HEAD_NETCARD, strlen(CLP_HEAD_NETCARD)) == 0) {
            (void)memset_s(card_buf, sizeof(card_buf), 0, sizeof(card_buf));
            fm_ret = fm_fgets(card_buf, LINE_MAX_LEN, file_handle);
            if (fm_ret == NULL) {
                (void)memset_s(data_buf, sizeof(data_buf), 0, sizeof(data_buf));
                debug_log(DLOG_MASS, "%s: fm_fgets fail.", __FUNCTION__);
                continue;
            }
            if (response_unit_p == NULL) {
                (void)memset_s(data_buf, sizeof(data_buf), 0, sizeof(data_buf));
                debug_log(DLOG_MASS, "%s: response_unit_p is NULL.", __FUNCTION__);
                continue;
            }
            ret = _parse_response_card_did_vid(response_unit_p, card_buf, sizeof(card_buf));
            if (ret != RET_OK) {
                (void)memset_s(data_buf, sizeof(data_buf), 0, sizeof(data_buf));
                debug_log(DLOG_MASS, "%s: _parse_response_card_did_vid fail.", __FUNCTION__);
                continue;
            }
        }
        // 解析配置 信息
        else if (g_ascii_strncasecmp(data_buf, CLP_CMD_SHOW, strlen(CLP_CMD_SHOW)) == 0) {
            
            
            if (response_unit_p == NULL) {
                continue;
            }
            
            
            if (response_unit_p->board_id == MHFA_ID && response_unit_p_1 == NULL) {
                response_unit_p_1 = (CLP_RESPONSE_UNIT *)g_malloc0(sizeof(CLP_RESPONSE_UNIT));
                if (response_unit_p_1 == NULL) {
                    debug_log(DLOG_ERROR, "%s: g_malloc0 failed", __FUNCTION__);
                    goto err_out;
                }
                (void)memcpy_s(response_unit_p_1, sizeof(CLP_RESPONSE_UNIT), response_unit_p,
                    sizeof(CLP_RESPONSE_UNIT));
                response_unit_p_1->headinfo.portid = 1;
            }
            ret = _parse_response_cmd_show(data_buf, sizeof(data_buf), file_handle, response_unit_p, response_unit_p_1);
            
            if (ret == RET_ERR) {
                continue;
            }
        } else if (g_ascii_strncasecmp(data_buf, CLP_CMD_SET, strlen(CLP_CMD_SET)) == 0) {
            if (response_unit_p == NULL) {
                continue;
            }
            // 只有MEZZ220和MEZZ221解析set
            if (response_unit_p->board_id != MHFA_ID && response_unit_p->board_id != MHFB_ID) {
                (void)memset_s(data_buf, sizeof(data_buf), 0, sizeof(data_buf));
                continue;
            }

            if (response_unit_p->board_id == MHFB_ID) {
                ret = _parse_response_for_221(data_buf, file_handle, response_unit_p);
                if (ret != RET_OK) {
                    (void)memset_s(data_buf, sizeof(data_buf), 0, sizeof(data_buf));
                    debug_log(DLOG_MASS, "%s: _parse_response_for_221 failed.", __FUNCTION__);
                    continue;
                }
            }

            if (response_unit_p->board_id == MHFA_ID) {
                if (response_unit_p_1 == NULL) {
                    response_unit_p_1 = (CLP_RESPONSE_UNIT *)g_malloc0(sizeof(CLP_RESPONSE_UNIT));
                    if (response_unit_p_1 == NULL) {
                        debug_log(DLOG_ERROR, "%s: g_malloc0 failed", __FUNCTION__);
                        goto err_out;
                    }
                    (void)memcpy_s(response_unit_p_1, sizeof(CLP_RESPONSE_UNIT), response_unit_p,
                        sizeof(CLP_RESPONSE_UNIT));
                    response_unit_p_1->headinfo.portid = 1;
                }

                ret = _parse_response_for_220(data_buf, file_handle, response_unit_p, response_unit_p_1);
                if (ret != RET_OK) {
                    (void)memset_s(data_buf, sizeof(data_buf), 0, sizeof(data_buf));
                    debug_log(DLOG_MASS, "%s: _parse_response_for_220 failed.", __FUNCTION__);
                    continue;
                }
            }
        } else if (g_ascii_strncasecmp(data_buf, CLP_HEAD_FC_FCOE, strlen(CLP_HEAD_FC_FCOE)) == 0) {
            // 此处预留给linkconfig
        }

        

        // 解析结果添加到链表
        if (g_ascii_strncasecmp(data_buf, "}", strlen("}")) == 0) {
            *config_list = g_slist_append(*config_list, response_unit_p);
            response_unit_p = NULL;

            if (response_unit_p_1 != NULL && response_unit_p_1->board_id == MHFA_ID) {
                *config_list = g_slist_append(*config_list, response_unit_p_1);
                response_unit_p_1 = NULL;
            }
        }

        (void)memset_s(data_buf, sizeof(data_buf), 0, sizeof(data_buf));
    }

    
    if (response_unit_p != NULL) {
        g_free(response_unit_p);
    }
    if (response_unit_p_1 != NULL) {
        g_free(response_unit_p_1);
    }
    fm_fclose(file_handle);
    return RET_OK;

err_out:

    if (response_unit_p != NULL) {
        g_free(response_unit_p);
    }
    if (response_unit_p_1 != NULL) {
        g_free(response_unit_p_1);
    }
    
    if (file_handle != NULL) {
        fm_fclose(file_handle);
    }

    return RET_ERR;
}

LOCAL gint32 create_lom_json_data(json_object *port_data, gchar *mac_address, guchar port_id)
{
    json_object *pfs_info = NULL;
    json_object *port_pf = NULL;
    gint32 pf_id = 0;

    if (port_data == NULL || mac_address == NULL || port_id == 0) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    json_object_object_add(port_data, RFPROP_CLP_PORT_ID, json_object_new_int((gint32)port_id));

    pfs_info = json_object_new_array();
    if (pfs_info == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_array fail.\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }
    json_object_object_add(port_data, RFPROP_CLP_PFS_INFO, pfs_info);

    port_pf = json_object_new_object();
    if (port_pf == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_array fail.\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    json_object_array_add(pfs_info, port_pf);

    if (port_id % 2) {
        pf_id = 0;
    } else {
        pf_id = 1;
    }

    json_object_object_add(port_pf, RFPROP_CLP_PF_ID, json_object_new_int(pf_id));

    if (g_strcmp0(mac_address, INVALID_DATA_STRING) && g_strcmp0(mac_address, MAC_ADDR_DEFAULT)) {
        (void)_convert_display_mac_format(mac_address);
        json_object_object_add(port_pf, CLP_KEY_ETH_MAC, json_object_new_string(mac_address));
    } else {
        json_object_object_add(port_pf, CLP_KEY_ETH_MAC, NULL);
    }

    return RET_OK;
}


LOCAL gint32 _if_netcard_supported(guint16 board_id)
{
    guint16 supported_board_id[] = {MXEP_ID, MXEN_ID, MHFA_ID, MHFB_ID, MXEM_ID, MXEK_ID, MXEL_ID};
    gint32 supported_count = sizeof(supported_board_id) / sizeof(guint16);
    gint32 i = 0;

    for (i = 0; i < supported_count; i++) {
        if (board_id == supported_board_id[i]) {
            return RET_OK;
        }
    }

    return RET_ERR;
}


LOCAL void _count_max_portid_prechip(GSList *config_list, guint16 board_id, const gchar *devicename,
    guint8 *max_port_num)
{
    GSList *obj_node = NULL;
    CLP_RESPONSE_UNIT *node_p = NULL;

    for (obj_node = config_list; obj_node; obj_node = obj_node->next) {
        node_p = (CLP_RESPONSE_UNIT *)obj_node->data;

        
        if (board_id == node_p->board_id && g_ascii_strcasecmp(node_p->headinfo.devicename, devicename) == 0) {
            

            if (node_p->headinfo.chipid == 1) {
                (*max_port_num)++;
            }
        }
    }
}


LOCAL gint32 _parse_response_cmd_show(gchar *data_buf, guint32 data_buf_size, FM_FILE_S *file_handle,
    CLP_RESPONSE_UNIT *response_unit_p, CLP_RESPONSE_UNIT *response_unit_p_1)
{
    gint32 ret = 0;

    if (response_unit_p == NULL || data_buf == NULL || file_handle == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL point.", __FUNCTION__);
        return RET_ERR;
    }

    switch (response_unit_p->board_id) {
        case MXEP_ID:
        case MXEN_ID: {
            ret = _parse_response_show_for_520_or_522(data_buf, file_handle, response_unit_p);
            if (ret != RET_OK) {
                (void)memset_s(data_buf, data_buf_size, 0, data_buf_size);
                debug_log(DLOG_MASS, "%s: _parse_response_for_520_or_522 failed.", __FUNCTION__);
                return RET_ERR;
            }
            break;
        }

        case MHFA_ID: {
            ret = _parse_response_for_220(data_buf, file_handle, response_unit_p, response_unit_p_1);
            if (ret != RET_OK) {
                (void)memset_s(data_buf, data_buf_size, 0, data_buf_size);
                debug_log(DLOG_MASS, "%s: _parse_response_for_220 failed.", __FUNCTION__);
                return RET_ERR;
            }

            break;
        }

        case MHFB_ID: {
            ret = _parse_response_for_221(data_buf, file_handle, response_unit_p);
            if (ret != RET_OK) {
                (void)memset_s(data_buf, data_buf_size, 0, data_buf_size);
                debug_log(DLOG_MASS, "%s: _parse_response_for_221 failed.", __FUNCTION__);
                return RET_ERR;
            }
            break;
        }

        case MXEK_ID:
        case MXEM_ID:
        case MXEL_ID: {
            ret = _parse_response_show_for_310_312_912(data_buf, file_handle, response_unit_p);
            if (ret != RET_OK) {
                (void)memset_s(data_buf, data_buf_size, 0, data_buf_size);
                debug_log(DLOG_MASS, "%s: _parse_response_for_310_312 failed.", __FUNCTION__);
                return RET_ERR;
            }
            break;
        }

        default:
            (void)memset_s(data_buf, data_buf_size, 0, data_buf_size);
            return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 _convert_display_mac_format(gchar *mac_addr)
{
    errno_t safe_fun_ret = EOK;
    gchar   tmp_str[MAX_LEN_128] = {0};
    gint32 i = 0;
    gint32 j = 0;

    if (mac_addr == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    for (i = 0; *(mac_addr + i); i++) {
        if (*(mac_addr + i) != ':') {
            tmp_str[j++] = *(mac_addr + i);
        }
    }

    safe_fun_ret = strncpy_s(mac_addr, MAX_LEN_128, tmp_str, strlen(tmp_str));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 _parse_response_for_221(const gchar *data_buf, FM_FILE_S *file_handle, CLP_RESPONSE_UNIT *response_unit_p)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret = 0;
    gchar       data_value_buf[LINE_MAX_LEN + 1] = {0};
    gchar *prop_name = NULL;
    gchar *prop_val = NULL;
    gchar *tmp_str = NULL;
    guint8 wwpn_index = G_MAXUINT8;
    gint16 boot_lun = G_MAXINT16;
    gchar       prop_val_arr[MAX_LEN_32] = {0};
    gchar       data_tmp[LINE_MAX_LEN + 1] = {0};
    gint32 i = 0;
    gchar       index_val[LINE_MAX_LEN] = {0};

    if (response_unit_p == NULL || data_buf == NULL || file_handle == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL point.", __FUNCTION__);
        return RET_ERR;
    }

    

    safe_fun_ret = strncpy_s(data_tmp, LINE_MAX_LEN + 1, data_buf, LINE_MAX_LEN);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    dal_trim_string(data_tmp, LINE_MAX_LEN + 1);

    prop_name = g_strrstr(data_tmp, " ");
    if (prop_name == NULL) {
        debug_log(DLOG_INFO, "%s: show(%s) format error", __FUNCTION__, data_tmp);
        return RET_ERR;
    }

    
    for (i = 0; i < 3; i++) {
        (void)memset_s(data_value_buf, LINE_MAX_LEN, 0, LINE_MAX_LEN);
        (void)fm_fgets(data_value_buf, LINE_MAX_LEN, file_handle);
    }

    
    ret = g_str_has_prefix(data_value_buf, CLP_PROP_DATA);
    if (ret != TRUE) {
        debug_log(DLOG_INFO, "%s: g_str_has_prefix(%s) failed", __FUNCTION__, data_value_buf);
        return RET_ERR;
    }

    prop_val = g_strrstr(data_value_buf, "=");
    if (prop_val == NULL) {
        debug_log(DLOG_INFO, "%s: data(%s) format error", __FUNCTION__, data_value_buf);
        return RET_ERR;
    }

    
    safe_fun_ret = strncpy_s(prop_val_arr, sizeof(prop_val_arr), prop_val + 1, sizeof(prop_val_arr) - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    dal_trim_string(prop_val_arr, MAX_LEN_32);

    if (g_ascii_strncasecmp(prop_name + 1, CLP_KEY_OEM_BOOT_ENABLED, strlen(CLP_KEY_OEM_BOOT_ENABLED)) == 0) {
        _fill_2xx_portconfig_bootenable(prop_val_arr, &response_unit_p->portconfig.SANBootEnable);
    } else if (g_ascii_strncasecmp(prop_name + 1, CLP_KEY_ETH_MAC, strlen(CLP_KEY_ETH_MAC)) == 0) {
        (void)strncpy_s(response_unit_p->portconfig.NparPF[0].FCoEWWPN, MAX_LEN_32 + 1, prop_val_arr, MAX_LEN_32);
    } else if (g_ascii_strncasecmp(prop_name + 1, CLP_KEY_NETWORK_ADDR, strlen(CLP_KEY_NETWORK_ADDR)) == 0) {
        // 忽略NetworkAddresses后面的[0]
        (void)strncpy_s(response_unit_p->portconfig.NparPF[0].FCoEWWNN, MAX_LEN_32 + 1, prop_val_arr, MAX_LEN_32);
    } else if (g_ascii_strncasecmp(prop_name + 1, CLP_KEY_OEM_WWPN, strlen(CLP_KEY_OEM_WWPN)) == 0) {
        // OEM_TargetWWPN[0]  prop_name指向O前面的空格，偏移字符串长度+2就指向了数字
        safe_fun_ret = strncpy_s(index_val, LINE_MAX_LEN, prop_name + strlen(CLP_KEY_OEM_WWPN) + 2, 1);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
        ret = vos_str2int(index_val, 10, &wwpn_index, NUM_TPYE_UCHAR);
        if (ret != RET_OK || (wwpn_index >= TARGET_MAX_NUM)) {
            debug_log(DLOG_INFO, "%s: data(%s) format error or wwpn_index(=%d) is out of buff", __FUNCTION__,
                prop_name + 1, wwpn_index);
            return RET_ERR;
        }

        tmp_str = g_ascii_strup(prop_val_arr, strlen(prop_val_arr));
        if (tmp_str == NULL) {
            debug_log(DLOG_INFO, "%s: g_ascii_strup failed.", __FUNCTION__);
            return RET_ERR;
        }
        (void)strncpy_s(response_unit_p->portconfig.FCoEBootTarget[wwpn_index].FCoEWWPN, MAX_LEN_32 + 1, tmp_str,
            MAX_LEN_32);
        g_free(tmp_str);
    } else if (g_ascii_strncasecmp(prop_name + 1, CLP_KEY_OEM_LUN, strlen(CLP_KEY_OEM_LUN)) == 0) {
        // OEM_TargetLUN[0]  prop_name指向O前面的空格，偏移字符串长度+2就指向了数字
        safe_fun_ret = strncpy_s(index_val, LINE_MAX_LEN, prop_name + strlen(CLP_KEY_OEM_LUN) + 2, 1);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
        ret = vos_str2int(index_val, 10, &wwpn_index, NUM_TPYE_UCHAR);
        if (ret != RET_OK  || (wwpn_index >= TARGET_MAX_NUM)) {
            debug_log(DLOG_INFO, "%s: data(%s) format error or wwpn_index(=%d) is out of buff", __FUNCTION__,
                prop_name + 1, wwpn_index);
            return RET_ERR;
        }

        ret = vos_str2int(prop_val_arr, 16, &boot_lun, NUM_TPYE_SHORT);
        if (ret != RET_OK) {
            debug_log(DLOG_INFO, "%s: value(%s) format error", __FUNCTION__, prop_val_arr);
            return RET_ERR;
        }

        response_unit_p->portconfig.FCoEBootTarget[wwpn_index].bootlun = boot_lun;
    }

    for (i = 0; i < 8; i++) {
        response_unit_p->portconfig.FCoEBootTarget[i].target_id = i + 1;
    }

    if (response_unit_p->headinfo.portid % 2) {
        response_unit_p->portconfig.NparPF[0].pf_id = 1;
    } else {
        response_unit_p->portconfig.NparPF[0].pf_id = 0;
    }

    return RET_OK;
}


LOCAL gint32 _parse_response_for_220(const gchar *data_buf, FM_FILE_S *file_handle, CLP_RESPONSE_UNIT *response_unit_p,
    CLP_RESPONSE_UNIT *response_unit_p_1)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret = 0;
    gchar       data_value_buf[LINE_MAX_LEN + 1] = {0};
    gchar *prop_name = NULL;
    gchar *prop_val = NULL;
    gchar *tmp_str = NULL;
    guint8 wwpn_index = G_MAXUINT8;
    gint16 boot_lun = G_MAXINT16;
    gchar       prop_val_arr[MAX_LEN_32] = {0};
    gchar       data_tmp[LINE_MAX_LEN + 1] = {0};
    gint32 i = 0;
    gchar       index_val[LINE_MAX_LEN] = {0};
    gchar **show_array = NULL;
    guint8 port_id = INVALID_UCHAR;

    if (response_unit_p == NULL || response_unit_p_1 == NULL || data_buf == NULL || file_handle == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL point.", __FUNCTION__);
        return RET_ERR;
    }

    

    safe_fun_ret = strncpy_s(data_tmp, LINE_MAX_LEN + 1, data_buf, LINE_MAX_LEN);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    dal_trim_string(data_tmp, LINE_MAX_LEN + 1);

    prop_name = g_strrstr(data_tmp, " ");
    if (prop_name == NULL) {
        debug_log(DLOG_INFO, "%s: show(%s) format error", __FUNCTION__, data_tmp);
        return RET_ERR;
    }

    // 从show fcport0 OEMELX_BootEnable 中取portid
    show_array = g_strsplit_set(data_tmp, " ", 0);
    if (show_array == NULL || g_strv_length(show_array) < 3) {
        if (show_array != NULL) {
            g_strfreev(show_array);
        }
        debug_log(DLOG_MASS, "%s:g_strsplit_set fail", __FUNCTION__);
        return RET_ERR;
    }

    
    if (g_ascii_strncasecmp(show_array[1], "fcport", strlen("fcport")) == 0) {
        ret = vos_str2int(&show_array[1][6], 10, &port_id, NUM_TPYE_UCHAR);
        if (ret != RET_OK) {
            g_strfreev(show_array);
            debug_log(DLOG_MASS, "%s:vos_str2int fail", __FUNCTION__);
            return RET_ERR;
        }
    }

    
    if (show_array != NULL) {
        g_strfreev(show_array);
    }

    
    for (i = 0; i < 3; i++) {
        (void)memset_s(data_value_buf, LINE_MAX_LEN, 0, LINE_MAX_LEN);
        (void)fm_fgets(data_value_buf, LINE_MAX_LEN, file_handle);
    }

    if (port_id == 0) {
        
        ret = g_str_has_prefix(data_value_buf, CLP_PROP_RETDATA);
        if (ret != TRUE) {
            debug_log(DLOG_INFO, "%s: g_str_has_prefix(%s) failed", __FUNCTION__, data_value_buf);
            return RET_ERR;
        }

        prop_val = g_strrstr(data_value_buf, "=");
        if (prop_val == NULL) {
            debug_log(DLOG_INFO, "%s: data(%s) format error", __FUNCTION__, data_value_buf);
            return RET_ERR;
        }

        
        safe_fun_ret = strncpy_s(prop_val_arr, sizeof(prop_val_arr), prop_val + 1, sizeof(prop_val_arr) - 1);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        dal_trim_string(prop_val_arr, MAX_LEN_32);

        if (g_ascii_strncasecmp(prop_name + 1, CLP_KEY_OEMELX_SERDES, strlen(CLP_KEY_OEMELX_SERDES)) == 0) {
            (void)strncpy_s(response_unit_p->portconfig.SerDesConfig, MAX_LEN_32 + 1, prop_val_arr, MAX_LEN_32);
        } else if (g_ascii_strncasecmp(prop_name + 1, CLP_KEY_OEMELX_BOOT_ENABLED,
            strlen(CLP_KEY_OEMELX_BOOT_ENABLED)) == 0) {
            _fill_2xx_portconfig_bootenable(prop_val_arr, &response_unit_p->portconfig.SANBootEnable);
        } else if (g_ascii_strncasecmp(prop_name + 1, CLP_KEY_OEMELX_WWPN, strlen(CLP_KEY_OEMELX_WWPN)) == 0) {
            (void)strncpy_s(response_unit_p->portconfig.NparPF[0].FCoEWWPN, MAX_LEN_32 + 1, prop_val_arr, MAX_LEN_32);
        } else if (g_ascii_strncasecmp(prop_name + 1, CLP_KEY_OEMELX_WWNN, strlen(CLP_KEY_OEMELX_WWNN)) == 0) {
            (void)strncpy_s(response_unit_p->portconfig.NparPF[0].FCoEWWNN, MAX_LEN_32 + 1, prop_val_arr, MAX_LEN_32);
        } else if (g_ascii_strncasecmp(prop_name + 1, CLP_KEY_OEMELX_TARGET_WWPN, strlen(CLP_KEY_OEMELX_TARGET_WWPN)) ==
            0) {
            // OEMELX_TargetWWPN[0]  prop_name指向O前面的空格，偏移字符串长度+2就指向了数字
            safe_fun_ret = strncpy_s(index_val, LINE_MAX_LEN, prop_name + strlen(CLP_KEY_OEMELX_TARGET_WWPN) + 2, 1);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                return RET_ERR;
            }
            ret = vos_str2int(index_val, 10, &wwpn_index, NUM_TPYE_UCHAR);
            if (ret != RET_OK || (wwpn_index >= TARGET_MAX_NUM)) {
                debug_log(DLOG_INFO, "%s: data(%s) format error or wwpn_index(=%d) is out of buff", __FUNCTION__,
                    prop_name + 1, wwpn_index);
                return RET_ERR;
            }

            tmp_str = g_ascii_strup(prop_val_arr, strlen(prop_val_arr));
            if (tmp_str == NULL) {
                debug_log(DLOG_INFO, "%s: g_ascii_strup failed.", __FUNCTION__);
                return RET_ERR;
            }
            (void)strncpy_s(response_unit_p->portconfig.FCoEBootTarget[wwpn_index].FCoEWWPN, MAX_LEN_32 + 1, tmp_str,
                MAX_LEN_32);
            g_free(tmp_str);
        } else if (g_ascii_strncasecmp(prop_name + 1, CLP_KEY_OEMELX_LUN, strlen(CLP_KEY_OEMELX_LUN)) == 0) {
            // OEMELX_TargetLUN[0]  prop_name指向O前面的空格，偏移字符串长度+2就指向了数字
            safe_fun_ret = strncpy_s(index_val, LINE_MAX_LEN, prop_name + strlen(CLP_KEY_OEMELX_LUN) + 2, 1);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                return RET_ERR;
            }
            ret = vos_str2int(index_val, 10, &wwpn_index, NUM_TPYE_UCHAR);
            if (ret != RET_OK || (wwpn_index >= TARGET_MAX_NUM)) {
                debug_log(DLOG_INFO, "%s: data(%s) format error or wwpn_index(=%d) is out of buff", __FUNCTION__,
                    prop_name + 1, wwpn_index);
                return RET_ERR;
            }

            ret = vos_str2int(prop_val_arr, 16, &boot_lun, NUM_TPYE_SHORT);
            if (ret != RET_OK) {
                debug_log(DLOG_INFO, "%s: value(%s) format error", __FUNCTION__, prop_val_arr);
                return RET_ERR;
            }

            response_unit_p->portconfig.FCoEBootTarget[wwpn_index].bootlun = boot_lun;
        }

        for (i = 0; i < 8; i++) {
            response_unit_p->portconfig.FCoEBootTarget[i].target_id = i + 1;
        }

        if (response_unit_p->headinfo.portid % 2) {
            response_unit_p->portconfig.NparPF[0].pf_id = 1;
        } else {
            response_unit_p->portconfig.NparPF[0].pf_id = 0;
        }
    } else if (port_id == 1) {
        
        ret = g_str_has_prefix(data_value_buf, CLP_PROP_RETDATA);
        if (ret != TRUE) {
            debug_log(DLOG_INFO, "%s: g_str_has_prefix(%s) failed", __FUNCTION__, data_value_buf);
            return RET_ERR;
        }

        prop_val = g_strrstr(data_value_buf, "=");
        if (prop_val == NULL) {
            debug_log(DLOG_INFO, "%s: data(%s) format error", __FUNCTION__, data_value_buf);
            return RET_ERR;
        }

        
        safe_fun_ret = strncpy_s(prop_val_arr, sizeof(prop_val_arr), prop_val + 1, sizeof(prop_val_arr) - 1);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        dal_trim_string(prop_val_arr, MAX_LEN_32);

        if (g_ascii_strncasecmp(prop_name + 1, CLP_KEY_OEMELX_SERDES, strlen(CLP_KEY_OEMELX_SERDES)) == 0) {
            (void)strncpy_s(response_unit_p_1->portconfig.SerDesConfig, MAX_LEN_32 + 1, prop_val_arr, MAX_LEN_32);
        } else if (g_ascii_strncasecmp(prop_name + 1, CLP_KEY_OEMELX_BOOT_ENABLED,
            strlen(CLP_KEY_OEMELX_BOOT_ENABLED)) == 0) {
            _fill_2xx_portconfig_bootenable(prop_val_arr, &response_unit_p_1->portconfig.SANBootEnable);
        } else if (g_ascii_strncasecmp(prop_name + 1, CLP_KEY_OEMELX_WWPN, strlen(CLP_KEY_OEMELX_WWPN)) == 0) {
            (void)strncpy_s(response_unit_p_1->portconfig.NparPF[0].FCoEWWPN, MAX_LEN_32 + 1, prop_val_arr, MAX_LEN_32);
        } else if (g_ascii_strncasecmp(prop_name + 1, CLP_KEY_OEMELX_WWNN, strlen(CLP_KEY_OEMELX_WWNN)) == 0) {
            (void)strncpy_s(response_unit_p_1->portconfig.NparPF[0].FCoEWWNN, MAX_LEN_32 + 1, prop_val_arr, MAX_LEN_32);
        } else if (g_ascii_strncasecmp(prop_name + 1, CLP_KEY_OEMELX_TARGET_WWPN, strlen(CLP_KEY_OEMELX_TARGET_WWPN)) ==
            0) {
            // OEMELX_TargetWWPN[0]  prop_name指向O前面的空格，偏移字符串长度+2就指向了数字
            safe_fun_ret = strncpy_s(index_val, LINE_MAX_LEN, prop_name + strlen(CLP_KEY_OEMELX_TARGET_WWPN) + 2, 1);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                return RET_ERR;
            }
            ret = vos_str2int(index_val, 10, &wwpn_index, NUM_TPYE_UCHAR);
            if (ret != RET_OK || (wwpn_index >= TARGET_MAX_NUM)) {
                debug_log(DLOG_INFO, "%s: data(%s) format error or wwpn_index(=%d) is out of buff", __FUNCTION__,
                    prop_name + 1, wwpn_index);
                return RET_ERR;
            }

            tmp_str = g_ascii_strup(prop_val_arr, strlen(prop_val_arr));
            if (tmp_str == NULL) {
                debug_log(DLOG_INFO, "%s: g_ascii_strup failed.", __FUNCTION__);
                return RET_ERR;
            }
            (void)strncpy_s(response_unit_p_1->portconfig.FCoEBootTarget[wwpn_index].FCoEWWPN, MAX_LEN_32 + 1, tmp_str,
                MAX_LEN_32);
            g_free(tmp_str);
        } else if (g_ascii_strncasecmp(prop_name + 1, CLP_KEY_OEMELX_LUN, strlen(CLP_KEY_OEMELX_LUN)) == 0) {
            // OEMELX_TargetLUN[0]  prop_name指向O前面的空格，偏移字符串长度+2就指向了数字
            safe_fun_ret = strncpy_s(index_val, LINE_MAX_LEN, prop_name + strlen(CLP_KEY_OEMELX_LUN) + 2, 1);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                return RET_ERR;
            }
            ret = vos_str2int(index_val, 10, &wwpn_index, NUM_TPYE_UCHAR);
            if (ret != RET_OK || (wwpn_index >= TARGET_MAX_NUM)) {
                debug_log(DLOG_INFO, "%s: data(%s) format error or wwpn_index(=%d) is out of buff", __FUNCTION__,
                    prop_name + 1, wwpn_index);
                return RET_ERR;
            }

            ret = vos_str2int(prop_val_arr, 16, &boot_lun, NUM_TPYE_SHORT);
            if (ret != RET_OK) {
                debug_log(DLOG_INFO, "%s: value(%s) format error", __FUNCTION__, prop_val_arr);
                return RET_ERR;
            }

            response_unit_p_1->portconfig.FCoEBootTarget[wwpn_index].bootlun = boot_lun;
        }

        for (i = 0; i < 8; i++) {
            response_unit_p_1->portconfig.FCoEBootTarget[i].target_id = i + 1;
        }

        if (response_unit_p_1->headinfo.portid % 2) {
            response_unit_p_1->portconfig.NparPF[0].pf_id = 1;
        } else {
            response_unit_p_1->portconfig.NparPF[0].pf_id = 0;
        }
    }

    return RET_OK;
}


LOCAL gint32 _parse_response_show_for_520_or_522(gchar *data_buf, FM_FILE_S *file_handle,
    CLP_RESPONSE_UNIT *response_unit_p)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret = 0;
    gchar **show_array = NULL;
    gchar show_buf[PF_CONFIG_MAX_LEN  + 1];
    guint8 pf_id = INVALID_UCHAR;
    gchar *fm_ret = NULL;
    gchar       data_tmp[LINE_MAX_LEN + 1] = {0};

    if (response_unit_p == NULL || data_buf == NULL || file_handle == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL point.", __FUNCTION__);
        return RET_ERR;
    }

    safe_fun_ret = strncpy_s(data_tmp, LINE_MAX_LEN + 1, data_buf, LINE_MAX_LEN);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    dal_trim_string(data_tmp, LINE_MAX_LEN + 1);

    if (g_ascii_strncasecmp(data_tmp, CLP_CMD_SHOW, strlen(CLP_CMD_SHOW)) == 0) {
        // show pf0 QinqVlanId
        show_array = g_strsplit_set(data_tmp, " ", 0);
        if (show_array == NULL || g_strv_length(show_array) < 3) {
            if (show_array != NULL) {
                g_strfreev(show_array);
            }
            debug_log(DLOG_MASS, "%s:g_strsplit_set fail", __FUNCTION__);
            return RET_ERR;
        }

        
        if (g_ascii_strncasecmp(show_array[1], CLP_CMD_SHOW_PF, strlen(CLP_CMD_SHOW_PF)) == 0) {
            ret = vos_str2int(&show_array[1][2], 10, &pf_id, NUM_TPYE_UCHAR);
            if (ret != RET_OK) {
                g_strfreev(show_array);
                debug_log(DLOG_MASS, "%s:vos_str2int fail", __FUNCTION__);
                return RET_ERR;
            }
        }

        (void)memset_s(show_buf, sizeof(show_buf), 0, sizeof(show_buf));
        fm_ret = fm_fgets(show_buf, sizeof(show_buf) - 1, file_handle);
        if (fm_ret == NULL) {
            if (show_array != NULL) {
                g_strfreev(show_array);
            }
            debug_log(DLOG_MASS, "%s: fm_fgets fail.", __FUNCTION__);
            return RET_ERR;
        }
        ret = _parse_reponse_unit_show(response_unit_p, show_buf, pf_id, show_array[2]);
        
        if (show_array != NULL) {
            g_strfreev(show_array);
        }
        
        pf_id = INVALID_UCHAR;
        if (ret != RET_OK) {
            debug_log(DLOG_MASS, "%s: _parse_reponse_unit_show fail.", __FUNCTION__);
            return RET_ERR;
        }
    }

    return RET_OK;
}


LOCAL gint32 _parse_response_show_for_310_312_912(gchar *data_buf, FM_FILE_S *file_handle,
    CLP_RESPONSE_UNIT *response_unit_p)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret = 0;
    gchar       data_value_buf[LINE_MAX_LEN + 1] = {0};
    gchar *prop_name = NULL;
    gchar *prop_val = NULL;
    gchar       prop_val_arr[MAX_LEN_32] = {0};
    gchar       data_tmp[LINE_MAX_LEN + 1] = {0};
    gint32 i = 0;

    if (data_buf == NULL || response_unit_p == NULL || file_handle == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL point.", __FUNCTION__);
        return RET_ERR;
    }

    

    safe_fun_ret = strncpy_s(data_tmp, LINE_MAX_LEN + 1, data_buf, LINE_MAX_LEN);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    dal_trim_string(data_tmp, LINE_MAX_LEN + 1);

    prop_name = g_strrstr(data_tmp, " ");
    if (prop_name == NULL) {
        debug_log(DLOG_INFO, "%s: show(%s) format error", __FUNCTION__, data_tmp);
        return RET_ERR;
    }

    
    for (i = 0; i < 3; i++) {
        (void)memset_s(data_value_buf, LINE_MAX_LEN, 0, LINE_MAX_LEN);
        (void)fm_fgets(data_value_buf, LINE_MAX_LEN, file_handle);
    }

    
    ret = g_str_has_prefix(data_value_buf, CLP_PROP_PERMANENTADDRESS);
    if (ret != TRUE) {
        debug_log(DLOG_INFO, "%s: g_str_has_prefix(%s) failed", __FUNCTION__, data_value_buf);
        return RET_ERR;
    }

    prop_val = g_strrstr(data_value_buf, "=");
    if (prop_val == NULL) {
        debug_log(DLOG_INFO, "%s: data(%s) format error", __FUNCTION__, data_value_buf);
        return RET_ERR;
    }

    
    safe_fun_ret = strncpy_s(prop_val_arr, sizeof(prop_val_arr), prop_val + 1, sizeof(prop_val_arr) - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    dal_trim_string(prop_val_arr, MAX_LEN_32);

    if (g_ascii_strncasecmp(prop_name + 1, CLP_KEY_ETH_MAC, strlen(CLP_KEY_ETH_MAC)) == 0) {
        (void)strncpy_s(response_unit_p->portconfig.NparPF[0].PermanentAddress, MAX_LEN_32 + 1, prop_val_arr,
            MAX_LEN_32);
    }

    for (i = 0; i < 8; i++) {
        response_unit_p->portconfig.FCoEBootTarget[i].target_id = i + 1;
    }

    if (response_unit_p->headinfo.portid % 2) {
        response_unit_p->portconfig.NparPF[0].pf_id = 1;
    } else {
        response_unit_p->portconfig.NparPF[0].pf_id = 0;
    }

    return RET_OK;
}


LOCAL gint32 _parse_response_card_did_vid(CLP_RESPONSE_UNIT *response_unit_p, gchar *data, guint16 data_len)
{
    errno_t securec_rv;
    gchar **str_array = NULL;
    gint32 ret = 0;
    gchar data_tmp[LINE_MAX_LEN + 1] = {0};

    if (response_unit_p == NULL || data == NULL || data_len == 0) {
        debug_log(DLOG_ERROR, "%s:input parameter is NULL", __FUNCTION__);
        return RET_ERR;
    }

    if (strlen(data) <= 0 || strlen(data) > LINE_MAX_LEN) {
        debug_log(DLOG_MASS, "%s:buff size out of range.", __FUNCTION__);
        return RET_ERR;
    }
    
    securec_rv = strncpy_s(data_tmp, LINE_MAX_LEN + 1, data, LINE_MAX_LEN);
    if (securec_rv != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, securec_rv);
    }
    dal_trim_string(data_tmp, LINE_MAX_LEN + 1);

    // netcard=14E4:168E
    response_unit_p->cardinfo.did = 0x0000;
    response_unit_p->cardinfo.vid = 0x0000;

    str_array = g_strsplit_set(data_tmp, "=:", 0);
    if (str_array == NULL || g_strv_length(str_array) < 3) {
        if (str_array != NULL) {
            g_strfreev(str_array);
        }
        debug_log(DLOG_MASS, "%s:g_strsplit_set fail", __FUNCTION__);
        return RET_ERR;
    }

    ret = vos_str2int(str_array[1], 16, &response_unit_p->cardinfo.vid, NUM_TPYE_UINT32);
    if (ret != RET_OK) {
        g_strfreev(str_array);
        debug_log(DLOG_MASS, "%s:vos_str2int, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }
    ret = vos_str2int(str_array[2], 16, &response_unit_p->cardinfo.did, NUM_TPYE_UINT32);
    if (ret != RET_OK) {
        g_strfreev(str_array);
        debug_log(DLOG_MASS, "%s:vos_str2int, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    g_strfreev(str_array);

    return RET_OK;
}


LOCAL gint32 _parse_response_board_id(CLP_RESPONSE_UNIT *response_unit_p, const gchar *data)
{
    errno_t securec_rv;
    gchar **str_array = NULL;
    gint32 ret = 0;
    gchar data_tmp[LINE_MAX_LEN + 1] = {0};

    if (data == NULL || response_unit_p == NULL) {
        debug_log(DLOG_ERROR, "%s:input parameter is NULL", __FUNCTION__);
        return RET_ERR;
    }

    if (strlen(data) <= 0 || strlen(data) > LINE_MAX_LEN) {
        debug_log(DLOG_MASS, "%s:buff size out of range.", __FUNCTION__);
        return RET_ERR;
    }
    
    securec_rv = strncpy_s(data_tmp, LINE_MAX_LEN + 1, data, LINE_MAX_LEN);
    if (securec_rv != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, securec_rv);
    }
    dal_trim_string(data_tmp, LINE_MAX_LEN + 1);

    // data:board_id=154
    str_array = g_strsplit_set(data_tmp, "=", 0);
    if (str_array == NULL || g_strv_length(str_array) < 2) {
        if (str_array != NULL) {
            g_strfreev(str_array);
        }
        str_array = NULL;
        debug_log(DLOG_MASS, "%s:g_strsplit_set fail", __FUNCTION__);
        return RET_ERR;
    }

    response_unit_p->board_id = 0;

    ret = vos_str2int(str_array[1], 10, &response_unit_p->board_id, NUM_TPYE_UINT32);
    if (ret != RET_OK) {
        g_strfreev(str_array);
        debug_log(DLOG_MASS, "%s:vos_str2int, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    g_strfreev(str_array);

    return RET_OK;
}


LOCAL gint32 _parse_response_head_info(CLP_RESPONSE_UNIT *response_unit_p, const gchar *data)
{
    errno_t securec_rv;
    gchar **str_array = NULL;
    gint32 ret = 0;
    gchar data_tmp[LINE_MAX_LEN + 1] = {0};

    if (response_unit_p == NULL || data == NULL) {
        debug_log(DLOG_ERROR, "%s:input parameter is NULL", __FUNCTION__);
        return RET_ERR;
    }

    if (strlen(data) <= 0 || strlen(data) > LINE_MAX_LEN) {
        debug_log(DLOG_MASS, "%s:buff size out of range.", __FUNCTION__);
        return RET_ERR;
    }
    
    securec_rv = strncpy_s(data_tmp, LINE_MAX_LEN + 1, data, LINE_MAX_LEN);
    if (securec_rv != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, securec_rv);
    }
    
    dal_trim_string(data_tmp, LINE_MAX_LEN + 1);

    // data:mezz3.1.0

    str_array = g_strsplit_set(data_tmp, ".", 0);
    if (str_array == NULL || g_strv_length(str_array) < 3) {
        if (str_array != NULL) {
            g_strfreev(str_array);
        }
        debug_log(DLOG_MASS, "%s:g_strsplit_set fail", __FUNCTION__);
        return RET_ERR;
    }

    if (response_unit_p != NULL) {
        (void)strncpy_s(response_unit_p->headinfo.devicename, MAX_PROPERTY_VALUE_LEN, str_array[0],
            MAX_PROPERTY_VALUE_LEN - 1);

        ret = vos_str2int(str_array[1], 10, &response_unit_p->headinfo.chipid, NUM_TPYE_UCHAR);
        if (ret != RET_OK) {
            debug_log(DLOG_MASS, "%s:vos_str2int, ret = %d", __FUNCTION__, ret);
            g_strfreev(str_array);
            return RET_ERR;
        }

        (void)vos_str2int(str_array[2], 10, &response_unit_p->headinfo.portid, NUM_TPYE_UCHAR);

        if (ret != RET_OK) {
            debug_log(DLOG_MASS, "%s:vos_str2int, ret = %d", __FUNCTION__, ret);
            g_strfreev(str_array);
            return RET_ERR;
        }
    }

    g_strfreev(str_array);

    return RET_OK;
}


LOCAL void _fill_2xx_portconfig_bootenable(const gchar *ini_val, guchar *bootenable)
{
    if (bootenable == NULL) {
        return;
    }

    if (g_strcmp0(ini_val, CLP_VAL_ENABLED_SM) == 0 ||
        g_ascii_strncasecmp(ini_val, CLP_VAL_ENABLED, strlen(CLP_VAL_ENABLED)) == 0) {
        *bootenable = TRUE;
    } else if (g_strcmp0(ini_val, CLP_VAL_DISABLED_SM) == 0 ||
        g_ascii_strncasecmp(ini_val, CLP_VAL_DISABLED, strlen(CLP_VAL_DISABLED)) == 0) {
        *bootenable = FALSE;
    } else {
        debug_log(DLOG_INFO, "%s: invalid boot enable is %s.", __FUNCTION__, ini_val);
    }
}


LOCAL gint32 _parse_reponse_unit_show(CLP_RESPONSE_UNIT *response_unit_p, gchar *data_buf, guchar pf_id,
    gchar *show_cmd)
{
    errno_t securec_rv;
    gint32 ret = 0;
    gchar **str_array = NULL;
    gchar data[PF_CONFIG_MAX_LEN  + 1] = {0};
    gint16 status_val = INVALID_INT_VAL;

    if (response_unit_p == NULL || data_buf == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RET_ERR;
    }

    if (strlen(data_buf) <= 0 || strlen(data_buf) > PF_CONFIG_MAX_LEN || strlen(show_cmd) > LINE_MAX_LEN) {
        debug_log(DLOG_MASS, "%s:buff size out of range.", __FUNCTION__);
        return RET_ERR;
    }

    securec_rv = strcpy_s(data, sizeof(data), data_buf);
    if (securec_rv != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, securec_rv);
    }

    dal_trim_string(data, sizeof(data));

    str_array = g_strsplit_set(data, "=", 0);
    if (str_array == NULL || g_strv_length(str_array) < 2) {
        if (str_array != NULL) {
            g_strfreev(str_array);
        }
        debug_log(DLOG_MASS, "%s:g_strsplit_set fail", __FUNCTION__);
        return RET_ERR;
    }

    
    if (g_ascii_strncasecmp(str_array[0], CLP_KEY_STATUS, strlen(CLP_KEY_STATUS)) == 0) {
        ret = vos_str2int(str_array[1], 10, &status_val, NUM_TPYE_SHORT);
        if (ret != RET_OK) {
            g_strfreev(str_array);
            debug_log(DLOG_MASS, "%s:vos_str2int fail", __FUNCTION__);
            return RET_ERR;
        }

        if (status_val != 0) {
            ret = _update_reponse_unit_show_config(response_unit_p, show_cmd, INVALID_STR_VAL, pf_id);
            if (ret != RET_OK) {
                g_strfreev(str_array);
                debug_log(DLOG_ERROR, "%s:_update_reponse_unit_show_config fail", __FUNCTION__);
                return RET_ERR;
            }
            if (str_array != NULL) {
                g_strfreev(str_array);
            }
            return RET_OK;
        }
    }

    ret = _update_reponse_unit_show_config(response_unit_p, str_array[0], str_array[1], pf_id);
    if (ret != RET_OK) {
        g_strfreev(str_array);
        debug_log(DLOG_MASS, "%s:_update_reponse_unit_show_config fail", __FUNCTION__);
        return RET_ERR;
    }

    g_strfreev(str_array);
    return RET_OK;
}


LOCAL gint32 _update_reponse_unit_show_config(CLP_RESPONSE_UNIT *response_unit_p, gchar *prop_key, gchar *prop_val,
    guchar pf_id)
{
    gint32 ret = 0;
    guint8 keyid = 0;

    if (response_unit_p == NULL || prop_key == NULL || prop_val == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RET_ERR;
    }

    
    if (g_ascii_strncasecmp(prop_key, CLP_KEY_MULTI_MODE, strlen(CLP_KEY_MULTI_MODE)) == 0) {
        // @example  MultiFunctionMode=SF  @example
        (void)strncpy_s(response_unit_p->chipconfig.MultifunctionMode, MAX_LEN_32, prop_val, MAX_LEN_32 - 1);
    } else if (g_ascii_strncasecmp(prop_key, CLP_KEY_SRIOV, strlen(CLP_KEY_SRIOV)) == 0) {
        // @example  SRIOV=Disabled  @example
        response_unit_p->portconfig.SRIOVEnabled = (strlen(prop_val) ?
            (g_ascii_strncasecmp(prop_val, CLP_VAL_ENABLED, strlen(CLP_VAL_ENABLED)) == 0 ? TRUE : FALSE) :
            INVALID_UCHAR);
    }

    
    else if (g_ascii_strncasecmp(prop_key, CLP_KEY_BOOT_ENABLE, strlen(CLP_KEY_BOOT_ENABLE)) == 0) {
        // @example  BootEnable=ENABLED  @example
        response_unit_p->portconfig.BootEnable = (strlen(prop_val) ?
            (g_ascii_strncasecmp(prop_val, CLP_VAL_ENABLED, strlen(CLP_VAL_ENABLED)) == 0 ? TRUE : FALSE) :
            INVALID_UCHAR);
    } else if (g_ascii_strncasecmp(prop_key, CLP_KEY_BOOT_PROTOCOL, strlen(CLP_KEY_BOOT_PROTOCOL)) == 0) {
        // @example  BootProtocol=PXE  @example
        (void)strncpy_s(response_unit_p->portconfig.BootProtocol, MAX_LEN_32 + 1, prop_val, MAX_LEN_32);
    }

    else if (g_ascii_strcasecmp(prop_key, CLP_KEY_BOOT_TARGET) == 0) {
        // @example  FCoeBootToTarget=DISABLED  @example
        response_unit_p->portconfig.BootToTarget = (strlen(prop_val) ?
            (g_ascii_strncasecmp(prop_val, CLP_VAL_ENABLED, strlen(CLP_VAL_ENABLED)) == 0 ? TRUE : FALSE) :
            INVALID_UCHAR);
    }

    
    else if (g_ascii_strcasecmp(prop_key, CLP_KEY_VLANID) == 0) {
        // @example  VlanID=2048  @example
        if (strlen(prop_val) == 0) {
            (response_unit_p->portconfig.VlanId = INVALID_INT_VAL);
            return RET_OK;
        }

        ret = vos_str2int(prop_val, 10, &response_unit_p->portconfig.VlanId, NUM_TPYE_SHORT);
        if (ret != RET_OK) {
            debug_log(DLOG_MASS, "%s:vos_str2int fail", __FUNCTION__);
            return RET_ERR;
        }
    } else {
        
        ret = _parse_pf_vlan_info(response_unit_p, pf_id, prop_key, prop_val);
        if (ret != RET_OK) {
            debug_log(DLOG_MASS, "%s:_parse_pf_vlan_info fail", __FUNCTION__);
            return RET_ERR;
        }
    }

    
    if (g_ascii_strncasecmp(response_unit_p->chipconfig.MultifunctionMode, CLP_VAL_NPAR, strlen(CLP_VAL_NPAR)) == 0) {
        if (g_ascii_strncasecmp(prop_key, CLP_KEY_NPARPF, strlen(CLP_KEY_NPARPF)) == 0) {
            ret = vos_str2int(&prop_key[6], 10, &keyid, NUM_TPYE_UCHAR);
            if (ret != RET_OK) {
                debug_log(DLOG_MASS, "%s:vos_str2int fail", __FUNCTION__);
                return RET_ERR;
            }
            ret = _parse_pf_info(response_unit_p, keyid, prop_val);
            if (ret != RET_OK) {
                debug_log(DLOG_MASS, "%s:_parse_pf_info fail", __FUNCTION__);
                return RET_ERR;
            }
        }
    }
    
    
    else if (g_ascii_strncasecmp(response_unit_p->chipconfig.MultifunctionMode, CLP_VAL_SF, strlen(CLP_VAL_SF)) == 0 ||
        g_ascii_strcasecmp(response_unit_p->chipconfig.MultifunctionMode, "") == 0) {
        ret = _parse_port_info(response_unit_p, response_unit_p->headinfo.portid, prop_key, prop_val);
        if (ret != RET_OK) {
            debug_log(DLOG_MASS, "%s:_parse_port_info fail", __FUNCTION__);
            return RET_ERR;
        }
    }

    
    
    if (response_unit_p->portconfig.BootToTarget == TRUE) {
        ret = _parse_fcoeboottargets_info(response_unit_p, prop_key, prop_val);
        if (ret != RET_OK) {
            debug_log(DLOG_MASS, "%s:_parse_fcoeboottargets_info fail", __FUNCTION__);
            return RET_ERR;
        }
    }

    return RET_OK;
}


LOCAL gint32 _parse_pf_vlan_info(CLP_RESPONSE_UNIT *response_unit_p, guint8 keyid, gchar *prop_key, gchar *prop_val)
{
    guint32 index_id = 0;
    gint32 ret = 0;

    if (response_unit_p == NULL || prop_val == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RET_ERR;
    }

    
    // 此处返回ok是保证调用此函数后面的内容继续执行
    if (keyid > 7) {
        debug_log(DLOG_MASS, "%s:pf_id out of range fail", __FUNCTION__);
        return RET_OK;
    }
    index_id = keyid / 2;

    
    response_unit_p->portconfig.NparPF[index_id].pf_id = keyid;

    // @example  QinqVlanMode=FILTERING  @example
    if (g_ascii_strncasecmp(prop_key, CLP_KEY_QINQ_VLAN_MODE, strlen(CLP_KEY_QINQ_VLAN_MODE)) == 0) {
        (void)strncpy_s(response_unit_p->portconfig.NparPF[index_id].QinqVlanMode, MAX_LEN_32 + 1, prop_val,
            MAX_LEN_32);
    }
    // @example  QinqVlanID=   0  @example
    else if (g_ascii_strncasecmp(prop_key, CLP_KEY_QINQ_VLANID, strlen(CLP_KEY_QINQ_VLANID)) == 0) {
        if (strlen(prop_val) == 0) {
            (response_unit_p->portconfig.NparPF[index_id].QinqVlanId = INVALID_INT_VAL);
            return RET_OK;
        }
        ret = vos_str2int(prop_val, 10, &response_unit_p->portconfig.NparPF[index_id].QinqVlanId, NUM_TPYE_SHORT);
        if (ret != RET_OK) {
            debug_log(DLOG_MASS, "%s:vos_str2int fail, ret = %d", __FUNCTION__, ret);
            return RET_ERR;
        }
    }
    // @example  QinqPriority= 0  @example
    else if (g_ascii_strncasecmp(prop_key, CLP_KEY_QINQ_VLAN_PRIORITY, strlen(CLP_KEY_QINQ_VLAN_PRIORITY)) == 0 ||
        g_ascii_strncasecmp(prop_key, CLP_KEY_QINQ_PRIORITY, strlen(CLP_KEY_QINQ_PRIORITY)) == 0) {
        if (strlen(prop_val) == 0) {
            (response_unit_p->portconfig.NparPF[index_id].QinqVlanPriority = INVALID_UCHAR);
            return RET_OK;
        }

        ret = vos_str2int(prop_val, 10, &response_unit_p->portconfig.NparPF[index_id].QinqVlanPriority, NUM_TPYE_UCHAR);
        if (ret != RET_OK) {
            debug_log(DLOG_MASS, "%s:vos_str2int fail, ret = %d", __FUNCTION__, ret);
            return RET_ERR;
        }
    }
    // @example  QinqVlanTrunkRange=0,1-5  @example
    else if (g_ascii_strncasecmp(prop_key, CLP_KEY_QINQ_VLAN_TRUNKRANGE, strlen(CLP_KEY_QINQ_VLAN_TRUNKRANGE)) == 0) {
        (void)_trunk_range_replace(prop_val);
        ret = strcpy_s(response_unit_p->portconfig.NparPF[index_id].QinqVlanTrunkRange,
            sizeof(response_unit_p->portconfig.NparPF[index_id].QinqVlanTrunkRange), prop_val);
        if (ret != EOK) {
            return RET_ERR;
        }
    }

    return RET_OK;
}


LOCAL gint32 _parse_port_info(CLP_RESPONSE_UNIT *response_unit_p, guchar pf_id, gchar *prop_key, gchar *prop_val)
{
    if (response_unit_p == NULL || prop_val == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RET_ERR;
    }

    response_unit_p->portconfig.NparPF[0].pf_id = pf_id;

    
    // @example  PermanentAddress=A08CF88BA3E0  @example
    if (g_ascii_strncasecmp(prop_key, CLP_KEY_ETH_MAC, strlen(CLP_KEY_ETH_MAC)) == 0) {
        (void)strncpy_s(response_unit_p->portconfig.NparPF[0].PermanentAddress,
            sizeof(response_unit_p->portconfig.NparPF[0].PermanentAddress), prop_val,
            sizeof(response_unit_p->portconfig.NparPF[0].PermanentAddress) - 1);
    }
    // @example  iScsiAddress=A08CF88BA3E1  @example
    else if (g_ascii_strncasecmp(prop_key, "iScsiAddress", strlen("iScsiAddress")) == 0) {
        (void)strncpy_s(response_unit_p->portconfig.NparPF[0].iScsiAddress,
            sizeof(response_unit_p->portconfig.NparPF[0].iScsiAddress), prop_val,
            sizeof(response_unit_p->portconfig.NparPF[0].iScsiAddress) - 1);
    }
    // @example  FCoEFipMACAddress=A08CF88BA3E1  @example
    else if (g_ascii_strncasecmp(prop_key, CLP_KEY_FCOE_MAC, strlen(CLP_KEY_FCOE_MAC)) == 0) {
        (void)strncpy_s(response_unit_p->portconfig.NparPF[0].FCoEFipMACAddress,
            sizeof(response_unit_p->portconfig.NparPF[0].FCoEFipMACAddress), prop_val,
            sizeof(response_unit_p->portconfig.NparPF[0].FCoEFipMACAddress) - 1);
    }
    
    // @example  FCoEWWPN=2000A08CF88BA3E1  @example
    else if (g_ascii_strncasecmp(prop_key, CLP_KEY_FCOE_WWPN, strlen(CLP_KEY_FCOE_WWPN)) == 0) {
        (void)strncpy_s(response_unit_p->portconfig.NparPF[0].FCoEWWPN,
            sizeof(response_unit_p->portconfig.NparPF[0].FCoEWWPN), prop_val,
            sizeof(response_unit_p->portconfig.NparPF[0].FCoEWWPN) - 1);
    }
    // @example  FCoEWWNN=1000A08CF88BA3E1  @example
    else if (g_ascii_strncasecmp(prop_key, CLP_KEY_FCOE_WWNN, strlen(CLP_KEY_FCOE_WWNN)) == 0) {
        (void)strncpy_s(response_unit_p->portconfig.NparPF[0].FCoEWWNN,
            sizeof(response_unit_p->portconfig.NparPF[0].FCoEWWNN), prop_val,
            sizeof(response_unit_p->portconfig.NparPF[0].FCoEWWNN) - 1);
    }

    

    return RET_OK;
}


LOCAL gint32 _parse_pf_info(CLP_RESPONSE_UNIT *response_unit_p, guint8 keyid, gchar *prop_val)
{
    guint32 index_id = 0;
    gchar **str_array = NULL;
    gint32 ret = 0;

    if (response_unit_p == NULL || prop_val == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RET_ERR;
    }

    // data:NparPF<n>=<ETHERNET|FCOE|ISCSI>;<bandwidth weight>;<maximum bandwidth>;<network mac address>;<iscsi mac
    // address>;<fcoe fip mac address>;<fcoe wwpn>;<fcoe wwnn>
    
    if (keyid > 7) {
        debug_log(DLOG_MASS, "%s:pf_id outof range fail", __FUNCTION__);
        return RET_ERR;
    }
    index_id = keyid / 2;

    str_array = g_strsplit_set(prop_val, ";", 0);
    if (str_array == NULL || g_strv_length(str_array) < 8) {
        if (str_array != NULL) {
            g_strfreev(str_array);
        }
        debug_log(DLOG_MASS, "%s:g_strsplit_set fail", __FUNCTION__);
        return RET_ERR;
    }

    
    response_unit_p->portconfig.NparPF[index_id].pf_id = keyid;

    
    if (!g_ascii_strncasecmp(str_array[0], "ETHERNET:FCoE", strlen("ETHERNET:FCoE"))) {
        (void)strncpy_s(response_unit_p->portconfig.NparPF[index_id].Protocol,
            sizeof(response_unit_p->portconfig.NparPF[index_id].Protocol), "FCoE", strlen("FCoE"));
    } else if (!g_ascii_strncasecmp(str_array[0], "ETHERNET:iSCSI", strlen("ETHERNET:iSCSI"))) {
        (void)strncpy_s(response_unit_p->portconfig.NparPF[index_id].Protocol,
            sizeof(response_unit_p->portconfig.NparPF[index_id].Protocol), "iSCSI", strlen("iSCSI"));
    } else {
        (void)strncpy_s(response_unit_p->portconfig.NparPF[index_id].Protocol,
            sizeof(response_unit_p->portconfig.NparPF[index_id].Protocol), str_array[0],
            sizeof(response_unit_p->portconfig.NparPF[index_id].Protocol) - 1);
    }

    if (strlen(prop_val) == 0) {
        if (str_array) {
            g_strfreev(str_array);
        }
        response_unit_p->portconfig.NparPF[index_id].MinBandwidth = INVALID_UCHAR;
        response_unit_p->portconfig.NparPF[index_id].MaxBandwidth = INVALID_UCHAR;
        return RET_OK;
    }

    ret = vos_str2int(str_array[1], 10, &response_unit_p->portconfig.NparPF[index_id].MinBandwidth, NUM_TPYE_UCHAR);
    if (ret != RET_OK) {
        g_strfreev(str_array);
        debug_log(DLOG_MASS, "%s:_update_FCoEboottarget_info, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }
    ret = vos_str2int(str_array[2], 10, &response_unit_p->portconfig.NparPF[index_id].MaxBandwidth, NUM_TPYE_UCHAR);
    if (ret != RET_OK) {
        g_strfreev(str_array);
        debug_log(DLOG_MASS, "%s:_update_FCoEboottarget_info, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }
    (void)strncpy_s(response_unit_p->portconfig.NparPF[index_id].PermanentAddress,
        sizeof(response_unit_p->portconfig.NparPF[index_id].PermanentAddress), str_array[3],
        sizeof(response_unit_p->portconfig.NparPF[index_id].PermanentAddress) - 1);
    (void)strncpy_s(response_unit_p->portconfig.NparPF[index_id].iScsiAddress,
        sizeof(response_unit_p->portconfig.NparPF[index_id].iScsiAddress), str_array[4],
        sizeof(response_unit_p->portconfig.NparPF[index_id].iScsiAddress) - 1);
    (void)strncpy_s(response_unit_p->portconfig.NparPF[index_id].FCoEFipMACAddress,
        sizeof(response_unit_p->portconfig.NparPF[index_id].FCoEFipMACAddress), str_array[5],
        sizeof(response_unit_p->portconfig.NparPF[index_id].FCoEFipMACAddress) - 1);
    (void)strncpy_s(response_unit_p->portconfig.NparPF[index_id].FCoEWWPN,
        sizeof(response_unit_p->portconfig.NparPF[index_id].FCoEWWPN), str_array[6],
        sizeof(response_unit_p->portconfig.NparPF[index_id].FCoEWWPN) - 1);
    (void)strncpy_s(response_unit_p->portconfig.NparPF[index_id].FCoEWWNN,
        sizeof(response_unit_p->portconfig.NparPF[index_id].FCoEWWNN), str_array[7],
        sizeof(response_unit_p->portconfig.NparPF[index_id].FCoEWWNN) - 1);

    g_strfreev(str_array);
    return RET_OK;
}


LOCAL gint32 _parse_fcoeboottargets_info(CLP_RESPONSE_UNIT *response_unit_p, gchar *prop_key, gchar *prop_val)
{
    gint32 ret = 0;

    if (response_unit_p == NULL || prop_key == NULL || prop_val == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RET_ERR;
    }

    if (g_ascii_strncasecmp(prop_key, CLP_KEY_BOOT_TARGET1, strlen(CLP_KEY_BOOT_TARGET1)) == 0) {
        ret = _update_fcoeboottarget_info(response_unit_p, prop_key, prop_val, 1);
        if (ret != RET_OK) {
            debug_log(DLOG_MASS, "%s:_update_fcoeboottarget_info, ret = %d", __FUNCTION__, ret);
            return RET_ERR;
        }
    } else if (g_ascii_strncasecmp(prop_key, CLP_KEY_BOOT_TARGET2, strlen(CLP_KEY_BOOT_TARGET2)) == 0) {
        ret = _update_fcoeboottarget_info(response_unit_p, prop_key, prop_val, 2);
        if (ret != RET_OK) {
            debug_log(DLOG_MASS, "%s:_update_fcoeboottarget_info, ret = %d", __FUNCTION__, ret);
            return RET_ERR;
        }
    } else if (g_ascii_strncasecmp(prop_key, CLP_KEY_BOOT_TARGET3, strlen(CLP_KEY_BOOT_TARGET3)) == 0) {
        ret = _update_fcoeboottarget_info(response_unit_p, prop_key, prop_val, 3);
        if (ret != RET_OK) {
            debug_log(DLOG_MASS, "%s:_update_fcoeboottarget_info, ret = %d", __FUNCTION__, ret);
            return RET_ERR;
        }
    } else if (g_ascii_strncasecmp(prop_key, CLP_KEY_BOOT_TARGET4, strlen(CLP_KEY_BOOT_TARGET4)) == 0) {
        ret = _update_fcoeboottarget_info(response_unit_p, prop_key, prop_val, 4);
        if (ret != RET_OK) {
            debug_log(DLOG_MASS, "%s:_update_fcoeboottarget_info, ret = %d", __FUNCTION__, ret);
            return RET_ERR;
        }
    } else if (g_ascii_strncasecmp(prop_key, CLP_KEY_BOOT_TARGET5, strlen(CLP_KEY_BOOT_TARGET5)) == 0) {
        ret = _update_fcoeboottarget_info(response_unit_p, prop_key, prop_val, 5);
        if (ret != RET_OK) {
            debug_log(DLOG_MASS, "%s:_update_fcoeboottarget_info, ret = %d", __FUNCTION__, ret);
            return RET_ERR;
        }
    } else if (g_ascii_strncasecmp(prop_key, CLP_KEY_BOOT_TARGET6, strlen(CLP_KEY_BOOT_TARGET6)) == 0) {
        ret = _update_fcoeboottarget_info(response_unit_p, prop_key, prop_val, 6);
        if (ret != RET_OK) {
            debug_log(DLOG_MASS, "%s:_update_fcoeboottarget_info, ret = %d", __FUNCTION__, ret);
            return RET_ERR;
        }
    } else if (g_ascii_strncasecmp(prop_key, CLP_KEY_BOOT_TARGET7, strlen(CLP_KEY_BOOT_TARGET7)) == 0) {
        ret = _update_fcoeboottarget_info(response_unit_p, prop_key, prop_val, 7);
        if (ret != RET_OK) {
            debug_log(DLOG_MASS, "%s:_update_fcoeboottarget_info, ret = %d", __FUNCTION__, ret);
            return RET_ERR;
        }
    } else if (g_ascii_strncasecmp(prop_key, CLP_KEY_BOOT_TARGET8, strlen(CLP_KEY_BOOT_TARGET8)) == 0) {
        ret = _update_fcoeboottarget_info(response_unit_p, prop_key, prop_val, 8);
        if (ret != RET_OK) {
            debug_log(DLOG_MASS, "%s:_update_fcoeboottarget_info, ret = %d", __FUNCTION__, ret);
            return RET_ERR;
        }
    }

    return RET_OK;
}


LOCAL gchar *_trunk_range_replace(gchar *prop_val)
{
    gchar *psrc = prop_val;
    gchar *pdst = prop_val;

    if (prop_val == NULL || !strlen(prop_val)) {
        return NULL;
    }

    while (*psrc != '\0') {
        if (*psrc != ' ') {
            *pdst = *psrc;
            pdst++;
        } else {
            if ((*(psrc + 1) >= '0' && *(psrc + 1) <= '9') && (*(psrc - 1) >= '0' && *(psrc - 1) <= '9')) {
                *pdst = ',';
                pdst++;
            }
        }

        psrc++;
    }

    *pdst = '\0';

    return prop_val;
}


LOCAL gint32 _update_fcoeboottarget_info(CLP_RESPONSE_UNIT *response_unit_p, gchar *prop_key, gchar *prop_val,
    guint32 target_id)
{
    gchar **str_vector = NULL;
    gint32 ret = 0;

    if (response_unit_p == NULL || prop_key == NULL || prop_val == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RET_ERR;
    }

    // @example  FCoEBootTarget=ENABLED;10000000000000022;225  @example
    str_vector = g_strsplit_set(prop_val, ";", 0);
    if (str_vector == NULL || g_strv_length(str_vector) < 3) {
        if (str_vector != NULL) {
            g_strfreev(str_vector);
        }
        debug_log(DLOG_MASS, "%s:g_strsplit_set fail", __FUNCTION__);
        return RET_ERR;
    }

    if (strlen(prop_val) == 0) {
        if (str_vector) {
            g_strfreev(str_vector);
        }
        response_unit_p->portconfig.FCoEBootTarget[target_id - 1].TargetEnable = INVALID_UCHAR;
        response_unit_p->portconfig.FCoEBootTarget[target_id - 1].bootlun = INVALID_INT_VAL;
        return RET_OK;
    }

    response_unit_p->portconfig.FCoEBootTarget[target_id - 1].target_id = target_id;
    response_unit_p->portconfig.FCoEBootTarget[target_id - 1].TargetEnable = (strlen((const gchar *)str_vector[0]) ?
        (g_ascii_strncasecmp(str_vector[0], CLP_VAL_ENABLED, strlen(CLP_VAL_ENABLED)) == 0 ? TRUE : FALSE) :
        INVALID_UCHAR);

    (void)strncpy_s(response_unit_p->portconfig.FCoEBootTarget[target_id - 1].FCoEWWPN,
        sizeof(response_unit_p->portconfig.FCoEBootTarget[target_id - 1].FCoEWWPN), str_vector[1],
        sizeof(response_unit_p->portconfig.FCoEBootTarget[target_id - 1].FCoEWWPN) - 1);

    ret = vos_str2int(str_vector[2], 10, &response_unit_p->portconfig.FCoEBootTarget[target_id - 1].bootlun,
        NUM_TPYE_SHORT);
    if (ret != RET_OK) {
        g_strfreev(str_vector);
        debug_log(DLOG_MASS, "%s:vos_str2int, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    g_strfreev(str_vector);
    return RET_OK;
}
