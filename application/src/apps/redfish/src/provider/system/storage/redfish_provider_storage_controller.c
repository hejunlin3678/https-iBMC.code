
#include "ctype.h"
#include "redfish_provider.h"
#include "redfish_provider_storage_controller.h"

void get_storage_controller_jbod_state_supported(OBJ_HANDLE controller_handle, json_object *oem_obj)
{
    SML_CTRL_OPTION2_S ctrl_option2;
    json_object *json_state = NULL;
    gint32 ret = dal_get_property_value_uint32(controller_handle, PROPERTY_RAID_CONTROLLER_CTRL_OPTION2,
        &ctrl_option2.opt2_val);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get CtrlOption2 failed, ret = %d.", __FUNCTION__, ret);
        return;
    }
    if (ctrl_option2.opt2_div.ctrl_support_jbod_state != TRUE &&
        ctrl_option2.opt2_div.ctrl_support_jbod_state != FALSE) {
        debug_log(DLOG_MASS, "get CtrlOption2 ctrl_support_jbod_state failed.");
        json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_JBOD_STATE_SUPPORTED, NULL);
        return;
    }
    json_state = json_object_new_boolean(ctrl_option2.opt2_div.ctrl_support_jbod_state);
    json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_JBOD_STATE_SUPPORTED, json_state);
    return;
}


void get_storage_controller_epd_supported(OBJ_HANDLE controller_handle, json_object *oem_obj)
{
    SML_CTRL_OPTION2_S ctrl_option2;
    json_object *json_state = NULL;
    gint32 ret = dal_get_property_value_uint32(controller_handle, PROPERTY_RAID_CONTROLLER_CTRL_OPTION2,
        &ctrl_option2.opt2_val);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get CtrlOption2 failed, ret = %d.", __FUNCTION__, ret);
        return;
    }
    if (ctrl_option2.opt2_div.ctrl_support_epd != TRUE && ctrl_option2.opt2_div.ctrl_support_epd != FALSE) {
        debug_log(DLOG_MASS, "get CtrlOption2 ctrl_support_epd failed.");
        json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_EPD_SUPPORTED, NULL);
        return;
    }
    json_state = json_object_new_boolean(ctrl_option2.opt2_div.ctrl_support_epd);
    json_object_object_add(oem_obj, RFPROP_STORAGE_CONTROLLER_EPD_SUPPORTED, json_state);
    return;
}