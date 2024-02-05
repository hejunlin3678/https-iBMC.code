/******************************************************************************

                  版权所有 (C), 2001-2021, 华为技术有限公司

 ******************************************************************************
  文 件 名   : property_method.h
  版 本 号   : 初稿
  作    者   : zhenggenqiang 207985
  生成日期   : 2013年7月13日
  最近修改   :
  功能描述   : PME软件的所有类的属性方法描述文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年7月13日
    作    者   : zhenggenqiang 207985
    修改内容   : 创建文件

******************************************************************************/

#ifndef __PME_PROPERTY_METHOD_APP_H__
#define __PME_PROPERTY_METHOD_APP_H__

#include "diagnose/property_method_diagnose.h"
#include "storage_mgnt/ub_port.h"
#include "storage_mgnt/unit_cable_config.h"
#include "storage_mgnt/raid_controller.h"
#include "storage_mgnt/property_method_hdd.h"
#include "property_method_mngtportcap.h"
#include "property_method_bmc_global.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

//注册给DFM的模块名称，以下宏名不能修改，会影响持久化数据恢复
#define MODULE_NAME_FILE_MANAGE            "FileManage"
#define MODULE_NAME_AGENTAPP               "agentapp"
#define MODULE_NAME_MCTP                   "MCTP"
#define MODULE_NAME_BIOS                   "BIOS"
#define MODULE_NAME_BMC_GLOBAL             "BMC"
#define MODULE_NAME_CARD_MANAGE            "card_manage"
#define MODULE_NAME_COOLING                "cooling_app"
#define MODULE_NAME_CPU_MEM                "CpuMem"
#define MODULE_NAME_DATA_SYNC              "data_sync"
#define MODULE_NAME_DFT                    "Dft"
#define MODULE_NAME_DISCOVERY              "discovery"
#define MODULE_NAME_SWITCH_CARD            "switch_card"
#define MODULE_NAME_FDM                    "diagnose"
#define MODULE_NAME_HPC_MGNT               "hpc_mgmt"
#define MODULE_NAME_IPMI                   "ipmi_app"
#define MODULE_NAME_KVM                    "kvm_vmm"
#define MODULE_NAME_LCD                    "Lcd"
#define MODULE_NAME_MAINT_DEBUG            "MaintDebug"
#define MODULE_NAME_NET_NAT                "net_nat"
#define MODULE_NAME_PCIE_SWITCH            "PcieSwitch"
#define MODULE_NAME_POWER_MGNT             "PowerMgnt"
#define MODULE_NAME_REDFISH                "redfish"
#define MODULE_NAME_RIMM                   "rimm"
#define MODULE_NAME_SENSOR_ALARM           "sensor_alarm"
#define MODULE_NAME_SMM_MGNT               "Smm_Mgnt"
#define MODULE_NAME_UI_MANAGE              "Snmp"
#define MODULE_NAME_STORAGE_MGNT           "StorageMgnt"
#define MODULE_NAME_UPGRADE                "UPGRADE"
#define MODULE_NAME_PANGEA_UPGRADE         "pangea_upgrade"
#define MODULE_NAME_COOLING_SMM_STD        "cooling_smm_std"
#define MODULE_NAME_USER                   "User"
#define MODULE_NAME_ASM                    "Asm"
#define MODULE_NAME_MSM                    "msm"
#define MODULE_NAME_DATA_SYNC2             "data_sync2"
#define MODULE_NAME_SMM_LSW                "smm_lsw"
#define MODULE_NAME_LICENSE_MGNT           "LicenseMgnt"
#define MODULE_NAME_RACK_MGNT              "rack_mgnt"
#define MODULE_NAME_LOCAL_KVM              "localKVM"
#define MODULE_NAME_DDNS                   "ddns"
#define MODULE_NAME_AGENTLESS              "agentless"
#define MODULE_NAME_USB_MGNT               "usb_mgmt"
#define MODULE_NAME_NET_CONFIG             "NetConfig"
#define MODULE_NAME_PANGEA_BOARD           "pangea_board"
#define MODULE_NAME_PANGEA_CARD            "pangea_card"

//增加LSW Center、LSW Control Local、LSW Data Local 3个交换节点
#define MODULE_NAME_LSW_CENTER              "LswCenter"
#define MODULE_NAME_LSW_CONTROL_LOCAL       "LswControlLocal"
#define MODULE_NAME_LSW_DATA_LOCAL          "LswDataLocal"
#define MODULE_NAME_LSW_MAIN_LOCAL          "LswMainLocal"
#define MODULE_NAME_DFP                    "Dfp"


#define AGENT_OBJECT_NAME_PREFIX                        "Agent"

#define COMMON_FILE_SYNC_METHOD                 "FileSyncMethod"
#define COMMON_PROPERTY_SYNC_METHOD             "PropertySyncMethod"
#define METHOD_UPDATE_LDAP_CIPHERTEXT           "UpdateLDAPCiphertext"
#define METHOD_UPDATE_SSL_FILE                  "UpdateSSLFile"
#define COMMON_CERTINFO_PROPERTY_SYNC_METHOD    "CertInfoPropertySyncMethod"

#define COMMON_REPLACED_PRE_SN      "PreviousSN"
#define COMMON_REPLACED_CUR_SN      "SerialNumber"
#define COMMON_REPLACED_FLAG        "ReplaceFlag"
#define COMMON_HWSR_VERSION         "HWSRVersion"

#define CLASS_USB_MGMT                          "USBMgmt"
#define OBJ_USB_MGMT                            CLASS_USB_MGMT
#define PROPERTY_USB_MGMT_SUPPORT               "Support"
#define PROPERTY_USB_MGMT_PRESENCE              "Presence"
#define PROPERTY_USB_MGMT_ENABLE                "Enable"
#define PROPERTY_USB_MGMT_CC_MODE               "CCMode"
#define PROPERTY_USB_MGMT_CC_CHIPS              "CCChips"
#define PROPERTY_USB_MGMT_DEVICE_STATUS         "DeviceStatus"
#define PROPERTY_USB_MGMT_GLED_STATUS           "GLedStatus"
#define PROPERTY_USB_MGMT_RLED_STATUS           "RLedStatus"
#define PROPERTY_USB_MGMT_USB_NETWORK_IP_ADDR   "USBNetworkIpAddr"
#define PROPERTY_USB_MGMT_RNDIS_HOST_IP_ADDR    "RndisHostIpAddr"
#define PROPERTY_USB_MGMT_OPERATE_STEP          "OperateStep"
#define PROPERTY_USB_MGMT_OPERATE_STATUS        "OperateStatus"
#define PROPERTY_USB_MGMT_UNCOMPRESS_PASSPWORD  "UncompressPassword"
#define PROPERTY_USB_MGMT_MASTAER_KEYID         "UncompressMasterKeyId"
#define PROPERTY_USB_MGMT_USB_PORT_NUM   "PortNum"
#define METHOD_USB_MGMT_SET_ENABLE              "SetEnable"
#define METHOD_USB_MGMT_UNCOMPRESS_PASSWORD     "SetUncompressPassword"
#define METHOD_USB_MGMT_UPDATE_CIPHERTEXT       "UpdateUSBCiphertext"

/* Energy_Canchip类对象及属性 */
#define CLASS_ENERGY_CANCHIP                   "Energy_Canchip"
#define PROPERTY_ENERGY_CANCHIP_PROTOCOL       "Protocol"
#define PROPERTY_ENERGY_CANCHIP_ADDR            "Addr"
#define PROPERTY_ENERGY_CANCHIP_READTMOUT      "ReadTmout"


/* PsuSlot类的对象及属性 */
#define CLASS_POWER_SLOT                                "PsuSlot"
#define PROPERTY_SLOT_ID                                "SlotId"
#define PROPERTY_SLOT_I2C_ADDR                          "SlotI2cAddr"
#define PROPERTY_SLOT_I2C_CHAN                          "I2cId"
/* BEGIN : Added by d00381887, 2016/07/12,PN: AR-0000276585-016-016 */
#define PROPERTY_SLOT_PSU_CHIP                          "PsuChip"
/* BEGIN : Added by d00381887, 2016/07/12,PN: AR-0000276585-016-016 */
#define PROPERTY_SLOT_FRU_CHIP                          "FruChip"
#define PROPERTY_PSUSLOT_OUTPUT_STATUS_NAME             "OutputStatus"

/* BEGIN: Added by p00381829, 2017/12/30   MCTP*/
#define  MCTP_CLASS_NAME               "MctpCtrl"
#define PROPERTY_CLASS_ID_VALUE                       ("MctpID")
#define PROPERTY_ME_EID_VALUE                       ("MeEid")
#define PROPERTY_ME_PHY_VALUE                       ("MePhy")
#define PROPERTY_BMC_EID_VALUE                       ("BmcEid")
#define PROPERTY_BMC_PHY_VALUE                       ("BmcPhy")
#define PROPERTY_ROUTETABLE_STATUS                       ("TableStatus")
#define PROPERTY_DISCOVER_FLAG                        ("DiscoverFlag")
#define PROPERTY_BMC_SMBUS_EID                        "BmcSMBusEid"
#define PROPERTY_BMC_SMBUS_PYHADDR                    "BmcSMBusPhyAddr"

#define METHOD_MCTP_ROUTE_TABLE_TO_CHANGE             ("ChangeRouteTable")
/* END:   Added by p00381829, 2017/12/30  */

#define CLASS_MCTP_END_POINT                    "MCTPEndpoint"
#define PROPERTY_MCTP_EP_MEDIUM                 "Medium"
#define PROPERTY_MCTP_EP_CHIP                   "Chip"
#define PROPERTY_MCTP_EP_EID                    "EID"
#define PROPERTY_MCTP_EP_PHYADDR                "PhyAddr"
#define PROPERTY_MCTP_EP_COMMSTATE              "CommState"
#define PROPERTY_MCTP_EP_ISDISCOVERED           "IsDiscovered"
#define CLASS_SMBUS_CHIP_NAME                   "SMBusChip"
#define PROPERTY_SMBUS_CHIP_ADDR                "Addr"

/* Chip类的对象及属性 */
#define CLASS_CHIP                                      "Chip"
/* BEGIN: Added by z00382825, 2017/7/24   PN: AR-0000276585-008-001 */
#define OBJECT_CPLD1                                    "cpld1"
#define OBJECT_CPLD2                                    "cpld2"
/* END: Added by z00382825, 2017/7/24   PN: AR-0000276585-008-001 */

/* BEGIN: Added by wangwei wwx549865, 2019/3/18   PN:UADP164278*/
#define OBJECT_FPGA                                     "fpga"
/* END:   Added by wangwei wwx549865, 2019/3/18 */

#define PROPERTY_CHIP_ADDR                              "Addr"
#define PROPERTY_CHIP_LBUS                              "LBus"
#define PROPERTY_CHIP_PRE_CHIP                          "PrevChip"
#define PROPERTY_CHIP_CACHE_BUF                         "CacheBuf"
#define PROPERTY_CHIP_CACHE_SUSPENDED                   "CacheSuspended"

/* Chip子类名称 */
#define CLASS_CHIP_SUBCLASS_EEPROM                      "Eeprom"
#define CLASS_CHIP_SUBCLASS_FANCHIP                     "FanChip"
#define CLASS_CHIP_SUBCLASS_VDDQ                        "Vddq"
#define CLASS_CHIP_SUBCLASS_PCA9541                     "Pca9541"
#define CLASS_CHIP_SUBCLASS_PCA9545                     "Pca9545"
#define CLASS_CHIP_SUBCLASS_PCA9555                     "Pca9555"
#define CLASS_CHIP_SUBCLASS_LM75                        "Lm75"
#define CLASS_CHIP_SUBCLASS_LM96080                     "Lm96080"
#define CLASS_CHIP_SUBCLASS_RTC                         "Rtc"
#define CLASS_CHIP_SUBCLASS_PT7C4388                    "Pt7c4388"
/*BEGIN  ADD by z00397989 AR-0000276595-003-016  2017.04.20 */
#define CLASS_CHIP_SUBCLASS_PS8796          "Pex8796"
#define CLASS_CHIP_SUBCLASS_PS8725          "Pex8725"
#define CLASS_CHIP_SUBCLASS_PS9765          "Pex9765"
#define CLASS_CHIP_SUBCLASS_CPLD_PS9765          "PswCpld"
/*END  ADD by z00397989 AR-0000276595-003-016  2017.04.20 */

#define CLASS_CHIP_SUBCLASS_INA220        "Ina220"
#define PROPERTY_INA220_CONFIGREG         "ConfigReg"
#define PROPERTY_INA220_CALIBRATION       "Calibration"
#define PROPERTY_INA220_POWERINDIVISOR    "PowerInDivisor"
#define PROPERTY_INA220_POWERINMULTIPLIER "PowerInMultiplier"
#define PROPERTY_CHIP_INDEX               "Index"

/* VirtualI2cIP2的对象及属性 */
#define CLASS_CHIP_SUBCLASS_PR0832          "Idt0832"
/* END ADD by z00382105 */

/* BEGIN: Added by z00352904, 2017/6/28 19:24:31  PN:支持OPA单板装备测试需求 */
#define PROPERTY_EEPROM_STATUS                          "EepStatus"
/* END:   Added by z00352904, 2017/6/28 19:24:33*/
#define PROPERTY_EEPROM_WRITECOUNT                      "WriteCount"
#define METHOD_SET_EEPROM_STATUS                        "SetEepromStatus"
#define METHOD_EEP_SYNC_TEMPORARY                       "E2pSyncSaveTemporary"

/* Bus类的对象及属性 */
#define CLASS_BUS                                       "Bus"
#define PROPERTY_BUS_ID                                 "Id"

/* I2c类的对象及属性 */
#define CLASS_I2C                                       "I2c"
#define PROPERTY_I2C_ID                                 "Id"
#define PROPERTY_I2C_USE_SMBUS                          "UseSmbus"

#define CLASS_HISPORT_I2C                               "HisportI2c"
/* Gpio类的对象及属性 */
#define CLASS_GPIO                                       "Gpio"

/* Lcd类的对象及属性 */
#define CLASS_LCD                       "Lcd"
#define OBJECT_LCD                      "Lcd"
#define PROPERTY_LCD_PRESENCE         "Presence"        /* 在位状态 */
#define PROPERTY_LCD_HEALTH         "Health"        /* 健康状态 */
#define PROPERTY_LCD_CLEAR          "Clear"         /* CPLD清除 */
#define PROPERTY_LCD_STATUS      "Status"         /* CPLD主从切换位*/
#define PROPERTY_VGA_CLEAR          "VGASwitchClear"         /* CPLD清除 */
#define PROPERTY_VGA_STATUS      "VGASwitchStatus"         /* CPLD主从切换位*/
#define PROPERTY_LCD_VERSION     "LcdVer"         /* 版本信息*/
#define PROPERTY_LCD_NAME     "Name"         /* 板名*/
#define METHOD_LCD_SWICTH            "SwitchLcd"          /* LCD主从切换 */
#define METHOD_LCD_STARTSTOP            "StartStopLcd"          /* LCD UART使能 */
/* BEGIN: Added by c00192989, 2016-05-20, PN: DTS2016042401307 */
#define PROPERTY_LCD_UNITNUM      "LcdUnitNum"         /* CPLD位置号*/
/* END: Added by c00192989, 2016-05-20, PN: DTS2016042401307 */

/* WARNING类的对象及属性 */
#define CLASS_WARNING                       "Warning"               /* 告警状态类 */
#define OBJECT_WARNING                      "Warning"               /* 告警状态对象 */
#define PROPERTY_WARNING_NORMAL_NUM         "NormalAlarmNum"        /* NormalAlarm数量 */
#define PROPERTY_WARNING_MINOR_NUM          "MinorAlarmNum"         /* MinorAlarm数量 */
#define PROPERTY_WARNING_MAJOR_NUM          "MajorAlarmNum"         /* MajorAlarm数量 */
#define PROPERTY_WARNING_CRITICAL_NUM       "CriticalAlarmNum"      /* CriticalAlarm数量 */
#define PROPERTY_WARNING_HEALTH_SCORE       "HealthScore"           /* 系统的健康分数 */
#define PROPERTY_WARNING_HEALTH_STATUS      "HealthStatus"          /* 系统的健康状态 */
/* Start by h00272616 AR-0001000682 20151208 */
#define PROPERTY_WARNING_IMMEDIATELY_REPORT "ImmediatelyReport"
#define METHOD_WARNING_SET_IMMEDIATELY_REPORT "SetImmediatelyReport"
/* End by h00272616 AR-0001000682 20151208 */
#define PROPERTY_WARNING_UPDATE_TIMESTAMP   "AlarmUpdateTimestamp"
#define METHOD_WARNING_GET_EVENT            "GetEventItem"          /* 获取传感器事件 */
#define METHOD_GET_EVETN_DESC_INFO          "GetEventDescInfo"      /*获取事件的详细信息*/

/* BEGIN: Modified by h00422363, 2018/7/5 11:44:41   问题单号:DTS2018070400473 */
#define PROPERTY_PCIESLOT_FAULT_STATE "PcieSlotAlarmState"
#define METHOD_SETPCIESLOT_FAULTSTATE "SetPcieSlotAlarmState"
/* END:   Modified by h00422363, 2018/7/5 11:44:44 */

/* ENTITY类的对象及属性 */
#define CLASS_ENTITY                        "Entity"                /* ENTITY实体类 */
#define PROPERTY_ENTITY_ID                  "Id"                    /* ENTITY ID */
#define PROPERTY_ENTITY_INSTANCE            "Instance"              /* ENTITY Instance */
#define PROPERTY_ENTITY_ANCHOR_SLOT         "AnchorSlot"            /* ENTITY的槽位号 */
#define PROPERTY_ENTITY_TYPE                "EntityType"            /* 实体类型 */
#define PROPERTY_ENTITY_NAME                "EntityName"            /* 实体名称 */
#define PROPERTY_ENTITY_ENABLE_STATE        "EnableState"           /* 实体使能状态 */
#define PROPERTY_ENTITY_POWER_STATE         "PowerState"            /* 实体的上下电状态 */
#define PROPERTY_ENTITY_PRESENT_STATE       "PresentState"          /* 实体的在位状态 */
#define PROPERTY_ENTITY_HEALTH              "Health"                /* ENTITY健康状态 */
#define PROPERTY_ENTITY_MINOR_SCORE         "MinorScore"            /* MINOR状态扣的分数 */
#define PROPERTY_ENTITY_MAJOR_SCORE         "MajorScore"            /* MAJOR状态扣的分数 */
#define PROPERTY_ENTITY_CRITICAL_SCORE      "CriticalScore"         /* CRITICAL状态扣的分数 */
#define PROPERTY_ENTITY_CURRENT_SCORE       "CurrentScore"          /* 当前实际扣的分数 */
#define METHOD_ENTITY_GET_EVENT             "GetEventItem"          /* 获取实体的传感器事件 */

/* EntityPcie的对象及属性 */
#define OBJECT_ENTITYPCIE                        "EntityPcie"                /* EntityPcie实体对象 */


/* SdrDev类的对象及属性 */
#define CLASS_SDR_DEV                       "SdrDev"                /* SdrDev类 */
#define OBJECT_SDR_DEV                      "SdrDev"                /* SdrDev对象 */
#define PROPERTY_SDR_NUM                    "SdrNum"                /* SDR的数量 */
#define PROPERTY_SDR_TYPE12_RECID           "SdrType12RecId"        /* Type12的SDR的Record Id */
#define PROPERTY_SDR_VERSION_CHANGE         "VersionChange"         /* SDR版本是否发生变化 */
#define PROPERTY_SDR_SENSOR_ERROR_MIX       "SensorErrorMix"        /* 传感器异常信息组合属性 */
#define PROPERTY_SDR_SENSOR_STATUS          "SensorStatus"          /* 传感器访问状态 */
#define PROPERTY_SDR_SENSOR_ERR_INDEX       "SensorErrIndex"        /* 异常传感器编号 */
/** BEGIN: Added by l00272597, 2016/9/7   PN:DTS2016090704068 */
#define PROPERTY_SDR_DYNAMIC_SENSOR_NUM_BASE   "DynamicSensorNumBase"     /* 动态分配传感器号的起始传感器号 */
#define METHOD_SDR_SET_DYNAMIC_SENSOR_NUM_BASE "SetDynamicSensorNumBase"  /* 设置动态分配传感器号的起始传感器号 */
/** END:   Added by l00272597, 2016/9/7 */
/* BEGIN: Modified by jwx372839, 2018/6/19 17:53:31   问题单号:DTS2018061602038 */
#define METHOD_SDR_SET_SDR_VER_CHANGE_FLAG "SetSdrVerChangeFlag"  /* 设置SdrDev数量变化标志 */
/* END:   Modified by jwx372839, 2018/6/19 17:53:33 */
/* LedTubeData类的对象及属性 */
#define CLASS_LED_DISP_CONTROL              "LedDispControl"
#define OBJECT_LED_DISP_CONTROL             "led_disp_ctrl"         /* LED数码告警显示对象*/
#define PROPERTY_LED_TUBE_SUPPORT           "LedTbueSupport"        /* LED数码管功能是否支持 */
#define PROPERTY_LED_TUBE_SHOW_DATA1        "LedShowData1"          /* LED数码管的最高位数据 */
#define PROPERTY_LED_TUBE_SHOW_DATA2        "LedShowData2"          /* LED数码管的中间位数据 */
#define PROPERTY_LED_TUBE_SHOW_DATA3        "LedShowData3"          /* LED数码管的最低位数据 */
#define METHOD_LED_TUBE_SET_RAW_DATA        "SetRawData"

#define CLASS_LED_DISP_ELMT                 "LedDispElmt"
#define CLASS_LED_DISP_ELMT_S               "LedDispElmt_S"
#define CLASS_LED_DISP_ELMT_RP              "LedDispElmt_RP"
#define PROPERTY_SENSOR_OBJECT              "SensorObject"
#define PROPERTY_FMT                        "Fmt"
#define PROPERTY_NUM                        "Num"

/* 离散类传感器的对象及属性 */
#define CLASS_DISCERETE_SENSOR                  "DiscreteSensor"        /* 门限类传感器 */
#define PROPERTY_DIS_SENSOR_INNER_NAME          "InnerName"
#define PROPERTY_DIS_SENSOR_OWNER_ID            "OwnerId"
#define PROPERTY_DIS_SENSOR_OWRN_LUN            "OwnerLun"
#define PROPERTY_DIS_SENSOR_NUMBER              "Number"
#define PROPERTY_DIS_SENSOR_ENTITY_ID           "EntityId"
#define PROPERTY_DIS_SENSOR_ENTITY_INSTANCE     "EntityInstance"
#define PROPERTY_DIS_SENSOR_INITIALIZATION      "Initialization"
#define PROPERTY_DIS_SENSOR_CAPABILITIES        "Capabilities"
#define PROPERTY_DIS_SENSOR_SENSOR_TYPE         "SensorType"
#define PROPERTY_DIS_SENSOR_EVENT_READ_TYPE     "EventReadingType"
#define PROPERTY_DIS_SENSOR_ASSERT_MASK         "AssertionMask"
#define PROPERTY_DIS_SENSOR_DEASSERT_MASK       "DeassertionMask"
#define PROPERTY_DIS_SENSOR_READING_MASK        "DiscreteReadingMask"
#define PROPERTY_DIS_SENSOR_SENSOR_UNIT         "SensorUnit"
#define PROPERTY_DIS_SENSOR_BASE_UNIT           "BaseUnit"
#define PROPERTY_DIS_SENSOR_MODIFIER_UNIT       "ModifierUnit"
#define PROPERTY_DIS_SENSOR_RECORD_SHARING      "RecordSharing"
#define PROPERTY_DIS_SENSOR_POSITIVE_HYSTERESIS "PositiveHysteresis"
#define PROPERTY_DIS_SENSOR_NEGATIVE_HYSTERESIS "NegativeHysteresis"
#define PROPERTY_DIS_SENSOR_TYPE_LEN_CODE       "TypeLengthCode"
#define PROPERTY_DIS_SENSOR_SENSOR_NAME         "SensorName"
#define PROPERTY_DIS_SENSOR_READING_RAW         "ReadingRaw"
#define PROPERTY_DIS_SENSOR_DISCRETE_TYPE       "DiscreteType"
#define PROPERTY_DIS_SENSOR_HEALTH_STATUS       "Health"
#define PROPERTY_DIS_SENSOR_READING_STATUS      "ReadingStatus"
#define PROPERTY_DIS_SENSOR_ASSERT_STATUS       "AssertStatus"
#define METHOD_DIS_SENSOR_GET_EVENT             "GetEventItem"          /* 获取传感器事件 */
#define METHOD_DIS_SENSOR_TEST                  "Test"                  /* 传感器测试 */
#define METHOD_DIS_SENSOR_STATE                 "State"                 /* 传感器使能 */

/* Start by w00365865 AR-0000256850 20160817 */
#define CLASS_PCIESSDCARD_NAME                 "PCIeSSDCard"
#define PROPERTY_PCIESSDCARD_SN                   "SerialNumber"
#define PROPERTY_PCIESSDCARD_MN                   "ModelNumber"
#define PROPERTY_PCIESSDCARD_FWVERSION            "FirmwareVersion"
#define PROPERTY_PCIESSDCARD_STORAGE_LOGIC        "StorageLoc"
#define PROPERTY_PCIESSDCARD_NAME                 "Name"
#define PROPERTY_PCIESSDCARD_SLOT                 "Slot"
#define PROPERTY_PCIESSDCARD_PCBID                "PcbId"
#define PROPERTY_PCIESSDCARD_PCBVER               "PcbVer"
#define PROPERTY_PCIESSDCARD_DESC                 "Desc"
#define PROPERTY_PCIESSDCARD_MF                   "Manufacturer"
#define PROPERTY_PCIESSDCARD_BOARDNAME            "BoardName"
#define PROPERTY_PCIESSDCARD_HEALTH               "Health"
/* End by w00365865 AR-0000256850 20160817 */

/* 门限类传感器的对象及属性 */
#define CLASS_THRESHOLD_SENSOR                  "ThresholdSensor"       /* 门限类传感器 */
#define PROPERTY_THR_SENSOR_INNER_NAME          "InnerName"
#define PROPERTY_THR_SENSOR_OWNER_ID            "OwnerId"
#define PROPERTY_THR_SENSOR_OWRN_LUN            "OwnerLun"
#define PROPERTY_THR_SENSOR_NUMBER              "Number"
#define PROPERTY_THR_SENSOR_ENTITY_ID           "EntityId"
#define PROPERTY_THR_SENSOR_ENTITY_INSTANCE     "EntityInstance"
#define PROPERTY_THR_SENSOR_INITIALIZATION      "Initialization"
#define PROPERTY_THR_SENSOR_CAPABILITIES        "Capabilities"
#define PROPERTY_THR_SENSOR_SENSOR_TYPE         "SensorType"
#define PROPERTY_THR_SENSOR_SENSOR_TYPESTR      "SensorTypeStr"
#define PROPERTY_THR_SENSOR_EVENT_READ_TYPE     "EventReadingType"
#define PROPERTY_THR_SENSOR_ASSERT_MASK         "AssertionMask"
#define PROPERTY_THR_SENSOR_DEASSERT_MASK       "DeassertionMask"
#define PROPERTY_THR_SENSOR_READING_MASK        "DiscreteReadingMask"
#define PROPERTY_THR_SENSOR_SENSOR_UNIT         "SensorUnit"
#define PROPERTY_THR_SENSOR_BASE_UNIT           "BaseUnit"
#define PROPERTY_THR_SENSOR_MODIFIER_UNIT       "ModifierUnit"
#define PROPERTY_THR_SENSOR_FUNCTION            "Function"
#define PROPERTY_THR_SENSOR_M_PARA              "M"
#define PROPERTY_THR_SENSOR_TOLERANCE           "Tolerance"
#define PROPERTY_THR_SENSOR_B_PARA              "B"
#define PROPERTY_THR_SENSOR_ACCERACY            "Accuracy"
#define PROPERTY_THR_SENSOR_ACCURACY_EXP        "AccuracyExp"
#define PROPERTY_THR_SENSOR_RB_EXP              "RBExp"
#define PROPERTY_THR_SENSOR_ANALOG_FLAGS        "AnalogFlags"
#define PROPERTY_THR_SENSOR_NOMINAL_READING     "NominalReading"
#define PROPERTY_THR_SENSOR_NORMAL_MAX          "NormalMax"
#define PROPERTY_THR_SENSOR_NORMAL_MIN          "NormalMin"
#define PROPERTY_THR_SENSOR_MAX_READING         "MaxReading"
#define PROPERTY_THR_SENSOR_MIN_READING         "MinReading"
#define PROPERTY_THR_SENSOR_UPPER_NONRECOVERABLE     "UpperNonrecoverable"
#define PROPERTY_THR_SENSOR_UPPER_CRITICAL           "UpperCritical"
#define PROPERTY_THR_SENSOR_UPPER_NONCRITICAL        "UpperNoncritical"
#define PROPERTY_THR_SENSOR_LOWER_NONRECOVERABLE     "LowerNonrecoverable"
#define PROPERTY_THR_SENSOR_LOWER_CRITICAL           "LowerCritical"
#define PROPERTY_THR_SENSOR_LOWER_NONCRITICAL        "LowerNoncritical"
#define PROPERTY_THR_SENSOR_POSITIVE_HYSTERESIS "PositiveHysteresis"
#define PROPERTY_THR_SENSOR_NEGATIVE_HYSTERESIS "NegativeHysteresis"
#define PROPERTY_THR_SENSOR_TYPE_LEN_CODE       "TypeLengthCode"
#define PROPERTY_THR_SENSOR_SENSOR_NAME         "SensorName"
#define PROPERTY_THR_SENSOR_UNIT_STR            "SensorUnitStr"
#define PROPERTY_THR_SENSOR_READING_RAW         "ReadingRaw"
#define PROPERTY_THR_SENSOR_READING_CONVERT     "ReaddingConvert"
#define PROPERTY_THR_SENSOR_UNR_CONVERT         "UpperNonrecoverableConvert"
#define PROPERTY_THR_SENSOR_UC_CONVERT          "UpperCriticalConvert"
#define PROPERTY_THR_SENSOR_UNC_CONVERT         "UpperNoncriticalConvert"
#define PROPERTY_THR_SENSOR_LNR_CONVERT         "LowerNonrecoverableConvert"
#define PROPERTY_THR_SENSOR_LC_CONVERT          "LowerCriticalConvert"
#define PROPERTY_THR_SENSOR_LNC_CONVERT         "LowerNoncriticalConvert"
#define PROPERTY_THR_SENSOR_P_HYS_CONVERT       "PositiveHysteresisConvert"
#define PROPERTY_THR_SENSOR_N_HYS_CONVERT       "NegativeHysteresisConvert"
#define PROPERTY_THR_SENSOR_HEALTH_STATUS       "Health"
#define PROPERTY_THS_SENSOR_READING_STATUS      "ReadingStatus"
#define PROPERTY_THR_SENSOR_ASSERT_STATUS       "AssertStatus"
#define METHOD_THS_SENSOR_GET_EVENT             "GetEventItem"          /* 获取传感器事件 */
#define METHOD_THS_SENSOR_TEST                  "Test"                  /* 传感器测试 */
/* BEGIN: Added by zengrenchang(z00296903), 2016/11/23   问题单号:DTS2016112000496*/
#define METHOD_SET_THS_SENSOR_THRESHOLD         "SetSensorThreshold"    /* 设置传感器的门限值 */
/* END:   Added by zengrenchang(z00296903), 2016/11/23 */
/* BEGIN: Modified by wangpenghui wwx382232, 2018/4/16 14:10:22   问题单号:DTS2018041600753 */
#define METHOD_SET_THS_SENSOR_THRESHOLD_PARAM           "SetThresholdParam" /* 设置传感器的门限值 */
/* END:   Modified by wangpenghui wwx382232, 2018/4/16 14:10:23 */

#define CLASS_STRING_FORMATOR                        "StringFormator"
#define PROPERTY_STRING_FMTOR_VALUE                  "Value"
#define PROPERTY_STRING_FMTOR_FMT                    "Format"
#define PROPERTY_STRING_FMTOR_REF_OBJ_ARRAY          "RefObjArray"
#define PROPERTY_STRING_FMTOR_REF_PROP_ARRAY         "RefPropertyArray"

/* DEA SDR类的对象及属性 */
#define CLASS_DEA_SDR                                "DeaSdr"                /* DEA-SDR */
#define PROPERTY_DEA_SDR_CONTAINER_ENTITY_ID         "ContainerEntityId"
#define PROPERTY_DEA_SDR_CONTAINER_ENTITY_INSTANCE   "ContainerEntityInstance"
#define PROPERTY_DEA_SDR_CONTAINER_ENTITY_ADDRESS    "ContainerEntityAddress"
#define PROPERTY_DEA_SDR_CONTAINER_ENTITY_CHANNEL    "ContainerEntityChannel"
#define PROPERTY_DEA_SDR_FLAGS                       "Flags"
#define PROPERTY_DEA_SDR_CONTAINED_ENTITY1_ADDRESS   "ContainedEntity1Address"
#define PROPERTY_DEA_SDR_CONTAINED_ENTITY1_CHANNEL   "ContainedEntity1Channel"
#define PROPERTY_DEA_SDR_CONTAINED_ENTITY1_ID        "ContainedEntity1Id"
#define PROPERTY_DEA_SDR_CONTAINED_ENTITY1_INSTANCE  "ContainedEntity1Instance"
#define PROPERTY_DEA_SDR_CONTAINED_ENTITY2_ADDRESS   "ContainedEntity2Address"
#define PROPERTY_DEA_SDR_CONTAINED_ENTITY2_CHANNEL   "ContainedEntity2Channel"
#define PROPERTY_DEA_SDR_CONTAINED_ENTITY2_ID        "ContainedEntity2Id"
#define PROPERTY_DEA_SDR_CONTAINED_ENTITY2_INSTANCE  "ContainedEntity2Instance"
#define PROPERTY_DEA_SDR_CONTAINED_ENTITY3_ADDRESS   "ContainedEntity3Address"
#define PROPERTY_DEA_SDR_CONTAINED_ENTITY3_CHANNEL   "ContainedEntity3Channel"
#define PROPERTY_DEA_SDR_CONTAINED_ENTITY3_ID        "ContainedEntity3Id"
#define PROPERTY_DEA_SDR_CONTAINED_ENTITY3_INSTANCE  "ContainedEntity3Instance"
#define PROPERTY_DEA_SDR_CONTAINED_ENTITY4_ADDRESS   "ContainedEntity4Address"
#define PROPERTY_DEA_SDR_CONTAINED_ENTITY4_CHANNEL   "ContainedEntity4Channel"
#define PROPERTY_DEA_SDR_CONTAINED_ENTITY4_ID        "ContainedEntity4Id"
#define PROPERTY_DEA_SDR_CONTAINED_ENTITY4_INSTANCE  "ContainedEntity4Instance"

/* MCDL SDR类的对象及属性 */
#define CLASS_MCDL_SDR                          "McdlSdr"               /* MCDL-SDR */
#define PROPERTY_MCDL_SDR_SLAVE_ADDR            "DeviceSlaveAddr"
#define PROPERTY_MCDL_SDR_CHANNEL_NUM           "ChannelNum"
#define PROPERTY_MCDL_SDR_GLOBAL_INIT           "PowerStateGlobalInit"
#define PROPERTY_MCDL_SDR_DEVICE_CAP            "DeviceCap"
#define PROPERTY_MCDL_SDR_ENTITY_ID             "EntityId"
#define PROPERTY_MCDL_SDR_ENTITY_INSTANCE       "EntityInstance"
#define PROPERTY_MCDL_SDR_TYPE_LEN              "DeviceIdTypeLen"
#define PROPERTY_MCDL_SDR_ID_STRING             "DeviceIdString"

/*OEM SDR类的对象及属性 */
#define CLASS_OEM_SDR                            "OemSdr"
#define PROPERTY_OEM_SDR_MFR_ID                  "ManufacturerID"
#define PROPERTY_OEM_SDR_OEM_DATA                "OemData"


/* 离散事件侦听类的对象及属性 */
#define CLASS_DISCERETE_EVENT                   "DiscreteEvent"         /* 离散事件侦听类 */
#define PROPERTY_DIS_EVENT_PRO_REF              "PropertyRef"
#define PROPERTY_DIS_EVENT_LISTEN_TYPE          "ListenType"
#define PROPERTY_DIS_EVENT_DATA0_REF            "Data0Ref"
#define PROPERTY_DIS_EVENT_DATA1_REF            "Data1Ref"
#define PROPERTY_DIS_EVENT_DATA2_REF            "Data2Ref"
#define PROPERTY_DIS_EVENT_DIR_REF              "DirectionRef"
#define PROPERTY_DIS_EVENT_CONVERSION           "conversion"
#define PROPERTY_DIS_EVENT_SENSOR_NAME          "SensorName"            /* 事件对应的传感器名称 */



/* EVENT DESCP类的对象及属性 */
#define OBJ_EVENT_DESCP_UNKNOW                  "UnkownEvent_0"         /* Unknown事件描述 */
#define CLASS_EVENT_DESCP                       "EventDescp"            /* 事件描述类 */
#define PROPERTY_EVENT_DESCP_SENSOR_TYPE        "SensorType"            /* 传感器类型 */
#define PROPERTY_EVENT_DESCP_EVENT_READ_TYPE    "EventReadType"         /* 事件读数类型 */
#define PROPERTY_EVENT_DESCP_DATA0              "EventData0"            /* Event Data 0 */
#define PROPERTY_EVENT_DESCP_DATA1              "EventData1"            /* Event Data 1 */
#define PROPERTY_EVENT_DESCP_DATA2              "EventData2"            /* Event Data 2 */
#define PROPERTY_EVENT_DESCP_VALID_FLAG         "DescValidFlag"         /* 标识Data1/2不同时事件是否不同 */
#define PROPERTY_EVENT_DESCP_ALARM_LEVEL        "AlarmLevel"            /* 事件的告警等级 */
#define PROPERTY_EVENT_DESCP_EVENT_CODE         "EventCode"             /* 对应的事件码 */
#define PROPERTY_EVENT_DESCP_SENSOR_DESC        "SensorDesc"            /* 对应的传感器类型描述 */
#define PROPERTY_EVENT_DESCP_EVENT_DESC         "EventDesc"             /* 对应的事件详细描述 */
#define METHOD_EVENT_DESCP_GET_EVENT            "GetEventItem"          /* 获取对应事件详细描述的方法 */

/* SEL FILTER类的对象及属性 */
#define CLASS_SEL_FILTER                        "SelFilter"             /* SEL事件过滤类 */
#define PROPERTY_SFT_SENSOR_TYPE                "SensorType"
#define PROPERTY_SFT_EVENT_READ_TYPE            "EventReadingType"
#define PROPERTY_SFT_MASK1                      "Mask1"
#define PROPERTY_SFT_MASK2                      "Mask2"
#define PROPERTY_SFT_MASK3                      "Mask3"

/* PEF FILTER类的对象及属性 */
#define CLASS_PEF_FILTER                        "PefFilter"             /* PEF事件过滤类 */
#define PROPERTY_EFT_CONFIGURATION              "Configuration"         /* PEF表项的全局配置 */
#define PROPERTY_EFT_ACTION                     "Action"                /* PEF表项的执行动作 */
#define PROPERTY_EFT_POLICY_NUM                 "PolicyNumber"
#define PROPERTY_EFT_SEVERITY                   "Severity"
#define PROPERTY_EFT_GENERATOR_ID1              "GeneratorId1"
#define PROPERTY_EFT_GENERATOR_ID2              "GeneratorId2"
#define PROPERTY_EFT_SENSOR_TYPE                "SensorType"
#define PROPERTY_EFT_SENSOR_NUM                 "SensorNumber"
#define PROPERTY_EFT_EVETN_TRIGGER              "EventTrigger"
#define PROPERTY_EFT_OFFSET_MASK                "OffsetMask"
#define PROPERTY_EFT_EVENT1_ANDMASK             "Event1AndMask"
#define PROPERTY_EFT_EVENT1_COMPARE1            "Event1Compare1"
#define PROPERTY_EFT_EVENT1_COMPARE2            "Event1Compare2"
#define PROPERTY_EFT_EVENT2_ANDMASK             "Event2AndMask"
#define PROPERTY_EFT_EVENT2_COMPARE1            "Event2Compare1"
#define PROPERTY_EFT_EVENT2_COMPARE2            "Event2Compare2"
#define PROPERTY_EFT_EVENT3_ANDMASK             "Event3AndMask"
#define PROPERTY_EFT_EVENT3_COMPARE1            "Event3Compare1"
#define PROPERTY_EFT_EVENT3_COMPARE2            "Event3Compare2"

/* ALERT FILTER类的对象及属性 */
#define CLASS_ALERT_FILTER                      "AlertFilter"           /* 告警信息过滤类 */
#define PROPERTY_AFT_POLITY_NUM                 "PolicyNum"             /* Policy编号 */
#define PROPERTY_AFT_DESTINATION                "Destination"           /* 告警的目的地 */
#define PROPERTY_AFT_ALERT_STRING               "AlertString"           /* Alert String */

/* ALERT STRING类的对象及属性 */
#define CLASS_ALERT_STRING                      "AlertString"           /* AlertString类 */
#define PROPERTY_AST_FILTER_NUM                 "FilterNumber"          /* FilterNumber编号 */
#define PROPERTY_AST_STRING_SET                 "StringSet"             /* StringSet */
#define PROPERTY_AST_ALT_STRING                 "AltString"             /* Alert String */

/* TRAP CONFIG类的对象及属性 */
#define CLASS_TRAP_CONFIG                       "TrapConfig"            /* TRAP全局配置类 */
#define OBJECT_TRAP_CONFIG                      "TrapConfig"            /* TRAP全局配置类 */
#define PROPERTY_TRAP_ENABLE                    "TrapEnable"            /* TRAP全局使能 */
#define PROPERTY_TRAP_SEND_SEVERITY             "SendSeverity"          /* TRAP告警发送等级 */
#define PROPERTY_TRAP_VERSION                   "TrapVersion"           /* TRAP使用的版本 */
#define PROPERTY_TRAP_MODE                      "TrapMode"              /* TRAP上报的模式 */
#define PROPERTY_CUSTOMIZE_TRAP_MODE            "CustomizeTrapMode"     /* 定制TRAP上报信息 */
#define PROPERTY_TRAP_IDENTITY                  "TrapIdentity"          /* TRAP上报的server identity */
#define PROPERTY_TRAPV3_USERID                  "Trapv3Userid"          /* TRAPV3用户Id*/
#define PROPERTY_TRAP_COMMUNITY_NAME            "CommunityName"         /* TRAP的团体名 */
#define PROPERTY_TRAP_COMMUNITY_WORKKEY         "CommunityWorkKey"      /* TRAP的团体名工作密钥 */
#define PROPERTY_TRAP_COMMUNITY_COMPONENT       "CommunityComponent"    /* TRAP的团体名Component */
#define PROPERTY_TRAP_COMMUNITY_MASTERKEY_ID    "CommunityMasterKeyId"  /* TRAP的团体名主密钥Id */

#define METHOD_TRAP_SET_ENABLE                  "SetTrapEnable"         /* TRAP使能设置方法 */
#define METHOD_TRAP_SET_VERSION                 "SetVersion"            /* TRAP版本设置方法 */
#define METHOD_TRAP_SET_MODE                    "SetTrapMode"           /* TRAP模式设置方法 */
#define METHOD_TRAP_SET_IDENTITY                "SetTrapIdentity"           /* TRAP模式设置方法 */
#define METHOD_TRAP_SET_TRAPV3_USERID           "SetTrapv3Userid"       /* TRAPV3用户Id设置方法 */
#define METHOD_TRAP_SET_TRAPV3_AUTHPROTOCOL     "SetTrapv3AuthProtocol" /* TRAPV3认证协议设置方法 */
#define METHOD_TRAP_SET_TRAPV3_PRVIPROTOCOL     "SetTrapv3PrivProtocol" /* TRAPV3加密协议设置方法 */
#define METHOD_TRAP_SET_SEND_SEVERITY           "SetSendSeverity"       /* TRAP发送等级设置方法 */
#define METHOD_TRAP_SET_COMMUNITY_NAME          "SetCommunityName"      /* TRAP团体名称设置方法 */
#define METHOD_TRAP_GET_COMMUNITY_NAME          "GetCommunityName"      /* TRAP团体名称获取方法 */
#define METHOD_TRAP_SEND_TEST                   "SendTest"              /* TRAP发送测试功能 */
#define METHOD_TRAP_UPDATE_CIPHERTEXT           "UpdateTrapCiphertext"


/* TRAP ITEM类的对象及属性 */
#define CLASS_TRAP_ITEM_CONFIG                  "TrapItemCfg"           /* TRAP-ITEM配置类 */
#define PROPERTY_TRAP_ITEM_INDEX_NUM            "IndexNum"              /* TRAP-ITEM配置项序列号 */
#define PROPERTY_TRAP_ITEM_ENABLE               "ItemEnable"            /* TRAP-ITEM配置项是否使能 */
/* BEGIN: Added by qinjiaxiang, 2017/12/28   PN:AR.SR.SFEA02119720.003.001  */
#define PROPERTY_TRAP_BOB_ENABLE                "BobEnable"             /* TRAP-ITEM配置带内是否使能 */
/* END:   Added by qinjiaxiang, 2017/12/28 */
#define PROPERTY_TRAP_ITEM_DEST_IPADDR          "DestIpAddr"            /* TRAP-ITEM配置项目的IP地址 */
#define PROPERTY_TRAP_ITEM_DEST_IPPORT          "DestIpPort"            /* TRAP-ITEM配置项目的IP端口 */
/* BEGIN: Added by qinjiaxiang, 2018/1/3   PN:AR.SR.SFEA02119720.003.001  */
#define PROPERTY_TRAP_BOB_BMA_VETH_IP_ADDR       "BmaVethIpAddr"          /* TRAP-ITEM通过bob上报报文时候对应的BMA的veth网口ip地址*/
#define PROPERTY_TRAP_BOB_BMA_VETH_IP_PORT       "BmaVethIpPort"          /* TRAP-ITEM通过bob上报报文时候对应的BMA的veth网口端口地址*/
/* END:   Added by qinjiaxiang, 2018/1/3 */
#define PROPERTY_TRAP_ITEM_SEPARATOR            "Separator"             /* TRAP-ITEM配置项报文分隔符 */
#define PROPERTY_TRAP_ITEM_SHOW_KEYWORD         "ShowKeyWord"           /* TRAP-ITEM配置项报文是否带关键字 */
#define PROPERTY_TRAP_ITEM_TIME                 "Time"                  /* TRAP-ITEM配置项报文是否带时间信息 */
#define PROPERTY_TRAP_ITEM_SEVERITY             "Severity"              /* TRAP-ITEM配置项报文是否带告警等级信息 */
#define PROPERTY_TRAP_ITEM_SENSOR_NAME          "SensorName"            /* TRAP-ITEM配置项报文是否带传感器信息 */
#define PROPERTY_TRAP_ITEM_EVENT_CODE           "EventCode"             /* TRAP-ITEM配置项报文是否带事件码信息 */
#define PROPERTY_TRAP_ITEM_EVENT_DESC           "EventDesc"             /* TRAP-ITEM配置项报文是否带事件描述信息 */
#define METHOD_TRAP_ITEM_SET_ENABLE             "SetItemEnable"         /* TRAP-ITEM使能设置方法 */
#define METHOD_TRAP_ITEM_SET_BOB_ENABLE         "SetBobEnable"          /* TRAP-ITEM-BOB使能设置方法 */
#define METHOD_TRAP_ITEM_SET_IP_ADDR            "SetDestIpAddr"         /* TRAP-ITEM目的IP地址设置方法 */
#define METHOD_TRAP_ITEM_SET_IP_PORT            "SetDestIpPort"         /* TRAP-ITEM目的端口设置方法 */
#define METHOD_TRAP_ITEM_SET_SEPARATOR          "SetSeparator"          /* TRAP-ITEM报文分隔符设置方法 */
#define METHOD_TRAP_ITEM_SET_SHOW_KEYWORD       "SetShowKeyWord"        /* TRAP-ITEM报文是否带关键字设置方法 */
#define METHOD_TRAP_ITEM_SET_TIME_SEND          "SetTimeSend"           /* TRAP-ITEM报文是否带时间信息设置方法 */
#define METHOD_TRAP_ITEM_SET_SEVERITY_SEND      "SetSeveritySend"       /* TRAP-ITEM报文是否带告警等级信息设置方法 */
#define METHOD_TRAP_ITEM_SET_SENSOR_NAME_SEND   "SetSensorNameSend"     /* TRAP-ITEM报文是否带传感器信息设置方法 */
#define METHOD_TRAP_ITEM_SET_EVENT_CODE_SEND    "SetEventCodeSend"      /* TRAP-ITEM报文是否带事件码信息设置方法 */
#define METHOD_TRAP_ITEM_SET_EVENT_DESC_SEND    "SetEventDescSend"      /* TRAP-ITEM报文是否带事件描述信息设置方法 */
#define METHOD_TRAP_ITEM_SET_BMA_VETH_IP        "SetBmaVethIp"          /* TRAP-ITEM设置带内转发时BMA veth网口IP地址 */
#define METHOD_TRAP_ITEM_SET_BMA_VETH_PORT      "SetBmaVethPort"        /* TRAP-ITEM设置带内转发时BMA veth网口PORT*/
#define METHOD_TRAP_ITEM_TRAPTEST                "Traptest"


/* SMTP CONFIG类的对象及属性 */
#define CLASS_SMTP_CONFIG                       "SmtpConfig"            /* SMTP全局配置类 */
#define OBJECT_SMTP_CONFIG                      "SmtpConfig"            /* SMTP全局配置对象 */
#define PROPERTY_SMTP_ENABLE                    "SmtpEnable"            /* SMTP全局使能 */
#define PROPERTY_SMTP_SEND_SEVERITY             "SendSeverity"          /* SMTP告警发送等级 */
#define PROPERTY_SMTP_TLS_SEND_MODE             "TlsSendMode"           /* SMTP是否支持加密模式 */
#define PROPERTY_SMTP_ANONYMOUS_MODE            "AnonymousMode"         /* SMTP是否支持匿名模式 */
#define PROPERTY_SMTP_LOGIN_NAME                "LoginName"             /* SMTP登录名称 */
#define PROPERTY_SMTP_LOGIN_PASSWD              "LoginPasswd"           /* SMTP登录密码 */
#define PROPERTY_SMTP_ROOT_START_TIME           "RootStartTime"
#define PROPERTY_SMTP_ROOT_EXPIRE_TIME          "RootExpirTime"
#define PROPERTY_SMTP_SERVER_NAME               "SmtpServer"            /* SMTP服务器名称 */
#define PROPERTY_SMTP_SERVER_PORT               "ServerPort"            /* SMTP服务器端口 */
#define PROPERTY_SMTP_SENDER_NAME               "SenderName"            /* SMTP发件人名字 */
#define PROPERTY_SMTP_TEMPLET_TOPIC             "TempletTopic"          /* SMTP邮件主题 */
#define PROPERTY_SMTP_TEMPLET_IPADDR            "TempletIpaddr"         /* SMTP邮件主题是否带IP地址 */
#define PROPERTY_SMTP_TEMPLET_BOARD_SN          "TempletBoardSn"        /* SMTP邮件主题是否带单板序列号 */
#define PROPERTY_SMTP_TEMPLET_ASSET             "TempletAsset"          /* SMTP邮件主题是否带资产标签 */
#define PROPERTY_SMTP_TEMPLET_CHASSIS_NAME              "TempletChassisName"            /* SMTP邮件主题是否带机框名 */
#define PROPERTY_SMTP_TEMPLET_CHASSIS_LOCATION          "TempletChassisLocation"        /* SMTP邮件主题是否带机框位置 */
#define PROPERTY_SMTP_TEMPLET_CHASSIS_SERIAL_NUMBER     "TempletChassisSerialNumber"    /* SMTP邮件主题是否带机框序列号 */
#define PROPERTY_SMTP_LOGIN_PASSWD_WORKKEY      "LoginPasswdWorkKey"    /* SMTP邮件密码工作密钥 */
#define PROPERTY_SMTP_LOGIN_PASSWD_COMPONENT    "LoginPasswdComponent"  /* SMTP邮件密码Component */
#define PROPERTY_SMTP_LOGIN_PASSWD_MASTERKEY_ID "LoginPasswdMasterKeyId"  /* SMTP邮件密码主密钥Id */
#define PROPERTY_SMTP_CERT_VERIFY_ENABLE        "CertVerificationEnabled"

#define METHOD_SMTP_GET_CERT_INFO               "GetCertInfo"
#define METHOD_SMTP_SET_CERT_VERIFY_ENABLE      "SetCertVerificationEnabled" /* SMTP服务器证书校验使能 */
#define METHOD_SMTP_MGNT_IMPORT_CERT            "ImportCert"            /* SMTP使能导入证书 */
#define METHOD_SMTP_SET_ENABLE                  "SetSmtpEnable"         /* SMTP使能设置方法 */
#define METHOD_SMTP_TLS_SEND_MODE               "SetTlsSendMode"        /* SMTP支持加密模式方法 */
#define METHOD_SMTP_SET_ANONYMOUS_MODE          "SetAnonymousMode"      /* SMTP匿名模式设置方法 */
#define METHOD_SMTP_SET_SEND_SEVERITY           "SetSendSeverity"       /* SMTP发送等级设置方法 */
#define METHOD_SMTP_SET_LOGIN_NAME              "SetLoginName"          /* SMTP登录用户名称设置方法 */
#define METHOD_SMTP_SET_LOGIN_PASSWD            "SetLoginPasswd"        /* SMTP登录用户密码设置方法 */
#define METHOD_SMTP_SET_SMTP_SERVER             "SetSmtpServer"         /* SMTP服务器设置方法 */
#define METHOD_SMTP_SET_SERVER_PORT             "SetServerPort"         /* SMTP发送端口设置方法 */
#define METHOD_SMTP_SET_SENDER_NAME             "SetSenderName"         /* SMTP发送人员名称设置方法 */
#define METHOD_SMTP_SET_TEMPLET_TOPIC           "SetTempletTopic"       /* SMTP邮件主题设置方法 */
#define METHOD_SMTP_SET_TEMPLET_IPADDR          "SetTempletIpaddr"      /* SMTP邮件主题是否带IP地址设置方法 */
#define METHOD_SMTP_SET_TEMPLET_BOARD_SN        "SetTempletBoardSn"     /* SMTP邮件主题是否带单板序列号设置方法 */
#define METHOD_SMTP_SET_TEMPLET_ASSET           "SetTempletAsset"       /* SMTP邮件主题是否带资产标签设置方法 */
#define METHOD_SMTP_SET_TEMPLET_CHASSIS_NAME            "SetTempletChassisName"         /* SMTP邮件主题是否带机框名设置方法 */
#define METHOD_SMTP_SET_TEMPLET_CHASSIS_LOCATION        "SetTempletChassisLocation"     /* SMTP邮件主题是否带机框位置设置方法 */
#define METHOD_SMTP_SET_TEMPLET_CHASSIS_SERIAL_NUMBER   "SetTempletChassisSerialNumber" /* SMTP邮件主题是否带机框序列号设置方法 */
#define METHOD_SMTP_SEND_TEST                   "SendTest"              /* SMTP发送测试功能 */
#define METHOD_SMTP_UPDATE_CIPHERTEXT           "UpdateSmtpCiphertext"


/* SMTP ITEM类的对象及属性 */
#define CLASS_SMTP_ITEM_CONFIG                  "SmtpItemCfg"           /* SMTP-ITEM配置类 */
#define PROPERTY_SMTP_ITEM_INDEX_NUM            "IndexNum"              /* SMTP-ITEM配置项序列号 */
#define PROPERTY_SMTP_ITEM_ENABLE               "ItemEnable"            /* SMTP-ITEM配置项是否使能 */
#define PROPERTY_SMTP_ITEM_EMAIL_NAME           "EmailName"             /* SMTP-ITEM配置项邮件名称 */
#define PROPERTY_SMTP_ITEM_EMAIL_DESC           "EmailDesc"             /* SMTP-ITEM配置项邮件描述 */
#define PROPERTY_SMTP_ITEM_SEPARATOR            "Separator"             /* SMTP-ITEM配置项报文分隔符 */
#define PROPERTY_SMTP_ITEM_SHOW_KEYWORD         "ShowKeyWord"           /* SMTP-ITEM配置项报文是否带关键字 */
#define PROPERTY_SMTP_ITEM_TIME                 "Time"                  /* SMTP-ITEM配置项报文是否带时间信息 */
#define PROPERTY_SMTP_ITEM_SEVERITY             "Severity"              /* SMTP-ITEM配置项报文是否带告警等级信息 */
#define PROPERTY_SMTP_ITEM_SENSOR_NAME          "SensorName"            /* SMTP-ITEM配置项报文是否带传感器信息 */
#define PROPERTY_SMTP_ITEM_EVENT_CODE           "EventCode"             /* SMTP-ITEM配置项报文是否带事件码信息 */
#define PROPERTY_SMTP_ITEM_EVENT_DESC           "EventDesc"             /* SMTP-ITEM配置项报文是否带事件描述信息 */
#define METHOD_SMTP_ITEM_SET_ENABLE             "SetItemEnable"         /* SMTP-ITEM使能设置方法 */
#define METHOD_SMTP_ITEM_SET_EMAIL_NAME         "SetEmailName"          /* SMTP-ITEM邮件名称设置方法 */
#define METHOD_SMTP_ITEM_SET_EMAIL_DESC         "SetEmailDesc"          /* SMTP-ITEM邮件信息描述设置方法 */
#define METHOD_SMTP_ITEM_SET_SEPARATOR          "SetSeparator"          /* SMTP-ITEM报文分隔符设置方法 */
#define METHOD_SMTP_ITEM_SET_TIME_SEND          "SetTimeSend"           /* SMTP-ITEM报文是否带时间信息设置方法 */
#define METHOD_SMTP_ITEM_SET_SEVERITY_SEND      "SetSeveritySend"       /* TRAP-ITEM报文是否带告警等级信息设置方法 */
#define METHOD_SMTP_ITEM_SET_SENSOR_NAME_SEND   "SetSensorNameSend"     /* TRAP-ITEM报文是否带传感器信息设置方法 */
#define METHOD_SMTP_ITEM_SET_EVENT_CODE_SEND    "SetEventCodeSend"      /* TRAP-ITEM报文是否带事件码信息设置方法 */
#define METHOD_SMTP_ITEM_SET_EVENT_DESC_SEND    "SetEventDescSend"      /* TRAP-ITEM报文是否带事件描述信息设置方法 */

/* SEL类的对象及属性 */
#define CLASS_SEL                           "Sel"                   /* SEL全局类 */
#define OBJECT_SEL                          CLASS_SEL
#define PROPERTY_SEL_VERSION                "Version"               /* SEL版本 */
#define PROPERTY_SEL_CURRENT_NUM            "SelCurNum"             /* 当前SEL事件的个数 */
#define PROPERTY_SEL_MAX_NUM                "SelMaxNum"             /* SEL的最大个数 */
#define PROPERTY_SEL_UNIT_SIZE              "UnitSize"              /* 单条SEL的大小 */
#define PROPERTY_SEL_ADD_TIME               "AddTime"               /* SEL最后的添加时间 */
#define PROPERTY_SEL_DEL_TIME               "DelTime"               /* SEL最后的删除时间 */
#define PROPERTY_SEL_CLEARED                "SELCleared"            /* SEL被清除 */
/* BEGIN: Modifed by c00346034, 2018/11/13, PN:AR.SR.SFEA02130924.061.001 */
#define PROPERTY_SEL_QUERY_MAX_NUM          "QuerySelMaxNum"        /* ipmitool sel list 查询的SEL的最大个数 */
/* END: Modifed by c00346034, 2018/11/13, PN:AR.SR.SFEA02130924.061.001 */
#define METHOD_SEL_SET_SEL_ENABLE           "SetSelEnable"          /* SEL使能设置方法 */
#define METHOD_SEL_CLEAR_SEL                "ClearSel"              /* SEL清除方法 */
#define METHOD_SEL_GET_EVENT_INFO           "GetEventInfo"          /* 获取SEL事件描述信息 */
#define METHOD_SEL_COLLECT_SEL              "CollectSel"            /* 获取SEL事件描述信息 */
#define METHOD_SEL_REARM_EVENTS             "RearmEvents"        /* 获取SEL事件描述信息 */
/* BEGIN: Modifed by c00346034, 2018/11/13, PN:AR.SR.SFEA02130924.061.001 */
#define METHOD_SEL_SET_QUERY_MAXNUM_VALUE   "SetQuerySelMaxNum"           /* ipmitool sel list 命令设置支持查询的SEL条数支持可配置1~2000条(2000默认) */
#define METHOD_QUERY_EVENT_LIST_BY_DATE     "QueryEventListByDate"
#define METHOD_SAVE_EVENT_INFO              "SaveEventInfo"
#define METHOD_DELETE_EVENT_LIST            "DeleteEventList"
/* END: Modifed by c00346034, 2018/11/13, PN:AR.SR.SFEA02130924.061.001 */
#define METHOD_QUERY_SEL_LOG                "QuerySelLog"
/* 环境监控类的对象及属性 */
/* 每个xxxMonitor类为Monitor的子类 */
/* 基类Monitor类 */
#define CLASS_MONTIOR                       "Monitor"
#define PROPERTY_SELFTEST_OBJECT            "SelfTestObject"
#define PROPERTY_MON_SEN_VALUE              "MonSenValue"
#define PROPERTY_DEBOUNCED_CFG              "DebouncedCfg"

/* 温度监控 */
#define CLASS_TEMP_MONTIOR                  "TempMonitor"
#define PROPERTY_TEMP_VALUE                 "TempValue"

/* BEGIN: SR-0000276594-004. Added by cwx290152, 2017/6/24 */
#define CLASS_PMBUS_TEMP_MONTIOR            "PMBusTempMonitor"
/* END: SR-0000276594-004. Added by cwx290152, 2017/6/24 */

/* 电压监控 */
#define CLASS_VOLT_MONITOR                  "VoltMonitor"        /* 使用ADC监控的电压 */
#define PROPERTY_VOLT_VALUE                 "VoltValue"             /* 电压值 */
#define PROPERTY_CONVERT_COEF               "ConvertCoef"           /* 转换系数 */

/* 最大值监控 */
#define CLASS_MAX_VALUE_MONITOR             "MaxValueMonitor"
#define PROPERTY_MON_ELEMENT                "MonElement"

/* 寄存器监控 */
#define CLASS_REG_MONITOR                   "RegMonitor"
#define PROPERTY_AUTO_CLR                   "AutoClr"
#define PROPERTY_REG_VALUE                  "RegValue"

#define CLASS_SELECTOR_MONTIOR               "ValueSelector"
#define PROPERTY_VALUE_SELECTOR_VALUE        "Value"
#define PROPERTY_VALUE_SELECTOR_SELECT_X     "SelectX"
#define PROPERTY_VALUE_SELECTOR_X            "X"
#define PROPERTY_VALUE_SELECTOR_Y            "Y"
#define PROPERTY_VALUE_SELECTOR_PERIOD       "Period"
#define METHOD_VALUE_SELECTOR_SYNC           "ValueSelectorSync"

/* SESA状态监控对象 */
#define OBJ_NAME_SESA_STATUS_MON            "sesa_status_mon"

/* 进风口温度对象 */
#define OBJECT_INLETTEMP_MONITOR            "inlet_temp_monitor"
/* 出风口温度对象 */
#define OBJECT_OUTLETTEMP_MONITOR            "outlet_temp_monitor"

/* 监控自检类，每个xxxTest为MonSelftest的子类 */
/* 基类MonSelftest */
#define CLASS_MON_SELFTEST                  "MonSelftest"
#define PROPERTY_DEV_POWER_STATUS           "DevPowerStatus"

/* 物理设备自检类 */
#define CLASS_DEV_TEST                      "DevTest"
#define PROPERTY_REF_CHIP                   "RefChip"

/* 非设备自检 */
#define CLASS_NO_DEV_TEST                   "NoDevTest"
#define PROPERTY_TEST_RESULT                "TestResult"

/* 电压读数自检 */
#define CLASS_VOLT_TEST                     "VoltTest"
#define PROPERTY_POWON_STAND_VALUE          "PowOnStandValue"       /* 业务上电时标准值 */
#define PROPERTY_POWOFF_STAND_VALUE         "PowOffStandValue"      /* 业务下电时标准值 */
#define PROPERTY_HYST_VALUE                 "HystValue"             /* 业务下电时迟滞量 */

/* 防抖策略 */
#define CLASSS_DEBOUNCED_CFG                "DebouncedConfig"

/* 不需防抖 */
#define CLASS_DBD_CFG_NONE                  "DbdCfgNone"

/* 中值滤波防抖 */
#define CLASS_DBD_CFG_MEDIAN                "DbdCfgMedian"
#define PROPERTY_MEDIAN_IS_SIGNED           "IsSigned"
#define PROPERTY_MEDIAN_HISTORY_VAL_NUM     "HistoryValNum"

/* 中值平均防抖 */
#define CLASS_DBD_CFG_MIDAVG                "DbdCfgMidAvg"
#define PROPERTY_IS_SIGNED                  "IsSigned"
#define PROPERTY_HISTORY_VAL_NUM            "HistoryValNum"

/* 持续二值防抖，当持续X次读到相同值时认为信号稳定 */
#define CLASS_DBD_CFG_CONTBIN               "DbdCfgContBin"
#define PROPERTY_CONT_NUM_H                 "ContNum_H"
#define PROPERTY_CONT_NUM_L                 "ContNum_L"

/* 持续防抖，当持续X次读到相同值时认为信号稳定 */
#define CLASS_DBD_CFG_CONT                  "DbdCfgCont"
#define PROPERTY_CONT_NUM                   "ContNum"

/* 防抖配置对象 */
#define OBJECT_DBD_CFG_CONTBIN_H5L5         "dbd_contbin_H5L5"
#define OBJECT_DBD_CFG_CONTBIN_H10L10       "dbd_contbin_H10L10"
#define OBJECT_DBD_CFG_CONT_5               "dbd_cont_5"

/* 阈值Assert */
#define CLASS_THRESHOLD_ASSERT              "ThresholdAssert"
#define PROPERTY_ASSERT                     "Assert"
#define PROPERTY_L_NON_CRIT                 "LowerNoncritical"
#define PROPERTY_NEG_HYST                   "NegativeHysteresis"
#define PROPERTY_REF_PROP                   "ReferenceProperty"

/* 环境数据记录 */
#define CLASS_ENV_RECORD                    "EnvRecord"
#define OBJECT_ENV_RECORD                   "env_record"
#define OBJECT_BOARDPOWERDROP_MNTR               "BoardPowerDropMntr"
#define METHOD_ENV_RECORD_CLEAR_HISTORY     "ClearHistoryRecord"
#define METHOD_ENV_SET_BOARDPOWERDROP_MNTR     "SetBoardPowerDropMntr"
#define METHOD_ENV_RECORD_ROLLBACK          "RollbackEnvTempRecord"

/* BEGIN: Added by ouluchun 00294050, 2015/8/6   PN:AR-0000958109*/
#define PROPERTY_FDM_FAULT_STATE         "FDMFault"
#define METHOD_SET_FDM_FAULT_STATE       "SetFDMFault"
/* END:   Added by ouluchun 00294050, 2015/8/6 */

/* BEGIN: Added by wwx390838, 2017/12/18,  PN:AR.SR.SFEA02109778.002.002, 支持CE PFA */
#define PROPERTY_PFA_EVENT      "PFAEvent"
#define METHOD_SET_PFA_EVENT            "SetPFAEvent"
/* END:   Added by wwx390838, 2017/12/18 */
/* MeInfo类的对象及属性 */
#define CLASS_ME_INFO                       "MeInfo"                /* MeInfo类 */
#define PROPERTY_MEINFO_PCH_TEMP            "PchTemp"               /* PCH温度 */
#define PROPERTY_MEINFO_UTILISE_SUPPORT     "SupportUtiliseMode"    /* 支持CUPs统计 */
#define PROPERTY_MEINFO_FORCE_RECOVERY_CAUSE      "ForceRecoveryCause"
#define PROPERTY_MEINFO_CPU_UTILISE         "CpuUtilise"            /* CPU占用率 */
#define PROPERTY_MEINFO_CPU_CUR_POWER       "CpuCurPower"           /* CPU当前功耗*/
#define PROPERTY_MEINFO_MEM_CUR_POWER       "MemCurPower"           /* 内存当前功耗*/
#define PROPERTY_MEINFO_MEM_UTILISE         "MemUtilise"            /* 内存占用率 */
#define PROPERTY_MEINFO_MEM_BAND_WIDTH_UTILISE         "MemBandWidthUtilise"            /* 总带宽内存占用率 */
#define PROPERTY_MEINFO_CPU_UTILISE_THRESHOLD     "CpuUtiliseThre"   /* CPU占用率告警阈值 */
#define PROPERTY_MEINFO_MEM_UTILISE_THRESHOLD     "MemUtiliseThre"   /* 内存占用率告警阈值 */
#define PROPERTY_MEINFO_CPU_UTILISE_STATE   "CpuUtiliseState"       /* CPU占用率告警状态 */
#define PROPERTY_MEINFO_MEM_UTILISE_STATE   "MemUtiliseState"       /* 内存占用率告警状态 */
#define PROPERTY_MEINFO_ECC_CLEAR_CYCLE     "EccClearCycle"         /* ECC扫描周期 单位：分钟 */
#define PROPERTY_MEINFO_ECC_THRESHOLD       "EccThreshold"          /* ECC门限 单位：个 */
#define PROPERTY_MEINFO_ME_STATUS           "MeStatus"              /*  记录ME状态 */
#define PROPERTY_MEINFO_RESUME_STRATEGY     "MEResumeStrategy"      /* 升级bios，ME终极异常的时候恢复策略 */
#define PROPERTY_MEINFO_PFR                 "PfrRepairMe"
/* BEGIN: Added by baoxu, AR-0000276585-015-001 2017/8/8 */
#define PROPERTY_MEINFO_FIRM_VER       "FirmVersion"           /*Me 版本号*/
#define PROPERTY_MEINFO_CPU_NAME            "Name"                  /*处理器名称*/
#define PROPERTY_MEINFO_RESET_IME           "ResetiME"
#define METHOD_MEINFO_RESET_IME             "SetiMEReset"   /* 复位ARM iME*/
/* END:   Added by baoxu, 2017/8/8 */
#define PROPERTY_MEINFO_PCH_FAULT       "PCHFault"          /* PCH fault状态*/
#define PROPERTY_MEINFO_TCONTROL_TEMP       "TControlTemp"          /* CPU0的TControl Temp */
/*BEGIN : Modified by l00217452, 2017/01/18, PN : DTS2017011809779 */
#define PROPERTY_MEINFO_TJMAX                        "CpuTjmax"               /* CPU0的TJMAX */
#define PROPERTY_MEINFO_CPU_TDP                     "CpuTdp"                 /* CPU0的最大功耗 */
#define PROPERTY_ME_ACCESS_TYPE                     "MeAccessType"       /* ME访问类型；0: SMlink，1: SMBus转PECI  */
#define PROPERTY_SCA_ME_POWER_MODE      "ScaMEPowerMode"   /* 1：Powered in All Sx States. 2：Powered in S0 S1 Only */
/*END : Modified by l00217452, 2017/01/18, PN : DTS2017011809779 */
#define METHOD_MEINFO_SET_PCH_FAULT       "SetPCHFault"          /* 设置PCH fault状态*/
#define METHOD_MEINFO_UTILISE_THRESHOLD     "SetUtiliseThreshold"   /* 设置利用率告警门限*/
#define METHOD_CPU_UTILISE_THRESHOLD        "SetCpuUtiliseThre"
#define METHOD_MEM_UTILISE_THRESHOLD        "SetMemUtiliseThre"
/* BEGIN: Modified by jwx372839, 2017/3/10 11:4:35   问题单号:AR-0000276593-005-007 */
#define PROPERTY_MEINFO_DISK_UTILISE_THRESHOLD   "DiskPartitionUsageThre"   /* 磁盘分区占用率告警阈值 */
#define METHOD_DISK_UTILISE_THRESHOLD            "SetDiskPartitionUsageThre"
#define METHOD_SET_CPU_UTILISE                   "SetCpuUtilise"
#define METHOD_SET_MEM_UTILISE                   "SetMemUtilise"
/* END:   Modified by jwx372839, 2017/3/10 11:6:3 */
#define METHOD_MEINFO_SET_FORCE_RECOVERY_CAUSE   "SetForceRecoveryCause"
#define METHOD_MEINFO_REPIAR_ME_BY_PFR "RepiarMeByPfr"


/* MeInfo类的对象及属性 */
#define CLASS_CPU_STATISTIC                 "CpuStatistic"          /* CPU统计类 */
#define OBJECT_CPU_STATISTIC                "CpuStatistic0"         /* CPU统计对象 */
#define PROPERTY_CPU_STAT_USAGE_CUR         "UsageCur"              /* CPU使用率当前值 */
#define PROPERTY_CPU_STAT_USAGE_MAX         "UsageMax"              /* CPU使用率最小值 */
#define PROPERTY_CPU_STAT_USAGE_MIN         "UsageMin"              /* CPU使用率最大值 */
#define PROPERTY_CPU_STAT_USAGE_AVER        "UsageAver"             /* CPU使用率平均值 */


#define CLASS_CPU_ID_MAP                    "CpuIdMap"              /*  CPU物理号逻辑号映射关系类  */
#define OBJECT_CPU_ID_MAP                   "CpuIdMap"              /*  CPU物理号逻辑号映射关系对象*/
#define PROPERTY_CPUIDMAP_PHYSICALID        "PhysicalId"            /*  CPU物理号逻辑号映射物理ID号 */
#define PROPERTY_CPUIDMAP_LOGICALID         "LogicalId"             /*  CPU物理号逻辑号映射逻辑ID号*/

/* BEGIN: Added by gwx455466, 2017/7/5 12:2:1   问题单号:AR-0000276588-006-001 */
#define CLASS_COMPONENTINFO                           "ComponentInfo"                   /* 通用部件信息类 */
#define PROPERTY_COMPONENTINFO_TYPE               "type"                    /* 类型 */
#define PROPERTY_COMPONENTINFO_PART_NUM           "PartNum"                  /* 部件的部件号 */
#define PROPERTY_COMPONENTINFO_KEYWORD1           "Keyword1"          /* 关键字1 */
#define PROPERTY_COMPONENTINFO_KEYWORD2           "Keyword2"          /* 关键字2 */
#define PROPERTY_COMPONENTINFO_KEYWORD3           "Keyword3"          /* 关键字3 */
/* END:   Added by gwx455466, 2017/7/5 12:2:6 */

/* CPU处理器类的对象及属性 */
#define CLASS_CPU                           "Cpu"                   /* CPU处理器类 */
#define PROPERTY_CPU_PHYSIC_ID              "Id"                    /* 处理器的物理ID */
#define PROPERTY_CPU_NAME                   "Name"                  /* 处理器的名称 */
#define PROPERTY_CPU_MANUFACTURER           "Manufacturer"          /* CPU厂商信息 */
#define PROPERTY_CPU_FAMILY                 "Family"                /* CPU家族 */
#define PROPERTY_CPU_MODEL                  "Model"                 /* CPU型号 */
#define PROPERTY_CPU_HEALTH                 "CpuHealth"
#define PROPERTY_CPU_ENTITY                 "CpuEntity"
#define PROPERTY_CPU_PRESENCE               "Presence"
#define PROPERTY_CPU_INVISIBLE              "Invisible"             /* 是否不对外显示Cpu信息 */
#define PROPERTY_CPU_POWERON                "PowerOn"               /* CPU 上电信息 */
#define PROPERTY_CPU_LOCALITY               "Locality"              /* CPU 相关信息能否直接从本地ME获取 */
#define PROPERTY_CPU_SOCKET_DEST            "SocketDesignation"     /* CPU槽位信息 */
#define PROPERTY_CPU_MAX_SPEED              "MaxSpeed"              /* CPU最大速度 */
#define PROPERTY_CPU_CURRENT_SPEED          "CurrentSpeed"          /* CPU当前速度 */
#define PROPERTY_CPU_EXTERNAL_CLOCK         "ExternalClock"         /* CPU外部时钟 */
#define PROPERTY_CPU_ASSET_TAG              "AssetTag"              /* CPU资产标签 */
#define PROPERTY_CPU_SN                     "SN"                    /* CPU序例号 */
/* BEGIN: Added by wangwei wwx549865, 2018/8/15   PN:AR.SR.SFEA02130924.041.002*/
#define PROPERTY_CPU_PREVIOUS_SN                     "PreviousSN"                    /* 前CPU序例号 */
#define PROPERTY_CPU_REPLACE_FLAG                    "ReplaceFlag"                   /*CPU更换标志位*/
#define PROPERTY_CPU_REPLACE_TIME                    "ReplaceTime"                   /* CPU更换时间戳，秒 */

/* END:   Added by wangwei wwx549865, 2018/8/15 */
#define PROPERTY_CPU_VERSION                "Version"               /* CPU版本 */
#define PROPERTY_CPU_DTS_TEMP               "DTS"                   /* CPU的DTS温度 */
#define PROPERTY_CPU_NEGATIVE_DTS           "NegativeDTS"           /* CPU的DTS温度(取反) */
#define PROPERTY_CPU_MARGIN_TEMP            "Margin"                /* CPU的Margin温度 */
#define PROPERTY_CPU_NEGATIVE_MARGIN        "NegativeMargin"        /* CPU的NegativeMargin温度(取反) */
#define PROPERTY_CPU_THERM_TRIP             "ThermTrip"             /* CPU的过热保护状态*/
#define PROPERTY_CPU_CLEAR_THERM_TRIP       "ClearThermTrip"        /* 清CPU的过热保护状态*/
#define PROPERTY_CPU_PROCHOT                "ProcHot"               /* CPU的内核过热保护状态*/
#define PROPERTY_CPU_PROCHOT_CONV           "ProcHotConvert"        /* CPU的内核过热保护状态*/
#define PROPERTY_CPU_MEM_MAX_TEMP           "MaxMemTemp"            /* CPU下挂的内存的最高温度 */
#define PROPERTY_CPU_MEM_MAX_TEMP_LOCATION  "MaxMemTempLocation" /* CPU下挂的最高温度内存标识 */

/* BEGIN: Added by h00256973, 2018/9/20   PN:AR.SR.SFEA02130917.011.002*/
#define PROPERTY_CPU_AEP_MAX_TEMP                    "MaxAEPTemp"               /* CPU下挂的aep 内存的最高温度 */
#define PROPERTY_CPU_AEP_MAX_TEMP_LOCATION  "MaxAEPTempLocation"    /* CPU下挂的最高温度aep内存标识 */
/* END:   Added by  h00256973, 2018/9/20  */

#define PROPERTY_CPU_JC_MAX_TEMP            "MaxJCTemp"             /* CPU下挂的内存控制器的最高温度 */
#define PROPERTY_CPU_POWER                  "CpuPower"              /* CPU的功耗 */
/* BEGIN: Modified by baoxu, 2016/8/29  PN:AR-0000256850-003-002 */
#define PROPERTY_CPU_PROCESSORID             "ProcessorID"              /* CPU的processor ID*/
#define PROPERTY_CPU_CORE_COUNT             "CoreCount"              /* CPU的核数*/
#define PROPERTY_CPU_THREAD_COUNT          "ThreadCount"              /* CPU的线程数*/
#define PROPERTY_CPU_MEMORY_TEC          "MemoryTec"              /* CPU的内存技术*/
#define PROPERTY_CPU_L1CACHE                      "L1Cache"              /* CPU的一级缓存*/
#define PROPERTY_CPU_L2CACHE                      "L2Cache"              /* CPU的二极缓存*/
#define PROPERTY_CPU_L3CACHE                      "L3Cache"              /* CPU的三级缓存*/
/* END:   Modified by baoxu, 2016/8/29 */
#define PROPERTY_CPU_CPU       "CPU"
#define PROPERTY_CPU_PROCESSORTYPE        "ProcessorType"
#define PROPERTY_CPU_ARCHITECTURE          "Architecture"
#define PROPERTY_CPU_INSTRUCTIONSET        "InstructionSet"
//#define PROPERTY_CPU_TCONTROL_TEMP          "TControlTemp"          /* CPU的TControl Temp */
#define PROPERTY_CPU_CORE_TEMP              "CoreTemp"              /* CPU的Core Temp */
#define PROPERTY_CPU_BIST_FAILURE           "BISTFailure"           /* CPU的BIST FAIL状态 */
#define PROPERTY_CPU_CAT_ERR                "CatErr"                /* CPU的Cat Err状态 */
#define PROPERTY_CPU_IERR                   "IErr"                  /* CPU的IErr 状态 */
#define PROPERTY_CPU_TOTEM_ERROR_FLAG       "TotemErrorFlag"        /* CPU发生IErr时，各个totem的错误标志 */
#define PROPERTY_CPU_MCERR                  "McErr"                 /* CPU的McErr 状态 */
#define PROPERTY_CPU_CATIERR                "HDCatIErr"             /* CPU的CatIErr 状态 */
#define PROPERTY_CPU_CATMCERR               "HDCatMcErr"            /* CPU的CatMcErr 状态 */
#define PROPERTY_CPU_MSMIIERR               "HDMsmiIErr"            /* CPU的MSMIIErr 状态 */
#define PROPERTY_CPU_MSMIMCERR              "HDMsmiMcErr"           /* CPU的MSMIMcErr 状态 */
#define PROPERTY_CPU_HWERROR2               "HWError2"              /* CPU的Error2 硬件属性状态*/
#define PROPERTY_CPU_ERROR2                 "Error2"                /* CPU的Error2 状态 */
#define PROPERTY_CPU_FAILURE_ISO            "FailIsolate"           /* CPU的错误Isolate 状态 */
#define PROPERTY_CPU_MCE_ERROR              "MCEErr"                /* CPU的MCE Error 状态 */
#define PROPERTY_CPU_COMCE_ERROR            "CoMCEErr"              /* CPU的可纠正MCE Error */
#define PROPERTY_CPU_QPI_LINK               "QPILink"               /* CPU的QPI Link 状态 */
#define PROPERTY_CPU_CORE_ISO               "CoreIso"               /* CPU的Core Isolate 状态 */
#define PROPERTY_CPU_CONFIG_ERROR           "ConfigErr"             /* CPU的Config error 状态 */
#define PROPERTY_CPU_SOCKET_ISO_CTRL        "SocketIsoCtrl"         /* CPU的Socket隔离控制寄存器 */
#define PROPERTY_CPU_HOME_AGENT_NUM         "HomeAgentNum"          /* CPU的Home Agent 数目 */
#define PROPERTY_CPU_FW_MISMATCH           "FWMismatch"           /* CPU的BIST FAIL状态 */
#define PROPERTY_CPU_MISMATCH           "CPUMismatch"           /* CPU的BIST FAIL状态 */
#define PROPERTY_CPU_PART_NUM           "PartNum"                 /*CPU的部件号*/

#define PROPERTY_CPU_UPI_ERR_LINK_PORT_MASK        "UPIErrLinkPortMask"      /* CPU的故障的UPI端口号 */
#define PROPERTY_CPU_ISO_CORE_ID              "IsoCoreId"           /* CPU的被隔离的Core id */
#define PROPERTY_CPU_CACHEWAY_STATUS            "CacheWayStatus"

#define PROPERTY_CPU_LOCATION           "Location"
#define PROPERTY_CPU_FUNCTION           "Function"
#define PROPERTY_CPU_DEVICENAME           "DeviceName"
#define PROPERTY_CPU_MEM_HOT              "MemHot"
#define PROPERTY_CPU_MEM_PRESENCE         "MemPresence"
#define PROPERTY_CPU_IMCCHANNEL_FRAME_FORMAT "FrameFormat"
#define PROPERTY_CPU_IMCCHANNEL_DIMM_MASK    "IMCChanDimmMask"
#define PROPERTY_CPU_IMCCHANNEL_DIMM_MASK2    "IMCChanDimmMask2"
#define PROPERTY_CPU_MEM_HOT_C01               "MemhotC01"
#define PROPERTY_CPU_MEM_HOT_C23               "MemhotC23"
#define PROPERTY_CPU_ALERT_STATUS        ("ProcHotAlertMask")
#define PROPERTY_CPU_CPLD_TRIG_PROCHOT   ("CpldTriggerProcHot")
#define PROPERTY_CPU_CONFIG_ERROR_CODE        "ConfigErrorCode"
#define PROPERTY_FAILOVER_CH_MASK         "MirrorFailChMask"
/* BEGIN: Added by z00352904, 2016/11/22 17:16:7  PN:AR-0000264509-001-001 */
#define PROPERTY_CPU_USAGE               ("CpuUsage")
/* BEGIN: Added by z00352904, 2016/12/3 13:57:53  PN:AR-0000264509-001-001 */
#define METHOD_CPU_SET_CPU_USAGE         "SetCpuUsage"   /* 设置CPU使用率 */
/* END:   Added by z00352904, 2016/12/3 13:57:56*/
/* END:   Added by z00352904, 2016/11/22 17:16:10*/

/* BEGIN: Add for AR:Hardware_028 by xwx388597, 2016/09/14 */
#define PROPERTY_CPU_DISABLE_CPU_HW         "DisableCpuHw"
#define PROPERTY_CPU_DISABLE_CPU_SW         "DisableCpuSw"
#define METHOD_CPU_SET_DISABLE_CPU          "SetDisableCpu"
/* END: Add for AR:Hardware_028 by xwx388597, 2016/09/14 */

#define PROPERTY_CPU_BUS_NUM_ARRAY         "BusNumArray"
/* BEGIN: Added by gwx455466, 2017/7/5 11:49:51   问题单号:AR-0000276588-006-001 */
#define PROPETRY_CPU_PART_NUM               "PartNum"
/* END:   Added by gwx455466, 2017/7/5 11:49:54 */
#define METHOD_CPU_SET_BUS_NUM              "SetCpuBusNum"

/* BEGIN: Added by c00346034, 2018/06/13,  PN:AR.SR.SFEA02137099.001.017, 支持PECI获取PCIe设备信息 */
#define METHOD_CPU_GET_PCIE_INFO                 "CpuGetPcieInfo"
/* END:   Added by c00346034, 2018/06/13,  PN:AR.SR.SFEA02137099.001.017, 支持PECI获取PCIe设备信息 */

#define METHOD_CPU_GET_INFO                 "CpuGetInfo"
#define METHOD_CPU_SET_TCONTROL             "SetTControl"
#define METHOD_CPU_SET_CPUPOWER             "SetCpuPower"
#define METHOD_CPU_SET_DIMM_TEMP            "SetDimmTemp"

#define METHOD_CPU_GET_TOTEM_ERROR_FLAG     "GetTotemErrorFlag"

#define METHOD_CPU_SET_QPI_LINK             "SetQPILink"
#define METHOD_CLEAR_WAVE_RECORD         "ClearUsgeRecord"
#define METHOD_ROLLBACK_WAVE_RECORD      "RollbackUsgeRecord"

/* BEGIN: Added by wwx390838, 2017/12/18,  PN:AR.SR.SFEA02109778.002.002, 支持CE PFA */
#define PROPERTY_CPU_QPI_LINK_PFA_EVENT_PORT_MASK       "QPILinkPFAEventPortMask"
#define METHOD_CPU_SET_QPI_LINK_PFA_EVENT           "SetQPILinkPFAEvent"
/* END:   Added by wwx390838, 2017/12/18 */
#define PROPERTY_CPU_CACHEWAY_ISO           "CacheWayIso"

#define CLASS_DVFSFUNCTION                  "DVFSFunction"
#define PROPERTY_AVS_VRD_TEMP               "AvsTemp"
#define PROPERTY_AVS_VRD_VOLT               "AvsVolt"
#define PROPERTY_AVS_VRD_POWER              "AvsPower"
#define PROPERTY_CPU_DVFS_ID                "CpuId"
#define PROPERTY_CPU_DVFS_ENABLE            "DVFSEnable"

#define CLASS_NPU                           "NPU"                   /* NPU处理器类 */
#define PROPERTY_NPU_NAME                   "Name"                  /* NPU的名字 */
#define PROPERTY_NPU_POWER                  "Power"                 /* NPU的功耗 */
#define PROPERTY_NPU_ID                     "Id"                    /* NPU的id */
#define PROPERTY_NPU_DBOARD                 "Board"                 /* NPU的所处的板对象 */
#define PROPERTY_NPU_AICORE_TEMP            "AiCoreTemp"            /* NPU的AiCore温度 */
#define PROPERTY_NPU_VRDCHIP_TEMP           "VRDChipTemp"           /* NPU的VRD chip温度 */
#define PROPERTY_NPU_HBM_TEMP               "HBMTemp"               /* NPU的HBM温度 */
#define PROPERTY_NPU_NIMBUS_TEMP            "NimBusTemp"            /* NPU的NimBus温度 */
#define PROPERTY_NPU_VOLT                   "Volt"                  /* NPU的电压 */
#define PROPERTY_NPU_HEALTH                 "Health"                /* NPU的健康状态 */
#define PROPERTY_NPU_FAULT_CODE             "FaultCode"             /* NPU的故障码 */
#define PROPERTY_NPU_MODE                   "Mode"                  /* NPU的工作模式 */
#define PROPERTY_NPU_SINGLE_HBM_ECC         "SingleECC"             /* HBM ECC单BIT计数 */
#define PROPERTY_NPU_DOUBLE_HBM_ECC         "DoubleECC"             /* HBM ECC双BIT计数 */
#define PROPERTY_NPU_ECC_ALARM_STATUS       "ECCAlarmStatus"        /* NPU ECC错误告警状态 */
#define PROPERTY_NPU_VENDER_ID "VenderId"
#define PROPERTY_NPU_FIRMWARE_VERSION "FirmwareVersion"
#define PROPERTY_NPU_POWER_ON "PowerOn"
#define PROPERTY_NPU_SN "SN"
#define PROPERTY_NPU_MANUFACTURER "Manufacturer"
#define PROPERTY_NPU_VERSION "Version"
#define PROPERTY_NPU_PRESENCE "Presence"
#define PROPERTY_NPU_ARCHITECTURE "Architecture"
#define PROPERTY_NPU_INSTRCTIONSET "InstructionSet"
#define PROPERTY_NPU_COMPUTE_CAPABILITY "ComputeCapability"
#define PROPERTY_NPU_CTRL_CPU_USAGE_PERCENT "CtrlCPUUsagePercent"
#define PROPERTY_NPU_AICORE_MAX_SPEED "AiCoreMaxSpeedMHz"
#define PROPERTY_NPU_AICORE_USAGE_PERCENT "AiCoreUsagePercent"
#define PROPERTY_NPU_TOTAL_MEMORY "TotalMemoryKiB"
#define PROPERTY_NPU_MEMORY_USAGE "MemoryUsageKiB"
#define PROPERTY_NPU_MEMORY_FREE "MemoryFreeKiB"
#define PROPERTY_NPU_CAPABILITY_SUPPORT_MASK    "CapabilitySupportMask"
#define PROPERTY_NPU_MAC_ADDR "MACAddress"
#define PROPERTY_NPU_MEMORY_CAPACITY_KB    "MemoryCapacityKiB"
#define METHOD_SET_MEMORY_CAPACITY    "SetMemoryCapacity"
#define PROPERTY_NPU_READING_CELSIUS "ReadingCelsius"  /* NPU的温度 */
#define PROPERTY_NPU_POWER_CONSUME		"PowerConsumedWatts"  /* NPU的功耗 */
#define PROPERTY_NPU_POWER_CAPACITY		"PowerCapacityWatts" /* 额定功率 */
#define PROPERTY_NPU_MEMORY_BAND_WIDTH "MemoryBandWidth" /* 额定带宽 */
#define PROPERTY_NPU_SERIAL_NUMBER "SerialNumber" /* SN码 */
#define PROPERTY_NPU_MODEL "Model" /* 芯片型号 */
#define METHOD_SET_NPU_INFO_FROM_MCU "SetNpuInfoFromMcu"
#define PROPERTY_NPU_IPV4_ADDR "IpAddr"
#define PROPERTY_NPU_IPV4_SUBNET_MASK "SubnetMask"
#define PROPERTY_NPU_IPV4_GATEWAY "Gateway"
#define PROPERTY_NPU_PACKETS_RX "ReceivedPackets"
#define PROPERTY_NPU_PACKETS_TX "TransmittedPackets"
#define PROPERTY_NPU_DROPPED_PACKETS "DroppedPackets"
#define PROPERTY_NPU_IERR "IErr"
#define PROPERTY_NPU_IERRFLAG "IErrFlag"
#define PROPERTY_NPU_IMUSTATE "IMUState"
#define PROPERTY_NPU_HISTORY_ECC_COUNT          "HistoryEccCount"
#define PROPERTY_NPU_HISTORY_ECC_TIME           "HistoryEccTime"
#define PROPERTY_NPU_HISTORY_SINGLE_BIT_ECC     "HistorySingleBitEcc"
#define PROPERTY_NPU_HISTORY_MULTI_BIT_ECC      "HistoryMultiBitEcc"
#define PROPERTY_NPU_ROOTBDF "RootBDF"
#define PROPERTY_NPU_BDF "BDF"
#define PROPERTY_NPU_BMAID "BMAId"
#define PROPERTY_NPU_MEM_UTL "MemUsagePercent"
#define PROPERTY_NPU_HPM_UTL "HBMUsagePercent"
#define PROPERTY_NPU_MEM_BW "MemBWUsagePercent"
#define PROPERTY_NPU_HBM_BW "HBMBWUsagePercent"
#define PROPERTY_NPU_AICPU_UTL "AiCPUUsagePercent"
#define METHOD_SET_NPU_SUMMARY_FROM_BMA "SetNPUStatisticsObjFromBMA"
#define METHOD_SET_NPU_BDF "SetBDF"

#define CLASS_NPU_HISTORY_ECC_INFO              "NPUHistoryECCInfo"
#define PROPERTY_HISTORY_ECC_TYPE               "Type"
#define PROPERTY_HISTORY_ECC_COUNT              "Count"
#define PROPERTY_HISTORY_ECC_TIME               "Time"
#define PROPERTY_HISTORY_ECC_PHY_ADDR_LOW       "PhyAddrLow"
#define PROPERTY_HISTORY_ECC_PHY_ADDR_HIGH      "PhyAddrHigh"
#define PROPERTY_HISTORY_ECC_STACK_PC_ID        "StackPcId"
#define PROPERTY_HISTORY_ECC_ROW_COLUMN         "RowColumn"
#define PROPERTY_HISTORY_ECC_BANK               "Bank"
#define PROPERTY_HISTORY_ECC_ERR_CNT            "AddrErrorCount"

#define NPU_NUMBER_ON_BOARD 4

#define METHOD_SET_WORK_MODE                "SetMode"       /*设置NPU工作模式*/
#define METHOD_COLLECT_NPU_LOG              "CollectNpuLog"

#define PROPERTY_BOARD_ID                   "Slot"                  /* NPU所处的板ID */

#define CLASS_PROCESSOR_FAMILY              "ProcessorFamily"
#define PROPERTY_IMC_NUM                    "IMCNum"
#define PROPERTY_CPU_CODE                   "CPUCode"
#define PROPERTY_PLATFORM_ID                "PlatformID"       /* 平台代号*/

#define CLASS_BANK_MEM_MAP                  "BankMemMap"
#define PROPERTY_CPU_BANK                   "Bank"
#define PROPERTY_CPU_IMCID                  "IMCId"
#define PROPERTY_CPU_CHANNELID              "ChannelId"

#define CLASS_PCIE_ADDR_INFO                "PcieAddrInfo"
#define PROPERTY_DYNAMIC_BDF                "DynamicBDF"
#define PROPERTY_SEGMENT                    "Segment"
#define PROPERTY_GROUP_ID                   "GroupId"
#define PROPERTY_COMPONENT_TYPE             "ComponentType"
#define PROPERTY_PCIE_SLOT_ID               "SlotId"
#define PROPERTY_SOCKET_ID                  "SocketId"
#define PROPERTY_BDF_NO                     "BDFNo"
#define PROPERTY_PCIE_BUS                   "Bus"
#define PROPERTY_PCIE_DEVICE                "Device"
#define PROPERTY_PCIE_FUNCTION              "Function"
#define PROPERTY_VENDOR_ID                  "VendorId"
#define PROPERTY_DEVICE_ID                  "DeviceId"
#define PROPERTY_SEC_BUS                    "SecBus"
#define PROPERTY_SUB_BUS                    "SubBus"
#define PROPERTY_MMIO_COUNT                 "MmioRangeCount"
#define PROPERTY_MMIO_BASE_L                "MmioRangeBaseL"
#define PROPERTY_MMIO_BASE_H                "MmioRangeBaseH"
#define PROPERTY_MMIO_LIMIT_L               "MmioRangeLimitL"
#define PROPERTY_MMIO_LIMIT_H               "MmioRangeLimitH"
#define PROPERTY_CFG_LOW_BASE               "PciCfgLowBase"
#define PROPERTY_CFG_LOW_LIMIT              "PciCfgLowLimit"
#define PROPERTY_CFG_HI_BASE_L              "PciCfgHiBaseL"
#define PROPERTY_CFG_HI_BASE_H              "PciCfgHiBaseH"
#define PROPERTY_CFG_HI_LIMIT_L             "PciCfgHiLimitL"
#define PROPERTY_CFG_HI_LIMIT_H             "PciCfgHiLimitH"
/* BEGIN: Added by linzhen, 2017/06/21 19:43   问题单号:iBMC AR-0000276593-001-002         */
#define PROPERTY_PCIE_STACK                 "Stack"
/* END:   Added by linzhen, 2017/06/21 19:43 */
#define PROPERTY_PCIE_CONTROLLER_TYPE       "ControllerType"
#define PROPERTY_PCIE_CONTROLLER_INDEX      "ControllerIndex"

#define METHOD_SET_ROOT_PORT_BUS_INFO       "SetBusInfo"
/* BEGIN: Added for AR.SR.SFEA02130917.013.001 高端卡DIMM信息查询 by lwx459244, 2018/9/27 */
#define CLASS_FPGA_MEMORY                   "FPGAMemory"
/* END:   Added for AR.SR.SFEA02130917.013.001 高端卡DIMM信息查询 by lwx459244, 2018/9/27 */
/* Memory内存类的对象及属性 */
#define CLASS_MEMORY                        "Memory"                /* 内存类 */
#define PROPERTY_MEM_DIMM_NUM               "DimmNum"               /* 内存编号ID */
#define PROPERTY_MEM_HEALTH                 "MemHealth"             /* 内存健康状态 */
#define PROPERTY_MEM_ENTITY                 "MemEntity"             /* 内存实体引用 */
#define PROPERTY_MEM_NAME                   "DimmName"              /* 内存名称 */
#define PROPERTY_MEM_CPU_ID                 "CpuId"                 /* 内存对应的CPU ID */
#define PROPERTY_MEM_NODE_ID                "NodeId"                /* 内存对应的控制器ID */
#define PROPERTY_MEM_CHANNEL_ID             "ChannelId"             /* 内存对应的通道ID */
#define PROPERTY_MEM_DIMM_ID                "DimmId"                /* 内存对应的DIMM ID */
#define PROPERTY_MEM_RANK_NUM               "RankNum"               /* 内存对应分配的bank 数 */
#define PROPERTY_MEM_PRESENCE               "Presence"              /* 内存在位 */
#define PROPERTY_MEM_LOCALITY               "Locality"              /* 内存相关信息能否直接从本地ME获取 */
#define PROPERTY_MEM_MANUFACTURE            "Manufacturer"          /* 内存在位 */
#define PROPERTY_MEM_TEMP                   "DimmTemp"              /* 内存温度 */
#define PROPERTY_MEM_ECC_COUNT              "DimmEccCnt"            /* 内存ECC告警数量 */
#define PROPERTY_MEM_ECC_ALERT              "DimmEccAlert"          /* 内存是否有ECC告警标识 */
#define PROPERTY_MEM_MANUFACTURER           "Manufacturer"          /* 内存厂商 */
#define PROPERTY_MEM_CFG_ERROR              "DimmCfgErrAlert"       /* 内存配置错误 */
#define PROPERTY_MEM_CAPACITY               "Capacity"              /* 内存容量 */
#define PROPERTY_MEM_CLOCK_SPEED            "ClockSpeed"            /* 内存最大速度 */
#define PROPERTY_MEM_CONFIG_CLOCK_SPEED     "ConfigMemClkSpeed"     /* 内存当前运行速度 */
#define PROPERTY_MEM_ORIGINAL_PART_NUM      "OriPartNum"            /* 内存原厂家的Part       Number */
#define PROPERTY_MEM_ECC_LOGIN_LMT          "DimmECCErrloglmt"      /* 内存ECC logginglimit告警*/
#define PROPERTY_MEM_UC_ECC_ALERT           "DimmUCEccAlert"        /* 内存不可恢复ECC告警 */
#define PROPERTY_MEM_SPARE                  "DimmSpare"             /* 内存隔离 */
#define PROPERTY_MEM_FAULTLED_VALUE         "FaultLedValue"
#define PROPERTY_MEM_PFA_EVENT              "DimmPFAEvent"
#define PROPERTY_MEM_PART_NUM               "PartNum"
#define PROPERTY_MEM_BOM_NUM                "BomNum"
#define PROPERTY_MEM_HEALTHSCORE        "HealthScore"
#define PROPERTY_MEM_LAST_ISOLATION_STATUS           "LastIsolationStatus"
#define PROPERTY_MEM_LAST_PREDICT_TIME          "LastPredictTime"
#define PROPERTY_MEM_TEMP_ACC                   "DimmTempAcc"              /* 内存温度关联的i2c accessor信息 */
#define PROPERTY_MEM_STORM_CNT              "ErrorStormCount"              /* ce风暴抑制次数 */
#define PROPERTY_MEM_CE_RESTRAIN_TIME  "LastCERestrainTime"           /* 内存CE抑制发生时间 */

#define PROPERTY_MEM_LOCATION                   "Location"             /* 内存隔离 */
#define PROPERTY_MEM_FUNCTION                   "Function"             /* 内存隔离 */
#define PROPERTY_MEM_DEVICENAME             "DeviceName"             /* 内存隔离 */
/* BEGIN: Modified by baoxu, 2016/8/29  PN:AR-0000256850-004-002 */
#define PROPERTY_MEM_TYPE                        "Type"             /* 内存类型 */
#define PROPERTY_MEM_SN                             "SN"             /* 内存序列号 */
/* BEGIN: Modified by lwx469453, 2018/8/18   PN:AR.SR.SFEA02130924.041.003 */
#define PROPERTY_MEM_PREVIOUS_SN                                "PreviousSN"             /* 保存下电前内存序列号值*/
#define PROPERTY_MEM_REPLACE_FLAG                               "ReplaceFlag"             /* 触发事件记录标志，为1触发，为0不触发*/
#define PROPERTY_MEM_REPLACE_TIME                               "ReplaceTime"            /* 内存更换时间戳，秒 */

/* END:   Modified by lwx469453, 2018/8/18 */
#define PROPERTY_MEM_MINIMUM_VOLTAGE    "MinimumVoltage"           /* 内存最小电压*/
#define PROPERTY_MEM_RANK                    "Rank"             /*内存RANK(列) */
#define PROPERTY_MEM_BIT_WIDTH                  "BitWidth"             /*内存位宽 */
#define PROPERTY_MEM_CHIP_BIT_WIDTH                  "ChipBitWidth"             /* 芯片位宽 */
#define PROPERTY_MEM_TEC                             "Technology"             /*内存技术， SMbios 中定义的 Type Detail*/
/* END:   Modified by baoxu, 2016/8/29 */
/* BEGIN: Added by gwx455466, 2017/7/5 14:45:21   问题单号:AR-0000276588-006-001 */
#define PROPERTY_MEM_PART_NUM                             "PartNum"
/* END:   Added by gwx455466, 2017/7/5 14:45:26 */
/* BEGIN: Added by cwx388556, 2017/11/22   AR.SR.SFEA02109379.004.005 */
#define PROPERTY_MEM_BASEMODULE                           "BaseModuleType"
/* END:   Added by cwx388556, 2017/11/22 */
/* BEGIN: Added by gwx455466, 2018/9/26 10:8:36   问题单号:AR.SR.SFEA02130917.010.007 */
#define PROPERTY_MEM_NVDIMM_CAPACITOR_ABSENT                             "NVDIMMCapacitorAbsent"
/* END:   Added by gwx455466, 2018/9/26 10:8:42 */
/*BEGIN : Added by cwx290152, 2017/2/8, PN:DTS2017020800938*/
#define PROPERTY_MEM_CFG_ERR_CODE              "DimmCfgErrCode"     /*Major Code*/
/* BEGIN: Added by gwx455466, 2018/9/22 11:57:6   问题单号:AR.SR.SFEA02130917.011.002 */
#define PROPERTY_MEM_TECHNOLOGY2                            "Technology2"             /*SMbios 中定义的Memory Technology*/
/* END:   Added by gwx455466, 2018/9/22 11:57:9 */
#define PROPERTY_MEM_ISOLATIONADDR                            "IsolationAddr"
#define PROPERTY_MEM_IFMM_EVENT             "DimmIFMMEvent"             /* IFMM(智能内存故障管理)预测事件*/

/* BEGIN: Added by h00256973, 2018/11/8   需求号:AR.SR.SFEA02130917.011.003 */
#define PROPERTY_MEM_HEALTHSTATUS                           "HealthStatus"
#define PROPERTY_MEM_ALARMREASON                            "AlarmReason"
#define PROPERTY_MEM_REMAINLIFE                             "RemainLife"
#define PROPERTY_MEM_MEDIATEMP                              "MediaTemp"
#define PROPERTY_MEM_CONTROLLERTEMP                         "ControllerTemp"
#define PROPERTY_MEM_FWVERSION                              "FWVersion"
#define PROPERTY_MEM_VOLATILECAPACITY                       "VolatileCapacity"
#define PROPERTY_MEM_PERSISTENTCAPACITY                     "PersistentCapacity"
/* END:   Added by h00256973, 2018/11/8  */
/* BEGIN: Added by luyan l00446261, 2019/02/28 问题单号:iBMC DTS2019022801458 修改AEP内存判断方式 */
#define PROPERTY_MEM_TECHNOLOGY2_TYPE                    "Technology2Type"        /* 0:普通内存 1:AEP内存 */
/* END:   Added by luyan l00446261, 2019/02/28 */
/* BEGIN: Added for AR.SR.SFEA02130917.013.001 by lwx459244, 2018/9/27 */
#define METHOD_MEM_SET_DIMM_INFO_FROM_MCU       "SetDIMMInfoFromMcu"
/* END:   Added for AR.SR.SFEA02130917.013.001 by lwx459244, 2018/9/27 */
/* BEGIN: Added by gwx455466, 2018/10/23 16:4:8   问题单号:AR.SR.SFEA02130917.010.006 */
#define PROPERTY_MEM_NVDIMM_DATA_REC_ERR_CODE              "NVDIMMDataRecErrCode"     /*NVDIMM Code*/
/* END:   Added by gwx455466, 2018/10/23 16:4:11 */
#define PROPERTY_MEM_MANUFACTURER_ID                       "ManufacturerID"
#define PROPERTY_MEM_MANUFACTURING_LOCATION                "ManufacturingLocation"
#define PROPERTY_MEM_MANUFACTURING_DATE                    "ManufacturingDate"
/* BEGIN: Added by wwx390838, 2017/12/14,  PN:AR.SR.SFEA02109778.002.001, 支持内存CE溢出监控 */
#define METHOD_MEM_SET_DIMM_PFA_EVENT       "SetDimmPFAEvent"
/* END:   Added by wwx390838, 2017/12/14 */
#define METHOD_MEM_SET_ISOLATION_ADDR       "SetIsolationAddr"
#define METHOD_MEM_GET_CPU_LOGIC_ID         "MemGetCpuLogicId"
#define METHOD_MEM_SET_DIMM_IFMM_EVENT       "SetDimmIFMMEvent"
#define METHOD_MEM_SET_DIMM_IFMM_HRALTH_STATUS        "SetHealthStatus"
#define METHOD_MEM_SET_DIMM_STORM_CNT        "SetErrorStormCount"
#define METHOD_MEM_SET_LAST_CE_RESTRAIN_TIME "SetLastCERestrainTime"

#define CLASS_MEMORY_CHANNEL                "MemoryChannel"
#define PROPERTY_MEMORY_CHANNEL_CPU_ID      "CpuId"
#define PROPERTY_MEMORY_CHANNEL_CHAN_ID     "ChannelId"
#define PROPERTY_MEMORY_CHANNEL_NAME        "Name"
#define PROPERTY_MEMORY_CHANNEL_ERROR_CODE  "ConfigErrorCode"

#define CLASS_DATA_CONDITION                "DataCondition"
#define PROPERTY_DATA_CONDITION_ISTRUE      "IsTrue"
#define PROPERTY_DATA_CONDITION_INPUT       "Input"
#define PROPERTY_DATA_CONDITION_DATASET     "DataSet"
#define PROPERTY_DATA_CONDITION_VALID_DATA  "ValidData"
#define PROPERTY_DATA_CONDITION_VDATAUPDATELOGIC  "VDataUpdateLogic"

/*END:Added by cwx290152, 2017/2/8, PN:DTS2017020800938*/

/*BEGIN : Added by r00355870, 2017/2/8, PN:DTS2017021311752*/
#define CLASS_VMSE_CHANNEL                "VmseChannel"
#define PROPERTY_VMSE_CHANNEL_NODE_ID      "CpuId"
#define PROPERTY_VMSE_CHANNEL_CHAN_ID     "ChannelId"
#define PROPERTY_VMSE_CHANNEL_ERROR_CODE  "ConfigErrorCode"
/*END:Added by r00355870, 2017/2/8, PN:DTS2017021311752*/


/* 内存相关PCI寄存器对象及属性 */
#define CLASS_PCI_REG_INFO                  "PCIRegInfo"           /* PCI内存寄存器类 */
#define PROPERTY_PCI_REG_CPU_ID             "CPUId"                /* PCI内存对应CPU ID */
#define PROPERTY_PCI_REG_NODE_ID            "NodeId"               /* PCI内存对应节点 ID */
#define PROPERTY_PCI_REG_IMC_ID             "iMCId"                /* PCI内存对应内存控制器 ID */
#define PROPERTY_PCI_REG_DIMM_NUMBER        "DIMMNumber"           /* PCI内存对应通道最大dimm数量*/
#define PROPERTY_PCI_REG_CHANNEL_ID         "ChannelId"            /* PCI内存对应通道 ID*/
#define PROPERTY_PCI_REG_BUS_NUMBER         "BusNumber"            /* PCI内存对应Bus Num*/
#define PROPERTY_PCI_REG_DEVICE_NUMBER      "DeviceNumber"         /* PCI内存对应Device Num*/
#define PROPERTY_PCI_REG_FUNCTION_NUMBER    "FunctionNumber"       /* PCI内存对应Function Num*/
#define PROPERTY_PCI_REG_TYPE               "RegisterType"         /* PCI内存对应寄存器属性(ECC\PRESENT)*/
#define PROPERTY_PCI_REG_ADDRESS1           "RegisterNumber1"      /* PCI内存Dimm对应寄存器1*/
#define PROPERTY_PCI_REG_ADDRESS2           "RegisterNumber2"      /* PCI内存Dimm对应寄存器2*/
#define PROPERTY_PCI_REG_ADDRESS3           "RegisterNumber3"      /* PCI内存Dimm对应寄存器3*/
#define PROPERTY_PCI_REG_ADDRESS4           "RegisterNumber4"      /* PCI内存Dimm对应寄存器4*/


/* mezz类的对象及属性 */
#define CLASS_MEZZ                                   "MezzCard"
#define PROPERTY_MEZZ_NAME                                "Name"
#define PROPERTY_MEZZ_BOARDID                             "BoardId"
#define PROPERTY_MEZZ_PCBVER                              "PcbVer"
#define PROPERTY_MEZZ_SLOT                              "Slot"
#define PROPERTY_MEZZ_MANUFACTURER               "Manufacturer"
#define PROPERTY_MEZZ_LINK_WIDTH_ABILITY         "LinkWidthAbility"
#define PROPERTY_MEZZ_LINK_SPEED_ABILITY         "LinkSpeedAbility"
#define PROPERTY_MEZZ_LINK_WIDTH                 "LinkWidth"
#define PROPERTY_MEZZ_LINK_SPEED                 "LinkSpeed"

/* DPU卡类的对象及属性 */
#define CLASS_STORAGE_DPU_CARD              "PCIeStorageDPUCard"
#define PROPERTY_DPU_CARD_NAME              "Name"
#define PROPERTY_DPU_CARD_TYPE              "Type"
#define PROPERTY_DPU_CARD_DESC              "Desc"
#define PROPERTY_DPU_CARD_SLOT              "Slot"
#define PROPERTY_DPU_CARD_BOARD_ID          "BoardId"
#define PROPERTY_DPU_CARD_PCB_ID            "PcbId"
#define PROPERTY_DPU_CARD_PCB_VER           "PcbVer"
#define PROPERTY_DPU_CARD_MANUFACTURER      "Manufacturer"
#define PROPERTY_DPU_CARD_BOARD_NAME        "BoardName"
#define PROPERTY_DPU_CARD_HEALTH            "Health"
#define PROPERTY_DPU_CARD_M2_PRESENCE       "M2Presence"
#define PROPERTY_DPU_CARD_U2_PRESENCE       "U2Presence"
#define PROPERTY_DPU_CARD_SLAVE_CARD_PRESENCE      "NetCardPresence"
#define PROPERTY_DPU_CARD_SLAVE_CARD_TYPE          "SlaveCardType"
#define PROPERTY_DPU_CARD_STORAGE_IP_ADDR          "StorageIpAddr"
#define PROPERTY_DPU_CARD_STORAGE_IP_VLAN          "StorageIpVlan"
#define PROPERTY_DPU_CARD_SYSTEM_LOADED_STATUS     "SystemLoadedStatus"
#define PROPERTY_DPU_CARD_HARDWARE_FAILURE         "HardWareFailure"
#define PROPERTY_DPU_CARD_MEMORY_FAILURE           "MemoryFailure"
#define PROPERTY_DPU_CARD_FIRMWARE_INIT_FAILURE    "FirmwareInitFailure"
#define PROPERTY_DPU_CARD_CPU_INIT_FAILURE         "CPUInitFailure"
#define PROPERTY_DPU_CARD_WATCHDOG_TMOUT           "WatchdogTmout"
#define PROPERTY_DPU_CARD_IPV4_ADDR_REG            "IPV4AddrReg"
#define PROPERTY_DPU_CARD_IPV6_ADDR_REG            "IPV6AddrReg"
#define PROPERTY_DPU_CARD_CPLD_CHIP                "Cpld"
#define PROPERTY_DPU_CARD_CPLD_VER_REG             "CpldVerReg"
#define PROPERTY_DPU_CARD_MCTP_SUPPORT             "MctpSupport"
#define PROPERTY_DPU_CARD_ETH_LINK_STATUS          "DpuEthLinkStatus"
#define PROPERTY_DPU_CARD_EID                      "Eid"
#define PROPERTY_DPU_CARD_PHY_ADDR                 "PhyAddr"
#define PROPERTY_DPU_CARD_REF_COMPONENT            "RefComponent"

#define METHOD_GET_DPU_IP_ADDR  "GetStorageIpAddr"
#define METHOD_SET_DPU_CARD_EID   "SetEidToDPUCard"

/* PCIE标卡类的对象及属性 */
#define CLASS_PCIE_CARD                     "PcieCard"              /* PCIE标卡类类 */
#define PROPERTY_PCIE_CARD_VID              "VenderId"              /* 厂商ID号 */
#define PROPERTY_PCIE_CARD_DID              "DeviceId"              /* 设备ID号 */
#define PROPERTY_PCIE_CARD_SVID             "SubVenderId"           /* 厂商子ID号 */
#define PROPERTY_PCIE_CARD_SDID             "SubDeviceId"           /* 设备子ID号 */
#define PROPERTY_PCIE_CARD_PRESENCE         "Presence"              /* PCI-E卡是否在位 */
#define PROPERTY_PCIE_CARD_HEALTH           "Health"
#define PROPERTY_PCIE_CARD_CPU_NUM          "CpuNum"                /* 连接的CPU */
#define PROPERTY_PCIE_CARD_BUS_NUM          "BusNum"                /* 总线号 */
#define PROPERTY_PCIE_CARD_DEV_NUM          "DevNum"                /* 设备号 */
#define PROPERTY_PCIE_CARD_DESC             "CardDesc"              /* PCI-E卡描述信息 */
/* Start h00272616 AR-0000245338-001-009 2016/3/17 */
#define PROPERTY_PCIE_CARD_FIRMWARE_OBJ     "Firmware"              /* PCIE卡关联的固件类*/
#define PROPERTY_PCIE_CARD_PCB_ID_REG       "PcbIdReg"              /* PCIE卡PCB版本原始信息*/
#define PROPERTY_PCIE_CARD_PCB_VER          "PcbVer"                /* PCIE卡PCB版本信息*/
#define PROPERTY_PCIE_CARD_BOARD_ID         "BoardId"               /* PCIE卡的BOARD ID*/
#define PROPERTY_PCIE_CARD_BOM_ID           "BomId"                 /* PCIE卡的BOM ID */
#define PROPERTY_PCIE_CARD_SLOT_ID          "PcieCardSlot"
/* End h00272616 AR-0000245338-001-009 2016/3/17 */
#define PROPERTY_PCIE_CARD_LOGIC_SLOT       "LogicSlot"             /* 标卡丝印 */
/* Start by p00381829  mctp  2018/01/25 */
/* End by p00381829  mctp  2018/01/25 */
/* Start by h00272616 DTS2016022909391 20160505 */
#define PROPERTY_PCIE_CARD_SLAVE_CARD       "SlaveCard"
/* End by h00272616 DTS2016022909391 20160505 */
/* BEGIN: Added by Yangshigui YWX386910, 2017/6/15   问题单号:AR-0000276589-001-007*/
#define PROPERTY_PCIE_CARD_SLAVE_CARD_NAME  "Name"
/* END:   Added by Yangshigui YWX386910, 2017/6/15 */
/* BEGIN: Modified by wwx390838, 2017/11/02 19:26:5   问题单号:DTS2017110105353   */
#define PROPERTY_PCIE_CARD_REF_COMPONENT    "RefComponent"
/* END:   Modified by wwx390838, 2017/11/02 19:26:9 */
/* BEGIN: Added for  by lwx459244, 2017/11/28 */
#define PROPERTY_PCIE_CARD_PROTOCOL    "Protocol"
#define PROPERTY_PCIE_CARD_MAXFRAMELEN    "MaxFrameLen"
#define PROPERTY_PCIE_CARD_REFCHIP    "RefChip"
#define PROPERTY_PCIE_CARD_CHIPTEMP    "ChipTemp"
#define PROPERTY_PCIE_CARD_CHIP_NUMBER    "ChipNumber"
#define PROPERTY_PCIE_CARD_FAULT_STATE          "FaultState"
#define PROPERTY_PCIE_CARD_FAULT_CODE           "FaultCode"
/* END:   Added for  by lwx459244, 2017/11/28 */

/* BEGIN: 2019-8-2 tianpeng twx572344 PN:UADP320011 支持ECC */
#define PROPERTY_PCIE_CARD_CHIPECC           "ECCCountPerChip"
/* END:   2019-8-2 tianpeng twx572344 PN:UADP320011 支持ECC */

/* BEGIN: Modified by h00422363, 2018/5/31 14:21:25   问题单号:SR.SFEA02130917.004.001 FPGA卡基本信息查询*/
#define PROPERTY_PCIE_CARD_SLAVE_CARD_ARR   "SlaveCardArr"
/* END:   Modified by h00422363, 2018/5/31 14:21:42 */

/* BEGIN: Added by tangjie, 2018/5/7   问题单号:SR.SFEA02130925.002 基于MCTP管理网卡 */
/* END:   Added by tangjie, 2018/5/7 */
#define CLASS_OCP_CARD              "OCPCard"
/* BEGIN: FOR AR Added by wenghongzhang 00424937  2017/11/27 */
#define PROPERTY_ACCELERATE_CARD_REF_CHIP   "RefChip"
#define PROPERTY_ACCELERATE_CARD_SN             "SN"
#define PROPERTY_ACCELERATE_CARD_FAULT_STATE            "FaultState"
#define PROPERTY_ACCELERATE_CARD_FAULT_CODE         "FaultCode"
#define PROPERTY_ACCELERATE_CARD_CHIP_TEMP          "ChipTemperature"
#define METHOD_ACCELERATE_SET_HEALTH                 "SetHealthState"
#define METHOD_ACCELERATE_SET_FAULT_CODE                 "SetFaultCode"
#define METHOD_ACCELERATE_SET_CHIP_TEMP                "SetChipTemp"
#define PROPERTY_ACCELERATE_FIRMWARE_VERSION       "FirmwareVersion"
/* END: FOR AR Added by wenghongzhang 00424937  2017/11/27 */
#define METHOD_SET_PCIE_LINK_ABILITY                    "SetPcieLinkAbility"
#define METHOD_SET_PCIE_LINK_INFO                   "SetPcieLinkInfo"
#define METHOD_SET_PCIE_CHIP_TEMP                   "SetPcieTemp"
#define METHOD_SET_PCIE_ERROR_INFO                  "SetPcieErrorInfo"

#define METHOD_SET_PCIE_CARD_DESC   "SetPcieCardDesc"

/* BEGIN: Modified by h00422363, 2018/5/31 14:22:44   问题单号:SR.SFEA02130917.004.001 FPGA卡基本信息查询 */
#define METHOD_PCIE_CARD_GET_DATA_FROM_MCU "MCUReadData"
#define METHOD_PCIE_CARD_SET_DATA_TO_MCU "MCUWriteData"
/* END:   Modified by h00422363, 2018/5/31 14:22:58 */

/* Component类的对象及属性 */
#define CLASS_COMPONENT                         "Component"                   /* Component全局类 */
#define CLASS_AGENT_COMPONENT                         "AgentComponent"                   /* Component全局类 */
#define PROPERTY_COMPONENT_FRUID                     "FruId"                 /* FRUID */
#define PROPERTY_COMPONENT_DEVICE_TYPE                   "DeviceType"                  /* FRU名称 */
#define PROPERTY_COMPONENT_DEVICENUM                 "DeviceNum"                /* DeviceNum*/
#define PROPERTY_COMPONENT_GROUPID                     "GroupID"                 /* GroupID */
#define PROPERTY_COMPONENT_CONTAINERID                     "ContainerId"                 /* ContainerId */
#define PROPERTY_COMPONENT_LOCATION                   "Location"                 /* Location */
#define PROPERTY_COMPONENT_LOCATIONOBJ                   "LocationObj"                 /* LocationObj */
#define PROPERTY_COMPONENT_FUNCTION                     "Function"                 /* Function */
#define PROPERTY_COMPONENT_DEVICE_NAME                    "DeviceName"                 /* DeviceName */
#define PROPERTY_COMPONENT_CONFIG_TYPE                     "ConfigType"                 /* ConfigType */
#define PROPERTY_COMPONENT_PRESENCE                     "Presence"                 /* Presence */
#define PROPERTY_COMPONENT_HEALTH                     "Health"                 /* Health */
#define PROPERTY_COMPONENT_BOARDID                   "BoardId"                 /* Manufacturer */
#define PROPERTY_COMPONENT_MANUFACTURER                   "Manufacturer"                 /* Manufacturer */
#define PROPERTY_COMPONENT_VIRTUALFLAG             "VirtualFlag"
#define METHOD_COMPONENT_SET_HEALTH                "SetHealth"
#define METHOD_COMPONENT_VIRTUALFLAG               "VirtualFlag"
#define PROPERTY_COMPONENT_POWERSTATE              "PowerState"
#define METHOD_COMPONENT_UPDATE_DEVICENAME         "UpdateDeviceName"
#define PROPERTY_COMPONENT_UNIQUE_ID "ComponentUniqueID"
#define PROPERTY_COMPONENT_HOSTID              "HostId"

/* FRU类的对象及属性 */
#define CLASS_BOARD_POWER                  "BoardPower"             /* BoardPower类名 */
#define METHOD_GET_POWER_BY51              "GetPowerByC51"          /* 从51获取INA220的功率 */
#define PROPERTY_BOARD_POWER_NAME          "Name"                   /* 名称 */


/* FRU类的对象及属性 */
#define CLASS_FRU                           "Fru"                   /* Fru全局类 */
#define PROPERTY_FRU_ID                     "FruId"                 /* FRUID */
#define PROPERTY_IS_ALLOW_ACCESS            "IsAllowAccess"         /* 本控是否能访问 */
#define PROPERTY_FRU_NAME                   "Name"                  /* FRU名称 */
#define PROPERTY_FRU_ELABEL_RO              "Elabel"                /* Elabel引用对象 */
#define PROPERTY_FRUDEV_RO                  "FruDev"                /* FruDev引用对象 */
#define PROPERTY_FRU_HOTSWAP_RP             "HotSwapState"          /* 热插拔状态引用属性 */
#define PROPERTY_FRU_POWERON_STATE_RP       "PowerOnState"          /* FRU上下电状态 */
#define PROPERTY_FRU_HARD_ADDR              "HardAddress"           /* FRU硬件地址 */
#define PROPERTY_FRU_SITE_TYPE              "SiteType"              /* FRU site类型 */
#define PROPERTY_FRU_SITE_ADDR              "SiteAddr"              /* FRU site地址 */
#define PROPERTY_FRU_IPMB_ADDR              "IpmbAddr"              /* FRU IPMB地址 */
#define PROPERTY_FRU_PCB_VERSION            "PcbVersion"            /* FRU PCB版本号 */
#define PROPERTY_FRU_IS_MANAGED             "IsManaged"             /* 标志FRU是否可管理 */
#define PROPERTY_FRU_TYPE                   "Type"                  /* FRU的类型 */
#define PROPERTY_FRU_TARGET                 "Target"                /* FRU的Target,用于IPMI命令转发 */
#define PROPERTY_FRU_BOARDID                "BoardId"               /* FRU的BoardId */
#define PROPERTY_FRU_PCBID                  "PcbId"                 /* FRU的pcbid */
#define PROPERTY_FRU_POSITION               "Position"              /* FRU的位置号 */
#define PROPERTY_FRU_CPLD_STATUS            "CpldStatus"            /* FRU的cpld状态 */
#define PROPERTY_FRU_EEP_STATUS             "EepStatus"             /* FRU的eeprom的状态 */
#define PROTERY_FRU_DATA_VALID_STATE        "DataValidState"        /* 产品自描述信息状态 */
#define PROPERTY_FRU_CPLD_TEST_REG          "CpldTestReg"           /* FRU的cpld测试寄存器 */
#define PROPERTY_FRU_ELABEL_PRODUCT_REF     "ElabelProductRef"      /* 标记Fru电子标签的Product域是否需要关联其他Fru */
#define PROPERTY_FRU_ELABEL_RIMM_OBJ        "RimmObj"               /* 标记Fru电子标签关联的真实对象*/
/* BEGIN: Added by 00356691 zhongqiu, 2019/1/18   问题单号:DTS2018122701591 6U识别条概率性FT测试失败  */
#define PROPERTY_FRU_INIT_TIMEOUT_WAIT_TIME "InitTimeoutInSec"      /* Fru初始化完成最大超时时间 */
/* END: Added by 00356691 zhongqiu, 2019/1/18   问题单号:DTS2018122701591 6U识别条概率性FT测试失败  */

/* BEGIN: Added by zengrenchang(z00296903), 2017/6/24   问题单号:DTS2017051808232*/
#define PROPERTY_FRU_CPLD2_STATUS            "Cpld2Status"            /* FRU的cpld2状态 */
#define PROPERTY_FRU_CPLD2_TEST_REG          "Cpld2TestReg"           /* FRU的cpld2测试寄存器*/
/* END:   Added by zengrenchang(z00296903), 2017/6/24 */
#define PROPERTY_FRU_CPLD3_STATUS            "Cpld3Status"            /* FRU的cpld3状态 */
#define PROPERTY_FRU_CPLD3_TEST_REG          "Cpld3TestReg"           /* FRU的cpld3测试寄存器 */

/*BEGIN : Added by cwx290152, 2017/02/13, PN : DTS2017021311047*/
#define METHOD_FRU_SET_MEZZ_MACADDR         "SetMezzMacAddr"
/*END : Added by cwx290152, 2017/02/13, PN : DTS2017021311047*/
#define METHOD_FRU_SET_EEPROM_DATA          "SetEepromData"
#define METHOD_FRU_GET_EEPROM_DATA          "GetEepromData"
#define METHOD_FRU_SET_EEPROM_RAW_DATA      "SetEepromRawData"
#define METHOD_GET_CANBUS_DEV_EEPROM_DATA     "GetCanbusDevEepromData"
#define METHOD_SET_CANBUS_DEV_EEPROM_DATA     "SetCanbusDevEepromData"
#define METHOD_FRU_STOP_SHARED_CPLD_SCAN    "StopSharedCpldScan"
/* BEGIN: Modified by wangpenghui wwx382232, 2018/4/8 16:6:35   问题单号:DTS2018042505373  */
#define METHOD_UPDATE_FRU_ELABEL_INFO         "UpdateFruInfo"       /* 设置ProductAssetTag标签方法 */
/* END:   Modified by wangpenghui wwx382232, 2018/4/8 16:6:38 */
#define METHOD_FRU_SET_CPLD_STATUS_FAIL     "SetCpldStatusFail"
/* BEGIN: Modified by h00422363, 2018/5/25 14:48:6   问题单号:SR.SFEA02130917.004.001 FPGA卡基本信息查询 */
#define METHOD_FRU_INIT_FRU_INFO_FROM_MCU "InitFruInfoFromMcu"
/* END:   Modified by h00422363, 2018/5/25 14:48:40 */
/* BEGIN: Modified by c00518401, 2020/2/3 17:22:30 */
#define METHOD_E2PSTATUS_SYNC "PropertySyncMethod"
/* END:   Modified by c00518401, 2020/2/3 17:22:30 */
#define METHOD_UPDATE_FRU_PCB_VERSION         "UpdatePcbVer"
/* FruDev类的对象及属性 */
#define CLASS_FRUDEV                        "FruDev"                /* FruDev全局类 */
#define PROPERTY_FRUDEV_STORAGE_LOC         "StorageLoc"            /* FRU信息存取属性,通过这个属性进行FRU信息的读写 */
#define PROPERTY_FRUDEV_STORAGE_TYPE        "StorageType"           /* FRU信息存储介质的类型 */
#define PROPERTY_FRUDEV_SIZE                "Size"                  /* FRU信息最大存储空间大小 */
#define PROPERTY_FRUDEV_HEALTH              "Health"                /* FRU设备的健康状态 */
/* BEGIN: Added by h90006956, 2016/5/28 AR-0000245335-004-006 支持电子标签读写 */
#define PROPERTY_FRUDEV_ISWP                "IsWP"                  /* FRU写eeprom是否有写保护 */
#define PROPERTY_FRUDEV_EEPROM_WP           "EepromWp"              /* FRU EEPROM写保护的accessor */
#define PROPERTY_FRUDEV_WP_CONFIG           "WPCfg"                 /* FRU EEPROM写保护9555管脚的输入输出配置 */
#define PROPERTY_FRUDEV_WP_CFG_MASK         "CfgMask"               /* FRU EEPROM写保护9555管脚的掩码 */
/* END: Added by h90006956, 2016/5/28 AR-0000245335-004-006 支持电子标签读写 */

#define CLASS_DOUBLE_EEPROM_FRUDEV          "DoubleEepFruDev"     /* 双EEPROM背板FruDev全局类 */
#define PROPERTY_FRUDEV_BACK_STORAGE_LOC    "BackStorageLoc"            /* FRU信息存取属性,通过这个属性进行FRU信息的读写 */
#define PROPERTY_FRUDEV_EEPROM_BACK_WP      "BackEepromWp"              /* FRU Back_EEPROM写保护的accessor */

#define CLASS_CAFE_DEV                      "CafeDev"               /*Cafe格式的EEPROM管理类，继承自FruDev类*/
#define PROPERTY_CAFEDEV_SUPERBLOCK_OFFSET  "SuperBlockOffset"      /*超级块起始偏移*/
#define PROPERTY_CAFEDEV_CAFEBLK_SIZE       "CafeBlockSize"         /*Cafe分区大小*/
#define PROPERTY_CAFEDEV_CHKFAILACTION      "ChkFailAction"         /*EEPROM校验失败动作*/
#define PROPERTY_CAFEDEV_CAFEVER            "CafeVer"               /*Cafe分区版本*/
#define PROPERTY_CAFEDEV_CAFE_ACCESSOR      "CafeAccessor"          /*Cafe格式的EEPROM Accessor*/
#define PROPERTY_CAFEDEV_CAFE_RW_NODE       "CafeRWMode"            /*Cafe格式的EEPROM读写模式*/
#define PROPERTY_CAFEDEV_CAFE_PAGE_CHIPS    "CafePageChips"         /*Cafe分页写依赖的Chip*/
/* BEGIN: Added by 00356691 zhongqiu, 2017/12/8 日志信息增强  PN:AR.SR.SFEA02119723.009.001 支持节点配置机框拓扑*/
#define PROPERTY_CAFEDEV_CAFE_DESC          "CafeDescription"       /*Cafe格式的EEPROM描述*/
/* END:   Added by 00356691 zhongqiu, 2017/12/8 日志信息增强  PN:AR.SR.SFEA02119723.009.001 支持节点配置机框拓扑*/
#define METHOD_CAFEDEV_READ_CAFE_RECORD     "ReadCafeRecord"        /*读取指定键值的Cafe值信息*/
#define METHOD_CAFEDEV_WRITE_CAFE_RECORD    "WriteCafeRecord"       /*写入指定键值的Cafe值信息*/

#define CLASS_SHARE_FRUDEV  "ShareFruDev"                                        /* 共享EEPROM的管理类, 继承自FruDev类 */
#define PROPERTY_SHARE_FRUDEV_I2C_LOCK_ACCESSOR         "I2CLockAccessor"        /* 设置共享EEPROM的锁 */
#define PROPERTY_SHARE_FRUDEV_LOCK_STATUS_ACCESSOR      "LockStatusAccessor"     /* 共享EEPROM的锁状态寄存器 */
#define PROPERTY_SHARE_FRUDEV_LOCK_STATUS_EXPECT_VAL    "LockStatusExpectValue"  /* 共享EEPROM的锁状态寄存器期望值 */
#define PROPERTY_SHARE_FRUDEV_WAIT_LOCK_TIMEOUT         "WaitLockTimeOut"        /* 共享EEPROM的锁获取锁超时时间 */

/* Elabel类的对象及属性 */
#define CLASS_PERIPHERAL_ELABEL             "PeripheralElabel"
#define CLASS_ELABEL                        "Elabel"                /* Elabel全局类 */
#define OBJ_FRU0_ELABEL                     "Fru0Elabel"            /* 底板电子标签 */
#define PROPERTY_ELABEL_PARSER_PROCOTOL     "ParserProcotol"        /* 电子标签的解析标准 */
#define PROPERTY_ELABEL_CHASSIS_TYPE        "ChassisType"           /* 机框类型 */
#define PROPERTY_ELABEL_CHASSIS_PN          "ChassisPartNumber"     /* 机框部件号 */
#define PROPERTY_ELABEL_CHASSIS_SN          "ChassisSerialNumber"   /* 机框序列号 */
#define PROPERTY_ELABEL_CHASSIS_EL          "ChassisExtendLabel"    /* 机框扩展电子标签 */
#define PROPERTY_ELABEL_BOARD_MFG_TIME      "MfgDateTime"           /* 单板生产日期 */
#define PROPERTY_ELABEL_BOARD_MFG_NAME      "BoardManufacturer"     /* 单板制造商 */
#define PROPERTY_ELABEL_BOARD_PRO_NAME      "BoardProductName"      /* 单板产品名称 */
#define PROPERTY_ELABEL_BOARD_SN            "BoardSerialNumber"     /* 单板序列号 */
#define PROPERTY_ELABEL_BOARD_PN            "BoardPartNumber"       /* 单板部件号 */
#define PROPERTY_ELABEL_BOARD_ID            "BoardFileId"           /* 单板FileID */
#define PROPERTY_ELABEL_BOARD_EL            "ExtendLabel"           /* 单板扩展电子标签 */
#define PROPERTY_ELABEL_PRODUCT_SN          "ProductSerialNumber"   /* 产品序列号 */
#define PROPERTY_ELABEL_PRODUCT_NAME        "ProductName"           /* 产品名称 */
#define PROPERTY_ELABEL_PRODUCT_MFG_NAME    "ProductManufacturer"   /* 产品制造商 */
#define PROPERTY_ELABEL_PRODUCT_PN          "ProductPartNumber"     /* 产品部件号 */
#define PROPERTY_ELABEL_PRODUCT_VERSION     "ProductVersion"        /* 产品版本 */
#define PROPERTY_ELABEL_PRODUCT_ASSET_TAG   "ProductAssetTag"       /* 产品资产标签 */
#define PROPERTY_ELABEL_PRODUCT_ID          "ProductFileId"         /* 产品FileID */
#define PROPERTY_ELABEL_PRODUCT_EL          "ProductExtendLabel"    /* 产品扩展电子标签 */
#define PROPERTY_ELABEL_IS_CONTAIN_SYS_AREA "IsContainSystemArea"   /* 是否包含系统信息,目前只有主板有 */
#define PROPERTY_ELABEL_SYS_AREA_ASSESSOR   "SystemAreaAccessor"    /* 系统信息访问Accessor */
#define PROPERTY_ELABEL_PS_RATE             "PsRate"                /* 电源的额定功率 */
#define PROPERTY_ELABEL_PS_INPUT_MODE       "PsInputMode"           /* 电源的模式 */
#define PROPERTY_ELABEL_IS_SUPPORT_DFT      "IsSupportDft"          /* 是否支持装备写入FRU信息 */
#define PROPERTY_ELABEL_IS_SUPPORT_CONFIG_AREA  "IsSupportConfigArea" /* 标识此FRU是否支持配置域，配置域会覆盖掉扩展域，0表示不支持配置域，1表示支持配置域 */
#define PROPERTY_ELABEL_SYS_SERIAL_NUM      "SysSerialNum"          /* 定制化系统序列号 */
#define PROPERTY_ELABEL_SYS_NAME            "SysName"               /* 定制化系统名称 */
#define PROPERTY_PERIPHERAL_ELABEL_TARGET   "Target"                /* rimm管理电子标签访问通道 */
#define METHOD_SET_PERIPHERAL_ELABEL        "SetElabel"             /* 设置rimm管理电子标签方法 */
#define METHOD_SET_PRODUCT_ASSETTAG         "SetProductAssetTag"    /* 设置ProductAssetTag标签方法 */
#define METHOD_DATASYNC_ELABEL              "ElabelPropertySync"    /* 同步电子标签 */


/* FruStatics类的对象及属性 */
#define CLASS_FRU_STATICS                   "FruStatics"            /* Elabel全局类 */
#define OBJ_FRU_STATICS                     "FruStatics"            /* Elabel全局对象 */
#define PROPERTY_VERSION_CHANGE             "VerChange"             /* Version Change, 通知SMM板更新FRU信息 */
#define PROPERTY_FRU_MAX_SUPPORT_NUM        "MaxSupportFru"         /* 主板支持的最大Fru个数 */
#define PROPERTY_FRU_POWER_STABLE_TIME      "StableTime"            /* 主板的上电的稳定时间 */
#define PROPERTY_FRU_POWER_MUL              "PowerMul"              /* 主板功率因子 */
#define PROPERTY_FRU_POWER_DRAW             "PowerDraw"             /* 主板功率参数 */
#define METHOD_SET_VERSION_CHANGE           "SetVersionChange"      /* 设置version change的方法 */
#define METHOD_FRU_UPDATE_MAINBOARD_SLOT_ID "UpdateMainboardSlotID"

/* MassStorage类的对象及属性 */
#define CLASS_MASS_STORAGE_NAME             "MassStorage"            /* 大容量存储器件(例如SD卡)类 */
#define PROPETRY_PRE_CHIP                   "PreChip"                /* 前级管理芯片引用 */
#define PROPETRY_PRESENT                    "Presence"                /* 在位 */
#define PROPETRY_HEALTH                     "Health"                 /* 健康状态 */
#define PROPETRY_VOLUME                     "Volume"                 /* 容量 */
#define PROPETRY_ID                         "Id"                     /* Id */
#define PROPETRY_MANUFACTURER               "Manufacturer"           /* 厂商 */
#define PROPETRY_SN                         "SN"                     /* 序列号 */
#define PROPETRY_FAILEDARRAY                "FailedArray"            /* raid阵列状态 */
#define PROPETRY_REBUILDSTATE               "RebuildState"           /* raid重构状态 */
#define PROPETRY_REBUILDABOUTED             "RebuildAborted"
//BEGIN by wangenmao 2017-1-14 PN:DTS2016120804429
#define PROPERTY_ISFINISHUPGRADE            "IsFinishUpgrade"
#define METHOD_SET_FINISH_UPGRADE_FLAG      "SetFinishUpgradeFlag"
//END by wangenmao 2017-1-14 PN:DTS2016120804429

/* Stor管理芯片类的对象及属性 */
#define CLASS_RAID_CHIP_NAME                "RaidChip"               /* 大容量存储器件的管理芯片(例如: fx3s)类 */
#define OBJ_FX3S_NAME                       "Fx3s"                   /* SD RAID芯片 FX3S */
#define PROPETRY_PATITION_NUM               "PartNum"                /* 分区个数 */
#define PROPETRY_PATITION_SIZE              "PartSize"               /* 每个分区大小 */
#define PROPETRY_PATITION_EN                "PartEn"                 /* 分区使能控制, 如果使能X86能够看到 */
#define PROPETRY_PATITION_WRITE_EN          "PartWriteEn"            /* 分区写使能控制 */
#define PROPETRY_FX3S_VERSION               "Version"                /* 固件版本 */
#define PROPETRY_UNIT_NUM                   "Unit"                   /* 位置号 */
#define PROPETRY_NAME_NUM                   "Name"                   /* 芯片名称 */
#define PROPETRY_SDRAID_LOGIC_DIR           "SDRAIDLogicDir"         /* owner方向 */
#define PROPETRY_SDRAID_HEALTH              "SDRAIDHealth"           /* SDRAID健康状态 */
#define PROPETRY_SDRAID_REBUILD_STATE      "SDRAIDRebuildState"    /* SD卡同步状态 */



#define PROPETRY_SD_RW_CRTL                 "SdRwCtrl"               /* 读写控制权在哪方,(PME or X86) */
#define PROPETRY_READ_THRE                  "ReadThre"               /* 读出错阈值 */
#define PROPETRY_WRITE_THRE                 "WriteThre"              /* 写出错阈值 */
#define PROPETRY_PRIMARY_SD                 "PriSd"                  /* 主SD卡 */

#define CLASS_ENCRYPTKEY                    "EncryptKey"
#define OBJ_ENCRYPTKEY_ROOTKEY              "RootKey"
#define OBJ_ENCRYPTKEY_IPMIPWD              "IpmiPwdWorkKey"
#define OBJ_ENCRYPTKEY_SNMP_PRIVACYPWD      "SnmpPrivacyPwdWorkKey"
#define OBJ_ENCRYPTKEY_SNMPCOMM             "SnmpCommWorkKey"
#define OBJ_ENCRYPTKEY_VNCCOMM              "PwdWorkKey"
#define OBJ_ENCRYPTKEY_TRAPCOMM             "TrapCommWorkKey"
#define OBJ_ENCRYPTKEY_SMTPCOMM             "SmtpPwdWorkKey"
#define OBJ_ENCRYPTKEY_UPGRADE_FILE         "UpgradeFileWorkKey"
#define OBJ_ENCRYPTKEY_SSL                  "SSLWorkKey"
#define OBJ_ENCRYPTKEY_SSHD                 "SSHDWorkKey"
#define OBJ_ENCRYPTKEY_LDAP                 "LDAPWorkKey"
#define OBJ_ENCRYPTKEY_NTP                  "NTPWorkKey"
#define OBJ_ENCRYPTKEY_SENSITIVE_FILE       "SensitiveFileWorkKey"
#define OBJ_ENCRYPTKEY_BIOS                 "BIOSWorkKey"
#define OBJ_ENCRYPTKEY_USB                  "USBUncompressPwd"
/* BEGIN: Added by 00356691 zhongqiu, 2017/9/5   PN:DTS2017081800548*/
#define OBJ_ENCRYPTKEY_REDFISH              "RedfishWorkKey"
/* END:   Added by 00356691 zhongqiu, 2017/9/5   PN:DTS2017081800548*/
#define PROPERTY_ENCRYPTKEY_NAME            "Name"
#define PROPERTY_ENCRYPTKEY_TYPE            "Type"
#define PROPERTY_ENCRYPTKEY_LENGTH          "Length"
#define PROPERTY_ENCRYPTKEY_ALGORITHM       "Algorithm"
#define PROPERTY_ENCRYPTKEY_COMPONENT       "Component"
#define PROPERTY_ENCRYPTKEY_WORKKEY         "WorkKey"
#define PROPERTY_ENCRYPTKEY_DOMAIN_ID       "DomainId"
#define PROPERTY_ENCRYPTKEY_MASTERKEY_ID    "MasterKeyId"
#define METHOD_ENCRYPTKEY_UPDATEWORKKEY     "UpdateWorkKey"
#define METHOD_ENCRYPTKEY_UPDATECOMPONENT   "UpdateComponent"

#define CLASS_SSL_ENCRYPTKEY                "SSLEncryptKey"

/*User类的对象及属性*/
#define CLASS_USER                          "User"
#define PROPERTY_USER_ID                    "UserId"
#define PROPERTY_USER_NAME                  "UserName"
#define PROPERTY_USER_PASSWD                "PassWord"
#define PROPERTY_USER_DEFAULT_PASSWD        "d:PassWord"
#define PROPERTY_USER_PRIVILEGE             "Privilege"
#define PROPERTY_USER_ISENABLE              "IsUserEnable"
#define PROPERTY_USER_ISLOCKED              "IsUserLocked"
#define PROPERTY_USER_PWDVALIDDAYS          "PwdValidDays"
#define PROPERTY_USER_MINIMUMPWDAGEREMAINTIME "MinimumPwdAgeRemainTime"
#define PROPERTY_USER_ROLEID                "UserRoleId"
#define PROPERTY_USER_PERMIT_RULE_IDS       "PermitRuleIds"
#define PROPERTY_USER_LOGIN_INTERFACE       "LoginInterface"
#define PROPERTY_USER_INITIALSTATE          "InitialState"
#define PROPERTY_USER_PUBLICKEY_HASH        "PublickeyHash"
#define PROPERTY_USER_LAST_LOGIN_TIME    "LastLoginTime"
#define PROPERTY_USER_LAST_LOGIN_IP     "LastLoginIP"
/* BEGIN: Added for PN:AR-0000276587-002-003 SNMP加密密码与鉴权密码分开 by l00205093, 2017/4/23 */
#define PROPERTY_USER_SNMPPRIVACYPWDINITIALSTATE "SnmpPrivacyPwdInitialState"
/* END:   Added by l00205093, 2017/4/23 */
#define PROPERTY_USER_INACTUSER_REMAIN_DAYS "InactUserRemainDays"
#define PROPERTY_SNMP_AUTH_KU               "AuthKu"
#define PROPERTY_SNMP_PRIV_KU               "PrivKu"
#define PROPERTY_SNMP_AUTH_KULEN            "AuthKulen"
#define PROPERTY_SNMP_PRIV_KULEN            "PrivKulen"
#define PROPERTY_USER_SNMPAUTHPROTOCOL      "SnmpAuthProtocol"
#define PROPERTY_USER_SNMPAUTH_STATUS       "SnmpAuthProtocolStatus"
#define PROPERTY_USER_SNMPAUTH_ALARM_LIST   "SnmpAuthProtocolAlarmList"
#define PROPERTY_USER_SNMPPRIVACYPROTOCOL   "SnmpPrivProtocol"
#define PROPERTY_USER_SNMPPRIV_STATUS       "SnmpPrivProtocolStatus"
#define PROPERTY_USER_SNMPPRIV_ALARM_LIST   "SnmpPrivProtocolAlarmList"
#define PROP_USER_FIRST_LOGIN_POLICY        "FirstLoginPolicy"
#define PROP_USER_FIRST_M2MLOGIN_POLICY     "M2MFirstLoginPolicy"
#define METHOD_USER_SET_FIRST_LOGIN_PLCY    "SetUserFirstLoginPolicy"
#define METHOD_USER_SETUSERNAME             "SetUserName"
#define METHOD_USER_SETPASSWORD             "SetPasswd"
#define METHOD_USER_SETPASSWORDONLY         "SetPasswdOnly"
#define METHOD_USER_RESETROOT               "ResetRoot"
#define METHOD_USER_SETPRIVILEGE            "SetPrivilege"
#define METHOD_USER_SETUSERROLEID           "SetUserRoleId"
#define METHOD_USER_AUTHUSER                "AuthUser"
#define METHOD_USER_AUTH_AND_LOGIN_BY_CERT  "AuthAndLoginByCert"
#define METHOD_USER_AUTHUSERLOCKSTATE       "AuthUserLockState"
#define METHOD_USER_SETUSERLOCKSTATE        "SetUserLockState"
#define METHOD_USER_ISUSERLOCKED            "IsUserLocked"
#define METHOD_USER_CHECKPASSWORD           "CheckPassword"
#define METHOD_USER_GETPTPASSWORD           "GetPtPassword"
#define METHOD_USER_SET_PERMIT_RULE_ID      "SetUserPermitRuleIds"
#define METHOD_USER_SETINITIALSTATE         "SetInitialState"
#define METHOD_USER_RESET_USER_FAILURES     "ResetUserFailures"
#define METHOD_USER_INCREASE_USER_FAILURES  "IncreaseUserFailures"
#define METHOD_USER_SET_USER_LOGIN_INTERFACE  "SetUserLoginInterface"
#define METHOD_USER_SETPUBLICKEY            "SetPublicKey"
#define METHOD_USER_SETUSERENABLE           "SetUserEnable"
#define METHOD_USER_GET_UIDGID_BY_USERNAME "GetUidGidByUsername"
/* BEGIN: Added for PN:AR-0000276587-002-003 SNMP加密密码与鉴权密码分开 by l00205093, 2017/4/23 */
#define METHOD_USER_SETSNMPPRIVACYPWDINITIALSTATE   "SetSnmpPrivacyPwdInitialState"
#define METHOD_USER_SETSNMPPRIVACYPASSWORD  "SetSnmpPrivacyPassword"
#define METHOD_USER_SETSNMPAUTHPROTOCOL     "SetSnmpAuthProtocol"
#define METHOD_USER_SETSNMPPRIVACYPROTOCOL  "SetSnmpPrivacyProtocol"
#define METHOD_USER_SET_SNMPAUTH_KU         "SetSnmpAuthKu"
#define METHOD_USER_SET_SNMPPRIV_KU         "SetSnmpPrivKu"
/* END:   Added by l00205093, 2017/4/23 */
/* BEGIN: DTS2017070600311. Modified by f00381872, 2017/7/20 */
#define METHOD_USER_SET_USER_AUTH_CONFIG    "SetUserAuthConfig"
/* END: DTS2017070600311. Modified by f00381872, 2017/7/20 */
/* BEGIN: AREA02109777-001-001. Modified by f00381872, 2017/9/20 */
#define METHOD_USER_UPDATEUSERINACTSTARTTIME "UpdateUserInactStartTime"
/* END: AREA02109777-001-001. Modified by f00381872, 2017/9/20 */

#define METHOD_USER_CHECK_NAMESYNC_STATE    "CheckUserNameSyncState"
#define METHOD_USER_SYNC_STORAGE_USER       "SyncStorageUser"

#define CLASS_PASSWD_SETTING                "PasswdSetting"
#define OBJ_PASSWD_SETTING                  "PasswdSetting"
#define PROPERTY_USER_PASSWD_SETTING        "EnableStrongPassword"
#define PROPERTY_MIN_PASSWD_LENGTH          "MinPasswordLength"
#define METHOD_PASSWD_SETTING_SETENABLEPWDCOMPLEXITY    "SetEnablePwdComplexity"
#define METHOD_PASSWD_SETTING_SETMINPASSWDLENGTH    "SetMinPasswordLength"

#define CLASS_SESSION                       "Session"
#define OBJECT_SESSION                      "Session"
#define PROPERTY_SESSION_TIMEOUT            "Timeout"
#define PROPERTY_SESSION_MODE               "Mode"
#define METHOD_SESSION_ADDSESSION           "AddSession"
#define METHOD_SESSION_CHECKSSOSESSIONID    "CheckSSOSessionID"
#define METHOD_SESSION_ADDSSOSESSIONID      "AddSSOSessionID"
#define METHOD_SESSION_GETSSOSESSIONID      "GetSSOSessionID"
#define METHOD_SESSION_DESTROYSSOSESSIONID  "DestroySsoSession"
#define METHOD_SESSION_DELSESSION           "DelSession"
#define METHOD_SESSION_GETTOKEN             "GetToken"
#define METHOD_SESSION_GETONLINEUSER        "GetOnlineUser"

#define METHOD_SESSION_GETSESSIONLIST       "GetSessionList"
#define METHOD_SESSION_GETSESSIONINFO       "GetSessionInfo"

#define METHOD_SESSION_GETONLINEUSERNUM     "GetOnlineUserNum"
#define METHOD_SESSION_LOGOUTUSER           "LogoutUser"
#define METHOD_SESSION_ISSESSIONTIMEOUT     "IsSessionTimeout"
#define METHOD_SESSION_SETSESSIONTIMEOUT    "SetSessionTimeout"
#define METHOD_SESSION_SETSESSIONMODE       "SetSessionMode"
#define METHOD_SESSION_SETSESSIONBEAT       "SessionHeartBeat"
#define METHOD_SESSION_GETLOGOUTTYPE        "GetSessionLogoutType"
#define METHOD_SESSION_DESTORYUSER        "DestoryUserCliSession"
/* BEGIN for Redfish C30 : Added by lwx377954 2016-11-30 */
#define METHOD_SESSION_ISREDFISHINNERSESSION        "IsRedfishInnerSession"
/* END for Redfish C30 : Added by lwx377954 2016-11-30 */
#define METHOD_SESSION_GETONLINEKVMPRIUSERNUM       "GetOnlineKvmPriUserNum"
#define METHOD_SESSION_DESTROYSESSIONBYGROUPID      "DelSessionByGroupID"

/*SecurityEnhance类*/
#define CLASS_SECURITY_ENHANCE                             "SecurityEnhance"        /* 用户安全加固类 */
#define OBJ_SECURITY_ENHANCE                               "SecurityEnhance"
#define PROPERTY_AUTH_FAIL_LOCK_TIME                       "AuthFailLockTime"       /* 用户锁定时长 */
#define PROPERTY_AUTH_FAIL_MAX                             "AuthFailMax"            /* 用户连续认证失败最大次数 */
#define PROPERTY_AUTH_FAIL_LOCK_TIME_EXTENSION             "AuthFailLockTimeExtension"       /* 用户锁定时长 */
#define PROPERTY_AUTH_FAIL_MAX_EXTENSION                   "AuthFailMaxExtension"            /* 用户连续认证失败最大次数 */
#define PROPERTY_AUTH_FAIL_LOCK_TIME_EXTENSION_CUSTOM      "AuthFailLockTimeExCustom"       /* 用户锁定时长，白牌定制化兼容 */
#define PROPERTY_AUTH_FAIL_MAX_EXTENSION_CUSTOM            "AuthFailMaxExCustom"            /* 用户连续认证失败最大次数，白牌定制化兼容 */
#define PROPERTY_PWD_EXPIRED_TIME                          "PwdExpiredTime"         /* 密码有效期 */
#define PROPERTY_EXCLUDE_USER                              "ExcludeUser"            /* 特定用户, 密码有效期无效 */
#define PROPERTY_OLD_PWD_COUNT                             "OldPwdCount"            /* 历史密码数 */
#define PROPERTY_PERMIT_RULE_IDS                           "PermitRuleIds"
#define PROPERTY_ENABLE_USER_MGNT                          "EnableUserMgnt"         /* 用户管理操作使能标志 */
#define PROPERTY_TLS_VERSION                               "TLSVersion"            /* TLS协议安全配置 */
#define PROPERTY_AES_PADDING                               "AESPadding"
#define PROPERTY_SECURITY_IPMIPWDWORKKEY                   "IpmiPwdWorkKey"
#define PROPERTY_SECURITY_IPMIPWDCOMPONENT                 "IpmiPwdComponent"
#define PROPERTY_SECURITY_IPMIPWDMASTERKEY_ID              "IpmiMasterKeyId"
#define PROPERTY_SECURITY_SNMPPRIVACYPWDWORKKEY            "SnmpPrivacyPwdWorkKey"
#define PROPERTY_SECURITY_SNMPPRIVACYPWDCOMPONENT          "SnmpPrivacyPwdComponent"
#define PROPERTY_SECURITY_SNMPPRIVACYPWDMASTERKEY_ID       "SnmpPrivacyPwdMasterKeyId"
/* BEGIN for AR-0000245341-005-002 支持显示警示信息 : Added by ouluchun 00294050 2016-3-24 */
#define PROPERTY_SECURITY_BANNERSTATE                      "BannerState"
#define PROPERTY_SECURITY_BANNERCONTENT                    "BannerContent"
#define PROPERTY_SECURITY_D_BANNERCONTENT                  "d:BannerContent"
/* END for AR-0000245341-005-002 支持显示警示信息 : Added by ouluchun 00294050 2016-3-24 */
#define PROPERTY_SECURITY_SSH_CIPHERS                      "SSHCiphers"
#define PROPERTY_SECURITY_DEFAULT_SSH_CIPHERS              "DefaultSSHCiphers"
#define PROPERTY_SECURITY_SSH_CIPHERS_STATUS               "SSHCiphersStatus"
#define PROPERTY_SECURITY_SSH_CIPHERS_ALARM_LIST           "SSHCiphersAlarmList"
#define PROPERTY_SECURITY_SSH_KEXS                         "SSHKexs"
#define PROPERTY_SECURITY_DEFAULT_SSH_KEXS                 "DefaultSSHKexs"
#define PROPERTY_SECURITY_SSH_KEXS_STATUS                  "SSHKexsStatus"
#define PROPERTY_SECURITY_SSH_KEXS_ALARM_LIST              "SSHKexsAlarmList"
#define PROPERTY_SECURITY_SSH_MACS                         "SSHMACs"
#define PROPERTY_SECURITY_DEFAULT_SSH_MACS                 "DefaultSSHMACs"
#define PROPERTY_SECURITY_SSH_MACS_STATUS                  "SSHMACsStatus"
#define PROPERTY_SECURITY_SSH_MACS_ALARM_LIST              "SSHMACsAlarmList"
#define PROPERTY_SECURITY_SSH_HOST_KEYS                    "SSHHostKeys"
#define PROPERTY_SECURITY_DEFAULT_SSH_HOST_KEYS            "DefaultSSHHostKeys"
#define PROPERTY_SECURITY_SSH_HOST_KEYS_STATUS             "SSHHostKeysStatus"
#define PROPERTY_SECURITY_SSH_HOST_KEYS_ALARM_LIST         "SSHHostKeysAlarmList"
#define PROPERTY_SECURITY_SSL_CIPHER_SUITES                "SSLCipherSuites"
#define PROPERTY_SECURITY_DEFAULT_SSL_CIPHER_SUITES        "DefaultSSLCipherSuites"
#define PROPERTY_SECURITY_SSL_CIPHER_SUITES_STATUS         "SSLCipherSuitesStatus"
#define PROPERTY_SECURITY_SSL_CIPHER_SUITES_ALARM_LIST     "SSLCipherSuitesAlarmList"
#define PROPERTY_SECURITY_SNMPAUTH_CIPHER_SUITES           "SnmpAuthProtocolCipherSuites"
#define PROPERTY_SECURITY_DEFAULT_SNMPAUTH_CIPHER_SUITES   "DefaultSnmpAuthCipherSuites"
#define PROPERTY_SECURITY_SNMPPRIV_CIPHER_SUITES           "SnmpPrivProtocolCipherSuites"
#define PROPERTY_SECURITY_DEFAULT_SNMPPRIV_CIPHER_SUITES   "DefaultSnmpPrivCipherSuites"
#define PROPERTY_SECURITY_RMCP_CIPHER_SUITES               "RMCPCipherSuites"
#define PROPERTY_SECURITY_RMCP_STATUS                      "RMCPCipherSuitesStatus"
#define PROPERTY_SECURITY_RMCP_ALARM_LIST                  "RMCPCipherSuitesAlarmList"
#define PROPERTY_SECURITY_SYSTEMLOCKDOWNSTATE              "SystemLockDownStatus"
#define PROPERTY_SECURITY_ANTI_DNS_REBIND                  "AntiDNSRebindStatus"
#define PROP_SECURITY_IPMI_FIRST_LOGIN_WHITE_LIST          "IpmiFirstLoginWhiteList"
#define PROPERTY_SECURITY_ENCRYPT_COMPONENT_UPDATE_FLAG    "EncryptComponentUpdateFlag"

/* SSH协议各类算法关键字 */
#define SSH_CIPHERS         "Ciphers"
#define SSH_KEXS            "KexAlgorithms"
#define SSH_MACS            "MACs"
#define SSH_HOSTKEYS        "HostKeyAlgorithms"

#define MAX_ALGORITHM_STR_LENGTH   100      /* 算法（或加密套件）字符串（名称+使能状态）长度最大长度99 */
#define MAX_ALGORITHM_NUM          30       /* 最多支持30个算法（或加密套件） */

/* BEGIN for AR-0000276594-001-038  Added by j00383144 2017-6-28 */
#define PROPERTY_SECURITY_INITIALPWDPROMPT                "InitialPwdPrompt"
/* END for AR-AR-0000276594-001-038 Added by j00383144 2017-6-28 */

#define PROPERTY_SECURITY_MINIMUMPWDAGE                    "MinimumPwdAge"
#define PROPERTY_SECURITY_ENHANCE_PASSWORD_AUTHENTICATION  "SSHPasswordAuthentication"
#define PROPERTY_SECURITY_ENHANCE_DOUBLE_CERTIFICATIONENABLE  "DoubleCertificationEnable"
#define PROPERTY_SECURITY_WEAK_PWDDICT_ENABLE              "WeakPwdDictEnable"
/* BEGIN: AREA02109777-001-001. Modified by f00381872, 2017/8/22 */
#define PROPERTY_SECURITY_USER_INACT_TIME_LIMIT               "UserInactTimeLimit"
#define PROPERTY_SECURITY_MASTERKEY_UPDATE_INTERVAL "MKUpdateInterval"
#define PROPERTY_SECURITY_MASTERKEY_LAST_UPDATE_TIME "MKLastUpdateTime"


/* END: AREA02109777-001-001. Modified by f00381872, 2017/8/22 */
/* BEGIN: DTS2018060701245. Modified by f00381872, 2018/6/30 */
#define PROPERTY_SECURITY_RENEW_STARTTIME_FLAG              "RenewStartTimeFlag"
/* END: DTS2018060701245. Modified by f00381872, 2018/6/30 */
#define PROPERTY_SECURITY_CERT_OVERDUE_WARN_TIME            "CertOverdueWarnTime"    /* 证书过期提前告警时间 */

#define METHOD_SECURITY_ENHANCE_CHECK_FILTRATE             "CheckFiltrateStatus"    /*VNC回调检查登录规则 add by t00381753 */

#define METHOD_SECURITY_ENHANCE_CHECK_LIMIT                "CheckUserLimitStatus"   /* 校验用户登录规则 */
#define METHOD_SECURITY_ENHANCE_SETAUTHFAILLOCKTIME        "SetAuthFailLockTime"
#define METHOD_SECURITY_ENHANCE_SETAUTHFAILMAX             "SetAuthFailMax"
#define METHOD_SECURITY_ENHANCE_SETAUTHFAILLOCKTIME_EXTENSION  "SetAuthFailLockTimeExtension"
#define METHOD_SECURITY_ENHANCE_SETAUTHFAILMAX_EXTENSION       "SetAuthFailMaxExtension"
#define METHOD_SECURITY_ENHANCE_SETAUTHFAILLOCKTIMEEX_CUSTOM  "SetAuthFailLockTimeExCustom"
#define METHOD_SECURITY_ENHANCE_SETAUTHFAILMAXEX_CUSTOM       "SetAuthFailMaxExCustom"
/* BEGIN: Added by cwx290152, 2017/04/20   PN:AR-0000276587-001-002 */
#define METHOD_SECURITY_SET_WEAK_PWDDICT_STATE             "SetWeakPwdDictEnableState"
#define METHOD_SECURITY_IMPORT_WEAK_PWDDICT                "ImportWeakPwdDict"
#define METHOD_SECURITY_EXPORT_WEAK_PWDDICT                "ExportWeakPwdDict"
#define METHOD_SECURITY_SEARCH_WEAK_PWDDICT                "SearchWeakPwdDict"
/* END: Added by cwx290152, 2017/04/20   PN:AR-0000276587-001-002 */
#define METHOD_SECURITY_ENHANCE_SETPWDEXPIREDTIME          "SetPwdExpiredTime"
#define METHOD_SECURITY_ENHANCE_SETEXCLUDEUSER             "SetExcludeUser"
#define METHOD_SECURITY_ENHANCE_SETOLDPWDCOUNT             "SetOldPwdCount"
#define METHOD_SECURITY_ENHANCE_SETPERMITRULEIDS           "SetPermitRuleIds"
#define METHOD_SECURITY_ENHANCE_SETENABLEUSERMGNT          "SetEnableUserMgnt"
#define METHOD_SECURITY_ENHANCE_SETTLSVERSION              "SetTLSVersion"
/* BEGIN for AR-0000245341-005-002 支持显示警示信息 : Added by ouluchun 00294050 2016-3-24 */
#define METHOD_SECURITY_ENHANCE_SETBANNERSTATE             "SetBannerState"
#define METHOD_SECURITY_ENHANCE_SETBANNERCONTENT           "SetBannerContent"
#define METHOD_SECURITY_ENHANCE_D_SETBANNERCONTENT           "SetDBannerContent"
/* END for AR-0000245341-005-002 支持显示警示信息 : Added by ouluchun 00294050 2016-3-24 */
/* BEGIN for AR-0000276594-001-038  Added by j00383144 2017-6-28 */
#define METHOD_SECURITY_ENHANCE_SETINITIALPWDPROMPT                "SetInitialPwdPrompt"
/* END for AR-AR-0000276594-001-038 Added by j00383144 2017-6-28 */

#define METHOD_SECURITY_ENHANCE_DOUBLE_CERTIFICATIONENABLE    "SetDoubleCertificationEnable"
#define METHOD_SECURITY_ENHANCE_MODIFYSSHDCONFIGFILE    "ModifySshdConfigFile"

/*PermitRule类*/
/*begin: add by wenyong w00351873, 2016/06/01  DTS2016051106875*/
#define METHOD_SECURITY_ENHANCE_SETEXPIREDTIMEANDMINIMUMPWDAGE  "WebSetPwdExpiredAndMinimumTime"
/*end: add by wenyong w00351873, 2016/06/01*/
#define METHOD_SECURITY_ENHANCE_SETMINIMUMPWDAGE           "SetMinimumPwdAge"
#define METHOD_SECURITY_ENHANCE_SETSSHPASSWORDAUTHENTICATION "SetSSHPasswordAuthentication"
/* BEGIN: AREA02109777-001-001. Modified by f00381872, 2017/8/22 */
#define METHOD_SECURITY_ENHANCE_SETUSERINACTTIMELIMIT        "SetUserInactTimeLimit"
#define METHOD_SECURITY_ENHANCE_SETMASTERKEYUPDATEINTERVAL   "SetMasterkeyUpdateInterval"
#define METHOD_SECURITY_ENHANCE_UPDATEMASTERKEY              "UpdateMasterKey"
#define METHOD_SECURITY_ENHANCE_GETUPDATESTATUS              "GetUpdateStatus"
#define METHOD_SECURITY_ENHANCE_SETCERTOVERDUEWARNTIME       "SetCertOverdueWarnTime"
/* END: AREA02109777-001-001. Modified by f00381872, 2017/8/22 */
#define METHOD_SECURITY_ENHANCE_SET_SSH_CIPHER              "SetSSHCiphers"
#define METHOD_SECURITY_ENHANCE_SET_SSH_KEX                 "SetSSHKexs"
#define METHOD_SECURITY_ENHANCE_SET_SSH_MAC                 "SetSSHMACs"
#define METHOD_SECURITY_ENHANCE_SET_SSH_HOST_KEY            "SetSSHHostKeys"
#define METHOD_SECURITY_ENHANCE_SET_RMCP_CIPHER_SUITE       "SetRMCPCipherSuites"
#define METHOD_SECURITY_ENHANCE_SET_SSL_CIPHER_SUITE        "SetSSLCipherSuites"
#define METHOD_SECURITY_ENHANCE_SET_SYSTEM_LOCKDOWN         "SetSystemLockDownStatus"
#define METHOD_SECURITY_ENHANCE_SET_ANTI_DNS_REBIND         "SetAntiDNSRebindStatus"


/*PermitRule类*/
#define CLASS_PERMIT_RULE                                  "PermitRule"
#define PROPERTY_RULE_ID                                   "RuleId"
#define PROPERTY_IP_RULE_INFO                              "IpRuleInfo"
#define PROPERTY_MAC_RULE_INFO                             "MacRuleInfo"
#define PROPERTY_TIME_RULE_INFO                            "TimeRuleInfo"
#define METHOD_PERMIT_RULE_SETIPRULEINFO                   "SetIpRuleInfo"
#define METHOD_PERMIT_RULE_SETMACRULEINFO                  "SetMacRuleInfo"
#define METHOD_PERMIT_RULE_SETTIMERULEINFO                 "SetTimeRuleInfo"
/*用户角色类*/
#define CLASS_USERROLE                                     "UserRole"
#define PROPERTY_USERROLE_ID                               "RoleId"
#define PROPERTY_USERROLE_ROLENAME                         "RoleName"
#define PROPERTY_USERROLE_USERMGNT                         "UserMgnt"
#define PROPERTY_USERROLE_BASICSETTING                     "BasicSetting"
#define PROPERTY_USERROLE_KVMMGNT                          "KVMMgnt"
#define PROPERTY_USERROLE_VMMMGNT                          "VMMMgnt"
#define PROPERTY_USERROLE_SECURITYMGNT                     "SecurityMgnt"
#define PROPERTY_USERROLE_POWERMGNT                        "PowerMgnt"
#define PROPERTY_USERROLE_DIAGNOSEMGNT                     "DiagnoseMgnt"
#define PROPERTY_USERROLE_READONLY                         "ReadOnly"
#define PROPERTY_USERROLE_CONFIGURE_SELF                   "ConfigureSelf"
#define METHOD_USERROLE_SETKVMMGNT                         "SetKVMMgnt"
#define METHOD_USERROLE_SETVMMMGNT                         "SetVMMMgnt"
#define METHOD_USERROLE_BASESETTING                        "SetBasicSetting"
#define METHOD_USERROLE_SETUSERMGNT                        "SetUserMgnt"
#define METHOD_USERROLE_SETREADMGNT                        "SetReadOnlyMgnt"
#define METHOD_USERROLE_SETPOWERMGNT                       "SetPowerMgnt"
#define METHOD_USERROLE_SETDIAGNOSEMGNT                    "SetDiagnoseMgnt"
#define METHOD_USERROLE_SETSECURITYMGNT                    "SetSecurityMgnt"
#define METHOD_USERROLE_SETCONFIGURESELF                   "SetConfigureSelf"


/*PrivilegeAgent类*/
#define CLASS_PRIVILEGEAGENT                     "PrivilegeAgent"
#define OBJ_NAME_PRIVILEGEAGENT                  "PrivilegeAgent"
#define PROPERTY_PRIVILEGEAGENT_PATHLIST         "PathList"
#define METHOD_PRIVILEGEAGENT_COPYFILE           "CopyFile"
#define METHOD_PRIVILEGEAGENT_CHECK_DIR          "CheckDir"
#define METHOD_PRIVILEGEAGENT_MODIFY_FILE_MODE   "ModifyFileMode"
#define METHDO_PRIVILEGEAGENT_MODIFY_FILE_OWN    "ModifyFileOwn"
#define METHDO_PRIVILEGEAGENT_REVERT_FILE_OWN    "RevertFileOwn"
#define METHOD_PRIVILEGEAGENT_DELETE_FILE        "DeleteFile"
#define METHOD_PRIVILEGEAGENT_CREATE_FILE        "CreateFile"
#define METHOD_PRIVILEGEAGENT_SEND_RECV_MSG_WITH_M3 "SendRecvMsgWithM3"

/*Snmp类*/
#define CLASS_SNMP                               "Snmp"
#define OBJ_NAME_SNMP                            "Snmp"
#define PROPERTY_SNMP_ROCOMMUNITY                "ROCommunity"
#define PROPERTY_SNMP_RWCOMMUNITY                "RWCommunity"
#define PROPERTY_SNMP_STATE                      "State"
#define PROPERTY_SNMP_V1STATE                    "V1State"
#define PROPERTY_SNMP_V2CSTATE                   "V2CState"
#define PROPERTY_SNMP_V3STATE                    "V3State"   /* 新版本该属性废弃, 使用V3Status以解决版本回退兼容新问题 */
#define PROPERTY_SNMP_V3STATUS                   "V3Status"  /* 新版本SNMPv3使能状态使用该属性, 以解决版本回退兼容性问题, 0：禁止使能；1：使能 */
#define PROPERTY_SNMP_PORTID                     "PortID"
#define PROPERTY_SNMP_OLDENGINEID                "OldEngineID"
/* BEGIN: Modified by cwx579007, 2019/02/18, DTS:DTS2019012809736 */
#define PROPERTY_SNMP_IS_UPDATE_ENGINEID         "IsUpdateEngineID"
/* END: Modified by cwx579007, 2019/02/18, DTS:DTS2019012809736 */
#define PROPERTY_SNMP_AUTHPROTOCOL               "AuthProtocol"
#define PROPERTY_SNMP_PRIVPROTOCOL               "PrivProtocol"
#define PROPERTY_SNMP_COMMCOMPONENT              "SnmpCommComponent"
#define PROPERTY_SNMP_COMMWORKKEY                "SnmpCommWorkKey"
#define PROPERTY_SNMP_MASTERKEY_ID               "SnmpCommMasterKeyId"
#define PROPERTY_SNMP_V1V2C_PERMIT               "SNMPV1V2CPermitRuleIds"
#define PROPERTY_SNMP_LONG_PASSWORD_ENABLE       "LongPasswordEnable"
#define PROPERTY_SNMP_RW_COMMUNITY_STATE       "RWCommunityState"
#define PROPERTY_SNMP_HOSTNAME_SYNC_FLAG         "IsSyncHostName"   /* sysName是否同步HostName的标志 */
/** BEGIN: Added by l00272597, 2017/4/17   PN:DTS2017041113422 */
#define PROPERTY_SNMP_SYS_CONTACT                "sysContact"
#define PROPERTY_SNMP_SYS_NAME                   "sysName"
#define PROPERTY_SNMP_SYS_LOCATION               "sysLocation"
/** END:   Added by l00272597, 2017/4/17 */
#define METHOD_SNMP_SETSNMPV1V2CPERMIT           "SetSNMPV1V2CPermitRuleIds"
#define METHOD_SNMP_SETSNMPROCOMMUNITY           "SetSnmpROCommunity"
#define METHOD_SNMP_SETSNMPRWCOMMUNITY           "SetSnmpRWCommunity"
#define METHOD_SNMP_GETSNMPROCOMMUNITY           "GetSnmpROCommunity"
#define METHOD_SNMP_GETSNMPRWCOMMUNITY           "GetSnmpRWCommunity"
#define METHOD_SNMP_SETSNMPSTATE                 "SetSnmpState"
#define METHOD_SNMP_SETSNMPVERSIONSTATE          "SetSnmpVersionState"
#define METHOD_SNMP_SETSNMPV1VERSIONSTATE        "SetSnmpV1VersionState"
#define METHOD_SNMP_SETSNMPV2CVERSIONSTATE       "SetSnmpV2CVersionState"
#define METHOD_SNMP_SETSNMPV3VERSIONSTATE        "SetSnmpV3VersionState"
#define METHOD_SNMP_SETSNMPPORTID                "SetSnmpPortID"
#define METHOD_SNMP_ADDSNMPUSERNAME              "AddSnmpUserName"
#define METHOD_SNMP_DELSNMPUSERNAME              "DelSnmpUserName"
#define METHOD_SNMP_SETSNMPUSERPASSWORD          "SetSnmpUserPassword"
#define METHOD_SNMP_COMPARESNMPUSERPASSWORD      "CompareSnmpUserPassword"
#define METHOD_SNMP_ADDSNMPUSERPASSWORD          "AddSnmpUserPassword"
#define METHOD_SNMP_DELSNMPUSERPASSWORD          "DelSnmpUserPassword"
#define METHOD_SNMP_GETSNMPUSERINFO              "GetSnmpUserInfo"
#define METHOD_SNMP_GETSNMPUSERHEXINFO           "GetSnmpUserHexInfo"
#define METHOD_SNMP_REBOOTSNMP                   "RebootSnmp"
#define METHOD_SNMP_SETSNMPAUTHPROTOCOL          "SetSnmpAuthProtocol"
#define METHOD_SNMP_SETSNMPPRIVPROTOCOL          "SetSnmpPrivProtocol"
#define METHOD_SNMP_DELETENOACCESSSNMPUSER       "DeleteNoAccessSnmpUser"
#define METHOD_SNMP_SETSNMPUSERPRIVILEGE         "SetSnmpUserPrivilege"
#define METHOD_SNMP_SETLONGPASSWORDENABLE        "SetLongPasswordEnable"
/** BEGIN: Added by l00272597, 2017/4/17   PN:DTS2017041113422 */
#define METHOD_SNMP_SET_SYS_CONTACT              "SetsysContact"
#define METHOD_SNMP_SET_SYS_NAME                 "SetsysName"
#define METHOD_SNMP_SET_SYS_LOCATION             "SetsysLocation"
/** END:   Added by l00272597, 2017/4/17 */
/* BEGIN: Modified by cwx579007, 2019/02/18, DTS:DTS2019012809736 */
#define METHOD_SNMP_SET_IS_UPDATE_ENGINEID    "SetIsUpdateEngineID"
/* END: Modified by cwx579007, 2019/02/18, DTS:DTS2019012809736 */
/* BEGIN: Modified by wangpenghui wwx382232, 2017/11/28 11:39:11   问题单号:DTS2017111706432 */
#define METHOD_SNMP_SET_RW_COMMUNITY_STATE             "SetRWCommunityState"
/* END:   Modified by wangpenghui wwx382232, 2017/11/28 11:39:15 */
#define METHOD_SNMP_SET_HOSTNAME_SYNC_FLAG       "SetHostNameSyncFlag"
#define METHOD_SNMP_UPDATE_CIPHERTEXT            "UpdateSnmpCommCiphertext"
#define METHOD_SNMP_UPDATE_PASSWORD_BY_RENAME    "UpdateSnmpPasswordByRename"

/*DDNS*/
#define CLASS_NAME_DNSSETTING                               "DNSSetting"
#define OBJ_NAME_DNSSETTING                                 "DNSSetting"
#define OBJ_NAME_DNS_IPV4                                   "DNSSetting"
#define OBJ_NAME_DNS_IPV6                                   "DNSSetting"
#define PROPERTY_DNSSETTING_DOMAINNAME                      "DomainName"
#define PROPERTY_DNSSETTING_PRIMARYDOMAIN                   "PrimaryDomain"
#define PROPERTY_DNSSETTING_BACKUPDOMAIN                    "BackupDomain"
#define PROPERTY_DNSSETTING_TERTIARYDOMAIN                  "TertiaryDomain"
#define PROPERTY_DNSSETTING_DNSMODE                         "Mode"
#define PROPERTY_DNSSETTING_DNSIPVER                        "IPVer"
#define PROPERTY_DNSSETTING_DNSSTATE                        "State"
#define PROPERTY_DNSSETTING_DNSETHGROUP                     "EthGroup"
#define PROPERTY_DNSSETTING_DNSDHCPFLAG                     "DHCPFlag"
#define PROPERTY_DNSSETTING_DNSINTERVAL                     "Interval"
#define PROPERTY_DNSSETTING_DNSLEASETIME                    "LeaseTime"
#define METHOD_DNSSETTING_PRIMARYDOMAIN                     "SetPrimaryDomain"
#define METHOD_DNSSETTING_BACKUPDOMAIN                      "SetBackupDomain"
#define METHOD_DNSSETTING_TERTIARYDOMAIN                    "SetTertiaryDomain"
#define METHOD_DNSSETTING_DNSIPVER                          "SetDdnsIPVer"
#define METHOD_DNSSETTING_DNSMODE                           "SetDdnsMode"
#define METHOD_DNSSETTING_DNSDHCPFLAG                       "SetDdnsDHCPFlag"
#define METHOD_DNSSETTING_DNSETH                            "SetDdnsEthGroup"
#define METHOD_DNSSETTING_DNSSTATE                          "SetDdnsState"
#define METHOD_DNSSETTING_DOMAINNAME                        "SetDomainName"
#define METHOD_DNSSETTING_SET_DOMAIN_SERVER                 "SetDomainServer"
#define METHOD_DNSSETTING_SET_BACKUP_SERVER                 "SetBackupServer"
#define METHOD_DNSSETTING_SET_DDNS_IPVER                    "SetDdnsIPVer"
#define METHOD_DNSSETTING_PROPERTY_SYNC                     "PropertySyncMethod"



/* 调测日志类对象及属性 */
#define CLASS_LOG                           "Log"                   /* PME日志全局类 */
#define OBJECT_LOG                          "log_0"                 /* 日志对象名称 */
#define PROPERTY_LOG_DIR                    "LogDir"                /* 日志文件存放路径 */
#define PROPERTY_LOG_DEBUGLOG_FILE          "DebugLogFile"          /* 调试日志文件名称 */
#define PROPERTY_LOG_OPERATELOG_FILE        "OperateLogFile"        /* 操作日志文件名称 */
#define PROPERTY_LOG_STRATEGYLOG_FILE       "StrategyLogFile"       /* 策略日志(运行日志)文件名称 */
/* BEGIN: Added by z00352904, 2016/3/21 10:35:24   问题单号:AR-0000245341-003-002 */
#define PROPERTY_LOG_OPERATELOG_THRESHOLD   "OperateLogThreshold"          /* 操作日志容量门限 */
#define PROPERTY_LOG_OPERATELOG_EXCEED      "OperateLogExceedStatus"       /* 操作日志容量溢出标志 */
#define PROPERTY_LOG_OPERATELOG_BACKUPS     "OperateLogBackups"            /* 操作日志备份文件个数 */
#define PROPERTY_LOG_SECURITYLOG_THRESHOLD  "SecurityLogThreshold"         /* 安全日志容量标志 */
#define PROPERTY_LOG_SECURITYLOG_EXCEED     "SecurityLogExceedStatus"      /* 安全日志容量溢出标志 */
#define PROPERTY_LOG_SECURITYLOG_BACKUPS    "SecurityLogBackups"           /* 安全日志备份文件个数 */
#define PROPERTY_LOG_SECURITYLOG_FILE       "SecurityLogFile"              /* 安全日志文件名 */
/* END:   Added by z00352904, 2016/3/21 10:35:28 */
#define PROPERTY_LOG_LINUXOPERATELOG_FILE   "LinuxOperateLogFile"   /* Linux操作日志(安全日志/登陆日志)文件名称 */
#define METHOD_OPERATE_LOG_PROXY            "OperateLogProxy"       /* 操作日志记录代理 */
#define METHOD_MAINT_LOG_PROXY              "MaintLogProxy"         /* 维护日志记录代理 */

/*BEGIN: Added by h00164462, 2015/3/20, PN:深度恢复出厂配置*/
#define METHOD_REMOVE_LOG                   "RmLog"
/*END: Added by h00164462, 2015/3/20, PN:深度恢复出厂配置*/

/* Start by huangjiaqing DTS2017090802914  20170908 */
#define METHOD_ADVANCE_FACTORY_RESTORE                  "AdvanceLoadBak"
/* End by huangjiaqing DTS2017090802914  20170908 */

/* BEGIN: Modified by h00422363, 2017/11/13 20:4:52  AR.SR.SFEA02109777.002.001 */
#define METHOD_CLEAR_OPERATION_LOG              "ClearOperationLog"
#define METHOD_CLEAR_RUN_LOG                          "ClearRunLog"
#define METHOD_CLEAR_SECURITY_LOG                 "ClearSecurityLog"
/* END:   Modified by h00422363, 2017/11/13 20:5:14 */

/* 信息收集类对象及属性 */
#define CLASS_DUMP                          "Dump"                  /* Dump全局类 */
#define OBJECT_DUMP                         "dump_0"                /* dump对象 */
#define PROPERTY_DUMP_THIRD_APP_LOG_PATH    "ThirdAppLogPath"       /* 需要收集的第三方程序日志文件路径 */
#define PROPERTY_DUMP_CUSTOM_INFO_PATH      "CustomInfoPath"        /* 用户自定义信息收集路径 */
#define PROPERTY_DUMP_FILE_PATH             "DumpFilePath"          /* 信息收集路径 */
#define METHOD_DUMP_DUMPINFO                "DumpInfo"              /* 收集方法名称 */
#define METHOD_DUMP_DUMPINFO_ASYNC          "DumpInfoAsync"         /* 异步收集方法名称 */
#define METHOD_DUMP_DUMPINFO_BOTH_SMM_ASYNC "DumpInfoBothSmmAsync"  /* 异步收集主备SMM方法名称 */
#define METHOD_DUMP_GET_DUMP_PHASE          "GetDumpPhase"          /* 获取收集阶段 */
#define METHOD_DUMP_GET_BOTH_SMM_DUMP_PHASE "GetBothSmmDumpPhase"   /* 获取主备SMM收集阶段 */
#define METHOD_DUMP_GET_DUMP_IS_RUNNING     "GetDumpIsRunning"      /* 主备倒换时，获取是否处于信息收集 */

/* 脚本引擎类对象及属性 */
#define CLASS_SCRIPT_ENGINE                 "ScriptEngine"          /* 全局脚本引擎类 */
#define OBJECT_SCRIPT_ENGINE                "script_engine_0"       /* 脚本引擎对象 */
#define PROPERTY_SE_INIT_SCRIPT_PATH        "InitScriptPath"        /* 初始化脚本存放路径 */
#define PROPERTY_SE_SCRIPT_FILE_PATH        "ScriptFilePath"        /* 脚本文件存放路径 */

// START by h00272616  AR2C9A4135 20151113
/* PCIE-SWITCH模块对象及属性 */
#define  PCIE_SWITCH_CLASEE_NAME                       "PcieSwitch"
#define  PROPERTY_PCIE_SWITCH_TYPE                     "Type"
#define  PROPERTY_PCIE_SWITCH_TEMPERATURE              "Temperature"
#define  PROPERTY_PCIE_SWITCH_PRESENT                  "Present"
#define  PROPERTY_PCIE_SWITCH_VERSION                  "Version"
#define  PROPERTY_PCIE_SWITCH_UNITNUM                  "UnitNum"
#define  PROPERTY_PCIE_SWITCH_DEF_UPGRADE_FILE         "DefUpgradeFile"
/*BEGIN  ADD by z00397989 AR-0000276595-003-016  2017.04.20 */
#define  PROPERTY_PCIE_SWITCH_PWRON_UPGRADEACCESSOR           "PwrOnUpgradeAccessor"
#define  PROPERTY_PCIE_SWITCH_PWROFF_UPGRADEACCESSOR           "PwrOffUpgradeAccessor"
#define  PROPERTY_PCIE_SWITCH_PCIESWITCHACTIONOBJ          "PcieSwitchActionObj"
/*END  ADD by z00397989 AR-0000276595-003-016  2017.04.20 */
/*BEGIN  ADD by z00397989 DTS2017051808664  2017.05.23 */
#define  PROPERTY_PCIE_SWITCH_UPGRADESTATE                  "UpgradeState"
/* BEGIN: Added by liumancang, 2017/12/14 18:34:3   需求单号:AR.SR.SFEA02119718.002.001 */
#define PROPERTY_PCIE_SWITCH_UPGRADE_FINISH_FLAG           "UpgradeFinish"
/* END:   Added by liumancang, 2017/12/14 18:34:33 */
/*END  ADD by z00397989 DTS2017051808664  2017.05.23 */
/*BEGIN  ADD by z00397989 DTS2017052708202  2017.06.05 */
#define  PROPERTY_PCIE_SWITCH_POWERSTATE                 "PSWPowerstate"
/*END  ADD by z00397989 DTS2017052708202  2017.06.05 */

/*BEGIN  ADD by l00375984  2017.08.21 */
#define  PROPERTY_PCIE_SWITCH_LOAD_TRIG                  "LoadTrigger"
#define  PROPERTY_PCIE_SWITCH_NODE_ID                    "NodeId"
#define  PROPERTY_PCIE_SWITCH_VALID_MODE                 "ValidMode"
/*END  ADD by l00375984 2017.08.21 */

/* BEGIN: Added by Yangshigui YWX386910, 2017/6/8   问题单号:AR-0000276586-006-001*/
#define PROPERTY_PCIE_SWITCH_SWITCHID                   "SwitchId"
#define PROPERTY_PCIE_SWITCH_UPGRADEFLOWTYPE            "UpgradeFlowType"
/* END:   Added by Yangshigui YWX386910, 2017/6/8 */

#define  PCIE_SWITCH_METHOD_WRITE_8725_CFG                  "WritePex8725Cfg"
#define  PCIE_SWITCH_METHOD_WRITE_PCIESW_CFG                "WritePCIeSWCfg"
/*BEGIN  ADD by z00397989 AR-0000276595-003-016  2017.04.20 */
#define  PCIE_SWITCH_METHOD_UPGRADE_PSWEEP                 "UpgradePCIESWITCHEEPROM"
/*END  ADD by z00397989 AR-0000276595-003-016  2017.04.20 */
/*BEGIN  ADD by z00397989 DTS2017051808664  2017.05.23 */
#define  PCIE_SWITCH_METHOD_SET_UPGRADE_STATE           "SetPcieSwitchUpgradeState"
/*END  ADD by z00397989 DTS2017051808664  2017.05.23 */

#define  PCIE_RETIMER_CLASEE_NAME                         "PcieRetimer"
#define  PROPERTY_PCIE_RETIMER_EXPECT_DEVICE_ID           "ExpectDeviceId"
#define  PROPERTY_PCIE_RETIMER_DEVICE_ID                  "DeviceId"
#define  PROPERTY_PCIE_RETIMER_DEVICE_ID_SRC              "DeviceIdAccessor"
#define  PROPERTY_PCIE_RETIMER_PRESENT                    "Present"
#define  PROPERTY_PCIE_RETIMER_VERSION                    "Version"
#define  PROPERTY_PCIE_RETIMER_UNITNUM                    "UnitNum"
#define  PROPERTY_PCIE_RETIMER_DEF_UPGRADE_FILE           "DefUpgradeFile"
#define  PROPERTY_PCIE_RETIMER_RETIMERID                  "RetimerId"
#define  PROPERTY_PCIE_RETIMER_PWRON_UPGRADEACCESSOR      "PwrOnUpgradeAccessor"
#define  PROPERTY_PCIE_RETIMER_PWROFF_UPGRADEACCESSOR     "PwrOffUpgradeAccessor"
#define  PROPERTY_PCIE_RETIMER_PCIERETIMERACTIONOBJ       "PcieRetimerActionObj"
#define  PROPERTY_PCIE_RETIMER_UPGRADESTATE               "UpgradeState"
/* ADD By z00382105, 生效模式 ValidMode: 0:无动作；1:X86复位；2：X86下电再上电复位 */
#define  PROPERTY_PCIE_RETIMER_VLIDMODE                   "ValidMode"
/* ADD By z00382105, Retimer属于哪个节点，如果复位本板填写0xff,非本板填写对应的单板ID */
#define  PROPERTY_PCIE_RETIMER_NODEID                     "NodeId"
#define  PROPERTY_PCIE_RETIMER_DUMP_REG                   "PcieRetimerDumpReg"
#define  PROPERTY_PCIE_RETIMER_LOADSTATUS                 "LoadStatus"
#define  PROPERTY_PCIE_RETIMER_UPGRADING                  "Upgrading"
#define  PROPERTY_PCIE_RETIMER_LOAD_TRIG                  "LoadTrigger"
#define  PROPERTY_PCIE_RETIMER_POWERSTATE                 "PRTPowerstate"
#define  PROPERTY_PCIE_RETIMER_DELAYTIMEBEFOREUPGRADE     "DelayTimeBeforeUpgrade"
#define  PCIE_RETIMER_METHOD_WRITE_RETIMER_CFG            "WritePCIeRetimerCfg"
#define  PCIE_RETIMER_METHOD_UPGRADE_PSWEEP               "UpgradePCIERetimerEEPROM"
#define  PCIE_RETIMER_METHOD_SET_UPGRADE_STATE            "SetPcieRetimerUpgradeState"


#define  PCIE_SWITCH_MGNT_CFG_CLASS                     "PcieSwitchMgntCfg"
#define  PROPERTY_PCIE_SWITCH_MGNT_CFG_CONDITION1       "Condition1"
#define  PROPERTY_PCIE_SWITCH_MGNT_CFG_CONDITION2       "Condition2"
#define  PROPERTY_PCIE_SWITCH_MGNT_CFG_CONDITION3       "Condition3"
#define  PROPERTY_PCIE_SWITCH_MGNT_CFG_CONDITION4       "Condition4"
#define  PROPERTY_PCIE_SWITCH_MGNT_CFG_REF_REG_OBJ      "RefRegObj"
#define  PROPERTY_PCIE_SWITCH_MGNT_CFG_UPGRADE_FILE1    "UpdateFileName1"
#define  PROPERTY_PCIE_SWITCH_MGNT_CFG_UPGRADE_FILE2    "UpdateFileName2"
#define  PROPERTY_PCIE_SWITCH_MGNT_CFG_SWITCHID         "SwitchId"

/* BEGIN ADD by z00382105 */
#define  PCIE_RETIMER_MGNT_CFG_CLASS                     "PcieRetimerMgntCfg"
#define  PROPERTY_PCIE_RETIMER_MGNT_CFG_REF_REG_OBJ      "RefRegObj"
#define  PROPERTY_PCIE_RETIMER_MGNT_CFG_UPGRADE_FILE1    "UpdateFileName1"
#define  PROPERTY_PCIE_RETIMER_MGNT_CFG_UPGRADE_FILE2    "UpdateFileName2"
#define  PROPERTY_PCIE_RETIMER_MGNT_CFG_RETIMERID         "RetimerId"
/* END ADD by z00382105 */
/* BEGIN ADD by z00382105 */
#define  PCIE_RETIMER_DUMP_REG_CLASS                          "PcieRetimerDumpReg"
#define  PROPERTY_PCIE_RETIMER_DUMP_REG_DEVVICE_REG_ADDR      "DeviceRegAddr"
#define  PROPERTY_PCIE_RETIMER_DUMP_REG_MC_BASE_ADDR          "MCBaseAddr"
#define  PROPERTY_PCIE_RETIMER_DUMP_REG_MC_REG_OFFSET         "MCRegOffset"
/* BEGIN: 2018-3-13 z00416979 AR.SR.SFEA02119720.021.004 */
#define  PROPERTY_PCIE_RETIMER_DUMP_SERDES_NUMBER             "SerDesNum"
#define  PROPERTY_PCIE_RETIMER_DUMP_REG_PHY_LANE_BASE_ADDR    "PhyLaneBaseAddr"
#define  PROPERTY_PCIE_RETIMER_DUMP_REG_PHY_LANE_REG_OFFSET   "PhyLaneRegOffset"
/* END:   2018-3-13 z00416979 */

/* END ADD by z00382105 */
#define  MATCH_REG_VALUE_CLASS                         "MatchRegValue"
#define  PROPERTY_MATCH_REG_VALUE_REF_ACCESSOR         "RefAccessor"
#define  PROPERTY_MATCH_REG_VALUE_EXPECT_VALUE         "ExpectValue"
#define  PROPERTY_MATCH_REG_VALUE_IS_TRUE              "IsTrue"

#define  PROPERTY_ACCESSOR_OFFSET                      "Offset"
#define  PROPERTY_ACCESSOR_MASK                        "Mask"

#define  PROPERTY_ACCESSOR_CHIP                        "Chip"
/* BEGIN: Added by zhongqiu 00356691, 2017/6/20   PN:AR-支持G5500 拓扑编排需求*/
/*编排控制类*/
#define CLASS_NAME_COMPOSITION                         "Composition"
#define PROPERTY_COMPOSITION_ID                        "Id"
/* BEGIN: Added by 00356691 zhongqiu, 2017/11/22   问题单号:DTS2017111306556 拓扑信息收集增强 */
#define PROPERTY_COMPOSITION_SLOT                      "Slot"                   /*编排配置控制的槽位*/
/* END: Added by 00356691 zhongqiu, 2017/11/22   问题单号:DTS2017111306556 拓扑信息收集增强 */
/* BEGIN: Added by 00381887 dingzhixiong, 2018/4/12   问题单号: 装备切拓扑立即生效 */
#define PROPERTY_COMPOSITION_IS_ACTIVATE_IMMEDIATELY   "IsActivateImmediately"  /*编排配置控制的槽位*/
/* END: Added by 00381887 dingzhixiong, 2018/4/12   问题单号:  装备切拓扑立即生效 */
#define PROPERTY_COMPOSITION_PENDDING_SETTING          "PendingSettingId"         /*待生效的编排模式*/
#define PROPERTY_COMPOSITION_CURRENT_SETTING           "CurrentSettingId"         /*当前生效的模式*/
#define PROEPRTY_COMPOSITION_DEFAULT_SETTING_ID        "DefaultSettingId"       /*默认拓扑配置*/
/* BEGIN: Added by 00356691 zhongqiu, 2017/11/24   PN:AR.SR.SFEA02119723.009.001 支持节点配置机框拓扑*/
#define PROPERTY_COMPOSITION_TOPO_CFG_STATUS           "TopologyConfigStatus"   /*拓扑配置状态*/
/* END:   Added by 00356691 zhongqiu, 2017/11/24   PN:AR.SR.SFEA02119723.009.001 支持节点配置机框拓扑*/
#define PROPERTY_COMPOSITION_IS_SETTING_FROM_XML        "IsSettingFromXML"      /* 拓扑配置数据来源 */
#define METHOD_COMPOSITION_CONFIG_COMPOSITION          "ConfigComposition"      /*配置编排模式方法*/
/* BEGIN: Added by 00381887 dingzhixiong, 2018/4/12   问题单号: 装备切拓扑立即生效 */
#define METHOD_COMPOSITION_CONFIG_ACTIVATE_TYPE        "ConfigActivateType"      /*配置拓扑生效模式*/
/* END: Added by 00381887 dingzhixiong, 2018/4/12   问题单号:  装备切拓扑立即生效 */

/* BEGIN: Added by 00356691 zhongqiu, 2017/11/28   PN:AR.SR.SFEA02119723.009.001 支持节点配置机框拓扑*/
#define CLASS_NAME_PERIPHERAL_COMPOSITION                         "PeripheralComposition"  /*拓扑外围同步类*/
#define PROPERTY_PERIPHERAL_COMPOSITION_ID                        PROPERTY_COMPOSITION_ID
#define PROPERTY_PERIPHERAL_COMPOSITION_SLOT                      PROPERTY_COMPOSITION_SLOT
#define PROPERTY_PERIPHERAL_COMPOSITION_IS_ACTIVATE_IMMEDIATELY   PROPERTY_COMPOSITION_IS_ACTIVATE_IMMEDIATELY
#define PROPERTY_PERIPHERAL_COMPOSITION_PENDDING_SETTING          PROPERTY_COMPOSITION_PENDDING_SETTING
#define PROPERTY_PERIPHERAL_COMPOSITION_CURRENT_SETTING           PROPERTY_COMPOSITION_CURRENT_SETTING
#define PROEPRTY_PERIPHERAL_COMPOSITION_DEFAULT_SETTING_ID        PROEPRTY_COMPOSITION_DEFAULT_SETTING_ID
#define PROPERTY_PERIPHERAL_COMPOSITION_TOPO_CFG_STATUS           PROPERTY_COMPOSITION_TOPO_CFG_STATUS
#define PROPERTY_PERIPHERAL_COMPOSITION_TARGET                    "Target"
#define METHOD_PERIPHERAL_COMPOSITION_CONFIG_COMPOSITION          METHOD_COMPOSITION_CONFIG_COMPOSITION
/* END:   Added by 00356691 zhongqiu, 2017/11/28   PN:AR.SR.SFEA02119723.009.001 支持节点配置机框拓扑*/


/*编排模式类*/
#define CLASS_NAME_COMPOSITION_SETTING                 "CompositionSetting"
#define PROPERTY_COMPOSITION_SETTING_ID                "Id"                     /*编排模式Id*/
#define PROPERTY_COMPOSITION_SETTING_IS_ACTIVE         "IsActive"               /*当前编排模式是否启用中*/
#define PROPERTY_COMPOSITION_SETTING_AVAILABLE         "SettingAvaliable"       /*当前编排模式是否可用*/
#define PROPERTY_COMPOSITION_SETTING_ZONES             "Zones"                  /*编排模式中包含的Zone*/
#define PROPERTY_COMPOSITION_SETTING_TAKE_EFFECT_TYPE  "TakeEffectType"         /*编排模式生效方式*/
#define PROPERTY_COMPOSITION_SETTING_CONFIG_APPLY_OBJ  "ConfigApplyObj"         /*配置生效的对象*/

/*编排Zone*/
#define CLASS_NAME_COMPOSITION_ZONE                    "CompositionZone"
#define PROPERTY_COMPOSITION_ZONE_ID                   "Id"                     /*编排模式Id*/
#define PROPERTY_COMPOSITION_SETTING_IDS               "SettingIds"             /*Zone相关的配置模式Id*/
#define PROPERTY_UPSIDE_ENDPOINTS                      "UpsideEndpoints"        /*上行Endpoint*/
#define PROPERTY_UPSIDE_SWITCH_IDS                     "UpsideSwitchIds"        /*上行Switch编号*/
#define PROPERTY_UPSIDE_SWITCH_TYPES                   "UpsideSwitchType"       /*上行交换类型*/
#define PROPERTY_UPSIDE_PORT_IDS                       "UpsidePortIds"          /*上行口*/
#define PROPERTY_UPSIDE_PORT_WIDTH                     "UpsidePortWidth"        /*上行口带宽*/
#define PROPERTY_DOWNSIDE_ENDPOINTS                    "DownsideEndpoints"      /*下行Endpoint*/
#define PROPERTY_DOWNSIDE_SWITCH_IDS                   "DownsideSwitchIds"      /*下行交换编号*/
#define PROPERTY_DOWNSIDE_SWITCH_TYPES                 "DownsideSwitchType"     /*下行交换类型*/
#define PROPERTY_DOWNSIDE_PORT_IDS                     "DownsidePortIds"        /*下行口*/
#define PROPERTY_DOWNSIDE_PORT_WIDTH                   "DownsidePortWidth"      /*下行口带宽*/

/*Endpoint标识*/
#define CLASS_NAME_ENDPOINT_IDENTIFIER                 "EndpointIdentifier"
#define PROPERTY_NAME_EP_IDENTIFIER_ID                 "Id"                     /*Endpoint标识Id*/
#define PROPERTY_NAME_EP_IDENTIFIER_SHORT_NAME         "ShortName"              /*Endpoint短名称*/
#define PROPERTY_NAME_EP_IDENTIFIER_PRESENCE           "Presence"               /*Endpoint在位信息*/
#define PROPERTY_NAME_EP_IDENTIFIER_POSITION           "Position"               /*Endpoint位置信息*/
#define PROPERTY_NAME_EP_CONTAINER                     "Container"              /*Endpoint容器信息*/
#define PROPERTY_NAME_EP_CONTAINER_PRESENCE            "ContainerPresence"      /*Endpoint容器在位信息*/
/* BEGIN: Added by 00356691 zhongqiu, 2017/11/23   PN:AR.SR.SFEA02119723.009.001 支持节点配置机框拓扑*/
#define PROPERTY_NAME_EP_IDENTIFIER_DOMAIN_ID                     "DomainId"               /*Endpoint归属域Id*/
/* END:   Added by 00356691 zhongqiu, 2017/11/23   PN:AR.SR.SFEA02119723.009.001 支持节点配置机框拓扑*/
#define PROPERTY_NAME_EP_IDENTIFIER_VISIBLE            "Visible"

/*可编排部件信息*/
#define CLASS_NAME_COMPOSITION_ENDPOINT                "CompositionEndpoint"
#define PROPERTY_COMPOSITION_ENDPOINT_ID               "Id"                     /*Endpoint Id*/
#define PROPERTY_COMPOSITION_ENDPOINT_SHORT_NAME       "ShortName"              /*Endpoint短名称*/
#define PROPERTY_COMPOSITION_ENDPOINT_SLOT             "Slot"                   /*Endpoint槽位*/
#define PROPERTY_COMPOSITION_ENDPOINT_HEALTH           "Health"                 /*Endpoint健康状态*/
#define PROPERTY_COMPOSITION_ENDPOINT_MODEL            "Model"                  /*Endpoint型号*/
#define PROPERTY_COMPOSITION_ENDPOINT_DESCRIPTION      "Description"            /*Endpoint描述*/
#define PROPERTY_COMPOSITION_ENDPOINT_MANUFACTURE      "Manufacture"            /*Endpoint厂商*/
#define PROPERTY_COMPOSITION_ENDPOINT_SUB_VENDOR_INFO  "SubVendorInfo"          /*Endpoint附加厂商信息*/
#define PROPERTY_COMPOSITION_ENDPOINT_SUB_DEVICE_INFO  "SubDeviceInfo"          /*Endpoint附加设备信息*/
#define PROPERTY_COMPOSITION_ENDPOINT_VID              "VenderId"               /*Endpoint厂商Id*/
#define PROPERTY_COMPOSITION_ENDPOINT_DID              "DeviceId"               /*Endpoint设备Id*/
#define PROPERTY_COMPOSITION_ENDPOINT_SUB_VID          "SubVenderId"            /*Endpoint厂商Sub Id*/
#define PROPERTY_COMPOSITION_ENDPOINT_SUB_DID          "SubDeviceId"            /*Endpoint设备Sub Id*/
/* BEGIN: Added by 00356691 zhongqiu, 2017/12/14   PN:AR.SR.SFEA02119723.009.001 支持节点配置机框拓扑*/
#define PROPERTY_COMPOSITION_ENDPOINT_PROTOCOL         "Protocol"               /*Endpoint协议*/
#define PROPERTY_COMPOSITION_ENDPOINT_ENTITY_TYPE      "EntityType"             /*Endpoint类型，见class.xml中的描述*/
#define PROPERTY_COMPOSITION_ENDPOINT_REF_COMP         "RefComponent"           /*引用部件句柄*/
/* END:   Added by 00356691 zhongqiu, 2017/12/14   PN:AR.SR.SFEA02119723.009.001 支持节点配置机框拓扑*/


/* END:   Added by zhongqiu 00356691, 2017/6/20   PN:AR-支持G5500 拓扑编排需求*/

/* BEGIN: Added by 00356691 zhongqiu, 2017/7/17   PN:支持SMM加载PCIe标卡*/
/*SMM PCIe卡同步对象*/
#define CLASS_NAME_SMM_PCIEINFO                       "SMMPCIeInfo"
#define PROPERTY_SMM_PCIEINFO_CONTAINER_TYPE          "ContainerType"
#define PROPERTY_SMM_PCIEINFO_CONTAINER_SLOT          "ContainerSlot"

#define CLASS_NAME_NODE_PCIEINFO_SUMMARY              "NodePCIeInfoSummary"
#define PROPERTY_NODE_PCIEINFO_SUMMARY_ID             "Id"
#define PROPERTY_NODE_PCIEINFO_SUMMARY_PCIEINFO_ARRAY "PCIeInfoArray"
#define PROEPRTY_NODE_PCIEINFO_SUMMARY_CONNECTORS     "PCIeContainerConnectors"
/* END:   Added by 00356691 zhongqiu, 2017/7/17   PN:支持SMM加载PCIe标卡*/

// END by h00272616  AR2C9A4135 20151113

/* security_policy模块类 */
#define TPCM_CLASS_BASEINFO            "TPCMBaseInfo"
#define TPCM_OBJECT_BASEINFO           "TPCMBaseInfo"
#define PROPERTY_TPCM_TCM_MANUFACTURER "TCMManufacturer"
#define PROPERTY_TPCM_TCM_VERSION      "TCMVersion"
#define PROPERTY_TPCM_LOG_VERSION      "TPCMLogVersion"
#define PROPERTY_TPCM_TSB_ENABLE_STATUS      "TsbEnableStatus"
#define METHOD_TPCM_GETTPCMSPPORTSTATE "GetTpcmSupportState"

#define TPCM_CLASS_CONFIG              "TPCMConfig"
#define PROPERTY_TPCM_SERVER_ADDR      "TrustedCenterServerAddr"
#define PROPERTY_MEASURE_ENABLE        "MeasureEnable"
#define PROPERTY_CONTROL_ACTION        "ControlAction"
#define METHOD_TPCM_SET_TSB_IP         "SetTPCMTsbIp"
#define METHOD_SET_TPCM_TSB_SWITCH     "SetTPCMTsbEnable"
#define METHOD_SET_TPCM_TSB_ACTION     "SetTPCMTsbAction"
#define TPCM_CLASS_MEASURED            "TPCMMeasured"
#define TPCM_OBJECT_L1FW               "TPCML1FW"
#define TPCM_OBJECT_UBOOT              "TPCMUBOOT"
#define TPCM_OBJECT_ROOTFS             "TPCMROOTFS"
#define TPCM_OBJECT_BIOS               "TPCMBIOS"
#define TPCM_OBJECT_SHIM               "TPCMSHIM"
#define TPCM_OBJECT_GRUB               "TPCMGRUB"
#define TPCM_OBJECT_OS_CFG             "TPCMOSCfg"
#define TPCM_OBJECT_OS_KERNEL          "TPCMOSKernel"
#define TPCM_OBJECT_OS_INITRD          "TPCMOSInitrd"
#define TPCM_OBJECT_RESERVED           "Reserved"
#define PROPERTY_TPCM_MEASURED_OBJECT  "MeasuredObject"
#define PROPERTY_TPCM_OBJECT_VERSION   "ObjectVersion"
#define PROPERTY_TPCM_VERSION          "Version"
#define PROPERTY_TPCM_PCR_NAME         "PCRName"
#define PROPERTY_TPCM_ACTUAL_DIGEST    "ActualDigest"
#define PROPERTY_TPCM_STANDARD_DIGEST  "StandardDigest"
#define PROPERTY_TPCM_COMP_RESULT      "CompResult"
#define PROPERTY_TPCM_CTRL_COMMAND     "CtrlCommand"
#define METHOD_SET_STANDARD_DIGEST     "SetStandardDigest"
#define METHOD_SET_BIOS_DIGEST         "SetBIOSDigest"

/* BMC_GLOBAL模块对象及属性 */
#define  BMC_CLASEE_NAME                   "BMC"
#define  BMC_OBJECT_NAME                   BMC_CLASEE_NAME
#define  BMC_DEVICE_ID_NAME                ("DeviceId")
#define  BMC_DEVICE_REVI_NAME              ("DeviceRevision")
#define  BMC_FM_MAJOR_NAME                 ("PMEMajorVer")
#define  BMC_FM_MINOR_NAME                 ("PMEMinorVer")
#define  BMC_RE_MAJOR_NAME                 ("ReleaseMajorVer")   /* release拉分支时的版本号属性 */
#define  BMC_RE_MINOR_NAME                 ("ReleaseMinorVer")
#define  BMC_BACKUP_MANA_VER_NAME          ("PMEBackupVer")
#define  BMC_MANU_ID_NAME                  ("ManufactureId")
#define  BMC_CUSTOM_MANU_ID_NAME           ("CustomManufactureId")
#define  BMC_IPMC_CPU_VER_NAME             ("CpuType")
#define  BMC_IPMI_VER_NAME                 ("IpmiVersion")
#define  BMC_MANA_VER_NAME                 "PMEVer"
#define  BMC_MANA_BUILT_VER_NAME           ("PMEBuilt")
#define  BMC_UBOOT_VER_NAME                ("UbootVersion")
#define  BMC_DRIVER_VER_NAME               ("DriverVersion")
#define  BMC_TIME_ZONE_NAME                "TimeZone"
#define  BMC_TIME_ZONE_STR_NAME            "TimeZoneStr"
#define  BMC_DST_ENABLE_NAME               "DSTEnable"
#define  BMC_DEV_GUID_NAME                 "DeviceGuid"
#define  BMC_SYS_GUID_NAME                 ("SystemGuid")
#define  BMC_HOST_NAME                     ("HostName")
#define  BMC_RESET_CAUSE_NAME              ("ResetCause")
#define  BMC_SOFT_RESET_FLAG_NAME          ("SoftResetFlag")
#define  BMC_DRIVER_BUILT_NAME             ("DriverBuilt")
#define  BMC_CPLD_VER_NAME                 ("CpldVersion")
#define  BMC_CERT_OVER_DUE                 ("CertOverDueStatus")
#define  BMC_SOFTWARE_NAME                 ("SoftwareName")
#define  BMC_CUSTOM_MANUFACTURER           ("CustomManufacturer")
#define  BMC_CUSTOM_ALARM_NAME             ("CustomAlarmName")
#define  BMC_CUSTOM_LOGIBMA_NAME           ("CustomLogIbmaName")
#define  BMC_CUSTOM_LOGIBMC_NAME           ("CustomLogIbmcName")
#define  PROPERTY_BMC_WARN_PERCENT         ("WarnPercent")
#define  PROPERTY_BMC_FLASH_WARN           ("FlashUseWarn")
#define  PROPERTY_PME_VER_NAME             ("PMEVer")
#define  PROPERTY_PME_STARTFALG_NAME       ("BmcStartFlag")
#define  PROPERTY_BMC_UNIT_NUM             ("FlashUnitNum")
#define  PROPERTY_CPLD_UNIT_NUM            ("CpldUnitNum")
#define  PROPERTY_UBOOT_BACKUP_VER         ("UbootBackupVer")
#define  PROPERTY_REMOTE_OEM_INFO          ("RemoteOEMInfo")
#define  PROPERTY_SHARE_DATA_INFO          ("ShareDataInfo")
#define  PROPERTY_TIME_SYNC_SUCCESS        ("TimeSyncSuccess")
#define  PROPERTY_BMC_RESET_COUNT          ("RestCnt")
/* BEGIN: Added by zengrenchang(z00296903), 2017/3/14   问题单号:AR-0000276591-001-001*/
#define  PROPERTY_BLADE_MANAGE_MODE        ("BladeManageMode")
/* END:   Added by zengrenchang(z00296903), 2017/3/14 */
/* BEGIN: Added by linzhen(l00389091), 2017/11/27   DTS2017101708581*/
#define  PROPERTY_BMC_CPLD_VER_REG         ("CpldVerReg")
#define  PROPERTY_BMC_DEVICE_UNLOCK_RESET   "DeviceUnlockReset"

/* BEGIN: Added by h00256973, 2019/6/24   支持1711*/
#define  PROPERTY_GET_BMCVER_SCRIPT             ("GetBMCVerScript")
#define  PROPERTY_BMC_AVAILABLE_VER             ("BMCAvailableVer")
#define  PROPERTY_AVAILABLE_VER_BUILD_NUMBER    ("AvailableVerBuildNumber")
#define  PROPERTY_L0_FW_VERSION                 ("L0FWVersion")
#define  PROPERTY_L0_FW_BACKUP_VER              ("L0FWBackupVer")
#define  PROPERTY_L0_FW_BUILT                   ("L0FWBuilt")
#define  PROPERTY_L0_FW_BACKUP_VER_BUILT        ("L0FWBackupVerBuilt")
#define  PROPERTY_L1_FW_VERSION                 ("L1FWVersion")
#define  PROPERTY_L1_FW_BACKUP_VER              ("L1FWBackupVer")
#define  PROPERTY_L1_FW_BUILT                   ("L1FWBuilt")
#define  PROPERTY_L1_FW_BACKUP_VER_BUILT        ("L1FWBackupVerBuilt")
/* END:   Added by h00256973, 2019/6/24   支持1711 */

#define CLASS_NAME_BMC                                     BMC_CLASEE_NAME   /* BMC 类名称 */
#define OBJ_NAME_BMC                                       BMC_OBJECT_NAME   /* BMC 类名称 */
#define PROPERTY_BMC_DEV_SERIAL                            "DeviceSerialNumber"  /*设备 序列号*/
#define PROPERTY_BMC_DEV_NAME                              BMC_HOST_NAME  /*设备名称*/
#define PROPERTY_BMC_TIMEZONE                              BMC_TIME_ZONE_NAME
#define PROPERTY_BMC_TIMEZONE_STR                          BMC_TIME_ZONE_STR_NAME
#define PROPERTY_BMC_DST_ENABLE                       BMC_DST_ENABLE_NAME  /* 夏令时使能状态 */
#define PROPERTY_BMC_CPUTYPE                               BMC_IPMC_CPU_VER_NAME
#define PROPERTY_BMC_IPMIVER                               BMC_IPMI_VER_NAME
#define PROPERTY_BMC_CPLDVER                               BMC_CPLD_VER_NAME
/* BEGIN: Added by c00149416, DTS2017021500425  2017/2/16 */
#define PROPERTY_BMC_M3VER                                 BMC_M3_VER_NAME
/* END:   Added by c00149416, 2017/2/16 */
#define PROPERTY_BMC_CPLDUNITNUM                           PROPERTY_CPLD_UNIT_NUM
#define PROPERTY_BMC_FLASHUNITNUM                          PROPERTY_BMC_UNIT_NUM
#define PROPERTY_BMC_BAKPMEVER                             BMC_BACKUP_MANA_VER_NAME    //字符串  Imana备份镜像版本
#define PROPERTY_BMC_UOOTVER                               BMC_UBOOT_VER_NAME  //Uboot版本
#define PROPERTY_BMC_UOOTBACKUPVER                         PROPERTY_UBOOT_BACKUP_VER  //Uboot版本
#define PROPERTY_BMC_DRIVERVER                             BMC_DRIVER_VER_NAME
#define PROPERTY_BMC_DRIVERVERBUILTTIME                    BMC_DRIVER_BUILT_NAME
#define PROPERTY_BMC_PMEBUILTTIME                          BMC_MANA_BUILT_VER_NAME //编译时间
#define PROPERTY_BMC_PMEMAJORVER                           BMC_FM_MAJOR_NAME   //固件主版本号
#define PROPERTY_BMC_PMEMINORVER                           BMC_FM_MINOR_NAME  //固件次版本号
#define PROPERTY_BMC_PMEAUXMAJORVER                        BMC_RE_MAJOR_NAME  // 固件辅助主版本号
#define PROPERTY_BMC_PMEAUXMINORVER                        BMC_RE_MINOR_NAME  // 固件辅助次版本号

#define PROPERTY_BMC_ELABELMANUFORMAT                      "ElabelManuFormat"
#define PROPERTY_BMC_SUPPORT_ECO_FT                           "AuxVersion"  //是否支持ECO_FT
#define PROPERTY_BMC_SYSTEMNAME                            "SystemName"
#define PROPERTY_BMC_GUID                                  BMC_SYS_GUID_NAME
#define PROPERTY_BMC_BOM_VER                               "BomVer"  //BOM 版本
#define METHOD_BMC_GETTIME                                 "GetTime"
#define METHD_BMC_REBOOTPME                                "RebootPme"
#define PROPERTY_BMC_BUILD_NO                              "BuildNumber"  //版本构建编译版本号
/** BEGIN: Added by l00272597, 2016/10/8   PN:AR0000256853-001-002  */
#define PROPERTY_BMC_DIEID                                 "DIEID"        // BMC芯片DIEID
#define PROPERTY_BMC_CHIP_NAME                             "ChipName"     // BMC芯片名称
#define PROPERTY_BMC_CHIP_VERSION                          "ChipVersion"  // BMC芯片版本
#define PROPERTY_BMC_SECURITY_ENABLE                       "SecurityEnable"     // BMC芯片安全使能
#define  METHOD_BMC_SET_DIEID                              "SetDIEID"
/** END:   Added by l00272597, 2016/10/8 */
/* BEGIN by t00381753  PN:AR-0000276590-008-001*/
#define PROPERTY_BMC_LANGUAGESET                           "LanguageSet"  // BMC支持的基础语言（zh,en,fr,ja）集合 替换(制作白牌包)
/* END by t00381753  PN:AR-0000276590-008-001*/

#define PROPERTY_BMC_LANGUAGESET_APPEND                           "LanguageSetAppend"  //BMC支持的语言集合 追加（语言包升级）
#define PROPERTY_BMC_LANGUAGESET_EXTEND                           "LanguageSetExtend"  // BMC支持的扩展语言集合(ru,de,xx)
#define PROPERTY_BMC_LANGUAGESET_SUPPORT                          "LanguageSetSupport" // BMC支持的所有语言集合

#define PROPERTY_BMC_CERT_ALGORITHM                        "CertAlgorithm"  //BMC默认使用的证书算法
#define METHOD_BMC_SET_CERT_ALGORITHM                      "SetCertAlgorithm"   //设置BMC默认使用的证书算法
#define PROPERTY_BMC_FIRST_UPDATE_MASTERKEY_FLAG     "FirstUpdateMasterKeyFlag" /* 首次启动更新masterkey标志位 */
#define PROPERTY_BMC_CLI_SESSION_TIMEOUT              "CLISessionTimeOut"

#define  CLASS_BMC                         BMC_CLASEE_NAME         /* BMC全局类 */
#define  OBJECT_BMC                        BMC_OBJECT_NAME         /* BMC全局对象 */
#define  METHOD_BMC_SET_MFG_FORMAT         "SetMfgFormat"          /* 设置Fru生产日期时间戳格式 */
#define  METHOD_BMC_SET_TIME               "SetTime"               /* 设置BMC时间 */
#define  METHOD_BMC_TIMEZONE_STR           "SetTimeZoneStr"        /* 设置BMC时区 */
#define  METHOD_BMC_SET_DST_ENABLE         "SetDSTEnable"          /* 设置夏令时是否使能 */
#define  METHOD_SYNC_SET_TIMEZONE          "SyncSetTimeZone"     /* 远程同步设置时区 */
#define  METHOD_SYNC_SET_BMC_PROPERTY      "SyncPropertyBMC"       /*同步BMC属性*/
#define  METHOD_UPDATE_IRM_COMM_TIMESTAMP "UpdateiRMCommTimeStamp"
#define METHOD_RECORD_FILE_OWNER "RecordFileOwner"
#define METHOD_GET_FILE_OWNER "GetFileOwner"
#define METHOD_UPDATE_BMC_BUILD_TIME "UpdateBmcBuildTime"

/* BEGIN: Modified by zwx382755, 2018/9/17   PN:AR.SR.SFEA02130924.047.001 */
#define  METHOD_BMC_SET_UUID               "SetUUID"
/* END:   Modified by zwx382755, 2018/9/17 */
#define  METHOD_BMC_CLEAR_TMP_FILE         "ClearTmpFile"          /* 清除TMP目录文件 */
#define  METHOD_BMC_SET_PME_VER            "SetPmeBackupVer"       /* 设置PME备份版本 */
#define  METHOD_BMC_SET_NOTIMEOUT         "SetNotimeout"          /*设置notimeout*/
#define  METHOD_SET_REMOTE_OEM_INFO        "SetRemoteOEMInfo"
#define  METHOD_EXPORT_ALL_CONFIG            "ExportConfig"
#define  METHOD_IMPORT_ALL_CONFIG            "ImportConfig"
#define  METHOD_GET_IMPORT_EXPORT_STATUS     "GetImportExportStatus"
/* BEGIN by t00381753  PN:AR-0000276590-008-001*/
#define  METHOD_SET_LANGUAGESET              "SetLanguageSet"
/* END by t00381753  PN:AR-0000276590-008-001*/
#define METHOD_GET_TIMEZONE_LIST           "GetTimeZoneList"
#define METHOD_GET_CUR_TIME_OFFSET         "GetCurTimeOffset"
#define METHOD_GET_AREA_TIMEZONE_LIST      "GetAreaTimeZoneList"

#define  METHOD_MOUNT_COMMAND              "MountCommand"
#define  METHOD_UMOUNT_COMMAND             "UmountCommand"

/* BEGIN: Added by zengrenchang(z00296903), 2017/3/14   问题单号:AR-0000276591-001-001*/
#define  METHOD_SET_BLADE_MANAGE_MODE       "SetBladeManageMode"
/* END:   Added by zengrenchang(z00296903), 2017/3/14 */

#define  METHOD_BMC_SET_CUSTOM_ALARM         "SetCustomAlarm"    /* 设置CustomAlarm */

/* BEGIN: Added by lwx282797  2017/7/24   问题单号:AR-0000276594-001-023*/
#define  METHOD_UNINSTALL_LANGUAGE_RESOURCE         "UninstallLanguageResource"
/* END:   Added by lwx282797  2017/7/24 */

/* 主机域名*/         //added by zhangheng
#define PROPERTY_BMC_HOSTNAME                                    "HostName"
#define METHOD_BMC_HOSTNAME                                      "SetHostName"
#define METHOD_BMC_SET_LOCATION_ID                               "SetLocationID"

/* 远程管理标识*/
#define PROPERTY_BMC_LOCATION_INFO                                    "LocationInfo"
#define METHOD_BMC_SET_LOCATION_INFO                                      "SetLocationInfo"

/* BEGIN: DTS2018012214534 . Modified by f00381872, 2018/1/24 */
/*拷贝文件，解决非常驻进程中拷贝文件权限不足问题*/
#define METHOD_BMC_COPY_FILE                        "CopyFile"
/* END: DTS2018012214534 . Modified by f00381872, 2018/1/24 */

/* BEGIN: Added by h00256973,  for DICE 挑战 2019/7/27*/
#ifdef ARM64_HI1711_ENABLED   // 1711的版本需要
#define METHOD_EXPORT_DICE_CSR                    "ExportDiceCSR"
#define METHOD_IMPORT_DICE_CERT                   "ImportDiceCert"
#define METHOD_EXPORT_DICE_CERT_CHAIN             "ExportDiceCertChain"
#define METHOD_GET_DICE_CERT_CHAIN_EXPORT_STATE   "GetDiceCertChainExportState"
#define METHOD_GET_PARTNER_CERT_HASH              "GetPartnerCertHash"
#define METHOD_GET_PARTNER_MODE_ENABLE            "GetPartnerModeEnable"
#endif
/* END: Added by h00256973,  for DICE 挑战 2019/7/27*/

/* SSL模块对象及属性 */
#define  SSL_CLASEE_NAME                   "SSL"
#define  SSL_OBJECT_NAME                   SSL_CLASEE_NAME
#define  SSL_OBJECT_OWNER                ("OwnerSSL")
#define  SSL_OBJECT_ISSUER                ("IssuerSSL")
#define  SSL_COUNRTY                          ("Country")
#define  SSL_STATE                               ("State")
#define  SSL_LOCATION                             ("Location")
#define  SSL_ORG_NAME                        ("OrgName")
#define  SSL_ORG_UNIT                         ("OrgUnit")
#define  SSL_COMMON_NAME                 ("CommonName")
#define  SSL_START_TIME                         ("StartTime")
#define  SSL_EXPIRATION                       ("Expiration")
#define  SSL_SN                                       ("SerialNumber")
#define  SSL_SIG_ALGO                           ("SigAlgorithm")
#define  SSL_KEY_USAGE                          ("KeyUsage")
#define  SSL_KEY_LENGTH                         ("KeyLength")

#define CLASS_PRE_SSL_SERVER_INFO    "PreSSLSeverInfo"

/* BEGIN: Added by zhongqiu 00356691, 2016/11/24   PN:AR-0000267460-001-003*/
#define  SSL_CERT_TYPE                        ("CertType")
#define  SSL_CERT_ID                          ("CertId")
#define  SSL_SUBJECT_INFO                     ("SubjectInfo")
#define  SSL_ISSUER_INFO                      ("IssuerInfo")
#define  SSL_METHOD_GET_CSR_STRING            ("GetCSRString")
/* END:   Added by zhongqiu 00356691, 2016/11/24   PN:AR-0000267460-001-003*/
#define  SSL_CERT_STATUS                   ("CertStatus")

/* BEGIN: Added by z00356691 zhongqiu, 2017/4/5   PN:AR-0000276582-002-045 CSR导出*/
#define SSL_METHOD_EXPORT_CSR                 "ExportCSR"
/* END:   Added by z00356691 zhongqiu, 2017/4/5   PN:AR-0000276582-002-045 CSR导出*/

#define SSL_CERTSIGN_ERROR                 "CertSignError"

/* BEGIN: Added by wangenmao 00365865, 2016/12/20   PN:AR-0000264510-004-001*/
#define CERT_MONITOR_CLASS                        "CertMonitor"
#define CERT_ID                                   "CertID"
#define CERT_DESCRIPTION                          "CertDes"
#define CERT_OVER_STATUS                        "OverDueStatus"
#define CERT_START_TIME                         "StartTime"
#define CERT_EXPIRE_TIME                        "Expiration"
#define METHOD_SETOVERDUE_STATUS        "SetOverDueStatus"
/* END: Added by wangenmao 00365865, 2016/12/20   PN:AR-0000264510-004-001*/

#define  SSL_METHOD_GENERATECSR                 "GenerateCSR"
#define  SSL_METHOD_IMPORTPUBLICKEY                 "ImportPublicKey"
#define  SSL_METHOD_IMPORTCUSTOMCERT                 "ImportCustomCert"
#define  SSL_METHOD_SETCSRREQINFO         "SetCsrReqInfo"
#define  SSL_METHOD_GETCSRSTATUS         ("GetCsrStatus")
#define  SSL_METHOD_GETISCUSTOMEDCERTFLAG         "GetCustomedCertFlag"
#define  SSL_METHOD_SNMP_GENERATECSR         ("SnmpGenerateCSR")
#define  SSL_METHOD_GETCSRSTATE         ("GetCsrState")
#define  SSL_METHOD_SNMP_IMPORTPUBLICKEY         ("SnmpImportPublicKey")
#define  SSL_METHOD_SNMP_SETCUSTOMCERTFILE         ("SetCustomCertFile")
#define  SSL_METHOD_SNMP_SETCUSTOMCERTFILEPASS         ("SetCustomCertFilePass")
#define  SSL_METHOD_SNMP_IMPORTCUSTOMCERT         ("SnmpImportCustomCert")
#define  SSL_ENCRYPT_WORKKEY                      ("SSLWorkKey")
#define  SSL_ENCRYPT_COMPONENT                    ("SSLComponent")
#define  SSL_ENCRYPT_MASTERKEY_ID                 ("SSLMasterKeyId")

#define  SSL_METHOD_ENCRYPT_DECRYPT_CERT          ("EncryptDecrypeCert")
#define  SSL_METHOD_ENCRYPT_CERT           "EncryptCert"
#define  SSL_METHOD_DECRYPT_CERT           "DecryptCert"

#define  MUTUAL_AUTH_CLASS_NAME                                        "MutualAuthentication"
#define  MUTUAL_AUTH_OBJECT_NAME                                        "MutualAuthentication"
#define  MUTUAL_AUTH_STATE                                                    "MutualAuthenticationState"
#define  MUTUAL_AUTH_OCSP                                                     "MutualAuthenticationOCSP"
#define MUTUAL_AUTH_CRL "MutualAuthenticationCRL"
#define  MUTUAL_AUTH_METHOD_SET_STATE                              "SetMutualAuthenticationState"
#define  MUTUAL_AUTH_METHOD_SET_OCSP                               "SetMutualAuthenticationOCSP"
#define MUTUAL_AUTH_METHOD_SET_CRL "SetMutualAuthenticationCRL"
#define  MUTUAL_AUTH_METHOD_AUTHENTICATION                   "AuthClientCertificate"

#define  MUTUAL_AUTH_ROOT_CLASS_NAME                    "MutualAuthenticationRootCert"
#define  MUTUAL_AUTH_ROOT_CERT_ID                                      "CertId"
#define  MUTUAL_AUTH_ROOT_CERT_HASHID                             "hashId"
#define  MUTUAL_AUTH_ROOT_CERT_HASHVALUE                       "hashValue"
#define  MUTUAL_AUTH_ROOT_CERT_ISSUEBY                            "IssueBy"
#define  MUTUAL_AUTH_ROOT_CERT_ISSUEBYCN                        "IssueByCN"
#define  MUTUAL_AUTH_ROOT_CERT_ISSUETO                            "IssueTo"
#define  MUTUAL_AUTH_ROOT_CERT_ISSUETOCN                        "IssueToCN"
#define  MUTUAL_AUTH_ROOT_CERT_VALIDFROM                       "ValidFrom"
#define  MUTUAL_AUTH_ROOT_CERT_VALIDTO                           "ValidTo"
#define  MUTUAL_AUTH_ROOT_CERT_SERIALNUMBER                 "SerialNumber"
#define  MUTUAL_AUTH_ROOT_CERT_KEYUSAGE                     "KeyUsage"
#define  MUTUAL_AUTH_ROOT_CERT_KEYLENGTH                    "KeyLength"
#define  MUTUAL_AUTH_ROOT_CERT_SIGALGO                      "SigAlgorithm"
#define  MUTUAL_AUTH_ROOT_CERT_METHOD_IMPORT             "ImportRootCertificate"
#define  MUTUAL_AUTH_ROOT_CERT_METHOD_DELETE               "DeleteRootCertificate"
#define MUTUAL_AUTH_ROOT_CERT_CRL_FILEPATH "CRLFilePath"
#define MUTUAL_AUTH_ROOT_CERT_METHOD_IMPORT_CRL "ImportCRL"
#define MUTUAL_AUTH_ROOT_CERT_METHOD_DELETE_CRL "DeleteCRL"
#define  MUTUAL_AUTH_ROOT_CERT_METHOD_REINIT               "ReInitRootCertificate"
#define MUTUAL_AUTH_ROOT_CERT_CRL_START_TIME "CRLStartTime"
#define MUTUAL_AUTH_ROOT_CERT_CRL_EXPIRE_TIME "CRLExpireTime"


#define  MUTUAL_AUTH_CLIENT_CLASS_NAME                    "MutualAuthenticationCliCert"
#define  MUTUAL_AUTH_CLIENT_CERT_HASHID                             "hashId"
#define  MUTUAL_AUTH_CLIENT_CERT_HASHVALUE                       "hashValue"
#define  MUTUAL_AUTH_CLIENT_CERT_ISSUERID                           "IssuerId"
#define  MUTUAL_AUTH_CLIENT_CERT_ISSUEBY                            "IssueBy"
#define  MUTUAL_AUTH_CLIENT_CERT_ISSUETO                            "IssueTo"
#define  MUTUAL_AUTH_CLIENT_CERT_VALIDFROM                       "ValidFrom"
#define  MUTUAL_AUTH_CLIENT_CERT_VALIDTO                           "ValidTo"
#define  MUTUAL_AUTH_CLIENT_CERT_SERIALNUMBER                 "SerialNumber"
#define  MUTUAL_AUTH_CLIENT_CERT_KEYUSAGE                     "KeyUsage"
#define  MUTUAL_AUTH_CLIENT_CERT_KEYLENGTH                    "KeyLength"
#define  MUTUAL_AUTH_CLIENT_CERT_SIGALGO                      "SigAlgorithm"
#define  MUTUAL_AUTH_CLIENT_CERT_FINGERPRINT                    "FingerPrint"
#define  MUTUAL_AUTH_CLIENT_CERT_METHOD_IMPORT              "ImportClientCertificate"
#define  MUTUAL_AUTH_CLIENT_CERT_METHOD_DELETE               "DeleteClientCertificate"
#define  MUTUAL_AUTH_CLIENT_CERT_REVOKED_STATE               "RevokedState"
#define  MUTUAL_AUTH_CLIENT_CERT_REVOKED_DATE                "RevokedDate"
#define  MUTUAL_AUTH_CLIENT_CERT_METHOD_REINIT               "ReInitClientCertificate"

#define  BMC_CONTACT_NAME                 "Contact"
#define  BMC_CONTACT_OBJ_NAME             BMC_CONTACT_NAME
#define  BMC_NET_NAME                     "OfficalWeb"
#define  BMC_NET_LIST_NAME                "SupportWeb"
#define  BMC_ELEC_MAIL_NAME               "Email"
#define  BMC_PHONE_NUM_NAME               "Phone"
#define  BMC_COPYRIGHT                    "Copyright"
#define  PROPERTY_CONTACT_D_COPYRIGHT     "DefaultCopyright"
#define  BMC_DOWNLOADKVMLINK              "DownloadKVMLink"
#define  BMC_DOC_SUPPORT_FLAG             "DocSupportFlag"

#define  BMC_METHOD_SETHWOFFICALWEB        "SetOfficalWeb"
#define  BMC_METHOD_SETCOPYRIGHT           "SetCopyright"
#define  BMC_METHOD_SUPPORTWEB             "SetSupportWeb"
#define  BMC_METHOD_SETEMAIL               "SetEmail"
#define  BMC_METHOD_SETPHONE               "SetPhone"
#define  BMC_METHOD_SETDOWNLOADKVMLINK     "SetDownloadKVMLink"
#define  BMC_METHOD_SETNCSI                "SetPchNcsiInitFlag"

#define  BMC_MOTHER_BOARD_FROM_FRU        ("MotherBoard")
#define  BMC_BOARD_ID_NAME                ("BoardId")

#define  BMC_PRODUCT_NAME_APP             "PRODUCT"
#define  BMC_PRODUCT_CARDMGNT_BY_NCSI_SUPP "CardMgntByNcsiSupport"
#define  BMC_NEW_PRODUCT_PRO_NAME         ("NewProductName")
#define  BMC_PRODUCT_OBJ_NAME_APP         BMC_PRODUCT_NAME_APP
#define  RESOURE_PROPERTY_NAME             "ResoureNotEnough"
#define  BMC_PRODUCT_PICTURE              ("ProductPic")
#define  BMC_PRODUCT_VER_NAME             ("ProductVersion")
/* BEGIN: Add for AR:Hardware_010 by xwx388597, 2016/10/27 */
#define  BMC_PRODUCT_VER_NAME_NUM         ("ProductVersionNum")
/* BEGIN: Modified by h00282621, 2017-07-21 */
/* BEGIN: Added by luchao 0454427, 2018/9/4   PN: NA*/
#define  BMC_PRODUCT_MODEL_OBJECT_NAME    "ProductModel"
/* END:   Added by luchao 0454427, 2018/9/4 */
#define  BMC_MGNT_PLATFORM_VER_NUM        ("ProductVersionNum")
#define  BMC_X86_PLATFORM_VER_NUM         ("X86PlatformVersionNum")
/* END: Modified by h00282621, 2017-07-21 */
#define  BMC_PRODUCT_PCH_NCSI_INIT_FLAG   ("PchNcsiInitFlag")
#define  PROPERTY_PRODUCT_ALIAS             ("ProductAlias")
#define  BMC_METHOD_SETPRODUCTALIAS             "SetProductAlias"
#define  BMC_METHOD_SETPRODUCTALIAS_NO_PERSISTENCE             "SetProductAliasNoPersistence"
#define  BMC_METHOD_SETPRODUCT_DEFAULTALIAS             "SetProductDefaultAlias"
#define PROPERTY_PRODUCT_CONFIG_MODEL "ConfigurationModel"
#define METHOD_PRODUCT_SET_CONFIG_MODEL "SetConfigurationModel"
#define PROPERTY_PRODUCT_CMCC_VERSION "CmccVersion"
#define METHOD_PRODUCT_SET_CMCC_VERSION "SetCmccVersion"
#define PROPERTY_PRODUCT_CUCC_VERSION "CuccVersion"
#define METHOD_PRODUCT_SET_CUCC_VERSION "SetCuccVersion"

/* BEGIN: Modified by l00389091, 2019/3/25   问题单号:DTS2019032100931 */
#define  PROPERTY_DEFAULT_PRODUCT_ALIAS          "DefaultProductAlias"
#define  BMC_METHOD_UPDATEALIASBYCOOLINGMEDIUM   "UpdateAliasByCoolingMedium"
/* END: Modified by l00389091, 2019/3/25  */

/* END: Add for AR:Hardware_010 by xwx388597, 2016/10/27 */
#define DISK_TEMP_FOR_THERMAL_SUPPORT "DiskTempForThermalSupport"

/* BEGIN: Add for AR:Hardware_030 by j00383144, 2017/1/23 */
#define  BMC_PRODUCT_PADDING_MODE        ("UpgradeRSAPaddingMode")
/* END: Add for AR:Hardware_030 by j00383144, 2017/1/23 */
#define  BMC_PRODUCT_VIRTUAL_NETWORK_ENABLE        "VirtualNetEnable"

#define BMC_PRODUCT_SOL_UART_INDEX				   "SOLUartIndex"
#define BMC_PRODUCT_USER_PWD_CRYPT_SALT_LEN   "UserPwdCryptSaltLen"

#define  BMC_PRODUCT_STARTPOINT    "StartPoint"
#define  BMC_PRODUCT_LIFESPAN   "Lifespan"
#define  BMC_PRODUCT_FIRSTPOWERONTIME   "FirstPowerOnTime"
#define  BMC_PRODUCT_SOLD_PLACE   "SoldPlace"
#define  BMC_PRODUCT_PRO_NAME             ("ProductName")
#define  BMC_PRODUCT_PRO_MAXSTARTUPTIME   ("MaxStartUpTime")
#define  BMC_PRODUCT_ID_NAME              ("ProductId")
#define  BMC_CB_PRODUCT_ID_NAME            "CBProductID"
#define  BMC_IS_CUSTOM_BRAND               "IsCustomBrand"
#define  BMC_TIMESRC_NAME                 ("TimeSrc")
#define  PROPERTY_CHASSIS_NUM             ("ChassisNum")
/* BEGIN: Added by d00381887, 2017/12/18, AR.SR.SFEA02119723.006.002 支持识别媒质类型 */
#define  PROPERTY_OTM_DISPLAY_ENABLE      ("OtmDisplayEnable")
/* END: Added by d00381887, 2017/12/18, AR.SR.SFEA02119723.006.002 支持识别媒质类型 */
/* BEGIN: Modified by zhanglei wx382755, 2018/1/3   PN:AR.SR.SFEA02119721.007.001 从MM920移植 */
#define  PROPERTY_SHELF_CLI_ENABLE       ("ShelfCliEnable")
#define  PROPERTY_SHELF_CLI_SHIELD       ("ShelfCliShield")
/* END:   Modified by zhanglei wx382755, 2018/1/3 */
#define  PROPERTY_KVM_ENABLE              ("KVMEnable")
#define  PROPERTY_SOL_ENABLE              ("SOLEnable")
#define  PROPERTY_SWRAID_TYPE             ("SWRaidType")
#define  PROPERTY_APS_ENABLE              ("APSEnable")
#define  PROPERTY_CAR_ENABLE              ("CAREnable")
#define  PROPERTY_FWT_ENABLE             ("FWTEnable")
#define  PROPERTY_MAC_ENABLE             ("MACEnable")
#define  PROPERTY_VER_ENABLE             ("VEREnable")
#define  PROPERTY_PECI_ENABLE             ("PeciEnable")
#define  PROPERTY_PRODUCT_PSFMAXTEMP     "SFPMaxTemp"
#define  PROPERTY_PRODUCT_CDRMAXTEMP     "CdrMaxTemp"
#define  METHOD_PRODUCT_SET_PSFMAXTEMP   "SetSFPMaxTemp"
#define  METHOD_PRODUCT_SET_DISKS_MAXTEMP   "SetDisksMaxTemp"
#define  METHOD_PRODUCT_SET_CDRMAXTEMP      "SetCdrMaxTemp"

#define  METHOD_PRODUCT_SET_STARTPOINT     "SetStartPoint"
#define  METHOD_PRODUCT_SET_LIFESPAN    "SetLifespan"

#define  METHOD_PRODUCT_SET_PRODUCT_NAME "SetProductName"
#define  METHOD_PRODUCT_SET_PRODUCT_UID "SetProductUniqueId"
#define  METHOD_PRODUCT_SET_NEW_PRODUCT_NAME "SetNewProductName"
#define  METHOD_PRODUCT_SET_PRODUCT_UID "SetProductUniqueId"
#define  METHOD_PRODUCT_SET_PRODUCT_VERSION "SetProductVersion"

/*BEGIN:Add by litingting, 2018/05/18, PN:SR.SFEA02130837.005.005*/
#define  PROPERTY_NVME_VMD_ENABLE          "NVMeVMDEnable"
/*BEGIN:Add by litingting, 2018/05/18, PN:SR.SFEA02130837.005.005*/

/*BEGIN: Added by h00164462, 2015/4/11, PN:SR-0000245341-002*/
#define  PROPERTY_SSH_PUBLICKEY_ENABLE   ("SSHPublicKeyEnable")
/*END: Added by h00164462, 2015/4/11, PN:SR-0000245341-002*/
#define  PROPERTY_SERVER_TYPE            ("ServerType")
/*BEGIN: Added by cwx290152, 2017/02/06, PN : DTS2017020600489*/
#define  PROPERTY_MGMT_SOFTWARE_TYPE     ("MgmtSoftWareType")
/*END: Added by cwx290152, 2017/02/06, PN : DTS2017020600489*/
#define  PROPERTY_BOARD_WIDTHTYPE        ("BoardWidthType")
#define  PROPERTY_BOARD_SLOT_BASE        ("BoardSlotBase")
#define  PROPERTY_OPEN_BOX               ("OpenBoxEvent")
#define  PROPERTY_BIOS_VALID             ("BiosValid")
#define  PROPERTY_CPLD_VALID             ("CpldValid")
#define  PROPERTY_CPLD_VALID_MODE       ("CpldValidMode")
#define  PROPERTY_BIOS_FLASH_SIZE        ("BIOSFlashSize")
#define  PROPERTY_PCIE_NCSI_VALID        ("PcieNcsiValid")
#define  PROPERTY_PCIE_NCSI_SLOT_ID      ("PcieNcsiSlotId")
#define  METHOD_PRODUCT_SET_PCIE_NCSI_SLOTID  ("SetPcieNcsiSlotId")
#define  PROPERTY_RAID_INTINFO           ("RaidIntPin")
#define  PROPERTY_NO_SDCARD_STATUS       ("NOSdCardState")
/* BEGIN: Modified by wangpenghui wwx382232, 2018/3/23 16:12:13   问题单号:监控CPU 额定功耗  */
#define PROPERTY_CPU_POWER_PROTECTION   "PowerProtectionSupport"
/* END:   Modified by wangpenghui wwx382232, 2018/3/23 16:12:16 */
/* BEGIN for ARB7F63698-2463-45ba-AEEB-9A0B21853DA2 : Added by ouluchun 00294050 2015-11-19 */
#define  PROPERTY_UPGRADE_POWER_ENABLE   ("UpgradePowerEnable")
/* END for ARB7F63698-2463-45ba-AEEB-9A0B21853DA2 : Added by ouluchun 00294050 2015-11-19 */

 #define  PROPERTY_SYNC_UID_STATE_ENABLE                    "SyncUIDStateEnable"

/*STRAT by wangenmao 2016-12-5 PN: AR-0000256852-001-016*/
#define PROPERTY_BP_CPLD_VALIDE_FLAG               ("BpCpldValidFlag")
/*END by wangenmao 2016-12-5 PN: AR-0000256852-001-016*/
#define  PROPERTY_PCH_BOARD_VERSION      "PCHBoardVersion"
#define  PROPERTY_DISKS_MAX_TEMP         "DisksMaxTemp"
#define  PROPERTY_SSD_MAX_TEMP          "SSDMaxTemp"
#define  PROPERTY_M2_MAX_TEMP           "M2MaxTemp"
#define  PROPERTY_NVME_MAX_TEMP         "NvmeMaxTemp"
#define  PROPERTY_OBTAIN_DISKS_TEMP_FAILED     "ObtainDisksTempFailed"
#define  PROPERTY_PRODUCT_VENDOR_ID     "ProductVendorID"
#define  PROPERTY_PRODUCT_UNIQUE_ID      ("ProductUniqueID")
#define  PROPERTY_CB_PRODUCT_UNIQUE_ID         "CBProductUniqueID"
#define  PROPERTY_MUTUAL_AUTHENTICATION_ENABLE      ("MutualAuthenticationEnable")
/* BEGIN: Added by zengrenchang(z00296903), 2017/2/6   问题单号:AR-0000256850-015-009*/
#define  PROPERTY_WOL_STATE         ("WOLState")        //用户做的WOL配置
#define  PROPERTY_WOL_ACCESSOR      ("WOLAccessor")     //WOL功能关联的寄存器
#define  PROPERTY_VMD_STATE          ("VMDState")   //VMD功能关联的寄存器
#define  PROPERTY_HOTPLUG_SURPRISE     "HotplugSurprise"
#define  PROPERTY_VGA_SWITCH    "VGASwitch"
/* END:   Added by zengrenchang(z00296903), 2017/2/6 */
/* BEGIN: Added by cwx290152, 2017/04/20   PN:AR-0000276587-001-002 */
#define  PROPERTY_PRODUCT_WEAK_PWDDICT_SUPPORT      ("WeakPwdDictSupport")
/* END: Added by cwx290152, 2017/04/20   PN:AR-0000276587-001-002 */
#define  PROPERTY_PRODUCT_WHITE_IPMILIST_SUPPORT     "WhiteIPMIListSupport"
#define  PROPERTY_PRODUCT_CFG_MEMCLKSPEED_ENABLED    "CfgMemClkSpeedEnable"
#define  PROPERTY_PRODUCT_CACHEFS_ENABLE       ("CachefsEnable")
#define  PROPERTY_PRODUCT_LEAKDET_SUPPORT     "LeakDetectSupport"
#define  PROPERTY_PRODUCT_FLASH_RWTEST_SUPPORT     "FlashRwTestSupport"
#define  PROPERTY_STORAGE_CONFIG_READY   "StorageConfigReady"
#define  PROPERTY_PRODUCT_AC_CYCLE_ACHIEVED_TYPE   "ACCycleAchievedType"
#define  PRODUCT_METHOD_SETBOARDVERSION  "SetBoardVersion"
#define  PRODUCT_METHOD_SETSWRAIDTYPE    "SetSWRaidType"
#define  PRODUCT_METHOD_SETNOSDCARDSTATE ("SetNOSdCardState")
/* BEGIN: Added by zengrenchang(z00296903), 2017/2/6   问题单号:AR-0000256850-015-009*/
#define  METHOD_SET_WOL_STATE  "SetWOLState"
/* END:   Added by zengrenchang(z00296903), 2017/2/6 */
#define  METHOD_SET_VMD_STATE "SetVMDState"
#define  METHOD_SET_STORATE_CONFIG_READY "SetStorageConfigReady"
#define  METHOD_SET_SYSRESOURE_STATE "SetSysResourceState"

/*START by wangenmao 2016-12-5 PN: AR-0000256852-001-016*/
#define  METHOD_PFN_SET_BPCPLD_VALIDE_FLAG "SetBladeBpCpldValidFlag"
/*END by wangenmao 2016-12-5 PN: AR-0000256852-001-016*/
#define  METHOD_SET_VGA_SWITCH "SetVGASwitch"

#define CLASS_NAME_PRODUCT                                 BMC_PRODUCT_NAME_APP
#define OBJ_NAME_PRODUCT                                   BMC_PRODUCT_OBJ_NAME_APP
#define PROPERTY_PRODUCT_NAME                              BMC_PRODUCT_PRO_NAME
#define PROPERTY_PRODUCT_VERSION                           BMC_PRODUCT_VER_NAME
#define PROPERTY_PRODUCT_ID                                BMC_PRODUCT_ID_NAME
#define PROPERTY_PRODUCT_CHASSISNUM                        PROPERTY_CHASSIS_NUM
#define PROPERTY_PRODUCT_SERVERTYPE                        PROPERTY_SERVER_TYPE
#define PROPERTY_PRODUCT_CMS_SIGN_ENABLE                   ("CMSSignEnable")
#define PROPERTY_PRODUCT_HEIGHT_U                          "HeightU"
#define METHOD_PRODUCT_SET_HTTPS_CERT_VERIFY "SetHttpsCertVerification"
#define PROPERTY_PRODUCT_HTTPS_TRANSFER_CERT_VERIFY "HttpsTransferCertVerification"
#define PROPERTY_PRODUCT_SESSION_TOKEN_LENGTH "SessionTokenLength"
#define METHOD_PRODUCT_SET_SESSION_TOKEN_LENGTH "SetSessionTokenLength"
#define PROPERTY_PRODUCT_KVM_CAPS_SYNC_ENABLE "KVMCapsSyncEnable"
#define PROPERTY_PRODUCT_SYSTEM_AREA_FRU_ID                "SystemAreaFruId"
/* ProductComponent类的对象及属性 */
#define CLASS_PRODUCT_COMPONENT             "ProductComponent"      /* 产品部件信息描述类 */
#define OBJ_PRODUCT_COMPONENT               "ProductComponent"      /* 产品部件信息描述对象 */
#define PROPERTY_COMPOENT_MEZZ_LOCATION     "MezzLocation"          /* MEZZ卡位置信息 */
#define PROPERTY_COMPOENT_PS_NUM            "PsNum"                 /* 电源最大个数 */
#define PROPERTY_COMPOENT_FAN_NUM           "FanNum"                /* 风扇最大个数 */
#define PROPERTY_COMPOENT_CPU_NUM           "CpuNum"                /* 处理器最大个数 */
#define PROPERTY_COMPOENT_MEMORY_NUM        "MemoryNum"             /* 内存最大个数 */
#define PROPERTY_COMPOENT_DISK_NUM          "DiskNum"               /* 硬盘最大个数 */
#define PROPERTY_COMPOENT_RISER_CARD_NUM    "RiserCardNum"          /* Riser卡最大个数 */
#define PROPERTY_COMPOENT_PCIE_CARD_NUM     "PcieCardNum"           /* PCIE卡最大个数 */
#define PROPERTY_COMPOENT_RAID_CARD_NUM     "RaidCardNum"           /* RAID卡最大个数 */
#define PROPERTY_COMPOENT_MEZZ_CARD_NUM     "MezzCardNum"           /* MEZZ卡最大个数 */
#define PROPERTY_COMPOENT_HDDBACK_CARD_NUM  "HddBackCardNum"         /* 硬盘背板最大个数 */
#define PROPERTY_COMPOENT_NET_CARD_NUM      "NetCardNum"            /* 网卡最大个数 */
#define PROPERTY_COMPOENT_MEMORY_CARD_NUM      "MemoryCardNum"            /* 内存板最大个数 */
#define PROPERTY_COMPOENT_IO_CARD_NUM      "IOCardNum"            /* IO板最大个数 */
#define PROPERTY_COMPOENT_CPU_BOARD_NUM      "CpuBoardNum"            /* IO板最大个数 */
#define PROPERTY_COMPOENT_GPU_BOARD_NUM      "GPUBoardNum"            /* GPU板最大个数 */
#define PROPERTY_COMPOENT_SD_CARD_NUM       "SDCardNum"             /*SD卡的最大个数*/
#define PROPERTY_COMPOENT_SDCARD_CONTROLLER_NUM "SDCardControllerNum" /*SD卡控制器的最大个数*/
#define PROPERTY_COMPONENT_NODE_NUM         "NodeNum"                /*节点最大数量*/
#define PROPERTY_COMPONENT_MM_NUM           "MMNum"                 /*管理板数量*/
#define PROPERTY_COMPONENT_UNIT_NUM         "UnitNum"               /* Unit数量 */
#define PROPERTY_COMPONENT_M2_NUM           "M2AdpterCardNum"       /* M2转接卡最大个数 */ // DTS2019060506346 新增
/* BEGIN: Added by TWX555928, 2018/6/6   PN:X6800 V5 改板*/
#define PROPERTY_COMPONENT_HORIZ_CONN_BRD_NUM  "HorizConnBoardNum"    /*水平转接板(x6800v5)数量*/
#define PROPERTY_COMPONENT_BBU_MODULE_NUM   "BBUModuleNum"
/* END:   Added by TWX555928, 2018/6/6 */
/* BEGIN: Added by z00448564, 2018/11/7   PN: AR.SR.SFEA02130924.023.059*/
#define PROPERTY_COMPONENT_SECURITY_MODULE_NUM    "SecurityModuleNum"   /*安全模块最大个数*/
/* END:   Added by z00448564, 2018/11/7 */
#define PROPERTY_COMPONENT_LEAKDETCARD_NUM  "LeakDetCardNum"
#define PROPERTY_COMPONENT_OCP_CARD_NUM    "OCPNum"   /* OCP最大个数 */
#define PROPERTY_COMPONENT_EXP_BOARD_NUM    "ExpBoardNum"
#define PROPERTY_COMPONENT_LSW_PORT_NUM     "LswPortNum"               /* lan switch端口数量 */
#define PROPERTY_COMPONENT_EXP_MODULE_NUM   "ExpansionModuleNum"
#define PROPERTY_COMPONENT_FAN_MODULE_NUM   "FanModuleNum"
#define PROPERTY_COMPONENT_CIC_CARD_NUM     "CICCardNum"             /* CIC卡最大个数 */
#define PROPERTY_COMPONENT_ARCARD_NUM       "ARCardNum"              /* AR卡数量 */
#define PROPERTY_COMPONENT_HOST_NUM         "HostNum"
#define PROPERTY_COMPONENT_SOC_BOARD_NUM     "SoCBoardNum"
#define PRODUCT_COMPONENT_METHOD_UPDATE_COMPONENT_INFO         "update_product_component_info"
#define PROPERTY_EXPANSION_MODULE_TYPE_SUPPORT   "ExpansionModuleTypeSupport" /* 节点支持的级联模块类型 */


/* CLASS_SILK_TEMPLATE类的对象及属性 */
#define CLASS_SILK_TEMPLATE                 "SilkTemplate"        /* 产品丝印模板信息描述类 */
#define PROPERTY_MEM_SILK_TEMPLATE          "MemSilkTemplate"     /* 内存丝印模板信息 */
#define PROPERTY_CPU_SILK_TEMPLATE          "CpuSilkTemplate"     /* CPU 丝印模板信息 */

/* 框信息 */
#define CLASS_NAME_CHASSIS   "Chassis"
#define OBJECT_NAME_CHASSIS   CLASS_NAME_CHASSIS
/* 此对象需要定义在机框背板的XML中, 用于解决/redfish/v1/Chassis/Enclosure中"Model"属性动态获取的问题 */
#define OBJECT_NAME_ENCLOSURE   "Enclosure"
#define CLASS_NAME_AGENT_CHASSIS   "AgentChassis"
#define OBJECT_NAME_AGENT_CHASSIS   CLASS_NAME_AGENT_CHASSIS
#define PROPERTY_CHASSIS_CHASSIS_TYPE       "ChassisType"
#define PROPERTY_CHASSIS_LOCATION           "Location"
#define PROPERTY_CHASSIS_CHASSIS_NUM        "ChassisNum"
#define PROPERTY_CHASSIS_CHASSIS_NAME       "ChassisName"
#define PROPERTY_CHASSIS_OWNER_ID           "OwnerID"
#define PROPERTY_CHASSIS_MAX_NODE_INDEX     "MaxNodeIndex"
#define PROPERTY_CHASSIS_NODE_INDEX         "NodeIndex"
#define PROPERTY_CHASSIS_CHASSIS_NODE_PRESENT   "ChassisNodePresent"
#define PROPERTY_CHASSIS_CHASSIS_NODE_ACTIVE    "ChassisNodeActive"
#define PROPERTY_CHASSIS_SYNC_ELABEL        "SyncElabel"
#define PROPERTY_CHASSIS_NODE_INDEX_SUFFIX  "NodeIndexSuffix"
/* START  Modifyied for AR-0000976084 by h00272616  20151117 */
#define PROPERTY_CHASSIS_VERSION_DATA       "VersionData"
#define PROPERTY_CHASSIS_CUSTOM_DATA1       "CustomData1"
#define PROPERTY_CHASSIS_CUSTOM_DATA2       "CustomData2"
#define PROPERTY_CHASSIS_INLET_TEMP       "InletTemp"
#define PROPERTY_CHASSIS_OUTLET_TEMP      "OutletTemp"
#define PROPERTY_CHASSIS_INTRUSION      "ChassisIntrusion"
#define PROPERTY_CHASSIS_INTRUSION_LOCK      "ChassisIntrusionLock"
#define PROPERTY_CHASSIS_INTRUSION_EVENT      "ChassisIntrusionEvent"
#define PROPERTY_CHASSIS_UID_LOCATION       "UIDLocation"
#define PROPERTY_CHASSIS_INTRUSION_POWER_OFF       "ChassisIntrusionInPowerOff"
#define PROPERTY_CHASSIS_INTRUSION_STATUS_CLEAR       "ChassisIntrusionStatusClear"
#define PROPERTY_CHASSIS_LEFTEAR_SWITCH_SLOTID       "LeftEarSwitchSlotId"
#define PROPERTY_CHASSIS_MASTER_NODE_SLOTID  "MasterNodeSlotId"
#define PROPERTY_CHASSIS_PANEL_SERIAL_SW_NODE_SUPP  "PanelSerialSwNodeSupp"
#define PROPERTY_CHASSIS_PANEL_SERIAL_SW_NODE_ID    "PanelSerialSwNodeId"
#define PROPERTY_CHASSIS_MGNT_PORTS_STATUS           "MgntPortsLinkStatus"

#define METHOD_CHASSIS_SET_CHASSIS_NAME "SetChassisName"
#define METHOD_CHASSIS_SET_CHASSIS_NUMBER "SetChassisNumber"
#define METHOD_CHASSIS_SET_CHASSIS_LOCATION "SetChassisLocation"
#define METHOD_CHASSIS_GET_SHELF_VERSION "GetShelfVersion"
#define METHOD_CHASSIS_INFO_SYNC_METHOD "ChassisInfoSyncMethod"
#define METHOD_STOP_EEP_ROUTINE "StopEepRoutine"
#define METHOD_KEY_PROCUDT_INFO_INIT "KeyProcuctInfoInit"
#define METHOD_CHASSIS_SET_OWNER_ID         "SetOwnerID"
#define METHOD_CHASSIS_SET_NODE_INDEX       "SetNodeIndex"
#define METHOD_CHASSIS_SET_SYNC_ELABEL      "SetSyncElabel"
#define METHOD_CHASSIS_SET_NODE_INDEX_SUFFIX       "SetNodeIndexSuffix"
#define METHOD_CHASSIS_SET_VERSION_DATA       "SetVersionData"
#define METHOD_CHASSIS_SET_CUSTOM_DATA1       "SetCustomData1"
#define METHOD_CHASSIS_SET_CUSTOM_DATA2       "SetCustomData2"
#define PROPERTY_CHASSIS_ALARM_POLICY        "AlarmPolicy"
#define METHOD_CHASSIS_SET_ALARM_POLICY       "SetAlarmPolicy"
#define METHOD_CHASSIS_SET_PANEL_SERIAL_SW_NODE_ID "SetPanelSerialSwNodeId"


/* 敏感信息 */
#define CLASS_NAME_SENSITIVE_INFO           "SensitiveInfo"
#define PROPERTY_SENSITIVE_CLASS_NAME       "ClassName"
#define PROPERTY_SENSITIVE_PROPERTY_NAME    "PropertyName"




#define  BMC_BOARD_NAME                   ("BOARD")
#define  BMC_BOARD_OBJ_NAME               BMC_BOARD_NAME
#define  BMC_LOCATION_ID_NAME             ("LocationId")
#define  BMC_SLOT_ID_NAME                 ("SlotId")
#define  BMC_SLOT_ID_VERIFY_TYPE          ("VerifyType")
#define  BMC_BOARDID_NAME                 ("BoardId")
#define  BMC_HW_ADDR_NAME                 ("HardAddress")
#define  BMC_SLOT_ID_SRC_NAME             ("SlotIdSrc")
#define  BMC_BOARD_ID_SRC_NAME            ("BoardIdSrc")
#define  PROPERTY_BOADE_NAME              ("Name")
#define  PROPERTY_HW_ADDR_FAULT           ("HwAddrFault")
#define  PROPERTY_SLOTID_FAULT            ("SlotIdFault")
#define  PROPERTY_BOARD_ID_FAULT          ("BoardIdFault")
#define  PROPERTY_BOARD_IO_DRIVER         ("IoDriver")
#define  PROPERTY_CLR_PW_IO_INSTATUS      ("BmcClearPwInStatus")
#define  PROPERTY_BMC_CLR_PW_FLAG         ("BmcClearPwFlag")
#define  METHOD_SET_SLOT_ID               ("SetSlotId")


#define CLASS_NAME_BOARD                                   BMC_BOARD_NAME
#define OBJ_NAME_BOARD                                     BMC_BOARD_OBJ_NAME
#define PROPERTY_BOARD_LOCATIONID                          BMC_LOCATION_ID_NAME


#define  CLASS_WIDTH_BITMAP               ("WidthBitMap")
#define  PROPERTY_WIDTH_TYPE              ("WidthType")
#define  PROPERTY_BITMAP_VALUE            ("BitMapValue")


#define  CLASS_SMM                        ("SMM")
#define  OBJECT_SMM                        ("SMM")
#define  OBJECT_SMM0                      ("SMM0")
#define  OBJECT_SMM1                      ("SMM1")
#define  PROPERTY_SMM_INDEX               ("Index")
#define  PROPERTY_SMM_STATUS              ("Status")
#define  PROPERTY_SMM_FIXED_IP            ("FixedIP")
#define  PROPERTY_SMM_IPMB_ADDR           ("IpmbAddr")

#define  METHOD_RESET_OTHER_SMM           "ResetOtherSMM"


#define  CLASS_MASTER_SMM                 ("MasterSMM")
#define  OBJECT_MASTER_SMM                CLASS_MASTER_SMM
#define  PROPERTY_MASTER_SMM_ID           ("MasterSMMID")
#define  PROPERTY_MASTER_SMM_IP           ("MasterSMMIP")


/* BTD-依赖其它app的暂定义 */
#define  ETH_CLASS_NAME                   ("Eth")
#define  ETH_OBJECT_NAME                  ("Eth2")
#define  ETH_HEARTBEAT_NAME               "EthHeartBeat"
#define  ETH_MAC_ADDR_NAME                ("Mac")
#define  SEL_OBJECT_NAME                  ("Sel")
#ifdef ARM64_HI1711_ENABLED
#define  OBJ_SSIF_NAME                    ("Ssif")
#define  SSIF_PROPERTY_BUSID              ("BusId")
#define  SSIF_PROPERTY_SPEED              ("Speed")
#define  SSIF_PROPERTY_GPIONUM            ("SsifRdGpionum")
#define  SSIF_PROPERTY_SLAVE_ADDR         ("SlaveAddr")
#endif
#define  IPMB_CLASS_NAME                  ("Ipmb")
#define  IPMB_PROPERTY_CHANNEL_ID         ("ChanID")
#define  IPMB_PROPERTY_SLAVE_ADDR         ("SlaveAddr")
#define  IPMB_PROPERTY_BUS_ID             ("BusID")
#define  CLASS_ONEPOWER                   ("OnePower")
#define  PROPERTY_POWER_HEALTH            ("Health")
#define  CLASS_FANCLSEE                   ("FANClass")
#define  PROPERTY_FAN_STATUS              ("STATUS")
#define  CLASS_NAME_DFTVER                 "DftVersion"
#define  OBJ_NAME_FRU0IPMBDSP              "Fru0IpmbDisplay"
#define  PROPERTY_DFTVER_DISPLAYFLAG       "DisplayFlag"
#define  PROPERTY_DFTVER_IPMBADDR          "IpmbAddr"
#define  ETHGROUP_CLASS1_NAME               ("EthGroup")
#define  PROPERTY_ETHGROUP1_OUTTYPE         ("OutType")
#define  PROPERTY_ETHGROUP1_INNERNET        ("InnerNetWork")
#define  PROPERTY_ETH1_NAME                 ("Name")


#define  CFG_FILE_DISPLAY                ("display0.ini")
#define  RESP_FILE_OPTION                ("option0.ini")
#define  DFG_FILE_CHANGE                 ("changed0.ini")
#define  HMM_FILE_BMCFIRMWARE            ("Firmware")
#define  CFG_FILE_CLP                    ("ClpConfig.ini")
#define  RESP_FILE_CLP                   ("ClpResponse.ini")
#define  CFG_FILE_POLICYCONFIGREGISTRY   ("PolicyConfigRegistry.json")
#define  CFG_FILE_CONFIGVALUE            ("ConfigValue.json")
#define  CFG_FILE_REGISTRY               ("registry.json")
#define  RESP_FILE_CURRENTVALUE          ("currentvalue.json")
#define  CFG_FILE_SETTING                ("setting.json")
#define  RESP_FILE_RESULT                ("result.json")


/* BIOS模块对象及属性 */
#define  BIOS_CLASS_NAME                 "Bios"
#define  BIOS_OBJECT_NAME                BIOS_CLASS_NAME
#define  BIOS_CONFIG_EXPORT_SUPPORT      "ConfigExportSupport"
#define  BIOS_FILE_OPTION_NAME           ("OptionFileName")
#define  BIOS_FILE_CHANGE_NAME           ("ChangeFileName")
#define  BIOS_FILE_DISPLAY_NAME          ("DisplayFileName")
/* redfish C50, added by wwx390838, start */
#define  BIOS_FILE_CONFIGVALUE_NAME      ("ConfigValueFileName")
#define  BIOS_FILE_POLICYCONFIGREGISTRY_NAME ("PolicyConfigRegistryFileName")
#define  BIOS_FILE_COMPONENTVERSION_NAME ("ComponentVersionFileName")
#define  BIOS_FILE_REGISTRY_NAME         ("RegistryFileName")
#define  BIOS_FILE_CURRENT_VALUE_NAME    ("CurrentValueFileName")
#define  BIOS_FILE_SETTING_NAME          ("SettingFileName")
#define  BIOS_FILE_RESULT_NAME           ("ResultFileName")
#define  BIOS_REGISTRY_VERSION           ("RegistryVersion")
/* redfish C50, added by wwx390838, end */

/* Start by h00272616 DTS2016032501873 2016-04-19 */
#define  BIOS_LAST_START_OPTION_NAME     ("LastStartOption")
/* End by h00272616 DTS2016032501873 2016-04-19 */
#define  BIOS_START_OPTION_NAME          ("StartOption")
#define  BIOS_START_OPTION_NAME_EXT      ("StartOptionExt")
#define  BIOS_START_OPTION_FLAG_NAME     ("StartOptionFlag")
/* BEGIN: DTS2018011901428. Modified by f00381872, 2018/3/1 */
#define  BIOS_START_OPTION_FLAG_EXT_NAME ("StartOptionFlagExt")
/* END: DTS2018011901428. Modified by f00381872, 2018/3/1 */
#define  BIOS_BOOT_MODE_SUPPORT_FLAG     ("BiosBootModeSupportFlag")
#define  BIOS_BOOT_MODE                  ("BiosBootMode")
/* BEGIN: 2018-5-17 z00416979 AR.SR.SFEA02130924.005.003   支持IPMI设置启动模式选项支持定制化 */
#define  BIOS_BOOT_MODE_SW_ENABLE        ("BiosBootModeSwEnable")
/* END:   2018-5-17 z00416979 AR.SR.SFEA02130924.005.003 */
#define  BIOS_BOOT_MODE_SW               ("BiosBootModeSw")
#define  BIOS_CLEAR_CMOS_NAME            ("ResetCmos")
#define  BIOS_PRINT_FLAG_NAME            ("BiosPrintFlag")    //新需求AR586D337A-B0EF-49da-B718-473CC926DAA9
#define  BMC_BIOS_VER_NAME               ("Version")
#define  BMC_TEEOS_VER_NAME              ("TeeosVersion")
#define  BIOS_BACKUP_VER_NAME            ("BackupVersion")
#define  PROPERTY_BIOS_START_FLAG        ("BiosStartFlag")
#define  PROPERTY_BIOS_UNIT_NAME         ("UnitNum")
#define  PROPERTY_BIOS_CLP_NAME          ("ClpFileName")
#define  PROPERTY_BIOS_CLP_RESP_NAME     ("ClpRespFileName")
#define  PROPERTY_BIOS_CLP_CHANGE_FLAG   ("ClpChangeFlag")
#define  PROPERTY_BIOS_CLEAR_CLP_FLAG    ("ClpClearFlag")
#define  PROPERTY_BIOS_MENU_DATA         ("MenuData")
#define  PROPERTY_BIOS_MENU_CHANGE_FLAG  ("MenuChangeFlag")
#define  PROPERTY_BIOS_PCIECARD_BDF      ("PcieCardBDF")
#define  PROPERTY_BIOS_PCIEDISK_BDF      ("PcieDiskBDF")
#define  PROPERTY_BIOS_OCPCARD_BDF      ("OCPCardBDF")
//add by wangenmao 2017-3-18 AR-0000276590-005-002
#define  PROPERTY_BOIS_ORDER_FORMAT      ("BootOrderFormat")
/* BEGIN: Added by gwx455466, 2017/7/3 19:45:46   问题单号:AR-0000276588-006-001 */
#define  PROPERTY_BIOS_CMES_FILE_NAME          ("CMESFileName")
/* END:   Added by gwx455466, 2017/7/3 19:45:52 */
#define  PROPERTY_BIOS_FW_VERIFY_RESULT             ("FwVerifyResult")
#define  PROPERTY_BIOS_BAKCUP_FW_VERIFY_RESULT      ("BackupFwVerifyResult")
#define  METHOD_VERIFY_BIOS_FIRMWARE                      ("VerifyBIOSFirmware")
#define  METHOD_SET_BIOS_FW_VERIFY_RESULT           ("SetBIOSFwVerifyResult")

/* 对读硬件信息的命令做白名单*/
#define PROPERTY_BIOS_HDINFO_RD_CMD_WHITELIST      ("HdInfoRdCmdWhiteList")
#define PROPERTY_BIOS_HDINFO_REF_CHIP_HANDLE       ("RefChip")

/* BEGIN: Modified by h00422363, 2019/1/10 19:32:12   问题单号:UADP134291 智能节能需求 */
#define PROPERTY_BIOS_SMI_COMMAND_DATA_STATE      ("SMIDataState")
#define PROPERTY_BIOS_SMI_INTERRUPT                             ("SMIInterrupt")
#define PROPERTY_BIOS_ENERGY_MODE_ENABLE                ("EnergySavingEnabled")
#define PROPERTY_BIOS_DEMT_CONFIG_ITEM                      ("DEMTConfig")
/*
 * 多BIOS升级包归一时，差异区域的身份标识。目前为 ME&Gbe 区域。
 *      0:  不支持
 *      1:  1288hv5、2288hv5、5288v5 电口
 *      2:  1288hv5、2288hv5、5288v5 光口
 *      3:  2288hv5 降成本
 */
#define PROPERTY_BIOS_IMAGE_UNION_ID        ("ImageUnionRegionId")

/* 内存FW版本与BIOS版本不匹配 */
#define PROPERTY_BIOS_MEM_VER_MISMATCH      "MemVerMismatch"
#define PROPERTY_BIOS_CERT_APPLY_MODE       "CertApplyMode"

#define METHOD_SET_BIOS_ENERGY_MODE_ENABLE                             ("SetBiosEnergySavingEnable")
#define METHOD_SET_BIOS_DYNAMIC_CONFIGURATION                                       ("SetBiosDynamicConfig")
#define METHOD_SET_BIOS_DEMT_ENABLE                                       ("SetBiosDEMTEnable")
/* END:   Modified by h00422363, 2019/1/10 19:32:20 */

/* 硬件信息读取命令类 */
#define  CLASS_HARDWARDINFO_READ_CMD    ("HardwardInfoReadCmd")
#define  PROPERTY_TYPE     ("Type")
#define  PROPERTY_CHIPID   ("ChipId")
#define  PROPERTY_ADDR     ("Addr")
#define  PROPERTY_OFFSET   ("Offset")
#define  PROPERTY_LENGTH   ("Length")
#define  PROPERTY_PCA9545_BUSID   ("Pca9545BusId")
#define  PROPERTY_PCA9545_ADDR    ("Pca9545Addr")

/* AgentBios 由data_sync 模块管理，用于数据同步 */
#define  CLASS_AGENT_BIOS "AgentBios"
/* DataNotifyBios类由hpc_mgmt 模块管理，用于通知Bios模块某些属性变化 */
#define  CLASS_DATA_NOTIFY_BIOS "DataNotifyBios"
#define  OBJECT_DATA_NOTIFY_BIOS CLASS_DATA_NOTIFY_BIOS

/* BIOS */
#define CLASS_NAME_BIOS                                    BIOS_CLASS_NAME
#define OBJ_NAME_BIOS                                      BIOS_OBJECT_NAME
#define PROTERY_BIOS_VERSION                               BMC_BIOS_VER_NAME
#define PROTERY_TEEOS_VERSION                              BMC_TEEOS_VER_NAME
#define PROTERY_BIOS_UNITNUM                               PROPERTY_BIOS_UNIT_NAME
#define PROTERY_BIOS_BOOTOPTION                            BIOS_START_OPTION_NAME
#define PROTERY_BIOS_BOOTOPTION_EXT                        BIOS_START_OPTION_NAME_EXT
#define PROTERY_BIOS_BOOTOPTIONFLAG                        BIOS_START_OPTION_FLAG_NAME
/* BEGIN: Added by Yangshigui YWX386910, 2017/9/4   问题单号:DTS2017082312480*/
#define PROPERTY_BIOS_BOOT_STAGE                           "BiosBootStage"
/* END:   Added by Yangshigui YWX386910, 2017/9/4 */
/* BEGIN: Added by h00371221, 2018/01/18   DTS2018011812821 */
#define PROPERTY_BIOS_STARTUP_STATE                        "SystemStartupState"
/* END   : Added by h00371221, 2018/01/18   DTS2018011812821 */
#define PROPERTY_BIOS_MASTERKEY_ID                "BiosPwdMasterKeyId"
#define PROPERTY_BIOS_SETUP_PWD_OLD  "OldSetupPassword"
#define PROPERTY_BIOS_SETUP_PWD_NEW  "NewSetupPassword"
#define PROPERTY_BIOS_USER_SETUP_PWD_OLD  "UserOldSetupPassword"
#define PROPERTY_BIOS_USER_SETUP_PWD_NEW  "UserNewSetupPassword"
#define PROPERTY_BIOS_CUSTOM_SFC_CLK_MHZ  "CustomSfcClkMhz"
#define PROPERTY_BIOS_PORT_ROOTBDF     "PortRootBDF"
#define PROPERTY_BIOS_PCIE_PORT_ROOTBDF    "PciePortRootBDF"
#define PROPERTY_BIOS_SUPPORTED_START_OPTION "SupportedStartOption"
#define PROPERTY_BIOS_SUPPORTED_START_OPTION_EXT "SupportedStartOptionExt"


#define METHOD_BIOS_CLEARCMOS                              "ClearCmos"
#define METHOD_BIOS_BOOTOPTION                             "SetStartOption"
#define METHOD_BIOS_BOOTOPTION_EXT                         "SetStartOptionExt"
#define METHOD_BIOS_BOOTOPTION_FLAG                        "SetStartOptionFlag"
/* BEGIN: DTS2018011901428. Modified by f00381872, 2018/3/1 */
#define METHOD_BIOS_BOOTOPTION_FLAG_EXT                    "SetStartOptionFlagExt"
/* END: DTS2018011901428. Modified by f00381872, 2018/3/1 */
#define METHOD_BIOS_SET_WRITE_PROTECT                      "SetWriteProtect"
#define METHOD_BIOS_SMBIOS                                 "ClearSMBIOS"
#define METHOD_BIOS_GET_CONFIG                             "GetBIOSConfig"
#define METHOD_BIOS_SET_CONFIG                             "SetBIOSConfig"
/* BEGIN: Added by c00149416, DTS2017021500425 2017/2/16 */
#define METHOD_BIOS_GET_M3VER                              "GetM3Ver"
/* END:   Added by c00149416, 2017/2/16 */
/* Added for C50, wwx390838, 2017/04/10, start */
#define METHOD_BIOS_GET_JSON_FILE                          "GetBIOSJsonFile"
#define METHOD_BIOS_SET_JSON_FILE                          "SetBIOSJsonFile"
#define METHOD_BIOS_RESET                                  "ResetBIOS"
#define METHOD_BIOS_CHANGE_PWD                             "BIOSChangePassword"
#define METHOD_BIOS_UPDATE_CIPHERTEXT                      "UpdateBIOSCiphertext"
/* Added for C50, wwx390838, 2017/04/10, end */

/* BEGIN: Added by liumancang, 2018/6/11 16:1:35   问题单号:AR.SR.SFEA02130925.019.003    网卡配置的查询*/
#define METHOD_BIOS_GET_CLP_RESP                          "GetClpResponse"
/* END:   Added by liumancang, 2018/6/11 16:2:4 */
/* BEGIN: Added by liumancang, 2018/7/11 10:55:57   问题单号:AR.SR.SFEA02130925.019.002 redfish网卡配置下发 */
#define METHOD_BIOS_SET_CLP_CONF                       "SetClpConfig"
/* END:   Added by liumancang, 2018/7/11 10:58:7 */

/* BEGIN: Added by liumancang, 2018/3/12 11:23:24   问题单号:AR.SR.SFEA02119720.022.003 */
#define METHOD_REVOKE_BIOSSETTING                                "RevokeBiosSetting"
/* END:   Added by liumancang, 2018/3/12 11:23:44 */
/* BEGIN: Added by h00371221, 2018/01/18   DTS2018011812821 */
#define METHOD_BIOS_SET_STARTUP_STATE                      "SetSystemStartupState"
/* END   : Added by h00371221, 2018/01/18   DTS2018011812821 */

//add by wangenmao 2017-3-18 AR-0000276590-005-002
/* END:   2018-5-17 z00416979 AR.SR.SFEA02130924.005.003   支持IPMI设置启动模式选项支持定制化 */
#define METHOD_SET_BIOS_BOOT_MODE_SW_ENABLE       "SetBiosBootModeSwEnable"
/* END:   2018-5-17 z00416979 AR.SR.SFEA02130924.005.003   */
#define METHOD_SET_BOOT_ORDER                     "SetBootOrder"
#define METHOD_SET_BIOS_BOOT_MODE                 "SetBiosBootMode"
#define METHOD_SET_BIOS_BOOT_MODE_SW              "SetBiosBootModeSw"
#define METHOD_GET_BOOT_ORDER                     "GetBootOrder"

#define METHOD_BIOS_SET_PRINT_FLAG                          "SetBiosPrintFlag"    //新需求AR586D337A-B0EF-49da-B718-473CC926DAA9
#define METHOD_BIOS_GET_SETTING_EFFECTIVE_STATUS            "GetBiosSettingEffectiveStatus"
/* BEGIN: Added by Yangshigui YWX386910, 2017/9/6   问题单号:DTS2017082312480*/
#define METHOD_BIOS_NOTIFY_PCIE_INFO                     "NotifyPCIeInfo"
/* END:   Added by Yangshigui YWX386910, 2017/9/6 */
#define METHOD_BIOS_SET_FLASH_ID                     "SetSPIFlashId"
#define METHOD_BIOS_IMPORT_CERT                      "ImportCert"
#define METHOD_BIOS_RESET_CERT                       "ResetCert"
#define METHOD_BIOS_GET_CERTIFICATES                 "GetCertificates"
#define METHOD_BIOS_GET_HTTPS_CERTIFICATES           "GetHttpsCertificates"
#define METHOD_BIOS_GET_SECUREBOOT                   "GetSecureBoot"
#define METHOD_BIOS_SET_SECUREBOOT                   "SetSecureBoot"

/* AI_MGMT 模块对象及属性 */
#define MODULE_NAME_AI_MGMT                         "ai_mgmt"
#define AI_MGMT_CLASS_NAME                          "AiMgmt"
#define METHOD_AIMGMT_INFERENCE_ASYNC               "InferenceAsync"
#define METHOD_AIMGMT_INFERENCE_ASYNC_CALLBACK      "InferenceAsyncCallback"
#define METHOD_AIMGMT_INFERENCE_ASYNC_PCIE_CALLBACK      "InferenceAsyncPcieCallback"
#define METHOD_AIMGMT_INFERENCE_ASYNC_DIMM_CALLBACK      "InferenceAsyncDimmCallback"

#define  SMBIOS_CLASS_NAME               "SmBios"
#define  SMBIOS_OBJECT_NAME              SMBIOS_CLASS_NAME
#define  SMBIOS_FILE_NAME                ("FileName")
#define  SMBIOS_DIFF_FILE_NAME            ("DiffFileName")
#define  SMBIOS_CHANGE_FLAG              ("FileChange")
#define  BIOS_SERIAL_NUM_NAME            ("SerialNumber")
#define  XAGENT_REGSMBIOS_NAME               "XAgentRegSmBiosStatus"

/* SMBIOS */
#define CLASS_NAME_SMBIOS                                  SMBIOS_CLASS_NAME
#define OBJ_NAME_SMBIOS                                    SMBIOS_OBJECT_NAME
#define PROTERY_SMBIOS_SERIALNUM                           BIOS_SERIAL_NUM_NAME
#define PROPERTY_SMBIOS_STATUS_VALUE                       ("SmBiosStatus")
#define PROPERTY_SMBIOS_UPDATED_FLAG                      ("SmBiosUpdatedFlag")
#define PROPERTY_SMBIOS_VERSION                            ("Version")
#define PROPERTY_SMBIOS_SKUNUMBER                          ("SKUNumber")
#define PROPERTY_SMBIOS_FAMILY                             ("Family")

#define METHOD_SMBIOS_SET_SERIAL                           "SetSerialNumber"
#define METHOD_SMBIOS_SET_VERSION                          "SetVersion"
#define METHOD_SMBIOS_SET_SKUNUMBER                        "SetSKUNumber"
#define METHOD_SMBIOS_SET_FAMILY                           "SetFamily"


#define CLASS_NAME_PERIPHERAL_DEVICE_SENSOR   "PeripheralDeviceSensor"
#define PROPERTY_P_DEVICE_SENSOR_NAME             "SensorName"
#define PROPERTY_P_DEVICE_SENSOR_VALUE             "Value"

/* Upgrade模块对象及属性 */
#define PFN_CLASS_NAME                   "Upgrade"
#define OBJECT_PFN                       (PFN_CLASS_NAME)
#define PFN_UPGRADE_STATE_NAME           ("UpgradeState")
#define PFN_UPGRADE_MODE_NAME            ("UpgradeMode")
#define PFN_COMP_ID_NAME                 ("CompnentID")
#define PFN_ERR_CODE_NAME                ("ErrCode")
#define PFN_DEGRADE_NAME                 ("Degrade")
#define PFN_BUS_CHANGE_NAME              ("BusChange")
#define PFN_BIOS_SPI_CHANNEL              ("BiosSPIChannel")
#define PFN_UPGRADE_CPLD_NAME            ("ContrlCpld")
#define PFN_VALIDATE_CPLD_NAME           ("ValidateCpld")
#define PFN_BIOS_FLASH_SIZE              ("BIOSFlashSize")
#define PFN_UPGRADE_INFLAG_NAME          ("InUpgradeFlag")
#define PROPERTY_PFN_ROLL_STATE          ("RollState")
#define PFN_PREROLLBACK_FLAG             "PreRollbackFlag"
#define PFN_PREUPGRADE_FLAG               "PreUpgradeFlag"
#define PFN_UPGRADED_FLAG                 "UpgradedFlag"
#define PFN_ALLOW_BMC_POWEROFF_FLAG       "UpgradeAllowBMCPoweroffFlag"
#define PFN_ALLOW_BMC_POWEROFF_TIME       "UpgradeAllowBMCPoweroffTime"
#define PFN_ACTIVATE_UPGRADE_BIOS_FLAG    "ActivateBIOSUpgradeFlag"
#define PFN_ACTION_AFTER_UPGRADE        "ActionAfterUpgrade"
#define PFN_BMC_UPGRADE_RESULT_REDA_STATUS  "BMCUpgradeResultReadStatus"
#define PFN_UPGRADE_FILE_MASTERKEY_ID     "UpgradeFileMasterKeyId"
#define PFN_DOWNGRADE_DISABLED            "DowngradeDisabled"
#define PFN_BMC_SYNC_STATE                "BMCSyncState"
#define PFN_BMC_EXPECTED_BOOTORDER        "ExpectedBootOrder"

#define PFN_UPGRADE_PERIPHERAL_CPLD_NAME            ("ContrlPeripheralCpld")
#define PFN_VALIDATE_PERIPHERAL_CPLD_NAME           ("ValidatePeripheralCpld")


/* BEGIN: Added by chenqianqian c00416525, 2017/10/24*/
#define METHOD_PFN_VALID_CPLD            "ValidCpldManu"           //   手动生效CPLD的接口（MM920）

/*STRAT by wangenmao 2016-12-5 PN: AR-0000256852-001-016*/
#define PFN_FINISH_CPLD_FLAG                 "FinishCpldFlag"
/*END by wangenmao 2016-12-5 PN: AR-0000256852-001-016*/

/* BEGIN for ARB7F63698-2463-45ba-AEEB-9A0B21853DA2 : Added by ouluchun 00294050 2015-11-11 */
#define PFN_UPGRADE_DETAILED_RESULTS       ("UpgradeDetailedResults")
#define PFN_UPGRADE_RESULTS_CODE           ("UpgradeResultsCode")
/* END for ARB7F63698-2463-45ba-AEEB-9A0B21853DA2 : Added by ouluchun 00294050 2015-11-11 */

/* BEGIN: Added by zhongqiu 00356691, 2016/11/23   PN:AR-0000267460-001-003*/
#define PFN_COMP_STR                    ("CompnentStr")
/* END:   Added by zhongqiu 00356691, 2016/11/23   PN:AR-0000267460-001-003*/

/* BEGIN: Added by l00389091, 2018/02/03  DTS2018020308211 */
#define PFN_VALIDATING_CPLD_NAME           ("ValidatingCpldFlag") /* 表是CPLD正在生效，此标志置位时，从CPLD获取的值有异常 */
/* END: l00389091, 2018/02/03 */

/* BEGIN: DTS2018030802211. Modified by f00381872, 2018/3/9 */
#define PFN_FORCE_UP_BIOS_STATE          "ForceUpBiosState"
/* END: DTS2018030802211. Modified by f00381872, 2018/3/9 */
#define  METHOD_PFN_SET_UP_FLAG          "SetUpgradeFlag"      /* 设置PME备份版本 */
#define  METHOD_PFN_INITIATE_UP          "InitrialUpgrade"
#define  METHOD_PFN_INITIATE_IPMI_UP     "InitrialIpmiUpgrade"
#define  METHOD_PFN_GET_UPGRADE_STATUS   "GetUpgradeStatus"
#define  METHOD_PFN_GET_CPLD_VALID       "GetCpldValidFlag"
#define  METHOD_PFN_SET_CPLD_VALID_ACTIVE_MODE       "SetCpldValidActiveMode"
#define  METHOD_PFN_SET_ACTIVATE_BIOS_UPGRADE_FLAG  "SetActivateBIOSUpgradeFlag"
#define  METHOD_PFN_SET_ACTION_AFTER_UPGRADE  "SetActionAfterUpgrade"
#define  METHOD_PFN_GET_CACHE_UPGRADE_FILE_STATUS   "GetCacheUpgradeFileStatus"
#define  METHOD_PFN_SET_IERR_DIAG_FAIL_FLAG         "SetIErrDiagFailFlag"
#define  METHOD_PFN_REMOTE_UP                       "RemoteUpgrade"
#define  METHOD_PFN_BIOS_HOT_UPGRADE                "BiosHotUpgrade"

/* BEGIN: Added by z00352904, 2017/3/15 9:38:0  PN:AR-0000276594-002-002 */
#define  METHOD_PFN_IMPORT_CRL_FILE      "ImportCRLFile"
/* END:   Added by z00352904, 2017/3/15 9:38:3*/
#define  METHOD_PFN_CHANGE_BIOS_MUX      "ChangeBiosMux"
#define  METHOD_PFN_GET_COMPONENTS_INFO  "GetComponentsInfo"
#define  METHOD_PFN_SET_DOWNGRADE_DISABLED          "SetDowngradeDisabled"
/* BEGIN: Added by ouluchun 00294050, 2015/3/28   问题单号:DTS2015032801621*/
#define METHOD_PFN_INIT_DOWNLOAD_FIRMWARE  "InitDownloadFirmware"
/* END:   Added by ouluchun 00294050, 2015/3/28 */

/* BEGIN: Modified by h00422363, 2018/4/25 16:27:42   问题单号:DTS2018042506224  HMM板整框升级ch121 v3，概率性升级失败，优化BMC升级过程，升级完成后BMC复位前延 */
#define PFN_UPGRADE_DELAY_REBOOT_TIME   "UpgradeDelayRebootTime"
/* END:   Modified by h00422363, 2018/4/25 16:27:47 */

/* BEGIN: Added by chenqianqian c00416525, 2017/10/24*/
#define METHOD_PFN_VALID_CPLD                              "ValidCpldManu"      //   手动生效CPLD的接口（MM920）
/* END:   Added by chenqianqian c00416525, 2017/10/24*/

/* upgrade */
#define CLASS_NAME_UPGRADE                                 PFN_CLASS_NAME
#define OBJ_NAME_UPGRADE                                   OBJECT_PFN
#define PROTERY_UPGRADE_PROCESS                            PFN_UPGRADE_STATE_NAME  //升级进度
#define PROTERY_ROLL_STATE                                 PROPERTY_PFN_ROLL_STATE
#define PROTERY_UPGRADE_MODE                               PFN_UPGRADE_MODE_NAME //mode 0:finish 1:prepare 2:process 3:activity
#define PROTERY_UPGRADE_COMPNENTID                         PFN_COMP_ID_NAME
#define PROTERY_UPGRADE_ERRCODE                            PFN_ERR_CODE_NAME
#define METHOD_UPGRADE_ROLLBACK                            "SetRollback"
#define METHOD_UPGRADE_INITUPGRADE                         METHOD_PFN_INITIATE_UP
#define METHOD_UPGRADE_FINISH                              PFN_FINISH_METHORD_NAME
#define PROTERY_UP_VALID_CPLD                              "ValidCpldFlag"
/* BEGIN: Added by hucheng 0098892, 2017/9/20  虚拟u盘 sp升级入口，问题单号:AR-0000276587-001-006 */
#define METHOD_UPGRADE_SP                                  "UpgradeSP"
#define METHOD_UPGRADE_SP_PREPARE                          "PrepareUpgradeSP"
#define METHOD_UPGRADE_SP_LINKFILE                         "LinkUpgradeFile"
#define METHOD_UPGRADE_SP_GETMAJORVERSIONFLAG              "GetMajorVersionFlag"
/* END:   Added by hucheng 0098892, 2017/9/20 */

/* BEGIN: 2019-4-12 tianpeng twx572344 PN:DTS2019032601140 */
#define METHOD_UPDATE_SP_SCHEMA_PREPARE                       "PrepareUpdateSPSchema"
/* END:   2019-4-12 tianpeng twx572344 PN:DTS2019032601140 */
#define METHOD_UPDATE_UPGRADE_FILE                            "UpdateUpgradeFile"

#define METHOD_UPGRADE_RECOVER_BIOS                            "RecoverBios"

#define METHOD_UPGRADE_IBMA_STICK                               "UpgradeIbmaStick"
#define METHOD_UPGRADE_IBMA_STICK_PREPARE                       "PrepareUpgradeIbmaStick"
#define METHOD_UPGRADE_INSTALLALE_IBMA                               "UpgradeInstallableIbma"
#define METHOD_UPGRADE_INSTALLALE_IBMA_PREPARE                       "PreUpgradeInstallableIbma"
#define METHOD_UPGRADE_SET_SYNC_STATE                          "SetSyncState"
/* Watchdog类 */
#define CLASS_WATCHDOG                   "Watchdog2"
#define OBJECT_WATCHDOG                  "watchdog20"
#define PROPERTY_WD_NOTLOG               "NotLog"
#define PROPERTY_WD_TIMER_USE            "TimerUse"
/* BEGIN: Modified by zhanglei wx382755, 2017/9/7   PN:DTS2017072108963 */
#define PROPERTY_WD_TIMER_USE_STR     "TimerUseStr"
/* END:   Modified by zhanglei wx382755, 2017/9/7 */
#define PROPERTY_WD_RUNNING              "Running"
#define PROPERTY_WD_PRE_TM_INTR_F        "PreTmoutIntF"
#define PROPERTY_WD_PRE_TM_INTR          "PreTmoutInt"
#define PROPERTY_WD_PRE_TM_INTR_EN       "PreTmoutIntEn"
#define PROPERTY_WD_TM_ACTION            "TmoutAction"
#define PROPERTY_WD_OEM_EXP_F            "OemExpirFlag"
#define PROPERTY_WD_SMS_EXP_F            "SmsExpirFlag"
#define PROPERTY_WD_OSLOAD_EXP_F         "OsloadExpirFlag"
#define PROPERTY_WD_POST_EXP_F           "PostExpirFlag"
#define PROPERTY_WD_FRB_EXP_F            "Frb2ExpirFlag"
#define PROPERTY_WD_TM_FLAG              "WdTmFlag"
#define PROPERTY_IS_IPC_COMM_NORMAL        "IsIPCCommNormal"

#define METHOD_SET_WATCHDOG  "SetWDT"

#define CLASS_UPGRADE_FRUD                "FrudUpgrade"
#define PROPERTY_UID                      "Uid"
#define PROPERTY_SPEC_VERSION             "SPECVersion"
#define PROPERTY_WRITE_PROTECTION_STATUS  "WriteProtectStatus"
#define PROPERTY_HWSR_VERSION             "HWSRVersion"

/* Hotswap类 */
#define CLASS_HOTSWAP                    "Hotswap"
#define OBJECT_HOTSWAP                   "hotswap0"
#define PROPERTY_HS_FRUID                "FruID"
#define PROPERTY_HS_CRU_STATE            "CurrentHotswapState"
#define PROPERTY_HS_LAST_STATE           "LastHotswapState"
#define PROPERTY_HS_CAUSE                "HotswapCause"
#define PROPERTY_HS_MIX_STATE            "MixHotswapState"

#define CLASS_DATANOTIFY_HOTSWAP         "DataNotifyHotswap"

/* 存储新增StoragePayload类 */
#define CLASS_NAME_STOR_PAYLOAD             "StoragePayload"
#define OBJECT_STORPAYLOAD                  CLASS_NAME_STOR_PAYLOAD
#define PROPERTY_STORPAYLOAD_STANDBYSTATE   "StandbyState"
#define PROPERTY_OS_BOOT_FLG                "OsBootFlg"              /* 大系统启动标志 */
#define PROPERTY_BIOS_BOOT_FLG              "BiosBootFlg"            /* bios启动标志 */
#define PROPERTY_LAST_KO_OVER_FLG           "LastKoOverFlg"          /* 模块加载完成标志 */
#define PROPERTY_BACK_POWER_RUNNINTSTATE    "BackPowerRunningState"  /* 大系统后电运行状态 */
#define PROPERTY_MASTER_FLAG_STATE          "MasterFlagState"        /* 控制板主备状态查询 */
#define PROPERTY_ENCL_TYPE                  "EnclosureType"          /* 框类型 */
#define PROPERTY_BOARD_PRESENT_STATE        "BoardPresentState"      /* 单板在位状态查询，可以查询所有的单板在位 */
#define PROPERTY_BOARD_CUR_HOT_SWAPSTATE    "CurHotSwapState"        /* 存储节点热插拔状态 */
#define PROPERTY_POWER_ON                   "PowerOn"
#define PROPERTY_POWER_ON_WP                "PowerOnWp"
#define PROPERTY_POWER_OFF                  "PowerOff"
#define PROPERTY_POWER_OFF_WP               "PowerOffWp"
#define PROPERTY_POWER_SWITCH               "PowerSwitch"
#define PROPERTY_POWER_SOFT_OFF_WP          "NotifyCpldSoftOffWp"
#define PROPERTY_POWER_SOFT_OFF             "NotifyCpldSoftOff"
#define PROPERTY_POWER_SIM_ACDOWN           "SimACDown"
#define PROPERTY_POWER_SIM_ACDOWN_WP        "SimACDownWP"
#define PROPERTY_PHYSICAL_SHORT_BUTTON_CL   "ShortButtonClear"
#define PROPERTY_PHYSICAL_LONG_BUTTON       "PhySicalButtonLong"
#define PROPERTY_LOCAL_BOARD_PRESENT        "LocalBoardPresent"    /* 本板在位 */
#define PROPERTY_LOCAL_PRESENT_CHANGE       "LocalPresentChange"   /* 本板在位变化，拔板备电场景用来判断单板拔插动作 */
#define PROPERTY_ACTION_AFTER_INSERT        "ActionAfterInsert"    /* 单板插入后动作 */
#define PROPERTY_DELAY_KILL_BBU_EN          "DelayKillBbuEn"       /* 延时关闭BBU使能 */
#define PROPERTY_PHYSICAL_LONG_BUTTON_CL    "LongButtonClear"
#define PROPERTY_POWER_OPER_FLAG            "CurPowerOperFlag"       /* 0x95 寄存器标志，用于首次上电 */
#define PROPERTY_SEND_POWER_CMD             "SendDesPowerCmd"        /* 主控向对控发送命令 */
#define PROPERTY_REC_POWER_CMD              "RecDesPowerCmd"         /* 接收命令，从控例测解析命令 */
#define PROPERTY_IS_OFFING_STATE            "IsOffingState"     /* 表示当前是否正在下电过程中 */
#define PROPERTY_CLEAR_ABNORMAL_PWR         "ClearAbnormalPwr"  /* 用于清除异常上电状态 */
#define PROPERTY_PWR_FAIL_REG_VPP           "PwrFailRegVpp"   /* 异常掉电辅助定位 */
#define PROPERTY_PWR_FAIL_REG_VTT           "PwrFailRegVtt"   /* 异常掉电辅助定位 */
#define PROPERTY_PWR_FAIL_REG_VCC           "PwrFailRegVcc"   /* 异常掉电辅助定位 */
#define PROPERTY_PWR_FAIL_REG_DEBUG         "PwrFailRegDebug"
#define PROPERTY_ACDOWN_EVT                 "AcDownEvt"       /* AC掉电事件记录 */
#define PROPERTY_ACDOWN_EVT_CL              "AcDownEvtClear"  /* 清除AC掉电事件记录 */
#define PROPERTY_SYS_DISK0_POW_STATE        "SysDisk0PowState"   /* 系统盘0上电状态 */
#define PROPERTY_SYS_DISK1_POW_STATE        "SysDisk0PowState"   /* 系统盘1上电状态 */
#define PROPERTY_POWER_MODE                 "PowerMode"      /* 系统电源配置模式 */
#define PROPERTY_PSU_NUM_CONFIG             "PsuNumConfig"   /* 上电电源个数门限，低于门限不上电 */
#define PROPERTY_PSU_NOT_ENOUGH             "WorkingPSUNotEnough"  /* 工作电源个数不满足要求，1：不满足 0：满足 */
#define PROPERTY_IMU_FORBID_PWRUP_FLAG      "IMUForbidFlag"   /* IMU高温禁止上电标记 */
#define PROPERTY_FORBID_PWRON               "IMUForbidPwrOn"     /* 禁止单板上电，用于单板CPU发生过高温， */
#define PROPERTY_POWER_CONFIG               "PowerRedundanceConfig"     /* 电源冗余配置 */
#define PROPERTY_POWER_CMD_FLAG             "PowerCmdFlag"
#define PROPERTY_BOARD_EQUIP_MODE           "BoardEquipMode"            /* 单板装备模式 */
#define PROPERTY_PEER_PWR_STATE             "PeerSmmBoardPwrState"              /* 对控SMM管理板状态 */
#define PROPERTY_ENCL_WORK_STATE            "EnclWorkState"  /* 此处主要用于直通板检测是否处于极致待机 */
#define METHOD_HANDLE_BIOS_PWROFF_CMD       "HandleBiosPwrOffCmd"       /* 处理BIOS关机命令 */
#define METHOD_GET_PWROFF_FLAG              "GetPowerOffFlag"
#define PROPERTY_WATCH_DOG_STATUS           "WatchDogStatus"  /* 看门狗状态开关 */
#define PROPERTY_CURRENT_RESTART_CAUSE      "CurrentRestartCause"
#define PROPERTY_LAST_RESTART_CAUSE         "LastRestartCause"
#define PROPERTY_BEFORE_LAST_RESTART_CAUSE  "BeforeLastRestartCause"
#define PROPERTY_RESTART_CAUSE_ASSIST_INFO  "RestartCauseAssistInfo"
#define PROPERTY_RESTART_COUNT              "RestartCount"
#define PROPERTY_BBU_DISCHARGE_SWITCH       "BbuDischargeSwitch"
#define PROPERTY_OS_BOOT_STATE              "OsBootState"
#define PROPERTY_FORCE_POWER_OFF_TYPE       "ForcePowerOffType" /* 强制下电类型 */

/* FruPayLoad类 */
#define CLASS_FRUPAYLOAD                    "FruPayload"
#define OBJECT_FRUPAYLOAD                   CLASS_FRUPAYLOAD
#define PROPERTY_FRUPAYLOAD_POWERSTATE      "PowerState"
#define PROPERTY_FRU_PAYLOAD_FRUID          "FruID"

#define METHOD_POWER_CYCLE                  "PowerCycle"
#define METHOD_COLD_RESET                   "ColdReset"
#define METHOD_WARM_RESET                   "WarmReset"
#define METHOD_FRU_DIAG_INTR                "DiagInterrupt"
#define METHOD_GRACE_REBOOT                 "GracefulReboot"
#define METHOD_FRU_CONTROL                   "FruControl"

/*PayLoad类 */
#define CLASS_PAYLOAD                       "Payload"
/*chassispayload类*/
#define CLASS_CHASSISPAYLOAD                "Payload"/* 鉴于很多模块用了这个宏,此宏保留,以后新增代码使用CLASS_PAYLOAD这个宏*/
#define OBJECT_CHASSISPAYLOAD               "ChassisPayload"/* 鉴于很多模块用了这个宏,此宏保留,以后代码在使用上尽量不要用全局对象 */
#define PROPERTY_POWER_REST_POLICY          "PowerRestorePolicy"
#define PROPTERY_POWER_ON_STRATEGY_EXCEPTIONS "PowerOnStrategyExceptions"
#define PROPERTY_POWER_CTRL_FAULT           "PowerCtrlFault"
#define PROPERTY_PAYLOAD_RESET_FLAG         "ResetFlag"
#define PROPERTY_POWER_BUTTON_FLAG          "PwrButtonFlag"
#define PROPERTY_POWER_ON_TM                "PowerOnTimeout"
#define PROPERTY_PAYLOAD_CHASSPWR_STATE     "ChassisPowerState"
#define PROPERTY_PAYLOAD_CHASSISID          "ChassisID"
#define PROPERTY_PAYLOAD_RUNNING_STATE      "RunningState"
#define PROPERTY_PAYLOAD_RESTART_STATE      "RestartState"
#define PROPERTY_PAYLOAD_RESTART_CAUSE      "RestartCause"
#define PROPERTY_PAYLOAD_RESTART_CHAN       "RestartChannel"
#define PROPERTY_PAYLOAD_RESTART_VALUE      "RestartValue"
// 同RestartValue，但针对重启原因是OEM的情况，重启原因更详细
// 之所以不用RestartValue，是因为受限于IPMI规范，不能影响传感器的值
#define PROPERTY_PAYLOAD_OEM_RESTART_VALUE  "OEMRestartValue"
#define PROPERTY_PAYLOAD_PWRSIG_DROP        "PwrOkSigDrop"
#define PROPERTY_PAYLOAD_HOST_START_TM      "HostStartTimeout"
#define PROPERTY_PAYLOAD_PWROFF_TM          "PowerOffTimeout"
#define PROPERTY_PAYLOAD_PWROFF_TM_DEF      "PowerOffTmDefault"
#define PROPERTY_PAYLOAD_PWROFF_TM_MAX      "PowerOffTmMax"
#define PROPERTY_PAYLOAD_PWROFF_TM_MIN      "PowerOffTmMin"
#define PROPERTY_PAYLOAD_PWRDELAY_MODE      "PowerDelayMode"
#define PROPERTY_PAYLOAD_PWRDELAY_COUNT     "PowerDelayCount"
#define PROPERTY_PAYLOAD_PWROFF_TM_EN       "PowerOffTimeoutEN"
#define PROPERTY_PAYLOAD_PWR_BTN_PWR_ON_EN  "PwrButtonPwrOnEn"
#define PROPERTY_PAYLOAD_INSERT_EN          "CheckInsertEn"
#define PROPERTY_PAYLOAD_PWR_STATUS         "PowerStatus"
#define PROPERTY_PAYLOAD_PWR_GD             "PowerGd"
#define PROP_PAYLOAD_FT_PWR_GD              "FTPowerGd"
#define PROPERTY_PAYLOAD_ACPI_STATUS        "AcpiStatus"
#define PROPERTY_PAYLOAD_ACPI_VALUE         "AcpiValue"
#define PROPERTY_FORCED_POWER_CYCLE         "ForcedPowerCycle"
#define PROPERTY_PAYLOAD_PWR_BTN_SHORT      "PwrButtonShort"
#define PROPERTY_PAYLOAD_PWR_BTN_LONG       "PwrButtonLong"
#define PROPERTY_PAYLOAD_SYS_RST_SRC        "SysResetSrc"
#define PROPERTY_PAYLOAD_SYS_RST_FLAG       "SysResetFlag"
#define PROPERTY_PAYLOAD_NMI_INTR           "NMIInterrupt"
/* BEGIN: Added by liuxiangyang, 2016/3/31   DTS2015092504966 */
#define PROPERTY_PAYLOAD_PWR_ORIG_BEF_AC    "OriginalPwrBeforeACLost"
/* END: Added by liuxiangyang, 2016/3/31   DTS2015092504966 */
#define PROPERTY_PAYLOAD_PWR_BEF_AC         "PwrBeforeACLost"
#define PROPERTY_PAYLOAD_HARD_RESET_WP      "HardResetWp"
#define PROPERTY_PAYLOAD_HARD_RESET         "HardReset"
#define PROPERTY_PAYLOAD_PWR_BTN_LOCK       "PwrButtonLock"
#define PROPERTY_PAYLOAD_PWR_BTN_LOCK_PER   "PwrButtonLockPer"
#define PROPERTY_PAYLOAD_PWR_BTN_EVT        "PwrButtonEvt"
#define PROPERTY_PAYLOAD_INSERT_STATUS0     "InsertStatus0"
#define PROPERTY_PAYLOAD_INSERT_STATUS1     "InsertStatus1"
#define PROPERTY_PAYLOAD_CTL_OPTION         "FruCtlOption"          /* FRU的control 支持掩码 */
#define PROPERTY_PAYLOAD_FRUID              "FruID"
#define PROPERTY_PAYLOAD_STABLE_STATUS      "BoardStableStatus"
#define PROPERTY_BIOS_MEASURE_FLAG          "BiosMeasureFlag"
#define PROPERTY_PAYLOAD_PWRON_CTRL         "IsPowerCtrl"
#define PROPERTY_PAYLOAD_PWRON_TM_SRC       "PowerOnTmSrc"
#define PROPERTY_PAYLOAD_BIOS_SWTICH        "BiosSwitch"
#define PROPERTY_PAYLOAD_BOOT_TMOUT_ACTION        "BootTmoutAction"
#define PROPERTY_PAYLOAD_PWR_DROP_SRC       "PwrSigDropSrc"
#define PROPERTY_PAYLOAD_BIOS_BOOT          "BiosBootStatus"
#define PROPERTY_PAYLOAD_CHASSIS_CTRL_ACTION     "ChassisCtrlAction"
#define PROPERTY_PAYLOAD_SYS_BOOT_EVENT     "SYSBootEvent"
#define PROPERTY_PAYLOAD_PWR_BTN_SHIEID             "PwrButtonShield"
#define PROPERTY_PAYLOAD_PWR_BTN_SHIEID_SUPPORT     "PwrButtonShieldSupport"
#define PROPERTY_PAYLOAD_PHY_BTN_SHORT_EVT     "PhyButtonShortEvt"
#define PROPERTY_PAYLOAD_PHY_BTN_SHORT_CLEAR   "PhyButtonShortClear"

/* BEGIN: Added by liuchong, 2016/3/9   问题单号:DTS2016052100536*/
#define PROPERTY_PAYLOAD_CHASSIS_CTL_OPTION "ChassisCtlOption"
#define PROPERTY_PAYLOAD_ACTIVE_CTL_OPTION  "ActiveCtlOption"
/* END:   Added by liuchong, 2016/3/9 */
/** BEGIN: Added by l00272597, 2016/12/1   PN:DTS2016120100518  */
#define PROPERTY_PAYLOAD_CHASSIS_LAST_POWER_EVENT     "ChassisLastPowerEvent"
/** END:   Added by l00272597, 2016/12/1 */
#define PROPERTY_PAYLOAD_FORBID_POWERON_FLAG    "ForbidPowerOnFlag" //目前该标志只用来表示BIOS升级时禁止上电。如果新增其他场景，需要修改相关代码
#define PROPERTY_POWERSTATE_AFTER_BMC_RESET    "PowerstateAfterBMCReset" /* BMC启动后的上电状态 */
/* BEGIN: Added by zengrenchang(z00296903), 2017/4/22   问题单号:AR-0000276591-001-002*/
#define PROPERTY_BLADE_POWER_ON_PERMIT    "BladePowerOnPermit"
/* END:   Added by zengrenchang(z00296903), 2017/4/22 */

/* BEGIN: Added by z00296903, 2017/10/27   问题单号:DTS2017102703397*/
#define PROPERTY_WAIT_POWER_ON_PERMIT    "WaitPowerOnPermit"
/* END:   Added by z00296903, 2017/10/27 */

/* BEGIN: DTS2017092711331. Modified by f00381872, 2017/11/14 */
#define PROPERTY_PWR_CYCLE_INTERVAL      "PowerCycleInterval"
/* END: DTS2017092711331. Modified by f00381872, 2017/11/14 */
/* BEGIN: Added by liumancang, 2017/12/14 18:34:3   需求单号:AR.SR.SFEA02119718.002.001 */
#define PROPERTY_LAST_POWERCYCLERESULT           "LastPowerCycleResult"
#define PROPERTY_CURRENT_POWERCYCLERESULT    "CurrentPowerCycleResult"
/* END:   Added by liumancang, 2017/12/14 18:34:33 */

#define PROPERTY_THERMAL_TRIP_POWERPOLICY    "ThermalTripPowerPolicy"
#define PROPERTY_THERMAL_TRIP_TIMEDELAY    "ThermalTripTimeDelay"

/* Start by huangjiaqing RM201710270805 20180421 */
#define PROPERTY_PAYLOAD_AC_CYCLE    "ACCycle"
#define PROPERTY_PAYLOAD_AC_CYCLE_PROTECT    "ACCycleProtect"
#define METHOD_PAYLOAD_AC_CYCLE    "ACCycle"
/* Start by huangjiaqing RM201710270805 20180421 */
#define METHOD_PAYLOAD_PWR_RESTORE_DONE_FLAG  "GetPwrRestoreDoneFlag"

#define METHOD_SET_HOST_START               "SetHostStart"
#define METHOD_PS_ON_LOCK_CLEAR               "ClearPowerOnLock"
#define METHOD_PWR_ON                       "PowerOn"
#define METHOD_PWR_OFF                      "PowerOff"
#define METHOD_FORCE_PWR_OFF                "ForcePowerOff"
#define METHOD_PWR_CYCLE                    "PowerCycle"
#define METHOD_HARD_RESET                   "HardReset"
#define METHOD_HARD_RESET_WITH_PARA         "HardResetWithPara"
#define METHOD_DIAG_INTR                    "DiagInterrupt"
#define METHOD_SOFT_PWR_OFF                 "SoftPowerOff"
#define METHOD_SET_PWR_REST_POLICY          "SetPowerRestorePolicy"
#define METHOD_SET_POWER_ON_STRATEGY_EXCEPTIONS "SetPowerOnStrategyExceptions"
#define METHOD_EXE_PWR_REST_POLICY          "ExePowerRestorePolicy"
#define METHOD_SET_PWR_CYCLE_INTER          "SetPowerCycleInterval"
#define METHOD_SET_PWR_OFF_TM               "SetPowerOffTimeout"
#define METHOD_SET_PWR_OFF_TM_EN            "SetPowerOffTimeoutEN"
#define METHOD_SET_PWR_BTN_LOCK             "SetPowerButtonLock"
#define METHOD_GET_PWR_BTN_LOCK             "GetPowerButtonLock"
#define METHOD_SET_PWR_CTRL                 "SetPowerCtrl"
#define METHOD_SET_FRU_ACTIVE               "SetFRUActive"
#define METHOD_SET_PWR_LEVEL                "SetPowerLevel"
#define METHOD_SET_FORBID_POWERON_FLAG      "SetForbidPowerOnFlag"
#define METHOD_SET_BOARD_POWERDROP_FLAG     "SetBoardPowerDropFlag"
#define METHOD_PAYLOAD_CLEAR_RESTART_VALUE  "PayloadClearRestartValue"
#define METHOD_PAYLOAD_SET_LEAK_PWROFF_POLICY "SetLeakPowerOffPolicy"
#define METHOD_SET_PWR_DELAY_MODE           "SetPowerDelayMode"
#define METHOD_SET_PWR_DELAY_COUNT          "SetPowerDelayCount"
#define METHOD_SET_POWERSTATE_AFTER_BMC_RESET  "SetPowerstateAfterBMCReset"
#define METHOD_DFT_RESET_OS_TEST            "DftResetOsTest"
#define METHOD_SET_THERMAL_TRIP_POWERPOLICY            "SetThermalTripPowerPolicy"
#define METHOD_SET_THERMAL_TRIP_TIMEDELAY            "SetThermalTripTimeDelay"

/* BEGIN: Added by liumancang, 2017/12/14 18:34:3   需求单号:AR.SR.SFEA02119718.002.001 */
#define CLASS_POWER_ON_CRITERIA                      "PowerOnCriteria"
#define CLASS_POWER_OFF_CRITERIA                      "PowerOffCriteria"
#define PROPERTY_POWER_CYCLE_CRITERIA_IS_MET            "IsMet"
/* END:   Added by liumancang, 2017/12/14 18:34:33 */

/*Begin:IPMI 模块依赖的类和对象 */
#define IPMI_CHANNEL_CLASS_NAME               "IPMIChannel"
#define IPMI_CHANNEL_REMOTE_OBJ_NAME          "channel_remote"
#define IPMI_CHANNEL_AS_OBJECT                "channel_0"
#define PROPERTY_IPMITRAGET_TYPE              "ChanType"
#define PROPERTY_TARGET_INSTANCE              "Instance"
#define PROPERTY_IPMITRAGET_CHAN_PRO          "ChanPro"
#define PROPERTY_IPMITRAGET_CHAN_NUM          "ChannelID"
#define PROPERTY_IPMITRAGET_DEST_ADDR         "DestAddr"
#define PROPERTY_IPMITRAGET_SRC_ADDR          "SrcAddr"
#define PROPERTY_IPMITRAGET_MED_TYPE          "MedType"
#define METHOD_GET_PORT_ID                    "GetPortId"

/*RMCP  类名/属性*/
#define RMCP_CLASS_NAME                       "RmcpConfig"
#define RMCP_OBJ_NAME                         "rmcp"
#define PROPERTY_RMCP_PORT1                   "Port1"
#define PROPERTY_RMCP_PORT2                   "Port2"
#define PROPERTY_RMCP_SERVICE_FLAG            "ServiceFlag"
#define PROPERTY_LAN_STATE                    "LanState"
#define PROPERTY_LAN_PLUS_STATE               "LanPlusState"



#define METHOD_SET_RMCP_PORT                  "SetRmcpPort"
#define METHOD_SET_IPMILAN_PROTOCOL_STATE     "SetIPMILanProtocolState"

#define METHOD_SET_RMCP_SERVICE               "SetServiceState"
#define METHOD_SET_LAN_STATE                  "SetLanState"
#define METHOD_SET_LANPLUS_STATE              "SetLanlusState"

/* BEGIN: Added by ouluchun 00294050, 2015/7/23   PN:AR-0000956733*/
#define METHOD_TEST_IPMI_RPC                  "TestIpmiRpcReady"
/* END:   Added by ouluchun 00294050, 2015/7/23 */
#define METHOD_GET_SESSION_PRIVILEGE          "GetSessionPrivilege"

/*SOL串口*/
#define  SOL_CLASS_NAME                       "Com"
#define  BMC_COM_OBJ_NAME                     "BMCcom"
#define  PROPERTY_SOL_COM_ID                  "ComId"
#define  PROPERTY_SOL_COM_NAME                "ComName"
#define  PROPERTY_SOL_COM_SUPPORT             "ComSupport"
#define  PROPERTY_SOL_COM_ADAPTIVE            "AdaptiveFlag"
#define  PROPERTY_SOL_COM_BAUDRATE            "BaudRate"
#define  PROPERTY_SOL_COM_LOOPBACK            "LoopbackFlag"        //环回标识

#define  PROPERTY_SOL_ISSYSTEM                "IsSyscom"
#define  PROPERTY_SOL_DATADIRECTION           "SolDataDirection"
#define  PROPERTY_SOL_TXSIZE                  "TxSize"
#define  PROPERTY_SOL_RXSIZE                  "RxSize"
#define  PROPERTY_SOL_INDEX                   "SOLIndex"
#define  PROPERTY_SOL_CONNECTED_PORT          "SOLConnectedPort"
#define  PROPERTY_SOL_ENABLED                 "SolEnable"
#define  PROPERTY_SERIAL_LISTEN_SWITCH_ENABLED "SerialListenSwitchEnable"  // 支持串口捕获内容监听切换

/* BEGIN: Added by renlele r00355870, 2017/8/26 */
#define  PROPERTY_SOL_COM_INDEX               "ComIndex"            //串口序号
#define  PROPERTY_SOL_CLI_SUPPORT             "CliSupport"          //命令行串口支持标志
/* END:   Added by renlele r00355870, 2017/8/26 */
/* BEGIN: Added by renlele r00355870, 2017/12/12 PN:AR.SR.SFEA02109778.006.001*/
#define  PROPERTY_SOL_COM_CHANNEL             "ComChannel"
#define  PROPERTY_SOL_MAX_LOG_SIZE            "MaxLogSize"
#define  METHOD_SWITCH_COM_CHANNEL            "SwitchComChannel"    //对外方法
/* END:   Added by renlele r00355870, 2017/12/12 */
#define  METHOD_GET_LOOPBACK_CONFIRMED_FLG    "GetLoopbackConfirmedFlg"


/* BEGIN: Added by l00419687, 2018-08-03, PMI消息过滤类，用于需要在ipmi消息接受后过滤出指定消息的场景 */
#define  CLASS_NAME_IPMI_MESSAGE_FILTER       "IPMIMessageFilter"
#define  METHOD_SET_FILTER_CONDITION          "SetFilterCondition"
#define  METHOD_GET_FILTERED_MSG              "GetFilteredMessage"
/* END: Added by l00419687, 2018-08-03 */
#define  PROPERTY_IPMI_FIRE_WALL_STATUS       "FireWallStatus"
#define  PROPERTY_IPMI_FIRE_WALL_MODE         "FireWallMode"
#define  METHOD_SET_FIRE_WALL_STATUS          "SetFireWallStatus"
#define  METHOD_SET_FIRE_WALL_MODE            "SetFireWallMode"

/*串口管理类，每个对象是一种支持互连关系*/
#define CLASS_NAME_SERIAL_MANAGEMENT        "SeriaManagement"

#define PROPERTY_INDEX                      "Index"
#define PROPERTY_AVAILABILITY               "Availability"          // 串口管理可用(1)/不可用(0)
#define PROPERTY_CONNECT_STATUS             "ConnectStatus"         //连接状态
#define PROPERTY_SRC_SERIAL_ID              "SrcSerialID"           //源串口ID
#define PROPERTY_DEST_SERIAL_ID             "DestSerialID"          //目的串口ID
#define PROPERTY_SRC_SERIAL_NAME            "SrcSerialName"         //源串口名称
#define PROPERTY_DEST_SERIAL_NAME           "DestSerialName"        //目的串口名称
#define PROPERTY_SRC_SERIAL                 "SrcSerial"             //源串口uart/port
#define PROPERTY_DEST_SERIAL                "DestSerial"            //目的串口uart/port
#define PROPERTY_SER_LISTEN_SWITCH_CMD_ENABLE "SerListenSwitchCmdEnable" // 物理串口监听切换使能
#define PROPERTY_SER_LISTEN_SWITCH_CMD      "SerListenSwitchCmd"    // 物理串口监听切换命令
#define PROPERTY_CPLD_ACCESSOR              "CPLDAccessor"          //关联的CPLD Accessor
#define PROPERTY_CPLD_VALUE                 "CPLDValue"             //CPLD值
#define PROPERTY_REMOTE_ACCESSOR            "RemoteAccessor"        //远端寄存器

#define METHOD_SET_SERIAL_DIRECTION         "SetSerialDirection"    //对外方法

/*SOL管理类*/
#define CLASS_NAME_SOL_MANAGEMENT           "SOLManagement"
/* BEGIN: Added by renlele r00355870, 2017/8/26 */
#define OBJ_SOL_MANAGEMENT                  CLASS_NAME_SOL_MANAGEMENT
/* END:   Added by renlele r00355870, 2017/8/26 */
#define PROPERTY_DISCONNECT_ACCESSOR        "DisConnectAccessor"
#define PROPERTY_DISCONNECT_VALUE           "DisConnectValue"
/* BEGIN: Added by renlele r00355870, 2017/8/26 */
#define PROPERTY_SOL_MANAGEMENT_COMID       "ComId"           // SOL当前连接的COM口，0xff表示无效
#define PROPERTY_SOL_MANAGEMENT_TIMEOUT     "Timeout"
#define PROPERTY_SOL_OS_AUTO_LOCK_ENABLED   "OSAutoLockEnabled"
#define PROPERTY_SOL_OS_AUTO_LOCK_KEY       "OSAutoLockKey"
#define METHOD_OPEN_SOL                     "OpenSol"               //对外方法，命令行调用打开sol
#define METHOD_DEACTIVATE_SOL               "DeactivateSol"         //对外方法，踢除用户
#define METHOD_SET_SOL_TIMEOUT              "SetSolTimeout"         //对外方法，设置超时时间
#define METHOD_SET_OS_AUTO_LOCK_ENABLED     "SetOSAutoLockEnabled"
#define METHOD_SET_OS_AUTO_LOCK_KEY         "SetOSAutoLockKey"

/*SOLLINK 类定义*/
#define SOLLINK_CLASS_NAME                             "SolLink"
#define SOLLINK_PROPERTY_MODE                          "link_mode"
#define SOLLINK_PROPERTY_TYPE                          "link_type"  //会话类型(CLI、HMM、IPMI)
#define SOLLINK_PROPERTY_ID                            "link_id"
#define SOLLINK_PROPERTY_IP                            "link_ip"
#define SOLLINK_PROPERTY_USER_NAME                     "link_user_name"
#define SOLLINK_PROPERTY_LOGIN_TIME                    "link_login_time"
/* END:   Added by renlele r00355870, 2017/8/26 */

/*End:IPMI 模块依赖的类和对象 */


/* 指示灯类名定义 */
#define LED_CLASS_NAME                      "Led"
#define LED_TEST_CLASS_NAME                 "LedTest"
#define UID_BUTTON_CLASS_NAME               "UidButton"
#define INSERT_STATUS_CLASS_NAME            "InsertStatusLedCtrl"
#define I2C_LED_CLASS_NAME                  "I2cLed"
/* BEGIN: Added by l00419687, 2018-04-11 */
#define MANUAL_CONTROL_LED_CLASS_NAME       "ManualControlLed"
/* END: Added by l00419687, 2018-04-11 */

/* 指示灯对象名定义 */
#define SYSTEM_HEALTHY_LED                  "SysHealLed"
#define UID_LED                             "UIDLed"
#define CHASSIS_UID_LED                     "ChassisUIDLed" /* 框定位灯和板定位灯同时存在时使用 */
#define HLY_LED                             "HlyLed"
#define UID_BUTTON_EVENT                    "UidButtonEvent"
#define LED_TEST_OBJECT                     "LedTest"
#define INSERT_STATUS_LED_CTRL              "InsertStatusLedCtrl"
#define SYS_HEAL_LED_GREEN                  "SysHealLedGreen"
#define SYS_HEAL_LED_RED                    "SysHealLedRed"
#define SYSTEM_OFFING_LED                   "SysOffingLed"  /* OS下电过程指示灯 */

/* LED 面板 类 */
#define LED_PANEL_CLASS_NAME                "LedPanel"
#define LED_PANEL_PROP_PRESENT              "Present"
#define LED_PANEL_PROP_VALUE_MAP            "ValueMap"
#define LED_PANEL_PROP_DIGIT_CONTROL_RO     "DigitControl"
#define LED_PANEL_PROP_RELATED_TYPE_FLAG    "RelatedTypeFlag"
#define LED_PANEL_PROP_RELATED_NAME         "RelatedName"
#define LED_PANEL_PROP_LED_CONTROL_ROA      "LedControl"

/* 指示灯属性名定义 */
#define FRU_ID                              "FRUID"
#define LED_ID                              "ID"
#define NAME                                "Name"
#define MODE                                "Mode"
#define STATE                               "State"
#define COLOR                               "Color"
#define CONTROL_VALUE                       "CtrlValue"
#define ON_DURATION                         "OnDuration"
#define OFF_DURATION                        "OffDuration"
#define LCS_COLOR                           "LCSColor"
#define LCS_STATE                           "LCSState"
#define OS_COLOR                            "OSColor"
#define OS_STATE                            "OSState"
#define COLOR_CAPABILITIE                   "ColorCapabilitie"
#define DEFAULT_LCS_COLOR                   "DefaultLCSColor"
#define DEFAULT_OS_COLOR                    "DefaultOSColor"
#define LAMP_TEST_COLOR                     "LampTestColor"
#define LAMP_TEST_DURATION                  "LampTestDuration"
#define REF_HEALTH_EVENT                    "RefHealth"
#define REF_HEALTH_OBJ                      "RefHealthObj"
#define REF_HEALTH_PROP                     "RefHealthProp"
#define PRESS_EVENT                         "PressEvent"
#define MIX_PRESS_EVENT                     "MixPressEvent"
#define LED_TEST_REG_VAL                    "RegVal"
#define CHECK_INSERT_ENABLE_FLAG            "CheckInsertEn"
#define INSERT_STATUS0                      "InsertStatus0"
#define INSERT_STATUS1                      "InsertStatus1"
#define REF_LED                             "RefLed"
#define LED_ON_VALUE                        "LedOnValue"
#define NEED_START_UP_TEST                  "NeedStartUpTest"
#define METHOD_SET_STATE                    "SetState"
#define METHOD_PICMG_SET_LED                "PicmgSetLed"
#define METHOD_PICMG_GET_LED                "PicmgGetLed"
#define METHOD_GET_STATE                    "GetState"
#define METHOD_UID_STATE_SYNC               "UIDSyncMethod"
#define METHOD_SET_COMPONENT_LED            "SetComponentLed"
#define METHOD_SYNC_CHASSIS_LED_STATUS      "SyncChassisLedStatus"


/* button press test类定义 */
#define BUTTON_TEST_CLASS_NAME              "ButtonTest"
#define BUTTON_PRESS_EVENT                  "PressEvent"
#define CLEAR_PRESS_EVENT                   "ClearPressEvent"
#define EXPECT_VALUE                        "ExpectVal"
#define PRESS_TEST_READY                    "TestReady"
#define PRESS_TEST_READY_VALUE              "TestReadyVal"

/***************************************
 DFT ITEM类名定义
 ****************************************/
#define DFT_ITEM_CLASS "DftItem"

/***************************************
 DFT ITEM属性名定义
 ****************************************/
#define DFT_ID              "DftId"
#define SLOT_ID             "Slot"//槽位号索引
#define DEV_ID              "DevNum"// 设备编号
#define TEST_TYPE           "TestabilityType"//可测试性类型
#define ITEM_NAME           "ItemName"//测试项名称
#define PROMPT_READY        "PromptReady" //测试启动前准备提示
#define PROMPT_COMPLETE     "PromptComplete"//测试启动后交互提示
#define REF_OBJECT          "RefObject" //关联对象
#define PROCESS_TIME        "ProcessTime"//本测试项操作完成时间（如dftid为71的BMC串口环回就表示串口切换加打流时间，dftid为87的端口环回测试表示的是环回测试配置模式需要的时间，XCU需要打流的时间不包括在里面）
#define INPUT_DATA			"InputData"
#define DFT_ACTION			"DftAction"

/* 串口测试类 */
#define CLASS_NAME_SERIALPORT_TEST                 "SerialPortTest"

#define PROPERTY_SERIALPORT_TEST_SRC_UART_ID       "SrcUARTId"
#define PROPERTY_SERIALPORT_TEST_SRC_PORT_ID       "SrcPortId"
#define PROPERTY_SERIALPORT_TEST_DEST_UART_ID      "DestUARTId"
#define PROPERTY_SERIALPORT_TEST_DEST_PORT_ID      "DestPortId"

/*kvm 类定义*/
#define KVM_CLASS_NAME                             "Kvm"
#define KVM_OBJ_NAME                               "Kvm0"

#define KVM_PROPERTY_PORT                          "Port"
#define KVM_PROPERTY_d_PORT                        "d:Port"
#define KVM_PROPERTY_NUM                           "ConnectNum"
#define KVM_PROPERTY_STATE                         "State"
#define KVM_PROPERTY_COMPRESS_STATE                "EncryptState"
#define KVM_PROPERTY_KEYBOARDMODE                  "KeyboardMode"
#define KVM_PROPERTY_TIMEOUT                       "KvmTimeout"
#define SCREEN_PROPERTY_SWITCH                     "ScreenSwitch"
#define KVM_PROPERTY_LOCAL_KVM_STATE               "LocalKVMState"
#define KVM_PROPERTY_VISIBLE_JAVAKVM               "VisibleJavaKvm"
#define KVM_PROPERTY_AUTO_OS_LOCK_STATE            "AutoOSLockState"
#define KVM_PROPERTY_AUTO_OS_LOCK_TYPE             "AutoOSLockType"
#define KVM_PROPERTY_AUTO_OS_LOCK_KEY              "AutoOSLockKey"
#define KVM_METHOD_SET_PORT                        "SetPort"
#define KVM_METHOD_SET_ENABLE                      "SetState"
#define KVM_METHOD_SAVE_SCREEN                     "SaveScreen"
#define KVM_METHOD_GET_SCREENSHOT_CREATE_TIME      "GetScreenshotCreateTime"
#define KVM_METHOD_DELECT_LINK                     "DelectLink"
#define KVM_METHOD_SET_KEY                         "SetCodeKey"
#define KVM_METHOD_RM_SCREEN                       "RmScreen"
#define KVM_METHOD_SCREEN_SWITCH                   "SetScreenSwitch"
#define KVM_METHOD_SET_ENCRYPT_STATE               "SetEncryptState"
#define KVM_METHOD_GET_LINK_KEY                    "GetLinkKey"
#define KVM_METHOD_SETKEYBOARDMODE                 "SetKeyboardMode"
#define KVM_METHOD_SET_TIMEOUT                     "SetKvmTimeout"
#define KVM_METHOD_GET_IDENTIFIER                  "GetIdentifier"
#define KVM_METHOD_CHECK_DOWNLOAD_CODE             "CheckKvmDownloadCode"
#define KVM_METHOD_SSO_GET_IDENTIFIER              "SS0GetIdentifier"
#define KVM_METHOD_SSO_DESTROY_KVM                 "SsoKVMDestroy"
#define KVM_METHOD_SET_LOCAL_KVM_STATE             "SetLocalKVMState"
#define KVM_METHOD_SET_AUTO_OS_LOCK_STATE          "SetAutoOSLockState"
#define KVM_METHOD_SET_AUTO_OS_LOCK_TYPE           "SetAutoOSLockType"
#define KVM_METHOD_SET_AUTO_OS_LOCK_KEY            "SetAutoOSLockKey"

#define KVM_USB_CLASS_NAME                         "KvmUsb"
#define KVM_USB_PROPERTY_CDROM_ID                  "CdromId"
#define KVM_USB_PROPERTY_FLOPPY_ID                 "FloppyId"
#define KVM_USB_PROPERTY_KEYBOARD_MOUSE_ID         "KeyboardMouseId"
#define KVM_USB_PROPERTY_SP_UMS_ID                 "SpUmsId"
#define KVM_USB_PROPERTY_IBMA_UMS_ID               "iBmaUmsId"

/*VNC 类定义*/
/* BEGIN: Added by t00381753 tangjie, PN:VNC, 2017/02/24 */
#define VNC_CLASS_NAME                                "VNC"
#define OBJECT_VNC                               VNC_CLASS_NAME
#define PROPERTY_VNC_SUPPORT_STATE               "SupportState"
#define PROPERTY_VNC_ENABLE_STATE                "EnableState"
#define PROPERTY_VNC_PWD                         "Password"
#define PROPERTY_VNC_TIMEOUT                     "Timeout"
#define PROPERTY_VNC_SSL_STATE                   "SSLEnableState"
#define PROPERTY_VNC_PORT                        "Port"
#define PROPERTY_VNC_CONNECT_NUM                 "ConnectNum"
#define PROPERTY_VNC_KEYBOARD_LAYOUT             "KeyboardLayout"
#define PROPERTY_VNC_COMPONENT                   "PwdComponent"
#define PROPERTY_VNC_WORKKEY                     "PwdWorkKey"
#define PROPERTY_VNC_MASTERKEY_ID                "PwdMasterKeyId"
#define PROPERTY_VNC_PWD_COMPLEX_STATE           "PasswordComplexCheck"
#define PROPERTY_VNC_USERNAME                    "username"
#define PROPERTY_VNC_PWD_VALID_STARTTIME         "PwdValidStarttime"
#define PROPERTY_VNC_PWD_PERMIT                  "LoginPermit"
#define PROPERTY_VNC_PWD_LOCK_FLAG               "PwdLockFlag"
#define PROPERTY_VNC_PWD_VALID_FALG              "PwdValidFlag"
#define PROPERTY_VNC_PWD_REMAIN_DAYS             "PwdRemainDay"
#define PROPERTY_VNC_AUTHLOCK_START_TIME         "AuthFailLockStartTime"
#define PROPERTY_VNC_AUTHFAIL_TIMES              "AuthFailTimes"
#define PROPERTY_VNC_PERMIT_RULEIDS              "PermitRuleIds"


#define METHOD_VNC_SET_ENABLE_STATE              "SetState"
#define METHOD_VNC_SET_PWD                       "SetPwdValue"
#define METHOD_VNC_SET_TIMEOUT                   "SetTimeout"
#define METHOD_VNC_SET_SSL_STATE                 "SetSSLEnableState"
#define METHOD_VNC_SET_PORT                      "SetPort"
#define METHOD_VNC_SET_KEYBOARD_LAYOUT           "SetKeyboardLayout"
#define METHOD_VNC_DELECT_LINK                   "VncDelectLink"
#define METHOD_VNC_RENEW_PWD_START_TIME          "RenewPwdStartTime"
#define METHOD_VNC_CHECK_PWD_STATE               "CheckPwdState"
#define METHOD_VNC_RENEW_AUTHLOCK_START_TIME     "RenewAuthLockStartTime"
#define METHOD_VNC_SET_PERMIT_RULEIDS            "SetPermitRuleIds"
#define METHOD_VNC_UPDATE_LOCK_STATE             "UpdateLockState"
#define METHOD_VNC_UPDATE_CIPHERTEXT             "UpdateVncCiphertext"

/* END: Added by t00381753 tangjie, PN:VNC, 2017/02/24 */


/* SysFWProgressStatus类定义 */
#define SYSFW_CLASS_NAME                             "SysFWProgressStatus"
#define SYSFW_PROPERTY_KBCERR                        "KBCerr"
#define SYSFW_PROPERTY_VIDEOERR                      "Videoerr"
#define SYSFW_PROPERTY_NODIMMERR                     "NoDimmerr"


/*VMM 类定义*/
#define VMM_CLASS_NAME                             "Vmm"
#define VMM_OBJ_NAME                               "Vmm0"
#define VMM_PROPERTY_PORT                          "Port"
#define VMM_PROPERTY_D_PORT                        "d:Port"
#define VMM_PROPERTY_NUM                           "ConnectNum"
#define VMM_PROPERTY_STATE                         "State"
#define VMM_PROPERTY_COMPRESS_STATE                "EncryptState"
#define VMM_PROPERTY_VMM_CONNECT_STATUS            "VmmConnectStatus"
#define VMM_PROPERTY_VMM_CONNECT_URL               "VmmConnectUrl"
#define VMM_PROPERTY_VMM_FLOPPY_DRIVE              "FloppyDriveEnabled"

#define VMM_METHOD_DRVIVER_OPERATION               "DriverOperation"
#define VMM_METHOD_FORMAT_DEVICE                   "FormatDevice"
#define VMM_METHOD_GET_VMM_NAME                    "GetVmmName"
#define VMM_METHOD_SET_PORT                        "SetPort"
#define VMM_METHOD_SET_ENABLE                      "SetState"
#define VMM_METHOD_SET_CRYPT_ENABLE                "SetEncryptState"
#define VMM_METHOD_DELECT_LINK                     "DelectLink"
#define VMM_METHOD_SET_CONNECT                     "SetVmmConnect"
#define VMM_METHOD_SET_FLOPPYDRIVE_STATE           "SetFloppyDriveState"


/* BEGIN: Added by h00298892, 2017/7/22, AR  UMS 类定义*/
#define SP_UMS_CLASS_NAME                           "USBMassStorage"
#define SP_UMS_OBJ_NAME                             "USBMassStorage0"

#define UMS_PROPERTY_TYPE                           "UmsType"

#define UMS_PROPERTY_ENBLE                          "UmsEnable"
#define UMS_PROPERTY_BIOS_SUPPORT                   "UmsBIOSsupport"
#define UMS_PROPERTY_LOC                            "UmsLoc"
#define UMS_PROPERTY_SERV_FLG                       "UmsServFlg"
#define UMS_PROPERTY_CAPACITY                       "UmsCapacity"
#define UMS_PROPERTY_FILE_SYSTEM                    "UmsFileSystem"
#define UMS_PROPERTY_TRANSFER_PROGRESS              "UmsTransProgress"
#define UMS_PROPERTY_TRANSFER_FILE_NAME             "UmsTransFileName"
#define UMS_PROPERTY_MAX_UPDATE_SPACE               "UmsMaxUpdateSpace"
#define UMS_PROPERTY_FREE_UPDATE_SPACE              "UmsFreeUpdateSpace"
/* BEGIN: Added by h90006956, 2019/8/5, 支持板载iBMA */
#define UMS_PROPERTY_FLASHDEV                       "FlashDev"
#define UMS_PROPERTY_MOUNTPOINT                     "MountPoint"
#define UMS_PROPERTY_CONNECT_STATE                  "ConnectState"
/* END: Added by h90006956, 2019/8/5, 支持板载iBMA */
#define UMS_PROPERTY_SP_CFG_READY                   "SpConfigFileReady"
/* BEGIN: Modified by hucheng00298892, 2017/10/28   PN:DTS2017102500973  */
#define UMS_PROPERTY_SP_START_FLG                   "SPStartmode"
/* END:   Modified by hucheng00298892, 2017/10/28   PN:DTS2017102500973  */
#define UMS_PROPERTY_SYS_RESTART_DELAY              "SysRestartDelay"

#define UMS_PROPERTY_SP_VERSION_APP                 "SPAPPversion"
#define UMS_PROPERTY_SP_VERSION_DRV                 "SPDriverVersion"
#define UMS_PROPERTY_SP_REVISON_OS                  "SPOSrevision"
#define UMS_PROPERTY_SP_VERSION_OS                  "SPOSversion"
#define UMS_PROPERTY_SP_MAJORVERSION                "MajorVersion"
#define UMS_PROPERTY_SP_VERSION_COMPONENT           "ComponentsMessages"
#define UMS_PROPERTY_SP_DATA_EXIST                  "SPDataExist"

#define UMS_PROPERTY_SP_BOOTED_FLAG                 "SPBootedFlag"
#define UMS_PROPERTY_SP_UPGRADE_PROGRESS            "SPUpgradeProgress"
#define UMS_PROPERTY_SP_UPGRADE_FLAG                "SPInUpgradeFlag"

#define UMS_PROPERTY_SP_DEVICE_INFO_COLLECT_ENABLE  "SPDeviceInfoCollectEnable"
#define UMS_PROPERTY_SP_DEVICE_INFO_COLLECT_STATE   "SPDeviceInfoCollectState"
#define UMS_PROPERTY_SP_DEVICE_INFO_PARSE_STATE     "SPDeviceInfoParseState"

#define UMS_METHOD_SET_SP_DEVICE_INFO_COLLECT_ENABLE    "SetSPDeviceInfoCollectEnable"
#define UMS_METHOD_SET_SP_DEVICE_INFO_COLLECT_STATE     "SetSPDeviceInfoCollectState"
#define UMS_METHOD_CREAT_UMS                       "CreatUms"
#define UMS_METHOD_CLOSE_UMS                       "CloseUms"
#define UMS_METHOD_CHECK_FILE_STATE                "CheckFileState"
#define UMS_METHOD_DEL_FILES                       "DelUmsFiles"
#define UMS_METHOD_READ_FILE                       "ReadUmsFile"
#define UMS_METHOD_WRITE_FILE                      "WriteUmsFile"
#define UMS_METHOD_GET_FILE_LIST                   "GetUmsFileList"
#define UMS_METHOD_DEL_FILE                        "DelUmsFile"

#define UMS_METHOD_CLEAR_FIRMWARE                   "ClearFirmware"
#define UMS_METHOD_GET_FILE_LENGTH                  "GetFileLength"
#define UMS_METHOD_CHECK_FIRMWARE_PATH              "CheckFirmwarePath"
#define UMS_METHOD_UPDATE_FIRMWARE_SP               "UpdateFwSP"
#define UMS_METHOD_RENAME_LOCAL_FIRMWARE_SP         "RenameLocalFwSP"
#define UMS_METHOD_UPDATE_LOCAL_FIRMWARE_SP         "UpdateLocalFwSP"
#define UMS_METHOD_DELETE_LOCAL_FIRMWARE_SP         "DeleteLocalFwSP"
#define UMS_METHOD_UPDATE_GET_LOCAL_FIRMWARE_SP     "GetLocalFwSP"

#define UMS_METHOD_SET_MAX_UPDATE_SPACE             "SetUmsMaxUpdateSpace"
#define UMS_METHOD_SET_SP_CFG_READY                 "SetSpCfgReady"
#define UMS_METHOD_SET_SP_START_FLG                 "SetSpStartFlag"
#define UMS_METHOD_SET_SYS_RESTART_DELAY            "SetSysRestartDelay"
#define UMS_METHOD_SET_UPGRADE_PROGRESS             "SetSPUpgradeProgress"
#define UMS_METHOD_SET_BIOS_SUPPORT                 "SetBiosSupportSP"
/* END: Added by h00298892, 2017/7/22, AR  UMS 类定义*/

/* BEGIN: Added by h90006956, 2019/8/5, iBMAUMS 类定义*/
#define CLASS_IBMA_UMS_NAME                              "iBMAUSBMassStorage"
#define OBJ_IBMA_UMS_NAME                                "iBMAUSBMassStorage"

#define PROPERTY_IBMA_UMS_SERV_FLG                       "UmsServFlg"
#define PROPERTY_IBMA_UMS_CAPACITY                       "UmsCapacity"
#define PROPERTY_IBMA_UMS_FILE_SYSTEM                    "UmsFileSystem"
#define PROPERTY_IBMA_UMS_FLASHDEV                       "FlashDev"
#define PROPERTY_IBMA_UMS_OS_ACCESS_MODE                 "OSAccessMode"
#define PROPERTY_IBMA_UMS_PACKET_NUM                     "PacketNum"
#define PROPERTY_IBMA_UMS_OS_TYPE                        "OsType"
#define PROPERTY_IBMA_UMS_VERSION                        "Version"
#define PROPERTY_IBMA_UMS_STATE                          "State"
#define PROPERTY_IBMA_UMS_MOUNTPOINT                     "MountPoint"
#define PROPERTY_IBMA_UMS_CONNECT_STATE                  "ConnectState"
#define PROPERTY_IBMA_UMS_UPGRADE_FLG                    "InUpgradeFlag"
#define PROPERTY_IBMA_UMS_TRANSFER_PROGRESS              "UmsTransProgress"
#define PROPERTY_IBMA_UMS_TRANSFER_FILE_NAME             "UmsTransFileName"
#define PROPERTY_IBMA_UMS_UPGRADE_PROGRESS               "UpgradeProgress"
#define UMS_METHOD_UPDATE_INSTALLABLE_IBMA                "UpdateInstallablebma"
/* END: Added by h90006956, 2019/8/5, iBMAUMS 类定义*/

/*VIDEO 类定义*/

#define VIDEO_CLASS_NAME                             "Video"
#define VIDEO_OBJ_NAME                               "Video0"
#define VIDEO_PROPERTY_PORT                          "Port"
#define VIDEO_PROPERTY_D_PORT                        "d:Port"
#define VIDEO_PROPERTY_NUM                           "ConnectNum"
#define VIDEO_PROPERTY_STATE                         "State"
#define VIDEO_PROPERTY_SWITCH                        "VideoSwitch"
#define VIDEO_METHOD_SET_PORT                        "SetPort"
#define VIDEO_METHOD_SET_ENABLE                      "SetState"
#define VIDEO_METHOD_SET_SWITCH                      "SetRecordVideoSwitch"
#define VIDEO_METHOD_GET_VIDEO_KEY                   "GetVideoKey"
#define VIDEO_METHOD_GET_VIDEO_INFO                  "GetVideoInfo"
#define VIDEO_METHOD_SET_VIDEO_NAME                  "SetVideoName"
#define VIDEO_METHOD_DELECT_LINK                     "DelectLink"
#define VIDEO_METHOD_GET_VIDEO_NAME                  "GetVideoName"
/*BEGIN: Added by h00164462, 2015/3/19, PN:深度恢复出厂配置*/
#define VIDEO_METHOD_RM_VIDEO                        "RmVideo"
/*END: Added by h00164462, 2015/3/19, PN:深度恢复出厂配置*/

/* BEGIN: Added by ouluchun 00294050, 2015/8/14   PN:AR-AR-0000956733*/
#define PROXY_KVM_CLASS_NAME                             "ProxyKvm"
#define PROXY_KVM_OBJ_NAME                             "ProxyKvm0"
#define PROXY_VMM_CLASS_NAME                             "ProxyVmm"
#define PROXY_VMM_OBJ_NAME                             "ProxyVmm0"
#define PROXY_VIDEO_CLASS_NAME                             "ProxyVideo"
#define PROXY_VIDEO_OBJ_NAME                             "ProxyVideo0"
/* END:   Added by ouluchun 00294050, 2015/8/14 */


/*KVMLINK 类定义*/
#define KVMLINK_CLASS_NAME                             "KvmLink"
#define KVMLINK_PROPERTY_MODE                          "link_mode"
#define KVMLINK_PROPERTY_TYPE                          "link_type"
#define KVMLINK_PROPERTY_ID                            "link_id"
#define KVMLINK_PROPERTY_IP                            "link_ip"
#define KVMLINK_PROPERTY_USER_NAME                     "link_user_name"
#define KVMLINK_PROPERTY_LOGIN_TIME                    "link_login_time"

/*VNCLINK 类定义*/
#define VNCLINK_CLASS_NAME                             "VncLink"
#define VNCLINK_PROPERTY_MODE                          "link_mode"
#define VNCLINK_PROPERTY_ID                            "link_id"
#define VNCLINK_PROPERTY_IP                            "link_ip"
#define VNCLINK_PROPERTY_LOGIN_TIME                    "link_login_time"
#define VNCLINK_PROPERTY_USERNAME                      "link_user_name"



/* ipmbeth 类定义*/
#define CLASS_NAME_IPMBETH                  "IpmbEth"
#define IPMBETH_OBJ_NAME                    CLASS_NAME_IPMBETH
#define IPMBETH_OBJ_NAME_SMM                "BMC_IpmbEth"
/* BEGIN: Modified by dingzhixiong (d00381887), AR.SR.SF-0000276585.001.013, 2017/10/20 */
#define IPMBETH_PROPERTY_CHAN_ID            "ChanID"
/* END: Modified by dingzhixiong (d00381887), AR.SR.SF-0000276585.001.013, 2017/10/20 */
#define IPMBETH_PROPERTY_SA                 "SlaveAddr"
#define IPMBETH_PROPERTY_SMM0STATE          "SMM0State"
#define IPMBETH_PROPERTY_SMM1STATE          "SMM1State"
#define IPMBETH_PROPERTY_HEARTBEAT          "HeartBeat"
/* BEGIN: Added by d00381887, 2017/12/18, AR.SR.SFEA02119723.002.002 节点BMC支持检测SMU板心跳丢失告警 */
#define IPMBETH_PROPERTY_BAK_HEARTBEAT      "BakHeartBeat"
/* END: Added by d00381887, 2017/12/18, AR.SR.SFEA02119723.002.002 */
#define IPMBETH_PROPERTY_BASE_HEARTBEAT     "BaseHeartBeat"
#define IPMBETH_PROPERTY_FABRIC_HEARTBEAT   "FabricHeartBeat"

/*IpmbAddr 类定义*/
#define CLASS_NAME_IPMBADDR                 "IpmbAddr"
#define IPMBADDR_PROPERTY_SLOTID            "SlotId"
#define IPMBADDR_PROPERTY_SA                "SlaveAddr"
#define IPMBADDR_PROPERTY_SA_BASE           "SlaveAddrBase"
#define IPMBADDR_PROPERTY_SA_MULTI          "SlaveAddrMulti"

/*存储器类*/
#define BLOCK_STORAGE_CLASS_NAME            "BlockStorage"
#define BLOCK_STORAGE_DEVICE                "Device"
#define BLOCK_STORAGE_OFFSET                "Offset"
#define BLOCK_STORAGE_SIZE                  "Size"

/*   diagnose   大模块，包括FDM类的处理和diagnose 类的处理两部分*/

/* FDM类的对象及属性 */
#define CLASS_FDM_NAME   "FdmConfig"
#define OBJ_FDM_NAME     "FdmConfig"
#define PROPERTY_FDM_ENABLE    "FdmEnable"
#define PROPERTY_PFA_ENABLE    "PFAEnable"
#define PROPERTY_FDM_EMCA          "EMCA"
#define PROPERTY_FDM_IOMCA         "IOMCA"
#define PROPERTY_FDM_Viral         "Viral"
#define PROPERTY_FDM_CDC           "CDC"
/* IERR收集失败时热复位使能属性已扩展为处理策略属性,考虑兼容性不做名称修改 策略为0不做处理，1热复位，2诊断后下电 */
#define PROPERTY_FDM_IERR_DIAG_FAIL_POLICY      "IErrWarmResetEnable"
/* 内存DQ线接触不良(poor contact)告警功能开关, 默认为1 打开 */
#define PROPERTY_FDM_MEM_POOR_CONTACT_ALARM_ENABLED   "MemPoorContactAlarmEnabled"
#define PROPERTY_FDM_DAYNAMIC_REMAPPING_ENABLE  "MemoryDynamicRemappingEnabled"
#define PROPERTY_FDM_IERR_DIAG_SUCCESS_POLICY   "IErrDiagSuccessPolicy"
#define PROPERTY_FDM_SMI_RECOVER   "SMIRevocer"
#define PROPERTY_FDM_PROCESSBAR    "FdmProcessbar"
#define PROPERTY_FDM_MAJOR_VER     "FdmMajorVersion"
#define PROPERTY_FDM_MINOR_VER     "FdmMinorVersion"
/* BEGIN: Added by ouluchun 00294050, 2015/8/3   PN:AR-0000958109*/
#define PROPERTY_FDM_GATHER_FAIL   "FDMGatherFail"
/*DTS2015111104563，诊断成功，更新属性"FDMUnknownFault"与故障部件属性，诊断失败，静默处理*/
#define PROPERTY_FDM_UNKNOWN_FAULT "FDMUnknownFault"
/* END:   Added by ouluchun 00294050, 2015/8/3 */
#define PROPERTY_FDM_MSP_ENABLE    "MSPEnable"
#define PROPERTY_FDM_SITEAI_ENABLE    "MemForecastAlgorithm"
#define METHOD_FDM_FRU_HOG_PLUG       "FruHotPlugCallback"
#define METHOD_FDM_SET_FDM_FAULT  "FdmConfigSetFdmFault"
#define METHOD_FDM_WRITE_BIN_LOG     "FDMWriteBinLog"
#define METHOD_FDM_SET_IERR_DIAG_FAIL_POLICY   "SetIErrWarmResetEnable"
#define METHOD_FDM_SET_IERR_DIAG_SUCCESS_POLICY   "SetIErrDiagSuccessPolicy"
#define METHOD_FDM_SET_MEM_POOR_CONTACT_ALARM_ENABLED   "SetMemPoorContactAlarmEnabled"
#define METHOD_FDM_SET_DYNAMIC_REMAPPING_ENABLED   "SetMemDynamicRemappingEnabled"
#define METHOD_FDM_RESET_IFMM   "FDMResetIFMM"

/* ARM FDM类的对象及属性 */
#define CLASS_ARM_FDM_NAME          "ARMFdmConfig"
#define OBJ_ARM_FDM_NAME            "ARMFdmConfig"
#define PROPERTY_ARM_FDM_ENABLE     "FdmEnable"
#define PROPERTY_ARM_FDM_SERVERTYPE "ServerType"

/*   fdm_web类(主要是fdmweb页面和健康报告数据等内容的处理)   */
#define CLASS_FDM_WEB_NAME "FdmWebConfig"
#define OBJ_FDM_WEB_NAME     "FdmWebConfig"
#define PROPERTY_PFAE_ENABLE "FdmPfaeEnable"
#define PROPERTY_FDMWEB_AUTO_UP_ENABLE  "FdmAutoUpEnable"
#define PROPERTY_FDMWEB_AUTO_UP_CYCLE  "FdmAutoUpCycle"
#define PROPERTY_FDMWEB_TIMEQUANT  "FdmTimeQuant"
#define PROPERTY_FDMWEB_SYNC_NAME "FdmWebSyncName"
#define PROPERTY_FDMWEB_PFAE_NOTIFY_SYSTEM "FdmPfaeNotifySystemEnable"
#define PROPERTY_PFAE_RESULT_DISPLAY "FdmPfaeResultDisplay"

//fdmweb 接口
#define METHOD_FDM_FRUTREE "FDMFruTree"
#define METHOD_SET_FDM_FRUTREE "SetFDMFruTree"
#define METHOD_SET_UPDATE_ENABLE                        "SetAotuUpdateEnable"
#define METHOD_SET_REFRESH_INTERVAL                        "SetRefreshInterval"
#define METHOD_SET_PFAE_TIMES                        "SetPfaeTimes"
#define METHOD_SET_PFAE_NOTIFY_SYSTEM_ENABLE  "SetPfaeNotifySystemEnable"
#define METHOD_POST_SEL_TO_FDM  "PostSelToFdm"
#define METHOD_POST_SYS_EVENT_TO_FDM  "PostSysEventToFdm"

//树节点
#define CLASS_TREE_NODE_NAME            "TreeNode"
#define PROPERTY_REF_COMPONENT          "RefComponent"
#define PROPERTY_SN                     "SN"
#define PROPERTY_PRIOR_SN               "PriorSN"
#define PROPERTY_PRESENCE               "Presence"
#define PROPERTY_ONLINETIME             "OnlineTime"
#define PROPERTY_REPLACE_FLGA           "ReplaceFlag"
#define PROPERTY_NODE_TYPE              "NodeType"
#define PROPERTY_PARENT_OBJ             "ParentObj"
#define PROPERTY_HIDE_RELATION_OBJ      "HideRelationObj"
#define PROPERTY_HIDE_RELATION_OBJ_STR  "HideRelationObjStr"
#define PROPERTY_SUB_TREE               "SubTree"
#define PROPERTY_INNER_TREE             "InnerTree"
#define PROPERTY_UPDATE_MODE            "UpdateMode"
#define PROPERTY_REF_FRU_OBJ            "RefFruObj"

//描述节点父子关系的管道链接类
#define CLASS_TREE_NODE_LINK_PIPE       "PIPE"
#define PROPERTY_PIPE_ID                "ID"
#define PROPERTY_PIPE_SECID             "SecID"
#define PROPERTY_PIPE_TYPE              "TYPE"
#define PROPERTY_PIPE_UPLINK            "UpLink"
#define PROPERTY_PIPE_SUBLINK           "SubLink"

/*   diagnose类(主要是黑匣子和串口数据等内容的处理)   */
#define CLASS_DIAGNOSE_NAME                   "Diagnose"
#define OBJ_DIAGNOSE_NAME                     "Diagnose"
#define PROPERTY_PCIEINTERFACE_SWITCH          "PCIeInterfaceSwitch"
#define PROPERTY_BLACKBOX_STATE               "BlackBoxState"
#define PROPERTY_SOL_DATA_STATE               "SolDataState"
/* BEGIN: Added by c00149416, DTS2017021500425 2017/2/16 */
#define PROPERTY_SOL_DATA_DIRECTION           "SolDataDirection"
/* END:   Added by c00149416, 2017/2/16 */
#define PROPERTY_BLACKBOX_PROCESSBAR          "BlackBoxProcessbar"
#define PROPERTY_SYSTEMCOM_PROCESSBAR         "SystemComProcessbar"
#define PROPERTY_NPU_LOG_PROCESSBAR           "NpuLogProcessBar"
#define PROPERTY_PORT_LED_STATE               "Port80LedState"
#define PROPERTY_SOL_AUTOCOLLECT_ENABLE       "SolAutoCollectEnable"
#define PROPERTY_ALL_SOL_VOLUME_SIZE          "AllSolVolumeSize"
#define PROPERTY_SOLUARTOBJ                   "SolUartObj"

#define METHOD_DIAGNOS_GETSYSTEMCOM           "GetSystemCom"
#define METHOD_DIAGNOS_GETPORT80              "GetPort80"
#define METHOD_DIAGNOS_GETBLACKBOX            "GetBlackBox"
#define METHOD_DIAGNOS_GETSOL                 "GetSol"
#define METHOD_DIAGNOS_SET_SOL_STATE          "SetSolDataState"
#define METHOD_DIAGNOS_SET_PCIEINTERFACE_SWITCH "SetPCIeInterfaceSwitch"
#define METHOD_DIAGNOS_SET_BLACKBOX_DUMP_STATE  "SetBlackBoxDumpState"

#define MDIO_CHIP_CLASS "MDIOChipManager"
#define MDIO_CHIP_ATTRIBUTE_CHIPTYPE                  "ChipType"
#define MDIO_CHIP_ATTRIBUTE_CHANID                    "ChanID"
#define MDIO_CHIP_ATTRIBUTE_ADDR                      "Addr"
#define MDIO_CHIP_ATTRIBUTE_LINKSTATUS                "LinkStatus"
#define MDIO_CHIP_ATTRIBUTE_LINKSPEED                 "LinkSpeed"
#define MDIO_CHIP_ATTRIBUTE_CHIP_ID                   "ChipId"

#define MDIO_CFG_CHIP_CFG_CLASS "MDIOChipConfig"
#define MDIO_CFG_CHIP_ATTRIBUTE_ADDR                      "Addr"
#define MDIO_CFG_CHIP_ATTRIBUTE_CHIP_ID                   "ChipId"
#define MDIO_CFG_CHIP_CFG_ATTRIBUTE_REG_ADDR                  "RegAddr"
#define MDIO_CFG_CHIP_CFG_ATTRIBUTE_REG_VALUE                   "RegValue"
#define MDIO_CFG_CHIP_CFG_ATTRIBUTE_DELAY                      "Delay"

/*osca 后插板类*/
/*类*/
#define FCCARD_CLASS_CARD                                      "XCUCard"
#define FCCARD_CLASS_FCSWCONFIG                          "FCSWConfig"
#define FCCARD_CLASS_INITMAX24101                        "MAX24101"
#define FCCARD_CLASS_MAX24101PRA                        "MAX24101Para"

#define FCCARD_CLASS_MDIOSIMULATOR                    "MDIOSimulator"

/*对象*/
#define FCCARD_OBJ_M8PBSIMULATOR                       "M8PBSimulator"
#define FCCARD_OBJ_MAX24101                                  "max24101config"
#define FCCARD_OBJ_MAX24101PRA1                          "max24101config_1"
#define FCCARD_OBJ_MAX24101PRA2                          "max24101config_2"
#define FCCARD_OBJ_MAX24101PRA3                          "max24101config_3"
#define FCCARD_OBJ_MAX24101PRA4                          "max24101config_4"
#define FCCARD_OBJ_M8PBCARD                                 "M8PBCard"
#define FCCARD_OBJ_FCSWCARD                                 "FCSWCard"
#define FCCARD_OBJ_FCSWCONF                                 "FCSWConf"

/*属性*/
#define FCCARD_ATTRIBUTE_PRESENT                   "FCPresent"
#define FCCARD_ATTRIBUTE_TYPE                         "FCType"
#define FCCARD_ATTRIBUTE_PCBID                        "FCpcbid"
#define FCCARD_ATTRIBUTE_RESET                        "FCReset"
#define FCCARD_ATTRIBUTE_INIT24101                 "InitMAX24101"
#define FCCARD_ATTRIBUTE_INITSERDES               "InitFCSERDES"
#define FCCARD_ATTRIBUTE_LOADM8PB                 "loadM8PBFirmware"


#define FCCARD_ATTRIBUTE_IIC_CMD                    "FCI2cCmd"
#define FCCARD_ATTRIBUTE_IIC_RSP                     "FCI2cRsp"
#define FCCARD_ATTRIBUTE_IIC_BMCHOLD            "FCI2cBMCHold"
#define FCCARD_ATTRIBUTE_IIC_MEZZHOLD           "FCI2cMezzHold"
#define FCCARD_ATTRIBUTE_IIC_MEZZSTA              "FCI2cMezzSta"
#define FCCARD_ATTRIBUTE_IIC_CLRRSP                "FCI2cClrRsp"
#define FCCARD_ATTRIBUTE_IIC_CLRMEZZSTA        "FCI2cClrMezzSta"
#define FCCARD_ATTRIBUTE_IIC_MEZZHEALTH        "FCI2cMezzHealth"
#define FCCARD_ATTRIBUTE_IIC_CLRMEZZHOLD     "FCI2cClrMezzHold"
#define FCCARD_ATTRIBUTE_CONF_PAR_AREA        "FCConfParaArea"
#define FCCARD_ATTRIBUTE_HEART_STATUS          "FCHeartStatus"


#define FCCARD_ATTRIBUTE_MDIOMODE                 "MDIOMode"
#define FCCARD_ATTRIBUTE_MDIOPHYREG             "MDIOPhyReg"
#define FCCARD_ATTRIBUTE_MDIOPHYADDR           "MDIOPhyAddr"
#define FCCARD_ATTRIBUTE_MDIOWRITEREGL       "MDIOWriteRegl"
#define FCCARD_ATTRIBUTE_MDIOWRITEREGH      "MDIOWriteRegH"
#define FCCARD_ATTRIBUTE_MDIOCMDREG            "MDIOCMDReg"
#define FCCARD_ATTRIBUTE_MDIOREADREGL         "MDIOREADREGL"
#define FCCARD_ATTRIBUTE_MDIOREADREGH        "MDIOREADREGH"
#define FCCARD_ATTRIBUTE_MDIOCOMFLG            "MDIOCOMPLETEDFLAG"
#define FCCARD_ATTRIBUTE_MDIOERRREG            "MDIOERRREG"

#define FCCARD_ATTRIBUTE_CONFADR                          "confAddr"
#define FCCARD_ATTRIBUTE_I2CINITADDR                    "I2CInitAddr"
#define FCCARD_ATTRIBUTE_PARMDEV1LOW                  "Parmdev1low"
#define FCCARD_ATTRIBUTE_PARMDEV1HIGH                  "Parmdev1high"
#define FCCARD_ATTRIBUTE_PARMDEV2LOW                  "Parmdev2low"
#define FCCARD_ATTRIBUTE_PARMDEV2HIGH                  "Parmdev2high"
#define FCCARD_ATTRIBUTE_CONTROL1TO4                   "channel1to4control"
#define FCCARD_ATTRIBUTE_CONTROL5TO8                   "channel5to8control"
#define FCCARD_ATTRIBUTE_CONTROL9TO12                 "channel9to12control"
#define FCCARD_ATTRIBUTE_CONTROL13TO16               "channel13to16control"

#define FCCARD_ATTRIBUTE_SLOTID                             "SlotId"
#define FCCARD_ATTRIBUTE_MOTHERBOARDID              "MotherBoardId"
#define FCCARD_ATTRIBUTE_CHANNEL1TO4                   "channel1to4"
#define FCCARD_ATTRIBUTE_CHANNEL5TO8                   "channel5to8"
#define FCCARD_ATTRIBUTE_CHANNEL9TO12                 "channel9to12"
#define FCCARD_ATTRIBUTE_CHANNEL13TO16               "channel13to16"



#define METHOD_SWI_IPMICMD_PROCESS                      "SwiCmdProcess"

/* 分区类*/
#define CLASS_COMPUTER_PARTITION "ComputerPartition"
#define OBJECT_PHYSICAL_PARTITION "PhysicalPartition"
#define PROPERTY_COMPUTER_PARTITION_TYPE "Type"
#define PROPERTY_COMPUTER_PARTITION_MODE "Mode"
#define PROPERTY_COMPUTER_PARTITION_ID "Id"
#define PROPERTY_COMPUTER_PARTITION_RHPRODUCTID "RHProductId"


#define METHOD_HPC_MANAGEMENT_GET_PATITION_MODE "GetOppInfo"  /*获取硬分区形态*/

/* HPCMgmt 类*/
#define CLASS_HPC_MANAGEMENT "HPCMgmt"
#define OBJECT_HPC_MANAGEMENT CLASS_HPC_MANAGEMENT
#define PROPERTY_HPC_MANAGEMENT_ACTION_ROA "HPCActionRoa" /* HPC 服务器硬件访问属性组*/
#define METHOD_HPC_MANAGEMENT_GETCONFIGINFO "GetHPCMgmtConfig"
#define METHOD_HPC_MANAGEMENT_SETCONFIGINFO "SetHPCMgmtConfig"
#define METHOD_HPC_MANAGEMENT_GET_PSW_UPSTATUS "GetPCIeSWUpStatus"  /*获取PCIe Switch 是否处于升级状态 */
#define METHOD_HPC_MANAGEMENT_GET_LPARDFT_STATUS "GetLparDFTStatus"  /* 获取现在DFT 状态   */
#define METHOD_HPC_MANAGENTMENT_SET_OPP_BMC_CFG_NC  "SetOPPBMCConfigNC"
#define METHOD_SET_REMOTE_RAID_NUM  "SetRaidNum"
#define METHOD_HPC_MANAGEMENT_SUBPAYLOAD "HPCSubPayload" /*硬分区特殊上下电请求，当前只应用在升级bios时ME挂死后上下电处理*/

#define CLASS_HPC_MANAGEMENT_ACTION_NAME                        "HPCAction"
#define OBJECT_HPC_CPUBRD_MIX                                   "HPCCPUBoardMix"
#define OBJECT_HPC_MEMBRD_MIX                                   "HPCMEMBoardMix"
#define PROPERTY_HPC_MANAGEMENT_ACTION                          "Destination"
#define PROPERTY_HPC_MANAGEMENT_ACTION_DATA                     "Data"

/* 辅助上电属性，配置NC、配置时钟*/
#define PROPERTY_SUBPAYLOAD_NC_IIC_STATUS "NCIICStatus" /* NC IIC 链路状态*/
#define PROPERTY_SUBPAYLOAD_PWTON_VALUE "PWTValue"  /* PBI power button 按钮属性 */
#define PROPERTY_SUBPAYLOAD_RST_PCS_PBI_NC "RSTPCSReset" /* RST_PCS_PBI_NC 信号复位 */
#define PROPERTY_SUBPAYLOAD_RST_SI_HARD_PBI_NC "RSTSIReset"  /* RST_SI_HARD_PBI_NC 信号复位*/
#define PROPERTY_SUBPAYLOAD_RST_QPIPHY0_PBI_NC "QPIPHY0Reset" /* RST_QPIPHY0_PBI_NC 信号复位*/
#define PROPERTY_SUBPAYLOAD_RST_QPIPHY1_PBI_NC "QPIPHY1Reset" /* RST_QPIPHY1_PBI_NC 信号复位*/
#define PROPERTY_SUBPAYLOAD_NC_PBI_CFG_STATUS "NCPBICfgStatus"   /* PBI完成高速配置*/
#define PROPERTY_SUBPAYLOAD_NC_OPP_PBI_CFG_STATUS "NCOppPBICfgStatus"   /* PBI 同机框对板 完成高速配置*/
#define PROPERTY_SUBPAYLOAD_START_CHECK_NI_ENABLE "StartCheckNI"   /* 轮训NI 建立标志*/
#define PROPERTY_SUBPAYLOAD_SMM_PWT_LOCK "SMMPwrLock"    /* 9032 SMM 下发上电使能信号允许上电*/
#define PROPERTY_SUBPAYLOAD_NC_CHIP_CFG "NCChipConfig"   /* NC 配置*/
#define PROPERTY_PBI_CLOCK_CFG "Zl30169ChipConfig"   /* pbi 30169 时钟配置*/

#define OBJECT_PCIE_CB_MIX_MON_HPCACTION                        "PCIECableMixMonHPCAction"
#define OBJECT_SIG_BOARD_ID_HPCACTION                        "SigBoardIdHPCAction"
#define OBJECT_RAID1_PRESENT_HPCACTION                        "Raid1PresHPCAction"
#define OBJECT_RAID2_PRESENT_HPCACTION                        "Raid2PresHPCAction"
#define OBJECT_EXPANDER_SERIAL_SWITCH                        "ExpanderSeriaSwitch"
#define OBJECT_BIOS_HOT_RESET_FINISH                                 "HOTResetFinish"


/* 数据同步类*/
#define CLASS_DATA_SYNC "DataSync"
#define OBJECT_DATA_SYNC CLASS_DATA_SYNC
#define PROPERTY_ClASS_SYNC "ClassSync"
#define PROPERTY_PROP_SYNC "PropSync"
#define METHOD_RESET_DATA_SYNC  "ResetDataSync"

/* 同步远程对象*/
#define CLASS_AGENT_CPU "AgentCpu"
#define CLASS_AGENT_MEMORY "AgentMemory"

/*节点硬分区类*/
#define CLASS_PARTITION "Partition"
#define PARTITION_OBJECT_NAME CLASS_PARTITION
#define METHOD_PARTITION_SWITCHVGA  "SwitchVGA"
#define METHOD_PARTITION_SWITCHPARTITION  "SwitchPartition"
#define METHOD_PARTITION_SWITCHRAID  "SetExpanderMode"
#define METHOD_PARTITION_GETEXPANDERMODE  "GetExpanderMode"
#define METHOD_PARTITION_GETCPLDINFO "GetCpldInfo"
#define METHOD_PARTITION_SETMODECHANGEDVALUE  "SetParModeChangedValue"
/*节点硬分区对象*/
#define PARTITION_OBJ_NAME  "Partition"
#define XMLPARTITION_OBJ_NAME  "xmlPartition"

/*节点硬分区属性*/
#define NODE_MODE "NodeMode"
#define EEP_PARTITION "EepPartition"
#define CPLD_PARTITION "CpldPartition"
#define XML_PARTITION "XmlPartition"
#define CPLD_PARTITION_CFG "CpldPartitionCfg"

#define NEGOTIATE_STATE "NegotiateState"
#define REMOTE_NEGOTIATE_STATE "RemoteNegotiateState"
#define REMOTE_HFC_PRESENT "RemoteBMCBoardPresent"

#define PARTITION_CPLD_RESET "CpldReset"
#define REMOTE_SYNC_IP  "RemoteSyncIp"
#define COM_SEL  "ComSel"
#define REMOTE_EXTERN_IP "RemoteExternIp"
#define BPN_CPLD_RESET "PbnCpldReset"
#define IO_CPLD_RESET "IoCpldReset"
#define BIO_CPLD_RESET "BIoCpldReset"
#define PBI_CPLD_RESET "PbiCpldReset"
#define RAID_MODE  "RAIDMode"
#define METHOD_SET_PARTITION "SetPartition"
#define METHOD_SWITCH_PARTITION "SwitchPartition"
#define METHOD_RESET_CPLD "ResetCpld"
#define METHOD_SET_MEM_BRD_PROP "SyncSetMemBrdProperty"
#define METHOD_STOP_LINK_MONITOR_TASK "StopLinkMonitorTask"
#define SYNC_REMOTE_IP "SyncRemoteIP"

#define METHOD_SWITCH_BIOS_PARTITION    "SwitchBiosFlash"
#define PROPERTY_SWITCH_BIOS_PARTITION  "SwitchBiosFlash"
#define PROPERTY_PARTITION_VMWAREENABLE "VmwareEnable"
#define PROPERTY_PARTITION_MODE_CHANGED "ModeChanged"


#define OBJECT_ERRSTATE2_HPCACTION "ErrState2HPCAction"

#define CLASS_NCBOARD_NAME                "NCBoard"
#define OBJECT_NCBOARD_NAME               "NCBoard"
#define PROPERTY_NCBOARD_LOGICVERID       "LogicVerId"
#define PROPERTY_NCBOARD_LOGICVER         "LogicVer"
#define PROPERTY_NCBOARD_PCBID            "PcbId"
#define PROPERTY_NCBOARD_PCBVER           "PcbVer"
#define PROPERTY_NCBOARD_PRESENCE         "Presence"
#define PROPERTY_NCBOARD_ERRSTATE1        "ErrState1"
#define PROPERTY_NCBOARD_ERRSTATE2        "ErrState2"
#define PROPERTY_NCBOARD_QPI_RETRY_ERR    "QpiRetryErr"
#define PROPERTY_NCBOARD_QPI_DEGRADE_ERR  "QpiDegradeErr"
#define PROPERTY_NCBOARD_LP_DIR_ECC_ERR   "LpDirEccErr"
#define PROPERTY_NCBOARD_RC_TAG_ECC_ERR   "RcTagEccErr"
/** BEGIN: Added by l00272597, 2016/10/8   PN:AR0000256853-001-002  */
#define PROPERTY_NCBOARD_LOGICUNIT        "LogicUnit"
#define PROPERTY_NCBOARD_CHIP_NAME        "ChipName"
#define PROPERTY_NCBOARD_DIEID            "DIEID"
#define PROPERTY_NCBOARD_CHIP_LOCATION    "ChipLocation"
#define PROPERTY_NCBOARD_CHIP_VERSION     "ChipVersion"
/** END:   Added by l00272597, 2016/10/8 */
#define PROPERTY_NCBOARD_PORTA_LINKDOWN   "PortALinkDown"
#define PROPERTY_NCBOARD_PORTB_LINKDOWN   "PortBLinkDown"
#define PROPERTY_NCBOARD_PORTC_LINKDOWN   "PortCLinkDown"
/** BEGIN: Added by r00355870, 2017/5/27   PN:AR0000276595-003-023  */
#define PROPERTY_NCBOARD_CDR_CONFIG_ERR   "CDRCfgErr"
/** END: Added by r00355870, 2017/5/27   PN:AR0000276595-003-023  */
/** BEGIN: Added by z00355831, 2017/12/1   PN:AR.SR.SFEA02109782.006.001  */
#define PROPERTY_NCBOARD_POWER_STATE      "PowerState"
/** END: Added by z00355831, 2017/12/1   PN:AR.SR.SFEA02109782.006.001  */



#define OBJECT_NCBOARD_PWRLED             "NCBoardPwrLed"


#define CLASS_CXP "CXPCable"
#define PROPERTY_CXP_SLOT             "Slot"
#define PROPERTY_CXP_NAME             "Name"
#define PROPERTY_CXP_PRESENCE         "Presence"
#define PROPERTY_CXP_MINOR_ERR        "LinkMinorErr"
#define PROPERTY_CXP_MAJOR_ERR        "LinkMajorErr"
#define PROPERTY_CXP_LED_COLOR        "LedColor"
#define PROPERTY_CXP_LED_STATE        "LedState"


/*网口切换类*/
#define CLASS_PORT_SWITCH "PortSwitch"

/*网口切换对象和属性*/
#define OBJECT_PORT_SWITCH "PortSwitch"

#define SWITCH_PORT_STATUS "PortLinkStatus"
#define PHY_LINK_STATUS  "PhyLinkStatus"
#define REMOTE_PORT_STATUS "RemotePortLinkStatus"
#define REMOTE_PHY_LINK_STATUS  "RemotePhyLinkStatus"
#define HEARTBEAT_STATUS  "HeartbeatStatus"

/* VGAq切换*/
#define CURRENT_VGA_SWITCH  "CurrentVGASwitch"
#define VGA_SWITCH  "VGASwitch"
#define VGA_SWITCH_CLEAR  "VGASwitchClear"


/* AB灯属性*/
#define A_LED_CTRL  "ALedCtrl"
#define B_LED_CTRL  "BLedCtrl"


/* LSW芯片配置 */
#define CLASS_LSW_CFG_NAME          "LSWConfig"
#define PROPERTY_LSW_CFG_CHIPID     "ChipId"
#define PROPERTY_LSW_CFG_PHYNUM     "PhyNum"
#define PROPERTY_LSW_CFG_PHYREG     "PhyReg"
#define PROPERTY_LSW_CFG_VALUE      "Value"
#define PROPERTY_LSW_CFG_MASK       "Mask"
/* BEGIN: Added by 00381887 dingzhixiong, 2018/03/18   PN:DTS2018031800698 装备P0网口环回测试无法通过，提单跟踪 */
#define CLASS_DFT_LSW_CFG_NAME      "DftLSWConfig"
/* END:   Added by 00381887 dingzhixiong, 2018/03/18   PN:DTS2018031800698 */

/* LSW芯片配置 */
#define CLASS_LSW_MGNT_NAME       "LSWChipMgnt"
#define PROPERTY_LSW_CHIPID_LIST  "Id"
#define PORPERTY_LSW_CHIP_STATUS  "Status"
#define PORPERTY_LSW_CHIP_RESET    "ResetIO"
#define PORPERTY_LSW_HW_ENABLE     "EnableIO"
#define METHOD_RESET_LSW_CONFIG    "ResetConfig"
#define PROPERTY_LSW_EGRESS_PORT_COUNT    "EgressPortCount"
/* BEGIN for DTS2014082906128 : Added by ouluchun 00294050 2015-9-29 */
#define PROPERTY_LSW_HEALTH     "Health"
#define PROPERTY_LSW_TYPE       "Function"
#define OBJECT_BMC_LSW_MGNT_NAME    "LSWManagement"
#define OBJECT_SWITCH_LSW_MGNT_NAME  "SwitchLSWMgnt"
#define METHOD_SET_LSW_HEALTH_STATE  "SetHealthState"
/* END for DTS2014082906128 : Added by ouluchun 00294050 2015-9-29 */
#define CLASS_PHY_MGNT_NAME       "PHYChipMgnt"
#define PORPERTY_PHY_CHIP_ADDR    "PhyAddr"
#define PORPERTY_PHY_LOOP_MODE    "LoopMode"


/* LSW SMM主备切换*/
#define CLASS_LSW_SMM_SWITCH              "SMMportswitch"
#define PROPERTY_LSW_SMM0_ADDR         "SMM0LSWaddr"
#define PROPERTY_LSW_SMM1_ADDR         "SMM1LSWaddr"
#define PROPERTY_LSW_SMM0_PHYNUM     "SMM0LSWphynum"
#define PROPERTY_LSW_SMM1_PHYNUM     "SMM1LSWphynum"
#define PROPERTY_LSW_SMM0_REG            "SMM0LSWreg"
#define PROPERTY_LSW_SMM1_REG            "SMM1LSWreg"
#define PROPERTY_LSW_MASTER_REGVLAUE  "enableLSWregvalue"
#define PROPERTY_LSW_SLAVE_REGVLAUE  "disableLSWregvalue"
#define METHOD_SET_LSW_PORT                 "SetLswMasterPort"


/*  minisas线缆基本信息类 */
#define CLASS_SASCABLE_NAME             "SASCable"
#define CLASS_EXTEND_SASCABLE_NAME      "ExtendSASCable"
#define CLASS_SLIMLINECABLE_NAME        "SLIMCable"

#define OBJECT_SASCABLE_NAME            "SASCableInfo"
#define OBJECT_EXTEND_SASCABLE_NAME     "ExtendSASCableInfo"
#define OBJECT_SLIMLINECABLE_NAME       "SLIMCableInfo"

#define PROPERTY_SASCABLE_MINISASSTATUS "MiniSasStatus"
#define PROPERTY_SASCABLE_DST_HBPBOARDID    "DST_HBP_BoardID"
#define PROPERTY_SASCABLE_DST_PORTA         "DST_PortA"
#define PROPERTY_SASCABLE_DST_PORTB         "DST_PortB"
#define PROPERTY_SASCABLE_DST_PORTC         "DST_PortC"
#define PROPERTY_SASCABLE_DST_PORTD         "DST_PortD"
#define PROPERTY_SASCABLE_DST_PORTE         "DST_PortE"
#define PROPERTY_SASCABLE_DST_PORTF         "DST_PortF"
#define PROPERTY_SASCABLE_DST_PORTG         "DST_PortG"
#define PROPERTY_SASCABLE_DST_PORTH         "DST_PortH"
#define PROPERTY_SASCABLE_DST_PORTI         "DST_PortI"
#define PROPERTY_SASCABLE_DST_PORTJ         "DST_PortJ"

#define PROPERTY_SASCABLE_DST_PORTK         "DST_PortK"
#define PROPERTY_SASCABLE_DST_PORTL         "DST_PortL"
#define PROPERTY_SASCABLE_DST_PORTM         "DST_PortM"
#define PROPERTY_SASCABLE_DST_PORTN         "DST_PortN"
#define PROPERTY_SASCABLE_DST_PORTO         "DST_PortO"
#define PROPERTY_SASCABLE_DST_PORTP         "DST_PortP"
#define PROPERTY_SASCABLE_DST_PORTQ         "DST_PortQ"
#define PROPERTY_SASCABLE_DST_PORTR         "DST_PortR"
#define PROPERTY_SASCABLE_DST_PORTS         "DST_PortS"
#define PROPERTY_SASCABLE_DST_PORTT         "DST_PortT"
#define PROPERTY_SASCABLE_DST_PORTU         "DST_PortU"
#define PROPERTY_SASCABLE_DST_PORTV         "DST_PortV"
#define PROPERTY_SASCABLE_DST_PORTW         "DST_PortW"
#define PROPERTY_SASCABLE_DST_PORTX         "DST_PortX"
#define PROPERTY_SASCABLE_DST_PORTY         "DST_PortY"
#define PROPERTY_SASCABLE_DST_PORTZ         "DST_PortZ"

/* BEGIN: Add for AR:Hardware_006 by xwx388597, 2016/12/6 */
#define PROPERTY_SASCABLE_DST_PORTA_STATUS  "DST_PortA_STUTS"
#define PROPERTY_SASCABLE_DST_PORTB_STATUS  "DST_PortB_STUTS"
#define PROPERTY_SASCABLE_DST_PORTC_STATUS  "DST_PortC_STUTS"
#define PROPERTY_SASCABLE_DST_PORTD_STATUS  "DST_PortD_STUTS"
#define PROPERTY_SASCABLE_DST_PORTE_STATUS  "DST_PortE_STUTS"
#define PROPERTY_SASCABLE_DST_PORTF_STATUS  "DST_PortF_STUTS"
#define PROPERTY_SASCABLE_DST_PORTG_STATUS  "DST_PortG_STUTS"
#define PROPERTY_SASCABLE_DST_PORTH_STATUS  "DST_PortH_STUTS"
#define PROPERTY_SASCABLE_DST_PORTI_STATUS  "DST_PortI_STUTS"
#define PROPERTY_SASCABLE_DST_PORTJ_STATUS  "DST_PortJ_STUTS"
#define PROPERTY_SASCABLE_DST_PORTK_STATUS  "DST_PortK_STUTS"
#define PROPERTY_SASCABLE_DST_PORTL_STATUS  "DST_PortL_STUTS"
#define PROPERTY_SASCABLE_DST_PORTM_STATUS  "DST_PortM_STUTS"
#define PROPERTY_SASCABLE_DST_PORTN_STATUS  "DST_PortN_STUTS"
#define PROPERTY_SASCABLE_DST_PORTO_STATUS  "DST_PortO_STUTS"
#define PROPERTY_SASCABLE_DST_PORTP_STATUS  "DST_PortP_STUTS"
#define PROPERTY_SASCABLE_DST_PORTQ_STATUS  "DST_PortQ_STUTS"
#define PROPERTY_SASCABLE_DST_PORTR_STATUS  "DST_PortR_STUTS"
#define PROPERTY_SASCABLE_DST_PORTS_STATUS  "DST_PortS_STUTS"
#define PROPERTY_SASCABLE_DST_PORTT_STATUS  "DST_PortT_STUTS"
#define PROPERTY_SASCABLE_DST_PORTU_STATUS  "DST_PortU_STUTS"
#define PROPERTY_SASCABLE_DST_PORTV_STATUS  "DST_PortV_STUTS"
#define PROPERTY_SASCABLE_DST_PORTW_STATUS  "DST_PortW_STUTS"
#define PROPERTY_SASCABLE_DST_PORTX_STATUS  "DST_PortX_STUTS"
#define PROPERTY_SASCABLE_DST_PORTY_STATUS  "DST_PortY_STUTS"
#define PROPERTY_SASCABLE_DST_PORTZ_STATUS  "DST_PortZ_STUTS"
/* END: Add for AR:Hardware_006 by xwx388597, 2016/12/6 */
/* BEGIN Modify by z00397989 AR.SR.SFEA02119720.017.001 2018.03.08*/
#define CLASS_STORAGE_CFG_CUSTOMER_NAME     "StorageCfgCustomize"
#define OBJECT_STORAGE_CFG_CUSTOMER_NAME   "StorageCfgCustomize"
#define PROPERTY_STORAGE_CFG_CUSTOMER_SLOTCUSTOMEID   "DiskSlotCustomId"

#define METHOD_STORAGE_CFG_CUSTOMER_SET_SLOT_ID     "SetDiskSlotCustomId"
/* END Modify by z00397989 AR.SR.SFEA02119720.017.001 2018.03.08*/

/* 硬盘Slot和CPU Phy控制器的映射类 */
#define CLASS_DISK_ADDR_INFO                "DiskAddrInfo"
#define PROPERTY_DISK_ADDR_INFO_SOCKET_ID   "SocketId"
#define PROPERTY_DISK_ADDR_INFO_CONTROL_ID  "ControlId"
#define PROPERTY_DISK_ADDR_INFO_PHY_ID      "PhyId"
#define PROPERTY_DISK_ADDR_INFO_SLOT_ID     "SlotId"
#define PROPERTY_DISK_ADDR_INFO_ROOT_BDF    "RootBDF"

/* BEGIN: Added by 00356691 zhongqiu, 2018/5/19   PN:AR.SR.SFEA02130925.003.001 支持采集的数据列表*/
#define METHOD_HDD_COLLECT_HDD_SMART       "CollectHddSMART"
/* END:   Added by 00356691 zhongqiu, 2018/5/19   PN:AR.SR.SFEA02130925.003.001 支持采集的数据列表*/
/* BEGIN: Modified by zwx382755, 2018/9/26   PN:AR.SR.SFEA02130924.051.001 */
#define METHOD_HDD_CRYPTO_ERASE             "CryptoErase"
/* END:   Modified by zwx382755, 2018/9/26 */
/* END:   Added by huanghan (h00282621), 2016/4/13 */

/* SAS线缆的配置类,需要配置一块单板支持的所有线缆配置 */
#define CLASS_SASCABLE_CFG_NAME            "SASCableConfig"
#define PROPERTY_SASCABLE_CFG_HBPBOARDID   "HBPBoardId"
#define PROPERTY_SASCABLE_CFG_PORTA        "SRC_PortA"
#define PROPERTY_SASCABLE_CFG_PORTB        "SRC_PortB"
#define PROPERTY_SASCABLE_CFG_PORTC        "SRC_PortC"
#define PROPERTY_SASCABLE_CFG_PORTD        "SRC_PortD"
#define PROPERTY_SASCABLE_CFG_PORTE        "SRC_PortE"
#define PROPERTY_SASCABLE_CFG_PORTF        "SRC_PortF"
#define PROPERTY_SASCABLE_CFG_PORTG        "SRC_PortG"
#define PROPERTY_SASCABLE_CFG_PORTH        "SRC_PortH"
#define PROPERTY_SASCABLE_CFG_PORTI        "SRC_PortI"
#define PROPERTY_SASCABLE_CFG_PORTJ        "SRC_PortJ"
#define CLASS_EXTEND_SASCABLE_CFG_NAME     "ExtendSASCableConfig"
#define PROPERTY_SASCABLE_CFG_PORTK        "SRC_PortK"
#define PROPERTY_SASCABLE_CFG_PORTL        "SRC_PortL"
#define PROPERTY_SASCABLE_CFG_PORTM        "SRC_PortM"
#define PROPERTY_SASCABLE_CFG_PORTN        "SRC_PortN"
#define PROPERTY_SASCABLE_CFG_PORTO        "SRC_PortO"
#define PROPERTY_SASCABLE_CFG_PORTP        "SRC_PortP"
#define PROPERTY_SASCABLE_CFG_PORTQ        "SRC_PortQ"
#define PROPERTY_SASCABLE_CFG_PORTR        "SRC_PortR"
#define PROPERTY_SASCABLE_CFG_PORTS        "SRC_PortS"
#define PROPERTY_SASCABLE_CFG_PORTT        "SRC_PortT"
#define PROPERTY_SASCABLE_CFG_PORTU        "SRC_PortU"
#define PROPERTY_SASCABLE_CFG_PORTV        "SRC_PortV"
#define PROPERTY_SASCABLE_CFG_PORTW        "SRC_PortW"
#define PROPERTY_SASCABLE_CFG_PORTX        "SRC_PortX"
#define PROPERTY_SASCABLE_CFG_PORTY        "SRC_PortY"
#define PROPERTY_SASCABLE_CFG_PORTZ        "SRC_PortZ"
#define CLASS_SLIMLINECABLE_CFG_NAME       "SLIMCableConfig"

#define CLASS_LOCAL_PCIECARD_NAME          "LocalPcieCard"          /* BMC芯片直连的本地PCIE卡类 */

#define CLASS_EXTEND_PCIECARD_NAME         "ExtendPcieCard"        /* PCIE标卡扩展类 */
#define PROPETRY_EXTEND_PCIECARD_BASECARD  "BaseCard"
/* BEGIN: Modified by lwx655206, 2019/9/6   UADP372212  */
#define PROPETRY_EXTEND_PCIECARD_SYNC_PFINFO    "SyncPfInfo"
#define EXTEND_PCIECARD_METHOD_GET_PFINFO       "GetPfInfo"
/* END:   Modified by lwx655206, 2019/9/6  */
#define CLASS_EXTEND_NETCARD_NAME          "ExtendNetCard"         /*PCIE标卡扩展网卡类*/
#define CLASS_EXTEND_NETCARD_SLOT          "SlotId"

#define CLASS_PCIECARD_NAME                "PcieCard"              /* PCIE标卡类 */
#define PROPETRY_PCIECARD_LINK_WIDTH_ABILITY    "LinkWidthAbility"
#define PROPETRY_PCIECARD_LINK_SPEED_ABILITY    "LinkSpeedAbility"
#define PROPETRY_PCIECARD_LINK_WIDTH            "LinkWidth"
#define PROPETRY_PCIECARD_LINK_SPEED            "LinkSpeed"
#define PROPETRY_PCIECARD_SLOT             "PcieCardSlot"
#define PROPERTY_PCIECARD_CPU_ID           "ResId"
#define PROPETRY_PCIECARD_MANUFACTURER     "Manufacturer"
#define PROPETRY_PCIECARD_PRESENCE         "Presence"
#define PROPERTY_PCIECARD_VID              "VenderId"              /* 厂商ID号 */
#define PROPERTY_PCIECARD_DID              "DeviceId"              /* 设备ID号 */
/* BEGIN: AR-0000276594-001-018. Modified by f00381872, 2017/4/9 */
#define PROPERTY_PCIECARD_SUBVID           "SubVenderId"
#define PROPERTY_PCIECARD_SUBDID           "SubDeviceId"
/* END: AR-0000276594-001-018. Modified by f00381872, 2017/4/9 */
/* BEGIN: AR-0000276582-002-008. Modified by zwx382233, 2017/4/19 */
#define PROPERTY_PCIECARD_FUNCTIONCLASS    "FunctionClass"
/* END: AR-0000276594-001-018. Modified by zwx382233, 2017/4/19 */

#define PROPERTY_PCIECARD_FUN_NUM          "FunctionNum"           /* 连接的CPU */
#define PROPERTY_PCIECARD_BUS_NUM          "BusNum"                /* 总线号 */
#define PROPERTY_PCIECARD_DEV_NUM          "DeviceNum"                /* 设备号 */
#define PROPERTY_PCIECARD_DESC             "CardDesc"
#define PROPERTY_PCIECARD_FATALERROR       "FatalError"
#define PROPERTY_PCIECARD_HEALTH           "Health"
#define PROPERTY_PCIECARD_FALT_BY_BIOS     "FaultByBIOS"
/* BEGIN: 2018-5-4 z00416979 AR.SR.SFEA0219782.007.012 支持BIOS阶段PCIE降带宽检测失败的上报 */
#define PROPERTY_PCIECARD_DECREASE_BANDWIDTH "DecreasedBandWidth"
/* END:   2018-5-4 z00416979 AR.SR.SFEA0219782.007.012 */
/* BEGIN: Modified by gwx455466, 2018/6/27 10:28:17   问题单号:DTS2018053003818 */
#define PROPERTY_PCIECARD_RELOAD_XML   "ReloadXml"
/* END:   Modified by gwx455466, 2018/6/27 10:28:19 */
// 201705108246 需求支持PARNUM查询, 20190723 RAOJIANZHONG 00267466
#define PROPERTY_PCIECARD_PART_NUMBER      "PartNum"
#define PROPERTY_PCIECARD_SUPPORT_PCIIDS      "IndentifyInIDS"
#define PROPERTY_PCIE_CARD_LOGIC_SLOT       "LogicSlot"              /* 标卡丝印 */



#define CLASS_OSCAPCIECARD_NAME            "OscaPcieCard"          /* PCIE标卡类类 */

#define CLASS_RISERPCIECARD_NAME               "RiserPcieCard"         /* riser卡*/
#define PROPERTY_RISERPCIECARD_NAME            "Name"
#define PROPETRY_RISERPCIECARD_SLOT            "Slot"
#define PROPETRY_RISERPCIECARD_PRESENCE        "Presence"
#define PROPERTY_RISERPCIECARD_SUBSLOTCOUNT    "SubSlotCount"
#define PROPERTY_RISERPCIECARD_SUBSLOTWIDTH    "SubSlotWidth"
#define PROPERTY_RISERPCIECARD_HEALTH          "Health"
#define PROPERTY_RISERPCIECARD_BOARDID         "BoardId"
#define PROPERTY_RISERPCIECARD_MANUFACTURER    "Manufacturer"
#define PROPERTY_RISERPCIECARD_TYPE            "Type"
#define PROPERTY_RISERPCIECARD_TYPESET         "TypeSet"
#define PROPERTY_RISERPCIECARD_TYPEVALUE       "TypeValue"
#define PROPERTY_RISERPCIECARD_LOGICVERID   "LogicVerId"
#define PROPERTY_RISERPCIECARD_LOGICVER     "LogicVer"
#define PROPERTY_RISERPCIECARD_PCBID        "PcbId"
#define PROPERTY_RISERPCIECARD_PCBVER       "PcbVer"
#define PROPERTY_RISERPCIECARD_LOGICUNIT       "LogicUnit"
// 201705108246 需求支持PARNUM查询, 20190723 RAOJIANZHONG 00267466
#define PROPERTY_RISERPCIECARD_PART_NUMBER      "PartNum"


#define CLASS_SASEXPANDER_NAME                  "SASExpander"
#define PROPERTY_SASEXPANDER_EXPANDER_ID        "ExpanderId"
#define PROPERTY_SASEXPANDER_REF_RAID_NAME      "RefRAIDName"
#define PROPERTY_SASEXPANDER_REF_FRU_TYPE       "RefFruType"
#define PROPERTY_SASEXPANDER_REF_FRU_NAME       "RefFruName"
#define PROPERTY_SASEXPANDER_CONNECT_FRU_TYPE   "ConnectFruType"
#define PROPERTY_SASEXPANDER_CONNECT_FRU_NAME   "ConnectFruName"
#define PROPERTY_SASEXPANDER_CONNECT_FRU_ID     "ConnectFruId"
#define PROPERTY_SASEXPANDER_PHY_ERR_STATE      "PHYErrState"
#define PROPERTY_SASEXPANDER_COMM_ERR_STATE     "CommErrState"

#define CLASS_HDDBACKPLANE_NAME            "HDDBackplane"
#define CLASS_VIRTUAL_HDDBACKPLANE_NAME    "VirtualHDDBackplane"
#define PROPETRY_HDDBACKPLANE_SLOT         "Slot"
#define PROPERTY_HDDBACKPLANE_NAME         "Name"
#define PROPERTY_HDDBACKPLANE_PRESENCE     "Presence"
#define PROPERTY_HDDBACKPLANE_LOGICUNIT    "LogicUnit"
#define PROPERTY_HDDBACKPLANE_LOGICVERID   "LogicVerId"
#define PROPERTY_HDDBACKPLANE_LOGICVER     "LogicVer"
#define PROPERTY_HDDBACKPLANE_PCBID        "PcbId"
#define PROPERTY_HDDBACKPLANE_PCBVER       "PcbVer"
#define PROPERTY_HDDBACKPLANE_BOARDID      "BoardId"
#define PROPERTY_HDDBACKPLANE_TYPE         "Type"
#define PROPERTY_HDDBACKPLANE_MANUFACTURER "Manufacturer"
#define PROPERTY_HDDBACKPLANE_STARTSLOT    "StartSlot"
#define PROPERTY_HDDBACKPLANE_SLOTCOUNT    "SlotCount"
#define PROPERTY_HDDBACKPLANE_PCIESLOT_SILKARRAY     "PcieSlotSilkTextArray"
#define PROPERTY_HDDBACKPLANE_DISKS_POWERCTRL        "DisksPowerCtrl"
#define PROPERTY_HDDBACKPLANE_HDDSASMAXTEMP       "HddSasMaxTemp"
#define PROPERTY_HDDBACKPLANE_SSDSASMAXTEMP       "SsdSasMaxTemp"
#define METHOD_HDDBACKPLANE_SET_HDD_MAX_TEMP       "SetHddBPTemp"
#define PROPERTY_HDDBACKPLANE_REF_COMPONENT    "RefComponent"
// 201705108246 需求支持PARNUM查询, 20190723 RAOJIANZHONG 00267466
#define PROPERTY_HDDBACKPLANE_PART_NUMBER      "PartNum"

#define CLASS_RAIDCARD_NAME                "Raid"
/* BEGIN: Modified by zwx382233, 2017/5/18 19:26:5   问题单号:DTS2017050904064  */
#define PROPERTY_RAIDCARD_NAME             "ProductName"
/* END:   Modified by zwx382233, 2017/5/18 19:26:9 */
#define PROPERTY_RAIDCARD_SLOT             "SlotId"
#define PROPERTY_RAIDCARD_CPUID            "ResId"
#define PROPERTY_RAIDCARD_PRESENCE         "Presence"
#define PROPERTY_RAIDCARD_LOGICUNIT        "LogicUnit"
#define PROPERTY_RAIDCARD_LOGICVERID       "LogicVerId"
#define PROPERTY_RAIDCARD_LOGICVER         "LogicVer"
#define PROPERTY_RAIDCARD_PCBID            "PcbId"
#define PROPERTY_RAIDCARD_PCBVER           "PcbVer"
#define PROPERTY_RAIDCARD_BOARDID          "BoardId"
#define PROPERTY_RAIDCARD_TYPE             "Type"
#define PROPERTY_RAIDCARD_MODEL            "Model"
#define PROPERTY_RAIDCARD_HEALTH           "Health"
#define PROPERTY_RAIDCARD_MANUFACTURER     "Manufacturer"
#define PROPERTY_RAIDCARD_FAULT            "Fault"
#define PROPERTY_RAIDCARD_STATUS           "Status"
#define PROPERTY_RAIDCARD_FAULT_BY_BIOS    "FaultByBIOS"
#define PROPERTY_RAIDCARD_REF_COMPONENT    "RefComponent"
#define PROPERTY_RAIDCARD_PART_NUMBER      "PartNum"
#define PROPERTY_RAIDCARD_VID              "VenderId"              /* 厂商ID号 */
#define PROPERTY_RAIDCARD_DID              "DeviceId"              /* 设备ID号 */
#define PROPERTY_RAIDCARD_SVID             "SubVenderId"           /* 厂商子ID号 */
#define PROPERTY_RAIDCARD_SDID             "SubDeviceId"           /* 设备子ID号 */

#define CLASS_SASPHYSTATUS_NAME                             "SASPHYStatus"
#define PROPERTY_SASPHYSTATUS_PHY_ID                        "PHYId"
#define PROPERTY_SASPHYSTATUS_INVALID_DWORD_COUNT           "InvalidDwordCount"
#define PROPERTY_SASPHYSTATUS_LOSS_DWORD_SYNC_COUNT         "LossDwordSyncCount"
#define PROPERTY_SASPHYSTATUS_PHY_RESET_PROBLEM_COUNT       "PhyResetProblemCount"
#define PROPERTY_SASPHYSTATUS_RUNNING_DISPARITY_ERROR_COUNT "RunningDisparityErrorCount"

#define CLASS_LOGICAL_DRIVE_NAME                       "LogicalDrive"
#define PROPERTY_LOGICAL_DRIVE_REF_RAID_CONTROLLER     "RefRAIDController"
#define PROPERTY_LOGICAL_DRIVE_TARGET_ID               "TargetID"
#define PROPERTY_LOGICAL_DRIVE_NAME                    "DriveName"
#define PROPERTY_LOGICAL_DRIVE_STATUS                  "DriveStatus"
#define PROPERTY_LOGICAL_DRIVE_HEALTH                  "HealthStatus"
#define PROPERTY_LOGICAL_DRIVE_RAID_LEVEL              "RaidLevel"
#define PROPERTY_LOGICAL_DRIVE_DEF_READ_POLICY         "DefaultReadPolicy"
#define PROPERTY_LOGICAL_DRIVE_DEF_WRITE_POLICY        "DefaultWritePolicy"
#define PROPERTY_LOGICAL_DRIVE_DEF_CACHE_POLICY        "DefaultCachePolicy"
#define PROPERTY_LOGICAL_DRIVE_CUR_READ_POLICY         "CurrentReadPolicy"
#define PROPERTY_LOGICAL_DRIVE_CUR_WRITE_POLICY        "CurrentWritePolicy"
#define PROPERTY_LOGICAL_DRIVE_CUR_CACHE_POLICY        "CurrentCachePolicy"
#define PROPERTY_LOGICAL_DRIVE_ACCESS_POLICY           "AccessPolicy"
#define PROPERTY_LOGICAL_DRIVE_STRIP_SZIE              "StripSize"
#define PROPERTY_LOGICAL_DRIVE_SIZE                    "Size"
#define PROPERTY_LOGICAL_DRIVE_MAX_RESIZABLE_SIZE      "MaxResizableSize"
#define PROPERTY_LOGICAL_DRIVE_SPAN_DEPTH              "SpanDepth"
#define PROPERTY_LOGICAL_DRIVE_NUMDRIVE_PER_SPAN       "NumDrivePerSpan"
#define PROPERTY_LOGICAL_DRIVE_DISK_CACHE_POLICY       "DiskCachePolicy"
#define PROPERTY_LOGICAL_DRIVE_INIT_STATE              "InitState"
#define PROPERTY_LOGICAL_DRIVE_CC_STATE                "ConsistencyCheck"
#define PROPERTY_LOGICAL_DRIVE_BGI_ENABLED             "BGIEnabled"
#define PROPERTY_LOGICAL_DRIVE_BOOTABLE                "IsBootable"
#define PROPERTY_LOGICAL_DRIVE_BOOT_PRIORITY           "BootPriority"
#define PROPERTY_LOGICAL_DRIVE_IS_SSCD                 "IsSSCD"
#define PROPERTY_LOGICAL_DRIVE_CACHECADE_LD            "CacheCadeLD"
#define PROPERTY_LOGICAL_DRIVE_ASSOCIATED_LD           "AssociatedLDs"
#define PROPERTY_LOGICAL_DRIVE_ASSOCIATED_CACHECADE    "AssociatedCacheCadeLDs"
#define PROPERTY_LOGICAL_DRIVE_REF_ARRAY               "RefArray"
/* BEGIN: Modified by gwx455466, 2017/9/5 14:20:23   问题单号:DTS2017083101891  */
#define PROPERTY_LOGICAL_DRIVE_PARTICIPATED_PD_SLOT         "ParticipatedPDSlot"
#define PROPERTY_LOGICAL_DRIVE_DEDICATED_SPARED_PD_SLOT     "DedicatedSparedPDSlots"
#define PROPERTY_LOGICAL_DRIVE_PARTICIPATED_PD_ENCLOSURE         "ParticipatedPDEnclosure"
#define PROPERTY_LOGICAL_DRIVE_DEDICATED_SPARED_PD_ENCLOSURE     "DedicatedSparedPDEnclosures"
/* END:   Modified by gwx455466, 2017/9/5 14:20:29 */
/* BEGIN: Added by z00352904, 2016/12/2 16:17:35  PN:AR-0000264509-001-001 */
#define PROPERTY_LOGICAL_DRIVE_OS_DRIVE_NAME           "OSDriveName"
#define PROPERTY_LOGICAL_DRIVE_BMA_ID                  "BMAId"
/* END:   Added by z00352904, 2016/12/2 16:17:40*/
/* BEGIN: Modified by zhanglei wx382755, 2017/12/2   PN:AR.SR.SFEA02109379.004.006 */
#define PROPERTY_LOGICAL_DRIVE_CURRENT_FGI_STATE       "CurrentFGIState"
#define PROPERTY_LOGICAL_DRIVE_FGI_PROGRESS            "FGIProgress"
/* END:   Modified by zhanglei wx382755, 2017/12/2 */
#define PROPERTY_LOGICAL_DRIVE_ACCELERATION_METHOD     "AccelerationMethod"
#define PROPERTY_LOGICAL_DRIVE_CACHE_LINE_SIZE         "CacheLineSize"
#define PROPERTY_LOGICAL_DRIVE_REBUILD_STATE           "RebuildState"
#define PROPERTY_LOGICAL_DRIVE_REBUILDPROGRESS         "RebuildProgress"
#define METHOD_LOGICAL_DRIVE_SET_NAME                  "SetName"
#define METHOD_LOGICAL_DRIVE_SET_READ_POLICY           "SetReadPolicy"
#define METHOD_LOGICAL_DRIVE_SET_WRITE_POLICY          "SetWritePolicy"
#define METHOD_LOGICAL_DRIVE_SET_IO_POLICY             "SetIOPolicy"
#define METHOD_LOGICAL_DRIVE_SET_ACCESS_POLICY         "SetAccessPolicy"
#define METHOD_LOGICAL_DRIVE_SET_DISK_CACHE_POLICY     "SetDiskCachePolicy"
#define METHOD_LOGICAL_DRIVE_SET_BGI                   "SetBGI"
#define METHOD_LOGICAL_DRIVE_SET_CACHECADE             "SetCachecade"
#define METHOD_LOGICAL_DRIVE_SET_BOOTABLE              "SetBootable"
#define METHOD_LOGICAL_DRIVE_SET_ACCELERATION_METHOD   "SetAccelerationMethod"
#define METHOD_LOGICAL_DRIVE_SET_CAPACITY              "SetCapacity"
#define METHOD_LOGICAL_DRIVE_SET_STRIP_SIZE            "SetStripSize"
/* BEGIN: Modified by zhanglei wx382755, 2017/12/2   PN:AR.SR.SFEA02109379.004.006 */
#define METHOD_LOGICAL_DRIVE_START_FGI                "StartFGI"
#define METHOD_LOGICAL_DRIVE_CANCEL_FGI               "CancelFGI"
/* END:   Modified by zhanglei wx382755, 2017/12/2 */
/* END:   Added by huanghan (h00282621), 2016/3/8 */

/* BEGIN: Added by huanghan (h00282621), 2016/11/9   问题单号:AR-0000264511-001-001*/
#define CLASS_DISK_ARRAY_NAME                          "DiskArray"
#define CLASS_DISK_SUB_ARRAY_NAME                      "DiskSubArray"
#define PROPERTY_DISK_ARRAY_REF_RAID_CONTROLLER        "RefRAIDController"
#define PROPERTY_DISK_ARRAY_ID                         "ArrayID"
#define PROPERTY_DISK_ARRAY_USED_SPACE                 "UsedSpace"
#define PROPERTY_DISK_ARRAY_TOTAL_FREE_SPACE           "TotalFreeSpace"         // DTS2019051404640根据赋值的内容将名字从 FreeSpace 修改成 TotalFreeSpace
#define PROPERTY_DISK_ARRAY_FREE_BLOCKS_SPACE          "FreeBlocksSpace"        // DTS2019051404640新增
#define PROPERTY_DISK_ARRAY_LD_COUNT                   "LDCount"
#define PROPERTY_DISK_ARRAY_REF_LDS                    "RefLDs"
#define PROPERTY_DISK_ARRAY_PD_COUNT                   "PDCount"
#define PROPERTY_DISK_ARRAY_REF_PD_SLOTS                    "RefPDSlots"
/* BEGIN: Modified by gwx455466, 2017/9/5 16:29:42   问题单号:DTS2017083101891  */
#define PROPERTY_DISK_ARRAY_REF_PD_ENCLOSURES         "RefPDEnclosures"
/* END:   Modified by gwx455466, 2017/9/5 16:29:45 */
/* END:   Added by huanghan (h00282621), 2016/11/9 */

#define CLASS_MEZZCARD_NAME                "MezzCard"
#define PROPERTY_MEZZCARD_FRU_ID           "FruId"
#define PROPERTY_MEZZCARD_INTERFACE_TYPE   "InterfaceType"
#define PROPERTY_MEZZCARD_MAC_ADDR         "MacAddr"
#define PROPERTY_MEZZCARD_NAME             "Name"
#define PROPERTY_MEZZCARD_SLOT             "Slot"
#define PROPERTY_MEZZCARD_CPUID            "ResId"
#define PROPERTY_MEZZCARD_WIDTHTYPE        "WidthType"
#define PROPERTY_MEZZCARD_PRESENCE         "Presence"
#define PROPERTY_MEZZCARD_LOGICUNIT        "LogicUnit"
#define PROPERTY_MEZZCARD_LOGICVERID       "LogicVerId"
#define PROPERTY_MEZZCARD_LOGICVER         "LogicVer"
#define PROPERTY_MEZZCARD_PCBID            "PcbId"
#define PROPERTY_MEZZCARD_PCBVER           "PcbVer"
#define PROPERTY_MEZZCARD_BOARDID          "BoardId"
/** BEGIN: Modified by l00272597, 2015/12/2   PN:DTS2015102904577  */
#define PROPERTY_MEZZCARD_PRODUCTNAME      "ProductName"
/** END:   Modified by l00272597, 2015/12/2 */
#define PROPERTY_MEZZCARD_TYPE             "Type"
#define PROPERTY_MEZZCARD_DESC             "Desc"
#define PROPERTY_MEZZCARD_MANUFACTURER     "Manufacturer"
#define PROPERTY_MEZZCARD_HEALTH           "Health"
// 201705108246 需求支持PARNUM查询, 20190723 RAOJIANZHONG 00267466
#define PROPERTY_MEZZCARD_PART_NUM         "PartNum"

#define METHOD_SET_MEZZ_MAC_ADDR           "SetMezzMacAddr"

/* BEGIN: Added by zwxc388296, 2017/10/31   问题单号:AREA02109385-008-001*/
#define CLASS_MEZZCARDOPA_NAME                "OpaMezzInfo"
#define PROPERTY_MEZZCARDOPA_SVIDTYPECHIP           "OpaSvidChip"
#define PROPERTY_MEZZCARDOPA_MEZZTEMP           "OpaTemp"
#define PROPERTY_MEZZCARDOPA_MEZZVCCTHD           "OpaVolt1Threshold"
#define PROPERTY_MEZZCARDOPA_MEZZVCC0V9           "OpaVolt1"
#define PROPERTY_MEZZCARDOPA_MEZZVCC1V0           "OpaVolt2"
#define PROPERTY_MEZZCARDOPA_MEZZVCC1V8           "OpaVolt3"
#define PROPERTY_MEZZCARDOPA_MEZZVCC2V5           "OpaVolt4"
#define PROPERTY_MEZZCARDOPA_MEZZVCC3V3           "OpaVolt5"
#define PROPERTY_MEZZCARDOPA_MEZZSVIDREADFAIL          "MezzSvidReadFail"
/** END:   Modified by zwx388296, 2017/10/31 */




#define CLASS_PERIPHERAL_CARD_NAME     "PeripheralCard"
#define CLASS_M2TRANSFORMCARD		   "M2TransformCard"
#define PROPERTY_M2TRANSFORMCARD_SSD1PRESENCE   "SSD1Presence"
#define PROPERTY_M2TRANSFORMCARD_SSD2PRESENCE   "SSD2Presence"
// 201705108246 需求支持PARNUM查询, 20190723 RAOJIANZHONG 00267466
#define PROPERTY_M2TRANSFORMCARD_PART_NUMBER      "PartNum"

#define CLASS_PCIETRANSFORMCARD		   "PCIeTransformCard"
#define CLASS_PASSTHROUGH_CARD     	   "PassThroughCard"
// 201705108246 需求支持PARNUM查询, 20190723 RAOJIANZHONG 00267466
#define PROPERTY_PASSTHROUGH_CARD_PART_NUMBER      "PartNum"

#define CLASS_ACCLERATE_CARD     	   "AccelerateCard"

/* BEGIN: Added by TWX555928, 2018/6/8   PN:X6800 V5 改板*/
#define CLASS_HORIZ_CONN_BRD           "HorizConnectionBoard"
#define PROPERTY_HORIZ_CONN_BRD_NAME   "Name"
#define PROPERTY_HORIZ_CONN_BRD_PCBVER "PcbVer"
#define PROPERTY_HORIZ_CONN_BRD_BRDID  "BoardId"
/* END:   Added by TWX555928, 2018/6/8 */

// CIC卡管理
#define CLASS_CIC_CARD     	   "CICCard"
#define PROPERTY_CIC_M2APRESENCE       "M2Device1Presence"
#define PROPERTY_CIC_M2BPRESENCE       "M2Device2Presence"
#define PROPERTY_CIC_PRESENCE          "Presence"

#define CLASS_CARD_NAME                "Card"
#define PROPERTY_CARD_NAME             "Name"
#define PROPERTY_CARD_SLOT             "Slot"
#define PROPERTY_CARD_WIDTHTYPE        "WidthType"
#define PROPERTY_CARD_PRESENCE         "Presence"
#define PROPERTY_CARD_LOGICUNIT        "LogicUnit"
#define PROPERTY_CARD_LOGICVERID       "LogicVerId"
#define PROPERTY_CARD_LOGICVER         "LogicVer"
#define PROPERTY_CARD_PCBID            "PcbId"
#define PROPERTY_CARD_PCBVER           "PcbVer"
#define PROPERTY_CARD_BOARDID          "BoardId"
#define PROPERTY_CARD_BOARDNAME        "BoardName"
#define PROPERTY_CARD_TYPE             "Type"
#define PROPERTY_CARD_DESC             "Desc"
#define PROPERTY_CARD_MANUFACTURER     "Manufacturer"
#define PROPERTY_CARD_HEALTH           "Health"
#define PROPERTY_PERIPHERAL_CARD_TARGET              "Target"
/* Start by h00272616 DTS2016022909391 20160505 */
#define PROPERTY_ACCELERATE_CARD_BOMID  "BomId"
/* End by h00272616 DTS2016022909391 20160505 */

#define CLASS_EXPBOARD_NAME                "ExpBoard"
#define CLASS_FAN_MODULE_NAME              "FanModule"
#define PROPERTY_EXPBOARD_NAME             "Name"
#define PROPERTY_EXPBOARD_PCBVER           "PcbVer"
#define PROPERTY_EXPBOARD_PCBID            "PcbId"
#define PROPERTY_EXPBOARD_SLOT             "Slot"
#define PROPERTY_EXPBOARD_PRESENCE         "Presence"
#define PROPERTY_EXPBOARD_LOGICUNIT        "LogicUnit"
#define PROPERTY_EXPBOARD_LOGICVERID       "LogicVerId"
#define PROPERTY_EXPBOARD_LOGICVER         "LogicVer"
#define PROPERTY_EXPBOARD_BOARDID          "BoardId"
#define PROPERTY_EXPBOARD_LOGICUNIT        "LogicUnit"

#define CLASS_GPUBOARD_NAME                "GPUBoard"
#define PROPERTY_GPUBOARD_NAME             "Name"
#define PROPERTY_GPUBOARD_PRESENCE         "Presence"
#define PROPERTY_GPUBOARD_LOGICUNIT        "LogicUnit"
#define PROPERTY_GPUBOARD_LOGICVERID       "LogicVerId"
#define PROPERTY_GPUBOARD_LOGICVER         "LogicVer"
#define PROPERTY_GPUBOARD_PCBID            "PcbId"
#define PROPERTY_GPUBOARD_PCBVER           "PcbVer"
#define PROPERTY_GPUBOARD_BOARDID          "BoardId"
#define PROPERTY_GPUBOARD_TYPE             "Type"
#define PROPERTY_GPUBOARD_MANUFACTURER     "Manufacturer"
#define PROPERTY_GPUBOARD_SERIALNUMBER     "SerialNumber"
#define PROPERTY_GPUBOARD_SLOT             "Slot"
#define PROPERTY_GPUBOARD_BOARDNAME        "BoardName"
#define PROPERTY_GPUBOARD_HEALTH           "Health"
#define PROPERTY_GPUBOARD_POWER            "Power"
/* BEGIN: Added by 00356691 zhongqiu, 2018/2/5   PN:DTS2018020200599 StringFormator类优化*/
#define PROPERTY_GPUBOARD_DUMPACCESSOR     "DumpAccessor"
#define PROPERTY_GPUBOARD_DUMPSIZE         "DumpSize"
/* END:   Added by 00356691 zhongqiu, 2018/2/5   PN:DTS2018020200599 StringFormator类优化*/
#define PROPERTY_GPUBOARD_PCIESLOTNUM   "PcieSlotNum"

/* BEGIN: UADP337667. Modified by f00381872, 2019/10/16 */
#define CLASS_LEAK_DET_CARD                     "LeakageDetectionCard"
#define PROPERTY_LEAK_WATERDET_CABLE_PRES_A     "WaterDetectionCableAPres"
#define PROPERTY_LEAK_WATERDET_CABLE_PRES_B     "WaterDetectionCableBPres"
#define PROPERTY_LEAK_MAGVALVE_CTRL             "MagneticValveSwitchCtrl"
#define PROPERTY_LEAK_MAGVALVE_STATE            "MagneticValveSwitchState"
#define PROPERTY_LEAK_MAGVALVE_PRESENT          "MagneticValvePresent"
#define PROPERTY_LEAK_TEST                      "LeakageDetectionTest"
#define PROPERTY_LEAK_LEAK_ALARM                "LeakageState"
#define PROPERTY_LEAK_PARTNUM                   "BomCode"
#define PROPERTY_LEAK_WATERDET_CABLE_PRESENCE   "WaterDetectionCablePresence"
#define PROPERTY_LEAK_LEAKAGE_STATUS            "LeakageStatus"
#define METHOD_LEAK_CTRL_MAGVALVE_STATE         "SetMagneticValveSwitch"
#define METHOD_LEAK_TEST_ALARM                  "LeakageDetectionMockTest"

#define LEAK_ALARM_EVENT_POLICY_OBJ             "LiquidLeakagePlcy"

#define CLASS_CONVERGE_BOARD                    "ConvergeBoard"
#define PROPERTY_CONVERGE_BOARD_CPLD_VER_REG    "CpldVerReg"
#define PROPERTY_CONVERGE_BOARD_CPLD_UNIT       "CpldUnit"
#define PROPERTY_CONVERGE_BOARD_CPLD_VERSION    "CpldVersion"

/* BEGIN: Added by zengrenchang(z00296903), 2017/6/19   问题单号:	AR-0000276586-008-001*/
#define CLASS_NETCARD_CHIP_NAME                 "NetCardChip"
#define PROPERTY_NETCARD_CHIP_NUM               "ChipNum"
#define PROPERTY_NETCARD_CHIP_MODEL             "ChipModel"
#define PROPERTY_NETCARD_CHIP_PORT_NUM          "ChipPortNum"
#define PROPERTY_NETCARD_CHIP_MANUFACTURE       "ChipManufacture"
/* END:   Added by zengrenchang(z00296903), 2017/6/19 */
 /* BEGIN: Modified by lwx459244, 2018/3/19 10:20:11   PN:DTS2018031606349  */
#define CLASS_SPECIAL_PCIECARD_NAME         "SpecialPcieCard"
#define PROPERTY_REF_PCIECARD               "RefPcieCard"
#define PROPERTY_REF_VENDER_ID              "VenderId"
#define PROPERTY_REF_DEVICE_ID              "DeviceId"
#define PROPERTY_REF_SUBVENDER_ID           "SubVenderId"
#define PROPERTY_REF_SUBDEVICE_ID           "SubDeviceId"
/* END: Modified by lwx459244, 2018/3/19 10:20:11   PN:DTS2018031606349  */

#define CLASS_NETCARD_NAME                 "NetCard"
#define PROPERTY_OCPCARD_REF_CONFIG        "RefConfig"
/* BEGIN: Added by zengrenchang(z00296903), 2017/6/19   问题单号:   AR-0000276586-008-001*/
#define BUSY_NETCARD_FIRM_VERSION_FROM_MCTP  "FirmwareVerFromMCTP"
#define BUSY_NETCARD_METHOD_SET_FV_FROM_MCTP "SetFVerFromMCTP"
#define BUSY_NETCARD_METHOD_SET_SERIAL_NUMBER "SetSN"
#define BUSY_NETCARD_METHOD_SET_VID_DID_FROM_MCTP "SetVIdDIdFromMCTP"
#define PROPERTY_REF_NETCARD_CHIP_NAME      "RefNetCardChip"
/* END:   Added by zengrenchang(z00296903), 2017/6/19 */
/* END:   Modified by zwx382233, 2017/6/6 14:30:59 */
#define PROPERTY_IOBOARD_REFCOMPONENT     "RefComponent"
#define PROPERTY_NETCARD_NAME              "ProductName"
#define PROPERTY_NETCARD_BOARDNAME         "BoardName"
/* END:   Modified by zwx382233, 2017/6/6 14:31:2 */
#define PROPERTY_NETCARD_DESC              "CardDesc"
#define PROPERTY_NETCARD_TYPE              "TYPE"
#define PROPERTY_NETCARD_PORTNUM           "PortNum"
#define PROPERTY_NETCARD_CARDTYPE          "CardType"
#define PROPERTY_NETCARD_NCSI_SUPPORTED    "NcsiSupported"
#define PROPERTY_NETCARD_SLOT_ID           "SlotId"
#define PROPERTY_NETCARD_CPU_ID            "ResId"
#define PROPERTY_NETCARD_REF_FRU            "RefFru"
/* BEGIN: 2018-4-17 z00416979 AR.SR.SFEA02130924.001.001 */
#define PROPERTY_NETCARD_ROOT_BDF          "RootBDF"
/* END:   2018-4-17 z00416979 AR.SR.SFEA02130924.001.001 */
#define PROPERTY_NETCARD_STATUS            "Health"
#define PROPERTY_NETCARD_REFSTDBYCOOL_CONN "RefStdbyCoolingConn"
#define PROPERTY_NETCARD_CHIP_TEMP         "ChipTemp"
#define METHOD_NETCARD_SET_CHIP_TEMP       "SetChipTemp"

/* BEGIN: Added by z00352904, 2016/12/8 16:14:6  PN:AR-0000264509-001-001 */
#define BUSY_NETCARD_MANU                  "Manufacture"
#define BUSY_NETCARD_HEALTH                "Health"
#define BUSY_NETCARD_FDMFAULT              "FDMFault"
#define BUSY_NETCARD_OSNICNAME             "OSNICName"
#define BUSY_NETCARD_MODEL                 "Model"
#define BUSY_NETCARD_FIRM_VERSION          "FirmwareVersion"
#define BUSY_NETCARD_DRIVE_NAME            "DriverName"
#define BUSY_NETCARD_DRIVE_VERSION         "DriverVersion"
#define BUSY_NETCARD_DESCRIPTION           "Description"
#define BUSY_NETCARD_CHIP_MANU             "ChipManufacturer"
#define BUSY_NETCARD_ROOT_BDF              "RootBDF"
#define BUSY_NETCARD_METHOD_SET_BMA_PROP   "SetCardPropFromBMA"
#define BUSY_NETCARD_MCTP_CAPABILITY       "MCTPCapability"
/* END:   Added by z00352904, 2016/12/8 16:14:9*/
#define PROPERTY_NETCARD_VENDER_ID         "VenderId"
#define PROPERTY_NETCARD_DEVICE_ID         "DeviceId"
#define PROPERTY_NETCARD_SUB_VENDER_VID    "SubVenderId"
#define PROPERTY_NETCARD_SUB_DEVICE_ID     "SubDeviceId"

/* Start by p00381829  mctp  2018/01/25 */
#define PROPERTY_NETCARD_EID                "Eid"                    /* PCIE卡的EID*/
#define PROPERTY_NETCARD_PHY_ADDR         "PhyAddr"
/* End by p00381829  mctp  2018/01/25 */
/* BEGIN: Added by tangjie, 2018/5/7   问题单号:SR.SFEA02130925.002 基于MCTP管理网卡 */
#define PROPERTY_NETCARD_MCTP_SUPPORT            "MctpSupport"
#define PROPERTY_NETCARD_HOTPLUG_SUPPORT            "SupportHotPlug"
#define PROPERTY_NETCARD_PREPLUG_STATE                 "PrePlug"
#define PROPERTY_NETCARD_HOTPLUG_CONTROL            "HotplugCtrlStatus"
#define OCP_CARD_INOPERABLE      0x0
#define OCP_CARD_REMOVEABLE     0x1
#define OCP_CARD_INSERTABLE       0x2
#define PROPERTY_NETCARD_MANU_ID         "ManufacturerId"
#define METHOD_NETCARD_SET_MANU_ID         "SetManufacturerId"
/* END:   Added by tangjie, 2018/5/7 */

#define PROPERTY_NETCARD_LLDP_ENABLE    "LLDPEnableFlag"
#define PROPERTY_NETCARD_LLDPOVERNCSI_ENABLE    "LldpOverNcsiEnable"
#define METHOD_NETCARD_SET_LLDPOVERNCSI_ENABLE   "SetLldpOverNcsiEnable"
#define PORPERTY_NETCARD_LLDP_SUPPORTED  "LLDPSupported"
/* BEGIN: Added by p00381829, 2017/12/8 mctp */
#define  METHOD_SET_NETCARD_MCTP_IDENTITY        "SetMCTPIdentity"
/* END:   Added by p00381829, 2017/12/8 mctp*/
#define  METHOD_SET_NETCARD_PRE_PLUG_STATE                "SetPrePlug"
#define METHOD_SET_LLDP_ENABLE                  "SetNetCardLLDPEnable"
#define PROPERTY_NETCARD_VIRTUAL_FLAG   "VirtualNetCardFlag"
#define PROPERTY_NETCARD_NCSI_PACKAGE_ID   "NcsiPackageId"
#define NETCARD_METHOD_SET_NCSI_PACKAGE_ID   "SetNcsiPackageId"
#define PROPERTY_NETCARD_SERIAL_NUMBER  "SerialNumber"
#define METHOD_NETCARD_SET_SERIAL_NUMBER    "SetSerialNumber"
#define METHOD_NETCARD_SET_ROOTBDF   "SetNetCardRootBDF"
/* BEGIN: Modified by jwx372839, 2017/8/31 9:57:27   问题单号:AREA02109379-006-001 */
#define BUSY_NETCARD_BWU_WAVE_FILENAME     "BWUWaveFileName"
#define BUSY_NETCARD_BWU_WAVE_TITLE        "BWUWaveTitle"
#define BUSY_NETCARD_METHOD_UPDATE_BWU_WAVE "UpdateNetworkBWUWave"
#define PROPERTY_NETCARD_REF_COMPONENT     "RefComponent"
#define CLASS_OCPCONTROL_NAME                              "OcpCardControl"
#define PROPERTY_OCPCONTROL_THYS                         "THYS"
#define PROPERTY_OCPCONTROL_THYS_REG                  "THYSReg"
#define PROPERTY_OCPCONTROL_THYS_CONVERT          "THYSConvert"
#define PROPERTY_OCPCONTROL_TOS                           "TOS"
#define PROPERTY_OCPCONTROL_TOS_REG                    "TOSReg"
#define PROPERTY_OCPCONTROL_TOS_CONVERT            "TOSConvert"
#define PROPERTY_OCPCONTROL_DEFAULT_CONFIG       "DefOCPConfig"
#define PROPERTY_OCPCONTROL_REG_CONFIG              "OCPConfigReg"
/* END:   Modified by jwx372839, 2017/8/31 9:57:34 */
/* BEGIN: Modified by jwx372839, 2017/10/20 16:15:26   问题单号:DTS2017102312404 */
#define BUSY_NETCARD_METHOD_CLEAR_BWU_WAVE "ClearNetworkBWUWaveRecord"
/* END:   Modified by jwx372839, 2017/10/20 16:15:32 */
#define BUSY_NETCARD_METHOD_ROLLBACK_BWU  "RollbackNetworkBWURecord"

#define CLASS_MEMBOARD_NAME                "MemoryBoard"
#define PROPERTY_MEMBOARD_NAME             "Name"
#define PROPERTY_MEMBOARD_PRESENCE         "Presence"
#define PROPERTY_MEMBOARD_LOGICUNIT        "LogicUnit"
#define PROPERTY_MEMBOARD_LOGICVERID       "LogicVerId"
#define PROPERTY_MEMBOARD_LOGICVER         "LogicVer"
#define PROPERTY_MEMBOARD_PCBID            "PcbId"
#define PROPERTY_MEMBOARD_PCBVER           "PcbVer"
#define PROPERTY_MEMBOARD_BOARDID          "BoardId"
#define PROPERTY_MEMBOARD_LOCALITY         "Locality"
#define PROPERTY_MEMBOARD_TYPE             "Type"
#define PROPERTY_MEMBOARD_MANUFACTURER     "Manufacturer"
#define PROPERTY_MEMBOARD_SERIALNUMBER     "SerialNumber"
#define PROPERTY_MEMBOARD_SLOT             "Slot"
#define PROPERTY_MEMBOARD_BTN_EVENT        "MigButtonEvent"
#define PROPERTY_MEMBOARD_BTN_TEST_MODE    "MigButtonTestMode"
#define PROPERTY_MEMBOARD_CLEAR_BTN_EVENT  "ClearMigButtonEvent"
#define PROPERTY_MEMBOARD_CONFIG_ERROR_CODE        "ConfigErrorCode"

//内存板故障相关属性
#define MEM_BRD_MIGRATION_SUCC             "MigrationSucc"
#define MEM_BRD_MIGRATION_FAIL             "MigrationFail"
#define MEM_BRD_MIGRATION_CTRL             "MigrationCtrl"
#define MEM_BRD_ADD_FAIL                   "InstallFail"
#define MEM_BRD_ONLINE                     "BoardOnline"
#define MEM_BRD_OFFLINE                    "BoardOffline"
#define MEM_BRD_ERR_PREDICTION             "BoardErrPrediction"
#define MEM_BRD_HW_PC_FAULT                "MemPowerChipFail"
#define MEM_BRD_MIRROR_STATUS              "MemBrdMirrorStatus"

#define CLASS_CPUBOARD_NAME                "CpuBoard"
#define PROPERTY_CPUBOARD_NAME             "Name"
#define PROPERTY_CPUBOARD_PRESENCE         "Presence"
#define PROPERTY_CPUBOARD_LOGICUNIT        "LogicUnit"
#define PROPERTY_CPUBOARD_LOGICVERID       "LogicVerId"
#define PROPERTY_CPUBOARD_LOGICVER         "LogicVer"
#define PROPERTY_CPUBOARD_PCBID            "PcbId"
#define PROPERTY_CPUBOARD_PCBVER           "PcbVer"
#define PROPERTY_CPUBOARD_BOARDID          "BoardId"
#define PROPERTY_CPUBOARD_TYPE             "Type"
#define PROPERTY_CPUBOARD_MANUFACTURER     "Manufacturer"
#define PROPERTY_CPUBOARD_SERIALNUMBER     "SerialNumber"
#define PROPERTY_CPUBOARD_SLOT             "Slot"
/* BEGIN: Modified by zwx382233, 2017/8/22 10:31:16   问题单号:DTS2017080401461 */
#define PROPERTY_CPUBOARD_REFCOMPONENT     "RefComponent"
/* END:   Modified by zwx382233, 2017/8/22 10:31:18 */
#define PROPERTY_CPUBOARD_BIOSVER  "IOCtrlFwVer"
//Cpu 板热插拔相关属性
#define CPU_BRD_MIGRATION_CTRL   "MigrationCtrl"
#define CPU_BRD_MIGRATION_W_SIGNAL "MigrationWSignal"
#define CPU_BRD_MIGRATION_R_SIGNAL "MigrationRSignal"
#define METHOD_SET_RASEVENT "RasEvent"


#define CLASS_IOBOARD_NAME                 "IOBoard"
#define PROPERTY_IOBOARD_NAME             "Name"
#define PROPERTY_IOBOARD_PRESENCE         "Presence"
#define PROPERTY_IOBOARD_LOGICUNIT        "LogicUnit"
#define PROPERTY_IOBOARD_LOGICVERID       "LogicVerId"
#define PROPERTY_IOBOARD_LOGICVER         "LogicVer"
#define PROPERTY_IOBOARD_PCBID            "PcbId"
#define PROPERTY_IOBOARD_PCBVER           "PcbVer"
#define PROPERTY_IOBOARD_BOARDID          "BoardId"
#define PROPERTY_IOBOARD_TYPE             "Type"
#define PROPERTY_IOBOARD_MANUFACTURER     "Manufacturer"
#define PROPERTY_IOBOARD_SERIALNUMBER     "SerialNumber"
#define PROPERTY_IOBOARD_SLOT             "Slot"
/* BEGIN: Modified by h00371221, 2017/9/4   问题单号:DTS2017081602270 */
#define PROPERTY_IOBOARD_REFCOMPONENT     "RefComponent"
/* END: Modified by h00371221, 2017/9/4   问题单号:DTS2017081602270 */
#define PROPERTY_IOBOARD_FIRMWARE_VERSION       "FirmwareVersion"
#define PROPERTY_IOBOARD_FLASH_UNIT             "FlashUnit"
#define PROPERTY_IOBOARD_PART_NUM            "PartNum"
#define PROPERTY_IOBOARD_OOB_CHAN            "OobChannel"
#define PROPERTY_IOBOARD_PWR_STATE "PowerState"
#define METHOD_IOBOARD_SET_FIRMWARE_VERSION     "SetIOBoardFW"

#define CLASS_MAINBOARD_NAME              "MainBoard"
#define PROPERTY_MAINBOARD_NAME           "Name"
#define PROPERTY_MAINBOARD_PCHMODEL   "PCHModel"
#define PROPERTY_MAINBOARD_PCBID          "PcbId"
#define PROPERTY_MAINBOARD_PCBVER         "PcbVer"
#define PROPERTY_MAINBOARD_BOARDID        "BoardId"
#define PROPERTY_MAINBOARD_BOMID          "BomId"
#define PROPERTY_MAINBOARD_HEALTH         "Health"
#define PROPERTY_MAINBOARD_PCIEFATALERROR "PCIEFatalError"
#define PROPERTY_MAINBOARD_CHIP_LOCATION  "ChipLocation"
#define PROPERTY_MAINBOARD_VCC_12V      "vcc_12v"
#define METHOD_MAINBOARD_SETPCHMODEL   "SetPchModel"
#define METHOD_MAINBOARD_INIT_BOARD     "InitBoard"
#define METHOD_MAINBOARD_RESTORE_FPGA_CARD          "RestoreFpgaCard"
#define METHOD_MAINBOARD_GET_FPGA_RESTORE_STATUS    "GetRestoreFpgaCardStaus"
#define METHOD_MAINBOARD_CHECK_THIRD_PCIE_RAID_CARD       "CheckThirdPCIeRaidCard"
#define METHOD_CARD_XML_UPGRADE         "UpgradeCardXML"
#define PROPERTY_MAINBOARD_PCIE_SLOT_DISABLED_MASK      "PCIeSlotDisabledMask"



/*Connector*/
#define CLASS_AGENTCONNECTOR_NAME               "AgentConnector"
#define CLASS_CONNECTOR_NAME               "Connector"
#define PROPERTY_CONNECTOR_BOM             "Bom"
#define PROPERTY_CONNECTOR_PRESENT      "Present"
#define PROPERTY_CONNECTOR_TYPE            "Type"
#define PROPERTY_CONNECTOR_SLOT            "Slot"
#define PROPERTY_CONNECTOR_ID              "Id"
#define PROPERTY_CONNECTOR_AUXID           "AuxId"
#define PROPERTY_CONNECTOR_POSITION        "Position"
#define PROPERTY_CONNECTOR_CHIP            "Chip"
#define PROPERTY_CONNECTOR_LEGACY          "Legacy"

#define POWER_SUPPLY_CONNECTOR_TYPE     "PsuConnect"
#define MESH_CARD_CONNECTOR_TYPE           "SWCardConnect"
#define MESH_CARD_CONNECTOR_TYPE_1         "MESHCardConnect"
#define BBU_MODULE_CONNECTOR_TYPE       "BbuConnect"

//内存板故障相关
#define CLASS_ISMEMBOARD_NAME              "IsMemBoard"
#define ISMEMBOARD_OBJECT_NAME             CLASS_ISMEMBOARD_NAME
#define METHOD_ISMEMBOARD_SYNC             "MembrdSyncSetDevStatus"
#define METHOD_ISMEMBOARD_MIG_ENABLE       "MembrdMigrationEnable"
#define METHOD_ISMEMBOARD_MIG_STATUS       "MembrdMigrationStatus"
#define METHOD_ISMEMBOARD_MIG_REFRESH      "MembrdMigrationRefresh"
#define PROPERTY_ISMEMBOARD_MiG_ENABLE     "MigrationEnable"
#define PROPERTY_ISMEMBOARD_SPARE_ID_MASK  "MembrdSpareIdMask"


//散热全局类
#define COOLINGCLASS                          "Cooling"
#define COOLING_PROPERTY_MODE                 "Mode"
#define COOLING_PROPERTY_POWER_MODE           "PowerMode"
#define COOLING_PROPERTY_POWER_MODE_ENABLE    "PowerModeEnable"
#define COOLING_PROPERTY_DFTLEVEL             "MinimalLevel"
#define COOLING_PROPERTY_LIMIT_MAX_LEVEL      "MaxLimitLevel"
#define COOLING_PROPERTY_LEVEL                "Level"
#define COOLING_PROPERTY_TIMEOUT              "TimeOut"
#define COOLING_PROPERTY_TYPE                 "Cooling_fan_type"   //0, 命令0x30 0x93 0x0e的范围时0-100;
#define COOLING_PROPERTY_PRECAPPING_ENABLE    "PowerPreCappingEnable" /* 散热是否支持功耗预封顶 */
#define COOLING_PROPERTY_PRECAPPING_STATE     "PowerPreCappingState"  /* 散热功耗预封顶状态 */
#define COOLING_PROPERTY_PID_MODE_RECG_ENABLE "PidModeRecogniseEnable"   /* PID模块是否打开高温模式识别 */

//1, 命令0x30 0x93 0x0e的范围时0-255;
#define COOLING_PROPERTY_STANDBYFAN           "StandbyFan"         //standby电源下风扇是否运转.
#define COOLING_TYPE                          "CoolingType"               // PID SMM
#define COOLING_POWERSTATE                    "PowerOnState"
#define COOLING_TCONTROL                      "TControl"
#define COOLING_THERMALLEVEL                  "DefThermalLevel"
#define COOLING_OBTAIN_DISKS_TEMP_FAILED    "ObtainDisksTempFailed"
/* BEGIN: Added by gwx455466, 2019/2/16 16:43:26   问题单号:UADP164471 */
#define COOLING_IDENTIFY_SPEED_LEVEL                  "IdentifySpeedLevel"
#define COOLING_IDENTIFIED_FAN_TYPE_INDEX                  "IdentifiedFanTypeIndex"
/* END:   Added by gwx455466, 2019/2/16 16:43:29 */
#define COOLING_EFFECTIVE  "CoolingEffective"
#define COOLING_DFT_FAN_DEFAULT_LEVEL         "DftFanDefaultLevel"
#define FAN_TYPE_IDENTIFIED_WAY               "FanTypeIdentifiedWay"
#define COOLING_FAN_MANUAL_MODE_CONTROL_NODE   "FanManualModeCtrlNode"
#define COOLING_APP_START_INIT                "CoolingStartInit"

#define COOLING_FAULT_FAN_ROTOR_CNT           "FaultRotorNum"
#define COOLING_FAN_INIT_DEFAULT_LEVEL        "FanInitDefaultLevel"     /* AC上电通电开机场景风扇默认速率比 */
#define COOLING_CUR_MAX_SPEED                 "MaxFanSpeed"
#define COOLING_FAN_CTRL_MODE                 "FanCtrlMode"
#define COOLING_FAN_STATUS_SCAN_MODE          "FanStatusScanMode"
#define FAN_INIT_DEFAULT_LEVEL_MIN_VAL        50
#define FAN_INIT_DEFAULT_LEVEL_MAX_VAL        100

#define COOLING_PROPERTY_INITIALLEVEL         "InitialLevel"
#define COOLING_PROPERTY_STOP_FAN_COND        "StopFanCond"              //不为0时风扇停转
#define COOLING_PROPERTY_POWER_PRECAPPING_ENABLE "PowerPreCappingEnable" /* 散热是否支持功耗预封顶 */
#define COOLING_PROPERTY_POWER_PRECAPPING_STATE  "PowerPreCappingState"  /* 散热功耗预封顶状态 */
#define METHOD_COOLING_FAN_STATE              "GetFanState"
#define METHOD_COOLING_SET_BLADE_COOLINGMEDIUM "SetBladeCoolingMedium"
#define METHOD_COOLING_POST_TEMP_ALARM        "PostTempAlarm"
#define METHOD_COOLING_SET_ALL_TEMP_ALARM     "SetAllTempAlarm"
#define METHOD_COOLING_MODE                   "SetCoolingMode"
#define METHOD_COOLING_POWER_MODE             "SetCoolingPowerMode"
#define METHOD_COOLING_LEVEL                  "SetFanLevel"
#define METHOD_SET_INIT_COOLING_LEVEL         "SetInitFanLevel"
#define METHOD_COOLING_GET_CURRENT_PWM        "GetCurrentPWM"
#define METHOD_COOLING_GET_FAN_CTRL_FLAG      "GetFanCtrlFlag"
#define METHOD_COOLING_RECEIVE_PWM_BY_CANBUS  "ReceiveAllSlavePwm"
#define METHOD_COOLING_SINGLE_LEVEL           "SetSingleFanLevel"
#define METHOD_COOLING_GET_MANUAL_LEVEL       "GetManualFanLevel"
#define METHOD_COOLING_PROPERTY_SYNC          "PropertySyncMethod"
#define METHOD_COOLING_MODE_PROPERTY_SYNC     "ModePropertySyncMethod"
#define METHOD_COOLING_READ_ELABLE            "ReadFanElable"
#define METHOD_COOLING_WRITE_ELABLE           "WriteFanElable"
#define METHOD_COOLING_SET_PRE_PSC_STATE      "SetPowerPreCappingState"
#define METHOD_COOLING_PROPERTY_SAVED_SYNC    "PropertySavedSyncMethod"
#define METHOD_COOLING_SET_FAN_ROUTINE_STOP   "SetFanRoutineStop"
#define METHOD_SMMFAN_SET_FAN_SPEED           "SetFanSpeed"

#define METHOD_COOLING_GETCPU_THERMALINFO     "GetCpuThermalInfo"
#define METHOD_COOLING_CLEAR_BLADE            "SetThermalSrcState"
#define COOLING_PROPERTY_MAX_SUPPORT_PWM      "MaxSupportPWM"
/*BEGIN:Added by ywx383385 2017/03/07 for redfish C50*/
#define METHOD_COOLING_GET_FAN_POWER          "GetTotolFanPower"            //风扇总功率
/*BEGIN:Added by ywx383385 2017/03/07 for redfish C50*/

/* BEGIN: Added by linzhen, 2017/07/13 21:37   问题单号:iBMC AR-0000276585-013-002 支持液冷/风冷配置         */
#define COOLING_PROPERTY_MEDIUM               "Medium"                      //冷媒
#define METHOD_COOLING_SET_MEDIUM             "SetCoolingMedium"            //set medium method
/* END:   Added by linzhen, 2017/07/13 21:37 */

/** BEGIN: Added by cwx290152, 2017/11/14   PN:AR.SR.SFEA02109379.002.001 支持机框风扇分区调速 */
#define COOLING_PROPERTY_PARTCOOLING_ENABLE   "ChassisPartCoolingEnable"
/** END:   Added by cwx290152, 2017/11/14 */

#define COOLING_PROPERTY_SMARTCOOLINGMODE                       "SmartCoolingMode"  /* Added by d00384307, 2018/5/11 10:32:17 */
#define COOLING_PROPERTY_SMARTCOOLINGENABLE                     "SmartCoolingEnable"    // 是否支持SmartCooling功能 0:不支持 1:支持
#define COOLING_PROPERTY_CUSTOMCPU_COREMTOBJ                    "CustomCpuCoremTobj"    // CPU目标温度
#define COOLING_PROPERTY_CPUCOREMTOBJ_TJMAXDIFF                 "CpuCoremTobjTjmaxDiff" // CPU目标温度所能设置的最大温度边界值为Tjmax-11，属性值为11，可能会有单板差异
#define METHOD_SMART_COOLING_MODE                               "SetSmartCoolingMode"   // 设置smart cooling调速模式
#define METHOD_COOLING_SET_CPU_TOBJ                             "SetCpuTobj"            // 设置目标温度
#define COOLING_PROPERTY_CUSTOMOUTLET_TOBJ                      "CustomOutletTobj"      // CPU目标温度
#define COOLING_PROPERTY_CUSTOMSETTING_MASK                     "CustomSettingMask"
#define COOLING_PROPERTY_CUSTOM_CPU_RANGE_MAX                   "CustomCpuCoremTobjRangeMax"
#define COOLING_PROPERTY_CUSTOM_CPU_RANGE_MIN                   "CustomCpuCoremTobjRangeMin"
#define COOLING_PROPERTY_CUSTOM_OUTLET_RANGE                    "CustomOutletTobjRange"
#define COOLING_PROPERTY_CUSTOM_INLET_SPEED_RANGE               "CustomInletSpeedRange"
#define COOLING_PROPERTY_CUSTOM_DISK_SPEED_RANGE                "CustomDiskSpeedRange"
#define COOLING_PROPERTY_CUSTOMDISK_TOBJ                        "CustomDiskTobj"
#define COOLING_PROPERTY_CUSTOM_DISK_RANGE                      "CustomDiskTobjRange"
#define COOLING_PROPERTY_CUSTOMMEMORY_TOBJ                      "CustomMemoryTobj"
#define COOLING_PROPERTY_CUSTOM_MEMORY_RANGE                    "CustomMemoryTobjRange"
#define COOLING_PROPERTY_CUSTOMPCH_TOBJ                         "CustomPCHTobj"
#define COOLING_PROPERTY_CUSTOM_PCH_RANGE                       "CustomPCHTobjRange"
#define COOLING_PROPERTY_CUSTOMVRD_TOBJ                         "CustomVRDTobj"
#define COOLING_PROPERTY_CUSTOM_VRD_RANGE                       "CustomVRDTobjRange"
#define COOLING_PROPERTY_CUSTOMVDDQ_TOBJ                        "CustomVDDQTobj"
#define COOLING_PROPERTY_CUSTOM_VDDQ_RANGE                      "CustomVDDQTobjRange"
#define COOLING_PROPERTY_CUSTOMNPU_HBM_TOBJ                     "CustomNpuHbmTobj"
#define COOLING_PROPERTY_CUSTOMNPU_HBM_RANGE                    "CustomNpuHbmTobjRange"
#define COOLING_PROPERTY_CUSTOMNPU_AICORE_TOBJ                  "CustomNpuAiCoreTobj"
#define COOLING_PROPERTY_CUSTOMNPU_AICORE_RANGE                 "CustomNpuAiCoreTobjRange"
#define COOLING_PROPERTY_CUSTOMNPU_BOARD_TOBJ                   "CustomNpuBoardTobj"
#define COOLING_PROPERTY_CUSTOMNPU_BOARD_RANGE                  "CustomNpuBoardTobjRange"
#define COOLING_PROPERTY_CUSTOMSOCBRD_INLET_TOBJ                   "CustomSoCBoardInletTobj"
#define COOLING_PROPERTY_CUSTOMSOCBRD_INLET_RANGE                  "CustomSoCBoardInletTobjRange"
#define COOLING_PROPERTY_CUSTOMSOCBRD_OUTLET_TOBJ                  "CustomSoCBoardOutletTobj"
#define COOLING_PROPERTY_CUSTOMSOCBRD_OUTLET_RANGE                 "CustomSoCBoardOutletTobjRange"
#define COOLING_PROPERTY_POLICYCLASS_TMAX_TOLERANCE                "PolicyClassTmaxTolerance"

#define METHOD_COOLING_SET_OUTTEMP_TOBJ                         "SetOutletTobj"
#define METHOD_COOLING_SET_DISKTEMP_TOBJ                        "SetDiskTobj"
#define METHOD_COOLING_SET_MEMORYTEMP_TOBJ                      "SetMemoryTobj"
#define METHOD_COOLING_SET_PCHTEMP_TOBJ                         "SetPCHTobj"
#define METHOD_COOLING_SET_VRDTEMP_TOBJ                         "SetVRDTobj"
#define METHOD_COOLING_SET_VDDQTEMP_TOBJ                        "SetVDDQTobj"
#define METHOD_COOLING_SET_NPU_HBMTEMP_TOBJ                     "SetNPUHbmTobj"
#define METHOD_COOLING_SET_NPU_AICORETEMP_TOBJ                  "SetNPUAiCoreTobj"
#define METHOD_COOLING_SET_NPU_BOARDTEMP_TOBJ                   "SetNPUBoardTobj"
#define METHOD_COOLING_SET_SOCBRD_INTEMP_TOBJ                      "SetSoCBoardInletTobj"
#define METHOD_COOLING_SET_SOCBRD_OUTTEMP_TOBJ                     "SetSoCBoardOutletTobj"

/* BEGIN: Added by zengrenchang, 2018/01/11 问题单号:AR.SR.SFEA02109379.009.001  OSCA V5删除PID调速         */
#define COOLING_PROPERTY_RMCUNSUPPORTPID                     "RMCUnSupportPid"  //远端是否支持PID，1不支持,0为默认值表示支持或者不涉及PID调速
/* END:   Added by zengrenchang, 2018/01/11 */

/* BEGIN: Added by c00416525, 2018/6/14 */
#define METHOD_SET_FAN_UPGRADE_STATE                           "SetFanUpgradeState"
/* BEGIN: Added by c00416525, 2018/6/14 */

#define METHOD_COOLING_SET_LEAK_POLICY "SetLeakageAction"

#define  COOLING_PROPERTY_FAN_POWER_CONSUMPTION       "FansPowerConsumption"           /* 风扇当前总功耗 */
#define PROPERTY_COOLING_LEAK_POLICY             "Policy"

#define CLASS_FAN_SPEED_LIMIT               "FanSpeedLimit"
#define PROPERTY_FAN_SPEED_MAX_LIMIT_LEVEL  "MaxLimitLevel"
#define PROPERTY_FAN_SPEED_LIMIT_IS_VALID   "IsValid"

/* 散热组件类 */
#define CLASS_COOLINGUNIT_NAME             "CoolingUnit"
#define PROPERTY_COOLINGUNIT_NAME          "Name"
#define PROPERTY_COOLINGUNIT_SLOT          "Slot"
#define PROPERTY_COOLINGUNIT_POWER         "Power"

//风扇板版本信息
#define CLASS_FANBOARD_NAME                "FANBoard"
#define PROPERTY_FANBOARD_NAME             "Name"
#define PROPERTY_FANBOARD_PRESENCE         "Presence"
#define PROPERTY_FANBOARD_LOGICUNIT        "LogicUnit"
#define PROPERTY_FANBOARD_LOGICVERID       "LogicVerId"
#define PROPERTY_FANBOARD_LOGICVER         "LogicVer"
#define PROPERTY_FANBOARD_PCBID            "PcbId"
#define PROPERTY_FANBOARD_PCBVER           "PcbVer"
#define PROPERTY_FANBOARD_BOARDID          "BoardId"
#define PROPERTY_FANBOARD_TYPE             "Type"
#define PROPERTY_FANBOARD_MCUTYPE          "MCUType"
#define PROPERTY_FANBOARD_MANUFACTURER     "Manufacturer"
#define PROPERTY_FANBOARD_SERIALNUMBER     "SerialNumber"
#define PROPERTY_FANBOARD_SLOT             "Slot"
#define PROPERTY_FANBOARD_FW_TYPE          "FWType"
#define METHOD_RELOAD_FAN_INFO             "ReloadFanInfo"
#define PROPERTY_FANBOARD_LED              "Led"
#define PROPERTY_FANBOARD_MCU_OPTION       "MCUOption"
#define PROPERTY_FANBOARD_LOG              "LogData"
#define PROPERTY_FANBOARD_TIME             "SyncTime"
#define PROPERTY_FANBOARD_TEMP_STATUS      "TempSensorStatus"
#define PROPERTY_FANBOARD_SYNC_TEMP_STATUS "SyncTempSensorStatus"
#define METHOD_OPERATION_FANBOARD_LED      "SetLed"
#define METHOD_SYNC_FANBOARD_INFO          "SyncFanBoardInfoMethod"     /* 同步风扇板信息 */
#define METHOD_FAN_EXPBOARD_RELOAD_INFO     "FANExpReloadInfo"

//风扇类型类定义
#define FANTYPECLASS                           "CLASSFANTYPE"
#define FANTYPE_INDEX                          "Index"
#define FANTYPE_CHARACTOR_NAME                 "Name"
#define FANTYPE_CHARACTOR_SPEEDRANGE           "SpeedAtHarfPWM"
#define FANTYPE_CHARACTOR_TWINS                "Twins"
#define FANTYPE_CHARACTOR_MAINBOARDID          "MainBoardID"
#define FANTYPE_CHARACTOR_EXCLUDECPUTYPE       "ExcludeCpuType"
#define FANTYPE_CHARACTOR_EXCLUDEPCIETYPE      "ExcludePCIEType"
#define FANTYPE_DEFAULT_TYPE                   "DefaultType"
#define FANTYPE_F_SPEED                        "Fspeed"
#define FANTYPE_R_SPEED                        "Rspeed"
#define FANTYPE_POWER_SPEED_RANGE              "SpeedRange"
#define FANTYPE_POWER_POWER_RANGE              "PowerRange"
/* BEGIN: Added by gwx455466, 2017/7/5 15:42:2   问题单号:AR-0000276588-006-001 */
#define PROPERTY_FANTYPE_PART_NUM                    "PartNum"
/* END:   Added by gwx455466, 2017/7/5 15:42:6 */

//风扇类定义
#define SHADOWFANCLASS                         "AgentFANClass"
#define FANCLASS                               "FANClass"
#define FAN_INDEX                              "FANID"
#define FAN_SLOT                               "FANSlot"
#define FAN_PROPERTY_ENTITY                    "FanEntity"
#define FAN_PROPERTY_HEALTH                    "FanHealth"
#define FAN_PROPERTY_FSPEED                    "FSpeed"
#define FAN_PROPERTY_RSPEED                    "RSpeed"
#define FAN_PROPERTY_FRPM                      "FRPM"
#define FAN_PROPERTY_RRPM                      "RRPM"
#define FAN_F_PROPERTY_STATUS                  "FSTATUS"
#define FAN_R_PROPERTY_STATUS                  "RSTATUS"
#define FAN_PROPERTY_TYPE                      "TYPE"
#define FAN_PROPERTY_PWM_APP                   "PWM"  //WEB显示的值，0-100
#define FAN_PROPERTY_PWM                       "PWMHW"    //设置给硬件的值 0-1000
/* BEGIN: Added by d00381887, 2016/11/4   问题单号: DTS2017092310821 一个风扇转速异常，其他风扇转速未达到80% */
#define FAN_PROPERTY_EXPECT_PWM                "ExpectPWM"    // 预期硬件的转速 0-1000
/* END: Added by d00381887, 2016/11/4   问题单号: DTS2017092310821 一个风扇转速异常，其他风扇转速未达到80% */
#define FAN_PROPERTY_PRESENT                   "Presence"
#define FAN_R_PROPERTY_PRESENT                 "PresenceR"
#define FAN_PROPERTY_SPEED_COEFFICIENT         "Coefficient"
#define FAN_PROPERTY_LOCATION                  "Location"
#define FAN_PROPERTY_FUNCTION                    "Function"
#define FAN_PROPERTY_DEVICENAME                  "DeviceName"
#define FAN_PROPERTY_MODEL                       "Model"
#define FAN_PROPERTY_LOCALITY                  "Locality"
#define FAN_PROPERTY_RIMM_OBJ                  "RimmObj"               /* 标记FANClass关联的真实对象*/
#define FAN_PROPERTY_SYNC_FSPEED               "SyncFSpeed" /* 存储风扇的前风扇同步转速 */
#define FAN_PROPERTY_SYNC_RSPEED               "SyncRSpeed" /* 存储风扇的后风扇同步转速 */
#define FAN_PROPERTY_SYNC_FSTATUS              "SyncFStatus" /* 存储风扇的前转子同步状态 */
#define FAN_PROPERTY_SYNC_RSTATUS              "SyncRStatus" /* 存储风扇的后转子同步状态 */
#define FAN_PROPERTY_FAN_ALARM_LED             "FanAlarmLed"   /* 设置硬件点风扇告警灯寄存器 */
#define FAN_PROPERTY_MAX_LIMIT_LEVEL           "MaxLimitLevel"   /* 风扇当前允许的最大转速 */

/** BEGIN: Added by cwx290152, 2017/11/14   PN:AR.SR.SFEA02109379.002.001支持机框分区调速 */
#define FAN_PROPERTY_INCLUDE_BLADES            "IncludeBlades"
/** END:   Added by cwx290152, 2017/11/14 */
#define METHOD_COOLING_SYNC_FAN_INFO           "SyncFanInfoMethod"     /* 同步风扇相关信息 */
#define METHOD_COMM_FRU_SET_ROUTINE             "SetRoutineMethod"     /* 设置Routine */
#define METHOD_COMM_FRU_SYNC_I2C_STATUS         "SyncCommFruI2cStatusMethod"  /* 同步风扇i2c状态 */

/* BEGIN: Added by gwx455466, 2017/7/5 15:42:2   问题单号:AR-0000276588-006-001 */
#define FAN_PROPERTY_PART_NUM                    "PartNum"
/* END:   Added by gwx455466, 2017/7/5 15:42:6 */

/* 风扇电源操作类定义 */
#define CLASS_FAN_POWER_CTRL                   "FanPowerCtrl"
#define FAN_POWER_PROPERTY_POWER_SWITCH        "FanPowerSwitch"            /* 风扇开关切换 */
#define FAN_POWER_PROPERTY_POWER_SWITCH_MODE   "FanPowerSwitchMode"        /* 风扇开关切换模式 */
#define FAN_POWER_PROPERTY_PROTECT_MODE        "FanPowerProtectMode"       /* 存储风扇写保护模式 */
#define FAN_POWER_PROPERTY_PROTECT_SWITCH      "FanPowerProtectSwitch"     /* 存储风扇写保护切换 */
#define METHOD_OPERATION_SET_FAN_POWER         "SetFanPower"
#define FAN_POWER_PROPERTY_FANID               "FANID"
#define FAN_POWER_PROPERTY_POWER_TYPE          "PowerType"
#define FAN_POWER_PROPERTY_POWER_ON_TRIGGER    "PowerOnTrigger"
#define FAN_POWER_PROPERTY_SWITCH_STATUS       "PowerSwitchStatus"

/*BEGIN:Added by h00164462, 2015/02/02, PN:PTAS*/
//风扇分区类定义
#define CLASS_FAN_GROUP                        "FanGroup"
#define PROPERTY_FANGROUP_ID                   "ID"
#define PROPERTY_FANGROUP_FANOBJ               "FanObj"

#define CLASS_PTAS                             "PTAS"
#define PROPERTY_PTAS_ALTITUDE                 "Altitude"
#define PROPERTY_PTAS_SERVERTAG                "ServerTag"
#define METHOD_SET_SERVERTAG                   "SetServerTag"
/* BEGIN: Added by DTS2015032405413, 2015/5/9 SN:DTS2015040807396  */
#define METHOD_SET_ALTITUDE_LEVEL                  "SetAltitudeLevel"
#define METHOD_SET_ENABLE                   "SetEnable"
/* END: Added by DTS2015032405413, 2015/5/9 SN:DTS2015040807396  */

#define CLASS_PTAS_POLICY                      "PTASPolicy"
#define PROPERTY_PTAS_POLICY_COEFFICIENT       "CoolingCoefficient"
/*END:Added by h00164462, 2015/02/02, PN:PTAS*/

//机架的散热配置策略.
#define POLICY1_CLASS                          "Policy1Class"
#define POLICY_PROPERTY_INDEX                  "Index"
#define POLICY_PROPERTY_TYPE                   "Type"
#define POLICY_PROPERTY_SENSORNAME             "SensorName"
#define POLICY_PROPERTY_TMAX                   "Tmax"
#define POLICY_PROPERTY_TOBJ                   "Tobj"
#define POLICY_PROPERTY_HSIS                   "Hsis"
#define POLICY_PROPERTY_FAN_LIST               "FAN"
#define POLICY_PROPERTY_ALM_SPD                "AlarmSpeed"
#define POLICY_PROPERTY_ENV_RANGE_READING_LOW  "EnvRangeRdL"
#define POLICY_PROPERTY_ENV_RANGE_READING_HIGH "EnvRangeRdH"
#define POLICY_PROPERTY_ENV_RANGE_SPEED_LOW    "EnvRangeSpdL"
#define POLICY_PROPERTY_ENV_RANGE_SPEED_HIGH   "EnvRangeSpdH"
#define POLICY_PROPERTY_CONDITION_INDEX        "ConditionIndex"
#define POLICY_PROPERTY_FAILED_VALUE           "FailedValue"                //从硬件读取失败，使用的读数
#define POLICY_PROPERTY_DISABLED_VALUE         "DisabledValue"              //硬件禁止扫描，使用的默认读数
#define METHOD_COOLING_SET_ENVTEMPSPD          "SetEnvTempSpd"  // 设置环境温度区间
#define POLICY_PROPERTY_ISVALID                "IsValid"        // 策略是否生效属性
#define POLICY_PROPERTY_SUPPORT_STANDBY        "IsSupportStandby"

// MPC配置
#define MPCCONFIGCLASS                          "MpcConfig"
#define MPCCONFIG_PROPERTY_TOTALSTEPS           "TotalSteps"
#define MPCCONFIG_PROPERTY_MAXCPUTEMP           "MaxCpuTemp"
#define MPCCONFIG_PROPERTY_CPUEDGETEMP          "CpuEdgeTemp"
#define MPCCONFIG_PROPERTY_CPUFREQUENCY         "CpuFrequency"
#define MPCCONFIG_PROPERTY_ENABLED              "Enabled"
#define MPCCONFIG_PROPERTY_DISKNUM              "DiskNum"
#define MPCCONFIG_PROPERTY_NETCARDNUM           "NetCardNum"
#define MPCCONFIG_PROPERTY_RETRAIN              "Retrain"
#define MPCCONFIG_PROPERTY_STABLEPOWER          "StableTotalPower"
#define MPCCONFIG_PROPERTY_POWERDIFF            "PowerDiff"
#define MPCCONFIG_PROPERTY_VERSION              "Version"
#define MPCCONFIG_PROPERTY_RELATEDSENSORS       "RelatedSensors"
#define MPCCONFIG_PROPERTY_MINPWMVALUE          "MinPWMValue"

// MPC模型
#define MPCMODELCLASS                           "MpcModel"
#define MPCMODEL_PROPERTY_PARAMETERS            "Parameters"
#define MPCMODEL_PROPERTY_TYPE                  "type"
#define MPCMODEL_PROPERTY_ENVTEMP               "EnvTemp"

//散热条件
#define CONDITIONCLASS                         "CoolingCondition"
#define CONDITION_PROPERTY_INDEX               "Index"
#define CONDITION_PROPERTY_TYPE                "Type"
#define CONDITION_PROPERTY_ENABLE              "Enable"
#define CONDITION_PROPERTY_PARAMETER           "Parameter"
#define CONDITION_PROPERTY_REFERENCETYPE       "RefType"
/* BEGIN: Added by r00355870, 2017/02/13 */
#define CONDITION_PROPERTY_VALUE               "Value"
/* END:   Added by r00355870, 2017/02/13 PN:  DTS2017011809355*/

//条件组定义；一个条件组由一个或多个条件关系组成，与 或者 或 关系
#define CONDITION_GROUPCLASS                   "ConditionGroup"
#define CONDITION_GROUP_INDEX                  "Index"
#define CONDITION_GROUP_ISTRUE                 "IsTrue"
#define CONDITION_GROUP_COMBO                  "ComboType"
#define CONDITION_GROUP_REFTYPE                "RefType"
#define CONDITION_GROUP_MEMBERINDEX            "ConditionIndex"

//cpu 功耗 和 目标温度
#define TEMPPOWMAPCLASS                       "CpuPowTempMap"
#define MAP_PROPERTY_SENSORNAME               "SensorName"
#define MAP_PROPERTY_POW_RANGE_LOW            "PowRangeL"
#define MAP_PROPERTY_POW_RANGE_HIGH           "PowRangeH"
#define MAP_PROPERTY_OBJ_TEMP                 "ObjTemp"
#define MAP_PROPERTY_OBJ_TEMP_VAR             "ObjTempVar"
#define MAP_PROPERTY_OBJ_TEMP_FIX             "ObjTempFix"
#define MAP_PROPERTY_CONDITION_INDEX          "ConditionIndex"
#define MAP_PROPERTY_HSIS                     "Hsis"
#define MAP_PROPERTY_REF_POW                  "RefPow"

//用于可变风扇数量支持
#define NEEDFAN                               "AdditionalFan"

/* BEGIN: Modified by wangpenghui wwx382232, 2019/5/16 10:32:4   问题单号:DTS2018062010020 */
#define CLASS_NODE_ETH_STATUS                   "NodeEthStatus"
#define NODE_ETH_ATTRIBUTE_LINK_STATUS             "PortLinkStatus"
#define NODE_ETH_METHOD_SET_PORT_LINK_STATUS    "SetPortLinkStatus"

#define CLASS_LANSWITCH_PORT_STATUS                       "LANSwitchPortStatus"
#define LANSWITCH_PORT_ATTRIBUTE_LANSWITCH_ID                   "LANSwiticID"
#define LANSWITCH_PORT_ATTRIBUTE_PORT_ID                "PortID"
#define LANSWITCH_PORT_ATTRIBUTE_LINKSTATUS                  "LinkStatus"
/* END:   Modified by wangpenghui wwx382232, 2019/5/16 10:32:7 */

/*Begin: NETWORK 模块依赖的类和对象 */
/* eth类定义*/
#define ETH_CLASS_NAME_ETH                  "Eth"
#define ETH_ATTRIBUTE_MAC                   "Mac"
#define ETH_ATTRIBUTE_ENABLE                "EthEnable"
#define ETH_ATTRIBUTE_NAME                  "Name"
#define ETH_ATTRIBUTE_TYPE                  "Type"
/* BEGIN: Added by zengrenchang(z00296903), 2016/12/6   问题单号:DTS2016120900218*/
#define ETH_ATTRIBUTE_NCSI_NIC_SWITCH_TYPE      "NcsiNicSwitchType"
#define ETH_ATTRIBUTE_NCSI_NIC_SWITCH_ACCESSOR  "NcsiNicSwitchAccessor"
/* END:   Added by zengrenchang(z00296903), 2016/12/6 */
#define ETH_ATTRIBUTE_OUT_TYPE              "OutType"
#define ETH_ATTRIBUTE_NUM                   "Num"
#define ETH_ATTRIBUTE_NCSI_PORT             "NCSIPort"
#define ETH_ATTRIBUTE_VLAN_ID               "VlanID"
#define ETH_ATTRIBUTE_VLAN_STATE            "VlanState"
#define ETH_ATTRIBUTE_GROUP_ID              "GroupID"
#define ETH_ATTRIBUTE_LINK_STATUS           "EthLinkStatus"
#define ETH_ATTRIBUTE_ADAPTIVE_FLAG         "AdaptiveFlag"
#define ETH_ATTRIBUTE_ADAPTIVE_PORT_MASK    "AdaptivePortMask"
#define ETH_ATTRIBUTE_PRIORITY              "Priority"
#define ETH_ATTRIBUTE_SPEED_DUPLEX_DISABLE  "SpeedDuplexEnable"
#define ETH_ATTRIBUTE_SPEED_SETTING         "SpeedSetting"
#define ETH_ATTRIBUTE_DUPLEX_SETTING        "DuplexSetting"
#define ETH_ATTRIBUTE_ETH_UID        "UID"
#define ETH_ATTRIBUTE_ETH_ID         "id"

#define CLASS_NAME_PHYCLOCK_CONF             "PhyClockConf"
#define PHYCLOCK_CONF_RMII_CLK_CONF         "RmiiClkConf"
#define PHYCLOCK_CONF_RMII_CLK_EDGE_SEL      "RmiiClkEdgeSel"
#define PHYCLOCK_CONF_RMII_DRI_POWER         "RmiiDriPower"
#define PHYCLOCK_CONF_ETH_NUM                "EthNum"

#define ETH_METHOD_SET_MAC                  "SetMac"
#define ETH_METHOD_SET_VLAN_ID              "SetVlanID"
#define ETH_METHOD_SET_ETH_ENABLE           "SetEthEnable"
#define  ETH_METHOD_ADD_IPTABLE              "AddIptablesRule"
/* BEGIN: 2018-11-27 z00416979 Ipv6 ip6tables 配置 DTS2018112611095 */
#define  ETH_METHOD_ADD_IP6TABLE              "AddIp6tablesRule"
/* END:   2018-11-27 z00416979 Ipv6 ip6tables 配置 DTS2018112611095 */
#define METHOD_ETHERNETPORT_SIDEBANDPORT                   "EnablePortSet"
/* BEGIN: Added by luchao 0454427, 2018/8/23   PN: NA*/
#define ETH_METHOD_DFT_LOOP_CHECK       "DftEthLoopCheck"
/* END:   Added by luchao 0454427, 2018/8/23 */


/* eth group类定义*/
#define ETH_CLASS_NAME_ETHGROUP             "EthGroup"
#define OUT_ETHGROUP_OBJ_NAME               "EthGroup0"
#define INNER_ETHGROUP_OBJ_NAME             "EthGroup1"
/* BEGIN: Modified by gwx455466, 2017/9/21 17:38:59   问题单号:AREA02109778-004-003 */
#define VETH_ETHGROUP_OBJ_NAME             "EthGroup2"
/* END:   Modified by gwx455466, 2017/9/21 17:39:3 */
#define FLOAT_ETHGROUP_OBJ_NAME             "EthGroup3"
#define MAINTENANCE_ETH_GROUP_OBJ_NAME      "EthGroup8"    /* 近端维护网口 */

#define ETH_GROUP_ATTRIBUTE_IP_VERSION        "IpVersion"
#define ETH_GROUP_ATTRIBUTE_GROUP_ID        "GroupID"
#define ETH_GROUP_ATTRIBUTE_NET_MODE        "NetMode"
#define ETH_GROUP_ATTRIBUTE_NCSI_MODE       "NcsiMode"
#define ETH_GROUP_ATTRIBUTE_NET_MODE_FLAG   "NetModeFlag"
#define ETH_GROUP_ATTRIBUTE_MAC_SOURCE_PORT "MacSourcePort"
#define ETH_GROUP_ATTRIBUTE_ACTIVE_PORT     "ActivePort"
#define ETH_GROUP_ATTRIBUTE_SET_IN_PROGRESS "Status"
#define ETH_GROUP_ATTRIBUTE_TYPE            "Type"
#define ETH_GROUP_ATTRIBUTE_OUT_TYPE        "OutType"
#define ETH_GROUP_ATTRIBUTE_NCSI_PORT       "NCSIPort"
#define ETH_GROUP_ATTRIBUTE_IP_SET_FLAG     "IpSetFlag"
#define ETH_GROUP_ATTRIBUTE_CHANNEL         "Channel"
#define ETH_GROUP_ATTRIBUTE_IP_MODE         "IpMode"
/* BEGIN: Added by zwx559561, 2018/12/19    */
#define ETH_GROUP_ATTRIBUTE_D_IP_MODE         "d:IpMode"
#define ETH_GROUP_ATTRIBUTE_FACTORY_D_IP_MODE       "FactoryDefaultIpMode"
#define ETH_GROUP_ATTRIBUTE_FACTORY_D_IP_ADDR       "FactoryDefaultIpAddr"
/* END:   Modified by zwx559561, 2018/12/19 */
#define ETH_GROUP_ATTRIBUTE_IP_ADDR         "IpAddr"
#define ETH_GROUP_ATTRIBUTE_BASE_IP_ADDR    "BaseIpAddr"
#define ETH_GROUP_ATTRIBUTE_D_IP_ADDR       "d:IpAddr"
#define ETH_GROUP_ATTRIBUTE_SUB_MASK        "SubnetMask"
#define ETH_GROUP_ATTRIBUTE_BASE_SUB_MASK   "BaseSubnetMask"
#define ETH_GROUP_ATTRIBUTE_GATEWAY         "DefaultGateway"
#define ETH_GROUP_ATTRIBUTE_IPV6_MODE       "Ipv6Mode"
#define ETH_GROUP_ATTRIBUTE_IPV6_ADDR       "Ipv6Addr"
#define ETH_GROUP_ATTRIBUTE_IPV6_PREFIX     "Ipv6Prefix"
#define ETH_GROUP_ATTRIBUTE_SLAAC_IPV6_ADDR       "SLAACIpv6Addr"
#define ETH_GROUP_ATTRIBUTE_SLAAC_IPV6_PREFIX     "SLAACIpv6Prefix"
#define ETH_GROUP_ATTRIBUTE_IPV6_GATEWAY    "Ipv6DefaultGateway"
#define ETH_GROUP_ATTRIBUTE_IPV6_LOCAL_LINK "Ipv6LocalLink"
#define ETH_GROUP_ATTRIBUTE_INNER_NETWORK   "InnerNetWork"
#define ETH_GROUP_ATTRIBUTE_IPPOLICY        "IpPolicy"
#define ETH_GROUP_ATTRIBUTE_MAC             "Mac"
#define ETH_GROUP_ATTRIBUTE_VLAN_ID         "VlanID"
#define ETH_GROUP_ATTRIBUTE_VLAN_STATE      "VlanState"
#define ETH_GROUP_ATTRIBUTE_VLAN_NUM        "VlanNum"
#define ETH_GROUP_ATTRIBUTE_VLAN_MIN        "VlanMin"
#define ETH_GROUP_ATTRIBUTE_VLAN_MAX        "VlanMax"
#define ETH_GROUP_ATTRIBUTE_RMCP_LEVEL      "Level"
#define ETH_GROUP_ATTRIBUTE_BACKUP_IP_ADDR      "BackupIpAddr"
#define ETH_GROUP_ATTRIBUTE_BACKUP_SUB_MASK     "BackupSubnetMask"
#define ETH_GROUP_ATTRIBUTE_ALIAS                  "EthAlias"
#define ETH_GROUP_METHOD_SET_BACKUP_IP_MAKS         "SetBackupIpMask"
#define ETH_GROUP_METHOD_VIRTUAL_NETWORK_STATUS         "VirtualNetworkStatus"

#define ETH_GROUP_METHOD_SET_NET_MODE                 "SetNetMode"
#define ETH_GROUP_METHOD_SET_NCSI_MODE            "SetNcsiMode"
#define ETH_GROUP_METHOD_SET_IPV4_MODE                "SetIpMode"
#define ETH_GROUP_METHOD_SET_IPV4_ADDR                "SetIpAddr"
#define ETH_GROUP_METHOD_SET_IPV4_MASK                "SetSubnetMask"
#define ETH_GROUP_METHOD_SET_BACKUP_IPV4_ADDR         "SetBackupIpAddr"
#define ETH_GROUP_METHOD_SET_BACKUP_IPV4_MASK         "SetBackupSubnetMask"
#define ETH_GROUP_METHOD_SET_IPV4_DEFAULT_GATEWAY     "SetDefaultGateway"
#define ETH_GROUP_METHOD_SET_IPV4_MASK_GATEWAY        "SetIpMaskGateway"
#define ETH_GROUP_METHOD_SET_IPV6_MODE                "SetIpv6Mode"
#define ETH_GROUP_METHOD_SET_IPV6_ADDR                "SetIpv6Addr"
#define ETH_GROUP_METHOD_SET_IPV6_PREFIX              "SetIpv6Prefix"
#define ETH_GROUP_METHOD_SET_IPV6_DEFAULT_GATEWAY     "SetIpv6DefaultGateway"
#define ETH_GROUP_METHOD_SET_IPV6_IP_PREFIX_GATEWAY   "SetIpv6IpFixGateway"
#define ETH_GROUP_METHOD_SET_STATUS                   "SetStatus"
#define ETH_GROUP_METHOD_SET_VLAN_ID                  "SetVlanID"
#define ETH_GROUP_METHOD_SET_VLAN_STATE               "SetVlanState"
#define ETH_GROUP_METHOD_SET_NCSI_PORT                "SetNCSIPort"
#define ETH_GROUP_METHOD_SET_ACTIVE_PORT              "SetActivePort"
#define ETH_GROUP_METHOD_SMM_PORT_SWITCH              "SmmPortSwitch"
#define ETH_GROUP_METHOD_CHOOSE_ADAPTIVE_ETH          "ChooseAdaptiveEth"
#define ETH_GROUP_METHOD_SET_DEFAULT                  "SetDefault"
#define ETH_GROUP_METHOD_SET_IP_VERSION               "SetIpVersion"
#define ETH_GROUP_METHOD_FLOAT_IP_SYNC                "FloatIpSync"
#define ETH_GROUP_METHOD_DEFAULT_GATEWAY_SYNC         "DefaultGatewaySync"
#define ETH_GROUP_METHOD_SET_FACTORY_DEFAULT          "SetFactoryDefaultIpInfo"

/* BEGIN: Added by gwx455466, 2017/3/18 20:46:42   问题单号:AR-0000276593-005-003 */
#define VETH_ETH_OBJ_NAME             "veth"
/* END:   Added by gwx455466, 2017/3/18 20:46:45 */
/* BusinessPort类定义*/
#define ETH_CLASS_NAME_BUSY_ETH             "BusinessPort"
/* BEGIN:  ncsi over mctp add by tangjie*/
#define BUSY_ETH_MCTP_MAC                   "MacAddrFromMctp"
#define BUSY_ETH_MCTP_LINK_STATUS           "LinkStatusFromMctp"
#define BUSY_ETH_MCTP_SPEED                 "SpeedFromMctp"
#define BUSY_ETH_MCTP_DUPLEX_FLAG           "DuplexFlagFromMctp"
#define BUSY_ETH_MCTP_NEGOTIATE_FLAG        "NegotiateFlagFromMctp"

#define BUSY_ETH_MCTP_TOTAL_BYTES_RX        "TotalBytesReceived"
#define BUSY_ETH_MCTP_TOTAL_BYTES_TX        "TotalBytesTransmitted"
#define BUSY_ETH_MCTP_TOTAL_UNI_PKT_RX      "UnicastPacketsReceived"
#define BUSY_ETH_MCTP_TOTAL_MUL_PKT_RX      "MulticastPacketsReceived"
#define BUSY_ETH_MCTP_TOTAL_BC_PKT_RX       "BroadcastPacketsReceived"
#define BUSY_ETH_MCTP_TOTAL_UNI_PKT_TX      "UnicastPacketsTransmitted"
#define BUSY_ETH_MCTP_TOTAL_MUL_PKT_TX      "MulticastPacketsTransmitted"
#define BUSY_ETH_MCTP_TOTAL_BC_PKT_TX       "BroadcastPacketsTransmitted"
#define BUSY_ETH_MCTP_FCS_RX_ERRS           "FcsReceiveErrors"
#define BUSY_ETH_MCTP_RUNT_PKT_RX           "RuntPacketsReceived"
#define BUSY_ETH_MCTP_JABBER_PKT_RX         "JabberPacketsReceived"
#define BUSY_ETH_MCTP_SINGLE_COLLISION_TX_FRAMES          "SingleCollisionTransmitFrames"
#define BUSY_ETH_MCTP_MULTIPLE_COLLISION_TX_FRAMES        "MultipleCollisionTransmitFrames"
#define BUSY_ETH_MCTP_LATE_COLLISION_FRAMES               "LateCollisionFrames"
#define BUSY_ETH_MCTP_EXCESSIVE_COLLISION_FRAMES          "ExcessiveCollisionFrames"
#define BUSY_ETH_MCTP_GET_SDI5_LOG 						  "GetNetCardLogOverMCTP"
/* END:  ncsi over mctp add by tangjie*/

#define BUSY_ETH_LLDP_CHASSISID_SUBTYPE     "ChassisIDSubType"
#define BUSY_ETH_LLDP_CHASSISID_SUBDESP     "ChassisIDSubDesp"
#define BUSY_ETH_LLDP_PORTID_SUBTYPE           "PortIDSubType"
#define BUSY_ETH_LLDP_PORTID_SUBDESP           "PortIDSubDesp"
#define BUSY_ETH_LLDP_SWITCH_NAME               "PortConnecSwitchName"
#define BUSY_ETH_LLDP_OS_VLAN_ID                    "PortOSVlanID"

#define BUSY_ETH_ATTRIBUTE_MACID            "MacId"
#define BUSY_ETH_ATTRIBUTE_LINK_STATUS      "LinkStatus"
/*BEGIN : Added by cwx290152, 2017/05/04, PN : DTS2017032712490 */
#define BUSY_ETH_ATTRIBUTE_LINK_SRC         "LinkStatusSrc"
/*END : Added by cwx290152, 2017/05/04, PN : DTS2017032712490 */
#define BUSY_ETH_ATTRIBUTE_NIC_TYPE         "NICType"
#define BUSY_ETH_ATTRIBUTE_MANUFACTURE      "Manufacture"
#define BUSY_ETH_ATTRIBUTE_CARD_TYPE        "CardType"
#define BUSY_ETH_ATTRIBUTE_PORT_NUM         "PortNum"
#define BUSY_ETH_ATTRIBUTE_ETH_NUM          "EthNum"
#define BUSY_ETH_ATTRIBUTE_SILK_NUM         "SilkNum"
#define BUSY_ETH_ATTRIBUTE_ADAPTIVE_FLAG    "AdaptiveFlag"
#define BUSY_ETH_GROUP_ATTRIBUTE_MAC        "MacAddr"  //默认永久MAC地址
#define BUSY_ETH_ATTRIBUTE_ACTUAL_MAC_ADDR          "ActualMac"  //当前实际临时MAC(无状态计算配置等),无BMA默认与永久MAC一致,有BMA以BMA上报为准
#define BUSY_ETH_ATTRIBUTE_COS2UP                   "Cos2up"
#define BUSY_ETH_ATTRIBUTE_UP_PGID                  "Uppgid"
#define BUSY_ETH_ATTRIBUTE_PGPCT                        "Pgpct"
#define BUSY_ETH_ATTRIBUTE_STRICT                   "Strict"
#define BUSY_ETH_ATTRIBUTE_pfcmap                   "Pfcmap"
#define BUSY_ETH_ATTRIBUTE_FRU_DEVICEID     "FruDeviceId"
#define BUSY_ETH_ATTRIBUTE_REF_NETCARD      "RefNetCard"
#define BUSY_ETH_METHOD_SET_LINK_STATUS     "SetLinkStatus"
#define BUSY_ETH_METHOD_SET_LINK_STATUS_FROM_BMA   "SetLinkStatusFromBMA"
#define BUSY_ETH_METHOD_SET_ADAPTIVE_FLAG   "SetAdaptiveFlag"
/* BEGIN: Added by z00352904, 2016/11/15 20:19:4  PN:AR-0000264509-001-001 */
/* BEGIN: 2018-4-18 z00416979 AR.SR.SFEA02130924.001.001 */
#define BUSY_ETH_METHOD_SET_BDF             "SetBDF"
#define BUSY_ETH_METHOD_SET_ROOTBDF         "SetROOTBDF"
/* END:   2018-4-18 z00416979 AR.SR.SFEA02130924.001.001 */
/* BEGIN: Added by qinjiaxiang, 2018/5/15   PN:SR.SFEA02130923.001.001*/
#define BUSY_ETH_METHOD_SET_MAC             "SetMacAddress"
#define BUSY_ETH_METHOD_SET_ACTUAL_MAC      "SetActualMacAddress"
#define BUSY_ETH_METHOD_SET_LINK_STATUS_STR "SetLinkStatusStr"
/* END:   Added by qinjiaxiang, 2018/5/15 */
#define BUSY_ETH_ATTRIBUTE_NAME             "Name"
#define BUSY_ETH_ATTRIBUTE_INTER_SPEED      "InterfaceSpeed"
#define BUSY_ETH_ATTRIBUTE_DIS_NAME         "DisplayName"
#define BUSY_ETH_ATTRIBUTE_CONN_FLAG        "ConnectFlag"

#define BUSY_ETH_ATTRIBUTE_OSETH_NAME       "OSEthName"
#define BUSY_ETH_ATTRIBUTE_IPV4INFO         "IPv4Info"
#define BUSY_ETH_ATTRIBUTE_IPV6INFO         "IPv6Info"
#define BUSY_ETH_ATTRIBUTE_BMAID            "BMAId"
#define BUSY_ETH_ATTRIBUTE_BDF              "BDF"
#define BUSY_ETH_ATTRIBUTE_ROOT_BDF         "RootBDF"
#define BUSY_ETH_ATTRIBUTE_CONTAINED_PORTS  "ContainedPorts"
#define BUSY_ETH_ATTRIBUTE_WORK_MODE        "WorkMode"
#define BUSY_ETH_ATTRIBUTE_LINK_STATUS_STR  "LinkStatusStr"
#define BUSY_ETH_ATTRIBUTE_LINK_STATUS_BMA  "LinkStatusBMA"
#define BUSY_ETH_ATTRIBUTE_ALIGNMENTERRORS  "AlignmentErrors"
/* END:   Added by z00352904, 2016/11/15 20:19:6*/
/* BEGIN: Added by gwx455466, 2017/4/6 15:5:24   问题单号:AR-0000276593-005-009 */
#define BUSY_ETH_ATTRIBUTE_PORT_TYPE        "PortType"
/* END:   Added by gwx455466, 2017/4/6 15:5:28 */
/* BEGIN: Added by jwx372839, 2017/4/9 10:10:28   问题单号:AR-0000276593-004-007 */
#define BUSY_ETH_ATTRIBUTE_NETDEV_FUNCTYPE  "NetDevFuncType"
/* END:   Added by jwx372839, 2017/4/9 10:31:33 */
/* BEGIN: Modified by jwx372839, 2017/6/13 14:57:4   问题单号:DTS2017041900836 */
#define BUSY_ETH_ATTRIBUTE_SUPPORT_FUNCTYPE "SupportedFuncType"
/* END:   Modified by jwx372839, 2017/6/13 14:57:8 */

/* BEGIN: Modified by jwx372839, 2017/8/3 21:18:28   问题单号:AREA02109379-006-001 */
#define BUSY_ETH_ATTRIBUTE_BANDWIDTH_USAGE  "BandwidthUsage"
/* END:   Modified by jwx372839, 2017/8/3 21:18:33 */
#define BUSY_ETH_ATTRIBUTE_PACKETS_ERRORS  "ErrorPackets"
#define BUSY_ETH_ATTRIBUTE_PACKETS_DROPPED  "DroppedPackets"

/* BEGIN: Added by gwx455466, 2017/8/23 21:53:44   问题单号:SREA02109352-001-001 */
#define BUSY_ETH_ATTRIBUTE_IFTYPE             "IFType"
/* END:   Added by gwx455466, 2017/8/23 21:53:48 */
/* BEGIN: Added by lixing lwx559562, 2018/7/20 */
#define BUSY_ETH_ATTRIBUTE_AVAILABLEFLAG   "AvailableFlag"
/* END:   Added by lixing lwx559562, 2018/7/20 */
#define BUSY_ETH_ATTRIBUTE_LLDP_ENABLE_MODE "LLDPEnabled"
#define BUSY_ETH_ATTRIBUTE_ALWAYS_AVAILABLE "AlwaysAvailable"

/* BEGIN: Added by jwx372839, 2017/4/12 14:43:24   问题单号:AR-0000276593-004-006 */
#define BUSY_ETH_ATTRIBUTE_REF_SLAVEPORT    "RefSlavePort"
/* BEGIN: Modified by zwx382755, 2018/3/23   PN:AR.SR.SFEA02119720.021.001 */
#define BUSY_ETH_ATTRIBUTE_LINK_MNTR_FRE    "LinkMonitorFrequency"
#define BUSY_ETH_ATTRIBUTE_UUID             "UUID"
#define BUSY_ETH_ATTRIBUTE_CONFIG_STATE     "ConfigState"
#define BUSY_ETH_ATTRIBUTE_CONFIG_RESULT    "ConfigResult"
#define BUSY_ETH_ATTRIBUTE_IS_ONBOOT        "IsOnBoot"
#define BUSY_ETH_METHOD_CREATE_VLAN         "CreateVLAN"
#define BUSY_ETH_METHOD_DELETE_VLAN         "DeleteVLAN"
#define BUSY_ETH_METHOD_CREATE_BOND         "CreateBond"
#define BUSY_ETH_METHOD_DELETE_BOND         "DeleteBond"
#define BUSY_ETH_METHOD_SET_IPV4_INFO       "SetIPv4Info"
#define BUSY_ETH_METHOD_SET_BOND_MNTR_FRE   "SetMonitorFrequency"
/* END:   Modified by zwx382755, 2018/3/23 */

#define BUSY_ETH_METHOD_SET_PORT_LLDP_ENABLE       "SetPortLLDPEnable"

/* BEGIN: Modified by jwx372839, 2018/8/21 10:18:55   问题单号:DTS2018071703443 */
#define BUSY_ETH_ATTRIBUTE_FIRM_VERSION          "FirmwareVersion"
#define BUSY_ETH_ATTRIBUTE_DRIVER_NAME            "DriverName"
#define BUSY_ETH_ATTRIBUTE_DRIVER_VERSION         "DriverVersion"
/* END:   Modified by jwx372839, 2018/8/21 10:19:1 */

/* BEGIN: Added by zengrenchang 00296903, 2018/9/15   问题单号:DTS2018091508691*/
#define BUSY_ETH_ATTRIBUTE_NCSI_RX_CHANNEL_STATE          "NCSIRXChannelState"
/* END:   Added by zengrenchang 00296903, 2018/9/15 */

#define BUSY_ETH_ATTRIBUTE_PACKETS_DEFAULT_NUM  0xffffffff


#define BUSY_ETH_ATTRIBUTE_LINK_STATUS_BMA_LINKDOWN         1
#define BUSY_ETH_ATTRIBUTE_LINK_STATUS_BMA_LINKUP       2
#define BUSY_ETH_ATTRIBUTE_LINK_STATUS_BMA_NOLINK           3
#define BUSY_ETH_ATTRIBUTE_LINK_STATUS_BMA_NA       0xFF

#define CLASS_FIBRE_CHANNEL_PORT            "FibreChannel"
#define PROPERTY_FC_CARD_PORT_ID                     "Id"
#define PROPERTY_FC_CARD_PORT_WWPN                   "WWPN"
#define PROPERTY_FC_CARD_PORT_WWNN                   "WWNN"
#define PROPERTY_FC_CARD_PORT_DEFAULT_WWPN           "DefaultWWPN"
#define PROPERTY_FC_CARD_PORT_DEFAULT_WWNN           "DefaultWWNN"
#define PROPERTY_FC_CARD_PORT_FCID                   "FCId"
#define PROPERTY_FC_CARD_PORT_PORT_TYPE              "PortType"
#define PROPERTY_FC_CARD_PORT_PORT_NAME              "PortName"
#define PROPERTY_FC_CARD_PORT_LINK_SPEED             "LinkSpeed"
#define PROPERTY_FC_CARD_PORT_LINK_STATUS            "LinkStatus"
#define PROPERTY_FC_CARD_PORT_ROOT_BDF               "RootBDF"
#define PROPERTY_FC_CARD_PORT_BDF                    "BDF"
/* BEGIN: Added by fwx527488, 2018/11/12 ,支持通过MCTP获取1822芯片的HBA卡信息*/
#define PROPERTY_FC_CARD_PORT_TX_SN_SPEED                   "TxSnSpeed"
#define PROPERTY_FC_CARD_PORT_RX_SN_SPEED                   "RxSnSpeed"
#define PROPERTY_FC_CARD_PORT_SN_STAGE                      "SnStage"
#define PROPERTY_FC_CARD_PORT_SFP_OPEN                      "SfpOpen"
#define PROPERTY_FC_CARD_PORT_WORK_SPEED                    "WorkSpeed"
#define PROPERTY_FC_CARD_PORT_WORK_TOPO                     "WorkTopo"
#define PROPERTY_FC_CARD_PORT_TX_BB_CREDIT                  "TxBbCredit"
#define PROPERTY_FC_CARD_PORT_RX_BB_CREDIT                  "RxBbCredit"
#define PROPERTY_FC_CARD_PORT_PORT_STATUS                   "PortStatus"
/* END: Added by fwx527488, 2018/11/12 */

/* END:   Added by jwx372839, 2017/4/12 14:43:33 */

/* OscaPort类定义*/
#define ETH_CLASS_NAME_OSCAPORT             "OscaPort"
#define ETH_OSCAPORT_ATTRIBUTE_PORTSELECT   "PortSelect"
#define ETH_OSCAPORT_ATTRIBUTE_PORTTYPE     "PortType"

/* LDAP类定义*/
#define LDAPCOMMON_CLASS_NAME                     "LDAPCommon"
#define LDAPCOMMON_ATTRIBUTE_ENABLE               "Enable"
#define LDAPCOMMON_METHOD_SET_LDAP_ENABLE        "SetLDAPEnable"

/*  Kerberos 类定义 */
#define KRBCOMMON_CLASS_NAME                      "KRBCommon"
#define KRBCOMMON_ATTRIBUTE_ENABLE                "Enable"
#define KRBCOMMON_METHOD_SET_KERBEROS_ENABLE      "SetKRBEnable"


/* LDAPServer类定义*/
#define LDAP_CLASS_NAME                     "LDAP"
#define OBJECT_LDAP_NAME                    "LDAP"
#define LDAP1_OBJ_NAME                      "LDAP"
#define LDAP1_CERT_OBJ_NAME                "LDAPServer1"
#define LDAP2_OBJ_NAME                      "LDAPServer2"
#define LDAP3_OBJ_NAME                      "LDAPServer3"
#define LDAP4_OBJ_NAME                      "LDAPServer4"
#define LDAP5_OBJ_NAME                      "LDAPServer5"
#define LDAP6_OBJ_NAME                      "LDAPServer6"
#define LDAP_ATTRIBUTE_ENABLE              "Enable"
#define LDAP_ATTRIBUTE_ID                   "ID"
#define LDAP_ATTRIBUTE_CERTSTATUS          "CertStatus"
/* BEGIN: Modified by baoxu, 2018/8/21   PN:AR.SR.SFEA02130924.006.003*/
#define LDAP_ATTRIBUTE_CERT_VERIFI_LEVEL   "CertificateVerificationLevel"
/* END: Modified by baoxu, 2018/8/21   PN:AR.SR.SFEA02130924.006.003*/
#define LDAP_ATTRIBUTE_HOST_ADDR           "HostAddr"
#define LDAP_ATTRIBUTE_USER_DOMAIN         "UserDomain"
#define LDAP_ATTRIBUTE_LADPVER              "LdapVer"
#define LDAP_ATTRIBUTE_PORT                 "Port"
#define LDAP_ATTRIBUTE_SCOPE                "Scope"
#define LDAP_ATTRIBUTE_TIMELIMIT            "TimeLimit"
#define LDAP_ATTRIBUTE_BIND_TIMELIMIT       "BindTimeLimit"
#define LDAP_ATTRIBUTE_FOLDER               "Folder"
#define LDAP_ATTRIBUTE_STARTTIME            "StartTime"
#define LDAP_ATTRIBUTE_EXPIRETIME           "Expiration"
#define LDAP_ATTRIBUTE_CRL_ENABLED "CRLVerificationEnabled"
#define LDAP_ATTRIBUTE_CRL_START_TIME "CRLStartTime"
#define LDAP_ATTRIBUTE_CRL_EXPIRE_TIME "CRLExpireTime"

/* BEGIN: AREA02109379-009-001 支持LDAP代理用户认证. Added by cwx290152, 2017/8/15 */
#define LDAP_ATTRIBUTE_BIND_DN                  "BindDN"
#define LDAP_ATTRIBUTE_BIND_DN_PSW              "BindDNPsw"
#define LDAP_ATTRIBUTE_BIND_DN_PSW_MASTERKEY_ID "BindDNPswMasterKeyId"
#define LDAP_ATTRIBUTE_BASE_DN                  "BaseDN"
#define LDAP_METHOD_SET_BIND_DN                 "SetBindDN"
#define LDAP_METHOD_SET_BIND_DN_PSW             "SetBindDNPsw"
#define LDAP_METHOD_SET_BASE_DN                 "SetBaseDN"
/* END: AREA02109379-009-001 支持LDAP代理用户认证. Added by cwx290152, 2017/8/15 */

#define LDAP_METHOD_SET_ENABLE              "SetEnable"
#define LDAP_METHOD_SET_PORT              "SetLdapPort"
#define LDAP_METHOD_SET_CERTSTATUS              "SetCertStatus"
/* BEGIN: Modified by baoxu, 2018/8/21   PN:AR.SR.SFEA02130924.006.003*/
#define LDAP_METHOD_SET_CERT_VERIFI_LEVEL     "SetCertificateVerificationLevel"
/* END: Modified by baoxu, 2018/8/21   PN:AR.SR.SFEA02130924.006.003*/
#define LDAP_METHOD_SET_HOST_ADDR           "SetHostAddr"
#define LDAP_METHOD_SET_USER_DOMAIN         "SetUserDomain"
#define LDAP_METHOD_SET_USER_FOLDER         "SetFolder"
#define LDAP_METHOD_COMPOSE_USER_DOMAIN     "LdapComposeUserDomain"
#define LDAP_METHOD_SPLIT_USER_DOMAIN       "LdapSplitUserDomain"
#define LDAP_METHOD_CERTUPLOAD              "LDAPCertUpload"
#define LDAP_METHOD_IMPORT_CRL "ImportCRL"
#define LDAP_METHOD_DELETE_CRL "DeleteCRL"

#define KRB_GROUP_METHOD_SET_GROUP_SID      "SetGroupSID"
#define KRB_METHOD_UPLOAD_KEYTABLE          "KRBKeytabUpload"
#define KRB_METHOD_SET_REALM                "SetKRBRealm"
/* BEGIN: 2019-2-27 tianpeng twx572344 PN:DTS2019022302557 */
#define LDAP_KRB_GROUP_METHOD_SET_GROUP_INFO  "SetGroupInfo"
/* END:   2019-2-27 tianpeng twx572344 PN:DTS2019022302557 */

/** BEGIN: Modified by z00355831, 2018/4/18 AR.SR.SFEA02130924.006.001 支持LDAP证书链*/
/*LDAPCert类定义*/
#define LDAP_CERT_CLASS_NAME                "LDAPCert"
#define LDAP_CERT_PROPERTY_CERTID           "CertId"
#define LDAP_CERT_PROPERTY_CERTTYPE         "CertType"
#define LDAP_CERT_PROPERTY_SUBJECT_INFO     "SubjectInfo"
#define LDAP_CERT_PROPERTY_ISSUER_INFO      "IssuerInfo"
#define LDAP_CERT_PROPERTY_START_TIME       "StartTime"
#define LDAP_CERT_PROPERTY_EXPIRATION       "Expiration"
#define LDAP_CERT_PROPERTY_SERIALNUMBER     "SerialNumber"
#define LDAP_CERT_PROPERTY_KEYUSAGE         "KeyUsage"
#define LDAP_CERT_PROPERTY_KEYLENGTH        "KeyLength"
#define LDAP_CERT_PROPERTY_SIGALGO          "SigAlgorithm"
/** END:   Added by z00355831, 2018/4/18 */


/* LDAP group类定义*/
#define LDAP_GROUP_CLASS_NAME                     "LDAPGroup"
#define LDAP_GROUP_ATTRIBUTE_LDAPSERVER_ID        "LDAPServerID"
#define LDAP_GROUP_ATTRIBUTE_GROUP_ID             "GroupID"
#define LDAP_GROUP_ATTRIBUTE_GROUP_NAME           "GroupName"
#define LDAP_GROUP_ATTRIBUTE_GROUP_DOMAIN         "GroupDomain"
#define LDAP_GROUP_ATTRIBUTE_GROUP_PRIVILEGE      "GroupPrivilege"
#define LDAP_GROUP_ATTRIBUTE_GROUP_USERROLEID     "GroupUserRoleId"
#define LDAP_GROUP_ATTRIBUTE_GROUP_FOLDER         "GroupFolder"
#define LDAP_GROUP_ATTRIBUTE_GROUP_PRIVILEGE_MASK "GroupPrivilegeMask"
#define LDAP_GROUP_ATTRIBUTE_GROUP_PERMIT_ID      "GroupPermitRuleIds"
#define LDAP_GROUP_ATTRIBUTE_LOGIN_INTERFACE      "GroupLoginInterface"
#define LDAP_GROUP_METHOD_SET_GROUP_PERMIT_ID          "SetLdapPermitRuleIds"
#define LDAP_GROUP_METHOD_SET_GROUP_NAME               "SetGroupName"
#define LDAP_GROUP_METHOD_SET_GROUP_DOMAIN             "SetGroupDomain"
#define LDAP_GROUP_METHOD_SET_GROUP_FOLDER             "SetGroupFolder"
#define LDAP_GROUP_METHOD_SET_GROUP_PRIVILEGE          "SetGroupPrivilege"
#define LDAP_GROUP_METHOD_SET_GROUP_USERROLEID         "SetGroupUserRoleId"
#define LDAP_GROUP_METHOD_DEL_GROUP                    "DelGroup"
#define LDAP_GROUP_METHOD_COMPOSE_GROUP_DOMAIN         "LdapComposeGroupDomain"
#define LDAP_GROUP_METHOD_SPLIT_GROUP_DOMAIN           "LdapSplitGroupDomain"
#define LDAP_GROUP_METHOD_CERT_UPLOAD                    "LDAPCertUpload"
#define LDAP_GROUP_METHOD_CERT_INFO           "GetLDAPCertInformation"
#define LDAP_GROUP_METHOD_SET_GROUP_LOGIN_INTERFACE   "SetGroupLoginInterface"

#define LDAP_GROUP_SID_INFO                 "SID"
#define KRB_OBJECT_NAME                       "KRBServer"

/* PortConfig类定义 */
#define SERVER_PORT_CLASS_NAME                    "PortConfig"
#define FTP_SERVER_PORT_OBJ_NAME                  "FTP"
#define SSH_SERVER_PORT_OBJ_NAME                  "SSH"
#define WEBHTTP_SERVER_PORT_OBJ_NAME              "WEBHTTP"
#define WEBHTTPS_SERVER_PORT_OBJ_NAME             "WEBHTTPS"
#define HMMSSHNAT_SERVER_PORT_OBJ_NAME            "HMMSSHNAT"
#define HMMSSHNAT_SERVER_PORT_OBJ_NAME2           "HMMSSHNAT2"
#define HMMSSHNAT_SERVER_NAME                     "hmmsshnat"
#define HMMSSHNAT_SERVER_NAME2                    "hmmsshnat2"
#define SERVER_PORT_ATTRIBUTE_STATE               "State"
#define SERVER_PORT_ATTRIBUTE_PORT                "Port"
#define SERVER_PORT_ATTRIBUTE_SERVER_ID           "ServiceID"
#define SERVER_PORT_ATTRIBUTE_PORT_FILE           "PortFile"
#define SERVER_PORT_ATTRIBUTE_STATE_FILE          "StateFile"
#define SERVER_PORT_ATTRIBUTE_NAME                "Name"
#define SERVER_PORT_ATTRIBUTE_PROTOCOL            "Protocol"
#define SERVER_PORT_METHOD_SET_STATE              "SetState"
#define SERVER_PORT_METHOD_SET_PORT               "SetPort"
#define SERVER_PORT_METHOD_PORT_CHECK             "PortCheck"
#define SERVER_PORT_METHOD_STATE_CHECK            "StateCheck"
#define SERVER_PORT_METHOD_PROPERTY_SYNC          "PropertySyncMethod"

/* BEGIN: Modified by m00168195, 2015/11/14   PN:AR6C1E316C-B5B1-49e9-8EB7-7F0E7391C37B*/
#define CLASS_EX_PORT_CONFIG                      "ExPortConfig"
#define OBJ_NAME_EX_PORT_CONFIG_SSDP              "SSDP"
#define PROPERTY_EX_PORT_CONFGI_STATE             "State"
#define PROPERTY_EX_PORT_CONFGI_PORT              "Port"
#define PROPERTY_EX_PORT_CONFGI_SERVERID          "ServiceID"
#define PROPERTY_EX_PORT_CONFGI_PORTFILE          "PortFile"
#define PROPERTY_EX_PORT_CONFGI_STATEFILE         "StateFile"
#define PROPERTY_EX_PORT_CONFGI_NAME              "Name"
#define PROPERTY_EX_PORT_CONFGI_PROTOCOL          "Protocol"
/* END:   Modified by m00168195, 2015/11/14 */

/* PortMapping类定义 */
#define CLASS_NAME_PORTMAPPING                    "PortMapping"
#define OBJ_NAME_SSHPORTMAPPING                   "SSHPortMapping"
#define PROPERTY_PORTMAPPING_STATE                "State"
#define PROPERTY_PORTMAPPING_INPUTETH             "InputEth"
#define PROPERTY_PORTMAPPING_DSTIP                "DstIP"           //映射前目的ip
#define PROPERTY_PORTMAPPING_DSTIPMAPING          "DstIPMapping"    //映射后目的ip
#define PROPERTY_PORTMAPPING_SRCIPMAPING          "SrcIPMapping"    //映射后源ip
#define PROPERTY_PORTMAPPING_PROTOCOL             "Protocol"
#define PROPERTY_PORTMAPPING_PORT                 "Port"
#define PROPERTY_PORTMAPPING_PORTMAPPING          "PortMapping"

/*End: NETWORK 模块依赖的类和对象 */

#define CLASS_NAME_DFTVERSION                              "DftVersion"
#define OBJ_NAME_FRU0IPMBDISPLAY                           "Fru0IpmbDisplay"
#define PROPERTY_DFTVERSION_DISPLAYFLAG                    "DisplayFlag"
#define PROPERTY_DFTVERSION_IPMBADDR                       "IpmbAddr"

#define CLASS_NAME_IPMBETH                                 "IpmbEth"
#define PROPERTY_IPMBETH_SA                                "SlaveAddr"

/*FactorySetting*/
#define CLASS_NAME_FACTORYSETTING                          "CfgMgnt"
#define OBJ_NAME_FACTORYSETTING                            "cfgmgnt"
#define METHOD_FACTORYSETTING_LOADBAK                      "LoadBak"
#define METHOD_FACTORYSETTING_BACKUP                       "BackUp"

/*功率管理*/
#define CLASS_NAME_AMMETER                                  "SysPower"
#define OBJ_NAME_AMMETER                                    "syspower"
#define PROTERY_PSTOTAL_POWER                               "Power"
#define PROTERY_SYSPOWER_PCA9545_BUS                        "Pca9545bus"
#define PROTERY_SYSPOWER_PCA9545_ADDR                       "Pca9545addr"
#define PROTERY_SYSPOWER_PCA9545_CHANNEL                    "Pca9545ch"
#define PROTERY_SYSPOWER_IS_PCA9545_CHANNEL                 "IsPca9545Switch"
#define PROTERY_SYSPOWER_PSU_ASSIGNED_CHANNEL               "PsuAssignedChannel"
#define PROTERY_SYSPOWER_FRU_SOURCE                         "FruSource"
#define PROTERY_SYSPOWER_DEEP_SLEEP_MODE_ENABLE             "DeepSleepModeEnable"
#define PROTERY_SYSPOWER_PSU_OPOK_STATUS  "PsuOPOKStatus"
#define PROTERY_SYSPOWER_PSU_UPGRATE_STATE                  "PsuUpgradeState"
#define PROTERY_SYSPOWER_REDUNDANCY_MODE                    "RedundancyMode"
#define PROPERTY_SYSPOWEWR_EXPECTED_WORK_CNT                "ExpectedWorkPsCnt"
#define PROPERTY_SYSPOWEWR_WORKLOAD_NEEDED_CNT              "WorkloadNeededPsCnt"
/* BEGIN: Added by DTS2015032405413, 2015/5/9 SN:DTS2015040807396  */
#define PROTERY_SYSPOWER_FORCE_UPGRADE_FLAG                 "ForceUpgradeFlag"
/* END: Added by DTS2015032405413, 2015/5/9 SN:DTS2015040807396  */
/* BEGIN: Modified by wangpenghui wwx382232, 2018/8/13 11:48:54   问题单号:Redfish接口补齐 */
#define PROTERY_SYSPOWER_COLLECT_HISTORY_DATA_STATUS              "CollectHistoryDataStatus"
/* END:   Modified by wangpenghui wwx382232, 2018/8/13 11:48:57 */
/* BEGIN: Modified by w84145563, 2019/11/6 */
#define PROTERY_SYSPOWER_NORMAL_AND_REDUNDANCY_ENABLE             "NormalAndRedundancyEnable"
#define PROTERY_SYSPOWER_NORMAL_AND_REDUNDANCY_ERROR_NO             "NormalAndRedundancyErrorNo"
#define PROPERTY_SYSPOWER_PS_MANAGE_HOST_TYPE                       "PSManageHostType"
/* END:   Modified by w84145563, 2019/11/6 */
/* BEGIN: Added by DTS2020041812238, 2020/4/22, zwx891482 */
#define PROPERTY_UPGRADING_SLOTS                             "UpgradingSlots"
/* END:   Added by DTS2020041812238, 2020/4/22, zwx891482 */
#define PROTERY_SYSPOWER_SINGLE_DC_PSU_BLACK_BOX_SUPPORT     "SingleDcPsuBlackBoxSupport"
#define PROPERTY_SYSPOWEWR_PSU_MIX_INSERT_SUPPORT "PsuMixInsertSupport"
#define PROPERTY_SYSPOWEWR_POWER_BACKUP_LOW_ALARM "PowerBackupLowAlarm"
/*Begin: modify by  l84105457 for AR.SR.SFEA02130924.023.011 2018-07-17*/
#define PSM_ESPERCENT_PROPERTY_NAME                         "ESPercent"
#define PSM_ENERGYSAVE_PROPERTY_NAME                        "EnergySave"
#define PSM_CARBONSAVE_PROPERTY_NAME                        "CarbonSave"
/*End: modify by  l84105457 for AR.SR.SFEA02130924.023.011 2018-07-17*/
#define PROPERTY_PW_INFO_MODE                               "PowerInfoMode"
#define PROPERTY_POWER_UNIT_GROUP_START_ID                  "PowerUnitGroupStartID"
#define PROPERTY_PSU_FRU_CONFIG               "PsuFruConfig"
#define PROPERTY_PSU_COOLING_POLICY_STATUS               "PSUCoolingPolicyStatus"
#define METHOD_SET_PW_INFO_MODE                             "SetPowerInfoMode"
#define METHOD_SET_PW_UNIT_GROUP_START_ID                   "SetPowerUnitGroupStartID"
#define METHOD_SET_DEEP_SLEEP_MODE                          "SetDeepSleepMode"

#define METHOD_PSTOTAL_CLEARRECORD                                     "ClearRecord"
#define METHOD_SET_SYS_POWER_STATE                                    "CtrlSysPowerState"
#define METHOD_SET_SYS_POWER_UPGRADING_SLOTS                          "SetSysPowerUpgradingSlots"
#define METHOD_SHIELD_ACLOST_EVENT_RECORD                             "ShieldAclostEventRecord"


#define PROTERY_RECORD_BEGIN_TIME                 "RecordBeginTime"
#define PROTERY_PEAK_TIME                         "PeakTime"
#define PROTERY_PEAK_VALUE                        "PeakValue"
#define PROTERY_MIN_VALUE                         "MinValue"
#define PROTERY_POWER_CONSUME                     "PowerConsumption"
#define PROTERY_HEAT                              "Heat"
#define PROTERY_HIGH_POWER_THRESHOLD_WATTS        "HighPowerThresholdWatts"

#define PROTERY_AVERAGE_VALUE                     "AveragePower"
#define PROTERY_UNSUPPORT_ACTIVE_STANDBY_MODE     "UnSupportActiveStandbyMode"

#define PROPERTY_UPPER_LIMIT                        "UpperLimit"
#define PROPERTY_LOWER_LIMIT                        "LowerLimit"
#define PROPERTY_EXPECTED_POWER_MODE                "ExpectedMode"
#define PROPERTY_ACTUAL_POWER_MODE                  "ActualMode"
#define PROPERTY_EXPECTED_ACTIVE                    "ExpectedActive"

#define PROPERTY_WORK_PS_COUNT                      "WorkPsCnt"

/* BEGIN: Added by cwx290152, 2015/11/25 SN:AR6C80A962-95DF-427b-B45F-1E7BB2AA7FD7  */
#define PROPERTY_INIT_PRESENT_PS_COUNT                   "ACBootPSCount"
#define PROPERTY_PRESENT_PS_COUNT                            "PSPresentCount"
#define PROPERTY_SYS_POWER_HEALTH                            "Health"
/* END: Added by cwx290152, 2015/11/25 SN: AR6C80A962-95DF-427b-B45F-1E7BB2AA7FD7 */

/* BEGIN: Modified for 问题单号:Hardware_020 by j00383144, 2016/10/9 */
#define PROPERTY_BOARD_POWER_DROP_STAMP                   "BoardPowerDropStamp"
#define PROPERTY_BOARD_POWER_DROP_FLAG                    "BoardPowerDropFlag"
/* END:  Modified for 问题单号:Hardware_020 by j00383144, 2016/10/9 */

#define METHOD_AMMETER_RESET                        "Reset"
#define METHOD_SET_PLANTFORM_WORKMODE               "SetPlatformWorkMode"
#define METHOD_SET_POWER_WORKMODE               "SetPowerWorkMode"
#define METHOD_SET_POWER_WORKMODEMASK               "SetPowerWorkModeMask"
/* BEGIN: Added by DTS2015032405413, 2015/5/9 SN:DTS2015040807396  */
#define METHOD_SET_POWER_LOWER_LIMIT               "SetPowerLowerLimit"
#define METHOD_SET_POWER_UPPER_LIMIT               "SetPowerUpperLimit"
/* END: Added by DTS2015032405413, 2015/5/9 SN:DTS2015040807396  */

/* 增加方法用于判断主备模式参数是否有效,hwx150504, 2020/20/24 */
#define METHOD_CHECK_FAILOVER_CONDITION  "CheckFailoverCondition"


#define METHOD_ROLLBACK_POWER_RECORD             "RollbackRecord"

/* BEGIN for ARB7F63698-2463-45ba-AEEB-9A0B21853DA2 : Added by ouluchun 00294050 2015-11-18 */
#define METHOD_SET_HI_51_STATE               "SetHi51State"
/* END for ARB7F63698-2463-45ba-AEEB-9A0B21853DA2 : Added by ouluchun 00294050 2015-11-18 */
#define METHOD_SET_PS_INFO_TASK_STATE        "SetPsInfoTaskState"
#define METHOD_GET_FRU_FROM_PSU               "GetFruFromPsu"
/* BEGIN: Modified for 问题单号:DTS2016121307330 by j00383144, 2017/1/18 */
#define METHOD_SET_PSU_UPGRADE_STATE               "SetPsuUpgradeState"
/* END:  Modified for 问题单号:DTS2016121307330 by j00383144, 2017/1/18 */

/* BEGIN: Modified by wangpenghui wwx382232, 2018/7/31 9:24:23   问题单号:Redfish接口补齐 */
#define METHOD_COLLECT_HISTOTY_DATA               "CollectHistoryData"
/* END:   Modified by wangpenghui wwx382232, 2018/7/31 9:24:25 */

/* BEGIN: Added by DTS2015032405413, 2015/5/9 SN:DTS2015040807396  */
#define METHOD_SET_FORCE_UPGRADE_FLAG               "SetForceUpgradeFlag"
/* END: Added by DTS2015032405413, 2015/5/9 SN:DTS2015040807396  */
#define METHOD_SET_POWER_HIGH_THRESHOLD "SetHighPowerThreshold"
#define METHOD_SYSPOWER_PROPERTY_SYNC "PropertySyncMethod"
#define METHOD_SET_PSU_SUPPLY_SOURCE_CFG "SetPsuSupplySourceCfg"

/* BEGIN: Added by l00419687, 2018-03-19 */
#define METHOD_POWER_HISTORY_PARA_SYNC             "PowerHistoryParaSync"
#define METHOD_POWERCAPPING_PROPERTY_SYNC          "PowerCappingSync"
#define METHOD_POWERMGHNT_PROPERTY_SYNC_SAVE        "PowerMgntSyncSave"
#define METHOD_POWERMGHNT_PROPERTY_SYNC             "PowerMgntPropertySync"
/* END: Added by l00419687, 2018-03-19 */

#define METHOD_SET_OUTPUT_LIMIT_POWER               "SetOutputLimitPower"
#define METHOD_SYNC_CONF_AFTER_STARTUP              "SyncConfAfterStartup"

/*功率封顶*/
#define CLASS_NAME_POWERCAPPING                                  "PowerCapping"
#define OBJ_NAME_POWERCAPPING                                  "powercapping"
/* BEGIN: Modified by wangpenghui wwx382232, 2018/3/23 17:45:48   问题单号:DTS2018010606398  */
#define PROTERY_POWERCAP_CPU_LIMIT_POWER                 "CPULimitPower"
#define PROTERY_CPU_POWER_PROTECTION_STA               "PowerProtectionStatus"
/* END:   Modified by wangpenghui wwx382232, 2018/3/23 17:45:57 */
#define PROTERY_POWERCAP_ENABLE                 "Enable"   //功率封顶启用状态
#define PROTERY_POWERCAP_LIMITVALUE                        "LimitValue"  //功率封顶值
#define PROTERY_POWERCAP_FAILED                            "Failed"     // 功率封顶失败标志
#define PROTERY_POWERCAP_FAILACTION_SUPPORT                "FailActionSupport"  // 功率封顶失败动作配置支持标志
#define PROTERY_POWERCAP_FAILACTION                        "FailAction"  //功率封顶进一步动作
#define PROTERY_POWERCAP_BASEVALUE                        "BaseValue"   //功率封顶下限值
#define PROTERY_POWERCAP_TOPVALUE                        "TopValue"     //电源总输出功率
#define PROTERY_POWERCAP_BASEVALUETIME                  "BaseValueTime" //功率封顶下限值计算次数
#define PROTERY_POWERCAP_BASEVALUETOLERANCE             "BaseValueTolerance"    //功率封顶下限值计算
#define PROTERY_POWERCAP_REVISEPARAM                    "ReviseParam"   //功率封顶下限值计算系数
#define PROTERY_POWERCAP_NPUCAPSUPPORT                  "CapSupportMask"
#define PROTERY_POWERCAP_NPUCAPSTARTPARAM               "CapStartParam"
#define PROTERY_POWERCAP_NPUCAPSTOPPARAM                "CapStopParam"
#define PROTERY_POWERCAP_NPUCAPRECOVERPARAM             "CapRecoverParam"
#define METHOD_POWERCAP_ENABLE                        "SetEnable"
#define PROTERY_POWERCAP_PRESET_LIMITVALUE              "PresetLimitValue"  // 功率封顶预设值
#define METHOD_POWERCAP_LIMITVALUE                        "SetLimit"  //功率封顶值
#define METHOD_PRESET_POWERCAP_LIMITVALUE              "PresetLimit"  /* 预设功率封顶值值，当FusionPoD节点和RM210通讯丢失后可能采用预设值自动开启功耗封顶 */
#define METHOD_ACTIVATE_PRESET_LIMIT                   "ActivatePresetLimit"
#define METHOD_POWERCAP_SETFAILED                      "SetFailed"
#define METHOD_POWERCAP_FAILACTION                        "SetFailAction"  //功率封顶进一步动作
#define METHOD_POWERCAP_SETPT                        "SetPT"
#define METHOD_POWERCAP_GETPT                        "GetPT"
#define METHOD_POWERCAP_SETBASE                        "SetBase"
#define METHOD_POWERCAP_GETPTLIMT                        "GetPTLimit"
/* BEGIN: 2018-11-22 田鹏twx572344 问题单号：DTS2018112200734 */
#define METHOD_POWERCAP_GETLIMT_POLICY_STATUS                 "GetLimitPolicyStatus"
/* END:   2018-11-22 田鹏twx572344 问题单号：DTS2018112200734 */
#define METHOD_POWERCAP_SET_MANUL_ENABLE                 "SetPowerCappingManualSetEnable"
#define PROPERTY_POWERCAP_MANUALSETENABLE                "ManualSetEnable"
#define PROTERY_POWERCAP_MAXCAPVALUE                     "MaxCapValue"
#define PROTERY_POWERCAP_TYPE                            "CapType"
#define PROTERY_POWERCAP_NODE_POWER_SOURCE               "NodePowerSource"
#define PROTERY_POWERCAP_CAP_MODE                        "CapMode"

#define PROPERTY_POWERCAP_WHEN_STEADY  "PowerLimitWhenSteady"
#define METHOD_POWERCAP_WHEN_STEADY    "SetPowerLimitWhenSteady"

#define CLASS_NAME_SHELF_POWER_CAPPING                  "ShelfPowerCapping"
#define OBJ_NAME_SHELF_POWER_CAPPING                        "shelfPowerCapping"

#define PROTERY_SHELF_POWER_CAPPING_MODE                    "Mode"
#define PROTERY_SHELF_POWER_CAPPING_ENABLE              "Enable"
#define PROTERY_SHELF_POWER_CAPPING_VALUE                   "Value"
#define PROTERY_SHELF_POWER_CAPPING_THRESHOLD               "Threshold"
#define PROTERY_SHELF_POWER_CAPPING_MAX_THRESHOLD       "MaxThreshold"
#define PORTERY_SHELF_POWER_CAAPING_THRESHOLD_TOLERANCE "ThresholdTolerance"
#define PROTERY_SHELF_POWER_CAPPING_BLADES_BASE_VALUE   "BladesPowerCappingBaseValue"
#define PROTERY_SHELF_POWER_CAPPING_MAX_EXTRA_VALUE     "MaxExtraValue"
#define PROTERY_SHELF_POWER_CAPPING_STATE                   "State"
/* BEGIN: DTS2018020510661. Added by cwx528279, 2018/1/25 */
#define PROTERY_SHELF_POWER_CAPPING_CURRENT_POWER       "CurrentPower"
/* END: DTS2018020510661. Added by cwx528279, 2018/1/25 */

/* BEGIN: Added by l00419687, 2018/3/3 */
#define PROTERY_SHELF_POWER_CAPPING_ACTIVE_STATE        "ActiveState"
/* END: Added by l00419687, 2018/3/3 */

/* BEGIN: Modified by wangpenghui wwx382232, 2018/6/25 19:15:39   问题单号:DTS2018062611597 */
#define PROTERY_SHELF_POWER_CAPPING_MAX_EXTRA_VERIFY_VALUE   "MaxExtraVerifyValue"
/* END:   Modified by wangpenghui wwx382232, 2018/6/25 19:15:41 */
#define METHOD_SET_SHELF_POWER_CAPPING_MODE             "SetShelfPowerCappingMode"
#define METHOD_SET_SHELF_POWER_CAPPING_ENABLE               "SetShelfPowerCappingEnable"
#define METHOD_SET_SHELF_POWER_CAPPING_VALUE                "SetShelfPowerCappingValue"
#define METHOD_SET_SHELF_POWER_CAPPING_THRESHOLD            "SetShelfPowerCappingThreshod"
#define METHOD_SET_SHELF_POWER_CAPPING_MAX_THRESHOLD    "SetShelfPowerCappingMaxThreshod"
/* BEGIN: Added by l00419687, 2017-12-25, 原因:整框功率封顶配置主备同步 */
#define METHOD_SHELF_POWER_CAPPING_CONFIG_SYNC              "ShelfPowerCappingConfigSync"
/* END: Added by l00419687, 2017-12-25 */
/* BEGIN: Added by l00419687, 2018-02-02, 原因:增加整框可设置功率封顶值查询方法 */
#define METHOD_GET_SHELF_POWER_CAPPING_VALUE_RANGE          "GetShelfPowerCappingValueRange"
/* END: Added by l00419687, 2018-02-02 */

/*BEGIN : Modified by l00217452, 2017/01/18, PN : AR-0000256852-002-007 */
#define CLASS_NAME_MSPP                                                      "MSPP"
#define OBJ_NAME_MSPP                                                            "MSPP"
#define PROTERY_MSPP_ENABLE                                                          "Enable"
#define PROTERY_MSPP_RUNNING_STATE                                            "MSPPRunningState"
#define PROTERY_MSPP_NEED_POWERCAPPING                                   "NeedPowerCapping"
#define PROTERY_MSPP_POWER_FAILURE                                            "PowerFailure"
#define PROTERY_MSPP_POWER_CAPPING_STATE                               "PowerCappingState"
#define PROTERY_MSPP_CLEAR_PROHOT                                             "ClearProhot"
#define METHOD_SET_MSPP_ENABLE                                               "SetMSPPEnable"
#define METHOD_SET_SHELF_MSPP_ENABLE                                    "SetShelfMSPPEnable"
#define METHOD_SET_NEED_MSPP                                                        "SetNeedMSPP"
#define METHOD_SET_RUNNING_STATE                                                 "SetRunningState"
/*END : Modified by l00217452, 2017/01/18, PN : AR-0000256852-002-007 */
/* 电源供应power supply */
#define CLASS_NAME_PS                                           "OnePower"
#define OBJ_NAME_PS                                                "OnePower"
#define PROTERY_PS_VISABLE                                            "Visable"
#define PROTERY_PS_INDEX                                            "PsId"
#define PROTERY_PS_MANUFACTURER                        "Manufacture"
#define PROTERY_PS_INPUTMODE                                     "AcDc"
#define PROTERY_PS_MODEL                                     "PsType"
#define PROTERY_PS_VERSION                           "PsVer"
#define PROTERY_PS_RATING                                   "PsRate"
#define PROTERY_PS_STATUS                                "Health"
#define PROTERY_PS_ENTITY                                "PsEntity"
#define PROTERY_PS_INPUTPOWER                                "Power"
#define PROTERY_PS_PRESENCE                                "Presence"
#define PROTERY_PS_PROTOCAL                                "Protocol"
#define PROTERY_PS_FAILURE                                "Failure"
#define PROTERY_PS_OVERTEMP                                "OverTemp"
#define PROTERY_PS_INPUTLOSS                                "InputLoss"
#define PROPERTY_PS_UNIT_GROUP                            "PowerUnitGroup"
#define PROTERY_PS_INLETTEMP                                "InletTemp"
#define PROTERY_PS_INNERTEMP                                "InnerTemp"
#define PROTERY_PS_FORBIDDEN_WALKIN_STATE                   "ForbiddenWalkinState"
/* BEGIN: Added by l00419687, 2017-12-14, 原因:onepower类中增加电源通信状态的属性 */
#define PROTERY_PS_COMMU_STATE                            "CommunicationState"
/* END: Added by l00419687, 2017-12-14 */
#define PROTERY_PS_LOCATION                              "Location"
#define PROTERY_PS_FUNCTION                            "Function"
#define PROTERY_PS_DEVICENAME                                "DeviceName"
#define PROPERTY_ACTUAL_ACTIVE                          "ActualActive"
#define PROPERTY_DEEP_SLEEP_ACTUAL_ACTIVE               "DeepSleepActualActive"
#define PROPERTY_SOURCE_TYPE                            "SourceType"
#define PROTERY_PS_SLOT_ID                              "AnchorSlot"
#define PROTERY_PS_POWER_16BIT     "Power16"
#define PROTERY_PS_FWVERSION                           "PsFwVer"
#define PROTERY_PS_LOCALITY                             "Locality"
#define PROTERY_PS_SN                            "SN"
#define PROTERY_PS_VIN                                  "Vin"
#define PROTERY_PS_VIN_MULTI                            "VinMultiplier"
#define PROTERY_PS_PART_NUM             "PartNum"
/* BEGIN : Added by zwx536747, 2020/02/27,UADP509211 */
#define PROTERY_PS_MANUFACTURE_DATE                "ManufactureDate"
/* END   : Added by zwx536747, 2016/02/27,UADP509211 */
#define PROTERY_PS_VOUT                                 "Vout"
#define PROTERY_PS_IIN                                  "Iin"
#define PROTERY_PS_IOUT                                 "Iout"
#define PROTERY_PS_OUTPUTPOWER                          "PowerOut"
#define PROTERY_PS_POWERSUPPLY_CYCLE                    "PowerSupplyCycle"
#define PROTERY_PS_MANUFACTUREDATE                      "ManufactureDate"
#define PROTERY_PS_I_OUT_RATE                           "IoutRate"
#define PROTERY_PS_TOTAL_RUNNING_TIME                   "TotalRunningTime"
#define PROTERY_PS_FREQUENCY                            "Frequency"
#define PROTERY_PS_POWERSUPPLY_CHANNEL                  "PowerSupplyChannel"
#define PROTERY_PS_POWERSUPPLY_CFG                      "PowerSupplyCfg"
#define PROTERY_PS_VIN_CHANNEL_A                        "VinChannelA"
#define PROTERY_PS_VIN_CHANNEL_B                        "VinChannelB"
#define PROTERY_PS_INPUT_A_STATUS "PSUInputAStatus"
#define PROTERY_PS_INPUT_B_STATUS "PSUInputBStatus"
#define PROTERY_PS_BATTERY_PRES_STATE "BatteryPresenceState"

/* BEGIN: Added by gwx455466, 2017/7/4 14:53:11   问题单号:AR-0000276588-006-001 */
#define PROPETRY_PS_PART_NUM               "PartNum"
#define PROPETRY_PS_POWER_TYPE             "PowerType"
#define METHOD_PS_UPDATE_PART_NUM               "UpdatePartNum"
/* END:   Added by gwx455466, 2017/7/4 14:53:15 */
#define METHOD_PS_REPLACE_RECORD_SEL        "GenerateReplaceRecord"
#define METHOD_SET_PRE_REPLACE_INFO         "SetPreReplaceInfo"
#define METHOD_UPDATE_PS_VERSION                        "UpdatePsVer"
/* BEGIN: Added by cwx388556, 2017/9/19   问题单号:DTS2017081419406 */
#define PROPETRY_PS_UPDATEFLAG                          "UpgradeFlag"
/* END:   Added by cwx388556, 2017/9/19 */
/* BEGIN: Added by l00419687, 2017-12-25, 原因:同步电源事件相关属性 */
#define METHOD_POWER_EVENT_PROPERTY_SYNC                "PowerEventPropertySync"
#define PROTERY_PS_GET_PS_ELABEL_DATA                   "GetPsElabelData"
#define PROTERY_PS_SET_ROUTINE_FLAG                     "SetPsRoutineFlag"
/* END: Added by l00419687, 2017-12-25 */
/* 2020/06/13 zwx655588 DTS2020060909397 */
#define METHOD_ELABEL_PROPERTY_SYNC                     "ElabelPropertySync"

/* BEGIN:Added by fwx527488,2018-08-29*/
#define CLASS_PERIPHERAL_PSU_NAME                                   "PeripheralPsuSlot"
#define PROTERY_PERIPHERAL_PSU_DETECT                               "DetectFail"
/* BEGIN:Added by fwx527488,2018-08-29*/
#define PROPERTY_PERIPHERAL_ACTUAL_MODE                             "ActualMode"

#define CLASS_NAME_RIMM_PS                                          "PeripheralPower"
#define CLASS_NAME_RIMM_SYSPOWER                                    "PeripheralSysPower"
#define CLASS_NAME_RIMM_PRESENCE                                         "Presence"
#define PROTERY_PS_RIMM_INDEX                                            "PsId"
#define PROTERY_PS_RIMM_MANUFACTURER                                     "Manufacture"
#define PROTERY_PS_RIMM_INPUTMODE                                        "AcDc"
#define PROTERY_PS_RIMM_MODEL                                            "PsType"
#define PROTERY_PS_RIMM_VERSION                                          "PsVer"
#define PROTERY_PS_RIMM_RATING                                           "PsRate"
#define PROTERY_PS_RIMM_PROTOCAL                                         "Protocol"
#define PROTERY_PS_RIMM_POWEROUT                                         "PowerOut"
#define PROTERY_PS_RIMM_VIN                                              "Vin"
#define PROTERY_PS_RIMM_VOUT                                             "Vout"
#define PROTERY_PS_RIMM_IIN                                              "Iin"
#define PROTERY_PS_RIMM_IOUT                                             "Iout"
#define PROTERY_PS_RIMM_OVERTEMP                                         "OverTemp"
#define PROTERY_PS_RIMM_FAILURE                                          "Failure"
#define PROTERY_PS_RIMM_INPUTLOSS                                        "InputLoss"
#define PROTERY_PS_RIMM_ENVTEMP                                     "EnvTemp"
#define PROPERTY_PERIPHERAL_POWER_TARGET                            "Target"
#define PROTERY_PS_RIMM_SN                                          "SN"
#define PROTERY_PS_RIMM_ACTUAL_ACTIVE                                    "ActualActive"
/** BEGIN: Added by z00382825, 2017/9/27   PN:DTS2017092400751 */
#define PROTERY_PS_RIMM_POWER16                                          "Power16"
/** END: Added by z00382825, 2017/9/27   PN:DTS2017092400751 */
/* BEGIN: Added by gwx455466, 2017/7/13 15:8:59   问题单号:AR-0000276588-006-001 */
#define PROPETRY_PS_RIMM_PART_NUM               "PartNum"
/* END:   Added by gwx455466, 2017/7/13 15:9:2 */
/* BEGIN: Added by 00356691 zhongqiu, 2017/10/25   PN:DTS2017102507340  支持同步电源告警监控信息*/
#define PROPERTY_PS_RIMM_VOUT_STATUS                                     "VoutStatus"
#define PROPERTY_PS_RIMM_IOUT_STATUS                                     "IoutStatus"
#define PROPERTY_PS_RIMM_VIN_STATUS                                      "VinStatus"
#define PROPERTY_PS_RIMM_TEMP_STATUS                                     "TemperStatus"
#define PROPERTY_PS_RIMM_FAN_STATUS                                      "FanStatus"
/* END:   Added by 00356691 zhongqiu, 2017/10/25   PN:DTS2017102507340  支持同步电源告警监控信息*/
/* BEGIN: Added by cwx70046 chenhongliang, 2020/4/15 */
#define PROPERTY_PS_ALARM_STATUS                                      "AlarmStatus"
/* END:   Added by cwx70046 chenhongliang, 2020/4/15 */
#define PROPERTY_PS_OUTPUT_LIMIT_POWER                     "OutputLimitPower"
#define METHOD_UIDLED_STATE                                "SetUIDLedIdentify"


/* DftCtrl DFT装备控制 */
#define CLASS_NAME_DFT_CTRL            ("DftCtrl")
#define PROTERY_DFT_CTRL_ADDR       ("RegAddr")
#define PROTERY_DFT_CTRL_VALUE      ("RegVal")
#define PROTERY_DFT_CTRL_FRU_ID        ("FruId")

#define CLASS_NAME_DFT_RSEV_CAHN                  "ReserveChanTest"
#define PROTERY_RSEV_CAHN_9555IN1                 "9555In1"
#define PROTERY_RSEV_CAHN_9555IN2                 "9555In2"
#define PROTERY_RSEV_CAHN_9555OUT1              "9555out1"
#define PROTERY_RSEV_CAHN_9555OUT2              "9555out2"
#define PROTERY_RSEV_CAHN_CPLDIN                 "cpldin"
#define PROTERY_RSEV_CAHN_CPLDOUT              "cpldout"

/*PME平台配套功能*/
#define CLASS_NAME_PME_SERVICECONFIG           "PMEServiceConfig"
#define OBJ_NAME_PME_SERVICECONFIG             "PMEServiceConfig"
#define PROTERY_PME_SERVICECONFIG_KVM          "KVMEnable"
#define PROTERY_PME_SERVICECONFIG_VNC          "VNCEnable"
#define PROTERY_PME_SERVICECONFIG_IBMA_ENABLE  "iBMAOnBoardEnable"

/* 不归Raid卡管理的盘 */
#define CLASS_DRIVE_IDENTIFY_SHIELD         "DriveIdentifyShield"
#define PROTERY_EXCLUDE_HDD                 "ExcludeHdd"
#define PROTERY_IS_VALID                    "IsValid"

/* BEGIN: Added by hucheng 0098892, 2017/9/4   问题单号:AR-0000276587-001-006 */
#define PROTERY_PME_SERVICECONFIG_UMS          "VirtualUmsEnable"
/* END:   Added by hucheng 0098892, 2017/9/4 */

#define PROTERY_PME_SERVICECONFIG_SP_SERVICE_OS_ENABLE        "SPServiceOSEnable"

/* BEGIN: Added by liumancang, 2017/2/24 10:37:27  PN:AR-0000276582-003-004 */
#define PROTERY_PME_SERVICECONFIG_HTTPS          "HTTPSEnable"
#define PROTERY_PME_SERVICECONFIG_IPMI          "IPMILanEnable"
/* END:   Added by liumancang, 2017/2/24 10:37:48*/
/* BEGIN: Modified by zhanglei wx382755, 2017/5/18   PN:DTS2017033104566 */
#define PROTERY_PME_SERVICECONFIG_HTTP          "HTTPEnable"
#define PROTERY_PME_SERVICECONFIG_SSH            "SSHEnable"
/* END:   Modified by zhanglei wx382755, 2017/5/18 */
/* BEGIN: Added by z00356691 zhongqiu, 2017/4/27   PN:DTS2017041100345 */
#define PROPERTY_PME_SERVICECONFIG_REDFISH     "RedfishEnable"
/* END:   Added by z00356691 zhongqiu, 2017/4/27   PN:DTS2017041100345 */
#define PROTERY_PME_SERVICECONFIG_WEB          "WEBEnable"
#define PROTERY_PME_SERVICECONFIG_TRAP         "TrapEnable"
#define PROTERY_PME_SERVICECONFIG_SNMPD        "SnmpdEnable"
#define PROTERY_PME_SERVICECONFIG_SSDP         "SSDPEnable"
#define PROTERY_PME_SERVICECONFIG_LLDP         "LLDPSupport"
#define PROTERY_PME_SERVICECONFIG_X86          "X86Enable"
#define PROTERY_PME_SERVICECONFIG_SSPC         "SupportSysPowerCtrl"
/* BEGIN: Added by c00149416, DTS2017021500425 2017/2/16 */
#define PROTERY_PME_SERVICECONFIG_RESOURCEMONITOR  "SystemResourceMonitorEnable"
#define PROTERY_PME_SERVICECONFIG_ARM          "ARMEnable"
/* END:   Added by c00149416, 2017/2/16 */
#define PROTERY_PME_SERVICECONFIG_BIOS_PFR_SUPPORT          "BIOSPFRSupport"
#define PROTERY_PME_SERVICECONFIG_WOL          "WOLConfigEnable"
#define PROTERY_PME_SERVICECONFIG_POWER_CAPPLING          "PowerCappingSupport"
#define PROTERY_PME_SERVICECONFIG_COOLING      "CoolingEnable"
#define PROTERY_PME_SERVICECONFIG_POWER      "PowerActiveStandbyEnable"
/* BEGIN: Modified by cwx528279, 2018/3/28   PN: AR.SR.SFEA02119721.025.001*/
#define PROTERY_PME_SERVICECONFIG_DEEP_SLEEP_ENABLE     "PowerDeepSleepEnable"
/* END: Modified by cwx528279, 2018/3/28   PN: AR.SR.SFEA02119721.025.001*/
#define PROTERY_PME_NORMAL_AND_REDUNDANCY_SUPPORT     "NormalAndRedundancySupport"
#define PROTERY_PME_PSU_SOURCE_SWITCH_SUPPORT     "PSUSourceSwitchSupport"
#define PROPERTY_PME_LOCAL_PCIE_MGNT_SUPPORT            "LocalPcieMgntSupport"
#define  PROTERY_PME_LLDP_OVER_NCSI_ENABLE  "LldpOverNcsiEnable"
#define  PROTERY_PME_IRM_HEART_BEAT_ENABLE  "IRMHeartBeatEnable"
#define PROPERTY_PME_HARDWARE_ROOTKEY_SUPPORT   "HardwareRootKeySupport"

#define PROTERY_PME_SERVICECONFIG_RTC          "RTCEnable"
#define PROTERY_PME_SERVICECONFIG_PSU          "PSUEnable"
/*BEGIN: Added by h00164462, 2015/5/8, PN:DTS2015012901068*/
#define PROTERY_PME_SERVICECONFIG_NETCONFIG    "NetConfigEnable"
/*END: Added by h00164462, 2015/5/8, PN:DTS2015012901068*/
/* BEGIN for ARB7F63698-2463-45ba-AEEB-9A0B21853DA2 : Added by ouluchun 00294050 2015-11-24 */
#define PROTERY_PME_SERVICECONFIG_PS_UPGRADE    "PSFirmwareUpgradeEnable"
/* END for ARB7F63698-2463-45ba-AEEB-9A0B21853DA2 : Added by ouluchun 00294050 2015-11-24 */

/* BEGIN: Added by h00272616, dizhihua_snmp_iod 2016/9/9    */
#define PROPERTY_PME_SERVICECONFIG_CUSTOM_SNMPOID   "CustomSNMPOID"
#define METHOD_PME_SERVICECONFIG_SET_SNMPOID  "SetCustomSNMPOID"
#define METHOD_PME_SERVICECONFIG_SET_POWERCAPPINGSUPPORT "SetPowerCappingSupport"
/* END: Added by h00272616, dizhihua_snmp_iod 2016/9/9    */

/* BEGIN: Added by 00356691 zhongqiu, 2018/5/21   PN:AR.SR.SFEA02130925.003.001 支持采集的数据列表*/
#define PROPERTY_PME_SERVICECONFIG_DATA_ACQUISITION_SUPPORT        "DataAcquisitionSupport"
/* END:   Added by 00356691 zhongqiu, 2018/5/21   PN:AR.SR.SFEA02130925.003.001 支持采集的数据列表*/
/* FPC接口服务功能 */
#define PROPERTY_PME_SERVICECONFIG_FPC_SUPPORT          "FPCSupport"
#define PROPERTY_PME_SERVICECONFIG_SYSTEM_LOCKDOWN_SUPPORT 	"SystemLockDownSupport"

/* BEGIN: AR.SR.SFEA02130924.022.001  . Modified by lichangdi, 2018/6/7 */
#define PROPERTY_PME_SERVICECONFIG_JAVA_KVM_ENABLE                              "JavaKVMEnable"
#define PROPERTY_PME_SERVICECONFIG_JAVA_VIDEO_ENABLE                                    "JavaVideoEnable"
/* END: AR.SR.SFEA02130924.022.001  . Modified by lichangdi, 2018/6/7 */

#define PROPERTY_PME_SERVICECONFIG_ENERGY_SAVING_SUPPORT                   "EnergySavingSupport"

#define PROPERTY_PME_SERVICECONFIG_BOARD_INFO_COLLECT_ENABLE                   "BoardInfoCollectEnable"
#define PROPERTY_PME_SERVICECONFIG_BOARD_INFO_COLLECT_STATUS                   "BoardInfoCollectStatus"
#define METHOD_PME_SERVICECONFIG_BOARD_INFO_COLLECT_ENABLE                     "SetBoardInfoCollect"

/* BEGIN: Modified by zhangxiong z00454788, 2019/7/27   问题单号:DTS2019072706597 【iRM】升级到新版后，串口初始化映射关系冲突，导致资产管理板从机Modbus地址自动分配失败 */
#define PROPERTY_PME_SERVICECONFIG_SERIAL_LOOPBACK_ENABLE                   "SerialLoopbackDetectEnable"
/* END: Modified by zhangxiong z00454788, 2019/7/27   问题单号:DTS2019072706597 【iRM】升级到新版后，串口初始化映射关系冲突，导致资产管理板从机Modbus地址自动分配失败 */
// 2021.3.31 硬件是否支持b2h_irq中断，bmc to host中断是否支持的标记，host侧ipmi驱动会根据bmc侧的配置来使能或禁止b2h_irq_en状态。 0: 不支持 1：支持
#define PROPERTY_PME_SERVICECONFIG_B2H_IRQ_SUPPORT                             "B2HIrqSupport"

/* 2021-5-10 w30011073 是否支持ipmi AC掉电 0：不支持；1：支持 */
#define PROPERTY_PME_SERVICECONFIG_IPMI_AC_CYCLE_SUPPORT                    "IpmiAcCycleSupport"

#define PROPERTY_PME_SERVICECONFIG_CFG_IMPORT_EXPORT_SUPPORT                   "CfgImportAndExportSupport"

/*BEGIN: Added by t00186940, 2015/12/21, DTS:DTS2015120702574 */
#define CLASS_NAME_PME_PRODUCT          "PRODUCT"
#define PROTERY_PRODUCT_SUPPORT_PCIE_HOTPLUG      "SupportPcieHotPlug"
#define PROTERY_PME_SERVICECONFIG_BIOSPRINT          "BiosPrintEnable"
/*END: Added by t00186940, 2015/12/21, DTS:DTS2015120702574 */
#define PROTERY_PRODUCT_BMC_VALID_MODE               "BMCValidMode"   // BMC生效模式属性
#define PROTERY_UPGRADE_BMC_CURRENT_ACTIVE_MODE      "ActiveMode"   // BMC当前生效模式
#define PROTERY_UPGRADE_BIOS_CURRENT_ACTIVE_MODE     "BIOSActiveMode"   // BIOS当前升级模式
#define PROTERY_PRODUCT_SYNC_UPGRADE_ENABLE          "SyncUpgradeEnable"   // BMC当前生效模式
#define METHOD_SET_BMC_ACTIVE_MODE  "SetActiveMode"
#define METHOD_SET_BIOS_ACTIVE_MODE "SetBIOSActiveMode"

/*sendmessage转发限制功能*/
#define CLASS_NAME_PME_SENDMSGCONFIG           "PMESendMsgConfig"
#define PROTERY_SENDMSG_SRC_CHAN               "SrcChan"
#define PROTERY_SENDMSG_SRC_ADDR               "SrcAddr"
#define PROTERY_SENDMSG_DEST_CHAN              "DestChan"
#define PROTERY_SENDMSG_DEST_ADDR              "DestAddr"
#define PROTERY_SENDMSG_ENABLE                 "SendMsgEnable"
/*BEGIN: Modified by h00164462, 2015/7/31, PN:AR-0000956734*/
#define PROTERY_SENDMSG_WHITELIST_STATE        "WhiteListState"
#define PROTERY_SENDMSG_IPMICMD                "IPMICommand"

#define CLASS_NAME_IPMI_COMMAND                "IPMIOemCommand"
#define PROPERTY_IPMI_COMMAND_READWRITE        "ReadWrite"
#define PROPERTY_IPMI_COMMAND_NETFN            "NetFunction"
#define PROPERTY_IPMI_COMMAND_CHAN_PROTOCOL    "ChanProtocol"
#define PROPERTY_IPMI_COMMAND_CMD              "Command"
#define PROPERTY_IPMI_COMMAND_DATA             "Data"
#define PROPERTY_IPMI_CMMMAND_SHIEDFLAG        "ShiedFlag"
/*END: Modified by h00164462, 2015/7/31, PN:AR-0000956734*/

// IpmbEthBlade, 用于SMM板管理刀片的在位和心跳
#define CLASS_NAME_IPMBETH_BLADE               "IpmbEthBlade"
#define OBJECT_NAME_IPMBETH_BLADE1             "IpmbEthBlade1"
#define PROPERTY_IPMBETH_BLADE_ID              "Id"
#define PROTERY_IPMBETH_BLADE_SLAVEADDR        "SlaveAddr"
#define PROTERY_IPMBETH_BLADE_PRESENCE         "Presence"
#define PROTERY_IPMBETH_BLADE_COOLING_MEDIUM   "CoolingMedium"
#define PROTERY_IPMBETH_BLADE_BLADETYPE        "BladeType"
#define PROTERY_IPMBETH_BLADE_HOTSWAP          "HotSwap"
#define PROPERTY_IPMBETH_OS_BOOT_STATE         "OsBootState"

#define PROTERY_IPMBETH_BLADE_HEARTBEAT        "CommunicateLost"
#define PROTERY_IPMBETH_BLADE_REBOOTFLAG       "RebootLost"
#define PROTERY_IPMBETH_BLADE_FAILACTION       "PowerCappingFailAction"

#define PROTERY_IPMBETH_POWER_CAPPING_MANUAL_STATE      "PowerCappingManualState"
#define PROTERY_IPMBETH_POWER_CAPPING_ENABLE      "PowerCappingEnable"
#define PROTERY_IPMBETH_POWER_CAPPING_VALUE      "PowerCappingValue"
#define PROTERY_IPMBETH_POWER_CAPPING_BASE_VALUE "PowerCappingBaseValue"
#define PROTERY_IPMBETH_POWER_CAPPING_CURRENT_POWER       "CurrentPower"
/*BEGIN : Modified by l00217452, 2017/01/18, PN : AR-0000256852-002-007 */
#define PROTERY_IPMBETH_POWER_CAPPING_MAX_CAP_POWER        "PowerCappingMaxPower"
#define PROTERY_IPMBETH_POWER_CAPPING_NODE_MSPP_STATE       "NodeMSPPState"
/*END : Modified by l00217452, 2017/01/18, PN : AR-0000256852-002-007 */
/* BEGIN: Added by 00356691 zhongqiu, 2017/7/13   PN:支持SMM加载PCIe标卡*/
#define PROPERTY_IPMBETH_POWER_STATE                    "PowerState"
#define PROPERTY_IPMBETH_BOARD_WIDTH_TYPE               "BoardWidthType"
/* END:   Added by 00356691 zhongqiu, 2017/7/13   PN:支持SMM加载PCIe标卡*/
#define PROPERTY_IPMBETH_BOARD_MANAGED               "ManagedByMM"

/*BEGIN : Modified by d00384307, 2017/12/05, smm 需要表示单板的冷却介质 */
#define PROTERY_IPMBETH_BLADE_COOLING_MEDIUM       "CoolingMedium"
/*END : Modified by d00384307, 2017/12/05, smm 需要表示单板的冷却介质 */

/*BEGIN : Modified by c00416525, 2018/8/17, EM 需要对单板进行受控上电 */
#define PROTERY_IPMBETH_BLADE_CONTROLLED_POWER_ON       "ControlledPowerOn"
/*END : Modified by c00416525, 2018/8/17 */

#define METHOD_IPMBETH_NODE_PPOWER_WORK_MODE        "SetNodePowerWorkMode"

#define PROTERY_IPMBETH_NODE_UIDSTATE       "NodeUIDState"
#define METHOD_IPMBETH_NODE_SET_UIDSTATE        "SetNodeUIDState"

#define METHOD_SET_BLADE_POWER_CAPPING_VALUE            "SetPowerCappingValue"
#define METHOD_SET_BLADE_FAIL_ACTION                        "SetFailAction"
#define METHOD_IPMBETH_PROPERSYNC_METHOD                    "PropertySyncMethod"
#define METHOD_IPMBETH_SETMSPPSTATE_METHOD                    "SetNodeMSPPState"
#define METHOD_IPMBETH_BLADE_UPDATE_HEARTBEAT  "UpdateHeartBeat"
#define METHOD_IPMBETH_BLADE_FRU_CONTROL        "BladeFruControl"
#define METHOD_IPMBETH_BLADE_GETFRUHOTSWAPSTATE     "GetFruHotSwapState"
#define METHOD_IPMBETH_BLADE_SEND_CHASSISID  "SendChassisNumber"
/* BEGIN: Added by l00419687, 2018-02-02 */
#define METHOD_IPMBETH_BLADE_GET_CAPPING_VALUE_RANGE     "GetPowerCappingValueRange"
/* END: Added by l00419687, 2018-02-02 */


/* BEGIN: Added by l00419687, 2018-06-04, 控制单板上下电 */
#define METHOD_SET_BLADE_BMC_POWERTSATE              "SetBladePowerState"
/* END: Added by l00419687, 2018-06-04 */
#define PROTERY_IPMBETH_BLADE_HEARTBEAT_LIVE   0
#define PROTERY_IPMBETH_BLADE_HEARTBEAT_LOST   1

#define METHOD_SHELF_GET_BLADE_VLAN_ID_RANGE        "GetVLANIDRange"
#define METHOD_SHELF_GET_BLADE_MNGTPORTCAP          "GetBladeMngtPortCap"
#define METHOD_SET_BLADE_BMC_NETMODE                "SetBladeNetMode"
#define METHOD_SET_BLADE_BMC_VLAN_INFO              "SetBladeVLANInfo"
#define METHOD_SET_BLADE_BMC_ACTIVE_PORT            "SetBladeActivePort"
/*BEGIN : Modified by c00416525, 2017/12/18, MM920需求，SMM板查询和配置单板信息 */
#define METHOD_SHELF_GET_BLADE_INFO                 "GetBladeInfo"
#define METHOD_SHELF_GET_BLADE_LANCONFIG            "GetBladeLanConfig"
#define METHOD_SHELF_GET_BLADE_IPMODE               "GetBladeIpMode"
#define METHOD_SHELF_GET_BLADE_IP6MODE              "GetBladeIp6Mode"
#define METHOD_SET_BLADE_BMC_IPADDR                 "SetBladeIpAddr"
#define METHOD_SET_BLADE_BMC_SUBNETMASK             "SetBladeSubNetMask"
#define METHOD_SET_BLADE_BMC_GATEWAY                "SetBladeGateWay"
#define METHOD_SET_BLADE_BMC_IP6ADDR                "SetBladeIp6Addr"
#define METHOD_SET_BLADE_BMC_IP6PREFIX              "SetBladeIp6Prefix"
#define METHOD_SET_BLADE_BMC_IP6GATEWAY             "SetBladeIp6GateWay"
#define METHOD_SET_BLADE_BMC_IPMODE                 "SetBladeIpMode"
#define METHOD_SET_BLADE_BMC_IP6MODE                "SetBladeIp6Mode"
#define METHOD_GET_BLADE_BMC_USERNAME               "GetBladeUserName"
#define METHOD_GET_BLADE_BMC_USERPRIVILEGE          "GetBladeUserPrivilege"
#define METHOD_SET_BLADE_BMC_USERPRIVILEGE          "SetBladeUserPrivilege"
#define METHOD_SET_BLADE_BMC_USERPWD                "SetBladeUserPassword"
#define METHOD_SET_BLADE_BMC_ADDUSER                "SetBladeAddUser"
#define METHOD_SET_BLADE_BMC_DELUSER                "SetBladeDelUser"
/*END : Modified by c00416525, 2017/12/18, MM920需求，SMM板查询和配置单板信息 */

#define METHOD_GET_BLADE_PRESENT_FRU   "GetBladePresentFru"
#define METHOD_DELETE_SWI_CFG_FILE      "DeleteSwiCfgFile"
/* BEGIN: Added by zwx559561, 2019/3/7   问题单号:UADP109378 */
#define METHOD_RESTORE_SWI_CFG_FILE      "RestoreSwiCfgFile"
#define METHOD_DELETE_INVALID_SWI_CFG_FILE      "DeleteInvalidSwiCfgFile"
#define METHOD_PARDE_SWI_CFG_FILE      "ParseSwiCfgFile"
/* END:   Modified by zwx559561, 2019/3/7 */
#define METHOD_OFFLINE_SWI_CFG_FILE         "OfflineLoadSwiCfgFile"

#define CLASS_NAME_SHELF_MANAGE                          "ShelfManage"
#define OBJ_NAME_SHELF_MANAGE                              CLASS_NAME_SHELF_MANAGE
#define PROTERY_SHELF_RATED_POWER_CTRL_EN       "RatedPowerCtrlEn"
#define PROTERY_SHELF_STAGGR_POWERDELAYTIME   "StaggerPowerDelayTime"
#define PROTERY_SHELF_MAX_STAGGR_TIME                "MaxStaggerTime"
#define PROTERY_SHELF_MIN_STAGGR_TIME                 "MinStaggerTime"
#define PROPERTY_SHELF_HEALTH                  "Health"
#define PROTERY_SHELF_MANAGEMENT_STATE                 "ShelfManagementState"
#define PROTERY_SHELF_MANAGE_DFT_BACK_PLANE_TYPE       "DftBackPlaneType"
#define METHOD_SET_SHELF_RATED_POWER_CTRL_EN "SetRatedPowerCtrlEn"
#define METHOD_SYNC_SET_SHELF_POWER_CTRL_EN   "SyncSetRatedPowerCtrlEn"
#define METHOD_GET_SHELF_RATED_POWER_CTRL_EN "GetRatedPowerCtrlEn"
#define METHOD_SET_STAGGER_POWER_DELAY_TIME   "SetStaggerPowerDelayTime"
#define METHOD_SYN_SET_POWER_DELAY_TIME           "SyncSetStaggerPowerDelayTime"
#define METHOD_GET_STAGGER_POWER_DELAY_TIME   "GetStaggerPowerDelayTime"
#define METHOD_PUT_EVENT_QUE                                  "PutEventQue"
#define METHOD_ADD_SEL_ENTRY_HNDL                        "AddSelEntryHndl"
#define METHOD_SET_TIME_OR_TIMEZONE_TO_BLADE   "SetTimeOrTimeZoneToBlade"
#define METHOD_SET_SHELF_INNER_SUBNET_SEGMENT         "SetShelfInnerSubnetSegment"

/*BEGIN : Modified by c00416525, 2018/08/17, MM920需求*/
#define PROTERY_SHELF_CONTROL_POWER_ON_TIMEOUT       "ControlledPowerOnTimeout"              //EM对单板进行受控上电的等待超时时间
#define METHOD_SET_BLADE_CONTROLLED_POWER_ON_INFO    "SetBladeControlledPoweronInfo"         //设置单板受控上电信息
#define METHOD_SET_CONTROLLED_POWER_ON_TMOUT         "SetBladeCtlPoweronTMOut"      //设置单板受控上电等待超时时间
#define METHOD_SET_BLADE_POWER_STATE                    "SetBladePowerOnEnabled"             //允许单板上电
/*END : Modified by c00416525, 2018/08/17 */

#define CLASS_NAME_MM_LSW_MGNT                                      "MMLswMgnt"
#define OBJ_NAME_MM_LSW_MGNT                             CLASS_NAME_MM_LSW_MGNT
#define PROTERY_OUT_PORT_MODE                                     "OutPortMode"
#define PROTERY_ASCONFIG_STATE                                  "ASConfigState"
#define PROTERY_CASCADE_PORT_STATE                           "CascadePortState"
#define PROTERY_UP_PORT_POLICY                                   "UpPortPolicy"
#define PROTERY_DOWN_PORT_POLICY                               "DownPortPolicy"
#define PROTERY_CASCADE_PORT_POLICY                         "CascadePortPolicy"
#define PROTERY_SEND_HEART_ETH                                   "SendHeartEth"
#define PROTERY_RECV_HEART_PORT                                 "RecvHeartPort"
#define PROTERY_LSW_STATUS                                          "LswStatus"
#define PROTERY_OUT_PORT_STATUS                                 "OutPortStatus"
#define PROTERY_IN_VLANID                                            "InVlanID"
#define PROTERY_OUT_VLANID                                          "OutVlanID"
#define PROTERY_PORT_COUNT                                          "PortCount"
#define PROTERY_DFT_BACK_PLANE_TYPE                          "DftBackPlaneType"
#define PROTERY_DFT_DEFAULT_OPEN_PORT                      "DftDefaultOpenPort"
#define PROTERY_DFT_DEFAULT_PORT_SPEED                    "DftDefaultPortSpeed"

#define METHOD_SET_OUT_PORT_MODE                             "SetOutPortMode"
#define METHOD_SET_CASCADE_PORT_STATE                   "SetCascadePortState"
#define METHOD_SYNC_SET_CASCADE_PORT_STATE          "SyncSetCascadePortState"
#define METHOD_SYNC_MMLSWMGNT_PROTERTY                "SyncMmlswMgntProperty"
#define METHOD_GET_LSW_PORT_INFO                             "GetLswPortInfo"

#define METHOD_GET_LSW_AGETEST_STATUS                   "GetLswAgetestStatus"
#define METHOD_SET_LSW_AGETEST                                "SetLswAgetest"

#define CLASS_NAME_LSW_PORT_INFORM                              "LswPortInform"
#define PROTERY_PORT_NAME                                            "PortName"
#define PROTERY_PORT_NUM                                              "PortNum"
#define PROTERY_PORT_TYPE                                            "PortType"
#define PROTERY_PORT_STATE                                          "PortState"
#define PROTERY_PORT_PRIORITY                                    "PortPriority"
#define PROTERY_PORT_SELECT                                        "PortSelect"
#define METHOD_SYNC_PORT_SELECT_PROT                    "SyncPortSelectProtery"
#define METHOD_GET_PORT_STATISTICS                          "GetPortStatistics"
/* END:   Added by h00256973, 2017/12/14 */

#define METHOD_GET_BLADE_ALARM_EVENT_MSG_LIST   "GetBladeAlarmEventMsgList"
#define METHOD_SHELF_SET_SHELF_MANAGEMENT_STATE   "SetShelfManagementState"
#define METHOD_SYNC_SHELF_MANAGE_STATUS         "ShelfManageStateSync"
#define METHOD_SHELF_CLEAR_BLADE_ALARM_EVENT_MSG_LIST "ClearBladeAlarmEventMsgList"
#define METHOD_SYNC_SHELF_MANAGE_SAVED_PROP         "SavedPropertySyncMethod"

// SMM板管理下发平台事件
#define CLASS_NAME_PLATFORM_EVENT_CONFIG   "PlatformEventConfig"
#define PROTERY_PLATFORM_EVENT_SENSOR_NUMBER  "SensorNumber"
#define PROTERY_PLATFORM_EVENT_SENSOR_VALUE    "SensorValue"
#define PROTERY_PLATFORM_EVENT_TYPE           "type"
#define PROTERY_PLATFORM_EVENT_SENSOR_TYPE    "SensorType"
#define PROTERY_PLATFORM_EVENT_EVENT_READING_TYPE  "EventReadingType"
#define PROTERY_PLATFORM_EVENT_DEASSERT_MASK       "DeassertionMask"
/* DFT类 */
#define CLASS_DFT_STATUS                        "DftStatus"         /* DFT测试状态 */
#define PROPERTY_DFT_ENABLE                     "DftEnable"         /* 装备使能状态 */
#define PROPERTY_CPLD_DFT_ENABLE                "DftCpldEnable"
/*BEGIN:Added by h00164462, 2015/3/17, PN:深度恢复出厂配置*/
#define PROPERTY_DFT_MODE_FLAG                  "DftModeFlag"
#define METHOR_SET_DFT_MODE_FLAG                "SetDftModeFlag"
/* BEGIN: Added by zengrenchang(z00296903), 2017/2/13   问题单号:DTS2017021311169 */
#define METHOR_SET_FT_MODE_FLAG                 "SetFTModeFlag"
#define PROPERTY_FT_MODE_FLAG                   "FTModeFlag"
#define PROPERTY_FT_IDENTIFY_TYPE               "FTIdentifyType"
#define PROPERTY_FT_UART_PORT                   "FTUartPort"
#define PROPERTY_ALARM_DEASSERT_ENABLE          "AlarmDeassertEnable"
#define PROPERTY_FT_ETH_PORT                    "FTEthPorts"
/* END:   Added by zengrenchang(z00296903), 2017/2/13 */

#define METHOR_SET_CUSTOMER_RESTRICTED_IPMI_LIST "SetIPMICmdList"
/*END:Added by h00164462, 2015/3/17, PN:深度恢复出厂配置*/
#define METHOR_SET_CUSTOMER_WHITE_IPMI_LIST "SetWhiteIPMICmdList"
/* BEGIN: Added for PN:AR56AFEBDA-8035-4720-8228-519A01DD6AB3 by l00205093, 2015/10/29 */
#define PROPERTY_RESTRITED_IPMI_CMD_LIST         "RestrictedIPMICmdList"
#define PROPERTY_RESTRITED_CLASS_METHOD_LIST     "RestrictedClassMethodList"
/* END:   Added by l00205093, 2015/10/29 */
/* BEGIN: Add for by y00383654 2016/11/28*/
#define PROPERTY_DYNAMIC_CUSTOMER_RESTRICTED_IPMI_CMD_LIST "DynamicRestrictedIPMICmdList" /*用户自定义屏蔽命令列表*/
/* END:   Add by y00383654 2016/11/28*/
#define PROPERTY_DYNAMIC_WHITE_IPMI_CMD_LIST "DynamicWhiteIPMICmdList"
/* BEGIN: Added for PN:AR-0000256852-002-015 by cwx290152, 2016/08/13 */
#define PROPERTY_DFT_PACKAGE_FILENAME           "DftPackageFilename"
/* END:   Added by cwx290152, 2016/08/13 */
#define METHOD_DFT_FTMODE_PROPERTY_SYNC         "PropertySyncMethod"

#define CLASS_DFT_IP_STORE                                  "DftIpStore"
#define PROPERTY_DFT_IP_STORE_IPADDR1                       "Ipaddr1"
#define PROPERTY_DFT_IP_STORE_IPADDR2                       "Ipaddr2"
#define PROPERTY_DFT_IP_STORE_IPADDR3                       "Ipaddr3"
#define PROPERTY_DFT_IP_STORE_IPADDR4                       "Ipaddr4"

#define CLASS_DFT_I2C_TEST                      "I2CTest"           /* I2C测试 */
#define CLASS_DFT_VERSION                       "DftVersion"
#define PROPERTY_VERSION_TYPE                   "VersionType"
#define PROPERTY_VERSION_STR                    "Version"
#define PROPERTY_UNIT_NUM                       "UnitNum"
#define PROPERTY_NEED_UNIT_NUM                  "NeedUintNum"
#define PROPERTY_FRU_ID                         "FruId"

/* BEGIN: Added by huanghan (h00282621), 2014/8/12 */
#define CLASS_NAME_HARDWARE_CHAN_TEST       "InterfaceTest"
#define PROTERY_HARDWARE_IF_TYPE                   "IFType"
#define PROTERY_HARDWARE_TEST_TYPE                 "TestType"
#define PROTERY_HARDWARE_INPUT                      "Input"
#define PROTERY_HARDWARE_OUTPUT                   "Output"
#define PROTERY_HARDWARE_EXPECT_VALUE        "InputValue"
#define PROTERY_HARDWARE_CONTROL_VALUE        "OutputValue"
#define PROTERY_HARDWARE_OPERATOR             "Operator"
/* END:   Added by huanghan (h00282621), 2014/8/12 */


/* BEGIN: Added by zhanglipeng (z90004119), 2016/4/15 */
#define CLASS_NAME_TRANSFERFILE                "TransferFile"
#define OBJ_NAME_TRANSFERFILE                  CLASS_NAME_TRANSFERFILE
#define PROTERY_IS_TRANSFERING                 "IsTransfering"
#define METHOD_INITRIAL_FILE_TRANSFER          "InitialFileTransfer"
#define METHOD_GET_TRANSFER_STATUS             "GetTransferStatus"
#define METHOD_GET_FILE_AUTH_STRING            "GetFileAuthString"

#define CLASS_NAME_TRANSFERFILE_FILE_INFO       "TransferFileInfo"
#define PROTERY_TRANSFERFILE_FILE_ID            "ID"
#define PROTERY_TRANSFERFILE_FILE_NAME          "FileName"
#define PROTERY_TRANSFERFILE_FILE_PATH          "FilePath"
#define PROTERY_TRANSFERFILE_DOWNLOAD           "Download"
#define PROTERY_TRANSFERFILE_UPLOAD             "Upload"
#define PROTERY_TRANSFERFILE_MAXLENGTH          "Maxlength"
#define PROTERY_TRANSFERFILE_UPDATE_FLAG        "UpdateFlag"
#define PROTERY_TRANSFERFILE_FILE_AUTHORITY     "FileAuth"
/* END:   Added by zhanglipeng (z90004119), 2016/4/15 */


#define CLASS_VIRTUAL_WIRE                      "VirtualWire"
#define PROPERTY_VW_CONDITION                   "Condition"
#define PROPERTY_VW_DESTINATION                 "Destination"
#define PROPERTY_VW_DATA                        "Data"
/* BEGIN: Added by 00381887 dingzhixiong, 2018/3/6   PN:SR.SF-0000276585.006 支持机框部件的故障汇聚节点BMC*/
#define CLASS_RPC_REST_VIRTUAL_WIRE            "RpcResetVirtualWire"
/* END:   Added by 00381887 dingzhixiong, 2018/3/6 */
/* BEGIN: Added by dingzhixiong, 2018/3/9,  DTS2018030910618  PCIe SSD基于I2cOverLan实现告警优化的扫描条件，未关联RPC状态，提单优化 */
#define PROPERTY_RPC_RESET_VW_SLEEP_TIME       "SleepTime"
/* END:   Added by dingzhixiong, 2018/3/9 */

#define CLASS_PERIPHERAL_FIRMWARE_NAME                   "PeripheralFirmware"
#define CLASS_UPGRADE_SOFTWARE_NAME                      "Firmware"
#define CLASS_UPGRADE_MCU_SOFTWARE_NAME                  "MCUFirmware"
#define PROPERTY_SOFTWARE_PREPAREACTION                  "PrepareAction"
#define PROPERTY_SOFTWARE_FINISHACTION                   "FinishAction"
#define PROPERTY_SOFTWARE_CLASSIFICATIONS                "Classifications"
#define PROPERTY_SOFTWARE_COMPONENTIDEX                "ComponentIDEx"
#define PROPERTY_SOFTWARE_MAJORVERSION                   "MajorVersion"
#define PROPERTY_SOFTWARE_MINORVERSION                   "MinorVersion"
#define PROPERTY_SOFTWARE_VERSIONSTRING                  "VersionString"
#define PROPERTY_SOFTWARE_ISACTIVE                       "IsActive"
#define PROPERTY_SOFTWARE_INSTANCEID                     "InstanceID"
#define PROPERTY_SOFTWARE_NAME                           "Name"
#define PROPERTY_SOFTWARE_RELEASEDATE                    "ReleaseDate"
#define PROPERTY_SOFTWARE_LOCATION                       "Location"
#define PROPERTY_PERIPHERAL_FIRMWARE_TARGET              "Target"
#define PROPERTY_SOFTWARE_REVISION                       "RevisionNumber"
/* BEGIN: Added for 问题单号: by lwx459244, 2017/12/13 */
#define METHOD_SOFTWARE_SET_FRMWARE_VERSION                    "SetFirmwareVersion"
/* END:   Added for 问题单号: by lwx459244, 2017/12/13 */
/* BEGIN: Added by 00356691 zhongqiu, 2018/1/5   PN:AR.SR.SFEA02119723.011.001 节点BMC查询机框固件的版本信息*/
#define PROPERTY_SOFTWARE_UNITNUM                        "UnitNum"
/* END:   Added by 00356691 zhongqiu, 2018/1/5   PN:AR.SR.SFEA02119723.011.001 节点BMC查询机框固件的版本信息*/
#define PROPERTY_COMPONENT_UID                            "Uid"
#define PROPERTY_CHIP_CFG_GETSWITCHACTION                 "GetSwitchAction"
/* BEGIN: Added for MM920风扇升级，by c00416525 2018/5/29 */
#define CLASS_SMM_FAN_UPGRADE_FIRMWARE_NAME              "SmmFanFirmware"
#define PROPERTY_SOFTWARE_UPGRADE_PROGRESS               "UpgradeProgress"
/* END: Added for MM920风扇升级，by c00416525 2018/5/29 */
#define CLASS_HINIC_FIRMWARE                             "HinicFirmware"
#define PROP_HINIC_LAST_FW_VER_STR                       "LastVersionString"

#define CLASS_CPLD_UPGRADE_FIRMWARE_NAME                 "CPLDFirmware"
#define MAIN_BOARD_CPLD_OBJ_NAME                         "CpldMainBoardUpgrade"
#define PROPERTY_SOFTWARE_RESUME_ENABLE                  "ResumeEnable"
#define PROPERTY_SOFTWARE_BACKUP_MODE                    "BackupMode"
#define PROPERTY_SOFTWARE_RESUME_FILE                    "ResumeFilePath"
#define PROPERTY_SOFTWARE_REMAP_ENABLE                   "RemapBoardIDEnable"
#define PROPERTY_SOFTWARE_REMAP_BOARDID_LIST             "BoardIDList"
#define PROPERTY_CPLD_ACBIVE_MODE                        "ActiveMode"
#define PROPERTY_CPLD_FW_STATUS                          "CPLDFirmwareStatus"
#define PROPERTY_CPLD_FW_BRD_ID                          "BoardId"

#define CLASS_CHIP_CFG_MGNT_NAME                         "ChipConfigMgnt"
#define PROPERTY_CHIP_CFG_PREPAREACTION                  "PrepareAction"
#define PROPERTY_CHIP_CFG_FINISHACTION                   "FinishAction"

#define CLASS_UPGRADE_VERFIY_ACTION_NAME                 "VerifyAction"
#define PROPERTY_UPGRADE_VERFIY_ACTION_EXPECT_DATA       "ExpectData"
#define PROPERTY_UPGRADE_VERFIY_ACTION_VERIFY_SOURCE     "VerifySource"
#define PROPERTY_UPGRADE_VERFIY_ACTION_CLEAR_DATA        "ClearData"

#define CLASS_UPGRADE_SFC_ACTION                         "SfcAction"
#define PROPERTY_UPGRADE_ACTION_CS                       "ChipSelect"

#define CLASS_UPGRADE_ACTION_NAME                        "Action"
#define CLASS_UPGRADE_VRD_ACTION_NAME                    "VRDAction"
#define PROPERTY_UPGRADE_ACTION                          "Destination"
#define PROPERTY_UPGRADE_ACTION_DATA                     "Data"
#define METHOD_SET_ACTION_DATA                           "SetActionData"
#define OBJECT_BIOS_SWITCH_LOCK_ACTION                        "BiosSwitchLockAction"
/* BEGIN: Added by chenshihao cwx398307, 2018/03/28   问题单号:DTS2018031405575 */
#define CLASS_HDD_LOCATION_STATUS                  "HDDLocationStatus"
#define ALL_PD_LOCATE_STATE                                  "AllPDLocate"
/* END:   Added by chenshihao cwx398307, 2018/03/28 问题单号:DTS2018031405575 */

#define PROPERTY_VRD_ACTION_INDDEX_ID                    "IndexId"
#define PROPERTY_CONFIG_DATA                             "ConfigData"

#define CLASS_COMPONENTFIRMWARE_NAME "ComponentFirmware"
#define PROPETRY_COMPONENTFIRMWARE_PRESENCE "Presence"
#define PROPETRY_COMPONENTFIRMWARE_BOARDID "BoardId"
#define PROPETRY_COMPONENTFIRMWARE_SLOT "Slot"
#define PROPETRY_COMPONENTFIRMWARE_REF_FIRMWARE "RefFirmware"
#define PROPETRY_COMPONENTFIRMWARE_UPDATE_STATUS "UpdateStatus"
#define PROPETRY_COMPONENTFIRMWARE_REF_VIRTUAL_CONNECTOR "RefVirtualConnector"
#define PROPETRY_COMPONENTFIRMWARE_COMPONENT_ID "ComponentID"

/*BEGIN  ADD by z00397989 AR-0000276595-003-016  2017.04.20 */
#define CLASS_PCIE_SWITCH_ACTION                "PcieSwitchAction"
#define PROPERTY_PSW_ACTION_DESTINATION         "Destination"
#define PROPERTY_PSW_ACTION_DATA            "Data"
#define PROPERTY_PSW_ACTION_NAME            "Name"
#define PROPERTY_PSW_ACTION_OFFSET          "Offset"
#define PROPERTY_PSW_ACTION_PSWACTIONDATA          "PSWActionData"
/*END  ADD by z00397989 AR-0000276595-003-016  2017.04.20 */

/* BEGIN: Modified by zhangfuhai 00382105, 2017/12/26   PN:DTS20170101 */
#define CLASS_PCIE_RETIMER_ACTION                "PcieRetimerAction"
#define PROPERTY_PRT_ACTION_DESTINATION          "Destination"
#define PROPERTY_PRT_ACTION_DATA                 "Data"
#define PROPERTY_PRT_ACTION_NAME                 "Name"
#define PROPERTY_PRT_ACTION_OFFSET               "Offset"
#define PROPERTY_PRT_ACTION_PRTACTIONDATA        "PRTActionData"
/* END:   Modified by zhangfuhai 00382105, 2017/12/26 */

#define CLASS_CARD_ACTION                      "CardAction"




/* BEGIN: Added by zengrenchang (z00296903), 2016/11/14   PN:DTS2016120910505 VDDQ电压拉偏需求*/
#define CLASS_DFT_ACTION                                "DftAction"
/* END:   Added by zengrenchang (z00296903), 2016/11/14 */

/* CardAction 远程同步对象*/
#define CLASS_AGENT_CARD_ACTION                          "AgentCardAction"
#define PROPERTY_CARD_ACTION_DESTINATION       "Destination"
#define PROPERTY_CARD_ACTION_DATA                     "Data"
#define PROPERTY_CARD_ACTION_TYPE                     "Type"

#define CLASS_RESET_CHIP_ACTION                 "ResetChipAction"
#define PROPERTY_CHIP_ACTION_DESTINATION        "Destination"
#define PROPERTY_CHIP_ACTION_DATA               "Data"
#define PROPERTY_CHIP_ACTION_RESETTRAGET        "ResetTraget"


#define CLASS_ON_BOARD_RTC                      "BoardRTC"
#define OBJ_NAME_ON_BOARD_RTC                   "OnBoardRtc"
#define PROPERTY_ON_BOARD_RTC_YEAR              "Year"
#define PROPERTY_ON_BOARD_RTC_MONTH             "Month"
#define PROPERTY_ON_BOARD_RTC_DATE              "Date"
#define PROPERTY_ON_BOARD_RTC_HOUR              "Hour"
#define PROPERTY_ON_BOARD_RTC_MINUTE            "Minute"
#define PROPERTY_ON_BOARD_RTC_SECOND            "Second"
#define PROPERTY_ON_BOARD_RTC_TIMEINSEC    "TimeStampInSec"
#define PROPERTY_ON_BOARD_RTC_SRC               "Timestamp"
/* BEGIN: Added by c00149416, DTS2017021500425 2017/2/16 */
#define PROPERTY_ON_BOARD_RTC_TIME              "GetTimeFail"
/* END:   Added by c00149416, 2017/2/16 */
#define METHOD_SET_ON_BOARD_RTC_TIME            "SetRtcTime"
#define METHOD_SYNC_SET_RTC_TIME                  "SyncSetRtcTime"


#define CLASS_STATELESS                                 "Stateless"
#define PROPERTY_STATELESS_ENABLE                       "Enable"
#define PROPERTY_STATELESS_BROADCAST_NET_SEGMENT        "BroadcastNetSegment"
#define PROPERTY_STATELESS_BROADCAST_NET_PORT           "BroadcastPort"
#define PROPERTY_STATELESS_SYS_MANAGER_IP               "SysManagerIP"
#define PROPERTY_STATELESS_SYS_MANAGER_PORT             "SysManagerPort"
#define PROPERTY_STATELESS_AUTO_POWER_ON                "AutoPowerOn"
#define PROPERTY_STATELESS_SYS_MANAGER_ID               "SysManagerID"
#define METHOD_STATELESS_SET_ENABLE                     "SetEnable"
#define METHOD_STATELESS_SET_BROADCAST_NET_SEGMENT      "SetBroadcastNetSegment"
#define METHOD_STATELESS_SET_BROADCAST_NET_PORT         "SetBroadcastPort"
#define METHOD_STATELESS_SET_POWER_ON_CONTROL_STRING    "SetPowerOnControlString"
#define METHOD_STATELESS_SET_POWER_ON_PERMIT            "SetPowerOnPermit"
#define METHOD_STATELESS_SET_SYS_MANAGER_ID             "SetSysManagerID"
#define METHOD_STATELESS_SET_SYS_MANAGER_IP             "SetSysManagerIP"
#define METHOD_STATELESS_SET_SYS_MANAGER_PORT           "SetSysManagerPort"
#define METHOD_STATELESS_SET_AUTO_POWER_ON              "SetAutoPowerOn"

/* BEGIN: Added by y00356679 yuchaoqi, 2019/1/8   */
#define CLASS_SYSTEM_MANAGER_INFO                       "SystemManagerInfo"
#define PROPERTY_SYSTEM_MANAGER_INFO_ID                 "SysManagerId"
#define PROPERTY_SYSTEM_MANAGER_INFO_NAME               "SysManagerName"
#define PROPERTY_SYSTEM_MANAGER_INFO_IP                 "SysManagerIp"
#define PROPERTY_SYSTEM_MANAGER_INFO_LOCKED_TIME        "SysManagerLockedTime"
#define METHOD_SYSTEM_MANAGER_INFO_SET_ID               "SetSysManagerId"
#define METHOD_SYSTEM_MANAGER_INFO_SET_NAME             "SetSysManagerName"
#define METHOD_SYSTEM_MANAGER_INFO_SET_IP               "SetSysManagerIp"
#define METHOD_SYSTEM_MANAGER_INFO_SET_LOCKED_TIME      "SetSysManagerLockedTime"
/* END: Added by y00356679 yuchaoqi, 2019/1/8   */
/* zwx655588, 2019/12/9, 需求:UADP529954, SystemManagerInfo数据同步 */
#define METHOD_SYSTEM_MANAGER_INFO_PROPERTY_SYNC        "PropertySyncMethod"


#define CLASS_SATELLITE_CONTROLLER_NAME             "SatelliteController"
#define PROPERTY_SATELLITE_CONTROLLER_IPMICHANNEL   "IPMIChannel"
#define PROPERTY_SATELLITE_CONTROLLER_STATUS        "Status"

#define CLASS_PERIPHERAL_FAN_NAME                   "PeripheralFan"
#define PROPERTY_PERIPHERAL_FAN_TARGET              "Target"
#define PROPERTY_PERIPHERAL_FAN_ID                  "Id"
#define PROPERTY_PERIPHERAL_FAN_MODEL               "Model"
#define PROPERTY_PERIPHERAL_FAN_TYPE                "Type"
#define PROPERTY_PERIPHERAL_FAN_CHANNEL_TYPE        "ChannelType"
#define METHOD_SET_PERIPHERAL_FAN_LEVEL             "SetFanLevel"


#define CLASS_PERIPHERAL_SENSOR_NAME                "PeripheralSensor"
#define PROPERTY_PERIPHERAL_SENSOR_TARGET           "Target"
#define PROPERTY_PERIPHERAL_SENSOR_NUMBER           "SensorNumber"
#define PROPERTY_PERIPHERAL_SENSOR_READING          "Reading"
#define PROPERTY_PERIPHERAL_SENSOR_ASSERTSTATUS     "AssertStatus"

/* Pcie root port映射信息 */
#define CLASS_PCIE_ROOT_PORT_MAP                    "PcieRootPortMap"
#define PROPERTY_CARD_DEV_TYPE                      "CardType"
#define PROPERTY_CARD_DEV_NUMBER                    "CardNum"
#define PROPERTY_PORT_NUMBER                        "PortNumber"
#define PROPERTY_CONNECT_TYPE                       "ConnectType"
#define PROPERTY_BUS_NUMBER                         "BusNum"
#define PROPERTY_DEVICE_NUMBER                      "DevNum"
#define PROPERTY_FUNCTION_NUMBER                    "FunNum"

/*Signal Monitor*/
#define CLASS_SIGNAL_MONITOR                        "SignalMonitor"
#define PROPERTY_SIGNAL_MONITOR_NAME                "Name"
#define PROPERTY_SIGNAL_MONITOR_ASSERTINFO          "AssertInfo"
#define PROPERTY_SIGNAL_MONITOR_DEASSERTINFO        "DeassertInfo"
#define PROPERTY_SIGNAL_MONITOR_VALUESOURCE         "ValueSource"
#define PROPERTY_SIGNAL_MONITOR_SCANWHEN            "ScanWhen"
#define PROPERTY_SIGNAL_MONITOR_ENABLECLEAR         "EnableClear"
#define PROPERTY_SIGNAL_MONITOR_CLEARSTATUS         "ClearStatus"
#define PROPERTY_SIGNAL_MONITOR_CLEARVALUE          "ClearValue"
#define PROPERTY_SIGNAL_MONITOR_DEFAULTVALUE        "DefaultValue"
/* BEGIN: Add for AR:Hardware_010 by xwx388597, 2016/10/27 */
#define PROPERTY_SIGNAL_MONITOR_FAULTFLAG           "FaultFlag"
#define PROPERTY_SIGNAL_MONITOR_RECORDFAULTFLAG     "RecordFaultFlag"
#define PROPERTY_SIGNAL_MONITOR_ERRORCODE           "ErrorCode"
#define PROPERTY_SIGNAL_MONITOR_EVENTMONITORMASK1   "EventMonitorMask1"
#define PROPERTY_SIGNAL_MONITOR_EVENTMONITORMASK2   "EventMonitorMask2"
#define PROPERTY_SIGNAL_MONITOR_EVENTMONITORMASK3   "EventMonitorMask3"
#define PROPERTY_SIGNAL_MONITOR_EVENTMONITORMASK4   "EventMonitorMask4"
/* BEGIN: Add for AR:Hardware_010 by xwx388597, 2016/10/27 */

#define CLASS_EXTEND_BOARD_SIGNAL_MONITOR                        "ExtendBoardSignalMonitor"

/* BEGIN: Added by Yangshigui YWX386910, 2017/4/15   问题单号:AR-0000276589-003-001/002*/
#define CLASS_POWER_SIGNAL_MONITOR                  "PowerSignalMonitor"
#define PROPERTY_SIGNAL_MONITOR_LOGLEVEL            "LogLevel"
/* END:   Added by Yangshigui YWX386910, 2017/4/15 */

/*DevicesExist*/
#define CLASS_DEVICES_CONDITION                     "Condition"
#define PROPERTY_DEVICES_CONDITION_IS_TRUE          "IsTrue"
#define PROPERTY_DEVICES_CONDITION_OBJCOMBOTYPE     "ObjComboType"
#define PROPERTY_DEVICES_CONDITION_COMBOTYPE        "ComboType"
#define PROPERTY_DEVICES_CONDITION_REFCLASS         "RefClass"
#define PROPERTY_DEVICES_CONDITION_REFPROPERTIES    "RefProperties"
#define PROPERTY_DEVICES_CONDITION_MATCHVALUE       "MatchValue"
#define PROPERTY_DEVICES_CONDITION_MATCHSTRING      "MatchString"

/* ThresholdSensorMonitor  */
#define CLASS_THRESHOLD_SENSOR_MONITOR              "ThresholdSensorMonitor"
#define PROPERTY_THRESHOLD_SENSOR_MONITOR_POSIDEVIATION    "PositiveDeviation"
#define PROPERTY_THRESHOLD_SENSOR_MONITOR_NEGADEVIATION    "NegativeDeviation"
#define PROPERTY_THRESHOLD_SENSOR_REFERENCED_OBJECT        "RefSensor"

/* BEGIN: Added by f00225994, 2015/7/27   问题单号:AR-0000956737*/
//MM板的机框背板版本信息
#define BACKPLANEBOARD_OBJ_NAME                "BackplaneBoard"
#define PROPERTY_BACKPLANEBOARD_NAME             "Name"
#define PROPERTY_BACKPLANEBOARD_PCBVER           "PcbVersion"
#define PROPERTY_BACKPLANEBOARD_BOARDID          "BoardId"
#define PROPERTY_BACKPLANEBOARD_TYPE          "Type"

/* END:   Added by f00225994, 2015/7/27 */

/* BEGIN: Added for PN:AR0D988348-8024-4f5b-94BE-6B33CA76E4E1  TPM/TCM卡 by l00205093, 2015/10/30 */
// TPM/TCM卡
#define CLASS_SECURITY_MODULE                               "SecurityModule"
#define PROPERTY_SECURITY_MODULE_PRESENCE                   "Presence"
#define PROPERTY_SECURITY_MODULE_SELF_TEST_RESULT           "SelfTestResult"
#define PROPERTY_SECURITY_MODULE_SPECIFICATION_TYPE         "SpecificationType"
#define PROPERTY_SECURITY_MODULE_MANUFACTURER_NAME          "ManufacturerName"
#define PROPERTY_SECURITY_MODULE_SPECIFICATION_VERSION      "SpecificationVersion"
#define PROPERTY_SECURITY_MODULE_MANUFACTURER_VERSION       "ManufacturerVersion"
#define METHOD_SECURITY_MODULE_SET_SELF_TEST_RESULT         "SetSelfTestResult"
#define METHOD_SECURITY_MODULE_SET_SPECIFICATION_TYPE       "SetSpecificationType"
#define METHOD_SECURITY_MODULE_SET_MANUFACTURER_NAME        "SetManufacturerName"
#define METHOD_SECURITY_MODULE_SET_SPECIFICATION_VERSION    "SetSpecificationVersion"
#define METHOD_SECURITY_MODULE_SET_MANUFACTURER_VERSION     "SetManufacturerVersion"
/* END:   Added by l00205093, 2015/10/30 */
/* BEGIN: Added for PN:AR3B11456D-0DF7-4472-9395-98C20D375D76 OEM命令 by l00205093, 2015/11/4 */
#define CLASS_SENSORCATEGORY                               "SensorCategory"
#define PROPERTY_SENSORCATEGORY_CATEGORY                   "Category"
#define PROPERTY_SENSORCATEGORY_OFFSET                     "Offset"
#define PROPERTY_SENSORCATEGORY_VALUE                      "Value"
#define PROPERTY_SENSORCATEGORY_INNER_NAME                 "InnerName"
/* END:   Added by l00205093, 2015/11/4 */

#define CLASS_SENSOR_OEM_CFONIG             "SensorOemConfig"
#define PROPERTY_SENSOR_INNER_NAME          "InnerName"
#define PROPERTY_SENSOR_NAME                "SensorName"
#define PROPERTY_SENSOR_NUMBER              "Number"
#define PROPERTY_SENSOR_ASSERTMASK          "AssertionMask"
#define PROPERTY_SENSOR_DEASSERTMASK        "DeassertionMask"
#define PROPERTY_SENSOR_READINGMASK         "DiscreteReadingMask"
#define PROPERTY_OEM_SENSOR_UPPER_NONRECOVERABLE     "UpperNonrecoverable"
#define PROPERTY_OEM_SENSOR_UPPER_CRITICAL           "UpperCritical"
#define PROPERTY_OEM_SENSOR_UPPER_NONCRITICAL        "UpperNoncritical"
#define PROPERTY_OEM_SENSOR_LOWER_NONRECOVERABLE     "LowerNonrecoverable"
#define PROPERTY_OEM_SENSOR_LOWER_CRITICAL           "LowerCritical"
#define PROPERTY_OEM_SENSOR_LOWER_NONCRITICAL        "LowerNoncritical"


/*连接器类型*/
#define CONNECTOR_TYPE_PSU      "PsuConnect"
#define CONNECTOR_TYPE_HDD      "DiskConnector"
/* BEGIN: Added by cWX398307,DTS2016101201583  , 2016/10/24  */
#define CONNECTOR_TYPE_HDD1    "DiskConnector1"
#define CONNECTOR_TYPE_HDDNVME    "DiskConnectorNVMe"
/* END:   Added by cWX398307,DTS2016101201583  , 2016/10/24 */

/* BEGIN: Added by 00356691 zhongqiu, 2017/9/23   问题单号:DTS2017091214515 */
#define CONNECTOR_TYPE_HDD0    "DiskConnector0"
/* END: Added by 00356691 zhongqiu, 2017/9/23   问题单号:DTS2017091214515 */


/* BEGIN: Added by l00264274, AR-0000245339-002-001, 2015/3/2  */
#define CLASS_PORT_MGNT                             "PortMgnt"
#define PROPERTY_PORT_MGNT_PORT_NO                  "PortNo"
#define PROPERTY_PORT_MGNT_DEST_SLOT                "DstSlot"
#define PROPERTY_PORT_MGNT_DEST_PORT                "DstPort"
#define PROPERTY_PORT_MGNT_SILK_TEXT                "SilkText"
#define PROPERTY_PORT_MGNT_PRESENT                  "Present"
#define PROPERTY_PORT_MGNT_LINK_STATUS              "LinkStatus"
#define PROPERTY_PORT_MGNT_SUBLINK_STATUS           "SubLinkStatus"
#define PROPERTY_PORT_MGNT_DIRECTION                "Direction"

#define CLASS_SWITCH_MGNT                           "SwitchMgnt"
#define PROPERTY_SWITCH_MGNT_SLOT_NUM               "SlotNum"
#define PROPERTY_SWITCH_MGNT_PORT_NUM               "PortNum"
/* END:   Added by l00264274, AR-0000245339-002-001, 2015/3/2 */

/* BEGIN: Added by zengrenchang, 2015/12/7   AR837F6D53-5DE9-47bf-86A2-5477986F15CD:对phy进行配置 */
#define CLASS_PORT_ADAPTER                          "PortAdapter"
/* BEGIN: Added by d00381887, DTS2017121201040, 2017/12/17 */
#define PROPERTY_PORT_ADAPTER_IS_PWROFF_UPRADE      "IsPowerOffUpgrade"
/* END: Added by d00381887, DTS2017121201040, 2017/12/17 */
#define PROPERTY_PORT_ADAPTER_PORT_NO               "PortNo"
#define PROPERTY_PORT_ADAPTER_PHY_ADDR              "PhyAddr"
#define PROPERTY_PORT_ADAPTER_SILK_TEXT             "SilkText"
#define PROPERTY_PORT_ADAPTER_MEDIA_CATAGORY        "TransceiverMediaCatagory"
#define PROPERTY_PORT_ADAPTER_EEPROM_A0             "StorageEepromA0"
#define PROPERTY_PORT_ADAPTER_EEPROM_A2             "StorageEepromA2"
#define PROPERTY_PORT_ADAPTER_PRESENT               "Present"
#define PROPERTY_PORT_ADAPTER_HEALTH                "Health"
#define PROPERTY_PORT_ADAPTER_LINK_STATUS           "LinkStatus"
#define PROPERTY_PORT_ADAPTER_UPGRADE_STATUS        "UpgradeStatus"

#define CLASS_PHY_MGNT                              "PHYMgnt"
#define PROPERTY_PHY_MGNT_OPERATER_TYPE             "OperateType"
#define PROPERTY_PHY_MGNT_PHY_ADDR                  "PhyAddr"
#define PROPERTY_PHY_MGNT_REGISTER_ADDR             "RegisterAddr"
#define PROPERTY_PHY_MGNT_REGISTER_VALUE            "RegisterValue"
#define PROPERTY_PHY_MGNT_EXPECT_VALUE              "ExpectValue"
/* BEGIN: Added by d00381887, 2017/12/18, AR.SR.SFEA02119723.006.002 支持识别媒质类型 */
#define METHOD_GET_PORT_MANUFACTURE_INFO            "GetManufactureInfo"
#define METHOD_GET_HARDWARE_STATUS                  "GetHardwareStatus"
#define METHOD_GET_POARTADAPTER_OBJ_LIST            "GetPortAdapterObjList"
/* END: Added by d00381887, 2017/12/18, AR.SR.SFEA02119723.006.002 支持识别媒质类型 */

/* END:   Added by zengrenchang, 2015/12/7 */

/* BEGIN: Added by zengrenchang, 2015/12/7   AR24969A8D-3EF1-4b89-BAF0-8448A2C5B55D:MDIO通道检测 */
#define CLASS_PHY_INFO                              "PHYInfo"
#define PROPERTY_PHY_INFO_NUMBER                    "Number"
#define PROPERTY_PHY_INFO_STATUS                    "Status"
#define PROPERTY_PHY_INFO_FW_VERSION                "FWVersion"
#define PROPERTY_PHY_INFO_SUPPORT_MEDIA_TYPE        "SupportedMediaType"
/* END:   Added by zengrenchang, 2015/12/7 */

/* BEGIN: Added by zengrenchang(z00296903), 2017/4/12   问题单号:AR-0000276591-001-004 */
#define CLASS_SWITCH_PLANE_INFO_MGNT                "SwitchPlaneInfoMgnt"
#define PROPERTY_SWITCH_PLANE_FRU_ID                "FruId"                 /* FRUID */
/* BEGIN: Added by z00352904, 2017/7/14 10:19:31  PN:SR-0000276585-011 */
#define PROPERTY_SWITCH_PLANE_CHIP_VER              "ChipVersion"
#define PROPERTY_SWITCH_PLANE_UP_EN                 "UpgradeEnabled"

/* BEGIN: Added by Yangshigui YWX386910, 2017/9/23   问题单号:DTS2017092212812*/
#define PROPERTY_SWITCH_CHIP_SVID_READ_FAILED       "ChipSvidReadFailed"
/* END:   Added by Yangshigui YWX386910, 2017/9/23 */

/* END:   Added by z00352904, 2017/7/14 10:19:33*/
/* BEGIN: Added by jwx456325, 2017/8/2  */
#define PROPERTY_SWITCH_PLANE_RESTART                 "SwitchPlaneRestart"
/* END:   Added by jwx456325, 2017/8/2*/
#define SWITCH_METHOD_GET_IP_MASK_GW                        "GetIpMaskGw"
#define SWITCH_METHOD_SET_IP_MASK_GW                        "SetIpMaskGw"
/* END:   Added by zengrenchang(z00296903), 2017/4/12 */
/* Added by lwx655206, 2019/10/22  UADP433859 & UADP433860 */
#define SWITCH_METHOD_GET_IPV6_ADDR                        "GetSwiIpv6Addr"
#define SWITCH_METHOD_GET_IPV6_ROUTE                       "GetSwiIpv6Route"
#define SWITCH_METHOD_ADD_IPV6_ADDR                        "AddSwiIpv6Addr"
#define SWITCH_METHOD_ADD_IPV6_ROUTE                       "AddSwiIpv6Route"
#define SWITCH_METHOD_DEL_IPV6_ADDR                        "DelSwiIpv6Addr"
#define SWITCH_METHOD_DEL_IPV6_ROUTE                       "DelSwiIpv6Route"

#define SWITCH_METHOD_GET_STACKING_STATUS           "GetStackingStatus"
#define SWITCH_METHOD_GET_VERSION                   "GetSwitchVersion"

/* BEGIN: Added for PN:远程syslog by l00205093, 2016/3/24 */
#define CLASS_SYSLOG_CONFIG                         "SyslogConfig"
#define OBJECT_SYSLOG_CONFIG                        "SyslogConfig_0"
#define PROPERTY_SYSLOG_MGNT_ENABLE_STATE           "EnableState"
#define PROPERTY_SYSLOG_MGNT_MSG_FORMAT             "MsgFormat"
#define PROPERTY_SYSLOG_MGNT_MSG_IDENTITY           "MsgIdentity"
#define PROPERTY_SYSLOG_MGNT_MSG_SEVE_MASK          "MsgSeverity"
#define PROPERTY_SYSLOG_MGNT_AUTH_TYPE              "AuthType"
#define PROPERTY_SYSLOG_MGNT_HOST_CERT_PATH         "HostCertPath"
#define PROPERTY_SYSLOG_MGNT_CLIENT_CERT_PATH       "ClientCertPath"
#define PROPERTY_SYSLOG_MGNT_MSG_PROTOCOL           "MsgProtocol"
#define PROPERTY_SYSLOG_MGNT_NET_PROTOCOL           "NetProtocol"
#define PROPERTY_SYSLOG_ROOT_START_TIME             "RootStartTime"
#define PROPERTY_SYSLOG_ROOT_EXPIRE_TIME            "RootExpirTime"
#define PROPERTY_SYSLOG_CLINET_START_TIME           "ClientStartTime"
#define PROPERTY_SYSLOG_CLINET_EXPIRE_TIME          "ClientExpirTime"
#define PROPERTY_SYSLOG_CRL_VERIFICATION_ENABLED    "CRLVerificationEnabled"
#define PROPERTY_SYSLOG_CRL_START_TIME              "CRLStartTime"
#define PROPERTY_SYSLOG_CRL_EXPIRE_TIME             "CRLExpireTime"
#define METHOD_SYSLOG_MGNT_SET_ENABLE_STATE         "SetEnableState"
#define METHOD_SYSLOG_MGNT_SET_MSG_FORMAT           "SetMsgFormat"
#define METHOD_SYSLOG_MGNT_SET_MSG_IDENTITY         "SetMsgIdentity"
#define METHOD_SYSLOG_MGNT_SET_MSG_SEVE_MASK        "SetMsgSeverity"
#define METHOD_SYSLOG_MGNT_SET_AUTH_TYPE            "SetAuthType"
#define METHOD_SYSLOG_MGNT_SET_MSG_PROTOCOL         "SetMsgProtocol"
#define METHOD_SYSLOG_MGNT_SET_NET_PROTOCOL         "SetNetProtocol"
#define METHOD_SYSLOG_MGNT_IMPORT_CERT              "ImportCert"
#define METHOD_SYSLOG_MGNT_GET_CERT_STATUS          "GetCertStatus"
#define METHOD_SYSLOG_MGNT_GET_CERT_INFO            "GetCertInfo"
#define METHOD_SYSLOG_MGNT_PROPERTY_SYNC            "PropertySyncMethod"
#define METHOD_SYSLOG_MGNT_CERTINFO_PROPERTY_SYNC   "CertInfoPropertySyncMethod"
#define METHOD_SYSLOG_MGNT_IMPORT_CRL "ImportCRL"
#define METHOD_SYSLOG_MGNT_DELETE_CRL "DeleteCRL"


#define CLASS_SYSLOG_ITEM_CONFIG                    "SyslogItemCfg"
#define PROPERTY_REMOTE_SYSLOG_INDEX                "Index"
#define PROPERTY_REMOTE_SYSLOG_ENABLE_STATE         "EnableState"
#define PROPERTY_REMOTE_SYSLOG_LOG_SRC_MASK         "LogSrcMask"
#define PROPERTY_REMOTE_SYSLOG_DEST_ADDR            "DestAddr"
#define PROPERTY_REMOTE_SYSLOG_DEST_PORT            "DestPort"
#define PROPERTY_REMOTE_SYSLOG_TEST_TRIGGER         "TestTrigger"
#define PROPERTY_REMOTE_SYSLOG_DESTADDR_VALID       "DestAddrValid"
#define METHOD_REMOTE_SYSLOG_SET_ENABLE_STATE       "SetEnableState"
#define METHOD_REMOTE_SYSLOG_SET_DEST_ADDR          "SetDestAddr"
#define METHOD_REMOTE_SYSLOG_SET_DEST_PORT          "SetDestPort"
#define METHOD_REMOTE_SYSLOG_SET_TEST_TRIGGER       "SetTestTrigger"
#define METHOD_REMOTE_SYSLOG_SET_LOG_SRC_MASK       "SetLogSrcMask"
#define METHOD_REMOTE_SYSLOG_PROPERTY_SYNC          "PropertySyncMethod"

/* END:   Added by l00205093, 2016/3/24 */

/* BEGIN: Add by h90006956: AR-0000245426-002-001 支持监控硬盘拔插 */
#define CLASS_DISK_SLOT_PST_INFO                    "DiskSlotInfo"
#define PROPERTY_DISK_OWN_SLOT_ID                   "OwnSlotId"
#define PROPERTY_DISK_CONNECTOR_PRESENT             "Present"
#define PROPERTY_DISK_PLANE_ID                      "PlaneId"
/* END: Add by h90006956: AR-0000245426-002-001 支持监控硬盘拔插 */

/* BEGIN: Added by zengrenchang, 2016/6/13   问题单号:DTS2016010902797 */
#define CLASS_CLI_BLACK_LIST                    "CLIBlackList"
#define PROPERTY_CMD_LIST                       "CmdList"
/* END:   Added by zengrenchang, 2016/6/13 */
/* Start by h00272616  DTS2016062504101 20160628 */
#define CLASS_ANCHOR                                "Anchor"
#define PROPERTY_ANCHOR_POSITION                    "Position"
#define PROPERTY_ANCHOR_SLOT_ID                     "Slot"
/* End by h00272616  DTS2016062504101 20160628 */

/* Start by h00272616  AR-0000245342-004-002 20160713 */
#define CLASS_CONDITION_TASK                     "ConditionTask"
#define PROPERTY_CONDITION_TASK_CONDITION        "Condition"
#define PROPERTY_CONDITION_TASK_LAST_CONDITION   "LastCondition"
#define PROPERTY_CONDITION_TASK_LAST_CONDITION_ENABLE  "LastConditionEnable"
#define PROPERTY_CONDITION_TASK_ACTION_REF_OBJ   "ActionRefObj"

#define PROPERTY_CONDITION_TASK_PERIOD           "Period"
#define PROPERTY_CONDITION_TASK_BEFORE_ACTION    "BeforeAction"
#define PROPERTY_CONDITION_TASK_LOG_INFO         "LogInfo"
#define PROPERTY_CONDITION_TASK_LOG_TYPE         "LogType"
#define PROPERTY_CONDITION_TASK_LOG_FILE_TYPE    "LogFileType"
#define PROPERTY_CONDITION_TASK_LOG_PARAM_REF_OBJ   "LogParamRefObj"
#define PROPERTY_CONDITION_TASK_LOG_PARAM_REF_PROP  "LogParamRefProp"
/* End by h00272616  AR-0000245342-004-002 20160713 */


/* BEGIN: Added for PN:告警优化 AR-0000256847-001-001 by l00205093, 2016/7/14 */
#define CLASS_LANGUAGE                              "Language"
#define PROPERTY_LANGUAGE_EVENT_LANG_SUPPORT        "EventLangSupport"
#define PROPERTY_LANGUAGE_EVENT_LANG_VERSION        "EventLangVersion"
#define PROPERTY_LANGUAGE_INTERFACE_LANG_VERSION    "InterfaceLangVersion"
#define PROPERTY_LANGUAGE_SUPPORTED_LANGUAGE        "SupportedLanguage"





#define CLASS_EVENT_CONFIGURATION                           "EventConfiguration"
#define PROPERTY_EVENT_CONFIGURATION_VERSION                "Version"
#define PROPERTY_EVENT_CONFIGURATION_IPMI_EVENT_SEQUENCE    "IPMIEventSequence"
#define PROPERTY_EVENT_CONFIGURATION_SCAN_CYCLE_ARRAY       "ScanCycleArray"
#define PROPERTY_EVENT_CONFIGURATION_MAX_EVENT_NUM          "MaxEventNum"
#define PROPERTY_EVENT_CONFIGURATION_CUR_EVENT_NUM          "CurEventNum"

#define CLASS_PRODUCT_FAMILY                                "ProductFamily"

#define PROPERTY_EVENT_CONFIGURATION_SEL_SYNC               "SelSync"
#define PROPERTY_EVENT_CONFIGURATION_BASE_VERSION           "BaseVersion"
#define METHOD_EVENT_CONFIGURATION_SET_SEL_SYNC             "SetSelSync"
/* BEGIN: Added by gwx455466, 2017/12/28 17:20:59   问题单号:AR.SR.SFEA02109778.006.003 */
#define METHOD_EVENT_CONFIGURATION_POST_EVENT               "PostEvent"
#define METHOD_EVENT_CONFIGURATION_POST_EVENT_PARA          "PostEventWithParam"
/* END:   Added by gwx455466, 2017/12/28 17:21:3 */
#define METHOD_POST_EVT_MSG_TO_SENSOR                "PostEvengMsgToSensor"


#define CLASS_EVENT_INFORMATION                             "EventInformation"
#define PROPERTY_EVENT_INFORMATION_ENABLE                   "Enable"
#define PROPERTY_EVENT_INFORMATION_EVENT_TYPE               "EventType"
#define PROPERTY_EVENT_INFORMATION_SEVERITY                 "Severity"
#define PROPERTY_EVENT_INFORMATION_DESCRIPTION              "Description"
/* BEGIN: Modified by zhanglei wx382755, 2017/10/26   PN:DTS2017102607919 */
#define PROPERTY_EVENT_INFORMATION_DEASSERT_DESCRIPTION     "DeassertDescription"
/* END:   Modified by zhanglei wx382755, 2017/10/26 */
#define PROPERTY_EVENT_INFORMATION_SUGGESTION               "Suggestion"
#define PROPERTY_EVENT_INFORMATION_MUTEX_CODE               "MutexCode"
#define PROPERTY_EVENT_INFORMATION_HEALTH_MUTEX_CODE        "HealthMutexCode"
#define PROPERTY_EVENT_INFORMATION_EVENT_CODE               "EventCode"
#define PROPERTY_EVENT_INFORMATION_OLD_EVENT_CODE           "OldEventCode"
#define PROPERTY_EVENT_INFORMATION_EVENT_READING_TYPE       "EventReadingType"
#define PROPERTY_EVENT_INFORMATION_SENSOR_TYPE              "SensorType"
#define PROPERTY_EVENT_INFORMATION_EVENT_NAME               "EventName"
#define PROPERTY_EVENT_INFORMATION_EVENT_EFFECT             "EventEffect"
#define PROPERTY_EVENT_INFORMATION_EVENT_CAUSE              "EventCause"
/* BEGIN: Added for 问题单号:DTS2016122913791 by wwx383734, 2016/12/30 */
#define METHOD_EVENT_INFORMATION_SET_ENABLE                 "SetEnable"
#define METHOD_EVENT_INFORMATION_SET_EVENT_TYPE             "SetEventType"
#define METHOD_EVENT_INFORMATION_SET_SEVERITY               "SetSeverity"
/* BEGIN: Added for 问题单号:DTS2017011200372 by wwx383734, 2017/1/12 */
#define METHOD_EVENT_MONITOR_PRESICE_ALARM_SIMULATE "Mock"
/* END:   Added for 问题单号:DTS2017011200372 by wwx383734, 2017/1/12 */

/* END:   Added for 问题单号:DTS2016122913791 by wwx383734, 2016/12/30 */
/* BEGIN: Added by l00419687, 2017-12-25, 原因:事件级别主备同步方法，级别可变事件的级别需要进行主备同步 */
#define METHOD_EVENT_INFORMATION_SEVERITY_SYNC              "SeveritySync"
/* END: Added by l00419687, 2017-12-25 */
#define METHOD_EVENT_MONITOR_SHIELD "ShieldEvent"

#define CLASS_EVENT_POLICY                          "EventPolicy"
#define PROPERTY_EVENT_POLICY_SCAN_CONDITION        "ScanCondition"
#define PROPERTY_EVENT_POLICY_ACTION                "Action"
#define PROPERTY_EVENT_POLICY_DEASSERT_FLAG         "DeassertFlag"
#define PROPERTY_EVENT_POLICY_DEASSERT_POLICY       "DeassertPolicy"
#define PROPERTY_EVENT_POLICY_SCAN_POLICY           "ScanPolicy"
#define PROPERTY_EVENT_POLICY_SCAN_CYCLE            "ScanCycle"
#define PROPERTY_EVENT_POLICY_LOG_CHANNEL_FILTER    "LogChannelFilter"
#define METHOD_EVENT_POLICY_SET_POLICY_DEASSERT_FLAG       "SetDeassertFlag"
/* BEGIN: Added by l00419687, 2017-12-25, 原因:事件级别主备同步方法，级别可变事件的级别需要进行主备同步 */
#define METHOD_EVENT_POLICY_DEASSERT_FLAG_SYNC      "DeassertFlagSync"
/* END: Added by l00419687, 2017-12-25 */
#define METHOD_EVENT_POLICY_SET_ACTION              "SetAction"

#define CLASS_EVENT_MONITOR                         "EventMonitor"
#define PROPERTY_EVENT_MONITOR_SUBJECT_OBJ          "SubjectObj"
#define PROPERTY_EVENT_MONITOR_INFORMATION_OBJ      "InformationObj"
#define PROPERTY_EVENT_MONITOR_POLICY_OBJ           "PolicyObj"
#define PROPERTY_EVENT_MONITOR_SENSOR_OBJ           "SensorObj"
#define PROPERTY_EVENT_MONITOR_EVENT_FRU            "EventFRU"
#define PROPERTY_EVENT_MONITOR_MODE                 "Mode"
#define PROPERTY_EVENT_MONITOR_SAVED_MODE           "SavedMode"
#define PROPERTY_EVENT_MONITOR_TIME                 "Time"
#define PROPERTY_EVENT_MONITOR_STATE                "State"
#define PROPERTY_EVENT_MONITOR_SCANNED_COUNT        "ScannedCount"
#define PROPERTY_EVENT_MONITOR_READING              "Reading"
#define PROPERTY_EVENT_MONITOR_SAVED_READING        "SavedReading"
#define PROPERTY_EVENT_MONITOR_READING_MASK         "ReadingMask"
#define PROPERTY_EVENT_MONITOR_THRESHOLD            "Threshold"
#define PROPERTY_EVENT_MONITOR_HYSTERESIS           "Hysteresis"
#define PROPERTY_EVENT_MONITOR_OPERATOR             "Operator"
#define PROPERTY_EVENT_MONITOR_DESCRIPTION_DYNAMIC  "DescriptionDynamic"
#define PROPERTY_EVENT_MONITOR_SUGGESTION_DYNAMIC   "SuggestionDynamic"
#define PROPERTY_EVENT_MONITOR_ADDITIONAL_INFO      "AddInfo"
#define METHOD_EVENT_MONITOR_DEL_EVENT_FROM_MONITOR "DelEventFromMonitor"

#define CLASS_BIOS_EVENT                            "BIOSEvent"
#define PROPERTY_BIOS_EVENT_SENSOR_TYPE             "SensorType"
#define PROPERTY_BIOS_EVENT_READING_TYPE            "EventReadingType"
#define PROPERTY_BIOS_EVENT_DATA                   "EventData"

/* END:   Added by l00205093, 2016/7/14 */

/* BEGIN: Added for PN:DTS2017021303916 by z00403145, 2017/02/13 */
#define CLASS_EVENT_SEVERITY_CHANGE            "EventSeverityChange"
#define PROPERTY_EVENT_SEVERITY_CHANGE_EVENT_CODE      "EventCode"
#define PROPERTY_EVENT_SEVERITY_CHANGE_SEVERITY_VALUE      "SeverityValue"
#define PROPERTY_EVENT_SEVERITY_CHANGE_EVENTINFOOBJ         "EventInfoObj"
/* END:   Added by z00403145, 2017/02/13 */

#define CLASS_REDFISH               "Redfish"
#define OBJECT_REDFISH_NAME         CLASS_REDFISH
#define PROPERTY_REDFISH_SESSION_TIMEOUT     "SessionTimeout"
#define PROPERTY_REDFISH_ODATA_VERSION       "OdataVersion"
/* BEGIN: Added by 00356691 zhongqiu, 2017/9/5   PN:DTS2017081800548*/
#define PROPERTY_REDFISH_ENCRYPTO_COMPONENT  "RedfishComponent"
#define PROPERTY_REDFISH_WORKKEY             "RedfishWorkKey"
/* END:   Added by 00356691 zhongqiu, 2017/9/5   PN:DTS2017081800548*/
#define PROPERTY_REDFISH_MASTERKEY_ID        "RedfishMasterKeyId"
#define PROPERTY_REDFISH_DECOMPRESSEWD_CFG_FILE  "CfgFileDecompressed"
#define METHOD_HTTP_REQUEST                  "ProcessHttpRequest"
#define METHOD_HTTP_REDIRECT                 "RedirectHttpRequest"
#define METHOD_COPY_SP_SCHEMA                "CopySpSchema"
#define METHOD_REDFISH_UPDATE_CIPHERTEXT    "UpdateRedfishCiphertext"

/* BEGIN: Added by z00356691 zhongqiu, 2017/2/21   PN:AR------Redfish事件上报*/
#define CLASS_RF_EVT_SVC                             "RedfishEventService"
#define PROPERTY_RF_EVT_SVC_SERVICE_ENABLED          "ServiceEnabled"
#define PROPERTY_RF_EVT_SVC_SERVER_IDENTITY_SRC      "ServerIdentitySource"
#define PROPERTY_RF_EVT_SVC_EVENT_RETRY_TIMES        "DeliveryRetryAttempts"
#define PROPERTY_RF_EVT_SVC_EVENT_DELAY_INSEC        "DeliveryRetryIntervalSeconds"
#define PROPERTY_RF_EVT_SVC_EVENT_DETAIL_CUSTOMIZED_ID   "EventDetailCustomizedID"
#define PROPERTY_RF_EVT_SVC_EVENT_RECORD_ID          "EventRecordID"
#define PROPERTY_RF_EVT_SVC_EVENT_RECORDS            "EventRecords"
#define PROPERTY_RF_EVT_SVC_EVENT_RESOURCE_ID        "EventResourceID"
#define PROPERTY_RF_EVT_SVC_LAST_ALERT_ID            "EventLastAlertID"

#define METHOD_RF_EVT_SVC_GET_REDFISH_EVT_MSG        "GenRedfishEventMsg"
#define METHOD_RF_EVT_SVC_POST_EVT_MSG               "PostRedfishEventMsg"
#define METHOD_RF_EVT_SVC_GEN_ALERT_ORIGIN_RSC       "GenAlertOriginRscURI"
#define METHOD_RF_EVT_SVC_SET_SERVER_IDENTITY_SOURCE "SetServerIdentitySource"
#define METHOD_RF_EVT_SVC_SET_EVENT_POST_ENABLE      "SetServiceEnabled"
#define METHOD_RF_EVT_SVC_DATA_SYNC      "DataSyncMethod"
#define METHOD_RF_EVT_SVC_SET_EVENT_DETAIL_CUSTOMIZED_ID "SetEventDetailCustomizedID"
#define METHOD_RF_START_SP_FIRMWARE_UPGRADE_MONITOR  "StartSPFirmwareUpgradeMonitor"

#define CLASS_EVT_SUBSCRIPTION                       "RedfishEvtSubscription"
#define PROPERTY_EVT_SUBSCRIPTION_ID                 "Id"
#define PROPERTY_EVT_SUBSCRIPTION_DES                "Destination"
#define PROPERTY_EVT_SUBSCRIPTION_EVT_TYPES          "EventTypes"
#define PROPERTY_EVT_SUBSCRIPTION_EVT_CTX            "Context"
#define PROPERTY_EVT_SUBSCRIPTION_EVT_DRP            "DeliveryRetryPolicy"
#define PROPERTY_EVT_SUBSCRIPTION_EVT_STATE         "State"
#define PROPERTY_EVT_SUBSCRIPTION_EVT_PROTOCOL       "Protocol"
#define PROPERTY_EVT_SUBSCRIPTION_HTTP_HEADERS       "HttpHeaders"
#define PROPERTY_EVT_SUBSCRIPTION_ENCRYPTED_HTTP_HEADERS       "EncryptedHttpHeaders"
#define PROPERTY_EVT_SUBSCRIPTION_ORIGIN_RSC         "OriginResources"
#define PROPERTY_EVT_SUBSCRIPTION_MSG_ID             "MessageIds"
#define PROPERTY_EVT_SUBSCRIPTION_TYPE "SubscriberType"
#define METHOD_EVT_SUBSCRIPTION_SET_CONTEXT          "SetEventSubscriptionContext"
#define METHOD_EVT_SUBSCRIPTION_SET_HTTP_HEADERS     "SetEvtSubscriptionHttpHeaders"
#define METHOD_EVT_SUBSCRIPTION_DATA_SYNC     "DataSyncMethod"
/* END:   Added by z00356691 zhongqiu, 2017/2/21   PN:AR------Redfish事件上报*/

#define CLASS_SSDP_CONFIG                            "SSDPConfig"
#define PROPERTY_SSDP_CONFIG_MODE                    "Mode"
#define PROPERTY_SSDP_CONFIG_TYPE                    "Type"
#define PROPERTY_SSDP_CONFIG_USEINTERFACE            "UseInterface"
#define PROPERTY_SSDP_CONFIG_INTERVAL                "Interval"
#define PROPERTY_SSDP_CONFIG_IPV4ADDR                "IPv4Addr"
#define PROPERTY_SSDP_CONFIG_IPV6ADDR                "IPv6Addr"
#define PROPERTY_SSDP_CONFIG_PORT                    "Port"
#define PROPERTY_SSDP_CONFIG_MULTICAST_TTL           "NotifyTTL"
#define PROPERTY_SSDP_CONFIG_USN                     "USN"
#define METHOD_SSDP_CONFIG_USEINTERFACE              "SetUseInterface"
#define METHOD_SSDP_CONFIG_INTERVAL                  "SetNotifyInterval"
#define METHOD_SSDP_CONFIG_SCOPE                     "SetIpv6Scope"
#define METHOD_SSDP_CONFIG_TTL                       "SetNotifyTTL"
#define METHOD_SSDP_CONFIG_PORT                      "SetSSDPPort"
#define METHOD_SSDP_CONFIG_MODE                      "SetSSDPMode"
#define METHOD_SSDP_CONFIG_RESTART_SERVICE           "RestartService"
#define METHOD_SSDP_CONFIG_SYNC_PROPERTY             "PropertySyncMethod"

#define CLASS_NAME_LLDP_CONFIG                       "LLDPConfig"
#define PROPERTY_LLDP_CONFIG_ENABLE                  "LLDPEnable"
#define PROPERTY_LLDP_CONFIG_WORKMODE                "WorkMode"
#define PROPERTY_LLDP_CONFIG_TX_DELAY                "TxDelaySeconds"
#define PROPERTY_LLDP_CONFIG_TX_INTERVAL             "TxIntervalSeconds"
#define PROPERTY_LLDP_CONFIG_TX_HOLD                 "TxHold"
#define METHOD_LLDP_CONFIG_LLDP_ENABLE               "SetLLDPEnable"
#define METHOD_LLDP_CONFIG_TX_DELAY                  "SetTxDelaySeconds"
#define METHOD_LLDP_CONFIG_TX_INTERVAL               "SetTxIntervalSeconds"
#define METHOD_LLDP_CONFIG_TX_HOLD                   "SetTxHold"
#define METHOD_LLDP_CONFIG_WORKMODE                  "SetWorkMode"
#define METHOD_LLDP_SYSLOG_PROPERTY_SYNC             "PropertySyncMethod"

#define CLASS_NAME_MCTP_DEV_OPERATE                  "MctpDevOperate"
#define PROPERTY_MCTP_BLACKBOX_PROCESSBAR            "BlackBoxProcessbar"
#define METHOD_MCTP_GET_BLACKBOX                     "GetBlackBox"

#define CLASS_NAME_ENERGY_SAVING_CFG_ITEM "EnergySavingCfgItem"
#define PROPERTY_ENERGY_SAVING_CFG_NAME "CfgName"
#define PROPERTY_ENERGY_SAVING_CFG_VALUE "Value"
#define ENERGY_SAVING_CFG_NAME_DEMT "DEMTEnabled"
#define ENERGY_SAVING_CFG_NAME_POWERMODE "PowerMode"
#define ENERGY_SAVING_CFG_NAME_ACTIVE_PSU "ActivePSU"

#define CLASS_NAME_ENERGY_SAVING "EnergySaving"
#define PROPERTY_ENERGY_SAVING_ENABLE "Enable"
#define PROPERTY_ENERGY_SAVING_STATE "State"
#define PROPERTY_ENERGY_SAVING_LOGIC_CPU_USAGE_THRE "LogicalCpuUsageThre"
#define PROPERTY_ENERGY_SAVING_CANCEL_CFG_TIME "CancelCfgTime"
#define PROPERTY_ENERGY_SAVING_CANCEL_CFG "CancelCfg"
#define PROPERTY_ENERGY_SAVING_ACTIVE_CFG "ActiveCfg"
#define METHOD_ENERGY_SAVING_SET_ENABLE "SetEnergySavingEnable"
#define METHOD_ENERGY_SAVING_LOGIC_CPU_UASGE_THRE "SetLogicalCpuUsageThre"
#define METHOD_ENERGY_SAVING_CANCEL_CFG "SetCancleCfg"
#define METHOD_ENERGY_SAVING_ACTIVE_CFG "SetActiveCfg"

/* BEGIN: Added by fuzhengwen, PN:NTP, 2016/10/09 */
#define CLASS_NTP                                "NTP"
#define OBJECT_NTP                               CLASS_NTP
#define PROPERTY_NTP_ENABLE_STATUS               "EnableStatus"
#define PROPERTY_NTP_PREFERRED_SERVER            "PreferredServer"
#define PROPERTY_NTP_ALTERNATIVE_SERVER          "AlternativeServer"
#define PROPERTY_NTP_EXTRA_SERVER                "AlternativeServerArray"
#define PROPERTY_NTP_SERVER_COUNT                "NtpServerCount"
#define PROPERTY_NTP_MODE                        "Mode"
#define PROPERTY_NTP_AUTH_ENABLE_STATUS          "AuthEnableStatus"
#define PROPERTY_NTP_GROUP_KEY_PATH              "GroupKeyPath"
#define PROPERTY_NTP_SYSTIME_JUMP_VAL            "SysTimeJumpVal"
#define PROPERTY_NTP_SYSTIME_TOTAL_JUMP_VAL      "TotalTimeJumpValue"
#define PROPERTY_NTP_SYNC_TIME_STATUS            "SyncTimeStatus"
#define PROPERTY_NTP_SYNC_RESULT                 "SyncResult"
/* BEGIN: Added by gwx455466, 2017/6/20 11:24:40   问题单号:AR-0000276590-016-003 */
#define PROPERTY_NTP_MINIMUM_POLLING_INTERVAL   "MinPollInterval"
#define PROPERTY_NTP_MAXIMUM_POLLING_INTERVAL   "MaxPollInterval"
/* END:   Added by gwx455466, 2017/6/20 11:24:46 */
#define PROPERTY_NTP_PWDMASTERKEY_ID            "NtpPwdMasterKeyId"
#define PROPERTY_NTP_GROUP_KEY_TYPE             "NtpGroupKeyType"

#define METHOD_NTP_SET_ENABLE_STATUS             "SetEnableStatus"          /*设置NTP的使能状态*/
#define METHOD_NTP_SET_PREFERRED_SERVER          "SetPreferredServer"       /*设置主NTP服务器地址*/
#define METHOD_NTP_SET_ALTERNATIVE_SERVER        "SetAlternativeServer"     /*设置备NTP服务器地址*/
#define METHOD_NTP_SET_EXTRA_SERVER              "SetAlternativeServerArray" /*设置拓展的-NTP服务器地址*/
#define METHOD_NTP_SET_NTP_SERVER_COUNT          "SetNTPServerCount"        /*设置支持的NTP服务器地址的个数*/
#define METHOD_NTP_SET_NTP_SERVER                "SetNTPServer"             /*同时设置主备NTP服务器地址*/
#define METHOD_NTP_SET_MODE                      "SetNTPMode"               /*设置NTP的工作模式*/
#define METHOD_NTP_QUERY_SYNC_TIME_STATUS        "QuerySyncTimeStatus"      /*查询NTP同步时间的状态*/
#define METHOD_NTP_SET_AUTH_ENABLE_STATUS        "SetNTPAuthEnableStatus"   /* 设置证书类型 */
#define METHOD_NTP_IMPORT_GROUP_KEY              "ImportNTPGroupKey"        /* 导入组密钥 */
#define METHOD_NTP_GET_GROUP_KEY_STATUS          "GetNTPGroupKeyStatus"     /* 获取组密钥的状态 */
#define METHOD_NTP_SET_ALL_CONFIG                "SetAllConfig"             /*设置所有配置，主要用于Web上点击保存按钮*/
#define METHOD_NTP_SET_SMM_TIME                  "SetSmmTimeFromBmc"        /* 设置备控SMM板时间 */
#define METHOD_NTP_UPDATE_CIPHERTEXT             "UpdateNTPCiphertext"
/* BEGIN: Added by gwx455466, 2017/6/20 11:24:40   问题单号:AR-0000276590-016-003 */
#define METHOD_NTP_SET_MIN_POLLING_INTERVAL         "SetNtpMinPollingInterval"
#define METHOD_NTP_SET_MAX_POLLING_INTERVAL         "SetNtpMaxPollingInterval"
/* END:   Added by gwx455466, 2017/6/20 11:24:46 */
/* END: Added by fuzhengwen, PN:NTP, 2016/10/09 */

#define CLASS_UPLOAD_SENSITIVE_FILE "UploadSensitiveFile"
#define OBJECT_UPLOAD_SENSITIVE_FILE CLASS_UPLOAD_SENSITIVE_FILE
#define PROPERTY_SENSITIVE_FILE_MASTERKEY_ID "MasterKeyId"
#define METHOD_SENSITIVE_FILE_UPDATE_CIPHERTEXT "UpdateCiphertext"
#define METHOD_SENSITIVE_FILE_ENCRYPT_DATA "EncryptFileData"

/* BEGIN: Added by z00352904, 2016/11/15 15:34:39  PN:AR-0000264509-001-001 */
#define CLASS_SMS                                    "SMS"
#define OBJECT_CLASS_SMS                             "sms0"
#define PROPERTY_SMS_ID                              "Id"
#define PROPERTY_SMS_TYPE                            "Type"
#define PROPERTY_SMS_SMSNAME                         "SmsName"
#define PROPERTY_SMS_FIRMWAREVERSION                 "FirmwareVersion"
#define PROPERTY_SMS_STATUS                          "Status"

#define PROPERTY_SMS_CDEVDRIVENAME                   "CDEVDriveName"
#define PROPERTY_SMS_CDEVDRIVEVERSION                "CDEVDriveVersion"

#define PROPERTY_SMS_KBOXDRIVENAME                   "KBOXDriveName"
#define PROPERTY_SMS_KBOXDRIVEVERSION                "KBOXDriveVersion"

#define PROPERTY_SMS_EDMADRIVENAME                   "EDMADriveName"
#define PROPERTY_SMS_EDMADRIVEVERSION                "EDMADriveVersion"
#define PROPERTY_SMS_VNICDRIVENAME                   "VNICDriveName"
#define PROPERTY_SMS_VNICDEIVEVERSION                "VNICDriveVersion"
#define PROPERTY_SMS_IPADDR                          "IPAddr"
#define PROPERTY_SMS_PORT                            "Port"
#define PROPERTY_SMS_REGISTERSTATE                   "RegisterState"
#define PROPERTY_SMS_REGISTER_CODE                   "RegisterCode"
#define PROPERTY_SMS_HEALTHSTATE                     "HealthState"
#define PROPERTY_SMS_REDIRECT_WHITELIST              "RedirectWhiteList"
#define PROPERTY_SMS_UPDATE_STATUS                   "UpdateStatus"

/* BEGIN: Modified by k00400483, 2018/08/18 问题单号:AR.SR.SFEA02130924.036.001 */
#define PROPERTY_SMS_HTTPTYPE                     "HTTPType"
/* END:   Modified by k00400483, 2018/08/18 */

/* BEGIN: Added by qinjiaxiang, 2018/1/3   PN:AR.SR.SFEA02119720.003.001  */
#define PROPERTY_SMS_MINPORT                         "MinPort"
#define PROPERTY_SMS_MAXPORT                         "MaxPort"
#define PROPERTY_SMS_ALARMSTATE                      "alarmState"
#define PROPERTY_SMS_ALARMDESC                       "alarmDesc"
/* END:   Added by qinjiaxiang, 2018/1/3 */

#define PROPERTY_SMS_CDEV_CHANNEL_ENABLED "CdevChannelEnabled"
/* BEGIN: Added by lichangdi, 2018/1/7  PN:AR.SR.SFEA02119720.003.001  */
#define METHOD_SMS_SET_MAX_PORT                      "SetMaxPort"
#define METHOD_SMS_SET_MIN_PORT                      "SetMinPort"
#define METHOD_SMS_SET_ALARM_STATE                      "SetalarmState"
#define METHOD_SMS_SET_ALARM_DESC                      "SetalarmDesc"
/* END:   Added by lichangdi, 2018/1/7 */
#define METHOD_SMS_SET_CDEV_CHANNEL_ENABLED "SetCdevChannelEnabled"
#define METHOD_SMS_SET_CHANNEL_TYPE "SetBoBChannelType"
#define METHOD_SMS_GET_CHANNEL_TYPE "GetBoBChannelType"
#define METHOD_SMS_SET_READY_CHANNEL "SetReadyBoBChannel"

#define PROPERTY_SMS_BOB_CHANNEL_TYPE       "BoBChannelType"
#define PROPERTY_SMS_READY_BOB_CHANNEL      "ReadyBoBChannel"
#define PROPERTY_SMS_ENABLED                "SmsEnabled"


#define CLASS_LOCAL_HOST_AGENT_PORT                   "LocalHostAgentPort"
#define PROPERTY_LOCAL_HTTPS_AGENT_PORT              "LocalhostHttpsAgentPort"
#define PROPERTY_LOCAL_TRAP0_AGENT_PORT              "LocalhostTrapAgentPort0"
#define PROPERTY_LOCAL_TRAP1_AGENT_PORT              "LocalhostTrapAgentPort1"
#define PROPERTY_LOCAL_TRAP2_AGENT_PORT              "LocalhostTrapAgentPort2"
#define PROPERTY_LOCAL_TRAP3_AGENT_PORT              "LocalhostTrapAgentPort3"

#define METHOD_SMS_SET_REDIRECT_WHITELIST            "SetRedirectWhiteList"
#define METHOD_SMS_FORWARD_REDFISH_REQUEST           "ForwardRedfishRequest"
#define METHOD_SMS_HANDLE_REDFISH_EVENT              "HandleRedfishEvent"

/* BEGIN: Modified by jwx372839, 2017/3/27 21:1:20   问题单号:DTS2017031702127 */
#define METHOD_SMS_GET_BMA_CONNECT_STATE             "GetBMAConnectState"
/* END:   Modified by jwx372839, 2017/3/27 21:1:25 */

/* BEGIN: Modified by jwx372839, 2017/8/3 10:24:29   问题单号:DTS2017080104207 */
#define METHOD_SMS_UPDATE_BMA_SPECIFIC_RESOURCE      "UpdateBMASpecificRsc"
/* END:   Modified by jwx372839, 2017/8/3 10:24:34 */

/* BEGIN: Modified by jwx372839, 2017/8/3 19:19:29   问题单号:AREA02109379-006-001 */
#define METHOD_COMSYS_SET_BANDWIDTH_USAGETHRE        "SetBandwidthUsageThre"
/* END:   Modified by jwx372839, 2017/8/3 19:19:32 */
/* BEGIN: Modified by zwx772918, 2019/10/12  UADP441027,UADP441026支持修改hostname信息 */
#define METHOD_COMSYS_SET_SYNC_ENABLE                "SetHostnameSyncEnabled"
#define METHOD_COMSYS_SET_HOSTNAME                   "SetHostname"
/* END: Modified by zwx772918, 2019/10/12  UADP441027,UADP441026支持修改hostname信息 */
#define CLASS_COMPUTER_SYSTEM                        "ComputerSystem"
#define PROPERTY_COMPUTER_SYSTEM_ID                  "Id"
#define PROPERTY_COMPUTER_SYSTEM_OSVERSION           "OSVersion"
#define PROPERTY_COMPUTER_SYSTEM_KERNEL_VERSION      "KernelVersion"
#define PROPERTY_COMPUTER_SYSTEM_HOSTNAME            "HostName"
#define PROPERTY_COMPUTER_SYSTEM_DOMAIN              "Domain"
#define PROPERTY_COMPUTER_SYSTEM_DESCRIPTION         "Description"
#define PROPERTY_COMPUTER_SYSTEM_TOTALSYSMEM         "TotalSystemMemoryGiB"
#define PROPERTY_COMPUTER_SYSTEM_TOTALPHYMEM         "TotalPhysicalMemoryGiB"
#define PROPERTY_COMPUTER_SYSTEM_BUFFERMEM           "SystemMemoryBuffersGiB"
#define PROPERTY_COMPUTER_SYSTEM_CACHEDMEM           "SystemMemoryCachedGiB"
#define PROPERTY_COMPUTER_SYSTEM_FREE                "SystemMemoryFreeGiB"
#define PROPERTY_COMPUTER_SYSTEM_MEMUSAGE            "MemUsage"
#define PROPERTY_COMPUTER_SYSTEM_TOTALCPUUSAGE       "TotalCPUUsage"
#define PROPERTY_COMPUTER_SYSTEM_MAX_LOGICCPU_UASGE  "MaxLogicCPUUsage"
#define PROPERTY_COMPUTER_SYSTEM_PCHMAXTEMP          "PchMaxTempCelsius"
#define PROPERTY_COMPUTER_SYSTEM_NVMEMAXTEMP         "NvmeMaxTempCelsius"
/* BEGIN: Added by gwx455466, 2017/3/11   PN:PN:AR-0000276593-005-010 */
/* BEGIN: Added by gwx455466, 2017/3/11   PN:PN:AR-0000276593-005-010 */
#define PROPERTY_COMPUTER_SYSTEM_ARCH                "Arch"
#define PROPERTY_COMPUTER_SYSTEM_MAJOR_VERSION       "MajorVersion"
#define PROPERTY_COMPUTER_SYSTEM_MINOR_VERSION       "MinorVersion"
#define PROPERTY_COMPUTER_SYSTEM_SP_MAJOR_VERSION    "SpMajorVersion"
#define PROPERTY_COMPUTER_SYSTEM_SP_MINOR_VERSION    "SpMinorVersion"
#define PROPERTY_COMPUTER_SYSTEM_FAULT_DETAIL_STATUS "FaultDetailStatus"
/* END:   Added by gwx455466, 2017/3/11 */
#define PROPERTY_COMPUTER_SYSTEM_IBMA_EVENT          "iBMAEvent"
#define PROPERTY_COMPUTER_SYSTEM_READONLY_RF         "ReadonlyFSName"
#define PROPERTY_COMPUTER_SYSTEM_SYNC_ENABLE         "HostnameSyncEnabled"
/* BEGIN: Modified by jwx372839, 2017/8/3 19:23:44   问题单号:AREA02109379-006-001 */
#define PROPERTY_COMPUTER_SYSTEM_BANDWIDTH_USAGETHRE "BandwidthUsageThre"
/* END:   Modified by jwx372839, 2017/8/3 19:23:46 */
#define PROPERTY_COMPUTER_SYSTEM_LD_STATE_ABNORMAL   "LogicalDriveStateAbnormal"
#define METHOD_COMSYS_SET_LD_STATE_ABNORMAL          "SetLDStateAbnormal"

#define PROPERTY_DFTACTION_ACTION_TYPE                  "ActionType"
#define DFTACTION_READ_ACTION                      1
#define DFTACTION_WRITE_ACTION                    0
/* BEGIN: Added by z00352904, 2017/7/12 21:31:51  PN:SR-0000276585-011 */
#define DFTACTION_KEEP_ACTION                    2//将 Destination 中数据读出保存在 Data 属性中
/* END:   Added by z00352904, 2017/7/12 21:31:53*/

#define CLASS_OS_DRIVE_PARTITION                     "OSDrivePartition"
#define PROPERTY_OS_DRIVE_OS_DRIVE_NAME              "OSDriveName"
#define PROPERTY_OS_DRIVE_USAGE                      "Usage"
#define PROPERTY_OS_DRIVE_PARTITION_TOTALGB          "TotalCapacityGiB"
#define PROPERTY_OS_DRIVE_PARTITION_USEDGB           "UsedCapacityGiB"
#define PROPERTY_OS_DRIVE_VOLUME_NAME                "VolumeName"

#define CLASS_BUSINESS_PORT_VLAN                     "BusinessPortVLAN"
#define PROPERTY_BUSINESS_PORT_VLAN_ID               "Id"
#define PROPERTY_BUSINESS_PORT_VLAN_ENABLE           "VLANEnable"
#define PROPERTY_BUSINESS_PORT_VLAN_PRIORITY         "VLANPriority"
#define PROPERTY_BUSINESS_PORT_VLAN_ETHBDF           "EthBDF"
/* BEGIN: Modified by zwx382755, 2018/3/23   PN:AR.SR.SFEA02119720.021.001 */
#define PROPERTY_BUSINESS_PORT_VLAN_IPV4_INFO        "IPv4Info"
#define PROPERTY_BUSINESS_PORT_VLAN_IPV6_INFO        "IPv6Info"
#define PROPERTY_BUSINESS_PORT_VLAN_CONFIG_STATE     "ConfigState"
#define PROPERTY_BUSINESS_PORT_VLAN_CONFIG_RESULT    "ConfigResult"
#define PROPERTY_BUSINESS_PORT_VLAN_IS_ONBOOT        "IsOnBoot"
#define METHOD_BUSINESS_PORT_VLAN_SET_VLAN_PROP      "SetVLANProp"
/* END:   Modified by zwx382755, 2018/3/23 */

#define CLASS_RESIDENT_STORAGE_MGNT                  "ResidentStorageMgnt"
#define PROPERTY_RESIDENT_STORAGE_LIGHT_FLAG         "LightFlag"
#define METHOD_SET_OBJ_FROM_BMA                      "SetBMCObjFromBMA"
#define METHOD_SET_LIGHT_FLAG                        "SetLightFlag"

#define CLASS_RESIDENT_NET_CONFIG                    "ResidentNetConfig"
#define METHOD_SET_NET_OBJ_FROM_BMA                  "SetBMCNetObjFromBMA"
/* END:   Added by z00352904, 2016/11/15 15:34:42*/

/* BEGIN: Added by Yangshigui YWX386910, 2017/12/13   问题单号:DTS2017112702299*/
#define CLASS_LOGICAL_DRIVE_ALERT_INFO                      "LogicalDriveAlertInfo"
#define PROPERTY_LOGICAL_DRIVE_ALERT_INFO_IS_EXIST          "IsExist"
#define PROPERTY_LOGICAL_DRIVE_ALERT_INFO_ID                "Id"
#define PROPERTY_LOGICAL_DRIVE_ALERT_INFO_ALERT_STATUS      "AlertStatus"
#define PROPERTY_LOGICAL_DRIVE_ALERT_INFO_ALERT_TYPE_DESC   "AlertTypeDesc"
#define PROPERTY_LOGICAL_DRIVE_ALERT_INFO_IBMA_IS_EXIST     "iBMAIsExist"
#define PROPERTY_LOGICAL_DRIVE_ALERT_INFO_IBMA_STATUS       "iBMAAlertStatus"
/* END:   Added by Yangshigui YWX386910, 2017/12/13 */

/* BEGIN: Added by Yangshigui YWX386910, 2018/1/3   问题单号:AR.SR.SFEA02109778.006.003/004/005*/
#define CLASS_STORAGE_DIAGNOSE                                              "StorageDiagnose"
#define PROPERTY_STORAGE_DIAGNOSE_PHY_ERR_ENABLE                            "PHYErrEnable"
#define PROPERTY_STORAGE_DIAGNOSE_PHY_ERR_INTERVAL                          "PHYErrInterval"
#define PROPERTY_STORAGE_DIAGNOSE_PHY_ERR_MAX_RECORD                        "PHYErrMaxRecord"
#define PROPERTY_STORAGE_DIAGNOSE_PHY_ERR_THRESHOLD                         "PHYErrThreshold"
#define PROPERTY_STORAGE_DIAGNOSE_PHY_ERR_SEL_DESC                          "PHYErrSelDesc"
#define PROPERTY_STORAGE_DIAGNOSE_PHY_ERR_LOGROTATE_NUM                     "PHYErrLogrotateNum"
#define PROPERTY_STORAGE_DIAGNOSE_PD_LOG_ENABLE                             "PDLogEnable"
#define PROPERTY_STORAGE_DIAGNOSE_PD_LOG_START_TIME                         "PDLogStartTime"
#define PROPERTY_STORAGE_DIAGNOSE_PD_LOG_INTERVAL                           "PDLogInterval"
#define PROPERTY_STORAGE_DIAGNOSE_PD_LOG_LAST_TIME                          "PDLogLastTime"
#define PROPERTY_STORAGE_DIAGNOSE_PD_LOGROTATE_NUM                          "PDLogrotateNum"
/* BEGIN: Added by x00453037, 2018/9/17   问题单号:AR.SR.SFEA02130924.009.007*/
#define PROPERTY_STORAGE_DIAGNOSE_PD_FARM_LOGROTATE_NUM                 "PDFARMLogrotateNum"
/* END:   Added by x00453037, 2018/9/17 */
#define PROPERTY_STORAGE_DIAGNOSE_PD_MEDIA_WEAROUT_THRESHOLD                "PDMediaWearoutThreshold"
#define PROPERTY_STORAGE_DIAGNOSE_PD_LOG_BMAID                         "PdLogBMAId"
#define PROPERTY_STORAGE_DIAGNOSE_PHY_ERR_SERIAL_DIAG_THRESHOLD             "PHYErrSerialDiagThreshold"
#define PROPERTY_STORAGE_DIAGNOSE_PHY_ERR_RECENT_DIAG_THRESHOLD             "PHYErrRecentDiagThreshold"
#define PROPERTY_STORAGE_DIAGNOSE_IO_DETERIORATION_ENABLE                   "IODeteriorationEnable"
#define PROPERTY_STORAGE_DIAGNOSE_IO_DETERIORATION_UNC_THRESHOLD            "IODeteriorationUNCThre"
#define PROPERTY_STORAGE_DIAGNOSE_IO_DETERIORATION_UNC_INCREASE_THRESHOLD   "IODeteriorationUNCIncThre"
#define PROPERTY_STORAGE_DIAGNOSE_IO_DETERIORATION_UNC_PERIOD               "IODeteriorationUNCPeriod"
#define PROPERTY_STORAGE_DIAGNOSE_IO_DETERIORATION_ERR_THRESHOLD            "IODeteriorationOtherErrThre"
#define PROPERTY_STORAGE_DIAGNOSE_IO_DETERIORATION_ERR_INCREASE_THRESHOLD   "IODeteriorationOtherErrIncThre"
#define PROPERTY_STORAGE_DIAGNOSE_IO_DETERIORATION_ERR_PERIOD               "IODeteriorationOtherErrPeriod"
#define PROPERTY_STORAGE_DIAGNOSE_IO_DETERIORATION_CMD_THRESHOLD            "IODeteriorationCMDThre"
#define PROPERTY_STORAGE_DIAGNOSE_IO_DETERIORATION_CMD_PERIOD               "IODeteriorationCMDPeriod"
#define PROPERTY_STORAGE_DIAGNOSE_IO_DETERIORATION_WHITE_BOX_ENABLE         "IODeteriorationWhiteEnable"
#define PROPERTY_STORAGE_DIAGNOSE_IO_DETERIORATION_BER_THRESHOLD            "IODeteriorationBERThre"
#define PROPERTY_STORAGE_DIAGNOSE_IO_DETERIORATION_FLY_HEI_THRESHOLD        "IODeteriorationFlyHeiThre"
#define PROPERTY_STORAGE_DIAGNOSE_IO_DETERIORATION_MR_RES_THRESHOLD         "IODeteriorationMRResThre"
#define PROPERTY_STORAGE_DIAGNOSE_RAID_OOB_ENABLE                           "RaidOOBEnable"
#define METHOD_STORAGE_DIAGNOSE_SET_PHY_ERR_INTERVAL                        "SetPHYErrInterval"
#define METHOD_STORAGE_DIAGNOSE_SET_PHY_ERR_MAX_RECORD                      "SetPHYErrMaxRecord"
#define METHOD_STORAGE_DIAGNOSE_SET_PHY_ERR_THRESHOLD                       "SetPHYErrThreshold"
#define METHOD_STORAGE_DIAGNOSE_SET_PHY_ERR_LOGROTATE_NUM                   "SetPHYErrLogrotateNum"
#define METHOD_STORAGE_DIAGNOSE_SET_PD_LOG_ENABLE                           "SetPDLogEnable"
#define METHOD_STORAGE_DIAGNOSE_SET_PD_LOG_START_TIME                       "SetPDLogStartTime"
#define METHOD_STORAGE_DIAGNOSE_SET_PD_LOG_INTERVAL                         "SetPDLogInterval"
#define METHOD_STORAGE_DIAGNOSE_SET_PD_MEDIA_WEAROUT_THRESHOLD              "SetPDMediaWearoutThreshold"
/* BEGIN: Added by x00453037, 2018/9/18   问题单号:AR.SR.SFEA02130924.009.007*/
#define METHOD_STORAGE_DIAGNOSE_SET_PD_FARM_LOGROTATE_NUM                   "SetPDFARMLogrotateNum"
/* END:   Added by x00453037, 2018/9/18 */
#define METHOD_STORAGE_DIAGNOSE_HANDLE_POST_MESSAGE                         "HandlePostMessage"
/* END:   Added by Yangshigui YWX386910, 2018/1/3 */
#define METHOD_STORAGE_DIAGNOSE_SET_PHY_ERR_SERIAL_DIAG_THRESHOLD           "SetPHYErrSerialDiagThreshold"
#define METHOD_STORAGE_DIAGNOSE_SET_PHY_ERR_RECENT_DIAG_THRESHOLD           "SetPHYErrRecentDiagThreshold"
#define METHOD_STORAGE_DIAGNOSE_SET_RAID_OOB_ENABLE                         "SetRaidOOBEnable"

/*BEGIN : Added by y00383654 2016/10/14 新增SD100卡相关属性 */
#define CLASS_PCIE_SDI_CARD                     "PCIeSDICard"
#define PROPERTY_SDI_CARD_SLOT                  "Slot"
#define PROPERTY_SDI_CARD_M2                    "M2Presence"
#define PROPERTY_SDI_CARD_DIMM                  "SODIMMPresence"
#define PROPERTY_SDI_CARD_NETCARD               "NetCardPresence"
#define PROPERTY_SDI_CARD_BOOT_ORDER            "BootOrder"
#define PROPERTY_SDI_CARD_BOOT_VALID_TYPE       "BootOrderValidType"
#define PROPERTY_SDI_CARD_SYSTEM_STATUS         "SystemLoadedStatus"
#define PROPERTY_SDI_CARD_STORAGE_IP_ADDR       "StorageIpAddr"
#define PROPERTY_SDI_CARD_STORAGE_VLAN          "StorageIpVlan"
#define PROPERTY_SDI_CARD_M3_ACCESSOR           "M3"
#define PROPERTY_SDI_CARD_CPLD_ACCESSOR         "Cpld"
#define PROPERTY_SDI_PCIE_TEST_REG              "PcieTestReg"
#define PROPERTY_SDI_80_PORT_REG                "80PortReg"
#define PROPERTY_SDI_STATUS_REG                 "SDIEthStatusReg"
#define PROPERTY_SDI_IP_REG                     "SDIEthIpReg"
#define PROPERTY_SDI_VLAN_REG                   "SDIEthVlanReg"
#define PROPERTY_SDI_FSA_STATUS_REG             "FSAStatusReg"
#define PROPERTY_SDI_FSA_WRITE_IP_REG           "FSAWriteIpReg"
#define PROPERTY_SDI_FSA_WRITE_STATUS_REG       "FSAWriteStatusReg"
#define PROPERTY_SDI_CARD_DESC                  "Desc"
#define PROPERTY_SDI_CPLD_VERSION_REG           "CpldVerReg"
#define PROPERTY_SDI_CARD_BOARDNAME             "BoardName"
#define PROPERTY_SDI_EXT_CARD_MAC_ADDR          "ExtCardMacReg"
#define PROPERTY_SDI_MULTI_HOST_EXT_CARD_MAC_ADDR          "MultiHostExtCardMacReg"
#define PROPERTY_SDI_CARD_EID                   "Eid"
#define PROPERTY_SDI_CARD_PHY_ADDR              "PhyAddr"
#define PROPERTY_SDI_CARD_REF_COMPONENT         "RefComponent"
#define PROPERTY_SDI_CARD_SUPPORT               "MctpSupport"
#define PROPERTY_SDI_SERIAL_RECORD_CONNECT      "SerialRecordConnect"
#define PROPERTY_SDI_DOG_TIMEOUT                "WatchdogTmout"
#define PROPERTY_SDI_FAULT_CODE_BITMAP          "FaultCodeBitMap"
#define PROPERTY_SDI_RESET_LINKAGE_POLICY       "ResetLinkagePolicy"

#define METHOD_GET_SDI_CARD_SET_IP_STATUS       "GetOsSetIpStatus"
#define METHOD_GET_SDI_FSA_IP_STATUS            "GetFSAIpStatus"
#define METHOD_SET_SDI_PCIE_TEST_REG            "SetPcieTestReg"
#define METHOD_SET_SDI_CARD_EID                 "SetEidToSDICard"
#define METHOD_SET_SDI_CARD_NMI            "SetSDICardNMI"
#define METHOD_SET_SDI_BOOT_OPTION               "SetSDIBootOption"

/*END : Added by y00383654 2016/10/14 新增SD100卡相关属性*/
/* BEGIN: Added by zwx388296, 2018/1/18   问题单号:DTS2018010907429 */
#define PROPERTY_SDI_CARD_BOARDID               "BoardId"
#define PROPERTY_SDI_CARD_BOARD_NAME                "Name"
/* END:   Added by zwx388296, 2018/1/18 */
/* BEGIN : Added by d00381887, 2017/08/20,PN: AR-0000276585-009-002 */

#define PROPERTY_SDI_IPV4_ADDRREG               "IPV4AddrReg"
#define PROPERTY_SDI_IPV6_ADDRREG               "IPV6AddrReg"

#define CLASS_REMOTE_PCIE_FPGA_CARD				"RemotePCIeFPGACard"
#define PROPERTY_REMOTE_PCIE_FPGA_CARD_TARGET   "Target"
#define CLASS_PCIE_FPGA_CARD                    "PCIeFPGACard"
#define PROPERTY_FPGA_CARD_NAME                 "Name"
#define PROPERTY_FPGA_CARD_DESC                 "Desc"
#define PROPERTY_FPGA_CARD_SLOT                 "Slot"
#define PROPERTY_FPGA_CARD_BOARDID              "BoardId"
#define PROPERTY_FPGA_CARD_PCBID                "PcbId"
#define PROPERTY_FPGA_CARD_PCBVER               "PcbVer"
#define PROPERTY_FPGA_CARD_MANU                 "Manufacturer"
#define PROPERTY_FPGA_CARD_BOARDNAME            "BoardName"
#define PROPERTY_FPGA_CARD_HEALTH               "Health"
#define PROPERTY_FPGA_CARD_SERIALNAME           "SerialNumber"
#define PROPERTY_FPGA_CARD_POWER                "Power"
#define PROPERTY_FPGA_CARD_LOGIC_UNIT           "LogicUnit"
#define PROPERTY_FPGA_CARD_LOGIC_VERID          "LogicVerId"
#define PROPERTY_FPGA_CARD_LOGIC_VER            "LogicVer"
/* END : Added by d00381887, 2017/08/20,PN: AR-0000276585-009-002 */

/* BEGIN: Added by h00371221, 2018/03/19   SR.SFEA02119719.013*/
#define PROPERTY_FPGA_CARD_DIMM_TEMP            "MaxDIMMTemp"
#define PROPERTY_FPGA_CARD_SHELL_ID             "ShellID"
/* END  : Added by h00371221, 2018/03/19   SR.SFEA02119719.013*/
/* BEGIN: Modified by h00422363, 2018/3/23 15:28:45   问题单号: SR.SFEA02130917.004.001 FPGA卡基本信息查询 */
#define PROPERTY_FPGA_CARD_MCUSUPPORTED      "McuSupported"
#define PROPERTY_FPGA_CARD_MCUVERSION           "McuVersion"
#define PROPERTY_FPGA_CARD_SCALERATION          "ScaleRatio"
#define PROPERTY_FPGA_CARD_POWERDOUBLE          "PowerDouble"
/* END:   Modified by h00422363, 2018/3/23 15:29:2 */

/* SDI MCU */
#define PROPERTY_SDI_CARD_MCUSUPPORTED PROPERTY_FPGA_CARD_MCUSUPPORTED
#define PROPERTY_SDI_CARD_MCUVERSION PROPERTY_FPGA_CARD_MCUVERSION
#define PROPERTY_SDI_CARD_SCALERATION PROPERTY_FPGA_CARD_SCALERATION
#define PROPERTY_SDI_CARD_POWERDOUBLE PROPERTY_FPGA_CARD_POWERDOUBLE
#define PROPERTY_SDI_CARD_POWER PROPERTY_FPGA_CARD_POWER

#define CLASS_GPU_CARD                              "GPUCard"
#define PROPERTY_GPU_CARD_NAME                      "Name"
#define PROPERTY_GPU_CARD_TYPE                      "Type"
#define PROPERTY_GPU_CARD_DESC                      "Desc"
#define PROPERTY_GPU_CARD_SLOT                      "Slot"
#define PROPERTY_GPU_CARD_BOARDID                   "BoardId"
#define PROPERTY_GPU_CARD_PCBID                     "PcbId"
#define PROPERTY_GPU_CARD_PCBVER                    "PcbVer"
#define PROPERTY_GPU_CARD_MANU                      "Manufacturer"
#define PROPERTY_GPU_CARD_BOARDNAME                 "BoardName"
#define PROPERTY_GPU_CARD_HEALTH                    "Health"
#define PROPERTY_GPU_CARD_POWER_STATE               "PowerState"
#define PROPERTY_GPU_CARD_DEV_ACCESSOR              "DeviceAccessor"
#define PROPERTY_GPU_CARD_GPUTYPE                   "GPUType"
#define PROPERTY_GPU_CARD_MODEL                     "Model"
#define PROPERTY_GPU_CARD_POWER                     "Power"
#define PROPERTY_GPU_CARD_SER_NUM                   "SerialNumber"
#define PROPERTY_GPU_CARD_FIRM_VER                  "FirmwareVersion"
#define PROPERTY_GPU_CARD_SBE_COUNT                 "SBECount"
#define PROPERTY_GPU_CARD_DBE_COUNT                 "DBECount"
#define PROPERTY_GPU_CARD_NVLINK_NUM                "NvLinkNum"
#define PROPERTY_GPU_CARD_NVLINK_STATUS             "NvLinkStatus"
#define PROPERTY_GPU_CARD_LINK_SPEED_MBPS           "LinkSpeedMbps"
#define PROPERTY_GPU_CARD_REPLAY_ERROR_COUNT        "ReplayErrorCount"
#define PROPERTY_GPU_CARD_RECOVERY_ERROR_COUNT      "RecoveryErrorCount"
#define PROPERTY_GPU_CARD_FLIT_CRC_ERROR_COUNT      "FlitCRCErrorCount"
#define PROPERTY_GPU_CARD_DATA_CRC_ERROR_COUNT      "DataCRCErrorCount"
#define PROPERTY_GPU_CARD_INFO_ROM_VERSION          "InfoRomVersion"
#define PROPERTY_GPU_CARD_LINK_WIDTH_ABILITY        "LinkWidthAbility"
#define PROPERTY_GPU_CARD_LINK_SPEED_ABILITY        "LinkSpeedAbility"
#define PROPERTY_GPU_CARD_LINK_WIDTH                "LinkWidth"
#define PROPERTY_GPU_CARD_LINK_SPEED                "LinkSpeed"
#define PROPERTY_GPU_CARD_BOARD_PART_NUMBER         "BoardPartNumber"
#define PROPERTY_GPU_CARD_GPU_PART_NUMBER           "GPUPartNumber"
#define PROPERTY_GPU_CARD_MEMORY_VENDOR             "MemoryVendor"
#define PROPERTY_GPU_CARD_MEMORY_PART_NUMBER        "MemoryPartNumber"
#define PROPERTY_GPU_CARD_BUILD_DATE                "BuildDate"
#define PROPERTY_GPU_CARD_UUID                      "UUID"
#define PROPERTY_GPU_CARD_POWER_BRAKE_IS_SET        "PowerBrakeIsSet"
#define PROPERTY_GPU_CARD_SUFFI_EX_POWER            "SufficientExternalPower"
#define PROPERTY_GPU_CARD_ECC_MODE_ENABLED          "ECCModeEnabled"
#define PROPERTY_GPU_CARD_ECC_MODE_PEND_ENSBLED     "ECCModePendingEnabled"
#define PROPERTY_GPU_CARD_GPU_RESET_REQUIRED        "GPUResetRequired"
#define PROPERTY_GPU_CARD_SINGLE_BIT_ERROR_COUNTS   "SingleBitErrorCounts"
#define PROPERTY_GPU_CARD_DOUBLE_BIT_ERROR_COUNTS   "DoubleBitErrorCounts"
#define PROPERTY_GPU_CARD_THERMALS_ALTERT_STATE     "ThermalsAlertState"
#define PROPERTY_GPU_CARD_PRIMARY_GPU_TEMP_CELSIUS  "PrimaryGPUTemperatureCelsius"
#define PROPERTY_GPU_CARD_MEMORY_TMP_CELSIUS        "MemoryTemperatureCelsius"

#define PROPERTY_GPU_CARD_BMAID                     "BMAId"
#define PROPERTY_GPU_CARD_GPUUTILIZATION            "GPUUtilization"
#define PROPERTY_GPU_CARD_MEMORYUTILIZATION         "MemoryUtilization"
#define METHOD_SET_GPU_SUMMARY_FROM_BMA             "SetGPUStatisticsObjFromBMA"
/* BEGIN : Added by l00375984, 2017/11/15, AR:支持获取GPU卡HBM温度 */
#define CLASS_NVGPU_ATCION                      "NvGPUAction"
#define PROPERTY_NVGPU_REFDEV                   "RefDevice"
#define PROPERTY_NVGPU_NAME                     "Name"
#define PROPERTY_NVGPU_OPCODE                   "OpCode"
#define PROPERTY_NVGPU_ARG1                     "Arg1"
#define PROPERTY_NVGPU_ARG2                     "Arg2"
#define PROPERTY_NVGPU_INPUT_DATA               "InputData"
#define PROPERTY_NVGPU_CAP_DWORD_NUM            "CapaDWordNum"
#define PROPERTY_NVGPU_CAP_MASK                 "CapabilityMask"
#define PROPERTY_NVGPU_DATA                     "Data"
#define PROPERTY_NVGPU_DATA_TYPE                "DataType"
#define PROPERTY_NVGPU_DATA_LENGTH              "DataLength"
#define PROPERTY_NVGPU_DATA_MASK                "DataMask"
/* END : Added by l00375984, 2017/11/15, AR:支持获取GPU卡HBM温度 */
/* BEGIN: Modified by wangpenghui wwx382232, 2019/1/23 14:8:31   问题单号:DTS2018071206794  */
#define PROPERTY_NVGPU_LAST_LOG_TIME              "LastLogTime"
#define PROPERTY_NVGPU_LOG_COUNT              "LogCount"
/* END:   Modified by wangpenghui wwx382232, 2019/1/23 14:8:34 */
/* Start by h00272616 AR-0000256847-006-002 */
#define CLASS_CONDITION_ATCION                       "ConditionAction"
#define PROPERTY_CONDITION_ATCION_DESTINATION        "Destination"
#define PROPERTY_CONDITION_ATCION_DATA               "Data"
#define PROPERTY_CONDITION_ATCION_VERIFY_ENABLE      "VerifyEnable"
#define PROPERTY_CONDITION_ATCION_VERIFY_RESULT      "VerifyResult"
#define PROPERTY_CONDITION_ATCION_DELAY_MS           "Delayms"
#define PROPERTY_CONDITION_ATCION_ACTION_REF_OBJ     "ActionRefObj"
#define PROPERTY_CONDITION_ATCION_ACTION_METHOD      "ActionMethod"
#define METHOD_HANDLE_ASCONDITION_ATCION             "ActionSync"

#define CLASS_ATOMICACTION_ACTION                    "AtomicAction"
#define PROPERTY_ATOMICACTION_LOCK_ACTION            "LockAction"
#define PROPERTY_ATOMICACTION_REF_ACTION_OBJ         "ActionRefObj"
#define PROPERTY_ATOMICACTION_UNLOCK_ACTION          "UnLockAction"
#define PROPERTY_ATOMICACTION_RETRY_COUNT            "RetryCount"
/* End by h00272616 AR-0000256847-006-002 */

/* Start by h00272616 DTS2016112209471  20161118 */
#define CLASS_EVENT_DESCP_CFG_TO_HMM                 "EventDescpCfgToHmm"
#define PROPERTY_EVENT_DESCP_CFG_TO_HMM_REF_OBJ      "EventDescRefObj"
#define PROPERTY_EVENT_DESCP_CFG_TO_HMM_EVENT_FLAG0  "EventData0Flag"
#define PROPERTY_EVENT_DESCP_CFG_TO_HMM_EVENT_FLAG1  "EventData1Flag"
#define PROPERTY_EVENT_DESCP_CFG_TO_HMM_EVENT_FLAG2  "EventData2Flag"
/* End by h00272616 DTS2016112209471  20161118 */
/* BEGIN : Added by cwx290152, 2016/12/09,PN: AR-0000256853-001-005,AR-0000256853-001-006 */
#define CLASS_DEVICE_CHIP                           "DeviceChip"
#define PROPERTY_DEVICE_CHIP                        "Chip"
#define PROPERTY_DEVICE_DEVICE_TYPE                 "DeviceType"
#define PROPERTY_DEVICE_SLOT                        "Slot"
/* END : Added by cwx290152, 2016/12/09,PN: AR-0000256853-001-005,AR-0000256853-001-006 */

/* Start by huangjiaqing AR-0000276585-002-021 2017-04-22 */
#define CLASS_CHIP_UPGRADE_CFG              ("ChipFirmwareMgnt")
/*BEGIN: Added by y00204747, 2019.6.19, pn: UADP213242 支持CPU loadline功能*/
#define PROPERTY_FIRMWARE_CPU_ID            ("Id") // 新增CPU ID
#define PROPERTY_FIRMWARE_VRD_ID            ("VrdId")
#define PROPERTY_CHIP_TYPE                  ("ChipType")
#define PROPERTY_REF_CHIP_OBJECT            ("RefChip")
#define PROPERTY_REF_CHIP_OBJECT2           ("RefChip2")
#define PROPERTY_RECOVERY_CHIP_OBJECT       ("RecoverChip")
#define PROPERTY_RECOVERY_CHIP_OBJECT2      ("RecoverChip2")
#define PROPERTY_FIRMWARE_FILE_NAME         ("FirmwareFileName")
#define PROPERTY_BOOT_FIRMWARE_FILE_NAME    ("BootFwFileName")
#define PROPERTY_FIRMWARE_LIB_NAME          ("FirmwareLibName")
#define PROPERTY_FIRMWARE_UPDATE_FLAG       ("UpdateFlag")
#define PROPERTY_REMAINNING_UPDATE_TIME   ("RemainingWrites")
#define PROPERTY_FIRMWARE_VERSION           ("FirmwareVersion")
#define PROPERTY_EXPECT_FIRMWARE_VERSION    ("ExpectFirmwareVersion")
#define PROPERTY_CHIP_UPGRADE_PREPAREACTION  "PrepareAction"
#define PROPERTY_CHIP_UPGRADE_FINISHACTION   "FinishAction"
/* End by huangjiaqing AR-0000276585-002-021 2017-04-22 */
#define PROPERTY_FIRMWARE_COMPONENT_OBJECT   "ComponentObj"

/* BEGIN: VRD升级. Added by chenfeng cwx290152, 2017/5/22 */
#define OBJECT_VRD_CPLD2_CHIP               "Cpld2WriteChip"

#define CLASS_VRD_UPGRADE_MGNT              "VRDUpgradeMgnt"
#define PROPERTY_VRD_UPGRADE_CHIPTYPE_ARRAY "ChipTypeArray"
#define PROPERTY_VRD_UPGRADE_REFER_CHIP     "ReferCPLDChip"
#define PROPERTY_VRD_UPGRADE_BOMID          "BomID"
#define PROPERTY_VRD_UPGRADE_VALIDATEVRD    "ValidateVRD"
#define PROPERTY_VRD_UPGRADE_VALID_STATUS   "ValidateStatue"
#define PROPERTY_VRD_CPLD2_VERSION_REG      "Cpld2VerReg"
#define PROPERTY_VRD_CPLD2_VERSION          "Cpld2Version"
#define PROPERTY_VRD_CPLD2_UNIT_NUMBER      "Cpld2UnitNum"
#define PROPERTY_VRD_UPGRADE_FLAG           "UpgradeFlag"
#define PROPERTY_VRD_UPGRADE_WAY            "VrdUpgradeType"

#define PROPERTY_VRD_VALIDATE_CONTROL_TYPE  "ValidateControlType"
#define PROPERTY_VRD_VALIDATE_FLAG          "ValidateFlag"
#define PROPERTY_VRD_ACTION_AFTER_VALIDATE  "ActionAfterValidateVRD"
#define METHOD_VRD_SET_VALIDATE_FLAG        "SetVRDValidateFlag"
#define METHOD_VRD_SET_VALIDATE_ACTION      "SetVRDValidateAction"

#define PROPERTY_VRD_VERSION_CHANGE_FLAG    "VersionChangeFlag"
#define PROPERTY_VRDVERSION                 "VRDVersion"
#define PROPERTY_COMPONENT_OBJECT           "ComponentObj"
/* BEGIN lilie */
#define PROPERTY_VRD_UPGRADE_PROTECTION     "WriteProtection"
/* END lilie */

/* END: VRD升级. Added by chenfeng cwx290152, 2017/5/22 */
#define CLASS_VRD_UPGRADE_MGNT_ASSIST       "VRDUpgradeMgntAssist"
#define PROPERTY_VRD_UPGRADE_ASSIST_VALIDATEVRD    "ValidateVRD"
#define PROPERTY_VRD_UPGRADE_ASSIST_VALID_STATUS   "ValidateStatue"
/*BEGIN : Added by z00397989,2017/03/02 .PN:  AR-0000268578-009-003,AR-0000268578-002-007  */
#define CLASS_SUPPORTCMCVMA                    "Supportcmcvma"
#define OBJECT_SUPPORTCMCVMA_IPINFO                     "Supportcmcvmaipinfo"
#define PROPERTY_SUPPORTCMCVMA_VMAWORKSTATUS                    "VMAWorkStatus"
#define PROPERTY_SUPPORTCMCVMA_CMCFLOATIP                  "CMCFloatIP"
#define PROPERTY_SUPPORTCMCVMA_CMCFLOATMASK                    "CMCFloatMASK"
#define PROPERTY_SUPPORTCMCVMA_HYPERVISORIP                    "HyperVisorIP"
#define PROPERTY_SUPPORTCMCVMA_HYPERVISORMASK                      "HyperVisorMASK"
/*END : Added by z00397989,2017/03/02 .PN:  AR-0000268578-009-003,AR-0000268578-002-007  */
#define PROPERTY_SUPPORTCMCVMA_CMCGATEWAYIP                  "CMCGateWayIP"
#define PROPERTY_SUPPORTCMCVMA_HYPERVISORGATEWAYIP                  "HyperVisorGateWayIP"


#define CLASS_HDD_BASEBOARD                 "HddBaseBoard"
#define PROPERTY_BASEBOARD_RAW_TEMP         "RawTemp"
#define PROPERTY_BASEBOARD_SYNC_RAW_TEMP    "SyncRawTemp"
#define PROPERTY_BASEBOARD_IIC_STATE        "IICState"
#define PROPERTY_BASEBOARD_SYNC_IIC_STATE   "SyncIICState"
#define METHOD_PROPERTY_SYNC                "PropertySyncMethod"

#define CLASS_CHIP_VALUE_CHECKER                          "ChipValueChecker"
#define PROPERTY_CHIP_VALUE_CHECKER_ENABLE  "Enable"
#define PROPERTY_REF_CHIP_VALUE_READER_OBJ  "RefObj"
#define PROPERTY_OBJ_OPERATOR                            "ObjOperator"
#define PROPERTY_VDDQ_CHECKER_Action                "Action"
#define PROPERTY_VDDQ_FORCE_PWR_OFF_FLAG            "ForcePwrOffFlag"

#define CLASS_CHIP_VALUE_READER                                                  "ChipValueReader"
#define PROPERTY_CHIP_VALUE_READER_ENABLE                              "Enable"
#define PROPERTY_CHIP_VALUE_READER_TRESHOLD_VALUE              "Threshold"
#define PROPERTY_CHIP_VALUE_READER_OPERATOR                          "Operator"
#define PROPERTY_CHIP_VALUE_READER_OPERATOR_RESULT            "OperatorResult"
#define PROPERTY_BEFORE_READ_WRITE_OBJ                                    "RegWriteRefObj"
#define PROPERTY_BEFORE_READ_WRITE_DATA                                 "RegWriteData"
#define PROPERTY_READ_OBJ                                                               "RegReadRefObj"
#define PROPERTY_AFTER_READ_WRITE_OBJ                                      "RegWriteRefObj2"
#define PROPERTY_AFTER_READ_WRITE_DATA                                   "RegWriteData2"
#define PROPERTY_VDDQ_VLOT_RAW_VALUE                                       "RawValue"
#define PROPERTY_M_SCALE_VALUE                                                    "M"
#define PROPERTY_VDDQ_VOLT_REAL_VALUE                                      "Value"
#define PROPERTY_CHIPVALUE_DEBOUNCE_VALUE                             "DebounceValue"
#define PROPERTY_CHIPVALUE_DEBOUNCE_COUNT                             "DebounceCount"

/* Start by huangjiaqing DTS2017091811440 20170919 */
#define PROPERTY_CHIPVALUE_MIN_REASONABLE_VALUE                  "NormalMin"
#define PROPERTY_CHIPVALUE_MAX_REASONABLE_VALUE                  "NormalMax"
/* End by huangjiaqing DTS2017091811440 20170919 */

/* BEGIN : Added by d00381887, 2016/07/12,PN: AR-0000276585-009-002 */
#define CLASS_REG_ACTION                             "RegAction"
#define PROPERTY_REG_ACTION_SLOT_ID                  "SlotId"
#define PROPERTY_REG_ACTION_OVERALL_INDEX            "OverallIndex"
#define PROPERTY_REG_ACTION_DEVICE_NAME              "DeviceName"
#define PROPERTY_REG_ACTION_TRIGGER_TYPE             "TriggerType"
#define PROPERTY_REG_ACTION_CONDITION                "Condition"
#define PROPERTY_REG_ACTION_CONDITION_SRC            "ConditionSrc"
#define PROPERTY_REG_ACTION_REF_CHIP                 "RefChip"
#define PROPERTY_REG_ACTION_LOGIC_UNIT_ARRAY         "LogicUnitArray"
#define PROPERTY_REG_ACTION_CONFIG_TABLE             "ConfigTable"
#define PROPERTY_REG_ACTION_CONFIG_RESULT            "ConfigResult"
#define METHOD_START_REG_ACTION                      "StartRegAction"
#define METHOD_CHECK_REG_ACTION                      "CheckRegAction"
#define METHOD_CHECK_REG_LOG_TYPE                    "LogType"

#define CLASS_REG_CONFIG                             "RegConfig"
#define PROPERTY_REG_CONFIG_OFFSET                   "Offset"
#define PROPERTY_REG_CONFIG_DATASET                  "DataSet"
#define PROPERTY_REG_CONFIG_MASKSET                  "MaskSet"
/* BEGIN: Added by dingzhixiong 00381887, 2018/1/12   PN:DTS2017122306946 */
#define PROPERTY_REG_CONFIG_IS_SHIELD_CHECK          "IsShieldCheck"
/* END: Added by dingzhixiong 00381887, 2018/1/12 */
#define PROPERTY_REG_CONFIG_VERIFY_ENABLE            "VerifyEnable"
#define PROPERTY_REG_CONFIG_VERIFY_INTERVAL          "VerifyInterval"
#define PROPERTY_REG_CONFIG_RETRY_COUNT              "RetryCount"
#define PROPERTY_REG_CONFIG_CHECK_BEFORE_WRITE       "CheckBeforeWrite"

#define CLASS_REPEATER_MGNT                          "RepeaterMgnt"
#define PROPERTY_REPEATER_MGNT_REG_ACTION_INDEX      "RegActionIndex"
#define PROPERTY_REPEATER_MGNT_TRIGGER               "Trigger"
#define PROPERTY_REPEATER_MGNT_STATUS                "Status"
#define CLASS_SASREDRIVER_MGNT                          "SASRedriverMgnt"
#define PROPERTY_SASREDRIVER_MGNT_SLOTID                "SlotId"
#define PROPERTY_SASREDRIVER_MGNT_REGSTARTOFFSET        "RegStartOffset"
#define PROPERTY_SASREDRIVER_MGNT_REGNUMBER             "RegNumber"
#define PROPERTY_SASREDRIVER_MGNT_REFCHIP               "RefChip"

#define CLASS_POWER_ON_CRITERIA                      "PowerOnCriteria"
#define PROPERTY_POWER_ON_CRITERIA_FRUID             "FruId"
#define PROPERTY_POWER_ON_CRITERIA_IS_MET            "IsMet"
#define PROPERTY_POWER_ON_CRITERIA_LOG_RECORD        "LogRecord"
#define PROPERTY_POWER_ON_CRITERIA_DESCRIPTION       "Description"
/* END : Added by d00381887, 2016/07/12,PN: AR-0000276585-009-002 */

/* BEGIN : Added by l00397738, 2017/09/04,PN: AR-0000276585-016-036 */
#define CLASS_CHIP_UPGRADE_I2C                           "ChipUpgradeByI2C"
#define PROPERTY_CHIP_UPGRADE_I2C_COM_OBJ                "ComponentObj"
#define PROPERTY_CHIP_UPGRADE_I2C_CHIP_OBJ               "ChipObj"
#define PROPERTY_CHIP_UPGRADE_I2C_VALID_COND             "ValidCondition"
#define PROPERTY_CHIP_UPGRADE_I2C_START_VALID_COND       "StartValidCondition"
#define PROPERTY_CHIP_UPGRADE_I2C_ACTION_UPGRADE         "ActionAfterUpgrade"
#define PROPERTY_CHIP_UPGRADE_I2C_ACTION_VALID           "ActionAfterValid"
/* END : Added by l00397738, 2017/09/04,PN: AR-0000276585-016-036 */

/* BEGIN: Modified by dingzhixiong (d00381887), AR.SR.SF-0000276585.001.013, 2017/10/20 */
#define CLASS_SMM_BAK_MGNT                               "SMMBakMgnt"
#define PROPERTY_SMM_BAK_MGNT_SLOT                       "Slot"
#define PROPERTY_SMM_BAK_MGNT_IS_ACTIVE                  "IsActive"
#define PROPERTY_SMM_BAK_MGNT_IS_ENABLED                 "IsEnabled"
/* END: Modified by dingzhixiong (d00381887), AR.SR.SF-0000276585.001.013, 2017/10/20 */

/* BEGIN: Added by zhoudeyou, 2017/11/10 问题单号: DTS2017102307765 10GE phy下电升级 */
#define CLASS_PHYUPGRADE_MGNT                       "PHYUpgradeMgnt"
#define PROPERTY_PHY_UPGRADE_MGNT_SMMSLOT           "SMMSlot"
#define PROPERTY_PHY_UPGRADE_MGNT_CONDITION         "Condition"
#define PROPERTY_PHY_UPGRADE_MGNT_STATUS            "Status"
/* END: Added by zhoudeyou, 2017/11/10 问题单号: DTS2017102307765 10GE phy下电升级 */
/* BEGIN: Modified by zhangfuhai 00382105, 2018/1/20   PN:DTS20170101 */
#define CLASS_COMPONENT_POWER_CTRL                      "ComponentPowerCtrl"
#define PROPERTY_COMPONENT_POWER_CTRL_CONDITION         "Condition"
#define PROPERTY_COMPONENT_POWER_CTRL_SLOT              "Slot"
#define PROPERTY_COMPONENT_POWER_CTRL_COMPTYPE          "CompType"
#define PROPERTY_COMPONENT_POWER_CTRL_POSITION          "Position"
#define PROPERTY_COMPONENT_POWER_CTRL_OPERATETYPE       "OperateType"
#define PROPERTY_COMPONENT_POWER_CTRL_REFCHIP           "RefChip"
#define PROPERTY_COMPONENT_POWER_CTRL_CONFIGTABLE       "ConfigTable"
/* END:   Modified by zhangfuhai 00382105, 2018/1/20 */

/* BEGIN: Added for 问题单号: by lwx459244, 2017/12/14 */
#define CLASS_VIRTUAL_OPTICAL_MODULE                 "VirOpticalModule"
/* END:   Added for 问题单号: by lwx459244, 2017/12/14 */

#define CLASS_POWER_MGNT                                           "PowerMgnt"
#define PROPERTY_POWER_MGNT_SLOT                                   "Slot"
#define PROPERTY_POWER_MGNT_PRESENCE                               "Presence"
#define PROPERTY_POWER_MGNT_HEALTH                                 "Health"
#define PROPERTY_POWER_MGNT_PS_HEALTH                              "PsHealth"
#define PROPERTY_POWER_MGNT_SWITCH_WP                              "SwitchWp"
#define PROPERTY_POWER_MGNT_MFR_STATUS                             "MfrStatus"
#define PROPERTY_POWER_MGNT_SWITCH                                 "Switch"
#define PROPERTY_POWER_MGNT_FAULT_REASON                           "FaultReason"
#define PROPERTY_POWER_MGNT_PS_INFO                                "PSInfo"
#define PROPERTY_POWER_MGNT_PS_FRU_EEPROM                          "PSFruEeprom"
#define PROPERTY_POWER_MGNT_CHANNEL_TYPE                           "ChannelType"
#define PROPERTY_POWER_MGNT_ACTION_MASK                            "ActionMask"
#define PROPERTY_POWER_MGNT_DELAY_TIME_INTERVAL                    "DelayTimeInterval"
#define METHOD_SET_PS_SWITCH                                       "SetPsSwitch"
#define METHOD_SET_PS_RESET                                        "SetPsReset"
#define METHOD_POWER_MGNT_PROPERTY_SYNC                            "PropertySyncMethod"

/* 电源休眠相关类 */
#define CLASS_POWER_SLEEP                                          "PsSleep"
#define PROPERTY_POWER_SLEEP_ENABLE                                "Enable"
#define ROPERTY_POWER_SLEEP_MODE                                   "Mode"
#define ROPERTY_POWER_SLEEP_SLEEP_STATUS_BITMAP                    "SleepStatusBitmap"
#define METHOD_SET_PS_SLEEP_CONFIG                                 "SetPsSleepConfig"
#define METHOD_PS_SLEEP_SYNC                                       "PsSleepSync"

/* 电源alert相关类 */
#define CLASS_POWER_ALERT                                          "PowerAlert"
#define PROPERTY_POWER_ALERT_ENABLE_ACCESSOR                       "AlertEnableAccessor"
#define PROPERTY_POWER_ALERT_ENABLE                                "AlertEnable"
#define PROPERTY_POWER_ALERT_STATE                                 "AlertState"
#define PROPERTY_POWER_ALERT_PSU_NUM_EXPECT                        "PsuNumExpect"
#define PROPERTY_POWER_ALERT_POWER_BUDGET                          "PowerBudget"
#define PROPERTY_POWER_ALERT_POWER_USED                            "PowerUsed"
#define PROPERTY_POWER_ALERT_SHELF_RUNTIME_POWER                   "ShelfRuntimePower"
#define METHOD_SET_POWER_ALERT_ENABLE                              "SetPowerAlertEnable"
#define METHOD_ALERT_RECOVERY                                      "AlertRecovery"
#define METHOD_POWER_ALERT_SYNC                                    "PowerAlertSync"
#define METHOD_EXPECT_MIN_PSU_NUM_SYNC                             "ExpectMinPsuNumSync"

/* BEGIN: Added by gwx455466, 2017/8/4 16:29:45   问题单号:DTS2017092902173 */
#define CLASS_OPTICAL_MODULE                 "OpticalModule"
#define PROPERTY_OPT_MDL_REF_PORT_OBJ                "RefPortObj"
#define PROPERTY_OPT_MDL_BMAID                "BMAId"
#define PROPERTY_OPT_MDL_PRESENT                "Present"
#define PROPERTY_OPT_MDL_POWER_STATUS           "PowerStatus"
#define PROPERTY_OPT_MDL_VENDOR_NAME                "VendorName"
#define PROPERTY_OPT_MDL_PART_NUMBER                "PartNumber"
#define PROPERTY_OPT_MDL_SERIAL_NUMBER                "SerialNumber"
#define PROPERTY_OPT_MDL_MANUFACTURE_DATE                "ManufactureDate"
#define PROPERTY_OPT_MDL_TRANSCEIVER_TYPE                "TransceiverType"
#define PROPERTY_OPT_MDL_WAVE_LENGTH                "Wavelength"
#define PROPERTY_OPT_MDL_MEDIUM_MODE                "MediumMode"
#define PROPERTY_OPT_MDL_TEMP_CURRENT                "TempCurrent"
#define PROPERTY_OPT_MDL_TEMP_HIGH_ALARM                "TempHighAlarm"
#define PROPERTY_OPT_MDL_TEMP_LOW_ALARM                "TempLowAlarm"
#define PROPERTY_OPT_MDL_TEMP_HIGH_WARN                "TempHighWarn"
#define PROPERTY_OPT_MDL_TEMP_LOW_WARN                "TempLowWarn"
#define PROPERTY_OPT_MDL_VCC_CURRENT                "VccCurrent"
#define PROPERTY_OPT_MDL_VCC_HIGH_ALARM                "VccHighAlarm"
#define PROPERTY_OPT_MDL_VCC_LOW_ALARM                "VccLowAlarm"
#define PROPERTY_OPT_MDL_VCC_HIGH_WARN                "VccHighWarn"
#define PROPERTY_OPT_MDL_VCC_LOW_WARN                "VccLowWarn"
#define PROPERTY_OPT_MDL_TX_BIAS_CURRENT                "TxBiasCurrent"
#define PROPERTY_OPT_MDL_TX_BIAS_HIGH_ALARM                "TxBiasHighAlarm"
#define PROPERTY_OPT_MDL_TX_BIAS_LOW_ALARM                "TxBiasLowAlarm"
#define PROPERTY_OPT_MDL_TX_BIAS_HIGH_WARN                "TxBiasHighWarn"
#define PROPERTY_OPT_MDL_TX_BIAS_LOW_WARN                "TxBiasLowWarn"
#define PROPERTY_OPT_MDL_TX_POWER_CURRENT                "TxPowerCurrent"
#define PROPERTY_OPT_MDL_TX_POWER_HIGH_ALARM                "TxPowerHighAlarm"
#define PROPERTY_OPT_MDL_TX_POWER_LOW_ALARM                "TxPowerLowAlarm"
#define PROPERTY_OPT_MDL_TX_POWER_HIGH_WARN                "TxPowerHighWarn"
#define PROPERTY_OPT_MDL_TX_POWER_LOW_WARN                "TxPowerLowWarn"
#define PROPERTY_OPT_MDL_RX_POWER_CURRENT                "RxPowerCurrent"
#define PROPERTY_OPT_MDL_RX_POWER_HIGH_ALARM                "RxPowerHighAlarm"
#define PROPERTY_OPT_MDL_RX_POWER_LOW_ALARM                "RxPowerLowAlarm"
#define PROPERTY_OPT_MDL_RX_POWER_HIGH_WARN                "RxPowerHighWarn"
#define PROPERTY_OPT_MDL_RX_POWER_LOW_WARN                "RxPowerLowWarn"
#define PROPERTY_OPT_MDL_TYPE                            "Type"
#define PROPERTY_OPT_MDL_SPEED                           "Speed"
#define PROPERTY_OPT_MDL_TYPE_MATCH                      "TypeMatch"
#define PROPERTY_OPT_MDL_SPEED_MATCH                    "SpeedMatch"
#define PROPERTY_OPT_MDL_HEALTH                         "Health"
#define PROPERTY_OPT_MDL_CHANNEL_NUM                    "ChannelNum"
#define PROPERTY_OPT_MDL_DIAG_STATUS                   "DiagStatus"
#define PROPERTY_OPT_MDL_DIAG_STATUS_PRE              "DiagStatusPre"
#define PROPERTY_OPT_MDL_DIAG_STATUS_MERGE            "DiagStatusMerge"
#define PROPERTY_OPT_MDL_INDENTIFIER                  "Indentifier"
#define PROPERTY_OPT_MDL_CONNECTOR_TYPE               "ConnectorType"
#define PROPERTY_OPT_MDL_DEVICE_TYPE                  "DeviceType"
#define PROPERTY_OPT_MDL_TRANSFER_DISTANCE            "TransferDistance"
#define PROPERTY_OPT_MDL_RX_LOS_STATE                 "RxLosState"
#define PROPERTY_OPT_MDL_TX_Fult_STATE                "TxFultState"
#define PROPERTY_OPT_MDL_LOW_SPEED_IOINFO              "LowSpeedIOInfo"
#define METHOD_OPT_MDL_SET_OPTICAL_MODULE_TEMP        "SetOpticalModuleTemp"
#define METHOD_OPT_MDL_UPDATE_OPTICAL_PROP_INFO       "UpdateOpticalPropInfo"  /* FPGA卡光模块信息查询 by lwx459244, 2018/9/28 */
#define METHOD_OPT_MDL_SET_POWER_STATUS               "SetPowerStatus"
#define METHOD_OPT_MDL_SET_SCAN_STATUS                "SetScanStatus"
#define METHOD_OPT_MDL_GET_SCAN_STATUS                "GetScanStatus"

/* 支持机框部件的故障汇聚节点BMC */
#define CLASS_DATA_SYNC_SERVER_CONFIG           "DataSyncServerConfig"
#define OBJ_NAME_DATA_SYNC_SERVER_CONFIG        "DataSyncServerConfig"
#define PROPERTY_DATA_SYNC_BINDED_ETH_GROUP     "BindedEthGroup"
#define PROPERTY_DATA_SYNC_LISTENED_PORT        "ListenedPort"
#define METHOD_TEST_CHANNEL_STATUS              "TestChannelStatus"
#define METHOD_GET_OBJ_NAME_LIST                "GetObjNameList"
#define METHOD_GET_SPECIAL_OBJECT_BYTE          "GetSpecialObjectByte"

#define CLASS_REMOTE_RPC_INFO                   "RemoteRpcInfo"
#define PROPERTY_REMOTE_RPC_INFO_PRESENCE       "Presence"
#define PROPERTY_REMOTE_RPC_INFO_INDEX          "Index"
#define PROPERTY_REMOTE_RPC_INFO_TARGET_IP      "TargetIP"
#define PROPERTY_REMOTE_RPC_INFO_TARGET_PORT    "TargetPort"
#define PROPERTY_REMOTE_RPC_INFO_TARGET_TYPE    "TargetType"
#define PROPERTY_REMOTE_RPC_INFO_LOCAL_IP       "LocalIP"
#define PROPERTY_REMOTE_RPC_INFO_STATUS         "Status"
/* END:   Added by d00381887, 2018/2/27 */
/** BEGIN: Modified by qinjiaxiang, 2018/3/10 问题单号:#### 修改原因:*/
#define CLASS_NAND_FLASH                                     "NandFlash"
#define PROPERTY_NAND_FLASH_VID                              "VID"
#define PROPERTY_NAND_FLASH_WP                               "WriteProtection"
#define PROPERTY_NAND_FLASH_LIFE_TIME                        "LifeTime"
#define PROPERTY_NAND_FLASH_PRE_EOL                          "PreEOL"
#define PROPERTY_NAND_FLASH_RESERVED_BLOCK                   "ReservedBlock"
#define PROPERTY_NAND_FLASH_RESERVED_BLOCK_CNT               "ReservedBlockCnt"

#define PROPERTY_NAND_FLASH_LIFE_TIME_THRESHOLD              "LifeThres"
#define PROPERTY_NAND_FLASH_RESERVED_BLOCK_THRESHOLD         "BlockThres"
#define PROPERTY_NAND_FLASH_RUNTIME_BAD_BLOCK                "RunTimeBadBlock"
#define PROPERTY_NAND_FLASH_DFT_LIFE_TIME_THRESHOLD          "DftLifeThres"
#define PROPERTY_NAND_FLASH_DFT_RESERVED_BLOCK_THRESHOLD     "DftBlockThres"
#define PROPERTY_NAND_FLASH_OVER_WRITE                       "OverWrite"
#define PROPERTY_NAND_FLASH_WRITE_THRES                      "WriteThres"

#define PROPERTY_NAND_FLASH_REPAIRED                         "Repaired"
#define PROPERTY_NAND_FLASH_RW_STATE                         "RWState"
#define PROPERTY_NAND_FLASH_CSD_REG_INFO                     "CsdRegInfo"
#define PROPERTY_NAND_FLASH_CSD_EXT_REG_INFO                 "CsdExtRegInfo"

#define METHOD_GET_NAND_FLASH_VID                            "GetVid"
#define METHOD_SET_NAND_FLASH_WP                             "SetWriteProtection"
#define METHOD_CLEAR_FLASH_STATISTICS                        "ClearFlashStatistics"

#define CLASS_NAND_FLASH_INFO                                     "NandInfo"
#define PROPERTY_NAND_FLASH_INFO_VID                              "VID"
#define PROPERTY_NAND_FLASH_INFO_LIFE_TIME_THRESHOLD              "LifeThres"
#define PROPERTY_NAND_FLASH_INFO_RESERVED_BLOCK_THRESHOLD         "BlockThres"
#define PROPERTY_NAND_FLASH_INFO_DFT_LIFE_TIME_THRESHOLD          "DftLifeThres"
#define PROPERTY_NAND_FLASH_INFO_DFT_RESERVED_BLOCK_THRESHOLD     "DftBlockThres"
/** END:   Added by qinjiaxiang, 2018/3/10 */

/* BEGIN: Added by gwx455466, 2017/8/26 11:0:22   问题单号:SREA02109352-001-003 */

#define CLASS_OAM                 "OAM"
#define PROPERTY_OAM_REF_PORT_OBJ                "RefPortObj"
#define PROPERTY_OAM_BMAID                "BMAId"

#define PROPERTY_OAM_IS_OAM_ENABLED  "IsOAMEnabled"
#define PROPERTY_OAM_LOST_LINK_STATE  "OAMLostLinkState"
#define PROPERTY_OAM_ErrPkt_COUNT      "OAMErrPktCnt"
/* END:   Added by gwx455466, 2017/8/26 11:0:26 */

/* BEGIN: Added by gwx455466, 2018/4/20 11:15:12   问题单号:SR.SFEA02130837.001.010 */
#define CLASS_USB_PORT_SWITCH                 "USBPortSwitch"
#define CLASS_USB_PORT_SWITCH_DESTINATION                "Destination"
#define CLASS_USB_PORT_SWITCH_STATE               "State"

/* END:   Added by gwx455466, 2018/4/20 11:15:14 */

/* Begin by huangjiaqing 2018/04/30 RM 201801235759 */
#define CLASS_SENSOR_SHORTTERM_MONITOR   "SensorShortTermSpecMonitor"
#define PROPERTY_SENSOR_SHORTTERM_MONITOR_SENSOR_VALUE  "SensorValue"
#define PROPERTY_SENSOR_SHORTTERM_MONITOR_MASK  "Mask"
#define PROPERTY_SENSOR_SHORTTERM_MONITOR_THRESHOLD  "Threshold"
#define PROPERTY_SENSOR_SHORTTERM_MONITOR_SCAN_CONDITION  "ScanConditon"
#define PROPERTY_SENSOR_SHORTTERM_MONITOR_OPERATOR   "Operator"
#define PROPERTY_SENSOR_SHORTTERM_MONITOR_CONTINUE_TIME       "ContinueTime"
#define PROPERTY_SENSOR_SHORTTERM_MONITOR_CONTINUE_RESULT     "ContinueResult"
#define PROPERTY_SENSOR_SHORTTERM_MONITOR_TOTAL_TIME          "TotalTime"
#define PROPERTY_SENSOR_SHORTTERM_MONITOR_TOTAL_PERIOD_TIME   "ToalPeriodTime"
#define PROPERTY_SENSOR_SHORTTERM_MONITOR_TOTAL_RESULT        "TotalResult"
#define PROPERTY_SENSOR_SHORTTERM_MONITOR_TOTAL_REFREASH_PERIOD         "TotalCountFreshPeriod"
/* End by huangjiaqing 2018/04/30 RM 201801235759 */

#define MSM_NODE_CLASS_NAME                 "MsmNode"
#define PROPERTY_MSM_SLOT_ID                "SlotId"
#define PROPERTY_MSM_PRESENT_STATUS         "PresentStatus"
#define PROPERTY_MSM_HEARTBEAT              "HeartBeat"
#define PROPERTY_MSM_MASTER_ENABLE          "MasterEnable"
#define PROPERTY_MSM_MASTER_SLOT_ID         "MasterSlotId"
#define PROPERTY_MSM_LOCAL_MS_STATUS        "LocalMsStatus"
#define PROPERTY_MSM_ALIVE_STATUS           "AliveStatus"
#define PROPERTY_MSM_IIC_ENABLE             "IicEnable"

/* 主备类相关 */
#define ASM_CLASS_NAME                      "ASM"
#define ASM_OBJECT_NAME                     "AsmObject"
#define PROPERTY_ASM_AS_STATUS              "AsStatus"
#define PROPERTY_ASM_SLOT_ID                "AsSlotId"
#define PROPERTY_ASM_REMOTE_SLOT_ID         "RemoteSlotId"
#define PROPERTY_DEFAULT_ACTIVE_SMM         "DefaultActiveSlot"
#define PROPERTY_ASM_ETHGROUP               "BindEthGroup"
#define PROPERTY_AS_STATUS_TIME_INTERVAL    "AsStatusProcTimeInterval"
#define PROPERTY_HEART_BEAT_TIME_INTERVAL   "SoftHeartBeatTimeInterval"
#define PROPERTY_SOFT_HEARTBEAT_COUNTDOWN   "SoftHeartBeatCountDown"
#define PROPERTY_MSM_SOFT_MASTER_SLOT_ID    "MasterSlotId"
#define PROPERTY_UPDATE_MS_ALIVE_STATUS     "UpdateMsAliveStatus"
#define PROPERTY_MSM_NODES_OBJ_LIST         "MsmNodes"
#define PROPERTY_ASM_LAST_AS_STATUS_CHANGE_TIME "LastAsStatusChangeTime"

#define FAILOVER_RESTRICTION_CLASS_NAME     "FailoverRestriction"
#define PROPERTY_FAILOVER_ID                "ID"
#define PROPERTY_FAILOVER_CHECK_LOCATION    "CheckLocation"
#define PROPERTY_FAILOVER_CHECK_TYPE        "CheckType"
#define PROPERTY_FAILOVER_OBJECT_NAME       "ObjectName"
#define PROPERTY_FAILOVER_PROPERTY_VALUE    "PropertyValue"
#define PROPERTY_FAILOVER_METHOD_NAME       "MethodName"
#define PROPERTY_FAILOVER_EXPECT_VALUE      "ExpectValue"
#define PROPERTY_FAILOVER_ERROR_MSG         "ErrorMsg"

/* 只读寄存器 */
#define PROPERTY_LOCAL_AS_STATUS            "AsCpldLocalAsStatus"
#define PROPERTY_LOCAL_PRESENT_STATUS       "AsCpldLocalPresentStatus"
#define PROPERTY_LOCAL_HEALTH_STATUS        "AsCpldLocalHealthStatus"
#define PROPERTY_REMOTE_AS_STATUS           "AsCpldRemoteAsStatus"
#define PROPERTY_REMOTE_PRESENT_STATUS      "AsCpldRemotePresentStatus"
#define PROPERTY_REMOTE_HEALTH_STATUS       "AsCpldRemoteHealthStatus"
#define PROPERTY_BOARD_IN                   "AsCpldBoardIn"
#define PROPERTY_CHECK_HEART_BEAT           "AsCpldCheckHeartBeat"
#define PROPERTY_BOTH_ACTIVE                "AsCpldBothActive"

/* 读写寄存器 */
#define PROPERTY_WRITE_HEARTBEAT            "AsCpldWriteHeartBeat"
#define PROPERTY_ACTIVE_TO_STANDBY          "AsCpldActiveToStandby"
#define PROPERTY_STANDBY_TO_ACTIVE          "AsCpldStandbyToActive"
#define PROPERTY_MASTER_WRITE_PRO           "AsCpldMasterBeWritePro"
#define PROPERTY_STANDBY_WRITE_PRO          "AsCpldStandbyBeWritePro"
#define PROPERTY_CLR_BOTH_ACTIVE            "AsCpldClrBothActive"
#define PROPERTY_BOTH_ACTIVE                "AsCpldBothActive"
#define PROPERTY_REMOTE_HEALTH_ALARM_FLAG   "AsRemoteBoardHealthStatus"
#define PROPERTY_FAILOVER_ALARM             "AsFailoverAlarm"
#define PROPERTY_DATA_SYNC_STATUS           "AsDataSyncStatus"
#define METHOD_ASM_FAILOVER                       "AsmFailover"
#define METHOD_ASM_STANDBY_TO_MASTER              "AsmSlaveToMater"
#define METHOD_ASM_CHECK_LOCAL_FAILOVER_CONDITION "AsmCheckLocalFailoverCondition"
#define METHOD_ASM_CHANGE_STATUS                  "AsmChangeStatus"


/* 本地kvm类相关 */
#define LOCAL_KVM_CLASS_NAME                      "LocalKVM"
#define LOCAL_KVM_OBJECT_NAME                     "LocalKVMObject"
/* 只读寄存器 */
#define PROPERTY_LOCAL_KVM_FPGA_DONE              "FpgaDoneStatus"
/* 读写寄存器 */
#define PROPERTY_LOCAL_KVM_FPGA_RESET                "FpgaReset"
#define PROPERTY_LOCAL_KVM_FPGA_PROGRAM         "FpgaProgram"
#define PROPERTY_LOCAL_KVM_FPGA_SOL              "FpgaSol"
/* SOL:修改CPLD，使能/去使能备板SOL串口通道 */
#define METHOD_SET_FPGA_SOL                       "SetFpgaSol"

/* 同步模块管理类相关 */
#define METHOD_GET_SECURITY_INFO                    "GetSecurityInfoMethod"
#define METHOD_PROPERTY_SYNC_METHOD                 "PropertySyncMethod"
#define DATA_SYNC2_MANAGE_CLASS_NAME                "DataSyncManagement"
#define DATA_SYNC2_MANAGE_OBJECT_NAME               "DataSyncManagementObject"
#define PROPERTY_DATA_SYNC2_SLOTID                  "SlotId"
#define PROPERTY_DATA_SYNC2_BIND_ETHGROUP           "BindEthGroup"
#define PROPERTY_DATA_SYNC2_BIND_ETH_PORT_STATE     "BindEthPortState"
#define PROPERTY_DATA_SYNC2_SERVER_PORT             "ServerPort"
#define PROPERTY_DATA_SYNC2_FILESYNC_SERVER_PORT    "FileSyncServerPort"
#define PROPERTY_DATA_SYNC2_FILESYNC_RUNNING_MODE   "FileSyncRunningMode"
#define PROPERTY_DATA_SYNC2_PROPSYNC_RUNNING_MODE   "PropertySyncRunningMode"
#define PROPERTY_DATA_SYNC2_FILESYNC_TIME_INTERVAL  "FilePeriodSyncTimeInterval"
#define PROPERTY_DATA_SYNC2_PROPSYNC_TIME_INTERVAL  "PropertyPeriodSyncTimeInterval"
#define PROPERTY_DATA_SYNC2_PROPERTY_SYNC_STATE     "PropertySyncState"
#define PROPERTY_DATA_SYNC2_FILE_SYNC_STATE         "FileSyncState"
#define MEHTOD_GET_SYNC_STATE                       "GetSyncStateMethod"
#define METHOD_SYNC_DESTINATION_COMMUNICATION_UPDATE     "CommunicationUpdateMethod"
#define METHOD_INNER_TRANSFER_FILE                       "StartInnerTransfer"
#define METHOD_GET_INNER_TRANSFER_STATUS                 "GetInnerTransferStatus"

/* 同步目标类相关 */
#define SYNC_DESTINATION_CLASS_NAME                      "SyncDestination"
#define SYNC_DESTINATION_OBJECT_NAME                     "SyncDestinationRemoteSMM"
#define PROPERTY_SYNC_DESTINATION_SLOTID                 "SlotId"
#define PROPERTY_SYNC_DESTINATION_PRESENCE               "Presence"
#define PROPERTY_SYNC_DESTINATION_COMMUNICATION_STATUS   "CommunicationStatus"
#define PROPERTY_SYNC_DESTINATION_ASSERT_FLAG            "AssertFlag"
#define PROPERTY_SYNC_DESTINATION_LINK_LOSS_ASSERTION_DELAY     "LinkLossAssertionDelay"

/* 属性同步类相关 */
#define PROPERTY_SYNC_CLASS_NAME                    "PropertySync"
#define PROPERTY_PROPERTY_SYNC_SYNC_DESTINATION     "SyncDestination"
#define PROPERTY_PROPERTY_SYNC_SYNC_MODE            "SyncMode"
#define PROPERTY_PROPERTY_SYNC_CLASS_NAME           "ClassName"
#define PROPERTY_PROPERTY_SYNC_OBJECT_NAME          "ObjectName"
#define PROPERTY_PROPERTY_SYNC_PROPERTY_NAME        "PropertyName"
#define PROPERTY_PROPERTY_SYNC_METHOD_NAME          "MethodName"

/* 基于ID的属性同步类定义 */
#define PROPERTY_ID_SYNC_CLASS_NAME                    "PropertyIdSync"
#define PROPERTY_PROPERTY_ID_SYNC_SYNC_DESTINATION     "SyncDestination"
#define PROPERTY_PROPERTY_ID_SYNC_SYNC_MODE            "SyncMode"
#define PROPERTY_PROPERTY_ID_SYNC_SYNC_TYPE            "SyncType"
#define PROPERTY_PROPERTY_ID_SYNC_CLASS_NAME           "ClassName"
#define PROPERTY_PROPERTY_ID_SYNC_OBJECT_NAME          "ObjectName"
#define PROPERTY_PROPERTY_ID_SYNC_OBJECT_SYNC_ID       "ObjectSyncId"
#define PROPERTY_PROPERTY_ID_SYNC_PROPERTY_NAME        "PropertyName"
#define PROPERTY_PROPERTY_ID_SYNC_PROPERTY_TYPE        "PropertyType"
#define PROPERTY_PROPERTY_ID_SYNC_METHOD_NAME          "MethodName"
#define METHOD_PROPERTY_ID_SYNC_SEND_COMMAND_METHOD    "SendCommand"
#define METHOD_IDSYNC_SFM_BROADCAST_MSG                "SfmBroadcastMsg"    /* 共享器件检测使用的广播消息方法 */

/*文件同步类*/
#define FILE_SYNC_CLASS_NAME                        "FileSync"
#define PROPERTY_FILE_SYNC_SYNC_DESTINATION         "SyncDestination"
#define PROPERTY_FILE_SYNC_FILE_LIST                "FileList"
#define PROPERTY_FILE_SYNC_SYNC_MODE                "SyncMode"
#define PROPERTY_FILE_SYNC_SYNC_TYPE                "SyncType"
#define PROPERTY_FILE_SYNC_TEMP_DIR                 "SyncTempDir"
#define PROPERTY_FILE_SYNC_CHECK_DIR                "SyncCheckTempDir"
#define PROPERTY_FILE_SYNC_GROUP_SYNC_OBJECT_NAME   "GroupSyncCallBackObjectName"
#define PROPERTY_FILE_SYNC_GROUP_SYNC_METHOD_NAME   "GroupSyncCallBackMethodName"

/*Smm Sol*/
#define MODULE_NAME_SMM_SOL                 "SmmSol"
#define CLASS_NAME_SMM_SOL                  "SmmSol"
#define OBJECT_NAME_SMM_SOL                 "SmmSolObj"
#define PROPERTY_SMM_SOL_ETH_GROUP          "BindEthGroup"
#define PROPERTY_SMM_SOL_TIMEOUT            "Timeout"
#define METHOD_SMM_SOL_SET_TIMEOUT          "SetSolTimeout"
#define METHOD_SMM_SOL_GET_CONNECTION_INFO  "GetConnectionInfo"
#define METHOD_SMM_SOL_SYNC_METHOD          "SyncMethod"

/* 管理板底板 BoardId h00478734 对应(4U的SMM板CN90SMMA0 PRODUCT_601_01.xml or 8U的SMM板CN90SMMA1 PRODUCT_601_02.xml) */
#define OBJECT_NAME_PRODUCT_CONNECT          "ProductConnector"
#define PROPERTY_PRODUCT_CONNECT_AUXID       "AuxId"

/*Back plane l00422028*/
#define CLASS_NAME_RAW_EEPROM_DEV               "RawEepromFruDev"
#define OBJECT_NAME_BACK_PLANE_CONNECT          "BackPlaneConnect"
#define PROPERTY_AUX_ID                         "AuxId"
#define PROPERTY_ID                             "Id"
#define PROPERTY_PCB_ID                         "PcbId"
#define OBJECT_NAME_BACK_PLANE                  "BackPlane"
#define OBJECT_NAME_BACK_PLANE_TYPE_ACCESSOR    "BackPlaneTypeAccessor"
#define OBJECT_NAME_BACK_PLANE_PCB_VER_ACCESSOR "BackPlanePCBVerAccessor"
#define PROPERTY_RAW_EEPROM_ACCESSOR            "RawEepromAccessor"
#define PROPERTY_RAW_EEPROM_PAGES               "RawEepromPages"
#define PROPERTY_RAW_EEPROM_PAGE_SIZE           "PageSize"
#define PROPERTY_RAW_EEPROM_BLOCK_SIZE          "BlockSize"
/*l00422028*/

/* 高速背板定义,识别装备背板类型: Component类 */
#define OBJECT_COM_BACKPLANE_DEVICE_NAME        "HighSpeedBackPlane"
#define OBJECT_BASEBOARD_CONNECT                "BaseboardConnector"

/* start: cooling_smm */
#define CLASS_NAME_FAN_REDUNDANCY "SMMFanGroup"
#define PROPERTY_FAN_REDUNDANCY_ID "ID"
#define PROPERTY_FAN_REDUNDANCY_MIN_FAN "MinFanNum"
#define PROPERTY_FAN_REDUNDANCY_MAX_FAN "MaxFanNum"
#define PROPERTY_FAN_REDUNDANCY_FAN_SLOT "FanSlot"
#define PROPERTY_FAN_REDUNDANCY_DEV_SLOT "DevAddr"
#define PROPERTY_FAN_REDUNDANCY_SPEED    "Speed"
#define PROPERTY_FAN_REDUNDANCY_HEALTH   "Health"
#define PROPERTY_FAN_REDUNDANCY_STATUS   "Status"
#define PROPERTY_FAN_REDUNDANCY_MODE     "Mode"
#define PROPERTY_FAN_REDUNDANCY_MINIMALLEVEL "MinimalLevel"

#define CLASS_NAME_SMM_FAN "SMMFANClass"
#define PROPERTY_FAN_SLOT "FANSlot"
#define PROPERTY_FAN_IO "IO"
#define PROPERTY_FAN_FRU_EEPROM "FruEeprom"
#define PROPERTY_FAN_PRESENT "Presence"
#define PROPERTY_FAN_SPEED "Speed"
#define PROPERTY_FAN_PCB_VERSION "PCBVersion"
#define PROPERTY_FAN_SOFT_VERSION "SoftVersion"
#define PROPERTY_FAN_FANGROUP "FanGroup"
#define PROPERTY_FAN_MINIMALLEVEL "MinimalLevel"

#define CLASS_NAME_COOLING_APP COOLINGCLASS
#define PROPERTY_COOLING_APP_DEFAULT_FAN_SPEED "MinimalLevel"
#define PROPERTY_COOLING_APP_DEFAULT_FAN_CTL_MODE "Mode"
/* end: cooling_smm */

/*MM920光模块信息*/
#define CLASS_NAME_MM_PORT_OTM_INFO                     "MMPortOtmInfo"
#define OBJECT_NAME_MANAGEMENT_PORT_ADAPTER             "MMPortAdapter"
#define OBJECT_NAME_MM_OPT_MODULE                       "MMOpticalModule"
#define PROPERTY_MM_OTM_PORT_DISABLE                    "PortDisable"
#define METHOD_SET_MM_OTM_PORT_ENABLE                   "SetPortEnableState"

#define CLASS_NAME_CLOCK_TEST                           "ClockTest"
#define OBJECT_NAME_CLOCK_SELF_TEST                     "DftClockTest"
#define PROPERTY_CLK_TEST_OBJECT                        "RefObject"

#define CLASS_NAME_AGE_TEST                                "AgeTest"
#define PROPERTY_AGE_TEST_FLAG                             "AgeTestFlag"
#define PROPERTY_AGE_TEST_MANUAL_CLEAR                     "AgeTestFlagManualClear"
#define PROPERTY_AGE_TEST_START                            "AgeTestStart"
#define PROPERTY_AGE_TEST_FINISHED                         "AgeTestFinished"
#define PROPERTY_AGE_TEST_RESULT                           "AgeTestResult"
#define PROPERTY_AGE_TEST_FAILED_REASON                    "AgeTestFailedReason"
#define PROPERTY_AGE_TEST_FAILED_POSITION1                 "AgeTestFailedPosition1"
#define PROPERTY_AGE_TEST_FAILED_POSITION2                 "AgeTestFailedPosition2"
#define PROPERTY_AGE_TEST_FAILED_POSITION3                 "AgeTestFailedPosition3"
#define PROPERTY_AGE_TEST_FAILED_POSITION4                 "AgeTestFailedPosition4"
#define PROPERTY_AGE_TEST_FAILED_POSITION5                 "AgeTestFailedPosition5"
#define PROPERTY_AGE_TEST_FAILED_POSITION6                 "AgeTestFailedPosition6"
#define PROPERTY_MEMORY_AGE_TEST_PROCESS                   "MemoryAgeTestProcess"
#define PROPERTY_HARDDISK_AGE_TEST_PROCESS                 "HarddiskAgeTestProcess"
#define PROPERTY_AGE_TEST_VDDQ_CONTROL                     "VDDQControl"

/*BEGIN: Added by cwx579007 DTS2018082507394 20180825 */
#define CLASS_NAME_VMD_CONDICTION                          "VMDCondition"
#define PROPERTY_VMD_SUPPORT                               "VMDSupport"
/*END: Added by cwx579007 DTS2018082507394 20180825 */

#define CLASS_NAME_LICENSE_MANAGE               "LicenseManage"
#define PROPERTY_LM_LIC_CAPABILITY              "LicenseCapability"
#define PROPERTY_LM_LIC_SOURCE_TYPE             "LicenseSourceType"
#define PROPERTY_LM_LIC_SOURCE_ADDR             "LicenseSourceAddr"
#define PROPERTY_LM_INSTALL_STATUS              "InstallStatus"
#define PROPERTY_LM_INSTALL_STATUS_BAK          "InstallStatusBak"
#define PROPERTY_LM_INSTALL_PROGRESS            "InstallProgress"
#define PROPERTY_LM_INSTALL_RESULT              "InstallResult"
#define PROPERTY_LM_LIC_CLASS                   "LicenseClass"
#define PROPERTY_LM_PRODUCT_NAME                "ProductName"
#define PROPERTY_LM_PRODUCT_VERSION             "ProductVersion"
#define PROPERTY_LM_PRODUCT_ESN                 "ProductESN"
#define PROPERTY_LM_LIC_STATUS                  "LicenseStatus"
#define PROPERTY_LM_REVOKE_TICKET               "RevokeTicket"
#define METHOD_LM_SET_SOURCE_TYPE               "SetLicenseSourceType"
#define METHOD_LM_SET_SOURCE_ADDR               "SetLicenseSourceAddr"
#define METHOD_LM_INSTALL_LIC                   "InstallLicense"
#define METHOD_LM_EXPORT_LIC                    "ExportLicense"
#define METHOD_LM_DELETE_LIC                    "DeleteLicense"
#define METHOD_LM_REVOKE_LIC                    "RevokeLicense"

#define CLASS_NAME_ARM_SERVICE_CONFIG           "ARMServiceConfig"
#define METHOD_GET_KUNPENG_FEATURE_KEY          "GetKunpengFeatureKey"

#define CLASS_NAME_LICENSE_GENERAL_INFO         "LicenseGeneralInfo"
#define PROPERTY_LIC_FORMAT_VERSION             "LicenseFormatVersion"
#define PROPERTY_LIC_COPY_RIGHT                 "LicenseCopyRight"
#define PROPERTY_LIC_SN                         "LicenseSN"
#define PROPERTY_LIC_TYPE                       "LicenseType"
#define PROPERTY_LIC_GRACE_DAY                  "LicenseGraceDay"
#define PROPERTY_LIC_CREATOR                    "LicenseCreator"
#define PROPERTY_LIC_ISSUER                     "LicenseIssuer"
#define PROPERTY_LIC_CREATE_TIME                "LicenseCreateTime"
#define PROPERTY_LIC_CUSTOMER                   "LicenseCustomer"
#define PROPERTY_LIC_COUNTRY                    "LicenseCountry"
#define PROPERTY_LIC_OFFICE                     "LicenseOffice"
#define PROPERTY_LIC_NODE_NAME                  "NodeName"
#define PROPERTY_LIC_NODE_ESN                   "NodeESN"

#define CLASS_NAME_LICENSE_SALE_INFO            "LicenseSaleInfo"
#define PROPERTY_LIC_SALE_PRODUCT_NAME          "ProductName"
#define PROPERTY_LIC_SALE_PRODUCT_VERSION       "ProductVersion"
#define PROPERTY_LIC_SALE_ITEM                  "SaleItem"

#define CLASS_NAME_LICENSE_SALE_ITEM            "LicenseSaleItem"
#define PROPERTY_LIC_SALE_ITEM_NAME             "SaleName"
#define PROPERTY_LIC_SALE_ITEM_VALUE            "SaleValue"
#define PROPERTY_LIC_SALE_ITEM_VALID_DATE       "ValidDate"
#define PROPERTY_LIC_SALE_ITEM_DESC             "SaleDesc"
#define PROPERTY_LIC_SALE_ITEM_IS_PERMANENT     "IsPermanent"
#define PROPERTY_LIC_SALE_ITEM_IS_NOCONTROL     "IsNoControl"
#define PROPERTY_LIC_SALE_ITEM_FK               "FeatureKey"

#define CLASS_NAME_LICENSE_KEY_INFO             "LicenseKeyInfo"
#define PROPERTY_LIC_KEY_PRODUCT_NAME           "ProductName"
#define PROPERTY_LIC_KEY_PRODUCT_VERSION        "ProductVersion"
#define PROPERTY_LIC_KEY_FK                     "FeatureKey"

#define CLASS_NAME_LICENSE_FEATURE_KEY              "LicenseFeatureKey"
#define PROPERTY_LIC_FK_KEY_NAME                    "KeyName"
#define PROPERTY_LIC_FK_GROUP_NAME                  "GroupName"
#define PROPERTY_LIC_FK_FEATURE_NAME                "FeatureName"
#define PROPERTY_LIC_FK_KEY_VALUE                   "KeyValue"
#define PROPERTY_LIC_FK_IS_NOCONTROL                "IsNoControl"
#define PROPERTY_LIC_FK_VALID_DATE                  "ValidDate"
#define PROPERTY_LIC_FK_CURRENT_STATE               "CurrentState"
#define PROPERTY_LIC_FK_CONTROL_VALUE               "ControlValue"
#define PROPERTY_LIC_FK_REMOTE_CONTROL_VALUE        "RemoteControlValue"


#define CLASS_NAME_LICENSE_ALARM_INFO                   "LicenseAlarmInfo"
#define PROPERTY_LIC_ALARM_REMAIN_GRACEDAYS             "RemainGraceDays"
#define PROPERTY_LIC_ALARM_REMAIN_COMMISSIONINGDAYS     "RemainCommissioningDays"
#define PROPERTY_LIC_ALARM_PRODUCT_ESN_INVALID          "ProductESNInvalid"
#define PROPERTY_LIC_ALARM_FILE_ERROR                   "FileError"
#define PROPERTY_LIC_ALARM_ESN_NOT_MATCH                "ProductESNNotMatch"
#define PROPERTY_LIC_ALARM_VER_NOT_MATCH                "ProductVerNotMatch"

/* Modbus总线类 */
#define CLASS_BUS_MODBUS                        "Modbus"
#define PROPERTY_MODBUS_ID                      "Id"
#define PROPERTY_MODBUS_UART                    "UART"
#define PROPERTY_MODBUS_PORT                    "Port"
#define PROPERTY_MODBUS_TYPE                    "Type"
#define PROPERTY_MODBUS_MODE                    "Mode"
#define PROPERTY_MODBUS_SPEED                   "Speed"
#define PROPERTY_MODBUS_PARITY                  "Parity"
#define PROPERTY_MODBUS_SILK_NAME               "SilkName"
#define PROPERTY_MODBUS_PWR_OUT_ENABLE          "PowerOutputEnable"
#define PROPERTY_MODBUS_CURR_LIMIT_STATUS       "CurrentLimitStatus"

#define METHOD_MODBUS_PWR_OUT_SWITCH            "ModbusPwrOutSwitch"
#define METHOD_MODBUS_READ_INPUT_REG            "ModbusReadInputReg"
#define METHOD_MODBUS_SEND_CMD                  "ModbusSendCmd"


/* Modbus Slave类 */
#define CLASS_MODBUS_SLAVE                      "ModbusSlave"
#define PROPERTY_MB_SLAVE_ID                    "SlaveId"
#define PROPERTY_MB_SLAVE_ADDR                  "Addr"
#define PROPERTY_MB_SLAVE_LBUS                  "LBus"
#define PROPERTY_MB_SLAVE_HEALTH                "Health"

/* Modbus从机地址分配管理配置类 */
#define CLASS_MB_ADDR_ALLOC_MGMT                "ModbusAddrAllocMgmt"
#define PROP_MB_ADDR_ALLOC_UART_CHAN            "Channel"
#define PROP_MB_ADDR_ALLOC_DEFAULT_ADDR         "ExpDefAddr"
#define PROP_MB_ADDR_ALLOC_START_ADDR           "SlaveAddrStart"
#define PROP_MB_ADDR_ALLOC_MAX_SLAVE_NUM        "MaxSlaveNumber"
#define PROP_MB_ADDR_ALLOC_STATUS               "Status"
/* 继承子类，华为自研的软硬件结合的地址分配方案 */
#define CLASS_HW_MB_ADDR_ALLOC_MGMT             "HwMBAddrAllocMgmt"
#define PROP_HW_MB_ADDR_ALLOC_MGMT_PRST         "Present"
#define PROP_HW_MB_ADDR_ALLOC_MGMT_PWR1         "Power1"
#define PROP_HW_MB_ADDR_ALLOC_MGMT_PWR_ALL      "PowerAll"

/* 全局的柜级资产管理配置；类 */
#define CLASS_RACK_ASSET_MGMT                   "RackAssetMgmt"
#define PROPERTY_RACK_ASSET_MGMT_TYPE           "MgmtType"
#define PROPERTY_RACK_ASSET_MGMT_MODBUS_ID      "ModbusId"
#define PROPERTY_RACK_ASSET_MGMT_DIRECTION      "Direction"
#define PROPERTY_RACK_ASSET_MGMT_DIRECTION_TYPE "DirectionType"
#define PROPERTY_RACK_ASSET_MGMT_U_COUNT        "UCount"
#define PROPERTY_RACK_ASSET_MGMT_U_COUNT_USED   "UCountUsed"
#define PROPERTY_RACK_ASSET_MGMT_LOAD_CAPACITY  "LoadCapacityKg"
#define PROPERTY_RACK_ASSET_MGMT_RATED_POWER    "RatedPowerWatts"
#define PROPERTY_RACK_ASSET_MGMT_DEPTH_MM       "DepthMm"
#define PROPERTY_RACK_ASSET_MGMT_HEIGHT_MM      "HeightMm"
#define PROPERTY_RACK_ASSET_MGMT_WIDTH_MM       "WidthMm"
#define PROPERTY_RACK_ASSET_MGMT_COUNTRY        "Country"
#define PROPERTY_RACK_ASSET_MGMT_TERRITORY      "Territory"
#define PROPERTY_RACK_ASSET_MGMT_CITY           "City"
#define PROPERTY_RACK_ASSET_MGMT_STREET         "Street"
#define PROPERTY_RACK_ASSET_MGMT_HOUSE_NUM      "HouseNumber"
#define PROPERTY_RACK_ASSET_MGMT_NAME           "Name"
#define PROPERTY_RACK_ASSET_MGMT_POST_CODE      "PostCode"
#define PROPERTY_RACK_ASSET_MGMT_BUILDING       "Building"
#define PROPERTY_RACK_ASSET_MGMT_FLOOR          "Floor"
#define PROPERTY_RACK_ASSET_MGMT_ROOM           "Room"
#define PROPERTY_RACK_ASSET_MGMT_ROW            "Row"
#define PROPERTY_RACK_ASSET_MGMT_RACK           "Rack"
#define PROPERTY_RACK_ASSET_MGMT_LOCATION_INFO           "LocationInfo"
#define PROPERTY_RACK_ASSET_MGMT_NETWORK_MGMT_ETH  "NetworkMgmtEth"
#define PROPERTY_RACK_ASSET_MGMT_NETWORK_MGMT_IP_BASE "NetworkMgmtIpBase"
#define PROPERTY_RACK_ASSET_MGMT_UNIT_OCCUPY_DIRECTION  "UnitOccupyDirection"
#define PROPERTY_RACK_ASSET_MGMT_RACK_MODEL "RackModel"
#define PROPERTY_RACK_ASSET_MGMT_TYPICAL_CONFIGURATION_SN "TypicalConfigurationSN"
#define PROPERTY_RACK_ASSET_MGMT_EMPTY_RACK_SN "EmptyRackSN"
#define PROPERTY_RACK_ASSET_MGMT_BASIC_RACK_SN "BasicRackSN"
#define PROPERTY_RACK_ASSET_MGMT_RACK_FUNCTION "RackFunction"

#define PROPERTY_RACK_ASSET_MGMT_SVC_STATUS     "ServiceStatus"
#define PROPERTY_RACK_ASSET_MGMT_HEALTH			"Health"
#define PROPERTY_RACK_ASSET_MGMT_FIXTURE_ID    "FixtureID"
#define PROPERTY_RACK_ASSET_MGMT_WRITE_TAG_TIME "WriteTagTime"
#define PROPERTY_RACK_ASSET_MGMT_UPGRADE_MCU_TIME "UpgradeMCUWaitTime"

#define METHOD_RACK_ASSET_MGMT_SET_LOAD_CAPACITY        "SetLoadCapacity"

#define METHOD_RACK_ASSET_MGMT_SET_RATED_POWER          "SetRatedPower"

#define METHOD_RACK_ASSET_MGMT_SETCOUNTRY       "SetCountry"
#define METHOD_RACK_ASSET_MGMT_SETTERRITORY     "SetTerritory"
#define METHOD_RACK_ASSET_MGMT_SETCITY          "SetCity"
#define METHOD_RACK_ASSET_MGMT_SETSTREET        "SetStreet"
#define METHOD_RACK_ASSET_MGMT_SETHOUSENUM      "SetHouseNumber"
#define METHOD_RACK_ASSET_MGMT_SETNAME          "SetName"
#define METHOD_RACK_ASSET_MGMT_SETPOSTCODE      "SetPostCode"
#define METHOD_RACK_ASSET_MGMT_SETBUILDING      "SetBuilding"
#define METHOD_RACK_ASSET_MGMT_SETFLOOR         "SetFloor"
#define METHOD_RACK_ASSET_MGMT_SETROOM          "SetRoom"
#define METHOD_RACK_ASSET_MGMT_SETROW           "SetRow"
#define METHOD_RACK_ASSET_MGMT_SETRACK          "SetRack"
#define METHOD_RACK_ASSET_MGMT_SETDIRECTION        "SetDirection"
#define METHOD_RACK_ASSET_MGMT_SET_LOCATION_INFO        "SetLocationInfo"
#define METHOD_SET_RACK_DIMENSIONS              "SetRackDimensions"
#define METHOD_SET_UNIT_COUNT_TOTAL             "SetUnitCountTotal"
#define METHOD_SET_UNIT_COUNT_OCCUPIED          "SetUnitCountOccupied"
#define METHOD_SET_ASSET_MGMT_STAT              "SetAssetMgmtStatus"

/* 机柜管理类*/
#define CLASS_RACK_MANAGEMENT					        "RackManagement"
#define PROPERTY_RACK_MANAGEMENT_CURRENT_POWER		    "CurrentPower"
#define PROPERTY_RACK_MANAGEMENT_EN_POW_CAP_COEFFICIENT	 "EnPowCapCoefficient"
#define PROPERTY_RACK_MANAGEMENT_REL_POW_CAP_COEFFICIENT "RelPowCapCoefficient"
#define PROPERTY_RACK_MANAGEMENT_CAPPING_POWER		     "CappingPower"
#define PROPERTY_RACK_MANAGEMENT_ACTUAL_CAPPING_POWER   "ActualCappingPower"
#define PROPERTY_RACK_MANAGEMENT_POWER_CAPPING_STATE	 "PowerCappingState"
#define PROPERTY_RACK_MANAGEMENT_POWER_CAPPING_ALLOWED	 "PowerCappingAllowedStatus"
#define PROPERTY_RACK_MANAGEMENT_ALARM_STATUS	         "AlarmStatus"
#define PROPERTY_RACK_MANAGEMENT_POWER_CAPPING_ENABLED	 "PowerCappingEnabled"
#define PROPERTY_RACK_MANAGEMENT_CAPPING_STRATEGY        "CappingStrategy"
/* BEGIN: Added by cwx70046 chenhongliang, 2020/4/15 */
#define PROPERTY_RACK_MANAGEMENT_PEAK_CLIPPING_STATUS    "PeakClippingStatus"
/* END:   Added by cwx70046 chenhongliang, 2020/4/15 */
#define PROPERTY_RACK_MANAGEMENT_ASSIGNED_DEV_COUNTS     "AssignedDevCounts"
#define PROPERTY_RACK_MANAGEMENT_DEV_SUPPORT_LIST_COUNTS "DevSupportListCounts"
#define PROPERTY_RACK_MANAGEMENT_POW_CAP_SET_FLAG    	 "PowCapSetFlag"
#define PROPERTY_RACK_MANAGEMENT_INIT_DEV_DISCOVERED     "InitDevDiscovered"
#define PROPERTY_RACK_MANAGEMENT_AGENT_SVC_STATUS        "AgentServiceStatus"
#define PROPERTY_RACK_MANAGEMENT_DFT_TOOLBOX_MODE        "DftToolBoxModeFlag"
#define PROPERTY_RACK_MANAGEMENT_PEAK_CLIPPING_ENABLED    "PeakClippingEnabled" // 削峰填谷使能状态
#define PROPERTY_RACK_DA_CFG_FILE_NAME                  "RackDaCfgFileName" // 机柜数据采集配置文件名称
#define METHOD_GET_RACK_DA_CFG                          "GetRackDaCfg"      // 查询机柜数据采集配置方法
#define METHOD_UPDATE_RACK_DA_CFG                       "UpdateRackDaCfg"   // 更新机柜数据采集配置方法
#define METHOD_UPDATE_POW_CAP_CFG                        "UpdatePowCapCfg"   // 更新机柜功耗封顶参数设置
#define METHOD_SET_RACK_INIT_DEV_DISCOVERED              "SetRackInitDevDiscovered"   // 更新机柜功耗封顶参数设置
#define METHOD_QUERY_RACK_DA_DATA                       "QueryRackDaData"   // 筛查机柜数据采集结果方法
#define METHOD_SET_TOOLBOX_MODE                         "SetDftToolBoxModeFlag"
#define METHOD_SAVE_POWER_CAPPING_CONFIG                "SavePowerCappingConfig" // 校验机柜功耗封顶配置
#define METHOD_SET_PEAK_CLIPPING_ENABLED                "SetPeakClippingEnabled" // 设置削峰填谷使能状态
#define METHOD_SET_PEAK_CLIPPING_BACKUP_POWER           "SetPeakClippingBackupPower" // 设置削峰填谷备电电量
#define METHOD_SET_PEAK_CLIPPING_BACKUP_PERIOD          "SetPeakClippingBackupPeriod" // 设置削峰填谷备电时间
#define METHOD_RACK_MANAGEMENT_NOTIFY_POWER_MGNT_EVENT      "NotifyPowerMgntEvent" // 接收电源拔出事件

/* 资产管理板/U位识别板类 */
#define CLASS_ASSET_LOCATE_BOARD				"AssetLocateBoard"
#define PROPERTY_ASSET_LOCATE_BRD_U_COUNT	    "UCount"
#define PROPERTY_ASSET_LOCATE_BRD_ID            "BoardId"
#define PROPERTY_ASSET_LOCATE_BRD_NAME          "Name"
#define PROPERTY_ASSET_LOCATE_BRD_PRODUCT	    "ProductName"
#define PROPERTY_ASSET_LOCATE_BRD_SLOT	        "Slot"
#define PROPERTY_ASSET_LOCATE_BRD_PCB_ID	    "PcbId"
#define PROPERTY_ASSET_LOCATE_BRD_PCB_VER	    "PcbVer"
#define PROPERTY_ASSET_LOCATE_BRD_BOOT_VER	    "BootloaderVer"
#define PROPERTY_ASSET_LOCATE_BRD_WARNING_STATUS "WarningStatus"
#define PROPERTY_ASSET_LOCATE_BRD_FIRM_VER	    "FirmwareVersion"
#define PROPERTY_ASSET_LOCATE_BRD_MANU          "Manufacturer"
#define PROPERTY_ASSET_LOCATE_BRD_FIRM_NAME     "FirmwareName"
#define PROPERTY_ASSET_LOCATE_BRD_EXPECT_VER    "ExpectFirmVer"

/* U位信息类 */
#define CLASS_UNIT_INFO							"UnitInfo"
/* U位信息: 内存中维护的属性 */
#define PROPERTY_UNIT_INFO_REF_COMP				"RefComponent"
#define PROPERTY_UNIT_INFO_RES_ID				"ResId"
#define PROPERTY_UNIT_INFO_U_NUM                "UNum"
#define PROPERTY_UNIT_INFO_DETECTED             "Detected"
#define PROPERTY_UNIT_INFO_STATUS               "Status"
#define PROPERTY_UNIT_INFO_LED_STATUS           "LedStatus"
#define PROPERTY_UNIT_INFO_INDICATOR_COLOR      "IndicatorColor"
#define PROPERTY_UNIT_INFO_INDICATOR_LED        "IndicatorLED"
#define PROPERTY_UNIT_INFO_INDICATOR_FUNC       "IndicatorFunction"
#define PROPERTY_UNIT_INFO_WARN_STATUS          "WarnStatus"
#define PROPERTY_UNIT_INFO_LAST_PST_TIME        "LastPresentTime"
/* U位信息: 标签中维护的属性 */
#define PROPERTY_UNIT_INFO_DEV_MODEL        	"Model"
#define PROPERTY_UNIT_INFO_DEV_TYPE        		"DeviceType"
#define PROPERTY_UNIT_INFO_MANUFACTURER        	"Manufacturer"
#define PROPERTY_UNIT_INFO_UID                  "RFIDTagUID"
#define PROPERTY_UNIT_INFO_SER_NUM        		"SerialNumber"
#define PROPERTY_UNIT_INFO_LAST_SER_NUM         "LastSerialNumber"
#define PROPERTY_UNIT_INFO_PART_NUM             "PartNumber"
#define PROPERTY_UNIT_INFO_U_HEIGHT        		"UHeight"
#define PROPERTY_UNIT_INFO_AVAILABLE_RACK_SPACE	"AvailableRackSpaceU"
#define PROPERTY_UNIT_INFO_LIFE_CYCLE           "LifeCycleYear"
#define PROPERTY_UNIT_INFO_CHK_IN_TIME          "CheckInTime"
#define PROPERTY_UNIT_INFO_WEIGHT        		"WeightKg"
#define PROPERTY_UNIT_INFO_RATED_POWER          "RatedPowerWatts"
#define PROPERTY_UNIT_INFO_ASSET_OWNER          "AssetOwner"
#define PROPERTY_UNIT_INFO_EXTEND_FIELD         "ExtendField"
#define PROPERTY_UNIT_INFO_DISCOVERED_TIME      "DiscoveredTime"
#define PROPERTY_UNIT_INFO_RW_CAPABILITY        "RWCapability"
/* U位信息类方法 */
#define METHOD_SET_UNIT_INFO			        "SetUnitInfo"
#define METHOD_SET_LED_STATUS        			"SetLedStatus"
#define METHOD_SET_UNIT_INSTALLED_STATUS  "SetUnitInstalledStatus"

#define TRANSFER_UNIT_INFO_DEV_MODEL        	    "1"
#define TRANSFER_UNIT_INFO_DEV_TYPE        		"2"
#define TRANSFER_UNIT_INFO_MANUFACTURER        	"3"
#define TRANSFER_UNIT_INFO_SER_NUM        		"4"
#define TRANSFER_UNIT_INFO_PART_NUM              "5"
#define TRANSFER_UNIT_INFO_HIGHT        		    "6"
#define TRANSFER_UNIT_INFO_LIFE_CYCLE          	"7"
#define TRANSFER_UNIT_INFO_CHK_IN_TIME           "8"
#define TRANSFER_UNIT_INFO_WEIGHT        		    "9"
#define TRANSFER_UNIT_INFO_RATED_POWER           "10"
#define TRANSFER_UNIT_INFO_ASSET_OWNER           "11"
#define TRANSFER_UNIT_INFO_EXTENDFIELD           "ExtendField"

/* 设备信息类，用于iRM管理柜内设备 */
#define CLASS_DEVICE_INFO						"DeviceInfo"
#define PROPERTY_DEVICE_INFO_REF_UNIT			"RefUnitInfo"       //设备所归属的U位对象
#define PROPERTY_DEVICE_INFO_REF_UNUM			"RefUNum"           //设备所归属的U位号
#define PROPERTY_DEVICE_INFO_SLOT				"Slot"              //设备槽位号
#define PROPERTY_DEVICE_INFO_VENDOR				"Vendor"
#define PROPERTY_DEVICE_INFO_SER_NUM            "SerialNumber"      //设备的序列号
#define PROPERTY_DEVICE_INFO_LAST_SER_NUM       "LastSerialNumber"      //上一次发现设备的序列号
#define PROPERTY_DEVICE_INFO_DISCOVERED			"Discovered"        //设备被发现(上线)标志
#define PROPERTY_DEVICE_INFO_POW_CAP_STATE      "PowCapState"
#define PROPERTY_DEVICE_INFO_POW_CAP_START_TIME     "PowCapStartTime"
#define PROPERTY_DEVICE_INFO_POW_CAP_END_TIME       "PowCapEndTime"
#define PROPERTY_DEVICE_INFO_POW_CAP_DURATION       "PowCapDuration"
#define PROPERTY_DEVICE_INFO_POW_CAP_TIMES          "PowCapTimes"
#define PROPERTY_DEVICE_INFO_POW_CAP_FIRST_TIME     "PowCapFisrtTime"
#define PROPERTY_DEVICE_INFO_POW_CAP_TRIG_STA       "PowCapTriggerStatus"
#define PROPERTY_DEVICE_INFO_POW_CAP_LAST_TRIG_STA  "PowCapLastTriggerStatus"
#define PROPERTY_DEVICE_INFO_POW_CAP_RESULT         "PowCapResult"
#define PROPERTY_DEVICE_INFO_POW_CAP_FILED_TIMES    "PowCapFailedTimes"
#define PROPERTY_DEVICE_INFO_CURRENT_POWER          "PowerConsumedWatts"    // 设备的功耗封顶的当前值
#define PROPERTY_DEVICE_INFO_POWER_LIMIT_VALUE  "MinPowerLimitInWatts"  // 设备功耗封顶下限值
#define PROPERTY_DEVICE_INFO_LAST_POWER_LIMIT_VALUE "LastMinPowerLimitInWatts"
#define PROPERTY_DEVICE_INFO_POWER_MAX_VALUE        "MaxPowerLimitInWatts"
#define PROPERTY_DEVICE_INFO_LAST_POWER_MAX_VALUE   "LastMaxPowerLimitInWatts"
#define PROPERTY_DEVICE_INFO_AVERAGE_POWER      "AveragePowerWatts"     // 设备平均功耗
#define PROPERTY_DEVICE_INFO_PEAK_POWER         "PeakPowerWatts"        // 设备峰值功耗
#define PROPERTY_DEVICE_INFO_UPPER_BOUND_POWER      "UpperBoundPower"
#define PROPERTY_DEVICE_INFO_LOWER_BOUND_POWER      "LowerBoundPower"
#define PROPERTY_DEVICE_INFO_PRIORITY               "Priority"
#define PROPERTY_DEVICE_INFO_ASSIGNED_FLAG          "AssignedFlag"
#define PROPERTY_DEVICE_INFO_POW_CAP_SUPPORT        "PowCapSupport"
#define PROPERTY_DEVICE_INFO_REV_POW_CAP_VALUE      "RevPowCapValue"
#define PROPERTY_DEVICE_INFO_LIMIT_IN_WATTS         "LimitInWatts"          //设备查询到的功耗封顶值
#define PROPERTY_DEVICE_INFO_CAPPING_POWER          "CappingPower"          //功耗封顶下发的封顶值
#define PROPERTY_DEVICE_INFO_LAST_CAPPING_POWER     "LastCappingPower"
#define PROPERTY_DEVICE_INFO_RF_GET_POWER_URL       "RFGetPowerUrl"
#define PROPERTY_DEVICE_INFO_RF_PATCH_POWER_ETAG    "RFPatchPowerEtag"
#define PROPERTY_DEVICE_INFO_IP_ADDR			"IpAddr"
#define PROPERTY_DEVICE_INFO_IPV6_ADDR			"Ipv6Addr"
#define PROPERTY_DEVICE_INFO_TOKEN_STATUS       "TokenStatus"       //Token状态标志，0无效，1有效
#define PROPERTY_DEVICE_INFO_MAX_RETRY_TIMES    "MaxRetryTimes"     //处理REST请求时的最大允许重试次数
#define PROPERTY_DEVICE_INFO_DA_SVC_STATUS      "DataAcqSvcStatus"      //从iBMC查到的数据采集服务开关状态
#define PROPERTY_DEV_DA_ACTIVE_DB_FILE_NAME     "ActiveDBFileName"
#define PROPERTY_DEVICE_INFO_DA_ITEMS           "AcquisitionItems"      //设备的数据采集项
#define PROPERTY_DEVICE_INFO_DA_CUR_TIMESTAMP   "CurrentTimestamp"      //设备的数据采集时间戳
#define PROPERTY_DEVICE_INFO_HEART_BEAT_LAST_RECV_TIME  "HeartBeatLastRecvTime" // 设备心跳最近一次响应的时间
#define PROPERTY_DEVICE_INFO_HEART_BEAT_OFFLINE_TYPE    "HeartBeatOfflineType"  // 设备心跳丢失异常离线告警

/* 设备信息类方法 */
#define METHOD_SAVE_DEVICE_SER_NUM              "SaveSerialNum"     //保存SSDP发现的序列号到设备信息对象
#define METHOD_SAVE_DEVICE_LAST_SER_NUM         "SaveLastSerialNum" //保存SSDP上一轮发现的设备序列号信息
#define METHOD_SET_PERIOD_POWER                 "SetPeriodPower"    // 设置周期功耗信息
#define METHOD_SAVE_DEVICE_IP_ADDR              "SaveIpAddr"        //保存SSDP发现的设备IP地址到设备信息对象
#define METHOD_SAVE_DEVICE_IPV6_ADDR        	"SaveIpv6Addr"      //保存SSDP发现的设备IPv6地址到设备信息对象
#define METHOD_SET_DISCOVER_STATUS              "SetDiscoverStatus" //设置设备的SSDP发现状态
#define METHOD_SET_DEVICE_TOKEN                 "SetDeviceToken"    //设置设备的Token
#define METHOD_HANDLE_REST_REQUEST              "HandleRestRequest" //处理REST请求

/* BEGIN: Added by gwx455466, 2018/12/11 9:49:26   问题单号:SR.SFEA02130837.003 */
#define CLASS_CHIP_VRD_PARAMETER_CONFIG              "ChipVrdParameterConfig"
#define PROPERTY_CHIP_VRD_PC_CHIP_TYPE                  "ChipType"
#define PROPERTY_CHIP_VRD_PC_ID                       "Id"
#define PROPERTY_CHIP_VRD_PC_SET_DOMAIN                       "SetDomain"
#define PROPERTY_CHIP_VRD_PC_PARAM_TYPE         "ParamType"
#define PROPERTY_CHIP_VRD_PC_PARAM_VALUE        "ParamValue"

/*ParamFrom - 参数来源0:Local-不需要起线程 1:ME  2:IMU 从IMU或者ME获取的都需要起起线程，
只要判断整块单板其中一个对象即可,因为IMU和ME只能存在一个,该参数xml固定写死,不需要自动配置的置为0 */
#define PROPERTY_CHIP_VRD_PC_PARAM_FROM         "ParamFrom"
/*ParamReadStatus 标识从远端是否读取到数据1表示正在读取,2表示读取完成, 3表示读取完成但是参数无效(不需要配置)*/
#define PROPERTY_CHIP_VRD_PC_PARAM_READ_STATUS  "ParamReadStatus"
/*参数从期望设置到的目标值*/
#define PROPERTY_CHIP_VRD_PC_PARAM_TARGET       "ParamTarget"
#define PROPERTY_CHIP_VRD_PC_VMIN               "VMin"
/*当前参数配置阶段*/
#define PROPERTY_CHIP_VRD_PC_CONFIG_FLAG        "ConfigFlag"
/**/
#define PROPERTY_CHIP_VRD_PC_CONFIG_STATUS      "ConfigStatus"
#define PROPERTY_CHIP_VRD_PC_PREPAREACTION  "PrepareAction"
#define PROPERTY_CHIP_VRD_PC_FINISHACTION   "FinishAction"

#define METHOD_GET_VRD_PARAM_INFO        "GetVrdParamInfo" // 获取当前VRD电源寄存器实际值
#define METHOD_SET_VRD_PARAM_REG         "SetVrdParamInfo" // 将bmc本地数据刷新到vrd电源中
#define METHOD_SWITCH_LOADLINE           "SwitchLoadline"


#define CLASS_ACCESSOR_UPDATE_ACTION                        "AccessorUpdateAction"
#define PROPERTY_ACCESSOR_UPDATE_DESTINATION            "Destination"
#define PROPERTY_ACCESSOR_UPDATE_DATA                         "Data"
#define PROPERTY_ACCESSOR_UPDATE_SHITF_BIT_CNT                "ShiftBitCount"
#define PROPERTY_ACCESSOR_UPDATE_MASK                         "Mask"
/* END:   Added by gwx455466, 2018/12/11 9:49:30 */

/* BEGIN: Modified by zwx566676, 2019/1/9 16:23:0   AR号:UADP164484 */
#define CLASS_NAME_VARIANT_EXPRESSION                   "VariantExpression"

/* BEGIN: Modified by l00389091, 2019/3/25   问题单号:DTS2019032100931 */
#define  PROPERTY_VARIANT_EXPRESSION_A           "A"
#define  PROPERTY_VARIANT_EXPRESSION_B           "B"
#define  PROPERTY_VARIANT_EXPRESSION_C           "C"
/* END: Modified by l00389091, 2019/3/25  */

#define  PROPERTY_VARIANT_EXPRESSION_VALUE           "Value"
#define  PROPERTY_VARIANT_EXPRESSION_INDEX            "Index"
/* END:   Modified by zwx566676, 2019/1/9 16:23:6 */

#define  CLASS_BOARD_INFO_COLLECT_CFG    			 "BoardInfoCollectCfg"
#define  PROPERTY_BOARD_INFO_COLLECT_REF_CALSS_OBJ   "RefClassObj"
#define  PROPERTY_BOARD_INFO_COLLECT_DISPLAY_NAME    "DisplayName"
#define  PROPERTY_BOARD_INFO_COLLECT_INFO_NAME       "CollectInfoName"
#define  PROPERTY_BOARD_INFO_COLLECT_PROPERTY_OBJ_ARRAY       "CollectProperyObjArray"

#define  CLASS_BOARD_TYPE_DEFINITION                    "BoardTypeDefinition"
#define  PROPERTY_BOARD_TYPE_DEFINITION_REF_CALSS_OBJ   "RefClass"
#define  PROPERTY_BOARD_TYPE_DEFINITION_REF_PROPERTY    "RefProperty"
#define  PROPERTY_BOARD_TYPE_DEFINITION_REF_PROPERTY_MATCH_VALUE    "ValueMatchRefProperty"
#define  PROPERTY_BOARD_TYPE_DEFINITION_MATCH_TYPE    "MatchType"
#define  PROPERTY_BOARD_TYPE_DEFINITION_DISPLAY_NAME    "DisplayName"

#define  CLASS_COLLECT_PROPERTY_DEFINITION                  "CollectPropertyDefinition"
#define  PROPERTY_COLLECT_PROPERTY_DEFINITION_TYPE          "Type"
#define  PROPERTY_COLLECT_PROPERTY_DEFINITION_REF_CALSS_OBJ          "RefClassObj"
#define  PROPERTY_COLLECT_PROPERTY_DEFINITION_PROPERTY         "RefProperty"
#define  PROPERTY_COLLECT_PROPERTY_DEFINITION_DISPLAY_PROPERTY         "DisplayProperty"

#define CLASS_SYS_BATTERY                      "SysBattery"
#define PROPERTY_SYS_BATTERY_RATED_CAP         "RatedCapacity"
#define PROPERTY_SYS_BATTERY_FULL_CAP          "FullCapacity"
#define PROPERTY_SYS_BATTERY_REMAIN_CAP        "RemainCapacity"
#define PROPERTY_SYS_BATTERY_BACKUP_BAT        "BackupBattery"
#define PROPERTY_SYS_BATTERY_BACKUP_POWER      "BackupPower"
#define PROPERTY_SYS_BATTERY_BACKUP_PERIOD     "BackupPeriod"
#define PROPERTY_SYS_BATTERY_BAT_PRES_COUNTS   "BatteryPresentCounts"
#define PROPERTY_SYS_BATTERY_ACTUAL_BACKUP_CAP      "ActualBackupCap"
#define PROPERTY_SYS_BATTERY_ACTUAL_BACKUP_TIME     "ActualBackupTime"
#define PROPERTY_SYS_BATTERY_BACKUP_SCENARIO   "BackupScenario"
#define PROPERTY_SYS_BATTERY_BACKUP_CAP_ALARM  "BackupCapAlarm"
#define PROPERTY_SYS_BATTERY_DISCHARGING_BBU_CNT    "DischargingBBUCnt"
#define PROPERTY_SYS_BATTERY_REMAIN_DISCHARGING_SEC    "RemainDischargingSec"
#define METHOD_SET_BACKUP_CFG                  "SetBackupConfig"
#define METHOD_SET_MAX_CHARGE_CURRENT           "SetMaxChargeCurrent"
#define METHOD_SET_SYS_BATTERY_UPGRADING_SLOTS  "SetSysBatteryUpgradingSlots"

#define PROPERTY_SYS_BATTERY_EXPECTED_WORK_CNT  "ExpectedWorkBatteryCnt"
/* BEGIN: Added by cwx70046 chenhongliang, 2020/4/15 */
#define PROPERTY_SYS_BATTERY_ALARM_STATUS      "AlarmStatus"
/* END:   Added by cwx70046 chenhongliang, 2020/4/15 */

#define CLASS_LED_CTRL_NAME                 "LedCtrl"
#define PROPERTY_LED_CTRL_LED_TEST          "LedTest"
#define PROPERTY_LED_CTRL_CTRL_VALUE        "CtrlValue"
#define PROPERTY_LED_CTRL_CTRL_ACCESSOR     "CtrlAccessor"
#define PROPERTY_LED_CTRL_ON_VALUE          "LedOnValue"


// 优化LOCAL gboolean redfish_get_board_class_name( OBJ_HANDLE obj_handle, gchar* o_classname, gint32 length )接口实现，将原放在redfish的宏定义放在公共文件中
#define RFPROP_TYPE_TWIN_NODE_BACKPLANE                 "TwinNodeBackplane"
#define RFPROP_TYPE_BOARD_PCIE_ADAPTER                  "PCIeTransformCard"
/* BEGIN: Modified by jwx372839, 2017/11/27 17:30:36   问题单号:直通卡管理需求 */
#define RFPROP_TYPE_PASS_THROUGH_CARD                     "PassThroughCard"
/* END:   Modified by jwx372839, 2017/11/27 17:30:40 */
/* BEGIN: Modified by zwx382233, 2017/3/28 19:24:42   问题单号:DTS2017032211195 */
#define RFPROP_CLASS_LEDBOARD                           "LedBoard"
/* END:   Modified by zwx382233, 2017/3/28 19:24:45 */

/* BEGIN: Modified by w00590405, 2021/8/24  增加lsw相关配置 */
#define  OBJ_LSW_PEER_PRESENCE             "PeerPresence"
#define  PROPERTY_LSW_VALUE                "Value"
/* END:   Modified by w00590405, 2021/8/27 11:49 */

/* BEGIN: Modified by m00446762, 2019/7/15  增加lsw相关配置 */
#define  CLASS_LSW_PUBLIC_ATTR_CENTER      "LswPublicAttrCenter"
#define  OBJ_LSW_PUBLIC_CENTER_CP          "LswCenterCP"
#define  PROPERTY_LSW_AS_STATUS            "AsStatus"
#define  PROPERTY_LSW_PUBLIC_PLANE_NAME           "PlaneName"
#define  PROPERTY_LSW_PUBLIC_PLANE_ID             "PlaneId"
#define  PROPERTY_LSW_CYCLE_SYNC_SWITCH           "CycleSyncSwitch"
#define  PROPERTY_LSW_SERVER_PORT                 "ServerPort"
#define  PROPERTY_LSW_PUBLIC_VLAN                 "Vlan"
#define  PROPERTY_LSW_PUBLIC_TRUNK                "Trunk"
#define  PROPERTY_LSW_PUBLIC_PORT_NUMS            "PortNums"
#define  PROPERTY_LSW_PUBLIC_CHIP_STATUS          "ChipStatus"
#define  PROPERTY_LSW_PUBLIC_CHIP_CHANNEL_STATUS     "ChipChannelStatus"

#define  METHOD_LSW_GET_CHIP_STATUS        "GetChipStatus"
#define  METHOD_LSW_GET_VLAN               "GetVlan"
#define  METHOD_LSW_GET_VLAN_PORT          "GetVlanPort"
#define  METHOD_LSW_CREATE_VLAN            "CreateVlan"
#define  METHOD_LSW_DELETE_VLAN            "DeleteVlan"
#define  METHOD_LSW_CREATE_TRUNK           "CreateTrunk"
#define  METHOD_LSW_DELETE_TRUNK           "DeleteTrunk"
#define  METHOD_LSW_GET_TRUNK_PORT         "GetTrunkPort"
#define  METHOD_LSW_PUBLIC_ATTR_SYNC       "PropertySyncMethod"

#define  CLASS_LSW_PUBLIC_ATTR_LOCAL_CP    "LswPublicAttrLocalCP"
#define  METHOD_CP_GET_STATIS              "GetPortStatistics"
#define  METHOD_CP_GET_L2_TABLE            "GetL2Table"
#define  METHOD_CP_GET_PORT_INFO           "GetPortInfo"
#define  METHOD_CP_GET_CHIP_INFO           "GetChipInfo"
#define  METHOD_CP_SET_CMESH_PARA          "SetCmeshPara"
#define  PROPERTY_LSW_PUBLIC_CMESH_MODE    "CmeshMode"
#define  PROPERTY_LSW_PUBLIC_SLOTID        "SlotId"
#define  CLASS_LSW_PUBLIC_ATTR_LOCAL_DP    "LswPublicAttrLocalDP"
#define  PROPERTY_LSW_CORE_TEMP            "TemperatureSensor"
#define  PROPERTY_LSW_MESH_M_PATH          "MeshMPathMode"
#define  PROPERTY_LSW_MESH_M_PATH_STATUS   "MeshMPathStatus"
#define  PROPERTY_LSW_MESH_M_PATH_RELAY    "MeshMPathRelayNode"
#define  PROPERTY_LSW_MESH_M_PATH_CTRL     "MeshMPathCtrlNode"
#define  PROPERTY_LSW_MESH_M_PATH_UNDER    "MeshMPathUnderCtrlNode"
#define  METHOD_DATA_STATIS                "IpmcDataPortStatistics"
#define  METHOD_DATA_L2                    "IpmcDataL2Table"
#define  METHOD_DATA_INFO                  "IpmcDataPortInformation"
#define  METHOD_DATA_SET_PORT_FEC          "IpmcDataPortFec"
#define  METHOD_DATA_SET_MESH_MPATH        "IpmcDataSetMeshMPath"
#define  METHOD_DATA_GET_MESH_MPATH        "IpmcDataGetMeshMPath"
#define  METHOD_MESH_MPATH_CONDITION_CHECK "MeshMPathConditionCheck"

#define  OBJ_LSW_PUBLIC_ATTR_LOCAL         "LswPublicAttrLocal"
#define  PROPERTY_LSW_LOCAL_CTRL_NODE_NAME  "NodeName"

#define  CLASS_LSW_PORT_ATTR_CENTER        "LswPortAttrCenter"
#define  PROPERTY_LSW_CENTER_SLOTID        "SlotId"
#define  PROPERTY_LSW_CENTER_PORT_NAME     "PortName"
#define  PROPERTY_LSW_SUB_PORT_NAME        "SubPortName"
#define  PROPERTY_LSW_PORT_POSITION        "Position"
#define  PROPERTY_LSW_LOGIC_PORT_NUM       "LogicPortNum"
#define  PROPERTY_LSW_UNIT                 "Unit"
#define  PROPERTY_LSW_PORT                 "Port"
#define  PROPERTY_LSW_PORT_TYPE            "PortType"
#define  PROPERTY_LSW_ADJACENCY_UNIT       "AdjacencyUnit"
#define  PROPERTY_LSW_STP_STATE            "StpState"
#define  PROPERTY_LSW_PLANE                "Plane"
#define  PROPERTY_LSW_PLANE_ID             "PlaneId"
/* Local的属性值 */
#define  PROPERTY_LSW_MTU                  "Mtu"
#define  PROPERTY_LSW_PVID                 "Pvid"
#define  PROPERTY_LSW_PEER_BLADE_STATUS    "PeerBladePresent"
#define  PROPERTY_LSW_PEER_BLADE_NUM       "PeerBladeNum"
#define  PROPERTY_LSW_IPMI_VALID           "IpmiValid"

/* Center的属性值 */
#define  PROPERTY_CENTER_MTU               "Mtu1"
#define  PROPERTY_CENTER_PVID              "Pvid1"
#define  PROPERTY_LSW_VLAN                 "Vlan"
#define  PROPERTY_LSW_TAG                  "Tag"
#define  PROPERTY_LSW_TRUNK_ID             "TrunkId"
#define  PROPERTY_LSW_LINK_STATUS          "LinkStatus"
#define  PROPERTY_LSW_SPEED                "Speed"
#define  PROPERTY_LSW_OAM_STATUS           "OamStatus"
#define  PROPERTY_LSW_OAM_ENABLE           "OamEnable"
#define  PROPERTY_LSW_SHUTDOWN             "Shutdown"
#define  PROPERTY_LSW_LOOPSTATUS           "LoopStatus"
#define  PROPERTY_LSW_TRANSRULE            "TransRule"
#define  PROPERTY_LSW_SUPPRESSION          "Suppression"
#define  PROPERTY_LSW_AUTONEGO             "Autonego"
#define  PROPERTY_LSW_DUPLEX               "Duplex"
#define  PROPERTY_LSW_STP_STATE            "StpState"
#define  PROPERTY_LSW_STATUS               "EnableStatus"

#define  METHOD_LSW_GET_PORT_NAME          "GetPortName"
#define  METHOD_LSW_GET_PORT_INFO          "GetPortInfo"
#define  METHOD_LSW_GET_PORT_STATISTICS    "GetPortStatistics"
#define  METHOD_LSW_SET_PORT_MTU           "SetPortMtu"
#define  METHOD_LSW_GET_PORT_MTU           "GetPortMtu"
#define  METHOD_LSW_SET_PORT_ENABLE        "SetPortEnable"
#define  METHOD_LSW_ADD_PORT_VLAN          "AddPortVlan"
#define  METHOD_LSW_DEL_PORT_VLAN          "DelPortVlan"
#define  METHOD_LSW_GET_PORT_VLAN          "GetPortVlan"
#define  METHOD_LSW_ADD_PORT_TRUNK         "AddPortTrunk"
#define  METHOD_LSW_DEL_PORT_TRUNK         "DelPortTrunk"
#define  METHOD_LSW_GET_LINK_STATUS        "GetLinkStatus"
#define  METHOD_LSW_GET_SPEED              "GetSpeed"
#define  METHOD_LSW_GET_LOOP_MODE          "GetLoopMode"
#define  METHOD_LSW_SET_LOOP_MODE          "SetLoopMode"
#define  METHOD_LSW_GET_TRANS_RULE         "GetTransRule"
#define  METHOD_LSW_SET_TRANS_RULE         "SetTransRule"
#define  METHOD_LSW_GET_SUPPRESSION        "GetSuppression"
#define  METHOD_LSW_SET_SUPPRESSION        "SetSuppression"
#define  METHOD_LSW_CLR_PORT_STATISTICS    "ClrPortStatistics"
#define  METHOD_LSW_ENABLE_PORT            "EnablePort"
#define  METHOD_LSW_DISABLE_PORT           "DisablePort"
#define  METHOD_LSW_PORT_ATTR_SYNC         "PropertySyncMethod"
#define  METHOD_LSW_UPDATE_LINK_STATUS     "UpdateLinkStatus"
#define  METHOD_LSW_SET_PORT_PVID          "SetPortPvid"
#define  METHOD_LSW_GET_PORT_PVID          "GetPortPvid"

#define  METHOD_LSW_SET_OM_CHANNEL         "SetOmChannel"
#define  METHOD_LSW_SET_OM_CHANNEL_VLAN    "SetOmChannelVlan"
#define  METHOD_LSW_GET_OM_CHANNEL         "GetOmChannel"
#define  METHOD_LSW_GET_PORT_PEER          "GetPortPeerInfo"

#define  CLASS_LSW_PORT_ATTR_LOCAL_CP      "LswPortAttrLocalCP"
#define  PROPERTY_LSW_SDK_SO_NAME           "sdk_so_name"
#define  PROPERTY_LSW_SGMI_CHECK_PORTS      "sgmi_check_port"
#define  PROPERTY_LSW_SDK_ADPT_TYPE         "sdk_adpt_type"
#define  PROPERTY_LSW_MAX_PKT_LEN           "max_pkt_len"
#define  PROPERTY_LSW_RXCRC                 "RxCRC"
#define  PROPERTY_LSW_TXCRC                 "TxCRC"
#define  CLASS_LSW_PORT_ATTR_LOCAL_DP       "LswPortAttrLocalDP"
#define  PROPERTY_LSW_LINK_HEALTH           "LinkHealth"

#define  CLASS_LSW_CHIP_INFO                "LswChipInfo"
#define  PROPERTY_LSW_CHIP_INDEX            "ChipIndex"
#define  PROPERTY_LSW_CHIP_FAULT            "ChipFault"
#define  PROPERTY_LSW_RST_TYPE              "ResetType"
#define  PROPERTY_LSW_CHIP_RST              "ChipRst"
#define  PROPERTY_LSW_GPIO_PIN              "GpioPin"
#define  PROPERTY_LSW_CHIP_POWER_STATE      "ChipPowerState"
#define  CLASS_LSW_SWITCH_INFO              "LswSwitchInfo"
#define  CLASS_LSW_PHY_INFO                 "LswPhyInfo"

#define  CLASS_LSW_DFT_CHIP_INFO            "LswDftTestChipInfo"
#define  PROPERTY_LSW_DFT_CHIP_INDEX        "ChipIndex"
#define  PROPERTY_LSW_DFT_TEST_IN_PORT      "LoopTestInPort"

#define  CLASS_LSW_NODE_MANAGE_CENTER      "LswNodeManageCenter"
#define  PROPERTY_LSW_NODE_SLOTID          "SlotId"
#define  PROPERTY_LSW_NODE_PLANE_NAME      "PlaneName"
#define  PROPERTY_LSW_NODE_PLANE_ID        "PlaneId"
#define  PROPERTY_LSW_NODE_VALID           "Valid"
#define  PROPERTY_LSW_NODE_USABLE          "Usable"
#define  PROPERTY_LSW_NODE_ASSERT_FLAG     "AssertFlag"

#define  METHOD_LSW_SET_VALID_STATUS       "SetValidStatus"
#define  METHOD_LSW_VALID_STATUS_UPDATE    "ValidStatusUpdateMethod"

#define  CLASS_LSW_NODE_MANAGE_DP          "LswNodeManageDP"
#define  PROPERTY_LSW_MANUFACTURE_STATUS   "ManufactureStatus"
#define  PROPERTY_LSW_ITF_PORT_MODE        "ItfPortMode"

#define  METHOD_LSW_GET_ITF_PORT_MODE      "GetItfPortMode"
#define  METHOD_LSW_SET_ITF_PORT_MODE      "SetItfPortMode"

#define  CLASS_LSW_NODE_MANAGE_CP          "LswNodeManageCP"
#define  OBJ_LSW_LOCAL_NODE                "LswLocalNode"
#define  PROPERTY_LSW_CENTER_IP_ADDR       "CenterIpAddr"
#define  PROPERTY_LSW_CENTER_SLOT_ID       "CenterSlotId"

#define CLASS_BMC_CORE_MANAGEMENT                       "BMCCoreManagement"
#define PROPERTY_BMC_CORE_TEMP                          "CoreTemp"
#define PROPERTY_DCA_ENABLE                             "DCAEnable"
#define PROPERTY_CORE_DISABLE_TEMP                      "CoreDisableTemp"
#define PROPERTY_CORE_ENABLE_TEMP                       "CoreEnableTemp"
#define PROPERTY_CORE_DISABLE_POWER_OFF_TIME            "CoreDisablePowerOffTime"

#define CLASS_NAME_ETH_FUNCTION_DEFINE             "EthFunctionDefine"
#define PROPERTY_GEN_RANDOM_MAC                    "GenRandomMAC"
#define PROPERTY_SUPPORT_DETACH_ETHGROUP           "SupportDetachEthGroup"
#define PROPERTY_SUPPORT_MULTI_VLAN                "SupportMultiVlan"
#define PROPERTY_UNSUPPORT_VLANID_MOTIFY           "UnsupportVlanIdMotify"
#define PROPERTY_SUPPORT_RESTORE_IP                "SupportRestoreIP"
#define PROPERTY_RESTORE_IP_TRIGGER                "RestoreIPTrigger"
#define PROPERTY_ETH_FUNCTIONS                     "EthFunctions"

#define CLASS_NAME_ETH_FUNCTION                    "EthFunction"
#define PROPERTY_FUNCTION_ENABLE                   "FunctionEnable"
#define PROPERTY_FUNCTION_TYPE                     "Functiontype"
#define PROPERTY_VLAN_STATUS                       "VlanStatus"
#define PROPERTY_IS_NON_VLAN_ETH_USED              "IsNonVlanEthUsed"
#define PROPERTY_SUPPORT_FLOAT_IP                  "SupportFloatIP"
#define PROPERTY_SUPPORT_MULTI_PLANE               "SupportMultiPlane"
#define PROPERTY_CUR_PLANE_ID                      "CurrentPlaneId"
#define PROPERTY_PLANE1_VLAN_ID                    "Plane1VlanId"
#define PROPERTY_PLANE1_GROUP_ID                   "Plane1GroupId"
#define PROPERTY_PLANE1_DESCRIPTION                "Plane1Description"
#define PROPERTY_PLANE1_ETH_NUM                    "Plane1EthNum"
#define PROPERTY_PLANE2_VLAN_ID                    "Plane2VlanId"
#define PROPERTY_PLANE2_GROUP_ID                   "Plane2GroupId"
#define PROPERTY_PLANE2_DESCRIPTION                "Plane2Description"
#define PROPERTY_PLANE2_ETH_NUM                    "Plane2EthNum"

#define PROPERTY_KERBEROS_SUPPORT                   "KerberosSupport"

#define CLASS_NAME_ETH_RENAME                       "EthRename"
#define PROPERTY_ETH_RENAME_ENABLED                 "EthRenameEnabled"
#define PROPERTY_ETH_NAME1                          "EthName1"
#define PROPERTY_ETH_NAME2                          "EthName2"

#define PROPERTY_LOCATION                           "Location"
#define PROPERTY_DEVICE_NAME                        "DeviceName"

#define CLASS_RETIMER_NAME                  "Retimer"
#define OBJ_NAME_RETIMER                    "Cdr5902L_Obj"
#define PROPERTY_RETIMER_ID                 "RetimerId"
#define PROPERTY_RETIMER_LOAD_SEQUENCE      "LoadSequence"
#define PROPERTY_RETIMER_CHIP_NAME          "ChipName"
#define PROPERTY_RETIMER_CHIP_TYPE          "ChipType"
#define PROPERTY_RETIMER_REFCHIP            "RefChip"
#define PROPERTY_RETIMER_REFCOMPONENT       "RefComponent"
#define PROPERTY_RETIMER_REFOPTICAL         "RefOptical"
#define PROPERTY_RETIMER_IS_READY           "IsReady"
#define PROPERTY_RETIMER_SOFT_VER           "SoftVer"
#define PROPERTY_RETIMER_UPG_STATE          "UpgStatus"
#define PROPERTY_RETIMER_CHIP_STATE         "ChipStatus"
#define PROPERTY_RETIMER_LOCATION           "ChipLocation"
#define PROPERTY_RETIMER_UPG_GROUP          "UpgradeGroup"
#define PROPERTY_RETIMER_REQ_ACC            "ReqAccNotify"
#define PROPERTY_RETIMER_ACCESS_SWITCH      "Switch"
#define PROPERTY_RETIMER_I2C_RESET          "Reset"
#define PROPERTY_RETIMER_COMMUNICATION_STATE "CommunicationState"
#define PROPERTY_RETIMER_TEMPERATURE        "Temperature"
#define PROPERTY_RETIMER_CHANNEL_SPEED      "ChannelSpeed"
#define PROPERTY_RETIMER_ADAPT_FLAG         "AdaptFlag"
#define PROPERTY_RETIMER_LINK_STATUS        "LinkStatus"
#define PROPERTY_RETIMER_DIEID              "DIEID"
#define PROPERTY_RETIMER_CHIP_VER           "ChipVersion"
#define PROPERTY_RETIMER_IPMB_INDEX         "IpmiChannelId"
#define PROPERTY_RETIMER_ERR1_TIME          "LastErr1LogTime"
#define PROPERTY_RETIMER_ERR2_TIME          "LastErr2LogTime"
#define PROPERTY_RETIMER_ERR3_TIME          "LastErr3LogTime"
#define PROPERTY_RETIMER_UPG_RESULT         "UpgResult"
#define PROPERTY_RETIMER_REF_COMPONET       "RefComponent"
#define PROPERTY_RETIMER_DISABLE_LANE       "DisableLane"
#define PROPERTY_RETIMER_MANAGE_MASK        "ManageMask"

#define METHOD_RETIMER_NOTIFY_UPGING        "NotifyUpging"
#define METHOD_RETIMER_CHANNEL_SWITCH       "ChannelSwitch"
#define METHOD_RETIMER_SET_UPG_STATUS       "SetUpgStatus"
#define METHOD_RETIMER_SET_NPU_CDR_TEMP     "SetNpuCdrTemp"
#define METHOD_RETIMER_SET_NPU_CDR_VER      "SetNpuCdrVer"
#define METHOD_RETIMER_SET_UPG_RESULT       "SetUpgResult"
#define METHOD_RETIMER_SYNC_INFO            "SyncRetimerInfoMethod"     /* 同步Retimer信息 */

#define CLASS_USBPORTSMGNT_NAME            "USBPortsMgnt"
#define PROPERTY_USBPORTNUM_NAME           "USBPortNum"
#define PROPERTY_USBPORTS_NAME             "USBPorts"
#define OBJECT_USBPORTSMGNT_NAME           "UsbPortsMgnt"
/* fdm带外收集同步获取总线异常信息 */
#define CLASS_SYSTEM_BUS_EXCEPTION   "SystemBusException"
#define OBJECT_SYS_BUS_EXCEPTION     "SysBusExceptionInfo"
#define PROPERTY_EXCEPTION_FLAG      "ExceptionFlag"
#define PROPERTY_EXCEPTION_ADDR_W    "ExceptionAddrW"
#define PROPERTY_EXCEPTION_ADDR_R    "ExceptionAddrR"

#define CLASS_TRANSFORM_IPMI                     "TransformIpmiCmd"
#define PERPORTY_TRANSFORM_IPMI_TRANS_TYPE       "TransformType"
#define PERPORTY_TRANSFORM_IPMI_NETFN            "Netfn"
#define PERPORTY_TRANSFORM_IPMI_CMD              "Cmd"
#define PERPORTY_TRANSFORM_IPMI_SUBCMD_INDEX     "SubcmdIndex"
#define PERPORTY_TRANSFORM_IPMI_SUBCMD           "Subcmd"
#define METHOD_TRANSFORM_IPMI_BY_REMOTE_BUS      "MethodIpmiByRemoteBus"

#define CLASS_USBPORTPOWERMGMT_NAME        "USBPortPowerMgnt"
#define PROPERTY_PORTPOWERSTATUS_NAME      "PortPowerStatus"
#define OBJECT_NAME_NODE_INFO                      "NodeInfo-1"

#define CLASS_MGMT_CHIP_ENV_CONFIG          "MgmtChipEnvConfig"
#define PROPERTY_MGMT_CHIP_ENV_NAME         "EnvName"
#define PROPERTY_MGMT_CHIP_ENV_VALUE        "EnvValue"
#define PROPERTY_MGMT_CHIP_ENV_ACTION        "EnvEffectiveAction"
#define PROPERTY_BMC_CUSTOM_ALARM            "CustomAlarm" // 定制化告警
#define CLASS_FIRMWARE_LIMIT                     "FirmwareLimit"
#define PROPERTY_FIRMWARE_LIMIT_CHIP_TYPE        "ChipType"
#define PROPERTY_FIRMWARE_LIMIT_FIRMWARE_TYPE    "FirmwareType"
#define PROPERTY_FIRMWARE_LIMIT_CHIP_ID          "ChipID"
#define PROPERTY_FIRMWARE_LIMIT_FIRMWARE_REVISION    "RevisionNumber"
#define PROPERTY_FIRMWARE_LIMIT_REVISION_TYPE     "RevisionType"
#define METHOD_TRANSFORM_IPMI_BY_REMOTE_BUS      "MethodIpmiByRemoteBus"

#define PROPERTY_SPI_FLASH_ID   "SPIFlashId"
#define PROPERTY_PHY_ID         "PHYId"

#define CLASS_DFP                           "DiskFaultPrediction"
#define DFP_OBJECT_NAME                     "Dfp"
#define PROPERTY_DFP_ENABLE                 "DfpEnable"
#define PROPERTY_DFP_TIMESTAMP              "TimeStamp"
#define METHOD_DFP_SET_DFP_ENABLE           "SetDfpEnable"
#define METHOD_COLLECT_PCIE_FAULT_DATA      "CollectPcieData"
#define CLASS_CHIP_FT_CPU_TEMP              "FtCpuTemp"
#define PROPERTY_CHIP_REF_OBJ               "RefCpuObj"

#define CLASS_BIOS_FLASH_SWITCH             "BiosUpgradeSwitch"
#define OBJ_BIOS_UPGRADE_SWITCH             "BiosUpgradeSwitch"
#define PROPERTY_PROCESSOR_COUNT            "ProcessorCount"
#define PROPERTY_BIOS_FLASH_SWITCH          "BiosFlashSwitch"
#define PROPERTY_BIOS_FLASH_SWITCH_LOCK     "SwitchLock"
#define METHOD_SET_BIOS_FLASH_SWITCH        "SetBiosFlashSwitch"
#define METHOD_SET_BIOS_FLASH_SWITCH_LOCK   "SetSwitchLock"

#define CLASS_NAME_BUS_MANAGE               "BusManage"
#define PROPERTY_BUS_MANAGE_BUS             "Bus"
#define PROPERTY_BUS_MANAGE_ENABLE          "Enable"
#define PROPERTY_BUS_MANAGE_STATE           "State"
#define METHOD_BUS_MANAGE_SET_STATE         "SetState"
#define METHOD_BUS_MANAGE_SET_ENABLE        "SetEnable"

#define CLASS_NAME_CANBUS                   "Canbus"

#define CLASS_MCU_CHIP                 "MCUChip"
#define PROPERTY_MCU_CHIP_ADDR         "Addr"
#define PROPERTY_MCU_CHIP_ADDR_WIDTH   "AddrWidth"
#define PROPERTY_MCU_CHIP_OFFSET_WIDTH "OffsetWidth"
#define PROPERTY_MCU_CHIP_PREV_CHIP    "PrevChip"
#define PROPERTY_MCU_CHIP_INDEX        "Index"
#define PROPERTY_MCU_CHIP_LEVEL        "Level"
#define PROPERTY_MCU_CHIP_LBUS_TYPE    "LBusType"
#define PORPERTY_MCU_CHIP_RBUS_TYPE    "RBusType"
#define PROPERTY_MCU_CHIP_LBUS         "LBus"

#define CLASS_JTAG_NAME             "Jtag"
#define PROPERTY_TCK_GPIO_NUM       "TckGpioNum"
#define PROPERTY_TMS_GPIO_NUM       "TmsGpioNum"
#define PROPERTY_TDO_GPIO_NUM       "TdoGpioNum"
#define PROPERTY_TDI_GPIO_NUM       "TdiGpioNum"
#define PROPERTY_JTAG_CPLD_OFFSET   "JtagCpldOffset"
#define PROPERTY_JTPG_TCK_BIT       "jtpg_tck_bit"
#define PROPERTY_JTPG_TMS_BIT       "jtpg_tms_bit"
#define PROPERTY_JTPG_TDO_BIT       "jtpg_tdo_bit"
#define PROPERTY_JTPG_TDI_BIT       "jtpg_tdi_bit"

#define CLASS_NPU_ABILITY                   "NpuAbility"
#define PROPERTY_NPU_ABILITY_TYPE_ARRAY     "TypeArray"
#define PROPERTY_NPU_ABILITY_VALUE_ARRAY    "ValueArray"
#define METHOD_NPU_ABILITY_SET_NPU_ABILITY  "SetNpuAbility"

#define CLASS_SOC_BOARD "SoCBoard"
#define PROPERTY_SOC_BOARD_ID "Id"
#define PROPERTY_SOC_BOARD_TYPE "Type"
#define PROPERTY_SOC_BOARD_PCBVER "PcbVer"
#define PROPERTY_SOC_BOARD_BOARDID "BoardId"
#define PROPERTY_SOC_BOARD_MANUFACTURER "Manufacturer"
#define PROPERTY_SOC_BOARD_NAME "Name"
#define PROPERTY_SOC_BOARD_BOARDNAME "BoardName"
#define PROPERTY_SOC_BOARD_PARTNUM "PartNum"
#define PROPERTY_SOC_BOARD_BOMID "BomId"
#define PROPERTY_SOC_BOARD_HEALTH "Health"
#define PROPERTY_SOC_BOARD_FAULT_CODE "FaultCode"
#define PROPERTY_SOC_BOARD_FAULT_STATE "FaultState"
#define PROPERTY_SOC_BOARD_PWR_STATE "PowerState"
#define PROPERTY_SOC_BOARD_OOB_CHAN "OobChannel"
#define PROPERTY_SOC_BOARD_POWER "Power"
#define PROPERTY_SOC_BOARD_TEMP "ChipTemp"
#define PROPERTY_SOC_BOARD_MCU_VER "McuVersion"
#define PROPERTY_SOC_BOARD_MCU_SUPPORT "McuSupported"
#define PROPERTY_SOC_BOARD_SN "SerialNumber"
#define PROPERTY_SOC_BOARD_HOST_FAULT_CODE "HostFaultCode"
#define PROPERTY_SOC_BOARD_HOST_FAULT_STATE "HostFaultState"
#define PROPERTY_SOC_BOARD_DESC "Description"
#define PROPERTY_SOC_BOARD_SHARE_RAM_ADDR "SharedRamAddr"
#define PROPERTY_SOC_BOARD_SHARE_RAM_DATA "SharedRamData"
#define METHOD_SOC_BOARD_GET_DATA_FROM_MCU METHOD_PCIE_CARD_GET_DATA_FROM_MCU
#define METHOD_SOC_BOARD_SET_DATA_TO_MCU METHOD_PCIE_CARD_SET_DATA_TO_MCU
#define METHOD_SOC_BOARD_SET_HOST_FAULT_INFO "SetHostFaultInfo"
#define METHOD_SOC_BOARD_UPLODE_BOOT_CERT "UploadBootCert"
#define METHOD_SOC_BOARD_GET_BOOT_CERT_STATUS "GetBootCertStatus"
#define METHOD_SOC_BOARD_SET_SECUREBOOT "SetSecureBoot"
#define METHOD_MCU_GET_SECUREBOOT   "GetSecureBoot"

#define CLASS_OOB_CHAN "OobChannel"
#define PROPERTY_OOB_CHAN_PROTOCOL "Protocol"
#define PROPERTY_OOB_CHAN_REFCHIP "RefChip"
#define PROPERTY_OOB_CHAN_MAXFRAMELEN "MaxFrameLen"
#define PROPERTY_OOB_CHAN_REFCONN "RefConnector"

#define CLASS_SAS_EXPBOARD_NAME             "SASExpBoard"
#define PROPERTY_SAS_EXPBOARD_SLOTID        "Slot"
#define PROPERTY_SAS_EXPBOARD_PRESENCE      "Presence"
#define PROPERTY_SAS_EXPBOARD_BOARDID       "BoardId"
#define PROPERTY_SAS_EXPBOARD_LOCALLED      "Location"
#define PROPERTY_SAS_EXPBOARD_ALARMLED      "Fault"
#define PROPERTY_SAS_EXPBOARD_CHANNEL       "SASExpChannel"
#define PROPERTY_SAS_EXPBOARD_PG            "PowerGoodState"
#define PROPERTY_SAS_EXPBOARD_SETPOWER      "PowerStatus"
#define PROPERTY_SAS_EXPBOARD_HEALTH        "Health"
#define PROPERTY_SAS_EXPBOARD_LED_FLAG      "AlarmLedFlag"
#define PROPERTY_SAS_EXPBOARD_5V_PG         "DisksPowerGood"
#define PROPERTY_SAS_EXPBOARD_3V3_PG        "DisksBoardPG"
#define PROPERTY_SAS_EXPBOARD_MAXTEMP       "MaxTemp"
#define PROPERTY_SAS_EXPBOARD_CORECHIP      "CoreChip"
#define PROPERTY_SAS_EXPBOARD_CORETEMP      "CoreTemp"
#define PROPERTY_SAS_EXPBOARD_CORESTATUS    "CoreStatus"
#define PROPERTY_SAS_EXPBOARD_CORE_CHIP_PG  "CoreChipPowerState"
#define PROPERTY_SAS_EXPBOARD_SYNC_PG       "SyncPowerGoodState"
#define PROPERTY_SAS_EXPBOARD_SYNC_5V_PG    "SyncDisksPowerGood"
#define PROPERTY_SAS_EXPBOARD_SYNC_3V3_PG   "SyncDisksBoardPG"
#define PROPERTY_SAS_EXPBOARD_SYNC_MAXTEMP  "SyncMaxTemp"
#define PROPERTY_SAS_EXPBOARD_I2C           "I2CStatus"
#define PROPERTY_SAS_EXPBOARD_SYNC_I2C      "SyncI2CStatus"
#define METHOD_SAS_EXPBOARD_SYNC            "SASExpPropertySync"
#define PROPERTY_SAS_EXPBOARD_PCBVER        "PcbVer"
#define PROPERTY_SAS_EXPBOARD_LOGICUNIT     "LogicUnit"
#define PROPERTY_SAS_EXPBOARD_LOGICVERID    "LogicVerId"
#define PROPERTY_SAS_EXPBOARD_LOGICVER      "LogicVer"
#define METHOD_SAS_EXPBOARD_PCIE_FAULT      "SASExpSetPcieFault"
#define PROPERTY_SAS_EXPBOARD_ALARMSTATE    "AlarmState"
#define PROPERTY_SAS_EXPBOARD_BDF           "BDF"
#define PROPERTY_SAS_EXPBOARD_CHIP_REST     "ChipReset"

#define CLASS_AR_CARD_NAME                 "ARCard"
#define PROPERTY_AR_CARD_SLOT              "Slot"
#define PROPERTY_AR_CARD_POWERON_STATE     "PowerOnState"
#define PROPERTY_AR_CARD_RESET_MODE        "ResetMode"
#define PROPERTY_AR_CARD_RESET_STATE       "ResetState"
#define METHOD_ARCARD_ACTION_ACCESSOR      "ARCardActionAccessor"
#define METHOD_ARCARD_PROPERTY_SYNC        "PropertySyncMethod"
#define METHOD_ARCARD_HW_PROPERTY_SYNC     "PropertyHwSyncMethod"
#define METHOD_ARCARD_CONTROL              "ARCardControl"
#define PROPERTY_ARCARD_REFCOMPONENT       "RefComponent"
#define PROPERTY_ARCARD_ID                  "Id"
#define PROPERTY_ARCARD_ID_STRG_MD          "IdStorageMedia"
#define PROPERTY_OOB_CHAN_REFCONN "RefConnector"

#define CLASS_HOSTMU "HostMU"
#define PROPERTY_HOSTMU_ID "Id"
#define PROPERTY_HOSTMU_OOB_CHAN "OobChannel"
#define PROPERTY_HOSTMU_STATUS "Status"
#define PROPERTY_HOSTMU_BOOT_STAGE "BootStage"
#define METHOD_HOSTMU_GET_SYSTEM_TIME  "GetSystemTime"
#define METHOD_HOSTMU_GET_CERT_INFO    "GetCertInfo"


#define CLASS_MCU_CHIP                 "MCUChip"
#define PROPERTY_MCU_CHIP_ADDR         "Addr"
#define PROPERTY_MCU_CHIP_ADDR_WIDTH   "AddrWidth"
#define PROPERTY_MCU_CHIP_OFFSET_WIDTH "OffsetWidth"
#define PROPERTY_MCU_CHIP_PREV_CHIP    "PrevChip"
#define PROPERTY_MCU_CHIP_INDEX        "Index"
#define PROPERTY_MCU_CHIP_LEVEL        "Level"
#define PROPERTY_MCU_CHIP_LBUS_TYPE    "LBusType"
#define PORPERTY_MCU_CHIP_RBUS_TYPE    "RBusType"
#define PROPERTY_MCU_CHIP_LBUS         "LBus"

/* VirtualI2cIP2的对象及属性 */
#define CLASS_VIRTUALI2C                                "VirtualI2c"
#define PROPERTY_CPLD_CORE_REPAIR                       "CpldCoreRepair"
#define PROPERTY_VIRTUALI2CIP2_TIMEOUTSET               "TimeoutSet"
#define METHOD_SET_TIME_OUT_VALUE                       "SetTimeOutValue"
#define METHOD_RESET_CPLD_CORE                          "ResetCpldCore"

/* PCIE共享卡类 */
#define CLASS_PANGEA_PCIE_CARD               "PangeaPcieCard"
#define PROPERTY_PANGEA_PCIE_CARD_SLOT       "PcieCardSlot"
#define PROPERTY_PANGEA_PCIE_MCUVER          "MCUVer"
#define PROPERTY_PANGEA_CARD_MCUUPGRADESTATE "MCUUpgradeStatus"
#define PROPERTY_PANGEA_CARD_MCUTEMP         "MCUTemp"
#define PROPERTY_PANGEA_CARD_LOGICTYPE       "LogicType"
#define PROPERTY_PANGEA_CARD_WORKMODE        "Workmode"
#define PROPERTY_PANGEA_CARD_LINKHOST        "PCIeLinkHostCnt"
#define PROPERTY_PANGEA_CARD_PDATA           "ProductConfData"
#define PROPERTY_PANGEA_CARD_EXDATA          "ExtentConfData"
#define PROPERTY_PANGEA_CARD_CHIPTEMP        "ChipTemp"
#define PROPERTY_PANGEA_CARD_SFPTEMP         "SfpTemp"
#define PROPERTY_PANGEA_CARD_REF_COMPONENT   "NetCardComponent"
#define PROPERTY_MCU_CHIP                   "McuChip"
#define PROPERTY_IOCARD_INSERTOK            "InsertOK"
#define PROPERTY_IOCARD_INSERTOK_WP         "InsertOkWp"
#define PROPERTY_MCU_CHIP_ADDR              "Addr"
#define PROPERTY_PANGEA_CARD_PG_STATUS      "CardPGStatus"
#define METHOD_UPDATE_MCU_VER               "UpdateMCUVer"
#define METHOD_SET_MCU_ELABLE               "PangeaCardSetElable"
#define METHOD_GET_MCU_ELABLE               "PangeaCardGetElable"
#define METHOD_SET_ALARM_LED_STATE          "SetPcieAlarmLedState"
#define METHOD_GET_ALARM_LED_STATE          "GetPcieAlarmLedState"
#define METHOD_READ_DATA_FROM_MCU           "ReadDataFromMCU"

#define CLASS_SLOT_REPAIRATION         "SlotIdRepair"
#define LOCAL_CTRL_SLOT_REPAIR         "LocalCtrlSlotIdRepair"
#define PROPERTY_LOCAL_CTRL            "LocalCtrl"
#define PROPERTY_CTRL_SLOT_ID          "SlotId"
#define PROPERTY_REPAIRED_SLOT_ID      "RepairedSlotId"
#define PROPERTY_SLOT_ID_VALIDATE      "ValidSlotId"
#define PROPERTY_CHASSISPRESENT_CTRL   "ChassisPresentCtrl"

#define CLASS_FRU_WARNING                      "FruWarning"
#define PROPERTY_FRU_WARNING_TYPE              "FruType"
#define PROPERTY_FRU_WARNING_HEALTH            "HealthStatus"
#define PROPERTY_FRU_WARNING_COMP_TYPE         "ComponentTypeList"
#define PROPERTY_FRU_ALARM_TYPE                "AlarmType"
#define PROPERTY_FRU_CHASSIS_HEALTH_STATUS     "ChassisHealthStatus"
#define METHOD_FRU_WARNING_GET_HEALTH          "GetHealthStatus"
#define METHOD_FRU_WARNING_SYNC_CHASSIS_HEALTH "SyncChassisHealthStatus"

#define CLASS_DEVICE_FAULT_INFO                "DeviceFaultInfoAdpt"
#define OBJ_DEVICE_HDD_DISK_FRONT              "HddDiskFront"
#define OBJ_DEVICE_HDD_DISK_REAR               "HddDiskRear"
#define OBJ_DEVICE_PALM_DISK                   "PalmDisk"
#define PERPORTY_DEVICE_FAULT_TYPE             "AlarmType"
#define PROPERTY_DEVICE_HEALTH                 "Health"

#define CLASS_CTRL_AUXILIARY_CPLD_VER                    "CtrlAuxiliaryCpldVer"
#define PROPERTY_CTRL_AUXILIARY_CPLD_REG                 "CpldVerReg"

#define CLASS_NAME_STORAGE_ARB                 "StorageArbitration"
#define CLASS_OBJECT_STORAGE_ARB               "StorageArb"
#define PROPERTY_ARB_FORCE_RESTART_ENABLED     "ForceRestartEnabled"
#define PROPERTY_ARB_POWERINGOFF_STATUS        "PoweringOffStatus"
#define PROPERTY_ARB_FORCE_RESTART_OS_RESULT   "ForceRestartOsResult"
#define PROPERTY_ARB_FORCE_RESTART_OS_PROTECT  "ForceRestartOsProtect"
#define PROPERTY_ARB_FORCE_RESTART_OS             "ForceRestartOs"
#define METHOD_ARB_SET_FORCE_RESTART_OS        "SetForceRestartOs"
#define METHOD_ARB_GET_FORCE_RESTART_OS_RESULT "GetForceRestartOsResult"
#define METHOD_ARB_FORCE_CPU_POWER_DOWN        "ForceCpuPowerDown"


/* 数据修复(同步方式)类 */
#define CLASS_DATA_REPAIR                   "DataRepair"
#define DATA_REPAIR_PROP_ORI_REG0           "OriginReg0"
#define DATA_REPAIR_PROP_ORI_REG1           "OriginReg1"
#define DATA_REPAIR_PROP_ORI_REG2           "OriginReg2"
#define DATA_REPAIR_PROP_ORI_REG3           "OriginReg3"
#define DATA_REPAIR_PROP_FIX_REG0           "RepairReg0"
#define DATA_REPAIR_PROP_FIX_REG1           "RepairReg1"
#define DATA_REPAIR_PROP_FIX_REG2           "RepairReg2"
#define DATA_REPAIR_PROP_FIX_REG3           "RepairReg3"
#define DATA_REPAIR_PROP_MASK               "RepairMask"
#define DATA_REPAIR_PROP_ALGORITHM          "VerifyMethod"
#define DATA_REPAIR_PROP_STATUS             "RepairStatus"
#define OBJECT_BACK_PLANE_ID_REPAIR         "BackIdRepairObj"
#define METHOD_DATA_REPAIR_RECV_DATA        "ReceiveRepairData"

#define CLASS_SHARED_FRU_MONITOR            "SharedFruMonitor"
#define SFM_PROP_DEV_NUM                    "DeviceNum"
#define SFM_PROP_DEV_TYPE                   "DeviceType"
#define SFM_PROP_TRIGGER                    "DetectTrigger"
#define SFM_PROP_ROUTINE_DIS                "RoutineDisabled"
#define SFM_PROP_ROUTINE_DIS_DELAY          "RoutineStopDelay"
#define SFM_PROP_NOTICE_TYPE                "NoticeType"
#define SFM_PROP_NOTICE_CLASS               "NoticeClass"   /* 被通知的类 */
#define SFM_PROP_NOTICE_METHOD              "NoticeMethod"
#define SFM_PROP_DETECT_CHIP                "DetectChip"
#define SFM_PROP_ACCESS_OFFSET              "AccessOffset"
#define SFM_PROP_ACCESS_LENGTH              "AccessLength"
#define SFM_PROP_NATIVE_STATE               "NativeErrorState"
#define SFM_PROP_FINAL_STATE                "FinalErrorState"
#define SFM_PROP_DETAIL_STATE               "AllErrorState"
#define SFM_PROP_DETECT_PHASE               "DetectPhase"
#define METHOD_SFM_SMM_REMOTE_DETECT        "SmmRemoteDetect"
#define METHOD_SFM_CANBUS_MSG_RECV_CALLBACK "CanbusMsgRecvCallback"
#define METHOD_SFM_SMM_SET_FINAL_RESULT     "SmmSetFinalResult"

#define CLASS_SHARED_FRU_MANAGER            "SharedFruManager"
#define SFMNG_PROP_SLOT_ID                  "BoardSlotId"
#define SFMNG_PROP_SLOT_START               "BoardSlotIdStart"
#define SFMNG_PROP_QUIT_ACTIVE_FORBID       "ActiveQuitForbid"
#define SFMNG_PROP_MANAGE_STATE             "ManageState"

#define CLASS_UPGRADE_FILENAME_MAP      "UpgradeFileNameMapping"
#define PROPERTY_UG_FM_COM_ID           "ComponentId"
#define PROPERTY_UG_FM_DEV_ID           "ChipDeviceId"
#define PROPERTY_UG_FM_BRD_ID           "BoardId"
#define PROPERTY_UG_FM_CHIP_NUM         "ChipNumber"
#define PROPERTY_UG_FM_PRI_FW_N         "PrimaryFWName"
#define PROPERTY_UG_FM_SEC_FW_N         "SecondaryFWName"

#define CLASS_PANGE_BOARD_ADATPTER_TABLE "PangeaBoardAdapterTable"
#define PROPERTY_BOARD_ADAPTER_TYPE "AdapterType"
#define PROPERTY_BOARD_ADAPTER_FRU_ID "AdapterFruId"
#define PROPERTY_BOARD_ADAPTER_OBJ_ARRAY "AdapterObjArray"
#define PANGE_BOARD_ADAPTER_TYPE_ARRAY "AdapterTypeArray"

/* 背板eeprom组 */
#define CLASS_EEPROM_STORAGE_GROUP_NAME                     "GroupEepStorage"
#define PROPERTY_EEP_GROUP_ID                               "EepGroupId"
#define PROPERTY_EEP_GROUP_OBJ_ARRAY                        "EepObjArray"
#define PROPERTY_EEP_GROUP_OBJ_COUNT                        "EepObjCount"
#define PROPERTY_EEP_GROUP_DATA_SYNC                        "EepDataSync"
#define PROPERTY_EEP_GROUP_OFFSET                           "EepOffset"
#define PROPERTY_EEP_GROUP_DATA_LENGTH                      "DataLength"
#define PROPERTY_EEP_GROUP_VALUE                            "EepValue"
#define PROPERTY_EEP_GROUP_CLEAR_FLAG                       "ClearFlag"
#define PROPERTY_EEP_GROUP_HAS_VERSION                      "EepHasVersion"
#define PROPERTY_EEP_GROUP_DATA_VER                         "DataVersion"
#define PROPERTY_EEP_GROUP_CPLD_ACCESSOR                    "AccessorObj"
#define PROPERTY_EEP_GROUP_INDEX_VALUE                      "EepValueIndex"
#define PROPERTY_EEP_GROUP_KEYINFO_FLAG                     "KeyInfoFlag"
#define METHOD_BACKPLANE_PROPERTY_SYNC                      "SysBackPlanePropertySync"
#define METHOD_SET_EEP_STORAGE_VALUE                        "SetEepromStorageValue"
#define METHOD_GET_EEP_STORAGE_VALUE                        "GetEepromStorageValue"
#define METHOD_KEY_PROCUDT_INFO_INIT                        "KeyProcuctInfoInit"

/* 背板eeprom数据存储类 */
#define CLASS_EEPROM_STORAGE_NAME                           "EepromStorage"
#define PROPERTY_EEP_STORAGE_EEP_STORAGE_LOC                "EepStorageLoc"
#define PROPERTY_EEP_STORAGE_EEPROM_WP                      "EepromWp"

#define PROPERTY_EEPROM_WRITECOUNT                          "WriteCount"

/* 字典树存储类 */
#define CLASS_PREFIX_TREE_NODE                              "PrefixTreeNode"
#define PROPERTY_PREFIX_TREE_TYPE                           "TreeType"
#define PROPERTY_PREFIX_TREE_INDEX                          "Index"
#define PROPERTY_PREFIX_TREE_DATA                           "Data"

#define CLASS_SHELF_PAYLOAD                          "ShelfPayload"
#define PROTERY_SHELF_POWER_STATE                    "ShelfPowerState"
#define PROTERY_SHELF_PAYLOAD_PROCESS                "ShelfPayloadProcess"
#define PROTERY_SHELF_IS_POWEROFFING                 "ShelfIsPowerOffing"
#define PROTERY_SHELF_POWERON_CONDITIONS             "PowerOnConditions"
#define PROTERY_SHELF_SHORT_BUT_EVT                  "ShelfShortButtonEvt"
#define PROTERY_SHELF_SHORT_BUT_CLR                  "ShelfShortButtonClear"
#define PROTERY_SHELF_LONG_BUT_EVT                   "ShelfLongButtonEvt"
#define PROTERY_SHELF_LONG_BUT_CLR                   "ShelfLongButtonClear"
#define PROTERY_SHELF_AC_DOWN_EVT                    "ShelfAcDownEvt"
#define PROTERY_SHELF_AC_DOWN_FLAG                   "ShelfAcDownFlag"
#define PROTERY_SHELF_POWER_BUTTON_MODE              "ShelfPowerButtonMode"
#define METHOD_SHELF_POWER_CONTROL                   "ShelfPowerControl"
#define METHOD_SHELF_PAYLOAD_STATE_SYNC              "ShelfPayloadStateSync"
#define METHOD_SHELF_PAYLOAD_SAVE_SYNC               "ShelfPayloadSaveSync"
#define METHOD_SHELF_PAYLOAD_SET_BUTTON_MODE         "SetButtonMode"

#define CLASS_IPMBETH_STORAGE                        "IpmbEthStorage"
#define PROPERTY_IPMBETH_STORAGE_SLOT_ID             "SlotId"
#define PROPERTY_IPMBETH_STORAGE_SLAVE_ADDR          "SlaveAddr"
#define PROPERTY_IPMBETH_STORAGE_PRESENCE            "Presence"
#define PROPERTY_IPMBETH_STORAGE_CPLD_STATUS         "CpldStatus"
#define PROPERTY_IPMBETH_STORAGE_UCE_STATUS          "UceStatus"

/* CPLD热升级关键寄存器记录类 */
#define OBJNAME_CPLD_UPGRADE_KEYGEG             "UpgradeRegCollect"
#define PROPERTY_UPGRADE_CPLD_KEYGEG            "KeyReg"                /* CPLD升级数据回填寄存器 */
#define PROPERTY_UPGRADE_CPLD_KEYWPGEG          "KeyWPReg"              /* CPLD升级数据回填有写保护的寄存器 */
#define PROPERTY_UPGRADE_CPLD_KEYGEG_COUNT      "KeyRegCount"           /* CPLD升级数据回填寄存器个数 */
#define PROPERTY_UPGRADE_CPLD_KEYWPGEG_COUNT    "KeyWPRegCount"         /* CPLD升级数据回填有写保护的寄存器个数 */

#define EVENT_REPORT_FILTER_CLASS_NAME              "EventReportFilter"
#define EVENT_REPORT_FILTER_PROPERTY_CONDITION      "Condition"
#define EVENT_REPORT_FILTER_PROPERTY_COMPONENTS     "Components"
#define EVENT_REPORT_FILTER_PROPERTY_COMP_CHANNELS  "ComponentChannels"
#define EVENT_REPORT_FILTER_PROPERTY_EVENT_CODES    "EventCodes"
#define EVENT_REPORT_FILTER_PROPERTY_EVENT_CHANNELS "EventChannels"

/* pangea 升级类 */
#define CLASS_PANGEA_UPGRADE                    "PangeaUpgrade"
#define METHOD_PANGEA_GET_UPGRADE_OBJS          "GetUpgradeObjects"

/* Pangea接口卡类 */
#define CLASS_PANGEA_CARD                           "PangeaCard"
#define PROPERTY_PANGEA_CARD_FRUID                  "FruId"
#define PROPERTY_PANGEA_CARD_SLOTID                 "SlotId"
#define PROPERTY_PANGEA_CARD_PRESENT                "Present"
#define PROPERTY_PANGEA_CARD_INSERTOK               "InsertOK"
#define PROPERTY_PANGEA_CARD_INSERTOKWP             "InsertOKWp"
#define PROPERTY_PANGEA_CARD_PSTCHANGE              "PstChange"
#define PROPERTY_PANGEA_CARD_PCBVER                 "PCBVer"
#define PROPERTY_PANGEA_CARD_PCB                    "PCB"
#define PROPERTY_PANGEA_CARD_TYPE                   "Type"
#define PROPERTY_PANGEA_CARD_QOSINFO                "QosInfo"
#define PROPERTY_PANGEA_CARD_CORETEMPERATURE        "CoreTemperature"
#define PROPERTY_PANGEA_CARD_OPTICALTEMPERATURE     "OpticalTemperature"
#define PROPERTY_PANGEA_CARD_EINSSTART              "EInsStart"
#define PROPERTY_PANGEA_CARD_EINSDURATION           "EInsDuration"
#define PROPERTY_PANGEA_CARD_POWERGOOD              "PowerGood"
#define PROPERTY_PANGEA_CARD_POWERON                "PowerOn"
#define PROPERTY_PANGEA_CARD_HWDESC                 "HwDesc"
#define PROPERTY_PANGEA_CARD_RESEEPROM              "ResEeprom"
#define PROPERTY_PANGEA_CARD_EEPROMWP               "EepromWp"
#define PROPERTY_PANGEA_CARD_TYPEERR                "TypeErr"
#define PROPERTY_PANGEA_CARD_RES9555                "Res9555"
#define PROPERTY_PANGEA_CARD_CONNECTOR              "Connector"

#define CLASS_CPLD_SPACE_TEST                       "CpldSpaceTest"
#define PROPERTY_CPLD_SPACE_CPLD_INDEX              "CpldIndex"
#define PROPERTY_CPLD_SPACE_MANUFACTURER            "Manufacturer"
#define PROPERTY_CPLD_SPACE_PRI_FW_NAME             "PrimaryFileFWName"
#define PROPERTY_CPLD_SPACE_RESULT_GPIO             "ResultGPIO"
#define PROPERTY_CPLD_SPACE_SEC_FW_NAME             "SecondaryFWNmae"
#define PROPERTY_CPLD_SPACE_TEST_RESULT             "TestResult"
#define PROPERTY_CPLD_SPACE_TRIGGER_GPIO            "TriggerGPIO"
#define PROPERTY_CPLD_SPACE_WAIT_TIME               "WaitTime"

#define CLASS_DETECT_HARD_WARE                 "DetectHardWare"
#define PROPERTY_DETECT_HARD_REFACCESSOR       "RefAccessor"
#define PROPERTY_DETECT_HARD_COUNT             "Count"
#define PROPERTY_DETECT_HARD_DETECT_RESULT     "DetectResult"
#define PROPERTY_DETECT_HARD_STATUS            "Status"
#define PROPERTY_DETECT_HARD_DATA              "Data"

#define CLASS_DETECT_HARD_WARE                 "DetectHardWare"
#define PROPERTY_DETECT_HARD_REFACCESSOR       "RefAccessor"
#define PROPERTY_DETECT_HARD_COUNT             "Count"
#define PROPERTY_DETECT_HARD_DETECT_RESULT     "DetectResult"
#define PROPERTY_DETECT_HARD_STATUS            "Status"
#define PROPERTY_DETECT_HARD_DATA              "Data"

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __PME_PROPERTY_METHOD_APP_H__ */
