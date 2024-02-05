

#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/file.h>

#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "retimer_driver.h"
#include "retimer_dft.h"

LOCAL TASKID g_taskId = 0;
DFT_RESULT_S g_testResult = { 0 };


LOCAL void dft_get_optical_module_intl(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    guint8 optical_intl = 0;
    OBJ_HANDLE optical_handle;

    
    ret = dfl_get_referenced_object(obj_handle, PROPERTY_RETIMER_REFOPTICAL, &optical_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get referentce obj (%s) failed(%d).", dfl_get_object_name(obj_handle), ret);
        return;
    }

    (void)dal_get_property_value_byte(optical_handle, PROPERTY_OPT_MDL_LOW_SPEED_IOINFO, &optical_intl);
    debug_log(DLOG_ERROR, "[%s] INTL = 0x%x.", dfl_get_object_name(optical_handle), optical_intl);
}


LOCAL void dft_get_optical_module_temprature(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    double optical_temp = 0.0;
    OBJ_HANDLE optical_handle;

    
    ret = dfl_get_referenced_object(obj_handle, PROPERTY_RETIMER_REFOPTICAL, &optical_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get referentce obj (%s) failed(%d).", dfl_get_object_name(obj_handle), ret);
        return;
    }

    (void)dal_get_property_value_double(optical_handle, PROPERTY_OPT_MDL_TEMP_CURRENT, &optical_temp);
    debug_log(DLOG_ERROR, "[%s] optical temprature = %f.", dfl_get_object_name(optical_handle), optical_temp);
}


LOCAL void dft_get_loss(OBJ_HANDLE obj_handle)
{
    gint32 ret;

    ret = retimer_check_lane_loss(obj_handle);
    debug_log(DLOG_ERROR, "[%s] retimer lane los state = %d", dfl_get_object_name(obj_handle), ret);
}


LOCAL void dft_get_eye(OBJ_HANDLE obj_handle)
{
    guint8 i;
    gint32 ret;
    guint8 retimer_id;
    guint8 channel_id;
    gint8 data_info[BUFFER_MAX_LEN] = {};
    gchar* fileld_name[0x4] = {"eye_bot", "eye_top", "eye_left", "eye_right"};
    guint32 tmp_len;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_RETIMER_ID, &retimer_id);

    for (channel_id = 0; channel_id < RETIMER_LANE_NUM; channel_id++) {
        ret = retimer_get_eye_diag(obj_handle, channel_id, data_info, BUFFER_MAX_LEN);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Read eye diag failed: %d. retimer_id = %d.", ret, retimer_id);
            continue;
        }
        if ((data_info[SMBUS_READ_DATA_OFFSET + 1] != 0) || (data_info[SMBUS_READ_DATA_OFFSET] != 0)) {
            debug_log(DLOG_ERROR, "Read eye diag failed: %d-%d. retimer_id = %d.", data_info[SMBUS_READ_DATA_OFFSET],
                data_info[SMBUS_READ_DATA_OFFSET + 1], retimer_id);
            continue;
        }
        debug_log(DLOG_ERROR, "Eye diag, lane(%d): ", channel_id);
        for (i = 0; i < 0x4; i++) {
            tmp_len = 16 + i * sizeof(gint16);   
            if (tmp_len + 1 >= BUFFER_MAX_LEN) { 
                debug_log(DLOG_ERROR, "Array out of bounds when getting eye diag. retimer_id = %d.", retimer_id);
                return;
            }
            debug_log(DLOG_ERROR, "%s: %d", fileld_name[i], *(gint16 *)(void *)&data_info[tmp_len]);
        }
    }
}


LOCAL void dft_get_retimer_temprature(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    gint16 temp_data = 0x4000;

    ret = retimer_get_temperature(obj_handle, &temp_data);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get temperature failed, object = %s.", dfl_get_object_name(obj_handle));
    }
    debug_log(DLOG_ERROR, "[%s] retimer temp_data = %d.", dfl_get_object_name(obj_handle), temp_data);
}


LOCAL gint32 retimer_get_dft_prbs_error(OBJ_HANDLE obj_handle)
{
    guint8 retimer_id = 0;

    gint8 data_info[BUFFER_MAX_LEN] = {};
    PRBS_ERROR_CHECK prbs_error_check;
    guint32 sum = 0;

    prbs_error_check.channel_id = 0x0;
    prbs_error_check.lane_num = RETIMER_LANE_NUM;

    gint32 ret = retimer_get_prbs_error(obj_handle, &prbs_error_check, data_info, BUFFER_MAX_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get_prbs_error second failed. ret = %d.", dfl_get_object_name(obj_handle), ret);
        return RET_ERR;
    }

    
    errno_t rst =
        memcpy_s(&(g_testResult.result_str[2]), sizeof(g_testResult.result_str) - 2, data_info, 32); 
    if (rst != EOK) {
        debug_log(DLOG_ERROR, "memcpy_s failed ret[%d]", rst);
        return RET_ERR;
    }

    for (guint8 i = 0x0; i < RETIMER_LANE_NUM; i++) {
        sum += *(guint32 *)(void *)&data_info[i * sizeof(guint32)];
    }

    if (sum != 0) {
        
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_RETIMER_ID, &retimer_id);
        g_testResult.result_str[1] = (gchar)retimer_id;
        debug_log(DLOG_ERROR, "Retimer %s get error failed.", dfl_get_object_name(obj_handle));

        dft_get_optical_module_temprature(obj_handle); 
        dft_get_eye(obj_handle);                       
        dft_get_retimer_temprature(obj_handle);        
    } else {
        g_testResult.result_str[1] = (gchar)0xff;
        debug_log(DLOG_INFO, "Retimer %s get error succeed.", dfl_get_object_name(obj_handle));
    }

    return RET_OK;
}


LOCAL void get_dft_result(void)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE obj_handle;

    ret = dfl_get_object_list(CLASS_RETIMER_NAME, &obj_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "dfl_get_object_list failed(ret: %d)", ret);
        return;
    }

    
    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        obj_handle = (OBJ_HANDLE)obj_node->data;

        ret = retimer_get_dft_prbs_error(obj_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: lane has error cnt. ret = %d.", dfl_get_object_name(obj_handle), ret);
            break;
        }
    }

    g_slist_free(obj_list);
    obj_list = NULL;
}


LOCAL void close_prbs(void)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE obj_handle;

    PRBS_SEND_RECV prbs_send_recv = {
        .channel_id = 0x0, 
        .lane_num = 0x8,   
        .prbs_type = 0x0,  
        .prbs_dir = 0x2    
    };

    ret = dfl_get_object_list(CLASS_RETIMER_NAME, &obj_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "dfl_get_object_list failed(ret: %d)", ret);
        return;
    }

    
    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        obj_handle = (OBJ_HANDLE)obj_node->data;

        ret = retimer_prbs_send_recv(obj_handle, &prbs_send_recv);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: prbs_send_recv close failed. ret = %d.", dfl_get_object_name(obj_handle), ret);
        }
        vos_task_delay(1000); 
    }

    g_slist_free(obj_list);
    obj_list = NULL;
}


LOCAL gint32 dft_adapt_prepare(OBJ_HANDLE obj_handle)
{
    gint32 ret;

    
    ret = retimer_prbs_optical_module_pwr(obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: optical module power off and on failed. ret = %d.", dfl_get_object_name(obj_handle),
            ret);
        return RET_ERR;
    }

    
    dft_get_optical_module_intl(obj_handle);

    
    dft_get_optical_module_temprature(obj_handle);

    
    dft_get_loss(obj_handle);

    
    dft_get_eye(obj_handle);

    
    dft_get_retimer_temprature(obj_handle);

    return RET_OK;
}


LOCAL gint32 dft_prbs_self_test(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    gint8 data_info[BUFFER_MAX_LEN] = {};

    PRBS_ERROR_CHECK prbs_error_check = {
        .channel_id = 0x0,
        .lane_num = RETIMER_LANE_NUM
    };

    PRBS_SEND_RECV prbs_send_recv = {
        .channel_id = 0x0, 
        .lane_num = 0x8,   
        .prbs_type = 0x8,  
        .prbs_dir = 0x2    
    };

    debug_log(DLOG_ERROR, "[%s] Come in self test.", dfl_get_object_name(obj_handle));

    
    ret = retimer_set_prbs_register(obj_handle, prbs_send_recv.channel_id, prbs_send_recv.lane_num);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set prbs register failed. ret = %d.", dfl_get_object_name(obj_handle), ret);
        return RET_ERR;
    }

    
    ret = retimer_prbs_send_recv(obj_handle, &prbs_send_recv);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: prbs_send_recv open failed. ret = %d.", dfl_get_object_name(obj_handle), ret);
        return RET_ERR;
    }
    vos_task_delay(2000); 

    
    ret = dft_adapt_prepare(obj_handle);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    
    ret = retimer_serdes_adapt(obj_handle, prbs_send_recv.channel_id, prbs_send_recv.lane_num);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: retimer serdes adapt failed. ret = %d.", dfl_get_object_name(obj_handle), ret);
        return RET_ERR;
    }
    vos_task_delay(4000); 

    
    ret = retimer_get_prbs_error(obj_handle, &prbs_error_check, data_info, BUFFER_MAX_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get_prbs_error first failed. ret = %d.", dfl_get_object_name(obj_handle), ret);
    }
    return ret;
}


LOCAL gint32 stop_retimer_routine(void)
{
    guint8 time_out = 0;

    write_retimer_routine_open_status(RETIMER_START_CLOSE_ROUTINE);
    while (1) {
        vos_task_delay(1000); 
        time_out++;
        if (time_out >= 30) { 
            debug_log(DLOG_ERROR, "Close routine time out");
            g_testResult.status = COMP_CODE_COMPLETE;
            g_testResult.result = TEST_NON;
            return RET_ERR;
        }

        if (read_retimer_routine_open_status() == RETIMER_ALREADY_CLOSE_ROUTINE) {
            return RET_OK;
        }
    }
}


LOCAL gint32 retimer_self_test(GSList *obj_list)
{
    gint32 ret;
    guint8 retimer_id = 0;
    GSList *obj_node = NULL;
    OBJ_HANDLE obj_handle;
    guint8 tmp = 0;

    
    ret = stop_retimer_routine();
    if (ret != RET_OK) {
        return RET_ERR;
    }
    retimer_stop_adapt();

    
    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        obj_handle = (OBJ_HANDLE)obj_node->data;

        if (dft_prbs_self_test(obj_handle) == RET_OK) {
            vos_task_delay(1000); 
        } else {
            debug_log(DLOG_ERROR, "%s do dft_prbs_self_test failed.", dfl_get_object_name(obj_handle));

            (void)dal_get_property_value_byte(obj_handle, PROPERTY_RETIMER_ID, &retimer_id);
            if (retimer_id < RETIMER_DEVICE_NUM) {
                tmp |= (1 << retimer_id);
            } else {
                debug_log(DLOG_ERROR, "Wrong Id. id = %d", retimer_id);
            }
            break;
        }
    }

    g_testResult.result_str[0] = (gchar)tmp;
    g_testResult.status = COMP_CODE_COMPLETE;
    if (obj_node == NULL) {
        g_testResult.result = TEST_SUCCEED;
        return RET_OK;
    } else {
        g_testResult.result = TEST_FAILED;
        return RET_ERR;
    }
}


LOCAL void retimer_dft_self_test(gpointer user_data)
{
    gint32 ret;
    guint8 software_type = 0;
    GSList *obj_list = NULL;

    do {
        
        if (dal_get_software_type(&software_type) != DFL_OK) {
            debug_log(DLOG_ERROR, "dal_get_software_type fail!");
            break;
        }

        if (software_type != MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
            debug_log(DLOG_ERROR, "Not smm! software_type = %d.", software_type);
            break;
        }

        ret = dfl_get_object_list(CLASS_RETIMER_NAME, &obj_list);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "dfl_get_object_list failed(ret: %d)", ret);
            break;
        }

        
        ret = retimer_self_test(obj_list);
        g_slist_free(obj_list);
        obj_list = NULL;
        if (ret != RET_OK) {
            g_taskId = 0;
            return;
        }

        while (1) {
            vos_task_delay(180000); 
            get_dft_result();
        }
    } while (0);

    g_testResult.status = COMP_CODE_COMPLETE;
    g_testResult.result = TEST_NON;
    g_taskId = 0;
}


LOCAL gint32 retimer_dft_test_adapter(guint32 command, gsize para_size, gconstpointer para_data, DFT_RESULT_S *dft_data,
    gpointer user_data)
{
    gint32 ret;

    switch (command) {
        case TEST_START:
            if (g_taskId != 0) {
                vos_task_delete(g_taskId);
                g_taskId = 0;
            }
            g_testResult.status = COMP_CODE_TESTING;
            g_testResult.result = TEST_NON;
            (void)memset_s(g_testResult.result_str, DFT_RESULT_STR_LEN, 0, DFT_RESULT_STR_LEN);
            ret = vos_task_create(&g_taskId, "RetimerSelfTest", (TASK_ENTRY)retimer_dft_self_test, NULL,
                DEFAULT_PRIORITY);
            if (ret != DFL_OK) {
                debug_log(DLOG_ERROR, "vos_task_create failed in retimer_dft_test_adapter.");
                g_testResult.status = COMP_CODE_COMPLETE;
                g_testResult.result = TEST_NON;
                return RET_ERR;
            }
            break;

        case TEST_STOP:
            close_prbs();
            break;

        case TEST_GET_RESULT:
            dft_data->status = g_testResult.status;
            dft_data->result = g_testResult.result;
            (void)memcpy_s(dft_data->result_str, DFT_RESULT_STR_LEN, g_testResult.result_str, DFT_RESULT_STR_LEN);
            break;

        default:
            return DFL_ERR;
    }

    return VOS_OK;
}


LOCAL gint32 dft_init(OBJ_HANDLE handle, gpointer user_data)
{
    gint32 ret;
    guint32 dft_id;
    guint32 slot_id;
    guint32 dev_id;

    
    ret = dal_get_property_value_uint32(handle, DFT_ID, &dft_id);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s get DFT_ID failed. ret = %d.", dfl_get_object_name(handle), ret);
        return ret;
    }
    
    if (dft_id == DFT_LSW_PORT_TEST) {
        ret = dal_get_property_value_uint32(handle, SLOT_ID, &slot_id);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s get SLOT_ID failed. ret = %d.", dfl_get_object_name(handle), ret);
            return ret;
        }

        ret = dal_get_property_value_uint32(handle, DEV_ID, &dev_id);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s dal_get_property_value_uint32 failed, prop_name:%s, ret:%d.",
                dfl_get_object_name(handle), DEV_ID, ret);
            return ret;
        }
        if (dev_id != 0x8) { 
            debug_log(DLOG_INFO, "Test dev_id is not ok. dev_id = %d", dev_id);
            return RET_ERR;
        }

        debug_log(DLOG_ERROR, "Test dfl_register_dft_item %d, %d, %d", dft_id, slot_id, dev_id);
        ret = dfl_register_dft_item(dft_id, slot_id, dev_id, retimer_dft_test_adapter, NULL);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Retimer do dfl_register_dft_item failed.");
            return ret;
        }
    }

    return RET_OK;
}


void retimer_dft_init(void)
{
    gint32 ret;

    
    ret = dfl_foreach_object(DFT_ITEM_CLASS, dft_init, NULL, NULL);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "Retimer dfl_foreach_object failed.");
    }

    return;
}
