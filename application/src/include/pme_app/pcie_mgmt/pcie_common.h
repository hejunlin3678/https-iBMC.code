/******************************************************************************

                  版权所有 (C), 2012-2013, 华为技术有限公司

 ******************************************************************************
  文 件 名   :
  版 本 号   : 初稿
  部    门   :
  作    者   :
  生成日期   :
  最近修改   :
  功能描述   :
  函数列表   :
******************************************************************************/
#ifndef __PCIE_MGMT_H__
#define __PCIE_MGMT_H__

#include <glib.h>
#include "pme/common/mscm_macro.h"
#include "pme/pme.h"
#ifdef __cplusplus
#if __cplusplus
    extern "C"{
#endif
#endif /* __cplusplus */

//pcie_mgmt lib库返回的错误码定义
#define PM_ERROR_CODE_BASE  0x3000
#define PM_ERROR_CODE_NIC                         (PM_ERROR_CODE_BASE + 0x100)
#define PM_ERROR_CODE_FPGA                        (PM_ERROR_CODE_BASE + 0x200)


#define PM_ERROR_CODE_I2C_READ_FAILED             (PM_ERROR_CODE_BASE + 0x01)
#define PM_ERROR_CODE_I2C_WRITE_FAILED            (PM_ERROR_CODE_BASE + 0x02)
#define PM_ERROR_CODE_REQUEST_LEN_ERROR           (PM_ERROR_CODE_BASE + 0x03)
#define PM_ERROR_CODE_IDENTIFY_CODE_ERROR         (PM_ERROR_CODE_BASE + 0x04)
#define PM_ERROR_CODE_CAP_UNSUPPORTED             (PM_ERROR_CODE_BASE + 0x05)


#define PM_ERROR_CODE_NIC_CHIP_TEMP_UNSUPPORTED   (PM_ERROR_CODE_NIC + 0x02)
#define PM_ERROR_CODE_NIC_OP_TEMP_UNSUPPORTED     (PM_ERROR_CODE_NIC + 0x03)
#define PM_ERROR_CODE_NIC_HEALTH_UNSUPPORTED      (PM_ERROR_CODE_NIC + 0x04)
#define PM_ERROR_CODE_NIC_FAULT_CODE_UNSUPPORTED  (PM_ERROR_CODE_NIC + 0x05)
#define PM_ERROR_CODE_NIC_FIRMWARE_UNSUPPORTED    (PM_ERROR_CODE_NIC + 0x06)
#define PM_ERROR_CODE_NIC_LOG_UNSUPPORTED         (PM_ERROR_CODE_NIC + 0x07)


#define KEY_CHIP_RESET_TIME   2
#define KEY_HEALTH_LEN      1
#define KEY_PCB_ID_LEN      1
#define KEY_SSL_STATUS_LEN  1
#define KEY_TEMP_LEN     2
#define KEY_VID_LEN     2
#define KEY_DID_LEN     2
#define KEY_SUB_VID_LEN     2
#define KEY_SUB_DID_LEN     2
#define KEY_BOARD_ID_LEN     2
#define KEY_BOM_ID_LEN     1
#define KEY_FIRM_VER_LEN 3
#define KEY_SHELL_ID_LEN    4
#define KEY_SN_LEN            32
#define STR_FAULT_CODE_LEN  1024
#define STR_FIRMWARE_LEN    20
#define KEY_LOG_STATE_LEN   1
#define KEY_BOOTORDER_LEN   4
#define KEY_OSSTATUS_LEN    2
#define KEY_SDI_RESET_LINKAGE_LEN   1   // SDI卡联动复位策略响应数据长度
#define MAX_FAULT_CODE_NUM 1024

#define STR_PCB_VER_LEN      5
#define RESP_DATA_LENGTH_OPCODE     0xFFFFFFFF
#define RESP_DATA_LENGTH_HEALTH     1
#define RESP_DATA_LENGTH_ERR_CODES  0xFFFFFFFF
#define RESP_DATA_LENGTH_TEMP        2
#define RESP_DATA_LENGTH_POWER      2
#define RESP_DATA_LENGTH_VERSION    3
#define RESP_DATA_LENGTH_VID        2
#define RESP_DATA_LENGTH_DID        2
#define RESP_DATA_LENGTH_SN            32
#define RESP_DATA_LENGTH_VOLT        2
#define RESP_DATA_LENGTH_ERROR_LOG     0xFFFFFFFF
#define RESP_DATA_LENGTH_DYING_SAY  0xFFFFFFFF
#define RESP_DATA_LENGTH_RUNNING_LOG 0xFFFFFFFF
#define RESP_DATA_LENGTH_DIMM_PRESENCE 5
/* BEGIN: Added for AR.SR.SFEA02130917.013.017 FPGA卡光模块信息查询 by lwx459244, 2018/9/28 */
#define RESP_DATA_LENGTH_OPTICAL_VENDORNAME      32
#define RESP_DATA_LENGTH_OPTICAL_PARTNUMBER      32
#define RESP_DATA_LENGTH_OPTICAL_SN              32
#define RESP_DATA_LENGTH_OPTICAL_MANUFACTUREDATE 8
#define RESP_DATA_LENGTH_OPTICAL_TYPE_INFO       29
#define RESP_DATA_LENGTH_OPTICAL_TEMP_VOLT       4

#define OPTICAL_TYPE_QSFP          0x11
#define OPTICAL_MEDIUM_MODE_FOR_MM 0
#define OPTICAL_MEDIUM_MODE_FOR_SM 1
#define MAX_OPTICAL_TYPE_LEN       20
#define MAX_OPTICAL_WAVELENGTH_LEN 10
#define MAX_OPTICAL_MEDIUMMODE_LEN 5
/* END:   Added for AR.SR.SFEA02130917.013.017 FPGA卡光模块信息查询 by lwx459244, 2018/9/28 */
#define OPTICAL_MODULE_TEMP_PAD                   0x7ffd//光模块温度的填充数据
#define OPTICAL_MODULE_TEMP_ABSENT                0x7ffe//不在位
#define OPTICAL_MODULE_TEMP_READ_FAIL             0x7fff//温度读取失败

#define RUNNING_LOG_FIRST_DELAY_FOR_RW          3000//巡检日志第一帧读写延时3s
#define COMMON_DELAY_TIME_FOR_RW                 10//读写i2c命令之间公共延时
#define LOG_DELAY_TIME_FOR_RW                     1//读取日志的读写i2c命令之间所需延时

#define FAULT_CODE_LEN                  256//故障码总长度

#define DEFAULT_CHIP_RESET_TIME                120//芯片复位默认时间

/* BEGIN: DTS2018103009508 . Modified by f00381872, 2018/11/2 */
#define LOG_DUMP_TRIGGER_RESET_OS               2//2为OS重启触发日志收集
#define LOG_DUMP_TRIGGER_RESET_BMC              1//1为BMC重启触发日志收集
/* END: DTS2018103009508 . Modified by f00381872, 2018/11/2 */
#define LOG_DUMP_TRIGGER_UNRESET                0//0为不触发日志收集

#define PCIE_INIT_WITHOUT_LOG_DUMP              0xff // MCU固件升级不触发日志收集

#define TASK_ERROR_LOG_REGEX   "error_log_[0-9]{14}\\.bin"//错误日志文件正则匹配表达式
#define TASK_LAST_WORD_REGEX   "last_word_[0-9]{14}\\.bin"//临终遗言文件正则匹配表达式
#define TASK_RUNNING_LOG_REGEX   "running_log_[0-9]{14}\\.bin"//巡检日志文件正则匹配表达式
#define PCIE_CARD_LOG_BASE_DIR        "/data/var/log/pciecard/"//网卡存放日志目录

#define TASK_OPERATE_LOG_REGEX   "operate_log_[0-9]{14}\\.bin"//操作日志文件正则匹配表达式
#define TASK_MAINTAINCE_LOG_REGEX   "maintaince_log_[0-9]{14}\\.bin"//维护日志文件正则匹配表达式

typedef struct Receive_Data_Len
{
    guint16 opcode;
    guint32 len;            //返回数据长度
}RECEIVE_DATA_LEN;

#define IDENTIFY_CODE        0x1eee//识别码
#define PCIE_MGNT_PROTOCOL_I2C       0x01
#define PCIE_MGNT_PROTOCOL_SMBUS  0x02
/* BEGIN: Modified by h00422363, 2018/3/26 9:50:42   问题单号:SR.SFEA02130917.004.001 FPGA卡基本信息查询 */

#define PCIE_MGNT_PROTOCOL_STD_SMBUS    0x03

/* 从SDI卡CPLD获取信息，用于SDI+CX5时获取CX5的MAC地址 */
#define PCIE_MGNT_PROTOCOL_SDI_CPLD    0x04
/* 从SDI卡CPLD获取信息，用于SDI+CX6时获取CX6的MAC地址 */
#define PCIE_MGNT_PROTOCOL_SDI_MULTI_HOST_CPLD 0x05

/* 从寒武纪S卡的带外寄存读取直接读取,实现直接读取4Bytes内容 */
#define PCIE_MGNT_PROTOCOL_SMBUS_REG 0x06

/* 无实质协议，只对象绑定初始化时私有数据 */
#define PCIE_MGNT_PROTOCOL_EMPTY 0xfe

/* END:   Modified by h00422363, 2018/3/26 9:50:52 */
#define PCIE_MGNT_INVALID_PROTOCOL     0xff
//网卡特殊的Data Register Key定义
#define REGISTER_KEY_OPTICAL_MODULE_TEMP     0x400//光模块温度
/** BEGIN: Added by qinjiaxiang, 2018/5/14  问题单号:SR.SFEA02130923.001.001 修改原因:自研1822网卡从带外获取MAC地址及LINK状态 */
#define REGISTER_KEY_NETCARD_LINK_STATUS     0x403//物理端口link状态
#define REGISTER_KEY_NETCARD_MAC_ADDRESS    0x404//物理端口永久MAC状态
//支持通过带外的方式获取网卡的mac地址和link状态，该值存放在linkstatussrc属性中
#define GET_LINK_STATUS_FROM_OUT_BAND       2
/* BEGIN: Added by h00371221, 2018/03/19   SR.SFEA02119719.013*/
//FPGA卡特殊的Data Register Key定义
#define REGISTER_KEY_DDR_TEMP     0x500//内存温度
#define REGISTER_KEY_SHELL_ID     0x501//shellid
/* END  : Added by h00371221, 2018/03/19   SR.SFEA02119719.013*/
/* BEGIN: Added for AR.SR.SFEA02130917.013.001 高端卡DIMM信息查询 by lwx459244, 2018/9/27 */
#define DIMM_NAME_LEN          16
#define DIMM_SN_LEN            32
#define DIMM_PN_LEN            32
#define DIMM_MANUFACTURER_LEN  32
#define DIMM_PART_CODING_LEN   32
#define DIMM_TECHNOLOGY_LEN    64
#define DIMM_CAP_LEN           9
#define DIMM_PREENCE           1
#define NPU_PRESENCE           1
#define NPU_MEMORY_SUPPORT    1
#define DIMM_TEMP_READ_INVALID 0x7ffd
#define DIMM_TEMP_READ_FAIL 0x7fff
/* END:   Added for AR.SR.SFEA02130917.013.001 高端卡DIMM信息查询 by lwx459244, 2018/9/27 */
/* NPU信息查询 */
#define NPU_SN_LEN            32
#define NPU_FIRE_LEN          20
#define NPU_BUF_LEN         32
#define NPU_TEMP_READ_INVALID 0x7ffd
#define NPU_TEMP_READ_FAIL 0x7fff
/* BEGIN: 查询内存容量opcode命令 */
#define REGISTER_KEY_PCIE_MEMORY_CMD  0
#define REGISTER_KEY_PCIE_MEMORY_INPUT  0x21
/* END 命令字 0x605 1 0 33 */
#define REGISTER_KEY_PCIE_RATED_MEMORY_INPUT  0x31
#define REGISTER_KEY_PCIE_RATED_POWER_INPUT  0x01
#pragma pack(1)
//i2c 通道映射关系
typedef struct data_region
{
    guint16 key;
    guint16 offset;            //该偏移为整个data region 相对0地址的偏移
    guint16 len;            //data_region数据内容，不包含data region结构的前两个字节
}DATA_REGION;

typedef struct data_num_region
{
    guint8 num;
    DATA_REGION *data_region;
    guint8 fault_retry_time[255];
} DATA_NUM_REGION;

typedef struct data_table
{
    guint16 opcode_num;
    guint16* opcode;
} DATA_TABLE;

typedef enum data_register_key
{
    REGISTER_KEY_CAPABILITY = 0,        //仅SMBus协议支持 长度不定
    REGISTER_KEY_HEALTH,                //健康
    REGISTER_KEY_ERROR_CODE,            //故障码  长度不定
    REGISTER_KEY_CHIP_TEMP,                //芯片温度
    REGISTER_KEY_POWER,                 //设备功效
    REGISTER_KEY_FIRMWARE,                //固件版本
    REGISTER_KEY_VID,
    REGISTER_KEY_DID,
    REGISTER_KEY_SN,                    //电子标签号
    REGISTER_KEY_SUB_VID,
    REGISTER_KEY_SUB_DID = 10,
    REGISTER_KEY_VOLTAGE,
    REGISTER_KEY_ERROR_LOG,                //故障日志: 不定长
    REGISTER_KEY_LAST_WORD,                //临终遗言: 不定长
    REGISTER_KEY_RUNNING_LOG,            //运行数据: 不定长
    REGISTER_KEY_BOARD_ID,
    REGISTER_KEY_PCB_ID,
    REGISTER_KEY_CHIP_RESET_TIME,
    REGISTER_KEY_CHIP_DRIVE_NAME,
    REGISTER_KEY_CHIP_DRIVE_VERSION,
    /* BEGIN: Modified by h00422363, 2018/4/12 9:44:59   问题单号:SR.SFEA02130917.004.001 FPGA卡基本信息查询 */
    REGISTER_KEY_SET_ELABLE = 20,    //设置电子标签
    REGISTER_KEY_GET_ELABLE,    //查询电子标签
    REGISTER_KEY_DFT_TEST,    //装备测试命令
    REGISTER_KEY_DFT_RESULT,    //装备测试结果查询
    REGISTER_KEY_UPGRADE_FILE_TRANS,    //FW升级文件传输
    REGISTER_KEY_UPGRADE_CONTROL_CMD,   //FW升级控制命令
    REGISTER_KEY_UPGRADE_STATE,     //升级状态查询
    REGISTER_KEY_RESERVED,
    REGISTER_KEY_DEVICE_VOLTAGE,    //查询所有的电压值
    REGISTER_KEY_DEVICE_TEMP,           //查询所有的温度值
    REGISTER_KEY_FAN_INFO = 30,              //查询所有的风扇转速
    REGISTER_KEY_ECC_STATISTICS,    //查询ECC统计信息
    REGISTER_KEY_GET_SYSTEM_TIME,       //查询系统时间
    REGISTER_KEY_SET_SYSTEM_TIME,       //设置系统时间
    REGISTER_KEY_SET_OLD_FLAG,    //设置老化标志和时间
    REGISTER_KEY_GET_OLD_FLAG,   //查询老化标志和时间
    REGISTER_KEY_GET_OLD_RESULT,  //查询老化测试结果
    REGISTER_KEY_SET_FAN_LEVEL,    //设置风扇转速
    REGISTER_KEY_OPERATE_LOG,           //操作日志
    REGISTER_KEY_MAINTAINCE_LOG,   //维护日志
    REGISTER_KEY_BOM_ID = 0x28,    // 查询bom id信息
    REGISTER_KEY_SET_IP = 0x1010,     // 设置IP地址
    REGISTER_KEY_GET_IP = 0x1011,     // 查询IP地址
    REGISTER_KEY_RESET_MCU = 0x1018,  // 设置系统复位
    REGISTER_KEY_SET_CARD_SLOT = 0x100E,    // 上报卡槽位号
    REGISTER_KEY_GET_CARD_SLOT = 0x100F,    // 查询卡槽位号
    REGISTER_KEY_SET_BOOT_ORDER = 0x1000,    // 设置系统启动选项
    REGISTER_KEY_GET_BOOT_ORDER = 0x1001,    // 查询系统启动选项
    REGISTER_KEY_GET_DEVICE_STATUS = 0x1005,    // 获取设备运行状态
    REGISTER_KEY_SET_RTC_TIME = 0X1017,       // 设置系统时间
    REGISTER_KEY_GET_NETCARD_ELABLE = 0x1035,    // 查询网络卡电子标签
    REGISTER_KEY_MCU_CHIP_TYPE = 0x0090,
    REGISTER_KEY_MCU_SDI_CHIP_TYPE = 0x1021,
    /* END:   Modified by h00422363, 2018/4/12 9:46:38 */
    /* BEGIN: Added for AR.SR.SFEA02130917.013.017 FPGA卡光模块信息查询 by lwx459244, 2018/9/28 */
    REGISTER_KEY_CHIP_RESET = 0x601,
    REGISTER_KEY_GET_CHIP_RESET_RESULT,
    REGISTER_KEY_PCIE_MEMORY = 0x605,            // 查询内存容量
    REGISTER_KEY_PCIE_NPU_FIRMWARE = 0x0607, // NPU固件版本
    REGISTER_KEY_PCIE_RATED_POWER = 0x06A1,
    REGISTER_KEY_GET_BIOS_VERSION = 0x607,
    REGISTER_KEY_GET_MEM_CHIP_TEMP = 0x697,
    REGISTER_KEY_GET_MEM_INFO,
    REGISTER_KEY_GET_CPU_INFO = 0x69b,
    REGISTER_KEY_GET_SSL_STATUS = 0x69F,
    REGISTER_KEY_SEND_BOOT_CERT = 0x06A0,
    REGISTER_KEY_MFPGA_OPTICAL_PRESENT = 0x700,
    REGISTER_KEY_MFPGA_OPTICAL_VENDORNAME,
    REGISTER_KEY_MFPGA_OPTICAL_PARTNUMBER,
    REGISTER_KEY_MFPGA_OPTICAL_SN,
    REGISTER_KEY_MFPGA_OPTICAL_MANUFACTUREDATE,
    REGISTER_KEY_MFPGA_OPTICAL_TYPE_INFO,
    REGISTER_KEY_MFPGA_OPTICAL_TEMP_INFO,
    REGISTER_KEY_MFPGA_OPTICAL_POWER_INFO,
    /* END:   Added for AR.SR.SFEA02130917.013.017 FPGA卡光模块信息查询 by lwx459244, 2018/9/28 */
    /* BEGIN: Modified by xwx554433, 2019/4/3 17:37:18  支持FPGA卡远程Golden自愈 UADP171189 */
    REGISTER_KEY_FPGA_SELFHEALING,
    REGISTER_KEY_FPGA_QUERY_SELFHEALING_STATUS,
    /* END:  Modified by xwx554433, 2019/4/3 17:45:28  支持FPGA卡远程Golden自愈 UADP171189 */
    /* BEGIN: Added for AR.SR.SFEA02130917.013.001 高端卡DIMM信息查询 by lwx459244, 2018/9/27 */
    REGISTER_KEY_DIMM_PRESENT = 0x720, //内存条在位状态
    REGISTER_KEY_DIMM_INFO,     //内存条信息(厂商、SN、容量、部件编码等)
    REGISTER_KEY_DIMM_TEMP = 0x727,//内存条温度
    /* END:   Added for AR.SR.SFEA02130917.013.001 高端卡DIMM信息查询 by lwx459244, 2018/9/27 */
    REGISTER_KEY_PCIE_MCU_GPIO_RESET = 0x730,
    REGISTER_KEY_SDI_GET_MAC = 0x101B,   // MCU获取MAC地址
    REGISTER_KEY_GET_PRESENCE = 0x1020,  // MCU获取在位信息
    REGISTER_KEY_SET_RESET_LINKAGE_POLICY = 0x1023,    // 设置SDI卡复位联动策略
    REGISTER_KEY_GET_RESET_LINKAGE_POLICY = 0x1024,    // 查询SDI卡复位联动策略
    REGISTER_KEY_GET_LOG_STATE = 0x1031,  // MCU查询设备日志更新状态
    REGISTER_KEY_SET_NMI = 0x1042,  // 设置SDI卡NMI模拟中断
    REGISTER_KEY_INVALID = 0XFF            //无效值
} DATA_REGION_KEY;

typedef enum {
    MCU_UPGRADE_CMD_INITATE = 1,
    MCU_UPGRADE_CMD_STOP,
    MCU_UPGRADE_CMD_TAKEEFFECT,
    MCU_UPGRADE_CMD_SWITCH_IMAGE,
    MCU_UPGRADE_CMD_ROLLBACK,
    MCU_RESET
} MCU_UPGRADE_CMD;

typedef enum {
    RESET_MCU_IDLE,
    RESETING_MCU,
    RESET_MCU_FAILED,
    RESET_MCU_SUCCESSFUL
} RESET_MCU_STATUS;

typedef enum Pcie_Card_Type
{
    INVALID_CARD = 0,
    RAID_CARD,
    SSD_CARD,
    NIC_CARD,
    COMPRESSION_CARD,
    FPGA_ACCELERATE_CARD,
    DMINI_CARD,
    MFPGA_CARD,
    SDI_CARD,
    PCIE_CARD_TYPE_MAX
}PCIE_CARD_TYPE;
/* BEGIN: Added for AR.SR.SFEA02130917.013.017 FPGA卡光模块信息查询 by lwx459244, 2018/9/28 */
typedef struct _optical_current_power_threshold
{
    guint8 type;
    guint16 Wavelength;
    guint8 MediumMode;
    guint8 ChannelNum;
    guint16 RxPowerHighAlarm;
    guint16 RxPowerLowAlarm;
    guint16 RxPowerHighWarn;
    guint16 RxPowerLowWarn;
    guint16 TxBiasHighAlarm;
    guint16 TxBiasLowAlarm;
    guint16 TxBiasHighWarn;
    guint16 TxBiasLowWarn;
    guint16 TxPowerHighAlarm;
    guint16 TxPowerLowAlarm;
    guint16 TxPowerHighWarn;
    guint16 TxPowerLowWarn;
}OP_CURRENT_POWER_THER;
typedef struct _optical_temp_volt
{
    guint16 temp;
    guint16 volt;
}OP_TEMP_VOLT;
typedef struct _optical_temp_volt_double
{
    gdouble temp;
    gdouble volt;
}OP_TEMP_VOLT_DOUBLE;
#define MAX_CHANNEL_NUM  16
typedef struct _optical_power_current
{
    gdouble RxPower[MAX_CHANNEL_NUM];//接收功率
    gdouble TxPower[MAX_CHANNEL_NUM];//发送功率
    gdouble TxBiasCurrent[MAX_CHANNEL_NUM];//偏置电流
}OP_POWER_CURRENT;
typedef struct _optical_power_current_convert
{
    guint16 RxPower;//接收功率
    guint16 TxPower;//发送功率
    guint16 TxBiasCurrent;//偏置电流
}OP_POWER_CURRENT_CONVERT;
#define OP_TEMP_VOLT_CURRENT_POWER_INVALID 0x7f7f//光模块温度、电压、电流、功率非法值
#define OP_TEMP_VOLT_CURRENT_POWER_NA 0xfdfd//光模块温度、电压、电流、功率不在位值
/* END:   Added for AR.SR.SFEA02130917.013.017 FPGA卡光模块信息查询 by lwx459244, 2018/9/28 */
/* BEGIN: Modified by h00422363, 2018/5/15 15:50:29   问题单号:SR.SFEA02130917.004.001 FPGA卡基本信息查询 */
#define HEAD_PARA_MAX_LEN 64
//请求头部参数结构体定义
#define CARD0_PWR_ON_SENSOR_NAME  "Card0PowerOn"
#define CARD1_PWR_ON_SENSOR_NAME  "Card1PowerOn"
// 请求头部参数结构体定义
typedef struct protocol_head_para
{
    guint8 data_object_index;
    guint8 cmd_arg;
    guint16 opcode;
    guint32 input_data_len;
    guint8 input_data[HEAD_PARA_MAX_LEN];  //读请求时所带数据入参
}PROTOCOL_HEAD_PARA;

//电子标签信息存储结构体定义
typedef struct elabel_item_info
{
    guint8 item_id;
    guint16 len;
}ELABEL_ITEM_INFO;
extern ELABEL_ITEM_INFO elabel_datalen_table[];
extern ELABEL_ITEM_INFO elabel_modbus_datalen_table[];

enum
{
    ELABEL_CHASSIS_TYPE = 0x10,
    ELABEL_CHASSIS_PART_NUMBER,
    ELABEL_CHASSIS_SERIAL_NUMBER,
    ELABEL_BOARD_MFG_TIME = 0x20,
    ELABEL_BOARD_MANUFACTURE,
    ELABEL_BOARD_PRODUCT_NAME,
    ELABEL_BOARD_SERIAL_NUMBER,
    ELABEL_BOARD_PART_NUMBER,
    ELABEL_BOARD_FILE_ID,
    ELABEL_PRODUCT_MANUFACTURE_NAME = 0x30,
    ELABEL_PRODUCT_NAME,
    ELABEL_PRODUCT_PART_NUMBER,
    ELABEL_PRODUCT_VERSION,
    ELABEL_PRODUCT_SERIAL_NUMBER,
    ELABEL_PRODUCT_ASSET_TAG,
    ELABEL_PRODUCT_FILE_ID,
    ELABEL_EXTEND_FIELD = 0x50,
    ELABEL_SYSTEM_MANUFACTURE_NAME = 0x60,
    ELABEL_SYSTEM_PRODUCT_NAME,
    ELABEL_SYSTEM_VERSION,
    ELABEL_SYSTEM_SERIAL_NUMBER
};

enum
{
    MCU_WRITE_ELABEL = 0x00,       /* MCU写电子标签 */
    MCU_CLEAR_ELABEL,       /* MCU清除电子标签 */
    MCU_UPDATE_ELABEL,       /* MCU更新电子标签 */
};
/* END:   Modified by h00422363, 2018/5/8 14:9:56 */
/* BEGIN: Added for AR.SR.SFEA02130917.013.001 高端卡DIMM信息查询 by lwx459244, 2018/9/27 */
#define DIMM_FIXED_VALUE_PROPERTY "dimm_fixed_value_prop"
typedef struct _dimm_present_info
{
    guint8 dimm_num;
    guint32 dimm_present;
}DIMM_PRESENT_INFO;
typedef struct _fpga_dimm_info
{
    gchar dimm_name[DIMM_NAME_LEN];//内存条丝印名称
    guint32 dimm_cap;//容量
    guint32 dimm_basic_frequency;//主频
    guint8 dimm_type;//类型
    guint16 dimm_min_volt;//最小电压
    guint8 dimm_rank_num;//RANK个数
    gchar dimm_manufacturer[DIMM_MANUFACTURER_LEN];//厂商名称
    gchar dimm_sn[DIMM_SN_LEN];//SN
    gchar dimm_pn[DIMM_PN_LEN];//PN
    gchar dimm_part_coding[DIMM_PART_CODING_LEN];//部件编码
    gchar dimm_technology[DIMM_TECHNOLOGY_LEN];//技术
    gchar reserve[64];//预留
}FPGA_DIMM_INFO;

typedef struct _dimm_tmp_info
{
    guchar dimm_name[DIMM_NAME_LEN];
    guint16 dimm_tmp;
}DIMM_TMP_INFO;
/* END:   Added for AR.SR.SFEA02130917.013.001 高端卡DIMM信息查询 by lwx459244, 2018/9/27 */

/* PCIe设备带外管理的私有数据结构 */
typedef struct _chip_info
{
    /* BEGIN: Modiied by h00282621, 2019/2/2 */
    /*
     * Linux内核4.4 + glib-2.54版本以上的环境下，g_mutex_lock操作的GMutex变量地址
     * 必须是内存对齐的，       否则g_mutex_lock原子操作访问非对齐内存，触发SIGBUS/SIGSEGV后
     * 程序会退出，此结构体不被其他结构体包含，因此  将GMutex变量放在结构体起始位置
     */
    GMutex rw_mutex;
    GMutex key_mutex;
    /* END:   Added by h00282621, 2019/2/2 */

    gchar chip_name[MAX_NAME_SIZE];
    guint8 init_state;    // 初始化状态
    guint8 inf_protocol;  // 支持的协议: 1：i2c, 2: OEM Smbus, 3: Standard Smbus, 255: Unknown
    guint32 framelen;     // 单帧传输最大长度
    guint32 offset;       // 读取数据的偏移
    guint16 len;          // 读取的长度
    guint16 chip_reset_time; // 芯片复位需要的时长，单位秒

    guint32 temp_read_fail_timestamp;
    guint32 ddr_temp_read_fail_timestamp;
    guint32 power_read_fail_timestamp;
    guint32 optical_temp_read_fail_timestamp;
    guint32 optical_current_read_fail_timestamp;

    guint32 common_delay_time_for_rw;  // 读写i2c命令之间公共延时
    guint32 log_delay_time_for_rw;     // 读取日志的读写i2c命令之间所需延时

    guint32 fault_state_from_sms; // 系统软件上报的故障状态，按bit位定义，与带外（I2C/SMBUS）获取的故障类型定义一致
    guint8 pcieslot_fault_state;  // 标记卡的总体状态，0表示健康，1表示有故障
    guint8 last_fault_code[FAULT_CODE_LEN];  // 上一次获取的故障码
    guint32 last_fault_code_len;             // 上一次获取的故障码长度

    guint8 log_dump_trigger; // 日志触发收集标志

    TASKID collect_log_task_id;
    TASKID reset_mcu_task_id;
    guint8 reset_mcu_state;
    guint8 collect_log_from_mcu; // 从mcu收集日志标志
    union
    {
        //i2c
        DATA_NUM_REGION data_region_table;
        //SMBus
        DATA_TABLE receive_data_table;
    };
    guint8 card_type;
    guint8  pf_update_delay_cnt;  // 获取pf信息的计数，每5次真正查询一次；
    GSList* pf_info_list;         // 保存pf信息的链表
    guint32 ref_cnt; // PM_CHIP_INFO引用计数，所有实例读写前都需要先取得锁g_pm_ref_mutex
    guint32 prev_bitmap[MAX_FAULT_CODE_NUM >> 5];
} PM_CHIP_INFO;
#pragma pack()

typedef struct tag_OobDeviceTemp {
    gchar sensor_name[9];
    guint16 temp;
} OobDeviceTemp;

typedef struct tag_OobCpuInfo {
    guint16 core_count;
    guint16 thread_count;
    guint32 max_speed;
    guint32 current_speed;
    guint32 part_num;
    // 鎶ユ枃瑙勫畾ProcessorID鐨勬暟鎹?绫诲瀷涓簎int64, LSB
    guint8 processor_id[8];
    guint8 model[32];
    guint32 die_id[5];
} OobCpuInfo;

typedef struct tag_OobMemInfo {
    guint32 type;               // 绫诲瀷
    guint32 capacity;           // 瀹归噺
    guint32 present_state;      // 棰楃矑鍦ㄤ綅鐘舵�?
    guint32 rank_num;           // RANK涓?鏁?
    guint32 ecc_enable;
    guint16 bit_width;
    guint16 min_volt;           // 鏈�灏忕數鍘?
    guint32 max_speed;          // 涓婚??
    guint32 current_speed;
    guint8 manufacturer[24];    // 鍘傚晢鍚嶇О
} OobMemInfo;
extern RECEIVE_DATA_LEN opcode_datalen_table[];

extern gint32 pm_init(PM_CHIP_INFO* input);
extern void pm_clear(gpointer user_data);
extern gint32 pm_get_temp(PM_CHIP_INFO* input, guint8* buf, guint32 buf_len);
extern gint32 pm_get_sn(PM_CHIP_INFO* input, guint8* buf, guint32 buf_len);
extern gint32 pm_get_firmware(PM_CHIP_INFO* input, guint8* buf, guint32 buf_len);
extern gint32 pm_get_fault_code(PM_CHIP_INFO* input, guint8** buf, guint32* buf_len);
extern gint32 pm_get_board_id(PM_CHIP_INFO* input, guint8* buf, guint32 buf_len);
extern gint32 pm_get_pcb_id(PM_CHIP_INFO* input, guint8* buf, guint32 buf_len);
extern gint32 pm_get_health_state(PM_CHIP_INFO* input, guint8* buf, guint32 buf_len);
extern gint32 pm_get_optical_module_temp(PM_CHIP_INFO* input, guint8** buf, guint32* buf_len);
extern gint32 pm_get_chip_reset_time(PM_CHIP_INFO* input, guint8* buf, guint32 buf_len);
extern gint32 pm_get_log(PM_CHIP_INFO* input, guint16 key, guint8** buf, guint32* buf_len);
/* BEGIN: Added by h00371221, 2018/03/19   SR.SFEA02119719.013*/
extern gint32 pm_get_ddr_temp(PM_CHIP_INFO* input, guint8** buf, guint32* buf_len);
extern gint32 pm_get_shell_id(PM_CHIP_INFO* input, guint8* buf, guint32 buf_len);
/* END  : Added by h00371221, 2018/03/19   SR.SFEA02119719.013*/

/** BEGIN: Modified by qinjiaxiang, 2018/5/14 问题单号:SR.SFEA02130923.001.001 修改原因:*/
extern gint32 pm_get_netcard_link_status(PM_CHIP_INFO* input, guint8** buf, guint32* buf_len);
extern gint32 pm_get_netcard_mac_address(PM_CHIP_INFO* input, guint8** buf, guint32* buf_len);
/** END:   Added by qinjiaxiang, 2018/5/14 */

extern gint32 pm_get_binded_data(OBJ_HANDLE obj_handle, gpointer *pointer);
extern void pm_unref_binded_data(gpointer pointer);
extern void pm_ref_mutex_init(void);
gint32 pcie_card_set_pfinfo(OBJ_HANDLE extend_card_handle, PM_CHIP_INFO* pm_chip_info, 
    guint16 pf, guint16 physical_port, gchar* mac_addr);
gint32 pm_get_dev_temp(PM_CHIP_INFO *input, GSList **temp_list);
gint32 pm_get_power(PM_CHIP_INFO *input, guint8 *buf, guint32 buf_len);
gint32 pm_get_mcu_reset_time(PM_CHIP_INFO *input, guint8 *buf, guint32 buf_len);
gint32 pm_get_random_len_data(PM_CHIP_INFO *input, PROTOCOL_HEAD_PARA *head_para,
    guint8 **buf, guint32 *buf_len);
gint32 pm_get_fixed_len_data(PM_CHIP_INFO *input, PROTOCOL_HEAD_PARA *head_para, guint8 *buf, guint32 buf_len);
gint32 pm_get_card_obj_by_name(const gchar* netcard_name, const gchar* class_name, OBJ_HANDLE* card_obj);
void pciecard_power_on(void);
gint32 pm_send_data(PM_CHIP_INFO *input, PROTOCOL_HEAD_PARA *head_para, const guint8 *buf, guint32 buf_len);
gint32 pm_get_bios_version(PM_CHIP_INFO *input, guint8 bios_id, guint8 **buf, guint32 *buf_len);
gint32 pm_get_cpu_info(PM_CHIP_INFO *input, guint8 cpu_id, OobCpuInfo *cpu_info);
gint32 pm_get_mem_info(PM_CHIP_INFO *input, guint8 cpu_id, OobMemInfo *mem_info);
gint32 pm_get_mem_chip_temp(PM_CHIP_INFO *chip_info, guint8 cpu_id, guint8 **buf, guint32 *buf_len);
gint32 pm_get_system_time(PM_CHIP_INFO *input, guint8 *buf, guint32 buf_len);
gint32 pm_get_pcb_version(PM_CHIP_INFO *input, guint8 *buf, guint32 buf_len);
gint32 pm_get_https_boot_cert(PM_CHIP_INFO *input, guint8 **buf, guint32 *buf_len);
gint32 pm_get_bios_secure_boot(PM_CHIP_INFO *input, guint8 **buf, guint32 *buf_len);
gint32 pm_set_bios_secure_boot(PM_CHIP_INFO *input, guint8 *buf, guint32 buf_len, guint8 secureboot);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __PCIE_MGMT_H__ */



