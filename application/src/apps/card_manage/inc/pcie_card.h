

#ifndef __PCIE_CARD_H__
#define __PCIE_CARD_H__

#include <ctype.h>
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 




#define PCIE_MONITOR_TASK_SLEEP_TIME 2000
#define TIME_UNIT_SECOND 1000
#define TIME_MULTIPLE 3
#define INIT_STATE_INCOMPLETE 0
#define INIT_STATE_SUCCESS 1
#define INIT_STATE_FAIL 2



#define MAX_PCIE_CARD 200
#define PCIE_CARD_DUMPINFO_MAX_LEN 255
#define PCIE_CARD_XMLFILE_MAX_LEN 255

#define PCIE_AND_PCIEPLUS_CONNECTOR 1
#define DISK_CONNECTOR 2
#define NVME_VPD_CONNECTOR 3
#define OCPCARD_CONNECTOR 4
#define PANGEA_CARD_CONNECTOR 5
#define CHIP_CONNECTOR 6
#define NVME_RAID_CONNECTOR 7

#define PCIE_PHYSICAL_PARTITION_MAX_NODE_NUM 8


#define PCIE_CARD_PCB_VERSION_LEN 3




#define VPD_CONNECTOR_BOM_PROTOCOL "14140224_PROTOCOL"

#define CONNECTOR_TYPE_MAX_LEN 32
#define CONNECTOR_BOM_ID_MAX_LEN 32
#define VPD_UNLOAD 0xff
#define VPD_LOAD 1
#define VPD_FIRST_TIME_LOAD 0xfe

#define VPD_COMMON_HEADER_LEN 8
#define VPD_COMMON_MODEL_NUMBER 8
#define VPD_COMMON_MODEL_NUMBER_LEN 65
#define GET_MODEL_NUMBER_LEN 40
#define OFFSET_MODEL_NUMBER_LEN 25


#define NORMAL_OPT_TEMP 1

#define PCIE_SSD_MAX_BUFF_SIZE 128
#define PCIE_CARD_XML_LOAD 1
#define PCIE_CARD_XML_UNLOAD 0

#define PCIE_IDS_FILE "/opt/pme/conf/pcie_ids/pci.ids"
#define PCIE_SKIP_BLANKS(pStr) for (; (*(pStr)) && isspace(*(pStr)); (pStr)++)

#define PCIE_SKIP_XDIGITS(pStr) for (; (*(pStr)) && isxdigit(*(pStr)); (pStr)++)

#define PCIE_SKIP_DIGITS(pStr) for (; (*(pStr)) && isdigit(*(pStr)); (pStr)++)


#define WIDTH_TYPE_X8 3
#define WIDTH_TYPE_X16 4

#define PCIE_NO_CARD 0xffffffff
#define PCIE_INVALID_DID 0xffff
#define PCIE_INVALID_VID 0xffff

#define PCIE_INVALID_SUBDID 0xffff
#define PCIE_INVALID_SUBVID 0xffff
#define PCIE_INVALID_VID_DID 0xffffffff

#define PCIE_INVALID_BOARD_ID 0xfffffffe

#define PCIE_NO_FATALERROR 0
#define PCIE_FATALERROR 1

#define PCIE_PLUG_IN 1
#define PCIE_PLUG_OUT 0
#define PCIE_INVALID_PRESENT 0xff
#define PCIE_INVALID_BDF 0xff
#define PCIE_INVALID_SLOT_ID 0xff
#define CLASSE_NAME_LEN 128
#define DEFAULT_WAIT_TIME 120
#define FIRMWARE_UNKNOW 2

#define REVERT_UINT16(i) (((((i) >> 8) & 0xff) | ((i) << 8)) & 0xffff)
#define SET_ARRAY_VALUE(array, count, value) \
    do {                                     \
        int i = 0;                           \
        for (i = 0; i < (count); i++)          \
            array[i] = value;                \
    } while (0)



#define PCIE_POWER_ON 1
#define PCIE_POWER_OFF 0
#define PCIE_SENSOR_INVALID_READING 0x7fff
#define PCIE_SENSOR_NA_READING 0x7ffd



#define PCIE_3004RAID_VID 0x1000
#define PCIE_3004RAID_DID 0x005F
#define PCIE_3004RAID_SUBVID 0x19e5
#define PCIE_3004RAID_SUBDID 0xd223


#define PCIE_3152RAID_VID 0x9005
#define PCIE_3152RAID_DID 0x028F
#define PCIE_3152RAID_SUBVID 0x9005
#define PCIE_3152RAID_SUBDID 0x0801

#define PCIE_INTEL_VID 0x8086
#define PCIE_INTEL_DH895XCC_DID 0x0435

#define PCIE_SSD_SN 0
#define PCIE_SSD_MN 1
#define PCIE_SSD_FW_VER 2

#define LINK_WIDTH_X1 1
#define LINK_WIDTH_X2 (1 << 1)
#define LINK_WIDTH_X4 (1 << 2)
#define LINK_WIDTH_X8 (1 << 3)
#define LINK_WIDTH_X12 (LINK_WIDTH_X4 | LINK_WIDTH_X8)
#define LINK_WIDTH_X16 (1 << 4)
#define LINK_WIDTH_X32 (1 << 5)



#define PCIE_HUAWEI_SUBVID 0x19e5
#define PCIE_HUAWEI_MANU "Huawei Technologies Co., Ltd."


#define VIRTUAL_FPGA_CARD_DES "Virtual FPGA Card"
#define RESTORE_FROM_IPMI 1
#define RESTORE_FROM_CLI 2

#define NETCARD_MAC_ADDRESS_RESPONSE_LENGTH 9
#define NETCARD_MAC_ADDRESS_LENGTH 18
#define MAC_ADDRESS_LENGTH 6

#define MAX_CARD_CNT 2
#define MAX_ACCESS_ERR_CNT 10

#define PCIE_LINK_CAP_REG_OFFSET 0x0C // pcie link capabilities寄存器偏移
#define PCIE_LINK_CONTROL_OFFSET 0x10 // pcie link control寄存器偏移
#define PCIE_CAP_ID_OFFSET 0x34
#define PCIE_CAP_ID_MASK 0xff
#define PCIE_CAP_NEXT_POINT_OFFSET 8
#define PCIE_CAP_NEXT_POINT_MASK 0xff
#define PCIE_LINK_STATUS_OFFSET 16
#define PCIE_LINK_STATUS_MASK 0xFFFF
#define PCIE_LINK_WIDTH_OFFSET 4
#define PCIE_LINK_WIDTH_MASK 0x3F
#define BMC_XML_PATH "/opt/pme/extern/profile"
#define UPGRADE_XML_PATH "/data/opt/pme/conf/profile"

#define NIC_CARD_DUMPINFO_MAX_LEN 255




typedef struct tag_pcie_card_static_info {
    OBJ_HANDLE handle;
} PCIE_CARD_STATIC_INFO_S;

typedef struct pcie_action_priv_data_s {
    guint8 present;
    guint8 rev1;
    guint8 rev2;
    guint8 rev3;
    guint8 vpd_load;
} PCIE_ACTION_PRIV_DATA_S;

typedef struct tag_pcie_card_info_s {
    guint16 device_id;
    guint16 vender_id;
} PCIE_CARD_INFO_S;

typedef struct tag_pcie_card_sub_info_s {
    guint16 sub_device_id;
    guint16 sub_vender_id;
} PCIE_CARD_SUB_INFO_S;

typedef struct tag_pcie_card_class_info_s {
    guint8 class_code;
    guint8 sub_class_code;
    guint8 prog;
    guint8 revision;
} PCIE_CARD_CLASS_INFO_S;

typedef struct tag_pcie_card_status_s {
    guint8 status[MAX_PCIE_CARD];
} PCIE_CARD_STATUS_GROUP_S;

typedef struct tag_pcie_card_bdf_info_s {
    guint8 segment;
    guint8 cpuid;
    guint8 bus_num;
    guint8 device_num;
    guint8 function_num;
} PCIE_CARD_BDF_INFO_S;


typedef struct info_need_get_pcie_info {
    guint8 is_local; 
    guint8 cpu_num;
    guint8 bus;
    guint8 dev;
    guint8 func;
    guint8 address;
    guint8 length;
} INFO_NEED_GET_PCIE_INFO;

typedef struct tag_pcie_card_group_info_s {
    guint8 count;
    PCIE_CARD_INFO_S info[MAX_PCIE_CARD];
} PCIE_CARD_GROUP_INFO_S;

typedef struct tag_pcie_card_group_sub_info_s {
    guint8 count;
    PCIE_CARD_SUB_INFO_S sub_info[MAX_PCIE_CARD];
} PCIE_CARD_GROUP_SUB_INFO_S;

typedef struct tag_pcie_card_group_class_info_s {
    guint8 count;
    PCIE_CARD_CLASS_INFO_S class_info[MAX_PCIE_CARD];
} PCIE_CARD_GROUP_CLASS_INFO_S;

typedef struct tag_pcie_card_group_bdf_info_s {
    guint8 count;
    PCIE_CARD_BDF_INFO_S bdf_info[MAX_PCIE_CARD];
} PCIE_CARD_GROUP_BDF_INFO_S;


typedef struct tag_node_pcie_summary_info {
    guint8 pcie_info_cnt;
    guint16 pcie_info_array[MAX_PCIE_INFO_ARRAY_LEN];
} NODE_PCIE_SUMMARY_INFO_S;

typedef struct tag_node_pcie_group_summary_info {
    guint8 node_count;
    NODE_PCIE_SUMMARY_INFO_S pcie_summary_info[MAX_NODE_NUM];
} NODE_PCIE_GROUP_SUMMARY_INFO;

typedef struct tag_node_pcie_info {
    
    guint16 container_type;

    
    guint16 container_slot;

    
    guint16 vendor_id;

    
    guint16 device_id;

    
    guint16 sub_vendor_id;

    
    guint16 sub_device_id;

    
    guint16 pcie_slot;
} NODE_PCIE_INFO_S;



typedef struct tag_pcie_bdf_parse_info {
    OBJ_HANDLE obj_handle;
    gchar prop_name[MAX_NAME_SIZE];
} PCIE_BDF_PARSE_INFO;

typedef enum tag_pcie_card_p_type_e {
    PCIE_CARD_P_TYPE_BYTE = 0,
    PCIE_CARD_P_TYPE_UINT16,
    PCIE_CARD_P_TYPE_UINT32,
    PCIE_CARD_P_TYPE_DOUBLE,
    PCIE_CARD_P_TYPE_STRING,
    PCIE_CARD_P_TYPE_INT16
} PCIE_CARD_P_TYPE_E;

typedef enum tag_pcie_dev_stat {
    PCIE_DEV_STAT_TYPE0 = 0,
    PCIE_DEV_STAT_TYPE1 = 0x84,
    PCIE_DEV_STAT_TYPE2 = 0x85,
    PCIE_DEV_STAT_TYPE3 = 0x86,
    PCIE_DEV_STAT_TYPE4 = 0x87,
    PCIE_DEV_STAT_TYPE5 = 0x89,
    PCIE_DEV_STAT_TYPE6 = 0x8A,
    PCIE_DEV_STAT_TYPE7 = 0x8B,
    PCIE_DEV_STAT_TYPE8 = 0x90,
    PCIE_DEV_STAT_TYPE9 = 0x91,
    PCIE_DEV_STAT_TYPE10 = 0x92,
    PCIE_DEV_STAT_TYPE11 = 0x93,
    PCIE_DEV_STAT_TYPE12 = 0xFB,
    PCIE_DEV_STAT_TYPE13 = 0xFC,
    PCIE_DEV_STAT_TYPE14 = 0xFD,
    PCIE_DEV_STAT_TYPE15 = 0xFE,
    PCIE_DEV_STAT_TYPE16 = 0xFA,
    PCIE_DEV_STAT_TYPE17 = 0xF7,
    PCIE_DEV_STAT_TYPE18 = 0xF8,
    PCIE_DEV_STAT_TYPE19 = 0xF9,
    PCIE_DEV_STAT_TYPE20 = 0xF6, 
    PCIE_DEV_STAT_TYPE21 = 0x1   
} PCIE_DEVICE_STATE_E;

typedef struct tag_pcie_event_type {
    guint8 event_type;
    gchar *event_info;
} PCIE_EVENT_TYPE;

typedef struct tag_pcie_card_extra_info {
    guint16 board_id;
    gchar pcb_ver[PCIE_CARD_PCB_VERSION_LEN + 1];
    gchar cpld_ver[FIRMWARE_VERSION_STRING_LEN];
    guint8 sub_card_bom_id;
} PCIE_CARD_EXTRA_INFO;

typedef struct tag_pcie_restore_info {
    guint8 slot_id;
    guint32 position;
    IPMI_MSG_S msg;
    guint8 op_type;
} PCIE_RESTORE_INFO;

typedef struct tag_pcie_card_parse_persave_data {
    PCIE_CARD_GROUP_INFO_S *group_info_s_ptr;
    PCIE_CARD_GROUP_SUB_INFO_S *sub_group_info_s_ptr;
    PCIE_CARD_GROUP_BDF_INFO_S *bdf_group_info_s_ptr;
} PCIE_CARD_PARSE_PERSAVE_DATA_S;
typedef struct tag_xml_action_param {
    OBJ_HANDLE obj_handle;
    guint32 board_id;
    guint32 aux_id;
    const gchar *card_type_string;
    guint8 action;
    gchar *bom_value;
} XML_ACTION_PARAM_S;
typedef struct tag_xml_path_filename_param {
    const gchar *path;
    guint32 board_id;
    guint32 aux_id;
} XML_PATH_FILENAME_PARAM_S;


typedef struct pcie_viddid_svidsdid {
    guint16 vid;
    guint16 did;
    guint16 sub_vid;
    guint16 sub_did;
} PCIE_VIDDID_SVIDSDID;

typedef struct pcie_netcard_viddid_svidsdid {
    const gchar *card_name;
    PCIE_VIDDID_SVIDSDID upid;
    PCIE_VIDDID_SVIDSDID dpid;
    PCIE_VIDDID_SVIDSDID pfid;
} PCIE_NETCARD_VIDDID_SVIDSDID;

typedef struct ocp_card_ipmi_info {
    guint8 slot_id;
    PCIE_CARD_GROUP_INFO_S ocp_card_info;
    PCIE_CARD_GROUP_SUB_INFO_S ocp_card_sub_info;
} OCP_CARD_IPMI_INFO_S;
typedef struct mix_port_rootbdf_info_s {
    guint8 type;
    guint8 segment;
    guint8 slot_id;
    guint8 bus_num;
    guint8 device_num;
    guint8 function_num;
} MIX_PORT_ROOTBDF_INFO_S;

typedef struct mix_port_group_bdf_info_s {
    guint8 count;
    MIX_PORT_ROOTBDF_INFO_S bdf_info[MAX_PCIE_CARD];
} MIX_PORT_GROUP_BDF_INFO_S;

typedef struct tag_port_bdf_info_s {
    guint8 segment;
    guint8 slot_id;
    guint8 bus_num;
    guint8 device_num;
    guint8 function_num;
} PORT_ROOTBDF_INFO_S;

typedef struct tag_port_group_bdf_info_s {
    guint8 count;
    PORT_ROOTBDF_INFO_S bdf_info[MAX_PCIE_CARD];
} PORT_GROUP_BDF_INFO_S;

typedef struct tag_port_info_s {
    guint8 type;
    guint8 slot_id;
    guint8 port_idx;
} PORT_INFO_S;

// #pragma pack(1)     


// #pragma pack()      



extern gint32 pcie_card_load(void);
extern gint32 pcie_card_start(void);
extern gint32 pcie_card_ipmi_set_vender_device_id(const void *msg_data, gpointer user_data);
extern gint32 pcie_disk_ipmi_set_vender_device_id(const void *msg_data, gpointer user_data);
extern gint32 pcie_card_ipmi_set_chip_temp(const void *msg_data, gpointer user_data);
extern gint32 pcie_card_ipmi_set_chip_health_status(const void *msg_data, gpointer user_data);
extern gint32 pcie_card_ipmi_set_device_status(const void *msg_data, gpointer user_data);
extern gint32 pcie_card_ipmi_get_pcie_width_info(const void *msg_data, gpointer user_data);
extern gint32 pcie_card_add_object_callback(OBJ_HANDLE object_handle);
extern gint32 pcie_card_del_object_callback(OBJ_HANDLE object_handle);


extern gint32 extend_pcie_card_add_object_callback(OBJ_HANDLE object_handle);
extern gint32 extend_pcie_card_del_object_callback(OBJ_HANDLE object_handle);


extern gint32 pcie_card_dump_info(const gchar *class_name, const gchar *title, const gchar *path);
extern gint32 nic_card_dump_info(const gchar *class_name, const gchar *title, const gchar *path);
extern void pcie_card_set_device_status_default_value(void);
extern gint32 pcie_card_parse_bdf_info(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
extern gint32 card_action_add_object_callback(OBJ_HANDLE object_handle);
extern gint32 card_action_del_object_callback(OBJ_HANDLE object_handle);

extern gint32 m2transformcard_init(void);



extern gint32 pcie_card_delete_complete_func(guint32 position);


extern gint32 pcie_card_set_obj_property(OBJ_HANDLE handle, const gchar *name, const void *indata,
    PCIE_CARD_P_TYPE_E type, guint8 option);

extern gint m2transformcard_add_object_callback(OBJ_HANDLE obj_handle);
extern gint m2transformcard_del_object_callback(OBJ_HANDLE obj_handle);


extern gint32 pcietransformcard_init(void);

extern gint pcietransformcard_add_object_callback(OBJ_HANDLE obj_handle);
extern gint pcietransformcard_del_object_callback(OBJ_HANDLE obj_handle);

extern gint32 pcie_device_mock(GSList *input_list);



extern gint32 parse_node_pcie_card_info(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);


extern gint32 pcie_mgnt_init_info(OBJ_HANDLE obj_handle, gpointer user_data);

extern gint32 pcie_mgnt_init_virtual_card_info(OBJ_HANDLE obj_handle);

extern gint32 card_manage_bios_state_change(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);

extern gint32 card_manage_smbios_state_change(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
extern gint32 update_netcard_and_port_bdf_info(OBJ_HANDLE netcard_handle, gpointer user_data);

extern gint32 debug_read_viddid_from_me(GSList *input_list);

extern gint32 pcie_card_change_notify_mctp(void);




extern gint32 card_manage_mcu_get_data(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 card_manage_mcu_set_data(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);


extern gint32 upgrade_mcu_send_file(GSList *input_list);




gint32 collect_mcu_log(GSList *input_list);



extern void pcie_card_set_pcieslot_fault_state(guint8 fault_state);
extern guint8 pcie_card_get_pcieslot_fault_state(void);




extern gint32 card_manage_ipmi_mcu_set_device_action(const void *msg_data, gpointer user_data);
extern gint32 card_manage_ipmi_mcu_get_device_action_result(const void *msg_data, gpointer user_data);

extern gint32 pcie_card_update_info(OBJ_HANDLE obj_handle, gpointer user_data);
gint32 ocp_card_update_info(OBJ_HANDLE obj_handle, gpointer user_data);

extern gint32 method_get_pfinfo(OBJ_HANDLE extend_card_obj, GSList *caller_info, GSList *input_list,
    GSList **output_list);

extern gint32 card_manage_ipmi_restore_fpga_card(const void *msg_data, const void *req_data_buf, gint32 *todo_flg);
extern gint32 get_card_obj_handle_by_slot_position(OBJ_HANDLE *obj_handle, guint8 slot_id, guint32 psosition);
extern gint32 ipmi_get_pcie_dev_present_status(const void *msg_data, gpointer user_data);

gint32 ocp_card_add_object_callback(OBJ_HANDLE object_handle);
gint32 ocp_card_del_object_callback(OBJ_HANDLE object_handle);
gint32 pcie_card_get_connector_handle(guint8 slot_id, OBJ_HANDLE *obj_handle, guint8 connector_type);
void get_group_info_by_type(guint8 card_type, PCIE_CARD_GROUP_INFO_S **card_info,
    PCIE_CARD_GROUP_SUB_INFO_S **card_sub_info);
void get_pcie_status_info(PCIE_CARD_STATUS_GROUP_S **status_group, PCIE_CARD_STATUS_GROUP_S **status_by_bios_group);

void ocp_card_set_device_status_default_value(void);
void reset_os_update_resources_state(void);

gboolean is_sdi_support_mcu(OBJ_HANDLE obj_handle);
gboolean is_sdi_support_mcu_by_pciecard(void);

void update_pcie_lane_info(OBJ_HANDLE obj_handle, guint8 slot_id);
gint32 pcie_card_set_pfinfo(OBJ_HANDLE extend_card_handle, PM_CHIP_INFO *pm_chip_info, guint16 pf,
    guint16 physical_port, gchar *mac_addr);

extern PCIE_CARD_GROUP_BDF_INFO_S g_pcie_cards_bdf_info;
gint32 pciecard_test_bdf_is_valid(guint8 socket_id, guint8 bus, guint8 device, guint8 function,
    PCIE_VIDDID_SVIDSDID *pcie_viddid_ptr);
gint32 pciecard_parse_vid_did_subvid_subdid(gconstpointer msg_data, gpointer user_data);
extern PCIE_CARD_GROUP_INFO_S g_pcie_group;
extern PCIE_CARD_GROUP_INFO_S g_pcie_disk_group;
extern PCIE_CARD_STATUS_GROUP_S g_pcie_status_group;
extern PCIE_CARD_GROUP_SUB_INFO_S g_pcie_group_sub_info;
extern PCIE_CARD_GROUP_SUB_INFO_S g_pcie_disk_group_sub_info;
extern PCIE_CARD_GROUP_BDF_INFO_S g_pcie_cards_bdf_info;
extern PCIE_CARD_GROUP_BDF_INFO_S g_pcie_disks_bdf_info;
extern PCIE_CARD_GROUP_INFO_S g_ocp_group;
extern PCIE_CARD_GROUP_SUB_INFO_S g_ocp_group_sub_info;
extern PCIE_CARD_GROUP_BDF_INFO_S g_ocp_bdf_info;
extern NODE_PCIE_GROUP_SUMMARY_INFO g_pcie_summary_info;
extern guint8 g_bdf_info_parsed_status;
extern gulong g_set_pcie_card_info_lock;
extern gulong g_set_pcie_disk_info_lock;
extern gulong g_set_node_pcie_summary_lock;
extern PCIE_CARD_STATUS_GROUP_S g_pcie_status_by_bios_group;
extern GAsyncQueue *g_pcie_dec_bandwidth_msg_queue;
extern guint32 g_pcie_alarm_state;

gboolean filter_msg_by_sys_channel(gconstpointer msg_data);

gint32 ocp_card_get_connector_slot_id(guint8 *slot_id, guint32 *connector_id);
gint32 ocp_card_ipmi_set_vid_did_subvid_subdid_info(gconstpointer msg_data, gpointer user_data);
void ocp_card_ipmi_get_pcie_vid_did_subvid_subdid(OCP_CARD_IPMI_INFO_S *ocp_card_data, const guint8 *req_data_buf);
void ocp_card_xml_unload_for_bios(OCP_CARD_IPMI_INFO_S *ocp_card_data);
void ocp_card_xml_load_for_bios(OCP_CARD_IPMI_INFO_S *ocp_card_data);
void ocp_card_info_per_save(OCP_CARD_IPMI_INFO_S *ocp_card_data);
void update_rootbdf_info(guint8 type, PORT_GROUP_BDF_INFO_S bdf_info);
extern PORT_GROUP_BDF_INFO_S g_pcie_port_rootbdf_info;
extern PORT_GROUP_BDF_INFO_S g_ocp_port_rootbdf_info;
extern PORT_GROUP_BDF_INFO_S g_pcie_raid_rootbdf_info;
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
