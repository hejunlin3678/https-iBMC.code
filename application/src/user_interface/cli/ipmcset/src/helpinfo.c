

#include <stdlib.h>
#include <pwd.h>
#include "pme/pme.h"
#include "ipmctypedef.h"
#include "pme_app/pme_app.h"
#include "pme_app/uip/uip.h"
#include "pme_app/uip/ipmc_public.h"
#include "pme_app/smlib/sml_base.h"
#include "ipmcset_sys_mgmt.h"


#include "ipmcsethelp.h"
#include "ipmcsetcommand.h"
#include "shelfsetcommand.h"
#include "ipmc_shelf_common.h"
#include "ipmcsetmain.h"


void print_shelf_cli_helpinfo(void)
{
    g_printf("Usage: ipmcset [-l location] [-t target] -d dataitem [-v value]\r\n");
}


void ipmc_helpinfo(guchar level, gchar *target)
{
    guint32 i = 0;
    gint32 match_count = 0;
    guint64 record = 0;
    gint64 return_record = 0;

    if (!level) {
        g_printf("Usage: ipmcset [-t target] -d dataitem [-v value]\r\n");
        g_printf("    -t <target>\r\n");
        print_cmd_list(get_g_bmc_cmd_list2());
        g_printf("\r\n    -d <dataitem>\r\n");
        custom_print_cmd_list(get_g_bmc_cmd_list1());
    } else {
        if (target == NULL) {
            g_printf("    -t <target>\r\n");
            print_cmd_list(get_g_bmc_cmd_list2());
        } else {
            
            return_record = ipmc_match_str(target, get_g_bmc_cmd_list2());
            if (return_record != IPMC_ERROR) {
                record = return_record;
                match_count = 1;
            } else {
                
                record = 0;

                while (get_g_bmc_cmd_list2()[i].name && target) {
                    if (!strncmp(target, get_g_bmc_cmd_list2()[i].name, strlen(target))) {
                        record |= ((guint64)1 << i);
                        match_count++;
                    }

                    i++;
                }
            }

            if (!match_count) {
                g_printf("    -t <target>\r\n");
                print_cmd_list(get_g_bmc_cmd_list2());
            }

            
            if (match_count == 1) {
                g_printf("\r\n    -d <dataitem>\r\n");
                i = 0;
                while (record) {
                    if (record & ((guint64)1 << i)) {
                        print_cmd_list(get_g_bmc_cmd_list_d()[i].help_info);
                        break;
                    }

                    i++;
                }
            }

            
            if (match_count > 1) {
                g_printf("    -t <target>\r\n");
                list_special_item(record, get_g_bmc_cmd_list2());
            }
        }
    }
}


void print_vnctimeout_usage(void)
{
    g_printf("Usage: ipmcset -t vnc -d timeout -v <value>\r\n");
    g_printf("Value:   \r\n");
    g_printf("    0       No limit\r\n");
    g_printf("    1-480   VNC timeout period(min)\r\n");
}


void print_fanmode_usage(void)
{
    g_printf("Usage: ipmcset -d fanmode -v <mode> [timeout]\r\n");
    g_printf("Modes are:\r\n");
    g_printf("    0    auto regulate fan speed, without parameter [timeout]\r\n");
    g_printf("    1    manual regulate fan speed\r\n");
    g_printf("         without parameter [timeout], default timeout(30 seconds)\r\n");
    g_printf("         with parameter [timeout], timeout range <0-100000000>,(0 means max timeout)\r\n");
}


void print_fanlevel_usage(guint8 min_fan_slot, guint8 max_fan_slot)
{
    g_printf("Usage: ipmcset -d fanlevel -v <value> [fanid]\r\n");
    g_printf("Note :\r\n");
    g_printf("      1.Set fan speed level for all fans, without parameter [fanid]\r\n");
    g_printf("      2.Set fan speed level for specific fan, with parameter [fanid]\r\n");
    g_printf("       [fanid] range <%d-%d>\r\n", min_fan_slot, max_fan_slot);
}

void print_biosprotect_usage(void)
{
    g_printf("Usage: ipmcset -d bioswriteprotect -v <option>\r\n");
    g_printf("Options are:\r\n");
    g_printf("    1    BIOS write protect enable\r\n");
    g_printf("    0    BIOS write protect disable\r\n");
}

void print_watchdog_usage(void)
{
    g_printf("Usage: ipmcset -d wdt -v <option>\r\n");
    g_printf("Options are:\r\n");
    g_printf("    0    Start watchdog timer\r\n");
    g_printf("    1    Stop watchdog timer\r\n");
}


void print_fru_control(void)
{
    g_printf("Usage: ipmcset -d frucontrol -v <option>\r\n");
    g_printf("Options are:\r\n");
    g_printf("    0    Forced System Reset\r\n");
    g_printf("    2    Forced Power Cycle\r\n");
    if (dal_is_pangea_enterprise_board()) {
        g_printf("    4    Power Cycle\r\n");
    }
}

void print_power_state_usage(void)
{
    g_printf("Usage: ipmcset -d powerstate -v <option>\r\n");
    g_printf("Options are:\r\n");
    g_printf("    0    Normal Power Off, deactivate FRU\r\n");
    g_printf("    1    Power on, activate FRU\r\n");
    g_printf("    2    Forced Power Off, deactivate FRU\r\n");
}


void print_shelf_power_control_usage(void)
{
    g_printf("Usage: ipmcset -d shelfpowercontrol -v <option>\r\n");
    g_printf("Options are:\r\n");
    g_printf("    0    Normal Power Off, deactivate shelf\r\n");
    g_printf("    1    Power on, activate shelf\r\n");
}

void print_power_button_mode_usage(void)
{
    g_printf("Usage: ipmcset -d powerbuttonmode -v <option>\r\n");
    g_printf("Options are:\r\n");
    g_printf("    0    Restore the default mode\r\n");
}

void print_set_ledstate_usage(void)
{
    g_printf("Usage: ipmcset -t led -d ledstate -v <ledname> <option>"
        "[value]...\r\n");
    g_printf("\nOptions are:\r\n");
    g_printf("off                                               LED off\r\n");
    g_printf("on  <color>                                       LED on\r\n");

    g_printf("blink \r\n");
    g_printf("lamptest <on duration> <color>                    LAMP TEST, Input ledname with "
        "                                                  \"ledall\" lamp all leds.       "
        "                                                  (duration range <0-127>,      "
        "                                                   units is 100ms)\r\n");
    g_printf("localcontrol <color>                              Local Control\r\n");
    g_printf("\nColors are:\n");
    g_printf("    1         Use BLUE\n");
    g_printf("    2         Use RED\n");
    g_printf("    3         Use GREEN\n");
    g_printf("    4         Use AMBER\n");
    g_printf("    5         Use ORANGE\n");
    g_printf("    6         Use WHITE\n");
    g_printf("    0xE       Do not change\n");
    g_printf("    0xF       Use default color\n");
}

void print_userprivilege_usage(void)
{
    g_printf("Usage: ipmcset %s-d privilege -v <username> <privilege>\r\n", help_default_name);
    g_printf("Privileges are:\r\n");
    g_printf("    15       No Access\r\n");
    g_printf("    2        User\r\n");
    g_printf("    3        Operator\r\n");
    g_printf("    4        Administrator\r\n");
    
    g_printf("    5        Custom Role1\r\n");
    g_printf("    6        Custom Role2\r\n");
    g_printf("    7        Custom Role3\r\n");
    g_printf("    8        Custom Role4\r\n");
    
}

void print_workmode_usage(void)
{
    g_printf("Usage: ipmcset %s-d npuworkmode -v <option> \r\n", help_default_name);
    g_printf("Options are:\r\n");
    g_printf("    0        AMP mode\r\n");
    g_printf("    1        SMP mode\r\n");
}

void print_poweroninterval_usage(void)
{
    g_printf("Usage: ipmcset %s-d poweroninterval -v <value>\r\n", help_default_name);
    g_printf("value : the range is 500 ~ 20000.\r\n");
    return;
}

void print_bladepowercontrol_usage(void)
{
    g_printf("Usage: ipmcset %s-d bladepowercontrol -v <option>\r\n", help_default_name);
    g_printf("Options are:\r\n");
    g_printf("    0    disable \r\n");
    g_printf("    1    enable \r\n");
    return;
}

void print_set_stack_state_usage(void)
{
    g_printf("Usage: ipmcset %s-d stackstate -v <value>\r\n", help_default_name);
    g_printf("Options are:\r\n");
    g_printf("    disable \r\n");
    g_printf("    enable \r\n");
    return;
}

void print_set_outport_mode_usage(void)
{
    g_printf("Usage: ipmcset %s-d outportmode -v <value>\r\n", help_default_name);
    g_printf("Values are:\r\n");
    g_printf("    0    out port mode from switch \r\n");
    g_printf("    1    out port mode from smm \r\n");
    return;
}

void print_blade_cooling_medium_usage(void)
{
    g_printf("Usage: ipmcset %s-d coolingmode -v <option>\r\n", help_default_name);
    g_printf("Options are:\r\n");
    g_printf("    0    air cooling \r\n");
    g_printf("    1    liquid cooling \r\n");
    return;
}


void print_netmode_usage(gboolean is_mm)
{
    if (is_mm) { // 兼容MM版设置net mode
        g_printf("Usage: ipmcset -l bladeN -d netmode -v <option>\r\n");
    } else {
        g_printf("Usage: ipmcset -d netmode -v <option>\r\n");
    }
    g_printf("Options are:\r\n");
    g_printf("    1    Manual \r\n");
    g_printf("    2    Adaptive \r\n");
    return;
}

void print_upgrade_usage(void)
{
    guint8 bmc_valid_mode = get_bmc_valid_mode();
    if (bmc_valid_mode == BMC_MANUAL_VALID) {
        g_printf("Usage: ipmcset %s-d upgrade -v <filepath> [option]\r\n", help_default_name);
        g_printf("Filepath      e.g.: /tmp/image.hpm\r\n");

        if (dal_check_if_mm_board() == TRUE) {
            g_printf("Note:\r\n");
            g_printf("    1. %s need to reboot manually after %s is upgraded.\r\n",
                product_type_name, product_type_name);
        } else {
            g_printf("Options are:\r\n");
            g_printf("    1: %s will reboot after %s is upgraded(default).\r\n", product_type_name, product_type_name);
            g_printf("    0: %s need to reboot manually after %s is upgraded.\r\n",
                product_type_name, product_type_name);
            g_printf("Note:\r\n");
            g_printf("    1.If OS is power on,the BIOS upgrade file is uploaded to %s and take effect "
                "when OS is power-off or reseted next time.\r\n",
                product_type_name);
            g_printf("    2.Length of filepath should not exceed 255 characters.\r\n");
        }
    } else {
        
        g_printf("Usage: ipmcset %s-d upgrade -v <filepath>\r\n", help_default_name);
        g_printf("Filepath      e.g.: /tmp/image.hpm\r\n");
        g_printf("Note:\r\n");
        g_printf("    1.%s will reboot after %s is upgraded.\r\n", product_type_name, product_type_name);
        if (dal_check_if_mm_board() == FALSE) {
            g_printf("    2.If OS is power on,the BIOS upgrade file is uploaded to %s and take effect "
                "when OS is power-off or reseted next time.\r\n",
                product_type_name);
            g_printf("    3.If OS is power on,the CPLD or VRD upgrade file is uploaded to %s and take effect "
                "when OS is power-off.\r\n",
                product_type_name);
            g_printf("    4.Length of filepath should not exceed 255 characters.\r\n");
        }
        
    }

    return;
}

void print_trapseverity_usage(void)
{
    g_printf("Usage: ipmcset -t trap -d severity -v <level>\r\n");
    g_printf(
        "Level contains at least one of the following severity levels : none,all,normal,minor,major,critical.\r\n");
    g_printf("    e.g.: none\r\n");
    g_printf("    e.g.: normal minor\r\n");
    return;
}

void print_boot_device_usage(void)
{
    const gchar *start_option_list[] = {
        "    0    No override",
        "    1    Force PXE",
        "    2    Force boot from default Hard-drive", // 0, 1, 2
        INVALID_DATA_STRING_VALUE, INVALID_DATA_STRING_VALUE, // 3,4
        "    5    Force boot from default CD/DVD",
        "    6    Force boot into BIOS Setup", // 5, 6
        INVALID_DATA_STRING_VALUE, INVALID_DATA_STRING_VALUE, INVALID_DATA_STRING_VALUE,
        INVALID_DATA_STRING_VALUE, INVALID_DATA_STRING_VALUE, INVALID_DATA_STRING_VALUE,
        INVALID_DATA_STRING_VALUE, INVALID_DATA_STRING_VALUE, // 7, 8, 9, 10, 11, 12, 13, 14
        "    0xF  Force boot from Floppy/primary removable media" // 15
    };

    g_printf("Usage: ipmcset -d bootdevice -v <option> [once|permanent]\r\n");
    g_printf("Options are:\r\n");

    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dfl_get_object_handle(BIOS_OBJECT_NAME, &obj_handle);
    if (ret != DFL_OK) {
        g_printf("get Bios handle failed.\r\n");
        return;
    }

    guint32 supported_option = 0;
    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_BIOS_SUPPORTED_START_OPTION, &supported_option);
    if (ret != RET_OK) {
        g_printf("get Bios.SupportedStartOption failed.\r\n");
        return;
    }

    for (gsize i = 0; i < G_N_ELEMENTS(start_option_list); i++) {
        guint32 mask = ((guint32)0x1 << i);
        if (mask & supported_option) {
            g_printf("%s\r\n", start_option_list[i]);
        }
    }
    g_printf("Without parameter [once|permanent], default once.\r\n");
    return;
}
void print_adduser_helpinfo(void)
{
    g_printf("Usage: ipmcset %s-d adduser -v <username>\r\n", help_default_name);
    g_printf("    Contain any digits, letters, or characters except (:,\\<>&\"'/%%) and space.\n");
    
    g_printf("    User name's length is limitted to 1-16.\n");
    g_printf("    User name cannot start with character \"#\" \"+\" or \"-\".\r\n");
    g_printf("    User name cannot be just a dot (.) or two consecutive dots (..).\r\n");
    
    return;
}

void print_trapcommunity_plexity_usage(void)
{
    g_printf("Invalid Community:\n");
    g_printf("1) Contains 8-32 characters.\n");
    g_printf("2) Contains at least one of the following special characters:`~!@#$%%^&*()-_=+\\|[{}];:'\",<.>/?\n");
    g_printf("3) Contains at least two types of the following characters:\n");
    g_printf("   - Letters:a to z;\n");
    g_printf("   - Letters:A to Z;\n");
    g_printf("   - Digits:0 to 9;\n");
    return;
}

void print_SetPasswordComplexityCheckState_helpinfo(void)
{
    g_printf("Usage: ipmcset -d passwordcomplexity -v <enabled|disabled>\r\n");
    return;
}

void print_set_snmp_privacy_help(void)
{
    g_printf("Usage: ipmcset -t user -d snmpprivacypassword -v <username>\r\n");
    g_printf("Note :\r\n");
    g_printf("     User name length is 1 to 16 characters.\r\n");
}

void print_switch_usermgnt_helpinfo(void)
{
    g_printf("Usage: ipmcset -t user -d usermgmtbyhost -v <option>\r\n");
    g_printf("Options are:\r\n");
    g_printf("    0    Disable the BMC user management function on the host side\r\n");
    g_printf("    1    Enable the BMC user management function on the host side\r\n");
    return;
}


void print_sensor_test_help(void)
{
    g_printf("Usage: ipmcset -t sensor -d test -v <sensorname/stopall> [value/stop]\r\n");
    g_printf("Parameters are:\r\n");
    g_printf("  sensorname   sensor name for test\r\n");
    g_printf("  stopall      stop all sensor test\r\n");
    g_printf("\r\n");
    g_printf("  value        start test and set the current value\r\n");
    g_printf("  stop         stop sensor test\r\n");
}


void print_sensor_state_help(void)
{
    g_printf("Usage: ipmcset -t sensor -d state -v <sensorname> <enabled|disabled>\r\n");
    g_printf("Parameters are:\r\n");
    g_printf("  sensorname   sensor name\r\n");
    g_printf("\r\n");
    g_printf("  enabled      enable  sensor\r\n");
    g_printf("  disabled     disable sensor\r\n");
}

const gchar *get_bootdevice_name(guchar device)
{
    switch (device) {
        case NO_OVERRIDE:
            return "No override";

        case FORCE_PEX:
            return "PXE";

        case FORCE_HARD_DRIVE:
            return "Hard-drive";

        case FORCE_CD_DVD:
            return "CD/DVD";

        case FORCE_FLOPPY_REMOVABLE_MEDIA:
            return "floppy/primary removable media";

        default:
            return "unknown";
    }
}

const gchar *get_user_privilege_str(gint32 priv)
{
    switch (priv) {
        case PRIVILEGE_LEVEL_ADMINISTRATOR:
            return "administrator";

        case PRIVILEGE_LEVEL_OPERATOR:
            return "operator";

        case PRIVILEGE_LEVEL_USER:
            return "user";

        case PRIVILEGE_LEVEL_CALLBACK:
            return "callback";

        case PRIVILEGE_LEVEL_NOACCESS:
            return "no access";

        default:
            return "illegal level";
    }
}

void print_set_maintenance_download_helpinfo(void)
{
    g_printf("Usage: ipmcset -t maintenance -d download -v <option>\r\n");
    g_printf("Options are:\r\n");
    g_printf("    1    Download BIOS.\r\n");

    return;
}

void print_set_maintenance_biosprint_helpinfo(void)
{
    g_printf("Usage: ipmcset -t maintenance -d biosprint -v <option>\r\n");
    g_printf("Options are:\r\n");
    g_printf("    1    BIOS debug info enable\r\n");
    
    g_printf("    2    BIOS debug info enabled/disabled by BIOS setup menu\r\n");
    
    return;
}

void print_set_maintenance_coolingpowermode_helpinfo(void)
{
    g_printf("Usage: ipmcset -t maintenance -d coolingpowermode -v <option>\r\n");
    g_printf("Options are:\r\n");
    g_printf("    0    Power saving mode\r\n");
    g_printf("    1    High reliability mode\r\n");
    return;
}


void print_set_security_banner_state_help_info(void)
{
    g_printf("Usage: ipmcset -t securitybanner -d state -v <enabled|disabled>\r\n");
    return;
}


void print_set_security_banner_content_help_info(void)
{
    g_printf("Usage: ipmcset -t securitybanner -d content -v <option>\r\n");
    g_printf("Options are:\r\n");
    g_printf("default    Set login security banner information to default messages.\r\n");
    g_printf("\"<strings>\"    Set logint security banner information to given strings(0~%d bytes).\r\n",
        MOTD_FILE_MAX_LEN);

    g_printf("\r\n");

    g_printf("Exaplmes:\r\n");
    g_printf("1. ipmcset -t securitybanner -d content -v default\r\n");
    g_printf("2. ipmcset -t securitybanner -d content -v \"Warning! This is a private system. "
        "Unauthorized use of the system is prohibited.\"\r\n");
    return;
}

void print_eventsource_usage(void)
{
    
    g_printf("Usage : ipmcset -t syslog -d logtype -v <destination> <type>\r\n");
    g_printf("destination : The destination index that will be set. The values can be 1, 2, 3 or 4.\r\n");
    g_printf(
        "type : contains at least one of the following types : none,all,operationlogs,securitylogs,eventlogs.\r\n");
    g_printf("\r\nexample :\r\n");
    g_printf("        ipmcset -t syslog -d logtype -v 1 none\r\n");
    g_printf("        ipmcset -t syslog -d logtype -v 4 operationlogs eventlogs\r\n");
    
    return;
}

void print_syslogseverity_usage(void)
{
    
    g_printf("Usage : ipmcset -t syslog -d severity -v <level>\r\n");
    g_printf("Level : contains one of the following levels : none,all,normal,minor,major,critical.\r\n");
    g_printf("        normal  : send all level message.\r\n");
    g_printf("        minor   : send minor and above level message.\r\n");
    g_printf("        major   : send major and above level message.\r\n");
    g_printf("        critical: send critical level message.\r\n");
    g_printf("\r\nexample :\r\n");
    
    g_printf("        ipmcset -t syslog -d severity -v none\r\n");
    g_printf("        ipmcset -t syslog -d severity -v normal\r\n");
    
    
    return;
}

void print_syslogidentity_usage(void)
{
    
    g_printf("Usage : ipmcset -t syslog -d identity -v <option>\r\n");
    guint8 software_type = 0;

    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_EM) {
        g_printf("Option : The values can be 3, 4, 5 or 6.\r\n");
        g_printf("         3 : host name  \r\n");
        g_printf("         4 : chassis name  \r\n");
        g_printf("         5 : chassis location  \r\n");
        g_printf("         6 : chassis serial number  \r\n");
    } else {
        g_printf("Option : The values can be 1, 2 or 3.\r\n");
        g_printf("         1 : board serial number  \r\n");
        g_printf("         2 : product asset tag  \r\n");
        g_printf("         3 : host name  \r\n");
    }
    g_printf("\r\nexample :\r\n");
    g_printf("        ipmcset -t syslog -d identity -v 3\r\n");
    
    return;
}

void print_syslogauth_usage(void)
{
    
    g_printf("Usage : ipmcset -t syslog -d auth -v <option>\r\n");
    g_printf("Option : The values can be 1 or 2.\r\n");
    g_printf("          1 : one way authentication  \r\n");
    g_printf("          2 : mutual authentication  \r\n");
    g_printf("\r\nexample :\r\n");
    g_printf("        ipmcset -t syslog -d auth -v 1\r\n");
    g_printf("        ipmcset -t syslog -d auth -v 2\r\n");
    
    return;
}

void print_syslogprotocol_usage(void)
{
    
    g_printf("Usage : ipmcset -t syslog -d protocol -v <option>\r\n");
    g_printf("Option : The values can be 1, 2 or 3.\r\n");
    g_printf("         1 : UDP  \r\n");
    g_printf("         2 : TCP  \r\n");
    g_printf("         3 : TLS  \r\n");
    g_printf("\r\nexample :\r\n");
    g_printf("        ipmcset -t syslog -d protocol -v 1\r\n");
    g_printf("        ipmcset -t syslog -d protocol -v 3\r\n");
    
    return;
}

void print_syslog_rootcertificate_usage(void)
{
    
    g_printf("Usage: ipmcset -t syslog -d rootcertificate -v <filepath>\r\n");
    g_printf("\r\nexample :\r\n");
    g_printf("        ipmcset -t syslog -d rootcertificate -v /tmp/rootcertificate.cer\r\n");
    
    return;
}
void print_syslog_clientcertificate_usage(void)
{
    
    g_printf("Usage: ipmcset -t syslog -d clientcertificate -v <filepath>\r\n");
    g_printf("\r\nexample :\r\n");
    g_printf("        ipmcset -t syslog -d clientcertificate -v /tmp/clientcertificate.pfx\r\n");
    
    return;
}

void print_syslog_dest_address(void)
{
    g_printf("Usage: ipmcset -t syslog -d address -v <destination> <ipaddr>\r\n");
    g_printf("destination : The destination index that will be set. The values can be 1, 2, 3 or 4.\r\n");
    g_printf("ipaddr      : The IP address that will be set. The value \"\" means clearing the address.\r\n");
    g_printf("\r\nexample :\r\n");
    g_printf("        ipmcset -t syslog -d address -v 1 192.168.1.1\r\n");
    g_printf("        ipmcset -t syslog -d address -v 4 \"\"\r\n");
    return;
}
void print_syslog_dest_state(void)
{
    
    g_printf("Usage: ipmcset -t syslog -d state -v [destination] <disabled|enabled>\r\n");
    g_printf("To enable or disable the syslog function for a syslog server, you must specifiy the destination "
        "parameter. The value range of destination is 1 to 4.\r\n");
    g_printf("To enable or disable the syslog function,leave destination unspecified.\r\n");
    g_printf("\r\nexample :\r\n");
    g_printf("        ipmcset -t syslog -d state -v disabled\r\n");
    g_printf("        ipmcset -t syslog -d state -v enabled\r\n");
    g_printf("        ipmcset -t syslog -d state -v 1 disabled\r\n");
    g_printf("        ipmcset -t syslog -d state -v 4 enabled\r\n");
    
    return;
}
void print_syslog_dest_port(void)
{
    g_printf("Usage: ipmcset -t syslog -d port -v <destination> <portvalue>\r\n");
    g_printf("destination : The destination index that will be set. The values can be 1, 2, 3 or 4.\r\n");
    g_printf("portvalue   : The port value that will be set. The value can be from 1 to 65535.\r\n");
    g_printf("\r\nexample :\r\n");
    g_printf("        ipmcset -t syslog -d port -v 1 1\r\n");
    g_printf("        ipmcset -t syslog -d port -v 4 65535\r\n");
    return;
}
void print_syslog_dest_test(void)
{
    g_printf("Usage: ipmcset -t syslog -d test -v <destination>\r\n");
    g_printf("destination : The destination index that will be set. The values can be 1, 2, 3 or 4.\r\n");
    g_printf("\r\nexample :\r\n");
    g_printf("        ipmcset -t syslog -d test -v 1\r\n");
    g_printf("        ipmcset -t syslog -d test -v 4\r\n");
    return;
}
void print_trap_dest_address(void)
{
    g_printf("Usage: ipmcset -t trap -d address -v <destination> <ipaddr>\r\n");
    g_printf("destination : The destination index that will be set. The values can be 1, 2, 3 or 4.\r\n");
    g_printf("ipaddr      : The IP address that will be set. The value \"\" means clearing the address.\r\n");
    g_printf("\r\nexample :\r\n");
    g_printf("        ipmcset -t trap -d address -v 1 192.168.1.1\r\n");
    g_printf("        ipmcset -t trap -d address -v 4 \"\"\r\n");
    return;
}
void print_trap_dest_state(void)
{
    
    g_printf("Usage: ipmcset -t trap -d state -v [destination] <disabled|enabled>\r\n");
    g_printf("To enable or disable the trap function for a trap server, you must specifiy the destination parameter. "
        "The value range of destination is 1 to 4.\r\n");
    g_printf("To enable or disable the trap function,leave destination unspecified.\r\n");
    g_printf("\r\nexample :\r\n");
    g_printf("        ipmcset -t trap -d state -v disabled\r\n");
    g_printf("        ipmcset -t trap -d state -v enabled\r\n");
    g_printf("        ipmcset -t trap -d state -v 1 disabled\r\n");
    g_printf("        ipmcset -t trap -d state -v 4 enabled\r\n");
    
    return;
}
void print_trap_dest_port(void)
{
    g_printf("Usage: ipmcset -t syslog -d port -v <destination> <portvalue>\r\n");
    g_printf("destination : The destination index that will be set. The values can be 1, 2, 3 or 4.\r\n");
    g_printf("portvalue   : The port value that will be set. The value can be from 1 to 65535.\r\n");
    g_printf("\r\nexample :\r\n");
    g_printf("        ipmcset -t trap -d port -v 1 1\r\n");
    g_printf("        ipmcset -t trap -d port -v 4 65535\r\n");
    return;
}

void print_set_pwd_minimum_age_usage(guint32 valid_days)
{
    g_printf("Usage: ipmcset -d minimumpasswordage -v <value>.\r\n");

    if (valid_days == 0) {
        g_printf("value : the range is 0 ~ 365.\r\n");
    } else if (valid_days <= (USER_MODIFY_PASSWORD_WARNING_DAYS + 1)) {
        g_printf("value : only can be 0.\r\n");
    } else {
        g_printf("value : the range is 0 ~ %d.\r\n", valid_days - USER_MODIFY_PASSWORD_WARNING_DAYS - 1);
    }

    return;
}


void print_user_login_interface_usage(void)
{
    g_printf("Usage: ipmcset -t user -d interface -v <username> <enabled/disabled> <option1 option2 ...>\r\n");
    g_printf("Options are:\r\n");
    g_printf("    1     Web\r\n");
    g_printf("    2     SNMP\r\n");
    g_printf("    3     IPMI\r\n");
    g_printf("    4     SSH\r\n");
    g_printf("    5     SFTP\r\n");
    

    
    g_printf("    7     Local\r\n");
    
    g_printf("    8     Redfish\r\n");
    
    return;
}



void print_add_pulickey_usage(void)
{
    g_printf("Usage: ipmcset -d addpublickey -v <username> <localpath/URL>\r\n");
    g_printf("Localpath  e.g.: /tmp/key.pub\r\n");
    g_printf("URL            : protocol://[username:password@]IP[:port]/directory/filename\r\n");
    g_printf("    The parameters in the URL are described as follows:\r\n");
    g_printf("        The protocol must be https,sftp,cifs,scp or nfs.\r\n");
    g_printf("        The URL can contain only letters, digits, and special characters. The directory or file name "
        "cannot contain @.\r\n");
    g_printf("        Use double quotation marks (\") to enclose the URL that contains a space or double quotation "
        "marks (\"). Escape the double quotation marks (\") and back slash (\\) contained in the URL.\r\n");
    g_printf("        For example, if you want to enter:\r\n");
    g_printf("        a b\\cd\"\r\n");
    g_printf("        Enter:\r\n");
    g_printf("        \"a b\\\\cd\\\"\"\r\n");

    return;
}


void print_import_config_usage(void)
{
    g_printf("Usage: ipmcset -t config -d import -v <localpath/URL>\r\n");
    g_printf("Localpath  e.g.: /tmp/config.xml\r\n");
    g_printf("URL            : protocol://[username:password@]IP[:port]/directory/filename\r\n");
    g_printf("    The parameters in the URL are described as follows:\r\n");
    g_printf("        The protocol must be https,sftp,cifs,scp or nfs.\r\n");
    g_printf("        The URL can contain only letters, digits, and special characters. The directory or file name "
        "cannot contain @.\r\n");
    g_printf("        Use double quotation marks (\") to enclose the URL that contains a space or double quotation "
        "marks (\"). Escape the double quotation marks (\") and back slash (\\) contained in the URL.\r\n");
    g_printf("        For example, if you want to enter:\r\n");
    g_printf("        a b\\cd\"\r\n");
    g_printf("        Enter:\r\n");
    g_printf("        \"a b\\\\cd\\\"\"\r\n");

    return;
}

void print_weak_pwddic_usage(void)
{
    g_printf("Usage: ipmcset -t user -d weakpwddic -v <option>\r\n");
    g_printf("Options are:\r\n");
    g_printf("    enabled                  Enable weak password dictionary check.\r\n");
    g_printf("    disabled                 Disable weak password dictionary check.\r\n");
    g_printf("    import <localpath/URL>   Import weak password dictionary.\r\n");
    g_printf("    export [localpath/URL]   Export weak password dictionary.\r\n");
    g_printf("Localpath  e.g.: /tmp/weakdictionary\r\n");
    g_printf("URL            : protocol://[username:password@]IP[:port]/directory/filename\r\n");
    g_printf("    The parameters in the URL are described as follows:\r\n");
    g_printf("        The protocol must be https,sftp,cifs,scp or nfs.\r\n");
    g_printf("        The URL can contain only letters, digits, and special characters. The directory or file name "
        "cannot contain @.\r\n");
    g_printf("        Use double quotation marks (\") to enclose the URL that contains a space or double quotation "
        "marks (\"). Escape the double quotation marks (\") and back slash (\\) contained in the URL.\r\n");
    g_printf("        For example, if you want to enter:\r\n");
    g_printf("        a b\\cd\"\r\n");
    g_printf("        Enter:\r\n");
    g_printf("        \"a b\\\\cd\\\"\"\r\n");

    return;
}

void print_set_vmm_connect(void)
{
    g_printf("Usage: ipmcset -t vmm -d connect -v <URL>\r\n");
    g_printf("URL      e.g.: protocol://[username:password@]IP[:port]/directory/filename\r\n");
    g_printf("    The parameters in the URL are described as follows:\r\n");
    g_printf("        The protocol must be nfs, cifs, or https.\r\n");
    g_printf("        The URL can contain only letters, digits, and special characters. The directory or file name "
        "cannot contain @. The password cannot contain commas (,).\r\n");
    g_printf("        Use double quotation marks (\") to enclose the URL that contains a space or double quotation "
        "marks (\"). Escape the double quotation marks (\") and back slash (\\) contained in the URL.\r\n");
    g_printf("        For example, if you want to enter:\r\n");
    g_printf("        a b\\cd\"\r\n");
    g_printf("        Enter:\r\n");
    g_printf("        \"a b\\\\cd\\\"\"\r\n");

    return;
}

void print_set_vmm_disconnect(void)
{
    g_printf("Usage: ipmcset -t vmm -d disconnect\r\n");
    return;
}


void print_ipmort_certificate_usage(void)
{
    g_printf("Usage: ipmcset -t certificate -d import -v <localpath/URL> <type> [passphrase]\r\n");
    g_printf("Localpath  e.g.: /tmp/customer.pfx\r\n");
    g_printf("URL            : protocol://[username:password@]IP[:port]/directory/filename\r\n");
    g_printf("    The parameters in the URL are described as follows:\r\n");
    g_printf("        The protocol must be https,sftp,cifs,scp or nfs.\r\n");
    g_printf("        The URL can contain only letters, digits, and special characters. The directory or file name "
        "cannot contain @.\r\n");
    g_printf("        Use double quotation marks (\") to enclose the URL that contains a space or double quotation "
        "marks (\"). Escape the double quotation marks (\") and back slash (\\) contained in the URL.\r\n");
    g_printf("        For example, if you want to enter:\r\n");
    g_printf("        a b\\cd\"\r\n");
    g_printf("        Enter:\r\n");
    g_printf("        \"a b\\\\cd\\\"\"\r\n");
    g_printf("Types are:\r\n");
    g_printf("     1        custom certificate, customized p12 or pfx certificate that contains private keys.\r\n");
    return;
}





LOCAL void print_create_ld_usage(void)
{
    g_printf("Usage:\n");
    g_printf("  ipmcset -t storage -d createld -v <RAID Controller ID> -rl <r0|r1|r5|r6|r10|r50|r60> -pd <PD ID "
        "separated by commas>\r\n");
    g_printf("                                 [-cachecade] [-sc <Span Count>] [-name <LD name>]  [-size <LD "
        "size>{m|g|t}] [-ss <64K|128K|256K|512K|1M>]\r\n");
    g_printf("                                 [-rp <ra|nra>] [-wp <wt|wbwithbbu|wb>] [-iop <cio|dio>] [-ap "
        "<rw|ro|blocked>] [-dcp <enabled|disabled|default>]\r\n");
    g_printf("                                 [-init <no|quick|full>]\r\n\n");
    g_printf("options:\n");
    g_printf("  -v <RAID Controller ID>               The RAID controller on which logical drive will be created. It "
        "is mandatory.\n");
    g_printf("  -rl <r0|r1|r5|r6|r10|r50|r60>         Logical drive RAID level. It is mandatory. When '-cachecade' is "
        "specified, RAID level 0/1 is valid.\n");
    g_printf("      r0        =  RAID0\n");
    g_printf("      r1        =  RAID1\n");
    g_printf("      r5        =  RAID5\n");
    g_printf("      r6        =  RAID6\n");
    g_printf("      r10       =  RAID10\n");
    g_printf("      r50       =  RAID50\n");
    g_printf("      r60       =  RAID60\n\n");
    g_printf("  -pd <PD ID separated by commas>       Physical drives ID participated in this logical drive, separated "
        "by commas. It is mandatory.\n");
    g_printf("  -cachecade                            Logical drive is used for secondary cache. It is optional. It "
        "must be supported by RAID controller first.\n");
    g_printf("  -sc <Span Count>                      Logical drive span count. 8 is maximum. It will be 1 on "
        "RAID0/1/5/6 and 2 on RAID10/50/60 by default.\n"); // 8：博通卡的最大数为8
    g_printf(
        "                                        It is optional and not necessary when '-cachecade' is specified.\n");
    g_printf(
        "  -name <LD name>                       Logical drive name. %d ASCII characters is maximum. It is optional.\n",
        SL_LD_NAME_LENGTH - 1);
    g_printf("                                        The value can't be same as the options if it starts with '-'.\n");
    g_printf("  -size <LD size>{m|g|t}                The size of logical drive and the uint can be 'm'(megabytes) , "
        "'g'(gigabytes) , 't'(terabytes).\n");
    g_printf("                                        It is optional and not necessary when '-cachecade' is specified. "
        "If this option is not specified,\n");
    g_printf("                                        the size of logical drive depends on the usable size of each "
        "physical drive.\n");
    g_printf("  -ss <64K|128K|256K|512K|1M>           Logical drive strip size. The recommended range is 64K ~ 1M. It "
        "is optional and not necessary when '-cachecade' is specified.\n");
    g_printf("                                        If this option is not specified and '-cachecade' is not "
        "specified, the default of strip size is '256K'.\n");
    g_printf("                                        If this option is not specified but '-cachecade' is specified, "
        "the default of strip size is '1M'.\n");
    g_printf("  -rp <ra|nra>                          Logical drive read policy. It is optional and not necessary when "
        "'-cachecade' is specified.\n");
    g_printf("                                        If this option is not specified and '-cachecade' is not "
        "specified, the default of read policy is 'Read Ahead'.\n");
    g_printf("                                        If this option is not specified but '-cachecade' is specified, "
        "the default of read policy is 'No Read Ahead'.\n");
    g_printf("      ra        = Read ahead\n");
    g_printf("      nra       = No Read ahead\n\n");
    g_printf("  -wp <wt|wbwithbbu|wb>                 Logical drive write policy. It is optional. If this option is "
        "not specified, the default of\n");
    g_printf("                                        write policy is 'Write Back with BBU'.\n");
    g_printf("      wt        = Write through\n");
    g_printf("      wbwithbbu = Write back with BBU\n");
    g_printf("      wb        = Write back\n\n");
    g_printf("  -iop <cio|dio>                        Logical drive IO policy. It is optional and not necessary when "
        "'-cachecade' is specified.\n");
    g_printf("                                        If this option is not specified, the default of IO policy is "
        "'Direct IO'.\n");
    g_printf("      cio       = Cached IO\n");
    g_printf("      dio       = Direct IO\n\n");
    g_printf("  -ap <rw|ro|blocked>                   Logical drive access policy. It is optional and not necessary "
        "when '-cachecade' is specified.\n");
    g_printf("                                        If this option is not specified, the default of access policy is "
        "'Read Write'.\n");
    g_printf("      rw        = Read write\n");
    g_printf("      ro        = Read only\n");
    g_printf("      blocked   = Blocked\n\n");
    g_printf("  -dcp <enabled|disabled|default>       Logical drive disk cache policy. It is optional and not "
        "necessary when '-cachecade' is specified.\n");
    g_printf("                                        If this option is not specified and '-cachecade' is not "
        "specified, the default of disk cache policy is 'Enabled'.\n");
    g_printf("                                        If this option is not specified but '-cachecade' is specified, "
        "the default of disk cache policy is 'Disk's default'.\n");
    g_printf("      enabled   = Enable disk cache\n");
    g_printf("      disabled  = Disable disk cache \n");
    g_printf("      default   = Unchanged and determined by disk type\n\n");
    g_printf("  -init <no|quick|full>                 Logical drive init type. It is optional and not necessary when "
        "'-cachecade' is specified.\n");
    g_printf("                                        If this option is not specified, the default of init type is "
        "'None'.\n");
    g_printf("      no        = Don't initialize the logical drive\n");
    g_printf("      quick     = Quickly initialize the logical drive\n");
    g_printf("      full      = Fully initialize the logical drive\n\n");

    g_printf("example:\n");
    
    // 新建逻辑盘指定容量单位为G或者T时,支持输入小数
    g_printf("    ipmcset -t storage -d createld -v 0 -rl r1 -pd 0,1 -name example -size 100.375g -ss 512k -rp ra -wp "
        "wb -ap rw -iop cio -dcp enabled -init quick\r\n\n");
    
    g_printf("  or\n");
    g_printf("    ipmcset -t storage -d createld -v 0 -rl r0 -pd 0,1,2 -name cachecade -cachecade -wp wb\r\n\n");
}

LOCAL void print_ld_common_usage_for_pmc(void)
{
    
    g_printf("  -rl <r0|r1|r5|r6|r10|r50|r60|r1adm|r10adm|r1triple|r10triple>   Logical drive RAID level. "
             "It is mandatory.\n");
    g_printf("                                        When '-cachecade' is specified, RAID level 0/1/5/10 is valid.\n");
    g_printf("      r0        =  RAID0\n      r1        =  RAID1\n      r5        =  RAID5\n");
    g_printf("      r6        =  RAID6\n      r10       =  RAID10\n      r50       =  RAID50\n");
    g_printf("      r60       =  RAID60\n      r1adm     =  RAID1ADM\n      r10adm    =  RAID10ADM\n");
    g_printf("      r1triple  =  RAID1Triple\n      r10triple =  RAID10Triple\n\n");
    
    g_printf("  -cachecade                            Logical drive is used for secondary cache. It is optional. "
             "It must be supported by RAID controller first.\n");
    
    g_printf("  -sc <Span Count>                      Logical drive span count. "
             "It will be 1 on RAID0/1/5/6/1ADM/1Triple and 2 on RAID50/60 by default.\n");
    g_printf("                                        "
             "It is optional and not involved when '-cachecade' is specified.\n");
    
    g_printf("  -name <LD name>                       Logical drive name. %d ASCII characters is maximum. "
             "It is optional and not involved when '-cachecade' is specified.\n",
             SC_LD_NAME_LENGTH - 1);
    g_printf("                                        The value can't be same as the options if it starts with '-'.\n");
    
    g_printf("  -size <LD size>{m|g|t}                "
             "The size of logical drive and the uint can be 'm'(megabytes) , 'g'(gigabytes) , 't'(terabytes).\n");
    g_printf("                                        It is optional. If this option is not specified, \n");
    g_printf("                                        "
             "the size of logical drive depends on the usable size of each physical drive.\n");
    
    g_printf("  -ss <16K|32K|64K|128K|256K|512K|1M>   Logical drive strip size. "
             "It is optional and not involved when '-cachecade' is specified.\n");
    g_printf("                                        "
             "If this option is not specified the default of strip size is '256K'.\n");
    
    g_printf("  -acc <no|cache|iobypass>              Logical drive acceleration method. "
             "It is optional and not involved when '-cachecade' is specified.\n");
    g_printf("                                        "
             "If this option is not specified, the default of acceleration method is 'None'.\n");
    g_printf("      no        = None\n      cache     = Controller Cache\n");
    g_printf("      iobypass  = I/O Bypass. Only valid for Logical drive created by SSD\n\n");
    
    g_printf("  -associatedld <LD ID>                 General Logical Drive ID associated with cachecade. "
             "It is mandatory when '-cachecade' is specified\n");
    
    g_printf("  -wp <wt|wbwithbbu>                    Logical drive write policy. "
             "It is mandatory when '-cachecade' is specified\n");
    g_printf("      wt        = Write through\n      wbwithbbu = Write back with BBU\n");
    
    g_printf("  -cls <64K|256K>                       Cache Line Size. "
             "It is optional and not involved when '-cachecade' is not specified.\n");
    g_printf("                                        "
             "If this option is not specified, the default of cache line size is 64K.\n");
    
    g_printf("  -init <no|rpi|opo>                    Logical drive init method. "
             "It is optional and not involved when '-cachecade' is specified.\n");
    g_printf("                                        "
             "If this option is not specified, the default of init method is 'None'.\n");
    g_printf("      no        = Disables RPI & OPO\n      rpi       = Rapid-Parity-Initialization\n");
    g_printf("      opo       = Over-Provisioning Optimization. Only valid for Logical drive created by SSD\n\n");
}


LOCAL void print_create_ld_usage_for_pmc(void)
{
    g_printf("Usage:\n");
    g_printf("  ipmcset -t storage -d createld -v <RAID Controller ID> -pd <PD ID separated by commas> -rl "
             "<r0|r1|r5|r6|r10|r50|r60|r1adm|r10adm|r1triple|r10triple>\r\n");
    g_printf("                                 [-cachecade] [-sc <Span Count>] [-name <LD name>] "
             "[-size <LD size>{m|g|t}] [-ss <16K|32K|64K|128K|256K|512K|1M>]\r\n");
    g_printf("                                 [-acc <no|cache|iobypass>] [-associatedld <LD ID>] [-wp <wt|wbwithbbu>] "
             "[-cls <64K|256K>]\r\n");
    g_printf("                                 [-init <no|rpi|opo>]\r\n\n");
    g_printf("options:\n");
    g_printf("  -v <RAID Controller ID>               "
             "The RAID controller on which logical drive will be created. It is mandatory.\n");
    
    g_printf("  -pd <PD ID separated by commas>       "
        "Physical drives ID participated in this logical drive, separated by commas. It is mandatory.\n");
    print_ld_common_usage_for_pmc();

    g_printf("example:\n");
    g_printf("    ipmcset -t storage -d createld -v 0 -rl r1 -pd 0,1 -name example -size 100.375g -ss 512k -acc cache "
             "-init rpi\r\n\n");
    g_printf("  or\n");
    g_printf("    ipmcset -t storage -d createld -v 0 -rl r0 -pd 0,1,2 -cachecade -size 100g -associatedld 0 -wp wt "
             "-cls 256K\r\n\n");
}

void print_create_ld_help_info(guint8 type_id)
{
    if (dal_test_controller_vendor(type_id, VENDER_PMC)) {
        print_create_ld_usage_for_pmc();
    } else {
        print_create_ld_usage();
    }

    return;
}


LOCAL void print_add_ld_usage(void)
{
    g_printf("Usage:\n");
    g_printf("  ipmcset -t storage -d addld -v <RAID Controller ID> -array <Disk array ID>\r\n");
    g_printf(
        "                              [-name <LD name>]  [-size <LD size>{m|g|t}] [-ss <64K|128K|256K|512K|1M>]\r\n");
    g_printf("                              [-rp <ra|nra>] [-wp <wt|wbwithbbu|wb>] [-iop <cio|dio>] [-ap "
        "<rw|ro|blocked>] [-dcp <enabled|disabled|default>]\r\n");
    g_printf("                              [-init <no|quick|full>] [-block <block index>]\r\n\n");
    g_printf("options:\n");
    g_printf("  -v <RAID Controller ID>               The RAID controller on which logical drive will be created. It "
        "is mandatory.\n");
    g_printf("  -array <Disk array ID>                The array that logical drive will be added. It is mandatory.\n");
    g_printf(
        "  -name <LD name>                       Logical drive name. %d ASCII characters is maximum. It is optional.\n",
        SL_LD_NAME_LENGTH - 1);
    g_printf("                                        The value can't be same as the options if it starts with '-'.\n");
    g_printf("  -size <LD size>{m|g|t}                The size of logical drive and the uint can be 'm'(megabytes) , "
        "'g'(gigabytes) , 't'(terabytes). It is optional.\n");
    g_printf("                                        If this option is not specified, the size of logical drive "
        "depends on the usable size of each array.\n");
    g_printf("  -ss <64K|128K|256K|512K|1M>           Logical drive strip size. The recommended range is 64K ~ 1M. It "
        "is optional. The default of strip size is '256K'.\n");
    g_printf("  -rp <ra|nra>                          Logical drive read policy. It is optional. The default of read "
        "policy is 'Read Ahead'.\n");
    g_printf("      ra        = Read ahead\n");
    g_printf("      nra       = No Read ahead\n\n");
    g_printf("  -wp <wt|wbwithbbu|wb>                 Logical drive write policy. It is optional. the default of write "
        "policy is 'Write Back with BBU'.\n");
    g_printf("      wt        = Write through\n");
    g_printf("      wbwithbbu = Write back with BBU\n");
    g_printf("      wb        = Write back\n\n");
    g_printf("  -iop <cio|dio>                        Logical drive IO policy. It is optional. The default of IO "
        "policy is 'Direct IO'.\n");
    g_printf("      cio       = Cached IO\n");
    g_printf("      dio       = Direct IO\n\n");
    g_printf("  -ap <rw|ro|blocked>                   Logical drive access policy. It is optional. The default of "
        "access policy is 'Read Write'.\n");
    g_printf("      rw        = Read write\n");
    g_printf("      ro        = Read only\n");
    g_printf("      blocked   = Blocked\n\n");
    g_printf("  -dcp <enabled|disabled|default>       Logical drive disk cache policy. It is optional. The default of "
        "disk cache policy is 'Enabled'.\n");
    g_printf("      enabled   = Enable disk cache\n");
    g_printf("      disabled  = Disable disk cache \n");
    g_printf("      default   = Unchanged and determined by disk type\n\n");
    g_printf("  -init <no|quick|full>                 Logical drive init type. It is optional. The default of init "
        "type is 'None'.\n");
    g_printf("      no        = Don't initialize the logical drive\n");
    g_printf("      quick     = Quickly initialize the logical drive\n");
    g_printf("      full      = Fully initialize the logical drive\n");
    g_printf("  -block <block index>                  Continuous space to which a logical drive is to be added. It is "
        "optional.\n\n");

    g_printf("example:\n");
    
    // 添加逻辑盘指定容量单位为G或者T时,支持输入小数
    g_printf("    ipmcset -t storage -d addld -v 0 -array 1 -name example -size 500.650g -ss 256k -rp ra -wp wb -ap rw "
        "-iop cio -dcp enabled -init quick -block 1\r\n\n");
    
}

LOCAL void print_add_ld_usage_for_pmc(void)
{
    g_printf("Usage:\n");
    g_printf("  ipmcset -t storage -d addld -v <RAID Controller ID> -array <Disk array ID> -rl "
             "<r0|r1|r5|r6|r10|r50|r60|r1adm|r10adm|r1triple|r10triple>\r\n");
    g_printf("                                 [-cachecade] [-sc <Span Count>] [-name <LD name>] "
             "[-size <LD size>{m|g|t}] [-ss <16K|32K|64K|128K|256K|512K|1M>]\r\n");
    g_printf("                                 [-acc <no|cache|iobypass>] [-associatedld <LD ID>] [-wp <wt|wbwithbbu>] "
             "[-cls <64K|256K>]\r\n");
    g_printf("                                 [-init <no|rpi|opo>]\r\n\n");

    g_printf("options:\n");
    g_printf("  -v <RAID Controller ID>               The RAID controller on which logical drive will be created. "
             "It is mandatory.\n");
    
    g_printf("  -array <Disk array ID>                The array that logical drive will be added. It is mandatory.\n");
    print_ld_common_usage_for_pmc();

    g_printf("example:\n");
    g_printf("    ipmcset -t storage -d addld -v 0 -rl r1 -array 0 -name example -size 500.650g -ss 256k -acc cache "
             "-init rpi\r\n\n");
    g_printf("  or\n");
    g_printf("    ipmcset -t storage -d addld -v 0 -rl r0 -array 1 -cachecade -size 100g -associatedld 0 -wp wt "
             "-cls 256K\r\n\n");
}

void print_add_ld_help_info(guint8 type_id)
{
    if (dal_test_controller_vendor(type_id, VENDER_PMC)) {
        print_add_ld_usage_for_pmc();
    } else {
        print_add_ld_usage();
    }
    return;
}

LOCAL void print_modify_ld_policy_usage(void)
{
    g_printf("  -rp <ra|nra>                                Set logical drive read policy. This option is not "
        "supported when logical drive is cachecade.\n");
    g_printf("      ra        = Read ahead\n");
    g_printf("      nra       = No Read ahead\n\n");
    g_printf("  -wp <wt|wbwithbbu|wb>                       Set logical drive write policy. The 'Write back' is not "
        "supported when logical drive is cachecade.\n");
    g_printf("      wt        = Write through\n");
    g_printf("      wbwithbbu = Write back with BBU\n");
    g_printf("      wb        = Write back\n\n");
    g_printf("  -iop <cio|dio>                              Set logical drive IO policy. This option is not supported "
        "when logical drive is cachecade.\n");
    g_printf("      cio       = Cached IO\n");
    g_printf("      dio       = Direct IO\n\n");
    g_printf("  -ap  <rw|ro|blocked>                        Set logical drive access policy. This option is not "
        "supported when logical drive is cachecade.\n");
    g_printf("      rw        = Read write\n");
    g_printf("      ro        = Read only\n");
    g_printf("      blocked   = Blocked\n\n");
    g_printf("  -dcp <enabled|disabled|default>             Set logical drive disk cache policy. This option is not "
        "supported when logical drive is cachecade.\n");
    g_printf("      enabled    = Enable disk cache\n");
    g_printf("      disabled   = Disable disk cache\n");
    g_printf("      default    = Unchanged and determined by disk type\n\n");
}


LOCAL void print_modify_ld_usage(void)
{
    g_printf("Usage:\n");
    g_printf("  ipmcset -t storage -d ldconfig -v <RAID Controller ID> <Logical Drive ID>\r\n");
    g_printf("                                 <[-name <LD name>] [-rp <ra|nra>] [-wp <wt|wbwithbbu|wb>] [-iop "
        "<cio|dio>] [-ap <rw|ro|blocked>] [-dcp <enabled|disabled|default>]\r\n");
    g_printf("                            [-bgi <enabled|disabled>] "
                                          "[-boot <none|primary>] [-sscd <enabled|disabled>]>\r\n\n");
    g_printf("options:\n");
    g_printf("  -v <RAID Controller ID> <Logical Drive ID>  The logical drive of which RAID controller will be set. It "
        "is mandatory.\n");
    g_printf("  -name <LD name>                             Set logical drive name. %d ASCII characters is maximum. "
        "The value can't be same as the options if it starts with '-'.\n",
        SL_LD_NAME_LENGTH - 1);
    print_modify_ld_policy_usage();
    g_printf("  -bgi <enabled|disabled>                     Set logical drive BGI state. This option is not supported "
        "when logical drive is cachecade.\n");
    g_printf("      enabled   = Enable logical drive BGI\n");
    g_printf("      disabled  = Disable logical drive BGI\n\n");
    g_printf("  -boot                                       Set logical drive as boot device. This option is not "
        "supported when logical drive is cachecade.\n");
    g_printf("      none      = Disable logical drive as boot device\n");
    g_printf("      primary   = Set logical drive as first boot device\n\n");
    g_printf("  -sscd <enabled|disabled>                    Set logical drive SSCD caching state. There must be one or "
        "more cachecade logical drive in the same RAID controller.\n");
    g_printf("                                              This option is not supported when logical drive is "
        "cachecade.\n");
    g_printf("      enabled   = Enable logical drive SSCD caching\n");
    g_printf("      disabled  = Disable logical drive SSCD caching\n\n");
    g_printf("example:\n");
    g_printf("    ipmcset -t storage -d ldconfig -v 0 0 -name example -rp ra -wp wb -ap rw -iop cio -dcp enabled -bgi "
        "enabled -boot primary -sscd enabled\r\n\n");
}

LOCAL void print_modify_ld_usage_for_pmc(void)
{
    g_printf("Usage:\n");
    g_printf("  ipmcset -t storage -d ldconfig -v <RAID Controller ID> <Logical Drive ID>\r\n");
    g_printf("                                 <[-name <LD name>] [-wp <wt|wbwithbbu>] [-acc <no|cache|iobypass>]\r\n");
    g_printf("                                  [-size <LD size>{m|g|t}] [-ss <16K|32K|64K|128K|256K|512K|1M>]\r\n");
    g_printf("                                  [-boot <none|primary|secondary|all>]>\r\n\n");
    g_printf("options:\n");
    g_printf("  -v <RAID Controller ID> <Logical Drive ID>  The logical drive of which RAID controller will be set. It "
        "is mandatory.\n");
    g_printf("  -name <LD name>                             Set logical drive name. %d ASCII characters is maximum. "
        "The value can't be same as the options if it starts with '-'. This option is not supported "
        "when logical drive is cachecade.\n",
        SC_LD_NAME_LENGTH - 1);
    g_printf("  -wp <wt|wbwithbbu>                          Set logical drive write policy. This option is only "
        "supported when logical drive is cachecade.\n");
    g_printf("      wt        = Write through\n");
    g_printf("      wbwithbbu = Write back with BBU\n\n");
    g_printf("  -acc <no|cache|iobypass>                    Set Logical drive acceleration method. This option is "
        "not supported when logical drive is cachecade.\n");
    g_printf("      no        = None\n      cache     = Controller Cache\n");
    g_printf("      iobypass  = I/O Bypass. Only valid for Logical drive created by SSD\n\n");
    g_printf("  -size <LD size>{m|g|t}                      Set Logical drive capacity size. The uint can be "
        "'m'(megabytes) , 'g'(gigabytes) , 't'(terabytes). "
        "The size to be set cannot be smaller than the current one.\n\n");
    g_printf("  -ss <16K|32K|64K|128K|256K|512K|1M>         Set Logical drive strip size.\n\n");
    g_printf("  -boot                                       Set logical drive as boot device. This option is not "
        "supported when logical drive is cachecade.\n");
    g_printf("      none      = Disable logical drive as boot device\n");
    g_printf("      primary   = Set logical drive as first boot device\n");
    g_printf("      secondary = Set logical drive as second boot device\n");
    g_printf("      all       = Set logical drive as first and second boot device\n\n");

    g_printf("example:\n");
    g_printf("    ipmcset -t storage -d ldconfig -v 0 0 -name example -size 100g -ss 64K -boot primary\r\n\n");
}

void print_modify_ld_help_info(guint8 type_id)
{
    if (dal_test_controller_vendor(type_id, VENDER_PMC)) {
        print_modify_ld_usage_for_pmc();
    } else {
        print_modify_ld_usage();
    }
}


void print_set_ctrl_config_usage(void)
{
    g_printf("Usage:\n");
    g_printf("  ipmcset -t storage -d ctrlconfig -v <RAID Controller ID>\r\n");
    g_printf("                                   <[-cb <enabled|disabled>] [-smartercb <enabled|disabled>] [-jbod "
        "<enabled|disabled>] [-mode <RAID|HBA|JBOD|Mixed>] [-restore]>\r\n");
    g_printf("options:\n");
    g_printf("  -v <RAID Controller ID>               The RAID controller which will be set. It is mandatory.\n");
    g_printf("  -cb <enabled|disabled>                Set RAID controller Copyback to be enabled or disabled.\n");
    g_printf("      enabled   = Enable RAID controller Copyback\n");
    g_printf("      disabled  = Disable RAID controller Copyback\n\n");
    g_printf("  -smartercb <enabled|disabled>         Set RAID controller Copyback on SMART error to be enabled or "
        "disabled. It will be disabled when 'Copyback' is disabled.\n");
    g_printf("      enabled   = Enable RAID controller Copyback on SMART error. It can be enabled only when Copyback "
        "is enabled\n");
    g_printf("      disabled  = Disable RAID controller Copyback on SMART error\n\n");
    g_printf("  -jbod <enabled|disabled>              Set RAID controller JBOD to be enabled or disabled.\n");
    g_printf("      enabled   = Enable RAID controller JBOD mode\n");
    g_printf("      disabled  = Disable RAID controller JBOD mode\n\n");
    g_printf("  -mode <RAID|HBA|JBOD|Mixed>           Set RAID controller working mode.\n");
    g_printf("  -restore                              Restore RAID controller settings. It can't be specified with "
        "other options simultaneously but mode.\n\n");

    g_printf("example:\n");
    g_printf("    ipmcset -t storage -d ctrlconfig -v 0 -cb enabled -smartercb enabled -jbod enabled -mode RAID\r\n\n");
    g_printf("  or\n");
    g_printf("    ipmcset -t storage -d ctrlconfig -v 0 -restore\r\n\n");
}


void print_set_pd_config_usage(void)
{
    g_printf("Usage:\n");
    g_printf("  ipmcset -t storage -d pdconfig -v <Physical Drive ID>\r\n");
    g_printf("                                 <[-state <online|offline|ug|jbod>] "
        "[-hotspare <none|global|dedicated|autoreplace> [-ld <Logical Drive ID>]]\n");
    g_printf("                                  [-locate <start|stop>] [-cryptoerase] "
        "[-boot <none|primary|secondary|all>]>\r\n");
    g_printf("options:\n");
    g_printf("  -v <Physical Drive ID>                The physical drive which will be set. It is mandatory.\n");
    g_printf("  -state <online|offline|ug|jbod>       Set physical drive firmware state.\n");
    g_printf(
        "      online     = Set physical drive state to ONLINE. The drive must be participated in logical drive\n");
    g_printf(
        "      offline    = Set physical drive state to OFFLINE. The drive must be participated in logical drive\n");
    g_printf("      ug         = Set physical drive state to UNCONFIGURED GOOD\n");
    g_printf("      jbod       = Set physical drive state to JBOD (Drive is exposed and controlled by host). You "
        "should enable JBOD of RAID controller first\n\n");
    g_printf("  -hotspare <none|global|dedicated|autoreplace>     Set physical drive hotspare state.\n");
    g_printf("      none       = Cancel physical drive hot spare\n");
    g_printf("      global     = Set physical drive as global hot spare device\n");
    g_printf("      dedicated -ld <Logical Drive ID>     = Set physical drive as dedicated hot spare device and the "
        "option '-ld' specify\n");
    g_printf("                                             the logical drive ID for which physical drive dedicated hot "
        "spare, separated by commas.\n");
    g_printf("      autoreplace -ld <Logical Drive ID>   = Set physical drive as autoreplace hot spare device and the "
        "option '-ld' specify\n");
    g_printf("                                             the logical drive ID for which physical drive autoreplace"
        " hot spare\n\n");
    g_printf("  -locate <start|stop>                  Set physical drive location state.\n");
    g_printf("      start      = Start locating physical drive\n");
    g_printf("      stop       = Stop locating physical drive\n\n");
    g_printf("  -cryptoerase                          Cryptographically erase physical drive.\n\n");
    g_printf("  -boot                                 Set physical drive as boot device.\n");
    g_printf("      none       = Disable physical drive as boot device\n");
    g_printf("      primary    = Set physical drive as first boot device\n");
    g_printf("      secondary  = Set physical drive as second boot device\n");
    g_printf("      all        = Set physical drive as first and second boot device\n\n");

    g_printf("example:\n");
    g_printf("    ipmcset -t storage -d pdconfig -v 1 -state ug\r\n\n");
    g_printf("  or\n");
    g_printf("    ipmcset -t storage -d pdconfig -v 1 -locate start\r\n\n");
}





void print_upload_crl_usage(void)
{
    g_printf("Usage: ipmcset -d crl -v <localpath/URL> <type>\r\n");
    g_printf("Localpath  e.g.: /tmp/cms.crl\r\n");
    g_printf("URL            : protocol://[username:password@]IP[:port]/directory/filename\r\n");
    g_printf("    The parameters in the URL are described as follows:\r\n");
    g_printf("        The protocol must be https,sftp,cifs,scp or nfs.\r\n");
    g_printf("        The URL can contain only letters, digits, and special characters. The directory or file name "
        "cannot contain @.\r\n");
    g_printf("        Use double quotation marks (\") to enclose the URL that contains a space or double quotation "
        "marks (\"). Escape the double quotation marks (\") and back slash (\\) contained in the URL.\r\n");
    g_printf("        For example, if you want to enter:\r\n");
    g_printf("        a b\\cd\"\r\n");
    g_printf("        Enter:\r\n");
    g_printf("        \"a b\\\\cd\\\"\"\r\n");
    g_printf("Types are:\r\n");
    g_printf("     1        CRL file for upgrade package integrity checking.\r\n");
}


void print_psu_work_mode(guint8 max_ps_num)
{
    OBJ_HANDLE service_handle = 0;
    guint8 deep_sleep_enable = 0;

    guint8 min = 1;
    guint8 max = max_ps_num;
    if (dal_match_product_id(PRODUCT_ID_PANGEA_V6)) { 
        min--;
        max--;
    }
    
    (void)dal_get_object_handle_nth(CLASS_NAME_PME_SERVICECONFIG, 0, &service_handle);
    (void)dal_get_property_value_byte(service_handle, PROTERY_PME_SERVICECONFIG_DEEP_SLEEP_ENABLE, &deep_sleep_enable);

    if (deep_sleep_enable != DEEP_SLEEP_MODE_ENABLED) {
        if (max_ps_num == 2) {
            g_printf("Usage: ipmcset -d psuworkmode -v <option> [active psuid]\r\n");
        } else {
            g_printf("Usage: ipmcset -d psuworkmode -v <option> [active psuid] [active psuid] \r\n");
        }
        g_printf("Options are:\r\n");
        g_printf("0:           Load Balancing, without parameter [active psuid] \r\n");
        g_printf("1:           Active/Standby, [active psuid] range <%d-%d> \r\n", min, max);
    } else {
        if (max_ps_num == 2) {
            g_printf("Usage: ipmcset -d psuworkmode -v <option> [active psuid|enabled/disabled]\r\n");
        } else {
            g_printf("Usage: ipmcset -d psuworkmode -v <option> [active psuid|enabled/disabled] [active psuid]\r\n");
        }
        g_printf("Options are:\r\n");
        g_printf("0:           Load Balancing, without parameter [active psuid|enabled/disabled] \r\n");
        g_printf("1:           Active/Standby, with parameter [active psuid] range <%d-%d> \r\n", min, max);
        g_printf("2:           Hibernate, with parameter [enabled/disabled] \r\n");
    }
    
    return;
}



void print_set_inactive_user_timeout_helpinfo(void)
{
    g_printf("Usage: ipmcset -t securityenhance -d inactivetimelimit -v <time>\r\n");
    g_printf("Options are:\r\n");
    g_printf("     %d    Never disable inactive user\r\n", SECURITYENHANCE_INACTIVEUSERTIME_DEFAULT);
    g_printf("%d~%d    Days of inactivity before disable user\r\n", SECURITYENHANCE_INACTIVEUSERTIME_MIN,
        SECURITYENHANCE_INACTIVEUSERTIME_MAX);
    return;
}


void print_set_update_interval_helpinfo(void)
{
    g_printf("Usage: ipmcset -t securityenhance -d masterkeyupdateinterval -v <interval>\r\n");
    g_printf("Options are:\r\n");
    g_printf("    %d    Never enabled to update master key automatically\r\n",
        SECURITYENHANCE_AUTOUPDATEINTERVAL_DEFAULT);
    g_printf("%d~%d    Days of master key automatic update interval\r\n", SECURITYENHANCE_AUTOUPDATEINTERVAL_MIN,
        SECURITYENHANCE_AUTOUPDATEINTERVAL_MAX);
    return;
}

void print_update_master_key_warnninginfo(void)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar VNCEnable = 0;

    g_printf("WARNING: You are about to update the following master key:\r\n");
    if (dal_check_if_mm_board() == TRUE) {
        g_printf("       Upgrade file master key\r\n");
        g_printf("       SSH host key master key\r\n");
        return;
    }
    g_printf("       IPMI password master key\r\n");
    g_printf("       SNMP community master key\r\n");
    g_printf("       SNMP privacy password master key\r\n");
    g_printf("       Trap community master key\r\n");
    g_printf("       SMTP password master key\r\n");
    g_printf("       Redfish master key\r\n");
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_VNC, &VNCEnable);
    if (VNCEnable == SERVICE_STATE_ENABLE) {
        g_printf("       VNC password master key\r\n");
    }
    g_printf("       Upgrade file master key\r\n");
    g_printf("       SSH host key master key\r\n");
    g_printf("       SSL master key\r\n");
    g_printf("       LDAP bind password master key\r\n");
    g_printf("       NTP GroupKey file master key\r\n");

    ret = dfl_get_object_handle(BIOS_CLASS_NAME, &obj_handle);
    if (ret == RET_OK) {
        g_printf("       BIOS password master key\r\n");
    }
    ret = dfl_get_object_handle(CLASS_USB_MGMT, &obj_handle);
    if (ret == RET_OK) {
        g_printf("       USB uncompress password master key\r\n");
    }
    return;
}


void print_set_timezone_helpinfo(void)
{
    g_printf("Usage: ipmcset %s-d timezone -v <timezone>\r\n", help_default_name);
    g_printf("Options are:\r\n");
    g_printf("        Time-Offset: -12:00~+14:00            e.g. +08:00,-04:30\r\n");
    g_printf("                     UTC-12:00~UTC+14:00      e.g. UTC+08:00,UTC-04:30\r\n");
    g_printf("                     GMT-12:00~GMT+14:00      e.g. GMT+08:00,GMT-04:30\r\n");
    g_printf("        Timezone   : <City Name>              e.g. Asia/Shanghai,America/New_York\r\n");
}


void print_timezone_area_helpinfo(const TZ_CITY *tz_list, guint32 list_len)
{
    guint32 i;

    print_set_timezone_helpinfo();

    g_printf("Continents and Oceans:\r\n");
    for (i = 0; i < list_len; i += 5) { 
        g_printf("      %3d) %-15s ", i + 1, tz_list[i].area);
        if (i + 1 < list_len) {
            g_printf("%3d) %-15s ", i + 2, tz_list[i + 1].area);
        }
        if (i + 2 < list_len) {
            g_printf("%3d) %-15s ", i + 3, tz_list[i + 2].area);
        }
        if (i + 3 < list_len) {
            g_printf("%3d) %-15s ", i + 4, tz_list[i + 3].area);
        }
        if (i + 4 < list_len) {
            g_printf("%3d) %-15s\r\n", i + 5, tz_list[i + 4].area);
        }
    }
    g_printf("\r\n");
}


void print_timezone_city_helpinfo(const TZ_CITY *tz_city)
{
    guint32 i;

    print_set_timezone_helpinfo();

    g_printf("Cities of %s:\r\n", tz_city->area);
    for (i = 0; i < tz_city->city_num; i += 5) {
        g_printf("      %3d) %-25s ", i + 1, tz_city->city[i]);
        if (i + 1 < tz_city->city_num) {
            g_printf("%3d) %-25s ", i + 2, tz_city->city[i + 1]);
        }
        if (i + 2 < tz_city->city_num) {
            g_printf("%3d) %-25s ", i + 3, tz_city->city[i + 2]);
        }
        if (i + 3 < tz_city->city_num) {
            g_printf("%3d) %-25s ", i + 4, tz_city->city[i + 3]);
        }
        if (i + 4 < tz_city->city_num) {
            g_printf("%3d) %-25s\r\n", i + 5, tz_city->city[i + 4]);
        }
    }

    if (tz_city->city_num % 5 != 0) {
        g_printf("\r\n");
    }
}


LOCAL gint32 print_com_info(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    guint8 cli_support;
    guint8 session_index;
    gchar com_name[MAX_PROPERTY_VALUE_LEN] = {0};

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SOL_CLI_SUPPORT, &cli_support);
    if (ret != DFL_OK || cli_support == 0) {
        return RET_OK;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SOL_COM_INDEX, &session_index);
    ret += dal_get_property_value_string(obj_handle, PROPERTY_SOL_COM_NAME, com_name, sizeof(com_name));
    if (ret != DFL_OK) {
        g_printf("Get COM information failed.\r\n");
        return RET_ERR;
    }

    g_printf("    %d    %s\r\n", session_index, com_name);
    return RET_OK;
}


void print_cli_sol_activate_usage(void)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    OBJ_HANDLE obj_handle = 0;
    guint8 com_id = 255;

    g_printf("Usage: ipmcset -t sol -d activate -v <option> <mode>\r\n");
    g_printf("Options are:\r\n");

    ret = dal_get_object_handle_nth(CLASS_NAME_SOL_MANAGEMENT, 0, &obj_handle);
    if (ret != RET_OK) {
        return;
    }
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SOL_MANAGEMENT_COMID, &com_id);
    if (ret != RET_OK) {
        return;
    }

    
    ret = dal_get_specific_object_byte(SOL_CLASS_NAME, PROPERTY_SOL_COM_ID, com_id, &obj_handle);
    
    if (ret == RET_OK) {
        (void)print_com_info(obj_handle);

        
        g_printf("Mode :\r\n");
        g_printf("    0    Shared mode\r\n");
        g_printf("    1    Private mode\r\n");
        return;
    }
    
    ret = dfl_get_object_list(SOL_CLASS_NAME, &obj_list);
    if (ret != DFL_OK || g_slist_last(obj_list) == 0) {
        return;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        ret = print_com_info((OBJ_HANDLE)obj_note->data);
        if (ret != RET_OK) {
            break;
        }
    }
    g_printf("Mode :\r\n");
    g_printf("    0    Shared mode\r\n");
    g_printf("    1    Private mode\r\n");
    g_slist_free(obj_list);
    return;
}


void print_sol_timeout_usage(void)
{
    g_printf("Usage: ipmcset -t sol -d timeout -v <value>\r\n");
    g_printf("Value:   \r\n");
    g_printf("    0       No limit\r\n");
    g_printf("    1-480   SOL timeout period(min)\r\n");
}

void print_rtc_time_usage(void)
{
    g_printf("Usage: ipmcset %s-d time -v 1970-01-01 00:00:01\r\n", help_default_name);
    return;
}


#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V4
void print_clear_log_usage(void)
{
    g_printf("Usage: ipmcset -d clearlog -v <value>\r\n");
    g_printf("Value:   \r\n");
    g_printf("    0       Operation log\r\n");
    g_printf("    1       Run log\r\n");
    g_printf("    2       Security log\r\n");
}
#endif


void print_precisealarm_mock_help(void)
{
    g_printf("Usage:ipmcset %s-t precisealarm -d mock -v <eventcode/stopall> [subjectindex] [assert/deassert/stop]\r\n",
        help_default_name);
    g_printf("Parameters are:\r\n");
    g_printf("  eventcode   Event object code for mock, <eventcode> must be hexadecimal  e.g.:0xffffffff\r\n");
    g_printf("  stopall     Stop all event object mock\r\n");
    g_printf("\r\n");
    g_printf("  subjectindex    Mock the specific event object alarm  [subjectindex] range <1-65535>\r\n");
    g_printf("  assert          Assert event object alarm\r\n");
    g_printf("  deassert        Deassert event object alarm\r\n");
    g_printf("  stop            Stop event object mocking\r\n");
    g_printf("\r\nexample:\n");
    g_printf("      ipmcset -t precisealarm -d mock -v stopall\r\n");
    g_printf("      ipmcset -t precisealarm -d mock -v 0xffffffff assert\r\n");
    g_printf("      ipmcset -t precisealarm -d mock -v 0x04000007 1 assert\r\n");
}


void print_maintenance_raidcom_usage(void)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *node = NULL;
    guint8 com_channel = 0xff;
    gchar controller_name[PROP_VAL_LENGTH] = {0};

    g_printf("Usage: ipmcset -t maintenance -d raidcom -v <value>\r\n");
    g_printf("Values are:\r\n");

    ret = dfl_get_object_list(CLASS_RAID_CONTROLLER_NAME, &obj_list);
    if ((ret != DFL_OK) || g_slist_length(obj_list) == 0) {
        return;
    }

    for (node = obj_list; node; node = g_slist_next(node)) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)(node->data), PROPERTY_RAID_CONTROLLER_COM_CHANNEL, &com_channel);
        if (ret != RET_OK) {
            continue;
        }
        ret = dal_get_property_value_string((OBJ_HANDLE)(node->data), PROPERTY_RAID_CONTROLLER_COMPONENT_NAME,
            controller_name, sizeof(controller_name));
        if (ret != RET_OK) {
            continue;
        }
        if (com_channel != 0xff) {
            g_printf("    %-5d %s\r\n", com_channel, controller_name);
        }
    }

    g_slist_free(obj_list);
    return;
}


void print_set_fpga_golden_fw_restore_usage(gchar *product_type_name)
{
    guchar arm_enable = 0;
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);
    g_printf("Usage: ipmcset -d fpgagoldenfwrestore -v <slotid> [position]\r\n");
    g_printf("Slotid: Slot ID of the FPGA card. You can obtain the PCIe device information from the System Info page ");
    g_printf("on the %s WebUI.\r\n", product_type_name);
    if (arm_enable) {
        g_printf("Position: No need to be filled\r\n");
    } else {
        g_printf("Position: If this is 8100 server needs to be filled\r\n");
    }
}


void print_mesh_mpath_mode_usage(void)
{
    g_printf("Usage: ipmcset -t mesh -d multiplepath -v <option>\r\n");
    g_printf("MultiplePath:   \r\n");
    g_printf("    0       Set mesh multiple path disabled mode\r\n");
    g_printf("    1       Set mesh multiple path enabled mode\r\n");
}


void print_leak_policy_usage(void)
{
    g_printf("Usage : ipmcset -d leakagestrategy -v <strategy>\r\n");
    g_printf("Strategies are:\r\n");
    g_printf("    0    manual power-off after leakage\r\n");
    g_printf("    1    auto power-off after leakage\r\n");
}


void print_set_psu_supply_source_usage(void)
{
    g_printf("Usage: ipmcset -t maintenance %s-d psusupplysource -v <option>\r\n", help_default_name);
    g_printf("Note:\r\n");
    g_printf("Ensure that the standby power supply voltage is normal before line maintenance\r\n");
    g_printf("Abnormal standby power supply voltage poses the power-off risks\r\n");
    g_printf("Options are:\r\n");
    g_printf("0 indicates the input A power supply\r\n");
    g_printf("1 indicates the input B power supply\r\n");
    return;
}
