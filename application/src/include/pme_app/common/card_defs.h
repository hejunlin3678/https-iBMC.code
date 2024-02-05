/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: 板卡管理相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义。
 */

#ifndef __CARD_DEFINE_H__
#define __CARD_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* 单板类型 */
#define BOARD_SWITCH 1
#define BOARD_BLADE 2
#define BOARD_OTHER 3
#define BOARD_MM 4
#define BOARD_RACK 5
#define BOARD_HMM 6
#define BOARD_RM 7


/* 卡类型 */
#define CARD_TYPE_MEZZ 11
#define CARD_TYPE_PWRBKP 25
#define CARD_TYPE_SW 31
#define CARD_TYPE_PIC 48
#define CARD_TYPE_MESH 85

#define UNKOWN_CARD_TYPE 0xff /* 未知类型的卡，web和命令行都识别不到这个卡 */

#define MAX_VER_INFO_LEN 24 /* 板卡的版本信息最大长度 */
#define PRESENT_STATE 0x01  /* 板卡在位 */

#define PCIECARD_FUNCTION_TYPE_MAX_LEN 30

/* PCIeCard中FunctionClass取值定义 */
#define PCIECARD_FUNCTION_UNKNOWN 0
#define PCIECARD_FUNCTION_RAID 1
#define PCIECARD_FUNCTION_NETCARD 2
#define PCIECARD_FUNCTION_GPU 3
#define PCIECARD_FUNCTION_STORAGE 4
#define PCIECARD_FUNCTION_SDI 5
#define PCIECARD_FUNCTION_ACCELERATE 6
#define PCIECARD_FUNCTION_PCIE_RISER 7
#define PCIECARD_FUNCTION_FPGA 8
#define PCIECARD_FUNCTION_NPU 9
#define PCIECARD_FUNCTION_STORAGE_DPU 10

#define LIQUID_DETECT_ALARM_ASSERT 1
#define LIQUID_DETECT_ALARM_DEASSERT 0
#define BYTE_BITS_NUM 8

/* PCIe资源ID归属定义 */
// PCIe资源归属于其它PCIe设备时, 资源ID的基数, 实际的资源ID=基数 + PCIe设备FRU ID
#define RESOURCE_ID_PCIE_DEVICE_BASE 0x80
// PCIe资源归属于其它PCIe设备时, 资源ID的最大值, 实际的资源ID=基数 + PCIe设备FRU ID
#define RESOURCE_ID_PCIE_DEVICE_MAX 0xC0
#define RESOURCE_ID_CPU1_TO_CPU4 0xFC     // PCIe资源归属于所有CPU
#define RESOURCE_ID_CPU1_AND_CPU2 0xFD    // PCIe资源归属于CPU1和CPU2
#define RESOURCE_ID_PCH 0xFE              // PCIe资源归属于PCH
#define RESOURCE_ID_PCIE_SWTICH 0xFF      // PCIe资源归属于PCIe Switch, 可动态切换, 不固定

/* 连接器类型名称 */
#define CONNECTOR_TYPE_PCIE             "PCIe"
#define CONNECTOR_TYPE_PCIEPLUS         "PCIePlus"
#define CONNECTOR_TYPE_PCIECONNECTOR    "PCIeConnector"
#define VIRTUAL_CONNECTOR               "VirtualConnector"
#define CONNECTOR_TYPE_DISKCONNECTOR    "DiskConnector"
#define CONNECTOR_TYPE_NVMEVPDCONNECTOR "NVMeVPDConnector"
#define CONNECTOR_TYPE_OCPCARDCONNECTOR "OCPConnector"
#define CONNECTOR_TYPE_PANGEA_CARD      "PangeaCard"
#define CONNECTOR_TYPE_CHIP             "Chip"
#define MAX_CONNECTOR_TYPE_LEN  128
#define CONNECTOR_TYPE_PCIE_SDI         "PCIeSDI"

#define CARD_TYPE_CRC_VALIDATE_FAILED  (-2)
#define GET_CARD_TYPE_FAILED           (-3)

/* 接口卡MCU升级文件长度固定位96k */
#define IOCARD_MCU_IMAGE_LEN (96 * 1024)
#define IOCARD_IIC_READ_BIT       1
#define IOCARD_IIC_PEC_LEN        1
#define IOCARD_IIC_READ_HEADLEN   3
#define IOCARD_IIC_WRITE_HEADLEN  2

#define MAX_CARD_NUM              2

#define card_err_log_limit(slot, ret, format, arg...)                \
    do {                                                             \
        static gint32 pre_ret[MAX_CARD_NUM] = {0};                   \
        if ((slot - 1) >= 0 && (slot - 1 ) < MAX_CARD_NUM) {         \
            if (ret != pre_ret[slot - 1] && ret != RET_OK) {         \
                debug_log(DLOG_ERROR, format, ##arg);                \
            }                                                        \
            pre_ret[slot - 1] = ret;                                 \
        }                                                            \
    } while (0)

#pragma pack(1)

/* SDI扩展网卡的端口PF属性 */
typedef struct tag_pfport_info {
    guint16 pf;
    guint16 physical_port;
    gchar mac_addr[MACADDRESS_LEN + 1];
} PFPORT_INFO_S;

#pragma pack()

typedef enum tag_mcu_iic_cmd_e {
    MCU_IIC_SYS_SET_LED         = 0x01,
    MCU_IIC_SYS_SYSC_ONLINE     = 0x02,
    MCU_IIC_SYS_READ_PG         = 0x03,
    MCU_IIC_READ_PCB_VER        = 0x04,
    MCU_IIC_SYS_SET_CHIP_RESET  = 0x05,
    MCU_IIC_SYS_SET_QSPF_POWER  = 0x06,
    MCU_IIC_SYS_SET_EEPROM_WP   = 0x07,
    MCU_IIC_SYS_SYNC_TIME       = 0x08,
    MCU_IIC_READ_TEMPERATURE    = 0x09,
    MCU_IIC_SIMU_TEMP           = 0x0A,
    MCU_IIC_SYS_SYNC_STATUS     = 0x0B,
    MCU_IIC_READ_SYS_SYNC_INFO  = 0x0C,
    MCU_IIC_READ_CML_STATUS     = 0x0F,
    MCU_IIC_SYS_SYNC_CARDTYPE   = 0x10,
    MCU_IIC_SYS_SYNC_LINKTYPE   = 0x11,
    MCU_IIC_SYS_SYNC_QOSINFO    = 0x12,
    MCU_IIC_EQUIP_TEST_LED      = 0x20,
    MCU_IIC_EQUIP_TEST_MODE_SW  = 0x21,
    MCU_IIC_LOG_READ            = 0x22,
    MCU_IIC_LOG_READ_ALL        = 0x23,
    MCU_IIC_FW_UPDATE_PREPARE   = 0x30,
    MCU_IIC_FW_UPDATE_FILE_TRANS = 0x31,
    MCU_IIC_FW_UPDATE_START     = 0x32,
    MCU_IIC_VER_OUT             = 0x33,
    MCU_IIC_EKEY_ACCESS         = 0x40,
    MCU_IIC_EKEY_ACCESS_SHORT   = 0x41,
    MCU_IIC_CORE_CHIP_READY     = 0x42,
    MCU_CONTRL_I2C_CHIP_POWER   = 0x44,
    MCU_IIC_RESET_MCU           = 0x45,
    MCU_IIC_PROCDUCT_SELFDESC   = 0x46,
    MCU_IIC_HW_SELFDES          = 0x47,
    MCU_IIC_1822_SEC_BOOT       = 0x48, /* BMC通知1822安全启动命令字 */
    MCU_IIC_HW_OPERATE          = 0x49, /* 读取GPIO 命令字 */
    MCU_IIC_READ_MCU_TYPE       = 0x4a, /* 获取mcu芯片的类型 */
    MCU_IIC_READ_CARDTYPE       = 0x4e, /* 读取卡类型第二套接口，读0x10为全ff时用此命令字 */
    MCU_IIC_GET_LM75_STATUS     = 0x50, /* 查询LM75状态 */
} MCU_IIC_CMD_E;
#pragma pack(1)
/* cpld-i2c先写后读的方式与MCU通信 */
typedef struct tagIOCARD_WRINPUT_S {
    guint8 *write_data;
    guint8 *read_data;
    gint32 cmd;
    guint32 chip_addr;
    guint32 write_len;
    guint32 read_len;
} PANGEA_CARD_WRITE_READ_S;

typedef struct tagCARD_IIC_READ_S {
    guint8 write_addr;
    guint8 cmd;
    guint8 read_addr;
    guint8 *read_data;
    guint32 data_len;
} PANGEA_CARD_IIC_READ_S;
#pragma pack()
/* retimer芯片类型 */
enum CDR_CHIP_TYPE {
    CDR_CHIP_TYPE_5902L = 0,
    CDR_CHIP_TYPE_5902H = 1,
    CDR_CHIP_TYPE_DS280 = 3,
};

/* Retimer IsReady 值 */
enum retimer_ready_status {
    RETIMER_NOT_READY = 0,
    RETIMER_HAS_READY = 1
};

typedef struct function_type_provider {
    guint8 func_type;
    const gchar function_type[PCIECARD_FUNCTION_TYPE_MAX_LEN + 1];
} FUNCTION_TYPE_PROVIDER;

gint32 share_card_send_data_to_mcu(OBJ_HANDLE chip_handle, MCU_IIC_CMD_E cmd, gpointer data, guint32 data_len);
gint32 share_card_read_data_from_mcu(guint8 slot, OBJ_HANDLE chip_handle, MCU_IIC_CMD_E cmd, guint8 *rd_buf,
    guint32 rd_len);
gint32 share_card_check_read_data(guint8 *data, guint32 len);
gint32 retimer_write_i2c_data(RETIMER_DRV_INTF_S *write_data);
gint32 share_card_check_read_data(guint8 *data, guint32 len);
gint32 retimer_write_i2c_data(RETIMER_DRV_INTF_S *write_data);
gint32 pm_get_virtual_local_bus(OBJ_HANDLE card_obj, const gchar* ref_obj_prop,
                                const gchar* class_name, OBJ_HANDLE* local_bus);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CARD_DEFINE_H__ */
