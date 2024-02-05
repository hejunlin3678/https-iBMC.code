

#ifndef __LIQDET_CARD_H__
#define __LIQDET_CARD_H__

#include "pme/pme.h"
#include "pme_app/pme_app.h"

#ifdef __cplusplus
extern "C" {
#endif

gint32 ctrl_magvalve_state(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 test_leak_detect_alarm(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 waterdetcab_debug(GSList* input_list);
gint32 leakage_debug(GSList* input_list);
void leak_detect_card_start(void);

#ifdef __cplusplus
}
#endif

#endif 
