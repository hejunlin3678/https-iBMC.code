/******************************************************************************

                  版权所有 (C), 2001-2013, 华为技术有限公司

******************************************************************************
  文 件 名   : dfx_api.h
  版 本 号   : 初稿
  作    者   : l00354559
  生成日期   : 2016年12月10日
  最近修改   :
  功能描述   : hi1710平台SDK用户程序接口
  函数列表   :
  修改历史   :
  1.日    期   : 2016年12月10日
    作    者   : l00354559
    修改内容   : 创建文件

******************************************************************************/

#ifndef _DFX_API_H_
#define _DFX_API_H_

#include "glib.h"


#define MAX_GMAC_NUM 4
#define MAX_USB_NUM 4
#define MAX_FORMAT_SIZE 4096

enum ECC_RESULT
{
	ECC_OK = 0,              /*没有错误*/
	ECC_SINGLE_BIT_ERROR,  /*单比特错误*/
	ECC_MULTI_BIT_ERROR,   /*多比特错误*/
	ECC_NONE
};

enum ECC_ERR_RESULT
{
	ECC_ERR_OK = 0,              /*没有错误*/
	ECC_ERR_SINGLE_BIT_ERROR = 0x1,  /*单比特错误*/
	ECC_ERR_MULTI_BIT_ERROR =0x2,   /*多比特错误*/
	ECC_ERR_SINGLE_MULTI_BIT_ERROR = (ECC_ERR_SINGLE_BIT_ERROR)|(ECC_ERR_MULTI_BIT_ERROR),  /* 单比特多比特错误并存*/
	ECC_ERR_NONE
};

enum PLL_LOCK_STATUS
{
    PLL_UNLOCKED = 0,
    PLL_LOCKED = 1,
    PLL_NONE
};

#define PCIE_LINKUP 0x11

enum SERDES_READY_STATUS
{
    SERDES_UNREADY = 0,
    SERDES_READY = 1,
    SERDES_NONE
};

enum SERDES_PLL_LOCK_STATUS
{
    SERDES_PLL_UNLOCKED = 0,
    SERDES_PLL_LOCKED = 1,
    SERDES_PLL_NONE
};

enum BMC_RESET_TYPE
{
    CHIP_RESET = 0,
    BMC_RESET = 1,
    BMC_RESET_TYPE_NONE
};

enum HOST_RESET_STATUS
{
    HOST_RESETING = 0,
    HOST_WORKING = 1,
    HOST_RESET_STATUS_NONE
};

enum BMC_LAST_RESET_TYPE
{
    RESET_TYPE_POWER_UP = 0x1,
    RESET_TYPE_EXT_WDT_BUTTON = 0x2,
    RESET_TYPE_INNER_WDT = 0x4,
    RESET_TYPE_NONE
};

enum DDR_ECC_CHECK //指示DDR ECC功能是否开启
{
    DDR_ECC_DISABLED = 0,
    DDR_ECC_ENABLED = 1,
    DDR_ECC_CHECK_NONE
};

enum DDRC_INIT_CHECK //DDRC初始化状态；
{
    DDRC_INIT_WITHOUT_ECC = 0,
    DDRC_INIT_WITH_ECC = 1,
    DDRC_INIT_NONE
};

enum DDRC_SR_CHECK //DDRC自刷新状态检测
{
    DDRC_SR_STOP = 0,
    DDRC_SR_WORKING = 1,
    DDRC_SR_NONE
};

enum USB_DEVICE_ERR
{
    USB_DEVICE_WITHOUT_ERRTICERR = 0,
    USB_DEVICE_ERRTICERR = 1,
    USB_DEVICE_ERRTICERR_NONE
};

enum USB_HOST_OCA_CHECK
{
    USB_HOST_WITHOUT_OCA = 0,
    USB_HOST_OCA = 1,
    USB_HOST_OCA_CHECK_NONE
};

enum VCE_ENVC_VSYNC_CHECK
{
    VCE_ENVC_VSYNC_OK = 0,
    VCE_ENVC_VSYNC_LOST = 1,
    VCE_ENVC_VSYNC_NONE
};

typedef struct tag_dfx_ecc
{
    unsigned int orig_value;    /*ecc注入的原始数据*/
    unsigned int actual_value; /*ecc注入的实际数据*/
    unsigned int result;        /* ecc注入结果，enum ECC_RESULT*/
}DFX_ECC_S;

typedef struct tag_dfx_ecc_status
{    
    unsigned int ecc_status;        /* ecc状态，enum ECC_RESULT*/
    unsigned int rsv;
}DFX_ECC_STATUS_S;

typedef struct tag_dfx_die_id
{
    unsigned int dieid[5];//DIE ID
    unsigned char format;       //格式位bit0~3
    unsigned char tsmc;          //bit 4~7
    unsigned long long lotid;   //bit8~67
    unsigned char wagerid;    //bit68~72
    unsigned int diexy;           //bit 73~88
    unsigned int test_date;         //bit89~104
    unsigned char m_repair;  //bit105
    unsigned int iddq;             //bit106~121
    unsigned char crc;            //bit122~125
    unsigned int fass_flag;     //bit126~135
    unsigned int rsv;               //bit136~154
} DFX_DIE_ID_S;

typedef union tag_pll_lock_status   //PLL锁定状态
{
    unsigned int value;
    struct
    {
#ifdef CONFIG_CPU_BIG_ENDIAN
        unsigned int reserved : 28;
        unsigned int pll2_lock : 1; //pll2_lock状态。0未锁定；1锁定
        unsigned int pll1_lock : 1; //pll1_lock状态。0未锁定；1锁定
        unsigned int pll0_lock : 1; //pll0_lock状态。0未锁定；1锁定
        unsigned int cpll_lock : 1; //cpll_lock状态。0未锁定；1锁定
#else
        unsigned int cpll_lock : 1; //cpll_lock状态。0未锁定；1锁定
        unsigned int pll0_lock : 1; //pll0_lock状态。0未锁定；1锁定
        unsigned int pll1_lock : 1; //pll1_lock状态。0未锁定；1锁定
        unsigned int pll2_lock : 1; //pll2_lock状态。0未锁定；1锁定
        unsigned int reserved : 28;
#endif
    } reg;
}DFX_PLL_LOCK_STATUS_U;

typedef union tag_cache_err //A9 L1 cache错误状态监测
{
    unsigned int value;
    struct
    {
#ifdef CONFIG_CPU_BIG_ENDIAN
        unsigned int CPU_BTAC_parityerror: 4; //bit[28~31] CPU的BTAC memory奇偶校验错误计数，当计数到4'b1111时，停止计数保持。
        unsigned int CPU_GHB_parityerror: 4; //bit[24~27] CPU的GHB memory奇偶校验错误计数，当计数到4'b1111时，停止计数保持。
        unsigned int CPU_Inst_tagRAM_parityerror: 4; //bit[20~23] CPU的指令tag memory奇偶校验错误计数，当计数到4'b1111时，停止计数保持。
        unsigned int CPU_Inst_dataRAM_parityerror: 4; //bit[16~19] CPU的指令data memory奇偶校验错误计数，当计数到4'b1111时，停止计数保持。
        unsigned int CPU_MainTLB_parityerror: 4; //bit[12~15] CPU的TLB memory奇偶校验错误计数，当计数到4'b1111时，停止计数保持。
        unsigned int CPU_DouterRAM_parityerror: 4; //bit[8~11] CPU的Douter memory奇偶校验错误计数，当计数到4'b1111时，停止计数保持。
        unsigned int CPU_Data_tagRAM_parityerror: 4; //bit[4~7] CPU的数据tag memory奇偶校验错误计数，当计数到4'b1111时，停止计数保持。
        unsigned int CPU_Data_dataRAM_parityerror: 4; //bit[0~3] CPU的数据data memory奇偶校验错误计数，当计数到4'b1111时，停止计数保持。
#else
        unsigned int CPU_Data_dataRAM_parityerror: 4; //bit[0~3] CPU的数据data memory奇偶校验错误计数，当计数到4'b1111时，停止计数保持。
        unsigned int CPU_Data_tagRAM_parityerror: 4; //bit[4~7] CPU的数据tag memory奇偶校验错误计数，当计数到4'b1111时，停止计数保持。
        unsigned int CPU_DouterRAM_parityerror: 4; //bit[8~11] CPU的Douter memory奇偶校验错误计数，当计数到4'b1111时，停止计数保持。
        unsigned int CPU_MainTLB_parityerror: 4; //bit[12~15] CPU的TLB memory奇偶校验错误计数，当计数到4'b1111时，停止计数保持。
        unsigned int CPU_Inst_dataRAM_parityerror: 4; //bit[16~19] CPU的指令data memory奇偶校验错误计数，当计数到4'b1111时，停止计数保持。
        unsigned int CPU_Inst_tagRAM_parityerror: 4; //bit[20~23] CPU的指令tag memory奇偶校验错误计数，当计数到4'b1111时，停止计数保持。
        unsigned int CPU_GHB_parityerror: 4; //bit[24~27] CPU的GHB memory奇偶校验错误计数，当计数到4'b1111时，停止计数保持。
        unsigned int CPU_BTAC_parityerror: 4; //bit[28~31] CPU的BTAC memory奇偶校验错误计数，当计数到4'b1111时，停止计数保持。
#endif
    } reg;
} DFX_CACHE_ERR_U;

typedef union tag_pcie0_status   //PCIE0状态检测
{
    unsigned int value;
    struct 
    {
#ifdef CONFIG_CPU_BIG_ENDIAN
        unsigned int reserved       :  26;
        unsigned int xmlh_ltssm_state: 6; //bit[0~5]指示当前PCIe控制器ltssm状态link up成功的状态是：0x11
#else
        unsigned int xmlh_ltssm_state: 6; //bit[0~5]指示当前PCIe控制器ltssm状态link up成功的状态是：0x11
        unsigned int reserved       :  26;
#endif
    }reg;
}DFX_PCIE0_STATUS_U;


typedef union  tag_pcie1_status  //PCIE1状态检测
{
    unsigned int value;
    struct 
    {
#ifdef CONFIG_CPU_BIG_ENDIAN
        unsigned int reserved       :  26;
        unsigned int xmlh_ltssm_state: 6; //bit[0~5]指示当前PCIe控制器ltssm状态link up成功的状态是：0x11
#else
        unsigned int xmlh_ltssm_state: 6; //bit[0~5]指示当前PCIe控制器ltssm状态link up成功的状态是：0x11
        unsigned int reserved       :  26;
#endif
    }reg;
}DFX_PCIE1_STATUS_U;

typedef union tag_pcie_serdes_lock   //PCIE Serdes锁定状态监测
{
    unsigned int value;
    struct 
    {
#ifdef CONFIG_CPU_BIG_ENDIAN
        unsigned int psds_c_ready: 1; //SERDES 操作准备完成指示信号。0：操作准备未完成；1：操作准备已完成。
        unsigned int psds_c_cmulock: 1; //SERDES PLL 锁定完成指示信号。0：PLL 锁定未完成；1：PLL 锁定已完成。
#else
        unsigned int psds_c_cmulock: 1; //SERDES PLL 锁定完成指示信号。0：PLL 锁定未完成；1：PLL 锁定已完成。
        unsigned int psds_c_ready: 1; //SERDES 操作准备完成指示信号。0：操作准备未完成；1：操作准备已完成。
#endif
    } reg;
}DFX_PCIE_SERDES_LOCK_U;

