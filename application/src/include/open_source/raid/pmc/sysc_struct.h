/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: 数据结构体定义
 * Author: huangleisheng 00475287
 * Create: 2021年3月1日
 */

#ifndef _SC_STRUCT_H_
#define _SC_STRUCT_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MAX_PHYSICAL_DEVICE_NUM   256
#define MAX_PHYSICAL_DRIVE_NUM    238
#define MAX_LOGICAL_DRIVE_NUM     64
#define MAX_PD_NUM_OF_PER_LD      128
#define MAX_ARRAY_NUM             64
#define MAX_LD_REF_ARRAY_NUM      16
#define MAX_SPARE_PD_NUM          32

#pragma pack(push)
#pragma pack(1)
typedef struct _SC_PD_REF
{
    SA_UINT16     deviceId;               //Device Id
} SC_PD_REF;

typedef struct _SC_ENCL_REF
{
    SA_UINT16     deviceId;              //Enclosure device Id
} SC_ENCL_REF;

typedef struct _SC_LD_REF
{
    SA_UINT16     targetId;               //Unique sparse logical drive Id.
} SC_LD_REF;

typedef struct _SC_ARRAY_REF
{
    SA_UINT16     ref;                    //Array Reference Number
} SC_ARRAY_REF;

typedef struct _SC_LIB_CMD_PARAM_T
{
    SA_UINT8      cmdType;
    SA_UINT8      cmd;
    SA_UINT8	  AppsSupportEXTConfig; //This suggests that the Apps have the 240 headers packaged. This will differentiate between the old and new apps and for Backward compatibility.
    SA_UINT8      reserved0;
    SA_UINT32     ctrlId;          //Controller number
    union   //Address to identify devices
    {
        SC_PD_REF           pdRef;
        SC_ENCL_REF         enclRef;
        SC_ARRAY_REF        arrayRef;
        SC_LD_REF           ldRef;
    };
    union
    {
        SA_UINT8      cmdParam_1b[8];    //1 byte usage
        SA_UINT16     cmdParam_2b[4];    //2 bytes usage
        SA_UINT32     cmdParam_4b[2];    //4 bytes usage
        SA_UINT64     cmdParam_8b;       //8 bytes usage
        void    *pCmdParam;        //ptr usage
    };
    SA_UINT32     reserved1;
    SA_UINT32     timeout;    // us
    SA_UINT32     dataSize;       //Length of the buffer passed in pData below.
                            //For receive data, dataSize will be updated with size of data returned
    void    *pData;         //Pointer to buffer to send/receive data
} SC_LIB_CMD_PARAM_T;

#define SC_LIB_CMD_PARAM_S (sizeof(SC_LIB_CMD_PARAM_T) - sizeof(void*))

typedef struct _SC_CTRL_OPERATIONS {
    SA_UINT32 support_raid0 : 1;
    SA_UINT32 support_raid1 : 1;
    SA_UINT32 support_raid10 : 1;
    SA_UINT32 support_raid5 : 1;
    SA_UINT32 support_raid50 : 1;
    SA_UINT32 support_raid6 : 1;
    SA_UINT32 support_raid60 : 1;
    SA_UINT32 support_raid1ADM : 1;
    SA_UINT32 support_raid10ADM : 1;
    SA_UINT32 support_raid1triple : 1;
    SA_UINT32 support_raid10triple : 1;
    SA_UINT32 reserved : 21;
} SC_CTRL_OPERATIONS;

typedef struct _SC_CTRL_PROPERTIES {
    SA_UINT32 min_strip_size_bytes;
    SA_UINT32 max_strip_size_bytes;
    SA_UINT8 configured_drive_wcp;
    SA_UINT8 unconfigured_drive_wcp;
    SA_UINT8 hba_drive_wcp;
} SC_CTRL_PROPERTIES;

typedef struct _tag_sc_ctrl_wcp {
    SA_UINT8 type;
    SA_UINT8 policy;
} SC_DRIVE_WCP_INFO;

typedef struct _SC_CRTL_INFO {
    SA_UINT8 spare_activation_mode; // spare activation mode
    SA_UINT8 pcie_link_width;
    SA_UINT8 nobattery_write_cache;
    SA_UINT8 read_cache_percent;
    SA_UINT8 write_cache_policy;
    SA_UINT8 max_phy_rate;
    SA_UINT16 memorySize;
    SA_UINT16 flashSize;
    SA_UINT16 totalCacheSize;
    SA_UINT16 mode_info;
    SA_UINT32 cur_temp_reading;
    SC_CTRL_OPERATIONS operations;
    SC_CTRL_PROPERTIES properties;
    char familyID[kMinGetControllerFamilyIDBufferLength];
    char productID[kMinGetControllerProductIDBufferLength];
    char fwVersion[kMinGetControllerFirmwareVersionBufferLength];
    char wwn[kMinGetControllerWWNBufferLength];
    char sn[kMinGetControllerSerialNumberBufferLength];
    char pci_location[kMinGetControllerPCILocationBufferLength];
    char drive_version[kMinGetControllerDriverVersionBufferLength];
    char hardware_revision[kMinGetControllerHardwareRevisionBufferLength];
} SC_CTRL_INFO;

typedef struct _SC_PD_LIST {
    SA_UINT32 count;
    SA_UINT16 pd_number[MAX_PHYSICAL_DRIVE_NUM];
    SA_UINT8 slot_number[MAX_PHYSICAL_DRIVE_NUM];
    SA_UINT16 encl_number[MAX_PHYSICAL_DRIVE_NUM];
} SC_PD_LIST;

typedef struct _SC_PD_INFO {
    SA_UINT8 usage_info; // firmware status
    SA_UINT8 current_temp;
    SA_UINT8 hot_spare;
    SA_UINT8 slot_number;
    SA_UINT8 boot_priority;
    SA_UINT8 reserved[3];
    SA_UINT16 encl_number;
    SA_UINT16 health;
    SA_UINT16 poweron_hours;
    SA_UINT16 utilization;
    SA_UINT32 if_type;
    SA_UINT32 link_rate;
    SA_UINT32 status_info;
    SA_UINT32 rotation_speed;
    SA_UINT64 total_size;
    SA_UINT64 block_size;
    char location[kMinGetPhysicalDeviceLocationCompleteBufferLength];
    char fm_version[kMinGetPhysicalDriveFirmwareVersionBufferLength];
    char vendor[kMinGetPhysicalDriveVendorIDBufferLength];
    char model[kMinGetPhysicalDriveModelBufferLength];
    char sn[kMinGetPhysicalDriveSerialNumberBufferLength];
    char wwid[kMinGetPhysicalDriveWWIDStringLength];
} SC_PD_INFO;

typedef struct _SC_LD_LIST {
    SA_UINT16 count;
    SA_UINT16 ld_number[MAX_LOGICAL_DRIVE_NUM];
} SC_LD_LIST;

typedef struct _SC_EDITABLE_LD_INFO {
    char label[kMinGetLogicalDriveVolumeLabelBufferLength];
    SA_UINT64 capacity;       // byte为单位
    SA_UINT64 strip_size;     // byte为单位
    SA_UINT64 max_resizeable_size; // 最大可设置的容量，byte为单位
    SA_UINT8 raid_level;
    SA_UINT8 parity_group_count; // 50/60为非0
    SA_UINT8 accelerator;
    /* 以下为maxCache独有 */
    SA_UINT16 data_ld_number; // 关联的普通逻辑盘id，只有一个
    SA_UINT8 lu_write_policy; // maxCache的写策略
    SA_UINT8 cache_line_size; // 影响maxCache的性能和最大支持大小
} SC_EDITABLE_LD_INFO;

typedef struct _SC_LD_INFO {
    SC_EDITABLE_LD_INFO editable_info;
    SA_UINT8 type;
    SA_UINT8 boot_priority;
    SA_UINT8 rebuild_progress;
    SA_UINT32 status;
    SA_UINT16 parity_init_type;
    SA_UINT16 array_count;
    SA_UINT16 ref_array[MAX_LD_REF_ARRAY_NUM];
    SA_UINT8 spare_count;
    SA_UINT16 spare_pd_ids[MAX_SPARE_PD_NUM];
    SA_UINT16 spare_pd_eids[MAX_SPARE_PD_NUM];
    SA_UINT8 spare_pd_sids[MAX_SPARE_PD_NUM];
} SC_LD_INFO;

typedef struct _SC_PD_IN_LD
{
    SA_UINT16 count;
    SA_UINT16 pd_number[MAX_PD_NUM_OF_PER_LD];
} SC_PD_IN_LD;

typedef struct _SC_CREATE_LD_INFO
{
    SA_BOOL is_max_cache;
    SA_UINT32 pd_count;
    SA_UINT16 pd_number[MAX_PHYSICAL_DRIVE_NUM];
    SA_UINT16 relate_array_number;
    SA_UINT16 new_ld_number;      // output
    SA_UINT8 init_method;
    SC_EDITABLE_LD_INFO editable_info;
} SC_CREATE_LD_INFO;

typedef struct _SC_SET_LD_PROP
{
    SA_UINT32 setting_options;
    SC_EDITABLE_LD_INFO editable_info;
} SC_SET_LD_PROP;

typedef struct _SC_ARRAY_LIST {
    SA_UINT16 count;
    SA_UINT16 array_number[MAX_ARRAY_NUM];
} SC_ARRAY_LIST;

typedef struct _SC_ARRAY_INFO {
    SA_UINT32 pd_count;
    SA_UINT16 pd_number[MAX_PHYSICAL_DRIVE_NUM];
    SA_UINT16 pd_encl[MAX_PHYSICAL_DRIVE_NUM];
    SA_UINT8 pd_slot[MAX_PHYSICAL_DRIVE_NUM];
    SA_UINT16 ld_count;
    SA_UINT16 ld_number[MAX_LOGICAL_DRIVE_NUM];
    SA_UINT64 used_size;
    SA_UINT64 free_size;
} SC_ARRAY_INFO;

typedef struct _SC_ADD_SPARE_INFO {
    SA_BYTE spare_type;
    SA_UINT8 ld_count;
    SA_WORD ld_number[MAX_LOGICAL_DRIVE_NUM];
} SC_ADD_SPARE_INFO;

typedef struct _SC_BBU_INFO {
    SA_UINT16 status;
    SA_UINT8 type;
} SC_BBU_INFO;

typedef struct {
    SA_UINT8 log_type;
    SA_UINT8* log_buffer;
    SA_DWORD log_length;
} SC_CTRL_LOG_DATA;

#pragma pack(pop)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif