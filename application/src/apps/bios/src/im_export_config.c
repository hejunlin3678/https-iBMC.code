

#include "im_export_config.h"
#include "boot_options.h"
#include "bios_settings.h"
#include "file_op.h"
#include "bios.h"

#define BIOS_JSON_REGISTRYVERSION "RegistryVersion"
#define BIOS_CONFIG_VALUE_LEN 64
#define PROP_JSON_TYPE_INTEGER 1
#define PROP_JSON_TYPE_STRING 2
#define BIOS_ERR_OFFSET_MSG_LEN 8
#define BIOS_ERR_NORMAL_MSG_LEN 9

#define BIOS_PROP_NONEXIST (-2)

#define DECIMAL_TYPE 10

LOCAL gint32 bios_set_config_json(struct config_record *pconfig, GSList *caller_info, GSList **output_list);
LOCAL gint32 bios_set_config_ini(struct config_record *pconfig, GSList *caller_info, GSList **output_list);
LOCAL gint32 bios_write_config_to_json_object(struct property_record *ppropertyList, json_object **bios_jso);
LOCAL gint32 bios_regist_check_entry(OBJ_HANDLE obj_handle, json_object *object, json_object **out_err_array,
    gboolean check_setting_flag);
LOCAL gint32 bios_config_value_check(gchar *filename, struct property_record *ppropertyList,
    struct property_record **pfailList);
LOCAL gint32 bios_write_config_to_inifile(struct property_record *ppropertyList, gchar *filename);
LOCAL gint32 bios_get_property_value_type(gchar *propertyname, json_object *json_obj);
LOCAL gint32 bios_regist_check(json_object *object, json_object *basic_data, json_object **out_err_array);
LOCAL gint32 bios_check_one_porperty_valid(gchar *value, gchar *string);
LOCAL gchar **find_config_name_from_list(GSList *list, gchar *name);
LOCAL gint32 bios_get_config0_line_list(gchar *filename, GSList **list);
LOCAL gint32 bios_regist_dependency_change(json_object *dependencies, json_object *item_obj_jso,
    json_object *attributes, const gchar *changed_attribute, const gchar *changed_propery, json_bool flag);
LOCAL gint32 bios_read_config_from_jsonfile(gchar *filename, struct property_record **ppropertyList);
LOCAL gint32 bios_read_config_from_inifile(gchar *filename, struct property_record **ppropertyList);
LOCAL gint32 json_one_object_bios_config(const gchar *key, json_object *val, struct property_record **pproperty);
LOCAL gint32 parser_one_line_bios_config(gchar *linebuffer, guint32 buflen, struct property_record **pproperty);
LOCAL void clear_space_tab_in_string(gchar *string_src, gint32 string_len);
LOCAL gchar *get_bios_version(void);
LOCAL gint32 bios_read_version_from_display0(gchar *filename, gchar *version, gint32 sz);
LOCAL gint32 bios_regist_get_current_value(OBJ_HANDLE obj_handle, const gchar *property_name, json_object *regist_data);
LOCAL gint32 bios_regist_dependency_response(json_object *dependencies, json_object *attributes, const gchar *name,
    const gchar *value, json_bool flag);


gint32 bios_set_config(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    struct config_record *pconfig = NULL;
    gint32 ret = 0;

    const gchar *desfile = NULL;
    gsize desfile_string_len = 0;
    gchar filenamebuf[128] = {0};

    guint32 product_version = 0;
    
    if (input_list == NULL) {
        return RET_ERR;
    }

    desfile = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), &desfile_string_len);
    if (desfile == NULL || desfile_string_len == 0) {
        debug_log(DLOG_ERROR, "bios_set_config:invalid file length!\n");
        method_operation_log(caller_info, NULL, "Import BIOS configuration failed");
        return RET_ERR;
    }

    (void)strncpy_s(filenamebuf, sizeof(filenamebuf), desfile, sizeof(filenamebuf) - 1);

    ret = xml_read_file(filenamebuf, &pconfig);
    if (ret == RET_ERR) {
        if (pconfig != NULL) {
            xml_free_all_config_struct(pconfig);
        }
        debug_log(DLOG_ERROR, " xml_read_file fail ! ");
        method_operation_log(caller_info, NULL, "Import BIOS configuration failed");
        return RET_ERR;
    }

    if (pconfig->pcomponentlist == NULL) {
        method_operation_log(caller_info, NULL, "Import BIOS configuration successfully");
        xml_free_all_config_struct(pconfig);
        return RET_OK;
    }

    
    ret = dal_get_x86_platform_version_num(&product_version);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, " dal_get_x86_platform_version_num fail ! ");
        xml_free_all_config_struct(pconfig);
        method_operation_log(caller_info, NULL, "Import BIOS configuration failed");
        return RET_ERR;
    }

    if (product_version >= PRODUCT_VERSION_V5 || g_bios_config_format == 1) {
        ret = bios_set_config_json(pconfig, caller_info, output_list);
    } else {
        ret = bios_set_config_ini(pconfig, caller_info, output_list);
    }
    
    xml_free_all_config_struct(pconfig);
    return ret;
}


LOCAL gint32 bios_set_config_json(struct config_record *pconfig, GSList *caller_info, GSList **output_list)
{
    // V5环境 配置文件导入时 版本不进行判断控制 数据进行检验
    struct property_record *pprop = NULL;
    gint32 ret = 0;
    gchar *pversion = NULL;
    OBJ_HANDLE obj_handle = 0;
    json_object *out_err_array = NULL;
    json_object *tmp_jso = NULL;
    json_object *json_pprop = NULL;
    gchar property_name[BIOS_CONFIG_VALUE_LEN] = {0};

    pversion = get_bios_version();
    if (pversion == NULL) {
        debug_log(DLOG_ERROR, " NULL == pversion ! ");
        method_operation_log(caller_info, NULL, "Import BIOS configuration failed");
        return RET_ERR;
    }

    if (pconfig->pcomponentlist->componetsoftver != NULL) {
        if (strcmp(pversion, pconfig->pcomponentlist->componetsoftver) != 0) {
            debug_log(DLOG_ERROR, " version is diff board(%s), file(%s)", pversion,
                pconfig->pcomponentlist->componetsoftver);
        }
    }

    g_free(pversion);

    pprop = pconfig->pcomponentlist->ppropertylist;

    if (pprop == NULL) {
        (*output_list = g_slist_append(*output_list, g_variant_new_string("OK")));
        debug_log(DLOG_ERROR, " no bios config ! ");
        method_operation_log(caller_info, NULL, "Import BIOS configuration successfully");
        return RET_OK;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_BIOS, 0, &obj_handle);
    if (ret != RET_OK) {
        (*output_list = g_slist_append(*output_list, g_variant_new_string("inner error")));
        debug_log(DLOG_ERROR, " dal_get_object_handle_nth fail ! ");
        method_operation_log(caller_info, NULL, "Import BIOS configuration failed");
        return RET_OK;
    }
    
    ret = bios_write_config_to_json_object(pprop, &json_pprop);
    if (ret != RET_OK) {
        (*output_list = g_slist_append(*output_list, g_variant_new_string("inner error")));
        debug_log(DLOG_ERROR, " bios_write_config_to_json_object fail ! ");
        method_operation_log(caller_info, NULL, "Import BIOS configuration failed");
        return RET_OK;
    }
    
    
    ret = bios_regist_check_entry(obj_handle, json_pprop, &out_err_array, FALSE);
    
    if (ret != RET_OK) {
        // 出错取错误数组第一个进行返回
        // 取错误值失败 返回内部错误
        const gchar *str = dal_json_object_get_str(tmp_jso);
        json_object *array_jso = (out_err_array == NULL) ? NULL : json_object_array_get_idx(out_err_array, 0);
        if (array_jso == NULL ||
            (json_object_object_get_ex(array_jso, REGRIST_PROP_ATTRIBUTENAME, &tmp_jso)) == FALSE || str == NULL ||
            (snprintf_s(property_name, sizeof(property_name), strlen(str), "%s", str) <= 0)) {
            debug_log(DLOG_ERROR, "get prop from out_err_array fail");
            *output_list = g_slist_append(*output_list, g_variant_new_string("inner error"));
        } else {
            *output_list = g_slist_append(*output_list, g_variant_new_string(str));
        }

        (void)json_object_put(json_pprop);
        (void)json_object_put(out_err_array);
        debug_log(DLOG_ERROR, " bios_config_value_check fail ! ");
        method_operation_log(caller_info, NULL, "Import BIOS configuration failed");

        return RET_OK;
    }

    ret = json_object_to_file(TMP_BIOS_SETTING_JSON_FILE_NAME, json_pprop);
    if (ret != RET_OK) {
        *output_list = g_slist_append(*output_list, g_variant_new_string("inner error"));
        (void)json_object_put(json_pprop);
        debug_log(DLOG_ERROR, " json_object_to_file fail!");
        method_operation_log(caller_info, NULL, "Import BIOS configuration failed");
        return RET_OK;
    }

    (void)json_object_put(json_pprop);

    
    (void)copy_bios_file(RESOURCE_BIOS_PROFILE_SETTING, TMP_BIOS_SETTING_JSON_FILE_NAME);

    (void)vos_rm_filepath(TMP_BIOS_SETTING_JSON_FILE_NAME);

    
    set_bios_setting_file_change_flag(BIOS_SETTING_FILE_CHANGED, 0);

    *output_list = g_slist_append(*output_list, g_variant_new_string("OK"));
    method_operation_log(caller_info, NULL, "Import BIOS configuration successfully");

    return RET_OK;
}


LOCAL gint32 bios_set_config_ini(struct config_record *pconfig, GSList *caller_info, GSList **output_list)
{
    struct property_record *pprop = NULL;
    struct property_record *propertyRecord = NULL;
    gint32 ret = 0;
    gchar *pversion = NULL;
    gchar *pconfigversion = NULL;

    gdouble config_version;

    pversion = get_bios_version();
    if (pversion == NULL) {
        debug_log(DLOG_ERROR, " NULL == pversion ! ");
        method_operation_log(caller_info, NULL, "Import BIOS configuration failed");
        return RET_ERR;
    }

    pconfigversion = (gchar *)g_malloc0(CONFIG_VER_LEN);
    if (pconfigversion == NULL) {
        g_free(pversion);
        debug_log(DLOG_ERROR, " NULL == pconfigversion ! ");
        method_operation_log(caller_info, NULL, "Import BIOS configuration failed");
        return RET_ERR;
    }

    ret = bios_read_version_from_display0(BIOS_DISPLAY0_INI_FILE_NAME, pconfigversion, CONFIG_VER_LEN);
    if (ret != RET_OK) {
        g_free(pconfigversion);
        g_free(pversion);
        debug_log(DLOG_ERROR, "%s: bios read version from display failed.", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Import BIOS configuration failed");
        return RET_ERR;
    }

    if (pconfig->pcomponentlist->componetsoftver != NULL) {
        if (strcmp(pversion, pconfig->pcomponentlist->componetsoftver) != 0) {
            debug_log(DLOG_ERROR, " version is diff board(%s), file(%s)", pversion,
                pconfig->pcomponentlist->componetsoftver);
        }

        if ((strcmp(pconfigversion, pconfig->pcomponentlist->componetconfigver) != 0)) {
            debug_log(DLOG_ERROR, " version is diff board(%s), file(%s)", pconfigversion,
                pconfig->pcomponentlist->componetconfigver);
        }
    } else {
        
        g_free(pconfigversion);
        g_free(pversion);
        debug_log(DLOG_ERROR, "BIOS software version is NULL.");
        method_operation_log(caller_info, NULL, "Import BIOS configuration failed");
        return RET_ERR;
    }

    g_free(pversion);

    pprop = pconfig->pcomponentlist->ppropertylist;

    if (pprop == NULL) {
        g_free(pconfigversion);
        *output_list = g_slist_append(*output_list, g_variant_new_string("OK"));
        debug_log(DLOG_ERROR, " no bios config ! ");
        method_operation_log(caller_info, NULL, "Import BIOS configuration successfully");
        return RET_OK;
    }

    
    config_version = strtof(pconfigversion, NULL);
    g_free(pconfigversion);

    if (config_version > CHECK_BIOS_CONFIG_VER) {
        
        ret = bios_config_value_check(BIOS_DISPLAY0_INI_FILE_NAME, pprop, &propertyRecord);
        if (ret == RET_ERR) {
            *output_list = g_slist_append(*output_list, g_variant_new_string(propertyRecord->propertyname));
            debug_log(DLOG_ERROR, " bios_config_value_check fail ! ");
            method_operation_log(caller_info, NULL, "Import BIOS configuration failed");
            return RET_OK;
        }
    } else {
        debug_log(DLOG_ERROR, "current bios configuration file version do not support check.\r\n");
    }

    
    ret = bios_write_config_to_inifile(pprop, TMP_BIOS_CHANGE0_INI_FILE_NAME);
    if (ret == RET_ERR) {
        *output_list = g_slist_append(*output_list, g_variant_new_string("inner error"));
        debug_log(DLOG_ERROR, " bios_write_config_to_inifile fail ! ");
        method_operation_log(caller_info, NULL, "Import BIOS configuration failed");
        return RET_OK;
    }

    
    (void)copy_bios_file(RESOURCE_BIOS_PROFILE_CHANGED, TMP_BIOS_CHANGE0_INI_FILE_NAME);

    (void)vos_rm_filepath(TMP_BIOS_CHANGE0_INI_FILE_NAME);

    
    set_bios_setting_file_change_flag(BIOS_SETTING_FILE_CHANGED, 0);

    *output_list = g_slist_append(*output_list, g_variant_new_string("OK"));
    method_operation_log(caller_info, NULL, "Import BIOS configuration successfully");

    return RET_OK;
}


LOCAL gchar *get_bios_version(void)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = 0;
    gchar *pversion = NULL;

    ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        return NULL;
    }

    pversion = (gchar *)g_malloc0(BIOS_VERSION_LEN);
    if (pversion == NULL) {
        return NULL;
    }

    ret = dal_get_property_value_string(obj_handle, BMC_BIOS_VER_NAME, pversion, BIOS_VERSION_LEN);
    if (ret != RET_OK) {
        g_free(pversion);
        return NULL;
    }

    
    if ((strlen(pversion) == 0) || (strlen(pversion) == 1)) {
        g_free(pversion);
        pversion = NULL;
    }

    return pversion;
}


LOCAL gint32 bios_write_config_to_json_object(struct property_record *ppropertyList, json_object **bios_jso)
{
    gint32 ret = 0;
    gint32 value = 0;
    struct property_record *pproperty = NULL;
    json_object *registry_file = NULL;
    json_object *registry_json = NULL;

    if (ppropertyList == NULL || bios_jso == NULL) {
        debug_log(DLOG_ERROR, " input parameter error ! ");
        return RET_ERR;
    }

    registry_file = json_object_from_file(BIOS_REGISTRY_JSON_FILE_NAME);
    if (registry_file == NULL) {
        debug_log(DLOG_ERROR, "json_object_from_file fail");
        return RET_ERR;
    }

    ret = json_object_object_get_ex(registry_file, REGRIST_PROP_REGISTRYENTRIES, &registry_json);
    if (ret != TRUE) {
        (void)json_object_put(registry_file);
        debug_log(DLOG_ERROR, "json_object_object_get_ex fail");
        return RET_ERR;
    }

    pproperty = ppropertyList;

    *bios_jso = json_object_new_object();
    if (*bios_jso == NULL) {
        (void)json_object_put(registry_file);
        debug_log(DLOG_ERROR, "json_object_new_object fail");
        return RET_ERR;
    }

    while (pproperty) {
        vos_task_delay(DELAY_FORTY_MILLISECONDS); // 优化配置导入CPU占用率
        debug_log(DLOG_DEBUG, " %s %s ", pproperty->propertyname, pproperty->propertyvalue);

        if (pproperty->propertyname == NULL) {
            (void)json_object_put(registry_file);
            return RET_ERR;
        }

        // 根据registry.json中描述的格式，写json对象
        ret = bios_get_property_value_type(pproperty->propertyname, registry_json);
        if (ret == RET_ERR) {
            (void)json_object_put(registry_file);
            debug_log(DLOG_ERROR, "bios_get_property_value_type fail");
            return RET_ERR;
        }
        
        else if (BIOS_PROP_NONEXIST == ret) {
            debug_log(DLOG_ERROR, "Can't find property %s in current BIOS version", pproperty->propertyname);
            pproperty = pproperty->pnextproperty;
            continue;
        }

        

        if (ret == PROP_JSON_TYPE_INTEGER) {
            
            ret = vos_str2int(pproperty->propertyvalue, 10, &value, NUM_TPYE_INT32);
            if (ret != RET_OK) {
                (void)json_object_put(registry_file);
                debug_log(DLOG_ERROR, "bios_get_property_value_type fail");
                return RET_ERR;
            }

            
            json_object_object_add(*bios_jso, pproperty->propertyname, json_object_new_int(value));
        } else if (ret == PROP_JSON_TYPE_STRING) {
            json_object_object_add(*bios_jso, pproperty->propertyname,
                json_object_new_string(pproperty->propertyvalue));
        }

        pproperty = pproperty->pnextproperty;
    }

    (void)json_object_put(registry_file);
    return RET_OK;
}


LOCAL gint32 bios_regist_check_entry(OBJ_HANDLE obj_handle, json_object *object, json_object **out_err_array,
    gboolean check_setting_flag)
{
    gint32 ret = 0;

    json_object *regist_data = NULL;
    json_object *regist_data_entries = NULL;
    json_object *array_item_obj = NULL;

    gchar regist_filename[REGRIST_FILE_NAME_LENGTH] = {0};

    
    if (object == NULL || out_err_array == NULL) {
        debug_log(DLOG_ERROR, "[%s][%d] parameter error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    
    ret = bios_regist_check_boottypeorder(object);
    if (ret != RET_OK) {
        *out_err_array = json_object_new_array();
        array_item_obj = json_object_new_object();

        json_object_object_add(array_item_obj, REGRIST_PROP_ATTRIBUTENAME,
            json_object_new_string(REGRIST_PROP_BOOTTYPEORDER));
        json_object_object_add(array_item_obj, REGRIST_PROP_VALUE, json_object_new_string("N/A"));
        json_object_object_add(array_item_obj, REGRIST_PROP_TYPE,
            json_object_new_int(REGRIST_TYPE_PROP_BOOTTYPEORDER_ERROR));

        
        ret = json_object_array_add(*out_err_array, array_item_obj);
        if (ret != RET_OK) {
            json_object_put(array_item_obj);
        }
        

        debug_log(DLOG_ERROR, "[%s][%d] bios_regist_check_boottypeorder error \n", __FUNCTION__, __LINE__);
        return REGRIST_TYPE_PROP_BOOTTYPEORDER_ERROR;
    }

    (void)dal_get_property_value_string(obj_handle, BIOS_FILE_REGISTRY_NAME, regist_filename, REGRIST_FILE_NAME_LENGTH);
    regist_data = json_object_from_file(regist_filename);

    

    ret = json_object_object_get_ex(regist_data, REGRIST_PROP_REGISTRYENTRIES, &regist_data_entries);
    if (ret != TRUE) {
        debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
        ret = RET_ERR;
        goto ERR_EXIT;
    }

    debug_log(DLOG_DEBUG, "[%s][%d] bios_regist_get_current_value pre \n", __FUNCTION__, __LINE__);
    
    
    ret = bios_regist_get_current_value(obj_handle, BIOS_FILE_CURRENT_VALUE_NAME, regist_data_entries);

    debug_log(DLOG_DEBUG, "[%s][%d] bios_regist_get_current_value next \n", __FUNCTION__, __LINE__);

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
        ret = RET_ERR;
        goto ERR_EXIT;
    }

    
    
    if (TRUE == check_setting_flag) {
        ret = bios_regist_get_current_value(obj_handle, BIOS_FILE_SETTING_NAME, regist_data_entries);

        debug_log(DLOG_DEBUG, "[%s][%d] bios_regist_get_current_value next \n", __FUNCTION__, __LINE__);

        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
            ret = RET_ERR;
            goto ERR_EXIT;
        }
    }
    

    
    ret = bios_regist_check(object, regist_data_entries, out_err_array);

ERR_EXIT:

    json_object_put(regist_data);
    return ret;
    
}


gint32 copy_bios_file(guint8 resource_id, const gchar *file_name)
{
    glong src_len = 0;
    gint32 ret = 0;
    gchar *resolved_path = NULL;

    if (file_name == NULL) {
        debug_log(DLOG_ERROR, "file_name: NULL invalid");
        return RET_ERR;
    }

    if (resource_id >= sizeof(g_file_property_name) / sizeof(g_file_property_name[0]) ||
        g_file_property_name[resource_id] == NULL) {
        debug_log(DLOG_ERROR, "resource_id: %u invalid", resource_id);
        return RET_ERR;
    }

    resolved_path = realpath(file_name, NULL);
    if (resolved_path == NULL) {
        debug_log(DLOG_ERROR, "error: %s!", g_strerror(errno));
        return RET_ERR;
    }

    
    src_len = vos_get_file_length(file_name);
    ret = write_src_content_to_bios_config(resolved_path, "r", src_len, g_file_property_name[resource_id], "w+b");

    g_free(resolved_path);

    return ret;
}


LOCAL gint32 bios_read_version_from_display0(gchar *filename, gchar *version, gint32 sz)
{
    errno_t safe_fun_ret = EOK;
    FILE *fp = NULL;
    gchar **p = NULL;
    gint32 len = 0;
    gchar linebuffer[MAX_BIOS_FILE_LINE_LEN] = {0};

    if ((version == NULL) || (filename == NULL)) {
        debug_log(DLOG_ERROR, " NULL pointer param");
        return RET_ERR;
    }

    fp = fopen(filename, "r");
    if (fp == NULL) {
        debug_log(DLOG_ERROR, " fopen(%s) fail ! ", filename);
        return RET_ERR;
    }

    while (!feof(fp) && vos_fgets(linebuffer, MAX_BIOS_FILE_LINE_LEN, fp) != NULL) {
        if (strlen(linebuffer) < 2) {
            continue;
        }

        if (linebuffer[0] == '/') {
            p = g_strsplit(linebuffer, "=", -1);
            if (p == NULL) {
                debug_log(DLOG_ERROR, " g_strsplit(%s) error ! ", linebuffer);
                (void)fclose(fp);
                return RET_ERR;
            }

            len = g_strv_length(p);
            if (len == 2) {
                safe_fun_ret = strncpy_s(version, sz, p[len - 1], sz - 1);
                if (safe_fun_ret != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                }
                g_strfreev(p);

                version[strlen(version) - 1] = '\0'; // 删除结尾的换行符
                g_strstrip(version);                 // 去掉首尾的空格
                (void)fclose(fp);
                return RET_OK;
            }

            g_strfreev(p);
        }
    }

    (void)fclose(fp);
    return RET_ERR;
}


LOCAL gint32 bios_config_value_check(gchar *filename, struct property_record *ppropertyList,
    struct property_record **pfailList)
{
    struct property_record *pproperty = NULL;
    GSList *bioslist = NULL;
    gint32 ret = 0;
    gchar **p = NULL;

    if ((ppropertyList == NULL) || (filename == NULL) || (pfailList == NULL)) {
        debug_log(DLOG_ERROR, " input parameter error ! ");
        return RET_ERR;
    }

    ret = bios_get_config0_line_list(filename, &bioslist);
    if (ret == RET_ERR) {
        debug_log(DLOG_ERROR, " bios_get_config0_line_list fail ! %s", filename);
        goto error;
    }

    pproperty = ppropertyList;

    while (pproperty) {
        debug_log(DLOG_DEBUG, " %s %s ", pproperty->propertyname, pproperty->propertyvalue);

        p = find_config_name_from_list(bioslist, pproperty->propertyname);
        if (p == NULL) {
            
            debug_log(DLOG_ERROR, "Can't find BIOS configuration %s in current BIOS version", pproperty->propertyname);
            pproperty = pproperty->pnextproperty;
            continue;
            
        }

        debug_log(DLOG_DEBUG, " %s %s ", p[0], p[2]);

        ret = bios_check_one_porperty_valid(pproperty->propertyvalue, p[2]);
        if (ret == RET_ERR) {
            debug_log(DLOG_ERROR, " bios_check_one_porperty_valid(%s) fail ! ", pproperty->propertyname);
            *pfailList = pproperty;
            goto error;
        }

        pproperty = pproperty->pnextproperty;
    }

    g_slist_free_full(bioslist, (GDestroyNotify)g_strfreev);

    return RET_OK;

error:

    if (bioslist != NULL) {
        g_slist_free_full(bioslist, (GDestroyNotify)g_strfreev);
    }

    return RET_ERR;
}


LOCAL gint32 bios_write_config_to_inifile(struct property_record *ppropertyList, gchar *filename)
{
    gint32 iRet = -1;
    FILE *fp = NULL;
    struct property_record *pproperty = NULL;
    gchar buf[MAX_BIOS_FILE_LINE_LEN] = {0};
    GSList *bioslist = NULL;
    gint32 ret = 0;
    gchar **p = NULL;

    if ((ppropertyList == NULL) || (filename == NULL)) {
        debug_log(DLOG_ERROR, " input parameter error ! ");
        return RET_ERR;
    }

    fp = fopen(filename, "w");
    if (fp == NULL) {
        debug_log(DLOG_ERROR, " fopen(%s) fail ! ", filename);
        return RET_ERR;
    }

    
    (void)fchmod(fileno(fp), 0640);
    

    ret = bios_get_config0_line_list(BIOS_OPTIONS0_INI_FILE_NAME, &bioslist);
    if (ret == RET_ERR || bioslist == NULL) {
        debug_log(DLOG_ERROR, " bios_get_config0_line_list fail ! ");
        (void)fclose(fp);
        g_slist_free_full(bioslist, (GDestroyNotify)g_strfreev);
        return RET_ERR;
    }

    (void)fwrite(BIOS_CONFIG_HEAD_LINE, strlen(BIOS_CONFIG_HEAD_LINE), 1, fp);
    pproperty = ppropertyList;

    while (pproperty) {
        debug_log(DLOG_DEBUG, " %s %s ", pproperty->propertyname, pproperty->propertyvalue);
        if (pproperty->propertyname == NULL) {
            goto error;
        }

        p = find_config_name_from_list(bioslist, pproperty->propertyname);
        if (p == NULL) {
            debug_log(DLOG_ERROR, "Can not find property name : %s.\r\n", pproperty->propertyname);
            pproperty = pproperty->pnextproperty;
            continue;
        }

        iRet = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "%s    %s    %s    %s    %s", pproperty->propertyname,
            p[1], p[2], pproperty->propertyvalue, p[4]);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
        }

        (void)fwrite(buf, strlen(buf), 1, fp);
        pproperty = pproperty->pnextproperty;
    }

    (void)fclose(fp);
    g_slist_free_full(bioslist, (GDestroyNotify)g_strfreev);

    return RET_OK;

error:

    (void)fclose(fp);

    if (bioslist != NULL) {
        g_slist_free_full(bioslist, (GDestroyNotify)g_strfreev);
    }

    return RET_ERR;
}


LOCAL gint32 bios_get_property_value_type(gchar *propertyname, json_object *json_obj)
{
    json_bool ret_bool = FALSE;
    json_object *attributes_jso = NULL;
    json_object *attributes_item = NULL;
    json_object *prop_jso = NULL;
    json_object *tmp_jso = NULL;
    const gchar *prop_str = NULL;
    const gchar *type_str = NULL;
    gint32 array_index = 0;
    gint32 array_cnt = 0;

    ret_bool = json_object_object_get_ex(json_obj, REGRIST_PROP_ATTRIBUTES, &attributes_jso);
    if (ret_bool != TRUE) {
        debug_log(DLOG_ERROR, "json_object_object_get_ex fail");
        return RET_ERR;
    }

    array_cnt = json_object_array_length(attributes_jso);

    for (array_index = 0; array_index < array_cnt; array_index++) {
        attributes_item = json_object_array_get_idx(attributes_jso, array_index);
        if (attributes_item == NULL) {
            debug_log(DLOG_ERROR, "json_object_array_get_idx fail");
            return RET_ERR;
        }

        ret_bool = json_object_object_get_ex(attributes_item, REGRIST_PROP_ATTRIBUTENAME, &prop_jso);
        if (ret_bool != TRUE) {
            debug_log(DLOG_ERROR, "json_object_object_get_ex fail");
            return RET_ERR;
        }

        prop_str = dal_json_object_get_str(prop_jso);
        if (prop_str == NULL) {
            debug_log(DLOG_ERROR, "dal_json_object_get_str fail");
            return RET_ERR;
        }

        if (g_strcmp0(propertyname, prop_str) == 0) {
            // 在registry.json中找到 取出Type
            ret_bool = json_object_object_get_ex(attributes_item, REGRIST_PROP_TYPE, &tmp_jso);
            if (ret_bool != TRUE) {
                debug_log(DLOG_ERROR, "json_object_object_get_ex fail");
                return RET_ERR;
            }

            type_str = dal_json_object_get_str(tmp_jso);
            if (type_str == NULL) {
                debug_log(DLOG_ERROR, "dal_json_object_get_str fail");
                return RET_ERR;
            }

            // 数据类型为整形 或 字符串
            if (g_strcmp0(type_str, REGRIST_PROP_TYPE_INTEGER) == 0) {
                return PROP_JSON_TYPE_INTEGER;
            } else {
                return PROP_JSON_TYPE_STRING;
            }
        }
    }

    
    return BIOS_PROP_NONEXIST;
    
}


LOCAL gint32 bios_regist_get_current_value(OBJ_HANDLE obj_handle, const gchar *property_name, json_object *regist_data)
{
    gint32 ret = RET_OK;
    gint32 index_id = 0;
    gint32 length = 0;
    gchar  current_filename[REGRIST_FILE_NAME_LENGTH] = {0};
    json_object *file_obj = NULL;
    json_object *attributes_obj = NULL;
    json_object *name_value_jso = NULL;
    json_object *item_value_jso = NULL;
    json_object *current_jso = NULL;
    json_object *dependencies_jso = NULL;

    if (regist_data == NULL) {
        debug_log(DLOG_ERROR, "[%s][%d]  parameter error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    if (json_object_object_get_ex(regist_data, REGRIST_PROP_ATTRIBUTES, &attributes_obj) == FALSE) {
        debug_log(DLOG_ERROR, "[%s][%d]  json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    if (json_object_object_get_ex(regist_data, REGRIST_PROP_DEPENDENCIES, &dependencies_jso) == FALSE) {
        debug_log(DLOG_ERROR, "[%s][%d]  json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    length = json_object_array_length(attributes_obj);

    (void)dal_get_property_value_string(obj_handle, property_name, current_filename, REGRIST_FILE_NAME_LENGTH);

    
    if (vos_get_file_accessible(current_filename) != TRUE || vos_get_file_length(current_filename) == 0) {
        ret = (g_strcmp0(property_name, BIOS_FILE_SETTING_NAME) == 0) ? RET_OK : RET_ERR;
        debug_log(DLOG_DEBUG, "[%s][%d]  %s not exist \n", __FUNCTION__, __LINE__, property_name);
        return ret;
    }
    

    file_obj = json_object_from_file(current_filename);
    if (file_obj != NULL) {
        for (index_id = 0; index_id < length; index_id++) {
            item_value_jso = json_object_array_get_idx(attributes_obj, index_id);
            if (item_value_jso == NULL) {
                continue;
            };

            (void)json_object_object_get_ex(item_value_jso, REGRIST_PROP_ATTRIBUTENAME, &name_value_jso);

            if (name_value_jso == NULL) {
                continue;
            };

            (void)json_object_object_get_ex(file_obj, dal_json_object_get_str(name_value_jso), &current_jso);

            if (current_jso == NULL) {
                continue;
            };

            

            json_object_object_add(item_value_jso, REGRIST_PROP_CURRENTVALUE, json_object_get(current_jso));

            debug_log(DLOG_DEBUG, "[%s][%d] name %s  value %s", __FUNCTION__, __LINE__,
                dal_json_object_get_str(name_value_jso), dal_json_object_get_str(current_jso));

            

            (void)bios_regist_dependency_response(dependencies_jso, attributes_obj,
                dal_json_object_get_str(name_value_jso), "CurrentValue", FALSE);

            item_value_jso = NULL;

            name_value_jso = NULL;

            current_jso = NULL;
        }
    } else {
        debug_log(DLOG_ERROR, "[%s][%d] json_object_from_file error ", __FUNCTION__, __LINE__);
        ret = RET_ERR;
    }

    json_object_put(file_obj);

    return ret;
}



LOCAL gint32 bios_regist_check(json_object *object, json_object *basic_data, json_object **out_err_array)
{
    json_object *attributes = NULL;
    json_object *dependencies = NULL;
    json_object *array_item_obj = NULL;
    json_object *tmp_object = NULL;
    const gchar *object_resource = NULL;
    gint32 ret;
    gint32 type_ret;
    json_bool loop_flag = TRUE;
    json_bool error_info_flag = FALSE;

    
    if (object == NULL || basic_data == NULL || out_err_array == NULL) {
        debug_log(DLOG_ERROR, "[%s][%d] parameter error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    
    if (json_type_object != json_object_get_type(object)) {
        debug_log(DLOG_ERROR, "[%s][%d] type error  \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    
    gint32 object_length = json_object_object_length(object);
    if (object_length == 0) {
        debug_log(DLOG_ERROR, "[%s][%d] object_length error  \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    
    if (TRUE != json_object_object_get_ex(basic_data, REGRIST_PROP_ATTRIBUTES, &attributes) ||
        TRUE != json_object_object_get_ex(basic_data, REGRIST_PROP_DEPENDENCIES, &dependencies)) {
        debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error  \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    
    *out_err_array = json_object_new_array();

    if (*out_err_array == NULL) {
        debug_log(DLOG_ERROR, "[%s][%d] json_object_new_array error  \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    
    object_resource = dal_json_object_get_str(object);
    tmp_object = json_tokener_parse(object_resource);

    
    while (TRUE == loop_flag) {
        loop_flag = FALSE;
        // 此处用于消除foreach编译告警"assignment discards 'const' qualifier from pointer target type"
#pragma GCC diagnostic push // require GCC 4.6
#pragma GCC diagnostic ignored "-Wcast-qual"

        
        json_object_object_foreach0(tmp_object, name, val)
        {
            type_ret = bios_regist_check_object(attributes, name, val, EXPORT_FILE_FALSE);

            debug_log(DLOG_DEBUG, "[%s][%d]name:[%s], type_ret:[%d] \n", __FUNCTION__, __LINE__, name,
                type_ret); // test

            if (type_ret != RET_OK) {
                if (TRUE == error_info_flag) {
                    array_item_obj = NULL;
                    array_item_obj = json_object_new_object();
                    if (array_item_obj == NULL) {
                        debug_log(DLOG_ERROR, "[%s][%d] json_object_new_object error  \n", __FUNCTION__, __LINE__);
                        continue;
                    }

                    json_object_get(val);
                    json_object_object_add(array_item_obj, REGRIST_PROP_ATTRIBUTENAME, json_object_new_string(name));
                    json_object_object_add(array_item_obj, REGRIST_PROP_VALUE, val);
                    json_object_object_add(array_item_obj, REGRIST_PROP_TYPE, json_object_new_int(type_ret));

                    ret = json_object_array_add(*out_err_array, array_item_obj);
                    if (ret != RET_OK) {
                        debug_log(DLOG_ERROR, "[%s][%d] json_object_array_add error  \n", __FUNCTION__, __LINE__);
                        json_object_put(array_item_obj);
                    }

                    goto ERR_EXIT;
                }
            } else {
                // 有检测可以设置成功的，则需对设置不成功的重新校验
                loop_flag = TRUE;
                
                bios_regist_dependency_response(dependencies, attributes, name, REGRIST_PROP_CURRENTVALUE, TRUE);
                json_object_object_del(tmp_object, name);
            }
        }

        // 如果 FALSE == loop_flag&&error_info_flag == FALSE说明，已经无可以设置属性，再循环一次生成错误消息
        if (FALSE == loop_flag && FALSE == error_info_flag) {
            loop_flag = TRUE;
            // error_info_flag 为TRUE;去尝试生成消息体
            error_info_flag = TRUE;
        }

#pragma GCC diagnostic pop // require GCC 4.6
    }

ERR_EXIT:

    json_object_put(tmp_object);

    
    if (json_object_array_length(*out_err_array) != 0) {
        return RET_ERR;
    }

    json_object_put(*out_err_array);
    *out_err_array = NULL;
    return RET_OK;
}


gint32 bios_check_one_porperty_valid(gchar *value, gchar *string)
{
    gchar **p = NULL;
    gchar **p2 = NULL;
    gint32 ulvalue = 0;
    gint32 i = 0;

    if (value == NULL || string == NULL) {
        debug_log(DLOG_ERROR, " input parameter error ! ");
        return RET_ERR;
    }

    ulvalue = strtol(value, NULL, DECIMAL_TYPE);
    
    if ((ulvalue == 0) && (strlen(value) != 1 || value[0] != '0')) {
        debug_log(DLOG_ERROR, " value(%s) error ! ", value);
        return RET_ERR;
    }

    
    if (string[0] == '$') {
        p = g_strsplit(&string[1], "~", 2);
        if (p == NULL) {
            debug_log(DLOG_ERROR, " g_strsplit failed ! ");
            return RET_ERR;
        }

        if (p[0] == NULL || p[1] == NULL) {
            debug_log(DLOG_ERROR, " g_strsplit failed ! ");
            g_strfreev(p);
            return RET_ERR;
        }

        if ((ulvalue < strtol(p[0], NULL, DECIMAL_TYPE)) || (ulvalue > strtol(p[1], NULL, DECIMAL_TYPE))) {
            debug_log(DLOG_ERROR, " %d not in [%s, %s] ", ulvalue, p[0], p[1]);
            g_strfreev(p);

            

            if (ulvalue == 0) {
                debug_log(DLOG_ERROR, " value(%s) is default ! ", value);
                return RET_OK;
            }
            return RET_ERR;
        }

        g_strfreev(p);
        return RET_OK;
    }
    
    else {
        p = g_strsplit(&string[0], "/", 10);
        if (p == NULL) {
            debug_log(DLOG_ERROR, " g_strsplit failed ! ");
            return RET_ERR;
        }

        while (p[i] != NULL && i < 10) {
            p2 = g_strsplit(p[i], ":", 2);
            if (p2 == NULL) {
                debug_log(DLOG_ERROR, " g_strsplit failed ! ");
                g_strfreev(p);
                return RET_ERR;
            }

            if (strtol(p2[0], NULL, DECIMAL_TYPE) == ulvalue) {
                g_strfreev(p);
                g_strfreev(p2);
                return RET_OK;
            }

            g_strfreev(p2);
            i++;
        }

        g_strfreev(p);
    }

    return RET_ERR;
}


gchar **find_config_name_from_list(GSList *list, gchar *name)
{
    GSList *iterator = NULL;
    gchar **p = NULL;

    if ((list == NULL) || (name == NULL)) {
        debug_log(DLOG_ERROR, " input parameter error ! ");
        return NULL;
    }

    for (iterator = list; iterator; iterator = iterator->next) {
        p = (gchar **)iterator->data;

        if (strcmp(p[0], name) == 0) {
            return (gchar **)iterator->data;
        }
    }

    return NULL;
}


gint32 bios_get_config0_line_list(gchar *filename, GSList **list)
{
    FILE *fp = NULL;
    gchar **p = NULL;
    GSList *plist = NULL;
    gchar linebuffer[MAX_BIOS_FILE_LINE_LEN] = {0};

    fp = fopen(filename, "r");
    if (fp == NULL) {
        debug_log(DLOG_ERROR, " fopen(%s) fail ! ", filename);
        return RET_ERR;
    }

    while (!feof(fp) && vos_fgets(linebuffer, MAX_BIOS_FILE_LINE_LEN, fp) != NULL) {
        if (strlen(linebuffer) < 2) {
            continue;
        }

        if (linebuffer[0] == '/') {
            continue;
        }

        if (linebuffer[0] == '[') {
            continue;
        }

        p = g_strsplit(linebuffer, "    ", 5);
        if (p == NULL) {
            debug_log(DLOG_ERROR, " g_strsplit(%s) error ! ", linebuffer);

            if (plist != NULL) {
                g_slist_free_full(plist, (GDestroyNotify)g_strfreev);
            }

            (void)fclose(fp);
            return RET_ERR;
        }

        plist = g_slist_append(plist, p);
    }

    *list = plist;

    (void)fclose(fp);
    return RET_OK;
}


LOCAL gint32 bios_regist_dependency_response(json_object *dependencies, json_object *attributes,
    const gchar *changed_attribute, const gchar *changed_propery, json_bool flag)
{
    gint32 dependencies_length = 0;
    gint32 ret = 0;
    gint32 index_id;
    json_object *item_obj_jso = NULL;
    json_object *prop_value_jso = NULL;
    const gchar *value_str = NULL;
    static gint32 depth_value = 0;

    depth_value++;

    if (depth_value > 10 || depth_value < 0) {
        depth_value--;
        debug_log(DLOG_ERROR, "%s, %d: depth_valueerror.\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    if (dependencies == NULL || attributes == NULL || changed_attribute == NULL || changed_propery == NULL) {
        debug_log(DLOG_ERROR, "[%s][%d]  parameter error \n", __FUNCTION__, __LINE__);
        depth_value--;
        return RET_ERR;
    }

    
    dependencies_length = json_object_array_length(dependencies);

    for (index_id = 0; index_id < dependencies_length; index_id++) {
        item_obj_jso = NULL;
        item_obj_jso = json_object_array_get_idx(dependencies, index_id);
        if (item_obj_jso == NULL) {
            depth_value--;
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }
        ret = json_object_object_get_ex(item_obj_jso, REGRIST_PROP_DEPENDENCYFOR, &prop_value_jso);
        if (ret == FALSE) {
            depth_value--;
            debug_log(DLOG_ERROR, "[%s][%d] name error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }
        value_str = dal_json_object_get_str(prop_value_jso);
        if (value_str == NULL) {
            depth_value--;
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }
        
        if (g_strcmp0(value_str, changed_attribute) == 0) {
            (void)bios_regist_dependency_change(dependencies, item_obj_jso, attributes, changed_attribute,
                changed_propery, flag);
        }
    }

    depth_value--;
    return RET_OK;
}


LOCAL gint32 bios_regist_dependency_change(json_object *dependencies, json_object *item_obj_jso,
    json_object *attributes, const gchar *changed_attribute, const gchar *changed_propery, json_bool flag)
{
    gint32 ret = 0;
    gint32 index_id = 0;
    gint32 mapfrom_length;
    gint32 set_flag = 0;

    json_object *prop_value_jso = NULL;
    json_object *dependency_jso = NULL;
    json_object *mapfrom_item_jso = NULL;
    json_object *mapfrom_item_prop_jso = NULL;
    json_object *compare_prop_jso = NULL;
    json_object *mapfromvalue_jso = NULL;

    const gchar *map_name = NULL;
    const gchar *map_prop = NULL;
    const gchar *mapterm = NULL;
    const gchar *condtion = NULL;

    const gchar *set_map_name = NULL;
    const gchar *set_map_prop = NULL;
    json_object *set_object = NULL;
    json_object *attributes_item_obj = NULL;
    guint8 change_effected = FALSE; // 变更的数据是否影响依赖项

    if (item_obj_jso == NULL || attributes == NULL || changed_attribute == NULL || changed_propery == NULL) {
        debug_log(DLOG_ERROR, "[%s][%d]  parameter error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    ret = json_object_object_get_ex(item_obj_jso, REGRIST_PROP_DEPENDENCY, &dependency_jso);
    if (ret == FALSE) {
        debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    ret = json_object_object_get_ex(dependency_jso, REGRIST_PROP_MAPFORM, &prop_value_jso);
    if (ret == FALSE) {
        debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    mapfrom_length = json_object_array_length(prop_value_jso);

    for (index_id = 0; index_id < mapfrom_length; index_id++) {
        mapfrom_item_jso = NULL;
        mapfrom_item_jso = json_object_array_get_idx(prop_value_jso, index_id);
        if (mapfrom_item_jso == NULL) {
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }

        
        ret = json_object_object_get_ex(mapfrom_item_jso, REGRIST_PROP_MAPFORMATTRIBUTE, &mapfrom_item_prop_jso);
        if (ret == FALSE) {
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }

        map_name = dal_json_object_get_str(mapfrom_item_prop_jso);
        if (map_name == NULL) {
            debug_log(DLOG_ERROR, "[%s][%d] dal_json_object_get_str error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }
        mapfrom_item_prop_jso = NULL;

        
        ret = json_object_object_get_ex(mapfrom_item_jso, REGRIST_PROP_MAPFROMPROPERTY, &mapfrom_item_prop_jso);
        if (ret == FALSE) {
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }

        map_prop = dal_json_object_get_str(mapfrom_item_prop_jso);
        if (map_prop == NULL) {
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }
        mapfrom_item_prop_jso = NULL;
        
        if (strcmp(map_prop, changed_propery) == 0 && strcmp(map_name, changed_attribute) == 0) {
            change_effected = TRUE;
        }
        
        ret = json_object_object_get_ex(mapfrom_item_jso, REGRIST_PROP_MAPTERMS, &mapfrom_item_prop_jso);
        if (ret == FALSE) {
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }

        mapterm = dal_json_object_get_str(mapfrom_item_prop_jso);
        if (mapterm == NULL) {
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }
        mapfrom_item_prop_jso = NULL;

        
        ret = json_object_object_get_ex(mapfrom_item_jso, REGRIST_PROP_MAPFROMCONDITION, &mapfrom_item_prop_jso);
        if (ret == FALSE) {
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }

        condtion = dal_json_object_get_str(mapfrom_item_prop_jso);
        if (condtion == NULL) {
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }
        mapfrom_item_prop_jso = NULL;

        
        ret = json_object_object_get_ex(mapfrom_item_jso, REGRIST_PROP_MAPFROMVALUE, &mapfromvalue_jso);
        if (ret == FALSE) {
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }

        
        ret = bios_regist_get_value(attributes, map_name, map_prop, &compare_prop_jso, NULL);
        if (ret == RET_ERR) {
            debug_log(DLOG_ERROR, "[%s][%d] bios_regist_get_value error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }
        mapfrom_item_prop_jso = NULL;

        
        ret = bios_regist_compare_object(mapfromvalue_jso, compare_prop_jso, condtion);
        debug_log(DLOG_MASS, "[%s][%d] MapFrom%d:%s.%s %s %s %s", __FUNCTION__, __LINE__, index_id, map_name, map_prop,
            condtion, dal_json_object_get_str(mapfromvalue_jso), mapterm);

        if (g_ascii_strcasecmp(mapterm, REGRIST_STRING_AND) == 0) {
            set_flag = RET_OK;
            if (ret != RET_OK) {
                set_flag = RET_ERR;
                break;
            }
        } else if (g_ascii_strcasecmp(mapterm, REGRIST_STRING_OR) == 0) {
            set_flag = RET_ERR;
            if (ret == RET_OK) {
                set_flag = RET_OK;
                break;
            }
        }
    }
    
    if (set_flag == RET_OK && change_effected == TRUE) {
        
        ret = json_object_object_get_ex(dependency_jso, REGRIST_PROP_MAPTOATTRIBUTE, &mapfrom_item_prop_jso);
        if (ret == FALSE) {
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }

        set_map_name = dal_json_object_get_str(mapfrom_item_prop_jso);
        if (map_name == NULL) {
            debug_log(DLOG_ERROR, "[%s][%d] dal_json_object_get_str error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }
        mapfrom_item_prop_jso = NULL;

        
        ret = json_object_object_get_ex(dependency_jso, REGRIST_PROP_MAPTOPROPERTY, &mapfrom_item_prop_jso);
        if (ret == FALSE) {
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }

        set_map_prop = dal_json_object_get_str(mapfrom_item_prop_jso);
        if (map_prop == NULL) {
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }

        
        ret = bios_regist_get_value(attributes, set_map_name, set_map_prop, NULL, &attributes_item_obj);
        if (ret == RET_ERR) {
            debug_log(DLOG_ERROR, "[%s][%d] bios_regist_get_value error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }

        
        ret = json_object_object_get_ex(dependency_jso, REGRIST_PROP_MAPTOVALUE, &set_object);
        if (ret == FALSE) {
            debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
            return RET_ERR;
        }

        
        json_object_object_add(attributes_item_obj, set_map_prop, json_object_get(set_object));

        debug_log(DLOG_DEBUG, "[%s][%d] update [%s]  [%s]  success \n", __FUNCTION__, __LINE__, set_map_name,
            set_map_prop);

        
        
        if (TRUE == flag) {
            bios_regist_dependency_response(dependencies, attributes, set_map_name, set_map_prop, TRUE);
        }

        
    } else {
        debug_log(DLOG_MASS, "[%s][%d] %s.%s is changed but has no effect. set_flag=%d, effect_flag=%d ", __FUNCTION__,
            __LINE__, changed_attribute, changed_propery, set_flag, change_effected);
    }

    return RET_OK;
}


gint32 bios_get_config(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 iRet = -1;
    struct property_record *pprop = NULL;
    struct component_record *pcom = NULL;
    struct config_record *pconfig = NULL;
    struct head_record *phead = NULL;
    gchar *pname = NULL;
    gint32 ret = 0;

    const gchar *desfile = NULL;
    gsize desfile_string_len = 0;
    gchar filenamebuf[128] = {0};
    gchar *ptimestamp = NULL;
    gchar *pconfigversion = NULL;
    gchar *pbiosversion = NULL;
    struct tm local_time = { 0 };
    gchar* wday[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    guint32 product_version = 0;

    
    if (input_list == NULL) {
        return RET_ERR;
    }

    desfile = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), &desfile_string_len);
    if (desfile == NULL || desfile_string_len == 0) {
        debug_log(DLOG_ERROR, "bios_get_config:invalid file length!\n");
        method_operation_log(caller_info, NULL, "Export BIOS configuration failed");
        return RET_ERR;
    }

    (void)strncpy_s(filenamebuf, sizeof(filenamebuf), desfile, sizeof(filenamebuf) - 1);

    
    
    
    ret = dal_get_x86_platform_version_num(&product_version);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, " dal_get_x86_platform_version_num fail ! ");
        method_operation_log(caller_info, NULL, "Export BIOS configuration failed");
        return RET_ERR;
    }

    if ((product_version >= PRODUCT_VERSION_V5 || g_bios_config_format == 1) &&
        (vos_get_file_accessible(BIOS_CURRENTVALUE_JSON_FILE_NAME) == TRUE)) {
        
        ret = bios_read_config_from_jsonfile(BIOS_CURRENTVALUE_JSON_FILE_NAME, &pprop);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, " bios_read_config_from_jsonfile fail ! ");
            method_operation_log(caller_info, NULL, "Export BIOS configuration failed");
            return RET_ERR;
        }
    }
    
    
    
    else if (TRUE == vos_get_file_accessible(BIOS_OPTIONS0_INI_FILE_NAME)) {
        
        ret = bios_read_config_from_inifile(BIOS_OPTIONS0_INI_FILE_NAME, &pprop);
        if (ret == RET_ERR) {
            debug_log(DLOG_ERROR, " bios_read_config_from_inifile fail ! ");
            method_operation_log(caller_info, NULL, "Export BIOS configuration failed");
            return RET_ERR;
        }
    } else {
        debug_log(DLOG_ERROR, "BIOS configuration file (%s) do not exist.\r\n", BIOS_OPTIONS0_INI_FILE_NAME);
    }

    pname = (gchar *)g_malloc0(strlen(COMPONENT_NAME_BIOS) + 1);
    if (pname == NULL) {
        goto end;
    }

    (void)strncpy_s(pname, strlen(COMPONENT_NAME_BIOS) + 1, COMPONENT_NAME_BIOS, strlen(COMPONENT_NAME_BIOS));

    phead = (struct head_record *)g_malloc0(sizeof(struct head_record));
    if (phead == NULL) {
        goto end;
    }

    ret = dal_localtime_r(NULL, &local_time);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, " localtime_r fail ! ");
        goto end;
    }

    ptimestamp = (gchar *)g_malloc0(TIMESTAMP_LEN);
    if (ptimestamp == NULL) {
        goto end;
    }

    iRet = snprintf_s(ptimestamp, TIMESTAMP_LEN, TIMESTAMP_LEN - 1, "%04d-%02d-%02d %s %02d:%02d:%02d",
        (1900 + local_time.tm_year), (1 + local_time.tm_mon), local_time.tm_mday, wday[local_time.tm_wday],
        local_time.tm_hour, local_time.tm_min, local_time.tm_sec);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
    }
    phead->timestamp = ptimestamp;

    pconfigversion = (gchar *)g_malloc0(CONFIG_VER_LEN);
    if (pconfigversion == NULL) {
        goto end;
    }

    
    
    if (PRODUCT_VERSION_V5 <= product_version || g_bios_config_format == 1) {
        ret = bios_read_version_from_registry(BIOS_FILE_REGISTRY_NAME, pconfigversion, CONFIG_VER_LEN);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Failed to read version from registry.json.\n");
            (void)memset_s(pconfigversion, CONFIG_VER_LEN, 0, CONFIG_VER_LEN);
            goto end;
        }
    } else {
        
        ret = bios_read_version_from_display0(BIOS_DISPLAY0_INI_FILE_NAME, pconfigversion, CONFIG_VER_LEN);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Failed to read version from display0.ini.\n");
            (void)memset_s(pconfigversion, CONFIG_VER_LEN, 0, CONFIG_VER_LEN);
            goto end;
        }
    }

    pcom = (struct component_record *)g_malloc0(sizeof(struct component_record));
    if (pcom == NULL) {
        goto end;
    }

    pconfig = (struct config_record *)g_malloc0(sizeof(struct config_record));
    if (pconfig == NULL) {
        goto end;
    }

    pbiosversion = get_bios_version();
    if (pbiosversion == NULL) {
        goto end;
    }

    pcom->ppropertylist = pprop;
    pcom->componentname = pname;
    pcom->componetsoftver = pbiosversion;
    pcom->componetconfigver = pconfigversion;
    pconfig->pcomponentlist = pcom;
    pconfig->phead = phead;

    
    ret = xml_write_file(pconfig, filenamebuf);
    /*lint -save -e429 */

    
    
    (void)chown(filenamebuf, 0, 0);
    (void)chmod(filenamebuf, S_IRUSR | S_IRGRP);
    

    if (ret == RET_ERR) {
        debug_log(DLOG_ERROR, " xml_write_file fail ! ");
        goto end;
    }

    xml_free_all_config_struct(pconfig);

    method_operation_log(caller_info, NULL, "Export BIOS configuration successfully");

    return RET_OK;
    /*lint -restore*/

end:
    method_operation_log(caller_info, NULL, "Export BIOS configuration failed");

    if (pname != NULL) {
        g_free(pname);
    }

    if (phead != NULL) {
        g_free(phead);
    }

    if (ptimestamp != NULL) {
        g_free(ptimestamp);
    }

    if (pconfigversion != NULL) {
        g_free(pconfigversion);
    }

    if (pbiosversion != NULL) {
        g_free(pbiosversion);
    }

    if (pcom != NULL) {
        g_free(pcom);
    }

    if (pconfig != NULL) {
        g_free(pconfig);
    }

    if (pprop != NULL) {
        xml_free_property_struct_list(pprop);
    }

    return RET_ERR;
}


LOCAL gint32 bios_read_config_from_jsonfile(gchar *filename, struct property_record **ppropertyList)
{
    gint32 ret = 0;
    struct property_record *pproperty = NULL;
    struct property_record *plisthead = NULL;
    struct property_record *plastproperty = NULL;

    json_object *file_jso = NULL;

    OBJ_HANDLE obj_handle = 0;
    json_object *registry_file = NULL;
    json_object *registry_json = NULL;
    json_object *attributes = NULL;
    json_object *dependencies = NULL;
    json_bool delete_err_flag = FALSE;
    json_bool loop_flag = TRUE;
    const gchar *object_resource = NULL;
    json_object *tmp_object = NULL;

    if (filename == NULL || ppropertyList == NULL) {
        debug_log(DLOG_ERROR, " input parameter error ! ");
        return RET_ERR;
    }

    registry_file = json_object_from_file(BIOS_REGISTRY_JSON_FILE_NAME);
    if (registry_file == NULL) {
        debug_log(DLOG_ERROR, "[%s][%d] json_object_from_file error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    ret = json_object_object_get_ex(registry_file, REGRIST_PROP_REGISTRYENTRIES, &registry_json);
    if (ret != TRUE) {
        (void)json_object_put(registry_file);
        debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    
    (void)dal_get_object_handle_nth(CLASS_NAME_BIOS, 0, &obj_handle);
    
    ret = bios_regist_get_current_value(obj_handle, BIOS_FILE_CURRENT_VALUE_NAME, registry_json);
    
    if (ret != RET_OK) {
        (void)json_object_put(registry_file);
        debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    
    if (TRUE != json_object_object_get_ex(registry_json, REGRIST_PROP_ATTRIBUTES, &attributes) ||
        TRUE != json_object_object_get_ex(registry_json, REGRIST_PROP_DEPENDENCIES, &dependencies)) {
        (void)json_object_put(registry_file);
        debug_log(DLOG_ERROR, "[%s][%d] json_object_object_get_ex error \n", __FUNCTION__, __LINE__);

        return RET_ERR;
    }

    
    file_jso = json_object_from_file(filename);
    if (file_jso == NULL) {
        (void)json_object_put(registry_file);
        debug_log(DLOG_ERROR, "cann't load json file : %s", filename);
        return RET_ERR;
    }

    object_resource = dal_json_object_get_str(file_jso);
    tmp_object = json_tokener_parse(object_resource);

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif

    
    while (TRUE == loop_flag) {
        loop_flag = FALSE;
        json_object_object_foreach0(tmp_object, o_key, o_val)
        {
            vos_task_delay(DELAY_THIRTY_MILLISECONDS); // 优化配置导出CPU占用率
            
            ret = bios_regist_check_object(attributes, o_key, o_val, EXPORT_FILE_TRUE);
            if (ret != RET_OK) {
                if (TRUE == delete_err_flag) {
                    json_object_object_del(file_jso, o_key);
                }
            } else {
                loop_flag = TRUE;
                
                bios_regist_dependency_response(dependencies, attributes, o_key, REGRIST_PROP_CURRENTVALUE, TRUE);
                json_object_object_del(tmp_object, o_key);
            }
        }

        // 如果 FALSE == loop_flag 说明，已经无可以设置属性，则在循环一次删除错误消息
        if (FALSE == loop_flag && FALSE == delete_err_flag) {
            loop_flag = TRUE;
            delete_err_flag = TRUE;
        }
    }

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
    json_object_put(tmp_object);

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif

    json_object_object_foreach0(file_jso, key, val)
    {
        vos_task_delay(DELAY_THIRTY_MILLISECONDS); // 优化配置导出CPU占用率
        ret = json_one_object_bios_config(key, val, &pproperty);
        if (ret != RET_OK) {
            continue;
        }

        if (plisthead == NULL) {
            plisthead = pproperty;
        }

        if (plastproperty == NULL) {
            plastproperty = pproperty;
        } else {
            plastproperty->pnextproperty = pproperty;
            plastproperty = pproperty;
        }
    }

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif

    (void)json_object_put(file_jso);
    (void)json_object_put(registry_file);

    if (plisthead != NULL) {
        *ppropertyList = plisthead;
        return RET_OK;
    }

    return RET_ERR;
}


LOCAL gint32 bios_read_config_from_inifile(gchar *filename, struct property_record **ppropertyList)
{
    FILE *fp = NULL;
    gchar linebuffer[MAX_BIOS_FILE_LINE_LEN] = {0};
    struct property_record *pproperty = NULL;
    struct property_record *plisthead = NULL;
    struct property_record *plastproperty = NULL;
    gint32 ret = 0;

    if ((filename == NULL) || (ppropertyList == NULL)) {
        debug_log(DLOG_ERROR, " input parameter error ! ");
        return RET_ERR;
    }

    fp = fopen(filename, "r");
    if (fp == NULL) {
        debug_log(DLOG_ERROR, " fopen(%s) fail ! ", filename);
        return RET_ERR;
    }

    while (!feof(fp) && vos_fgets(linebuffer, MAX_BIOS_FILE_LINE_LEN, fp) != NULL) {
        if (strlen(linebuffer) < 2) {
            continue;
        }

        if (linebuffer[0] == '/') {
            continue;
        }

        ret = parser_one_line_bios_config(linebuffer, sizeof(linebuffer), &pproperty);
        if (ret != RET_OK) {
            continue;
        }

        if (plisthead == NULL) {
            plisthead = pproperty;
        }

        if (plastproperty == NULL) {
            plastproperty = pproperty;
        } else {
            plastproperty->pnextproperty = pproperty;
            plastproperty = pproperty;
        }

        debug_log(DLOG_DEBUG, " %s %s ", pproperty->propertyname, pproperty->propertyvalue);
    }

    (void)fclose(fp);

    if (plisthead != NULL) {
        *ppropertyList = plisthead;

        return RET_OK;
    }

    return RET_ERR;
}


gint32 bios_read_version_from_registry(gchar *fileshortname, gchar *version, gint32 sz)
{
    FM_FILE_S *file_handle = NULL;
    gchar *data_buf = NULL;
    gint32 fm_fseek_back = 1; // 返回成功是0，不成功非0
    gint32 file_size = 0;
    guint32 file_actual_size = 0;
    const gchar *str = NULL;
    const gchar *str_end = NULL;
    errno_t ret = RET_ERR;

    if ((fileshortname == NULL) || (version == NULL)) {
        debug_log(DLOG_ERROR, " input parameter error ! ");
        return RET_ERR;
    }

    
    file_handle = fm_fopen(fileshortname, "r+b");
    if (file_handle == NULL) {
        debug_log(DLOG_ERROR, "%s: open file %s failed", __FUNCTION__, fileshortname);
        goto err_out;
    }

    
    fm_fseek_back = fm_fseek(file_handle, 0, SEEK_END);
    if (fm_fseek_back != 0) {
        debug_log(DLOG_ERROR, "%s: file seek %s failed", __FUNCTION__, fileshortname);
        goto err_out;
    }

    file_size = fm_ftell(file_handle);
    if (file_size < 0) {
        debug_log(DLOG_ERROR, "%s: file tell %s failed", __FUNCTION__, fileshortname);
        goto err_out;
    }

    
    data_buf = (gchar *)g_malloc0(file_size + 1);
    if (data_buf == NULL) {
        debug_log(DLOG_ERROR, "g_malloc0 len:%d fail!\n", file_size + 1);
        goto err_out;
    }

    
    fm_fseek_back = fm_fseek(file_handle, 0, SEEK_SET);
    if (fm_fseek_back != 0) {
        debug_log(DLOG_ERROR, "%s: file seek %s failed", __FUNCTION__, fileshortname);
        goto err_out;
    }

    file_actual_size = fm_fread(data_buf, BIOS_FILE_STREAM_SIZE, (guint32)file_size, file_handle);
    if (file_size != file_actual_size) {
        debug_log(DLOG_ERROR, "read file(InLen:%d,OutLen:%d) failed", file_size, file_actual_size);
        goto err_out;
    }

    debug_log(DLOG_INFO, "%s: file read OK, %s\n", __FUNCTION__, data_buf);

    
    clear_space_tab_in_string(data_buf,
        strlen(data_buf)); // 去除字符串中空格和制表符，防止因键值之间存在空格或制表符导致版本获取失败
    str = strstr(data_buf, "\"RegistryVersion\":\"");
    if (str != NULL) {
        
        if (strlen(data_buf) > (str - data_buf)) {
            
            str = str + strlen("\"RegistryVersion\":\"");
            str_end = strstr(str, "\"");
            if (str_end != NULL) {
                ret = strncpy_s(version, sz, str, str_end - str);
            }
        }
    }

    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s, %d, get RegistryVersion failed\n", __FUNCTION__, __LINE__);
        goto err_out;
    }
    debug_log(DLOG_INFO, "%s: RegistryVersion OK, %s\n", __FUNCTION__, version);

    
    fm_fclose(file_handle);
    g_free(data_buf);

    return RET_OK;

err_out:
    if (file_handle != NULL) {
        fm_fclose(file_handle);
    }
    if (data_buf != NULL) {
        g_free(data_buf);
    }

    return RET_ERR;
}


LOCAL gint32 finish_read_version_from_file(FM_FILE_S *file_handle, gchar *data_buf)
{
    if (file_handle != NULL) {
        fm_fclose(file_handle);
    }
    if (data_buf != NULL) {
        g_free(data_buf);
    }
    return RET_ERR;
}


LOCAL gint32 get_config_version(gchar *data_buf, gchar *version, guint32 ver_size)
{
    gint32 ret = RET_ERR;
    const gchar *str_end = NULL;

    
    clear_space_tab_in_string(data_buf, strlen(data_buf));

    
    const gchar *str = strstr(data_buf, "\"Version\":\"");
    if (str != NULL) {
        
        if (strlen(data_buf) > (str - data_buf)) {
            
            str = str + strlen("\"Version\":\"");
            str_end = strstr(str, "\"");
            if (str_end != NULL) {
                ret = strncpy_s(version, ver_size, str, str_end - str);
            }
        }
    }

    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s, get Version failed,ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    guint32 ver_len = strlen(version);
    if (ver_len > BIOS_VERSION_MAX_LEN) {
        debug_log(DLOG_ERROR, "%s, Version length invaild, len = %d", __FUNCTION__, ver_len);
        return RET_ERR;
    }

    debug_log(DLOG_INFO, "%s: get Version success, Version is %s", __FUNCTION__, version);
    return RET_OK;
}


gint32 bios_read_version_from_file(gchar *fileshortname, gchar *version, guint32 ver_size)
{
    gchar *data_buf = NULL;
    if ((fileshortname == NULL) || (version == NULL)) {
        debug_log(DLOG_ERROR, "%s: input parameter error", __FUNCTION__);
        return RET_ERR;
    }
    
    FM_FILE_S *file_handle = fm_fopen(fileshortname, "r+b");
    if (file_handle == NULL) {
        debug_log(DLOG_ERROR, "%s: open file %s failed", __FUNCTION__, fileshortname);
        return finish_read_version_from_file(file_handle, data_buf);
    }

    
    gint32 fm_fseek_back = fm_fseek(file_handle, 0, SEEK_END);
    if (fm_fseek_back != RET_OK) {
        debug_log(DLOG_ERROR, "%s: file seek %s failed", __FUNCTION__, fileshortname);
        return finish_read_version_from_file(file_handle, data_buf);
    }

    gint32 file_size = fm_ftell(file_handle);
    if (file_size < 0) {
        debug_log(DLOG_ERROR, "%s: file tell %s failed", __FUNCTION__, fileshortname);
        return finish_read_version_from_file(file_handle, data_buf);
    }

    
    data_buf = (gchar *)g_malloc0(file_size + 1);
    if (data_buf == NULL) {
        debug_log(DLOG_ERROR, "g_malloc0 len:%d fail!", file_size + 1);
        return finish_read_version_from_file(file_handle, data_buf);
    }

    
    fm_fseek_back = fm_fseek(file_handle, 0, SEEK_SET);
    if (fm_fseek_back != RET_OK) {
        debug_log(DLOG_ERROR, "%s: file seek %s failed", __FUNCTION__, fileshortname);
        return finish_read_version_from_file(file_handle, data_buf);
    }

    guint32 file_actual_size = fm_fread(data_buf, BIOS_FILE_STREAM_SIZE, (guint32)file_size, file_handle);
    if (file_size != file_actual_size) {
        debug_log(DLOG_ERROR, "%s: read file(InLen:%d,OutLen:%d) failed", __FUNCTION__, file_size, file_actual_size);
        return finish_read_version_from_file(file_handle, data_buf);
    }

    debug_log(DLOG_INFO, "%s: file read OK, %s", __FUNCTION__, data_buf);

    if (get_config_version(data_buf, version, ver_size) != RET_OK) {
        return finish_read_version_from_file(file_handle, data_buf);
    }

    
    fm_fclose(file_handle);
    g_free(data_buf);

    return RET_OK;
}


LOCAL gint32 json_one_object_bios_config(const gchar *key, json_object *val, struct property_record **pproperty)
{
    gint32 ret = 0;
    gint32 value = 0;
    guint32 len = 0;
    const gchar *str = NULL;
    gchar *pname = NULL;
    gchar *pvalue = NULL;
    gchar str_value[BIOS_CONFIG_VALUE_LEN] = {0};
    struct property_record *ptmp = NULL;

    if (key == NULL) {
        debug_log(DLOG_ERROR, " input parameter error ! ");
        return RET_ERR;
    }

    // key
    len = strlen(key);
    // 判断整型是否溢出
    if (len == 0xffffffff) {
        return RET_ERR;
    }

    pname = (gchar *)g_malloc0(len + 1);
    if (pname == NULL) {
        return RET_ERR;
    }

    // value    type : int or string
    pvalue = (gchar *)g_malloc0(BIOS_CONFIG_VALUE_LEN);
    if (pvalue == NULL) {
        g_free(pname);
        return RET_ERR;
    }

    ret = json_object_get_type(val);
    if (json_type_string == ret) {
        str = dal_json_object_get_str(val);
        if (str == NULL) {
            debug_log(DLOG_ERROR, "%s, %d, dal_json_object_get_str fail.", __FUNCTION__, __LINE__);
            goto err;
        }

        if (strncpy_s(pvalue, BIOS_CONFIG_VALUE_LEN, str, BIOS_CONFIG_VALUE_LEN - 1) != EOK) {
            debug_log(DLOG_ERROR, "%s, %d, strncpy_s fail.", __FUNCTION__, __LINE__);
            goto err;
        }
    } else if (json_type_int == ret) {
        value = json_object_get_int(val);
        if (snprintf_s(str_value, BIOS_CONFIG_VALUE_LEN, BIOS_CONFIG_VALUE_LEN - 1, "%d", value) <= 0) {
            debug_log(DLOG_ERROR, "%s, %d, snprintf_s fail.", __FUNCTION__, __LINE__);
            goto err;
        }

        if (strncpy_s(pvalue, BIOS_CONFIG_VALUE_LEN, str_value, BIOS_CONFIG_VALUE_LEN - 1) != EOK) {
            debug_log(DLOG_ERROR, "%s, %d, strncpy_s fail.", __FUNCTION__, __LINE__);
            goto err;
        }
    } else {
        debug_log(DLOG_ERROR, "%s, %d, Unknow type. type : %d ", __FUNCTION__, __LINE__, ret);
        goto err;
    }

    if (strncpy_s(pname, len + 1, key, len) != EOK) {
        debug_log(DLOG_ERROR, "%s, %d, strncpy_s fail.", __FUNCTION__, __LINE__);
        goto err;
    }

    ptmp = (struct property_record *)g_malloc0(sizeof(struct property_record));
    if (ptmp == NULL) {
        debug_log(DLOG_ERROR, "%s, %d, g_malloc0 property_record fail ! ", __FUNCTION__, __LINE__);
        goto err;
    }

    ptmp->propertyname = pname;
    ptmp->propertyvalue = pvalue;

    *pproperty = ptmp;

    return RET_OK;

err:
    g_free(pname);
    g_free(pvalue);

    return RET_ERR;
}


LOCAL gint32 parser_one_line_bios_config(gchar *linebuffer, guint32 buflen, struct property_record **pproperty)
{
    errno_t safe_fun_ret = EOK;
    struct property_record *ptmp = NULL;
    gchar *pname = NULL;
    gchar *pvalue = NULL;
    gchar **p = NULL;
    guint32 len = 0;

    if ((linebuffer == NULL) || (pproperty == NULL)) {
        debug_log(DLOG_ERROR, " input parameter error ! ");
        return RET_ERR;
    }

    p = g_strsplit(linebuffer, "    ", 5);
    if (p == NULL) {
        debug_log(DLOG_ERROR, " g_strsplit(%s) error ! ", linebuffer);
        return RET_ERR;
    }

    if ((p[0] == NULL) || (p[3] == NULL)) {
        g_strfreev(p);
        return RET_ERR;
    }

    len = strlen(p[0]);
    // 判断整型是否溢出
    if (len == 0xffffffff) {
        g_strfreev(p);
        return RET_ERR;
    }

    pname = (gchar *)g_malloc0(len + 1);
    if (pname == NULL) {
        g_strfreev(p);
        return RET_ERR;
    }

    // 判断整型是否溢出
    len = strlen(p[3]);
    if (len == 0xffffffff) {
        g_strfreev(p);
        g_free(pname);
        return RET_ERR;
    }

    pvalue = (gchar *)g_malloc0(len + 1);
    if (pvalue == NULL) {
        g_strfreev(p);
        g_free(pname);
        return RET_ERR;
    }

    safe_fun_ret = strncpy_s(pname, strlen(p[0]) + 1, p[0], strlen(p[0]));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    safe_fun_ret = strncpy_s(pvalue, len + 1, p[3], len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }

    ptmp = (struct property_record *)g_malloc0(sizeof(struct property_record));
    if (ptmp == NULL) {
        g_strfreev(p);
        g_free(pname);
        g_free(pvalue);
        debug_log(DLOG_ERROR, " g_malloc0 property_record fail ! ");
        return RET_ERR;
    }

    ptmp->propertyname = pname;
    ptmp->propertyvalue = pvalue;

    *pproperty = ptmp;
    g_strfreev(p);
    return RET_OK;
}


LOCAL void clear_space_tab_in_string(gchar *string_src, gint32 string_len)
{
    gchar *pdst = string_src;
    gchar *psrc = string_src;
    gchar *pend = NULL;

    pend = string_src + string_len;
    while ((*psrc != '\0') && (psrc <= pend)) {
        if ((*psrc != ' ') && (*psrc != '\t')) {
            *pdst = *psrc;
            pdst++;
        }
        psrc++;
    }

    *pdst = '\0';
}