#ifndef IPMCGETHELP_H
#define IPMCGETHELP_H

#include "pme/pme.h"
#include "pme_app/pme_app.h"

#define BIOS_DEGUG_INFO_ENABLE 1
#define BIOS_DEGUG_INFO_BY_SETUP_MENU 0
#define G_BMC_GETCMD_FRU0 6
struct helpinfo g_bmc_getcmd_fru0[G_BMC_GETCMD_FRU0];

gint32 get_username_ip(gchar *username, gchar *ip, guint32 ip_size);
void ipmc_helpinfo(guchar level, gchar *target);
gint32 ipmc_find_matchtarget(const gchar *target);
gint32 ipmc_find_cmd(gchar *target, gchar *cmd, gint32 argc, gchar **argv);
gint32 ipmc_find_matchcmd(gchar *cmd, gint32 argc, gchar **argv);
void ipmc_list_data(gchar *target);
gint64 ipmc_match_str(const gchar *str, struct helpinfo *cmd_list);
void printf_sel_helpinfo(void);
void print_download_usage(void);

void print_export_config_usage(void);

void print_ipmcget_syslog_dest_state(void);
void print_ipmcget_trap_dest_state(void);

struct helpinfo *get_g_bmc_cmd_list1(void);
struct helpinfo *get_g_bmc_cmd_list2(void);
struct helpinfoplus *get_g_bmc_cmd_list_d(void);
struct helpinfo *get_g_bmc_cmd_list_l(void);
struct helpinfoplus *get_g_bmc_cmd_list_l_d_bond(void);
struct helpinfoplus_plus *get_g_bmc_l_t_d_bond(void);
struct helpinfoplus *get_g_bmc_cmd_list_l_t_bond(void);
struct helpinfo *get_g_bmc_l_smm_t(void);
struct helpinfo *get_g_bmc_l_smm_d(void);

#endif
