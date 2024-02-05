

#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "pme_app/dalib/dal_obj_priv_data.h"
#include "elabel/elabel.h"
#include "elabel/base.h"
#include "media/e2p_compact.h"
#include "media/e2p_large.h"
#include "media/etc.h"
#include "protocol.h"
#include "media/port_rate.h"
#include "media/custom_mcu.h"
#include "module_mgnt.h"

#define PS_INPUT_TYPE_NONE       0xff
#define PS_INPUT_TYPE_AC         1
#define PS_INPUT_TYPE_DC         0
#define PS_INPUT_TYPE_ACORDC     2
#define PS_POWER_TYPE_AC         0
#define PS_POWER_TYPE_DC         1
#define PS_POWER_TYPE_HVDC       2
#define PS_POWER_TYPE_AC_HVDC    3

#define POWER_SUPPLY_TYPE        0
#define POWER_SUPPLY_OEM_TYPE    0xD0
#define FRU_ESN_DATA_LEN         20

#define PART_NUMBER_LEN          16
#define SUPPLY2_PART_NUMBER_LEN  10  // pod供应 2.0电源砖part_num字段扩展为10字节
#define PART_NUMBER_ESN_LEN      8

#define MAX_FIELD_NUM 8
#define MAX_PRINT_COUNT 10
#define IPMI_CHASSIS_INFO_REQ_DATA_LEN 8

typedef struct {
    guint32 max_len;
    guint32 offset;
} ELABEL_SYSTEM_AREA_INFO;

LOCAL ELABEL_SYSTEM_AREA_INFO g_system_area_table[] = {
    {SYS_MFG_NAME_MAX_LEN, SYS_MFG_NAME_OFFSET},
    {SYS_PRODUCT_NAME_MAX_LEN, SYS_PRODUCT_NAME_OFFSET},
    {SYS_VER_MAX_LEN, SYS_VER_OFFSET},
    {SYS_SN_MAX_LEN, SYS_SN_OFFSET}
};

LOCAL guint8 __delete_one_extend_label(ELABEL_EXTEND_AREA_S *extend_label, const gchar *key);
LOCAL gboolean __ismodify_extend_label(ELABEL_EXTEND_AREA_S *extend_label, const gchar *key, guint8 *buf);
LOCAL gint32 __modify_one_extend_label(ELABEL_EXTEND_AREA_S *extend_label, gchar *key, guint8 *buf, guint16 length);
LOCAL gint32 __update_multi_elabel(guint8 *multi_buf, guint8 *input, guint16 length);

typedef struct {
    guint8 field_number;
    guint8 area_number;
} ELABEL_FIELD_AREA_COMB;

typedef struct tag_ps_oem_record {
    guint8 oemid[3];
    guint8 multi_rec_subtype;
    guint16 low_line_overall_capacity;
    union {
        guint16 pc_ht_us;
        struct {
            guint16 peakcapacity : 12;
            guint16 holduptime : 4;
        } pc_ht_st;
    } pc_ht_un;

    guint8 ps_feature_class;

    union {
        guint8 ps_indentifier_uc;
        struct {
            guint8 psid : 6;
            guint8 input_mode : 2;
        } ps_indentifier_st;
    } ps_indentifier_un;
    guint8 reserved[6];
    guint8 oemname[2];
} PS_OEM_RECORD_S;

LOCAL GMutex g_update_elabel_mutex;


LOCAL void get_common_elabel_string(char *dest_str, guint32 dst_max_len, char *src_str)
{
    guint32 i = 0;

    if (dest_str == NULL || src_str == NULL) {
        return;
    }

    char *ptr = src_str;
    for (; i < dst_max_len - 1; i++) {
        
        if (isgraph(ptr[i])) {
            dest_str[i] = ptr[i];
            continue;
        }

        dest_str[i] = '\0';
        break;
    }
    if (i == dst_max_len - 1) {
        dest_str[i] = '\0';
    }
}


LOCAL void get_extend_elabel_string(char *dest_str, guint32 dst_max_len, const gchar *key_buf, char *src_str)
{
    guint32 i = 0;
    gchar *ptr = src_str;

    if (dest_str == NULL || src_str == NULL) {
        return;
    }

    for (; i < dst_max_len - 1; i++) {
        
        if (ptr[i] == 0x0A || ptr[i] == 0x0D) {
            dest_str[i] = '\0';
            break;
        }
        dest_str[i] = ptr[i];
    }
    if (i == dst_max_len - 1) {
        dest_str[i] = '\0';
    }
}


void get_manufacture_time(const gchar *src_data, FRU_INFO_S *fru_info)
{
    guint32 manuftime;
    char manu_data_str[32] = { 0 }; 
    char *ptr = NULL;

    
    ptr = strstr(src_data, BOARD_ELABEL_KEY_MANUFACTURED);
    if (ptr != NULL) {
        ptr += strlen(BOARD_ELABEL_KEY_MANUFACTURED);
        get_common_elabel_string(manu_data_str, sizeof(manu_data_str), ptr);

        
        if (strlen(manu_data_str) != 0) {
            manuftime = __calc_time_interval(manu_data_str);
            fru_info->board.mfg_date_time[0] = LONGB0(manuftime);
            fru_info->board.mfg_date_time[1] = LONGB1(manuftime);
            fru_info->board.mfg_date_time[2] = LONGB2(manuftime); 

            OBJ_HANDLE obj_handle = 0;
            guint8 elabel_format = 0;
            (void)dfl_get_object_handle(OBJECT_BMC, &obj_handle);
            (void)dal_get_property_value_byte(obj_handle, PROPERTY_BMC_ELABELMANUFORMAT, &elabel_format);
            if (elabel_format == 1) {
                fru_info->board.mfg_date_time[3] = LONGB3(manuftime); 
            }
        }
    }
}


void get_correspond_elabel_data(gchar *all_elabel_data, const gchar *key_buf, FRU_ELABEL_FORMAT_S *elabel_buf)
{
    gchar *ptr = NULL;
    if (all_elabel_data == NULL || key_buf == NULL || elabel_buf == NULL) {
        return;
    }

    ptr = strstr(all_elabel_data, key_buf);
    if (ptr != NULL) {
        ptr += strlen(key_buf);
        get_common_elabel_string((gchar *)elabel_buf->content, sizeof(elabel_buf->content), ptr);
        elabel_buf->len = (guint8)strlen((const gchar *)elabel_buf->content) + 1;
        elabel_buf->type = CHAR_TYPE;
    }
    return;
}


LOCAL gint32 __get_key_data(gchar *key, gint32 key_len, guint8 *input, gchar *p, gchar *p1)
{
    errno_t safe_fun_ret;

    if (p != NULL) {
        safe_fun_ret = memcpy_s(key, key_len, (const gchar *)input, ((guint8 *)p - input + 1));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            return COMP_CODE_UNKNOWN;
        }
    }

    if (p1 == NULL) {
        return RET_OK;
    }

    safe_fun_ret = memcpy_s(key, key_len, (const gchar *)input, ((guint8 *)p1 - input + 1));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return COMP_CODE_UNKNOWN;
    }

    return RET_OK;
}


void get_extend_elabel_data_by_key(gchar *elabel_data, const gchar *key_buf, ELABEL_EXTEND_AREA_S *extend_buf)
{
    gint32 ret;
    gchar *ptr = NULL;
    guint16 str_len;
    gchar value_buf[FRU_EXTERN_LABEL_AREA_LEN - 8] = { 0 };
    gint16 buf_size = FRU_EXTERN_LABEL_AREA_LEN - 8; 

    if (elabel_data == NULL || key_buf == NULL || extend_buf == NULL) {
        return;
    }

    ptr = strstr(elabel_data, key_buf);
    if (ptr == NULL) {
        debug_log(DLOG_ERROR, "key(%s) not exists in extend elabel", key_buf);
        return;
    }

    get_extend_elabel_string(value_buf, buf_size, key_buf, ptr);
    str_len = (guint16)strlen((const gchar *)value_buf);
    if (str_len == 0 || str_len == strlen(key_buf)) {
        debug_log(DLOG_ERROR, "Extend elabel key string len:%d", str_len);
        return;
    }

    ret = update_extend_elabel(extend_buf, (guint8 *)value_buf, str_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s update extend elabel failed", __FUNCTION__);
    }

    return;
}

LOCAL void delet_useless_character(GList *extend_label_l)
{
    errno_t safe_fun_ret;
    while (extend_label_l) {
        GList *next = extend_label_l->next;
        FRU_ELABEL_FORMAT_S *tmp_elabel = (FRU_ELABEL_FORMAT_S *)extend_label_l->data;
        if ((tmp_elabel->content[0] <= 0x0f)
            || ((tmp_elabel->content[0] >= 0x80) && (tmp_elabel->content[0] <= 0x8f))) {
            tmp_elabel->len -= 1;
            safe_fun_ret = memmove_s(tmp_elabel->content, tmp_elabel->len, &(tmp_elabel->content[1]), tmp_elabel->len);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            tmp_elabel->content[tmp_elabel->len] = 0;
        }
        extend_label_l = next;
    }
}
void init_extend_elabel_from_data(char *src_data, guint16 data_size, FRU_INFO_S *fru_info)
{
    ELABEL_EXTEND_AREA_S extend_buf = { 0 };

    get_extend_elabel_data_by_key(src_data, EXTEND_ELABEL_KEY_DESC, &extend_buf);
    get_extend_elabel_data_by_key(src_data, EXTEND_ELABEL_KEY_CLEI_CODE, &extend_buf);
    get_extend_elabel_data_by_key(src_data, EXTEND_ELABEL_KEY_BOM_ID, &extend_buf);
    get_extend_elabel_data_by_key(src_data, EXTEND_ELABEL_KEY_MODEL, &extend_buf);
    copy_extend_label(&(fru_info->board.extension_label), &extend_buf);
    
    delet_useless_character(fru_info->board.extension_label);
}


void init_board_fru_from_data(char *src_data, guint16 data_size, FRU_INFO_S *fru_info)
{
    fru_info->has_board = TRUE;
    fru_info->board.area_ver = 0x01;

    
    get_correspond_elabel_data(src_data, PRODUCT_ELABEL_KEY_BAR_CODE, &(fru_info->board.board_serial_num));

    
    get_correspond_elabel_data(src_data, PRODUCT_ELABEL_KEY_ITEM, &(fru_info->board.board_part_num));

    
    get_manufacture_time(src_data, fru_info);

    
    get_correspond_elabel_data(src_data, PRODUCT_ELABEL_KEY_VEN_NAME, &(fru_info->board.board_manufacturer));

    
    get_correspond_elabel_data(src_data, PRODUCT_ELABEL_KEY_BOARD_TYPE, &(fru_info->board.board_product_name));
}

LOCAL gint32 fru_get_chassis_obj_handle(OBJ_HANDLE *obj_handle)
{
    if (obj_handle == NULL) {
        debug_log(DLOG_ERROR, "%s The obj_handle is null.", __FUNCTION__);
        return RET_ERR;
    }

    GSList *obj_list = NULL;
    gint32 ret_val = dfl_get_object_list(CLASS_NAME_CHASSIS, &obj_list);
    if (ret_val != DFL_OK || obj_list == NULL) {
        debug_log(DLOG_ERROR, "%s dfl_get_object_list %s", __FUNCTION__, CLASS_NAME_CHASSIS);
        return RET_ERR;
    }

    
    if (g_slist_length(obj_list) != 1) {
        debug_log(DLOG_ERROR, "%s g_slist_length fail", __FUNCTION__);
        g_slist_free(obj_list);
        return RET_ERR;
    }

    *obj_handle = (OBJ_HANDLE)g_slist_nth_data(obj_list, 0);
    g_slist_free(obj_list);

    return RET_OK;
}


LOCAL guint8 __delete_one_extend_label(ELABEL_EXTEND_AREA_S *extend_label, const gchar *key)
{
    gchar **split_str = g_strsplit((const gchar *)extend_label->data, ";", MAX_EXTERN_ELABEL_NUM);
    if (split_str == NULL) {
        debug_log(DLOG_ERROR, "%s: g_strsplit error.", __FUNCTION__);
        return COMP_CODE_UNKNOWN;
    }

    gchar **p = split_str;
    gboolean is_find_elabel = FALSE;
    while (*p) {
        if (strncmp(*p, key, strlen(key)) == 0 && !is_find_elabel) { 
            g_free(*p);
            *p = NULL;
            is_find_elabel = TRUE;
        }

        
        if (is_find_elabel) {
            *p = *(p + 1);
        }

        p++;
    }

    
    if (!is_find_elabel) {
        debug_log(DLOG_ERROR, "%s: del elabel(%s) is not found.", __FUNCTION__, key);
        g_strfreev(split_str);
        return COMP_CODE_UNKNOWN;
    }

    
    gchar *new_string = g_strjoinv(";", split_str);
    if (new_string == NULL) {
        debug_log(DLOG_ERROR, "%s: g_strjoinv fail.", __FUNCTION__);
        g_strfreev(split_str);
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }

    guint16 extend_num = extend_label->num;
    (void)memset_s(extend_label, sizeof(ELABEL_EXTEND_AREA_S), 0, sizeof(ELABEL_EXTEND_AREA_S));
    extend_label->num = extend_num;
    extend_label->len = (guint16)strlen(new_string);
    errno_t safe_fun_ret = memcpy_s(extend_label->data, sizeof(extend_label->data), new_string, extend_label->len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    extend_label->crc =
        arith_make_crc_checksum(0, (const guchar *)&extend_label->len, extend_label->len + FRU_EXTERN_LABEL_PREFIX_LEN);

    g_free(new_string);
    g_strfreev(split_str);

    if (extend_label->num > 0) {
        extend_label->num--;
    }
    extend_label->valid_flag = TRUE;

    return COMP_CODE_SUCCESS;
}


LOCAL gboolean __ismodify_extend_label(ELABEL_EXTEND_AREA_S *extend_label, const gchar *key, guint8 *buf)
{
    if (extend_label == NULL || buf == NULL || key == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return 0;
    }

    gchar **split_str = g_strsplit((const gchar *)extend_label->data, ";", MAX_EXTERN_ELABEL_NUM);
    gchar **p = split_str;
    if (p == NULL) {
        return FALSE;
    }

    gboolean ret = FALSE;
    while (*p) {
        if (strncmp(*p, key, strlen(key)) == 0) { 
            g_free(*p);
            *p = g_strdup((const gchar *)buf);
            ret = TRUE;
            break;
        }

        p++;
    }

    g_strfreev(split_str);

    return ret;
}


LOCAL gint32 __modify_one_extend_label(ELABEL_EXTEND_AREA_S *extend_label, gchar *key, guint8 *buf, guint16 length)
{
    if (extend_label == NULL || buf == NULL || key == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return COMP_CODE_UNKNOWN;
    }

    gchar **split_str = g_strsplit((const gchar *)extend_label->data, ";", MAX_EXTERN_ELABEL_NUM);
    if (split_str == NULL) {
        debug_log(DLOG_ERROR, "%s: g_strsplit failed", __FUNCTION__);
        return COMP_CODE_UNKNOWN;
    }

    gchar **p = split_str;
    while (*p) {
        if (!strncmp(*p, key, strlen(key))) { 
            g_free(*p);
            *p = NULL;
            *p = g_strdup((const gchar *)buf);
            break;
        }

        p++;
    }

    
    gchar *new_string = g_strjoinv(";", split_str);
    if (new_string == NULL) {
        debug_log(DLOG_ERROR, "%s: g_strjoinv fail", __FUNCTION__);
        g_strfreev(split_str);
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }

    // According to the definition of ELABEL_EXTEND_AREA_S, all fields except data occupy 8 bytes.
    if (strlen(new_string) + 1 > FRU_EXTERN_LABEL_AREA_LEN - 8) {
        debug_log(DLOG_ERROR, "%s: extend buf is too long, len = %zu.", __FUNCTION__, strlen(new_string) + 1);

        g_free(new_string);
        g_strfreev(split_str);
        return COMP_CODE_OUTOF_RANGE;
    }

    guint16 extend_num = extend_label->num;
    (void)memset_s(extend_label, sizeof(ELABEL_EXTEND_AREA_S), 0, sizeof(ELABEL_EXTEND_AREA_S));
    extend_label->len = (guint16)strlen(new_string);
    errno_t safe_fun_ret = memcpy_s(extend_label->data, sizeof(extend_label->data), new_string, extend_label->len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    extend_label->num = extend_num;
    extend_label->crc =
        arith_make_crc_checksum(0, (const guchar *)&extend_label->len, extend_label->len + FRU_EXTERN_LABEL_PREFIX_LEN);
    extend_label->valid_flag = TRUE;

    g_free(new_string);
    g_strfreev(split_str);

    return COMP_CODE_SUCCESS;
}


gint32 update_extend_elabel(ELABEL_EXTEND_AREA_S *extend_buf, guint8 *input, guint16 length)
{
    if (extend_buf == NULL || input == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return COMP_CODE_UNKNOWN;
    }

    gchar *p = NULL;
    gchar *p1 = NULL;

    
    if (search_equal_symbol_1st_postion(&p, &p1, input) != RET_OK) {
        return COMP_CODE_UNKNOWN;
    }

    
    gchar key[FRU_MAX_ELABEL_LEN + 1] = {0};

    
    if (__get_key_data(key, sizeof(key), input, p, p1) != RET_OK) {
        return COMP_CODE_UNKNOWN;
    }

    
    if (strlen(key) == strlen((const gchar *)input)) {
        return __delete_one_extend_label(extend_buf, key);
    }

    if (__ismodify_extend_label(extend_buf, (const gchar *)key, input)) {
        return __modify_one_extend_label(extend_buf, key, input, length);
    }

    return add_one_extend_label(extend_buf, input, length);
}


gint32 get_fru_priv_data_by_id(guint8 fruid, FRU_PRIV_PROPERTY_S **fru_priv_data, OBJ_HANDLE *fru_handle)
{
    if (fru_priv_data == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return RET_ERR;
    }

    
    gint32 ret = dfl_get_binded_object(CLASS_FRU, fruid, fru_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: call dfl_get_binded_object failed, ret = %d", __FUNCTION__, ret);
        return ret;
    }

    
    ret = get_obj_priv_data(*fru_handle, (gpointer *)(fru_priv_data));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: call dfl_get_binded_object failed, ret = %d!", __FUNCTION__, ret);
    }

    return ret;
}


gint32 send_write_elabel(guint8 fru_id, guint8 area_id, guint8 field_id, const gchar *mmc_elabel)
{
    if (mmc_elabel == NULL) {
        return RET_ERR;
    }

    FRU_PRIV_PROPERTY_S *fru_priv = NULL;
    OBJ_HANDLE fru_handle = 0;
    gint32 ret = get_fru_priv_data_by_id(fru_id, &fru_priv, &fru_handle);
    if (ret != RET_OK || fru_priv == NULL) {
        debug_log(DLOG_ERROR, "%s get_fru_priv_data_by_id fail", __FUNCTION__);
        return ret;
    }

    gchar dft_write_elabel_req[ELABEL_MAX_WRITE_REQ_LEN] = {0};

    // Six bytes are reserved as the prefix for transmission data.
    if ((guint32)(strlen(mmc_elabel)) > (guint32)(sizeof(dft_write_elabel_req) - 6)) {
        debug_log(DLOG_ERROR, "%s lenth fail", __FUNCTION__);
        put_obj_priv_data(fru_handle, (gpointer)fru_priv);
        return ret;
    }

    dft_write_elabel_req[0] = 4;
    dft_write_elabel_req[1] = fru_id;
    dft_write_elabel_req[2] = area_id;
    dft_write_elabel_req[3] = field_id;
    dft_write_elabel_req[4] = 0;
    dft_write_elabel_req[5] = (gchar)strlen(mmc_elabel);

    // Six bytes are reserved as the prefix for transmission data.
    errno_t safe_fun_ret =
        memcpy_s(&dft_write_elabel_req[6], sizeof(dft_write_elabel_req) - 6, mmc_elabel, strlen(mmc_elabel));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        put_obj_priv_data(fru_handle, (gpointer)fru_priv);
        return RET_ERR;
    }

    ret = update_elabel_info(fru_priv, (DFT_WRITE_ELABEL_REQ_S *)dft_write_elabel_req, strlen(mmc_elabel) + 6);
    put_obj_priv_data(fru_handle, (gpointer)fru_priv);

    return ret;
}

LOCAL gint32 validation_check_read_elabel(guint8 *resp_data, guint32 *resp_len, guint8 *end_flag,
    FRU_PRIV_PROPERTY_S *fru_priv, const DFT_READ_ELABEL_REQ_S *dft_read_elabel_req)
{
    if (resp_data == NULL || resp_len == NULL || end_flag == NULL || fru_priv == NULL || dft_read_elabel_req == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return COMP_CODE_UNKNOWN;
    }
    return COMP_CODE_SUCCESS;
}

gint32 read_elabel_info(guint8 *resp_data, guint32 data_len, guint32 *resp_len, guint8 *end_flag,
    FRU_PRIV_PROPERTY_S *fru_priv, const DFT_READ_ELABEL_REQ_S *dft_read_elabel_req)
{
    if (validation_check_read_elabel(resp_data, resp_len, end_flag, fru_priv, dft_read_elabel_req) !=
        COMP_CODE_SUCCESS) {
        return COMP_CODE_UNKNOWN;
    }

    guint32 label_len = 0;
    guint8 *label_buf = NULL;
    gint32 ret =
        get_label_buf_and_len(fru_priv, dft_read_elabel_req->area, dft_read_elabel_req->field, &label_buf, &label_len);
    if (ret != COMP_CODE_SUCCESS) {
        debug_log(DLOG_ERROR, "%s: get_label_buf_and_len failed.", __FUNCTION__);
        return ret;
    }

    if (label_buf == NULL) {
        
        *end_flag = 0x80;
        *resp_len = 0;
        return COMP_CODE_SUCCESS;
    }

    
    if ((label_len == 1) && (strlen((gchar *)label_buf) == 0)) {
        
        
        g_free(label_buf);
        *end_flag = 0x80;
        *resp_len = 0;
        return COMP_CODE_SUCCESS;
    }

    
    guint8 length = dft_read_elabel_req->len;
    if (length == 0) {
        length = label_len;
    }

    guint8 offset = dft_read_elabel_req->flag_offset;
    *end_flag = ((offset + length) < label_len) ? 0x00 : 0x80;
    if (label_len < offset) {
        *resp_len = 0;
        g_free(label_buf);
        return COMP_CODE_SUCCESS;
    }

    
    *resp_len = ((length + offset) > label_len) ? (label_len - offset) : length;
    if (*resp_len != 0) {
        errno_t safe_fun_ret = memcpy_s(resp_data, data_len, label_buf + offset, *resp_len);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }
    }

    g_free(label_buf);
    return COMP_CODE_SUCCESS;
}

gint32 send_peripheral_elabel_cmd(OBJ_HANDLE obj_handle, const guint8 *input, guint32 inputlen, guint8 *output,
    guint32 *outputlen)
{
    if (input == NULL || inputlen == 0) {
        debug_log(DLOG_ERROR, "%s input param fail", __FUNCTION__);
        return RET_ERR;
    }

    if (output == NULL || *outputlen == 0) {
        debug_log(DLOG_ERROR, "%s output param fail", __FUNCTION__);
        return RET_ERR;
    }

    GVariant *variant = g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, input, inputlen, sizeof(guint8));
    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, (gpointer)variant);
    GSList *output_list = NULL;
    gint32 ret = dfl_call_class_method(obj_handle, METHOD_SET_PERIPHERAL_ELABEL, NULL, input_list, &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_call_class_method(%s) failed: ret %d", METHOD_SET_PERIPHERAL_ELABEL, ret);
        return RET_ERR;
    }
    gsize tmp_out_len = 0;
    const guint8 *tmp_out = (const guint8 *)g_variant_get_fixed_array((GVariant *)g_slist_nth_data(output_list, 0),
        &tmp_out_len, sizeof(guint8));
    if (tmp_out == NULL) {
        debug_log(DLOG_ERROR, "%s:g_variant_get_fixed_array return NULL error.", __FUNCTION__);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        return RET_ERR;
    }

    if (*outputlen < tmp_out_len) {
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        return RET_ERR;
    }

    errno_t safe_fun_ret = memcpy_s(output, *outputlen, tmp_out, tmp_out_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    *outputlen = tmp_out_len;
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    return ret;
}


LOCAL gint32 __refresh_extend_elabel_info(OBJ_HANDLE frudev_handle, GList **custom_info, ELABEL_EXTEND_AREA_S *extend,
    guint32 eeprom_offset)
{
    if (custom_info == NULL || extend == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return RET_ERR;
    }

    
    if (*custom_info != NULL) {
        return RET_OK;
    }

    gint32 ret = frudev_read_from_eeprom(frudev_handle, eeprom_offset, extend, sizeof(ELABEL_EXTEND_AREA_S));
    if (ret != SYN_FRU_OK) {
        debug_log(DLOG_ERROR, "[%s] read extra elabel from eeprom fail.", __FUNCTION__);
        return ret;
    }

    guint16 crc = arith_make_crc_checksum(0, (const guchar *)&extend->len,
        MIN(extend->len + FRU_EXTERN_LABEL_PREFIX_LEN, FRU_EXTERN_LABEL_AREA_LEN - sizeof(guint16)));
    if (crc != extend->crc || extend->len > FRU_EXTEND_DATA_MAX_LEN) {
        return RET_OK;
    }

    
    ELABEL_EXTEND_AREA_S extend_data;
    (void)memcpy_s(&extend_data, sizeof(ELABEL_EXTEND_AREA_S), extend, sizeof(ELABEL_EXTEND_AREA_S));
    split_extend_with_semicolon(&extend_data);
    (void)copy_extend_label(custom_info, &extend_data);

    return RET_OK;
}


LOCAL gboolean __fru_elabel_product_cmp_len(ELABEL_INFO_S *elabel_info, ELABEL_INFO_S *ref_elabel_info)
{
    if (elabel_info->product.manufacture.len != ref_elabel_info->product.manufacture.len) {
        return FALSE;
    }

    if (elabel_info->product.name.len != ref_elabel_info->product.name.len) {
        return FALSE;
    }

    if (elabel_info->product.part_num.len != ref_elabel_info->product.part_num.len) {
        return FALSE;
    }

    if (elabel_info->product.version.len != ref_elabel_info->product.version.len) {
        return FALSE;
    }

    if (elabel_info->product.serial_num.len != ref_elabel_info->product.serial_num.len) {
        return FALSE;
    }

    if (elabel_info->product.asset_tag.len != ref_elabel_info->product.asset_tag.len) {
        return FALSE;
    }

    if (elabel_info->product.file_id.len != ref_elabel_info->product.file_id.len) {
        return FALSE;
    }

    return TRUE;
}


LOCAL gboolean __fru_elabel_product_cmp_data(ELABEL_INFO_S *elabel_info, ELABEL_INFO_S *ref_elabel_info)
{
    if (strcmp((gchar *)elabel_info->product.manufacture.data, (gchar *)ref_elabel_info->product.manufacture.data) !=
        0) {
        return FALSE;
    }

    if (strcmp((gchar *)elabel_info->product.name.data, (gchar *)ref_elabel_info->product.name.data) != 0) {
        return FALSE;
    }

    if (strcmp((gchar *)elabel_info->product.part_num.data, (gchar *)ref_elabel_info->product.part_num.data) != 0) {
        return FALSE;
    }

    if (strcmp((gchar *)elabel_info->product.version.data, (gchar *)ref_elabel_info->product.version.data) != 0) {
        return FALSE;
    }

    if (strcmp((gchar *)elabel_info->product.serial_num.data, (gchar *)ref_elabel_info->product.serial_num.data) != 0) {
        return FALSE;
    }

    if (strcmp((gchar *)elabel_info->product.asset_tag.data, (gchar *)ref_elabel_info->product.asset_tag.data) != 0) {
        return FALSE;
    }

    if (strcmp((gchar *)elabel_info->product.file_id.data, (gchar *)ref_elabel_info->product.file_id.data) != 0) {
        return FALSE;
    }

    return TRUE;
}


LOCAL gboolean __fru_elabel_product_cmp(ELABEL_INFO_S *elabel_info, ELABEL_INFO_S *ref_elabel_info)
{
    if (__fru_elabel_product_cmp_len(elabel_info, ref_elabel_info) == FALSE) {
        return FALSE;
    }

    if (__fru_elabel_product_cmp_data(elabel_info, ref_elabel_info) == FALSE) {
        return FALSE;
    }

    return TRUE;
}


LOCAL void __fru_elabel_product_cpy(ELABEL_INFO_S *elabel_info, ELABEL_INFO_S *ref_elabel_info)
{
    if (elabel_info->product.manufacture.len != 0) {
        (void)memcpy_s(&ref_elabel_info->product.manufacture, sizeof(ref_elabel_info->product.manufacture),
            &elabel_info->product.manufacture, sizeof(elabel_info->product.manufacture));
    }

    if (elabel_info->product.name.len != 0) {
        (void)memcpy_s(&ref_elabel_info->product.name, sizeof(ref_elabel_info->product.name),
            &elabel_info->product.name, sizeof(elabel_info->product.name));
    }

    if (elabel_info->product.part_num.len != 0) {
        (void)memcpy_s(&ref_elabel_info->product.part_num, sizeof(ref_elabel_info->product.part_num),
            &elabel_info->product.part_num, sizeof(elabel_info->product.part_num));
    }

    if (elabel_info->product.version.len != 0) {
        (void)memcpy_s(&ref_elabel_info->product.version, sizeof(ref_elabel_info->product.version),
            &elabel_info->product.version, sizeof(elabel_info->product.version));
    }

    if (elabel_info->product.serial_num.len != 0) {
        (void)memcpy_s(&ref_elabel_info->product.serial_num, sizeof(ref_elabel_info->product.serial_num),
            &elabel_info->product.serial_num, sizeof(elabel_info->product.serial_num));
    }

    if (elabel_info->product.asset_tag.len != 0) {
        (void)memcpy_s(&ref_elabel_info->product.asset_tag, sizeof(ref_elabel_info->product.asset_tag),
            &elabel_info->product.asset_tag, sizeof(elabel_info->product.asset_tag));
    }

    if (elabel_info->product.file_id.len != 0) {
        (void)memcpy_s(&ref_elabel_info->product.file_id, sizeof(ref_elabel_info->product.file_id),
            &elabel_info->product.file_id, sizeof(elabel_info->product.file_id));
    }

    return;
}

LOCAL void __set_ps_part_num(OBJ_HANDLE object_handle, const FRU_INFO_S *fru_info)
{
    // 如果有20字节ESN，跳过开头2字节 “21”，取后面8字节长度部件编码，否则取 fru 中 product_part_num
    gchar part_num[PART_NUMBER_LEN + 1] = {0};
    if (strlen((const gchar *)fru_info->product.product_serial_num.content) == FRU_ESN_DATA_LEN) {
        // 如果有 10 字节PN， 表示供应pod供应2.0电源否则直接取FRU中的product_part_num
        if (strlen((const gchar *)fru_info->product.product_part_num.content) == SUPPLY2_PART_NUMBER_LEN) {
            if (strncpy_s(part_num, sizeof(part_num), (const gchar *)fru_info->product.product_part_num.content,
                SUPPLY2_PART_NUMBER_LEN) != EOK) {
                debug_log(DLOG_ERROR, "%s:strncpy_s failed", __FUNCTION__);
            }
        } else {
            // 跳过 ESN 开头的“21”, 截取后面8字节部件号
            if (strncpy_s(part_num, sizeof(part_num), (const gchar *)fru_info->product.product_serial_num.content + 2,
                PART_NUMBER_ESN_LEN) != EOK) {
                debug_log(DLOG_ERROR, "%s:strncpy_s failed.", __FUNCTION__);
            }
        }
    } else {
        if (strncpy_s(part_num, sizeof(part_num), (const gchar *)fru_info->product.product_part_num.content,
            PART_NUMBER_LEN) != EOK) {
            debug_log(DLOG_ERROR, "%s:strncpy_s failed.", __FUNCTION__);
        }
    }

    set_one_elabel_property(object_handle, PROPERTY_ELABEL_PRODUCT_PN, part_num);

    GSList *handle_list = NULL;
    gint32 ret_val = dfl_get_object_list(CLASS_NAME_PS, &handle_list);
    if (ret_val != DFL_OK) {
        return;
    }

    GSList *input_list = NULL;
    for (GSList *list_item = handle_list; list_item; list_item = g_slist_next(list_item)) {
        OBJ_HANDLE onepower_obj_handle = (OBJ_HANDLE)list_item->data;
        if (dfl_get_position(onepower_obj_handle) != dfl_get_position(object_handle)) {
            continue;
        }

        input_list = g_slist_append(input_list, g_variant_new_string(part_num));
        ret_val = dfl_call_class_method(onepower_obj_handle, METHOD_PS_UPDATE_PART_NUM, NULL, input_list, NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        if (ret_val != DFL_OK) {
            debug_log(DLOG_ERROR, "%s:call METHOD_PS_UPDATE_PART_NUM fail, ret %d.", __FUNCTION__, ret_val);
        }
        break;
    }
    g_slist_free(handle_list);
}


gint32 get_sync_elabel_state(guint8 *state)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret_val = fru_get_chassis_obj_handle(&obj_handle);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "%s dfl_get_object_handle %s", __FUNCTION__, OBJECT_NAME_CHASSIS);
        return ret_val;
    }

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_CHASSIS_SYNC_ELABEL, state);

    return RET_OK;
}


gint32 get_node_index_suffix(gchar *suffix_buf, gint8 buf_len)
{
    if ((suffix_buf == NULL) || (buf_len < ELABEL_SUFFIX_MIN_LEN)) {
        debug_log(DLOG_ERROR, "%s input param fail", __FUNCTION__);
        return RET_ERR;
    }

    OBJ_HANDLE obj_handle = 0;
    gint32 ret_val = fru_get_chassis_obj_handle(&obj_handle);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "%s dfl_get_object_handle %s", __FUNCTION__, OBJECT_NAME_CHASSIS);
        return ret_val;
    }

    guint8 chassis_index = 0;
    dal_get_property_value_byte(obj_handle, PROPERTY_CHASSIS_NODE_INDEX, &chassis_index);

    GVariant *property_data = NULL;
    ret_val = dfl_get_property_value(obj_handle, PROPERTY_CHASSIS_NODE_INDEX_SUFFIX, &property_data);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, "%s dfl_get_property_value %s", __FUNCTION__, PROPERTY_CHASSIS_NODE_INDEX_SUFFIX);
        return ret_val;
    }

    gsize n = 0;
    const gchar **pnode_suffix = g_variant_get_strv(property_data, &n);
    if (pnode_suffix == NULL) {
        debug_log(DLOG_INFO, "%s %s is NULL", __FUNCTION__, PROPERTY_CHASSIS_NODE_INDEX_SUFFIX);
        g_variant_unref(property_data);
        return RET_OK;
    }

    if (n == 0) {
        debug_log(DLOG_INFO, "%s %s is NULL", __FUNCTION__, PROPERTY_CHASSIS_NODE_INDEX_SUFFIX);
        g_free(pnode_suffix);
        g_variant_unref(property_data);
        return RET_OK;
    }

    if (chassis_index <= 0 || chassis_index > n) {
        debug_log(DLOG_ERROR, "%s g_variant_get_strv fail", __FUNCTION__);
        g_free(pnode_suffix);
        g_variant_unref(property_data);
        return RET_ERR;
    }

    errno_t safe_fun_ret = strncpy_s(suffix_buf, buf_len, pnode_suffix[chassis_index - 1], buf_len - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    g_free(pnode_suffix);
    g_variant_unref(property_data);
    return RET_OK;
}


void upgrade_elabel(guint8 fru_id)
{
    FRU_PRIV_PROPERTY_S *fru_priv = NULL;
    OBJ_HANDLE fru_handle = 0;
    gint32 ret = get_fru_priv_data_by_id(fru_id, &fru_priv, &fru_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s get_fru_priv_data_by_id fail!", __FUNCTION__);
        return;
    }

    sync_elabel_to_fru(fru_priv->fru_info, fru_priv->elabel_info);
    (void)vos_thread_sem4_v(fru_priv->update_syn_semid); 
    (void)dal_set_property_value_byte(fru_priv->fru_property.frudev_handle, PROPERTY_FRUDEV_HEALTH, FRU_HEALTH_OK,
        DF_NONE);
    put_obj_priv_data(fru_handle, (gpointer)fru_priv);
}


gint32 handle_extra_elabel_info(FRU_PRIV_PROPERTY_S *fru_priv)
{
    if ((fru_priv == NULL) || (fru_priv->fru_info == NULL) || (fru_priv->fru_property.frudev_handle == 0)) {
        return RET_OK;
    }

    if (fru_priv->eeprom_format == EEPROM_FORMAT_TIANCHI) {
        return RET_OK;
    }

    
    if (fru_priv->fru_property.fru_id != FRU_ID_MAINBOARD) {
        return RET_OK;
    }

    if (fru_priv->fru_property.is_support_config_area == 1) {
        return RET_OK;
    }

    (void)__refresh_extend_elabel_info(fru_priv->fru_property.frudev_handle, &fru_priv->fru_info->chassis.custom_info,
        &fru_priv->elabel_info->chassis_extend, ELABEL_EXTEND_INFO_BLOCK_OFFSET + sizeof(ELABEL_EXTEND_AREA_S));

    gint32 ret =
        __refresh_extend_elabel_info(fru_priv->fru_property.frudev_handle, &fru_priv->fru_info->product.custom_info,
        &fru_priv->elabel_info->product_extend, ELABEL_EXTEND_INFO_BLOCK_OFFSET + (2 * sizeof(ELABEL_EXTEND_AREA_S)));

    return ret;
}


gint32 set_elabel_property(FRU_PROPERTY_S *fru_prop, const FRU_INFO_S *fru_info)
{
    if (fru_prop == NULL || fru_info == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return RET_ERR;
    }

    if (fru_prop->fru_type == PS_MODULE) {
        update_ps_elabel(fru_prop->elabel_handle, fru_info);
        return RET_OK;
    }

    if (fru_prop->rimm_obj_handle != 0) {
        return RET_OK;
    }

    
    set_elabel_chassis_property(fru_prop, fru_info);

    
    set_elabel_board_property(fru_prop, fru_info);

    
    set_elabel_product_property(fru_prop, fru_info);

    return RET_OK;
}

gint32 get_elabel(FRU_PRIV_PROPERTY_S *fru_priv, ELABEL_INFO_S *elabel)
{
    if (fru_priv == NULL || elabel == NULL) {
        debug_log(DLOG_ERROR, "%s fru_priv or elabel is NULL!", __FUNCTION__);
        return RET_ERR;
    }

    if (fru_priv->eeprom_format == EEPROM_FORMAT_TIANCHI) {
        errno_t safe_fun_ret = memcpy_s(elabel, sizeof(ELABEL_INFO_S), fru_priv->elabel_info, sizeof(ELABEL_INFO_S));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
        return RET_OK;
    }

    gint32 ret = frudev_read_from_eeprom(fru_priv->fru_property.frudev_handle, ELABEL_INFO_BLOCK_OFFSET, elabel,
        ELABEL_INFO_AREA_SIZE);
    if (ret != SYN_FRU_OK) {
        debug_log(DLOG_ERROR, "%s:read elabel from eeprom fail,ret=%d.", __FUNCTION__, ret);
        return ret;
    }

    ret = frudev_read_from_eeprom(fru_priv->fru_property.frudev_handle, ELABEL_MULTI_AREA_OFFSET, elabel->multi_record,
        MAX_MULTI_LEN);
    if (ret != SYN_FRU_OK) {
        debug_log(DLOG_ERROR, "[%s]read multi record from eeprom fail, ret=%d.", __FUNCTION__, ret);
        return ret;
    }

    if (fru_priv->fru_property.is_support_config_area == 0 && is_support_extern_lable(fru_priv->fru_property.fru_id)) {
        
        ret = frudev_read_from_eeprom(fru_priv->fru_property.frudev_handle, ELABEL_EXTEND_INFO_BLOCK_OFFSET,
            &elabel->internal_extend, ELABEL_EXTEND_INFO_SIZE);
        if (ret != SYN_FRU_OK) {
            debug_log(DLOG_ERROR, "[%s]read extern elabel from eeprom fail.", __FUNCTION__);
            return ret;
        }
    }

    return RET_OK;
}


void check_elabel_from_eeprom(FRU_PRIV_PROPERTY_S *fru_priv_property, gpointer elabel_tmp)
{
    
    if (!is_open_fru_read_back()) {
        return;
    }

    ELABEL_INFO_S *elabel_e2p = (ELABEL_INFO_S *)g_malloc0(sizeof(ELABEL_INFO_S));
    if (elabel_e2p == NULL) {
        return;
    }

    for (gint32 read_retry_times = 3; read_retry_times > 0; read_retry_times--) {
        (void)memset_s(elabel_e2p, ELABEL_INFO_AREA_SIZE, 0, ELABEL_INFO_AREA_SIZE);
        OBJ_HANDLE object_handle = fru_priv_property->fru_property.frudev_handle;
        gint32 ret =
            frudev_read_from_eeprom(object_handle, ELABEL_INFO_BLOCK_OFFSET, elabel_e2p, ELABEL_INFO_AREA_SIZE);
        if (ret != RET_OK) {
            vos_task_delay(TASK_DELAY_200_MS);
            continue;
        }

        debug_log(DLOG_DEBUG, "%s read elabel from eeprom success!", dfl_get_object_name(object_handle));
        if (memcmp(elabel_e2p, elabel_tmp, ELABEL_INFO_AREA_SIZE) == 0) {
            debug_log(DLOG_DEBUG, "%s memcmp elabel success!", dfl_get_object_name(object_handle));
            vos_task_delay(TASK_DELAY_200_MS);
            continue;
        }

        debug_log(DLOG_ERROR, "%s memcmp elabel fail, copy elabel_e2p to memory!", dfl_get_object_name(object_handle));

        
        ret = memcpy_s(elabel_tmp, ELABEL_INFO_AREA_SIZE, elabel_e2p, ELABEL_INFO_AREA_SIZE);
        if (ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s error, ret = %d", __FUNCTION__, ret);
        }
        
        split_extend_with_semicolon(&elabel_e2p->extend);

        
        ret = memcpy_s(fru_priv_property->elabel_info, ELABEL_INFO_AREA_SIZE, elabel_e2p, ELABEL_INFO_AREA_SIZE);
        if (ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s error, ret = %d", __FUNCTION__, ret);
        }
    }
    g_free(elabel_e2p);
}


gint32 fru_elabel_product_combine(OBJ_HANDLE object_handle, gpointer user_data)
{
    OBJ_HANDLE ref_handle = 0;
    gint32 ret = dfl_get_referenced_object(object_handle, PROPERTY_FRU_ELABEL_PRODUCT_REF, &ref_handle);
    if (ret != DFL_OK) {
        return RET_OK;
    }

    
    FRU_PRIV_PROPERTY_S *fru_priv_property = NULL;
    ret = get_obj_priv_data(object_handle, (gpointer *)&(fru_priv_property));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: call dfl_get_binded_object failed, error = %d", __FUNCTION__, ret);
        return RET_OK;
    }

    FRU_PRIV_PROPERTY_S *ref_fru_priv_property = NULL;
    ret = get_obj_priv_data(ref_handle, (gpointer *)&(ref_fru_priv_property));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: call dfl_get_binded_object failed, error = %d", __FUNCTION__, ret);
        put_obj_priv_data(object_handle, (gpointer)fru_priv_property);
        return RET_OK;
    }

    
    if (__fru_elabel_product_cmp(fru_priv_property->elabel_info, ref_fru_priv_property->elabel_info)) {
        put_obj_priv_data(object_handle, (gpointer)fru_priv_property);
        put_obj_priv_data(ref_handle, (gpointer)ref_fru_priv_property);
        return RET_OK;
    }

    
    __fru_elabel_product_cpy(fru_priv_property->elabel_info, ref_fru_priv_property->elabel_info);
    (void)vos_thread_sem4_v(ref_fru_priv_property->elabel_syn_semid);

    
    sync_elabel_to_fru(ref_fru_priv_property->fru_info, ref_fru_priv_property->elabel_info);

    
    (void)vos_thread_sem4_v(ref_fru_priv_property->update_syn_semid);
    (void)dal_set_property_value_byte(ref_fru_priv_property->fru_property.frudev_handle, PROPERTY_FRUDEV_HEALTH,
        FRU_HEALTH_OK, DF_NONE);
    put_obj_priv_data(object_handle, (gpointer)fru_priv_property);
    put_obj_priv_data(ref_handle, (gpointer)ref_fru_priv_property);

    return RET_OK;
}


LOCAL gint32 last_elabel_handle(FRU_PRIV_PROPERTY_S *fru_priv, guint8 area, guint8 field, guint8 *tmp_elabel_buffer,
    guint32 field_data_size, guint32 field_size, gpointer operator_addr, guint32 *last_len)
{
    gint32 ret;
    guint16 minlen = 0;
    guint8 *multi_addr = NULL;
    if ((area == FRU_AREA_EXTENDELABLE) || ((area == FRU_AREA_INTERNALUSE) && (field == FRU_INTERNAL_EXTRA)) ||
        ((area == FRU_AREA_CHASSISINFO) && (field == FRU_CHASSIS_EXTRA)) ||
        ((area == FRU_AREA_BOARDINFO) && (field == FRU_BOARD_EXTRA)) ||
        ((area == FRU_AREA_PRODUCTINFO) && (field == FRU_PRODUCT_EXTRA))) {
        ELABEL_EXTEND_AREA_S *extend_addr = (ELABEL_EXTEND_AREA_S *)operator_addr;
        ret = update_extend_elabel(extend_addr, tmp_elabel_buffer, *last_len);
        if (ret != RET_OK) {
            ret = COMP_CODE_OUTOF_RANGE;
            debug_log(DLOG_ERROR, "%s: error: area = %d, field = %d", __FUNCTION__, area, field);
            return ret;
        }
    } else if (area == FRU_AREA_MULTIRECORD) {
        multi_addr = (guint8 *)operator_addr;
        ret = __update_multi_elabel(multi_addr, tmp_elabel_buffer, *last_len);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: error: area = %d, field = %d, ret=%x", __FUNCTION__, area, field, ret);
            return COMP_CODE_OUTOF_RANGE;
        }
    } else {
        ELABEL_FIELD_S *field_addr = (ELABEL_FIELD_S *)operator_addr;
        (void)memset_s(field_addr, field_size, 0, field_size);
        minlen = (*last_len > field_data_size) ? field_data_size : *last_len;

        
        ret = e2p_cafe_update_to_e2p(area, field, fru_priv->cafe_info, tmp_elabel_buffer, minlen);
        if (ret != COMP_CODE_SUCCESS) {
            return ret;
        }

        ret = memcpy_s(field_addr->data, field_data_size, tmp_elabel_buffer, minlen);
        if (ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s error, ret = %d", __FUNCTION__, ret);
            return COMP_CODE_UNKNOWN;
        }
        field_addr->len = minlen;
        field_addr->crc =
            arith_make_crc_checksum(0, (const guchar *)&(field_addr->len), field_addr->len + sizeof(guint16));
    }
    ret = update_chassis_fru_in_raw_eeprom(area, field, fru_priv, tmp_elabel_buffer, minlen);

    return ret;
}


LOCAL gint32 __single_multi_record_crc_check(guint8 *input)
{
    ELABEL_MULTIRECORD_SINGLE_S *single_record = (ELABEL_MULTIRECORD_SINGLE_S *)input;
    if (single_record->rec_crc !=
        vos_calc_check_sum((const guchar *)input + SINGLE_MULTI_HEAD_LEN, single_record->len)) {
        debug_log(DLOG_ERROR, "[%s] multi record area record crc failed, expect content crc:%02x, actual crc:%02x.",
            __FUNCTION__, vos_calc_check_sum((const guchar *)input + SINGLE_MULTI_HEAD_LEN, single_record->len),
            single_record->rec_crc);
        return RMCP_REQ_LENGTH_INVALID;
    }

    if (single_record->head_crc != vos_calc_check_sum((const guchar *)input, SINGLE_MULTI_HEAD_LEN - 1)) {
        debug_log(DLOG_ERROR, "[%s] multi record area head crc failed, expect head crc:%02x, actual crc:%02x.",
            __FUNCTION__, vos_calc_check_sum((const guchar *)input, SINGLE_MULTI_HEAD_LEN - 1),
            single_record->head_crc);
        return RMCP_REQ_LENGTH_INVALID;
    }
    return COMP_CODE_SUCCESS;
}


LOCAL gint32 __multi_elabel_crc_check(guint8 *input, guint16 length)
{
    ELABEL_MULTIRECORD_SINGLE_S *single_record = (ELABEL_MULTIRECORD_SINGLE_S *)input;
    guint16 len = 0;
    gint32 ret;
    while (!(single_record->end_flag & MASK_BIT7) && len < length) {
        ret = __single_multi_record_crc_check((guint8 *)single_record);
        if (ret != COMP_CODE_SUCCESS) {
            return ret;
        }
        len += SINGLE_MULTI_HEAD_LEN + single_record->len;
        single_record = (ELABEL_MULTIRECORD_SINGLE_S *)(input + len);
    }

    if (single_record->end_flag & MASK_BIT7) {
        ret = __single_multi_record_crc_check((guint8 *)single_record);
        if (ret != COMP_CODE_SUCCESS) {
            return ret;
        }
        return COMP_CODE_SUCCESS;
    }
    debug_log(DLOG_ERROR, "[%s] endless multi record area msg, len=%d.", __FUNCTION__, len);
    return RMCP_REQ_LENGTH_INVALID;
}


LOCAL gint32 __update_multi_elabel(guint8 *multi_buf, guint8 *input, guint16 length)
{
    if (!multi_buf || !input) {
        debug_log(DLOG_ERROR, "[%s] Invalid parameter!", __FUNCTION__);
        return COMP_CODE_UNKNOWN;
    }
    gint32 ret;
    
    if (length + 1 > MAX_MULTI_LEN) {
        debug_log(DLOG_ERROR, "[%s]: multi buf is too long, len = %zu.", __FUNCTION__, strlen((const char *)input) + 1);
        return COMP_CODE_OUTOF_RANGE;
    }

    ret = __multi_elabel_crc_check(input, length);
    if (ret != COMP_CODE_SUCCESS) {
        return ret;
    }

    ret = memcpy_s(multi_buf, MAX_MULTI_LEN, (const guint8 *)input, length);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] copy data failed, ret=%d!", __FUNCTION__, ret);
        return COMP_CODE_UNKNOWN;
    }

    for (gint32 i = 0; i < length; i++) {
        debug_log(DLOG_DEBUG, "%s: multi_buf[%d]=0x%02x.", __FUNCTION__, i, *(multi_buf + i));
    }

    return COMP_CODE_SUCCESS;
}


LOCAL gboolean is_set_serial_number_contain_space(const DFT_WRITE_ELABEL_REQ_S *dft_write_elabel_req)
{
    gboolean set_serial = FALSE;
    ELABEL_FIELD_AREA_COMB elabel_filed_area_comb[] = {
        {FRU_CHASSIS_SERIAL_NUMBER, FRU_AREA_CHASSISINFO}, {FRU_BOARD_SERIALNUMBER, FRU_AREA_BOARDINFO},
        {FRU_PRODUCT_SERIALNUMBER, FRU_AREA_PRODUCTINFO}, {FRU_SYSTEM_SERIALNUMBER, FRU_AREA_SYSTEMINFO}
    };
 
    for (gsize i = 0; i < G_N_ELEMENTS(elabel_filed_area_comb); i++) {
        if (elabel_filed_area_comb[i].field_number == dft_write_elabel_req->field &&
            elabel_filed_area_comb[i].area_number == dft_write_elabel_req->area) {
            set_serial = TRUE;
            break;
        }
    }
    if (set_serial == FALSE) {
        return FALSE;
    }
 
    for (gsize i = 0; i < dft_write_elabel_req->len; i++) {
        if (dft_write_elabel_req->data[i] == ' ') {
            debug_log(DLOG_ERROR, "[%s] write elabel serial number contain space", __FUNCTION__);
            return TRUE;
        }
    }
    return FALSE;
}
 

LOCAL gint32 elabel_info_parameter_invalid(FRU_PRIV_PROPERTY_S *fru_priv,
    const DFT_WRITE_ELABEL_REQ_S *write_elabel_req)
{
    if (fru_priv == NULL || write_elabel_req == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return COMP_CODE_UNKNOWN;
    }
 
    if ((write_elabel_req->area > FRU_AREA_SYSTEMINFO && write_elabel_req->area != FRU_AREA_MULTIRECORD) ||
        write_elabel_req->field >= MAX_FIELD_NUM) {
        debug_log(DLOG_ERROR, "%s:dft write ipmi cmd parametter error:area = %u,field = %u", __FUNCTION__,
            write_elabel_req->area, write_elabel_req->field);
        return COMP_CODE_OUTOF_RANGE;
    }
    return RET_OK;
}

LOCAL void syn_system_area(FRU_PRIV_PROPERTY_S *fru_priv, guint8 field, guint32 last_len, guint8 *tmp_elabel_buffer)
{
    guint32 min_len;
    errno_t safe_fun_ret;

    if (fru_priv->eeprom_format == EEPROM_FORMAT_TIANCHI) {
        guint32 max_len = g_system_area_table[field].max_len;
        guint8 *start = (guint8 *)&fru_priv->system_info->tc_system_desc_st + g_system_area_table[field].offset;
        (void)memset_s(start, max_len, 0, max_len);
        min_len = (last_len > max_len) ? max_len : last_len;
        safe_fun_ret = memcpy_s(start, max_len, tmp_elabel_buffer, min_len);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    } else {
        DATA_LENGTH_S *data_length_p = (DATA_LENGTH_S *)(&fru_priv->system_info->system_area_array[field]);
        (void)memset_s(data_length_p->data, FRU_MAX_ELABEL_LEN, 0, FRU_MAX_ELABEL_LEN);
        min_len = (last_len > FRU_MAX_ELABEL_LEN) ? FRU_MAX_ELABEL_LEN : last_len;
        safe_fun_ret = memcpy_s(data_length_p->data, sizeof(data_length_p->data), tmp_elabel_buffer, min_len);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        data_length_p->len = (guint8)min_len;
    }

    (void)vos_thread_sem4_v(fru_priv->update_sys_syn_semid);
}


gint32 update_elabel_info(FRU_PRIV_PROPERTY_S *fru_priv, const DFT_WRITE_ELABEL_REQ_S *dft_write_elabel_req,
    guint8 req_len)
{
    gint32 ret = elabel_info_parameter_invalid(fru_priv, dft_write_elabel_req);
    if (ret != RET_OK) {
        return ret;
    }

    guint8 field = dft_write_elabel_req->field;
    guint8 area = dft_write_elabel_req->area;

    g_mutex_lock(&g_update_elabel_mutex);

    static guint8 *elabel_buffer[MAX_AREA_NUM][MAX_FIELD_NUM] = {{0}, {0}};
    static guint32 elabel_buffer_len[MAX_AREA_NUM][MAX_FIELD_NUM] = {{0}, {0}};
    guint8 *tmp_elabel_buffer = elabel_buffer[area][field];
    guint32 *last_len = &elabel_buffer_len[area][field];
    
    gpointer operator_addr = NULL;
    guint32 field_size = 0;
    guint32 field_data_size = 0;
    guint32 max_length = 0;
    ret = lookup_operator_field_addr_size(fru_priv, area, field, &operator_addr, &field_size, &field_data_size,
        &max_length);
    if (ret != RET_OK || field_data_size == 0) {
        g_mutex_unlock(&g_update_elabel_mutex);
        debug_log(DLOG_ERROR, "%s: parametter error: area = %hhu, field = %hhu, field_data_size = %u",
            __FUNCTION__, area, field, field_data_size);
        return COMP_CODE_OUTOF_RANGE;
    }

    
    guint8 cur_len = dft_write_elabel_req->len;
    guint8 offset = dft_write_elabel_req->flag_offset & 0x7f;
    ret = check_elabel_length(field_data_size, offset, cur_len, *last_len, req_len, max_length);
    if (ret != RET_OK) {
        g_mutex_unlock(&g_update_elabel_mutex);
        debug_log(DLOG_ERROR, "%s: parametter error: area = %hhu, offset = %hhu, cur_len = %hhu,  last_len = %u.",
            __FUNCTION__, area, offset, cur_len, *last_len);
        return COMP_CODE_OUTOF_RANGE;
    }

    
    if (is_set_serial_number_contain_space(dft_write_elabel_req)) {
        g_mutex_unlock(&g_update_elabel_mutex);
        return COMP_CODE_INVALID_FIELD;
    }

    
    if (tmp_elabel_buffer == NULL) {
        debug_log(DLOG_INFO, "tmp_elabel_buffer is null");
        elabel_buffer[area][field] = (guint8 *)g_malloc0(field_data_size);
        if (elabel_buffer[area][field] == NULL) {
            g_mutex_unlock(&g_update_elabel_mutex);
            debug_log(DLOG_ERROR, "Failed to malloc buffer");
            return COMP_CODE_CANNOT_RESPONSE;
        }

        tmp_elabel_buffer = elabel_buffer[area][field];
        *last_len = 0;
    }

    
    if ((dft_write_elabel_req->flag_offset & ~0x80) == 0) {
        (void)memset_s(tmp_elabel_buffer, field_data_size, 0, field_data_size);
        *last_len = 0;
    }

    
    errno_t safe_fun_ret =
        memcpy_s(tmp_elabel_buffer + offset, field_data_size - offset, dft_write_elabel_req->data, cur_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    *last_len = MAX(offset + cur_len, *last_len);

    
    if ((dft_write_elabel_req->flag_offset & 0x80) != 0) {
        g_mutex_unlock(&g_update_elabel_mutex);
        return COMP_CODE_SUCCESS;
    }

    do {
        
        ret = last_elabel_handle(fru_priv, area, field, tmp_elabel_buffer, field_data_size, field_size, operator_addr,
            last_len);
        if (ret != RET_OK) {
            break;
        }
        if ((area == FRU_AREA_SYSTEMINFO) && (field < SYS_AREA_ELABEL_NUM)) {
            syn_system_area(fru_priv, field, *last_len, tmp_elabel_buffer);
        }

        check_and_fill_default_manufacturer(fru_priv->elabel_info);

        if (fru_priv->fru_property.frudev_type == STORAGE_TYPE_MCU) {
            fru_priv->fru_property.dft_operate_type = MCU_WRITE_ELABEL;
        }

        
        open_fru_read_back();

        
        (void)vos_thread_sem4_v(fru_priv->elabel_syn_semid);
    } while (0);

    *last_len = 0;
    g_free(tmp_elabel_buffer);
    elabel_buffer[area][field] = NULL;
    g_mutex_unlock(&g_update_elabel_mutex);
    return ret;
}

LOCAL void __set_elabel_power_supply(OBJ_HANDLE object_handle, FRU_MULTI_AREA_S *multi_area)
{
    PS_PROPERTY_TYPE_INFO_S *ps_type0 = (PS_PROPERTY_TYPE_INFO_S *)multi_area->data;
    guint16 ratedpower = (guint16)(ps_type0->overallcapacity);
    (void)dal_set_property_value_uint16(object_handle, PROPERTY_ELABEL_PS_RATE, ratedpower, DF_NONE);

    // 类OnePower 的属性AcDc中: 0:DC  1:AC  2:AC/DC */
    // 电源中: 0:无输入或者输入电压异常1:交流输入 90~290Vac  2:高压直流 240Vdc/380Vdc  3:低压直流 48Vdc
    guint8 ps_type = (guint8)(ps_type0->input_type);
    guint8 ps_mode = PS_INPUT_TYPE_NONE;
    if (ps_type == PS_POWER_TYPE_DC) {
        ps_mode = PS_INPUT_TYPE_AC;
    } else if (ps_type == PS_POWER_TYPE_HVDC || ps_type == PS_POWER_TYPE_AC_HVDC) {
        ps_mode = PS_INPUT_TYPE_DC;
    }

    (void)dal_set_property_value_byte(object_handle, PROPERTY_ELABEL_PS_INPUT_MODE, ps_mode, DF_NONE);
}

LOCAL void __set_elabel_power_supply_oem(OBJ_HANDLE object_handle, FRU_MULTI_AREA_S *multi_area)
{
    PS_OEM_RECORD_S *ps_oem_type = (PS_OEM_RECORD_S *)(void *)multi_area->data;
    guint8 ps_mode = ps_oem_type->ps_indentifier_un.ps_indentifier_st.input_mode;

    // EEPROM中输入模式00代表DC,10代表AC,11代表AC或者240DC
    // 类OnePower 的属性AcDc中: 0:DC  1:AC  2:AC/DC */
    if (ps_mode == PS_POWER_TYPE_DC) {
        ps_mode = PS_INPUT_TYPE_NONE;
    } else if (ps_mode == PS_POWER_TYPE_HVDC || ps_mode == PS_POWER_TYPE_AC_HVDC) {
        ps_mode--;
    }

    (void)dal_set_property_value_byte(object_handle, PROPERTY_ELABEL_PS_INPUT_MODE, ps_mode, DF_NONE);
}


gint32 update_ps_elabel(OBJ_HANDLE object_handle, const FRU_INFO_S *fru_info)
{
    if (fru_info == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return RET_ERR;
    }

    set_one_elabel_property(object_handle, PROPERTY_ELABEL_PRODUCT_MFG_NAME,
        (const gchar *)fru_info->product.product_manufacturer.content);
    set_one_elabel_property(object_handle, PROPERTY_ELABEL_PRODUCT_NAME,
        (const gchar *)fru_info->product.product_name.content);
    set_one_elabel_property(object_handle, PROPERTY_ELABEL_PRODUCT_SN,
        (const gchar *)fru_info->product.product_serial_num.content);
    set_one_elabel_property(object_handle, PROPERTY_ELABEL_PRODUCT_VERSION,
        (const gchar *)fru_info->product.product_version.content);

    __set_ps_part_num(object_handle, fru_info);

    GList *multi = fru_info->multi_record;
    while (multi) {
        GList *next = multi->next;
        FRU_MULTI_AREA_S *multi_area = (FRU_MULTI_AREA_S *)multi->data;
        if (multi_area->header.type == POWER_SUPPLY_TYPE) {
            __set_elabel_power_supply(object_handle, multi_area);
        }

        if (multi_area->header.type == POWER_SUPPLY_OEM_TYPE) {
            __set_elabel_power_supply_oem(object_handle, multi_area);
        }

        multi = next;
    }

    return RET_OK;
}


LOCAL gint32 __elabel_update_system_area(OBJ_HANDLE object_handle, const FRU_PRIV_PROPERTY_S *fru_priv_property,
    SYSTEM_AREA_S *system_area)
{
    if (system_area == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return RET_ERR;
    }
    gint32 ret;

    
    system_area->crc = arith_make_crc_checksum(0, (guchar *)system_area,
        SYS_AREA_ELABEL_NUM * sizeof(DATA_LENGTH_S) + sizeof(gboolean));

    if (fru_priv_property->fru_property.frudev_type == STORAGE_TYPE_FILE) {
        ret = frudev_write_file_withoffset(object_handle, fru_priv_property->area_offsets->system_desc_offset,
            system_area, sizeof(SYSTEM_AREA_S));
    } else {
        ret = frudev_write_to_eeprom(object_handle, fru_priv_property->area_offsets->system_desc_offset,
            system_area, sizeof(SYSTEM_AREA_S));
    }

    if (fru_priv_property->fru_property.frudev_type == STORAGE_TYPE_E2P_DOUBLE) {
        ret += frudev_write_to_back_eeprom(object_handle, E2P_SYSTEM_AREA_OFF, system_area, sizeof(SYSTEM_AREA_S));
    }

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s write data to eeprom failed, ret=%d.", dfl_get_object_name(object_handle), ret);
    }

    set_one_elabel_property(fru_priv_property->fru_property.elabel_handle, PROPERTY_ELABEL_SYS_SERIAL_NUM,
        (const gchar *)fru_priv_property->system_info->system_area_st.sys_sn.data);
    set_one_elabel_property(fru_priv_property->fru_property.elabel_handle, PROPERTY_ELABEL_SYS_NAME,
        (const gchar *)fru_priv_property->system_info->system_area_st.sys_product_name.data);
    return ret;
}


LOCAL gint32 tc_elabel_update_system_area(OBJ_HANDLE object_handle, const FRU_PRIV_PROPERTY_S *fru_priv_property,
    TC_SYSTEM_DESC_S *system_desc)
{
    if (system_desc == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return RET_ERR;
    }

    
    system_desc->crc = arith_make_crc32(0, (const char *)system_desc, sizeof(TC_SYSTEM_DESC_S) - sizeof(guint32));

    gint32 ret = frudev_write_to_eeprom(object_handle, fru_priv_property->area_offsets->system_desc_offset, system_desc,
        sizeof(TC_SYSTEM_DESC_S));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "call frudev_write_to_eeprom failed, error = %d.", ret);
    }

    set_one_elabel_property(fru_priv_property->fru_property.elabel_handle, PROPERTY_ELABEL_SYS_SERIAL_NUM,
        (const gchar *)fru_priv_property->system_info->tc_system_desc_st.sys_sn);
    set_one_elabel_property(fru_priv_property->fru_property.elabel_handle, PROPERTY_ELABEL_SYS_NAME,
        (const gchar *)fru_priv_property->system_info->tc_system_desc_st.sys_product_name);
    return ret;
}


gint32 update_system_area(FRU_PRIV_PROPERTY_S *fru_priv)
{
    if (fru_priv == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return RET_ERR;
    }

    if (fru_priv->eeprom_format == EEPROM_FORMAT_TIANCHI) {
        return tc_elabel_update_system_area(fru_priv->fru_property.frudev_handle, fru_priv,
            &(fru_priv->system_info->tc_system_desc_st));
    }

    return __elabel_update_system_area(fru_priv->fru_property.frudev_handle, fru_priv,
        &(fru_priv->system_info->system_area_st));
}


void elabel_internal_area_paser(FRU_INTERNAL_AREA_S *internal_area, const guint8 *databuf,
    const FRU_INFO_HEADER_S *header, gsize buf_size)
{
    if (internal_area == NULL || databuf == NULL || header == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return;
    }

    if (buf_size < sizeof(guint16)) {
        debug_log(DLOG_ERROR, "%s: read out of bounds, 2>%" G_GSIZE_FORMAT, __FUNCTION__, buf_size);
        return;
    }

    gulong len = *databuf + (*(databuf + 1) << OFFSET_8_BIT);
    if (len + sizeof(guint16) > buf_size) {
        debug_log(DLOG_ERROR, "%s: read out of bounds, %lu>%" G_GSIZE_FORMAT, __FUNCTION__, len + sizeof(guint16),
            buf_size);
        return;
    }

    // 出现了frudata挂掉的情况, 这里move需要限定长度
    internal_area->data = (guint8 *)g_malloc0(FRU_INTERNAL_AREA_LEN);
    if (internal_area->data == NULL) {
        return;
    }

    errno_t safe_fun_ret = memmove_s(internal_area->data, FRU_INTERNAL_AREA_LEN, databuf + sizeof(guint16), len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    internal_area->len = len;

    return;
}


void elabel_chassis_area_paser(FRU_CHASSIS_AREA_S *chassis_area, const guint8 *databuf, gsize buf_size)
{
#define FRU_DATA_CHASSIS_MIN_PARSE_LEN 6
    if (chassis_area == NULL || databuf == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return;
    }

    if (buf_size < FRU_DATA_CHASSIS_MIN_PARSE_LEN) {
        debug_log(DLOG_ERROR, "%s The buf_size is too short, buf_size = %" G_GSIZE_FORMAT, __FUNCTION__, buf_size);
        return;
    }

    guint16 chassis_offset = 0;
    chassis_area->area_ver = databuf[chassis_offset++];
    chassis_area->area_len = databuf[chassis_offset++] * MULITPLE_PARA;
    chassis_area->chassis_type = databuf[chassis_offset++];

    set_label_text(&(chassis_area->chassis_part_num), databuf, &chassis_offset, buf_size);
    set_label_text(&(chassis_area->chassis_serial_num), databuf, &chassis_offset, buf_size);

    if (chassis_offset >= buf_size) {
        debug_log(DLOG_ERROR, "%s: read out of bounds, %u>=%" G_GSIZE_FORMAT, __FUNCTION__, chassis_offset, buf_size);
        return;
    }

    if (G_LIKELY(databuf[chassis_offset] == NO_MORE_INFO)) {
        chassis_area->custom_info = NULL;
        return;
    }

    while ((chassis_offset < chassis_area->area_len) && (databuf[chassis_offset] != NO_MORE_INFO)) {
        if (elabel_area_parse(databuf, &chassis_offset, &(chassis_area->custom_info), buf_size) != RET_OK) {
            return;
        }
    }

    return;
}


LOCAL void clear_fru_extend_label_data(FRU_BOARD_AREA_S *board_area)
{
    if (g_list_length(board_area->extension_label) == 0) {
        return;
    }
    g_list_free_full(g_steal_pointer(&(board_area->extension_label)), g_free);
    board_area->extension_label = NULL;
}


void elabel_board_area_paser(FRU_BOARD_AREA_S *board_area, const guint8 *databuf, gsize buf_size)
{
#define FRU_DATA_BOARD_MIN_PARSE_LEN 12
    if (board_area == NULL || databuf == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return;
    }

    if (buf_size < FRU_DATA_BOARD_MIN_PARSE_LEN) {
        debug_log(DLOG_ERROR, "%s The buf_size is too short, buf_size = %" G_GSIZE_FORMAT, __FUNCTION__, buf_size);
        return;
    }

    guint16 offset = 0;
    board_area->area_ver = databuf[offset++];
    board_area->area_len = databuf[offset++] * MULITPLE_PARA;
    board_area->lang = databuf[offset++];
    OBJ_HANDLE obj_handle = 0;
    guint8 elabel_format = 0;
    (void)dfl_get_object_handle(OBJECT_BMC, &obj_handle);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_BMC_ELABELMANUFORMAT, &elabel_format);
    guint8 mfg_format_len = 3;  // 旧格式长度为3
    if (elabel_format == 1) {
        mfg_format_len = 4;     // 新格式长度为4
    }
    errno_t safe_fun_ret = memcpy_s(board_area->mfg_date_time, mfg_format_len, databuf + offset,
        mfg_format_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    offset += (guint16)mfg_format_len;

    set_label_text(&(board_area->board_manufacturer), databuf, &offset, buf_size);
    set_label_text(&(board_area->board_product_name), databuf, &offset, buf_size);
    set_label_text(&(board_area->board_serial_num), databuf, &offset, buf_size);
    set_label_text(&(board_area->board_part_num), databuf, &offset, buf_size);
    set_label_text(&(board_area->file_id), databuf, &offset, buf_size);

    if (offset >= buf_size) {
        debug_log(DLOG_ERROR, "%s: read out of bounds, %u>=%" G_GSIZE_FORMAT, __FUNCTION__, offset, buf_size);
        return;
    }

    if (G_LIKELY(databuf[offset] == NO_MORE_INFO)) {
        board_area->extension_label = NULL;
        return;
    }
    clear_fru_extend_label_data(board_area);
    while ((offset < board_area->area_len) && (databuf[offset] != NO_MORE_INFO)) {
        if (elabel_area_parse(databuf, &offset, &(board_area->extension_label), buf_size) != RET_OK) {
            return;
        }
    }

    return;
}


void elabel_product_area_paser(FRU_PRODUCT_AREA_S *product_area, const guint8 *databuf, gsize buf_size)
{
#define FRU_DATA_PRODUCT_MIN_PARSE_LEN 11
    if (product_area == NULL || databuf == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return;
    }

    if (buf_size < FRU_DATA_PRODUCT_MIN_PARSE_LEN) {
        debug_log(DLOG_ERROR, "%s The buf_size is too short, buf_size = %" G_GSIZE_FORMAT, __FUNCTION__, buf_size);
        return;
    }

    guint16 pro_offset = 0;
    product_area->area_ver = databuf[pro_offset++];
    product_area->area_len = databuf[pro_offset++] * MULITPLE_PARA;
    product_area->lang = databuf[pro_offset++];

    
    set_label_text(&(product_area->product_manufacturer), databuf, &pro_offset, buf_size);
    set_label_text(&(product_area->product_name), databuf, &pro_offset, buf_size);
    set_label_text(&(product_area->product_part_num), databuf, &pro_offset, buf_size);
    set_label_text(&(product_area->product_version), databuf, &pro_offset, buf_size);
    set_label_text(&(product_area->product_serial_num), databuf, &pro_offset, buf_size);
    set_label_text(&(product_area->product_asset_tag), databuf, &pro_offset, buf_size);
    set_label_text(&(product_area->file_id), databuf, &pro_offset, buf_size);

    if (pro_offset >= buf_size) {
        debug_log(DLOG_ERROR, "%s: read out of bounds, %u>=%" G_GSIZE_FORMAT, __FUNCTION__, pro_offset, buf_size);
        return;
    }

    if (G_LIKELY(databuf[pro_offset] == NO_MORE_INFO)) {
        product_area->custom_info = NULL;
        return;
    }

    while ((pro_offset < product_area->area_len) && (databuf[pro_offset] != NO_MORE_INFO)) {
        if (elabel_area_parse(databuf, &pro_offset, &(product_area->custom_info), buf_size) != RET_OK) {
            return;
        }
    }

    return;
}


gint32 update_elabel_product_assettag(GSList *caller_info, guint8 fruid, const gchar *assettag)
{
    OBJ_HANDLE fru_handle = 0;
    gint32 ret = dfl_get_binded_object(CLASS_FRU, fruid, &fru_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: error : %d", __FUNCTION__, ret);
        return ret;
    }

    FRU_PRIV_PROPERTY_S *fru_priv = NULL;
    ret = get_obj_priv_data(fru_handle, (gpointer *)(&fru_priv));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: error : %d", __FUNCTION__, ret);
        return ret;
    }

    gpointer operator_addr = NULL;
    guint32 field_size = 0;
    guint32 field_data_size = 0;
    guint32 max_length = 0;
    ret = lookup_operator_field_addr_size(fru_priv, FRU_AREA_PRODUCTINFO, FRU_PRODUCT_ASSETTAG, &operator_addr,
        &field_size, &field_data_size, &max_length);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: error : %d", __FUNCTION__, ret);
        put_obj_priv_data(fru_handle, (gpointer)fru_priv);
        return ret;
    }

    
    guint32 assettag_length = (guint32)strlen(assettag);
    if (assettag_length > FRU_MAX_ELABEL_LEN) {
        debug_log(DLOG_ERROR, "Assettag(%s) length(%d) is invalid", assettag, assettag_length);
        method_operation_log(caller_info, NULL, "Write fru%d E-label data failed\n", fruid);
        put_obj_priv_data(fru_handle, (gpointer)fru_priv);
        return RET_ERR;
    }

    
    ELABEL_FIELD_S *field_addr = (ELABEL_FIELD_S *)operator_addr;
    (void)memset_s(field_addr, field_size, 0, field_size);
    errno_t safe_fun_ret = memcpy_s(field_addr->data, field_data_size, assettag, assettag_length);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    field_addr->len = assettag_length;
    field_addr->crc = arith_make_crc_checksum(0, (const guchar *)&(field_addr->len), field_addr->len + sizeof(guint16));

    
    (void)vos_thread_sem4_v(fru_priv->elabel_syn_semid);
    method_operation_log(caller_info, NULL, "Write fru%d E-label data(%s) successfully", fruid, assettag);

    
    sync_elabel_to_fru(fru_priv->fru_info, fru_priv->elabel_info);

    
    (void)vos_thread_sem4_v(fru_priv->update_syn_semid);
    method_operation_log(caller_info, NULL, "Update fru%d E-label data successfully", fruid);

    (void)e2p_cafe_update_to_e2p(FRU_AREA_PRODUCTINFO, FRU_PRODUCT_ASSETTAG, fru_priv->cafe_info, assettag,
        assettag_length);

    (void)dal_set_property_value_byte(fru_priv->fru_property.frudev_handle, PROPERTY_FRUDEV_HEALTH, 0, DF_NONE);
    put_obj_priv_data(fru_handle, (gpointer)fru_priv);
    return RET_OK;
}


void update_file_id(FRU_PRIV_PROPERTY_S *fru_priv)
{
    if (fru_priv == NULL) {
        debug_log(DLOG_ERROR, "%s Invalid parameter!", __FUNCTION__);
        return;
    }

    guint8 len = (guint8)strlen(fru_priv->fru_property.fru_ver);

    
    (void)strncpy_s((gchar *)fru_priv->fru_info->board.file_id.content, FRU_MAX_ELABEL_LEN,
        (gchar *)fru_priv->fru_property.fru_ver, MAX_BMC_VER_LEN);
    fru_priv->fru_info->board.file_id.len = len;
    fru_priv->fru_info->board.file_id.type = CHAR_TYPE;

    
    (void)strncpy_s((gchar *)fru_priv->fru_info->product.file_id.content, FRU_MAX_ELABEL_LEN,
        (gchar *)fru_priv->fru_property.fru_ver, MAX_BMC_VER_LEN);
    fru_priv->fru_info->product.file_id.len = len;
    fru_priv->fru_info->product.file_id.type = CHAR_TYPE;

    
    sync_elabel_from_fru(&(fru_priv->elabel_info->board.file_id), fru_priv->fru_info->board.file_id);

    
    sync_elabel_from_fru(&(fru_priv->elabel_info->product.file_id), fru_priv->fru_info->product.file_id);
    (void)vos_thread_sem4_v(fru_priv->elabel_syn_semid);

    
    fru_priv->upgrate_state = TRUE;

    
    (void)vos_thread_sem4_v(fru_priv->update_syn_semid);

    

    return;
}


void split_extend_with_null(ELABEL_EXTEND_AREA_S *elabel_extend)
{
    if (elabel_extend->num <= 0) {
        return;
    }

    elabel_extend->len = elabel_extend->len + 1; // V1 的长度包含最后一个为0的字节
    if (elabel_extend->len > FRU_EXTEND_DATA_MAX_LEN) {
        debug_log(DLOG_ERROR, "[%s] invlaid extend elabel length = %u", __FUNCTION__, elabel_extend->len);
        return;
    }

    for (guint32 i = 0; i < elabel_extend->len; i++) {
        if (elabel_extend->data[i] == ';') {
            elabel_extend->data[i] = '\0';
        }
    }

    elabel_extend->crc = arith_make_crc_checksum(0, (const guchar *)&elabel_extend->len,
        MIN(elabel_extend->len + FRU_EXTERN_LABEL_PREFIX_LEN,
        sizeof(elabel_extend->data) + FRU_EXTERN_LABEL_PREFIX_LEN));
}


void write_extend_elabel(OBJ_HANDLE object_handle, guint32 offset, ELABEL_EXTEND_AREA_S *elabel_extend, guint32 length)
{
    if (elabel_extend == NULL) {
        debug_log(DLOG_ERROR, "%s Parameter is NULL.", __FUNCTION__);
        return;
    }

    guint8 retry_times = 0;
    guint32 extend_offset = ELABEL_EXTEND_INFO_BLOCK_OFFSET + offset;
    while (frudev_write_to_eeprom(object_handle, extend_offset, elabel_extend, length) != RET_OK) {
        debug_log(DLOG_INFO, "%s update internal/chassis/product extend elabel info fail, offset=%d.", __FUNCTION__,
            offset);

        if (retry_times > 10) { // 10:
            break;
        }

        vos_task_delay(TASK_DELAY_ONE_SECOND);
        retry_times++;
    }

    if (retry_times <= 10) { // 10:
        elabel_extend->valid_flag = 0;
    }
}


LOCAL gint32 init_ipmi_reqdata_for_chassis_sn(IPMI_REQ_MSG_HEAD_S *p_req_head, guint8 *req_data, guint32 req_data_len)
{
    if ((p_req_head == NULL) || (req_data_len != IPMI_CHASSIS_INFO_REQ_DATA_LEN)) {
        return RET_ERR;
    }

    if (dal_check_if_vsmm_supported() && dal_get_vsmm_ms_status() == MSM_SECONDARY_STATE) {
        p_req_head->target_type = IPMI_BMC;
        p_req_head->target_instance = dal_get_master_vsmm_slot_id();
    } else {
        p_req_head->target_type = IPMI_SMM;
        p_req_head->target_instance = 0;
    }
    p_req_head->src_len = req_data_len;
    p_req_head->netfn = NETFN_OEM_REQ;
    p_req_head->lun = 0;
    p_req_head->cmd = 0x94;    

    req_data[0x0] = 0xdb;    
    req_data[0x1] = 0x07;    
    req_data[0x2] = 0x00;    
    req_data[0x3] = 0x30;    
    req_data[0x5] = 0x01;    
    req_data[0x6] = 0x00;    
    req_data[0x7] = 0x30;    

    return RET_OK;
}


LOCAL gint32 copy_chassis_sn_resp_data(gchar *buf, guint32 buf_size,
    const IPMIMSG_GET_SHELF_DATA_RESP_S *tmp_resp, guint32 data_len)
{
    errno_t safe_fun_ret;

    
    if (data_len < 5) {
        debug_log(DLOG_DEBUG, "%s: resp data len error", __FUNCTION__);
        return RET_ERR;
    }
    
    if ((data_len - 5) < buf_size) {
        safe_fun_ret = memcpy_s(buf, buf_size, tmp_resp->content, data_len - 5); // 前5个字节不包含条码信息
    } else {
        safe_fun_ret = memcpy_s(buf, buf_size, tmp_resp->content, buf_size);
    }
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_DEBUG, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 get_chassis_sn_from_smm(gchar *buf, guint32 buf_size)
{
    guint8 req_data[IPMI_CHASSIS_INFO_REQ_DATA_LEN] = {0};
    guint8 i;
    IPMI_REQ_MSG_HEAD_S req_msg_head;
    const IPMIMSG_GET_SHELF_DATA_RESP_S *tmp_resp = NULL;
    gpointer resp_ipmi = NULL;
    guint32 data_len;
    gint32 ret;

    (void)memset_s(&req_msg_head, sizeof(IPMI_REQ_MSG_HEAD_S), 0, sizeof(IPMI_REQ_MSG_HEAD_S));
    ret = init_ipmi_reqdata_for_chassis_sn(&req_msg_head, req_data, sizeof(req_data));
    if (ret != RET_OK) {
        return RET_ERR;
    }

    
    for (i = 0; i < 3; i++) {
        ret = ipmi_send_request(&req_msg_head, &req_data, MAX_POSSIBLE_IPMI_FRAME_LEN, &resp_ipmi, TRUE);
        if (ret == RET_OK) {
            break;
        }
        vos_task_delay(1000); // 延时1000ms
    }

    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: ipmi_send_request failed, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    tmp_resp = (const IPMIMSG_GET_SHELF_DATA_RESP_S *)get_ipmi_src_data((gconstpointer)resp_ipmi);
    if (tmp_resp == NULL) {
        goto ERR_EXIT;
    }
    if (tmp_resp->completion != COMP_CODE_SUCCESS) {
        debug_log(DLOG_DEBUG, "%s: completion code error, completion=0x%x",
            __FUNCTION__, tmp_resp->completion);
        goto ERR_EXIT;
    }

    data_len = get_ipmi_src_data_len(resp_ipmi);
    ret = copy_chassis_sn_resp_data(buf, buf_size, tmp_resp, data_len);
    if (ret != RET_OK) {
        goto ERR_EXIT;
    }

    g_free(resp_ipmi);
    return RET_OK;

ERR_EXIT:
    
    g_free(resp_ipmi);

    return RET_ERR;
}


LOCAL void get_chasssis_sn(guint8 get_count)
{
    gint32 ret;
    OBJ_HANDLE elabel_handle = 0;
    gchar buf[FRU_MAX_ELABEL_LEN + 1] = {0};
    gchar chassis_sn[FRU_MAX_ELABEL_LEN + 1] = {0};
    static gchar local_chassis_sn[FRU_MAX_ELABEL_LEN + 1] = {0};

    if (dal_get_vsmm_ms_status() == MSM_PRIMARY_STATE) {
        debug_log(DLOG_DEBUG, "%s: vsmm master blade do not support!", __FUNCTION__);
        return;
    }

    
    ret = dal_get_object_handle_nth(CLASS_ELABEL, 0, &elabel_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: get main board elabel object failed, ret=%d", __FUNCTION__, ret);
        return;
    }
    
    ret = dal_get_property_value_string(elabel_handle, PROPERTY_ELABEL_CHASSIS_SN,
        chassis_sn, sizeof(chassis_sn));
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: get chassis sn failed, ret=%d", __FUNCTION__, ret);
        return;
    }
    if ((strlen(local_chassis_sn) != 0) && (strcmp(local_chassis_sn, chassis_sn) == 0)) {
        return;
    }
    
    ret = get_chassis_sn_from_smm(buf, sizeof(buf));
    if (ret != RET_OK) {
        if (get_count < MAX_PRINT_COUNT) {
            debug_log(DLOG_ERROR, "%s: get chassis sn from smm fail, ret=%d", __FUNCTION__, ret);
        }
        return;
    }

    
    if ((strlen(buf) == 0) || (strcmp(buf, chassis_sn) == 0)) {
        
        debug_log(DLOG_ERROR, "%s: get chassis sn success, sn:%s.", __FUNCTION__, buf);
        return;
    }
    
    ret = dal_set_property_value_string(elabel_handle, PROPERTY_ELABEL_CHASSIS_SN, buf, DF_NONE);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: set chassis sn fail, ret=%d", __FUNCTION__, ret);
        return;
    }
    (void)memcpy_s(local_chassis_sn, sizeof(local_chassis_sn), buf, sizeof(buf));

    
    debug_log(DLOG_DEBUG, "%s: get chassis sn success, sn:%s.", __FUNCTION__, buf);

    return;
}


void task_get_chassis_sn(void)
{
    guint8 count = 0;
    guchar mgmt_software_type = 0xFF;

    (void)prctl(PR_SET_NAME, (uintptr_t)"getChassisSN");
    debug_log(DLOG_DEBUG, "%s: Enter get chassis sn task.", __FUNCTION__);

    gint32 ret = fru_elabel_init();
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: fru elabel init fail", __FUNCTION__);
        return;
    }

    (void)dal_get_software_type(&mgmt_software_type);
    if (mgmt_software_type != MGMT_SOFTWARE_TYPE_BLADE) {
        debug_log(DLOG_DEBUG, "%s: mgmt_software_type = 0x%02X.", __FUNCTION__, mgmt_software_type);
        return;
    }
    
    vos_task_delay(60 * 1000);
    while (TRUE) {
        get_chasssis_sn(count);
        count++;
        count = (count > MAX_PRINT_COUNT) ? MAX_PRINT_COUNT : count;
        
        vos_task_delay(600 * 1000);
    }

    debug_log(DLOG_DEBUG, "%s: Exit get chassis sn task.", __FUNCTION__);
    return;
}
void init_update_elabel_lock(void)
{
    g_mutex_init(&g_update_elabel_mutex);
}
