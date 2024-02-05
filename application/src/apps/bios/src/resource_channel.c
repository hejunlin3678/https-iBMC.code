
#include "pme/pme.h"
#include "bios_ipmi.h"
#include "bios.h"
#include "resource_channel.h"

LOCAL guint8 g_upload_flag = COMMAND_COMPLETE;
LOCAL guint8 g_download_flag = COMMAND_COMPLETE;
LOCAL TASKID g_UploadFile;
LOCAL TASKID g_DownloadFile;
LOCAL gchar g_debug_info_filename[DEBUG_INFO_FILE_NAME_LEN] = {0};
LOCAL gchar g_debug_info_filepath[DEBUG_INFO_FILE_NAME_LEN] = {0};

LOCAL pthread_mutex_t g_tftp_download_mutex = PTHREAD_MUTEX_INITIALIZER;
LOCAL pthread_mutex_t g_tftp_upload_mutex = PTHREAD_MUTEX_INITIALIZER;

LOCAL guint32 g_manufacture_id_resource = 0;

LOCAL STRUCT_CONFIG_FILE_INFO g_config_file_info_array[RESOURCE_NUM];


LOCAL gint32 clear_tmp_file(void);
LOCAL guint8 get_status_by_resource_id(guint8 resource_id);




#define PER_K_CLP_FILE_BT_FLAG "CLPFlagBT"
GMutex g_clpconfig_mutex = { 0 };
guint8 g_is_clpconfig_from_bt = 0;

#define RESOURCE_MSG_HEAD_LEN 5
#define RESOURCE_MSG_STATUS_LEN 6
#define RESOURCE_MSG_SHA256_LEN 7
#define RESOURCE_MSG_NORMAL_LEN 8


void get_bios_conf_file_info(guint8 bios_type, gchar *file_path, guint32 file_path_len, gchar *file_name,
    guint32 file_name_len)
{
    errno_t safe_fun_ret = EOK;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret_val = 0;
    gsize string_len = 0;
    gsize path_len = 0;
    GVariant *property_data = NULL;
    const gchar *path_pos = NULL;
    gchar *name_pos = NULL;
    gchar *temp = NULL;
    gchar path_file[MAX_FILE_PATH_LEN] = {0};

    if (bios_type > RESOURCE_BIOS_PROFILE_RESULT) {
        debug_log(DLOG_ERROR, "bios_type:%u is invalid!\r\n", bios_type);
        return;
    }

    ret_val = dfl_get_object_handle(BIOS_OBJECT_NAME, &obj_handle);
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return;
    }

    ret_val = dfl_get_property_value(obj_handle, g_file_property_name[bios_type], &property_data);
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return;
    }

    
    path_pos = g_variant_get_string(property_data, &string_len);
    if ((string_len <= 0) || (string_len >= MAX_FILE_PATH_LEN)) {
        g_variant_unref(property_data);
        debug_log(DLOG_ERROR, "FileNameLen:%" G_GSIZE_FORMAT " is err!\n", string_len);
        return;
    }

    
    
    safe_fun_ret = strncpy_s(path_file, sizeof(path_file), path_pos, string_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    

    temp = strrchr(path_file, '/');
    if (temp == NULL) {
        g_variant_unref(property_data);
        debug_log(DLOG_ERROR, "strrchr fail!\r\n");
        return;
    }

    path_len = (gsize)((uintptr_t)temp - (uintptr_t)path_file) + 1;

    if (path_len >= string_len) {
        g_variant_unref(property_data);
        debug_log(DLOG_ERROR, "get path_len:%" G_GSIZE_FORMAT "(Max:%" G_GSIZE_FORMAT ") invalid!\r\n", path_len,
            string_len);
        return;
    }

    if (file_path != NULL) {
        
        safe_fun_ret = strncpy_s(file_path, file_path_len, path_pos, path_len);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        
    }

    if (string_len - path_len > RES_MAX_FILE_NAME_LEN) {
        g_variant_unref(property_data);
        debug_log(DLOG_ERROR, "get file_name_len:%" G_GSIZE_FORMAT "(Max:%d) invalid!\r\n", string_len - path_len,
            RES_MAX_FILE_NAME_LEN);
        return;
    }

    name_pos = temp + 1;

    if (file_name != NULL) {
        
        safe_fun_ret = strncpy_s(file_name, file_name_len, name_pos, string_len - path_len);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        
    }

    g_variant_unref(property_data);

    return;
}


LOCAL gint32 get_master_smm_net(gchar *value, guint32 value_len, gsize *len)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret_val = 0;
    OBJ_HANDLE obj_handle = 0;
    gsize string_len = 0;
    const gchar *string_addr = NULL;
    GSList *obj_list = NULL;
    GSList *node = NULL;
    GVariant *property_data = NULL;
    guint8 net_type = 0;

    if (value == NULL || value_len == 0 || len == NULL) {
        return RET_ERR;
    }

    ret_val = dfl_get_object_list(ETHGROUP_CLASS1_NAME, &obj_list);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return RET_ERR;
    }

    
    for (node = obj_list; node; node = node->next) {
        obj_handle = (OBJ_HANDLE)node->data;

        
        ret_val = dfl_get_property_value(obj_handle, PROPERTY_ETHGROUP1_OUTTYPE, &property_data);
        if (ret_val != DFL_OK) {
            g_slist_free(obj_list);
            
            debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
            return ret_val;
        }

        net_type = g_variant_get_byte(property_data);
        g_variant_unref(property_data);

        if (net_type == SMM_INNER_ETHERNET) {
            
            ret_val = dfl_get_property_value(obj_handle, PROPERTY_ETHGROUP1_INNERNET, &property_data);
            if (ret_val != DFL_OK) {
                g_slist_free(obj_list);
                
                debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
                return ret_val;
            }

            string_addr = g_variant_get_string(property_data, &string_len);
            if ((string_addr == NULL) || (string_len == 0) || (string_len >= MAX_FILE_PATH_LEN)) {
                g_variant_unref(property_data);
                g_slist_free(obj_list);
                debug_log(DLOG_ERROR, "smm ip is null or len:%" G_GSIZE_FORMAT "(Max:%d) is err!\n", string_len,
                    MAX_FILE_PATH_LEN);
                return RET_ERR;
            }

            
            safe_fun_ret = strncpy_s(value, value_len, string_addr, string_len);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            
            g_variant_unref(property_data);

            *len = string_len;

            g_slist_free(obj_list);

            return RET_OK;
        }
    }

    debug_log(DLOG_ERROR, "Don't find master smm net!\r\n");

    g_slist_free(obj_list);

    return RET_ERR;
}


LOCAL gint32 get_master_smm_ip(gchar *value, guint32 value_len, guchar *len)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret_val = 0;
    OBJ_HANDLE obj_handle = 0;
    gsize ip_len = 0;
    gsize net_len = 0;
    const gchar *string_addr = NULL;
    GVariant *property_data = NULL;
    gchar *ip_temp_pos = NULL;
    gchar *net_temp_pos = NULL;
    gchar old_master_ip[MAX_FILE_PATH_LEN] = {0};
    gchar cur_master_net[MAX_FILE_PATH_LEN] = {0};

    if (value == NULL || value_len == 0 || len == NULL) {
        return RET_ERR;
    }

    ret_val = dfl_get_object_handle(OBJECT_MASTER_SMM, &obj_handle);
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return RET_ERR;
    }

    
    ret_val = dfl_get_property_value(obj_handle, PROPERTY_MASTER_SMM_IP, &property_data);
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return ret_val;
    }

    string_addr = g_variant_get_string(property_data, &ip_len);
    if ((string_addr == NULL) || (ip_len == 0) || (ip_len >= MAX_FILE_PATH_LEN)) {
        debug_log(DLOG_ERROR, "smm ip is null or len:%" G_GSIZE_FORMAT "(Max:%d) is err!\n", ip_len, MAX_FILE_PATH_LEN);
        g_variant_unref(property_data);
        return RET_ERR;
    }

    
    safe_fun_ret = strncpy_s(old_master_ip, sizeof(old_master_ip), string_addr, ip_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    
    g_variant_unref(property_data);

    
    ret_val = get_master_smm_net(cur_master_net, sizeof(cur_master_net), &net_len);
    if (ret_val != RET_OK) {
        
        debug_log(DLOG_ERROR, "get_smm_master_net fail(Ret:%d)!\r\n", ret_val);
        return ret_val;
    }

    
    ip_temp_pos = strstr(old_master_ip, NET_SEPERATE_CHAR);
    if (ip_temp_pos == NULL) {
        return RET_ERR;
    }
    ip_temp_pos = strstr(ip_temp_pos + 1, NET_SEPERATE_CHAR);
    if (ip_temp_pos == NULL) {
        return RET_ERR;
    }
    net_temp_pos = strstr(cur_master_net, NET_SEPERATE_CHAR);
    if (net_temp_pos == NULL) {
        return RET_ERR;
    }
    net_temp_pos = strstr(net_temp_pos + 1, NET_SEPERATE_CHAR);
    if (net_temp_pos == NULL) {
        return RET_ERR;
    }

    
    safe_fun_ret =
        strncpy_s(net_temp_pos, MAX_FILE_PATH_LEN - (net_temp_pos - cur_master_net), ip_temp_pos, strlen(ip_temp_pos));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    *len = (guchar)strlen(cur_master_net); // cur_master_net数组长度128
    safe_fun_ret = strncpy_s(value, value_len, cur_master_net, *len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    

    return RET_OK;
}


LOCAL gint32 get_and_judge_manufacture_id(const guint8 *manufacture_id)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret_val = 0;
    GVariant *property_data = NULL;
    guint32 manufacture_id_to_judge = 0;

    ret_val = dfl_get_object_handle(BMC_OBJECT_NAME, &obj_handle);
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return RET_ERR;
    }

    
    ret_val = dfl_get_property_value(obj_handle, BMC_MANU_ID_NAME, &property_data);
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return ret_val;
    }

    if (property_data == NULL) {
        
        debug_log(DLOG_ERROR, DB_STR_GET_PROPERTY_NULL);
        return RET_ERR;
    }

    g_manufacture_id_resource = g_variant_get_uint32(property_data);
    g_variant_unref(property_data);

    if (manufacture_id == NULL) {
        return RET_ERR;
    }

    manufacture_id_to_judge = MAKE_DWORD(0, manufacture_id[2], manufacture_id[1], manufacture_id[0]);
    if (manufacture_id_to_judge != g_manufacture_id_resource) {
        debug_log(DLOG_INFO, "ManufactureId:%d(ShouldBe:%d) is invalid!\n", manufacture_id_to_judge,
            g_manufacture_id_resource);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 resource_respond_msg(const void *req_msg, gint32 data_len, guint8 *pdata, guint32 pdata_size)
{
    guint32 manu_id = g_manufacture_id_resource;

    if (req_msg == NULL) {
        return RET_ERR;
    }
    if (pdata == NULL) {
        return RET_ERR;
    }
    if (pdata_size < BIOS_MSG_HEAD_MIN_LEN) {
        return RET_ERR;
    }

    
    pdata[1] = LONGB0(manu_id);
    pdata[2] = LONGB1(manu_id);
    pdata[3] = LONGB2(manu_id);

    if (data_len > IPMB_MSG_MAX_PAYLOAD_LEN) {
        data_len = IPMB_MSG_MAX_PAYLOAD_LEN;
    }

    debug_log(DLOG_INFO, "RespCode:%u,RespLen:%d\n", pdata[0], data_len);

    return ipmi_send_response(req_msg, data_len, pdata);
}


gint32 atftp_execute_and_check_result(const gchar *command)
{
    gchar buf[256] = {0};
    FILE *ptr = NULL;
    gint32 ret = RET_ERR;

    if (command == NULL) {
        debug_log(DLOG_ERROR, "[%s] command is empty\n.", __FUNCTION__);
        return RET_ERR;
    }
    if (dal_check_shell_special_character(command) != RET_OK) {
        debug_log(DLOG_ERROR, "%s:command dal_check_shell_special_character failed", __FUNCTION__);
        return RET_ERR;
    }

    ptr = popen(command, "r");
    if (ptr != NULL) {
        if (vos_fgets_s(buf, sizeof(buf) - 1, ptr) != NULL) {
            // 返回0表示成功，否则都是有问题
            if (strncasecmp(buf, "0", strlen("0")) == 0) {
                ret = RET_OK;
            } else {
                debug_log(DLOG_ERROR, "[%s] check failed, buf=%s.\n", __FUNCTION__, buf);
            }
        }
        pclose(ptr);
    }
    return ret;
}

LOCAL gint32 is_file_upload_complete(gchar *file_name, guint8 name_len)
{
    glong old_len = -1;
    glong new_len = 0;
    gint32 i = 0;

    if (file_name == NULL || name_len == 0) {
        return RET_ERR;
    }
    // 接收到文件大小一致时，再启动是否接收完验证
    new_len = vos_get_file_length(file_name);

    while (i < MAX_LOOKUP_TIMES) {
        vos_task_delay(500);

        old_len = new_len;
        new_len = vos_get_file_length(file_name);
        if (new_len == old_len) {
            i++;
        } else {
            i = 0;
        }
    }

    return RET_OK;
}

LOCAL gint32 compare_and_copy_file(const gchar *file_name)
{
    gint32 iRet = -1;
    FILE *file_handle_src = NULL;
    FILE *file_handle_dst = NULL;
    gint32 i = MAX_COMP_DATA_BLOCK_LEN;
    glong old_len = 0;
    glong new_len = 0;
    gchar dest_path[MAX_FILE_PATH_LEN] = {0};
    gchar short_name[RES_MAX_FILE_NAME_LEN] = {0};
    gchar full_dest_file[MAX_FILE_PATH_LEN] = {0};
    guchar old_file[MAX_COMP_DATA_BLOCK_LEN] = {0};
    guchar new_file[MAX_COMP_DATA_BLOCK_LEN] = {0};

    get_bios_conf_file_info(RESOURCE_BIOS_CLP_CONFIG, dest_path, MAX_FILE_PATH_LEN, short_name, RES_MAX_FILE_NAME_LEN);
    iRet = snprintf_s(full_dest_file, MAX_FILE_PATH_LEN, MAX_FILE_PATH_LEN - 1, "%s/%s", dest_path, short_name);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }

    
    if (vos_get_file_accessible(full_dest_file) == FALSE) {
        debug_log(DLOG_INFO, "dest file %s not exist!\r\n", short_name);

        (void)copy_bios_file(RESOURCE_BIOS_CLP_CONFIG, file_name);
        return RET_OK;
    }

    new_len = vos_get_file_length(file_name);
    old_len = vos_get_file_length(full_dest_file);
    if (new_len != old_len) {
        debug_log(DLOG_INFO, "file size not same!\r\n");
        (void)copy_bios_file(RESOURCE_BIOS_CLP_CONFIG, file_name);

        
        set_clp_file_change_flag(TRUE);
        return RET_OK;
    }

    
    file_handle_src = dal_fopen_check_realpath(file_name, "r", file_name);
    if (file_handle_src == NULL) {
        debug_log(DLOG_ERROR, "g_fopen file failed, errstring:%s!\r\n", g_strerror(errno));
        return RET_ERR;
    }

    file_handle_dst = dal_fopen_check_realpath(full_dest_file, "r", full_dest_file);
    if (file_handle_dst == NULL) {
        (void)fclose(file_handle_src);
        debug_log(DLOG_ERROR, "g_fopen dest file failed, errstring:%s!", g_strerror(errno));
        return RET_ERR;
    }

    
    while (old_len) {
        if (old_len < MAX_COMP_DATA_BLOCK_LEN) {
            i = old_len;
        }

        
        if (i != fread(new_file, 1, i, file_handle_src)) {
            debug_log(DLOG_ERROR, "fread file failed!\r\n");
            (void)fclose(file_handle_src);
            (void)fclose(file_handle_dst);
            return RET_ERR;
        }

        
        if (i != fread(old_file, 1, i, file_handle_dst)) {
            debug_log(DLOG_ERROR, "fread file failed!\r\n");
            (void)fclose(file_handle_src);
            (void)fclose(file_handle_dst);
            return RET_ERR;
        }

        
        if (memcmp(old_file, new_file, i)) {
            debug_log(DLOG_INFO, "file data not same!\r\n");
            (void)fclose(file_handle_src);
            (void)fclose(file_handle_dst);

            (void)copy_bios_file(RESOURCE_BIOS_CLP_CONFIG, file_name);

            
            set_clp_file_change_flag(TRUE);

            return RET_OK;
        }

        old_len -= i;
    }

    debug_log(DLOG_INFO, "file size and data both same!\r\n");

    (void)fclose(file_handle_src);
    (void)fclose(file_handle_dst);

    return RET_OK;
}

#define MEZZ_HEAD "mezz"

LOCAL gint32 merge_clpconfig_files(void)
{
    const gchar *src_path = TMP_CLP_CONFIG_FILE_FROM_BT;
    const gchar *dst_path = TMP_BIOS_CLPCONFIG_INI_FILE_NAME;
    gint32 ret = 0;
    FILE *fp_src = NULL;
    gchar   data_buf[MAX_LINE_SIZE + 1] = {0};
    gchar **str_array = NULL;
    gchar   devicename[COMPONENT_DEVICE_NAME_LEN + 1] = {0};
    glong src_len = 0;

    // clpconfigfrombt.ini文件不存在时不需要合并，直接用clpconfig.ini文件，因此返回RET_OK，记录debug日志
    ret = vos_get_file_accessible(src_path);
    if (ret == FALSE) {
        debug_log(DLOG_DEBUG, "%s: clpconfig file from BT not existed.", __func__);
        return RET_OK;
    }

    debug_log(DLOG_DEBUG, "%s: merge clpconfig files start.", __func__);

    // clpconfig.ini不存在时，直接将clpconfigfrombt.ini内容复制到clpconfig.ini里，clpconfig.ini存在时，需要做检查
    if (vos_get_file_accessible(dst_path) == TRUE) {
        // 获取clpconfigfrombt.ini配置的网卡的devicename
        fp_src = g_fopen(src_path, "r");
        if (fp_src == NULL) {
            debug_log(DLOG_ERROR, "%s: clpconfig file from BT open fail.", __func__);
            return RET_ERR;
        }

        while (fgets(data_buf, MAX_LINE_SIZE, fp_src) != NULL) {
            if (g_ascii_strncasecmp(data_buf, MEZZ_HEAD, strlen(MEZZ_HEAD)) == 0) {
                str_array = g_strsplit_set(data_buf, ".", 0);
                if (str_array == NULL) {
                    (void)fclose(fp_src);
                    debug_log(DLOG_ERROR, "%s: g_strsplit_set fail.", __func__);
                    return RET_ERR;
                }
                ret = strncpy_s(devicename, sizeof(devicename), str_array[0], sizeof(devicename) - 1);
                if (ret != EOK) {
                    (void)fclose(fp_src);
                    g_strfreev(str_array);
                    debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret %d.", __func__, ret);
                    return RET_ERR;
                }
                break;
            }
        }
        (void)fclose(fp_src);
        if (str_array != NULL) {
            g_strfreev(str_array);
            str_array = NULL;
        }

        // 检查clpconfigfrombt.ini配置的mezz卡在clpconfig.ini里是否有配置，如果有，则删除clpconfig.ini里这张mezz卡的配置
        ret = _check_current_file(devicename);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: check current file error, ret %d.", __func__, ret);
            return RET_ERR;
        }
    }

    // 将clpconfigfrombt.ini文件内容追加到clpconfig.ini文件内，如果clpconfig.ini文件不存在，则先创建该文件
    src_len = vos_get_file_length(src_path);
    if (src_len == 0) {
        debug_log(DLOG_ERROR, "%s: clpconfig file from BT length is 0.", __func__);
        return RET_ERR;
    }

    ret = write_src_content_to_bios_config(src_path, "r", src_len, dst_path, "a+b");
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: merge clpconfig files fail, ret %d.", __func__, ret);
        return RET_ERR;
    }

    // 删除clpconfigfrombt.ini，避免每次带外设置网卡都得和该文件进行合并
    (void)vos_rm_filepath(src_path);

    debug_log(DLOG_DEBUG, "%s: merge clpconfig files success.", __func__);

    return RET_OK;
}


gint32 compare_and_copy_clp_config_file_with_lock(gchar *full_file_name)
{
    g_mutex_lock(&g_clpconfig_mutex);

    if (merge_clpconfig_files() != RET_OK) {
        g_mutex_unlock(&g_clpconfig_mutex);
        debug_log(DLOG_ERROR, "%s: merge_clpconfig_files fail.", __func__);
        return RET_ERR;
    }

    if (compare_and_copy_file(TMP_BIOS_CLPCONFIG_INI_FILE_NAME) != RET_OK) {
        g_mutex_unlock(&g_clpconfig_mutex);
        debug_log(DLOG_ERROR, "%s: compare_and_copy_clp_config_file_with_lock fail.", __func__);
        return RET_ERR;
    }

    
    if (g_ascii_strcasecmp(full_file_name, TMP_BIOS_CLPCONFIG_INI_FILE_NAME) != 0) {
        set_clp_config_bt_flag(CLP_FILE_FROM_BT, FALSE);
    }
    
    g_mutex_unlock(&g_clpconfig_mutex);

    return RET_OK;
}


guint8 is_clp_config_file_from_bt(void)
{
    return g_is_clpconfig_from_bt;
}

LOCAL gint32 get_file_name(const gchar *file_path, gchar *file_name, size_t file_name_len)
{
    gint32 ret;
    const gchar *temp = NULL;
    const gchar *temp1 = NULL;

    if (file_name_len == 0) {
        debug_log(DLOG_ERROR, "%s : file name len is invalid", __FUNCTION__);
        return RET_ERR;
    }

    // 无"/"表示路径名即为文件名
    temp = strrchr(file_path, '/');
    temp1 = (temp == NULL) ? (file_path) : (temp + 1);
    if (strlen(temp1) >= file_name_len) {
        debug_log(DLOG_ERROR, "file name is too long");
        return RET_ERR;
    }

    ret = snprintf_s(file_name, file_name_len, file_name_len - 1, "%s", temp1);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 get_full_file_path(const gchar *file_dir, const gchar *short_name, gchar *full_file_path,
    gint32 full_file_path_len)
{
    if (file_dir == NULL || strlen(file_dir) == 0) {
        debug_log(DLOG_ERROR, "%s: input file dir error", __FUNCTION__);
        return RET_ERR;
    }

    gint32 ret = 0;
    gint32 dir_len = strlen(file_dir);
    if (file_dir[dir_len - 1] == '/') {
        ret = snprintf_s(full_file_path, full_file_path_len, full_file_path_len - 1, "%s%s", file_dir, short_name);
    } else {
        ret = snprintf_s(full_file_path, full_file_path_len, full_file_path_len - 1, "%s/%s", file_dir, short_name);
    }

    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 process_clp_config_file(const gchar *file_name, const gchar *cur_dir)
{
    gint32 ret;
    gchar short_name[RES_MAX_FILE_NAME_LEN] = {0};
    gchar cur_full_name[MAX_FILE_PATH_LEN] = {0};

    if (file_name == NULL) {
        debug_log(DLOG_ERROR, "NULL  pointer param\r\n");
        return RET_ERR;
    }

    ret = get_file_name(file_name, short_name, sizeof(short_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get file name failed, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    if (is_file_upload_complete(short_name, sizeof(short_name)) != RET_OK) {
        debug_log(DLOG_ERROR, "upload file incomlete\r\n");
        return RET_ERR;
    }

    // 更改产生的临时配置文件权限
    (void)chown(short_name, 0, 200);
    (void)chmod(short_name, (S_IRUSR | S_IRGRP));

    
    g_mutex_lock(&g_clpconfig_mutex);

    ret = get_full_file_path(cur_dir, short_name, cur_full_name, sizeof(cur_full_name));
    if (ret != RET_OK) {
        g_mutex_unlock(&g_clpconfig_mutex);
        debug_log(DLOG_ERROR, "%s: get_full_file_path fail, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    if (compare_and_copy_file(cur_full_name) != RET_OK) {
        g_mutex_unlock(&g_clpconfig_mutex);
        debug_log(DLOG_ERROR, "compare_and_copy_file fail");
        return RET_ERR;
    }

    set_clp_config_bt_flag(CLP_FILE_FROM_HMM, FALSE);

    g_mutex_unlock(&g_clpconfig_mutex);
    

    return RET_OK;
}

LOCAL gint32 process_bios_config_file(guint8 resource_id, gchar *file_name)
{
    gint32 ret;
    gchar short_name[RES_MAX_FILE_NAME_LEN] = {0};

    if (file_name == NULL) {
        debug_log(DLOG_ERROR, "NULL  pointer param\r\n");
        return RET_ERR;
    }

    if ((resource_id != RESOURCE_BIOS_PROFILE_DISPLAY) && (resource_id != RESOURCE_BIOS_PROFILE_OPTION) &&
        (resource_id != RESOURCE_BIOS_PROFILE_CHANGED)
        
        && (resource_id != RESOURCE_BIOS_PROFILE_CURRENTVALUE) && (resource_id != RESOURCE_BIOS_PROFILE_SETTING) &&
        (resource_id != RESOURCE_BIOS_PROFILE_REGISTRY) && (resource_id != RESOURCE_BIOS_PROFILE_RESULT)) {
        
        return RET_OK;
    }

    ret = get_file_name(file_name, short_name, sizeof(short_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get file name failed, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    if (is_file_upload_complete(short_name, sizeof(short_name)) != RET_OK) {
        debug_log(DLOG_ERROR, "upload file incomlete\r\n");
        return RET_ERR;
    }

    // 更改产生的临时配置文件权限
    (void)chown(short_name, 0, 200);
    (void)chmod(short_name, (S_IRUSR | S_IRGRP));

    if (copy_bios_file(resource_id, short_name) != RET_OK) {
        debug_log(DLOG_ERROR, "copy_bios_file fail");
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL void set_respfile_status_by_configfile_id(guint8 resource_id, guint8 status)
{
#ifndef BMC_RELEASE
    guint8 st = 0;
#endif
    switch (resource_id) {
        case RESOURCE_BIOS_PROFILE_CHANGED:
#ifdef BMC_RELEASE
            (void)get_status_by_resource_id(RESOURCE_BIOS_PROFILE_OPTION);
#else
            st = get_status_by_resource_id(RESOURCE_BIOS_PROFILE_OPTION);
            debug_log(DLOG_DEBUG,
                "Before set_respfile_status_by_configfile_id ::: ,config_id = 0x%02x, respfile_id = 0x%02x, status = "
                "%d.",
                RESOURCE_BIOS_PROFILE_CHANGED, RESOURCE_BIOS_PROFILE_OPTION, st);
            set_status_by_resource_id(RESOURCE_BIOS_PROFILE_OPTION, status);
            debug_log(DLOG_DEBUG,
                "END set_respfile_status_by_configfile_id ::: ,config_id = 0x%02x, respfile_id = 0x%02x, status = %d.",
                RESOURCE_BIOS_PROFILE_CHANGED, RESOURCE_BIOS_PROFILE_OPTION, status);
#endif
            break;
        case RESOURCE_BIOS_PROFILE_SETTING:
#ifdef BMC_RELEASE
            (void)get_status_by_resource_id(RESOURCE_BIOS_PROFILE_CURRENTVALUE);
#else
            st = get_status_by_resource_id(RESOURCE_BIOS_PROFILE_CURRENTVALUE);
            debug_log(DLOG_DEBUG,
                "Before set_respfile_status_by_configfile_id ::: ,config_id = 0x%02x, respfile_id = 0x%02x, status = "
                "%d.",
                RESOURCE_BIOS_PROFILE_SETTING, RESOURCE_BIOS_PROFILE_CURRENTVALUE, st);
            set_status_by_resource_id(RESOURCE_BIOS_PROFILE_CURRENTVALUE, status);
            debug_log(DLOG_DEBUG,
                "END set_respfile_status_by_configfile_id ::: ,config_id = 0x%02x, respfile_id = 0x%02x, status = %d.",
                RESOURCE_BIOS_PROFILE_SETTING, RESOURCE_BIOS_PROFILE_CURRENTVALUE, status);
#endif
            break;
        case RESOURCE_BIOS_CLP_CONFIG:
#ifndef BMC_RELEASE
            st = get_status_by_resource_id(RESOURCE_BIOS_CLP_RESPONSE);
#else
            (void)get_status_by_resource_id(RESOURCE_BIOS_CLP_RESPONSE);
#endif
            debug_log(DLOG_DEBUG,
                "Before set_respfile_status_by_configfile_id ::: ,config_id = 0x%02x, respfile_id = 0x%02x, status = "
                "%d.",
                RESOURCE_BIOS_CLP_CONFIG, RESOURCE_BIOS_CLP_RESPONSE, st);
            set_status_by_resource_id(RESOURCE_BIOS_CLP_RESPONSE, status);
            debug_log(DLOG_DEBUG,
                "END set_respfile_status_by_configfile_id ::: ,config_id = 0x%02x, respfile_id = 0x%02x, status = %d.",
                RESOURCE_BIOS_CLP_CONFIG, RESOURCE_BIOS_CLP_RESPONSE, status);
            break;
        default:
            debug_log(DLOG_DEBUG, "The reource is no need set respfile status, resource_id = 0x%02x .", resource_id);
    }
}


LOCAL void TaskUploadFile(FILE_TRANS_INFO *trans_info)
{
    gint32 ret = RET_OK;
    gchar cmdbuf[256] = {0};
    guint8 upload_state = COMMAND_IN_PROGRESS;
    gchar short_name[RES_MAX_FILE_NAME_LEN] = {0};
    gchar custom_name[LOG_SPECIAL_STRING_LEN_MAX] = {"iBMC"};
    gchar* file_property_name[] = {
        CFG_FILE_DISPLAY, RESP_FILE_OPTION, DFG_FILE_CHANGE, NULL, HMM_FILE_BMCFIRMWARE, NULL, NULL, NULL,
        
        CFG_FILE_CLP, RESP_FILE_CLP, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, CFG_FILE_REGISTRY, RESP_FILE_CURRENTVALUE, CFG_FILE_SETTING, RESP_FILE_RESULT
        
    };

    if (trans_info == NULL) {
        debug_log(DLOG_ERROR, "%s:trans_info is null.\r\n", __FUNCTION__);
        upload_state = COMMAND_FAIL;
        goto error_quit2;
    }

    
    if (dal_get_custom_string_name(custom_name, BMC_CUSTOM_LOGIBMC_NAME, sizeof(custom_name)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_custom_string_name fail.", __FUNCTION__);
    }

    
    strategy_log(SLOG_INFO, "Start to download file (%s) from the HMM.\r\n",
        file_property_name[trans_info->resource_id]);

    
    if (chdir(trans_info->dest_path) != 0) {
        debug_log(DLOG_ERROR, "%s : change dir fail.\r\n", __FUNCTION__);
        upload_state = COMMAND_FAIL;
        ipmi_operation_log((gpointer)(trans_info->request_msg), "Upload %s to %s failed",
            file_property_name[trans_info->resource_id], custom_name);
        goto error_quit1;
    }

    
    if (trans_info->resource_id == RESOURCE_IMANA_FIRMWARE) {
        
        if (!clear_tmp_file()) {
            maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "clear tmp directory for smm upgrade\r\n");
        }
    }

    
    ret = snprintf_s(cmdbuf, sizeof(cmdbuf), sizeof(cmdbuf) - 1, "/opt/pme/script/tftp_get.sh %s %s",
        trans_info->src_ip, trans_info->src_path);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    (void)pthread_mutex_lock(&g_tftp_upload_mutex);
    
    ret = atftp_execute_and_check_result(cmdbuf);
    (void)pthread_mutex_unlock(&g_tftp_upload_mutex);

    strategy_log(SLOG_INFO, "Stop downloading file (%s) from the HMM.\r\n",
        file_property_name[trans_info->resource_id]);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:atftp_execute_and_check_result fail(Ret:%d) src_ip = %s",
            __FUNCTION__, ret, trans_info->src_ip);
        
        upload_state = COMMAND_COMPLETE;
        ipmi_operation_log((gpointer)(trans_info->request_msg), "Upload %s to %s failed",
            file_property_name[trans_info->resource_id], custom_name);
        goto error_quit1;
    }
    ipmi_operation_log((gpointer)(trans_info->request_msg), "Upload %s to %s successfully",
        file_property_name[trans_info->resource_id], custom_name);

    
    ret = set_config_file_sha256(trans_info->resource_id, CHECK_SUM_HMM_FILE_NAME);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:set_config_file_sha256 fail(Ret:%d).\r\n", __FUNCTION__, ret);
        goto error_quit1;
    }
    
    debug_log(DLOG_DEBUG, "resource_id is %u.\r\n", trans_info->resource_id);

    
    if (trans_info->resource_id < (sizeof(file_property_name) / sizeof(guchar *))) {
        strategy_log(SLOG_INFO, "Downloading file (%s) from the HMM successfully\n",
            file_property_name[trans_info->resource_id]);
    }

    

    
    (void)set_respfile_status_by_configfile_id(trans_info->resource_id, REPORTED);
    

    
    if (trans_info->resource_id == RESOURCE_IMANA_FIRMWARE) {
        if (get_file_name(trans_info->src_path, short_name, sizeof(short_name)) == RET_OK) {
            (void)chown(short_name, ROOT_USER_GID, OPERATOR_GID);
            (void)chmod(short_name, (S_IRUSR | S_IRGRP));
        }
    }

    
    if (RESOURCE_BIOS_CLP_CONFIG == trans_info->resource_id) {
        if (process_clp_config_file(trans_info->src_path, trans_info->dest_path) != RET_OK) {
            debug_log(DLOG_ERROR, "process_clp_config_file fail\r\n");
            upload_state = COMMAND_FAIL;
            goto error_quit1;
        }
    }

    if (process_bios_config_file(trans_info->resource_id, trans_info->src_path) != RET_OK) {
        debug_log(DLOG_ERROR, "process_bios_config_file fail\r\n");
    }

    upload_state = COMMAND_COMPLETE;

error_quit1:

    g_free(trans_info);

error_quit2:

    (void)pthread_mutex_lock(&g_tftp_upload_mutex);
    g_upload_flag = upload_state;
    (void)pthread_mutex_unlock(&g_tftp_upload_mutex);

    return;
}
LOCAL void __handle_transfer_result(gint32 transfer_ret, FILE_TRANS_INFO *trans_info, gchar *file_name,
    gchar *custom_name, guint8 *download_state)
{
    if (transfer_ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:atftp_execute_and_check_result fail(Ret:%d) src_ip = %s, file_name = %s.",
            __FUNCTION__, transfer_ret, trans_info->src_ip, file_name);
        *download_state = COMMAND_FAIL;
        ipmi_operation_log((gpointer)(trans_info->request_msg), "Download %s from %s failed", file_name, custom_name);
        return;
    }
    *download_state = COMMAND_COMPLETE;
    ipmi_operation_log((gpointer)(trans_info->request_msg), "Download %s from %s successfully", file_name, custom_name);
    strategy_log(SLOG_INFO, "Uploading file (%s) to the HMM successfully", file_name);
}

LOCAL void TaskDownloadFile(FILE_TRANS_INFO *trans_info)
{
    gint32 iRet = -1;
    gint32 ret = RET_OK;
    gchar cmdbuf[MAX_CMD_BUF_LEN] = {0};
    gchar file_name[RES_MAX_FILE_NAME_LEN] = {0};
    guint8 download_state = COMMAND_IN_PROGRESS;
    gchar custom_name[LOG_SPECIAL_STRING_LEN_MAX] = {"iBMC"};

    debug_log(DLOG_ERROR, "%s : TaskDownloadFile start.\r\n", __FUNCTION__);
    if (trans_info == NULL) {
        debug_log(DLOG_ERROR, "%s:trans_info is null.\r\n", __FUNCTION__);
        download_state = COMMAND_FAIL;
        goto error_quit2;
    }

    
    if (dal_get_custom_string_name(custom_name, BMC_CUSTOM_LOGIBMC_NAME, sizeof(custom_name)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_custom_string_name fail.", __FUNCTION__);
    }

    
    if (chdir(trans_info->src_path) != 0) {
        debug_log(DLOG_ERROR, "%s : change dir fail.\r\n", __FUNCTION__);
        download_state = COMMAND_FAIL;
        ipmi_operation_log((gpointer)(trans_info->request_msg), "Download resource from %s failed", custom_name);
        goto error_quit1;
    }

    
    switch (trans_info->resource_id) {
        case RESOURCE_BIOS_PROFILE_DISPLAY:
        case RESOURCE_BIOS_PROFILE_OPTION:
        case RESOURCE_BIOS_PROFILE_CHANGED:
        case RESOURCE_BIOS_CLP_CONFIG:
        case RESOURCE_BIOS_CLP_RESPONSE:

            
        case RESOURCE_BIOS_PROFILE_CURRENTVALUE:
        case RESOURCE_BIOS_PROFILE_SETTING:
        case RESOURCE_BIOS_PROFILE_REGISTRY:
        case RESOURCE_BIOS_PROFILE_RESULT:
            
            get_bios_conf_file_info(trans_info->resource_id, NULL, 0, file_name, RES_MAX_FILE_NAME_LEN);
            break;

        case RESOURCE_BMC_DEBUG_INFO:
        case RESOURCE_BMC_LOG_INFO:
            (void)pthread_mutex_lock(&g_tftp_download_mutex);
            iRet = snprintf_s(file_name, sizeof(file_name), sizeof(file_name) - 1, "%s", g_debug_info_filename);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
            }
            (void)pthread_mutex_unlock(&g_tftp_download_mutex);
            break;

        case RESOURCE_SOL_LOG_INFO:
            (void)snprintf_s(file_name, sizeof(file_name), sizeof(file_name) - 1, "%s", RESOURCE_SOL_LOG_FILE);
            break;

        case RESOURCE_BLACKBOX_INFO:
            (void)snprintf_s(file_name, sizeof(file_name), sizeof(file_name) - 1, "%s", RESOURCE_BLACKBOX_FILE);
            break;

        case RESOURCE_POWER_VIEW_ID:
            (void)snprintf_s(file_name, sizeof(file_name), sizeof(file_name) - 1, "%s", RESOURCE_POWER_VIEW_FILE);
            break;

        default:
            download_state = COMMAND_COMPLETE;
            ipmi_operation_log((gpointer)(trans_info->request_msg), "Download resource from %s failed", custom_name);
            goto error_quit1;
    }

    
    
    strategy_log(SLOG_INFO, "Start upload file (%s) to the HMM.\r\n", file_name);
    iRet = snprintf_s(cmdbuf, sizeof(cmdbuf), sizeof(cmdbuf) - 1, "/opt/pme/script/tftp_put.sh %s%s %s %s/%s",
        trans_info->src_path, file_name, trans_info->src_ip, trans_info->dest_path, file_name);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    (void)pthread_mutex_lock(&g_tftp_download_mutex);
    ret = atftp_execute_and_check_result(cmdbuf);
    (void)pthread_mutex_unlock(&g_tftp_download_mutex);

    strategy_log(SLOG_INFO, "Stop uploading file (%s) to the HMM.\r\n", file_name);
    __handle_transfer_result(ret, trans_info, file_name, custom_name, &download_state);

    
    
    ret = set_config_file_sha256(trans_info->resource_id, CHECK_SUM_BIOS_FILE_NAME);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:set_config_file_sha256 fail(Ret:%d).\r\n", __FUNCTION__, ret);
        goto error_quit1;
    }
    

    debug_log(DLOG_DEBUG, "HMM set_status_by_resource_id : resource_id = 0x%x , status = REPORTED\n",
        trans_info->resource_id);
    // 设置文件状态  UNREPORTED:未上报给HMM板  REPORTED:已上报给HMM板
    (void)set_status_by_resource_id(trans_info->resource_id, REPORTED);
    

error_quit1:

    g_free(trans_info);

error_quit2:

    (void)pthread_mutex_lock(&g_tftp_download_mutex);
    g_download_flag = download_state;
    (void)pthread_mutex_unlock(&g_tftp_download_mutex);
    return;
}


LOCAL gint32 resource_initiate_upload(const void *request_msg, guint8 *resp_data, guint32 resp_data_len)
{
    errno_t safe_fun_ret = EOK;
    const guint8 *src_data = NULL;
    guint8 resource_flag = 0;
    guint8 finish_flag = 0;
    guint8 offset = 0;
    guint8 src_len = 0;
    guint8 file_len = 0;
    gchar master_ip[MAX_FILE_PATH_LEN] = {0};
    guint8 ip_len = 0;
    static gchar file_name[MAX_FILE_PATH_LEN] = {0};
    FILE_TRANS_INFO *trans_info = NULL;
    
    gint32 retv = RET_OK;
    

    if (request_msg == NULL) {
        return RET_ERR;
    }
    if (!((request_msg != NULL) && (resp_data_len > BIOS_MSG_HEAD_MIN_LEN))) {
        return RET_ERR;
    }

    src_data = get_ipmi_src_data(request_msg);
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, "src_data is NULL!\n");
        resp_data[0] = COMP_CODE_LEN_INVALID;
        return resource_respond_msg(request_msg, BIOS_MSG_HEAD_MIN_LEN, resp_data, resp_data_len);
    }

    guint8 resource_id = src_data[RESOURCE_ID_OFFSET];
    resource_flag = src_data[RESOURCE_FLAG_OFFSET];

    
    
    if ((resource_id == RESOURCE_BIOS_CLP_CONFIG) && (g_is_clpconfig_from_bt > 0)) {
        debug_log(DLOG_INFO, "ClpConfig file from BT channel already exists.\r\n");
        maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "ClpConfig file from BT channel already exists.\r\n");
        resp_data[0] = COMP_CODE_STATUS_INVALID;
        return resource_respond_msg(request_msg, BIOS_MSG_HEAD_MIN_LEN, resp_data, resp_data_len);
    }
    

    finish_flag = resource_flag & 0x80;
    offset = resource_flag & 0x7f;

    
    src_len = get_ipmi_src_data_len(request_msg);
    if (src_len > UPLOAD_MIN_HEAD_LEN) {
        file_len = src_len - UPLOAD_MIN_HEAD_LEN;
    } else {
        debug_log(DLOG_ERROR, "src data_len:%u invalid!\n", src_len);
        resp_data[0] = COMP_CODE_LEN_INVALID;
        return resource_respond_msg(request_msg, BIOS_MSG_HEAD_MIN_LEN, resp_data, resp_data_len);
    }

    
    if (offset + file_len < MAX_FILE_PATH_LEN) {
        // 并非字符串。已判断目的缓冲区足够，无需判断返回值
        errno_t securec_rv = memcpy_s(file_name + offset, MAX_FILE_PATH_LEN - offset, src_data + UPLOAD_MIN_HEAD_LEN,
            file_len);
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "memcpy_s fail, securec_rv = %d", securec_rv);
        }
    } else {
        debug_log(DLOG_ERROR, "file path_len:%u(Max:%u) invalid!\n", offset + file_len, MAX_FILE_PATH_LEN);
        
        resp_data[0] = COMP_CODE_LEN_INVALID;
        return resource_respond_msg(request_msg, BIOS_MSG_HEAD_MIN_LEN, resp_data, resp_data_len);
    }

    
    if (finish_flag) {
        debug_log(DLOG_INFO, "not last frame,wait next frame...!\n");
        resp_data[0] = COMP_CODE_SUCCESS;
        return resource_respond_msg(request_msg, BIOS_MSG_HEAD_MIN_LEN, resp_data, resp_data_len);
    } else {
        trans_info = (FILE_TRANS_INFO *)g_malloc0(sizeof(FILE_TRANS_INFO));
        if (trans_info == NULL) {
            debug_log(DLOG_ERROR, "g_malloc0 fail!\n");
            resp_data[0] = COMP_CODE_STATUS_INVALID;
            return resource_respond_msg(request_msg, BIOS_MSG_HEAD_MIN_LEN, resp_data, resp_data_len);
        }

        // 并非字符串, 目的缓冲区大小为MAX_FILE_PATH_LEN，无需判断返回值
        (void)memcpy_s(trans_info->src_path, sizeof(trans_info->src_path), file_name, MAX_FILE_PATH_LEN);
        (void)memset_s(file_name, sizeof(file_name), 0, sizeof(file_name));
    }

    
    if (get_master_smm_ip(master_ip, sizeof(master_ip), &ip_len) == RET_OK) {
        
        safe_fun_ret = strncpy_s(trans_info->src_ip, sizeof(trans_info->src_ip), master_ip, ip_len);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        
    } else {
        g_free(trans_info);
        debug_log(DLOG_ERROR, "get_master_smm_ip fail!\n");
        resp_data[0] = COMP_CODE_STATUS_INVALID;
        return resource_respond_msg(request_msg, BIOS_MSG_HEAD_MIN_LEN, resp_data, resp_data_len);
    }

    trans_info->resource_id = resource_id;

    
    switch (resource_id) {
        case RESOURCE_BIOS_PROFILE_DISPLAY:
        case RESOURCE_BIOS_PROFILE_OPTION:
        case RESOURCE_BIOS_PROFILE_CHANGED:
        case RESOURCE_BIOS_CLP_CONFIG:
        case RESOURCE_IMANA_FIRMWARE:

            
        case RESOURCE_BIOS_PROFILE_REGISTRY:
        case RESOURCE_BIOS_PROFILE_CURRENTVALUE:
        case RESOURCE_BIOS_PROFILE_SETTING:
        case RESOURCE_BIOS_PROFILE_RESULT:
            
            (void)strncpy_s(trans_info->dest_path, sizeof(trans_info->dest_path), RESOURCE_BMC_DEBUG_INFO_PATH,
                sizeof(RESOURCE_BMC_DEBUG_INFO_PATH) - 1);
            break;

        default:

            g_free(trans_info);
            debug_log(DLOG_ERROR, "resource_id:%u is invalid!\n", resource_id);
            resp_data[0] = COMP_CODE_INVALID_FIELD;
            return resource_respond_msg(request_msg, BIOS_MSG_HEAD_MIN_LEN, resp_data, resp_data_len);
    }
    // 目标buffer长度为最大数据长度+IPMI消息头，源buffer长度为实际数据长度+IPMI消息头
    (void)memset_s(trans_info->request_msg, sizeof(trans_info->request_msg), 0, sizeof(trans_info->request_msg));
    safe_fun_ret = memcpy_s(trans_info->request_msg, sizeof(trans_info->request_msg), (const gchar *)request_msg,
        src_len + IPMI_MSG_HEAD_LEN);
    if (safe_fun_ret != EOK) {
        g_free(trans_info);
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        resp_data[0] = COMP_CODE_UNKNOWN;
        return resource_respond_msg(request_msg, BIOS_MSG_HEAD_MIN_LEN, resp_data, resp_data_len);
    }
    
    
    if (g_upload_flag != COMMAND_IN_PROGRESS) {
        (void)pthread_mutex_lock(&g_tftp_upload_mutex);
        g_upload_flag = COMMAND_IN_PROGRESS;
        (void)pthread_mutex_unlock(&g_tftp_upload_mutex);

        
        retv = vos_task_create(&g_UploadFile, "UploadFile", (TASK_ENTRY)TaskUploadFile, (void *)trans_info,
            DEFAULT_PRIORITY);
        if (retv != RET_OK) {
            debug_log(DLOG_ERROR, "Failed to creat task.");
            g_free(trans_info);
            resp_data[0] = COMP_CODE_BUSY;
            return resource_respond_msg(request_msg, BIOS_MSG_HEAD_MIN_LEN, resp_data, resp_data_len);
        }
        
    } else {
        g_free(trans_info);
        debug_log(DLOG_INFO, "upload task is busy!\n");
        resp_data[0] = COMP_CODE_INVALID_FIELD;
    }
    
    return resource_respond_msg(request_msg, BIOS_MSG_HEAD_MIN_LEN, resp_data, resp_data_len);
}


LOCAL gint32 resource_get_upload_status(const void *request_msg, guint8 *resp_data, guint32 resp_len)
{
    if (request_msg == NULL) {
        return RET_ERR;
    }
    if (resp_data == NULL) {
        return RET_ERR;
    }
    if (resp_len < DESP_DATA_LEN) {
        return RET_ERR;
    }

    
    if (g_upload_flag == COMMAND_COMPLETE) {
        resp_data[4] = 0x80;
    } else {
        resp_data[4] = 0x00;
    }

    return resource_respond_msg(request_msg, DESP_DATA_LEN, resp_data, resp_len);
}



LOCAL gint32 clear_tmp_file(void)
{
    gint32 ret = 0;
    GSList *output_list = NULL;
    OBJ_HANDLE bmc_handle = 0;
    ret = dal_get_object_handle_nth(CLASS_NAME_BMC, 0, &bmc_handle);
    if (ret == RET_OK) {
        ret = dfl_call_class_method(bmc_handle, METHOD_BMC_CLEAR_TMP_FILE, NULL, NULL, &output_list);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "call method %s failed \n", METHOD_BMC_CLEAR_TMP_FILE);
            return ret;
        }

        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    } else {
        debug_log(DLOG_ERROR, "get the %s object fail, ret = %d\n", CLASS_NAME_BMC, ret);
    }

    return ret;
}
LOCAL gint32 get_resource_file_type(guint8 resource_id, gchar *file_type, size_t file_type_len)
{
    gint32 ret;
    gchar custom_name[LOG_SPECIAL_STRING_LEN_MAX] = {"iBMC"};

    if (file_type_len == 0) {
        debug_log(DLOG_ERROR, "%s : file_type_len is invalid", __FUNCTION__);
        return RET_ERR;
    }

    
    if (dal_get_custom_string_name(custom_name, BMC_CUSTOM_LOGIBMC_NAME, sizeof(custom_name)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_custom_string_name fail.", __FUNCTION__);
    }

    switch (resource_id) {
        case RESOURCE_BIOS_CLP_CONFIG:
        case RESOURCE_BIOS_CLP_RESPONSE:
            ret = snprintf_s(file_type, file_type_len, file_type_len - 1, "%s", "Clp config file");
            break;

        case RESOURCE_IMANA_FIRMWARE:
            ret = snprintf_s(file_type, file_type_len, file_type_len - 1, "%s firmware file", custom_name);
            break;

        case RESOURCE_BIOS_PROFILE_DISPLAY:
        case RESOURCE_BIOS_PROFILE_OPTION:
        case RESOURCE_BIOS_PROFILE_CHANGED:
        case RESOURCE_BIOS_PROFILE_REGISTRY:
        case RESOURCE_BIOS_PROFILE_CURRENTVALUE:
        case RESOURCE_BIOS_PROFILE_SETTING:
        case RESOURCE_BIOS_PROFILE_RESULT:
            ret = snprintf_s(file_type, file_type_len, file_type_len - 1, "%s", "Bios config file");
            break;

        case RESOURCE_BMC_DEBUG_INFO:
        case RESOURCE_BMC_LOG_INFO:
            ret = snprintf_s(file_type, file_type_len, file_type_len - 1, "%s log file", custom_name);
            break;

        case RESOURCE_SOL_LOG_INFO:
            ret = snprintf_s(file_type, file_type_len, file_type_len - 1, "%s", "Sol log file");
            break;

        case RESOURCE_BLACKBOX_INFO:
            ret = snprintf_s(file_type, file_type_len, file_type_len - 1, "%s", "Blackbox info file");
            break;

        case RESOURCE_POWER_VIEW_ID:
            ret = snprintf_s(file_type, file_type_len, file_type_len - 1, "%s", "Power view file");
            break;

        default:
            debug_log(DLOG_ERROR, "%s :resource id (%#x) is invalid", __FUNCTION__, resource_id);
            return RET_ERR;
    }
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}
LOCAL gint32 check_resource_precondition(const guint8 *src_data)
{
    guint8 ser_type = 0;
    gint32 ret_val;

    (void)bios_get_server_type(&ser_type);
    if (ser_type != SERVER_TYPE_BLADE) {
        debug_log(DLOG_ERROR, "%s:current server type not support this cmd.\n", __FUNCTION__);
        return RET_ERR;
    }

    
    ret_val = get_and_judge_manufacture_id(src_data);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, DB_STR_MUNU_ID_ERR, ret_val);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 upload_resource_to_bmc(const void *req_msg, gpointer user_data)
{
    const guint8 *src_data = NULL;
    gint32 ret_val = RET_ERR;
    guint8 resp_data[10] = {0};
    gchar file_type[RES_MAX_FILE_NAME_LEN] = {0};
    gchar custom_name[LOG_SPECIAL_STRING_LEN_MAX] = {"iBMC"};

    
    if (req_msg == NULL) {
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    
    if (dal_get_custom_string_name(custom_name, BMC_CUSTOM_LOGIBMC_NAME, sizeof(custom_name)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_custom_string_name fail.", __FUNCTION__);
    }

    src_data = get_ipmi_src_data(req_msg);
    
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, DB_STR_GET_SRC_DATA_FAIL);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }
    

    

    if (check_resource_precondition(src_data) != RET_OK) {
        debug_log(DLOG_ERROR, "%s:check_resource_precondition failed", __FUNCTION__);
        ipmi_operation_log(req_msg, "Upload resource to %s failed", custom_name);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }

    if (get_resource_file_type(src_data[RESOURCE_ID_OFFSET], file_type, sizeof(file_type)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get_resource_file_type failed", __FUNCTION__);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }

    switch (src_data[RESOURCE_SUB_CMD_OFFSET]) {
        case UPLOAD_OPERATE_INITIATE:

            // 1)initiate upload
            ret_val = resource_initiate_upload(req_msg, resp_data, sizeof(resp_data));
            if (ret_val != RET_OK) {
                ipmi_operation_log(req_msg, "Start uploading %s to %s failed", file_type, custom_name);
            }
            break;

        case UPLOAD_OPERATE_STATUS:

            // 2)get upload status
            ret_val = resource_get_upload_status(req_msg, resp_data, sizeof(resp_data));
            break;

        default:
            resp_data[0] = COMP_CODE_INVALID_FIELD;
            ipmi_operation_log(req_msg, "Upload %s to %s failed", file_type, custom_name);
            return resource_respond_msg(req_msg, BIOS_MSG_HEAD_MIN_LEN, resp_data, sizeof(resp_data));
    }

    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "SubCmd:%d fail(Ret:0x%x)!\n", src_data[RESOURCE_SUB_CMD_OFFSET], ret_val);
    }

    return ret_val;
}


LOCAL gint32 get_debug_file_info(void)
{
    gsize path_len = 0;
    gint32 iRet = RET_OK;
    OBJ_HANDLE obj_handle = 0;
    const gchar *resource_path = NULL;
    GVariant *path_value = NULL;

    
    (void)memset_s(g_debug_info_filename, sizeof(g_debug_info_filename), 0, sizeof(g_debug_info_filename));
    (void)memset_s(g_debug_info_filepath, sizeof(g_debug_info_filepath), 0, sizeof(g_debug_info_filepath));

    iRet = dfl_get_object_handle(OBJECT_DUMP, &obj_handle);
    if (iRet != DFL_OK) {
        debug_log(DLOG_ERROR, DB_STR_DFL_HANDLE_NULL);
        return RET_ERR;
    }

    
    iRet = dfl_get_property_value(obj_handle, PROPERTY_DUMP_FILE_PATH, &path_value);
    if (iRet != DFL_OK) {
        debug_log(DLOG_ERROR, DB_STR_SET_PROPERTY_ERR, iRet);
        return RET_ERR;
    }

    if (path_value == NULL) {
        
        debug_log(DLOG_ERROR, DB_STR_GET_PROPERTY_NULL);
        return RET_ERR;
    }

    resource_path = g_variant_get_string(path_value, &path_len);
    if ((path_len == 0) || (path_len > DEBUG_INFO_FILE_NAME_LEN)) {
        g_variant_unref(path_value);
        debug_log(DLOG_ERROR, "path_len:%" G_GSIZE_FORMAT "(Max:%d) is invalid!\r\n", path_len,
            DEBUG_INFO_FILE_NAME_LEN);
        return RET_ERR;
    }

    if (dal_get_file_name(resource_path, g_debug_info_filename, sizeof(g_debug_info_filename)) == FALSE) {
        g_variant_unref(path_value);
        debug_log(DLOG_ERROR, "dal_get_file_name failed\r\n");
        return RET_ERR;
    }

    if (vos_get_file_dir(resource_path, g_debug_info_filepath, DEBUG_INFO_FILE_NAME_LEN) == FALSE) {
        g_variant_unref(path_value);
        debug_log(DLOG_ERROR, "vos_get_file_dir failed\r\n");
        return RET_ERR;
    }

    g_variant_unref(path_value);

    return RET_OK;
}



LOCAL void print_resource_info(FILE_TRANS_INFO *trans_info, const gchar *property_name, guint32 size, gchar *buff,
    guint32 buff_size, const gchar *property_name2)
{
    errno_t safe_fun_ret = EOK;

    if (buff == NULL || buff_size == 0) {
        debug_log(DLOG_ERROR, "%s: input parm error\n", __FUNCTION__);
        return;
    }

    safe_fun_ret = memcpy_s(trans_info->src_path, sizeof(trans_info->src_path), property_name, size);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    safe_fun_ret = snprintf_s(buff, buff_size, buff_size - 1, "%s%s", trans_info->src_path, property_name2);
    if (safe_fun_ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
}


LOCAL gint32 resource_initiate_download(const void *request_msg, guint8 *resp_data, guint32 resp_data_size)
{
    gint32 iRet = -1;
    const guint8 *src_data = NULL;
    guint8 resource_flag = 0;
    guint8 finish_flag = 0;
    guint8 offset = 0;
    guint8 src_len = 0;
    guint8 file_len = 0;
    static gchar file_name[MAX_FILE_PATH_LEN] = {0};
    FILE_TRANS_INFO *trans_info = NULL;
    gchar bios_file_name[RES_MAX_FILE_NAME_LEN] = {0};
    gchar buff[MAX_FILE_PATH_LEN] = {0};
    gchar master_ip[MAX_FILE_PATH_LEN] = {0};
    guint8 ip_len = 0;
    
    gint32 retv = RET_OK;
    
    if (request_msg == NULL) {
        return RET_ERR;
    }
    if (resp_data == NULL) {
        return RET_ERR;
    }
    if (resp_data_size < RESOURCE_MSG_HEAD_LEN) {
        debug_log(DLOG_ERROR, "%s:resp_data buffer length is insufficient: %d!\n", __FUNCTION__, resp_data_size);
        return RET_ERR;
    }

    src_data = get_ipmi_src_data(request_msg);
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, "src_data is NULL!\n");
        resp_data[0] = COMP_CODE_LEN_INVALID;
        return resource_respond_msg(request_msg, RESOURCE_MSG_HEAD_LEN, resp_data, resp_data_size);
    }

    guint8 resource_id = src_data[RESOURCE_ID_OFFSET];
    resource_flag = src_data[RESOURCE_FLAG_OFFSET];
    resp_data[4] = COMP_CODE_SUCCESS;

    finish_flag = resource_flag & 0x80;
    offset = resource_flag & 0x7f;

    
    src_len = get_ipmi_src_data_len(request_msg);
    if (src_len > UPLOAD_MIN_HEAD_LEN) {
        file_len = src_len - UPLOAD_MIN_HEAD_LEN;
    } else {
        debug_log(DLOG_ERROR, "src data_len:%u invalid!\n", src_len);
        resp_data[0] = COMP_CODE_LEN_INVALID;
        return resource_respond_msg(request_msg, RESOURCE_MSG_HEAD_LEN, resp_data, resp_data_size);
    }

    
    if (offset + file_len < MAX_FILE_PATH_LEN) {
        
        errno_t securec_rv = memcpy_s(file_name + offset, MAX_FILE_PATH_LEN - offset, src_data + UPLOAD_MIN_HEAD_LEN,
            file_len);
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "memcpy_s fail, securec_rv = %d", securec_rv);
        }
    } else {
        debug_log(DLOG_ERROR, "file path_len:%u(Max:%u) invalid!\n", offset + file_len, MAX_FILE_PATH_LEN);
        
        resp_data[0] = COMP_CODE_LEN_INVALID;
        return resource_respond_msg(request_msg, RESOURCE_MSG_HEAD_LEN, resp_data, resp_data_size);
    }

    
    if (finish_flag) {
        debug_log(DLOG_INFO, "not last frame,wait next frame...!\n");
        return resource_respond_msg(request_msg, RESOURCE_MSG_HEAD_LEN, resp_data, resp_data_size);
    } else {
        trans_info = (FILE_TRANS_INFO *)g_malloc0(sizeof(FILE_TRANS_INFO));
        if (trans_info == NULL) {
            debug_log(DLOG_ERROR, "g_malloc0 fail!\n");
            resp_data[0] = COMP_CODE_STATUS_INVALID;
            return resource_respond_msg(request_msg, RESOURCE_MSG_HEAD_LEN, resp_data, resp_data_size);
        }

        // 目的缓冲区大小为MAX_FILE_PATH_LEN，无需判断返回值
        (void)memcpy_s(trans_info->dest_path, sizeof(trans_info->dest_path), file_name, MAX_FILE_PATH_LEN);
        (void)memset_s(file_name, sizeof(file_name), 0, sizeof(file_name));
    }

    
    if (get_master_smm_ip(master_ip, sizeof(master_ip), &ip_len) == RET_OK) {
        
        (void)strncpy_s(trans_info->src_ip, sizeof(trans_info->src_ip), master_ip, sizeof(trans_info->src_ip) - 1);
        
    } else {
        g_free(trans_info);
        debug_log(DLOG_ERROR, "get_master_smm_ip fail!\n");
        resp_data[0] = COMP_CODE_STATUS_INVALID;
        return resource_respond_msg(request_msg, RESOURCE_MSG_HEAD_LEN, resp_data, resp_data_size);
    }

    
    trans_info->resource_id = resource_id;

    switch (resource_id) {
        case RESOURCE_BIOS_PROFILE_DISPLAY:
        case RESOURCE_BIOS_PROFILE_OPTION:
        case RESOURCE_BIOS_PROFILE_CHANGED:
        case RESOURCE_BIOS_CLP_RESPONSE:
        case RESOURCE_BIOS_CLP_CONFIG:
            
        case RESOURCE_BIOS_PROFILE_CURRENTVALUE:
        case RESOURCE_BIOS_PROFILE_SETTING:
        case RESOURCE_BIOS_PROFILE_REGISTRY:
        case RESOURCE_BIOS_PROFILE_RESULT:
            
            get_bios_conf_file_info(resource_id, trans_info->src_path, MAX_FILE_PATH_LEN, bios_file_name,
                RES_MAX_FILE_NAME_LEN);
            iRet = snprintf_s(buff, sizeof(buff), sizeof(buff) - 1, "%s%s", trans_info->src_path, bios_file_name);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
            }
            break;

        case RESOURCE_BMC_DEBUG_INFO:
        case RESOURCE_BMC_LOG_INFO:
            (void)get_debug_file_info();
            
            (void)strncpy_s(trans_info->src_path, sizeof(trans_info->src_path), g_debug_info_filepath,
                sizeof(trans_info->src_path) - 1);
             // 安全排查 20170512，字符串拷贝用 strncpy_s
            iRet =
                snprintf_s(buff, sizeof(buff), sizeof(buff) - 1, "%s%s", trans_info->src_path, g_debug_info_filename);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
            }
            break;

        case RESOURCE_SOL_LOG_INFO:
            print_resource_info(trans_info, RESOURCE_SOL_LOG_PATH, sizeof(RESOURCE_SOL_LOG_PATH), buff, sizeof(buff),
                RESOURCE_SOL_LOG_FILE);
            break;

        case RESOURCE_BLACKBOX_INFO:
            print_resource_info(trans_info, RESOURCE_BLACKBOX_PATH, sizeof(RESOURCE_BLACKBOX_PATH), buff, sizeof(buff),
                RESOURCE_BLACKBOX_FILE);
            break;

        case RESOURCE_POWER_VIEW_ID:
            print_resource_info(trans_info, RESOURCE_POWER_VIEW_PATH, sizeof(RESOURCE_POWER_VIEW_PATH), buff,
                sizeof(buff), RESOURCE_POWER_VIEW_FILE);
            break;

        default:
            g_free(trans_info);
            debug_log(DLOG_ERROR, "resource_id:%u is invalid!\n", resource_id);
            resp_data[0] = COMP_CODE_INVALID_FIELD;
            return resource_respond_msg(request_msg, RESOURCE_MSG_HEAD_LEN, resp_data, resp_data_size);
    }

    // 判断要下载的文件是否存在
    if (FALSE == vos_get_file_accessible(buff)) {
        g_free(trans_info);
        debug_log(DLOG_INFO, "%s file not exist!\n", buff);
        resp_data[4] = COMP_FILE_NOT_EXIT;
        return resource_respond_msg(request_msg, RESOURCE_MSG_HEAD_LEN, resp_data, resp_data_size);
    }
    // 目标buffer长度为最大数据长度+IPMI消息头，源buffer长度为实际数据长度+IPMI消息头
    (void)memset_s(trans_info->request_msg, sizeof(trans_info->request_msg), 0, sizeof(trans_info->request_msg));
    iRet = memcpy_s(trans_info->request_msg, sizeof(trans_info->request_msg), (const gchar *)request_msg,
        src_len + IPMI_MSG_HEAD_LEN);
    if (iRet != EOK) {
        g_free(trans_info);
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, iRet);
        resp_data[0] = COMP_CODE_UNKNOWN;
        return resource_respond_msg(request_msg, BIOS_MSG_HEAD_MIN_LEN, resp_data, resp_data_size);
    }
    
    if (g_download_flag != COMMAND_IN_PROGRESS) {
        (void)pthread_mutex_lock(&g_tftp_download_mutex);
        g_download_flag = COMMAND_IN_PROGRESS;
        (void)pthread_mutex_unlock(&g_tftp_download_mutex);

        
        retv = vos_task_create(&g_DownloadFile, "DownloadFile", (TASK_ENTRY)TaskDownloadFile, (void *)trans_info,
            DEFAULT_PRIORITY);
        if (retv != RET_OK) {
            debug_log(DLOG_ERROR, "Failed to creat task.");
            g_free(trans_info);
            resp_data[0] = COMP_CODE_BUSY;
            return resource_respond_msg(request_msg, RESOURCE_MSG_HEAD_LEN, resp_data, resp_data_size);
        }
        
    } else {
        g_free(trans_info);
        resp_data[0] = COMP_CODE_BUSY;
        debug_log(DLOG_INFO, "download task is busy!\n");
    }

    return resource_respond_msg(request_msg, RESOURCE_MSG_HEAD_LEN, resp_data, resp_data_size);
}


LOCAL gint32 resource_get_download_name(const void *request_msg, guint8 *resp_data, guint32 resp_data_len)
{
    errno_t safe_fun_ret = EOK;
    const guint8 *src_data = NULL;
    guint8 resource_id = 0;
    gchar file_name[RES_MAX_FILE_NAME_LEN] = {0};

    if (request_msg == NULL) {
        return RET_ERR;
    }
    if (resp_data == NULL) {
        return RET_ERR;
    }

    src_data = get_ipmi_src_data(request_msg);
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, "src_data is NULL!\n");
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        return resource_respond_msg(request_msg, RESOURCE_MSG_HEAD_LEN, resp_data, resp_data_len);
    }

    resource_id = src_data[RESOURCE_ID_OFFSET];

    
    if (COMMAND_IN_PROGRESS == g_download_flag) {
        resp_data[5] = 0x00;
        resp_data[6] = 0x00;
    } else {
        resp_data[5] = 0x80;
        resp_data[6] = 0x00;
    }

    // 返回下载的文件名
    switch (resource_id) {
        case RESOURCE_BIOS_PROFILE_DISPLAY:
        case RESOURCE_BIOS_PROFILE_OPTION:
        case RESOURCE_BIOS_PROFILE_CHANGED:
        case RESOURCE_BIOS_CLP_CONFIG:
        case RESOURCE_BIOS_CLP_RESPONSE:

            
        case RESOURCE_BIOS_PROFILE_CURRENTVALUE:
        case RESOURCE_BIOS_PROFILE_SETTING:
        case RESOURCE_BIOS_PROFILE_REGISTRY:
        case RESOURCE_BIOS_PROFILE_RESULT:
            
            get_bios_conf_file_info(resource_id, NULL, 0, file_name, RES_MAX_FILE_NAME_LEN);
            resp_data[7] = (guint8)strlen(file_name); // file_name数组大小为64
            // resp_data是一个大小为10字节的数组
            safe_fun_ret = memcpy_s(resp_data + 8, resp_data_len - 8, file_name, resp_data[7]);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            return resource_respond_msg(request_msg, RESOURCE_MSG_NORMAL_LEN + resp_data[7], resp_data, resp_data_len);

        case RESOURCE_BMC_DEBUG_INFO:
        case RESOURCE_BMC_LOG_INFO:
            (void)get_debug_file_info();
            resp_data[7] = (guint8)strlen(g_debug_info_filename); // g_debug_info_filename数组大小为64
            safe_fun_ret = memcpy_s(resp_data + 8, resp_data_len - 8, g_debug_info_filename, resp_data[7]);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            return resource_respond_msg(request_msg, RESOURCE_MSG_NORMAL_LEN + resp_data[7], resp_data, resp_data_len);

        case RESOURCE_SOL_LOG_INFO:
            resp_data[7] = (guint8)strlen(RESOURCE_SOL_LOG_FILE); // 常数字符串长度13
            safe_fun_ret = memcpy_s(resp_data + 8, resp_data_len - 8, RESOURCE_SOL_LOG_FILE, resp_data[7]);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            return resource_respond_msg(request_msg, RESOURCE_MSG_NORMAL_LEN + resp_data[7], resp_data, resp_data_len);

        case RESOURCE_BLACKBOX_INFO:
            resp_data[7] = (guint8)strlen(RESOURCE_BLACKBOX_FILE); // 常数字符串长度12
            safe_fun_ret = memcpy_s(resp_data + 8, resp_data_len - 8, RESOURCE_BLACKBOX_FILE, resp_data[7]);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            return resource_respond_msg(request_msg, RESOURCE_MSG_NORMAL_LEN + resp_data[7], resp_data, resp_data_len);

        case RESOURCE_POWER_VIEW_ID:
            resp_data[7] = (guint8)strlen(RESOURCE_POWER_VIEW_FILE); // 常数字符串长度13
            safe_fun_ret = memcpy_s(resp_data + 8, resp_data_len - 8, RESOURCE_POWER_VIEW_FILE, resp_data[7]);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            return resource_respond_msg(request_msg, RESOURCE_MSG_NORMAL_LEN + resp_data[7], resp_data, resp_data_len);

        default:
            resp_data[0] = COMP_CODE_INVALID_FIELD;
            break;
    }

    return resource_respond_msg(request_msg, RESOURCE_MSG_HEAD_LEN, resp_data, resp_data_len);
}


LOCAL gint32 resource_get_download_status(const void *request_msg, guint8 *resp_data, guint16 data_len)
{
    if (request_msg == NULL) {
        return RET_ERR;
    }
    if (resp_data == NULL || data_len == 0) {
        return RET_ERR;
    }

    
    if (g_download_flag == COMMAND_COMPLETE) {
        resp_data[5] = 0x80;
        resp_data[6] = 0x00;
    } else if (g_download_flag == COMMAND_IN_PROGRESS) {
        resp_data[5] = 0x00;
        resp_data[6] = 0x00;
    } else {
        resp_data[0] = COMP_CODE_UNKNOWN;
        return resource_respond_msg(request_msg, RESOURCE_MSG_HEAD_LEN, resp_data, data_len);
    }

    return resource_respond_msg(request_msg, RESOURCE_MSG_STATUS_LEN, resp_data, data_len);
}


gint32 download_resource_from_bmc(const void *req_msg, gpointer user_data)
{
    const guint8 *src_data = NULL;
    gint32 ret_val = RET_ERR;
    guint8 resp_data[MAX_CMD_BUF_LEN] = {0};
    gchar file_type[RES_MAX_FILE_NAME_LEN] = {0};
    gchar custom_name[LOG_SPECIAL_STRING_LEN_MAX] = {"iBMC"};

    if (req_msg == NULL) {
        
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    
    if (dal_get_custom_string_name(custom_name, BMC_CUSTOM_LOGIBMC_NAME, sizeof(custom_name)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_custom_string_name fail.", __FUNCTION__);
    }

    src_data = get_ipmi_src_data(req_msg);
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, DB_STR_GET_SRC_DATA_FAIL);
        
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
        
    }
    if (check_resource_precondition(src_data) != RET_OK) {
        debug_log(DLOG_ERROR, "%s:check_resource_precondition failed", __FUNCTION__);
        ipmi_operation_log(req_msg, "Download resource from %s failed", custom_name);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }

    if (get_resource_file_type(src_data[RESOURCE_ID_OFFSET], file_type, sizeof(file_type)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get_resource_file_type failed", __FUNCTION__);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }

    
    switch (src_data[RESOURCE_SUB_CMD_OFFSET]) {
        case DOWNLOAD_OPERATE_INITIATE:

            // 1)initiate download
            ret_val = resource_initiate_download(req_msg, resp_data, sizeof(resp_data));
            if (ret_val != RET_OK) {
                ipmi_operation_log(req_msg, "Start downloading %s from %s failed", file_type, custom_name);
            }
            break;

        case DOWNLOAD_OPERATE_NAME:

            // 2)get resource name
            ret_val = resource_get_download_name(req_msg, resp_data, sizeof(resp_data));
            break;

        case DOWNLOAD_OPERATE_STATUS:

            // 3)get download status
            ret_val = resource_get_download_status(req_msg, resp_data, sizeof(resp_data));
            break;

        default:
            resp_data[0] = COMP_CODE_INVALID_FIELD;
            ipmi_operation_log(req_msg, "Download %s from %s failed", file_type, custom_name);
            return resource_respond_msg(req_msg, BIOS_MSG_HEAD_MIN_LEN, resp_data, sizeof(resp_data));
    }

    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "SubCmd:%d fail(Ret:0x%x)!\n", src_data[RESOURCE_SUB_CMD_OFFSET], ret_val);
    }

    return ret_val;
}


void set_status_by_resource_id(guint8 resource_id, guint8 status)
{
    if (resource_id >= RESOURCE_NUM) {
        debug_log(DLOG_ERROR, "resource_id is out of range, resource_id = 0x%x\n", resource_id);
        return;
    }
    g_config_file_info_array[resource_id].status = status;
    return;
}


LOCAL guint8 get_status_by_resource_id(guint8 resource_id)
{
    guint8 status = REPORTED;
    if (resource_id >= RESOURCE_NUM) {
        status = 0xff;
        debug_log(DLOG_ERROR, "resource_id is out of range, resource_id = 0x%x\n", resource_id);
        return status;
    }
    status = g_config_file_info_array[resource_id].status;
    return status;
}


LOCAL gint32 get_respconfig_file_status(const void *request_msg, guint8 *resp_data, guint32 resp_data_size)
{
    const guint8 *src_data = NULL;
    guint8 resource_id = 0;
    guint8 file_status;

    if (request_msg == NULL || resp_data == NULL) {
        return RET_ERR;
    }
    src_data = get_ipmi_src_data(request_msg);
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, "src_data is NULL!\n");
        resp_data[0] = COMP_CODE_LEN_INVALID;
        return resource_respond_msg(request_msg, RESOURCE_MSG_HEAD_LEN, resp_data, resp_data_size);
    }

    resource_id = src_data[RESOURCE_ID_OFFSET];
    file_status = get_status_by_resource_id(resource_id);
    if (file_status != REPORTED && file_status != UNREPORTED) {
        debug_log(DLOG_ERROR, "get status by resource id fail. file_status = %d , resource_id = 0x%x\n", file_status,
            resource_id);
        resp_data[0] = COMP_CODE_UNKNOWN;
        return resource_respond_msg(request_msg, RESOURCE_MSG_HEAD_LEN, resp_data, resp_data_size);
    }
    resp_data[4] = file_status;
    return resource_respond_msg(request_msg, RESOURCE_MSG_HEAD_LEN, resp_data, resp_data_size);
}


LOCAL void set_icv_value_by_resource_id(guint8 resource_id, guint8 *icv_value_sha256)
{
    errno_t safe_fun_ret = EOK;

    if (resource_id >= RESOURCE_NUM) {
        debug_log(DLOG_ERROR, "resource_id is out of range, resource_id = 0x%x\n", resource_id);
        return;
    }
    safe_fun_ret = memcpy_s(g_config_file_info_array[resource_id].icv_value_sha256, SHA256_CHECKSUM_LENGTH,
        icv_value_sha256, SHA256_CHECKSUM_LENGTH);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    return;
}


LOCAL void get_icv_value_by_resource_id(guint8 resource_id, guint8 **icv_value_sha256)
{
    if (resource_id >= RESOURCE_NUM) {
        debug_log(DLOG_ERROR, "resource_id is out of range, resource_id = 0x%x\n", resource_id);
        return;
    }
    *icv_value_sha256 = g_config_file_info_array[resource_id].icv_value_sha256;
    return;
}


LOCAL gint32 get_config_file_sha256(const void *request_msg, guint8 *resp_data, guint16 data_len)
{
    errno_t safe_fun_ret = EOK;
    const guint8 *src_data = NULL;
    guint8 resource_id = 0;
    guint8 *icv_value_sha256 = NULL;

    if (request_msg == NULL || resp_data == NULL || data_len < MAX_CMD_STR_LENGTH) {
        debug_log(DLOG_ERROR, "%s: Parameter error", __FUNCTION__);
        return RET_ERR;
    }
    src_data = get_ipmi_src_data(request_msg);
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, "src_data is NULL!\n");
        resp_data[0] = COMP_CODE_LEN_INVALID;
        return resource_respond_msg(request_msg, RESOURCE_MSG_HEAD_LEN, resp_data, data_len);
    }
    resource_id = src_data[RESOURCE_ID_OFFSET];
    (void)get_icv_value_by_resource_id(resource_id, &icv_value_sha256);
    if (icv_value_sha256 == NULL) {
        debug_log(DLOG_ERROR, "get respconfig by resource id fail.resource_id = 0x%x\n", resource_id);
        resp_data[0] = COMP_CODE_UNKNOWN;
        return resource_respond_msg(request_msg, RESOURCE_MSG_HEAD_LEN, resp_data, data_len);
    }
    resp_data[5] = SHA256_CHECKSUM_LENGTH;
    safe_fun_ret = memcpy_s(resp_data + 7, MAX_CMD_STR_LENGTH - 7, icv_value_sha256, resp_data[5]);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    return resource_respond_msg(request_msg, RESOURCE_MSG_SHA256_LEN + resp_data[5], resp_data, data_len);
}


gint32 set_config_file_sha256(guint8 resoure_id, gchar *checksum_file_path)
{
    gint32 ret = 0;
    guint8 temp[SHA256_CHECKSUM_LENGTH*2] = {0};
    guint8 icv_value_sha256[SHA256_CHECKSUM_LENGTH] = {0};
    guint32 count = 0;
    FILE *fp = NULL;

    fp = g_fopen(checksum_file_path, "r");
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "g_fopen file failed!");
        return RET_ERR;
    }
    while (count < SHA256_CHECKSUM_LENGTH * 2) {
        ret = fread(temp + count, 1, 1, fp);
        if (ret < 1) {
            debug_log(DLOG_DEBUG, "fread failed, count:%d", count);
            break;
        }
        count++;
    }
    vos_rm_filepath(checksum_file_path);
    (void)fclose(fp);
    (void)convert_string_to_bytes(temp, SHA256_CHECKSUM_LENGTH * 2, icv_value_sha256);
    // 设置checksum
    (void)set_icv_value_by_resource_id(resoure_id, icv_value_sha256);
    return RET_OK;
}


gint32 get_file_checksum_from_bmc(const void *req_msg, gpointer user_data)
{
    const guint8 *src_data = NULL;
    gint32 ret_val = RET_ERR;
    guint8 sub_cmd = 0;
    guint8 resp_data[MAX_CMD_BUF_LEN] = {0};
    guint8 ser_type = 0;

    if (req_msg == NULL) {
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    (void)bios_get_server_type(&ser_type);

    if (ser_type != SERVER_TYPE_BLADE) {
        debug_log(DLOG_ERROR, "%s:current server type not support this cmd.\n", __FUNCTION__);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }

    src_data = get_ipmi_src_data(req_msg);
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, DB_STR_GET_SRC_DATA_FAIL);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    
    sub_cmd = src_data[RESOURCE_SUB_CMD_OFFSET];

    switch (sub_cmd) {
        case GET_CONFIG_FILE_STATUS:
            ret_val = get_respconfig_file_status(req_msg, resp_data, sizeof(resp_data));
            break;
        case GET_SHA256_VALUE:
            ret_val = get_config_file_sha256(req_msg, resp_data, sizeof(resp_data));
            break;
        default:
            resp_data[0] = COMP_CODE_INVALID_FIELD;
            return resource_respond_msg(req_msg, BIOS_MSG_HEAD_MIN_LEN, resp_data, sizeof(resp_data));
    }
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "SubCmd:%d fail(Ret:0x%x)!\n", sub_cmd, ret_val);
    }
    return ret_val;
}


void resource_init(void)
{
    gint32 ret_val = RET_OK;
    PER_S per_data = { 0 };

    
    g_mutex_init(&g_clpconfig_mutex);

    per_data.key = PER_K_CLP_FILE_BT_FLAG;
    per_data.mode = PER_MODE_RESET;
    per_data.data = &g_is_clpconfig_from_bt;
    per_data.len = sizeof(g_is_clpconfig_from_bt);

    ret_val = per_init(&per_data, 1);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "per_init FOR g_is_clpconfig_from_bt,%s fail!\r\n", PER_K_CLP_FILE_BT_FLAG);
        g_is_clpconfig_from_bt = 0;
    } else {
        debug_log(DLOG_ERROR, "per_init FOR g_is_clpconfig_from_bt,%s ok:%u!\r\n", PER_K_CLP_FILE_BT_FLAG,
            g_is_clpconfig_from_bt);
    }
}
