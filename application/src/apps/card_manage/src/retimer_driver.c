
#include <pthread.h>
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "retimer_driver.h"

guint8 g_retimerRoutineOpenStatus = RETIMER_OPEN_ROUTINE;
static pthread_mutex_t g_retimerRoutineMutex = PTHREAD_MUTEX_INITIALIZER;


guint8 read_retimer_routine_open_status(void)
{
    return g_retimerRoutineOpenStatus;
}


void write_retimer_routine_open_status(guint8 num)
{
    (void)pthread_mutex_lock(&g_retimerRoutineMutex);
    g_retimerRoutineOpenStatus = num;
    (void)pthread_mutex_unlock(&g_retimerRoutineMutex);
}


LOCAL void retimer_5902_driver_init(void)
{
    retimer_set_rx_ctle = retimer_set_rx_ctle_5902;
    retimer_set_tx_ffe = retimer_set_tx_ffe_5902;
    retimer_set_reg_info = retimer_set_reg_info_5902;
    retimer_get_reg_info = retimer_get_reg_info_5902;
    retimer_get_eye_diag = retimer_get_eye_diag_5902;
    retimer_get_rx_ctle = retimer_get_rx_ctle_5902;
    retimer_get_tx_ffe = retimer_get_tx_ffe_5902;
    retimer_get_waite_flag = retimer_get_waite_flag_5902;
    retimer_get_prbs_error = retimer_get_prbs_error_5902;
    retimer_prbs_send_recv = retimer_prbs_send_recv_5902;
    retimer_set_tx_swing = retimer_set_tx_swing_5902;
    retimer_get_temperature = retimer_get_temperature_5902;
    retimer_get_heartbeat = retimer_get_heartbeat_5902;
}


LOCAL void retimer_ds280_driver_init(void)
{
    retimer_get_reg_info = retimer_get_reg_info_ds280;
    retimer_get_tx_ffe = retimer_get_tx_ffe_ds280;
    retimer_set_tx_ffe = retimer_set_tx_ffe_ds280;
    retimer_get_temperature = retimer_get_temperature_ds280;
    retimer_get_eye_diag = retimer_get_eye_diag_ds280;
    retimer_get_prbs_error = retimer_get_prbs_error_ds280;
    retimer_prbs_send_recv = retimer_prbs_send_recv_ds280;
    retimer_get_heartbeat = retimer_get_heartbeat_ds280;
}


guint8 retimer_get_type(void)
{
    gint32 ret;
    GSList *obj_list = NULL;
    OBJ_HANDLE obj_handle;
    guint8 chip_type = 0xff; 

    ret = dfl_get_object_list(CLASS_RETIMER_NAME, &obj_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "dfl_get_object_list failed(ret: %d)", ret);
        return chip_type;
    }

    
    obj_handle = (OBJ_HANDLE)obj_list->data;
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_RETIMER_CHIP_TYPE, &chip_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] dal_get_property_value_byte failed. ret = %d.", dfl_get_object_name(obj_handle),
            ret);
    }
    g_slist_free(obj_list);
    obj_list = NULL;

    debug_log(DLOG_ERROR, "Current CDR chip type is (%d)", chip_type);
    return chip_type;
}


void retimer_driver_init(guint8 chip_type)
{
    switch (chip_type) {
        case CDR_CHIP_TYPE_5902L:
        case CDR_CHIP_TYPE_5902H:
            retimer_5902_driver_init();
            break;

        case CDR_CHIP_TYPE_DS280:
            retimer_ds280_driver_init();
            break;

        default:
            break;
    }
}
