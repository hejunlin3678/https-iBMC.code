

#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <math.h>
#include "pme_app/pme_app.h"
#include "pcie_card.h"
#include "pcie_card_parse_bdf_thread.h"
#include "card_ipmi_msg.h"
#include "get_version.h"
#include "card_manage_module.h"
#include "pcie_card_comm.h"
#include "pme_app/common/debug_log_macro.h"
#include "../../../libs/pcie_mgmt/inc/pcie_mgmt_by_std_smbus.h"


LOCAL gint32 netcard_businessport_bdf_info_update(OBJ_HANDLE netcard_handle, OBJ_HANDLE pcieaddr_handle);
LOCAL gint32 netcard_1822_type_businessport_bdf_info_update(OBJ_HANDLE netcard_handle, OBJ_HANDLE pcieaddr_handle,
    PCIE_NETCARD_VIDDID_SVIDSDID *pcie_netcard_ptr);
LOCAL gint32 netcard_and_port_bdf_info_update_condition_check(void);



LOCAL PCIE_NETCARD_VIDDID_SVIDSDID netcard_1822_table[] =
{
    
    {"SP570", {0x19e5, 0x371e, 0x19e5, 0xd129}, {0x19e5, 0x371e, 0x19e5, 0xd129}, {0x19e5, 0x1822, 0x19e5, 0xd129}},
    {"SD100", {0x19e5, 0x371e, 0x19e5, 0xd129}, {0x19e5, 0x371e, 0x19e5, 0xd129}, {0x19e5, 0x0211, 0x19e5, 0xd12f}},
    {"SP521", {0x19e5, 0x371e, 0x19e5, 0xd129}, {0x19e5, 0x371e, 0x19e5, 0xd129}, {0x19e5, 0x0202, 0x19e5, 0xd302}},
    {"SP520", {0x19e5, 0x371e, 0x19e5, 0xd129}, {0x19e5, 0x371e, 0x19e5, 0xd129}, {0x19e5, 0x0203, 0x19e5, 0xd301}},
    {"MZ711", {0x19e5, 0x371e, 0x19e5, 0xd129}, {0x19e5, 0x371e, 0x19e5, 0xd129}, {0x19e5, 0x020D, 0x19e5, 0xd12d}},
    {"MZ532", {0x19e5, 0x371e, 0x19e5, 0xd129}, {0x19e5, 0x371e, 0x19e5, 0xd129}, {0x19e5, 0x0210, 0x19e5, 0xdf2e}},
    {"MZ731", {0x19e5, 0x371e, 0x19e5, 0xd129}, {0x19e5, 0x371e, 0x19e5, 0xd129}, {0x19e5, 0x0205, 0x19e5, 0xdf27}},
    {"SC381", {0x19e5, 0x371e, 0x19e5, 0xd129}, {0x19e5, 0x371e, 0x19e5, 0xd129}, {0x19e5, 0x0200, 0x19e5, 0xd13a}},
    {"SC371", {0x19e5, 0x371e, 0x19e5, 0xd129}, {0x19e5, 0x371e, 0x19e5, 0xd129}, {0x19e5, 0x0200, 0x19e5, 0xd13d}},
    {"SP522", {0x19e5, 0x0212, 0x19e5, 0xd303}, {0x19e5, 0x0212, 0x19e5, 0xd303}, {0x19e5, 0x0212, 0x19e5, 0xd303}},
    {"SP571", {0x19e5, 0x0211, 0x19e5, 0xd12f}, {0x19e5, 0x0211, 0x19e5, 0xd12f}, {0x19e5, 0x0211, 0x19e5, 0xd12f}},
    {"SP581", {0x19e5, 0x0211, 0x19e5, 0xd137}, {0x19e5, 0x0211, 0x19e5, 0xd137}, {0x19e5, 0x0211, 0x19e5, 0xd137}},
    {"SP580", {0x19e5, 0x1822, 0x19e5, 0xd136}, {0x19e5, 0x1822, 0x19e5, 0xd136}, {0x19e5, 0x1822, 0x19e5, 0xd136}},
    {"SP525", {0x19e5, 0x0203, 0x19e5, 0xd305}, {0x19e5, 0x0203, 0x19e5, 0xd305}, {0x19e5, 0x0203, 0x19e5, 0xd305}},
    {"SP582", {0x19e5, 0x0206, 0x19e5, 0xd138}, {0x19e5, 0x0206, 0x19e5, 0xd138}, {0x19e5, 0x0206, 0x19e5, 0xd138}},
    {"SP526", {0x19e5, 0x0202, 0x19e5, 0xd304}, {0x19e5, 0x0202, 0x19e5, 0xd304}, {0x19e5, 0x0202, 0x19e5, 0xd304}},
    {"SP523", {0x19e5, 0x0212, 0x19e5, 0xd306}, {0x19e5, 0x0212, 0x19e5, 0xd306}, {0x19e5, 0x0212, 0x19e5, 0xd306}},
    {"SP572", {0x19e5, 0x0200, 0x19e5, 0xd139}, {0x19e5, 0x0200, 0x19e5, 0xd139}, {0x19e5, 0x0200, 0x19e5, 0xd139}},
    {"SP583", {0x19e5, 0x375e, 0x19e5, 0xd129}, {0x19e5, 0x375e, 0x19e5, 0xd129}, {0x19e5, 0x1822, 0x19e5, 0xd141}},
    {"SP583", {0x19e5, 0x375e, 0x19e5, 0xd129}, {0x19e5, 0x375e, 0x19e5, 0xd129}, {0x19e5, 0x0211, 0x19e5, 0xd142}},
    {"RM210", {0x19e5, 0x371e, 0x19e5, 0xd143}, {0x19e5, 0x371e, 0x19e5, 0xd143}, {0x19e5, 0x0206, 0x19e5, 0xd143}},
    {"SP586", {0x19e5, 0x0206, 0x19e5, 0xd145}, {0x19e5, 0x0206, 0x19e5, 0xd145}, {0x19e5, 0x0206, 0x19e5, 0xd145}},
    {"SP527", {0x19e5, 0x0203, 0x19e5, 0xd148}, {0x19e5, 0x0203, 0x19e5, 0xd148}, {0x19e5, 0x0203, 0x19e5, 0xd148}}
};


#define PCIE_1822_NETCARD_ITEM_COUNT (sizeof(netcard_1822_table) / sizeof(netcard_1822_table[0]))
#define BDF_NO_MAX 2

LOCAL gint32 check_in_netcard_1822_table_by_id(guint16 vender_id, guint16 device_id,
    PCIE_NETCARD_VIDDID_SVIDSDID **pcie_netcard_ptr)
{
    guint32 idx;

    if (pcie_netcard_ptr == NULL) {
        return RET_ERR;
    }

    debug_log(DLOG_DEBUG, "begin find venderid:0x%02x,deviceid:0x%02x", vender_id, device_id);
    for (idx = 0; idx < PCIE_1822_NETCARD_ITEM_COUNT; idx++) {
        if ((netcard_1822_table[idx].pfid.vid == vender_id) && (netcard_1822_table[idx].pfid.did == device_id)) {
            *pcie_netcard_ptr = &netcard_1822_table[idx];
            debug_log(DLOG_INFO, "find 1822 card success, name is %s", netcard_1822_table[idx].card_name);
            return RET_OK;
        }
    }
    debug_log(DLOG_DEBUG, "find venderid:0x%02x,deviceid:0x%02x failed", vender_id, device_id);
    return RET_ERR;
}

LOCAL gint32 check_in_netcard_1822_table_by_name(const gchar *card_name,
    PCIE_NETCARD_VIDDID_SVIDSDID **pcie_netcard_ptr)
{
    guint32 idx;

    if (card_name == NULL || pcie_netcard_ptr == NULL) {
        return RET_ERR;
    }

    debug_log(DLOG_DEBUG, "begin find card name %s", card_name);
    for (idx = 0; idx < PCIE_1822_NETCARD_ITEM_COUNT; idx++) {
        if (strcmp(netcard_1822_table[idx].card_name, card_name) == 0) {
            *pcie_netcard_ptr = &netcard_1822_table[idx];
            debug_log(DLOG_INFO, "find 1822 card success, name is %s", netcard_1822_table[idx].card_name);
            return RET_OK;
        }
    }

    debug_log(DLOG_DEBUG, "find card name %s failed", card_name);
    return RET_ERR;
}


LOCAL gint32 netcard_is_1822_type_check(OBJ_HANDLE netcard_handle, PCIE_NETCARD_VIDDID_SVIDSDID **pcie_netcard_ptr)
{
    gint32 ret;
    OBJ_HANDLE pciecard_handle = 0;
    OBJ_HANDLE ocpcard_handle = 0;
    OBJ_HANDLE mezzcard_handle = 0;
    guint8 slot_id = 0;
    guint16 vender_id;
    guint16 device_id;
    gchar card_name[MAX_INFO_LEN + 1] = {0};
    if ((netcard_handle == 0) || (pcie_netcard_ptr == NULL)) {
        debug_log(DLOG_ERROR, "Invalid input, netcard_handle is 0 or pcie_netcard_ptr is NULL");
        return RET_ERR;
    }

    
    ret = dal_get_specific_object_position(netcard_handle, CLASS_PCIECARD_NAME, &pciecard_handle);
    if ((ret == RET_OK) && (pciecard_handle != 0)) {
        
        ret = dal_get_property_value_byte(pciecard_handle, PROPETRY_PCIECARD_SLOT, &slot_id);
        if ((ret != RET_OK) || (slot_id > MAX_PCIE_CARD) || (slot_id <= 0)) {
            debug_log(DLOG_ERROR, "Get %s PcieCardSlot failed, ret = %d, slot = %u",
                dfl_get_object_name(pciecard_handle), ret, slot_id);
            return RET_ERR;
        }
        vender_id = g_pcie_group.info[slot_id - 1].vender_id;
        device_id = g_pcie_group.info[slot_id - 1].device_id;
        return check_in_netcard_1822_table_by_id(vender_id, device_id, pcie_netcard_ptr);
    }

    ret = dal_get_specific_object_position(netcard_handle, CLASS_OCP_CARD, &ocpcard_handle);
    if ((ret == RET_OK) && (ocpcard_handle != 0)) {
        ret = dal_get_property_value_byte(ocpcard_handle, PROPETRY_PCIECARD_SLOT, &slot_id);
        if ((ret != RET_OK) || (slot_id > MAX_PCIE_CARD) || (slot_id <= 0)) {
            debug_log(DLOG_ERROR, "Get %s OCPCardSlot failed, ret = %d, slot = %u", dfl_get_object_name(ocpcard_handle),
                ret, slot_id);
            return RET_ERR;
        }
        vender_id = g_ocp_group.info[slot_id - 1].vender_id;
        device_id = g_ocp_group.info[slot_id - 1].device_id;
        return check_in_netcard_1822_table_by_id(vender_id, device_id, pcie_netcard_ptr);
    }

    ret = dal_get_specific_object_position(netcard_handle, CLASS_MEZZCARD_NAME, &mezzcard_handle);
    if ((ret == RET_OK) && (mezzcard_handle != 0)) {
        ret =
            dal_get_property_value_string(mezzcard_handle, PROPERTY_MEZZCARD_PRODUCTNAME, card_name, sizeof(card_name));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Get %s mezzCard name failed, ret = %d", dfl_get_object_name(ocpcard_handle), ret);
            return RET_ERR;
        }
        return check_in_netcard_1822_table_by_name(card_name, pcie_netcard_ptr);
    }
    return RET_ERR;
}


LOCAL gint32 netcard_append_rootbdf(OBJ_HANDLE netcard_handle, const gchar bdf_str[])
{
    gint32 ret;
    gchar bdf_str_prev[MAX_BDF_STR_LEN] = {0};
    gchar bdf_str_des[MAX_BDF_STR_LEN] = {0};

    ret = dal_get_property_value_string(netcard_handle, PROPERTY_NETCARD_ROOT_BDF, bdf_str_prev, sizeof(bdf_str_prev));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Get %s Function failed, ret = %d.", __FUNCTION__,
            dfl_get_object_name(netcard_handle), ret);
        return RET_ERR;
    }

    ret = snprintf_s(bdf_str_des, MAX_BDF_STR_LEN, MAX_BDF_STR_LEN - 1, "%s,%s", bdf_str_prev, bdf_str);
    if (ret < 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed, ret = %d.", __FUNCTION__, ret);
        return RET_ERR;
    }
    ret = dal_set_property_value_string(netcard_handle, PROPERTY_NETCARD_ROOT_BDF, bdf_str_des, DF_NONE);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set %s rootBDF failed, ret = %d.", dfl_get_object_name(netcard_handle), ret);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL gint32 get_netcard_bdf_str(OBJ_HANDLE pcieaddr_handle, gchar *bdf_str, guint32 len)
{
    gint32 ret;
    
    guint16 segment = 0;
    guint8 bus = 0;
    guint8 device = 0;
    guint8 function = 0;

    ret = dal_get_property_value_uint16(pcieaddr_handle, PROPERTY_SEGMENT, &segment);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get %s segment failed, ret = %d.", dfl_get_object_name(pcieaddr_handle), ret);
        return ret;
    }

    ret = dal_get_property_value_byte(pcieaddr_handle, PROPERTY_PCIE_BUS, &bus);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get %s Bus failed, ret = %d.", dfl_get_object_name(pcieaddr_handle), ret);
        return ret;
    }
    ret = dal_get_property_value_byte(pcieaddr_handle, PROPERTY_PCIE_DEVICE, &device);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get %s Device failed, ret = %d.", dfl_get_object_name(pcieaddr_handle), ret);
        return ret;
    }

    ret = dal_get_property_value_byte(pcieaddr_handle, PROPERTY_PCIE_FUNCTION, &function);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get %s Function failed, ret = %d.", dfl_get_object_name(pcieaddr_handle), ret);
        return ret;
    }

    (void)snprintf_s(bdf_str, len, len - 1, "%04x:%02x:%02x.%01x", segment, bus, device,
        function);
    debug_log(DLOG_INFO, "rootBDF:%s", bdf_str);

    return ret;
}
 

LOCAL gint32 netcard_rootport_bdf_info_update(OBJ_HANDLE netcard_handle, OBJ_HANDLE pcieaddr_handle,
    gboolean is_first_bdf)
{
    gint32 ret;
    gchar bdf_str[MAX_BDF_STR_LEN] = {0};
    gchar bdf_str_des[MAX_BDF_STR_LEN] = {0};
    guint8 bdfno;

    if (netcard_handle == 0 || pcieaddr_handle == 0) {
        debug_log(DLOG_ERROR, "%s,Invalid parameter.", __FUNCTION__);
        return RET_ERR;
    }

    ret = get_netcard_bdf_str(pcieaddr_handle, bdf_str, MAX_BDF_STR_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get %s netcard bdf str failed, ret = %d.", __FUNCTION__,
            dfl_get_object_name(pcieaddr_handle), ret);
        return ret;
    }

    ret = dal_get_property_value_string(netcard_handle, PROPERTY_NETCARD_ROOT_BDF, bdf_str_des, sizeof(bdf_str_des));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get %s Function failed, ret = %d.", __FUNCTION__,
            dfl_get_object_name(netcard_handle), ret);
        return ret;
    }
    ret = dal_get_property_value_byte(pcieaddr_handle, PROPERTY_BDF_NO, &bdfno);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get %s Device failed, ret = %d.", __FUNCTION__, dfl_get_object_name(pcieaddr_handle),
            ret);
        return ret;
    }

    if (is_first_bdf == TRUE) {
        ret = dal_set_property_value_string(netcard_handle, PROPERTY_NETCARD_ROOT_BDF, bdf_str, DF_NONE);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Set %s rootBDF failed, ret = %d.", dfl_get_object_name(netcard_handle), ret);
            return RET_ERR;
        }
        return RET_OK;
    } else {
        return netcard_append_rootbdf(netcard_handle, bdf_str);
    }
}


LOCAL gint32 get_netcard_port_bdfno_existence(OBJ_HANDLE netcard_handle, gint8 bdfno_exist[], guint32 len)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE businessport_handle = 0;
    guint8 port_bdfno;

    ret = dal_get_object_list_position(netcard_handle, ETH_CLASS_NAME_BUSY_ETH, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get %s same position level BusinessPort handle list failed, ret = %d.",
            dfl_get_object_name(netcard_handle), ret);
        return RET_ERR;
    }

    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        businessport_handle = (OBJ_HANDLE)obj_node->data;

        ret = dal_get_property_value_byte(businessport_handle, PROPERTY_BDF_NO, &port_bdfno);
        if (ret != DFL_OK || port_bdfno >= len) {
            debug_log(DLOG_ERROR, "Get %s BDFNo failed, ret = %d, port_bdfno = %u",
                dfl_get_object_name(businessport_handle), ret, port_bdfno);
            continue;
        }
        bdfno_exist[port_bdfno] = 1;
    }
    g_slist_free(obj_list);
    return RET_OK;
}


gint32 update_netcard_and_port_bdf_info(OBJ_HANDLE netcard_handle, gpointer user_data)
{
    gint32 ret;
    OBJ_HANDLE pcieaddr_handle = 0;
    PCIE_NETCARD_VIDDID_SVIDSDID *pcie_1822_netcard_type_ptr = NULL;
    gint32 bdfno;
    gint8 bdfno_exist[BDF_NO_MAX] = {0};
    gboolean is_first_bdf = TRUE;

    if (netcard_handle == 0) {
        debug_log(DLOG_ERROR, "%s: Invalid input!", __FUNCTION__);
        return RET_OK;
    }

    
    if (netcard_and_port_bdf_info_update_condition_check() == RET_ERR) {
        debug_log(DLOG_INFO, "%s: skip netcard bdf info update process.", __FUNCTION__);
        return RET_OK;
    }

    ret = get_netcard_port_bdfno_existence(netcard_handle, bdfno_exist, BDF_NO_MAX);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: check %s port BDFNo Failed.", __FUNCTION__, dfl_get_object_name(netcard_handle));
        return RET_OK;
    }

    for (bdfno = 0; bdfno < BDF_NO_MAX; bdfno++) {
        if (bdfno_exist[bdfno] == 0) {
            continue;
        }
        
        ret = pcie_find_netcard_related_pcieaddrinfo_handle(netcard_handle, bdfno, &pcieaddr_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: find %s related pcieaddrinfo handle failed.", __FUNCTION__,
                dfl_get_object_name(netcard_handle));
            continue;
        }
        
        ret = netcard_rootport_bdf_info_update(netcard_handle, pcieaddr_handle, is_first_bdf);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: update %s rootBDF failed.", __FUNCTION__, dfl_get_object_name(netcard_handle));
            return RET_OK;
        }
        is_first_bdf = FALSE;
    }

    
    
    if (netcard_is_1822_type_check(netcard_handle, &pcie_1822_netcard_type_ptr) == RET_ERR) {
        debug_log(DLOG_INFO, "%s,enter normal netcard update process.", __FUNCTION__);
        ret = netcard_businessport_bdf_info_update(netcard_handle, pcieaddr_handle);
    } else {
        debug_log(DLOG_INFO, "%s,enter 1822 netcard update process.", __FUNCTION__);
        ret =
            netcard_1822_type_businessport_bdf_info_update(netcard_handle, pcieaddr_handle, pcie_1822_netcard_type_ptr);
    }
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: update %s Related BusinessPort BDF failed.", __FUNCTION__,
            dfl_get_object_name(netcard_handle));
        return RET_OK;
    }
    
    return RET_OK;
}


LOCAL gboolean is_mlnx_shared_bdf_port(OBJ_HANDLE netcard_handle, guint8 *function)
{
    guint8 net_type = 0;
    OBJ_HANDLE mezz_handle = 0;
    gint32 ret;
    guint16 board_id = 0;
    const static guint16 mlnx_shared_board[] = {0x0093};
    const static guint8 mlnx_shared_function[] = {0};

    ret = dal_get_property_value_byte(netcard_handle, PROPERTY_NETCARD_CARDTYPE, &net_type);
    if (ret != DFL_OK || net_type != NET_TYPE_MEZZ_CATED) {
        return FALSE;
    }

    ret = dal_get_specific_object_position(netcard_handle, CLASS_MEZZ, &mezz_handle);
    if (ret != DFL_OK || mezz_handle == 0) {
        debug_log(DLOG_ERROR, "Get %s same position level MezzCard handle failed, ret = %d.",
            dfl_get_object_name(netcard_handle), ret);
        return FALSE;
    }

    ret = dal_get_property_value_uint16(mezz_handle, PROPERTY_MEZZCARD_BOARDID, &board_id);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get %s same position level MezzCard board_id failed, ret = %d.",
            dfl_get_object_name(netcard_handle), ret);
        return FALSE;
    }

    for (guint32 i = 0; i < G_N_ELEMENTS(mlnx_shared_board); i++) {
        if (mlnx_shared_board[i] == board_id) {
            *function = mlnx_shared_function[i];
            debug_log(DLOG_INFO, "%s is a Mezzine card that shares bdf, board_id = %u",
                dfl_get_object_name(netcard_handle), board_id);
            return TRUE;
        }
    }
    return FALSE;
}


LOCAL gint32 update_pcie_addr_info(OBJ_HANDLE netcard_handle, OBJ_HANDLE businessport_handle,
    OBJ_HANDLE pcieaddr_handle, PcieAddrInfo *pcie_addr_info)
{
    guint8 port_bdfno = 0;
    gint32 ret;

    ret = dal_get_property_value_byte(businessport_handle, PROPERTY_BDF_NO, &port_bdfno);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get %s BDFNo failed, ret = %d.", dfl_get_object_name(businessport_handle), ret);
        return RET_ERR;
    }

    if (pcie_addr_info->bdfno == port_bdfno) {
        return RET_OK;
    }

    ret = pcie_find_netcard_related_pcieaddrinfo_handle(netcard_handle, port_bdfno, &pcieaddr_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Update %s related pcieaddrinfo failed, ret = %d.",
            dfl_get_object_name(businessport_handle), ret);
        return RET_ERR;
    }
    ret = get_pcieaddr_info(pcieaddr_handle, pcie_addr_info);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get %s info failed, ret = %d", __FUNCTION__, dfl_get_object_name(pcieaddr_handle),
            ret);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 netcard_businessport_bdf_info_update(OBJ_HANDLE netcard_handle, OBJ_HANDLE pcieaddr_handle)
{
    gint32 ret = RET_OK;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint8 total_port_num = 0;
    guint8 update_port_num = 0;
    guint8 port_num = 0;
    OBJ_HANDLE businessport_handle = 0;
    OBJ_HANDLE ref_netcard_handle = 0;

    PcieAddrInfo pcie_addr_info = { 0 };
    guint8 bus_index = 0;
    gchar bdf_str[MAX_BDF_STR_LEN] = {0};
    PCIE_VIDDID_SVIDSDID pcie_viddid = { 0 };
    GSList *input_list = NULL;
    guint8 function = 0;
    gboolean is_shared_bdf_port = FALSE;

    if (netcard_handle == 0 || pcieaddr_handle == 0) {
        debug_log(DLOG_ERROR, "%s,Invalid parameter.", __FUNCTION__);
        return DFL_ERR;
    }

    ret = dal_get_object_list_position(netcard_handle, ETH_CLASS_NAME_BUSY_ETH, &obj_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get %s same position level BusinessPort handle list failed, ret = %d.",
            dfl_get_object_name(netcard_handle), ret);
        return ret;
    }
    (void)dal_get_property_value_byte(netcard_handle, PROPERTY_NETCARD_PORTNUM, &total_port_num);

    ret = get_pcieaddr_info(pcieaddr_handle, &pcie_addr_info);
    if (ret != RET_OK) {
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "Get %s info failed, ret = %d", dfl_get_object_name(pcieaddr_handle), ret);
        return ret;
    }

    is_shared_bdf_port = is_mlnx_shared_bdf_port(netcard_handle, &function);

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        businessport_handle = (OBJ_HANDLE)obj_node->data;

        ret = dfl_get_referenced_object(businessport_handle, BUSY_ETH_ATTRIBUTE_REF_NETCARD, &ref_netcard_handle);
        if (ret != DFL_OK) {
            g_slist_free(obj_list);
            debug_log(DLOG_ERROR, "%s:%s get %s failed, ret:%d.", __FUNCTION__,
                dfl_get_object_name(businessport_handle), BUSY_ETH_ATTRIBUTE_REF_NETCARD, ret);
            return ret;
        }

        if (g_strcmp0(dfl_get_object_name(netcard_handle), dfl_get_object_name(ref_netcard_handle)) != 0) {
            debug_log(DLOG_INFO, "businessport %s is not referenced to %s, skip bdf updating.",
                dfl_get_object_name(businessport_handle), dfl_get_object_name(netcard_handle));
            continue;
        }

        if (update_pcie_addr_info(netcard_handle, businessport_handle, pcieaddr_handle, &pcie_addr_info) != RET_OK) {
            continue;
        }
        
        ret = dal_get_property_value_byte(businessport_handle, BUSY_ETH_ATTRIBUTE_PORT_NUM, &port_num);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Get %s PortNum failed, ret = %d.", dfl_get_object_name(businessport_handle), ret);
            continue;
        }

        // 一般网口的PCIe Function等于其PortNum
        // MZ710等存在共享BDF的情况，Function采用给定值
        if (is_shared_bdf_port == FALSE) {
            function = port_num;
        }

        for (bus_index = pcie_addr_info.secbus; bus_index <= pcie_addr_info.subbus; bus_index++) {
            ret = pciecard_test_bdf_is_valid(pcie_addr_info.socket_id, bus_index, 0, function, &pcie_viddid);
            if (ret == RET_OK) {
                break;
            }
        }

        if (bus_index == (pcie_addr_info.subbus + 1)) {
            debug_log(DLOG_ERROR, "find %s RootBDF failed.", dfl_get_object_name(businessport_handle));
            update_port_num++;
            continue;
        }

        
        (void)memset_s(bdf_str, MAX_BDF_STR_LEN, 0, MAX_BDF_STR_LEN);
        if (is_shared_bdf_port == TRUE && port_num != 0) {
            (void)snprintf_s(bdf_str, MAX_BDF_STR_LEN, MAX_BDF_STR_LEN - 1, "%04x:%02x:%02x.%01x_%01x",
                pcie_addr_info.segment, bus_index, 0, function, port_num);
        } else {
            (void)snprintf_s(bdf_str, MAX_BDF_STR_LEN, MAX_BDF_STR_LEN - 1, "%04x:%02x:%02x.%01x",
                pcie_addr_info.segment, bus_index, 0, function);
        }
        debug_log(DLOG_INFO, "Port:%s,BusinessPort BDF:%s", dfl_get_object_name(businessport_handle), bdf_str);

        
        input_list = g_slist_append(input_list, g_variant_new_string(bdf_str));
        ret = dfl_call_class_method(businessport_handle, BUSY_ETH_METHOD_SET_BDF, NULL, input_list, NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        input_list = NULL;
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Set %s BDF failed, ret = %d.", dfl_get_object_name(businessport_handle), ret);
        }

        update_port_num++;
    }

    g_slist_free(obj_list);

    if (update_port_num == total_port_num) {
        return RET_OK;
    } else {
        return RET_ERR;
    }
}


