/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2001-2020. All rights reserved.
 * Description: hi51 type header file
 */
#ifndef __HI51_TYPE_H__
#define __HI51_TYPE_H__

#define REG_WIDTH   8U

#ifdef USE_SDCC

#define NEW_VARIABLE(name, seg, type, addr) seg __at (addr) type name
#define XDATA_U8    __xdata guint8
#define IDATA_U8    __idata guint8
#define CODE_U32    __code guint32
#define CODE_U16    __code guint16
#define CODE_U8     __code guint8
#define CODE_VPTR   __code void *

#define SFR_DEFINE(name, addr) __sfr __at (addr) name

#else /* NOT USE_SDCC */

#define NEW_VARIABLE(name, seg, type, addr)	type seg name _at_ addr
#define __idata idata
#define __xdata xdata
#define __code  code
#define __data  data
#define __bit   bit
#define __interrupt interrupt
#define __using using

#define XDATA_U8    guint8 xdata
#define IDATA_U8    guint8 idata
#define CODE_U32    guint32 code
#define CODE_U16    guint16 code
#define CODE_U8     guint8 code
#define CODE_VPTR   void code *

#define SFR_DEFINE(name, addr) sfr name = addr

#endif /* end of USE_SDCC */

typedef unsigned char           guint8;
typedef unsigned char           guchar;
typedef unsigned int            guint16;
typedef unsigned long           guint32;
typedef char                    gint8;
typedef int                     gint16;
typedef long                    gint32;
typedef char                    gchar;

/* struct */
typedef union hiDATA_U32_S {
    guint32      val32;
    guint16      val16[2];
    guint8       val8[4];
} DATA_U32_S;

typedef union {
    guint16  val16;
    guint8   val8[2];
} DATA_U16_S;

typedef struct tag_i2c_io_cfg {
    guint32 addr;
    guint32 val;
} I2C_IO_CFG_S;

#define FALSE       0
#define TRUE        1

#define ON          1
#define OFF         0

#ifndef NULL
#define NULL                0U
#endif

#define HI_SUCCESS          0
#define HI_FAILURE          (-1)

/* BIT掩码（与操作用） */
#define  PBIT0 0x01
#define  PBIT1 0x02
#define  PBIT2 0x04
#define  PBIT3 0x08
#define  PBIT4 0x10
#define  PBIT5 0x20
#define  PBIT6 0x40
#define  PBIT7 0x80
/* BIT掩码（或操作用） */
#define  NBIT0 0xfe
#define  NBIT1 0xfd
#define  NBIT2 0xfb
#define  NBIT3 0xf7
#define  NBIT4 0xef
#define  NBIT5 0xdf
#define  NBIT6 0xbf
#define  NBIT7 0x7f
#define ARRAY_SIZE(a)  (sizeof(a) / sizeof((a)[0]))

#endif /* __TYPE_H__ */