

#include "component/connector.h"
#include "component/base.h"
#include "common.h"
#include "prepare_fru.h"

#define SECURITY_MODULE_CONNECTOR "SecurityModuleConnector"
#define VIR_MULTI_CONNECTOR "VirMultiConnector"


LOCAL gint32 __create_connector(OBJ_HANDLE object_handle, gpointer container)
{
    gchar connector_type[MAX_CONNECTOR_TYPE_LEN] = {0};
    gint32 ret = dal_get_property_value_string(object_handle, CONNECTOR_TYPE, connector_type, MAX_CONNECTOR_TYPE_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s dal_get_property_value_string error, ret = %d", __FUNCTION__, ret);
        return RET_OK;
    }

    if ((strcmp(connector_type, VIRTUAL_CONNECTOR) == 0) || (strcmp(connector_type, SECURITY_MODULE_CONNECTOR) == 0) ||
        (strcmp(connector_type, VIR_MULTI_CONNECTOR) == 0) ||
        (strcmp(connector_type, CONNECTOR_TYPE_NVMEVPDCONNECTOR) == 0)) {
        debug_log(DLOG_INFO, "%s did not care the connector, connector_type is %s", __FUNCTION__, connector_type);
        return RET_OK;
    }

    DFT_COMPONENT_POSN_INFO_S *pos_info = (DFT_COMPONENT_POSN_INFO_S *)g_malloc0(sizeof(DFT_COMPONENT_POSN_INFO_S));
    if (pos_info == NULL) {
        return RET_OK;
    }

    guint32 position_id = 0;
    ret = dal_get_property_value_uint32(object_handle, CONNECTOR_POSITION, &position_id);
    if (ret != RET_OK) {
        g_free(pos_info);
        debug_log(DLOG_ERROR, "%s dal_get_property_value_uint32 error, ret = %d", __FUNCTION__, ret);
        return RET_OK;
    }
    pos_info->position_id = position_id;

    gchar silk_text[MAX_CONNECTOR_SILK_TEXT_LEN] = {0};
    ret = dal_get_property_value_string(object_handle, CONNECTOR_SILKTEXT, silk_text, MAX_CONNECTOR_SILK_TEXT_LEN);
    if (ret != RET_OK) {
        g_free(pos_info);
        debug_log(DLOG_ERROR, "%s dal_get_property_value_string error, ret = %d", __FUNCTION__, ret);
        return RET_OK;
    }
    (void)strncpy_s(pos_info->silk_text, sizeof(pos_info->silk_text), silk_text, sizeof(pos_info->silk_text) - 1);

    GSList **connector_list = (GSList **)container;
    GSList *tmp_list = *connector_list;
    *connector_list = g_slist_insert_sorted(tmp_list, pos_info, position_compare);

    return RET_OK;
}

void init_connectors(void)
{
    GSList **connector_list = get_connectors_addr();

    if (get_fru_callback_flag() == FALSE) {
        return;
    }

    if (*connector_list != NULL) {
        free_connectors();
    }

    (void)dfl_foreach_object(CONNECTOR_CLASS_NAME, __create_connector, connector_list, NULL);
}