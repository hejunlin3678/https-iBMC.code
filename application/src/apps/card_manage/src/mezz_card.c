


#include "pme_app/pme_app.h"


#include "mezz_card.h"
#include "get_version.h"
#include "card_manage_module.h"


#define CHIP_OBJ_CNT 16
#define SVID_WRITE_INDEX 0
#define SVID_READ_INDEX 1
#define VCC_WRITE_INDEX 2
#define VCC0V9_CHIP_INDEX 3
#define SVID_CHIP_INDEX 4
#define VCC1V0_CHIP_INDEX 5
#define VCC1V8_CHIP_INDEX 6
#define VCC2V5_CHIP_INDEX 7
#define VCC3V3_CHIP_INDEX 8
#define MONITOR_DATA_UNAVAILABLE 0X4000
#define MEZZOPA_INVALID_READING 0x8000





LOCAL void mezz_card_update_resId_info(OBJ_HANDLE obj_handle)
{
    
    guint8 cpu_id = 0;
    gint32 ret = 0;
    // 更新当前MezzCard属于哪个CPU
    
    ret = get_card_cpu_id(obj_handle, &cpu_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Get %s ResId failed! ret:%d.", __FUNCTION__, dfl_get_object_name(obj_handle), ret);
        return;
    }
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_MEZZCARD_CPUID, cpu_id, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Update %s ResId(%d) failed! ret:%d.", __FUNCTION__, dfl_get_object_name(obj_handle),
            cpu_id, ret);
        return;
    }
    
    
}

LOCAL gint32 process_each_mezzcard(OBJ_HANDLE object_handle, gpointer user_data)
{
    gint32 ret = 0;

    
    vos_task_delay(200);
    
    mezz_card_update_resId_info(object_handle);
    
    ret = get_pcb_version(object_handle, PROPERTY_MEZZCARD_PCBID, PROPERTY_MEZZCARD_PCBVER);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    return RET_OK;
}

gint32 mezz_card_add_object_callback(OBJ_HANDLE object_handle)
{
    return process_each_mezzcard(object_handle, NULL);
}

gint32 mezz_card_del_object_callback(OBJ_HANDLE object_handle)
{
    return RET_OK;
}

gint32 mezz_card_dump_info(const gchar *path)
{
    gint32 iRet = -1;
    gint32 ret = 0;
    guint16 board_id = 0;
    const gchar *pcb_ver = NULL;
    const gchar *name = NULL;
    const gchar *card_desc = NULL;
    const gchar *partnum = NULL;
    const gchar *manufacturer = NULL;
    gchar file_name[MEZZ_CARD_DUMPINFO_MAX_LEN + 1] = {0};
    gchar mezz_card_info[MEZZ_CARD_DUMPINFO_MAX_LEN + 1] = {0};
    FILE *fp = NULL;

    guint8 slot_id = 0;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    size_t fwrite_back = 0;

    if (path == NULL) {
        return RET_ERR;
    }

    ret = dfl_get_object_list(CLASS_MEZZCARD_NAME, &obj_list);
    if (ret != DFL_OK) {
        if (ERRCODE_OBJECT_NOT_EXIST == ret) {
            return RET_OK;
        }

        return RET_ERR;
    }

    // 在dump_dir目录下创建文件
    
    iRet = snprintf_s(file_name, MEZZ_CARD_DUMPINFO_MAX_LEN + 1, MEZZ_CARD_DUMPINFO_MAX_LEN, "%s/card_info", path);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    fp = dal_fopen_check_realpath(file_name, "a+", file_name);
    if (fp == NULL) {
        g_slist_free(obj_list);
        return RET_ERR;
    }

    
    (void)fchmod(fileno(fp), COMMON_DUMP_FILE_UMASK);
    
    (void)snprintf_s(mezz_card_info, MEZZ_CARD_DUMPINFO_MAX_LEN + 1, MEZZ_CARD_DUMPINFO_MAX_LEN, "%s",
        "Mezz Card Info\n");
    fwrite_back = fwrite(mezz_card_info, strlen(mezz_card_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    (void)memset_s(mezz_card_info, MEZZ_CARD_DUMPINFO_MAX_LEN + 1, 0, MEZZ_CARD_DUMPINFO_MAX_LEN + 1);

    (void)snprintf_s(mezz_card_info, MEZZ_CARD_DUMPINFO_MAX_LEN + 1, MEZZ_CARD_DUMPINFO_MAX_LEN,
        "%-4s | %-10s | %-10s | %-32s | %-10s | %-50s | %-8s\n", "Slot", "BoardId", "Name", "Manufacturer", "PCB Ver",
        "Card Desc", "PartNum");
    fwrite_back = fwrite(mezz_card_info, strlen(mezz_card_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        property_name_list = g_slist_append(property_name_list, PROPERTY_MEZZCARD_SLOT);
        property_name_list = g_slist_append(property_name_list, PROPERTY_MEZZCARD_BOARDID);
        property_name_list = g_slist_append(property_name_list, PROPERTY_MEZZCARD_PRODUCTNAME);
        property_name_list = g_slist_append(property_name_list, PROPERTY_MEZZCARD_MANUFACTURER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_MEZZCARD_PCBVER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_MEZZCARD_DESC);
        // 201705108246 支持PARTNUM带出 20190723 RAOJIANZHONG 00267466
        property_name_list = g_slist_append(property_name_list, PROPERTY_MEZZCARD_PART_NUM);

        ret = dfl_multiget_property_value((OBJ_HANDLE)obj_note->data, property_name_list, &property_value);
        g_slist_free(property_name_list);
        property_name_list = NULL;

        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Get mezz card information failed!");
            g_slist_free(obj_list);
            (void)fclose(fp);
            return RET_ERR;
        }

        slot_id = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
        board_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 1));
        name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 2), 0);
        manufacturer = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 3), 0);
        pcb_ver = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 4), 0);
        card_desc = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 5), 0);
        // 201705108246 支持PARTNUM带出 20190723 RAOJIANZHONG 00267466
        partnum = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 6), 0);
        if (board_id == 0) {
            
            g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
            property_value = NULL;
            continue;
        }

        (void)memset_s(mezz_card_info, MEZZ_CARD_DUMPINFO_MAX_LEN + 1, 0, MEZZ_CARD_DUMPINFO_MAX_LEN + 1);
        iRet = snprintf_s(mezz_card_info, sizeof(mezz_card_info), sizeof(mezz_card_info) - 1,
            "%-4u | 0x%-4x     | %-10s | %-32s | %-10s | %-50s | %-8s\n", slot_id, board_id, name, manufacturer,
            pcb_ver, card_desc, partnum);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
        }
        fwrite_back = fwrite(mezz_card_info, strlen(mezz_card_info), 1, fp);
        if (fwrite_back != 1) {
            (void)fclose(fp);
            g_slist_free(obj_list);
            g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
            debug_log(DLOG_ERROR, "fwrite failed!");
            return RET_ERR;
        }
        (void)memset_s(mezz_card_info, MEZZ_CARD_DUMPINFO_MAX_LEN + 1, 0, MEZZ_CARD_DUMPINFO_MAX_LEN + 1);
        g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
        property_value = NULL;
    }

    (void)snprintf_s(mezz_card_info, MEZZ_CARD_DUMPINFO_MAX_LEN + 1, MEZZ_CARD_DUMPINFO_MAX_LEN, "%s", "\n\n");
    
    fwrite_back = fwrite(mezz_card_info, strlen(mezz_card_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }
    (void)memset_s(mezz_card_info, MEZZ_CARD_DUMPINFO_MAX_LEN + 1, 0, MEZZ_CARD_DUMPINFO_MAX_LEN + 1);
    g_slist_free(obj_list);
    // 关闭文件
    (void)fclose(fp);

    return RET_OK;
}


gint32 mezz_card_init(void)
{
    (void)dfl_foreach_object(CLASS_MEZZCARD_NAME, process_each_mezzcard, NULL, NULL);

    return RET_OK;
}


LOCAL gint32 start_read_mezz_svid(const gchar *obj_name_swchip, const gchar *obj_name_srchip,
    const gchar *obj_name_svidchip, guint16 *card_vcc)
{
#define ENABLE_PCA9555_OFFSET 0x06
#define PCA9555_DATA_LENGTH 0x01
#define NULL_PCA955_DATA_LENGTH 0x00
#define SWCHIP_DATA_OFFSET 0x02
#define CARD_VCC_DATA_LENGTH 0x02
#define PCA9555_OFFSET_MOS 0x09
#define READ_PCA9555_OFFSET 0x00
    guint8 data_1 = 0xfe;
    guint8 data_2 = 0xff;
    gint32 ret = RET_OK;
    OBJ_HANDLE vcc_handle;

    // 访问chip前需要先获取下句柄才能找到对象，框架的问题，不能删除必须保留
    (void)dfl_get_object_handle(obj_name_swchip, &vcc_handle);
    (void)dfl_get_object_handle(obj_name_svidchip, &vcc_handle);
    (void)dfl_get_object_handle(obj_name_srchip, &vcc_handle);

    
    ret = dfl_chip_blkwrite(obj_name_swchip, ENABLE_PCA9555_OFFSET, PCA9555_DATA_LENGTH, &data_1);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "write chip %s offset:0x06 failed, ret=%d", obj_name_swchip, ret);
        return RET_ERR;
    }
    
    ret = dfl_chip_blkwrite(obj_name_swchip, SWCHIP_DATA_OFFSET, PCA9555_DATA_LENGTH, &data_2);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "write chip %s offset:0x02 failed, ret=%d", obj_name_swchip, ret);
        return RET_ERR;
    }
    vos_task_delay(200);

    
    ret = dfl_chip_blkwrite(obj_name_svidchip, PCA9555_OFFSET_MOS, NULL_PCA955_DATA_LENGTH, &data_1);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "write chip %s offset:0x09 failed, ret=%d", obj_name_svidchip, ret);
        return RET_ERR;
    }
    
    ret = dfl_chip_blkread(obj_name_srchip, READ_PCA9555_OFFSET, CARD_VCC_DATA_LENGTH, card_vcc);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "read chip %s offset:0x00 failed, ret=%d", obj_name_svidchip, ret);
        return RET_ERR;
    }
    
    ret = dfl_chip_blkwrite(obj_name_swchip, SWCHIP_DATA_OFFSET, PCA9555_DATA_LENGTH, &data_1);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "write chip %s offset:0x00 failed, ret=%d", obj_name_swchip, ret);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 handle_mezz_svid_vcc_config(void)
{
    errno_t safe_fun_ret = EOK;
#define MOS_OFFSET_DATA 0x07
#define WRITE_DATA_LENGTH 0x01
#define WRITE_CHIP_LENGTH 0x03
#define NULL_OFFSE_ADDR 0x00
#define OPEN_MOS_DATA 0xf0
#define CLOSE_MOS_DATA 0xf0
#define ENABLE_96080_READ 0x1
    gint32 ret = RET_OK;
    guint8 svid_data = 0;
    guint16 card_vcc = 0;
    GSList *mezz_opa_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE mzd_obj_handle = 0;
    GVariant *array_obj_name_gvar = NULL;
    GVariant *obj_name_gvar = NULL;
    GVariantIter tmp_iter;
    guint8 obj_name_flag = 0;
    gchar chip_obj_name[CHIP_OBJ_CNT][MAX_OBJECT_NAME_LEN];
    const gchar *obj_name_temp = NULL;
    guint8 mezz_svid_faild_cnt = 0;
    guint8 data_3 = OPEN_MOS_DATA;
    guint8 data_4 = CLOSE_MOS_DATA;
    guint8 data_5 = ENABLE_96080_READ;
    OBJ_HANDLE obj_handle = 0;

    vos_task_delay(15000); // 延时等待网卡芯片启动完成

    (void)memset_s(chip_obj_name, sizeof(chip_obj_name), 0, sizeof(chip_obj_name));

    ret = dfl_get_object_list(CLASS_MEZZCARDOPA_NAME, &mezz_opa_list);
    if ((ret != DFL_OK) || (mezz_opa_list == NULL)) {
        debug_log(DLOG_DEBUG, "%s: Get class (%s) handle list failed", __FUNCTION__, CLASS_MEZZCARD_NAME);
        return RET_OK;
    }

    
    for (obj_node = mezz_opa_list; obj_node; obj_node = obj_node->next) {
        // 1 获取OpaSvidChip数组中的元素
        mzd_obj_handle = (OBJ_HANDLE)obj_node->data;
        ret = dfl_get_property_value(mzd_obj_handle, PROPERTY_MEZZCARDOPA_SVIDTYPECHIP, &array_obj_name_gvar);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: failed for dfl_get_property_value.", __FUNCTION__);
            continue;
        }
        if (array_obj_name_gvar == NULL) {
            debug_log(DLOG_DEBUG, "%s: array is null", __FUNCTION__);
            continue;
        }
        (void)g_variant_iter_init(&tmp_iter, array_obj_name_gvar);

        
        obj_name_flag = 0;

        
        while ((obj_name_gvar = g_variant_iter_next_value(&tmp_iter)) != NULL) {
            obj_name_temp = g_variant_get_string(obj_name_gvar, NULL);
            
            if (obj_name_temp == NULL) {
                g_variant_unref(obj_name_gvar);
                debug_log(DLOG_ERROR, "%s: failed for g_variant_get_string.", __FUNCTION__);
                continue;
            }
            (void)memset_s(chip_obj_name[obj_name_flag], MAX_OBJECT_NAME_LEN, 0, MAX_OBJECT_NAME_LEN);
            safe_fun_ret =
                strncpy_s(chip_obj_name[obj_name_flag], MAX_OBJECT_NAME_LEN, obj_name_temp, MAX_OBJECT_NAME_LEN - 1);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            }
            g_variant_unref(obj_name_gvar);
            obj_name_flag++;
        }

        g_variant_unref(array_obj_name_gvar);
        array_obj_name_gvar = NULL;

        
        // 访问chip前需要先获取下句柄才能找到对象，框架的问题，不能删除必须保留
        (void)dfl_get_object_handle(chip_obj_name[SVID_WRITE_INDEX], &obj_handle);
        (void)dfl_get_object_handle(chip_obj_name[VCC_WRITE_INDEX], &obj_handle);

        // 2 调压
        
        for (mezz_svid_faild_cnt = 0; mezz_svid_faild_cnt < 3; mezz_svid_faild_cnt++) {
            ret = start_read_mezz_svid(chip_obj_name[SVID_WRITE_INDEX], chip_obj_name[SVID_READ_INDEX],
                chip_obj_name[SVID_CHIP_INDEX], &card_vcc);
            if (ret == RET_OK) {
                break;
            }

            vos_task_delay(5000);
        }

        if (mezz_svid_faild_cnt == 3) {
            (void)dal_set_property_value_byte(mzd_obj_handle, PROPERTY_MEZZCARDOPA_MEZZSVIDREADFAIL, 1, DF_NONE);
            debug_log(DLOG_ERROR, " %s read svid vcc failed ret=%d", dfl_get_object_name(mzd_obj_handle), ret);
            svid_data = 90;
        } else {
            (void)dal_set_property_value_byte(mzd_obj_handle, PROPERTY_MEZZCARDOPA_MEZZSVIDREADFAIL, 0, DF_NONE);
            // 3 设置电压门限
            svid_data = (guint8)(card_vcc >> 8);
            if (svid_data == 0x01 || svid_data == 0x02 || svid_data == 0x03 || svid_data == 0x04) {
                card_vcc = (guint16)(0.85 * 100);
            } else if (svid_data == 0x05) {
                card_vcc = (guint16)(0.875 * 100);
            } else if (svid_data == 0x06) {
                card_vcc = (guint16)(0.9 * 100);
            } else if (svid_data == 0x07 || svid_data == 0x08) {
                card_vcc = (guint16)(0.925 * 100);
            } else if (svid_data == 0x09) {
                card_vcc = (guint16)(0.95 * 100);
            } else {
                debug_log(DLOG_ERROR, "%s: obj %s svid data is wrong, %d", __FUNCTION__, chip_obj_name[SVID_READ_INDEX],
                    svid_data);
                continue;
            }

            debug_log(DLOG_DEBUG, "%s: obj %s svid data %d.", __FUNCTION__, chip_obj_name[SVID_READ_INDEX], card_vcc);
            (void)dal_set_property_value_byte(mzd_obj_handle, PROPERTY_MEZZCARDOPA_MEZZVCCTHD, card_vcc, DF_NONE);
        }

        

        

        
        ret = dfl_chip_blkwrite(chip_obj_name[SVID_WRITE_INDEX], MOS_OFFSET_DATA, WRITE_DATA_LENGTH, &data_3);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "read chip %s offset:0x07 data:%x ret=%d", chip_obj_name[SVID_WRITE_INDEX], data_4,
                ret);
            continue;
        }

        
        ret = dfl_chip_blkwrite(chip_obj_name[SVID_WRITE_INDEX], WRITE_CHIP_LENGTH, WRITE_DATA_LENGTH, &svid_data);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "read chip %s offset:0x02 data:%x ret=%d", chip_obj_name[SVID_WRITE_INDEX], svid_data,
                ret);
            continue;
        }
        
        ret = dfl_chip_blkwrite(chip_obj_name[VCC_WRITE_INDEX], NULL_OFFSE_ADDR, WRITE_DATA_LENGTH, &data_4);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "write chip %s offset:0x00 data:%x ret=%d", chip_obj_name[VCC_WRITE_INDEX], data_4,
                ret);
            continue;
        }

        // 5 开启电压监控
        ret = dfl_chip_blkwrite(chip_obj_name[VCC_WRITE_INDEX], NULL_OFFSE_ADDR, WRITE_DATA_LENGTH, &data_5);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "write chip %s offset:0x00 data:%x ret=%d", chip_obj_name[VCC_WRITE_INDEX], data_5,
                ret);
            continue;
        }
    }

    g_slist_free(mezz_opa_list);

    return RET_OK;
}


LOCAL gint32 mezz_temp_for_opa_card(OBJ_HANDLE object_handle, const gchar *obj_name_swchip,
    const gchar *obj_name_srchip, const gchar *obj_name_svidchip)
{
#define ENABLE_LM9306_LENGTH 0x01 // zrch length
#define ENABLE_LM9306_OFFSET 0x06
#define ENABLE_LM9306_MOS_OFFSET 0x02
#define NULL_DATA_LENGTH 0x00
#define LM9306_OFFSET_MOS2 0x05
#define READ_DATA_LENGTH 0x02
#define READ_OFFSET_DATA 0x08
#define READ_TEMP_OFFSET_DATA 0x02
#define TEMP_OFFSET_DATA 0x03
#define ENLABLED_LM9306_DATA 0xfe
#define DISABLED_LM9306_DATA 0xff
#define INVAILD_DATA 0x01

    gint32 ret = RET_OK;
    guint16 temp_data = 0;
    guint8 data_1 = ENLABLED_LM9306_DATA;
    guint8 data_2 = DISABLED_LM9306_DATA;
    guint8 data_3 = INVAILD_DATA;
    OBJ_HANDLE temp_handle = 0;
    guint16 temp_convert = 0;

    
    (void)dfl_get_object_handle(obj_name_swchip, &temp_handle);
    (void)dfl_get_object_handle(obj_name_srchip, &temp_handle);
    (void)dfl_get_object_handle(obj_name_svidchip, &temp_handle);

    
    ret = dfl_chip_blkwrite(obj_name_swchip, ENABLE_LM9306_OFFSET, ENABLE_LM9306_LENGTH, &data_1);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "read chip %s offset:0x07 ret=%d", obj_name_swchip, ret);
        return ret;
    }

    
    ret = dfl_chip_blkwrite(obj_name_swchip, ENABLE_LM9306_MOS_OFFSET, ENABLE_LM9306_LENGTH, &data_2);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "write chip %s offset:0x02 ret=%d", obj_name_swchip, ret);
        return ret;
    }
    vos_task_delay(100);
    
    ret = dfl_chip_blkwrite(obj_name_svidchip, LM9306_OFFSET_MOS2, NULL_DATA_LENGTH, &data_3);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "write chip %s offset:0x05 ret=%d", obj_name_svidchip, ret);
        return ret;
    }
    
    ret = dfl_chip_blkread(obj_name_srchip, NULL_DATA_LENGTH, READ_DATA_LENGTH, &temp_data);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_chip_blkread %s ret=%d", obj_name_srchip, ret);
        return ret;
    }
    
    temp_convert |= (temp_data >> 8) & 0xff;
    temp_convert |= (temp_data << 8) & 0xff00;
    temp_convert = temp_convert >> READ_TEMP_OFFSET_DATA;
    temp_convert = (guint16)((temp_convert & 0x1ffff) * 0.25);
    // 取出符号位
    temp_convert |= ((temp_data & 0x10) << TEMP_OFFSET_DATA);
    (void)dal_set_property_value_uint16(object_handle, PROPERTY_MEZZCARDOPA_MEZZTEMP, temp_convert, DF_NONE);
    
    ret = dfl_chip_blkwrite(obj_name_swchip, ENABLE_LM9306_MOS_OFFSET, ENABLE_LM9306_LENGTH, &data_1);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "write chip %s offset:0x02 data:%x ret=%d", obj_name_swchip, data_1, ret);
        return ret;
    }
    return RET_OK;
}


