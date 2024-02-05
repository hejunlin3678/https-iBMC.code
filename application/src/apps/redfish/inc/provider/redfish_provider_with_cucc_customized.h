
#ifndef __REDFISH_PROVIDER_WITH_CUCC_CUSTOMIZED_H__
#define __REDFISH_PROVIDER_WITH_CUCC_CUSTOMIZED_H__
#include "redfish_provider.h"

#ifdef _cplusplus
extern "C" {
#endif

gint32 get_card_type_from_uri(const gchar* rsc_uri, guint8 *card_type, gulong *slot_id);
gint32 cucc_customized_get_npu_handle(gulong card_slot, OBJ_HANDLE *obj_handle);
gboolean check_npu_handle_with_cucc_customized(OBJ_HANDLE obj_handle);
gint32 get_cucc_pciedevices_oem_property(PROVIDER_PARAS_S *i_paras, json_object **o_result_jso);

#define LINK_WIDTH_SIZE 10
#define RFPROP_PCIEDEVICES_MAX_LANES_CUCC "MaxLanes"
#define RFPROP_CUCC_NPU_MEMORY_SIZE_MIB        "MemorySizeMiB"
#define RFPROP_CUCC_NPU_MEMORY_BANDWIDTH        "MemoryBandWidth"
#define RFPROP_PCIECARD_TYPE_NPU    "NPU"
#define RF_NPU_PREFIX   "NPU"

#define NPU_POSITION    "mainboard"

#ifdef _cplusplus
}
#endif 

#endif 