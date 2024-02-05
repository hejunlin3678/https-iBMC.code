

#include "set_clp_tools.h"

LOCAL gint32 check_discrete_num(guint16 *discrete_num, gboolean check_flag);
LOCAL gint32 _write_default_clp_config(FILE *fp, const gchar *devicename, gint16 port_num, gint16 board_id);
LOCAL gint32 _file_delete_someth(gsize del_len, gsize offset, gsize file_len);
LOCAL void _gen_default_string_for_520(const gchar *devicename, gint16 board_id, gchar *data_string);
LOCAL void _gen_default_string_for_220(const gchar *devicename, gint16 board_id, gchar *data_string);
LOCAL void _gen_default_string_for_221(const gchar *devicename, gint16 board_id, gchar *data_string);
LOCAL void _gen_default_string_for_310_912(const gchar *devicename, gint16 board_id, gchar *data_string);
LOCAL void _gen_default_string_for_312(const gchar *devicename, gint16 board_id, gchar *data_string);
LOCAL void _gen_default_string_for_lom(const gchar *devicename, gint16 port_num, gchar *data_string);
LOCAL void _concat_default_string(gchar *data_string, gchar *tmp_name, DEF_INDEX_ID def_index, gchar *vid_did,
    gint16 board_id);
LOCAL gint32 _string_concat(gchar *str_val, guint32 buf_len, gchar *data_string, const gchar *vid_did,
    DEF_STR_S def_str);
LOCAL void _gen_default_string_for_522(const gchar *devicename, gint16 board_id, gchar *data_string);


gint32 _check_ports_valid(guint16 board_id, json_object *ports_json, json_object **error_info)
{
    gint32 ports_num = 0;
    json_object *obj_json = NULL;
    json_object *object = NULL;
    gint32 portid = 0;
    gint32 i = 0;
    gint32 ret = 0;
    gint32 portid_array[MXEP_PORT_NUM + 1] = {0};
    
    gchar prop_path[MAX_PROPERTY_PATH_LEN + 1] = {0};
    
    guint16 board_id_val = 0;

    
    if (ports_json == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    ports_num = json_object_array_length(ports_json);

    
    
    
    
    (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s", MESSAGE_PROPERTY_PATH_HEAD,
        RFPROP_CLP_PORTS_CONFIG);

    if ((board_id == MXEN_ID || board_id == MHFA_ID || board_id == MHFB_ID || board_id == MXEM_ID ||
        board_id == MXEL_ID)) {
        if (ports_num > MXEN_PORT_NUM) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            debug_log(DLOG_ERROR, "%s: Board_id:%d excessive number of port.", __FUNCTION__, board_id);
            return PORT_PF_TARGET_EXCEED;
        }
        if (ports_num < MXEN_PORT_NUM) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            debug_log(DLOG_ERROR, "%s: Board_id:%d number of ports is insufficient.", __FUNCTION__, board_id);
            return PORT_PF_TARGET_INSUFFICIENT;
        }
    } else if (MXEP_ID == board_id || MXEK_ID == board_id) {
        if (ports_num > MXEP_PORT_NUM) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            debug_log(DLOG_ERROR, "%s: Board_id:%d excessive number of port.", __FUNCTION__, board_id);
            return PORT_PF_TARGET_EXCEED;
        }
        if (ports_num < MXEP_PORT_NUM) {
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            debug_log(DLOG_ERROR, "%s: Board_id:%d number of ports is insufficient.", __FUNCTION__, board_id);
            return PORT_PF_TARGET_INSUFFICIENT;
        }
    }

    

    for (i = 0; i < ports_num; i++) {
        
        (void)memset_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, 0, MAX_PROPERTY_PATH_LEN + 1);
        (void)snprintf_s(prop_path, MAX_PROPERTY_PATH_LEN + 1, MAX_PROPERTY_PATH_LEN, "%s/%s/%d/%s",
            MESSAGE_PROPERTY_PATH_HEAD, RFPROP_CLP_PORTS_CONFIG, i, RFPROP_CLP_PORT_ID);
        

        object = json_object_array_get_idx(ports_json, i);
        ret = json_object_object_get_ex(object, RFPROP_CLP_PORT_ID, &obj_json);
        if (ret) {
            if (json_type_int != json_object_get_type(obj_json)) {
                json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
                json_object_object_add(*error_info, ERROR_VAL,
                    json_object_new_string(dal_json_object_get_str(obj_json)));
                debug_log(DLOG_ERROR, "%s: json_type error", __FUNCTION__);
                return PROPERTY_VALUE_NOT_INLIST;
            }

            portid = json_object_get_int(obj_json);

            // mezz520 ,220,221,310,912Portid范围校验
            
            board_id_val = 0;
            
            if (board_id == MXEN_ID || board_id == MHFA_ID || board_id == MHFB_ID || board_id == MXEM_ID ||
                board_id == MXEL_ID) {
                (board_id_val = board_id);
            }
            if (board_id_val == board_id) {
                if (portid <= 0 || portid > MXEN_PORT_NUM) {
                    json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
                    json_object_object_add(*error_info, ERROR_VAL,
                        json_object_new_string(dal_json_object_get_str(obj_json)));
                    debug_log(DLOG_ERROR, "%s: portid invalid.", __FUNCTION__);
                    return PROPERTY_VALUE_NOT_INLIST;
                }
            }

            // mezz522,312 Portid范围校验
            if (board_id == MXEP_ID || board_id == MXEK_ID) {
                (board_id_val = board_id);
            }
            if (board_id_val == board_id) {
                if (portid <= 0 || portid > MXEP_PORT_NUM) {
                    json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
                    json_object_object_add(*error_info, ERROR_VAL,
                        json_object_new_string(dal_json_object_get_str(obj_json)));
                    debug_log(DLOG_ERROR, "%s: portid invalid.", __FUNCTION__);
                    return PROPERTY_VALUE_NOT_INLIST;
                }
            }

            // portid重复性检查
            if (portid_array[portid] == 1) {
                json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
                json_object_object_add(*error_info, ERROR_VAL,
                    json_object_new_string(dal_json_object_get_str(obj_json)));
                debug_log(DLOG_ERROR, "%s: The value of portid is the same as that of other parameters.", __FUNCTION__);
                return PROPERTY_VALUE_ERROR;
            } else {
                portid_array[portid] = 1;
            }
        }
        
        else {
            debug_log(DLOG_ERROR, "%s: property PortId missing.", __FUNCTION__);
            json_object_object_add(*error_info, ERROR_PROP, json_object_new_string(prop_path));
            return PROPERTY_MISSING;
        }
        
    }

    return RET_OK;
}


void _initial_config_unit(CLP_RESPONSE_UNIT *config_unit_p)
{
    gint32 index_id = 0;
    if (config_unit_p == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return;
    }
    config_unit_p->board_id = INVALID_UINT_VAL;
    config_unit_p->cardinfo.did = INVALID_UINT_VAL;
    config_unit_p->cardinfo.vid = INVALID_UINT_VAL;
    config_unit_p->headinfo.chipid = INVALID_UCHAR;
    config_unit_p->headinfo.portid = INVALID_UCHAR;
    config_unit_p->portconfig.BootEnable = TRUE;
    config_unit_p->portconfig.BootToTarget = FALSE;
    config_unit_p->portconfig.SRIOVEnabled = INVALID_UCHAR;
    config_unit_p->portconfig.VlanId = INVALID_INT_VAL;
    config_unit_p->portconfig.SANBootEnable = FALSE;
    config_unit_p->portconfig.Band_sum = 0;
    config_unit_p->portconfig.FCoE_count = 0;
    config_unit_p->portconfig.iSCSI_count = 0;

    for (index_id = 0; index_id < PORT_PF_MAX; index_id++) {
        config_unit_p->portconfig.NparPF[index_id].pf_id = INVALID_UCHAR;
        config_unit_p->portconfig.NparPF[index_id].MinBandwidth = 0;
        config_unit_p->portconfig.NparPF[index_id].MaxBandwidth = 0;
        config_unit_p->portconfig.NparPF[index_id].QinqVlanId = INVALID_INT_VAL;
        config_unit_p->portconfig.NparPF[index_id].QinqVlanPriority = INVALID_UCHAR;
    }

    for (index_id = 0; index_id < TARGET_MAX_NUM; index_id++) {
        config_unit_p->portconfig.FCoEBootTarget[index_id].target_id = INVALID_UCHAR;
        config_unit_p->portconfig.FCoEBootTarget[index_id].TargetEnable = INVALID_UCHAR;
        config_unit_p->portconfig.FCoEBootTarget[index_id].bootlun = INVALID_INT_VAL;
    }
}


gint32 _set_header_info(guint16 board_id, json_object *port_json, CLP_RESPONSE_UNIT *config_unit_p, gint32 port_index)
{
    gint32 ret = 0;
    gint32 portid = 0;
    json_object *obj_json = NULL;

    
    if (port_json == NULL || config_unit_p == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }
    
    ret = json_object_object_get_ex(port_json, RFPROP_CLP_PORT_ID, &obj_json);
    
    if (ret) {
        portid = json_object_get_int(obj_json);
        
        if (port_index != portid - 1 && board_id != 0) {
            debug_log(DLOG_ERROR, "%s: The port id %d sequence is incorrect.", __FUNCTION__, portid);
            return PORT_ID_SEQUENCE_ERROR;
        }
        config_unit_p->headinfo.chipid = (portid - 1) / 2 + 1;
        config_unit_p->headinfo.portid = (portid - 1) % 2;
    } else {
        config_unit_p->headinfo.chipid = (port_index) / 2 + 1;
        config_unit_p->headinfo.portid = (port_index) % 2;
    }

    

    return RET_OK;
}

LOCAL gint32 check_discrete_num(guint16 *discrete_num, gboolean check_flag)
{
    if (check_flag == TRUE) {
        if (*discrete_num != 1) {
            *discrete_num = 1;
        } else {
            debug_log(DLOG_ERROR, "%s:trunkrange order error. ", __FUNCTION__);
            return TRUNK_RANGE_CHECK_FAIL;
        }
    } else {
        if (*discrete_num != 1) {
            *discrete_num = 1;
        }
    }

    return RET_OK;
}

gint32 _write_tmp_file_and_copy(const void *ptr, size_t size, const gchar *devicename)
{
    gint32 ret_val = RET_ERR;
    size_t rt = 0;
    FILE *fp = NULL;

    if (ptr == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is NULL.", __FUNCTION__);
        return RET_ERR;
    }

    // 查看临时文件是否存在
    if (vos_get_file_accessible(TMP_BIOS_CLPCONFIG_INI_FILE_NAME) == FALSE) {
        // 如果不存在，则创建默认文件
        if (_create_temp_file_and_set_default() != RET_OK) {
            debug_log(DLOG_ERROR, "create %s file fail!\r\n", TMP_BIOS_CLPCONFIG_INI_FILE_NAME);
            return RET_ERR;
        }
    }

    ret_val = _check_current_file(devicename);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "check current file error.");
        return RET_ERR;
    }

    // 以"r+"，模式打开，追加或修改文件
    fp = g_fopen(TMP_BIOS_CLPCONFIG_INI_FILE_NAME, "a+");
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "open %s file fail!\r\n", TMP_BIOS_CLPCONFIG_INI_FILE_NAME);
        return RET_ERR;
    }

    (void)fchmod(fileno(fp), 0600);

    
    rt = fwrite(ptr, size, BIOS_FILE_STREAM_SIZE, fp);
    if (rt != BIOS_FILE_STREAM_SIZE) {
        (void)fclose(fp);
        debug_log(DLOG_ERROR, "write %s with size fail!\n", TMP_BIOS_CLPCONFIG_INI_FILE_NAME);
        return RET_ERR;
    }
    
    (void)fflush(fp);
    (void)fclose(fp);
    fp = NULL;

    ret_val = compare_and_copy_clp_config_file_with_lock(TMP_BIOS_CLPCONFIG_INI_FILE_NAME);

    maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "Save CLP config file from Redfish, ret = %d.\n", ret_val);

    return ret_val;
}


void _data_string_concat(const gchar *str_val, gint32 str_val_len, gchar *data_string)
{
    if (strlen(str_val)) {
        errno_t safe_fun_ret = strncat_s(data_string, str_val_len, str_val, strlen(str_val));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    }
}


gint32 _create_temp_file_and_set_default(void)
{
    FILE *fp = NULL;
    gint32 ret = RET_OK;
    guint8 dev_type = 0;
    gint16 port_num = 0;
    gint16 board_id = 0;
    OBJ_HANDLE component_handle = 0;

    GSList *netcard_list = NULL;
    GSList *netcard_node = NULL;
    gchar       netcard_devicename[MAX_LEN_128] = {0};

    // 获取handle列表
    ret = dfl_get_object_list(CLASS_NETCARD_NAME, &netcard_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get netcard object list failed\n");
        return RET_ERR;
    }

    // 以"a+"模式创建文件，以便添加默认clp命令到文件中
    fp = g_fopen(TMP_BIOS_CLPCONFIG_INI_FILE_NAME, "a+");
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "open %s file fail!\r\n", TMP_BIOS_CLPCONFIG_INI_FILE_NAME);
        g_slist_free(netcard_list);
        return RET_ERR;
    }
    (void)fchmod(fileno(fp), 0600);

    // 遍历网卡对象，匹配Component类中的Device 和 Location
    for (netcard_node = netcard_list; netcard_node; netcard_node = netcard_node->next) {
        ret = dfl_get_referenced_object((OBJ_HANDLE)netcard_node->data, PROPERTY_NETCARD_REF_COMPONENT,
            &component_handle);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "dfl_get_referenced_object fail");
            goto dump;
        }

        (void)dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &dev_type);
        // 只在网卡的部件中查找。包括 板载网卡和MEZZ网卡
        if ((COMPONENT_TYPE_NIC_CARD != dev_type) && (COMPONENT_TYPE_MEZZ != dev_type)) {
            continue;
        }

        ret = dal_get_property_value_string(component_handle, PROPERTY_COMPONENT_DEVICE_NAME, netcard_devicename,
            MAX_LEN_128);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "get devicename fail.");
            goto dump;
        }

        ret = dal_get_property_value_int16(component_handle, PROPERTY_COMPONENT_BOARDID, &board_id);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "get board_id fail.");
            goto dump;
        }

        ret = dal_get_property_value_int16((OBJ_HANDLE)netcard_node->data, PROPERTY_NETCARD_PORTNUM, &port_num);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "get port_num fail");
            goto dump;
        }

        ret = _write_default_clp_config(fp, netcard_devicename, port_num, board_id);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "_write_default_clp_config fail");
            goto dump;
        }
    }

dump:
    g_slist_free(netcard_list);
    (void)fclose(fp);
    fp = NULL;
    return ret == RET_OK ? RET_OK : RET_ERR;
};


gint32 _check_current_file(const gchar *devicename)
{
    gchar       data_buf[LINE_MAX_LEN + 1] = {0};
    gchar data_tmp[LINE_MAX_LEN + 1];
    gchar *f_ret = 0;
    gchar **str_array = NULL;
    gint32 p_start = 0;
    gint32 p_end = 0;
    gint32 data_len = 0;
    glong file_len = 0;
    guchar first_flag = 0;
    gint32 ret = 0;
    FILE *p_file = NULL;

    if (devicename == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RET_ERR;
    }

    p_file = g_fopen(TMP_BIOS_CLPCONFIG_INI_FILE_NAME, "r+");
    if (p_file == NULL) {
        debug_log(DLOG_ERROR, "open %s file fail!\r\n", TMP_BIOS_CLPCONFIG_INI_FILE_NAME);
        return RET_ERR;
    }

    (void)fchmod(fileno(p_file), 0600);

    while (fgets(data_buf, LINE_MAX_LEN, p_file) != NULL) {
        if (g_ascii_strncasecmp(data_buf, "{", strlen("{")) == 0 && !first_flag) {
            // 获取删除的起始位置，需要规避"\r""\n"
            p_start = ftell(p_file) - 3;
        }

        if (g_ascii_strncasecmp(data_buf, "}", strlen("}")) == 0) {
            p_end = ftell(p_file);
        }

        if (g_ascii_strncasecmp(data_buf, CLP_HEAD_MEZZ, strlen(CLP_HEAD_MEZZ)) == 0 ||
            g_ascii_strncasecmp(data_buf, CLP_HEAD_LOM, strlen(CLP_HEAD_LOM)) == 0) {
            (void)memset_s(data_tmp, sizeof(data_tmp), 0, sizeof(data_tmp));
            f_ret = fgets(data_tmp, LINE_MAX_LEN, p_file);
            if (f_ret == NULL) {
                (void)memset_s(data_buf, sizeof(data_buf), 0, sizeof(data_buf));
                if (p_file != NULL) {
                    (void)fclose(p_file);
                    p_file = NULL;
                }
                debug_log(DLOG_ERROR, "%s: fgets fail.", __FUNCTION__);
                return RET_ERR;
            }
            str_array = g_strsplit_set(data_tmp, ".", 0);
            if (str_array == NULL) {
                if (p_file != NULL) {
                    (void)fclose(p_file);
                    p_file = NULL;
                }
                debug_log(DLOG_ERROR, "%s:g_strsplit_set fail", __FUNCTION__);
                return RET_ERR;
            }

            if (!g_ascii_strncasecmp((const gchar *)str_array[0], devicename, strlen(devicename)) && !first_flag) {
                // 查询到相同devicename，则记录此值为1
                first_flag++;
                (void)memset_s(data_buf, sizeof(data_buf), 0, sizeof(data_buf));
                if (str_array != NULL) {
                    g_strfreev(str_array);
                    str_array = NULL;
                }
                continue;
            } else if (g_ascii_strncasecmp((const gchar *)str_array[0], devicename, strlen(devicename)) && first_flag) {
                if (str_array != NULL) {
                    g_strfreev(str_array);
                    str_array = NULL;
                }
                // 删除内容，退出函数
                data_len = p_end - p_start;
                ret = fseek(p_file, 0, SEEK_END);
                if (ret != 0) {
                    debug_log(DLOG_ERROR, "%s: fseek SEEK_END failed!\n", __FUNCTION__);
                    if (p_file != NULL) {
                        (void)fclose(p_file);
                        p_file = NULL;
                    }
                    return RET_ERR;
                }

                file_len = ftell(p_file);
                if (p_file != NULL) {
                    (void)fclose(p_file);
                    p_file = NULL;
                }

                if (file_len < 0) {
                    debug_log(DLOG_ERROR, "%s: ftell fail, errno(%d)", __FUNCTION__, errno);
                    return RET_ERR;
                }

                ret = _file_delete_someth((gsize)data_len, (gsize)p_start, (gsize)file_len);
                if (ret != RET_OK) {
                    debug_log(DLOG_ERROR, "%s: _file_delete_someth fail", __FUNCTION__);
                    return RET_ERR;
                }
                return RET_OK;
            }
        }

        (void)memset_s(data_buf, sizeof(data_buf), 0, sizeof(data_buf));
        if (str_array != NULL) {
            g_strfreev(str_array);
            str_array = NULL;
        }
    }

    if (first_flag) {
        p_end = ftell(p_file);
        data_len = p_end - p_start;
        ret = fseek(p_file, 0, SEEK_END);
        if (ret != 0) {
            debug_log(DLOG_ERROR, "%s: fseek SEEK_END failed!\n", __FUNCTION__);
            if (p_file != NULL) {
                (void)fclose(p_file);
                p_file = NULL;
            }
            return RET_ERR;
        }

        file_len = ftell(p_file);
        if (p_file != NULL) {
            (void)fclose(p_file);
            p_file = NULL;
        }
        if (file_len < 0) {
            debug_log(DLOG_ERROR, "%s: ftell fail, errno(%d)", __FUNCTION__, errno);
            return RET_ERR;
        }

        ret = _file_delete_someth((gsize)data_len, (gsize)p_start, (gsize)file_len);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: _file_delete_someth fail", __FUNCTION__);
            return RET_ERR;
        }
    } else {
        debug_log(DLOG_MASS, "%s: The same device name is not found.", __FUNCTION__);
    }

    if (p_file != NULL) {
        (void)fclose(p_file);
        p_file = NULL;
    }
    return RET_OK;
}


LOCAL gint32 _write_default_clp_config(FILE *fp, const gchar *devicename, gint16 port_num, gint16 board_id)
{
    gchar *default_string = NULL;

    if (fp == NULL || devicename == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL point.", __FUNCTION__);
        return RET_ERR;
    }

    default_string = (gchar *)g_malloc0(SMBIOS_DATA_SIZE);
    if (default_string == NULL) {
        debug_log(DLOG_ERROR, "default_string malloc error.");
        return RET_ERR;
    }

    switch (board_id) {
        case 152: // 522
            _gen_default_string_for_522(devicename, board_id, default_string);
            break;

        case 154: // 520
            _gen_default_string_for_520(devicename, board_id, default_string);
            break;

        case 149: // 220
            _gen_default_string_for_220(devicename, board_id, default_string);
            break;

        case 157: // 221
            _gen_default_string_for_221(devicename, board_id, default_string);
            break;

        case 153:
        case 151: // 310/912
            _gen_default_string_for_310_912(devicename, board_id, default_string);
            break;

        case 150: // 312
            _gen_default_string_for_312(devicename, board_id, default_string);
            break;

        default:
            if (!g_ascii_strncasecmp(devicename, LOM_CARD_DEVICE_NAME, strlen(LOM_CARD_DEVICE_NAME))) {
                _gen_default_string_for_lom(devicename, port_num, default_string);
            } else {
                g_free(default_string);
                return RET_OK;
            }

            break;
    }

    if (fwrite((gchar *)default_string, strlen(default_string), BIOS_FILE_STREAM_SIZE, fp) != BIOS_FILE_STREAM_SIZE) {
        debug_log(DLOG_ERROR, "write %s with size fail!\n", TMP_BIOS_CLPCONFIG_INI_FILE_NAME);
        g_free(default_string);
        return RET_ERR;
    }

    (void)fflush(fp);
    g_free(default_string);
    return RET_OK;
}


LOCAL gint32 _file_delete_someth(gsize del_len, gsize offset, gsize file_len)
{
    gint32 ret;
    gchar *contents = NULL;
    gchar *data_buf = NULL;
    FILE *p_file = NULL;

    if (del_len <= 0 || file_len <= 0) {
        return RET_ERR;
    }

    data_buf = (gchar *)g_malloc0(file_len);
    if (data_buf == NULL) {
        debug_log(DLOG_ERROR, "data_buf malloc error.");
        return RET_ERR;
    }

    if (g_file_get_contents(TMP_BIOS_CLPCONFIG_INI_FILE_NAME, &contents, &file_len, NULL) != TRUE) {
        debug_log(DLOG_ERROR, "g_file_get_contents(%s) failed.", TMP_BIOS_CLPCONFIG_INI_FILE_NAME);
        g_free(data_buf);
        return RET_ERR;
    }
    if (contents == NULL) {
        debug_log(DLOG_ERROR, "contents is null.");
        g_free(data_buf);
        return RET_ERR;
    }
    if (offset + del_len > file_len) {
        del_len = file_len - offset;
    }

    gsize move_len = file_len - offset - del_len;

    errno_t safe_fun_ret = strncat_s(data_buf, file_len, contents, offset);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    safe_fun_ret = strncat_s(data_buf, file_len, contents + offset + del_len, move_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }

    
    p_file = g_fopen(TMP_BIOS_CLPCONFIG_INI_FILE_NAME, "w+");
    if (p_file == NULL) {
        debug_log(DLOG_ERROR, "open %s file fail!\r\n", TMP_BIOS_CLPCONFIG_INI_FILE_NAME);
        ret = RET_ERR;
        goto undo;
    }

    if (fseek(p_file, 0, SEEK_SET) != 0) {
        debug_log(DLOG_ERROR, "%s: fseek SEEK_SET failed!\n", __FUNCTION__);
        ret = RET_ERR;
        goto undo;
    }

    ret = fwrite(data_buf, 1, strlen(data_buf), p_file);
    if (ret != (file_len - del_len)) {
        debug_log(DLOG_ERROR, "fwrite %s failed.", TMP_BIOS_CLPCONFIG_INI_FILE_NAME);
        ret = RET_ERR;
        goto undo;
    }

    (void)fflush(p_file);
    ret = RET_OK;
undo:
    if (contents != NULL) {
        g_free(contents);
    }
    if (data_buf != NULL) {
        g_free(data_buf);
    }
    if (p_file != NULL) {
        (void)fclose(p_file);
        p_file = NULL;
    }
    return ret;
}


LOCAL void _gen_default_string_for_522(const gchar *devicename, gint16 board_id, gchar *data_string)
{
    gchar *vid_did = NULL;
    gchar *tmp_name = NULL;
    DEF_INDEX_ID def_index = { 0, 0, 0 };

    tmp_name = g_ascii_strdown(devicename, strlen(devicename));
    if (tmp_name == NULL) {
        debug_log(DLOG_ERROR, "%s: tmp_name is NULL.", __FUNCTION__);
        return;
    }

    for (def_index.i_index = 0; def_index.i_index < 2; def_index.i_index++) {
        if (def_index.i_index == 0) {
            vid_did = MXEP_VID_DID;
        } else {
            vid_did = MXEP_VID_DID_NPAR;
        }

        for (def_index.chip_index_id = 1; def_index.chip_index_id <= 2; def_index.chip_index_id++) {
            for (def_index.port_index_id = 0; def_index.port_index_id < 2; def_index.port_index_id++) {
                _concat_default_string(data_string, tmp_name, def_index, vid_did, board_id);
            }
        }
    }

    g_free(tmp_name);
}


LOCAL void _concat_default_string(gchar *data_string, gchar *tmp_name, DEF_INDEX_ID def_index, gchar *vid_did,
    gint16 board_id)
{
    gint32 ret;
    gchar str_val[LINE_MAX_LEN + 1] = {0};
    DEF_STR_S def_str = { 0, 0 };
    def_str.board_id = board_id;

    errno_t safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "{\r\n", strlen("{\r\n"));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    ret = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "#%s.%d.%d\r\n", tmp_name, def_index.chip_index_id,
        def_index.port_index_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret);
    }
    _data_string_concat(str_val, SMBIOS_DATA_SIZE, data_string);

    (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
    ret = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "%s.%d.%d\r\n\r\n", tmp_name, def_index.chip_index_id,
        def_index.port_index_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    def_str.iRet = ret;
    _string_concat(str_val, sizeof(str_val), data_string, vid_did, def_str);
    safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, CLP_DEFAULT_SHOW, strlen(CLP_DEFAULT_SHOW));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }

    if (def_index.port_index_id == 0) {
        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, CLP_PORT0_SHOW, strlen(CLP_PORT0_SHOW));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    } else if (def_index.port_index_id == 1) {
        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, CLP_PORT1_SHOW, strlen(CLP_PORT1_SHOW));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    }

    safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#Oprom\r\n}\r\n", strlen("#Oprom\r\n}\r\n"));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
}

LOCAL void _gen_default_string_for_520(const gchar *devicename, gint16 board_id, gchar *data_string)
{
    gint32 ret = -1;
    errno_t safe_fun_ret = EOK;
    guint8 port_index_id = 0;
    guint8 i_index = 0;
    gchar *vid_did = NULL;
    gchar       str_val[LINE_MAX_LEN + 1] = {0};
    gchar *tmp_name = NULL;

    tmp_name = g_ascii_strdown(devicename, strlen(devicename));
    if (tmp_name == NULL) {
        debug_log(DLOG_ERROR, "%s: tmp_name is NULL.", __FUNCTION__);
        return;
    }

    for (i_index = 0; i_index < 2; i_index++) {
        if (i_index == 0) {
            vid_did = MXEP_VID_DID;
        } else {
            vid_did = MXEP_VID_DID_NPAR;
        }

        for (port_index_id = 0; port_index_id < 2; port_index_id++) {
            safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "{\r\n", strlen("{\r\n"));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            ret = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "#%s.1.%u\r\n", tmp_name, port_index_id);
            if (ret <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret);
            }
            _data_string_concat(str_val, SMBIOS_DATA_SIZE, data_string);

            (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
            ret = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "%s.1.%u\r\n\r\n", tmp_name, port_index_id);
            if (ret <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret);
            }
            _data_string_concat(str_val, SMBIOS_DATA_SIZE, data_string);

            safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#board_id\r\n", strlen("#board_id\r\n"));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
            (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "board_id=%d\r\n\r\n", board_id);
            _data_string_concat(str_val, SMBIOS_DATA_SIZE, data_string);

            safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#netcard\r\n", strlen("#netcard\r\n"));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
            ret = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "netcard=%s\r\n", vid_did);
            if (ret <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret);
            }
            _data_string_concat(str_val, SMBIOS_DATA_SIZE, data_string);

            safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#reset\r\n\r\n", strlen("#reset\r\n\r\n"));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#CMD\r\n", strlen("#CMD\r\n"));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }

            safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, CLP_DEFAULT_SHOW, strlen(CLP_DEFAULT_SHOW));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }

            if (port_index_id == 0) {
                safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, CLP_PORT0_SHOW, strlen(CLP_PORT0_SHOW));
                if (safe_fun_ret != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                }
            } else if (port_index_id == 1) {
                safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, CLP_PORT1_SHOW, strlen(CLP_PORT1_SHOW));
                if (safe_fun_ret != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                }
            }

            safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#Oprom\r\n}\r\n", strlen("#Oprom\r\n}\r\n"));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
        }
    }

    g_free(tmp_name);
}


LOCAL void _gen_default_string_for_220(const gchar *devicename, gint16 board_id, gchar *data_string)
{
    gint32 iRet = -1;
    errno_t safe_fun_ret = EOK;
    gchar *vid_did = NULL;
    gchar       str_val[LINE_MAX_LEN + 1] = {0};
    gchar *tmp_name = NULL;
    guint8 i_index = 0;

    tmp_name = g_ascii_strdown(devicename, strlen(devicename));
    if (tmp_name == NULL) {
        debug_log(DLOG_ERROR, "%s: tmp_name is NULL.", __FUNCTION__);
        return;
    }

    for (i_index = 0; i_index < 2; i_index++) {
        if (i_index == 0) {
            vid_did = MHFA_VID_DID;
        } else {
            vid_did = MHFA_VID_DID_FCOE;
        }

        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "{\r\n", strlen("{\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "#%s.1\r\n", tmp_name);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
        }
        _data_string_concat(str_val, SMBIOS_DATA_SIZE, data_string);

        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "%s.1\r\n\r\n", tmp_name);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
        }
        _data_string_concat(str_val, SMBIOS_DATA_SIZE, data_string);

        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#board_id\r\n", strlen("#board_id\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "board_id=%d\r\n\r\n", board_id);
        _data_string_concat(str_val, SMBIOS_DATA_SIZE, data_string);

        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#netcard\r\n", strlen("#netcard\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);

        if (strlen(vid_did)) {
            iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "netcard=%s\r\n", vid_did);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
            }
            _data_string_concat(str_val, SMBIOS_DATA_SIZE, data_string);
        } else {
            safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "netcard=\r\n", strlen("netcard=\r\n"));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
        }

        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#fc/fcoe=E200\r\n", strlen("#fc/fcoe=E200\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#reset\r\n\r\n", strlen("#reset\r\n\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#CMD\r\n", strlen("#CMD\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "show fcport0 OEMELX_BootEnable\r\n");
        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, str_val, strlen(str_val));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "show fcport1 OEMELX_BootEnable\r\n");
        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, str_val, strlen(str_val));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "show fcport0 OEMELX_VolatileWWNN\r\n");
        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, str_val, strlen(str_val));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "show fcport1 OEMELX_VolatileWWNN\r\n");
        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, str_val, strlen(str_val));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "show fcport0 OEMELX_VolatileWWPN\r\n");
        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, str_val, strlen(str_val));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "show fcport1 OEMELX_VolatileWWPN\r\n");
        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, str_val, strlen(str_val));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#Oprom\r\n}\r\n", strlen("#Oprom\r\n}\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    }

    g_free(tmp_name);
}


LOCAL void _gen_default_string_for_221(const gchar *devicename, gint16 board_id, gchar *data_string)
{
    gint32 ret;
    gchar *vid_did = MHFB_VID_DID;
    gchar       str_val[LINE_MAX_LEN + 1] = {0};
    gchar *tmp_name = NULL;
    DEF_STR_S def_str = { 0, 0 };
    def_str.board_id = board_id;

    tmp_name = g_ascii_strdown(devicename, strlen(devicename));
    if (tmp_name == NULL) {
        debug_log(DLOG_ERROR, "%s: tmp_name is NULL.", __FUNCTION__);
        return;
    }

    for (guint8 port_index_id = 0; port_index_id < 2; port_index_id++) {
        errno_t safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "{\r\n", strlen("{\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        ret = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "#%s.1.%u\r\n", tmp_name, port_index_id);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret);
        }
        _data_string_concat(str_val, SMBIOS_DATA_SIZE, data_string);

        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        ret = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "%s.1.%u\r\n\r\n", tmp_name, port_index_id);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
        def_str.iRet = ret;
        _string_concat(str_val, sizeof(str_val), data_string, vid_did, def_str);
        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "show netport%d OEM_BootEnable\r\n",
            port_index_id + 1);
        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, str_val, strlen(str_val));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "show netport%d NetworkAddresses[0]\r\n",
            port_index_id + 1);
        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, str_val, strlen(str_val));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        (void)snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "show netport%d PermanentAddress\r\n",
            port_index_id + 1);
        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, str_val, strlen(str_val));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#Oprom\r\n}\r\n", strlen("#Oprom\r\n}\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    }

    g_free(tmp_name);
}


LOCAL void _gen_default_string_for_310_912(const gchar *devicename, gint16 board_id, gchar *data_string)
{
    guint8 port_index_id = 0;
    gchar *vid_did = NULL;
    gchar       str_value[LINE_MAX_LEN + 1] = {0};
    gchar *device_name = NULL;
    gint32 i_ret;
    device_name = g_ascii_strdown(devicename, strlen(devicename));
    if (device_name == NULL) {
        debug_log(DLOG_ERROR, "%s: tmp_name is NULL.", __FUNCTION__);
        return;
    }
    vid_did = MXEK_VID_DID;
    for (port_index_id = 0; port_index_id < 2; port_index_id++) {
        // begin {

        errno_t safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "{\r\n", strlen("{\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }

        // 0
        i_ret = snprintf_s(str_value, LINE_MAX_LEN + 1, LINE_MAX_LEN, "#%s.1.%u\r\n", device_name, port_index_id);
        if (i_ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, i_ret);
        }
        _data_string_concat(str_value, SMBIOS_DATA_SIZE, data_string);

        // 1
        (void)memset_s(str_value, sizeof(str_value), 0, sizeof(str_value));
        i_ret = snprintf_s(str_value, LINE_MAX_LEN + 1, LINE_MAX_LEN, "%s.1.%u\r\n\r\n", device_name, port_index_id);
        if (i_ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, i_ret);
        }
        _data_string_concat(str_value, SMBIOS_DATA_SIZE, data_string);
        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#board_id\r\n", strlen("#board_id\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }

        // 2
        (void)memset_s(str_value, sizeof(str_value), 0, sizeof(str_value));
        (void)snprintf_s(str_value, LINE_MAX_LEN + 1, LINE_MAX_LEN, "board_id=%d\r\n\r\n", board_id);
        _data_string_concat(str_value, SMBIOS_DATA_SIZE, data_string);

        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#netcard\r\n", strlen("#netcard\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }

        // 3
        (void)memset_s(str_value, sizeof(str_value), 0, sizeof(str_value));
        i_ret = snprintf_s(str_value, LINE_MAX_LEN + 1, LINE_MAX_LEN, "netcard=%s\r\n", vid_did);
        if (i_ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, i_ret);
        }
        _data_string_concat(str_value, SMBIOS_DATA_SIZE, data_string);

        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#reset\r\n\r\n", strlen("#reset\r\n\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }

        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#CMD\r\n\r\n", strlen("#CMD\r\n\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }

        // 4
        (void)memset_s(str_value, sizeof(str_value), 0, sizeof(str_value));
        (void)snprintf_s(str_value, LINE_MAX_LEN + 1, LINE_MAX_LEN, "show netport1 PermanentAddress\r\n");

        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, str_value, strlen(str_value));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }

        // end }

        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#Oprom\r\n}\r\n", strlen("#Oprom\r\n}\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    }

    g_free(device_name);
}


LOCAL void _gen_default_string_for_312(const gchar *devicename, gint16 board_id, gchar *data_string)
{
    gint32 iRet; // 安全函数返回值检查
    guint8 port_index_id = 0;
    gchar *vid_did = NULL;
    gchar       prop_val[LINE_MAX_LEN + 1] = {0}; // 属性值缓存区，以期写入data_string
    gchar *tmp_name = NULL;
    gint16 chip_id = 0;

    tmp_name = g_ascii_strdown(devicename, strlen(devicename));
    if (tmp_name == NULL) {
        debug_log(DLOG_ERROR, "%s: tmp_name is NULL.", __FUNCTION__);
        return;
    }

    vid_did = MXEK_VID_DID;
    for (chip_id = 1; chip_id <= 2; chip_id++) {
        for (port_index_id = 0; port_index_id < 2; port_index_id++) {
            // 起始字符串
            errno_t safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "{\r\n", strlen("{\r\n"));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }

            iRet = snprintf_s(prop_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "#%s.%d.%u\r\n\r\n", tmp_name, chip_id,
                port_index_id);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
            }
            _data_string_concat(prop_val, SMBIOS_DATA_SIZE, data_string);

            // 1
            (void)memset_s(prop_val, sizeof(prop_val), 0, sizeof(prop_val));

            iRet = snprintf_s(prop_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "%s.%d.%u\r\n\r\n", tmp_name, chip_id,
                port_index_id);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
            }
            _data_string_concat(prop_val, SMBIOS_DATA_SIZE, data_string);
            safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "{\r\n", strlen("{\r\n"));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }

            // 2 board_id
            (void)memset_s(prop_val, sizeof(prop_val), 0, sizeof(prop_val));
            (void)snprintf_s(prop_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "board_id=%d\r\n\r\n", board_id);
            _data_string_concat(prop_val, SMBIOS_DATA_SIZE, data_string);

            safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#netcard\r\n", strlen("#netcard\r\n"));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }

            // 3
            (void)memset_s(prop_val, sizeof(prop_val), 0, sizeof(prop_val));
            iRet = snprintf_s(prop_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "netcard=%s\r\n", vid_did);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
            }
            _data_string_concat(prop_val, SMBIOS_DATA_SIZE, data_string);

            safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#reset\r\n\r\n", strlen("#reset\r\n\r\n"));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }

            safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#CMD\r\n\r\n", strlen("#CMD\r\n\r\n"));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }

            // 4
            (void)memset_s(prop_val, sizeof(prop_val), 0, sizeof(prop_val));
            (void)snprintf_s(prop_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "show netport1 PermanentAddress\r\n");

            safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, prop_val, strlen(prop_val));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }

            // 结束字符串

            safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#Oprom\r\n}\r\n", strlen("#Oprom\r\n}\r\n"));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
        }
    }
    g_free(tmp_name);
}


LOCAL void _gen_default_string_for_lom(const gchar *devicename, gint16 port_num, gchar *data_string)
{
    gint32 iRet = -1;
    errno_t safe_fun_ret = EOK;
    guint16 port_index_id = 0;
    gchar *vid_did = NULL;
    gchar       str_val[LINE_MAX_LEN + 1] = {0};
    gchar *tmp_name = NULL;
    gint16 chip_id = 0;

    tmp_name = g_ascii_strdown(devicename, strlen(devicename));
    if (tmp_name == NULL) {
        debug_log(DLOG_ERROR, "%s: tmp_name is NULL.", __FUNCTION__);
        return;
    }

    vid_did = "8086:37ce";

    for (port_index_id = 1; port_index_id <= port_num; port_index_id++) {
        chip_id = (port_index_id - 1) / 2 + 1;
        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "{\r\n", strlen("{\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "#%s%d.%u\r\n", tmp_name, chip_id, port_index_id);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
        }
        _data_string_concat(str_val, SMBIOS_DATA_SIZE, data_string);

        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "%s%d.%u\r\n\r\n", tmp_name, chip_id, port_index_id);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
        }
        _data_string_concat(str_val, SMBIOS_DATA_SIZE, data_string);

        safe_fun_ret =
            strncat_s(data_string, SMBIOS_DATA_SIZE, "board_id=X722\r\n\r\n", strlen("board_id=X722\r\n\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }

        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#netcard\r\n", strlen("#netcard\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        (void)memset_s(str_val, LINE_MAX_LEN + 1, 0, LINE_MAX_LEN + 1);
        iRet = snprintf_s(str_val, LINE_MAX_LEN + 1, LINE_MAX_LEN, "netcard=%s\r\n", vid_did);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
        }
        _data_string_concat(str_val, SMBIOS_DATA_SIZE, data_string);

        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#reset\r\n\r\n", strlen("#reset\r\n\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#CMD\r\n\r\n", strlen("#CMD\r\n\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }

        safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#Oprom\r\n}\r\n", strlen("#Oprom\r\n}\r\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    }

    g_free(tmp_name);
}


