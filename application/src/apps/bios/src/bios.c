

#include <malloc.h>
#include "pme_app/pme_app.h"
#include "bios_ipmi.h"
#include "bios.h"
#include "bios_extra.h"
#include "smbios.h"
#include "bios_cert.h"
#include "pme_app/xmlparser/xmlparser.h"




#define BIOS_BOOT_TYPE_STR "BootType"
#define BIOS_UEFI_BOOT_STR "UEFIBoot"
#define BIOS_LEGACY_BOOT_STR "LegacyBoot"



#define BIOS_PROP_NONEXIST (-2)


#define VALID_FLAG_FORMAT "valid_flag[%d]:%u\r\n"
#define SET_IN_PROGRESS_FMT "set_in_progress:%u\r\n"
#define SERVICE_PATTITION_SELECT_FMT "service_partition_selector:%u\r\n"
#define SERVICE_PATTITION_SCAN_FMT "service_partition_scan:%u\r\n"
#define BOOT_FLAG_VALID_FMT "boot_flag_valid_bit_clearing:%u\r\n"
#define BOOT_INFO_ACK_FMT "boot_info_acknowledge[%d]:%u\r\n"
#define BOOT_FLAG_FMT "boot_flags[%d]:%u\r\n"
#define BOOT_INIT_INFO_FMT "boot_initiator_info[%d]:%u\r\n"
#define BOOT_INIT_MAILBOX_FMT "boot_initiator_mailbox[%d]:%u\r\n"
#define WTITE_PROTECT_FMT "write_protect:%u\r\n"
#define FILE_NUM_FMT "file_num%d:%u\r\n"
#define FILE_CHANGE_FLAG_FMT "file_change_flag%d:%u\r\n"
#define FILE_CHANNEDL_FMT "file_channel%d:%u\r\n"
#define WRITE_PROTECT_FMT "write_protect:%u\r\n"

#define SMBIOS_WR_CFG_STR "smbios_wr_data_cfg"
#define BIOS_RD_CFG_STR "bios_rd_data_cfg"
#define BIOS_WR_CFG_STR "bios_wr_data_cfg"

#define TEMP_BUFFER_LEN 256

enum {
    SMBIOS_WR_DATA_TYPE,
    BIOS_RD_DATA_TYPE,
    BIOS_WR_DATA_TYPE
};

typedef void (*BIOS_BOOT_OPTION_FUN)(const void *req_msg, gpointer user_data, guint8 *resp_data, guint8 resp_data_len,
    guint8 *resp_len);

#define OS_BOOT_FINISHED 1
#define OS_BOOT_NOT_FINISHED 0
#define BIOS_BOOT_FINISHED 1
#define BIOS_BOOT_NOT_FINISHED 0

typedef struct tag_bios_boot_cmd_info {
    guint8 bios_boot_cmd_type;
    BIOS_BOOT_OPTION_FUN bios_boot_option_fun;
} STRUCT_BIOS_BOOT_CMD_INFO;

STRUCT_BIOS_CHANGE_INI_CONFIG g_boot_type_order_array[] = {
    
    {"BootTypeOrder[0]", 141, 1, 2, 0}, {"BootTypeOrder[1]", 142, 1, 3, 0}, {"BootTypeOrder[2]", 143, 1, 128, 0}, {"BootTypeOrder[3]", 144, 1, 238, 0},
    
    {"BootTypeOrder[0]", 175, 2, 2, 0}, {"BootTypeOrder[1]", 177, 2, 3, 0}, {"BootTypeOrder[2]", 179, 2, 128, 0}, {"BootTypeOrder[3]", 181, 2, 238, 0},
    
    {"LegacyPriorities[0]", 175, 2, 1, 0}, {"LegacyPriorities[1]", 177, 2, 2, 0}, {"LegacyPriorities[2]", 179, 2, 0, 0}, {"LegacyPriorities[3]", 181, 2, 3, 0}
};




#define NO_CLEAR_CLP_CONFIG_FILE 0


const gint8 interface_type_str[TYPE_RESERVED][5] = {"ge", "10ge", "40ge", "fc", "fcoe"};


BIOS_FILE_CHANGE_S g_bios_file_change[BIOS_FILE_MAX_SELECTOR];


guchar g_bios_setting_file_state = BIOS_SETTING_FILE_INEFFECTIVE;



guchar g_bios_config_format = 0;



BOOT_OPTIONS_S g_boot_options;


BIOS_DATA_OPERATE_S g_smbios_wr_data_cfg = { 0 };
BIOS_DATA_OPERATE_S g_bios_rd_data_cfg = { 0 };
BIOS_DATA_OPERATE_S g_bios_wr_data_cfg = { 0 };
BIOS_DATA_OPERATE_S g_upg_fw_data_cfg = { 0 };
BIOS_DATA_OPERATE_S g_bios_imu_wr_data_cfg = { 0 };


guint32 g_manufacture_id_bios = 0;
guint32 g_rsp_manufactureid_bios = 0;


LOCAL PER_S g_bios_per_info[] = {
    
    
    {BIOS_START_OPTION_NAME, PER_MODE_POWER_OFF, (guint8*)& (g_boot_options.boot_flags[1]),  sizeof(g_boot_options.boot_flags[1])},
    {"BootOptions",  PER_MODE_POWER_OFF, (guint8*)& g_boot_options,  sizeof(g_boot_options)},
    
    {"SerPartScan",   PER_MODE_POWER_OFF, (guint8*)& (g_boot_options.service_partition_scan),  sizeof(g_boot_options.service_partition_scan)},
    {"ChangeFile", PER_MODE_POWER_OFF, (guint8*)& g_bios_file_change[0],  sizeof(g_bios_file_change)},
    {"BiosConfig", PER_MODE_POWER_OFF, (guint8*)& g_bios_config_format,  sizeof(g_bios_config_format)},
};

const gchar* g_file_property_name[BIOS_FILE_NAME_NUM] = {
    BIOS_FILE_DISPLAY_NAME, BIOS_FILE_OPTION_NAME, BIOS_FILE_CHANGE_NAME, NULL, NULL,
    NULL, NULL, NULL, PROPERTY_BIOS_CLP_NAME, PROPERTY_BIOS_CLP_RESP_NAME,
    NULL, NULL, PROPERTY_BIOS_MENU_DATA, SMBIOS_DIFF_FILE_NAME, NULL,
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL,
    BIOS_FILE_REGISTRY_NAME, BIOS_FILE_CURRENT_VALUE_NAME, BIOS_FILE_SETTING_NAME, BIOS_FILE_RESULT_NAME, NULL,
    PROPERTY_BIOS_CMES_FILE_NAME, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL,
    BIOS_FILE_CONFIGVALUE_NAME, BIOS_FILE_POLICYCONFIGREGISTRY_NAME, NULL, NULL, NULL
};

LOCAL void monitor_boot_flag(void);


gint32 bios_set_boot_mode_prop_value(guint8 boot_mode)
{
    guint8 boot_mode_support_flag = 0;
    OBJ_HANDLE obj_handle;

    gint32 ret_val = dal_get_object_handle_nth(CLASS_NAME_BIOS, 0, &obj_handle);
    if (ret_val != RET_OK) {
        return RET_ERR;
    }
    (void)dal_get_property_value_byte(obj_handle, BIOS_BOOT_MODE_SUPPORT_FLAG, &boot_mode_support_flag);
    if (boot_mode_support_flag != BOOT_MODE_SUPPORT_FLAG_ENABLE) {
        return RET_ERR;
    }

    
    g_boot_options.boot_flags[0] |= PBIT7;

    if (boot_mode == BIOS_BOOT_LEGACY) {
        g_boot_options.boot_flags[0] &= NBIT5;
    } else if (boot_mode == BIOS_BOOT_UEFI) {
        g_boot_options.boot_flags[0] |= PBIT5;
    } else {
        debug_log(DLOG_ERROR, "Set boot mode failed, boot_mode=0x%x\r\n", boot_mode);
        return RET_ERR;
    }

    (void)dal_set_property_value_byte(obj_handle, BIOS_BOOT_MODE, boot_mode, DF_SAVE);

    ret_val = per_save((guint8 *)&g_boot_options);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "per_save fail(Ret:%d)!\n", ret_val);
        return RET_ERR;
    }

    set_boot_option_valid(BIOS_BOOT_FLAGS_CMD, BIOS_BOOT_OPTION_VALID);

    return ret_val;
}



LOCAL guint8 get_boot_option_valid(guint8 select_val)
{
    
    if (select_val > (sizeof(guint8) * BIOS_BOOT_VALID_SEG_NUM)) {
        return BOOT_OPTION_VALID;
    }

    if (g_boot_options.valid_flag[select_val / BIOS_VALID_FLAG_INTREVAL] &
        (1 << (select_val % BIOS_VALID_FLAG_INTREVAL))) {
        return BOOT_OPTION_INVALID;
    } else {
        return BOOT_OPTION_VALID;
    }
}


LOCAL void dump_bios_cfg_file(FILE *fp_handle, gchar *src_file)
{
    guint32 temp_file_size = 0;
    gint32 i = 0;
    gint32 real_file_size = 0;
    gchar file_tmp[MAX_POSSIBLE_IPMI_FRAME_LEN] = {0};
    size_t fwrite_back = 0;

    if ((fp_handle == NULL) || (src_file == NULL)) {
        debug_log(DLOG_ERROR, "param invalid\n");
        return;
    }

    FM_FILE_S *file_handle = fm_fopen(src_file, "rb");
    if (file_handle == NULL) {
        debug_log(DLOG_ERROR, "%s is not exited\n", src_file);
        return;
    }

    
    gint32 fm_fseek_back = fm_fseek(file_handle, 0, SEEK_END);  // success is 0 ,other is length
    if (fm_fseek_back != 0) {
        fm_fclose(file_handle);
        debug_log(DLOG_ERROR, "%s : fm_fseek fail.\r\n", __FUNCTION__);
        return;
    }

    gint32 file_size = fm_ftell(file_handle);
    if (file_size <= 0) {
        debug_log(DLOG_ERROR, "%s file_size is 0\n", src_file);
        fm_fclose(file_handle);
        return;
    }

    
    fm_fseek_back = fm_fseek(file_handle, 0, SEEK_SET);
    if (fm_fseek_back != 0) {
        fm_fclose(file_handle);
        debug_log(DLOG_ERROR, "%s : fm_fseek fail.\r\n", __FUNCTION__);
        return;
    }

    while (i < file_size) {
        real_file_size = file_size - i;
        real_file_size = real_file_size >= MAX_POSSIBLE_IPMI_FRAME_LEN ? MAX_POSSIBLE_IPMI_FRAME_LEN : real_file_size;

        (void)memset_s(file_tmp, sizeof(file_tmp), 0, sizeof(file_tmp));
        temp_file_size = fm_fread(file_tmp, BIOS_FILE_STREAM_SIZE, (guint32)real_file_size, file_handle);
        if (temp_file_size != (guint32)real_file_size) {
            debug_log(DLOG_ERROR, "fm_fread file size invalid(Should:%u,Real:%u)\n", (guint32)real_file_size,
                temp_file_size);
            break;
        }

        fwrite_back = fwrite(file_tmp, real_file_size, BIOS_FILE_STREAM_SIZE, fp_handle);
        if (fwrite_back != BIOS_FILE_STREAM_SIZE) {
            fm_fclose(file_handle);
            debug_log(DLOG_ERROR, "%s : fwrite fail.\r\n", __FUNCTION__);
            return;
        }

        i += real_file_size;
    }

    fm_fclose(file_handle);

    return;
}


LOCAL gint32 copy_file_to_dest(gchar *path_dest, guint32 path_dest_len, gchar *path_src, guint32 path_src_len)
{
    gchar* argv[10] = {};
    sig_t old_handler;

    if ((path_dest == NULL) || (path_src == NULL)) {
        debug_log(DLOG_DEBUG, "param invalid\n");
        return RET_ERR;
    }

    if (vos_get_file_accessible(path_src) == TRUE) {
        argv[0] = "/bin/cp";
        argv[1] = path_src;
        argv[2] = path_dest;
        argv[3] = NULL;

        old_handler = signal(SIGCHLD, SIG_DFL);
        (void)vos_system_s("/bin/cp", argv);
        (void)signal(SIGCHLD, old_handler);

        return RET_OK;
    }

    debug_log(DLOG_DEBUG, "file no exit");
    return RET_ERR;
}


LOCAL void copy_bios_config_file(OBJ_HANDLE obj_handle, const gchar *property, gchar *path_dest, guint32 len)
{
    gchar file_tmp[256] = {0};

    (void)memset_s(file_tmp, sizeof(file_tmp), 0, sizeof(file_tmp));
    (void)dal_get_property_value_string(obj_handle, property, file_tmp, sizeof(file_tmp));
    (void)copy_file_to_dest(path_dest, len, file_tmp, sizeof(file_tmp));
}


LOCAL gint32 bios_get_file_len(gchar *load_file_name, guint32 *len)
{
    guint32 file_size = 0;
    
    gint32 ret = dal_check_real_path2(load_file_name, DEV_SHM_PATH_WITH_SLASH);
    if (ret == RET_ERR) {
        debug_log(DLOG_ERROR, "%s: check real path fail!", __FUNCTION__);
        return RET_ERR;
    }

    FILE *file_id = g_fopen(load_file_name, "r");
    if (file_id == NULL) {
        debug_log(DLOG_ERROR, "%s: Unable to load file!", __FUNCTION__);
        return RET_ERR;
    }

    ret = fseek(file_id, 0, SEEK_END);
    if (ret != 0) {
        debug_log(DLOG_ERROR, "%s: fseek SEEK_END failed!\n", __FUNCTION__);
        (void)fclose(file_id);
        return RET_ERR;
    }

    file_size = ftell(file_id);
    *len = file_size;
    (void)fclose(file_id);
    return RET_OK;
}


LOCAL gint32 bios_get_file_to_buffer(gchar *load_file_name, gchar *load_buffer, guint32 buffer_size)
{
    if (load_file_name == NULL || load_buffer == NULL || buffer_size == 0) {
        debug_log(DLOG_ERROR, "%s: load_file_name == NULL or load_buffer == NULL or buffer_size = %u.\n", __FUNCTION__,
            buffer_size);
        return RET_ERR;
    }

    gint32 ret = dal_check_real_path2(load_file_name, DEV_SHM_PATH_WITH_SLASH);
    if (ret == RET_ERR) {
        debug_log(DLOG_ERROR, "%s: check real path fail!", __FUNCTION__);
        return RET_ERR;
    }

    FILE *file_id = g_fopen(load_file_name, "r");
    if (file_id == NULL) {
        debug_log(DLOG_ERROR, "%s: Unable to load file", __FUNCTION__);
        return RET_ERR;
    }

    ret = fseek(file_id, 0, SEEK_END);
    if (ret != 0) {
        debug_log(DLOG_ERROR, "%s: fseek end failed!\n", __FUNCTION__);
        goto ReleaseResource;
    }

    guint32 file_size = ftell(file_id);
    if (file_size > buffer_size) {
        debug_log(DLOG_ERROR, "%s: file_size (%u) > buffersize(%u), failed!\n", __FUNCTION__, file_size, buffer_size);
        goto ReleaseResource;
    }

    ret = fseek(file_id, 0, SEEK_SET);
    if (ret != 0) {
        debug_log(DLOG_ERROR, "%s: fseek SET failed!\n", __FUNCTION__);
        goto ReleaseResource;
    }

    ret = fread(load_buffer, sizeof(gchar), file_size, file_id);
    if (ret != file_size) {
        debug_log(DLOG_ERROR, "%s: fread failed!\n", __FUNCTION__);
        goto ReleaseResource;
    }

    (void)fclose(file_id);
    return RET_OK;

ReleaseResource:

    (void)fclose(file_id);
    return RET_ERR;
}


LOCAL gint32 bios_write_buffer_to_file(gchar *buffer, guint32 buffer_size, gchar *write_file_name)
{
    if (write_file_name == NULL || buffer == NULL || buffer_size == 0) {
        debug_log(DLOG_ERROR, "%s: write_file_name == NULL or buffer == NULL or buffer_size (%u).\n", __FUNCTION__,
            buffer_size);
        return RET_ERR;
    }

    gint32 ret = dal_check_real_path2(write_file_name, DEV_SHM_PATH_WITH_SLASH);
    if (ret == RET_ERR) {
        debug_log(DLOG_ERROR, "%s: check real path fail!", __FUNCTION__);
        return RET_ERR;
    }

    FILE *file_id = g_fopen(write_file_name, "w+");
    if (file_id == NULL) {
        debug_log(DLOG_ERROR, "%s, Unable to create file.\n", __FUNCTION__);
        return RET_ERR;
    }

    ret = fwrite(buffer, sizeof(gchar), buffer_size, file_id);
    if (buffer_size != ret) {
        debug_log(DLOG_ERROR, "%s: fwrite failed, write chars (%d) != buffer size (%d)!\n", __FUNCTION__, ret,
            buffer_size);
        (void)fclose(file_id);
        return RET_ERR;
    }

    (void)fclose(file_id);
    return RET_OK;
}


LOCAL void bios_fmt_json_file(OBJ_HANDLE obj_handle, const gchar *property, gchar *path, guint16 path_len)
{
    gchar path_file_name[MAX_FILEPATH_LENGTH] = {0};
    gchar dst_pf_name[MAX_FILEPATH_LENGTH] = {0};
    gchar *pchr_tmpdst = NULL;
    gchar *pchr_src = NULL;
    gchar *pchr_dst = NULL;
    gchar *LoadBuffer = NULL;
    gchar *DstBuffer = NULL;
    gchar *pchr_tmp = NULL;
    guint32 LoadBuf_Len = 0;
    guint32 DstBuf_Len = 0;
    guint32 Cvt_Len = 0;

    if (property == NULL || path == NULL || path_len == 0) {
        return;
    }
    (void)dal_get_property_value_string(obj_handle, property, path_file_name, sizeof(path_file_name));

    
    gchar *pchr_fname = strrchr(path_file_name, '/');

    errno_t safe_fun_ret = memmove_s(dst_pf_name, sizeof(dst_pf_name), path, strlen(path));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    pchr_tmpdst = &dst_pf_name[0] + strlen(path);
    if (pchr_fname != NULL) {
        safe_fun_ret =
            memmove_s(pchr_tmpdst, MAX_FILEPATH_LENGTH - strlen(dst_pf_name), pchr_fname, strlen(pchr_fname));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    } else { 
        if (((guint32)strlen(dst_pf_name) < sizeof(path_file_name))) {
            *pchr_tmpdst++ = '/';
        }
        safe_fun_ret =
            memmove_s(pchr_tmpdst, MAX_FILEPATH_LENGTH - strlen(dst_pf_name), path_file_name, strlen(path_file_name));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    }

    
    dst_pf_name[MAX_FILEPATH_LENGTH - 1] = '\0';

    if (vos_get_file_accessible(dst_pf_name) != TRUE) {
        debug_log(DLOG_DEBUG, "%s: file no exit.\n", __FUNCTION__);
        return;
    }

    gint32 iRet = dal_check_real_path(dst_pf_name);
    if (iRet == RET_ERR) {
        debug_log(DLOG_ERROR, "%s: check real path fail!", __FUNCTION__);
        return;
    }

    iRet = bios_get_file_len(dst_pf_name, &LoadBuf_Len);
    if (iRet != RET_OK) {
        debug_log(DLOG_ERROR, "%s: bios_get_file_len return fail.\n", __FUNCTION__);
        return;
    }

    
    if (LoadBuf_Len > BIOS_FILE_MAX_SIZE) {
        debug_log(DLOG_ERROR, "%s: file length over %u", __FUNCTION__, BIOS_FILE_MAX_SIZE);
        return;
    }
    

    LoadBuf_Len++; 
    LoadBuffer = (gchar *)g_malloc(LoadBuf_Len);
    if (LoadBuffer == NULL) {
        debug_log(DLOG_ERROR, "%s: memory alloc failure for source buffer (%d bytes).\n", __FUNCTION__, LoadBuf_Len);
        return;
    }
    (void)memset_s(LoadBuffer, LoadBuf_Len, 0, LoadBuf_Len);

    iRet = bios_get_file_to_buffer(dst_pf_name, LoadBuffer, LoadBuf_Len);
    if (iRet != RET_OK) {
        debug_log(DLOG_ERROR, "%s: bios_get_file_len return fail.\n", __FUNCTION__);
        goto ReleaseResource;
    }

    *(LoadBuffer + LoadBuf_Len - 1) = '\0';

    
    if (LoadBuf_Len < 500) {
        DstBuf_Len = LoadBuf_Len + LoadBuf_Len + LoadBuf_Len + LoadBuf_Len;
    } else {
        DstBuf_Len = LoadBuf_Len + (LoadBuf_Len >> 1);
    }

    DstBuffer = (gchar *)g_malloc(DstBuf_Len);
    if (DstBuffer == NULL) {
        debug_log(DLOG_ERROR, "%s: memory alloc failure for dst buffer (%d bytes).\n", __FUNCTION__, DstBuf_Len);
        goto ReleaseResource;
    }

    (void)memset_s(DstBuffer, DstBuf_Len, 0, DstBuf_Len);

    pchr_src = LoadBuffer;
    pchr_dst = DstBuffer;

    
    while (*pchr_src == ' ') {
        pchr_src++;
    }

    
    if (*pchr_src == '{' && Cvt_Len < DstBuf_Len) {
        *pchr_dst++ = *pchr_src++;
        *pchr_dst++ = '\n';
        Cvt_Len += 2;
    }

    
    
    while ((*pchr_src != '\0') && (Cvt_Len + 3 < DstBuf_Len)) {
        switch (*pchr_src) {
            case '{': 
                *pchr_dst++ = '\n';
                *pchr_dst++ = *pchr_src++;
                *pchr_dst++ = '\n';
                Cvt_Len += 3;
                break;

            case '}': 
                *pchr_dst++ = '\n';
                *pchr_dst++ = *pchr_src++;
                Cvt_Len += 2;

                
                while (*pchr_src == ' ') {
                    pchr_src++;
                }

                break;

            case ']': 
                if (pchr_src > LoadBuffer && *(pchr_src - 1) == '}') {
                    *pchr_dst++ = '\n';
                    *pchr_dst++ = *pchr_src++;
                    Cvt_Len += 2;
                } else {
                    *pchr_dst++ = *pchr_src++;
                    Cvt_Len += 1;
                }

                break;
            case ',':
                pchr_tmp = pchr_src;

                
                do {
                    pchr_tmp++;
                } while (*pchr_tmp == ' ');

                
                if (*pchr_tmp == '"') {
                    *pchr_dst++ = *pchr_src++;
                    *pchr_dst++ = '\n';
                    *pchr_dst++ = *pchr_tmp++;
                    pchr_src = pchr_tmp;
                    Cvt_Len += 3;
                }
                
                if (*pchr_tmp != '"') {
                    *pchr_dst++ = *pchr_src++;
                    Cvt_Len += 1;
                }
                break;

            case ' ': 
                *pchr_dst++ = *pchr_src++;
                Cvt_Len += 1;
                break;
            default:
                *pchr_dst++ = *pchr_src++;
                Cvt_Len += 1;
        }
    }

    if ((*pchr_src != '\0') && (Cvt_Len + 3 >= DstBuf_Len)) {
        debug_log(DLOG_ERROR,
            "%s: process stopped because "
            "converted len (%d) is to be large than Dest_Buffer_Len (%d).\n",
            __FUNCTION__, Cvt_Len, DstBuf_Len);
        goto ReleaseResource;
    }

    
    *pchr_dst++ = '\0';

    (void)bios_write_buffer_to_file(DstBuffer, Cvt_Len, dst_pf_name);

ReleaseResource:

    if (LoadBuffer != NULL) {
        g_free(LoadBuffer);
    }
    if (DstBuffer != NULL) {
        g_free(DstBuffer);
    }
}

LOCAL gint32 write_info_to_file(FILE *fp, const gchar *fmt, ...)
{
    gchar file_tmp[TEMP_BUFFER_LEN] = {0};
    size_t fwrite_back;
    va_list arglist;

    va_start(arglist, fmt);
    gint32 ret = vsnprintf_s(file_tmp, sizeof(file_tmp), sizeof(file_tmp) - 1, fmt, arglist);
    va_end(arglist);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: vsnprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    fwrite_back = fwrite(file_tmp, strlen(file_tmp), 1, fp);
    if (fwrite_back != 1) {
        debug_log(DLOG_ERROR, "%s:fwrite fail!", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 create_and_open_dump_dir(const gchar *path, FILE **fp)
{
    gchar file_tmp[TEMP_BUFFER_LEN] = {0};

    gint32 ret_val = snprintf_s(file_tmp, sizeof(file_tmp), sizeof(file_tmp) - 1, "%s/bios_info", path);
    if (ret_val <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret_val);
        return RET_ERR;
    }
    ret_val = dal_check_real_path2(file_tmp, DEV_SHM_PATH_WITH_SLASH);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "%s: check realpath fail, ret = %d", __FUNCTION__, ret_val);
        return RET_ERR;
    }

    *fp = dal_fopen_check_realpath(file_tmp, "a+", file_tmp);
    if (*fp == NULL) {
        debug_log(DLOG_ERROR, "%s: Open file by dal_fopen_check_realpath failed", __FUNCTION__);
        return RET_ERR;
    }

    (void)fchmod(fileno(*fp), (S_IRUSR | S_IWUSR | S_IRGRP));

    return RET_OK;
}

LOCAL gint32 dump_bios_boot_single_info(BOOT_OPTIONS_S *boot_option, FILE *fp)
{
    gint32 ret_val;

    do {
        ret_val = write_info_to_file(fp, SET_IN_PROGRESS_FMT, boot_option->set_in_progress);
        if (ret_val != RET_OK) {
            break;
        }

        ret_val = write_info_to_file(fp, SERVICE_PATTITION_SELECT_FMT, boot_option->service_partition_selector);
        if (ret_val != RET_OK) {
            break;
        }

        ret_val = write_info_to_file(fp, SERVICE_PATTITION_SCAN_FMT, boot_option->service_partition_scan);
        if (ret_val != RET_OK) {
            break;
        }

        ret_val = write_info_to_file(fp, BOOT_FLAG_VALID_FMT, boot_option->boot_flag_valid_bit_clearing);
        if (ret_val != RET_OK) {
            break;
        }

        return RET_OK;
    } while (0);

    debug_log(DLOG_ERROR, "%s: write_info_to_file fail!", __FUNCTION__);
    return RET_ERR;
}

LOCAL gint32 dump_bios_boot_array_info(guint8 *data_info, guint32 data_info_len, const gchar *data_info_fmt, FILE *fp)
{
#define DATA_MAX_INFO_LEN 1000
    gint32 ret_val;

    if (data_info_len > DATA_MAX_INFO_LEN) {
        debug_log(DLOG_ERROR, "%s: Invalid data_info_len(%u)", __FUNCTION__, data_info_len);
        return RET_ERR;
    }

    for (guint32 i = 0; i < data_info_len; i++) {
        ret_val = write_info_to_file(fp, data_info_fmt, i, data_info[i]);
        if (ret_val != RET_OK) {
            debug_log(DLOG_ERROR, "%s: write_info_to_file fail!", __FUNCTION__);
            return RET_ERR;
        }
    }

    return RET_OK;
}

LOCAL gint32 dump_bios_boot_info(BOOT_OPTIONS_S *boot_option, FILE *fp)
{
    size_t fwrite_back;

    fwrite_back = fwrite(BIOS_DUMP_BOOT_HEAD, strlen(BIOS_DUMP_BOOT_HEAD), 1, fp);
    if (fwrite_back != 1) {
        debug_log(DLOG_ERROR, "%s:fwrite fail!", __FUNCTION__);
        return RET_ERR;
    }

    gint32 ret_val =
        dump_bios_boot_array_info(boot_option->valid_flag, sizeof(boot_option->valid_flag), VALID_FLAG_FORMAT, fp);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dump valid flag failed", __FUNCTION__);
        return RET_ERR;
    }

    ret_val = dump_bios_boot_single_info(boot_option, fp);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dump bios boot info failed", __FUNCTION__);
        return RET_ERR;
    }

    ret_val = dump_bios_boot_array_info(boot_option->boot_info_acknowledge, sizeof(boot_option->boot_info_acknowledge),
        BOOT_INFO_ACK_FMT, fp);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dump boot_info_acknowledge failed", __FUNCTION__);
        return RET_ERR;
    }
    ret_val = dump_bios_boot_array_info(boot_option->boot_flags, sizeof(boot_option->boot_flags), BOOT_FLAG_FMT, fp);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dump boot_flags failed", __FUNCTION__);
        return RET_ERR;
    }
    ret_val = dump_bios_boot_array_info(boot_option->boot_initiator_info, sizeof(boot_option->boot_initiator_info),
        BOOT_INIT_INFO_FMT, fp);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dump boot_initiator_info failed", __FUNCTION__);
        return RET_ERR;
    }
    ret_val = dump_bios_boot_array_info(boot_option->boot_initiator_mailbox,
        sizeof(boot_option->boot_initiator_mailbox), BOOT_INIT_MAILBOX_FMT, fp);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dump boot_initiator_mailbox failed", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 dump_file_change_info(BIOS_FILE_CHANGE_S *file_change, FILE *fp)
{
    gint32 ret_val;
    size_t fwrite_back;

    fwrite_back = fwrite(BIOS_DUMP_CHANGE_HEAD, strlen(BIOS_DUMP_CHANGE_HEAD), 1, fp);
    if (fwrite_back != 1) {
        debug_log(DLOG_ERROR, "%s:fwrite fail!", __FUNCTION__);
        return RET_ERR;
    }

    for (guint8 i = 0; i < BIOS_FILE_MAX_SELECTOR; i++) {
        ret_val = write_info_to_file(fp, FILE_NUM_FMT, i, (file_change + i)->file_num);
        if (ret_val != RET_OK) {
            debug_log(DLOG_ERROR, "%s: write file_num info to file fail!", __FUNCTION__);
            return RET_ERR;
        }

        ret_val = write_info_to_file(fp, FILE_CHANGE_FLAG_FMT, i, (file_change + i)->file_change_flag);
        if (ret_val != RET_OK) {
            debug_log(DLOG_ERROR, "%s: write file_change_flag info to file fail!", __FUNCTION__);
            return RET_ERR;
        }

        ret_val = write_info_to_file(fp, FILE_CHANNEDL_FMT, i, (file_change + i)->file_channel);
        if (ret_val != RET_OK) {
            debug_log(DLOG_ERROR, "%s: write file_channel info to file fail!", __FUNCTION__);
            return RET_ERR;
        }
    }

    return RET_OK;
}

LOCAL gint32 dump_smbios_bios_data_cfg(BIOS_DATA_OPERATE_S *data_operation, guint8 data_cfg_type, FILE *fp)
{
    gint32 ret_val;
    gchar file_tmp[TEMP_BUFFER_LEN] = {0};
    size_t fwrite_back;

    (void)memset_s(file_tmp, sizeof(file_tmp), 0, sizeof(file_tmp));

    switch (data_cfg_type) {
        case SMBIOS_WR_DATA_TYPE:
            ret_val = snprintf_s(file_tmp, sizeof(file_tmp), sizeof(file_tmp) - 1, BIOS_DUMP_DATA_CONT,
                SMBIOS_WR_CFG_STR, data_operation->data_flag, data_operation->data_offset, data_operation->data_len);
            break;
        case BIOS_RD_DATA_TYPE:
            ret_val = snprintf_s(file_tmp, sizeof(file_tmp), sizeof(file_tmp) - 1, BIOS_DUMP_DATA_CONT, BIOS_RD_CFG_STR,
                data_operation->data_flag, data_operation->data_offset, data_operation->data_len);
            break;
        case BIOS_WR_DATA_TYPE:
            ret_val = snprintf_s(file_tmp, sizeof(file_tmp), sizeof(file_tmp) - 1, BIOS_DUMP_DATA_CONT, BIOS_WR_CFG_STR,
                data_operation->data_flag, data_operation->data_offset, data_operation->data_len);
            break;
        default:
            return RET_ERR;
    }

    if (ret_val <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret_val);
    }

    fwrite_back = fwrite(file_tmp, strlen(file_tmp), 1, fp);
    if (fwrite_back != 1) {
        debug_log(DLOG_ERROR, "%s:fwrite fail!", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 dump_data_operation_info(FILE *fp)
{
    BIOS_DATA_OPERATE_S *data_operation = NULL;
    size_t fwrite_back;

    fwrite_back = fwrite(BIOS_DUMP_DATA_HEAD, strlen(BIOS_DUMP_DATA_HEAD), 1, fp);
    if (fwrite_back != 1) {
        debug_log(DLOG_ERROR, "%s:fwrite fail!", __FUNCTION__);
        return RET_ERR;
    }

    data_operation = &g_smbios_wr_data_cfg;
    gint32 ret_val = dump_smbios_bios_data_cfg(data_operation, SMBIOS_WR_DATA_TYPE, fp);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "%s:write smbios wr data cfg failed", __FUNCTION__);
        return RET_ERR;
    }

    data_operation = &g_bios_rd_data_cfg;
    ret_val = dump_smbios_bios_data_cfg(data_operation, BIOS_RD_DATA_TYPE, fp);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "%s:write bios rd data cfg failed", __FUNCTION__);
        return RET_ERR;
    }

    data_operation = &g_bios_wr_data_cfg;
    ret_val = dump_smbios_bios_data_cfg(data_operation, BIOS_WR_DATA_TYPE, fp);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "%s:write bios wr data cfg failed", __FUNCTION__);
        return RET_ERR;
    }

    data_operation = &g_bios_imu_wr_data_cfg;
    ret_val = dump_smbios_bios_data_cfg(data_operation, BIOS_WR_DATA_TYPE, fp);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "%s:write bios imu wr data cfg failed", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gboolean check_is_json_file(OBJ_HANDLE obj_handle, const gchar *property)
{
#define JSON_FILE_SUFFIX ".json"
    gchar *file_suffix = NULL;
    gchar file_name[MAX_FILEPATH_LENGTH] = {0};

    (void)dal_get_property_value_string(obj_handle, property, file_name, sizeof(file_name));

    file_suffix = strrchr(file_name, '.');
    if (file_suffix != NULL) {
        if (strcmp(file_suffix, JSON_FILE_SUFFIX) == 0) {
            return TRUE;
        }
    }
    debug_log(DLOG_DEBUG, "%s:%s file is not json fomat file", __FUNCTION__, file_name);

    return FALSE;
}

LOCAL gint32 copy_and_format_bios_json_file(const gchar *path)
{
    gint32 ret_val;
    OBJ_HANDLE bios_handle = 0;
    gchar path_tmp[TEMP_BUFFER_LEN] = {0};

    const gchar* dump_file_name[] = {
        PROPERTY_BIOS_CLP_NAME, PROPERTY_BIOS_CLP_RESP_NAME, BIOS_FILE_OPTION_NAME,
        BIOS_FILE_CHANGE_NAME, BIOS_FILE_DISPLAY_NAME, BIOS_FILE_REGISTRY_NAME,
        BIOS_FILE_CURRENT_VALUE_NAME, BIOS_FILE_SETTING_NAME, BIOS_FILE_RESULT_NAME,
        PROPERTY_BIOS_CMES_FILE_NAME
    };

    ret_val = dfl_get_object_handle(BIOS_OBJECT_NAME, &bios_handle);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return RET_ERR;
    }

    (void)strncpy_s(path_tmp, sizeof(path_tmp), path, sizeof(path_tmp) - 1);

    for (guint8 i = 0; i < sizeof(dump_file_name) / sizeof(dump_file_name[0]); i++) {
        copy_bios_config_file(bios_handle, dump_file_name[i], path_tmp, sizeof(path_tmp));
        if (check_is_json_file(bios_handle, dump_file_name[i]) == TRUE) {
            bios_fmt_json_file(bios_handle, dump_file_name[i], path_tmp, sizeof(path_tmp));
        }
    }

    return RET_OK;
}


gint32 bios_dump_info(const gchar *path)
{
    BIOS_FILE_CHANGE_S *file_change = g_bios_file_change;
    BOOT_OPTIONS_S *boot_option = &g_boot_options;
    FILE *fp = NULL;

    gint32 ret_val = create_and_open_dump_dir(path, &fp); // 此处有打开 fp 操作
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "%s: create_dump_dir failed", __FUNCTION__);
        return RET_ERR;
    }
    // dump Bios Boot Options 信息
    ret_val = dump_bios_boot_info(boot_option, fp);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "%s: write bios boot info failed", __FUNCTION__);
        goto EXIT;
    }

    // dump write_protect信息
    ret_val = write_info_to_file(fp, WRITE_PROTECT_FMT, boot_option->write_protect.byte);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "%s: write write_protect info to file fail!", __FUNCTION__);
        goto EXIT;
    }

    // dump Bios File Change 信息
    ret_val = dump_file_change_info(file_change, fp);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dump_file_change_info failed", __FUNCTION__);
        goto EXIT;
    }

    // dump Bios Data Operation 信息
    ret_val = dump_data_operation_info(fp);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "%s:dump_data_operation_info failed", __FUNCTION__);
        goto EXIT;
    }

    // dump SmBios Config file 信息
    size_t fwrite_back = fwrite(BIOS_DUMP_SMBIOS_CFG, strlen(BIOS_DUMP_SMBIOS_CFG), 1, fp);
    if (fwrite_back != 1) {
        debug_log(DLOG_ERROR, "%s:fwrite fail!", __FUNCTION__);
        goto EXIT;
    }
    dump_bios_cfg_file(fp, SMBIOS_FILE_NAME);

    (void)fclose(fp);

    ret_val = copy_and_format_bios_json_file(path);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "%s: format_bios_json_file", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;

EXIT:
    (void)fclose(fp);
    return RET_ERR;
}


gint32 bios_set_file_changed(const void *req_msg, gpointer user_data)
{
    const guint8 *src_data = NULL;
    guint8 resp_data[BIOS_MSG_HEAD_MIN_LEN];
    guint8 bios_id = 0;
    guint8 file_select = 0;
    guchar arm_enable = 0;
    gint32 ret = 0;
    guint32 product_version = 0;

    if (req_msg == NULL) {
        
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    src_data = get_ipmi_src_data(req_msg);
    if (src_data == NULL) {
        dal_ipmi_log_with_bios_set_check(req_msg, MLOG_INFO, FC__PUBLIC_OK, BIOS_OP_LOG_FILE_CHANGE);
        debug_log(DLOG_ERROR, DB_STR_GET_SRC_DATA_FAIL);
        
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
        
    }

    
    gint32 ret_val = judge_manu_id_valid_bios(src_data);
    if (ret_val != RET_OK) {
        dal_ipmi_log_with_bios_set_check(req_msg, MLOG_INFO, FC__PUBLIC_OK, BIOS_OP_LOG_FILE_CHANGE);
        debug_log(DLOG_ERROR, DB_STR_MUNU_ID_ERR, ret_val);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }

    bios_id = src_data[BIOS_ID_OFFSET];

    if (bios_id > BIOS_ID) {
        dal_ipmi_log_with_bios_set_check(req_msg, MLOG_INFO, FC__PUBLIC_OK, BIOS_OP_LOG_FILE_CHANGE);
        debug_log(DLOG_ERROR, "BiosId:%d(MaxId:%d) is invalid!\n", bios_id, BIOS_ID);

        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        return construt_and_respond_msg(req_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }

    file_select = src_data[5];

    if (file_select >= BIOS_FILE_MAX_SELECTOR) {
        dal_ipmi_log_with_bios_set_check(req_msg, MLOG_INFO, FC__PUBLIC_OK, BIOS_OP_LOG_FILE_CHANGE);
        debug_log(DLOG_ERROR, "file_select:%d(Max:%d) is invalid!\n", file_select, BIOS_FILE_MAX_SELECTOR);

        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        return construt_and_respond_msg(req_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }

    
    g_bios_file_change[0].file_num = bios_id;
    set_bios_setting_file_change_flag(src_data[6], src_data[7]);

    
    dal_ipmi_log_with_bios_set_check(req_msg, MLOG_INFO, FC__PUBLIC_OK,
        "Set bios setting file changed flag to (%s) successfully", src_data[6] == 1 ? "changed" : "no changed");
    

    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);
    (void)dal_get_product_version_num(&product_version);
    if ((PRODUCT_VERSION_V5 >= product_version) && (ENABLE == arm_enable) && (BIOS_FILE_CHANGE_NUM == file_select)) {
        
        ret = fm_clear_file(BIOS_FILE_SETTING_NAME);
        if (ret != FM_OK) {
            debug_log(DLOG_ERROR, "%s: fm_clear_file %s failed", __FUNCTION__, BIOS_FILE_CHANGE_NAME);
        }

        g_bios_config_format = 0;
        per_save((guint8 *)&g_bios_config_format);
        debug_log(DLOG_DEBUG, "BIOS config is ini format.\r\n");
    }
    

    resp_data[0] = BIOS_ERR_NORMALLY;
    return construt_and_respond_msg(req_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
}


LOCAL gint32 get_silk_file_changed(guint8 *changed_flag)
{
    BIOS_DATA_OPERATE_S *data_operate = &g_bios_rd_data_cfg;
    guint8 *bak_buf = NULL;
    guint32 bak_buf_len;

    if (data_operate->data_buf == NULL) {
        debug_log(DLOG_ERROR, "cached buffer is empty, silk file not loaded");
        return RET_ERR;
    }
    *changed_flag = BIOS_SETTING_FILE_CHANGED;
    
    FILE *bak_file_handle = fopen(BIOS_SILK_JSON_BAK_FILE_NAME, "r");
    if (bak_file_handle == NULL) {
        debug_log(DLOG_ERROR, "open silk bak file fail, may not exist");
        return RET_OK;
    }
    gint32 ret = load_file_to_buffer(bak_file_handle, &bak_buf, &bak_buf_len);
    (void)fclose(bak_file_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "load silk bak file fail, conflict may happen");
        return RET_OK;
    }
    
    gchar *data_buf_checksum =
        g_compute_checksum_for_data(G_CHECKSUM_SHA256, data_operate->data_buf, data_operate->data_len);
    gchar *bak_buf_checksum = g_compute_checksum_for_data(G_CHECKSUM_SHA256, bak_buf, bak_buf_len);
    if (data_buf_checksum == NULL || bak_buf_checksum == NULL) {
        debug_log(DLOG_ERROR, "g_compute_checksum_for_data failed, invalid checksum");
        ret = RET_ERR;
        goto EXIT;
    }
    if (g_strcmp0(data_buf_checksum, bak_buf_checksum) == 0) {
        *changed_flag = BIOS_SETTING_FILE_UNCHANGED;
    }
    ret = RET_OK;
EXIT:
    g_free(bak_buf);
    g_free(data_buf_checksum);
    g_free(bak_buf_checksum);
    return ret;
}


gint32 bios_get_file_changed(const void *req_msg, gpointer user_data)
{
    BIOS_FILE_CHANGE_S *file_change = g_bios_file_change;
    guint8 resp_data[6];

    if (req_msg == NULL) {
        
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    const guint8 *src_data = get_ipmi_src_data(req_msg);
    
    gint32 ret_val = judge_manu_id_valid_bios(src_data);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, DB_STR_MUNU_ID_ERR, ret_val);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }

    guint8 data_len = get_ipmi_src_data_len(req_msg);
    if (data_len < BIOS_FILE_SELECT_OFFSET + 1) {
        debug_log(DLOG_ERROR, "%s:Invalid cmd length(%u)", __FUNCTION__, data_len);
        resp_data[0] = BIOS_ERR_IV_LEN;
        return construt_and_respond_msg(req_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }
    guint8 file_select = src_data[5]; // 参考接口说明，表示目标文件的标号
    if (file_select >= BIOS_FILE_MAX_SELECTOR) {
        debug_log(DLOG_ERROR, "file_select:%d(Max:%d) is invalid!\n", file_select, BIOS_FILE_MAX_SELECTOR);
        goto ERROR_EXIT;
    }
    if (file_select == BIOS_FILE_SILK_CONFIG_NUM) {
        guint8 changed_flag;
        if (get_silk_file_changed(&changed_flag) != RET_OK) {
            goto ERROR_EXIT;
        }
        resp_data[0] = BIOS_ERR_NORMALLY;
        resp_data[4] = changed_flag; // 参考接口说明，00h -No changed，01h -Changed
        resp_data[5] = 0;            // 5 -Last Changed Channel， 丝印文件不涉及，直接赋值0
        return construt_and_respond_msg(req_msg, resp_data, sizeof(resp_data));
    }
    
    guint8 bios_id = src_data[BIOS_ID_OFFSET];
    if ((bios_id > BIOS_ID) || (bios_id != (file_change + 0)->file_num)) {
        debug_log(DLOG_ERROR, "BiosId:%d(MaxId:%d) is invalid!\n", bios_id, BIOS_ID);
        goto ERROR_EXIT;
    }
    resp_data[4] = (file_change + 0)->file_change_flag; // 参考接口说明，00h -No changed，01h -Changed
    resp_data[5] = (file_change + 0)->file_channel;     // Last Changed Channel
    resp_data[0] = BIOS_ERR_NORMALLY;
    return construt_and_respond_msg(req_msg, resp_data, sizeof(resp_data));
ERROR_EXIT:
    resp_data[0] = BIOS_ERR_INVALID_STATUS;
    return construt_and_respond_msg(req_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
}


gint32 bios_add_log_entry(const void *req_msg, gpointer user_data)
{
    guint8 resp_data[4];
    gchar log_detail[BIOS_IPMI_CMD_MAX_LEN] = {0};
    gchar target[BIOS_IPMI_CMD_MAX_LEN] = {0};

    if (req_msg == NULL) {
        
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    

    const gchar *src_data = (const gchar *)get_ipmi_src_data(req_msg);
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, DB_STR_GET_SRC_DATA_FAIL);
        
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
        
    }

    
    gint32 ret_val = judge_manu_id_valid_bios((const guint8 *)src_data);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, DB_STR_MUNU_ID_ERR, ret_val);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }

    
    guint8 event_type = src_data[4] + 1;

    
    guint8 data_len = get_ipmi_src_data_len(req_msg);
    
    
    if (data_len <= BIOS_ADD_LOG_MIN_LEN) {
        debug_log(DLOG_ERROR, "%s:Invalid cmd length(%u)", __FUNCTION__, data_len);
        resp_data[0] = BIOS_ERR_IV_LEN;
        return construt_and_respond_msg(req_msg, resp_data, sizeof(resp_data));
    }
    const gchar *first_separator_location = strchr(src_data + BIOS_ADD_LOG_MIN_LEN, ',');

    

    const gchar *last_separator_location = strrchr(src_data + BIOS_ADD_LOG_MIN_LEN, ',');

    
    if (first_separator_location == NULL) {
        debug_log(DLOG_ERROR, "Log message format is incorrect!");
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }

    if (first_separator_location <= (src_data + BIOS_ADD_LOG_MIN_LEN) ||
        last_separator_location == (src_data + data_len - 1) || first_separator_location == last_separator_location) {
        debug_log(DLOG_ERROR, "Log message format is incorrect!");
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }

    guint8 target_len = first_separator_location - src_data - BIOS_ADD_LOG_MIN_LEN;

    
    if (strncpy_s(target, sizeof(target), src_data + BIOS_ADD_LOG_MIN_LEN, target_len) != EOK) {
        debug_log(DLOG_ERROR, "strncpy_s failed");
        return ipmi_send_simple_response(req_msg, COMP_CODE_MEMOUT);
    }

    if (data_len <= BIOS_ADD_LOG_MIN_LEN + target_len + 1) {
        debug_log(DLOG_ERROR, "%s: exceeded array length", __FUNCTION__);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }
    guint8 log_detail_len = data_len - BIOS_ADD_LOG_MIN_LEN - target_len - 1; // 减1为减去逗号的长度

    
    if (strncpy_s(log_detail, sizeof(log_detail), first_separator_location + 1, log_detail_len) != EOK) {
        debug_log(DLOG_ERROR, "[%s] strncpy_s failed", __FUNCTION__);
        return ipmi_send_simple_response(req_msg, COMP_CODE_MEMOUT);
    }

    debug_log(DLOG_DEBUG, "event_type:%u,target:%s,detail:%s\n", event_type, target, log_detail);

    resp_data[0] = BIOS_ERR_NORMALLY;

    if (event_type == IMANA_LOG_OPERATION) {
        ipmi_operation_log(req_msg, "%s\n", log_detail);
    } else if (IMANA_LOG_SECURITY == event_type) {
        security_log("%s,%s,%s\n", "Security", target, log_detail);
    } else {
        resp_data[0] = COMP_CODE_INVALID_FIELD;
    }

    vos_task_delay(100); 
    
    file_log_flush();

    return construt_and_respond_msg(req_msg, resp_data, sizeof(resp_data));
}


LOCAL void get_bios_boot_oem_write_prot(const void *req_msg, gpointer user_data, guint8 *resp_data,
    guint8 resp_data_len, guint8 *resp_len)
{
    resp_data[0] = COMP_CODE_INVALID_CMD;
}


LOCAL void get_bios_boot_progress(const void *req_msg, gpointer user_data, guint8 *resp_data, guint8 resp_data_len,
    guint8 *resp_len)
{
    BOOT_OPTIONS_S *boot_option = &g_boot_options;
    *resp_len = BIOS_IPMI_CMD_MIN_LEN;
    resp_data[3] = boot_option->set_in_progress;
}


LOCAL void get_bios_boot_partition_sel(const void *req_msg, gpointer user_data, guint8 *resp_data, guint8 resp_data_len,
    guint8 *resp_len)
{
    BOOT_OPTIONS_S *boot_option = &g_boot_options;
    *resp_len = BIOS_IPMI_CMD_MIN_LEN;
    resp_data[3] = boot_option->service_partition_selector;
}


