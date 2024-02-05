
#ifndef __REDFISH_EVENT_CUSTOM_H__
#define __REDFISH_EVENT_CUSTOM_H__

gint32 set_redfish_evt_detail_customized_id(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 delete_simulate_alert_event_record(void);
gint32 add_str_to_jso(json_object *obj_jso, const gchar *str, const gchar *prop_name);
gint32 get_event_detail_customized_by_cmcc(RF_EVT_MSG_INFO *evt_info, json_object *hw_jso);
gint32 replace_event_severity_customized_by_cmcc(RF_EVT_MSG_INFO *evt_info, json_object *evt_msg_jso);
gint32 replace_event_id_customized_by_cmcc_cucc(RF_EVT_MSG_INFO *evt_info, json_object *evt_msg_jso);
gint32 get_event_detail_customized_by_cucc(RF_EVT_MSG_INFO *evt_info, json_object *hw_jso);

#endif