LOCAL gint32 _string_concat(gchar *str_val, guint32 buf_len, gchar *data_string, const gchar *vid_did,
    DEF_STR_S def_str)
{
    errno_t safe_fun_ret;
    if (str_val == NULL || data_string == NULL || vid_did == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RET_ERR;
    }

    if (buf_len <= 1) {
        debug_log(DLOG_ERROR, "%s: write overflow.", __FUNCTION__);
        return RET_ERR;
    }

    if (def_str.iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, def_str.iRet);
    }
    _data_string_concat(str_val, SMBIOS_DATA_SIZE, data_string);

    safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#board_id\r\n", strlen("#board_id\r\n"));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    (void)memset_s(str_val, buf_len, 0, buf_len);
    (void)snprintf_s(str_val, buf_len, buf_len - 1, "board_id=%d\r\n\r\n", def_str.board_id);
    _data_string_concat(str_val, SMBIOS_DATA_SIZE, data_string);
    safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#netcard\r\n", strlen("#netcard\r\n"));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    (void)memset_s(str_val, buf_len, 0, buf_len);

    def_str.iRet = snprintf_s(str_val, buf_len, buf_len - 1, "netcard=%s\r\n", vid_did);
    if (def_str.iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, def_str.iRet);
    }
    _data_string_concat(str_val, SMBIOS_DATA_SIZE, data_string);

    safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#reset\r\n\r\n", strlen("#reset\r\n\r\n"));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    safe_fun_ret = strncat_s(data_string, SMBIOS_DATA_SIZE, "#CMD\r\n", strlen("#CMD\r\n"));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    return RET_OK;
}


gint32 _check_pfs_vlan_trunkrange(const gchar *data_buf, gboolean check_flag)
{
    gint32 ret = RET_OK;
    gchar **range_arry = NULL;
    gchar **sub_range_arry = NULL;
    guint16 id_index = 0;
    guint16 n_index = 0;
    guint16 continuous_num[4096] = {0};
    guint16 discrete_num[4096] = {0};
    gint32 i = 0;
    gint32 idx = 0;
    gint32 range_number = 0;

    if (data_buf == NULL || strlen(data_buf) == 0) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RET_ERR;
    }

    range_arry = g_strsplit_set(data_buf, ",", 0);
    if (range_arry == NULL) {
        debug_log(DLOG_ERROR, "%s:g_strsplit_set fail", __FUNCTION__);
        return RET_ERR;
    }

    for (id_index = 0; range_arry[id_index] != NULL; id_index++) {
        if (g_strrstr(range_arry[id_index], "-")) {
            sub_range_arry = g_strsplit_set(range_arry[id_index], "-", 0);
            if (sub_range_arry == NULL || g_strv_length(sub_range_arry) != 2) {
                if (sub_range_arry != NULL) {
                    g_strfreev(sub_range_arry);
                }
                g_strfreev(range_arry);
                debug_log(DLOG_ERROR, "%s:g_strsplit_set fail", __FUNCTION__);
                return RET_ERR;
            }

            ret = vos_str2int(sub_range_arry[0], 10, &continuous_num[n_index++], NUM_TPYE_USHORT);
            if (ret != RET_OK) {
                g_strfreev(sub_range_arry);
                g_strfreev(range_arry);
                debug_log(DLOG_ERROR, "%s:vos_str2int , ret = %d", __FUNCTION__, ret);
                return RET_ERR;
            }

            ret = vos_str2int(sub_range_arry[1], 10, &continuous_num[n_index++], NUM_TPYE_USHORT);
            if (ret != RET_OK) {
                g_strfreev(sub_range_arry);
                g_strfreev(range_arry);
                debug_log(DLOG_ERROR, "%s:vos_str2int , ret = %d", __FUNCTION__, ret);
                return RET_ERR;
            }
            n_index++; // 此处+1保证连续值组之间用0隔开,保证后面进行连续重复性检查
            if (sub_range_arry != NULL) {
                g_strfreev(sub_range_arry);
                sub_range_arry = NULL;
            }
        } else {
            ret = vos_str2int(range_arry[id_index], 10, &idx, NUM_TPYE_USHORT);
            if (ret != RET_OK || idx > 4095) { // discrete_num数组最大下标是4095
                g_strfreev(range_arry);
                debug_log(DLOG_ERROR, "%s:vos_str2int , ret = %d or  idx(=%d) is out of buff", __FUNCTION__, ret, idx);
                return RET_ERR;
            }
            // 避免空字符串转换为0后trunkrange规则校验中一个chip中总数少于256
            if (idx != 0) {
                (discrete_num[idx] = 1);
            }
        }
    }

    g_strfreev(range_arry);

    

    id_index = 0;

    while (id_index < 4095) {
        if (continuous_num[id_index] == 0) {
            id_index++;
            continue;
        }

        
        for (i = continuous_num[id_index]; i <= continuous_num[id_index + 1]; i++) {
            if (i < 4095) { // discrete_num数组最大下标是4095
                ret = check_discrete_num(&discrete_num[i], check_flag);
                if (ret != RET_OK) {
                    return ret;
                }
            }
        }

        id_index++;
    }

    
    idx = 0;

    while (idx < 4096) {
        if (discrete_num[idx++] == 1) {
            range_number++;
        }
    }

    if (range_number > 256) {
        debug_log(DLOG_ERROR, "%s: trunk range error", __FUNCTION__);
        return TRUNK_RANGE_CHECK_FAIL;
    }

    return RET_OK;
}