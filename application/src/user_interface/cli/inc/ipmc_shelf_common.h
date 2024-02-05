#ifndef IPMC_SHELF_COMMON_H
#define IPMC_SHELF_COMMON_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define SHELF_MGMT_HELP_DEFAULT_NAME ("[-l smm] ")
#define SHELF_MGMT_DEFAULT_NAME ("smm")
#define PRODUCT_TYPE_IBMC_NAME ("iBMC")
#define PRODUCT_TYPE_SMM_NAME ("SMM")
#define HELP_DEFAULT_NAME_LEN 32
#define SHELF_PART_PRESENCE 1
#define MAX_CLI_CMD_LENGTH 128
#define MAX_CLI_ARGV_LENGTH 1024

#define NEED_L_SHELF ("-l shelf ")
#define NEED_L_SHELF_LEN 32

#define TASK_FINISH 100



#define PEM_TYPE_AC 0
#define PEM_TYPE_DC 1
#define PEM_TYPE_AC_AND_DC 2

#define PEM_CLOSED_MANULLY (-2) 
#define PEM_RETURN_ERROR (-1)
#define PEM_RETURN_OK 0
#define PEM_CONFIG_INVALID 1         
#define POWER_ALERT_DISABLED 1       
#define PEM_COMMUNICATION_LOST 2     
#define PEM_NOT_PRESENT 3            
#define PEM_GET_SOFT_VER_FAIL 5      
#define PEM_GET_HARD_VER_FAIL 6      
#define PEM_GET_SOFT_HARD_VER_FAIL 7 
#define PEM_RESET_LESS_THAN_TIMEINT 8 

#define PS_SWITCH_OPEN 0  
#define PS_SWITCH_CLOSE 1 
#define PS_SWITCH_RESET 2 
#define PEM_CHANNEL_TYPE_NONE 0 
#define PEM_CHANNEL_TYPE_CPLD 1 
#define PEM_CHANNEL_TYPE_I2C 2 

#define MSM_PRIMARY_STATE          1
#define MSM_SECONDARY_STATE        0

#define MAX_REASON_STR_LEN 20



#define PS_REMOVEED_INFO "PsRemovedInfo"
#define FAN_REMOVEED_INFO "FanRemovedInfo"
#define BLADE_REMOVEED_INFO "BladeRemovedInfo"
#define SWI_REMOVEED_INFO "SwiRemovedInfo"
#define SMM_REMOVEED_INFO "SMMRemovedInfo"
#define PS_REMOVEED_POLICY "PsRemovedPlcy"
#define FAN_REMOVEED_POLICY "FanRemovedPlcy"
#define BLADE_REMOVEED_POLICY "BladeRemovedPlcy"
#define SWI_REMOVEED_POLICY "SwiRemovedPlcy"
#define SMM_REMOVEED_POLICY "SMMRemovedPlcy"


typedef struct ps_channel_action {
    guint8 channel_type;
    guint8 action_type;
} PS_CHANNEL_ACTION;

extern gchar help_default_name[HELP_DEFAULT_NAME_LEN];
extern gchar product_type_name[HELP_DEFAULT_NAME_LEN];
extern gchar need_l_shelf[NEED_L_SHELF_LEN];

guchar ipmc_need_l(void);
gint32 ipmc_find_matchlocation(gchar *location, guint32 size);
gchar init_cli_black_cmd_list_1_level(gchar *cmd_buf, struct helpinfo *cmd_list);
gchar init_cli_black_cmd_list_2_level(gchar *cmd_buf, struct helpinfo *cmdlist, struct helpinfoplus *cmdlist_plus);
gint32 ipmc_find_l_t_d_cmd(gchar *location, gchar *target, gchar *cmd, gint32 argc, gchar **argv);
gint32 ipmc_find_l_d_cmd(gchar *location, gchar *cmd, gint32 argc, gchar **argv);
void ipmc_print_helpinfo(guchar need_l, guchar l_level, gchar *location, guchar t_level, gchar *target);
gint32 ipmc_find_match_l_target(gchar *location, gchar *target);
void list_special_item(guint64 flag, struct helpinfo *item);
void print_cmd_list(struct helpinfo *cmd_list);
void custom_print_cmd_list(struct helpinfo* cmd_list);
void print_shelf_cli_helpinfo(void);
gint32 get_ipmbeth_blade_name(guint8 slave_addr, gchar *smm_name, guint8 name_len);
gboolean is_support_bios_configuration(void);
gint32 check_blade_present_communication_status(gchar *location);
guchar ipmc_get_blade_num(gchar *plocation);
guchar ipmc_get_blade_ipmbaddr(guchar num);
gint32 get_blade_presence(OBJ_HANDLE object_handle);
void ipmc_print_d_helpinfo(guchar l_level, gchar *location, guchar t_level, gchar *target);
gint32 cli_system_lockdown_check(guint8 allow);
extern gint32 ascend_mode_info(OBJ_HANDLE obj_handle, gpointer data);
gchar **ipmc_clear_sensitive_info(gint32 argc, gchar **argv);
gint32 get_username_ip(gchar *username, gchar *ip, guint32 ip_size);
gint32 upload_file_to_url(const gchar *url, guint8 file_id);
gint32 ipmc_call_method_user_info(const gchar *interface, const gchar *username, const gchar *client,
    const gchar *class_name, const gchar *obj_name, const gchar *method_name, GSList *input_list, GSList **output_list);
gint32 ipmc_get_local_uid_gid(uid_t *uid, gid_t *gid);
gint32 ipmc_change_file_owner(const gchar *file_path);
void ipmc_get_lsw_print(gchar *tmp_buf);
gint32 print_blade_bmc_ip_info(guint8 slave_addr);
gint32 _get_outer_eth_ipaddr_mask(gchar *ipaddr, guint32 iplen, gchar *mask, guint32 masklen);
gint32 parse_sol_login_time(guint32 time_value, gchar *time_buf, gsize buf_len);
gboolean is_control_by_other_node(guint8 *out_ctrl_node);

gint32 ipmc_get_matched_cmd(gchar *cmd_buf, struct helpinfo *cmd_list);
void set_g_bmc_cmd_list1(struct helpinfo *list);
void set_bmc_cmd_list_l(struct helpinfo *list);
void set_bmc_l_smm_t(struct helpinfo *list);
void set_bmc_l_smm_d(struct helpinfo *list);
void set_bmc_cmd_list_l_t_bond(struct helpinfoplus *list);
void set_bmc_cmd_list_l_d_bond(struct helpinfoplus *list);
void set_bmc_l_t_d_bond(struct helpinfoplus_plus *list);
void set_bmc_cmd_list2(struct helpinfo *list);
void set_bmc_cmd_list_d(struct helpinfoplus *list);
typedef void (*helpinfo_func)(guchar level, gchar *target);
void set_ipmc_helpinfo_func(helpinfo_func fun);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
