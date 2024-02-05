/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2001-2020. All rights reserved.
 * Description: hi51 ipmb trans header file
 */
#ifndef _HI51_IPMB_TRANS_H_
#define _HI51_IPMB_TRANS_H_

void ipmbt_task_trans_to_a9(guint8 task_id);
void ipmbt_task_tans_to_me(guint8 task_id);
void ipmbt_task_ack_me(guint8 task_id);

#endif