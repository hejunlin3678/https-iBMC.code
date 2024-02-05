/* 
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved. 
 * Description: 参考BRCM StorelibIR3实现的相关定义
 * Author: 任乐乐 r00355870
 * Create: 2020-2-26 
 */
#ifndef __SML_PHY_EVENT_H__
#define __SML_PHY_EVENT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

// PhyEvent Code
#define SML_PHY_EVENT_CODE_NO_EVENT                    0x00
#define SML_PHY_EVENT_CODE_INVALID_DWORD               0x01
#define SML_PHY_EVENT_CODE_RUNNING_DISPARITY_ERROR     0x02
#define SML_PHY_EVENT_CODE_LOSS_DWORD_SYNC             0x03
#define SML_PHY_EVENT_CODE_PHY_RESET_PROBLEM           0x04
#define SML_PHY_EVENT_CODE_ELASTICITY_BUF_OVERFLOW     0x05
#define SML_PHY_EVENT_CODE_RX_ERROR                    0x06
#define SML_PHY_EVENT_CODE_RX_ADDR_FRAME_ERROR         0x20
#define SML_PHY_EVENT_CODE_TX_AC_OPEN_REJECT           0x21
#define SML_PHY_EVENT_CODE_RX_AC_OPEN_REJECT           0x22
#define SML_PHY_EVENT_CODE_TX_RC_OPEN_REJECT           0x23
#define SML_PHY_EVENT_CODE_RX_RC_OPEN_REJECT           0x24
#define SML_PHY_EVENT_CODE_RX_AIP_PARTIAL_WAITING_ON   0x25
#define SML_PHY_EVENT_CODE_RX_AIP_CONNECT_WAITING_ON   0x26
#define SML_PHY_EVENT_CODE_TX_BREAK                    0x27
#define SML_PHY_EVENT_CODE_RX_BREAK                    0x28
#define SML_PHY_EVENT_CODE_BREAK_TIMEOUT               0x29
#define SML_PHY_EVENT_CODE_CONNECTION                  0x2A
#define SML_PHY_EVENT_CODE_PEAKTX_PATHWAY_BLOCKED      0x2B
#define SML_PHY_EVENT_CODE_PEAKTX_ARB_WAIT_TIME        0x2C
#define SML_PHY_EVENT_CODE_PEAK_ARB_WAIT_TIME          0x2D
#define SML_PHY_EVENT_CODE_PEAK_CONNECT_TIME           0x2E
#define SML_PHY_EVENT_CODE_TX_SSP_FRAMES               0x40
#define SML_PHY_EVENT_CODE_RX_SSP_FRAMES               0x41
#define SML_PHY_EVENT_CODE_TX_SSP_ERROR_FRAMES         0x42
#define SML_PHY_EVENT_CODE_RX_SSP_ERROR_FRAMES         0x43
#define SML_PHY_EVENT_CODE_TX_CREDIT_BLOCKED           0x44
#define SML_PHY_EVENT_CODE_RX_CREDIT_BLOCKED           0x45
#define SML_PHY_EVENT_CODE_TX_SATA_FRAMES              0x50
#define SML_PHY_EVENT_CODE_RX_SATA_FRAMES              0x51
#define SML_PHY_EVENT_CODE_SATA_OVERFLOW               0x52
#define SML_PHY_EVENT_CODE_TX_SMP_FRAMES               0x60
#define SML_PHY_EVENT_CODE_RX_SMP_FRAMES               0x61
#define SML_PHY_EVENT_CODE_RX_SMP_ERROR_FRAMES         0x63
#define SML_PHY_EVENT_CODE_HOTPLUG_TIMEOUT             0xD0
#define SML_PHY_EVENT_CODE_MISALIGNED_MUX_PRIMITIVE    0xD1
#define SML_PHY_EVENT_CODE_RX_AIP                      0xD2
#define SML_PHY_EVENT_CODE_LCARB_WAIT_TIME             0xD3
#define SML_PHY_EVENT_CODE_RCVD_CONN_RESP_WAIT_TIME    0xD4
#define SML_PHY_EVENT_CODE_LCCONN_TIME                 0xD5
#define SML_PHY_EVENT_CODE_SSP_TX_START_TRANSMIT       0xD6
#define SML_PHY_EVENT_CODE_SATA_TX_START               0xD7
#define SML_PHY_EVENT_CODE_SMP_TX_START_TRANSMT        0xD8
#define SML_PHY_EVENT_CODE_TX_SMP_BREAK_CONN           0xD9
#define SML_PHY_EVENT_CODE_SSP_RX_START_RECEIVE        0xDA
#define SML_PHY_EVENT_CODE_SATA_RX_START_RECEIVE       0xDB
#define SML_PHY_EVENT_CODE_SMP_RX_START_RECEIVE        0xDC

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif

