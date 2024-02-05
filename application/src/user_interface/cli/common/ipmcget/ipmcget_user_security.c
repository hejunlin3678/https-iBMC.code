

#include "ipmcget_user_security.h"

const struct valstr ipmc_privlvl_vals[IPMC_PRIVIVI_VALS] = {
    
    { USERROLE_USER,     "USER"          },
    { USERROLE_OPERATOR, "OPERATOR"      },
    { USERROLE_ADMIN,    "ADMINISTRATOR" },
    { USERROLE_CUST1,    "CUSTOM ROLE1"  },
    { USERROLE_CUST2,    "CUSTOM ROLE2"  },
    { USERROLE_CUST3,    "CUSTOM ROLE3"  },
    { USERROLE_CUST4,    "CUSTOM ROLE4"  },
    { USERROLE_NOACCESS, "NO ACCESS"     },
    { 0xFF,              NULL            },
    
};


gint32 get_userinfo_level(guchar privilege, const gchar *rolepriv, gint32 *uid_level)
{
    uid_t uid = 0;
    gint32 ret;
    guint8 user_roleid = 0;
    guint8 user_enablestate = 0;
    guint8 role_privilege = 0;
    guint8 user_privilege = 0;
    GVariant *property_value = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    LDAP_USER usr = { 0 };
    guint32 i = 0;

     
    if ((rolepriv == NULL) || (uid_level == NULL)) {
        return FALSE;
    }

    
    ret = ipmc_get_uid(&uid);
    if (ret != RET_OK) {
        g_printf("Get user info failed.\r\n");
        return FALSE;
    }

    
    if (uid >= LDAP_USER_ID_BASE) {
        if (uip_read_ldap_user(uid, &usr) != RET_OK) {
            return FALSE;
        }

        for (i = 0; i < LDAP_USER_ROLE_ID_NUM; i++) {
            // 0 表示结束了
            if (usr.roleid[i] == 0) {
                break;
            }

            
            obj_handle = 0;
            ret = dal_get_specific_object_byte(CLASS_USERROLE, PROPERTY_USERROLE_ID, usr.roleid[i], &obj_handle);
            if (ret != RET_OK) {
                return FALSE;
            }

            
            (void)dal_get_property_value_byte(obj_handle, rolepriv, &role_privilege);
            if ((role_privilege == 1) && (usr.privilege >= privilege)) {
                *uid_level = 0;
                return TRUE;
            }
        }

        return FALSE;
    }

    
    uid = (uid == 0) ? 502 : uid; 
    if (uid > USER_ID_BASE) {
        uid = uid - USER_ID_BASE;
        
        property_value = g_variant_new_byte((guint8)uid);
        ret = dfl_get_specific_object(CLASS_USER, PROPERTY_USER_ID, property_value, &obj_handle);
        g_variant_unref(property_value);
        if (ret != RET_OK) {
            g_printf("User is not exist.\r\n");
            return FALSE;
        }

        property_name_list = g_slist_append(property_name_list, PROPERTY_USER_ROLEID);
        property_name_list = g_slist_append(property_name_list, PROPERTY_USER_ISENABLE);
        property_name_list = g_slist_append(property_name_list, PROPERTY_USER_PRIVILEGE);
        ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value_list);
        g_slist_free(property_name_list);
        if (ret != DFL_OK) {
            g_printf("Get User role and enable state failed.\r\n");
            return FALSE;
        }

        user_roleid = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, 0));
        user_enablestate = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, 1));
        user_privilege = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, 2));
        uip_free_gvariant_list(property_value_list);

        
        obj_handle = 0;
        ret = dal_get_specific_object_byte(CLASS_USERROLE, PROPERTY_USERROLE_ID, user_roleid, &obj_handle);
        if (ret != RET_OK) {
            g_printf("Get user role object by role id failed.\r\n");
            return FALSE;
        }

        
        (void)dal_get_property_value_byte(obj_handle, rolepriv, &role_privilege);
        
        if ((role_privilege == 1) && (user_enablestate == 1) && (user_privilege >= privilege)) {
            *uid_level = 0;
        } else {
            *uid_level = uid;
        }

        return TRUE;
    }

    return FALSE;
}

LOCAL void print_login_interface_info(guint32 login_interface, guint32 buff_size, gchar **plogin_interface_string)
{
    gint32 n;
    gint32 interface_index[USER_LOGIN_INTERFACE_MAX] = {0};
    gint32 interface_index_count = 0;

    for (n = 0; n < USER_LOGIN_INTERFACE_MAX; n++) {
        if (USER_LOGIN_INTERFACE_GET_BIT(login_interface, (guint32)n) == USER_LOGIN_INTERFACE_ENABLE) {
            interface_index[interface_index_count++] = n;
        }
    }

    for (n = 0; n < interface_index_count; n++) {
        (void)strncat_s(*plogin_interface_string, buff_size, USER_LOGIN_INTERFACE_NAMES[interface_index[n]],
            strlen(USER_LOGIN_INTERFACE_NAMES[interface_index[n]]));

        if (n < interface_index_count - 1) {
            (void)strncat_s(*plogin_interface_string, buff_size, ",", strlen(","));
        }
    }
}



