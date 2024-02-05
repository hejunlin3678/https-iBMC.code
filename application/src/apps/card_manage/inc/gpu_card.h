

#ifndef __GPU_CARD_H__
#define __GPU_CARD_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 



#define NV_GPU_TYPE_FERMI 1  // Fermi架构GPU卡
#define NV_GPU_TYPE_KEPLER 2 // Kepler架构GPU卡
#define NV_GPU_TYPE_PASCAL 3 // Pascal架构GPU卡
#define NV_GPU_TYPE_VOLTA 4  // Volta架构GPU卡

#define NV_CMD_REG 0x5c        // SMBPBI默认的命令寄存器偏移为0x5c
#define NV_DATA_REG 0x5d       // SMBPBI的数据寄存器偏移，取决于产品，Kepler、Pascal、Volta架构GPU卡
#define NV_DATA_REG_FERMI 0x58 // Fermi架构GPU卡的DATA寄存器的地址
#define NV_REG_SIZE 4          // SMBus操作的数据长度
#define NV_CAPABILITY_NUM 4    // 当前SBMPBI支持4个Dword字段标记支持的特性
#define MAX_STATUS_POLLS 20    // 读取命令响应状态的最大尝试次数

#define GPU_CARD_DUMPINFO_MAX_LEN 255


enum NVACTION_CONVERT_TYPE {
    KEEP_DIGITAL = 0,
    KEEP_ASCII,
    ARRAY_TO_ASCII_BIGENDIAN,
    ARRAY_TO_ASCII_LITTLEENDIAN,
    KEEP_ARRAY
};

#define NV_GPU_SER_NUM_LEN 16
#define NV_GPU_FIRM_VER_LEN 14
#define NV_GPU_BUILD_DATE_LEN 8
#define NV_GPU_PROPERTY_LEN 32
#define MAX_NV_ACTION_NAME_LEN 64 // Action的名称长度


#define GPU_TEMP_GPU0_ARG1 0x00  // 主GPU温度
#define GPU_TEMP_GPU1_ARG1 0x01  // 从GPU温度(仅在双芯片时支持)
#define GPU_TEMP_BOARD_ARG1 0x04 // 板卡温度(需查询是否支持)
#define GPU_TEMP_MEM_ARG1 0x05   // HBM显存温度(需查询是否支持)


#define GPU_POWER_TOTAL_ARG1 0x00 // 整卡功耗


#define GPU_SER_NUM_ARG1 0x02    // 序列号
#define GPU_BUILD_DATE_ARG1 0x07 // 制造日期
#define GPU_FIRM_VER_ARG1 0x08   // 固件版本

#define NV_CAP_0_TEMP_GPU0_MASK 0x00000001   // bit0 : 1
#define NV_CAP_0_TEMP_GPU1_MASK 0x00000002   // bit1 : 1
#define NV_CAP_0_TEMP_BOARD_MASK 0x00000010  // bit4 : 1
#define NV_CAP_0_TEMP_MEMORY_MASK 0x00000020 // bit5 : 1
#define NV_CAP_0_TOTAL_POWER_MASK 0x00010000 // bit16 : 1

#define NV_CAP_1_SER_NUM_MASK 0x00000004      // bit2 : 1
#define NV_CAP_1_BUILD_DATE_MASK 0x00000080   // bit7 : 1
#define NV_CAP_1_FIRM_VER_MASK 0x00000100     // bit8 : 1
#define NV_CAP_1_ECC_V1_MASK 0x00010000       // bit16 : 1
#define NV_CAP_1_ECC_V2_MASK 0x00020000       // bit17 : 1
#define NV_CAP_1_ECC_V3_MASK 0x00040000       // bit18 : 1
#define NV_CAP_1_RETIRED_PAGE_MASK 0x00080000 // bit19 : 1
#define NV_CAP_1_ECC_V4_MASK 0x00100000       // bit20 : 1
#define NV_CAP_1_ECC_V5_MASK 0x00200000       // bit21 : 1

#define HW_ACCESS_FAIL_MASK 0x8000
#define HW_ACCESS_IN_UPDATE_PROGRESS 0x4000


typedef union tag_data_reg {
    guint8 byte[4];
    guint32 word;
} reg_t;


typedef struct st_smbpbi_data {
    guint8 length;
    reg_t reg_data;
} smbpbi_data_t;


typedef struct st_ecc {
    guint32 sbe;
    guint32 dbe;
} ecc_t;

enum {
    OPCODE_NULL_CMD = 0x00,
    OPCODE_GET_CAP = 0x01,          // 获取GPU卡功能特性
    OPCODE_GET_TEMP = 0x02,         // 获取温度(单精度)
    OPCODE_GET_EXT_TEMP = 0x03,     // 获取温度(扩展精度)
    OPCODE_GET_POWER = 0x04,        // 获取功率
    OPCODE_GET_GPU_INFO = 0x05,     // 获取GPU信息
    OPCODE_GET_ECC_V1 = 0x06,       // 查询ECC统计参数，格式1
    OPCODE_GET_ECC_V2 = 0x07,       // 查询ECC统计参数，格式2
    OPCODE_GET_ECC_V3 = 0x0C,       // 查询ECC统计参数，格式3
    OPCODE_SCRATCH_READ = 0x0D,     // 读缓存
    OPCODE_SCRATCH_WRITE = 0x0E,    // 写缓存
    OPCODE_SCRATCH_COPY = 0x0F,     // 缓存拷贝
    OPCODE_ASYNC_REQUEST = 0x10,    // 提交或查询异步请求
    OPCODE_REGISTER_ACCESS = 0x11,  // 获取内部状态寄存器
    OPCODE_CHK_EXT_PWR = 0x12,      // 检查外部电源
    OPCODE_GET_PG_RET_STATS = 0x13, // 读取动态内存页失效统计
    OPCODE_GET_ECC_V4 = 0x14,       // 查询ECC统计参数，格式4
    OPCODE_GET_THERMAL_PARA = 0x15, // 读取散热参数，降频温度、下电温度
    OPCODE_PWR_SWITCH = 0xF0,       // 供电开关
    OPCODE_GET_PWR_STATUS = 0xF1    // 获取供电状态
};

enum {
    CMD_STATUS_NULL = 0x00,              // 无效的状态
    CMD_STATUS_ERR_REQUEST = 0x01,       // SMBus错误
    CMD_STATUS_ERR_OPCODE = 0x02,        // 无效的操作码
    CMD_STATUS_ERR_ARG1 = 0x03,          // 无效的参数1
    CMD_STATUS_ERR_ARG2 = 0x04,          // 无效的参数2
    CMD_STATUS_ERR_DATA = 0x05,          // 无效数据
    CMD_STATUS_ERR_MISC = 0x06,          // 未知错误
    CMD_STATUS_ERR_I2C_ACCESS = 0x07,    // I2C内部错误
    CMD_STATUS_ERR_NOT_SUPPORTED = 0x08, // 请求参数不被支持
    CMD_STATUS_ERR_NOT_AVAILABLE = 0x09, // 请求当前不可用
    CMD_STATUS_ERR_BUSY = 0x0a,          // 请求正在被处理
    CMD_STATUS_ERR_AGAIN = 0x0b,         // 没有足够的资源处理该请求，请重试
    CMD_STATUS_ACCEPTED = 0x1c,          // 异步请求已被接受
    CMD_STATUS_INACTIVE = 0x1d,          // 未激活，不处理任何请求
    CMD_STATUS_READY = 0x1e,             // 就绪
    CMD_STATUS_SUCCESS = 0x1f            // 请求执行成功
};


extern gint32 gpu_card_add_object_callback(OBJ_HANDLE obj_hnd);
extern gint32 gpu_card_del_object_callback(OBJ_HANDLE obj_hnd);
extern gint32 gpu_card_dump_info(const gchar *path);
extern gint32 gpu_card_start(void);
gint32 set_gpucard_info_from_bma(OBJ_HANDLE group_obj, GSList *caller_info, GSList *input_list, GSList **output_list);
void gpucard_agentless_init(void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
