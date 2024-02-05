
#ifndef SET_CLP_H
#define SET_CLP_H

#include "pme_app/pme_app.h"
#include "bios_ipmi.h"
#include "pme_app/xmlparser/xmlparser.h"
#include "clp_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

gint32 Bios_Set_Clp_Config(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif 