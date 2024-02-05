/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: CPU/GPU/NPU management related macro or data structure definitions
 * Author: lihongbin 00375984
 * Create: 2012-12-22
 * Notes: Related definitions for cross-module use
 *
 */

#ifndef __XPU_DEFINE_H__
#define __XPU_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define CST1020V6_BOARD_ID_B    0x9b  /* 计算型存储独有，新增全宽计算X86 boardid */
#define CST0210V6_BOARD_ID      0xa3  /* 计算型存储：XPU */
#define CST0220V6_BOARD_ID      0xa6  /* 计算型存储：半宽计算ARM */
#define CST0221V6_BOARD_ID      0xa9  /* 计算型存储：半宽存储ARM(单P/双P) */
#define SMM0001V6_BOARD_ID      0xaf  /* 计算型存储：纯管理板 */

/* NPU相关的定义 */
#define NPU_WORK_MODE_AMP 0
#define NPU_WORK_MODE_SMP 1

#define NPU_WORK_MODE_AMP_STR "AMP"
#define NPU_WORK_MODE_SMP_STR "SMP"

/* BMA相关定义 */
#define RF_TYPE_GPUS                                "OemGPUs"
#define RF_TYPE_GPU_SUMMARY                         "OemGPUSummary"
#define RF_TYPE_UTLIZATION_ALL                      "Utilization"
#define RF_TYPE_GPU_UTLIZATION_GPU                  "GPU"
#define RF_TYPE_GPU_UTLIZATION_MEM                  "Memory"
#define RF_TYPE_NPUS                                "OemNPUs"
#define RF_TYPE_NPU_SUMMARY                         "OemNPUSummary"
#define RF_TYPE_NPU_UTLIZATION_MEM                  "Memory"
#define RF_TYPE_NPU_UTLIZATION_HBM                  "Hbm"
#define RF_TYPE_NPU_UTLIZATION_MEMBW                "MemoryBandwidth"
#define RF_TYPE_NPU_UTLIZATION_HBMBW                "HbmBandwidth"
#define RF_TYPE_NPU_UTLIZATION_AICORE               "AiCore"
#define RF_TYPE_NPU_UTLIZATION_AICPU                "AiCpu"
#define RF_TYPE_NPU_UTLIZATION_CTRLCPU              "CtrlCpu"

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __XPU_DEFINE_H__ */
