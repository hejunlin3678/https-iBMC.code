


#include "pme_app/pme_app.h"


#include "sw_card.h"
#include "get_version.h"


LOCAL gint32 process_each_swcard(OBJ_HANDLE object_handle, gpointer user_data)
{
    gint32 ret = 0;

    
    ret = get_pcb_version(object_handle, PROPERTY_CARD_PCBID, PROPERTY_CARD_PCBVER);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    return RET_OK;
}


gint32 sw_card_add_object_callback(OBJ_HANDLE object_handle)
{
    return process_each_swcard(object_handle, NULL);
}


gint32 sw_card_del_object_callback(OBJ_HANDLE object_handle)
{
    return RET_OK;
}


gint32 sw_card_init(void)
{
    (void)dfl_foreach_object(CLASS_CARD_NAME, process_each_swcard, NULL, NULL);

    return RET_OK;
}


gint32 sw_card_start(void)
{
    return RET_OK;
}