LOCAL gint32 netcard_1822_type_businessport_bdf_info_update(OBJ_HANDLE netcard_handle, OBJ_HANDLE pcieaddr_handle,
    PCIE_NETCARD_VIDDID_SVIDSDID *pcie_netcard_ptr)
{
    gint32 ret = RET_OK;
    guint8 total_port_num = 0;
    guint8 update_port_num = 0;
    OBJ_HANDLE businessport_handle = 0;
    guint8 port_num = 0;
    
    guint16 segment = 0;
    guint8 socket_id = 0;
    guint8 secbus = 0;
    guint8 subbus = 0;
    guint8 bus_index = 0;
    guint8 dev_index = 0;
    guint8 fun_index = 0;
    gchar bdf_str[MAX_BDF_STR_LEN] = {0};
    PCIE_VIDDID_SVIDSDID pcie_viddid = { 0 };
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    GSList *port_bdf_str_list = NULL;
    const gchar *bdf_str_ptr = NULL;
    guint8 bus_index_skip_flag = FALSE;
    guint8 dev_index_skip_flag = FALSE;
    GSList *input_list = NULL;

    if (netcard_handle == 0 || 0 == pcieaddr_handle || NULL == pcie_netcard_ptr) {
        debug_log(DLOG_ERROR, "%s: Invalid input!", __FUNCTION__);
        return RET_ERR;
    }

    (void)dal_get_property_value_byte(netcard_handle, PROPERTY_NETCARD_PORTNUM, &total_port_num);

    
    (void)dal_get_property_value_byte(pcieaddr_handle, PROPERTY_SOCKET_ID, &socket_id);
    (void)dal_get_property_value_byte(pcieaddr_handle, PROPERTY_SEC_BUS, &secbus);
    (void)dal_get_property_value_byte(pcieaddr_handle, PROPERTY_SUB_BUS, &subbus);

    
    for (bus_index = secbus; (bus_index <= subbus) && (FALSE == bus_index_skip_flag); bus_index++) {
        dev_index_skip_flag = FALSE;
        for (dev_index = 0; (dev_index <= 31) && (FALSE == dev_index_skip_flag); dev_index++) {
            for (fun_index = 0; fun_index <= 7; fun_index++) {
                debug_log(DLOG_INFO, "bus_index:%02x,dev_index:%02x,fun_index:%02x", bus_index, dev_index, fun_index);
                (void)memset_s((void *)&pcie_viddid, sizeof(PCIE_VIDDID_SVIDSDID), 0, sizeof(PCIE_VIDDID_SVIDSDID));
                ret = pciecard_test_bdf_is_valid(socket_id, bus_index, dev_index, fun_index, &pcie_viddid);
                if (ret == RET_ERR) {
                    
                    if (fun_index == 0) {
                        dev_index_skip_flag = TRUE;
                    }
                    break;
                } else if (((pcie_viddid.vid == pcie_netcard_ptr->upid.vid) &&
                    (pcie_viddid.did == pcie_netcard_ptr->upid.did)) ||
                    ((pcie_viddid.vid == pcie_netcard_ptr->dpid.vid) &&
                    (pcie_viddid.did == pcie_netcard_ptr->dpid.did))) {
                    dev_index_skip_flag = TRUE;
                    break;
                } else if ((pcie_viddid.vid == pcie_netcard_ptr->pfid.vid) &&
                    (pcie_viddid.did == pcie_netcard_ptr->pfid.did)) {
                    
                    update_port_num++;
                    
                    (void)memset_s(bdf_str, MAX_BDF_STR_LEN, 0, MAX_BDF_STR_LEN);
                    (void)snprintf_s(bdf_str, MAX_BDF_STR_LEN, MAX_BDF_STR_LEN - 1, "%04x:%02x:%02x.%01x", segment,
                        bus_index, dev_index, fun_index);
                    debug_log(DLOG_INFO, "Port%d,BusinessPort BDF:%s ", update_port_num, bdf_str);
                    port_bdf_str_list = g_slist_append(port_bdf_str_list, g_variant_new_string(bdf_str));

                    
                    if (update_port_num == total_port_num) {
                        debug_log(DLOG_INFO, "find all expected port number(%d).", update_port_num);
                        dev_index_skip_flag = TRUE;
                        bus_index_skip_flag = TRUE;
                        break;
                    }
                } else { 
                    continue;
                }
            }
        }
    }

    
    if (update_port_num == total_port_num) {
        ret = dal_get_object_list_position(netcard_handle, ETH_CLASS_NAME_BUSY_ETH, &obj_list);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Get %s same position level BusinessPort handle list failed, ret = %d.",
                dfl_get_object_name(netcard_handle), ret);
            g_slist_free_full(port_bdf_str_list, (GDestroyNotify)g_variant_unref);
            return ret;
        }

        for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
            businessport_handle = (OBJ_HANDLE)obj_node->data;
            ret = dal_get_property_value_byte(businessport_handle, BUSY_ETH_ATTRIBUTE_PORT_NUM, &port_num);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "Get %s PortNum failed, ret = %d.", dfl_get_object_name(businessport_handle),
                    ret);
                continue;
            }

            bdf_str_ptr = g_variant_get_string((GVariant *)g_slist_nth_data(port_bdf_str_list, port_num), NULL);
            if (bdf_str_ptr != NULL) {
                debug_log(DLOG_INFO, "SET Port(%s),BusinessPort BDF:%s ", dfl_get_object_name(businessport_handle),
                    bdf_str_ptr);

                
                input_list = g_slist_append(input_list, g_variant_new_string(bdf_str_ptr));
                ret = dfl_call_class_method(businessport_handle, BUSY_ETH_METHOD_SET_BDF, NULL, input_list, NULL);
                g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
                input_list = NULL;
                bdf_str_ptr = NULL;
                if (ret != DFL_OK) {
                    debug_log(DLOG_ERROR, "Set %s BDF failed, ret = %d.", dfl_get_object_name(businessport_handle),
                        ret);
                }
            } else {
                debug_log(DLOG_ERROR, "Impossible find Port(%s) in list ",
                    dfl_get_object_name(businessport_handle));
            }
        }

        g_slist_free_full(port_bdf_str_list, (GDestroyNotify)g_variant_unref);
        g_slist_free(obj_list);
        return RET_OK;
    } else {
        debug_log(DLOG_ERROR, "update 1822 %s BusinessPort BDF failed,port number(%d).",
            dfl_get_object_name(netcard_handle), update_port_num);
        if (port_bdf_str_list) {
            g_slist_free_full(port_bdf_str_list, (GDestroyNotify)g_variant_unref);
        }
        return RET_ERR;
    }
}


LOCAL gint32 netcard_and_port_bdf_info_update_condition_check(void)
{
    gint32 ret = RET_OK;
    OBJ_HANDLE physical_partition_handle = 0;

    
    ret = dal_get_object_handle_nth(CLASS_COMPUTER_PARTITION, 0, &physical_partition_handle);
    if (ret == RET_OK) {
        debug_log(DLOG_ERROR, "%s: skip netcard bdf info update process.", __FUNCTION__);
        return RET_ERR;
    }
    return RET_OK;
}


gint32 _pf_port_compare(gconstpointer a, gconstpointer b)
{
    const PFPORT_INFO_S *p_pfport_info_a = (const PFPORT_INFO_S *)a;
    const PFPORT_INFO_S *p_pfport_info_b = (const PFPORT_INFO_S *)b;
    // 优先按照port从小到大排序
    if (p_pfport_info_a->physical_port > p_pfport_info_b->physical_port) {
        return TRUE;
    }
    // 如果port相等，按pf从小到大排序；
    else if (p_pfport_info_a->physical_port == p_pfport_info_b->physical_port) {
        if (p_pfport_info_a->pf > p_pfport_info_b->pf) {
            return TRUE;
        }
    }
    return FALSE;
}
