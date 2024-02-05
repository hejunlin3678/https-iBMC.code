

#include "intf/dump.h"
#include "common.h"
#include "media/e2p_compact.h"

#define FRU_MAX_CMD_LENGTH 256

LOCAL void __print_chassis_info(OBJ_HANDLE obj_handle, const gchar *property_name, gchar *out_val, guint32 out_len,
    guint32 out_val_size, gchar *print_buf, const gchar *print_info);
LOCAL void __print_board_info(OBJ_HANDLE obj_handle, const gchar *property_name, gchar *fru_data, guint32 fru_data_size,
    gchar *print_buf, const gchar *board_info);
LOCAL gint32 __splice_elabel(gchar *print_buf, gsize buf_size, gchar *elabel);
LOCAL gint32 __get_ref_elabel_obj_handle(OBJ_HANDLE obj_handle, const char *ref_obj_elabel, OBJ_HANDLE *ref_obj_handle);
LOCAL gint32 __print_fru_chassis_info(OBJ_HANDLE obj_handle, gchar *print_buf, gsize buf_size);
LOCAL gint32 __print_fru_board_info(OBJ_HANDLE obj_handle, gchar *print_buf, gsize buf_size);
LOCAL gint32 __print_fru_product_info(OBJ_HANDLE obj_handle, gchar *print_buf, gsize buf_size);
LOCAL gint32 __print_fru_info(OBJ_HANDLE obj_handle, gchar *print_buf, gsize buf_size);
LOCAL void __array_object_handle_by_fruid(FRU_OBJECT_S *fru_object, guint32 count);
LOCAL gint32 dump_fru_info_all(const gchar *path);

