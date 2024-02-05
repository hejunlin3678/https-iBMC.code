/******************************************************************************

                  版权所有 (C), 2012-2013, 华为技术有限公司

 ******************************************************************************
  文 件 名   :
  版 本 号   : 初稿
  部    门   :
  作    者   :
  生成日期   :
  最近修改   :
  功能描述   :
  函数列表   :
******************************************************************************/
#ifndef __NVME_VPD_COMMON_H__
#define __NVME_VPD_COMMON_H__

#include <glib.h>
#ifdef __cplusplus
#if __cplusplus
    extern "C"{
#endif
#endif /* __cplusplus */

#define NVME_VPD_PROTOCOL_NVME_MI			0
#define NVME_VPD_PROTOCOL_SSD_FORM_FACTOR	1
#define SAMSUNG_NVME_VPD_PROTOCOL_SSD_FORM_FACTOR	2
#define NVME_VPD_PROTOCOL_UNKNOWN			0xff
#define NVME_VPD_VENDOR_ID_FOR_SAMSUNG        0x144d
#define NVME_VPD_VENDOR_ID_FOR_SAMSUNG_PM1733  0x144d0001
#define NVME_VPD_VENDOR_ID_FOR_SAMSUNG_PM983   0x144d0002
#define NVME_VPD_VENDOR_ID_FOR_MEMBLAZE_D520   0x1c5f0001
#define NVME_VPD_VENDOR_ID_FOR_MEMBLAZE_D920   0x1c5f0002
#define NVME_VPD_VENDOR_ID_FOR_MEMBLAZE_D526   0x1c5f0003
#define NVME_VPD_VENDOR_ID_FOR_MEMBLAZE_D926   0x1c5f0004
#define NVME_VPD_VENDOR_ID_FOR_MEMBLAZE_D6530_D6536  0x1c5f0005
#define NVME_VPD_VENDOR_ID_FOR_DERA_D5427_D5447      0x1d780001
#define NVME_VPD_PROTOCOL_SSD_FORM_VENDOR_ID_OFFSET_L        3
#define NVME_VPD_PROTOCOL_SSD_FORM_VENDOR_ID_OFFSET_H        4

#define NVME_VPD_VENDOR_ID_FOR_INTEL        0x8086
 /* BEGIN:Add by zwx566676,2015/5/28, 问题单号: DTS2018052503059 */
#define NVME_VPD_VENDOR_ID_FOR_INTEL_P4600    0x80860001
#define NVME_VPD_VENDOR_ID_FOR_INTEL_P5800X   0x80860002
#define NVME_VPD_VENDOR_ID_FOR_INTEL_P5620   0x80860B60
#define NVME_VPD_VENDOR_ID_FOR_SAMSUNG_PM1733  0x144d0001
#define NVME_VPD_VENDOR_ID_FOR_SAMSUNG_PM983   0x144d0002
#define NVME_VPD_VENDOR_ID_FOR_SAMSUNG_PM1735  0x144d0003
#define NVME_VPD_VENDOR_ID_FOR_SAMSUNG_PM1725  0x144d0004
#define NVME_VPD_VENDOR_ID_FOR_SAMSUNG_PM9A3   0x144d0005
#define NVME_VPD_VENDOR_ID_FOR_DAPUSTOR_P5100_P5300 0x1e3b0030
/* END:Add by zwx566676,2015/5/28 */
#define NVME_VPD_VENDOR_ID_FOR_HUAWEI       0x19e5
/* BEGIN:Add by lwx469453,2019/6/03, PN: UADP260528 */
#define NVME_VPD_VENDOR_ID_FOR_UMIS         0x1cc4
#define NVME_VPD_VENDOR_ID_FOR_MEMBLAZE     0x1c5f
#define NVME_VPD_VENDOR_ID_FOR_SHANNON     0x1cb0
#define NVME_VPD_VENDOR_ID_FOR_MICRON       0x1344
#define NVME_VPD_VENDOR_ID_FOR_WESTDIGI    0x1b96
#define NVME_VPD_VENDOR_ID_FOR_RAMAXEL     0x1e81
#define NVME_VPD_VENDOR_ID_FOR_DERA        0x1d78
#define NVME_VPD_VENDOR_ID_FOR_XFUSION        0x1f24
#define NVME_VPD_VENDOR_ID_FOR_DAPUSTOR    0x1e3b
#define NVME_VPD_VENDOR_ID_FOR_STARBLZ     0x9d32
#define NVME_VPD_VENDOR_ID_FOR_KIOXIA       0x1e0f
#define NVME_VPD_VENDOR_ID_FOR_YMTC         0x1e49
#define NVME_MANUFACTURER_YMTC              "YMTC"
#define NVME_MANUFACTURER_INTEL             "INTEL"
#define NVME_MANUFACTURER_HUAWEI            "HUAWEI"			 
#define NVME_MANUFACTURER_SAMSUNG           "SAMSUNG"
#define NVME_MANUFACTURER_MEMBLAZE          "MEMBLAZE"
#define NVME_MANUFACTURER_SHANNON           "SHANNON"
#define NVME_MANUFACTURER_MICRON            "MICRON"
#define NVME_MANUFACTURER_WESTDIGI          "WESTDIGI"
#define NVME_MANUFACTURER_RAMAXEL           "RAMAXEL"
#define NVME_MANUFACTURER_DERA              "DERA"
#define NVME_MANUFACTURER_DAPUSTOR          "DAPUSTOR"
#define NVME_MANUFACTURER_UMIS              "UMIS"
#define NVME_MANUFACTURER_KIOXIA            "KIOXIA"
#define NVME_MANUFACTURER_STARBLZ           "STARBLZ"
#define NVME_MANUFACTURER_XFUSION           "XFUSION"
/* END:Add by lwx469453,2019/6/03 */

#define NVME_VPD_PRODUCT_INFO_MAX_LEN       64
#define NVME_VPD_SN_LEN                  	20
#define NVME_VPD_MODEL_NUMBER_LEN        	40
#define NVME_VPD_MANUFACTURER_LEN			20
 /* BEGIN:Add by zwx566676,2015/5/28, 问题单号: DTS2018052503059 */
#define NVME_VPD_PRODUCT_NAME_LEN           24
/* END:Add by zwx566676,2015/5/28 */
#define NVME_VPD_VENDOR_ID_LEN           	2
#define NVME_VPD_LINK_SPEED_LEN          	1
#define NVME_VPD_LINK_WIDTH_LEN     	    1

#define NVME_VPD_STATE_INVALID				0xffffffff
#define NVME_VPD_STATE_ERROR				0x1
/* BEGIN:Add by c00346034,2018/09/01, PN: DTS2018080603135 */
#define NVME_INVALID_VPD_VENDORID                0x00
/* END: Add by c00346034,2018/09/01,  PN: DTS2018080603135 */

#define NVME_PHYSICAL_TYPE_DISK            0
#define NVME_PHYSICAL_TYPE_CARD            1

#define SSD_FORM_CAPACITY_SAMSUNG_PM983_960   0xDF84758000
#define SSD_FORM_CAPACITY_SAMSUNG_PM983_1T9   0x1BF08EB0000
#define SSD_FORM_CAPACITY_SAMSUNG_PM983_3T8   0x37E11D60000
#define SSD_FORM_CAPACITY_SAMSUNG_PM983_7T6   0x6FC23AC0000
/* BEGIN:Add by cwx1191422,2023/04/21 */
#define SSD_FORM_CAPACITY_SAMSUNG_PM1725b_1T6   0x1999999999A
#define SSD_FORM_CAPACITY_SAMSUNG_PM1725b_3T2   0x33333333334

#define SSD_FORM_CAPACITY_YMTC_PE310_1T6    0x174876e8000
#define SSD_FORM_CAPACITY_YMTC_PE310_3T2    0x2e90edd0000
#define SSD_FORM_CAPACITY_YMTC_PE310_3T8    0x37E11D60000
#define SSD_FORM_CAPACITY_YMTC_PE310_1T9    0x1BF08EB0000
#define SSD_FORM_CAPACITY_YMTC_PE310_7T6    0x6FC23AC0000
#define SSD_FORM_CAPACITY_YMTC_PE310_6T4    0x5D21DBA0000
/* END:Add by cwx1191422,2023/04/21 */

#pragma pack(1)

typedef struct _info_state
{
    guint32 sn_state:1;
    guint32 ppmn_state:1;
    guint32 manu_state:1;
    guint32 vid_state:1;
    guint32 lsp_state:1;
    guint32 lwd_state:1;
    /* BEGIN:Add by zwx566676,2015/5/28, 问题单号: DTS2018052503059 */
    guint32 product_state:1;
    /* END:Add by zwx566676,2015/5/28 */
	guint32 capacity_state:1;
    guint32 reserved:24;
}NVME_VPD_INFO_STATE_ST;

typedef union _vpd_state
{
    guint32 total_state;
    NVME_VPD_INFO_STATE_ST state;
}NVME_VPD_STATE_UN;

typedef struct _nvme_info
{
    guint8 SN[NVME_VPD_SN_LEN + 1];
    guint8 model_number[NVME_VPD_MODEL_NUMBER_LEN + 1];
    guint8 Manufacturer[NVME_VPD_MANUFACTURER_LEN + 1];
    /* BEGIN:Add by zwx566676,2015/5/28, 问题单号: DTS2018052503059 */
    guint8 product_name[NVME_VPD_PRODUCT_NAME_LEN + 1];
    /* END:Add by zwx566676,2015/5/28 */
    guint32 vendor_id;
    double link_speed;
    guint8 link_width;
	guint64 capacity_bytes;
    NVME_VPD_STATE_UN vpd_state;		//记录各个寄存器位读取状态，异常则上层不刷新
} NVME_VPD_INFO;

/* BEGIN:Add by lwx469453,2019/6/15, PN: UADP260528 */
typedef struct nvme_disk_vendor_info_s {
    guint32 vendor_id;
    const gchar *manufacturer;
} NVME_DISK_VENDOR_INFO_S;
/* END:Add by lwx469453,2019/6/15 */
typedef struct nvme_mn_info_s {
    const gchar *model_number;
    guint32 vendor_id;
    guint32 special_vendor_id;
}NVME_MN_INFO_S;

typedef struct _ssd_form_capacity_info_s {
    const gchar *model_number;
    guint64 capacity;
}SSD_FORM_CAPACITY_INFO_S;

typedef struct nvme_policy_info_s {
    const gchar *identifier;
    guint32 vendor_id;
    guint32 special_vendor_id;
}NVME_POLICY_INFO_S;
 
#pragma pack()

extern void nvme_vpd_get_info(const gchar* vpd_chip_name, guint8 protocol, NVME_VPD_INFO* nvme_vpd_info,guint8 physical_type);
gint32 get_peci_hdd_thermal_id(const gchar *manufactur, const gchar *model, guint32 *identifier);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __NVME_VPD_COMMON_H__ */


