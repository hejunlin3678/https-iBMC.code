
#ifndef REDFISH_PROVIDER_TPCM_H
#define REDFISH_PROVIDER_TPCM_H
#include "pme_app/dalib/dal_m3_msg_intf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

#define TRUSTED_CENTERSERVER_ADDR "TrustedCenterServerAddr"
#define STANDARD_DIGEST  "StandardDigest"
#define ACTUAL_DIGEST    "ActualDigest"
#define MEASURE_ENABLED  "MeasureEnabled"
#define CONTROL_POLICY    "ControlPolicy"

#define MANAGER_TPCMSERVICE_SVC_URI "/redfish/v1/Managers/%s/SecurityService/TpcmService"
#define TSB_DISABLE 0x10
#define ADDR_INVALID 0X11
#define TSB_DISABLE_MSG   "TsbServerDisable"
#define TSB_SERVER_ERROR  "TsbServerError"
#define TSB_ERROR             0X12
#define DIGEST_SIZE           65
gint32 tpcm_config_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif