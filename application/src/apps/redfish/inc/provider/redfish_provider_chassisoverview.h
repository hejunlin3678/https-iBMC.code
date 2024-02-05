

#ifndef REDFISH_PROVIDER_CHASSISOVERVIEW_H
#define REDFISH_PROVIDER_CHASSISOVERVIEW_H
#include "redfish_provider.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

gint32 get_chassis_overview(PROVIDER_PARAS_S *i_paras,
    json_object *o_rsc_jso, json_object *o_err_array_jso, gchar **o_rsp_body_str);
gboolean rm_is_support_chassis_overview(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif 