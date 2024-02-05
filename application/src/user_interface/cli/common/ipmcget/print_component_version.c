

#include "ipmcget_component_version.h"

#define M3_VERSION_LEN 16

#define RETIMER_VERSION_LEN  20
#define RETIMER_CHIP_NAME_LEN  20



void get_M3_version(void)
{
    gint32 result;
    gchar M3version[M3_VERSION_LEN + 1] = {0};
    OBJ_HANDLE me_info_handle = 0;

    result = dal_get_object_handle_nth(CLASS_ME_INFO, 0, &me_info_handle);
    if (result != RET_OK) {
        return;
    }

    result = dal_get_property_value_string(me_info_handle, PROPERTY_MEINFO_FIRM_VER, M3version, M3_VERSION_LEN);
    if (result != RET_OK) {
        return;
    }

    g_printf("iME            Version:           %s\r\n", M3version);
}


void print_retimer_location_version(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    gchar retimer_version[RETIMER_VERSION_LEN] = {0};
    gchar retimer_chip_name[RETIMER_CHIP_NAME_LEN] = {0};

    ret = dal_get_property_value_string(obj_handle, PROPERTY_RETIMER_SOFT_VER, retimer_version, RETIMER_VERSION_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get retimer version failed: %d, object = %s.", ret, dfl_get_object_name(obj_handle));
        return;
    }
    ret = dal_get_property_value_string(obj_handle, PROPERTY_RETIMER_CHIP_NAME,
        retimer_chip_name, RETIMER_CHIP_NAME_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get retimer chip name failed: %d, object = %s.", ret, dfl_get_object_name(obj_handle));
        return;
    }
    
    if (strlen(retimer_chip_name) > 15) { 
        g_printf("%s    Version:      %s\r\n", retimer_chip_name, retimer_version);
        return;
    }
    g_printf("%-15s Version:          %s\r\n", retimer_chip_name, retimer_version);
}

