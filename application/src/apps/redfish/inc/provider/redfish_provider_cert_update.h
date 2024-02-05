
#ifndef REDFISH_PROVIDER_CERT_UPDATE_H
#define REDFISH_PROVIDER_CERT_UPDATE_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

#define RFPROP_CERTUPDATE_SVC "CertUpdateService"
#define MANAGER_CERTUPDATE_SVC_URI "/redfish/v1/Managers/%s/SecurityService/CertUpdateService"

gint32 cert_update_svc_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 cert_update_svc_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif