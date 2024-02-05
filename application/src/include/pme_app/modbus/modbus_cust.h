/******************************************************************************

                  版权所有 (C), 2012-2018, 华为技术有限公司

 ******************************************************************************
  文 件 名   : modbus_cust.h
  版 本 号   : 初稿
  部    门   :
  作    者   : l00375984
  生成日期   : 2018/8/28
  最近修改   :
  功能描述   : libmodbus自定义功能
  函数列表   :
******************************************************************************/
#ifndef __MODBUS_CUST_H__
#define __MODBUS_CUST_H__
    
    
#ifdef __cplusplus
#if __cplusplus
    extern "C"{
#endif
#endif /* __cplusplus */

/* UART相关宏定义 */
#ifdef ARM64_HI1711_ENABLED
#define MAX_UART_NUM 8						/* Hi1711支持的最大UART口数目UART0~UART7 */
#else
#define MAX_UART_NUM 5						/* Hi1710支持的最大UART口数目UART0~UART4 */						
#endif
#define MAX_SLAVE_NUM               16      /* 单一Modbus总线上支持的最大Slave数目 */
#define UART2                       2       /* 连接BMC的UART2, 默认的调试串口, /dev/ttyAMA0 */
#define UART3                       3       /* 连接BMC的UART3, 常用作SOL, /dev/ttySS0 */
#define UART4                       4       /* 连接BMC的UART4, 预留扩展, /dev/ttySS1 */
#define PORT3                       3
#define PORT4                       4
#define MB_RTU_DEF_BAUD_RATE        115200  /* 默认波特率 */
#define UART_MODE_RS232             0
#define UART_MODE_RS485             1
#define MODBUS_OK                   0       /* 此宏为保证返回值完整性定义，一般建议调用者判断是否为MODBUS_ERR */
#define MODBUS_ERR                  -1
#define MB_ADDR_BROADCAST           0       /* Modbus广播地址 */
#define MB_ADDR_START               1       /* Modbus有效地址起始值 */
#define MB_ADDR_END                 247     /* Modbus有效地址结束值 */

#define MB_ADDR_INVALID             255

#define MAX_MESSAGE_LENGTH          260
#define MB_DATA_LEN_OFFSET          2       /* 0:slave_addr, 1:func_code, 2:data_len */

/* Modbus协议数据结构 */
typedef struct modbus
{
    uint8_t         slave;          /* MODBUS slave addr : 0~247 */
    uint8_t         type;           /* MODBUS type : 0, RTU; 1, ASCII, 2,TCP*/
    uint8_t         mode;           /* UART phy mode: 0, RS232; 1, RS485 */
    char            chan;           /* UART channel: 0、1、2、3、4 */
    uint32_t        baud;           /* Bauds: 9600, 19200, 57600, 115200, etc */
    int             s;				/* File descriptor */
} modbus_t;

typedef enum
{
    MODBUS_TYPE_RTU = 0x00,
    MODBUS_TYPE_TCP,
} mb_type_t;

typedef struct tag_mb_cmd_req
{
    /* Modbus上下文句柄 */
    modbus_t*           ctx;
    
    /* Modbus请求帧 */
    uint8_t             data[MAX_MESSAGE_LENGTH];

    /* Modbus请求帧长度 */
    uint32_t            req_len;

    /* 响应超时时间，默认值0表示使用默认超时 */
    uint32_t            timeout_ms;

    /* 默认值0, 表示正常接收响应;1，表示忽略响应 */
    uint8_t             ignore_rsp_flag;
}MB_CMD_REQ_S;

typedef struct tag_mb_cmd_resp
{
    /* Modbus响应帧 */
    uint8_t             data[MAX_MESSAGE_LENGTH];
    uint8_t             error_code;
}MB_CMD_RESP_S;

/* BEGIN: Added by zhongqiu 00356691, 2019/3/16   PN:DTS2019030608069-Modbus通道使用率提升 */
#define MB_RECV_MAX_TMOUT                    500 /* Modbus命令响应最大超时时间，500ms */
/* END:   Added by zhongqiu 00356691, 2019/3/16   PN:DTS2019030608069-Modbus通道使用率提升 */

#define MB_MIN_CMD_INTERVAL         20    /* Modbus两次发送数据的最小间隔, 实测75ms+ */
#define MB_STANDARD_CMD_INTERVAL    80    /* Modbus发送命令标准间隔，用在传大数据帧场景下，如升级传包 */

#define MB_RECV_USE_DEFAULT_TMOUT            0
#define MB_RETRY_TIMES                        3       /* Modbus命令最大重试次数 */

extern const char* uart_to_device(uint8_t uart);
extern uint16_t modbus_calcu_crc16(uint8_t* buffer, uint32_t buffer_size);
extern modbus_t* modbus_new_rtu(uint8_t uart_chan, uint32_t baud);
extern void modbus_free(modbus_t *ctx);
extern int modbus_connect(modbus_t* ctx);
extern void modbus_close(modbus_t* ctx);
extern void modbus_set_slave(modbus_t* ctx, int slave_addr);
extern int modbus_report_slave_id(modbus_t* ctx, uint8_t* dest, uint32_t dest_len);
extern int modbus_read_registers(modbus_t* ctx, int addr, int nb, uint16_t* dest);
extern int modbus_read_input_registers(modbus_t* ctx, int addr, int nb, uint16_t* dest);
extern int modbus_read_bits(modbus_t* ctx, int addr, int nb, uint8_t* dest);
int modbus_read_input_bits(modbus_t* ctx, int addr, int nb, uint8_t* dest);
int modbus_write_bit(modbus_t* ctx, int addr, int status);
int modbus_write_register(modbus_t* ctx, int addr, int value, int timeout_ms);

int modbus_send_command(MB_CMD_REQ_S* mb_req, MB_CMD_RESP_S* mb_rsp);

gint32 modbus_get_debug_state(guchar uart_ch, guchar* debug_state);
gint32 modbus_set_debug_state(guchar uart_ch, guchar debug_state);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __MODBUS_CUST_H__ */
