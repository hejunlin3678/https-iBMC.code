/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: storage_mgnt模块管理的Hdd类相关宏定义
 * History: 2022/8/29 首次创建
 */

#ifndef PROPERTY_METHOD_HDD_H
#define PROPERTY_METHOD_HDD_H


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* Hdd硬盘类 */
#define MODULE_HDD_NAME                         "Harddisk"
#define CLASS_HDD_NAME                          "Hdd"
/* BEGIN: Modified by zwx382233, 2017/5/19 9:7:12   问题单号:DTS2017051802343 */
#define CLASS_PCIEHDD_NAME                      "PCIeDisk"
/* END:   Modified by zwx382233, 2017/5/19 9:7:15 */
#define PROPERTY_HDD_ID                         "Id"
#define PROPERTY_HDD_NAME                       "Name"
#define PROPERTY_HDD_PRESENCE                   "Presence"
/* BEGIN:   Added by chenshihao (cwx398307), DTS: 2017060813319 2017/10/14 */
/* 用来区分AgentHdd对象 */
#define PROPERTY_HDD_VIRTUALHDD        "VirtualHdd"
/* END:   Added by chenshihao (cwx398307), DTS: 2017060813319 2017/10/14 */

#define PROPERTY_HDD_SATAPRESENCE               "SATAPresence"
#define PROPERTY_HDD_PCIEPRESENCE               "PciePresence"
#define PROPERTY_HDD_INTERFACE_TYPE_STR         "InterfaceTypeString"
#define PROPERTY_HDD_FAULT                      "Fault"
#define PROPERTY_HDD_ACTIVATION                 "Activation"
#define PROPERTY_HDD_LOCATION                   "Location"
#define PROPERTY_HDD_NVME_OWNER                  "NVMeOwner"
#define PROPERTY_HDD_FAILEDARRAY                "FailedArray"
#define PROPERTY_HDD_DEVICEFAULT                "DeviceFault"
#define PROPERTY_HDD_PREDICTIVEFAILURE          "PredictiveFailure"
#define PROPERTY_HDD_DEVICEPREDICTIVEFAILURE    "DevicePredictiveFailure"
#define PROPERTY_HDD_REBUILDPROGRESS            "RebuildProgress"
#define PROPERTY_HDD_DISKLED                    "DiskLed"
#define PROPERTY_HDD_OWN_SLOT_ID                "OwnSlotId"
#define PROPERTY_HDD_PLANE_ID                   "PlaneId"
#define PROPERTY_HDD_RES_ID                   "ResId"
#define PROPERTY_HDD_REF_COMPONENT    "RefComponent"

#define PROPERTY_HDD_STORAGE_LOGIC     "StorageLoc"
#define PROPERTY_HDD_FDE_CAPABLE       "FDECapable"
#define PROPERTY_HDD_ROTATION_SPEED    "RotationSpeed"
#define PROPERTY_HDD_FORM_FACTOR       "FormFactor"
#define PROPERTY_HDD_SERIAL_NUMBER     "SerialNumber"
#define PROPERTY_HDD_MODEL_NUMBER      "ModelNumber"
#define PROPERTY_HDD_MANUFACTURER      "Manufacturer"
#define PROPERTY_HDD_FIRMWAREVERION    "FirmwareVersion"

#define PROPERTY_HDD_PART_NUMBER       "PartNum"
#define PROPERTY_HDD_PREVIOUS_SN       "PreviousSN"
#define PROPERTY_HDD_REPLACE_FLAG      "ReplaceFlag"

#define PROPERTY_HDD_POWER_SWITCH      "PowerStatus"
#define PROPERTY_HDD_DFP_PROBABILITY   "DFPPropability"

/* BEGIN: Modified by gwx455466, 2018/5/29 13:56:16   问题单号:DTS2018052900008 */
#define PROPERTY_HDD_PCIE_POWER_REMOVE     "PCIePowerRemove"
/* END:   Modified by gwx455466, 2018/5/29 13:56:20 */

/* BEGIN: Modified by gwx455466, 2018/5/28 15:22:45   问题单号:DTS2018031800869 */
#define CLASS_PCIE_HDD_REMOVE_MONITOR                          "PcieHddRemoveMonitor"
#define CLASS_PCIE_HDD_REMOVE_MONITOR_SLOT                          "Slot"
#define CLASS_PCIE_HDD_REMOVE_MONITOR_POWER_ENABLE                          "PowerEnable"
#define CLASS_PCIE_HDD_REMOVE_MONITOR_SAVE_REMOVE                          "SaveRemove"
/* END:   Modified by gwx455466, 2018/5/28 15:22:51 */

/* BEGIN:Add by litingting, 2018/05/18, PN:SR.SFEA02130837.005.005 */
#define CLASS_VPD_NAME                 "VPD"
#define PROPERTY_VPD_CHIP              "Chip"
#define PROPERTY_VPD_TYPE              "Type"
#define PROPERTY_VPD_SERIALNUMBER      "SerialNumber"
#define PROPERTY_VPD_MODELNUMBER       "ModelNumber"
#define PROPERTY_VPD_MANUFACTURER      "Manufacturer"
#define PROPERTY_VPD_VENDORID          "VendorId"
#define PROPERTY_VPD_PCIELINKSPEED     "PCIeLinkSpeed"
#define PROPERTY_VPD_INTERFACESPEED    "InterfaceSpeed"
/* BEGIN: Added for AR.SR.SFEA02130917.003.001 by lwx459244, 2018/7/22 */
#define PROPERTY_VPD_CAPACITY          "Capacity"
#define PROPERTY_VPD_CAPACITY_MB       "CapacityMB"
#define PROPERTY_VPD_PHYSICAL_TYPE     "PhysicalType"
/* END:   Added for AR.SR.SFEA02130917.003.001 by lwx459244, 2018/7/22 */
/* END:Add by litingting, 2018/05/18, PN:SR.SFEA02130837.005.005 */

#define CLASS_HDD                           "Hdd"     // 重复的宏定义，但是有很多接口使用此宏定义，因此保留此定义。新增接口不再使用
#define PROPERTY_HDD_HEALTH                 "HddHealth"
#define PROPERTY_HDD_ENTITY                 "HddEntity"
/* Start by h00272616  DTS2016062504101 20160628 */
#define  PROPERTY_HDD_HDDENTITY_NAME    "HddEntity"
/* End by h00272616  DTS2016062504101 20160628 */
#define PROPERTY_HDD_PYSICAL_LOCATION       "PhysicalLocation"
#define PROPERTY_HDD_FUNCTION               "Function"
#define PROPERTY_HDD_DEVICENAME             "DeviceName"
#define PROPERTY_HDD_ID                 "Id"
/* BEGIN: Added by cwx398307, 2018/11/26, DTS2018112006934 */
#define PROPERTY_HDD_FW_STATUS_FAULT                 "FWStatusFault"
/* END:   Added by cwx398307, 2018/11/26 */
/* BEGIN: Added by huanghan (h00282621), 2016/4/13   问题单号:SR-0000245426-003 */
#define PROPERTY_HDD_HEALTH_CODE            "HealthCode"
#define PROPERTY_HDD_DEVICE_ID              "DeviceId"
#define PROPERTY_HDD_SAS_ADDR1              "SASAddress1"
#define PROPERTY_HDD_SAS_ADDR2              "SASAddress2"
#define PROPERTY_HDD_MEDIA_ERR_COUNT        "MediaErrorCount"
#define PROPERTY_HDD_PREFAIL_ERR_COUNT      "PrefailErrorCount"
#define PROPERTY_HDD_OTHER_ERR_COUNT        "OtherErrorCount"
#define PROPERTY_HDD_FIRMWARE_STATUS        "FirmwareStatus"
#define PROPERTY_HDD_POWER_STATE            "PowerState"
#define PROPERTY_HDD_TEMPERATURE            "Temperature"
#define PROPERTY_HDD_REF_POLICY             "RefPolicy"
#define PROPERTY_HDD_CAPACITY               "Capacity"
/* BEGIN: Added by Yangshigui YWX386910, 2017/1/10   问题单号:DTS2016122703485 */
#define PROPERTY_HDD_CAPACITY_MB            "CapacityMB"
/* END:   Added by Yangshigui YWX386910, 2017/1/10 */
#define PROPERTY_HDD_BLOCK_SIZE             "BlockSize"
#define PROPERTY_HDD_CAPACITY_ERROR         "CapacityError"
#define PROPERTY_HDD_FIRMWARE_VERSION       "FirmwareVersion"
#define PROPERTY_HDD_MEDIA_TYPE             "MediaType"
#define PROPERTY_HDD_INTERFACE_TYPE         "InterfaceType"
#define PROPERTY_HDD_INTERFACE_SPEED        "InterfaceSpeed"
#define PROPERTY_HDD_LINK_SPEED             "LinkSpeed"
#define PROPERTY_HDD_HOT_SPARE              "HotSpare"
#define PROPERTY_HDD_REBUILD_STATE          "RebuildState"
#define PROPERTY_HDD_PATROL_STATE           "PatrolState"
#define PROPERTY_HDD_REMNANT_MEDIA_WEAROUT  "RemnantMediaWearout"
#define PROPERTY_HDD_REF_RAID_CONTROLLER    "RefRAIDController"
#define PROPERTY_HDD_SLOT_NUMBER            "SlotNumber"
/* BEGIN: Modified by gwx455466, 2017/9/4 14:49:9   问题单号:DTS2017083101891  */
#define PROPERTY_HDD_ENCLOSURE_DEVICE_ID              "EnclosureDeviceId"
/* END:   Modified by gwx455466, 2017/9/4 14:49:12 */
#define PROPERTY_HDD_MISSING                "Missing"
#define PROPERTY_HDD_PHY_ERR_STATE          "PHYErrState"
#define PROPERTY_HDD_CONNECT_FRU_ID         "ConnectFruId"
#define PROPERTY_HDD_CONNECT_FRU_NAME       "ConnectFruName"
#define PROPERTY_HDD_BOOT_PRIORITY          "BootPriority"
#define METHOD_HDD_SET_LOCATE_STATE         "SetLocateState"
#define METHOD_HDD_SET_HOT_SPARE            "SetHotspare"
#define METHOD_HDD_SET_FIRMWARE_STATE       "SetFirmwareState"
#define METHOD_HDD_SET_HDD_ID               "SetHddId"
#define METHOD_SET_DFP_PROBABILITY          "SetDfpProbability"
#define METHOD_HDD_SET_PATROL_STATE         "SetPatrolState"

/* BEGIN: Added by z00352904, 2016/12/6 21:14:41  PN:AR-0000264509-001-001 */
#define PROPERTY_HDD_ATTR_REV_NUM           "AttrRevisionNum"
#define PROPERTY_HDD_ATTR_ITEM_NUM          "AttrItemsNum"
#define PROPERTY_HDD_ATTR_ITEM_LIST         "AttrItemList"
#define PROPERTY_HDD_HEAL_STATUS            "HealthStatus"
#define PROPERTY_HDD_DRIVE_TEMP             "DriveTemperature"
#define PROPERTY_HDD_TRIP_TEMP              "TripTemperature"
#define PROPERTY_HDD_ELE_GROWN_LIST         "ElementsInGrownDefectList"
#define PROPERTY_HDD_ELE_DEFECT_LIST        "ElementsInPrimaryDefectList"
#define PROPERTY_HDD_MNU_WEEK_YEAR          "ManufacturedInWeekOfYear"
#define PROPERTY_HDD_BLOCK_SEND             "BlocksSentToInitiator"
#define PROPERTY_HDD_BLOCK_FROM             "BlocksReceivedFromInitiator"
#define PROPERTY_HDD_HOUR_POWER_UP          "HoursOfPoweredUp"
#define PROPERTY_HDD_MINU_SMART_TEST        "MinuUntilNextInterSMARTTest"
#define PROPERTY_HDD_OS_LAST_EVENT_NUM      "LastPredFailEventSeqNum"
#define PROPERTY_HDD_BDF_PORT               "BDFPort"
/* END:   Added by z00352904, 2016/12/6 21:14:44 */
#define PROPERTY_HDD_VENDORID          "VendorId"
/* BEGIN: Modified by zwx382755, 2018/1/24   PN:DTS2017122308881 */
#define PROPERTY_HDD_PCIE_LINK_SPEED        "PCIeLinkSpeed"
/* END:   Modified by zwx382755, 2018/1/24 */
#define PROPERTY_HDD_IBMA_EVENT             "iBMAEvent"
#define PROPERTY_HDD_FLASH_TIMESTAMP        "FlashTimeStamp"
#define PROPERTY_HDD_OOB_SUPPORT            "OOBSupport"

/* BEGIN: Added by 00356691 zhongqiu, 2018/5/19   PN:AR.SR.SFEA02130925.003.001 支持采集的数据列表 */
#define METHOD_HDD_COLLECT_HDD_SMART       "CollectHddSMART"
/* END:   Added by 00356691 zhongqiu, 2018/5/19   PN:AR.SR.SFEA02130925.003.001 支持采集的数据列表 */
/* BEGIN: Modified by zwx382755, 2018/9/26   PN:AR.SR.SFEA02130924.051.001 */
#define METHOD_HDD_CRYPTO_ERASE             "CryptoErase"
/* END:   Modified by zwx382755, 2018/9/26 */
/* END:   Added by huanghan (h00282621), 2016/4/13 */
#define METHOD_HDD_SET_BOOTABLE            "SetBootable"

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* PROPERTY_METHOD_HDD_H */

