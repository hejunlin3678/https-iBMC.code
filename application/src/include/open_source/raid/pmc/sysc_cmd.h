/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: 对外命令定义
 * Author: huangleisheng 00475287
 * Create: 2021年3月1日
 */

#ifndef _SYSC_CMD_H_
#define _SYSC_CMD_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#ifndef BIT
#define BIT(x)    (1 << (x))
#endif

#define SC_SET_LD_LABLE          BIT(0)
#define SC_SET_LD_ACCELERATOR    BIT(1)
#define SC_SET_LD_WRITE_POLICY   BIT(2)
#define SC_SET_LD_SIZE           BIT(3)
#define SC_SET_LD_STRIP_SIZE     BIT(4)

#define SC_CMD_CAN_OFFSET        0x80

/* 为了和kRAID1ADM和kRAID10ADM的取值不同 */
#define SC_RAID1_TRIPLE    0xFE
#define SC_RAID10_TRIPLE   0xFD

enum SC_CMD_TYPE
{
    SC_SYSTEM_CMD_TYPE   = 0,
    SC_CTRL_CMD_TYPE     = 1,
    SC_PD_CMD_TYPE       = 2,
    SC_LD_CMD_TYPE       = 3,
    SC_CONFIG_CMD_TYPE   = 4,
    SC_BBU_CMD_TYPE      = 5,
    //@@@@@ Always add new commands here @@@@@//

    SC_MAX_CMD_TYPE = 0xFF,
    //@@@@@ Do NOT add new cmd type below!!@@@@@//
};

enum SC_SYSTEM_CMD
{
   /*
    * SC_INIT_LIB
    * Desc: 初始化storageCore
    * Input: 无
    * Output: 无
    * storageCore Error Codes: 无
    */
    SC_INIT_LIB = 0,

   /*
    * SC_UNINIT_LIB
    * Desc: 去初始化storageCore
    * Input: 无
    * Output: 无
    * storageCore Error Codes: 无
    */
    SC_UNINIT_LIB,
};

enum SC_CTRL_CMD
{
   /*
    * SC_INIT_CDA
    * Desc: 初始化CDA
    * Input: 控制器id
    * Output: 无
    * storageCore Error Codes: 无
    */
    SC_INIT_CDA,

   /*
    * SC_INIT_CDA
    * Desc: 去初始化CDA
    * Input: 控制器id
    * Output: 无
    * storageCore Error Codes: 无
    */
    SC_UNINIT_CDA,

   /*
    * SC_REINIT_CDA
    * Desc: 重新初始化CDA
    * Input: 控制器id
    * Output: 无
    * storageCore Error Codes: 无
    */
    SC_REINIT_CDA,

   /*
    * SC_GET_CTRL_INFO
    * Desc: 获取控制器信息
    * Input: 控制器id
    * Output: SC_CTRL_INFO
    * storageCore Error Codes: 无
    */
    SC_GET_CTRL_INFO,

   /*
    * SC_SET_CTRL_MODE
    * Desc: 设置控制器模式
    * Input: 模式
    * Output: 无
    * storageCore Error Codes: 无
    */
    SC_SET_CTRL_MODE,

    /*
    * SC_SET_CTRL_NBWC
    * Desc: 设置无电池写缓存
    * Input: 控制器id
    * Output: 无
    */
    SC_SET_CTRL_NBWC,

    /*
    * SC_SET_CTRL_READ_CACHE_PERCENT
    * Desc: 设置读缓存百分比
    * Input: 控制器id
    * Output: 无
    */
    SC_SET_CTRL_READ_CACHE_PERCENT,

    /*
    * SC_SET_CTRL_WRITE_CACHE_POLICY
    * Desc: 设置写缓存策略
    * Input: 控制器id
    * Output: 无
    */
    SC_SET_CTRL_WRITE_CACHE_POLICY,

    /*
    * SC_GET_CTRL_LOG
    * Desc: 收集控制器日志
    * Input: 控制器id
    * Output: 无
    */
    SC_GET_CTRL_LOG,
};

enum SC_PD_CMD
{
   /*
    * SC_GET_PD_LIST
    * Desc: 获取物理盘id列表
    * Input: 控制器id
    * Output: SC_PD_LIST
    * storageCore Error Codes: 无
    */
    SC_GET_PD_LIST = 0,

   /*
    * SC_GET_PD_INFO
    * Desc: 获取物理盘信息
    * Input: 控制器id，硬盘id
    * Output: SC_PD_INFO
    * storageCore Error Codes: 无
    */
    SC_GET_PD_INFO,

   /*
    * SC_START_LOCATE_PD
    * Desc: 开始硬盘点灯
    * Input: 控制器id，硬盘id
    * Output: 无
    * storageCore Error Codes: 无
    */
    SC_START_LOCATE_PD,

   /*
    * SC_STOP_LOCATE_PD
    * Desc: 停止硬盘点灯
    * Input: 控制器id，硬盘id
    * Output: 无
    * storageCore Error Codes: 无
    */
    SC_STOP_LOCATE_PD,

    /*
    * SC_SET_PD_BOOT_PRIORITY
    * Desc: 设置物理盘的启动优先级
    * Input: 控制器id，硬盘id
    * Output: 无
    * storageCore Error Codes: 无
    */
    SC_SET_PD_BOOT_PRIORITY,

   /*
    * SC_ADD_SPARE
    * Desc: 添加热备盘
    * Input: 控制器id，硬盘id，SC_ADD_SPARE_INFO
    * Output: 无
    * storageCore Error Codes: 无
    */
    SC_ADD_SPARE,

   /*
    * SC_REMOVE_SPARE
    * Desc: 移除热备盘
    * Input: 控制器id，硬盘id
    * Output: 无
    * storageCore Error Codes: 无
    */
    SC_REMOVE_SPARE,

   /*
    * SC_ADD_SPARE_CAN
    * Desc: 添加热备盘判断
    * Input: 控制器id，硬盘id，SC_ADD_SPARE_INFO
    * Output: 无
    * storageCore Error Codes: 无
    */
    SC_ADD_SPARE_CAN = (SC_ADD_SPARE + SC_CMD_CAN_OFFSET),

   /*
    * SC_REMOVE_SPARE_CAN
    * Desc: 移除热备盘判断
    * Input: 控制器id，硬盘id
    * Output: 无
    * storageCore Error Codes: 无
    */
    SC_REMOVE_SPARE_CAN = (SC_REMOVE_SPARE + SC_CMD_CAN_OFFSET),
};

enum SC_LD_CMD_TYPE
{
   /*
    * SC_GET_LD_LIST
    * Desc: 获取逻辑盘id列表
    * Input: 控制器id
    * Output: SC_LD_LIST
    * storageCore Error Codes: 无
    */
    SC_GET_LD_LIST = 0,

   /*
    * SC_GET_LD_INFO
    * Desc: 获取逻辑盘信息
    * Input: 控制器id，逻辑盘id
    * Output: SC_LD_INFO
    * storageCore Error Codes: 无
    */
    SC_GET_LD_INFO,

   /*
    * SC_GET_PD_IN_LD
    * Desc: 获取逻辑盘的成员盘id列表
    * Input: 控制器id，逻辑盘id
    * Output: SC_PD_IN_LD
    * storageCore Error Codes: 无
    */
    SC_GET_PD_IN_LD,

   /*
    * SC_CREATE_LD
    * Desc: 创建逻辑盘
    * Input: 控制器id，SC_CREATE_LD_INFO
    * Output: 逻辑盘id
    * storageCore Error Codes: 无
    */
    SC_CREATE_LD,

   /*
    * SC_DELETE_LD
    * Desc: 删除逻辑盘
    * Input: 控制器id，逻辑盘id
    * Output: 无
    * storageCore Error Codes: 无
    */
    SC_DELETE_LD,

    /*
    * SC_SET_LD_BOOT_PRIORITY
    * Desc: 设置逻辑盘的启动优先级
    * Input: 控制器id，逻辑盘id
    * Output: 无
    * storageCore Error Codes: 无
    */
    SC_SET_LD_BOOT_PRIORITY,

   /*
    * SC_SET_LD_PROPERTIES
    * Desc: 设置逻辑盘属性
    * Input: 控制器id，逻辑盘id，SC_SET_LD_PROP
    * Output: 无
    * storageCore Error Codes: 无
    */
    SC_SET_LD_PROPERTIES,
};

enum SC_CONFIG_CMD_TYPE
{
   /*
    * SC_GET_ARRAY_LIST
    * Desc: 获取硬盘阵列id列表
    * Input: 控制器id
    * Output: SC_ARRAY_LIST
    * storageCore Error Codes: 无
    */
    SC_GET_ARRAY_LIST = 0,

   /*
    * SC_GET_ARRAY_INFO
    * Desc: 获取硬盘阵列信息
    * Input: 控制器id，硬盘阵列id
    * Output: SC_ARRAY_INFO
    * storageCore Error Codes: 无
    */
    SC_GET_ARRAY_INFO,
};

enum SC_BBU_CMD_TYPE
{
   /*
    * SC_GET_BBU_INFO
    * Desc: 获取BBU信息
    * Input: 控制器id
    * Output: SC_BBU_INFO
    * storageCore Error Codes: 无
    */
    SC_GET_BBU_INFO = 0,
};

typedef enum
{
    SC_LOG_LAST_CRASH_DUMP,
    SC_LOG_SERIAL_OUTPUT,
} SC_LOG_TYPE;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif