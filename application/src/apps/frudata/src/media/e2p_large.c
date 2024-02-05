

#include <math.h>
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "pme_app/dalib/dal_obj_priv_data.h"
#include "media/e2p_large.h"
#include "common.h"
#include "protocol.h"

#define EEPROM_PAGE_SIZE 256 // 每页256字节
#define EEPROM_PAGE_COUNT 4  // 1k背板，分为4页
#define EEPROM_BLOCK_SIZE 16 // 一次只能写入一个块，不能跨块写入
#define CHASSIS_FRU_COUNT 3
#define CHASSIS_PART_NUMBER_OFFSET 0x04
#define CHASSIS_PART_NUMBER_BLOCK_SIZE 28
#define CHASSIS_SERIAL_NUMBER_OFFSET 0x20
#define CHASSIS_SERIAL_NUMBER_BLOCK_SIZE 36
#define CHASSIS_TYPE_OFFSET 0x84
#define CHASSIS_TYPE_LEN 32
#define CHASSIS_TYPE_BLOCK_SIZE 36
#define BACK_PLANE_LABEL_OFFSET 0x200
#define MAX_BACK_PLANE_LABEL_LEN 64
#define BACK_PLANE_LABEL_LEN 512
#define BACK_PLANE_TYPE_KEY "BoardType"
#define BACK_PLANE_SERIAL_NUMBER_KEY "BarCode"
#define BACK_PLANE_ITEM_KEY "Item"
#define BACK_PLANE_DESCRIPTION_KEY "Description"
#define BACK_PLANE_MANUFACTURED_DATE_KEY "Manufactured"
#define BACK_PLANE_VENDOR_NAME_KEY "VendorName"
#define BACK_PLANE_ISSUE_NUMBER_KEY "IssueNumber"
#define BACK_PLANE_CLEI_CODE_KEY "CLEICode"
#define BACK_PLANE_BOM_KEY "BOM"
#define BACK_PLANE_CHASSIS_TYPE "ChassisType"
#define WRITE_PROTECT 1 // 写保护寄存器 0x69， 1写保护，0关闭写保护
#define NO_WRITE_PROTECT 0
#define FRU_INFO_VALID 1
#define HIBYTE(a) ((guint8)((a) >> 8))
#define LOBYTE(a) ((guint8)(a))
#define MAX_TRY_TIMES 5 // 最大写入尝试次数

struct chassis_info { // 机框信息的存储格式
    guint8 crc_high;  // CRC高位
    guint8 crc_low;   // CRC低位
    guint8 flag;      // 是否有效标志，标识数据域是否有效，1：有效，其它：无效
    guint8 data[1];   // 数据指针
};

#define CHASSIS_INFO_HEADER_SIZE offsetof(struct chassis_info, data)

LOCAL gint32 __init_back_plane(void);
LOCAL gint32 __eeprom_block_read(OBJ_HANDLE dev_handle, gint32 offset, gsize length, gpointer out_data);
LOCAL gint32 __read_info_from_raw_eeprom(OBJ_HANDLE dev_handle, gpointer buffer, gsize buffer_length, gint32 offset,
    gsize block_size);
LOCAL gint32 init_chassis_info_from_back_plane(FRU_PROPERTY_S *fru_prop);
LOCAL gint32 __init_chassis_fru_info_from_raw_eeprom(FRU_PROPERTY_S *fru_prop, FRU_INFO_S *fru_info);
LOCAL gchar *__get_back_board_label(FRU_PROPERTY_S *fru_prop);
LOCAL gint32 __read_chassis_type(FRU_PROPERTY_S *fru_prop, gchar *chassis_type, guint32 length);
LOCAL void __read_info_from_elabel(const gchar *back_board_label, gchar *key, gchar *buffer, gint32 buffer_length);
LOCAL void __init_backplane_fru_serial_number(const gchar *back_board_label, FRU_INFO_S *fru_info);
LOCAL void __init_backplane_fru_item(const gchar *back_board_label, FRU_INFO_S *fru_info);
LOCAL void __init_backplane_fru_manufacture_time(const gchar *back_board_label, FRU_INFO_S *fru_info);
LOCAL void __init_backplane_fru_product_name(const gchar *back_board_label, FRU_INFO_S *fru_info);
LOCAL void __init_backplane_fru_manufacture(const gchar *back_board_label, FRU_INFO_S *fru_info);
LOCAL gint32 __init_backplane_fru_info_from_raw_eeprom(FRU_PROPERTY_S *fru_prop, FRU_INFO_S *fru_info);
LOCAL gint32 __eeprom_block_write(OBJ_HANDLE dev_handle, gint32 offset, gsize length, gpointer in_data);
LOCAL void __clear_chassis_fru_in_raw_eeprom_task(gpointer data);
LOCAL gint32 init_fru_from_back_plane(FRU_PROPERTY_S *fru_prop, FRU_INFO_S *fru_info);
LOCAL void __init_write_eeprom_buffer(const void *data, gsize data_length, gsize defined_length, gsize block_size,
    gchar *buffer);

LOCAL gulong g_eeprom_write_lock; // 保证eeprom只有一个线程写入

G_LOCK_DEFINE(g_lock_write_to_eeprom);
G_LOCK_DEFINE(g_lock_write_double_eeprom);


LOCAL gint32 __init_back_plane(void)
{
    
    gint32 ret = vos_thread_mutex_sem4_create(&g_eeprom_write_lock, "BackPlane");
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Create eeprom wp lock failed, err code %d", ret);
    }
    return ret;
}


LOCAL gint32 __eeprom_block_read(OBJ_HANDLE dev_handle, gint32 offset, gsize length, gpointer out_data)
{
    // 计算写入的页面，页面大小256
    guint32 page = offset / EEPROM_PAGE_SIZE;
    if (page >= EEPROM_PAGE_COUNT) {
        debug_log(DLOG_ERROR, "Offset %d is not right", offset);
        return RET_ERR;
    }

    // 获取对应页面的写入对象
    OBJ_HANDLE page_handle = 0;
    gint32 ret = dal_get_refobject_handle_nth(dev_handle, PROPERTY_RAW_EEPROM_PAGES, page, &page_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get reference object %s failed, nth, page %u, err code %d", PROPERTY_RAW_EEPROM_PAGES,
            page, ret);
        return RET_ERR;
    }

    const gchar *chip_name = dfl_get_object_name(page_handle);
    if (chip_name == NULL) {
        debug_log(DLOG_ERROR, "Get object %s name failed", PROPERTY_RAW_EEPROM_PAGES);
        return RET_ERR;
    }

    // 写入eeprom
    for (gint32 i = 0; i < MAX_TRY_TIMES; i++) {
        ret = dfl_chip_blkread(chip_name, offset - page * EEPROM_PAGE_SIZE, length, out_data);
        if (ret == DFL_OK) {
            break;
        }
        vos_task_delay(100);
    }
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Read %s faile, err code %d", chip_name, ret);
    }
    return ret;
}


LOCAL gint32 __read_info_from_raw_eeprom(OBJ_HANDLE dev_handle, gpointer buffer, gsize buffer_length, gint32 offset,
    gsize block_size)
{
    guint8 *info = (guint8 *)g_malloc0(block_size);
    if (info == NULL) {
        debug_log(DLOG_ERROR, "[%s] Malloc failed, size %" G_GSIZE_FORMAT, __FUNCTION__, block_size);
        return RET_ERR;
    }

    // 从eeprom读取数据
    gint32 ret = __eeprom_block_read(dev_handle, offset, block_size, info);
    if (ret != RET_OK) {
        g_free(info);
        debug_log(DLOG_ERROR, "[%s] Read eeprom failed, err code %d", __FUNCTION__, ret);
        return ret;
    }

    // 检验数据
    struct chassis_info *chassis_fru_info_ptr = (struct chassis_info *)info;
    if (chassis_fru_info_ptr->flag != FRU_INFO_VALID) { // 数据有效
        g_free(info);
        debug_log(DLOG_ERROR, "[%s] read info from e2p failed, Crc checksum is not right", __func__);
        return RET_ERR;
    }

    
    guint16 crc_checksum =
        arith_make_crc_checksum(0, &(chassis_fru_info_ptr->flag), buffer_length + sizeof(chassis_fru_info_ptr->flag));
    if (HIBYTE(crc_checksum) == chassis_fru_info_ptr->crc_high &&
        LOBYTE(crc_checksum) == chassis_fru_info_ptr->crc_low) { // 校验数据
        errno_t safe_fun_ret = memcpy_s(buffer, buffer_length, chassis_fru_info_ptr->data, buffer_length);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        ret = RET_OK;
    } else {
        debug_log(DLOG_ERROR, "[%s] read info from e2p failed, Crc checksum is not right", __func__);
        ret = RET_ERR;
    }
    g_free(info);
    return ret;
}

LOCAL void init_chassis_location_from_back_plane(FRU_PROPERTY_S *fru_prop, OBJ_HANDLE chassis_obj)
{
    
    gchar location[CHASSIS_LOCATION_LEN + 1] = { 0 }; // 机框位置
    gint32 ret = __read_info_from_raw_eeprom(fru_prop->frudev_handle, location, CHASSIS_LOCATION_LEN,
        CHASSIS_LOCATION_OFFSET, CHASSIS_LOCATION_BLOCK_SIZE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Read chassis location from eeprom failed, err code %d", ret);
        return;
    }

    GSList *input_list = g_slist_append(NULL, (gpointer)g_variant_new_string(location));
    input_list = g_slist_append(input_list, g_variant_new_byte(0));
    input_list = g_slist_append(input_list, g_variant_new_byte(0));
    ret = dfl_call_class_method(chassis_obj, METHOD_CHASSIS_SET_CHASSIS_LOCATION, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call (%s) fail, ret:%d.", __FUNCTION__, METHOD_CHASSIS_SET_CHASSIS_LOCATION, ret);
    }
}

LOCAL void init_chassis_num_from_back_plane(FRU_PROPERTY_S *fru_prop, OBJ_HANDLE chassis_obj)
{
    
    guint32 id = 0; // 机框号
    gint32 ret = __read_info_from_raw_eeprom(fru_prop->frudev_handle, &id, CHASSIS_ID_LEN, CHASSIS_ID_OFFSET,
        CHASSIS_ID_BLOCK_SIZE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Read chassis id from eeprom failed, err code %d", ret);
        id = 1000000; // Set 1000000 when error occurred
    }

    // MM920为小端，而MM910数据存储为大端，为保持兼容进行读取转换
    GSList *input_list = g_slist_append(NULL, g_variant_new_uint32(g_ntohl(id)));
    input_list = g_slist_append(input_list, g_variant_new_byte(1));
    ret = dfl_call_class_method(chassis_obj, METHOD_CHASSIS_SET_CHASSIS_NUMBER, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call (%s) fail, ret:%d.", __FUNCTION__, METHOD_CHASSIS_SET_CHASSIS_NUMBER, ret);
    }
}
LOCAL void init_chassis_name_from_back_plane(FRU_PROPERTY_S *fru_prop, OBJ_HANDLE chassis_obj)
{
    
    gchar name[CHASSIS_NAME_LEN + 1] = { 0 }; // 机框名
    gint32 ret = __read_info_from_raw_eeprom(fru_prop->frudev_handle, name, CHASSIS_NAME_LEN, CHASSIS_NAME_OFFSET,
        CHASSIS_NAME_BLOCK_SIZE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Read chassis name from eeprom failed, err code %d", ret);
        return;
    }

    GSList *input_list = g_slist_append(NULL, (gpointer)g_variant_new_string(name));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(1));
    ret = dfl_call_class_method(chassis_obj, METHOD_CHASSIS_SET_CHASSIS_NAME, NULL, input_list, NULL);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call (%s) fail, ret:%d.", __FUNCTION__, METHOD_CHASSIS_SET_CHASSIS_NAME, ret);
    }
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
}

LOCAL gint32 init_chassis_info_from_back_plane(FRU_PROPERTY_S *fru_prop)
{
    
    OBJ_HANDLE chassis_handle = 0; // 机框对象句柄
    gint32 ret = dfl_get_object_handle(OBJECT_NAME_CHASSIS, &chassis_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get chassis handle failed, err code %d", ret);
        return RET_ERR;
    }
    init_chassis_location_from_back_plane(fru_prop, chassis_handle);
    init_chassis_num_from_back_plane(fru_prop, chassis_handle);
    init_chassis_name_from_back_plane(fru_prop, chassis_handle);
    return RET_OK;
}


LOCAL gint32 __init_chassis_fru_info_from_raw_eeprom(FRU_PROPERTY_S *fru_prop, FRU_INFO_S *fru_info)
{
    
    gchar chassis_part_number[CHASSIS_PART_NUMBER_LEN + 1] = { 0 };
    gint32 ret = __read_info_from_raw_eeprom(fru_prop->frudev_handle, chassis_part_number, CHASSIS_PART_NUMBER_LEN,
        CHASSIS_PART_NUMBER_OFFSET, CHASSIS_PART_NUMBER_BLOCK_SIZE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Read chassis part number failed, err code %d", ret);
    }

    
    gchar chassis_serial_number[CHASSIS_SERIAL_NUMBER_LEN + 1] = { 0 };
    ret = __read_info_from_raw_eeprom(fru_prop->frudev_handle, chassis_serial_number, CHASSIS_SERIAL_NUMBER_LEN,
        CHASSIS_SERIAL_NUMBER_OFFSET, CHASSIS_SERIAL_NUMBER_BLOCK_SIZE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Read chassis serial number failed, err code %d", ret);
    }

    
    fru_info->has_chassis = TRUE;
    fru_info->chassis.area_ver = 0x01;
    
    if (strlen(chassis_part_number) != 0) {
        fru_info->chassis.chassis_part_num.type = CHAR_TYPE;
        fru_info->chassis.chassis_part_num.len = (guint8)strlen(chassis_part_number);
        (void)strncpy_s((gchar *)(fru_info->chassis.chassis_part_num.content), FRU_MAX_ELABEL_LEN, chassis_part_number,
            FRU_MAX_ELABEL_LEN - 1);
    }
    
    if (strlen(chassis_serial_number) != 0) {
        fru_info->chassis.chassis_serial_num.type = CHAR_TYPE;
        fru_info->chassis.chassis_serial_num.len = (guint8)strlen(chassis_serial_number);
        (void)strncpy_s((gchar *)(fru_info->chassis.chassis_serial_num.content), FRU_MAX_ELABEL_LEN,
            chassis_serial_number, FRU_MAX_ELABEL_LEN - 1);
    }

    fru_info->chassis.chassis_type = 0x01; 

    return RET_OK;
}


LOCAL gchar *__get_back_board_label(FRU_PROPERTY_S *fru_prop)
{
    gchar *back_board_label = (gchar *)g_malloc0(BACK_PLANE_LABEL_LEN + 1);
    if (back_board_label == NULL) {
        debug_log(DLOG_ERROR, "Malloc failed, size %d", BACK_PLANE_LABEL_LEN + 1);
        return NULL;
    }

    gint32 ret =
        __eeprom_block_read(fru_prop->frudev_handle, BACK_PLANE_LABEL_OFFSET, EEPROM_PAGE_SIZE, back_board_label);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Read back board serial number failed, err code %d", ret);
        g_free(back_board_label);
        return NULL;
    }

    ret = __eeprom_block_read(fru_prop->frudev_handle, BACK_PLANE_LABEL_OFFSET + EEPROM_PAGE_SIZE, EEPROM_PAGE_SIZE,
        back_board_label + EEPROM_PAGE_SIZE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Read back board serial number failed, err code %d", ret);
        g_free(back_board_label);
        return NULL;
    }
    return back_board_label;
}


LOCAL gint32 __read_chassis_type(FRU_PROPERTY_S *fru_prop, gchar *chassis_type, guint32 length)
{
    
    gint32 ret = __read_info_from_raw_eeprom(fru_prop->frudev_handle, chassis_type, length, CHASSIS_TYPE_OFFSET,
        CHASSIS_TYPE_BLOCK_SIZE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Read chassis type failed, err code %d", ret);
        (void)memset_s(chassis_type, length, 0, length);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL void __read_info_from_elabel(const gchar *back_board_label, gchar *key, gchar *buffer, gint32 buffer_length)
{
    // 寻找标签起始位置
    gchar *start_pos = strstr(back_board_label, key);
    if (start_pos == NULL) {
        debug_log(DLOG_ERROR, "Can not find key %s", key);
        return;
    }

    start_pos = strstr(start_pos, "=");
    if (start_pos == NULL) {
        debug_log(DLOG_ERROR, "Read %s failed, can not find '='", key);
        return;
    }

    gchar *end_pos = strstr(start_pos, "\r\n");
    start_pos++; // 指向 '=' 下一个字符
    if (end_pos == NULL) {
        debug_log(DLOG_ERROR, "Read %s failed, can not find '\r\n'", key);
        return;
    }

    gint32 data_length = end_pos - start_pos;
    if (data_length <= 0 || data_length >= buffer_length) {
        return;
    }

    (void)memset_s(buffer, buffer_length, 0, buffer_length);
    errno_t safe_fun_ret = memcpy_s(buffer, buffer_length, start_pos, data_length);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    return;
}


guint32 __calc_time_interval(const gchar *sztime)
{
    gint32 date[3] = { 0 };
    gint32 i = 0;
    gint32 j = 0;
    while (sztime[j] != '\0') {
        if (sztime[j] == '-') { // value end
            i++;
        } else if (sztime[j] <= '9' && sztime[j] >= '0') {
            date[i] = date[i] * 10 + sztime[j] - '0';
        } else { // Format not right
            return 0;
        }
        j++;
    }

    
    GDateTime *manufactured_time = g_date_time_new_local(date[0], date[1], date[2], 0, 0, 0);

    
    GDateTime *base_time = g_date_time_new_local(1996, 1, 1, 0, 0, 0);

    
    GTimeSpan time_interval = g_date_time_difference(manufactured_time, base_time);
    time_interval = time_interval / 1000 / 1000 / 60; // 转换为分钟数
    g_date_time_unref(manufactured_time);
    g_date_time_unref(base_time);
    return (guint32)time_interval;
}


void __add_extension_label(GList **extension_label, gchar *key, const gchar *data)
{
    gint32 data_length = strlen(key) + 1 + strlen(data);
    gchar *buffer = (gchar *)g_malloc0(data_length + 1);
    if (buffer == NULL) {
        debug_log(DLOG_ERROR, "Malloc failed, size %d", data_length + 1);
        return;
    }

    
    gint32 ret = snprintf_s(buffer, data_length + 1, data_length, "%s=%s", key, data);
    if (ret <= 0) {
        g_free(buffer);
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret);
        return;
    }
    gchar *cur_pos = buffer;
    
    while (data_length > 0) {
        gint32 length_to_write;
        if (data_length > FRU_MAX_ELABEL_LEN) {
            data_length = data_length - FRU_MAX_ELABEL_LEN;
            length_to_write = FRU_MAX_ELABEL_LEN;
        } else {
            length_to_write = data_length;
            data_length = 0;
        }
        FRU_ELABEL_FORMAT_S *elabel = (FRU_ELABEL_FORMAT_S *)g_malloc0(sizeof(FRU_ELABEL_FORMAT_S));
        if (elabel == NULL) {
            debug_log(DLOG_ERROR, "Malloc failed, size %" G_GSIZE_FORMAT, sizeof(FRU_ELABEL_FORMAT_S));
            break;
        }
        elabel->type = CHAR_TYPE;
        elabel->len = length_to_write;
        errno_t safe_fun_ret = memcpy_s(elabel->content, FRU_MAX_ELABEL_LEN, cur_pos, length_to_write);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        cur_pos = cur_pos + length_to_write;
        
        *extension_label = g_list_append(*extension_label, (gpointer)elabel);
    }
    g_free(buffer);
    return;
}

LOCAL void __init_backplane_fru_serial_number(const gchar *back_board_label, FRU_INFO_S *fru_info)
{
    // 背板序列号
    gchar serial_number[MAX_BACK_PLANE_LABEL_LEN] = { 0 };
    __read_info_from_elabel(back_board_label, BACK_PLANE_SERIAL_NUMBER_KEY, serial_number, MAX_BACK_PLANE_LABEL_LEN);
    gint32 serial_number_len = strlen(serial_number);
    if (serial_number_len <= 0 || serial_number_len > FRU_MAX_ELABEL_LEN) {
        return;
    }

    fru_info->board.board_serial_num.type = CHAR_TYPE;
    fru_info->board.board_serial_num.len = (guint8)strlen(serial_number);
    // 上面已经判断过缓冲区大小，这边无需判断返回值
    gint32 ret =
        memcpy_s(fru_info->board.board_serial_num.content, FRU_MAX_ELABEL_LEN, serial_number, serial_number_len);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, ret);
    }
}

LOCAL void __init_backplane_fru_item(const gchar *back_board_label, FRU_INFO_S *fru_info)
{
    // 背板BBOM编码
    gchar item[MAX_BACK_PLANE_LABEL_LEN] = { 0 };
    __read_info_from_elabel(back_board_label, BACK_PLANE_ITEM_KEY, item, MAX_BACK_PLANE_LABEL_LEN);
    gint32 item_len = strlen(item);
    if (item_len <= 0 || item_len > FRU_MAX_ELABEL_LEN) {
        return;
    }

    fru_info->board.board_part_num.type = CHAR_TYPE;
    fru_info->board.board_part_num.len = item_len;
    // 上面已经判断过缓冲区大小，这边无需判断返回值
    gint32 ret = memcpy_s(fru_info->board.board_part_num.content, FRU_MAX_ELABEL_LEN, item, item_len);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, ret);
    }
}

LOCAL void __init_backplane_fru_manufacture_time(const gchar *back_board_label, FRU_INFO_S *fru_info)
{
    // 背板生产日期
    gchar manufactured[MAX_BACK_PLANE_LABEL_LEN] = { 0 };
    __read_info_from_elabel(back_board_label, BACK_PLANE_MANUFACTURED_DATE_KEY, manufactured, MAX_BACK_PLANE_LABEL_LEN);
    if (strlen(manufactured) == 0) {
        return;
    }

    guint32 manuftime = __calc_time_interval(manufactured);
    fru_info->board.mfg_date_time[0] = (guint8)(manuftime & 0xFF);
    fru_info->board.mfg_date_time[1] = (guint8)((manuftime >> 8) & 0xFF);
    fru_info->board.mfg_date_time[2] = (guint8)((manuftime >> 16) & 0xFF);

    OBJ_HANDLE obj_handle = 0;
    guint8 elabel_format = 0;
    (void)dfl_get_object_handle(OBJECT_BMC, &obj_handle);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_BMC_ELABELMANUFORMAT, &elabel_format);
    if (elabel_format == 1) {
        fru_info->board.mfg_date_time[3] = (guint8)((manuftime >> 24) & 0xFF); // 新格式，data[3]要移位24
    }
}

LOCAL void __init_backplane_fru_product_name(const gchar *back_board_label, FRU_INFO_S *fru_info)
{
    // 背板类型
    gchar board_type[MAX_BACK_PLANE_LABEL_LEN] = { 0 };
    __read_info_from_elabel(back_board_label, BACK_PLANE_TYPE_KEY, board_type, MAX_BACK_PLANE_LABEL_LEN);
    gint32 board_type_len = strlen(board_type);
    if (board_type_len <= 0 || board_type_len > FRU_MAX_ELABEL_LEN) {
        return;
    }

    fru_info->board.board_product_name.type = CHAR_TYPE;
    fru_info->board.board_product_name.len = (guint8)strlen(board_type);
    gint32 ret = memcpy_s(fru_info->board.board_product_name.content, FRU_MAX_ELABEL_LEN, board_type, board_type_len);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, ret);
    }
}

LOCAL void __init_backplane_fru_manufacture(const gchar *back_board_label, FRU_INFO_S *fru_info)
{
    // 背板制造商
    gchar vendor_name[MAX_BACK_PLANE_LABEL_LEN] = { 0 };
    __read_info_from_elabel(back_board_label, BACK_PLANE_VENDOR_NAME_KEY, vendor_name, MAX_BACK_PLANE_LABEL_LEN);
    gint32 verdor_name_len = strlen(vendor_name);
    if (verdor_name_len <= 0 || verdor_name_len >= FRU_MAX_ELABEL_LEN) {
        return;
    }

    fru_info->board.board_manufacturer.type = CHAR_TYPE;
    fru_info->board.board_manufacturer.len = (guint8)strlen(vendor_name);
    gint32 ret = memcpy_s(fru_info->board.board_manufacturer.content, FRU_MAX_ELABEL_LEN, vendor_name, verdor_name_len);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, ret);
    }
}


LOCAL gint32 __init_backplane_fru_info_from_raw_eeprom(FRU_PROPERTY_S *fru_prop, FRU_INFO_S *fru_info)
{
    
    
    gchar *back_board_label = __get_back_board_label(fru_prop);
    if (back_board_label == NULL) {
        debug_log(DLOG_ERROR, "[%s] get back board label failed.", __FUNCTION__);
        return RET_ERR;
    }

    
    fru_info->has_board = TRUE;
    fru_info->board.area_ver = 0x01;

    
    __init_backplane_fru_serial_number(back_board_label, fru_info);

    
    __init_backplane_fru_item(back_board_label, fru_info);

    
    __init_backplane_fru_manufacture_time(back_board_label, fru_info);

    
    __init_backplane_fru_product_name(back_board_label, fru_info);

    
    __init_backplane_fru_manufacture(back_board_label, fru_info);

    
    gchar description[MAX_BACK_PLANE_LABEL_LEN] = { 0 }; // 背板描述
    __read_info_from_elabel(back_board_label, BACK_PLANE_DESCRIPTION_KEY, description, MAX_BACK_PLANE_LABEL_LEN);
    if (strlen(description) != 0) {
        __add_extension_label(&(fru_info->board.extension_label), BACK_PLANE_DESCRIPTION_KEY, description);
    }

    gchar issue_number[MAX_BACK_PLANE_LABEL_LEN] = { 0 }; // 发行号
    __read_info_from_elabel(back_board_label, BACK_PLANE_ISSUE_NUMBER_KEY, issue_number, MAX_BACK_PLANE_LABEL_LEN);
    if (strlen(issue_number) != 0) {
        __add_extension_label(&(fru_info->board.extension_label), BACK_PLANE_ISSUE_NUMBER_KEY, issue_number);
    }

    gchar clei_code[MAX_BACK_PLANE_LABEL_LEN] = { 0 }; // CLEI码
    __read_info_from_elabel(back_board_label, BACK_PLANE_CLEI_CODE_KEY, clei_code, MAX_BACK_PLANE_LABEL_LEN);
    if (strlen(clei_code) != 0) {
        __add_extension_label(&(fru_info->board.extension_label), BACK_PLANE_CLEI_CODE_KEY, clei_code);
    }

    gchar bom[MAX_BACK_PLANE_LABEL_LEN] = { 0 }; // BOM编码
    __read_info_from_elabel(back_board_label, BACK_PLANE_BOM_KEY, bom, MAX_BACK_PLANE_LABEL_LEN);
    if (strlen(bom) != 0) {
        __add_extension_label(&(fru_info->board.extension_label), BACK_PLANE_BOM_KEY, bom);
    }

    
    gchar chassis_type[CHASSIS_TYPE_LEN + 1] = {0};
    (void)__read_chassis_type(fru_prop, chassis_type, CHASSIS_TYPE_LEN);
    if (strlen(chassis_type) != 0) {
        __add_extension_label(&(fru_info->board.extension_label), BACK_PLANE_CHASSIS_TYPE, chassis_type);
    }

    g_free(back_board_label);
    return RET_OK;
}

typedef struct {
    gint32 page_size;
    gint32 block_size;
    gint32 start_pos;
    gint32 end_pos;
    const gchar *chip_name;
} EEPROM_WRITE_PARA_S;


LOCAL gint32 __eeprom_block_write_precheck(OBJ_HANDLE dev_handle, gint32 offset, gsize length, gpointer in_data,
    EEPROM_WRITE_PARA_S *write_para)
{
    // 检查输入参数
    gchar *data_pointer = (gchar *)in_data;
    if (data_pointer == NULL) {
        debug_log(DLOG_ERROR, "[%s] Write data failed, data is null", __func__);
        return RET_ERR;
    }

    // 检查page size 和 block size
    gint32 page_size = 0;
    gint32 block_size = 0;
    (void)dal_get_property_value_int32(dev_handle, PROPERTY_RAW_EEPROM_PAGE_SIZE, &page_size);
    (void)dal_get_property_value_int32(dev_handle, PROPERTY_RAW_EEPROM_BLOCK_SIZE, &block_size);
    if (page_size == 0 || block_size == 0) {
        debug_log(DLOG_ERROR, "[%s] page_size:%d  block_size:%d", __func__, page_size, block_size);
        return RET_ERR;
    }
    write_para->block_size = block_size;
    write_para->page_size = page_size;

    // 检查page的合法性，page合法说明offset合法，start_pos合法
    gint32 page = offset / page_size;
    if (page < 0 || page >= EEPROM_PAGE_COUNT) {
        debug_log(DLOG_ERROR, "[%s] Offset %d is not right", __func__, offset);
        return RET_ERR;
    }

    // 检查 end pos
    gint32 start_pos = offset - page * page_size;
    gint32 end_pos = start_pos + length;
    if (end_pos >= EEPROM_PAGE_SIZE) {
        debug_log(DLOG_ERROR, "[%s] Write out of one page, start pos %d, end pos %d", __func__, start_pos, end_pos);
        return RET_ERR;
    }
    write_para->start_pos = start_pos;
    write_para->end_pos = end_pos;

    // 获取对应页面的对象
    OBJ_HANDLE page_handle = 0;
    gint32 ret = dal_get_refobject_handle_nth(dev_handle, PROPERTY_RAW_EEPROM_PAGES, page, &page_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get reference object %s failed, nth, page %d, err code %d", PROPERTY_RAW_EEPROM_PAGES,
            page, ret);
        return RET_ERR;
    }
    const gchar *chip_name = dfl_get_object_name(page_handle);
    if (chip_name == NULL) {
        debug_log(DLOG_ERROR, "Get object %s name failed", PROPERTY_RAW_EEPROM_PAGES);
        return RET_ERR;
    }
    write_para->chip_name = chip_name;

    return RET_OK;
}


LOCAL gint32 __eeprom_block_write_do(OBJ_HANDLE dev_handle, gsize length, gpointer in_data,
    EEPROM_WRITE_PARA_S write_para)
{
    gint32 ret = RET_OK;
    // EEPROM每16字节划分为一个块
    // 每次数据写入只能在块内操作
    // 划分数据为多个块进行写入
    gchar *data_pointer = (gchar *)in_data;
    gint32 start_block = write_para.start_pos / write_para.block_size;
    gint32 end_block = write_para.end_pos / write_para.block_size;
    for (gint32 current_block = start_block; current_block <= end_block; current_block++) {
        gint32 write_length = EEPROM_BLOCK_SIZE;
        if (current_block == start_block) {
            write_length = EEPROM_BLOCK_SIZE - write_para.start_pos % EEPROM_BLOCK_SIZE;
            if (write_length > (gint32)length) { // 实际写入长度
                write_length = length;
            }
        }

        if (current_block == end_block) {
            write_length = write_para.end_pos % EEPROM_BLOCK_SIZE;
        }

        if (write_length == 0) {
            break;
        }

        for (gint32 i = 0; i < MAX_TRY_TIMES; i++) { // 尝试五次写入
            ret = dfl_chip_blkwrite(write_para.chip_name, write_para.start_pos, write_length, data_pointer);
            if (ret == DFL_OK) {
                break;
            }
            vos_task_delay(50);
        }
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Write data to eeprom failed, chip_name %s, offset %d, length %d",
                write_para.chip_name, write_para.start_pos, write_length);
            break;
        }
        write_para.start_pos += write_length; // Eeprom中地址增加
        data_pointer += write_length;         // 数据buffer中地址增加
    }
    return ret;
}


LOCAL gint32 __eeprom_block_write(OBJ_HANDLE dev_handle, gint32 offset, gsize length, gpointer in_data)
{
    EEPROM_WRITE_PARA_S write_para = { 0 };

    // 写入前合法性检查
    gint32 check_ret = __eeprom_block_write_precheck(dev_handle, offset, length, in_data, &write_para);
    if (check_ret == RET_ERR) {
        return check_ret;
    }

    // 分块写入
    return __eeprom_block_write_do(dev_handle, length, in_data, write_para);
}


LOCAL void __clear_chassis_fru_in_raw_eeprom_task(gpointer data)
{
    FRU_PRIV_PROPERTY_S *fru_priv = NULL;

    (void)prctl(PR_SET_NAME, (gulong) "ClearRawE2p");

    
    (void)vos_thread_sem4_p(g_eeprom_write_lock, 0);

    
    OBJ_HANDLE fru_handle = (OBJ_HANDLE)data;
    gint32 ret = get_obj_priv_data(fru_handle, (gpointer)fru_priv);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get object private data failed, ret = %d!", ret);
        (void)vos_thread_sem4_v(g_eeprom_write_lock);
        return;
    }

    OBJ_HANDLE frudev_handle = fru_priv->fru_property.frudev_handle;
    ret = set_write_protect_flag(frudev_handle, NO_WRITE_PROTECT);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set no write protect failed, err code %d", ret);
        (void)vos_thread_sem4_v(g_eeprom_write_lock);
        put_obj_priv_data(fru_handle, (gpointer)fru_priv);
        return;
    }

    
    gint32 offset_array[] = {
        CHASSIS_PART_NUMBER_OFFSET + fru_priv->fru_property.fru_offset,
        CHASSIS_SERIAL_NUMBER_OFFSET + fru_priv->fru_property.fru_offset,
        CHASSIS_TYPE_OFFSET + fru_priv->fru_property.fru_offset
    };
    gchar clear_buffer[3] = { 0 }; // Erase flag to zero
    for (gint32 i = 0; i < CHASSIS_FRU_COUNT; i++) {
        ret = __eeprom_block_write(frudev_handle, offset_array[i], sizeof(clear_buffer), clear_buffer); // 擦除有效位
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Clear chassis infomation failed, offset %d, err code %d", offset_array[i], ret);
        }
    }

    
    ret = set_write_protect_flag(frudev_handle, WRITE_PROTECT);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set write protect failed, err code %d", ret);
    }
    (void)vos_thread_sem4_v(g_eeprom_write_lock);
    put_obj_priv_data(fru_handle, (gpointer)fru_priv);
}


LOCAL gint32 init_fru_from_back_plane(FRU_PROPERTY_S *fru_prop, FRU_INFO_S *fru_info)
{
    
    gint32 ret = __init_chassis_fru_info_from_raw_eeprom(fru_prop, fru_info);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Init chassis fru info failed, err code %d", ret);
    }

    
    ret = __init_backplane_fru_info_from_raw_eeprom(fru_prop, fru_info);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Init back plane fru info failed, err code %d", ret);
    }

    return ret;
}


gint32 init_smm_backplane_fru_file(FRU_PROPERTY_S *fru_prop, FRU_INFO_S *tmp_fru_info)
{
    fru_prop->eeprom_accessor_prop = PROPERTY_RAW_EEPROM_ACCESSOR; // Eeprom心跳，检测Eeprom健康状况
    
    gint32 ret = __init_back_plane();
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "init back plane failed, ret is %d", ret);
        return RET_ERR;
    }

    
    ret = init_chassis_info_from_back_plane(fru_prop);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "update chassis info from raw eeprom failed, ret is %d", ret);
        return RET_ERR;
    }

    
    init_default_fru_info(fru_prop, tmp_fru_info);
    ret = init_fru_from_back_plane(fru_prop, tmp_fru_info);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "init fru info from raw eeprom failed, ret is %d", ret);
        return RET_ERR;
    }
    return RET_OK;
}


gint32 lock_share_eeprom(OBJ_HANDLE object_handle)
{
#define ONE_SECOND 1000
#define TASK_DELAY_ONCE 100
#define DEFAULT_TIME_OUT 3 // 默认超时时间

    guint8 is_lock = 0;
    guint8 timeout = DEFAULT_TIME_OUT;
    guint32 time = 0;
    guint8 status = 0;
    guint8 expect_status = 0;
    gint32 ret;
    (void)dal_get_property_value_byte(object_handle, PROPERTY_SHARE_FRUDEV_WAIT_LOCK_TIMEOUT, &timeout);
    while (time / ONE_SECOND <= timeout) {
        (void)dal_get_property_value_byte(object_handle, PROPERTY_SHARE_FRUDEV_I2C_LOCK_ACCESSOR, &is_lock);
        if (is_lock != TRUE) {
            ret = dal_set_property_value_byte(object_handle, PROPERTY_SHARE_FRUDEV_I2C_LOCK_ACCESSOR, TRUE, DF_HW);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, " %s %s set property %s byte fail: %d", __FUNCTION__,
                    dfl_get_object_name(object_handle), PROPERTY_SHARE_FRUDEV_I2C_LOCK_ACCESSOR, ret);
                break;
            }
            ret = dal_get_extern_value_byte(object_handle, PROPERTY_SHARE_FRUDEV_LOCK_STATUS_ACCESSOR, &status, DF_HW);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, " %s %s get property %s byte fail: %d", __FUNCTION__,
                    dfl_get_object_name(object_handle), PROPERTY_SHARE_FRUDEV_LOCK_STATUS_ACCESSOR, ret);
                break;
            }
            ret = dal_get_property_value_byte(object_handle, PROPERTY_SHARE_FRUDEV_LOCK_STATUS_EXPECT_VAL,
                &expect_status);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, " %s %s get property %s byte fail: %d", __FUNCTION__,
                    dfl_get_object_name(object_handle), PROPERTY_SHARE_FRUDEV_LOCK_STATUS_EXPECT_VAL, ret);
                break;
            }
            if (status == expect_status) {
                return RET_OK;
            }
            (void)dal_set_property_value_byte(object_handle, PROPERTY_SHARE_FRUDEV_I2C_LOCK_ACCESSOR, FALSE, DF_HW);
        }
        vos_task_delay(TASK_DELAY_ONCE);
        time = time + TASK_DELAY_ONCE;
    }
    (void)dal_set_property_value_byte(object_handle, PROPERTY_SHARE_FRUDEV_I2C_LOCK_ACCESSOR, FALSE, DF_HW);
    return RET_ERR;
}

void unlock_share_eeprom(OBJ_HANDLE object_handle)
{
    (void)dal_set_property_value_byte(object_handle, PROPERTY_SHARE_FRUDEV_I2C_LOCK_ACCESSOR, FALSE, DF_HW);
    return;
}


LOCAL gint32 get_access_chip_handle(OBJ_HANDLE obj_handle, const char *relation_name, OBJ_HANDLE *chip_handle)
{
    gchar accessor_name[MAX_NAME_SIZE];
    OBJ_HANDLE accessor_obj;
    OBJ_HANDLE chip_obj;

    gint32 ret = dfl_get_property_accessor(obj_handle, relation_name, accessor_name, sizeof(accessor_name));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s get accessor name failed, ret(%d).", relation_name, ret);
        return ret;
    }

    ret = dfl_get_object_handle(accessor_name, &accessor_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s get accessor object handle failed, ret(%d).", accessor_name, ret);
        return ret;
    }

    ret = dfl_get_referenced_object(accessor_obj, PROPERTY_ACCESSOR_CHIP, &chip_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get chip object failed, ret(%d).", ret);
        return ret;
    }

    *chip_handle = chip_obj;

    return ret;
}


void write_eeprom_count_log(OBJ_HANDLE obj_handle, const char *relation_name)
{
    gint32 ret;
    OBJ_HANDLE chip_handle = 0;
    guint32 write_count = 0;

    ret = get_access_chip_handle(obj_handle, relation_name, &chip_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get chip object handle failed, ret(%d).", ret);
        return;
    }

    ret = dal_get_property_value_uint32(chip_handle, PROPERTY_EEPROM_WRITECOUNT, &write_count);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get eeprom %s failed, ret(%d).", PROPERTY_EEPROM_WRITECOUNT, ret);
        return;
    }

    write_count++;

    ret = dal_set_property_value_uint32(chip_handle, PROPERTY_EEPROM_WRITECOUNT, write_count, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s set property %s value %u failed! result: %d.", dfl_get_object_name(chip_handle),
            PROPERTY_EEPROM_WRITECOUNT, write_count, ret);
    } else {
        debug_log(DLOG_INFO, "Obj(%s) eeprom(%s) wrote %u time(s).", dfl_get_object_name(obj_handle), relation_name,
            write_count);
    }
}


gint32 frudev_write_to_eeprom(OBJ_HANDLE object_handle, guint32 offset, const void *write_buf, guint32 length)
{
    G_LOCK(g_lock_write_to_eeprom);

    
    guint8 wpflag = 0;
    (void)dal_get_property_value_byte(object_handle, PROPERTY_FRUDEV_ISWP, &wpflag);

    
    gint32 ret;
    if (wpflag == 1) {
        ret = dal_frudev_opt_eeprom_wp(object_handle, PROPERTY_FRUDEV_EEPROM_WP, CLOSE_WP);
        if (ret != RET_OK) {
            G_UNLOCK(g_lock_write_to_eeprom);
            debug_log(DLOG_ERROR, "%s close wp failed, ret=%d", dfl_get_object_name(object_handle), ret);
            return ERROR_CODE_WRITE_E2P_ERROR;
        }
    }

    gchar  fru_dev_class_name[MAX_NAME_SIZE] = {0};
    (void)dfl_get_class_name(object_handle, fru_dev_class_name, MAX_NAME_SIZE);
    if (g_strcmp0(fru_dev_class_name, CLASS_SHARE_FRUDEV) == 0) {
        ret = lock_share_eeprom(object_handle);
        if (ret != RET_OK) {
            G_UNLOCK(g_lock_write_to_eeprom);
            debug_log(DLOG_ERROR, " %s lock_share_eeprom fail: %d", __FUNCTION__, ret);
            return ret;
        }
    }
    ret = fru_block_write(object_handle, PROPERTY_FRUDEV_STORAGE_LOC, offset, length, write_buf);
    if (g_strcmp0(fru_dev_class_name, CLASS_SHARE_FRUDEV) == 0) {
        unlock_share_eeprom(object_handle);
    }

    
    if (wpflag == 1) {
        if (dal_frudev_opt_eeprom_wp(object_handle, PROPERTY_FRUDEV_EEPROM_WP, OPEN_WP) != RET_OK) {
            G_UNLOCK(g_lock_write_to_eeprom);
            debug_log(DLOG_ERROR, "%s open wp failed, ret=%d", dfl_get_object_name(object_handle), ret);
            return ERROR_CODE_WRITE_E2P_ERROR;
        }
    }

    G_UNLOCK(g_lock_write_to_eeprom);

    if (ret == RET_OK) {
        write_eeprom_count_log(object_handle, PROPERTY_FRUDEV_STORAGE_LOC);
        return RET_OK;
    }

    debug_log(DLOG_ERROR, "%s write eeprom failed!(ret: %d).", dfl_get_object_name(object_handle), ret);
    return ERROR_CODE_WRITE_E2P_ERROR;
}


gint32 frudev_write_to_back_eeprom(OBJ_HANDLE object_handle, guint32 offset, const gpointer write_buf, guint32 length)
{
    gint32 ret;
    gint32 result;
    guint8 wpflag;

    G_LOCK(g_lock_write_double_eeprom);
    (void)dal_get_property_value_byte(object_handle, PROPERTY_FRUDEV_ISWP, &wpflag);

    if (wpflag == EEPROM_WP_STATUS_ON) {
        ret = dal_frudev_opt_eeprom_wp(object_handle, PROPERTY_FRUDEV_EEPROM_BACK_WP, CLOSE_WP);
        if (ret != DFL_OK) {
            G_UNLOCK(g_lock_write_double_eeprom);
            debug_log(DLOG_ERROR, "%s close wp fail: %d", dfl_get_object_name(object_handle), ret);
            return ERROR_CODE_WRITE_E2P_ERROR;
        }
    }

    result = fru_block_write(object_handle, PROPERTY_FRUDEV_BACK_STORAGE_LOC, offset, length, write_buf);

    if (wpflag == EEPROM_WP_STATUS_ON) {
        ret = dal_frudev_opt_eeprom_wp(object_handle, PROPERTY_FRUDEV_EEPROM_BACK_WP, OPEN_WP);
        if (ret != DFL_OK) {
            G_UNLOCK(g_lock_write_double_eeprom);
            debug_log(DLOG_ERROR, "%s open wp fail: %d", dfl_get_object_name(object_handle), ret);
            return ERROR_CODE_WRITE_E2P_ERROR;
        }
    }

    G_UNLOCK(g_lock_write_double_eeprom);

    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "%s write eeprom failed!(ret: %d).", dfl_get_object_name(object_handle), result);
        return ERROR_CODE_WRITE_E2P_ERROR;
    }
    return result;
}


gint32 frudev_write_to_eeprom_double(OBJ_HANDLE object_handle, guint32 offset, const gpointer write_buf,
    guint32 length)
{
    gint32 ret;

    ret = frudev_write_to_eeprom(object_handle, offset, write_buf, length);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Write 1st eeprom failed!(ret: %d).", ret);
        return ret;
    }
    ret = frudev_write_to_back_eeprom(object_handle, offset, write_buf, length);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Write 2nd eeprom failed!(ret: %d).", ret);
    }

    return ret;
}


gint32 frudev_read_from_eeprom(OBJ_HANDLE object_handle, guint32 offset, gpointer read_buf, guint32 length)
{
    gint32 ret;
    gchar  fru_dev_class_name[MAX_NAME_SIZE] = {0};
    (void)dfl_get_class_name(object_handle, fru_dev_class_name, MAX_NAME_SIZE);

    if (g_strcmp0(fru_dev_class_name, CLASS_SHARE_FRUDEV) == 0) {
        ret = lock_share_eeprom(object_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, " %s lock_share_eeprom fail: %d", __FUNCTION__, ret);
            return ret;
        }
    }
    ret = fru_block_read(object_handle, PROPERTY_FRUDEV_STORAGE_LOC, offset, length, read_buf);
    if (g_strcmp0(fru_dev_class_name, CLASS_SHARE_FRUDEV) == 0) {
        unlock_share_eeprom(object_handle);
    }
    return ret;
}


gint32 get_back_plane_dev_handle(OBJ_HANDLE *dev_handle)
{
    OBJ_HANDLE fru_handle;
    gint32 ret = dal_get_specific_object_byte(CLASS_FRU, PROPERTY_FRU_TYPE, FRU_TYPE_BACKPLANE, &fru_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get back plane fru handle failed, err code %d", ret);
        return ret;
    }

    ret = dfl_get_referenced_object(fru_handle, PROPERTY_FRUDEV_RO, dev_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get back plane fru dev failed, err code %d", ret);
        return ret;
    }
    return RET_OK;
}


gint32 get_back_plane_fru_offset(guint16 *fru_offset)
{
    OBJ_HANDLE fru_handle;
    FRU_PRIV_PROPERTY_S *fru_priv_property = NULL;

    gint32 ret = dal_get_specific_object_byte(CLASS_FRU, PROPERTY_FRU_TYPE, FRU_TYPE_BACKPLANE, &fru_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get back plane fru handle failed, err code %d", ret);
        return ret;
    }

    
    ret = get_obj_priv_data(fru_handle, (gpointer *)&(fru_priv_property));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get binded object failed, ret = %d.", __FUNCTION__, ret);
        return ret;
    }

    *fru_offset = fru_priv_property->fru_property.fru_offset;
    put_obj_priv_data(fru_handle, (gpointer)fru_priv_property);
    return RET_OK;
}

LOCAL void __init_write_eeprom_buffer(const void *data, gsize data_length, gsize defined_length, gsize block_size,
    gchar *buffer)
{
    struct chassis_info *chassis_fru_info_ptr = (struct chassis_info *)buffer;

    
    chassis_fru_info_ptr->flag = 1;
    if (data_length > defined_length) {
        data_length = defined_length; // 数据超过长度限制后，截断。
    }

    if (data_length > 0) { // 数据长度为空，表示将eeprom中数据擦除。
        errno_t safe_fun_ret = memcpy_s(chassis_fru_info_ptr->data, block_size - CHASSIS_INFO_HEADER_SIZE, data,
            data_length); // data_length + 3 byte(crc_high + crc_low + flag) <= block_size
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    }

    guint16 crc_checksum = arith_make_crc_checksum(0, &(chassis_fru_info_ptr->flag),
        defined_length + sizeof(chassis_fru_info_ptr->flag)); // (writelength + flag) to make checksum
    chassis_fru_info_ptr->crc_high = HIBYTE(crc_checksum);
    chassis_fru_info_ptr->crc_low = LOBYTE(crc_checksum);
}


gint32 write_info_to_raw_eeprom(OBJ_HANDLE dev_handle, const void *data, gsize data_length, gsize defined_length,
    gint32 offset, gsize block_size)
{
    if (block_size <= CHASSIS_INFO_HEADER_SIZE) {
        debug_log(DLOG_ERROR, "[%s] Block size %" G_GSIZE_FORMAT " is too short", __FUNCTION__, block_size);
        return RET_ERR;
    }

    if (defined_length > (block_size - CHASSIS_INFO_HEADER_SIZE)) {
        debug_log(DLOG_ERROR, "[%s] Block size %" G_GSIZE_FORMAT " and defined length %" G_GSIZE_FORMAT " are invalid",
            __FUNCTION__, block_size, defined_length);
        return RET_ERR;
    }

    gchar *buffer = (gchar *)g_malloc0(block_size);
    if (buffer == NULL) {
        debug_log(DLOG_ERROR, "[%s] Malloc failed, size %" G_GSIZE_FORMAT, __FUNCTION__, block_size);
        return RET_ERR;
    }

    __init_write_eeprom_buffer(data, data_length, defined_length, block_size, buffer);

    
    (void)vos_thread_sem4_p(g_eeprom_write_lock, 0);

    
    gint32 ret = set_write_protect_flag(dev_handle, NO_WRITE_PROTECT);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set no write protect failed, err code %d", ret);
        g_free(buffer);
        goto ERR_OUT;
    }

    vos_task_delay(50);
    
    ret = __eeprom_block_write(dev_handle, offset, block_size, buffer);
    g_free(buffer);

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Write to eeprom failed, err code %d", ret);
        (void)vos_thread_sem4_v(g_eeprom_write_lock);
        return ret;
    }

ERR_OUT:
    
    ret = set_write_protect_flag(dev_handle, WRITE_PROTECT);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set write protect failed, err code %d", ret);
    }

    
    (void)vos_thread_sem4_v(g_eeprom_write_lock);
    return ret;
}


gint32 set_write_protect_flag(OBJ_HANDLE dev_handle, guint8 wpflag)
{
    
    guint8 hasWp = 0;
    gint32 ret = dal_get_property_value_byte(dev_handle, PROPERTY_FRUDEV_ISWP, &hasWp);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get property %s failed, err code %d", PROPERTY_FRUDEV_ISWP, ret);
        return ret;
    }

    
    if (hasWp != 1) {
        return RET_OK;
    }

    ret = dal_set_property_value_byte(dev_handle, PROPERTY_FRUDEV_EEPROM_WP, wpflag, DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set property %s failed, err code %d", PROPERTY_FRUDEV_EEPROM_WP, ret);
        return ret;
    }

    return RET_OK;
}


LOCAL gint32 __update_chassis_fru_in_raw_eeprom(guint8 area, guint8 field, FRU_PRIV_PROPERTY_S *fru_priv,
    guint8 *buffer, guint16 length)
{
    
    gint32 offset = 0;
    gsize block_size = 0;
    gsize write_len = 0;
    guint8 *data = NULL;

    if (area == FRU_AREA_EXTENDELABLE && strstr((const gchar *)buffer, BACK_PLANE_CHASSIS_TYPE)) {
        data = (guint8 *)strstr((const gchar *)buffer, "=");
        if (data != NULL) {
            offset = CHASSIS_TYPE_OFFSET;
            block_size = CHASSIS_TYPE_BLOCK_SIZE;
            write_len = CHASSIS_TYPE_LEN;
            data = data + 1; 
            length = (guint16)strlen((const gchar *)data);
        } else {
            debug_log(DLOG_ERROR, "Update elabel info failed, can not find info with area [%u] filed [%u]",
                area, field);
            return COMP_CODE_UNSUPPORT;
        }
    } else if (area == FRU_AREA_CHASSISINFO && field == FRU_CHASSIS_PART_NUMBER) {
        offset = CHASSIS_PART_NUMBER_OFFSET;
        block_size = CHASSIS_PART_NUMBER_BLOCK_SIZE;
        write_len = CHASSIS_PART_NUMBER_LEN;
        data = buffer;
    } else if (area == FRU_AREA_CHASSISINFO && field == FRU_CHASSIS_SERIAL_NUMBER) {
        offset = CHASSIS_SERIAL_NUMBER_OFFSET;
        block_size = CHASSIS_SERIAL_NUMBER_BLOCK_SIZE;
        write_len = CHASSIS_SERIAL_NUMBER_LEN;
        data = buffer;
    } else {
        debug_log(DLOG_ERROR, "Update elabel info failed, can not find info with area [%d] filed [%d]", area, field);
        return COMP_CODE_UNSUPPORT;
    }

    offset = offset + fru_priv->fru_property.fru_offset;

    
    gint32 ret =
        write_info_to_raw_eeprom(fru_priv->fru_property.frudev_handle, data, length, write_len, offset, block_size);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Update elabel info failed, can not write area [%d] filed [%d] to eeprom, err code %d",
            area, field, ret);
        return COMP_CODE_UNKNOWN;
    }
    return COMP_CODE_SUCCESS;
}


gint32 update_chassis_fru_in_raw_eeprom(guint8 area, guint8 field, FRU_PRIV_PROPERTY_S *fru_priv, guint8 *buffer,
    guint16 length)
{
    if (fru_priv == NULL || buffer == NULL) {
        return COMP_CODE_SUCCESS;
    }

    
    if (dal_match_product_id(PRODUCT_ID_PANGEA_V6) == TRUE) {
        return COMP_CODE_SUCCESS;
    }

    
    if (fru_priv->fru_property.fru_type != FRU_TYPE_BACKPLANE) {
        return COMP_CODE_SUCCESS;
    }

    return __update_chassis_fru_in_raw_eeprom(area, field, fru_priv, buffer, length);
}


gint32 clear_chassis_fru_in_raw_eeprom(OBJ_HANDLE fru_handle, FRU_PRIV_PROPERTY_S *fru_priv)
{
    if (fru_priv == NULL) {
        return COMP_CODE_SUCCESS;
    }

    
    guint8 software_type = MGMT_SOFTWARE_TYPE_UNKNOW;
    (void)dal_get_software_type(&software_type);
    if (software_type != MGMT_SOFTWARE_TYPE_EM) {
        return COMP_CODE_SUCCESS;
    }

    
    if (fru_priv->fru_property.fru_type != FRU_TYPE_BACKPLANE) {
        return COMP_CODE_SUCCESS;
    }

    gulong task_id = 0;
    gint32 ret = vos_task_create(&task_id, "ClearElabelRawE2p", (TASK_ENTRY)__clear_chassis_fru_in_raw_eeprom_task,
        (void *)fru_handle, DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Create clear elabel task failed, err code %d", ret);
        return COMP_CODE_UNKNOWN;
    }
    return COMP_CODE_SUCCESS;
}

#ifdef DFT_ENABLED

LOCAL gint32 __eeprom_test(guint32 command, gsize para_size, gconstpointer para_data, DFT_RESULT_S *dft_data,
    gpointer user_data)
{
    if (dft_data == NULL || user_data == NULL) {
        return RET_ERR;
    }

    (void)memset_s(dft_data->result_str, DFT_RESULT_STR_LEN, 0, DFT_RESULT_STR_LEN);

    switch (command) {
        case TEST_START:
        case TEST_STOP: {
            dft_data->status = COMP_CODE_SUCCESS;
            dft_data->result = TEST_NON;
            (void)memset_s(dft_data->result_str, DFT_RESULT_STR_LEN, 0, DFT_RESULT_STR_LEN);
            break;
        }
        case TEST_GET_RESULT: {
            OBJ_HANDLE handle = (OBJ_HANDLE)user_data;
            const gchar *chip_name = dfl_get_object_name(handle);
            gint32 result = 0;
            gint32 ret = dfl_chip_test(chip_name, &result);
            if (ret != DFL_OK) {
                debug_log(DLOG_ERROR, "call dfl_chip_test failed, error = %d!(line:%d).", ret, __LINE__);
                dft_data->status = COMP_CODE_SUCCESS;
                dft_data->result = TEST_FAILED;
                gint32 ret = snprintf_s(dft_data->result_str, sizeof(dft_data->result_str),
                    sizeof(dft_data->result_str) - 1, "%s Failed", chip_name);
                if (ret <= 0) {
                    debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret);
                }
                break;
            }

            dft_data->status = COMP_CODE_SUCCESS;
            dft_data->result = TEST_SUCCEED;
            (void)memset_s(dft_data->result_str, DFT_RESULT_STR_LEN, 0, DFT_RESULT_STR_LEN);
            break;
        }
        default:
            return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 __wp_eeprom_test_element_gvar(GVariant *array_gvar, OBJ_HANDLE *eeprom_handle,
    OBJ_HANDLE *eeprom_fru_handle)
{
#define REF_OBJ_NUM 2

    if (array_gvar == NULL || eeprom_handle == NULL || eeprom_fru_handle == NULL) {
        return RET_ERR;
    }

    
    GVariantIter iter;
    guint32 ref_obj_num = g_variant_iter_init(&iter, array_gvar);
    if (ref_obj_num != REF_OBJ_NUM) {
        debug_log(DLOG_ERROR, "%s get refobj num is %d. and num isn't equal 2.", __FUNCTION__, ref_obj_num);
        return RET_ERR;
    }

    
    GVariant *element_gvar = g_variant_iter_next_value(&iter);
    const gchar *str = g_variant_get_string(element_gvar, NULL);
    gint32 ret = dfl_get_object_handle(str, eeprom_handle);
    g_variant_unref(element_gvar);
    element_gvar = NULL;
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s get refobj handle failed, ret = %d.", __FUNCTION__, ret);
        return ret;
    }

    
    element_gvar = g_variant_iter_next_value(&iter);
    str = g_variant_get_string(element_gvar, NULL);
    ret = dfl_get_object_handle(str, eeprom_fru_handle);
    g_variant_unref(element_gvar);
    element_gvar = NULL;
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s get refobj handle failed, ret = %d.", __FUNCTION__, ret);
        return ret;
    }

    return RET_OK;
}


LOCAL gint32 __wp_eeprom_test(guint32 command, gsize para_size, gconstpointer para_data, DFT_RESULT_S *dft_data,
    gpointer user_data)
{
    OBJ_HANDLE handle = 0;
    OBJ_HANDLE eeprom_handle = 0;
    OBJ_HANDLE eeprom_fru_handle = 0;
    GVariant *array_gvar = NULL;
    gint32 ret;

    if (dft_data == NULL || user_data == NULL) {
        return RET_ERR;
    }

    handle = (OBJ_HANDLE)user_data;

    
    ret = dfl_get_property_value(handle, REF_OBJECT, &array_gvar);
    if (ret != DFL_OK) {
        return RET_ERR;
    }

    // 获取关联对象的句柄信息
    ret = __wp_eeprom_test_element_gvar(array_gvar, &eeprom_handle, &eeprom_fru_handle);
    g_variant_unref(array_gvar);
    if (ret != RET_OK) {
        return ret;
    }

    const gchar *chip_name = dfl_get_object_name(eeprom_handle);
    if (command == TEST_GET_RESULT) {
        ret = set_write_protect_flag(eeprom_fru_handle, NO_WRITE_PROTECT);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "[%s]Close write protect failed! ret = %d", __func__, ret);
            (void)memset_s(dft_data->result_str, DFT_RESULT_STR_LEN, 0, DFT_RESULT_STR_LEN);
            dft_data->status = COMP_CODE_SUCCESS;
            dft_data->result = TEST_FAILED;
            ret = snprintf_s(dft_data->result_str, sizeof(dft_data->result_str), sizeof(dft_data->result_str) - 1,
                "%s Failed", chip_name);
            if (ret <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret);
            }
            return RET_OK;
        }
        // 写保护关闭后，需要延时预留硬件生效时间，否则可能出现无法正常写入eeprom导致自测失败
        vos_task_delay(10); // 10 ms是硬件给出的硬件生效时间值
    }

    ret = __eeprom_test(command, para_size, para_data, dft_data, (gpointer)eeprom_handle);
    if (command == TEST_GET_RESULT) {
        if (set_write_protect_flag(eeprom_fru_handle, WRITE_PROTECT) != RET_OK) {
            debug_log(DLOG_ERROR, "[%s]Opem write protect failed! ret = %d", __func__, ret);
        }
    }

    return ret;
}

LOCAL gint32 __test_get_result_from_eeprom(DFT_RESULT_S *dft_data, gpointer user_data)
{
    OBJ_HANDLE dev_handle = (OBJ_HANDLE)user_data;
    OBJ_HANDLE access_obj = 0;
    gint32 ret = dal_get_refobject_handle_nth(dev_handle, PROPERTY_RAW_EEPROM_PAGES, 0, &access_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s]dal_get_refobject_handle_nth(%s) error = %d!", __func__, PROPERTY_RAW_EEPROM_PAGES,
            ret);
        return RET_ERR;
    }

    const gchar *chip_name = dfl_get_object_name(access_obj);

    
    (void)vos_thread_sem4_p(g_eeprom_write_lock, 0);
    
    ret = set_write_protect_flag(dev_handle, NO_WRITE_PROTECT);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s]Set no write protect failed, err code %d", __func__, ret);
        (void)vos_thread_sem4_v(g_eeprom_write_lock);
        dft_data->status = COMP_CODE_SUCCESS;
        dft_data->result = TEST_FAILED;
        gint32 ret = snprintf_s(dft_data->result_str, sizeof(dft_data->result_str), sizeof(dft_data->result_str) - 1,
            "%s Failed", chip_name);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
        return RET_OK;
    }

    vos_task_delay(50);
    gint32 result = 0;
    ret = dfl_chip_test(chip_name, &result);
    
    if (set_write_protect_flag(dev_handle, WRITE_PROTECT) != RET_OK) {
        debug_log(DLOG_ERROR, "[%s]Set write protect failed, err code %d", __func__, ret);
    }

    
    (void)vos_thread_sem4_v(g_eeprom_write_lock);

    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s]dfl_chip_test failed, err code %d", __func__, ret);
        dft_data->status = COMP_CODE_SUCCESS;
        dft_data->result = TEST_FAILED;
        gint32 ret = snprintf_s(dft_data->result_str, sizeof(dft_data->result_str), sizeof(dft_data->result_str) - 1,
            "%s Failed", chip_name);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
        return RET_OK;
    }

    dft_data->result = TEST_SUCCEED;
    dft_data->status = COMP_CODE_SUCCESS;
    (void)memset_s(dft_data->result_str, DFT_RESULT_STR_LEN, 0, DFT_RESULT_STR_LEN);

    return RET_OK;
}


gint32 shelf_eeprom_test(guint32 command, gsize para_size, gconstpointer para_data, DFT_RESULT_S *dft_data,
    gpointer user_data)
{
    if (dft_data == NULL || user_data == NULL) {
        return RET_ERR;
    }

    (void)memset_s(dft_data->result_str, DFT_RESULT_STR_LEN, 0, DFT_RESULT_STR_LEN);

    switch (command) {
        case TEST_START:
        case TEST_STOP:
            dft_data->status = COMP_CODE_SUCCESS;
            dft_data->result = TEST_NON;
            (void)memset_s(dft_data->result_str, DFT_RESULT_STR_LEN, 0, DFT_RESULT_STR_LEN);
            break;

        case TEST_GET_RESULT:
            return __test_get_result_from_eeprom(dft_data, user_data);
        default:
            return RET_ERR;
    }

    return RET_OK;
}


LOCAL gboolean __is_raw_eeprom_dev(gchar className[], gint32 wp_flag)
{
    return ((strcmp(className, CLASS_NAME_RAW_EEPROM_DEV) == 0) && (wp_flag == 0xAA));
}


LOCAL gboolean __is_no_raw_eeprom_dev(gchar className[], gint32 wp_flag)
{
    return ((strcmp(className, CLASS_NAME_RAW_EEPROM_DEV) != 0) && (wp_flag == 0xAA));
}


gint32 eeprom_self_test_dft_hook(OBJ_HANDLE handle, gpointer data)
{
    guint32 dft_id = 0;
    (void)dal_get_property_value_uint32(handle, DFT_ID, &dft_id);
    if (dft_id != DFT_EEPROM) {
        return RET_OK;
    }

    guint32 slot_id = 0;
    (void)dal_get_property_value_uint32(handle, SLOT_ID, &slot_id);

    guint32 dev_id = 0;
    (void)dal_get_property_value_uint32(handle, DEV_ID, &dev_id);

    OBJ_HANDLE ref_handle = 0;
    gint32 ret = dal_get_refobject_handle_nth(handle, REF_OBJECT, 0, &ref_handle);
    if (ret != RET_OK) {
        return RET_OK;
    }

    
    gchar className[MAX_NAME_SIZE] = {0};
    ret = dfl_get_class_name(ref_handle, className, MAX_NAME_SIZE);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s]get blackplane eeprom dft item name fail.", __FUNCTION__);
        return ret;
    }

    
    gint32 wp_flag = 0;
    (void)dal_get_property_value_int32(handle, INPUT_DATA, &wp_flag);

    if (__is_raw_eeprom_dev(className, wp_flag)) {
        ret = dfl_register_dft_item(dft_id, slot_id, dev_id, shelf_eeprom_test, (gpointer)ref_handle);
    } else if (__is_no_raw_eeprom_dev(className, wp_flag)) {
        ret = dfl_register_dft_item(dft_id, slot_id, dev_id, __wp_eeprom_test, (gpointer)handle);
    } else {
        ret = dfl_register_dft_item(dft_id, slot_id, dev_id, __eeprom_test, (gpointer)ref_handle);
    }

    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "register_dft_item %d fail ret %d.", dft_id, ret);
        return RET_OK;
    }

    debug_log(DLOG_INFO, "register_dft_item %d succeed.", dft_id);

    return RET_OK;
}
#endif


gint32 frudev_read_from_eeprom_raw(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file)
{
    if (fru_file == NULL) {
        debug_log(DLOG_ERROR, "[%s] param error.", __func__);
        return ERROR_CODE_WRITE_E2P_ERROR;
    }

    guint8 dev_slot_id = 0;
    (void)dal_get_property_value_byte(object_handle, PROPERTY_FRUDEV_STORAGE_LOC, &dev_slot_id);

    OBJ_HANDLE dev_object_handle = 0;
    gint32 ret =
        dal_get_specific_object_byte(CLASS_POWER_MGNT, PROPERTY_POWER_MGNT_SLOT, dev_slot_id - 1, &dev_object_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get special object fail!", __FUNCTION__);
        return ret;
    }

    ret = (gint32)fru_block_read(dev_object_handle, PROPERTY_POWER_MGNT_PS_FRU_EEPROM, 0, FRU_DATA_LEN,
        fru_file->frudata);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "read ps%d eeprom failed!dev_obj:%s,(ret:%d)", dev_slot_id,
            dfl_get_object_name(dev_object_handle), ret);
    }

    return ret;
}

gint32 frudev_read_from_eeprom_custom(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file)
{
    if (fru_file == NULL) {
        debug_log(DLOG_ERROR, "[%s] param error.", __func__);
        return ERROR_CODE_WRITE_E2P_ERROR;
    }

    guint8 fan_slot = 0;
    (void)dal_get_property_value_byte(object_handle, PROPERTY_FRUDEV_STORAGE_LOC, &fan_slot);

    OBJ_HANDLE fan_object_handle = 0;
    gint32 ret = dal_get_specific_object_byte(CLASS_NAME_SMM_FAN, PROPERTY_FAN_SLOT, fan_slot, &fan_object_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get special object fail!", __FUNCTION__);
        return ret;
    }

    
    guchar fan_fru_header[FAN_FRU_HEAD_LEN] = { 0 }; 
    ret = (gint32)fru_block_read(fan_object_handle, PROPERTY_FAN_FRU_EEPROM, 0, sizeof(fan_fru_header), fan_fru_header);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "read fan%d eeprom failed!(ret:%d)", fan_slot, ret);
        return ret;
    }

    
    guint32 data_length = 0x0000 | (fan_fru_header[2] << 8) | fan_fru_header[3];

    
    ret = (gint32)fru_block_read(fan_object_handle, PROPERTY_FAN_FRU_EEPROM, FAN_FRU_HEAD_LEN, data_length,
        fru_file->frudata);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "read fan%d eeprom failed!(ret:%d)", fan_slot, ret);
        return ret;
    }

    return ret;
}


void try_fix_fru_header(OBJ_HANDLE frudev_handle)
{
    gint32 ret;
    debug_log(DLOG_ERROR, "Warning: %s fru header error, trying to recover.", dfl_get_object_name(frudev_handle));
    ret = frudev_write_to_eeprom(frudev_handle, 0, "FRU", sizeof("FRU"));
    if (ret != FRUDATA_OK) {
        debug_log(DLOG_ERROR, "Warning: %s fru header recover fail.", dfl_get_object_name(frudev_handle));
    }
}
