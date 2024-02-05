/*
* Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
* Description: header file that contains constant.
* Author: AdaptiveLM team
* Create: 2015-07-14
*/
#ifndef __ALM_INTF_CONSTANT_H__
#define __ALM_INTF_CONSTANT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */

#if defined(ALM_PACK_4)
#pragma pack(4)
#elif defined(ALM_PACK_8)
#pragma pack(8)
#endif

/* AdaptiveLM当前版本编号 */
#define ALM_CONST_LIC_CBB_C_VER "AdaptiveLMV100R005C10SPC047"
#define ALM_CONST_LIC_CBB_B_VER "AdaptiveLMV100R005C10SPC047B004"
#define ALM_CONST_LIC_CBB_D_VER "AdaptiveLMV100R005C10SPC047B004T63266"

/* **************************** 常用常量 ********************************* */
/* FIXTIME查询到组状态数 */
#define ALM_CONST_FIXTIME_GROUPNUM 4

/* 一般定义名称用的长度 */
#define ALM_CONST_NAME_MAX_LEN 63

/* 日期转换为字符串的长度xxxx-xx-xx xx:xx:xx */
#define ALM_CONST_DATE_LEN 19

/* 哈希结果的长度 */
#define ALM_CONST_HASH_LEN 72

/* ************************ License文件及其相关 ************************** */
/* 失效码的最大长度 */
#define ALM_CONST_RVK_TIKET_LEN 128

/* 产品名最大长度，包含License文件中产品名及注册产品时指定的产品名 */
#define ALM_CONST_PRD_NAME_LEN_MAX 54

/* 产品版本最大长度，包含License文件中产品版本及注册产品时指定的产品版本 */
#define ALM_CONST_PRD_VER_MAX 30

/* License常用的名称长度 */
#define ALM_CONST_LIC_NAME_MAX 64

/* License常用的描述信息的长度 */
#define ALM_CONST_LIC_DESC_MAX 256

/* License常用的量纲的长度 */
#define ALM_CONST_LIC_UNIT_MAX 64

/* License文件规格限制 */
#define ALM_CONST_LIC_LSN 17
#define ALM_CONST_LIC_TYPE_MAX 16

#define ALM_CONST_LIC_FILE_LEN_MAX (2 * 1024 * 1024)
#define ALM_CONST_LIC_ALL_ESN_LEN_MAX 2048
#define ALM_CONST_LIC_ESN_FEAT_FORMAT_LEN_MAX (ALM_CONST_LIC_ALL_ESN_LEN_MAX + 2)
#define ALM_CONST_LIC_SINGLE_ESN_LEN_MAX 40

#define ALM_CONST_LIC_FILE_VER_LEN_MAX 31
#define ALM_CONST_LIC_FILE_TYPE_LEN_MAX 31
#define ALM_CONST_LIC_COPY_RIGHT_MAX 128
#define ALM_CONST_LIC_CREATER_MAX 128
#define ALM_CONST_LIC_ISSUER_MAX 128
#define ALM_CONST_LIC_COUNTRY_MAX 128
#define ALM_CONST_LIC_CUSTOMER_MAX 512
#define ALM_CONST_LIC_OFFICE_MAX 128
#define ALM_CONST_LIC_DOMAIN_MAX 64

#define ALM_CONST_LIC_OFFERING_NAME_MAX 64
#define ALM_CONST_LIC_OFFERING_VER_MAX 64

#define ALM_CONST_LIC_NODE_NAME_MAX ALM_CONST_LIC_NAME_MAX
#define ALM_CONST_LIC_NODE_DES_MAX 128
#define ALM_CONST_LIC_NODE_LOCKMODEL_MAX 16

#define ALM_CONST_LIC_SOFT_ID_MAX 64

#define ALM_CONST_LIC_BBOMGROUP_NAME_MAX ALM_CONST_NAME_MAX_LEN

#define ALM_CONST_LIC_ATRRIBULT_MAX 128
#define ALM_CONST_LIC_BBOM_NAME_MAX 63
#define ALM_CONST_LIC_GROUP_NAME_MAX ALM_CONST_LIC_NAME_MAX
#define ALM_CONST_LIC_SBOM_NAME_MAX ALM_CONST_LIC_NAME_MAX
#define ALM_CONST_LIC_SBOM_DESC_CHS_UNIT_MAX ALM_CONST_LIC_UNIT_MAX
#define ALM_CONST_LIC_SBOM_DESC_ENG_UNIT_MAX ALM_CONST_LIC_UNIT_MAX
#define ALM_CONST_LIC_SBOM_DESC_CHS_MAX ALM_CONST_LIC_DESC_MAX
#define ALM_CONST_LIC_SBOM_DESC_ENG_MAX ALM_CONST_LIC_DESC_MAX
#define ALM_CONST_LIC_PACK_NAME_MAX ALM_CONST_LIC_NAME_MAX
#define ALM_CONST_LIC_PACK_DESC_CHS_UNIT_MAX ALM_CONST_LIC_UNIT_MAX
#define ALM_CONST_LIC_PACK_DESC_ENG_UNIT_MAX ALM_CONST_LIC_UNIT_MAX
#define ALM_CONST_LIC_PACK_DESC_CHS_MAX ALM_CONST_LIC_DESC_MAX
#define ALM_CONST_LIC_PACK_DESC_ENG_MAX ALM_CONST_LIC_DESC_MAX

/* License中截止日期字符串若为此值，表示永久 */
#define ALM_CONST_LIC_DEADLINE_PERM "PERMANENT"

/* ******************************** 其他常量 ********************************* */
/* 产品适配的获取环境ESN回调得到的ESN允许的最大长度 */
#define ALM_CONST_ENV_ESN_MAX_LEN 2047

/* 密钥最大长度，密钥段 + 数字签名、冒号分隔、算法 + 产品名 + 产品版本 */
#define ALM_CONST_KEY_MAX_LEN (3752 * 2 + 25 + ALM_CONST_PRD_NAME_LEN_MAX + ALM_CONST_PRD_VER_MAX)

/* 单次打印日志最大长度，超出会被截断 */
#define ALM_CONST_LOG_MAX_LEN 511

/* 紧急默认允许启动次数 */
#define ALM_CONST_EMERGENCY_DFLT_COUNT 3

/* 紧急允许启动次数的最大值 */
#define ALM_CONST_EMERGENCY_MAX_COUNT 10

/* 紧急启动后，允许运行的天数 */
#define ALM_CONST_EMERGENCY_DAYS 7

/* STICK允许启动次数可配的最大值 */
#define ALM_CONST_STICK_MAX_COUNT 2

/* STICK默认允许启动次数 */
#define ALM_CONST_STICK_DFLT_COUNT 2

/* STICK启动后默认允许运行的天数 */
#define ALM_CONST_STICK_DFLT_DAYS 30

/* STICK启动后允许运行的天数可配的最大值 */
#define ALM_CONST_STICK_MAX_DAYS 180

/* 持久化文件最大允许的产品数 */
#define ALM_CONST_PS_PRD_MAX_NUM 10

/* 持久化单个产品最多允许保存多少个License记录 */
#define ALM_CONST_PS_PRD_LIC_MAX_NUM 20

/* 持久化单个产品允许保存多少个失效license记录 */
#define ALM_CONST_PS_PRD_RVK_LIC_MAX_NUM 20

/* 默认状态刷新点数，[AM 2:00, AM 3:00) */
#define ALM_CONST_STATE_REFRESH_DFLT_HOUR 2

/* 默认状态刷新间隔，单位为分钟 */
#define ALM_CONST_STATE_REFRESH_DFLT_INTERVAL 30

/* ******************************** 错误码 ********************************* */
/* 错误码起始值 */
#define ALM_ERR_BASE 0x79000000

/* Enum for return codes */
typedef enum {
    /* 返回成功 */
    ALM_OK = 0,

    /* 未知错误 */
    ALM_ERR_UNKNOW = (ALM_ERR_BASE | 0x01),

    /* 安全函数返回错误 */
    ALM_ERR_SECURE_ERROR = (ALM_ERR_BASE | 0x02),

    /* ******************  参数合法性相关错误码 ******************* */
    /* 启动指定的AdaptiveLm版本与LIB库版本不一致 */
    /* 有可能使用的AdaptiveLm头文件与AdaptiveLm库版本不一致 */
    ALM_ERR_HEAD_AND_LIB_MIS = (ALM_ERR_BASE | 0x10),

    /* 空指针错误 */
    ALM_ERR_NULL_PTR = (ALM_ERR_BASE | 0x11),

    /* 无效的参数 */
    ALM_ERR_INVALID_PARA = (ALM_ERR_BASE | 0x12),

    /* 数据溢出 */
    ALM_ERR_DATA_OVERFLOW = (ALM_ERR_BASE | 0x13),

    /* 字符串的长度超出最大值 */
    ALM_ERR_STR_OVERFLOW = (ALM_ERR_BASE | 0x14),

    /* 待查询结构体还没有初始化 */
    ALM_ERR_QUERY_DATA_NOT_INIT = (ALM_ERR_BASE | 0x15),

    /* *******************  适配函数相关错误码 ******************** */
    /* 内存不足 */
    ALM_ERR_MEM_NOT_ENOUGH = (ALM_ERR_BASE | 0x20),

    /* 没有注册锁相关回调函数 */
    ALM_ERR_MUTEX_FUNCS_NOT_REG = (ALM_ERR_BASE | 0x21),

    /* 没有注册定时器相关回调函数 */
    ALM_ERR_TIMER_FUNCS_NOT_REG = (ALM_ERR_BASE | 0x22),

    /* 没有注册获取时间回调函数 */
    ALM_ERR_GET_TIME_FUNC_NOT_REG = (ALM_ERR_BASE | 0x23),

    /* 没有注册获取ESN回调函数 */
    ALM_ERR_GET_ESN_FUNC_NOT_REG = (ALM_ERR_BASE | 0x24),

    /* 没有注册读写回调函数 */
    ALM_ERR_RW_FUNC_NOT_REG = (ALM_ERR_BASE | 0x25),

    /* 没有注册内存分配释放回调函数 */
    ALM_ERR_MEM_FUNCS_NOT_REG = (ALM_ERR_BASE | 0x26),

    /* 调用创建锁回调函数失败 */
    ALM_ERR_OS_MUTEX_CREATE_FAILED = (ALM_ERR_BASE | 0x27),

    /* 调用释放锁回调函数失败 */
    ALM_ERR_OS_MUTEX_RELEASE_FAILED = (ALM_ERR_BASE | 0x28),

    /* 调用删除锁回调函数失败 */
    ALM_ERR_OS_MUTEX_DEL_FAILED = (ALM_ERR_BASE | 0x29),

    /* 调用获取锁回调函数失败 */
    ALM_ERR_OS_MUTEX_AQUIRE_FAILED = (ALM_ERR_BASE | 0x2A),

    /* 调用获取时间函数失败 */
    ALM_ERR_OS_GET_TIME_FAILED = (ALM_ERR_BASE | 0x2B),

    /* 调用启动定时器回调失败 */
    ALM_ERR_OS_TIMER_START_FAILED = (ALM_ERR_BASE | 0x2C),

    /* 调用停止定时器回调失败 */
    ALM_ERR_OS_TIMER_STOP_FAILED = (ALM_ERR_BASE | 0x2D),

    /* 申请内存失败 */
    ALM_ERR_OS_MALLOC_FAILED = (ALM_ERR_BASE | 0x2E),

    /* 调用写保密文件回调失败 */
    ALM_ERR_ADAPTER_PS_WRITE_FAILED = (ALM_ERR_BASE | 0x2F),

    /* 调用读保密文件回调失败 */
    ALM_ERR_ADAPTER_PS_READ_FAILED = (ALM_ERR_BASE | 0x30),

    /* 调用notify 回调失败 */
    ALM_ERR_ADAPTER_NOTIFY_FAILED = (ALM_ERR_BASE | 0x31),

    /* 调用alarm 回调失败 */
    ALM_ERR_ADAPTER_ALARM_FAILED = (ALM_ERR_BASE | 0x32),

    /* 调用ps c01 read 回调失败 */
    ALM_ERR_ADAPTER_PS_C01_READ_FAILED = (ALM_ERR_BASE | 0x33),

    /* 调用 c01 get prds 回调失败 */
    ALM_ERR_ADAPTER_PS_C01_GET_PRDS_FAILED = (ALM_ERR_BASE | 0x34),

    /* 调用ps c01 Write 回调失败 */
    ALM_ERR_ADAPTER_PS_C01_WRITE_FAILED = (ALM_ERR_BASE | 0x35),

    /* 注册读写低版本保密文件相关回调错误 */
    ALM_ERR_OP_OLD_VER_PS = (ALM_ERR_BASE | 0x36),

    /* *******************  产品管理相关错误码 ******************** */
    /* 还没有启动 */
    ALM_ERR_NOT_START = (ALM_ERR_BASE | 0x40),

    /* 已经完成启动 */
    ALM_ERR_HAVE_START = (ALM_ERR_BASE | 0x41),

    /* 不存在指定产品 */
    ALM_ERR_PRD_NOT_EXIST = (ALM_ERR_BASE | 0x42),

    /* 产品已经存在 */
    ALM_ERR_PRD_EXIST = (ALM_ERR_BASE | 0x43),

    /* 当前产品数已经超出持久化配置的最大数目 */
    ALM_ERR_PRD_OVER_PS_MAX_NUM = (ALM_ERR_BASE | 0x44),

    /* 无效的产品BBOM 值 */
    ALM_ERR_PRD_BBOM_VALUE_INVALID = (ALM_ERR_BASE | 0x45),

    /* 没有指定BBOM */
    ALM_ERR_PRD_BBOM_IS_NULL = (ALM_ERR_BASE | 0x46),

    /* BBOM名超长 */
    ALM_ERR_PRD_BBOM_NAME_EXCEED_MAX_LEN = (ALM_ERR_BASE | 0x47),

    /* BBOM编号重复 */
    ALM_ERR_PRD_BBOM_SAME_ID = (ALM_ERR_BASE | 0x48),

    /* BBOM名重复 */
    ALM_ERR_PRD_BBOM_SAME_NAME = (ALM_ERR_BASE | 0x49),

    /* BBOM类型无效 */
    ALM_ERR_PRD_BBOM_TYPE_INVALID = (ALM_ERR_BASE | 0x4A),

    /* 无效的密钥 */
    ALM_ERR_INVALID_KEY = (ALM_ERR_BASE | 0x4B),

    /* 产品名超过最大长度 */
    ALM_ERR_PRD_NAME_EXCEED_MAX_LEN = (ALM_ERR_BASE | 0x4C),

    /* 产品版本号超过最大长度 */
    ALM_ERR_PRD_VER_EXCEED_MAX_LEN = (ALM_ERR_BASE | 0x4D),

    /* 产品当前处于disable状态 */
    ALM_ERR_PRD_DISABLED = (ALM_ERR_BASE | 0x4E),

    /* 产品当前处于Not Disable 状态 */
    ALM_ERR_PRD_NOT_DISABLED = (ALM_ERR_BASE | 0x4F),

    /* *******************  License管理相关错误码 ******************** */
    /* License 文件格式错误 */
    ALM_ERR_LIC_FORMAT = (ALM_ERR_BASE | 0x60),

    /* License 文件长度为0或文件字符串为空 */
    ALM_ERR_LIC_IS_NULL = (ALM_ERR_BASE | 0x61),

    /* 校验License文件签名失败 */
    ALM_ERR_LIC_CHECK_SIGN = (ALM_ERR_BASE | 0x62),

    /* 已经激活了License文件 */
    ALM_ERR_LIC_EXIST = (ALM_ERR_BASE | 0x63),

    /* License已经超期或ESN、产品名、版本、失效等原因导致 */
    ALM_ERR_LIC_INVALID = (ALM_ERR_BASE | 0x64),

    /* 一次性在同一产品上激活多个License */
    ALM_ERR_LIC_ACTIVED_MORE_THAN_ONE_AT_A_TIME = (ALM_ERR_BASE | 0x65),

    /* 不存在指定的License文件 */
    ALM_ERR_LIC_NOT_EXIST = (ALM_ERR_BASE | 0x66),

    /* License文件中不存在指定产品已注册的BBOM */
    ALM_ERR_LIC_NO_BBOM_REGISTED_TO_PRD = (ALM_ERR_BASE | 0x67),

    /* ESN长度超长 */
    ALM_ERR_LIC_ESN_EXCEED_MAX_LEN = (ALM_ERR_BASE | 0x68),

    /* 产品不支持临时项失效功能 */
    ALM_ERR_FIX_TIME_NOT_SUPPORT = (ALM_ERR_BASE | 0x69),

    /* 打开临时项失效功能的
     * license必须是永久的带有临时项的商用license
     */
    ALM_ERR_FIX_TIME_LK_NOT_MATCH = (ALM_ERR_BASE | 0x6A),

    /* License文件长度超长 */
    ALM_ERR_LIC_EXCEED_MAX_LEN = (ALM_ERR_BASE | 0x6B),

    /* License文件不存在对应SpartName 或者 BpartName */
    ALM_ERR_LIC_S2B_BPART_SPART_NOT_EXIST = (ALM_ERR_BASE | 0x6C),

    /* License SBOM 中spart value全为0(S2B 中有效授权总量为0) */
    ALM_ERR_LIC_S2B_SBOM_VALUE_INVALID = (ALM_ERR_BASE | 0x6D),

    /* FIX TIME 失效组名非法 */
    ALM_ERR_FIX_TIME_GROUP_NAME_INVALID = (ALM_ERR_BASE | 0x6E),

    /* Service段不支持 FIX TIME 功能 */
    ALM_ERR_FIX_TIME_SERVICE_NOT_SUPPORT = (ALM_ERR_BASE | 0X70),

    /* *******************  策略管理相关错误码 ******************** */
    /* 已经配置了stick */
    ALM_ERR_LIC_STICK_EXIST = (ALM_ERR_BASE | 0x80),

    /* 没有配置stick */
    ALM_ERR_LIC_STICK_NOT_EXIST = (ALM_ERR_BASE | 0x81),

    /* 还没有启动stick */
    ALM_ERR_LIC_STICK_NOT_RUNNING = (ALM_ERR_BASE | 0x82),

    /* stick正在运行 */
    ALM_ERR_LIC_STICK_IS_RUNNING = (ALM_ERR_BASE | 0x83),

    /* 已经配置了紧急 */
    ALM_ERR_LIC_EMERGENCY_EXIST = (ALM_ERR_BASE | 0x84),

    /* 没有配置紧急license */
    ALM_ERR_LIC_EMERGENCY_NOT_EXIST = (ALM_ERR_BASE | 0x85),

    /* 还没有启动紧急 */
    ALM_ERR_LIC_EMERGENCY_NOT_RUNNING = (ALM_ERR_BASE | 0x86),

    /* 紧急正在运行 */
    ALM_ERR_LIC_EMERGENCY_IS_RUNNING = (ALM_ERR_BASE | 0x87),

    /* 紧急或stick等启动次数已被耗光 */
    ALM_ERR_LIC_NO_USE_COUNT = (ALM_ERR_BASE | 0x88),

    /* 不通过时间安全检测 */
    ALM_ERR_SAFETIME_CHK_NOT_PASS = (ALM_ERR_BASE | 0x89),

    /* 解析持久化内容失败 */
    ALM_ERR_PS_INVALID = (ALM_ERR_BASE | 0x8A),

    /* 产品在当前状态下不允许失效 */
    ALM_ERR_REVOKE_NOT_ALLOWED = (ALM_ERR_BASE | 0x8B),

    /* 产品在紧急状态下不允许失效 */
    ALM_ERR_EM_REVOKE_NOT_ALLOWED = (ALM_ERR_BASE | 0x8C),

    /* 没有配置BBOM支持当前紧急状态 */
    ALM_ERR_LIC_EMERGENCY_NO_ITEM_IN_EME = (ALM_ERR_BASE | 0x8D),

    /* **************************状态相关****************************** */
    /* 不满足忽略校验项的配置规则 */
    ALM_ERR_STATE_NOT_MATCH_VERIFY_RULE = (ALM_ERR_BASE | 0x90),

    /* Unknown */
    ALM_ERROR_BUTT = (ALM_ERR_BASE | 0xFF)

} ALM_RET_ENUM;

#if (defined(ALM_PACK_4) || defined(ALM_PACK_8))
#pragma pack()
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */

#endif /* __ALM_INTF_CONSTANT_H__ */
