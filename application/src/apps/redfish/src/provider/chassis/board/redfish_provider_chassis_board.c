
#include "redfish_provider.h"
#include "redfish_provider_system.h"
#include "redfish_util.h"
#include "redfish_provider_chassis.h"

LOCAL gint32 get_chassis_board_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_board_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_board_id_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_chassis_board_associatedrsc(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_board_descp(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_board_positionid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_board_powerwatts(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_chassis_board_devicelocator(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_board_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_board_devicetype(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_board_location(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_board_productname(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_board_assettag(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_board_boardid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gboolean is_raid_card_type(gchar *classname, guint8 pcietype);
LOCAL gint32 get_pcie_card_boardname(OBJ_HANDLE o_obj_handle, gchar *boardname, guint32 boardname_len);
LOCAL gint32 get_chassis_board_manufacturedate(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_board_support_raidlevels(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_board_pch_model(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_mezzcard_linkprop(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_ciccard_m2prop(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);


LOCAL gint32 get_specific_peripheral_card_obj(OBJ_HANDLE component_handle, OBJ_HANDLE *peripheral_card_handle);
LOCAL gint32 get_chassis_board_work_mode(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_chassis_board_work_mode(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_board_retimer_version(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
                                               json_object **o_result_jso);
LOCAL gint32 get_ar_card_id(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 get_chassis_board_act(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso);
LOCAL void get_chassis_board_action_res(json_object* obj_huawei, const gchar* slot, const gchar* member);
LOCAL gint32 act_chassis_arcardcontrol(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);

#define REAR_BACKPLANE_CONNECT "RearHddBackConnect"
#define INNER_BACKPLANE_CONNECT "InnerHddBackConnect"
#define FRONT_BACKPLANE_CONNECT "FrontHddBackConnect"

#define REAR "Rear"
#define INNER "Inner"
#define FRONT "Front"

LOCAL PROPERTY_PROVIDER_S g_chassis_board_provider[] = {
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_id_name, NULL, NULL, ETAG_FLAG_ENABLE},
    
    
    {RFPROP_COMMON_DESCRIPTION, CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_descp, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_PCIEFUNCTION_CONNECTEDCPU, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_associatedrsc, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_FWINV_POSITION_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_positionid, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_POWER_DATA, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_powerwatts, NULL, NULL, ETAG_FLAG_DISABLE},
    
    {RFPROP_COMMON_NAME, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_id_name, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_DEVICELOCATOR, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_devicelocator, NULL, NULL, ETAG_FLAG_ENABLE},
    {REPROP_SYSTEM_STORAGE_DRIVE_STATUS, CLASS_COMPONENT, PROPERTY_COMPONENT_HEALTH, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_status, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_CARDID, CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICENUM, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_cardid, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_DEVICETYPE, CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_devicetype, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_LOCATION, CLASS_COMPONENT, PROPERTY_COMPONENT_LOCATION, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_location, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_MANUFACTURER, CLASS_COMPONENT, PROPERTY_COMPONENT_MANUFACTURER, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_manufacturer, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_PRODUCTNAME, MAP_PROPERTY_NULL, PROPERTY_ELABEL_PRODUCT_NAME, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_productname, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_SERIALNUMBER, MAP_PROPERTY_NULL, PROPERTY_ELABEL_PRODUCT_SN, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_serialnumber, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_PARTNUMBER, MAP_PROPERTY_NULL, PROPERTY_ELABEL_PRODUCT_PN, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_partnumber, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_ASSETTAG, MAP_PROPERTY_NULL, PROPERTY_ELABEL_PRODUCT_ASSET_TAG, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_assettag, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_CPLDVERSION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_cpldversion, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_BOARDID, CLASS_COMPONENT, PROPERTY_COMPONENT_BOARDID, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_boardid, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_PCBVERSION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_pcbversion, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_BOARDNAME, MAP_PROPERTY_NULL, PROPERTY_ELABEL_BOARD_PRO_NAME, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_boardname, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_MANUFACTUREDATE, MAP_PROPERTY_NULL, PROPERTY_ELABEL_BOARD_MFG_TIME, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_manufacturedate, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_SUPPORT_RAIDLEVEL, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_support_raidlevels, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_PCH_MODEL, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_pch_model, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_LINK_WIDTH_ABILITY, MAP_PROPERTY_NULL, PROPERTY_MEZZ_LINK_WIDTH_ABILITY, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_mezzcard_linkprop, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_LINK_SPEED_ABILITY, MAP_PROPERTY_NULL, PROPERTY_MEZZ_LINK_SPEED_ABILITY, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_mezzcard_linkprop, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_LINK_WIDTH, MAP_PROPERTY_NULL, PROPERTY_MEZZ_LINK_WIDTH, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_mezzcard_linkprop, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_LINK_SPEED, MAP_PROPERTY_NULL, PROPERTY_MEZZ_LINK_SPEED, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_mezzcard_linkprop, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_WORK_MODE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_chassis_board_work_mode, set_chassis_board_work_mode, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_M2DEVICE1_PRESENCE, MAP_PROPERTY_NULL, PROPERTY_CIC_M2APRESENCE, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_ciccard_m2prop, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_M2DEVICE2_PRESENCE, MAP_PROPERTY_NULL, PROPERTY_CIC_M2BPRESENCE, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_ciccard_m2prop, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_RETIMER_VERSION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_retimer_version, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_COMPONENTUID, CLASS_COMPONENT, PROPERTY_COMPONENT_UNIQUE_ID, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_board_componentuid, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ARCARD_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_ar_card_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_ACTIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_chassis_board_act, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BOARD_ARCARD_CTL, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_POWERMGNT,
        SYS_LOCKDOWN_ALLOW, NULL, NULL, act_chassis_arcardcontrol, ETAG_FLAG_ENABLE}
};

LOCAL INT2STR_MAP_S g_component_map[] = {
    {COMPONENT_TYPE_HDD_BACKPLANE,      RFPROP_TYPE_HDDBACKPLANE},
    {COMPONENT_TYPE_RAID_CARD,          RFPROP_TYPE_RAIDCARD},
    {COMPONENT_TYPE_PCIE_CARD,          RFPROP_TYPE_PCIECARD},
    {COMPONENT_TYPE_MEZZ,               RFPROP_TYPE_MEZZCARD},
    {COMPONENT_TYPE_NIC_CARD,           RFPROP_TYPE_NIC},
    {COMPONENT_TYPE_MEMORY_RISER,       RFPROP_TYPE_MEMORYBOARD},
    {COMPONENT_TYPE_PCIE_RISER,         RFPROP_TYPE_PCIERISER},
    {COMPONENT_TYPE_MAINBOARD,          RFPROP_TYPE_MAINBOARD},
    {COMPONENT_TYPE_FAN_BACKPLANE,      RFPROP_TYPE_FANBACKPLANE},
    {COMPONENT_TYPE_SWITCH_MEZZ,        RFPROP_TYPE_SWITCHMMEZZ},
    {COMPONENT_TYPE_IO_BOARD,           RFPROP_TYPE_IOBOARD},
    {COMPONENT_TYPE_BOARD_CPU,          RFPROP_TYPE_CPUBOARD},
    {COMPONENT_TYPE_EXPAND_BOARD,       RFPROP_TYPE_EXPANDBOARD},
    {COMPONENT_TYPE_LED_BOARD,          RFPROP_TYPE_LEDBOARD},
    {COMPONENT_TYPE_PIC_CARD,           RFPROP_TYPE_PICCARD},
    
    {COMPONENT_TYPE_TWIN_NODE_BACKPLANE, RFPROP_TYPE_TWIN_NODE_BACKPLANE},
    
    
    { COMPONENT_TYPE_BOARD_PCIE_ADAPTER, RFPROP_TYPE_BOARD_PCIE_ADAPTER },
    
    {COMPONENT_TYPE_GPU_BOARD,          RFPROP_TYPE_PCIECARD_CARRIER_BOARD},
    
    { COMPONENT_TYPE_PASS_THROUGH_CARD, RFPROP_TYPE_PASS_THROUGH_CARD },
    
    
    { COMPONENT_TYPE_PSU_BACKPLANE,     RFPROP_TYPE_POWER_BOARD },
    
    
    {COMPONENT_TYPE_HORIZ_CONN_BRD,     RFPROP_TYPE_HORIZ_CONN_BRD},
    
	
    
    {COMPONENT_TYPE_CHASSIS_BACKPLANE,  RFPROP_TYPE_CHASSIS_BACKPLANE},
    
	
    {COMPONENT_TYPE_ASSET_LOCATE_BRD, CLASS_ASSET_LOCATE_BOARD},
    {COMPONENT_TYPE_EXPANDER, RFPROP_TYPE_EXPANDBOARD},
    {COMPONENT_TYPE_EXPBOARD, RFPROP_TYPE_EXPANDBOARD},
    {COMPONENT_TYPE_EPM, RFPROP_TYPE_EXPANDBOARD},
    {COMPONENT_TYPE_FAN_MODULE, RFPROP_TYPE_FAN_MODULE},
    
    {COMPONENT_TYPE_LEAKDET_CARD, RFPROP_TYPE_LEAKDETECTIONCARD},
    {COMPONENT_TYPE_AR_CARD, RFPROP_TYPE_ARCARD},
    {COMPONENT_TYPE_SOC_BOARD, RFPROP_TYPE_SOCBOARD}
};


LOCAL gboolean redfish_board_string_check(gchar *string, guint32 size)
{
    if ((NULL == string) || (0 == strlen(string)) || (0 == size) || (0 == g_strcmp0("N/A", string)) ||
        (0 == g_strcmp0("Unknown", string)) || (0 == g_strcmp0(string, "null")) || (0 == g_strcmp0(string, "NA"))) {
        debug_log(DLOG_MASS, "%s, %d: the string invalid.", __FUNCTION__, __LINE__);
        return TRUE;
    }

    return FALSE;
}


LOCAL gboolean redfish_get_class_handle_by_pciehandle(OBJ_HANDLE pcie_handle, const gchar *class_name,
    OBJ_HANDLE *net_card_handle)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    gchar obj_ref_object[MAX_NAME_SIZE] = {0};
    gchar obj_ref_property[MAX_NAME_SIZE] = {0};
    gchar obj_pcie_object[MAX_NAME_SIZE] = {0};
    gchar obj_pcie_property[MAX_NAME_SIZE] = {0};

    if (NULL == class_name || 0 == strlen(class_name)) {
        debug_log(DLOG_MASS, "%s, %d: input parameter invalid.", __FUNCTION__, __LINE__);
        return FALSE;
    }

    ret = dfl_get_referenced_property(pcie_handle, PROPETRY_PCIECARD_SLOT, obj_pcie_object, obj_pcie_property,
        MAX_NAME_SIZE, MAX_NAME_SIZE);
    return_val_do_info_if_fail(VOS_OK == ret, FALSE,
        debug_log(DLOG_MASS, "%s, %d: dfl_get_referenced_property fail.", __FUNCTION__, __LINE__));

    ret = dfl_get_object_list(class_name, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, FALSE,
        debug_log(DLOG_MASS, "%s, %d: get obj list fail.", __FUNCTION__, __LINE__));

    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        if (0 == g_strcmp0(class_name, CLASS_NETCARD_NAME)) {
            ret = dfl_get_referenced_property((OBJ_HANDLE)obj_node->data, PROPERTY_NETCARD_SLOT_ID, obj_ref_object,
                obj_ref_property, MAX_NAME_SIZE, MAX_NAME_SIZE);
            continue_if_expr(VOS_OK != ret);
        } else {
            ret = dfl_get_referenced_property((OBJ_HANDLE)obj_node->data, PROPERTY_CARD_SLOT, obj_ref_object,
                obj_ref_property, MAX_NAME_SIZE, MAX_NAME_SIZE);
            continue_if_expr(VOS_OK != ret);
        }

        if (0 == g_strcmp0(obj_pcie_object, obj_ref_object)) {
            *net_card_handle = (OBJ_HANDLE)obj_node->data;
            g_slist_free(obj_list);
            return TRUE;
        }
    }

    g_slist_free(obj_list);
    return FALSE;
}


LOCAL gboolean get_refcomponent_class_obj(const gchar *classname, OBJ_HANDLE obj_handle, OBJ_HANDLE *o_obj_handle)
{
    gint32 ret;
    gboolean result = FALSE;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE referencd_handle = 0;
    const char *component_name = NULL;
    const char *ref_component_name = NULL;

    ret = dfl_get_object_list(classname, &obj_list);
    return_val_do_info_if_expr(VOS_OK != ret, result,
        debug_log(DLOG_MASS, "%s, %d: get obj list fail.", __FUNCTION__, __LINE__));

    component_name = dfl_get_object_name(obj_handle);
    return_val_do_info_if_expr(NULL == component_name, result, debug_log(DLOG_ERROR, "get object name faild\n");
        g_slist_free(obj_list));

    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        ret = dfl_get_referenced_object((OBJ_HANDLE)obj_node->data, PROPERTY_CPUBOARD_REFCOMPONENT, &referencd_handle);
        continue_if_expr(VOS_OK != ret);

        ref_component_name = dfl_get_object_name(referencd_handle);
        continue_if_expr(NULL == ref_component_name);

        if (0 == g_strcmp0(component_name, ref_component_name)) {
            *o_obj_handle = (OBJ_HANDLE)obj_node->data;
            result = TRUE;
            break;
        }

        ref_component_name = NULL;
    }

    g_slist_free(obj_list);
    return result;
}

LOCAL gboolean redfish_get_one_refproperty_object_handle(OBJ_HANDLE obj_handle, const gchar *classname,
    OBJ_HANDLE *o_obj_handle)
{
    gint32 ret;
    gboolean result = FALSE;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    const gchar *obj_name = NULL;
    gchar comp_ref_object[MAX_NAME_SIZE] = {0};
    gchar ref_property[MAX_NAME_SIZE] = {0};
    gchar obj_ref_object[MAX_NAME_SIZE] = {0};
    gchar obj_ref_property[MAX_NAME_SIZE] = {0};

    if (obj_handle == 0 || classname == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: input parameter invalid.", __FUNCTION__, __LINE__);
        return result;
    }

    
    
    if (0 == g_strcmp0(classname, CLASS_HDDBACKPLANE_NAME)
        
        || 0 == g_strcmp0(classname, CLASS_HORIZ_CONN_BRD)) {
        
        
        
        ret = dal_get_specific_object_position(obj_handle, classname, o_obj_handle);
        
        return_val_do_info_if_expr(VOS_OK != ret, result,
            debug_log(DLOG_MASS, "%s, %d: get dal_get_specific_object_position fail.", __FUNCTION__, __LINE__));
        return TRUE;
        
    }

    
    
    ret = dfl_get_referenced_property(obj_handle, PROPERTY_COMPONENT_DEVICENUM, comp_ref_object, ref_property,
        MAX_NAME_SIZE, MAX_NAME_SIZE);

    return_val_do_info_if_expr(VOS_OK != ret, result,
        debug_log(DLOG_MASS, "%s, %d: get referenced property fail.", __FUNCTION__, __LINE__));

    ret = dfl_get_object_list(classname, &obj_list);
    return_val_do_info_if_expr(VOS_OK != ret, result,
        debug_log(DLOG_MASS, "%s, %d: get obj list fail.", __FUNCTION__, __LINE__));

    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        
        if (g_strcmp0(classname, CLASS_IOBOARD_NAME) == 0) {
            obj_name = dfl_get_object_name((OBJ_HANDLE)obj_node->data);
            do_if_expr(NULL == obj_name, continue);
        } 
        else {
            ret = dfl_get_referenced_property((OBJ_HANDLE)obj_node->data, ref_property, obj_ref_object,
                obj_ref_property, MAX_NAME_SIZE, MAX_NAME_SIZE);
            continue_if_expr(VOS_OK != ret);
        }

        if ((g_strcmp0(comp_ref_object, obj_ref_object) == 0) || (g_strcmp0(comp_ref_object, obj_name) == 0)) {
            *o_obj_handle = (OBJ_HANDLE)obj_node->data;
            result = TRUE;
            break;
        }
    }

    do_if_true(NULL != obj_list, g_slist_free(obj_list));
    return result;
}

LOCAL gboolean redfish_get_refproperty_object(OBJ_HANDLE obj_handle, const gchar *classname, OBJ_HANDLE *o_obj_handle)
{
    gboolean result = FALSE;
    GSList *obj_list = NULL;
    OBJ_HANDLE comp_handle = 0;
    gchar comp_ref_object[MAX_NAME_SIZE] = {0};
    gchar ref_property[MAX_NAME_SIZE] = {0};
    gchar ref_ref_object[MAX_NAME_SIZE] = {0};
    gchar ref_ref_property[MAX_NAME_SIZE] = {0};
    gchar obj_ref_object[MAX_NAME_SIZE] = {0};
    gchar obj_ref_property[MAX_NAME_SIZE] = {0};

    if ((obj_handle == 0) || (classname == NULL)) {
        debug_log(DLOG_ERROR, "%s, %d: input parameter invalid.", __FUNCTION__, __LINE__);
        return result;
    }

    
    if (g_strcmp0(classname, CLASS_CPUBOARD_NAME) == 0 || g_strcmp0(classname, CLASS_EXPBOARD_NAME) == 0) {
        return get_refcomponent_class_obj(classname, obj_handle, o_obj_handle);
    }

    
    
    
    gint32 ret = dfl_get_referenced_property(obj_handle, PROPERTY_COMPONENT_DEVICENUM, comp_ref_object, ref_property,
        MAX_NAME_SIZE, MAX_NAME_SIZE);
    return_val_do_info_if_expr(ret != VOS_OK, result,
        debug_log(DLOG_MASS, "%s, %d: get referenced property fail.", __FUNCTION__, __LINE__));

    
    ret = dfl_get_object_handle((const gchar *)comp_ref_object, &comp_handle);
    return_val_do_info_if_expr(ret != VOS_OK, result,
        debug_log(DLOG_MASS, "%s, %d: get obj handle fail.", __FUNCTION__, __LINE__));

    
    ret = dfl_get_referenced_property(comp_handle, ref_property, ref_ref_object, ref_ref_property, MAX_NAME_SIZE,
        MAX_NAME_SIZE);
    return_val_do_info_if_expr(ret != VOS_OK, result,
        debug_log(DLOG_MASS, "%s, %d: get referenced property fail.", __FUNCTION__, __LINE__));

    ret = dfl_get_object_list(classname, &obj_list);
    return_val_do_info_if_expr(ret != VOS_OK, result,
        debug_log(DLOG_MASS, "%s, %d: get obj list fail.", __FUNCTION__, __LINE__));

    for (GSList *obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        const gchar *prop_name = ref_ref_property;
        
        if (g_strcmp0(classname, CLASS_RAIDCARD_NAME) == 0 || g_strcmp0(classname, CLASS_NETCARD_NAME) == 0) {
            
            prop_name = PROPERTY_NETCARD_SLOT_ID;
        } else if ((g_strcmp0(classname, CLASS_PCIECARD_NAME) == 0) || (g_strcmp0(classname, CLASS_OCP_CARD) == 0)) {
            prop_name = PROPETRY_PCIECARD_SLOT;
        } else if (g_strcmp0(classname, CLASS_SOC_BOARD) == 0) {
            prop_name = PROPERTY_SOC_BOARD_ID;
        }
        ret = dfl_get_referenced_property((OBJ_HANDLE)obj_node->data, prop_name, obj_ref_object,
            obj_ref_property, MAX_NAME_SIZE, MAX_NAME_SIZE);

        continue_if_expr(ret != VOS_OK);

        if (g_strcmp0(ref_ref_object, obj_ref_object) == 0) {
            *o_obj_handle = (OBJ_HANDLE)obj_node->data;
            result = TRUE;
            break;
        }
    }

    do_if_true(NULL != obj_list, g_slist_free(obj_list));
    return result;
}


LOCAL gboolean _get_device_type_0x26_class_name(OBJ_HANDLE obj_handle, gchar *o_classname, gint32 length)
{
    gint32 ret;
    OBJ_HANDLE obj_handle_tmp = 0;

    return_val_if_expr(0 == obj_handle || NULL == o_classname || 0 == length, FALSE);

    ret = dal_get_specific_object_position(obj_handle, CLASS_PCIETRANSFORMCARD, &obj_handle_tmp);
    return_val_do_info_if_expr(DFL_OK == ret, TRUE,
        (void)strncpy_s(o_classname, length, CLASS_PCIETRANSFORMCARD, strlen(CLASS_PCIETRANSFORMCARD)));

    ret = dal_get_specific_object_position(obj_handle, CLASS_RISERPCIECARD_NAME, &obj_handle_tmp);
    return_val_do_info_if_expr(DFL_OK == ret, TRUE,
        (void)strncpy_s(o_classname, length, CLASS_RISERPCIECARD_NAME, strlen(CLASS_RISERPCIECARD_NAME)));

    ret = dal_get_specific_object_position(obj_handle, CLASS_IOBOARD_NAME, &obj_handle_tmp);
    return_val_do_info_if_expr(DFL_OK == ret, TRUE,
        (void)strncpy_s(o_classname, length, CLASS_IOBOARD_NAME, strlen(CLASS_IOBOARD_NAME)));

    return FALSE;
}


LOCAL gboolean _get_power_bp_class_name(OBJ_HANDLE obj_handle, gchar *o_classname, gint32 length)
{
    OBJ_HANDLE  target_obj = 0;
    gboolean    ret_val = FALSE;
    errno_t     err_val = EOK;
    gint32 ret;

    return_val_if_expr(o_classname == NULL || length <= 0, FALSE);

    // /profile/atlas_01/14100075_66.xml                文件类名是   Card
    // /profile/8100_server_01/9008v5/14100249_62.xml   文件类名是IOBoard
    if (DFL_OK == dal_get_specific_object_string(CLASS_IOBOARD_NAME, PROPERTY_IOBOARD_REFCOMPONENT,
        dfl_get_object_name(obj_handle), &target_obj)) {
        err_val = strncpy_s(o_classname, length, CLASS_IOBOARD_NAME, strlen(CLASS_IOBOARD_NAME));
        return_val_if_expr(err_val != EOK, FALSE);
        ret_val = TRUE;
    } else if (DFL_OK == dal_get_specific_object_position(obj_handle, CLASS_CARD_NAME, &target_obj)) {
        err_val = strncpy_s(o_classname, length, CLASS_CARD_NAME, strlen(CLASS_CARD_NAME));
        return_val_if_expr(err_val != EOK, FALSE);
        ret_val = TRUE;
    } else {
        ret = dal_get_specific_object_position(obj_handle, CLASS_ELABEL, &target_obj);
        if (ret == RET_OK) {
            ret_val = TRUE;
        }
    }

    return ret_val;
}


gboolean redfish_get_board_class_name(OBJ_HANDLE obj_handle, gchar* o_classname, gint32 length)
{
    errno_t safe_fun_ret = EOK;
    gboolean match_flag = FALSE;
    guchar component_type = 0;
    gchar class_name[MAX_NAME_SIZE] = {0};

    char *classname = NULL;

    if (obj_handle == 0) {
        debug_log(DLOG_ERROR, "%s, %d: obj handle is Invaid.", __FUNCTION__, __LINE__);
        return match_flag;
    }

    
    gint32 ret = dfl_get_class_name(obj_handle, class_name, MAX_NAME_SIZE);
    if (VOS_OK == ret && 0 == g_strcmp0(class_name, CLASS_PERIPHERAL_CARD_NAME)) {
        safe_fun_ret = strcpy_s(o_classname, length, class_name);
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        match_flag = TRUE;
        return match_flag;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
    return_val_do_info_if_fail(VOS_OK == ret, match_flag,
        debug_log(DLOG_MASS, "%s, %d: get property value byte fail.\r\n", __FUNCTION__, __LINE__));

    
    if (COMPONENT_TYPE_BOARD_PCIE_ADAPTER == component_type) {
        
        match_flag = _get_device_type_0x26_class_name(obj_handle, o_classname, length);
        return match_flag;
    
    } else if (COMPONENT_TYPE_PSU_BACKPLANE == component_type) {
        
        match_flag = _get_power_bp_class_name(obj_handle, o_classname, length);
        return match_flag;
    } else if ((dal_match_product_id(PRODUCT_ID_PANGEA_V6) == TRUE) && (component_type == COMPONENT_TYPE_CHASSIS_BACKPLANE)) {
        ret = strncpy_s(o_classname, length, RFPROP_TYPE_CHASSIS_BACKPLANE, strlen(RFPROP_TYPE_CHASSIS_BACKPLANE));
        if (ret == RET_OK) {
            return TRUE;
        }
        return FALSE;
    }

    classname = _dal_devicetype_to_class(component_type);
    if (NULL != classname) {
        safe_fun_ret = strcpy_s(o_classname, length, classname);
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        match_flag = TRUE;
    }
    if ((g_strcmp0(classname, CLASS_PERIPHERAL_CARD_NAME) == 0) && dal_check_if_vsmm_supported()) {
        match_flag = FALSE;
    }

    return match_flag;
}


LOCAL gint32 redfish_board_elabel_info(OBJ_HANDLE obj_handle, gchar *property_name, gchar *o_value, gint32 value_length)
{
    errno_t safe_fun_ret;
    gint32 ret;
    guchar component_fruid = 0;
    OBJ_HANDLE fru_handle = 0;
    OBJ_HANDLE ref_handle = 0;
    gchar ref_string[MAX_STRBUF_LEN] = {0};

    if (obj_handle == 0 || (property_name == NULL)) {
        debug_log(DLOG_ERROR, "%s, %d: obj handle is Invaid.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_FRUID, &component_fruid);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get property value byte fail.", __FUNCTION__, __LINE__));

    ret = dal_get_specific_object_byte(CLASS_FRU, PROPERTY_FRU_ID, component_fruid, &fru_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get specific object fail.", __FUNCTION__, __LINE__));

    
    ret = rf_get_elabel_handle_of_fru(fru_handle, &ref_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get referenced object fail.", __FUNCTION__, __LINE__));
    

    (void)dal_get_property_value_string(ref_handle, property_name, ref_string, sizeof(ref_string));
    safe_fun_ret = strncpy_s(o_value, value_length, ref_string, value_length - 1);
    return_val_do_info_if_expr(safe_fun_ret != EOK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    if (0 != strlen(ref_string)) {
        safe_fun_ret = memcpy_s(o_value, value_length, ref_string, strlen(ref_string));
        return_val_do_info_if_expr(safe_fun_ret != EOK, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        return VOS_OK;
    }

    return VOS_ERR;
}


LOCAL gint32 get_chassis_board_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar str_context[MAX_STRBUF_LEN] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    
    OBJ_HANDLE chassis_comp_handle;
    

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    
    chassis_comp_handle = (OBJ_HANDLE)g_slist_nth_data(i_paras->parent_obj_handle_list, 0);

    ret = rf_gen_chassis_component_id(NULL, chassis_comp_handle, slot_id, MAX_RSC_ID_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));
    

    ret = snprintf_s(str_context, sizeof(str_context), sizeof(str_context) - 1, CHASSIS_BOARD_METADATA, slot_id);
    do_if_expr(0 >= ret, debug_log(DLOG_ERROR, "%s %d:snprintf_s fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string((const gchar *)str_context);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_chassis_board_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar member_id[MAX_STRBUF_LEN] = {0};
    gchar str_odataid[MAX_STRBUF_LEN] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    OBJ_HANDLE chassis_comp_handle;

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    chassis_comp_handle = (OBJ_HANDLE)g_slist_nth_data(i_paras->parent_obj_handle_list, 0);

    ret = rf_gen_chassis_component_id(NULL, chassis_comp_handle, slot_id, MAX_RSC_ID_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    ret = get_object_obj_location_devicename(i_paras->obj_handle, member_id, sizeof(member_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    ret = snprintf_s(str_odataid, sizeof(str_odataid), sizeof(str_odataid) - 1, URI_FORMAT_CHASSIS_BOARD, slot_id,
        member_id);
    do_if_expr(0 >= ret, debug_log(DLOG_ERROR, "%s %d: snprintf_s fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string((const gchar *)str_odataid);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_chassis_board_id_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    errno_t safe_fun_ret = EOK;
    gchar device_name[MAX_STRBUF_LEN] = {0};
    gchar device_location[MAX_STRBUF_LEN] = {0};
    gchar str_id[MAX_STRBUF_LEN] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    gchar class_name[MAX_NAME_SIZE] = {0};

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    
    (void)dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE);

    if (0 == g_strcmp0(class_name, CLASS_PERIPHERAL_CARD_NAME)) {
        safe_fun_ret =
            strncpy_s(device_location, sizeof(device_location), RF_PREFIX_PERIPHERAL, strlen(RF_PREFIX_PERIPHERAL));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        (void)dal_get_property_value_string(i_paras->obj_handle, PROPERTY_CARD_NAME, device_name, sizeof(device_name));
    } else {
        (void)dal_get_property_value_string(i_paras->obj_handle, PROPERTY_COMPONENT_DEVICE_NAME, device_name,
            sizeof(device_name));

        (void)dal_get_property_value_string(i_paras->obj_handle, PROPERTY_COMPONENT_LOCATION, device_location,
            sizeof(device_location));
    }

    

    (void)dal_clear_string_blank(device_name, sizeof(device_name));
    (void)dal_clear_string_blank(device_location, sizeof(device_location));

    ret = snprintf_s(str_id, sizeof(str_id), sizeof(str_id) - 1, "%s%s", device_location, device_name);
    do_val_if_expr(0 >= ret, debug_log(DLOG_ERROR, "%s %d : snprintf_s fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string((const gchar *)str_id);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gboolean check_pciecard_class(const gchar *class_name)
{
    if ((g_strcmp0(class_name, CLASS_PCIECARD_NAME) == 0 || g_strcmp0(class_name, CLASS_CIC_CARD) == 0 ||
        g_strcmp0(class_name, CLASS_NETCARD_NAME) == 0)) {
        return TRUE;
    } else {
        return FALSE;
    }
}


LOCAL gboolean match_descp_with_classname(gchar *classname, gsize lens)
{
    gchar *class_name_table[] = {
        CLASS_M2TRANSFORMCARD,
        CLASS_EXPBOARD_NAME,
        CLASS_PASSTHROUGH_CARD,
        CLASS_PCIETRANSFORMCARD,
        CLASS_MEZZ,
        CLASS_HORIZ_CONN_BRD,
        CLASS_AR_CARD_NAME
    };
    size_t count = sizeof(class_name_table) / sizeof(class_name_table[0]);

    for (size_t i = 0; i < count; i++) {
        if (!g_strcmp0(classname, class_name_table[i])) {
            return TRUE;
        }
    }
    return FALSE;
}




LOCAL gint32 get_chassis_board_descp(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    errno_t safe_fun_ret = EOK;
    gchar class_name[MAX_STRBUF_LEN] = {0};
    gchar descp_str[MAX_PCIE_CARD_DESC_LEN] = {0};
    OBJ_HANDLE card_handle = 0;

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    gboolean ret_bool = redfish_get_board_class_name(i_paras->obj_handle, class_name, sizeof(class_name));
    return_val_do_info_if_fail(ret_bool, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get class name fail.", __FUNCTION__, __LINE__));

    
    ret = dal_get_specific_object_position(i_paras->obj_handle, (const gchar *)class_name, &card_handle);
    do_info_if_true(VOS_OK != ret,
        debug_log(DLOG_MASS, "%s, %d: get %s obj handle fail.", __FUNCTION__, __LINE__, class_name));
    
    if (match_descp_with_classname(class_name, sizeof(class_name))) {
        ret = dal_get_property_value_string(card_handle, PROPERTY_CARD_DESC, descp_str, sizeof(descp_str));
    } else if (0 == g_strcmp0(class_name, CLASS_PERIPHERAL_CARD_NAME) || 0 == g_strcmp0(class_name, CLASS_CARD_NAME)) {
        return get_chassis_board_devicetype(i_paras, o_message_jso, o_result_jso);
    } else if (check_pciecard_class(class_name)) {
        ret = dal_get_property_value_string(card_handle, PROPERTY_PCIE_CARD_DESC, descp_str, sizeof(descp_str));
    } else if (0 == g_strcmp0(class_name, CLASS_MAINBOARD_NAME) || 0 == g_strcmp0(class_name, RFPROP_CLASS_LEDBOARD) ||
        0 == g_strcmp0(class_name, RFPROP_TYPE_TWIN_NODE_BACKPLANE) ||
        0 == g_strcmp0(class_name, CLASS_GPUBOARD_NAME)) {
        safe_fun_ret = strcpy_s(descp_str, sizeof(descp_str), class_name);
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        ret = VOS_OK;
    } else if (0 == g_strcmp0(class_name, CLASS_SOC_BOARD)) {
        ret = dal_get_property_value_string(card_handle, PROPERTY_SOC_BOARD_DESC, descp_str, sizeof(descp_str));
    } else {
        ret = dal_get_property_value_string(card_handle, PROPERTY_CARD_TYPE, descp_str, sizeof(descp_str));
    }

    if (VOS_OK != ret) {
        safe_fun_ret = strcpy_s(descp_str, sizeof(descp_str), class_name);
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }

    *o_result_jso = json_object_new_string((const gchar *)descp_str);
    do_if_expr(NULL == *o_result_jso, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_chassis_board_associatedrsc(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar class_name[MAX_STRBUF_LEN] = {0};
    OBJ_HANDLE card_handle = 0;
    gboolean ret_bool;

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    ret_bool = redfish_get_board_class_name(i_paras->obj_handle, class_name, sizeof(class_name));
    return_val_do_info_if_fail(ret_bool, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get class name fail.", __FUNCTION__, __LINE__));

    ret = dal_get_specific_object_position(i_paras->obj_handle, (const gchar *)class_name, &card_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get obj handle fail.", __FUNCTION__, __LINE__));

    if (0 == g_strcmp0(class_name, CLASS_PCIECARD_NAME) || 0 == g_strcmp0(class_name, CLASS_NETCARD_NAME) ||
        0 == g_strcmp0(class_name, CLASS_MEZZ) || 0 == g_strcmp0(class_name, CLASS_RAIDCARD_NAME)) {
        ret = get_chassis_pciefunction_oem_associatedresource(card_handle,
            o_result_jso); // 复用pciefunction资源中资源归属获取接口
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get %s associated resource fail.", __FUNCTION__, __LINE__,
            dfl_get_object_name(card_handle)));
    } else {
        return HTTP_INTERNAL_SERVER_ERROR; // 其他卡不支持资源归属
    }

    return HTTP_OK;
}


LOCAL gint32 get_chassis_board_positionid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar class_name[MAX_STRBUF_LEN] = {0};
    gchar position_str[MAX_STRBUF_LEN] = {0};
    OBJ_HANDLE card_handle = 0;
    gboolean ret_bool;
    guint32 logic_unit = 0;

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    ret_bool = redfish_get_board_class_name(i_paras->obj_handle, class_name, sizeof(class_name));
    return_val_do_info_if_fail(ret_bool, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get class name fail.", __FUNCTION__, __LINE__));

    if (0 == g_strcmp0(class_name, CLASS_HDDBACKPLANE_NAME) || 0 == g_strcmp0(class_name, CLASS_RISERPCIECARD_NAME) ||
        0 == g_strcmp0(class_name, CLASS_CPUBOARD_NAME) || 0 == g_strcmp0(class_name, CLASS_MEMBOARD_NAME) ||
        0 == g_strcmp0(class_name, CLASS_IOBOARD_NAME) || 0 == g_strcmp0(class_name, CLASS_GPUBOARD_NAME) ||
        0 == g_strcmp0(class_name, CLASS_FANBOARD_NAME)) {
        ret = dal_get_specific_object_position(i_paras->obj_handle, (const gchar *)class_name, &card_handle);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get obj handle fail.", __FUNCTION__, __LINE__));

        ret = dal_get_property_value_uint32(card_handle, PROPERTY_FIRMWARE_LOGICUNIT,
            &logic_unit); // 位号不能为0，0为默认值
        return_val_do_info_if_fail(VOS_OK == ret && 0 != logic_unit, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get %s associated resource fail.", __FUNCTION__, __LINE__,
            dfl_get_object_name(card_handle)));

        (void)snprintf_s(position_str, sizeof(position_str), sizeof(position_str) - 1, "U%u", logic_unit);

        *o_result_jso = json_object_new_string((const gchar *)position_str);
        return HTTP_OK;
    }

    return HTTP_INTERNAL_SERVER_ERROR; // 其他板卡不支持位号
}


LOCAL gint32 get_chassis_board_powerwatts(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar class_name[MAX_STRBUF_LEN] = {0};
    OBJ_HANDLE card_handle = 0;
    gboolean ret_bool;
    guint16 power_watts = 0;

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    ret_bool = redfish_get_board_class_name(i_paras->obj_handle, class_name, sizeof(class_name));
    return_val_do_info_if_fail(ret_bool, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get class name fail.", __FUNCTION__, __LINE__));

    if (0 == g_strcmp0(class_name, CLASS_CPUBOARD_NAME) || 0 == g_strcmp0(class_name, CLASS_IOBOARD_NAME) ||
        0 == g_strcmp0(class_name, CLASS_GPUBOARD_NAME)) {
        ret = dal_get_specific_object_position(i_paras->obj_handle, (const gchar *)class_name, &card_handle);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get obj handle fail.", __FUNCTION__, __LINE__));

        ret =
            dal_get_property_value_uint16(card_handle, PROPERTY_GPUBOARD_POWER, &power_watts); // 功率不能为0，0为默认值
        return_val_do_info_if_fail(VOS_OK == ret && 0 != power_watts, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get %s associated resource fail.", __FUNCTION__, __LINE__,
            dfl_get_object_name(card_handle)));

        *o_result_jso = json_object_new_int(power_watts);
        return HTTP_OK;
    }

    return HTTP_INTERNAL_SERVER_ERROR; // 其他板卡不支持位号
}




LOCAL gint32 get_chassis_board_devicelocator(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar device_name[MAX_STRBUF_LEN] = {0};
    gchar class_name[MAX_NAME_SIZE] = {0};

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    (void)dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE); // 不需判断返回值

    if (0 != g_strcmp0(class_name, CLASS_PERIPHERAL_CARD_NAME)) {
        (void)dal_get_property_value_string(i_paras->obj_handle, PROPERTY_COMPONENT_DEVICE_NAME, device_name,
            sizeof(device_name));
        (void)dal_clear_string_blank(device_name, sizeof(device_name));
        *o_result_jso = json_object_new_string((const gchar *)device_name);
    }

    

    do_if_expr(NULL == *o_result_jso, return HTTP_INTERNAL_SERVER_ERROR);
    return HTTP_OK;
}


gint32 get_chassis_board_cardid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 component_num = 0;
    gchar class_name[MAX_NAME_SIZE] = {0};

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    (void)dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE);

    if (0 != g_strcmp0(class_name, CLASS_PERIPHERAL_CARD_NAME)) {
        (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_COMPONENT_DEVICENUM,
            &component_num);

        *o_result_jso = json_object_new_int(component_num);
    }

    

    do_if_expr(NULL == *o_result_jso, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_chassis_board_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 health;
    gchar *state = "Enabled";
    gboolean result;
    gchar classname[MAX_STRBUF_LEN] = {0};
    OBJ_HANDLE o_obj_handle = 0;
    gchar perial_classname[MAX_STRBUF_LEN] = {0};

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    result = redfish_get_board_class_name(i_paras->obj_handle, classname, sizeof(classname));
    goto_label_do_info_if_fail(result, __EXIT, (health = DRIVE_INVALID_VALUE));

    
    (void)dfl_get_class_name(i_paras->obj_handle, perial_classname, MAX_STRBUF_LEN);
    

    
    
    if (0 == g_strcmp0(classname, RFPROP_CLASS_LEDBOARD) || 0 == g_strcmp0(classname, CLASS_FANBOARD_NAME) ||
        0 == g_strcmp0(perial_classname, CLASS_PERIPHERAL_CARD_NAME)) {
        health = 0;
    } else if (0 == g_strcmp0(classname, CLASS_MAINBOARD_NAME)) {
        
        ret = dfl_get_object_handle(CLASS_MAINBOARD_NAME, &o_obj_handle);
        goto_label_do_info_if_fail(VOS_OK == ret, __EXIT, (health = DRIVE_INVALID_VALUE));
        (void)dal_get_property_value_byte(o_obj_handle, RFPROP_BOARD_HEALTH, &health);
    } 
    else if (0 == g_strcmp0(classname, CLASS_HDDBACKPLANE_NAME) || (0 == g_strcmp0(classname, CLASS_IOBOARD_NAME)) ||
        (0 == g_strcmp0(classname, CLASS_M2TRANSFORMCARD)) ||
        (0 ==
        g_strcmp0(classname, CLASS_PCIETRANSFORMCARD))) { 
        
        
        result = redfish_get_one_refproperty_object_handle(i_paras->obj_handle, classname, &o_obj_handle);
        
        goto_label_do_info_if_fail(result, __EXIT, (health = DRIVE_INVALID_VALUE));
        (void)dal_get_property_value_byte(o_obj_handle, RFPROP_BOARD_HEALTH, &health);
    } 
    
    else if (0 == g_strcmp0(classname, RFPROP_TYPE_TWIN_NODE_BACKPLANE) ||
        (0 == g_strcmp0(classname, CLASS_GPUBOARD_NAME))) {
        
        (void)dal_get_property_value_byte(i_paras->obj_handle, RFPROP_BOARD_HEALTH, &health);
    } 
    else {
        result = redfish_get_refproperty_object(i_paras->obj_handle, classname, &o_obj_handle);
        goto_label_do_info_if_fail(result, __EXIT, (health = DRIVE_INVALID_VALUE));
        (void)dal_get_property_value_byte(o_obj_handle, RFPROP_BOARD_HEALTH, &health);
    }

__EXIT:

    
    do_val_if_expr(DRIVE_INVALID_VALUE == health,
        (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_COMPONENT_HEALTH, &health));
    

    (void)get_resource_status_property(&health, NULL, state, o_result_jso, TRUE);
    return HTTP_OK;
}


LOCAL gint32 get_chassis_board_devicetype(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso)
{
    guint8 component_type = 0;
    const gchar *str_type = NULL;
    gchar class_name[MAX_NAME_SIZE] = {0};
    gint32 size = G_N_ELEMENTS(g_component_map);

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    (void)dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE);

    if (0 == g_strcmp0(class_name, CLASS_PERIPHERAL_CARD_NAME)) {
        (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_CARD_TYPE, &component_type);
    } else {
        (void)dal_get_property_value_byte(i_paras->obj_handle, g_chassis_board_provider[i_paras->index].pme_prop_name,
            &component_type);
    }

    

    for (guint8 i = 0; i < size; i++) {
        if (component_type == g_component_map[i].int_val) {
            str_type = g_component_map[i].str_val;
        }
    }

    *o_result_jso = (NULL == str_type) ? NULL : json_object_new_string(str_type);
    do_if_expr(NULL == *o_result_jso, return HTTP_INTERNAL_SERVER_ERROR);

    

    return HTTP_OK;
}


gint32 get_chassis_disk_backplane(OBJ_HANDLE com_handle, gchar *location_suffix, gint32 location_suffix_len)
{
    errno_t safe_fun_ret = EOK;

    gchar connector_type[MAX_STRBUF_LEN] = {0};
    guint8 component_type = 0;
    guint32 position = 0;
    OBJ_HANDLE anchor_handle = 0;
    OBJ_HANDLE connector_handle = 0;

    return_val_if_expr(NULL == location_suffix, VOS_ERR);
    (void)dal_get_property_value_byte(com_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
    return_val_if_expr(component_type != COMPONENT_TYPE_HDD_BACKPLANE, VOS_ERR);

    (void)dal_get_specific_object_position(com_handle, CLASS_ANCHOR, &anchor_handle);
    (void)dal_get_property_value_uint32(anchor_handle, PROPERTY_ANCHOR_POSITION, &position);
    (void)dal_get_specific_object_uint32(CLASS_CONNECTOR_NAME, PROPERTY_CONNECTOR_POSITION, position,
        &connector_handle);

    (void)dal_get_property_value_string(connector_handle, PROPERTY_CONNECTOR_TYPE, connector_type, MAX_STRBUF_LEN);

    if (0 == g_strcmp0(connector_type, REAR_BACKPLANE_CONNECT)) {
        safe_fun_ret = strncat_s(location_suffix, location_suffix_len, REAR, strlen(REAR));
        return_val_do_info_if_expr(safe_fun_ret != EOK, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    } else if (0 == g_strcmp0(connector_type, INNER_BACKPLANE_CONNECT)) {
        safe_fun_ret = strncat_s(location_suffix, location_suffix_len, INNER, strlen(INNER));
        return_val_do_info_if_expr(safe_fun_ret != EOK, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    } else if (0 == g_strcmp0(connector_type, FRONT_BACKPLANE_CONNECT)) {
        safe_fun_ret = strncat_s(location_suffix, location_suffix_len, FRONT, strlen(FRONT));
        return_val_do_info_if_expr(safe_fun_ret != EOK, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }

    return VOS_OK;
}


LOCAL gint32 get_chassis_board_location(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar device_location[MAX_STRBUF_LEN] = {0};
    gchar class_name[MAX_NAME_SIZE] = {0};

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    (void)dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE);

    if (g_strcmp0(class_name, CLASS_PERIPHERAL_CARD_NAME) == 0) {
        (void)strncpy_s(device_location, sizeof(device_location), RF_LOCATION_PERIPHERY, strlen(RF_LOCATION_PERIPHERY));
    } else {
        (void)dal_get_property_value_string(i_paras->obj_handle, g_chassis_board_provider[i_paras->index].pme_prop_name,
            device_location, sizeof(device_location));
        (void)dal_clear_string_blank(device_location, sizeof(device_location));
        
        return_val_do_info_if_expr(strlen(device_location) == 0, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_DEBUG, "%s, %d: get_chassis_board_location fail.", __FUNCTION__, __LINE__));
        
    }

    
    
    (void)get_chassis_disk_backplane(i_paras->obj_handle, device_location, MAX_STRBUF_LEN);
    

    *o_result_jso = json_object_new_string((const gchar *)device_location);
    do_if_expr(NULL == *o_result_jso, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


gint32 get_chassis_board_manufacturer(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar manufacturer[MAX_STRBUF_LEN] = {0};

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    ret = dal_get_property_value_string(i_paras->obj_handle, PROPERTY_COMPONENT_MANUFACTURER,
        manufacturer, sizeof(manufacturer));
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: manufacturer is fail.", __FUNCTION__, __LINE__));

    return_val_if_expr(redfish_board_string_check(manufacturer, sizeof(manufacturer)), HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string((const gchar *)manufacturer);
    do_if_expr(NULL == *o_result_jso, return HTTP_INTERNAL_SERVER_ERROR);
    return HTTP_OK;
}


LOCAL gint32 get_chassis_board_productname(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE o_obj_handle = 0;
    gchar classname[MAX_STRBUF_LEN] = {0};
    gchar productname[MAX_STRBUF_LEN] = {0};
    
    guint8 pcietype = 0;
    gchar perial_classname[MAX_STRBUF_LEN] = {0};

    
    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    (void)dfl_get_class_name(i_paras->obj_handle, perial_classname, MAX_STRBUF_LEN);
    return_val_do_info_if_expr(g_strcmp0(perial_classname, CLASS_PERIPHERAL_CARD_NAME) == 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: perial card do not support.", __FUNCTION__, __LINE__));
    

    gboolean result = redfish_get_board_class_name(i_paras->obj_handle, classname, sizeof(classname));
    return_val_do_info_if_expr(!result, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get class name fail.", __FUNCTION__, __LINE__));

    if ((g_strcmp0(classname, CLASS_RAIDCARD_NAME) == 0) || (g_strcmp0(classname, CLASS_MEZZ) == 0) ||
        (g_strcmp0(classname, CLASS_PCIECARD_NAME) == 0) || (g_strcmp0(classname, CLASS_PASSTHROUGH_CARD) == 0) ||
        (g_strcmp0(classname, CLASS_OCP_CARD) == 0) || (g_strcmp0(classname, CLASS_SOC_BOARD) == 0)) {
        result = redfish_get_refproperty_object(i_paras->obj_handle, classname, &o_obj_handle);
        return_val_do_info_if_fail(result, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get ref obj_handle fail.", __FUNCTION__, __LINE__));
    } else if (g_strcmp0(classname, CLASS_NETCARD_NAME) == 0) {
        result = dal_get_specific_object_string((const gchar *)classname, PROPERTY_NETCARD_REF_COMPONENT,
            dfl_get_object_name(i_paras->obj_handle), &o_obj_handle);
        return_val_do_info_if_fail(result == DFL_OK, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s: get %s object failed.", __FUNCTION__, CLASS_NETCARD_NAME));
    } else {
        
        
        ret = redfish_board_elabel_info(i_paras->obj_handle, g_chassis_board_provider[i_paras->index].pme_prop_name,
            productname, sizeof(productname));
        return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s: redfish_board_elabel_info ProductName fail.", __FUNCTION__));
        return_val_if_expr(redfish_board_string_check(productname, sizeof(productname)), HTTP_INTERNAL_SERVER_ERROR);
        *o_result_jso = json_object_new_string((const gchar *)productname);
        return HTTP_OK;
        
    }

    
    
    if ((g_strcmp0(classname, CLASS_PCIECARD_NAME) == 0) || (g_strcmp0(classname, CLASS_OCP_CARD) == 0)) {
        
        (void)dal_get_property_value_byte(o_obj_handle, PROPERTY_PCIECARD_FUNCTIONCLASS, &pcietype);
        return_val_do_info_if_fail(PCIECARD_FUNCTION_NETCARD == pcietype || PCIECARD_FUNCTION_RAID == pcietype,
            HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: pcie card type is invalid.", __FUNCTION__, __LINE__));

        ret = dal_get_property_value_string(o_obj_handle, RFPROP_BOARD_NAME, productname, sizeof(productname));
    } else if ((g_strcmp0(classname, CLASS_PASSTHROUGH_CARD) == 0) || (g_strcmp0(classname, CLASS_SOC_BOARD) == 0)) {
        ret = dal_get_property_value_string(o_obj_handle, RFPROP_BOARD_NAME, productname, sizeof(productname));
    } else {
        ret = dal_get_property_value_string(o_obj_handle, PROPERTY_MEZZCARD_PRODUCTNAME, productname,
            sizeof(productname));
    }

    if (ret != RET_OK || redfish_board_string_check(productname, sizeof(productname))) {
        debug_log(DLOG_MASS, "%s: get %s productname fail.", __FUNCTION__, classname);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    

    *o_result_jso = json_object_new_string((const gchar *)productname);
    do_if_expr(NULL == *o_result_jso, return HTTP_INTERNAL_SERVER_ERROR);
    return HTTP_OK;
}

gint32 get_chassis_board_serialnumber(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar classname[MAX_STRBUF_LEN] = {0};
    gchar serialnumber[MAX_STRBUF_LEN] = {0};
    gchar perial_classname[MAX_STRBUF_LEN] = {0};

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    (void)dfl_get_class_name(i_paras->obj_handle, perial_classname, MAX_STRBUF_LEN);
    return_val_do_info_if_expr(0 == g_strcmp0(perial_classname, CLASS_PERIPHERAL_CARD_NAME), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: perial card do not support.", __FUNCTION__, __LINE__));
    

    gboolean result = redfish_get_board_class_name(i_paras->obj_handle, classname, sizeof(classname));
    return_val_do_info_if_expr(!result, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get class name fail.", __FUNCTION__, __LINE__));

    if (g_strcmp0(classname, CLASS_MAINBOARD_NAME) == 0 || g_strcmp0(classname, CLASS_CARD_NAME) == 0 ||
        g_strcmp0(classname, CLASS_PCIECARD_NAME) == 0 || g_strcmp0(classname, CLASS_CIC_CARD) == 0 ||
        g_strcmp0(classname, CLASS_AR_CARD_NAME) == 0 || g_strcmp0(classname, CLASS_SOC_BOARD) == 0) {
        ret = redfish_board_elabel_info(i_paras->obj_handle, PROPERTY_ELABEL_BOARD_SN, serialnumber, MAX_STRBUF_LEN);
    }
    // 201705108246 需求支持PARNUM查询, 20190723 RAOJIANZHONG 00267466 start
    else if (INVALID_DATA_BYTE != dal_class_to_devicetype(classname)
        // SASExpBoard对象序列号直接从电子标签获取
        && g_strcmp0(classname, CLASS_SAS_EXPBOARD_NAME) != 0) {
        ret = dal_get_card_sn(i_paras->obj_handle, serialnumber, MAX_STRBUF_LEN);
    }
    // 201705108246 需求支持PARNUM查询, 20190723 RAOJIANZHONG 00267466 end
    else {
        ret = redfish_board_elabel_info(i_paras->obj_handle, g_chassis_board_provider[i_paras->index].pme_prop_name,
            serialnumber, MAX_STRBUF_LEN);
    }

    if (ret != RET_OK || redfish_board_string_check(serialnumber, sizeof(serialnumber))) {
        debug_log(DLOG_MASS, "%s: get %s elabel serialnumber information fail.", __FUNCTION__, classname);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)serialnumber);
    do_if_expr(NULL == *o_result_jso, return HTTP_INTERNAL_SERVER_ERROR);
    return HTTP_OK;
}


gint32 get_chassis_board_partnumber(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gboolean result;
    gchar classname[MAX_STRBUF_LEN] = {0};
    gchar partnumber[MAX_STRBUF_LEN] = {0};
    gchar perial_classname[MAX_STRBUF_LEN] = {0};

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    (void)dfl_get_class_name(i_paras->obj_handle, perial_classname, MAX_STRBUF_LEN);
    return_val_do_info_if_expr(0 == g_strcmp0(perial_classname, CLASS_PERIPHERAL_CARD_NAME), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: perial card do not support.", __FUNCTION__, __LINE__));
    

    result = redfish_get_board_class_name(i_paras->obj_handle, classname, sizeof(classname));
    return_val_do_info_if_expr(!result, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get class name fail.", __FUNCTION__, __LINE__));

    if (g_strcmp0(classname, CLASS_MAINBOARD_NAME) == 0 || g_strcmp0(classname, CLASS_CIC_CARD) == 0 ||
        g_strcmp0(classname, CLASS_AR_CARD_NAME) == 0) {
        ret = redfish_board_elabel_info(i_paras->obj_handle, PROPERTY_ELABEL_BOARD_PN, partnumber, MAX_STRBUF_LEN);
    }
    // 201705108246 需求支持PARNUM查询, 20190723 RAOJIANZHONG 00267466 start
    else if (INVALID_DATA_BYTE != dal_class_to_devicetype(classname)
        // SASExpBoard对象部件编号直接从电子标签获取
        && g_strcmp0(classname, CLASS_SAS_EXPBOARD_NAME) != 0) {
        ret = dal_get_card_bom(i_paras->obj_handle, dal_class_to_devicetype(classname), partnumber, MAX_STRBUF_LEN);
    }
    // 201705108246 需求支持PARNUM查询, 20190723 RAOJIANZHONG 00267466 end
    else {
        ret = redfish_board_elabel_info(i_paras->obj_handle, g_chassis_board_provider[i_paras->index].pme_prop_name,
            partnumber, MAX_STRBUF_LEN);
    }

    if (ret != RET_OK || redfish_board_string_check(partnumber, sizeof(partnumber))) {
        debug_log(DLOG_MASS, "%s: get %s partnumber information fail.", __FUNCTION__, classname);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)partnumber);
    do_if_expr(NULL == *o_result_jso, return HTTP_INTERNAL_SERVER_ERROR);
    return HTTP_OK;
}


LOCAL gint32 get_chassis_board_assettag(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar assettag[MAX_STRBUF_LEN] = {0};
    gchar perial_classname[MAX_STRBUF_LEN] = {0};

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    (void)dfl_get_class_name(i_paras->obj_handle, perial_classname, MAX_STRBUF_LEN);
    return_val_do_info_if_expr(0 == g_strcmp0(perial_classname, CLASS_PERIPHERAL_CARD_NAME), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: perial card do not support.", __FUNCTION__, __LINE__));
    

    ret = redfish_board_elabel_info(i_paras->obj_handle, g_chassis_board_provider[i_paras->index].pme_prop_name,
        assettag, MAX_STRBUF_LEN);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get elabel assettag information fail.", __FUNCTION__, __LINE__));

    return_val_if_expr(redfish_board_string_check(assettag, sizeof(assettag)), HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string((const gchar *)assettag);
    do_if_expr(NULL == *o_result_jso, return HTTP_INTERNAL_SERVER_ERROR);
    return HTTP_OK;
}


gint32 get_chassis_board_cpldversion(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gboolean result;
    OBJ_HANDLE bmc_obj_handle = 0;
    gchar classname[MAX_STRBUF_LEN] = {0};
    gchar cpldversion[MAX_STRBUF_LEN] = {0};
    gchar prop_str[MAX_STRBUF_LEN] = {0};
    guint32 cpld_unit_num = 0;
    OBJ_HANDLE o_obj_handle = 0;
    OBJ_HANDLE refer_handle = 0;
    gchar logic_ver[FIRMWARE_VERSION_STRING_LEN] = {0};
    gchar perial_classname[MAX_STRBUF_LEN] = {0};
    guint8 pcietype = 0;

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    (void)dfl_get_class_name(i_paras->obj_handle, perial_classname, MAX_STRBUF_LEN);
    return_val_do_info_if_expr(0 == g_strcmp0(perial_classname, CLASS_PERIPHERAL_CARD_NAME), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: perial card do not support.", __FUNCTION__, __LINE__));
    

    result = redfish_get_board_class_name(i_paras->obj_handle, classname, sizeof(classname));
    return_val_do_info_if_expr(!result, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get class name fail.", __FUNCTION__, __LINE__));

    do_if_true(0 == g_strcmp0(classname, CLASS_PCIECARD_NAME),
        result = redfish_get_refproperty_object(i_paras->obj_handle, classname, &o_obj_handle);
        return_val_do_info_if_fail(result, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get ref obj_handle fail.", __FUNCTION__, __LINE__));
        (void)dal_get_property_value_byte(o_obj_handle, PROPERTY_PCIECARD_FUNCTIONCLASS, &pcietype));

    
    
    if (0 == g_strcmp0(classname, RFPROP_CLASS_LEDBOARD)) {
        
        return HTTP_INTERNAL_SERVER_ERROR;
    } else if (0 == g_strcmp0(classname, CLASS_MAINBOARD_NAME)) {
        
        ret = dfl_get_object_handle(BMC_CLASEE_NAME, &bmc_obj_handle);
        return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d:bmc get bmc obj_handle fail.\n", __FUNCTION__, __LINE__));

        ret = dal_get_property_value_string(bmc_obj_handle, BMC_CPLD_VER_NAME, cpldversion, sizeof(cpldversion));
        return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get property value fail.\n", __FUNCTION__, __LINE__));

        ret = dal_get_property_value_uint32(bmc_obj_handle, PROPERTY_CPLD_UNIT_NUM, &cpld_unit_num);
        return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get property value fail.\n", __FUNCTION__, __LINE__));

        ret = snprintf_s(prop_str, sizeof(prop_str), sizeof(prop_str) - 1, "%s(U%u)", cpldversion, cpld_unit_num);
        do_if_expr(0 >= ret, debug_log(DLOG_MASS, "%s %d:snprintf_s fail.", __FUNCTION__, __LINE__));
        *o_result_jso = json_object_new_string((const gchar *)prop_str);
        do_if_expr(NULL == *o_result_jso, return HTTP_INTERNAL_SERVER_ERROR);
        return HTTP_OK;
    } 
    else if (0 == g_strcmp0(classname, CLASS_HDDBACKPLANE_NAME) || (0 == g_strcmp0(classname, CLASS_IOBOARD_NAME)) ||
        (g_strcmp0(classname, CLASS_FANBOARD_NAME) == 0)) {
        
        result = redfish_get_one_refproperty_object_handle(i_paras->obj_handle, classname, &o_obj_handle);
        
        return_val_do_info_if_fail(result, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get ref obj_handle fail.", __FUNCTION__, __LINE__));
    }
    
     
    else if ((0 == g_strcmp0(classname, CLASS_NETCARD_NAME)) || (0 == g_strcmp0(classname, CLASS_MEZZ)) ||
        (0 == g_strcmp0(classname, CLASS_CARD_NAME)) || (0 == g_strcmp0(classname, RFPROP_TYPE_TWIN_NODE_BACKPLANE))) {
        
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    else if (is_raid_card_type(classname, pcietype)) {
        result = redfish_get_refproperty_object(i_paras->obj_handle, classname, &o_obj_handle);
        return_val_do_info_if_fail(result, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get ref obj_handle fail.", __FUNCTION__, __LINE__));
        ret = dfl_get_referenced_object(o_obj_handle, PROPERTY_PCIE_CARD_FIRMWARE_OBJ, &refer_handle);
        
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get ref obj_handle fail.", __FUNCTION__, __LINE__));
        ret = dal_get_property_value_string(refer_handle, PROPERTY_SOFTWARE_VERSIONSTRING, logic_ver,
            FIRMWARE_VERSION_STRING_LEN);
        return_val_do_info_if_fail((VOS_OK == ret) && (0 != strlen(logic_ver)), HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get ref obj_handle fail.", __FUNCTION__, __LINE__));
        *o_result_jso = json_object_new_string((const gchar *)logic_ver);
        return_val_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR);
        return HTTP_OK;
        
    } else {
        result = redfish_get_refproperty_object(i_paras->obj_handle, classname, &o_obj_handle);
        return_val_do_info_if_fail(result, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get ref obj_handle fail.", __FUNCTION__, __LINE__));
    }

    ret = dal_get_property_value_string(o_obj_handle, RFPROP_BOARD_LOGICVER, cpldversion, sizeof(cpldversion));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get property value fail.", __FUNCTION__, __LINE__));

    return_val_if_expr(redfish_board_string_check(cpldversion, sizeof(cpldversion)), HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string((const gchar *)cpldversion);
    return_val_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gboolean is_raid_card_type(gchar *classname, guint8 pcietype)
{
    return_val_do_info_if_expr(classname == NULL, FALSE,
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return (0 == g_strcmp0(classname, CLASS_RAIDCARD_NAME) ||
        ((0 == g_strcmp0(classname, CLASS_PCIECARD_NAME)) && (PCIECARD_FUNCTION_RAID == pcietype)));
}

LOCAL gint32 get_chassis_board_boardid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint16 boardid = 0;
    gchar str_boardid[MAX_STRBUF_LEN] = {0};

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    (void)dal_get_property_value_uint16(i_paras->obj_handle, g_chassis_board_provider[i_paras->index].pme_prop_name,
        &boardid);
    return_val_do_info_if_expr((0 == boardid) || (0xffff == boardid), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: The BoardId is invalid.", __FUNCTION__, __LINE__));

    ret = snprintf_s(str_boardid, sizeof(str_boardid), sizeof(str_boardid) - 1, "0x%04x", boardid);
    do_val_if_expr(0 >= ret, debug_log(DLOG_MASS, "%s %d : snprintf_s fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string((const gchar *)str_boardid);
    do_if_expr(NULL == *o_result_jso, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

gint32 get_chassis_board_componentuid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    gchar component_uid[STRING_LEN_MAX] = {0};
    (void)dal_get_property_value_string(i_paras->obj_handle, PROPERTY_COMPONENT_UNIQUE_ID,
        component_uid, sizeof(component_uid));

    if (dal_check_component_uid_valid((const gchar*)component_uid) != TRUE) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)component_uid);

    return HTTP_OK;
}


LOCAL gboolean match_class_name_in_table(gchar *classname, gsize lens)
{
    gchar *class_name_table[] = {
        CLASS_NETCARD_NAME,
        RFPROP_CLASS_LEDBOARD,
        CLASS_FANBOARD_NAME,
        CLASS_MAINBOARD_NAME,
        RFPROP_TYPE_TWIN_NODE_BACKPLANE,
        CLASS_HORIZ_CONN_BRD,
        CLASS_SAS_EXPBOARD_NAME,
        CLASS_FAN_MODULE_NAME,
        CLASS_PCIECARD_NAME,
        CLASS_ASSET_LOCATE_BOARD,
        RFPROP_TYPE_CHASSIS_BACKPLANE,
        CLASS_SOC_BOARD
    };

    for (guint8 i = 0; i < (sizeof(class_name_table) / sizeof(class_name_table[0])); i++) {
        if (!g_strcmp0(classname, class_name_table[i])) {
            return TRUE;
        }
    }
    return FALSE;
}


LOCAL gboolean match_boardname_with_classname(gchar *classname, gsize lens)
{
    gchar *class_name_table[] = {
        CLASS_FANBOARD_NAME,
        CLASS_IOBOARD_NAME,
        CLASS_RISERPCIECARD_NAME,
        CLASS_PCIETRANSFORMCARD,
        CLASS_CARD_NAME,
        CLASS_GPUBOARD_NAME,
        CLASS_LEAK_DET_CARD,
        CLASS_AR_CARD_NAME
    };
    size_t count = sizeof(class_name_table) / sizeof(class_name_table[0]);

    for (size_t i = 0; i < count; i++) {
        if (!g_strcmp0(classname, class_name_table[i])) {
            return TRUE;
        }
    }
    return FALSE;
}

gint32 get_chassis_board_pcbversion(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar classname[MAX_STRBUF_LEN] = {0};
    gchar pcbversion[MAX_STRBUF_LEN] = {0};
    OBJ_HANDLE o_obj_handle = 0;

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    gboolean result = redfish_get_board_class_name(i_paras->obj_handle, classname, sizeof(classname));
    return_val_do_info_if_expr(!result, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get class name fail.", __FUNCTION__, __LINE__));

    
    if (match_class_name_in_table(classname, sizeof(classname))) {
        guchar fru_id = 0;
        (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_COMPONENT_FRUID, &fru_id);
        ret = dal_get_specific_object_byte(CLASS_FRU, PROPERTY_FRU_ID, fru_id, &o_obj_handle);
        do_if_fail(ret == VOS_OK, debug_log(DLOG_MASS, "%s, %d: get specific handle fail.", __FUNCTION__, __LINE__));
        
        ret = dal_get_property_value_string(o_obj_handle, PROPERTY_FRU_PCB_VERSION, pcbversion, sizeof(pcbversion));
        return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get property value fail.", __FUNCTION__, __LINE__));
        return_val_if_expr(redfish_board_string_check(pcbversion, sizeof(pcbversion)), HTTP_INTERNAL_SERVER_ERROR);
        *o_result_jso = json_object_new_string((const gchar *)pcbversion);
        do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);
        return HTTP_OK;
        
    } else if ((g_strcmp0(classname, CLASS_HDDBACKPLANE_NAME) == 0) ||
        (g_strcmp0(classname, CLASS_M2TRANSFORMCARD)) == 0) {
        
        result = redfish_get_one_refproperty_object_handle(i_paras->obj_handle, classname, &o_obj_handle);
        
        return_val_do_info_if_fail(result, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get ref obj_handle fail.", __FUNCTION__, __LINE__));
    
    } else if ((!g_strcmp0(classname, CLASS_IOBOARD_NAME)) || (!g_strcmp0(classname, CLASS_PCIETRANSFORMCARD)) ||
        (!g_strcmp0(classname, CLASS_CARD_NAME)) || (!g_strcmp0(classname, CLASS_LEAK_DET_CARD))) {
        ret = dal_get_specific_object_position(i_paras->obj_handle, (const gchar *)classname, &o_obj_handle);
        
        
        do_if_expr((ret != VOS_OK && g_strcmp0(classname, CLASS_IOBOARD_NAME) == 0),
            (ret = dal_get_specific_object_position(i_paras->obj_handle, CLASS_PERIPHERAL_CARD_NAME, &o_obj_handle)));
        
        return_val_do_info_if_fail(ret == DFL_OK, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s: dal_get_specific_object_position fail.class = %s", __FUNCTION__, classname));
        
    } else if (g_strcmp0(classname, CLASS_PERIPHERAL_CARD_NAME) == 0) {
        ret = get_specific_peripheral_card_obj(i_paras->obj_handle, &o_obj_handle);
        if (ret != VOS_OK) {
            o_obj_handle = i_paras->obj_handle;
        }
    } else {
        result = redfish_get_refproperty_object(i_paras->obj_handle, classname, &o_obj_handle);
        
        
        
        do_if_expr(result == FALSE,
            (void)dal_get_specific_object_position(i_paras->obj_handle, (const gchar *)classname, &o_obj_handle));
        
        return_val_do_info_if_fail(o_obj_handle != 0, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get obj_handle fail.", __FUNCTION__, __LINE__));
    }

    
    
    ret = dal_get_property_value_string(o_obj_handle, RFPROP_BOARD_PCBVER, pcbversion, sizeof(pcbversion));
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get property value fail.", __FUNCTION__, __LINE__));
    return_val_if_expr(redfish_board_string_check(pcbversion, sizeof(pcbversion)), HTTP_INTERNAL_SERVER_ERROR);
    
    *o_result_jso = json_object_new_string((const gchar *)pcbversion);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_specific_peripheral_card_obj(OBJ_HANDLE component_handle, OBJ_HANDLE *peripheral_card_handle)
{
    GSList *peripheral_card_list = NULL;
    gint32 ret;
    gint32 ret_val = VOS_ERR;
    GSList *node = NULL;
    OBJ_HANDLE card_handle;
    guint8 card_type = 0;
    gchar device_name[MAX_STRBUF_LEN] = {0};
    gchar board_name[MAX_STRBUF_LEN] = {0};
    guint8 component_type = 0;

    return_val_do_info_if_expr(NULL == peripheral_card_handle, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__));

    ret = dal_get_object_list_position(component_handle, CLASS_PERIPHERAL_CARD_NAME, &peripheral_card_list);
    return_val_do_info_if_expr(VOS_OK != ret || NULL == peripheral_card_list, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: call dal_get_object_list_position failed, ret is %d", __FUNCTION__, ret));

    ret = dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
    goto_label_do_info_if_expr(VOS_OK != ret, exit,
        debug_log(DLOG_DEBUG, "%s: get obj %s device type failed, ret is %d", __FUNCTION__,
        dfl_get_object_name(component_handle), ret));

    ret = dal_get_property_value_string(component_handle, PROPERTY_COMPONENT_DEVICE_NAME, device_name,
        sizeof(device_name));
    goto_label_do_info_if_expr(VOS_OK != ret, exit,
        debug_log(DLOG_DEBUG, "%s: get obj %s device name failed, ret is %d", __FUNCTION__,
        dfl_get_object_name(component_handle), ret));

    for (node = peripheral_card_list; node != NULL; node = node->next) {
        card_handle = (OBJ_HANDLE)node->data;
        (void)dal_get_property_value_byte(card_handle, PROPERTY_CARD_TYPE, &card_type);
        continue_if_expr(component_type != card_type);

        (void)dal_get_property_value_string(card_handle, PROPERTY_CARD_BOARDNAME, board_name, sizeof(board_name));

        if (!strcmp(board_name, device_name)) {
            *peripheral_card_handle = card_handle;
            ret_val = VOS_OK;
            break;
        }
    }

exit:
    g_slist_free(peripheral_card_list);
    return ret_val;
}


gint32 get_chassis_board_boardname(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE o_obj_handle = 0;
    gchar classname[MAX_STRBUF_LEN] = {0};
    gchar boardname[MAX_STRBUF_LEN] = {0};
    const gchar *pme_prop_name = RFPROP_BOARD_NAME;

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    gboolean result = redfish_get_board_class_name(i_paras->obj_handle, classname, sizeof(classname));
    return_val_do_info_if_expr(!result, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get class name fail.", __FUNCTION__, __LINE__));

    
    
        
        
        
    if ((g_strcmp0(classname, CLASS_RAIDCARD_NAME) == 0) || (g_strcmp0(classname, CLASS_NETCARD_NAME) == 0) ||
        (g_strcmp0(classname, CLASS_PASSTHROUGH_CARD) == 0) || (g_strcmp0(classname, CLASS_SOC_BOARD) == 0)) {
        
        
        result = redfish_get_refproperty_object(i_paras->obj_handle, classname, &o_obj_handle);
        return_val_do_info_if_fail(result, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get ref obj_handle fail.", __FUNCTION__, __LINE__));
        pme_prop_name = PROPERTY_CARD_BOARDNAME;
        
    } else if (g_strcmp0(classname, CLASS_M2TRANSFORMCARD) ==
        0) { 
        
        result = redfish_get_one_refproperty_object_handle(i_paras->obj_handle, classname, &o_obj_handle);
        
        return_val_do_info_if_fail(result, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get ref obj_handle fail.", __FUNCTION__, __LINE__));
        pme_prop_name = PROPERTY_CARD_BOARDNAME;
        
    } else if (g_strcmp0(classname, CLASS_PCIECARD_NAME) == 0) { 
        
        result = redfish_get_refproperty_object(i_paras->obj_handle, classname, &o_obj_handle);
        return_val_do_info_if_fail(result, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get ref obj_handle fail.", __FUNCTION__, __LINE__));

        result = get_pcie_card_boardname(o_obj_handle, boardname, MAX_STRBUF_LEN);
        return_val_do_info_if_fail(result == HTTP_OK, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get pciecard boardname fail.", __FUNCTION__, __LINE__));
        goto NAME_OBTAINED;
        
        
    } else if (g_strcmp0(classname, CLASS_HDDBACKPLANE_NAME) == 0
        
        || g_strcmp0(classname, CLASS_HORIZ_CONN_BRD) == 0) {
        
        
        result = redfish_get_one_refproperty_object_handle(i_paras->obj_handle, classname, &o_obj_handle);
        
        return_val_do_info_if_fail(result, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get ref obj_handle fail.", __FUNCTION__, __LINE__));
    } else if ((0 == g_strcmp0(classname, CLASS_MEZZ)) || (0 == g_strcmp0(classname, CLASS_EXPBOARD_NAME)) ||
        (0 == g_strcmp0(classname, CLASS_CPUBOARD_NAME)) || (0 == g_strcmp0(classname, CLASS_MEMBOARD_NAME))) {
        result = redfish_get_refproperty_object(i_paras->obj_handle, classname, &o_obj_handle);
        return_val_do_info_if_fail(result, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get ref obj_handle fail.", __FUNCTION__, __LINE__));
        
        
        
    } else if (match_boardname_with_classname(classname, sizeof(classname))) {
        
        ret = dal_get_specific_object_position(i_paras->obj_handle, (const gchar *)classname, &o_obj_handle);
        
        
        do_if_expr((ret != VOS_OK && g_strcmp0(classname, CLASS_IOBOARD_NAME) == 0),
            (ret = dal_get_specific_object_position(i_paras->obj_handle, CLASS_PERIPHERAL_CARD_NAME, &o_obj_handle)));
        

        
        
        do_if_expr((ret != VOS_OK && g_strcmp0(classname, CLASS_FANBOARD_NAME) == 0),
            (ret = get_specific_peripheral_card_obj(i_paras->obj_handle, &o_obj_handle)));
        

        return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s: get class %s, obj name: %s failed", __FUNCTION__, classname,
            dfl_get_object_name(i_paras->obj_handle)));

        
        do_if_expr((g_strcmp0(classname, CLASS_GPUBOARD_NAME) == 0), (pme_prop_name = PROPERTY_GPUBOARD_BOARDNAME));
        do_if_expr((g_strcmp0(classname, CLASS_CARD_NAME) == 0), (pme_prop_name = PROPERTY_CARD_BOARDNAME));
        
        
    } else if (g_strcmp0(classname, CLASS_PERIPHERAL_CARD_NAME) == 0) {
        ret = get_specific_peripheral_card_obj(i_paras->obj_handle, &o_obj_handle);
        if (ret != VOS_OK) {
            o_obj_handle = i_paras->obj_handle;
            pme_prop_name = PROPERTY_CARD_BOARDNAME;
        }
    } else {
        
        ret = redfish_board_elabel_info(i_paras->obj_handle, PROPERTY_ELABEL_BOARD_PRO_NAME,
            boardname, MAX_STRBUF_LEN);
        goto NAME_OBTAINED;
    }

    ret = dal_get_property_value_string(o_obj_handle, pme_prop_name, boardname, sizeof(boardname));
NAME_OBTAINED:
    if (ret != RET_OK || redfish_board_string_check(boardname, sizeof(boardname))) {
        debug_log(DLOG_MASS, "%s: get %s boardname information fail.", __FUNCTION__, classname);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)boardname);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);
    return HTTP_OK;
}


LOCAL gint32 get_pcie_card_boardname(OBJ_HANDLE o_obj_handle, gchar *boardname, guint32 boardname_len)
{
    gint32 ret;
    OBJ_HANDLE m2card_handle = 0;
    OBJ_HANDLE netcard_handle = 0;
    gboolean result = FALSE;
    guint8 pcietype = 0;
    gchar slavecard[MAX_STRBUF_LEN] = {0};

    ret = dal_get_property_value_byte(o_obj_handle, PROPERTY_PCIECARD_FUNCTIONCLASS, &pcietype);
    return_val_do_info_if_expr(ret != DFL_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get pciecard fail.", __FUNCTION__, __LINE__));

    if (PCIECARD_FUNCTION_NETCARD == pcietype) {
        ret = dal_get_specific_object_position(o_obj_handle, CLASS_NETCARD_NAME, &netcard_handle);
        return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get netcard_handle fail.", __FUNCTION__, __LINE__));
        (void)dal_get_property_value_string(netcard_handle, PROPERTY_CARD_BOARDNAME, boardname, boardname_len);
    } else if (PCIECARD_FUNCTION_RAID == pcietype) {
        (void)dal_get_property_value_string(o_obj_handle, PROPERTY_CARD_BOARDNAME, boardname, boardname_len);
    } else {
        
        ret = dal_get_property_value_string(o_obj_handle, PROPERTY_PCIE_CARD_SLAVE_CARD, slavecard, sizeof(slavecard));
        return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get strig value fail.", __FUNCTION__, __LINE__));

        result = redfish_get_class_handle_by_pciehandle(o_obj_handle, slavecard, &m2card_handle);
        return_val_do_info_if_fail(result, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get net card obj_handle fail.", __FUNCTION__, __LINE__));
        (void)dal_get_property_value_string(m2card_handle, PROPERTY_CARD_BOARDNAME, boardname, boardname_len);
    }

    return HTTP_OK;
}


LOCAL gint32 get_chassis_board_manufacturedate(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar manufacturedata[MAX_STRBUF_LEN] = {0};
    gchar class_name[MAX_NAME_SIZE] = {0};

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    (void)dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE);

    if (0 == g_strcmp0(class_name, CLASS_PERIPHERAL_CARD_NAME)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = redfish_board_elabel_info(i_paras->obj_handle, g_chassis_board_provider[i_paras->index].pme_prop_name,
        manufacturedata, MAX_STRBUF_LEN);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get elabel _manufacturedate information fail.", __FUNCTION__, __LINE__));

    return_val_if_expr(redfish_board_string_check(manufacturedata, sizeof(manufacturedata)),
        HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string((const gchar *)manufacturedata);
    do_if_expr(NULL == *o_result_jso, return HTTP_INTERNAL_SERVER_ERROR);
    return HTTP_OK;
}


LOCAL gint32 get_chassis_board_support_raidlevels(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE o_obj_handle = 0;
    gchar model[MAX_STRBUF_LEN] = {0};
    gchar class_name[MAX_NAME_SIZE] = {0};

    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    ret = redfish_get_board_class_name(i_paras->obj_handle, class_name, sizeof(class_name));
    return_val_do_info_if_expr(!ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get class name fail.", __FUNCTION__, __LINE__));

    if (0 == g_strcmp0(class_name, CLASS_RAIDCARD_NAME)) {
        ret = redfish_get_refproperty_object(i_paras->obj_handle, class_name, &o_obj_handle);
        return_val_do_info_if_fail(ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get ref obj_handle fail.", __FUNCTION__, __LINE__));
        ret = dal_get_property_value_string(o_obj_handle, PROPERTY_RAIDCARD_MODEL, model, MAX_STRBUF_LEN);
        return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get model fail.", __FUNCTION__, __LINE__));
        *o_result_jso = json_object_new_string((const gchar *)model);
        do_if_expr(NULL == *o_result_jso, return HTTP_INTERNAL_SERVER_ERROR);
    }

    return HTTP_OK;
}


LOCAL gint32 get_chassis_board_pch_model(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE o_obj_handle = 0;
    gchar pch_model[MAX_STRBUF_LEN] = {0};
    gchar class_name[MAX_NAME_SIZE] = {0};

    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    ret = redfish_get_board_class_name(i_paras->obj_handle, class_name, sizeof(class_name));
    return_val_do_info_if_expr(!ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get class name fail.", __FUNCTION__, __LINE__));

    if (0 == g_strcmp0(class_name, CLASS_MAINBOARD_NAME)) {
        (void)dal_get_object_handle_nth(class_name, 0, &o_obj_handle);
        ret = dal_get_property_value_string(o_obj_handle, PROPERTY_MAINBOARD_PCHMODEL, pch_model, MAX_STRBUF_LEN);
        return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get pch model fail.", __FUNCTION__, __LINE__));

        if (rf_string_check((const gchar*)pch_model) == VOS_OK) {
            *o_result_jso = json_object_new_string((const gchar*)pch_model);
            do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);
        } else {
            *o_result_jso = NULL;
        }
    }

    return HTTP_OK;
}


LOCAL gint32 get_chassis_mezzcard_linkprop(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gboolean bool_ret;
    gchar class_name[MAX_NAME_SIZE + 1] = {0};
    gchar prop_val[MAX_STRBUF_LEN + 1] = {0};
    OBJ_HANDLE card_handle = 0;

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    bool_ret = redfish_get_board_class_name(i_paras->obj_handle, class_name, sizeof(class_name));
    if (bool_ret != TRUE || g_strcmp0(class_name, CLASS_MEZZ) != 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_specific_object_position(i_paras->obj_handle, (const gchar *)class_name, &card_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get object handle fail(%d).", __func__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_string(card_handle, g_chassis_board_provider[i_paras->index].pme_prop_name,
        prop_val, sizeof(prop_val));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get prop %s fail(%d).", __func__, class_name, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = rf_string_check((const gchar *)prop_val);
    if (ret != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)prop_val);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: new json object fail.", __func__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_chassis_ciccard_m2prop(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gboolean bool_ret;
    gchar class_name[MAX_NAME_SIZE + 1] = {0};
    guint8 presence = 0;
    OBJ_HANDLE card_handle = 0;

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    bool_ret = redfish_get_board_class_name(i_paras->obj_handle, class_name, sizeof(class_name));
    if (bool_ret != TRUE || g_strcmp0(class_name, CLASS_CIC_CARD) != 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_specific_object_position(i_paras->obj_handle, (const gchar*)class_name, &card_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get object handle fail(%d).", __func__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_byte(card_handle, g_chassis_board_provider[i_paras->index].pme_prop_name, &presence);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get prop %s fail(%d).", __func__, class_name, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    gboolean m2_presence = (presence == 1) ? TRUE : FALSE;
    *o_result_jso = json_object_new_boolean(m2_presence);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: new json object fail.", __func__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

/*****************************************************************************
 函 数 名  : chassis_board_provider_entry
 功能描述  : 更新扩展板卡资源入口函数
 输入参数  : PROVIDER_PARAS_S* i_paras, PROPERTY_PROVIDER_S** prop_provider, guint32* count
 输出参数  : 无
 返 回 值  : gint32

 修改历史      :
  1.日    期   : 2017年3月3日
    作    者   : zwx382233
    修改内容   : 新生成函数
    需求单号   : AR-0000276582-002-036
**************************************************************************** */
gint32 chassis_board_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean bool_ret;
    OBJ_HANDLE chassis_handle = OBJ_HANDLE_COMMON;
    gint32 ret;

    
    
    bool_ret = rf_check_chassis_uri_valid_allow_node_enc(i_paras->uri, &chassis_handle);
    
    if (!bool_ret || 0 == chassis_handle) {
        return HTTP_NOT_FOUND;
    }

    i_paras->parent_obj_handle_list = g_slist_append(i_paras->parent_obj_handle_list, (gpointer)chassis_handle);

    
    ret = check_enclosure_component_type(chassis_handle, TRUE);
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);
    

    if (redfish_check_board_uri_effective(i_paras->member_id, &(i_paras->obj_handle), chassis_handle)) {
        *prop_provider = g_chassis_board_provider;
        *count = sizeof(g_chassis_board_provider) / sizeof(PROPERTY_PROVIDER_S);
        return VOS_OK;
    }

    return HTTP_NOT_FOUND;
}


LOCAL gint32 get_chassis_board_firmwareversion(PROVIDER_PARAS_S *i_paras, json_object *o_result_jso)
{
    gint32 ret;
    gboolean result;
    gchar classname[MAX_STRBUF_LEN] = {0};
    
    gchar firmwareversion[MAX_MCU_FW_VER_LEN] = {0};
    
    OBJ_HANDLE o_obj_handle = 0;
    json_object *firmwareversion_jso = NULL;

    
    return_val_if_fail(i_paras->user_role_privilege & USERROLE_READONLY, HTTP_FORBIDDEN);

    result = redfish_get_board_class_name(i_paras->obj_handle, classname, sizeof(classname));
    return_val_do_info_if_expr(!result, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s: obj(%s) get class name fail", __FUNCTION__,
        dfl_get_object_name(i_paras->obj_handle)));

    result = redfish_get_one_refproperty_object_handle(i_paras->obj_handle, classname, &o_obj_handle);
    return_val_do_info_if_fail(result, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s: com_obj(%s) get ref obj_handle fail", __FUNCTION__,
        dfl_get_object_name(i_paras->obj_handle)));

    ret = dal_get_property_value_string(o_obj_handle, PROPERTY_ASSET_LOCATE_BRD_FIRM_VER, firmwareversion,
        sizeof(firmwareversion));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s: obj(%s) get property value fail", __FUNCTION__, dfl_get_object_name(o_obj_handle)));

    return_val_if_expr(redfish_board_string_check(firmwareversion, sizeof(firmwareversion)),
        HTTP_INTERNAL_SERVER_ERROR);

    firmwareversion_jso = json_object_new_string((const gchar *)firmwareversion);
    do_if_expr(NULL == firmwareversion_jso, return HTTP_INTERNAL_SERVER_ERROR);

    json_object_object_add(o_result_jso, PROPERTY_ASSET_LOCATE_BRD_FIRM_VER, firmwareversion_jso);

    return HTTP_OK;
}


LOCAL gboolean is_npu_board(OBJ_HANDLE comp_handle)
{
    gchar device_name[MAX_STRBUF_LEN] = {0};
    gchar class_name[MAX_CLASS_NAME] = {0};
    gboolean ret_bool;

    ret_bool = redfish_get_board_class_name(comp_handle, class_name, sizeof(class_name));
    // NPU板目前属于GPUBoard类
    if (ret_bool != TRUE || g_strcmp0((const gchar *)class_name, CLASS_GPUBOARD_NAME) != 0) {
        return FALSE;
    }

    // NPU板的DeviceName以NPU开头
    (void)dal_get_property_value_string(comp_handle, PROPERTY_COMPONENT_DEVICE_NAME, device_name, sizeof(device_name));
    if (g_ascii_strncasecmp((const gchar *)device_name, "NPU", strlen("NPU")) == 0) {
        return TRUE;
    }

    return FALSE;
}


LOCAL gint32 get_chassis_board_work_mode(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guchar work_mode = 0;
    OBJ_HANDLE npu_handle = 0;

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    // 判断是否为NPU板
    if (is_npu_board(i_paras->obj_handle) == FALSE) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 当前所有NPU的工作模式相同,取其中一个对象获取WorkMode即可
    (void)dal_get_object_handle_nth(CLASS_NPU, 0, &npu_handle);
    ret = dal_get_extern_value_byte(npu_handle, PROPERTY_NPU_MODE, &work_mode, DF_AUTO);
    if (ret != VOS_OK || (work_mode == INVALID_DATA_BYTE)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((work_mode == NPU_WORK_MODE_AMP) ? RFPROP_BOARD_WORK_MODE_AMP :
                                                                              RFPROP_BOARD_WORK_MODE_SMP);
    return HTTP_OK;
}


LOCAL gint32 set_single_npu_work_mode(OBJ_HANDLE obj_handle, PROVIDER_PARAS_S *i_paras, guint8 new_mode)
{
    gint32 ret;
    GSList *input_list = NULL;

    input_list = g_slist_append(input_list, g_variant_new_byte(new_mode));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NPU, METHOD_SET_WORK_MODE, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call %s fail, ret:%d", __FUNCTION__, METHOD_SET_WORK_MODE, ret);
    }
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    return ret;
}


LOCAL gint32 set_npu_work_mode(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, guint8 new_mode)
{
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    // 当前设置任意NPU板的工作模式时,会将所有NPU的工作模式设置成相同值
    gint32 ret = dfl_get_object_list(CLASS_NPU, &obj_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get NPU obj list failed, ret=%d", __FUNCTION__, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFPROP_BOARD_WORK_MODE, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        OBJ_HANDLE obj_hd = (OBJ_HANDLE)obj_node->data;
        ret = set_single_npu_work_mode(obj_hd, i_paras, new_mode);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: set work mode for %s failed, ret:%d", __FUNCTION__,
                dfl_get_object_name(obj_hd), ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, RFPROP_BOARD_WORK_MODE, o_message_jso);
            g_slist_free(obj_list);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    g_slist_free(obj_list);
    return HTTP_OK;
}


LOCAL gint32 set_chassis_board_work_mode(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guchar sys_power_state = 1;
    const gchar *work_mode_str = NULL;
    guchar new_mode;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (i_paras->val_jso == NULL) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_BOARD_WORK_MODE, o_message_jso,
            RF_NULL_STR, RFPROP_BOARD_WORK_MODE);
        return HTTP_BAD_REQUEST;
    }

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    // 判断是否为NPU板,不是则返回400错误
    return_val_do_info_if_expr(is_npu_board(i_paras->obj_handle) == FALSE, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_BOARD_WORK_MODE, o_message_jso,
        RFPROP_BOARD_WORK_MODE));

    // 获取设置参数值
    work_mode_str = dal_json_object_get_str(i_paras->val_jso);
    if (work_mode_str != NULL && g_strcmp0(work_mode_str, RFPROP_BOARD_WORK_MODE_AMP) == 0) {
        new_mode = NPU_WORK_MODE_AMP;
    } else if (work_mode_str != NULL && g_strcmp0(work_mode_str, RFPROP_BOARD_WORK_MODE_SMP) == 0) {
        new_mode = NPU_WORK_MODE_SMP;
    } else {
        (void)create_message_info(MSGID_PROPERTY_VALUE_ERR, RFPROP_BOARD_WORK_MODE, o_message_jso,
            RFPROP_BOARD_WORK_MODE);
        return HTTP_BAD_REQUEST;
    }

    // NPU工作模式仅能在下电状态下设置
    (void)rf_get_system_powerstate(&sys_power_state);
    if (sys_power_state == 1) {
        (void)create_message_info(MSGID_FAILED_SET_NPU_WORK_MODE, RFPROP_BOARD_WORK_MODE, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    return set_npu_work_mode(i_paras, o_message_jso, o_result_jso, new_mode);
}


gint32 get_chassis_board_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    gint32 ret;
    gchar classname[MAX_STRBUF_LEN] = {0};

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_rsc_jso) && (NULL != o_err_array_jso), RF_FAILED,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    ret = provider_get_prop_values(i_paras, &o_rsc_jso, o_err_array_jso, g_chassis_board_provider,
        G_N_ELEMENTS(g_chassis_board_provider));
    if (ret != VOS_OK) {
        return ret;
    }

    // 如果当前为非资产管理板 则应当去除firmwareversion资源
    (void)redfish_get_board_class_name(i_paras->obj_handle, classname, sizeof(classname));

    if (g_strcmp0(classname, CLASS_ASSET_LOCATE_BOARD) == 0 || g_strcmp0(classname, CLASS_IOBOARD_NAME) == 0) {
        (void)get_chassis_board_firmwareversion(i_paras, o_rsc_jso);
    }

    return RF_OK;
}

LOCAL gint32 get_chassis_board_retimer_version(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
                                               json_object **o_result_jso)
{
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    gchar classname[MAX_STRBUF_LEN] = {0};
    gboolean ret_bool =  FALSE;

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    ret_bool = redfish_get_board_class_name(i_paras->obj_handle, classname, MAX_STRBUF_LEN);
    if (ret_bool == FALSE || g_strcmp0(classname, CLASS_RISERPCIECARD_NAME) != 0) {
        
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_object_list_position(i_paras->obj_handle, CLASS_RETIMER_NAME, &obj_list);
    if (ret != RET_OK || obj_list == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_array();
    if (*o_result_jso == NULL) {
        g_slist_free(obj_list);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        json_object *version_jso = json_object_new_object();
        if (version_jso == NULL) {
            continue;
        }
        (void)rf_add_property_jso_string((OBJ_HANDLE)obj_node->data, PROPERTY_RETIMER_SOFT_VER, RFPROP_FWINV_VERSION,
                                         version_jso);
        (void)rf_add_property_jso_string((OBJ_HANDLE)obj_node->data, PROPERTY_RETIMER_LOCATION,
                                         RFPROP_FWINV_POSITION_ID, version_jso);

        json_object_array_add(*o_result_jso, version_jso);
    }
    g_slist_free(obj_list);

    return HTTP_OK;
}


LOCAL gint32 get_ar_card_id(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __func__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    if (i_paras->is_satisfy_privi == 0) {
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: Privilege error.", __func__);
        return HTTP_FORBIDDEN;
    }
    
    guint8 component_type = 0;
    OBJ_HANDLE arcard_handle = 0;
    gchar arcard_macaddr[MACADDRESS_LEN + 1] = { 0 };
    gint32 ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Failed to get component type, (ret:%d)", __func__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (component_type != COMPONENT_TYPE_AR_CARD) {
        debug_log(DLOG_INFO, "[%s] Current component_type is %d", __func__, component_type);
        return HTTP_OK;
    }

    ret = dal_get_specific_object_string(CLASS_AR_CARD_NAME, PROPERTY_ARCARD_REFCOMPONENT,
        dfl_get_object_name(i_paras->obj_handle), &arcard_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Get arcard arcard obj failed!(ret:%d)", __func__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = dal_get_property_value_string(arcard_handle, PROPERTY_ARCARD_ID, arcard_macaddr, sizeof(arcard_macaddr));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Get property(%s."PROPERTY_ARCARD_ID") failed!(ret:%d)", __func__,
            dfl_get_object_name(arcard_handle), ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    *o_result_jso = json_object_new_string((const gchar *)arcard_macaddr);
    if (*o_result_jso == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}


LOCAL gint32 get_chassis_board_act(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    gchar slot[MAX_RSC_NAME_LEN + 1] = {0};
    gchar member_id[MAX_STRBUF_LEN] = {0};
    guint8 component_type = 0;

    
    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
    if ((ret != RET_OK) || (component_type != COMPONENT_TYPE_AR_CARD)) {
        debug_log(DLOG_DEBUG, "%s: failed to find AR Card component, current component_type is %d, ret is %d",
                  __FUNCTION__, component_type, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    OBJ_HANDLE chassis_comp_handle = (OBJ_HANDLE)g_slist_nth_data(i_paras->parent_obj_handle_list, 0);
    ret = rf_gen_chassis_component_id(NULL, chassis_comp_handle, slot, MAX_RSC_NAME_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get slot id fail(%d).", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = get_object_obj_location_devicename(i_paras->obj_handle, member_id, sizeof(member_id));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : function  return err of redfish_slot_id.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    json_object* oem_jso = json_object_new_object();
    json_object* huawei_jso = json_object_new_object();
    if (oem_jso == NULL || huawei_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: new json object fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_object_object_add(oem_jso, RFPROP_COMMON_MANUFACTURER, huawei_jso);

    
    get_chassis_board_action_res(*o_result_jso, slot, member_id);

    return HTTP_OK;
}


LOCAL void get_chassis_board_action_res(json_object* obj_result, const gchar* slot, const gchar* member)
{
    gint32 ret;
    json_object* obj_uid_ac = NULL;
    json_object* obj_target = NULL;
    json_object* obj_action_info = NULL;
    gchar string_value[POWER_STR_MAX_LEN * 2] = {0};

    
    if (obj_result == NULL || slot == NULL || member == NULL) {
        return;
    }

    
    int iRet = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, "#%s", RFPROP_BOARD_ARCARD_CTL);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    obj_uid_ac = json_object_new_object();
    if (obj_uid_ac == NULL) {
        return;
    }
    json_object_object_add(obj_result, (const gchar*)string_value, obj_uid_ac);

    
    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1,
                     RF_CHASSIS_OEM_ACTION_ARCARD_CONTROL, slot, member);
    if (ret <= 0) {
        return;
    }

    obj_target = json_object_new_string((const gchar*)string_value);
    if (obj_target == NULL) {
        return;
    }
    json_object_object_add(obj_uid_ac, RFPROP_TARGET, obj_target);

    
    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1,
                     RF_CHASSIS_OEM_ACTION_ARCARD_CONTROL_ACTIONINFO, slot, member);
    if (ret <= 0) {
        return;
    }

    obj_action_info = json_object_new_string((const gchar*)string_value);
    if (obj_action_info == NULL) {
        return;
    }
    json_object_object_add(obj_uid_ac, RFPROP_ACTION_INFO, obj_action_info);

    return;
}


LOCAL guint8 redfish_arcard_control_choose(const gchar* i_control_type)
{
    
    if (i_control_type == NULL || strlen(i_control_type) + 1 > MAX_STRBUF_LEN) {
        return ARCARD_CONTROL_INVALID_ACTION;
    }

    
    if (g_strcmp0(RF_ACTION_ARCARD_POWEROFF, i_control_type) == RET_OK) {
        return ARCARD_CONTROL_POWER_OFF;
    } else if (g_strcmp0(RF_ACTION_ARCARD_POWERON, i_control_type) == RET_OK) {
        return ARCARD_CONTROL_POWER_ON;
    } else if (g_strcmp0(RF_ACTION_ARCARD_RESET, i_control_type) == RET_OK) {
        return ARCARD_CONTROL_RESET;
    } else if (g_strcmp0(RF_ACTION_ARCARD_FACTORY_RESET, i_control_type) == RET_OK) {
        return ARCARD_CONTROL_FACTORY_RESET;
    } else {
        return ARCARD_CONTROL_INVALID_ACTION;
    }
}


LOCAL gint32 parse_control_result(gint32 ret_code, const gchar* action, const gchar* slot,
    json_object** o_message_jso)
{
    switch (ret_code) {
        case COMP_CODE_SUCCESS:
            (void)create_message_info(MSGID_SUCCESS, NULL, o_message_jso);
            return HTTP_OK;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            return HTTP_FORBIDDEN;

        case COMP_CODE_INVALID_FIELD:
            (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, RFPROP_BOARD_ARCARD_CTL,
                RFPROP_BOARD_ARCARD_SLOT);
            return HTTP_BAD_REQUEST;

        // 不支持动作
        case COMP_CODE_UNSUPPORT:
            (void)create_message_info(MSGID_CMD_NOT_SUPPORTED, RFPROP_FRUCONTROL_TYPE, o_message_jso,
                RFPROP_FRUCONTROL_TYPE, slot, action);
            return HTTP_NOT_IMPLEMENTED;

        default:
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 act_chassis_arcardcontrol(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso)
{
    gint32 ret;
    gint32 slot;
    guint8 result;
    GSList *input_list = NULL;
    json_object *obj_json = NULL;
    gchar slot_id[10] = {0};
    const gchar* actions = NULL;

    
    if ((o_message_jso == NULL) || (i_paras == NULL) || (i_paras->val_jso == NULL)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = json_object_object_get_ex(i_paras->val_jso, RFPROP_BOARD_ARCARD_SLOT, &obj_json);
    if (ret == FALSE) {
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso,
                                  RFPROP_BOARD_ARCARD_CTL, RFPROP_BOARD_ARCARD_SLOT);
        return HTTP_BAD_REQUEST;
    }

    
    slot = json_object_get_int(obj_json);
    if (slot < 0 || slot > G_MAXUINT8 || (json_object_get_type(obj_json) != json_type_int)) {
        (void)create_message_info(MSGID_ACT_PARA_TYPE_ERR, NULL, o_message_jso,
            dal_json_object_to_json_string(obj_json), RFPROP_BOARD_ARCARD_SLOT, RFPROP_BOARD_ARCARD_CTL);
        return HTTP_BAD_REQUEST;
    }

    obj_json = NULL;
    ret = json_object_object_get_ex(i_paras->val_jso, RFPROP_BOARD_ARCARD_CONTROL_TYPE, &obj_json);
    if (ret == FALSE) {
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, RFPROP_BOARD_ARCARD_CTL,
                                  RFPROP_BOARD_ARCARD_CONTROL_TYPE);
        return HTTP_BAD_REQUEST;
    }

    actions = dal_json_object_get_str(obj_json);
    result = redfish_arcard_control_choose(actions);
    if (result == 0xff) {
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, RFPROP_BOARD_ARCARD_CONTROL_TYPE, o_message_jso, actions,
                                  RFPROP_BOARD_ARCARD_CONTROL_TYPE);
        return HTTP_BAD_REQUEST;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(result));
    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)slot));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_NULL,
        CLASS_AR_CARD_NAME, METHOD_ARCARD_CONTROL, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);

    debug_log(DLOG_DEBUG, "%s : action=%s, result=%d, slot=%d.", __FUNCTION__, actions, result, slot);

    uip_free_gvariant_list(input_list);
    input_list = NULL;
    (void)snprintf_s(slot_id, sizeof(slot_id), sizeof(slot_id) - 1, "%d", slot);

    return parse_control_result(ret, actions, (const gchar*)slot_id, o_message_jso);
}
