
#ifndef REDFISH_PROVIDER_CHASSIS
#define REDFISH_PROVIDER_CHASSIS

#define URI_FORMAT_CHASSIS_BOARDS "/redfish/v1/Chassis/%s/Boards"

#define RFPROP_CHASSIS_LANSWITCH_LSW_NAME "Lsw"
#define RFPROP_CHASSIS_LANSWITCH_MESH_NAME "Mesh0"
#define LSW_CONTROL_PLANE_ID 0
#define LSW_DATA_PLANE_ID 1
#define SW_STATUS_VALID 1
#define SW_STATUS_INVALID 0
#define INVALID_PORT_ID 0xffffffff
#define LSW_PORT_TYPE_CAS 3
#define LSW_PORT_TYPE_MGMT 4
#define LSW_LOCAL_PORT_AUTONEG_ENABLE 1
#define LSW_LOCAL_PORT_DUPLEX_HALF 0
#define LSW_LOCAL_PORT_STATUS_DISABLE 0
#define LSW_REDFISH_PORT_WORK_RATE_NUM 6

gint32 chassis_boards_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 get_chassis_board_serialnumber(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 get_chassis_board_manufacturer(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 get_chassis_board_cpldversion(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 get_chassis_board_pcbversion(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 get_chassis_board_componentuid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 get_chassis_board_partnumber(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 get_chassis_board_cardid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 get_chassis_board_boardname(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gboolean redfish_get_board_class_name(OBJ_HANDLE obj_handle, gchar* o_classname, gint32 length);
gint32 get_chassis_disk_backplane(OBJ_HANDLE com_handle, gchar *location_suffix, gint32 location_suffix_len);
gint32 add_chassis_fru(json_object *huawei_obj);
#endif
