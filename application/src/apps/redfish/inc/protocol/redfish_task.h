

#ifndef __REDFISH_TASK_H__
#define __REDFISH_TASK_H__

#include "redfish_util.h"

#include <glib.h>
#include <json.h>

#ifdef _cplusplus
extern "C" {
#endif

#define TASK_COLLECTION_PATH "/redfish/v1/TaskService/Tasks/"
#define TASK_CLASS "Task"
#define RF_ORIGIN_OBJ "OriginObj"

#define MAX_TASK_NAME_LEN 64
#define MAX_TASK_STATUS_LEN 64
#define TASK_NO_PROGRESS 255

#define REDFISH_TASK_INTERVAL_IN_SEC 2

typedef enum tag_task_state {
    RF_TASK_NEW = 0,
    RF_TASK_STARTING,
    RF_TASK_RUNNING,
    RF_TASK_SUSPENDED,
    RF_TASK_INTERRUPTED,
    RF_TASK_PENDING,
    RF_TASK_STOPPING,
    RF_TASK_COMPLETED,
    RF_TASK_KILLED,
    RF_TASK_EXCEPTION,
    RF_TASK_SERVICE
} REDFISH_TASK_STATE;


#define TASK_INITIAL_TMOUT 600


typedef struct tag_task_monitor_info {
    
    REDFISH_TASK_STATE task_state;
    
    guchar task_progress;

    
    guint8 rsc_privilege;

    
    gchar task_owner[USER_USERNAME_MAX_LEN + 1];

    
    void *user_data;

    
    GDestroyNotify user_data_destory_func;

    
    gchar create_resourse[MAX_URI_LENGTH];

    
    json_object *response_header;

    
    json_object *extended_info;

    
    OBJ_HANDLE ldap_obj_handle;
} TASK_MONITOR_INFO_S;

typedef gint32 (*task_status_monitor_fn)(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj);

typedef struct tag_task_rsc {
    
    gchar task_id[MAX_RSC_ID_LEN];

    
    gchar task_name[MAX_TASK_NAME_LEN];

    
    REDFISH_TASK_STATE task_state;

    
    task_status_monitor_fn status_monitor_fn;

    
    json_object *param_jso;

    
    TASK_MONITOR_INFO_S *monitor_fn_data;

    
    gchar origin_rsc_path[MAX_URI_LENGTH];

    
    gchar start_time[MAX_DATETIME_LEN];

    
    gchar end_time[MAX_DATETIME_LEN];

    
    gchar status[MAX_TASK_STATUS_LEN];

    json_object *message_array_jso;

    
    gboolean stop_flag;

    
    guint32 alive_time;

    
    guchar task_progress;
} TASK_RSC_S;



extern gint32 create_new_task(const char *task_name, task_status_monitor_fn status_monitor_fn,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object *param_jso, const char *origin_obj_path, json_object **task_obj);




extern int redfish_task_init(void);



extern json_object *get_exist_running_task(task_status_monitor_fn status_monitor_fn, json_object *param_jso,
    const char *origin_rsc_path);


extern json_object *pack_task_rsc(TASK_RSC_S *task_info);

json_object *get_task_rsc_from_id(int task_id, guint8 *task_priv, const gchar **task_owner);

json_object *get_task_monitor_need_info(int task_id, guint8 *task_priv, const gchar **task_owner);

extern TASK_RSC_S *get_task_rsc_structure(gint32 task_id);

extern json_object *pack_task_monitor_data(TASK_RSC_S *task_info);

extern gboolean is_task_exist(int task_id);

extern GSList *get_cur_task_list(void);

extern gint32 find_available_task_id(void);

extern TASK_MONITOR_INFO_S *task_monitor_info_new(GDestroyNotify destroy_func);

extern void task_monitor_info_free(TASK_MONITOR_INFO_S *task_monitor_info);

extern gboolean check_redfish_running_task_exist(task_status_monitor_fn status_monitor_fn, json_object *param_jso,
    const gchar *origin_rsc_path, gint32 *task_id);
gboolean check_export_task_exist(task_status_monitor_fn status_monitor_fn, json_object *param_jso,
    const gchar *origin_rsc_path, gint32 *task_id);
#ifdef _cplusplus
}
#endif

#endif 
