
#ifndef __REDFISH_EVENT_H__
#define __REDFISH_EVENT_H__

#define EVT_LANG_FILE_CRLF_SYMBOL "@#AB;"
#define EVT_LANG_FILE_FORMAT_SYMBOL "%s"

#define OSCA_EVT_LOCATION_FMT "ChassisId:%u,BoardName:%s"


#define OSCA_EVT_LOCATION_ANOTHER_FMT "ChassisId:null,BoardName:%s"

#define RF_EVT_SUBSCRIPTION_ID_MIN 1
#define RF_EVT_SUBSCRIPTION_ID_MAX 4

#define EF_EVT_PROPERTY_LENGTH 128

#define REDFISH_DELAY_1000_MSEC  1000

typedef enum {
    EVT_SHIELD_NONE,
    EVT_SHIELD_RESOURCE,
    EVT_SHIELD_ATTRUBITES
} RSC_EVT_SHIELD_TYPE;

typedef struct evt_time_info {
    guint32 start;
    guint32 end;
} EVT_TIME_INFO;

enum CACHE_EVENT_TYPE {
    SUSPENED_TYPE = 0,
    RETRY_TYPE = 1,
};

gint32 redfish_event_init(void);


gint32 redfish_event_start(void);


gint32 redfish_post_event_msg(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 get_event_orgin_rsc(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 set_redfish_evt_server_identity_source(const OBJ_HANDLE object_handle, GSList *caller_info,
    GSList *input_list, GSList **output_list);

gint32 set_redfish_evt_service_enabled(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 set_redfish_evt_subscription_ctx(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 set_redfish_evt_subscription_http_headers(OBJ_HANDLE object_handle, GSList *caller_info,
    GSList *input_list, GSList **output_list);

gint32 create_evt_message_info(RF_EVT_MSG_INFO *evt_info, json_object **message_info_jso);

gint32 create_event_message(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 redfish_print_evt_report_error_info(GSList *input_list);

gint32 rf_dump_evt_sender_info(const gchar* path);

gint32 set_evt_svc_shield_resources(PROVIDER_PARAS_S *i_paras, json_object *shield_jso, json_object *message_jso);
void get_evt_svc_shield_resources(json_object *result_jso);
void get_event_shielded_resources_conf(void);
void backup_event_shield_resources_conf(void);
void restore_event_shield_resources_conf(void);
gint32 delete_shield_resources(const gchar* url, json_object **rsc_jso, RSC_EVT_SHIELD_TYPE *sheild_type);
gint32 update_redfish_oem_manufacturer(OBJ_HANDLE object_handle, gpointer user_data, gchar* property_name,
    GVariant* property_value);
gboolean check_property_delivery_retry_policy(const char *property);
gboolean check_property_status(const char *property);
void delete_event_log(EVT_TIME_INFO time, gint32 sub_id, gint32 type);
enum HTTP_RESPONSE_CODE change_drp_value(PROVIDER_PARAS_S* i_paras,
    OBJ_HANDLE description, const gchar *drp);
json_object* get_event_list(EVT_TIME_INFO time, gint32 sub_id, gint32 type);
enum HTTP_RESPONSE_CODE change_state_value(PROVIDER_PARAS_S* i_paras,
    OBJ_HANDLE description, const gchar *state);
gint32 print_description_info(GSList *input_list);
enum HTTP_RESPONSE_CODE resume_suspended_event(json_object **o_message_jso, PROVIDER_PARAS_S *i_paras,
    OBJ_HANDLE description, gint32 id);
#endif