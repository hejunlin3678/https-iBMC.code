


#include "pme_app/pme_app.h"


#include "card_ipmi_msg.h"

#include "pme_app/common/debug_log_macro.h"

gint32 card_mgmt_send_smm_ipmi_msg(const void *data, guint8 data_len, guint8 retry)
{
    gpointer resp_ipmi = NULL;
    gint32 ret = RET_OK;
    const guint8 *resp_data_buf = NULL;
    IPMI_REQ_MSG_HEAD_S req_header = {};

    if (data == NULL) {
        debug_log(DLOG_ERROR, "data msg is NULL");
        return RET_ERR;
    }

    req_header.target_type = IPMI_SMM;
    req_header.target_instance = 0x00;
    req_header.lun = 0x00;
    req_header.netfn = NETFN_OEM_REQ;
    req_header.cmd = 0x92;
    req_header.src_len = data_len;

    debug_log(DLOG_DEBUG, "[%s] send_smm_ipmi_msg start, retry %d times", __FUNCTION__, retry);

    do {
        ret = ipmi_send_request(&req_header, data, 0, &resp_ipmi, TRUE);
        if (ret == RET_OK) {
            resp_data_buf = get_ipmi_src_data((gconstpointer)resp_ipmi);
            if ((resp_data_buf != NULL) && ((resp_data_buf[0]) == COMP_CODE_SUCCESS)) {
                debug_log(DLOG_DEBUG, "[%s] smm response is OK [%d]", __FUNCTION__, resp_data_buf[0]);
                g_free(resp_ipmi);
                return RET_OK;
            } else {
                debug_log(DLOG_ERROR, "[%s] smm response failed", __FUNCTION__);
                g_free(resp_ipmi);
                resp_ipmi = NULL;
            }
        } else {
            debug_log(DLOG_ERROR, "[%s] failed to send ipmi to smm, result is [%d]", __FUNCTION__, ret);
        }

        retry--;
        vos_task_delay(50);
    } while (retry > 0);

    debug_log(DLOG_ERROR, "[%s] failed to send ipmi to smm, quit...", __FUNCTION__);
    return ret;
}


gint32 card_mgmt_get_smm_ras_status(const void *data, guint8 data_len, guint8 *response, gsize resp_size, guint8 retry)
{
    gint32 ret = RET_OK;
    gpointer resp_ipmi = NULL;
    const guint8 *resp_data_buf = NULL;
    IPMI_REQ_MSG_HEAD_S req_header = {};

    if (data == NULL) {
        debug_log(DLOG_ERROR, "data msg is NULL");
        return RET_ERR;
    }

    req_header.target_type = IPMI_SMM;
    req_header.target_instance = 0x00;
    req_header.lun = 0x00;
    req_header.netfn = NETFN_OEM_REQ;
    req_header.cmd = 0x92;
    req_header.src_len = data_len;

    debug_log(DLOG_ERROR, "[%s]send_smm_ipmi_msg start, retry %d times", __FUNCTION__, retry);

    do {
        ret = ipmi_send_request(&req_header, data, resp_size + 4, &resp_ipmi, TRUE);
        if (ret == RET_OK) {
            resp_data_buf = get_ipmi_src_data((gconstpointer)resp_ipmi);
            if ((resp_data_buf != NULL) && ((resp_data_buf[0]) == COMP_CODE_SUCCESS)) {
                for (gsize part_index = 0; part_index < resp_size; part_index++) {
                    response[part_index] = resp_data_buf[4 + part_index];
                }
                g_free(resp_ipmi);
                return RET_OK;
            } else {
                debug_log(DLOG_ERROR, "[%s] smm response failed", __FUNCTION__);
                g_free(resp_ipmi);
                resp_ipmi = NULL;
            }
        } else {
            debug_log(DLOG_ERROR, "[%s]send ipmi to smm failed, result is [%d]", __FUNCTION__, ret);
        }

        retry--;
        vos_task_delay(50);
    } while (retry > 0);

    debug_log(DLOG_ERROR, "[%s]Failed to send ipmi to smm, quit...", __FUNCTION__);
    return ret;
}


gint32 card_manage_send_ipmi_request(IPMI_REQ_MSG_HEAD_S *header, gconstpointer req_data, gsize resp_size,
    gpointer *response, gboolean wait_response, gint32 channel_type)
{
    IPMI_MSG_S *ipmi_msg = NULL;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = RET_ERR;
    guint32 resp_len = 0;
    GSList *list = NULL;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gsize parm_temp;

    if ((header->target_type != IPMI_ME)) { 
        ret = ipmi_send_request(header, req_data, resp_size, response, wait_response);
    } else { 
        ret = dfl_get_object_list(CLASS_CPU, &list);
        
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "[%s] Get Cpu object handle failed!%d", __FUNCTION__, ret);
            return RET_ERR;
        }
        
        
        obj_handle = (OBJ_HANDLE)g_slist_nth_data(list, 0);
        g_slist_free(list);
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_int32(wait_response));
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_uint32(resp_size));
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, header,
            sizeof(IPMI_REQ_MSG_HEAD_S), sizeof(guint8)));
        input_list = g_slist_append(input_list,
            (gpointer)g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, req_data, header->src_len, sizeof(guint8)));
        
        
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_int32(channel_type));
        
        
        ret = dfl_call_class_method(obj_handle, METHOD_CPU_GET_INFO, NULL, input_list, &output_list);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        
        
        if (ret != DFL_OK || output_list == NULL) {
            debug_log(DLOG_ERROR, "[%s] failed with result;%d", __FUNCTION__, ret);
            return RET_ERR;
        }
        

        ret = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
        if (ret == RET_OK) {
            resp_len = g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, 1));
            *response = g_malloc0(sizeof(IPMI_MSG_S));
            if (*response == NULL) {
                debug_log(DLOG_ERROR, "%s : g_malloc0 failed for size(%" G_GSIZE_FORMAT ").", __FUNCTION__,
                    IPMI_MSG_HEAD_LEN + resp_size);
                g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
                return RET_ERR;
            }
            
            ipmi_msg = (IPMI_MSG_S *)(*response);
            ipmi_msg->data_len = resp_len; // 实际的返回长度,赋值，应用可能要用
            const void *resp_msg =
                g_variant_get_fixed_array((GVariant *)g_slist_nth_data(output_list, 2), &parm_temp, sizeof(guint8));
            resp_len = (guint32)parm_temp;
            if (resp_msg == NULL) {
                debug_log(DLOG_ERROR, "%s:resp_msg is NULL.", __FUNCTION__);
                g_free(*response);
                *response = NULL;
                g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
                return RET_ERR;
            }

            errno_t iRet = memcpy_s((void *)(ipmi_msg->payload.ipmi_data.src_data),
                sizeof(ipmi_msg->payload.ipmi_data.src_data), resp_msg, resp_len);
            if (iRet != EOK) {
                debug_log(DLOG_ERROR, "%s: resp_msg memcpy_s fail, iRet = %d", __FUNCTION__, iRet);
            }
        }
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    }
    return ret;
}