LOCAL gint32 mezz_vcc_for_opa_card(OBJ_HANDLE object_handle, const gchar *vcc_chip_0v9, const gchar *vcc_chip_1v0,
    const gchar *vcc_chip_1v8, const gchar *vcc_chip_2v5, const gchar *vcc_chip_3v3)
{
#define READ_DATA_LENGTH 0x02
#define READ_3V3_ADDR 0x20
#define READ_0V9_ADDR 0x21
#define READ_1V0_ADDR 0x22
#define READ_1V8_ADDR 0x23
#define READ_2V5_ADDR 0x24
#define READ_OFFSET_DATA 0x08
#define VCC_OFFSET_DATA 0x06
    gint32 ret = RET_OK;
    guint16 vcc_0v9 = 0;
    guint16 vcc_1v0 = 0;
    guint16 vcc_1v8 = 0;
    guint16 vcc_2v5 = 0;
    guint16 vcc_3v3 = 0;
    guint16 vcc_data = 0;
    OBJ_HANDLE vcc_handle = 0;

    (void)dfl_get_object_handle(vcc_chip_0v9, &vcc_handle);
    (void)dfl_get_object_handle(vcc_chip_1v0, &vcc_handle);
    (void)dfl_get_object_handle(vcc_chip_1v8, &vcc_handle);
    (void)dfl_get_object_handle(vcc_chip_2v5, &vcc_handle);
    (void)dfl_get_object_handle(vcc_chip_3v3, &vcc_handle);

    
    //  1: 0.9V
    ret = dfl_chip_blkread(vcc_chip_0v9, READ_0V9_ADDR, READ_DATA_LENGTH, &vcc_data);
    if (ret != DFL_OK) {
        (void)dal_set_property_value_uint16(object_handle, PROPERTY_MEZZCARDOPA_MEZZVCC0V9, MEZZOPA_INVALID_READING,
            DF_NONE);
    }

    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "read 0v9 chip  %s offset:0x21 failed, ret=%d", vcc_chip_0v9, ret);
        return ret;
    }
    
    vcc_0v9 |= (vcc_data >> READ_OFFSET_DATA) & 0x00FF;
    vcc_0v9 |= (vcc_data << READ_OFFSET_DATA) & 0xFF00;
    vcc_0v9 = (guint16)((vcc_0v9 >> VCC_OFFSET_DATA) * 0.25);
    (void)dal_set_property_value_uint16(object_handle, PROPERTY_MEZZCARDOPA_MEZZVCC0V9, vcc_0v9, DF_NONE);

    //  2:1V0
    vcc_data = 0;
    ret = dfl_chip_blkread(vcc_chip_1v0, READ_1V0_ADDR, READ_DATA_LENGTH, &vcc_data);
    if (ret != DFL_OK) {
        (void)dal_set_property_value_uint16(object_handle, PROPERTY_MEZZCARDOPA_MEZZVCC1V0, MEZZOPA_INVALID_READING,
            DF_NONE);
    }

    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "read 1v0 chip %s offset:0x22 failed, ret=%d", vcc_chip_1v0, ret);
        return ret;
    }
    vcc_1v0 |= (vcc_data >> READ_OFFSET_DATA) & 0x00FF;
    vcc_1v0 |= (vcc_data << READ_OFFSET_DATA) & 0xFF00;
    vcc_1v0 = (guint16)((vcc_1v0 >> VCC_OFFSET_DATA) * 0.25);

    (void)dal_set_property_value_uint16(object_handle, PROPERTY_MEZZCARDOPA_MEZZVCC1V0, vcc_1v0, DF_NONE);

    //  3:1V8
    vcc_data = 0;
    ret = dfl_chip_blkread(vcc_chip_1v8, READ_1V8_ADDR, READ_DATA_LENGTH, &vcc_data);
    if (ret != DFL_OK) {
        (void)dal_set_property_value_uint16(object_handle, PROPERTY_MEZZCARDOPA_MEZZVCC1V8, MEZZOPA_INVALID_READING,
            DF_NONE);
    }

    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "read 1v8 chip %s offset:0x23 failed, ret=%d", vcc_chip_1v8, ret);
        return ret;
    }
    vcc_1v8 |= (vcc_data >> READ_OFFSET_DATA) & 0x00FF;
    vcc_1v8 |= (vcc_data << READ_OFFSET_DATA) & 0xFF00;
    vcc_1v8 = (guint16)((vcc_1v8 >> VCC_OFFSET_DATA) * 0.25);

    (void)dal_set_property_value_uint16(object_handle, PROPERTY_MEZZCARDOPA_MEZZVCC1V8, vcc_1v8, DF_NONE);

    //  4:2v5
    vcc_data = 0;
    ret = dfl_chip_blkread(vcc_chip_2v5, READ_2V5_ADDR, READ_DATA_LENGTH, &vcc_data);
    if (ret != DFL_OK) {
        (void)dal_set_property_value_uint16(object_handle, PROPERTY_MEZZCARDOPA_MEZZVCC2V5, MEZZOPA_INVALID_READING,
            DF_NONE);
    }

    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "read 2v5 chip %s offset:0x24 failed, ret=%d", vcc_chip_2v5, ret);
        return ret;
    }
    vcc_2v5 |= (vcc_data >> READ_OFFSET_DATA) & 0x00FF;
    vcc_2v5 |= (vcc_data << READ_OFFSET_DATA) & 0xFF00;
    vcc_2v5 = (guint16)((vcc_2v5 >> VCC_OFFSET_DATA) * 0.25);
    (void)dal_set_property_value_uint16(object_handle, PROPERTY_MEZZCARDOPA_MEZZVCC2V5, vcc_2v5, DF_NONE);

    //  5:3v3
    vcc_data = 0;
    ret = dfl_chip_blkread(vcc_chip_3v3, READ_3V3_ADDR, READ_DATA_LENGTH, &vcc_data);
    if (ret != DFL_OK) {
        (void)dal_set_property_value_uint16(object_handle, PROPERTY_MEZZCARDOPA_MEZZVCC3V3, MEZZOPA_INVALID_READING,
            DF_NONE);
    }

    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "read 3v3 chip %s offset:0x20 failed, ret=%d", vcc_chip_3v3, ret);
        return ret;
    }
    vcc_3v3 |= (vcc_data >> 8) & 0x00FF;
    vcc_3v3 |= (vcc_data << 8) & 0xFF00;
    vcc_3v3 = (guint16)((vcc_3v3 >> VCC_OFFSET_DATA) * 0.25);
    (void)dal_set_property_value_uint16(object_handle, PROPERTY_MEZZCARDOPA_MEZZVCC3V3, vcc_3v3, DF_NONE);
    return RET_OK;
}


LOCAL void opa_meez_card_read_temp_volt(void)
{
    errno_t safe_fun_ret = EOK;

    guint8 smbios_tatus = 0;
    GSList *mezz_opa_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE mzd_obj_handle = 0;
    guint8 obj_name_flag = 0;
    const gchar *obj_name_temp = NULL;
    OBJ_HANDLE smbios_obj_handle = 0;
    GVariantIter tmp_iter;
    GVariant *array_obj_name_gvar = NULL;
    GVariant *obj_name_gvar = NULL;
    gchar chip_obj_name[CHIP_OBJ_CNT][MAX_OBJECT_NAME_LEN];
    gint32 ret = 0;

    ret = dfl_get_object_list(CLASS_MEZZCARDOPA_NAME, &mezz_opa_list);
    if ((ret != DFL_OK) || (mezz_opa_list == NULL)) {
        debug_log(DLOG_DEBUG, "%s: Get class (%s) handle list failed", __FUNCTION__, CLASS_MEZZCARDOPA_NAME);
        return;
    }

    
    for (obj_node = mezz_opa_list; obj_node; obj_node = obj_node->next) {
        mzd_obj_handle = (OBJ_HANDLE)obj_node->data;
        ret = dfl_get_property_value(mzd_obj_handle, PROPERTY_MEZZCARDOPA_SVIDTYPECHIP, &array_obj_name_gvar);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: dfl_get_property_value failed", __FUNCTION__);
            continue;
        }
        if (array_obj_name_gvar == NULL) {
            debug_log(DLOG_DEBUG, "%s: array is null", __FUNCTION__);
            continue;
        }
        (void)g_variant_iter_init(&tmp_iter, array_obj_name_gvar);
        obj_name_flag = 0;

        
        while ((obj_name_gvar = g_variant_iter_next_value(&tmp_iter)) != NULL) {
            
            obj_name_temp = g_variant_get_string(obj_name_gvar, NULL);
            if (obj_name_temp == NULL) {
                g_variant_unref(obj_name_gvar);
                debug_log(DLOG_ERROR, "%s: g_variant_get_string failed", __FUNCTION__);
                continue;
            }
            (void)memset_s(chip_obj_name[obj_name_flag], MAX_OBJECT_NAME_LEN, 0, MAX_OBJECT_NAME_LEN);
            safe_fun_ret =
                strncpy_s(chip_obj_name[obj_name_flag], MAX_OBJECT_NAME_LEN, obj_name_temp, MAX_OBJECT_NAME_LEN - 1);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            }
            g_variant_unref(obj_name_gvar);
            obj_name_flag++;
        }

        g_variant_unref(array_obj_name_gvar);
        array_obj_name_gvar = NULL;

        
        ret = mezz_vcc_for_opa_card(mzd_obj_handle, chip_obj_name[VCC0V9_CHIP_INDEX], chip_obj_name[VCC1V0_CHIP_INDEX],
            chip_obj_name[VCC1V8_CHIP_INDEX], chip_obj_name[VCC2V5_CHIP_INDEX], chip_obj_name[VCC3V3_CHIP_INDEX]);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s: ret obj %s vcc failed, ret: %d", __FUNCTION__,
                dfl_get_object_name(mzd_obj_handle), ret);
        }
        ret = dfl_get_object_handle(CLASS_NAME_SMBIOS, &smbios_obj_handle);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: get obj handle failed, ret %d", __FUNCTION__, ret);
            continue;
        }
        ret = dal_get_property_value_byte(smbios_obj_handle, PROPERTY_SMBIOS_STATUS_VALUE, &smbios_tatus);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: smbios state is off, ret: %d", __FUNCTION__, ret);
            continue;
        }

        
        if (smbios_tatus == 3) { // zrch用宏
            ret = mezz_temp_for_opa_card(mzd_obj_handle, chip_obj_name[SVID_WRITE_INDEX],
                chip_obj_name[SVID_READ_INDEX], chip_obj_name[SVID_CHIP_INDEX]);
            
            if (ret != RET_OK) {
                (void)dal_set_property_value_uint16(mzd_obj_handle, PROPERTY_MEZZCARDOPA_MEZZTEMP,
                    MEZZOPA_INVALID_READING, DF_NONE);
            }

            if (ret != RET_OK) {
                debug_log(DLOG_DEBUG, "%s: ret obj %s temp failed, ret: %d", __FUNCTION__,
                    dfl_get_object_name(mzd_obj_handle), ret);
            }
        }
    }

    g_slist_free(mezz_opa_list);
}


LOCAL void set_opa_mezz_card_default_vlue(void)
{
    GSList *obj_node_powerff = NULL;
    OBJ_HANDLE mzd_obj_powerff_handle = 0;
    gint32 ret = 0;
    GSList *mezz_opa_list = NULL;
    ret = dfl_get_object_list(CLASS_MEZZCARDOPA_NAME, &mezz_opa_list);
    if ((ret != DFL_OK) || (mezz_opa_list == NULL)) {
        debug_log(DLOG_DEBUG, "%s: Get class (%s) handle list failed", __FUNCTION__, CLASS_MEZZCARD_NAME);
        return;
    }

    for (obj_node_powerff = mezz_opa_list; obj_node_powerff; obj_node_powerff = obj_node_powerff->next) {
        mzd_obj_powerff_handle = (OBJ_HANDLE)obj_node_powerff->data;
        (void)dal_set_property_value_uint16(mzd_obj_powerff_handle, PROPERTY_MEZZCARDOPA_MEZZTEMP,
            MONITOR_DATA_UNAVAILABLE, DF_NONE);
        (void)dal_set_property_value_uint16(mzd_obj_powerff_handle, PROPERTY_MEZZCARDOPA_MEZZVCC0V9,
            MONITOR_DATA_UNAVAILABLE, DF_NONE);
        (void)dal_set_property_value_uint16(mzd_obj_powerff_handle, PROPERTY_MEZZCARDOPA_MEZZVCC1V0,
            MONITOR_DATA_UNAVAILABLE, DF_NONE);
        (void)dal_set_property_value_uint16(mzd_obj_powerff_handle, PROPERTY_MEZZCARDOPA_MEZZVCC1V8,
            MONITOR_DATA_UNAVAILABLE, DF_NONE);
        (void)dal_set_property_value_uint16(mzd_obj_powerff_handle, PROPERTY_MEZZCARDOPA_MEZZVCC2V5,
            MONITOR_DATA_UNAVAILABLE, DF_NONE);
        (void)dal_set_property_value_uint16(mzd_obj_powerff_handle, PROPERTY_MEZZCARDOPA_MEZZVCC3V3,
            MONITOR_DATA_UNAVAILABLE, DF_NONE);
        (void)dal_set_property_value_byte(mzd_obj_powerff_handle, PROPERTY_MEZZCARDOPA_MEZZSVIDREADFAIL, 0, DF_NONE);
    }

    g_slist_free(mezz_opa_list);
    return;
}


LOCAL void opa_mezz_mgnt_task(void)
{
    gint32 ret = 0;
    OBJ_HANDLE obj_handle = 0;
    guint8 power_state = 0;
    gint32 svid_vcc_config_finish = RET_ERR;
    
    gint32 init_finish = RET_ERR;
    
    ret = dfl_get_object_handle(OBJECT_FRUPAYLOAD, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get obj handle failed, ret %d", __FUNCTION__, ret);
        return;
    }

    while (1) {
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_FRUPAYLOAD_POWERSTATE, &power_state);

        if (ALL_POWER_GOOD_OK == power_state) {
            
            if (init_finish != RET_OK) {
                (void)dfl_foreach_object(CLASS_MEZZCARD_NAME, pcie_mgnt_init_info, NULL, NULL);
                init_finish = RET_OK;
            }
            

            // 1、x86上电后需要写一次svid寄存器，进行调压
            if (svid_vcc_config_finish != RET_OK) {
                svid_vcc_config_finish = handle_mezz_svid_vcc_config();
            }

            // 2、x86上电:svid调压完后才去执行温度和电压监控
            if (svid_vcc_config_finish == RET_OK) {
                opa_meez_card_read_temp_volt();
            }
        } else {
            
            init_finish = RET_ERR;
            
            svid_vcc_config_finish = RET_ERR;
            set_opa_mezz_card_default_vlue();
            // 3、x86上电温度和电压设置为NA,默认值0x4000
        }

        
        vos_task_delay(2000);
        
        (void)dfl_foreach_object(CLASS_MEZZCARD_NAME, pcie_card_update_info, NULL, NULL);
        
    }
}


gint32 mezz_card_start(void)
{
    
    OBJ_HANDLE computer_handle = 0;
    if (dal_get_object_handle_nth(CLASS_COMPUTER_PARTITION, 0, &computer_handle) == RET_OK) {
        return RET_OK;
    }
    

    TASKID task_svid_mgnt;
    (void)vos_task_create(&task_svid_mgnt, "SVID_CHIP_MGNT", (TASK_ENTRY)opa_mezz_mgnt_task, 0, DEFAULT_PRIORITY);
    return RET_OK;
}


gint32 mezz_card_ipmi_get_pcie_width_info(const void *msg_data, gpointer user_data)
{
    gint32 ret = 0;
    guint8 resp_data[255] = {0};
    guint8 slot_id = 0;
    guint8 width_type = 0;
    guint8 width = 0;
    guint8 mezzcard_num = 0;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;
    GSList *mezz_card_obj_list = NULL;
    GSList *obj_note = NULL;

    
    if (msg_data == NULL) {
        return RET_ERR;
    }

    resp_data[1] = 0x11; // parameter revision

    ret = dfl_get_object_list(CLASS_MEZZCARD_NAME, &mezz_card_obj_list);
    if (ret != DFL_OK) {
        if (-2009 == ret) {
            resp_data[0] = COMP_CODE_SUCCESS;
            resp_data[1] = 0x11; // parameter revision
            resp_data[2] = 0;
            return ipmi_send_response(msg_data, 3, resp_data);
        }

        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    for (obj_note = mezz_card_obj_list; obj_note; obj_note = obj_note->next) {
        property_name_list = g_slist_append(property_name_list, PROPERTY_MEZZCARD_SLOT);
        property_name_list = g_slist_append(property_name_list, PROPERTY_MEZZCARD_WIDTHTYPE);

        ret = dfl_multiget_property_value((OBJ_HANDLE)obj_note->data, property_name_list, &property_value_list);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Get %s information failed!", dfl_get_object_name((OBJ_HANDLE)obj_note->data));
            g_slist_free(property_name_list);
            g_slist_free(mezz_card_obj_list);
            return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
        }

        slot_id = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, 0));
        width_type = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, 1));

        g_slist_free(property_name_list);
        g_slist_free_full(property_value_list, (GDestroyNotify)g_variant_unref);
        property_name_list = NULL;
        property_value_list = NULL;

        if (width_type == 0) {
            
            width = 3;
        } else if (width_type == 1) {
            
            width = 4;
        } else if (width_type == 2) {
            
            width = 0;
        }

        
        resp_data[3 + mezzcard_num * 2] = slot_id - 1;
        resp_data[4 + mezzcard_num * 2] = width;
        mezzcard_num++;
    }

    g_slist_free(mezz_card_obj_list);

    resp_data[0] = COMP_CODE_SUCCESS; // parameter revision
    resp_data[1] = 0x11;              // parameter revision
    resp_data[2] = mezzcard_num;
    return ipmi_send_response(msg_data, (3 + mezzcard_num * 2), resp_data);
}


gint32 mezz_set_mezz_mac_addr(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret;
    gsize mac_len = 0;
    GVariant *property_value = NULL;
    gchar **mac_vector = NULL;
    gchar *mac_addr_str = NULL;
    gsize mac_addr_num = 0;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s: input_list is null", __FUNCTION__);
        return RET_ERR;
    }

    
    guint8 mac_id = (guint8)g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    if (mac_id < 1 || mac_id > MAX_MAC_NUM) {
        debug_log(DLOG_ERROR, "mezz_set_mezz_mac_addr failed, mac_id = %d", mac_id);
        return RET_ERR;
    }
    
    guint8 mac_index = mac_id - 1;

    
    const gchar *mac_addr = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 1), &mac_len);
    
    if ((mac_addr == NULL) || (mac_len > MAX_MAC_STR_LEN)) {
        debug_log(DLOG_ERROR, "mezz_set_mezz_mac_addr failed, mac_len = %" G_GSIZE_FORMAT, mac_len);
        return RET_ERR;
    }

    // MAC地址统一大写
    gchar *mac_upper = g_ascii_strup(mac_addr, -1);
    if (mac_upper == NULL) {
        ret = RET_ERR;
        goto EXIT_FUNC;
    }

    mac_addr_str = (gchar *)g_malloc0(mac_len + 1); // 不需要单独释放
    if (mac_addr_str == NULL) {
        debug_log(DLOG_ERROR, "mezz_set_mezz_mac_addr failed, mac_len = %" G_GSIZE_FORMAT, mac_len + 1);
        ret = RET_ERR;
        goto EXIT_FUNC;
    }

    
    safe_fun_ret = strncpy_s(mac_addr_str, mac_len + 1, mac_upper, mac_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    

    
    ret = dfl_get_property_value(object_handle, PROPERTY_MEZZCARD_MAC_ADDR, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_property_value failed!(ret: %d).", ret);
        goto EXIT_FUNC;
    }

    mac_vector = g_variant_dup_strv(property_value, &mac_addr_num);
    g_variant_unref(property_value);

    
    if (mac_vector == NULL) {
        ret = RET_ERR;
        debug_log(DLOG_ERROR, "g_variant_dup_strv fail , mac_addr_num = %" G_GSIZE_FORMAT "(ret: %d).", mac_addr_num,
            ret);
        goto EXIT_FUNC;
    }

    if (mac_vector[mac_index] != NULL) {
        debug_log(DLOG_INFO, "come here , mac_addr_num = %" G_GSIZE_FORMAT ", mac_addr_str = %s", mac_addr_num,
            mac_addr_str);
        g_free(mac_vector[mac_index]);
        mac_vector[mac_index] = mac_addr_str;
        mac_addr_str = NULL;
    }

    
    
    ret = dal_set_property_value_array_string(object_handle, PROPERTY_MEZZCARD_MAC_ADDR,
        (const gchar * const *)mac_vector, mac_addr_num, DF_NONE);
    
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "dfl_set_property_value failed, ret = %d", ret);
        goto EXIT_FUNC;
    }

EXIT_FUNC:
    if (ret == RET_OK) {
        method_operation_log(caller_info, CLASS_MEZZCARD_NAME, "Set Mezz Card Mac %d Address to %s successfully",
            mac_id, mac_upper);
    } else {
        method_operation_log(caller_info, CLASS_MEZZCARD_NAME, "Set Mezz Card Mac %d Address to %s failed", mac_id,
            mac_upper);
    }
    g_strfreev(mac_vector);
    g_free(mac_addr_str);
    g_free(mac_upper);

    return ret;
}


LOCAL gint32 mezz_card_recover_device_status(OBJ_HANDLE obj_handle, gpointer user_data)
{
    gint32 ret = 0;
    guint8 default_value = *(guint32 *)user_data;
    GVariant *property_data = NULL;

    property_data = g_variant_new_byte(default_value);

    ret = dfl_set_property_value(obj_handle, PROPERTY_FDM_FAULT_STATE, property_data, DF_SAVE_TEMPORARY);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set MEZZ Card FDM fault state default value failed: %d.", ret);
    }

    g_variant_unref(property_data);

    
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_PFA_EVENT, default_value, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set PFAEvent default value failed: %d.", ret);
    }
    

    return RET_OK;
}


void mezz_card_set_device_status_default_value(void)
{
    gint32 ret = 0;
    guint8 default_state = 0;

    ret = dfl_foreach_object(CLASS_MEZZCARD_NAME, mezz_card_recover_device_status, &default_state, NULL);
    if (ret != DFL_OK) {
        if (ERRCODE_OBJECT_NOT_EXIST != ret) {
            debug_log(DLOG_ERROR, "Recover raid default value failed!");
        }
    }

    return;
}


gint32 mezz_card_ipmi_set_health_status(const void *msg_data, gpointer user_data)
{
    gint32 ret = 0;
    const guint8 *req_data = NULL;
    guint8 resp_data[255] = {0};
    guint32 manufactureid = 0;
    guint32 imana = 0;
    guint8 event_dir = 0;
    
    guint8 device_status0 = 0;
    guint8 device_status1 = 0;
    GVariant *property_value = NULL;
    OBJ_HANDLE obj_handle = 0;
    guint8 complete_code = 0xFF;

    
    if (msg_data == NULL) {
        return RET_ERR;
    }

    
    req_data = get_ipmi_src_data(msg_data);
    
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "get ipmi src data fail!");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }
    

    
    ret = dfl_get_object_handle(BMC_OBJECT_NAME, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Failed to get BMC object %d.", ret);
        complete_code = COMP_CODE_DES_UNAVAILABLE;
        goto out;
    }

    ret = dfl_get_property_value(obj_handle, BMC_MANU_ID_NAME, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Failed to get manufacture ID.");
        complete_code = COMP_CODE_DES_UNAVAILABLE;
        goto out;
    }

    manufactureid = g_variant_get_uint32(property_value);
    g_variant_unref(property_value);
    property_value = NULL;

    
    imana = MAKE_DWORD(0, req_data[2], req_data[1], req_data[0]);
    if (manufactureid != imana) {
        debug_log(DLOG_ERROR, "Manufacture ID didn't match.");
        complete_code = COMP_CODE_INVALID_FIELD;
        goto out;
    }

    event_dir = !((req_data[5] & 0x80) >> 7);
    device_status0 = req_data[6];
    device_status1 = req_data[7];

    switch (device_status0) {
        case MEZZCARD_HW_HEALTH_STATUS:
            debug_log(DLOG_DEBUG, "mezz_card_ipmi_set_health_status(%d) %s, slotid(%d)", device_status0,
                (event_dir == 1) ? "Asserted" : "Deasserted", device_status1);

            ret = dal_get_specific_object_byte(CLASS_CARD_NAME, PROPERTY_CARD_SLOT, device_status1, &obj_handle);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "Get object Card failed,slotid:%d, ret:%d.", device_status1, ret);
                complete_code = COMP_CODE_DES_UNAVAILABLE;
                break;
            }

            ret = dal_set_property_value_byte(obj_handle, PROPERTY_CARD_HEALTH, event_dir, DF_SAVE_TEMPORARY);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "Set property Card(slotid:%d) health state failed, ret:%d.", device_status1,
                    ret);
                complete_code = COMP_CODE_DES_UNAVAILABLE;
                break;
            }

            complete_code = COMP_CODE_SUCCESS;
            break;

        default:
            debug_log(DLOG_ERROR, "Set property Card health state failed, device_status[0]:%d.", device_status0);
            complete_code = COMP_CODE_DES_UNAVAILABLE;
            break;
    }

out:
    resp_data[0] = complete_code;
    resp_data[1] = LONGB0(manufactureid);
    resp_data[2] = LONGB1(manufactureid);
    resp_data[3] = LONGB2(manufactureid);
    (void)ipmi_send_response(msg_data, 4, resp_data);

    if (COMP_CODE_SUCCESS == resp_data[0]) {
        ipmi_operation_log(msg_data, "Set mezz card status successfully");
        return RET_OK;
    } else {
        ipmi_operation_log(msg_data, "Set mezz card status failed");
        return RET_ERR;
    }
}