gint32 print_user_info_list(guint32 uid)
{
#define PUBLICKEY_ENABLE_ON 1
#define PUBLICKEY_ENABLE_OFF 0
    gint32 ret;
    guchar user_roleid = 0;
    guint8 id;
    gchar name[128] = {0};
    OBJ_HANDLE obj_handle = 0;
    
    OBJ_HANDLE obj_handle_netconfig = 0;
    guint8 netconfig_enable = 0;
    
    guint32 login_interface = 0;
    gchar login_interface_string[128] = {0};
    gchar *plogin_interface_string = NULL;
    guint8 public_key_enable = 0;
    gchar publickey_hash_string[128] = {0};
    guint8 user_state = 0;

    
    ret = dal_get_object_handle_nth(BMC_PRODUCT_NAME_APP, 0, &obj_handle);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    
    ret = dal_get_object_handle_nth(OBJ_NAME_PME_SERVICECONFIG, 0, &obj_handle_netconfig);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_SSH_PUBLICKEY_ENABLE, &public_key_enable);
    
    (void)dal_get_property_value_byte(obj_handle_netconfig, PROTERY_PME_SERVICECONFIG_NETCONFIG, &netconfig_enable);
    

    
    
    if (public_key_enable == PUBLICKEY_ENABLE_ON) {
        g_printf("%-8s%-18s%-15s%-48s%-48s%-15s\r\n", "ID", "Name", "Privilege", "Interface", "PublicKeyHash", "State");
    } else {
        g_printf("%-8s%-18s%-15s%-48s%-15s\r\n", "ID", "Name", "Privilege", "Interface", "State");
    }

    

    
    for (id = 2; id < IPMC_MAX_USER_NUMBER; id++) {
        
        if ((id != uid) && (uid != 0)) {
            continue;
        }

        if (dal_get_specific_object_byte(CLASS_USER, PROPERTY_USER_ID, id, &obj_handle) != RET_OK) {
            return RET_ERR;
        }

        (void)dal_get_property_value_string(obj_handle, PROPERTY_USER_NAME, name, sizeof(name));
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_USER_ROLEID, &user_roleid);
        (void)dal_get_property_value_uint32(obj_handle, PROPERTY_USER_LOGIN_INTERFACE, &login_interface);

        
        if (netconfig_enable == SERVICE_NETCONFIG_ENABLE) {
            
            login_interface = login_interface & USER_LOGIN_INTERFACE_VALUE_MASK;
            
        } else {
            login_interface = login_interface & USER_LOGIN_IPMI_LOCAL_VALUE_MASK;
        }

        

        (void)memset_s(login_interface_string, sizeof(login_interface_string), 0, sizeof(login_interface_string));

        
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_USER_ISENABLE, &user_state);
        

        // 如果没有用户，不需要显示接口信息
        if (name[0] != 0) {
            plogin_interface_string = login_interface_string;
            print_login_interface_info(login_interface, sizeof(login_interface_string), &plogin_interface_string);
        }

        if (public_key_enable == PUBLICKEY_ENABLE_ON) {
            (void)memset_s(publickey_hash_string, sizeof(publickey_hash_string), 0, sizeof(publickey_hash_string));

            
            (void)dal_get_property_value_string(obj_handle, PROPERTY_USER_PUBLICKEY_HASH, publickey_hash_string,
                sizeof(publickey_hash_string));

            if (strlen(publickey_hash_string) > 0) {
                g_printf("%-8u%-18s%-15s%-48s%-48s%-15s\r\n", id, name, val_to_str(user_roleid, ipmc_privlvl_vals),
                    login_interface_string, publickey_hash_string, (user_state == 0) ? "Disabled" : "Enabled");
            } else {
                g_printf("%-8u%-18s%-15s%-48s%-48s%-15s\r\n", id, name, val_to_str(user_roleid, ipmc_privlvl_vals),
                    login_interface_string, "NA", (user_state == 0) ? "Disabled" : "Enabled");
            }
        } else {
            g_printf("%-8u%-18s%-15s%-48s%-15s\r\n", id, name, val_to_str(user_roleid, ipmc_privlvl_vals),
                login_interface_string, (user_state == 0) ? "Disabled" : "Enabled");
        }
    }

    

    return RET_OK;
}