LOCAL void get_bios_boot_partition_scan(const void *req_msg, gpointer user_data, guint8 *resp_data,
    guint8 resp_data_len, guint8 *resp_len)
{
    BOOT_OPTIONS_S *boot_option = &g_boot_options;
    *resp_len = BIOS_IPMI_CMD_MIN_LEN;
    resp_data[3] = boot_option->service_partition_scan;
}


LOCAL void get_bios_boot_flag_valid(const void *req_msg, gpointer user_data, guint8 *resp_data, guint8 resp_data_len,
    guint8 *resp_len)
{
    BOOT_OPTIONS_S *boot_option = &g_boot_options;
    *resp_len = BIOS_IPMI_CMD_MIN_LEN;
    resp_data[3] = boot_option->boot_flag_valid_bit_clearing;
}


LOCAL void get_bios_boot_acknowledge(const void *req_msg, gpointer user_data, guint8 *resp_data, guint8 resp_data_len,
    guint8 *resp_len)
{
    BOOT_OPTIONS_S *boot_option = &g_boot_options;
    *resp_len = 5;
    errno_t safe_fun_ret =
        memmove_s(resp_data + 3, resp_data_len - 3, boot_option->boot_info_acknowledge, BIOS_BOOT_ACK_INFO_NUM);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
}


LOCAL void get_bios_boot_initiator_info(const void *req_msg, gpointer user_data, guint8 *resp_data,
    guint8 resp_data_len, guint8 *resp_len)
{
    BOOT_OPTIONS_S *boot_option = &g_boot_options;
    *resp_len = BIOS_BOOT_INIT_INFO_NUM + 3;
    errno_t safe_fun_ret =
        memmove_s(resp_data + 3, resp_data_len - 3, boot_option->boot_initiator_info, BIOS_BOOT_INIT_INFO_NUM);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
}


LOCAL void get_bios_boot_initiator_mailbox(const void *req_msg, gpointer user_data, guint8 *resp_data,
    guint8 resp_data_len, guint8 *resp_len)
{
    BOOT_OPTIONS_S *boot_option = &g_boot_options;

    const guint8 *src_data = get_ipmi_src_data(req_msg);
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, "src_data is NULL!");
        return;
    }

    const guint8 *para_data = &(src_data[1]);

    if (para_data[0] >= BIOS_MAILBOX_SEG_MAX_NUM) {
        resp_data[0] = COMP_CODE_OUTOF_RANGE;
    } else {
        *resp_len = 20;
        resp_data[3] = para_data[0];
        errno_t safe_fun_ret = memmove_s(resp_data + BIOS_IPMI_CMD_MIN_LEN, resp_data_len - BIOS_IPMI_CMD_MIN_LEN,
            &(boot_option->boot_initiator_mailbox[BIOS_BOOT_VALID_SEG_NUM * para_data[0]]), BIOS_BOOT_VALID_SEG_NUM);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    }
}


gint32 bios_get_system_boot_options(const void *req_msg, gpointer user_data)
{
    guint8 resp_len = 1;
    guint8 resp_data[24] = {0};
    gint32 ipmi_ret = -1;
    BIOS_BOOT_OPTION_FUN bios_boot_temp = NULL;

    if (req_msg == NULL) {
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    const guint8 *src_data = get_ipmi_src_data(req_msg);
    
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, DB_STR_GET_SRC_DATA_FAIL);
        ipmi_ret = ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
        return ipmi_ret;
    }
    

    STRUCT_BIOS_BOOT_CMD_INFO bios_boot_cmd_info_arr[] =
    {
        {BIOS_BOOT_OEM_WRITE_PROT_CMD, get_bios_boot_oem_write_prot},
        {BIOS_BOOT_PROGRESS_CMD, get_bios_boot_progress},
        {BIOS_BOOT_FLAGS_CMD, get_bios_boot_flags},
        {BIOS_BOOT_PARTITION_SEL_CMD, get_bios_boot_partition_sel},
        {BIOS_BOOT_PARTITION_SCAN_CMD, get_bios_boot_partition_scan},
        {BIOS_BOOT_FLAG_VALID_CMD, get_bios_boot_flag_valid},
        {BIOS_BOOT_ACKNOWLEDGE_CMD, get_bios_boot_acknowledge},
        {BIOS_BOOT_INITIATOR_INFO_CMD, get_bios_boot_initiator_info},
        {BIOS_BOOT_INITIATOR_MAILBOX_CMD, get_bios_boot_initiator_mailbox}
    };

    guint8 sub_cmd = src_data[0] & NBIT7;
    resp_data[0] = COMP_CODE_SUCCESS;
    resp_data[1] = BOOT_OPTION_PARAMETER_VERSION;

    guint8 boot_valid = get_boot_option_valid(sub_cmd);
    resp_data[2] = (guint8)(LEFT_SHIFT7(boot_valid) | sub_cmd);

    debug_log(DLOG_DEBUG, "SubCmd:%d Valid:%d!\n", sub_cmd, boot_valid);

    guint8 bios_boot_cmd_info_count = (guint8)ARRAY_SIZE(bios_boot_cmd_info_arr);
    for (guint8 i = 0; i < bios_boot_cmd_info_count; i++) {
        if (bios_boot_cmd_info_arr[i].bios_boot_cmd_type == sub_cmd) {
            bios_boot_temp = bios_boot_cmd_info_arr[i].bios_boot_option_fun;
            break;
        }
    }

    if (bios_boot_temp != NULL) {
        bios_boot_temp(req_msg, user_data, resp_data, sizeof(resp_data), &resp_len);
    } else {
        resp_data[0] = COMP_CODE_PARAM_NOT_SUPPORTED;
    }

    if (resp_data[0] == COMP_CODE_SUCCESS) {
        return ipmi_send_response(req_msg, resp_len, resp_data);
    } else {
        return ipmi_send_simple_response(req_msg, resp_data[0]);
    }
}


gint32 bios_update_bios_status(const void *req_msg, gpointer user_data)
{
    OBJ_HANDLE obj_handle = 0;
    guint8 resp_data[4];

    if (req_msg == NULL) {
        
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    const guint8 *src_data = get_ipmi_src_data(req_msg);
    if (src_data == NULL) {
        ipmi_operation_log(req_msg, BIOS_OP_LOG_DEFAULT_CFG);
        debug_log(DLOG_ERROR, DB_STR_GET_SRC_DATA_FAIL);
        
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
        
    }

    resp_data[0] = COMP_CODE_INVALID_CMD;

    
    const UPDATE_BIOS_STATUS_S *bios_status = (const UPDATE_BIOS_STATUS_S *)(&src_data[0]);
    gint32 ret_val = judge_manu_id_valid_bios(bios_status->manufacture);
    if (ret_val != RET_OK) {
        ipmi_operation_log(req_msg, BIOS_OP_LOG_DEFAULT_CFG);
        debug_log(DLOG_ERROR, DB_STR_MUNU_ID_ERR, ret_val);
        goto update_bios_exit;
    }

    if (bios_status->status == BIOS_LOAD_DEFAULT_CONF) {
        
        ret_val = dfl_get_binded_object(BIOS_CLASS_NAME, BIOS_OBJECT_HANDLE_NAME, &obj_handle);
        if (ret_val != DFL_OK) {
            ipmi_operation_log(req_msg, BIOS_OP_LOG_DEFAULT_CFG);
            
            debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
            
            return ipmi_send_simple_response(req_msg, COMP_CODE_CANNOT_RESPONSE);
            
        }

        
        GVariant *property_data = g_variant_new_byte(BMC_DISABLE);
        ret_val = dfl_set_property_value(obj_handle, BIOS_CLEAR_CMOS_NAME, property_data, DF_HW);
        if (ret_val != DFL_OK) {
            ipmi_operation_log(req_msg, BIOS_OP_LOG_DEFAULT_CFG);
            g_variant_unref(property_data);
            debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
            
            return ipmi_send_simple_response(req_msg, COMP_CODE_CANNOT_RESPONSE);
            
        }

        g_variant_unref(property_data);

        resp_data[0] = COMP_CODE_SUCCESS;

        ipmi_operation_log(req_msg, "Set BIOS status to (default config) successfully");
    }

update_bios_exit:

    resp_data[1] = bios_status->manufacture[0];
    resp_data[2] = bios_status->manufacture[1];
    resp_data[3] = bios_status->manufacture[2];

    return ipmi_send_response(req_msg, sizeof(resp_data), resp_data);
}


gint32 bios_set_certificate_overdue_status(const void *req_msg, gpointer user_data)
{
#define CERT_OVERDUE_STATUS_BYTE 6
#define CERT_MONITOR_ID 108

    guchar cert_monitor_id = CERT_MONITOR_ID;
    OBJ_HANDLE cert_monitor_handle = 0;

    GSList *input_list = NULL;

    if (req_msg == NULL) {
        debug_log(DLOG_ERROR, "%s: Param pointer is null", __func__);
        dal_ipmi_log_with_bios_set_check(req_msg, MLOG_INFO, FC__PUBLIC_OK,
            "set bios certificate overdue status failed");
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    
    const guint8 *src_data = get_ipmi_src_data(req_msg);
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, "%s: get_ipmi_src_data fail!\n", __func__);
        dal_ipmi_log_with_bios_set_check(req_msg, MLOG_INFO, FC__PUBLIC_OK,
            "set bios certificate overdue status failed");
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    guint8 state = src_data[CERT_OVERDUE_STATUS_BYTE];
    if (state != 1 && state != 0) {
        debug_log(DLOG_ERROR, "%s: Invalid parameter!, src_data[6] is %u", __func__, state);
        dal_ipmi_log_with_bios_set_check(req_msg, MLOG_INFO, FC__PUBLIC_OK,
            "set bios certificate overdue status failed");
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    gint32 ret = dal_get_specific_object_byte(CERT_MONITOR_CLASS, CERT_ID, cert_monitor_id, &cert_monitor_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: cann't get CertMonitor obj, cert_monitor_id:%u, ret:%d", __func__, cert_monitor_id,
            ret);
        dal_ipmi_log_with_bios_set_check(req_msg, MLOG_INFO, FC__PUBLIC_OK,
            "set bios certificate overdue status failed");
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }

    // set overdue status
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte((guchar)state));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_uint32(DF_SAVE));
    ret = dfl_call_class_method(cert_monitor_handle, METHOD_SETOVERDUE_STATUS, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: set bios certificate overdue status failed, cert_monitor_id:%u, ret:%d", __func__,
            cert_monitor_id, ret);
        dal_ipmi_log_with_bios_set_check(req_msg, MLOG_INFO, FC__PUBLIC_OK,
            "set bios certificate overdue status failed");
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }

    dal_ipmi_log_with_bios_set_check(req_msg, MLOG_INFO, FC__PUBLIC_OK,
        "set bios certificate overdue status successfully");
    return ipmi_send_simple_response(req_msg, USER_COMMAND_NORMALLY);
}


gint32 bios_update_post_status(const void *req_msg, gpointer user_data)
{
    guint8 resp_data[4] = {0};

    if (req_msg == NULL) {
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    
    const guint8 *src_data = get_ipmi_src_data(req_msg);
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, DB_STR_GET_SRC_DATA_FAIL);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    
    guint32 manu_id = MAKE_DWORD(0, src_data[2], src_data[1], src_data[0]);
    if (g_manufacture_id_bios != manu_id) {
        debug_log(DLOG_ERROR, "ManufactureId:%d(Should be:%u) is invalid!\n", manu_id, g_manufacture_id_bios);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    if (src_data[5] == BIOS_REPORT_POST_FINISH_FLAG) {
        debug_log(DLOG_ERROR, "%s: bios already post finish!\r\n", __FUNCTION__);
        update_bios_startup_state(BIOS_STARTUP_POST_STAGE_FINISH);
    } else {
        debug_log(DLOG_ERROR, "%s:bios update post state failed, bios post status is %u\n", __FUNCTION__, src_data[5]);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }

    resp_data[0] = COMP_CODE_SUCCESS;
    resp_data[1] = LONGB0(manu_id);
    resp_data[2] = LONGB1(manu_id);
    resp_data[3] = LONGB2(manu_id);
    return ipmi_send_response(req_msg, sizeof(resp_data), resp_data);
}


LOCAL gint32 bios_set_teeos_ver(const guint8 *src_data, guint32 data_len)
{
    gchar teeos_version[TEEOS_INFO_MAX_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;

    guint32 ver_len = data_len - TEEOS_MSG_HEAD_MIN_LEN;
    
    errno_t safe_fun_ret =
        memmove_s(teeos_version, sizeof(teeos_version), (const gchar *)&(src_data[TEEOS_MSG_HEAD_MIN_LEN]), ver_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    
    teeos_version[ver_len] = '\0';

    debug_log(DLOG_DEBUG, "InputLen:%d!", ver_len);

    
    gint32 ret_val = dfl_get_binded_object(BIOS_CLASS_NAME, BIOS_OBJECT_HANDLE_NAME, &obj_handle);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return RET_ERR;
    }

    ret_val = dal_set_property_value_string(obj_handle, BMC_TEEOS_VER_NAME, teeos_version, DF_SAVE);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return RET_ERR;
    }

    debug_log(DLOG_ERROR, "Set TEEOS version to (%s)", teeos_version);
    return RET_OK;
}


gint32 bios_set_teeos_info(const void *req_msg, gpointer user_data)
{
    guint8 resp_data[BIOS_MSG_HEAD_MIN_LEN];
    const guint8 info_category_byte = 4;

    if (req_msg == NULL) {
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }
    guint32 data_len = get_ipmi_src_data_len(req_msg);
    
    guint32 valid_len = data_len - TEEOS_MSG_HEAD_MIN_LEN;
    if ((valid_len == 0) || (valid_len > TEEOS_INFO_MAX_LEN)) {
        debug_log(DLOG_ERROR, "Request msg len:%d is invalid!", valid_len);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }

    const guint8 *src_data = get_ipmi_src_data(req_msg);
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, DB_STR_GET_SRC_DATA_FAIL);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    
    gint32 ret_val = judge_manu_id_valid_bios(src_data);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, DB_STR_MUNU_ID_ERR, ret_val);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }
    
    switch (src_data[info_category_byte]) {
        case TEEOS_VER_INFO:
            ret_val = bios_set_teeos_ver(src_data, data_len);
            if (ret_val != RET_OK) {
                debug_log(DLOG_ERROR, "Set TEEOS version failed, ret = %d", ret_val);
                return ipmi_send_simple_response(req_msg, COMP_CODE_CANNOT_RESPONSE);
            }
            break;
        default:
            return ipmi_send_simple_response(req_msg, COMP_CODE_CANNOT_RESPONSE);
    }

    
    resp_data[0] = COMP_CODE_SUCCESS;

    
    ret_val = construt_and_respond_msg(req_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
    }

    return ret_val;
}



LOCAL gint32 boot_option_init(void)
{
    
    BOOT_OPTIONS_S *boot_option = &g_boot_options;

    // 初始化启动参数
    (void)memset_s(boot_option, sizeof(BOOT_OPTIONS_S), 0, sizeof(BOOT_OPTIONS_S));

    // 初始化设置全部参数非法
    for (guint32 i = 0; i < sizeof(boot_option->valid_flag); i++) {
        boot_option->valid_flag[i] = 0xff;
    }

    

    
    set_boot_option_valid(BIOS_BOOT_PARTITION_SCAN_CMD, BOOT_OPTION_VALID);

    set_boot_option_valid(BIOS_BOOT_OEM_WRITE_PROT_CMD, BOOT_OPTION_VALID);

    
    boot_option->boot_flags[0] |= PBIT7;

    boot_option->set_in_progress = BOOTOPTION_SET_COMPLETE;

    return RET_OK;
}


LOCAL gint32 boot_option_default_init(void)
{
    guint8 boot_mode = 0;
    guint8 boot_mode_support_flag = 0;
    OBJ_HANDLE obj_handle = 0;
    guint8 boot_start_option = 0;
    guint8 boot_start_option_flag = 0;

    BOOT_OPTIONS_S *boot_option = &g_boot_options;

    gint32 ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &obj_handle);
    if (ret == RET_OK) {
        debug_log(DLOG_DEBUG, "boot_option->boot_flags[0]=0x%x boot_option->boot_flags[1]=0x%x",
            boot_option->boot_flags[0], boot_option->boot_flags[1]);

        (void)dal_get_property_value_byte(obj_handle, BIOS_BOOT_MODE_SUPPORT_FLAG, &boot_mode_support_flag);
        (void)dal_get_property_value_byte(obj_handle, BIOS_BOOT_MODE, &boot_mode);
        (void)dal_get_property_value_byte(obj_handle, BIOS_START_OPTION_NAME, &boot_start_option);
        (void)dal_get_property_value_byte(obj_handle, BIOS_START_OPTION_FLAG_NAME, &boot_start_option_flag);

        
        if (BOOT_MODE_SUPPORT_FLAG_ENABLE == boot_mode_support_flag) {
            
            boot_option->boot_flags[0] |= ((boot_mode << 5) & PBIT5);
            
            ret = write_boot_mode_to_setting_json(boot_mode);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "write_boot_mode_to_setting_json fail!ret:%d", ret);
            }
        }

        
        boot_option->boot_flags[1] = ((boot_start_option & 0x0f) << 2) | (boot_option->boot_flags[1] & 0xc3);
        
        ret = modify_boot_options_boot_flag(boot_start_option_flag);

        debug_log(DLOG_DEBUG, "boot_mode_support_flag=0x%x, boot_mode=0x%x, boot_option->boot_flags[0]=0x%x "
            "boot_option->boot_flags[1]=0x%x",
            boot_mode_support_flag, boot_mode, boot_option->boot_flags[0], boot_option->boot_flags[1]);

        
        if (ret == RET_OK) {
            return RET_OK;
        } else {
            debug_log(DLOG_ERROR, "modify_boot_options_boot_flag fail!ret:%d", ret);
        }
        
    }

    
    ret = per_save((guint8 *)&g_boot_options);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Persistance g_boot_options statistic info fail!");
    }

    return RET_OK;
}



LOCAL gint32 process_bios_class_handle(OBJ_HANDLE obj_handle, gpointer user_data)
{
    gint32 ret_val = dfl_bind_object_alias(obj_handle, BIOS_OBJECT_HANDLE_NAME);
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return ret_val;
    }

    return RET_OK;
}


LOCAL gint32 process_smbios_class_handle(OBJ_HANDLE obj_handle, gpointer user_data)
{
    gint32 ret_val = dfl_bind_object_alias(obj_handle, BIOS_OBJECT_SMBIOS_NAME);
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
    }

    return ret_val;
}


LOCAL gint32 file_register_init(const gchar *obj_name, gchar *property_name, gint32 dst_flag, FM_PROTECT_E mode)
{
    OBJ_HANDLE obj_handle = 0;
    gsize string_len = 0;
    GVariant *property_data = NULL;
    const gchar *temp = NULL;
    FM_REGISTER_S file_reg = { 0 };
    gchar property_file_name[BIOS_FILE_NAME_MAX_LEN] = {0};

    if (obj_name == NULL) {
        return RET_ERR;
    }
    if (property_name == NULL) {
        return RET_ERR;
    }

    gint32 ret_val = dfl_get_object_handle(obj_name, &obj_handle);
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return RET_ERR;
    }

    ret_val = dfl_get_property_value(obj_handle, property_name, &property_data);
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return RET_ERR;
    }

    
    temp = g_variant_get_string(property_data, &string_len);
    if ((string_len <= 0) || (string_len >= BIOS_FILE_NAME_MAX_LEN)) {
        g_variant_unref(property_data);
        debug_log(DLOG_ERROR, "FileNameLen:%" G_GSIZE_FORMAT " is err!\n", string_len);
        return RET_ERR;
    }

    errno_t safe_fun_ret = memmove_s(property_file_name, sizeof(property_file_name), temp, string_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }

    file_reg.short_name = property_name;
    file_reg.dst_name = (BMC_ENABLE == dst_flag) ? property_file_name : NULL;
    
    file_reg.mode = mode;
    
    file_reg.package_size = 0;
    file_reg.package_notify = NULL;
    ret_val = fm_init(&file_reg, 1);
    if (ret_val != RET_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
    }

    g_variant_unref(property_data);

    return ret_val;
}


LOCAL gint32 get_and_save_manufacture_id(void)
{
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_data = NULL;

    gint32 ret_val = dfl_get_object_handle(BMC_OBJECT_NAME, &obj_handle);
    
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
    

    g_manufacture_id_bios = g_variant_get_uint32(property_data);
    g_variant_unref(property_data);

    g_rsp_manufactureid_bios = dal_get_manu_id();
    return RET_OK;
}

LOCAL void register_bios_file(void)
{
    file_register_init(SMBIOS_OBJECT_NAME, SMBIOS_FILE_NAME, BMC_DISABLE, FM_PROTECT_POWER_OFF);
    file_register_init(SMBIOS_OBJECT_NAME, SMBIOS_DIFF_FILE_NAME, BMC_DISABLE, FM_PROTECT_POWER_OFF);
    file_register_init(BIOS_OBJECT_NAME, BIOS_FILE_OPTION_NAME, BMC_ENABLE, FM_PROTECT_POWER_OFF);
    file_register_init(BIOS_OBJECT_NAME, BIOS_FILE_CHANGE_NAME, BMC_ENABLE, FM_PROTECT_POWER_OFF);
    file_register_init(BIOS_OBJECT_NAME, BIOS_FILE_DISPLAY_NAME, BMC_ENABLE, FM_PROTECT_POWER_OFF);
    file_register_init(BIOS_OBJECT_NAME, PROPERTY_BIOS_CLP_NAME, BMC_ENABLE, FM_PROTECT_POWER_OFF);
    file_register_init(BIOS_OBJECT_NAME, PROPERTY_BIOS_CLP_RESP_NAME, BMC_ENABLE, FM_PROTECT_POWER_OFF);

    file_register_init(BIOS_OBJECT_NAME, BIOS_FILE_REGISTRY_NAME, BMC_ENABLE, FM_PROTECT_POWER_OFF);
    file_register_init(BIOS_OBJECT_NAME, BIOS_FILE_CURRENT_VALUE_NAME, BMC_ENABLE, FM_PROTECT_POWER_OFF);
    file_register_init(BIOS_OBJECT_NAME, BIOS_FILE_SETTING_NAME, BMC_ENABLE, FM_PROTECT_POWER_OFF_E);
    file_register_init(BIOS_OBJECT_NAME, BIOS_FILE_RESULT_NAME, BMC_ENABLE, FM_PROTECT_POWER_OFF);
    file_register_init(BIOS_OBJECT_NAME, BIOS_FILE_POLICYCONFIGREGISTRY_NAME, BMC_ENABLE, FM_PROTECT_POWER_OFF);
    file_register_init(BIOS_OBJECT_NAME, BIOS_FILE_COMPONENTVERSION_NAME, BMC_ENABLE, FM_PROTECT_POWER_OFF);

    file_register_init(BIOS_OBJECT_NAME, BIOS_FILE_CONFIGVALUE_NAME, BMC_ENABLE, FM_PROTECT_POWER_OFF);

    file_register_init(BIOS_OBJECT_NAME, PROPERTY_BIOS_CMES_FILE_NAME, BMC_ENABLE, FM_PROTECT_POWER_OFF);

    bios_secureboot_file_init();
}

LOCAL gint32 bios_init_foreach_object(void)
{
    gint32 ret_val = dfl_foreach_object(BIOS_CLASS_NAME, process_bios_class_handle, NULL, NULL);
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return ret_val;
    }

    ret_val = dfl_foreach_object(SMBIOS_CLASS_NAME, process_smbios_class_handle, NULL, NULL);
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return ret_val;
    }

    
    ret_val = get_and_save_manufacture_id();
    if (ret_val != RET_OK) {
        
        debug_log(DLOG_ERROR, "get_and_save_manufacture_id fail!\n");
        return ret_val;
    }
    return ret_val;
}


#ifdef ENABLE_ASAN
int __lsan_do_recoverable_leak_check(void);
LOCAL void mem_leak_handler(int sig)
{
    debug_log(DLOG_ERROR, "mem_leak_handler start");
    __lsan_do_recoverable_leak_check();
    debug_log(DLOG_ERROR, "mem_leak_handler end");
}
#endif

LOCAL void init_mutex(void)
{
    init_bios_rollback_mutex();
    init_bios_rd_data_cfg_mutex();
    init_smbios_wr_data_cfg_mutex();
    init_bdf_comm_mutex();
    init_file_op_comm_mutex();
    init_cert_update_mutex();
}


