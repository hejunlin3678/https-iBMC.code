
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include "snmp_manage_app.h"
#include "keyGen.h"
#include "pme_app/pme_app.h"

#define SNMP_ENGINE_ID_LENGTH 13 


gint32 sc_get_properlength1(gint32 hashtype, guint32 hashtype_len)
{
    
    if (hashtype == USE_MD5) {
        return BYTESIZE(SNMP_TRANS_AUTHLEN_HMACMD5);
    } else if ((hashtype == USE_SHA) || (hashtype == USE_SHA1)) {
        return BYTESIZE(SNMP_TRANS_AUTHLEN_HMACSHA1);
    } else if (hashtype == USE_SHA256) {
        return BYTESIZE(SNMP_TRANS_AUTHLEN_HMAC192SHA256);
    } else if (hashtype == USE_SHA384) {
        return BYTESIZE(SNMP_TRANS_AUTHLEN_HMAC256SHA384);
    } else if (hashtype == USE_SHA512) {
        return BYTESIZE(SNMP_TRANS_AUTHLEN_HMAC384SHA512);
    }

    return RET_ERR;
}


LOCAL gint32 get_user_snmp_Ku_property(const gchar *user_name, guchar pwd_key_type, gchar *ku_key, guint32 ku_max_len,
    guint32 *ku_key_len)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar *property_Ku_name = NULL;
    gchar *property_Ku_name_len = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;
    const gchar *snmp_ku_val = NULL;
    gsize tmp_key_len = 0;

    if (pwd_key_type == SNMP_AUTH_TYPE) {
        property_Ku_name = PROPERTY_SNMP_AUTH_KU;
        property_Ku_name_len = PROPERTY_SNMP_AUTH_KULEN;
    } else if (pwd_key_type == SNMP_PRIV_TYPE) {
        property_Ku_name = PROPERTY_SNMP_PRIV_KU;
        property_Ku_name_len = PROPERTY_SNMP_PRIV_KULEN;
    } else {
        debug_log(DLOG_ERROR, "invalid pwd_key_type %d", pwd_key_type);
        return RET_ERR;
    }

    ret = dal_get_specific_object_string(CLASS_USER, PROPERTY_USER_NAME, user_name, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get user(%s) object handle failed, %d!", __FUNCTION__, user_name, ret);
        return ret;
    }
    property_name_list = g_slist_append(property_name_list, property_Ku_name);
    property_name_list = g_slist_append(property_name_list, property_Ku_name_len);
    ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value_list);
    g_slist_free(property_name_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:dfl_multiget_property_value failed", __FUNCTION__);
        return ret;
    }

    snmp_ku_val = (const gchar *)g_variant_get_fixed_array((GVariant *)g_slist_nth_data(property_value_list, 0),
        &tmp_key_len, sizeof(guchar));
    if (snmp_ku_val == NULL) {
        g_slist_free_full(property_value_list, (GDestroyNotify)g_variant_unref);
        debug_log(DLOG_ERROR, "%s:g_variant_get_fixed_array failed", __FUNCTION__);
        return RET_ERR;
    }
    *ku_key_len = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value_list, 1));
    ret = memcpy_s(ku_key, ku_max_len, snmp_ku_val, tmp_key_len);
    g_slist_free_full(property_value_list, (GDestroyNotify)g_variant_unref);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s:memcpy_s failed", __FUNCTION__);
        return RET_ERR;
    }
    return RET_OK;
}


gint32 get_snmp_pwd_key(const gchar *user_name, const gchar *passbuf, SNMP_ADDITION_DATA *snmp_data, gchar *pwd_key,
    guint32 pwd_key_len)

{
    oid hashtype[USM_AUTH_PROTO_NOAUTH_LEN * 2] = { 0 };
    gint32 ret;
    gchar Ku[USM_AUTH_KU_LEN] = { 0 };
    guint32 Kulen = USM_AUTH_KU_LEN;
    gint32 pwd_len;
    size_t tmp_key_len;

    if (user_name == NULL || passbuf == NULL || snmp_data == NULL || pwd_key == NULL) {
        debug_log(DLOG_ERROR, "input is NULL");
        return RET_ERR;
    }

    
    init_snmp("snmpapp");
    ret = get_snmp_hash_protocol(snmp_data->snmp_auth, hashtype, sizeof(hashtype), USER_SNMP_TYPE_AUTH);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get_snmp_hash_protocol(%d) failed", __FUNCTION__, snmp_data->snmp_auth);
        return ret;
    }

    ret = get_user_snmp_Ku_property(user_name, snmp_data->pwd_key_type, Ku, sizeof(Ku), &Kulen);
    if (ret != RET_OK) { 
        if (strlen(passbuf) == 0) {
            debug_log(DLOG_ERROR, "Length of passbuf is invalid");
            return RET_ERR;
        }
        
        pwd_len = ((strlen(passbuf) < USM_LENGTH_P_MIN) ? USM_LENGTH_P_MIN : strlen(passbuf));
        ret = generate_Ku(hashtype, USM_AUTH_PROTO_NOAUTH_LEN, (const guchar *)passbuf, pwd_len, (guchar *)Ku,
            (size_t *)&Kulen);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s Fail to generate Ku Key.ret = (%d)", __FUNCTION__, ret);
            return RET_ERR;
        }
    }
    tmp_key_len = pwd_key_len;
    ret = generate_kul(hashtype, USM_AUTH_PROTO_NOAUTH_LEN, (const guchar *)snmp_data->str_engineID,
        SNMP_ENGINE_ID_LENGTH, (guchar *)Ku, Kulen, (guchar *)pwd_key, &tmp_key_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s Fail to generate pwdkey Key.ret = (%d)", __FUNCTION__, ret);
        return RET_ERR;
    }
    return RET_OK;
}
