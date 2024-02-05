

#ifndef __CLP_CONFIG_H__
#define __CLP_CONFIG_H__

#include "set_clp_lom.h"
#include "set_clp_310_312_912.h"
#include "set_clp_220_221.h"
#include "set_clp_520_522.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

#define RFPROP_CLP_VLAN_MODE "VlanMode"
#define RFPROP_CLP_VLANID "VlanId"
#define RFPROP_CLP_VLAN_PRIORITY "VlanPriority"
#define RFPROP_CLP_VLAN_TRUNKRANGE "VlanTrunkRange"
#define RFPROP_CLP_BOOT_ENABLED "BootEnabled"
#define RFPROP_CLP_LINK_CONFIG "LinkConfig"
#define RFPROP_CLP_SERDES_CONFIG "SerDesConfig"


#define PROPERTY_MISSING 0xA1
#define BOARD_UNSUPPORTED 0xA2
#define PORT_PF_TARGET_EXCEED 0xA3
#define PROPERTY_VALUE_ERROR 0xA4
#define PROPERTY_VALUE_NOT_INLIST 0xA5
#define MODE_UNSUPPORTED 0xA6
#define BANDWITH_ERROR 0xA7
#define VLANID_CONFLICT 0xA8
#define TRUNK_RANGE_CHECK_FAIL 0xAA
#define PROPERTY_UNKNOW 0xAB

#define CLP_KEY_WWNN "WWNN"
#define CLP_KEY_WWPN "WWPN"
#define CLP_KEY_FCOE_WWNN "FCoEWWNN"
#define CLP_KEY_FCOE_WWPN "FCoEWWPN"

#define REGEX_TRUNK_RANGE "^([0-9]+(,[0-9]+)*)(-([0-9]+(,[0-9]+)*)|[0-9])*$"
#define REGEX_MAC "^([a-fA-F0-9]){12}$"
#define REGEX_WWPN_WWNN "^([a-fA-F0-9]){16}$"

#define MAX_FCOE_COUNT 1
#define MAX_ISCSI_COUNT 2
#define MAX_PROP_LEN_16 16
#define MAX_PROP_LEN 12
#define MIN_PROP_LEN 0

#define RFPROP_CLP_CONFIG "Configuration"
#define RFPROP_CLP_EFFECTIVE "Effective"
#define RFPROP_CLP_SRIOVENABLED "SRIOVEnabled"
#define RFPROP_CLP_MULTI_MODE "MultifunctionMode"
#define RFPROP_CLP_PORTS_CONFIG "PortsConfig"
#define RFPROP_CLP_BOOTTOTARGET "BootToTarget"
#define RFPROP_CLP_PROTOCOL "Protocol"
#define RFPROP_CLP_MIN_BAND "MinBandwidth"
#define RFPROP_CLP_MAX_BAND "MaxBandwidth"


#define MHFB_ID 157
#define MHFB_VID_DID ("1077:2261")


#define MHFA_ID 149
#define MHFA_VID_DID ("10DF:E200")
#define MHFA_VID_DID_FCOE ("10DF:E220")


#define MXEP ("152")
#define MXEP_VID_DID ("14E4:168E")
#define MXEP_ID 152
// NPAR 模式下的vid did会改变
#define MXEP_VID_DID_NPAR ("14E4:16AE")
#define MXEP_PORT_NUM 4
#define MXEP_VID 0x14E4
#define MXEP_DID 0x168E
#define MXEP_VID_NPAR 0x14E4
#define MXEP_DID_NPAR 0x16AE


#define MXEN ("154")
#define MXEN_VID_DID ("14E4:168E")
#define MXEN_ID 154
#define MXEN_PORT_NUM 2


#define MXEM ("153")
#define MXEM_VID_DID ("8086:10F8")
#define MXEM_ID 153
#define MXEM_PORT_NUM 2


#define MXEK ("150")
#define MXEK_VID_DID ("8086:10F8")
#define MXEK_ID 150
#define MXEK_PORT_NUM 4


#define MXEL ("151")
#define MXEL_ID 151
#define MXEL_ETH_PORT_NUM 2

#define MALLOC_ERR 2

#define CLP_220_221 0


#define MAX_PROPERTY_PATH_LEN 512


#define INVALID_UCHAR 0xFF
#define INVALID_INT_VAL (-1)
#define INVALID_STR_VAL ""
#define INVALID_UINT_VAL 0xFFFFFFFF
#define MAX_UINT16_VAL 0xFFFF

#define MIN_VLAN_ID 0
#define MAX_VLAN_ID 4094

#define MIN_VLAN_PRIORITY 0
#define MAX_VLAN_PRIORITY 7

#define MIN_PF_ID 0
#define MAX_PF_ID 7

#define MIN_BAND 0
#define MAX_BAND 100

#define MIN_TARGET_ID 1
#define MAX_TARGET_ID 8

#define MIN_LUN_NUM 0
#define MAX_LUN_NUM 255

#define CLP_DEFAULT_SHOW \
    "show netport1 MultifunctionMode\r\n\
show netport1 SRIOV\r\n\
show netport1 BootProtocol\r\n\
show netport1 BootEnable\r\n\
show netport1 VlanID\r\n\
show netport1 PermanentAddress\r\n\
show netport1 iScsiAddress\r\n\
show netport1 FCoEFipMACAddress\r\n\
show netport1 FCoEWWPN\r\n\
show netport1 FCoEWWNN\r\n\
show netport1 FCoEBootToTarget\r\n\
show netport1 FCoEBootTarget1\r\n\
show netport1 FCoEBootTarget2\r\n\
show netport1 FCoEBootTarget3\r\n\
show netport1 FCoEBootTarget4\r\n\
show netport1 FCoEBootTarget5\r\n\
show netport1 FCoEBootTarget6\r\n\
show netport1 FCoEBootTarget7\r\n\
show netport1 FCoEBootTarget8\r\n"


#define CLP_PORT0_SHOW \
    "\
show netport1 NparPF0\r\n\
show netport1 NparPF2\r\n\
show netport1 NparPF4\r\n\
show netport1 NparPF6\r\n\
show pf0 QinqVlanMode\r\n\
show pf0 QinqVlanId\r\n\
show pf0 QinqVlanPriority\r\n\
show pf0 QinqVlanTrunkRange\r\n\
show pf2 QinqVlanMode\r\n\
show pf2 QinqVlanId\r\n\
show pf2 QinqVlanPriority\r\n\
show pf2 QinqVlanTrunkRange\r\n\
show pf4 QinqVlanMode\r\n\
show pf4 QinqVlanId\r\n\
show pf4 QinqVlanPriority\r\n\
show pf4 QinqVlanTrunkRange\r\n\
show pf6 QinqVlanMode\r\n\
show pf6 QinqVlanId\r\n\
show pf6 QinqVlanPriority\r\n\
show pf6 QinqVlanTrunkRange\r\n"

#define CLP_PORT1_SHOW \
    "\
show netport1 NparPF1\r\n\
show netport1 NparPF3\r\n\
show netport1 NparPF5\r\n\
show netport1 NparPF7\r\n\
show pf1 QinqVlanMode\r\n\
show pf1 QinqVlanId\r\n\
show pf1 QinqVlanPriority\r\n\
show pf1 QinqVlanTrunkRange\r\n\
show pf3 QinqVlanMode\r\n\
show pf3 QinqVlanId\r\n\
show pf3 QinqVlanPriority\r\n\
show pf3 QinqVlanTrunkRange\r\n\
show pf5 QinqVlanMode\r\n\
show pf5 QinqVlanId\r\n\
show pf5 QinqVlanPriority\r\n\
show pf5 QinqVlanTrunkRange\r\n\
show pf7 QinqVlanMode\r\n\
show pf7 QinqVlanId\r\n\
show pf7 QinqVlanPriority\r\n\
show pf7 QinqVlanTrunkRange\r\n"

#define CLP_HEAD_MEZZ "#mezz"
#define CLP_HEAD_BOARD_ID "#board_id"
#define CLP_HEAD_NETCARD "#netcard"

#define CLP_KEY_STATUS "status"
#define CLP_KEY_NPARPF "NparPF"
#define CLP_KEY_BOOT_TARGET "FCoEBootToTarget"
#define CLP_KEY_MULTI_MODE "MultifunctionMode"
#define CLP_KEY_SRIOV "SRIOV"
#define CLP_KEY_BOOT_ENABLE "BootEnable"
#define CLP_KEY_BOOT_PROTOCOL "BootProtocol"
#define CLP_KEY_VLANID "VlanID"
#define CLP_KEY_QINQ_VLAN_MODE "QinqVlanMode"
#define CLP_KEY_QINQ_VLANID "QinqVlanID"
#define CLP_KEY_QINQ_VLAN_PRIORITY "QinqVlanPriority"
#define CLP_KEY_QINQ_VLAN_TRUNKRANGE "QinqVlanTrunkRange"
#define CLP_KEY_BOOT_TARGET1 "FCoEBootTarget1"
#define CLP_KEY_BOOT_TARGET2 "FCoEBootTarget2"
#define CLP_KEY_BOOT_TARGET3 "FCoEBootTarget3"
#define CLP_KEY_BOOT_TARGET4 "FCoEBootTarget4"
#define CLP_KEY_BOOT_TARGET5 "FCoEBootTarget5"
#define CLP_KEY_BOOT_TARGET6 "FCoEBootTarget6"
#define CLP_KEY_BOOT_TARGET7 "FCoEBootTarget7"
#define CLP_KEY_BOOT_TARGET8 "FCoEBootTarget8"
#define CLP_KEY_ETH_MAC "PermanentAddress"
#define CLP_KEY_ISCSI_MAC "iSCSIAddress"
#define CLP_KEY_FCOE_MAC "FCoEFipMACAddress"

#define CLP_KEY_QINQ_PRIORITY "QinqPriority"
#define CLP_KEY_VLAN "Vlan"
#define CLP_PROP_DATA "data"
#define CLP_PROP_RETDATA "retdata"
#define CLP_PROP_PERMANENTADDRESS "permanentaddress"

#define CLP_VAL_NULL "null"
#define CLP_VAL_ENABLED "ENABLED"
#define CLP_VAL_DISABLED "DISABLED"
#define CLP_VAL_ETH "ETHERNET"
#define CLP_VAL_PXE "PXE"
#define CLP_VAL_ISCSI "iSCSI"
#define CLP_VAL_FCOE "FCoE"
#define CLP_VAL_NONE "NONE"
#define CLP_VAL_SF "SF"
#define CLP_VAL_NPAR "NPAR"
#define CLP_VAL_ENABLED_SM "Enable"
#define CLP_VAL_DISABLED_SM "Disable"

#define CLP_CMD_SHOW "show"
#define CLP_CMD_SHOW_PF "pf"
#define CLP_CMD_SET "set"

#define RFPROP_CLP_TARGET_ENABLED "TargetEnabled"
#define RFPROP_CLP_TARGET_LUN "BootLun"
#define RFPROP_CLP_CHIP_ID "ChipId"
#define RFPROP_CLP_PORT_ID "PortId"
#define RFPROP_CLP_PF_ID "PFId"
#define RFPROP_CLP_PFS_INFO "PFsInfo"
#define RFPROP_CLP_TARGET_ID "TargetId"

#define RFPROP_CLP_BOOT_TARGETS "BootToTargets"
#define RFPROP_CLP_BOOT_TO_TARGETS "FCoEBootToTargets"



#define CLP_KEY_OEM_BOOT_ENABLED "OEM_BootEnable"
#define CLP_KEY_NETWORK_ADDR "NetworkAddresses"
#define CLP_KEY_OEM_WWPN "OEM_TargetWWPN"
#define CLP_KEY_OEM_LUN "OEM_TargetLUN"
#define RFPROP_CLP_SAN_BOOT_ENABLE "SANBootEnabled"

#define CLP_HEAD_LOM "#lom"
#define CLP_HEAD_FC_FCOE "#fc/fcoe"
#define CLP_HEAD_CMD "#CMD"
#define CLP_KEY_OEMELX_LINK_CONFIG "OEMELX_LinkConfig"
#define CLP_KEY_OEMELX_SERDES "OEMELX_hssregs"
#define CLP_KEY_OEMELX_BOOT_ENABLED "OEMELX_BootEnable"
#define CLP_KEY_OEMELX_WWNN "OEMELX_VolatileWWNN"
#define CLP_KEY_OEMELX_WWPN "OEMELX_VolatileWWPN"
#define CLP_KEY_OEMELX_TARGET_WWPN "OEMELX_TargetWWPN"
#define CLP_KEY_OEMELX_LUN "OEMELX_TargetLUN"

#define ERROR_PROP "Property"
#define ERROR_VAL "Value"
#define ERROR_REL_PROP "RelatedProperty"
#define ERROR_REL_VAL "RelatedValue"
#define ERROR_MSGID_KEY "MessageId"


#define PORT_PF_TARGET_INSUFFICIENT 0xAC


#define CHIP_RULE_CHECK_FAIL 0xAD


#define PROTOCOLS_CHECK_FAIL 0xAE


#define PROPERTY_TYPE_ERROR 0xAF


#define PORT_ID_SEQUENCE_ERROR 0xB0

#define QINQ_VLANID_ERROR 0xB1
#define BOOTPROTOCOL_NOT_FOUND_IN_PFS 0xB2


#define PROPERTY_MISSING_ID "PropertyMissing"
#define BOARD_UNSUPPORTED_ID "PropertyModificationNotSupported"
#define PORT_PF_TARGET_EXCEED_ID "ExcessiveQuantity"
#define PROPERTY_VALUE_ERROR_ID "PropertyValueError"
#define PROPERTY_VALUE_NOT_INLIST_ID "PropertyValueNotInList"
#define MODE_UNSUPPORTED_ID "CurrentModeNotSupport"
#define BANDWITH_ERROR_ID "SettingBandWidthFailed"
#define VLANID_CONFLICT_ID "VlanIdConflict"
#define TRUNK_RANGE_CHECK_FAIL_ID "TrunkRangeCheckFail"
#define CONFIG_BUSY "ConfigurationBusy"
#define PROPERTY_UNKNOW_ID "PropertyUnknown"

#define PORT_PF_TARGET_INSUFFICIENT_ID "InsufficientQuantity"


#define CHIP_RULE_CHECK_FAIL_ID "ChipRuleCheckFail"


#define PROTOCOLS_CHECK_FAIL_ID "ProtocolsCheckFail"


#define PROPERTY_TYPE_ERROR_ID "PropertyValueTypeError"


#define PORT_ID_SEQUENCE_ERROR_ID "PortIdSequenceError"


#define MSGID_QINQ_VLANID_ERROR "VlanIdValueError"

#define BOOTPROTOCOL_NOT_FOUND_IN_PFS_ID "BootProtocolNotFoundInPFs"


#define BOOT_TYPE_NOT_FOUND 0
#define BOOT_TYPE_IS_UEFI 1
#define BOOT_TYPE_NOT_UEFI 2

#define BIOS_BOOT_TYPE_STR "BootType"
#define BIOS_UEFI_BOOT_STR "UEFIBoot"
#define BIOS_LEGACY_BOOT_STR "LegacyBoot"

#define LOM_CARD_DEVICE_NAME "LOM"

#define MESSAGE_PROPERTY_PATH_HEAD "Oem/Huawei/Configuration"

#define CLP_520_522 1

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
