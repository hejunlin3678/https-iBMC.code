

#include "boot_options.h"
#include "bios.h"


#define BIOS_BOOT_TYPE_STR "BootType"
#define BIOS_UEFI_BOOT_STR "UEFIBoot"
#define BIOS_LEGACY_BOOT_STR "LegacyBoot"

#define BIOS_BOOT_SEG_MIN_NUM 1
#define BOOT_FLAGS_FMT "%02X-%02X-%02X-%02X-%02X"
#define BOOT_FLAGS_OBJ(_flags) (_flags)[0], (_flags)[1], (_flags)[2], (_flags)[3], (_flags)[4]
#define EFFECT_OPT_TO_TIMES(_opt) ((_opt) ? EFFECTIVE_FOREVER : EFFECTIVE_ONCE)
#define LOG_STR_BOOT_TYPE_LEGACY "Legacy"
#define LOG_STR_BOOT_TYPE_UEFI "UEFI"
#define MAX_BOOT_OPTION_ATTRIBUTE_LEN  64
typedef struct boot_option_attr_name {
    guint8 attr_id;
    const gchar attr_name[MAX_BOOT_OPTION_ATTRIBUTE_LEN];
} BOOT_OPTION_ATTR_NAME_S;

LOCAL const BOOT_OPTION_ATTR_NAME_S g_boot_option_arr[] = {
    {NO_OVERRIDE,        "no override"},
    {FORCE_HARD_DRIVE,   "hard_drive"},
    {FORCE_PEX,          "PXE"},
    {FORCE_CD_DVD,       "CD/DVD"},
    {FORCE_BIOS_SETUP,   "BIOS setup"},
    {FORCE_FLOPPY_REMOVABLE_MEDIA,  "floppy/primary removable media"},
};

LOCAL const BOOT_OPTION_ATTR_NAME_S g_boot_option_ext_arr[] = {
    {START_OPTION_EXT_TFTP,     "PXE with TFTP"},
    {START_OPTION_EXT_HTTPS,    "PXE with HTTPS"},
    {START_OPTION_EXT_NVMEM2,     "hard_drive with NVMe M.2"},
    {START_OPTION_EXT_EMMC,       "hard_drive with eMMC"},
    {START_OPTION_EXT_SATA,       "hard_drive with SATA"},
    {START_OPTION_EXT_USB,        "floppy/primary removable media with USB"},
};

LOCAL TASKID g_save_flags_taskID = 0;
LOCAL GAsyncQueue *g_bios_ipmi_save_info_queue = NULL;

LOCAL void bios_boot_flags_change_log(BOOT_FLAGS_PARAM_S *f_orig, BOOT_FLAGS_PARAM_S *f_new, GSList *caller_info);
LOCAL inline const gchar *transform_boot_type_to_string(guint8 boot_type);
LOCAL const gchar *transform_boot_dev_to_string(guint8 boot_dev, guint8 boot_dev_ext);
LOCAL void set_boot_option_property(guint8 flag, guint8 boot_dev, gint32 ipmi_chan);
LOCAL gchar *transform_effective_times_to_string(guint8 effective_times);

typedef struct {
    gint32 chann_num;
    TARGET_TYPE_E chann_type;
} IPMI_CHANN_INFO;


typedef struct {
    gboolean sync_to_setting;
    guint8 boot_type;
    BOOT_FLAGS_PARAM_S b_flags_orig;
    BOOT_FLAGS_PARAM_S b_flags_new;
    GSList *caller_info;
    guint8 clear_cmos;
    guint8 boot_flag;
    guint8 boot_dev;
    gint32 ipmi_chan;
} STRUCT_SAVE_BOOT_FLAGS_PARAM;


typedef struct {
    guint8 start_option_ext;   // 客户输入的BIOS启动设备或协议索引
    guint8 reg_val;            // 实际写进cpld寄存器值
} BIOS_OPTION_EXT_REG_MAP;

typedef struct {
    guint8 start_option;       // 配置的启动项
    guint8 start_option_ext;   // 配置的启动设备或启动协议
} BIOS_OPTION_EXT_MATCH;


LOCAL gint32 get_boot_order_offset(guint8 boot_format);
LOCAL gint32 bootorder_parameter_check(guint8 boot_order);
LOCAL gint32 modify_bios_change_file(const STRUCT_BIOS_CHANGE_INI_CONFIG *bios_change_array, guint8 length);
LOCAL void bios_boot_flag_ipmi_operate_log(const void *req_msg, const guint8 *boot_data,
    const IPMI_CHANN_INFO *chann_info);
LOCAL void bios_ipmi_opt_log(const void *req_msg, const guint8 *data, gint32 size);
LOCAL guint8 process_boot_flags_ipmi(const guint8 *boot_data, const IPMI_CHANN_INFO *chann_info, GSList *caller_info);
LOCAL guint8 process_progress_flag_ipmi(guint8 progress_flag);
LOCAL gint32 bios_boot_flags_generate(const BOOT_FLAGS_PARAM_S *i_flags, const IPMI_CHANN_INFO *chann,
    BOOT_FLAGS_PARAM_S *o_flags, gboolean *sync_to_setting);
LOCAL gint32 bios_save_boot_option(const STRUCT_SAVE_BOOT_FLAGS_PARAM *save_param);
LOCAL void task_per_save_boot_flags(void);

LOCAL gboolean is_bios_start_opt_valid(guint8 option)
{
    guint32 supported_option = 0;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dfl_get_object_handle(BIOS_OBJECT_NAME, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: get Bios handle failed", __FUNCTION__);
        return FALSE;
    }

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_BIOS_SUPPORTED_START_OPTION, &supported_option);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get Bios.SupportedStartOption failed", __FUNCTION__);
        return FALSE;
    }
    if (option < U32_BITS) {
        return ((((guint32)0x1 << option) & supported_option) != 0);
    }
    return FALSE;
    }

LOCAL gboolean is_bios_start_opt_ext_valid(guint8 option, guint8 optionext)
{
    guint32 supported_option_ext = 0;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = RET_ERR;
    BIOS_OPTION_EXT_MATCH bios_option_ext_match[] = {
        {NO_OVERRIDE, START_OPTION_EXT_NONE}, {FORCE_PEX, START_OPTION_EXT_TFTP},
        {FORCE_PEX, START_OPTION_EXT_HTTPS}, {FORCE_HARD_DRIVE, START_OPTION_EXT_NVMEM2},
        {FORCE_HARD_DRIVE, START_OPTION_EXT_EMMC}, {FORCE_HARD_DRIVE, START_OPTION_EXT_SATA},
        {FORCE_CD_DVD, START_OPTION_EXT_NONE}, {FORCE_FLOPPY_REMOVABLE_MEDIA, START_OPTION_EXT_USB}
    };
    for (gsize i = 0; i < G_N_ELEMENTS(bios_option_ext_match); i++) {
        if (bios_option_ext_match[i].start_option == option &&
            bios_option_ext_match[i].start_option_ext == optionext) {
            ret = RET_OK;
        }
    }
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: bios option ext not match", __FUNCTION__);
        return FALSE;
    }
    if (optionext == START_OPTION_EXT_NONE) {
        return TRUE;
    }
    ret = dfl_get_object_handle(BIOS_OBJECT_NAME, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get Bios handle failed", __FUNCTION__);
        return FALSE;
    }

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_BIOS_SUPPORTED_START_OPTION_EXT, &supported_option_ext);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get Bios.SupportedStartOptionExt failed", __FUNCTION__);
        return FALSE;
    }
    if (optionext < U32_BITS) {
        return ((((guint32)0x1 << optionext) & supported_option_ext) != 0);
    }

    return FALSE;
    }


LOCAL void bios_start_option_ext_transform(guint8 *optionext)
{
    BIOS_OPTION_EXT_REG_MAP bios_option_ext_reg_map[] = {{START_OPTION_EXT_TFTP, REG_VAL_MAP_TFTP},
        {START_OPTION_EXT_HTTPS, REG_VAL_MAP_HTTPS}, {START_OPTION_EXT_NVMEM2, REG_VAL_MAP_NVMEM2},
        {START_OPTION_EXT_EMMC, REG_VAL_MAP_EMMC}, {START_OPTION_EXT_SATA, REG_VAL_MAP_SATA},
        {START_OPTION_EXT_USB, REG_VAL_MAP_USB}, {START_OPTION_EXT_NONE, REG_VAL_MAP_DEFAULT}};

    for (gsize i = 0; i < G_N_ELEMENTS(bios_option_ext_reg_map); i++) {
        if (bios_option_ext_reg_map[i].start_option_ext == *optionext) {
            debug_log(DLOG_DEBUG, "[%s] translate option ext %d to %d reg", __FUNCTION__, *optionext,
                bios_option_ext_reg_map[i].reg_val);
            *optionext = bios_option_ext_reg_map[i].reg_val;
            return;
        }
    }
}


LOCAL gint32 bios_common_set_start_option(OBJ_HANDLE obj_handle, GSList *caller_info,
    guint8 start_option, guint8 start_option_ext)
{
    BOOT_OPTIONS_S *boot_option = &g_boot_options;
    guint32 ret_val = 0;
    guint8 start_flag = 0;
    
    boot_option->boot_flags[0] |= PBIT7;
    boot_option->boot_flags[0] &= ~PBIT6;
    
    (void)dal_get_property_value_byte(obj_handle, BIOS_START_OPTION_FLAG_NAME, &start_flag);

    if (start_flag == BIOS_START_FLAG_PERMANENT) {
        boot_option->boot_flags[0] |= PBIT6;
    }

    boot_option->boot_flags[1] = ((start_option & 0x0f) << BIOS_BOOT_OPTION_BIT2) | (boot_option->boot_flags[1] & 0xc3);
    ret_val = (guint32)per_save((guint8 *)&(boot_option->boot_flags[1]));
    ret_val |= (guint32)per_save((guint8 *)&g_boot_options);
    if (ret_val != RET_OK) {
        method_operation_log(caller_info, NULL, BIOS_OP_LOG_BOOT_DEVICE,
            transform_boot_dev_to_string(start_option, start_option_ext));
        debug_log(DLOG_ERROR, "per_save fail(Ret:%u)!", ret_val);
        return RET_ERR;
    }
    return RET_OK;
}


gint32 bios_set_start_option(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    BOOT_OPTIONS_S *boot_option = &g_boot_options;
    guint8 start_option = 0;
    gint32 ret_val = 0;

    if (input_list == NULL) {
        method_operation_log(caller_info, NULL, "Set boot device failed");
        
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    start_option = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));

    debug_log(DLOG_DEBUG, "start_option:%d.", start_option);

    if (!is_bios_start_opt_valid(start_option)) {
        method_operation_log(caller_info, NULL, BIOS_OP_LOG_BOOT_DEVICE,
            transform_boot_dev_to_string(start_option, START_OPTION_EXT_NONE));
        
        debug_log(DLOG_ERROR, "Param start_ption:%d is invalid!", start_option);
        return BIOS_ERROR_INVALID_INPUT_PARA;
    }
    debug_log(DLOG_INFO, "BootFlag:%d,ClearCmos:%d,StartOption:%d", (boot_option->boot_flags[0] & PBIT7),
        (boot_option->boot_flags[1] & PBIT7), start_option);

    ret_val = bios_common_set_start_option(obj_handle, caller_info, start_option, START_OPTION_EXT_NONE);
    if (ret_val != RET_OK) {
        return RET_ERR;
    }
    
    set_boot_option_property(((boot_option->boot_flags[1] & PBIT7) ? BMC_DISABLE : BMC_ENABLE), start_option,
        INVALID_CHAN_NUM);
    set_boot_option_valid(BIOS_BOOT_FLAGS_CMD, BIOS_BOOT_OPTION_VALID);
    (void)dal_set_property_value_byte(obj_handle, BIOS_START_OPTION_NAME_EXT, REG_VAL_MAP_DEFAULT, DF_SAVE | DF_AUTO);

    method_operation_log(caller_info, NULL, "Set boot device to (%s) successfully",
        transform_boot_dev_to_string(start_option, START_OPTION_EXT_NONE));

    return RET_OK;
}


gint32 bios_set_start_option_ext(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    BOOT_OPTIONS_S *boot_option = &g_boot_options;
    gint32 ret_val = 0;

    if (input_list == NULL) {
        method_operation_log(caller_info, NULL, "Set boot device failed");
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    if (g_slist_length(input_list) != 2) {  // 入参个数必须为2
        debug_log(DLOG_ERROR, "%s: failed with input_list length is not 2", __FUNCTION__);
        return RET_ERR;
    }

    guint8 start_option = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    guint8 start_option_ext = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1));
    guint8 start_option_ext_bak = start_option_ext;

    debug_log(DLOG_DEBUG, "start_option:%d,start_option_ext:%d", start_option, start_option_ext);

    if (!is_bios_start_opt_valid(start_option) || !is_bios_start_opt_ext_valid(start_option, start_option_ext)) {
        method_operation_log(caller_info, NULL, BIOS_OP_LOG_BOOT_DEVICE,
            transform_boot_dev_to_string(start_option, start_option_ext));
        debug_log(DLOG_ERROR, "Param start_ption:%d or Param start_ption_ext:%d is invalid!",
            start_option, start_option_ext);
        return BIOS_ERROR_INVALID_INPUT_PARA;
    }

    debug_log(DLOG_INFO, "BootFlag:%d,ClearCmos:%d,StartOption:%d,StartOptionExt:%d",
        (boot_option->boot_flags[0] & PBIT7), (boot_option->boot_flags[1] & PBIT7), start_option, start_option_ext);

    ret_val = bios_common_set_start_option(obj_handle, caller_info, start_option, start_option_ext);
    if (ret_val != RET_OK) {
        return RET_ERR;
    }

    
    
    set_boot_option_property(((boot_option->boot_flags[1] & PBIT7) ? BMC_DISABLE : BMC_ENABLE), start_option,
        INVALID_CHAN_NUM);
    set_boot_option_valid(BIOS_BOOT_FLAGS_CMD, BIOS_BOOT_OPTION_VALID);
    bios_start_option_ext_transform(&start_option_ext);
    (void)dal_set_property_value_byte(obj_handle, BIOS_START_OPTION_NAME_EXT, start_option_ext, DF_SAVE | DF_AUTO);

    method_operation_log(caller_info, NULL, "Set boot device to (%s) successfully",
        transform_boot_dev_to_string(start_option, start_option_ext_bak));

    return RET_OK;
}


LOCAL const gchar *transform_boot_dev_to_string(guint8 boot_dev, guint8 boot_dev_ext)
{
    if (boot_dev_ext == START_OPTION_EXT_NONE) {
        for (gsize i = 0; i < G_N_ELEMENTS(g_boot_option_arr); i++) {
            if (g_boot_option_arr[i].attr_id == boot_dev) {
                return g_boot_option_arr[i].attr_name;
            }
        }
    } else {
        for (gsize i = 0; i < G_N_ELEMENTS(g_boot_option_ext_arr); i++) {
            if (g_boot_option_ext_arr[i].attr_id == boot_dev_ext) {
                return g_boot_option_ext_arr[i].attr_name;
            }
        }
    }

            return "unspecified";
}


LOCAL void set_boot_option_property(guint8 flag, guint8 boot_dev, gint32 ipmi_chan)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret_val = 0;
    GVariant *property_data = NULL;
    guchar start_option = 0;

    ret_val = dfl_get_binded_object(BIOS_CLASS_NAME, BIOS_OBJECT_HANDLE_NAME, &obj_handle);
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return;
    }

    
    
    if ((BMC_DISABLE == flag) && (SYS_CHAN_NUM == ipmi_chan)) {
        (void)dal_get_property_value_byte(obj_handle, BIOS_START_OPTION_NAME, &start_option);
        (void)dal_set_property_value_byte(obj_handle, BIOS_LAST_START_OPTION_NAME, start_option, DF_NONE);
    }

    

    
    
    if (BMC_DISABLE == flag) {
        property_data = g_variant_new_byte(flag);
        
        ret_val = dfl_set_property_value(obj_handle, BIOS_START_OPTION_FLAG_NAME, property_data, DF_SAVE);

        
        
        
        (void)dal_set_property_value_byte(obj_handle, BIOS_START_OPTION_FLAG_EXT_NAME, flag, DF_SAVE);

        
        if (ret_val != DFL_OK) {
            g_variant_unref(property_data);

            
            debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
            return;
        }

        g_variant_unref(property_data);
    }

    
    
    property_data = g_variant_new_byte(boot_dev);
    
    ret_val = dfl_set_property_value(obj_handle, BIOS_START_OPTION_NAME, property_data, DF_SAVE | DF_AUTO);
    
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
    }

    g_variant_unref(property_data);

    return;
}


void set_boot_option_valid(guint8 selector, guint8 valid)
{
    g_boot_options.valid_flag[selector / BIOS_VALID_FLAG_INTREVAL] &=
        ~((guint8)(1 << (selector % BIOS_VALID_FLAG_INTREVAL)));
    g_boot_options.valid_flag[selector / BIOS_VALID_FLAG_INTREVAL] |=
        (guint8)(valid << (selector % BIOS_VALID_FLAG_INTREVAL));
}


gint32 bios_set_start_option_flag(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    guint8 effective_times = 0; // 0 :不生效一次生效 1:永久生效
    gint32 ret_val = 0;

    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "Set bios_start_option_flag failed, input_list can not be NULL.\r\n");
        return RET_ERR;
    }
    
    effective_times = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    debug_log(DLOG_DEBUG, "effective_times:%d.\r\n", effective_times);

    ret_val = modify_boot_options_boot_flag(effective_times);
    if (ret_val != RET_OK) {
        method_operation_log(caller_info, NULL, "Set boot device effective mode (%s) failed\r\n",
            transform_effective_times_to_string(effective_times));
        
        debug_log(DLOG_ERROR, "per_save fail(Ret:%d)!\n", ret_val);
        return RET_ERR;
    }

    (void)dal_set_property_value_byte(obj_handle, BIOS_START_OPTION_FLAG_NAME, effective_times, DF_SAVE);
    
    
    (void)dal_set_property_value_byte(obj_handle, BIOS_START_OPTION_FLAG_EXT_NAME, effective_times, DF_SAVE);
    

    set_boot_option_valid(BIOS_BOOT_FLAGS_CMD, BIOS_BOOT_OPTION_VALID);
    method_operation_log(caller_info, NULL, "Set boot device effective mode (%s) successfully",
        transform_effective_times_to_string(effective_times));

    return RET_OK;
}



LOCAL gchar *transform_effective_times_to_string(guint8 effective_times)
{
    switch (effective_times) {
        case EFFECTIVE_ONCE:
            return "once";

        case EFFECTIVE_FOREVER:
            return "permanent";

        default:
            return "unspecified";
    }
}


gint32 modify_boot_options_boot_flag(guint8 effective_times)
{
    BOOT_OPTIONS_S *boot_option = &g_boot_options;
    gint32 ret_val = 0;

    if ((effective_times != EFFECTIVE_NONE) && (effective_times != EFFECTIVE_ONCE) &&
        (effective_times != EFFECTIVE_FOREVER)) {
        debug_log(DLOG_ERROR, "Param effective_times:%d is invalid!\n", effective_times);
        return RET_ERR;
    }

    if (effective_times == EFFECTIVE_NONE) {
        boot_option->boot_flags[0] &= ~PBIT7;
    } else if (EFFECTIVE_FOREVER == effective_times) {
        boot_option->boot_flags[0] |= PBIT7;
        boot_option->boot_flags[0] |= PBIT6;
    } else {
        boot_option->boot_flags[0] |= PBIT7;
        boot_option->boot_flags[0] &= ~PBIT6;
    }

    ret_val = per_save((guint8 *)&g_boot_options);

    return ret_val;
}



gint32 bios_set_start_option_flag_ext(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    guint8 effective_times = 0;
    guint8 start_opt_flag_ext = 0;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "Set bios_start_option_flag failed, input_list can not be NULL.\r\n");
        return RET_ERR;
    }

    effective_times = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    (void)dal_get_property_value_byte(obj_handle, BIOS_START_OPTION_FLAG_EXT_NAME, &start_opt_flag_ext);
    if (effective_times == start_opt_flag_ext) {
        return RET_OK;
    }

    
    return bios_set_start_option_flag(obj_handle, caller_info, input_list, output_list);
}



gint32 method_set_boot_order(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    STRUCT_BIOS_CHANGE_INI_CONFIG boot_order_array[BIOS_BOOT_DEVICE_NUM];
    gint32 ret = RET_ERR;
    guint8 i = 0;
    guint8 boot_type = 0;
    guint8 boot_type_num = 0;
    gsize parm_temp = 0;

    gchar boot_type_array[BIOS_BOOT_DEVICE_NUM][CONFIG_VER_LEN] =
    {
        "HDD", "CD/DVD", "PXE", "OTHER"
    };

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "Input parameter is error \r\n");
        return RET_ERR;
    }

    const guint8 *boot_order = (const guint8 *)g_variant_get_fixed_array((GVariant *)g_slist_nth_data(input_list, 0),
        &parm_temp, sizeof(guint8));
    guint32 input_len = (guint32)parm_temp;

    
    if (boot_order == NULL) {
        return RET_ERR;
    }

    
    if (input_len != BIOS_BOOT_DEVICE_NUM) {
        debug_log(DLOG_ERROR, "bootorder parameter is error\r\n");
        return RET_ERR;
    }

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_BOIS_ORDER_FORMAT, &boot_type);

    boot_type_num =
        (guint8)(sizeof(g_boot_type_order_array) / (sizeof(STRUCT_BIOS_CHANGE_INI_CONFIG) * BIOS_BOOT_DEVICE_NUM));

    // 当前V5 、其它产品不支持设置
    if (boot_type >= boot_type_num) {
        debug_log(DLOG_DEBUG, "crurrent product don't support modify bootorder \r\n");
        return RET_ERR;
    }

    // 获取boot_order 中的offset值
    
    ret = get_boot_order_offset(boot_type);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, " check boot_order offset failed \r\n");
    }

    
    (void)memset_s(boot_order_array, sizeof(boot_order_array), 0x00, sizeof(boot_order_array));
    for (i = 0; i < input_len; i++) {
        ret = bootorder_parameter_check(boot_order[i]);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "bootorder parameter check failed!! \r\n");
            return RET_ERR;
        }

        // 查找表格中的数据进行匹配, 目的缓冲区大小为CONFIG_VER_LEN，无需判断返回值
        (void)memcpy_s(boot_order_array[i].boot_item, sizeof(boot_order_array[i].boot_item),
            g_boot_type_order_array[BIOS_BOOT_DEVICE_NUM * boot_type + i].boot_item, CONFIG_VER_LEN);
        boot_order_array[i].value = g_boot_type_order_array[BIOS_BOOT_DEVICE_NUM * boot_type + boot_order[i]].value;
        boot_order_array[i].attribute =
            g_boot_type_order_array[BIOS_BOOT_DEVICE_NUM * boot_type + boot_order[i]].attribute;
        boot_order_array[i].offset = g_boot_type_order_array[BIOS_BOOT_DEVICE_NUM * boot_type + i].offset;
        boot_order_array[i].width = g_boot_type_order_array[BIOS_BOOT_DEVICE_NUM * boot_type + boot_order[i]].width;
    }

    // 实现changed0.ini 配置文件的修改
    ret = modify_bios_change_file(boot_order_array, input_len);
    if (ret == RET_ERR) {
        debug_log(DLOG_ERROR, "Modify changed0.ini file failed!! \r\n");
        method_operation_log(caller_info, NULL, "Set Syetem BootOrder( %s:%s:%s:%s) Failed",
            boot_type_array[boot_order[0]], boot_type_array[boot_order[1]], boot_type_array[boot_order[2]],
            boot_type_array[boot_order[3]]);
        return RET_ERR;
    }

    method_operation_log(caller_info, NULL, "Set Syetem BootOrder( %s:%s:%s:%s) Successfully",
        boot_type_array[boot_order[0]], boot_type_array[boot_order[1]], boot_type_array[boot_order[2]],
        boot_type_array[boot_order[3]]);
    return ret;
}


LOCAL gint32 get_boot_order_offset(guint8 boot_format)
{
    guint8 i = 0;
    FM_FILE_S *fp = NULL;
    gchar **p = NULL;
    gchar linebuffer[MAX_BIOS_FILE_LINE_LEN] = {0};

    for (i = 0; i < BIOS_BOOT_DEVICE_NUM; i++) {
        fp = fm_fopen(BIOS_FILE_OPTION_NAME, "r");
        if (fp == NULL) {
            debug_log(DLOG_ERROR, " fopen(%s) fail ! ", BIOS_FILE_OPTION_NAME);
            return RET_ERR;
        }

        while (!feof(fp->fd) && fm_fgets(linebuffer, MAX_BIOS_FILE_LINE_LEN, fp) != NULL) {
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
                debug_log(DLOG_ERROR, " g_strsplit(%s) error ....", linebuffer);
                continue;
            }

            if ((g_strv_length(p) != 5) || (p[0] == NULL) || (p[1] == NULL)) {
                debug_log(DLOG_ERROR, " g_strsplit(%s) error ....", linebuffer);
                g_strfreev(p);
                p = NULL;
                continue;
            }

            // 从option0.ini文件中获取启动顺序的关键字，获取到offset值
            if (strcmp(p[0], g_boot_type_order_array[BIOS_BOOT_DEVICE_NUM * boot_format + i].boot_item) == 0) {
                g_boot_type_order_array[BIOS_BOOT_DEVICE_NUM * boot_format + i].offset = (guint8)strtol(p[1], NULL, 10);
            }

            g_strfreev(p);
            p = NULL;
        }

        fm_fclose(fp);
    }

    return RET_OK;
}


LOCAL gint32 modify_bios_change_file(const STRUCT_BIOS_CHANGE_INI_CONFIG *bios_change_array, guint8 length)
{
    gint32 iRet = -1;
    guint8 i = 0;
    gint32 ret_file_exist;
    gchar buf[MAX_BIOS_FILE_LINE_LEN + 1] = {0x00};
    FM_FILE_S *fp = NULL;
    gint32 ret = 0;
    gchar **p = NULL;
    gchar linebuffer[MAX_BIOS_FILE_LINE_LEN] = {0};
    size_t delete_length = 0;
    guint32 offset = 0;
    guchar arm_enable = 0;
    guint32 product_version = 0;

    if (length != BIOS_BOOT_DEVICE_NUM || bios_change_array == NULL) {
        debug_log(DLOG_ERROR, "Input parameter is error ! \r\n");
        return RET_ERR;
    }

    ret_file_exist = g_access(BIOS_CHANGE_INI_FILE_NAME, F_OK);
    
    if (ret_file_exist == RET_ERR) {
        fp = fm_fopen(BIOS_FILE_CHANGE_NAME, "w");
        if (fp == NULL) {
            debug_log(DLOG_ERROR, " fopen(%s) fail !", BIOS_CHANGE_INI_FILE_NAME);
            return RET_ERR;
        }

        for (i = 0; i < BIOS_BOOT_DEVICE_NUM; i++) {
            iRet = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "%s    %d    %d    %d    %d\r\n",
                bios_change_array[i].boot_item, bios_change_array[i].offset, bios_change_array[i].width,
                bios_change_array[i].value, bios_change_array[i].attribute);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
            }
            (void)fm_fwrite(buf, strlen(buf), 1, fp);
        }

        fm_fclose(fp);
        return RET_OK;
    }

    // changed0.ini文件存在
    else {
        // 启动顺序依次写入
        for (i = 0; i < BIOS_BOOT_DEVICE_NUM; i++) {
            fp = fm_fopen(BIOS_FILE_CHANGE_NAME, "r+");
            if (fp == NULL) {
                debug_log(DLOG_ERROR, " fopen(%s) fail ! ", BIOS_CHANGE_INI_FILE_NAME);
                return RET_ERR;
            }

            while (!feof(fp->fd) && fm_fgets(linebuffer, MAX_BIOS_FILE_LINE_LEN, fp) != NULL) {
                if (strlen(linebuffer) < 2) {
                    continue;
                }

                if (linebuffer[0] == '/') {
                    continue;
                }

                if (linebuffer[0] == '[') {
                    continue;
                }

                delete_length = strlen(linebuffer);

                p = g_strsplit(linebuffer, "    ", 5);
                if (p == NULL) {
                    debug_log(DLOG_ERROR, " g_strsplit(%s) error ....", linebuffer);
                    continue;
                }

                // 从changed0.ini文件中获取启动顺序的关键字，并删除该行信息
                if (strcmp(p[0], bios_change_array[i].boot_item) == 0) {
                    offset = fm_ftell(fp);
                    ret = fm_delete(delete_length, offset - delete_length, fp);
                    if (ret == RET_ERR) {
                        debug_log(DLOG_ERROR, "delete line content failed !\r\n");
                    }
                }

                g_strfreev(p);
                p = NULL;
            }

            if (feof(fp->fd)) {
                iRet = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "%s    %d    %d    %d    %d\r\n",
                    bios_change_array[i].boot_item, bios_change_array[i].offset, bios_change_array[i].width,
                    bios_change_array[i].value, bios_change_array[i].attribute);
                if (iRet <= 0) {
                    debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
                }
                (void)fm_fwrite(buf, strlen(buf), 1, fp);
                (void)memset_s(buf, sizeof(buf), 0x00, sizeof(buf));
            }

            fm_fclose(fp);
        }
    }

    set_bios_setting_file_change_flag(BIOS_SETTING_FILE_CHANGED, 2);

    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);
    (void)dal_get_product_version_num(&product_version);
    if ((product_version <= PRODUCT_VERSION_V5) && (arm_enable == ENABLE)) {
        
        ret = fm_clear_file(BIOS_FILE_SETTING_NAME);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: fm_clear_file %s failed", __FUNCTION__, BIOS_FILE_CHANGE_NAME);
        }

        g_bios_config_format = 0;
        per_save((guint8 *)&g_bios_config_format);
        debug_log(DLOG_DEBUG, "BIOS config is ini format.\r\n");
    }
    

    return RET_OK;
}


LOCAL gint32 bootorder_parameter_check(guint8 boot_order)
{
    if ((BOOT_ORDER_TPYE_HDD != boot_order) && (BOOT_ORDER_TPYE_CD != boot_order) &&
        (BOOT_ORDER_TPYE_PXE != boot_order) && (BOOT_ORDER_TPYE_OTHERS != boot_order)) {
        debug_log(DLOG_ERROR, "bootorder type is error\r\n");
        return RET_ERR;
    }

    return RET_OK;
}


void set_bios_setting_file_change_flag(guint8 flag, guint8 channel)
{
    g_bios_file_change[0].file_change_flag = flag;
    g_bios_file_change[0].file_channel = channel;

    per_save((guint8 *)g_bios_file_change);

    
    if (flag == BIOS_SETTING_FILE_CHANGED) {
        g_bios_setting_file_state = BIOS_SETTING_FILE_INEFFECTIVE;
    }

    return;
}


gint32 bios_set_boot_mode(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret_val = 0;
    BOOT_OPTIONS_S boot_option;
    guint8 boot_mode = 0;
    guint8 boot_mode_support_flag = 0;

    if (input_list == NULL) {
        method_operation_log(caller_info, NULL, "Set boot mode failed.\r\n");
        
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    
    (void)dal_get_property_value_byte(obj_handle, BIOS_BOOT_MODE_SUPPORT_FLAG, &boot_mode_support_flag);

    if (boot_mode_support_flag != BOOT_MODE_SUPPORT_FLAG_ENABLE) {
        method_operation_log(caller_info, NULL, "Set boot mode failed.\r\n");
        
        debug_log(DLOG_ERROR, "Set boot mode failed,this board is not support.boot_mode_support_flag=0x%x\r\n",
            boot_mode_support_flag);
        return RET_ERR;
    }

    

    
    
    boot_mode = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    ret_val = write_boot_mode_to_setting_json(boot_mode);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "%s : Failed to write boot mode to setting.json, ret_val = %d", __FUNCTION__, ret_val);
        method_operation_log(caller_info, NULL, "Set boot mode (%s) failed",
            (BIOS_BOOT_LEGACY == boot_mode) ? "LEGACY" : "UEFI");
        return RET_ERR;
    }

    (void)memcpy_s(&boot_option, sizeof(BOOT_OPTIONS_S), &g_boot_options, sizeof(BOOT_OPTIONS_S));
    

    debug_log(DLOG_DEBUG, "BootFlag:%d, ClearCmos:%d, boot_mode:%d\n", (boot_option.boot_flags[0] & PBIT7),
        (boot_option.boot_flags[0] & PBIT5), boot_mode);

    
    boot_option.boot_flags[0] |= PBIT7;

    if (boot_mode == BIOS_BOOT_LEGACY) {
        boot_option.boot_flags[0] &= NBIT5;
    } else if (BIOS_BOOT_UEFI == boot_mode) {
        boot_option.boot_flags[0] |= PBIT5;
    } else {
        method_operation_log(caller_info, NULL, "Set boot mode failed.\r\n");
        
        debug_log(DLOG_ERROR, "Set boot mode failed, boot_mode=0x%x\r\n", boot_mode);
        return RET_ERR;
    }

    (void)dal_set_property_value_byte(obj_handle, BIOS_BOOT_MODE, boot_mode, DF_SAVE);

    
    
    (void)memcpy_s(&g_boot_options, sizeof(BOOT_OPTIONS_S), &boot_option, sizeof(BOOT_OPTIONS_S));
    ret_val = per_save((guint8 *)&g_boot_options);
    if (ret_val != RET_OK) {
        method_operation_log(caller_info, NULL, "Set boot mode (%s) failed",
            (BIOS_BOOT_LEGACY == boot_mode) ? "LEGACY" : "UEFI");
        
        debug_log(DLOG_ERROR, "per_save fail(Ret:%d)!\n", ret_val);
        return RET_ERR;
    }

    

    set_boot_option_valid(BIOS_BOOT_FLAGS_CMD, BIOS_BOOT_OPTION_VALID);

    method_operation_log(caller_info, NULL, "Set boot mode (%s) successfully",
        (BIOS_BOOT_LEGACY == boot_mode) ? "LEGACY" : "UEFI");

    return ret_val;
}


gint32 write_boot_mode_to_setting_json(guint8 boot_mode)
{
    gint32 ret = RET_ERR;
    FM_FILE_S *file_handle = NULL;
    json_object *input_json = NULL;
    gint32 file_size = 0;
    guint32 file_actual_size = 0;
    gint32 fm_fseek_back = 1;
    gchar *data_buf = NULL;
    const gchar *content = NULL;

    
    if (TRUE != vos_get_file_accessible(BIOS_FILE_SETTING_PATH)) {
        debug_log(DLOG_DEBUG, "%s : file don't exist", __FUNCTION__);
        return RET_OK;
    }

    

    
    file_handle = fm_fopen(BIOS_FILE_SETTING_NAME, "r");
    if (file_handle == NULL) {
        debug_log(DLOG_ERROR, "%s : Failed to open file %s", __FUNCTION__, BIOS_FILE_SETTING_NAME);
        goto err_exit;
    }

    
    fm_fseek_back = fm_fseek(file_handle, 0, SEEK_END);
    if (fm_fseek_back != 0) {
        debug_log(DLOG_ERROR, "%s: file seek failed", __FUNCTION__);
        goto err_exit;
    }

    file_size = fm_ftell(file_handle);
    if (file_size < 0) {
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

    if (file_size >= 0) {
        file_actual_size = fm_fread(data_buf, BIOS_FILE_STREAM_SIZE, (guint32)file_size, file_handle);
        if (file_size != file_actual_size) {
            debug_log(DLOG_ERROR, "%s : read file(InLen:%d,OutLen:%d) failed", __FUNCTION__, file_size,
                file_actual_size);
            goto err_exit;
        }
    }

    
    fm_fclose(file_handle);
    

    
    file_handle = NULL;
    

    
    if (strlen(data_buf) == 0) {
        input_json = json_object_new_object();
    } else {
        input_json = json_tokener_parse(data_buf);
    }

    if (input_json == NULL) {
        debug_log(DLOG_ERROR, "%s : Failed to call json_tokener_parse", __FUNCTION__);
        goto err_exit;
    }

    
    if (boot_mode == BIOS_BOOT_UEFI) {
        json_object_object_add(input_json, BIOS_BOOT_TYPE_STR, json_object_new_string(BIOS_UEFI_BOOT_STR));
    } else if (boot_mode == BIOS_BOOT_LEGACY) {
        json_object_object_add(input_json, BIOS_BOOT_TYPE_STR, json_object_new_string(BIOS_LEGACY_BOOT_STR));
    } else {
        debug_log(DLOG_ERROR, "%s : boot mode is out of range, boot_mode = %d", __FUNCTION__, boot_mode);
        goto err_exit;
    }

    
    content = dal_json_object_get_str(input_json);
    if (content == NULL || strlen(content) == 0) {
        debug_log(DLOG_ERROR, "%s : Failed to call dal_json_object_get_str", __FUNCTION__);
        goto err_exit;
    }

    
    
    file_handle = fm_fopen(BIOS_FILE_SETTING_NAME, "w");
    if (file_handle == NULL) {
        debug_log(DLOG_ERROR, "%s : Failed to open file %s", __FUNCTION__, BIOS_FILE_SETTING_NAME);
        goto err_exit;
    }
    

    
    fm_fseek_back = fm_fseek(file_handle, 0, SEEK_SET);
    if (fm_fseek_back != 0) {
        debug_log(DLOG_ERROR, "%s: file seek %s failed", __FUNCTION__, BIOS_FILE_RESULT_NAME);
        goto err_exit;
    }

    file_actual_size = fm_fwrite(content, BIOS_FILE_STREAM_SIZE, strlen(content), file_handle);
    if (file_actual_size != strlen(content)) {
        debug_log(DLOG_ERROR, "%s : Failed to write file %s, file_actual_size = %d, strlen(content) = %zu",
            __FUNCTION__, BIOS_FILE_SETTING_NAME, file_actual_size, strlen(content));
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



gint32 bios_set_boot_mode_sw(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    guint8 boot_mode_sw = 0;
    guint8 boot_mode_support_flag = 0;

    if (input_list == NULL) {
        method_operation_log(caller_info, NULL, "Failed to set Boot Mode Configuration Over IPMI\r\n");
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    (void)dal_get_property_value_byte(obj_handle, BIOS_BOOT_MODE_SUPPORT_FLAG, &boot_mode_support_flag);

    if (boot_mode_support_flag != BOOT_MODE_SUPPORT_FLAG_ENABLE) {
        method_operation_log(caller_info, NULL, "Failed to set Boot Mode Configuration Over IPMI\r\n");
        debug_log(DLOG_ERROR, "Set boot mode failed,this board is not support.boot_mode_support_flag=0x%x\r\n",
            boot_mode_support_flag);
        return RET_ERR;
    }

    boot_mode_sw = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    if ((boot_mode_sw != BIOS_BOOT_MODE_SW_ON) && (boot_mode_sw != BIOS_BOOT_MODE_SW_OFF)) {
        method_operation_log(caller_info, NULL, "Failed to set Boot Mode Configuration Over IPMI\r\n");
        debug_log(DLOG_ERROR, "parameter is error.boot_mode_sw=0x%x\r\n", boot_mode_sw);
        return RET_ERR;
    }

    (void)dal_set_property_value_byte(obj_handle, BIOS_BOOT_MODE_SW, boot_mode_sw, DF_SAVE);

    method_operation_log(caller_info, NULL, "Set Boot Mode Configuration Over IPMI to %s successfully",
        (BIOS_BOOT_MODE_SW_ON == boot_mode_sw) ? "ON" : "OFF");

    return RET_OK;
}



gint32 bios_set_boot_mode_sw_enable(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    guint8 boot_mode_ipmi_sw_enable = 0;

    if (input_list == NULL) {
        method_operation_log(caller_info, NULL, "Failed to set Boot Mode Switch Enable Over IPMI\r\n");
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    boot_mode_ipmi_sw_enable = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    if ((boot_mode_ipmi_sw_enable != BIOS_BOOT_MODE_SW_ENABLE_ON) &&
        (boot_mode_ipmi_sw_enable != BIOS_BOOT_MODE_SW_ENABLE_OFF)) {
        method_operation_log(caller_info, NULL, "Failed to set Boot Mode Switch Enable Over IPMI\r\n");
        debug_log(DLOG_ERROR, "parameter is error.boot_mode_ipmi_sw_enable=0x%x\r\n", boot_mode_ipmi_sw_enable);
        return RET_ERR;
    }

    (void)dal_set_property_value_byte(obj_handle, BIOS_BOOT_MODE_SW_ENABLE, boot_mode_ipmi_sw_enable, DF_SAVE);

    method_operation_log(caller_info, NULL, "Set Boot Mode Switch Enable Over IPMI to %s successfully",
        (BIOS_BOOT_MODE_SW_ENABLE_ON == boot_mode_ipmi_sw_enable) ? "ON" : "OFF");

    return RET_OK;
}


gint32 method_get_boot_order(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    guint8 boot_type = 0;
    guint8 boot_type_num = 0;
    guint8 boot_order[BIOS_BOOT_DEVICE_NUM] = {0};
    GVariant *property = NULL;
    gint32 ret = 0;
    gint32 i = 0;
    FM_FILE_S *fp = NULL;
    gchar linebuffer[MAX_BIOS_FILE_LINE_LEN] = {0};
    gchar **split = NULL;
    guint8 value = 0;
    gint32 j = 0;

    // 入参检测
    if (output_list == NULL) {
        debug_log(DLOG_ERROR, "Input parameter is error \r\n");
        return RET_ERR;
    }

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_BOIS_ORDER_FORMAT, &boot_type);
    boot_type_num = (guint8)(ARRAY_SIZE(g_boot_type_order_array) / BIOS_BOOT_DEVICE_NUM);

    // 当前V5 、其它产品不支持查询启动顺序
    if (boot_type >= boot_type_num) {
        debug_log(DLOG_DEBUG, "crurrent product don't support modify bootorder!! \r\n");
        return RET_ERR;
    }

    // 读文件获取启动顺序
    ret = vos_get_file_accessible(BIOS_OPTIONS0_INI_FILE_NAME);
    if (!ret) {
        debug_log(DLOG_DEBUG, "BIOS configuration file (%s) do not exist.\r\n", BIOS_OPTIONS0_INI_FILE_NAME);
        return RET_ERR;
    }

    fp = fm_fopen(BIOS_FILE_OPTION_NAME, "r");
    if (fp == NULL) {
        debug_log(DLOG_DEBUG, "open BIOS configuration file (%s) fail.\r\n", BIOS_OPTIONS0_INI_FILE_NAME);
        return RET_ERR;
    }

    while (!feof(fp->fd) && fm_fgets(linebuffer, MAX_BIOS_FILE_LINE_LEN, fp) != NULL) {
        if (g_str_has_prefix((const gchar *)linebuffer, "BootTypeOrder") ||
            g_str_has_prefix((const gchar *)linebuffer, "LegacyPriorities")) {
            split = g_strsplit(linebuffer, "    ", 0);
            if (split == NULL) {
                debug_log(DLOG_ERROR, "splited string fail.\r\n");
                continue;
            }

            if (g_strv_length(split) < 4) {
                debug_log(DLOG_ERROR, "wrong splited string number.\r\n");
                g_strfreev(split);
                continue;
            }

            ret = vos_str2int((const gchar *)split[3], 10, &value, NUM_TPYE_UCHAR);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: vos_str2int fail.\r\n", __FUNCTION__);
                g_strfreev(split);
                continue;
            }

            for (j = 0; j < BIOS_BOOT_DEVICE_NUM; j++) {
                // 根据文件中读到的值，在数组里面查找
                if (g_boot_type_order_array[BIOS_BOOT_DEVICE_NUM * boot_type + j].value == value) {
                    boot_order[i] = j;
                    i++;

                    break;
                }
            }

            g_strfreev(split);
            split = NULL;
        }
    }

    fm_fclose(fp);

    // 启动顺序的数量不为4，返回错误
    if (BIOS_BOOT_DEVICE_NUM != i) {
        debug_log(DLOG_ERROR, "%s: invalid boot device number is %d.\r\n", __FUNCTION__, i);
        return RET_ERR;
    }

    property = g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, boot_order, BIOS_BOOT_DEVICE_NUM, sizeof(guint8));

    *output_list = g_slist_append(*output_list, property);

    return RET_OK;
}


gint32 bios_set_system_startup_state(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    GVariant *list_value = NULL;
    guint8 stage = 0;
    gint32 result = RET_OK;
    OBJ_HANDLE handle = 0;

    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input_list is NULL", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Set system startup state failed.\r\n");
        return RET_ERR;
    }

    list_value = (GVariant *)g_slist_nth_data(input_list, 0);
    if (list_value == NULL) {
        debug_log(DLOG_ERROR, "%s:list_value is NULL", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Set system startup state failed.\r\n");
        return RET_ERR;
    }

    stage = g_variant_get_byte(list_value);

    result = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &handle);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Get BIOS object handle fail.", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Set system startup state failed.\r\n");
        return RET_ERR;
    }

    result = dal_set_property_value_byte(handle, PROPERTY_BIOS_STARTUP_STATE, stage, DF_SAVE_TEMPORARY);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Set BIOS boot stage fail.", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Set system startup state failed.\r\n");
        return RET_ERR;
    }

    method_operation_log(caller_info, NULL, "Set system startup state(%d) successfully.\r\n", stage);

    return RET_OK;
}


gint32 bios_set_system_startup_state_default_value(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    guint8 iflag = 0;
    guint32 value = 1;

    static gulong async_sem_1 = 0;

    if (vos_get_file_accessible(TMP_BIOS_CLPCONFIG_INI_FILE_NAME) == TRUE) {
        
        debug_log(DLOG_INFO, "%s: delete tmp config file ok.\n", __FUNCTION__);
        (void)vos_rm_filepath(TMP_BIOS_CLPCONFIG_INI_FILE_NAME);
    }

    if (async_sem_1 == 0) {
        (void)vos_thread_binary_sem4_create(&async_sem_1, "biorst", 1UL);
    }

    value = g_variant_get_uint32(property_value);
    debug_log(DLOG_INFO, "%s: bios_set_system_startup_state_default_value, value = %08x\n", __FUNCTION__, value);
    iflag = (value & 0x01000000) ? 1 : 0;

    if (iflag == 1) {
        if (vos_thread_async_sem4_p(async_sem_1) == 0) {
            update_bios_startup_state(BIOS_STARTUP_STATE_OFF);

            (void)vos_thread_sem4_v(async_sem_1);
        }
    }

    return RET_OK;
}


gint32 bios_set_system_boot_options(const void *req_msg, gpointer user_data)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret = 0;
    gint32 valid = 0;
    gint32 para_size = 0;
    const guint8 *para_data = NULL;
    BOOT_OPTIONS_S *boot_option = &g_boot_options;
    const guint8 *src_data = NULL;
    guint8 comp_code = COMP_CODE_SUCCESS;
    guint8 sub_cmd = 0;
    gint32 ipmi_ret = RET_ERR;
    IPMI_CHANN_INFO chann_info = { 0, IPMI_HOST };
    GSList *caller_info = NULL;
    guint8 data_len;
    if (req_msg == NULL) {
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    chann_info.chann_num = get_ipmi_chan_num(req_msg);
    chann_info.chann_type = get_ipmi_target_type(req_msg);

    src_data = get_ipmi_src_data(req_msg);
    data_len = get_ipmi_src_data_len(req_msg);
    
    if (src_data == NULL) {
        ipmi_operation_log(req_msg, BIOS_OP_LOG_SYS_BOOT);
        debug_log(DLOG_ERROR, DB_STR_GET_SRC_DATA_FAIL);
        ipmi_ret = ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
        return ipmi_ret;
    }
    

    para_data = &(src_data[1]);

    valid = (src_data[0] & PBIT7) >> BIOS_BOOT_OPTION_BIT;
    sub_cmd = src_data[0] & BIOS_SUB_CMD_VALID_BIT;

    set_boot_option_valid(sub_cmd, valid);

    debug_log(DLOG_DEBUG, "SubCmd:%d Valid:%d!\n", sub_cmd, valid);
    
    
    switch (sub_cmd) {
        case BIOS_BOOT_OEM_WRITE_PROT_CMD:
            ret = ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
            break;

        case BIOS_BOOT_PROGRESS_CMD:
            
            comp_code = process_progress_flag_ipmi(para_data[0] & 3);
            ret = ipmi_send_simple_response(req_msg, comp_code);
            break;

        case BIOS_BOOT_FLAGS_CMD:
            if (get_ipmi_src_data_len(req_msg) < BIOS_BOOT_VALID_NUM) {
                return ipmi_send_simple_response(req_msg, COMP_CODE_LEN_INVALID);
            }
            
            caller_info = ipmimsg_to_callerinfo(req_msg);
            comp_code = process_boot_flags_ipmi(para_data, &chann_info, caller_info);
            ret = ipmi_send_simple_response(req_msg, comp_code);
            bios_boot_flag_ipmi_operate_log(req_msg, para_data, &chann_info);
            break;

        case BIOS_BOOT_PARTITION_SEL_CMD:
            if (get_ipmi_src_data_len(req_msg) < sizeof(boot_option->service_partition_selector)) {
                return ipmi_send_simple_response(req_msg, COMP_CODE_LEN_INVALID);
            }
            boot_option->service_partition_selector = para_data[0];
            ret = ipmi_send_simple_response(req_msg, comp_code);
            ipmi_operation_log(req_msg, "Set service partition selector to (RAW:%02X) successfully", para_data[0]);
            break;

        case BIOS_BOOT_PARTITION_SCAN_CMD:
            if (get_ipmi_src_data_len(req_msg) < sizeof(boot_option->service_partition_scan)) {
                return ipmi_send_simple_response(req_msg, COMP_CODE_LEN_INVALID);
            }
            boot_option->service_partition_scan = para_data[0];
            ret = ipmi_send_simple_response(req_msg, comp_code);
            ipmi_operation_log(req_msg, "Set service partition scan to (RAW:%02X) successfully", para_data[0]);
            break;

        case BIOS_BOOT_FLAG_VALID_CMD:
            if (get_ipmi_src_data_len(req_msg) < sizeof(boot_option->boot_flag_valid_bit_clearing)) {
                return ipmi_send_simple_response(req_msg, COMP_CODE_LEN_INVALID);
            }
            boot_option->boot_flag_valid_bit_clearing = para_data[0];
            ret = ipmi_send_simple_response(req_msg, comp_code);
            ipmi_operation_log(req_msg, "Set boot flag valid bit clearing to (RAW:%02X) successfully", para_data[0]);
            break;

        case BIOS_BOOT_ACKNOWLEDGE_CMD:

            if (get_ipmi_src_data_len(req_msg) < BIOS_BOOT_ACK_INFO_NUM) {
                return ipmi_send_simple_response(req_msg, COMP_CODE_LEN_INVALID);
            }
            safe_fun_ret = memmove_s(boot_option->boot_info_acknowledge, sizeof(boot_option->boot_info_acknowledge),
                para_data, BIOS_BOOT_ACK_INFO_NUM);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            ret = ipmi_send_simple_response(req_msg, comp_code);
            ipmi_operation_log(req_msg, "Set boot info acknowledge to (RAW:%02X-%02X) successfully", para_data[0],
                para_data[1]);
            break;

        case BIOS_BOOT_INITIATOR_INFO_CMD:

            if (get_ipmi_src_data_len(req_msg) < BIOS_BOOT_INIT_INFO_NUM) {
                return ipmi_send_simple_response(req_msg, COMP_CODE_LEN_INVALID);
            }

            if (data_len < BIOS_BOOT_INIT_INFO_NUM) {
                debug_log(DLOG_ERROR, "%s:Invalid cmd length(%u)", __FUNCTION__, data_len);
                ret = ipmi_send_simple_response(req_msg, COMP_CODE_CMD_INVALID);
                break;
            }
            safe_fun_ret = memmove_s(boot_option->boot_initiator_info, sizeof(boot_option->boot_initiator_info),
                para_data, BIOS_BOOT_INIT_INFO_NUM);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            ret = ipmi_send_simple_response(req_msg, comp_code);
            ipmi_operation_log(req_msg,
                "Set boot initiator info to (RAW:%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X) successfully",
                para_data[0], para_data[1], para_data[2], para_data[3], para_data[4], para_data[5], para_data[6],
                para_data[7], para_data[8]);
            break;

        case BIOS_BOOT_INITIATOR_MAILBOX_CMD:

            if (get_ipmi_src_data_len(req_msg) < BIOS_BOOT_MAIBOX_SEG_LEN) {
                return ipmi_send_simple_response(req_msg, COMP_CODE_LEN_INVALID);
            }

            if (para_data[0] >= BIOS_MAILBOX_SEG_MAX_NUM) {
                comp_code = COMP_CODE_OUTOF_RANGE;
            } else {
                para_size = (gint32)(data_len - BIOS_BOOT_MAIBOX_SEG_LEN);
                if ((para_size > BIOS_BOOT_SEG_MIN_NUM) && (para_size < BIOS_BOOT_VALID_SEG_NUM)) {
                    safe_fun_ret =
                        memmove_s(&(boot_option->boot_initiator_mailbox[BIOS_BOOT_VALID_SEG_NUM * para_data[0]]),
                        BIOS_BOOT_INIT_MAILBOX_NUM - (BIOS_BOOT_VALID_SEG_NUM * para_data[0]), &para_data[1],
                        (guint32)(para_size - BIOS_BOOT_SEG_MIN_NUM));
                    if (safe_fun_ret != EOK) {
                        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                    }

                    bios_ipmi_opt_log(req_msg, para_data, para_size);
                }
            }

            ret = ipmi_send_simple_response(req_msg, comp_code);
            break;

        default:
            ret = ipmi_send_simple_response(req_msg, COMP_CODE_PARAM_NOT_SUPPORTED);
            break;
    }

    per_save((guint8 *)boot_option);
    return ret;
    
}

LOCAL void bios_boot_flag_ipmi_operate_log(const void *req_msg, const guint8 *boot_data,
    const IPMI_CHANN_INFO *chann_info)
{
    OBJ_HANDLE bios_handle = 0;
    guint8 bios_status = BIOS_STARTUP_STATE_OFF;

    (void)dal_get_object_handle_nth(CLASS_NAME_BIOS, 0, &bios_handle);
    (void)dal_get_property_value_byte(bios_handle, PROPERTY_BIOS_STARTUP_STATE, &bios_status);

    
    if (chann_info->chann_type == IPMI_HOST && bios_status != BIOS_STARTUP_POST_STAGE_FINISH) {
        maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "Set boot flags to (RAW:" BOOT_FLAGS_FMT ") successfully",
            BOOT_FLAGS_OBJ(boot_data));
    } else {
        ipmi_operation_log(req_msg, "Set boot flags to (RAW:" BOOT_FLAGS_FMT ") successfully",
            BOOT_FLAGS_OBJ(boot_data));
    }

    return;
}


LOCAL void bios_ipmi_opt_log(const void *req_msg, const guint8 *data, gint32 size)
{
    gint32 i = 0;
    gchar log_buf[BIOS_IPMI_CMD_MAX_LEN] = {0};
    gchar *log_tmp = log_buf;

    if (BIOS_IPMI_CMD_MAX_LEN <= size * BIOS_IPMI_LOG_FORMAT_LEN) {
        debug_log(DLOG_ERROR, "data len:%d(Max:%d) invalid!\r\n", size * BIOS_IPMI_LOG_FORMAT_LEN,
            BIOS_IPMI_CMD_MAX_LEN);
        return;
    }

    for (i = 0; i < size - 1; i++) {
        (void)snprintf_s(log_tmp, BIOS_IPMI_CMD_MAX_LEN - (i * BIOS_IPMI_LOG_FORMAT_LEN),
            BIOS_IPMI_CMD_MAX_LEN - (i * BIOS_IPMI_LOG_FORMAT_LEN) - 1, "%02X-", data[i]);
        log_tmp += BIOS_IPMI_LOG_FORMAT_LEN;
    }

    (void)snprintf_s(log_tmp, BIOS_IPMI_CMD_MAX_LEN - ((size - 1) * BIOS_IPMI_LOG_FORMAT_LEN),
        BIOS_IPMI_CMD_MAX_LEN - ((size - 1) * BIOS_IPMI_LOG_FORMAT_LEN) - 1, "%02X", data[i]);

    ipmi_operation_log(req_msg, "Set boot initiator mailbox to (RAW:%s)", log_buf);

    return;
}



LOCAL guint8 process_boot_flags_ipmi(const guint8 *boot_data, const IPMI_CHANN_INFO *chann_info, GSList *caller_info)
{
    errno_t secure_rv = EOK;
    gint32 ret;
    OBJ_HANDLE bios_handle = 0;
    guint8 boot_dev = NO_OVERRIDE;
    guint8 boot_flag = BMC_DISABLE;
    guint8 comp_code = COMP_CODE_UNKNOWN;
    BOOT_OPTIONS_S boot_option = { 0 };
    const BOOT_FLAGS_PARAM_S *param_in = (const BOOT_FLAGS_PARAM_S *)boot_data;
    STRUCT_SAVE_BOOT_FLAGS_PARAM save_param = { 0 };

    debug_log(DLOG_DEBUG, "boot_data:" BOOT_FLAGS_FMT "\r\n", BOOT_FLAGS_OBJ(boot_data));

    
    if (!(param_in->clear_cmos) && (param_in->flags_valid)) {
        boot_dev = param_in->boot_dev;
        if ((boot_dev == FORCE_SAFE_MODE) || (boot_dev == FORCE_DIAGNOSTIC_PARTITION)) {
            comp_code = COMP_CODE_PARAM_NOT_SUPPORTED;
            goto label_free_caller;
        }
        boot_flag = BMC_ENABLE;
    }

    ret = dfl_get_binded_object(BIOS_CLASS_NAME, BIOS_OBJECT_HANDLE_NAME, &bios_handle);
    if (ret != DFL_OK) {
        goto label_free_caller;
    }

    
    (void)memcpy_s(&boot_option, sizeof(BOOT_OPTIONS_S), &g_boot_options, sizeof(BOOT_OPTIONS_S));
    
    secure_rv = memcpy_s(&save_param.b_flags_orig, sizeof(BOOT_FLAGS_PARAM_S),
        (BOOT_FLAGS_PARAM_S *)boot_option.boot_flags, sizeof(boot_option.boot_flags));
    if (secure_rv != EOK) {
        goto label_free_caller;
    }

    ret = bios_boot_flags_generate(param_in, chann_info, &save_param.b_flags_new, &save_param.sync_to_setting);
    if (ret != RET_OK) {
        goto label_free_caller;
    }

    if (save_param.sync_to_setting) {
        if (chann_info->chann_type == IPMI_SMM && !(boot_data[0] & PBIT3)) {
            
            
            
            save_param.b_flags_new.boot_type = save_param.b_flags_orig.boot_type;
            
            (void)dal_get_property_value_byte(bios_handle, BIOS_BOOT_MODE, &save_param.boot_type);
        } else {
            save_param.boot_type = param_in->boot_type;
        }
    }

    secure_rv = memcpy_s(boot_option.boot_flags, sizeof(boot_option.boot_flags), (guint8 *)&save_param.b_flags_new,
        sizeof(BOOT_FLAGS_PARAM_S));
    if (secure_rv != EOK) {
        goto label_free_caller;
    }
    
    (void)memcpy_s(&g_boot_options, sizeof(BOOT_OPTIONS_S), &boot_option, sizeof(BOOT_OPTIONS_S));

    save_param.caller_info = caller_info;
    save_param.boot_flag = boot_flag;
    save_param.boot_dev = boot_dev;
    save_param.ipmi_chan = chann_info->chann_num;
    save_param.clear_cmos = param_in->clear_cmos;

    ret = bios_save_boot_option(&save_param); // caller_info在任务创建成功时由任务结束时释放
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "save_boot_option failed(ret:0x%x)\n", ret);
        comp_code = COMP_CODE_UNKNOWN;
        goto label_free_caller;
    }

    return COMP_CODE_SUCCESS;

label_free_caller:
    g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
    return comp_code;
}



LOCAL guint8 process_progress_flag_ipmi(guint8 progress_flag)
{
    BOOT_OPTIONS_S *boot_option = &g_boot_options;
    guint8 comp_code = COMP_CODE_SUCCESS;

    switch (progress_flag) {
        case BOOTOPTION_SET_COMPLETE:
            boot_option->set_in_progress = BOOTOPTION_SET_COMPLETE;
            break;

        case BOOTOPTION_SET_INPROGRESS:
            if (BOOTOPTION_SET_INPROGRESS == boot_option->set_in_progress) {
                comp_code = COMP_CODE_SET_IN_PROGRESS;
            } else {
                boot_option->set_in_progress = BOOTOPTION_SET_INPROGRESS;
            }

            break;

        default:
            comp_code = COMP_CODE_OUTOF_RANGE;
            break;
    }

    return comp_code;
}


LOCAL gint32 bios_boot_flags_generate(const BOOT_FLAGS_PARAM_S *i_flags, const IPMI_CHANN_INFO *chann,
    BOOT_FLAGS_PARAM_S *o_flags, gboolean *sync_to_setting)
{
    errno_t safe_fun_ret;
    OBJ_HANDLE bios_handle = 0;
    OBJ_HANDLE smbios_obj_handle = 0;
    gint32 ret;
    guint8 support_flag = 0;
    guint8 sw_flag = 0;
    guint8 boot_mode = 0;
    guint8 smbios_tatus = SMBIOS_WRITE_NOT_START;
    guchar arm_enable = 0;

    safe_fun_ret = memmove_s(o_flags, sizeof(BOOT_FLAGS_PARAM_S), i_flags, BIOS_BOOT_VALID_NUM);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }

    ret = dfl_get_binded_object(BIOS_CLASS_NAME, BIOS_OBJECT_HANDLE_NAME, &bios_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret);
        return RET_ERR;
    }

    
    (void)dal_get_property_value_byte(bios_handle, BIOS_BOOT_MODE_SUPPORT_FLAG, &support_flag);
    (void)dal_get_property_value_byte(bios_handle, BIOS_BOOT_MODE_SW, &sw_flag);
    (void)dal_get_property_value_byte(bios_handle, BIOS_BOOT_MODE, &boot_mode);

    
    if (chann->chann_type == IPMI_HOST) {
        ret = dfl_get_object_handle(CLASS_NAME_SMBIOS, &smbios_obj_handle);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret);
            return RET_ERR;
        }
        (void)dal_get_property_value_byte(smbios_obj_handle, PROPERTY_SMBIOS_STATUS_VALUE, &smbios_tatus);
    }

    if (support_flag == BOOT_MODE_SUPPORT_FLAG_ENABLE)  {
        if ((sw_flag == BIOS_BOOT_MODE_SW_OFF) 
            && ((smbios_tatus == SMBIOS_WRITE_FINISH && chann->chann_type == IPMI_HOST) ||
            chann->chann_type == IPMI_LAN) ) {
            
            o_flags->boot_type = boot_mode;
        } else {
            
            
            
            
            *sync_to_setting = TRUE;
            
            
        }
    } else {
        
        
        (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);
        
        if (arm_enable || (sw_flag == BIOS_BOOT_MODE_SW_OFF)) {
            o_flags->boot_type = boot_mode;
        }
        
    }

    
    
    
    return RET_OK;
}


LOCAL gint32 bios_save_boot_option(const STRUCT_SAVE_BOOT_FLAGS_PARAM *save_param)
{
    gint32 ret_val;
    
    STRUCT_SAVE_BOOT_FLAGS_PARAM *save_info = NULL;
    if (save_param == NULL) {
        debug_log(DLOG_ERROR, "save_param is null.");
        return RET_ERR;
    }

    
    
    save_info = (STRUCT_SAVE_BOOT_FLAGS_PARAM *)g_malloc0(sizeof(STRUCT_SAVE_BOOT_FLAGS_PARAM));
    if (save_info == NULL) {
        debug_log(DLOG_ERROR, "malloc to save failed.");
        return RET_ERR;
    }
    (void)memcpy_s(save_info, sizeof(STRUCT_SAVE_BOOT_FLAGS_PARAM), save_param, sizeof(STRUCT_SAVE_BOOT_FLAGS_PARAM));

    // 将需要保存的信息发送到队列，若当前没有任务，则启动任务去保存启动项
    ret_val = vos_queue_send(g_bios_ipmi_save_info_queue, (gpointer)save_info);
    if (ret_val != RET_OK) {
        g_free(save_info);
        debug_log(DLOG_ERROR, "%s: vos_queue_send failed, ret = %d", __FUNCTION__, ret_val);
        return RET_ERR;
    }

    if (g_save_flags_taskID == 0) {
        ret_val = vos_task_create(&g_save_flags_taskID, "taskSaveBootFlags", (TASK_ENTRY)task_per_save_boot_flags, NULL,
            DEFAULT_PRIORITY);
        if (ret_val != RET_OK) {
            debug_log(DLOG_ERROR, "%s: create task_per_save_boot_flags fail(Ret:0x%x)!", __FUNCTION__, ret_val);
            g_save_flags_taskID = 0;
            return RET_ERR;
        }
    }
    

    return RET_OK;
}

gint32 create_ipmi_save_info_queue(void)
{
    gint32 ret = vos_queue_create(&g_bios_ipmi_save_info_queue);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: create bios save info queue failed(%d)", __FUNCTION__, ret);
    }

    return ret;
}




LOCAL void task_per_save_boot_flags(void)
{
    gint32 ret_val = 0;
    OBJ_HANDLE bios_handle = 0;
    gpointer save_info = NULL;

    if ((ret_val = dfl_get_binded_object(BIOS_CLASS_NAME, BIOS_OBJECT_HANDLE_NAME, &bios_handle)) != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get Bios object handle failed. ret=%d", __FUNCTION__, ret_val);
        g_save_flags_taskID = 0;
        return;
    }

    while (1) {
        ret_val = vos_queue_receive(g_bios_ipmi_save_info_queue, (gpointer *)&save_info, QUE_WAIT_FOREVER);
        if (ret_val != RET_OK || save_info == NULL) {
            debug_log(DLOG_MASS, "%s: receive queue data failed", __FUNCTION__);
            continue;
        }

        STRUCT_SAVE_BOOT_FLAGS_PARAM *save_param = (STRUCT_SAVE_BOOT_FLAGS_PARAM *)save_info;

        (void)dal_set_property_value_byte(bios_handle, BIOS_START_OPTION_FLAG_NAME,
            EFFECT_OPT_TO_TIMES(save_param->b_flags_new.effect_opt), DF_SAVE);
        (void)dal_set_property_value_byte(bios_handle, BIOS_START_OPTION_FLAG_EXT_NAME,
            EFFECT_OPT_TO_TIMES(save_param->b_flags_new.effect_opt), DF_SAVE);

        if (save_param->sync_to_setting) {
            debug_log(DLOG_DEBUG, "Save boot type[%d] into setting.json.", save_param->boot_type);
            ret_val = write_boot_mode_to_setting_json(save_param->boot_type);
            if (ret_val != RET_OK) {
                debug_log(DLOG_ERROR, "%s : Failed to write boot mode to setting.json, ret_val = %d", __FUNCTION__,
                    ret_val);
                goto NEXT_LOOP;
            }

            (void)dal_set_property_value_byte(bios_handle, BIOS_BOOT_MODE, save_param->boot_type, DF_SAVE);
        }

        ret_val = per_save((guint8 *)&g_boot_options);
        gint32 ret_val2 = per_save((guint8 *)&(g_boot_options.boot_flags[1]));
        if (ret_val != RET_OK || ret_val2 != RET_OK) {
            debug_log(DLOG_ERROR, "per_save boot flags fail(Ret:0x%x,0x%x)!", ret_val, ret_val2);
        } else {
            bios_boot_flags_change_log(&save_param->b_flags_orig, &save_param->b_flags_new, save_param->caller_info);
        }

        set_boot_option_property(save_param->boot_flag, save_param->boot_dev, save_param->ipmi_chan);

        
        if (save_param->clear_cmos) {
            ret_val = bios_clear_cmos(bios_handle, NULL, NULL, NULL);
            if (ret_val != RET_OK) {
                debug_log(DLOG_ERROR, "%s:bios_clear_cmos fail(ret:0x%x)!", __FUNCTION__, ret_val);
            }
        }

    NEXT_LOOP:
        g_slist_free_full(save_param->caller_info, (GDestroyNotify)g_variant_unref);
        g_free(save_info);
        save_info = NULL;
        vos_task_delay(1000); // 延时1000ms等待设置完成
    }
}



LOCAL void bios_boot_flags_change_log(BOOT_FLAGS_PARAM_S *f_orig, BOOT_FLAGS_PARAM_S *f_new, GSList *caller_info)
{
    if (f_orig->effect_opt != f_new->effect_opt) {
        method_operation_log(caller_info, NULL, "Set boot flags effective mode from (%s) to (%s) successfully",
            transform_effective_times_to_string(EFFECT_OPT_TO_TIMES(f_orig->effect_opt)),
            transform_effective_times_to_string(EFFECT_OPT_TO_TIMES(f_new->effect_opt)));
    }

    if (f_orig->boot_type != f_new->boot_type) {
        method_operation_log(caller_info, NULL, "Set boot type from (%s) to (%s) successfully",
            transform_boot_type_to_string(f_orig->boot_type), transform_boot_type_to_string(f_new->boot_type));
    }

    if (f_orig->boot_dev != f_new->boot_dev) {
        method_operation_log(caller_info, NULL, "Set boot device from (%s) to (%s) successfully",
            transform_boot_dev_to_string(f_orig->boot_dev, START_OPTION_EXT_NONE),
            transform_boot_dev_to_string(f_new->boot_dev, START_OPTION_EXT_NONE));
    }

    return;
}


gint32 bios_clear_cmos(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret_val;
    BOOT_OPTIONS_S *boot_option = &g_boot_options;
    GVariant *property_data = NULL;

    

    boot_option->boot_flags[1] |= PBIT7;

    
    property_data = g_variant_new_byte(BMC_ENABLE);
    ret_val = dfl_set_property_value(obj_handle, BIOS_CLEAR_CMOS_NAME, property_data, DF_HW);
    if (ret_val != DFL_OK) {
        
        method_operation_log(caller_info, NULL,
            "Failed to issue the command for restoring BIOS Setup default settings.");
        

        g_variant_unref(property_data);
        debug_log(DLOG_ERROR, DB_STR_SET_PROPERTY_ERR, ret_val);
        return RET_ERR;
    }

    g_variant_unref(property_data);

    boot_option->boot_flags[1] &= NBIT7;

    

    per_save((guint8 *)boot_option);

    
    method_operation_log(caller_info, NULL,
        "Restoring BIOS Setup default settings issued successfully and the BIOS default settings will be restored upon "
        "the next startup.");
    

    return RET_OK;
}


LOCAL inline const gchar *transform_boot_type_to_string(guint8 boot_type)
{
    return (boot_type == BIOS_BOOT_LEGACY) ? LOG_STR_BOOT_TYPE_LEGACY : LOG_STR_BOOT_TYPE_UEFI;
}

void get_bios_boot_flags(const void *req_msg, gpointer user_data, guint8 *resp_data, guint8 resp_data_len,
    guint8 *resp_len)
{
    errno_t safe_fun_ret;
    gint32 ipmi_ret;
    guint8 boot_mode = 0;
    guchar arm_enable = 0;
    guint8 support_flag = 0;
    guint8 sw_flag = 0;
    OBJ_HANDLE bios_handle;
    BOOT_OPTIONS_S *boot_option = &g_boot_options;
    
    
    ipmi_ret = read_boot_mode_from_setting_json(&boot_mode);
    if (ipmi_ret == RET_OK) {
        
        ipmi_ret = bios_set_boot_mode_prop_value(boot_mode);
        
        if (ipmi_ret != RET_OK) {
            debug_log(DLOG_ERROR, "Failed to call bios_set_boot_mode, ret = %d", ipmi_ret);
        }

        maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "Set boot mode (%s) with result : %d",
            (BIOS_BOOT_LEGACY == boot_mode) ? "LEGACY" : "UEFI", ipmi_ret);
    }

    

    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);

    ipmi_ret = dfl_get_binded_object(BIOS_CLASS_NAME, BIOS_OBJECT_HANDLE_NAME, &bios_handle);
    if (ipmi_ret != DFL_OK) {
        return;
    }
    (void)dal_get_property_value_byte(bios_handle, BIOS_BOOT_MODE_SUPPORT_FLAG, &support_flag);
    (void)dal_get_property_value_byte(bios_handle, BIOS_BOOT_MODE, &boot_mode);
    (void)dal_get_property_value_byte(bios_handle, BIOS_BOOT_MODE_SW, &sw_flag);

    if (arm_enable == 1) {
        
        
        boot_option->boot_flags[0] |= PBIT5;
    } else if (support_flag != BOOT_MODE_SUPPORT_FLAG_ENABLE && sw_flag != BIOS_BOOT_MODE_SW_ON) {
        if (boot_mode == BIOS_BOOT_LEGACY) {
            boot_option->boot_flags[0] &= NBIT5;
        } else if (boot_mode == BIOS_BOOT_UEFI) {
            boot_option->boot_flags[0] |= PBIT5;
        }
    }
    
    
    *resp_len = 8; // 响应消息长度为8
    
    safe_fun_ret = memmove_s(resp_data + 3, resp_data_len - 3, boot_option->boot_flags, BIOS_BOOT_VALID_NUM);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
}


void update_bios_startup_state(guint8 value)
{
    gint32 result = RET_OK;
    OBJ_HANDLE handle = 0;

    result = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &handle);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "Get Bios object handle failed.");
        return;
    }

    (void)dal_set_property_value_byte(handle, PROPERTY_BIOS_STARTUP_STATE, value, DF_SAVE_TEMPORARY);

    return;
}


LOCAL void bios_boot_stage_sync(guint8 boot_stage)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dfl_get_object_handle(SMBIOS_OBJECT_NAME, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Get SMBios object handle fail, ret=%d", __FUNCTION__, ret);
        return;
    }

    guint8 smbios_status = (boot_stage == BIOS_BOOT_STAGE_OFF) ? SMBIOS_WRITE_NOT_START : SMBIOS_WRITE_FINISH;
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_SMBIOS_STATUS_VALUE, smbios_status, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set SmBios.SmBiosStatus fail, ret=%d", __FUNCTION__, ret);
        return;
    }

    update_bios_startup_state((boot_stage == BIOS_BOOT_STAGE_OFF) ?
        BIOS_STARTUP_STATE_OFF : BIOS_STARTUP_POST_STAGE_FINISH);
}

void bios_boot_stage_monitor(gpointer data)
{
#define BOOT_STAGE_PERIOD 2000
    OBJ_HANDLE obj_handle = 0;
    gboolean init = FALSE;
    guint8 boot_stage_old = BIOS_BOOT_STAGE_OFF;

    (void)prctl(PR_SET_NAME, (gulong)"BootStageMonitor");

    gint32 ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get Bios handle fail", __FUNCTION__);
        return;
    }

    while (1) {
        guint8 boot_stage = BIOS_BOOT_STAGE_OFF;
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_BIOS_BOOT_STAGE, &boot_stage);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s: Bios.BiosBootStage fail, ret=%d", __FUNCTION__, ret);
            vos_task_delay(BOOT_STAGE_PERIOD);
            continue;
        }
        if (init == FALSE) {
            init = TRUE;
            bios_boot_stage_sync(boot_stage);
            goto NEXT_LOOP;
        }

        if (boot_stage_old != boot_stage) {
            bios_boot_stage_sync(boot_stage);
        }

        if (boot_stage_old != BIOS_BOOT_STAGE_OFF || boot_stage == BIOS_BOOT_STAGE_OFF) {
            goto NEXT_LOOP;
        }

        guint8 effective_times = EFFECTIVE_NONE;
        ret = dal_get_property_value_byte(obj_handle, PROTERY_BIOS_BOOTOPTIONFLAG, &effective_times);
        if (effective_times != EFFECTIVE_ONCE) {
            debug_log(DLOG_DEBUG, "%s: boot option flag is not once", __FUNCTION__);
            goto NEXT_LOOP;
        }

        ret = dal_set_property_value_byte(obj_handle, BIOS_START_OPTION_NAME, NO_OVERRIDE, DF_SAVE | DF_AUTO);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s: clear start option fail, ret=%d", __FUNCTION__, ret);
        }

        ret = dal_set_property_value_byte(obj_handle, BIOS_START_OPTION_NAME_EXT, REG_VAL_MAP_DEFAULT,
            DF_SAVE | DF_AUTO);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s: clear start option ext fail, ret=%d", __FUNCTION__, ret);
        }

NEXT_LOOP:
        boot_stage_old = boot_stage;
        vos_task_delay(BOOT_STAGE_PERIOD);
    }
}