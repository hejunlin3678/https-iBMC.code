
#ifndef __SNMPUSERMETHOD_H__
#define __SNMPUSERMETHOD_H__

#include "pme_app/pme_app.h"
gint32 AddSnmpUserName(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 DelSnmpUserName(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 SetSnmpUserPassword(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 CompareSnmpUserPassword(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 AddSnmpUserPassword(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 GetSnmpUserInfo(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 GetSnmpUserHexInfo(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 DelSnmpUserPassword(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 DeleteNoAccessSnmpUser(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 SetSnmpUserPrivilege(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 UpdateSnmpUserPasswordByRename(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 snmp_check_username_exist(const gchar *username);
gint32 snmp_del_username_from_file(const gchar *user_name);
gint32 del_snmp_conf_from_file(const gchar *user_name, guint8 *match_flag);
#endif