/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2001-2020. All rights reserved.
 * Description: hi51 fru header file
 */
#ifndef _FRU_H_
#define _FRU_H_

#define FRU_E2P_READ_SIZE       32 /* Must be a multiply of 4 */
#define FRU_BLOCKS              ((FRU_DATA_LEN + FRU_E2P_READ_SIZE - 1) / FRU_E2P_READ_SIZE)
#define FRU_U32_BLOCKS          ((FRU_DATA_LEN + sizeof(guint32) - 1) / sizeof(guint32))

#define FRU_STATE_IGNORE        0
#define FRU_STATE_READING       1
#define FRU_STATE_OK            2

#ifdef _IN_51_
void fru_task_fatch_fru(guint8 task_id);
void fru_start_fatch(void);
void fru_stop_fatch(void);
void fru_set_state(guint8 dev_id, guint8 state);
void fru_init(void);
extern guint8 g_fru_type;
#endif

#endif