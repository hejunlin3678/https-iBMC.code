

#include "set_clp_520_522.h"

LOCAL gint32 _chip_rules_check(guint16 board_id, GSList *config_list, json_object **error_info);
LOCAL gint32 _parse_set_file_content_for_520_522(GSList *config_list, gchar *content, guint32 size);
LOCAL gint32 _chip_rules_vlan_mode_check(gchar **pf_vlan_mode, gchar *mulfunc_mode);
LOCAL void _check_bootlun_is_invalid(FCOE_BOOT_TARGET target_data, gchar *o_str, guint32 o_str_size);


gint32 bios_set_json_to_clp_520_522(guint16 board_id, const gchar *devicename, const gchar *config_data,
    json_object **error_info)
{
    gint32 ret = 0;
    GSList *config_list = NULL;
    gchar *content = NULL;
    CLP_SET_S clp_set = { 0, CLP_520_522 };
    clp_set.board_id = board_id;
    clp_set.clp_flag = CLP_520_522;

    
    if (config_data == NULL || error_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    
    if (board_id != MXEN_ID && board_id != MXEP_ID) {
        debug_log(DLOG_ERROR, "%s:Board id is not match%d.", __FUNCTION__, board_id);
        return BOARD_UNSUPPORTED;
    }

    
    ret = _json_set_and_rules_check(devicename, config_data, error_info, &config_list, clp_set);
    if (ret != RET_OK) {
        if (config_list) {
            g_slist_free_full(config_list, (GDestroyNotify)g_free);
        }
        debug_log(DLOG_ERROR, "%s:_json_set_and_rules_check failed, ret is %d", __FUNCTION__, ret);
        return ret;
    }

    ret = _chip_rules_check(board_id, config_list, error_info);
    if (ret != RET_OK) {
        if (config_list) {
            g_slist_free_full(config_list, (GDestroyNotify)g_free);
        }
        debug_log(DLOG_ERROR, "%s:_chip_rules_check failed", __FUNCTION__);
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
    ret = _parse_set_file_content_for_520_522(config_list, content, SMBIOS_DATA_SIZE);
    if (ret != RET_OK) {
        g_free(content);
        if (config_list) {
            g_slist_free_full(config_list, (GDestroyNotify)g_free);
        }
        debug_log(DLOG_ERROR, "%s:_parse_set_file_content failed", __FUNCTION__);
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


gint32 _parse_set_file_content_for_520_522(GSList *config_list, gchar *content, guint32 size)
{
    gint32 iRet = -1;
    errno_t securec_rv;
    GSList *obj_node = NULL;
    CLP_RESPONSE_UNIT *node_p = NULL;
    gchar str_val[LINE_MAX_LEN + 1] = {0};
    gchar vlan_trunk_range[PF_CONFIG_MAX_LEN + 1] = {0};
    gint32 pf_index = 0;
    gint32 target_index = 0;
    gchar *devicename = NULL;
    gchar *vid_did = NULL;

    if (config_list == NULL || content == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.\n", __FUNCTION__);
        return RET_ERR;
    }

    for (gint32 i = 0; i < 2; i++) {
        if (i == 0) {
            vid_did = MXEP_VID_DID;
        } else {
            vid_did = MXEP_VID_DID_NPAR;
        }

        for (obj_node = config_list; obj_node; obj_node = obj_node->next) {
            node_p = (CLP_RESPONSE_UNIT *)obj_node->data;
            securec_rv = strncat_s(content, size, "{\r\n", strlen("{\r\n"));
            if (securec_rv != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
            }
            devicename = g_ascii_strdown(node_p->headinfo.devicename, strlen(node_p->headinfo.devicename));
            if (devicename != NULL) {
                iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "#%s.%d.%d\r\n", devicename,
                    node_p->headinfo.chipid, node_p->headinfo.portid);
                if (iRet <= 0) {
                    debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
                }
                _data_string_concat(str_val, size, content);

                (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
                iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "%s.%d.%d\r\n\r\n", devicename,
                    node_p->headinfo.chipid, node_p->headinfo.portid);
                if (iRet <= 0) {
                    debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
                }
                _data_string_concat(str_val, size, content);
                g_free(devicename);
            }

            securec_rv = strncat_s(content, size, "#board_id\r\n", strlen("#board_id\r\n"));
            if (securec_rv != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
            }
            (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
            (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "board_id=%u\r\n\r\n", node_p->board_id);
            _data_string_concat(str_val, size, content);

            securec_rv = strncat_s(content, size, "#netcard\r\n", strlen("#netcard\r\n"));
            if (securec_rv != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
            }
            (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
            iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "netcard=%s\r\n", vid_did);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
            }
            _data_string_concat(str_val, size, content);

            securec_rv = strncat_s(content, size, "#reset\r\n", strlen("#reset\r\n"));
            if (securec_rv != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
            }
            securec_rv = strncat_s(content, size, "#CMD\r\n", strlen("#CMD\r\n"));
            if (securec_rv != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
            }
            securec_rv = strncat_s(content, size, "set netport1 default\r\n", strlen("set netport1 default\r\n"));
            if (securec_rv != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
            }
            (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
            iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "set netport1 %s=%s\r\n", CLP_KEY_MULTI_MODE,
                node_p->chipconfig.MultifunctionMode);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
            }
            _data_string_concat(str_val, size, content);

            (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);

            if (node_p->portconfig.SRIOVEnabled != INVALID_UCHAR) {
                if (g_ascii_strcasecmp(node_p->chipconfig.MultifunctionMode, CLP_VAL_NPAR) ==
                    0) { // 当为NPAR模式默认设置SRIOV为0
                    (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "set netport1 %s=%s\r\n", CLP_KEY_SRIOV,
                        CLP_VAL_DISABLED);
                } else {
                    (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "set netport1 %s=%s\r\n", CLP_KEY_SRIOV,
                        node_p->portconfig.SRIOVEnabled == TRUE ? CLP_VAL_ENABLED : CLP_VAL_DISABLED);
                }

                _data_string_concat(str_val, size, content);
            }

            (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);

            if (strlen(node_p->portconfig.BootProtocol)) {
                iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "set netport1 %s=%s\r\n",
                    CLP_KEY_BOOT_PROTOCOL, node_p->portconfig.BootProtocol);
                if (iRet <= 0) {
                    debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
                }
                _data_string_concat(str_val, size, content);
            }

            (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);

            if (node_p->portconfig.BootToTarget != INVALID_UCHAR) {
                (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "set netport1 %s=%s\r\n", CLP_KEY_BOOT_TARGET,
                    node_p->portconfig.BootToTarget == TRUE ? CLP_VAL_ENABLED : CLP_VAL_DISABLED);
                _data_string_concat(str_val, size, content);
            }

            (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);

            if (node_p->portconfig.VlanId != INVALID_INT_VAL) {
                (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "set netport1 %s=%d\r\n", CLP_KEY_VLAN,
                    node_p->portconfig.VlanId);
                _data_string_concat(str_val, size, content);
            }

            

            for (pf_index = 0; pf_index < PORT_PF_MAX; pf_index++) {
                (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);

                if (node_p->portconfig.NparPF[pf_index].pf_id != INVALID_UCHAR) {
                    if (!g_ascii_strncasecmp(node_p->chipconfig.MultifunctionMode, CLP_VAL_SF, strlen(CLP_VAL_SF))) {
                        iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN,
                            "set netport1 PermanentAddress=%s\r\n", node_p->portconfig.NparPF[0].PermanentAddress);
                        if (iRet <= 0) {
                            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
                        }
                        _data_string_concat(str_val, size, content);

                        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);

                        if (strlen(node_p->portconfig.NparPF[0].iScsiAddress)) {
                            iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN,
                                "set netport1 iScsiAddress=%s\r\n", node_p->portconfig.NparPF[0].iScsiAddress);
                            if (iRet <= 0) {
                                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
                            }
                            _data_string_concat(str_val, size, content);
                        }

                        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);

                        if (strlen(node_p->portconfig.NparPF[0].FCoEFipMACAddress)) {
                            iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN,
                                "set netport1 FCoEFipMACAddress=%s\r\n",
                                node_p->portconfig.NparPF[0].FCoEFipMACAddress);
                            if (iRet <= 0) {
                                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
                            }
                            _data_string_concat(str_val, size, content);
                        }

                        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);

                        if (strlen(node_p->portconfig.NparPF[0].FCoEWWPN)) {
                            iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "set netport1 FCoEWWPN=%s\r\n",
                                node_p->portconfig.NparPF[0].FCoEWWPN);
                            if (iRet <= 0) {
                                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
                            }
                            _data_string_concat(str_val, size, content);
                        }

                        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);

                        if (strlen(node_p->portconfig.NparPF[0].FCoEWWNN)) {
                            iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "set netport1 FCoEWWNN=%s\r\n",
                                node_p->portconfig.NparPF[0].FCoEWWNN);
                            if (iRet <= 0) {
                                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
                            }
                            _data_string_concat(str_val, size, content);
                        }
                    } else {
                        iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN,
                            "set netport1 NparPF%d=%s;%d;%d;%s;%s;%s;%s;%s\r\n",
                            node_p->portconfig.NparPF[pf_index].pf_id, node_p->portconfig.NparPF[pf_index].Protocol,
                            node_p->portconfig.NparPF[pf_index].MinBandwidth,
                            node_p->portconfig.NparPF[pf_index].MaxBandwidth,
                            node_p->portconfig.NparPF[pf_index].PermanentAddress,
                            node_p->portconfig.NparPF[pf_index].iScsiAddress,
                            node_p->portconfig.NparPF[pf_index].FCoEFipMACAddress,
                            node_p->portconfig.NparPF[pf_index].FCoEWWPN, node_p->portconfig.NparPF[pf_index].FCoEWWNN);
                        if (iRet <= 0) {
                            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
                        }
                        _data_string_concat(str_val, size, content);
                    }

                    (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);

                    if (strlen(node_p->portconfig.NparPF[pf_index].QinqVlanMode)) {
                        iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "set pf%d %s=%s\r\n",
                            node_p->portconfig.NparPF[pf_index].pf_id, CLP_KEY_QINQ_VLAN_MODE,
                            node_p->portconfig.NparPF[pf_index].QinqVlanMode);
                        if (iRet <= 0) {
                            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
                        }
                        _data_string_concat(str_val, size, content);
                    }

                    (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);

                    if (node_p->portconfig.NparPF[pf_index].QinqVlanId != INVALID_INT_VAL) {
                        (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "set pf%d %s=%d\r\n",
                            node_p->portconfig.NparPF[pf_index].pf_id, "QinqVlanId",
                            node_p->portconfig.NparPF[pf_index].QinqVlanId);
                        _data_string_concat(str_val, size, content);
                    }

                    (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);

                    if (node_p->portconfig.NparPF[pf_index].QinqVlanPriority != INVALID_UCHAR) {
                        (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "set pf%d %s=%d\r\n",
                            node_p->portconfig.NparPF[pf_index].pf_id, CLP_KEY_QINQ_VLAN_PRIORITY,
                            node_p->portconfig.NparPF[pf_index].QinqVlanPriority);
                        _data_string_concat(str_val, size, content);
                    }

                    (void)memset_s(vlan_trunk_range, sizeof(vlan_trunk_range), 0, sizeof(vlan_trunk_range));

                    if (strlen(node_p->portconfig.NparPF[pf_index].QinqVlanTrunkRange)) {
                        iRet = sprintf_s(vlan_trunk_range, sizeof(vlan_trunk_range), "set pf%u %s=%s\r\n",
                            node_p->portconfig.NparPF[pf_index].pf_id, CLP_KEY_QINQ_VLAN_TRUNKRANGE,
                            node_p->portconfig.NparPF[pf_index].QinqVlanTrunkRange);
                        if (iRet <= 0) {
                            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
                        }
                        _data_string_concat(vlan_trunk_range, size, content);
                    }
                }
            }

            for (target_index = 0; target_index < TARGET_MAX_NUM; target_index++) {
                (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);

                if (node_p->portconfig.FCoEBootTarget[target_index].target_id != INVALID_UCHAR) {
                    if (node_p->portconfig.FCoEBootTarget[target_index].TargetEnable == TRUE) {
                        (void)_check_bootlun_is_invalid(node_p->portconfig.FCoEBootTarget[target_index], str_val,
                            sizeof(str_val));
                        _data_string_concat(str_val, size, content);
                    } else {
                        (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN,
                            "set netport1 FCoEBootTarget%d=%s;;\r\n",
                            node_p->portconfig.FCoEBootTarget[target_index].target_id, CLP_VAL_DISABLED);
                        _data_string_concat(str_val, size, content);
                    }
                }
            }

            securec_rv = strncat_s(content, size, "exit\r\n", strlen("exit\r\n"));
            if (securec_rv != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
            }
            securec_rv = strncat_s(content, size, "set netport1 default\r\n", strlen("set netport1 default\r\n"));
            if (securec_rv != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
            }
            securec_rv = strncat_s(content, size, CLP_DEFAULT_SHOW, strlen(CLP_DEFAULT_SHOW));
            if (securec_rv != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
            }

            if (node_p->headinfo.portid == 0) {
                securec_rv = strncat_s(content, size, CLP_PORT0_SHOW, strlen(CLP_PORT0_SHOW));
                if (securec_rv != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
                }
            } else if (node_p->headinfo.portid == 1) {
                securec_rv = strncat_s(content, size, CLP_PORT1_SHOW, strlen(CLP_PORT1_SHOW));
                if (securec_rv != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
                }
            }

            securec_rv = strncat_s(content, size, "#Oprom\r\n}\r\n", strlen("#Oprom\r\n}\r\n"));
            if (securec_rv != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
            }
        }
    }

    return RET_OK;
}


gint32 _chip_rules_check(guint16 board_id, GSList *config_list, json_object **error_info)
{
    errno_t safe_fun_ret = EOK;
    GSList *obj_node = NULL;
    CLP_RESPONSE_UNIT *node_p = NULL;
    gchar* MultifunctionMode[2][2] = {{NULL}, {NULL}};
    guchar port_sriov[2][2] = {{0}, {0}};
    gchar* vlan_runk_range[2][8] = {{NULL}, {NULL}};

    gchar* pf_vlan_mode[2][8] = {{NULL}, {NULL}};

    gint32 i = 0;
    gint32 chip_num = 0;
    gint32 pf_index = 0;
    gchar *all_range = NULL;
    gint32 ret = 0;
    guchar pf_id = 0;
    guchar chip_id = 0;
    guchar port_id = 0;

    if (config_list == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.\n", __FUNCTION__);
        return RET_ERR;
    }

    
    for (obj_node = config_list; obj_node; obj_node = obj_node->next) {
        node_p = (CLP_RESPONSE_UNIT *)obj_node->data;

        chip_id = node_p->headinfo.chipid - 1;
        port_id = node_p->headinfo.portid;

        if (chip_id < 2 && port_id < 2) {
            
            
            if (INVALID_UCHAR == node_p->portconfig.SRIOVEnabled) {
                port_sriov[chip_id][port_id] = FALSE;
            } else {
                port_sriov[chip_id][port_id] = node_p->portconfig.SRIOVEnabled;
            }

            

            MultifunctionMode[chip_id][port_id] = node_p->chipconfig.MultifunctionMode;

            for (pf_index = 0; pf_index < 4; pf_index++) {
                pf_id = node_p->portconfig.NparPF[pf_index].pf_id;

                if (pf_id < 8) {
                    vlan_runk_range[chip_id][pf_id] = node_p->portconfig.NparPF[pf_index].QinqVlanTrunkRange;
                    
                    pf_vlan_mode[chip_id][pf_id] = node_p->portconfig.NparPF[pf_index].QinqVlanMode;
                    
                }
            }
        }
    }

    if (board_id == MXEN_ID) {
        chip_num = 1;
    } else if (MXEP_ID == board_id) {
        chip_num = 2;
    }

    for (i = 0; i < chip_num; i++) {
        
        if (port_sriov[i][0] != port_sriov[i][1]) {
            debug_log(DLOG_ERROR, "%s: SRIOVEnableds are different.", __FUNCTION__);
            
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(RFPROP_CLP_SRIOVENABLED));
            return CHIP_RULE_CHECK_FAIL;
            
        }

        
        if (g_ascii_strcasecmp(MultifunctionMode[i][0], MultifunctionMode[i][1]) != 0) {
            debug_log(DLOG_ERROR, "%s: MultifunctionModes are different.", __FUNCTION__);
            
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(RFPROP_CLP_MULTI_MODE));
            return CHIP_RULE_CHECK_FAIL;
            
        }

        
        
        all_range = (gchar *)g_malloc0(MAX_UINT16_VAL);
        if (all_range == NULL) {
            debug_log(DLOG_ERROR, "%s: g_malloc0 failed", __FUNCTION__);
            return RET_ERR;
        }

        for (pf_index = 0; pf_index < 8; pf_index++) {
            if (vlan_runk_range[i][pf_index]) {
                safe_fun_ret = strncat_s(all_range, MAX_UINT16_VAL, vlan_runk_range[i][pf_index],
                    strlen(vlan_runk_range[i][pf_index]));
                if (safe_fun_ret != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                }
                safe_fun_ret = strncat_s(all_range, MAX_UINT16_VAL, ",", strlen(","));
                if (safe_fun_ret != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                }
            }
        }

        ret = _check_pfs_vlan_trunkrange(all_range, FALSE);
        
        g_free(all_range);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:_check_pfs_vlan_trunkrange failed", __FUNCTION__);
            return TRUNK_RANGE_CHECK_FAIL;
        }

        ret = _chip_rules_vlan_mode_check(pf_vlan_mode[i], MultifunctionMode[i][0]);
        if (ret != RET_OK) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(RFPROP_CLP_VLAN_MODE));
            return CHIP_RULE_CHECK_FAIL;
        }
    }

    return RET_OK;
}


LOCAL gint32 _chip_rules_vlan_mode_check(gchar **pf_vlan_mode, gchar *mulfunc_mode)
{
    gint32 pf_index = 0;
    gint32 pf_num = 0;

    
    
    if (g_ascii_strcasecmp(mulfunc_mode, CLP_VAL_SF) == 0) {
        
        pf_num = 2;
    } else {
        
        pf_num = 8;
    }

    for (pf_index = 1; pf_index < pf_num; pf_index++) {
        if (pf_vlan_mode[0] && pf_vlan_mode[pf_index]) {
            if (g_ascii_strcasecmp(pf_vlan_mode[0], pf_vlan_mode[pf_index]) != 0) {
                debug_log(DLOG_ERROR, "%s: pfs QinQVlanMode are different.", __FUNCTION__);
                return RET_ERR;
            }
        }
    }

    
    return RET_OK;
}


void _check_bootlun_is_invalid(FCOE_BOOT_TARGET target_data, gchar *o_str, guint32 o_str_size)
{
    gint32 iRet = -1;
    if (o_str == NULL || o_str_size == 0) {
        return;
    }

    if (target_data.bootlun != INVALID_INT_VAL) {
        iRet = snprintf_s(o_str, o_str_size, o_str_size - 1, "set netport1 FCoEBootTarget%d=%s;%s;%04x\r\n",
            target_data.target_id, CLP_VAL_ENABLED, target_data.FCoEWWPN, target_data.bootlun);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
        }
    } else {
        iRet = snprintf_s(o_str, o_str_size, o_str_size - 1, "set netport1 FCoEBootTarget%d=%s;%s;\r\n",
            target_data.target_id, CLP_VAL_ENABLED, target_data.FCoEWWPN);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
        }
    }
}
