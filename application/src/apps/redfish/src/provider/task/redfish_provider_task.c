

#include "redfish_provider.h"

#include "redfish_task.h"

#include "json_patch_util.h"


gboolean check_user_priv_for_query_task(PROVIDER_PARAS_S *i_paras, guint8 task_need_priv, const gchar *task_owner)
{
    gint32 ret;
    guint8 configure_self_priv = DISABLED;

    if (i_paras == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return FALSE;
    }

    if ((i_paras->user_role_privilege & task_need_priv) != 0) {
        return TRUE;
    }

    
    if (task_owner == NULL || strlen(task_owner) == 0) {
        return FALSE;
    }

    if (g_strcmp0(task_owner, (const gchar *)i_paras->user_name) != 0) {
        return FALSE;
    }

    ret = dal_get_user_role_priv_enable(task_owner, PROPERTY_USERROLE_CONFIGURE_SELF, &configure_self_priv);
    if (ret == VOS_OK && configure_self_priv == ENABLED) {
        return TRUE;
    } else {
        return FALSE;
    }
}


gint32 get_task_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    gint task_id;
    json_object *task_rsc_jso = NULL;
    gint32 ret;
    guint8 task_need_priv = 0;
    const gchar *task_owner = NULL;

    if (o_rsc_jso == NULL || o_err_array_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RF_FAILED;
    }

    ret = vos_str2int(i_paras->member_id, 10, &task_id, NUM_TPYE_INT32);
    check_fail_do_return_val(VOS_OK == ret, RF_FAILED,
        debug_log(DLOG_ERROR, "get task id failed, id is %s", i_paras->member_id));

    task_rsc_jso = get_task_rsc_from_id(task_id, &task_need_priv, &task_owner);
    check_fail_do_return_val(task_rsc_jso, RF_RSC_NOT_FOUND, debug_log(DLOG_ERROR, "get task %d failed", task_id));

    if (check_user_priv_for_query_task(i_paras, task_need_priv, task_owner) == FALSE) {
        json_object_put(task_rsc_jso);
        return HTTP_FORBIDDEN;
    }

    debug_log(DLOG_INFO, "orignal task rsc:%s", dal_json_object_get_str(task_rsc_jso));

    
    json_object_set_object(o_rsc_jso, task_rsc_jso);

    (void)json_object_put(task_rsc_jso);

    return RF_OK;
}


gint32 get_task_collection_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    int iRet = -1;
    json_object *task_member_jso = NULL;
    int i = 0;
    GSList *cur_task_list = NULL;
    GSList *task_iter = NULL;
    char             task_rsc_path[MAX_URI_LENGTH] = {0};
    int task_id = 0;
    int ret;
    json_object *task_rsc_jso = NULL;
    json_object *odata_id_jso = NULL;

    if (o_rsc_jso == NULL || o_err_array_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RF_FAILED;
    }

    task_member_jso = json_object_new_array();
    return_val_do_info_if_fail(task_member_jso, RF_FAILED, debug_log(DLOG_ERROR, "alloc a new json array failed"));

    
    cur_task_list = get_cur_task_list();
    for (task_iter = cur_task_list; task_iter; task_iter = task_iter->next) {
        task_id = (intptr_t)task_iter->data;

        iRet = snprintf_s(task_rsc_path, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "%s%d", TASK_COLLECTION_PATH, task_id);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

        odata_id_jso = json_object_new_object();
        break_do_info_if_expr(odata_id_jso == NULL, debug_log(DLOG_ERROR, "alloc a new json object failed"));
        task_rsc_jso = json_object_new_string((const gchar *)task_rsc_path);
        json_object_object_add(odata_id_jso, ODATA_ID, task_rsc_jso);

        ret = json_object_array_add(task_member_jso, odata_id_jso);
        check_fail_do(0 == ret, (void)json_object_put(odata_id_jso));

        i++;
    }

    g_slist_free(cur_task_list);

    
    json_object_object_add(o_rsc_jso, RFOBJ_MEMBERS_COUNT, json_object_new_int(i));
    json_object_object_add(o_rsc_jso, RFOBJ_MEMBERS, task_member_jso);

    return RF_OK;
}

LOCAL PROPERTY_PROVIDER_S g_task_rsc_provider[] = {
};


gint32 task_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint task_id;
    gint32 ret;
    json_object *task_rsc_jso = NULL;

    ret = vos_str2int(i_paras->member_id, 10, &task_id, NUM_TPYE_INT32);
    check_fail_do_return_val(VOS_OK == ret, RF_FAILED,
        debug_log(DLOG_ERROR, "get task id failed, id is %s", i_paras->member_id));

    
    task_rsc_jso = get_task_rsc_from_id(task_id, NULL, NULL);
    check_fail_do_return_val(NULL != task_rsc_jso, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "get task with id %d failed", task_id));

    (void)json_object_put(task_rsc_jso);

    *prop_provider = g_task_rsc_provider;
    
    *count = 0;
    ;

    return VOS_OK;
}

LOCAL PROPERTY_PROVIDER_S g_task_collection_rsc[] = {
};


gint32 task_collection_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    
    return_val_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), HTTP_INTERNAL_SERVER_ERROR);

    *prop_provider = g_task_collection_rsc;

    
    *count = 0;
    ;

    return VOS_OK;
}

LOCAL PROPERTY_PROVIDER_S task_svc_provider[] = {
    {
        RF_DATETIME,
        "", "\0",
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        NULL,
        NULL,
        NULL, ETAG_FLAG_DISABLE
    }
};


gint32 task_svc_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    
    return_val_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), HTTP_INTERNAL_SERVER_ERROR);

    *prop_provider = task_svc_provider;
    *count = sizeof(task_svc_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}


gint32 get_task_svc_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    gint32 ret;
    json_object *single_err_jso = NULL;
    json_object *date_time_jso = NULL;
    int obj_add_ret;

    
    ret = get_manager_datetime(i_paras, &single_err_jso, &date_time_jso);
    if (single_err_jso) {
        obj_add_ret = json_object_array_add(o_err_array_jso, single_err_jso);
        check_fail_do(0 == obj_add_ret, (void)json_object_put(single_err_jso));

        debug_log(DLOG_ERROR, "get task service date failed, ret is %d", ret);
        return ret;
    }

    check_fail_do_return_val(HTTP_OK == ret, ret,
        debug_log(DLOG_ERROR, "get task service date failed, ret is %d", ret));

    json_object_object_add(o_rsc_jso, RF_DATETIME, date_time_jso);

    return RF_OK;
}
