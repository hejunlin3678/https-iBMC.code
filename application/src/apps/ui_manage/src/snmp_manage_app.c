
#include "snmp_manage_app.h"
#include "keyGen.h"
#include "pme_app/kmc/kmc.h"
#include "web_manage.h"
#include "snmp_method.h"

G_LOCK_EXTERN(g_web_service_lock);

#define SNMP_SYSTEM_CMD_LEN 15
#define DECIMAL_BASE 10

GSList *g_revert_owner_list = NULL;

GAsyncQueue *g_snmpSendQueue;
GMutex g_snmp_mutex;
GMutex g_snmpcomm_mutex;
guchar g_https_enable_flag = SERVICE_START;
guchar g_snmpd_enable_flag = SERVICE_START;
guint8 g_rollback_state = 0;
guint8 g_snmpcomm_update_state = 1;
volatile gboolean g_rebootSnmp = FALSE;
guint8 g_snmpv3_state = SNMP_STATE_ENABLE;
gint32 get_system_service_config(void);
LOCAL void enable_disable_snmpd(void);
LOCAL gint32 check_user_snmp_enable(const gchar *user_name, gboolean *user_snmp_enable);
LOCAL gint32 snmp_add_username_to_file(const char *hex_name, const char *buf);
LOCAL gint32 snmp_port_init(OBJ_HANDLE obj_handle);
LOCAL gint32 snmp_get_oldengineid(gchar *oldengineid, gint32 engine_size);


G_LOCK_DEFINE_STATIC(g_third_party_proc_hashtable);
LOCAL GHashTable *g_third_party_proc_hashtable = NULL; // 用于保存第三方进程关键字及进程号信息


LOCAL void third_party_proc_hashtable_init(void)
{
    G_LOCK(g_third_party_proc_hashtable);

    if (g_third_party_proc_hashtable == NULL) {
        g_third_party_proc_hashtable = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, (GDestroyNotify)g_free);
    }

    G_UNLOCK(g_third_party_proc_hashtable);
}

gint32 third_party_proc_hashtable_insert(gchar *proc_kw, guint32 pid)
{
    guint32 *proc_id = NULL;
    gchar *proc_kw_temp = NULL;

    G_LOCK(g_third_party_proc_hashtable);
    if (proc_kw == NULL || g_third_party_proc_hashtable == NULL) {
        debug_log(DLOG_ERROR, "input param is null or hashtable is null");
        G_UNLOCK(g_third_party_proc_hashtable);
        return RET_ERR;
    }

    proc_id = (guint32 *)g_malloc0(sizeof(guint32));
    if (proc_id == NULL) {
        debug_log(DLOG_ERROR, "%s :g_malloc0 fail", __FUNCTION__);
        G_UNLOCK(g_third_party_proc_hashtable);
        return RET_ERR;
    }

    *proc_id = pid;

    proc_kw_temp = g_strdup(proc_kw);
    if (proc_kw_temp == NULL) {
        debug_log(DLOG_ERROR, "%s :g_strdup fail", __FUNCTION__);
        G_UNLOCK(g_third_party_proc_hashtable);
        g_free(proc_id);
        return RET_ERR;
    }

    g_hash_table_insert(g_third_party_proc_hashtable, (gpointer)proc_kw_temp, (gpointer)proc_id);

    G_UNLOCK(g_third_party_proc_hashtable);
    return RET_OK;
}

gint32 third_party_proc_hashtable_remove(const gchar *proc_kw)
{
    gint32 ret = RET_OK;

    G_LOCK(g_third_party_proc_hashtable);
    if (proc_kw == NULL || g_third_party_proc_hashtable == NULL) {
        debug_log(DLOG_ERROR, "input param is null or hashtable is null");
        G_UNLOCK(g_third_party_proc_hashtable);
        return RET_ERR;
    }

    if (!g_hash_table_remove(g_third_party_proc_hashtable, (gconstpointer)proc_kw)) {
        ret = RET_ERR;
    }

    G_UNLOCK(g_third_party_proc_hashtable);
    return ret;
}

gint32 third_party_proc_check_exist(const gchar *proc_kw)
{
    guint32 *proc_id = NULL;
    gint32 ret;
    guint32 proc_id_temp;

    G_LOCK(g_third_party_proc_hashtable);
    if (proc_kw == NULL || g_third_party_proc_hashtable == NULL) {
        debug_log(DLOG_ERROR, "input param is null or hashtable is null");
        G_UNLOCK(g_third_party_proc_hashtable);
        return RET_ERR;
    }

    proc_id = (guint32 *)g_hash_table_lookup(g_third_party_proc_hashtable, (gconstpointer)proc_kw);
    if (proc_id == NULL) {
        G_UNLOCK(g_third_party_proc_hashtable);
        goto error_exit;
    } else {
        proc_id_temp = *proc_id;
    }

    G_UNLOCK(g_third_party_proc_hashtable);

    // 进程号非默认值,且能够接收空信号则认为进程存在
    if (kill(proc_id_temp, 0) == 0) {
        goto normal_exit;
    }

    // 如果进程号不存在，则从hash表中移除
    ret = third_party_proc_hashtable_remove(proc_kw);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s :remove %s fail", __FUNCTION__, proc_kw);
    }

error_exit:
    return RET_ERR;

normal_exit:
    return RET_OK;
}

LOCAL void snmp_shield_oldengineid(void)
{
    gint32 iRet = -1;
    FM_FILE_S *fp = NULL;
    gchar key[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gchar buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gchar new_buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };

    (void)snprintf_s(key, sizeof(key), sizeof(key) - 1, "%s", "oldEngineID");

    g_mutex_lock(&g_snmp_mutex);
    fp = fm_fopen(SNMP_SHORT_FILE_NAME, "r+");
    if (fp == NULL) {
        g_mutex_unlock(&g_snmp_mutex);
        debug_log(DLOG_ERROR, "%s:%d:fail to open %s\n", __FUNCTION__, __LINE__, SNMP_SHORT_FILE_NAME);
        return;
    }

    gint32 p_start = fm_ftell(fp);
    gchar *old_eng = NULL;
    
    while (fm_fgets(buf, 1 + SNMP_CONF_MAXLINE_LEN, fp) != NULL) {
        old_eng = strstr(buf, "#oldEngineI");
        if (old_eng != NULL) {
            errno_t safe_fun_ret = strncpy_s(new_buf, sizeof(new_buf), old_eng, sizeof(new_buf) - 1);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s, strncpy_s fail, securec_rv = %d.", __FUNCTION__, safe_fun_ret);
            }
            iRet = fm_modify(new_buf, (gsize)strlen(new_buf), (gsize)p_start, (gsize)strlen(buf), fp);
            if (iRet != RET_OK) {
                debug_log(DLOG_ERROR, "Call fm_modify failed!\n");
            }
            fm_fclose(fp);
            g_mutex_unlock(&g_snmp_mutex);
            return;
        }
        if (strstr(buf, key) != NULL) {
            
            iRet = snprintf_s(new_buf, sizeof(new_buf), sizeof(new_buf) - 1, "%c%s", '#', buf);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
            }
            iRet = fm_modify(new_buf, (gsize)strlen(new_buf), (gsize)p_start, (gsize)strlen(buf), fp);
            if (iRet != RET_OK) {
                debug_log(DLOG_ERROR, "Call fm_modify failed!\n");
            }
            fm_fclose(fp);
            g_mutex_unlock(&g_snmp_mutex);
            return;
        }

        p_start = fm_ftell(fp);
    }

    
    fm_fclose(fp);
    g_mutex_unlock(&g_snmp_mutex);
    return;
}

LOCAL gint32 snmp_modify_oldengineid(const gchar *new_oldengineid)
{
    gint32 iRet = -1;
    gint32 ret = 0;
    FM_FILE_S *fp = NULL;
    gint32 p_start = 0;
    gint32 old_buf_len = 0;
    gint32 new_buf_len = 0;
    gchar key[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gchar buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };

    if (new_oldengineid == NULL) {
        return RET_ERR;
    }
    
    (void)snprintf_s(key, sizeof(key), sizeof(key) - 1, "%s", "oldEngineID");
    

    g_mutex_lock(&g_snmp_mutex);
    fp = fm_fopen(SNMP_SHORT_FILE_NAME, "r+");
    if (fp == NULL) {
        g_mutex_unlock(&g_snmp_mutex);
        debug_log(DLOG_ERROR, "%s:%d:fail to open %s\n", __FUNCTION__, __LINE__, SNMP_SHORT_FILE_NAME);
        return RET_ERR;
    }

    p_start = fm_ftell(fp);

    
    while (fm_fgets(buf, 1 + SNMP_CONF_MAXLINE_LEN, fp) != NULL) {
        if (strstr(buf, key) != NULL) {
            old_buf_len = strlen(buf); // 用户信息长度

            
            (void)memset_s(buf, sizeof(buf), 0, sizeof(buf));
            
            iRet = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "%s %s\n", key, new_oldengineid);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
            }
            
            new_buf_len = strlen(buf);
            ret = fm_modify(buf, (gsize)new_buf_len, (gsize)p_start, (gsize)old_buf_len, fp);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "Call fm_modify failed!\n");
                fm_fclose(fp);
                g_mutex_unlock(&g_snmp_mutex);
                return RET_ERR;
            }

            fm_fclose(fp);
            g_mutex_unlock(&g_snmp_mutex);
            return RET_OK;
        }

        p_start = fm_ftell(fp);
    }

    
    ret = feof(fp->fd);
    if (ret != 0) {
        
        
        (void)memset_s(buf, sizeof(buf), 0, sizeof(buf));
        
        iRet = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "%s %s\n", key, new_oldengineid);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
        }
        
        new_buf_len = strlen(buf);
        ret = fm_insert(buf, (gsize)new_buf_len, p_start, fp);
        if (ret == RET_OK) {
            fm_fclose(fp);
            g_mutex_unlock(&g_snmp_mutex);
            return RET_OK;
        }

        debug_log(DLOG_ERROR, "Call fm_insert failed!\n");
    }

    

    
    fm_fclose(fp);
    g_mutex_unlock(&g_snmp_mutex);
    return RET_ERR;
}

LOCAL gint32 snmp_get_oldengineid(gchar *oldengineid, gint32 engine_size)
{
    FM_FILE_S *fp = NULL;
    gchar *pointer = NULL;
    gchar *new_buff_pointer = NULL;
    gchar conf_buff[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };

    if (oldengineid == NULL) {
        return RET_ERR;
    }

    g_mutex_lock(&g_snmp_mutex);
    fp = fm_fopen(SNMP_SHORT_FILE_NAME, "r");
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "Open snmpd.conf file failed\n");
        g_mutex_unlock(&g_snmp_mutex);
        return RET_ERR;
    }

    while (fm_fgets(conf_buff, SNMP_CONF_MAXLINE_LEN + 1, fp) != NULL) {
        if (strstr(conf_buff, "oldEngineID") == NULL) {
            continue;
        }

        dal_trim_string(conf_buff, SNMP_CONF_MAXLINE_LEN + 1);

        if (strtok_r(conf_buff, " ", &new_buff_pointer) != NULL) {
            if ((pointer = strtok_r(new_buff_pointer, " ", &new_buff_pointer)) != NULL) {
                
                if (strncpy_s(oldengineid, engine_size, pointer, strlen(pointer)) != EOK) {
                    debug_log(DLOG_ERROR, "%s %d: strncpy_s fail!", __FUNCTION__, __LINE__);
                }
                
            }
        }

        fm_fclose(fp);
        g_mutex_unlock(&g_snmp_mutex);
        return RET_OK;
    }

    fm_fclose(fp);
    g_mutex_unlock(&g_snmp_mutex);
    return RET_OK;
}

LOCAL gint32 snmp_get_newoldengineid(gchar *new_oldengineid, gint32 engineid_size)
{
    FILE *fp = NULL;
    gint32 ret = 0;
    gchar *pointer = NULL;
    gchar *new_buff_pointer = NULL;
    gchar conf_buff[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
#define SNMP_ENGINE_ID_LENGTH 28 // oldEngineID长度(固定长度28个字符)

    if (new_oldengineid == NULL) {
        return RET_ERR;
    }

    
    ret = vos_get_file_accessible("/var/net-snmp/snmpd.conf");
    if (ret == FALSE) {
        debug_log(DLOG_DEBUG, "Snmpd.conf file is not exist\n");
        return RET_ERR;
    }

    fp = g_fopen("/var/net-snmp/snmpd.conf", "r");
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "Open snmpd.conf file failed\n");
        return RET_ERR;
    }

    while (vos_fgets_s(conf_buff, SNMP_CONF_MAXLINE_LEN + 1, fp) != NULL) {
        if (strstr(conf_buff, "oldEngineID") == NULL) {
            continue;
        }

        dal_trim_string(conf_buff, SNMP_CONF_MAXLINE_LEN + 1);

        if (strtok_r(conf_buff, " ", &new_buff_pointer) != NULL) {
            if ((pointer = strtok_r(new_buff_pointer, " ", &new_buff_pointer)) != NULL) {
                
                ret = strncpy_s(new_oldengineid, engineid_size, pointer, strlen(pointer));
                if (ret != EOK) {
                    debug_log(DLOG_ERROR, "%s %d: strncpy_s fail!", __FUNCTION__, __LINE__);
                }
                
            }
        }

        
        // 通过长度来判断是否正确
        if (strlen(new_oldengineid) == SNMP_ENGINE_ID_LENGTH) {
            (void)fclose(fp);
            return RET_OK;
        }

        
    }

    (void)fclose(fp);
    return RET_ERR;
}

LOCAL gint32 snmp_get_outter_eth_group_mac(guint32 outtype, gchar *eth_mac, guint32 eth_mac_len)
{
    errno_t safe_fun_ret = EOK;
    gint32 retv = RET_OK;
    guint32 mac_buf_len = 0;
    const gchar *mac_buf = NULL;
    GVariant *property_value = NULL;
    OBJ_HANDLE group_obj = 0;
    gchar tmp_mac[MAX_MAC_STR_LEN] = { 0 };
    guint32 i = 0;
    guint32 n = 0;
    gsize parm_temp = 0;

    // 入参检查
    if ((eth_mac == NULL) || (eth_mac_len < MAX_MAC_STR_LEN) || (outtype == 0)) {
        debug_log(DLOG_ERROR, "%s invalid pram: eth_mac=%s, eth_mac_len=%u, outtype=%u\n", __func__, eth_mac,
            eth_mac_len, outtype);
        return RET_ERR;
    }

    // 获取外出网口对象
    property_value = g_variant_new_byte(outtype);
    retv = dfl_get_specific_object(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_OUT_TYPE, property_value, &group_obj);
    if (property_value != NULL) {
        g_variant_unref(property_value);
        property_value = NULL;
    }

    if (retv != DFL_OK) {
        debug_log(DLOG_MASS, "%s:dfl_get_specific_object failed: ethgroup outype=%d\n", ETH_CLASS_NAME_ETHGROUP,
            outtype);
        return RET_ERR;
    }

    
    retv = dfl_get_property_value(group_obj, ETH_GROUP_ATTRIBUTE_MAC, &property_value);
    if (retv != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:dfl_get_property_value failed:obj_name is %s\n", __func__,
            dfl_get_object_name(group_obj));
        return RET_ERR;
    }

    mac_buf = g_variant_get_string(property_value, &parm_temp);
    mac_buf_len = (guint32)parm_temp;
    if (mac_buf_len >= MAX_MAC_STR_LEN) {
        mac_buf_len = MAX_MAC_STR_LEN - 1;
    }

    safe_fun_ret = memcpy_s(tmp_mac, sizeof(tmp_mac), mac_buf, mac_buf_len);
    if (safe_fun_ret != EOK) {
        g_variant_unref(property_value);
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    tmp_mac[mac_buf_len] = 0;

    if (property_value != NULL) {
        g_variant_unref(property_value);
    }

    // 去掉mac的分隔符:
    for (i = 0; i < mac_buf_len; i++) {
        if (tmp_mac[i] != ':') {
            eth_mac[n++] = tmp_mac[i];
        }
    }

    eth_mac[n] = 0;

    return RET_OK;
}

LOCAL gint32 snmp_check_mac_in_engineid(const gchar *engineid)
{
    
    
    gint32 ret = 0;
    gchar mac[MAX_MAC_STR_LEN] = { 0 };
    OBJ_HANDLE group_obj = 0;
    OBJ_HANDLE snmp_obj = 0;
    guchar is_update_engineID = 0;

    ret = dal_get_object_handle_nth(CLASS_SNMP, 0, &snmp_obj);
    // 获取SNMP对象失败的异常情况，返回OK，不更新
    if (ret != RET_OK) {
        return RET_OK;
    }

    ret = dal_get_property_value_byte(snmp_obj, PROPERTY_SNMP_IS_UPDATE_ENGINEID, &is_update_engineID);
    // 获取SNMP IsMacChanged属性失败的异常情况，返回OK，不更新
    if (ret != RET_OK) {
        return RET_OK;
    }

    
    if (!is_update_engineID) {
        return RET_OK;
    }

    // 获取外出网口mac, 读取mac失败, 是异常情况，此时不进行更新

    
    // Atlas产品的外出网口类型为 OUT_ETHERNET_FOR_ATLAS
    if (dal_get_specific_object_byte(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_OUT_TYPE, OUT_ETHERNET_FOR_ATLAS,
        &group_obj) == RET_OK) {
        (void)memset_s(mac, MAX_MAC_STR_LEN, 0, MAX_MAC_STR_LEN);

        if (snmp_get_outter_eth_group_mac(OUT_ETHERNET_FOR_ATLAS, mac, sizeof(mac)) != RET_OK) {
            debug_log(DLOG_MASS, "%s: snmp_get_outter_eth_group_mac failed, ignore it. OutType is %d\n", __func__,
                OUT_ETHERNET_FOR_ATLAS);
            return RET_OK;
        }

        if (strstr(engineid, mac) != NULL) {
            return RET_OK;
        }
    }
    
    // Atlas之外的产品，外出网口类型为 OUT_ETHERNET
    else {
        (void)memset_s(mac, MAX_MAC_STR_LEN, 0, MAX_MAC_STR_LEN);

        if (snmp_get_outter_eth_group_mac(OUT_ETHERNET, mac, sizeof(mac)) != RET_OK) {
            debug_log(DLOG_ERROR, "%s: snmp_get_outter_eth_group_mac failed, ignore it, OutType is %d\n", __func__,
                OUT_ETHERNET);

            return RET_OK;
        }

        // engineid中的mac与外出网口一致，返回ok; 这里面的字符串大小写必须是一致的.
        if (strstr(engineid, mac) != NULL) {
            return RET_OK;
        }
    }
    
    
    return RET_ERR;
}

LOCAL void snmp_update_oldengineid(void)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret = 0;
    guint8 snmpd_status = 0;
    const gchar *oldengineid = NULL;
    gchar old_engineid[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gchar new_oldengineid[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_value = NULL;

    
    ret = dfl_get_object_handle(OBJ_NAME_PME_SERVICECONFIG, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get pme service config handle failed!");
        return;
    }

    ret = dfl_get_property_value(obj_handle, PROTERY_PME_SERVICECONFIG_SNMPD, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get snmpd service status failed!");
        return;
    }

    snmpd_status = g_variant_get_byte(property_value);
    g_variant_unref(property_value);
    property_value = NULL;

    
    if (snmpd_status == 0) {
        return;
    }

    ret = dfl_get_object_handle(OBJ_NAME_SNMP, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get SNMP handle failed!");
        return;
    }

    //
    // 1、如果/var/net-snmp/snmpd.conf的engineid中的mac地址与网卡mac不同，重新生成
    //
    
    ret = snmp_get_newoldengineid(new_oldengineid, sizeof(new_oldengineid));
    if ((ret != RET_OK) || (strlen(new_oldengineid) == 0)) {
        debug_log(DLOG_ERROR, "Get SNMP new oldengineid failed!");
        return;
    }

    
    if (snmp_check_mac_in_engineid(new_oldengineid) != RET_OK) {
        // 重新生成engine id(先屏蔽旧引擎ID, 然后重启snmpd)
        debug_log(DLOG_ERROR, "%s: generate new MAC EngineID for snmp!", __FUNCTION__);
        
        snmp_shield_oldengineid();
        vos_task_delay(1000);

        
        ret = kill_process_by_name(SNMP_PROC_NAME);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: Kill snmpd failed!\r\n", __FUNCTION__);
        }

        
        
        // 这里先删除oldEngineID(直接删除文件和单独删除oldEngineID这行效果相同)，否则不会重新生成engineid
        gchar* cmd[] = {"/bin/sh", "-c", "/bin/echo > /var/net-snmp/snmpd.conf", NULL};
        (void)vos_system_s("/bin/sh", cmd);
        

        ret = snmp_start_app_by_name(SNMP_PROC_NAME);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: Start snmpd failed!\r\n", __FUNCTION__);
        }
    }

    //
    // 2、如果/var/net-snmp/snmpd.conf的engineid与/etc/snmp/snmpd.conf中的不同，进行同步
    //
    
    ret = dfl_get_property_value(obj_handle, PROPERTY_SNMP_OLDENGINEID, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get SNMP oldengine id failed!");
        return;
    }

    oldengineid = g_variant_get_string(property_value, 0);
    if (oldengineid == NULL) {
        debug_log(DLOG_ERROR, "g_variant_get_string() failed!\n");
        g_variant_unref(property_value);
        return;
    }

    
    safe_fun_ret = strncpy_s(old_engineid, sizeof(old_engineid), oldengineid, sizeof(old_engineid) - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    
    g_variant_unref(property_value);
    property_value = NULL;

    
    if (strcmp(old_engineid, new_oldengineid) == 0) {
        debug_log(DLOG_DEBUG, "SNMP /var/net-snmp/snmpd.conf engine id is same to /etc/snmp/snmpd.conf!");
        return;
    }

    
    ret = snmp_modify_oldengineid(new_oldengineid);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Modify SNMP oldengineid failed!");
        return;
    }

    
    property_value = g_variant_new_string(new_oldengineid);
    ret = dfl_set_property_value(obj_handle, PROPERTY_SNMP_OLDENGINEID, property_value, DF_SAVE);
    g_variant_unref(property_value);

    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set SNMP oldengineid failed!");
        return;
    }

    return;
}

gint32 kill_process_by_pid(gchar *pid, guint8 pid_len)
{
    gchar* cmdstring[5] = {"kill", "-9", pid, NULL};
    gint32 ret;
    sig_t old_handler;

    if (pid == NULL || pid_len == 0) {
        debug_log(DLOG_ERROR, "Input NULL pointer!\n");
        return RET_ERR;
    }

    
    
    old_handler = signal(SIGCHLD, SIG_DFL);
    ret = vos_system_s("/bin/kill", cmdstring);
    (void)signal(SIGCHLD, old_handler);

    
    if (ret != 0) {
        debug_log(DLOG_ERROR, "vos_system_s fail!\n");
        return RET_ERR;
    }

    return RET_OK;
}

gint32 kill_process_by_name(gchar *app_name)
{
    gint32 ret = 0;
    gint32 count = 0;
    gchar pid[SNMP_PID_MAX_LEN + 1] = { 0 };

    if (app_name == NULL) {
        debug_log(DLOG_ERROR, "Input NULL pointer!\n");
        return RET_ERR;
    }

    
    ret = third_party_proc_check_exist(app_name); // 在hash表中查找判断是否已存在
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: check snmpd not exist success!", __FUNCTION__);
        return RET_OK;
    }
    

    
    ret = check_process_exist_status(app_name, pid, sizeof(pid));
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "Snmpd is not exist!\n");
        
        ret = third_party_proc_hashtable_remove(app_name);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: remove snmpd from hash failed!\n", __FUNCTION__);
        }
        
        return RET_OK;
    }

    for (count = 0; count < 2; count++) {
        
        ret = kill_process_by_pid(pid, sizeof(pid));
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "Send kill snmpd command failed!\n");
            vos_task_delay(200);
            continue;
        }

        (void)memset_s(pid, SNMP_PID_MAX_LEN + 1, 0, SNMP_PID_MAX_LEN + 1);
        vos_task_delay(200);

        
        ret = check_process_exist_status(app_name, pid, sizeof(pid));
        if (ret == RET_OK) {
            debug_log(DLOG_DEBUG, "Kill snmpd failed!\n");
            continue;
        }

        ret = third_party_proc_hashtable_remove(app_name);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: remove snmpd from hash failed!", __FUNCTION__);
        }
        return RET_OK;
    }

    debug_log(DLOG_ERROR, "Kill snmpd failed!\n");
    return RET_ERR;
}

gint32 check_process_exist_status(gchar *app_name, gchar *pid, gint32 pid_size)
{
    gint32 iRet = -1;
    errno_t safe_fun_ret;

    FILE *ptr = NULL;
    gchar *pointer_begin = NULL;
    gchar *end = NULL;
    gchar buff[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gchar ps[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gint32 ret = RET_OK;

    if (app_name == NULL || pid == NULL) {
        debug_log(DLOG_ERROR, "Input NULL pointer!\n");
        return RET_ERR;
    }

    
    // 检查特殊字符
    ret = dal_check_shell_special_character(app_name);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    
    
    iRet = snprintf_s(ps, sizeof(ps), sizeof(ps) - 1, "ps -e | grep %s", app_name);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    
    ptr = popen((const gchar *)ps, "r");
    if (ptr != NULL) {
        if (vos_fgets_s(buff, SNMP_CONF_MAXLINE_LEN + 1, ptr) != NULL) {
            pointer_begin = strtok_s(buff, " ", &end);
            if (pointer_begin != NULL) {
                safe_fun_ret = strncpy_s(pid, pid_size, pointer_begin, pid_size - 1);
                if (safe_fun_ret != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                }
            }

            pclose(ptr);
            return RET_OK;
        }
    }

    if (ptr != NULL) {
        pclose(ptr);
    }

    return RET_ERR;
}

gint32 snmp_start_app_by_pid(void)
{
    gint32 ret = 0;
    gchar oldengineid[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gchar* cmd[SNMP_SYSTEM_CMD_LEN] = { NULL };

    
    
    
    
    ret = snmp_get_newoldengineid(oldengineid, sizeof(oldengineid));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "snmp_start_app_by_pid Get oldEngineID failed, clear /var/net-snmp/snmpd.conf\r\n");
        cmd[0] = "/bin/sh";
        cmd[1] = "-c";
        cmd[2] = "/bin/echo > /var/net-snmp/snmpd.conf";
        cmd[3] = NULL;
        (void)vos_system_s("/bin/sh", cmd);
    }

    
    

    
    cmd[0] = "/bin/sed";
    cmd[1] = "-i";
    cmd[2] = "/usmUser/d";
    cmd[3] = "/data/var/net-snmp/snmpd.conf";
    cmd[4] = NULL;
    (void)vos_system_s("/bin/sed", cmd);
    

    
    cmd[0] = "/bin/sh";
    cmd[1] = "-c";
    cmd[2] = "/usr/sbin/snmpd -p /var/run/snmpd.pid -c /opt/pme/pram/Snmp_snmpd.conf "
        "-g snmpd_user -u snmpd_user -I -icmp -f&";
    cmd[3] = NULL;
    (void)vos_system_s("/bin/sh", cmd);

    
    if (ret == -1) {
        debug_log(DLOG_DEBUG, "%s:line:%d vos_system_s fail!\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    return RET_OK;
}

gint32 snmp_start_app_by_name(gchar *app_name)
{
    gint32 ret = 0;
    gint32 count = 0;
    gchar pid[SNMP_PID_MAX_LEN + 1] = { 0 };
    guint32 proc_id;

    if (app_name == NULL) {
        debug_log(DLOG_ERROR, "Input NULL pointer!\n");
        return RET_ERR;
    }

    
    ret = third_party_proc_check_exist(SNMP_PROC_NAME); // 在hash表中查找判断是否已存在
    if (ret == RET_OK) {
        debug_log(DLOG_DEBUG, "%s: check snmpd exist success!", __FUNCTION__);
        return RET_OK;
    }
    

    
    ret = check_process_exist_status(app_name, pid, sizeof(pid)); // 通过ps命令检测是否已存在
    if (ret == RET_OK) {
        debug_log(DLOG_INFO, "Snmpd is exist!\n");
        
        ret = vos_str2int(pid, 10, &proc_id, NUM_TPYE_UINT32);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: vos_str2int %s failed!\n", __FUNCTION__, pid);
            return RET_OK;
        }
        ret = third_party_proc_hashtable_insert(SNMP_PROC_NAME, proc_id);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: insert %d to hash failed!\n", __FUNCTION__, proc_id);
        }
        
        return RET_OK;
    }

    
    for (count = 0; count < 2; count++) {
        ret = snmp_start_app_by_pid();
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "Send start snmpd command failed!\n");
            vos_task_delay(200);
            continue;
        }

        (void)memset_s(pid, SNMP_PID_MAX_LEN + 1, 0, SNMP_PID_MAX_LEN + 1);
        vos_task_delay(200);

        ret = check_process_exist_status(app_name, pid, sizeof(pid));
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "Start snmpd failed!\n");
            continue;
        }

        ret = vos_str2int(pid, DECIMAL_BASE, &proc_id, NUM_TPYE_UINT32);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: vos_str2int %s failed!", __FUNCTION__, pid);
            return RET_ERR;
        }

        ret = third_party_proc_hashtable_insert(SNMP_PROC_NAME, proc_id);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: insert %d to hash failed!", __FUNCTION__, proc_id);
        }

        debug_log(DLOG_DEBUG, "%s:Start snmpd success!\r\n", __FUNCTION__);

        return RET_OK;
    }

    debug_log(DLOG_ERROR, "Start snmpd failed!\n");
    return RET_ERR;
}


LOCAL gint32 snmp_get_user_privilege(const gchar *user_name, guint8 *privilege)
{
    OBJ_HANDLE user_obj_handle = 0;
    guint32 ret;

    ret = dal_get_specific_object_string(CLASS_USER, PROPERTY_USER_NAME, user_name, &user_obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:%d:Get user object handel failed.", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    ret = dal_get_property_value_byte(user_obj_handle, PROPERTY_USER_PRIVILEGE, privilege);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:%d:Get user privilege failed.", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 snmp_add_username_info(const gchar *user_name)
{
    gint32 ret = 0;
    guint8 privilege = 0;
    gchar buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gboolean user_snmp_enable = TRUE;
    gchar hex_buf[SNMP_HEX_USERNAME_LEN + 1] = { 0 };
    gchar hex_name[SNMP_HEX_USERNAME_LEN + 1] = { 0 };
    errno_t safe_fun_ret = EOK;
    gchar tmp_user_name[SNMP_USERNAME_MAX_LEN + 1] = { 0 };

    ret = check_user_snmp_enable(user_name, &user_snmp_enable);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    if (user_snmp_enable == FALSE) {
        return RET_OK;
    }

    ret = snmp_get_user_privilege(user_name, &privilege);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    if (privilege == RMCP_ADMIN) {
        ret = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "rwuser %s priv\ngroup gmyuser usm %s\n", user_name,
            user_name);
    } else if (privilege == RMCP_OPERATOR) {
        ret = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "rwuser %s priv -V %s\ngroup %s usm %s\n", user_name,
            VIEW_OPERATOR, GROUP_OPERATOR, user_name);
    } else if (privilege == RMCP_USER) {
        ret = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "rwuser %s priv -V %s\ngroup %s usm %s\n", user_name,
            VIEW_USER, GROUP_USER, user_name);
    } else {
        return RET_OK;
    }
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    safe_fun_ret = strncpy_s(tmp_user_name, sizeof(tmp_user_name), user_name, strlen(user_name));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    dal_convert_string_to_hex((guchar *)tmp_user_name, sizeof(tmp_user_name), (guchar *)hex_buf, sizeof(hex_buf));
    ret = snprintf_s(hex_name, sizeof(hex_name), sizeof(hex_name) - 1, "0x%s00", hex_buf);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    ret = snmp_add_username_to_file(hex_name, buf);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 check_user_snmp_enable(const gchar *user_name, gboolean *user_snmp_enable)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = 0;
    guint32 val = 0;
    guchar user_enable = 0;

    ret = dal_get_specific_object_string(CLASS_USER, PROPERTY_USER_NAME, user_name, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get handle of %s failed, username %s, ret %d!", CLASS_USER, user_name, ret);
        return RET_ERR;
    }

    // 获取用户是否禁用
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_USER_ISENABLE, &user_enable);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get %s.%s failed, ret %d!", dfl_get_object_name(obj_handle), PROPERTY_USER_ISENABLE,
            ret);
        return RET_ERR;
    }

    // 获取用户SNMP接口是否禁用
    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_USER_LOGIN_INTERFACE, &val);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get %s.%s failed, ret %d!", dfl_get_object_name(obj_handle),
            PROPERTY_USER_LOGIN_INTERFACE, ret);
        return RET_ERR;
    }

    if (user_enable == TRUE &&
        USER_LOGIN_INTERFACE_GET_BIT(val, USER_LOGIN_INTERFACE_SNMP_OFFSET) == USER_LOGIN_INTERFACE_ENABLE) {
        *user_snmp_enable = TRUE;
    } else {
        *user_snmp_enable = FALSE;
    }
    debug_log(DLOG_INFO, "user(%s) snmp interface enable state %d!", user_name, *user_snmp_enable);
    return RET_OK;
}


LOCAL gint32 snmp_add_username_to_file(const char *hex_name, const char *buf)
{
    gint32 ret = 0;
    FM_FILE_S *stream = NULL;
    gboolean found = FALSE;
    gint32 p_start = 0;
    gint32 p_end;
    gchar line_buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };

    g_mutex_lock(&g_snmp_mutex);
    
    stream = fm_fopen(SNMP_SHORT_FILE_NAME, "a+"); // a+
    if (stream == NULL) {
        g_mutex_unlock(&g_snmp_mutex);
        debug_log(DLOG_ERROR, "%s: fail to open %s", __FUNCTION__, SNMP_SHORT_FILE_NAME);
        return RET_ERR;
    }

    while (fm_fgets(line_buf, SNMP_CONF_MAXLINE_LEN + 1, stream) != NULL) {
        if (strstr(line_buf, hex_name) != NULL) {
            found = TRUE;
            p_end = fm_ftell(stream);           // 文件内部指针的读取位置
            p_start = p_end - strlen(line_buf); // 开始位置
            break;
        }
        (void)memset_s(line_buf, sizeof(line_buf), 0, sizeof(line_buf));
    }

    if (found == TRUE) { 
        ret = fm_insert(buf, strlen(buf), p_start, stream);
        if (ret != 0) {
            debug_log(DLOG_ERROR, "Call fm_insert failed!, ret %d", ret);
            fm_fclose(stream);
            g_mutex_unlock(&g_snmp_mutex);
            return RET_ERR;
        }
    } else {
        ret = fm_fwrite(buf, 1, strlen(buf), stream);
        if (ret != strlen(buf)) {
            debug_log(DLOG_ERROR, "Call fm_fwrite failed!");
            fm_fclose(stream);
            g_mutex_unlock(&g_snmp_mutex);
            return RET_ERR;
        }
    }
    
    fm_fclose(stream);
    g_mutex_unlock(&g_snmp_mutex);
    debug_log(DLOG_INFO, "add user snmp config successd!");
    return ret;
}


LOCAL gint32 snmp_modify_pravicy_protocol(gchar *user_name, guint32 len, guint8 pravicy_protocol)
{
    errno_t safe_fun_ret;
    gint32 ret = 0;
    FM_FILE_S *fp = NULL;
    gint32 p_start = 0;
    gchar hex_name[SNMP_HEX_USERNAME_LEN + 1] = { 0 };
    gchar buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gchar **str_array = NULL;
    gint32 str_length;
    gint32 old_buf_len;
    gchar *privacy_str = NULL;
    gchar new_privacy_str[SNMP_AUTH_LEN + 1] = { 0 };

    dal_convert_string_to_hex((guchar *)user_name, len, (guchar *)hex_name, sizeof(hex_name));
    safe_fun_ret = strcat_s(hex_name, sizeof(hex_name), "00");
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    g_mutex_lock(&g_snmp_mutex);
    fp = fm_fopen(SNMP_SHORT_FILE_NAME, "r+");
    if (fp == NULL) {
        g_mutex_unlock(&g_snmp_mutex);
        debug_log(DLOG_ERROR, "%s:fail to open %s", __FUNCTION__, SNMP_SHORT_FILE_NAME);
        return RET_ERR;
    }

    p_start = fm_ftell(fp);
    while (fm_fgets(buf, SNMP_CONF_MAXLINE_LEN + 1, fp) != NULL) {
        if (strstr(buf, hex_name) != NULL) {
            str_array = g_strsplit(buf, " ", SNMP_TOKENS_PER_LINE);
            if (str_array == NULL) {
                debug_log(DLOG_ERROR, "Prase snmp file failed!");
                continue;
            }

            str_length = g_strv_length(str_array);
            if (str_length != SNMP_TOKENS_PER_LINE) {
                debug_log(DLOG_DEBUG, "This line is not in the correct format!");
                g_strfreev(str_array);
                str_array = NULL;
                continue;
            }

            
            if (str_array[SNMP_USERNAMEHEX_INDEX_LINE] == NULL ||
                strlen(str_array[SNMP_USERNAMEHEX_INDEX_LINE]) < SNMP_HEX_PREFIX_LEN ||
                g_strcmp0(str_array[SNMP_USERNAMEHEX_INDEX_LINE] + SNMP_HEX_PREFIX_LEN, hex_name) != 0) {
                (void)memset_s(buf, sizeof(buf), 0, sizeof(buf));
                g_strfreev(str_array);
                str_array = NULL;
                continue;
            }
            
            privacy_str = strstr(buf, str_array[SNMP_PRIVPROCOTOL_INDEX_LINE]);
            if (privacy_str == NULL) {
                debug_log(DLOG_DEBUG, "This line is not in the correct format!");
                g_strfreev(str_array);
                break;
            }
            p_start += (strlen(buf) - strlen(privacy_str));
            old_buf_len = strlen(str_array[SNMP_PRIVPROCOTOL_INDEX_LINE]);
            g_strfreev(str_array);

            ret = snprintf_s(new_privacy_str, sizeof(new_privacy_str), sizeof(new_privacy_str) - 1, "%s",
                pravicy_protocol == USE_DES ? SNMP_PRIV_DES : SNMP_PRIV_AES);
            if (ret <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
                ret = RET_ERR;
                break;
            }
            ret = fm_modify(new_privacy_str, strlen(new_privacy_str), (gsize)p_start, (gsize)old_buf_len, fp);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: failed to call fm_modify!", __FUNCTION__);
            }
            break;
        }
        p_start = fm_ftell(fp);
        (void)memset_s(buf, sizeof(buf), 0, sizeof(buf));
    }

    fm_fclose(fp);
    g_mutex_unlock(&g_snmp_mutex);
    return ret;
}

LOCAL void snmp_free_property_value(GVariant **property_value)
{
    if (*property_value != NULL) {
        g_variant_unref(*property_value);
        *property_value = NULL;
    }
}

LOCAL gint32 snmp_get_state(guchar *state)
{
    GVariant *property_value = NULL;
    OBJ_HANDLE snmp_obj = 0;
    gint32 retv = 0;

    retv = dfl_get_object_handle(OBJ_NAME_SNMP, &snmp_obj);
    if (retv != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:dfl_get_object_handle(OBJ_NAME_SNMP) failed. Ret is %d\n", __FUNCTION__, retv);
        return RET_ERR;
    }

    retv = dfl_get_property_value(snmp_obj, PROPERTY_SNMP_STATE, &property_value);
    if (retv != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:dfl_get_property_value(SERVER_PORT_ATTRIBUTE_STATE) failed. Ret is %d\n",
            __FUNCTION__, retv);
        snmp_free_property_value(&property_value);
        return RET_ERR;
    }

    *state = g_variant_get_byte(property_value);
    snmp_free_property_value(&property_value);

    return RET_OK;
}

void enable_disable_snmpd(void)
{
    gint32 ret = 0;
    guchar snmp_state = 0;
    guint8 *option = NULL;

    
    ret = snmp_get_state(&snmp_state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:snmp_get_state failed!\r\n", __FUNCTION__);
        return;
    }

    if (g_snmpd_enable_flag == SERVICE_STOP || snmp_state == SERVICE_STOP) {
        
        option = (guint8 *)g_malloc0(sizeof(guint8));
        if (option == NULL) {
            debug_log(DLOG_ERROR, "Malloc failed!\n");
            return;
        }

        *option = STOP_SNMP;
        ret = vos_queue_send(g_snmpSendQueue, option);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Send stop snmp command failed!\n");
            g_free(option);
            return;
        }
    } else if (g_snmpd_enable_flag == SERVICE_START && snmp_state == SERVICE_START) {
        
        option = (guint8 *)g_malloc0(sizeof(guint8));
        if (option == NULL) {
            debug_log(DLOG_ERROR, "Malloc failed!\n");
            return;
        }

        *option = START_SNMP;
        ret = vos_queue_send(g_snmpSendQueue, option);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Send start snmp command failed!\n");
            g_free(option);
            return;
        }
    }

    return;
}

gint32 get_system_service_config(void)
{
    gint32 retv = RET_OK;
    GVariant *property_value = NULL;
    OBJ_HANDLE obj_handle = 0;

    retv = dfl_get_object_handle(OBJ_NAME_PME_SERVICECONFIG, &obj_handle);
    if (retv != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:Can't find object %s. Ret is %d\n", __FUNCTION__, OBJ_NAME_PME_SERVICECONFIG, retv);
        return RET_ERR;
    }

    
    
    retv = dfl_get_property_value(obj_handle, PROTERY_PME_SERVICECONFIG_HTTPS, &property_value);
    
    if (retv != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:dfl_get_property_value(PROTERY_PME_SERVICECONFIG_HTTPS) failed. Ret is %d\n",
            __FUNCTION__, retv);
        snmp_free_property_value(&property_value);
        return RET_ERR;
    }

    g_https_enable_flag = g_variant_get_byte(property_value);
    snmp_free_property_value(&property_value);

    retv = dfl_get_property_value(obj_handle, PROTERY_PME_SERVICECONFIG_SNMPD, &property_value);
    if (retv != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:dfl_get_property_value(PROTERY_PME_SERVICECONFIG_HTTPS) failed. Ret is %d\n",
            __FUNCTION__, retv);
        snmp_free_property_value(&property_value);
        return RET_ERR;
    }

    g_snmpd_enable_flag = g_variant_get_byte(property_value);
    snmp_free_property_value(&property_value);

    debug_log(DLOG_DEBUG, "%s:g_https_enable_flag = %d, g_snmpd_enable_flag = %d", __FUNCTION__, g_https_enable_flag,
        g_snmpd_enable_flag);

    return RET_OK;
}


void app_supervisory_task(void)
{
    gint32 retv = RET_OK;
    guint8 certificate_status = 0;
    OBJ_HANDLE obj_handle = 0;

    (void)prctl(PR_SET_NAME, (unsigned long)"AppSupervisor");

    
    retv = config_https_tls_protocol();
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "%s:config_https_tls_protocol failed", __func__);
    }

    

    retv = get_system_service_config();
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get_system_service_config failed", __func__);
    }

    update_http_configuration();

    update_https_configuration();

    config_https_verify_client();

    while (1) {
        if (certificate_status == 0) {
            (void)dfl_get_object_handle(SSL_OBJECT_OWNER, &obj_handle);
            (void)dal_get_property_value_byte(obj_handle, SSL_CERT_STATUS, &certificate_status);
        } else {
            enable_disable_web_service();
        }

        
        snmp_update_oldengineid();

        enable_disable_snmpd();

        
        vos_task_delay(2000);
    }
}


LOCAL void __execute_operate_snmpd_command(guint8 option)
{
    gint32 ret;
    switch (option) {
        case REBOOT_SNMP:
            g_rebootSnmp = FALSE;
            g_mutex_lock(&g_snmp_mutex);
            
            ret = kill_process_by_name(SNMP_PROC_NAME);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "Kill snmpd failed!");
                g_mutex_unlock(&g_snmp_mutex);
                break;
            }

            
            ret = snmp_start_app_by_name(SNMP_PROC_NAME);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "Start snmpd failed!");
                g_mutex_unlock(&g_snmp_mutex);
                break;
            }
            g_mutex_unlock(&g_snmp_mutex);
            break;

        case STOP_SNMP:
            
            g_mutex_lock(&g_snmp_mutex);
            ret = kill_process_by_name(SNMP_PROC_NAME);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "Kill snmpd failed!");
            }
            g_mutex_unlock(&g_snmp_mutex);
            break;

        case START_SNMP:
            
            g_mutex_lock(&g_snmp_mutex);
            ret = snmp_start_app_by_name(SNMP_PROC_NAME);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "Start snmpd failed!");
            }
            g_mutex_unlock(&g_snmp_mutex);
            break;

        default:
            break;
    }
}

gpointer revceive_operate_snmpd_command(gpointer data)
{
    OBJ_HANDLE pme_service_obj_handle = 0;
    guint8 snmp_enable = 0;
    GVariant *value = NULL;
    gint32 ret = 0;
    guint8 *option = NULL;

    (void)prctl(PR_SET_NAME, (unsigned long)"SnmpdServerCtrl");

    ret = dfl_get_object_handle(OBJ_NAME_PME_SERVICECONFIG, &pme_service_obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get pme service config handle failed!\n");
        return NULL;
    }

    ret = dfl_get_property_value(pme_service_obj_handle, PROTERY_PME_SERVICECONFIG_SNMPD, &value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get pme snmp enable state failed!\n");
        return NULL;
    }

    snmp_enable = g_variant_get_byte(value);
    g_variant_unref(value);
    value = NULL;

    for (;;) {
        ret = vos_queue_receive(g_snmpSendQueue, (gpointer *)&option, QUE_WAIT_FOREVER);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Get option from snmp queue failed!\r\n");
            continue;
        }

        if (snmp_enable == TRUE) {
            __execute_operate_snmpd_command(*option);
        }

        vos_task_delay(1000);
        g_free(option);
        option = NULL;
    }
}

LOCAL gint32 snmp_group_init(void)
{
    gchar *end = NULL;
    guint8 found_operator_view = 0;
    guint8 found_user_view = 0;
    gchar buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };

    g_mutex_lock(&g_snmp_mutex);
    FM_FILE_S *fp = fm_fopen(SNMP_SHORT_FILE_NAME, "r+");
    if (fp == NULL) {
        g_mutex_unlock(&g_snmp_mutex);
        debug_log(DLOG_ERROR, "%s:%d:fail to open %s\n", __FUNCTION__, __LINE__, SNMP_SHORT_FILE_NAME);
        return RET_ERR;
    }

    while (fm_fgets(buf, SNMP_CONF_MAXLINE_LEN + 1, fp) != NULL) {
        gchar *p = strtok_s(buf, " ", &end);
        if (p && (strcmp(p, "view") == 0)) {
            p = strtok_s(end, " ", &end);
            if (p != NULL) {
                if (strcmp(p, VIEW_OPERATOR) == 0) {
                    found_operator_view = 1;
                }

                if (strcmp(p, VIEW_USER) == 0) {
                    found_user_view = 1;
                }
            }
        }
    }

    dal_insert_file_end(fp, '\n');

    if (found_operator_view == 0) {
        gint32 ret = fm_fwrite(VIEW_ACCESS_OPERATOUR, 1, strlen(VIEW_ACCESS_OPERATOUR), fp);
        if (strlen(VIEW_ACCESS_OPERATOUR) != ret) {
            debug_log(DLOG_ERROR, "Call fm_fwrite failed, ret = %d!\n", ret);
            fm_fclose(fp);
            g_mutex_unlock(&g_snmp_mutex);
            return RET_ERR;
        }
    }

    dal_insert_file_end(fp, '\n');

    if (found_user_view == 0) {
        gint32 ret = fm_fwrite(VIEW_ACCESS_USER, 1, strlen(VIEW_ACCESS_USER), fp);
        if (strlen(VIEW_ACCESS_USER) != ret) {
            debug_log(DLOG_ERROR, "Call fm_fwrite failed, ret = %d!\n", ret);
            fm_fclose(fp);
            g_mutex_unlock(&g_snmp_mutex);
            return RET_ERR;
        }
    }

    fm_fclose(fp);
    g_mutex_unlock(&g_snmp_mutex);
    return RET_OK;
}

LOCAL gint32 snmp_user_init(void)
{
    FM_FILE_S *fp = NULL;
    gchar *end = NULL;
    gchar *p = NULL;
    gint32 p_start = 0;
    gchar buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };

    g_mutex_lock(&g_snmp_mutex);
    fp = fm_fopen(SNMP_SHORT_FILE_NAME, "r+");
    if (fp == NULL) {
        g_mutex_unlock(&g_snmp_mutex);
        debug_log(DLOG_ERROR, "%s:%d:fail to open %s\n", __FUNCTION__, __LINE__, SNMP_SHORT_FILE_NAME);
        return RET_ERR;
    }

    while (fm_fgets(buf, SNMP_CONF_MAXLINE_LEN + 1, fp) != NULL) {
        p = strtok_s(buf, " ", &end);
        if (p && (strcmp(p, SNMP_ROUSER) == 0)) {
            
            (void)fm_modify(SNMP_RWUSER, strlen(SNMP_RWUSER), (gsize)p_start, strlen(SNMP_ROUSER), fp);

            
            (void)fm_fseek(fp, (gsize)p_start, SEEK_SET);
        }

        p_start = fm_ftell(fp);
        if (p_start < 0) {
            debug_log(DLOG_ERROR, "Fail to modify %s ,Ret : %d.\n", SNMP_SHORT_FILE_NAME, p_start);
            break;
        }
    }

    fm_fclose(fp);
    g_mutex_unlock(&g_snmp_mutex);

    return RET_OK;
}


LOCAL gint32 snmp_set_ciphertext(OBJ_HANDLE obj_handle, const gchar *property, const gchar *new_ciphertext)
{
    gint32 ret;

    
    if (g_strcmp0(property, PROPERTY_SNMP_ROCOMMUNITY) == 0) {
        ret = snmp_modify_community(SNMP_ROCOMMIUNITY_IPV4, new_ciphertext);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Modify Snmp ipv4 rocommunity failed");
            return ret;
        }

        ret = snmp_modify_community(SNMP_ROCOMMIUNITY_IPV6, new_ciphertext);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Modify Snmp ipv6 rocommunity failed");
            return ret;
        }
    } else {
        ret = snmp_modify_community(SNMP_RWCOMMIUNITY_IPV4, new_ciphertext);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Modify Snmp ipv4 rocommunity failed");
            return ret;
        }

        ret = snmp_modify_community(SNMP_RWCOMMIUNITY_IPV6, new_ciphertext);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Modify Snmp ipv6 rocommunity failed");
            return ret;
        }
    }

    
    ret = dal_set_property_value_string(obj_handle, property, new_ciphertext, DF_SAVE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set %s failed, ret = %d", property, ret);
        return ret;
    }

    return RET_OK;
}


LOCAL gint32 snmp_update_community(const guint8 *old_component, const gchar *old_workkey, const guint8 *new_component,
    const gchar *new_workkey)
{
    gint32 ret = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar plain_rocomm[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gchar plain_rwcomm[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gchar *old_encrypt_rocomm = NULL;
    gchar *old_encrypt_rwcomm = NULL;
    gchar new_encrypt_rocomm[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gchar new_encrypt_rwcomm[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gint32 encrypt_len = 0;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;

    if (new_workkey == NULL || new_component == NULL || old_component == NULL || old_workkey == NULL) {
        return RET_ERR;
    }

    
    if (g_strcmp0(old_workkey, new_workkey) == 0) {
        return RET_OK;
    }

    ret = dfl_get_object_handle(OBJ_NAME_SNMP, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get snmp object handle failed, ret = %d!\n", ret);
        return RET_ERR;
    }

    
    property_name_list = g_slist_append(property_name_list, PROPERTY_SNMP_ROCOMMUNITY);
    property_name_list = g_slist_append(property_name_list, PROPERTY_SNMP_RWCOMMUNITY);
    ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value_list);
    g_slist_free(property_name_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get snmp ROCommunity and RWCommunity failed, ret = %d!\n", ret);
        return RET_ERR;
    }

    old_encrypt_rocomm = g_variant_dup_string((GVariant *)g_slist_nth_data(property_value_list, 0), 0);
    old_encrypt_rwcomm = g_variant_dup_string((GVariant *)g_slist_nth_data(property_value_list, 1), 0);
    g_slist_free_full(property_value_list, (GDestroyNotify)g_variant_unref);

    
    ret = get_plain_aes_128_data(old_component, COMPONENT_SIZE, old_encrypt_rocomm, plain_rocomm, sizeof(plain_rocomm),
        NO_PADDING);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get snmp plain ROCommunity failed, ret = %d!\n", ret);
        goto exit;
    }

    
    ret = get_encrypt_aes_128_data(new_component, COMPONENT_SIZE, new_workkey, plain_rocomm, new_encrypt_rocomm,
        SNMP_CONF_MAXLINE_LEN + 1, &encrypt_len, NO_PADDING);
    clear_sensitive_info(plain_rocomm, sizeof(plain_rocomm));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get snmp encrypt ROCommunity failed, ret = %d!\n", ret);
        goto exit;
    }

    
    ret = snmp_set_ciphertext(obj_handle, PROPERTY_SNMP_ROCOMMUNITY, new_encrypt_rocomm);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Update snmp ROCommunity failed, ret = %d!\n", ret);
        goto exit;
    }

    
    ret = get_plain_aes_128_data(old_component, COMPONENT_SIZE, old_encrypt_rwcomm, plain_rwcomm, sizeof(plain_rwcomm),
        NO_PADDING);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get snmp plain RWCommunity failed, ret = %d!\n", ret);
        goto exit;
    }

    
    ret = get_encrypt_aes_128_data(new_component, COMPONENT_SIZE, new_workkey, plain_rwcomm, new_encrypt_rwcomm,
        SNMP_CONF_MAXLINE_LEN + 1, &encrypt_len, NO_PADDING);
    clear_sensitive_info(plain_rwcomm, sizeof(plain_rwcomm));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get snmp encrypt RWCommunity failed, ret = %d!\n", ret);
        goto exit;
    }

    
    ret = snmp_set_ciphertext(obj_handle, PROPERTY_SNMP_RWCOMMUNITY, new_encrypt_rwcomm);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Update snmp RWCommunity failed, ret = %d!\n", ret);
        goto exit;
    }

exit:
    free_mem_set_null(old_encrypt_rocomm);
    free_mem_set_null(old_encrypt_rwcomm);
    clear_sensitive_info(new_encrypt_rocomm, sizeof(new_encrypt_rocomm));
    clear_sensitive_info(new_encrypt_rwcomm, sizeof(new_encrypt_rwcomm));
    return ret;
}


gint32 UpdateSnmpPrivacyProtocol(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    gint32 ret;
    guint8 privacy_protocol;
    gchar user_name[USER_NAME_MAX_LEN + 1] = { 0 };

    ret = dal_get_property_value_byte(object_handle, PROPERTY_USER_SNMPPRIVACYPROTOCOL, &privacy_protocol);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get %s.%s failed, ret %d", __FUNCTION__, dfl_get_object_name(object_handle),
            PROPERTY_USER_SNMPPRIVACYPROTOCOL, ret);
        return ret;
    }
    if (privacy_protocol == USE_NONE || privacy_protocol >= SNMP_PRIVACY_PROTOCOL_BUT) {
        debug_log(DLOG_ERROR, "%s:privacy protocol(%d) is invalid", __FUNCTION__, privacy_protocol);
        return RET_ERR;
    }

    ret = dal_get_property_value_string(object_handle, PROPERTY_USER_NAME, user_name, sizeof(user_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get %s.%s failed, ret %d", __FUNCTION__, dfl_get_object_name(object_handle),
            PROPERTY_USER_NAME, ret);
        return ret;
    }

    return snmp_modify_pravicy_protocol(user_name, sizeof(user_name), privacy_protocol);
}


gint32 UpdateSnmpCommunity(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name, GVariant *property_value)
{
    errno_t safe_fun_ret;
    gint32 ret;
    guint8 new_component[COMPONENT_SIZE] = { 0 };
    guint8 old_component[COMPONENT_SIZE] = { 0 };
    gchar new_workkey_buf[ENCRYPT_WORKKEY_STR_SIZE] = { 0 };
    gchar old_workkey_buf[ENCRYPT_WORKKEY_STR_SIZE] = { 0 };
    const gchar *new_workkey = NULL;
    const gchar *object_name = NULL;
    object_name = dfl_get_object_name(object_handle);

    
    if (dal_check_if_mm_board() == FALSE) {
        return RET_OK;
    }

    if (object_name == NULL) {
        debug_log(DLOG_ERROR, "dfl_get_object_name failed.\n");
        return RET_ERR;
    }

    if (strcmp(OBJ_ENCRYPTKEY_SNMPCOMM, object_name)) {
        return RET_OK;
    }

    g_mutex_lock(&g_snmpcomm_mutex);

    if (g_rollback_state == 1) {
        g_mutex_unlock(&g_snmpcomm_mutex);
        return RET_OK;
    }

    
    g_snmpcomm_update_state = 0;
    
    new_workkey = g_variant_get_string(property_value, 0);
    
    safe_fun_ret = strncpy_s(new_workkey_buf, sizeof(new_workkey_buf), new_workkey, sizeof(new_workkey_buf) - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }

    
    ret = dal_get_encryptkey_component(OBJ_ENCRYPTKEY_SNMPCOMM, PROPERTY_ENCRYPTKEY_COMPONENT, new_component,
        COMPONENT_SIZE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get new snmp community component failed, ret = %d!\n", ret);
        goto __ERROR_PROC;
    }

    
    ret = dal_get_encryptkey_component(OBJ_NAME_SNMP, PROPERTY_SNMP_COMMCOMPONENT, old_component, COMPONENT_SIZE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get local snmp community component failed, ret = %d!\n", ret);
        goto __ERROR_PROC;
    }

    
    ret =
        dal_get_encryptkey_workkey(OBJ_NAME_SNMP, PROPERTY_SNMP_COMMWORKKEY, old_workkey_buf, sizeof(old_workkey_buf));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get local snmp community workkey failed, ret = %d!\n", ret);
        goto __ERROR_PROC;
    }

    ret = snmp_update_community(old_component, old_workkey_buf, new_component, new_workkey_buf);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Update community failed, ret = %d!\n", ret);
        goto __ERROR_PROC;
    }

    if (memcmp(new_component, (const void *)old_component, COMPONENT_SIZE) != 0) {
        
        ret = dal_set_encryptkey_component(OBJ_NAME_SNMP, PROPERTY_SNMP_COMMCOMPONENT, new_component, COMPONENT_SIZE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Update snmp community workkey component failed, ret = %d!\n", ret);
            goto __ERROR_PROC;
        }
    }

    if (memcmp(new_workkey_buf, (const void *)old_workkey_buf, ENCRYPT_WORKKEY_STR_SIZE) != 0) {
        
        ret = dal_set_encryptkey_workkey(OBJ_NAME_SNMP, PROPERTY_SNMP_COMMWORKKEY, new_workkey_buf);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Update snmp community workkey failed, ret = %d!\n", ret);
            goto __ERROR_PROC;
        }
    }

__ERROR_PROC:
    g_snmpcomm_update_state = 1;
    
    g_mutex_unlock(&g_snmpcomm_mutex);

    clear_sensitive_info(new_workkey_buf, sizeof(new_workkey_buf));
    clear_sensitive_info(old_workkey_buf, sizeof(old_workkey_buf));
    clear_sensitive_info(new_component, sizeof(new_component));
    clear_sensitive_info(old_component, sizeof(old_component));
    return ret;
}


gint32 RollbackSnmpCommunity(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    gint32 ret = RET_OK;
    guint8 rollback_flag = 0;

    rollback_flag = g_variant_get_byte(property_value);
    if (rollback_flag == 0) {
        return RET_OK;
    }

    g_mutex_lock(&g_snmpcomm_mutex);

    if (g_rollback_state == 1) {
        g_mutex_unlock(&g_snmpcomm_mutex);
        return RET_OK;
    }

    
    if (dal_check_if_mm_board()) {
        g_rollback_state = 1;
        g_mutex_unlock(&g_snmpcomm_mutex);
        return RET_OK;
    }

#ifndef ARM64_HI1711_ENABLED
    OBJ_HANDLE snmp_obj = 0;

    
    ret = dfl_get_object_handle(OBJ_NAME_SNMP, &snmp_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get Snmp obj failed, ret = %d", ret);
        goto exit;
    }

    if (dal_check_if_masterkey_id_set(snmp_obj, PROPERTY_SNMP_MASTERKEY_ID) == FALSE) {
        goto exit;
    }

    
    ret = kmc_update_community(snmp_obj, PROPERTY_SNMP_ROCOMMUNITY, ENCRYPT_TYPE_REMOTE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Update rocommunity failed, ret = %d", ret);
        goto exit;
    }

    
    ret = kmc_update_community(snmp_obj, PROPERTY_SNMP_RWCOMMUNITY, ENCRYPT_TYPE_REMOTE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Update rwcommunity failed, ret = %d", ret);
        goto exit;
    }

    ret = kmc_sync_local_property(DOMAIN_ID_SNMP_COMM, snmp_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Sync property failed, ret = %d", ret);
    }

exit:
#endif
    g_rollback_state = 1;
    g_mutex_unlock(&g_snmpcomm_mutex);
    return ret;
}


LOCAL gint32 snmp_community_init(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    gchar ROCommunity[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gchar RWCommunity[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };

    ret = dal_get_property_value_string(obj_handle, PROPERTY_SNMP_ROCOMMUNITY, ROCommunity, SNMP_CONF_MAXLINE_LEN + 1);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get SNMP roCommunity failed!");
        return RET_ERR;
    }

    
    ret = snmp_modify_community(SNMP_ROCOMMIUNITY_IPV4, ROCommunity);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Modify Snmp ipv4 ROCommunity failed!");
    }
    ret = snmp_modify_community(SNMP_ROCOMMIUNITY_IPV6, ROCommunity);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Modify Snmp ipv6 ROCommunity failed!");
    }

    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_SNMP_RWCOMMUNITY, RWCommunity, SNMP_CONF_MAXLINE_LEN + 1);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get SNMP rwCommunity failed!");
        return RET_ERR;
    }
    
    ret = snmp_modify_community(SNMP_RWCOMMIUNITY_IPV4, RWCommunity);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Modify Snmp ipv4 RWCommunity failed!");
    }
    ret = snmp_modify_community(SNMP_RWCOMMIUNITY_IPV6, RWCommunity);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Modify Snmp ipv6 RWCommunity failed!");
    }

    return RET_OK;
}


LOCAL gint32 snmp_state_init(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    guint8 snmp_v1_status = 0;
    guint8 snmp_v2c_status = 0;
    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SNMP_V1STATE, &snmp_v1_status);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get SNMP V1 status failed!");
        return RET_ERR;
    }

    
    ret = snmp_modify_version_state(SNMP_VERSION_V1, SNMP_COMMUNITY_RO, snmp_v1_status);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Modify Snmp V1 ROCommunity state failed!");
    }
    ret = snmp_modify_version_state(SNMP_VERSION_V1, SNMP_COMMUNITY_RW, snmp_v1_status);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Modify Snmp V1 RWCommunity state failed!");
    }

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SNMP_V2CSTATE, &snmp_v2c_status);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get SNMP V2C status failed!");
        return RET_ERR;
    }

    
    ret = snmp_modify_version_state(SNMP_VERSION_V2C, SNMP_COMMUNITY_RO, snmp_v2c_status);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Modify Snmp V2C ROCommunity state failed!");
    }
    ret = snmp_modify_version_state(SNMP_VERSION_V2C, SNMP_COMMUNITY_RW, snmp_v2c_status);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Modify Snmp V2C RWCommunity state failed!");
    }
    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SNMP_V3STATUS, &g_snmpv3_state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get Snmp V3 state failed, ret %d!", ret);
        return ret;
    }
    get_snmpv3_initial_status(&g_snmpv3_state);
    
    if (g_snmpv3_state == SNMP_STATE_DISABLE) {
        ret = snmp_del_all_userinfo_from_file();
        if (ret != RET_OK) {
            return RET_ERR;
        }
    }
    return RET_OK;
}


LOCAL gint32 snmp_port_init(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    gint32 snmp_portid;
    GVariant *property_value = NULL;
    
    ret = dfl_get_property_value(obj_handle, PROPERTY_SNMP_PORTID, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get SNMP port id failed!");
        return RET_ERR;
    }
    snmp_portid = g_variant_get_int32(property_value);
    g_variant_unref(property_value);
    property_value = NULL;
    
    ret = snmp_modify_port(snmp_portid);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Modify Snmp port failed!\r\n");
    }
    return RET_OK;
}


LOCAL gint32 get_snmpd_staus(guint8 *snmpd_staus)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    ret = dfl_get_object_handle(OBJ_NAME_PME_SERVICECONFIG, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get pme service config handle failed!");
        return RET_ERR;
    }

    ret = dal_get_property_value_byte(obj_handle, PROTERY_PME_SERVICECONFIG_SNMPD, snmpd_staus);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get snmpd service status failed!");
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL void snmp_engine_id_nic_init(void)
{
#define ENGINE_ID_NIC_KEY "engineIDNic"
#define ENGINE_ID_NIC_CONFIG "engineIDNic eth3\n"
#define ATLAS800D_RM_BOARD_ID 0xbe
    gint32 ret;
    FM_FILE_S *fp = NULL;
    gchar *end = NULL;
    gchar *p = NULL;
    guint8 found_flag = 0;
    gchar buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };

    
    if (!(dal_is_aggregation_management_chassis_product() == TRUE ||
        dal_match_board_id(PRODUCT_ID_ATLAS, ATLAS800D_RM_BOARD_ID) == TRUE)) {
        return;
    }

    g_mutex_lock(&g_snmp_mutex);
    fp = fm_fopen(SNMP_SHORT_FILE_NAME, "r+");
    if (fp == NULL) {
        g_mutex_unlock(&g_snmp_mutex);
        debug_log(DLOG_ERROR, "%s:fail to open %s", __FUNCTION__, SNMP_SHORT_FILE_NAME);
        return;
    }

    while (fm_fgets(buf, SNMP_CONF_MAXLINE_LEN, fp) != NULL) {
        p = strtok_s(buf, " ", &end);
        if (p != NULL && (strcmp(p, ENGINE_ID_NIC_KEY) == 0)) {
            
            found_flag = 1;
            break;
        }
    }

    if (found_flag == 0) {
        dal_insert_file_end(fp, '\n');
        ret = fm_fwrite(ENGINE_ID_NIC_CONFIG, 1, strlen(ENGINE_ID_NIC_CONFIG), fp);
        if (ret != strlen(ENGINE_ID_NIC_CONFIG)) {
            debug_log(DLOG_ERROR, "Call fm_fwrite failed, ret = %d", ret);
        }
    }
    fm_fclose(fp);
    g_mutex_unlock(&g_snmp_mutex);
    return;
}

gint32 snmp_configuration_init(void)
{
    gint32 ret;
    guint8 snmpd_staus = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar oldengineid[SNMP_CONF_MAXLINE_LEN + 1 ] = { 0 };

    
    third_party_proc_hashtable_init();
    

    
    ret = get_snmpd_staus(&snmpd_staus);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    if (snmpd_staus == 0) {
        return RET_OK;
    }
    
    ret = dfl_get_object_handle(OBJ_NAME_SNMP, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get SNMP handle failed!");
        return RET_ERR;
    }

    ret = snmp_community_init(obj_handle);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    ret = snmp_state_init(obj_handle);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    
    ret = snmp_port_init(obj_handle);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    
    ret = snmp_get_oldengineid(oldengineid, sizeof(oldengineid));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get SNMP oldengineid failed!");
        return RET_ERR;
    }

    
    if (strlen(oldengineid) == 0 || (!strncmp(oldengineid, SNMP_ERROR_OLDENGINEID, 0))) {
        
        snmp_shield_oldengineid();
    }

    
    
    ret = snmp_group_init();
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Snmp group init failed!\n");
    }
    

    
    ret = snmp_user_init();
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Snmp user init failed!\n");
    }
    

    snmp_engine_id_nic_init();

    return RET_OK;
}

void snmp_wait_community_rollback(void)
{
    gint32 ret = 0;
    guint8 pre_rollback_flag = 0;
    guint8 upgraded_flag = 0;
    OBJ_HANDLE obj_handle = 0;
    
    ret = dfl_get_object_handle(OBJECT_PFN, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "snmp: Get upgrade object handle failed, ret = %d!\n", ret);
        return;
    }

    ret = dal_get_property_value_byte(obj_handle, PFN_PREROLLBACK_FLAG, &pre_rollback_flag);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "snmp: Get upgrade prerollback flag failed!, ret = %d\n", ret);
        return;
    }

    ret = dal_get_property_value_byte(obj_handle, PFN_UPGRADED_FLAG, &upgraded_flag);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "snmp: Get upgraded flag failed!, ret = %d\n", ret);
        return;
    }

    
    
    while (1) {
        if (upgraded_flag == 0 && pre_rollback_flag == 0) {
            
            if (g_snmpcomm_update_state == 1) {
                break;
            }
        } else {
            
            if (g_rollback_state == 1) {
                break;
            }
        }

        vos_task_delay(10);
    }

    
    return;
}


void revert_file_owner_on_exit(void)
{
    GSList *obj_node = NULL;
    file_owner_info *owner_info = NULL;
    gint32 ret;

    for (obj_node = g_revert_owner_list; obj_node != NULL; obj_node = obj_node->next) {
        owner_info = (file_owner_info *)obj_node->data;
        if (owner_info == NULL) {
            continue;
        }

        
        ret = chown(owner_info->file_name, owner_info->own_uid, owner_info->own_gid);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: change file %s owner failed, ret = %d", __FUNCTION__, owner_info->file_name,
                ret);
        }
    }

    
    g_slist_free_full(g_revert_owner_list, g_free);
    g_revert_owner_list = NULL;

    return;
}


gint32 SetSnmpAgentAddr(OBJ_HANDLE obj_handle, gpointer user_data, gchar *property_name, GVariant *property_value)
{
    gint32 ret;
    OBJ_HANDLE snmp_handle = 0;
    gint32 port = 0;
    const gchar *obj_name = NULL;

    obj_name = dfl_get_object_name(obj_handle);
    if ((check_object_is_specific_out_type(obj_handle, OUTTER_GROUP_TYPE) != RET_OK) &&
        (check_object_is_specific_out_type(obj_handle, VETH_GROUP_TYPE) != RET_OK) &&
        (check_object_is_specific_out_type(obj_handle, FLOATIP_ETHERNET) != RET_OK) &&
        (check_object_is_specific_out_type(obj_handle, MAINT_DEDICATED_ETHERNET) != RET_OK) &&
        (strcmp(obj_name, OBJECT_CLASS_SMS) != 0) && (strcmp(obj_name, ASM_OBJECT_NAME) != 0)) {
        return RET_OK;
    }

    ret = dal_get_object_handle_nth(CLASS_SNMP, 0, &snmp_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get object handle failed, ret:%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    ret = dal_get_property_value_int32(snmp_handle, PROPERTY_SNMP_PORTID, &port);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get snmp service port failed, ret:%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    ret = snmp_modify_port(port);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:modify snmp agent address failed, ret:%d", __FUNCTION__, ret);
    }
    return RET_OK;
}


gint32 ipmi_cmd_set_snmp_sys_name(gconstpointer msg_data, gpointer user_data)
{
    const guint8 *req_data = NULL;
    guint8 length;
    gint req_len;
    gint32 ret = RET_OK;
    OBJ_HANDLE obj_handle = 0;
    gchar sysName[SNMP_STANDARD_SYS_NAME_MAX_LEN + 1] = { 0 };
    debug_log(DLOG_DEBUG, "%s entry", __FUNCTION__);

    req_data = get_ipmi_src_data(msg_data);
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "Failed to set the snmp host name, req is NULL");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }
    req_len = get_ipmi_src_data_len(msg_data);
    length = req_data[SNMP_SYS_NAME_LEN_OFFSET];
    if (length == 0 || req_len != length + SNMP_SYS_NAME_OFFSET) {
        debug_log(DLOG_ERROR, "Failed to set the snmp host name, length: %d, req_len:%d", length, req_len);
        return ipmi_send_simple_response(msg_data, COMP_CODE_LEN_INVALID);
    }
    ret = memcpy_s(&sysName[0], sizeof(sysName) - 1, &req_data[SNMP_SYS_NAME_OFFSET], length);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s memcpy_s failed, ret = %d!", __FUNCTION__, ret);
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    ret = dal_get_object_handle_nth(CLASS_SNMP, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get snmp handle Failed. %d", ret);
        return ipmi_send_simple_response(msg_data, COMP_CODE_DATA_NOT_AVAILABLE);
    }

    ret = dal_set_property_value_string(obj_handle, PROPERTY_SNMP_SYS_NAME, sysName, DF_SAVE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "set snmp sysName Failed. %d", ret);
        ipmi_operation_log(msg_data, "Set snmp sysName to %s failed", sysName);
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }
    ipmi_operation_log(msg_data, "Set snmp sysName to %s successfully", sysName);
    return ipmi_send_simple_response(msg_data, COMP_CODE_SUCCESS);
}


gint32 ipmi_cmd_get_snmp_sys_name(gconstpointer msg_data, gpointer user_data)
{
    const guint8 *req_data = NULL;
    guint8 resp_data[SNMP_STANDARD_SYS_NAME_MAX_LEN] = { 0 };
    gchar sysName[SNMP_STANDARD_SYS_NAME_MAX_LEN + 1] = { 0 };
    gint32 ret;
    guint32 manufactureid = OEM_MANUFACTURE_ID;
    guint8 index_id = 0;
    OBJ_HANDLE obj_handle = 0;

    debug_log(DLOG_DEBUG, "%s entry", __FUNCTION__);

    req_data = get_ipmi_src_data(msg_data);
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "Failed to get the snmp host name, req is NULL");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    ret = dal_get_object_handle_nth(CLASS_SNMP, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get snmp handle Failed. %d", ret);
        return ipmi_send_simple_response(msg_data, COMP_CODE_DATA_NOT_AVAILABLE);
    }

    ret = dal_get_property_value_string(obj_handle, PROPERTY_SNMP_SYS_NAME, sysName, sizeof(sysName));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get snmp sysName Failed. %d", ret);
        return ipmi_send_simple_response(msg_data, COMP_CODE_DATA_NOT_AVAILABLE);
    }

    resp_data[index_id++] = COMP_CODE_SUCCESS;
    resp_data[index_id++] = LONGB0(manufactureid);
    resp_data[index_id++] = LONGB1(manufactureid);
    resp_data[index_id++] = LONGB2(manufactureid);
    ret = memcpy_s(&resp_data[SNMP_SYS_NAME_IPMI_HEAD], sizeof(resp_data) - SNMP_SYS_NAME_IPMI_HEAD, &sysName[0],
        strlen(sysName));
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "copy snmp sysName Failed. %d", ret);
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }
    debug_log(DLOG_DEBUG, "Obtain the snmp host name(%s) successfully", sysName);
    return ipmi_send_response(msg_data, strlen(sysName) + SNMP_SYS_NAME_IPMI_HEAD, resp_data);
}