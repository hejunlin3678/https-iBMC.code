#ifndef __UIP_WEB_INTERFACE_H__
#define __UIP_WEB_INTERFACE_H__
#include "uip.h"
#include "pme_app/pme_app.h"

#ifdef __cplusplus
extern "C" {
#endif



#define  UIP_PACKET_ERR_LOCK_FAIL	1000
#define SEL_HISTORY_TAR_FILE    "/opt/pme/save/sensor_alarm_sel.bin.tar.gz"
#define SEL_CURRENT_DIR "/opt/pme/pram"
#define SEL_FILE_NAME "sensor_alarm_sel.bin"

/*
数据库中表各列为
selid		-- SEL ID
level		-- 告警级别
errornum	-- 事件码
alerttime	-- 告警时间
status		-- 状态(产生或消除)
sensortype	-- 传感器类型
sensorname	-- 传感器名称
selrecord	-- 事件详细信息
*/

typedef struct
{
	guint16 id;
	guint8 level;
	gchar time_str[MAX_SEL_TIME_LEN];
	gchar sensorname[MAX_NAME_SIZE];
	gchar desc_str[EVENT_DESC_LENGTH];
	guint32 errcode;
	guint8 event_dir;
}SEL_RECORD_TXT_S;

/* BEGIN: Added for PN:告警优化 by l00205093, 2016/10/17 */
typedef struct
{
    gchar entity_name[MAX_NAME_SIZE + 16];
    void* json_obj_array;
}HEALTH_EVENT_NODE;
/* END:   Added by l00205093, 2016/10/17 */

/* BEGIN: Added for DTS2017051006775 by lwx282797, 2017/05/16 */
typedef struct
{
    gchar entity_name[MAX_NAME_SIZE + 16];
    void* json_obj;
}HEALTH_SUBTYPE_NODE;
/* END:   Added  by lwx282797, 2017/05/16 */

enum user_role_privilege
{
    USERROLE_KVMMGNT = 0x80,
    USERROLE_VMMMGNT = 0x40,
    USERROLE_POWERMGNT = 0x20,
    USERROLE_USERMGNT = 0x10,
    USERROLE_BASICSETTING = 0x08,
    USERROLE_SECURITYMGNT = 0x04,
    USERROLE_DIAGNOSEMGNT = 0x02,
    USERROLE_READONLY = 0x01
};

extern gint32 uip_web_get_object_property(const gchar * input, gchar ** output);
extern gint32 uip_web_call_method(const gchar * input, gchar ** output);
extern gint32 uip_web_call_method_ex(const gchar *username, const gchar *client,const gchar *session, const gchar * input, gchar ** output);
extern gint32 uip_web_get_objname_by_property(const gchar * input, gchar ** output);
extern gint32 uip_web_json_interface(gint32 func_id, const gchar * input, gchar ** output);
extern gint32 uip_start_packet_sel(gchar * user_name, gchar * client);
extern gint32 uip_get_packet_sel_progress(gint32* progress);
/* BEGIN : Modified for PN:DTS2015011201358   by l00227791, 2015/4/20*/
extern gint32 uip_get_privilege_from_session(const gchar* input, guchar* privilege, guint8* user_privilege, guint8 *local_user_id);
/* END : Modified for PN:DTS2015011201358   by l00227791, 2015/4/20*/
extern gint32 uip_get_privilege_from_redfish_session(const gchar* input, guchar* privilege, guint8* user_privilege, guint8 *local_user_id);
extern gint32 uip_module_init(const gchar* module_name);

#ifdef __cplusplus
}
#endif
#endif
