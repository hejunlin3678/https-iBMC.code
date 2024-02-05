

#include "ipmcget_component_mgmt.h"
#include "ipmcget_component_version.h"
#include "ipmcget_component_boards_version.h"
#include "ipmcget_card_version.h"

const gchar* led_mode_str[] = {
    "Local Control",
    "Override",
    "Lamp Test",
};

const gchar* led_color_str[] = {"reserved", "BLUE", "RED", "GREEN", "AMBER", "ORANGE", "WHITE"};


LOCAL gint32 get_led_state(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    guchar mode;
    const gchar *name = NULL;
    guchar state;
    guchar color;
    guint8 OnDuration;
    guint8 OffDuration;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    
    property_name_list = g_slist_append(property_name_list, NAME);
    property_name_list = g_slist_append(property_name_list, MODE);
    property_name_list = g_slist_append(property_name_list, STATE);
    property_name_list = g_slist_append(property_name_list, COLOR);
    property_name_list = g_slist_append(property_name_list, ON_DURATION);
    property_name_list = g_slist_append(property_name_list, OFF_DURATION);

    ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value);
    if (ret != DFL_OK) {
        g_printf("Get led state info failed.\r\n");
        g_slist_free(property_name_list);
        return RET_ERR;
    }

    
    name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 0), 0);

    
    mode = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 1));

    
    state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 2));

    
    color = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 3));
    OnDuration = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 4));
    OffDuration = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 5));

    g_printf("LED Name               : %s\r\n", name);
    g_printf("LED Mode               : %s\r\n", (mode > 2) ? "unknown" : led_mode_str[mode]); // led_mode_str下标最大2
    g_printf("LED State              : ");

    if (state == LED_STATE_OFF) {
        g_printf("OFF\r\n");
    } else if (state == LED_STATE_ON) {
        g_printf("ON\r\n");
    } else {
        g_printf("BLINKING\r\n");
        g_printf("Off Duration           : %d ms\r\n", OffDuration * 10);  // 乘10转毫秒
        g_printf("On Duration            : %d ms\r\n", OnDuration * 10);  // 乘10转毫秒
    }

    g_printf("LED Color              : %s\r\n", (color > LED_COLOR_NUM - 1) ? "reserved" : led_color_str[color]);
    g_slist_free(property_name_list);
    uip_free_gvariant_list(property_value);
    return RET_OK;
}


LOCAL gint32 get_led_colerprops(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    guint32 i;
    guchar capabilities;
    guchar local_color;
    guchar override_color;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    
    property_name_list = g_slist_append(property_name_list, COLOR_CAPABILITIE);
    property_name_list = g_slist_append(property_name_list, DEFAULT_LCS_COLOR);
    property_name_list = g_slist_append(property_name_list, DEFAULT_OS_COLOR);

    ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value);
    if (ret != DFL_OK) {
        g_printf("Get led color property failed.\r\n");
        g_slist_free(property_name_list);
        return RET_ERR;
    }

    
    capabilities = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));

    
    override_color = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 1));

    
    local_color = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 2));

    g_printf("LED Color Capabilities : ");

    for (i = 0; i < LED_COLOR_NUM; i++) {
        if (capabilities & (0x01 << i)) {
            g_printf("%s ", led_color_str[i]);
        }
    }

    g_printf("\r\n");

    g_printf("Default LED Color in\n\r");
    g_printf("      Local Control    : %s\n\r",
        (local_color > LED_COLOR_NUM - 1) ? "reserved" : led_color_str[local_color]);
    g_printf("      Override State   : %s\n\r",
        (override_color > LED_COLOR_NUM - 1) ? "reserved" : led_color_str[override_color]);
    g_printf("\r\n");
    g_slist_free(property_name_list);
    uip_free_gvariant_list(property_value);
    return RET_OK;
}


LOCAL gint32 print_led_info(OBJ_HANDLE obj_handle, gpointer user_data)
{
    gint32 ret;

    
    ret = get_led_state(obj_handle);
    if (ret != RET_OK) {
        g_printf("Get led state info failed.\r\n");
        return RET_ERR;
    }

    
    ret = get_led_colerprops(obj_handle);
    if (ret != RET_OK) {
        g_printf("Get led color property failed.\r\n");
        return RET_ERR;
    }

    return RET_OK;
}


gint32 ipmc_get_ledinfo(guchar privilege, const gchar* rolepriv,
                        guchar argc, gchar** argv, gchar* target)
{
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    gint32 ret = dfl_foreach_object(LED_CLASS_NAME, print_led_info, NULL, NULL);
    if (ret != RET_OK) {
        g_printf("Get led information failed.\r\n");
        return FALSE;
    }

    return TRUE;
}


gint32 ipmc_get_raid_controller_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *node = NULL;
    guint32 obj_count = 0;
    guint8 idx = 0;
    gint32 raid_index = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    
    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcget -t storage -d ctrlinfo -v <Option>\r\n");
        g_printf("Option:\r\n");
        g_printf("\tall   - Show all RAID controllers.\r\n");
        g_printf("\tID    - Show one RAID controller with this ID(An non-negative integer base 0). eg. 0,1,2...\r\n");
        return FALSE;
    }

    ret = dfl_get_object_list(CLASS_RAID_CONTROLLER_NAME, &obj_list);
    if (ret != DFL_OK || obj_list == NULL) {
        g_printf("Not found RAID controllers.\r\n");
        return FALSE;
    }

    
    if (!strcmp(argv[1], "all")) {
        for (node = obj_list; node; node = g_slist_next(node)) {
            idx = 0xFF;
            (void)dal_get_property_value_byte((OBJ_HANDLE)(node->data), PROPERTY_RAID_CONTROLLER_ID, &idx);

            g_printf("RAID Controller #%u Information\n", idx);
            g_printf("----------------------------------------------------------------------\n");
            print_raid_controller_info((OBJ_HANDLE)(node->data));
            g_printf("----------------------------------------------------------------------\n\n");
        }
    } else {
        raid_index = str_to_int(argv[1]);
        if (raid_index < 0) {
            g_printf("Invalid RAID controller ID.\r\n");
            g_slist_free(obj_list);
            return FALSE;
        }

        obj_count = g_slist_length(obj_list);
        for (node = obj_list; node; node = g_slist_next(node)) {
            idx = 0xFF;
            (void)dal_get_property_value_byte((OBJ_HANDLE)(node->data), PROPERTY_RAID_CONTROLLER_ID, &idx);

            if (idx == raid_index) {
                g_printf("RAID Controller #%u Information\n", idx);
                g_printf("----------------------------------------------------------------------\n");
                print_raid_controller_info((OBJ_HANDLE)(node->data));
                g_printf("----------------------------------------------------------------------\n\n");
                break;
            }
        }

        if (node == NULL) {
            g_printf("RAID controller ID is out of range[0 to %d].\r\n", obj_count - 1);
            g_slist_free(obj_list);
            return FALSE;
        }
    }

    g_slist_free(obj_list);
    return TRUE;
}


