

#include "ipmcget_component_boards_version.h"
#include "ipmcget_component_version.h"

#define PROP_LEN 128

void print_riser_boardid_pcb_version(void)
{
#define RISER_CARD_NAME "Riser"
    OBJ_HANDLE riser_handle = 0;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    guint16 board_id = 0;
    gchar blade_name[PROP_LEN] = {0};
    gchar pcb_version[PROP_LEN] = {0};
    gchar logic_version[PROP_LEN] = {0};
    guint32 logic_unit = 0;
    guchar slot_id = 0;

    gint32 ret = dfl_get_object_list(CLASS_RISERPCIECARD_NAME, &obj_list);
    if (ret != DFL_OK || g_slist_length(obj_list) == 0) {
        return;
    }

    g_printf("--------------- Riser Card INFO ----------------\r\n");

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        
        riser_handle = (OBJ_HANDLE)obj_note->data;
        (void)dal_get_property_value_string(riser_handle, PROPERTY_RISERPCIECARD_NAME, blade_name, sizeof(blade_name));
        (void)dal_get_property_value_uint16(riser_handle, PROPERTY_RISERPCIECARD_BOARDID, &board_id);
        (void)dal_get_property_value_string(riser_handle, PROPERTY_RISERPCIECARD_PCBVER, pcb_version,
            sizeof(pcb_version));
        (void)dal_get_property_value_string(riser_handle, PROPERTY_RISERPCIECARD_LOGICVER, logic_version,
            sizeof(logic_version));
        (void)dal_get_property_value_uint32(riser_handle, PROPERTY_RISERPCIECARD_LOGICUNIT, &logic_unit);
        (void)dal_get_property_value_byte(riser_handle, PROPETRY_RISERPCIECARD_SLOT, &slot_id);

        g_printf("%s%-5d   BoardName:           %s\r\n", RISER_CARD_NAME, slot_id, blade_name);
        g_printf("%s%-7d   BoardID:           0x%04x\r\n", RISER_CARD_NAME, slot_id, board_id);

        print_component_uid(riser_handle, RISER_CARD_NAME, slot_id, 12); // 按12字节左对齐

        
        if ((strlen(pcb_version) != 0) && (strcmp(pcb_version, "N/A") != 0)) {
            g_printf("%s%-11d   PCB:           %s\r\n", RISER_CARD_NAME, slot_id, pcb_version);
        }

        if ((strlen(logic_version) != 0) && (strcmp(logic_version, "N/A") != 0)) {
            g_printf("%s%-2d   CPLD Version:           (U%d)%s\r\n", RISER_CARD_NAME, slot_id, logic_unit,
                logic_version);
        }
    }

    g_slist_free(obj_list);
    return;
}

void print_cpuboard_boardid_pcb_version(void)
{
    #define CPU_BOARD_NAME "CpuBoard"
    OBJ_HANDLE board_handle = 0;
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    gchar pcb_ver[PROP_LEN] = {0};
    guint32 logic_unit = 0;
    gchar logic_ver[PROP_LEN] = {0};
    guint16 board_id = 0;
    gchar blade_name[PROP_LEN] = {0};
    gchar ver_data[PROP_LEN] = {0};
    guchar slot_id = 0;

    ret = dfl_get_object_list(CLASS_CPUBOARD_NAME, &obj_list);
    if (ret != DFL_OK || g_slist_length(obj_list) == 0) {
        return;
    }
    g_printf("---------------- CPU Board INFO ----------------\r\n");
    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        board_handle = (OBJ_HANDLE)obj_note->data;
        (void)dal_get_property_value_string(board_handle, PROPERTY_CPUBOARD_NAME, blade_name, sizeof(blade_name));
        (void)dal_get_property_value_string(board_handle, PROPERTY_CPUBOARD_PCBVER, pcb_ver, sizeof(pcb_ver));
        (void)dal_get_property_value_uint16(board_handle, PROPERTY_CPUBOARD_BOARDID, &board_id);
        (void)dal_get_property_value_uint32(board_handle, PROPERTY_CPUBOARD_LOGICUNIT, &logic_unit);
        (void)dal_get_property_value_string(board_handle, PROPERTY_CPUBOARD_LOGICVER, logic_ver, sizeof(logic_ver));
        (void)dal_get_property_value_string(board_handle, PROPERTY_CPUBOARD_BIOSVER, ver_data, sizeof(ver_data));
        (void)dal_get_property_value_byte(board_handle, PROPERTY_CPUBOARD_SLOT, &slot_id);

        if (dal_match_product_id(PRODUCT_ID_TIANCHI) == TRUE) {
            g_printf("%s%-4d BoardName:           %s\r\n", CPU_BOARD_NAME, slot_id, blade_name);
            g_printf("%s%-6d BoardID:           0x%04x\r\n", CPU_BOARD_NAME, slot_id, board_id);
            print_component_uid(board_handle, CPU_BOARD_NAME, slot_id, 9); // 按9字节左对齐
            g_printf("%s%-10d PCB:           %s\r\n", CPU_BOARD_NAME, slot_id, pcb_ver);
            if (strlen(logic_ver) != 0) {
                g_printf("%s%-1d CPLD Version:           (U%d)%s\r\n", CPU_BOARD_NAME, slot_id, logic_unit, logic_ver);
            }
        } else {
            g_printf("%-14s BoardID:           0x%04x\r\n", blade_name, board_id);
            print_component_uid(board_handle, blade_name, INVALID_UINT32, 17); // 按17字节左对齐
            g_printf("%-18s PCB:           %s\r\n", blade_name, pcb_ver);
            if (strlen(logic_ver) != 0) {
                g_printf("%-8s CPLD Version:           (U%d)%s\r\n", blade_name, logic_unit, logic_ver);
            }
            if (strlen(ver_data) != 0) {
                g_printf("%-7s IOCtrl Version:           %s\r\n", blade_name, ver_data);
            }
        }
    }
    g_slist_free(obj_list);
    return;
}

void print_expboard_boardid_pcb_version(void)
{
#define EXP_BOARD_NAME "ExpBoard"
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    guint32 logic_unit = 0;
    guint16 board_id = 0;
    guchar slot_id = 0;
    gchar blade_name[NAME_LEN] = { 0 };
    gchar pcb_ver[PROP_LEN] = { 0 };
    gchar logic_ver[PROP_LEN] = { 0 };

    ret = dfl_get_object_list(CLASS_EXPBOARD_NAME, &obj_list);
    if (ret != DFL_OK || g_slist_length(obj_list) == 0) {
        return;
    }

    g_printf("---------------- EXP Board INFO ----------------\r\n");

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_note->data, PROPERTY_EXPBOARD_NAME,
            blade_name, sizeof(blade_name));
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_note->data, PROPERTY_EXPBOARD_PCBVER,
            pcb_ver, sizeof(pcb_ver));
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_note->data, PROPERTY_EXPBOARD_LOGICVER,
            logic_ver, sizeof(logic_ver));
        (void)dal_get_property_value_uint16((OBJ_HANDLE)obj_note->data, PROPERTY_EXPBOARD_BOARDID, &board_id);
        (void)dal_get_property_value_uint32((OBJ_HANDLE)obj_note->data, PROPERTY_EXPBOARD_LOGICUNIT, &logic_unit);
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROPERTY_EXPBOARD_SLOT, &slot_id);

        g_printf("%s%-4d BoardName:           %s\r\n", EXP_BOARD_NAME, slot_id, blade_name);
        g_printf("%s%-6d BoardID:           0x%04x\r\n", EXP_BOARD_NAME, slot_id, board_id);
        print_component_uid((OBJ_HANDLE)obj_note->data, EXP_BOARD_NAME, slot_id, 9); // 按9字节左对齐
        g_printf("%s%-10d PCB:           %s\r\n", EXP_BOARD_NAME, slot_id, pcb_ver);

        if (strlen(logic_ver) != 0) {
            g_printf("%s%-1d CPLD Version:           (U%d)%s\r\n", EXP_BOARD_NAME, slot_id, logic_unit,
                logic_ver);
        }
    }

    g_slist_free(obj_list);
    return;
}