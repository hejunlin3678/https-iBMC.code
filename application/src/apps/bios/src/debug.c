
#include "debug.h"
#include "boot_options.h"
#include "bios.h"

#define BIOS_DUMP_BOOT_HEAD ("\n------Bios Boot Options Begin------\n")
#define BIOS_DUMP_CHANGE_HEAD ("\n------Bios File Change Begin------\n")
#define BIOS_DUMP_DATA_HEAD ("\n------Bios Data Operation Begin------\n")
#define BIOS_DUMP_DATA_CONT ("%s:\r\ndata_flag:%u,data_offset:%u,data_len:%u\r\n")
#define BIOS_DUMP_CLP_CFG ("\n------CLP Config file Begin------\n")
#define BIOS_DUMP_SMBIOS_CFG ("\n------SmBios Config file Begin------\n")

LOCAL void show_data_operations(BIOS_DATA_OPERATE_S *data_operation);


gint32 bios_show_boot_options(GSList *input_list)
{
    BOOT_OPTIONS_S *boot_option = &g_boot_options;
    guint32 i;

    debug_printf(BIOS_DUMP_BOOT_HEAD);

    debug_printf(" valid_flag:");

    for (i = 0; i < BIOS_BOOT_VALID_SEG_NUM; i++) {
        debug_printf("0x%x ", boot_option->valid_flag[i]);
    }

    debug_printf("set_in_progress:");
    debug_printf("0x%x", boot_option->set_in_progress);

    debug_printf("service_partition_selector:");
    debug_printf("0x%x", boot_option->service_partition_selector);

    debug_printf("service_partition_scan:");
    debug_printf("0x%x", boot_option->service_partition_scan);

    debug_printf("boot_flag_valid_bit_clearing:");
    debug_printf("0x%x", boot_option->boot_flag_valid_bit_clearing);

    debug_printf("boot_info_acknowledge:");

    for (i = 0; i < BIOS_BOOT_ACK_INFO_NUM; i++) {
        debug_printf("0x%x ", boot_option->boot_info_acknowledge[i]);
    }

    debug_printf("boot_flags:");

    for (i = 0; i < BIOS_BOOT_VALID_NUM; i++) {
        debug_printf("0x%x ", boot_option->boot_flags[i]);
    }

    debug_printf("boot_initiator_info:");

    for (i = 0; i < BIOS_BOOT_INIT_INFO_NUM; i++) {
        debug_printf("0x%x ", boot_option->boot_initiator_info[i]);
    }

    debug_printf("boot_initiator_mailbox:");

    for (i = 0; i < BIOS_BOOT_INIT_MAILBOX_NUM; i++) {
        debug_printf("0x%x ", boot_option->boot_initiator_mailbox[i]);
    }

    debug_printf("write_protect:");
    debug_printf("0x%x", boot_option->write_protect.byte);

    debug_printf("------Bios Boot Options End------");

    return RET_OK;
}


gint32 bios_show_file_change(GSList *input_list)
{
    gint32 i = 0;

    debug_printf(BIOS_DUMP_CHANGE_HEAD);

    for (i = 0; i < BIOS_FILE_MAX_SELECTOR; i++) {
        debug_printf(" file_num%d:", i);
        debug_printf("0x%x", g_bios_file_change[i].file_num);

        debug_printf("\n file_change_flag%d:", i);
        debug_printf("0x%x", g_bios_file_change[i].file_change_flag);

        debug_printf("\n file_channel%d:", i);
        debug_printf("0x%x", g_bios_file_change[i].file_channel);
    }

    debug_printf("\n bios_setting_file_state:%d", g_bios_setting_file_state);

    debug_printf("\n------Bios File Change End------\n");

    return RET_OK;
}


gint32 bios_show_data_operations(GSList *input_list)
{
    debug_printf(BIOS_DUMP_DATA_HEAD);

    debug_printf("\n---smbios_wr_data_cfg---");
    show_data_operations(&g_smbios_wr_data_cfg);

    debug_printf("\n---bios_rd_data_cfg---");
    show_data_operations(&g_bios_rd_data_cfg);

    debug_printf("\n---bios_wr_data_cfg---");
    show_data_operations(&g_bios_wr_data_cfg);

    debug_printf("\n---bios_imu_wr_data_cfg---");
    show_data_operations(&g_bios_imu_wr_data_cfg);
 
    debug_printf("\n------Bios Data Operation End------\n");

    return RET_OK;
}


LOCAL void show_data_operations(BIOS_DATA_OPERATE_S *data_operation)
{
    if (data_operation == NULL) {
        debug_log(DLOG_ERROR, "%s: data_operation is NULL", __FUNCTION__);
        return;
    }

    console_printf("\n data_flag:");
    console_printf("0x%x", data_operation->data_flag);

    console_printf("\n data_offset:");
    console_printf("0x%x", data_operation->data_offset);

    console_printf("\n data_len:");
    console_printf("0x%x", data_operation->data_len);
}


gint32 show_upload_firmware_info(GSList *input_list)
{
    debug_printf("\n------Upload Firmware Info Begin------\n");

    debug_printf("\n---upload firmware info---");
    show_data_operations(&g_upg_fw_data_cfg);

    debug_printf("\n------Upload Firmware Info End------\n");

    return RET_OK;
}