/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: storage_mgnt模块管理的RaidController类及BBU类相关宏定义
 * History: 2022/9/2 首次创建
 */

#ifndef RAID_CONTROLLER_H
#define RAID_CONTROLLER_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define CLASS_RAID_CONTROLLER_NAME                  "RaidController"
#define PROPERTY_RAID_CONTROLLER_ID                 "Id"
#define PROPERTY_RAID_CONTROLLER_NAME               "Name"
#define PROPERTY_RAID_CONTROLLER_TYPE               "Type"
#define PROPERTY_RAID_CONTROLLER_OOB_SUPPORT        "OOBSupport"
#define PROPERTY_RAID_CONTROLLER_COMPONENT_NAME     "ComponentName"
#define PROPERTY_RAID_CONTROLLER_TYPE_ID            "TypeID"
#define PROPERTY_RAID_CONTROLLER_CHIP_OBJECT        "RAIDChip"
#define PROPERTY_RAID_CONTROLLER_CPLD_UPGRD_CHIP_OBJECT        "CpldUpgrdChip"
#define PROPERTY_RAID_CONTROLLER_CPLD_CHIP_TYPE     "CpldChipType"
#define PROPERTY_RAID_CONTROLLER_PCI_BUS            "BusNum"
#define PROPERTY_RAID_CONTROLLER_PCI_DEVICE         "DeviceNum"
#define PROPERTY_RAID_CONTROLLER_PCI_FUNCTION       "Function"
#define PROPERTY_RAID_CONTROLLER_PCI_SEGMENT        "Segment"
#define PROPERTY_RAID_CONTROLLER_FW_VERSION         "FirmwareVersion"
#define PROPERTY_RAID_CONTROLLER_NVDT_VERSION       "NVDataVersion"
#define PROPERTY_RAID_CONTROLLER_MEM_SIZE           "MemorySize"
#define PROPERTY_RAID_CONTROLLER_ECC_COUNT          "EccCount"
#define PROPERTY_RAID_CONTROLLER_SAS_ADDR           "SASAddr"
#define PROPERTY_RAID_CONTROLLER_DEVICE_INTERFACE   "DeviceInterface"
#define PROPERTY_RAID_CONTROLLER_DEVICE_INTERFACE_STR   "DeviceInterfaceString"
#define PROPERTY_RAID_CONTROLLER_MODE               "Mode"
#define PROPERTY_RAID_CONTROLLER_CACHE_PINNED       "CachePinned"
#define PROPERTY_RAID_CONTROLLER_MAINT_PD_FAIL      "MaintPDFailHistory"
#define PROPERTY_RAID_CONTROLLER_COPYBACK_ENABLED   "CopybackEnabled"
#define PROPERTY_RAID_CONTROLLER_SMARTER_COPYBACK_ENABLED   "SMARTerCopybackEnabled"
#define PROPERTY_RAID_CONTROLLER_JBOD_ENABLED       "JBODEnabled"
#define PROPERTY_RAID_CONTROLLER_CC_ENABLED         "LDCCEnabled"
#define PROPERTY_RAID_CONTROLLER_CC_PERIOD          "LDCCPeriod"
#define PROPERTY_RAID_CONTROLLER_CC_RATE            "LDCCRate"
#define PROPERTY_RAID_CONTROLLER_CC_REPAIR          "LDCCAutoRepairEnabled"
#define PROPERTY_RAID_CONTROLLER_CC_STATUS          "LDCCStatus"
#define PROPERTY_RAID_CONTROLLER_CC_TOTALVD         "LDCCTotalVD"
#define PROPERTY_RAID_CONTROLLER_CC_COMPLETEDVD     "LDCCCompletedVD"
#define PROPERTY_RAID_CONTROLLER_CC_DELAY           "LDCCDelay"
#define PROPERTY_RAID_CONTROLLER_MIN_STRIP_SUPPORT  "MinStripSize"
#define PROPERTY_RAID_CONTROLLER_MAX_STRIP_SUPPORT  "MaxStripSize"
#define PROPERTY_RAID_CONTROLLER_HEALTH_STATUS_CODE         "HealthStatusCode"
#define PROPERTY_RAID_CONTROLLER_REF_OBJECT_BBUSTATUS       "RefObjectBBUStatus"
#define PROPERTY_RAID_CONTROLLER_REF_OBJECT_SASPHYSTATUS    "RefObjectSasPHYStatus"
#define PROPERTY_RAID_CONTROLLER_COMMUNICATION_LOSS "CommunicationLoss"
#define PROPERTY_RAID_CONTROLLER_PWR_HEALTH_STATUS   "PowerHealthStatus"
#define PROPERTY_RAID_CONTROLLER_CTRL_OPTION1       "CtrlOption1"
#define PROPERTY_RAID_CONTROLLER_CTRL_OPTION2       "CtrlOption2"
#define PROPERTY_RAID_CONTROLLER_CTRL_OPTION3       "CtrlOption3"
#define PROPERTY_RAID_CONTROLLER_TEMP               "RaidCtrlTemp"
#define PROPERTY_RAID_CONTROLLER_COM_CHANNEL        "ComChannel"
#define PROPERTY_RAID_CONTROLLER_DRIVE_NAME         "DriverName"
#define PROPERTY_RAID_CONTROLLER_DRIVE_VERSION      "DriverVersion"
#define PROPERTY_RAID_CONTROLLER_MANUFACTURER       "Manufacturer"
#define PROPERTY_RAID_CONTROLLER_SERIALNUMBER       "SerialNumber"
#define PROPERTY_RAID_CONTROLLER_BMAID              "BMAId"
#define PROPERTY_RAID_CONTROLLER_HARDWARE_FAULT     "HardwareFault"
#define PROPERTY_RAID_CONTROLLER_SERIAL_NUMBER      "SerialNumber"
#define PROPERTY_RAID_CONTROLLER_EID                "Eid"
#define PROPERTY_RAID_CONTROLLER_PHY_ADDR           "PhyAddr"
#define PROPERTY_RAID_CONTROLLER_SPARE_ACTIVATION_MODE  "SpareActivationMode"
#define PROPERTY_RAID_CONTROLLER_PCIE_LINK_WIDTH        "PCIeLinkWidth"
#define PROPERTY_RAID_CONTROLLER_HARDWARE_REVISION      "HardwareRevision"
#define PROPERTY_RAID_CONTROLLER_NO_BATTERY_WRITE_CACHE "NoBatteryWriteCache"
#define PROPERTY_RAID_CONTROLLER_READ_CACHE_PERCENT     "ReadCachePercent"
#define PROPERTY_RAID_CONTROLLER_BOOT_DEVICES       "BootDevices"

#define METHOD_RAID_CONTROLLER_SET_SERIAL_NUMBER    "SetSerialNumber"
#define METHOD_RAID_CONTROLLER_CREATE_COMMON_LD     "CreateCommonLD"
#define METHOD_RAID_CONTROLLER_ADD_LD               "AddLD"
#define METHOD_RAID_CONTROLLER_CREATE_CACHECADE_LD  "CreateCachecadeLD"
#define METHOD_RAID_CONTROLLER_DELETE_LD            "DeleteLD"
#define METHOD_RAID_CONTROLLER_SET_COPYBACK         "SetCopyback"
#define METHOD_RAID_CONTROLLER_SET_SMARTER_COPYBACK "SetSMARTerCopyback"
#define METHOD_RAID_CONTROLLER_SET_JBOD             "SetJBOD"
#define METHOD_RAID_CONTROLLER_SET_CONSIS_CHECK     "SetConsisCheck"
#define METHOD_RAID_CONTROLLER_SET_NBWC             "SetNBWC"
#define METHOD_RAID_CONTROLLER_SET_READ_CAHCE_PERCENT    "SetReadCachePercent"
#define METHOD_RAID_CONTROLLER_SET_WCP              "SetDriveWriteCachePolicy"
#define METHOD_RAID_CONTROLLER_RESTORE_SETTINGS     "RestoreSettings"
#define METHOD_RAID_CONTROLLER_SET_MODE             "SetMode"
#define METHOD_RAID_CONTROLLER_SET_MAINTPDFAILHISTORY  "SetMaintPDFailHistory"
#define METHOD_RAID_CONTROLLER_SET_MCTP_IDENTIFY    "SetMCTPIdentity"
#define METHOD_RAID_CONTROLLER_IMPORT_FOREIGN_CONFIG "ImportForeignConfig"
#define METHOD_RAID_CONTROLLER_CLEAR_FOREIGN_CONFIG "ClearForeignConfig"
#define METHOD_PCIERAID_SET_ROOTBDF                 "SetRootBDF"
#define METHOD_RAID_CONTROLLER_SET_BOOT_DEVICES      "SetBootDevices"
#define METHOD_RAID_CONTROLLER_DUMP_LOG             "DumpLog"
#define METHOD_RAID_CONTROLLER_GET_DUMP_PHASE       "GetDumpPhase"

#define CLASS_NAME_AGENT_RAID_CONTROLLER            "AgentRaidController"
#define CLASS_NAME_AGENT_BBU                        "AgentBBU"
#define METHOD_SET_AGENT_PROPERTY                   "SetAgentProperty"

#define CLASS_BBU_NAME                      "BBU"
#define PROPERTY_BBU_PRESENT                "Present"    /* 禁止任何场景下设置BBU在位信息为0xff 否者会导致BBU传感器异常 */
#define PROPERTY_BBU_TYPE                   "Type"
#define PROPERTY_BBU_HEALTH                 "Health"
#define PROPERTY_BBU_PACK_MISSING           "PackMissing"
#define PROPERTY_BBU_VOLTAGE_LOW            "VoltageLow"
#define PROPERTY_BBU_TEMPERATURE_HIGH       "TemperatureHigh"
#define PROPERTY_BBU_TEMPERATURE            "Temperature"
#define PROPERTY_BBU_REPLACE_PACK           "ReplacePack"
#define PROPERTY_BBU_LEARN_CYCLE_FAILED     "LearnCycleFailed"
#define PROPERTY_BBU_LEARN_CYCLE_TIMEOUT    "LearnCycleTimeout"
#define PROPERTY_BBU_REMAINING_CAPACITY_LOW "RemainingCapacityLow"
#define PROPERTY_BBU_PREDICTIVE_FAILURE     "PredictiveFailure"
#define PROPERTY_BBU_NO_SPACE_FOR_CACHE_OFFLOAD   "NoSpaceForCacheOffload"
#define PROPERTY_BBU_FAILED    "Failed"

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* RAID_CONTROLLER_H */