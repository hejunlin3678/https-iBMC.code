/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2001-2020. All rights reserved.
 * Description: hi51 pmbus header file
 */
#ifndef _HI51_PMBUS_H_
#define _HI51_PMBUS_H_

#ifdef _IN_51_

#define WFLAG 0
#define RFLAG 1

typedef struct {
    guint8 slave_w;
    guint8 cmd;
    guint8 slave_r;
} CRC_MSG_HEAD;

typedef union {
    struct crc_byte_msg {
        CRC_MSG_HEAD head;
        guint8 byte;
    } msg;
    guint8 val8[sizeof(struct crc_byte_msg)];
} CRC_BYTE_MSG;

typedef union {
    struct crc_word_msg {
        CRC_MSG_HEAD head;
        guint8 byte[2]; /* 2 个字节 */
    } msg;
    guint8 val8[sizeof(struct crc_word_msg)];
} CRC_WORD_MSG;

/* 接收数据(包括校验和)不超过 12 时才校验 */
#define BLOCK_MSG_CRC_CHK_THRESHOLDS 12
typedef union {
    struct crc_block_msg {
        CRC_MSG_HEAD head;
        union { /* |len|data[1]|data[2]|...|data[len]|checksum| */
            guint8 val8[BLOCK_MSG_CRC_CHK_THRESHOLDS]; /* 首字节为长度 */
            guint32 val32[BLOCK_MSG_CRC_CHK_THRESHOLDS / sizeof(guint32)];
        } body;
    } msg;
    guint8 val8[sizeof(struct crc_block_msg)];
} CRC_BLOCK_MSG;
void crc_msg_head_set(CRC_MSG_HEAD *head, guint8 slave, guint8 command);
guint8 pmbus_crc8_make_bitwise(const guint8 *buf, guint16 size);
guint8  pmbus_get_raw_byte (guint8 bus_id, guint8 slave, guint8 command);
guint16 pmbus_get_raw_word(guint8 bus_id, guint8 slave, guint8 command);
guint16 pmbus_get_linear_11(guint8 bus_id, guint8 slave, guint8 command);
void pmbus_get_block(guint8 bus_id, guint8 slave, guint8 command, guint32 buf_offset);
guint16 pmbus_get_linear_16(guint8 bus_id, guint8 slave, guint8 command, guint8 dev_type);
#endif

#endif