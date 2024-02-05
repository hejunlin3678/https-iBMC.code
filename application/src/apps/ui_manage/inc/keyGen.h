#ifndef UUSER_KEY_H_
#define UUSER_KEY_H_

#include <glib.h>
#include "snmp_manage_app.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 


#define SNMP_AUTH_TYPE 0
#define SNMP_PRIV_TYPE 1

typedef struct snmp_addition_data {
    gchar str_engineID[SNMP_ENGINEID_MAX_LEN + 1];
    gint32 snmp_auth;    
    guchar pwd_key_type; 
} SNMP_ADDITION_DATA;

gint32 get_snmp_pwd_key(const gchar *user_name, const gchar *passbuf, SNMP_ADDITION_DATA *snmp_data, gchar *pwd_key,
    guint32 pwd_key_len);
gint32 sc_get_properlength1(gint32 hashtype, guint32 hashtype_len);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 
#endif
