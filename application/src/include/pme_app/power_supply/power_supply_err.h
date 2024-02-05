#include <pme/common/mscm_vos.h>

#ifndef _POWER_SUPPLY_ERR_H_
#define _POWER_SUPPLY_ERR_H_

#ifdef __cplusplus  
#if __cplusplus
extern "C" {  
#endif
#endif /* __cplusplus */ 

typedef enum tag_psm_error
{
    /* 正常功能性返回 */
    PSM_OK = VOS_OK,                              
    
    /* 错误码返回 */
    PSM_ERROR_PARA_0_INVALID = DEF_ERROR_NUMBER(ERR_MODULE_ID_PSM, 0),        /* 第1参数无效 */
    PSM_ERROR_PARA_1_INVALID,                /* 第2参数无效 */
    PSM_ERROR_PARA_2_INVALID,                /* 第3参数无效 */
    PSM_ERROR_PARA_3_INVALID,                /* 第4参数无效 */
    PSM_ERROR_PARA_4_INVALID,                /* 第5参数无效 */
    PSM_ERROR_PARA_OTHER_INVALID,            /* 其他输入参数（比如从全局变量输入的参数）无效 */
    PSM_ERROR_DATA_INVALID,                  /* 文件CRC校验数据丢失 */
    PSM_ERROR_51_OPEN,                       /* 51协处理器打开失败 */
    PSM_ERROR_CANT_READ_FIRMWARE,            /* 无法读取51固件文件 */
    PSM_ERROR_CANT_OPEN_FIRMWARE,            /* 无法打开51固件文件 */
    PSM_ERROR_CANT_LOAD_FIRMWARE,            /* 无法正确加载51固件文件 */
    PSM_ERROR_51_START_FAIL,                 /* 51协处理器开机失败 */
    PSM_ERROR_I2C_BUS_ID_INVALID,            /* I2C总线号超过最大值 */
    PSM_ERROR_I2C_SPEED_INVALID,             /* I2C总线速率配置值无效 */
    PSM_ERROR_POWER_SRC_INVALID,             /* 功率数据来源无效 */
    PSM_ERROR_GET_REGS_FAIL,                 /* 读取多个寄存器值失败 */
    PSM_ERROR_GET_REG_FAIL,                  /* 读取单个寄存器值失败 */
    PSM_ERROR_READ_FRU_TIMEOUT,              /* 获取最新FRU信息时超时 */
    PSM_ERROR_FRU_NOT_AVAILABLE,             /* 指定电源的FRU信息不存在 */
    PSM_ERROR_NOT_SUPPORT_CAPPING,           /* 不支持通过BMC设置功率封顶 */
    /* BEGIN: 2018-11-22 田鹏twx572344 问题单号：DTS2018112200734 */
    PSM_ERROR_LIMIT_POLICY_STATUS_FAIL              /*	获取封顶策略生效状态失败 */
    /* END:   2018-11-22 田鹏twx572344 问题单号：DTS2018112200734 */
}PSM_ERROR_E;

#ifdef __cplusplus  
#if __cplusplus
}  
#endif
#endif /* __cplusplus */

#endif