typedef union tag_gmac_serdes_lock   //GMAC Serdes锁定状态监测
{
    unsigned int value;
    struct 
    {
#ifdef CONFIG_CPU_BIG_ENDIAN
        unsigned int gsds_c_ready: 1; //SERDES 操作准备完成指示信号。0：操作准备未完成；1：操作准备已完成。
        unsigned int gsds_c_cmulock: 1; //SERDES PLL 锁定完成指示信号。0：PLL 锁定未完成；1：PLL 锁定已完成。
#else
        unsigned int gsds_c_cmulock: 1; //SERDES PLL 锁定完成指示信号。0：PLL 锁定未完成；1：PLL 锁定已完成。
        unsigned int gsds_c_ready: 1; //SERDES 操作准备完成指示信号。0：操作准备未完成；1：操作准备已完成。
#endif
    } reg;
}DFX_GMAC_SERDES_LOCK_U;

typedef union tag_reset_info  //复位只是和统计
{
    unsigned int value;
    struct 
    {
#ifdef CONFIG_CPU_BIG_ENDIAN
        unsigned int bmc_reset_count: 8;//bit[24~31] BMC复位次数计数器
        unsigned int host_reset_count: 8;//bit[16~23] HOST复位次数计数器
        unsigned int chip_reset_count: 8;//bit[8~15] 整芯片复位次数计数器
        unsigned int reserved: 3;//bit[5~7]
        unsigned int bmc_reset_status: 1;//bit[4] BMC最后一次复位状态，0:chip reset ,1:bmc reset
        unsigned int host_reset_status: 1; //bit[3] HOST复位状态,0:HOST处于复位状态，1：HOST处于工作状态，撤离复位
        unsigned int last_reset_st: 3; //bit[0~2] 指示最近一次的触发复位的源头。001：最近一次为上电复位 010：最近一次为外部按键或外部看门狗复位 100：最近一次为内部总线看门狗复位
#else
        unsigned int last_reset_st: 3; //bit[0~2] 指示最近一次的触发复位的源头。001：最近一次为上电复位 010：最近一次为外部按键或外部看门狗复位 100：最近一次为内部总线看门狗复位
        unsigned int host_reset_status: 1; //bit[3] HOST复位状态,0:HOST处于复位状态，1：HOST处于工作状态，撤离复位
        unsigned int bmc_reset_status: 1;//bit[4] BMC最后一次复位状态，0:chip reset ,1:bmc reset
        unsigned int reserved: 3;//bit[5~7]
        unsigned int chip_reset_count: 8;//bit[8~15] 整芯片复位次数计数器
        unsigned int host_reset_count: 8;//bit[16~23] HOST复位次数计数器
        unsigned int bmc_reset_count: 8;//bit[24~31] BMC复位次数计数器
#endif
    } reg;
}DFX_RESET_INFO_U;

typedef union tag_ddr_ecc_enable   //DDR ECC功能使能与否检测
{
    unsigned int value;
    struct 
    {
#ifdef CONFIG_CPU_BIG_ENDIAN
        unsigned int reserved: 30;
        unsigned int ecc_en: 1; //控制器ECC使能。0：禁止；1：使能。
        unsigned int unused0: 1;

#else
        unsigned int unused0: 1;
        unsigned int ecc_en: 1; //控制器ECC使能。0：禁止；1：使能。
        unsigned int reserved: 30;
#endif
    } reg;
}DFX_DDR_ECC_ENABLE_U;

typedef union tag_ddr_ecc_err_count   //DDR ECC错误统计
{
    unsigned int value;
    struct 
    {
#ifdef CONFIG_CPU_BIG_ENDIAN
        unsigned int multi_err: 16; //bit[16~31] 多bit数据错误统计值。
        unsigned int single_err: 16;//bit[0~15] 单bit数据错误统计值。
#else
        unsigned int single_err: 16;//bit[0~15] 单bit数据错误统计值。
        unsigned int multi_err: 16; //bit[16~31] 多bit数据错误统计值。
#endif
    } reg;
}DFX_DDR_ECC_ERR_COUNT_U;

typedef union tag_ddr_ecc_err_status   //DDR ECC 实时错误信息
{
    unsigned int value;
    struct 
    {
#ifdef CONFIG_CPU_BIG_ENDIAN
        unsigned int reserved: 24;//bit[8~31]
        unsigned int int_st: 4;//bit[4~7] DDRC状态寄存器
        unsigned int in_init: 1;//bit[3] 控制器初始化状态
        unsigned int in_sr: 1;//bit[2] 控制器自刷新状态
        unsigned int unused0: 2;//bit[0~1]
#else
        unsigned int unused0: 2;//bit[0~1]
        unsigned int in_sr: 1;//bit[2] 控制器自刷新状态
        unsigned int in_init: 1;//bit[3] 控制器初始化状态
        unsigned int int_st: 4;//bit[4~7] DDRC状态寄存器
        unsigned int reserved: 24;//bit[8~31]
#endif
    } reg;
}DFX_DDR_ECC_ERR_STATUS_U;

typedef struct tag_gmac_err_count//GMAC接收发送错误统计;注意此寄存器是读清的
{
    unsigned int id;//GMAC ID号，范围0~3
    unsigned int rx_fcs_errors;//RX_FCS_ERRORS为接收CRC检验错误的帧数统计寄存器。
    unsigned int rx_data_err;//RX_DATA_ERR为接收数据错误帧统计寄存器。
    unsigned int rx_short_err_cnt;//RX_SHORT_ERR_CNT为接收帧长小于short_runts_thr字节的帧数统计寄存器。
    unsigned int tx_excessive_length_drop;//TX_EXCESSIVE_LENGTH_DROP为超过设定的最大帧长导致发送失败次数统计寄存器。
    unsigned int tx_underrun;//TX_UNDERRUN为帧发送过程中发生内部错误而导致发送失败的次数统计。
    unsigned int tx_crc_error;//TX_CRC_ERROR为发送帧长正确CRC错误的帧数统计寄存器。
} DFX_GMAC_ERR_COUNT_S;

typedef struct tag_gmac_short_runts_thr //GMAC short_runts_thr配置
{
    unsigned int id;//GMAC ID号，范围0~3
    union 
    {
        unsigned int value;
        struct
        {
#ifdef CONFIG_CPU_BIG_ENDIAN
            unsigned int reserved: 27;
            unsigned int short_runts_thr: 5;//bit[0~4] 短帧、超短帧界限，用于统计。
#else
            unsigned int short_runts_thr: 5;//bit[0~4] 短帧、超短帧界限，用于统计。
            unsigned int reserved: 27;
#endif
        } reg;
    }short_runts_thr;
} DFX_GMAC_SHORT_RUNTS_THR_S;

typedef struct tag_gmac_max_frm_size //最大帧长配置寄存器
{
    unsigned int id;//GMAC ID号，范围0~3
    union
    {
        unsigned int value;
        struct
        {
#ifdef CONFIG_CPU_BIG_ENDIAN
            unsigned int reserved: 16;
            unsigned int max_frm_size: 16;//bit[0~15] MAC部分允许的最大帧长。
#else
            unsigned int max_frm_size: 16;//bit[0~15] MAC部分允许的最大帧长。
            unsigned int reserved: 16;
#endif
        } reg;
    }max_frm_size_u;
} DFX_GMAC_MAX_FRM_SIZE_S;

typedef struct tag_usb_dsts //USB DEVICE UTMI+上接收到的不确定的错误检测
{
    unsigned int id;//USB ID号，范围0~3
    union
    {
        unsigned int value;
        struct
        {
#ifdef CONFIG_CPU_BIG_ENDIAN
            unsigned int unused1: 28;
            unsigned int errticerr: 1;//bit[3] 用于指示任何UTMI+上接收到的不确定的错误。产生这种错误时，控制器进入suspend状态，并上报中断GINTSTS[ErlySusp]。0：没有UTMI+接口错误；1：有UTMI+接口错误。
            unsigned int unused0: 3;//bit[0~2] 
#else
            unsigned int unused0: 3;//bit[0~2] 
            unsigned int errticerr: 1;//bit[3] 用于指示任何UTMI+上接收到的不确定的错误。产生这种错误时，控制器进入suspend状态，并上报中断GINTSTS[ErlySusp]。0：没有UTMI+接口错误；1：有UTMI+接口错误。
            unsigned int unused1: 28;
#endif
        } reg;
    }dsts;
} DFX_USB_DSTS_S;

typedef union tag_usb_ehci_portsc//USB HOST(EHCI)端口电流过载状态检测
{
    unsigned int value;
    struct
    {
#ifdef CONFIG_CPU_BIG_ENDIAN
        unsigned int unused1: 27;
        unsigned int oca: 1;//bit[4] 过载激活状态。0：端口未出现电流过载现象；1：端口出现电流过载现象。
        unsigned int unused0: 4;//bit[0~3]
#else
        unsigned int unused0: 4;//bit[0~3]
        unsigned int oca: 1;//bit[4] 过载激活状态。0：端口未出现电流过载现象；1：端口出现电流过载现象。
        unsigned int unused1: 27;
#endif
    } reg;
} DFX_USB_EHCI_PORTSC_U;

typedef union tag_vce_envc_frm_err//视频信号丢失检测
{
    unsigned int value;
    struct
    {
#ifdef CONFIG_CPU_BIG_ENDIAN
        unsigned int unused1: 30;
        unsigned int envc_vsync_lost: 1;//bit[1] 视频信号丢失标志，一旦持续160ms没有视频信号压缩，该信号有效
        unsigned int unused0: 1;//bit[0]
#else
        unsigned int unused0: 1;//bit[0]
        unsigned int envc_vsync_lost: 1;//bit[1] 视频信号丢失标志，一旦持续160ms没有视频信号压缩，该信号有效
        unsigned int unused1: 30;
#endif
    } reg;
} DFX_VCE_ENVC_FRM_ERR_U;


/*****************************************************************************
对外接口函数定义, 添加对C++的支持
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */

extern gint32  simulate_ecc_inject(DFX_ECC_S* dfx_ecc);
extern gint32  get_memory_ecc_status(DFX_ECC_STATUS_S* ecc_status);
extern gint32  get_chip_die_id(DFX_DIE_ID_S* die_id);
extern gint32  get_pll_lock_status(DFX_PLL_LOCK_STATUS_U* pll_lock_status);
extern gint32  get_a9l1_cache_err(DFX_CACHE_ERR_U* cache_err);
extern gint32  get_pcie0_link_info(DFX_PCIE0_STATUS_U* pcie0_status);
extern gint32  get_pcie1_link_info(DFX_PCIE1_STATUS_U* pcie1_status);
extern gint32  get_pcie_serdes_lock_info(DFX_PCIE_SERDES_LOCK_U* pcie_serdes_lock);
extern gint32  get_gmac_serdes_lock_info(DFX_GMAC_SERDES_LOCK_U* gmac_serdes_lock);
extern gint32  get_reset_info(DFX_RESET_INFO_U* reset_info);
extern gint32  get_ddr_ecc_enable(DFX_DDR_ECC_ENABLE_U* ddr_ecc_enable);
extern gint32  get_ddr_ecc_err_count(DFX_DDR_ECC_ERR_COUNT_U*  err_count);
extern gint32  get_ddr_ecc_err_status(DFX_DDR_ECC_ERR_STATUS_U*  err_status);
extern gint32  get_gmac_err_count(DFX_GMAC_ERR_COUNT_S*  err_count);
extern gint32  get_gmac_short_runts_thr(DFX_GMAC_SHORT_RUNTS_THR_S*  short_runts_thr);
extern gint32  get_gmac_max_frm_size(DFX_GMAC_MAX_FRM_SIZE_S*  max_frm_size);
extern gint32  get_usb_dsts_info(DFX_USB_DSTS_S*  dsts_info);
extern gint32  get_usb_ehci_portsc(DFX_USB_EHCI_PORTSC_U*  usb_ehci_portsc);
extern gint32  get_vce_envc_frm_err(DFX_VCE_ENVC_FRM_ERR_U*  vce_frm_err);
extern gint32  dfx_dying_saying(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif
