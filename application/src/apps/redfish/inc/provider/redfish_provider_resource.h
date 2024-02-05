

#ifndef __REDFISH_PROVIDER_RESOURCE_H__
#define __REDFISH_PROVIDER_RESOURCE_H__

#include <json.h>
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "redfish_task.h"
#include "redfish_message.h"
#include <utmp.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 




#define RF_ERROR (-1)
#define RF_MATCH_OK 0
#define MAP_PROPERTY_NULL { \
        0                   \
    }
#define PRIVILEGE_MAX_LEN 20
#define RF_ENABLE 1
#define RF_DISABLE 0
#define RF_ITEM_PER_PAGE 20



#define RF_INIT_VAL_MACADDR "00:00:00:00:00:00"

#define RFPROP_COMMON_ID "Id"
#define RFPROP_COMMON_NAME "Name"
#define RFPROP_COMMON_MANUFACTURER "Huawei"
#define RFPROP_MEMBERS_COUNT "Members@odata.count"
#define RFPROP_MEMBERS "Members"
#define RFPROP_ODATA_CONTEXT "@odata.context"
#define RFPROP_ODATA_ID "@odata.id"
#define RFPROP_ODATA_TYPE "@odata.type"
#define RFPROP_ACTIONS "Actions"
#define RFPROP_TARGET "target"
#define RFPROP_SETTINGSOBJECT "SettingsObject"


#define RFPROP_COMMON_DESCRIPTION "Description"

#define RFPROP_ACTION_INFO "@Redfish.ActionInfo"
#define RFPROP_DEVICELOCATOR "DeviceLocator"
#define RFPROP_POSITION "Position"
#define RFPROP_POWER "Power"

#define ERROR_LOG_NULL_POINTER "%s: NULL pointer"
#define ERROR_LOG_WITH_CODE "%s: %s failed, error code: %d"
#define RFPROP_MCUVERSION "McuFirmwareVersion"
#define RFPROP_PCIE_ALARM_INDICATELED    "AlarmIndicatorLed"

#define RFPROP_FPGA_MEMORY "Memory"
#define RFPROP_FPGA_MEMORY_NAME "DimmName"
#define RFPROP_FPGA_MEMORY_STATE "State"
#define RFPROP_FPGA_MEMORY_MANUFACTURER "Manufacturer"
#define RFPROP_FPGA_MEMORY_SN "SerialNumber"
#define RFPROP_FPGA_MEMORY_PARTNUM "PartNum"
#define RFPROP_FPGA_MEMORY_CAPACITY_MB "CapacityMB"



#define RFPROP_FPGA_ECC_INFO "ChipsECC"
#define RFPROP_FPGA_CHIP_TEMP "ChipTemperatureCelsius"

#define RFPROP_NPU_MEMORY_CAPACITY_MB "MemoryCapacityMiB"


#define RFPROP_UUID "UUID"



#define RFPROP_FUNCTIONTYPE "FunctionType"

#define RFPROP_PCIECARDTYPE "PCIeCardType"

#define RFPROP_ATTRIBUTES "Attributes"

#define RFACTION_PARAM_VALUE_TEXT "text"
#define RFACTION_PARAM_VALUE_URI "URI"

#define RFPROP_CERT_SUBJECT_INFO "Subject"
#define RFPROP_CERT_ISSUER_INFO "Issuer"
#define RFPROP_CERT_START_TIME "ValidNotBefore"
#define RFPROP_CERT_EXPIRATION "ValidNotAfter"
#define RFPROP_CERT_SN "SerialNumber"
#define RFPROP_CERT_SIG_ALGO "SignatureAlgorithm"
#define RFPROP_CERT_KEY_USAGE "KeyUsage"
#define RFPROP_CERT_KEY_LEN "PublicKeyLengthBits"

#define ENABLED_STRING "Enabled"
#define DISABLED_STRING "Disabled"

#define AUTO_STRING "Auto Failover Mode"
#define MANUAL_STRING "Manual Switch Mode"

#define MEMBERID_STRING "MemberId"

#define SEVERITY_NORMAL 0
#define SEVERITY_MINOR 1
#define SEVERITY_MAJOR 2
#define SEVERITY_CRITICAL 3
#define CRITICAL_STRING "Critical"
#define MAJOR_STRING "Major"
#define MINOR_STRING "Minor"
#define NORMAL_STRING "Normal"
#define WARNING_STRING "Warning"
#define OK_STRING "OK"
#define INFORMATIONAL_STRING "Informational"
#define NONE_STRING "None"
#define INFO_STRING "Info"


#define ABSENT_STRING "Absent"
#define PRESENT_STRING "Present"

#define CRITICA_NUM_LEVEL 0x08
#define MAJOR_NUM_LEVEL 0x0c
#define MINO_NUM_LEVEL 0x0e
#define NORMAL_NUM_LEVEL 0x0f
#define NONE_NUM_LEVEL 0x00


typedef struct _tag_special_url_map {
    gchar source_url[MAX_URL_LEN];
    gchar target_url[MAX_URL_LEN];
    const gchar *members_json_pointer;
} SPECIAL_URL_MAP_S;


typedef enum _tag_special_url_index {
    URL_INDEX_DRIVES
} SPECIAL_URL_INDEX;

typedef enum _import_cert_file_type {
    FILE_TYPE_ROOT_CERT,
    FILE_TYPE_CRL,
    FILE_TYPE_LDAP_CRL,
    FILE_TYPE_SYSLOG_CRL,
    FILE_TYPE_HTTPS_CERT,
    FILE_TYPE_CLIENT_CERT
} IMPORT_CERT_FILE_TYPE;

#define DRIVES_MEMBERS_JSON_POINTER "Links/Drives"



#define RFPROP_ServiceRoot_UUID "UUID"

#define RFPROP_SERVICEROOT_SECURITY_BANNER "SecurityBanner"
#define RFPROP_SERVICEROOT_PRODUCT_NAME "ProductName"
#define RFPROP_SERVICEROOT_PRODUCT_SN "ProductSerialNumber"
#define RFPROP_SERVICEROOT_HOST_NAME "HostName"
#define RFPROP_SERVICEROOT_LANGUAGE_SET "LanguageSet"
#define RFPROP_SERVICEROOT_COPYRIGHT "Copyright"
#define RFPROP_SERVICEROOT_DOWNLOAD_KVM_LINK "DownloadKVMLink"
#define RFPROP_SERVICEROOT_DOC_SUPPORT_FLAG "DocSupportFlag"
#define RFPROP_SERVICEROOT_DOMAIN_NAME "DomainName"
#define RFPROP_SERVICEROOT_PRODUCT_PIC "ProductPicture"
#define RFPROP_SERVICEROOT_SMSNAME "SmsName"
#define RFPROP_SERVICEROOT_SOFTWARE_NAME "SoftwareName"

#define RFPROP_SERVICEROOT_CHASSIS_HEIGHT_U "ChassisHeightU" 
#define RFPROP_PLASH_PROTECT_NAME "FlashWriteProtectState"
#define RFPROP_PLATFORM_NAME "Platform"
#define ARM_ENABLE 1
#define ARM_DISABLE 0


#define RFPROP_SERVICEROOT_KRB_STATE "KerberosEnabled"


#define MAX_DOMAIN_BYTE_NUM 6

#define RF_UUID_LEN 40
#define RF_GUID_LEN 16

#define RFPROP_INFORMATION "Information"


#define SMM_CHASSIS_IPV4CONFIG "ipv4config"
#define SMM_CHASSIS_IPV6CONFIG "ipv6config"
#define RFPROP_IPCONFIG_COMPONENTS_IPV4 "IPv4Addresses"
#define RFPROP_IPCONFIG_ID "Id"
#define RFPROP_IPCONFIG_IPMODE "AddressOrigin"
#define RFPROP_IPCONFIG_IPADDR "Address"
#define RFPROP_IPCONFIG_GATEWAY "GateWay"
#define RFPROP_IPCONFIG_MASK "SubnetMask"
#define RFPROP_IPCONFIG_COMPONENTS_IPV6 "IPv6Addresses"
#define RFPROP_IPCONFIG_GATEWAY_IPV6 "IPv6DefaultGateway"
#define RFPROP_IPCONFIG_PREFIX_LENGTH_IPV6 "PrefixLength"
#define RFPROP_IPCONFIG_SLAAC_IP_IPV6 "SLACCIPv6Addresses"

#define NETFN_TRANSPORT_REQ 0X0c
#define IPMI_GET_LAN_CONFIGURATION_PARAMETERS 0x02
#define IPMI_IPV6_SLAAC_IP_OFFSET 0x3b
#define IPMI_IPV6_PREFIX_OFFSET 0xcd
#define IPMI_IPV6_MODE_OFFSET 0xd0
#define IPMI_IPV4_ADDRESS_OFFSET 0x03
#define IPMI_IPV4_MASK_OFFSET 0x06
#define IPMI_IPV4_GATEWAY_OFFSET 0x0c
#define IPMI_IPV4_MODE_OFFSET 0x04
#define IPMI_MAC_ADDRESS_OFFSET 0x05


#define RFPROP_OVERVIEW_COMPONENTS "Components"
#define RFPROP_OVERVIEW_ID "Id"
#define RFPROP_OVERVIEW_HOTSWAPSTATE "HotswapState"
#define RFPROP_OVERVIEW_HEALTH "Health"
#define RFPROP_OVERVIEW_MANAGEDBYMM "ManagedByMM"
#define RFPROP_OVERVIEW_WIDTH "Width"
#define RFPROP_OVERVIEW_HEIGHT "Height"



#define RFPROP_ACCOUNTSERVICE_MIN_PSWLEN "MinPasswordLength" 
#define RFPROP_ACCOUNTSERVICE_MAX_PSWLEN "MaxPasswordLength" 
#define RFPROP_ACCOUNTSERVICE_LOCKOUTTHRESHOLD \
    "AccountLockoutThreshold" 
#define RFPROP_ACCOUNTSERVICE_LOCKOUTDURATION \
    "AccountLockoutDuration" 

#define RFPROP_MANAGER_FIRMWARE_VERSION "FirmwareVersion" 
#define RFPROP_MANAGER_MODEL "Model"                      

#define RFPROP_MANAGER_TIMEZONE "DateTimeLocalOffset"          
#define RFPROP_MANAGER_DST_ENABLE "DSTEnabled"                 
#define RFPROP_MANAGER_STATUS "Status"                         
#define RFPROP_MANAGER_NETWORKPROTOCOL "NetworkProtocol"       
#define RFPROP_MANAGER_SYSLOG "SyslogService"                  
#define RFPROP_MANAGER_ETHERNETINTERFACES "EthernetInterfaces" 
#define RFPROP_MANAGER_SECURITY_SVC "SecurityService"          
#define RFPROP_MANAGER_RESET "Manager.Reset"
#define RFPROP_MANAGER_FORCE_FAILOVER "Manager.ForceFailover"
#define RFPROP_MANAGER_SNMP "SnmpService" 
#define RFPROP_MANAGER_LLDP "LldpService" 
#define RFPROP_MANAGER_USBMGMT "USBMgmtService"
#define RFPROP_MANAGER_ENERGY_SAVING "EnergySavingService"                     
#define RFPROP_MANAGER_SMTP "SmtpService"                                      
#define RFPROP_MANAGER_SYSLOG "SyslogService"                                  
#define RFPROP_MANAGER_KVM "KvmService"                                        
#define RFPROP_MANAGER_NTP "NtpService"                                        
#define RFPROP_MANAGER_SWITCH_PROFILE "SwitchProfiles"                         
#define RFPROP_MANAGER_PWON_PERMIT_TMOUT_MINUTES "PowerOnPermitTimeoutMinutes" 
#define RFPROP_MANAGER_PWON_PERMIT_STATUS "PowerOnPermitStatus"                
#define RFPROP_MANAGER_SHELF_POWER_BUTTON_MODE "ShelfPowerButtonMode"          
#define RFPROP_MANAGER_PWON_CONTROL_ENABLE "ControlEnabled" 
#define RFPROP_MANAGER_POWER_ON_SLOT_ID "SlotIds"           
#define RFPROP_MANAGER_SHELF_POWER_STATE "ShelfPowerState"  
#define RFPROP_MANAGER_DIAGNOSTIC "DiagnosticService"       
#define RFPROP_MANAGER_SP "SPService"                       
#define RFPROP_MANAGER_SMS "SmsService"                     
#define RFPROP_MANAGER_NICS "NICs"
#define RFPROP_MANAGER_SLOT_ID "SlotId"
#define RFPROP_MANAGER_SWI_MODEL "SwitchModel"
#define RFPROP_MANAGER_FRU_ID "FruId"
#define RFPROP_MANAGER_REBOOT "Reboot"
#define RFPROP_MANAGER_LICENSE_SERVICE "LicenseService"

#define RFPROP_MANAGER_OEM_TIMEZONE_CONFIGURABLE "TimeZoneConfigurable"



#define RFPROP_MANAGER_BACKUP_TIME "BackupTime"



#define RFPROP_MANAGER_ROLLBACK "Oem/Huawei/Manager.RollBack"
#define RFPROP_MANAGER_GRAPHICALCONSOLE "GraphicalConsole"
#define RFPROP_MANAGER_SERIALCONSOLE "SerialConsole"
#define RFPROP_MANAGER_SERVICEENABLED "ServiceEnabled"
#define RFPROP_MANAGER_CONNECTTYPESSUPPORTED "ConnectTypesSupported"
#define RFPROP_MANAGER_MAXCONCURRENTSESSIONS "MaxConcurrentSessions"
#define RFPROP_MANAGER_UUID "UUID"
#define RFPROP_MANAGER_MODEL "Model"
#define RFPROP_MANAGER_IPMI "IPMI"
#define RFPROP_MANAGER_KVMIP "KVMIP"
#ifdef ARM64_HI1711_ENABLED // 1711的版本需要
#define RFPROP_MANAGER_iBMC "iBMC3"
#else
#define RFPROP_MANAGER_iBMC "iBMC"
#endif
#define RFPROP_MANAGER_iRM "iRM"

#define PS_REMOVEED_INFO "PsRemovedInfo"
#define FAN_REMOVEED_INFO "FanRemovedInfo"
#define BLADE_REMOVEED_INFO "BladeRemovedInfo"
#define SWI_REMOVEED_INFO "SwiRemovedInfo"
#define SMM_REMOVEED_INFO "SMMRemovedInfo"
#define PS_REMOVEED_POLICY "PsRemovedPlcy"
#define FAN_REMOVEED_POLICY "FanRemovedPlcy"
#define BLADE_REMOVEED_POLICY "BladeRemovedPlcy"
#define SWI_REMOVEED_POLICY "SwiRemovedPlcy"
#define SMM_REMOVEED_POLICY "SMMRemovedPlcy"



#define RF_CHASSIS_UI "/redfish/v1/Chassis/"
#define REDFISH_CHASSIS_MANAGEDBY "ManagedBy"
#define RFPROP_CASSIS_POWER "Power"
#define RFPROP_CHASSIS_POWERPCONTROL "/Power#/PowerControl"
#define REDFISH_CHASSIS_COMPUTERSYSTEMS "ComputerSystems"
#define REDFISH_CHASSIS_POWERVOLTAGES "/Power#/Voltages"
#define RFPROP_MANAGER_REDUNDANCY "Redundancy"
#define RFPROP_MANAGER_EVENT_SEVERITY_CFG "EventSeverityConfig"
#define RFPROP_MANAGER_REMOVED_SEVERITY "RemovedEventSeverity"
#define RFPROP_MANAGER_LINKS "Links"
#define RFPROP_MANAGER_ACTION "Actions"
#define RFPROP_MANAGER_THERMALTEM "/Thermal#/Temperatures"
#define RFPROP_MANAGER_THERMALFAN "/Thermal#/Fans"
#define RFPROP_CHASSIS_POWERPS "/Power#/PowerSupplies"
#define RFPROP_CHASSIS_ENCLOSURE_POWERSUPPLIES "PowerSupplies"
#define RFPROP_CHASSIS_ENCLOSURE_REDUNDANCY "Redundancy"
#define RFPROP_CHASSIS_STATUS "Status"
#define RFPROP_CHASSIS_STATUS_STATE "State"
#define RFPROP_CHASSIS_STATUS_HEALTH "Health"
#define N1_MODE_STRING "N+1"
#define NN_MODE_STRING "N+N"


#define URI_FORMAT_CHASSIS_INLET_HISTORY_TEMPERATURE "/redfish/v1/Chassis/%s/Thermal/InletHistoryTemperature"
#define URI_FORMAT_CHASSIS_POWER_HISTORY_DATA "/redfish/v1/Chassis/%s/Power/PowerHistoryData"



#define RFPROP_CHASSIS_DRIVES "Drives"


#define RFPROP_MANAGER_STATE_FALSE 0
#define RFPROP_MANAGER_STATE_TRUE 1
#define RFPROP_MANAGER_IPMI_MAXCONCURRENTSESSIONS 1
#define RFPROP_MANAGER_KVMIP_MAXCONCURRENTSESSIONS 2




typedef enum tag_syslog_prop_trans_direction {
    SYSLOG_PROP_INT_TO_STR,
    SYSLOG_PROP_STR_TO_INT
} RF_SYSLOG_TRANS_DIRECTION;



#define RF_UNIT_CHASSIS_PREFIX "U"
#define RFPROP_UNITDEVICES_HEIGHT "UHeight"
#define RFPROP_UNITDEVICES_AVAILABLE_RACK_SPACE "AvailableRackSpaceU"
#define RFPROP_UNITDEVICES_LIFECYCLE "LifeCycleYear"
#define RFPROP_UNITDEVICES_CHECKINTIME "CheckInTime"
#define RFPROP_UNITDEVICES_WEIGHT "WeightKg"
#define RFPROP_UNITDEVICES_RATEPOWER "RatedPowerWatts"
#define RFPROP_UNITDEVICES_ASSETOWNER "AssetOwner"
#define RFPROP_UNITDEVICES_DEVICETYPE "DeviceType"
#define RFPROP_UNITDEVICES_DISCOVEREDTIME "DiscoveredTime"
#define RFPROP_UNITDEVICES_INDICATOR_LED "IndicatorLED"
#define RFPROP_UNITDEVICES_INDICATOR_COLOR "IndicatorColor"
#define RFPROP_UNITDEVICES_UNUM "TopUSlot"
#define RFPROP_UNITDEVICES_RFID_TAG_UID "RFIDTagUID"
#define RFPROP_UNITDEVICES_RW_CAPABILITY "RWCapability"
#define RFPROP_UNITDEVICES_EXTEND_FIELD "ExtendField"



#define RFPROP_DEVICES_SESSION_INFO "SessionInfoOfDevices"
#define RFPROP_TOKEN_AVAILABLE "TokenAvailable"



#define RFPROP_DEVICES_DISCOVERED_COUNT "DiscoveredDevicesCount"
#define RFPROP_DEVICES_IPINFO "NetworkInfoOfDevices"
#define RFPROP_DEVICES_SN "SerialNumber"
#define RFPROP_DEVICE_IPV4ADDR "IPv4Addresses"          
#define RFPROP_DEVICE_IPV6ADDR "IPv6Addresses"          
#define RFPROP_DEVICE_IPV6_GATEWAY "IPv6DefaultGateway" 
#define RFPROP_DEVICE_USER_NAME "UserName"              
#define RFPROP_DEVICE_PASSWORD "Password"               



#define RFPROP_DEVICE_POWER_CAPPING_INFO "DevicePowerCappingInfo"
#define RFPROP_DEVICE_POWER_CAPPING_STATUS "PowerCappingStatus"
#define RFPROP_DEVICE_CURRENT_POWER_VALUE "PowerConsumedWatts"
#define RFPROP_DEVICE_CAPPING_VALUE "LimitInWatts"
#define RFPROP_DEVICE_POWER_LIMIT_VALUE "MinPowerLimitInWatts"

#define RFPROP_DEVICE_PRIORITY "ServerPriority"
#define RFPROP_RACK_ENABLE_POWER_CAPPING "EnablePowerCapping"
#define RFPROP_RACK_CAPPING_STRATEGY "CappingStrategy"
#define RFPROP_RACK_CAPPING_NUMBERS "CappingNumbers"
#define RFPROP_RACK_POWER_CAPPING_VALUE "RackPowerCapping"
#define RFPROP_RACK_POWER_CAPPING_STATUS "PowerCappingStatus"
#define RFPROP_RACK_PEAK_CLIPPING_ENABLED "PeakPowerClippingEnabled"
#define RFPROP_RACK_BACKUP_POWER_WATTS "BackupPowerWatts"
#define RFPROP_RACK_BACKUP_PERIOD_SECONDS "BackupPeriodSeconds"

#define RF_RACK_POWER_CAPPING_ON 1
#define RF_POWER_CAPPING_ENABLE "Enable"
#define RF_POWER_CAPPING_DISABLE "Disable"
#define RF_CAPPING_STRATEGY_EQUALIZATION "Equalization"
#define RF_CAPPING_STRATEGY_DESIGNATION "Designation"
#define RF_ALLOCATION_FOR_EQUALIZATION 1
#define RF_ALLOCATION_FOR_DESIGNATION 2




#define RFPROP_CHASSIS_PCIEDEVICES "PCIeDevices"
#define RFPROP_CHASSIS_PCIEDEVICES_METADATA "/redfish/v1/$metadata#Chassis/Members/%s/PCIeDevices/Members/$entity"
#define URI_FORMAT_CHASSIS_PCIEDEVICE_COLLECTION        "/redfish/v1/Chassis/%s/PCIeDevices"
#define URI_FORMAT_CHASSIS_PCIEDEVICES_URI "/redfish/v1/Chassis/%s/PCIeDevices/%s"
#define RFPROP_PCIEDEVICES_DESCRIPTION "Description"
#define RFPROP_PCIEDEVICES_MANUFACTURER "Manufacturer"
#define RFPROP_PCIEDEVICES_MODEL "Model"
#define RFPROP_PCIEDEVICES_CARD_MANUFACTURER "CardManufacturer"
#define RFPROP_PCIEDEVICES_CARD_MODEL "CardModel"
#define RFPROP_PCIEDEVICES_SERIALNUMBER "SerialNumber"
#define RFPROP_PCIEDEVICES_FIRMWAREVERSION "FirmwareVersion"
#define RFPROP_PCIEDEVICES_STATUS "Status"
#define RFPROP_PCIEDEVICES_INTERFACE "PCIeInterface"
#define RFPROP_PCIEDEVICES_LANES_IN_USE "LanesInUse"
#define RFPROP_PCIEDEVICES_MAX_LANES "Maxlanes"
#define RFPROP_PCIEDEVICES_MAX_LANES_CMCC "MaxLanes"
#define RFPROP_PCIEDEVICES_LINKS "Links"
#define RFPROP_PCIEDEVICES_STATUS_STATE "Enabled"

#define RFPROP_PCIEDEVICES_SLOT_NUM                     "SlotNumber"
#define RFPROP_PCIEDEVICES_PRODUCT_NAME "ProductName"
#define RFPROP_PCIEDEVICES_BOARD_NAME "BoardName"
#define RFPROP_PCIEDEVICES_BOARDID "BoardId"
#define RFPROP_PCIEDEVICES_PCB_VERSION "PCBVersion"
#define RFPROP_PCIEDEVICES_CPLD_VERSION "CPLDVersion"
#define RFPROP_PCIEDEVICES_M2DEVICE1_PRESENCE "M2Device1Presence"
#define RFPROP_PCIEDEVICES_M2DEVICE2_PRESENCE "M2Device2Presence"
#define RFPROP_PCIEDEVICES_SODIMM00_PRESENCE "SODIMM00Presence"
#define RFPROP_PCIEDEVICES_SODIMM10_PRESENCE "SODIMM10Presence"
#define RFPROP_PCIEDEVICES_STORAGE_MANAGEMENT_IP "StorageManagementIP"
#define RFPROP_PCIEDEVICES_STORAGE_MANAGEMENT_VLANID "StorageManagementVlanId"
#define RFPROP_PCIEDEVICES_STORAGE_MANAGEMENT_IPV6 "StorageManagementIPv6"
#define RFPROP_PCIEDEVICES_STORAGE_MANAGEMENT_PORT_STATUS "StorageManagementPortStatus"
#define RFPROP_PCIEDEVICES_BOOTOPTION "BootOption"
#define RFPROP_PCIEDEVICES_BOOT_EFFECTIVE "BootEffective"
#define RFPROP_PCIEDEVICES_EXTENDCARD_INFO "ExtendCardInfo"
#define RFPROP_PCIEDEVICES_EXTENDCARD_MANUFACTURER "Manufacturer"
#define RFPROP_PCIEDEVICES_EXTENDCARD_CHIPMANUFACTURER "ChipManufacturer"
#define RFPROP_PCIEDEVICES_EXTENDCARD_DESCRIPTION "Description"
#define RFPROP_PCIEDEVICES_EXTENDCARD_MODEL "Model"
#define RFPROP_PCIEDEVICES_REMAINING_WEAR_RATE_PERCENT "RemainingWearRatePercent"
#define RFPROP_PCIEDEVICES_BOMID "BomId"
#define RFPROP_PCIEDEVICES_SSD1_PRESENCE "SSD1Presence"
#define RFPROP_PCIEDEVICES_SSD2_PRESENCE "SSD2Presence"
#define STORAGE_MANAGEMENT_VLANID_INVALID 0xFFF
#define BOOTOPTION_HDD "HDD Boot"
#define BOOTOPTION_PXE "PXE Boot"

#define BOOTOPTION_BASE_BOARD_PXE_BOOT "Base Board PXE Boot"
#define BOOTOPTION_BUCKLE_CARD1_PXE_BOOT "Buckle Card1 PXE Boot"
#define BOOTOPTION_BUCKLE_CARD2_PXE_BOOT "Buckle Card2 PXE Boot"
#define BOOTOPTION_BUCKLE_CARD3_PXE_BOOT "Buckle Card3 PXE Boot"
#define BOOTOPTION_UNSET  "None"
#define RFPROP_CHASSIS "Chassis"

#define BOOTORDER_ONCE "One-time"
#define BOOTORDER_PERMANENT "Permanent"

#define RFPROP_PCIEFUNCTION_METADATA \
    "/redfish/v1/$metadata#Chassis/Members/%s/PCIeDevices/Members/%s/Functions/Members/$entity"
#define URI_FORMAT_PCIEFUNCTION_URI_NUMBER "/redfish/v1/Chassis/%s/PCIeDevices/%s/Functions/%d"
#define URI_FORMAT_PCIEFUNCTION_URI_STRING "/redfish/v1/Chassis/%s/PCIeDevices/%s/Functions/%s"
#define URI_FORMAT_CHASSIS_DRIVES_URI "/redfish/v1/Chassis/%s/Drives/%s"
#define RFPROP_CHASSIS_PCIEFUNCTIONS "PCIeFunctions"
#define RFPROP_PCIEFUNCTION_MEMBERID 1
#define RFPROP_PCIEFUNCTION_NAME "Function"
#define RFPROP_PCIEFUNCTION_DEVICEID "DeviceId"
#define RFPROP_PCIEFUNCTION_VENDORID "VendorId"
#define RFPROP_PCIEFUNCTION_SUBSYSTEMID "SubsystemId"
#define RFPROP_PCIEFUNCTION_SUBSYSTEMVENDORID "SubsystemVendorId"
#define RFPROP_PCIEFUNCTION_DEVICECLASS "DeviceClass"
#define RFPROP_PCIEFUNCTION_CONNECTEDCPU "AssociatedResource"
#define RFPROP_PCIEFUNCTION_PCIESWITCH "PCIeSwitch"
#define RFPROP_PCIEFUNCTION_PCH "PCH"
#define RFPROP_PCIEFUNCTION_CPUNAME "CPU%d"
#define RFPROP_PCIEFUNCTION_ROOTPORTBDF "RootPortBDF"
#define RFPROP_PCIEFUNCTION_BUS "BusNumber"
#define RFPROP_PCIEFUNCTION_DEVICE "DeviceNumber"
#define RFPROP_PCIEFUNCTION_FUNCTION "FunctionNumber"
#define RFPROP_PCIEFUNCTION_DEVICE_BDF "DeviceBDF"
#define RFPROP_PCIEFUNCTION_D_BUS "Bus"
#define RFPROP_PCIEFUNCTION_D_DEVICE "Device"
#define RFPROP_PCIEFUNCTION_D_FUNCTION "Function"
#define BDF_STR_LEN 4
#define RFPROP_PCIEFUNCTION_RAIDCARD "RaidCard"
#define RFPROP_PCIEFUNCTION_STORAGE_CONTROLLER "MassStorageController"
#define RFPROP_PCIEFUNCTION_NETWORK_CONTROLLER "NetworkController"
#define RFPROP_PCIEFUNCTION_DISPLAY_CONTROLLER "DisplayController"
#define RFPROP_PCIEFUNCTION_INTELLIGENT_CONTROLLER "IntelligentController"
#define RFPROP_PCIEFUNCTION_PROCESSING_ACCELERATORS "ProcessingAccelerators"
#define RFPROP_PCIEFUNCTION_MEMORY_CONTROLLER "MemoryController"
#define RFPROP_PCIEFUNCTION_UNCLASSIFIED_DEVICE "UnclassifiedDevice"
#define RFPROP_PCIEFUNCTION_OTHER "Other"
#define RFPROP_PCIEFUNCTION_BDFNUMBER "BDFNo"
#define RFPROP_PCIEFUNCTION_PCIE_RAID 1
#define RFPROP_PCIEFUNCTION_PCIE_SSD 2
#define RFPROP_PCIEFUNCTION_PCIE_NETCARD 3
#define RFPROP_PCIEFUNCTION_PCIE_LINK_WIDTH_ABILITY "LinkWidthAbility"
#define RFPROP_PCIEFUNCTION_PCIE_LINK_SPEED_ABILITY "LinkSpeedAbility"
#define RFPROP_PCIEFUNCTION_PCIE_LINK_WIDTH "LinkWidth"
#define RFPROP_PCIEFUNCTION_PCIE_LINK_SPEED "LinkSpeed"

#define RFPROP_PCIEFUNCTION_FUNCTIONID "FunctionId"



#define RFPROP_FUNCTIONTYPE_RAIDCARD "RAID Card"
#define RFPROP_FUNCTIONTYPE_NETCARD "Net Card"
#define RFPROP_FUNCTIONTYPE_GPUCARD "GPU Card"
#define RFPROP_FUNCTIONTYPE_STORAGE "Storage Card"
#define RFPROP_FUNCTIONTYPE_SDI "SDI Card"
#define RFPROP_FUNCTIONTYPE_ACCELERATE "Accelerate Card"
#define RFPROP_FUNCTIONTYPE_PCIERISER "PCIe Riser Card"
#define RFPROP_FUNCTIONTYPE_FPGA "FPGA Card"
#define RFPROP_FUNCTIONTYPE_NPUCARD "NPU Card"
#define RFPROP_FUNCTIONTYPE_STORAGE_DPU "Storage DPU Card"

#define FUNCTIONTYPE_RAIDCARD "RAID"
#define FUNCTIONTYPE_NETCARD "NIC"
#define FUNCTIONTYPE_GPUCARD "GPU"
#define FUNCTIONTYPE_STORAGE "SAS"
#define FUNCTIONTYPE_SDI "SDI"
#define FUNCTIONTYPE_ACCELERATE "Accelerate"
#define FUNCTIONTYPE_PCIERISER "Riser"
#define FUNCTIONTYPE_FPGA "FPGA"
#define FUNCTIONTYPE_FC "FC"
#define FUNCTIONTYPE_IB "IB"
#define FUNCTIONTYPE_STORAGE_DPU "Storage DPU"
#define FUNCTIONTYPE_NPU "NPU"


#define RESOURCE_ID_PCIE_CARD_BELONG_TO_CPU1_AND_CPU2_STR ("CPU1,CPU2")

#define RESOURCE_ID_PCIE_CARD_BELONG_TO_ALL_CPU_STR ("CPU1,CPU2,CPU3,CPU4")




typedef struct tag_syslog_certificate_info {
    const gchar *subject_str;
    const gchar *issuer_str;
    const gchar *start_time_str;
    const gchar *expiration_str;
    const gchar *serial_number_str;
    const gchar *sig_algo_str;
    const gchar *key_usage_str;
    guint32 key_len;
} RF_SYSLOG_CERTIFICATE;

typedef struct tag_syslog_item_info {
    gchar member_id[PROP_VAL_LENGTH];
    guint8 enabled;
    
    gchar address[HOST_ADDR_MAX_LEN + 1];
    
    guint32 port;
    guint32 log_type_int;
} RF_SYSLOG_ITEM;

#define RF_SYSLOG_PROTOCOL_TLS "TLS"
#define RF_SYSLOG_PROTOCOL_TCP "TCP"
#define RF_SYSLOG_PROTOCOL_UDP "UDP"
#define RF_AUTH_MODE_ONE_WAY "OneWay"
#define RF_AUTH_MODE_TWO_WAY "TwoWay"
#define RF_SYSLOG_ITEM_LOG_OP "OperationLog"
#define RF_SYSLOG_ITEM_LOG_SEC "SecurityLog"
#define RF_SYSLOG_ITEM_LOG_SEL "EventLog"
#define RF_SYSLOG_MSG_FORMAT "MessageFormat"
#define RF_SYSLOG_MSG_FORMAT_CUSTOM "Custom"
#define RF_SYSLOG_MSG_FORMAT_RFC3164 "RFC3164"
#define RF_ALARM_SEVERITY "AlarmSeverity"
#define RF_TRAN_PROTOCOL "TransmissionProtocol"
#define RF_AUTH_MODE "AuthenticateMode"
#define RF_ROOT_CERTIFICATE "RootCertificate"
#define RF_CLIENT_CERTIFICATE "ClientCertificate"

#define RFPROP_SYSLOG_SERVERS "SyslogServers"
#define RFPROP_SYSLOG_ITEM_PORT "Port"
#define RFPROP_SYSLOG_ITEM_LOG_TYPE "LogType"
#define RFPROP_SYSLOG_IMPORT_ROOT_CERT "SyslogService.ImportRootCertificate"
#define RFPROP_SYSLOG_IMPORT_CLIENT_CERT "SyslogService.ImportClientCertificate"
#define RFPROP_SYSLOG_IMPORT_CRL "SyslogService.ImportCrl"
#define RFPROP_SYSLOG_DELETE_CRL "SyslogService.DeleteCrl"
#define RFPROP_SYSLOG_TEST "SyslogService.SubmitTestEvent"
#define RFPROP_SYSLOG_CRL_ENABLED "CrlVerificationEnabled"
#define RFPROP_SYSLOG_CRL_VALID_FROM "CrlValidFrom"
#define RFPROP_SYSLOG_CRL_VALID_TO "CrlValidTo"

#define TMP_SYSLOG_ROOT_CERT_FILE "/tmp/syslog_root_cert.pem"
#define TMP_SYSLOG_CLIENT_CERT_FILE "/tmp/syslog_client_cert.pfx"



#define RFPROP_SWITCH_PROFILES "SwitchProfiles"
#define RFPROP_SWITCH_PROFILES_SLOT "Slot"
#define RFPROP_SWITCH_PROFILES_BOARDTYPE "BoardType"
#define RFPROP_SWITCH_PROFILES_PLANETYPE "PlaneType"
#define RFPROP_SWITCH_PROFILES_FLIENAME "FlieName"
#define RFPROP_SWITCH_PROFILES_BACKUPTIME "BackupTime"

#define TASK_IMPORT_ROOT_CERT "import root cert"
#define TASK_IMPORT_SYSLOG_CLIENT_CERT "import syslog client cert"
#define RFPROP_SYSLOG_CERT_TYPE "CertType"
#define SYSLOG_ROOT_CERT_FILE_ID 0x15
#define SYSLOG_CLIENT_CERT_FILE_ID 0x16
#define SMPT_ROOT_CERT_FILE_ID 0x17


#define RFPROP_MANAGER_MANAGER_IPV4_MODE0 0
#define RFPROP_MANAGER_MANAGER_IPV4_MODE1 1
#define RFPROP_MANAGER_MANAGER_IPV4_MODE2 2

#define RFPROP_MANAGER_MANAGER_IPV4_MODE "IPMode"
#define RFPROP_MANAGER_MANAGER_IPV4_ADDRESS "IPv4Address"
#define RFPROP_MANAGER_MANAGER_MODESTATIC "Static"
#define RFPROP_MANAGER_MANAGER_MODEDHCP "DHCP"
#define RFPROP_MANAGER_MANAGER_IPV4_DEFAULT "DefaultIPv4Info" 


#define RFPROP_MANAGER_MANAGER_IP_ADDR "DeviceIPv4"   
#define RFPROP_MANAGER_MANAGER_IPV6_ADDR "DeviceIPv6" 



#define RFPROP_MANAGER_ETH_ODATA_ID "@odata.id"
#define RFPROP_MANAGER_ETH_ID "Id"
#define RFPROP_MANAGER_ETH_MACADDR "PermanentMACAddress"
#define RFPROP_MANAGER_BMC_HOSTNAME "HostName"
#define RFPROP_MANAGER_BMC_FQDN "FQDN" 
#define RFPROP_MANAGER_ETH_VLAN "VLAN" 
#define RFPROP_MANAGER_ETH_VLAN_STATE "VLANEnable"
#define RFPROP_MANAGER_ETH_VLAN_ID "VLANId"
#define RFPROP_MANAGER_ETH_IPV4ADDR "IPv4Addresses" 
#define RFPROP_MANAGER_ETH_IPV4_STATIC_ADDR "IPv4StaticAddresses"
#define RFPROP_MANAGER_ETH_IPADDR "Address" 
#define RFPROP_MANAGER_ETH_IPMASK "SubnetMask"
#define RFPROP_MANAGER_ETH_IPMODE "AddressOrigin" 
#define RFPROP_MANAGER_ETH_IPGATEWAY "Gateway"    
#define RFPROP_MANAGER_ETH_IPV6_GATEWAY "IPv6DefaultGateway"
#define RFPROP_MANAGER_ETH_IPV6_STATIC_GATEWAYS "IPv6StaticDefaultGateways"
#define RFPROP_MANAGER_ETH_IPV6_ADDR "IPv6Addresses" 
#define RFPROP_MANAGER_ETH_IPV6_PREFIXLEN "PrefixLength"
#define RFPROP_MANAGER_ETH_IPV6_STATIC_ADDR "IPv6StaticAddresses"
#define RFPROP_MANAGER_ETH_NAMESERVER "NameServers"
#define RFPROP_MANAGER_ETH_STATIC_NAMESERVER "StaticNameServers"
#define RFPROP_MANAGER_ETH_OEM_IPVER "IPVersion"
#define RFPROP_MANAGER_ETH_OEM_NET_MODE "NetworkPortMode"
#define RFPROP_MANAGER_ETH_OEM_NCSI_MODE "NCSIMode"
#define RFPROP_MANAGER_ETH_OEM_DNS_MODE "DNSAddressOrigin"
#define RFPROP_MANAGER_ETH_OEM_SW_CONNS "SwitchConnections"
#define RFPROP_MANAGER_ETH_LINK "Link"
#define RFPROP_MANAGER_ETH_OEM_MANAGER_PORT "ManagementNetworkPort"
#define RFPROP_MANAGER_ETH_OEM_PORT_TYPE "Type"
#define RFPROP_MANAGER_ETH_OEM_PORT_SLIK "PortNumber"
#define RFPROP_MANAGER_ETH_OEM_NET_PORT "ManagementNetworkPort@Redfish.AllowableValues"
#define RFPROP_MANAGER_ETH_OEM_LINK_STATUS "LinkStatus"
#define RFPROP_MANAGER_ETH_OEM_ADAPTIVEPORT "AdaptivePort"
#define RFPROP_MANAGER_ETH_OEM_ADAPTIVE "AdaptiveFlag"
#define RFPROP_NIC_DHCPV4 "DHCPv4"
#define RFPROP_NIC_DHCP_ENABLED "DHCPEnabled"
#define RFPROP_NIC_USE_DNS_SERVER "UseDNSServers"
#define RFPROP_NIC_OPERATING_MODE "OperatingMode"
#define RFPROP_NIC_DHCPV6 "DHCPv6"
#define RFPROP_NIC_INTERFACE_ENABLED "InterfaceEnabled"


#define RFPROP_MANAGER_LLDP_ENABLED "LldpEnabled"
#define RFPROP_MANAGER_LLDP_WORKMODE "WorkMode"
#define RFPROP_MANAGER_LLDP_TX_DELAY "TxDelaySeconds"
#define RFPROP_MANAGER_LLDP_TX_INTERVAL "TxIntervalSeconds"
#define RFPROP_MANAGER_LLDP_TX_HOLD "TxHold"
#define RFVALUE_LLDP_WORKMODE_TX_STR "Tx"
#define RFVALUE_LLDP_WORKMODE_RX_STR "Rx"
#define RFVALUE_LLDP_WORKMODE_TX_RX_STR "TxRx"
#define RFVALUE_LLDP_WORKMODE_DISABLED_STR "Disabled"
#define RFVALUE_LLDP_WORKMODE_TX_CODE 1
#define RFVALUE_LLDP_WORKMODE_RX_CODE 2
#define RFVALUE_LLDP_WORKMODE_TX_RX_CODE 3
#define RFVALUE_LLDP_WORKMODE_DISABLED_CODE 0

#define RFPROP_MANAGER_USBMGMT_SERVICE_ENABLED "ServiceEnabled"
#define RFPROP_MANAGER_USBMGMT_USB_DEVICE_PRESENCE "USBDevicePresence"
#define RFPROP_MANAGER_USBMGMT_USB_UNCOMPRESS_PASSWORD   "USBUncompressPassword"
#define USB_UNCOMPRESS_PASSWORD_LEN_MIN                 "1"
#define USB_UNCOMPRESS_PASSWORD_LEN_MAX                 "32"



#define RFPROP_DIGITAL_WARRANTY_PRODUCTNAME "ProductName"
#define RFPROP_DIGITAL_WARRANTY_SN "SerialNumber"
#define RFPROP_DIGITAL_WARRANTY_MANUFACTUREDATE "ManufactureDate"
#define RFPROP_DIGITAL_WARRANTY_UUID "UUID"
#define RFPROP_DIGITAL_WARRANTY_UNITTYPE "UnitType"
#define RFPROP_DIGITAL_WARRANTY_STARTPOINT "StartPoint"
#define RFPROP_DIGITAL_WARRANTY_LIFESPAN "Lifespan"
#define RFPROP_DIGITAL_WARRANTY_FIRST_POWER_ON_TIME "FirstPowerOnTime"


#define RFPROP_MANAGER_ENERGY_SAVING_ENABLED "ServiceEnabled"
#define RFPROP_MANAGER_ENERGY_SAVING_DEACTIVE_THRE "DeactiveThreshold"
#define RFPROP_MANAGER_ENERGY_SAVING_CPU_USAGE_THRE "LogicalCoreCpuUtilisePercents"
#define RFPROP_MANAGER_ENERGY_SAVING_DEACTIVE_CONFIG "ExpectedDeactiveConfiguration"
#define RFPROP_MANAGER_ENERGY_SAVING_ACTIVE_CONFIG "ExpectedActiveConfiguration"
#define RFPROP_MANAGER_ENERGY_SAVING_ACTUAL_CONFIG "ActualActiveConfiguration"
#define RFPROP_MANAGER_ENERGY_SAVING_DEMT_ENABLED "DEMTEnabled"
#define RFPROP_MANAGER_ENERGY_SAVING_POWER_CONTROL "PowerControl"
#define RFPROP_MANAGER_ENERGY_SAVING_POWER_MODE "PowerMode"
#define RFPROP_MANAGER_ENERGY_SAVING_ACTIVE_PSU "ActivePSU"


#define REDFISH_ETH_PARAM_NOT_VALIID (-2)
#define REDFISH_ETH_ERR (-1)
#define REDFISH_ETH_PARAM_IP_CONFLICT_ERROR (-6)
#define REDFISH_ETH_UNSUPPORT (-3)
#define REDFISH_ETH_AVAFLAG_DISABLED (-8)


#define REDFISH_VOS_ERROR (-1)
#define IP_MODE_STATIC_STRING "Static"
#define IP_MODE_DHCP_STRING "DHCP"
#define IPV6_MODE_DHCP "DHCPv6"
#define IPV6_MODE_SLAAC "SLAAC"
#define IPV6_MODE_LINKLOCAL "LinkLocal"
#define DEFAULT_IPV6_PREFIX_LEN 64
#define NIC_OPERATING_MODE_STATEFUL "Stateful"
#define NIC_OPERATING_MODE_DISABLED "Disabled"
#define IP_VER_IPV4 "IPv4"
#define IP_VER_IPV6 "IPv6"
#define IP_VER_IPV4_ADN_IPV6 "IPv4AndIPv6"
#define NET_MODE_STRING_FIXED "Fixed"
#define NET_MODE_STRING_AUTO "Automatic"
#define DNS_ADD_ORIGIN_MANUAL_STR "Static"
#define DNS_ADD_ORIGIN_MANUAL 0
#define DNS_ADD_ORIGIN_AUTO 1
#define DNS_IP_VER_IPV4_STR "IPv4"
#define DNS_IP_VER_IPV6_STR "IPv6"
#define DNS_IP_VER_IPV4 0
#define DNS_IP_VER_IPV6 1
#define NET_TYPE_LOM_STR "LOM"
#define NET_TYPE_DEDICATED_STR "Dedicated"
#define NET_TYPE_PCIE_STR "ExternalPCIe"
#define NET_TYPE_AGGREGATION_STR "Aggregation"

#define NET_TYPE_LOM2_STR "FlexIO"

#define NET_TYPE_OCP_STR "OCP"
#define NET_TYPE_OCP2_STR "OCP2"
#define RF_UNSUPPORT_ADAPTIVE_NET_MODE 0

#define NUMBER_NETWORK_PORT_ATTRIBUTES_OEM 6



#define NUMBER_THERMAL_OEM 20


#define MGNT_PLANE_STR "Plane%d"
#define MGNT_PLANE_STR_VLAN "Plane%d.%d"
#define MGNT_FLOAT_IP_STR "FloatIp"
#define MGNT_DEFAULT_IP_STR "StaticIp"
#define MGNT_DEFAULT_IP_ETHGROUP_ID 8 
#define MGNT_DEFAULT_IP_VLAN_ID 4093  

#define RF_MANAGERS_URI_S "/redfish/v1/Managers/%s/%s"
#define RF_MANAGERS_URI "/redfish/v1/Managers/"
#define RF_MANAGERS_HMM1 "/redfish/v1/Managers/HMM1"
#define RF_MANAGERS_HMM2 "/redfish/v1/Managers/HMM2"
#define RFPROP_MANAGER_MANAGERFSERVER "ManagerForServers"
#define RFPROP_MANAGER_MANAGERFCHASSIS "ManagerForChassis"
#define RFPROP_MANAGER_ACTIONMTESET "/Actions/Manager.Reset"
#define RFPROP_MANAGER_TARGET "target"
#define RFPROP_MANAGER_RTALLOWVAL "ResetType@Redfish.AllowableValues"
#define RFPROP_MANAGER_HUAWEI "Huawei"
#define RFPROP_MANAGER_OEM "Oem"
#define RFPROP_MANAGER_ETH "/redfish/v1/Managers/%s/EthernetInterfaces/%s"
#define RFPROP_MANAGER_ETHS "/redfish/v1/Managers/%s/EthernetInterfaces"
#define URI_FORMAT_MANAGER_NIC "/redfish/v1/Managers/%s/NICs/%s"
#define URI_FORMAT_MANAGER_NICS "/redfish/v1/Managers/%s/NICs"
#define RFPROP_MANAGER_NETPRO "/redfish/v1/Managers/%s/NetworkProtocol"
#define MANAGER_SECURITY_SVC_URI "/redfish/v1/Managers/%s/SecurityService"
#define MANAGER_SECURITY_SVC_METADATA "/redfish/v1/$metadata#Managers/Members/%s/SecurityService/$entity"
#define MANAGER_HTTPS_CERT_URI "/redfish/v1/Managers/%s/SecurityService/HttpsCert"
#define MANAGER_HTTPS_CERT_METADATA "/redfish/v1/$metadata#Managers/Members/%s/SecurityService/HttpsCert/$entity"
#define ACTION_HTTPS_CERT_GEN_CSR "/redfish/v1/Managers/%s/SecurityService/HttpsCert/Actions/HttpsCert.GenerateCSR"
#define ACTION_HTTPS_CERT_IMPORT_SERVER_CERT \
    "/redfish/v1/Managers/%s/SecurityService/HttpsCert/Actions/HttpsCert.ImportServerCertificate"
#define ACTION_HTTPS_CERT_IMPORT_CUSTOM_CERT \
    "/redfish/v1/Managers/%s/SecurityService/HttpsCert/Actions/HttpsCert.ImportCustomCertificate"

#ifdef ARM64_HI1711_ENABLED
#define MANAGER_DICE_CERT_URI "/redfish/v1/Managers/%s/SecurityService/DiceCert"
#define MANAGER_DICE_CERT_METADATA "/redfish/v1/$metadata#Managers/Members/%s/SecurityService/DiceCert/$entity"
#define MANAGER_TPCM_URI "/redfish/v1/Managers/%s/SecurityService/TpcmService"
#endif

#define RFACTION_SYSLOG_IMPORT_ROOT_CERT \
    "/redfish/v1/Managers/%s/SyslogService/Actions/SyslogService.ImportRootCertificate"
#define RFACTION_SYSLOG_IMPORT_CLIENT_CERT \
    "/redfish/v1/Managers/%s/SyslogService/Actions/SyslogService.ImportClientCertificate"
#define RFACTION_SYSLOG_IMPORT_CRL "/redfish/v1/Managers/%s/SyslogService/Actions/SyslogService.ImportCrl"
#define RFACTION_SYSLOG_DELETE_CRL "/redfish/v1/Managers/%s/SyslogService/Actions/SyslogService.DeleteCrl"
#define RFACTION_SYSLOG_TEST "/redfish/v1/Managers/%s/SyslogService/Actions/SyslogService.SubmitTestEvent"
#define RFACTION_INFO_SYSLOG_IMPORT_ROOT_CERT "/redfish/v1/Managers/%s/SyslogService/ImportRootCertificateActionInfo"
#define RFACTION_INFO_SYSLOG_IMPORT_CLIENT_CERT \
    "/redfish/v1/Managers/%s/SyslogService/ImportClientCertificateActionInfo"
#define RFACTION_INFO_SYSLOG_IMPORT_CRL "/redfish/v1/Managers/%s/SyslogService/ImportCrlActionInfo"
#define RFACTION_INFO_SYSLOG_DELETE_CRL "/redfish/v1/Managers/%s/SyslogService/DeleteCrlActionInfo"
#define RFACTION_INFO_SYSLOG_TEST "/redfish/v1/Managers/%s/SyslogService/SubmitTestEventActionInfo"
#define RFACTION_LICENSE_SERVICE_INSTALL "/redfish/v1/Managers/%s/LicenseService/Actions/LicenseService.InstallLicense"
#define RFACTION_LICENSE_SERVICE_EXPORT "/redfish/v1/Managers/%s/LicenseService/Actions/LicenseService.ExportLicense"
#define RFACTION_LICENSE_SERVICE_REVOKE "/redfish/v1/Managers/%s/LicenseService/Actions/LicenseService.RevokeLicense"
#define RFACTION_LICENSE_SERVICE_DELETE "/redfish/v1/Managers/%s/LicenseService/Actions/LicenseService.DeleteLicense"
#define RFACTION_INFO_LICENSE_SERVICE_INSTALL "/redfish/v1/Managers/%s/LicenseService/InstallLicenseActionInfo"
#define RFACTION_INFO_LICENSE_SERVICE_EXPORT "/redfish/v1/Managers/%s/LicenseService/ExportLicenseActionInfo"
#define RFACTION_INFO_LICENSE_SERVICE_REVOKE "/redfish/v1/Managers/%s/LicenseService/RevokeLicenseActionInfo"
#define RFACTION_INFO_LICENSE_SERVICE_DELETE "/redfish/v1/Managers/%s/LicenseService/DeleteLicenseActionInfo"
#define IMPORT_SYSLOG_CRL_TMP_PATH "/tmp/syslog_tmp.crl"
#define IMPORT_SYSLOG_CRL_TRANSFER_FILEINFO_CODE 0x31
#define IMPORT_SYSLOG_CRL_TASK_NAME "syslog crl import"

#define MANAGER_LICENSE_SERVICE_URI "/redfish/v1/Managers/%s/LicenseService"
#define RFPROP_LICENSE_SERVICE_INSTALL "LicenseService.InstallLicense"
#define RFPROP_LICENSE_SERVICE_EXPORT "LicenseService.ExportLicense"
#define RFPROP_LICENSE_SERVICE_REVOKE "LicenseService.RevokeLicense"
#define RFPROP_LICENSE_SERVICE_DELETE "LicenseService.DeleteLicense"
#define RFPROP_LICSVC_PARAM_FILE_SOURCE "FileSource"
#define RFPROP_LICSVC_CAPABILITY "Capability"
#define RFPROP_LICSVC_DEVICE_ESN "DeviceESN"
#define RFPROP_LICSVC_INSTALLED_STATUS "InstalledStatus"
#define RFPROP_LICSVC_INSTALLED_RESULT "InstalledResult"
#define RFPROP_LICSVC_REVOKE_TICKET "RevokeTicket"
#define RFPROP_LICSVC_CLASS "LicenseClass"
#define RFPROP_LICSVC_STATUS "LicenseStatus"
#define RFPROP_LICSVC_ADVANCED_FEATURE "AdvancedFeature"
#define RFPROP_LICSVC_LICENSE_INFO "LicenseInfo"
#define RFPROP_LICSVC_FORMAT_VERSION "FileFormatVersion"
#define RFPROP_LICSVC_GENERAL_INFO "GeneralInfo"
#define RFPROP_LICSVC_COPY_RIGHT "CopyRight"
#define RFPROP_LICSVC_LSN "LSN"
#define RFPROP_LICSVC_LIC_TYPE "LicenseType"
#define RFPROP_LICSVC_GRACE_DAY "GraceDay"
#define RFPROP_LICSVC_CREATOR "Creator"
#define RFPROP_LICSVC_ISSUER "Issuer"
#define RFPROP_LICSVC_CREATE_TIME "CreateTime"
#define RFPROP_LICSVC_CUSTOMER_INFO "CustomerInfo"
#define RFPROP_LICSVC_CUSTOMER "Customer"
#define RFPROP_LICSVC_COUNTRY "Country"
#define RFPROP_LICSVC_OFFICE "Office"
#define RFPROP_LICSVC_NODE_INFO "NodeInfo"
#define RFPROP_LICSVC_NODE_NAME "Name"
#define RFPROP_LICSVC_NODE_ESN "ESN"
#define RFPROP_LICSVC_SALE_INFO "SaleInfo"
#define RFPROP_LICSVC_OFFER_PRODUCT "OfferingProduct"
#define RFPROP_LICSVC_PRODUCT_NAME "ProductName"
#define RFPROP_LICSVC_SALE_ITEM "SaleItem"
#define RFPROP_LICSVC_SALE_NAME "Name"
#define RFPROP_LICSVC_SALE_VALUE "Value"
#define RFPROP_LICSVC_SALE_VALID_DATE "ValidDate"
#define RFPROP_LICSVC_SALE_CONTROL_VALUE "IsControlled"
#define RFPROP_LICSVC_SALE_DESCRIPTION "Description"
#define RFPROP_LICSVC_FEATURE_KEY "FeatureKey"
#define RFPROP_LICSVC_FEATURE_NAME "Name"
#define RFPROP_LICSVC_FEATURE_GROUP "Group"
#define RFPROP_LICSVC_FEATURE_VALUE "Value"
#define RFPROP_LICSVC_FEATURE_VALID_DATE "ValidDate"
#define RFPROP_LICSVC_FEATURE_NO_CONTROL "IsControlled"
#define RFPROP_LICSVC_FEATURE_STATE "State"
#define RFPROP_LICSVC_FEATURE_CONTROL_VALUE "ControlValue"
#define RFPROP_LICSVC_FEATURE_REMOTE_CONTROL_VALUE "RemoteControlValue"
#define RFPROP_LICSVC_ALARM_INFO "AlarmInfo"
#define RFPROP_LICSVC_ALARM_REMAIN_GRACE_DAY "RemainGraceDay"
#define RFPROP_LICSVC_ALARM_REMAIN_COMM_DAY "RemainCommissioningDay"
#define RFPROP_LICSVC_ALARM_PRODUCT_ESN_VALID "ProductESNValidState"
#define RFPROP_LICSVC_ALARM_FILE_SATE "FileState"
#define RFPROP_LICSVC_ALARM_PRODUCT_ESN_MATCH "ProductESNMatchState"
#define RFPROP_LICSVC_ALARM_PRODUCT_VERSION_MATCH "ProductVersionMatchState"
#define RFPROP_LICSVC_KEY_INFO "KeyInfo"



#define ACTION_HTTPS_CERT_EXPORT_CSR "/redfish/v1/Managers/%s/SecurityService/HttpsCert/Actions/HttpsCert.ExportCSR"



#define URI_FOMRAT_ACTION_URI "%s/Actions/%s"
#define URI_FORMAT_MANAGER_ACTION "/redfish/v1/Managers/%s/Actions/%s"



#define RFPROP_MANAGER_ACTIONOEM_DUMP_ACTION_INFO "/DumpActionInfo"
#define RFPROP_MANAGER_ACTIONOEM_EXPORT_CONFIG_ACTION_INFO "/ExportConfigurationActionInfo"
#define RFPROP_MANAGER_ACTIONOEM_IMPORT_CONFIG_ACTION_INFO "/ImportConfigurationActionInfo"
#define RFPROP_MANAGER_ACTIONOEM_POWER_ON_ACTION_INFO "/PowerOnPermitActionInfo"
#define RFPROP_MANAGER_ACTIONOEM_DUMP_LOWER_ACTION_INFO "dumpactioninfo"
#define RFPROP_MANAGER_ACTIONOEM_EXPORT_CONFIG_LOWER_ACTION_INFO "exportconfigurationactioninfo"
#define RFPROP_MANAGER_ACTIONOEM_IMPORT_CONFIG_LOWER_ACTION_INFO "importconfigurationactioninfo"
#define RFPROP_MANAGER_ACTIONOEM_POWER_ON_LOWER_ACTION_INFO "poweronpermitactioninfo"

#define URI_PATTERN_MANAGER_RESET_ACTIONINFO "^/redfish/v1/Managers/[a-zA-Z0-9]+/ResetActionInfo$"
#define RSC_PATH_MANAGER_RESET_ACTIONINFO "/redfish/v1/managers/1/resetactioninfo"
#define URI_PATTERN_MANAGER_ROLL_BACK_ACTIONINFO "^/redfish/v1/Managers/[a-zA-Z0-9]+/RollBackActionInfo$"
#define RSC_PATH_MANAGER_ROLLBACK_ACTIONINFO "/redfish/v1/managers/1/rollbackactioninfo"
#define URI_PATTERN_HTTPS_CERT_GEN_CSR_ACTIONINFO \
    "^/redfish/v1/Managers/[a-zA-Z0-9]+/SecurityService/HttpsCert/GenerateCSRActionInfo$"
#define RSC_PATH_HTTPS_CERT_GEN_CSR_ACTIONINFO "/redfish/v1/managers/1/securityservice/httpscert/generatecsractioninfo"
#define URI_PATTERN_HTTPS_CERT_EXPORT_CSR_ACTIONINFO \
    "^/redfish/v1/Managers/[a-zA-Z0-9]+/SecurityService/HttpsCert/ExportCSRActionInfo$"
#define RSC_PATH_HTTPS_CERT_EXPORT_CSR_ACTIONINFO "/redfish/v1/managers/1/securityservice/httpscert/exportcsractioninfo"
#define URI_PATTERN_HTTPS_CERT_IMPORT_CERT_ACTIONINFO \
    "^/redfish/v1/Managers/[a-zA-Z0-9]+/SecurityService/HttpsCert/ImportServerCertificateActionInfo$"
#define RSC_PATH_HTTPS_CERT_IMPORT_CERT_ACTIONINFO \
    "/redfish/v1/managers/1/securityservice/httpscert/importservercertactinfo"
#define URI_PATTERN_HTTPS_CERT_IMPORT_CUST_CERT_ACTIONINFO \
    "^/redfish/v1/Managers/[a-zA-Z0-9]+/SecurityService/HttpsCert/ImportCustomCertificateActionInfo$"
#define RSC_PATH_HTTPS_CERT_IMPORT_CUST_CERT_ACTIONINFO \
    "/redfish/v1/managers/1/securityservice/httpscert/importcustcertactinfo"
#define URI_PATTERN_SYSTEM_RESET_ACTIONINFO "^/redfish/v1/Systems/[a-zA-Z0-9]+/ResetActionInfo$"
#define RSC_PATH_SYSTEM_RESET_ACTIONINFO "/redfish/v1/systems/1/resetactioninfo"
#define URI_PATTERN_SYSTEM_FRU_CONTROL_ACTIONINFO "^/redfish/v1/Systems/[a-zA-Z0-9]+/FruControlActionInfo$"
#define RSC_PATH_SYSTEM_FRU_CONTROL_ACTIONINFO "/redfish/v1/systems/1/frucontrolactioninfo"
#define URI_PATTERN_STORAGE_RESTORE_CTRLER_DFLT_SET_ACTIONNINFO \
    "(?i)^/redfish/v1/Systems/[a-zA-Z0-9]+/Storages/[a-zA-Z0-9]+/RestoreStorageControllerDefaultSettingsActionInfo$"
#define URI_PATTERN_STORAGE_IMPORT_FOREIGN_CONFIG_ACTIONNINFO \
    "(?i)^/redfish/v1/Systems/[a-zA-Z0-9]+/Storages/[a-zA-Z0-9]+/ImportForeignConfigActionInfo$"
#define URI_FORMAT_RAIDSTORAGE_IMPORT_FOREIGN_CONFIG_ACTIONINFO \
    "/redfish/v1/Systems/%s/Storages/RAIDStorage%d/ImportForeignConfigActionInfo"
#define URI_PATTERN_UPDT_SVC_SIMPLE_UPDT_ACTIONINFO "^/redfish/v1/UpdateService/SimpleUpdateActionInfo$"
#define RSC_PATH_STORAGE_IMPORT_FOREIGN_CONFIG_ACTIONNINFO \
    "/redfish/v1/systems/1/storages/template/importforeignconfigactinfo"
#define URI_PATTERN_STORAGE_CLEAR_FOREIGN_CONFIG_ACTIONNINFO \
    "(?i)^/redfish/v1/Systems/[a-zA-Z0-9]+/Storages/[a-zA-Z0-9]+/ClearForeignConfigActionInfo$"
#define URI_FORMAT_RAIDSTORAGE_CLEAR_FOREIGN_CONFIG_ACTIONINFO \
    "/redfish/v1/Systems/%s/Storages/RAIDStorage%d/ClearForeignConfigActionInfo"
#define RSC_PATH_STORAGE_CLEAR_FOREIGN_CONFIG_ACTIONNINFO \
    "/redfish/v1/systems/1/storages/template/clearforeignconfigactinfo"

#define URI_FORMAT_RAIDSTORAGE_RESTORE_DFLT_SET_ACTIONINFO \
    "/redfish/v1/Systems/%s/Storages/RAIDStorage%d/RestoreStorageControllerDefaultSettingsActionInfo"


#define RSC_PATH_STORAGE_RESTORE_CTRLER_DFLT_SET_ACTIONNINFO \
    "/redfish/v1/systems/1/storages/template/restoredefaultsettingsactinfo"
#define URI_PATTERN_UPDT_SVC_SIMPLE_UPDT_ACTIONINFO "^/redfish/v1/UpdateService/SimpleUpdateActionInfo$"
#define RSC_PATH_UPDT_SVC_SIMPLE_UPDT_ACTIONINFO "/redfish/v1/updateservice/simpleupdateactioninfo"


#define URI_PATTERN_SYSTEM_ADD_SWI_IPV6ADDR_ACTIONINFO "^/redfish/v1/Systems/[a-zA-Z0-9]+/AddSwIpv6AddrActionInfo"
#define RSC_PATH_SYSTEM_ADD_SWI_IPV6ADDR_ACTIONINFO "/redfish/v1/systems/1/addswiipv6addractioninfo"
#define URI_PATTERN_SYSTEM_DEL_SWI_IPV6ADDR_ACTIONINFO "^/redfish/v1/Systems/[a-zA-Z0-9]+/DelSwIpv6AddrActionInfo"
#define RSC_PATH_SYSTEM_DEL_SWI_IPV6ADDR_ACTIONINFO "/redfish/v1/systems/1/delswiipv6addractioninfo"
#define URI_PATTERN_SYSTEM_ADD_SWI_IPV6ROUTE_ACTIONINFO "^/redfish/v1/Systems/[a-zA-Z0-9]+/AddSwIpv6RouteActionInfo"
#define RSC_PATH_SYSTEM_ADD_SWI_IPV6ROUTE_ACTIONINFO "/redfish/v1/systems/1/addswiipv6routeactioninfo"
#define URI_PATTERN_SYSTEM_DEL_SWI_IPV6ROUTE_ACTIONINFO "^/redfish/v1/Systems/[a-zA-Z0-9]+/DelSwIpv6RouteActionInfo"
#define RSC_PATH_SYSTEM_DEL_SWI_IPV6ROUTE_ACTIONINFO "/redfish/v1/systems/1/delswiipv6routeactioninfo"


#ifdef ARM64_HI1711_ENABLED
#define URI_PATTERN_DICE_CERT_EXPORT_CSR_ACTIONINFO \
    "^/redfish/v1/Managers/[a-zA-Z0-9]+/SecurityService/DiceCert/ExportDiceCSRActionInfo$"
#define RSC_PATH_DICE_CERT_EXPORT_CSR_ACTIONINFO \
    "/redfish/v1/managers/1/securityservice/dicecert/exportdicecsractioninfo"
#define URI_PATTERN_DICE_CERT_IMPORT_CERT_ACTIONINFO \
    "^/redfish/v1/Managers/[a-zA-Z0-9]+/SecurityService/DiceCert/ImportDiceCertificateActionInfo$"
#define RSC_PATH_DICE_CERT_IMPORT_CERT_ACTIONINFO \
    "/redfish/v1/managers/1/securityservice/dicecert/importdicecertificateactioninfo"
#define URI_PATTERN_DICE_CERT_EXPORT_CERT_CHAIN_ACTIONINFO \
    "^/redfish/v1/Managers/[a-zA-Z0-9]+/SecurityService/DiceCert/ExportDiceCertficateChainActionInfo$"
#define RSC_PATH_DICE_CERT_EXPORT_CERT_CHAIN_ACTIONINFO \
    "/redfish/v1/managers/1/securityservice/dicecert/exportdicecertficatechainactioninfo"
#endif


#define URI_SPECIAL_BLADE_PREFIX    "^/redfish/v1/chassis/blade[0-9]+"
#define URI_PATTERN_CHASSIS_BLADE_POWER "^/redfish/v1/chassis/blade[0-9]+/power$"





#define RFPROP_SYSTEM_LINKS "Links"
#define RFPROP_SYSTEM_CHASSIS "Chassis"
#define RFPROP_SYSTEM_MANAGERS "Managers"
#define PSLOT_URI_TARGET "target"
#define PSLOT_URI_RALLOW "ResetType@Redfish.AllowableValues"
#define PSLOT_URI_CTESET "#ComputerSystem.Reset"
#define RFPROP_SYSTEM_ACTIONS "Actions"

#define RFPROP_SYSTEM_RESET "ComputerSystem.Reset"
#define RF_ACTION_ON "On"
#define RF_ACTION_FORCEOFF "ForceOff"
#define RF_ACTION_GRACEFULSHUTDOWN "GracefulShutdown"
#define RFPROP_SYSTEM_FRUCONTROL "Oem/Huawei/ComputerSystem.FruControl"
#define RFPROP_SYSTEM_ADD_SWI_IPV6ADDR "Oem/Huawei/ComputerSystem.AddSwiIpv6Addr"
#define RFPROP_SYSTEM_ADD_SWI_IPV6ROUTE "Oem/Huawei/ComputerSystem.AddSwiIpv6Route"
#define RFPROP_SYSTEM_DEL_SWI_IPV6ADDR "Oem/Huawei/ComputerSystem.DelSwiIpv6Addr"
#define RFPROP_SYSTEM_DEL_SWI_IPV6ROUTE "Oem/Huawei/ComputerSystem.DelSwiIpv6Route"
#define RFPROP_SYSTEM_IPV6ADDR "Address"
#define RFPROP_SYSTEM_IPV6PREFIX "PrefixLength"
#define RFPROP_SYSTEM_IPV6GATEWAY "Gateway"


#define RFPROP_CLEAR_NETWORK_USAGERATE "Oem/Huawei/ComputerSystem.ClearNetworkHistoryUsageRate"
#define RFPROP_SYSTEM_FRUCTL "ComputerSystem.FruControl"
#define RFPROP_CLEAR_NETWORK_USAGE "ComputerSystem.ClearNetworkHistoryUsageRate"
#define RFPROP_FRUID "FruID"
#define RFPROP_FRUCONTROL_TYPE "FruControlType"
#define PSLOT_URI_FRUCONTROL "#ComputerSystem.FruControl"
#define RF_ACTION_FRU_POWERON "On"
#define RF_ACTION_FRU_POWEROFF "GracefulShutdown"
#define RF_ACTION_FRU_POWERRESET "ForceRestart"
#define RF_ACTION_FRU_POWERDIAGRESET "Nmi"
#define RF_ACTION_FRU_POWERCYCLE "ForcePowerCycle"
#define PSLOT_URI_FRU_RALLOW "ControlType@Redfish.AllowableValues"



#define RF_SYSTEMS_URI "/redfish/v1/Systems/"
#define RFPROP_SYSTEM_RESTORAGE "/redfish/v1/Systems/%s/Storages/1"
#define URI_FORMAT_SYS_ETH "/redfish/v1/Systems/%s/EthernetInterfaces/%s"
#define URI_FORMAT_SYS_ETH_2 "/redfish/v1/Systems/%s/EthernetInterfaces/%sPort%s"
#define URI_FORMAT_SYS_ETH_ACINFO "/redfish/v1/Systems/%s/EthernetInterfaces/%s/ConfigureActionInfo"
#define RFPROP_SYSTEM_ETHS "/redfish/v1/Systems/%s/EthernetInterfaces"
#define RFPROP_SYSTEM_REMEMORY "/redfish/v1/Systems/%s/Memory"
#define RFPROP_SYSTEM_REMENETWORK "/redfish/v1/Systems/%s/NetworkInterfaces"
#define RFPROP_SYSTEM_REMEMORYSS "/redfish/v1/Systems/%s/Memory/%s%s"
#define URI_FORMAT_SYSTEM_REMEMORYS "/redfish/v1/Systems/%s/Memory/%s"
#define URI_FORMAT_REMEMORY_METRICS "/Memory/%s%s/MemoryMetrics"
#define URI_FORMAT_SYSTEM_REMEMORY_METRICS "/redfish/v1/Systems/%s/Memory/%s%s/MemoryMetrics"
#define URI_FORMAT_SYSTEM_REPROCESSORS "/redfish/v1/Systems/%s/Processors/%d"
#define URI_FORMAT_SYSTEM_REPROCESSORS_XPU "/redfish/v1/Systems/%s/Processors/%s"
#define MEMBER_ID_FORMAT_SYSTEM_REPROCESSORS_GPU "Gpu%u"
#define RFPROP_SYSTEM_REPROCESSOR "/redfish/v1/Systems/%s/Processors"
#define RFPROP_SYSTEM_REPROCESSORD "/redfish/v1/Systems/%s/Processors/%d"

#define RFPROP_SYSTEM_REPROCESSOR_VIEW "/redfish/v1/Systems/%s/ProcessorView"
#define RFPROP_SYSTEM_MEMORY_VIEW "/redfish/v1/Systems/%s/MemoryView"

#define URI_FORMAT_SYS_BONDINGS "/redfish/v1/Systems/%s/NetworkBondings"
#define URI_FORMAT_SYS_BONDING "/redfish/v1/Systems/%s/NetworkBondings/%s"
#define URI_FORMAT_SYS_BONDING_ACTION_INFO "/redfish/v1/Systems/%s/NetworkBondings/%s/ConfigureActionInfo"
#define URI_FORMAT_SYS_BONDING_ACTION_URL "/redfish/v1/Systems/%s/NetworkBondings/%s/Actions/NetworkBonding.Configure"
#define URI_FORMAT_SYS_IBS "/redfish/v1/Systems/%s/InfiniBandInterfaces"
#define URI_FORMAT_SYS_IB "/redfish/v1/Systems/%s/InfiniBandInterfaces/%s"
#define URI_FORMAT_SYS_IB_2 "/redfish/v1/Systems/%s/InfiniBandInterfaces/%sPort%s"




#define URI_FORMAT_SYS_BRIDGE_COLLECTION "/redfish/v1/Systems/%s/NetworkBridge"
#define URI_FORMAT_SYS_BRIDGE "/redfish/v1/Systems/%s/NetworkBridge/%s"


#define URI_FORMAT_SYS_PROCESSORS_HISTORY_USAGE_RATE "/redfish/v1/Systems/%s/ProcessorsHistoryUsageRate"
#define URI_FORMAT_SYS_MEMORY_HISTORY_USAGE_RATE "/redfish/v1/Systems/%s/MemoryHistoryUsageRate"
#define URI_FORMAT_SYS_NETWORK_HISTORY_USAGE_RATE "/redfish/v1/Systems/%s/NetworkHistoryUsageRate"


#define RF_ACTION_FORCERESTART "ForceRestart"
#define RF_ACTION_NMI "Nmi"
#define RF_ACTION_FORCE_POWER_CYCLE "ForcePowerCycle"


#define RFPROP_MANAGERCOLLECTION_ID "Id"
#define RFPROP_MANAGERCOLLECTION_NAME "Name"
#define RFPROP_MANAGERCOLLECTION_COUNT "Members@odata.count"
#define RFPROP_MANAGERCOLLECTION_MEMBERS "Members"



#define RFPROP_NETWORKPROTOCOL_HOSTNAME "HostName"
#define RFPROP_NETWORKPROTOCOL_FQDN "FQDN"

#define RFPROP_NETWORKPROTOCOL_SSH "SSH"
#define RFPROP_NETWORKPROTOCOL_SSDP "SSDP"
#define RFPROP_NETWORKPROTOCOL_IPMI "IPMI"
#define RFPROP_NETWORKPROTOCOL_SNMP "SNMP"
#define RFPROP_NETWORKPROTOCOL_KVMIP "KVMIP"
#define RFPROP_NETWORKPROTOCOL_VMM "VirtualMedia"
#define RFPROP_NETWORKPROTOCOL_STATE "ProtocolEnabled"
#define RFPROP_NETWORKPROTOCOL_PORTID "Port"
#define RFPROP_NETWORKPROTOCOL_NMIS "NotifyMulticastIntervalSeconds"
#define RFPROP_NETWORKPROTOCOL_NTTL "NotifyTTL"
#define RFPROP_NETWORKPROTOCOL_NIPS "NotifyIPv6Scope"
#define RFPROP_NETWORKPROTOCOL_VNC "VNC"
#define ERROR_VNC "Oem/Huawei/VNC"
#define ERROR_VNC_PORT "Oem/Huawei/VNC/Port"
#define CHECK_VNC_PORT_EXIST_ERR 6
#define CHECK_VNC_PORT_RANGE_ERR 7
#define PROPERTY_MANAGER_VNCPORTENABLED "Oem/Huawei/VNC/ProtocolEnabled"
#define RFPROP_NETWORKPROTOCOL_NOTIFY_ENABLED "NotifyEnabled"




#define RFPROP_NETWORKPROTOCOL_VIDEO "Video"
#define RFPROP_NETWORKPROTOCOL_NAT "NAT"
#define RFPROP_RMCP_ENALBED "RMCPEnabled"
#define RFPROP_RMCP_PLUS_ENABLED "RMCPPlusEnabled"
#define RFPROP_IPMI_PORT1 "Port1"
#define RFPROP_IPMI_PORT2 "Port2"

#define ERROR_VIDEO "Oem/Huawei/Video"
#define ERROR_VIDEO_PORT "Oem/Huawei/Video/Port"
#define ERROR_VIDEO_ENABLED "Oem/Huawei/Video/ProtocolEnabled"
#define ERROR_NAT "Oem/Huawei/NAT"
#define ERROR_NAT_PORT "Oem/Huawei/NAT/Port"
#define ERROR_NAT_ENABLED "Oem/Huawei/NAT/ProtocolEnabled"
#define ERROR_IPMI "Oem/Huawei/IPMI"
#define ERROR_IPMI_PORT_1 "Oem/Huawei/IPMI/Port1"
#define ERROR_IPMI_PORT_2 "Oem/Huawei/IPMI/Port2"
#define ERROR_IPMI_RMCP_ENABLED "Oem/Huawei/IPMI/RMCPEnabled"
#define ERROR_IPMI_RMCP_PLUS_ENABLED "Oem/Huawei/IPMI/RMCPPlusEnabled"
#define ERROR_SSDP_NOTIFY_ENABLED "Oem/Huawei/SSDP/NotifyEnabled"




#define RF_SMS "/redfish/v1/Sms/"
#define RF_UPDATESVC "UpdateService"
#define RF_SMSUPDATESVC "SmsUpdateService"
#define RF_DASVC "DataAcquisitionService"




#define RFPROP_SESSIONSERVICE_ID "Id"
#define RFPROP_SESSIONSERVICE_NAME "Name"
#define RFPROP_SESSIONSERVICE_TIMEOUT "SessionTimeout" 
#define RFPROP_SESSIONSERVICE_SESSIONS "Sessions"
#define RFPROP_SESSIONSERVICE_WEBTIMEOUT "WebSessionTimeoutMinutes"
#define RFPROP_SESSIONSERVICE_WEBMODE "WebSessionMode"

#define RFACTION_SESSION_REFRESH_ACTIVATION_STATE "Session.RefreshActivationState"
#define RFACTION_SESSION_REFRESH_ACTIVATION_STATE_URI "/redfish/v1/SessionService/Sessions/%s/Oem/Huawei/Actions/%s"
#define RFPROP_SESSION_ACTIVATE_MODE "Mode"
#define RF_SESSION_REFRESH_ACTIVATION_STATE_ACTION_INFO \
    "/redfish/v1/SessionService/Sessions/%s/RefreshActivationStateActionInfo"
#define SESSION_ACTIVATE "Activate"
#define SESSION_DEACTIVATE "Deactivate"

#define RF_CHASSISOVERVIEW_URI "/redfish/v1/ChassisOverview"
#define RF_CHASSISOVERVIEW "ChassisOverview"
#define RF_SYSTEMOVERVIEW_URI "/redfish/v1/SystemOverview"
#define RF_SYSTEMOVERVIEW "SystemOverview"





#define RFPROP_ACCOUNT_ODATA_ID "@odata.id"
#define RFPROP_ACCOUNT_ID "Id"
#define RFPROP_ACCOUNT_USERNAME "UserName"
#define RFPROP_ACCOUNT_PASSWORD "Password"
#define RFPROP_ACCOUNT_ROLE_ID "RoleId"
#define RFPROP_ACCOUNT_LOCKED "Locked"
#define RFPROP_ACCOUNT_ENABLED "Enabled"
#define RF_ACCOUNT_FIRST_LOGIN_PLCY "FirstLoginPolicy"
#define RFPROP_ACCOUNT_LINKS "Links"
#define RF_ACCOUNT_LINKS_ROLE "Role"
#define RFPROP_ACCOUNT_OEM "Oem"
#define RFPROP_ACCOUNT_HUAWEI "Huawei"
#define RF_ACCOUNT_MACLICERT_ELEMENT_MAXLEN 128


#define RF_LINKS_MAXURI_LEN 50
#define USERID_MAX_LEN 2
#define ACCOUNTS_URI_LEN 35 // uri为/redfish/v1/AccountService/Accounts
#define USER_NAME_MAX_LEN_STR "16"
#define USER_PWD_MAX_LEN_STR "20"
#define USER_PASSWORD_MIN_LEN_STR "1"


#define BIOS_PASSWORD_MIN_LEN_STR                   "0"
#define BIOS_PASSWORD_MAX_LEN_STR                   "32"




#define RFPROP_ROLE_ISPREDEFINED "IsPredefined"
#define RFPROP_ROLE_ASSIGNEDPRIVILEGES "AssignedPrivileges"
#define RFPROP_ROLE_OEMPRIVILEGES "OemPrivileges"


#define RF_VALUE_LOGIN "Login"
#define RF_VALUE_CONFIGURE_MANAGER "ConfigureManager"
#define RF_VALUE_CONFIGURE_USERS "ConfigureUsers"
#define RF_VALUE_CONFIGURE_SELF "ConfigureSelf"
#define RF_VALUE_KVM "OemKvm"
#define RF_VALUE_VMM "OemVmm"
#define RF_VALUE_SECURITY_SETTINGS "OemSecurityMgmt"
#define RF_VALUE_POWER_CONTROL "OemPowerControl"
#define RF_VALUE_DIAGNOSIS "OemDiagnosis"


#define RF_VALUE_CONFIGURE_COMPONENTS "ConfigureComponents"


#define RF_INDEX_LOGIN 0
#define RF_INDEX_CONFIGURE_SELF 1
#define RF_INDEX_CONFIGURE_MANAGER 2
#define RF_INDEX_CONFIGURE_USERS 3
#define RF_INDEX_KVM 4
#define RF_INDEX_VMM 5
#define RF_INDEX_SECURITY_SETTINGS 6
#define RF_INDEX_POWER_CONTROL 7
#define RF_INDEX_DIAGNOSIS 8


#define PROPERTY_USERROLE_ROLENAME "RoleName"
#define RF_ROLESTR_LEN 16
#define RF_ENABLED_PRIV 1




#define RFPROP_ETH_PERMANENT_MACADDR "PermanentMACAddress"
#define RFPROP_ETH_MACADDR "MACAddress"
#define RFPROP_SYSTEM_VLAN_PRIORITY "PriorityEnabled"
#define RFPROP_ETH_IPV6_GATEWAY "IPv6DefaultGateway"
#define RFPROP_ETH_SPEED "SpeedMbps"
#define RFPROP_ETH_FULLDUPLEX "FullDuplex"
#define RFPROP_ETH_AUTONEG "AutoNeg"

#define RFPROP_INTERFACE_TYPE "InterfaceType"
#define RFPROP_USAGE_THRE "BandwidthUsageThre"
#define ETHS_URI_LEN 40 // uri为/redfish/v1/Systems/1/EthernetInterfaces
#define RFPROP_VAL_DISABLE "Disable"
#define RFPROP_VAL_ENABLE "Enable"
#define RF_MAX_VLAN_ID 4094 // VLanNetworkInterface.v1_1_3.json 中定义的vlan id最大值是 4094
#define RFPROP_ETH_BDF "BDF"
#define RFPROP_ETH_NETWORK_ADAPTER_NAME "NetworkAdapterName"
#define RFPROP_ETH_RELATED_PORT "RelatedPort"
#define RFPROP_SYSTEM_ETHERNET_CONFIGURE "Oem/Huawei/EthernetInterface.Configure"
#define RFPROP_SYSTEM_ETHERNET_VLAN_CONFIGURE "Oem/Huawei/VLanNetworkInterface.Configure"

#define FIXED_ETH_NAME "System Ethernet Interface"



#define RF_MSGDIS_BMA_NOT_PRESENT "Operation failed with BMA disconnect"

typedef struct tag_config_ip_task_monitor_info {
    OBJ_HANDLE obj_handle;                 // 承载类方法的句柄
    guint32 free_state_flag;               // 类方法返回的可用的标志位
    const gchar *state_prop;               // 查询任务进度的属性
    const gchar *result_prop;              // 查询任务结果描述的属性
    gchar rsc_url[MAX_URL_LEN];            // 资源的URL
    gchar operation_name[PROP_VAL_LENGTH]; // 操作的名字
    const gchar *rf_method;
} CONFIG_IP_TASK_MONITOR_INFO_S;

#define MAX_FILENAME_LENTH 1024

#define RFPROP_BONDING_BRIDGE_WORKMODE "WorkMode"
#define RFPROP_BRIDGE_TYPE "BridgeType"
#define RFPROP_BRIDGE_INTERFACES "BridgeInterfaces"
// bond资源的宏
#define RFPROP_BONDING_TYPE "BondingType"
#define RFPROP_BONDED_INTERFACES "BondedInterfaces"
#define RFPROP_RELATED_INTERFACE "RelatedInterface"
#define RFACTION_BONDING_CONFIGURE "NetworkBonding.Configure"


#define RFPROP_STORAGE_CONTROLLER_ID "MemberId"
#define RFPROP_STORAGE_CONTROLLER_STATUS "Status"
#define RFPROP_STORAGE_CONTROLLER_SPEED_GBPS "SpeedGbps"
#define RFPROP_STORAGE_CONTROLLER_FW_VERSION "FirmwareVersion"

#define RFPROP_STORAGE_CONTROLLER_MAX_STRIPE "MaxStripeSizeBytes"
#define RFPROP_STORAGE_CONTROLLER_MIN_STRIPE "MinStripeSizeBytes"
#define RFPROP_STORAGE_CONTROLLER_DRIVER_NAME "DriverName"
#define RFPROP_STORAGE_CONTROLLER_DRIVER_VERSION "DriverVersion"
#define RFPROP_STORAGE_CONTROLLER_MODEL "Model"

#define RFPROP_STORAGE_CONTROLLER_MANUFACTURER "Manufacturer"
#define RFPROP_STORAGE_CONTROLLER_CARD_MANUFACTURER "CardManufacturer"
#define RFPROP_STORAGE_CONTROLLER_CARD_MODEL "CardModel"
#define RFPROP_STORAGE_CONTROLLER_SERIALNUMBER          "SerialNumber"
#define RFPROP_STORAGE_CONTROLLER_DDRECCCOUNT "DDRECCCount"
#define RFPROP_STORAGE_CONTROLLER_PHYSTATUS "PHYStatus"
#define RFPROP_STORAGE_PHYSTATUS_PHYID "PHYId"
#define RFPROP_STORAGE_PHYSTATUS_INVALID_DWORD_COUNT "InvalidDwordCount"
#define RFPROP_STORAGE_PHYSTATUS_LOSS_DWORD_COUNT "LossDwordSyncCount"
#define RFPROP_STORAGE_PHYSTATUS_RESET_PROBLEM_COUNT "PHYResetProblemCount"
#define RFPROP_STORAGE_PHYSTATUS_RUNNING_DIS_ERR_COUNT "RunningDisparityErrorCount"
#define RFPROP_STORAGE_DRIVER_COUNT "Drives@odata.count"
#define RFPROP_STORAGE_DRIVERS "Drives"
#define RFPROP_STORAGE_CONTROLLERS_COUNT "StorageControllers@odata.count"
#define RFPROP_STORAGE_CONTROLLERS "StorageControllers"
#define RFPROP_STORAGE_CONTROLLER_DESCRIPTION "Description"

#define RFPROP_STORAGE_CONTROLLER_DEVICE_PROTOCOL "SupportedDeviceProtocols"
#define RFPROP_STORAGE_CONTROLLER_CONTROLLER "Controller"


#define RFPROP_STORAGE_CONTROLLER_SUPPORT_RAIDLEVEL "SupportedRAIDLevels"

#define REFROP_STORAGE_CONYTOLLER_SUPPORT_MODE  "SupportedModes"

#define RFPROP_STORAGE_CONTROLLER_ASSOCIATED_CARD "AssociatedCard"

#define RFPROP_STORAGE_CONTROLLER_TYPE "Type"
#define RFPROP_STORAGE_CONTROLLER_SASADDRESS "SASAddress"
#define RFPROP_STORAGE_CONTROLLER_CFG_VERSION "ConfigurationVersion"
#define RFPROP_STORAGE_CONTROLLER_MEMORY_SIZE "MemorySizeMiB"
#define RFPROP_STORAGE_CONTROLLER_FAULT_HISTORY "MaintainPDFailHistory"
#define RFPROP_STORAGE_CONTROLLER_COPYBACK "CopyBackState"
#define RFPROP_STORAGE_CONTROLLER_SMART_COPYBACK "SmarterCopyBackState"
#define RFPROP_STORAGE_CONTROLLER_JBOD "JBODState"
#define RFPROP_STORAGE_CONTROLLER_JBOD_STATE_SUPPORTED  "JbodStateSupported"
#define RFPROP_STORAGE_CONTROLLER_EPD_SUPPORTED         "EpdSupported"
#define RFPROP_STORAGE_CONTROLLER_CAPACITANCE_STATUS "CapacitanceStatus"
#define RFPROP_STORAGE_CONTROLLER_DRIVER_INFO "DriverInfo"
#define RFPROP_STORAGE_CONTROLLER_DRIVER_NAME "DriverName"
#define RFPROP_STORAGE_CONTROLLER_DRIVER_VERSION "DriverVersion"
#define RFPROP_STORAGE_CONTROLLER_PCIE_LINK_WIDTH        "PCIeLinkWidth"
#define RFPROP_STORAGE_CONTROLLER_NO_BATTERY_WRITE_CACHE "NoBatteryWriteCacheEnabled"
#define RFPROP_STORAGE_CONTROLLER_READ_CACHE_PERCENT     "ReadCachePercent"
#define RFPROP_STORAGE_CONTROLLER_CONFIG_DRIVE           "ConfiguredDrive"
#define RFPROP_STORAGE_CONTROLLER_UNCONFIG_DRIVE         "UnconfiguredDrive"
#define RFPROP_STORAGE_CONTROLLER_HBA_DRIVE              "HBADrive"
#define RFPROP_STORAGE_CONTROLLER_WRITE_CACHE_POLICY     "WriteCachePolicy"
#define RFPROP_STORAGE_CONTROLLER_SPARE_ACTIVATION_MODE  "SpareActivationMode"
#define REPROP_STORAGE_CONTROLLER_BOOT_DEVICES    "BootDevices"
#define RFPROP_STORAGE_CONTROLLERS_RESTORE "Oem/Huawei/Storage.RestoreStorageControllerDefaultSettings"
#define RFPROP_STORAGE_CONTROLLERS_IMPORT_FOREIGN_CONFIG "Oem/Huawei/Storage.ImportForeignConfig"
#define RFPROP_STORAGE_CONTROLLERS_CLEAR_FOREIGN_CONFIG "Oem/Huawei/Storage.ClearForeignConfig"

#define RFPROP_STORAGE_CONTROLLERS_CACHE_SUMMARY "CacheSummary"
#define RFPROP_STORAGE_CONTROLLERS_TOTAL_CACHE_SIZE "TotalCacheSizeMiB"
#define RFPROP_STORAGE_CONTROLLERS_SUPPORT_RAIDTYPES "SupportedRAIDTypes"



#define RFPROP_STORAGE_OOB_SUPPORT "OOBSupport"
#define RFPROP_STORAGE_BBU_NAME "CapacitanceName"


#define RFPROP_STORAGE_CONTROLLER_CC_INFO "VolumeConsistencyCheckConfig"
#define RFPROP_STORAGE_CONTROLLER_CC_ENABLED "Enabled"
#define RFPROP_STORAGE_CONTROLLER_CC_PERIOD "PeriodOfHours"
#define RFPROP_STORAGE_CONTROLLER_CC_RATE "Rate"
#define RFPROP_STORAGE_CONTROLLER_CC_REPAIR "AutoRepairEnabled"
#define RFPROP_STORAGE_CONTROLLER_CC_STATUS "RunningStatus"
#define RFPROP_STORAGE_CONTROLLER_CC_TOTALVD "TotalVolumeCounts"
#define RFPROP_STORAGE_CONTROLLER_CC_COMPLETEDVD "CompletedVolumeCounts"
#define RFPROP_STORAGE_CONTROLLER_CC_DELAY "DelayToStart"


#define RFPROP_STORAGE_CONTROLLER_SUPPORT_RIAD "VolumeSupported"
#define RFPROP_STORAGE_CONTROLLER_FAULT_DETAILS "FaultDetails"
#define RFPROP_STORAGE_CONTROLLER_CAPACITION_FAULT_DETAILS "FaultDetails"
#define RFPROP_STORAGE_CONTROLLER_READ_POLICY "ReadPolicy"
#define RFPROP_STORAGE_CONTROLLER_WRITE_POLICY "WritePolicy"
#define RFPROP_STORAGE_CONTROLLER_CACHE_POLICY "CachePolicy"
#define RFPROP_STORAGE_CONTROLLER_ACCESS_POLICY "AccessPolicy"
#define RFPROP_STORAGE_CONTROLLER_DRIVE_CACHE_POLICY "DriveCachePolicy"
#define RFPROP_STORAGE_CONTROLLER_READPOLICY_CONFIGURABLE "Configurable"
#define RFPROP_STORAGE_CONTROLLER_READPOLICY_SUPPORTED "SupportedPolicy"
#define MEMORY_CORRECTAB_ERRORS "Memory correctable errors,"
#define MEMORY_UNCORRECTAB_ERRORS "Memory uncorrectable errors,"
#define MEMORY_ECC_ERRORS "Memory ECC errors,"
#define NVRAM_UNCORRECTABLE_ERRORS "NVRAM uncorrectable errors,"
#define RAID_COMMUNICATION_LOSS "Raid communication loss,"
#define BBU_VOLTAGE_LOW "Voltage Low,"
#define BBU_NEEDS_TO_BE_REPLACED "Battery Needs To Be Replaced,"
#define BBU_LEARN_CYCLE_FAILED "Learn Cycle Failed,"
#define BBU_LEARN_CYCLE_TIMEOUT "Learn Cycle Timeout,"
#define BBU_PACK_IS_ABOUT_TO_FAIL "Pack is about to fail,"
#define BBU_REMAINING_CAPACITY_LOW "Remaining Capacity Low,"
#define BBU_NO_SPACE_FOR_CACHE_OFFLOAD "No Space for Cache Offload,"
#define RFPROP_STORAGE_CONTROLLER_ARRAY "RelatedArrayInfo"
#define RFPROP_STORAGE_CONTROLLER_ARRAY_DRIVE_FREE_SPACE "FreeSpaceMiBPerDrive"
#define RFPROP_STORAGE_CONTROLLER_TOTAL_FREE_SPACE "TotalFreeSpaceMiB"   
#define RFPROP_STORAGE_CONTROLLER_FREE_BLOCKS_SPACE "FreeBlocksSpaceMiB" 
#define BIT_OFFSET_ONE 1
#define BIT_OFFSET_TWO 2
#define BIT_OFFSET_TREE 3
#define BIT_OFFSET_SEVEN 7
#define FIRST_BYTE_START_BIT 0
#define SECOND_BYTE_START_BIT 8
#define THIRD_BYTE_START_BIT 16
#define FOURTH_BYTE_START_BIT 24
#define RF_DEVIES_MEDIATYPE_HDD_VALUE 0
#define RF_DEVIES_MEDIATYPE_SSD_VALUE 1
#define RF_DEVIES_MEDIATYPE_SSM_VALUE 2
#define RF_DEVIES_MEDIATYPE_UNKNOW_VALUE 255
#define RF_DEVIES_MEDIATYPE_HDD_NAME "HDD"
#define RF_DEVIES_MEDIATYPE_SSD_NAME "SSD"
#define RF_DEVIES_MEDIATYPE_SSM_NAME "SSM"
#define MULTIPLE_MB_TO_BYTES (1024 * 1024)

#define LOGICAL_DRIVE_OBJ_HANDLE "LogicalDriveHandle"


#define RF_ACTION_STORAGE_ACTION_TARGET_VALUE \
    "/redfish/v1/Systems/%s/Storages/RAIDStorage%d/Actions/Oem/Huawei/Storage.RestoreStorageControllerDefaultSettings"

#define STORAGE_CONTROLLERS_ODATA_ID_VALUE "/redfish/v1/Systems/%s/Storages/RAIDStorage%d#/StorageControllers/%d"
#define STORAGE_CONTROLLERS_ODATA_TYPE_VALUE "#Storage.v1_7_0.StorageController"
#define STORAGE_CONTROLLER_DESCRIPTION_VALUE "RAID Controller"
#define RFPROP_STORAGE_VOLUMES_ODATA_ID "Volumes"
#define SPARE_ACTIVATION_MODE_FAILURE_VALUE         "Failure"
#define SPARE_ACTIVATION_MODE_PREDICTIVE_VALUE      "Predictive"

#define STORAGE_CTRL_NAME_LENGTH 128
#define STORAGE_CTRL_INFO_LENGTH 128

#define STORAGE_CONTROLLERS_MEMBERID "RAIDStorage%d"
#define STORAGE_SDCONTROLLER_MEMBERID "SDStorage"

#define RFPROP_STORAGE_CONTROLLER_CACHE_PINNED "CachePinnedState"
#define RFPROP_STORAGE_CONTROLLER_MODE "Mode"

#define RFPROP_SENSOR_TEMPERATURES "Temperatures"
#define RFPROP_POWER_VOLTAGES "Voltages"
#define RFPROP_SENSOR_FANS "Fans"

#define TEMPERATURE_TYPE 0x1
#define VOLTAGE_TYPE 0x2
#define ARRAY_LENTH 128
#define RFPROP_SENSOR_PRESENCE_ABSENT 0
#define RFPROP_SENSOR_PRESENCE_OTHER 5

#define RFPROP_SENSOR_ODATA_ID "@odata.id"
#define RFPROP_SENSOR_MEMBER_ID "MemberId"
#define RFPROP_SENSOR_SENSOR_TYPE "SensorType"
#define RFPROP_SENSOR_SENSOR_NUM "SensorNumber"
#define RFPROP_SENSOR_NAME "Name"
#define RFPROP_SENSOR_ID "SensorId"
#define RFPROP_SENSOR_UNIT "Unit"
#define RFPROP_SENSOR_MEMORY_SIZE "MemorySizeMiB"
#define RFPROP_SENSOR_READING_VALUE "ReadingValue"
#define RFPROP_SENSOR_READING_CELSIUS "ReadingCelsius"
#define RFPROP_SENSOR_READINGVOLTS "ReadingVolts"
#define RFPROP_SENSOR_READING "Reading"
#define RFPROP_SENSOR_UPPER_THRESHOLD_NON_CRITICAL "UpperThresholdNonCritical"
#define RFPROP_SENSOR_UPPER_THRESHOLD_CRITICAL "UpperThresholdCritical"
#define RFPROP_SENSOR_UPPER_THRESHOLD_FATAL "UpperThresholdFatal"
#define RFPROP_SENSOR_LOWER_THRESHOLD_NON_CRITICAL "LowerThresholdNonCritical"
#define RFPROP_SENSOR_LOWER_THRESHOLD_CRITICAL "LowerThresholdCritical"
#define RFPROP_SENSOR_LOWER_THRESHOLD_FATAL "LowerThresholdFatal"
#define RFPROP_SENSOR_MIN_READING_RANGE_TEMP "MinReadingRangeTemp"
#define RFPROP_SENSOR_MIN_READING_RANGE "MinReadingRange"
#define RFPROP_SENSOR_MAX_READING_RANGE_TEMP "MaxReadingRangeTemp"
#define RFPROP_SENSOR_MAX_READING_RANGE "MaxReadingRange"
#define RFPROP_SENSOR_STATUS "Status"
#define RFPROP_SENSOR_STATE "State"
#define RFPROP_SENSOR_HEALTH "Health"
#define RFPROP_SENSOR_ENABLED "Enabled"
#define RFPROP_SENSOR_DISABLED "Disabled"
#define RFPROP_SENSOR_STAND_BY_OFFLINE "StandbyOffline"
#define RFPROP_SENSOR_READING_UNITS "ReadingUnits"
#define RFPROP_SENSOR_RPMS "RPM"
#define RFPROP_SENSOR_ABSENT "Absent"

#define RFPROP_INLETTEMP_MINOR_ALARM_THRESHOLD "InletTempMinorAlarmThreshold"
#define RFPROP_INLETTEMP_MAJOR_ALARM_THRESHOLD "InletTempMajorAlarmThreshold"


#define POWER_STR_MAX_LEN 64


#define RFPROP_POWER_CONTROL "PowerControl"
#define RF_POWER_CONTROL_URI_VALUE "/redfish/v1/Chassis/1/Power#/PowerControl"
#define RFPROP_POWER_CONTROL_ODATA_ID "@odata.id"
#define RFPROP_POWER_CONTROL_MEMBER_ID "MemberId"
#define RFPROP_POWER_CONTROL_NAME "Name"
#define RF_POWER_CONTROL_NAME_VALUE "System Power Control"
#define RFPROP_POWER_CONTROL_POWER_CONSUME "PowerConsumedWatts"
#define RFPROP_POWER_AVAILABLE_WATTS "PowerAvailableWatts"
#define RFPROP_POWER_CAPACITY_WATTS "PowerCapacityWatts"
#define RFPROP_POWER_DROP_STAMP "ShelfPowerDropStamp"
#define RFPROP_POWER_CONTROL_METRICS "PowerMetrics"
#define RFPROP_POWER_CONTROL_METRICS_MIN_CONSUME "MinConsumedWatts"
#define RFPROP_POWER_CONTROL_METRICS_MAX_CONSUME "MaxConsumedWatts"
#define RFPROP_POWER_CONTROL_METRICS_AVERAGE_CONSUME "AverageConsumedWatts"
#define RF_POWER_CONTROL_LIMIT_ENABLE 1
#define RF_POWER_CONTROL_LIMIT_DISABLE 0
#define RFPROP_POWER_CONTROL_LIMIT "PowerLimit"
#define RFPROP_POWER_CONTROL_LIMIT_VALUE "LimitInWatts"
#define RFPROP_POWER_CONTROL_LIMIT_EXCEPTION "LimitException"
#define RF_POWER_CONTROL_LIMIT_EXCEPTION_VALUE_NOACTION "NoAction"
#define RFPROP_POWER_CONTROL_LIMIT_EXCEPTION_VAL_NOACTION 0
#define RF_POWER_CONTROL_LIMIT_EXCEPTION_VALUE_HARDPOWEROFF "HardPowerOff"
#define RFPROP_POWER_CONTROL_LIMIT_EXCEPTION_VAL_HARDPOWEROFF 1
#define RFPROP_POWER_CONTROL_HEAT "HeatKcal"
#define RFPROP_POWER_CONTROL_HEAT_OEM_PATH "Oem/Huawei/HeatKcal"
#define RFPROP_POWER_CONTROL_PSU_ACTIVE_PSU "ActivePSU"
#define RF_POWER_CONTROL_PSU_LOAD_BALANCE 0
#define RF_POWER_CONTROL_PSU_LOAD_BALANCE_STR "LoadBalancing"
#define RF_POWER_CONTROL_PSU_ACTIVE_STANDBY 1
#define RF_POWER_CONTROL_PSU_ACTIVE_STANDBY_STR "ActiveStandby"
#define RF_POWER_CONTROL_PSU_ACTIVE_POWER 0
#define RF_POWER_CONTROL_PSU_ACTIVE_POWER_STR "Active"
#define RF_POWER_CONTROL_PSU_STANDBY_POWER 1
#define RF_POWER_CONTROL_PSU_STANDBY_POWER_STR "Standby"
#define RFPROP_POWER_FAULT_REASONS "FaultReasons"
#define RFPROP_POWER_LIMIT_VALUE_RANGE "PowerLimitRangeWatts"
#define RFPROP_POWER_LIMIT_ACTIVATED "PowerLimitActivated"
#define RFPROP_POWER_LIMIT_FAILED "PowerLimitFailed"
#define RFPROP_POWER_LIMIT_BLADE_INFO "PowerLimitBladeInfo"
#define RFPROP_POWER_LIMIT_THRESHOLD "PowerLimitThreshold"
#define RFPROP_POWER_LIMIT_MODE "PowerLimitMode"
#define RFPROP_POWER_RELATED_ITEM "RelatedItem"
#define RF_POWER_LIMIT_MODE_EQUAL "Equal"
#define RF_POWER_LIMIT_MODE_MANUAL "Manual"
#define RF_POWER_LIMIT_MODE_PROPORTION "Proportion"
#define RF_POWER_EXTEND_METRICS "PowerMetricsExtended"
#define RF_POWER_STATISTICS_COLLECTED "StatisticsCollected"
#define RF_POWER_MAX_CONSUMED_POWER_OCCURRED "MaxConsumedOccurred"
#define RF_POWER_CURRENT_CPU_POWER "CurrentCPUPowerWatts"
#define RF_POWER_CURRENT_MEM_POWER "CurrentMemoryPowerWatts"
#define RF_POWER_TOTAL_CONSUMED_POWER "TotalConsumedPowerkWh"
#define RF_POWER_EXTEND_METRICS_OBJECT  "Oem/Huawei/PowerMetricsExtended/"
#define RFPROP_POWER_CONTROL_STATIS_COLL_PATH       "Oem/Huawei/StatisticsCollected"
#define RFPROP_POWER_CONTROL_MAX_CONSUME_OCC_PATH       "Oem/Huawei/MaxConsumedOccurred"
#define RF_MAX_POWER_LIMIT "MaxPowerLimitInWatts"
#define RF_MIN_POWER_LIMIT "MinPowerLimitInWatts"
#define RF_POWER_LIMIT_WHEN_STEADY "PowerLimitWhenSteady"
#define RFPROP_POWER_LIMIT_STATUS "PowerLimitStatus"

#define REDUNDANCY_PROP_PATH "PowerControl/%d/Oem/Huawei/ExpectedRedundancy/@odata.id"
#define ACTIVE_PSU_PROP_PATH "PowerControl/%d/Oem/Huawei/ExpectedActivePSU"
#define ACTIVE_PSU_ODATAID_PROP_PATH "PowerControl/%d/Oem/Huawei/ExpectedActivePSU/%d/@odata.id"
#define DEEP_SLEEP_PROP_PATH "PowerControl/%d/Oem/Huawei/DeepSleep"
#define DEEP_NAR_PROP_PATH "PowerControl/%d/Oem/Huawei/NormalAndRedundancy"
#define HIGH_POWER_PROP_PATH "PowerControl/%d/Oem/Huawei/HighPowerThresholdWatts"
#define PRESET_LIMIT_IN_WATTS_PROP_PATH "PowerControl/%d/Oem/Huawei/PresetLimitInWatts"
#define POWER_LIMIT_WHEN_STEADY_PATH "PowerControl/%d/Oem/Huawei/PowerMetricsExtended/PowerLimitWhenSteady"



#define RFPROP_POWER_SUPPLIES "PowerSupplies"
#define RF_POWER_SUPPLIES_URI_VALUE "/redfish/v1/Chassis/1/Power#/PowerSupplies"
#define RFPROP_POWER_SUPPLIES_ODATA_ID "@odata.id"
#define RFPROP_POWER_SUPPLIES_MEMBER_ID "MemberId"
#define RFPROP_POWER_SUPPLIES_NAME "Name"
#define RFPROP_POWER_SUPPLIES_STATUS "Status"
#define RF_POWER_SUPPLIES_STATUS_STATE_VALUE_STANDBY "StandbySpare"
#define PROPERTY_ACTUAL_ACTIVE_VALUE_STANDBY 1
#define RF_POWER_SUPPLIES_STATUS_STATE_VALUE_ENABLED "Enabled"
#define PROPERTY_ACTUAL_ACTIVE_VALUE_ENABLED 0
#define RFPROP_POWER_SUPPLIES_TYPE "PowerSupplyType"
#define RF_POWER_SUPPLIES_TYPE_VALUE_DC "DC"
#define PROTERY_PS_INPUTMODE_VALUE_DC 0
#define RF_POWER_SUPPLIES_TYPE_VALUE_AC "AC"
#define PROTERY_PS_INPUTMODE_VALUE_AC 1
#define RF_POWER_SUPPLIES_TYPE_VALUE_AC_OR_DC "ACorDC"
#define PROTERY_PS_INPUTMODE_VALUE_AC_OR_DC 2
#define RFPROP_POWER_SUPPLIES_INPUT_VOLTAGE "LineInputVoltage"
#define RFPROP_POWER_SUPPLIES_CAPACITY "PowerCapacityWatts"
#define RFPROP_POWER_SUPPLIES_MODEL "Model"
#define RFPROP_POWER_SUPPLIES_FIRMWARE_VER "FirmwareVersion"
#define RFPROP_POWER_SUPPLIES_SERIAL_NUM "SerialNumber"
#define RFPROP_POWER_SUPPLIES_MANUFACTURER "Manufacturer"
#define PROTERY_PS_PROTOCOL_PSMI_STR "PSMI"
#define PROTERY_PS_PROTOCOL_PMBUS_STR "PMBUS"
#define RFPROP_POWER_SUPPLIES_SLOTNUM "SlotNumber"
#define RFPROP_POWER_SUPPLIES_PROTOCOL "Protocol"
#define RFPROP_POWER_SUPPLIES_ACTIVESTANDBY "ActiveStandby"
#define RFPROP_POWER_SUPPLIES_INPUT_POWER "PowerInputWatts"
#define RFPROP_POWER_SUPPLIES_INPUT_CURRENT "InputAmperage"
#define RFPROP_POWER_SUPPLIES_OUTPUT_POWER "PowerOutputWatts"
#define RFPROP_POWER_SUPPLIES_OUTPUT_CURRENT "OutputAmperage"
#define RFPROP_POWER_SUPPLIES_OUTPUT_VOLTAGE "OutputVoltage"
#define RFPROP_POWER_SUPPLIES_INLET_TEMPERATURE "InletTemperatureCelsius"
#define RFPROP_POWER_SUPPLIES_INNER_TEMPERATURE "InnerTemperatureCelsius"
#define RFPROP_POWER_SUPPLIES_LOCATION "DeviceLocator"
#define RFPROP_POWER_SUPPLIES_POSITION "Position"
#define RFPROP_POWER_SUPPLIES_LASTPOWEROUTPUT "LastPowerOutputWatts"
#define RFPROP_POWER_SUPPLIES_HARDWARE_VER "HardwareVersion"
#define RFPROP_POWER_SUPPLIES_SLEEP_STATUS "SleepEnabled"
#define RFPROP_POWER_SUPPLIES_POWER_TYPE   "PowerType"

#define RFPROP_POWER_SUPPLIES_MANUFACTUREDATE "ManufactureDate"
#define RFPROP_POWER_SUPPLIES_CURRENT_CAPACITY "CurrentCapacityAmperage"
#define RFPROP_POWER_SUPPLIES_TOTAL_RUNNING_TIME "TotalRunningHours"
#define RFPROP_POWER_SUPPLIES_FREQUENCY "InputFrequencyHz"
#define RFPROP_POWER_SUPPLIES_POWERSUPPLY_CHANNEL "PowerSupplyChannel"
#define RFPROP_POWER_SUPPLIES_VIN_CHANNEL_A "VinChannelAVoltage"
#define RFPROP_POWER_SUPPLIES_VIN_CHANNEL_B "VinChannelBVoltage"
#define PROPERTY_POWERSUPPLY_CHANNEL_VALUE_MAIN_CIRCUIT 0
#define PROPERTY_POWERSUPPLY_CHANNEL_VALUE_BACKUP_CIRCUIT 1
#define PROTERY_PS_PROTOCOL_CANBUS_STR "CANBUS"
#define RF_POWERSUPPLY_CHANNEL_VALUE_MAIN_CIRCUIT "MainCircuit"
#define RF_POWERSUPPLY_CHANNEL_VALUE_BACKUP_CIRCUIT "BackupCircuit"
#define RFPROP_PS_INPUT_A_STATUS                                    "PSUInputAStatus"
#define RFPROP_PS_INPUT_B_STATUS                                    "PSUInputBStatus"
#define RFPROP_PS_BATTERY_PRES_STATE                                "BatteryPresenceState"
#define RFPROP_PS_EOD_ALARM_STATUS                                  "EODAlarmState"

#define RFPROP_POWER_SUPPLIES_CURRENT_CAPACITY_OEM_PATH "Oem/Huawei/CurrentCapacityAmperage"
#define RFPROP_POWER_SUPPLIES_TOTAL_RUNNING_TIME_OEM_PATH "Oem/Huawei/TotalRunningHours"
#define RFPROP_POWER_SUPPLIES_FREQUENCY_OEM_PATH "Oem/Huawei/InputFrequencyHz"
#define RFPROP_POWER_SUPPLIES_VIN_CHANNEL_A_OEM_PATH "Oem/Huawei/VinChannelAVoltage"
#define RFPROP_POWER_SUPPLIES_VIN_CHANNEL_B_OEM_PATH "Oem/Huawei/VinChannelBVoltage"


#define RFPROP_POWER_CONTROL_TOTAL_CONSUMED_POWER_OEM_PATH "Oem/Huawei/PowerMetricsExtended/TotalConsumedPowerkWh"
#define RFPROP_POWER_SUPPLIES_INPUT_POWER_OEM_PATH "Oem/Huawei/PowerInputWatts"
#define RFPROP_POWER_SUPPLIES_INPUT_CURRENT_OEM_PATH "Oem/Huawei/InputAmperage"
#define RFPROP_POWER_SUPPLIES_OUTPUT_POWER_OEM_PATH "Oem/Huawei/PowerOutputWatts"
#define RFPROP_POWER_SUPPLIES_OUTPUT_CURRENT_OEM_PATH "Oem/Huawei/OutputAmperage"
#define RFPROP_POWER_SUPPLIES_OUTPUT_VOLTAGE_OEM_PATH "Oem/Huawei/OutputVoltage"

#define RFPROP_POWER_SUPPLIES_INLET_TEMPERATURE_OEM_PATH "Oem/Huawei/InletTemperatureCelsius"
#define RFPROP_POWER_SUPPLIES_INNER_TEMPERATURE_OEM_PATH "Oem/Huawei/InnerTemperatureCelsius"


#define RFPROP_POWER_REDUNDANCY "Redundancy"
#define RF_POWER_REDUNDANCY_ARRAY_LEN 2
#define RFPROP_POWER_REDUNDANCY_MAX_NUM_SUPPORTED "MaxNumSupported"
#define RFPROP_POWER_REDUNDANCY_MIN_NUM_NEEDED "MinNumNeeded"
#define RF_POWER_REDUNDANCY_MIN_NUM_NEEDED_VALUE 2
#define RFPROP_POWER_REDUNDANCY_SET "RedundancySet"
#define RF_POWER_REDUNDANCY_NAME_VALUE "PowerSupply Redundancy Group %d"
#define RFPROP_POWER_REDUNDANCY_MODE "Mode"
#define RF_POWER_REDUNDANCY_MODE_FAILOVER "Failover"
#define RF_POWER_REDUNDANCY_MODE_SHARING "Sharing"
#define RFPROP_POWER_REDUNDANCY_ODATA_ID "@odata.id"
#define RFPROP_POWER_REDUNDANCY_MEMBER_ID "MemberId"
#define RFPROP_POWER_REDUNDANCY_NAME "Name"
#define RFPROP_POWER_REDUNDANCY_SLEEPCONFIG "SleepModeEnabled"
#define RFPROP_POWER_REDUNDANCY_OEM "Oem"
#define RFPROP_POWER_REDUNDANCY_HUAWEI "Huawei"

#define RFPROP_THERMAL_REDUNDANCY "Redundancy"


#define RFPROP_POWER_EXPECTED_REDUNDANCY "ExpectedRedundancy"
#define RFPROP_POWER_EXPECTED_ACTIVE_PSU "ExpectedActivePSU"
#define RFPROP_POWER_DEEP_SLEEP_MODE "DeepSleep"
#define RFPROP_POWER_DEEP_MSPP_MODE "MSPPEnabled"
#define RFPROP_POWER_WORKMODE_CONFIGURABLE "WorkModeConfigurable"
#define RFPROP_POWER_NORMAL_AND_REDUNDANCY "NormalAndRedundancy"
#define RFPROP_POWER_HIGH_THRESHOLD "HighPowerThresholdWatts"
#define RFPROP_POWER_CONTROL_PRESET_LIMIT_VALUE "PresetLimitInWatts"
#define RFPROP_POWER_CONTROL_LIMIT_EXCEPTION_SUPPORTED "PowerLimitExceptionSupported"

#define RFPROP_REDUNDANCY_ODATA_ID_VALUE "/redfish/v1/Chassis/%s/Power#/Redundancy/%d"
#define RFPROP_REDUNDANCY_ODATA_TYPE_VALUE "#Redundancy.v1_2_1.Redundancy"
#define RFPROP_POWER_SUPPLY_ODATA_ID_VALUE "/redfish/v1/Chassis/%s/Power#/PowerSupplies/%d"
#define RFPROP_REDUNDANCY_PATH "Redundancy/%d"
#define RFPROP_REDUNDANCY_MODE_PATH "Redundancy/%d/Mode"
#define RFPROP_REDUNDANCY_SET_PATH "Redundancy/%d/RedundancySet"
#define RFPROP_REDUNDANCY_SET_ODATA_PATH "Redundancy/%d/RedundancySet/%d/@odata.id"

#define RFPROP_POWER_OEM_ACTION_COLLECT_HISTORY_DATA "Power.CollectHistoryData"
#define RF_POWER_OEM_ACTION_COLLECT_HISTORY_DATA_TARGET \
    "/redfish/v1/Chassis/%s/Power/Oem/Huawei/Actions/Power.CollectHistoryData"
#define RF_POWER_OEM_ACTION_COLLECT_HISTORY_DATA_ACTION_INFO "/redfish/v1/Chassis/%s/Power/CollectHistoryDataActionInfo"

#define RFPROP_POWER_OEM_ACTION_RESET_STATISTICS "Power.ResetStatistics"
#define RF_POWER_OEM_ACTION_RESET_STATISTICS_TARGET \
    "/redfish/v1/Chassis/%s/Power/Oem/Huawei/Actions/Power.ResetStatistics"
#define RF_POWER_OEM_ACTION_RESET_STATISTICS_ACTION_INFO "/redfish/v1/Chassis/%s/Power/ResetStatisticsActionInfo"

#define RFPROP_POWER_OEM_ACTION_RESET_HISTORY_DATA "Power.ResetHistoryData"
#define RF_POWER_OEM_ACTION_RESET_TARGET "/redfish/v1/Chassis/%s/Power/Oem/Huawei/Actions/Power.ResetHistoryData"
#define RF_POWER_OEM_ACTION_RESET_ACTION_INFO "/redfish/v1/Chassis/%s/Power/ResetHistoryDataActionInfo"

#define RFPROP_POWER_OEM_ACTION_SET_PSU_SUPPLY_SOURCE "Power.SetPsuSupplySource"
#define RF_POWER_OEM_ACTION_SET_PSU_SUPPLY_SOURCE_TARGET \
    "/redfish/v1/Chassis/%s/Power/Oem/Huawei/Actions/Power.SetPsuSupplySource"
#define RF_POWER_OEM_ACTION_SET_PSU_SUPPLY_SOURCE_ACTION_INFO \
    "/redfish/v1/Chassis/%s/Power/SetPsuSupplySourceActionInfo"
#define RF_POWER_OEM_ACTION_PARAM_SUPPLY_SOURCE "SupplySource"

#define RFPROP_CHASSIS_OEM_ACTION_CONTROL_LED "Chassis.ControlIndicatorLED"
#define RF_CHASSIS_OEM_ACTION_CONTROL_LED "/redfish/v1/Chassis/%s/Oem/Huawei/Actions/Chassis.ControlIndicatorLED"
#define RF_CHASSIS_OEM_ACTION_CONTROL_LED_ACTIONINFO "/redfish/v1/Chassis/%s/ControlIndicatorLEDActionInfo"

#define RFPROP_CHASSIS_OEM_ACTION_SET_UNIT_RFID_INFO "Chassis.SetUnitRFIDInfo"
#define RF_CHASSIS_OEM_ACTION_SET_UNIT_RFID_INFO "/redfish/v1/Chassis/%s/Oem/Huawei/Actions/Chassis.SetUnitRFIDInfo"
#define RF_CHASSIS_OEM_ACTION_SET_UNIT_RFID_INFO_ACTIONINFO "/redfish/v1/Chassis/%s/SetUnitRFIDInfoActionInfo"
#define RF_CHASSIS_OEM_ACTION_PARAM_INDICATOR_LED "IndicatorLED"
#define RF_CHASSIS_OEM_ACTION_PARAM_DURATION "Duration"

#define RFPROP_CHASSIS_OEM_ACTION_SET_DEVICE_INSTALLED_STATUS "Chassis.SetDeviceInstalledStatus"
#define RF_CHASSIS_OEM_ACTION_SET_DEVICE_INSTALLED_STATUS \
    "/redfish/v1/Chassis/%s/Oem/Huawei/Actions/Chassis.SetDeviceInstalledStatus"
#define RF_CHASSIS_OEM_ACTION_SET_DEVICE_INSTALLED_STATUS_ACTIONINFO \
    "/redfish/v1/Chassis/%s/SetDeviceInstalledStatusActionInfo"
#define RF_CHASSIS_OEM_ACTION_PARAM_INSTALLED_STATE "InstalledState"

#define RFPROP_CHASSIS_OEM_ACTION_EXPORT_TEMPLATE_FILE "Chassis.ExportUMarkLabelTemplate"
#define RF_CHASSIS_OEM_ACTION_EXPORT_TEMPLATE_FILE \
    "/redfish/v1/Chassis/%s/Oem/Huawei/Actions/Chassis.ExportUMarkLabelTemplate"
#define RF_CHASSIS_OEM_ACTION_EXPORT_TEMPLATE_FILE_ACTIONINFO \
    "/redfish/v1/Chassis/%s/ExportUMarkLabelTemplateActionInfo"
#define RFPROP_CHASSIS_OEM_ACTION_IMPORT_CONFIG_FILE "Chassis.ImportConfigFile"
#define RF_CHASSIS_OEM_ACTION_IMPORT_CONFIG_FILE "/redfish/v1/Chassis/%s/Oem/Huawei/Actions/Chassis.ImportConfigFile"
#define RF_CHASSIS_OEM_ACTION_IMPORT_CONFIG_FILE_ACTIONINFO "/redfish/v1/Chassis/%s/ImportConfigFileActionInfo"


#define RFPROP_CHASSIS_OEM_ACTION_SET_DIMENSIONS "Chassis.SetDimensions"
#define RF_CHASSIS_OEM_ACTION_SET_DIMENSIONS "/redfish/v1/Chassis/%s/Oem/Huawei/Actions/Chassis.SetDimensions"
#define RF_CHASSIS_OEM_ACTION_SET_DIMENSIONS_ACTIONINFO "/redfish/v1/Chassis/%s/SetDimensionsActionInfo"

#define CHASSIS_SET_DIMENSIONS_ACTIONINFO_PATTERN "^/redfish/v1/chassis/rack/setdimensionsactioninfo$"
#define CHASSIS_SET_DIMENSIONS_ACTIONINFO_RSC_PATH "/redfish/v1/chassis/rack/setdimensionsactioninfo"




#define URI_FORMAT_CHASSIS_STORAGE_DRIVES "/redfish/v1/Chassis/%s/Drives/%s"


#define RFPROP_CHASSIS_TYPE "ChassisType"         // 机箱类型
#define RFPROP_CHASSIS_INDICATELED "IndicatorLED" // UID指示灯状态
#define RFPROP_CHASSIS_HEALTH_LED "HealthLED"     // 健康灯
#define RFPROP_CHASSIS_PN "PartNumber"
#define RFPROP_CHASSIS_MODEL "Model"
#define RFPROP_CHASSIS_SN "SerialNumber"
#define RFPROP_CHASSIS_MF_DATE "ManufacturingDate"
#define RFPROP_CHASSIS_MFER "Manufacturer"
#define RFRPOP_CHASSIS_PRODUCT_NAME "ProductName"
#define RFPROP_CHASSIS_FRU "Fru"

#define RFPROP_CHASSIS_DEPTH_MM "DepthMm"
#define RFPROP_CHASSIS_HEIGHT_MM "HeightMm"
#define RFPROP_CHASSIS_WIDTH_MM "WidthMm"

#define RF_CHASSIS_TYPE_RACK_VALUE 0
#define RF_CHASSIS_TYPE_BLADE_VALUE 1
#define RF_CHASSIS_TYPE_OTHER "Other"
#define RF_CHASSIS_LED_LIT_VALUE 255
#define RF_CHASSIS_LED_BLINKING_TIME 255
#define RF_CHASSIS_LED_OFF_VALUE 0
#define RF_CHASSIS_LED_LOCK_STATE 0
#define RF_LED_LIT        "Lit"
#define RF_LED_OFF        "Off"
#define RF_LED_BLINKING   "Blinking"
#define RF_CHASSIS_DEVICE_INSTALLED_STATE_INSTALLED "Installed"
#define RF_CHASSIS_DEVICE_INSTALLED_STATE_REMOVED "Removed"
#define RF_CHASSIS_TYPE_RACK "Rack"
#define RF_CHASSIS_TYPE_BLADE "Blade"
#define RF_CHASSIS_THERMAL "Thermal"
#define RF_CHASSIS_POWER "Power"
#define RF_CHASSIS_HDD_DRAWER "HDDdrawer"
#define RF_CHASSIS_LINKS "Links"
#define RF_CHASSIS_ID_MAX_LEN_VALUE 16
#define RFPROP_CHASSIS_ID "ChassisID"
#define RFPROP_CHASSIS_NAME "ChassisName"
#define RFPROP_CHASSIS_LOCATION "ChassisLocation"
#define RFPROP_CHASSIS_INLET_TEM "InletTemperatureCelsius"
#define RFPROP_CHASSIS_MAINBOARD "Mainboard"
#define RFPROP_CONTAINS "Contains"
#define RFPROP_CONTAINED_BY "ContainedBy"
#define RF_CHASSIS_NAME_MAX_LEN 20
#define RF_MAX_CHASSIS_ID 999999
#define RF_CHASSIS_LOCATION_MAX_LEN 20
#define RF_BACKPLANE_PCB_VER_MAX_LEN 20
#define RFPROP_CHASSIS_IS_MANAGED "IsManaged"
#define RFPROP_CHASSIS_BACKPLANE_PCB_VERSION "BackPlanePcbVersion"

#define RF_CHASSIS_TYPE_ENCL "Enclosure"
#define RF_CHASSIS_TYPE_MODULE "Module"
#define RF_CHASSIS_TYPE_COMPONENT "Component"
#define RF_CHASSIS_TYPE_DRAWER "Drawer"

#define RFPROP_CHASSIS_NETWORK_ADAPTERS_SUMMARY "NetworkAdaptersSummary"

#define RFPROP_CHASSIS_DRIVE_SUMMARY "DriveSummary"
#define RFPROP_CHASSIS_POWERSUPPLY_SUMMARY "PowerSupplySummary"
#define RFPROP_CHASSIS_PACKAGE_CODE "PackageCode"
#define RFPROP_SYSTEM_STORAGE_SUMMARY "StorageSummary"
#define RFPROP_CHASSIS_LANSWITCH_SUMMARY "Switches"


#define RFPROP_CHASSIS_MAX_NUM "DeviceMaxNum"
#define RFPROP_MEM_NUM "MemoryNum"
#define RFPROP_PCIE_NUM "PCIeNum"

#define RFPROP_ENCLOSURE_SERIAL_NUM         "EnclosureSN"

#define RFPROP_CPU_NUM "CPUNum"
#define RFPROP_DISK_NUM "DiskNum"
#define RFPROP_POWERSUPPLY_NUM "PowerSupplyNum"
#define RFPROP_FAN_NUM "FanNum"
#define RFPROP_MEZZCARD_NUM "MezzCardNum"


#define RFPROP_SD_CARD_NUM "SDCardNum"
#define RFPROP_SD_CONTROLLER_NUM "SDContollerNum"
#define RFPROP_SECURITY_MODULE_NUM "SecurityModuleNum"

#define RFPROP_BBU_MODULE_NUM "BackupBatteryUnitNum"

#define RFPROP_CHASSIS_BATTERY_SUMMARY "BatterySummary"
#define RFPROP_BATTERY "Battery"


#define RFPROP_SYSTEM_STORAGE_LOGICAL_DRIVE_SUMMARY "LogicalDriveSummary"
#define RFPROP_SYSTEM_STORAGE_SUBSYSTEM_SUMMARY "StorageSubsystemSummary"

#define RFPROP_HEALTH_ROLLUP "HealthRollup"



#define RFPROP_SYSTEM_STORAGE_VIEWS_SUMMARY "StorageViewsSummary"
#define RFPROP_SYSTEM_STORAGE_RAID_CONTROLLERS "RaidControllers"
#define RFPROP_SYSTEM_STORAGE_LINK "Link"
#define RFPROP_SYSTEM_STORAGE_LOGICAL_DRIVE_NAME "Logical Drive %d"
#define RFPROP_SYSTEM_STORAGE_SPAN_NAME "Span %d"
#define RFPROP_SYSTEM_STORAGE_HOT_SPARE "HotSpare"
#define RFPROP_SYSTEM_STORAGE_HOT_SPARE_NAME "Hot Spare"



#define RFPROP_CHASSIS_ENERGY_EFFICIENCY "EnergyEfficiency"
#define RFPROP_CHASSIS_ENERGYSAVINGS_PERCENT "EnergySavingsPercent"
#define RFPROP_CHASSIS_POWER_SAVINGS "PowerSavingsKwh"
#define RFPROP_CHASSIS_CARBONFOOTPRINT_REDUCTION "CarbonFootprintReductionKg"


#define RFPROP_NETWORK_ADAPTER_CONFIG "Configuration"
#define RFPROP_NETWORK_ADAPTER_CONFIG_EFFECTIVE "Effective"
#define RFPROP_NETWORK_ADAPTER_ERROR_PROP "Property"
#define RFPROP_NETWORK_ADAPTER_ERROR_VAL "Value"
#define RFPROP_NETWORK_ADAPTER_ERROR_REL_PROP "RelatedProperty"
#define RFPROP_NETWORK_ADAPTER_ERROR_REL_VAL "RelatedValue"


#define RFPROP_SYSTEM_ASSETTAG "AssetTag"
#define RFPROP_SYSTEM_MANUFACTURER "Manufacturer"
#define RFPROP_SYSTEM_MODEL "Model"
#define RFPROP_SYSTEM_SERIALNUMBER "SerialNumber"

#define RFPROP_SYSTEM_DEV_SERIALNUMBER "SystemSerialNumber"
#define RFPROP_SYSTEM_BOARD_MANUFACTURER "BoardManufacturer"

#define RFPROP_SYSTEM_UUID "UUID"
#define RFPROP_SYSTEM_STATUS "Status"
#define RFPROP_SYSTEM_FRU_TYPE "FruType"
#define RFPROP_VALUE_SYSTEM_OS "OS"
#define OS_FRU_ID 0
#define RFPROP_SYSTEM_POWERSTATE "PowerState"
#define RFPROP_SYSTEM_HEALTH_LED_STATE      "HealthLedState"
#define RFPROP_VALUE_SYSTEM_POWER_ON "On"
#define RFPROP_VALUE_SYSTEM_POWER_OFF "Off"
#define RFPROP_VALUE_SYSTEM_POWERING_ON "PoweringOn"
#define RFPROP_VALUE_SYSTEM_POWERING_OFF "PoweringOff"
#define RFPROP_SYSTEM_BIOSVERSION "BiosVersion"
#define RFPROP_SYSTEM_TEEOSVERSION "TeeosVersion"
#define RFPROP_SYSTEM_PROCESSORSUMMARY "ProcessorSummary"
#define RFPROP_SYSTEM_MEMORYSUMMARY "MemorySummary"
#define RFPROP_SYSTEM_BOOT "Boot"
#define RFPROP_SYSTEM_HOSTINGROLE "HostingRole"
#define RFPROP_SYSTEM_HOSTNAME "HostName"

#define RFPROP_SYSTEM_HEALTH_SUMMARY "HealthSummary"
#define RFPROP_SYSTEM_CRITICAL_ALARM_CNT "CriticalAlarmCount"
#define RFPROP_SYSTEM_MAJOR_ALARM_CNT "MajorAlarmCount"
#define RFPROP_SYSTEM_MINOR_ALARM_CNT "MinorAlarmCount"



#define INVALID_VAL 0xFF

#define SYSTEM_HEALTH_LED_LIT_VALUE                255
#define SYSTEM_HEALTH_LED_OFF_VALUE                0


#define RFPROP_SYSTEM_PROCESSORSIMPLEVIEW "ProcessorView"
#define RFPROP_SYSTEM_MEMORYSIMPLEVIEW "MemoryView"

#define URI_FORMAT_SYSTEM_FORCERESTARTSERVICE "/redfish/v1/Systems/%s/ForceRestartService"
#define RFPROP_SYSTEM_RESTARTSERVICE "Oem/Huawei/ForceRestartService.RestartOs"
#define URI_PATTERN_SYSTEM_RESTART_OS_ACTIONINFO "^/redfish/v1/Systems/[a-zA-Z0-9]+/RestartOsActionInfo$"
#define RFPROP_SYSTEM_FORCE_RESTART_SERVICE "/redfish/v1/Systems/%s/ForceRestartService"
#define RFPROP_SYSTEM_RESTART_OS_ACTION_INFOURI "/redfish/v1/Systems/%s/ForceRestartService/RestartOsActionInfo"
#define RFPROP_SYSTEM_DIGITALWARRANTY "DigitalWarranty"

#define RFPROP_SYSTEM_PROCESSORS "Processors"
#define RFPROP_SYSTEM_MEMORY "Memory"
#define RFPROP_SYSTEM_STORAGES "Storages"
#define RFPROP_SYSTEM_STORAGE "Storage"
#define RFPROP_SYSTEM_INTERFACES "NetworkInterfaces"

#define RFPROP_SYSTEM_IB "InfiniBandInterfaces"
#define RFPROP_SYSTEM_NB "NetworkBondings"

#define RFPROP_SYSTEM_NBRIDGE "NetworkBridge"
#define RFPROP_SYSTEM_LOGSERVICES "LogServices"

#define RF_MONITOR_TASK_RUNNING "Running"
#define RF_MONITOR_TASK_EXCEPTION "Exception"
#define RF_MONITOR_ORIGIN_RESOURSE "OriginRsc"
#define RF_MONITOR_CREATE_RESOURSE "NewRsc"

#define RF_MONITOR_EXTENDED_INFO "ExtendedInfo"
#define RF_MONITOR_EXTENDED_FILENAME "FileName"

#define TASK_MONITOR_REGEX "^/redfish/v1/taskservice/tasks/([1-9]|[1-2][0-9]|[3][0-2])/monitor$"
#define TASK_MONITOR_PATH "/redfish/v1/taskservice/tasks/template/monitor"

#define RFPROP_COMMON_COUNT "Count"
#define RFPROP_SYSTEM_PROCESSOR_MODE "Model"
#define RFPROP_SYSTEM_MEMORY_TOTAL "TotalSystemMemoryGiB"
#define RFPROP_SYSTEM_SUMMARY_STATUS "Status"
#define RFPROP_SYSTEM_BOOT_ENABLED "BootSourceOverrideEnabled"
#define RFPROP_SYSTEM_BOOT_TARGET "BootSourceOverrideTarget"
#define RFPROP_SYSTEM_BOOT_MODE "BootSourceOverrideMode"
#define RFPROP_SYSTEM_BOOT_TARGET_ALLOW "BootSourceOverrideTarget@Redfish.AllowableValues"


#define RFPROP_SYSTEM_REVOLUMES "/redfish/v1/Systems/%s/Storages/RAIDStorage%d/Volumes"

#define RFPROP_SYSTEM_REVOLUME_MEMBERID "LogicalDrive%d"
#define RFPROP_SYSTEM_REVOLUME_STR_MEM "/redfish/v1/Systems/%s/Storages/RAIDStorage%d/Volumes/%s"
#define RFPROP_SYSTEM_VOLUME_ACTION_SUFFIX "/Actions/Volume.Initialize"
#define RFPROP_SYSTEM_VOLUME_ACTION_INFO_SUFFIX "/InitializeActionInfo"
#define RFPROP_SYSTEM_REVOLUME "/redfish/v1/Systems/%s/Storages/RAIDStorage%d/Volumes/LogicalDrive%hu"

#define RFPROP_VOLUME_ODATA_CONTEXT \
    "/redfish/v1/$metadata#Systems/Members/%s/Storages/Members/RAIDStorage%d/Volumes/Members/$entity"
#define RFPROP_VOLUMES_ODATA_CONTEXT \
    "/redfish/v1/$metadata#Systems/Members/%s/Storages/Members/RAIDStorage%d/Volumes/$entity"
#define RFPROP_VOLUME_VOLUMETYPE "VolumeType"
#define RFPROP_VOLUME_STATUS "Status"
#define RFPROP_VOLUME_STATUS_STATE "State"
#define RFPROP_VOLUME_STATUS_HEALTH "Health"
#define RFPROP_VOLUME_CAPACITY "CapacityBytes"
#define RFPROP_VOLUME_OPTIMUMIOSIZE "OptimumIOSizeBytes"
#define RFPROP_VOLUME_STRIPSIZEBYTES "StripSizeBytes"
#define RFPROP_VOLUME_READCACHEPOLICY "ReadCachePolicy"

#define RFPROP_VOLUME_OPERATIONS "Operations"
#define RFPROP_VOLUME_OPERATIONS_NAME "OperationName"
#define RFPROP_VOLUME_OPERATIONS_PERCENTAGE_COMPLETE "PercentageComplete"
#define RFPROP_VOLUME_OPERATIONS_ASSOCIATED_TASK "AssociatedTask"
#define RFACTION_VOLUME_INITIALIZE "Volume.Initialize"
#define RFACTION_FORMAT_CANCEL_INIT "CancelInit"
#define RFTASK_NAME_VOLUME_TEMPLATE "RaidController%d_LogicalDrive%d_init"
#define RFVALUE_VOLUME_OPERATIONS_NAME_FGI "Foreground Init"
typedef struct tag_volume_init_task_monitor_info {
    gchar user_name[PROP_VAL_LENGTH];
    gchar client[PROP_VAL_LENGTH];
    OBJ_HANDLE obj_handle;
    gint32 get_progress_failed_count; // 获取失败的次数
    gboolean before_success;
} VOLUME_INIT_TASK_MONITOR_INFO_S;

// 以下属性为oem属性

#define RFPROP_VOLUME_STATE_OFFLINE "Offline"
#define RFPROP_VOLUME_STATE_PARTTIALLY_DEGRADED "Partially degraded"
#define RFPROP_VOLUME_STATE_DEGRADED "Degraded"
#define RFPROP_VOLUME_STATE_OPTIMAL "Optimal"
#define RFPROP_VOLUME_STATE_FAILED "Failed"


#define RFPROP_VOLUME_NAME "VolumeName"
#define RFPROP_VOLUME_REF_RAIDCONTROLLER_ID "RaidControllerID"
#define RFPROP_VOLUME_RAID_LEVEL "VolumeRaidLevel"
#define RFPROP_VOLUME_DEFAULT_READ_POLICY "DefaultReadPolicy"
#define RFPROP_VOLUME_DEFAULT_WRITE_POLICY "DefaultWritePolicy"
#define RFPROP_VOLUME_DEFAULT_CACHE_POLICY "DefaultCachePolicy"
#define RFPROP_VOLUME_CONSISTENCY_CHECK "ConsistencyCheck"
#define RFPROP_VOLUME_SPAN_NUMBER "SpanNumber"
#define RFPROP_VOLUME_NUM_DRIVE_PER_SPAN "NumDrivePerSpan"
#define RFPROP_VOLUME_DRIVE_SPANS "Spans"
#define RFPROP_VOLUME_DRIVE_SPAN_NAME "SpanName"
#define RFPROP_VOLUME_DRIVE_SPAN_DRIVES "Drives"
#define RFPROP_VOLUME_DRIVE_SPAN_ARRAY_ID "ArrayID"
#define RFPROP_VOLUME_DRIVE_SPAN_USED_SPACE "UsedSpaceMiB"
#define RFPROP_VOLUME_DRIVE_SPAN_FREE_SPACE "FreeSpaceMiB"
#define RFPROP_VOLUME_CURRENT_READ_POLICY "CurrentReadPolicy"
#define RFPROP_VOLUME_CURRENT_WRITE_POLICY "CurrentWritePolicy"
#define RFPROP_VOLUME_CURRENT_CACHE_POLICY "CurrentCachePolicy"
#define RFPROP_VOLUME_BOOT_ABLE "BootEnable"
#define RFPROP_VOLUME_BOOT_PRIORITY "BootPriority"
#define RFPROP_VOLUME_ACCESS_POLICY "AccessPolicy"
#define RFPROP_VOLUME_BGI_ENABLE "BGIEnable"
#define RFPROP_VOLUME_CACHECADE_VOLUME "SSDCachecadeVolume"
#define RFPROP_VOLUME_SSD_CACHING_ENABLE "SSDCachingEnable"
#define RFPROP_VOLUME_CACHECADE_LD "AssociatedCacheCadeVolume"
#define REPROP_VOLUME_ASSOCIATED_LD "AssociatedVolumes"
#define RFPROP_VOLUME_DRIVE_CACHE_POLICY "DriveCachePolicy"
#define RFPROP_VOLUME_LINK "Links"
#define RFPROP_VOLUME_LINK_DRIVES_COUNT "Drives@odata.count"
#define RFPROP_VOLUME_LINK_DRIVES "Drives"
#define RFPROP_VOLUME_LINK_CACHECADE "CacheCade"
#define RFPROP_VOLUME_OS_DRIVE_NAME "OSDriveName"
#define REPROP_VOLUME_ACCELERATION_METHOD "AccelerationMethod"
#define REPROP_VOLUME_CACHE_LINE_SIZE "CacheLineSizeKB"
#define REPROP_VOLUME_REBUILDSTATE "RebuildState"
#define REPROP_VOLUME_REBUILDPROGRESS "RebuildProgress"


#define RFPROP_SYSTEM_PROCESEEOR_ARCHITECTURE "ProcessorArchitecture"
#define RFPROP_SYSTEM_PROCESEEOR_INSTRUCTIONSET "InstructionSet"
#define RFPROP_SYSTEM_PROCESEEOR_MANUFACTURER "Manufacturer"
#define RFPROP_SYSTEM_PROCESEEOR_MODEL "Model"
#define RFPROP_SYSTEM_PROCESEEOR_PROCESSORID "ProcessorId"
#define RFPROP_SYSTEM_PROCESEEOR_MAXSPEED "MaxSpeedMHz"
#define RFPROP_SYSTEM_PROCESEEOR_TOTALCORES "TotalCores"
#define RFPROP_SYSTEM_PROCESEEOR_TOTALTHTREADS "TotalThreads"
#define RFPROP_SYSTEM_PROCESEEOR_SOCKET "Socket"
#define RFPROP_SYSTEM_PROCESEEOR_STATUS "Status"
#define RFPROP_SYSTEM_PROCESEEOR_IDENTIFICATIONREGISTERS "IdentificationRegisters"
#define RFPROP_SYSTEM_PROCESSOR_VENDOR_ID "VendorId"
#define RFPROP_SYSTEM_PROCESSOR_NPU_FWVERSION "FirmwareVersion"
#define RFPROP_SYSTEM_PROCESSOR_NPU_POWER "PowerWatts"
#define RFPROP_SYSTEM_PROCESSOR_NPU_POWERON "PowerOn"
#define RFPROP_SYSTEM_PROCESSOR_NPU_AICORE_TEMP "AiCoreTempCelsius"
#define RFPROP_SYSTEM_PROCESSOR_NPU_HBM_TEMP "HBMTempCelsius"
#define RFPROP_SYSTEM_PROCESSOR_NPU_HISTORY_ECC_NUM "HistoryEccCount"
#define RFPROP_SYSTEM_PROCESSOR_NPU_HISTORY_ECC_TIME "HistoryEccTime"
#define RFPROP_SYSTEM_PROCESSOR_NPU_NIMBUS_TEMP "NimBusTempCelsius"
#define RFPROP_SYSTEM_PROCESSOR_NPU_SERIALNUMBER "SerialNumber"
#define RFPROP_SYSTEM_PROCESSOR_NPU_COMPUTE_CAPABILITY "ComputeCapability"
#define RFPROP_SYSTEM_PROCESSOR_NPU_ID "NpuId"
#define RFPROP_SYSTEM_PROCESSOR_TYPE "ProcessorType"
#define RFPROP_SYSTEM_PROCESEEOR_EFFECTIVEFAMILY "EffectiveFamily"
#define RFPROP_SYSTEM_PROCESEEOR_EFFECTIVEMODEL "EffectiveModel"
#define RFPROP_SYSTEM_PROCESEEOR_STEP "Step"
#define RFPROP_SYSTEM_PROCESSOR_NPU_CTRL_CPU_USAGE_PERCENT "CtrlCPUUsagePercent"
#define RFPROP_SYSTEM_PROCESSOR_NPU_AICORE_MAX_SPEED "AiCoreMaxSpeedMHz"
#define RFPROP_SYSTEM_PROCESSOR_NPU_AICORE_USAGE_PERCENT "AiCoreUsagePercent"
#define RFPROP_SYSTEM_PROCESSOR_NPU_MEMORY_ON_CHIP "MemoryOnChip"
#define RFPROP_SYSTEM_PROCESSOR_NPU_TOTAL_MEMORY "TotalMemoryKiB"
#define RFPROP_SYSTEM_PROCESSOR_NPU_MEMORY_USAGE "MemoryUsageKiB"
#define RFPROP_SYSTEM_PROCESSOR_NPU_MEMORY_FREE "MemoryFreeKiB"
#define RFPROP_SYSTEM_PROCESSOR_NPU_ETH "EthernetInterface"
#define RFPROP_SYSTEM_PROCESSOR_NPU_IPV4_ADDRESS "IPv4Address"
#define RFPROP_SYSTEM_PROCESSOR_NPU_RX_PACKETS "PacketsReceived"
#define RFPROP_SYSTEM_PROCESSOR_NPU_TX_PACKETS "PacketsTransmitted"
#define RFPROP_SYSTEM_PROCESSOR_NPU_DROPPED_PACKETS "PacketsDropped"
#define RFPROP_NPU_MEM_UTL "MemoryUsagePercent"
#define RFPROP_NPU_HPM_UTL "HBMUsagePercent"
#define RFPROP_NPU_MEM_BW "MemoryBandwidthUsagePercent"
#define RFPROP_NPU_HBM_BW "HBMBandwidthUsagePercent"
#define RFPROP_NPU_AICPU_UTL "AiCPUUsagePercent"
#define RFPROP_NPU_ECC_INFO             "EccInfo"
#define RFPROP_NPU_NAME                 "Name"
#define RFPROP_NPU_POSITION             "Position"
#define RFPROP_NPU_SINGLE_BIT_ECC       "SingleBitEcc"
#define RFPROP_NPU_MULTI_BIT_ECC        "MultiBitEcc"
#define RFPROP_NPU_ECC_COUNT            "Count"
#define RFPROP_NPU_INFO                 "Info"
#define RFPROP_NPU_PHY_ADDR             "PhysicalAddress"
#define RFPROP_NPU_ECC_STACK_PC_ID      "StackPcId"
#define RFPROP_NPU_ECC_ROW_COLUMN       "RowColumn"
#define RFPROP_NPU_ECC_BANK             "Bank"
#define RFPROP_NPU_ECC_ADDR_ERROR_COUNT "ErrorCount"
#define RFPROP_NPU_ECC_TIME             "Time"


#define RFPROP_SYSTEM_PROCESEEOR_OTHERPARAMETERS "OtherParameters"
#define RFPROP_SYSTEM_PROCESEEOR_ONECACHE "L1CacheKiB"
#define RFPROP_SYSTEM_PROCESEEOR_TWOCACHE "L2CacheKiB"
#define RFPROP_SYSTEM_PROCESEEOR_THREECACHE "L3CacheKiB"
#define RFPROP_SYSTEM_PROCESEEOR_TEMPERATURE "Temperature"
#define RFPROP_SYSTEM_PROCESEEOR_ENABLEDSETTING "EnabledSetting"
#define RFPROP_SYSTEM_PROCESEEOR_TYPE "ProcessorType"

#define RFPROP_GPU_CARD_FIRMWARE_VERSION             "FirmwareVersion"
#define RFPROP_GPU_CARD_STATE                        "State"
#define RFPROP_GPU_CARD_HEALTH                       "Health"
#define RFPROP_GPU_CARD_SLOT_NUMBER                  "SlotNumber"
#define RFPROP_GPU_CARD_DEVICE_LOCATOR               "DeviceLocator"
#define RFPROP_GPU_CARD_SERIAL_NUMBER                "SerialNumber"
#define RFPROP_GPU_CARD_POSITION                     "Position"
#define RFPROP_GPU_CARD_INFO_ROM_VERSION             "InfoRomVersion"
#define RFPROP_GPU_CARD_LINK_WIDTH_ABILITY           "LinkWidthAbility"
#define RFPROP_GPU_CARD_LINK_SPEED_ABILITY           "LinkSpeedAbility"
#define RFPROP_GPU_CARD_LINK_WIDTH                   "LinkWidth"
#define RFPROP_GPU_CARD_LINK_SPEED                   "LinkSpeed"
#define RFPROP_GPU_CARD_INVENTORY                    "Inventory"
#define RFPROP_GPU_CARD_BOARD_PART_NUMBER            "BoardPartNumber"
#define RFPROP_GPU_CARD_PART_NUMBER                  "GPUPartNumber"
#define RFPROP_GPU_CARD_MEMORY_VENDOR                "MemoryVendor"
#define RFPROP_GPU_CARD_MEMORY_PART_NUMBER           "MemoryPartNumber"
#define RFPROP_GPU_CARD_BUILD_DATE                   "BuildDate"
#define RFPROP_GPU_CARD_PCIE_VID                     "PCIeVendorId"
#define RFPROP_GPU_CARD_PCIE_DID                     "PCIeDeviceId"
#define RFPROP_GPU_CARD_PCIE_SVID                    "PCIeSubSystemVendorId"
#define RFPROP_GPU_CARD_PCIE_SDID                    "PCIeSubSystemDeviceId"
#define RFPROP_GPU_CARD_UUID                         "UUID"
#define RFPROP_GPU_CARD_POWER                        "Power"
#define RFPROP_GPU_CARD_POWER_DRAW_WATTS             "PowerDrawWatts"
#define RFPROP_GPU_CARD_POWER_BRAKE_IS_SET           "PowerBrakeIsSet"
#define RFPROP_GPU_CARD_SUFFICIENT_EXTERNAL_POWER    "SufficientExternalPower"
#define RFPROP_GPU_CARD_STATISTICS                   "Statistics"
#define RFPROP_GPU_CARD_ERROR_STATUS                 "ErrorStatus"
#define RFPROP_GPU_CARD_ECC_MODE_ENABLED             "ECCModeEnabled"
#define RFPROP_GPU_CARD_ECC_MODE_PENDING_ENABLED     "ECCModePendingEnabled"
#define RFPROP_GPU_CARD_RESET_REQUIRED               "GPUResetRequired"
#define RFPROP_GPU_CARD_SINGLE_BIT_ERROR_COUNTS      "SingleBitErrorCounts"
#define RFPROP_GPU_CARD_DOUBLE_BIT_ERROR_COUNTS      "DoubleBitErrorCounts"
#define RFPROP_GPU_CARD_RETIRED_PAGES                "RetiredPages"
#define RFPROP_GPU_CARD_DUE_TO_SINGLE_BIT_ERRORS     "DueToMultipleSingleBitErrors"
#define RFPROP_GPU_CARD_DUE_TO_DOUBLE_BIT_ERRORS     "DueToDoubleBitErrors"
#define RFPROP_GPU_CARD_PAGE_COUNT                   "PageCount"
#define RFPROP_GPU_CARD_NVLINK                       "NvLink"
#define RFPROP_GPU_CARD_LINK_STATUS                  "LinkStatus"
#define RFPROP_GPU_CARD_LINK_SPEED_MBPS              "LinkSpeedMbps"
#define RFPROP_GPU_CARD_REPLAY_ERROR_MBPS            "ReplayErrorCount"
#define RFPROP_GPU_CARD_RECOVERY_ERROR_COUNT         "RecoveryErrorCount"
#define RFPROP_GPU_CARD_FLIT_CRC_ERROR_COUNT         "FlitCRCErrorCount"
#define RFPROP_GPU_CARD_DATA_CRC_ERROR_COUNT         "DataCRCErrorCount"
#define RFPROP_GPU_CARD_THERMALS                     "Thermals"
#define RFPROP_GPU_CARD_THERMALS_ALERT_STATE         "ThermalsAlertState"
#define RFPROP_GPU_CARD_PRIMARY_GPU_TEMP_CELSIUS     "PrimaryGPUTemperatureCelsius"
#define RFPROP_GPU_CARD_MEMORY_TEMP_CELISIUS         "MemoryTemperatureCelsius"
#define RFPROP_GPU_CARD_UTILIZATION                  "Utilization"
#define RFPROP_GPU_CARD_UTILIZATION_GPU              "GPUUsagePercent"
#define RFPROP_GPU_CARD_UTILIZATION_MEM              "MemoryUsagePercent"

#define RFPROP_PART_NUMBER "PartNumber"
#define RFPROP_PROCESSOR_FREQUENCY "FrequencyMHz"
#define RFPROP_REGRIST_ODTAID_URI "/redfish/v1/Registries/%s"
#define RFPROP_REGRIST_REGISTRY_URI "/redfish/v1/RegistryStore/AttributeRegistries/en/%s.json"
#define RFPROP_REGRIST_ODTAID_RESOURCE "^/redfish/v1/registries/biosattributeregistry[a-zA-Z0-9_\\.]+$"
#define RFPROP_REGRIST_LANGUAGE "Language"
#define RFPROP_REGRIST_ENTRIES "RegistryEntries"
#define RFPROP_REGRIST_DEPENDENCIES "Dependencies"
#define RFPROP_REGRIST_SUPPORTEDSYSTEMS "SupportedSystems"
#define RFPROP_REGRIST_BIOS_PRODUCTNAME "ProductName"
#define RFPROP_REGRIST_BIOS_SYSTEMID "SystemId"
#define RFPROP_REGRIST_BIOS_FIRMWAREVERSION "FirmwareVersion"
#define RFPROP_REGRIST_BIOS_OWNING_ENTITY "OwningEntity"
#define RFPROP_VAL_REGRIST_BIOS_NAME "Bios Attribute Registry"
#define RFPROP_VAL_REGRIST_BIOS_DESC "This registry defines the Bios Attribute for Redfish"


#define MAX_DEPEND_PROP 5
#define THE_VALUE_FOR_THE_PROPERTY_BLANK "the value for the property "
#define BLANK_IS_BLANK " is "
#define BLANK_OR_BLANK " or "
#define BLANK_AND_BLANK " and "
#define COMMA_BLANK ", "



#define MEMORY_NAME_LEN 64
#define MEMORY_DEVICE_LEN 32
#define MEMORY_URI_LEN 80
#define MEMORY_CAP_LEN 16

#define MEMORY_MANFAC_LEN 64

#define MEMORY_SPEED_LEN 16
#define MEMORY_SN_LEN 32
#define MEMORY_MANUFACTURER_ID_LEN 16
#define MEMORY_MANUFACTURING_LOCATION_LEN 16
#define MEMORY_MANUFACTURING_DATE_LEN 16

#define MEMORY_NO_DIMM "NO DIMM"
#define MEMORY_IS_NULL "null"
#define MEMORY_UNKNOWN "Unknown"
#define MEMORY_UNKNOWN_2 " Unknown"
#define MEMORY_NA "N/A"

#define RFPROP_MEMORY_ODATA_ID "@odata.id"
#define RFPROP_MEMORY_ID "Id"
#define RFPROP_MEMORY_NAME "Name"
#define RFPROP_MEMORY_CAPMIB "CapacityMiB"
#define RFPROP_MEMORY_MANFAC "Manufacturer"
#define RFPROP_MEMORY_SPEEDMHZ "OperatingSpeedMhz"
#define RFPROP_MEMORY_ALLOWSPEEDMHZ "AllowedSpeedsMHz"
#define RFPROP_MEMORY_STATUS "Status"
#define RFPROP_MEMORY_SN "SerialNumber"
#define RFPROP_MEMORY_ORIGINAL_PART_NUM "OriginalPartNumber"
#define RFPROP_MEMORY_BOM_NUM "BomNumber"
#define RFPROP_MEMORY_ECC_COUNT  "EccCount"

#define RFPROP_MEMORY_DEVICETYPE "MemoryDeviceType"
#define RFPROP_MEMORY_DATAWIDTH "DataWidthBits"
#define RFPROP_MEMORY_RANKCOUNT "RankCount"
#define RFPROP_MEMORY_DEVICELOCATOR "DeviceLocator"
#define RFPROP_MEMORY_MEMORYLOCATION "MemoryLocation"
#define RFPROP_MEMORY_SOCKET "Socket"
#define RFPROP_MEMORY_CONTROLLER "Controller"
#define RFPROP_MEMORY_CHANNEL "Channel"
#define RFPROP_MEMORY_SLOT "Slot"

#define RFPROP_MEMORY_MINVOLTAGE "MinVoltageMillivolt"
#define RFPROP_MEMORY_TECHNOLOGY "Technology"
#define RFPROP_MEMORY_TYPE_DETAIL "TypeDetail"
#define RFPROP_MEMORY_POSITION "Position"
#define RFPROP_MEMORY_CHIP_BIT_WIDTH "ChipBitWidth"
#define RFPROP_MEMORY_MANUFACTURER_ID "ManufacturerID"
#define RFPROP_MEMORY_MANUFACTURING_LOCATION "ManufacturingLocation"
#define RFPROP_MEMORY_MANUFACTURING_DATE "ManufacturingDate"
#define RFPROP_MEMORY_METRICS "Metrics"


#define RFPROP_MEMORY_REMAINING_SERVICE_LIFE "RemainingServiceLifePercent"
#define RFPROP_MEMORY_MEDIUM_TEMPERATURE "MediumTemperatureCelsius"
#define RFPROP_MEMORY_CONTROL_TEMPERATURE "ControllerTemperatureCelsius"
#define RFPROP_MEMORY_FIRMWARE_VERSION "FirmwareRevision"
#define RFPROP_MEMORY_VOLATILE_CAPACITY "VolatileRegionSizeLimitMiB"
#define RFPROP_MEMORY_PERSISTENT_CAPACITY "PersistentRegionSizeLimitMiB"


#define RFPROP_FPCSERVICE_MEMORY_COUNT    "Count"
#define RFPROP_FPCSERVICE_MEMORY_RECORDS    "Records"
#define RFPROP_FPCSERVICE_MEMORY_TIMESTAMP    "TimeStamp"
#define RFPROP_FPCSERVICE_MEMORY_TIMEZONE   "TimeZone"
#define RFPROP_FPCSERVICE_MEMORY_SOCKET   "Socket"
#define RFPROP_FPCSERVICE_MEMORY_CHANNEL   "Channel"
#define RFPROP_FPCSERVICE_MEMORY_SLOT   "Slot"
#define RFPROP_FPCSERVICE_MEMORY_DEVICELOCATOR   "DeviceLocator"
#define RFPROP_FPCSERVICE_MEMORY_SN   "SerialNumber"
#define RFPROP_FPCSERVICE_MEMORY_HEALTHSCORE   "HealthScore"
#define RFPROP_FPCSERVICE_MEMORY_LAST_ISOLATION_STATUS  "IsolationStatus"

#define RFPROP_SPSERVICE_FILE_LIST "FileList"
#define RFPROP_SPSERVICE_SIGNATURE "Signature"

#define RFPROP_SPSERVICE_STARTFLAG "SPStartEnabled"
#define RFPROP_SPSERVICE_TIMEDELY "SysRestartDelaySeconds"

#define RFPROP_SPSERVICE_TIMEOUT "SPTimeout"
#define RFPROP_SPSERVICE_FINISHED "SPFinished"
#define RFPROP_SPSERVICE_DEVICE_INFO_COLLECT_ENABLED  "SPDeviceInfoCollectEnabled"

#define RFPROP_SPSERVICE_UP_TSSTATE "TransferState"
#define RFPROP_SPSERVICE_UP_TSFILENAME "TransferFileName"
#define RFPROP_SPSERVICE_UP_TSPROGRESSPERCENT "TransferProgressPercent"
#define RFPROP_SPSERVICE_UP_UPGRADEPROGRESS "UpgradeProgress"
#define RFPROP_SPSERVICE_UP_MESSAGES "Messages"
#define RFPROP_SPSERVICE_UP_TSFILELIST "FileList"
#define RFPROP_SPSERVICE_UP_VERSION "Version"


#define RFPROP_SPSERVICE_UP_VERSION_APP     "APPVersion"
#define RFPROP_SPSERVICE_UP_REVISON_OS      "OSRevision"
#define RFPROP_SPSERVICE_UP_VERSION_DATA    "DataVersion"
#define RFPROP_SPSERVICE_UP_VERSION_OS      "OSVersion"
#define RFPROP_SPSERVICE_UP_MAJORVERSION    "MajorVersion"



#define RFPROP_SPSERVICE_UP_TSSTATE_SUCCESS "Success"
#define RFPROP_SPSERVICE_UP_TSSTATE_FAILURE "Failure"
#define RFPROP_SPSERVICE_UP_TSSTATE_PROCESSING "Processing"

#define RFPROP_SPSERVICE_RAID "SPRAID"
#define RFPROP_SPSERVICE_RAID_CURRENT_CFG "SPRAIDCurrentConfigurations"
#define RFPROP_SPSERVICE_OS "SPOSInstallPara"
#define RFPROP_SPSERVICE_FWUPDATE "SPFWUpdate"
#define RFPROP_SPSERVICE_SPRESULT "SPResult"
#define RFPROP_SPSERVICE_SPCFG "SPCfg"
#define RFPROP_SPSERVICE_DEVICEINFO "DeviceInfo"

#define RFPROP_SPSERVICE_STATUS "Status"

#define RFPROP_SPSERVICE_DIAGNOSE "SPDiagnose"
#define RFPROP_SPSERVICE_DRIVE_ERASE "SPDriveErase"

#define RF_UIR_NEXT_SPSERVICE_RAID "SPService/SPRAID"
#define RF_UIR_NEXT_SPSERVICE_RAID_CURRNT_CFG "SPService/SPRAIDCurrentConfigurations"
#define RF_UIR_NEXT_SPSERVICE_OS "SPService/SPOSInstallPara"
#define RF_UIR_NEXT_SPSERVICE_FWUPDATE "SPService/SPFWUpdate"
#define RF_UIR_NEXT_SPSERVICE_SPRESULT "SPService/SPResult"
#define RF_UIR_NEXT_SPSERVICE_SPCFG "SPService/SPCfg"
#define RF_UIR_NEXT_SPSERVICE_DEVICEINFO "SPService/DeviceInfo"
#define RF_UIR_NEXT_SPSERVICE_DIAGNOSE "SPService/SPDiagnose"
#define RF_UIR_NEXT_SPSERVICE_DRIVE_ERASE "SPService/SPDriveErase"

#define RFPROP_MANAGER_SP_ACTION_IM_URI "ImageURI"
#define RFPROP_MANAGER_SP_ACTION_SI_URI "SignalURI"
#define RFPROP_MANAGER_SP_ACTION_IM_TYPE "ImageType"
#define RFPROP_MANAGER_SP_ACTION_PARA "Parameter"
#define RFPROP_MANAGER_SP_ACTION_UP_MODE "UpgradeMode"
#define RFPROP_MANAGER_SP_ACTION_ACTIVE_MODE "ActiveMethod"

#define RFPROP_MANAGER_SP_IM_TYPE_FIRMWARE "Firmware"



#define SP_ACTION_IM_URI_REGEX "^(https|sftp|nfs|cifs|scp|file)://.{1,1000}\\.(zip)$"
#define SP_ACTION_SI_URI_REGEX "^(https|sftp|nfs|cifs|scp|file)://.{1,1000}\\.(asc|p7s)$"




#define SP_FORWARD_SPRESULT_URI "/redfish/v1/Sms/%d/Managers/1/SPService/SPResult/1"


#define RFPROP_MANAGER_SP_OS_NAME "osinstall"
#define RFPROP_MANAGER_SP_CFG_NAME "spcfg"
#define RFPROP_MANAGER_SP_RESULT_NAME "result"
#define RFPROP_MANAGER_SP_SUPPORT_NAME "support"


#define RFPROP_MANAGER_SP_DEVICEINFO_NAME "deviceinfo"

#define RFPROP_MANAGER_SP_DIAGNOSE_NAME "diagnose"
#define RFPROP_MANAGER_SP_DRIVE_ERASE_NAME "driveerase"

#define RFPROP_MANAGER_SP_POST_ACTION_NAME "SPService.DeleteFile"
#define RFPROP_MANAGER_SP_ACTION_NAME "#SPService.DeleteFile"
#define RFPROP_MANAGER_SP_ACTION_INFONAME "@Redfish.ActionInfo"
#define RFPROP_MANAGER_SP_ACTION_TARGETURI "/redfish/v1/Managers/%s/SPService/Actions/SPService.DeleteFile"
#define RFPROP_MANAGER_SP_ACTION_INFOURI "/redfish/v1/Managers/%s/SPService/DeleteFileActionInfo"

#define RFPROP_MANAGER_SP_UP_POST_ACTION_NAME "SPFWUpdate.SimpleUpdate"
#define RFPROP_MANAGER_SP_UP_ACTION_NAME "#SPFWUpdate.SimpleUpdate"
#define RFPROP_MANAGER_SP_UP_ACTION_INFONAME "@Redfish.ActionInfo"
#define RFPROP_MANAGER_SP_UP_ACTION_TARGETURI \
    "/redfish/v1/Managers/%s/SPService/SPFWUpdate/1/Actions/SPFWUpdate.SimpleUpdate"
#define RFPROP_MANAGER_SP_UP_ACTION_INFOURI "/redfish/v1/Managers/%s/SPService/SPFWUpdate/1/SimpleUpdateActionInfo"
#define SPRAID_EXPORT_CFG_FILE_SUFFIX "_current_cfg"
#define SPRAID_TRIGGER_EXPORT_CFG_FILE "export_cfg.json"
#define RFPROP_MANAGER_SPRAID_EXPORT_ACTION "SPService.ExportSPRAIDConfigurations"
#define RFPROP_MANAGER_SP_COPY_SCHEMA_ACTION "SPService.UpdateSchemaFiles"
#define RFPROP_MANAGER_SPRAID_EXPORT_ACTION_INFO "ExportSPRAIDConfigurationsActionInfo"
#define RFPROP_MANAGER_SPRAID_EXPORT_ACTION_INFOURI \
    "/redfish/v1/Managers/%s/SPService/ExportSPRAIDConfigurationsActionInfo"
#define RFPROP_MANAGER_UPDATE_SCHEMA_ACTION_INFOURI "/redfish/v1/Managers/%s/SPService/UpdateSchemaFilesActionInfo"
#define SP_FILE_SPLITE_FLAG "/"
#define RF_UPDATE_SPSERVICE_SCHEMA_FILES "Update SP Schema Files Task"
#define RF_PROP_CHECKSUM_VALUE "ChecksumValue"
#define RF_PROP_SP_SCHEMA_FILE_NAME "FileName"
#define REDFISH_UPLOAD_FILE_PATH "/tmp/web"
#define UPDATE_SCHEMA_IN_PROCESS 4
#define SP_FILE_TYPE "Type"
#define SP_FILE_NAME "Name"

#define SP_FILE_TYPE_RAID "RAIDCfg"
#define SP_FILE_TYPE_FWUPDATE "FWUpdateCfg"
#define SP_FILE_TYPE_SPCFG "SPCfg"
#define SP_FILE_TYPE_SPOSINSTALL "OSInstall"
#define SP_FILE_TYPE_SPDRIVEERASE "DriveEraseCfg"

#define SP_SCHEMA_FILE_NAME_SPCFG "hwspcfg.v1_0_0.json"
#define SP_SCHEMA_FILE_NAME_OSINSTALL "hwsposinstallpara.v1_0_0.json"
#define SP_SCHEMA_FILE_NAME_RAID "hwspraid.v1_0_0.json"
#define SP_SCHEMA_FILE_NAME_RESULT "hwspresult.v1_0_0.json"
#define SP_SCHEMA_FILE_NAME_DEVICEINFO "hwdeviceinfo.v1_0_0.json"
#define SP_SCHEMA_FILE_NAME_DIAGNOSE "hwspdiagnose.v1_0_0.json"
#define SP_SCHEMA_FILE_NAME_DRIVE_ERASE "hwspdriveerase.v1_0_0.json"


#define SP_SCHEMA_FILE_DES_NAME_SPCFG "HwSPCfg"
#define SP_SCHEMA_FILE_DES_NAME_OSINSTALL "HwSPOSInstallPara"
#define SP_SCHEMA_FILE_DES_NAME_RAID "HwSPRAID"
#define SP_SCHEMA_FILE_DES_NAME_RESULT "HwSPResult"

#define SP_SCHEMA_FILE_DEEFINITIONS "definitions"
#define SP_SCHEMA_FILE_PROPERTIER "properties"

#define SP_MIN_TIMEOUT 300
#define SP_MAX_TIMEOUT 86400
#define SP_DEFAULT_TIMEOUT 7200

#define SP_PROP_NAME_CDKEY "CDKey"
#define SP_PROP_NAME_ROOT_PWD "RootPwd"

typedef struct tag_sp_sensitive_s {
    guint8 file_id;
    const gchar *prop_name;
} SP_SENSITIVE_PROP_S;


#define RF_HDD_PRESENCE 1
#define MAX_STRBUF_LEN 128
#define MAX_BUF_LEN 512
#define MAX_STATUS_LEN 32
#define RF_DRIVES_NAME_LEN 10

#define RF_DEVIES_CAPACITY_ERROR 0xFFFFFFFF
#define RF_DRIVES_MEDIATYPE_VALUES 1

#define MSM_PRIMARY_STATE          1
#define MSM_SECONDARY_STATE        0

#define MAX_ARRAY_LEN   128
#define CHASSIS_SERIAL_NUM  0x11

// Drive 类中属性宏定义
#define RFPROP_SYSTEM_STORAGE_DRIVE_MODEL "Model"                            // 型号
#define RFPROP_SYSTEM_STORAGE_DRIVE_REVISION "Revision"                      // 固件版本
#define REPROP_SYSTEM_STORAGE_DRIVE_STATUS "Status"                          // 健康状态
#define RFPROP_SYSTEM_STORAGE_DRIVE_CAPACITYBYTES "CapacityBytes"            // 容量
#define RFPROP_SYSTEM_STORAGE_DRIVE_BLOCK_SIZE    "BlockSizeBytes"           // 块大小
#define RFPROP_SYSTEM_STORAGE_DRIVE_FAILUREPREDICTED "FailurePredicted"      // 是否检测到预故障
#define RFPROP_SYSTEM_STORAGE_DRIVE_PROTOCOL "Protocol"                      // 接口协议
#define RFPROP_SYSTEM_STORAGE_DRIVE_MEDIATYPE "MediaType"                    // 媒质类型
#define RFPROP_SYSTEM_STORAGE_DRIVE_MANUFACTURER "Manufacturer"              // 制造厂商
#define RFPROP_SYSTEM_STORAGE_DRIVE_SERIAL_NUMBER "SerialNumber"             // 序列号
#define RFPROP_SYSTEM_STORAGE_DRIVE_CAPABLE_SPEEDGBS "CapableSpeedGbs"       // 接口支持速率
#define RFPROP_SYSTEM_STORAGE_DRIVE_NEGOTIATED_SPEEDGBS "NegotiatedSpeedGbs" // 接口协商速率
#define RFPROP_SYSTEM_STORAGE_DRIVE_PREDICTED_MEDIA_LIFEFT_PERCENT "PredictedMediaLifeLeftPercent" // 剩余寿命百分比
#define RFPROP_SYSTEM_STORAGE_DRIVE_STATUSINDICATOR "StatusIndicator"

#define RFACTION_DRIVE_CRYPTO_ERASE "Oem/Huawei/Drive.CryptoErase"


#define RFPROP_DRIVE_INDICATORLED "IndicatorLED" // 硬盘指示灯状态
#define RF_INDICATORLED_OFF "Off"
#define RF_INDICATORLED_BLINKING "Blinking"
#define RFPROP_DRIVE_HOTSPARETYPE "HotspareType" // 热备盘类型
#define RF_HOTSPARETYPE_NONE "None"
#define RF_HOTSPARETYPE_GLOBAL "Global"
#define RF_HOTSPARETYPE_DEDICATED "Dedicated"
#define RF_HOTSPARETYPE_AUTO_REPLACE "Auto Replace"
#define RFPROP_DRIVE_LOCATION "Location" // 硬盘丝印
#define RFPROP_DRIVE_ROTATION_SPEED_RPM "RotationSpeedRPM"
#define RF_LOCATION_INFO "Info"
#define RF_LOCATION_INFOFORMAT "InfoFormat"

#define RF_LOCATION_POSTAL_ADDRESS "PostalAddress"
#define RF_LOCATION_PLACEMENT "Placement"
#define RF_LOCATION_COUNTRY "Country"
#define RF_LOCATION_PROVINCE "Territory"
#define RF_LOCATION_CITY "City"
#define RF_LOCATION_STREET "Street"
#define RF_LOCATION_HOUSE_NUMBER "HouseNumber"
#define RF_LOCATION_NAME "Name"
#define RF_LOCATION_POSTAL_CODE "PostalCode"
#define RF_LOCATION_BUILDING "Building"
#define RF_LOCATION_FLOOR "Floor"
#define RF_LOCATION_ROOM "Room"
#define RF_LOCATION_ROW "Row"
#define RF_LOCATION_RACK "Rack"
#define RF_LOCATION_UCOUNT "TotalUCount"
#define RF_LOCATION_UCOUNT_USED "UcountUsed"
#define RF_LOCATION_DIRECTION "Direction"
#define RF_LOCATION_DIRECTION_TYPE "DirectionType"
#define RF_LOCATION_LOAD_CAPACITY "LoadCapacityKg"
#define RF_LOCATION_RATED_POWER "RatedPowerWatts"
#define RF_LOCATION_LOCATION "Location"
#define RF_LOCATION_RACK_MODEL "RackModel"
#define RF_LOCATION_RACK_FUNCTION "RackFunction"
#define RF_LOCATION_TYPICAL_CONF_RACK_SN "TypicalConfigurationRackSN"
#define RF_LOCATION_EMPTY_RACK_SN "EmptyRackSN"
#define RF_LOCATION_BASIC_RACK_SN "BasicRackSN"
#define RF_DIRECTION_FROM_BOTTOM_UP "FromBottomUp"
#define RF_DIRECTION_FROM_TOP_DOWN "FromTopDown"
#define RF_UNIT_OCCUPY_DIRECTION "UnitOccupyDirection"
#define RF_UNIT_OCCUPY_DIRECTION_DOWNWARD "Downward"
#define RF_UNIT_OCCUPY_DIRECTION_UPWARD "Upward"

#define RF_FROM_BOTTOM_UP 0
#define RF_FROM_TOP_DOWN 1



#define RFPROP_DRIVE_OEM_REBUILDSTATE "RebuildState" // 重构状态
#define RF_REBUILDSTATE_REBUILDING "Rebuilding"

#define RF_REBUILDSTATE_REBUILDORNOREBUILD "DoneOrNotRebuilt"
#define RFPROP_DRIVE_OEM_EBUILDPROGRESS "RebuildProgress"
#define RF_EBUILDPROGRESS_UNKNOWNREBUILDPROGRESS "UnknownRebuildProgress"
#define RFPROP_DRIVE_OEM_PATROLSTATE "PatrolState" // 巡检状态
#define RF_PATROLSTATE_PATROLEDORNOPATROLED "DoneOrNotPatrolled"
#define RF_PATROLSTATE_PATROLLING "Patrolling"
#define RFPROP_DRIVE_OEM_TEMPERATURE "TemperatureCelsius"
#define RFPROP_DRIVE_OEM_PHYSICALDISKSTATE "FirmwareStatus" // 物理盘状态
#define RFPROP_DRIVE_OEM_IS_EPD "IsEPD"
#define RFPROP_DRIVE_OEM_SLOTNUMBER "SlotNumber"
#define RFPROP_DRIVE_OEM_ENCLOSUREDEVICEID "EnclosureDeviceId"
#define RFPROP_DRIVE_OEM_FORM_FACTOR "FormFactor"
#define RF_PHYSICALDISKSTATE_UNCONFIGUREDGOOD "UnconfiguredGood"
#define RF_PHYSICALDISKSTATE_UNCONFIGUREDBAD "UnconfiguredBad"

#define RF_PHYSICALDISKSTATE_HOTSPARE "HotSpareDrive"
#define RF_PHYSICALDISKSTATE_CONFIGUREDGOOD "Offline"

#define RF_PHYSICALDISKSTATE_CONFIGUREDBAD "Failed"
#define RF_PHYSICALDISKSTATE_ACTIVE "Active"
#define RF_PHYSICALDISKSTATE_STANDBY "Standby"
#define RF_PHYSICALDISKSTATE_SLEEP "Sleep"
#define RF_PHYSICALDISKSTATE_DST "DSTInProgress"
#define RF_PHYSICALDISKSTATE_SMART "SMARTOfflineDataCollection"
#define RF_PHYSICALDISKSTATE_SCT "SCTCommand"

#define RF_PHYSICALDISKSTATE_REBUILDING "Rebuilding"
#define RF_PHYSICALDISKSTATE_ONLINE "Online"
#define RF_PHYSICALDISKSTATE_RAW "Raw"
#define RF_PHYSICALDISKSTATE_READY "Ready"
#define RF_PHYSICALDISKSTATE_NOTSUPPORTED "NotSupported"
#define RF_PHYSICALDISKSTATE_DIAGNOSING "Diagnosing"
#define RF_PHYSICALDISKSTATE_INCOMPATIBLE  "Incompatible"
#define RF_PHYSICALDISKSTATE_ERASING "EraseInProgress"
#define RF_PHYSICALDISKSTATE_PREDICTIVE_FAILURE "PredictiveFailure"
#define RF_PHYSICALDISKSTATE_GETTINGCOPIED "GettingCopied"
#define RF_PHYSICALDISKSTATE_CONTROLLEDBYHOST "JBOD"
#define RF_PHYSICALDISKSTATE_UNCONFIGUREDSHIELDED "UnconfiguredShielded"
#define RF_PHYSICALDISKSTATE_HOTSPARESHIELDED "HotSpareShielded"
#define RF_PHYSICALDISKSTATE_CONFIGUREDSHIELDED "ConfiguredShielded"
#define RFPROP_DRIVE_OEM_HOTFORLOGICAL "SpareforLogicalDrives" // 热备盘所属的逻辑盘
#define RFPROP_DRIVE_OEM_SASADDRESS "SASAddress"               // 线缆地址
#define RFPROP_DRIVE_LINKS "Links"                             // 链接属性
#define RF_LINKS_VOLUMES "Volumes"
#define DRIVE_LINK_COMMEN "/redfish/v1/Systems/1/Storages/1/Volumes/"
#define RFPROP_DRIVE_DEVICENAME "DeviceName"
#define DRIVE_REBUILTING_VALUE 1
#define DRIVE_SET_INDICATOR_TIME 0
#define DRIVE_SET_HOTSPARE_VALUE 0
#define DRIVE_INDICATORLED_OFF 0
#define DRIVE_INDICATORLED_BLINKING 1
#define DRIVE_INVALID_VALUE 0xFF
#define DRIVE_OEM_PROPERTY_SPARE "Oem/Huawei/SpareforLogicalDrives"

#define RF_PHYSICALDISKSTATE_FOREIGN "Foreign"


#define DRIVE_FAULT_OK 0
#define DRIVE_FAULT_YES 1



#define RFPROP_DRIVE_DRIVEID "DriveID"



#define DRIVE_TYPE_DISK "Disk"

#define DRIVE_TYPE_PCIESSDCARD "PCIe SSD Card"

#define DRIVE_SMART_INVALID_U32_VALUE 0xFFFFFFFF
#define DRIVE_SMART_INVALID_U16_VALUE 0xFFFF
#define RFPROP_DRIVE_INTERFACE_SATA "SATA"
#define RFPROP_DRIVE_INTERFACE_SAS "SAS"
#define RFPROP_DRIVE_INTERFACE_PCIE "PCIe"
#define RFPROP_DRIVE_OEM_TYPE "Type"
#define RFPROP_DRIVE_OEM_POSITION "Position"
#define RFPROP_DRIVE_OEM_POWERUP "HoursOfPoweredUp"
#define RFPROP_DRIVE_OEM_SATASMARTINFO "SATASmartInformation"
#define RFPROP_DRIVE_OEM_NVMESMARTINFO "NVMeSmartInformation"
#define RFPROP_DRIVE_OEM_BOOT_ENABLE "BootEnable"
#define RFPROP_DRIVE_OEM_BOOT_PRIORITY "BootPriority"
#define RFPROP_SATASMART_ATTRREVISION "AttributeRevision"
#define RFPROP_SATASMART_ATTRREVISIONNUM "AttributeRevisionNumber"
#define RFPROP_SATASMART_ATTRREVISIONLIST "AttributeItemList"
#define RFPROP_SATASMART_ATTR_RAWVALUE "RawValue"
#define RFPROP_SATASMART_ATTR_UPDATEMODE "UpdateMode"
#define RFPROP_SATASMART_ATTR_NAME "AttributeName"
#define RFPROP_SATASMART_ATTR_NORMALVALUE "NormalizedValue"
#define RFPROP_SATASMART_ATTR_FLAG "Flag"
#define RFPROP_SATASMART_ATTR_WORSTVALUE "WorstValue"
#define RFPROP_SATASMART_ATTR_THRESHOLD "Threshold"
#define RFPROP_SATASMART_ATTR_STATUS "FaultStatus"
#define RFPROP_SATASMART_ATTR_MODE "FaultWarningMode"
#define RFPROP_SATASMART_ATTR_ID "SmartAttributeID"
#define RFPROP_DRIVE_OEM_SASSMARTINFO "SASSmartInformation"
#define RFPROP_SASSMART_MEDIA_ERROR_COUNT "MediaErrorCount"
#define RFPROP_SASSMART_PREFAIL_ERROR_COUNT "PrefailErrorCount"
#define RFPROP_SASSMART_OTHER_ERROR_COUNT "OtherErrorCount"
#define RFPROP_SASSMART_HEALTH_STATUS "HealthStatus"
#define RFPROP_SASSMART_DISK_TEMPERATURE "DiskTemperatureCelsius"
#define RFPROP_SASSMART_DISK_STRIPE_TEMPERATURE "DiskStripeTemperatureCelsius"
#define RFPROP_SASSMART_ELEMENTS_IN_GROWN_DEFECT_LIST "ElementsInGrownDefectList"
#define RFPROP_SASSMART_ELEMENTS_IN_PRIMARY_DEFECT_LIST "ElementsInPrimaryDefectList"
#define RFPROP_SASSMART_WEEK_AND_YEAR_OF_MANUFACTURE "WeekAndYearOfManufacture"
#define RFPROP_SASSMART_BLOCKS_SEND_TO_INITIATOR "BlocksSentToInitiator"
#define RFPROP_SASSMART_BLOCKS_RECEIVED_FROM_INITIATOR "BlocksReceivedFromInitiator"
#define RFPROP_SASSMART_NUM_OF_MINUTES_UNTIL_NEXT_TEST "NumberOfMinutesUntilNextSMARTTest"
#define RFPROP_SASSMART_SEQ_NUM_OF_LAST_PREDFAIL_EVENT "SequenceNumberOfLastPredFailEvent"
#define RFPROP_DRIVE_OK "OK"
#define RFPROP_DRIVE_FAIL "Fail"
#define RFPROP_DRIVE_REBUILD "Rebuild"
#define RFPROP_DRIVE_HOTSPARE "Hotspare"
#define RFPROP_DRIVE_PRE_FAIL_ANALYSIS "PredictiveFailureAnalysis"
#define RFPROP_DRIVE_INAFAILEDARRAY "InAFailedArray"
#define RFPROP_DRIVE_POWER_STATE "PowerState"
#define RFPROP_DRIVE_POWER_STATE_V_SPUNUP "Spun Up"
#define RFPROP_DRIVE_POWER_STATE_V_SPUNDOWN "Spun Down"
#define RFPROP_DRIVE_POWER_STATE_V_TRANSITION "Transition"
#define RFPROP_DRIVE_POWER_STATE_V_SPUNUP_NUM 0
#define RFPROP_DRIVE_POWER_STATE_V_SPUNDOWN_NUM 1
#define RFPROP_DRIVE_POWER_STATE_V_TRANSITION_NUM 2

#define RFPROP_DRIVE_OEM_FORM_FACTOR_UNKNOWN                "Unknown"
#define RFPROP_DRIVE_OEM_FORM_FACTOR_5_25_INCH              "5.25 inch"
#define RFPROP_DRIVE_OEM_FORM_FACTOR_3_5_INCH               "3.5 inch"
#define RFPROP_DRIVE_OEM_FORM_FACTOR_2_5_INCH               "2.5 inch"
#define RFPROP_DRIVE_OEM_FORM_FACTOR_1_8_INCH               "1.8 inch"
#define RFPROP_DRIVE_OEM_FORM_FACTOR_LESS_THAN_1_8_INCH     "Less than 1.8 inch"
#define RFPROP_DRIVE_OEM_FORM_FACTOR_MSATA                  "mSATA"
#define RFPROP_DRIVE_OEM_FORM_FACTOR_M2                     "M.2"
#define RFPROP_DRIVE_OEM_FORM_FACTOR_MICRO_SSD              "MicroSSD"
#define RFPROP_DRIVE_OEM_FORM_FACTOR_CFAST                  "CFast"


#define RFRESC_OPTICAL_MODULE "OpticalModule"
#define RFPROP_OPTICAL_MODULE_MANUFACTURER "Manufacturer"
#define RFPROP_OPTICAL_MODULE_PARTNUMBER "PartNumber"
#define RFPROP_OPTICAL_MODULE_SERIALNUMBER "SerialNumber"
#define RFPROP_OPTICAL_MODULE_PRODUCTIONDATE "ProductionDate"
#define RFPROP_OPTICAL_MODULE_TYPE "TransceiverType"
#define RFPROP_OPTICAL_MODULE_PACKAGING_TYPE "PackagingType"
#define RFPROP_OPTICAL_MODULE_WAVE_LENGTH "WaveLengthNanometer"
#define RFPROP_OPTICAL_MODULE_MEDIUMMODE "MediumMode"
#define RFPROP_OPTICAL_MODULE_SPEED "SupportedSpeedsMbps"
#define RFPROP_OPTICAL_MODULE_VOLTAGE "Voltage"
#define RFPROP_OPTICAL_MODULE_TEMPERATURE "Temperature"
#define RFPROP_OPTICAL_MODULE_BIAS "TXBiasCurrent"
#define RFPROP_OPTICAL_MODULE_RXPOWER "RXPower"
#define RFPROP_OPTICAL_MODULE_TXPOWER "TXPower"
#define RFPROP_OPTICAL_MODULE_READINGCELSIUS "ReadingCelsius"
#define RFPROP_OPTICAL_MODULE_READINGVOLTS "ReadingVolts"
#define RFPROP_OPTICAL_MODULE_READINGMILLIWATTS "ReadingMilliWatts"
#define RFPROP_OPTICAL_MODULE_READINGMILLIAMPERES "ReadingMilliAmperes"
#define RFPROP_OPTICAL_MODULE_LOWER_NOT_CRIT "LowerThresholdNonCritical"
#define RFPROP_OPTICAL_MODULE_LOWER_CRIT "LowerThresholdCritical"
#define RFPROP_OPTICAL_MODULE_UPPER_NOT_CRIT "UpperThresholdNonCritical"
#define RFPROP_OPTICAL_MODULE_UPPER_CRIT "UpperThresholdCritical"
#define RFPROP_OPTICAL_MODULE_INDENTIFIER "Indentifier"
#define RFPROP_OPTICAL_MODULE_DEVICE_TYPE "DeviceType"
#define RFPROP_OPTICAL_MODULE_CONNECTOR_TYPE "ConnectorType"
#define RFPROP_OPTICAL_MODULE_TRANSFER_DISTANCE "TransferDistance"
#define RFPROP_OPTICAL_MODULE_RXLOS_STATE "RxLosState"
#define RFPROP_OPTICAL_MODULE_TXFAULT_STATE "TxFaultState"
typedef struct _optical_modul_prop_object {
    const gchar *current_prop;    // 当前读数的属性名
    const gchar *lower_c_prop;    // 严重告警低门限属性名
    const gchar *upper_c_prop;    // 严重告警高门限属性名
    const gchar *current_rf_prop; // 当前读数的redfish属性名
} optical_modul_prop_object_s;

#define RFVALUE_OPTICAL_MODULE_VOLT 0
#define RFVALUE_OPTICAL_MODULE_TEMP 1
#define RFVALUE_OPTICAL_MODULE_BIAS 2
#define RFVALUE_OPTICAL_MODULE_RXPOWER 0
#define RFVALUE_OPTICAL_MODULE_TXPOWER 1
#define PME_PROPVAL_SM "SM"
#define PME_PROPVAL_MM "MM"
#define RFVALUE_SM "Monomode"
#define RFVALUE_MM "Multimode"

#define RF_TIME_FORMAT_CONNECT "-"

#define INVALID_DOUBLE_VALUE_ALL_1 0xffffffffffffffffULL // 属性默认值配64个1，用double的方式读出来是该值


#define URI_FORMAT_CHASSIS_SENSORLIST_THRESHOLD "/redfish/v1/Chassis/%s/ThresholdSensors"
#define URI_FORMAT_CHASSIS_SENSORLIST_DISCRETE "/redfish/v1/Chassis/%s/DiscreteSensors"
#define URI_FORMAT_CHASSIS_SENSORLIST_MEMBER "Sensors"
#define RFPROP_CHASSIS_THRESHOLD_SENSORS "ThresholdSensors"
#define RFPROP_CHASSIS_DISCRETE_SENSORS "DiscreteSensors"

#define RF_BATTERY_EXTEND_METRICS "BatteryMetricsExtended"
#define RFPROP_BBU_RATED_CAP_WATT_HOUR "RatedCapacityWattHour"
#define RFPROP_BBU_REMAIN_CAP_WATT_HOUR "RemainCapacityWattHour"
#define RFPROP_BBU_PRESNET_NUMBER "PresentBatteryNum"

#define RF_BATTERY_EXTEND_METRICS_OEM_PATH "Oem/Huawei/BatteryMetricsExtended"

#define URI_FORMAT_CHASSIS_BBUS "/redfish/v1/Chassis/%s/BackupBatteryUnits"
#define RFPROP_CHASSIS_BBU "BackupBatteryUnits"
#define RFPROP_BBU_MODULE_BOARDNAME "BoardName"
#define RFPROP_BBU_MODUlE_FW "FirmwareVersion"
#define RFPROP_BBU_MODUlE_WORKING_HOURS "WorkingHours"
#define RFPROP_BBU_MODUlE_WORKING_STATE "WorkingState"
#define RFPROP_BBU_MODUlE_M2_1_PRESENCE "M2Device1Presence"
#define RFPROP_BBU_MODUlE_M2_2_PRESENCE "M2Device2Presence"
#define RFPROP_BBU_MODUlE_BATTERY "Battery"
#define RFPROP_BBU_BATTERY_FULL_CHARGED "FullChargedMilliWatts"
#define RFPROP_BBU_BATTERY_REMAIN_POWER "RemainingPowerMilliWatts"
#define RFPROP_BBU_BATTERY_REMAIN_POWER_PER "RemainingPowerPercent"
#define RFPROP_BBU_BATTERY_CELL_MAX_TEMP "CellMaxTemperatureCelsius"
#define RFPROP_BBU_BATTERY_CELL_MIN_TEMP "CellMinTemperatureCelsius"
#define RFPROP_BBU_BATTERY_REMAIN_DISCHARGE_TIME "RemainingDischargeTimeHours"
#define RFPROP_BBU_BATTERY_REMAIN_CHARGE_TIME "RemainingChargingTimeHours"
#define RFPROP_BBU_BATTERY_MANUFACTURER "Manufacturer"
#define RFPROP_BBU_BATTERY_SN "SerialNumber"
#define RFPROP_BBU_BATTERY_PRODUCTION_DATE "ProductionDate"
#define RFPROP_BBU_BATTERY_MODEL "Model"
#define RFPROP_BBU_BATTERY_PART_NUMBER "PartNumber"
#define RFPROP_BBU_BATTERY_RATED_CAP_WATT_H "RatedCapacityWattHour"
#define RFPROP_BBU_BATTERY_REMAIN_HEALTH_PER "RemainingHealthPercent"
#define RFPROP_BBU_BATTERY_DISCHARGE_TIMES "DischargeTimes"
#define RFPROP_BBU_BATTERY_DEVICE_LOCATOR "DeviceLocator"
#define RFPROP_BBU_BATTERY_POWER_TYPE     "PowerType"

#define RFPROP_BBU_DISCHARGE_STATUS                                 "DischargeStatus"
#define RFPROP_BBU_BAT_STR_VOLTAGE                                  "BatteryStringVoltage"
#define RFPROP_BBU_BAT_STR_CURRENT                                  "BatteryStringCurrent"
#define RFPROP_BBU_BUSBAR_VOLTAGE                                   "BusbarVoltage"
#define RFPROP_BBU_CELL_MAX_TEMP_HIGHER_THAN_60_TIME                "CellMaxTempHigherThan60CelsiusTime"
#define RFPROP_BBU_DIRE_SAMPL_VOLTAGE                               "DirectSamplingVoltage"
#define RFPROP_BBU_BUSBAR_CURR_SAMPL                                "BusbarCurrentSampling"
#define RFPROP_BBU_BATTERY_CURR_SAMPL                                   "BatteryCurrentSampling"
#define RFPROP_BBU_POWER_OUTPUT_WATTS                               "PowerOutputWatts"
#define RFPROP_BBU_CELL_VOLTAGE                                     "CellVoltage"
#define RFPROP_BBU_CELL_VOLTAGE1                                    "CellVoltage1"
#define RFPROP_BBU_CELL_VOLTAGE2                                    "CellVoltage2"
#define RFPROP_BBU_CELL_VOLTAGE3                                    "CellVoltage3"
#define RFPROP_BBU_CELL_VOLTAGE4                                    "CellVoltage4"
#define RFPROP_BBU_CELL_VOLTAGE5                                    "CellVoltage5"
#define RFPROP_BBU_CELL_VOLTAGE6                                    "CellVoltage6"
#define RFPROP_BBU_CELL_VOLTAGE7                                    "CellVoltage7"
#define RFPROP_BBU_CELL_VOLTAGE8                                    "CellVoltage8"
#define RFPROP_BBU_CELL_VOLTAGE9                                    "CellVoltage9"
#define RFPROP_BBU_CELL_VOLTAGE10                                   "CellVoltage10"
#define RFPROP_BBU_CELL_VOLTAGE11                                   "CellVoltage11"
#define RFPROP_BBU_CELL_VOLTAGE12                                   "CellVoltage12"
#define RFPROP_BBU_CELL_VOLTAGE13                                   "CellVoltage13"
#define RFPROP_BBU_CELL_VOLTAGE14                                   "CellVoltage14"
#define RFPROP_BBU_CELL_VOLTAGE15                                   "CellVoltage15"
#define RFPROP_BBU_CELL_VOLTAGE16                                   "CellVoltage16"
#define RFPROP_BBU_CELL_VOLTAGE17                                   "CellVoltage17"
#define RFPROP_BBU_CELL_VOLTAGE18                                   "CellVoltage18"
#define RFPROP_BBU_CELL_TEMPERATURE                                 "CellTemperature"
#define RFPROP_BBU_CELL_TEMPERATURE1                                "CellTemperature1"
#define RFPROP_BBU_CELL_TEMPERATURE2                                "CellTemperature2"
#define RFPROP_BBU_CELL_TEMPERATURE3                                "CellTemperature3"
#define RFPROP_BBU_CELL_TEMPERATURE4                                "CellTemperature4"
#define RFPROP_BBU_CELL_TEMPERATURE5                                "CellTemperature5"
#define RFPROP_BBU_CELL_TEMPERATURE6                                "CellTemperature6"
#define RFPROP_BBU_CELL_TEMPERATURE7                                "CellTemperature7"
#define RFPROP_BBU_CELL_TEMPERATURE8                                "CellTemperature8"
#define RFPROP_BBU_CELL_TEMPERATURE9                                "CellTemperature9"
#define RFPROP_BBU_CELL_TEMPERATURE10                               "CellTemperature10"
#define RFPROP_BBU_CELL_TEMPERATURE11                               "CellTemperature11"
#define RFPROP_BBU_CELL_TEMPERATURE12                               "CellTemperature12"
#define RFPROP_BBU_CELL_TEMPERATURE13                               "CellTemperature13"
#define RFPROP_BBU_CELL_TEMPERATURE14                               "CellTemperature14"
#define RFPROP_BBU_CELL_TEMPERATURE15                               "CellTemperature15"
#define RFPROP_BBU_CELL_TEMPERATURE16                               "CellTemperature16"
#define RFPROP_BBU_CELL_TEMPERATURE17                               "CellTemperature17"
#define RFPROP_BBU_CELL_TEMPERATURE18                               "CellTemperature18"

#define RFPROP_CHASSIS_BBUS "/redfish/v1/Chassis/%s/BackupBatteryUnits"
#define URI_FORMAT_CHASSIS_BBU "/redfish/v1/Chassis/%s/BackupBatteryUnits/%d"

#define URI_FORMAT_CHASSIS_LED_GROUP                               "/redfish/v1/Chassis/%s/LedGroup"
#define RFPROP_CHASSIS_LED_GROUP                                   "LedGroup"
#define RFPROP_CHASSIS_ALARM_LED_F                                 "ChassisAlarmLedFront"
#define RFPROP_CHASSIS_IDENTIFY_LED_F                              "ChassisIdentifyLedFront"
#define RFPROP_CHASSIS_PUSH_LED_F                                  "ChassisPushLedFront"
#define RFPROP_CHASSIS_PULL_LED_F                                  "ChassisPullLedFront"
#define RFPROP_CHASSIS_ALARM_LED_R                                 "ChassisAlarmLedRear"
#define RFPROP_CHASSIS_IDENTIFY_LED_R                              "ChassisIdentifyLedRear"
#define RFPROP_CHASSIS_PUSH_LED_R                                  "ChassisPushLedRear"
#define RFPROP_CHASSIS_PULL_LED_R                                  "ChassisPullLedRear"



#define RFPROP_CHASSIS_BOARDS "Boards"
#define CHASSIS_BOARDS_METADATA "/redfish/v1/$metadata#Chassis/Members/%s/Boards/$entity"
#define CHASSIS_BOARD_METADATA "/redfish/v1/$metadata#Chassis/Members/%s/Boards/Members/$entity"
#define URI_FORMAT_CHASSIS_BOARD "/redfish/v1/Chassis/%s/Boards/%s"
#define URI_FORMAT_CHASSIS_SWITCHES                "/redfish/v1/Chassis/%s/Switches"
#define URI_FORMAT_CHASSIS_SWITCH                    "/redfish/v1/Chassis/%s/Switches/%s"
#define URI_PATTERN_CHASSIS_MESHSWITCH               "/redfish/v1/Chassis/%s/Switches/%s/Ports"
#define URI_PATTERN_CHASSIS_MESHSWITCH_PORTS         "/redfish/v1/Chassis/%s/Switches/%s/Ports/%s"
#define RFPROP_BOARD_CARDID "CardNo"
#define RFPROP_BOARD_DEVICETYPE "DeviceType"
#define RFPROP_BOARD_LOCATION "Location"
#define RFPROP_BOARD_DEVICELOCATOR "DeviceLocator"
#define RFPROP_BOARD_MANUFACTURER "Manufacturer"
#define RFPROP_BOARD_PRODUCTNAME "ProductName"
#define RFPROP_BOARD_SERIALNUMBER "SerialNumber"
#define RFPROP_BOARD_PARTNUMBER "PartNumber"
#define RFPROP_BOARD_ASSETTAG "AssetTag"
#define RFPROP_BOARD_BOARDID "BoardId"
#define RFPROP_BOARD_COMPONENTUID "ComponentUniqueID"

#define RFPROP_BOARD_BOARDIDHEX "BoardIdHex"

#define RFPROP_BOARD_CPLDVERSION "CPLDVersion"
#define RFPROP_BOARD_PCBVERSION "PCBVersion"
#define RFPROP_BOARD_BOARDNAME "BoardName"
#define RFPROP_BOARD_MANUFACTUREDATE "ManufactureDate"
#define RFPROP_BOARD_SUPPORT_RAIDLEVEL "SupportedRAIDLevels"
#define RFPROP_BOARD_PCH_MODEL "PchModel"
#define RFPROP_BOARD_LINK_WIDTH_ABILITY "LinkWidthAbility"
#define RFPROP_BOARD_LINK_SPEED_ABILITY "LinkSpeedAbility"
#define RFPROP_BOARD_LINK_WIDTH "LinkWidth"
#define RFPROP_BOARD_LINK_SPEED "LinkSpeed"
#define RFPROP_BOARD_M2DEVICE1_PRESENCE "M2Device1Presence"
#define RFPROP_BOARD_M2DEVICE2_PRESENCE "M2Device2Presence"
#define RFPROP_BOARD_WORK_MODE "WorkMode"
#define RFPROP_BOARD_RETIMER_VERSION               "RetimerVersion"
#define RFPROP_BOARD_WORK_MODE_AMP "AMP"
#define RFPROP_BOARD_WORK_MODE_SMP "SMP"
#define RFPROP_BOARD_ACTIONS  "Actions"

#define RFPROP_UNIT_HWSR_VERSION         "HWSRVersion"
#define RFPROP_UNIT_DEVICE_NUMBER        "DeviceNumber"
#define RFPROP_UNIT_NAME                 "UnitName"

#define RF_CHASSIS_OEM_ACTION_ARCARD_CONTROL              "/redfish/v1/Chassis/%s/Boards/%s/Actions/Board.ARCardControl"
#define RF_CHASSIS_OEM_ACTION_ARCARD_CONTROL_ACTIONINFO \
    "/redfish/v1/Chassis/%s/Boards/%s/ARCardControlActionInfo"
#define RFPROP_BOARD_ARCARD_SLOT                    "Slot"
#define RFPROP_BOARD_ARCARD_CONTROL_TYPE            "ARCardControlType"
#define RFPROP_BOARD_ARCARD_CTL                     "Board.ARCardControl"
#define RF_ACTION_ARCARD_POWERON                    "On"
#define RF_ACTION_ARCARD_POWEROFF                   "Off"
#define RF_ACTION_ARCARD_RESET                      "Reset"
#define RF_ACTION_ARCARD_FACTORY_RESET              "RestoreFactory"
#define RFPROP_ARCARD_ID                            "ARCardId"

#define RFPROP_TYPE_HDDBACKPLANE "DiskBackplane"
#define RFPROP_TYPE_RAIDCARD "RAIDCard"
#define RFPROP_TYPE_PCIECARD "PCIeCard"
#define RFPROP_TYPE_MEZZCARD "MezzCard"
#define RFPROP_TYPE_NIC "NICCard"
#define RFPROP_TYPE_MEMORYBOARD "MemoryBoard"
#define RFPROP_TYPE_PCIERISER "PCIeRiserCard"
#define RFPROP_TYPE_MAINBOARD "MainBoard"
#define RFPROP_TYPE_FANBACKPLANE "FanBackplane"
#define RFPROP_TYPE_SWITCHMMEZZ "SwitchMezzCard"
#define RFPROP_TYPE_IOBOARD "IOBoard"
#define RFPROP_TYPE_CPUBOARD "CPUBoard"
#define RFPROP_TYPE_EXPANDBOARD "ExpandBoard"
#define RFPROP_TYPE_EXP_MODULE "ExpansionModule"
#define RFPROP_TYPE_FAN_MODULE "FanModule"
#define RFPROP_TYPE_CICCARD "CICCard"
#define RFPROP_TYPE_LEDBOARD "LEDBoard"
#define RFPROP_TYPE_PICCARD "PICCard"
#define RFPROP_TYPE_LEAKDETECTIONCARD "LeakageDetectionCard"
#define RFPROP_TYPE_SOCBOARD "SoCBoard"
#define RFPROP_TYPE_ARCARD "ARCard"

#define PROPERTY_DEVICE_OEM_BDF "BDF"
#define PROPERTY_DEVICE_OEM_VID "VenderID"
#define PROPERTY_DEVICE_OEM_DID "DeviceID"
#define PROPERTY_DEVICE_OEM_SUB_DID "SubsystemDeviceID"
#define PROPERTY_DEVICE_OEM_SUB_VID "SubsystemVenderID"
#define PROPERTY_DEVICE_OEM_CRYPTO_ERASE_SUPPORTED "CryptoEraseSupported"

#define RFPROP_TYPE_M2_TRANSFORM_CARD "M2TransformCard"
#define RFPROP_TYPE_GPU_BOARD "GPUBoard"
#define RFPROP_TYPE_POWER_BACKPLANE "PowerBackplane"



#define RFPROP_TYPE_PERIPHERAL_CARD "PeripheralCard"



#define RFPROP_TYPE_POWER_BOARD "PowerBoard"


#define RFPROP_TYPE_CHASSIS_BACKPLANE "ChassisBackplane"



#define RFPROP_TYPE_HORIZ_CONN_BRD "HorizontalConnectionBoard"


#define RFPROP_TYPE_PCIECARD_CARRIER_BOARD "PCIeCardCarrierBoard"
#define RFPROP_BOARD_LOGICVER "LogicVer"
#define RFPROP_BOARD_PCBVER "PcbVer"
#define RFPROP_BOARD_NAME "Name"
#define RFPROP_BOARD_HEALTH "Health"



#define RFPROP_BOARD_MAX_NUM_SUPPORTED "MaxNumberSupported"



#define URI_FORMAT_CHASSIS_THERMAL "/redfish/v1/Chassis/%s/Thermal"
#define URI_FORMAT_CHASSIS_POWER "/redfish/v1/Chassis/%s/Power"



#define SYSTEMS_STRORAGE_SDCARD_METADATA "/redfish/v1/$metadata#Chassis/Members/%s/Drives/Members/$entity"
#define STORAGE_SDCARD_CONTROLLERS_ODATA_ID "/redfish/v1/Systems/%s/Storages/SDStorage#/StorageControllers/%d"
#define URI_FORMAT_CHASSIS_DRIVE_COLLECTION          "/redfish/v1/Chassis/%s/Drives"
#define URI_FORMAT_CHASSIS_DRIVE "/redfish/v1/Chassis/%s/Drives/%s"
#define SYSTEMS_STORAGE_DRIVES_SDCARDS "/redfish/v1/Chassis/%s/Drives/%s%s"
#define SYSTEMS_STORAGE_SDCARDS "/redfish/v1/Systems/%s/Storages/SDStorage"
#define STRING_CONSET_VALUE_ENABLE "Enabled"
#define RFPROP_SDCONTROLLER_MANUFACTURER "Manufacturer"
#define RFPROP_STORAGE_OEM_TYPE "Type"
#define SDCARD_SYNC_DONE 0
#define SDCARD_WAITTING_SYNC 1
#define SDCARD_SYNC_IN_PROGRESS 2
#define STRING_VALUE_IS_NA "N/A"
#define STRING_VALUE_IS_UNKNOWN "Unknown"
#define STRING_VALUE_IS_UNDEFINED "Undefined"



#define RFPROP_LOGSERVICE_OEM_HUWEI_ACTION_QUERY_SEL_LOG "Oem/Huawei/LogService.QuerySelLogEntries"
#define RFPROP_LOGSERVICE_ACTION_QUERY_SEL_LOG "#LogService.QuerySelLogEntries"
#define RFPROP_LOGSERVICE_QUERY_SEL_ACTION_ODATA_ID \
    "/redfish/v1/Systems/%s/LogServices/Log1/QuerySelLogEntriesActionInfo"
#define RFPROP_LOGSERVICE_OBJECT_TYPE_LIST "ObjectTypeList"
#define RFPROP_LOGSERVICE_SEL_LOG_ENTRIES "SelLogEntries"

#define RF_SYSTEM_LOGSERVICE_LOG1_OEM_ACTION_QUERY_SEL_LOG_TARGET \
    "/redfish/v1/Systems/%s/LogServices/Log1/Actions/Oem/Huawei/LogService.QuerySelLogEntries"
#define RF_SYSTEM_LOGSERVICE_LOG1_OEM_ACTION_QUERY_SEL_LOG_ACTION_INFO \
    "/redfish/v1/Systems/%s/LogServices/Log1/QuerySelLogEntriesActionInfo"

#define RFACTION_PARAM_START_ENTRY_ID "StartEntryId"
#define RFACTION_PARAM_ENTRIES_COUNT "EntriesCount"
#define RFACTION_PARAM_SEL_LEVEL "SelLevel"
#define RFACTION_PARAM_SEL_OBJECT_TYPE "SelObjectType"
#define RFACTION_PARAM_SEL_BEGIN_TIME "SelBeginTime"
#define RFACTION_PARAM_SEL_END_TIME "SelEndTime"
#define RFACTION_PARAM_SEL_SEARCH_STRING "SelSearchString"

#define RFACTION_PARAM_GET_EVENT 3
#define RFACTION_PARAM_GET_EVENT_LOG_DATA 1
#define RFACTION_PARAM_GET_OBJECT_TYPE_LIST 4


#define RFPROP_LOGSERVICE_OEM_HUWEI_ACTION_COLLECT_SEL "Oem/Huawei/LogService.CollectSel"
#define RFPROP_LOGSERVICE_ACTION_COLLECT_SEL "#LogService.CollectSel"
#define RF_SYSTEM_LOGSERVICE_LOG1_OEM_ACTION_COLLECT_SEL_TARGET \
    "/redfish/v1/Systems/%s/LogServices/Log1/Actions/Oem/Huawei/LogService.CollectSel"
#define RF_SYSTEM_LOGSERVICE_LOG1_OEM_ACTION_COLLECT_SEL_ACTION_INFO \
    "/redfish/v1/Systems/%s/LogServices/Log1/CollectSelActionInfo"



#define RFPROP_LOGSERVICE_ACTIONINFO_ID "/redfish/v1/Systems/%s/LogServices/Log1/ClearLogActionInfo"
#define RFPROP_LOGSERVICE_TARGET_ID "/redfish/v1/Systems/%s/LogServices/Log1/Actions/LogService.ClearLog"
#define RFPROP_LOGSERVICE_CLEARLOG "#LogService.ClearLog"
#define RFPROP_LOGSERVICE_CLEARLOG_METHOD "LogService.ClearLog"
#define RFPROP_LOGSERVICE_CONTEXT "/redfish/v1/$metadata#Systems/Members/%s/LogServices/Members/$entity"
#define RFPROP_LOGSERVICE_ODATA_ID "/redfish/v1/Systems/%s/LogServices/Log1"
#define RFPROP_ENTRIES_ODATA_ID "/redfish/v1/Systems/%s/LogServices/Log1/Entries"
#define RFPROP_LOGSERVICE_ACTION_ODATA_ID "/redfish/v1/Systems/%s/LogServices/Log1/ClearLogActionInfo"
#define RFPROP_LOGSERVICE_COLLECTION_CONTEXT "/redfish/v1/$metadata#Systems/Members/%s/LogServices/$entity"
#define RFPROP_LOGSERVICE_COLLECTION_ODATA_ID "/redfish/v1/Systems/%s/LogServices"
#define RFPROP_LOGSERVICE_COLLECTION_MEMBER_ID "/redfish/v1/Systems/%s/LogServices/Log1"

#define RFPROP_LOGSERVICE_COLLECTION_HOSTLOG_MEMBER_ID "/redfish/v1/Systems/%s/LogServices/HostLog"
#define RFPROP_LOGSERVICE_HOTLOG_ODATA_ID "/redfish/v1/Systems/%s/LogServices/HostLog"
#define RFPROP_LOGSERVICE_HOSTLOG_TARGET_ID \
    "/redfish/v1/Systems/%s/LogServices/HostLog/Actions/Oem/Huawei/LogService.Push"
#define RFPROP_LOGSERVICE_HOSTLOG_ACTIONINFO_ID "/redfish/v1/Systems/%s/LogServices/HostLog/PushActionInfo"
#define RFPROP_LOGSERVICE_HOSTLOG_PUSH "#LogService.Push"
#define RFPROP_LOGSERVICE_HOSTLOG_ACTION_ODATA_ID "/redfish/v1/Systems/%s/LogServices/HostLog/PushActionInfo"
#define RFPROP_LOGSERVICE_HOSTLOG_PUSH_METHOD "Oem/Huawei/LogService.Push"

#define RFPROP_MAX_LOG_RECORDS "MaxNumberOfRecords"
#define RFPROP_LOGSERVICE_ENABLE "ServiceEnabled"
#define RFPROP_LOGSERVICE_DATE_TIME "DateTime"
#define RFPROP_LOGSERVICE_TIME_OFFSET "DateTimeLocalOffset"
#define RFPROP_LOGSERVICE_ENTYIES_ODATA_ID "Entries"
#define RFPROP_LOGSERVICE_EVENT_TIME "Created"
#define RFPROP_LOGSERVICE_EVENT_LEVEL "Severity"
#define RFPROP_LOGSERVICE_ENTRY_DESCROPTION "Message"
#define RFPROP_LOGSERVICE_ENTRY_SUGGESTION "HandlingSuggestion"
#define RFPROP_LOGSERVICE_EVENT_CODE "EventId"
#define RFPROP_LOGSERVICE_MESSAGE_ID "MessageId"
#define RFPROP_LOGSERVICE_MESSAGE_ARGS "MessageArgs"
#define RFPROP_LOGSERVICE_EVENT_TYPE "EventType"
#define RFPROP_LOGSERVICE_EVENT_TYPE_VALUE "Alert"
#define RFPROP_LOGSERVICE_ENTRY_TYPE "EntryType"
#define RFPROP_LOGSERVICE_ENTRY_TYPE_VALUE "Event"
#define RFPROP_LOGSERVICE_EVENT_SUBJECT "EventSubject"
#define RFPROP_LOGSERVICE_HEALTH_EVENT "HealthEvent"
#define RFPROP_SEL_ENANLE "Enable"

#define MESSAGE_NAME_STRLEN 32
#define SYETEM_SEL_LOG_ENABLE 1
#define SYSTEM_SEL_LOG_DISABLE 0
#define MAX_SEL_LEN 16
#define ERROR_SNPRINTF 0




#define RFPROP_VOLUMES_OEM "Oem"
#define RFPROP_COMMON_MANUFACTURER "Huawei"
// 以下属性为oem属性
#define RFPROP_VOLUME_DRIVES "Drives" // 此处为逻辑盘关联的物理盘的ID列表
#define RFPROP_VOLUME_CACHECADE_FLAG "CreateCacheCadeFlag"
#define RFPROP_VOLUME_INITMODE "InitializationMode"
#define RFPROP_VOLUME_BLOCK_INDEX "BlockIndex" 
// oem属性定义结束



#define RFPROP_SYSTEM_COMPUTERSYSTEM_OSVERSION ("OSVersion")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_KERNALVERSION ("KernalVersion")

#define RFPROP_SYSTEM_COMPUTERSYSTEM_SYNCENABLE ("HostnameSyncEnabled")



#define RFPROP_SYSTEM_COMPUTERSYSTEM_MAJOROSVERSION ("MajorOSVersion")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_MINOROSVERSION ("MinorOSVersion")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_MAJORPATCHVERSION ("MajorPatchVersion")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_MINORPATCHVERSION ("MinorPatchVersion")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_DOMAINNAME ("DomainName")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_HOSTDESCRIPTION ("HostDescription")

#define RFPROP_SYSTEM_COMPUTERSYSTEM_IBMA_SERVICE ("iBMAService")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_IBMA_STATUS ("iBMARunningStatus")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_IBMA_DRIVER ("iBMADriver")

#define RFPROP_SYSTEM_COMPUTERSYSTEM_SMS_ENABLED ("SmsEnabled")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_DEVICEOWNERID ("DeviceOwnerID")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_DEVICESLOTID ("DeviceSlotID")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_RAID_CFG_READY ("StorageConfigReady")

#define RFPROP_SYSTEM_COMPUTERSYSTEM_NETBANDWIDTHTHRESHOLD "NetBandwidthThresholdPercent"
#define RFPROP_SYSTEM_COMPUTERSYSTEM_CPUTHRESHOLD ("CPUThresholdPercent")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_MEMORYTHRESHOLD ("MemoryThresholdPercent")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_HDDTHRESHOLD ("HardDiskThresholdPercent")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_CPUPOWERADJUSTMENT ("CPUPowerAdjustment")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_CPUPLIMIT ("CPUPLimit")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_CPUPSTATE ("CPUPState")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_CPUTLIMIT ("CPUTLimit")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_CPUTSTATE ("CPUTState")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_CPUTOTALCORES  ("CPUTotalCores")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_CPUUSAGE "CPUUsagePercent"
#define RFPROP_SYSTEM_COMPUTERSYSTEM_BUFFER ("SystemMemoryBuffersGiB")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_CACHED ("SystemMemoryCachedGiB")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_FREE ("SystemMemoryFreeGiB")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_MEMORYUSAGE "MemoryUsagePercent"
#define RFPROP_SYSTEM_COMPUTERSYSTEM_PARTITIONLIST "PartitionLists"
#define RFPROP_SYSTEM_COMPUTERSYSTEM_PARTITIONNAME ("PartitionName")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_PARTITIONUSAGE ("UsagePercent")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_USAGECAPACITY ("UsedCapacityGB")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_TOTALCAPACITY ("TotalCapacityGB")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_POWEROFFTIMEOUT ("SafePowerOffTimoutSeconds")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_POWEROFFTIMEOUT_ENABLED ("SafePowerOffTimeoutEnabled")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_POWEROFFTIMEOUT_DEF ("SafePowerOffDefaultTimeoutSeconds")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_POWEROFFTIMEOUT_MAX ("SafePowerOffMaxTimeoutSeconds")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_POWEROFFTIMEOUT_MIN ("SafePowerOffMinTimeoutSeconds")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_PANELPOWERBUTTONENABLED ("PanelPowerButtonEnabled")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_SYSTEMSTATEAFTERPOWERSUPPLIED ("PowerOnStrategy")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_POWERONSTRATEGYEXCEPTIONS ("PowerOnStrategyExceptions")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_POWERRESTORE_DELAYMODE ("PowerRestoreDelayMode")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_POWERRESTORE_DELAYSECONDS ("PowerRestoreDelaySeconds")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_LEAKDET_SUPPORT ("LeakDetectionSupport")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_LEAK_STRATEGY ("LeakStrategy")

#define RFPROP_SYSTEM_COMPUTERSYSTEM_SLOTWIDTH ("SlotWidth")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_WOL_STATUS ("WakeOnLANStatus")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_BOOTUPSEQUENCE ("BootupSequence")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_CONFIGMODEL    "ConfigurationModel"
#define RFPROP_SYSTEM_COMPUTERSYSTEM_CMCC_VERSION   "CmccVersion"
#define RFPROP_SYSTEM_COMPUTERSYSTEM_CUCC_VERSION   "Version"
#define RFPROP_SYSTEM_COMPUTERSYSTEM_PRODUCTALIAS ("ProductAlias")
#define RFPROP_SYSTEM_COMPUTERSYSTEM_PRODUCTVERSION ("ProductVersion")
#define RFPROP_SYSTEM_OEM_POST_STATE "PostState"
#define RFPROP_SYSTEM_TRUSTEDMODULES "TrustedModules"
#define RFPROP_SYSTEM_TRUSTEDMODULES_FMVER ("FirmwareVersion")
#define RFPROP_SYSTEM_TRUSTEDMODULES_INTERFACETYPE ("InterfaceType")

#define RFPROP_SYSTEM_TRUSTEDMODULES_PROTOCOLTYPE ("ProtocolType")
#define RFPROP_SYSTEM_TRUSTEDMODULES_SPECIFICATIONVERSION ("ProtocolVersion")
#define RFPROP_SYSTEM_TRUSTEDMODULES_MANUFACTURERNAME ("Manufacturer")
#define RFPROP_SYSTEM_TRUSTEDMODULES_SELFTESTRESULT ("SelfTestResult")

#define RFPROP_SYSTEM_PRESENT_FRUS ("PresentFrus")
#define RFPROP_SYSTEM_SWITCH_IPINFO ("SwitchIpInfo")
#define RFPROP_SYSTEM_SWITCH_STACKING_STATUS ("StackingStatus")
#define RFPROP_SYSTEM_SWITCH_VERSION_INFO ("Version")
#define RFPROP_SYSTEM_SWITCH_INFO ("SwitchInfo")

#define RFPROP_SYSTEM_SWITCH_BASE ("Base")
#define RFPROP_SYSTEM_SWITCH_FABRIC ("Fabric")
#define RFPROP_SYSTEM_SWITCH_FC ("FC")
#define RFPROP_SYSTEM_POWER_POLICY_TURN_ON ("TurnOn")
#define RFPROP_SYSTEM_POWER_POLICY_STAY_OFF ("StayOff")
#define RFPROP_SYSTEM_POWER_POLICY_PREVIOUS_STATE ("RestorePreviousState")
#define RFPROP_SYSTEM_POWER_POLICY_TURN_ON_RO ("TurnOnRO")
#define RFPROP_SYSTEM_LEAK_STRATEGY_MANUAL_PWROFF ("ManualPowerOff")
#define RFPROP_SYSTEM_LEAK_STRATEGY_AUTO_PWROFF ("AutoPowerOff")
#define RFPROP_SYSTEM_BOOTUPSEQUENCE_HDD ("Hdd")
#define RFPROP_SYSTEM_BOOTUPSEQUENCE_CD ("Cd")
#define RFPROP_SYSTEM_BOOTUPSEQUENCE_PXE ("Pxe")
#define RFPROP_SYSTEM_BOOTUPSEQUENCE_OTHER ("Others")

#define RFPROP_SYSTEM_BOOT_MODE_LEGACY ("Legacy")

#define RFPROP_SYSTEM_BOOT_MODE_UEFI ("UEFI")
#define RFPROP_SYSTEM_TRUSTEDMODULES_TPM ("TPM")
#define RFPROP_SYSTEM_TRUSTEDMODULES_1_2 ("1.2")
#define RFPROP_SYSTEM_TRUSTEDMODULES_TPM_1_2 ("TPM1_2")
#define RFPROP_SYSTEM_TRUSTEDMODULES_2_0 ("2.0")
#define RFPROP_SYSTEM_TRUSTEDMODULES_TPM_2_0 ("TPM2_0")
#define RFPROP_SYSTEM_TRUSTEDMODULES_TCM ("TCM")
#define RFPROP_SYSTEM_TRUSTEDMODULES_1_0 ("1.0")
#define RFPROP_SYSTEM_TRUSTEDMODULES_TCM_1_0 ("TCM1_0")
#define RFPROP_INVALID_IP_ADDR ("0.0.0.0")
#define RFPROP_SYSTEM_PCH_MODEL ("PchModel")


#define NUMBER_SYSTEM_ATTRIBUTES_OEM 31

#define SWITCH_STACKING_ROLE_NUM 4



#define RFPROP_SYSTEM_BOOT_MODE_CHANGE_ENABLED ("BootModeConfigOverIpmiEnabled")
#define RFPROP_BIOS_BT_MD_IPMI_SW_DISP "BootModeIpmiSWDisplayEnabled"
#define ERROR_PROP_BOOT_MODE_CHANGE_ENABLED ("Oem/Huwei/BootModeConfigOverIpmiEnabled")


#define ERROR_PROP_CFG_ITEM ("Oem/Huawei/BiosDynamicParamConfig/")
#define ERROR_PROP_DEMT_CFG_ITEM ("Oem/Huawei/BiosDynamicParamConfig/DEMTEnabled")
#define RFPROP_SYSTEM_BIOS_DEMT_CFG_PARA ("DEMTEnabled")
#define RFPROP_SYSTEM_BIOS_ENERGY_CONFIGURATION ("BiosDynamicParamConfig")
#define RFPROP_SYSTEM_BIOS_ENERGY_MODE_ENABLED ("EnergySavingEnabled")
#define ERROR_PROP_ENERGY_SAVING_ENABLED ("Oem/Huawei/EnergySavingEnabled")

#define RFPROP_NPU_ABILITY               "NpuAbility"
#define RFPROP_NPU_ABILITY_TYPE          "Type"
#define RFPROP_NPU_ABILITY_VALUE         "Value"

#define MAX_PD_ID_NUM 255
#define MAX_PD_ID_NUM_IN_MSG 5

#define MAX_VOL_STR_LEN 128

#define MAX_RAID_LEVEL_LEN 16

#define STRIPE_SIZE_16K_SCHE (16 * 1024)
#define STRIPE_SIZE_32K_SCHE (32 * 1024)
#define STRIPE_SIZE_64K_SCHE (64 * 1024)
#define STRIPE_SIZE_128K_SCHE (128 * 1024)
#define STRIPE_SIZE_256K_SCHE (256 * 1024)
#define STRIPE_SIZE_512K_SCHE (512 * 1024)
#define STRIPE_SIZE_1M_SCHE (1024 * 1024)

#define STRIPE_SIZE_16K_BMC 5
#define STRIPE_SIZE_32K_BMC 6
#define STRIPE_SIZE_64K_BMC 7
#define STRIPE_SIZE_128K_BMC 8
#define STRIPE_SIZE_256K_BMC 9
#define STRIPE_SIZE_512K_BMC 10
#define STRIPE_SIZE_1M_BMC 11
#define STRIPE_SIZE_DEFAULT_BMC 255

#define RAID_LEVEL_STR_0 "RAID0"
#define RAID_LEVEL_STR_1 "RAID1"
#define RAID_LEVEL_STR_5 "RAID5"
#define RAID_LEVEL_STR_6 "RAID6"
#define RAID_LEVEL_STR_10 "RAID10"
#define RAID_LEVEL_STR_50 "RAID50"
#define RAID_LEVEL_STR_60 "RAID60"
#define RAID_LEVEL_STR_1ADM     "RAID1(ADM)"
#define RAID_LEVEL_STR_10ADM    "RAID10(ADM)"
#define RAID_LEVEL_STR_1TRIPLE  "RAID1Triple"
#define RAID_LEVEL_STR_10TRIPLE "RAID10Triple"

#define SPAN_NUM_1 1
#define SPAN_NUM_2 2
#define READ_POLICY_0 0
#define READ_POLICY_1 1
#define READ_POLICY_DEF 255
#define WRITE_POLICY_0 0
#define WRITE_POLICY_1 1
#define WRITE_POLICY_2 2
#define WRITE_POLICY_DEF 255
#define IO_POLICY_0 0
#define IO_POLICY_1 1
#define IO_POLICY_DEF 255
#define CACHE_POLICY_0 0
#define CACHE_POLICY_1 1
#define CACHE_POLICY_2 2
#define CACHE_POLICY_DEF 255
#define ACCESS_POLICY_0 0
#define ACCESS_POLICY_1 1
#define ACCESS_POLICY_2 2
#define ACCESS_POLICY_DEF 255
#define INIT_MODE_DEF 255
#define INIT_MODE_0 0
#define INIT_MODE_1 1
#define INIT_MODE_2 2

#define LD_WRITE_THROUGH "WriteThrough"
#define LD_WRITE_BACKBBU "WriteBackWithBBU"
#define LD_WRITE_BACK "WriteBack"
#define LD_READ_NO "NoReadAhead"
#define LD_READ_AHEAD "ReadAhead"
#define LD_IO_CACHE "CachedIO"
#define LC_IO_DIRECT "DirectIO"
#define LD_ACC_RW "ReadWrite"
#define LD_ACC_READ "ReadOnly"
#define LD_ACC_BLOCK "Blocked"
#define LD_DCACHE_DEF "Unchanged"
#define LD_DCACHE_ENABLE "Enabled"
#define LD_DCACHE_DISABLE "Disabled"
#define LD_INITMODE_UN "UnInit"
#define LD_INITMODE_QUICK "QuickInit"
#define LD_INITMODE_FULL "FullInit"
#define LD_INITMODE_RPI "RPI"
#define LD_INITMODE_OPO "OPO"
#define LD_INITMODE_FRONT "Front"
#define LD_INITMODE_BACKGROUND "Background"
#define LD_DCACHE_DISK_DEF "Disk'sDefault"
#define LD_ACC_METHOD_NONE "None"
#define LD_ACC_METHOD_CACHE "ControllerCache"
#define LD_ACC_METHOD_IOBYPASS "IOBypass"
#define LD_ACC_METHOD_MAXCACHE "maxCache"

#define CAPACITY_DEFAULT_VAL 0xFFFFFFFF
#define CAPACITY_UNIT_MB 0
#define INIT_WAY_DEFAULT 255

#define VOLUME_MEMBER_ID_CONNECTOR "LogicalDrive"

#define ERR_CODE_VOL_CREATE_SUCCESS 0x40
#define ERR_CODE_VOL_DEL_SUCCESS 0x41
#define ERR_CODE_THREAD_NOT_READY 0x42




#define FI_NAME_LEN 16
#define FI_URI_LEN 80
#define FI_ABMC_MEMBERID "ActiveBMC"
#define FI_BBMC_MEMBERID "BackupBMC"
#define FI_AUBOOT_MEMBERID "ActiveUboot"
#define FI_BUBOOT_MEMBERID "BackupUboot"
#define FI_BIOS_MEMBERID "Bios"
#define FI_IMU_MEMBERID "IMU"
#define FI_LCD_MEMBERID "Lcd"
#define FI_CPLD_MEMBERID "CPLD"
#define FI_MAINCPLD_MEMBERID "MainBoardCPLD"
#define FI_BASICCPLD_MEMBERID "BasicBoardCPLD"
#define FI_IOCTRL_MEMBERID "IOCtrl"
#define INVALID_IMU_MAJOR_VERSION "0"
#define IMU_MAJOR_VERSION_LEN 1


#ifdef ARM64_HI1711_ENABLED
#define FI_AVAILBMC_MEMBERID "AvailableBMC"
#endif



#define FI_SDCARD_CONTROLLER_MEMBERID "SDController"


#define FWINV_STR_MAX_LEN 128
#define RFPROP_FWINV_OEM "Oem"
#define RFPROP_FWINV_HUAWEI "Huawei"
#define RFPROP_FWINV_POSITION_ID "PositionId"
#define RFPROP_FWINV_COMPONENT_INFO "ComponentInfo"
#define RFPROP_FWINV_ATTRIBUTE "Attribute"
#define RFPROP_FWINV_ATTRIBUTE_HOTFIX "Hotfix"
#define RFPROP_FWINV_ODATA_ID "@odata.id"
#define RFPROP_FWINV_ID "Id"
#define RFPROP_FWINV_NAME "Name"
#define RFPROP_FWINV_STATUS "Status"
#define RFPROP_FWINV_VERSION "Version"
#define RFPROP_FWINV_UPDATEABLE "Updateable"
#define RFPROP_FWINV_SOFTWAREID "SoftwareId"


#define RFPROP_FWINV_RELATEDITEM "RelatedItem"
#define RFPROP_SD_CONTROLLER "/redfish/v1/Systems/%s/Storages/2#/StorageControllers/0"
#define MAINBOARD_DEVICETYPE 16
#define PCIE_ADAPTER_DEVICETYPE 38
#define PCIE_RISER_DEVICETYPE 15
#define CPUBOARD_DEVICETYPE "36"
#define MEMBOARD_DEVICETYPE "14"
#define HDDBACKPLANE_DEVICETYPE "5"
#define IOBOARD_DEVICETYPE "35"
#define FANBOARD_DEVICETYPE "24"
#define EXPBOARD_DEVICETYPE "93"
#define RAID_DEVICETYPE "6"
#define PCIERAID_DEVICETYPE "8"
#define PS_DEVICETYPE "3"
#define PCIERISER_DEVICETYPE "15"
#define MAINBOARDCPLD_DEVICETYPE "16"
#define SWITCHPLANE_DEVICETYPE "30"
#define BBU_DEVICETYPE "82"
#define EXPENSION_MOD_DEVICETYPE   "88"

#define COMPONENT_BITMAP "BitMap"



#define GPUBOARD_DEVICETYPE "64"


#define FI_CONST_URI "/redfish/v1/UpdateService/FirmwareInventory/"

#define PROPERTY_FIRMWARE_PRESENCE "Presence"
#define PROPERTY_FIRMWARE_ID "Id"
#define PROPERTY_FIRMWARE_LOGICVER "LogicVer"
#define PROPERTY_FIRMWARE_LOGICUNIT "LogicUnit"
#define PROPERTY_FIRMWARE_SLOT "Slot"
#define PROPERTY_FIRMWARE_FWVER "FirmwareVersion"
#define PROPERTY_FIRMWARE_DEVNAME "DeviceName"

#define URI_CLASS_DISK "Disk"
#define URI_CLASS_PS "PS"
#define URI_CLASS_CPLD "CPLD"
#define URI_CLASS_CPUBOARD "CPUBoard"
#define URI_CLASS_FANBOARD "FanBoard"
#define URI_CLASS_EXPBOARD "EXPBoard"
#define URI_CLASS_RAID "RAID"
#define URI_CLASS_PCIERAID "PCIERAID"

#define URI_OBJ_ACTIVEBMC "ActiveBMC"
#define URI_OBJ_BACKUPBMC "BackupBMC"
#define URI_OBJ_AVAILABLEBMC "AvailableBMC"
#define URI_OBJ_ACTIVEUBOOT "ActiveUboot"
#define URI_OBJ_BACKUPUBOOT "BackupUboot"
#define URI_OBJ_MAINBOARDCPLD "MainBoardCPLD"
#define URI_OBJ_BIOS "Bios"
#define URI_OBJ_IMU "IMU"
#define URI_OBJ_LCD "Lcd"
#define URI_OBJ_MAINBOARDCPLD2 "MainBoardBackCPLD"
#define URI_OBJ_BASICBOARDCPLD "BasicBoardCPLD"

#define URI_OBJ_CDR "CDR"
#define URI_OBJ_RETIMER "Retimer"

#define URI_OBJ_HMM1_AC_UBOOT "HMM1ActiveUboot"
#define URI_OBJ_HMM1_BK_UBOOT "HMM1BackupUboot"
#define URI_OBJ_HMM1_AC_SOFTWARE "HMM1ActiveSoftware"
#define URI_OBJ_HMM1_BK_SOFTWARE "HMM1BackupSoftware"
#define URI_OBJ_HMM1_CPLD "HMM1CPLD"
#define URI_OBJ_HMM2_AC_UBOOT "HMM2ActiveUboot"
#define URI_OBJ_HMM2_BK_UBOOT "HMM2BackupUboot"
#define URI_OBJ_HMM2_AC_SOFTWARE "HMM2ActiveSoftware"
#define URI_OBJ_HMM2_BK_SOFTWARE "HMM2BackupSoftware"
#define URI_OBJ_HMM2_CPLD "HMM2CPLD"

#ifdef ARM64_HI1711_ENABLED
#define URI_OBJ_HMM1_AV_SOFTWARE "HMM1AvailableBMC"
#define URI_OBJ_HMM2_AV_SOFTWARE "HMM2AvailableBMC"
#endif
#define SMM_NUM_INDEX 3


#define URI_OBJ_SDCARD_CONTROLLER "SDController"


#define SW_ID_BMC "BMC-iBMC"
#define SW_ID_BIOS "BIOS-BIOS"
#define SW_ID_IMU "BIOS-IMU"
#define SW_ID_UBOOT "UBOOT-UBOOT"
#define SW_ID_LCD "LCD-%s"
#define SW_ID_PSU "PSU-%s"
#define SW_ID_VRD "VRD-%s"
#define SW_ID_SDCARD_CONTROLLER "SDController-%s"
#define SW_ID_DISK "Disk-%s"
#define SW_ID_CPLD "CPLD-%s"
#define SW_ID_IOCTRL "IOCtrl-%s"
#define SW_ID_PCIEFE "PCIeFW-%d.%d.%d.%d"
#define SW_ID_NA "NA"
#define SW_ID_NULL ""
#define SW_ID_CPLD2 "CPLD-Mainboard"
#define SW_ID_FABRIC_PLANE "Switch-Fabric"
#define SW_ID_BBU "BBU-%s"
#define SW_ID_CDR "CDR-5902L"
#define SW_ID_RETIMER "Retimer-5902H"


#define URI_SUFFIX_CPLD "CPLD"
#define URI_SUFFIX_NULL ""
#define URI_SUFFIX_CDR "CDR"
#define URI_SUFFIX_RETIMER "Retimer"
#define URI_SUFFIX_IOCTRL "IOCtrl"




typedef enum {
    CLS_INFO_NAME = 0,
    CLS_INFO_REF_NAME,
    CLS_INFO_SW_ID_PATTERN,
    CLS_INFO_SW_ID_PROP_NAME,
    CLS_INFO_URI_SUFFIX,
    URI_SUFFIX,
    CLS_INFO_MAX_SIZE 
} CLS_INFO_INDEX;


#define MAX_SOFTWARE_ID_LEN 64


#define URI_FORMAT_SYSTEM "/redfish/v1/Systems/%s"
#define URI_FORMAT_CHASSIS "/redfish/v1/Chassis/%s"
#define URI_FORMAT_MANAGER "/redfish/v1/Managers/%s"
#define PSLOT_MAX_LEN 50
#define PSLOT_URI_LEN 256
#define RF_RESOURCE_ETHERNETINTERFACES "EthernetInterfaces"
#define CHASSIS_URI_LEN 256


#define RFACTION_PARAM_PATH "Path"


#define RFACTION_UPDATE_SVC_SIMPLE_UPDATE "UpdateService.SimpleUpdate"
#define RFACTION_UPDATE_SVC_SYNC_UPDATE   "Oem/Huawei/UpdateService.StartSyncUpdate"
#define RFACTION_UPDATE_ACTIVATE_BIOS   "Oem/Huawei/UpdateService.ActivateBios"
#define RFACTION_PARAM_IMAGE_URI "ImageURI"
#define RFACTION_PARAM_TRANSFER_PROTOCOL "TransferProtocol"
#define RFACTION_PARAM_RESET_FLAG "ResetFlag"
#define RF_UPGRADING_FLAG "UpgradingFlag"
#define RFACTION_BMC_ACTIVE_MODE "ActiveMode"
#define RFACTION_ACTIVATE_BIOS "ActivateBios"
#define RFACTION_ACTIVATED_SCOPE "BiosActivatedScope"
#define RF_IS_SUPPORT_OPTIONAL_UPGRADE_EFECTIVE_MODE "ActiveModeSupported"
#define RF_BMC_CURRENT_ACTIVE_MODE "ActiveMode"
#define RFPROP_ACIONS "Actions"

#define RF_UPGRADING_NEED_FILE_TRASFER "NeedFileTrasfer"

#define RF_FILE_TRANSFER_PERCENTAGE 10
#define RF_MAX_PROGRESS_LEN 32
#define RF_FILE_TRANSFER_OBJ_FILE_ID 0x04
#define RF_UPGRADE_TASK_DESC "Upgrade Task"
#define RF_UPDATE_SVC_DOWNGRADE_DISABLED             "DowngradeDisabled"
#define RF_UPDATE_SVC_SYNCUPDATESTATE             "SyncUpdateState"
#define RF_UPDATE_SVC_BIOSUPDATESTATE             "BiosUpdateState"
#define RF_BMC_SYNC_UPDATE_URI   "/redfish/v1/UpdateService/Actions/Oem/Huawei/UpdateService.StartSyncUpdate"
#define RF_BMC_SYNC_UPDATE_ACTIONINFO_URI   "/redfish/v1/UpdateService/Actions/Oem/Huawei/UpdateService.StartSyncUpdate"
#define RF_ACTIVATE_BIOS_URI   "/redfish/v1/UpdateService/Actions/Oem/Huawei/UpdateService.ActivateBios"
#define RF_ACTIVATE_BIOS_ACTIONINFO_URI   "/redfish/v1/UpdateService/ActivateBiosActionInfo"



#define RFACTION_HTTPS_CERT_GEN_CSR "HttpsCert.GenerateCSR"
#define RFACTION_HTTPS_CERT_IMPORT_SERVER_CERT "HttpsCert.ImportServerCertificate"
#define RFACTION_HTTPS_CERT_IMPORT_CUST_CERT "HttpsCert.ImportCustomCertificate"
#define RFPROP_HTTPS_CERT_X509_CERT_INFO "X509CertificateInformation"
#define RFPROP_HTTPS_CERT_CSR_INFO "CertificateSigningRequest"
#define RFPROP_HTTPS_CERT_SEVER_CERT "ServerCert"
#define RFPROP_HTTPS_CERT_INTER_CERT "IntermediateCert"
#define RFPROP_HTTPS_CERT_ROOT_CERT "RootCert"
#define RFPROP_HTTPS_CERT_IS_DEFAULT_CERT "IsDefaultSSLCert"

#define RFPROP_HTTPS_CERT_SUBJECT_INFO "Subject"
#define RFPROP_HTTPS_CERT_ISSUER_INFO "Issuer"
#define RFPROP_HTTPS_CERT_START_TIME "ValidNotBefore"
#define RFPROP_HTTPS_CERT_EXPIRATION "ValidNotAfter"
#define RFPROP_HTTPS_CERT_SN "SerialNumber"
#define RFPROP_HTTPS_CERT_KEY_USAGE "KeyUsage"
#define RFPROP_HTTPS_CERT_KEY_LEN "PublicKeyLengthBits"
#define RFPROP_HTTPS_CERT_SIG_ALGO "SignatureAlgorithm"
#define RFACTION_PARAM_COUNTRY "Country"
#define RFACTION_PARAM_STATE "State"
#define RFACTION_PARAM_CITY "City"
#define RFACTION_PARAM_ORGNAME "OrgName"
#define RFACTION_PARAM_ORGUNIT "OrgUnit"
#define RFACTION_PARAM_COMMON_NAME "CommonName"
#define RFACTION_PARAM_CERTICATE "Certificate"
#define RFACTION_PARAM_PWD "Password"
#define RFPROP_SECURITY_SVC_HTTPS_CERT "HttpsCert"



#define RFACTION_HTTPS_CERT_EXPORT_CSR "HttpsCert.ExportCSR"




#define IMPORT_CERT_ERR_MA_IDEL 0
#define IMPORT_CERT_ERR_MA_REACH_MAX 1             
#define IMPORT_CERT_ERR_MA_CERT_EXSIT 2            
#define IMPORT_CERT_ERR_MA_FORMAT_ERR 3            
#define IMPORT_CERT_ERR_MA_NO_ISSUER 4             
#define IMPORT_CERT_ERR_MA_BASIC_CONSTRAINTS_ERR 5 



#define RFACTION_FORMAT_TEXT "text"
#define RFACTION_FORMAT_FILE "URI"


#define RF_MINITOR_TIME_OUT_COUNT "TimeOutCount"

#define MAX_FILE_PATH_LENGTH 256
#define MAX_ERR_STRING_LENGTH 256
#define MAX_URI_PATH_LENGTH 2048
#define MAX_DES_LENGTH 256
#define RF_IMPORTING_FLAG "ImportingFlag"
#define RF_EXPORTING_FLAG "ExportingFlag"
#define RF_IMPORT_FILE_TYPE "filetype"
#define RF_IMPORT_FILE_TYPE_CERT 1
#define RF_IMPORT_FILE_TYPE_SSH 2
#define RF_IMPORT_USER_ID "userid"
#define RF_IMPORT_USER_NAME "username"
#define RF_IMPORT_USER_CLIENT "client"
#define RF_IMPORT_USER_ROLE "userrole"

#define RF_USERDATA_FROM_WEBUI_FLAG "from_webui_flag"

#define RF_IMPORT_RESULT "importresult"
#define RF_IMPORTING_FLAG "ImportingFlag"
#define RF_IMPORT_CLIENT_CERT_TASK_DESC "Import File Task"
#define RF_IMPORT_ACTION_CERT_NAME "importmutualauthclientcert"
#define RF_IMPORT_ACTION_SSH_NAME "importsshpublickey"
#define RF_EXPORT_EXPORT_DUMP_TASK_NAME "Export Dump File Task"
#define RF_EXPORT_EXPORT_CONFIG_TASK_NAME "Export Config File Task"
#define RF_IMPORT_CONFIG_TASK_DESC "Import Config File Task"
#define RF_EXPORT_ACTION_DUMP_NAME "dump"
#define RF_IMPORT_ACTION_IMPORT_CONFIG_NAME "importconfiguration"
#define RF_EXPORT_ACTION_EXPORT_CONFIG_NAME "exportconfiguration"
#define RF_EXPORT_FILE_DUMP 3
#define RF_EXPORT_FILE_CONFIG 4
#define RFACTION_MUTAU_CERT_IMPORT_CLI_CERT "Account.ImportMutualAuthClientCert"
#define RFACTION_MUTAU_CERT_DELETE_CLI_CERT "Account.DeleteMutualAuthClientCert"

#define RFPROP_MUTAU_CERT "MutualAuthClientCert"
#define RFPROP_MUTAU_CERT_ISSUEBY "IssueBy"
#define RFPROP_MUTAU_CERT_ISSUTO "IssueTo"
#define RFPROP_MUTAU_CERT_VALIDFROM "ValidFrom"
#define RFPROP_MUTAU_CERT_VALIDTO "ValidTo"
#define RFPROP_MUTAU_CERT_SN "SerialNumber"
#define RFPROP_MUTAU_CERT_SIGALGO "SignatureAlgorithm"
#define RFPROP_MUTAU_CERT_KEYUSAGE "KeyUsage"
#define RFPROP_MUTAU_CERT_KEYLENGTH "PublicKeyLengthBits"
#define RFPROP_MUTAU_CERT_FINGER_PRINT "FingerPrint"
#define RFPROP_MUTAU_CERT_ROOT_STATE "RootCertUploadedState"



#define RFPROP_PRODUCT_UNIQUE_ID "ProductUniqueID"
#define RFPOP_PRODUCT_TYPE "PlatformType"

#define RFPOP_PRODUCT_SPSTATUS "SPStatus"
#ifndef ARM64_HI1711_ENABLED
#define SP_PARTITION_TABLE_PATH "/dev/mmcblk0p4"
#else
#define SP_PARTITION_TABLE_PATH "/dev/mmcblk0p2"
#endif

#define RFPROP_PRODUCT_SHARE_INFO "RemoteOEMInfo"
#define RFPROP_PRODUCT_LOCATION_INFO "DeviceLocation"
#define CLASS_NAME_KVM "Kvm "
#define RFPROP_KVM_LOCAL_STATE_VALUE "LocalKVMState "
#define RFPROP_KVM_LOCAL_STATE "LocalKvmEnabled"
#define RFPROP_STATELESS_POWER_ENABLE "ControlPowerOnEnabled"
#define RFPROP_STATELESS_ENABLE "AutoSearchEnabled"
#define RFPROP_STATELESS_STATE "Stateless"
#define RFPROP_MANAGER_LANGUAGE_SET "LanguageSet"

#define RFPROP_MANAGER_FUSION_PARTITION "FusionPartition"
#define RFPROP_VAL_FP_SINGLE_SYSTEM "SingleSystem"
#define RFPROP_VAL_FP_DUAL_SYSTEM "DualSystem"
#define RFPROP_MANAGER_REMOTE_IPV4 "RemoteBMCIPv4Address"
#define RFPROP_MANAGER_VGA_ENABLED "VGAUSBDVDEnabled"
#define RFACTION_PARAM_FP "FusionPartition"
#define RFACTION_PARAM_FP_REMOTE_NAME "RemoteNodeUserName"
#define RFACTION_PARAM_FP_REMOTE_PASSWD "RemoteNodePassword"



#define RFPROP_MANAGER_LOGSERVICE_COLLECTION_CONTEXT "/redfish/v1/$metadata#Managers/Members/%s/LogServices/$entity"
#define RFPROP_MANAGER_LOGSERVICE_COLLECTION_ODATA_ID "/redfish/v1/Managers/%s/LogServices"
#define RFPROP_MANAGER_LOGSERVICE_COLLECTION_OPERATELOG "/redfish/v1/Managers/%s/LogServices/OperateLog"
#define RFPROP_MANAGER_LOGSERVICE_COLLECTION_RUNLOG "/redfish/v1/Managers/%s/LogServices/RunLog"
#define RFPROP_MANAGER_LOGSERVICE_COLLECTION_SECURITYLOG "/redfish/v1/Managers/%s/LogServices/SecurityLog"
#define RFPROP_LOGSERVICE_EXPORT "#LogService.ExportLog"
#define RFPROP_LOGSERVICE_EXPORT_URI "Oem/Huawei/LogService.ExportLog"
#define RFPROP_OPERATELOG_EXPORT_TARGET \
    "/redfish/v1/Managers/%s/LogServices/OperateLog/Actions/Oem/Huawei/LogService.ExportLog"
#define RFPROP_OPERATELOG_EXPORT_ACTION "/redfish/v1/Managers/%s/LogServices/OperateLog/ExportLogActionInfo"
#define RFPROP_RUNLOG_EXPORT_TARGET "/redfish/v1/Managers/%s/LogServices/RunLog/Actions/Oem/Huawei/LogService.ExportLog"
#define RFPROP_RUNLOG_EXPORT_ACTION "/redfish/v1/Managers/%s/LogServices/RunLog/ExportLogActionInfo"
#define RFPROP_SECURITYLOG_EXPORT_TARGET \
    "/redfish/v1/Managers/%s/LogServices/SecurityLog/Actions/Oem/Huawei/LogService.ExportLog"
#define RFPROP_SECURITYLOG_EXPORT_ACTION "/redfish/v1/Managers/%s/LogServices/SecurityLog/ExportLogActionInfo"

#define RFPROP_MANAGER_LOGSERVICE_MEMBERS_CONTEXT \
    "/redfish/v1/$metadata#Managers/Members/%s/LogServices/Members/$entity"
#define RFPROP_MANAGER_LOGSERVICE_OPERATELOG_ENTRIES "/redfish/v1/Managers/%s/LogServices/OperateLog/Entries"
#define RFPROP_MANAGER_LOGSERVICE_RUNLOG_ENTRIES "/redfish/v1/Managers/%s/LogServices/RunLog/Entries"
#define RFPROP_MANAGER_LOGSERVICE_SECURITYLOG_ENTRIES "/redfish/v1/Managers/%s/LogServices/SecurityLog/Entries"

#define RFPROP_MANAGER_LOGSERVICE_OPERATELOG_CONTEXT \
    "/redfish/v1/$metadata#Managers/Members/%s/LogServices/OperateLog/$entity"
#define RFPROP_MANAGER_LOGSERVICE_OPERATELOG_URI "/redfish/v1/Managers/%s/LogServices/OperateLog/Entries/%d"
#define RFPROP_MANAGER_OPERATELOG_ITEM_CONTEXT \
    "/redfish/v1/$metadata#Managers/Members/%s/LogServices/OperateLog/Entries/Members/$entity"

#define RFPROP_MANAGER_LOGSERVICE_RUNLOG_CONTEXT "/redfish/v1/$metadata#Managers/Members/%s/LogServices/RunLog/$entity"
#define RFPROP_MANAGER_LOGSERVICE_RUNLOG_URI "/redfish/v1/Managers/%s/LogServices/RunLog/Entries/%d"
#define RFPROP_MANAGER_RUNLOG_ITEM_CONTEXT \
    "/redfish/v1/$metadata#Managers/Members/%s/LogServices/RunLog/Entries/Members/$entity"

#define RFPROP_MANAGER_LOGSERVICE_SECURITYLOG_CONTEXT \
    "/redfish/v1/$metadata#Managers/Members/%s/LogServices/SecurityLog/$entity"
#define RFPROP_MANAGER_LOGSERVICE_SECURITYLOG_URI "/redfish/v1/Managers/%s/LogServices/SecurityLog/Entries/%d"
#define RFPROP_MANAGER_SECURITYLOG_ITEM_CONTEXT \
    "/redfish/v1/$metadata#Managers/Members/%s/LogServices/SecurityLog/Entries/Members/$entity"

#define RFPROP_MANAGER_FPCSERVICE_CONTEXT		"/redfish/v1/$metadata#Managers/Members/%s/FPCService/$entity"
#define RFPROP_MANAGER_FPCSERVICE_ODATA_ID		"/redfish/v1/Managers/%s/FPCService"
#define RFPROP_MANAGER_FPCSERVICE_MEMORY                "/redfish/v1/Managers/%s/FPCService/Memory"
#define RFPROP_MANAGER_FPCSERVICE_MEMORY_CONTEXT		"/redfish/v1/$metadata#Managers/Members/%s/FPCService/Members/$entity"



#define URI_FORMAT_POWER_AGENT "/redfish/v1/RackManagementService/PowerAgent"
#define RFPROP_SET_RACk_POWER_CAPPING_ACTION "PowerAgent.SetRackPowerCapping"
#define RF_SET_RACk_POWER_CAPPING_ACTION \
    "/redfish/v1/RackManagementService/PowerAgent/Actions/PowerAgent.SetRackPowerCapping"
#define URI_FORMAT_SET_RACk_POWER_CAPPING_ACTIONINFO \
    "/redfish/v1/RackManagementService/PowerAgent/SetRackPowerCappingActionInfo"

#define RFPROP_PERIOD_PRE_RACk_POWER_CAPPING_ACTION "PowerAgent.PeriodPreRackPowerCapping"
#define RF_PERIOD_PRE_RACk_POWER_CAPPING_ACTION \
    "/redfish/v1/RackManagementService/PowerAgent/Actions/PowerAgent.PeriodPreRackPowerCapping"
#define URI_FORMAT_PERIOD_PRE_RACk_POWER_CAPPING_ACTIONINFO \
    "/redfish/v1/RackManagementService/PowerAgent/PeriodPreRackPowerCappingActionInfo"

#define RF_SET_RACK_POWER_CAPPING_TASK_DESC "Set Rack Power Capping Task."

#define RF_RACK_POWER_CAPPING_USER_NAME "UserName"
#define RF_RACK_POWER_CAPPING_CLIENT "Client"
#define RF_RACK_POWER_CAPPING_USER_ROLE "UserRole"
#define RF_RACK_POWER_CAPPING_OBJ_HANDLE "ObjHandle"

#define RFPROP_PERIOD_PRE_PARA "PeriodPredictPara"
#define RFPROP_AVERAGE_POWER_WATTS "AveragePowerWatts"
#define RFPROP_PEAK_POWER_WATTS "PeakPowerWatts"




#define RFPROP_MANAGER_DELETE_SWI_PROFILE "Oem/Huawei/Manager.DeleteSwitchProfile"


#define RFPROP_MANAGER_RESTORE_SWI_PROFILE "Oem/Huawei/Manager.RestoreSwitchProfile"
#define RFPROP_MANAGER_PARSE_SWI_PROFILE "Oem/Huawei/Manager.ParseSwitchProfile"


#define RFPROP_MANAGER_EXPORT_DUMP "Oem/Huawei/Manager.Dump"
#define RFPROP_MANAGER_EXPORT_CONFIG "Oem/Huawei/Manager.ExportConfiguration"
#define RFPROP_MANAGER_IMPORT_CONFIG "Oem/Huawei/Manager.ImportConfiguration"
#define RFPROP_MANAGER_POWER_ON "Oem/Huawei/Manager.PowerOnPermit"
#define RFPROP_MANAGER_RESTORE_FACTORY "Oem/Huawei/Manager.RestoreFactory"
#define RFPROP_MANAGER_DELETE_LANGUAGE "Oem/Huawei/Manager.DeleteLanguage"
#define RFPROP_MANAGER_GENERAL_DOWNLOAD "Oem/Huawei/Manager.GeneralDownload"
#define RFPROP_MANAGER_SET_FUSION_PARTITION "Oem/Huawei/Manager.SetFusionPartition"
#define RFPROP_MANAGER_SHELF_POWER_CONTROL "Oem/Huawei/Manager.ShelfPowerControl"

#define RFPROP_SMSSERVICE_REFRESH_BMA "SmsService.RefreshInstallableBma"
#define URI_FORMAT_SMSSERVICE "/redfish/v1/Managers/%s/SmsService"
#define ACTION_INFO_REFRESH_INSTALLABLE_BMA "/redfish/v1/Managers/%s/SmsService/RefreshInstallableBmaActionInfo"
#define ACTION_REFRESH_BMA_INFO_URI_SUFFIX "RefreshInstallableBmaActionInfo"

#define RFPROP_MANAGER_SMS_ACTION_PARAM_MODE "Mode"
#define INSTALLABLE_BMA_PATH "/data/ibma/Linux"
#define RFPROP_MANAGER_SMS_ACTION_PARAM_SI_URI "SignatureURI"
#define RFPROP_MANAGER_SMS_ACTION_PARAM_CRL_URI "CrlURI"
#define INSTALLABLE_BMA_UPGRADE_TASK_DESC "Upgrade Installable IBMA Task"
#define INSTALLABLE_IBMA_UPGRADE_NO_PROGRESS 255
#define BMA_ACTION_REMOTE_IM_URI_REGEX "^(https|sftp|nfs|cifs|scp)://.{1,1000}\\.(tar.gz)$"
#define BMA_ACTION_REMOTE_SI_URI_REGEX "^(https|sftp|nfs|cifs|scp)://.{1,1000}\\.(cms)$"
#define BMA_ACTION_REMOTE_CRL_URI_REGEX "^(https|sftp|nfs|cifs|scp)://.{1,1000}\\.(crl)$"
#define BMA_ACTION_LOCAL_IM_URI_REGEX "/tmp/.{1,1000}\\.(tar.gz)$"
#define BMA_ACTION_LOCAL_SI_URI_REGEX "/tmp/.{1,1000}\\.(cms)$"
#define BMA_ACTION_LOCAL_CRL_URI_REGEX "/tmp/.{1,1000}\\.(crl)$"

enum BMA_FILE_STATE {
    BMA_IS_WORKING = 2,
    BMA_FILE_NOT_EXIST,
    BMA_PART_TABLE_NOT_EXIST,
    INVALID_DATA = INVALID_DATA_BYTE
};


#define PROPERTY_LEN 512
#define PROPERTY_NAME_LENGTH 128
#define MAC_RULE_LEN 17
#define MAX_TIME_LENGTH 256
#define MAX_LEN_OEM_INFO 255
#define MAX_RULE_LEN 64
#define MAX_PROCUCT_ID_LEN 16


#ifdef ARM64_HI1711_ENABLED
#define IBMC_VERSION ("0x20")
#else
#define IBMC_VERSION ("0x10")
#endif

#define RFACTION_MANAGER_ACTION_POWER_ON_PERMIT "PowerOnPermit"


#define RFPROP_SYSTEM_MANAGER_INFO "SystemManagerInfo"
#define RFPROP_SYSTEM_MANAGER_INFO_ID "Id"
#define RFPROP_SYSTEM_MANAGER_INFO_ID_PATH "Oem/Huawei/SystemManagerInfo/Id"
#define RFPROP_SYSTEM_MANAGER_INFO_NAME "Name"
#define RFPROP_SYSTEM_MANAGER_INFO_NAME_PATH "Oem/Huawei/SystemManagerInfo/Name"
#define RFPROP_SYSTEM_MANAGER_INFO_IP "Ip"
#define RFPROP_SYSTEM_MANAGER_INFO_IP_PATH "Oem/Huawei/SystemManagerInfo/Ip"
#define RFPROP_SYSTEM_MANAGER_INFO_LOCKEDTIME "LockedTime"
#define RFPROP_SYSTEM_MANAGER_INFO_LOCKEDTIME_PATH "Oem/Huawei/SystemManagerInfo/LockedTime"

#define RFPROP_MANAGER_PATCH_LANGUAGE "LanguageSet"
#define RFPROP_MANAGER_PATCH_REMOTE "RemoteOEMInfo"
#define RFPROP_MANAGER_PATCH_DEVICE "DeviceLocation"
#define RFPROP_MANAGER_PATCH_STATELESS "Stateless"
#define RFPROP_MANAGER_PATCH_STATELESS_SYS_ID "SysManagerId"

#define RFPROP_MANAGER_PATCH_STATELESS_SYS_IP "SysManagerIP"
#define RFPROP_MANAGER_PATCH_STATELESS_SYS_PORT "SysManagerPort"
#define ERROR_MANAGER_STATELESS_SYS_IP "Oem/Huawei/Stateless/SysManagerIP"
#define ERROR_MANAGER_STATELESS_SYS_PORT "Oem/Huawei/Stateless/SysManagerPort"
#define ERROR_MANAGER_STATELESS_CONTROLLED_POWERON "Oem/Huawei/Stateless/ControlPowerOnEnabled"

#define RFPROP_MANAGER_PATCH_STATELESS_POWER_ENABLED "ControlPowerOnEnabled"
#define RFPROP_MANAGER_PATCH_STATELESS_SEARCH_ENABLED "AutoSearchEnabled"
#define RFPROP_MANAGER_PATCH_LOGIN "LoginRule"
#define RFPROP_MANAGER_PATCH_LOGIN_ID "MemberId"
#define RFPROP_MANAGER_PATCH_LOGIN_RULE_ENABLED "RuleEnabled"
#define RFPROP_MANAGER_PATCH_LOGIN_START_TIME "StartTime"
#define RFPROP_MANAGER_PATCH_LOGIN_END_TIME "EndTime"
#define RFPROP_MANAGER_PATCH_LOGIN_IP "IP"
#define RFPROP_MANAGER_PATCH_LOGIN_MAC "Mac"
#define RFPROP_MANAGER_PATCH_LOGIN_TIME "StartTime/EndTime"
#define RFPROP_MANAGER_PATCH_LOCAL_ENABLED "LocalKvmEnabled"
#define RFPROP_MANAGER_PATCH_PROPERTY_NAME "Oem/Huawei/%s"
#define RFPROP_MODEL_REDFISH "REDFISH"
#define RFACTION_SYSTEM_LOCAL_KVM "LocalKvmEnabled"
#define RF_SPECIAL_VALUE 2


#define RFPROP_MANAGER_VNC_SESSION_IP "IP"
#define RFPROP_MANAGER_VNC_SESSION_MODE "SessionMode"
#define PROPVAL_MODE_SHARED "Shared"
#define PROPVAL_MODE_EXCLUSIVE "Private"
#define PROPVAL_MODE_SHARED_NUM 1
#define PROPVAL_MODE_EXCLUSIVE_NUM 0
#define PROPERTY_MANAGER_VNCTIMOUT "SessionTimeoutMinutes"
#define PROPERTY_MANAGER_SSLENCRYPTION "SSLEncryptionEnabled"
#define PROPERTY_MANAGER_VNCPASSVAL_DAYS "PasswordValidityDays"
#define PROPERTY_MANAGER_VNCPASSWORD "Password"
#define PROPERTY_MANAGER_VNCKEYLAYOUT "KeyboardLayout"
#define PROPERTY_MANAGER_VNCSERVICE "Sessions"
#define PROPERTY_MANAGER_VNCNUM_SESSION "NumberOfActivatedSessions"
#define RFPROP_MANAGER_VNC "VncService"
#define VNC_PASSWORD_LEN_MIN "1"
#define VNC_PASSWORD_LEN_MAX "8"
#define VNC_PSWD_CMPLX_CHK_FAIL 3
#define VNC_PSWD_LEN_ERR 2
#define VNC_STRLEN_256 256
#define LOGRULE_1 1
#define LOGRULE_2 2
#define LOGRULE_3 3



#define ACTION_URI_ACCOUNTS "/accounts/"
#define ACTION_URI_OEM_HUAWEI "/oem/huawei"
#define ACTION_IMPORT_MUTAU_CERT \
    "/redfish/v1/AccountService/Accounts/%s/Oem/Huawei/Actions/Account.ImportMutualAuthClientCert"
#define ACTION_DELETE_MUTAU_CERT \
    "/redfish/v1/AccountService/Accounts/%s/Oem/Huawei/Actions/Account.DeleteMutualAuthClientCert"
#define ACTION_TAIL_IMPORT_SSH_PUBLIC_KEY "Oem/Huawei/Actions/Account.ImportSSHPublicKey"
#define ACTION_IMPORT_SSH_PUBLIC_KEY \
    "/redfish/v1/AccountService/Accounts/%s/Oem/Huawei/Actions/Account.ImportSSHPublicKey"
#define ACTION_DELETE_SSH_PUBLIC_KEY \
    "/redfish/v1/AccountService/Accounts/%s/Oem/Huawei/Actions/Account.DeleteSSHPublicKey"



#define ACTION_INFO "@Redfish.ActionInfo"
#define ACTION_INFO_IMPORT_MUTAU_CERT "/redfish/v1/AccountService/Accounts/%s/ImportMutualAuthClientCertActionInfo"
#define ACTION_INFO_DELETE_MUTAU_CERT "/redfish/v1/AccountService/Accounts/%s/DeleteMutualAuthClientCertActionInfo"
#define ACTION_INFO_IMPORT_SSH_PUBLIC_KEY "/redfish/v1/AccountService/Accounts/%s/ImportSSHPublicKeyActionInfo"
#define ACTION_INFO_DELETE_SSH_PUBLIC_KEY "/redfish/v1/AccountService/Accounts/%s/DeleteSSHPublicKeyActionInfo"


#define ACTION_INFO_NAME_LOWER_IMPORT_MUTUAL_CERT "importmutualauthclientcertactioninfo"
#define ACTION_INFO_NAME_LOWER_DELETE_MUTUAL_CERT "deletemutualauthclientcertactioninfo"
#define ACTION_INFO_NAME_LOWER_IMPORT_SSH_PUBLIC "importsshpublickeyactioninfo"
#define ACTION_INFO_NAME_LOWER_DELETE_SSH_PUBLIC "deletesshpublickeyactioninfo"
#define ACTION_INFO_NAME_UPPER_IMPORT_MUTUAL_CERT "ImportMutualAuthClientCertActionInfo"
#define ACTION_INFO_NAME_UPPER_DELETE_MUTUAL_CERT "DeleteMutualAuthClientCertActionInfo"
#define ACTION_INFO_NAME_UPPER_IMPORT_SSH_PUBLIC "ImportSSHPublicKeyActionInfo"
#define ACTION_INFO_NAME_UPPER_DELETE_SSH_PUBLIC "DeleteSSHPublicKeyActionInfo"






#define TMP_DUMP_ORIGINAL_FILE "/tmp/dump_info.tar.gz"




#define TMP_CONFIG_PEM_FILE "/tmp/config.xml"

#define TMP_CONFIG_ORIGINAL_FILE "/dev/shm/config.xml"




#define TMP_CLIENT_PEM_FILE "/tmp/client_tmp.cer"





#define TMP_SSH_PUBLIC_KEY_PUB_FILE "/tmp/client_tmp.pub"




#define RFACTION_IMPORT_SSH_PUBLICKEY "Account.ImportSSHPublicKey"
#define RFACTION_DELETE_SSH_PUBLICKEY "Account.DeleteSSHPublicKey"


#define PFPROP_ACCOUNT_INSECURE_PROMPT_ENABLED "AccountInsecurePromptEnabled"
#define PFPROP_USER_VALID_DAYS "UserValidDays"
#define PFPROP_USER_ID "UserId"


#define RFPROP_SSHPK_HASH "SSHPublicKeyHash"

#define PROP_ACCOUNT_INACTIVEA "AccountInactiveTimelimit"
#define PROP_OEM_ACCOUNT_INACTIVEA "Oem/Huawei/AccountInactiveTimelimit"
#define PROP_ACCOUNT_CLI_SESSION_TIMEOUT "CLISessionTimeoutMinutes"

#define USER_PROCESS 7
#define RFPROP_USER_DELETEABLE "Deleteable"
#define RFPROP_USER_DELDISABLEREASON "DelDisableReason"
#define RFPROP_USER_SSHLOGIN "SSHLogin"
#define RFPROP_USER_EMERGENCYUSER "EmergencyUser"
#define RFPROP_USER_TRAPV3USER "TrapV3User"
#define REPROP_USER_UNIQUEADMINUSER "UniqueAdminUser"

#define RFPROP_LOGIN_RULE "LoginRule"

#define RFPROP_LOGIN_INTERFACE "LoginInterface"
#define RFPROP_SNMP_ENCRYPT_PWD_INIT "SNMPEncryptPwdInit"
#define RFPROP_LOGIN_INTERFACE_ARRAY_LEN 8
#define RFPROP_LOGIN_INTERFACE_WEB "Web"
#define RFPROP_LOGIN_INTERFACE_WEB_VALUE 1
#define RFPROP_LOGIN_INTERFACE_SNMP "SNMP"
#define RFPROP_LOGIN_INTERFACE_SNMP_VALUE 2
#define RFPROP_LOGIN_INTERFACE_IPMI "IPMI"
#define RFPROP_LOGIN_INTERFACE_IPMI_VALUE 4
#define RFPROP_LOGIN_INTERFACE_SSH "SSH"
#define RFPROP_LOGIN_INTERFACE_SSH_VALUE 8
#define RFPROP_LOGIN_INTERFACE_SFTP "SFTP"
#define RFPROP_LOGIN_INTERFACE_SFTP_VALUE 16
#define RFPROP_LOGIN_INTERFACE_LOCAL "Local"
#define RFPROP_LOGIN_INTERFACE_LOCAL_VALUE 64
#define RFPROP_LOGIN_INTERFACE_REDFISH "Redfish"
#define RFPROP_LOGIN_INTERFACE_REDFISH_VALUE 128
#define RFPROP_LOGIN_INTERFACE_MAX_NUM 1


#define RFPROP_LOGIN_INTERFACE_WEB_NUM 0


#define RFPROP_PWD_VALID_DAYS "PasswordValidityDays"



#define SSH_PUBLIC_KEY_MAX_LEN 2048 // public key最大长度

#define IMPORT_CERTIFICATE_MAX_LEN (1 * 1024 * 1024)

#define RFPROP_MANAGER_SMTP_SERVICEENABLED "ServiceEnabled"
#define RFPROP_MANAGER_SMTP_SERVICEADDRESS "ServerAddress"
#define RFPROP_MANAGER_SMTP_CERTVERIFYENABLED "CertVerificationEnabled"

#define RFPROP_MANAGER_SMTP_TLSENABLED "TLSEnabled"
#define RFPROP_MANAGER_SMTP_ANONYMOUSENABLED "AnonymousLoginEnabled"
#define RFPROP_MANAGER_SMTP_SENDERUSERNAME "SenderUserName"
#define RFPROP_MANAGER_SMTP_SENDERPASSWORD "SenderPassword"
#define RFPROP_MANAGER_SMTP_SENDERADDRESS "SenderAddress"
#define RFPROP_MANAGER_SMTP_EMAILSUBJECT "EmailSubject"
#define RFPROP_MANAGER_SMTP_EMAILSUBJECTCONTAINS "EmailSubjectContains"
#define RFPROP_MANAGER_SMTP_ALSRMSEVERITY "AlarmSeverity"
#define RFPROP_MANAGER_SMTP_RECIPIENTADDRESSES "RecipientAddresses"
#define RFPROP_MANAGER_SMTP_SERVERPORT "ServerPort"

#define RFPROP_MANAGER_SMTP_EMAILADDRESS "EmailAddress"
#define RFPROP_MANAGER_SMTP_DESCRIPTION "Description"

#define RFPROP_MANAGER_SMTP_IMPORT_ROOT_CERT "SmtpService.ImportRootCertificate"
#define RFPROP_MANAGER_SMTP_POST_ACTION_NAME "SmtpService.SubmitTestEvent"
#define RFPROP_MANAGER_SMTP_ACTION_NAME "#SmtpService.SubmitTestEvent"
#define RFPROP_MANAGER_SMTP_ACTION_INFONAME "@Redfish.ActionInfo"
#define RFPROP_MANAGER_SMTP_ACTION_TARGETURI "/redfish/v1/Managers/%s/SmtpService/Actions/SmtpService.SubmitTestEvent"
#define RFPROP_MANAGER_SMTP_ACTION_INFOURI "/redfish/v1/Managers/%s/SmtpService/SubmitTestEventActionInfo"
#define TMP_SMTP_ROOT_CERT_FILE "/tmp/smtp_root_cert.pem"
#define RFACTION_SMTP_IMPORT_CERT_TARGETURI \
    "/redfish/v1/Managers/%s/SmtpService/Actions/SmtpService.ImportRootCertificate"
#define RFACTION_SMTP_IMPORT_CERT_INFOURI "/redfish/v1/Managers/%s/SmtpService/ImportRootCertificateActionInfo"





#define URI_FORMAT_SYS_ETH_VLANS "/redfish/v1/Systems/%s/EthernetInterfaces/%s/VLANs"
#define URI_FORMAT_SYS_ETH_VLAN "/redfish/v1/Systems/%s/EthernetInterfaces/%s/VLANs/%u"
#define URI_FORMAT_SYS_ETH_VLAN_ACINFO "/redfish/v1/Systems/%s/EthernetInterfaces/%s/VLANs/%d/ConfigureActionInfo"
#define URI_FORMAT_SYS_ETH_VLAN_ACTION \
    "/redfish/v1/Systems/%s/EthernetInterfaces/%s/VLANs/%d/Actions/Oem/Huawei/VLanNetworkInterface.Configure"
#define SESSIONSERVICE_SESSIONS "/redfish/v1/SessionService/Sessions"
#define ACCOUNTSERVICE_ROLES "/redfish/v1/AccountService/Roles"
#define SYSTEMS_STORAGES "/redfish/v1/Systems/%s/Storages"
#define SYSTEMS_STORAGE "/redfish/v1/Systems/%s/Storages/RAIDStorage%d"
#define SYSTEMS_STORAGE_BIOS "/redfish/v1/Systems/%s/Bios"
#define URI_FORMAT_CHASSIS_FM_BOARDS "/redfish/v1/Chassis/%s/Boards/%s"
#define URI_FORMAT_CHASSIS_FM_PCIEDEVICES "/redfish/v1/Chassis/%s/PCIeDevices/%s"
#define URI_FORMAT_MANAGER_SYSLOG "/redfish/v1/Managers/%s/SyslogService"
#define CHASSIS_DRIVES_URI "/redfish/v1/Chassis/%s/Drives/%s%s"
#define SYSTEMS_STORAGE_CONTROLLER_URI "/redfish/v1/Systems/%s/Storages/RAIDStorage%d"
#define SYSTEMS_STORAGE_SDCARDCONTROLLER_URI "/redfish/v1/Systems/%s/Storages/SDStorage"
#define SYSTEMS_STORAGE_VOLUMES_URI "/redfish/v1/Systems/%s/Storages/RAIDStorage%d/Volumes"


#define MANAGER_SMTPSERVICE_URI "/redfish/v1/Managers/%s/SmtpService"
#define MANAGER_SMTPSERVICE_ACTIONINFO_URI "/redfish/v1/Managers/%s/SmtpService/SubmitTestEventActionInfo"


#define URI_FORMAT_MANAGER_SWITCHPROFILES "/redfish/v1/Managers/%s/SwitchProfiles"
#define MANAGERS_SWITCHPROFILES_METADATA "/redfish/v1/$metadata#Managers/Members/%s/SwitchProfiles/$entity"


#define MANAGER_SPSERVICE_URI "/redfish/v1/Managers/%s/SPService"

#define MANAGER_SPSERVICE_RAID_CURRENTS_URI "/redfish/v1/Managers/%s/SPService/SPRAIDCurrentConfigurations"
#define MANAGER_SPSERVICE_RAIDS_URI "/redfish/v1/Managers/%s/SPService/SPRAID"
#define MANAGER_SPSERVICE_OSINSTALLS_URI "/redfish/v1/Managers/%s/SPService/SPOSInstallPara"
#define MANAGER_SPSERVICE_UPDATES_URI "/redfish/v1/Managers/%s/SPService/SPFWUpdate"
#define MANAGER_SPSERVICE_RESULTS_URI "/redfish/v1/Managers/%s/SPService/SPResult"
#define MANAGER_SPSERVICE_CFGS_URI "/redfish/v1/Managers/%s/SPService/SPCfg"
#define MANAGER_SPSERVICE_DIAGNOSES_URI "/redfish/v1/Managers/%s/SPService/SPDiagnose"
#define MANAGER_SPSERVICE_DIAGNOSE_URI "/redfish/v1/Managers/%s/SPService/SPDiagnose/%s"
#define MANAGER_SPSERVICE_DRIVE_ERASES_URI "/redfish/v1/Managers/%s/SPService/SPDriveErase"
#define MANAGER_SPSERVICE_DRIVE_ERASE_URI "/redfish/v1/Managers/%s/SPService/SPDriveErase/%s"

#define MANAGER_SPSERVICE_RAID_CURRENT_URI "/redfish/v1/Managers/%s/SPService/SPRAIDCurrentConfigurations/%s"
#define MANAGER_SPSERVICE_RAID_URI "/redfish/v1/Managers/%s/SPService/SPRAID/%s"
#define MANAGER_SPSERVICE_OSINSTALL_URI "/redfish/v1/Managers/%s/SPService/SPOSInstallPara/%s"
#define MANAGER_SPSERVICE_UPDATE_URI "/redfish/v1/Managers/%s/SPService/SPFWUpdate/%s"
#define MANAGER_SPSERVICE_RESULT_URI "/redfish/v1/Managers/%s/SPService/SPResult/%s"
#define MANAGER_SPSERVICE_CFG_URI "/redfish/v1/Managers/%s/SPService/SPCfg/%s"

#define MANAGER_SPSERVICE_DEVICEINFO_URI "/redfish/v1/Managers/%s/SPService/DeviceInfo"

#define URI_FORMAT_VNCSERVICE_ODATAID "/redfish/v1/Managers/%s/VncService"
#define URI_FORMAT_LOGRULE "/redfish/v1/Managers/%s#/Oem/Huawei/LoginRule/%d"
#define PROPERTY_LOGIN_RULE_PATH "Oem/Huawei/LoginRule"

#define PROPERTY_MANAGER_KEYBOARDLAYOUT_LEN 8
#define URI_FORMAT_VNCSESSION_URI "/redfish/v1/Sessionservice/Sessions/%s"


#define VLANS_ODATA_CONTEXT "/redfish/v1/$metadata#Systems/Members/%s/EthernetInterfaces/Members/%s/VLANs/$entity"
#define VLAN_ODATA_CONTEXT \
    "/redfish/v1/$metadata#Systems/Members/%s/EthernetInterfaces/Members/%s/VLANs/Members/$entity"
#define SYSTEMS_STROGES_METADATA "/redfish/v1/$metadata#Systems/Members/%s/Storages/$entity"
#define CHASSIS_DRIVES_METADATA "/redfish/v1/$metadata#Chassis/Members/%s/Drives/Members/$entity"
#define SYSTEMS_STROGE_METADATA "/redfish/v1/$metadata#Systems/Members/%s/Storages/Members/$entity"
#define SYSTEMS_ETH_METADATA "/redfish/v1/$metadata#Systems/Members/%s/EthernetInterfaces/Members/$entity"
#define SYSTEMS_ETHS_METADATA "/redfish/v1/$metadata#Systems/Members/%s/EthernetInterfaces/$entity"
#define SYSTEMS_PROCE_METADATA "/redfish/v1/$metadata#Systems/Members/%s/Processors/Members/$entity"

#define SYSTEMS_PROCES_METADATA "/redfish/v1/$metadata#Systems/Members/%s/Processors/$entity"

#define SYSTEMS_MEMORY_METADATA "/redfish/v1/$metadata#Systems/Members/%s/Memory/Members/$entity"
#define SYSTEMS_MEMORYS_METADATA "/redfish/v1/$metadata#Systems/Members/%s/Memory/$entity"
#define SYSTEMS_NETWORKINTERFACES_METADATA "/redfish/v1/$metadata#Systems/Members/%s/NetworkInterfaces/$entity"
#define BRIDGE_COLLECT_ODATA_CONTEXT "/redfish/v1/$metadata#Systems/Members/%s/NetworkBridge/$entity"
#define BRIDGE_ODATA_CONTEXT "/redfish/v1/$metadata#Systems/Members/%s/NetworkBridge/Members/$entity"

#define BONDINGS_ODATA_CONTEXT "/redfish/v1/$metadata#Systems/Members/%s/NetworkBondings/$entity"
#define BONDING_ODATA_CONTEXT "/redfish/v1/$metadata#Systems/Members/%s/NetworkBondings/Members/$entity"
#define IBS_ODATA_CONTEXT "/redfish/v1/$metadata#Systems/Members/%s/InfiniBandInterfaces/$entity"
#define IB_ODATA_CONTEXT "/redfish/v1/$metadata#Systems/Members/%s/InfiniBandInterfaces/Members/$entity"

#define MANAGERS_ETHS_METADATA "/redfish/v1/$metadata#Managers/Members/%s/EthernetInterfaces/$entity"
#define MANAGERS_ETH_METADATA "/redfish/v1/$metadata#Managers/Members/%s/EthernetInterfaces/Members/$entity"
#define MANAGERS_SMTPSERVICE_METADATA "/redfish/v1/$metadata#Managers/Members/%s/SmtpService/$entity"
#define MANAGERS_NETPROCOL_METADATA "/redfish/v1/$metadata#Managers/Members/%s/NetworkProtocol/$entity"
#define CHASSIS_POWER_METADATA "/redfish/v1/$metadata#Chassis/Members/%s/Power/$entity"
#define CHASSIS_THERMAL_METADATA "/redfish/v1/$metadata#Chassis/Members/%s/Thermal/$entity"
#define MANAGERS_SYSLOG_METADATA "/redfish/v1/$metadata#Managers/Members/%s/SyslogService/$entity"
#define URI_FORMAT_VNCSERVICE_CONTEXT "/redfish/v1/$metadata#Managers/Members/%s/VncService/$entity"

#define SYSTEMS_PROCE_VIEW_METADATA "/redfish/v1/$metadata#Systems/Members/%s/ProcessorView/$entity"
#define SYSTEMS_MEMORY_VIEW_METADATA "/redfish/v1/$metadata#Systems/Members/%s/MemoryView/$entity"


#define MANAGERS_SPSERVICE_METADATA "/redfish/v1/$metadata#Managers/Members/%s/SPService/$entity"
#define MANAGERS_SPSERVICE_NETDEVS_METADATA "/redfish/v1/$metadata#Managers/Members/%s/SPService/SPNetDev/$entity"
#define MANAGERS_SPSERVICE_NETDEV_METADATA \
    "/redfish/v1/$metadata#Managers/Members/%s/SPService/SPNetDev/Members/$entity"
#define MANAGERS_SPSERVICE_RAID_CURRENTS_METADATA \
    "/redfish/v1/$metadata#Managers/Members/%s/SPService/SPRAIDCurrentConfigurations/$entity"
#define MANAGERS_SPSERVICE_RAIDS_METADATA "/redfish/v1/$metadata#Managers/Members/%s/SPService/SPRAID/$entity"
#define MANAGERS_SPSERVICE_RAID_CURRENT_METADATA \
    "/redfish/v1/$metadata#Managers/Members/%s/SPService/SPRAIDCurrentConfigurations/Members/$entity"
#define MANAGERS_SPSERVICE_RAID_METADATA "/redfish/v1/$metadata#Managers/Members/%s/SPService/SPRAID/Members/$entity"
#define MANAGERS_SPSERVICE_OSINSTALLS_METADATA \
    "/redfish/v1/$metadata#Managers/Members/%s/SPService/SPOSInstallPara/$entity"
#define MANAGERS_SPSERVICE_OSINSTALL_METADATA \
    "/redfish/v1/$metadata#Managers/Members/%s/SPService/SPOSInstallPara/Members/$entity"
#define MANAGERS_SPSERVICE_UPDATES_METADATA "/redfish/v1/$metadata#Managers/Members/%s/SPService/SPFWUpdate/$entity"
#define MANAGERS_SPSERVICE_UPDATE_METADATA \
    "/redfish/v1/$metadata#Managers/Members/%s/SPService/SPFWUpdate/Members/$entity"
#define MANAGERS_SPSERVICE_RESULTS_METADATA "/redfish/v1/$metadata#Managers/Members/%s/SPService/SPResult/$entity"
#define MANAGERS_SPSERVICE_RESULT_METADATA \
    "/redfish/v1/$metadata#Managers/Members/%s/SPService/SPResult/Members/$entity"
#define MANAGERS_SPSERVICE_CFGS_METADATA "/redfish/v1/$metadata#Managers/Members/%s/SPService/SPCfg/$entity"
#define MANAGERS_SPSERVICE_CFG_METADATA "/redfish/v1/$metadata#Managers/Members/%s/SPService/SPCfg/Members/$entity"
#define MANAGERS_SPSERVICE_DIAGNOSES_METADATA "/redfish/v1/$metadata#Managers/Members/%s/SPService/SPDiagnose/$entity"
#define MANAGERS_SPSERVICE_DIAGNOSE_METADATA \
    "/redfish/v1/$metadata#Managers/Members/%s/SPService/SPDiagnose/Members/$entity"
#define MANAGERS_SPSERVICE_DRIVE_ERASES_METADATA \
    "/redfish/v1/$metadata#Managers/Members/%s/SPService/SPDriveErase/$entity"
#define MANAGERS_SPSERVICE_DRIVE_ERASE_METADATA \
    "/redfish/v1/$metadata#Managers/Members/%s/SPService/SPDriveErase/Members/$entity"

#define MANAGERS_SPSERVICE_DEVICEINFO_METADATA "/redfish/v1/$metadata#Managers/Members/%s/SPService/DeviceInfo/$entity"


#define SYSTEMS_PROCESSORS_HISTORY_USAGE_RATE_METADATA \
    "/redfish/v1/$metadata#Systems/Members/%s/ProcessorsHistoryUsageRate/$entity"
#define SYSTEMS_MEMORY_HISTORY_USAGE_RATE_METADATA \
    "/redfish/v1/$metadata#Systems/Members/%s/MemoryHistoryUsageRate/$entity"
#define SYSTEMS_NETWORK_HISTORY_USAGE_RATE_METADATA \
    "/redfish/v1/$metadata#Systems/Members/%s/NetworkHistoryUsageRate/$entity"
#define CHASSIS_INLET_HISTORY_TEMPERATURE_METADATA \
    "/redfish/v1/$metadata#Chassis/Members/%s/Thermal/InletHistoryTemperature/$entity"
#define CHASSIS_POWER_HISTORY_DATA_METADATA "/redfish/v1/$metadata#Chassis/Members/%s/Power/PowerHistoryData/$entity"



#define RFPROP_COOLING_MODE "FanSpeedAdjustmentMode"
#define RFPROP_COOLING_MODE_MANUAL "Manual"
#define RFPROP_COOLING_MODE_AUTOMATIC "Automatic"
#define RFPROP_COOLING_LEVEL "FanSpeedLevelPercents"
#define RFPROP_COOLING_MINIMAL_LEVEL "FanSpeedMinimalLevelPercents" 
#define RFPROP_COOLING_MAXIMUM_LEVEL "FanSpeedMaximumLevelPercents" 
#define RFPROP_COOLING_TIMEOUT "FanManualModeTimeoutSeconds"
#define RFPROP_COOLING_FAN_POWER "FanTotalPowerWatts"
#define RFPROP_COOLING_POSITION "Position"

#define RFPROP_COOLING_SPEED_RATIO "SpeedRatio"
#define RFPROP_COOLING_SLOT_ID "SlotId"
#define SPEED_RATIO_JSON_POINTER "Oem/Huawei/SpeedRatio"

#define RFPROP_COOLING_MANUAL 1
#define RFPROP_COOLING_AUTOMATIC 0
#define RFPROP_COOLING_DEFAULT_TIME 30
#define RFPROP_COOLING_SET_COUNT 1


#define RFPROP_FAN_SLOT_NUM "SlotNumber"


#define RFPROP_SMART_COOLING_MODE "FanSmartCoolingMode"
#define RFPROP_FAN_ADJUSTMENT_MODE "FanSpeedAdjustmentMode"
#define RFPROP_SMART_COOLING_CUSTOM "FanSpeedCustom"
#define RFPROP_COOLING_MEDIUM "CoolingMedium"
#define RFPROP_SMART_COOLING_COREM "CPUTargetTemperatureCelsius"
#define RFPROP_SMART_COOLING_INLET_SPEED "InletTemperature"
#define RFPROP_SMART_COOLING_OUTLET "OutletTargetTemperatureCelsius"
#define RFPROP_SMART_COOLING_TEM_READ "TemperatureRangeCelsius"
#define RFPROP_SMART_COOLING_FAN_SPEED "FanSpeedPercents"

#define RF_SMART_COOLING_LOW_POWER "LowPowerConsumption"
#define RF_SMART_COOLING_HIGH_POWER "HighPowerConsumption"
#define RF_SMART_COOLING_MED_POWER "MediumPowerConsumption"
#define RF_SMART_COOLING_ENERGY_SAVE "EnergySaving"
#define RF_SMART_COOLING_LOW_NOISE "LowNoise"
#define RF_SMART_COOLING_HIGH_PER "HighPerformance"
#define RF_SMART_COOLING_CUSTOM "Custom"
#define RF_SMART_COOLING_LIQUID "LiquidCooling"
#define RFPROP_VALUE_AIRCOOLED "AirCooled"
#define RFPROP_VALUE_LIQUIDCOOLED "LiquidCooled"

#define RFPROP_CUSTOM_DISK_TOBJ "DiskTargetTemperatureCelsius"
#define RFPROP_CUSTOM_DISK_TOBJ_MIN "DiskMinTargetTemperatureCelsius"
#define RFPROP_CUSTOM_DISK_TOBJ_MAX "DiskMaxTargetTemperatureCelsius"
#define RFPROP_CUSTOM_MEMORY_TOBJ "MemoryTargetTemperatureCelsius"
#define RFPROP_CUSTOM_MEMORY_TOBJ_MIN "MemoryMinTargetTemperatureCelsius"
#define RFPROP_CUSTOM_MEMORY_TOBJ_MAX "MemoryMaxTargetTemperatureCelsius"
#define RFPROP_CUSTOM_PCH_TOBJ "PCHTargetTemperatureCelsius"
#define RFPROP_CUSTOM_PCH_TOBJ_MIN "PCHMinTargetTemperatureCelsius"
#define RFPROP_CUSTOM_PCH_TOBJ_MAX "PCHMaxTargetTemperatureCelsius"
#define RFPROP_CUSTOM_VRD_TOBJ "VRDTargetTemperatureCelsius"
#define RFPROP_CUSTOM_VRD_TOBJ_MIN "VRDMinTargetTemperatureCelsius"
#define RFPROP_CUSTOM_VRD_TOBJ_MAX "VRDMaxTargetTemperatureCelsius"
#define RFPROP_CUSTOM_VDDQ_TOBJ "VDDQTargetTemperatureCelsius"
#define RFPROP_CUSTOM_VDDQ_TOBJ_MIN "VDDQMinTargetTemperatureCelsius"
#define RFPROP_CUSTOM_VDDQ_TOBJ_MAX "VDDQMaxTargetTemperatureCelsius"
#define RFPROP_CUSTOM_NPU_HBM_TOBJ          "NPUHbmTargetTemperatureCelsius"
#define RFPROP_CUSTOM_NPU_HBM_TOBJ_MIN      "NPUHbmMinTargetTemperatureCelsius"
#define RFPROP_CUSTOM_NPU_HBM_TOBJ_MAX      "NPUHbmMaxTargetTemperatureCelsius"
#define RFPROP_CUSTOM_NPU_AICORE_TOBJ       "NPUAiCoreTargetTemperatureCelsius"
#define RFPROP_CUSTOM_NPU_AICORE_TOBJ_MIN   "NPUAiCoreMinTargetTemperatureCelsius"
#define RFPROP_CUSTOM_NPU_AICORE_TOBJ_MAX   "NPUAiCoreMaxTargetTemperatureCelsius"
#define RFPROP_CUSTOM_NPU_BOARD_TOBJ       "NPUBoardTargetTemperatureCelsius"
#define RFPROP_CUSTOM_NPU_BOARD_TOBJ_MIN   "NPUBoardMinTargetTemperatureCelsius"
#define RFPROP_CUSTOM_NPU_BOARD_TOBJ_MAX   "NPUBoardMaxTargetTemperatureCelsius"
#define RFPROP_CUSTOM_DISK_ENV "DiskTemperature"
#define RF_JSON_POINTER_COOLING_CUSTOM RFPROP_COMMON_OEM "/" RFPROP_COMMON_HUAWEI "/" RFPROP_SMART_COOLING_CUSTOM
#define RF_JSON_POINTER_DISK_TOBJ RF_JSON_POINTER_COOLING_CUSTOM "/" RFPROP_CUSTOM_DISK_TOBJ
#define RF_JSON_POINTER_MEMORY_TOBJ RF_JSON_POINTER_COOLING_CUSTOM "/" RFPROP_CUSTOM_MEMORY_TOBJ
#define RF_JSON_POINTER_PCH_TOBJ RF_JSON_POINTER_COOLING_CUSTOM "/" RFPROP_CUSTOM_PCH_TOBJ
#define RF_JSON_POINTER_VRD_TOBJ RF_JSON_POINTER_COOLING_CUSTOM "/" RFPROP_CUSTOM_VRD_TOBJ
#define RF_JSON_POINTER_VDDQ_TOBJ RF_JSON_POINTER_COOLING_CUSTOM "/" RFPROP_CUSTOM_VDDQ_TOBJ
#define RF_JSON_POINTER_NPU_HBM_TOBJ RF_JSON_POINTER_COOLING_CUSTOM"/"RFPROP_CUSTOM_NPU_HBM_TOBJ
#define RF_JSON_POINTER_NPU_AICORE_TOBJ RF_JSON_POINTER_COOLING_CUSTOM"/"RFPROP_CUSTOM_NPU_AICORE_TOBJ
#define RF_JSON_POINTER_NPU_BOARD_TOBJ RF_JSON_POINTER_COOLING_CUSTOM"/"RFPROP_CUSTOM_NPU_BOARD_TOBJ
#define RF_JSON_POINTER_INLET_ENV RF_JSON_POINTER_COOLING_CUSTOM "/" RFPROP_SMART_COOLING_INLET_SPEED
#define RF_JSON_POINTER_DISK_ENV RF_JSON_POINTER_COOLING_CUSTOM "/" RFPROP_CUSTOM_DISK_ENV
#define RF_JSON_POINTER_INLET_ENV_TEMP RF_JSON_POINTER_INLET_ENV "/" RFPROP_SMART_COOLING_TEM_READ
#define RF_JSON_POINTER_INLET_ENV_SPEED RF_JSON_POINTER_INLET_ENV "/" RFPROP_SMART_COOLING_FAN_SPEED
#define RF_JSON_POINTER_DISK_ENV_TEMP RF_JSON_POINTER_DISK_ENV "/" RFPROP_SMART_COOLING_TEM_READ
#define RF_JSON_POINTER_DISK_ENV_SPEED RF_JSON_POINTER_DISK_ENV "/" RFPROP_SMART_COOLING_FAN_SPEED
#define RF_JSON_POINTER_INLET_TEMP_DEL                                                        \
    RFPROP_COMMON_HUAWEI "/" RFPROP_SMART_COOLING_CUSTOM "/" RFPROP_SMART_COOLING_INLET_SPEED \
        "/" RFPROP_SMART_COOLING_TEM_READ
#define RF_JSON_POINTER_INLET_SPD_DEL                                                         \
    RFPROP_COMMON_HUAWEI "/" RFPROP_SMART_COOLING_CUSTOM "/" RFPROP_SMART_COOLING_INLET_SPEED \
        "/" RFPROP_SMART_COOLING_FAN_SPEED
#define RF_JSON_POINTER_DISK_TEMP_DEL \
    RFPROP_COMMON_HUAWEI "/" RFPROP_SMART_COOLING_CUSTOM "/" RFPROP_CUSTOM_DISK_ENV "/" RFPROP_SMART_COOLING_TEM_READ
#define RF_JSON_POINTER_DISK_SPD_DEL \
    RFPROP_COMMON_HUAWEI "/" RFPROP_SMART_COOLING_CUSTOM "/" RFPROP_CUSTOM_DISK_ENV "/" RFPROP_SMART_COOLING_FAN_SPEED

typedef struct {
    guint8 component_bit;
    const gchar *pme_prop_name;
    const gchar *rf_key_name;
} CUSTOM_TOBJ_INFO;

typedef struct {
    guint8 component_bit;
    const gchar *pme_prop_name;
    const gchar *rf_min_name;
    const gchar *rf_max_name;
} CUSTOM_TOBJ_RANGE_INFO;

typedef struct {
    guint8 component_bit;
    guint16 policy_index;
    const gchar *speed_range_prop;
    const gchar *rf_key_name;
} CUSTOM_ENV_COOLING_INFO;

typedef struct {
    guint8 component_bit;
    const gchar *json_pointer_name;
    const gchar *pme_method_name;
    const gchar *pme_range_prop;
} CUSTOM_TOBJ_SET_INFO;

typedef struct {
    guint8 component_bit;
    guint16 policy_index;
    const gchar *json_p_obj;
    const gchar *json_p_speed;
    const gchar *json_p_temp;
    const gchar *json_p_speed_del;
    const gchar *json_p_temp_del;
    const gchar *pme_range_prop;
} CUSTOM_ENV_TEMP_SET_INFO;



#define RFPROP_THERMAL_OEM_ACTION_CLEAR_HISTORY_RECORD "Thermal.ClearInletHistoryTemperature"
#define URI_RF_THERMAL_OEM_ACTION_CLEAR_TARGET \
    "/redfish/v1/Chassis/%s/Thermal/Oem/Huawei/Actions/Thermal.ClearInletHistoryTemperature"
#define URI_RF_THERMAL_OEM_ACTION_CLEAR_ACTION_INFO \
    "/redfish/v1/Chassis/%s/Thermal/ClearInletHistoryTemperatureActionInfo"



#define RFPROP_SMART_COOLING_CPUTEM_MIN "MinCPUTargetTemperatureCelsius"
#define RFPROP_SMART_COOLING_CPUTEM_MAX "MaxCPUTargetTemperatureCelsius"

#define RFPROP_SMART_COOLING_OUTLETTEM_MIN "MinOutletTargetTemperatureCelsius"
#define RFPROP_SMART_COOLING_OUTLETTEM_MAX "MaxOutletTargetTemperatureCelsius"

#define RFPROP_SMART_COOLING_INLETTEM_MIN "MinTemperatureRangeCelsius"
#define RFPROP_SMART_COOLING_INLETTEM_MAX "MaxTemperatureRangeCelsius"

#define FAN_SPEED_MAX "MaxFanSpeedPercents"
#define FAN_SPEED_MIN "MinFanSpeedPercents"

#define INLETTEM_VALUE_MIN 0
#define INLETTEM_VALUE_MAX 60

#define DOMAIN_STR_SPLIT_FLAG "," 
#define DOMAIN_STR_CN_OFFSET 3    

#define RFPROP_THERMAL_TEMPARATURE_SUMMARY "TemperatureSummary"
#define RFPROP_THERMAL_SENSOR_COUNT "Count"
#define RFPROP_THERMAL_FAN_SUMMARY "FanSummary"


#define SYSTEMS_STRORAGE_DRIVE_METADATA \
    "/redfish/v1/$metadata#Systems/Members/%s/Storages/Members/1/Drives/Members/$entity"



#define LOGENTRY_REISTRY_LEN 125
#define SELINFO_XML_FILENAME_LEN 256
#define SELINFO_SEVERITY_LEN 4
#define SELINFO_SUBJECJ_RTPE_LEN 32
#define RFPROP_SEL_MESSARGLEN 64
#define RFPROP_SEL_EVENTCODELEN 16
#define SELINFO_DESC_LEN 255
#define SELINFO_CREATE_TIME 32
#define SELINFO_TRIGMODE_LEN 4
#define LOGENTRY_SEVERITY_OK "OK"
#define LOGENTRY_SEVERITY_WARNING "Warning"
#define LOGENTRY_SEVERITY_CRITICAL "Critical"

#define LOGENTRY_STATUS_INFORMATIONAL_CODE 0
#define LOGENTRY_STATUS_MINOR_CODE 1
#define LOGENTRY_STATUS_MAJOR_CODE 2
#define LOGENTRY_STATUS_CRITICAL_CODE 3

#define SEL_MESSAGEID_STR "%s.%s"
#define SELINFI_FILE_PATH "%s/%s"
#define SYSTEM_LOGCOLLECTION_URI "/redfish/v1/Systems/%s/LogServices/Log1/Entries/%d"
#define SYSTEM_LOGCOLLECTION_CONTEXT "/redfish/v1/$metadata#Systems/Members/%s/LogServices/Log1/Entries/$entity"
#define SYSTEM_LOGCOLLECTION_ODATAID "/redfish/v1/Systems/%s/LogServices/Log1/Entries"

#define SYSTEM_LOGENTRY_CONTEXT "/redfish/v1/$metadata#Systems/Members/%s/LogServices/Log1/Entries/Members/$entity"
#define SYSTEM_LOGENTRY_ODATAID "/redfish/v1/Systems/%s/LogServices/Log1/Entries/%s"
#define SYSTEM_LOGENTRY_NAME "Log Entry %s"
#define RFPROP_SEL_SEVERITY "Severity"
#define RFPROP_SEL_EVENTTIMESTAMP "EventTimestamp"
#define RFPROP_SEL_ENTRYTYPE "EntryType"
#define RFPROP_SEL_MESSAGE "Message"
#define RFPROP_SEL_MESSAGEID "MessageId"
#define RFPROP_SEL_MESSAGEARGS "MessageArgs"
#define RFPROP_SEL_EVENTTYPE "EventType"
#define RFPROP_SEL_EVENTID "EventId"
#define RFPROP_SEL_CREATED "Created"


#define SELINFO_SELECT_SQL                                                                                           \
    "select level, subjecttype, eventdesc, trigmode, alerttime, eventcode, oldeventcode, eventsubject, sn, pn from " \
    "%s left join monitor_table on monitor_table.evtid=event_table.eventid and "                                     \
    "monitor_table.evttime=event_table.alerttime where eventid = %d "





#define REGISTRY_FILE_COL_ODATAID "/redfish/v1/Registries"
#define REGISTRY_LOCATION_ODATAID "/redfish/v1/RegistryStore/Messages/en"
#define RFPROP_LOCATION "Location"
#define RFRPOP_LANGUAGE "Language"
#define EVT_REGISTRY_LANG_EN "en"
#define RFPROP_LOCATION_URI "Uri"
#define RFPROP_REGISTRY "Registry"



#define URI_FORMAT_SYSTEM_DRIVE "/redfish/v1/Systems/%s/Storages/1/Drives/%s"
#define URI_FORMAT_PS "/redfish/v1/Chassis/%s/Power#/PowerSupplies/%d"
#define URI_FORMAT_FAN "/redfish/v1/Chassis/%s/Thermal#/Fans/%d"
#define URI_CLASS_FAN "Fan"
#define URI_CLASS_CPU "Cpu"



#define RFPROP_RACK_MGNT_SERVICE "RackManagementService"
#define URI_FORMAT_RACK_MGNT_SERVICE "/redfish/v1/RackManagementService"
#define URI_FORMAT_SESSION_AGENT "/redfish/v1/RackManagementService/SessionAgent"
#define RFPROP_TOKEN "Token"
#define RFPROP_SET_DEV_TOKEN_ACTION "SessionAgent.SetDeviceToken"
#define RF_SET_DEV_TOKEN_ACTION "/redfish/v1/RackManagementService/SessionAgent/Actions/SessionAgent.SetDeviceToken"
#define URI_FORMAT_SET_DEV_TOKEN_ACTIONINFO "/redfish/v1/RackManagementService/SessionAgent/SetDeviceTokenActionInfo"



#define URI_FORMAT_NETWORK_AGENT "/redfish/v1/RackManagementService/NetworkAgent"
#define RFPROP_SET_DEV_IPINFO_ACTION "NetworkAgent.SetDeviceIPInfo"
#define RF_SET_DEV_IPINFO_ACTION "/redfish/v1/RackManagementService/NetworkAgent/Actions/NetworkAgent.SetDeviceIPInfo"
#define URI_FORMAT_SET_DEV_IPINFO_ACTIONINFO "/redfish/v1/RackManagementService/NetworkAgent/SetDeviceIPInfoActionInfo"





#define AUTH_ENABLE 1
#define AUTH_DISABLE 0

#define AUTH_PRIV_NONE 0



#define PROPERTY_ACCOUNT_SERVICE_TLS "TlsVersion"
#define RFPROP_TLS_10 "Tls1.0"
#define RFPROP_TLS_11 "Tls1.1"
#define RFPROP_TLS_12 "Tls1.2"
#define RFPROP_TLS_13 "Tls1.3"

#define PROPERTY_ACCOUNT_SERVICE_SECURITY_BANNER_ENABLED "SecurityBannerEnabled"
#define PROPERTY_ACCOUNT_SERVICE_SECURITY_BANNER "SecurityBanner"
#define PROPERTY_ACCOUNT_SERVICE_DEFAULT_SECURITY_BANNER "DefaultSecurityBanner"
#define PROPERTY_ACCOUNT_SERVICE_PASSWORD_COMPLEXITY_ENABLED "PasswordComplexityCheckEnabled"
#define PROPERTY_ACCOUNT_SERVICE_SSH_AUTH_ENABLED "SSHPasswordAuthenticationEnabled"

#define PROPERTY_ACCOUNT_SERVICE_MINIMUM_PASSWORD_AGE_DAYS "MinimumPasswordAgeDays"
#define PROPERTY_ACCOUNT_SERVICE_OS_USER_MANAGEMENT_ENABLED "OSUserManagementEnabled"
#define PROPERTY_ACCOUNT_SERVICE_PASSWORD_VALIDITY_DAYS "PasswordValidityDays"
#define PROPERTY_ACCOUNT_SERVICE_PREVIOUS_PASSWORDS_DISALLOWED_COUNT "PreviousPasswordsDisallowedCount"

#define PROPERTY_ACCOUNT_SERVICE_EMERGENCY_LOHIN_USER "EmergencyLoginUser"
#define PROPERTY_ACCOUNT_SERVICE_TWO_FACTOR_AUTH_INFO "TwoFactorAuthenticationInformation"
#define PROPERTY_ACCOUNT_SERVICE_TWO_FACTOR_ENABLED "TwoFactorAuthenticationStateEnabled"
#define PROPERTY_ACCOUNT_SERVICE_CERT_CHECK_ENABLED "CertificateRevocationCheckEnabled"
#define PROPERTY_ACCOUNT_SERVICE_CRL_CHECK_ENABLED "CrlVerificationEnabled"
#define PROPERTY_ACCOUNT_SERVICE_ROOT_CERT "RootCertificate"
#define PROPERTY_ACCOUNT_SERVICE_CERT_ID "CertId"
#define PROPERTY_ACCOUNT_SERVICE_ISSUE_BY "IssueBy"
#define PROPERTY_ACCOUNT_SERVICE_ISSUE_TO "IssueTo"
#define PROPERTY_ACCOUNT_SERVICE_VALID_FROM "ValidFrom"
#define PROPERTY_ACCOUNT_SERVICE_VALID_TO "ValidTo"
#define PROPERTY_ACCOUNT_SERVICE_SERIAL_NUMBER "SerialNumber"
#define PROPERTY_ACCOUNT_SERVICE_KEY_USAGE "KeyUsage"
#define PROPERTY_ACCOUNT_SERVICE_KEY_LENGTH "PublicKeyLengthBits"
#define PROPERTY_ACCOUNT_SERVICE_SIG_ALGO "SignatureAlgorithm"
#define PROPERTY_ACCOUNT_SERVICE_ACTIONS "Actions"
#define PROPERTY_ACCOUNT_SERVICE_IMPORT_ROOT_CERT_KEY "#AccountService.ImportRootCertificate"
#define PROPERTY_ACCOUNT_SERVICE_TARGET "target"
#define PROPERTY_ACCOUNT_SERVICE_ACTION_INFO "@Redfish.ActionInfo"
#define PROPERTY_ACCOUNT_SERVICE_IMPORT_INFO "/redfish/v1/AccountService/ImportRootCertificateActionInfo"
#define PROPERTY_ACCOUNT_SERVICE_DELETE_INFO "/redfish/v1/AccountService/DeleteRootCertificateActionInfo"
#define PROPERTY_ACCOUNT_SERVICE_IMPORT_ROOT_CERT_VALUE \
    "/redfish/v1/AccountService/Oem/Huawei/Actions/AccountService.ImportRootCertificate"
#define PROPERTY_ACCOUNT_SERVICE_DELETE_ROOT_CERT_KEY "#AccountService.DeleteRootCertificate"
#define PROPERTY_ACCOUNT_SERVICE_DELETE_ROOT_CERT_VALUE \
    "/redfish/v1/AccountService/Oem/Huawei/Actions/AccountService.DeleteRootCertificate"
#define PROPERTY_ACCOUNT_SERVICE_LINKS "Links"
#define PROPERTY_ACCOUNT_SERVICE_PERMIT_ROLE_IDS_KEY "PermitRuleIds"
#define PROPERTY_ACCOUNT_SERVICE_ODATA_ID "@odata.id"
#define PROPERTY_ACCOUNT_SERVICE_PERMIT_ROLE_IDS_VALUE "/redfish/v1/Registries/PrivilegeRegistry.v1_0_0"
#define PROPERTY_ACCOUNT_SERVICE_SYSTEM_LOCKDOWN_ENABLED "SystemLockDownEnabled"
#define PROPERTY_ACCOUNT_SERVICE_IMPORT_CRL_KEY "#AccountService.ImportCrl"
#define PROPERTY_ACCOUNT_SERVICE_IMPORT_CRL_VALUE \
    "/redfish/v1/AccountService/Oem/Huawei/Actions/AccountService.ImportCrl"
#define PROPERTY_ACCOUNT_SERVICE_IMPORT_CRL_ACTION_INFO "/redfish/v1/AccountService/ImportCrlActionInfo"
#define PROPERTY_ACCOUNT_SERVICE_DELETE_CRL_KEY "#AccountService.DeleteCrl"
#define PROPERTY_ACCOUNT_SERVICE_DELETE_CRL_VALUE \
    "/redfish/v1/AccountService/Oem/Huawei/Actions/AccountService.DeleteCrl"
#define PROPERTY_ACCOUNT_SERVICE_DELETE_CRL_ACTION_INFO "/redfish/v1/AccountService/DeleteCrlActionInfo"
#define PROPERTY_ACCOUNT_SERVICE_IS_IMPORT_CRL "IsImportCrl"
#define PROPPETY_ACCOUNT_SERVICE_CRL_VALID_FROM "CrlValidFrom"
#define PROPPETY_ACCOUNT_SERVICE_CRL_VALID_TO "CrlValidTo"

#define RFACTION_PARAM_ROOT_CERT_ID "RootCertId"
#define RFACTION_PARAM_ROOT_CERT_USAGE "Usage"
#define RF_CERT_USAGE_EVT_SUBSCRIPTION "EventSubscription"
#define RF_CERT_USAGE_FILE_TRANSFER "FileTransfer"
#define IMPORT_MUTUAL_AUTH_CRL_TMP_PATH "/tmp/mutual_auth_tmp.crl"
#define IMPORT_MUTUAL_AUTH_CRL_TRANSFER_FILEINFO_CODE 0x30
#define IMPORT_MUTUAL_AUTH_CRL_TASK_NAME "two-factor crl import"



#define IMPORT_CERT_URI_DISMATCH_ERR 10

#define IMPORT_CERT_INVALID_FILEPATH_ERR 11

#define IMPORT_CERT_FILE_LEN_EXCEED_ERR 12

#define PROPERTY_ACCOUNT_SERVICE_IMPORT_ROOT_CERT "AccountService.ImportRootCertificate"
#define PROPERTY_ACCOUNT_SERVICE_DELETE_ROOT_CERT "AccountService.DeleteRootCertificate"
#define PROPERTY_ACCOUNT_SERVICE_IMPORT_CRL "AccountService.ImportCrl"
#define PROPERTY_ACCOUNT_SERVICE_DELETE_CRL "AccountService.DeleteCrl"


#define PROPERTY_ACCOUNT_SERVICE_ROOT_CERT_PATH "/tmp/ca_tmp.pem"


#define PROPERTY_SECURITY_SERVICE_HTTPS_CERT_PATH "/tmp/https_ca.pem"

#define PROPERTY_SECURITY_SERVICE_HTTPS_CRL_PATH "/tmp/https_ca.crl"

#define PROPERTY_ACCOUNT_SERVICE_CERTIFICATE_TEXT "text"
#define PROPERTY_ACCOUNT_SERVICE_CERTIFICATE_URI "URI"
#define PROPERTY_ACCOUNT_SERVICE_ROOT_CONTENT "Content"
#define PROPERTY_ACCOUNT_SERVICE_ROOT_CERTIFICATE_TYPE "Type"
#define PROPERTY_ACCOUNT_SERVICE_CERTIFICATE_URI_IMPORT "two-factor root cert import"
#define PROPERTY_ACCOUNT_SERVICE_CERTIFICATE_URI_ROOT_CODE 0x13
#define PROPERTY_ACCOUNT_SERVICE_CERT_OVERDUE_WARN_TIME "CertificateOverdueWarningTime"
#define PROPERTY_OEM_SECURITY_CERT_OVERDUE_WARN_TIME "Oem/Huawei/CertificateOverdueWarningTime"
#define PROPERTY_SECURITY_SERVICE_CERTIFICATE_URI_ROOT_CODE 0x14

#define PROPERTY_SECURITY_SERVICE_CERT_ID "CertId"
#define PROPPETY_SECURITY_SERVICE_HTTPS_USAGE "Usage"
#define PROPERTY_SECURITY_SERVICE_ISSUE_BY "IssueBy"
#define PROPERTY_SECURITY_SERVICE_ISSUE_TO "IssueTo"
#define PROPERTY_SECURITY_SERVICE_VALID_FROM "ValidFrom"
#define PROPERTY_SECURITY_SERVICE_VALID_TO "ValidTo"
#define PROPERTY_SECURITY_SERVICE_SERIAL_NUMBER "SerialNumber"
#define PROPERTY_SECURITY_SERVICE_KEY_USAGE "KeyUsage"
#define PROPERTY_SECURITY_SERVICE_SIG_ALGO "SignatureAlgorithm"
#define PROPERTY_SECURITY_SERVICE_KEY_LENGTH "PublicKeyLengthBits"
#define PROPERTY_SECURITY_SERVICE_IS_IMPORT_CRL "IsImportCrl"
#define PROPPETY_SECURITY_SERVICE_CRL_VALID_FROM "CrlValidFrom"
#define PROPPETY_SECURITY_SERVICE_CRL_VALID_TO "CrlValidTo"

#define IMPORT_HTTPS_CRL_TASK_NAME "remote https server crl import"
#define IMPORT_HTTPS_SERVER_CRT_TASK_NAME "remote https server ca import"

#define IMPORT_SECURITY_SERVICE_CRT_TRANSFER_FILEINFO_CODE 0X34
#define IMPORT_SECURITY_SERVICE_CRL_TRANSFER_FILEINFO_CODE 0x33



#define PROPERTY_LDAP_SERVICE "LdapService"
#define PROPERTY_LDAP_SERVICE_ODATA_ID "@odata.id"
#define PROPERTY_LDAP_SERVICE_ODATA_CONTEXT "@odata.context"
#define PROPERTY_LDAP_SERVICE_ODATA_CONTEXT_STR \
    "/redfish/v1/$metadata#AccountService/LdapService/LdapControllers/%s/$entity"

#define PROPERTY_LDAP_SERVICE_URL "/redfish/v1/AccountService/LdapService"
#define PROPERTY_LDAP_SERVICE_ENABLED "LdapServiceEnabled"
#define PROPERTY_LDAP_SERVICE_DOMAIN_CONTROLLER "LdapDomainController"
#define PROPERTY_LDAP_SERVICE_DOMAIN_CONTROLLER_ID "DomainControllerId"
#define PROPERTY_LDAP_SERVICE_ADDRESS "LdapServerAddress"
#define PROPERTY_LDAP_SERVICE_PORT "LdapPort"
#define PROPERTY_LDAP_SERVICE_USER_DOMAIN "UserDomain"
#define PROPERTY_LDAP_SERVICE_USER_FOLDER "UserFolder"
#define PROPERTY_LDAP_SERVICE_BIND_DN "BindDN"
#define PROPERTY_LDAP_SERVICE_BIND_PWD "BindPassword"
#define PROPERTY_LDAP_SERVICE_CERT_ENABLED "CertificateVerificationEnabled"
#define PROPERTY_LDAP_SERVICE_CERT_LEVEL "CertificateVerificationLevel"
#define PROPERTY_LDAP_SERVICE_CERT_INFO "CertificateInformation"
#define PROPERTY_LDAP_SERVICE_CERT_CHAIN_INFO "CertificateChainInformation"
#define PROPERTY_LDAP_SERVICE_CERT_ISSUEBY "IssueBy"
#define PROPERTY_LDAP_SERVICE_CERT_ISSUETO "IssueTo"
#define PROPERTY_LDAP_SERVICE_CERT_SERIALNUMBER "SerialNumber"
#define PROPERTY_LDAP_SERVICE_CERT_VALIDFROM "ValidFrom"
#define PROPERTY_LDAP_SERVICE_CERT_VALIDTO "ValidTo"
#define PROPERTY_LDAP_SERVICE_CERT_SIGALGO "SignatureAlgorithm"
#define PROPERTY_LDAP_SERVICE_CERT_KEYUSAGE "KeyUsage"
#define PROPERTY_LDAP_SERVICE_CERT_KEYLENGTH "PublicKeyLengthBits"
#define PROPERTY_LDAP_SERVICE_CRL_ENABLED "CrlVerificationEnabled"
#define PROPERTY_LDAP_SERVICE_CRL_VALID_FROM "CrlValidFrom"
#define PROPERTY_LDAP_SERVICE_CRL_VALID_TO "CrlValidTo"


#define RFPROP_LDAP_CERT_SEVER_CERT "ServerCert"
#define RFPROP_LDAP_CERT_INTER_CERT "IntermediateCert"
#define RFPROP_LDAP_CERT_ROOT_CERT "RootCert"

#define PROPERTY_LDAP_SERVICE_LDAPGROUPS "LdapGroups"
#define PROPERTY_LDAP_SERVICE_GROUP_ID "MemberId"
#define PROPERTY_LDAP_SERVICE_GROUP_NAME "GroupName"
#define PROPERTY_LDAP_SERVICE_GROUP_DOMAIN "GroupDomain"
#define PROPERTY_LDAP_SERVICE_GROUP_FOLDER "GroupFolder"
#define PROPERTY_LDAP_SERVICE_GROUP_USER_ROLE "GroupRole"
#define PROPERTY_LDAP_SERVICE_GROUP_PERMIT_ROLE "GroupLoginRule"
#define PROPERTY_LDAP_SERVICE_GROUP_LOGIN_INTERFACE "GroupLoginInterface"
#define PROPERTY_LDAP_SERVICE_COLLECTION_MEMBER "/redfish/v1/AccountService/LdapService/LdapControllers/"
#define PROPERTY_LDAP_SERVICE_CONTROLLER_MEMBER "Members"
#define URI_FORMAT_LDAP_SERVICE_PCIEDEVICES_URI "/redfish/v1/Managers/%s#/Oem/Huawei/LoginRule/%d"

#define PROPERTY_LDAP_SERVICE_ACTION "Actions"
#define ACTION_TAIL_LDAP_IMPORT_CERT "Actions/HwLdapController.ImportCert"
#define PROPERTY_LDAP_SERVICE_IMPORT_CERT_METHOD "HwLdapController.ImportCert"
#define PROPERTY_LDAP_SERVICE_ACTION_TARGET "target"
#define PROPERTY_LDAP_SERVICE_ACTION_INFO "@Redfish.ActionInfo"
#define PROPERTY_LDAP_SERVICE_CERT_URI_IMPORT "ldap root cert import"
#define ACTION_TAIL_LDAP_IMPORT_CRL "Actions/LdapController.ImportCrl"
#define PROPERTY_LDAP_SERVICE_IMPORT_CRL_METHOD "LdapController.ImportCrl"
#define ACTION_TAIL_LDAP_DELETE_CRL "Actions/LdapController.DeleteCrl"
#define PROPERTY_LDAP_SERVICE_DELETE_CRL_METHOD "LdapController.DeleteCrl"

#define PROPERTY_LDAP_SERVICE_IMPORT_CERT_ACTIONINFO "/ImportCertActionInfo"
#define PROPERTY_LDAP_SERVICE_IMPORT_CRL_ACTIONINFO "/ImportCrlActionInfo"

#define IMPORT_LDAP_CRL_TMP_PATH "/tmp/ldap_tmp.crl"
#define IMPORT_LDAP_CRL_TASK_NAME "ldap crl import"
#define IMPORT_LDAP_CRL_TRANSFER_FILEINFO_CODE 0x32


#define PROPERTY_KRB_SERVICE "KerberosService"
#define PROPERTY_KRB_SERVICE_ODATA_ID "@odata.id"

#define PROPERTY_KRB_SERVICE_URL "/redfish/v1/AccountService/KerberosService"
#define PROPERTY_KRB_SERVICE_ENABLED "KerberosEnabled"
#define PROPERTY_KRB_SERVICE_COLLECTION_MEMBER "/redfish/v1/AccountService/KerberosService/KerberosControllers/"

#define RFPROP_KRB_COLLECTION_MEMBER "/redfish/v1/AccountService/KerberosService/KerberosControllers/%s"
#define RFPROP_KRB_ODATA_CONTEXT_STR \
    "/redfish/v1/$metadata#AccountService/KerberosService/KerberosControllers/%s/$entity"
#define RFPROP_KRB_ACTION_INFO_URI_B "/redfish/v1/AccountService/KerberosService/KerberosControllers/"
#define RFPROP_KRB_ACTION_INFO_URI_A "/ImportKeyTableActionInfo"
#define ACTION_TAIL_KRB_IMPORT_KEY_TABLE "Actions/HwKerberosController.ImportKeyTable"
#define RFPROP_KRB_ACTION_URI_A "/Actions/HwKerberosController.ImportKeyTable"
#define RFPROP_KRB_IMPORT_METHOD "#HwKerberosController.ImportKeyTable"
#define RFPROP_KRB_SERVICE_ADDRESS "KerberosServerAddress"
#define RFPROP_KRB_SERVICE_PORT "KerberosPort"
#define RFPROP_KRB_SERVICE_REALM "Realm"
#define RFPROP_KRB_SERVICE_IMPORT_METHOD "HwKerberosController.ImportKeyTable"
#define RFPROP_KRB_SERVICE_KRBGROUPS "KerberosGroups"
#define RFPROP_KRB_SERVICE_GROUP_SID "GroupSID"

enum group_role_type {
    G_OPERATOR = 3,
    G_CUSTOM_1 = 5,
    G_CUSTOM_2,
    G_CUSTOM_3,
    G_CUSTOM_4
};


#define MAX_CONTROLLER_COUNT 1


#define DOMAIN_ARRY_LEN 2
#define PROPERTY_LDAP_DOMAIN_LENTH 2013

#define PROPERTY_LDAP_SERVICE_DTR_LENTH 255
#define PROPERTY_LDAP_SERVICE_CERT_IMPORT "HwLdapController.ImportCert"
#define PROPERTY_LDAP_SERVICE_CRL_IMPORT "LdapController.ImportCrl"
#define PROPERTY_LDAP_SERVICE_CRL_DELETE "LdapController.DeleteCrl"
#define PROPERTY_LDAP_CACERT_UPLOAD_SUCCESS 0
#define PROPERTY_LDAP_CACERT_UPLOAD_FAILED 2
#define PROPERTY_LDAP_CACERT_TOO_BIG 1
#define PROPERTY_LDAP_CACERT_FORMAT_FAILED 3
#define PROPERTY_LDAP_NO_MATCH_ERROR 4
#define PROPERTY_LDAP_FOLDER_MISMATCH 5
#define PROPERTY_LDAP_RET_ERROR (-6002000)
#define PROPERTY_LDAP_LENGTH_ERROR (-6002002)
#define MAX_GROUP_COUNT 5
#define MAX_CERT_LEVEL_LEN 10




#define RFPROP_KRB_KEYTABLE_UPLOAD_FAILED 2
#define RFPROP_KRB_KEYTABLE_FORMAT_ERROR 3



#define LDAP_RS_CACERT_TMP_PATH "/tmp/ldap_cacert.cert"
#define TMP_RS_CUSTOM_CERT_PFX_FILE "/tmp/custom_cert_tmp.pfx"




#define RFACTION_EXPORT_TABLE "HwDataAcquisitionService.ExportAcquisitionData"
#define RFACTION_CLEAR_TABLE "HwDataAcquisitionService.ClearHistoryData"


#define RFACTION_DATA_FILTERING "HwDataAcquisitionService.DataFiltering"
#define RF_PROPERTY_STARTTIME "StartTime"
#define RF_PROPERTY_ENDTIME "EndTime"
#define RF_PROPERTY_COMP_TYPE "ComponentType"
#define RF_PROPERTY_METRIC_TYPE "MetricType"
#define RF_PROPERTY_ACQUISITION_ITEMS "AcquisitionItems"


#define RFPROP_DATA_SRC_ALLOW_VALUES "DataSource@Redfish.AllowableValues"

#define RF_DA_ACTION_EXPORT_DATA \
    "/redfish/v1/DataAcquisitionService/Actions/HwDataAcquisitionService.ExportAcquisitionData"
#define RF_DA_ACTION_CLEAR_DATA "/redfish/v1/DataAcquisitionService/Actions/HwDataAcquisitionService.ClearHistoryData"
#define RF_DA_EXPORT_DATA_ACTIONINFO "/redfish/v1/DataAcquisitionService/ExportAcquisitionDataActionInfo"
#define RF_DA_CLEAR_DATA_ACTIONINFO "/redfish/v1/DataAcquisitionService/ClearHistoryDataActionInfo"

#define RFACTION_PARAM_DATASOURCE "DataSource"
#define RF_EXPORT_CSV_TASK_DESC "Export Task"
#define RF_CLEAR_TABLE_TASK_DESC "Clear Task"

#define DA_SERVICE_EXPORT_ERROR "unknown error"





#define PROPERTY_DA_REPORT_DATA "Data"
#define DA_REPORT_COMPONENT_CPU "cpu"
#define DA_REPORT_COMPONENT_MEM "memory"
#define DA_REPORT_COMPONENT_DISK "disk"
#define DA_REPORT_COMPONENT_MAIN_BOARD "mainboard"
#define DA_REPORT_COMPONENT_PSU "psu"
#define DA_REPORT_COMPONENT_CHASSIS "chassis"
#define DA_REPORT_COMPONENT_SYSTEM "system"
#define DA_REPORT_COMPONENT_FAN "fan"

#define DA_REPORT_METRIC_TYPE_TEMP "temperature"
#define DA_REPORT_METRIC_TYPE_VOL "voltage"
#define DA_REPORT_METRIC_TYPE_CURRENT "current"
#define DA_REPORT_METRIC_TYPE_PERFORMANCE "performance"
#define DA_REPORT_METRIC_TYPE_HEALTH "health"
#define DA_REPORT_METRIC_TYPE_LOG "log"
#define DA_REPORT_METRIC_TYPE_SMART "smart"

#define RF_DA_ACQ_REPORT "DataAcquisitionReport"
#define RF_DA_ACQ_REPORT_URI "/redfish/v1/DataAcquisitionService/DataAcquisitionReport"
#define RFPROP_VAL_DA_REPORT_ID "DataAcquisitionReport"
#define RFPROP_VAL_DA_REPORT_NAME "Data Acquisition Report"

typedef struct comp_type_map {
    const gchar *type_str;
    gint32 type_num;
} COMP_TYPE_MAP;



#define RACK_MGNT_SVC_PATTERN "^/redfish/v1/RackManagementService$"
#define RACK_MGNT_SVC_RSC_PATH "/redfish/v1/rackmanagementservice"

#define RACK_MGNT_SESSION_AGENT_PATTERN "^/redfish/v1/RackManagementService/SessionAgent$"
#define RACK_MGNT_SESSION_AGENT_RSC_PATH "/redfish/v1/rackmanagementservice/sessionagent"

#define RACK_MGNT_SET_DEVICE_TOKEN_ACTION_INFO \
    "^/redfish/v1/RackManagementService/SessionAgent/SetDeviceTokenActionInfo$"
#define RACK_MGNT_SET_DEVICE_TOKEN_ACTION_TEMPLATE "/redfish/v1/rackmanagementservice/sessionagent/setdevtokenactinfo"



#define RACK_MGNT_NETWORK_AGENT_PATTERN "^/redfish/v1/RackManagementService/NetworkAgent$"
#define RACK_MGNT_NETWORK_AGENT_RSC_PATH "/redfish/v1/rackmanagementservice/networkagent"

#define RACK_MGNT_SET_DEVICE_IPINFO_ACTION_INFO \
    "^/redfish/v1/RackManagementService/NetworkAgent/SetDeviceIPInfoActionInfo$"
#define RACK_MGNT_SET_DEVICE_IPINFO_ACTION_TEMPLATE "/redfish/v1/rackmanagementservice/networkagent/setdevipinfoactinfo"





#define RACK_DA_CONFIG_PATTERN "^/redfish/v1/DataAcquisitionService/RackDataAcquisitionConfiguration$"
#define RACK_DA_CONFIG_RSC_PATH "/redfish/v1/dataacquisitionservice/rackdataacquisitionconfiguration"

#define URI_FORMAT_RACK_DA_CONFIG "/redfish/v1/DataAcquisitionService/RackDataAcquisitionConfiguration"
#define PROP_RACK_DA_CONFIG "RackDataAcquisitionConfiguration" 



#define SMS_BOB_SERVICE_STR "BobService"
#define SMS_MAPPING_TABLE_STR "MappingTable"
#define SMS_MAX_PORT "MaxPort"
#define SMS_MIN_PORT "MinPort"
#define SMS_ALARM_STATE "AlarmState"
#define SMS_ALARM_DESC "AlarmDesc"
#define SMS_SERVER_ADDRESS "ServerAddress"
#define SMS_SERVER_PORT "ServerPort"
#define SMS_VETH_PORT "VethPort"
#define SMS_TRAP_ENABLE "Enabled"
#define SMS_BOB_ENABLE "BobEnabled"
#define SMS_BOB_ID "Id"
#define SMS_CDEV_CHANNEL_ENABLED "CdevChannelEnabled"
#define SMS_CHANNEL_TYPE "ChannelType"


#define SMS_MAPKEYCOUNT_MAX 16
#define SMS_STRLEN_128 128
#define SMS_ALARM_ON 1
#define SMS_ALARM_OFF 0
#define SMS_STRLEN_256 256
#define SNMP_TRAP_ITEM_NUMBER 4
#define SNMP_BMA_VETH_PORT_NUMBER 16





#define SNMP_VERSION_V1_STR "SnmpV1Enabled"
#define SNMP_VERSION_V2C_STR "SnmpV2CEnabled"
#define SNMP_VERSION_V3_STR "SnmpV3Enabled"
#define RFPROP_SNMPV1V2C_LOGIN_RULE "SnmpV1V2CLoginRule"
#define RFPROP_VAL_RULE1 "Rule1"
#define RFPROP_VAL_RULE2 "Rule2"
#define RFPROP_VAL_RULE3 "Rule3"
#define SNMP_ACTIONSNAME_STR "Actions"
#define SNMP_LINKSNAME_STR "Links"
#define SNMP_LONGPASSWD_STR "LongPasswordEnabled"

#define SNMP_RW_COMMUNITY_ENABLED_STR "RWCommunityEnabled"

#define SNMP_ROCOMMUNITY_STR "ReadOnlyCommunity"
#define SNMP_RWCOMMUNITY_STR "ReadWriteCommunity"
#define SNMP_V3AUTH_STR "SnmpV3AuthProtocol"
#define SNMP_V3PRIV_STR "SnmpV3PrivProtocol"
#define SNMP_V3AUTH_USER "SnmpV3AuthUser"
#define SNMP_V3ENGINE_ID "SnmpV3EngineID"

#define SNMP_SYSTEM_CONTACT_NAME "SystemContact"


#define SNMP_SYSTEM_LOCATION_NAME "SystemLocation"

#define SNMP_ACTIONS_STR "SnmpService.SubmitTestEvent"
#define SNMP_ACTIONS_SNMPPASSWD_STR "SnmpService.ConfigSnmpV3PrivPasswd"
#define SNMP_V3PRIVPASSWD "SnmpV3PrivPasswd"
#define SNMP_TRAPITEM_ID "MemberId"
#define SNMP_TRAPITEM_IP "TrapServerAddress"
#define SNMP_BOBTRAPITEM_ENABLE "BobEnabled"
#define SNMP_TRAPITEM_PORT "TrapServerPort"
#define SNMP_TRAPITEM_ENABLE "Enabled"
#define SNMP_TRAP_ENABLE "ServiceEnabled"
#define SNMP_TRAP_VERSION "TrapVersion"
#define SNMP_TRAP_V3USER "TrapV3User"
#define SNMP_TRAP_MODE "TrapMode"
#define SNMP_TRAP_SERVICE_ID "TrapServerIdentity"
#define SNMP_TRAP_COMMUNITY "CommunityName"
#define SNMP_TRAP_SEVERITY "AlarmSeverity"
#define SNMP_TRAPITEM_INFO "TrapServer"
#define SNMP_TRAP_INFO "SnmpTrapNotification"


#define SNMP_TRAP_MESSAGE_DELIMITER "MessageDelimiter"
#define SNMP_TRAP_MESSAGE_CONTENT "MessageContent"
#define SNMP_TRAP_MESSAGE_TIME_SELECTED "TimeSelected"
#define SNMP_TRAP_MESSAGE_SENSOR_NAME_SELECTED "SensorNameSelected"
#define SNMP_TRAP_MESSAGE_SEVERITY_SELECTED "SeveritySelected"
#define SNMP_TRAP_MESSAGE_EVENT_CODE_SELECTED "EventCodeSelected"
#define SNMP_TRAP_MESSAGE_EVENT_DESCRIPTION_SELECTED "EventDescriptionSelected"
#define SNMP_TRAP_MESSAGE_DISPLAY_KEYWORD_ENABLED "MessageDisplayKeywordEnabled"



#define SNMP_PROPERTY_V1STATE "V1State"
#define SNMP_PROPERTY_V2CSTATE "V2CState"
#define SNMP_PROPERTY_V3STATE "V3State"

#define SNMP_TRAPVER_V1 "V1"
#define SNMP_TRAPVER_V2C "V2C"
#define SNMP_TRAPVER_V3 "V3"

#define SNMP_VERSION_V1             0
#define SNMP_VERSION_V2C            1
#define SNMP_VERSION_V3             2

#define SNMP_AUTHPRO_MD5            "MD5"
#define SNMP_AUTHPRO_SHA            "SHA"
#define SNMP_AUTHPRO_SHA1           "SHA1"
#define SNMP_AUTHPRO_SHA256         "SHA256"
#define SNMP_AUTHPRO_SHA384         "SHA384"
#define SNMP_AUTHPRO_SHA512         "SHA512"

#define SNMP_PRIVPRO_DES "DES"
#define SNMP_PRIVPRO_AES "AES"
#define SNMP_PRIVPRO_AES256 "AES256"
#define SNMP_PRIVPRO_VAL_DES 1
#define SNMP_PRIVPRO_VAL_AES 2
#define SNMP_PRIVPRO_VAL_AES256 3

#define SNMP_TRAPMODE_EVENT "EventCode"
#define SNMP_TRAPMODE_OID "OID"
#define SNMP_TRAPMODE_ALARM "PreciseAlarm"

#define SNMP_TRAP_CRITICAL "Critical"
#define SNMP_TRAP_MAJOR "Major"
#define SNMP_TRAP_MINOR "Minor"
#define SNMP_TRAP_NORMAL "Normal"

#define TRAP_CRITICAL 8
#define TRAP_MAJOR 12
#define TRAP_MINOR 14
#define TRAP_NORMAL 15

#define SNMP_STRLEN_32 32
#define SNMP_STRLEN_64 64
#define SNMP_STRLEN_128 128
#define SNMP_STRLEN_256 256
#define SNMP_CHAR_MIN 0
#define SNMP_CHAR_MAX 255
#define SNMP_ITEMCOUNT_MAX 4
#define SNMP_IPADDRESS_LEN_MAX 50
#define SNMP_TRAP_ITEM_ENABLE 1
#define SNMP_TRAP_ITEM_DISABLE 0
#define BOB_ENABLE 1
#define BOB_DISABLE 0

#define SNMP_URI_CONTEXT "/redfish/v1/$metadata#Managers/Members/%s/SnmpService/$entity"
#define SNMP_ACTION_TARGETURI "/redfish/v1/Managers/%s/SnmpService/Actions/SnmpService.SubmitTestEvent"
#define SNMP_ACTION_TARGET_CONFIG_PRIPASSWD_URI "SnmpService.ConfigSnmpV3PrivPasswd"
#define SNMP_URI_ODATAID "/redfish/v1/Managers/%s/SnmpService"
#define SNMP_LINKS_URI "/redfish/v1/Managers/%s#/Oem/Huawei/LoginRule/%d"
#define SNMP_ACTION_INFOURI "/redfish/v1/Managers/%s/SnmpService/SubmitTestEventActionInfo"
#define SNMP_ACTION_INFO_CONFIG_PRIPASSWD_URI "/redfish/v1/Managers/%s/SnmpService/ConfigSnmpV3PrivPasswdActionInfo"

#define SNMP_ACTION_NAME "#SnmpService.SubmitTestEvent"
#define SNMP_ACTION_INFONAME "@Redfish.ActionInfo"
#define SNMP_ACTION_TARGET "target"
#define SNMP_LINKS_RULES "LoginRule"

#define SNMP_PARAMETER_ERR (-5002)
#define SNMP_OUT_OF_RANGE_ERR (-5006)

#define RF_SENSOR_MODULE_FAILED (-5001)
#define RF_SENSOR_MODULE_STATE_ERR (-5010)
#define LLDP_URI_ODATAID "/redfish/v1/Managers/%s/LldpService"
#define ENERGY_SAVING_URI_ODATAID "/redfish/v1/Managers/%s/EnergySavingService"
#define USBMGMT_URI_ODATAID "/redfish/v1/Managers/%s/USBMgmtService"
#define DIGITAL_WARRANTY_ODATAID "/redfish/v1/Systems/%s/DigitalWarranty"

#define USBMGMT_URI_ODATAID "/redfish/v1/Managers/%s/USBMgmtService"



#define MANAGR_VMM_CONTEXT "/redfish/v1/$metadata#Managers/Members/%s/VirtualMedia/Members/$entity"
#define MANAGER_VMM_ODATAID "/redfish/v1/Managers/%s/VirtualMedia/CD"
#define VMM_PROPERTY_CONNECT_URL "VmmConnectUrl"
#define MANAGER_PROPERTY_VMM_IMAGENAME "ImageName"
#define MANAGER_PROPERTY_VMM_MEDIATYPE "MediaTypes"
#define MANAGER_PROPERTY_VMM_IMAGE "Image"
#define MANAGER_PROPERTY_VMM_CONNECTVIA "ConnectedVia"
#define MANAGER_PROPERTY_VMM_INSERTED "Inserted"
#define MANAGER_CONNECTVIA_VMM_URI "URI"
#define MANAGER_CONNECTVIA_VMM_APPLET "Applet"
#define MANAGER_CONNECTVIA_VMM_NOTCONN "NotConnected"
#define MANAGER_PROPERTY_ACTIONS "Actions"
#define MANAGER_PROPERTY_VMMTARGET "target"
#define MANAGER_PROPERTY_VMMCONTROL "#VirtualMedia.VmmControl"
#define MANAGER_VMMCONTROL_URI "/redfish/v1/Managers/%s/VirtualMedia/CD/Oem/Huawei/Actions/VirtualMedia.VmmControl"
#define MANAGER_ACTIONINFO_URI "/redfish/v1/Managers/%s/VirtualMedia/CD/VmmControlActionInfo"
#define MANAGER_PROPERTY_ACTIONINFO "@Redfish.ActionInfo"
#define MANAGER_PROPERTY_VMMCOLTYPE "VmmControlType"
#define MANAGER_PROPERTY_VMMCONNECT "Connect"

#define MANAGER_PROPERTY_VMMDISCONNECT "Disconnect"

#define MANAGER_PROPERTY_VMVMMCONTROL "VirtualMedia.VmmControl"
#define VMM_MESSAGE_WRONGFILE_FORMAT "WrongFileFormat"   // 文件格式错误
#define VMM_MESSAGE_CONNECTOCCUPIED "ConnectionOccupied" // 连接占用
#define VMM_MESSAGE_DIS_FAILED "Disconnectionfailed"     // 断开失败
#define VMM_MESSAGE_CONNECT_FAILED "ConnectionFailed"
#define MESSAGE_OPERATIONTIMEOUT "OperationTimeout"
#define VMM_MESSAGE_OPERATIONINPROCESS "OperationInProcess"
#define VMM_MESSAGE_VMM_MOUNTED "VirtualMediaMounted"
#define VMM_MESSAGE_VMM_DISMOUNTED "VirtualMediaDismounted"
#define VMM_MESSAGE_INVALD_URI "InvalidImageURI"
#define VMM_MANAGER_MEDIATYPE "CD"
#define VMM_MANAGER_IMAGE_LEN 255
#define VMM_MANAGER_ISO ".iso"
#define VMM_MANAGER_SEPARATOR "/"
#define VMM_MANAGER_DISCONNECT_TASK "vmm disconnect  task"
#define VMM_MANAGER_DISCONNECT_STAUS_TASK "vmm disconnect status task"
#define VMM_MANAGER_CONNECT_TASK "vmm connect task"
#define MANAGER_VIRTUALMEDIA "VirtualMedia"
#define PROPERTY_MANAGER_VMMENCRYPTION "EncryptionEnabled"
#define PROPERTY_MANAGER_FLOPPYDRIVE "FloppyDriveEnabled"

#define VMM_ENCRYPTION_TRUE 1
#define VMM_ENCRYPTION_FALSE 0
#define VMM_FLOPPY_ENABLE_TRUE 1
#define VMM_FLOPPY_ENABLE_FALSE 0

#define USB_CREATE_EXIST (-21)
#define USB_CREATE_ERR (-17)
#define USB_CLOSE_NONE_EXIST (-38)
#define USB_OPERATIONINPROCESS 9




#define URI_FORMAT_MANAGER_USB_ODATAID "/redfish/v1/Managers/%s/VirtualMedia/USBStick"
#define URI_FORMAT_MANAGR_USB_CONTEXT "/redfish/v1/$metadata#VirtualMedia.VirtualMedia"
#define PROPERTY_USBSTICK_CONTROL "#VirtualMedia.USBStickControl"
#define PROPERTY_USBSTICK_CONTROL_ACT "VirtualMedia.USBStickControl"

#define MANAGER_CONNECTVIA_VMM_OEM "Oem"
#define MANAGER_VMM_MEDIATYPE_USBSTICK "USBStick"
#define MANAGER_VMM_USBSTICK_REMOTEPATH "RemotePath"
#define MANAGER_USBSTICK_CONNECTING_TASK "usbstick connect task"
#define MANAGER_USBSTICK_CONNECTING_CHECK_TASK "usbstick connect status check task"
#define MANAGER_USBSTICK_DISCONNECTING_TASK "usbstick disconnect task"
#define MANAGER_USBSTICK_DISCONNECTING_CHECK_TASK "usbstick disconnect status check task"
#define MANAGER_PROPERTY_USBSTICKCOLTYPE "USBStickControlType"
#define USBSTICK_CONNECTED 1
#define USBSTICK_DISCONNECTED 0
#define MANAGER_VMM_USBSTICK_TYPE "ConnectType"



#define URI_FORMAT_MANAGER_USB_IBMA_ODATAID "/redfish/v1/Managers/%s/VirtualMedia/iBMAUSBStick"
#define URI_FORMAT_MANAGER_USBCONTROL_IBMA_URI \
    "/redfish/v1/Managers/%s/VirtualMedia/iBMAUSBStick/Oem/Huawei/Actions/VirtualMedia.USBStickControl"
#define URI_FORMAT_MANAGER_USBACTIONINFO_IBMA_URI \
    "/redfish/v1/Managers/%s/VirtualMedia/iBMAUSBStick/USBStickControlActionInfo"
#define PROPERTY_USBSTICK_IBMA_PACKET_INFO "iBMAPacketInformation"
#define PROPERTY_USBSTICK_IBMA_STATUS "iBMAPacketStatus"
#define MANAGER_USBSTICK_IBMA_OSTYPE "OSType"
#define MANAGER_USBSTICK_IBMA_VERSION "Version"




#define URI_FORMAT_NTPACTIONS_ODATAID "/redfish/v1/Managers/%s/NtpService/ImportNtpKeyActionInfo"
#define URI_FORMAT_NTPSERVICE_CONTEXT "/redfish/v1/$metadata#Managers/Members/%s/NtpService/$entity"
#define URI_FORMAT_NTPSERVICE_ODATAID "/redfish/v1/Managers/%s/NtpService"

#define PROPERTY_PREFERRED_NTP_ENABLE "ServiceEnabled"
#define PROPERTY_PREFERRED_NTP_SERVER "PreferredNtpServer"
#define PROPERTY_ALTERNATE_NTP_SERVER "AlternateNtpServer"
#define PROPERTY_EXTRA_NTP_SERVER "ExtraNtpServer"

#define PROPERTY_OPERATING_MODE "NtpAddressOrigin"


#define PROPERTY_NTP_MIN_POLLTNTERVAL "MinPollingInterval"
#define PROPERTY_NTP_MAX_POLLTNTERVAL "MaxPollingInterval"


#define PROPERTY_NTP_KEY_STATUS "NTPKeyStatus"
#define NTP_KEY_UPLOADED "Uploaded"
#define NTP_KEY_NOT_UPLOADED "NotUploaded"


#define PROPERTY_SERVER_AUTHENTICATION "ServerAuthenticationEnabled"

#define MANAGER_PREE_NTP_LEN MAX_EXTRA_NTP_SERVER_IP_LEN
#define MANAGER_NTPSERVICE_MODE_MANUAL "Static"
#define MANAGER_NTPSERVICE_MODE_DHCPV4 "IPv4"
#define MANAGER_NTPSERVICE_MODE_DHCPV6 "IPv6"
#define MANAGER_NTPSERVICE_NTPKEY "#NtpService.ImportNtpKey"
#define MANAGER_NTPIMPORT_KEY "/redfish/v1/Managers/%s/NtpService/Actions/NtpService.ImportNtpKey"
#define MANAGER_NTPACTION_INFO "/redfish/v1/Managers/%s/NtpService/ImportNtpKeyActionInfo"
#define NTPSERVICE_ACTIONS "NtpService.ImportNtpKey"
#define NTPSERVICE_FILE_PATH "/tmp/ntp.keys"
#define NTPSERVICE_TMP "/tmp%s"
#define NTP_MESSAGE_INVALID_URI "InvalidImageURI"

#define NTP_FILE_NAME_LEN 1024

#define NTP_KEY_FILE_ID 0x18
#define NTP_MANAGER_TYPE_TEXT "text"
#define MANAGER_NTP_MANUAL 1
#define MANAGER_NTP_DHCPV4 2
#define MANAGER_NTP_DHCPV6 3
#define KEY_TOO_BIG (-2)
#define KEY_FILE_NOT_SUPPORTED (-3)
#define MANAGER_VMMACTIONS_ODATAID "/redfish/v1/Managers/%s/VirtualMedia/CD/VmmControlActionInfo"

#define MAMAGER_NTP_MANUAL 1
#define MAMAGER_NTP_DHCPV4 2
#define MAMAGER_NTP_DHCPV6 3



#define PROPERTY_MANAGER_KVMNUM_SESSION "NumberOfActivatedSessions"
#define PROPERTY_MANAGER_ACTIVED_SESSION_TYPE "ActivatedSessionsType"
#define PROPERTY_MANAGER_VISIBLE_JAVAKVM             "VisibleJavaKvm"

#define PROPERTY_MANAGER_KVMTIMOUT "SessionTimeoutMinutes"

#define PROPERTY_MANAGER_KVMENCRYPTIONCONFIGURABLE "EncryptionConfigurable"
#define PROPERTY_MANAGER_KVMENCRYPTION "EncryptionEnabled"
#define PROPERTY_MANAGER_KVMMOUSE "PersistentUSBConnectionEnabled"
#define PROPERTY_MANAGER_AUTO_OS_LOCK_STATE "AutoOSLockEnabled"
#define PROPERTY_MANAGER_AUTO_OS_LOCK_TYPE "AutoOSLockType"
#define PROPERTY_MANAGER_AUTO_OS_LOCK_KEY "AutoOSLockKey"

#define SET_KVM_KEY_ACTION_NAME "KvmService.SetKvmKey"
#define EXPORT_KVM_JNLP_ACTION_NAME "KvmService.ExportKvmStartupFile"
#define RFPROP_MANAGER_KVMKEY_IDEXT "IdExt"
#define RFPROP_MANAGER_KVMKEY_SECRETKEY "SecretKey"
#define RFPROP_MANAGER_KVMKEY_MODE "Mode"
#define RFPROP_MANAGER_KVMKEY_MODE_SHARED 0
#define RFPROP_MANAGER_KVMKEY_MODE_PRIVATE 1
#define RFPROP_MANAGER_AUTO_OS_LOCK_ENABLED "AutoOSLockEnabled"
#define RFPROP_MANAGER_AUTO_OS_LOCK_TYPE "AutoOSLockType"
#define RFPROP_MANAGER_AUTO_OS_LOCK_KEY "AutoOSLockKey"
#define KVM_ACTION_INFO_URI "/redfish/v1/Managers/%s/KvmService/SetKvmKeyActionInfo"
#define KVM_ACTION_TARGET_URI "/redfish/v1/Managers/%s/KvmService/Actions/KvmService.SetKvmKey"
#define KVM_ACTION_PROP_NAME "#KvmService.SetKvmKey"
#define MANAGER_KVMSERVICE_CONTEXT "/redfish/v1/$metadata#Managers/Members/%s/KvmService/$entity"
#define MANAGER_KVMSERVICE_ODATAID "/redfish/v1/Managers/%s/KvmService"
#define VMM_MANAGER_ENCRYPTION "EncryptionEnabled"
#define VMM_MANAGER_ENCRYPTIONCONFIGURABLE "EncryptionConfigurable"
#define MANAGER_KVM_IS_IN_USE 0xff
#define MANAGER_VMM_ENABLE_IN_USE 0xfe
#define MANAGER_VMM_DISABLE_IN_USE 0xfd
#define KVM_ENCRYPTION_TRUE 1
#define KVM_ACTION_KVMJNLP_TARGET_RUI "/redfish/v1/Managers/%s/KvmService/Actions/KvmService.ExportKvmStartupFile"
#define KVM_ACTION_KVMJNLP_INFO_URI "/redfish/v1/Managers/%s/KvmService/ExportKvmStartupFileActionInfo"
#define KVM_ACTION_KVMJNLP_NAME "#KvmService.ExportKvmStartupFile"
#define KVM_FIND_JARNAME_PATH "/opt/pme/web/htdocs/bmc/pages/jar/"
#define KVM_STARTUP_FILE "kvm startup file"
#define KVM_ACTION_SETKVMKEY_INFO_URI_SUFFIX "setkvmkeyactioninfo"
#define KVM_ACTION_KVMJNLP_INFO_URI_SUFFIX "exportkvmstartupfileactioninfo"



#define MANAGER_DIAGNOSTIC_ODATAID "/redfish/v1/Managers/%s/DiagnosticService"
#define MANAGER_DIAGNOSTIC_CONTEXT "/redfish/v1/$metadata#Managers/Members/%s/DiagnosticService/$entity"
#define PROPERTY_VIDEO_PLAYBACK_CONN_NUM "VideoPlaybackConnNum"
#define PROPERTY_VIDEO_RECORDING_ENABLED "VideoRecordingEnabled"
#define PROPERTY_SCREENSHOT_ENABLED "ScreenShotEnabled"
#define PROPERTY_PCIEINTERFACE_ENABLED "PCIeInterfaceEnabled"
#define PROPERTY_BLACKBOXDUMP_ENABLED "BlackBoxDumpEnabled"
#define PROPERTY_BLACKBOX_ENABLED "BlackBoxEnabled"
#define PROPERTY_SERIALPORTDATA_ENABLED "SerialPortDataEnabled"
#define PROPERTY_DFP_ENABLED "DfpServiceEnabled"
#define PROPERTY_VIDEO_RECORD_INFO "VideoRecordInfo"
#define PROPERTY_SCREENSHOT_CREATE_TIME "ScreenShotCreateTime"
#define RFACTION_STOP_VIDEO_PLAYBACK "DiagnosticService.StopVideoPlayback"
#define RFACTION_EXPORT_VIDEO "DiagnosticService.ExportVideo"
#define RFACTION_EXPORT_VIDEO_PLAYBACK_STARTUP_FILE "DiagnosticService.ExportVideoPlaybackStartupFile"
#define PROPERTY_LOCAL "LocalVideo"
#define PROPERTY_OSRESET "OsresetVideo"
#define PROPERTY_CATERROR "CaterrorVideo"
#define PROPERTY_POWEROFF "PoweroffVideo"
#define PROPERTY_VIDEO_TYPE "VideoType"
#define PROPERTY_HOSTNAME "HostName"
#define PROPERTY_SERVERNAME "ServerName"
#define PROPERTY_LANGUAGE "Language"

#define PROPERTY_VIDEO_SIZE_BYTE "VideoSizeByte"
#define PROPERTY_CREATE_TIME "CreateTime"


#define PROPERTY_BLACKBOX_SIZE "BlackBoxSize"
#define PROPERTY_SERIALPOARTDATA_SIZE "SerialPortDataSize"

#define RFACTION_CAPTURE_SCREEN_SHOT "DiagnosticService.CaptureScreenShot"
#define RFACTION_DELETE_SCREEN_SHOT "DiagnosticService.DeleteScreenShot"
#define RFACTION_EXPORT_BLACK_BOX "DiagnosticService.ExportBlackBox"
#define RFACTION_EXPORT_SERIAL_PORT_DATA "DiagnosticService.ExportSerialPortData"
#define RFACTION_EXPORT_NPU_LOG "DiagnosticService.ExportNPULog"
#define DIAGNOSTIC_WORKRECORD_ODATAID "/redfish/v1/Managers/%s/DiagnosticService/WorkRecord"
#define DIAGNOSTIC_WORKRECORD_CONTEXT "/redfish/v1/$metadata#Managers/Members/%s/DiagnosticService/WorkRecord/$entity"
#define DIAGNOSTIC_WORKRECORD "WorkRecord"
#define DIAGNOSTIC_RECORDS "Records"
#define DIAGNOSTIC_NEXT_AVAILABLE_ID "NextAvailableId"
#define RFACTION_ADD_RECORD "WorkRecord.AddRecord"
#define RFACTION_DELETE_RECORD "WorkRecord.DeleteRecord"
#define RFACTION_MODIFY_RECORD "WorkRecord.ModifyRecord"
#define MAX_WORKRECORD_NUM 20

#define DIAGNOSTIC_OPERATELOG_ODATAID "/redfish/v1/Managers/%s/DiagnosticService/OperateLog"
#define DIAGNOSTIC_OPERATELOG_CONTEXT "/redfish/v1/$metadata#Managers/Members/%s/DiagnosticService/OperateLog/$entity"
#define DIAGNOSTIC_OPERATELOG_URI "/redfish/v1/Managers/%s/DiagnosticService/OperateLog/%d"
#define RFACTION_OPERATELOG_EXPORTLOG "OperateLog.ExportLog"
#define DIAGNOSTIC_RUNLOG_ODATAID "/redfish/v1/Managers/%s/DiagnosticService/RunLog"
#define DIAGNOSTIC_RUNLOG_CONTEXT "/redfish/v1/$metadata#Managers/Members/%s/DiagnosticService/RunLog/$entity"
#define DIAGNOSTIC_RUNLOG_URI "/redfish/v1/Managers/%s/DiagnosticService/RunLog/%d"
#define RFACTION_RUNLOG_EXPORTLOG "RunLog.ExportLog"
#define DIAGNOSTIC_SECURITYLOG_ODATAID "/redfish/v1/Managers/%s/DiagnosticService/SecurityLog"
#define DIAGNOSTIC_SECURITYLOG_CONTEXT "/redfish/v1/$metadata#Managers/Members/%s/DiagnosticService/SecurityLog/$entity"
#define DIAGNOSTIC_SECURITYLOG_URI "/redfish/v1/Managers/%s/DiagnosticService/SecurityLog/%d"
#define RFACTION_SECURITYLOG_EXPORTLOG "SecurityLog.ExportLog"
#define PROPERTY_LOG_TOTAL_ITEMS "TotalItems"
#define PROPERTY_LOG_ITEMSPERPAGE "ItemsPerPage"
#define PROPERTY_LOG_ITEMS "LogItems"
#define PROPERTY_LOG_ID "Id"
#define PROPERTY_LOG_TIME "Time"
#define PROPERTY_LOG_LEVEL "Level"
#define PROPERTY_LOG_HOST "Host"
#define PROPERTY_LOG_INTERFACE "Interface"
#define PROPERTY_LOG_USER "User"
#define PROPERTY_LOG_ADDR "Address"
#define PROPERTY_LOG_DETAILS "Details"
#define LOG_TYPE_NAME_OPERATE "Operate Log"
#define LOG_TYPE_NAME_SECURITY "Security Log"
#define LOG_TYPE_NAME_RUN "Run Log"


#define RE_AUTHENTICATION_ADD_USER "add user"
#define RE_AUTHENTICATION_DELETE_USER "delete user"
#define RE_AUTHENTICATION_SET_USER "set user information"
#define RE_AUTHENTICATION_SET_LDAP "set ldap controller"
#define RE_AUTHENTICATION_SET_KERBEROS "set kerberos controller"
#define RE_AUTHENTICATION_IMPORT_KERBEROS_KEY "import kerberos key table"
#define RE_AUTHENTICATION_SET_CUSTOMIZED_ROLE "set customized user role"
#define RE_AUTHENTICATION_IMPORT_SSH_PUBLIC_KEY "import ssh public key"
#define RE_AUTHENTICATION_DELETE_SSH_PUBLIC_KEY "delete ssh public key"
#define RE_AUTHENTICATION_SET_VNC_PASSWORD "set vnc password"
#define RE_AUTHENTICATION_SET_SNMPV3_PASSWORD "set user snmpv3 password"
#define RE_AUTHENTICATION_ADD_NODE_USER "add node user"
#define RE_AUTHENTICATION_SET_NODE_USER "set node user"
#define RE_AUTHENTICATION_DELETE_NODE_USER "delete node user"
#define RE_AUTHENTICATION_IMPORT_LDAP_CERTIFICATE "import ldap controller certificate"

#define RE_AUTHENTICATION_IMPORT_LDAP_CRL "import ldap controller crl"
#define RE_AUTHENTICATION_DELETE_LDAP_CRL "delete ldap controller crl"



#define PORTTYPE_PHYSICAL 1


#define LINK_DOWN 0
#define LINK_UP 1

#define TEAM_TYPE 7
#define BRIDGE_TYPE 8


#define NETWORK_TYPE_NET "NetCard"
#define NETWORK_TYPE_FC "FCCard"
#define NETWORK_TYPE_TEAM "Team"
#define NETWORK_TYPE_BRIDGE "Bridge"


#define RFPROP_PCIEDS "PCIeDevices"
#define RFPROP_NETWORKPORT "NetworkPort"
#define RFPROP_ODATA_ADAPTER "NetworkAdapter"
#define RF_CHASSIS_ADAPTERS "NetworkAdapters"
#define RFPROP_NETWORKINTERFACE "NetworkInterface"
#define RFPROP_NETWORKPORTS "NetworkPorts"
#define RFPROP_ETHERNET "Ethernet"

#define RFPROP_FIBRE_CHANNEL "FibreChannel"
#define RFPROP_ISCSI "iSCSI"
#define RFPROP_FCOE "FCoE"
#define RFPROP_OPA "OPA"
#define RFPROP_IB "IB"



#define RFPROP_INFINI_BAND "InfiniBand"


#define RFPROP_LINKS "Links"
#define RFPROP_PPA "PhysicalPortAssignment"
#define CHASSIS_NETWORK_ADAPTER_DRIVER_NAME "DriverName"
#define CHASSIS_NETWORK_ADAPTER_DRIVER_VERSION "DriverVersion"
#define CHASSIS_NETWORK_ADAPTER_MANU "Manufacturer"
#define CHASSIS_NETWORK_ADAPTER_DEVICE_LOCATOR "DeviceLocator"
#define CHASSIS_NETWORK_ADAPTER_POSITION "Position"
#define CHASSIS_NETWORK_ADAPTER_FIRM_VERSION "FirmwarePackageVersion"
#define CHASSIS_NETWORK_ADAPTER_PORTCOUNT "NetworkPortCount"
#define CHASSIS_NETWORK_ADAPTER_CONTROLLER_CAPA "ControllerCapabilities"
#define CHASSIS_NETWORK_ADAPTER_CONTROLLER "Controllers"
#define CHASSIS_NETWORK_ADAPTER_SLOT_NUM "SlotNumber"
#define CHASSIS_NETWORK_ADAPTER_NAME "Name"
#define CHASSIS_NETWORK_ADAPTER_SLOTID "PcieSlotId"
#define CHASSIS_NETWORK_ADAPTER_CARDMANU "CardManufacturer"
#define CHASSIS_NETWORK_ADAPTER_CARDMODEL "CardModel"
#define CHASSIS_NETWORK_ADAPTER_MODEL "Model"

#define CHASSIS_NETWORK_ADAPTER_NETWORK_TECHNOLOGY "NetworkTechnology"


#define CHASSIS_NETWORK_ADAPTER_ROOT_BDF "RootBDF"

#define CHASSIS_NETWORK_ADAPTER_FIRMWARE_VERSION "FirmwareVersion"
#define RFPROP_PORT_NUMBER "PhysicalPortNumber"
#define RFPROP_PORT_LINKSTATUS "LinkStatus"
#define RFPROP_PORT_ADDRESSES "AssociatedNetworkAddresses"


#define RFPROP_PORT_ACTIVE_LINK_TECHNOLOGY "ActiveLinkTechnology"


#define RFPROP_PORT_BDF "BDF"

#define RFPROP_PORT_LLDP_SERVICE                    "LldpService"
#define RFPROP_PORT_LLDP_ENABLE                     "LldpEnabled"
#define RFPROP_PORT_LLDP_WORKMODE                   "WorkMode"
#define RFPROP_PORT_LLDP_TX_WORKMODE                "Tx"

#define RFPROP_PORT_FIRM_VERSION "FirmwarePackageVersion"
#define RFPROP_PORT_DRIVER_NAME "DriverName"
#define RFPROP_PORT_DRIVER_VERSION "DriverVersion"
#define RFPROP_PORT_TECHNOLOGY "PortTechnology"
#define RFPROP_PORT_FC_PORT_STATE "PortState"
#define RFPROP_PORT_FC_ID "FCId"
#define RFPROP_PORT_SPEED "LinkSpeedGbps"

#define RFPROP_NETCARD_VIRTUAL_ETHERNET_INTERFACES "VirtualEthernetInterfaces"
#define RFPROP_NETCARD_MCTP_SUPPORT "MctpEnabled"

#define RFPROP_NETCARD_HOTPLUG_ENABLE "HotPlugSupported"
#define RFPROP_NETCARD_HOTPLUG_STATE "HotPlugAttention"
#define RFPROP_NETCARD_HOTPLUG_CONTROL "OrderlyHotplugCtrlStatus"

#define CHASSIS_FUNC_MACA "MACAddress"

#define CHASSIS_NETWORK_PORT_TYPE "PortType"
#define CHASSIS_NETWORK_PORT_SPEED "PortMaxSpeed"
#define RFVALUE_OPTICALPORT "OpticalPort"
#define RFVALUE_ELETICALPORT "ElectricalPort"
#define PORT_TYPE_OPTICALPORT 1
#define PORT_TYPE_ELETICALPORT 0

#define SYSTEMS_NETWORK_DEVICEFUNC_PHYPORTASS "PhysicalPortAssignment"
#define SYSTEMS_NETWORK_PORTS_ODATA_COUNT "NetworkPorts@odata.count"
#define CHASSIS_NETWORK_PCIEDS_ODATA_COUNT "PCIeDevices@odata.count"
#define CHASSIS_PCIEDS_MEMBERID "/redfish/v1/Chassis/%s/PCIeDevices/%s"

#define CHASSIS_NETWORK_ADAPTERS "/redfish/v1/Chassis/%s/NetworkAdapters"
#define CHASSIS_NETWORK_ADAPTERS_MEMBER_ID "/redfish/v1/Chassis/%s/NetworkAdapters/%s"
#define SYSTEMS_NETWORK_INTERFACES "/redfish/v1/Systems/%s/NetworkInterfaces/%s"
#define SYSTEMS_NETWORK_ADAPTERS_NETWORKPORTS "/redfish/v1/Chassis/%s/NetworkAdapters/%s/NetworkPorts"
#define CHASSIS_NETWORK_ADAPTERS_PORTS_MEMBERID "/redfish/v1/Chassis/%s/NetworkAdapters/%s/NetworkPorts/%d"
#define CHASSIS_NETWORK_ADAPTERS_PORTS_MEMBERID_S "/redfish/v1/Chassis/%s/NetworkAdapters/%s/NetworkPorts/%s"
#define SYSTEMS_NETWORK_INTERFACES_NETWORKPORTS "/redfish/v1/Systems/%s/NetworkInterfaces/%s/NetworkPorts"
#define URI_FORMAT_CHASSIS_OPTICAL_MODULE "/redfish/v1/Chassis/%s/NetworkAdapters/%s/NetworkPorts/%d/OpticalModule"
#define URI_FORMAT_CHASSIS_OPTICAL_MODULE_S "/redfish/v1/Chassis/%s/NetworkAdapters/%s/NetworkPorts/%s/OpticalModule"

#define URI_FORMAT_CHASSIS_DIRECT_OPTICAL           "/redfish/v1/Chassis/%s/OpticalModule"
#define URI_FORMAT_CHASSIS_DIRECT_OPTICAL_MODULE    "/redfish/v1/Chassis/%s/OpticalModule/%d"
#define CHASSIS_DIRECT_OPTICAL_MODULE_TYPE          "#OpticalModuleCollection.OpticalModuleCollection"
#define CHASSIS_DIRECT_OPTICAL_MODULE_NAME          "OpticalModule Collection"

#define URI_FORMAT_NIC_PRE "/redfish/v1/Chassis/%s/NetworkAdapters"

#define CHASSIS_NETWORK_ADAPTERS_METADATA "/redfish/v1/$metadata#Chassis/Members/%s/NetworkAdapters/$entity"
#define CHASSIS_NETWORK_ADAPTER_METADATA "/redfish/v1/$metadata#Chassis/Members/%s/NetworkAdapters/Members/$entity"
#define SYSTEMS_NETWORK_INTERFACES_METADATA "/redfish/v1/$metadata#Systems/Members/%s/NetworkInterfaces/Members/$entity"
#define SYSTEMS_NETWORK_INTERFACES_PORTS_METADATA \
    "/redfish/v1/$metadata#Systems/Members/%s/NetworkInterfaces/Members/%s/NetworkPorts/$entity"
#define CHASSIS_NETWORK_PORTS_MEMBERID \
    "/redfish/v1/$metadata#Chassis/Members/%s/NetworkAdapters/Members/%s/NetworkPorts/$entity"
#define CHASSIS_NETWORK_PORT_MEMBERID \
    "/redfish/v1/$metadata#Chassis/Members/%s/NetworkAdapters/Members/%s/NetworkPorts/Members/$entity"
#define CHASSIS_OPTICAL_MODULE_METADATA \
    "/redfish/v1/$metadata#Chassis/Members/%s/NetworkAdapters/Members/%s/NetworkPorts/Members/%d/OpticalModul/$entity"
#define CHASSIS_DIRECT_OPTICAL_MODULE_CONTEXT       "/redfish/v1/$metadata#Chassis/Members/%s/OpticalModuleCollection"
#define CHASSIS_DIRECT_OPTICAL_MODULE_METADATA \
    "/redfish/v1/$metadata#Chassis/Members/%s/OpticalModule/Members/%d/$entity"

#define JSONCHEMAS_CONFG_FILE_PREFIX "/data/opt/pme/conf"
#define JSONCHEMAS_CONFG_FILE_SUFFIX "index.json"
#define JSONCHEMAS_PROVIDER_CONFG_FILE "/data/opt/pme/conf/redfish/v1/jsonschemas/%s/index.json"


#define RF_EVT_SCAN_BLACK_LIST_LOGENTRY "(?i)^/redfish/v1/systems/[a-zA-Z0-9]+/logservices"
#define RF_EVT_SCAN_BLACK_LIST_JSON_SCHEMA "(?i)^/redfish/v1/jsonschemas"
#define RF_EVT_SCAN_BLACK_LIST_MANAGER_LOGENTRY "(?i)^/redfish/v1/managers/[a-zA-Z0-9]+/logservices"



#define RF_EVT_SCAN_BLACK_LIST_DATA_ACQ_REPORT "(?i)^/redfish/v1/dataacquisitionservice/dataacquisitionreport"


#define RF_EVT_SCAN_BLACK_LIST_SYSTEM_OVERVIEW "(?i)^/redfish/v1/systemoverview"
#define RF_EVT_SCAN_BLACK_LIST_REGISTRIES "(?i)^/redfish/v1/registries"
#define RF_EVT_SCAN_BLACK_LIST_SCHEMASTORE "(?i)^/redfish/v1/schemastore"
#define RF_EVT_SCAN_BLACK_LIST_PRIVILEGE_MAP "(?i)^/redfish/v1/accountservice/privilegemap$"

#define SMM_CHASSIS_ENCLOSURE "Enclosure"
#define URI_CHASSIS_ENCLOSURE "/redfish/v1/Chassis/Enclosure"
#define RFOBJ_HMM1 "HMM1"
#define RFOBJ_HMM2 "HMM2"
#define RFPROP_OEM "Oem"
#define RFPROP_OEM_HUAWEI "Huawei"
#define RFPROP_MANAGER_SERIALINTERFACES "SerialInterfaces"
#define RFPROP_MANAGER_NETWORK_PROTOCOL_URI "/redfish/v1/Managers/%s/NetworkProtocol"
#define RFPROP_MANAGER_NETWORK_PROTOCOL "NetworkProtocol"
#define RFPROP_MANAGER_LOGSERVICES_URI "/redfish/v1/Managers/%s/LogServices"
#define RFPROP_MANAGER_LOGSERVICES "LogServices"
#define PRPROP_MANAGER_SMTPSERVICES_URI "/redfish/v1/Managers/%s/SmtpService"
#define PRPROP_MANAGER_STMPSERVICES "SmtpService"
#define RFPROP_MANAGER_FPCSERVICE "FPCService"


#define URI_FORMAT_EVT_SUBSCRIPTION "/redfish/v1/EventService/Subscriptions/%d"




#define RFPROP_JNLP_FILE_NAME "file"
#define RFPROP_JNLP_FILE_CONTENT "buffer"

#define SP_CACHEDATA_MAX_LENGTH 51200

enum SP_DATA_HANDLE_OPERATION {
    SP_HANDLE_POST = 0,
    SP_HANDLE_OM_GET,
    SP_HANDLE_SP_GET,
    SP_HANDLE_DEL,
    SP_HANDLE_EXIST_CHECK,
    SP_HANDLE_UNKNOWN
};

enum SP_DATA_STATUS {
    SP_DATA_EXIST = 2, // 作为返回值，与VOS_OK和VOS_ERR错开
    SP_DATA_MISSING,
    SP_DATA_TOO_LONG,
    SP_DATA_ENCRYPT_FAILED,
    SP_DATA_TIMEOUT,
    SP_DATA_UNKNOWN
};

enum SP_RESULT_CHANGE_CONDITION {
    OM_POST_CFG_INFO = 0,
    OM_SET_START_FROM_SP
};

enum SP_RESULT_STATUS {
    SP_RET_IDLE = 0,  // BMC空闲
    SP_RET_INIT,      // BMC在SP心跳建立前主动备份result_init到flash，在OM设置从SP启动时还原
    SP_RET_DEPLOYING, // BMC与SP建立心跳后，SP回应结果
    SP_RET_FINISHED,  // SP部署完成后设置
    SP_RET_TIMEOUT,   // SP部署超时
};


typedef struct _role_privilege_info {
    gchar priv[PRIVILEGE_MAX_LEN];
    guchar enabled;
    guchar duplicate;
} role_priv_info_s;

typedef struct _user_privilege_info {
    const char *role;
    guchar privilege;
    guchar role_id;
} user_privilege_info_s;


typedef struct tag_csr_gen_info {
    gchar user_name[SESSION_USER_NAME_MAX_LEN + 1];
    gchar session_ip[SESSION_IP_LEN];
    guint32 user_role_priv;
    gchar country_name[SSL_PARAMETER_LEN2 + 1];
    gchar state_name[SSL_PARAMETER_LEN128 + 1];
    gchar city_name[SSL_PARAMETER_LEN128 + 1];
    gchar org_name[SSL_PARAMETER_LEN64 + 1];
    gchar org_unit[SSL_PARAMETER_LEN64 + 1];
    gchar common_name[SSL_PARAMETER_LEN64 + 1];
    guchar req_from_webui_flag;
} CSR_GEN_INFO_S;


typedef struct tag_key_update_info {
    gchar user_name[SESSION_USER_NAME_MAX_LEN + 1];
    gchar session_ip[SESSION_IP_LEN];
    guint8 user_role_priv;
    guchar req_from_webui_flag;
} KEY_UPDATE_INFO_S;


enum ARCARD_ACTION_INFO_S {
    ARCARD_CONTROL_POWER_OFF = 0,  // 下电
    ARCARD_CONTROL_POWER_ON, // 上电
    ARCARD_CONTROL_RESET, // 复位
    ARCARD_CONTROL_FACTORY_RESET, // 恢复出厂设置
    ARCARD_CONTROL_INVALID_ACTION = 0xff
};

enum EVENT_SOURCE {
    LOCAL_BOARD_EVENT = 0, // 本板的传感器事件
    OTHER_BOARD_EVENT, // 其他板的传感器事件
};

#define EVENT_SENSOR_NAME_LENGTH (256 + 32)
#define EVENT_STR_LENGTH 1024


typedef struct tag_sensor_healthevent_info {
    
    guint16 next_record_id;
    
    guint32 event_time;
    
    guint32 event_level;
    
    guint32 event_code;
    
    guint8 event_fru_type;
    
    gchar event_desc[EVENT_STR_LENGTH];
    
    gchar event_sugg[EVENT_STR_LENGTH];
    
    gchar event_args[EVENT_STR_LENGTH];
    
    gchar event_sensor_name[EVENT_SENSOR_NAME_LENGTH];
}SensorHealtheventInfo;

#define RFACTION_PARAM_TYPE "Type"
#define RFACTION_PARAM_CONTENT "Content"
#define RF_IMPORT_CONTENT_FILE_PATH "/tmp/rf_import_tmp"
#define RFPROP_CERT_TYPE "CertificateType"
#define SERVER_CERT_TYPE 0
#define CUSTOM_CERT_TYPE 1


#ifdef ARM64_HI1711_ENABLED
#define DICE_CERT_TYPE 2
#endif

#define RF_MEZZ_CONFIGURATION "Configuration"


#define RFPROP_MANUFACTURE "Manufacturer"
#define RFPROP_SERIAL_NUMBER "SerialNumber"
#define RFPROP_PART_NUMBER "PartNumber"
#define RFPROP_ASSET_TAG "AssetTag"
#define RFPROP_MODEL "Model"

#define RF_IOBOARD_CHASSIS_PREFIX "IOM"
#define RF_GPUBOARD_CHASSIS_PREFIX "Drawer"
#define RF_NODE_CHASSIS_PREFIX "Blade"
#define RF_SMM_CHASSIS_PREFIX "SMM"
#define RF_ENCLOSURE_CHASSIS_PREFIX "Enc"
#define RF_RACK_CHASSIS_PREFIX "Rack"
#define URI_PATTERN_CHASSIS_RELATED_RSC "(?i)^/redfish/v1/chassis/([a-zA-Z0-9]+).*$"
#define URI_PATTERN_PCIE_CARD_RSC "(?i)^/redfish/v1/chassis/[^/]+/pciedevices/PCIeCard([^/]+)"
#define URI_PATTERN_OCP_CARD_RSC "(?i)^/redfish/v1/chassis/[^/]+/pciedevices/OCPCard([^/]+)"
#define URI_PATTERN_NPU_RSC "(?i)^/redfish/v1/chassis/[^/]+/pciedevices/NPU([^/]+)"

#define RF_PCIECARD_PREFIX "PCIeCard"
#define RF_OCPCARD_PREFIX "OCPCard"


#define RF_RACK_CHASSIS_PREFIX "Rack"


#define RFPROP_UPGRADE_ENOUGH                       "UpgradeEnough"
#define RFPROP_TMP_SIZE                             98 
#define UPGRADE_THRESHOLD                           90 
#define RFPROP_POWER_TYPE                           "PowerType"
typedef struct tag_chassis_component_info {
    guchar component_type;
    const gchar *component_num_prop;
    const gchar *chassis_id_prefix;
    const gchar *chassis_type;
} CHASSIS_COMPONENT_INFO;


typedef struct tag_sensor_compare_info {
    
    guchar sensor_type;

    
    OBJ_HANDLE loc_handle;
} SENSOR_COMPARE_INFO_S;




#define RF_PROP_FABRIC_LINKS "Fabrics"
#define URI_FORMAT_FABRIC "/redfish/v1/Fabrics"
#define URI_FORMAT_ZONE "/redfish/v1/Fabrics/%s/Zones/%d"
#define URI_FORMAT_ENDPOINT "/redfish/v1/Fabrics/%s/Endpoints/%s"
#define RFACTION_CONFIG_PCIE_TOPO "Oem/Huawei/Fabric.ConfigCompositionMode"
#define URI_PATTERN_FABRIC_COLLECTION "(?i)^/redfish/v1/fabrics$"
#define URI_PATTERN_FABRIC_PCIE_CONFIG_TOPO_ACT_INFO "(?i)^/redfish/v1/fabrics/pcie/configcompositionmodeactioninfo$"
#define URI_PATTERN_FABRIC_PCIE "(?i)^/redfish/v1/fabrics/pcie$"
#define URI_PATTERN_FABRIC_PCIE_ZONES "(?i)^/redfish/v1/fabrics/pcie/zones$"
#define URI_PATTERN_FABRIC_PCIE_ENDPOINTS "(?i)^/redfish/v1/fabrics/pcie/endpoints$"
#define URI_PATTERN_FABRIC_PCIE_ZONE "(?i)^/redfish/v1/fabrics/pcie/zones/[^/]+$"
#define URI_PATTERN_FABRIC_PCIE_ENDPOINT "(?i)^/redfish/v1/fabrics/pcie/endpoints/[^/]+$"
#define RSC_PATH_FABRIC "/redfish/v1/fabrics"
#define RSC_PATH_FABRIC_PCIE "/redfish/v1/fabrics/pcie"
#define RSC_PATH_FABRIC_EP_COLLECTION "/redfish/v1/fabrics/pcie/endpoints"
#define RSC_PATH_FABRIC_EP "/redfish/v1/fabrics/pcie/endpoints/template"
#define RSC_PATH_FABRIC_ZONE_COLLECTION "/redfish/v1/fabrics/pcie/zones"
#define RSC_PATH_FABRIC_ZONE "/redfish/v1/fabrics/pcie/zones/template"
#define RSC_PATH_PCIE_FABRIC_CONFIG_TOPO_ACT_INFO "/redfish/v1/fabrics/pcie/configcompositionmodeactioninfo"

#define RFACTION_PARAM_MODE_ID "ModeId"
#define RF_TASK_NAME_CONFIG_COMPOSITION "PCIe Topology Config Task"

#define RFPROP_COMPOSITION_MODE_INFO "CompositionModeInfo"
#define RFPROP_COMPOSITION_CURRENT_SETTING "CurrentMode"
#define RFPROP_COMPOSITION_PENDDING_SETTING "PenddingMode"
#define RFPROP_COMPOSITION_AVALIBLE_MODE "AvailableMode"
#define RFPROP_COMPOSITION_MODE_ID "ModeId"
#define RFPROP_COMPOSITION_RELATED_ZONES "RelatedZones"

#define RF_PCIE_FABRIC "PCIe"

#define RFPROP_ENDPOINT_PROTOCOL "EndpointProtocol"
#define RFPROP_ENDPOINT_CONNECTED_ENTITIES "ConnectedEntities"
#define RFPROP_ENDPOINT_ENTITY_TYPE "EntityType"
#define RFPROP_ENDPOINT_ENTITY_PCI_ID "EntityPciId"
#define RFPROP_ENDPOINT_ENTITY_LINK "EntityLink"

#define RFPROP_ZONE_ENDPOINTS "Endpoints"

#define MAX_ZONE_RSC_ID_LEN 3

#define RF_ZONE_NAME_FORMAT "Resource Zone %d"

#define VERIFY_ACCOUNT_RET_NUM 3
#define VERIFY_LOCAL_ACCOUNT_RET 0
#define VERIFY_KERBEROS_ACCOUNT_RET 1
#define VERIFY_LDAP_ACCOUNT_RET 2


#define BMC_UP_TIME_FILE  "/proc/uptime"
#define BMC_UP_TIME_FILE_LENGTH  128
#define BMC_UP_TIME_VALUE     "BMCUpTime"


#define URI_PATTERN_PERIPHERAL_FW "(?i)^/redfish/v1/updateservice/firmwareinventory/peripheral[^/]+$"
#define RSC_PATH_FIRMWARE_INVENTORY "/redfish/v1/updateservice/firmwareinventory/template"
#define RF_PREFIX_PERIPHERAL "Peripheral"
#define RF_MCU_FIRMWARE "MCU"
#define URI_FORMAT_FIRMWARE_INVENTORY "/redfish/v1/UpdateService/FirmwareInventory/%s"



#define RF_LOCATION_PERIPHERY "Periphery"
#define RF_MANU_HUAWEI "huawei"




#define MCU_SUPPORTED 1 // PCIE卡支持MCU的管理




#define RFPROP_PROCESSORS_HISTORY_USAGE_RATE "ProcessorsHistoryUsageRate"
#define RFPROP_MEMORY_HISTORY_USAGE_RATE "MemoryHistoryUsageRate"
#define RFPROP_NETWORK_HISTORY_USAGE_RATE "NetworkHistoryUsageRate"
#define RFPROP_INLET_HISTORY_TEMPERATURE "InletHistoryTemperature"
#define RFPROP_POWER_HISTORY_DATA "PowerHistoryData"

#define RFPROP_HISTORY_DATA "Data"
#define RFPROP_PROCESSORS_UTILISE "CpuUtilisePercents"
#define RFPROP_MEMORY_UTILISE "MemOSUtilisePercents"
#define RFPROP_NETWORK_BWU_WAVE_TITLE "BWUWaveTitle"
#define RFPROP_NETWORK_UTILISE "UtilisePercents"
#define RFPROP_INLET_TEMPERATURE "InletTempCelsius"
#define RFPROP_POWER_DATA "PowerWatts"
#define RFPROP_POWER_AVERAGE "PowerAverageWatts"
#define RFPROP_POWER_PEAK "PowerPeakWatts"
#define RFPROP_CURRENT_NETWORK_UTILISE "CurrentUtilisePercents"

#define OPT_PME_PRAM_FOLDER "/opt/pme/pram/"
#define PROCESSOR_HISTORY_USAGE_RATE_FILE "cpu_utilise_webview.dat"
#define MEMORY_HISTORY_USAGE_RATE_FILE "memory_os_webview.dat"
#define INLET_HISTORY_TEMPERATURE_FILE "env_web_view.dat"
#define POWER_HISTORY_DATA_FILE "ps_web_view.dat"

#define MAX_LABEL_LENGTH 32

#define RF_IMPORT_CCONFIG_FILE_TASK_DESC "Import U Mark Label Batch Configuration Task."
#define RF_IMPORTING_FILE_FLAG "ImportingFileFlag"
#define RF_IMPORT_OBJ_HANDLE "ObjHandle"


#define HEALTH_REPORT_JSON_PATH "/opt/pme/pram/diagnose_fdm_health_report.json"
#define CLASS_FDM_WEB_NAME "FdmWebConfig"

#define METHOD_SET_UPDATE_ENABLE                        "SetAotuUpdateEnable"
#define METHOD_SET_REFRESH_INTERVAL                        "SetRefreshInterval"
#define METHOD_SET_PFAE_TIMES                        "SetPfaeTimes"

#define METHOD_FDM_FRUTREE "FDMFruTree"

#define METHOD_SET_FDM_FRUTREE "SetFDMFruTree"

#define FDM_SERVICE_ENABLED 1
#define FDM_REMOVE_ALL_ALARM_ENABLED 0
#define RFPROP_FDM_AUTO_UPDATE_FLAG "AutoUpdate"
#define RFPROP_FDM_REPORT_TIME "ReportTime"
#define RFPROP_FDM_REFRESH_INTERVAL "RefreshIntervalHours"
#define RFPROP_FDM_PFAE_NOTIFY_OS_ENABLED "PFAENotifyOsEnabled"
#define RFPROP_FDM_DIAGNOSE_TIME "PFAETimeDays"
#define RFPROP_FDM_DIAGNOSE_FAIL_POLICY "DiagnoseFailurePolicy"
#define RFPROP_FDM_DIAGNOSE_SUCCESS_POLICY "DiagnoseSuccessPolicy"
#define RFPROP_FDM_MEM_POOR_CONTACT_ALARM_ENABLED "MemPoorContactAlarmEnabled"
#define RFPROP_FDM_DYNAMIC_REMAPPING_ENABLE "MemoryDynamicRemappingEnabled"
#define RFPROP_FDM_HEALTH_REPORT "HealthReport"
#define RFPROP_FDM_DEVICE_TREE "DeviceTree"
#define RFPROP_FDM_EVENT_REPORT "EventReports"
#define RFPROP_FDM_EVENT_RECORD "EventRecords"
#define RFPROP_FDM_COMPONENT_ALIAS "Alias"
#define RFPROP_FDM_COMPONENT_HEALTH_STATUS "HealthStatus"
#define RFPROP_FDM_DEVICE_INFO "DeviceInfo"
#define RFACTION_FDM_REGENERATE "FDMService.ReGenerate"
#define RFACTION_FDM_REMOVEALARM "FDMService.RemoveAlarm"
#define RFPROP_FDM_RELEATED_DEVICE "RelatedDevices"
#define RFPROP_FDM_SILK_NAME "SilkName"
#define RFPROP_FDM_COMPONENT_TYPE "ComponentType"
#define RFPROP_FDM_COMPONENT_ID "ComponentID"
#define RFPROP_FDM_ALIAS "Alias"
#define RFPROP_FDM_HEALTH_STATUS "HealthStatus"
#define RFPROP_FDM_ACTION_NAME "Actions"
#define RFPROP_MANAGER_FDMSERVICE "FDMService"

#define PROPERTY_FDM_COMPONENT_MAINTENCE_TINE "ReportTimeStamp"
#define PROPERTY_FDM_AUTO_UPDATE_FLAG "AotuUpdateSwtich"
#define PROPERTY_FDM_REFRESH_INTERVAL "UpdateCycle"
#define PROPERTY_FDM_DIAGNOSE_TIME "TimeQuant"
#define PROPERTY_FDM_EVENT_REPORT "History"
#define PROPERTY_FDM_SUB_TREE "Children"
#define PROPERTY_FDM_MAINTENCE_REPORT "MaintenceHistroy"
#define PROPERTY_FDM_UPDATE_CONFIG "WebConfig"
#define PROPERTY_FDM_HEALTH_REPORT "HealthReport"
#define PROPERTY_FDM_DEVICE_TREE "DevTree"
#define PROPERTY_FDM_DEVICE_CHILDREN "Children"
#define PROPERTY_FDM_SILK_NAME "SilkName"
#define PROPERTY_FDM_COMPONENT_TYPE "ComponentType"
#define PROPERTY_FDM_COMPONENT_ID "ComponentID"
#define PROPERTY_FDM_ALIAS "Alias"
#define PROPERTY_FDM_HEALTH_STATUS "HealthStatus"

#define PROPERTY_FDM_COMPONENT_MAINTENCE_TIME_ZONE "ReportTimeZone"
#define PROPERTY_FDM_ONLINE_TIME_STAMP "OnlineTimeStamp"
#define PROPERTY_FDM_ONLINE_TIME_ZONE "OnlineTimeZone"


#define RF_FDM_DIAG_POLICY_NO_ACTION "NoAction"
#define RF_FDM_DIAG_POLICY_WARM_RESET "Reset"
#define RF_FDM_DIAG_POLICY_POWEROFF "PowerOff"
#define RF_FDM_DIAG_POLICY_NO_ACTION_VALUE 0
#define RF_FDM_DIAG_POLICY_WARM_RESET_VALUE 1
#define RF_FDM_DIAG_POLICY_POWEROFF_VALUE 2

#define PROPERTY_TYPE_COMPONENT_DEVICE_NAME "DeviceName"
#define PROPERTY_TYPE_COMPONENT_LOCATION "Location"

#define PROPERTY_REMOVE_ALARM_ALIAS "Alias"

#define URI_FORMAT_FDMSERVICE "/redfish/v1/Managers/%s/FDMService"
#define URI_FORMAT_FDM_EVENT_REPORT "/redfish/v1/Managers/%s/FDMService/%s/EventReports"
#define MANAGER_FDMSERVICE_CONTEXT "/redfish/v1/$metadata#Managers/%s/FDMService/$entity"
#define MANAGER_FDMSERVICE_EVENTREPORT "/redfish/v1/Managers/%s/FDMService/%s/EventReports"
#define URI_FORMAT_REGERNATE_END "FDMService.ReGenerateActionInfo"
#define URI_FORMAT_REMOVE_ALARM_END "FDMService.RemoveAlarmActionInfo"


#define URI_FORMAT_PS_PRE "/redfish/v1/Chassis/%s/Power#/PowerSupplies"
#define URI_FORMAT_FAN_PRE "/redfish/v1/Chassis/%s/Thermal#Fans"
#define URI_FORMAT_PCIE_PRE "/redfish/v1/Chassis/%s/PCIeDevices"
#define URI_FORMAT_PS_END "#/PowerSupplies"
#define URI_FORMAT_FAN_END "#Fans"
#define URI_FDMSERVICE "fdmservice"
#define URI_EVENT_REPORT "eventreports"

#define URI_PATTERN_DATA_DA_REPORT "^/redfish/v1/dataacquisitionservice/dataacquisitionreport$"
#define RSC_PATH_DATA_DA_REPORT "/redfish/v1/dataacquisitionservice/dataacquisitionreport"

#define DEVICE_NAME_PSU_STRING "PSU"
#define DEVICE_NAME_CPU_STRING "CPU"
#define DEVICE_NAME_FAN_STRING "Fan"
#define DEVICE_NAME_CHASSIS_STRING "Chassis"
#define DEVICE_NAME_PCIE_STRING "PCIeCard"

#define DEVICE_NAME_DISK_STRING "Disk"
#define DEVICE_NAME_DIMM_STRING "DIMM"
#define DEVICE_NAME_LOM_STRING "LOM"
#define DEVICE_NAME_DISKBP_STRING "DiskBP"
#define DEVICE_NAME_NIC_STRING "NIC"
#define DEVICE_NAME_RISERCARD_STRING "PcieRiserCard"
#define DEVICE_NAME_RAIDCARD_STRING "RiserCard"
#define DEVICE_NAME_MAINBOARD_STRING "RAIDCard"
#define DEVICE_NAME_DISK_BACKPLANE_STRING "Mainboard"
#define DEVICE_NAME_PCIERISERCARD_STRING "DiskBackplane"

#define MAX_ALIAS_LEN 11
#define DATE_TYPE_STRING_LEN 20
#define MAX_URL_END_LEN 128
#define MAX_SILK_NAME_LEN 40
#define MAX_RECURSION_DEPTH 200


#define PROPERTY_DA_REPORT_START_TIME "starttime"
#define PROPERTY_DA_REPORT_END_TIME "endtime"
#define PROPERTY_DA_REPORT_COMP_TYPE "componenttype"
#define PROPERTY_DA_REPORT_METRIC_TYPE "metrictype"
#define PROPERTY_DA_REPORT_SER_NUM "serialnumber"

#define RFPROP_SECURITY_CONTROL_VERSION "SecurityControlVersion"
#define RFPROPERTY_SESSION_TOKEN_LENGTH            "SessionTokenLength"
#define RFPROP_MK_AUTO_UPDATE_INTERVAL "MasterKeyUpdateInterval"
#define RFPROP_HTTPS_TRANSFER_CERT_VERIFICATION "HttpsTransferCertVerification"
#define REPROP_SOL_AUTO_OS_LOCK_ENABLED "SOLAutoOSLockEnabled"
#define REPROP_SOL_AUTO_OS_LOCK_KEY "SOLAutoOSLockKey"
#define RFPROP_REMOTE_HTTPS_SERVER_CERT_CHAIN_IFNO "RemoteHttpsServerCertChainInfo"
#define RPPROP_SERVER_CERT "ServerCert"
#define RFPROP_SSH_CIPHERS "SSHCiphers"
#define RFPROP_SSH_KEXS "SSHKexAlgorithms"
#define RFPROP_SSH_MACS "SSHMACs"
#define RFPROP_SSH_HOST_KEYS "SSHHostKeyAlgorithms"
#define RFPROP_SSL_CIPHER_SUITES "SSLCipherSuites"
#define RFPROP_RMCP_CIPHER_SUITES "RMCPCipherSuites"
#define RFACTION_UPDATE_MASTERKEY "SecurityService.UpdateMasterKey"
#define RFACTION_SECURITY_SERVICE_IMPORT_TRUST_CERT "SecurityService.ImportRemoteHttpsServerRootCA"
#define RFACTION_SECURITY_SERVICE_DELETE_TRUST_CERT "SecurityService.DeleteRemoteHttpsServerRootCA"
#define RFACTION_SECURITY_SERVICE_IMPORT_CRL "SecurityService.ImportRemoteHttpsServerCrl"
#define RFACTION_SECURITY_SERVICE_DELETE_CRL "SecurityService.DeleteRemoteHttpsServerCrl"

#define RFPROP_AUTHENTICATION_ALGORITHM "AuthenticationAlgorithm"
#define RFPROP_INTEGRITY_ALGORITHM "IntegrityAlgorithm"
#define RFPROP_CONFIDENTIALITY_ALGORITHM "ConfidentialityAlgorithm"


#define URI_FORMAT_SYSTEM_OVERVIEW_ODATA_ID "/redfish/v1/SystemOverview"
#define URI_FORMAT_SYSTEM_OVERVIEW_ODATA_CONTEXT "/redfish/v1/$metadata#SystemOverview"
#define URI_FORMAT_SYSTEM_OVERVIEW_ODATA_TYPE "#SystemOverview.v1_0_0.SystemOverview"
#define RFPROP_POWER_PRESENT_PSU_NUM "PresentPSUNum"
#define RFPROP_POWER_PRESENT_FAN_NUM "PresentFanNum"
#define RFPROP_SYSTEM_OVERVIEW_MANAGERS "Managers"
#define RFPROP_SYSTEM_OVERVIEW_SYSTEMS "Systems"
#define RFPROP_SYSTEM_OVERVIEW_CHASSIS "Chassis"
#define RFPROP_SYSTEM_OVERVIEW_INLETTEMP "InletTemperature"
#define SENSER_INLET_TEMP_INNER_NAME "Inlet_TEMP"
#define RFPROP_SYSTEM_OVERVIEW_MEMORY_COUNT "MemoryCount"

#define URI_PATTERN_CHASSIS_LANSWITCHES "^/redfish/v1/chassis/enclosure/switches$"
#define URI_PATTERN_CHASSIS_LANSWITCH "^/redfish/v1/chassis/enclosure/switches/[a-zA-Z0-9]+$"
#define URI_PATTERN_CHASSIS_LANSWITCH_PORTS "^/redfish/v1/chassis/enclosure/switches/[a-zA-Z0-9]+/ports$"
#define URI_PATTERN_CHASSIS_LANSWITCH_PORT \
    "^/redfish/v1/chassis/enclosure/switches/[a-zA-Z0-9]+\
/ports/([a-zA-Z0-9]|[a-zA-Z_][a-zA-Z0-9_.])+$"
#define URI_PATTERN_CHASSIS_PORT_ENABLE \
    "^/redfish/v1/chassis/enclosure/switches/[a-zA-Z0-9]+\
/ports/([a-zA-Z0-9]|[a-zA-Z_][a-zA-Z0-9_.])+/setportenableactioninfo"

#define RSC_PATH_CHASSIS_LANSWITCHES "/redfish/v1/chassis/enclosure/switches"
#define RSC_PATH_CHASSIS_LANSWITCH "/redfish/v1/chassis/enclosure/switches/1"
#define RSC_PATH_CHASSIS_LANSWITCH_PORTS "/redfish/v1/chassis/enclosure/switches/1/ports"
#define RSC_PATH_CHASSIS_LANSWITCH_PORT "/redfish/v1/chassis/enclosure/switches/1/ports/template"
#define RSC_PATH_CHASSIS_PORT_ENABLE "/redfish/v1/chassis/enclosure/switches/1/ports/template/setportenableactioninfo"

#define URI_FORMAT_LANSWITCH_COLLECTION_ODATA_ID "/redfish/v1/Chassis/Enclosure/Switches/%s"
#define URI_FORMAT_LANSWITCH_PORTS "/redfish/v1/Chassis/Enclosure/Switches/%s/Ports"
#define URI_FORMAT_LANSWITCH_PORT "/redfish/v1/Chassis/Enclosure/Switches/%s/Ports/%s"
#define URI_FORMAT_LANSWITCH_COLLECTION "/redfish/v1/Chassis/Enclosure/Switches"
#define RFPROP_CHASSIS_LANSWITCH_PORTS "Ports"
#define RFPROP_CHASSIS_LANSWITCH_PORT_ID "PortId"
#define RFPROP_CHASSIS_LANSWITCH_OEM_MTU "Mtu"
#define RFPROP_CHASSIS_LANSWITCH_OEM_PVID "Pvid"
#define RFPROP_CHASSIS_LANSWITCH_OEM_LINKSTATUS "LinkStatus"
#define RFPROP_CHASSIS_LANSWITCH_OEM_PORTS "Ports"
#define RFPROP_CHASSIS_LANSWITCH_OEM_WORKRATE "WorkRate(bps)"
#define RFPROP_CHASSIS_LANSWITCH_OEM_LOOPMODE "LoopMode"
#define RFPROP_CHASSIS_LANSWITCH_OEM_MAXSPEED "MaxSpeed"
#define RFPROP_CHASSIS_LANSWITCH_OEM_AUTONEG "Autonego"
#define RFPROP_CHASSIS_LANSWITCH_OEM_DUPLEX "Duplex"
#define RFPROP_CHASSIS_LANSWITCH_OEM_MAXSPEEDGBPS "MaxSpeedGbps"
#define RFPROP_CHASSIS_LANSWITCH_OEM_PEERPORTID "PeerPortId"
#define RFPROP_CHASSIS_LANSWITCH_OEM_PEERBLADEID "PeerBladeId"
#define RFPROP_CHASSIS_LANSWITCH_OEM_STATISTICS "Statistics"
#define RFPROP_CHASSIS_LANSWITCH_OEM_TOTALOCTRX "TotalOctetsReceived"
#define RFPROP_CHASSIS_LANSWITCH_OEM_TOTALOCTTX "TotalOctetsTransmitted"
#define RFPROP_CHASSIS_LANSWITCH_OEM_UNICASTPKTRX "UnicastPacketsReceived"
#define RFPROP_CHASSIS_LANSWITCH_OEM_MULTICASTPKTRX "MulticastPacketsReceived"
#define RFPROP_CHASSIS_LANSWITCH_OEM_BROADCASTPKTRX "BroadcastPacketsReceived"
#define RFPROP_CHASSIS_LANSWITCH_OEM_UNICASTPKTTX "UnicastPacketsTransmitted"
#define RFPROP_CHASSIS_LANSWITCH_OEM_MULTICASTPKTTX "MulticastPacketsTransmitted"
#define RFPROP_CHASSIS_LANSWITCH_OEM_BROADCASTPKTTX "BroadcastPacketsTransmitted"
#define RFPROP_CHASSIS_LANSWITCH_OEM_TOTALDISCARDSRX "TotalDiscardsReceived"
#define RFPROP_CHASSIS_LANSWITCH_OEM_TOTALDISCARDSTX "TotalDiscardsTransmitted"
#define RFPROP_CHASSIS_LANSWITCH_OEM_TOTALERRORRX "TotalErrorReceived"
#define RFPROP_CHASSIS_LANSWITCH_OEM_TOTALERRORTX "TotalErrorTransmitted"
#define RFPROP_CHASSIS_LANSWITCH_OEM_STATUS "Status"
#define RFPROP_CHASSIS_LANSWITCH_OEM_PORT_ENABLE "PortEnable"
#define RFPROP_CHASSIS_LSW_SET_PORT_ENABLE "#Port.SetPortEnable"
#define RFPROP_CHASSIS_SET_PORT_ENABLE_ACTION "Oem/Huawei/Port.SetPortEnable"
#define URI_FORMAT_LSW_SET_PORT_ENABLE_TARGET "/redfish/v1/Chassis/Enclosure/Switches/%s/Ports/%s/Actions/Oem/\
Huawei/Port.SetPortEnable"
#define URI_FORMAT_LSW_SET_PORT_ENABLE_INFO "/redfish/v1/Chassis/Enclosure/Switches/%s/Ports/%s/\
SetPortEnableActionInfo"

#define LSW_NODE_VALID 0x5a
#define LSW_NODE_INVALID 0xff
#define MAX_LSW_PLANE_NAME_LEN 32
#define MAX_LSW_PORT_NAME_LEN 32
#define MAX_LSW_PLANE_NUM 2

#define BLADE_NAME_POSITION 5

#define PLANE_NAME_POSITION 7

#define PORT_NAME_POSITION 9
#define MAX_MTU_NUMBER 65535
#define BLADE37 "blade37"
#define BLADE38 "blade38"
#define BLADE "blade"
#define SMM1_SLOT_ID 37
#define SMM2_SLOT_ID 38


#define UNKNOWN_ERROR_STR "unknown error"
#define FILENAME_TOO_LONG_STR "the length of filename exceeds the limit"

#define FILELEN_OUTOF_MAX_STR "failed to obtain the file length, or the file length exceeds the limit or out of memory"

#define CURL_RUN_ERROR_STR "file transfer failed, cann't finish file transfer process"
#define TRANSFILE_UNSUPPORTED_PROTOCOL_STR "transfer protocol is not allowed to use in current situation"

#define TRANSFILE_URL_FORMAT_STR "incorrect file path"

#define CURL_PEER_SSL_VERIFY_FAILED_STR "peer's certificate or fingerprint wasn't verified fine"
#define CURRENT_FILE_NO_PERMISSION_STR "have no permission to overwrite the same file"

#define DICE_GET_CERT_FAILED_STR "get dice cert failed"
#define UMS_BUSY_ERROR_STR "The SP is being used by x86 system"
#define MOUNT_FILE_UNAVAILABLE_STR \
    "The shared folder is faulty and cannot be mounted to the iBMC. Ensure that the shared folder can be accessed"
#define MOUNT_UMS_UNAVAILABLE_STR "Failed to mount the SP device"
#define CMS_AUTH_ERROR_STR "The file verification fails because the URI or SP file is incorrect"
#define UPGRADE_SP_ERROR_STR "Failed to upgrade the SP tool"
#define IMAGEURI_IS_ILLEGAL_STR "The URI format is incorrect"
#define UMS_TRANSFER_TIMEOUT_STR "File transfer timed out. Please check the network environment"
#define SP_IS_BUSY_STR "The SP is busy. Please try again"
#define SP_OF_X86_NEEDED_STR "Please insert the SmartProvisioning DVD for the x86_64 architecture"
#define SP_OF_ARM_NEEDED_STR "Please insert the SmartProvisioning DVD for the aarch64 architecture"
#define IN_UPGRADE_PROGRESS_STR "%s is being upgraded"

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 