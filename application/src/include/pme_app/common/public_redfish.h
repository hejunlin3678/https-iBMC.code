/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : public_redfish.h
  版 本 号   : 初稿
  作    者   : gongxiongtao (gwx455466)
  生成日期   : 2017年6月14日
  最近修改   :
  功能描述   : redfish 相关公共头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2016年6月14日
    作    者   : gongxiongtao (gwx455466)
    修改内容   : DTS2017061205611  提取出common_macro.h中redfish相关部分

******************************************************************************/
#ifndef __PUBLIC_REDFISH_H__
#define __PUBLIC_REDFISH_H__
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define HTTP_STATUS_CODE_MSD_DIVISOR 100
#define HTTP_STATUS_CODE_INFORMATIONAL_MSD 1
#define HTTP_STATUS_CODE_SUCCESSFUL_MSD 2
#define HTTP_STATUS_CODE_REDIRECTION_MSD 3
#define HTTP_STATUS_CODE_CLIENT_ERR_MSD 4
#define HTTP_STATUS_CODE_SERVER_ERR_MSD 5

#define HTTPS_PREFIX "https://"
#define HTTP_PREFIX "http://"

#define SERVICE_ROOT_PATH "/redfish/v1"

#define RF_SESSION_AUTH_TYPE         "authenticateType"
#define RF_SESSION_PRI               "privilege"
#define RF_SESSION_USERID            "local_userid"
#define RF_SESSION_IP                "loginipaddress"
#define RF_SESSION_NAME              "loginname"
#define RF_SESSION_USER_PRI          "user_privilege"
#define RF_SESSION_ROLEID            "user_roleid"
#define RF_SESSION_LOGIN_TIME        "logintime"
#define RF_SESSION_ACTIVE_TIME       "lastactivetime"
#define RF_SESSION_TOKEN             "x_auth_token"
#define RF_SESSION_ID                "session_id"
/* BEGIN for redfish : Added by lwx377954 2016-8-22 */
#define RF_SESSION_TYPE              "session_type"
#define RF_SESSION_CHECKSUM          "session_checksum"
/* END for redfish : Added by lwx377954 2016-8-22 */
#define RF_SESSION_DO_LOCAL_IBMC     "LocaliBMC"
#define RF_SESSION_DO_AUTO_MATCH     "AutomaticMatching"
/* BEGIN: Added by z00352904, 2019/2/11 21:49:22  PN:UADP155615 支持Kerberos后台认证接口，实现Kerberos认证和鉴权 */
#define RF_SESSION_DO_KRB_SSO        "KerberosSSO"
/* END:   Added by z00352904, 2019/2/11 21:49:25*/

/* BEGIN: Added by z00352904, 2016/12/1 17:22:5  PN:AR-0000264509-001-001 */

#define RF_TYPE_COMPUTER_SYSTEM                      "ComputerSystem"
#define RF_TYPE_ETH_INTERFACE                        "EthernetInterface"
#define RF_TYPE_VLAN_NET_INTERFACE                   "VLanNetworkInterface"
#define RF_TYPE_OEM_FC                               "OemFC"
#define RF_TYPE_FC_SFF                               "OemFCSff"
#define RF_TYPE_FC_SFF_DIAG                          "OemFCSffDiagnostic"
#define RF_TYPE_OEM_SUMMARY                          "OemSummary"
#define RF_TYPE_STORAGE_CONTROLLER                   "StorageController"
#define RF_TYPE_DRIVE                                "Drive"
#define RF_TYPE_DRIVE_COLLECTION                     "DriveCollection"
#define RF_TYPE_VOLUME                               "Volume"
#define RF_TYPE_OEM_SMART_INFO                       "OemSMARTInfo"
#define RF_TYPE_MANAGER                              "Manager"
#define RF_TYPE_EVENT_DEST_COLLECTION                "EventDestinationCollection"
/* BEGIN: Added by gwx455466, 2017/3/2    PN:AR-0000256847-001-001 */
#define RF_TYPE_OEM_BATTERY                          "OemBattery"
/* END:   Added by gwx455466, 2017/3/2*/
/* BEGIN: Added by gwx455466, 2017/12/22 15:19:6   问题单号:AR.SR.SFEA02109778.006.003 */
#define RF_TYPE_OEM_BIT_ERROR_COLLECTION            "OemBitErrorCollection"
#define RF_TYPE_OEM_BIT_ERROR                       "OemBitError"
#define RF_TYPE_LOGSERVICECOLLECTION                "LogServiceCollection"
#define RF_PROPERTY_MEMBERS                     	"Members"
#define RF_PROPERTY_PHYLOG_SERVICES_SUF             "LogServices/iBMA"

#define RF_TYPE_LOGSERVICE                          "LogService"
#define RF_PROPERTY_ACTIONS                         "Actions"
#define RF_PROPERTY_LOGSERVICE_COLLECTLOG		    "#LogService.CollectLog"
#define RF_PROPERTY_TARGET                          "target"
#define RF_PROPERTY_LOGSERVICE_VALUE_IBMA           "iBMA"

#define RF_PROPERTY_LOGSERVICE_COLL_TYPE  		"Type"
#define RF_PROPERTY_LOGSERVICE_COLL_TYPE_SMART  "SMART"
#define RF_PROPERTY_LOGSERVICE_COLL_TARGETDEVICE  "TargetDevice"
#define RF_PROPERTY_LOGSERVICE_COLL_TARGETDEVICE_IBMC "iBMC"
#define RF_PROPERTY_LOGSERVICE_COLL_MAXNUMBEROFRECORDS  "MaxNumberOfRecords"
#define RF_PROPERTY_LOGSERVICE_COLL_MAXLOGSIZEMIB  "MaxLogSizeMib"
#define RF_PROPERTY_LOGSERVICE_COLL_PROTOCOL  "Protocol"
#define RF_PROPERTY_LOGSERVICE_COLL_PROTOCOL_REDFISH  "Redfish"
#define RF_PROPERTY_LOGSERVICE_COLL_PORT  "Port"
#define RF_PROPERTY_LOGSERVICE_COLL_PORT_NUM  40443

#define RF_PROPERTY_LOGSERVICE_COLL_USER  "User"
#define RF_PROPERTY_LOGSERVICE_COLL_PASSWORD  "Password"
#define RF_PROPERTY_LOGSERVICE_COLL_LOGPATH  "LogPath"

#define RF_CONTENT_TYPE            "application/json"
#define RF_CONTENT_TYPE_OCTET_STREAM            "application/octet-stream"

#define RF_PROPERTY_LOGSERVICE_COLLECTLOG_STATUS  "status"
#define RF_PROPERTY_LOGSERVICE_COLLECTLOG_TASKID  "TaskId"
#define RF_PROPERTY_HTTP_OK     200

#define  RFPROP_LOGSERVICE_HOSTLOG_PUSH_TARGET  "/redfish/v1/Systems/%s/LogServices/HostLog/Actions/Oem/%s/LogService.Push"
/* END:   Added by gwx455466, 2017/12/22 15:19:9 */

/* BEGIN: Modified by jwx372839, 2017/8/4 9:44:29   问题单号:AREA02109379-006-001 */
#define RF_TYPE_OEM_ETH_PORT_STATISTICS              "OemEthernetInterfaceStatistics"
/* END:   Modified by jwx372839, 2017/8/4 9:44:32 */
/* BEGIN: Added by gwx455466, 2017/8/2 11:8:33   问题单号:AR-NFVI */
#define RF_TYPE_ETH_IF_SFF 						"OemEthernetInterfaceSff"
#define RF_TYPE_ETH_IF_SFF_DIAG 					"OemEthernetInterfaceSffDiagnostic"
/* END:   Added by gwx455466, 2017/8/4 19:42:27 */

/* BEGIN: Added by gwx455466, 2017/8/26 10:55:11   问题单号:SREA02109352-001-003 */
#define RF_TYPE_OEM_ETH_INTERFACE_OAM               "OemEthernetInterfaceOAM"
#define RF_TYPE_OEM_ETH_INTERFACE_OAM_STAT         "OemEthernetInterfaceOAMStatistics"
/* END:   Added by gwx455466, 2017/8/26 10:55:14 */
/* BEGIN: Modified by zhanglei wx382755, 2017/12/21   PN:AR.SR.SFEA02109385.003.001 支持nvme属性增强 */
#define RF_TYPE_OEM_PCIE_SSD                        "OemPCIE_SSD"
/* END:   Modified by zhanglei wx382755, 2017/12/21 */
#define RF_TYPE_OEM_PCIE_SSD_COLLECTION             "OemPCIE_SSDCollection"
/* BEGIN: Modified by zwx382755, 2018/4/10   PN:AR.SR.SFEA02119720.021.001 */
#define RF_TYPE_OEM_INFINIBAND                      "OemInfiniBand"
#define RF_TYPE_IB_SFF                              "OemInfiniBandSff"
#define RF_TYPE_IB_SFF_DIAG                         "OemInfiniBandSffDiagnostic"
/* END:   Modified by zwx382755, 2018/4/10 */

#define RF_PROPERTY_PREFIX_RAID                      "RAID"
#define RF_PROPERTY_PREFIX_PCH                       "PCH"


/* BEGIN: Added by gwx455466, 2017/4/6 12:43:36   问题单号:AR-0000276593-005-009 */
#define RF_PROPERTY_ETH_PORT_TYPE                      "PortType"
#define RF_PROPERTY_ETH_PORT_TYPE_PHYSICAL 			"Physical"
#define RF_PROPERTY_ETH_PORT_TYPE_VIRTUAL 			"Virtual"
/* END:   Added by gwx455466, 2017/4/6 12:43:40 */

/* BEGIN: Modified by wangpenghui wwx382232, 2017/11/13 11:14:2   问题单号:DTS2017101300451 */
#define PROPERTY_VALUE_SMS_TYPE_SP               "SmartProvisioning"
/* END:   Modified by wangpenghui wwx382232, 2017/11/13 11:14:5 */
/* BEGIN: Modified by wangpenghui wwx382232, 2018/9/26 18:50:55   问题单号:Redfish接口补齐 */
#define RF_PROPERTY_ETH_SPEED_MBPS						       "SpeedMbps"
#define RF_PROPERTY_ETH_FULL_DUPLEX_ENABLED					"FullDuplex"
#define RF_PROPERTY_ETH_AUTO_NEGOTION_ENABLED				"AutoNeg"
#define RF_PROPERTY_ETH_ALIGNMENT_ERRORS			            "AlignmentErrors"
/* END:   Modified by wangpenghui wwx382232, 2018/9/26 18:50:57 */
//default mac地址，及永久物理地址
#define RF_PROPERTY_DEFAULT_MAC_ADDR							"PermanentMACAddress"
#define RF_PROPERTY_UP2COS									"Up2cos"
#define RF_PROPERTY_PFCUP										"Pfcup"
#define RF_PROPERTY_PGID										"Pgid"
#define RF_PROPERTY_PGPCT										"PGPCT"
#define RF_PROPERTY_PGSTRICT									"PgStrict"
#define RF_PROPERTY_TOTAL_BYTES_RECV							"TotalBytesReceived"
#define RF_PROPERTY_TOTAL_BYTES_TRANSMITTED					"TotalBytesTransmitted"
#define RF_PROPERTY_UNICAST_PK_RECV							"UnicastPacketsReceived"
#define RF_PROPERTY_MUL_PK_RECV								"MulticastPacketsReceived"	
#define RF_PROPERTY_BROAD_CAST_PK_RECV						"BroadcastPacketsReceived"
#define RF_PROPERTY_UNICAST_PK_TRANSMITTED					"UnicastPacketsTransmitted"
#define RF_PROPERTY_MUL_PK_TRANSMITTED						"MulticastPacketsTransmitted"
#define RF_PROPERTY_BROAD_CAST_PK_TRANSMITTED				"BroadcastPacketsTransmitted"
#define RF_PROPERTY_FCS_RECV_ERR								"FcsReceiveErrors"
#define RF_PROPERTY_RUNT_PK_RECV								"RuntPacketsReceived"
#define RF_PROPERTY_JABBER_PK_RECV							"JabberPacketsReceived"
#define RF_PROPERTY_SINGLE_COLLISION_TRANSMIT_FRAMES		"SingleCollisionTransmitFrames"
#define RF_PROPERTY_MUL_COLLISION_TRANSMIT_FRAMES			"MultipleCollisionTransmitFrames"
#define RF_PROPERTY_LATE_COLLISION_FRAMES					"LateCollisionFrames"
#define RF_PROPERTY_EXCESSIVE_COLLISION_FRAMES				"ExcessiveCollisionFrames"

#define RF_PROPERTY_PORT_CONNEC_SWITCH_NAME                     "SwitchName"
#define RF_PROPERTY_PORT_CONNEC_SWITCH_ID                     "SwitchConnectionID"
#define RF_PROPERTY_PORT_CONNEC_SWITCH_PORT_ID                     "SwitchConnectionPortID"
#define RF_PROPERTY_PORT_CONNEC_SWITCH_OS_VLAN_ID                     "SwitchPortVlanID"


#define PROPERTY_VALUE_SMS_TYPE_INBAND               "InbandManagement"
#define RF_PROPERTY_DRIVER_INFO                      "DriverInfo"
#define RF_PROPERTY_DRIVE_NAME                       "DriverName"
#define RF_PROPERTY_DRIVE_VERSION                    "DriverVersion"
#define RF_PROPERTY_EVENTS                           "Events"
#define RF_PROPERTY_EVENT_TYPE                       "EventType"
#define RF_PROPERTY_NIC_NAME                         "NICName"
#define RF_PROPERTY_NAME                             "Name"
#define RF_PROPERTY_IPV4_ADDR                        "IPv4Addresses"
#define RF_PROPERTY_IPV6_ADDR                        "IPv6Addresses"
#define RF_PROPERTY_IPV6_ROUTE                       "IPv6Routes"
#define RF_PROPERTY_MAC_ADDR                         "MACAddress"
#define RF_PROPERTY_ADDR                             "Address"
#define RF_PROPERTY_SUB_MASK                         "SubnetMask"
#define RF_PROPERTY_ADDR_ORIGIN                      "AddressOrigin"
#define RF_PROPERTY_GATEWAY                          "Gateway"
#define RF_PROPERTY_PREF_LEN                         "PrefixLength"
#define RF_PROPERTY_ADDR_STATE                       "AddressState"
#define RF_PROPERTY_ID                               "Id"
#define RF_PROPERTY_ID_2                             "ID"
#define RF_PROPERTY_ODATA_TYPE                       "@odata.type"
#define RF_PROPERTY_ODATA_ID                         "@odata.id"
#define RF_PROPERTY_ODATA_COUNT                      "Members@odata.count"
#define RF_PROPERTY_OEM                              "Oem"
#define RF_PROPERTY_HUAWEI                           "Huawei"
#define RF_PROPERTY_BDF_NUMBER                       "BDFNumber"
#define RF_PROPERTY_ROOT_BDF                         "RootBDF"
#define RF_PROPERTY_BDF                              "BDF"
/* BEGIN: Modified by lwx459244, 2018/3/19 10:20:11   PN:DTS2018031606349  */
#define RF_PROPERTY_PCIEPATH                         "PCIePath"
#define RF_PROPERTY_DEVICE_ID                        "DeviceID"
#define RF_PROPERTY_VENDOR_ID                        "VendorID"
#define RF_PROPERTY_SUBSYSTEM_DEVICE_ID              "SubsystemDeviceID"
#define RF_PROPERTY_SUBSYSTEM_VENDOR_ID              "SubsystemVendorID"
/* END: Modified by lwx459244, 2018/3/19 10:20:11   PN:DTS2018031606349  */
#define RF_PROPERTY_WWN                              "WWN"
#define RF_PROPERTY_PHY_PORT                         "PhysicalPort"
#define RF_PROPERTY_MEMORY                           "Memory"
#define RF_PROPERTY_TOTAL_SYS_MEM_GB                 "TotalSystemMemoryGiB"
#define RF_PROPERTY_TOTAL_PHY_MEM_GB                 "TotalPhysicalMemoryGiB"
#define RF_PROPERTY_BUFFER_SYS_MEM_GB               "SystemMemoryBuffersGiB"
#define RF_PROPERTY_CACHED_SYS_MEM_GB               "SystemMemoryCachedGiB"
#define RF_PROPERTY_TOTAL_MEM_USAGE                  "MemUsage"
#define RF_PROPERTY_PROCESSOR                        "Processor"
#define RF_PROPERTY_TOTAL_CPU_USAGE                  "TotalCPUUsage"
#define RF_PROPERTY_LIST                             "List"
#define RF_PROPERTY_CPU_ID                           "CPUId"
#define RF_PROPERTY_USAGE                            "Usage"
#define RF_PROPERTY_BMA_CPU_USAGE                   "iBMACPUUsage"
#define RF_PROPERTY_PROCESSOR_INFO                  "ProcessorInfo"
#define RF_PROPERTY_PROCESSOR_ID                    "ProcessorId"
#define RF_PROPERTY_PROCESSOR_USAGE                 "ProcessorUsage"
#define RF_PROPERTY_STORAGE                          "Storage"
#define RF_PROPERTY_OS_DRIVE_NAME                    "OSDriveName"
#define RF_PROPERTY_PHY_DISK_INFO                    "PhyDiskInfo"
#define RF_PROPERTY_TOTAL_CAPA_GB                    "TotalCapacityGiB"
#define RF_PROPERTY_USED_CAPA_GB                     "UsedCapacityGiB"
#define RF_PROPERTY_SN                               "SerialNo"
#define RF_PROPERTY_MANUFACTURER                     "Manufacturer"
#define RF_PROPERTY_FIRM_VER                         "FirmwareVersion"
#define RF_PROPERTY_ORIGN_COND                       "OriginOfCondition"
#define RF_PROPERTY_LINK_STATUS                      "LinkStatus"
#define RF_PROPERTY_LINK_SPEED                       "LinkSpeed"
#define RF_PROPERTY_STATE                            "State"
#define RF_PROPERTY_RAID                             "Raid"
#define RF_PROPERTY_LINKS                            "Links"
#define RF_PROPERTY_DRIVES                           "Drives"
#define RF_PROPERTY_VLAN_ENABLE                      "VLANEnable"
#define RF_PROPERTY_VLAN_ID                          "VLANId"
#define RF_PROPERTY_VLAN_PRIORITY                    "VLANPriority"
#define RF_PROPERTY_DESTINATION                      "Destination"
#define RF_PROPERTY_EVENT_TYPES                      "EventTypes"
#define RF_PROPERTY_CONTEXT                          "Context"
#define RF_PROPERTY_PROTOCOL                         "Protocol"
#define RF_PROPERTY_HTTP_HEADERS                     "HttpHeaders"
#define RF_PROPERTY_ORIGIN_RSC                       "OriginResources"
#define RF_PROPERTY_ORIGIN_CNT                       "OriginResources@odata.count"
#define RF_PROPERTY_MODEL                            "Model"
#define RF_PROPERTY_CAPA_BYTES                       "CapacityBytes"
#define RF_PROPERTY_MEDIA_TYPE                       "MediaType"
#define RF_PROPERTY_SERIAL_NUM                       "SerialNumber"
#define RF_PROPERTY_STATUS                           "Status"
#define RF_PROPERTY_CAPA_SPEED_GBS                   "CapableSpeedGbs"
#define RF_PROPERTY_DESCRIPTION                      "Description"
#define RF_PROPERTY_VOLUME_NAME                      "VolumeName"
#define RF_PROPERTY_SMART_DETAILS                    "SmartDetails"
#define RF_PROPERTY_SATA_SMART                       "SATA_SmartInfo"
#define RF_PROPERTY_ATTR_REVI_NUM                    "AttributeRevisionNumber"
#define RF_PROPERTY_ATTR_ITEMS_NUM                   "AttributesItemsNumeber"
#define RF_PROPERTY_ATTR_ITEMS_LIST                  "AttributeItemList"
#define RF_PROPERTY_ATTR_NEGOTIATED_SPEED_GB         "NegotiatedSpeedGbs"
#define RF_PROPERTY_TEMPERATURE                      "Temperature"
#define RF_PROPERTY_POWERON_HOURS                    "PowerOnHours"
#define RF_PROPERTY_PORTS                            "Ports"
#define RF_PROPERTY_SILK_SCREEN                      "SilkScreen"
#define RF_PROPERTY_WORK_MODE                        "WorkMode"
/* BEGIN: Modified by zwx382755, 2018/3/24   PN:AR.SR.SFEA02119720.021.001 */
#define RF_PROPERTY_LINK_MONITOR_FREQUENCY           "MIILinkMonitoringFrequencyMS"
#define RF_PROPERTY_UUID                             "UUID"
#define RF_PROPERTY_VLANS                            "VLANs"
#define RF_PROPERTY_IS_ONBOOT                        "IsOnBoot"//BMC呈现的属性名
#define RF_PROPERTY_ONBOOT                           "OnBoot"  //BMA呈现的属性名
#define ERROR_KEY                                    "error"//从redfish_message.h移动到此
#define RFPROP_MSG_ID                                "MessageId"
#define EXTENDED_INFO_KEY                            "@Message.ExtendedInfo"
#define RF_VALUE_NO_NEED_QUERY                       "MessageNoNeedQueryURI"
#define MESSAGE_ARGS_KEY                             "MessageArgs"
#define MESSAGES_KEY                                 "Messages"
#define RF_TASK_STATE_PROP                           "TaskState"

#define RF_COMPONENT_ID                              "Id"
#define RF_COMPONENT_NAME                            "Name"
#define RF_COMPONENT_VERSION                         "Version"
#define RF_COMPONENT_TYPE                            "Type"

#define RF_VALUE_TASK_OVER_TIME                      60//暂定1分钟超时(任务运行和资源更新超时)
#define NUM_TPYE_STRING                              6
#define RF_TASK_PROGRESS_BMA_TASK_CRETED             20
#define RF_TASK_PROGRESS_BMA_TASK_COMPLETE           50
#define RF_FINISH_PERCENTAGE                         100

#define FILE_DOWNLOAD_BASE_RATIO                     10    // 文件收集阶段占文件下载的百分比
#define FILE_DOWNLOAD_RATIO                          90    // 文件传输阶段占文件下载的百分比
#define FILE_DOWNLOAD_RATIO_TOTAL                    100   // 文件下载总百分比

#define RF_DELAY_TIME                                5
#define RF_VALUE_MAX_VLAN_ID                         4094
#define RF_VALUE_MIN_VLAN_ID                         0
#define RF_VALUE_MAX_TASK_COUNT                      20 //支持的同时运行的任务数量
#define RF_VALUE_DEFAULT_FLAG_VALUE                  G_MAXUINT16
#define RF_VALUE_DEFAULT_FLAG_VALUE_STR              "0xffff"
#define RF_VALUE_ON_BOOT_YES                         "yes"
#define RF_VALUE_ON_BOOT_NO                          "no"
#define RF_IPV4_PROP_SPLIT_FLAG                      ";"
#define RF_CREATE_VLAN_PARM_MAX_LENGTH               7      //创建vlan的参数个数
#define RF_SET_IP_PARM_MAX_LENGTH                    6      //set ipv4的参数个数
#define RF_SET_VLAN_PARM_MAX_LENGTH                  6      //set vlan的参数个数
#define RF_CREATE_BOND_PARM_MAX_LENGTH               7      //创建bond最多的参数个数
#define SLASH_FLAG_STR                               "/"
#define RF_BMA_URL_TASKS                             "/redfish/v1/Sms/1/TaskService/Tasks"
#define RF_BMA_URL_TEAMS                             "/redfish/v1/Sms/1/Systems/1/EthernetInterfaces/Team"
#define RF_VALUE_TASK_STATE_RUNNING                  "Running"
#define RF_VALUE_TASK_STATE_COMPLETED                "Completed"
#define RF_VALUE_TASK_STATE_EXCEPTION                "Exception"
#define RF_RSC_TYPE_INFINIBANDS                      "InfiniBands"
//从net_agentless.h移动到此处
typedef enum _tag_eth_port_type
{
    ETH_PORT_TYPE_VIRTUAL = 0,
    ETH_PORT_TYPE_PHYSICAL = 1,
} ETH_PORT_TYPE_E;
/* BEGIN: Added by zhongqiu 00356691, 2016/8/3   PN:DTS2016080303038 */
#define JSON_NULL_OBJECT_STR      "{ }"
/* END:   Added by zhongqiu 00356691, 2016/8/3   PN:DTS2016080303038 */
/* END:   Modified by zwx382755, 2018/3/24 */
#define RF_PROPERTY_SPAN_LIST                        "SpanList"
#define RF_PROPERTY_USED_SPACE                       "UsedSpaceMB"
#define RF_PROPERTY_ARRAY_ID                         "ArrayID"
#define RF_PROPERTY_FREE_SPACE                       "FreeSpaceMB"
#define RF_PROPERTY_PHY_DISK_LIST                    "PhyDiskList"
#define RF_PROPERTY_FC_ID                            "FC_ID"
#define RF_PROPERTY_PRODUCT_NAME                     "ProductName"
/* BEGIN: Modified by zhanglei wx382755, 2017/12/22   PN:AR.SR.SFEA02109385.003.001 支持nvme属性增强(这3个属性只是nvme使用)*/
#define RF_PROPERTY_DEVICE_SILK_SCREEN               "DeviceSilkScreen"
#define RF_PROPERTY_DEVICE_LOCATION                  "DeviceLocation"
#define RF_PROPERTY_SMART_INFO                       "SMARTInfo"
/* END:   Modified by zhanglei wx382755, 2017/12/22 */
/* BEGIN: Modified by gwx455466, 2017/5/26 14:27:59   问题单号:DTS2017052510845  */
#define RF_PROPERTY_PORT_NUM                    "PortNum"
/* END:   Modified by gwx455466, 2017/5/26 14:28:6 */

/* BEGIN: Modified by gwx455466, 2017/5/20 20:13:35   问题单号:DTS2017052006731  */
#define RF_PROPERTY_HDD_PATROL_STATE                 "PatrolState"
/* END:   Modified by gwx455466, 2017/5/20 20:13:42 */
/* BEGIN: Added by gwx455466, 2017/4/7 16:43:52   问题单号:AR-0000276593-003-006 */
#define RF_PROPERTY_PRODUCT_VALUE                     "VALUE"
#define RF_PROPERTY_PRODUCT_ATTRIBUTE_NAME                     "ATTRIBUTE_NAME"
#define RF_PROPERTY_PRODUCT_ATTRIBUTE_ID_NUM                     "ID#"
#define RF_PROPERTY_PRODUCT_ATTRIBUTE_RAW_VALUE          "RAW_VALUE"
#define RF_PROPERTY_PRODUCT_ATTRIBUTE_UPDATED          "UPDATED"
#define RF_PROPERTY_PRODUCT_ATTRIBUTE_FLAG          "FLAG"
#define RF_PROPERTY_PRODUCT_ATTRIBUTE_WORST          "WORST"
#define RF_PROPERTY_PRODUCT_ATTRIBUTE_WHEN_FAILED          "WHEN_FAILED"
#define RF_PROPERTY_PRODUCT_ATTRIBUTE_THRESHOLD          "THRESHOLD"
#define RF_PROPERTY_PRODUCT_ATTRIBUTE_TYPE          "TYPE"

#define RF_PROPERTY_SS_MEDIA_USED_ENDURANCE_INDICATOR                "SSMediaUsedEnduranceIndicator"
/* END:   Added by gwx455466, 2017/4/7 16:43:55 */
/* BEGIN: Modified by gwx455466, 2017/4/23 22:40:55   问题单号:DTS2017031010790 */
#define RF_PROPERTY_MODEL_REBUILD_STATES               "RebuildStatus"
#define RF_PROPERTY_MODEL_REBUILD_STATE                "RebuildState"
#define RF_PROPERTY_MODEL_REBUILD_PROGRESS               "RebuildProgress"
/* END:   Modified by gwx455466, 2017/4/23 22:41:3 */

#define RF_PROPERTY_SAS_SMART                        "SAS_SmartInfo"
#define RF_PROPERTY_HEALTH_STATUS                    "HealthStatus"
#define RF_PROPERTY_DRIVE_TEMP                       "DriveTemperature"
#define RF_PROPERTY_TRIP_TEMP                        "TripTemperature"
#define RF_PROPERTY_ELE_IN_GROWN                     "ElementsInGrownDefectList"
#define RF_PROPERTY_ELE_IN_PRIMARY                   "ElementsInPrimaryDefectList"
#define RF_PROPERTY_MANU_IN_WEEK                     "ManufacturedInWeekOfYear"
#define RF_PROPERTY_BLOCK_SEND                       "BlocksSentToInitiator"
#define RF_PROPERTY_BLOCK_RECE                       "BlocksReceivedFromInitiator"
#define RF_PROPERTY_HOURS_POWERUP                    "HoursOfPoweredUp"
#define RF_PROPERTY_MINU_NEXT_TEST                   "MinutesUntilNextInternalSMARTTest"
#define RF_PROPERTY_MEDIA_ERR_CNT                    "MediaErrorCount"
#define RF_PROPERTY_OTHER_ERR_CNT                    "OtherErrorCount"
#define RF_PROPERTY_PRE_FAIL_CNT                     "PredFailCount"
#define RF_PROPERTY_LAST_PRE_FAIL_SEQ                "LastPredFailEventSeqNum"

/* BEGIN: Added by gwx455466, 2017/12/22 15:50:14   问题单号:AR.SR.SFEA02109778.006.003 */
#define RF_PROPERTY_BIT_ERROR                       "BitError"
#define RF_PROPERTY_MEMBERS        					"Members"
#define RF_PROPERTY_ITEMS        					"Items"
#define RF_PROPERTY_INVALID_DC        				  "InvalidDwordCount"
#define RF_PROPERTY_LOSS_DSC        				   "LossDwordSynchCount"
#define RF_PROPERTY_PHYID        				  "PhyId"
#define RF_PROPERTY_PHY_RESETPC        				  "PhyResetProblemCount"
#define RF_PROPERTY_RUNNING_DISPARITY_EC        				  "RunningDisparityErrorCount"
#define RF_PROPERTY_EXPANDER_PREFIXE                "Expander_"
#define RF_PROPERTY_RAID_PREFIXE                "Raid_"
/* END:   Added by gwx455466, 2017/12/22 15:50:19 */

#define RF_VALUE_STATUS_CHANGE                       "StatusChange"
#define RF_VALUE_RSC_UPDATED                         "ResourceUpdated"
#define RF_VALUE_RSC_ADDED                           "ResourceAdded"
#define RF_VALUE_RSC_REMOVED                         "ResourceRemoved"
#define RF_VALUE_ALERT                               "Alert"
#define RF_VALUE_LINKUP                              "LinkUp"
#define RF_VALUE_LINKDOWN                            "LinkDown"
#define RF_VALUE_NOLINK                              "NoLink"
#define RF_VALUE_TRUE                                "true"
#define RF_VALUE_FALSE                               "false"

/* BEGIN: Modified by jwx372839, 2017/5/21 20:43:23   问题单号:DTS2017042008908  */
#define RF_VALUE_ONLINE                              "Online"
/* END:   Modified by jwx372839, 2017/5/21 20:43:27 */

#define DYNAMIC_CONN_BUSINESS_PORT                   "OSEthConnector"
#define DYNAMIC_CONN_FC_CARD                         "FCCardConnector"
#define DYNAMIC_CONN_FC_PORT                         "FCPortConnector"
#define DYNAMIC_CONN_DRIVE_PARTITION                 "DrivePartitionConnector"
#define DYNAMIC_CONN_RAID_CONTROLLER                 "RaidControllerConnector"
#define DYNAMIC_CONN_LOGICAL_DRIVE                   "LogicalDriveDynamicConnector"
#define DYNAMIC_CONN_VLAN                            "VLANConnector"
#define DYNAMIC_CONN_DISK_ARRAY                      "DiskArrayDynamicConnector"

#define DYNAMIC_OBJ_ETH_CARD                         "OSEthCard"
#define DYNAMIC_OBJ_BUSINESS_PORT                    "OSEthPort"
#define DYNAMIC_OBJ_FC_CARD                          "FCCard"
#define DYNAMIC_OBJ_FC_CARD_PORT                     "FCPort"
#define DYNAMIC_OBJ_DRIVE_PARTITION                  "DrivePartition"
#define DYNAMIC_OBJ_RAID_CONTROLLER                  "RaidControllerDynamic"
#define DYNAMIC_OBJ_LOCICAL_DRIVE                    "LogicalDriveDynamic"
#define DYNAMIC_OBJ_VLAN                             "VLAN"
#define DYNAMIC_OBJ_DISK_ARRAY                       "DiskArrayDynamic"
/* BEGIN: Modified by jwx372839, 2017/3/11 14:40:21   问题单号:AR-0000276593-005-007 */
#define DYNAMIC_OBJ_DRIVE_USAGE_MNTR                 "DiskPartitionUsageMntr"
/* END:   Modified by jwx372839, 2017/3/11 14:55:37 */

#define PROPERTY_VALUE_DEFAULT_STRING                "N/A"

/*BEGIN for redfish : Added by gwx455466 2017/3/2    PN:AR-0000256847-001-001 */

#define RF_PROPERTY_BATTERY_INFO                         "BatteryInfo"

#define RF_PROPERTY_NVDATA_VERSION                   "NVDATA_Version"
#define RF_PROPERTY_DEVICE_INTERFACE                 "DeviceInterface"
#define RF_PROPERTY_SAS_ADDRESS                       "SAS_Address"
#define RF_PROPERTY_MIN_STRIP_SIZE                    "MinStripSizeKB"
#define RF_PROPERTY_MAX_STRIP_SIZE                    "MaxStripSizeKB"
#define RF_PROPERTY_CACHE_PINNED_STATUS              "CachePinnedStatus"
#define RF_PROPERTY_MAINTAIN_PDFAIL_HISTORY         "MaintainPdFailHistory"
#define RF_PROPERTY_COPY_BACK_ENABLED                "CopybackEnabled"
#define RF_PROPERTY_SMARTER_COPY_BACK_ENABLED       "SmarterCopybackEnabled"
#define RF_PROPERTY_JBOD_ENABLED                      "JBOD_Enabled"
#define RF_PROPERTY_IS_BATTERY_EXIST                  "IsBatteryExist"
#define RF_PROPERTY_MEMORY_UCE_COUNT                  "MemoryUCECount"
#define RF_PROPERTY_MEMORY_ECC_BUCKET_SIZE	         "MemoryEccBucketSize"
#define RF_PROPERTY_MEMORY_ECC_COUNT	                 "MemoryEccCount"
/* BEGIN: Added by gwx455466, 2017/4/15 9:48:46   问题单号:AR-0000276593-001-016 */
#define RF_PROPERTY_MEMORY_CE_COUNT                  "MemoryCECount"
#define RF_PROPERTY_NVRAM_ERROR_COUNT	                 "NVRAMErrorCount"

#define RF_PROPERTY_PHYDISK_NUMBER                   "PhyDiskNumber"
/* END:   Added by gwx455466, 2017/4/15 9:48:48 */

#define RF_PROPERTY_CAPACITY                           "Capacity"
#define RF_PROPERTY_STRIP_SIZE                         "StripSizeKB"

#define RF_PROPERTY_DISK_CACHE_STATUS                 "DiskCacheStatus"
#define RF_PROPERTY_INIT_STATE                         "InitState"
#define RF_PROPERTY_BGI_ENABLED                        "BGIEnabled"
#define RF_PROPERTY_USED_FOR_SECONDARY_CACHE         "UsedForSecondaryCache"
#define RF_PROPERTY_CONSISTEN_CHECK                   "ConsistenCheck"
#define RF_PROPERTY_BOOT_DISK                          "BootDisk"

#define RF_PROPERTY_SPAN_LIST                             "SpanList"

#define RF_PROPERTY_HOT_SPARE_STATE                             "HotspareState"

/* BEGIN: Modified by jwx372839, 2017/4/23 16:46:18   问题单号:AR-0000276593-004-003 */
#define RF_PROPERTY_PREDICTION_STATE                 "PredictionSate"
/* END:   Modified by jwx372839, 2017/4/23 16:46:22 */

/* BEGIN: Modified by jwx372839, 2017/9/15 14:34:8   问题单号:AREA02109379-006-001 */
#define RF_PROPERTY_FAULT_DETAIL_STATE                "FaultDetailStatus"
#define RF_PROPERTY_PREDICT_DETAIL_STATE              "PredictiveDetailStatus"
/* END:   Modified by jwx372839, 2017/9/15 14:34:26 */
#define RF_PROPERTY_READONLY_FILESYSTEM_LIST         "ReadOnlyFileSystemList"
#define RF_PROPERTY_SENSE_CODE_LIST                  "SenseCodeList"

#define RF_PROPERTY_HDD_SASADDRESS            "SAS_Address"

#define RF_PROPERTY_READ_AHEAD		"READ_AHEAD"
#define RF_PROPERTY_NO_READ_AHEAD		"NO_READ_AHEAD"

#define RF_PROPERTY_WRITE_BACK		"WRITE_BACK"
#define RF_PROPERTY_WRITE_THROUGH		"WRITE_THROUGH"
#define RF_PROPERTY_WRITE_CACHE_IF_BAD_BBU		"WRITE_CACHE_IF_BAD_BBU"

#define RF_PROPERTY_CACHE_CACHED_IO   "CACHED_IO"
#define RF_PROPERTY_CACHE_DIRECT_IO   "DIRECT_IO"

#define RF_PROPERTY_ACCESS_POLICY_RW                    "RW"
#define RF_PROPERTY_ACCESS_POLICY_READ_ONLY            "Read Only"
#define RF_PROPERTY_ACCESS_POLICY_BLOCKED				"Blocked"
#define RF_PROPERTY_ACCESS_POLICY_HIDDEN				"Hidden"

#define RF_PROPERTY_DISK_CACHE_STATUS_DEFAULT          "Default"
#define RF_PROPERTY_DISK_CACHE_STATUS_ENABLE          "Enable"
#define RF_PROPERTY_DISK_CACHE_STATUS_DISABLE          "Disable"

#define RF_PROPERTY_INITSTATE_NO_INIIT                   "No Init"
#define RF_PROPERTY_INITSTATE_QUICK_INIT                 "Quick Init"
#define RF_PROPERTY_INITSTATE_FULL_INIT                      "Full Init"

#define RF_PROPERTY_YES                    "Yes"
#define RF_PROPERTY_NO                     "No"
#define RF_PROPERTY_MEMORY_SIZE           "MemorySizeMB"
#define RF_PROPERTY_DEFAULT_READ_POLICY    "DefaultReadPolicy"
#define RF_PROPERTY_CURRENT_READ_POLICY    "CurrentReadPolicy"
#define RF_PROPERTY_DEFAULT_WRITE_POLICY   "DefaultWritePolicy"
#define RF_PROPERTY_DEFAULT_CACHE_POLICY   "DefaultCachePolicy"
#define RF_PROPERTY_ACCESS_POLICY   		   "AccessPolicy"
#define RF_PROPERTY_CONSISTENCY_CHECK      "ConsistencyCheck"

#define RF_PROPERTY_CURRENT_READ_POLICY    "CurrentReadPolicy"
#define RF_PROPERTY_CURRENT_WRITE_POLICY  "CurrentWritePolicy"
#define RF_PROPERTY_CURRENT_CACHE_POLICY   "CurrentCachePolicy"

#define RF_PROPERTY_HDD_SAS_ADDR1  				"0"
#define RF_PROPERTY_HDD_SAS_ADDR2  				"1"
/* END:   Added by gwx455466, 2017/3/2*/

/* BEGIN: Added by gwx455466, 2017/4/5 14:15:51   问题单号:AR-0000276593-002-001 */
#define RF_PROPERTY_HDD_POWER_STATE  				"PowerState"
#define RF_PROPERTY_HDD_POWER_STATE_SPUN_UP				"Spun Up"
#define RF_PROPERTY_HDD_POWER_STATE_SPUN_DOWN				"Spun Down"
#define RF_PROPERTY_HDD_POWER_STATE_TRANSITION				"Transition"
#define RF_PROPERTY_NA                     "NA"
#define RF_PROPERTY_NONA                     "None"
#define RF_PROPERTY_LOGICAL_DRIVE_CACHE_CADE                     "SSCD"
/* END:   Added by gwx455466, 2017/4/5 14:15:53 */

/* BEGIN: Modified by gwx455466, 2017/3/29 20:0:44   问题单号:DTS2017031009864  */
#define RF_PROPERTY_CONTROLLER_INTERFACE_TYPE_SPI_STR     "SPI"
#define RF_PROPERTY_CONTROLLER_INTERFACE_TYPE_SAS_3G_STR     "SAS_3G"
#define RF_PROPERTY_CONTROLLER_INTERFACE_TYPE_SATA_1_5G_STR     "SATA_1_5G"
#define RF_PROPERTY_CONTROLLER_INTERFACE_TYPE_SATA_3G_STR     "SATA_3G"
#define RF_PROPERTY_CONTROLLER_INTERFACE_TYPE_SAS_6G_STR     "SAS_6G"
#define RF_PROPERTY_CONTROLLER_INTERFACE_TYPE_SAS_12G_STR     "SAS_12G"
/* 3152 raid */
#define RF_PROPERTY_CONTROLLER_INTERFACE_TYPE_SAS_32G_STR     "SAS_32G"
#define RF_PROPERTY_CONTROLLER_INTERFACE_TYPE_SAS_64G_STR     "SAS_64G"
#define RF_PROPERTY_CONTROLLER_INTERFACE_TYPE_SATA_32G_STR     "SATA_32G"
#define RF_PROPERTY_CONTROLLER_INTERFACE_TYPE_SATA_64G_STR     "SATA_64G"
/* END:   Modified by gwx455466, 2017/3/29 20:0:48 */

/* BEGIN: Added by z00352904, 2017/5/9 19:14:0  PN:DTS2017042513377 */
#define RFPROP_NETWORKPROTOCOL_HTTP                  "HTTP"
#define RFPROP_NETWORKPROTOCOL_HTTPS                 "HTTPS"
/* END:   Added by z00352904, 2017/5/9 19:14:2*/

/* END:   Added by z00352904, 2016/12/1 17:22:11*/

/* BEGIN: Added by gwx455466, 2017/6/26 14:45:1   问题单号:AR-0000276590-016-003 */
#define NTP_MINPOLL         3
#define NTP_MAXPOLL         17
/* END:   Added by gwx455466, 2017/6/26 14:45:3 */
/* BEGIN: Modified by gwx455466, 2017/8/28 11:40:23   问题单号:DTS2017073102969 */
#define NTP_DEFAULT_MINPOLL         6
#define NTP_DEFAULT_MAXPOLL         10
#define NTP_DEFAULT_BYTE      0xFF
/* END:   Modified by gwx455466, 2017/8/28 11:40:25 */

#define UPDATE_STATUS_NOT_START         0
#define UPDATE_STATUS_COMPLETE          1
#define UPDATE_STATUS_DEFAULT           2

/* BEGIN: Modified by jwx372839, 2017/8/3 10:7:47   问题单号:DTS2017080104207 */
/* BEGIN: Added by z00356691 zhongqiu, 2017/3/17   问题单号:DTS2017031102069 优化数组包含null值成员处理 */
/*注:调用本宏过程中，如果调用json_object_array_delete_idx删除了当前成员，需要将索引减一，否则会跳过一个成员*/
#define json_object_array_foreach(obj, iter) \
    struct array_list* iter ## a = json_object_get_array(obj); \
    int iter ## _i ;\
    iter = NULL;\
    if (iter ## a)\
        for (iter ## _i = 0;(iter ## _i < iter ## a->length) ? (iter = (json_object *)iter ## a->array[iter ## _i], TRUE) : 0;iter ## _i++)
/* END: Added by z00356691 zhongqiu, 2017/3/17   问题单号:DTS2017031102069 优化数组包含null值成员处理 */
/* END:   Modified by jwx372839, 2017/8/3 10:7:52 */
/* BEGIN: Added by gwx455466, 2017/8/2 11:8:33   问题单号:AR-SREA02109352-001-002 */
#define RF_PROPERTY_SFF                    "Sff"

#define RF_PROPERTY_VENDOR_NAME                     "VendorName"
#define RF_PROPERTY_PART_NUMBER                     "PartNumber"
#define RF_PROPERTY_SERIAL_NUMBER                   "SerialNumber"
#define RF_PROPERTY_MANUFACTURE_DATE                "ManufactureDate"
#define RF_PROPERTY_TRANSCEIVER_TYPE                "TransceiverType"
#define RF_PROPERTY_WAVE_LENGTH                      "Wavelength"
#define RF_PROPERTY_MEDIUM_MODE                     "MediumMode"
#define RF_PROPERTY_SPEED                           "speed"
#define RF_PROPERTY_TYPE                            "type"
#define RF_PROPERTY_TYPE_MATCH                      "TypeMatch"
#define RF_PROPERTY_SPEED_MATCH                     "SpeedMatch"
#define RF_PROPERTY_IS_SFP_EXIST                    "IsSffExist"
#define RF_PROPERTY_DIAG_STATUS                    "DiagnosticStatus"

#define RF_PROPERTY_VCC_HIGH_WARN_VOLTS             "VccHighWarnVolts"
#define RF_PROPERTY_TX_POWER_HIGH_ALARM_MILLIWATT   "TxPowerHighAlarmMilliwatt"
#define RF_PROPERTY_TX_BIAS_HIGH_ALARM_MILLIAMPERE  "TxBiasHighAlarmMilliampere"
#define RF_PROPERTY_TEMP_LOW_ALARM_CELSIUS          "TempLowAlarmCelsius"
#define RF_PROPERTY_VCC_HIGH_ALARM_VOLTS            "VccHighAlarmVolts"
#define RF_PROPERTY_VCC_CURRENT_VOLTS               "VccCurrentVolts"
#define RF_PROPERTY_RX_POWER_CURRENT_MILLIWATT      "RxPowerCurrentMilliwatt"
#define RF_PROPERTY_TX_POWER_CURRENT_MILLIWATT      "TxPowerCurrentMilliwatt"
#define RF_PROPERTY_TX_BIAS_HIGH_WARN_MILLIAMPERE   "TxBiasHighWarnMilliampere"
#define RF_PROPERTY_TEMP_LOW_WARN_CELSIUS           "TempLowWarnCelsius"
#define RF_PROPERTY_TX_BIAS_LOW_WARN_MILLIAMPERE    "TxBiasLowWarnMilliampere"
#define RF_PROPERTY_TEMP_HIGH_WARN_CELSIUS          "TempHighWarnCelsius"
#define RF_PROPERTY_TX_POWER_LOW_WARN_MILLIWATT     "TxPowerLowWarnMilliwatt"
#define RF_PROPERTY_TX_POWER_HIGH_WARN_MILLIWATT    "TxPowerHighWarnMilliwatt"
#define RF_PROPERTY_RX_POWER_HIGH_WARN_MILLIWATT    "RxPowerHighWarnMilliwatt"
#define RF_PROPERTY_TX_BIAS_LOW_ALARM_MILLIAMPERE   "TxBiasLowAlarmMilliampere"
#define RF_PROPERTY_VCC_LOW_WARN_VOLTS              "VccLowWarnVolts"
#define RF_PROPERTY_RX_POWER_HIGH_ALARM_MILLIWATT   "RxPowerHighAlarmMilliwatt"
#define RF_PROPERTY_VCC_LOW_ALARM_VOLTS             "VccLowAlarmVolts"
#define RF_PROPERTY_RX_POWER_LOW_ALARM_MILLIWATT    "RxPowerLowAlarmMilliwatt"
#define RF_PROPERTY_RX_POWER_LOW_WARN_MILLIWATT     "RxPowerLowWarnMilliwatt"
#define RF_PROPERTY_TX_POWER_LOW_ALARM_MILLIWATT    "TxPowerLowAlarmMilliwatt"
#define RF_PROPERTY_TEMP_HIGH_ALARM_CELSIUS         "TempHighAlarmCelsius"
#define RF_PROPERTY_TX_BIAS_CURRENT_MILLIAMPERE     "TxBiasCurrentMilliampere"
#define RF_PROPERTY_TEMP_CURRENT_CELSIUS            "TempCurrentCelsius"

#define RF_PROPERTY_CHANNEL_NUM "ChannelNum"

/* END:   Added by gwx455466, 2017/8/2 11:8:38 */
/* BEGIN: Added by gwx455466, 2017/8/26 10:51:36   问题单号:SREA02109352-001-003 */
#define RF_PROPERTY_OAM                    "OAM"

#define RF_PROPERTY_OAM_IS_OAM_ENABLED                    "IsOAMEnabled"
#define RF_PROPERTY_OAM_LOST_LINK_STATE                    "OAMLostLinkState"
#define RF_PROPERTY_OAM_ERRPKT_COUNT              "OAMErrPktCnt"
/* END:   Added by gwx455466, 2017/8/26 10:51:40 */
/* BEGIN: Modified by gwx455466, 2017/10/19 14:1:29   问题单号:DTS2017092902173 */
#define RF_INFO_INVALID_DWORD     0xFFFFFFFF
#define RF_INFO_INVALID_DOUBLE    0xFFFFFFFFFFFFFFFFULL
/* END:   Modified by gwx455466, 2017/10/19 14:1:32 */

/* BEGIN: Added by gwx455466, 2018/1/8 10:34:45   问题单号:AR.SR.SFEA02109778.006.003 */
#define  RF_LOGSERVICE_LOGDATA "LogData"
#define  RF_LOGSERVICE_LOGDATA_TYPE "Type"
#define  RF_LOGSERVICE_LOGDATA_TYPE_SMART "SMART"
#define  RF_LOGSERVICE_FILE_SUFFIX  "FileSuffix"
/* END:   Added by gwx455466, 2018/1/8 10:34:48 */

/* BEGIN: Added by fwx527488, 2018/11/9 ,支持通过MCTP获取1822芯片的HBA卡信息*/
#define RFPROP_ETH_PORT_STATISTIC                  "StatisticInfo"

#define RF_PROPERTY_TX_SN_SPEED    "TXRateGbps"
#define RF_PROPERTY_RX_SN_SPEED    "RXRateGbps"
#define RF_PROPERTY_SN_STAGE       "RateNegotiationStage"
#define RF_PROPERTY_SFP_OPEN       "OpticalModuleEnabled"
#define RF_PROPERTY_WORK_SPEED     "WorkingRate"
#define RF_PROPERTY_WORK_TOPO      "WorkMode"
#define RF_PROPERTY_TX_BB_CREDIT   "PeerDeviceCredit"
#define RF_PROPERTY_RX_BB_CREDIT   "LocalDeviceCredit"
#define RF_PROPERTY_PORT_STATUS    "PortStatus"
/* END: Added by fwx527488, 2018/11/9 */

/* BEGIN: Added by fwx527488, 2018/11/19 ,支持通过MCTP获取1822芯片的HBA卡信息*/
#define    SN_STAGE_NEGOTIATING             "Negotiating"
#define    SN_STAGE_WAITING_SIGNAL          "WaitSignal"
#define    SN_STAGE_NEGOTIATION_COMPLETED   "NegotiationCompleted"
#define    WORK_MODE_LOOP                   "Loop"
#define    WORK_MODE_NON_LOOP               "NonLoop"
#define    PORT_STATUS_LINK_UP              "LinkUp"
#define    PORT_STATUS_DISABLED_STR         "Disabled"
#define    PORT_STATUS_WAITING_SIGNAL       "WaitSignal"

typedef enum {
    SN_STAGE_WAITING_SIGNAL_INT0 = 0,
    SN_STAGE_WAITING_SIGNAL_INT1,
    SN_STAGE_NEGOTIATING_INT2,
    SN_STAGE_NEGOTIATING_INT3,
    SN_STAGE_NEGOTIATING_INT4,
    SN_STAGE_NEGOTIATION_COMPLETED_INT
}ENUM_SN_STAGE_NEGOTIATING_INT;

typedef enum {
    NOT_WORK = 0,
    WORK_MODE_LOOP_INT,
    WORK_MODE_NON_LOOP_INT
}ENUM_WORK_MODE_INT;

#define    OPTICAL_MODULE_DISABLED              0
#define    OPTICAL_MODULE_ENABLED               1


#define    PORT_STATUS_DISABLED_INT             0
#define    PORT_STATUS_LINK_UP_INT              1
#define    PORT_STATUS_WAITING_SIGNAL_INT       4

#define    ZERO_DATA                            0
/* END: Added by fwx527488, 2018/11/19 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif

