
#ifndef REDFISH_FORWARD_H
#define REDFISH_FORWARD_H

#include "redfish_http.h"

#define SMM_REGEX "hmm[1-2]"
#define BLADE_REGEX "blade([1-9]|[1-2][0-9]|3[0-27-8])"
#define SWI_REGEX "swi[1-4]"

#define URI_CHASSIS_REGEX "^/redfish/v1/chassis/"
#define URI_MANAGERS_REGEX "^/redfish/v1/managers/"
#define URI_SYSTEMS_REGEX "^/redfish/v1/systems/"

#define URI_MANAGERS_HMM "^/redfish/v1/managers/hmm[12]$"
#define URI_MANAGERS_HMM_ETH_INTERFACES_PREFIX "^/redfish/v1/managers/hmm[12]/EthernetInterfaces/staticeth2$"
#define URI_MANAGERS_HMM_ETH_PREFIX "^/redfish/v1/managers/hmm[12]/ethernetinterfaces$"
#define URI_MANAGERS_HMM_ETH_STATIC_PREFIX "^/redfish/v1/managers/hmm[12]/EthernetInterfaces/StaticIp$"
#define URI_MANAGERS_HMM_ETH_VLAN_PREFIX "^/redfish/v1/managers/hmm[12]/EthernetInterfaces/Plane[0-9\\.]+$"
#define URI_MANAGERS_BOARD "^/redfish/v1/managers/(blade([1-9]|[1-2][0-9]|3[0-27-8])|swi[1-4])$"
#define URI_MANAGERS_BOARD_ETH_INTERFACES_PREFIX \
    "^/redfish/v1/managers/(blade([1-9]|[1-2][0-9]|3[0-27-8])|swi[1-4])/EthernetInterfaces" // 子级可透传

#define URI_CHASSIS_HMM "^/redfish/v1/chassis/hmm[12]$"
#define URI_CHASSIS_HMM_SENSORS "^/redfish/v1/chassis/hmm[12]/(threshold|discrete)sensors$"
#define URI_CHASSIS_BOARD "^/redfish/v1/chassis/(blade([1-9]|[1-2][0-9]|3[0-27-8])|swi[1-4])$"
#define URI_CHASSIS_BOARD_POWER "^/redfish/v1/chassis/(blade([1-9]|[1-2][0-9]|3[0-27-8])|swi[1-4])/power$"
#define URI_CHASSIS_BOARD_BOARDS_MAINBOARD \
    "^/redfish/v1/chassis/(blade([1-9]|[1-2][0-9]|3[0-27-8])|swi[1-4])/boards/[^/]+$"


#define URI_FORWARD_CHASSIS_ENC "^/redfish/v1/chassis/enc$"
#define URI_FORWARD_CHASSIS_ENC_THERMAL "^/redfish/v1/chassis/enc/thermal$"
#define URI_FORWARD_CHASSIS_ENC_POWER "^/redfish/v1/chassis/enc/power$"
#define URI_FORWARD_CHASSIS_ENC_LEDGROUP "^/redfish/v1/chassis/enc/ledgroup$"


#define URI_FORWARD_CHASSIS_ENCLOSURE "^/redfish/v1/chassis/(enclosure|enc)$"
#define URI_FORWARD_CHASSIS_ENCLOSURE_THERMAL "^/redfish/v1/chassis/enclosure/thermal$"
#define URI_FORWARD_CHASSIS_ENCLOSURE_POWER "^/redfish/v1/chassis/enclosure/power$"
#define URI_FORWARD_CHASSIS_ENCLOSURE_HDD_DRAWER "^/redfish/v1/chassis/enclosure/hdddrawer$"
#define URI_CHASSIS_LED_GROUP "^/redfish/v1/chassis/[a-zA-Z0-9]+/LedGroup$"

#define URI_FIRMWARE_INVENTORY "^/redfish/v1/updateservice/firmwareinventory/[^/]+$"
#define URI_EVENT_REARM "^/redfish/v1/eventservice"
#define URI_SYSTEMS_BOARD "^/redfish/v1/systems/(blade([1-9]|[1-2][0-9]|3[0-27-8])|swi[1-4])$"
#define URI_SYSTEMS_BOARD_RESET_ACTION_INFO \
    "^/redfish/v1/systems/(blade([1-9]|[1-2][0-9]|3[0-27-8])|swi[1-4])/ResetActionInfo$"
#define URI_SYSTEMS_BOARD_FRU_CTRL_ACTION_INFO \
    "^/redfish/v1/systems/(blade([1-9]|[1-2][0-9]|3[0-27-8])|swi[1-4])/FruControlActionInfo$"

#define URI_ACCOUNT_NODES "^/redfish/v1/accountservice/oem/huawei/nodes/[0-9]+_slot[0-9]+$"

typedef gint32 (*FORWARD_PROVIDER)(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, json_object **header,
    json_object **body, json_object *message_array_obj);

typedef struct tar_forward_provider_s {
    gchar *origin_uri;        // 管理板接收到的URI路径
    guint8 allowed_operation; // 需要透传的操作（不需要透传表示本地有处理，如本地不处理，则透传） 按位允许 bit1 GET bit2
                              // PATCH bit3 POST bit4 DELETE
    guint8 get_privilege;     // 获取操作权限
    guint8 set_privilege;     // 修改操作权限
    FORWARD_PROVIDER func;    // 处理透传请求
} FORWARD_PROVIDER_S;

struct tag_managers_collection_param {
    REQUEST_INFO_S *req_info;   // 发送到单板的请求
    pthread_mutex_t write_lock; // 结果写入锁
    pthread_mutex_t m;          // 用于条件信号量
    pthread_cond_t c;           // 条件变量
    guint32 task_running;       // 正在执行的任务数目
    guint32 members_count;      // 访问成功的单板数目
    json_object *members_array; // 访问成功的单板集合
};

struct tag_task_param {
    struct tag_managers_collection_param *param;
    OBJ_HANDLE blade_handle; // 单板句柄
};

void redfish_forward_init(void);

FORWARD_PROVIDER_S *find_forward_request(gchar *uri, gchar *method_name);
FORWARD_PROVIDER_S *find_forward_request_from_table(gchar* uri, gchar* method_name,
    FORWARD_PROVIDER_S* forward_tab, gint32 tab_len);
gint32 forward_for_em(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, HTTP_RESPONSE_DATA_S *response);


gint32 handle_forward_request(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param,
    FORWARD_PROVIDER_S *forward_provider, HTTP_RESPONSE_DATA_S *response);

gint32 send_redfish_request_to_target(guint8 ipmb_addr, PROVIDER_PARAS_S *i_param, REQUEST_INFO_S *request_info,
    json_object **header, json_object **body, json_object *message_array_obj);
gint32 forward_for_public_device(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param,
    HTTP_RESPONSE_DATA_S* response_str);
gint32 get_blade_name_from_uri(gchar *uri, gchar *blade_name, gsize buff_len, json_object *message_array_obj);
gint32 add_blade_system_name(guint8 ipmb, json_object *body);
gint32 add_blade_size(const gchar *blade_name, json_object *body);
gint32 add_blade_size_to_obj(const gchar *blade_name, json_object *json_obj);
gint32 convert_width_height(guint8 size, json_object *json_obj);
void format_account_body(json_object *body, const gchar *uid_str, const gchar *slot_id_str);

#endif 
