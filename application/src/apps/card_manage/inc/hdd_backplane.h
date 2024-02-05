

#ifndef __HDD_BACKPLANE_H__
#define __HDD_BACKPLANE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 



#define SOFTWARE_PREFIX_HDDBACKPLANE 0x40
#define HDD_BACKPLANE_PCB_VER 15
#define HDD_BACKPLANE_LOGIC_VER 15
#define HDD_BACKPLANE_DUMPINFO_MAX_LEN 255


#define HDD_OWNSLOTID_INIT 0xFF
#define HDD_EVENT_SUPPORT 1
#define HDD_EVENT_UNSUPPORT 0
#define DISK_PRESENT 1
#define DISK_ABSENT 0
#define PLUGIN_EVENT 1
#define PLUGOUT_EVENT 0


#define POWER_CONTROL_CMD_POWER_OFF 1 // 写1所有硬盘下电
#define POWER_CONTROL_CMD_NORMAL 0    // 写0表示默认，与单板上下电一致

extern gint32 hdd_backplane_init(void);
extern gint32 hdd_backplane_start(void);
extern gint32 hdd_backplane_add_object_callback(OBJ_HANDLE object_handle);
extern gint32 hdd_backplane_del_object_callback(OBJ_HANDLE object_handle);
extern gint32 hdd_backplane_dump_info(const gchar *path);
extern void hdd_backplane_updata_card_info(void);

extern gint32 ipmi_command_get_hdd_plug_cnt(const void *msg_data, gpointer user_data);

extern gint32 hdd_backplane_power_ctrl(const guint16 device_num, const guint8 cmd_code, guint8 *comp_code);


typedef struct tag_HddBackPlaneInfo_s {
    guint8 PlaneId;
    guint8 DiskCnt;
    guint8 *LastPst;
    guint8 *CurPst;
    guint8 *PlugInCnt;
    guint8 *PlugOutCnt;
} HDD_BACKPLANE_INFO_S;



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
