

#ifndef __SDI_CARD_H__
#define __SDI_CARD_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 



#define SD100_CARD_DUMPINFO_MAX_LEN 255
#define SD100_CARD_MAX_NUM 8
#define IPMI_RESPONSE_DATA_LEN 20
#define SDICpu "SD100Cpu"
#define M3_INIT_TASK "M3_INIT_TASK"
#define GET_BMC_IP_TASK "GET_BMC_IP_TASK"
#define UPDATE_SDI_SLOT_TASK "UPDATE_SDI_SLOT_TASK"
#define OPEN_DEBUG 0x55
#define CLOSE_DEBUG 0
#define M3_INNER_ADDR_OFFSET 0x30
#define M3_INNER_ADDR_DATA_SIZE 9
#define M3_OPERATER_OFFSET 0x80
#define M3_IS_WORKING_OFFSET 0x70
#define M3_OPERATER_DATA_SIZE 1
#define PORT_OFFSET 0x71
#define PORT_DATA_SIZE 6
#define RST_REASON_OFFSET 0x09
#define RST_REASON_DATA_SIZE 4
#define MAX_STRING_LENGTH 32
#define BMC_SET_FSA_IP_OFFSET 0x80
#define BMC_SET_FSA_IP_DONE_OFFSET 0x8B
#define BMC_SET_SDI_SLOT_OFFSET 0xB9 
#define BMC_RESET_SDI_OFFSET 0x06
#define BMC_RESET_SDI_VALUE 0x55
#define BMC_RESET_SDI_IMU_OFFSET 0x0D
#define BMC_RESET_SDI_IMU_VALUE 0x55
#define HDD_ORDER 0
#define PXE_ORDER 1
#define NONE_ORDER 255
#define NET_CARD_PXE_ORDER 3
#define SINGLE_HOST_PXE_ORDER 5
#define EXTERND_PXE_ORDER 7

#define BMC_IP_ADDR_OFFSET0 0xA1
#define BMC_IP_ADDR_OFFSET1 0xA2
#define BMC_IP_ADDR_OFFSET2 0xA3
#define BMC_IP_ADDR_OFFSET3 0xA4
#define SET_BOOT_ORDER 0x01
#define GET_IP_ADDR 0x01
#define IPMI_IP_DATA_LENGTH 10
#define IPMI_RESET_LINKAGE_DATA_LENGTH 1
#define CPLD_BIOS_PRINT_FLAG_OFFSET 0xB8
#define BIOS_LOAD_FAIL 0
#define OS_LOAD_SUCCESS 3
#define SDI_BOARD_5E_OS_LOAD_SUCCESS 7

#define IPV4_ADDR_LEN 16

#define CPU_TEMP_OFFSET 0x76
#define CPU_TEMP_CPLD_OFFSET 0x66
#define READ_ENABLE 0xaa
#define SDI_HANDLE_NOT_EXIST 0
#define SDI_HANDLE_OUT_OF_RANGE 0xff
#define CPLD_SDI_FSA_STATUS 0x96
#define CPLD_SDI_ETH01_IP_ADDR 0x98
#define CPLD_SDI_ETH01_VLAN 0x9C
#define CPLD_SDI_ETH01_STATUS 0x97
#define M3_ACTIVE 0x00

#define SDI_CARD_BOARDID_V5 0xd1
#define SDI_CARD_BOARDID_V3 0xd0

#define SDI_CARD_BOARDID_FD 0xfd


#define SDI_CARD_TASK_DELAY 50
#define SDI_EXTEND_CARD_TYPE_1822 0x00           // 表示1822网卡
#define SDI_EXTEND_CARD_TYPE_CX5_IN_SOURCE 0x01  // 自研CX5网卡
#define SDI_EXTEND_CARD_TYPE_CX5_OUT_SOURCE 0x02 // 外购CX5网卡

#define SDI_CARD_BOARDID_5E 0x5e
#define SDI_CARD_BOARDID_AC 0xac
#define SDI_CARD_36_BOARDID_AB 0xab
#define SDI_CARD_50_BOARDID_A5 0xa5
#define SDI_CARD_51_BOARDID_A9 0xa9
#define DPU_CARD_12_BOARDID_ED 0xed         // DPU1.2智能网卡（2*25GE）boradID  0xed
#define BMC_IPV6_ADDR_OFFSET0 0xD0
#define SDI_MAX_BOOT_ORDER_NUM 7
#define SDI_MAX_BOOT_ORDER_STR 32
#define SDI_MAX_BOOT_ORDER_LEN 15
#define PCIE_BOARD_MAX_LEN 64
#define BOOT_ORDER_ONCE 0
#define BOOT_ORDER_PERMANENT 1

#define IPV4_ADDR_SIZE 4
#define IPV6_ADDR_SIZE 16
#define CPLD_OBJ_MAX_NAME_LEN 64

#define PROPERTY_CPLD_ACCESSOR_CHIP "Chip"
#define PROPERTY_CPLD_ACCESSOR_OFFSET "Offset"
#define PROPERTY_CPLD_ACCESSOR_SIZE "Size"

#define CPLD_SDI_ADDR 0x36
#define CPLD_SDI_EXT_ADDR 0x38

#define SDI_CARD_NMI_PARAM 0xF0     // 使用Request parameter参数0xF0设置触发NMI
#define SDI_CARD_PARAM_NONE 0x00    // 不使用Request parameter参数

#define SDI_DEBUG_PARAM_NUM_2 2
#define SDI_DEBUG_PARAM_NUM_3 3
#define SDI_DEBUG_PARAM_NUM_4 4

#define SDI_CPLD_PRINT_NUMBER 16
#define SDI_CPLD_MAX_LINE 16
#define SDI_CPLD_DUMPINFO_MAX_LEN 2000
#define SDI_CPLD_RETRY_TIME 3
#define SDI_CPLD_VERSION_20 0x20
#define SDI_CPLD_MAX_NUMBER 256


#define SDI_NETCARD_PRESENCE_ABSENCE 1
#define SDI_NETCARD_ABSENCE_PRESENCE 2
#define SDI_NETCARD_PRESENCE_PRESENCE 3



void sdi_card_cpuTemp_start(OBJ_HANDLE obj_hnd);
void sdi_card_get_bmc_ip(OBJ_HANDLE obj_hnd);
gint32 get_sdi_card_handle_by_logic_num(guint8 logicalNum, OBJ_HANDLE *obj_handle);
gint32 sdi_card_add_object_callback(OBJ_HANDLE object_handle);
gint32 sdi_card_del_object_callback(OBJ_HANDLE object_handle);
gint32 ipmi_cmd_set_sdi_boot_order(const void *msg_data, gpointer user_data);
gint32 sdi_card_dump_info(const gchar *path);
gint32 sdi_bios_print(GSList *input_list);
gint32 ipmi_cmd_set_sdi_ip(const void *msg_data, gpointer user_data);
gint32 get_os_set_ip_status(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 get_fsa_ip_status(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 sdi_set_pcie_test_reg(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 ipmi_cmd_reset_sdi(const void *msg_data, gpointer user_data);
gint32 ipmi_cmd_reset_sdi_imu(const void *msg_data, gpointer user_data);
gint32 ipmi_cmd_set_sdicard_nmi(const void *msg_data, gpointer user_data);
gint32 method_sdi_card_read_reg(GSList *input_list);
gint32 method_sdi_card_write_reg(GSList *input_list);
gint32 sdi_card_dump_cpld_reg(const gchar *path);
gint32 sdi_card_set_eid(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 method_set_sdicard_nmi(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 method_sdi_card_set_boot_order(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 ipmi_cmd_set_sdicard_reset_linkage(gconstpointer msg, gpointer user_data);
gint32 is_valid_sdi_or_dpu_boardid(OBJ_HANDLE object_handle);


extern gint32 sdi_card_start(void);




#pragma pack(1)

typedef struct tag_sdi_ipmi_resp_s {
    guint32 comp_code : 8;     // 完成码
    guint32 manufacturer : 24; // 产商信息
} SDI_IPMI_RESP_S;

typedef struct tag_sdi_set_fsa_ip_req_s {
    guint32 manufacturer : 24; // 产商信息
    guint32 sub_cmd : 8;       // 子命令
    guint8 device_type;
    guint8 device_info;
    guint8 reserved;
    guint16 logical_num; // PCEI_SLOT槽位号
    guint8 flag;
    guint16 read_offset;
    guint8 length;
    guint8 write_data[IPMI_IP_DATA_LENGTH];
} SDI_SET_FSA_IP_REQ_S;

typedef struct tag_sdi_set_boot_order_req_s {
    guint32 manufacturer : 24; // 产商信息
    guint32 sub_cmd : 8;       // 子命令
    guint8 device_type;
    guint8 device_info;
    guint8 reserved;
    guint16 logical_num; // PCEI_SLOT槽位号
    guint8 flag;
    guint16 read_offset;
    guint8 length;
    guint8 boot_order;
    guint8 vaild_type;
} SDI_SET_BOOT_ORDER_REQ_S;

typedef struct tag_sdi_reset_req_s {
    guint32 manufacturer : 24; // 产商信息
    guint32 sub_cmd : 8;       // 子命令
    guint8 device_type;
    guint8 device_info;
    guint8 reserved;
    guint16 logical_num;
    guint8 flag;
    guint16 read_offset;
    guint8 length;
    guint8 reset_flag;
} SDI_RESET_REQ_S;

typedef struct tag_sdi_nmi_req_s {
    guint32 manufacturer : 24; // 产商信息
    guint32 sub_cmd : 8;       // 子命令
    guint8 device_type;
    guint8 device_info;
    guint8 reserved;
    guint16 logical_num;
    guint8 flag;
    guint16 read_offset;
    guint8 length;
} SDI_NMI_REQ_S;

typedef struct tag_sdi_rest_linkage_req_s {
    guint32 manufacturer : 24;  // 产商信息
    guint32 sub_cmd : 8;        // 子命令
    guint8 device_type;
    guint8 device_info;
    guint8 reserved;
    guint16 logical_num;        // PCEI_SLOT槽位号
    guint8 end_flag;
    guint16 read_offset;
    guint8 length;
    guint8 data;
} SDI_RESET_LINKAGE_REQ_S;

gint32 sdi_card_set_boot_order(const void *msg_data, OBJ_HANDLE obj_hnd, guint32 boardid,
    const SDI_SET_BOOT_ORDER_REQ_S *boot_order_req);
gint32 set_sdi_info_to_mcu(OBJ_HANDLE obj_handle, guint16 opcode, guint8 *in_data, guint8 in_size);
gint32 set_sdi_parameters_to_mcu(OBJ_HANDLE obj_handle, guint8 req_param, guint16 opcode, guint8 *in_data,
    guint8 in_size);
gint32 sdi_card_set_boot_order_mcu(const void *msg_data, OBJ_HANDLE obj_hnd, guint32 boardid,
    const SDI_SET_BOOT_ORDER_REQ_S *boot_order_req);

#pragma pack()

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
