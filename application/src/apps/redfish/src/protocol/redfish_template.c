
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "redfish_main.h"
#include "redfish_http.h"
#include "redfish_event.h"
#include "redfish_event_custom.h"
#include "redfish_ipmi.h"

// 1) 注册模块，必须填写
// 2) 注册管理的类, 如果有则填写，没有不填写
// 3) 注册类的方法实现，如果有则填写，没有不填写
// 4) 注册需要持久化的数据，如果有静态的可以填写，如果依赖对象的，在代码中动态添加
// 5) 注册需要处理的IPMI命令，如果有则填写，没有不填写
// 6) 注册reboot善后处理函数，如果有则填写，没有不填写
// 7) 注册增量文件，如果有则填写，没有不填写
// 8) 注册调试命令，如果有则填写，没有不填写
// 同类型的注册请连续放在一起便于维护。
BEGIN_MODULE_DECLARATION(MODULE_NAME_REDFISH)
// -- 本模块需要管理的类声明: class name(<=32 bytes)
DECLARATION_CLASS(CLASS_REDFISH)
DECLARATION_CLASS(CLASS_RF_EVT_SVC)
DECLARATION_CLASS(CLASS_EVT_SUBSCRIPTION)

// -- 类方法实现声明: class name(<=32 bytes), property name(<=32 bytes), callback function
DECLARATION_CLASS_METHOD(CLASS_REDFISH, METHOD_HTTP_REQUEST, method_process_http_request)
DECLARATION_CLASS_METHOD(CLASS_REDFISH, METHOD_HTTP_REDIRECT, method_redirect_http_request)
DECLARATION_CLASS_METHOD(CLASS_REDFISH, METHOD_COPY_SP_SCHEMA, method_copy_sp_schema)
DECLARATION_CLASS_METHOD(CLASS_REDFISH, METHOD_REDFISH_UPDATE_CIPHERTEXT, kmc_update_redfish_ciphertext)
DECLARATION_CLASS_METHOD(CLASS_REDFISH, METHOD_RF_START_SP_FIRMWARE_UPGRADE_MONITOR,
    start_sp_firmware_upgrade_monitor)

DECLARATION_CLASS_METHOD(CLASS_RF_EVT_SVC, METHOD_RF_EVT_SVC_POST_EVT_MSG, redfish_post_event_msg)
DECLARATION_CLASS_METHOD(CLASS_RF_EVT_SVC, METHOD_RF_EVT_SVC_GET_REDFISH_EVT_MSG, create_event_message)
DECLARATION_CLASS_METHOD(CLASS_RF_EVT_SVC, METHOD_RF_EVT_SVC_GEN_ALERT_ORIGIN_RSC, get_event_orgin_rsc)
DECLARATION_CLASS_METHOD(CLASS_RF_EVT_SVC, METHOD_RF_EVT_SVC_SET_SERVER_IDENTITY_SOURCE,
    set_redfish_evt_server_identity_source)
DECLARATION_CLASS_METHOD(CLASS_RF_EVT_SVC, METHOD_RF_EVT_SVC_SET_EVENT_POST_ENABLE, set_redfish_evt_service_enabled)
DECLARATION_CLASS_METHOD(CLASS_RF_EVT_SVC, METHOD_RF_EVT_SVC_SET_EVENT_DETAIL_CUSTOMIZED_ID,
    set_redfish_evt_detail_customized_id)

DECLARATION_CLASS_METHOD(CLASS_RF_EVT_SVC, METHOD_RF_EVT_SVC_DATA_SYNC, redfish_data_sync)
DECLARATION_CLASS_METHOD(CLASS_EVT_SUBSCRIPTION, METHOD_EVT_SUBSCRIPTION_SET_CONTEXT, set_redfish_evt_subscription_ctx)
DECLARATION_CLASS_METHOD(CLASS_EVT_SUBSCRIPTION, METHOD_EVT_SUBSCRIPTION_SET_HTTP_HEADERS,
    set_redfish_evt_subscription_http_headers)
DECLARATION_CLASS_METHOD(CLASS_EVT_SUBSCRIPTION, METHOD_EVT_SUBSCRIPTION_DATA_SYNC, redfish_data_sync)
ON_CLASS_PROPERTY_EVENT(CLASS_ENCRYPTKEY, PROPERTY_ENCRYPTKEY_WORKKEY, update_redfish_encrypted_data, NULL)
ON_CLASS_PROPERTY_EVENT(PFN_CLASS_NAME, PFN_UPGRADED_FLAG, rollback_redfish_encrypted_data, NULL)
ON_CLASS_PROPERTY_EVENT(PFN_CLASS_NAME, PFN_PREROLLBACK_FLAG, rollback_redfish_encrypted_data, NULL)
ON_CLASS_PROPERTY_EVENT(BMC_OBJECT_NAME, BMC_CUSTOM_MANUFACTURER, update_redfish_oem_manufacturer, NULL)
// -- reboot 请求处理函数
// -- 软件需要复位时，会向所有进程广播reboot消息，各个模块做退出善后处理(关闭文件、释放资源等)。如果无需处理可以不用注册
DECLARATION_REBOOT_PROCESS(redfish_reboot_request_process)

// -- 添加对象处理
// -- 对象添加后，模块可能需要管理新增的对象
// ON_ADD_OBJECT(NULL) // ? 是否要加类名?
// -- 删除对象处理
// -- 对象删除后，模块可能需要做一些清理处理
// ON_DEL_OBJECT(NULL)
// -- 本模块dump信息导出回调处理函数
ON_DUMP(redfish_dump_info)
// -- 监听属性变更事件
// -- 监听其他类属性变更事件回调. 如果监听指定对象事件需动态添加


// ON_CLASS_PROPERTY_EVENT("OtherClass", "Property1", NULL, NULL)
ON_CLASS_PROPERTY_EVENT(CLASS_SMS, PROPERTY_SMS_IPADDR, ConfigureSmsHost, NULL)
ON_CLASS_PROPERTY_EVENT(CLASS_SMS, PROPERTY_SMS_PORT, ConfigureSmsHost, NULL)

ON_CLASS_PROPERTY_EVENT(CLASS_SMS, PROPERTY_SMS_REGISTERSTATE, set_default_white_list, NULL)


ON_CLASS_PROPERTY_EVENT(CLASS_SECURITY_ENHANCE, PROPERTY_SECURITY_SYSTEMLOCKDOWNSTATE,
    redfish_system_lockdown_state_changed, NULL)

END_MODULE_DECLARATION()
BEGIN_DEUBG_COMMAND()
    ADD_DEBUG_COMMAND("show_evt_sender_errmsg", redfish_print_evt_report_error_info,  "", 
        "Usage: show_evt_sender_errmsg\nPrint event subscriptions curl error info.\n")
    ADD_DEBUG_COMMAND("print_scription_info", print_description_info,  "",
        "Usage: print_scription_info\nPrint event subscriptions length of queue and flag of retry.\n")
END_DEBUG_COMMAND()


BEGIN_IPMI_COMMAND()
ADD_IPMI_COMMAND_V2(NETFN_OEM_REQ, 0x93, "data[3]=47h", RMCP_USER | IPMI_SYS_LOCKDOWN_FORBID,
    PROPERTY_USERROLE_READONLY, 0xff, 0x05, IpmiSetSPInfo, NULL)
END_IPMI_COMMAND()


// -- 支持的IPMI命令声明: netfn, cmd, filter(e.g. "data[0]=1,data[2]=3"), privilege, maxlen, minlen, callback function

// -- 支持的debug调试命令声明

// -- 模块初始化及处理，必填
BEGIN_MODULE_MAIN()
// -- 本模块实现的初始化函数，可选
MODULE_INIT(redfish_init)
MODULE_START(redfish_start)
END_MODULE_MAIN()
