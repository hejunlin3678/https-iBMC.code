

#include <malloc.h>
#include "pme_app/pme_app.h"
#include "bios.h"
#include "bios_extra.h"


#define CONTENT_FLAG 1
#define BIOS_LONG_VERSION 32
#define PATCH_VER_KEY "PatchVersion"
#define COM_INFO_KEY "ComponentInfo"
#define COM_INFO_NAME_KEY "Name"
#define COM_INFO_VER_KEY "Version"
#define BIOS_COM_NAME "Extensible BIOS"

LOCAL gint32 get_basic_ver(gchar *basic_ver, size_t len)
{
    OBJ_HANDLE bios_handle = 0;
    errno_t sec_rv;
    gint32 ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &bios_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get bios handle failed, ret = %d.", __FUNCTION__, ret);
        return RET_ERR;
    }
    gchar cur_ver[BIOS_LONG_VERSION] = {0};
    ret = dal_get_property_value_string(bios_handle, BMC_BIOS_VER_NAME, cur_ver, BIOS_LONG_VERSION);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get bios version failed, ret = %d.", __FUNCTION__, ret);
        return RET_ERR;
    }
    gchar *pr = strchr(cur_ver, 'H');
    if (pr == NULL) {
        sec_rv = strcpy_s(basic_ver, len, cur_ver);
        if (sec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: strcpy_s failed, ret = %d.", __FUNCTION__, sec_rv);
            return RET_ERR;
        }
    } else {
        size_t count = pr - cur_ver - 1;
        sec_rv = strncpy_s(basic_ver, len, cur_ver, count);
        if (sec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s failed, ret = %d.", __FUNCTION__, sec_rv);
            return RET_ERR;
        }
    }
    return RET_OK;
}

LOCAL void set_bios_version(const gchar *long_ver)
{
    OBJ_HANDLE bios_handle = 0;
    gint32 ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &bios_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get bios handle failed, ret = %d.", __FUNCTION__, ret);
        return;
    }
    ret = dal_set_property_value_string(bios_handle, BMC_BIOS_VER_NAME, long_ver, DF_SAVE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set bios version failed, ret = %d.", __FUNCTION__, ret);
        return;
    }
    ret = dal_set_property_value_string(bios_handle, BIOS_BACKUP_VER_NAME, long_ver, DF_SAVE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set bios backup version failed, ret = %d.", __FUNCTION__, ret);
        return;
    }
}

LOCAL void update_bios_version(void)
{
    json_object *version_jso = NULL;
    version_jso = json_object_from_file(BIOS_COMPONENT_JSON_FILE_NAME);
    if (version_jso == NULL) {
        debug_log(DLOG_DEBUG, "%s: failed to parse component version json file.", __FUNCTION__);
        return;
    }
    gchar long_ver[BIOS_LONG_VERSION] = { 0 };
    gchar basic_ver[BIOS_LONG_VERSION] = { 0 };
    json_object *value = NULL;
    if (!json_object_object_get_ex(version_jso, PATCH_VER_KEY, &value)) {
        debug_log(DLOG_ERROR, "%s: failed to get patch version.", __FUNCTION__);
        json_object_put(version_jso);
        return;
    }
    const gchar *patch_ver = json_object_get_string(value);
    gint32 ret = get_basic_ver(basic_ver, BIOS_LONG_VERSION);
    if (ret != RET_OK) {
        json_object_put(version_jso);
        return;
    }
    if (strlen(basic_ver) == 0) {
        json_object_put(version_jso);
        return;
    }
    if (patch_ver == NULL || strlen(patch_ver) == 0) {
        if (strcpy_s(long_ver, BIOS_LONG_VERSION, basic_ver) != RET_OK) {
            debug_log(DLOG_ERROR, "%s: strcpy_s failed.", __FUNCTION__);
            json_object_put(version_jso);
            return;
        }
    } else if (sprintf_s(long_ver, BIOS_LONG_VERSION, "%s.%s", basic_ver, patch_ver) <= 0) {
        debug_log(DLOG_ERROR, "%s: sprintf_s failed.", __FUNCTION__);
        json_object_put(version_jso);
        return;
    }
    set_bios_version(long_ver);
    json_object_put(version_jso);
}

LOCAL void construct_common_request_data(IPMI_REQ_MSG_HEAD_S *req_header, guint8 *req_data, size_t req_len)
{
#define IMU_UPLOAD_FILE_CMD 0x27
    if (req_len < BIOS_COMPONENT_IPMI_LEN) {
        debug_log(DLOG_ERROR, "%s: req_data is too short.", __FUNCTION__);
        return;
    }
    const guint32 huawei_iana_id = HUAWEI_MFG_ID;
    req_header->target_type = 1;                   // IPMIChannel：ME
    req_header->target_instance = 0;
    req_header->lun = 0;
    req_header->netfn = BIOS_IPMI_CMD_NETNUM;      // 0x30
    req_header->cmd = BIOS_IPMI_CMD_BIOS_CMDNUM;   // 0x98
    req_header->src_len = BIOS_COMPONENT_IPMI_LEN;

    req_data[0] = LONGB0(huawei_iana_id);         // 0:0xdb
    req_data[1] = LONGB1(huawei_iana_id);         // 1:0x07
    req_data[2] = LONGB2(huawei_iana_id);         // 2:0x00
    req_data[3] = IMU_UPLOAD_FILE_CMD;            
}

LOCAL void set_offest(guint8 *req_data, guint32 offest, size_t len)
{
    if (len < BIOS_COMPONENT_IPMI_LEN) {
        debug_log(DLOG_ERROR, "%s: req_data is too short.", __FUNCTION__);
        return;
    }
    req_data[6] = LONGB0(offest); 
    req_data[7] = LONGB1(offest); 
    req_data[8] = LONGB2(offest); 
    req_data[9] = LONGB3(offest); 
}

LOCAL gint32 read_file(const gchar *file_name, gpointer *data_buf, size_t *len)
{
    *len = vos_get_file_length(file_name);
    if (vos_get_file_accessible(file_name) != TRUE || *len == 0) {
        debug_log(DLOG_ERROR, "%s: can not access file.", __FUNCTION__);
        return RET_ERR;
    }
    *data_buf = g_malloc0(*len);
    if (*data_buf == NULL) {
        debug_log(DLOG_ERROR, "%s: data_buf g_malloc0 failed.", __FUNCTION__);
        return RET_ERR;
    }

    FILE *fp = dal_fopen_check_realpath(file_name, "r", file_name);
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "%s: failed to open component version file.", __FUNCTION__);
        g_free(*data_buf);
        return RET_ERR;
    }

    size_t read_len = fread(*data_buf, sizeof(gchar), *len, fp);
    if (read_len != *len) {
        debug_log(DLOG_ERROR, "%s: fread failed", __FUNCTION__);
        (void)fclose(fp);
        g_free(*data_buf);
        return RET_ERR;
    }
    (void)fclose(fp);
    return RET_OK;
}

LOCAL gint32 send_request_and_handle_exception(IPMI_REQ_MSG_HEAD_S *req_header, gpointer req_data,
    gpointer *response, const IPMI_BIOS_COMPONENT_VERSION_RESP **rsp_data, size_t req_len)
{
    if (req_len < BIOS_COMPONENT_IPMI_LEN) {
        debug_log(DLOG_ERROR, "%s: req_data is too short.", __FUNCTION__);
        return RET_ERR;
    }
    gint32 result = ipmi_send_request(req_header, req_data, MAX_POSSIBLE_IPMI_FRAME_LEN, response, TRUE);
    if (result != RET_OK) {
        debug_log(DLOG_DEBUG, "%s : failed to send component version ipmi request.", __FUNCTION__);
        return RET_ERR;
    }
    *rsp_data = (const IPMI_BIOS_COMPONENT_VERSION_RESP *)get_ipmi_src_data(*response);
    if (*rsp_data == NULL) {
        debug_log(DLOG_DEBUG, "%s : get_ipmi_src_data failed.", __FUNCTION__);
        g_free(*response);
        return RET_ERR;
    }
    if ((*rsp_data)->comp_code != 0) {
        debug_log(DLOG_DEBUG, "%s : send ipmi request error, code = %u.", __FUNCTION__, (*rsp_data)->comp_code);
        g_free(*response);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL FILE *open_component_file(void)
{
    FILE *fp = dal_fopen_check_realpath(BIOS_COMPONENT_JSON_FILE_NAME, "w+", BIOS_COMPONENT_JSON_FILE_NAME);
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "%s: failed to open component version file.", __FUNCTION__);
        return NULL;
    }

    (void)chown(BIOS_COMPONENT_JSON_FILE_NAME, ROOT_USER_UID, ROOT_USER_GID);
    (void)chmod(BIOS_COMPONENT_JSON_FILE_NAME, (S_IRUSR | S_IWUSR));
    return fp;
}

LOCAL gint32 get_component_version_json(void)
{
    IPMI_REQ_MSG_HEAD_S req_header;
    guint8 req_data[BIOS_COMPONENT_IPMI_LEN] = {0};
    gpointer response = NULL;
    guint32 offest = 0;
    const IPMI_BIOS_COMPONENT_VERSION_RESP *rsp_data = NULL;

    construct_common_request_data(&req_header, req_data, BIOS_COMPONENT_IPMI_LEN);
    req_data[5] = CONTENT_FLAG; 

    
    FILE *fp = open_component_file();
    if (fp == NULL) {
        return RET_ERR;
    }

    do {
        
        gint32 result = send_request_and_handle_exception(&req_header, req_data, &response,
            &rsp_data, BIOS_COMPONENT_IPMI_LEN);
        if (result != RET_OK) {
            debug_log(DLOG_DEBUG, "%s : send_request_and_handle_exception failed.", __FUNCTION__);
            (void)fclose(fp);
            return RET_ERR;
        }

        if (rsp_data->data_len == 0) {
            g_free(response);
            break;
        }

        size_t write_size = fwrite(rsp_data->content.file_data, sizeof(gchar), rsp_data->data_len, fp);
        if (rsp_data->data_len != write_size) {
            debug_log(DLOG_ERROR, "%s: failed to write file.", __FUNCTION__);
            g_free(response);
            (void)fclose(fp);
            return RET_ERR;
        }

        if (rsp_data->last_frame == 0) {
            g_free(response);
            break;
        }

        offest += rsp_data->data_len;
        set_offest(req_data, offest, BIOS_COMPONENT_IPMI_LEN);
        g_free(response);
        response = NULL;
        rsp_data = NULL;
    } while (TRUE);

    (void)fclose(fp);
    if (vos_file_copy(BIOS_CLASS_COMPONENT_JSON_FILE_NAME, BIOS_COMPONENT_JSON_FILE_NAME) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: vos_file_copy failed.", __FUNCTION__);
    }
    update_bios_version();
    return RET_OK;
}

LOCAL gint32 calc_file_crc32(guint32 *crc32)
{
    gpointer data_buf = NULL;
    size_t file_len = 0;

    if (read_file(BIOS_COMPONENT_JSON_FILE_NAME, &data_buf, &file_len) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: read_file failed.", __FUNCTION__);
        return RET_ERR;
    }

    *crc32 = arith_make_crc32(0, (const gchar *)data_buf, file_len);
    g_free(data_buf);
    return RET_OK;
}


LOCAL gint32 verify_file_changed(guint32 *crc32, gchar *bios_ver, size_t ver_len)
{

#define INFO_FLAG 0
    IPMI_REQ_MSG_HEAD_S req_header;
    guint8 req_data[BIOS_COMPONENT_IPMI_LEN] = {0};
    gpointer response = NULL;
    const IPMI_BIOS_COMPONENT_VERSION_RESP *rsp_data = NULL;

    construct_common_request_data(&req_header, req_data, BIOS_COMPONENT_IPMI_LEN);
    req_data[5] = INFO_FLAG;  

    gint32 result = send_request_and_handle_exception(&req_header, req_data,
        &response, &rsp_data, BIOS_COMPONENT_IPMI_LEN);
    if (result != RET_OK) {
        debug_log(DLOG_DEBUG, "%s : send_request_and_handle_exception failed.", __FUNCTION__);
        return RET_ERR;
    }
    if (*crc32 != rsp_data->content.info_data.crc32 ||
        memcmp(bios_ver, rsp_data->content.info_data.bios_version, ver_len) != 0) {
        *crc32 = rsp_data->content.info_data.crc32;
        errno_t sec_rv = memcpy_s(bios_ver, ver_len, rsp_data->content.info_data.bios_version, ver_len);
        if (sec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s : bios_ver memcpy_s failed.", __FUNCTION__);
        }
        g_free(response);
        
        return get_component_version_json();
    } else {
        g_free(response);
        return RET_OK;
    }
}

LOCAL void remove_files(const gchar *component_file, const gchar *class_file)
{
    (void)vos_rm_filepath(component_file);
    (void)vos_rm_filepath(class_file);
}

void update_component_info(void)
{
    OBJ_HANDLE bios_handle = 0;
    guint8 bios_state = 0;
    gint32 ret = 0;
    guint32 crc32 = 0;
    
    
    gchar bios_ver[BIOS_VER_LEN] = { 0 };
    (void)prctl(PR_SET_NAME, (gulong)"UpdateComInfo");

    do {
        vos_task_delay(2000); 
        ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &bios_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: fail to obtain the bios object.", __FUNCTION__);
            continue;
        }

        ret = dal_get_property_value_byte(bios_handle, PROPERTY_BIOS_STARTUP_STATE, &bios_state);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: fail to obtain the bios startup state.", __FUNCTION__);
            continue;
        }

        
        if (bios_state != BIOS_STARTUP_POST_STAGE_FINISH) {
            continue;
        }
        update_bios_version();
        
        if (vos_get_file_accessible(BIOS_COMPONENT_JSON_FILE_NAME) != TRUE ||
            vos_get_file_length(BIOS_COMPONENT_JSON_FILE_NAME) == 0) {
            ret = get_component_version_json();
            if (ret != RET_OK) {
                
                debug_log(DLOG_DEBUG, "%s: get_component_version_json failed.", __FUNCTION__);
                continue;
            }
            
            ret = calc_file_crc32(&crc32);
            if (ret != RET_OK) {
                debug_log(DLOG_DEBUG, "%s: calc_file_crc32 failed.", __FUNCTION__);
                continue;
            }
            vos_task_delay(5000); 
        } else {
            ret = verify_file_changed(&crc32, bios_ver, BIOS_VER_LEN);
            if (ret != RET_OK) {
                debug_log(DLOG_DEBUG, "%s: verify_file_changed failed.", __FUNCTION__);
                
                remove_files(BIOS_COMPONENT_JSON_FILE_NAME, BIOS_CLASS_COMPONENT_JSON_FILE_NAME);
                continue;
            }
            vos_task_delay(5000);  
        }
    } while (TRUE);
}