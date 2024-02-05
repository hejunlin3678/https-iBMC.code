

#include "ipmcget_component_version.h"
#include "ipmcget_power.h"


struct card_type_info g_cardTypeInfoList[] = {
    {"SW Card",    "SW Card INFO",   CARD_TYPE_SW,     TRUE},
    {"PIC Card",   "PIC Card INFO",  CARD_TYPE_PIC,    TRUE},
    {"PowerBoard", "PWR Board INFO", CARD_TYPE_PWRBKP, TRUE},
    {"MESH Card",  "MESH Card INFO", CARD_TYPE_MESH,   TRUE},
    {NULL},
};
#define PROP_LEN 128

void print_component_uid(OBJ_HANDLE obj_handle, const gchar* component_name, guint32 slot_id,
    guint32 left_alignment)
{
    gchar unique_id[PROP_LEN] = {0};
    (void)dal_get_property_value_string(obj_handle, PROPERTY_COMPONENT_UNIQUE_ID, unique_id, sizeof(unique_id));
    if (dal_check_component_uid_valid((const gchar*)unique_id) == TRUE)  {
        if (slot_id == INVALID_UINT32) {
            g_printf("%-*s CUID:           %s\r\n", left_alignment, component_name, unique_id);
        } else {
            g_printf("%s%-*d CUID:           %s\r\n", component_name, left_alignment, slot_id, unique_id);
        }
    }
}


void print_passthrough_card_version(void)
{
    gint32 ret;
    GSList *card_list = NULL;
    GSList *obj_note = NULL;
    guint16 board_id = 0;
    const gchar *card_name = NULL;
    const gchar *pcb_version = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;

    ret = dfl_get_object_list(CLASS_PASSTHROUGH_CARD, &card_list);
    if (ret != DFL_OK || g_slist_length(card_list) == 0) {
        return;
    }

    g_printf("------------ Pass Through Card INFO ------------\r\n");

    for (obj_note = card_list; obj_note; obj_note = obj_note->next) {
        
        property_name_list = g_slist_append(property_name_list, PROPERTY_CARD_NAME);
        
        property_name_list = g_slist_append(property_name_list, PROPERTY_CARD_BOARDID);
        property_name_list = g_slist_append(property_name_list, PROPERTY_CARD_PCBVER);
        ret = dfl_multiget_property_value((OBJ_HANDLE)obj_note->data, property_name_list, &property_value_list);
        if (ret != DFL_OK) {
            g_slist_free(property_name_list);
            g_slist_free(card_list);
            return;
        }

        card_name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 0), 0);
        board_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value_list, 1));
        pcb_version = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 2), 0);

        g_printf("%-14s BoardID:           0x%04x\r\n", card_name, board_id);

        if ((strlen(pcb_version) != 0) && (strcmp(pcb_version, "N/A") != 0)) {
            g_printf("%-18s PCB:           %s\r\n", card_name, pcb_version);
        }

        g_slist_free(property_name_list);
        uip_free_gvariant_list(property_value_list);

        property_name_list = NULL;
        property_value_list = NULL;
    }

    g_slist_free(card_list);
    return;
}


void print_bmc_version(void)
{
    OBJ_HANDLE bmc_handle = 0;
    gint32 ret;
    gchar cpu_type[PROP_LEN] = {0};
    gchar ipmi_ver[PROP_LEN] = {0};
    gchar cpld_ver[PROP_LEN] = {0};
    guint32 cpld_unit_num = 0;
    guint8 major_ver = 0;
    guint8 minor_ver = 0;
    guint32 unit_num = 0;
    gchar driver_ver[PROP_LEN] = {0};
    gchar uboot_ver[PROP_LEN] = {0};
    gchar uboot_back_ver[PROP_LEN] = {0};
    gchar bak_pme_ver[PROP_LEN] = {0};
    gchar driver_time[PROP_LEN] = {0};
    gchar built_time[PROP_LEN] = {0};
    guint8 build_no = 0;
    OBJ_HANDLE vrd_handle = 0;
    OBJ_HANDLE chip_handle = 0;
    gchar cpld2_ver[16] = {0};
    guint32 cpld2_unit_num = 0;
    gchar *product_type_name = PRODUCT_TYPE_IBMC_NAME;
    gchar custom_name[LOG_SPECIAL_STRING_LEN_MAX] = {"iBMC"};

    
    if (uip_replace_bmc_version_ibmc_string(custom_name, sizeof(custom_name)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: replace_bmc_version_string_name failed.", __FUNCTION__);
    }
    product_type_name = custom_name;

    GVariant *property_value_bom = NULL;
    guchar MgmtSoftWareType = 0;
    guint8 bom_ver = 0;

#ifdef ARM64_HI1711_ENABLED
    gchar available_ver[PROP_LEN] = {0};
    guint8 available_ver_built_num = 0;
    gchar l0_fw_ver[PROP_LEN] = {0};
    gchar l0_fw_built_time[PROP_LEN] = {0};
    gchar l0_backup_fw_ver[PROP_LEN] = {0};
    gchar l0_backup_fw_built_time[PROP_LEN] = {0};
    gchar l1_fw_ver[PROP_LEN] = {0};
    gchar l1_fw_built_time[PROP_LEN] = {0};
    gchar l1_backup_fw_ver[PROP_LEN] = {0};
    gchar l1_backup_fw_built_time[PROP_LEN] = {0};
    guint8 pme_aux_major_ver = 0;
    guint8 pme_aux_minor_ver = 0;
#define AUXMAJORVERINDEX 24
#define AUXMINORVERINDEX 25
#endif

    if (dal_get_software_type(&MgmtSoftWareType) != RET_OK) {
        return;
    }

    
    if (MgmtSoftWareType == MGMT_SOFTWARE_TYPE_EM) {
        
        product_type_name = "SMM ";
        if (uip_get_object_property(CLASS_MAINBOARD_NAME, CLASS_MAINBOARD_NAME, PROPERTY_MAINBOARD_BOMID,
            &property_value_bom) != RET_OK) {
            g_printf("Get %s bom version failed.\r\n", product_type_name);
            return;
        }
        bom_ver = g_variant_get_byte(property_value_bom);
        g_variant_unref(property_value_bom);
        property_value_bom = NULL;
    }
    // iRM软件显示字段区别于iBMC
    if (MgmtSoftWareType == MGMT_SOFTWARE_TYPE_RM) {
        
        product_type_name = "iRM ";
    }

    
    if (dal_get_object_handle_nth(CLASS_NAME_BMC, 0, &bmc_handle) != RET_OK) {
        g_printf("Get %s version failed.\r\n", product_type_name);
        return;
    }
    (void)dal_get_property_value_string(bmc_handle, PROPERTY_BMC_CPUTYPE, cpu_type, sizeof(cpu_type));
    (void)dal_get_property_value_string(bmc_handle, PROPERTY_BMC_IPMIVER, ipmi_ver, sizeof(ipmi_ver));
    (void)dal_get_property_value_string(bmc_handle, PROPERTY_BMC_CPLDVER, cpld_ver, sizeof(cpld_ver));
    (void)dal_get_property_value_uint32(bmc_handle, PROPERTY_BMC_CPLDUNITNUM, &cpld_unit_num);
    (void)dal_get_property_value_byte(bmc_handle, PROPERTY_BMC_PMEMAJORVER, &major_ver);
    (void)dal_get_property_value_byte(bmc_handle, PROPERTY_BMC_PMEMINORVER, &minor_ver);
    (void)dal_get_property_value_uint32(bmc_handle, PROPERTY_BMC_FLASHUNITNUM, &unit_num);
    (void)dal_get_property_value_string(bmc_handle, PROPERTY_BMC_BAKPMEVER, bak_pme_ver, sizeof(bak_pme_ver));
    (void)dal_get_property_value_string(bmc_handle, PROPERTY_BMC_DRIVERVER, driver_ver, sizeof(driver_ver));
    (void)dal_get_property_value_string(bmc_handle, PROPERTY_BMC_DRIVERVERBUILTTIME, driver_time, sizeof(driver_time));
    (void)dal_get_property_value_string(bmc_handle, PROPERTY_BMC_UOOTVER, uboot_ver, sizeof(uboot_ver));
    (void)dal_get_property_value_string(bmc_handle, PROPERTY_BMC_PMEBUILTTIME, built_time, sizeof(built_time));
    (void)dal_get_property_value_string(bmc_handle, PROPERTY_BMC_UOOTBACKUPVER, uboot_back_ver, sizeof(uboot_back_ver));
    (void)dal_get_property_value_byte(bmc_handle, PROPERTY_BMC_BUILD_NO, &build_no);

#ifdef ARM64_HI1711_ENABLED
    (void)dal_get_property_value_string(bmc_handle, PROPERTY_BMC_AVAILABLE_VER, available_ver, sizeof(available_ver));
    (void)dal_get_property_value_byte(bmc_handle, PROPERTY_AVAILABLE_VER_BUILD_NUMBER, &available_ver_built_num);
    (void)dal_get_property_value_string(bmc_handle, PROPERTY_L0_FW_VERSION, l0_fw_ver, sizeof(l0_fw_ver));
    (void)dal_get_property_value_string(bmc_handle, PROPERTY_L0_FW_BUILT, l0_fw_built_time, sizeof(l0_fw_built_time));
    (void)dal_get_property_value_string(bmc_handle, PROPERTY_L0_FW_BACKUP_VER, l0_backup_fw_ver,
        sizeof(l0_backup_fw_ver));
    (void)dal_get_property_value_string(bmc_handle, PROPERTY_L0_FW_BACKUP_VER_BUILT, l0_backup_fw_built_time,
        sizeof(l0_backup_fw_built_time));
    (void)dal_get_property_value_string(bmc_handle, PROPERTY_L1_FW_VERSION, l1_fw_ver, sizeof(l1_fw_ver));
    (void)dal_get_property_value_string(bmc_handle, PROPERTY_L1_FW_BUILT, l1_fw_built_time, sizeof(l1_fw_built_time));
    (void)dal_get_property_value_string(bmc_handle, PROPERTY_L1_FW_BACKUP_VER, l1_backup_fw_ver,
        sizeof(l1_backup_fw_ver));
    (void)dal_get_property_value_string(bmc_handle, PROPERTY_L1_FW_BACKUP_VER_BUILT, l1_backup_fw_built_time,
        sizeof(l1_backup_fw_built_time));
    (void)dal_get_property_value_byte(bmc_handle, PROPERTY_BMC_PMEAUXMAJORVER, &pme_aux_major_ver);
    (void)dal_get_property_value_byte(bmc_handle, PROPERTY_BMC_PMEAUXMINORVER, &pme_aux_minor_ver);
#endif

    
    g_printf("IPMC               CPU:           %s\r\n", cpu_type);
    g_printf("IPMI           Version:           %s\r\n", ipmi_ver);

    
    if (cpld_unit_num != 0) {
        g_printf("CPLD           Version:           (U%u)%s\r\n", cpld_unit_num, cpld_ver);
    }

    do {
        
        ret = dal_get_object_handle_nth(CLASS_VRD_UPGRADE_MGNT, 0, &vrd_handle);
        if (ret != RET_OK) {
            break;
        }

        
        ret = dfl_get_referenced_object(vrd_handle, PROPERTY_VRD_UPGRADE_REFER_CHIP, &chip_handle);
        if (ret != DFL_OK || chip_handle == 0) {
            ret = RET_ERR;
            break;
        }

        (void)dal_get_property_value_uint32(vrd_handle, PROPERTY_VRD_CPLD2_UNIT_NUMBER, &cpld2_unit_num);
        
        
        (void)dal_get_property_value_string(vrd_handle, PROPERTY_VRD_CPLD2_VERSION, cpld2_ver, sizeof(cpld2_ver));
        
    } while (0);

    if (ret == RET_OK) {
        g_printf("CPLD2          Version:           (U%u)%s\r\n", cpld2_unit_num, cpld2_ver);
    }

#ifdef ARM64_HI1711_ENABLED
    g_printf("Active %s    Version:           (U%u)%d.%02d.%02d.%02d\r\n", product_type_name, unit_num,
        major_ver, minor_ver, pme_aux_major_ver, pme_aux_minor_ver);
#else
    g_printf("Active %s    Version:           (U%u)%d.%02d\r\n", product_type_name, unit_num, major_ver, minor_ver);
#endif

    g_printf("Active %s      Build:           %03d\r\n", product_type_name, build_no);
    g_printf("Active %s      Built:           %s\r\n", product_type_name, built_time);
    g_printf("Backup %s    Version:           %s\r\n", product_type_name, bak_pme_ver);

#ifdef ARM64_HI1711_ENABLED
    g_printf("Available %s Version:           %s\r\n", product_type_name, available_ver);
    g_printf("Available %s   Build:           %03d\r\n", product_type_name, available_ver_built_num);
#endif

    g_printf("SDK            Version:           %s\r\n", driver_ver);
    g_printf("SDK              Built:           %s\r\n", driver_time);

    
    if (MgmtSoftWareType == MGMT_SOFTWARE_TYPE_EM) {
        g_printf("BOM            Version:           %03d\r\n", bom_ver);
    }

    g_printf("Active Uboot   Version:           %s\r\n", uboot_ver);
    g_printf("Backup Uboot   Version:           %s\r\n", uboot_back_ver);

#ifdef ARM64_HI1711_ENABLED
    g_printf("Active Secure Bootloader Version: %s (%s)\r\n", l0_fw_ver, l0_fw_built_time);
    g_printf("Backup Secure Bootloader Version: %s (%s)\r\n", l0_backup_fw_ver, l0_backup_fw_built_time);
    g_printf("Active Secure Firmware   Version: %s (%s)\r\n", l1_fw_ver, l1_fw_built_time);
    g_printf("Backup Secure Firmware   Version: %s (%s)\r\n", l1_backup_fw_ver, l1_backup_fw_built_time);
#endif

    return;
}


void print_bios_version(void)
{
    guint8 host_num = 0;
    (void)dal_get_func_ability(CLASS_PRODUCT_COMPONENT, PROPERTY_COMPONENT_HOST_NUM, &host_num);
    if (host_num > 1) {
        return;
    }

    OBJ_HANDLE cfg_handle = 0;
    (void)dfl_get_object_handle(OBJ_NAME_PME_SERVICECONFIG, &cfg_handle);
    guint8 arm_enbale = 0;
    gint32 ret = dal_get_property_value_byte(cfg_handle, PROTERY_PME_SERVICECONFIG_ARM, &arm_enbale);
    if (ret != RET_OK) {
        g_printf("Get ARM Enable property failed.\r\n");
        return;
    }

    guint32 product_version_num = 0;
    (void)dal_get_product_version_num(&product_version_num);
    if (arm_enbale && product_version_num <= PRODUCT_VERSION_V5) { 
        
        (void)get_M3_version();
    }

    
    // BIOS           Version:           (U102)V051
    guint8 x86_enbale = 0;
    ret = dal_get_property_value_byte(cfg_handle, PROTERY_PME_SERVICECONFIG_X86, &x86_enbale);
    if (ret != RET_OK) {
        g_printf("Get X86 Enable property failed.\r\n");
        return;
    }
    if (x86_enbale == 0) {
        return;
    }

    
    
    OBJ_HANDLE obj_handle = 0;
    ret = dal_get_object_handle_nth(OBJ_NAME_BIOS, 0, &obj_handle);
    if (ret != RET_OK) {
        g_printf("Get BIOS version failed.\r\n");
        return;
    }

    gchar bios_ver[MAX_COMPONENT_STR_LEN] = { 0 };
    guint32 bios_unit_num = 0;
    (void)dal_get_property_value_string(obj_handle, PROTERY_BIOS_VERSION, bios_ver, sizeof(bios_ver));
    (void)dal_get_property_value_uint32(obj_handle, PROTERY_BIOS_UNITNUM, &bios_unit_num);
#ifdef ARM64_HI1711_ENABLED
    gchar backup_bios_ver[MAX_COMPONENT_STR_LEN] = { 0 };
    (void)dal_get_property_value_string(obj_handle, BIOS_BACKUP_VER_NAME, backup_bios_ver, sizeof(backup_bios_ver));
    g_printf("Active BIOS    Version:           (U%d)%s\r\n", bios_unit_num, bios_ver);
    g_printf("Backup BIOS    Version:           %s\r\n", backup_bios_ver);
#else
    g_printf("BIOS           Version:           (U%d)%s\r\n", bios_unit_num, bios_ver);
#endif
    
}


void print_ipmb_address(void)
{
    guint8 ipmb_addr = 0;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    GVariant *property_value = NULL;

    gint32 ret = dfl_get_object_list(CLASS_NAME_IPMBADDR, &obj_list);
    if (ret != DFL_OK) {
        return;
    }

    if (g_slist_length(obj_list) == 0) {
        return;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        ret = dfl_get_property_value((OBJ_HANDLE)obj_note->data, IPMBADDR_PROPERTY_SA, &property_value);
        if (ret != DFL_OK) {
            g_printf("Get IPMB address failed.\r\n");
            g_slist_free(obj_list);
            return;
        }

        ipmb_addr = g_variant_get_byte(property_value);
        g_printf("IPMB           Address:           0x%02X\r\n", ipmb_addr);
        g_variant_unref(property_value);
        property_value = NULL;
    }

    g_slist_free(obj_list);
    return;
}



void print_fabric_version(void)
{
    GSList *switch_list = NULL;
    GSList *switch_node = NULL;
    OBJ_HANDLE obj_handle = 0;
    guint8 fru_id;
    guint8 ver_type;
    guint32 unit_num;
    gchar fabric_ver[MAX_COMPONENT_STR_LEN] = {0};

    gint32 ret = dfl_get_object_list(CLASS_NAME_DFTVERSION, &switch_list);
    if (ret != DFL_OK || switch_list == NULL) {
        return;
    }

    for (switch_node = switch_list; switch_node; switch_node = switch_node->next) {
        obj_handle = (OBJ_HANDLE)switch_node->data;
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_FRU_ID, &fru_id);
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_VERSION_TYPE, &ver_type);

        if (fru_id != FABRIC_FRU_ID || ver_type != VERSION_TYPE_FABRIC) {
            continue;
        }

        (void)dal_get_property_value_uint32(obj_handle, PROPERTY_UNIT_NUM, &unit_num);
        (void)dal_get_property_value_string(obj_handle, PROPERTY_VERSION_STR, fabric_ver, sizeof(fabric_ver));
        g_printf("----------------- Fabric INFO ------------------\r\n");
        g_printf("Fabric  Chip   Version:          (U%d)%s\r\n", unit_num, fabric_ver);
        break;
    }

    g_slist_free(switch_list);

    return;
}



void print_m2_transform_card_version(void)
{
    GSList* obj_list = NULL;
    GSList* obj_node = NULL;
    gchar   board_name[MAX_NAME_SIZE] = {0};
    gchar   pcb_ver[MAX_NAME_SIZE] = {0};
    guint16 board_id = 0;
    gint32 ret = dfl_get_object_list(CLASS_M2TRANSFORMCARD, &obj_list);
    if (ret != DFL_OK || g_slist_length(obj_list) == 0) {
        return;
    }
    g_printf("--------------- M.2 Adapter INFO ----------------\r\n");
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_CARD_BOARDNAME,
            board_name, sizeof(board_name));
        (void)dal_get_property_value_uint16((OBJ_HANDLE)obj_node->data, PROPERTY_CARD_BOARDID, &board_id);
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_CARD_PCBVER,
            pcb_ver, sizeof(pcb_ver));

        g_printf("%-14s BoardID:           0x%04x\r\n", board_name, board_id);
        if ((strlen(pcb_ver) != 0) && (strcmp(pcb_ver, "N/A") != 0)) {
            g_printf("%-18s PCB:           %s\r\n", board_name, pcb_ver);
        }
    }
    g_slist_free(obj_list);
    return;
}


void print_nic_boardid_pcb_version(void)
{
    OBJ_HANDLE nic_handle = 0;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    gchar pcb_ver[PROP_LEN] = {0};
    guint16 board_id = 0;
    gchar blade_name[PROP_LEN] = {0};
    gboolean bprint_title = TRUE;
    guint8 fru_type = 0;
    guint8 out_of_width = 0;

    gint32 ret = dfl_get_object_list(CLASS_FRU, &obj_list);
    if (ret != DFL_OK || g_slist_length(obj_list) == 0) {
        return;
    }

    
    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        nic_handle = (OBJ_HANDLE)obj_note->data;
        (void)dal_get_property_value_string(nic_handle, PROPERTY_FRU_NAME, blade_name, sizeof(blade_name));
        (void)dal_get_property_value_string(nic_handle, PROPERTY_FRU_PCB_VERSION, pcb_ver, sizeof(pcb_ver));
        (void)dal_get_property_value_uint16(nic_handle, PROPERTY_FRU_BOARDID, &board_id);
        (void)dal_get_property_value_byte(nic_handle, PROPERTY_FRU_TYPE, &fru_type);

        
        if ((fru_type != FRU_TYPE_LOM_NIC) && (fru_type != FRU_TYPE_PCIE_NIC)) {
            continue;
        }

        if (bprint_title == TRUE) {
            bprint_title = FALSE;
            g_printf("------------------- NIC INFO -------------------\r\n");
        }
        
        if (strlen(blade_name) > 14) {  // 部分天池网卡有网卡名称超过14，导致打印错位，如（NIC $ (SF221Q))
            out_of_width = strlen(blade_name) - 14;  // 当网卡长度小于临界值14时，计算对齐board_id增加空格
            g_printf("%-14s BoardID:%*s0x%04x\r\n", blade_name, 11 - out_of_width,   // 11是正常打印情况下的空格个数
                " ", board_id);
        } else {
            g_printf("%-14s BoardID:           0x%04x\r\n", blade_name, board_id);  // 原打印方式不变
        }

        print_component_uid(nic_handle, blade_name, INVALID_UINT32, 17); // 按17字节左对齐

        g_printf("%-18s PCB:           %s\r\n", blade_name, pcb_ver);
    }

    g_slist_free(obj_list);
    return;
}


void print_raid_boardid_pcb_version(void)
{
    OBJ_HANDLE raid_handle = 0;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    gchar pcb_ver[PROP_LEN] = {0};
    guint16 board_id = 0;
    gchar blade_name[PROP_LEN] = {0};
    OBJ_HANDLE refer_handle = 0;
    guint32 logic_unit = 0;
    gchar logic_ver[FIRMWARE_VERSION_STRING_LEN] = {0};

    gint32 ret = dfl_get_object_list(CLASS_RAIDCARD_NAME, &obj_list);
    if (ret != DFL_OK || g_slist_length(obj_list) == 0) {
        return;
    }

    g_printf("---------------- RAID Card INFO ----------------\r\n");

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        raid_handle = (OBJ_HANDLE)obj_note->data;
        (void)dal_get_property_value_string(raid_handle, PROPERTY_RAIDCARD_NAME, blade_name, sizeof(blade_name));
        (void)dal_get_property_value_string(raid_handle, PROPERTY_RAIDCARD_PCBVER, pcb_ver, sizeof(pcb_ver));
        (void)dal_get_property_value_uint16(raid_handle, PROPERTY_RAIDCARD_BOARDID, &board_id);

        g_printf("%-14s BoardID:           0x%04x\r\n", blade_name, board_id);
        g_printf("%-18s PCB:           %s\r\n", blade_name, pcb_ver);
        
        
        ret = dfl_get_referenced_object((OBJ_HANDLE)obj_note->data, PROPERTY_PCIE_CARD_FIRMWARE_OBJ, &refer_handle);
        if (ret == DFL_OK) {
            
            (void)dal_get_property_value_uint32(refer_handle, PROPERTY_UNIT_NUM, &logic_unit);
            ret = dal_get_property_value_string(refer_handle, PROPERTY_SOFTWARE_VERSIONSTRING, logic_ver,
                FIRMWARE_VERSION_STRING_LEN);
            if ((ret == RET_OK) && (strlen(logic_ver) != 0)) {
                g_printf("%-9s CPLD Version:           (U%d)%s\r\n", blade_name, logic_unit, logic_ver);
            } else {
                g_printf("%-9s CPLD Version:           N/A\r\n", blade_name);
            }
        }
    }

    g_slist_free(obj_list);
    return;
}


void print_hddbackplane_boardid_pcb_version(void)
{
#define HDD_BACKPLANE_NAME "HddBp"
    OBJ_HANDLE hdd_handle = 0;
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    gchar pcb_ver[PROP_LEN] = {0};
    guint32 logic_unit = 0;
    gchar logic_ver[PROP_LEN] = {0};
    guint16 board_id = 0;
    gchar blade_name[PROP_LEN] = {0};
    guchar slot_id = 0;
    gchar backplane[STRING_LEN_MAX] = {0};
    OBJ_HANDLE refer_handle = 0;
    ret = dfl_get_object_list(CLASS_HDDBACKPLANE_NAME, &obj_list);
    if (ret != DFL_OK || g_slist_length(obj_list) == 0) {
        return;
    }

    g_printf("-------------- HDD Backplane INFO --------------\r\n");

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        hdd_handle = (OBJ_HANDLE)obj_note->data;
        (void)dal_get_property_value_string(hdd_handle, PROPERTY_HDDBACKPLANE_NAME, blade_name, sizeof(blade_name));
        (void)dal_get_property_value_string(hdd_handle, PROPERTY_HDDBACKPLANE_PCBVER, pcb_ver, sizeof(pcb_ver));
        (void)dal_get_property_value_uint16(hdd_handle, PROPERTY_HDDBACKPLANE_BOARDID, &board_id);
        (void)dal_get_property_value_uint32(hdd_handle, PROPERTY_HDDBACKPLANE_LOGICUNIT, &logic_unit);
        (void)dal_get_property_value_string(hdd_handle, PROPERTY_HDDBACKPLANE_LOGICVER, logic_ver, sizeof(logic_ver));
        (void)dal_get_property_value_byte(hdd_handle, PROPETRY_HDDBACKPLANE_SLOT, &slot_id);

        
        ret = dfl_get_referenced_object((OBJ_HANDLE)obj_note->data, PROPERTY_HDDBACKPLANE_REF_COMPONENT, &refer_handle);
        if ((ret != DFL_OK) ||
            (dal_get_property_value_string(refer_handle, PROPERTY_COMPONENT_DEVICE_NAME, backplane, STRING_LEN_MAX) !=
            RET_OK) ||
            (strlen(backplane) == 0)) {
            
            ret = snprintf_s(backplane, sizeof(backplane), sizeof(backplane) - 1, "%s%u", HDD_BACKPLANE_NAME, slot_id);
            if (ret <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
            }
        }

        
        
        g_printf("%-12s BoardName:           %s\r\n", backplane, blade_name);
        
        g_printf("%-14s BoardID:           0x%04x\r\n", backplane, board_id);
        print_component_uid(hdd_handle, backplane, INVALID_UINT32, 17); // CUID需要左对齐17个字节

        g_printf("%-18s PCB:           %s\r\n", backplane, pcb_ver);

        
        if ((strlen(logic_ver) != 0) && (strcmp(logic_ver, "N/A") != 0)) {
            g_printf("%-9s CPLD Version:           (U%d)%s\r\n", backplane, logic_unit, logic_ver);
        } else {
            g_printf("%-9s CPLD Version:           N/A\r\n", backplane);
        }
    }

    g_slist_free(obj_list);
    return;
}


void print_memboard_boardid_pcb_version(void)
{
    OBJ_HANDLE board_handle = 0;
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    gchar pcb_ver[PROP_LEN] = {0};
    guint32 logic_unit = 0;
    gchar logic_ver[PROP_LEN] = {0};
    guint8 locality = 0;
    guint16 board_id = 0;
    gchar blade_name[PROP_LEN] = {0};

    ret = dfl_get_object_list(CLASS_MEMBOARD_NAME, &obj_list);
    if (ret != DFL_OK || g_slist_length(obj_list) == 0) {
        return;
    }

    g_printf("-------------- Memory Board INFO --------------\r\n");

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        board_handle = (OBJ_HANDLE)obj_note->data;
        (void)dal_get_property_value_string(board_handle, PROPERTY_MEMBOARD_NAME, blade_name, sizeof(blade_name));
        (void)dal_get_property_value_string(board_handle, PROPERTY_MEMBOARD_PCBVER, pcb_ver, sizeof(pcb_ver));
        (void)dal_get_property_value_uint16(board_handle, PROPERTY_MEMBOARD_BOARDID, &board_id);
        (void)dal_get_property_value_uint32(board_handle, PROPERTY_MEMBOARD_LOGICUNIT, &logic_unit);
        (void)dal_get_property_value_string(board_handle, PROPERTY_MEMBOARD_LOGICVER, logic_ver, sizeof(logic_ver));
        (void)dal_get_property_value_byte(board_handle, PROPERTY_MEMBOARD_LOCALITY, &locality);

        
        if (locality != 0) {
            continue;
        }

        
        g_printf("%-14s BoardID:           0x%04x\r\n", blade_name, board_id);
        g_printf("%-18s PCB:           %s\r\n", blade_name, pcb_ver);

        if (strlen(logic_ver) != 0) {
            g_printf("%-9s CPLD Version:           (U%d)%s\r\n", blade_name, logic_unit, logic_ver);
        }
    }

    g_slist_free(obj_list);
    return;
}


LOCAL gint32 get_endpoint_property_info(OBJ_HANDLE handle, gchar *components_info, guint32 buff_size)
{
    errno_t safe_fun_ret;
    gint32 ret;
    guint16 vid = 0;
    guint16 did = 0;
    guint16 subvid = 0;
    guint16 subdid = 0;
    gchar description[RESOURCE_COMPONENTS_INFO_LEN] = {0};
    gchar manufacture[RESOURCE_COMPONENTS_INFO_LEN] = {0};
    gchar buf[RESOURCE_COMPONENTS_ID_INFO] = {0};

    ret = dal_get_property_value_string(handle, PROPERTY_COMPOSITION_ENDPOINT_DESCRIPTION, description,
        sizeof(description));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get PROPERTY_COMPOSITION_ENDPOINT_DESCRIPTION value failed, ret = %d\n", ret);
        return RET_ERR;
    }

    ret = dal_get_property_value_string(handle, PROPERTY_COMPOSITION_ENDPOINT_MANUFACTURE, manufacture,
        sizeof(manufacture));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get PROPERTY_COMPOSITION_ENDPOINT_MANUFACTURE value failed, ret = %d\n", ret);
        return RET_ERR;
    }

    ret = dal_get_property_value_uint16(handle, PROPERTY_COMPOSITION_ENDPOINT_VID, &vid);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get PROPERTY_COMPOSITION_ENDPOINT_VID value failed, ret = %d\n", ret);
        return RET_ERR;
    }

    ret = dal_get_property_value_uint16(handle, PROPERTY_COMPOSITION_ENDPOINT_DID, &did);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get PROPERTY_COMPOSITION_ENDPOINT_DID value failed, ret = %d\n", ret);
        return RET_ERR;
    }

    ret = dal_get_property_value_uint16(handle, PROPERTY_COMPOSITION_ENDPOINT_SUB_VID, &subvid);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get PROPERTY_COMPOSITION_ENDPOINT_SUB_VID value failed, ret = %d\n", ret);
        return RET_ERR;
    }

    ret = dal_get_property_value_uint16(handle, PROPERTY_COMPOSITION_ENDPOINT_SUB_DID, &subdid);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get PROPERTY_COMPOSITION_ENDPOINT_SUB_DID value failed, ret = %d\n", ret);
        return RET_ERR;
    }

    safe_fun_ret = strncat_s(components_info, buff_size, &manufacture[0], RESOURCE_COMPONENTS_INFO_LEN);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    if (vid) {
        (void)snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "[0x%04x]", vid);
        safe_fun_ret = strncat_s(components_info, buff_size, buf, RESOURCE_COMPONENTS_ID_INFO);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
        (void)memset_s(buf, RESOURCE_COMPONENTS_ID_INFO, 0, RESOURCE_COMPONENTS_ID_INFO);
    }
    safe_fun_ret = strncat_s(components_info, buff_size, ", ", sizeof(", "));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    safe_fun_ret = strncat_s(components_info, buff_size, &description[0], RESOURCE_COMPONENTS_INFO_LEN);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    if (did) {
        (void)snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "[0x%04x]", did);
        safe_fun_ret = strncat_s(components_info, buff_size, buf, RESOURCE_COMPONENTS_ID_INFO);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
        (void)memset_s(buf, RESOURCE_COMPONENTS_ID_INFO, 0, RESOURCE_COMPONENTS_ID_INFO);
    }
    safe_fun_ret = strncat_s(components_info, buff_size, ", ", sizeof(", "));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    
    if (vid) {
        (void)snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "[0x%04x]", subvid);
        safe_fun_ret = strncat_s(components_info, buff_size, buf, RESOURCE_COMPONENTS_ID_INFO);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
        (void)memset_s(buf, RESOURCE_COMPONENTS_ID_INFO, 0, RESOURCE_COMPONENTS_ID_INFO);
    }
    safe_fun_ret = strncat_s(components_info, buff_size, ", ", sizeof(", "));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    if (vid) {
        (void)snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "[0x%04x]", subdid);
        safe_fun_ret = strncat_s(components_info, buff_size, buf, RESOURCE_COMPONENTS_ID_INFO);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
        (void)memset_s(buf, RESOURCE_COMPONENTS_ID_INFO, 0, RESOURCE_COMPONENTS_ID_INFO);
    }
    

    safe_fun_ret = strncat_s(components_info, buff_size, "\0", sizeof("\0"));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint _ep_identifier_compare_func(gconstpointer a, gconstpointer b)
{
    OBJ_HANDLE handle_a = (OBJ_HANDLE)a;
    OBJ_HANDLE handle_b = (OBJ_HANDLE)b;
    guchar id_a = 0;
    guchar id_b = 0;
    gint32 ret;
    
    guchar ep_domain_id_a = 0;
    guchar ep_domain_id_b = 0;

    (void)dal_get_property_value_byte(handle_a, PROPERTY_NAME_EP_IDENTIFIER_DOMAIN_ID, &ep_domain_id_a);
    (void)dal_get_property_value_byte(handle_b, PROPERTY_NAME_EP_IDENTIFIER_DOMAIN_ID, &ep_domain_id_b);

    
    if (ep_domain_id_a != ep_domain_id_b) {
        return ep_domain_id_a - ep_domain_id_b;
    }

    
    
    ret = dal_get_property_value_byte(handle_a, PROPERTY_NAME_EP_IDENTIFIER_ID, &id_a);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get %s property %s value failed, return %d\n", __FUNCTION__,
            dfl_get_object_name(handle_a), PROPERTY_NAME_EP_IDENTIFIER_ID, ret);
        return 0;
    }

    ret = dal_get_property_value_byte(handle_b, PROPERTY_NAME_EP_IDENTIFIER_ID, &id_b);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get %s property %s value failed, return %d\n", __FUNCTION__,
            dfl_get_object_name(handle_a), PROPERTY_NAME_EP_IDENTIFIER_ID, ret);
        return 0;
    }

    return (id_a - id_b);
}


gint32 print_resource_components_list_info(guchar domain_id)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret;
    guint8 presence = 0;
    guint8 health_val = 0;
    guint8 container_presence = 0;
    gchar state[MAX_NAME_SIZE] = {0};
    gchar* health[] = { "OK", "OK", "Warning", "Critical", "Unknown" };
    gchar container[MAX_NAME_SIZE] = {0};
    gchar shortname[MAX_NAME_SIZE] = {0};
    gchar position_info[RESOURCE_COMPONENTS_INFO_LEN] = {0};
    gchar state_info[RESOURCE_COMPONENTS_INFO_LEN] = {0};
    gchar components_info[RESOURCE_COMPONENTS_DESCRIPTION_LEN] = {0};
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    OBJ_HANDLE ep_handle = 0;
    guchar ep_identifier_id = 0;
    
    guchar ep_domain_id = 0;
    OBJ_HANDLE ep_identifier_handle = 0;
    guchar last_domain_id = 0;
    guchar visible = 0;
    

    ret = dfl_get_object_list(CLASS_NAME_ENDPOINT_IDENTIFIER, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get CLASS_NAME_ENDPOINT_IDENTIFIER object list failed, ret = %d\n", ret);
        return RET_ERR;
    }

    
    obj_list = g_slist_sort(obj_list, _ep_identifier_compare_func);
    
    
    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        

        ep_identifier_handle = (OBJ_HANDLE)obj_note->data;

        ep_domain_id = 0;
        (void)dal_get_property_value_byte(ep_identifier_handle, PROPERTY_NAME_EP_IDENTIFIER_DOMAIN_ID, &ep_domain_id);
        
        (void)dal_get_property_value_byte(ep_identifier_handle, PROPERTY_NAME_EP_IDENTIFIER_VISIBLE, &visible);
        
        if ((domain_id != COMP_DOMAIN_ALL && domain_id != ep_domain_id) || (visible != 1)) {
            continue;
        }

        

        
        ep_identifier_id = 0;
        

        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROPERTY_NAME_EP_IDENTIFIER_ID,
            &ep_identifier_id);

        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROPERTY_NAME_EP_CONTAINER_PRESENCE,
            &container_presence);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Get PROPERTY_NAME_EP_CONTAINER_PRESENCE value failed, ret = %d\n", ret);
            continue;
        }

        ret = dal_get_property_value_string((OBJ_HANDLE)obj_note->data, PROPERTY_NAME_EP_CONTAINER, container,
            sizeof(container));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Get PROPERTY_NAME_EP_CONTAINER value failed, ret = %d\n", ret);
            continue;
        }

        ret = dal_get_property_value_string((OBJ_HANDLE)obj_note->data, PROPERTY_NAME_EP_IDENTIFIER_SHORT_NAME,
            shortname, sizeof(shortname));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Get PROPERTY_NAME_EP_IDENTIFIER_SHORT_NAME value failed, ret = %d\n", ret);
            continue;
        }

        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROPERTY_NAME_EP_IDENTIFIER_PRESENCE, &presence);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Get PROPERTY_NAME_EP_IDENTIFIER_PRESENCE value failed, ret = %d\n", ret);
            continue;
        }

        // 容器不在位，直接不显示
        if (container_presence == 0 || (ep_identifier_id == INVALID_EP_IDENTIFIER_ID)) {
            continue;
        }

        
        if (last_domain_id != ep_domain_id) {
            
            if (last_domain_id != 0) {
                g_printf("\r\n");
            }

            g_printf("Composable components for domain%d:\r\n", ep_domain_id);
            g_printf("  Position\t State|Health\t    Description\r\n");
            g_printf("------------------------------------------------------------\r\n");
            last_domain_id = ep_domain_id;
        }

        // 容器在位，EP不在位
        if (presence == 0) {
            // sdi卡不在位时不显示打印
            if (!g_strcmp0(shortname, "sdi")) {
                continue;
            }

            safe_fun_ret = strncat_s(position_info, RESOURCE_COMPONENTS_INFO_LEN, container, MAX_NAME_SIZE);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            safe_fun_ret = strncat_s(position_info, RESOURCE_COMPONENTS_INFO_LEN, "/", strlen("/") + 1);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            safe_fun_ret = strncat_s(position_info, RESOURCE_COMPONENTS_INFO_LEN, shortname, MAX_NAME_SIZE);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            safe_fun_ret = strncat_s(position_info, RESOURCE_COMPONENTS_INFO_LEN, ":", strlen(":") + 1);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            g_printf("  %-14s Absent|OK,         null\r\n", position_info);
            memset_s(position_info, RESOURCE_COMPONENTS_INFO_LEN, 0, RESOURCE_COMPONENTS_INFO_LEN);
            continue;
        }

        ret = dal_get_identifier_related_endpoint((OBJ_HANDLE)obj_note->data, &ep_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Call dal_get_identifier_related_endpoint failed, ret = %d\n", ret);
            continue;
        }

        ret = dal_get_property_value_byte(ep_handle, PROPERTY_COMPOSITION_ENDPOINT_HEALTH, &health_val);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Get PROPERTY_COMPOSITION_ENDPOINT_HEALTH value failed, ret = %d\n", ret);
            continue;
        }

        ret = get_endpoint_property_info(ep_handle, components_info, sizeof(components_info));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Call get_endpoint_property_info failed, ret = %d\n", ret);
            continue;
        }

        // health_val为4及以上的告警,均为未知状态
        if (health_val > 4) {
            health_val = 4;
        }

        if (presence == 1) {
            safe_fun_ret = memcpy_s(state, MAX_NAME_SIZE, "Enable", strlen("Enable") + 1);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
        } else {
            safe_fun_ret = memcpy_s(state, MAX_NAME_SIZE, "Absent", strlen("Absent") + 1);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
        }

        safe_fun_ret = strncat_s(position_info, RESOURCE_COMPONENTS_INFO_LEN, container, MAX_NAME_SIZE);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        safe_fun_ret = strncat_s(position_info, RESOURCE_COMPONENTS_INFO_LEN, "/", strlen("/") + 1);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        safe_fun_ret = strncat_s(position_info, RESOURCE_COMPONENTS_INFO_LEN, shortname, MAX_NAME_SIZE);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        safe_fun_ret = strncat_s(position_info, RESOURCE_COMPONENTS_INFO_LEN, ":", strlen(":") + 1);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        safe_fun_ret = strncat_s(state_info, RESOURCE_COMPONENTS_INFO_LEN, state, MAX_NAME_SIZE);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        safe_fun_ret = strncat_s(state_info, RESOURCE_COMPONENTS_INFO_LEN, "|", strlen("|") + 1);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        safe_fun_ret =
            strncat_s(state_info, RESOURCE_COMPONENTS_INFO_LEN, health[health_val], strlen(health[health_val]) + 1);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        safe_fun_ret = strncat_s(state_info, RESOURCE_COMPONENTS_INFO_LEN, ",", strlen(",") + 1);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }

        g_printf("  %-14s %-18s \"%s\"\r\n", position_info, state_info, components_info);

        memset_s(position_info, RESOURCE_COMPONENTS_INFO_LEN, 0, RESOURCE_COMPONENTS_INFO_LEN);
        memset_s(state_info, RESOURCE_COMPONENTS_INFO_LEN, 0, RESOURCE_COMPONENTS_INFO_LEN);
        memset_s(components_info, RESOURCE_COMPONENTS_DESCRIPTION_LEN, 0, RESOURCE_COMPONENTS_DESCRIPTION_LEN);

        
    }
    

    g_slist_free(obj_list);

    
    return (domain_id == COMP_DOMAIN_ALL) ? RET_OK : ((last_domain_id == domain_id) ? RET_OK : RET_ERR);
}


void print_resource_compositions_list_info(OBJ_HANDLE handle)
{
    gint32 ret;
    guint8 active_setting = 0;
    guint8 pending_setting = 0;
    OBJ_HANDLE active_obj_handle = 0;
    OBJ_HANDLE pending_obj_handle = 0;
    guchar comp_slot = 0;

    ret = dal_get_property_value_byte(handle, PROPERTY_COMPOSITION_CURRENT_SETTING, &active_setting);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get PROPERTY_COMPOSITION_SETTING_ID value failed, ret = %d\n", ret);
        return;
    }

    ret = dal_get_property_value_byte(handle, PROPERTY_COMPOSITION_PENDDING_SETTING, &pending_setting);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get PROPERTY_COMPOSITION_PENDDING_SETTING value failed, ret = %d\n", ret);
        return;
    }
    
    ret = dal_get_specific_position_object_byte(handle, CLASS_NAME_COMPOSITION_SETTING, PROPERTY_COMPOSITION_SETTING_ID,
        pending_setting, &pending_obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR,
            "Get CLASS_NAME_COMPOSITION_SETTING PROPERTY_COMPOSITION_SETTING_ID value failed, ret = %d\n", ret);
        return;
    }

    ret = dal_get_specific_position_object_byte(handle, CLASS_NAME_COMPOSITION_SETTING, PROPERTY_COMPOSITION_SETTING_ID,
        active_setting, &active_obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:cann't get object handle for class [%s] with [%s:%d]", __FUNCTION__,
            CLASS_NAME_COMPOSITION_SETTING, PROPERTY_COMPOSITION_SETTING_ID, active_setting);
        return;
    }
    
    (void)dal_get_property_value_byte(handle, PROPERTY_COMPOSITION_SLOT, &comp_slot);

    g_printf("Composition setting for domain%d: \r\n", comp_slot);
    g_printf("  current settings: %d\n", active_setting);
    dal_print_resource_compositions_list(active_obj_handle);

    if ((active_setting == pending_setting) || (pending_setting == 0xff)) {
        g_printf("  pending settings: null\n");
        return;
    }

    g_printf("  pending settings: %d\n", pending_setting);
    dal_print_resource_compositions_list(pending_obj_handle);

    return;
}


void print_ioboard_boardid_pcb_version(void)
{
#define IO_BOARD_NAME "IOBoard"
    OBJ_HANDLE board_handle = 0;
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    gchar pcb_ver[PROP_LEN] = {0};
    guint32 logic_unit = 0;
    gchar logic_ver[PROP_LEN] = {0};
    guint16 board_id = 0;
    gchar blade_name[PROP_LEN] = {0};
    guchar slot_id = 0;
    OBJ_HANDLE product = 0;
    guint32 product_id = 0;
    gchar fw_ver[PROP_LEN] = {0};
    guint32 flash_unit = 0;

    ret = dfl_get_object_list(CLASS_IOBOARD_NAME, &obj_list);
    if (ret != DFL_OK || g_slist_length(obj_list) == 0) {
        return;
    }

    g_printf("---------------- IO Board INFO ----------------\r\n");

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        board_handle = (OBJ_HANDLE)obj_note->data;
        (void)dal_get_property_value_string(board_handle, PROPERTY_IOBOARD_NAME, blade_name, sizeof(blade_name));
        (void)dal_get_property_value_string(board_handle, PROPERTY_IOBOARD_PCBVER, pcb_ver, sizeof(pcb_ver));
        (void)dal_get_property_value_uint16(board_handle, PROPERTY_IOBOARD_BOARDID, &board_id);
        (void)dal_get_property_value_uint32(board_handle, PROPERTY_IOBOARD_LOGICUNIT, &logic_unit);
        (void)dal_get_property_value_string(board_handle, PROPERTY_IOBOARD_LOGICVER, logic_ver, sizeof(logic_ver));
        (void)dal_get_property_value_byte(board_handle, PROPERTY_IOBOARD_SLOT, &slot_id);
        (void)dal_get_property_value_string(board_handle, PROPERTY_IOBOARD_FIRMWARE_VERSION, fw_ver, sizeof(fw_ver));
        (void)dal_get_property_value_uint32(board_handle, PROPERTY_IOBOARD_FLASH_UNIT, &flash_unit);

        

        (void)dal_get_object_handle_nth(CLASS_NAME_PRODUCT, 0, &product);
        (void)dal_get_property_value_uint32(product, BMC_PRODUCT_ID_NAME, &product_id);

        // 8100v5服务器的机框背板、电源背板、LED板、DVD_LCD板等也使用了IOBoard类型但未配置有效槽位号，需规避
        // 高性能服务器中的IOBoard类对象直接显示单板名来彼此区分，其他产品必须配置有效槽位号
        if (product_id == PRODUCT_ID_HPC_SERVER) {
            g_printf("%-14s BoardID:           0x%04x\r\n", blade_name, board_id);
            g_printf("%-18s PCB:           %s\r\n", blade_name, pcb_ver);

            if ((strlen(logic_ver) != 0) && (strcmp(logic_ver, "N/A") != 0)) {
                g_printf("%-9s CPLD Version:           (U%d)%s\r\n", blade_name, logic_unit, logic_ver);
            }
            if ((strlen(fw_ver) != 0) && (strcmp(fw_ver, "N/A") != 0)) {
                g_printf("%-9s FirmWare Version:       (U%d)%s\r\n", blade_name, flash_unit, fw_ver);
            }
        } else {
            g_printf("%s%-2d ProductName:           %s\r\n", IO_BOARD_NAME, slot_id, blade_name);
            g_printf("%s%-6d BoardID:           0x%04x\r\n", IO_BOARD_NAME, slot_id, board_id);
            g_printf("%s%-10d PCB:           %s\r\n", IO_BOARD_NAME, slot_id, pcb_ver);

            if ((strlen(logic_ver) != 0) && (strcmp(logic_ver, "N/A") != 0)) {
                g_printf("%s%-1d CPLD Version:           (U%d)%s\r\n", IO_BOARD_NAME, slot_id, logic_unit, logic_ver);
            }
            if ((strlen(fw_ver) != 0) && (strcmp(fw_ver, "N/A") != 0)) {
                g_printf("%s%-1d FirmWare Version:       (U%d)%s\r\n", IO_BOARD_NAME, slot_id, flash_unit, fw_ver);
            }
        }
    }

    g_slist_free(obj_list);
    return;
}


void print_ncboard_boardid_pcb_version(void)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    const gchar *pcb_ver = NULL;
    guint32 logic_unit = 0;
    const gchar *logic_ver = NULL;
    guint16 board_id = 0;
    const gchar *blade_name = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;

    ret = dfl_get_object_list(CLASS_NCBOARD_NAME, &obj_list);
    if (ret != DFL_OK || g_slist_length(obj_list) == 0) {
        return;
    }

    g_printf("---------------- NC Board INFO ----------------\r\n");

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        property_name_list = g_slist_append(property_name_list, PROPERTY_IOBOARD_NAME);
        property_name_list = g_slist_append(property_name_list, PROPERTY_IOBOARD_PCBVER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_IOBOARD_BOARDID);
        property_name_list = g_slist_append(property_name_list, PROPERTY_IOBOARD_LOGICUNIT);
        property_name_list = g_slist_append(property_name_list, PROPERTY_IOBOARD_LOGICVER);
        ret = dfl_multiget_property_value((OBJ_HANDLE)obj_note->data, property_name_list, &property_value_list);
        if (ret != DFL_OK) {
            g_slist_free(property_name_list);
            g_slist_free(obj_list);
            return;
        }

        blade_name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 0), 0);
        pcb_ver = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 1), 0);
        board_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value_list, 2));
        logic_unit = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value_list, 3));
        logic_ver = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 4), 0);

        g_printf("%-14s BoardID:           0x%04x\r\n", blade_name, board_id);
        g_printf("%-18s PCB:           %s\r\n", blade_name, pcb_ver);

        if (strlen(logic_ver) != 0) {
            g_printf("%-9s CPLD Version:           (U%d)%s\r\n", blade_name, logic_unit, logic_ver);
        }

        g_slist_free(property_name_list);
        uip_free_gvariant_list(property_value_list);

        property_name_list = NULL;
        property_value_list = NULL;
    }

    g_slist_free(obj_list);
    return;
}

void print_lcd_cpld_version(void)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;
    const gchar *logic_ver = NULL;
    guint8 present;
    guint8 as_status = 0;
    guchar MgmtSoftWareType = 0;

    ret = dal_get_software_type(&MgmtSoftWareType);
    if (ret != RET_OK) {
        return;
    }

    
    if (MgmtSoftWareType == MGMT_SOFTWARE_TYPE_EM) {
        ret = get_board_active_state(&as_status);
        if (ret != RET_OK) {
            g_printf("Get active or standby state failed.\n");
            return;
        }
        if (ACTIVE_STATE != as_status) {
            return;
        }
    }
    ret = dfl_get_object_handle(OBJECT_LCD, &obj_handle);
    if (ret != DFL_OK) {
        if (ret == ERRCODE_OBJECT_NOT_EXIST) {
            return;
        } else {
            g_printf("Get Lcd object handle failed.\r\n");
            return;
        }
    }

    property_name_list = g_slist_append(property_name_list, PROPERTY_LCD_PRESENCE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LCD_VERSION);
    ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value_list);
    g_slist_free(property_name_list);
    if (ret != DFL_OK) {
        g_printf("Get Lcd cpld version failed.\r\n");
        return;
    }

    present = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, 0));
    logic_ver = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 1), 0);
    if (present == 1 && strlen(logic_ver) != 0) {
        g_printf("------------------- LCD INFO -------------------\r\n");
        g_printf("LCD            Version:           %s\r\n", logic_ver);
    }

    uip_free_gvariant_list(property_value_list);
    return;
}


void print_ps_version(void)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar ps_version[PS_VERSION_LEN] = { 0 };
    guint32 print_flag = 0;
    guchar ps_presence = 0;
    GSList *ps_handle_list = NULL;
    GSList *ps_list_iter = NULL;
    guchar ps_slot = 0;

    if (check_print_ps_version_condition() == FALSE) {
        return;
    }

    
    ret = dfl_get_object_list(CLASS_NAME_PS, &ps_handle_list);
    if (ret != RET_OK) {
        return;
    }

    ps_handle_list = g_slist_sort(ps_handle_list, dal_compare_psu_slot_id);
    for (ps_list_iter = ps_handle_list; ps_list_iter != NULL; ps_list_iter = ps_list_iter->next) {
        obj_handle = (OBJ_HANDLE)ps_list_iter->data;
        
        (void)dal_get_property_value_byte(obj_handle, PROTERY_PS_PRESENCE, &ps_presence);
        if (ps_presence != PRESENT_STATE) {
            continue;
        }

        if (print_flag == 0) {
            g_printf("-------------------- PSU INFO -------------------\r\n");
            print_flag = 1;
        }

        if (dal_check_if_mm_board()) {
            ret = dal_get_property_value_string(obj_handle, PROTERY_PS_FWVERSION, ps_version, PS_VERSION_LEN);
        } else {
            ret = dal_get_property_value_string(obj_handle, PROTERY_PS_VERSION, ps_version, PS_VERSION_LEN);
        }
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Get power supply version failed: %d.\n", ret);
            continue;
        }

        (void)dal_get_property_value_byte(obj_handle, PROTERY_PS_SLOT_ID, &ps_slot);
        if (dal_match_product_id(PRODUCT_ID_PANGEA_V6)) {
            ps_slot -= 1;
        }
        g_printf("PSU%d           Version:           %s\r\n", ps_slot, ps_version);

        memset_s(ps_version, sizeof(ps_version), 0x00, strlen(ps_version));
    }

    g_slist_free(ps_handle_list);

    return;
}


void print_retimer_version(void)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList* retimer_list = NULL;
    GSList* retimer_iter = NULL;
    guint8 chip_type = 0xff; 

    
    ret = dal_get_object_handle_nth(CLASS_RETIMER_NAME, 0, &obj_handle);
    ret += dal_get_property_value_byte(obj_handle, PROPERTY_RETIMER_CHIP_TYPE, &chip_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get retimer chip type failed. ret = %d.",
                  dfl_get_object_name(obj_handle), ret);
        return;
    }
    if (chip_type == CDR_CHIP_TYPE_DS280) {
        return;
    }

    g_printf("-------------- RETIMER INFO --------------------\r\n");

    ret = dfl_get_object_list(CLASS_RETIMER_NAME, &retimer_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get retimer version version failed: %d.\n", ret);
        return;
    }
    for (retimer_iter = retimer_list; retimer_iter != NULL; retimer_iter = retimer_iter->next) {
        obj_handle = (OBJ_HANDLE)retimer_iter->data;
        print_retimer_location_version(obj_handle);
    }
    g_slist_free(retimer_list);
    return;
}


void print_mezz_boardid_pcb_version(void)
{
#define MEZZ_CARD_NAME "MezzCard"
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    const gchar *pcb_ver = NULL;
    guint16 board_id = 0;
    guchar slot_id = 0;
    const gchar *blade_name = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;

    ret = dfl_get_object_list(CLASS_MEZZ, &obj_list);
    if (ret != DFL_OK || g_slist_length(obj_list) == 0) {
        return;
    }

    g_printf("---------------- Mezz Card INFO ----------------\r\n");

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        
        property_name_list = g_slist_append(property_name_list, PROPERTY_MEZZCARD_PRODUCTNAME);
        property_name_list = g_slist_append(property_name_list, PROPERTY_MEZZ_PCBVER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_MEZZ_BOARDID);
        property_name_list = g_slist_append(property_name_list, PROPERTY_MEZZ_SLOT);
        ret = dfl_multiget_property_value((OBJ_HANDLE)obj_note->data, property_name_list, &property_value_list);
        if (ret != DFL_OK) {
            g_slist_free(property_name_list);
            g_slist_free(obj_list);
            return;
        }

        blade_name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 0), 0);
        pcb_ver = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 1), 0);
        board_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value_list, 2));
        slot_id = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, 3));
        if (board_id != 0) {
            
            
            if ((blade_name != NULL) && (strlen(blade_name) != 0)) {
                g_printf("%s%-2d ProductName:           %s\r\n", MEZZ_CARD_NAME, slot_id, blade_name);
            } else {
                g_printf("%s%-2d ProductName:           %s\r\n", MEZZ_CARD_NAME, slot_id, "N/A");
            }
            

            g_printf("%s%-6d BoardID:           0x%04x\r\n", MEZZ_CARD_NAME, slot_id, board_id);
            g_printf("%s%-10d PCB:           %s\r\n", MEZZ_CARD_NAME, slot_id, pcb_ver);
        }

        

        g_slist_free(property_name_list);
        uip_free_gvariant_list(property_value_list);

        property_name_list = NULL;
        property_value_list = NULL;
    }

    g_slist_free(obj_list);
    return;
}

LOCAL void print_single_card_boardid_pcb_info(GSList* property_value_list)
{
    guint8  idx;
    const gchar* pcb_ver = NULL;
    guint16 board_id = 0;
    guint8 card_type = 0;
    guint8 slot_id = 0;
    const  gchar* card_name = NULL;

    card_name = g_variant_get_string((GVariant*)g_slist_nth_data(property_value_list, 0), 0);
    pcb_ver = g_variant_get_string((GVariant*)g_slist_nth_data(property_value_list, 1), 0);
    board_id = g_variant_get_uint16((GVariant*)g_slist_nth_data(property_value_list, 2));
    card_type = g_variant_get_byte((GVariant*)g_slist_nth_data(property_value_list, 3));
    slot_id = g_variant_get_byte((GVariant*)g_slist_nth_data(property_value_list, 4));

    idx = 0;
    while (g_cardTypeInfoList[idx].typeDesc != NULL) {
        if (g_cardTypeInfoList[idx].type != card_type) {
            idx++;
            continue;
        }
        if (g_cardTypeInfoList[idx].flag == TRUE) {
            g_printf("----------------- %s ----------------\r\n", g_cardTypeInfoList[idx].title);
            g_cardTypeInfoList[idx].flag = FALSE;
        }
        if (card_type == CARD_TYPE_PIC) {
            g_printf("%s%-2d ProductName:           %s\r\n", g_cardTypeInfoList[idx].typeDesc, slot_id, card_name);
            g_printf("%s%-6d BoardID:           0x%04x\r\n", g_cardTypeInfoList[idx].typeDesc, slot_id, board_id);
            g_printf("%s%-10d PCB:           %s\r\n", g_cardTypeInfoList[idx].typeDesc, slot_id, pcb_ver);
        } else {
            g_printf("%-10s ProductName:           %s\r\n", g_cardTypeInfoList[idx].typeDesc, card_name);
            g_printf("%-14s BoardID:           0x%04x\r\n", g_cardTypeInfoList[idx].typeDesc, board_id);
            g_printf("%-18s PCB:           %s\r\n", g_cardTypeInfoList[idx].typeDesc, pcb_ver);
        }
        break;
    }
    return;
}

void print_card_boardid_pcb_version(void)
{
    gint32 ret = 0;
    GSList* obj_list = NULL;
    GSList* obj_note =  NULL;
    GSList* property_name_list = NULL;
    GSList* property_value_list = NULL;

    ret = dfl_get_object_list(CLASS_CARD_NAME, &obj_list);
    if (ret != DFL_OK || g_slist_length(obj_list) == 0) {
        return;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        property_name_list = g_slist_append(property_name_list, PROPERTY_CARD_NAME);
        property_name_list = g_slist_append(property_name_list, PROPERTY_CARD_PCBVER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_CARD_BOARDID);
        property_name_list = g_slist_append(property_name_list, PROPERTY_CARD_TYPE);
        property_name_list = g_slist_append(property_name_list, PROPERTY_CARD_SLOT);
        ret = dfl_multiget_property_value((OBJ_HANDLE)obj_note->data, property_name_list, &property_value_list);
        if (ret != DFL_OK) {
            g_slist_free(property_name_list);
            g_slist_free(obj_list);
            return;
        }
        print_single_card_boardid_pcb_info(property_value_list);

        g_slist_free(property_name_list);
        uip_free_gvariant_list(property_value_list);

        property_name_list = NULL;
        property_value_list = NULL;
    }

    g_slist_free(obj_list);
    return;
}



void print_liquid_detect_card_info(void)
{
    gint32 ret;
    OBJ_HANDLE obj_handle;
    guint8 leakdet_support;
    GSList *liqdet_obj_list = NULL;
    GSList *node = NULL;
    gchar board_name[NAME_LEN] = { 0 };
    gchar pcb_ver[PCB_VERSION_LEN] = { 0 };
    guint16 board_id = 0;
    guint8 slot_id = 0;
    guint8 print_notice = 1;

    leakdet_support = dal_get_leakdetect_support();
    if (leakdet_support == PME_SERVICE_UNSUPPORT) {
        
        return;
    }

    ret = dfl_get_object_list(CLASS_LEAK_DET_CARD, &liqdet_obj_list);
    if (ret != DFL_OK || g_slist_length(liqdet_obj_list) == 0) {
        return;
    }

    for (node = liqdet_obj_list; node != NULL; node = node->next) {
        obj_handle = (OBJ_HANDLE)node->data;

        memset_s(board_name, sizeof(board_name), 0x00, sizeof(board_name));
        memset_s(pcb_ver, sizeof(pcb_ver), 0x00, sizeof(pcb_ver));

        (void)dal_get_property_value_byte(obj_handle, PROPERTY_CARD_SLOT, &slot_id);
        (void)dal_get_property_value_string(obj_handle, PROPERTY_CARD_BOARDNAME, board_name, sizeof(board_name) - 1);
        (void)dal_get_property_value_string(obj_handle, PROPERTY_CARD_PCBVER, pcb_ver, sizeof(pcb_ver) - 1);
        (void)dal_get_property_value_uint16(obj_handle, PROPERTY_CARD_BOARDID, &board_id);

        if (print_notice != 0) {
            g_printf("---------- Leakage Detection Card INFO ---------\r\n");
        }

        g_printf("Leakage Detection BoardName:      %s\r\n", board_name);
        g_printf("Leakage Detection   BoardID:      0x%04x\r\n", board_id);
        g_printf("Leakage Detection       PCB:      %s\r\n", pcb_ver);
    }

    g_slist_free(liqdet_obj_list);

    return;
}


void print_mother_boardid_pcb_version(void)
{
    gint32 ret;
    guint8 fru_id = 0;
    gchar pcb_ver[PROP_LEN] = {0};
    guint16 board_id = 0;
    gchar blade_name[PROP_LEN] = {0};
    gchar sd_raid_ver[PROP_LEN] = {0};
    gchar sd_raid_name[PROP_LEN] = {0};
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_value = NULL;
    GSList *obj_list = NULL;
    guint32 obj_num;

    property_value = g_variant_new_byte((guint8)fru_id);
    ret = dfl_get_specific_object(CLASS_FRU, PROPERTY_FRU_ID, property_value, &obj_handle);
    g_variant_unref(property_value);
    if (ret != DFL_OK) {
        return;
    }

    g_printf("-------------- Mother Board INFO ---------------\r\n");

    
    (void)dal_get_property_value_string(obj_handle, PROPERTY_FRU_NAME, blade_name, sizeof(blade_name));
    (void)dal_get_property_value_string(obj_handle, PROPERTY_FRU_PCB_VERSION, pcb_ver, sizeof(pcb_ver));
    (void)dal_get_property_value_uint16(obj_handle, PROPERTY_FRU_BOARDID, &board_id);

    g_printf("%-14s BoardID:           0x%04x\r\n", blade_name, board_id);
    g_printf("%-18s PCB:           %s\r\n", blade_name, pcb_ver);

    ret = dfl_get_object_list(CLASS_RAID_CHIP_NAME, &obj_list);
    if (ret != DFL_OK) {
        return;
    }
    obj_num = g_slist_length(obj_list);
    if (obj_num == 0) {
        return;
    }
    obj_handle = (OBJ_HANDLE)obj_list->data;
    (void)dal_get_property_value_string(obj_handle, PROPERTY_FRU_PCB_VERSION, sd_raid_ver, sizeof(sd_raid_ver));
    (void)dal_get_property_value_string(obj_handle, PROPERTY_FRU_PCB_VERSION, sd_raid_name, sizeof(sd_raid_name));
    g_slist_free(obj_list);

    g_printf("%-18s MFR:           %s\r\n", SD_CONTROLLOR, sd_raid_name);
    g_printf("%-14s Version:           %s\r\n", SD_CONTROLLOR, sd_raid_ver);

    return;
}



void print_fanboard_boardid_pcb_version(void)
{
#define FAN_BOARD_NAME "FanBoard"
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    guint32 logic_unit = 0;
    guint16 board_id = 0;
    guchar slot_id = 0;
    OBJ_HANDLE product = 0;
    guint32 product_id = 0;
    guint8 fw_type = 0;
    gchar blade_name[NAME_LEN] = { 0 };
    gchar pcb_ver[PROP_LEN] = { 0 };
    gchar logic_ver[PROP_LEN] = { 0 };

    gint32 ret = dfl_get_object_list(CLASS_FANBOARD_NAME, &obj_list);
    if (ret != DFL_OK || g_slist_length(obj_list) == 0) {
        return;
    }

    (void)dal_get_object_handle_nth(CLASS_NAME_PRODUCT, 0, &product);
    (void)dal_get_property_value_uint32(product, BMC_PRODUCT_ID_NAME, &product_id);

    g_printf("---------------- FAN Board INFO ----------------\r\n");

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_note->data, PROPERTY_FANBOARD_NAME,
            blade_name, sizeof(blade_name));
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_note->data, PROPERTY_FANBOARD_PCBVER,
            pcb_ver, sizeof(pcb_ver));
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_note->data, PROPERTY_FANBOARD_LOGICVER,
            logic_ver, sizeof(logic_ver));
        (void)dal_get_property_value_uint16((OBJ_HANDLE)obj_note->data, PROPERTY_FANBOARD_BOARDID, &board_id);
        (void)dal_get_property_value_uint32((OBJ_HANDLE)obj_note->data, PROPERTY_FANBOARD_LOGICUNIT, &logic_unit);
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROPERTY_FANBOARD_SLOT, &slot_id);

        // X系列高密服务器的风扇背板只有一块，无需用槽位号区分，在MM630、MM810上管理和呈现
        // 由于历史原因，MM630、MM810装备TU在管理板BMC中执行，并采用ipmcget来获取和匹配BoardId等信息
        // Atlas服务器中有多块FANBoard，须显示有效槽位号来区分，后续新的FANBoard统一采用槽位号区分的规则
        if (product_id == PRODUCT_ID_XSERIAL_SERVER) {
            g_printf("%-14s BoardID:           0x%04x\r\n", blade_name, board_id);
            print_component_uid((OBJ_HANDLE)obj_note->data, blade_name, INVALID_UINT32, 17); // CUID需要左对齐17个字节

            g_printf("%-18s PCB:           %s\r\n", blade_name, pcb_ver);

            if (strlen(logic_ver) != 0) {
                g_printf("%-9s CPLD Version:           (U%d)%s\r\n", blade_name, logic_unit, logic_ver);
            }
        } else {
            g_printf("%s%-4d BoardName:           %s\r\n", FAN_BOARD_NAME, slot_id, blade_name);

            if (board_id != 0x0) {
                g_printf("%s%-6d BoardID:           0x%04x\r\n", FAN_BOARD_NAME, slot_id, board_id);
            }

            print_component_uid((OBJ_HANDLE)obj_note->data, FAN_BOARD_NAME, slot_id, 9); // 按9字节左对齐

            g_printf("%s%-10d PCB:           %s\r\n", FAN_BOARD_NAME, slot_id, pcb_ver);

            if (strlen(logic_ver) == 0) {
                continue;
            }

            (void)dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROPERTY_FANBOARD_FW_TYPE, &fw_type);
            if (fw_type == FANBOARD_FW_TYPE_CPLD) {
                g_printf("%s%-1d CPLD Version:           (U%d)%s\r\n", FAN_BOARD_NAME, slot_id, logic_unit, logic_ver);
            } else {
                g_printf("%s%-2d FW  Version:           (U%d)%s\r\n", FAN_BOARD_NAME, slot_id, logic_unit, logic_ver);
            }
        }
    }

    g_slist_free(obj_list);
    return;
}


LOCAL gint32 get_elabel_systemarea_flag(gchar fruid, guint8 *is_contain_sys_area)
{
    OBJ_HANDLE handle_fru = 0;
    OBJ_HANDLE handle_elabel = 0;
    gint32 ret;

    // 获取FRU的句柄
    ret = dal_get_specific_object_byte(CLASS_FRU, PROPERTY_FRU_ID, fruid, &handle_fru);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get fru object handle for Fru(id=%d) fail, ret=%d.", fruid, ret);
        return ret;
    }

    // 获取FRU对象Elabel属性关联的对象
    ret = dfl_get_referenced_object(handle_fru, PROPERTY_FRU_ELABEL_RO, &handle_elabel);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get %s referenced elabel handle failed, ret=%d", dfl_get_object_name(handle_fru), ret);
        return ret;
    }

    // 获取IsContainSystemArea
    ret = dal_get_property_value_byte(handle_elabel, PROPERTY_ELABEL_IS_CONTAIN_SYS_AREA, is_contain_sys_area);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get %s iscontainsystemarea failed, ret=%d", dfl_get_object_name(handle_elabel), ret);
        return ret;
    }

    return RET_OK;
}

LOCAL gint32 get_fmt_product_name(const gchar* product_name, const gchar* product_version, gchar* fmt_product_name,
    guint32 fmt_product_name_size, guint8 sys_area_fru_id)
{
    gint32 ret;
    guint8 iflag = IS_CONTAIN_SYSTEM_AREA;
    OBJ_HANDLE product = 0;

    // 获取底板的电子标签是否包含系统信息域的标志
    // 如果包含系统信息域，则直接获取system_name
    // 如果没有系统信息域，则获取product name , product version进行组合显示
    (void)get_elabel_systemarea_flag(sys_area_fru_id, &iflag);

    if (iflag == IS_CONTAIN_SYSTEM_AREA) {
        (void)dal_get_object_handle_nth(CLASS_NAME_BMC, 0, &product);
        ret = dal_get_property_value_string(product, PROPERTY_BMC_SYSTEMNAME, fmt_product_name, fmt_product_name_size);
        if (ret != RET_OK) {
            g_printf("Get system name failed.\r\n");
            return ret;
        }
    } else {
        debug_log(DLOG_ERROR, "%s: iflag = %d", __FUNCTION__, iflag);
        // product name + product  如 MM810 V1
        ret = sprintf_s(fmt_product_name, fmt_product_name_size, "%s %s", product_name, product_version);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
    }

    return RET_OK;
}


void print_product_version(void)
{
#define PRODUCT_NAME_SIZE 50
    const gchar *product_name = NULL;
    const gchar *product_version = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    gsize product_name_len = 0;
    gchar fmt_product_name[PRODUCT_NAME_SIZE] = {};
    gint32 ret;

    g_printf("----------------- Product INFO -----------------\r\n");
    
    if (dal_is_customized_machine()) {
        property_name_list = g_slist_append(property_name_list, BMC_CB_PRODUCT_ID_NAME);
    } else {
        property_name_list = g_slist_append(property_name_list, PROPERTY_PRODUCT_ID);
    }
    property_name_list = g_slist_append(property_name_list, PROPERTY_PRODUCT_NAME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_PRODUCT_VERSION);
    property_name_list = g_slist_append(property_name_list, PROPERTY_PRODUCT_SYSTEM_AREA_FRU_ID);
    ret = uip_multiget_object_property(CLASS_NAME_PRODUCT, OBJ_NAME_PRODUCT, property_name_list, &property_value);
    g_slist_free(property_name_list);
    property_name_list = NULL;
    if (ret != RET_OK) {
        g_printf("Get product version failed.\r\n");
        return;
    }

    guint32 product_id = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value, 0));
    product_name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 1), &product_name_len);
    product_version = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 2), 0);
    
    guint8 sys_area_fru_id = g_variant_get_byte((GVariant*)g_slist_nth_data(property_value, 3));

    ret = get_fmt_product_name(product_name, product_version, fmt_product_name, sizeof(fmt_product_name),
        sys_area_fru_id);
    if (ret != RET_OK) {
        uip_free_gvariant_list(property_value);
        return;
    }

    g_printf("Product             ID:           0x%04x\r\n", product_id);
    g_printf("Product           Name:           %s\r\n", fmt_product_name);
    uip_free_gvariant_list(property_value);
    
    print_bios_version();
    return;
}

LOCAL void print_rimm_card_version(GSList *obj_list)
{
    GSList *obj_note = NULL;
    const gchar *pcb_ver = NULL;
    guint16 board_id = 0;
    const gchar *blade_name = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;

    if (g_slist_length(obj_list) == 0) {
        return;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        property_name_list = g_slist_append(property_name_list, PROPERTY_CARD_NAME);
        property_name_list = g_slist_append(property_name_list, PROPERTY_CARD_PCBVER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_CARD_BOARDID);
        gint32 ret = dfl_multiget_property_value((OBJ_HANDLE)obj_note->data, property_name_list, &property_value_list);
        if (ret != DFL_OK) {
            g_slist_free(property_name_list);
            return;
        }

        blade_name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 0), 0);
        pcb_ver = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 1), 0);
        board_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value_list, 2));
        if (strlen(blade_name) != 0) {
            if (board_id != 0x0) {
                g_printf("%-14s BoardID:           0x%04x\r\n", blade_name, board_id);
            }

            if (strlen(pcb_ver) != 0) {
                g_printf("%-18s PCB:           %s\r\n", blade_name, pcb_ver);
            }
        }

        g_slist_free(property_name_list);
        uip_free_gvariant_list(property_value_list);

        property_name_list = NULL;
        property_value_list = NULL;
    }

    return;
}

LOCAL void print_rimm_firmware_version(GSList *obj_list)
{
    GSList *obj_note = NULL;
    const gchar *firmware_name = NULL;
    const gchar *version_str = NULL;
    const gchar *release_date = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;

    if (g_slist_length(obj_list) == 0) {
        return;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        property_name_list = g_slist_append(property_name_list, PROPERTY_SOFTWARE_NAME);
        property_name_list = g_slist_append(property_name_list, PROPERTY_SOFTWARE_VERSIONSTRING);
        property_name_list = g_slist_append(property_name_list, PROPERTY_SOFTWARE_RELEASEDATE);
        gint32 ret = dfl_multiget_property_value((OBJ_HANDLE)obj_note->data, property_name_list, &property_value_list);
        if (ret != DFL_OK) {
            g_slist_free(property_name_list);
            return;
        }

        firmware_name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 0), 0);
        version_str = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 1), 0);
        release_date = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 2), 0);
        if (strlen(firmware_name) != 0) {
            if (strlen(version_str) != 0) {
                g_printf("%-14s Version:           %s\r\n", firmware_name, version_str);
            }

            if (strlen(release_date) != 0) {
                g_printf("%-16s Built:           %s\r\n", firmware_name, release_date);
            }
        }

        g_slist_free(property_name_list);
        uip_free_gvariant_list(property_value_list);

        property_name_list = NULL;
        property_value_list = NULL;
    }

    return;
}

void print_rimm_version(void)
{
    gint32 firmware_ret;
    GSList *card_obj_list = NULL;
    GSList *firmware_obj_list = NULL;

    gint32 ret = dfl_get_object_list(CLASS_PERIPHERAL_CARD_NAME, &card_obj_list);
    firmware_ret = dfl_get_object_list(CLASS_PERIPHERAL_FIRMWARE_NAME, &firmware_obj_list);
    if ((firmware_ret != RET_OK || g_slist_length(firmware_obj_list) == 0) &&
        (ret != DFL_OK || g_slist_length(card_obj_list) == 0)) {
        return;
    }

    g_printf("--------------- Peripheral INFO ----------------\r\n");
    print_rimm_firmware_version(firmware_obj_list);
    print_rimm_card_version(card_obj_list);

    g_slist_free(firmware_obj_list);
    g_slist_free(card_obj_list);
}



void print_mm_backplaneBoard_version(void)
{
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    const gchar *pcb_ver = NULL;
    guint16 board_id = 0;
    const gchar *blade_name = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;
    GVariant *property_data = NULL;
    guint8 type = 0; // 部件类型
    guint8 flag = 0; // 机框背板对象是否存在标志
    guint8 fru_id = 0;
    
    gchar accessor[MAX_NAME_SIZE] = {0};
    

    gint32 ret = dfl_get_object_list(CLASS_FRU, &obj_list);
    if (ret != DFL_OK || g_slist_length(obj_list) == 0) {
        return;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        if (dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROPERTY_FRU_ID, &fru_id) != RET_OK) {
            debug_log(DLOG_ERROR, "Get property %s failed.\n", PROPERTY_FRU_ID);
            g_slist_free(obj_list);
            return;
        }

        if (fru_id == 0xff) {
            continue;
        }

        
        ret = dfl_get_property_value((OBJ_HANDLE)obj_note->data, PROPERTY_BACKPLANEBOARD_TYPE, &property_data);
        if (ret != DFL_OK) {
            g_slist_free(obj_list);
            return;
        }

        type = g_variant_get_byte(property_data);
        g_variant_unref(property_data);
        property_data = NULL;

        
        if (type != COMPONENT_TYPE_BACKPLANE_BOARD) {
            continue;
        }

        
        ret = dfl_get_property_accessor((OBJ_HANDLE)obj_note->data, PROPERTY_FRU_PCBID, accessor, MAX_NAME_SIZE);
        if (ret != DFL_OK) {
            continue;
        }
        

        if (flag == 0) {
            g_printf("-------------- Backplane INFO ------------------\r\n");
        }
        flag = 1;

        property_name_list = g_slist_append(property_name_list, PROPERTY_BACKPLANEBOARD_NAME);
        property_name_list = g_slist_append(property_name_list, PROPERTY_BACKPLANEBOARD_PCBVER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_BACKPLANEBOARD_BOARDID);
        ret = dfl_multiget_property_value((OBJ_HANDLE)obj_note->data, property_name_list, &property_value_list);
        if (ret != DFL_OK) {
            g_slist_free(property_name_list);
            g_slist_free(obj_list);
            return;
        }

        blade_name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 0), 0);
        pcb_ver = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 1), 0);
        board_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value_list, 2));
        g_printf("%-14s BoardID:           0x%04x\r\n", blade_name, board_id);
        g_printf("%-18s PCB:           %s\r\n", blade_name, pcb_ver);

        g_slist_free(property_name_list);
        uip_free_gvariant_list(property_value_list);

        property_name_list = NULL;
        property_value_list = NULL;
    }

    g_slist_free(obj_list);
    return;
}



void print_phy_fw_version(void)
{
    OBJ_HANDLE obj_handle = 0;
    guint16 FW_version = 0;

    if (dal_get_object_handle_nth(CLASS_PHY_INFO, 0, &obj_handle) != RET_OK) {
        return;
    }

    if (dal_get_property_value_uint16(obj_handle, PROPERTY_PHY_INFO_FW_VERSION, &FW_version) != RET_OK) {
        return;
    }

    g_printf("------------------- PHY INFO -------------------\r\n");
    g_printf("PHY FW      Version ID:           0x%x\r\n", FW_version);
    return;
}

void print_security_module_info(void)
{
    OBJ_HANDLE obj_handle = 0;
    gchar type[SECURITY_MODULE_MAX_STRING_LENGTH + 50] = {0};
    gchar manufacturer[SECURITY_MODULE_MAX_STRING_LENGTH + 50] = {0};
    gchar protocol_version[SECURITY_MODULE_MAX_STRING_LENGTH + 50] = {0};
    gchar firmware_version[SECURITY_MODULE_MAX_STRING_LENGTH + 50] = {0};

    // 获取对象
    if (dal_get_object_handle_nth(CLASS_SECURITY_MODULE, 0, &obj_handle) != RET_OK) {
        return;
    }

    // 读取属性
    gint32 ret = dal_get_property_value_string(obj_handle, PROPERTY_SECURITY_MODULE_SPECIFICATION_VERSION,
        protocol_version, sizeof(protocol_version));
    if (ret != RET_OK) {
        return;
    }

    ret = dal_get_property_value_string(obj_handle, PROPERTY_SECURITY_MODULE_MANUFACTURER_VERSION, firmware_version,
        sizeof(firmware_version));
    if (ret != RET_OK) {
        return;
    }

    ret = dal_get_property_value_string(obj_handle, PROPERTY_SECURITY_MODULE_SPECIFICATION_TYPE, type, sizeof(type));
    if (ret != RET_OK) {
        return;
    }

    ret = dal_get_property_value_string(obj_handle, PROPERTY_SECURITY_MODULE_MANUFACTURER_NAME, manufacturer,
        sizeof(manufacturer));
    if (ret != RET_OK) {
        return;
    }

    g_printf("------------- Security Module INFO -------------\r\n");
    g_printf("Specification     Type:           %s\r\n", type);
    g_printf("Specification  Version:           %s\r\n", protocol_version);
    g_printf("Manufacturer      Name:           %s\r\n", manufacturer);
    g_printf("Manufacturer   Version:           %s\r\n", firmware_version);
    return;
}


LOCAL void print_firmwareobj_version_info(OBJ_HANDLE obj_handle, gchar *desc, guchar slot_id)
{
    gchar firmmare_ver[FIRMWARE_VERSION_STRING_LEN] = {0};
    gchar location_info[FIRMWARE_LOCATION_INFO_LEN] = {0};
    guchar classification = 0;

    if (desc == NULL) {
        return;
    }

    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_SOFTWARE_CLASSIFICATIONS, &classification);

    
    
    if ((classification == FW_CLASSIFICATIONS_BACKPLANE_CPLD) || (classification == FW_CLASSIFICATIONS_CPLD)) {
        
        (void)dal_get_property_value_string(obj_handle, PROPERTY_SOFTWARE_LOCATION, location_info,
            FIRMWARE_LOCATION_INFO_LEN);
        (void)dal_get_property_value_string(obj_handle, PROPERTY_SOFTWARE_VERSIONSTRING, firmmare_ver,
            FIRMWARE_VERSION_STRING_LEN);

        g_printf("%s%-5d CPLD Version:           (%s)%s\r\n", desc, slot_id, location_info, firmmare_ver);
    }

    
}

void print_pcie_transform_card_version(void)
{
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    guint16 board_id = 0;
    const gchar *board_name = NULL;
    const gchar *pcb_version = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;

    gint32 ret = dfl_get_object_list(CLASS_PCIETRANSFORMCARD, &obj_list);
    if (ret != DFL_OK || g_slist_length(obj_list) == 0) {
        return;
    }

    g_printf("--------------- Pcie Adapter INFO ----------------\r\n");

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        property_name_list = g_slist_append(property_name_list, PROPERTY_CARD_BOARDNAME);
        property_name_list = g_slist_append(property_name_list, PROPERTY_CARD_BOARDID);
        property_name_list = g_slist_append(property_name_list, PROPERTY_CARD_PCBVER);
        ret = dfl_multiget_property_value((OBJ_HANDLE)obj_note->data, property_name_list, &property_value_list);
        if (ret != DFL_OK) {
            g_slist_free(property_name_list);
            g_slist_free(obj_list);
            return;
        }

        board_name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 0), 0);
        board_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value_list, 1));
        pcb_version = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 2), 0);

        g_printf("%-14s BoardID:           0x%04x\r\n", board_name, board_id);

        if ((strlen(pcb_version) != 0) && (strcmp(pcb_version, "N/A") != 0)) {
            g_printf("%-18s PCB:           %s\r\n", board_name, pcb_version);
        }

        g_slist_free(property_name_list);
        uip_free_gvariant_list(property_value_list);

        property_name_list = NULL;
        property_value_list = NULL;
    }

    g_slist_free(obj_list);
    return;
}

void print_pcie_card_version(void)
{
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    gchar card_name[PCIE_CARD_NAME_LEN] = {0};
    gchar pcb_ver[PCIE_CARD_PCB_VERSION] = {0};
    gchar slave_card_class_name[CLASSE_NAME_LEN] = {0};
    gchar accesor_name[CLASSE_NAME_LEN] = {0};
    gchar print_title_flag = NO_NEED_PRINTF_VER;
    guint8 slot_id = 0;
    guint16 board_id = 0;
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE refer_handle = 0;
    guchar bom_id = 0;
    guint8 need_print_info = FALSE;

    gint32 ret = dfl_get_object_list(CLASS_PCIE_CARD, &obj_list);
    if (ret != DFL_OK || g_slist_length(obj_list) == 0) {
        return;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        obj_handle = (OBJ_HANDLE)obj_note->data;

        
        need_print_info = FALSE;

        
        ret = dfl_get_property_accessor(obj_handle, PROPERTY_PCIE_CARD_PCB_ID_REG, accesor_name, CLASSE_NAME_LEN);
        if (ret == DFL_OK) {
            need_print_info = TRUE;
        } else {
            // 判断是否需要打印RAID标卡的CPLD版本信息
            if (dfl_get_referenced_object(obj_handle, PROPERTY_PCIE_CARD_FIRMWARE_OBJ, &refer_handle) == DFL_OK) {
                need_print_info = TRUE;
            }
        }

        

        if (need_print_info == TRUE) {
            
            if (print_title_flag == NO_NEED_PRINTF_VER) {
                g_printf("---------------- PCIe Card INFO ----------------\r\n");
                print_title_flag = NEED_PRINTF_VER;
            }

            
            (void)dal_get_silk_id(obj_handle, PROPERTY_PCIE_CARD_LOGIC_SLOT, PROPETRY_PCIECARD_SLOT, &slot_id);
            (void)dal_get_property_value_uint16(obj_handle, PROPERTY_PCIE_CARD_BOARD_ID, &board_id);
            (void)dal_get_property_value_string(obj_handle, PROPERTY_PCIE_CARD_SLAVE_CARD_NAME, card_name,
                PCIE_CARD_NAME_LEN);
            (void)dal_get_property_value_string(obj_handle, PROPERTY_PCIE_CARD_PCB_VER, pcb_ver, PCIE_CARD_PCB_VERSION);
            if (strcmp(card_name, "") == 0) {
                snprintf_s(card_name, sizeof(card_name), sizeof(card_name) - 1, "%s", "N/A");
            }

            g_printf("PCIe%-6d ProductName:           %s\r\n", slot_id, card_name);
            g_printf("PCIe%-10d BoardID:           0x%04x\r\n", slot_id, board_id);
            g_printf("PCIe%-14d PCB:           %s\r\n", slot_id, pcb_ver);

            
            ret = dfl_get_referenced_object(obj_handle, PROPERTY_PCIE_CARD_FIRMWARE_OBJ, &refer_handle);
            if (ret == DFL_OK) {
                print_firmwareobj_version_info(refer_handle, "PCIe", slot_id);
            }

            
            
            (void)dal_get_property_value_string(obj_handle, PROPERTY_PCIE_CARD_SLAVE_CARD, slave_card_class_name,
                sizeof(slave_card_class_name));

            if (strlen(slave_card_class_name) == 0) {
                continue;
            }

            ret = dal_get_specific_object_byte(slave_card_class_name, PROPERTY_CARD_SLOT, slot_id, &refer_handle);
            if (ret == RET_OK) {
                ret = dal_get_property_value_byte(refer_handle, PROPERTY_ACCELERATE_CARD_BOMID, &bom_id);
                if (ret == RET_OK) {
                    g_printf("PCIe%-12d BOMId:           0x%d\r\n", slot_id, bom_id);
                }
            }

            
        }
    }

    g_slist_free(obj_list);
    return;
}


void print_gpuboard_version(void)
{
#define NPU_BOARD_TYPE 2
#define GPU_BOARD_NAME "GPUBoard"
#define NPU_BOARD_NAME "NPUBoard"
    guint8 presence = 0;
    guint16 board_id = 0;
    guint32 logic_unit = 0;
    gchar blade_name[MAX_NAME_SIZE] = {0};
    gchar board_name[MAX_NAME_SIZE] = {0};
    gchar pcb_ver[GPU_BOARD_PCB_VERSION] = {0};
    gchar logic_ver[GPU_CPLD_LOGIC_VERSION] = {0};
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    guchar slot_id = 0;
    guint8 board_type = 0;
    gchar gpu_board_name[MAX_NAME_SIZE] = {0};

    gint32 ret = dfl_get_object_list(CLASS_GPUBOARD_NAME, &obj_list);
    if (ret != DFL_OK || g_slist_length(obj_list) == 0) {
        return;
    }

    g_printf("-------------- NPU/GPU Board INFO --------------\r\n");
    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROPERTY_GPUBOARD_PRESENCE, &presence);
        if (ret != RET_OK) {
            g_slist_free(obj_list);
            return;
        }

        if (presence == 0) {
            continue;
        }

        ret = dal_get_property_value_string((OBJ_HANDLE)obj_note->data, PROPERTY_GPUBOARD_NAME, blade_name,
            sizeof(blade_name));
        ret += dal_get_property_value_string((OBJ_HANDLE)obj_note->data, PROPERTY_GPUBOARD_BOARDNAME, board_name,
            sizeof(board_name));
        ret += dal_get_property_value_uint16((OBJ_HANDLE)obj_note->data, PROPERTY_GPUBOARD_BOARDID, &board_id);
        ret += dal_get_property_value_string((OBJ_HANDLE)obj_note->data, PROPERTY_GPUBOARD_PCBVER, pcb_ver,
            sizeof(pcb_ver));
        ret += dal_get_property_value_uint32((OBJ_HANDLE)obj_note->data, PROPERTY_GPUBOARD_LOGICUNIT, &logic_unit);
        ret += dal_get_property_value_string((OBJ_HANDLE)obj_note->data, PROPERTY_GPUBOARD_LOGICVER, logic_ver,
            sizeof(logic_ver));
        ret += dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROPERTY_GPUBOARD_SLOT, &slot_id);
        ret += dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROPERTY_GPUBOARD_TYPE, &board_type);
        if (ret != RET_OK) {
            g_slist_free(obj_list);
            return;
        }

        if (board_type == NPU_BOARD_TYPE) {
            strcpy_s(gpu_board_name, sizeof(gpu_board_name), NPU_BOARD_NAME);
        } else {
            strcpy_s(gpu_board_name, sizeof(gpu_board_name), GPU_BOARD_NAME);
        }

        
        g_printf("%s%-4d BoardName:           %s\r\n", gpu_board_name, slot_id, board_name);
        

        g_printf("%s%-6d BoardID:           0x%04x\r\n", gpu_board_name, slot_id, board_id);
        g_printf("%s%-10d PCB:           %s\r\n", gpu_board_name, slot_id, pcb_ver);

        if (strlen(logic_ver) != 0) {
            g_printf("%s%-1d CPLD Version:           (U%d)%s\r\n", gpu_board_name, slot_id, logic_unit, logic_ver);
        }
    }

    g_slist_free(obj_list);
    return;
}


void print_asset_locator_version(void)
{
#define AL_BOARD_NAME "ALBoard"
    guint16 board_id = 0;
    gchar   product_name[MAX_NAME_SIZE] = {0};
    gchar   board_name[MAX_NAME_SIZE] = {0};
    gchar   pcb_ver[MAX_NAME_SIZE] = {0};
    
    gchar   firm_ver[MAX_MCU_FW_VER_LEN] = {0};
    
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    guchar slot_id = 0;

    gint32 ret = dfl_get_object_list(CLASS_ASSET_LOCATE_BOARD, &obj_list);
    if (ret != DFL_OK || g_slist_length(obj_list) == 0) {
        return;
    }

    g_printf("----------------- AssetLocator INFO --------------\r\n");

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_note->data, PROPERTY_ASSET_LOCATE_BRD_PRODUCT, product_name,
            sizeof(product_name));
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_note->data, PROPERTY_ASSET_LOCATE_BRD_NAME, board_name,
            sizeof(board_name));
        (void)dal_get_property_value_uint16((OBJ_HANDLE)obj_note->data, PROPERTY_ASSET_LOCATE_BRD_ID, &board_id);
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_note->data, PROPERTY_ASSET_LOCATE_BRD_PCB_VER, pcb_ver,
            sizeof(pcb_ver));
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_note->data, PROPERTY_ASSET_LOCATE_BRD_FIRM_VER, firm_ver,
            sizeof(firm_ver));
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROPERTY_ASSET_LOCATE_BRD_SLOT, &slot_id);

        g_printf("%s%-2d ProductName:           %s\r\n", AL_BOARD_NAME, slot_id, product_name);
        g_printf("%s%-4d BoardName:           %s\r\n", AL_BOARD_NAME, slot_id, board_name);
        g_printf("%s%-6d BoardID:           0x%04x\r\n", AL_BOARD_NAME, slot_id, board_id);
        g_printf("%s%-10d PCB:           %s\r\n", AL_BOARD_NAME, slot_id, pcb_ver);

        if (strlen(firm_ver) != 0) {
            g_printf("%s%-2d FirmwareVer:           %s\r\n", AL_BOARD_NAME, slot_id, firm_ver);
        }
    }

    g_slist_free(obj_list);
    return;
}


void print_horizontal_connection_board_version(void)
{
    guint16 board_id = 0;
    GSList *HorizConnBrd_list = NULL;
    GSList *HorizConnBrd_Node = NULL;
    gchar board_name[MAX_NAME_SIZE] = {0};
    gchar pcb_ver[MAX_VERSION_LEN] = {0};

    gint32 ret = dfl_get_object_list(CLASS_HORIZ_CONN_BRD, &HorizConnBrd_list);
    if (ret != DFL_OK || g_slist_length(HorizConnBrd_list) == 0) {
        debug_log(DLOG_INFO, "%s: dfl_get_object_list CLASS_HORIZ_CONN_BRD failed\r\n", __FUNCTION__);
        return;
    }

    g_printf("------- Horizontal Connection Board INFO -------\r\n");

    
    for (HorizConnBrd_Node = HorizConnBrd_list; HorizConnBrd_Node; HorizConnBrd_Node = HorizConnBrd_Node->next) {
        (void)dal_get_property_value_string((OBJ_HANDLE)HorizConnBrd_Node->data, PROPERTY_HORIZ_CONN_BRD_NAME,
            board_name, sizeof(board_name));
        (void)dal_get_property_value_uint16((OBJ_HANDLE)HorizConnBrd_Node->data, PROPERTY_HORIZ_CONN_BRD_BRDID,
            &board_id);
        (void)dal_get_property_value_string((OBJ_HANDLE)HorizConnBrd_Node->data, PROPERTY_HORIZ_CONN_BRD_PCBVER,
            pcb_ver, sizeof(pcb_ver));

        g_printf("%-12s   BoardID:           0x%04x\r\n", board_name, board_id);
        g_printf("%-12s       PCB:           %s\r\n", board_name, pcb_ver);
    }

    g_slist_free(HorizConnBrd_list);
    return;
}


void print_converge_board_version(void)
{
    OBJ_HANDLE obj_handle = 0;
    guint16 board_id = 0;
    gchar cpld_ver[PROP_LEN] = {0};
    gchar board_name[MAX_NAME_SIZE] = {0};
    gchar pcb_ver[MAX_VERSION_LEN] = {0};
    guint32 cpld_unit_num = 0;

    
    if (dal_get_object_handle_nth(CLASS_CONVERGE_BOARD, 0, &obj_handle) != RET_OK) {
        return;
    }

    (void)dal_get_property_value_string(obj_handle, PROPERTY_CARD_BOARDNAME, board_name, sizeof(board_name));
    (void)dal_get_property_value_uint16(obj_handle, PROPERTY_CARD_BOARDID, &board_id);
    (void)dal_get_property_value_string(obj_handle, PROPERTY_CARD_PCBVER, pcb_ver, sizeof(pcb_ver));
    (void)dal_get_property_value_string(obj_handle, PROPERTY_CONVERGE_BOARD_CPLD_VERSION, cpld_ver, sizeof(cpld_ver));
    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_CONVERGE_BOARD_CPLD_UNIT, &cpld_unit_num);

    
    g_printf("-------------- Converge Board INFO -------------\r\n");
    g_printf("%-8s       BoardID:           0x%04x\r\n", board_name, board_id);
    g_printf("%-8s           PCB:           %s\r\n", board_name, pcb_ver);
    if (cpld_unit_num != 0) {
        g_printf("%-8s  CPLD Version:           (U%u)%s\r\n", board_name, cpld_unit_num, cpld_ver);
    }
}
