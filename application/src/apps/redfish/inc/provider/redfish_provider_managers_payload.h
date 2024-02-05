
#ifndef REDFISH_PROVIDER_MANAGERS_PAYLOAD_H
#define REDFISH_PROVIDER_MANAGERS_PAYLOAD_H


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

gint32 set_shelf_power_button_mode(PROVIDER_PARAS_S *i_paras, json_object **o_msg_jso);
gint32 get_shelf_power_button_mode(json_object **oem_property);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif 