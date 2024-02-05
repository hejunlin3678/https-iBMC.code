/******************************************************************************

                  ç‰ˆæƒæ‰€æœ‰ (C), 2013-2018, åä¸ºæŠ€æœ¯æœ‰é™å…¬å¸

 ******************************************************************************
  æ–‡ ä»¶ å   : df_rpc.h
  ç‰ˆ æœ¬ å·   : åˆç¨¿
  éƒ¨    é—¨   : åä¸ºæŠ€æœ¯æœ‰é™å…¬å¸
  ä½œ    è€…   : èƒ¡éª‹ h00188524
  ç”Ÿæˆæ—¥æœŸ   : 2013å¹´7æœˆ16æ—¥
  æœ€è¿‘ä¿®æ”¹   :
  åŠŸèƒ½æè¿°   : rpcåº•å±‚æ¥å£
  å‡½æ•°åˆ—è¡¨   :

  ä¿®æ”¹å†å²   :
  1.æ—¥    æœŸ   : 2013å¹´7æœˆ16æ—¥
    ä½œ    è€…   : h00188524
    ä¿®æ”¹å†…å®¹   : åˆ›å»ºæ–‡ä»¶

******************************************************************************/
#ifndef __DF__RPC__H_
#define __DF__RPC__H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#include "glib.h"

/**
 * interface struct define
 *
 * RPCå‡½æ•°å‚æ•°æœ‰å››ç§ç±»å‹ï¼š
 * 1. åŸºæœ¬ç±»å‹ï¼Œint stringç­‰ï¼›
 * 2. å†…å­˜bufferï¼Œç»Ÿä¸€ç”±æŒ‡é’ˆ+é•¿åº¦æ¥è¡¨ç¤ºï¼›
 * 3. RPC_DATA_Sï¼Œé‡Œé¢å¸¦é•¿åº¦ï¼Œè¿™ä¸ªè·Ÿ2å…¶å®æœ‰ç‚¹é‡å¤ï¼›
 * 4. GSListï¼Œä»¥RPC_DATA_Sä¸ºnodeã€‚
 */
#define RPC_LIST_DATA_TYPE_SIZE 6
typedef struct tag_rpc_data
{
    gchar type[RPC_LIST_DATA_TYPE_SIZE];
    gsize size;
    guint8 data[0];
}RPC_DATA_S;

/**
 * get rpc path
 *
 */
extern const gchar* rpc_get_service_path(void);

/**
 * rpc dfcore interface
 *
 */
extern int rpc_start_server(gint32 port);
/**
 * rpc ping test
 */
typedef gint32 (*ping_ptr)(void);
/* server call */
extern gint32 reg_ping(ping_ptr func);
/* client call */
extern gint32 rpc_ping(gint32 port);


/**
 * rpc manager register module
 */
typedef gint32 (*register_module_ptr)(const char *module_name,
                                      pid_t pid,
                                      gssize *size,
                                      gpointer *reg_info);
/* server call */
extern gint32 reg_register_module(register_module_ptr func);
/* client call */
extern gint32 rpc_register_module(gint32 port,
                                  const char *module_name,
                                  pid_t pid,
                                  gssize size,
                                  gpointer reg_info);


/**
 * rpc manager register class
 */
typedef gint32 (*register_class_ptr)(const char *module_name,
                                     const char *class_name);
/* server call */
extern gint32 reg_register_class(register_class_ptr func);
/* client call */
extern gint32 rpc_register_class(gint32 port,
                                 const char *module_name,
                                 const char *class_name);


/**
 * rpc manager module register completed
 */
typedef gint32 (*module_register_completed_ptr)(const char *module_name);
/* server call */
extern gint32 reg_module_register_completed(module_register_completed_ptr func);
/* client call */
extern gint32 rpc_module_register_completed(gint32 port,
                                            const char *module_name);


/**
 * rpc manager get object list
 */
typedef gint32 (*get_object_list_ptr)(const char *module_name,
                                      GSList **obj_list);
/* server call */
extern gint32 reg_get_object_list(get_object_list_ptr func);
/* client call */
extern gint32 rpc_get_object_list(gint32 port,
                                  const char *module_name,
                                  GSList **obj_list);


/**
 * rpc manager get listener list
 */
typedef gint32 (*get_listener_list_ptr)(const char *module_name,
                                        GSList **listener_list);
/* server call */
extern gint32 reg_get_listener_list(get_listener_list_ptr func);
/* client call */
extern gint32 rpc_get_listener_list(gint32 port,
                                    const char *module_name,
                                    GSList **listener_list);
/**
 * rpc manager get route info
 */
typedef gint32 (*get_route_info_ptr)(const char *module_name,
                                     gssize *size,
                                     gpointer *route);
/* server call */
extern gint32 reg_get_route_info(get_route_info_ptr func);
/* client call */
extern gint32 rpc_get_route_info(gint32 port,
                                 const char *module_name,
                                 gssize size,
                                 gpointer route);


/**
 * rpc manager get new object
 */
typedef gint32 (*get_new_object_ptr)(const char *module_name,
                                     const gchar* object_name,
                                     gssize *size,
                                     gpointer *object_info);
/* server call */
extern gint32 reg_get_new_object(get_new_object_ptr func);
/* client call */
extern gint32 rpc_get_new_object(gint32 port,
                                 const char *module_name,
                                 const gchar* object_name,
                                 gssize size,
                                 gpointer object_info);


/**
 * rpc manager listen class event
 */
typedef gint32 (*listen_class_event_ptr)(const char *module_name,
                                         const gchar* class_name,
                                         const gchar* property_name);
/* server call */
extern gint32 reg_listen_class_event(listen_class_event_ptr func);
/* client call */
extern gint32 rpc_listen_class_event(gint32 port,
                                     const char *module_name,
                                     const gchar* class_name,
                                     const gchar* property_name);


/**
 * rpc manager get class tree
 */
typedef gint32 (*get_class_tree_prt)(const char *module_name,
                                     gpointer *class_tree);
/* server call */
extern gint32 reg_get_class_tree(get_class_tree_prt func);
/* client call */
extern gint32 rpc_get_class_tree(gint32 port,
                                 const char *module_name,
                                 gpointer *class_tree);


/**
 * rpc manager set_property_value
 */
typedef gint32 (*set_property_value_ptr)(const char *objetct_name,
                                         const char *property_name,
                                         const RPC_DATA_S *property_value);
/* server call */
extern gint32 reg_set_property_value(set_property_value_ptr func);
/* client call */
extern gint32 rpc_set_property_value(gint32 port,
                                     const char *objetct_name,
                                     const char *property_name,
                                     const RPC_DATA_S *property_value);



/*
 * rpc app interface
 *
 */
/**
 * rpc listen object event
 */
typedef gint32 (*listen_object_event_ptr)(const char *module_name,
                                          const char *object_name,
                                          const char *property_name,
                                          gint32 ipc_que_id);
/* server call */
extern gint32 reg_listen_object_event(listen_object_event_ptr func);
/* client call */
extern gint32 rpc_listen_object_event(gint32 port,
                                      const char *module_name,
                                      const char *object_name,
                                      const char *property_name,
                                      gint32 ipc_que_id);


/**
 * rpc call module method
 */
typedef gint32 (*call_module_method_ptr)(const char *module_name,
                                         const char *method_name,
                                         GSList *input_list,
                                         GSList **output_list);
/* server call */
extern gint32 reg_call_module_method(call_module_method_ptr func);
/* client call */
extern gint32 rpc_call_module_method(gint32 port,
                                     const char *module_name,
                                     const char *method_name,
                                     GSList *input_list,
                                     GSList **output_list);


/**
 * rpc call class method
 */
typedef gint32 (*call_class_method_ptr)(const char *class_name,
                                        const char *method_name,
                                        GSList *input_list,
                                        GSList **output_list,
                                        GSList* user_data_list);
/* server call */
extern gint32 reg_call_class_method(call_class_method_ptr func);
/* client call */
extern gint32 rpc_call_class_method(gint32 port,
                                    const char *class_name,
                                    const char *method_name,
                                    GSList *input_list,
                                    GSList **output_list,
                                    GSList* user_data_list);


/**
 * rpc reboot
 */
typedef gint32 (*reboot_ptr)(const char *module_name, gint32 ctrl);
/* server call */
extern gint32 reg_reboot(reboot_ptr func);
/* client call */
extern gint32 rpc_reboot(gint32 port, const char *module_name, gint32 ctrl);


/**
 * rpc register ipmicmd
 */
typedef gint32 (*register_ipmicmd_ptr)(const gchar *module_name,
                                       const RPC_DATA_S *value);
/* server call */
extern gint32 reg_register_ipmicmd(register_ipmicmd_ptr func);
/* client call */
extern gint32 rpc_register_ipmicmd(gint32 rpc_port,
                                   const gchar *module_name,
                                   const RPC_DATA_S *value);


/**
 * rpc get ipmicmd table
 */
typedef gint32 (*get_ipmicmd_table_ptr)(const gchar *module_name, gssize *size, gpointer *cmd_table);
/* server call */
extern gint32 reg_get_ipmicmd_table(get_ipmicmd_table_ptr func);
/* client call */
extern gint32 rpc_get_ipmicmd_table(gint32 rpc_port, const gchar *module_name, gssize *size, gpointer *cmd_table);


/**
 * rpc call class method
 */
typedef gint32 (*send_ipmi_request_ptr)(gsize req_size,
                                     gconstpointer request,
                                     gsize *resp_size,
                                     gpointer *response,
                                     gboolean wait_response);
/* server call */
extern gint32 reg_send_ipmi_request(send_ipmi_request_ptr func);
/* client call */
extern gint32 rpc_send_ipmi_request(gint32 rpc_port,
                                    gsize req_size,
                                    gconstpointer request,
                                    gsize resp_size,
                                    gpointer response,
                                    gboolean wait_response);


/**
 * rpc send ipmi response
 */
typedef gint32 (*send_ipmi_response_ptr)(gsize resp_size, gconstpointer response);
/* server call */
extern gint32 reg_send_ipmi_response(send_ipmi_response_ptr func);
/* client call */
extern gint32 rpc_send_ipmi_response(gint32 rpc_port, gsize resp_size, gconstpointer response);


/**
 * rpc proxy ipmi
 */
typedef gint32 (*proxy_ipmi_ptr)(gsize req_size, gconstpointer request);
/* server call */
extern gint32 reg_proxy_ipmi(proxy_ipmi_ptr func);
/* client call */
extern gint32 rpc_proxy_ipmi(gint32 rpc_port, gsize req_size, gconstpointer request);


/**
 * rpc get module list
 */
typedef gint32 (*get_module_list_ptr)(GSList **list);
/* server call */
extern gint32 reg_get_module_list(get_module_list_ptr func);
/* client call */
extern gint32 rpc_get_module_list(gint32 rpc_port, GSList **list);


/**
 * rpc heartbeat
 */
typedef gint32 (*heartbeat_ptr)(const gchar *module_name);
/* server call */
extern gint32 reg_heartbeat(heartbeat_ptr func);
/* client call */
extern gint32 rpc_heartbeat(gint32 rpc_port, const gchar *module_name);


/**
 * rpc get debug cmdlist
 */
typedef gint32 (*get_debug_cmdlist_ptr)(const gchar *module_name, GSList **cmd_list);
/* server call */
extern gint32 reg_get_debug_cmdlist(get_debug_cmdlist_ptr func);
/* client call */
extern gint32 rpc_get_debug_cmdlist(gint32 rpc_port, const gchar *module_name, GSList **cmd_list);


/**
 * rpc call debug cmd
 */
typedef gint32 (*call_debug_cmd_ptr)(const gchar *module_name, const gchar *cmd_name, GSList *input_list);
/* server call */
extern gint32 reg_call_debug_cmd(call_debug_cmd_ptr func);
/* client call */
extern gint32 rpc_call_debug_cmd(gint32 rpc_port, const gchar *module_name, const gchar *cmd_name, GSList *input_list);


/**
 * rpc get debug cmd paralist
 */
typedef gint32 (*get_debug_cmd_paralist_ptr)(const gchar *module_name, const gchar *cmd_name, GSList **para_list);
/* server call */
extern gint32 reg_get_debug_cmd_paralist(get_debug_cmd_paralist_ptr func);
/* client call */
extern gint32 rpc_get_debug_cmd_paralist(gint32 rpc_port, const gchar *module_name, const gchar *cmd_name, GSList **para_list);


/**
 * rpc get module method paralist
 */
typedef gint32 (*get_module_method_paralist_ptr)(const gchar *module_name, const gchar *method_name, GSList **para_list);
/* server call */
extern gint32 reg_get_module_method_paralist(get_module_method_paralist_ptr func);
/* client call */
extern gint32 rpc_get_module_method_paralist(gint32 rpc_port, const gchar *module_name, const gchar *method_name, GSList **para_list);


/**
 * rpc dump
 */
typedef gint32 (*dump_ptr)(const gchar *dump_path);
/* server call */
extern gint32 reg_dump(dump_ptr func);
/* client call */
extern gint32 rpc_dump(gint32 rpc_port, const gchar *dump_path);


/**
 * rpc register ipmi module
 */
typedef gint32 (*register_ipmi_module_ptr)(const gchar *module_name, gint32 ipmi_port);
/* server call */
extern gint32 reg_register_ipmi_module(register_ipmi_module_ptr func);
/* client call */
extern gint32 rpc_register_ipmi_module(gint32 port, const gchar *module_name, gint32 ipmi_port);


/**
 * rpc register dft item
 */
typedef gint32 (*register_dft_item_ptr)(gsize size, gpointer data);
/* server call */
extern gint32 reg_register_dft_item(register_dft_item_ptr func);
/* client call */
extern gint32 rpc_register_dft_item(gint32 port, gsize size, gpointer data);


/**
 * rpc dft control
 */
typedef gint32 (*dft_control_ptr)(gint32 index, gint32 command, gsize para_size, gconstpointer para_data,
    gsize *size, gpointer *data_out);
/* server call */
extern gint32 reg_dft_control(dft_control_ptr func);
/* client call */
extern gint32 rpc_dft_control(gint32 port, gint32 index, gint32 command,
    gsize size_in, gconstpointer data_in,
    gsize size_out, gpointer data_out);


/**
 * rpc register dft module
 */
typedef gint32 (*register_dft_module_ptr)(const gchar *module_name, gint32 port);
/* server call */
extern gint32 reg_register_dft_module(register_dft_module_ptr func);
/* client call */
extern gint32 rpc_register_dft_module(gint32 port, const gchar *module_name, gint32 dft_port);


/** * rpc get remote property value */
typedef gint32 (*get_remote_property_value_ptr)(const char *object_name,  GSList *input_list,  GSList **output_list);
typedef gint32 ( *call_remote_class_method_ptr)( const gchar* object_name, const gchar* method_name, GSList* input_list, GSList** output_list, GSList* user_data_list);
typedef gint32 (*access_remote_bus_ptr)(GSList *input_list,  GSList **output_list);
/* server call */
extern gint32 reg_get_remote_property_value(get_remote_property_value_ptr func );
extern gint32 reg_call_remote_class_method( call_remote_class_method_ptr func );
extern void static_start_remote_proxy_service(void *p);
extern void static_start_remote_proxy_service2(void *p);
extern gint32 reg_access_remote_bus( access_remote_bus_ptr func );
/* client call */
extern gint32 rpc_get_remote_property_value(gint32 port,   const char *object_name, GSList *input_list, GSList **output_list );
extern gint32 rpc_call_remote_class_method(gint32 port,const char *object_name,const char *method_name,  GSList *input_list,GSList **output_list, GSList *user_data_list);
extern gint32 rpc_get_remote_property_value2(const char *ip, gint32 port,   const char *object_name, GSList *input_list, GSList **output_list );
extern gint32 rpc_call_remote_class_method2(const char *ip, gint32 port,const char *object_name,const char *method_name,  GSList *input_list,GSList **output_list, GSList *user_data_list);
extern gint32 rpc_access_remote_bus(const gchar *ip4_addr, gint32 port, GSList *cmd_list, GSList *out_list);

/* BEGIN: Added by licheng, 2015/8/15 20:38:5   ÎÊÌâµ¥ºÅ:DTS2015073105547
   Ìá¹©Ô¶³Ì·ÃÎÊ³Ö¾Ã»¯Êı¾İ¹¦ÄÜ */
/**
 * rpc manager save_property_value
 */
typedef gint32 (*save_property_value_ptr)(const gchar *object_name,
                               const gchar *property_name,
                               const gchar *type,
                               guint32 size,
                               const guint8 *data,
                               gboolean is_default,
                               gint32 per_mode);

/* server call */
extern gint32 reg_save_property_value(save_property_value_ptr func);
/* client call */
extern gint32 rpc_save_property_value(gint32 port,
                               const gchar *object_name,
                               const gchar *property_name,
                               const gchar *type,
                               guint32 size,
                               const guint8 *data,
                               gboolean is_default,
                               gint32 per_mode);

typedef gint32 (*get_saved_value_ptr)(const gchar *object_name,
                               const gchar *property_name,
                               RPC_DATA_S **data,
                               gboolean is_default,
                               gint32 per_mode);
extern gint32 reg_get_saved_value(get_saved_value_ptr func);
extern gint32 rpc_get_saved_value(gint32 port,
                                const gchar* object_name,
                                const gchar* property_name,
                                RPC_DATA_S **data,
                                gboolean is_default,
                                gint32 per_mode);
/* END:   Added by licheng, 2015/8/15 20:38:28 */

/* BEGIN: Added by lingmingqiang, 2016/8/31   ÎÊÌâµ¥ºÅ:DTS2016083105934
    ½µÈ¨ÏŞĞèÇó,Ìá¹©·ÇrootÈ¨ÏŞapp·ÃÎÊÓ²¼şµÄ´úÀíµ÷ÓÃ·½Ê½*/
/**
 * rpc manager get_property_value
 */
typedef gint32 (*get_property_value_ptr)(const char *objetct_name,
                                         const char *property_name,
                                         gint32 offset,
                                         guint32 length,
                                         RPC_DATA_S **data);
/* server call */
extern gint32 reg_get_property_value(get_property_value_ptr func);
/* client call */
extern gint32 rpc_get_property_value(gint32 port,
                                     const char *objetct_name,
                                     const char *property_name,
                                     gint32 offset,
                                     guint32 length,
                                     RPC_DATA_S **data);
/* END:   Added by lingmingqiang, 2016/8/31 */

/**
 * rpc manager manage_module
 */
typedef gint32 (*manage_module_ptr)(const char *module_name,
                                      guint32 options);
/* server call */
extern gint32 reg_manage_module(manage_module_ptr func);
/* client call */
extern gint32 rpc_manage_module(gint32 port,
                                  const char *module_name,
                                  guint32 options);

/**
 * rpc config timeout
 */
/* server call */
extern void rpc_set_timeout(guint32 timeout);
/* client call */
extern guint32 rpc_get_timeout(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif/* __DF__RPC__H_ */
