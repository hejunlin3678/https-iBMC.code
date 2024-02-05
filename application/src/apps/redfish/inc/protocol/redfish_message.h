

#ifndef __REDFISH_MESSAGE_H__
#define __REDFISH_MESSAGE_H__

#include "json.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 




#define MESSAGE_KEY "Message"
#define MESSAGE_ID_KEY "MessageId"
#define RELATED_PROP_KEY "RelatedProperties"
#define CODE_KEY "code"
#define SEVERITY_KEY "Severity"
#define RESOLUTION_KEY "Resolution"
#define NUMBER_OF_ARGS_KEY "NumberOfArgs"
#define T_MESSAGE_KEY "message"
#define CODE_VAL_GENERAL_STRING "Base.1.0.GeneralError"
#define MESSAGE_VAL_GENERAL_STRING "A general error has occurred. See ExtendedInfo for more information."


#define PARAM_TYPES_KEY "ParamTypes"


#define REDFISH_BASE_DIR ("/data/opt/pme/conf/redfish/v1/registrystore/messages/en/base.v1_0_0.json")
#define REDFISH_IBMC_MSG_DIR ("/data/opt/pme/conf/redfish/v1/registrystore/messages/en/ibmc.v1_0_0.json")


// Message的最大长度设置为比MessageAsgs最大长度大128
#define MESSAGE_MAX_LENGTH 640


// redfish 官方错误类型MessageId
#define MSGID_SUCCESS "Success"
#define MSGID_GENERAL_ERROR "GeneralError"
#define MSGID_CREATED "Created"
#define MSGID_PROP_DUPLICATE "PropertyDuplicate"
#define MSGID_PROP_UNKNOWN "PropertyUnknown"
#define MSGID_PROP_TYPE_ERR "PropertyValueTypeError"
#define MSGID_PROP_FORMAT_ERR "PropertyValueFormatError"
#define MSGID_PROP_NOT_IN_LIST "PropertyValueNotInList"
#define MSGID_PROP_NOT_WRITABLE "PropertyNotWritable"
#define MSGID_PROP_MISSING "PropertyMissing"
#define MSGID_MALFORMED_JSON "MalformedJSON"
#define MSGID_ACT_NOT_SUPPORTED "ActionNotSupported"
#define MSGID_ACT_PARA_MISSING "ActionParameterMissing"
#define MSGID_ACT_PARA_DUPLICATE "ActionParameterDuplicate"
#define MSGID_ACT_PARA_UNKNOWN "ActionParameterUnknown"
#define MSGID_ACT_PARA_TYPE_ERR "ActionParameterValueTypeError"
#define MSGID_ACT_PARA_FORMAT_ERR "ActionParameterValueFormatError"
#define MSGID_ACT_PARA_NOT_SUPPORTED "ActionParameterNotSupported"
#define MSGID_QUERY_PARA_TYPE_ERR "QueryParameterValueTypeError"
#define MSGID_QUERY_PARA_FORMAT_ERR "QueryParameterValueFormatError"
#define MSGID_QUERY_OUT_OF_RANGE "QueryParameterOutOfRange"
#define MSGID_QUERY_NOT_SUPPORTED_RSC "QueryNotSupportedOnResource"
#define MSGID_QUERT_NOT_SUPPORTED "QueryNotSupported"
#define MSGID_SESSION_LIMIT_EXCEED "SessionLimitExceeded"
#define MSGID_EVENT_LIMIT_EXCEED "EventSubscriptionLimitExceeded"
#define MSGID_RSC_CANNOT_DELET "ResourceCannotBeDeleted"
#define MSGID_RSC_IN_USE "ResourceInUse"
#define MSGID_RSC_ALREADY_EXIST "ResourceAlreadyExists"
#define MSGID_CREATE_FAILED_REQ_PROP "CreateFailedMissingReqProperties"
#define MSGID_CREATE_LIMIT_RSC "CreateLimitReachedForResource"
#define MSGID_SVC_SHUT_DOWN "ServiceShuttingDown"
#define MSGID_SVC_UNKNOWN_STATE "ServiceInUnknownState"
#define MSGID_NO_VALID_SESSION "NoValidSession"
#define MSGID_INSUFFICIENT_PRIVILEGE "InsufficientPrivilege"
#define MSGID_ACCOUNT_MODIFIED "AccountModified"
#define MSGID_ACCOUNT_NOT_MODIFIED "AccountNotModified"
#define MSGID_ACCOUNT_REMOVED "AccountRemoved"
#define MSGID_ACCOUNT_NO_EXIST "AccountForSessionNoLongerExists"
#define MSGID_INVALID_OBJ "InvalidObject"
#define MSGID_INTERNAL_ERR "InternalError"
#define MSGID_UNRECOGNIZED_REQ_BODY "UnrecognizedRequestBody"
#define MSGID_RSC_MISSING_URI "ResourceMissingAtURI"
#define MSGID_RSC_URI_UNKNOWN_FROMAT "ResourceAtUriInUnknownFormat"
#define MSGID_RSC_URI_UNAUTHORIZED "ResourceAtUriUnauthorized"
#define MSGID_COULD_NOT_CONNECT "CouldNotEstablishConnection"
#define MSGID_SRC_NOT_SUPPORT_PROTOCOL "SourceDoesNotSupportProtocol"
#define MSGID_ACCESS_DENIED "AccessDenied"
#define MSGID_SVC_TEMP_UNAVALIBLE "ServiceTemporarilyUnavailable"
#define MSGID_INVALID_IDX "InvalidIndex"
#define MSGID_PROP_VAL_MODIFIED "PropertyValueModified"
#define MSGID_ACCOUNT_MUST_RESET_PASSWORD "AccountMustResetPassword"
#define MSGID_MODIFY_SNMP_PROTOCOL_NOT_MATCH "PrivProtocolAes256NeedMatch"
#define MSGID_MODIFY_SNMP_LACK_PROPERTY "ModifyAuthProtocolLackProp"
#define MSGID_UNENABLED_NETWORK_PORT "UnEnableNetworkPort"
#define MSGID_MODIFY_FAIL_LACK_PROPERTY "ModifyFailedMissingReqProperties"
#define MSGID_STRING_VALUE_TOO_LONG "StringValueTooLong"
// redfish BMC自定义错误类型MessageId


#define MSGID_PORT_ENABLE_OK "SetPortEnableOK"

#define MSGID_RACK_POWER_CAPPING_DISABLED "RackPowerCappingDisabled"
#define MSGID_RACK_BATTERY_NOT_CONFIGURATION "RackBatteryNotConfiguration"

#define MSGID_UNSUPPORT_TO_REFRESH_ANOTHER_SESSION "UnsupportToRefreshAnotherSession"
#define MSGID_SESSION_TIMEOUT "SessionTimeout"
#define MSGID_SESSION_KICKOUT "SessionKickout"
#define MSGID_SESSION_RELOGIN "SessionRelogin"
#define MSGID_SESSION_CHANGED "SessionChanged"
#define MSGID_INVALID_RECORD_ID "InvalidRecordId"
#define MSGID_RECORDS_EXC_MAXNUM "RecordExceedsMaxNum"
#define MSGID_FILE_NOT_EXIST "FileNotExist"
#define MSGID_INVALID_USERNAME "InvalidUserName"
#define MSGID_PROP_VAL_NOT_EMPTY "PropertyValueNotEmpty"
#define MSGID_PROP_VAL_EXC_MAXLEN "PropertyValueExceedsMaxLength"
#define MSGID_BODY_EXC_MAXLEN "BodyExceedsMaxLength"
#define MSGID_INVALID_PSWD_LEN "InvalidPasswordLength"
#define MSGID_INVALID_PSWD_SAME_HISTORY "InvalidPasswordSameWithHistory"
#define MSGID_INVALID_PAWD "InvalidPassword"
#define MSGID_PSWD_CMPLX_CHK_FAIL "PasswordComplexityCheckFail"
#define MSGID_DURING_MIN_PSWD_AGE "DuringMinimumPasswordAge"
#define MSGID_USER_IS_LOGGED_IN "UserIsLoggingIn"
#define MSGID_EMRGNCY_LOGIN_USER "EmergencyLoginUser"
#define MSGID_DISABLE_LAST_ADMIN "CannotDisableLastAdministrator"
#define MSGID_DELETE_LAST_ADMIN "CannotDeleteLastAdministrator"
#define MSGID_CONFLICT_WITH_LINUX_DEFAULT_USER "ConflictWithLinuxDefaultUser"
#define MSGID_CONFILICT_WITH_CREATE_ACCOUNTS "ConflictWithCreateAccounts"
#define MSGID_PSWD_NOT_STSFY_CMPLX "PasswordNotSatisfyComplexity"
#define MSGID_LOGIN_ATTEMPT_FAILED "LoginFailed"
#define MSGID_LOGIN_AUTH_FAILED "AuthorizationFailed"
#define MSGID_NO_RESTORE_POINT "NoRestorePoint"

#define MSGID_AUTH_USER_NO_ACCESS "AuthorizationUserNoAccess"
#define MSGID_AUTH_USER_PSWD_EXPIRED "AuthorizationUserPasswordExpired"
#define MSGID_AUTH_USER_RESTRICTED "AuthorizationUserRestricted"
#define MSGID_AUTH_USER_LOCKED "AuthorizationUserLocked"

#define MSGID_NO_ACCESS "NoAccess"
#define MSGID_USER_LOCKED "UserLocked"
#define MSGID_LDAP_AUTH_FAIL "UserLdapAuthFail"
#define MSGID_USER_PSWD_EXPIRED "UserPasswordExpired"
#define MSGID_USER_LOGIN_RESTRICTED "UserLoginRestricted"
#define MSGID_PROP_MODIFY_NEED_PRIV "PropertyModificationNeedPrivilege"
#define MSGID_ACCOUNT_FORBID_REMOVED "AccountForbidRemoved"
#define MSGID_ACCOUNT_MODIFICATION_RESTRICTED "AccountInsecurePromptModificationRestricted"
#define MSGID_PROP_ITEM_TYPE_ERR "PropertyItemTypeError"
#define MSGID_PROP_ITEM_DUPLICATE "PropertyItemDuplicate"
#define MSGID_PROP_ITEM_NOT_IN_LIST "PropertyItemNotInList"
#define MSGID_PROP_ITEM_MISSING "PropertyItemMissing"
#define MSGID_PROP_ITEM_FORMAT_ERR "PropertyItemFormatError"
#define MSGID_ACT_FAILED_BY_POWER_OFF "ActionFailedByPowerOff"
#define MSGID_USERNAME_IS_RESTRICTED "UserNameIsRestricted"
#define MSGID_ROLEID_IS_RESTRICTED "RoleIdIsRestricted"

#define MSGID_PSWD_IN_WEAK_PWDDICT "PasswordInWeakPWDDict"

#define MSGID_USER_FORBID_SET_PASS_COMPLEXITY_CHECK "PasswordForbidSetComplexityCheck"

#define MSGID_POLL_INTERVAL_FAILED "SettingPollingIntervalFailed"

#define USBSTICK_MESSAGE_DOUBECONNECT "DoubleConnection"
#define USBSTICK_MESSAGE_DOUBEDISCONNECT "DoubleDisonnection"

#define USB_MESSAGE_CONNECT_FAILED "USBStickMountedFailed"
#define USB_MESSAGE_DIS_FAILED "USBStickDismountedFailed"
#define USB_MESSAGE_DISMOUNTED "USBStickDismounted"
#define USB_MESSAGE_MOUNTED "USBStickMounted"

#define MSGID_FILE_NOT_EXIST "FileNotExist"

#define MSGID_PROP_INVALID_VALUE "InvalidValue"
#define MSGID_PROP_NOT_REQUIRED "UnrequiredProperty"

#define MSGID_INVALID_HOSTNAME "InvalidHostName"
#define MSGID_INVALID_IPV4ADDRESS "InvalidIPv4Address"
#define MSGID_INVALID_IPV6ADDRESS "InvalidIPv6Address"
#define MSGID_EMPTY_IPV6_ADDRESS "EmptyIPv6Address"
#define MSGID_IPV6_ADDRESS_CONFLICT_GATEWAY "IPv6AddressConflictWithGateway"
#define MSGID_INVALID_SUBNETMASK "InvalidSubnetMask"
#define MSGID_INVALID_IPV4GATEWAY "InvalidIPv4Gateway"

#define MSGID_IPV4_ADDRESS_CONFLCT "IPv4AddressConflict"

#define MSGID_INVALID_IPV6GATEWAY "InvalidIPv6Gateway"
#define MSGID_INVALID_NAMESERVER "InvalidNameServer"
#define MSGID_IPV4_INFO_CONFLICT_DHCP "IPv4InfoConflictwithDHCP"
#define MSGID_IPV6_INFO_CONFLICT_DHCPV6 "IPv6InfoConflictwithDHCPv6"
#define MSGID_IPV4_MASK_CONFLICT_DHCP "IPv4SubnetMaskConflictWithDHCP"
#define MSGID_IPV4_GATEWAY_CONFLICT_DHCP "IPv4GatewayConflictWithDHCP"
#define MSGID_IPV6_PREFIX_CONFLICT_DHCPV6 "IPv6PrefixConflictWithDHCPv6"
#define MSGID_IPV6_GATEWAY_CONFLICT_DHCPV6 "IPv6GatewayConflictWithDHCPv6"
#define MSGID_VLAN_INFO_CONFLICT "VLANInfoConflict"
#define MSGID_PROP_MODIFY_UNSUPPORT "PropertyModificationNotSupported"
#define MSGID_TLS_VERSION_CONFIGURATION_UNSUPPORT "TLSVersionConfigurationNotSupported"
#define MSGID_MODIFY_TLS_FAIL_WITH_TWO_FACTORCERTIFICATION "TLSFailedWithTwoFactorCertification"
#define MSGID_ENABLE_TWO_FACTORCERTIFICATION_WITH_TLS "TwoFactorCertificationFailedWithTLS"
#define MSGID_PORTID_NOT_MODIFIED "PortIdModificationFailed"

#define MSGID_MODIFY_FAILED_EMPTY_ADDRESS "ModifyfailedWithEmptyAddress"

#define MSGID_RSC_CANNOT_CREATED "ResourceCannotBeCreated"
#define MSGID_MODIFY_FAIL_WITH_RAID_NOT_SUPPORT "ModifyfailedWithRaidControllerNotSupport"
#define MSGID_MODIFY_FAIL_WITH_CACHECADE "ModifyfailedWithCachecade"
#define MSGID_NO_CACHECADE_VOLUME "NoCachecadeVolume"
#define MSGID_MODIFY_FAIL_WITH_SSD "ModifyfailedWithSSD"

#define MSGID_INVALID_VOLUME_NAME "InvalidVolumeName"


#define MSGID_MODIFY_FAIL_CONTROLLER_NOT_SUPPORT "RAIDControllerNotSupported"

#define MSGID_CONFIGURATION_EXIST "ConfigurationExist"
#define MSGID_TASK_LIMIT_EXCEED "TaskLimitExceeded"
#define MSGID_FILE_TRANSFER_PROTOCOL_MISMATCH "FileTransferProtocolMismatch"
#define MSGID_FILE_TRANSFER_PROGRESS "FileTransferProgress"
#define MSGID_FW_UPGRADING "FirmwareUpgrading"
#define MSGID_FW_UPGRADE_ERROR "FirmwareUpgradeError"
#define MSGID_FILE_DOWNLOAD_TASK_OCCUPIED "FileDownloadTaskOccupied"
#define MSGID_FILE_TRANSFER_ERR_DESC "FileTransferErrorDesc"
#define MSGID_FW_UPGRADE_COMPONENT "FirmwareUpgradeComponent"

#define MSGID_FW_UPGRADE_COMPLETE "FirmwareUpgradeComplete"

#define MSGID_VOLUME_INITIALIZING "VolumeInitializing"
#define MSGID_CSR_GENERATING_ERR "CSRGeneratingErr"
#define MSGID_CSR_GEN_FAILED "CSRGenFailed"
#define MSGID_CERT_IMPORT_OK "CertImportOK"

#define MSGID_CERT_IMPORT_OK_NONE "CertImportOKNone"

#define MSGID_CERT_MISMATCH "CertMismatch"
#define MSGID_CERT_IMPORT_FAILED "CertImportFailed"
#define MSGID_CERT_EXPIRED "CertificateExpired"
#define MSGID_CERT_NO_MATCH_DEVICE "CertificateNotMatchDevice"


#define MSGID_ENCRYPTED_CERT_IMPORT_FAILED "EncryptedCertImportFailed"

#define MSGID_CA_MD_TOO_WEAK "EncryptionAlgorithmIsWeak"
#define MSGID_PROTOCOL_NOT_SUPPORTED "ProtocolNotAllowed"
#define MSGID_PROPERTY_CONTROLLER_INVAID "StorageControllerIDInvaid"
#define MSGID_CMD_NOT_SUPPORTED "CommandNotSupproted"
#define MSGID_FRU_NOT_EXIST "FruNotExist"

#define MSGID_PROPERTY_RAIDLEVEL_INVAID "PropertyModificationFailedByRAIDlevelNotSupported"
#define MSGID_PROPERTY_PD_STATE_NOT_SUPPORT "PropertyModificationNotSupportedByPDStatus"
#define MSGID_PROPERTY_BBU_STATE_NOT_SUPPORT "PropertyModificationNotSupportedByBBUStatus"
#define MSGID_PROPERTY_PD_NOT_SUPPORTSETSAMETIME "PropertyConflict"


#define MSGID_PROPERTY_PD_LOCATE_OR_REBUILDING "LocatedFailedByPDRebuilding"
#define MSGID_PROPERTY_STATUS_UNSUPPORT_MODIF "PropertyNotSupportedModificationByCurrentStatus"



#define MSGID_PROPERTY_PD_INTERFACE_MIXED "PropertyModificationFailedByIterfaceConflict"
#define MSGID_PROPERTY_PD_MEDIAMIXED "PropertyModificationFailedByMediaTypeConflict"



#define MSGID_VALUE_OUT_OF_RANGE "ValueOutOfRange"
#define MSGID_RAID_CONTROLLER_ID_INVALID "RaidControllerIdInvalid"
#define MSGID_STORAGE_MEDI_ERR "StorageMediaInconsistent"
#define MSGID_PHY_DISK_INTERFACES_ERR "DriveInterfacesInconsistent"
#define MSGID_DIFF_DRIVES_SECTOR_SIZE "DifferentDriveSectorsSize" 
#define MSGID_PHY_STATUS_ERR "DriveStatusNotSupported"
#define MSGID_PHY_LIST_DOUBLE_ERR "DriveListDuplicateValue"
#define MSGID_RAID_LEVEL_ERR "RaidControllerLevelInvalid"
#define MSGID_RAID_NOT_SUPPORTED "RAIDControllerNotSupportedOperation"
#define MSGID_SPAN_NUMBER_ERR "NumberOfSpansInvalid"
#define MSGID_SPAN_DISK_ERR "NumberofDrivesForEachSpanInvalid"
#define MSGID_RAID_VOLUMES_MAX "RAIDReachedVolumesNumberLimit"
#define MSGID_ARRAY_SPACE_ERR "ArraySpaceInsufficient"
#define MSGID_NO_ARRAY_SPACE "ArraySpaceNoFree"
#define MSGID_PHY_NOT_SSD "NonSSDExist"
#define MSGID_VOLUME_CAPACITY_ERR "VolumeCapacityOutRange"
#define MSGID_ASSOCIATED_VOLUME_CAPACITY_ERR "AssociatedVolumeCapacityOutRange"
#define MSGID_PHY_DISK_COUNT_ERR "PhysicalDiskIdCountError"
#define MSGID_STRIPE_SIZE_ERR "StripeSizeError"
#define MSGID_STATUS_NOT_SUPPORT "CurrentStatusNotSupport"
#define MSGID_OPERATION_NOT_SUPPORTED "OperationNotSupported"
#define MSGID_VOLUME_CREATE_SUCCESS "VolumeCreationSuccess"
#define MSGID_VOLUME_DELETE_SUCCESS "VolumeDeletionSuccess"
#define MSGID_VOLUME_SHRINK_NOT_ALLOWED "VolumeShrinkNotAllowed"
#define MSGID_PROPERTY_VALUE_ERR "PropertyValueError"

#define MSGID_PHY_DISK_ID_INVALID "PhysicalDiskIdInvalid"

#define MSGID_RCP_NOT_IN_RANGE "ModifyfailedWithRCPNotInRange"
#define MSGID_CRYTO_ERASE_NOT_SUPPORTED "CryptoEraseNotSupported"


#define MSGID_DUMP_COLLECT_ERR_DESC "DumpingErrorDesc"
#define MSGID_COLLECTING_ERR_DESC "CollectingConfigurationErrorDesc"
#define MSGID_DUMPING_OK "DumpingOK"
#define MSGID_COLLECTING_CONFIG_OK "CollectingConfigurationOK"
#define MSGID_POWER_ON_PERMIT_OK "PowerOnPermitOK"
#define MSGID_IMPORT_CONFIG_OK "ImportConfigOK"



#define MSGID_COLLECTING_OK "CollectingFileOK"





#define MSGID_POWER_ACTIVE_PS_NECESSARY "ActivePsNecessary"
#define MSGID_POWER_ACTIVE_PS_NOT_NECESSARY "ActivePsNotNecessary"
#define MSGID_POWER_ACTIVE_PS_NAME_ERROR "ActivePsNameError"
#define MSGID_POWER_ACTIVE_PS_EXCEEDS_NUM "ActivePsExceedsNum"
#define MSGID_POWER_ACTIVE_STANDBY_DISABLED "ActiveStandbyDisabled"
#define MSGID_POWER_INVALID_ODATA_ID "InvalidOdataId"
#define MSGID_POWER_EXPECTED_REDUNDANCY_UNSET "ExpectedRedundancyUnset"

#define MSGID_POWER_ACTIVE_PS_NOT_PST "ActivePsNotPosition"
#define MSGID_POWER_NO_STANDBY_PS_PST "NoStandbyPsPosition"
#define MSGID_POWER_ACTIVE_PS_NOT_HLS "ActivePsNotHealth"
#define MSGID_POWER_CURRENT_POWER_HIGN "CurrentPowerHigh"
#define MSGID_POWER_ACTIVE_PS_NUM_NE "ActivePsNotEnough"
#define MSGID_POWER_PS_NOT_SUPPORT "PsNotSupport"
#define MSGID_POWER_PS_NOT_SUPPORT_NAR "PsNotSupportNAR"
#define MSGID_POWER_ACTIVE_STANDBY_MODE_UNSUPPORT "ActiveStandbyModeUnsupport"
#define MSGID_POWER_DUP_ACTIVE_PSU "DuplicateElemInActivePSU"


#define MSGID_POWER_NAR_VOLTAGE_NOT_MATCH "VoltageOfRedundancyPsNotMatch"


#define MSGID_MODIFED_WITH_OS_STATE "ModifyfailedWithRAIDCardState"


#define MSGID_VOLUME_NUNECESSARY_PROP "UnnecessaryPropWhenCreateVol"


#define MSGID_CURRENT_STATUS_NOT_SUPPORT "CurrentStatusNotSupportOperation"

#define MSGID_MAX_LEN 64


#define MSGID_EVENT_MSG_ID_INVALID "InvalidEventMsgId"
#define MSGID_EVENT_ORIGIN_RSC "InvalidEventOrigin"
#define MSGID_SUBMIT_TEST_EVENT_FAILED "SubmitTestEventsFailed"
#define MSGID_ARRAY_REQUIRED_ITEM "ArrayPropertyInvalidItem"
#define MSGID_INVALID_EVT_DESTINATION "InvalidEventDestination"
#define MSGID_INVALID_EVT_DRP "InvalidDeliveryRetryPolicy"
#define MSGID_ARRAY_ITEM_EXCEED "PropertyMemberQtyExceedLimit"
#define MSGID_PROP_NAME_FORMAT_ERROR "PropertyNameFormatError"
#define MSGID__EVENT_SUBCRIPTION_DISABLED "DisabledEventSubscription"






#define MSGID_CLICERT_IMPORT_OK "ClientCertificateImportSuccess"
#define MSGID_CLICERT_CERT_EXSIT "ClientCertificateAlreadyExists"
#define MSGID_CLICERT_NO_ISSUER "ClientCertIssuerMismatch"
#define MSGID_CLICERT_DELETE_OK "ClientCertificateDeleteSuccess"
#define MSGID_CLICERT_DELETE_MISSING_ERR "ClientCertificateNotExist"



#define MSGID_CLICERT_REACH_MAX "ClientCertQtyExceedLimit"
#define MSGID_CLICERT_CONSTRAINTS_ERR "ClientCertLackProperties"
#define MSGID_IMPORT_PUBLIC_KEY_OK "ImportPublickeyOK"
#define MSGID_DELETE_PUBLIC_KEY_OK "DeletePublicKeyOK"
#define MSGID_IMPORT_PUBLIC_KEY_ERR "PublicKeyImportError"
#define MSGID_DETETE_PUBLIC_KEY_MISSING_ERR "PublicKeyNotExist"
#define MSGID_IMPORT_CLICERT_NOT_SUPPORTED_ERR "ClientCertImportNotSupported"
#define MSGID_DELETE_CLICERT_NOT_SUPPORTED_ERR "ClientCertDeleteNotSupported"
#define MSGID_SET_LOGIN_INTERFACE_DUPLICATE_ERR "DuplicateLoginInterface"




#define MSGID_TASK_SSH_IMPORT_ERR "SSHPublickeyImportingErr"
#define MSGID_TASK_PUBLIC_KEY_IMPORT_ERR "PublicKeyImportFailed"


#define MSGID_CONFIG_PORT_LLDP_ERR      "ConfigPortLLDPErr"
#define MSGID_PCIE_PORT_NOT_ENABLED_ERR      "PCIePortEnabledErr"


#define MSGID_TASK_DUMP_EXPORT_ERR "DumpExportingErr"
#define MSGID_TASK_CONFIG_EXPORT_ERR "ConfigurationExportingErr"
#define MSGID_TASK_CONFIG_IMPORT_ERR "ConfigurationImportingErr"
#define MSGID_TASK_CSR_EXPORT_ERR "CSRExportingErr"
#define MSGID_INVALID_PATH "InvalidPath"

#define MSGID_TASK_DUPLICATE_EXPORT_ERR "DuplicateExportingErr"


#define MSGID_TASK_UPDATE_SCHEMA_FAILED "UpdateSPSchemaFileFailed"


#define MSGID_TASK_DUMP_EXPORT_UPGRADING_ERR "UpgradingErr"
#define MSGID_TASK_DUMP_EXPORT_NO_MEMORY_ERR "InsufficientMemoryErr"

#define MSGID_FEATURE_BE_DISABLED "FeatureDisabledAndNotSupportOperation"

#define MSGID_FAN_ATTRIBUTE_CONFLICT "AttributeValueConflict"
#define MSGID_CERTIFICATE_UPLOAD_SUCCESS "RootCertificateImportSuccess"
#define MSGID_CERTIFICATE_DELETE_SUCCESS "RootCertificateDeleteSuccess"
#define MSGID_CERTIFICATE_2FA_NOT_SUPPORTED "2FANotSupported"
#define MSGID_CERTIFICATE_FILE_UPLOAD_NOT_SUPPORTED "FileUploadNotSupported"



#define MSGID_CUSTOM_INVALID "WrongValueQty"               //  元素个数减一的关系
#define MSGID_CUSTOM_TEM_INVALID "InvalidTemperatureRange" // 数组没有满足递增
#define MSGID_CUSTOM_FAN_SPEED_INVALID "InvalidFanSpeed"   // 数组没有满足递增


#define MSGID_CERTIFICATE_EXISTS "RootCertificateAlreadyExists"
#define MSGID_CERTIFICATE_FORMAT_ERR "CertificateFormatError"
#define MSGID_CA_CERTIFICATE_FORMAT_ERR "CACertificateFormatError"
#define MSGID_CERTIFICATE_CONSTRAINTS_ERR "RootCertLackProperties"
#define MSGID_CA_CERTIFICATE_CONSTRAINTS_ERR "CACertificateLackProperties"


#define MSGID_CERTIFICATE_FULL "CertificateQtyReachLimit"
#define MSGID_CERTIFICATE_WRONG_ID "WrongCertificateID"
#define MSGID_EMERGENCY_USER_FAILED "EmergencyLoginUserSettingFail"


#define MSGID_CERTIFICATE_NOT_EXIST "RootCertificateNotExist"

#define MSGID_CA_CERTIFICATE_NOT_EXIST "RootCANotExists"
#define MSGID_CA_CERTIFICATE_ALREADY_EXISTS "RootCAAlreadyExists"
#define MSGID_CA_EXCEED_LIMIT_ERR "RootCAQuantityExceedLimit"

#define MSGID_CERTIFICATE_FAILED_ENABLE_CERTIFICATE "FailedEnableTwoFactorCertification"


#define MSGID_VIDEOLINK_NOT_EXIST "VideoLinkNotExist"
#define MSGID_MODIFY_FAILED_WITH_IPVER "ModifyFailedWithIPVersion"
#define MSGID_MODIFY_FAILED_WITH_AUTO_DNS "ModifyFailedWithDNSAutoMode"
#define MSGID_MODIFY_FAILED_WITH_AUTO_NETMODE "NetModeNotSupportActivePort"
#define MSGID_PORT_NOT_EXIST "PortNotExist"
#define MSGID_NOT_ACTIVE_NETCARD "NotActiveNetCard"
#define MSGID_STATIC_IP_MODE "StaticIPMode"
#define MSGID_CARD_NOT_SUPPORT_ADAPTIVE "NetcardNotSupportAdaptive"
#define MSGID_NETMODE_NOT_SUPPORT_ADAPTIVE "NetmodeNotSupportAdaptive"
#define MSGID_NET_PORT_NO_LINK "NetPortNoLink"

#define MSGID_NET_PORT_DISABLED "NetPortDisabled"



#define MSGID_RESET_NOT_ALLOWED "ResetOperationNotAllowed"



#define MSGID_INVALID_SYSLOG_ADDRESS "InvalidSyslogServerAddress"
#define MSGID_SYSLOG_TEST_FAILED "SyslogTestFailed"



#define MSGID_COMMUNITY_SPACE_ERR "CommunityNameNotContainSpace"
#define MSGID_COMMUNITY_LENGTH_ERR "InvalidCommunityNameLength"
#define MSGID_RWCOMMUNITY_SAME_ERR "SameRWCommunityName"
#define MSGID_ROCOMMUNITY_SAME_ERR "SameROCommunityName"
#define MSGID_RWCOMMUNITY_SIMILAR_WITH_HISTORY "RWCommunitySimilarWithHistory"
#define MSGID_ROCOMMUNITY_SIMILAR_WITH_HISTORY "ROCommunitySimilarWithHistory"
#define MSGID_COMMUNITY_SIMPLE_ERR "SimpleCommunityName"
#define MSGID_USERNAME_NOT_EXIST "UserNameNotExist"
#define MSGID_SERVER_ADDRESS_ERR "InvalidServerAddress"
#define MSGID_TRAPITEM_SENDTEST_ERR "TrapSendTestErr"
#define MSGID_V3_NOT_SET_COMMUNITY "V3NotSetCommunityName"
#define MSGID_TRAPITEM_MEMBERID_NOTEXIST "TrapMemberIDNotExist"
#define MSGID_USERNAME_NOT_USED "V3UserNameNotUsed"
#define MSGID_ITEM_PROP_FEW "PropertyItemPropertyFew"

#define MSGID_COMMUNITY_IN_WEAK_PWDDICT "CommunityNameInWeakPWDDict"



#define MSGID_MONITOR_TASK_FAIL "TaskFailed"
#define MSGID_MONITOR_TASK_RUN "TaskInRunning"
#define MSGID_MONITOR_TASK_COMPLETE "TaskSuccessful"



#define MSGID_ANONYMOUS_SET_ERR "ModifyFailedWithAnonymousLoginEnabled"


#define MSGID_SET_FP_FAILED "SetFusionPartitionFailed"
#define MSGID_SERVICE_INCORRECT_SENDERINFO_ERR "IncorrectSenderInfo"
#define MSGID_SERVICE_SMTP_CONNECTTION_ERR "SmtpConnectionFailed"
#define MSGID_SERVICE_SENDING_EMAIL_ERR "SendingEmailFailed"


#define MSGID_EXPORT_CSR_FAILED "CSRExportFailed"

#define MSG_KVM_SET_ENCRYPTION_FAIL "DisableKVMEncryptionFailed"
#define MSG_VMM_SET_ENCRYPTION_FAIL "EnableVMMEncryptionFailed"
#define MANAGER_KVM_MESSAGE_KVMUSE "KvmInUse"
#define MANAGER_VMM_ENABLE_INUSE "VmmEnableFloppyInUse"
#define MANAGER_VMM_DISABLE_INUSE "VmmDisableFloppyInUse"
#define MAGID_VMM_IMAGE_LEN_ILLEGAL "IncorrectImageLength"
#define MANAGER_VNC_MESSAGE_VNCUSE "VncInUse"
#define MSGID_DISABLE_SCREENSHOT_IN_CURRENT_STATE "DisableScreenShotInCurrentState"

#define MSGID_SP_CFG_OVERSIZED "FileOversized"
#define MSGID_SP_CFG_LIMIT "FileCountReachedLimit"
#define MSGID_OPERATION_FAILED "OperationFailed"
#define MSGID_NOT_ALL_BBU_ON_CHARGING "NotAllBBUOnCharging"
#define MSGID_SP_IS_BEING_UPGRADED "SPIsBeingUpgraded"       
#define MSGID_UMS_IS_EXCLUSIVELY_USED "UMSIsExclusivelyUsed" 
#define MSGID_BMC_NOT_SUPPORT_SP "BMCNotSupportedSP"

#define MSGID_SP_TSFILE_OK "SPTransferSuccess"
#define MSGID_SP_TSFILE_RUN "SPTransferProgress"
#define MSGID_SP_UPGRADE_OK  "SPUpgradeSuccess"
#define MSGID_SP_UPGRADE_RUN "SPUpgradeProgress"

#define MSGID_BMC_NOT_SUPPORT_IBMA "BMCNotSupportediBMA"
#define MSGID_IBMA_DELETE_PROP_REDUNDANT "IBMADeleteParameterError"
#define MSGID_IBMA_UPGRADE_URI_ILLEGAL "IBMAUpgradeURIIllegal"
#define MSGID_IBMA_UPGRADE_PACAKE_INVALID "IBMAUpgradePackageInvalid"
#define MSGID_IBMA_PLATFORM_NOT_MATCH_BMC "BMCPlatformNotMatchiBMA"
#define MSGID_IBMA_UPGRADE_CMS_AUTH_ERROR "IBMAUpgradeFileCheckErr"
#define MSGID_IBMA_UPGRADE_OK  "IBMAUpgradeSuccess"
#define MSGID_IBMA_UPGRADE_RUN "IBMAUpgradeProgress"


#define MSGID_LDAP_MISMATCH_GROUP_AND_CONTOLLER "MismatchedGroupAndController"
#define MSGID_LDAP_CERT_IMPORT_SUCCESS "LDAPCertImportSuccess"



#define MSGID_KRB_KEYTAB_FILE_UPLOAD_SUCCESS "KRBKeytabUploadSuccess"



#define MSGID_KRB_KEYTAB_FILE_UPLOAD_FAILED "KRBKeytabUploadFail"


#define MSGID_PROP_VALUE_OUTOFRANGE "PropertyValueOutOfRange"
#define MSGID_PROP_WRONG_ARRAY_LENGTH "WrongArrayLength"

#define MSGID_FAILED_SET_IP_INFO "FailedSetIPInformation"

#define MSGID_UPLOAD_NTPGROUPKEY_FAILED "UploadNTPSecureGroupKeyFailed"
#define MAGID_UPLOAD_NTPGROUPKEY_SUCCESSFULLY "UploadNTPSecureGroupKeysuccessfully"
#define MSGID_NTPGROUPKEY_TOOLARGE "NTPSecureGroupKeyTooLarge"
#define MSGID_NTPGROUPKEY_NOT_SUPPORTED "NTPGroupKeyNotSupported"

#define MSGID_PROP_SCALARINCREMENT "PropertyScalarIncrement"
#define MSGID_PROP_SETTINGPROPFAILD "SettingPropertyFailed"
#define MSGID_PROP_SETTINGBOOTORDERFAILED "SettingBootOrderFailed"
#define MSGID_PROP_IMMUTABLE "PropertyImmutable"
#define MSGID_PROP_SETTINGPROPFAILDEXT "SettingPropertyFailedExtend"

#define MSGID_CERT_IMPORTING "CertificateImportingErr"

#define MSGID_CERT_NOT_SUPPORT_CRL_SIGN      "CANotSupportCrlSignature"
#define MSGID_CRL_IMPORTING "CrlImportingError"
#define MSGID_CRL_IMPORT_FAILED "CrlImportFailed"
#define MSGID_ROOT_CERT_ID_INVALID "RootCertificateIdInvalid"
#define MSGID_ROOT_CERT_MISMATCH "RootCertificateMismatch"
#define MSGID_CRL_FILE_FORMAT_ERROR "CrlFileFormatError"
#define MSGID_ROOT_CERT_NOT_IMPORT "RootCertificateNotImported"
#define MSGID_CRL_ISSUING_DATE_INVALID "CrlIssuingDateInvalid"
#define MSGID_CRL_NOT_EXISTED "CrlNotExisted"


#define EVT_REGISTRY_PREFIX "iBMCEvents"



#define MSGID_ACT_FAILED_BY_CPLD_UPGRADE "ActionFailedByCpldUpgrade"
#define MSGID_ACT_FAILED_BY_VRD_UPGRADE "ActionFailedByVRDUpgrade"
#define MSGID_RESET_FAILED "ResetOperationFailed"
#define MSGID_RESET_FAILED_BY_BBU "ActionFailedByBbuExist"
#define MSGID_RESET_FAILED_BY_VRD "ResetOperationFailedVRD"



#define MSGID_CONFIG_IP_FAILED "ConfigNetworkFailed"
#define MSGID_RESOURCE_CRETED "ResourceCreated"



#define MSGID_INVALID_ACTION_PARAM_VALUE "ActionParameterValueInvalid"
#define MSGID_PCIE_TOPO_CONFIG_FAILED "PCIeTopoConfigFailed"
#define MSGID_PCIE_TOPO_CONFIG_OK "PCIeTopoConfigOK"


#define MSGID_LAST_TASK_NOT_OVER "LastTaskNotComplete"
#define MSGID_CLEAR_TABLE_FAIL "ClearTableFail"
#define MSGID_EXPORT_TABLE_FAIL "ExportTableFail"


#define MSGID_ONLY_ONE_LANGUAGE_INSTALLED "OnlyOneLanguageInstalled"
#define MSGID_NOT_SUPPORT_ZH_AND_EN_UNINSTALLED "NotSupportZhAndEnUninstalled"
#define MSGID_LANGUAGE_NOT_INSTALLED "LanguageNotInstalled"
#define MSGID_LANGUAGE_NOT_SUPPORT "LanguageNotSupport"
#define MSGID_LANGUAGE_ZH_AND_EN_REQUIRED "LanguageZhAndEnRequired"


// COMMON
#define MSGID_NOT_ALLOWED_ON_STANDBY_MM "NotAllowedOnStandbyMM"
#define MSGID_CURRENT_MM_NOT_ACTIVE "CurrentMMNotActive"
#define MSGID_CURRENT_BOARD_ABSENT "CurrentBoardAbsent"
// POWER
#define MSGID_POWER_MODE_NOT_SUPPORTED_MODIFY "PowerModeNotSupportModification"
#define MSGID_POWER_SET_SLEEPCONFIG_MANUAL_CLOSE "PowerSetSleepConfigManualClose"
#define MSGID_POEWR_SET_SLEEPNFIG_CONFIG_ERR "PowerSetSleepConfigConfigErr"
#define MSGID_CAPACITY_VALUE_LESS_THAN_DEMAND "CapacityValueLessThanDemand"
#define MSGID_POWER_CAPPING_VALUE_LESS_THAN_ALLOWED "PowerCappingValueLessThanAllowed"
#define MSGID_POWER_CAPPING_VALUE_GREAT_THAN_CAPACITY "PowerCappingValueGreatThanCapacity"
#define MSGID_POWER_CAPPING_NOT_IN_MANUAL_MODE "PowerCappingNotInManualMode"
#define MSGID_NO_POWER_CAPPING_VALUE_FOR_THE_BLADE "NoPowerCappingValueForTheBlade"
#define MSGID_POWER_CAPPING_VALUE_SET_FAILED "PowerCappingValueSetFailed"
#define MSGID_POWER_CAPPING_BLADE_VALUE_SET_FAILED "PowerCappingBladeValueSetFailed"
#define MSGID_POWER_ON_CONTROL_STATE_SET_FAILED "PowerOnControlledStateSetFailed"
#define MSGID_POWER_ON_CONTROL_TIMEOUT_SET_FAILED "PowerOnControlledTimeoutSetFailed"
#define MSGID_BLADE_POWER_ON_ENABLE_FAILED "BladePowerOnEnableFailed"
#define MSGID_POWER_SET_PSU_SUPPLY_SOURCE_TIMEOUT "SetPsuSupplySourceTimeout"
// FANTRAY
#define MSGID_SET_FAN_SPEED_IN_AUTO_MODE "SetFanSpeedInAutoMode" // 在自动模式下不应设置风速
#define MSGID_FAN_NOT_PRESENT "FanNotPresent"
#define MSGID_SMART_MODE_CONFLICT_WITH_CONTROL_MODE "SmartCoolingModeConflictWithControlMode"
// BLADE
#define MSGID_BLADE_NOT_PRESENT "BladeNotPresent"
#define MSGID_REDFISH_NOT_SUPPORTED_BY_BOARD "BoardNotManagedByMM"
#define MSGID_BLADE_NOT_READY "BladeNotReady"
#define MSGID_BLADE_COMMUNICATION_LOST "BladeCommunicationLost"
// ASM
#define MSGID_FORCE_FAILOVER_ERROR "ForceFailoverError"
// UPGRADE
// IP
#define MSGID_IP_NOT_EXIST_VALUE_NOT_EMPTY "IPNotExistValueNotEmpty"
// TIME
#define MSGID_MODIFY_FAILED_WITH_NTP_MODE "ModifyFailedWithNtpMode"

#define MSGID_MINPWDAGE_AND_PWDVALIDITY_RESTRICT_EACH_OTHER "MinPwdAgeAndPwdValidityRestrictEachOther"




#define MSGID_REVOKE_LICENSE_SUCCESS "LicenseRevokeSuccess"
#define MSGID_ILLEGAL_URI_PATH "IllegalContentBody"
#define MSGID_INSTALL_LICENSE_SUCCESS "LicenseInstallSuccess"
#define MSGID_INSTALL_LICENSE_FAILED "LicenseInstallFailed"
#define MSGID_VERIFY_LICENSE_FAILED "LicenseVerifyFailed"
#define MSGID_ACTIVATE_LICENSE_FAILED "LicenseActivateFailed"
#define MSGID_LICENSE_EXPORTING "LicenseExporting"
#define MSGID_LICENSE_INSTALLING "LicenseInstalling"
#define MSGID_LICENSE_NOT_INSTALL "LicenseNotInstall"
#define MSGID_REVOKE_LICENSE_FAILED "LicenseRevokeFailed"
#define MSGID_DELETE_LICENSE_FAILED "LicenseDeleteFailed"
#define MSGID_EXPORT_LICENSE_FAILED "LicenseExportFailed"

#define MSGID_IMPORT_UTAG_CONFIG_FILE_ERR "ImportUtagConfFileErr"
#define MSGID_CONFIG_FILE_IMPORT_FAIL "ConfigFileImportFail"

#define MSGID_RACK_PWR_CAP_SERIALNUMBER_DUPLICATE "PowerCappingSerialNumberDuplicate"
#define MSGID_RACK_PWR_CAP_PARAMETER_VALUE_INVALID "PowerCappingParameterValueInvalid"
#define MSGID_DISABLE_PWR_CAP_FAIL_WITH_PEAK_CLIP_ENABLE "DisabledPowerCappingFailedWithPeakClippingEnabled"


#define MSGID_MODE_NOT_SUPPORT "CurrentModeNotSupport"
#define MSGID_VLAN_TRUNKRANG_ERROR "TrunkRangeCheckFail"
#define MSGID_PF_TARGET_EXCEED "ExcessiveQuantity"

#define MSGID_PORT_INSUFFICIENT "InsufficientQuantity"

#define MSGID_BANDWITH_ERROR "SettingBandWidthFailed"
#define MSGID_VLANID_CONFLICT "VlanIdConflict"
#define MSGID_VLANID_DIFF "VlanIdNotSame"
#define MSGID_CONFIG_BUSY "ConfigurationBusy"

#define MSGID_CHIP_RULE_CHECK_FAIL "ChipRuleCheckFail"


#define MSGID_PROTOCOLS_CHECK_FAIL "ProtocolsCheckFail"



#define MSGID_QINQ_VLANID_ERROR "VlanIdValueError"



#define MSGID_PORT_ID_SEQUENCE_ERROR "PortIdSequenceError"



#define MSGID_BOOTPROTOCOL_NOT_FOUND_IN_PFS "BootProtocolNotFoundInPFs"



#define MSGID_LDAP_NO_MATCH "GroupNameMismatch"

#define MSGID_PROP_VALUE_MISMATCH "PropertyValueMismatch"


#define MSGID_BIOS_STATUS_UNSUPPORTED "BIOSStatusNotSupported"
#define MSGID_BIOS_ENERGY_SAVING_DISABLED "EnergySavingModeNotEnabled"


#define MSGID_ENERGY_SAVING_NOT_ALLOWED "EnergySavingCfgNotAllowed"
#define MSGID_ENERGY_SAVING_ENABLED_NOT_ALLOWED "EnergySavingEnabledNotAllowed"
#define MSGID_ENERGY_SAVING_POWER_MODE_NOT_ALLOWED "EnergySavingPowerModeCfgNotAllowed"

#define MSGID_ALL_ALGORITHMS_DISABLED "AllAlgorithmsDisabled"
#define MSGID_ALL_CIPHER_SUITES_DISABLED "AllCipherSuitesDisabled"

#define MSGID_MULTICAST_INTERVAL_IS_ZERO "MulticastIntervalIsZeroSecond"

#define MSGID_SERVICE_RESTART_IN_PROGRESS "ServiceRestartInProgress"


#define MSGID_SYSLOCKDOWN_FORBID "SystemLockdownForbid"
#define MSGID_EXPORTFILE_SUFFIX_NAME_INVALID "ExportFileSuffixNameInvalid"

#define MSGID_MASTERKEY_UPDATINGLIMIT "MasterKeyUpdatingLimit"
#define MSGID_MASTERKEY_UPDATENOTALLOWED "MasterKeyUpdateNotAllowed"

#define MSGID_MUTUAL_AUTH_ROOT_CERT_IN_USE "RootCertificateInUse"
#define MSGID_FORBID_SET_PROP_BY_INBAND_CHANNEL "ForbidSetPropertyByInbandChannel"
#define MSGID_NO_PRIV_OPT_FILE "NoPrivilegeToOperateSpecifiedFile"

#define MSGID_FILE_EXIST "SpecifiedFileExist"
#define MSGID_FAILED_SET_NPU_WORK_MODE "FailedSetNpuWhenPoweredOn"
#define MSGID_SERVER_ID_MODIFY_UNSUPPORTED "ServerIDModifyNotSupported"

#define MSGID_HTTPS_ROOTCERT_NOT_IMPORT "HttpsSeverRootCertNotImported"
#define MSGID_PASSWORD_NEED_RESET "PasswordNeedReset"
#define MSGID_SWITCH_BOARD_SET_FAIL "SwitchBoardSetFailed"

#define MSGID_RAID_CONTROLLER_SET_SUCC "RAIDControllerSetSuccessfully"
#define MSGID_VOLUME_SET_SUCC "VolumeSetSuccessfully"
#define MSGID_NOT_ALLOW_OVERWRITE_DIR "NotAllowedToOverwriteDirectory"

#define MSGID_INCOMPLETE_FOREIGN_CONFIG "IncompleteForeignConfig"
#define MSGID_NO_FOREIGN_CONFIG "NoForeignConfig"

#define MSGID_SUBMIT_RESUMED_EVENT_COMPLETELY "SubmitResumedEventCompletely"
#define MSGID_SUBMITTING_RESUMED_EVENT "SubmittingResumedEvent"
#define MSGID_SUBMITTING_RESUMED_ERR "SumittingResumedErr"

#define RF_SENSITIVE_INFO "******"

typedef struct {
    gint32      ret_val;
    gint32      response_code;
    const gchar *msg_id;
    const gchar *prop_name;
    const gchar *param1;   // 可能是prop_val，也可能还是prop_name
    const gchar *param2;
    const gchar *param3;
} RET_MSG_INFO_MAP;






extern void redfish_message_init(void);

extern gint32 generate_error_message_info(json_object *extended_jso, json_object **err_message_jso);

extern gint32 generate_normal_message_info(json_object *extend_jso, json_object **err_message_jso);

extern gint32 create_message_vinfo(json_object **message_info_jso, const gchar *message_id,
    const gchar *related_property, va_list argv);

extern gint32 create_message_info(const gchar *message_id, const gchar *related_property,
    json_object **message_info_jso, ...);

extern char *get_single_message_str(json_object *single_err_jso);

extern gint32 check_event_msg_id_validity(const gchar *msg_id);

extern gint32 find_msg_info(const char *msg_id, char *registry_id, guint32 registry_id_len, json_object **msg_jso);

extern gint32 get_ret_message(gint32 ret_val, RET_MSG_INFO_MAP *info_map, gsize map_size, json_object *msg_array);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
