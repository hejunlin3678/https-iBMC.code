
#ifndef __REDFISH_SCHEMA_H__
#define __REDFISH_SCHEMA_H__

#ifdef _cplusplus
extern "C" {
#endif

#define RF_CLASS_ACCOUNTSERVICE "AccountService"
#define RF_CLASS_CHASSIS "Chassis"

// class name
#define ACCOUNT_CLASS "ManagerAccount"
#define SESSION_CLASS "Session"

// URI
#define URI_SESSION_COLLECTION "(?i)^/redfish/v1/SessionService/Sessions$"
#define URI_ACCOUNT_COLLECTION "(?i)/redfish/v1/AccountService/Accounts$"
#define URI_NODE_ACCOUNT_COLLECTION "(?i)/redfish/v1/AccountService/Oem/Huawei/Nodes$"
#define URI_VOLUME_COLLECTION "(?i)^/redfish/v1/Systems/[a-z0-9]+/Storages/[a-z0-9]+/volumes$"

#define URI_NETWORK_BONDING_COLLECTION "(?i)^/redfish/v1/Systems/[a-zA-Z0-9]+/NetworkBondings$"
#define URI_VLAN_COLLECTION "(?i)^/redfish/v1/Systems/[a-zA-Z0-9]+/EthernetInterfaces/[a-zA-Z0-9]+/VLANs"

#define URI_SP_NETDEV_COLLECTION "(?i)^/redfish/v1/Managers/[a-zA-Z0-9]+/SPService/SPNetDev$"
#define URI_SP_RAID_COLLECTION "(?i)^/redfish/v1/Managers/[a-zA-Z0-9]+/SPService/SPRAID$"
#define URI_SP_CFG_COLLECTION "(?i)^/redfish/v1/Managers/[a-zA-Z0-9]+/SPService/SPCfg$"
#define URI_SP_OSINSTALL_COLLECTION "(?i)^/redfish/v1/Managers/[a-zA-Z0-9]+/SPService/SPOSInstallPara$"

#define URI_SP_DIAGNOSE_COLLECTION "(?i)^/redfish/v1/Managers/[a-zA-Z0-9]+/SPService/SPDiagnose$"
#define URI_SP_DRIVEERASE_COLLECTION "(?i)^/redfish/v1/Managers/[a-zA-Z0-9]+/SPService/SPDriveErase$"


#define URI_EVT_SUBSCRIPTION_COLLECTION "(?i)^/redfish/v1/EventService/Subscriptions$"
#define EVT_SUBSCRIPTION_URI_PATTERN "/redfish/v1/EventService/Subscriptions"




#define REDFISH_MSG_REGISTRY_DIR REDFISH_PATH_DEFINE("/conf/redfish/v1/registrystore/messages/en/")

#define REDFISH_EVT_REGISTRY_LOWER_PRE "ibmcevents"


#define RF_CLASS_CHASSISCOLLECTION "ChassisCollection"
#define RF_CLASS_COMPUTERSYSTEM "ComputerSystem"
#define RF_CLASS_COMPUTERSYSTEMCOLLECTION "ComputerSystemCollection"
#define RF_CLASS_ETHERNETINTERFACE "EthernetInterface"
#define RF_CLASS_ETHERNETINTERFACECOLLECTION "EthernetInterfaceCollection"
#define RF_CLASS_EVENT "Event"
#define RF_CLASS_EVENTDESTINATION "EventDestination"
#define RF_CLASS_EVENTDESTINATIONCOLLECTION "EventDestinationCollection"
#define RF_CLASS_EVENTSERVICE "EventService"
#define RF_CLASS_IPADDRESSES "IPAddresses"
#define RF_CLASS_JSONSCHEMAFILE "JsonSchemaFile"
#define RF_CLASS_JSONSCHEMAFILECOLLECTION "JsonSchemaFileCollection"
#define RF_CLASS_LOGENTRY "LogEntry"
#define RF_CLASS_LOGENTRYCOLLECTION "LogEntryCollection"
#define RF_CLASS_LOGSERVICE "LogService"
#define RF_CLASS_LOGSERVICECOLLECTION "LogServiceCollection"
#define RF_CLASS_MANAGER "Manager"
#define RF_CLASS_MANAGERACCOUNT "ManagerAccount"
#define RF_CLASS_MANAGERACCOUNTCOLLECTION "ManagerAccountCollection"
#define RF_CLASS_MANAGERCOLLECTION "ManagerCollection"
#define RF_CLASS_MANAGERNETWORKPROTOCOL "ManagerNetworkProtocol"
#define RF_CLASS_MESSAGE "Message"
#define RF_CLASS_MESSAGEREGISTRY "MessageRegistry"
#define RF_CLASS_MESSAGEREGISTRYCOLLECTION "MessageRegistryCollection"
#define RF_CLASS_MESSAGEREGISTRYFILE "MessageRegistryFile"
#define RF_CLASS_MESSAGEREGISTRYFILECOLLECTION "MessageRegistryFileCollection"
#define RF_CLASS_PHYSICALCONTEXT "PhysicalContext"
#define RF_CLASS_POWER "Power"
#define RF_CLASS_PRIVILEGES "Privileges"
#define RF_CLASS_PROCESSOR "Processor"
#define RF_CLASS_PROCESSORCOLLECTION "ProcessorCollection"
#define RF_CLASS_REDFISHEXTENSIONS "RedfishExtensions"
#define RF_CLASS_REDUNDANCY "Redundancy"
#define RF_CLASS_RESOURCE "Resource"
#define RF_CLASS_ROLE "Role"
#define RF_CLASS_ROLECOLLECTION "RoleCollection"
#define RF_CLASS_SERIALINTERFACE "SerialInterface"
#define RF_CLASS_SERIALINTERFACECOLLECTION "SerialInterfaceCollection"
#define RF_CLASS_SERVICEROOT "ServiceRoot"
#define RF_CLASS_SESSION "Session"
#define RF_CLASS_SESSIONCOLLECTION "SessionCollection"
#define RF_CLASS_SESSIONSERVICE "SessionService"
#define RF_CLASS_SETTINGS "Settings"
#define RF_CLASS_SIMPLESTORAGE "SimpleStorage"
#define RF_CLASS_SIMPLESTORAGECOLLECTION "SimpleStorageCollection"
#define RF_CLASS_TASK "Task"
#define RF_CLASS_TASKCOLLECTION "TaskCollection"
#define RF_CLASS_TASKSERVICE "TaskService"
#define RF_CLASS_THERMAL "Thermal"
#define RF_CLASS_VIRTUALMEDIA "VirtualMedia"
#define RF_CLASS_VIRTUALMEDIACOLLECTION "VirtualMediaCollection"
#define RF_CLASS_VLANNETWORKINTERFACE "VLanNetworkInterface"
#define RF_CLASS_VLANNETWORKINTERFACECOLLECTION "VLanNetworkInterfaceCollection"

#define RF_USER_ADMIN "Admin"
#define RF_USER_OPERATOR "Operator"
#define RF_USER_READONLYUSER "ReadOnlyUser"

#define RF_ACCOUNT_USERNAME "UserName"
#define RF_ACCOUNT_PASSWORD "Password"
#define RF_ACCOUNT_ROLE "Role"
#define RF_ACCOUNT_ROLE_ID "RoleId"
#define RF_ACCOUNT_ASIGNED_PRIL "AssignedPrivileges"

#define RF_LOG_USER_NAME "UserName"
#define RF_LOG_USER_IP "UserIp"
#define RF_USER_PRIV "UserPriv"
#define RF_AUTH_TYPE "AuthType"
#define RF_ROLE_ID "RoleId"

#define RF_TASK_START_TIME_PROP "StartTime"
#define RF_TASK_END_TIME_PROP "EndTime"
#define RF_TASK_STATUS_PROP "TaskStatus"
#define RF_TASK_MESSAGES_PROP "Messages"
#define RF_TASK_PERCENTAGE "TaskPercentage"


#define RF_STATUS_OK "OK"
#define RF_STATUS_WARNING "Warning"
#define RF_STATUS_MAJOR "Major"
#define RF_STATUS_CRITICAL "Critical"


#define RF_STATUS_COUNT_OK 0
#define RF_STATUS_COUNT_WARNING 1
#define RF_STATUS_COUNT_MAJOR 2
#define RF_STATUS_COUNT_CRITICAL 3

#define ODATA_ID "@odata.id"
#define ODATA_CONTEXT "@odata.context"
#define ODATA_TYPE "@odata.type"
#define REDFISH_CPRT "@Redfish.Copyright"

#define MEMBER_ID "MemberId"

#define RFOBJ_ID "Id"
#define RFOBJ_NAME "Name"
#define RFOBJ_DESCRIPTION "Description"

#define RFOBJ_MEMBERS "Members"
#define RFOBJ_MEMBERS_COUNT "Members@odata.count"

#define RFOBJ_LINKS "Links"

#define RFOBJ_ACTIONS "Actions"

#define CONTEXT_PROP "Context"

#define RF_ACTIONS_TARGET_PROP "target"

#define RF_LINKS "Links"

#define RFPROP_STATUS "Status"
#define REDFISH_CLASS_NAME "ClassName"

#define RF_STATUS_STATE "State"
#define RF_STATUS_HEALTH "Health"
#define RF_STATE_ENABLED "Enabled"
#define RF_STATE_ABSENT "Absent"
#define RF_STATE_DISABLED "Disabled"

#define RF_FAN_SPEED_READING "Reading"

#define PROP_FAN_PRESENCE "Presence"

#define OEM_ACTION_SEGMENT "/Oem/Huawei/Actions/"
#define ACTION_SEGMENT "/Actions/"

#define RF_NULL_STR "null"
#define RF_DATETIME "DateTime"

#ifndef SESSION_ID_LEN
#define SESSION_ID_LEN 16
#endif

#ifndef SESSION_USER_NAME_MAX_LEN
#define SESSION_USER_NAME_MAX_LEN 320 // ldap: username(255) + @(1) + domainname(64)
#endif

#ifndef SESSION_IP_LEN
#define SESSION_IP_LEN 63
#endif

#ifndef USER_ROLEID_MAX_LEN
#define USER_ROLEID_MAX_LEN 31
#endif

#define MAX_CHARACTER_NUM 256

#define MAX_OEM_PROP_VAL_LEN 1024


#define MAX_FILE_NAME 256
#define MIN_FILE_NAME 5

#define MAX_RSC_ID_LEN 64
#define MAX_RSC_NAME_LEN 64

#define MAX_HEALTH_DESC_LEN 64

#define ETAG_FLAG_ENABLE 1
#define ETAG_FLAG_DISABLE 0

#define MAX_URI_SEGMENT_NUM 16
#define INVALID_INTEGER_VALUE 0x7FFFFFFF
#define SESSION_CLASS_REGEX "^Session\\.v*"
#define SESSION_ODATA_CONTEXT "/redfish/v1/$metadata#Session.Session"

#define RF_ID_EXP "${id}"
#define RF_COLLECTION_FLAG "Collection"

#define TASK_RSC_REGEX "^/redfish/v1/TaskService/Tasks/([1-9]|[1-2][0-9]|[3][0-2])$"

#define TASK_COLLECTION_RSC_REGEX "^/redfish/v1/TaskService/Tasks$"

#define TASK_SVC_RSC_REGEX "^/redfish/v1/TaskService$"
#define UPGRADE_SVC_RSC_REGEX "^/redfish/v1/UpdateService$"
#define HTTPS_CERT_REGEX "^/redfish/v1/managers/[a-zA-Z0-9]+/securityservice/httpscert$"
#define SECURITY_SVC_REGEX "^/redfish/v1/managers/[a-zA-Z0-9]+/securityservice$"
#define UPDATEMK_ACTIONINFO_REGEX "^/redfish/v1/managers/[a-zA-Z0-9]+/securityservice/updatemasterkeyactioninfo$"
#define UPDATEMK_ACTIONINFO_TEMPLATE_PATH "/redfish/v1/managers/1/securityservice/updatemasterkeyactioninfo"
#define IMPORTROOTCA_ACTIONINFO_REGEX \
    "^/redfish/v1/managers/[a-zA-Z0-9]+/securityservice/importremotehttpsserverrootcaactioninfo$"
#define IMPORTROOTCA_ACTIONINFO_TEMPLATE_PATH \
    "/redfish/v1/managers/1/securityservice/importremotehttpsserverrootcaactioninfo"
#define DELETEROOTCA_ACTIONINFO_REGEX \
    "^/redfish/v1/managers/[a-zA-Z0-9]+/securityservice/deleteremotehttpsserverrootcaactioninfo$"
#define DELETEROOTCA_ACTIONINFO_TEMPLATE_PATH \
    "/redfish/v1/managers/1/securityservice/deleteremotehttpsserverrootcaactioninfo"
#define IMPORTCRL_ACTIONINFO_REGEX \
    "^/redfish/v1/managers/[a-zA-Z0-9]+/securityservice/importremotehttpsservercrlactioninfo$"
#define IMPORTCRL_ACTIONINFO_TEMPLATE_PATH "/redfish/v1/managers/1/securityservice/importremotehttpsservercrlactioninfo"
#define DELETECRL_ACTIONINFO_REGEX \
    "^/redfish/v1/managers/[a-zA-Z0-9]+/securityservice/deleteremotehttpsservercrlactioninfo$"
#define DELETECRL_ACTIONINFO_TEMPLATE_PATH \
    "/redfish/v1/managers/1/securityservice/deleteremotehttpsservercrlactioninfo"

#define BIOS_ATTRIBUTE_REGISTRY_REGEX \
    "^/redfish/v1/registrystore/attributeregistries/en/biosattributeregistry[a-zA-Z0-9_\\.]+$"


#define EVENT_SVC_REGEX "^/redfish/v1/eventservice$"
#define EVENT_SUBSCRIPTION_COLLECTION_REGEX "^/redfish/v1/eventservice/subscriptions$"
#define EVENT_SUBSCRIPTION_REGEX "^/redfish/v1/eventservice/subscriptions/([1-4])$"
#define SUBSCRIPTION_TEMPLATE_PATH "/redfish/v1/eventservice/subscriptions/template"
#define TASK_SVC_TEMPLATE_PATH "/redfish/v1/taskservice"
#define EVT_SVC_TEMPLATE_PATH "/redfish/v1/eventservice"
#define EVT_SUBSCRIPTION_COL_TEMPLATE_PATH "/redfish/v1/eventservice/subscriptions"
#define EVT_RSC_PATTERN "/redfish/v1/EventService/Events/%s"

#define EVENTDESTINATION_RECSUME "EventDestination.ResumeSubscription"



#define RACK_MGNT_POWER_AGENT_PATTERN "^/redfish/v1/RackManagementService/PowerAgent$"
#define RACK_MGNT_POWER_AGENT_RSC_PATH "/redfish/v1/rackmanagementservice/poweragent"

#define RACK_MGNT_SET_RACK_POWER_CAPPING_ACTION_INFO \
    "^/redfish/v1/RackManagementService/PowerAgent/SetRackPowerCappingActionInfo$"
#define RACK_MGNT_SET_RACK_POWER_CAPPING_ACTION_TEMPLATE \
    "/redfish/v1/rackmanagementservice/poweragent/setrackpowercappingactioninfo"

#define RACK_MGNT_PERIOD_PRE_RACK_POWER_CAPPING_ACTION_INFO \
    "^/redfish/v1/RackManagementService/PowerAgent/PeriodPreRackPowerCappingActionInfo$"
#define RACK_MGNT_PERIOD_PRE_RACK_POWER_CAPPING_ACTION_TEMPLATE \
    "/redfish/v1/rackmanagementservice/poweragent/periodprerackpowercappingactioninfo"




#ifdef ARM64_HI1711_ENABLED
#define DICE_CERT_REGEX "^/redfish/v1/managers/[a-zA-Z0-9]+/securityservice/dicecert$"
#define DICE_CERT_TEMPLATE_PATH "/redfish/v1/managers/1/securityservice/dicecert"
#endif


#define TASK_TEMPLATE_PATH "/redfish/v1/taskservice/tasks/template"
#define TASK_COL_TEMPLATE_PATH "/redfish/v1/taskservice/tasks"
#define UPDATE_SVC_TEMPLATE_PATH "/redfish/v1/updateservice"
#define HTTPS_CERT_TEMPLATE_PATH "/redfish/v1/managers/1/securityservice/httpscert"
#define SECURITY_SVC_TEMPLATE_PATH "/redfish/v1/managers/1/securityservice"
#define REF_SMS_WHITE_LIST_URI "/redfish/v1/URIWhiteList"
#define REF_SMS_REGEX_MATCH "^/redfish/v1/sms/(\\d+).*$"

#define SERVICE_ROOT_PATH "/redfish/v1"
#define POUND_FLAG_STR "#"
#define RF_TRANSFER_PROTOCOL_FLAG "://"

#define RFPROP_LINKS "Links"

typedef enum _ACCOUNT_STATUS {
    CHECK_USERNAME_FAILED = 0,
    USER_CHECK_OK,
    USER_CODE_EXPIRED,
    USER_IS_NOT_EXIST,
    USER_IS_NOT_MANAGER,
    RPC_CONNECTION_FAILED,
    USER_LOCKED,
    USER_NEED_RESET_PASSWORD
} ACCOUNT_STATUS;


enum HTTP_RESPONSE_CODE {
    HTTP_OK = 200,
    HTTP_CREATED = 201,
    HTTP_ACCEPTED = 202,
    HTTP_NO_CONTENT = 204,
    HTTP_MOVED_PERMANENTLY = 301,
    HTTP_FOUND = 302,
    HTTP_NOT_MODIFIED = 304,
    HTTP_BAD_REQUEST = 400,
    HTTP_UNAUTHORIZED = 401,
    HTTP_FORBIDDEN = 403,
    HTTP_NOT_FOUND = 404,
    HTTP_METHOD_NOT_ALLOWED = 405,
    HTTP_NOT_ACCEPTABLE = 406,
    HTTP_CONFLICT = 409,
    HTTP_GONE = 410,
    HTTP_LENGTH_REQUIRED = 411,
    HTTP_PRECONDITION_FAILED = 412,
    HTTP_UNSUPPORTED_MEDIA_TYPE = 415,
    HTTP_INTERNAL_SERVER_ERROR = 500,
    HTTP_NOT_IMPLEMENTED = 501,
    HTTP_SERVICE_UNAVAILABLE = 503
};

#define RE_APPLICATION_JSON_WITHOUT_CHARSET "(?i)^[ ]*application[ ]*/[ ]*json[ ]*$"
#define RE_APPLICATION_JSON_WITH_CHARSET_UTF8 "(?i)^[ ]*application[ ]*/[ ]*json[ ]*;[ ]*charset[ ]*=[ ]*utf-8[ ]*$"
#define RF_SESSION_REAUTH_KEY "ReauthKey"

#define RFPROP_EVT_TYPE "EventType"
#define RFPROP_MSG_ID "MessageId"
#define RFPROP_EVT_TIMESTAMP "EventTimestamp"
#define RFPROP_ORIGIN_RSC "OriginOfCondition"
#define RFPROP_EVT_ID "EventId"
#define RFPROP_EVT_SUBJECT "EventSubject"
#define RFPROP_EVT_CONTEXT "Context"
#define RFPROP_EVT_LEVEL "Level"
#define RFPROP_EVENTS "Events"
#define RFPROP_SERVER_IDENTITY "ServerIdentity"
#define RFPROP_SERVER_LOCATION "ServerLocation"
#define RFPROP_LAST_ALERT_ID "LastAlertId"
#define RFPROP_ALARM_STATUS "alarmStatus"
#define RFPROP_SEPCIFIC_PROBLEM_ID "specificProblemID"
#define RFPROP_SEPCIFIC_PROBLEM "specificProblem"
#define RFPROP_NE_UID "neUID"
#define RFPROP_NE_NAME "neName"
#define RFPROP_NE_TYPE "neType"
#define RFPROP_OBJECT_UID "objectUID"
#define RFPROP_OBJECT_NAME "objectName"
#define RFPROP_OBJECT_TYPE "objectType"
#define RFPROP_LOCATION_INFO "locationInfo"
#define RFPROP_ADD_INFO "Addinfo"

#define HEADER_CONTENT_TYPE "Content-Type"
#define HEADER_X_AUTH_TOKEN "X-Auth-Token"
#define HEADER_LOCATION "Location"
#define CONTENT_TYPE_JSON "application/json"

#define RFPROP_REGISTRY_VERSION "RegistryVersion"

#define MSG_ARG_STRING_TYPE "string"

#define RFPROP_MSG_IDS "MessageIds"
#define RFPROP_ORIGIN_RSCES "OriginResources"
#define RFPROP_EVT_DESTINATION "Destination"
#define RFPROP_EVT_TYPES "EventTypes"
#define RFPROP_CONTEXT "Context"
#define RFPROP_PROTOCOL "Protocol"
#define RFPROP_HTTP_HEADERS "HttpHeaders"
#define RFPROP_DELIVERY_RETRY_POLICY "DeliveryRetryPolicy"
#define RFPROP_RESUME_TASK_DESC "ResumeSubscription%d Task"
#define RFPROP_EVENT_STATUS "Status"
#define RFPROP_EVENT_STATE "State"
#define RFPROP_EVENT_ACTION_URL "/redfish/v1/EventService/Subscriptions/%d/Actions/EventDestination.ResumeSubscription"
#define RFPROP_EVENT_ACTION_METHOD "#EventDestination.ResumeSubscription"
#define URI_EVENT_RESUME_ACTION_INFO "/redfish/v1/EventService/ResumeSubscriptionActionInfo"
#define RFPROP_DELIVERY_RETRY_ATTEMPTS "DeliveryRetryAttempts"
#define RFPROP_DELIVERY_RETRY_INTERVAL "DeliveryRetryIntervalSeconds"
#define RFPROP_SERVER_IDENTITY_SOURCE "ServerIdentitySource"
#define RFPROP_EVT_SHIELD_RESOURCE "ShieldResourcesForSubscriptions"
#define RFPROP_EVT_SHIELD_RESOURCE_PATH "Resource"
#define RFPROP_EVT_SSL_CERT_VERIFY "SslCertVerify"

#define STATE_VALUE_EANBLED "Enabled"
#define STATE_VALUE_DISABLED "Disabled"
#define STATUS_VALUE_STANDBY_OFFLINE  "StandbyOffline"
#define DELIVER_RETRY_VALUE_TERMINATE_AFTER_RETRY "TerminateAfterRetries"
#define DELIVER_RETRY_VALUE_SUSPEND_RETRIES "SuspendRetries"
#define DELIVER_RETRY_VALUE_RETRY_FOREVER "RetryForever"
#define RFPROP_EVT_DELIVER_BUFFERED_EVT_DURATION "DeliverBufferedEventDuration"

#define RFPROP_SHIELD_SYSTEM_ALERT        "ShieldSystemAlert"
#define RFPROP_SHIELD_EVENT_CODE          "EventCode"
#define RFPROP_SHIELD_EVENT_NAME          "EventName"
#define RFPROP_SERVICE_ENABLED "ServiceEnabled"
#define RFACTION_SUBMIT_TEST_EVENT "EventService.SubmitTestEvent"
#define RFACTION_REARM "Oem/Huawei/EventService.Rearm"

#define RFACTION_PRECISEALARM "Oem/Huawei/EventService.MockPreciseAlarm"

#define RFACTION_PRECISEALARM_EVENTCODE "EventCode"
#define RFACTION_PRECISEALARM_SUBJECTINDEX "SubjectIndex"

#define RFACTION_PRECISEALARM_TYPE_ASSERT "Assert"
#define RFACTION_PRECISEALARM_TYPE_DEASSERT "Deassert"
#define RFACTION_PRECISEALARM_TYPE_CANCEL "StopAll"

#define RFACTION_PRECISEALARM_EVENT_CODE_REGEX "^0x[0-9A-Fa-f]{2}[Ff]{6}$"

#define RFACTION_SHIELD_SYSTEM_ALERT "Oem/Huawei/EventService.ShieldSystemAlert"


#define RFPROP_ID "Id"
#define RFPROP_NAME "Name"

#define SLASH_CHAR '/'

#define DOT_CHAR '.'
#define DOT_STR "."
#define JSON_FILE_EXT "json"
#define OMISSION_STR "..."
#define MAX_PROP_VAL_LEN 128

#define MAX_MESSAGE_ARGS_LEN 512
#define MAX_PROP_VAL_LEN_STR "128"
#define MAX_PROP_VAL_LEN_EM 512

#define TWO_SLASH_STR "//"
#define TWO_BACK_SLASH_STR "\\"

#define HTTPS_PREFIX "https://"
#define HTTP_PREFIX "http://"

#define CUR_DIR_STR "/."

#define MAX_PATH_LENGTH 320
#define SERVICE_ROOT_NAME "ServiceRoot"
#define SERVICE_ROOT_URI "/redfish/v1"

#define ETAG_LEN 13



#define RF_STR_EMPTY ""



#define REGISTRY_COLLECTION_PATTERN "^/redfish/v1/registries$"
#define IBMC_EVENTS_REGISTRY_RSC_PATTERN "^/redfish/v1/registries/ibmcevents[a-zA-Z0-9_\\.]+$"
#define REGISTRY_COLLECTION_PATH "/redfish/v1/registries"
#define IBMC_EVENTS_REGISTRY_PATH "/redfish/v1/registries/ibmcevents_template"




#define RFACTION_PARAM_RESET_TYPE "ResetType"



#define PRIVILEGE_MAP_PATTERN "^/redfish/v1/accountservice/privilegemap$"
#define PRIVILEGE_MAP_RSC_PATH "/redfish/v1/accountservice/privilegemap"
#define RFPROP_TARGETS "Targets"
#define RF_PATTERN_ID "/${id}"



#define RF_ACTIONINFO_STR "ActionInfo"



#define RF_REGISTRY_VERSION_V 0
#define RF_REGISTRY_VERSION_DOT 1



#define RF_INVALID_TASK_ID (-1)


#ifdef _cplusplus
}
#endif 

#endif 