gint32 ipmc_get_logical_drive_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *node = NULL;
    guint32 obj_count;
    guint8 idx = 0;
    gint32 raid_index;
    guint8 oob_support = 0;
    gint32 ld_target_id = 0;
    guint8 sms_state = 0xFF;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    
    if ((argc != 3) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcget -t storage -d ldinfo -v <ContollerID> <Option>\r\n");
        g_printf("ContollerID : RAID Controller ID (An non-negative integer base 0). eg. 0,1,2...\r\n");
        g_printf("Option:\r\n");
        g_printf("\tall  - Show all logical drives in this RAID controller.\r\n");
        g_printf("\tID   - Show only one logical drive which matches with this ID (0 - 255).\r\n");
        return FALSE;
    }

    ret = dfl_get_object_list(CLASS_RAID_CONTROLLER_NAME, &obj_list);
    if (ret != DFL_OK || obj_list == NULL) {
        g_printf("Not found RAID controller.\r\n");
        return FALSE;
    }

    
    raid_index = str_to_int(argv[1]);
    if (raid_index < 0) {
        g_printf("Invalid RAID controller index.\r\n");
        g_slist_free(obj_list);
        return FALSE;
    }

    obj_count = g_slist_length(obj_list);
    for (node = obj_list; node; node = g_slist_next(node)) {
        idx = 0xFF;
        (void)dal_get_property_value_byte((OBJ_HANDLE)(node->data), PROPERTY_RAID_CONTROLLER_ID, &idx);

        if (idx == raid_index) {
            break;
        }
    }

    if (node == NULL) {
        g_printf("RAID controller index out of range[0 to %d].\r\n", obj_count - 1);
        g_slist_free(obj_list);
        return FALSE;
    }

    // 检测BMA的SMS连接状态
    
    (void)dal_get_bma_connect_state(&sms_state);
    

    ret = dal_get_property_value_byte((OBJ_HANDLE)(node->data), PROPERTY_RAID_CONTROLLER_OOB_SUPPORT, &oob_support);
    if ((sms_state == SMS_BMA_GLOBAL_STATE_NOTOK) && (ret != DFL_OK || oob_support == 0)) {
        g_printf("This RAID controller do not support out-of-band management.\r\n");
        g_slist_free(obj_list);
        return FALSE;
    }

    if (!strcmp(argv[2], "all")) {
        find_logical_drive((OBJ_HANDLE)(node->data), -1);
    } else {
        ld_target_id = str_to_int(argv[2]);
        if (ld_target_id < 0 || ld_target_id > 0xFFFF) {
            g_printf("Invalid Logical Drive Target Id.\r\n");
            g_slist_free(obj_list);
            return FALSE;
        }
        find_logical_drive((OBJ_HANDLE)(node->data), ld_target_id);
    }

    g_slist_free(obj_list);
    return TRUE;
}


gint32 ipmc_get_physical_drive_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 pd_id = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    
    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcget -t storage -d pdinfo -v <Option>\r\n");
        g_printf("Option:\r\n");
        g_printf("\tall - Show all physical drives.\r\n");
        g_printf("\tID  - Show only one physical drive which matches with this ID (0 - 255).\r\n");
        return FALSE;
    }

    if (!strcmp(argv[1], "all")) {
        find_physical_drive(-1);
    } else {
        pd_id = str_to_int(argv[1]);
        if (pd_id < 0) {
            g_printf("Invalid Physical Drive ID.\r\n");
            return FALSE;
        }

        find_physical_drive(pd_id);
    }

    return TRUE;
}


gint32 ipmc_get_disk_array_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *node = NULL;
    guint32 obj_count;
    guint8 idx = 0;
    gint32 raid_index;
    guint8 oob_support = 0;
    gint32 array_id = 0;
    guint8 sms_state = 0xFF;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    
    if ((argc != 3) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcget -t storage -d arrayinfo -v <ContollerID> <Option>\r\n");
        g_printf("ContollerID : RAID controller ID (An non-negative integer base 0). eg. 0,1,2...\r\n");
        g_printf("Option:\r\n");
        g_printf("\tall   - Show all disk arrays attached to this RAID controller.\r\n");
        g_printf("\tID    - Show only one disk array which matches with this ID.\r\n");
        return FALSE;
    }

    ret = dfl_get_object_list(CLASS_RAID_CONTROLLER_NAME, &obj_list);
    if (ret != DFL_OK || obj_list == NULL) {
        g_printf("Not found RAID controller.\r\n");
        return FALSE;
    }

    
    raid_index = str_to_int(argv[1]);
    if (raid_index < 0) {
        g_printf("Invalid RAID controller index.\r\n");
        g_slist_free(obj_list);
        return FALSE;
    }

    obj_count = g_slist_length(obj_list);
    for (node = obj_list; node; node = g_slist_next(node)) {
        idx = 0xFF;
        (void)dal_get_property_value_byte((OBJ_HANDLE)(node->data), PROPERTY_RAID_CONTROLLER_ID, &idx);

        if (idx == raid_index) {
            break;
        }
    }

    if (node == NULL) {
        g_printf("RAID controller index out of range[0 to %d].\r\n", obj_count - 1);
        g_slist_free(obj_list);
        return FALSE;
    }

    
    // 检测BMA的SMS连接状态
    
    (void)dal_get_bma_connect_state(&sms_state);
    

    ret = dal_get_property_value_byte((OBJ_HANDLE)(node->data), PROPERTY_RAID_CONTROLLER_OOB_SUPPORT, &oob_support);
    if (sms_state != SMS_BMA_GLOBAL_STATE_OK && (ret != RET_OK || oob_support == 0)) {
        g_printf("This RAID controller do not support out-of-band management.\r\n");
        g_slist_free(obj_list);
        return FALSE;
    }

    

    if (!strcmp(argv[2], "all")) {
        find_disk_array((OBJ_HANDLE)(node->data), -1);
    } else {
        array_id = str_to_int(argv[2]);
        if (array_id < 0) {
            g_printf("Invalid Disk Array ID.\r\n");
            g_slist_free(obj_list);
            return FALSE;
        }

        find_disk_array((OBJ_HANDLE)(node->data), array_id);
    }

    g_slist_free(obj_list);
    return TRUE;
}


LOCAL gint32 print_composition_component_get_identifier_list(GSList **ep_identifier_list)
{
    gint32 ret = dfl_get_object_list(CLASS_NAME_ENDPOINT_IDENTIFIER, ep_identifier_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: cann't get any object handle for %s, ret is %d", __FUNCTION__,
            CLASS_NAME_ENDPOINT_IDENTIFIER, ret);
        g_printf("Get [domain] failed.\r\n");
        return ret;
    }
    g_printf("  [domain]:   ");
    return ret;
}


LOCAL void print_composition_component_domain_id(void)
{
    GSList *ep_identifier_list = NULL;
    GSList *list_iter = NULL;
    guchar              domain_id_exist_flag[MAX_COMP_DOMAIN_NUM] = {0};
    OBJ_HANDLE ep_identifier_handle;
    guchar domain_id = 0;
    gchar               composition_id_slot[STRING_LEN_MAX] = {0};

    gint32 ret = print_composition_component_get_identifier_list(&ep_identifier_list);
    if (ret != DFL_OK) {
        return;
    }

    for (list_iter = ep_identifier_list; list_iter != NULL; list_iter = list_iter->next) {
        ep_identifier_handle = (OBJ_HANDLE)list_iter->data;

        (void)dal_get_property_value_byte(ep_identifier_handle, PROPERTY_NAME_EP_IDENTIFIER_DOMAIN_ID, &domain_id);
        if (domain_id > MAX_COMP_DOMAIN_NUM) {
            debug_log(DLOG_ERROR, "%s: Unknown domain id [%d]", __FUNCTION__, domain_id);
            continue;
        }

        guchar container_presence = 0;
        (void)dal_get_property_value_byte(ep_identifier_handle, PROPERTY_NAME_EP_CONTAINER_PRESENCE,
            &container_presence);
        if (container_presence != 1) {
            debug_log(DLOG_DEBUG, "%s: container for endpoint identifier [%s] is not present.", __FUNCTION__,
                dfl_get_object_name(ep_identifier_handle));
            continue;
        }

        domain_id_exist_flag[domain_id - 1] = 1;
    }

    g_slist_free(ep_identifier_list);

    for (gint32 i = 0; i < MAX_COMP_DOMAIN_NUM; i++) {
        if (domain_id_exist_flag[i] == 0) {
            continue;
        }

        ret = snprintf_s(composition_id_slot + strlen(composition_id_slot),
            sizeof(composition_id_slot) - strlen(composition_id_slot),
            sizeof(composition_id_slot) - strlen(composition_id_slot) - 1, "domain%d,", i + 1);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: format component id failed, ret is %d", __FUNCTION__, ret);
            continue;
        }
    }

    guint32 len = strlen(composition_id_slot);
    
    if (len > 0) {
        (composition_id_slot[len - 1] = '\0');
    }
    g_printf("%s\r\n", composition_id_slot);

    return;
}


gint32 ipmc_get_resource_components_list(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    
    guchar domain_id;
    gint32 ret;
    gchar *domain_id_index = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    
    if (((argc != 2) && (argc != 3)) || (strcmp(argv[0], "-v")) || ((argc == 2) && (strcmp(argv[1], "list"))) ||
        ((argc == 3) && (strcmp(argv[2], "list")))) {
        goto print_help_quit;
    }

    
    if (argc == 2) {
        (void)print_resource_components_list_info(COMP_DOMAIN_ALL);
    } else {
        domain_id_index = g_strrstr(argv[1], COMPOSITION_DOMAIN_PREFIX);
        
        if (domain_id_index == NULL) {
            goto print_help_quit;
        }

        ret = vos_str2int(argv[1] + strlen(COMPOSITION_DOMAIN_PREFIX), 10, (void *)&domain_id, NUM_TPYE_UCHAR);
        if (ret != RET_OK) {
            goto print_help_quit;
        }
        

        ret = print_resource_components_list_info(domain_id);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: no endpoint identifier found with domain id [%d]", __FUNCTION__, domain_id);
            goto print_help_quit;
        }
    }

    return TRUE;

print_help_quit:

    
    g_printf("Usage: ipmcget -t resource -d components -v [domain] list\r\n");
    

    g_printf("list   List all disaggregated components which could be bound to systems.\r\n");
    print_composition_component_domain_id();

    return FALSE;
    
}


gint32 ipmc_get_resource_compositions_list(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    
    guchar domain_id = 0;
    gint32 ret;
    GSList *composition_list = NULL;
    GSList *comp_handle_iter = NULL;
    OBJ_HANDLE comp_handle;
    gchar *domain_id_index = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    
    if (((argc != 2) && (argc != 3)) || (strcmp(argv[0], "-v")) || ((argc == 2) && (strcmp(argv[1], "list"))) ||
        ((argc == 3) && (strcmp(argv[2], "list")))) {
        goto print_help_quit;
    }

    
    if (argc == 2) {
        ret = dfl_get_object_list(CLASS_NAME_COMPOSITION, &composition_list);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s:cann't get any Composition handle, ret is %d", __FUNCTION__, ret);
            goto print_help_quit;
        }

        
        composition_list = g_slist_sort(composition_list, (GCompareFunc)sort_composition_handle_by_slot);
        for (comp_handle_iter = composition_list; comp_handle_iter != NULL; comp_handle_iter = comp_handle_iter->next) {
            comp_handle = (OBJ_HANDLE)comp_handle_iter->data;
            print_resource_compositions_list_info(comp_handle);

            
            if (comp_handle_iter->next != NULL) {
                g_printf("\r\n");
            }
        }

        g_slist_free(composition_list);
    } else {
        domain_id_index = g_strrstr(argv[1], COMPOSITION_DOMAIN_PREFIX);
        if (domain_id_index == NULL) {
            debug_log(DLOG_ERROR, "%s failed: cann't find domain prefix from %s", __FUNCTION__,
                COMPOSITION_DOMAIN_PREFIX);
            goto print_help_quit;
        }

        ret = vos_str2int(domain_id_index + strlen(COMPOSITION_DOMAIN_PREFIX), 10, (void *)&domain_id, NUM_TPYE_UCHAR);
        
        if (ret != RET_OK) {
            goto print_help_quit;
        }
        

        comp_handle = 0;
        ret = dal_get_specific_object_byte(CLASS_NAME_COMPOSITION, PROPERTY_COMPOSITION_SLOT, domain_id, &comp_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s failed: invalid domain number:%d", __FUNCTION__, domain_id);
            goto print_help_quit;
        }

        print_resource_compositions_list_info(comp_handle);
    }

    return TRUE;

