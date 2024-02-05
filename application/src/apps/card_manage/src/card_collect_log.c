

#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <math.h>
#include "pme_app/pme_app.h"
#include "pcie_card.h"
#include "pcie_card_parse_bdf_thread.h"
#include "card_ipmi_msg.h"
#include "get_version.h"
#include "card_manage_module.h"
#include "pcie_card_comm.h"
#include "pme_app/common/debug_log_macro.h"
#include "../../../libs/pcie_mgmt/inc/pcie_mgmt_by_std_smbus.h"
#include "soc_board.h"
#include "sdi_card.h"

#define MCU_MAX_LOG_SIZE    (1 * 1024 * 1024) // 从mcu收集日志，日志文件允许最大值1

#pragma pack(1)
typedef struct {
    guint16 key;
    const gchar *log_regex;
} LOG_REGEX_MAP_S;
#pragma pack()




LOCAL gint32 collect_pcie_log_manual(guint32 position, guint8 slot)
{
    OBJ_HANDLE obj_handle = 0;
    PM_CHIP_INFO *pm_chip_info = NULL;

    gint32 retval = get_card_obj_handle_by_slot_position(&obj_handle, slot, position);
    if (retval != RET_OK || obj_handle == 0) {
        return RET_ERR;
    }

    retval = pm_get_binded_data(obj_handle, (gpointer *)&pm_chip_info);
    if (retval != RET_OK || pm_chip_info == NULL) {
        debug_printf("get bind date failed\n");
        return RET_ERR;
    }

    pm_chip_info->log_dump_trigger = LOG_DUMP_TRIGGER_RESET_OS;
    pm_chip_info->collect_log_task_id = 0;

    debug_log(DLOG_ERROR, "%s: Start collect %s mcu log manually.", __FUNCTION__, dfl_get_object_name(obj_handle));

    pm_unref_binded_data((gpointer)pm_chip_info);
    pm_chip_info = NULL;
    return RET_OK;
}


gint32 collect_mcu_log(GSList *input_list)
{
    if (input_list == NULL || g_slist_length(input_list) != 2) { 
        return RET_ERR;
    }

    guint32 position = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 0));
    guint8 slot = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1));
    if (collect_pcie_log_manual(position, slot) != RET_OK &&
        collect_soc_board_log_manual(position, slot) != RET_OK) {
        debug_printf("Invalid position or slot\n");
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL void pcie_card_delete_log_file(gchar *log_dir_path, const gchar *log_file_path, const gchar *task_log_regex)
{
    gint32 iRet = -1;
    DIR *dir = NULL;
    struct dirent *pStEntry = NULL;
    gboolean log_match = FALSE;
    GRegex *regex = NULL;
    GMatchInfo *match_info = NULL;
    gchar log_file_path_for_delete[MAX_FILEPATH_LENGTH] = {0};

    dir = opendir(log_dir_path);
    if (dir == NULL) {
        return;
    }
    // 遍历文件夹下的文件
    while (1) {
        pStEntry = readdir(dir);
        if (pStEntry == NULL) {
            break;
        }

        // 正则匹配错误日志文件名
        regex = g_regex_new(task_log_regex, (GRegexCompileFlags)0x0, (GRegexMatchFlags)0x0, NULL);
        log_match = g_regex_match(regex, pStEntry->d_name, (GRegexMatchFlags)0x0, &match_info);
        if (TRUE == log_match) {
            (void)memset_s(log_file_path_for_delete, sizeof(log_file_path_for_delete), 0,
                sizeof(log_file_path_for_delete));
            iRet = snprintf_s(log_file_path_for_delete, sizeof(log_file_path_for_delete),
                sizeof(log_file_path_for_delete) - 1, "%s/%s", log_dir_path, pStEntry->d_name);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
            }

            if (g_strcmp0(log_file_path, log_file_path_for_delete) != 0) {
                // 删除上一次记录的日志
                (void)vos_rm_filepath(log_file_path_for_delete);
            }
        }

        g_match_info_free(match_info);
        g_regex_unref(regex);
    }

    closedir(dir);
}
LOG_REGEX_MAP_S g_log_regex_map[] = {
    {REGISTER_KEY_ERROR_LOG,    TASK_ERROR_LOG_REGEX},
    {REGISTER_KEY_LAST_WORD,    TASK_LAST_WORD_REGEX},
    {REGISTER_KEY_RUNNING_LOG,  TASK_RUNNING_LOG_REGEX},
    {REGISTER_KEY_OPERATE_LOG,  TASK_OPERATE_LOG_REGEX},
    {REGISTER_KEY_MAINTAINCE_LOG, TASK_MAINTAINCE_LOG_REGEX}
};
LOCAL const gchar *pcie_card_update_get_log_regex(guint16 key)
{
    for (gint i = 0; i < sizeof(g_log_regex_map) / sizeof(g_log_regex_map[0]); i++) {
        if (g_log_regex_map[i].key == key) {
            return g_log_regex_map[i].log_regex;
        }
    }
    return NULL;
}

// 获取日志文件大小
LOCAL glong get_log_file_size(const gchar *str_file_path)
{
    FILE *pf = dal_fopen_check_realpath(str_file_path, "r", str_file_path);
    glong file_size = RET_ERR;
    if (pf == NULL) {
        debug_log(DLOG_DEBUG, "%s: Failed to open file.", __FUNCTION__);
        return RET_ERR;
    }

    (void)fseek(pf, 0, SEEK_END);
    file_size = ftell(pf);
    (void)fclose(pf);

    return file_size;
}

// 获取日志文件名称
LOCAL void pcie_card_get_log_filename(PM_CHIP_INFO *pm_chip_info, gchar *log_dir_path, const gchar *log_name_prefix,
    gchar *log_file_path, guint32 log_file_path_len)
{
    GDateTime *dt = g_date_time_new_from_unix_local((gint64)vos_get_cur_time_stamp());
    gint32 ret = RET_OK;
    // 格式化输出文件路径
    if (pm_chip_info->collect_log_from_mcu == COLLECT_LOG_FROM_MCU_BUSY) { // MCU收集日志
        // mcu日志文件 error_log_+Card_desc,在application\rootfs\etc\logrotate.d\pme自动转储
        ret = snprintf_s(log_file_path, log_file_path_len, log_file_path_len - 1,
            "%s/%s.bin", log_dir_path, log_name_prefix);
    } else {
        ret = snprintf_s(log_file_path, log_file_path_len, log_file_path_len - 1,
            "%s/%s_%04d%02d%02d%02d%02d%02d.bin", log_dir_path, log_name_prefix, g_date_time_get_year(dt),
            g_date_time_get_month(dt), g_date_time_get_day_of_month(dt), g_date_time_get_hour(dt),
            g_date_time_get_minute(dt), g_date_time_get_second(dt));
    }
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    g_date_time_unref(dt);

    return;
}

// 打开日志文件
LOCAL FILE *pcie_card_open_log_file(PM_CHIP_INFO *pm_chip_info, const char *log_file_path, guint32 log_buf_len)
{
    if (pm_chip_info->collect_log_from_mcu != COLLECT_LOG_FROM_MCU_BUSY) {
        if (dal_check_realpath_before_open(log_file_path) == RET_OK) {
            return fopen(log_file_path, "wb+"); // 创建错误日志文件
        } else {
            return NULL;
        }
    }
    glong file_size = get_log_file_size(log_file_path);
    if (file_size < 0) { // 文件不存在则创建
        return dal_fopen_check_realpath(log_file_path, "wb+", log_file_path);
    }

    // 当前文件大小 + 要写入的大小 <= 1M 则从文件末尾追加写
    if (file_size + log_buf_len <= MCU_MAX_LOG_SIZE) {
        return dal_fopen_check_realpath(log_file_path, "ab+", log_file_path);
    } else { // 当前文件大小 + 要写入的大小 > 1M 则从文件开头开始写, 覆盖原有位置的内容
        return dal_fopen_check_realpath(log_file_path, "rb+", log_file_path);
    }
    return NULL;
}

void pcie_card_update_log(PM_CHIP_INFO *pm_chip_info, gchar *log_dir_path, guint16 key, guint16 path_len,
    const gchar *log_name_prefix)
{
    gint32 ret = RET_OK;
    guint8 *log_buf = NULL;
    guint32 log_buf_len = 0;
    gchar log_file_path[MAX_FILEPATH_LENGTH] = {0};
    FILE *fp = NULL;
    const gchar *task_log_regex = NULL;

    PROTOCOL_HEAD_PARA head_para = { 0 };

    if ((log_dir_path == NULL) || (path_len == 0)) {
        debug_log(DLOG_DEBUG, "%s : Input parameter error.", __FUNCTION__);
        return;
    }
    debug_log(DLOG_INFO, "%s : begin to collect log %x.....", __FUNCTION__, pm_chip_info->collect_log_from_mcu);

    
    if (pm_chip_info->collect_log_from_mcu != COLLECT_LOG_FROM_MCU_BUSY && pcie_card_mgnt_ready() != TRUE) {
        return;
    }
    // 收集日志
    debug_log(DLOG_DEBUG, "collect %s beginning time: %lu", log_name_prefix, vos_tick_get());

    if (PCIE_MGNT_PROTOCOL_SMBUS == pm_chip_info->inf_protocol) {
        ret = pm_get_log(pm_chip_info, key, &log_buf, &log_buf_len);
    } else if (PCIE_MGNT_PROTOCOL_STD_SMBUS == pm_chip_info->inf_protocol) {
        head_para.opcode = key;
        ret = pm_get_random_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, &log_buf, &log_buf_len);
    }

    if ((ret != RET_OK) || (log_buf == NULL)) {
        debug_log(DLOG_DEBUG, "%s : get pcie card error log from chip name(%s) failed! ret = %d\n", __FUNCTION__,
            pm_chip_info->chip_name, ret);
        return;
    }

    task_log_regex = pcie_card_update_get_log_regex(key);
    if (task_log_regex == NULL) {
        g_free(log_buf);
        return;
    }

    pcie_card_get_log_filename(pm_chip_info, log_dir_path, log_name_prefix, log_file_path, sizeof(log_file_path));

    debug_log(DLOG_DEBUG, "collect %s finishing time == %lu", log_name_prefix, vos_tick_get());
 
    fp = pcie_card_open_log_file(pm_chip_info, log_file_path, log_buf_len);
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "%s : Failed to open file error_log.bin.", __FUNCTION__);
        g_free(log_buf);
        return;
    }

    (void)fchmod(fileno(fp), 0640);
    // 往文件里写错误日志信息
    (void)fwrite((const void *)log_buf, log_buf_len, 1, fp);
    (void)fclose(fp);
    g_free(log_buf);

    // 删除上一次保留的日志
    pcie_card_delete_log_file(log_dir_path, log_file_path, task_log_regex);
}


LOCAL gint32 pcie_card_create_log_dir(OBJ_HANDLE obj_handle, gchar *log_dir_path, guint16 log_dir_path_len)
{
    gint32 iRet = -1;
    errno_t safe_fun_ret = EOK;
    OBJ_HANDLE ref_handle = 0;
    gchar device_name[CLASSE_NAME_LEN] = {0};
    gchar pciecard_name[CLASSE_NAME_LEN] = {0};
    gchar pciecard_name_formated[CLASSE_NAME_LEN] = {0};
    gint32 ret = RET_OK;

    // 获取component句柄
    ret = dfl_get_referenced_object(obj_handle, PROPERTY_PCIE_CARD_REF_COMPONENT, &ref_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get refcomponent failed, ret=%d!", ret);
        return ret;
    }

    (void)memset_s(device_name, sizeof(device_name), 0, sizeof(device_name));
    (void)memset_s(pciecard_name, sizeof(pciecard_name), 0, sizeof(pciecard_name));
    (void)memset_s(pciecard_name_formated, sizeof(pciecard_name_formated), 0, sizeof(pciecard_name_formated));
    // 获取device_name
    (void)dal_get_property_value_string(ref_handle, PROPERTY_COMPONENT_DEVICE_NAME, device_name, sizeof(device_name));
    // 获取卡对外名称
    (void)dal_get_property_value_string(obj_handle, PROPERTY_PCIE_CARD_SLAVE_CARD_NAME, pciecard_name,
        sizeof(pciecard_name));
    // 去除空格
    dal_clear_string_blank(device_name, sizeof(device_name));
    dal_clear_string_blank(pciecard_name, sizeof(pciecard_name));
    // 创建文件夹名 若后期新增支持8100等服务器，此处还应新增Location标识
    iRet =
        snprintf_s(log_dir_path, log_dir_path_len, log_dir_path_len - 1, "%s%s", PCIE_CARD_LOG_BASE_DIR, device_name);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    iRet = snprintf_s(pciecard_name_formated, sizeof(pciecard_name_formated), sizeof(pciecard_name_formated) - 1, "_%s",
        pciecard_name);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    safe_fun_ret = strncat_s(log_dir_path, log_dir_path_len, pciecard_name_formated, strlen(pciecard_name_formated));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    if (FALSE == vos_check_dir(PCIE_CARD_LOG_BASE_DIR)) {
        // 文件以及路径都不存在就创建路径
        ret = vos_mkdir_recursive(PCIE_CARD_LOG_BASE_DIR);
        if (ret < 0) {
            debug_log(DLOG_ERROR, "vos_mkdir_recursive failed: ret %d.", ret);
            return ret;
        }
    }

    (void)chmod(PCIE_CARD_LOG_BASE_DIR, 0750);

    if (FALSE == vos_check_dir(log_dir_path)) {
        // 文件以及路径都不存在就创建路径
        ret = vos_mkdir_recursive(log_dir_path);
        if (ret < 0) {
            debug_log(DLOG_ERROR, "vos_mkdir_recursive failed: ret %d.", ret);
            return ret;
        }
    }

    (void)chmod(log_dir_path, 0750);
    return RET_OK;
}
// MCU日志是否立即收集：在opcode=0x1031返回信息要求"日志收集"时收集
LOCAL gboolean is_collect_log_from_mcu_now(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    guint8 log_state = COLLECT_LOG_FROM_MCU_LATER;
    PROTOCOL_HEAD_PARA head_para = { 0 };
    
    head_para.opcode = REGISTER_KEY_GET_LOG_STATE;
    gint32 ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, (guint8 *)&log_state,
        KEY_LOG_STATE_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get pcie card log state failed! ret = %d", __FUNCTION__, ret);
        return FALSE;
    }

    // log_state：0 - 无需立即收集。 1 - 需要立即收集
    return (log_state == COLLECT_LOG_FROM_MCU_NOW);
}

// 修改 mcu收集日志的状态
LOCAL void update_collect_log_from_mcu_state(PM_CHIP_INFO *pm_chip_info, guint8 collect_log_from_mcu_state)
{
    g_mutex_lock(&pm_chip_info->rw_mutex);
    pm_chip_info->collect_log_from_mcu = collect_log_from_mcu_state;
    g_mutex_unlock(&pm_chip_info->rw_mutex);
    return;
}

// 获取mcu收集日志的日志文件前缀
LOCAL void pcie_card_get_log_name_prefix_of_mcu(OBJ_HANDLE obj_handle, gchar *log_name_prefix, guint32 log_name_len)
{
    gchar tmp[MAX_PCIE_CARD_DESC_LEN] = {0};
    (void)dal_get_property_value_string(obj_handle, PROPERTY_PCIECARD_DESC, tmp, sizeof(tmp));
    gint32 ret = snprintf_s(log_name_prefix, log_name_len, log_name_len - 1, "error_log_%s", tmp);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: obj[%s] snprintf_s fail, ret = %d.",
            __FUNCTION__, dfl_get_object_name(obj_handle), ret);
    }
    dal_remove_space_in_string(log_name_prefix, log_name_len);
    debug_log(DLOG_DEBUG, "%s:............. log_name_prefix[%s] .............", __FUNCTION__, log_name_prefix);

    return;
}

// 从mcu收集日志
LOCAL void pcie_card_collect_log_from_mcu(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info,
    gchar *log_dir_path, guint32 log_dir_path_len)
{
    // 检查是否需要立即收集,不需要立即收集就返回
    if (is_collect_log_from_mcu_now(obj_handle, pm_chip_info) == FALSE) {
        debug_log(DLOG_DEBUG, "%s: The log collection task of (%s) does not need to be executed right now.",
            __FUNCTION__, dfl_get_object_name(obj_handle));
        return ;
    }

    g_mutex_lock(&pm_chip_info->rw_mutex);
    if (pm_chip_info->collect_log_from_mcu == COLLECT_LOG_FROM_MCU_BUSY) {
        g_mutex_unlock(&pm_chip_info->rw_mutex);
        debug_log(DLOG_ERROR, "%s: The log collection task of (%s) has been running [%u].",
            __FUNCTION__, dfl_get_object_name(obj_handle), pm_chip_info->collect_log_from_mcu);
        return ;
    }
    pm_chip_info->collect_log_from_mcu = COLLECT_LOG_FROM_MCU_BUSY; // 占用收集通道
    g_mutex_unlock(&pm_chip_info->rw_mutex);

    gchar log_name_prefix[MAX_FILEPATH_LENGTH] = {0};
    pcie_card_get_log_name_prefix_of_mcu(obj_handle, log_name_prefix, sizeof(log_name_prefix));

    // 收集错误日志
    pcie_card_update_log(pm_chip_info, log_dir_path, REGISTER_KEY_ERROR_LOG, log_dir_path_len, log_name_prefix);

    update_collect_log_from_mcu_state(pm_chip_info, COLLECT_LOG_FROM_MCU_IDLE);  // 释放收集通道

    return;
}


LOCAL void pcie_card_collect_sdi5_log(OBJ_HANDLE obj_handle, const gchar* log_dir_path)
{
    guint16 board_id = 0;
    (void)dal_get_property_value_uint16(obj_handle, PROPERTY_COMPONENT_BOARDID, &board_id);
    // 通过PCie卡的board id判断是否是SDI5.0卡
    if (board_id != SDI_CARD_50_BOARDID_A5 && board_id != DPU_CARD_12_BOARDID_ED) {
        return;
    }

    OBJ_HANDLE ref_handle = 0;
    gint32 ret = dfl_get_referenced_object(obj_handle, PROPERTY_REF_COMPONENT, &ref_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: dfl_get_referenced_object ret %d", __FUNCTION__, ret);
        return;
    }
    
    _cleanup_gslist_ GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    (void)dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &obj_list);
    for (obj_node = obj_list; obj_node != NULL; obj_node = g_slist_next(obj_node)) {
        OBJ_HANDLE port_handle = (OBJ_HANDLE)obj_node->data;
        OBJ_HANDLE netcard_handle = 0;
        OBJ_HANDLE comp_handle = 0;
        
        ret = dfl_get_referenced_object(port_handle, BUSY_ETH_ATTRIBUTE_REF_NETCARD, &netcard_handle);
        if (ret != DFL_OK) {
            continue;
        }

        ret = dfl_get_referenced_object(netcard_handle, PROPERTY_REF_COMPONENT, &comp_handle);
        if (ret != DFL_OK) {
            continue;
        }

        if (comp_handle != ref_handle) {
            continue;
        }

        // 根据PCIE卡找到了SDI卡的网口对象, 开始收集日志
        _cleanup_gslist_ GSList* input_list = NULL;
        input_list = g_slist_append(input_list, g_variant_new_string(log_dir_path));
        
        ret = dfl_call_class_method(port_handle, BUSY_ETH_MCTP_GET_SDI5_LOG, NULL, input_list, NULL);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: collect log from port %s fail", __FUNCTION__, dfl_get_object_name(port_handle));
            continue;
        }

        // 只要网卡对应的网口有一个收集日志成功，即表示收集日志成功，
        return ;
    }
}


void *pcie_card_collect_log_task(OBJ_HANDLE obj_handle)
{
    gchar log_dir_path[MAX_FILEPATH_LENGTH] = {0};
    gint32 ret = RET_OK;
    PM_CHIP_INFO *pm_chip_info = NULL;

    (void)prctl(PR_SET_NAME, (gulong) "PcieLogTask");

    debug_log(DLOG_DEBUG, "%s: collect log start.", dfl_get_object_name(obj_handle));

    ret = pm_get_binded_data(obj_handle, (gpointer *)&pm_chip_info);
    if (ret != RET_OK || pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s (%d): Get object %s binded data failed. The error code is %d.", __FUNCTION__,
            __LINE__, dfl_get_object_name(obj_handle), ret);
        return NULL;
    }

    debug_log(DLOG_DEBUG, "%s, collect_log_task_id = %lu ", __FUNCTION__, pm_chip_info->collect_log_task_id);

    (void)memset_s(log_dir_path, sizeof(log_dir_path), 0, sizeof(log_dir_path));
    ret = pcie_card_create_log_dir(obj_handle, log_dir_path, sizeof(log_dir_path));
    if (ret != RET_OK) {
        pm_chip_info->collect_log_task_id = 0;
        debug_log(DLOG_ERROR, "%s (%d): Create pciecard log dir failed. ret = %d.", __FUNCTION__, __LINE__, ret);
        pm_unref_binded_data((gpointer)pm_chip_info);
        return NULL;
    }

    pcie_card_collect_sdi5_log(obj_handle, log_dir_path);

    // 是否是支持MCU的SDI卡
    if (is_sdi_support_mcu(obj_handle) == TRUE) {
        pcie_card_collect_log_from_mcu(obj_handle, pm_chip_info, log_dir_path, sizeof(log_dir_path));

        pm_chip_info->collect_log_task_id = 0;
        pm_unref_binded_data((gpointer)pm_chip_info);
        debug_log(DLOG_DEBUG, "%s: The log collection task of (%s) has finished.",
            __FUNCTION__, dfl_get_object_name(obj_handle));
        return NULL;
    }

    // 更新错误日志
    pcie_card_update_log(pm_chip_info, log_dir_path, REGISTER_KEY_ERROR_LOG, sizeof(log_dir_path), "error_log");

    if (pm_chip_info->inf_protocol == PCIE_MGNT_PROTOCOL_SMBUS) {
        // 更新临终遗言
        pcie_card_update_log(pm_chip_info, log_dir_path, REGISTER_KEY_LAST_WORD, sizeof(log_dir_path), "last_word");

        // 更新巡检日志
        pcie_card_update_log(pm_chip_info, log_dir_path, REGISTER_KEY_RUNNING_LOG, sizeof(log_dir_path), "running_log");
    } else if (pm_chip_info->inf_protocol == PCIE_MGNT_PROTOCOL_STD_SMBUS) {
        // 更新操作日志
        pcie_card_update_log(pm_chip_info, log_dir_path, REGISTER_KEY_OPERATE_LOG, sizeof(log_dir_path), "operate_log");

        // 更新维护日志
        pcie_card_update_log(pm_chip_info, log_dir_path, REGISTER_KEY_MAINTAINCE_LOG, sizeof(log_dir_path),
            "maintaince_log");
    }

    pm_chip_info->collect_log_task_id = 0;

    pm_unref_binded_data((gpointer)pm_chip_info);
    debug_log(DLOG_DEBUG, "%s: collect log finish.", dfl_get_object_name(obj_handle));
    return NULL;
}



LOCAL void *pcie_card_collect_log_task_delay(OBJ_HANDLE obj_handle)
{
    
    (void)prctl(PR_SET_NAME, (gulong) "PcieCollectLog");
    debug_log(DLOG_ERROR, "%s: Delay 3h, then begin to collect pcie card log.", __FUNCTION__);
    vos_task_delay(3 * 60 * 60 * 1000);
    return pcie_card_collect_log_task(obj_handle);
}
LOCAL void collect_log(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info,
    guint32 fault_code_buf_len, guint8 *fault_code_buf, guint8 health_state)
{
    guint8 slot_id = 0;
    gchar collect_task[PROP_VAL_LENGTH] = {0};
    gint32 ret = RET_OK;
    // 支持mcu的sdi卡需要立即收集
    gboolean collect_log_from_mcu_now = (pm_chip_info->card_type == SDI_CARD && is_sdi_support_mcu(obj_handle) &&
        is_collect_log_from_mcu_now(obj_handle, pm_chip_info));

    debug_log(DLOG_INFO, "%s: is_from_mcu[%u].", __FUNCTION__, collect_log_from_mcu_now);

    
    // 如果是BMC或os重启第一次轮询或者健康状态比上一次严重则开始收集日志
    gboolean collect_flag =
        (pm_chip_info->log_dump_trigger == LOG_DUMP_TRIGGER_RESET_OS) ||
        (pm_chip_info->log_dump_trigger == LOG_DUMP_TRIGGER_RESET_BMC) ||
        collect_log_from_mcu_now == TRUE ||
        ((health_state != 0) && (fault_code_buf != NULL) &&
        ((pm_chip_info->last_fault_code_len != fault_code_buf_len) || (memcmp(fault_code_buf,
        pm_chip_info->last_fault_code, MIN(fault_code_buf_len, sizeof(pm_chip_info->last_fault_code))) != 0)));
    if (collect_flag == TRUE) {
        
        get_slotid_by_handle(obj_handle, &slot_id);
        
        // 如果是BMC或os重启第一次轮询或者健康状态比上一次严重则开始收集日志
        (void)snprintf_s(collect_task, sizeof(collect_task), sizeof(collect_task) - 1, "collect_log_pcie%u", slot_id);

        if (pm_chip_info->collect_log_task_id == 0) {
            // collect_log_from_mcu_now = TRUE时,必须立即收集
            gboolean is_delay = (collect_log_from_mcu_now == FALSE && pm_chip_info->log_dump_trigger == LOG_DUMP_TRIGGER_RESET_BMC);
            if (is_delay == TRUE) {
                
                ret = vos_task_create(&(pm_chip_info->collect_log_task_id), collect_task,
                    (TASK_ENTRY)pcie_card_collect_log_task_delay, (void *)(uintptr_t)obj_handle, 20);
            } else {
                
                ret = vos_task_create(&(pm_chip_info->collect_log_task_id), collect_task,
                    (TASK_ENTRY)pcie_card_collect_log_task, (void *)(uintptr_t)obj_handle, 20);
            }

            if (ret != RET_OK) {
                debug_log(DLOG_INFO, "create %s task fail", collect_task);
            }
        }

        pm_chip_info->log_dump_trigger = LOG_DUMP_TRIGGER_UNRESET;
    }

    return;
}


void pcie_card_collect_log(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    gint32 ret = RET_OK;
    guint8 health_state = 0;
    guint8 *fault_code_buf = NULL;
    guint32 fault_code_buf_len = 0;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_DEBUG, "%s: parameter is null", __FUNCTION__);
        return;
    }

    
    if (pcie_card_mgnt_ready() != TRUE) {
        return;
    }

    ret = update_fault_state(obj_handle, pm_chip_info, &health_state, &fault_code_buf, &fault_code_buf_len);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: update pcie card fault code failed, ret = %d", __FUNCTION__, ret);
        return;
    }
    (void)collect_log(obj_handle, pm_chip_info, fault_code_buf_len, fault_code_buf, health_state);
    

    if (fault_code_buf == NULL) {
        return;
    }
    if (memcmp(fault_code_buf, pm_chip_info->last_fault_code,
        MIN(fault_code_buf_len, sizeof(pm_chip_info->last_fault_code))) != 0) {
        (void)memset_s(pm_chip_info->last_fault_code, sizeof(pm_chip_info->last_fault_code), 0,
            sizeof(pm_chip_info->last_fault_code));

        (void)memcpy_s(pm_chip_info->last_fault_code, sizeof(pm_chip_info->last_fault_code), fault_code_buf,
            MIN(fault_code_buf_len, sizeof(pm_chip_info->last_fault_code)));

        pm_chip_info->last_fault_code_len = fault_code_buf_len;
    }

    g_free(fault_code_buf);

    return;
}

// 检查收集mcu日志是否处于Idle状态
LOCAL gboolean is_collect_log_from_mcu_idle(PM_CHIP_INFO *pm_chip_info, gint8 retry_seconds)
{
    guint32 retry = 0;
    do {
        if (pm_chip_info->collect_log_from_mcu == COLLECT_LOG_FROM_MCU_IDLE) {
            return TRUE;
        }
        vos_task_delay(100);
    } while (++retry < 10 * retry_seconds); // 每次100ms,需要乘以10
    return FALSE;
}

LOCAL gboolean is_support_mcu(OBJ_HANDLE obj_handle)
{
    OBJ_HANDLE slave_obj_handle = 0;
    gchar slave_card_class_name[MAX_NAME_SIZE] = {0};

    if (obj_handle == 0) {
        return FALSE;
    }
    // 获取slave card对象
    (void)dal_get_property_value_string(obj_handle, PROPERTY_PCIE_CARD_SLAVE_CARD, slave_card_class_name,
        sizeof(slave_card_class_name));
    if (strlen(slave_card_class_name) == 0) {
        return FALSE;
    }

    gint32 ret = dal_get_specific_object_position(obj_handle, slave_card_class_name, &slave_obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get pcie card slave card object handle failed", __FUNCTION__);
        return FALSE;
    }

    guint8 mcu_support = 0;
    (void)dal_get_property_value_byte(slave_obj_handle, PROPERTY_SDI_CARD_MCUSUPPORTED, &mcu_support);
    if (mcu_support == 1) {
        debug_log(DLOG_INFO, "%s: SDI CARD %s  support mcu", __FUNCTION__, dfl_get_object_name(obj_handle));
        return TRUE;
    }
    return FALSE;
}

// 收集单个对象的mcu错误日志
LOCAL void pcie_dump_from_mcu_by_obj_handle(OBJ_HANDLE obj_handle)
{
    PM_CHIP_INFO *pm_chip_info = NULL;

    if (is_support_mcu(obj_handle) != TRUE) { // 不支持mcu的,不能从mcu收集日志.
        return;
    }

    gint32 ret = pm_get_binded_data(obj_handle, (gpointer *)&pm_chip_info);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: Get object %s binded data failed. The error code is %d.", __FUNCTION__,
            dfl_get_object_name(obj_handle), ret);
        return;
    }
 
    ret = get_pcie_mgnt_init_state(pm_chip_info);
    if (ret == RET_ERR) {
        debug_log(DLOG_DEBUG, "%s: Binded data is NULL, ret = %d. ", __FUNCTION__, ret);
        pm_unref_binded_data((gpointer)pm_chip_info);
        return;
    }

    gchar log_dir_path[MAX_FILEPATH_LENGTH] = {0};
    ret = pcie_card_create_log_dir(obj_handle, log_dir_path, sizeof(log_dir_path));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s (%d): Create pciecard log dir failed. ret = %d.",
            __FUNCTION__, __LINE__, ret);
        pm_unref_binded_data((gpointer)pm_chip_info);
        return;
    }
    debug_log(DLOG_DEBUG, "%s: collecting (%s) log from dump task beginning.",
        __FUNCTION__, dfl_get_object_name(obj_handle));

    g_mutex_lock(&pm_chip_info->rw_mutex);
    if (pm_chip_info->collect_log_from_mcu == COLLECT_LOG_FROM_MCU_BUSY) {
        g_mutex_unlock(&pm_chip_info->rw_mutex);
        // 若BUSY时,则轮询任务在收集,等轮询任务收集完直接退出
        gboolean is_idle = is_collect_log_from_mcu_idle(pm_chip_info, COLLECT_LOG_WAIT_IDLE_5_SECONDS);
        debug_log(DLOG_ERROR, "%s: (%s) log %s finished collecting.", __FUNCTION__,
            dfl_get_object_name(obj_handle), (is_idle) ? "has" : "hasn't");
        pm_unref_binded_data((gpointer)pm_chip_info);
        return;
    }
    pm_chip_info->collect_log_from_mcu = COLLECT_LOG_FROM_MCU_BUSY; // 占用收集通道
    g_mutex_unlock(&pm_chip_info->rw_mutex);

    gchar log_name_prefix[MAX_FILEPATH_LENGTH] = {0};
    pcie_card_get_log_name_prefix_of_mcu(obj_handle, log_name_prefix, sizeof(log_name_prefix));

    pcie_card_update_log(pm_chip_info, log_dir_path, REGISTER_KEY_ERROR_LOG, sizeof(log_dir_path), log_name_prefix);
    update_collect_log_from_mcu_state(pm_chip_info, COLLECT_LOG_FROM_MCU_IDLE);  // 释放收集通道

    pm_unref_binded_data((gpointer)pm_chip_info);
    debug_log(DLOG_DEBUG, "%s: collect (%s) log from dump task finished.",
        __FUNCTION__, dfl_get_object_name(obj_handle));

    return;
}


gint32 pcie_dump_info_from_mcu(const gchar *path)
{
    GSList *obj_list = NULL;
 
    gint32 ret = dfl_get_object_list(CLASS_PCIE_CARD, &obj_list);
    if (ret != DFL_OK || g_slist_length(obj_list) == 0) {
        return RET_OK;
    }

    debug_log(DLOG_DEBUG, "%s: begining to dump mcu info...", __FUNCTION__);
    for (GSList *obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        pcie_dump_from_mcu_by_obj_handle((OBJ_HANDLE)obj_node->data);
    }
    debug_log(DLOG_DEBUG, "%s: finishing dumping mcu info.", __FUNCTION__);

    g_slist_free(obj_list);

    return RET_OK;
}
