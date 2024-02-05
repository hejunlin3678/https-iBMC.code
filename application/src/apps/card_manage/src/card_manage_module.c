
#include "pme_app/pme_app.h"
#include "pme_app/dalib/dal_obj_priv_data.h"
#include "pme_app/common/public_redfish.h"
#include "pme_app/record_wave/record_wave.h"
#include "card_manage_module.h"
#include "pcie_card.h"
#include "pcie_card_comm.h"
#include "hdd_backplane.h"
#include "raid_card.h"
#include "mezz_card.h"
#include "sw_card.h"
#include "cic_card.h"
#include "sdi_card.h"
#include "dpu_card.h"
#include "riser_card.h"
#include "fpga_card.h"
#include "cpu_board.h"
#include "fan_board.h"
#include "mem_board.h"
#include "io_board.h"
#include "exp_board.h"
#include "main_board.h"
#include "gpu_board.h"
#include "gpu_card.h"
#include "leakdet_card.h"
#include "json.h"
#include "passthrough_card.h"
#include "share_card.h"
#include "ar_card.h"
#include "modbus_device.h"
#include "asset_locator.h"
#include "pme_app/common/debug_log_macro.h"
#include "retimer_manage.h"
#include "security_module.h"
#include "soc_board.h"

LOCAL gint32 dump_repeater_all_regs_info(const gchar *path);
LOCAL gint32 dump_sas_regs_info(const gchar *path);
LOCAL gint32 converge_add_object_callback(OBJ_HANDLE obj_handle);
LOCAL gint32 converge_del_object_callback(OBJ_HANDLE obj_handle);

#define REPEATER_READ_BUF_LEN 98
#define SASREDRIVER_READ_BUF_LEN 1024
#define DEVICE_NAME_LEN 30
#define CONTENT_BUF_LEN 4096
#define FILE_NAME_BUF_LEN 256

LOCAL gulong g_cm_set_device_async = 0;

LOCAL void __attribute__((constructor)) main_init(void)
{
    (void)vos_thread_binary_sem4_create(&g_cm_set_device_async, "carrst", 1UL);
}

typedef gint32 (*CARD_OBJECT_HOTSWAP_PROC)(OBJ_HANDLE obj_hanlde);

typedef struct tag_card_object_handle_s {
    gchar *class_name;
    CARD_OBJECT_HOTSWAP_PROC add_callback_func;
    CARD_OBJECT_HOTSWAP_PROC del_callback_func;
} CARD_OBJECT_HANLDE_S;


LOCAL gint8 g_os_reset_flag = 0;



gint32 cpu_id_logical_to_physical(guint8 logic_id, guint8 *phsical_id)
{
    gint32 ret;
    OBJ_HANDLE obj_handle;
    GVariant *get_value_physical = NULL;
    GVariant *get_value_logical = NULL;
    const guint8 *physicalid = NULL;
    const guint8 *logicalid = NULL;
    gsize n, m;
    guint32 i;
    gint32 find_flag = 0;

    ret = dfl_get_object_handle(OBJECT_CPU_ID_MAP, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get CpuIdMap object failed, result is %d", ret);
        return ret;
    }

    ret = dfl_get_property_value(obj_handle, PROPERTY_CPUIDMAP_PHYSICALID, &get_value_physical);
    if (ret != DFL_OK) {
        return ret;
    }
    physicalid = (const guint8 *)g_variant_get_fixed_array(get_value_physical, &n, sizeof(guint8));

    ret = dfl_get_property_value(obj_handle, PROPERTY_CPUIDMAP_LOGICALID, &get_value_logical);
    if (ret != DFL_OK) {
        g_variant_unref(get_value_physical);
        return ret;
    }

    logicalid = (const guint8 *)g_variant_get_fixed_array(get_value_logical, &m, sizeof(guint8));
    if ((m != n) || (physicalid == NULL) || (logicalid == NULL)) {
        g_variant_unref(get_value_physical);
        g_variant_unref(get_value_logical);
        return RET_ERR;
    }

    for (i = 0; i < m; i++) {
        if (logicalid[i] == logic_id) {
            *phsical_id = (gint32)physicalid[i];
            find_flag = 1;
            break;
        }
    }

    g_variant_unref(get_value_physical);
    g_variant_unref(get_value_logical);
    if (find_flag == 0 && dal_match_product_id(PRODUCT_ID_PANGEA_V6)) {
        *phsical_id = logic_id;
    }
    return RET_OK;
}


gint32 get_pcie_card_resId(void)
{
    GSList *obj_list = NULL;
    GSList *node = NULL;
    guint8 cpu_id = 0;
    gint32 ret = 0;
    OBJ_HANDLE obj_handle = 0;

    ret = dfl_get_object_list(CLASS_PCIECARD_NAME, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get PCIeCard object list failed.");
        return RET_ERR;
    }

    for (node = obj_list; node; node = node->next) {
        obj_handle = (OBJ_HANDLE)node->data;
        
        ret = get_card_cpu_id(obj_handle, &cpu_id);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:Get %s ResId failed! ret:%d.", __FUNCTION__, dfl_get_object_name(obj_handle),
                ret);
            continue;
        }
        ret = dal_set_property_value_byte(obj_handle, PROPERTY_PCIECARD_CPU_ID, cpu_id, DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:Update %s ResId(%d) failed! ret:%d.", __FUNCTION__,
                dfl_get_object_name(obj_handle), cpu_id, ret);
            continue;
        }
        
    }

    g_slist_free(obj_list);
    return RET_OK;
}

LOCAL CARD_OBJECT_HANLDE_S g_card_object_manage_tbl[] =
{
    {CLASS_PCIECARD_NAME,       pcie_card_add_object_callback,      pcie_card_del_object_callback},
    {CLASS_OCP_CARD,            ocp_card_add_object_callback,      ocp_card_del_object_callback},
    {CLASS_HDDBACKPLANE_NAME,   hdd_backplane_add_object_callback,  hdd_backplane_del_object_callback},
    {CLASS_RAIDCARD_NAME,       raid_card_add_object_callback,      raid_card_del_object_callback},
    {CLASS_MEZZCARD_NAME,       mezz_card_add_object_callback,      mezz_card_del_object_callback},
    {CLASS_CARD_NAME,           sw_card_add_object_callback,        sw_card_del_object_callback},
    {CLASS_RISERPCIECARD_NAME,  riser_card_add_object_callback,     riser_card_del_object_callback},
    {CLASS_CPUBOARD_NAME,       cpu_board_add_object_callback,      cpu_board_del_object_callback},
    {CLASS_MEMBOARD_NAME,       mem_board_add_object_callback,      mem_board_del_object_callback},
    {CLASS_IOBOARD_NAME,        io_board_add_object_callback,       io_board_del_object_callback},
    {CLASS_EXPBOARD_NAME,       exp_board_add_object_callback,      exp_board_del_object_callback},
    {CLASS_MAINBOARD_NAME,      main_board_add_object_callback,     main_board_del_object_callback},
    {CLASS_FANBOARD_NAME,       fan_board_add_object_callback,      fan_board_del_object_callback},
    {CLASS_CARD_ACTION,         card_action_add_object_callback,    card_action_del_object_callback},
    {CLASS_PCIE_SDI_CARD,       sdi_card_add_object_callback,     sdi_card_del_object_callback},
    {CLASS_NETCARD_NAME,        net_card_add_object_callback,   net_card_del_object_callback},
    
    {CLASS_M2TRANSFORMCARD,     m2transformcard_add_object_callback,   m2transformcard_del_object_callback},
    
    
    { CLASS_PCIETRANSFORMCARD,  pcietransformcard_add_object_callback,  pcietransformcard_del_object_callback},
    
    {CLASS_GPUBOARD_NAME,       gpu_board_add_object_callback,      gpu_board_del_object_callback},
    {CLASS_PCIE_FPGA_CARD,       fpga_card_add_object_callback,      fpga_card_del_object_callback},

    
    { CLASS_PASSTHROUGH_CARD,         passthroughcard_add_object_callback,     passthroughcard_del_object_callback},
    
    
    {CLASS_GPU_CARD,            gpu_card_add_object_callback,       gpu_card_del_object_callback},
    
    
    {CLASS_EXTEND_PCIECARD_NAME,  extend_pcie_card_add_object_callback,      extend_pcie_card_del_object_callback},
    
    {CLASS_STORAGE_DPU_CARD,       dpu_card_add_object_callback,      dpu_card_del_object_callback},
    {CLASS_PANGEA_PCIE_CARD,   pangea_card_add_object_callback,      pangea_card_del_object_callback},
    
    {CLASS_ASSET_LOCATE_BOARD,  asset_brd_add_object_callback,      asset_brd_del_object_callback},
    {CLASS_OCP_CARD,       ocp_card_add_object_callback,      ocp_card_del_object_callback},
    {CLASS_SECURITY_MODULE,    securitymodule_add_object_callback,      securitymodule_del_object_callback},
    {CLASS_RETIMER_NAME, retimer_add_object_callback, retimer_del_object_callback},
    {CLASS_AR_CARD_NAME, arcard_add_object_callback, arcard_del_object_callback},
    {CLASS_SOC_BOARD, soc_board_add_object_callback, soc_board_del_object_callback},
    {CLASS_OOB_CHAN, oob_chan_add_object_callback, oob_chan_del_object_callback},
    {CLASS_CONVERGE_BOARD, converge_add_object_callback, converge_del_object_callback}
};


LOCAL guint32 get_resource_id_by_flag(guchar flag_array[], guint32 len)
{
    guint32 i = 1;
    guint32 temp = 0;
    guint32 temp_index = 0;

    for (i = 1; i < len; i++) {
        if (flag_array[i] == 1) {
            temp++;
            temp_index = i;
        }
    }

    if (temp == 1) {
        return temp_index;
    } else if (temp == 0) {
        return 0;
    } else if ((temp_index == 2) && (flag_array[1] == 1) && (flag_array[2] == 1)) {
        return RESOURCE_ID_CPU1_AND_CPU2;
    } else if ((temp_index == FOURTH_CPU) && (flag_array[FIRST_CPU] == RESOURCE_VALID) &&
        (flag_array[SECEND_CPU] == RESOURCE_VALID) && (flag_array[THIRD_CPU] == RESOURCE_VALID) &&
        (flag_array[FOURTH_CPU] == RESOURCE_VALID)) {
        return RESOURCE_ID_PCIE_CARD_BELONG_TO_ALL_CPU;
    } else {
        return 0;
    }
}


LOCAL gint32 is_pcie_mmio_valid(OBJ_HANDLE obj_handle, guint8 flag)
{
    gint32 ret = RET_ERR;
    GVariant *get_value_l = NULL;
    GVariant *get_value_h = NULL;
    const guint32 *pcie_addr_info_l = NULL;
    const guint32 *pcie_addr_info_h = NULL;
    guint32 fdm_index = 0;
    gsize l_num = 0;
    gsize h_num = 0;

    ret = dfl_get_property_value(obj_handle, (flag == 1) ? PROPERTY_MMIO_BASE_L : PROPERTY_MMIO_LIMIT_L, &get_value_l);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get %s failed", (flag == 1) ? PROPERTY_MMIO_BASE_L : PROPERTY_MMIO_LIMIT_L);
        return RET_OK;
    }
    pcie_addr_info_l = (const guint32 *)g_variant_get_fixed_array(get_value_l, &l_num, sizeof(guint32));
    if (pcie_addr_info_l == NULL) {
        g_variant_unref(get_value_l);
        debug_log(DLOG_ERROR, "%s limit_array is NULL", (flag == 1) ? PROPERTY_MMIO_BASE_L : PROPERTY_MMIO_LIMIT_L);
        return RET_OK;
    }

    ret = dfl_get_property_value(obj_handle, (flag == 1) ? PROPERTY_MMIO_BASE_H : PROPERTY_MMIO_LIMIT_H, &get_value_h);
    if (ret != DFL_OK) {
        g_variant_unref(get_value_l);
        debug_log(DLOG_ERROR, "get %s failed", (flag == 1) ? PROPERTY_MMIO_BASE_H : PROPERTY_MMIO_LIMIT_H);
        return RET_OK;
    }
    pcie_addr_info_h = (const guint32 *)g_variant_get_fixed_array(get_value_h, &h_num, sizeof(guint32));
    if (pcie_addr_info_h == NULL) {
        g_variant_unref(get_value_l);
        g_variant_unref(get_value_h);
        debug_log(DLOG_ERROR, "%s limit_array is NULL", (flag == 1) ? PROPERTY_MMIO_BASE_H : PROPERTY_MMIO_LIMIT_H);
        return RET_OK;
    }

    if (l_num != h_num) {
        g_variant_unref(get_value_l);
        g_variant_unref(get_value_h);
        debug_log(DLOG_ERROR, "array num is not match");
        return RET_OK;
    }

    ret = RET_ERR;
    for (fdm_index = 0; fdm_index < l_num; fdm_index++) {
        if ((pcie_addr_info_h[fdm_index] != 0) || (pcie_addr_info_l[fdm_index] != 0)) {
            ret = RET_OK;
            break;
        }
    }
    g_variant_unref(get_value_l);
    g_variant_unref(get_value_h);

    return ret;
}


LOCAL gint32 modify_resource_id_flag(GSList *match_pcie_addrinfo, guchar flag_array[], guint32 len)
{
    gint32 ret = RET_ERR;
    OBJ_HANDLE obj_handle = 0;
    GSList *node = NULL;
    guchar tmp_cpu_id = 0;
    guint8 limit_flag = 0;
    guint8 base_flag = 1;

    for (node = match_pcie_addrinfo; node; node = node->next) {
        obj_handle = (OBJ_HANDLE)node->data;
        
        if ((is_pcie_mmio_valid(obj_handle, limit_flag) == RET_ERR) &&
            (is_pcie_mmio_valid(obj_handle, base_flag) == RET_ERR)) {
            ret = dal_get_property_value_byte(obj_handle, PROPERTY_SOCKET_ID, &tmp_cpu_id);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "Get PcieAddrInfo SlotId failed.");
                continue;
            }

            if (tmp_cpu_id + 1 < len) {
                
                flag_array[tmp_cpu_id + 1] = 0;
            } else {
                return RET_ERR;
            }
        }
    }

    return RET_OK;
}


LOCAL gint32 get_card_type(gchar class_name[MAX_NAME_SIZE], guchar *card_type)
{
    if (!strcmp(class_name, CLASS_PCIECARD_NAME)) {
        *card_type = COMPONENT_TYPE_PCIE_CARD;
    } else if (!strcmp(class_name, CLASS_RAIDCARD_NAME)) {
        *card_type = COMPONENT_TYPE_RAID_CARD;
    } else if (!strcmp(class_name, CLASS_MEZZCARD_NAME)) {
        *card_type = COMPONENT_TYPE_MEZZ;
    } else if (!strcmp(class_name, CLASS_NETCARD_NAME)) {
        *card_type = COMPONENT_TYPE_NIC_CARD;
    } else {
        debug_log(DLOG_ERROR, "Get invalid card type.");
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 get_tmp_cpu_id(guchar dynamic_bdf, OBJ_HANDLE obj_handle, guchar *tmp_cpu_id, GSList **match_pcie_addrinfo)
{
    guint8 physical_id = 0;
    gint32 ret = RET_ERR;
    guchar armEnable = 0;
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &armEnable);

    if (dynamic_bdf == 1) {
        // 资源归属于PCIe SWitch
        *tmp_cpu_id = RESOURCE_ID_PCIE_SWTICH;
    } else {
        if (*tmp_cpu_id == RESOURCE_ID_PCH) { // 资源归属于PCH,不作处理
        } else if (*tmp_cpu_id == RESOURCE_ID_PCIE_SWTICH) {
            *tmp_cpu_id = 0; // 此时的0xff无效
        } else {
            ret = cpu_id_logical_to_physical(*tmp_cpu_id, &physical_id);
            if (ret == RET_OK) {
                // 8100走的分支
                *tmp_cpu_id = physical_id;
            } else {
                // 资源归属于CPU时, 描述是CPU1,CPU2..., socketId从0开始, 需要加1
                *tmp_cpu_id += 1;
            }
            if (armEnable) {
                *match_pcie_addrinfo = g_slist_append(*match_pcie_addrinfo, (gpointer)(uintptr_t)obj_handle);
            }
        }
    }

    return RET_OK;
}


gint32 get_card_cpu_id(OBJ_HANDLE card_handle, guchar *cpu_id)
{
#define MAX_PCIE_RESOURCE_ID 255
    gint32 ret = RET_ERR;
    guchar tmp_id = 0;
    guchar tmp_type = 0;
    guchar tmp_groupid = 0;
    guchar card_type = 0;
    guchar device_num = 0;
    guchar group_id = 0;
    guchar tmp_cpu_id = 0;
    guchar dynamic_bdf = 0;
    guchar flag = 0;
    guchar ref_component_flag = 0;
    guint8 slot_id = 0;
    gchar class_name[MAX_NAME_SIZE] = {0};
    const gchar *object_name = NULL;
    guchar resource_id_exist_flag[MAX_PCIE_RESOURCE_ID + 1] = {0};
    guint32 temp_resource_id = 0;
    GSList *obj_list = NULL;
    GSList *node = NULL;
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE physical_partition_handle = 0;
    GSList *match_pcie_addrinfo = NULL;
    guchar armEnable = 0;
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &armEnable);

    if (cpu_id == NULL) {
        return RET_ERR;
    }

    

    
    ret = dfl_get_referenced_object(card_handle, PROPERTY_RAIDCARD_REF_COMPONENT, &obj_handle);
    if (ret == DFL_OK) {
        object_name = dfl_get_object_name(obj_handle);
        if (object_name == NULL) {
            debug_log(DLOG_ERROR, "%s:Failed to get object_name.", __FUNCTION__);
        }

        
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_DEVICENUM, &device_num);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:Failed to get %s DeviceNum, ret:%d.", __FUNCTION__, object_name, ret);
            return RET_ERR;
        }

        ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &card_type);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:Failed to get %s DeviceType, ret:%d.", __FUNCTION__, object_name, ret);
            return RET_ERR;
        }

        ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_GROUPID, &group_id);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:Failed to get %s GroupID, ret:%d.", __FUNCTION__, object_name, ret);
            return RET_ERR;
        }

        ref_component_flag = 1;
    } else {
        debug_log(DLOG_DEBUG, "%s:Failed to get RefComponent.", __FUNCTION__);

        object_name = dfl_get_object_name(card_handle);
        if (object_name == NULL) {
            debug_log(DLOG_ERROR, "%s:Failed to get object_name.", __FUNCTION__);
        }

        ret = dal_get_property_value_byte(card_handle, PROPERTY_NETCARD_SLOT_ID, &slot_id);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:Get %s SlotId failed, ret:%d.", __FUNCTION__, object_name, ret);
            return RET_ERR;
        }

        ret = dfl_get_class_name(card_handle, class_name, MAX_NAME_SIZE);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s:Get %s ClassName failed, ret:%d.", __FUNCTION__, object_name, ret);
            return RET_ERR;
        }

        ret = get_card_type(class_name, &card_type);
        if (ret == RET_ERR) {
            return ret;
        }
    }

    
    ret = dfl_get_object_list(CLASS_PCIE_ADDR_INFO, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:Get PcieAddrInfo object list failed.", __FUNCTION__);
        return RET_ERR;
    }

    for (node = obj_list; node; node = node->next) {
        obj_handle = (OBJ_HANDLE)node->data;
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_PCIE_SLOT_ID, &tmp_id);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:Get %s SlotId failed, ret:%d.", __FUNCTION__, dfl_get_object_name(obj_handle),
                ret);
            continue;
        }
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_TYPE, &tmp_type);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:Get %s ComponentType failed, ret:%d.", __FUNCTION__,
                dfl_get_object_name(obj_handle), ret);
            continue;
        }
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_GROUP_ID, &tmp_groupid);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:Get %s GroupId failed, ret:%d.", __FUNCTION__, dfl_get_object_name(obj_handle),
                ret);
            continue;
        }

        if ((ref_component_flag == 1 && tmp_id == device_num && tmp_type == card_type && tmp_groupid == group_id) ||
            (ref_component_flag == 0 && tmp_id == slot_id && tmp_type == card_type)) {
            ret = dal_get_property_value_byte(obj_handle, PROPERTY_SOCKET_ID, &tmp_cpu_id);
            if (ret != RET_OK) {
                g_slist_free(obj_list);
                debug_log(DLOG_ERROR,
                    "%s:Get %s SocketId failed! RefComponentFlag[%d],DeviceNum[%d],Slot[%d],CardType[%d],GroupId[%d], "
                    "ret:%d.",
                    __FUNCTION__, dfl_get_object_name(obj_handle), ref_component_flag, device_num, slot_id, card_type,
                    group_id, ret);
                return RET_ERR;
            }
            ret = dal_get_property_value_byte(obj_handle, PROPERTY_DYNAMIC_BDF, &dynamic_bdf);
            if (ret != RET_OK) {
                g_slist_free(obj_list);
                debug_log(DLOG_ERROR,
                    "%s:Get %s DynamicBDF failed! "
                    "RefComponentFlag[%d],DeviceNum[%d],Slot[%d],CardType[%d],GroupId[%d], ret:%d.",
                    __FUNCTION__, dfl_get_object_name(obj_handle), ref_component_flag, device_num, slot_id, card_type,
                    group_id, ret);
                return RET_ERR;
            }
            flag = 1;

            
            ret = dal_get_object_handle_nth(CLASS_COMPUTER_PARTITION, 0, &physical_partition_handle);
            if (ret == RET_OK && tmp_cpu_id > 3) {
                continue;
            }

            (void)get_tmp_cpu_id(dynamic_bdf, obj_handle, &tmp_cpu_id, &match_pcie_addrinfo);

            resource_id_exist_flag[tmp_cpu_id] = 1;
        }
    }

    g_slist_free(obj_list);

    if (flag == 0) {
        debug_log(DLOG_ERROR, "%s:PCieAddrInfo match failed.", __FUNCTION__);
        g_slist_free(match_pcie_addrinfo);
        return RET_ERR;
    } else {
        if (armEnable) {
            
            if ((g_slist_length(match_pcie_addrinfo) > 1) && (card_type == DEVICE_TYPE_PCIE_CARD)) {
                ret = modify_resource_id_flag(match_pcie_addrinfo, resource_id_exist_flag,
                    sizeof(resource_id_exist_flag));
                if (ret != RET_OK) {
                    g_slist_free(match_pcie_addrinfo);
                    debug_log(DLOG_ERROR, "Modify resource id flag failed.");
                    return RET_ERR;
                }
            }
        }
        temp_resource_id = get_resource_id_by_flag(resource_id_exist_flag, sizeof(resource_id_exist_flag));
        *cpu_id = (temp_resource_id > MAX_PCIE_RESOURCE_ID) ? 0 : (guchar)temp_resource_id;
        g_slist_free(match_pcie_addrinfo);
        return RET_OK;
    }

    
}


LOCAL void card_dump_info(const gchar *path)
{
    guint8 i;
    PCIE_CARD_LOG_PROVIDER dump_log_provider[] = {
        {"mezz card",          mezz_card_dump_info},
        {"raid card",          raid_card_dump_info},
        {"riser card",         riser_card_dump_info},
        {"sdi card",           sdi_card_dump_info},
        {"sdi card cpld",      sdi_card_dump_cpld_reg},
        {"fpga card",          fpga_card_dump_info},
        {"passthrough card",   passthrough_card_dump_info},
        {"gpu card",           gpu_card_dump_info},
        {"dpu card",           dpu_card_dump_info},
        {"dpu card cpld",      dpu_card_dump_cpld_reg},
        {"MCU",                pcie_dump_info_from_mcu}
    };
    gint32 ret = pcie_card_dump_info(CLASS_PCIECARD_NAME, "Pcie Card Info", path);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Dump pcie card info failed, ret is %d.", ret);
    }
    ret = pcie_card_dump_info(CLASS_OCP_CARD, "OCP Card Info", path);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Dump ocp card info failed, ret is %d.", ret);
    }

    ret = nic_card_dump_info(CLASS_OPTICAL_MODULE, "NIC Card Optical Module Info", path);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Dump nic card optical module info failed, ret is %d.", ret);
    }
 
    for (i = 0; i < G_N_ELEMENTS(dump_log_provider); i++) {
        ret = dump_log_provider[i].pfn_get(path);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Dump %s info failed, ret = %d.", dump_log_provider[i].card_name, ret);
        }
    }
}

 
LOCAL gint32 converge_board_dump_info(const gchar *path)
{
    OBJ_HANDLE obj_handle = 0;
    guint16 board_id = 0;
    gchar cpld_ver[FIRMWARE_VERSION_STRING_LEN] = {0};
    gchar board_name[MAX_NAME_SIZE] = {0};
    gchar pcb_ver[FIRMWARE_VERSION_STRING_LEN] = {0};
    guint32 cpld_unit_num = 0;
    gchar file_name[FILE_NAME_BUFFER_SIZE] = {0};

    
    if (dal_get_object_handle_nth(CLASS_CONVERGE_BOARD, 0, &obj_handle) != RET_OK) {
        return RET_ERR;
    }

    // 在dump_dir目录下创建文件
    gint32 ret = snprintf_s(file_name, sizeof(file_name), sizeof(file_name) - 1, "%s/card_info", path);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    FILE *fp = dal_fopen_check_realpath(file_name, "a+", file_name);
    if (fp == NULL) {
        return RET_ERR;
    }

    (void)fchmod(fileno(fp), COMMON_DUMP_FILE_UMASK);

    (void)dal_get_property_value_string(obj_handle, PROPERTY_CARD_BOARDNAME, board_name, sizeof(board_name));
    (void)dal_get_property_value_uint16(obj_handle, PROPERTY_CARD_BOARDID, &board_id);
    (void)dal_get_property_value_string(obj_handle, PROPERTY_CARD_PCBVER, pcb_ver, sizeof(pcb_ver));
    (void)dal_get_property_value_string(obj_handle, PROPERTY_CONVERGE_BOARD_CPLD_VERSION, cpld_ver, sizeof(cpld_ver));
    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_CONVERGE_BOARD_CPLD_UNIT, &cpld_unit_num);

    ret = fprintf(fp, "Converge Board Info\n");
    if (ret < 0) {
        goto ERR_EXIT;
    }

    // 先写标题栏
    ret = fprintf(fp, "%-16s | %-10s | %-10s | %-16s\n", "BoardName", "BoardId", "PCB Ver", "CPLD Ver");
    if (ret < 0) {
        goto ERR_EXIT;
    }

    // 标题栏下填充值, 打印汇聚板信息
    ret = fprintf(fp, "%-16s | 0x%-4x     | %-10s | (U%u)%s\n\n\n",
        board_name, board_id, pcb_ver, cpld_unit_num, cpld_ver);
    if (ret < 0) {
        goto ERR_EXIT;
    }

    (void)fclose(fp);

    return RET_OK;

ERR_EXIT:
    (void)fclose(fp);
    debug_log(DLOG_ERROR, "[%s]fprintf failed", __FUNCTION__);
    return RET_ERR;
}


LOCAL void board_dump_info(const gchar *path)
{
    gint32 ret;

    ret = hdd_backplane_dump_info(path);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Dump hdd backplane info failed, ret is %d.", ret);
    }

    ret = mem_board_dump_info(path);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Dump mem board info failed, ret is %d.", ret);
    }

    ret = cpu_board_dump_info(path);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Dump cpu board info failed, ret is %d.", ret);
    }

    ret = io_board_dump_info(path);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Dump io board info failed, ret is %d.", ret);
    }

    ret = main_board_dump_info(path);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Dump main board info failed, ret is %d.", ret);
    }

    ret = fan_board_dump_info(path);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Dump FAN board info failed, ret is %d.", ret);
    }

    ret = gpu_board_dump_info(path);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Dump GPU board info failed, ret is %d.", ret);
    }

    ret = converge_board_dump_info(path);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Dump Converge board info failed, ret is %d.", ret);
    }
}


LOCAL void component_dump_info(const gchar *path)
{
    gint32 ret;
    
    
    ret = dump_repeater_all_regs_info(path);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Dump repeater reg info failed, ret is %d.", ret);
    }
    

    ret = dump_sas_regs_info(path);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Dump SAS reg info failed, ret is %d.", ret);
    }

    ret = asset_locator_dump_info(path);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Dump asset locator info failed, ret is %d.", ret);
    }

    ret = retimer_get_logs_dump();
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Dump Retimer serdes log failed, ret is %d.", ret);
    }
}


gint32 card_manage_dump_info(const gchar *path)
{
    if (path == NULL) {
        return RET_ERR;
    }

    
    card_dump_info(path);

    
    board_dump_info(path);

    
    component_dump_info(path);

    return RET_OK;
}

gint32 card_manage_start(void)
{
    init_obj_priv_data_lock();

    
    gint32 ret = pcie_card_load();
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Pcie card load failed!");
    } else {
        
        (void)get_pcie_card_resId();
        
    }

    

    (void)gpu_board_start();

    (void)pcie_card_start();

    (void)pangea_card_start();

    (void)exp_board_start();

    (void)mezz_card_start();

    
    
    (void)sw_card_start();

    

    (void)hdd_backplane_start();

    ret = raid_card_start();
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Raid card start failed!");
    }

    (void)riser_card_start();

    (void)cpu_board_start();

    (void)mem_board_start();

    (void)io_board_start();

    (void)main_board_start();

    
    (void)fan_board_start();

    

    
    (void)sdi_card_start();
    

    (void)fpga_card_start();

    
    (void)passthroughcard_start();
    

    
    network_bandwidth_usage_waveform_init();
    

    
    (void)gpu_card_start();
    

    
    (void)modbus_start();
    

    
    leak_detect_card_start();

    oob_chan_start();
    soc_board_start();
    (void)dpu_card_start();

    (void)update_bmc_build_time_init();

    return RET_OK;
}


#define MAX_CLASS_NAME_LEN       48
#define MAX_HWSRVERSION_STR_LEN  9
#define MAX_B_VERSION            100
LOCAL void card_manage_rw_hwsrversion(void)
{
    GSList *handle_list = NULL;
    GSList *list_item = NULL;
    OBJ_HANDLE object_handle;
    guint32 offset = 0x62; // 版本号所在寄存器地址
    gchar hw_ver[2] = {0};
    gchar m_ver[MAX_HWSRVERSION_STR_LEN] = {0};
    gint32 ret;
    int i;

    gchar hwsrversion_manage_list[][MAX_CLASS_NAME_LEN] = {
        CLASS_HDDBACKPLANE_NAME,
        CLASS_RISERPCIECARD_NAME,
        CLASS_CPUBOARD_NAME,
        CLASS_FANBOARD_NAME,
        CLASS_EXPBOARD_NAME,
    };

    for (i = 0; i < sizeof(hwsrversion_manage_list) / sizeof(hwsrversion_manage_list[0]); i++) {
        ret = dfl_get_object_list(hwsrversion_manage_list[i], &handle_list);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Get object %s handle list failed: %d.", hwsrversion_manage_list[i], ret);
            continue;
        }

        for (list_item = handle_list; list_item; list_item = g_slist_next(list_item)) {
            object_handle = (OBJ_HANDLE)list_item->data;

            ret = dfl_block_read(object_handle, PROPERTY_HWSR_VERSION, offset, sizeof(hw_ver), hw_ver);
            if (ret != DFL_OK) {
                debug_log(DLOG_ERROR, "%s Get HWSRVersion failed:%d.", dfl_get_object_name(object_handle), ret);
                continue;
            }

            // 天池自描述固件版本，A.B格式，A是高字节，B是低字节，约定显示格式如：1.02、0.50等
            (void)sprintf_s(m_ver, sizeof(m_ver), "%d.%02d", hw_ver[1], hw_ver[0] % MAX_B_VERSION);
            ret = dal_set_property_value_string(object_handle, PROPERTY_HWSR_VERSION, m_ver, DF_NONE);
            debug_log(DLOG_INFO, "%s set HWSRVersion %s return:%d.", dfl_get_object_name(object_handle), m_ver, ret);
        }
        g_slist_free(handle_list);
        handle_list = NULL;
    }
    return;
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


LOCAL void converge_board_init(void)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dal_get_object_handle_nth(CLASS_CONVERGE_BOARD, 0, &obj_handle);
    if (ret != RET_OK) {
        return;  // 没有配置对象，只有部分服务器配置有汇聚板（Atlas 800D G1）
    }

    (void)converge_add_object_callback(obj_handle);
}

LOCAL gint32 converge_add_object_callback(OBJ_HANDLE obj_handle)
{
#define MAYJOR_VERSION_SHIFT_BITS   4
#define VERSION_MASK                0x0f
    guint8 cpld_ver = 0;
    gchar buf_temp[PROP_VAL_LENGTH];

    
    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_CONVERGE_BOARD_CPLD_VER_REG, &cpld_ver);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s]get converge board cpld version reg failed, ret = %d", __FUNCTION__, ret);
        return ret;
    }

    
    (void)memset_s(buf_temp, sizeof(buf_temp), 0, sizeof(buf_temp));
    (void)snprintf_s(buf_temp, PROP_VAL_LENGTH, PROP_VAL_LENGTH - 1, "%d.%02d",
        (cpld_ver >> MAYJOR_VERSION_SHIFT_BITS) & VERSION_MASK, cpld_ver & VERSION_MASK);

    ret = dal_set_property_value_string(obj_handle, PROPERTY_CONVERGE_BOARD_CPLD_VERSION, buf_temp, DF_AUTO);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s]set converge board cpld version failed, ret = %d", __FUNCTION__, ret);
    }
    return ret;
}

LOCAL gint32 converge_del_object_callback(OBJ_HANDLE obj_handle)
{
    return RET_OK;
}

gint32 card_manage_init(void)
{
    
    #ifdef ENABLE_ASAN
        if (signal(44, mem_leak_handler) == SIG_ERR) {  // 44 表示执行ASAN检测的信号
            debug_log(DLOG_ERROR, "Could not set signal handler");
        }
    #endif

    init_gcov_signal_for_test();
    
    
    dal_install_sig_action();
    

    (void)gpu_board_init();

    (void)exp_board_init();
    (void)mezz_card_init();

    
    
    (void)sw_card_init();

    
    (void)riser_card_init();

    (void)hdd_backplane_init();

    (void)raid_card_init();

    (void)cpu_board_init();

    (void)mem_board_init();

    (void)io_board_init();

    (void)main_board_init();

    (void)net_card_init();

    
    (void)fan_board_init();

    
    
    (void)m2transformcard_init();
    

    
    (void)pcietransformcard_init();
    

    (void)fpga_card_init();

    
    (void)passthroughcard_init();
    

    // CIC卡管理需求
    (void)cic_card_init();
    
    (void)modbus_init();
    

    retimer_manage_start();
    gpucard_agentless_init();
    pciecard_power_on();
    card_manage_rw_hwsrversion();
    soc_board_init();
    
    converge_board_init();

    return RET_OK;
}


LOCAL gint32 component_add_callback(void)
{
    gint32 result = 0;
    GSList *handle_list = NULL;
    OBJ_HANDLE handle = 0;

    result = dfl_get_object_list(CLASS_PRODUCT_COMPONENT, &handle_list);
    if ((result != RET_OK) || (handle_list == NULL)) {
        debug_log(DLOG_ERROR, "%s get the CLASS_PRODUCT_COMPONENT object list failed", __FUNCTION__);
        return result;
    }

    handle = (OBJ_HANDLE)(handle_list->data);
    g_slist_free(handle_list);

    result = dfl_call_class_method(handle, PRODUCT_COMPONENT_METHOD_UPDATE_COMPONENT_INFO, NULL, NULL, NULL);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "%s:call the method failed", __FUNCTION__);
    }

    return result;
}


gint32 card_manage_add_object_callback(const gchar *class_name, OBJ_HANDLE object_handle)
{
    guint32 i = 0;

    if (class_name == NULL) {
        return RET_ERR;
    }

    
    if (strcmp(class_name, CLASS_REG_ACTION) == RET_OK) {
        if (process_reg_action_with_hotswap_condition(object_handle, NULL) != RET_OK) {
            debug_log(DLOG_ERROR, "%s: Process RegAction fail.", __FUNCTION__);
        }
    }

    if (strcmp(class_name, CLASS_REPEATER_MGNT) == RET_OK) {
        debug_log(DLOG_INFO, "Config Repeater callback");

        
        (void)init_each_repeater_mgnt(object_handle, NULL);
    }

    for (i = 0; i < sizeof(g_card_object_manage_tbl) / sizeof(g_card_object_manage_tbl[0]); i++) {
        if (strcmp(g_card_object_manage_tbl[i].class_name, class_name) == RET_OK) {
            return g_card_object_manage_tbl[i].add_callback_func(object_handle);
        }
    }

    return RET_OK;
}



gint32 card_manage_add_complete_callback(guint32 position)
{
    if (component_add_callback() != RET_OK) {
        debug_log(DLOG_ERROR, " call the method component_add_callback failed ");
    }

    
    // 每1张卡完成加载，进行网络带宽占用率波形图初始化
    network_bandwidth_usage_waveform_init();
    

    return RET_OK;
}



gint32 card_manage_del_object_callback(const gchar *class_name, OBJ_HANDLE object_handle)
{
    guint32 i = 0;

    if (class_name == NULL) {
        return RET_ERR;
    }

    for (i = 0; i < sizeof(g_card_object_manage_tbl) / sizeof(g_card_object_manage_tbl[0]); i++) {
        if (strcmp(g_card_object_manage_tbl[i].class_name, class_name) == RET_OK) {
            return g_card_object_manage_tbl[i].del_callback_func(object_handle);
        }
    }

    return RET_OK;
}



gint32 card_manage_del_complete_callback(guint32 position)
{
    gint32 ret;

    
    if (component_add_callback() != RET_OK) {
        debug_log(DLOG_ERROR, " call the method component_add_callback failed ");
    }

    
    ret = pcie_card_delete_complete_func(position);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "handle pcie card delete complete func failed, ret is %d", ret);
    }

    return RET_OK;
}


gint32 card_manage_updata_card_info(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    guint8 hotswap_state = 0;

    hotswap_state = g_variant_get_byte(property_value);
    if (hotswap_state == HOTSWAP_M4) {
        hdd_backplane_updata_card_info();
        fan_board_updata_card_info();
    }

    return RET_OK;
}


LOCAL gint32 membrd_set_mirror_led_default_value_foreach(OBJ_HANDLE handle, gpointer user_data)
{
    guint8 light_led_off = 1; // 写1 灭灯
    guint8 count = 0;
    gint32 ret = RET_ERR;

    do {
        debug_log(DLOG_DEBUG, "[%s] set %s Value [%d]", __FUNCTION__, dfl_get_object_name(handle), light_led_off);
        ret = dal_set_property_value_byte(handle, MEM_BRD_MIRROR_STATUS, light_led_off, DF_AUTO);
        count++;
    } while ((ret != RET_OK) && (count <= 3)); //  为防止I2C 写失败，最多重复写3 次

    
    return RET_OK;
}


void card_manage_update_os_reset_flag(guint8 reset_state)
{
    g_os_reset_flag = reset_state;
}


gint32 card_manage_get_os_reset_flag(guint8 *reset_state)
{
    if (reset_state == NULL) {
        return RET_ERR;
    }
    *reset_state = g_os_reset_flag;
    return RET_OK;
}

LOCAL gint32 restore_gpuboard_deault_value_foreach(OBJ_HANDLE board_handle, gpointer user_data)
{
    // 恢复PFAEvent值
    (void)dal_set_property_value_byte(board_handle, PROPERTY_PFA_EVENT, 0, DF_SAVE_TEMPORARY);
    // 恢复FDMFault值
    (void)dal_set_property_value_byte(board_handle, PROPERTY_FDM_FAULT_STATE, 0, DF_SAVE_TEMPORARY);

    return RET_OK;
}


LOCAL void card_manage_set_device_status_default_value(OBJ_HANDLE object_handle, gpointer user_data,
    gchar *property_name, GVariant *property_value)
{
    guint8 reset_flag = 0;
    guint8 power_state = 1;
    guint8 sys_reset_flag = 0;

    guint8 fault_state = 0;

    
    if (!strcmp(property_name, PROPERTY_PAYLOAD_CHASSPWR_STATE)) {
        // cold reset flag
        power_state = g_variant_get_byte(property_value);
        debug_log(DLOG_INFO, "Receive power state(%d) change event!", power_state);
    } else if (!strcmp(property_name, PROPERTY_PAYLOAD_RESET_FLAG)) {
        // warm reset flag
        reset_flag = g_variant_get_byte(property_value);
        debug_log(DLOG_INFO, "Receive reset flag(%d) change event!", reset_flag);
    } else if (!strcmp(property_name, PROPERTY_PAYLOAD_SYS_RST_FLAG)) {
        // os reboot flag
        sys_reset_flag = g_variant_get_byte(property_value);
        debug_log(DLOG_INFO, "Receive sys_reset_flag flag(%d) change event!", sys_reset_flag);
    } else {
        return;
    }

    if ((power_state == 0) || (reset_flag == 1) || (sys_reset_flag == 1)) {
        if (vos_thread_async_sem4_p(g_cm_set_device_async) == 0) {
            pcie_card_set_device_status_default_value();
            mem_board_set_device_status_default_value();
            ocp_card_set_device_status_default_value();

            
            raid_set_device_status_default_value();
            net_card_set_device_status_default_value();
            mezz_card_set_device_status_default_value();
            

            
            (void)dfl_foreach_object(CLASS_MEMBOARD_NAME, membrd_set_mirror_led_default_value_foreach, NULL, NULL);
            

            // 恢复GPUBoard属性
            (void)dfl_foreach_object(CLASS_GPUBOARD_NAME, restore_gpuboard_deault_value_foreach, NULL, NULL);
            
            card_manage_update_os_reset_flag(1);
            

            // OS下电时告警恢复，复位时仅产生告警
            if (power_state == 0) {
                card_manage_update_fault_state(0);
            } else if ((reset_flag == 1) || (sys_reset_flag == 1)) {
                fault_state = pcie_card_get_pcieslot_fault_state();
                if (fault_state != 0) {
                    card_manage_update_fault_state(fault_state);
                }
            }
            // 恢复Legacy OPROM resources not enough告警
            reset_os_update_resources_state();

            (void)vos_thread_sem4_v(g_cm_set_device_async);
            debug_log(DLOG_DEBUG, "%s: restore card manage to default", __FUNCTION__);
        }
    }
}


LOCAL void notify_retimer_payload_event(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    guint8 power_state;
    guint8 sys_reset_flag;

    if (strcmp(property_name, PROPERTY_PAYLOAD_CHASSPWR_STATE) == 0) {
        // cold reset flag
        power_state = g_variant_get_byte(property_value);
        notify_retimer_power_state(power_state);
    } else if (strcmp(property_name, PROPERTY_PAYLOAD_SYS_RST_FLAG) == 0) {
        // os reboot flag
        sys_reset_flag = g_variant_get_byte(property_value);
        notify_retimer_sys_reset_state(sys_reset_flag);
    }
}

gint32 card_manage_payload_event_callback(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    card_manage_set_device_status_default_value(object_handle, user_data, property_name, property_value);
    notify_retimer_payload_event(object_handle, user_data, property_name, property_value);

    return RET_OK;
}


gint32 card_manage_reboot_request_process(REBOOT_CTRL ctrl)
{
    retimer_release_i2c();
    dal_common_reboot_request_process(ctrl);

    return RET_OK;
}


LOCAL void card_cache_property_string(json_object *resource_jso, gchar *redfish_prop, gchar *dest, guint32 destlen)
{
    errno_t safe_fun_ret = EOK;
    json_object *value_jso = NULL;
    const gchar *value_str = NULL;
    json_bool ret_bool;

    
    // resource_jso 可以为 NULL
    if ((redfish_prop == NULL) || (dest == NULL) || (destlen == 0)) {
        debug_log(DLOG_ERROR, "%s:card input param error, %d.", __FUNCTION__, destlen);
        return;
    }
    

    ret_bool = json_object_object_get_ex(resource_jso, redfish_prop, &value_jso);
    // 有效值则填充到结构体中,无效则默认为空字符串
    if (ret_bool && (value_jso != NULL)) {
        value_str = dal_json_object_get_str(value_jso);
        if (value_str != NULL) {
            (void)memset_s(dest, destlen, 0, destlen);
            safe_fun_ret = strncpy_s(dest, destlen, value_str, destlen - 1);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            }
        } else {
            (void)memset_s(dest, destlen, 0, destlen);
            (void)strncpy_s(dest, destlen, "N/A", strlen("N/A"));
            debug_log(DLOG_ERROR, "%s:card value_str is null, redfish_prop is %s.", __FUNCTION__, redfish_prop);
        }
    } else {
        (void)memset_s(dest, destlen, 0, destlen);
        (void)strncpy_s(dest, destlen, "N/A", strlen("N/A"));
        debug_log(DLOG_DEBUG, "%s:card value_jso is null, redfish_prop is %s.", __FUNCTION__, redfish_prop);
    }
}


void card_update_property_string(json_object *resource_jso, gchar *redfish_prop, OBJ_HANDLE pme_handle, gchar *pme_prop)
{
    gchar value_str[PROP_VAL_MAX_LENGTH] = {0};

    
    // resource_jso 可以为 NULL
    if ((redfish_prop == NULL) || (pme_prop == NULL)) {
        debug_log(DLOG_ERROR, "%s:input param error.", __FUNCTION__);
        return;
    }
    

    card_cache_property_string(resource_jso, redfish_prop, value_str, sizeof(value_str));

    (void)dal_set_property_value_string(pme_handle, pme_prop, value_str, DF_NONE);
}


void card_update_rootbdf_property_string_with_limit(json_object *bdf_num_jso, gchar *redfish_prop,
    OBJ_HANDLE card_handle, gchar *card_prop)
{
#define BUSY_NETCARD_ROOT_BDF_DEFAULT_VALUE "N/A"

    gchar ibma_report_rootbdf_str[PROP_VAL_LENGTH] = {0};
    gchar netcard_rotbdf_str[PROP_VAL_LENGTH] = {0};
    gchar custom_name[LOG_SPECIAL_STRING_LEN_MAX] = {"iBMA"};

    if ((redfish_prop == NULL) || (card_prop == NULL)) {
        debug_log(DLOG_ERROR, "%s:input param error.", __FUNCTION__);
        return;
    }

    
    if (dal_get_custom_string_name(custom_name, BMC_CUSTOM_LOGIBMA_NAME, sizeof(custom_name)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_custom_string_name fail.", __FUNCTION__);
    }

    (void)dal_get_property_value_string(card_handle, BUSY_NETCARD_ROOT_BDF, netcard_rotbdf_str,
        sizeof(netcard_rotbdf_str));

    if ((strncmp(netcard_rotbdf_str, BUSY_NETCARD_ROOT_BDF_DEFAULT_VALUE,
        strlen(BUSY_NETCARD_ROOT_BDF_DEFAULT_VALUE)) == 0) ||
        (netcard_rotbdf_str[0] == '\0')) {
        card_cache_property_string(bdf_num_jso, redfish_prop, ibma_report_rootbdf_str, sizeof(ibma_report_rootbdf_str));
        (void)dal_set_property_value_string(card_handle, card_prop, ibma_report_rootbdf_str, DF_NONE);
        debug_log(DLOG_INFO, "%s: %s set %s RootBDF value %s.", __FUNCTION__, custom_name,
            dfl_get_object_name(card_handle), ibma_report_rootbdf_str);
    }
}



gint32 card_manage_set_pch_model_from_bios(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    errno_t safe_fun_ret = EOK;

    gint32 ret_val = RET_OK;
    guint32 model_len = 0;
    gchar model_str[MAINBOARD_PCHMODEL_MAX_LEN + 1] = {0};
    const gchar *pmodel_str = NULL;
    GVariant *property_data = NULL;
    gsize parm_temp;

    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input_list is NULL", __FUNCTION__);
        return RET_ERR;
    }

    property_data = (GVariant *)g_slist_nth_data(input_list, 0);
    if (property_data == NULL) {
        debug_log(DLOG_ERROR, "%s:list_value is NULL", __FUNCTION__);
        return RET_ERR;
    }

    pmodel_str = g_variant_get_string(property_data, &parm_temp);
    model_len = (guint32)parm_temp;

    
    if (pmodel_str == NULL) {
        debug_log(DLOG_ERROR, "%s:g_variant_get_string failed", __FUNCTION__);
        return RET_ERR;
    }
    
    if (model_len > MAINBOARD_PCHMODEL_MAX_LEN) {
        debug_log(DLOG_ERROR, "%s:input parameter range error: len is %d", __FUNCTION__, model_len);
        return RET_ERR;
    }

    ret_val = vos_check_incorrect_char(pmodel_str, "", strlen(pmodel_str));
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "vos_check_incorrect_char(%s) failed", pmodel_str);
        return RET_ERR;
    }

    safe_fun_ret = strncpy_s(model_str, sizeof(model_str), pmodel_str, model_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    ret_val = dal_set_property_value_string(object_handle, PROPERTY_MAINBOARD_PCHMODEL, model_str, DF_SAVE);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "dal_set_property_value_string pch model error, ret=%d", ret_val);
        return RET_ERR;
    }

    return ret_val;
}


gint32 card_manage_reload_fan_info(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    (void)fan_board_reload_info();
    return RET_OK;
}


gint32 method_card_manage_set_fanboard_led(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    const guint8 input_para_num = 3; 
    guint8 list_length;
    guint8 fanboard_id;
    guint8 led_red_opcode, led_green_opcode;
    guint16 led_status;
    gint32 ret;

    if (object_handle == 0 || input_list == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is not valid", __FUNCTION__);
        return RET_ERR;
    }

    list_length = g_slist_length(input_list);
    if (list_length != input_para_num) {
        debug_log(DLOG_ERROR, "%s: input parameter cnt error, list len is %d", __FUNCTION__, list_length);
        return RET_ERR;
    }

    
    fanboard_id = g_variant_get_byte((GVariant *)(g_slist_nth_data(input_list, 0)));
    
    led_red_opcode = g_variant_get_byte((GVariant *)(g_slist_nth_data(input_list, 1)));
    
    led_green_opcode = g_variant_get_byte((GVariant *)(g_slist_nth_data(input_list, 2)));

    
    led_status = (led_green_opcode << 8) | led_red_opcode; 
    debug_log(DLOG_INFO, "%s set fanboard%d led switch : 0x%x", __FUNCTION__, fanboard_id, led_status);
    ret = dal_set_property_value_uint16(object_handle, PROPERTY_FANBOARD_LED, led_status, DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set fan board led fail, res = %d", __FUNCTION__, ret);
    }

    return ret;
}


LOCAL void print_component_power_ctrl_operatlog(const void *msg_data, const SET_COMPONENT_POWERS_REQS_S *req_data,
    char *component_type, guint32 ret)
{
#define POWER_STRING_LEN 30
    gchar power_ctrl_print[POWER_STRING_LEN] = {0};
    
    (void)snprintf_s(power_ctrl_print, POWER_STRING_LEN, POWER_STRING_LEN - 1, "Power Control to %uh",
        req_data->cmd_code);

    if (ret == RET_OK) {
        // device_num=0xffff 表示对当前所有Device_type类型的component下电
        if (req_data->device_num == 0xffff) {
            ipmi_operation_log(msg_data, "Set All The %s %s Success.\n", component_type, power_ctrl_print);
        } else {
            ipmi_operation_log(msg_data, "Set The %s %d %s Success.\n", component_type, req_data->device_num,
                power_ctrl_print);
        }
    } else {
        if (req_data->device_num == 0xffff) {
            ipmi_operation_log(msg_data, "Set All The %s %s Fail.\n", component_type, power_ctrl_print);
        } else {
            ipmi_operation_log(msg_data, "Set The %s %d %s Fail.\n", component_type, req_data->device_num,
                power_ctrl_print);
        }
    }
}


gint32 ipmi_cmd_component_power_ctrl(const void *msg_data, gpointer user_data)
{
    guint32 ret;
    COMPONENT_POWERS_RESP_S resp_data = { 0 };
    guint8 comp_code = 0;

    resp_data.manufacturer = HUAWEI_MFG_ID;
    resp_data.comp_code = COMP_CODE_UNKNOWN;

    const SET_COMPONENT_POWERS_REQS_S *req_data = (const SET_COMPONENT_POWERS_REQS_S *)get_ipmi_src_data(msg_data);
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "%s: get ipmi req_data failed", __FUNCTION__);
        resp_data.comp_code = COMP_CODE_UNKNOWN;
        return ipmi_send_response(msg_data, sizeof(COMPONENT_POWERS_RESP_S), (guint8 *)&resp_data);
    }

    if (req_data->manufacturer != HUAWEI_MFG_ID) {
        debug_log(DLOG_ERROR, "%s: invalid manufacturer id", __FUNCTION__);
        resp_data.comp_code = COMP_CODE_INVALID_FIELD;
        return ipmi_send_response(msg_data, sizeof(COMPONENT_POWERS_RESP_S), (guint8 *)&resp_data);
    }

    
    if ((req_data->device_info != DEVICE_INFO_POWER_CONTROL) || (req_data->pre_reserved != 0x00) ||
        (req_data->reserved != 0x00) || (req_data->offset != 0x00) || (req_data->length != 0x01)) {
        debug_log(DLOG_ERROR, "%s: invalid data of req_data", __FUNCTION__);
        resp_data.comp_code = COMP_CODE_OUTOF_RANGE;
        return ipmi_send_response(msg_data, sizeof(COMPONENT_POWERS_RESP_S), (guint8 *)&resp_data);
    }

    
    switch (req_data->device_type) {
        case DEVICE_TYPE_HARDDISK:
            ret = hdd_backplane_power_ctrl(req_data->device_num, req_data->cmd_code, &comp_code);
            print_component_power_ctrl_operatlog(msg_data, req_data, "HardDisk", ret);
            break;
        case DEVICE_TYPE_PCIE_CARD:
            ret = component_default_power_ctrl(req_data->device_type, req_data->device_num, INVALID_DEVICE_POSITION,
                req_data->cmd_code, &comp_code);
            print_component_power_ctrl_operatlog(msg_data, req_data, "PcieCard", ret);
            break;
        case DEVICE_TYPE_GPU_BOARD:
            ret = component_default_power_ctrl(req_data->device_type, req_data->device_num, INVALID_DEVICE_POSITION,
                req_data->cmd_code, &comp_code);
            print_component_power_ctrl_operatlog(msg_data, req_data, "GpuBoard", ret);
            break;
        
        case DEVICE_TYPE_PCIE_SWITCH:
            ret = component_default_power_ctrl(req_data->device_type, req_data->device_num, INVALID_DEVICE_POSITION,
                req_data->cmd_code, &comp_code);
            print_component_power_ctrl_operatlog(msg_data, req_data, "PcieSwitch", ret);
            break;
        
        case COMPONENT_TYPE_EXPANDER:
        case COMPONENT_TYPE_EPM:
            ret = sas_exp_chip_power_ctrl(req_data->device_num, req_data->cmd_code, &comp_code);
            print_component_power_ctrl_operatlog(msg_data, req_data, "Expander", ret);
            break;
        
        default:
            
            debug_log(DLOG_ERROR, "%s: invalid req_data->device_type(%d)", __FUNCTION__, req_data->device_type);
            comp_code = COMP_CODE_OUTOF_RANGE;
    }
    resp_data.comp_code = comp_code;
    return ipmi_send_response(msg_data, sizeof(COMPONENT_POWERS_RESP_S), (guint8 *)&resp_data);
}


gint32 init_board(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    debug_log(DLOG_DEBUG, "Init the board to read version again!");

    // 重新读取ioboard信息
    (void)io_board_init();

    
    (void)cpu_board_init();
    
    return RET_OK;
}



gint32 hddbackplane_set_hdd_temp(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    guint16 hddbp_hdd_temp = 0;
    guint16 hddbp_ssd_temp = 0;
    guint8 hddbp_temp = 0;

    if (input_list == NULL) {
        return RET_ERR;
    }

    hddbp_temp = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    hddbp_hdd_temp = (guint16)hddbp_temp;

    if (hddbp_hdd_temp == 0) {
        hddbp_hdd_temp = 0x4000;
    }

    (void)dal_set_property_value_uint16(object_handle, PROPERTY_HDDBACKPLANE_HDDSASMAXTEMP, hddbp_hdd_temp, DF_NONE);

    hddbp_temp = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1));
    hddbp_ssd_temp = (guint16)hddbp_temp;

    if (hddbp_ssd_temp == 0) {
        hddbp_ssd_temp = 0x4000;
    }

    (void)dal_set_property_value_uint16(object_handle, PROPERTY_HDDBACKPLANE_SSDSASMAXTEMP, hddbp_ssd_temp, DF_NONE);

    return RET_OK;
}


LOCAL gint32 dump_sas_regs_info(const gchar *path)
{
    gint32 iRet = -1;
    guint8 slotid = 0;
    guint32 reg_num = 0;
    guint32 reg_start_offset = 0;
    guint32 chip_addr = 0;
    guint32 ret = 0;
    guint32 offset = 0;
    GSList *list = NULL;
    GSList *obj_node = NULL;
    FILE *fp = NULL;
    gchar file_tmp[FILE_NAME_BUF_LEN] = {0};
    gchar content[CONTENT_BUF_LEN] = {0};
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE sas_chip_obj_handle = 0;

    ret = dfl_get_object_list(CLASS_SASREDRIVER_MGNT, &list);
    if (ret != DFL_OK || list == NULL) {
        debug_log(DLOG_DEBUG, "Get class (%s) handle list failed", CLASS_SASREDRIVER_MGNT);
        return RET_ERR;
    }

    iRet = snprintf_s(file_tmp, FILE_NAME_BUF_LEN, FILE_NAME_BUF_LEN - 1, "%s/SASRedriverRegInfo", path);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    fp = dal_fopen_check_realpath(file_tmp, "w+", file_tmp);
    if (fp == NULL) {
        g_slist_free(list);
        debug_log(DLOG_ERROR, " open file failed . ");
        return RET_ERR;
    }
    (void)fchmod(fileno(fp), COMMON_DUMP_FILE_UMASK);
    debug_log(DLOG_INFO, " pex dump path");

    for (obj_node = list; obj_node; obj_node = obj_node->next) {
        guint32 n = 0;
        obj_handle = (OBJ_HANDLE)obj_node->data;

        (void)dal_get_property_value_byte(obj_handle, PROPERTY_SASREDRIVER_MGNT_SLOTID, &slotid);

        (void)dal_get_property_value_uint32(obj_handle, PROPERTY_SASREDRIVER_MGNT_REGNUMBER, &reg_num);

        (void)dal_get_property_value_uint32(obj_handle, PROPERTY_SASREDRIVER_MGNT_REGSTARTOFFSET, &reg_start_offset);

        (void)snprintf_s(content, CONTENT_BUF_LEN, CONTENT_BUF_LEN - 1, "The Slot %u SASRedriver Reg Info \n", slotid);
        (void)strncat_s(content, sizeof(content),
            "--------------------------------------------------------------------\n",
            strlen("--------------------------------------------------------------------\n"));
        (void)fwrite(content, strlen(content), 1, fp);

        while (1) {
            guint8 read_data[SASREDRIVER_READ_BUF_LEN] = {0} ;
            
            ret = dal_get_refobject_handle_nth(obj_handle, PROPERTY_SASREDRIVER_MGNT_REFCHIP, n, &sas_chip_obj_handle);
            if (ret != RET_OK) {
                break;
            }

            n++;
            (void)dal_get_property_value_uint32(sas_chip_obj_handle, PROPERTY_CHIP_ADDR, &chip_addr);

            (void)snprintf_s(content, CONTENT_BUF_LEN, CONTENT_BUF_LEN - 1, "The Chip Addr is 0x%02x : \n", chip_addr);
            (void)fwrite(content, strlen(content), 1, fp);

            
            ret = dfl_chip_blkread(dfl_get_object_name(sas_chip_obj_handle), reg_start_offset, reg_num,
                (guint8 *)read_data);
            
            if (ret != DFL_OK) {
                debug_log(DLOG_ERROR, "read the %s failed , ret is %d ", dfl_get_object_name(sas_chip_obj_handle),
                    ret);
                iRet = snprintf_s(content, CONTENT_BUF_LEN, CONTENT_BUF_LEN - 1, "Get the Chip %s failed \n",
                    dfl_get_object_name(sas_chip_obj_handle));
                if (iRet <= 0) {
                    debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
                }

                (void)fwrite(content, strlen(content), 1, fp);
            }

            for (offset = 0; offset < reg_num && offset < SASREDRIVER_READ_BUF_LEN; offset++) {
                (void)snprintf_s(content, CONTENT_BUF_LEN, CONTENT_BUF_LEN - 1, "0x%02x reg data is 0x%02x \n", offset,
                    read_data[offset]);
                (void)fwrite(content, strlen(content), 1, fp);
            }

            (void)fwrite("\n", 1, 1, fp);
        }

        (void)snprintf_s(content, CONTENT_BUF_LEN, CONTENT_BUF_LEN - 1,
            "--------------------------------------------------------------------\n\n");
        (void)fwrite(content, strlen(content), 1, fp);
    }

    g_slist_free(list);
    (void)fclose(fp);
    return RET_OK;
}


LOCAL gint32 dump_repeater_all_regs_info(const gchar *path)
{
    gint32 iRet = -1;
    guint32 ret = 0;
    guint32 i = 0;
    guint32 j = 0;
    guint32 offset = 0;

    gchar file_tmp[FILE_NAME_BUF_LEN] = {0};
    gchar content[CONTENT_BUF_LEN] = {0};
    gchar device_name[DEVICE_NAME_LEN] = {0} ;

    const gchar *obj_name = NULL;
    const guchar *reg_action_array = NULL;
    const gchar **refchiparray = NULL;
    const gchar **logicunitarray = NULL;

    GSList *list = NULL;
    GSList *obj_node = NULL;
    GVariant *gvar = NULL;
    GVariant *grefchiparray = NULL;
    GVariant *glogicunitarray = NULL;
    FILE *fp = NULL;

    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE chip_obj_handle = 0;
    OBJ_HANDLE reg_action_handle = 0;

    gsize len = 0;
    gsize repeater_count = 0;
    gsize logicunit_count = 0;

    ret = dfl_get_object_list(CLASS_REPEATER_MGNT, &list);
    if (ret != DFL_OK || list == NULL) {
        debug_log(DLOG_DEBUG, "Get class (%s) handle list failed", CLASS_REPEATER_MGNT);
        return RET_ERR;
    }

    iRet = snprintf_s(file_tmp, FILE_NAME_BUF_LEN, FILE_NAME_BUF_LEN - 1, "%s/RepeaterRegInfo", path);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }

    fp = dal_fopen_check_realpath(file_tmp, "w+", file_tmp);
    if (fp == NULL) {
        g_slist_free(list);
        debug_log(DLOG_ERROR, " open file failed . ");
        return RET_ERR;
    }
    (void)fchmod(fileno(fp), COMMON_DUMP_FILE_UMASK);
    debug_log(DLOG_INFO, " pex dump path");

    for (obj_node = list; obj_node; obj_node = obj_node->next) {
        obj_handle = (OBJ_HANDLE)obj_node->data;

        ret = dfl_get_property_value(obj_handle, PROPERTY_REPEATER_MGNT_REG_ACTION_INDEX, &gvar);
        if (ret != DFL_OK || gvar == NULL) {
            debug_log(DLOG_ERROR, "Get property RefRegActionIndex value failed, ret = %d.", ret);
            continue;
        }

        reg_action_array = (const guchar *)g_variant_get_fixed_array(gvar, &len, sizeof(guchar));
        
        if (reg_action_array == NULL) {
            debug_log(DLOG_ERROR, "%s:%d:reg_action_array:NULL ", __FUNCTION__, __LINE__);
            g_variant_unref(gvar);
            gvar = NULL;
            continue;
        }

        for (i = 0; i < len; i++) {
            
            ret = dal_get_specific_object_byte(CLASS_REG_ACTION, PROPERTY_REG_ACTION_OVERALL_INDEX, reg_action_array[i],
                &reg_action_handle);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR,
                    "Related RegAction obj of RepeaterMgnt is not present, obj index is  %d ,ret is %d.",
                    reg_action_array[i], ret);
                continue;
            }

            obj_name = dfl_get_object_name(reg_action_handle);

            ret = dal_get_property_value_string(reg_action_handle, PROPERTY_REG_ACTION_DEVICE_NAME, device_name,
                DEVICE_NAME_LEN);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "get the DeviceName of %s failed,ret is %d", obj_name, ret);
                continue;
            }

            iRet = snprintf_s(content, CONTENT_BUF_LEN, CONTENT_BUF_LEN - 1, "\n The %s Reg Info\n", device_name);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
            }

            (void)strncat_s(content, sizeof(content),
                "--------------------------------------------------------------------\n",
                strlen("--------------------------------------------------------------------\n"));
            (void)fwrite(content, strlen(content), 1, fp);

            
            ret = dfl_get_property_value(reg_action_handle, PROPERTY_REG_ACTION_REF_CHIP, &grefchiparray);
            if (ret != DFL_OK || grefchiparray == NULL) {
                debug_log(DLOG_ERROR, "get %s of %s failed,ret=%d", dfl_get_object_name(reg_action_handle),
                    PROPERTY_REG_ACTION_REF_CHIP, ret);
                continue;
            }

            refchiparray = g_variant_get_strv(grefchiparray, &repeater_count);
            
            if (refchiparray == NULL) {
                debug_log(DLOG_ERROR, "%s:%d:refchiparray:NULL ", __FUNCTION__, __LINE__);
                if (grefchiparray != NULL) {
                    g_variant_unref(grefchiparray);
                }
                continue;
            }

            ret = dfl_get_property_value(reg_action_handle, PROPERTY_REG_ACTION_LOGIC_UNIT_ARRAY, &glogicunitarray);
            
            if (ret != DFL_OK || glogicunitarray == NULL) {
                debug_log(DLOG_ERROR, "get %s of %s failed,ret=%d", dfl_get_object_name(reg_action_handle),
                    PROPERTY_REG_ACTION_LOGIC_UNIT_ARRAY, ret);
                if (grefchiparray != NULL) {
                    g_variant_unref(grefchiparray);
                }
                g_free(refchiparray);
                continue;
            }
            

            logicunitarray = g_variant_get_strv(glogicunitarray, &logicunit_count);
            
            if (logicunitarray == NULL) {
                debug_log(DLOG_ERROR, "%s:%d:logicunitarray:NULL ", __FUNCTION__, __LINE__);
                if ((grefchiparray != NULL)) {
                    g_variant_unref(grefchiparray);
                }

                if ((glogicunitarray != NULL)) {
                    g_variant_unref(glogicunitarray);
                }

                g_free(refchiparray);
                continue;
            }

            for (j = 0; j < repeater_count; j++) {
                guint8 read_data[REPEATER_READ_BUF_LEN] = {0} ;
                iRet = snprintf_s(content, CONTENT_BUF_LEN, CONTENT_BUF_LEN - 1, "\n %s(UnitNum is %s) Reg Info\n",
                    refchiparray[j], logicunitarray[j]);
                if (iRet <= 0) {
                    debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
                }
                (void)strncat_s(content, sizeof(content),
                    "********************************************************************\n",
                    strlen("********************************************************************\n"));
                (void)fwrite(content, strlen(content), 1, fp);

                ret = dfl_get_object_handle(refchiparray[j], &chip_obj_handle);
                if (ret != DFL_OK) {
                    debug_log(DLOG_ERROR, "get the obj_handle of %s failed,ret is %d", refchiparray[j], ret);
                    continue;
                }
                
                ret = dfl_chip_blkread(dfl_get_object_name(chip_obj_handle), 0, REPEATER_READ_BUF_LEN,
                    (guint8 *)read_data);
                
                if (ret != DFL_OK) {
                    debug_log(DLOG_ERROR, "read the %s failed , ret is %d ", dfl_get_object_name(chip_obj_handle),
                        ret);
                }

                for (offset = 0; offset < REPEATER_READ_BUF_LEN; offset++) {
                    (void)snprintf_s(content, CONTENT_BUF_LEN, CONTENT_BUF_LEN - 1, "0x%02x reg data is 0x%02x \n",
                        offset, read_data[offset]);
                    (void)fwrite(content, strlen(content), 1, fp);
                }
            }

            g_variant_unref(grefchiparray);
            g_variant_unref(glogicunitarray);
            
            g_free(refchiparray);
            g_free(logicunitarray);
            
        }

        g_variant_unref(gvar);
        gvar = NULL;
    }

    g_slist_free(list);
    (void)fclose(fp);
    return RET_OK;
}



gint32 card_manage_set_pfa_event(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = RET_OK;
    guint8 event_val = 0;
    GVariant *property_data = NULL;

    if (input_list == NULL) {
        return RET_ERR;
    }

    property_data = (GVariant *)g_slist_nth_data(input_list, 0);
    event_val = g_variant_get_byte(property_data);
    if (event_val != 0 && event_val != 1) {
        debug_log(DLOG_ERROR, "Invalid parameter.");
        return RET_ERR;
    }

    ret = dal_set_property_value_byte(object_handle, PROPERTY_PFA_EVENT, event_val, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set PFAEvent property failed: %d.", ret);
    }

    return ret;
}



gint32 card_manage_set_pcie_link_ability(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = 0;
    const gchar *link_width_ability = NULL;
    const gchar *link_speed_ability = NULL;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input param null.", __FUNCTION__);
        return RET_ERR;
    }

    link_width_ability = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), NULL);
    link_speed_ability = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 1), NULL);

    ret =
        dal_set_property_value_string(object_handle, PROPETRY_PCIECARD_LINK_WIDTH_ABILITY, link_width_ability, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, Set net card (%s) LinkWidthAbility failed, ret = %d.", __FUNCTION__,
            dfl_get_object_name(object_handle), ret);
        return RET_ERR;
    }
    ret =
        dal_set_property_value_string(object_handle, PROPETRY_PCIECARD_LINK_SPEED_ABILITY, link_speed_ability, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, Set net card (%s) LinkSpeedAbility failed, ret = %d.", __FUNCTION__,
            dfl_get_object_name(object_handle), ret);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 card_manage_set_pcie_link_info(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = 0;
    const gchar *link_width = NULL;
    const gchar *link_speed = NULL;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input param null.", __FUNCTION__);
        return RET_ERR;
    }

    link_width = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), NULL);
    link_speed = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 1), NULL);

    ret = dal_set_property_value_string(object_handle, PROPETRY_PCIECARD_LINK_WIDTH, link_width, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, Set net card (%s) LinkWidth failed, ret = %d.", __FUNCTION__,
            dfl_get_object_name(object_handle), ret);
        return RET_ERR;
    }
    ret = dal_set_property_value_string(object_handle, PROPETRY_PCIECARD_LINK_SPEED, link_speed, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, Set net card (%s) LinkSpeed failed, ret = %d.", __FUNCTION__,
            dfl_get_object_name(object_handle), ret);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 card_manage_set_pcie_chip_temp(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = 0;
    guint16 chip_temp = 0;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input param null.", __FUNCTION__);
        return RET_ERR;
    }

    chip_temp = g_variant_get_uint16((GVariant *)g_slist_nth_data(input_list, 0));

    ret = dal_set_property_value_uint16(object_handle, PROPERTY_PCIE_CARD_CHIPTEMP, chip_temp, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, Set net card (%s) chip temp failed, ret = %d.", __FUNCTION__,
            dfl_get_object_name(object_handle), ret);
        return RET_ERR;
    }

    return RET_OK;
}

gint32 process_as_status_change(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
                                GVariant *property_value)
{
    guint8 as_status = g_variant_get_byte(property_value);
    if (as_status != ACTIVE_STATE) {
        debug_log(DLOG_ERROR, "[%s]Current board is slave, no need to reload info.", __FUNCTION__);
        return RET_OK;
    }
    debug_log(DLOG_ERROR, "[%s]Current board is master and reload fanboard and arcard info.", __FUNCTION__);
    (void)dfl_foreach_object(CLASS_AR_CARD_NAME, update_ar_card_id, NULL, NULL);
    return fan_board_init();
}

gint32 update_bmc_build_time_init(void)
{
#define BMC_TEMP_BUF_LEN 128
    gint32 ret_val = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar buf_temp[BMC_TEMP_BUF_LEN];
    GSList *input_list = NULL;

    ret_val = dfl_get_object_handle(BMC_OBJECT_NAME, &obj_handle);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return RET_ERR;
    }

    (void)memset_s(buf_temp, sizeof(buf_temp), 0, sizeof(buf_temp));
    ret_val = snprintf_s(buf_temp, BMC_TEMP_BUF_LEN, BMC_TEMP_BUF_LEN - 1, "%s %s", __TIME__, __DATE__);
    if (ret_val <= 0) {
        debug_log(DLOG_ERROR, "%s: time and date snprintf_s fail, ret = %d", __FUNCTION__, ret_val);
    }
    debug_log(DLOG_INFO, "The BMC firmware build time is : %s", buf_temp);

    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)buf_temp));
    ret_val = dfl_call_class_method(obj_handle, METHOD_UPDATE_BMC_BUILD_TIME, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: call METHOD_UPDATE_BMC_BUILD_TIME fail, ret %d.", __FUNCTION__, ret_val);
    }

    return ret_val;
}
