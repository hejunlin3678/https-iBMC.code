
#ifndef __SNMPMETHOD_H__
#define __SNMPMETHOD_H__

#include "pme_app/pme_app.h"

#ifdef ARM64_HI1711_ENABLED // 1711的版本需要
#define DEFAULT_SYSNAME "iBMC3"
#else
#define DEFAULT_SYSNAME "iBMC"
#endif

gint32 RebootSnmp(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 SetSnmpROCommunity(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 SetSnmpRWCommunity(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 GetSnmpROCommunity(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 GetSnmpRWCommunity(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 SetSnmpV1VersionState(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 SetSnmpV2CVersionState(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 SetSnmpV3VersionState(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 SetSnmpVersionState(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 SetSnmpState(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 SetSnmpPortID(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 SetSnmpAuthProtocol(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 SetSnmpPrivProtocol(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 SetLongPasswordEnable(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 SetSnmpRWCommunityState(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);

gint32 snmp_method_set_permit_rule(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 SetSnmpStadardSysContact(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 SetSnmpStadardSysName(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 SetSnmpStadardSysLocation(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);


gint32 SetIsUpdateEngineID(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);

gint32 SetHostNameSyncFlag(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 kmc_update_snmp_comm(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 snmp_config_sync_handler(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 snmp_modify_community(gint32 type, const gchar *community);
gint32 snmp_modify_version_state(guint8 version, guint8 type, guint8 state);
gint32 snmp_modify_port(gint32 port_id);
gint32 kmc_update_community(OBJ_HANDLE snmp_obj, const gchar *property, guint8 encrypt_type);
gint32 snmp_del_all_userinfo_from_file(void);
void get_snmpv3_initial_status(guint8 *status);
guint8 get_snmpv3_status(void);
#endif