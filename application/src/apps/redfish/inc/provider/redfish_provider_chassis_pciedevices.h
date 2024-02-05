
#ifndef REDFISH_PROVIDER_CHASSIS_PCIEDEVICES_H
#define REDFISH_PROVIDER_CHASSIS_PCIEDEVICES_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

#define SDI_CARD_50_BOARDID_A5 0xa5
#define SDI_CARD_51_BOARDID_A9 0xa9
#define DPU_CARD_12_BOARDID_ED 0xed
#define RFACTION_SET_PCIEDEVICES_NMI_ACTION "PCIeDevices.SetNMI"
#define URI_FORMAT_PCIEDEVICES_NMI_ACTION "/redfish/v1/Chassis/%s/PCIeDevices/%s"
#define PCIEDEVICES_NMI_ACTION "SetNMIActionInfo"

gint32 chassis_pciedevices_nmi_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 get_pciedevices_srv_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 set_pciedevices_nmi_entry(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif