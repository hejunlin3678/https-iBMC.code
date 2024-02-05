

#ifndef __FPGA_CARD_H__
#define __FPGA_CARD_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 


#define FPGA_CARD_DUMPINFO_MAX_LEN 255

extern gint32 fpga_card_init(void);
extern gint32 fpga_card_start(void);
extern gint32 fpga_card_add_object_callback(OBJ_HANDLE object_handle);
extern gint32 fpga_card_del_object_callback(OBJ_HANDLE object_handle);
extern gint32 fpga_card_dump_info(const gchar *path);
void update_all_npus_bdf_task(void);
gint32 get_op_temp_volt_and_power_current_from_mcu(PM_CHIP_INFO *pm_chip_info, guint8 port_num,
    OP_TEMP_VOLT_DOUBLE *optical_temp_volt, OP_POWER_CURRENT *optical_power_current);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif
