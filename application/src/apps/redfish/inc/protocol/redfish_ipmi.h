

#ifndef __REDFISH_IPMI_H__
#define __REDFISH_IPMI_H__

#include "redfish_util.h"

#include <glib.h>
#include <json.h>

#ifdef _cplusplus
extern "C" {
#endif

#define RFPROP_PROGRESS "Progress"

enum SP_OPERATE_TYPE {
    SP_RESULT = 0,
    SP_TIMEOUT = 1,
    SP_FINISHED = 2
};

extern gint32 IpmiSetSPInfo(const void *req_ptr, gpointer user_data);

#ifdef _cplusplus
}
#endif

#endif 