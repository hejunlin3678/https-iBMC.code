

#include "set_clp_220_221.h"
#include "bios.h"

LOCAL gboolean _check_only_append(GSList *config_list, gchar (*append_str)[LINE_MAX_LEN]);
LOCAL gint32 _parse_set_file_content_for_220(GSList *config_list, gchar *content, guint32 size);
LOCAL gint32 _append_str_to_file(const gchar *conf_port, const gchar *first_key, const gchar *second_key,
    const gchar *str);
LOCAL gint32 _check_and_insert_string(gchar *dest_str, guint32 dest_len, const gchar *src_str, const gchar *key_value,
    const gchar *add_position, const gchar *inser_str);
LOCAL gint32 _parse_set_file_content_for_221(GSList *config_list, gchar *content, guint32 size);


gint32 bios_set_json_to_clp_220(guint16 board_id, const gchar *devicename, const gchar *config_data,
    json_object **error_info)
{
    gint32 iRet = -1;
    gint32 ret = 0;
    GSList *config_list = NULL;
    gchar *content = NULL;
    gchar       append_str[4][LINE_MAX_LEN] = {{0}, {0}, {0}, {0}};
    gchar **tmp_arry1 = NULL;
    gchar **tmp_arry2 = NULL;
    gint32 i = 0;

    gchar       config_file_path[MAX_LEN_128] = {0};
    gchar       short_name[MAX_LEN_64] = {0};
    gchar       full_config_file[MAX_LEN_128] = {0};

    CLP_SET_S clp_set = { 0, CLP_220_221 };
    clp_set.board_id = board_id;
    clp_set.clp_flag = CLP_220_221;

    
    if (config_data == NULL || error_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    
    ret = _json_set_and_rules_check(devicename, config_data, error_info, &config_list, clp_set);
    if (ret != RET_OK) {
        g_slist_free_full(config_list, (GDestroyNotify)g_free);
        debug_log(DLOG_ERROR, "%s:_json_set_and_rules_check failed, ret is %d", __FUNCTION__, ret);
        return ret;
    }

    if (_check_only_append(config_list, append_str)) {
        // 拼接路径并读取/opt/pme/conf/bios/ClpConfig0.ini
        get_bios_conf_file_info(0x08, config_file_path, MAX_LEN_128, short_name, MAX_LEN_64);
        iRet = snprintf_s(full_config_file, MAX_LEN_128, MAX_LEN_128 - 1, "%s/%s", config_file_path, short_name);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
        }

        // ClpConfig0.ini 文件不存在则创建默认文件
        if (vos_get_file_accessible(full_config_file) == FALSE) {
            
            if (vos_get_file_accessible(TMP_BIOS_CLPCONFIG_INI_FILE_NAME) == TRUE) {
                (void)vos_rm_filepath(TMP_BIOS_CLPCONFIG_INI_FILE_NAME);
            }

            if (_create_temp_file_and_set_default() != RET_OK) {
                debug_log(DLOG_ERROR, "create %s file fail!\r\n", TMP_BIOS_CLPCONFIG_INI_FILE_NAME);
                g_slist_free_full(config_list, (GDestroyNotify)g_free);
                return RET_ERR;
            }

            
        } else {
            
            ret = vos_file_copy(TMP_BIOS_CLPCONFIG_INI_FILE_NAME, full_config_file);
            if (ret != RET_OK) {
                g_slist_free_full(config_list, (GDestroyNotify)g_free);
                debug_log(DLOG_ERROR, "Copy file failed, ret = %d!\n", ret);
                return RET_ERR;
            }

            (void)chmod(TMP_BIOS_CLPCONFIG_INI_FILE_NAME, 0600);
        }

        
        while (i < 4 && strlen(append_str[i])) {
            tmp_arry1 = g_strsplit_set(append_str[i], ";", 0);
            if (tmp_arry1 == NULL || g_strv_length(tmp_arry1) < 2) {
                g_slist_free_full(config_list, (GDestroyNotify)g_free);
                if (tmp_arry1 != NULL) {
                    g_strfreev(tmp_arry1);
                }
                debug_log(DLOG_ERROR, "%s:g_strsplit_set fail", __FUNCTION__);
                return RET_ERR;
            }
            tmp_arry2 = g_strsplit_set(tmp_arry1[1], "=", 0);
            if (tmp_arry2 == NULL || g_strv_length(tmp_arry1) < 1) {
                g_slist_free_full(config_list, (GDestroyNotify)g_free);
                if (tmp_arry1 != NULL) {
                    g_strfreev(tmp_arry1);
                }
                if (tmp_arry2 != NULL) {
                    g_strfreev(tmp_arry2);
                }
                debug_log(DLOG_ERROR, "%s:g_strsplit_set fail", __FUNCTION__);
                return RET_ERR;
            }

            if (g_strrstr(tmp_arry2[0], CLP_KEY_OEMELX_LINK_CONFIG) != NULL) {
                _append_str_to_file(tmp_arry1[0], tmp_arry2[0], CLP_HEAD_FC_FCOE, tmp_arry1[1]);
            } else if (g_strrstr(tmp_arry2[0], CLP_KEY_OEMELX_SERDES) != NULL) {
                _append_str_to_file(tmp_arry1[0], tmp_arry2[0], CLP_HEAD_CMD, tmp_arry1[1]);
            }

            i++;
            g_strfreev(tmp_arry1);
            g_strfreev(tmp_arry2);
        }

        
        ret = compare_and_copy_clp_config_file_with_lock(TMP_BIOS_CLPCONFIG_INI_FILE_NAME);
        if (ret != RET_OK) {
            g_slist_free_full(config_list, (GDestroyNotify)g_free);
            debug_log(DLOG_ERROR, "Copy file failed, ret = %d!\n", ret);
            return RET_ERR;
        }
    } else {
        
        content = (gchar *)g_malloc0(SMBIOS_DATA_SIZE);
        if (content == NULL) {
            g_slist_free_full(config_list, (GDestroyNotify)g_free);
            debug_log(DLOG_ERROR, "%s: g_malloc0 failed", __FUNCTION__);
            return RET_ERR;
        }
        ret = _parse_set_file_content_for_220(config_list, content, SMBIOS_DATA_SIZE);
        if (ret != RET_OK) {
            g_free(content);
            g_slist_free_full(config_list, (GDestroyNotify)g_free);
            debug_log(DLOG_ERROR, "%s:_parse_set_file_content failed", __FUNCTION__);
            return RET_ERR;
        }

        ret = _write_tmp_file_and_copy(content, strlen(content), devicename);
        
        g_free(content);
        g_slist_free_full(config_list, (GDestroyNotify)g_free);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:_write_tmp_file_for_copy failed", __FUNCTION__);
            return ret;
        }
    }

    return RET_OK;
}


LOCAL gboolean _check_only_append(GSList *config_list, gchar (*append_str)[LINE_MAX_LEN])
{
    gint32 iRet = -1;
    CLP_RESPONSE_UNIT *node_p = NULL;
    GSList *obj_node = NULL;
    gint32 i = 0;
    gint32 target_index = 0;
    gchar *tmp_name = NULL;

    for (obj_node = config_list; obj_node; obj_node = obj_node->next) {
        node_p = (CLP_RESPONSE_UNIT *)obj_node->data;

        tmp_name = g_ascii_strdown(node_p->headinfo.devicename, strlen(node_p->headinfo.devicename));
        if (tmp_name == NULL) {
            debug_log(DLOG_ERROR, "%s: tmp_name is NULL.", __FUNCTION__);
            return FALSE;
        }

        if (strlen(node_p->portconfig.LinkConfig)) {
            iRet = snprintf_s(append_str[i++], LINE_MAX_LEN + 1, LINE_MAX_LEN,
                "%s.%d;set fcport0 OEMELX_LinkConfig=%s\r\n", tmp_name, node_p->headinfo.chipid,
                node_p->portconfig.LinkConfig);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
            }
        }

        if (strlen(node_p->portconfig.SerDesConfig)) {
            iRet =
                snprintf_s(append_str[i++], LINE_MAX_LEN + 1, LINE_MAX_LEN, "%s.%d;set fcport%d OEMELX_hssregs=%s\r\n",
                tmp_name, node_p->headinfo.chipid, node_p->headinfo.portid, node_p->portconfig.SerDesConfig);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
            }
        }

        if (strlen(node_p->portconfig.NparPF[0].FCoEWWNN)) {
            if (tmp_name != NULL) {
                g_free(tmp_name);
            }
            return FALSE;
        }

        if (strlen(node_p->portconfig.NparPF[0].FCoEWWPN)) {
            if (tmp_name != NULL) {
                g_free(tmp_name);
            }
            return FALSE;
        }

        for (target_index = 0; target_index < TARGET_MAX_NUM; target_index++) {
            if (node_p->portconfig.FCoEBootTarget[target_index].target_id != INVALID_UCHAR) {
                if (strlen(node_p->portconfig.FCoEBootTarget[target_index].FCoEWWPN)) {
                    if (tmp_name != NULL) {
                        g_free(tmp_name);
                    }
                    return FALSE;
                }

                if (node_p->portconfig.FCoEBootTarget[target_index].bootlun != INVALID_INT_VAL) {
                    if (tmp_name != NULL) {
                        g_free(tmp_name);
                    }
                    return FALSE;
                }
            }
        }

        if (tmp_name != NULL) {
            g_free(tmp_name);
            tmp_name = NULL;
        }
    }

    return TRUE;
}


LOCAL gint32 _parse_set_file_content_for_220(GSList *config_list, gchar *content, guint32 size)
{
    gint32 iRet = -1;
    errno_t securec_rv;
    GSList *obj_node = NULL;
    CLP_RESPONSE_UNIT *node_p = NULL;
    gchar str_val[LINE_MAX_LEN + 1] = {0};
    gint32 target_index = 0;
    gchar *devicename = NULL;
    gchar *vid_did = MHFB_VID_DID;
    gint32 i_index = 0;

    if (config_list == NULL || content == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.\n", __FUNCTION__);
        return RET_ERR;
    }

    for (i_index = 0; i_index < 2; i_index++) {
        if (i_index == 0) {
            vid_did = MHFA_VID_DID;
        } else {
            vid_did = MHFA_VID_DID_FCOE;
        }

        securec_rv = strncat_s(content, SMBIOS_DATA_SIZE, "{\r\n", strlen("{\r\n"));
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
        }

        for (obj_node = config_list; obj_node; obj_node = obj_node->next) {
            node_p = (CLP_RESPONSE_UNIT *)obj_node->data;

            if (config_list == obj_node) {
                devicename = g_ascii_strdown(node_p->headinfo.devicename, strlen(node_p->headinfo.devicename));
                if (devicename != NULL) {
                    (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
                    iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "#%s.%d\r\n", devicename,
                        node_p->headinfo.chipid);
                    if (iRet <= 0) {
                        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
                    }
                    _data_string_concat(str_val, size, content);

                    (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
                    iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "%s.%d\r\n\r\n", devicename,
                        node_p->headinfo.chipid);
                    if (iRet <= 0) {
                        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
                    }
                    _data_string_concat(str_val, size, content);
                    g_free(devicename);
                }

                securec_rv = strncat_s(content, SMBIOS_DATA_SIZE, "#board_id\r\n", strlen("#board_id\r\n"));
                if (securec_rv != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
                }
                (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
                (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "board_id=%u\r\n\r\n", node_p->board_id);
                _data_string_concat(str_val, SMBIOS_DATA_SIZE, content);

                securec_rv = strncat_s(content, SMBIOS_DATA_SIZE, "#netcard\r\n", strlen("#netcard\r\n"));
                if (securec_rv != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
                }
                (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);

                if (strlen(vid_did)) {
                    iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "netcard=%s\r\n", vid_did);
                    if (iRet <= 0) {
                        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
                    }
                    _data_string_concat(str_val, SMBIOS_DATA_SIZE, content);
                } else {
                    securec_rv = strncat_s(content, SMBIOS_DATA_SIZE, "netcard=\r\n", strlen("netcard=\r\n"));
                    if (securec_rv != EOK) {
                        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
                    }
                }

                securec_rv = strncat_s(content, SMBIOS_DATA_SIZE, "#fc/fcoe=E200\r\n", strlen("#fc/fcoe=E200\r\n"));
                if (securec_rv != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
                }

                if (strlen(node_p->portconfig.LinkConfig)) {
                    (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
                    iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "set fcport0 OEMELX_LinkConfig=%s\r\n",
                        node_p->portconfig.LinkConfig);
                    if (iRet <= 0) {
                        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
                    }
                    _data_string_concat(str_val, SMBIOS_DATA_SIZE, content);
                }

                securec_rv = strncat_s(content, SMBIOS_DATA_SIZE, "#reset\r\n\r\n", strlen("#reset\r\n\r\n"));
                if (securec_rv != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
                }
                securec_rv = strncat_s(content, SMBIOS_DATA_SIZE, "#CMD\r\n", strlen("#CMD\r\n"));
                if (securec_rv != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
                }
            }

            if (strlen(node_p->portconfig.SerDesConfig)) {
                (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
                iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "set fcport%d OEMELX_hssregs=%s\r\n",
                    node_p->headinfo.portid, node_p->portconfig.SerDesConfig);
                if (iRet <= 0) {
                    debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
                }
                _data_string_concat(str_val, SMBIOS_DATA_SIZE, content);
            }

            (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
            (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "set fcport%d %s=%s\r\n",
                (node_p->headinfo.portid), CLP_KEY_OEMELX_BOOT_ENABLED,
                node_p->portconfig.SANBootEnable == TRUE ? CLP_VAL_ENABLED : CLP_VAL_DISABLED);
            _data_string_concat(str_val, size, content);
            (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
            iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "set fcport%d %s=%s\r\n",
                (node_p->headinfo.portid), CLP_KEY_OEMELX_WWNN, node_p->portconfig.NparPF[0].FCoEWWNN);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
            }
            _data_string_concat(str_val, size, content);

            (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
            iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "set fcport%d %s=%s\r\n",
                (node_p->headinfo.portid), CLP_KEY_OEMELX_WWPN, node_p->portconfig.NparPF[0].FCoEWWPN);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
            }
            _data_string_concat(str_val, size, content);

            for (target_index = 0; target_index < TARGET_MAX_NUM; target_index++) {
                if (INVALID_UCHAR != node_p->portconfig.FCoEBootTarget[target_index].target_id) {
                    if (strlen(node_p->portconfig.FCoEBootTarget[target_index].FCoEWWPN)) {
                        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
                        iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "set fcport%d %s[%d]=%s\r\n",
                            (node_p->headinfo.portid), CLP_KEY_OEMELX_TARGET_WWPN,
                            node_p->portconfig.FCoEBootTarget[target_index].target_id - 1,
                            node_p->portconfig.FCoEBootTarget[target_index].FCoEWWPN);
                        if (iRet <= 0) {
                            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
                        }
                        _data_string_concat(str_val, size, content);
                    }

                    if (node_p->portconfig.FCoEBootTarget[target_index].bootlun != INVALID_INT_VAL) {
                        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
                        iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "set fcport%d %s[%d]=%04x\r\n",
                            (node_p->headinfo.portid), CLP_KEY_OEMELX_LUN,
                            node_p->portconfig.FCoEBootTarget[target_index].target_id - 1,
                            node_p->portconfig.FCoEBootTarget[target_index].bootlun);
                        if (iRet <= 0) {
                            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
                        }
                        _data_string_concat(str_val, size, content);
                    }
                }
            }
        }

        securec_rv = strncat_s(content, size, "exit\r\n", strlen("exit\r\n"));
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
        }
        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "show fcport0 OEMELX_BootEnable\r\n");
        securec_rv = strncat_s(content, SMBIOS_DATA_SIZE, str_val, strlen(str_val));
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
        }
        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "show fcport1 OEMELX_BootEnable\r\n");
        securec_rv = strncat_s(content, SMBIOS_DATA_SIZE, str_val, strlen(str_val));
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
        }
        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "show fcport0 OEMELX_VolatileWWNN\r\n");
        securec_rv = strncat_s(content, SMBIOS_DATA_SIZE, str_val, strlen(str_val));
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
        }
        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "show fcport1 OEMELX_VolatileWWNN\r\n");
        securec_rv = strncat_s(content, SMBIOS_DATA_SIZE, str_val, strlen(str_val));
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
        }
        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "show fcport0 OEMELX_VolatileWWPN\r\n");
        securec_rv = strncat_s(content, SMBIOS_DATA_SIZE, str_val, strlen(str_val));
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
        }
        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "show fcport1 OEMELX_VolatileWWPN\r\n");
        securec_rv = strncat_s(content, SMBIOS_DATA_SIZE, str_val, strlen(str_val));
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
        }
        securec_rv = strncat_s(content, SMBIOS_DATA_SIZE, "#Oprom\r\n}\r\n", strlen("#Oprom\r\n}\r\n"));
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
        }
    }

    return RET_OK;
}


LOCAL gint32 _append_str_to_file(const gchar *conf_port, const gchar *first_key, const gchar *second_key,
    const gchar *str)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret = RET_OK;
    gchar *data_buf = NULL;
    gsize file_len = 0;
    gchar *contents = NULL;
    gchar **clp_array = NULL;
    FILE *fp = NULL;
    guint32 i = 0;
    gchar *tmp_pointer = NULL;

    if (first_key == NULL || str == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RET_ERR;
    }

    data_buf = (gchar *)g_malloc0(SMBIOS_DATA_SIZE);
    if (data_buf == NULL) {
        debug_log(DLOG_ERROR, "%s: g_malloc0 failed", __FUNCTION__);
        return RET_ERR;
    }

    
    fp = g_fopen(TMP_BIOS_CLPCONFIG_INI_FILE_NAME, "r+");
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "open %s file fail!\r\n", TMP_BIOS_CLPCONFIG_INI_FILE_NAME);
        ret = RET_ERR;
        goto err_out;
    }
    ret = fseek(fp, 0, SEEK_END);
    if (ret != 0) {
        debug_log(DLOG_ERROR, "%s: fseek SEEK_SET failed!\n", __FUNCTION__);
        ret = RET_ERR;
        (void)fclose(fp);
        goto err_out;
    }

    file_len = ftell(fp);
    (void)fclose(fp);
    fp = NULL;

    
    if (g_file_get_contents(TMP_BIOS_CLPCONFIG_INI_FILE_NAME, &contents, &file_len, NULL) != TRUE) {
        debug_log(DLOG_ERROR, "g_file_get_contents(%s) failed.", TMP_BIOS_CLPCONFIG_INI_FILE_NAME);
        ret = RET_ERR;
        goto err_out;
    }

    
    if (contents == NULL) {
        ret = RET_ERR;
        debug_log(DLOG_ERROR, "%s: Get contents error.", __FUNCTION__);
        goto err_out;
    }

    
    if (strlen(contents) <= 1) {
        
        if (_create_temp_file_and_set_default() != RET_OK) {
            debug_log(DLOG_ERROR, "create %s file fail!\r\n", TMP_BIOS_CLPCONFIG_INI_FILE_NAME);
            ret = RET_ERR;
            goto err_out;
        }

        
        if (contents != NULL) {
            g_free(contents);
            contents = NULL;
        }

        if (g_file_get_contents(TMP_BIOS_CLPCONFIG_INI_FILE_NAME, &contents, &file_len, NULL) != TRUE) {
            debug_log(DLOG_ERROR, "g_file_get_contents(%s) failed.", TMP_BIOS_CLPCONFIG_INI_FILE_NAME);
            ret = RET_ERR;
            goto err_out;
        }
    }

    

    clp_array = g_strsplit_set(contents, "{}", 0);
    if (clp_array == NULL) {
        ret = RET_ERR;
        debug_log(DLOG_ERROR, "%s:g_strsplit_set fail", __FUNCTION__);
        goto err_out;
    }

    for (i = 0; i < g_strv_length(clp_array); i++) {
        
        tmp_pointer = clp_array[i];

        
        while (*tmp_pointer == '\r' || *tmp_pointer == '\n') {
            tmp_pointer++;

            if (*tmp_pointer == '\0') {
                break;
            }
        }

        if (g_strrstr(tmp_pointer, conf_port) != NULL) {
            ret = _check_and_insert_string(data_buf, SMBIOS_DATA_SIZE, tmp_pointer, first_key, second_key, str);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "_check_and_insert_string error.");
                goto err_out;
            }
        } else {
            if (strlen(tmp_pointer)) {
                safe_fun_ret = strncat_s(data_buf, SMBIOS_DATA_SIZE, "{\r\n", strlen("{\r\n"));
                if (safe_fun_ret != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                }
                safe_fun_ret = strncat_s(data_buf, SMBIOS_DATA_SIZE, tmp_pointer, strlen(clp_array[i]));
                if (safe_fun_ret != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                }
                safe_fun_ret = strncat_s(data_buf, SMBIOS_DATA_SIZE, "}\r\n", strlen("}\r\n"));
                if (safe_fun_ret != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                }
            }
        }

        
    }

    
    fp = g_fopen(TMP_BIOS_CLPCONFIG_INI_FILE_NAME, "w+");
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "open %s file fail!\r\n", TMP_BIOS_CLPCONFIG_INI_FILE_NAME);
        ret = RET_ERR;
        goto err_out;
    }

    ret = fseek(fp, 0, SEEK_SET);
    if (ret != 0) {
        debug_log(DLOG_ERROR, "%s: fseek SEEK_SET failed!\n", __FUNCTION__);
        ret = RET_ERR;
        (void)fclose(fp);
        goto err_out;
    }

    ret = fwrite(data_buf, 1, strlen(data_buf), fp);
    if (ret != strlen(data_buf)) {
        debug_log(DLOG_ERROR, "%s: write file error.", __FUNCTION__);
        ret = RET_ERR;
        (void)fclose(fp);
        goto err_out;
    }

    (void)fflush(fp);
    (void)fclose(fp);

err_out:
    if (contents != NULL) {
        g_free(contents);
    }
    if (clp_array != NULL) {
        g_strfreev(clp_array);
    }
    if (data_buf != NULL) {
        g_free(data_buf);
    }
    return ret;
}


LOCAL gint32 _check_and_insert_string(gchar *dest_str, guint32 dest_len, const gchar *src_str, const gchar *key_value,
    const gchar *add_position, const gchar *inser_str)
{
    errno_t safe_fun_ret = EOK;
    gchar *p_location = NULL;
    gint32 offset_len = 0;
    gint32 del_len = 0;
    gint32 reserve_len = 0;
    gint32 str_len = 0;
    guint16 loop_protect = LINE_MAX_LEN;
    gint32 i = 0;

    str_len = strlen(src_str);
    if (dest_len == 0) {
        return RET_ERR;
    }

    
    p_location = g_strrstr(src_str, key_value);
    if (p_location == NULL) {
        if (add_position != NULL) {
            
            p_location = g_strrstr(src_str, add_position);
            if (p_location == NULL) {
                debug_log(DLOG_ERROR, "Not found key(%s or %s).", key_value, add_position);
                return RET_ERR;
            }

            
            for (i = 0; *(p_location + i) != '\n'; i++) {
                if (i >= loop_protect) {
                    break;
                }
            }
            p_location += i + 1;
            del_len = 0;
        } else {
            debug_log(DLOG_ERROR, "Not found key(%s).", key_value);
            return RET_ERR;
        }
    } else {
        
        for (i = 0; *(p_location + i) != '\n'; i++) {
            if (i >= loop_protect) {
                break;
            }
        }
        del_len = i + 1;
    }

    offset_len = p_location - src_str;
    if (str_len > offset_len + del_len) {
        reserve_len = str_len - offset_len - del_len;
    } else {
        reserve_len = 0;
    }

    safe_fun_ret = strncat_s(dest_str, SMBIOS_DATA_SIZE, "{\r\n", strlen("{\r\n"));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    safe_fun_ret = strncat_s(dest_str, SMBIOS_DATA_SIZE, src_str, offset_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    safe_fun_ret = strncat_s(dest_str, SMBIOS_DATA_SIZE, inser_str, strlen(inser_str));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    if (reserve_len != 0) {
        safe_fun_ret = strncat_s(dest_str, SMBIOS_DATA_SIZE, src_str + offset_len + del_len, reserve_len);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
    }

    safe_fun_ret = strncat_s(dest_str, SMBIOS_DATA_SIZE, "}\r\n", strlen("}\r\n"));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 bios_set_json_to_clp_221(guint16 board_id, const gchar *devicename, const gchar *config_data,
    json_object **error_info)
{
    gint32 ret = 0;
    GSList *config_list = NULL;
    gchar *content = NULL;
    CLP_SET_S clp_set = { 0, CLP_220_221 };
    clp_set.board_id = board_id;
    clp_set.clp_flag = CLP_220_221;

    
    if (config_data == NULL || error_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    
    ret = _json_set_and_rules_check(devicename, config_data, error_info, &config_list, clp_set);
    if (ret != RET_OK) {
        if (config_list) {
            g_slist_free_full(config_list, (GDestroyNotify)g_free);
        }
        debug_log(DLOG_ERROR, "%s:_json_set_and_rules_check failed, ret is %d", __FUNCTION__, ret);
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
    ret = _parse_set_file_content_for_221(config_list, content, SMBIOS_DATA_SIZE);
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


LOCAL gint32 _parse_set_file_content_for_221(GSList *config_list, gchar *content, guint32 size)
{
    gint32 ret = -1;
    errno_t securec_rv;
    GSList *obj_node = NULL;
    CLP_RESPONSE_UNIT *node_p = NULL;
    gchar str_val[LINE_MAX_LEN + 1] = {0};
    gint32 target_index = 0;
    gchar *devicename = NULL;
    gchar *vid_did = MHFB_VID_DID;

    if (config_list == NULL || content == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.\n", __FUNCTION__);
        return RET_ERR;
    }

    for (obj_node = config_list; obj_node; obj_node = obj_node->next) {
        node_p = (CLP_RESPONSE_UNIT *)obj_node->data;

        securec_rv = strncat_s(content, size, "{\r\n", strlen("{\r\n"));
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
        }
        devicename = g_ascii_strdown(node_p->headinfo.devicename, strlen(node_p->headinfo.devicename));
        if (devicename != NULL) {
            ret = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "#%s.%d.%d\r\n", devicename,
                node_p->headinfo.chipid, node_p->headinfo.portid);
            if (ret <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret);
            }
            _data_string_concat(str_val, size, content);

            (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
            ret = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "%s.%d.%d\r\n\r\n", devicename,
                node_p->headinfo.chipid, node_p->headinfo.portid);
            if (ret <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret);
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
        ret = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "netcard=%s\r\n", vid_did);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret);
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
        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        (void)snprintf_s(str_val, LINE_MAX_LEN, LINE_MAX_LEN, "set netport%d default\r\n",
            (node_p->headinfo.portid + 1));
        securec_rv = strncat_s(content, size, str_val, strlen(str_val));
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
        }
        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        ret = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "set netport%d %s=%s\r\n",
            (node_p->headinfo.portid + 1), CLP_KEY_OEM_BOOT_ENABLED,
            node_p->portconfig.SANBootEnable == TRUE ? CLP_VAL_ENABLED_SM : CLP_VAL_DISABLED_SM);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret);
        }
        _data_string_concat(str_val, size, content);

        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        ret = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "set netport%d %s[0]=%s\r\n",
            (node_p->headinfo.portid + 1), CLP_KEY_NETWORK_ADDR, node_p->portconfig.NparPF[0].FCoEWWNN);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret);
        }
        _data_string_concat(str_val, size, content);

        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        ret = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "set netport%d %s=%s\r\n",
            (node_p->headinfo.portid + 1), CLP_KEY_ETH_MAC, node_p->portconfig.NparPF[0].FCoEWWPN);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret);
        }
        _data_string_concat(str_val, size, content);

        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);

        for (target_index = 0; target_index < TARGET_MAX_NUM; target_index++) {
            if (INVALID_UCHAR != node_p->portconfig.FCoEBootTarget[target_index].target_id) {
                if (strlen(node_p->portconfig.FCoEBootTarget[target_index].FCoEWWPN)) {
                    (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
                    ret = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "set netport%d %s[%d]=%s\r\n",
                        (node_p->headinfo.portid + 1), CLP_KEY_OEM_WWPN,
                        node_p->portconfig.FCoEBootTarget[target_index].target_id - 1,
                        node_p->portconfig.FCoEBootTarget[target_index].FCoEWWPN);
                    if (ret <= 0) {
                        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret);
                    }
                    _data_string_concat(str_val, size, content);
                }

                if (node_p->portconfig.FCoEBootTarget[target_index].bootlun != INVALID_INT_VAL) {
                    (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
                    (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "set netport%d %s[%d]=%04x\r\n",
                        (node_p->headinfo.portid + 1), CLP_KEY_OEM_LUN,
                        node_p->portconfig.FCoEBootTarget[target_index].target_id - 1,
                        node_p->portconfig.FCoEBootTarget[target_index].bootlun);
                    _data_string_concat(str_val, size, content);
                }
            }
        }

        securec_rv = strncat_s(content, size, "exit\r\n", strlen("exit\r\n"));
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
        }
        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "show netport%d %s\r\n",
            (node_p->headinfo.portid + 1), CLP_KEY_OEM_BOOT_ENABLED);

        securec_rv = strncat_s(content, size, str_val, strlen(str_val));
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
        }
        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "show netport%d %s\r\n",
            (node_p->headinfo.portid + 1), CLP_KEY_NETWORK_ADDR);

        securec_rv = strncat_s(content, size, str_val, strlen(str_val));
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
        }
        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "show netport%d %s\r\n",
            (node_p->headinfo.portid + 1), CLP_KEY_ETH_MAC);

        securec_rv = strncat_s(content, size, str_val, strlen(str_val));
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
        }

        securec_rv = strncat_s(content, size, "#Oprom\r\n}\r\n", strlen("#Oprom\r\n}\r\n"));
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, securec_rv);
        }
    }

    return RET_OK;
}