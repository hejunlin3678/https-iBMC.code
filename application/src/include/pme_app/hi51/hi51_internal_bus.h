/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2001-2020. All rights reserved.
 * Description: hi51 internal bus header file
 */
#ifndef _HI51_INTERNAL_BUS_H_
#define _HI51_INTERNAL_BUS_H_

extern __idata guint32 g_src_addr;
extern __idata guint32 g_dst_addr;
extern __idata guint32 g_data;
extern guint8 g_read_cnt;
extern guint8 g_write_cnt;

void read_reg(void);
void write_reg(void);
void read_stream(void);
void write_stream(void);
guint32 read_val32(guint32 addr);
guint32 read_val32_stream(guint32 addr);

#define READ_VAL32_WITH_TYPE(addr, val, type) (val) = (type)read_val32(addr)

#define WRITE_VAL32(addr, val) do {\
    g_dst_addr = addr;\
    g_data = val;\
    write_reg();\
} while (0);

#define READ_VAL32_STREAM_WITH_TYPE(addr, val, type) (val) = (type)read_val32_stream(addr)

#define WRITE_VAL32_STREAM(addr, val) do {\
    g_dst_addr = addr;\
    g_data = val;\
    write_stream();\
} while (0);


#define COPY_VAL32(saddr, daddr) do {\
    g_src_addr = saddr;\
    read_reg();\
    g_dst_addr = daddr;\
    write_reg();\
} while (0);

#define INC_VAL32(addr) do {\
    g_src_addr = addr;\
    read_reg();\
    g_data++;\
    g_dst_addr = addr;\
    write_reg();\
} while (0);

#endif