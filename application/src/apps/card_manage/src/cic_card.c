

#include "pme/pme.h"
#include "pme_app/pme_app.h"


#include "cic_card.h"
#include "get_version.h"

#include "pme_app/common/debug_log_macro.h"

#define CIC_CARD_PRESENCE 1


LOCAL gint32 process_each_ciccard(OBJ_HANDLE object_handle, gpointer user_data)
{
    gint32 ret;
    guint8 presence;

    
    ret = dal_get_property_value_byte(object_handle, PROPERTY_CIC_PRESENCE, &presence);
    if (ret == RET_OK && presence == CIC_CARD_PRESENCE) {
        debug_log(DLOG_ERROR, "%s: CICCard is presence.", __FUNCTION__);
    } else {
        debug_log(DLOG_ERROR, "%s: CICCard is not presence.", __FUNCTION__);
    }

    
    ret = get_pcb_version(object_handle, PROPERTY_CARD_PCBID, PROPERTY_CARD_PCBVER);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get CICCard PCB Version failed.", __FUNCTION__);
    }

    return RET_OK;
}


gint32 cic_card_add_object_callback(OBJ_HANDLE object_handle)
{
    return process_each_ciccard(object_handle, NULL);
}


gint32 cic_card_del_object_callback(OBJ_HANDLE object_handle)
{
    return RET_OK;
}


gint32 cic_card_init(void)
{
    gint32 ret;
    ret = dfl_foreach_object(CLASS_CIC_CARD, process_each_ciccard, NULL, NULL);
    if (ret == ERRCODE_OBJECT_NOT_EXIST) {
        debug_log(DLOG_ERROR, "%s: CICCard is not presence.", __FUNCTION__);
    }
    return RET_OK;
}


gint32 cic_card_start(void)
{
    return RET_OK;
}