print_help_quit:
    g_printf("Usage: ipmcget -t resource -d composition -v [domain] list\r\n");
    g_printf("list : Show current configuration and pending settings of resource composition.\r\n");
    print_composition_node_id();

    return FALSE;
    
}


LOCAL void print_relevant_component_version(void)
{
    
    print_product_version();

    
    if (dal_match_product_id(PRODUCT_ID_TIANCHI) == TRUE) {
        
        print_riser_boardid_pcb_version();
        
        print_hddbackplane_boardid_pcb_version();
        
        print_cpuboard_boardid_pcb_version();
        
        print_expboard_boardid_pcb_version();
        
        print_fanboard_boardid_pcb_version();
    } else {
        
        print_mother_boardid_pcb_version();
        
        print_riser_boardid_pcb_version();
        
        print_hddbackplane_boardid_pcb_version();
        
        print_cpuboard_boardid_pcb_version();
        
        print_fanboard_boardid_pcb_version();
    }
    
    print_nic_boardid_pcb_version();

    
    print_raid_boardid_pcb_version();

    print_passthrough_card_version(); // 作为Raid卡的替代部件，和Raid卡实际上是互斥在位的，放在一起，符合查看逻辑*/

    
    print_memboard_boardid_pcb_version();

    
    print_ioboard_boardid_pcb_version();

    
    print_ncboard_boardid_pcb_version();

    
    print_mezz_boardid_pcb_version();
    
    print_card_boardid_pcb_version();

    
    print_liquid_detect_card_info();

    
    print_lcd_cpld_version();

    
    
    print_ps_version();
    

    print_retimer_version();

    
    
    print_rimm_version();
    

    
    
    print_mm_backplaneBoard_version();
    

    
    
    print_phy_fw_version();
    

    
    print_security_module_info();
    

    
    print_pcie_card_version();
    
    print_pangea_card_mcu_ver();

    print_gpuboard_version();

    
    print_asset_locator_version();
    

    
    print_fabric_version();
    

    
    print_m2_transform_card_version();

    
    print_pcie_transform_card_version();
    

    
    print_horizontal_connection_board_version();
    

    print_vrd_version();

    
    print_converge_board_version();

    return;
}


gint32 ipmc_get_version(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    guchar MgmtSoftWareType = 0;
    gint32 ret;
    gchar custom_name[LOG_SPECIAL_STRING_LEN_MAX] = {"iBMC"};

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }
    ret = dal_get_software_type(&MgmtSoftWareType);
    if (ret != RET_OK) {
        return FALSE;
    }

    if (MgmtSoftWareType == MGMT_SOFTWARE_TYPE_EM) {
        g_printf("------------------- SMM  INFO -------------------\r\n");
        
        print_bmc_version();
    }
    
    else if (MgmtSoftWareType == MGMT_SOFTWARE_TYPE_RM) {
        g_printf("------------------- iRM  INFO -------------------\r\n");
        
        print_bmc_version();
    }
    
    else {
        
        if (dal_get_custom_string_name(custom_name, BMC_CUSTOM_LOGIBMC_NAME, sizeof(custom_name)) != RET_OK) {
            debug_log(DLOG_ERROR, "%s: dal_get_custom_string_name failed.", __FUNCTION__);
        }

        g_printf("------------------- %s INFO -------------------\r\n", custom_name);
        
        print_bmc_version();

        
        print_ipmb_address();
    }
    
    print_relevant_component_version();

    return TRUE;
}