LOCAL void __print_chassis_info(OBJ_HANDLE obj_handle, const gchar *property_name, gchar *out_val, guint32 out_len,
    guint32 out_val_size, gchar *print_buf, const gchar *print_info)
{
    (void)dal_get_property_value_string(obj_handle, property_name, out_val, out_len);
    if (strlen(out_val) == 0) {
        return;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    gint32 ret = snprintf_s(print_buf + strlen(print_buf), SIZE_4K - strlen(print_buf), SIZE_4K - strlen(print_buf) - 1,
        print_info, out_val);
#pragma GCC diagnostic pop
    (void)memset_s(out_val, out_val_size, 0, out_val_size);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
}

LOCAL void __print_board_info(OBJ_HANDLE obj_handle, const gchar *property_name, gchar *fru_data, guint32 fru_data_size,
    gchar *print_buf, const gchar *board_info)
{
    (void)dal_get_property_value_string(obj_handle, property_name, fru_data, FRU_DATA_STRING_LEN);
    if (strlen(fru_data) == 0) {
        return;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    errno_t safe_func_ret = snprintf_s(print_buf + strlen(print_buf), SIZE_4K - strlen(print_buf),
        SIZE_4K - strlen(print_buf) - 1, board_info, fru_data);
#pragma GCC diagnostic pop
    (void)memset_s(fru_data, fru_data_size, 0, fru_data_size);
    if (safe_func_ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, safe_func_ret);
    }
}

LOCAL gint32 __splice_elabel(gchar *print_buf, gsize buf_size, gchar *elabel)
{
    
    const gchar *delim = ";";
    gchar *new_elabel = NULL;
    gchar *p = strtok_r(elabel, delim, &new_elabel);
    if (p == NULL) {
        return RET_ERR;
    }

    gint32 ret = snprintf_s(print_buf + strlen(print_buf), buf_size - strlen(print_buf),
        buf_size - strlen(print_buf) - 1, "%s\n", p);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    while ((p = strtok_r(new_elabel, delim, &new_elabel))) {
        ret = snprintf_s(print_buf + strlen(print_buf), buf_size - strlen(print_buf), buf_size - strlen(print_buf) - 1,
            "\r                         %s\n", p);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
    }
    return RET_OK;
}

LOCAL gint32 __get_ref_elabel_obj_handle(OBJ_HANDLE obj_handle, const char *ref_obj_elabel, OBJ_HANDLE *ref_obj_handle)
{
    OBJ_HANDLE ref_handle = 0;
    gint32 ret = dfl_get_referenced_object(obj_handle, PROPERTY_FRU_ELABEL_RIMM_OBJ, &ref_handle);
    if (ret == RET_OK) {
        return dfl_get_object_handle(dfl_get_object_name(ref_handle), ref_obj_handle);
    }

    return dfl_get_object_handle(ref_obj_elabel, ref_obj_handle);
}

LOCAL gint32 __print_fru_chassis_info(OBJ_HANDLE obj_handle, gchar *print_buf, gsize buf_size)
{
    gchar fru_data[FRU_DATA_STRING_LEN] = {0};
    __print_chassis_info(obj_handle, PROPERTY_ELABEL_CHASSIS_TYPE, fru_data, FRU_DATA_STRING_LEN, FRU_DATA_STRING_LEN,
        print_buf, "\r Chassis Type          : %s\n");
    __print_chassis_info(obj_handle, PROPERTY_ELABEL_CHASSIS_PN, fru_data, FRU_DATA_STRING_LEN, FRU_DATA_STRING_LEN,
        print_buf, "\r Chassis Part Number   : %s\n");
    __print_chassis_info(obj_handle, PROPERTY_ELABEL_CHASSIS_SN, fru_data, FRU_DATA_STRING_LEN, FRU_DATA_STRING_LEN,
        print_buf, "\r Chassis Serial Number : %s\n");

    gchar elabel[ELABEL_MAX_LEN + 1] = {0};
    (void)dal_get_property_value_string(obj_handle, PROPERTY_ELABEL_CHASSIS_EL, elabel, ELABEL_MAX_LEN + 1);
    if (strlen(elabel) == 0) {
        return RET_OK;
    }

    gint32 ret = snprintf_s(print_buf + strlen(print_buf), buf_size - strlen(print_buf),
        buf_size - strlen(print_buf) - 1, "\r Chassis Extend label          : ");
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    return __splice_elabel(print_buf, buf_size, elabel);
}

LOCAL gint32 __print_fru_board_info(OBJ_HANDLE obj_handle, gchar *print_buf, gsize buf_size)
{
    gchar fru_data[FRU_DATA_STRING_LEN] = {0};
    __print_board_info(obj_handle, PROPERTY_ELABEL_BOARD_MFG_TIME, fru_data, FRU_DATA_STRING_LEN, print_buf,
        "\r Board Mfg. Date       : %s\n");
    __print_board_info(obj_handle, PROPERTY_ELABEL_BOARD_MFG_NAME, fru_data, FRU_DATA_STRING_LEN, print_buf,
        "\r Board Manufacturer    : %s\n");
    __print_board_info(obj_handle, PROPERTY_ELABEL_BOARD_PRO_NAME, fru_data, FRU_DATA_STRING_LEN, print_buf,
        "\r Board Product Name    : %s\n");
    __print_board_info(obj_handle, PROPERTY_ELABEL_BOARD_SN, fru_data, FRU_DATA_STRING_LEN, print_buf,
        "\r Board Serial Number   : %s\n");
    __print_board_info(obj_handle, PROPERTY_ELABEL_BOARD_PN, fru_data, FRU_DATA_STRING_LEN, print_buf,
        "\r Board Part Number     : %s\n");
    __print_board_info(obj_handle, PROPERTY_ELABEL_BOARD_ID, fru_data, FRU_DATA_STRING_LEN, print_buf,
        "\r Board FRU File ID     : %s\n");

    gchar elabel[ELABEL_MAX_LEN + 1] = {0};
    (void)dal_get_property_value_string(obj_handle, PROPERTY_ELABEL_BOARD_EL, elabel, ELABEL_MAX_LEN + 1);

    if (strlen(elabel) == 0) {
        return RET_OK;
    }

    gint32 ret = snprintf_s(print_buf + strlen(print_buf), buf_size - strlen(print_buf),
        buf_size - strlen(print_buf) - 1, "\r Extend label          : ");
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    return __splice_elabel(print_buf, buf_size, elabel);
}

LOCAL gint32 __print_fru_product_info(OBJ_HANDLE obj_handle, gchar *print_buf, gsize buf_size)
{
    gchar fru_data[FRU_DATA_STRING_LEN] = {0};
    gint32 ret;

    (void)dal_get_property_value_string(obj_handle, PROPERTY_ELABEL_PRODUCT_MFG_NAME, fru_data, FRU_DATA_STRING_LEN);
    if (strlen(fru_data) != 0) {
        ret = snprintf_s(print_buf + strlen(print_buf), buf_size - strlen(print_buf), buf_size - strlen(print_buf) - 1,
            "\r Product Manufacturer  : %s\n", fru_data);
        (void)memset_s(fru_data, FRU_DATA_STRING_LEN, 0, FRU_DATA_STRING_LEN);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret);
        }
    }

    (void)dal_get_property_value_string(obj_handle, PROPERTY_ELABEL_PRODUCT_NAME, fru_data, FRU_DATA_STRING_LEN);
    if (strlen(fru_data) != 0) {
        ret = snprintf_s(print_buf + strlen(print_buf), buf_size - strlen(print_buf), buf_size - strlen(print_buf) - 1,
            "\r Product Name          : %s\n", fru_data);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
        (void)memset_s(fru_data, FRU_DATA_STRING_LEN, 0, FRU_DATA_STRING_LEN);
    }

    (void)dal_get_property_value_string(obj_handle, PROPERTY_ELABEL_PRODUCT_PN, fru_data, FRU_DATA_STRING_LEN);
    if (strlen(fru_data) != 0) {
        ret = snprintf_s(print_buf + strlen(print_buf), buf_size - strlen(print_buf), buf_size - strlen(print_buf) - 1,
            "\r Product Part Number   : %s\n", fru_data);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
        (void)memset_s(fru_data, FRU_DATA_STRING_LEN, 0, FRU_DATA_STRING_LEN);
    }

    (void)dal_get_property_value_string(obj_handle, PROPERTY_ELABEL_PRODUCT_VERSION, fru_data, FRU_DATA_STRING_LEN);
    if (strlen(fru_data) != 0) {
        ret = snprintf_s(print_buf + strlen(print_buf), buf_size - strlen(print_buf), buf_size - strlen(print_buf) - 1,
            "\r Product Version       : %s\n", fru_data);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
        (void)memset_s(fru_data, FRU_DATA_STRING_LEN, 0, FRU_DATA_STRING_LEN);
    }

    (void)dal_get_property_value_string(obj_handle, PROPERTY_ELABEL_PRODUCT_SN, fru_data, FRU_DATA_STRING_LEN);
    if (strlen(fru_data) != 0) {
        ret = snprintf_s(print_buf + strlen(print_buf), buf_size - strlen(print_buf), buf_size - strlen(print_buf) - 1,
            "\r Product Serial Number : %s\n", fru_data);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
        (void)memset_s(fru_data, FRU_DATA_STRING_LEN, 0, FRU_DATA_STRING_LEN);
    }

    (void)dal_get_property_value_string(obj_handle, PROPERTY_ELABEL_PRODUCT_ASSET_TAG, fru_data, FRU_DATA_STRING_LEN);
    if (strlen(fru_data) != 0) {
        ret = snprintf_s(print_buf + strlen(print_buf), buf_size - strlen(print_buf), buf_size - strlen(print_buf) - 1,
            "\r Product Asset Tag     : %s\n", fru_data);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
        (void)memset_s(fru_data, FRU_DATA_STRING_LEN, 0, FRU_DATA_STRING_LEN);
    }

    (void)dal_get_property_value_string(obj_handle, PROPERTY_ELABEL_PRODUCT_ID, fru_data, FRU_DATA_STRING_LEN);
    if (strlen(fru_data) != 0) {
        ret = snprintf_s(print_buf + strlen(print_buf), buf_size - strlen(print_buf), buf_size - strlen(print_buf) - 1,
            "\r Product FRU File ID   : %s\n", fru_data);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
        (void)memset_s(fru_data, FRU_DATA_STRING_LEN, 0, FRU_DATA_STRING_LEN);
    }

    gchar elabel[ELABEL_MAX_LEN + 1] = {0};
    (void)dal_get_property_value_string(obj_handle, PROPERTY_ELABEL_PRODUCT_EL, elabel, ELABEL_MAX_LEN + 1);
    if (strlen(elabel) == 0) {
        return RET_OK;
    }

    ret = snprintf_s(print_buf + strlen(print_buf), buf_size - strlen(print_buf), buf_size - strlen(print_buf) - 1,
        "\r Product Extend label          : ");
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret);
    }

    return __splice_elabel(print_buf, buf_size, elabel);
}


LOCAL gint32 __print_fru_info(OBJ_HANDLE obj_handle, gchar *print_buf, gsize buf_size)
{
    if (print_buf == NULL || buf_size == 0) {
        debug_log(DLOG_ERROR, "obj_handle OR print_buf is NULL.");
        return RET_ERR;
    }

    guint8 fru_type = 0;
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_FRU_TYPE, &fru_type);
    if (fru_type == FRU_TYPE_POWER) {
        return RET_OK;
    }

    gchar ref_obj_elabel[FRU_DATA_STRING_LEN] = {0};
    (void)dal_get_property_value_string(obj_handle, PROPERTY_FRU_ELABEL_RO, ref_obj_elabel, FRU_DATA_STRING_LEN);
    if (strlen(ref_obj_elabel) == 0) {
        return RET_OK;
    }

    OBJ_HANDLE ref_obj_handle = 0;
    gint32 ret = __get_ref_elabel_obj_handle(obj_handle, ref_obj_elabel, &ref_obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "__get_ref_elabel_obj_handle error, ret = %d.", ret);
        return ret;
    }

    gchar  fru_data[FRU_DATA_STRING_LEN] = {0};
    (void)dal_get_property_value_string(obj_handle, PROPERTY_FRU_NAME, fru_data, FRU_DATA_STRING_LEN);

    guint8 fru_id = MAX_FRU_ID;
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_FRU_ID, &fru_id);

    ret = snprintf_s(print_buf + strlen(print_buf), buf_size - strlen(print_buf), buf_size - strlen(print_buf) - 1,
        "FRU Device Description : Builtin FRU Device (ID %u, %s)\n", fru_id, fru_data);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    ret = __print_fru_chassis_info(ref_obj_handle, print_buf, buf_size);
    if (ret != RET_OK) {
        return ret;
    }

    ret = __print_fru_board_info(ref_obj_handle, print_buf, buf_size);
    if (ret != RET_OK) {
        return ret;
    }

    ret = __print_fru_product_info(ref_obj_handle, print_buf, buf_size);
    if (ret != RET_OK) {
        return ret;
    }

    ret =
        snprintf_s(print_buf + strlen(print_buf), buf_size - strlen(print_buf), buf_size - strlen(print_buf) - 1, "\n");
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    return RET_OK;
}


LOCAL void __array_object_handle_by_fruid(FRU_OBJECT_S *fru_object, guint32 count)
{
    FRU_OBJECT_S object_tmp = { 0 };
    for (guint32 i = 0; i < count; i++) {
        for (guint32 j = 0; j < count - i - 1; j++) {
            if (fru_object[j].fru_id <= fru_object[j + 1].fru_id) {
                continue;
            }

            errno_t safe_fun_ret = memcpy_s(&object_tmp, sizeof(FRU_OBJECT_S), &fru_object[j], sizeof(FRU_OBJECT_S));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "memcpy_s failed");
            }
            safe_fun_ret = memcpy_s(&fru_object[j], sizeof(FRU_OBJECT_S), &fru_object[j + 1], sizeof(FRU_OBJECT_S));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "memcpy_s failed");
            }
            safe_fun_ret = memcpy_s(&fru_object[j + 1], sizeof(FRU_OBJECT_S), &object_tmp, sizeof(FRU_OBJECT_S));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "memcpy_s failed");
            }
        }
    }
    return;
}


LOCAL gint32 dump_fru_info_all(const gchar *path)
{
    gchar *print_buf = (gchar *)g_malloc0(SIZE_4K);
    if (print_buf == NULL) {
        debug_log(DLOG_ERROR, "g_malloc0 len:4K fail!");
        return RET_ERR;
    }

    gchar file_name[FRU_MAX_CMD_LENGTH] = {0};
    gint32 ret = snprintf_s(file_name, sizeof(file_name), sizeof(file_name) - 1, "%s/fruinfo.txt", path);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    FILE *fp = dal_fopen_check_realpath(file_name, "a+", file_name);
    if (fp == NULL) {
        g_free(print_buf);
        return RET_ERR;
    }
    (void)fchmod(fileno(fp), 0640);
    size_t fwrite_back;

    // The do while(0) is only for process control.
    do {
        GSList *obj_list = NULL;
        ret = dfl_get_object_list(CLASS_FRU, &obj_list);
        if (ret != DFL_OK) {
            (void)snprintf_s(print_buf + strlen(print_buf), SIZE_4K - strlen(print_buf),
                SIZE_4K - strlen(print_buf) - 1, "Get FRU object list failed.\r\n");
            break;
        }

        guint32 obj_num = g_slist_length(obj_list);
        if (obj_num == 0) {
            g_slist_free(obj_list);
            (void)snprintf_s(print_buf + strlen(print_buf), SIZE_4K - strlen(print_buf),
                SIZE_4K - strlen(print_buf) - 1, "System has no fru information.\r\n");
            break;
        }

        FRU_OBJECT_S *fru_object = (FRU_OBJECT_S *)malloc(sizeof(FRU_OBJECT_S) * obj_num);
        if (fru_object == NULL) {
            (void)snprintf_s(print_buf + strlen(print_buf), SIZE_4K - strlen(print_buf),
                SIZE_4K - strlen(print_buf) - 1, "CLI malloc failed.\r\n");
            g_slist_free(obj_list);
            break;
        }

        (void)memset_s(fru_object, sizeof(FRU_OBJECT_S) * obj_num, 0, sizeof(FRU_OBJECT_S) * obj_num);
        guint32 count = 0;
        for (GSList *obj_note = obj_list; obj_note; obj_note = obj_note->next) {
            fru_object[count].obj_handle = (OBJ_HANDLE)obj_note->data;
            (void)dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROPERTY_FRU_ID, &(fru_object[count].fru_id));
            count++;
        }

        g_slist_free(obj_list);
        __array_object_handle_by_fruid(fru_object, obj_num);

        
        for (count = 0; count < obj_num; count++) {
            ret = __print_fru_info(fru_object[count].obj_handle, print_buf, SIZE_4K);
            if (ret != RET_OK) {
                (void)snprintf_s(print_buf + strlen(print_buf), SIZE_4K - strlen(print_buf),
                    SIZE_4K - strlen(print_buf) - 1, "Get fru%d information failed.\r\n", fru_object[count].fru_id);
                debug_log(DLOG_ERROR, "print_fru_info failed!\n");
                ret = FALSE;
            }

            (void)e2p_cafe_dump_fru_info(fru_object[count].obj_handle, print_buf, SIZE_4K);
            if (strlen(print_buf) == 0) {
                (void)memset_s(print_buf, SIZE_4K, 0, SIZE_4K);
                continue;
            }

            fwrite_back = fwrite(print_buf, strlen(print_buf), 1, fp);
            if (fwrite_back != 1) {
                debug_log(DLOG_ERROR, "fwrite failed!");
            }
            (void)memset_s(print_buf, SIZE_4K, 0, SIZE_4K);
        }

        free(fru_object);
        fru_object = NULL;
    } while (0);

    if (strlen(print_buf) != 0) {
        fwrite_back = fwrite(print_buf, strlen(print_buf), 1, fp);
        if (fwrite_back != 1) {
            debug_log(DLOG_ERROR, "fwrite failed!");
        }
    }

    g_free(print_buf);
    (void)fclose(fp);

    return ret;
}


gint32 dump_frudata_info(const gchar *path)
{
    if (path == NULL || strlen(path) >= MAX_CMD_LENGTH) {
        debug_log(DLOG_ERROR, "path is incorrect.");
        return RET_ERR;
    }

    if (strstr(path, "&") != NULL || strstr(path, "||") != NULL || strstr(path, ";") != NULL) {
        debug_log(DLOG_ERROR, "path is wrong");
        return RET_ERR;
    }

    if (dump_fru_info_all(path) != RET_OK) {
        debug_log(DLOG_ERROR, "dump_get_fruinfo failed.");
        return RET_ERR;
    }

    debug_log(DLOG_DEBUG, "dump_get_fruinfo finished.");

    return RET_OK;
}