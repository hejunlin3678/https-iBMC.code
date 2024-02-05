#ifndef CLI_INC_IPMCSET_NETCONFIG_H
#define CLI_INC_IPMCSET_NETCONFIG_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>

#include "pme_app/pme_app.h"
#include "pme_app/uip/uip.h"
#include "pme_app/uip/uip_vnc.h"
#include "pme_app/uip/ipmc_public.h"
#include "pme_app/uip/ipmctypedef.h"
#include "pme_app/uip/uip_network_config.h"
#include "pme_app/smlib/sml_base.h"
#include "pme_app/smlib/sml_errcodes.h"
#include "pme/common/mscm_vos.h"
#include "pme/ipmi/common_ipmi.h"

#include "ipmcsetcommand.h"
#include "ipmcsethelp.h"
#include "ipmc_shelf_common.h"
#include "cli_sol.h"

guchar mask_judge(guint32 mask);
gint32 set_ip_address(guint8 group_id, const gchar *ipaddr);
gint32 set_ip_mask(guint8 group_id, const gchar *mask);
gint32 set_ip_gateway(guint8 group_id, const gchar *gateway);
gint32 ipmc_ipv6_addr_valid_check(gchar *ipv6Str, const gchar *ipv6_addr);
gint32 ipmc_eth_judge_ipv6_ip_gateway(gchar *ipv6_addr, gchar *ipv6_gateway, guchar prefix, guchar *is_same);
gint32 set_ipv6_address(guint8 group_id, const gchar *ipv6addr);
gint32 set_ipv6_prefix(guint8 group_id, guint8 prefixlen);
gint32 set_ipv6_gateway(guint8 group_id, const gchar *ipv6gateway);
gint32 get_double_certification_enable_flag(guchar *pflag);
gint32 ipmc_check_user_name(gchar *user_name);
gint32 _auth_user(const gchar *user_name, const gchar *pw, const gchar *ip_addr, guint8 login_type);
gint32 ipmc_get_user_password(gchar *user_password, size_t length);
gint32 ipmc_check_user_passwd(void);
gint32 set_backup_ipaddr_mask(guint8 group_id, const gchar *ipaddr, const gchar *mask);
gint32 check_sol_com_is_valid(guint8 com_id);
void print_sol_deactivate_usage(void);
gint32 eth_is_continue_operate(const gchar *string1, const gchar *string2);
gboolean check_ip_mask(const gchar *str_ip, const gchar *str_mask);
gint32 set_ip_mask_gateway_address(guint8 group_id, const gchar *ipaddr, const gchar *mask, const gchar *gateway);
gint32 _netport_switch_get_net_type_and_port(guchar argc, gchar **argv, gint32 *net_type, gint32 *net_port);
gint32 ipmc_judge_mntportcab_valiad(guint8 net_type_index);
void print_activeport_helpinfo(void);
gint32 ipmc_judge_netport_exist(guint8 net_type, guint8 port_num);
gint32 set_blade_bmc_ip_config_by_type(guchar slave_addr, void *ip_param, gchar *type);

gint32 delete_float_ip(void);
gint32 delete_float_ipv6(void);
gboolean check_ip_gateway(guint32 ip, guint32 mask, guint8 out_type);
void printf_error_info(gchar *location, gint32 ret);

#endif 