gint32 bios_init(void)
{
    
    #ifdef ENABLE_ASAN
        if (signal(44, mem_leak_handler) == SIG_ERR) {  // 44 表示执行ASAN检测的信号
            debug_log(DLOG_ERROR, "Could not set signal handler");
        }
    #endif

    OBJ_HANDLE bios_handle = 0;
    GVariant *property_value = NULL;

    
    
    mallopt(M_ARENA_MAX, 1);
    
    mallopt(M_TRIM_THRESHOLD, 4 * 1024);

    init_gcov_signal_for_test();

    
    dal_install_sig_action();

    resource_init();

    gint32 ret_val = bios_init_foreach_object();
    if (ret_val != RET_OK) {
        return ret_val;
    }

    
    register_bios_file();

    boot_option_init();

    ret_val = per_init(g_bios_per_info, sizeof(g_bios_per_info) / sizeof(g_bios_per_info[0]));
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "%s: persistent g_bios_per_info failed, ret : %d", __FUNCTION__, ret_val);
    }

    
    g_bios_setting_file_state = g_bios_file_change[0].file_change_flag == BIOS_SETTING_FILE_CHANGED ?
        BIOS_SETTING_FILE_INEFFECTIVE : BIOS_SETTING_FILE_EFFECTIVE;

    (void)boot_option_default_init();

    
    if (dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &bios_handle) != RET_OK) {
        debug_log(DLOG_INFO, "get bios handle failed\n");
    }

    if (dfl_get_saved_value_v2(bios_handle, BIOS_PRINT_FLAG_NAME, &property_value, DF_SAVE) == DFL_OK) {
        guint8 flag = g_variant_get_byte(property_value);
        g_variant_unref(property_value);
        (void)dal_set_property_value_byte(bios_handle, BIOS_PRINT_FLAG_NAME, flag, DF_HW);
    } else {
        debug_log(DLOG_INFO, "get bios property failed\n");
    }

    clear_clp_config_file(bios_handle, FALSE);

    (void)bios_registry_version_update(TRUE);

    clear_clp_config_file(bios_handle, FALSE);

    init_mutex();

    if (kmc_agent_init_for_modules() != RET_OK) {
        return RET_ERR;
    }

    if (create_ipmi_save_info_queue() != RET_OK) {
        debug_log(DLOG_ERROR, "%s: create bios save info queue failed", __FUNCTION__);
        return RET_ERR;
    }

    return ret_val;
}


gint32 bios_start(void)
{
    TASKID monitor_id = 0;
    (void)vos_task_create(&monitor_id, "MonitorBootFlag", (TASK_ENTRY)monitor_boot_flag, NULL, DEFAULT_PRIORITY);

    TASKID component_info_id = 0;
    (void)vos_task_create(&component_info_id, "UpdateComponentInfo", (TASK_ENTRY)update_component_info,
        NULL, DEFAULT_PRIORITY);

    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get object handle for %s fail", __FUNCTION__, BIOS_CLASS_NAME);
        return RET_OK;
    }

    gchar accessor_name[MAX_NAME_SIZE] = {0};
    ret = dfl_get_property_accessor(obj_handle, BIOS_START_OPTION_NAME, accessor_name, sizeof(accessor_name));
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: BiosBootStage is not accessor, ret=%d", __FUNCTION__, ret);
        return RET_OK;
    }

    guint8 saved_option = NO_OVERRIDE;
    ret = dal_get_saved_value_byte(obj_handle, BIOS_START_OPTION_NAME, &saved_option, DF_SAVE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get saved startoption fail, ret=%d", __FUNCTION__, ret);
    }
    ret = dal_set_property_value_byte(obj_handle, BIOS_START_OPTION_NAME, saved_option, DF_SAVE | DF_AUTO);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set saved startoption%u fail, ret=%d", __FUNCTION__, saved_option, ret);
    }

    guint8 saved_option_ext = NO_OVERRIDE;
    ret = dal_get_saved_value_byte(obj_handle, BIOS_START_OPTION_NAME_EXT, &saved_option_ext, DF_SAVE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get saved startoptionext fail, ret=%d", __FUNCTION__, ret);
    }
    ret = dal_set_property_value_byte(obj_handle, BIOS_START_OPTION_NAME_EXT, saved_option_ext, DF_SAVE | DF_AUTO);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set saved startoptionext%u fail, ret=%d", __FUNCTION__, saved_option_ext, ret);
    }

    gulong task_id = 0;
    ret = vos_task_create(&task_id, "bios_boot_stage_monitor",
        (TASK_ENTRY)bios_boot_stage_monitor, NULL, DEFAULT_PRIORITY);
    return RET_OK;
}

LOCAL void free_property_value(GVariant **property_value)
{
    if (*property_value != NULL) {
        g_variant_unref(*property_value);
        *property_value = NULL;
    }
}


LOCAL gint32 nic_save_osca_port_type(OBJ_HANDLE object_handle, gpointer user_data)
{
    GVariant *property_value = NULL;
    NIC_SET_SYSTEM_INFO set_system_info = *(NIC_SET_SYSTEM_INFO *)user_data;
    guint8 port_select = set_system_info.port_select;

    
    gint32 result = dfl_get_property_value(object_handle, ETH_OSCAPORT_ATTRIBUTE_PORTSELECT, &property_value);
    if (result != DFL_OK) {
        return RET_OK;
    }
    guint8 port_select_property = g_variant_get_byte(property_value);
    free_property_value(&property_value);

    
    if (port_select != port_select_property) {
        return RET_OK;
    }

    
    property_value = g_variant_new_byte(set_system_info.port_type);
    result = dfl_set_property_value(object_handle, ETH_OSCAPORT_ATTRIBUTE_PORTTYPE, property_value, DF_SAVE);
    if (result != DFL_OK) {
        free_property_value(&property_value);
        return result;
    }

    free_property_value(&property_value);

    return RET_OK;
}


LOCAL gint32 nic_get_osca_port_type(OBJ_HANDLE object_handle, gpointer user_data)
{
    GVariant *property_value = NULL;
    NIC_SET_SYSTEM_INFO *set_system_info = (NIC_SET_SYSTEM_INFO *)user_data;
    guint8 port_select = set_system_info->port_select;

    
    gint32 result = dfl_get_property_value(object_handle, ETH_OSCAPORT_ATTRIBUTE_PORTSELECT, &property_value);
    if (result != DFL_OK) {
        return RET_OK;
    }
    guint8 port_select_property = g_variant_get_byte(property_value);
    free_property_value(&property_value);

    
    if (port_select != port_select_property) {
        return RET_OK;
    }

    
    result = dfl_get_property_value(object_handle, ETH_OSCAPORT_ATTRIBUTE_PORTTYPE, &property_value);
    if (result != DFL_OK) {
        return RET_OK;
    }
    set_system_info->port_type = g_variant_get_byte(property_value);
    free_property_value(&property_value);

    return RET_OK;
}



gint32 ipmi_set_system_info_parameters(gconstpointer req_msg, gpointer user_data)
{
    // 没有进行实际设置，不需要记录操作日志
    // 这里在现网出现了大量记录的情况，维护组苏楚光已提意见。
    return ipmi_send_simple_response(req_msg, COMP_CODE_PARAM_NOT_SUPPORTED);
}



gint32 ipmi_set_osca_port_type(const void *msg_data, gpointer user_data)
{
    gint32 result = RET_OK;
    NIC_SET_SYSTEM_INFO set_system_info = { 0 };
    guint8 ser_type = 0;

    
    if (msg_data == NULL) {
        return RET_ERR;
    }

    (void)bios_get_server_type(&ser_type);

    if (ser_type != SERVER_TYPE_BLADE) {
        debug_log(DLOG_ERROR, "%s:current server type not support this cmd.\n", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_CMD);
    }

    debug_log(DLOG_INFO, "ipmi_set_osca_port_type start");

    
    const guint8 *req_data_buf = get_ipmi_src_data(msg_data);
    if (req_data_buf == NULL) {
        ipmi_operation_log(msg_data, BIOS_OP_LOG_MEZZ_MODE_1);
        debug_log(DLOG_ERROR, "get_ipmi_src_data fail\r\n");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_CMD);
    }

    if (req_data_buf[2] >= MAX_INTERFACE_NUM || req_data_buf[3] >= TYPE_RESERVED) {
        ipmi_operation_log(msg_data, BIOS_OP_LOG_MEZZ_MODE_1);
        debug_log(DLOG_ERROR, "interface_num:%u or interface_type:%u invalid\r\n", req_data_buf[2], req_data_buf[3]);
        return ipmi_send_simple_response(msg_data, COMP_CODE_OUTOF_RANGE);
    }

    
    set_system_info.port_select = req_data_buf[2];
    result = dfl_foreach_object(ETH_CLASS_NAME_OSCAPORT, nic_get_osca_port_type, &set_system_info, NULL);
    if (result != DFL_OK) {
        ipmi_operation_log(msg_data, BIOS_OP_LOG_MEZZ_MODE, req_data_buf[2], interface_type_str[req_data_buf[3]]);
        debug_log(DLOG_ERROR, "dfl_foreach_object class:%s fail\r\n", ETH_CLASS_NAME_OSCAPORT);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_CMD);
    }

    
    if (set_system_info.port_type != req_data_buf[3]) {
        if (DEFAULT_OSCA_PORT != set_system_info.port_type) {
            set_clp_file_change_flag(TRUE);
        }

        
        set_system_info.port_select = req_data_buf[2];
        set_system_info.port_type = req_data_buf[3];
        result = dfl_foreach_object(ETH_CLASS_NAME_OSCAPORT, nic_save_osca_port_type, &set_system_info, NULL);
        if (result != DFL_OK) {
            ipmi_operation_log(msg_data, BIOS_OP_LOG_MEZZ_MODE, req_data_buf[2], interface_type_str[req_data_buf[3]]);
            debug_log(DLOG_ERROR, "dfl_foreach_object class:%s fail\r\n", ETH_CLASS_NAME_OSCAPORT);
            return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_CMD);
        }
    }

    ipmi_operation_log(msg_data, "Set osca mezz%d mode to (%s) successfully", req_data_buf[2],
        interface_type_str[req_data_buf[3]]);
    return ipmi_send_simple_response(msg_data, COMP_CODE_SUCCESS);
}



gint32 ipmi_get_system_info_parameters(gconstpointer req_msg, gpointer user_data)
{
    guint8 resp_data[2] = {0};
    resp_data[0] = COMP_CODE_PARAM_NOT_SUPPORTED;
    resp_data[1] = 0x11; // Parameter Revision. 11h for IPMI Specification 2.0 revision 1.1
    return ipmi_send_response(req_msg, 2, resp_data);
}



gint32 ipmi_get_osca_port_type(const void *msg_data, gpointer user_data)
{
    gint32 retv = RET_OK;
    guint8 resp_data[5] = {0};
    NIC_SET_SYSTEM_INFO set_system_info = { 0 };
    guint8 ser_type = 0;

    
    if (msg_data == NULL) {
        return COMP_CODE_INVALID_CMD;
    }

    (void)bios_get_server_type(&ser_type);

    if (ser_type != SERVER_TYPE_BLADE) {
        debug_log(DLOG_ERROR, "%s:current server type not support this cmd.\n", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_CMD);
    }

    debug_log(DLOG_INFO, "ipmi_get_osca_port_type start");

    
    const guint8 *req_data_buf = get_ipmi_src_data(msg_data);
    if (req_data_buf == NULL) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    if (req_data_buf[3] >= MAX_INTERFACE_NUM) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_OUTOF_RANGE);
    }

    
    set_system_info.port_select = req_data_buf[3];
    retv = dfl_foreach_object(ETH_CLASS_NAME_OSCAPORT, nic_get_osca_port_type, &set_system_info, NULL);
    if (retv != DFL_OK) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_CMD);
    }

    resp_data[0] = COMP_CODE_SUCCESS;
    resp_data[1] = 0x11;
    resp_data[2] = 0x00;
    resp_data[3] = set_system_info.port_select;
    resp_data[4] = set_system_info.port_type;

    
    set_clp_file_change_flag(FALSE);

    return ipmi_send_response(msg_data, 5, resp_data);
}

LOCAL gint32 write_data_to_bios_cfg_file(FILE *fp_src, FILE *fp_dst, FM_FILE_S *fm_fp_dst, gint32 length_remain)
{
    gint32 ret = RET_OK;
    size_t result_len = 0;
    gint32 single_write_len = MAX_COMP_DATA_BLOCK_LEN;
    guchar single_write_content[MAX_COMP_DATA_BLOCK_LEN] = {0};

    while (length_remain) {
        if (length_remain < MAX_COMP_DATA_BLOCK_LEN) {
            single_write_len = length_remain;
        }

        result_len = fread(single_write_content, 1, single_write_len, fp_src);
        if (result_len != single_write_len) {
            ret = RET_ERR;
            debug_log(DLOG_ERROR, "fread file(result_len: %zu) failed!", result_len);
            break;
        }

        // fm_fp_dst和fp_dst只会有且只有一个不为NULL
        if (fm_fp_dst != NULL) {
            result_len = fm_fwrite(single_write_content, BIOS_FILE_STREAM_SIZE, single_write_len, fm_fp_dst);
        } else if (fp_dst != NULL) {
            result_len = fwrite(single_write_content, BIOS_FILE_STREAM_SIZE, single_write_len, fp_dst);
        } else {
            ret = RET_ERR;
            break;
        }
        if (result_len != single_write_len) {
            ret = RET_ERR;
            debug_log(DLOG_ERROR, "fm_fwrite file(result_len: %zu) failed!", result_len);
            break;
        }
        length_remain -= single_write_len;
    }

    return ret;
}


gint32 write_src_content_to_bios_config(const gchar *src_file_name, const gchar *src_file_mode,
    const glong src_file_len, const gchar *dst_file_name, const gchar *dst_file_mode)
{
    FILE *fp_dst = NULL;

    if (src_file_name == NULL || src_file_mode == NULL || dst_file_name == NULL || dst_file_mode == NULL) {
        debug_log(DLOG_ERROR, "%s: Input parameter error.", __FUNCTION__);
        return RET_ERR;
    }

    // 源文件传入文件绝对路径，以fopen打开文件
    FILE *fp_src = dal_fopen_check_realpath(src_file_name, src_file_mode, src_file_name);
    if (fp_src == NULL) {
        debug_log(DLOG_ERROR, "%s: Open file by dal_fopen_check_realpath failed", __FUNCTION__);
        return RET_ERR;
    }

    // 目标文件传入短文件名时，以fm_fopen打开文件，否则以g_fopen打开文件
    FM_FILE_S *fm_fp_dst = fm_fopen(dst_file_name, dst_file_mode);
    if (fm_fp_dst == NULL) {
        fp_dst = dal_fopen_check_realpath(dst_file_name, dst_file_mode, dst_file_name);
        if (fp_dst == NULL) {
            (void)fclose(fp_src);
            debug_log(DLOG_ERROR, "%s: Open file by dal_fopen_check_realpath failed", __FUNCTION__);
            return RET_ERR;
        }
    }

    gint32 ret = write_data_to_bios_cfg_file(fp_src, fp_dst, fm_fp_dst, src_file_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Write data to bios cfg file fail. ret=%d", __FUNCTION__, ret);
    }

    (void)fclose(fp_src);
    if (fm_fp_dst != NULL) {
        fm_fclose(fm_fp_dst);
    }
    if (fp_dst != NULL) {
        (void)fclose(fp_dst);
    }

    return ret;
}



gint32 bios_notify_pcie_info(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    (void)bios_set_boot_stage(BIOS_BOOT_STAGE_PCIE_INFO_REPORTED);
    debug_log(DLOG_INFO, "%s: pcie card info reported, set bios boot stage completed\n", __FUNCTION__);

    return RET_OK;
}



gint32 read_boot_mode_from_setting_json(guint8 *boot_mode)
{
    gint32 ret = RET_ERR;
    json_object *input_json = NULL;
    json_object *attributes_obj = NULL;
    gchar *data_buf = NULL;
    const gchar *boot_type = NULL;

    if (boot_mode == NULL) {
        debug_log(DLOG_ERROR, "%s : Input parameter error, boot_mode == NULL", __FUNCTION__);
        return RET_ERR;
    }

    
    if (TRUE != vos_get_file_accessible(BIOS_FILE_SETTING_PATH)) {
        debug_log(DLOG_DEBUG, "%s : file don't exist", __FUNCTION__);
        return RET_ERR;
    }

    
    FM_FILE_S *file_handle = fm_fopen(BIOS_FILE_SETTING_NAME, "r");
    if (file_handle == NULL) {
        debug_log(DLOG_ERROR, "%s : Failed to open file %s", __FUNCTION__, BIOS_FILE_SETTING_NAME);
        goto err_exit;
    }

    
    gint32 fm_fseek_back = fm_fseek(file_handle, 0, SEEK_END);
    if (fm_fseek_back != 0) {
        debug_log(DLOG_ERROR, "%s: file seek failed", __FUNCTION__);
        goto err_exit;
    }

    gint32 file_size = fm_ftell(file_handle);
    if (file_size <= 0) {
        debug_log(DLOG_ERROR, "%s : Failed to get file(%s) size, file_size = %d", __FUNCTION__, BIOS_FILE_SETTING_NAME,
            file_size);
        goto err_exit;
    }

    fm_fseek_back = fm_fseek(file_handle, 0, SEEK_SET);
    if (fm_fseek_back != 0) {
        debug_log(DLOG_ERROR, "%s: file seek failed", __FUNCTION__);
        goto err_exit;
    }

    data_buf = (gchar *)g_malloc0(file_size + 1);
    if (data_buf == NULL) {
        debug_log(DLOG_ERROR, "%s : Failed to malloc", __FUNCTION__);
        goto err_exit;
    }

    guint32 file_actual_size = fm_fread(data_buf, BIOS_FILE_STREAM_SIZE, (guint32)file_size, file_handle);
    if (file_size != file_actual_size) {
        debug_log(DLOG_ERROR, "%s : read file(InLen:%d,OutLen:%d) failed", __FUNCTION__, file_size, file_actual_size);
        goto err_exit;
    }

    
    if (strlen(data_buf) == 0) {
        debug_log(DLOG_DEBUG, "setting.json is empty");
        goto err_exit;
    }

    input_json = json_tokener_parse(data_buf);
    if (input_json == NULL) {
        debug_log(DLOG_ERROR, "%s : Failed to call json_tokener_parse", __FUNCTION__);
        goto err_exit;
    }

    
    if (json_object_object_get_ex(input_json, BIOS_BOOT_TYPE_STR, &attributes_obj) == FALSE) {
        debug_log(DLOG_DEBUG, "[%s][%d]  json_object_object_get_ex error \n", __FUNCTION__, __LINE__);
        goto err_exit;
    }

    boot_type = dal_json_object_get_str(attributes_obj);
    if (g_strcmp0(boot_type, BIOS_UEFI_BOOT_STR) == 0) {
        *boot_mode = BIOS_BOOT_UEFI;
    } else if (g_strcmp0(boot_type, BIOS_LEGACY_BOOT_STR) == 0) {
        *boot_mode = BIOS_BOOT_LEGACY;
    } else {
        debug_log(DLOG_ERROR, "%s : value of boot mode is out of range", __FUNCTION__);
        goto err_exit;
    }

    ret = RET_OK;

err_exit:
    if (data_buf != NULL) {
        g_free(data_buf);
    }
    if (input_json != NULL) {
        (void)json_object_put(input_json);
    }
    if (file_handle != NULL) {
        fm_fclose(file_handle);
    }

    return ret;
}



LOCAL void monitor_boot_flag(void)
{
#define WAIT_PERIOD_TIME 10000
    guint32 product_id = 0;
    OBJ_HANDLE stor_payload_handle = 0;
    OBJ_HANDLE bios_handle = 0;
    OBJ_HANDLE smbios_handle = 0;
    guint8 stor_payload_osboot_flag = 0xff;
    guint8 stor_payload_biosboot_flag = 0xff;
    guint8 current_smbios_state = 0xff;
    guint8 current_bios_state = 0xff;

    (void)dal_get_product_id(&product_id);
    if (product_id != PRODUCT_ID_PANGEA_V6) {
        return;
    }
    do {
        vos_task_delay(WAIT_PERIOD_TIME); 
        (void)dal_get_object_handle_nth(CLASS_NAME_STOR_PAYLOAD, 0, &stor_payload_handle);
        (void)dal_get_property_value_byte(stor_payload_handle, PROPERTY_BIOS_BOOT_FLG, &stor_payload_biosboot_flag);
        (void)dal_get_property_value_byte(stor_payload_handle, PROPERTY_OS_BOOT_FLG, &stor_payload_osboot_flag);
    } while (!(stor_payload_osboot_flag == OS_BOOT_FINISHED && stor_payload_biosboot_flag == BIOS_BOOT_FINISHED));

    (void)dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &bios_handle);
    (void)dal_get_property_value_byte(bios_handle, PROPERTY_BIOS_BOOT_STAGE, &current_bios_state);
    if (current_bios_state != stor_payload_biosboot_flag && stor_payload_biosboot_flag == BIOS_BOOT_FINISHED) {
        gint32 ret = bios_set_boot_stage(stor_payload_biosboot_flag);
        (void)ret;
        debug_log(DLOG_DEBUG, "%s: set obj_handle(Bios) (%d) to (%d), ret=(%d).", __FUNCTION__, current_bios_state,
            stor_payload_biosboot_flag, ret);
    } else {
        debug_log(DLOG_DEBUG, "%s: get value is the same, value(%d).", __FUNCTION__, current_bios_state);
    }

    (void)dal_get_object_handle_nth(SMBIOS_CLASS_NAME, 0, &smbios_handle);
    (void)dal_get_property_value_byte(smbios_handle, PROPERTY_SMBIOS_STATUS_VALUE, &current_smbios_state);

    if (current_smbios_state != SMBIOS_WRITE_FINISH && stor_payload_osboot_flag == OS_BOOT_FINISHED) {
        update_smbios_status(SMBIOS_WRITE_FINISH);
        debug_log(DLOG_ERROR, "%s: set obj_handle(SmBios) from (%d) to (%d).", __FUNCTION__, current_smbios_state,
            SMBIOS_WRITE_FINISH);
    }
    debug_log(DLOG_DEBUG, "%s: current_smbios_state(%d) stor_payload_state(%d).", __FUNCTION__, current_smbios_state,
        stor_payload_osboot_flag);
    return;
}


LOCAL gboolean check_read_version_file_selector_valid(guint8 file_index)
{
    switch (file_index) {
        case BIOS_FILE_POLICYCONFIGREGISTRY_NUM:
            return TRUE;

        default:
            return FALSE;
    }
}


gint32 ipmi_cmd_get_property_from_bios_file(gconstpointer req_msg, gpointer user_data)
{
    guint8 resp_data[40];

    if (req_msg == NULL) {
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    const guint8 *src_data = get_ipmi_src_data(req_msg);
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, DB_STR_GET_SRC_DATA_FAIL);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    
    guint8 data_length = get_ipmi_src_data_len(req_msg);
    if (data_length != BIOS_VERSION_IPMI_CMD_LEN) {
        debug_log(DLOG_ERROR, "request data length invalid, length = %d", data_length);
        return ipmi_send_simple_response(req_msg, COMP_CODE_LEN_INVALID);
    }
    
    gint32 ret_val = judge_manu_id_valid_bios(src_data);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, DB_STR_MUNU_ID_ERR, ret_val);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }

    
    if (src_data[BIOS_ID_OFFSET] > BIOS_ID) {
        debug_log(DLOG_ERROR, "BiosId(MaxId:%d) is invalid!", BIOS_ID);
        return ipmi_send_simple_response(req_msg, BIOS_ERR_INVALID_STATUS);
    }

    
    guint8 file_selector = src_data[BIOS_FILE_SELECT_OFFSET];
    if (!check_read_version_file_selector_valid(file_selector)) {
        debug_log(DLOG_ERROR, "read bios file_index:%d is invalid!", file_selector);
        return ipmi_send_simple_response(req_msg, BIOS_ERR_INVALID_STATUS);
    }

    resp_data[0] = BIOS_ERR_NORMALLY;
    resp_data[4] = file_selector;           // 响应的data[4]为文件选择符
    resp_data[5] = 0;                       // 响应的data[5]为数据长度,默认为0

    
    guint8 res_len = 0;
    if (src_data[BIOS_FILE_PROPERTY_SELECT_OFFSET] == 0) {
        gchar file_version[BIOS_CONFIG_VALUE_LEN] = {0};
        gint32 ret = bios_read_version_from_file(BIOS_FILE_POLICYCONFIGREGISTRY_NAME,
            file_version, BIOS_CONFIG_VALUE_LEN);
        if (ret != RET_OK || strlen(file_version) == 0) {
            debug_log(DLOG_ERROR, "%s : Failed to read version, ret = %d", __FUNCTION__, ret);
            return construt_and_respond_msg(req_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN + 2);
        }

        res_len = (guint8)strlen(file_version);
        resp_data[5] = res_len;                // 响应的data[5]为数据长度
        
        if (sizeof(resp_data) - 6 < res_len) {
            debug_log(DLOG_ERROR, "%s: res_len(%u) greater than size of resp_data (%u)", __FUNCTION__, res_len,
                (guint8)sizeof(resp_data));
            return ipmi_send_simple_response(req_msg, COMP_CODE_OUTOF_RANGE);
        }
        for (guint8 i = 0; i < res_len; i++) {
            resp_data[i + 6] = (guint8)file_version[i]; // 响应的data[6]开始为属性值的ASCLL码
        }
    }

    return construt_and_respond_msg(req_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN + res_len + 2);
}