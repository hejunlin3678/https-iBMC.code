
#ifndef REDFISH_ROUTE_H
#define REDFISH_ROUTE_H


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

RESOURCE_PROVIDER_S *get_resource_provider_info(void);
gsize get_resource_provider_info_size(void);
RESOURCE_PROVIDER_S *get_hmm_resource_provider_info(void);
gsize get_hmm_resource_provider_info_size(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif 