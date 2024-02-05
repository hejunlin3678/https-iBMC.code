

#include "pme_app/pme_app.h"


#include "card_manage_module.h"
#include "pcie_card.h"
#include "mezz_card.h"
#include "sdi_card.h"
#include "mem_board.h"
#include "cpu_board.h"

#include "security_module.h"

#include "hdd_backplane.h"
#include "raid_card.h"
#include "gpu_board.h"
#include "gpu_card.h"
#include "share_card.h"
#include "io_board.h"
#include "fan_board.h"
#include "leakdet_card.h"
#include "exp_board.h"
#include "modbus_device.h"
#include "asset_locator.h"
#include "pme_app/common/debug_log_macro.h"
#include "retimer_manage.h"
#include "card_upgrade.h"
#include "retimer_debug.h"
#include "exp_board.h"
#include "pme_app/uip/ipmc_public.h"
#include "dpu_card.h"
#include "soc_board.h"

BEGIN_MODULE_DECLARATION(MODULE_NAME_CARD_MANAGE)
// -- ±¾Ä£¿éÊÇ·ñÐèÒªÑÓÊ±Æô¶¯£¬ÉèÖÃÆô¶¯ÑÓÊ±Ê±¼ä(µ¥Î»:ms)
DELAY_BEFORE_START(0)

DECLARATION_CLASS(CLASS_EXTEND_NETCARD_NAME)
DECLARATION_CLASS(CLASS_PCIECARD_NAME)
DECLARATION_CLASS(CLASS_OCP_CARD)
DECLARATION_CLASS(CLASS_EXTEND_PCIECARD_NAME)
DECLARATION_CLASS(CLASS_LOCAL_PCIECARD_NAME)
DECLARATION_CLASS(CLASS_OSCAPCIECARD_NAME)
DECLARATION_CLASS(CLASS_RISERPCIECARD_NAME)
DECLARATION_CLASS(CLASS_HDDBACKPLANE_NAME)
DECLARATION_CLASS(CLASS_VIRTUAL_HDDBACKPLANE_NAME)
DECLARATION_CLASS(CLASS_RAIDCARD_NAME)
DECLARATION_CLASS(CLASS_MEZZCARD_NAME)
DECLARATION_CLASS(CLASS_CARD_NAME)
DECLARATION_CLASS(CLASS_M2TRANSFORMCARD)
DECLARATION_CLASS(CLASS_CPUBOARD_NAME)
DECLARATION_CLASS(CLASS_MEMBOARD_NAME)
DECLARATION_CLASS(CLASS_IOBOARD_NAME)
DECLARATION_CLASS(CLASS_EXPBOARD_NAME)
DECLARATION_CLASS(CLASS_HDD_BASEBOARD)
DECLARATION_CLASS(CLASS_ISMEMBOARD_NAME)
DECLARATION_CLASS(CLASS_NETCARD_NAME)
DECLARATION_CLASS(CLASS_MAINBOARD_NAME)
DECLARATION_CLASS(CLASS_PCIETRANSFORMCARD)
DECLARATION_CLASS(CLASS_MEZZCARDOPA_NAME)
DECLARATION_CLASS(CLASS_ACCLERATE_CARD)

DECLARATION_CLASS(CLASS_PASSTHROUGH_CARD)


DECLARATION_CLASS(CLASS_LEAK_DET_CARD)

DECLARATION_CLASS(CLASS_CONVERGE_BOARD)


DECLARATION_CLASS(CLASS_PCIESSDCARD_NAME)



DECLARATION_CLASS(CLASS_FANBOARD_NAME)

DECLARATION_CLASS(CLASS_CARD_ACTION)

DECLARATION_CLASS(CLASS_SECURITY_MODULE)
DECLARATION_CLASS(CLASS_DISK_SLOT_PST_INFO)


DECLARATION_CLASS(CLASS_PCIE_SDI_CARD)

DECLARATION_CLASS(CLASS_GPUBOARD_NAME)
DECLARATION_CLASS(CLASS_REG_ACTION)
DECLARATION_CLASS(CLASS_REG_CONFIG)
DECLARATION_CLASS(CLASS_NAME_SMM_PCIEINFO)
DECLARATION_CLASS(CLASS_PCIE_FPGA_CARD)

DECLARATION_CLASS(CLASS_GPU_CARD)
DECLARATION_CLASS(CLASS_NVGPU_ATCION)

DECLARATION_CLASS(CLASS_REPEATER_MGNT)
DECLARATION_CLASS(CLASS_COMPONENT_POWER_CTRL)
DECLARATION_CLASS(CLASS_RESET_CHIP_ACTION)
DECLARATION_CLASS(CLASS_STORAGE_DPU_CARD)

DECLARATION_CLASS(CLASS_BUS_MODBUS)
DECLARATION_CLASS(CLASS_MODBUS_SLAVE)
DECLARATION_CLASS(CLASS_ASSET_LOCATE_BOARD)
DECLARATION_CLASS(CLASS_MB_ADDR_ALLOC_MGMT)
DECLARATION_CLASS(CLASS_HW_MB_ADDR_ALLOC_MGMT)


DECLARATION_CLASS(CLASS_NAME_PERIPHERAL_DEVICE_SENSOR)
DECLARATION_CLASS(CLASS_RETIMER_NAME)
DECLARATION_CLASS(CLASS_OCPCONTROL_NAME)
DECLARATION_CLASS(CLASS_SOC_BOARD)
DECLARATION_CLASS(CLASS_OOB_CHAN)
DECLARATION_CLASS(CLASS_PANGEA_PCIE_CARD)
DECLARATION_CLASS(CLASS_VIRTUALI2C)


DECLARATION_CLASS(CLASS_SAS_EXPBOARD_NAME)
DECLARATION_REBOOT_PROCESS(card_manage_reboot_request_process)

// -- Ìí¼Ó¡¢É¾³ý¶ÔÏó´¦Àí

DECLARATION_CLASS(CLASS_AR_CARD_NAME)

// -- æ·»åŠ ã€�åˆ é™¤å¯¹è±¡å¤„ç�†
ON_ADD_OBJECT(card_manage_add_object_callback)

ON_ADD_OBJECT_COMPLETE(card_manage_add_complete_callback)

ON_DEL_OBJECT(card_manage_del_object_callback)

ON_DEL_OBJECT_COMPLETE(card_manage_del_complete_callback)


// -- ±¾Ä£¿édumpÐÅÏ¢µ¼³ö»Øµ÷´¦Àíº¯Êý
ON_DUMP(card_manage_dump_info)
ON_CLASS_PROPERTY_EVENT(CLASS_CHASSISPAYLOAD, PROPERTY_PAYLOAD_CHASSPWR_STATE, card_manage_payload_event_callback, NULL)
ON_CLASS_PROPERTY_EVENT(CLASS_CHASSISPAYLOAD, PROPERTY_PAYLOAD_RESET_FLAG, card_manage_payload_event_callback, NULL)
ON_CLASS_PROPERTY_EVENT(CLASS_CHASSISPAYLOAD, PROPERTY_PAYLOAD_SYS_RST_FLAG, card_manage_payload_event_callback, NULL)
ON_CLASS_PROPERTY_EVENT(CLASS_HOTSWAP, PROPERTY_HS_CRU_STATE, card_manage_updata_card_info, NULL)
// BIOS BDFÐÅÏ¢ÉÏ±¨


ON_CLASS_PROPERTY_EVENT(BIOS_CLASS_NAME, PROPERTY_BIOS_PCIECARD_BDF, pcie_card_parse_bdf_info, NULL)

ON_CLASS_PROPERTY_EVENT(CLASS_DATA_NOTIFY_BIOS, PROPERTY_BIOS_PCIECARD_BDF, pcie_card_parse_bdf_info, NULL)
ON_CLASS_PROPERTY_EVENT(BIOS_CLASS_NAME, PROPERTY_BIOS_PCIEDISK_BDF, pcie_card_parse_bdf_info, NULL)
ON_CLASS_PROPERTY_EVENT(CLASS_DATA_NOTIFY_BIOS, PROPERTY_BIOS_PCIEDISK_BDF, pcie_card_parse_bdf_info, NULL)
ON_CLASS_PROPERTY_EVENT(BIOS_CLASS_NAME, PROPERTY_BIOS_OCPCARD_BDF, pcie_card_parse_bdf_info, NULL)
ON_CLASS_PROPERTY_EVENT(BIOS_CLASS_NAME, PROPERTY_BIOS_PORT_ROOTBDF, pcie_card_parse_bdf_info, NULL)
ON_CLASS_PROPERTY_EVENT(BIOS_CLASS_NAME, PROPERTY_BIOS_PCIE_PORT_ROOTBDF, pcie_card_parse_bdf_info, NULL)


ON_CLASS_PROPERTY_EVENT(BIOS_CLASS_NAME, PROPERTY_BIOS_MENU_CHANGE_FLAG, cpubrd_ras_init_info, NULL)


ON_CLASS_PROPERTY_EVENT(CLASS_NAME_NODE_PCIEINFO_SUMMARY, PROPERTY_NODE_PCIEINFO_SUMMARY_PCIEINFO_ARRAY,
    parse_node_pcie_card_info, NULL)


ON_CLASS_PROPERTY_EVENT(CLASS_REG_ACTION, PROPERTY_REG_ACTION_CONDITION, process_reg_action_with_dynamic_condition,
    NULL)

ON_CLASS_PROPERTY_EVENT(SMBIOS_CLASS_NAME, PROPERTY_SMBIOS_STATUS_VALUE, card_manage_smbios_state_change, NULL)

ON_CLASS_PROPERTY_EVENT(CLASS_NETCARD_NAME, PROPERTY_NETCARD_HOTPLUG_CONTROL, net_card_set_default_config, NULL)


ON_CLASS_PROPERTY_EVENT(ASM_CLASS_NAME, PROPERTY_ASM_AS_STATUS, process_as_status_change, NULL)
ON_CLASS_PROPERTY_EVENT(BMC_CLASEE_NAME, BMC_DEV_GUID_NAME, write_ram_uuid_atlas800d_g1, NULL)
ON_CLASS_PROPERTY_EVENT(CLASS_ELABEL, PROPERTY_ELABEL_SYS_SERIAL_NUM, write_ram_sn_atlas800d_g1, NULL)


DECLARATION_CLASS_METHOD(CLASS_NETCARD_NAME, METHOD_SET_NETCARD_MCTP_IDENTITY, net_card_set_eid)

// -- Ö§³Öcpu mem Ä£¿éÉèÖÃ°å¼¶×´Ì¬
DECLARATION_CLASS_METHOD(CLASS_ISMEMBOARD_NAME, METHOD_ISMEMBOARD_SYNC, mem_board_sync_set_dev_status)
// -- Ö§³ÖÄÚ´æÇ¨ÒÆ
DECLARATION_CLASS_METHOD(CLASS_ISMEMBOARD_NAME, METHOD_ISMEMBOARD_MIG_ENABLE, mem_board_set_migration_enable)
DECLARATION_CLASS_METHOD(CLASS_ISMEMBOARD_NAME, METHOD_ISMEMBOARD_MIG_STATUS, mem_board_get_migration_status)
DECLARATION_CLASS_METHOD(CLASS_ISMEMBOARD_NAME, METHOD_ISMEMBOARD_MIG_REFRESH, mem_board_set_migration_refresh)
// Ö§³ÖÉèÖÃMEZZ¿¨µÄmacµØÖ·
DECLARATION_CLASS_METHOD(CLASS_MEZZCARD_NAME, METHOD_SET_MEZZ_MAC_ADDR, mezz_set_mezz_mac_addr)

// Ö§³ÖCPU ÈÈ²å°ÎÊý¾ÝÍ¬²½
DECLARATION_CLASS_METHOD(CLASS_CPUBOARD_NAME, METHOD_SET_RASEVENT, cpubrd_hot_operate_sync_event)


DECLARATION_CLASS_METHOD(CLASS_PCIECARD_NAME, METHOD_SET_FDM_FAULT_STATE, card_manage_set_fdm_fault_state)
DECLARATION_CLASS_METHOD(CLASS_OCP_CARD, METHOD_SET_FDM_FAULT_STATE, card_manage_set_fdm_fault_state)
DECLARATION_CLASS_METHOD(CLASS_MEZZCARD_NAME, METHOD_SET_FDM_FAULT_STATE, card_manage_set_fdm_fault_state)
DECLARATION_CLASS_METHOD(CLASS_NETCARD_NAME, METHOD_SET_FDM_FAULT_STATE, card_manage_set_fdm_fault_state)
DECLARATION_CLASS_METHOD(CLASS_RAIDCARD_NAME, METHOD_SET_FDM_FAULT_STATE, card_manage_set_fdm_fault_state)
DECLARATION_CLASS_METHOD(CLASS_GPUBOARD_NAME, METHOD_SET_FDM_FAULT_STATE, card_manage_set_fdm_fault_state)


DECLARATION_CLASS_METHOD(CLASS_SECURITY_MODULE, METHOD_SECURITY_MODULE_SET_SELF_TEST_RESULT,
    security_module_set_self_test_result)
DECLARATION_CLASS_METHOD(CLASS_SECURITY_MODULE, METHOD_SECURITY_MODULE_SET_SPECIFICATION_TYPE,
    security_module_set_specification_type)
DECLARATION_CLASS_METHOD(CLASS_SECURITY_MODULE, METHOD_SECURITY_MODULE_SET_MANUFACTURER_NAME,
    security_module_set_manufacturer_name)
DECLARATION_CLASS_METHOD(CLASS_SECURITY_MODULE, METHOD_SECURITY_MODULE_SET_SPECIFICATION_VERSION,
    security_module_set_specification_version)
DECLARATION_CLASS_METHOD(CLASS_SECURITY_MODULE, METHOD_SECURITY_MODULE_SET_MANUFACTURER_VERSION,
    security_module_set_manufacturer_version)



DECLARATION_CLASS_METHOD(CLASS_MAINBOARD_NAME, METHOD_MAINBOARD_SETPCHMODEL, card_manage_set_pch_model_from_bios)


DECLARATION_CLASS_METHOD(CLASS_MAINBOARD_NAME, METHOD_MAINBOARD_INIT_BOARD, init_board)

DECLARATION_CLASS_METHOD(CLASS_EXTEND_NETCARD_NAME, BUSY_NETCARD_METHOD_SET_BMA_PROP, card_manage_set_net_from_bma)

DECLARATION_CLASS_METHOD(CLASS_NETCARD_NAME, BUSY_NETCARD_METHOD_SET_BMA_PROP, card_manage_set_net_from_bma)

DECLARATION_CLASS_METHOD(CLASS_NETCARD_NAME, BUSY_NETCARD_METHOD_SET_FV_FROM_MCTP, card_manage_set_firmver_from_mctp)
DECLARATION_CLASS_METHOD(CLASS_NETCARD_NAME, BUSY_NETCARD_METHOD_SET_SERIAL_NUMBER, card_manage_set_netcard_sn)
DECLARATION_CLASS_METHOD(CLASS_NETCARD_NAME, BUSY_NETCARD_METHOD_SET_VID_DID_FROM_MCTP,
    card_manage_set_vid_did_from_mctp)


DECLARATION_CLASS_METHOD(CLASS_NETCARD_NAME, BUSY_NETCARD_METHOD_UPDATE_BWU_WAVE,
    card_manage_update_bandwidth_usage_wave)



DECLARATION_CLASS_METHOD(CLASS_NETCARD_NAME, BUSY_NETCARD_METHOD_CLEAR_BWU_WAVE,
    method_netcard_bandwidth_usage_wave_record_clear)


DECLARATION_CLASS_METHOD(CLASS_NETCARD_NAME, BUSY_NETCARD_METHOD_ROLLBACK_BWU, netcard_bandwidth_usage_rollback)

DECLARATION_CLASS_METHOD(CLASS_FANBOARD_NAME, METHOD_OPERATION_FANBOARD_LED, method_card_manage_set_fanboard_led)

DECLARATION_CLASS_METHOD(CLASS_FANBOARD_NAME, METHOD_SYNC_FANBOARD_INFO, method_sync_fanboard_info)


DECLARATION_CLASS_METHOD(CLASS_FANBOARD_NAME, METHOD_RELOAD_FAN_INFO, card_manage_reload_fan_info)



DECLARATION_CLASS_METHOD(CLASS_PCIE_SDI_CARD, METHOD_GET_SDI_CARD_SET_IP_STATUS, get_os_set_ip_status)
DECLARATION_CLASS_METHOD(CLASS_PCIE_SDI_CARD, METHOD_GET_SDI_FSA_IP_STATUS, get_fsa_ip_status)


DECLARATION_CLASS_METHOD(CLASS_PCIE_SDI_CARD, METHOD_SET_SDI_PCIE_TEST_REG, sdi_set_pcie_test_reg)

DECLARATION_CLASS_METHOD(CLASS_PCIE_SDI_CARD, METHOD_SET_SDI_CARD_EID, sdi_card_set_eid)
DECLARATION_CLASS_METHOD(CLASS_PCIE_SDI_CARD, METHOD_SET_SDI_CARD_NMI, method_set_sdicard_nmi)
DECLARATION_CLASS_METHOD(CLASS_PCIE_SDI_CARD, METHOD_SET_SDI_BOOT_OPTION, method_sdi_card_set_boot_order)


DECLARATION_CLASS_METHOD(CLASS_HDDBACKPLANE_NAME, METHOD_HDDBACKPLANE_SET_HDD_MAX_TEMP, hddbackplane_set_hdd_temp)
DECLARATION_CLASS_METHOD(CLASS_VIRTUAL_HDDBACKPLANE_NAME, METHOD_HDDBACKPLANE_SET_HDD_MAX_TEMP,
    hddbackplane_set_hdd_temp)


DECLARATION_CLASS_METHOD(CLASS_PCIECARD_NAME, METHOD_SET_PFA_EVENT, card_manage_set_pfa_event)
DECLARATION_CLASS_METHOD(CLASS_OCP_CARD, METHOD_SET_PFA_EVENT, card_manage_set_pfa_event)
DECLARATION_CLASS_METHOD(CLASS_MEZZCARD_NAME, METHOD_SET_PFA_EVENT, card_manage_set_pfa_event)
DECLARATION_CLASS_METHOD(CLASS_NETCARD_NAME, METHOD_SET_PFA_EVENT, card_manage_set_pfa_event)
DECLARATION_CLASS_METHOD(CLASS_RAIDCARD_NAME, METHOD_SET_PFA_EVENT, card_manage_set_pfa_event)

DECLARATION_CLASS_METHOD(CLASS_GPUBOARD_NAME, METHOD_SET_PFA_EVENT, card_manage_set_pfa_event)
DECLARATION_CLASS_METHOD(CLASS_REG_ACTION, METHOD_START_REG_ACTION, card_manage_start_reg_action)
DECLARATION_CLASS_METHOD(CLASS_REG_ACTION, METHOD_CHECK_REG_ACTION, card_manage_check_reg_action)
DECLARATION_CLASS_METHOD(CLASS_PCIECARD_NAME, METHOD_SET_PCIE_LINK_ABILITY, card_manage_set_pcie_link_ability)
DECLARATION_CLASS_METHOD(CLASS_PCIECARD_NAME, METHOD_SET_PCIE_LINK_INFO, card_manage_set_pcie_link_info)
DECLARATION_CLASS_METHOD(CLASS_PCIECARD_NAME, METHOD_SET_PCIE_CHIP_TEMP, card_manage_set_pcie_chip_temp)
DECLARATION_CLASS_METHOD(CLASS_PCIECARD_NAME, METHOD_SET_PCIE_ERROR_INFO, card_manage_set_pcie_error_info)


DECLARATION_CLASS_METHOD(CLASS_PCIECARD_NAME, METHOD_SET_PCIE_CARD_DESC, card_manage_set_pcie_desc)



DECLARATION_CLASS_METHOD(CLASS_MEZZCARD_NAME, METHOD_SET_PCIE_LINK_ABILITY, card_manage_set_pcie_link_ability)
DECLARATION_CLASS_METHOD(CLASS_MEZZCARD_NAME, METHOD_SET_PCIE_LINK_INFO, card_manage_set_pcie_link_info)
DECLARATION_CLASS_METHOD(CLASS_MEZZCARD_NAME, METHOD_SET_PCIE_CHIP_TEMP, card_manage_set_pcie_chip_temp)
DECLARATION_CLASS_METHOD(CLASS_MEZZCARD_NAME, METHOD_SET_PCIE_ERROR_INFO, card_manage_set_pcie_error_info)


DECLARATION_CLASS_METHOD(CLASS_OCP_CARD, METHOD_SET_PCIE_LINK_ABILITY, card_manage_set_pcie_link_ability)
DECLARATION_CLASS_METHOD(CLASS_OCP_CARD, METHOD_SET_PCIE_LINK_INFO, card_manage_set_pcie_link_info)
DECLARATION_CLASS_METHOD(CLASS_OCP_CARD, METHOD_SET_PCIE_CHIP_TEMP, card_manage_set_pcie_chip_temp)
DECLARATION_CLASS_METHOD(CLASS_OCP_CARD, METHOD_SET_PCIE_ERROR_INFO, card_manage_set_pcie_error_info)


DECLARATION_CLASS_METHOD(CLASS_PCIECARD_NAME, METHOD_SET_AGENT_PROPERTY, dal_set_agent_object_property_method)
DECLARATION_CLASS_METHOD(CLASS_PCIECARD_NAME, METHOD_PCIE_CARD_GET_DATA_FROM_MCU, card_manage_mcu_get_data)
DECLARATION_CLASS_METHOD(CLASS_PCIECARD_NAME, METHOD_PCIE_CARD_SET_DATA_TO_MCU, card_manage_mcu_set_data)


DECLARATION_CLASS_METHOD(CLASS_SAS_EXPBOARD_NAME, METHOD_SAS_EXPBOARD_SYNC, method_sas_expboard_property_sync)
DECLARATION_CLASS_METHOD(CLASS_NETCARD_NAME, METHOD_SET_LLDP_ENABLE, card_manage_set_lldp_enable)
DECLARATION_CLASS_METHOD(CLASS_NETCARD_NAME, METHOD_NETCARD_SET_LLDPOVERNCSI_ENABLE,
    card_manage_set_lldp_over_ncsi_enable)
DECLARATION_CLASS_METHOD(CLASS_NETCARD_NAME, METHOD_NETCARD_SET_CHIP_TEMP, card_manage_set_netcard_chip_temp)
DECLARATION_CLASS_METHOD(CLASS_SAS_EXPBOARD_NAME, METHOD_SAS_EXPBOARD_PCIE_FAULT, set_sas_expboard_pcie_fault)
DECLARATION_CLASS_METHOD(CLASS_NETCARD_NAME, METHOD_NETCARD_SET_MANU_ID, card_manage_set_netcard_manu_id)

DECLARATION_CLASS_METHOD(CLASS_MAINBOARD_NAME, METHOD_MAINBOARD_CHECK_THIRD_PCIE_RAID_CARD, check_third_pcie_raid_card)
DECLARATION_CLASS_METHOD(CLASS_BUS_MODBUS, METHOD_MODBUS_PWR_OUT_SWITCH, method_modbus_pwr_out_switch)
DECLARATION_CLASS_METHOD(CLASS_BUS_MODBUS, METHOD_MODBUS_READ_INPUT_REG, method_modbus_read_input_reg)
DECLARATION_CLASS_METHOD(CLASS_BUS_MODBUS, METHOD_MODBUS_SEND_CMD, method_modbus_send_cmd)
DECLARATION_CLASS_METHOD(CLASS_MAINBOARD_NAME, METHOD_MAINBOARD_RESTORE_FPGA_CARD, card_manage_restore_fpga_card)
DECLARATION_CLASS_METHOD(CLASS_MAINBOARD_NAME, METHOD_MAINBOARD_GET_FPGA_RESTORE_STATUS,
    card_manage_restore_fpga_card_percent)
DECLARATION_CLASS_METHOD(CLASS_IOBOARD_NAME, METHOD_IOBOARD_SET_FIRMWARE_VERSION, io_board_set_firmware_version)

DECLARATION_CLASS_METHOD(CLASS_EXTEND_PCIECARD_NAME, EXTEND_PCIECARD_METHOD_GET_PFINFO, method_get_pfinfo)


DECLARATION_CLASS_METHOD(CLASS_RETIMER_NAME, METHOD_RETIMER_NOTIFY_UPGING, retimer_notify_upging)
DECLARATION_CLASS_METHOD(CLASS_RETIMER_NAME, METHOD_RETIMER_CHANNEL_SWITCH, retimer_channel_switch)
DECLARATION_CLASS_METHOD(CLASS_RETIMER_NAME, METHOD_RETIMER_SET_UPG_STATUS, retimer_set_upg_status)
DECLARATION_CLASS_METHOD(CLASS_RETIMER_NAME, METHOD_RETIMER_SET_NPU_CDR_TEMP, retimer_set_npu_cdr_temp)
DECLARATION_CLASS_METHOD(CLASS_RETIMER_NAME, METHOD_RETIMER_SET_NPU_CDR_VER, retimer_set_npu_cdr_ver)
DECLARATION_CLASS_METHOD(CLASS_RETIMER_NAME, METHOD_RETIMER_SET_UPG_RESULT, retimer_set_upg_result)

DECLARATION_CLASS_METHOD(CLASS_RETIMER_NAME, METHOD_RETIMER_SYNC_INFO, retimer_method_sync_info)
DECLARATION_CLASS_METHOD(CLASS_PANGEA_PCIE_CARD, METHOD_GET_MCU_ELABLE, pangea_card_get_elable)
DECLARATION_CLASS_METHOD(CLASS_PANGEA_PCIE_CARD, METHOD_UPDATE_MCU_VER, pangea_card_update_version)
DECLARATION_CLASS_METHOD(CLASS_PANGEA_PCIE_CARD, METHOD_SET_ALARM_LED_STATE, pangea_card_set_alarm_led)
DECLARATION_CLASS_METHOD(CLASS_PANGEA_PCIE_CARD, METHOD_GET_ALARM_LED_STATE, pangea_card_get_alarm_led)
DECLARATION_CLASS_METHOD(CLASS_VIRTUALI2C, METHOD_SET_TIME_OUT_VALUE, set_card_time_out_val)
DECLARATION_CLASS_METHOD(CLASS_VIRTUALI2C, METHOD_RESET_CPLD_CORE, reset_cpld_core)
DECLARATION_CLASS_METHOD(CLASS_PANGEA_PCIE_CARD, METHOD_READ_DATA_FROM_MCU, method_read_data_from_mcu)

DECLARATION_CLASS_METHOD(CLASS_LEAK_DET_CARD, METHOD_LEAK_CTRL_MAGVALVE_STATE, ctrl_magvalve_state)
DECLARATION_CLASS_METHOD(CLASS_LEAK_DET_CARD, METHOD_LEAK_TEST_ALARM, test_leak_detect_alarm)
DECLARATION_CLASS_METHOD(CLASS_NETCARD_NAME, METHOD_SET_NETCARD_PRE_PLUG_STATE, net_card_set_pre_plug)
DECLARATION_CLASS_METHOD(CLASS_NETCARD_NAME, NETCARD_METHOD_SET_NCSI_PACKAGE_ID, net_card_set_ncsi_package_id)
DECLARATION_CLASS_METHOD(CLASS_NETCARD_NAME, METHOD_SET_AGENT_PROPERTY, dal_set_agent_object_property_method)
DECLARATION_CLASS_METHOD(CLASS_NETCARD_NAME, METHOD_NETCARD_SET_ROOTBDF, net_card_set_rootbdf)
DECLARATION_CLASS_METHOD(CLASS_GPU_CARD, METHOD_SET_GPU_SUMMARY_FROM_BMA, set_gpucard_info_from_bma)
DECLARATION_CLASS_METHOD(CLASS_MAINBOARD_NAME, METHOD_CARD_XML_UPGRADE, upgrade_card_xml)
DECLARATION_CLASS_METHOD(CLASS_HDD_BASEBOARD, METHOD_PROPERTY_SYNC, (CLASS_METHOD_FUNC)hdd_board_method_sync_property)
DECLARATION_CLASS_METHOD(CLASS_SOC_BOARD, METHOD_SOC_BOARD_GET_DATA_FROM_MCU, soc_board_mcu_get_data)
DECLARATION_CLASS_METHOD(CLASS_SOC_BOARD, METHOD_SOC_BOARD_SET_DATA_TO_MCU, soc_board_mcu_set_data)
DECLARATION_CLASS_METHOD(CLASS_SOC_BOARD, METHOD_SOC_BOARD_SET_HOST_FAULT_INFO, soc_board_set_host_fault_info)
DECLARATION_CLASS_METHOD(CLASS_SOC_BOARD, METHOD_SOC_BOARD_UPLODE_BOOT_CERT, upload_boot_cert_string_to_mcu)
DECLARATION_CLASS_METHOD(CLASS_SOC_BOARD, METHOD_SOC_BOARD_GET_BOOT_CERT_STATUS, get_boot_cert_status_from_mcu)
DECLARATION_CLASS_METHOD(CLASS_SOC_BOARD, METHOD_SOC_BOARD_SET_SECUREBOOT, soc_board_set_secureboot_to_mcu)
DECLARATION_CLASS_METHOD(CLASS_SOC_BOARD, METHOD_MCU_GET_SECUREBOOT, soc_board_mcu_get_secureboot)


DECLARATION_CLASS_METHOD(CLASS_AR_CARD_NAME, METHOD_ARCARD_HW_PROPERTY_SYNC, property_sync_hw_common_handler)
DECLARATION_CLASS_METHOD(CLASS_AR_CARD_NAME, METHOD_ARCARD_PROPERTY_SYNC, property_sync_common_handler)
DECLARATION_CLASS_METHOD(CLASS_AR_CARD_NAME, METHOD_ARCARD_CONTROL, method_arcard_control)

DECLARATION_CLASS_METHOD(CLASS_STORAGE_DPU_CARD, METHOD_GET_DPU_IP_ADDR, method_get_dpucard_ipaddr)
DECLARATION_CLASS_METHOD(CLASS_STORAGE_DPU_CARD, METHOD_SET_DPU_CARD_EID, method_dpu_card_set_eid)

END_MODULE_DECLARATION()

// -- Ö§³ÖµÄIPMIÃüÁîÉùÃ÷: netfn, cmd, filter(e.g. "data[0]=1,data[2]=3"), privilege, maxlen, minlen, callback function
BEGIN_IPMI_COMMAND()


ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x93, "data[3]=53h", RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_FORBID,
    PROPERTY_USERROLE_BASICSETTING, 11, 11, card_manage_ipmi_mcu_set_device_action, NULL)
ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x93, "data[3]=54h", RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_ALLOW,
    PROPERTY_USERROLE_READONLY, 11, 11, card_manage_ipmi_mcu_get_device_action_result, NULL)

ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x90, "data[0]=59h,data[1]=13h",
    RMCP_USER | IPMI_SYS_LOCKDOWN_ALLOW, PROPERTY_USERROLE_READONLY,
    0x04, 0x04, ipmi_retimer_get_upgrade_err_slot, NULL)
ADD_IPMI_COMMAND_V2(NETFN_APP_REQ, 0x58, "data[0]=c0h", RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_FORBID,
    PROPERTY_USERROLE_BASICSETTING, 0xff, 0x02, pcie_card_ipmi_set_vender_device_id, NULL)
ADD_IPMI_COMMAND_V2(NETFN_APP_REQ, 0x58, "data[0]=c7h", RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_FORBID,
    PROPERTY_USERROLE_BASICSETTING, 0xff, 0x02, pcie_disk_ipmi_set_vender_device_id, NULL)

ADD_IPMI_COMMAND_V2(NETFN_APP_REQ, 0x58, "data[0]=cdh,data[9]=00h", RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_ALLOW,
    PROPERTY_USERROLE_BASICSETTING, 0xff, 0x0b, pcie_card_ipmi_set_chip_temp, NULL)
ADD_IPMI_COMMAND_V2(NETFN_APP_REQ, 0x58, "data[0]=cdh,data[9]=01h", RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_ALLOW,
    PROPERTY_USERROLE_BASICSETTING, 0xff, 0x0b, pcie_card_ipmi_set_chip_health_status, NULL)
ADD_IPMI_COMMAND_V2(NETFN_APP_REQ, 0x58, "data[0]=ceh", RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_FORBID,
    PROPERTY_USERROLE_BASICSETTING, 0xff, 0x02, pciecard_parse_vid_did_subvid_subdid, NULL)
ADD_IPMI_COMMAND_V2(NETFN_APP_REQ, 0x58, "data[0]=cfh", RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_FORBID,
    PROPERTY_USERROLE_BASICSETTING, 0xff, 0x02, ocp_card_ipmi_set_vid_did_subvid_subdid_info, NULL)
ADD_IPMI_COMMAND_V2(NETFN_APP_REQ, 0x59, "data[1]=c3h", RMCP_USER | IPMI_SYS_LOCKDOWN_ALLOW, PROPERTY_USERROLE_READONLY,
    0xff, 0x02, mezz_card_ipmi_get_pcie_width_info, NULL)
ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x92, "data[3]=05h,data[4]=02h", RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_ALLOW,
    PROPERTY_USERROLE_BASICSETTING, 0xFF, 0x0A, pcie_card_ipmi_set_device_status, NULL)
ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x92, "data[3]=05h,data[4]=06h", RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_ALLOW,
    PROPERTY_USERROLE_BASICSETTING, 0xFF, 0x0A, mem_board_ipmi_set_device_status, NULL)
ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x92, "data[3]=05h,data[4]=0ah", RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_ALLOW,
    PROPERTY_USERROLE_BASICSETTING, 0xFF, 0x0A, cpu_board_ipmi_set_device_status, NULL)
ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x92, "data[3]=05h,data[4]=0dh", RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_ALLOW,
    PROPERTY_USERROLE_BASICSETTING, 12, 8, mezz_card_ipmi_set_health_status, NULL)
ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x92, "data[3]=20h", RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_ALLOW,
    PROPERTY_USERROLE_BASICSETTING, 0xFF, 0x04, ipmi_get_ras_mission_status, NULL)

ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x92, "data[3]=05h,data[4]=0eh", RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_ALLOW,
    PROPERTY_USERROLE_BASICSETTING, 0xFF, 0x0B, ipmi_set_raid_fault_status, NULL)


ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x93, "data[3]=3Ah", RMCP_USER | IPMI_SYS_LOCKDOWN_ALLOW, PROPERTY_USERROLE_READONLY,
    0xff, 0x05, ipmi_command_get_hdd_plug_cnt, NULL)


ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x93, "data[3]=3Ch,data[5]=15h", RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_FORBID,
    PROPERTY_USERROLE_BASICSETTING, 14, 14, ipmi_cmd_component_power_ctrl, NULL)


ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x93, "data[3]=3Ch,data[4]=39h,data[5]=17h",
    RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_FORBID, PROPERTY_USERROLE_BASICSETTING, 15, 14, ipmi_cmd_set_sdi_boot_order, NULL)


ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x93, "data[3]=3Ch,data[4]=39h,data[5]=19h",
    RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_FORBID, PROPERTY_USERROLE_BASICSETTING, 23, 23, ipmi_cmd_set_sdi_ip, NULL)



ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x94, "data[3]=01h,data[5]=12h", RMCP_USER | IPMI_SYS_LOCKDOWN_ALLOW,
    PROPERTY_USERROLE_READONLY, 0x08, 0x08, ipmi_get_net_card_chip_info, NULL)



ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x93, "data[3]=3Ch,data[4]=39h,data[5]=1Ah",
    RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_FORBID, PROPERTY_USERROLE_BASICSETTING, 14, 14, ipmi_cmd_reset_sdi, NULL)



ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x93, "data[3]=3Ch,data[4]=39h,data[5]=1Bh",
    RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_FORBID, PROPERTY_USERROLE_BASICSETTING, 14, 14, ipmi_cmd_reset_sdi_imu, NULL)

ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x93, "data[3]=3Ch,data[4]=39h,data[5]=1Ch",
    RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_FORBID, PROPERTY_USERROLE_BASICSETTING, 13, 13, ipmi_cmd_set_sdicard_nmi, NULL)


ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x93, "data[3]=3Ch,data[4]=39h,data[5]=27h",
    RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_FORBID, PROPERTY_USERROLE_BASICSETTING, 14, 14,
    ipmi_cmd_set_sdicard_reset_linkage, NULL)
    
ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x92, "data[3]=4bh", RMCP_USER | IPMI_SYS_LOCKDOWN_FORBID,
    PROPERTY_USERROLE_READONLY, 0x5, 0x05, ipmi_get_pcie_dev_present_status, NULL)

ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x93, "data[3]=5ah,data[4]=09h", RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_FORBID,
    PROPERTY_USERROLE_BASICSETTING, 0xff, 0x0, ipmi_cmd_set_sas_expboard_led, NULL)


ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x93, "data[3]=5ah,data[4]=08h", RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_FORBID,
    PROPERTY_USERROLE_BASICSETTING, 0xff, 0x0, ipmi_cmd_set_sas_expboard_powerstate, NULL)

ADD_IPMI_COMMAND_V2(NETFN_PANGEA_OEM, 0xbc, "data[1]=01h,data[2]=00h,data[4]=05h",
    RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_FORBID, PROPERTY_USERROLE_BASICSETTING, 0xe5, 0x01,
    ipmi_cmd_set_pangea_card_elabel, NULL)

ADD_IPMI_COMMAND_V2(NETFN_PANGEA_OEM, 0xbb, "data[1]=01h,data[2]=00h,data[4]=0dh",
    RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_FORBID, PROPERTY_USERROLE_BASICSETTING, 0x09, 0x09,
    ipmi_cmd_get_pangea_card_elabel, NULL)
#ifndef BOARD_WITH_EBMC

ADD_IPMI_COMMAND_V2(NETFN_PANGEA_OEM, 0xbc, "data[1]=01h,data[2]=00h,data[4]=01h",
    RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_FORBID, PROPERTY_USERROLE_BASICSETTING,
    0x5, 0x5, ipmi_cmd_get_pangea_card_type, NULL)

ADD_IPMI_COMMAND_V2(NETFN_PANGEA_OEM, 0xbb, "data[1]=01h,data[2]=00h,data[4]=01h",
    RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_FORBID, PROPERTY_USERROLE_BASICSETTING,
    0x07, 0x07, ipmi_cmd_set_pangea_card_type, NULL)
#endif

ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, IPMI_OEM_INTERAL_COMMAND, "data[3]=5bh,data[4]=39h,data[5]=00h",
    RMCP_USER | IPMI_SYS_LOCKDOWN_ALLOW, PROPERTY_USERROLE_READONLY, 10, 10, ipmi_get_dpucard_ipaddr, NULL)

ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, IPMI_OEM_INTERAL_COMMAND, "data[3]=5bh,data[4]=3Bh,data[5]=00h",
    RMCP_USER | IPMI_SYS_LOCKDOWN_ALLOW, PROPERTY_USERROLE_READONLY, 9, 9, ipmi_get_dpucard_port_status, NULL)

ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x92, "data[3]=53h", RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_ALLOW,
    PROPERTY_USERROLE_BASICSETTING, 36, 6, cpu_board_set_bios_fw_ver, NULL)
ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x92, "data[3]=54h", RMCP_OPERATOR | IPMI_SYS_LOCKDOWN_ALLOW,
    PROPERTY_USERROLE_READONLY, 6, 6, cpu_board_get_bios_fw_ver, NULL)
END_IPMI_COMMAND()

// -- Ö§³ÖµÄdebugµ÷ÊÔÃüÁîÉùÃ÷
BEGIN_DEUBG_COMMAND()

ADD_DEBUG_COMMAND("sdi_bios_print", sdi_bios_print, "y", "Usage: sdi_bios_print\nOpen or Close sdi bios print\n")


ADD_DEBUG_COMMAND("pcie_device_mock", pcie_device_mock, "suuu",
    "Usage: pcie_device_mock [connector_name] [pcie_type(1 pcie card; 2 ssd)] [vid_did] [aux_id]\nMock PCIE device "
    "load/unload\n")

ADD_DEBUG_COMMAND("read_viddid_from_me", debug_read_viddid_from_me, "yyyy",
    "Usage: read_viddid_from_me [cpu] [bus] [device] [function] \nRead PcieCard Vid/Did Frem ME\n")


ADD_DEBUG_COMMAND("mcu_send_file", upgrade_mcu_send_file, "ys",
    "Usage: mcu_send_file [slotid] [upgradefile] \nSend file to mcu\n")

ADD_DEBUG_COMMAND("reset_chip", reset_card_chip, "y", "Usage: reset_chip [slotid] \nReset PcieCard Chip\n")
ADD_DEBUG_COMMAND("get_card_pg", get_card_powergood, "y", "Usage: get_card_pg [slotid] \nGet PcieCard PowerGood\n")
ADD_DEBUG_COMMAND("set_card_hilink", set_card_hilink, "yy", "Usage: set_card_hilink [slotid] \nSet Card Hilink\n")
ADD_DEBUG_COMMAND("set_card_type", set_card_type, "yyy", "Usage: set_card_type [slotid] [type] \nSet Card type\n")
ADD_DEBUG_COMMAND("set_card_workmode", set_card_workmode, "yyy",
    "Usage: set_card_workmode [slotid] [mode] [speed] \nSet Card Hilink\n")




ADD_DEBUG_COMMAND("collect_mcu_log", collect_mcu_log, "uy",
    "Usage: collect_mcu_log [position] [slot] \nCollect mcu log\n")

ADD_DEBUG_COMMAND("mb_set_debug_state", mb_set_debug_state, "yy",
    "Usage: mb_set_debug_state [channel_num] [debug_state] \nSet modbus channel debug state\n")


ADD_DEBUG_COMMAND("upgrade_al_board", upgrade_al_board, "ys",
    "Usage: upgrade_al_board [al_board_number] [upgrade_file_path]\n"
    "Upgrade specified asset locator board.\n")


ADD_DEBUG_COMMAND("sdi_reg_read", method_sdi_card_read_reg, "yuu",
    "Usage: sdi_reg_read [logic num] [cpld addr] [offset]\nRead SDI CPLD register\n")
ADD_DEBUG_COMMAND("sdi_reg_write", method_sdi_card_write_reg, "yuuy",
    "Usage: sdi_reg_write [logic num] [cpld addr] [offset] [value]\nWrite SDI CPLD register\n")

ADD_DEBUG_COMMAND("retimer_reg_read_write", method_retimer_reg_read_write, "suuu",
    "Usage: retimer_reg_read_write [retimer name] [opration:0 read, 1 write] [offset] [value]\n\
        Read or write retimer register\n")


ADD_DEBUG_COMMAND("setrxctle", retimer_set_rx_ctle_proc, "yyquuqq",
    "Usage: setrxctle [retimer id] [channel id] [enable] [gain] [boost] [squelch] [zero]\
    \nSet rx ctle info to retimer.\n")
ADD_DEBUG_COMMAND("settxffe", retimer_set_tx_ffe_proc, "yynnqnn",
    "Usage: settxffe [retimer id] [channel id] [pre2] [pre1] [main] [post1] [post2]\
    \nSet tx ffe info to retimer.\n")
ADD_DEBUG_COMMAND("setreg", retimer_set_reg_info_proc, "yuu",
    "Usage: setreg [retimer id] [reg addr] [reg data]\nSet reg info to retimer.\n")
ADD_DEBUG_COMMAND("getrxctle", retimer_get_rx_ctle_proc, "yy",
    "Usage: getrxctle [retimer id] [channel id]\nRead rx ctle from retimer.\n")
ADD_DEBUG_COMMAND("gettxffe", retimer_get_tx_ffe_proc, "yy",
    "Usage: gettxffe [retimer id] [channel id]\nRead tx ffe from retimer.\n")
ADD_DEBUG_COMMAND("geteyediag", retimer_get_eye_diag_proc, "yy",
    "Usage: geteyediag [retimer id] [channel id]\nRead eye diag from retimer.\n")
ADD_DEBUG_COMMAND("getreg", retimer_get_reg_info_proc, "yu",
    "Usage: getreg [retimer id] [reg addr]\nRead reg info from retimer.\n")
ADD_DEBUG_COMMAND("setchannel", retimer_set_channel_proc, "yu",
    "Usage: setchannel [retimer id] [flag]\nSet switch info from retimer.\n")
ADD_DEBUG_COMMAND("getidle", retimer_get_idle_proc, "y", "Usage: getidle [retimer id]\nGet idle info from retimer.\n")
ADD_DEBUG_COMMAND("getprbs", retimer_get_prbs_error_check_proc, "yyy",
    "Usage: getprbs [retimer id] [channel id] [lane num]\
    \nGet prbs error check info from retimer.\n")
ADD_DEBUG_COMMAND("setprbs", retimer_set_prbs_send_recv_proc, "yyyyy",
    "Usage: setprbs [retimer id] [channel id] [lane num] [prbs type] [prbs dir]\
    \nSet prbs send recv info from retimer.\n")
ADD_DEBUG_COMMAND("settxswing", retimer_set_tx_swing_proc, "yyyy",
    "Usage: settxswing [retimer id] [channel id] [lane num] [swing]\
    \nSet tx swing info from retimer.\n")
ADD_DEBUG_COMMAND("setroutine", retimer_turn_routine_on_or_off_proc, "y",
    "Usage: setroutine [open or close]\nSet routine info from retimer.\n")

ADD_DEBUG_COMMAND("get_exp_board_led_state", debug_get_exp_board_led, "yy", "Usage: get_exp_board_led_state\
                      [led type:0 power, 1 alarm] [exp board id]\nGet expander board led\n")
ADD_DEBUG_COMMAND("waterdetcab_debug", waterdetcab_debug, "yy",
    "Usage: start waterdetcab debug [debug_mode 1:open 0:close] [signal 1:present 0:abesent].\n")
ADD_DEBUG_COMMAND("leakage_debug", leakage_debug, "yy",
    "Usage: start leakage debug [debug_mode 1:open 0:close] [signal 1:leakage 0:no_leakage].\n")
END_DEBUG_COMMAND()

// -- Ä£¿é³õÊ¼»¯¼°´¦Àí£¬±ØÌî

BEGIN_MODULE_MAIN()
#ifdef ITEST
iTest_Init(argc, argv);
#endif
// -- ±¾Ä£¿éÊµÏÖµÄ³õÊ¼»¯º¯Êý£¬¿ÉÑ¡
MODULE_INIT(card_manage_init)
MODULE_START(card_manage_start)
#ifdef ITEST
start_tshell_main(argc, argv);
#endif
END_MODULE_MAIN()
