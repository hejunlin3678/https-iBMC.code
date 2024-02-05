

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






LOCAL gint32 process_each_m2transformcard(OBJ_HANDLE obj_handle, gpointer user_data)
{
    gint32 ret = 0;

    
    ret = get_pcb_version(obj_handle, PROPERTY_CARD_PCBID, PROPERTY_CARD_PCBVER);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    return RET_OK;
}

gint32 m2transformcard_add_object_callback(OBJ_HANDLE obj_handle)
{
    if (obj_handle == 0) {
        return RET_OK;
    }

    return process_each_m2transformcard(obj_handle, NULL);
}

gint32 m2transformcard_del_object_callback(OBJ_HANDLE obj_handle)
{
    return RET_OK;
}

LOCAL gint32 process_each_pcietransformcard(OBJ_HANDLE obj_handle, gpointer user_data)
{
    gint32 ret = 0;

    
    ret = get_pcb_version(obj_handle, PROPERTY_CARD_PCBID, PROPERTY_CARD_PCBVER);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    return RET_OK;
}
gint32 pcietransformcard_add_object_callback(OBJ_HANDLE obj_handle)
{
    if (obj_handle == 0) {
        return RET_OK;
    }

    return process_each_pcietransformcard(obj_handle, NULL);
}

gint32 pcietransformcard_del_object_callback(OBJ_HANDLE obj_handle)
{
    return RET_OK;
}



gint32 m2transformcard_init(void)
{
    (void)dfl_foreach_object(CLASS_M2TRANSFORMCARD, process_each_m2transformcard, NULL, NULL);

    return RET_OK;
}



gint32 pcietransformcard_init(void)
{
    (void)dfl_foreach_object(CLASS_PCIETRANSFORMCARD, process_each_pcietransformcard, NULL, NULL);

    return RET_OK;
}
