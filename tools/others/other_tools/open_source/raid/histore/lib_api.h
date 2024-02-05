/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: cmd main process
 * Create: 2020-12-9
 * Notes: none
 * History: 2020-11-13 create
 */
#ifndef INCLUDE_OPEN_API_LIB_API_H
#define INCLUDE_OPEN_API_LIB_API_H

// �����漰���ݽṹĬ�ϲ���8�ֽڶ���
#pragma pack(push)
#pragma pack(8)

#ifndef u8
typedef unsigned char u8;
#endif

#ifndef u16
typedef unsigned short u16;
#endif

#ifndef u32
typedef unsigned int u32;
#endif

#ifndef u64
typedef unsigned long long u64;
#endif

#define CMD_TYPE_SET 0x00
#define CMD_TYPE_WRITE 0x01
#define CMD_TYPE_READ 0x02
#define CMD_TYPE_MASK 0x03
#define CMD_MAGIC_FLAG 0xAABB

#define ADM_RAID_CMD_ERR 0xC0
#define USER_CMD_TIMEOUT_MS 600000          // ����������Ҫ����flash��ʱ�ϳ����������������ݶ�10���ӣ���ָ������Ĭ��1���ӳ�ʱ
#define USER_CMD_DEFAULT_TIMEOUT_MS 180000   // lib������Ĭ�ϳ�ʱ������
#define PD_UPGRADE_TIMEOUT_MS 120000         // �����̹̼�������ʱʱ��
#define LIB_PARAM_SIZE 24
#define IOCTL_TIME_OUT 0  // 60s
#define MAX_VERIFY_VD_PRRIOD 60
#define WORD_TO_DWORD 4
#define DISK_UPGRADE_OFFSET 3
#define MIN_VD_NAME_LEN 4

#define FIRMWARE_NAME_LEN 12     /* the max length of firmware name */
#define MAX_MODEL_NAME_LEN 40    /* the max length of model name */
#define MAX_RG_NAME_LEN 32       /* the max length of raid group name */
#define MAX_LUN_NAME_LEN 32      /* the max length of lun name */
#define CMD_MEMB_DISK_MAX_NUM 80U
#define MAX_RG_HOTSPARE_DISK_NUM 8
#define ADM_MAX_DATA_SIZE 0x1000U
#define HOST_MAX_LUN_NAME_LEN 20    // BMC�������չʾlun�������������

#define CMD_MAX_EXP_NUM 70
#define MAX_PHY_NUM 50
#define MAX_INCONSISTENT_STRIPE_NUM 100
#define ADM_MAX_FILE_NAME_LENGTH 255
#define CMD_MAX_FOREIGN_NUM 256
#define MASK_OF_STRUCT_INDEX_FIRST 1 // used for ADM_CMD_SET_CONSIS_CHECK
#define MASK_OF_STRUCT_INDEX_SECOND 2
#define MASK_OF_STRUCT_INDEX_THIRD 4
#define MASK_OF_STRUCT_INDEX_FOURTH 8

#define ADM_SCSI_CDB_MAX_LEN 32
#define ADM_SCSI_CDB_SENSE_MAX_LEN 96
#define ADM_SCSI_CDB_SENSE_LEN32 32
#define SET_CDW_NUM(U32A, U8_ARR)                                                   \
    {                                                                               \
        U32A = U8_ARR[0] << 24 | U8_ARR[1] << 16 | U8_ARR[2] << 8 | U8_ARR[3] << 0; \
    }

#define ADM_RAID_SET 0xc0
#define ADM_RAID_WRITE 0xc1
#define ADM_RAID_READ 0xc2
#define ADM_RAID_PARAM_WRITE 0xc3
#define ADM_RAID_READ_FROM_CQE 0xc4

#define ADM_RAID_INVALID_VALUE16 0xFFFF

#define MAX_ASYNC_EVENT_NUM 4
#define STOR_MAX_ASYNC_EVENT_NUM 4
#define NVME_MAX_ASYNC_EVENT_NUM 1
#define ADM_RAID_INVALID_PROGRESS 0xFF
#define DEV_DIR_IN_LINUX "/dev/"

#define JSON_C_TO_STRING_TAB 10 // ���Եó���ֵ���Դ���ʧЧ���޷��õ���Ҫ���
#define LAST_MAJOR_VERSION      3
#define LAST_SUB_VERSION        0
#define LAST_FULL_VERSION       ((LAST_MAJOR_VERSION << 8) + LAST_SUB_VERSION)
#define GET_MAJOR_VERSION(val)  (((val) >> 8) & 0xff)
#define GET_SUB_VERSION(val)    ((val) & 0xff)
#define GET_BUF_LEN_ALIG8(val)  (((val) & 0x7) ? ((((val) >> 3) << 3) + 8) : (val))
#define MAX_VD_NAME_LEN 24

/* bit offset used for SUBOPCODE:ADM_CMD_SET_PATROLREAD */
enum adm_patrolread_bits {
    PERIOD_BIT,
    RATE_BIT,
    DISK_NUM_BIT,
    DELAY_BIT,
    BUTT_BIT
};

/* bit offset used for ccheck parameter mask */
enum adm_ccheck_bits {
    CCPERIOD_BIT,
    CCSPEED_BIT,
    CCREPAIR_MODE_BIT,
    CCDELAY_BIT,
    CCHECK_BUTT_BIT
};

/* bit offset used for kick slowdisk cycles parameter mask */
enum adm_kick_slowdisks_bits {
    KICK_PERIOD_BIT,
    KICK_SUM_BIT,
    KICK_REFRESH_BIT
};

/*
 * �͵��̽�����ص�bitλ��bit0:�ع���bit1:Ѳ�죬bit2:�ؿ���bit3:����
 * ���cmd_show_disk��action��Ա����
 */
enum adm_disk_progress_bits {
    ADM_DISK_RECONSTRUCTION_BIT,
    ADM_DISK_PATROLREAD_BIT,
    ADM_DISK_COPYBACK_BIT,
    ADM_DISK_SANITIZE_BIT,
    ADM_DISK_BUTT_BIT
};

/* bit offset */
enum task_type_bit_val {
    DISK_TASK_RECONSTRUCTION = 0x1 << ADM_DISK_RECONSTRUCTION_BIT,
    DISK_TASK_PATROLREAD =     0x1 << ADM_DISK_PATROLREAD_BIT,
    DISK_TASK_COPYBACK =       0x1 << ADM_DISK_COPYBACK_BIT,
    DISK_TASK_SANITIZE =       0x1 << ADM_DISK_SANITIZE_BIT,
};

/* set disk online or offline in struct cmd_disk_on_offline */
enum adm_disk_online_offline {
    ADM_DISK_OFFLINE,
    ADM_DISK_ONLINE
};

enum sc_code {
    SC_CHECK_SENSE = 1, /* ����ִ��ʧ�ܣ���������Ҫ�鿴sense buffer */
    SC_FW_ERR,      /* ����ִ��ʧ�� */
    SC_ABT_NOT_FOUND, /* io abortʱ����û���ҵ���Ҫabort��io */
    SC_PERMISSION, /* �����Ѿ����ڱ�������״̬���赲������ظ���Ȩ�� */
    SC_RETRY, /* оƬ���ڸ�λģʽ�У����б��赲����ظ���Ҫ���� */
    SC_LEFT_IO, /* ��������Կػ��в�����û���ſ��� */
    SC_ABORTED_WITHOUT_NCQ,  /* ��Ǳ�abort��IO, ������NCQ TAG���� */
    SC_ABORTED_WITH_NCQ /* ��Ǳ�abort��IO, ����Я��NCQ TAG���� */
};

enum admin_channel {
    ADM_PCIE,
    ADM_MCTP_OVER_PCIE,
    ADM_I2C_SMBUS
};

enum sct_type {
    SCT_RAID = 0x07,
};

enum raid_aen_info {
    TIME_SYNC,
    PHY_EVENT,
    DISK_EVENT,
    CHIP_EVENT
};

enum raid_aen_type {
    AEN_RAID = 0x07,
};

enum adm_cache_del_lun_method {
    ADM_CACHE_METHOD_DEFAULT,
    ADM_CACHE_METHOD_FORCE,
    ADM_CACHE_METHOD_DISCARD_CACHE // ����δ��ˢ��������
};

/* Begin ��raid���ö�ٱ������壬�����޸Ĵ���ʱ����Ҫ���붨�� */
enum user_input_raid_level {
    USER_INPUT_RAID0_LEVEL = 0,
    USER_INPUT_RAID1_LEVEL = 1,
    USER_INPUT_RAID5_LEVEL = 5,
    USER_INPUT_RAID6_LEVEL = 6,
    USER_INPUT_RAID10_LEVEL = 10,
    USER_INPUT_RAID50_LEVEL = 50,
    USER_INPUT_RAID60_LEVEL = 60,
};
enum raid_group_level {
    CMD_RAID_LEVEL_RAID0 = 0,
    CMD_RAID_LEVEL_RAID1,
    CMD_RAID_LEVEL_RAID5,
    CMD_RAID_LEVEL_RAID6,
    CMD_RAID_LEVEL_RAID10,
    CMD_RAID_LEVEL_RAID50,
    CMD_RAID_LEVEL_RAID60,
    CMD_RAID_LEVEL_BUTT,
    CMD_RAID_DEFAULT_VALUE = 255
};
enum adm_raid_status {
    ADM_RAID_STATUS_NORMAL = 1,
    ADM_RAID_STATUS_FAULT,
    ADM_RAID_STATUS_DEGRADE,
    ADM_RAID_STATUS_BUTT
};
enum adm_disk_status {
    ADM_DISK_STATUS_NORMAL = 1,
    ADM_DISK_STATUS_FAULT,
    ADM_DISK_STATUS_RECONSTRUCTING,
    ADM_DISK_STATUS_RECONSTRUCTED,
    ADM_DISK_STATUS_COPYBACKING,
    ADM_DISK_STATUS_FREESPARE,
    ADM_DISK_STATUS_USEDSPARE,
    ADM_DISK_STATUS_PRECOPYING,
    ADM_DISK_STATUS_DIAGNOSING,
    ADM_DISK_STATUS_BUTT
};

enum adm_pd_status {
    ADM_PD_STATUS_ONLINE = 1,
    ADM_PD_STATUS_OFFLINE,
    ADM_PD_STATUS_CFG_FAULT,
    ADM_PD_STATUS_UCFG_GOOD,
    ADM_PD_STATUS_UCFG_FAULT,
    ADM_PD_STATUS_HOT_SPARE,
    ADM_PD_STATUS_FOREIGN,
    ADM_PD_STATUS_DIAGNOSING,
    ADM_PD_STATUS_RECONSTRUCTING,
    ADM_PD_STATUS_COPYBACK,
    ADM_PD_STATUS_INCOMPAT,
    ADM_PD_STATUS_UNSUPPORTE,
    ADM_PD_STATUS_UNKNOWN,
    ADM_PD_STATUS_BUTT,
};

enum adm_cut_capacity {
    ADM_MIN_CC = 512,
    ADM_MAX_CC = 1024
};
enum adm_susize {
    ADM_SUSZ_DEFAULT = 0,
    ADM_SUSZ_32 = 32,
    ADM_SUSZ_64 = 64,
    ADM_SUSZ_128 = 128,
    ADM_SUSZ_256 = 256,
    ADM_SUSZ_512 = 512,
    ADM_SUSZ_1024 = 1024
};
enum adm_write_type {
    ADM_WB = 1,
    ADM_WT,
    ADM_WBE
};
enum adm_set_vd_attr_type {
    VD_ATTR_WCACHE,
    VD_ATTR_RCACHE,
    VD_ATTR_NAME,
    VD_ATTR_MAP_TYPE,
    VD_ATTR_PF,
    VD_ATTR_ACCESS
};
enum adm_read_type {
    ADM_NO_AHEAD,
    ADM_AHEAD
};
enum adm_init_type {
    ADM_INIT_FAST,
    ADM_INIT_BACKGROUND,
    ADM_INIT_FRONT
};

enum adm_disk_cahe_switch {
    ADM_DISK_CAHE_SWITCH_OFF,
    ADM_DISK_CAHE_SWITCH_ON,
    ADM_DISK_CAHE_DEFAULT
};

enum adm_disk_delete_sanitize {
    ADM_DELETE_LUN_SANITIZE_OFF,
    ADM_DELETE_LUN_SANITIZE_ON
};

enum adm_lun_freq {
    ADM_LUN_FREQ_COLD = 0,
    ADM_LUN_FREQ_HOT,
    ADM_LUN_FREQ_NORMAL,
    ADM_LUN_FREQ_BUTT
};

enum adm_map_type {
    ADM_RAID_SCSI,
    ADM_RAID_NVME
};

enum adm_lun_status {
    ADM_LUN_STATUS_NORMAL = 1, /* ���� */
    ADM_LUN_STATUS_FAULT = 2,  /* ���� */
    ADM_LUN_STATUS_DEGRADE = 3,
    ADM_LUN_STATUS_NOTFORMATTED = 4,   /* δ��ʽ�� */
    ADM_LUN_STATUS_FORMATTING = 5,     /* ���ڸ�ʽ�� */
    ADM_LUN_STATUS_SANITIZING = 6,     /* ���ڽ����������� */
    ADM_LUN_STATUS_INITIALIZING = 7,   /* ThinLun��ʼ��״̬ */
    ADM_LUN_STATUS_INITIALIZEFAIL = 8, /* ThinLun��ʼ��ʧ��״̬ */
    ADM_LUN_STATUS_DELETING = 9,       /* ThinLunɾ��״̬ */
    ADM_LUN_STATUS_DELETEFAIL = 10,    /* ThinLunɾ����ʧ�� */
    ADM_LUN_STATUS_WRITE_PROTECT = 11  /* ThinLunֻ��״̬ */
};

enum adm_bgtask_type {
    ADM_BGTASK_TYPE_RW_FGR,       /* ��дǰ̨IO �����ֵ����ȡ */
    ADM_BGTASK_TYPE_COPYBACK,     /* �ؿ���̨IO */
    ADM_BGTASK_TYPE_PRECOPY,      /* ���࿽����̨IO */
    ADM_BGTASK_TYPE_BGINIT,       /* ��ʽ����̨IO */
    ADM_BGTASK_TYPE_REBUILD,      /* �ع���̨IO */
    ADM_BGTASK_TYPE_SANITIZE,     /* �������ٺ�̨IO */
    ADM_BGTASK_TYPE_EXPANDRG,     /* ��̬���Ժ�̨IO */
    ADM_BGTASK_TYPE_CONSIS_CHECK, /* һ����У�� */
    ADM_BGTASK_TYPE_PATROLREAD,   /* Ѳ�� */
    ADM_BGTASK_TYPE_FRONT_INIT,   /* ǰ̨��ʽ�� */
};

enum adm_card_bgtask_op_type {
    ADM_BGTASK_STOP_TYPE,
    ADM_BGTASK_START_TYPE,
    ADM_BGTASK_SET_TYPE
};

enum adm_bgtask_rate_speed {
    ADM_BGTASK_RATE_LOW = 1,
    ADM_BGTASK_RATE_MIDDLE,
    ADM_BGTASK_RATE_HIGH
};
enum adm_vd_repair_type { // ע��:�˿���ֵ��һ�㺬���෴
    ADM_AUTOCC_REPAIRE_AUTO, // �����ʾΪ:on (ֵΪ0)
    ADM_AUTOCC_REPAIRE_MANU
};

enum adm_back_rginfo_ground_type {
    ADM_RAID_GROUP_REBUILD,
    ADM_RAID_GROUP_COPYBACK,
    ADM_RAID_GROUP_PROCOPY,
    ADM_RAID_GROUP_EXPANDRG
};

enum adm_is_rg_copyback_all {
    ADM_RG_COPYBACK_ONE,
    ADM_RG_COPYBACK_ALL
};

enum adm_vd_access_policy {
    ADM_VD_ACCESS_READ_WRITE, // �ɶ���д
    ADM_VD_ACCESS_READ_ONLY,  // ֻ��
    ADM_VD_ACCESS_BUTT
};

/* End ��raid���ö�ٱ������壬�����޸Ĵ���ʱ����Ҫ���붨�� */

/* Begin �������ö�ٱ������壬�����޸Ĵ���ʱ����Ҫ���붨�� */
enum adm_pdlist_intf {
    ADM_DEVICE_TYPE_SAS = 1,
    ADM_DEVICE_TYPE_EXP = 2,
    ADM_DEVICE_TYPE_SATA = 8,
    ADM_DEVICE_TYPE_PLANE = 9,
    ADM_DEVICE_TYPE_NVME = 10,
    ADM_DEVICE_TYPE_UNKNOW,
    ADM_DEVICE_TYPE_BUTT
};

enum adm_pdlist_type {
    ADM_DISK_MEDIUM_TYPE_HDD = 0,
    ADM_DISK_MEDIUM_TYPE_SSD = 1,
    ADM_DISK_MEDIUM_TYPE_UNKNOWN
};

enum adm_pdlist_status {
    ADM_DISK_STATE_NEW,
    ADM_DISK_STATE_GOOD,
    ADM_DISK_STATE_BAD,
    ADM_DISK_STATE_OUT,
    ADM_DISK_STATE_INVALID,
    ADM_DISK_STATE_OFFLINE,
    ADM_DISK_STATE_PREFAIL,
    ADM_DISK_STATE_DISKSLOW,
    ADM_DISK_STATE_FOREIGN,
    ADM_DISK_STATE_INCOMPATIBLE,
    ADM_DISK_STATE_UNSUPPORTED,
    ADM_DISK_STATE_DIAGNOSE,
    ADM_DISK_STATE_PENDING,
    ADM_DISK_STATE_BUTT
};

enum adm_sas_link_speed {
    ADM_SAS_LINK_RATE_FREE = 0,   // no link
    ADM_SAS_LINK_RATE_1_5 = 8,    // 1.5G
    ADM_SAS_LINK_RATE_3_0 = 9,    // 3.0G
    ADM_SAS_LINK_RATE_6_0 = 10,   // 6.0G
    ADM_SAS_LINK_RATE_12_0 = 11,  // 12G
    ADM_SAS_LINK_RATE_24_0 = 12,  // 24G
    ADM_SAS_LINK_RATE_BUTT
};

enum adm_disk_size {
    ADM_DISK_SIZE_TYPE_NOT_REPORT = 0,
    ADM_DISK_SIZE_TYPE_525 = 1,  // 5.25inch
    ADM_DISK_SIZE_TYPE_35,       // 3.5inch
    ADM_DISK_SIZE_TYPE_25,       // 2.5inch
    ADM_DISK_SIZE_TYPE_18,       // 1.8inch
    ADM_DISK_SIZE_TYPE_LESS18,   // less than 1.8inch
    ADM_DISK_SIZE_TYPE_UNKNOWN
};

enum adm_disk_type {
    ADM_DISK_TYPE_FREE = 1,         /* ������ */
    ADM_DISK_TYPE_MEMBER = 2,       /* ��Ա�� */
    ADM_DISK_TYPE_SPARE = 3,        /* �ȱ��� */
    ADM_DISK_TYPE_SSDC = 4,         /* SSD CACHE��Ա�� */
    ADM_DISK_TYPE_PASS_THROUGH = 5, /* ֱͨ�� */
    ADM_DISK_TYPE_SMR = 6,          /* ֻ�ܵ���Ӳ��ʹ�ã��޷�����raid��ʹ�� */
    ADM_DISK_TYPE_RAW_DISK = 7,     /* ���� */
    ADM_DISK_TYPE_SED = 8,          /* ������ */
    ADM_DISK_TYPE_BUTT
};

enum adm_halflife_tag {
    ADM_NO_HALF_LIFE,
    ADM_HAVE_HALF_LIFE
};

enum adm_lun_map_type {
    ADM_MAP_TYPE_ST,
    ADM_MAP_TYPE_NT
};

enum adm_lun_type {
    ADM_LUN_TYPE_NORMAL,
    ADM_LUN_TYPE_PRIVATE,
    ADM_LUN_TYPE_EXPAND
};

enum adm_sas_phy_func {
    ADM_SAS_PHY_HARD_RESET = 1,
    ADM_SAS_PHY_LINK_RESET,
    ADM_SAS_PHY_DISABLE,
    ADM_SAS_PHY_ENABLE,
    ADM_SAS_PHY_SPEED,
    ADM_SAS_PHY_ENABLELOOP,
    ADM_SAS_PHY_DISABLELOOP,
    ADM_SAS_PHY_BUTT
};

enum adm_phy_enable {
    ADM_PHY_DISABLE,
    ADM_PHY_ENABLE
};

enum adm_bde_device_type {
    /* these are SAS protocol defined (attached device type field) */
    ADM_SAS_PHY_UNUSED = 0,
    ADM_SAS_END_DEVICE = 1,
    ADM_SAS_EDGE_EXPANDER_DEVICE = 2,
    ADM_SAS_SATA_DEV = 3,
    ADM_SAS_SATA_PENDING = 4,
    ADM_SAS_SES_DEVICE = 5,
    ADM_BDE_NVME_DEVICE = 6,
    ADM_BDE_DEVICE_BUTT
};

enum adm_sanitize_lun {
    ADM_SANITIZE_SIMPLE = 1,  /* ��һ�� */
    ADM_SANITIZE_NORMAL = 3,  /* ������ */
    ADM_SANITIZE_THOROUGH = 9 /* ������ */
};

enum adm_sanitize_mode {
    ADM_SANITIZE_OVERWRITE = 1,
    ADM_SANITIZE_BLOCKERASE = 2,
    ADM_SANITIZE_CRYPTOERASE = 3,
};

enum adm_precopy_switch {
    ADM_PRECOPY_SWITCH_ON = 1,
    ADM_PRECOPY_SWITCH_OFF
};

enum adm_chgegycpbsw_switch {
    ADM_CHGEGYCPBSW_ON = 1,
    ADM_CHGEGYCPBSW_OFF,
};

enum adm_diskbst_type {
    ADM_DISKBST_PHYSICAL,
    ADM_DISKBST_LOGICAL
};

enum adm_pcie_link_state {
    PCIE_STATE_LINKDOWN,
    PCIE_STATE_LINKUP
};

enum adm_pcie_link_speed {
    PCIE_LINK_SPEED_GEN1 = 1,
    PCIE_LINK_SPEED_GEN2,
    PCIE_LINK_SPEED_GEN3,
    PCIE_LINK_SPEED_GEN4
};

enum adm_disk_secerass_times {
    ADM_ERASE_TIME_SIMPLE = 1,
    ADM_ERASE_TIME_NORMAL = 3,
    ADM_ERASE_TIME_THOROUGH = 9
};

enum adm_raid_report_pf {
    ADM_RAID_PF0,
    ADM_RAID_PF1,
    ADM_RAID_AUTO
};

enum adm_dif_switch_status {
    ADM_DIF_SWITCH_OFF,
    ADM_DIF_SWITCH_ON
};

enum boot_pinnedcache_switch {
    SET_BOOT_PINNEDCACHE_OFF = 0,
    SET_BOOT_PINNEDCACHE_ON = 1
};

enum adm_scsi_cdb_len_teype {
    ADM_SCSI_CDB_LEN_SIX_BYTE = 6,
    ADM_SCSI_CDB_LEN_TEN_BYTE = 10,
    ADM_SCSI_CDB_LEN_TWELVE_BYTE = 12,
    ADM_SCSI_CDB_LEN_SIXTEEN_BYTE = 16,
    ADM_SCSI_CDB_LEN_THIRTY_TWO_BYTE = 32
};

enum adm_ldlist_sed_disk_type {
    ADM_LDLIST_NOT_SED_DISK,
    ADM_LDLIST_SED_DISK
};

enum adm_flash_test_type {
    ADM_CMD_FLASH_TEST_NAND,
    ADM_CMD_FLASH_TEST_NOR
};

enum adm_general_switch_value {
    ADM_GENERAL_SWITCH_OFF,
    ADM_GENERAL_SWITCH_ON
};

enum adm_card_passthru_type_value {
    ADM_CARD_PASSTHRU_OFF,
    ADM_CARD_PASSTHRU_JBOD,
    ADM_CARD_PASSTHRU_RAWDRIVE
};

enum adm_vd_sec_type_value {
    ADM_VD_SEC_TYPE_512N,
    ADM_VD_SEC_TYPE_512E,
    ADM_VD_SEC_TYPE_4KN,
    ADM_VD_SEC_TYPE_520,
    ADM_VD_SEC_TYPE_512_8,
    ADM_VD_SEC_TYPE_4160,
    ADM_VD_SEC_TYPE_4096_8_56,
    ADM_VD_SEC_TYPE_4096_56_8
};

enum adm_vd_sanitize_result {
    ADM_SANITIZE_RESULT_NONE = 1,
    ADM_SANITIZE_RESULT_RUNNING,
    ADM_SANITIZE_RESULT_COMPLETE,
    ADM_SANITIZE_RESULT_FAILED
};
enum adm_disk_physical_status {
    CMD_DISK_STATE_NEW,           // ��ʼ״̬
    CMD_DISK_STATE_GOOD,          // �û����õ�״̬
    CMD_DISK_STATE_BAD,           // ������
    CMD_DISK_STATE_OUTING,        // ���ڰ���
    CMD_DISK_STATE_OFFLINE,       // ��ʱ����
    CMD_DISK_STATE_ONLINE,        // ��ʱ����
    CMD_DISK_STATE_PREFAIL,       // ԤʧЧ��
    CMD_DISK_STATE_DISKSLOW,      // ����
    CMD_DISK_STATE_FOREIGN,       // raid���ü��ݵ��ǲ��Ǳ���raid�����
    CMD_DISK_STATE_INCOMPATIBLE,  // raid���ò����ݵ���
    CMD_DISK_STATE_UNSUPPORTED,   // ��֧�ֵ���
    CMD_DISK_STATE_DIAGNOSE,      // �������״̬����
    CMD_DISK_STATE_PENDING,       // �ȴ��ϱ�����
    CMD_DISK_STATE_BUTT
};

enum adm_set_boot_dev {
    CMD_BOOT_OFF,
    CMD_BOOT_ON
};

enum adm_show_boot_type {
    CMD_BOOT_TYPE_VD,
    CMD_BOOT_TYPE_PD
};

enum adm_new_switch_value {
    ADM_NEW_SWITCH_ON = 1,
    ADM_NEW_SWITCH_OFF
};

enum adm_hot_spare_status {
    HOT_SPARE_NON = 0,      // ���ȱ���
    HOT_SPARE_GLOBAL,       // ȫ���ȱ���
    HOT_SPARE_LOCAL,        // �ֲ��ȱ���
    HOT_SPARE_INVALID
};
/* End �������ö�ٱ������壬�����޸Ĵ���ʱ����Ҫ���붨�� */

/* ������Ҫ��¼������־�����Ƶ�����õ������ȥ�� */
#define CMD_NEED_PRINT(x) ((x) != ADM_CMD_GET_UART_LOG && (x) != ADM_CMD_UPDATE_FW \
                          && (x) != ADM_CMD_SEND_UART_LOG && (x) != ADM_CMD_SHOW_DISK \
                          && (x) != ADM_CMD_GET_LOG && (x) != ADM_CMD_GET_DUMP \
                          && (x) != ADM_CMD_TRANSPORT_FILE && (x) != ADM_CMD_GET_CTRL_INFO \
                          && (x) != ADM_CMD_GET_CTRL_STATUS && (x) != ADM_CMD_SHOW_PASSTHRU_TYPE \
                          && (x) != ADM_CMD_GET_SC_INFO && (x) != ADM_CMD_SHOW_EXP_PHY_STATUS \
                          && (x) != ADM_CMD_SHOW_VDS_ID && (x) != ADM_CMD_SHOW_VD_INFO \
                          && (x) != ADM_CMD_SHOW_RG_INFO && (x) != ADM_CMD_SHOW_PDS_ID \
                          && (x) != ADM_CMD_SHOW_RGS_ID)

/*****************************************************************************
 Description  : subopcode  ��ռ11λ
                ��2λ����������������:
                00 : ������
                01 : dfx
                10 : fanout + ����������dfx��ddi�������䣩
                11 : ddi
                �������������θ���λ���������������:
                00 : card����
                01 : rg �� vd����()
                10 : disk����
                11 : ϵͳ�� + ����
*****************************************************************************/
enum {
    ADM_CMD_UPDATE_FW = 0x0037,            // 55
    ADM_CMD_ACTIVE_FW = 0x0038,            // 56
    ADM_CMD_TRANSPORT_FILE = 0x0039,       // 57
    ADM_CMD_GET_FW_DATA = 0x003A,          // 58
    ADM_CMD_GET_UART_LOG = 0x003B,         // 59
    ADM_CMD_SEND_UART_LOG = 0x003C,        // 60
    ADM_CMD_HDTEST_FLASH = 0x003D,         // 61
    ADM_CMD_HDTEST_SRAM = 0x003E,          // 62
    ADM_CMD_HDTEST_SEC = 0x003F,           // 63
    ADM_CMD_SHOW_EYE_DIAGRAM = 0x0040,     // 64
    ADM_CMD_SDS_SET_DFE = 0x0041,          // 65
    ADM_CMD_SDS_SHOW_DFE = 0x0042,         // 66
    ADM_CMD_SDS_SET_CTLE = 0x0043,         // 67
    ADM_CMD_SDS_SHOW_CTLE = 0x0044,        // 68
    ADM_CMD_SDS_SET_FFE = 0x0045,          // 69
    ADM_CMD_SDS_SHOW_FFE = 0x0046,         // 70
    ADM_CMD_SDS_SET_SWING = 0x0047,        // 71
    ADM_CMD_SDS_SHOW_SWING = 0x0048,       // 72
    ADM_CMD_SHOW_PCIE_LINK = 0x0049,       // 73
    ADM_CMD_SHOW_PCIE_LTSSM = 0x004A,      // 74
    ADM_CMD_CLEAR_PCIE_LTSSM = 0x004B,     // 75
    ADM_CMD_SET_TX_MARGIN = 0x004C,        // 76
    ADM_CMD_SHOW_DIEID = 0x004E,           // 78
    ADM_CMD_SGPIO_LIGHT = 0x004F,          // 79
    ADM_CMD_SHOW_LINK_ERROR = 0x0050,      // 80
    ADM_CMD_CLEAN_LINK_ERROR = 0x0051,     // 81
    ADM_CMD_CLEAR_NAND = 0x0052,           // 82
    ADM_CMD_SRAM_ECC_INFO = 0x0053,        // 83
    ADM_CMD_DDR_INFO = 0x0054,             // 84
    ADM_CMD_SHOW_EFUSE = 0x0055,           // 85
    ADM_CMD_SET_EFUSE = 0x0056,            // 86
    ADM_CMD_SHOW_CHIP_ADC = 0x0057,        // 87
    ADM_CMD_SHOW_TEMP = 0x0058,            // 88
    ADM_CMD_SET_SAS_ADDR = 0x0059,         // 89
    ADM_CMD_SET_ELECTRON_LABEL = 0x005A,   // 90
    ADM_CMD_SET_LOOP_TYPE = 0x005B,        // 91
    ADM_CMD_SHOW_LOOP_TYPE = 0x005C,       // 92
    ADM_CMD_SHOW_PRBS_STA = 0x005D,        // 93
    ADM_CMD_SET_PRBS = 0x005E,             // 94
    ADM_CMD_SHOW_PRBS_TYPE = 0x005F,       // 95
    ADM_CMD_SHOW_VOL = 0x0060,             // 96
    ADM_CMD_SHOW_KEY_INFO = 0x0061,        // 97
    ADM_CMD_SHOW_CDR = 0x0062,             // 98
    ADM_CMD_SET_CDR = 0x0063,              // 99
    ADM_CMD_SET_PN = 0x0064,               // 100
    ADM_CMD_SHOW_PN = 0x0065,              // 101
    ADM_CMD_SHOW_FW_VERSION = 0x0066,      // 102
    ADM_CMD_GET_LOG = 0x0067,              // 103
    ADM_CMD_SET_BP_INJECT_DIE = 0x0068,    // 104
    ADM_CMD_SET_BP_DQS_DEFLECT = 0x0069,   // 105
    ADM_CMD_SET_MBIST = 0x006A,            // 106
    ADM_CMD_SHOW_MBIST = 0x006B,           // 107
    ADM_CMD_SET_BP_NAND = 0x006C,          // 108
    ADM_CMD_SET_EXMBIST = 0x006D,          // 109
    ADM_CMD_SHOW_EXMBIST = 0x006F,         // 111
    ADM_CMD_SHOW_ELECTRON_LABEL = 0x0070,  // 112
    ADM_CMD_CLEAR_LOG = 0x0071,            // 113
    ADM_CMD_SET_SELF_LEARN = 0x0072,       // 114
    ADM_CMD_SHOW_SM_CAP_V = 0x0073,        // 115
    ADM_CMD_SET_PRBS_CTRL = 0x0074,        // 116
    ADM_CMD_SET_BP_FS_V = 0x0075,          // 117
    ADM_CMD_SET_BP_FUN_V = 0x0076,         // 118
    ADM_CMD_GET_BP_INFO_V = 0x0077,        // 119
    ADM_CMD_CLEAR_EQUIP_CFG = 0x0078,      // 120
    ADM_CMD_GET_BP_METADATA_V = 0x0079,    // 121
    ADM_CMD_SET_TSP_FREQ = 0x007A,         // 122
    ADM_CMD_SHOW_TSP_FREQ = 0x007B,        // 123

    /* card������������ */
    ADM_CMD_GET_PDLIST_NUM = 0x501,              // 1281
    ADM_CMD_SHOW_PDLIST = 0x502,                 // 1282
    ADM_CMD_SET_BGRATE = 0x503,                  // 1283
    ADM_CMD_SHOW_BGTASK = 0x504,                 // 1284
    ADM_CMD_SET_COPYBACK_SW = 0x505,             // 1285
    ADM_CMD_SHOW_COPYBACK_SW = 0x506,            // 1286
    ADM_CMD_SET_PRECOPY_SW = 0x507,              // 1287
    ADM_CMD_SHOW_PRECOPY_SW = 0x508,             // 1288
    ADM_CMD_SET_PATROLREAD = 0x509,              // 1289
    ADM_CMD_SHOW_PATROLREAD = 0x50a,             // 1290
    ADM_CMD_SET_EHSP_SW = 0x50b,                 // 1291
    ADM_CMD_SHOW_EHSP_SW = 0x50c,                // 1292
    ADM_CMD_SET_PASSTHRU_TYPE = 0x50d,           // 1293
    ADM_CMD_SHOW_PASSTHRU_TYPE = 0x50e,          // 1294
    ADM_CMD_SET_DDMA = 0x50f,                    // 1295
    ADM_CMD_SHOW_DDMA = 0x510,                   // 1296
    ADM_CMD_SHOW_BOOT = 0x511,                   // 1297
    ADM_CMD_CLEAR_FNCFG = 0x512,                 // 1298
    ADM_CMD_IMPORT_FNCFG = 0x513,                // 1299
    ADM_CMD_SHOW_FNCFG = 0x514,                  // 1300
    ADM_CMD_SHOW_FNCFGLIST = 0x515,              // 1301
    ADM_CMD_SHOW_BST = 0x516,                    // 1302
    ADM_CMD_GET_CTRL_INFO = 0x517,               // 1303
    ADM_CMD_RESET_CFG = 0x518,                   // 1304
    ADM_CMD_SHOW_PDS_ID = 0x51b,                 // 1307
    ADM_CMD_SHOW_PCIE_ERROR = 0x51c,             // 1308
    ADM_CMD_CLEAR_PCIE_ERROR = 0x51d,            // 1309
    ADM_CMD_SCSI_PASSTHROUGH = 0x51e,            // 1310
    ADM_CMD_NVMe_PASSTHROUGH = 0x51f,            // 1311
    ADM_CMD_GET_CTRL_STATUS = 0x520,             // 1312
    ADM_CMD_GET_SC_INFO = 0x521,                 // 1313
    ADM_CMD_GET_BST_NUM = 0x522,                 // 1314
    ADM_CMD_SET_CONSIS_CHECK = 0x523,            // 1315
    ADM_CMD_SHOW_CONSIS_CHECK = 0x524,           // 1316
    ADM_CMD_SET_SMART_POLLING = 0x525,           // 1317
    ADM_CMD_SHOW_SMART_POLLING = 0x526,          // 1318
    ADM_CMD_SET_BOOT_PINNEDCACHE = 0x527,        // 1319
    ADM_CMD_SHOW_BOOT_PINNEDCACHE = 0x528,       // 1320
    ADM_CMD_I2C_DATATRANCE = 0x529,              // 1321
    ADM_CMD_SHOW_REG = 0x52a,                    // 1322
    ADM_CMD_SET_REG = 0x52b,                     // 1323
    ADM_CMD_SET_GLBCFG = 0x52c,                  // 1324
    ADM_CMD_SHOW_PHY_CFG = 0x52d,                // 1325
    ADM_CMD_SHOW_ALLREG = 0x52e,                 // 1326
    ADM_CMD_CLEAR_NORFLASH = 0x52f,              // 1327
    ADM_CMD_DETE_FLASH = 0x530,                  // 1328
    ADM_CMD_SET_POWERSAVE = 0x532,               // 1330
    ADM_CMD_SET_CAP_LEARN = 0x533,               // 1331
    ADM_CMD_SHOW_SELFLEARN = 0x534,              // 1332
    ADM_CMD_SET_ALARMTEMP = 0x535,               // 1333
    ADM_CMD_SET_SELFLEARN_CYCLE = 0x536,         // 1334
    ADM_CMD_QUERY_ACTIVE_TYPE = 0x537,           // 1335
    ADM_CMD_RESET_DEFAULT_CFG = 0x538,           // 1336
    ADM_CMD_SHOW_RAID_GLB_CFG = 0x539,           // 1337
    ADM_CMD_REGISTER_HOST_EVENT = 0x53a,         // 1338
    ADM_CMD_NOTIFY_POWERCYCLE_RESULT = 0x53b,    // 1339
    ADM_CMD_SET_BACKUPCAP = 0x53c,               // 1340
    ADM_CMD_GET_DUMP = 0x53d,                    // 1341
    ADM_CMD_SET_DUMP = 0x53e,                    // 1342
    ADM_CMD_SHOW_DDR_EYE = 0x53f,                // 1343
    ADM_CMD_SET_RETENTION = 0x540,               // 1344
    ADM_CMD_SET_REPORT_DRIVE_NUM = 0x541,        // 1345
    ADM_CMD_SHOW_PINNEDCACHE_LIST = 0x542,       // 1346
    ADM_CMD_SHOW_CAP_LEARN = 0x543,              // 1347
    ADM_CMD_SET_LOG_LEVEL = 0x544,               // 1348
    ADM_CMD_SHOW_LOG_LEVEL = 0x545,              // 1349
    ADM_CMD_SET_KICK_DISK_CYCLE = 0x546,         // 1350
    ADM_CMD_USER_RECOVERY = 0x547,               // 1351
    ADM_CMD_USER_RECOVERY_PROCESS = 0x548,       // 1352
    ADM_CMD_USER_RECOVERY_SET_DEFAULT = 0x549,   // 1353
    ADM_CMD_USER_RECOVERY_SHOW_DEFAULT = 0x54a,  // 1354
    ADM_CMD_USER_RECOVERY_CLEAR_ERROR = 0x54b,   // 1355
    ADM_CMD_SET_DRIVER_FC = 0x54c,               // 1356
    ADM_CMD_SHOW_DRIVER_FC = 0x54d,              // 1357
    ADM_CMD_SET_DPHSTARTMOD = 0x54e,             // 1358
    ADM_CMD_SHOW_DPHSTARTMOD = 0x54f,            // 1359
    ADM_CMD_GET_NAND_LOG = 0x550,                // 1360
    ADM_CMD_SET_NAND_LOG_IDX = 0x551,            // 1361
    ADM_CMD_SET_DHA_SW = 0x552,                  // 1362
    ADM_CMD_SHOW_DHA_SW = 0x553,                 // 1363
    ADM_CMD_SHOW_KICK_DRIVE_STATUS = 0x554,      // 1364
    ADM_CMD_SHOW_DRIVER_FC_EXPAND = 0x555,       // 1365
    ADM_CMD_SET_MERGE = 0x556,                   // 1366
    ADM_CMD_SHOW_MERGE = 0x557,                  // 1367
    ADM_CMD_SET_QUEUE_DEPTH = 0x558,             // 1368
    ADM_CMD_SHOW_QUEUE_DEPTH = 0x559,            // 1369
    ADM_CMD_SET_VD_FC = 0x55a,                   // 1370
    ADM_CMD_SHOW_VD_FC = 0x55b,                  // 1371
    ADM_CMD_SET_VD_MERGE = 0x55c,                // 1372
    ADM_CMD_SHOW_VD_MERGE = 0x55d,               // 1373
    ADM_CMD_SHOW_LOAD_FW_VERSION = 0x55e,        // 1374

    /* rg ������������ */
    ADM_CMD_CREATE_RG = 0x600,             // 1536
    ADM_CMD_DELETE_RG = 0x601,             // 1537
    ADM_CMD_GET_RG_NUM = 0x602,            // 1538
    ADM_CMD_SHOW_RGLIST = 0x603,           // 1539
    ADM_CMD_SHOW_RG_INFO = 0x604,          // 1540
    ADM_CMD_EXPAND_RG_DISK = 0x605,        // 1541
    ADM_CMD_SHOW_RG_EXP_PROGRESS = 0x606,  // 1542
    ADM_CMD_SHOW_MEMBER_DISKS = 0x607,     // 1543
    ADM_CMD_SHOW_RGS_ID = 0x608,           // 1544
    ADM_CMD_MEMBER_DISK_NUM = 0x609,       // 1545
    ADM_CMD_SET_RG_VD_PDCACHE = 0x60A,     // 1546

    /* vd ������������ */
    ADM_CMD_CREATE_VD = 0x700,              // 1792
    ADM_CMD_DELETE_VD = 0x701,              // 1793
    ADM_CMD_GET_VDLIST_NUM = 0x702,         // 1794
    ADM_CMD_SHOW_VDLIST = 0x703,            // 1795
    ADM_CMD_SHOW_VD_INFO = 0x704,           // 1796
    ADM_CMD_EXPAND_VD_SIZE = 0x706,         // 1798
    ADM_CMD_SET_VD_CONSIS_CHECK = 0x707,    // 1799
    ADM_CMD_SHOW_VD_CONSIS_CHECK = 0x708,   // 1800
    ADM_CMD_SET_VD_SANITIZE = 0x709,        // 1801
    ADM_CMD_SHOW_VD_SANITIZE = 0x70A,       // 1802
    ADM_CMD_SET_VD_BOOT = 0x70B,            // 1803
    ADM_CMD_SET_VD_ATTR = 0x70C,            // 1804
    ADM_CMD_SHOW_VD_INCONSISTENT = 0x70D,   // 1805
    ADM_CMD_SHOW_VDS_ID = 0x70E,            // 1806
    ADM_CMD_DELETE_VD_PINNEDCACHE = 0x70F,  // 1807

    /* disk������������ */
    ADM_CMD_GET_DISK_DID = 0x800,          // 2048
    ADM_CMD_SET_DISK_PASSTHRU = 0x801,     // 2049
    ADM_CMD_SET_DISK_BOOT = 0x802,         // 2050
    ADM_CMD_SET_DISK_HSP = 0x803,          // 2051
    ADM_CMD_SET_DISK_ON_OFFLINE = 0x804,   // 2052
    ADM_CMD_SET_DISK_FREE = 0x805,         // 2053
    ADM_CMD_SHOW_DISK = 0x806,             // 2054
    ADM_CMD_SET_DISK_PATROLREAD = 0x807,   // 2055
    ADM_CMD_SHOW_DISK_PATROLREAD = 0x808,  // 2056

    ADM_CMD_SET_DISK_PHY = 0x80A,            // 2058
    ADM_CMD_SHOW_PHY_STATUS = 0x80B,         // 2059
    ADM_CMD_SHOW_DISK_PHY_EEROR = 0x80C,     // 2060
    ADM_CMD_SET_DISK_QUEUE_DEPTH = 0x80D,    // 2061
    ADM_CMD_SHOW_DISK_IO_SUM = 0x80E,        // 2062
    ADM_CMD_SET_DISK_SANITIZE = 0x80F,       // 2063
    ADM_CMD_SET_DISK_ERASE = 0x810,          // 2064
    ADM_CMD_SHOW_SANITIZE_PROGRESS = 0x811,  // 2065
    ADM_CMD_SHOW_DISK_ERASE = 0x812,         // 2066
    ADM_CMD_SET_DISK_CACHE = 0x813,          // 2067
    ADM_CMD_SHOW_DISK_CACHE = 0x814,         // 2068
    ADM_CMD_SET_DISK_LED = 0x815,            // 2069
    ADM_CMD_ACTIVE_DISK_FW = 0x816,          // 2070
    ADM_CMD_GET_DISK_SMART_INFO = 0x817,     // 2071
    ADM_CMD_GET_DISKTYPE = 0x818,            // 2072
    ADM_CMD_SET_DISK_LOOPBACK = 0x819,       // 2073
    ADM_CMD_CLEAR_SAS_PHYERROR = 0x81a,      // 2074
    ADM_CMD_SET_NCQ = 0x81b,                 // 2075
    ADM_CMD_SET_DISK_POWERSAVE = 0x81c,      // 2076
    ADM_CMD_SET_SLOWDISK = 0x81d,            // 2077
    ADM_CMD_SET_PHY_CFG = 0x81e,             // 2078
    ADM_CMD_SHOW_SSD_WEAR = 0x81f,           // 2079
    ADM_CMD_SET_HOTSPARE_SLOT = 0x820,       // 2080
    ADM_CMD_SHOW_HOTSPARE_SLOT = 0x821,      // 2081
    ADM_CMD_SHOW_SLOWDISK = 0x822,           // 2082
    ADM_CMD_SET_SLOWDISK_EX = 0x823,         // 2083
    ADM_CMD_SHOW_SLOWDISK_EX = 0x824,        // 2084

    /* ������󣨹���disk���ֿռ䣩 */
    ADM_CMD_SHOW_EXP_PHY_STATUS = 0x8f0,  // = 2288
};

#define ADM_RAID_NVME_DOWNLOAD_IMAG_OPCODE (0x11)

/*****************************************************************************
 Description  : ��������Ĺ���ͨ����ioctl���ݽṹ��64+12�ֽ�
*****************************************************************************/
struct adm_ioctl_sqe_r64 {
    u8 opcode;  // �����Զ��������
    u8 flags;   // ����PSDT��FUSE������ѡ��PRP/SGL�Լ��ں������־�����ֶ�����������ѡ��
    u16 rsvd0;  // ����
    u32 nsid;   // namespace ID
    union {
        struct {
            u16 subopcode;  // ��ʶ�����һ������
            u16 rsvd1;      // ��ʶ�����һ������
        } info_0;
        u32 cdw2;
    };
    union {
        struct {
            u16 data_len;  // ���ݴ���ʱ����ָ�����ݳ���
            u16 param_len;
        } info_1;
        u32 cdw3;
    };
    u64 metadata;  // Ԫ����
    u64 addr;      // ���ݵ�ַ
    u32 metadata_len;
    u32 data_len; // ���ڼ���nvme��������������������ʹ�ô�ֵ��dma
    u32 cdw10;
    u32 cdw11;
    u32 cdw12;
    u32 cdw13;
    u32 cdw14;
    u32 cdw15;
    u32 timeout_ms;
    u32 result0;
    u32 result1;
};

// ���ڼ���nvme result�ֶ�ֻ��32λ�������汾
struct adm_ioctl_sqe_r32 {
    u8 opcode;  // �����Զ��������
    u8 flags;   // ����PSDT��FUSE������ѡ��PRP/SGL�Լ��ں������־�����ֶ�����������ѡ��
    u16 rsvd0;  // ����
    u32 nsid;   // namespace ID
    union {
        struct {
            u16 subopcode;  // ��ʶ�����һ������
            u16 rsvd1;      // ��ʶ�����һ������
        } info_0;
        u32 cdw2;
    };
    union {
        struct {
            u16 data_len;  // ���ݴ���ʱ����ָ�����ݳ���
            u16 param_len;
        } info_1;
        u32 cdw3;
    };
    u64 metadata;  // Ԫ����
    u64 addr;      // ���ݵ�ַ
    u32 metadata_len;
    u32 data_len; // ���ڼ���nvme��������������������ʹ�ô�ֵ��dma
    u32 cdw10;
    u32 cdw11;
    u32 cdw12;
    u32 cdw13;
    u32 cdw14;
    u32 cdw15;
    u32 timeout_ms;
    u32 result0;
};

/*****************************************************************************
 Description  : ���������ioͨ����ioctl���ݽṹ��128+12�ֽ�
*****************************************************************************/
struct adm_ioctl_scsi_pthru {
    u8  opcode;
    u8  flags;
    u16 rsvd0;
    u32 nsid;  // cdw1
    union {
        struct {
            u16 res_sense_len; // cdb͸��ʹ�ã���ǰĬ��96�ֽ�
            u8  cdb_len;
            u8  rsvd0;
        } info_0;
        u32 cdw2;
    };
    union {
        struct {
            u16 subopcode;
            u16 rsvd1;
        } info_1;
        u32 cdw3;
    };
    union {
        struct {
            u16 rsvd;
            u16 param_len; // cdw10 ~ cdw23 ,cdb͸����ʹ��
        } info_2;
        u32 cdw4;
    };
    u32 cdw5;
    u64 addr;
    u64 prp2;
    union {
        struct {
            u16 eid;
            u16 sid;
        } info_3;
        u32 cdw10;
    };
    union {
        struct {
            u16 did;
            u8  did_flag;
            u8  rsvd2;
        } info_4;
        u32 cdw11;
    };
    u32 cdw12;
    u32 cdw13;
    u32 cdw14;
    u32 data_len; // buff_len
    u32 cdw16;
    u32 cdw17;
    u32 cdw18;
    u32 cdw19;
    u32 cdw20;
    u32 cdw21;
    u32 cdw22;
    u32 cdw23;
    u64 sense_addr;
    u32 cdw26[4];
    u32 timeout_ms;
    u32 result0;        // BSG ��֧�� ����result�ֶ�
    u32 result1;
};

#define ADM_IOCTL_CODE_SQER64 _IOWR('N', 0x41, struct adm_ioctl_sqe_r64)
#define ADM_IOCTL_CODE_SQER32 _IOWR('N', 0x41, struct adm_ioctl_sqe_r32)
#define ADM_IOCTL_CODE_SCSI_PTHRU _IOWR('N', 0x42, struct adm_ioctl_scsi_pthru)

#define ADM_BSG_MSGCODE_SCSI_PTHRU  1    // ��ʶ��ͨ��BSG����ʱ���ݵ���������Ϊscsiֱͨ���һ���������Ĭ��Ϊ0
// BSG ioctl ��װ�ṹ��
struct bsg_ioctl_cmd {
    u32 msgcode;    // ���ԭIOCTL���������ã�BSGֻ��һ������������
    u32 control;    // ��bit���壨����sg_io_v4 response ������Sense��bit0=1ԭ���壬�����������޴˽ṹ������result��bit0=0��
    union {
        struct adm_ioctl_sqe_r64   ioctl_r64;      // �����ṹ��result�ֶ�64λ
        struct adm_ioctl_scsi_pthru ioctl_pthru;    // ͸��scsi�����ṹ��
    };
};

/* ***************************************************************
 *Description  : Hifan Tool Param Define Here
 *************************************************************** */
#define HIFAN_IOCTL_CMD _IOWR('N', 0x41, struct adm_ioctl_sqe_r64)
#define MAX_FRAGMENT_LEN (1024 * 4)

#ifdef PAGE_SIZE
#undef PAGE_SIZE
#endif
#define PAGE_SIZE (1024 * 4)

#define HIFAN_MAX_FILE_NAME_LENGTH (256)
#define HIFAN_MAX_FILE_DIR_LEN (512)

/* subcode:11bit,����λ������������ 10:fanout Ԥ��+ ���� */
enum hifan_cli_cmd_id {
    /* normal cmd */
    CMD_SYS_HELP = ((0x2 << 9) | 0x0), /* num 0x2 9 0x0 used to compute fanout cmd id */
    CMD_CARD_INFO,
    CMD_CARD_VERSION,
    CMD_REG_OPER,
    CMD_PORT_INFO,
    /* dma cmd */
    CMD_LOAD_FILE_ATTR = ((0x2 << 9) | 0x20), /* num 0x2 9 0x20 used to compute fanout cmd id */
    CMD_LOAD_FILE,
    CMD_FW_UPDATE_ATTR,
    CMD_FW_UPDATE,
    CMD_FW_LOG,
    CMD_DMA_DEBUG = ((0x2 << 9) | 0xFF), /* num 0x2 9 0xFF used to compute fanout cmd id */
    CMD_MAX_CNT = 0xFFFFFFFF,
};
#define is_dma_cmd(sub_cmd) \
    ((sub_cmd == CMD_FW_UPDATE) || (sub_cmd == CMD_FW_LOG) || (sub_cmd == CMD_LOAD_FILE) || (sub_cmd == CMD_DMA_DEBUG))

/* Define cmd name here */
#define HIFAN_CMD_SPACE "    "
#define HIFAN_TOOL_NAME "hifanadm"
#define CLI_CMD_HELP "help"
#define CLI_CMD_INFO "info"
#define CLI_CMD_VERSION "version"
#define CLI_CMD_REG_OPER "reg_oper"
#define CLI_CMD_PORT_INFO "port_info"
#define CLI_CMD_DOWN_LOAD "download"
#define CLI_CMD_FW_UPDATE "fw_update"
#define CLI_CMD_FW_LOG "fw_log"
#define CLI_CMD_DMA_DEBUG "dma_debug"

/* Define cmd param struct here */
#define CARD_NUM_PER_CHAN 10
struct hifan_cmd_card_info {
    u32 rsvd[2];
};
struct hifan_cmd_card_info_rsp {
    u8 card_num;
    u32 card_idx[CARD_NUM_PER_CHAN];
};

#define CARD_VERSION_LEN 16
struct hifan_cmd_card_version {
    u32 rsvd[2];
};
struct hifan_cmd_card_version_rsp {
    u8 drv_ver[CARD_VERSION_LEN];
    u8 fw_ver[CARD_VERSION_LEN];
    u8 pcie_ver[CARD_VERSION_LEN];
};

struct hifan_cmd_fw_log {
    u32 resv[2];
};
struct hifan_cmd_fw_log_rsp {
    u32 data[MAX_FRAGMENT_LEN];
    u32 data_len;
    u32 log_size;
};

#define MBOX_REG_READ (0)
#define MBOX_REG_WRITE (1)
struct hifan_cmd_reg_oper {
    /* (1) reg read:  opcode-0, reg: 0xxx, reg_val: any not use
       (2) reg write: opcode-1, reg: 0xxx, reg_val: 0xxxx */
    u32 opcode;
    u32 reg;
    u32 reg_val;
};
struct hifan_cmd_reg_oper_rsp {
    /* (1) reg read:  reg: read, reg_value: register val
       (2) reg write: not used */
    u32 reg;
    u32 reg_val;
};

#define LIMIT_NUM 100
struct hifan_cmd_reg_dump {
    u32 start_reg;
    u32 end_reg;
};
struct hifan_cmd_reg_dump_rsp {
    u32 num;
    u32 reg[LIMIT_NUM];
    u32 val[LIMIT_NUM];
};

#define MAX_PCIE_PORT_ID 16
struct hifan_cmd_port_info {
    /* bit0~5 define in pcie module
       port0-15 is rp, core0 port0-15 in chip
       port16 is ep, core1 port0 in chip */
    u32 pcie_port_id : 4;    // port1-15
    u32 pcie_port_mode : 1;  // 1:pcie-ep, 0:pcie-rp,disk
    u32 resv : 27;
};
struct hifan_cmd_port_info_rsp {
    u32 pcie_mode;   /* PCIe mode, RC/EP */
    u32 port_en;     /* disable or enable */
    u32 link_status; /* linkstatus, 0-link-down 1-link-up */
    u32 port_width;  /* link speed, Gen 1.0/2.0/3.0/4.0 */
    u32 port_gen;    /* linkstatus, 0-link-down 1-link-up */
};

struct hifan_cmd_common {
    u32 resv[5]; /* ����ܴ���20byte ���� */
};

/*****************************************************************************
 Description  : download file cmd for dfx
 Function     : download�ļ��Ĳ���
 command      : toolname cx download_dfx -f filepath
 Direction    : cmd ÿ�δ����ļ����Ϊ0x1000��
*****************************************************************************/
struct hifan_load_file_param {
    struct {
        u8 is_file_attr : 1;  // ���������Ƿ����ļ��������ݣ�1���ļ��������ݣ�0���ļ��������ݣ�
        u8 is_last : 1;       // �Ƿ�Ϊ���һ�����ݣ�1���ǣ�0 ������
        u8 rsvd : 6;          // ��������
    } info;
    u8 rsvd0;
    u16 send_cnt;      // ��ǰ�����ǵڼ�Ƭ���� ��start with 0;
    u32 current_size;  // ���δ������ݵĴ�Сbyte
    u32 file_tag;      // �ļ��ض��������tag
    u32 rsvd1;
};

struct hifan_load_file_attr {
    u32 total_size;  // ��ʶ�ļ���Сbyte
    u8 file_name[HIFAN_MAX_FILE_NAME_LENGTH];
    u32 file_tag;  // ÿ����һ���ļ�ʱ���ɵ������tag
    u32 file_crc;
};

struct hifan_cmd_dma_debug {
    u32 opt;
    u64 addr;
};

struct hifan_tool_param {
    u32 fanout_id;
    u32 opcode;
    void *addr;
    u32 data_len;
    union {
        struct hifan_cmd_common common;
        struct hifan_cmd_card_info card_info;
        struct hifan_cmd_card_version card_version;
        struct hifan_cmd_fw_log fw_log;
        struct hifan_cmd_reg_oper reg_oper;
        struct hifan_cmd_reg_dump reg_dump;
        struct hifan_load_file_param file_param;
        struct hifan_cmd_port_info port_info;
        struct hifan_cmd_dma_debug dma_debug;
    };
};

/*****************************************************************************
 Description  : lib NVMe_PASSTHROUGH�������ݽṹ ��lib_cmd_param->p_param�л�ȡ
*****************************************************************************/
struct nvme_pt_cmd {
    u8 opcode;  // �����Զ��������
    u8 flags;   // ����PSDT��FUSE������ѡ��PRP/SGL�Լ��ں������־�����ֶ�����������ѡ��
    u16 rsvd0;
    u32 nsid;  // namespace ID
    u32 cdw2;
    u32 cdw3;
    u64 metadata;      // Ԫ����
    u64 addr;          // ���ݵ�ַ prp1
    u32 metadata_len;  // prp2_parat_1
    u32 data_len;      // prp2_part_2
    u32 cdw10;
    u32 cdw11;
    u32 cdw12;
    u32 cdw13;
    u32 cdw14;
    u32 cdw15;
};

/*****************************************************************************
 Description  : nvme cmd
 Function     : NVMe_PASSTHROUGH����
 command      : toolname cx nvme --opcode u8 --nsid u32 -f save_data_file --cdw10 F0,F1,F2,F3
                --cdw11 F0,F1,F2,F3 --cdw12 F0,F1,F2,F3 --cdw13 F0,F1,F2,F3 --cdw14 F0,F1,F2,F3
                --cdw15 F0,F1,F2,F3
 Direction    : opcode: ADM_CMD_NVMe_PASSTHROUGH
*****************************************************************************/
struct nvme_cmd_param {
    u8 opcode;                                  //
    u8 type;                                    // cmd_type
    u16 data_len;                               // �������ݳ���
    u32 nsid;                                   // namespace id
    u8 file_path[ADM_MAX_FILE_NAME_LENGTH];  // nvme ����ص����ݱ����ļ�, ��������ڴ��ַ��ֵ�� nvme_pt_cmd->addr
    u32 cdw10;
    u32 cdw11;
    u32 cdw12;
    u32 cdw13;
    u32 cdw14;
    u32 cdw15;
};

#define CDB_DISK_SCSI_IMAGE_DOWNLOAD_OPCODE 0x3B
#define CDB_DISK_SCSI_IMAGE_SERVER_ACTION 0xE

struct scsi_update_pthru_cdb {
    u8 opcode;  // �����Զ��������
    u8 model;
    u8 buffer_id;
    u8 buff_offset[3];
    u8 transfer_length[3];
    u8 control;
};

/* card commands */

/*****************************************************************************
 Description  : copy back status
 command      : add
 Direction      : none
*****************************************************************************/
struct cmd_rg_copyback_status {  // ���úͲ�ѯ����
    u8 is_all_raidgroup;         // 0 ָ��raid��   1��������raid��
    u8 copyback_status;          // 0 or 1
};


enum pdlist_dif_size {
    PDLIST_DIF_SIZE_0 = 0,
    PDLIST_DIF_SIZE_8,
    PDLIST_DIF_SIZE_8_56,
    PDLIST_DIF_SIZE_56_8,
    PDLIST_DIF_SIZE_NA
};

enum pdlist_dif_type {
    PDLIST_DIF_TYPE_NA = 0,
    PDLIST_DIF_TYPE_1 = 1,
    PDLIST_DIF_TYPE_2 = 2,
    PDLIST_DIF_TYPE_3 = 3
};

enum pdlist_sp_type {
    PDLIST_SP_TYPE_NA = 0,
    PDLIST_SP_TYPE_1 = 1,
    PDLIST_SP_TYPE_2 = 2
};

/*****************************************************************************
 Description  :
 Function     : ��ʾexpander�б���Ϣ
 Command      : toolname cx explist show
 Direction    : read
*****************************************************************************/
struct cmd_show_explist {
    struct {
        u16 exp_num;  // ��ѯ����expander��
        u16 rsvd[3];  // �ֽڶ���
    } head;
    struct {
        u16 enc_id;  // ���
        u8 slot_id;  // ��λ��
        u8 level;    // ������expander������������֧��6��
        u32 rsvd;    // �ֽڶ���
    } expinfo[CMD_MAX_EXP_NUM];
};


/*****************************************************************************
 Description  : set lun attribute
 Function     : ����lun��������
 command      : toolname cx:vdx set [-name name] [-w {WB/WT/WBE}] [-r {No_Ahead|Ahead}]
 Direction    : set
*****************************************************************************/
struct cmd_set_lun {
    char name[MAX_LUN_NAME_LEN];
    u16 read_type;   // ��1��No_Ahead ��2��Ahead
    u16 write_type;  // ��1��WB����2��WT ����3��WBE
};

/*****************************************************************************
 Description  : set lun_disk_write_cache
 Function     : ����lun��Ա��дcache����
 command      : raidadm cx vbx writecache on/off
 Direction    : set
*****************************************************************************/
struct cmd_disk_write_cache {
    u16 lun_id;
    u16 switcher;  // 0:off    1:on
};

enum adm_disk_active {
    ADM_ACTIVE_DISK_FW_STOP_IO,
    ADM_ACTIVE_DISK_FW_RUN_IO
};

struct adm_sas_smart_attr {
    u32 asc;
    u32 ascq;
    u32 fru;
    u32 rsvd;
};

struct adm_sata_smart_attr {
    u16 smart_id;
    u16 value;
    u16 worst;
    u16 thresh;
    u8 type;
    u8 updated;
};

struct adm_nvme_smart_attr {
    u16 smart_id;
    u16 rsvd;
    u64 value[2];
};

enum adm_smart_type {
    ADM_SMART_DISK_SAS,
    ADM_SMART_DISK_SATA,
    ADM_SMART_DISK_NVME
};

#define ADM_MAX_SATA_SMART_NUM 64
#define ADM_MAX_NVME_SMART_NUM 64
#define ADM_SMART_DISK_MODEL_MAX_STR_LEN   41
#define ADM_SMART_DISK_VER_MAX_STR_LEN 32

enum adm_sata_attr_title {
    ADM_VENDOR_SPECIFIC_SMART,  // Vendor Specific SMART Attributes with Thresholds
    ADM_MAN_MADE_SMART          // Man-made SMART Attributes with Thresholds
};

struct adm_sata_smart_attr_all {
    struct adm_sata_smart_attr sata_smart[ADM_MAX_SATA_SMART_NUM];
    u16 flag[ADM_MAX_SATA_SMART_NUM];
    union {
        u64 raw_val;
        u8 raw_value[8];
    } raw_union[ADM_MAX_SATA_SMART_NUM];
    u8 model[ADM_SMART_DISK_MODEL_MAX_STR_LEN];  // �����ͺţ������ַ���������
    u8 rsv[7];
    u8 fw_version[ADM_SMART_DISK_VER_MAX_STR_LEN];  // ���̹̼��汾
};
/*****************************************************************************
 Description  : GET DISK SMART INFO
 Function     : ��ȡ��ӦӲ�̵�smart��Ϣ
 command      :
 Direction    : subopcode: ADM_CMD_GET_DISK_SMART_INFO
*****************************************************************************/
#define SMART_INFO_MAGIC 0xAABB
struct adm_smart_info {
    u8 type;        // ��ע���ص������������� adm_smart_type
    u8 title_type;  // Vend 1,
    u16 attr_num;   // ���ص���Ŀ����
    u16 magic;      // SMART_INFO_MAGIC,�����ϰ汾��ʾ
    u8 rsv[2];
    union {
        struct adm_sas_smart_attr sas_smart;
        struct adm_sata_smart_attr_all sata_smart_all;
        struct adm_nvme_smart_attr nvme_smart[ADM_MAX_NVME_SMART_NUM];
    };
};

#define ADM_RAID_SELECT_MASTER 1
#define ADM_RAID_SWITCH_MASTER 2

#define MT_MSG_DATA_LEN 28
/* multihost״̬�����Ϣ */
struct mt_status_msg {
    int msgid;
    u8 data[28]; /* ����ʹ�� */
};

enum ssd_max_io_size {
    SSD_MAX_IO_128K = 0x80,
    SSD_MAX_IO_256K = 0x100,
    SSD_MAX_IO_1M = 0x400
};

/* dm�������������Ϣ */
struct host_dev_table {
    u32 device_id;
    u16 host_target;
    u8 host_channel;
    u8 host_lun_num;
    u8 boot : 1;
    u8 pt : 1;
    u8 disktype : 3;
    u8 rawdisk : 1;
    u8 rsvd0 : 2;
    u8 valid : 1;  /* host driver use */
    u8 change : 1; /* if lun changed, such as capacity */
    u8 rsvd1 : 6;
    u16 max_io_size_kb;
};

/*****************************************************************************
 Description  : raid ��ȫ�����ò�ѯ
 Direction    : subopcode: ADM_CMD_SHOW_RAID_GLB_CFG
*****************************************************************************/
struct cmd_show_glb_cfg {
    u8 precopy_sw;
    u8 precopy_rate;
    u8 copyback_sw;
    u8 copyback_rate;

    u8 ehsp_sw;
    u8 ddma_sw;
    u8 rsvd;
    u8 powersave_sw;
    u32 powersave_wait_time;

    u8 ccheck_sw;
    u8 ccheck_repair_sw;
    u8 ccheck_rate;
    u8 ccheck_period;

    u8 patrolread_sw;
    u8 patrolread_rate;
    u8 disk_parall_num;
    u8 patrolread_period;

    u8 bginit_rate;
    u8 rebuild_rate;
    u8 sanitize_rate;
    u8 expandrg_rate;
    u8 rsvd1[8];
};

struct raid_set_card_dif {
    u8 opcode;
    u8 flags;
    u16 command_id;
    u32 device_id;  // �����ɼ����豸ID��NSID����0xffffffff��ʾ���Ѵ����������豸������
    u32 rsvd0[9];
    u32 sub_opcode : 8;
    u32 rsvd1 : 24;
    u8 enable;  // 0 �رգ�1 ����
    u8 rsvd2[3];
    u32 rsvd3[3];
};

struct set_wstrip_rinsert {
    u8 opcode;
    u8 flags;
    u16 command_id;
    u32 device_id;  // �����ɼ����豸ID��NSID��
    u32 rsvd0[9];
    u32 sub_opcode : 8;
    u32 rsvd1 : 24;
    u8 enable;  // 0 �ָ���LBAF��1 ������LBAF+DIF
    u8 rsvd2[3];
    u32 rsvd3[3];
};

enum sal_abt_type {
    SAL_ABORT_IO,
    SAL_ABORT_ADMIN,
};

struct raid_eh_param {
    union {
        struct {
            u16 cmd_id;
            u16 sq_id;
            u8 is_admin; /* 0:����IO,1:�������� */
            u8 rsv[3];
        } io_abt;
        struct {
            u8 type;
            u8 rsv[7];
        } dev_abt;
        struct {
            u8 type;
            u8 rsv[7];
        } pf_abt;
        struct {
            u8 ctrl_id;
            u8 rsv[7];
        } ctrl_rst;
    };
};

struct sal_reset_nvme_ctrl {
    u8 opcode;
    u8 fuse : 2;
    u8 rsv0 : 4;
    u8 psdt : 2;
    u16 cmd_id;
    u32 ns_id;
    u8 sub_opcode;
    u8 rsv1;
    u8 slot_id;
    u8 es_id;
    u32 rsv2;
    u64 mptr;
    u64 prp1;
    u64 prp2;
    u8 reset_type;
    u8 rsv3;
    u16 rsv4;
    u32 rsv5;
    u32 rsv6;
    u32 rsv7;
    u32 rsv8;
    u32 rsv9;
};

struct sal_set_pcie_port_state {
    u8 opcode;
    u8 fuse : 2;
    u8 rsv0 : 4;
    u8 psdt : 2;
    u16 cmd_id;
    u32 ns_id;
    u8 sub_opcode;
    u8 rsv1;
    u8 slot_id;
    u8 es_id;
    u32 rsv2;
    u64 mptr;
    u64 prp1;
    u64 prp2;
    u8 state;
    u8 rsv3;
    u16 rsv4;
    u32 rsv5;
    u32 rsv6;
    u32 rsv7;
    u32 rsv8;
    u32 rsv9;
};

struct sal_get_pcie_port_list {
    u8 opcode;
    u8 fuse : 2;
    u8 rsv0 : 4;
    u8 psdt : 2;
    u16 cmd_id;
    u32 ns_id;
    u8 sub_opcode;
    u8 rsv1;
    u8 slot_id;
    u8 es_id;
    u32 rsv2;
    u64 mptr;
    u64 prp1;
    u64 prp2;
    u32 rsv3;
    u32 rsv4;
    u32 rsv5;
    u32 rsv6;
    u32 rsv7;
    u32 rsv8;
};

struct sal_get_pcie_port_info {
    u8 opcode;
    u8 fuse : 2;
    u8 rsv0 : 4;
    u8 psdt : 2;
    u16 cmd_id;
    u32 ns_id;
    u8 sub_opcode;
    u8 rsv1;
    u8 slot_id;
    u8 es_id;
    u32 rsv2;
    u64 mptr;
    u64 prp1;
    u64 prp2;
    u32 rsv3;
    u32 rsv4;
    u32 rsv5;
    u32 rsv6;
    u32 rsv7;
    u32 rsv8;
};

struct sal_get_ctrl_list {
    u8 opcode;
    u8 fuse : 2;
    u8 rsv0 : 4;
    u8 psdt : 2;
    u16 cmd_id;
    u32 ns_id;
    u8 sub_opcode;
    u8 rsv1;
    u16 rsv2;
    u32 rsv3;
    u64 mptr;
    u64 prp1;
    u64 prp2;
    u32 rsv4;
    u32 rsv5;
    u32 rsv6;
    u32 rsv7;
    u32 rsv8;
    u32 rsv9;
};

struct sal_nvme_pt {
    u8 opcode;
    u8 fuse : 2;
    u8 rsv0 : 4;
    u8 psdt : 2;
    u16 cmd_id;
    u32 ns_id;
    u8 sub_opcode;
    u8 ctrl_ns_id; /* ��Ӧ���ϵ�nsid */
    u8 slot_id;
    u8 es_id;
    u32 rsv2;
    u64 mptr;
    u64 prp1;
    u64 prp2;
    u32 rsv3;
    u32 rsv4;
    u32 rsv5;
    u32 rsv6;
    u32 rsv7;
    u32 rsv8;
};

struct fe_stor_sqe {
    u8 opcode;
    u8 flags;
    u16 command_id;
    u32 device_id;
    u64 in_prp;     // ������� С��4K
    u64 out_prp;    // ������� С��4K
    u64 data_prp1;  // ���� С��1M
    u64 data_prp2;
    u32 in_len : 16;   // �����������
    u32 out_len : 16;  // �����������
    u32 sub_opcode : 8;
    u32 data_len : 24;  // ���ݳ���
    u32 cdw12[4];
};

struct sal_switch_master {
    u16 tmout;
};

#define SAL_DISK_EVENT_AEN 0x57

enum {
    NOTIFY_DISK_OPCODE = 0x1,
    NOTIFY_SATA_DISK_ERR = 0x2
};

struct sal_disk_event {
    u8 opcode;
    u8 rsvd[3];
    union {
        u32 status;
        struct {
            u32 cq_info[4];
            u32 logic_id;
            u32 rsv2[1];
        };
    };
};

struct sal_tmf_command {
    u8 tmf_type;
    u16 cmd_id;
    u16 sq_id;
    u8 is_admin;
};

enum raid_sal_opcode {
    RAID_SAL_NO_WR = 0xd0,
    RAID_SAL_WRITE = 0xd1,
    RAID_SAL_READ = 0xd2,
    RAID_SAL_EH = 0xd4
};

enum raid_sal_sub_opcode {
    /* 0xd0/0xd1/0xd2 */
    RAID_SAL_OPEN_PHY = 0X1,
    RAID_SAL_CLOSE_PHY = 0X2,
    RAID_SAL_LINK_RESET = 0X3,
    RAID_SAL_HARD_RESET = 0X4,
    RAID_SAL_NOTIFY = 0X5,
    RAID_SAL_ADD_DEVICE = 0X6,
    RAID_SAL_REMOVE_DEVICE = 0X7,
    RAID_SAL_COMMON_OPT = 0X8,
    RAID_SAL_SSP_MSG = 0X9,
    RAID_SAL_STP_MSG = 0XA,
    RAID_SAL_SMP_MSG = 0XB,
    RAID_SAL_MGMT_MSG = 0XC,
    RAID_SAL_QUERY_DEV_CNT = 0XD,  // ��ѯ˫�ػ�δע����������
    RAID_SAL_LOCATE_STATUS = 0xE,
    RAID_SAL_CTL_LED_STATUS = 0xF,
    RAID_SAL_EQUIP_OPEN_PHY = 0x10,
    RAID_SAL_QUERY_DEVICE = 0x11,
    RAID_SAL_QUERY_PF_DEVICE = 0x12,
    RAID_SAL_REMOVE_PF_DEVICE = 0x13,
    RAID_SAL_CFG_STREAM = 0x14,
    RAID_SAL_SET_DEVICE_STAT = 0x15,
    RAID_SAL_QUERY_DEVICE_STAT = 0x16,

    RAID_BIST_START = 0x20,
    RAID_BIST_RX_EN = 0x21,
    RAID_BIST_TX_EN = 0x22,
    RAID_BIST_CLOSE = 0x23,
    RAID_IN_LOOPBACK_OPEN = 0x24,
    RAID_IN_LOOPBACK_CLOSE = 0x25,
    RAID_OUT_LOOPBACK_OPEN = 0x26,
    RAID_OUT_LOOPBACK_CLOSE = 0x27,
    RAID_GET_ERROR_CNT = 0x28,
    RAID_GET_PHY_LINKRATE = 0x29,
    RAID_OPERATE_FFE = 0x2A,
    RAID_GET_CHIP_STATE = 0x2B,
    RAID_GET_PHY_STATE = 0x2C,

    RAID_SAL_SWITCH_MASTER = 0x80,
    RAID_SAL_ENABLE_PF = 0x81,
    RAID_SAL_CARD_DIF = 0x82,
    RAID_SAL_WSTRIP_RINSERT = 0x83,
    // Task Management Function
    RAID_SAL_TMF_CMD = 0x84,
    RAID_SAL_GPIO_CFG = 0x85,

    /* 0xd4 */
    RAID_SAL_IO_ABORT = 0x1,
    RAID_SAL_DEVICE_ABORT = 0x2,
    RAID_SAL_PF_ABORT = 0x3,
    RAID_SAL_DEVICE_RESET = 0x4,
    RAID_SAL_CTRL_RESET = 0x5,
    RAID_SAL_PRE_SOFT_RESET = 0x6,
    RAID_SAL_SOFT_RESET = 0x7,
    RAID_SAL_AEN_ABORT = 0x8
};

/*****************************************************************************
 Description  : �̼�������
 command      : toolname cx update fw file=xx bitmap=x
 Direction    : ADM_RAID_WRITE subopcode: ADM_CMD_UPDATE_FW 0x37
*****************************************************************************/
struct cmd_update_firmware {
    u16 seq;          // ��Ƭ��ţ�ÿ���������0��ʼ
    u16 tgt_bit_map;  // �����������bitmap
    u8 fw_type;       // ��ǰ�������, update_tgt_type_e
    u8 algo_type;     // �㷨����
    u8 trans_flag;    // ��ǰ��Ƭ����(��ʼ���в���β����ȫ��)
    u8 finish_flag;
    u32 tgt_len;        // ��ǰ���������padding���ܳ���
    u32 data_crc;       // ��ǰ����Ƭ������CRC32���
    u32 cur_slice_len;  // ��ǰ����Ƭ�����ݳ���(��С����  RAID:256KB; HBA:64KB�����ǵ�nvme����ÿ�δ������4K���̶�Ϊ4K)
};

/*****************************************************************************
 Description  : �̼�����
 command      : toolname cx active fw bitmap=x force={0|1} type={0|1}
 Direction    : ADM_RAID_SET subopcode: ADM_CMD_ACTIVE_FW 0x38
*****************************************************************************/
struct cmd_active_firmware {
    u16 act_bitmap;  //  �����������bitmap
    u8 is_force;     /* forceΪ1����ǿ�Ƽ���,0:�������� */
    u8 type;         /* active_type_e type=0   �ϵ缤�� type=1   �������� */
};

/*****************************************************************************
 Description  : ��ָ����д��ָ���ļ�����
 command      : toolname cx transport file name=xx offset=x
 Direction    : ADM_RAID_SET
*****************************************************************************/
struct cmd_transport_file {
    u8 trans_flag;  // ��ǰ��Ƭ����(ǰ3λ�ֱ��ʾ��ʼ���в���β������Ƭ�ܴ�����ʼ��β����λ)
    u8 rsvd;
    u16 cur_slice_len;  // ��ǰ����Ƭ�����ݳ���(��С����  RAID:256KB; HBA:64KB�����ǵ�nvme����ÿ�δ������4K���̶�Ϊ4K)
    u32 seq;            // ��Ƭ��ţ���0��ʼÿ�ε���1
    u32 slice_crc;      // ��ǰ����Ƭ������CRC32
    u32 file_len;       // �����ļ��Ĵ�С
    u32 offset;         // �ļ���Ҫ��д���ƫ�Ƶ�ַ
};

#define UART_REDIRECT_MAX_LOG_SIZE 128
/*****************************************************************************
 Description  : �����������ߣ����മ����־�ض���
*****************************************************************************/
struct cmd_uart_redirect {
    u16 char_num;  // ʵ�ʽ��պͷ��͵��ַ���
    u16 rsvd;
    char uart_data[UART_REDIRECT_MAX_LOG_SIZE];  // ���պͷ��͵��ַ�����
};

/*****************************************************************************
 Description  : ��ȡ�����ڴ����ݣ�RTOSCKû���ļ�ϵͳ���޷���ȡ�ļ���
 command      : toolname cx get fwdata start_addr=xx end_offset=xx save_path=xx
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_GET_FW_DATA
*****************************************************************************/
enum fwdata_blk_type {
    FIRST_BLK,
    LAST_BLK,
    OTHER_BLK
};

struct cmd_fwdata_param {
    u64 start_addr;
    struct {
        u32 rsvd : 6;
        u32 blk_type : 2;       // ���δ������ݿ�����:��һ��/���һ��/����
        u32 cur_blk_size : 24;  // ���δ������ݵĴ�Сbyte
    } data_info;
    u32 tag;  // �ļ��ض��������tag
};

/*****************************************************************************
 Description  : ��ͼ��ѯ
 command      : toolname cx show eye_diagram id=x
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_EYE_DIAGRAM
*****************************************************************************/
struct sds_com_phy_para {
    u8 phy_sds_id;  // 0 - 47
    u8 rsvd[3];
};

struct sds_four_eye_diagram {
    int eye_top; /* ˳��̶�Ϊ:�������� */
    int eye_bot;
    int eye_left;
    int eye_right;
};

/*****************************************************************************
Description  : TXԤ���ز��Բ���(set��show)
command      : toolname cx set sds_ffe id=x param=x,x,x,x,x
               toolname cx show sds_ffe id=x
Direction    : ADM_RAID_WRITE SUBOPCODE:ADM_CMD_SDS_SET_FFE(��ͨ��data����һ��������)
               ADM_RAID_READ SUBOPCODE:ADM_CMD_SDS_SHOW_FFE
*****************************************************************************/
struct sds_tx_fir_data {
    char fir_pre2;
    char fir_pre1;
    unsigned char fir_main;
    char fir_post1;
    char fir_post2;
    unsigned char rsvd[3];
};

/*****************************************************************************
Description  : RX ctle(set��show)
command      : toolname cx set sds_ctle id=x g=x,x,x,x b=x,x,x z=x,x,x s=x,x,x c=x,x,x r=x,x,x l=x,x p=x,x
               toolname cx show sds_ctle id=x
Direction    : ADM_RAID_WRITE SUBOPCODE:ADM_CMD_SDS_SET_CTLE
               ADM_RAID_READ SUBOPCODE:ADM_CMD_SDS_SHOW_CTLE
*****************************************************************************/
struct sds_ctle_data {
    u8 ctleactgn1;
    u8 ctleactgn2;
    u8 ctleactgn3;
    u8 ctleactgn4;
    u8 ctlebst1;
    u8 ctlebst2;
    u8 ctlebst3;
    u8 ctleza1;
    u8 ctleza2;
    u8 ctleza3;
    u8 ctlesqh1;
    u8 ctlesqh2;
    u8 ctlesqh3;
    u8 ctlecmband1;
    u8 ctlecmband2;
    u8 ctlecmband3;
    u8 ctlermband1;
    u8 ctlermband2;
    u8 ctlermband3;
    u8 rxctlerefsel;      // ������
    u8 ctlepassgn;        // ������
    u8 ctlehfzen1;        // ������
    u8 ctlehfzen2;        // ������
    u8 ctlehfzen3;        // ������
    u16 rxctleibiastune;  // ������
    u8 ctle_eq1_lunpk_enb;
    u8 ctle_gn1_lunpk_enb;
    u8 ctlepassattn;
    u8 ctlepasstune;
    u8 rsvd[2];
};

/*****************************************************************************
 Description  : RX dfe(set��show)
 command      : toolname cx set sds_dfe id=x param=x,x,x,x,,,  20 : TAP1cen��TAP1EDG��TAP2~9��FTAP1~6
                toolname cx show sds_dfe id=x
 Direction    : ADM_RAID_WRITE SUBOPCODE:ADM_CMD_SDS_SET_DFE
                ADM_RAID_READ SUBOPCODE:ADM_CMD_SDS_SHOW_DFE
*****************************************************************************/
struct sds_rx_dfe {
    /* 20 : TAP1cen��TAP1EDG��TAP2~9��FTAP1~6 */
    int tap_value[20];
};

/*****************************************************************************
 Description  : RX dfe(set��show)
 command      : toolname cx set sds_dfe id=x param=x,x,x,x,,,  20 : TAP1cen��TAP1EDG��TAP2~9��FTAP1~6
                toolname cx show sds_dfe id=x
 Direction    : ADM_RAID_WRITE SUBOPCODE:ADM_CMD_SDS_SET_SWING
                ADM_RAID_READ SUBOPCODE:ADM_CMD_SDS_SET_SWING
*****************************************************************************/
struct sds_tx_swing {
    u32 swing;
};

/*****************************************************************************
 Description  : PCIE����ep��·��ѯ
 command      : toolname cx show pcie_ep_link
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_PCIE_LINK
                ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_LINK_ERROR
*****************************************************************************/
struct pcie_ep_port {
    u32 port_id;
};

struct pcie_ep_state_get_rsp {
    u8 state;   // state:link״̬:1 ������0������
    u8 speed;   // speed:1~4����gen1��gen4
    u8 porten;  // porten:0,�ǲ�ʹ�ܣ�����״̬NA��1ʹ�ܣ��������ݲ���Ч��
    u8 width;   // width:��ʾ����,ֱ����ʾ
};

/*****************************************************************************
 Description  : PCIE ״̬����ѯ
                ״̬���
 command      : toolname cx show pcie_ltssm
                toolname cx clear pcie_ltssm
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_PCIE_LTSSM
                ADM_RAID_SET  SUBOPCODE:ADM_CMD_CLEAR_PCIE_LTSSM
*****************************************************************************/
#define LTSSM_TRACE_MAXNR 64
union pcie_ltssm_trace1 {
    struct {
        u32 ltssm_st : 6;                                     /* [5..0] */
        u32 duration_counter : 4;                             /* [9..6] */
        u32 mac_cur_link_speed : 2;                           /* [11..10] */
        u32 train_bit_map : 8;                                /* [19..12] */
        u32 rxl0s_st : 2;                                     /* [21..20] */
        u32 any_change_pipe_req : 1;                          /* [22] */
        u32 rcv_eios : 1;                                     /* [23] */
        u32 dl_retrain : 1;                                   /* [24] */
        u32 all_phy_rxeleidle_or_rx_skp_interval_timeout : 1; /* [25] */
        u32 directed_speed_change : 1;                        /* [26] */
        u32 any_det_eieos_ts : 1;                             /* [27] */
        u32 rxl0s_to_recovery : 1;                            /* [28] */
        u32 any_lane_rcv_speed_change : 1;                    /* [29] */
        u32 changed_speed_recovery : 1;                       /* [30] */
        u32 suceessful_speed_negotiation : 1;                 /* [31] */
    } bits;
    u32 val;
};

union pcie_ltssm_trace2 {
    struct {
        u32 train_bit_map : 16; /* [15..0] */
        u32 txdetrx : 16;       /* [31..16] */
    } bits;
    u32 val;
};

struct pcie_ltssm_rsp {
    u8 port_id;
    u8 trace_mode;
    u8 roll;
    u8 count;
    union pcie_ltssm_trace1 trace1[LTSSM_TRACE_MAXNR];
    union pcie_ltssm_trace2 trace2[LTSSM_TRACE_MAXNR];
};

/*****************************************************************************
 Description  : PCIE ����tx_margin
 command      : toolname cx set pcie_tx_margin id=x margin=x
 Direction    : ADM_RAID_SET SUBOPCODE:ADM_CMD_SET_TX_MARGIN
*****************************************************************************/
struct pcie_tx_margin {
    u32 port_id;
    u32 tx_margin;
};

/*****************************************************************************
 Description  : ��Nand/Nor flash �·��������󷵻ؽ��
 command      : toolname cx set flashtest type={nand|nor}
 Direction    : ADM_RAID_SET SUBOPCODE:ADM_CMD_HDTEST_FLASH
*****************************************************************************/
struct cmd_flash_test {
    u8 type;  // 0:ADM_CMD_FLASH_TEST_NAND 1:ADM_CMD_FLASH_TEST_NOR
    u8 rsvd[3];
};

/*****************************************************************************
 Description  :
 command      : toolname cx set bp_inject_die die=xx block=xx len=xx
 Direction    : ADM_RAID_SET SUBOPCODE:ADM_CMD_SET_BP_INJECT_DIE
*****************************************************************************/
struct cmd_bp_inject_die_unavail {
    u32 die;
    u32 block;
    u32 len;
};

/*****************************************************************************
 Description  :
 command      : toolname cx set bp_nand [type=xx] [addr=xx] [ch=xx] [ce=xx]
                [lun=xx] [plane=xx] [block=xx] [page=xx]
 Direction    : ADM_RAID_SET SUBOPCODE:ADM_CMD_SET_BP_NAND
*****************************************************************************/
struct cmd_bp_nand_operation {
    u32 op_type;
    u64 addr;
    u32 ch;
    u32 ce;
    u32 lun;
    u32 plane;
    u32 block;
    u32 page;
};

enum CMD_OP_TYPE_E {
    CMD_NAND_ERASE,
    CMD_NAND_RAW_WRITE,
    CMD_NAND_DATA_WRITE,
    CMD_NAND_RAW_READ,
    CMD_NAND_DATA_READ,
    CMD_NAND_ALL,
    CMD_NAND_INJECT_UNC,
    CMD_NAND_UID,
    CMD_NAND_OP_BUTT
};

/*****************************************************************************
 Description  :
 command      : toolname cx set bpfs [type = xx]
                [name=xx] [len=xx] [path=xx]
 Direction    : ADM_RAID_SET SUBOPCODE:
*****************************************************************************/
enum CMD_SET_FS_TYPE_E {
    FS_INIT,
    FS_OPEN,
    FS_CLEAN,
    FS_WRITE,
    FS_BUTT
};

struct cmd_bp_fs_operation {
    u32 type;
    u32 name;
    u32 seek;
    u32 skip;
    u32 size;
    u32 trans_len;
};

struct cmd_fs_size {
    u32 expect_size;
    u32 real_size;
};

/*****************************************************************************
 Description  :
 command      : toolname cx get bpfs_info [type = xx]
                [name=xx] [len=xx] [path=xx]
 Direction    : ADM_RAID_SET SUBOPCODE:
*****************************************************************************/
enum CMD_GET_BP_TYPE_E {
    FS_READ,
    BP_META,
    BP_FS,
    DATA_READ,
    BUTT
};

/*****************************************************************************
 Description  :
 command      : toolname cx set bpfun [type=xx]
 Direction    : ADM_RAID_SET SUBOPCODE:
*****************************************************************************/
typedef enum {
    BP_INIT,
    BP_RESTORE,
    BP_CLEAN,
    BP_BACKUP,
    FUN_BUTT
} CMD_FUN_TYPE_E;

struct cmd_bp_hba_function {
    u8 type;
    u8 rsv[3];
};

/*****************************************************************************
 Description  :
 command      : toolname cx get metadata addr=xx path=xx
 Direction    : ADM_RAID_GET SUBOPCODE:
*****************************************************************************/
typedef enum {
    CMD_GET_INFO,
    CMD_GET_DATA,
} CMD_METD_TYPE_E;

typedef struct {
    u16 load_finish;    // �ؿ�������־
    u16 data_valid;     // ���ݻؿ���Ч��
    u32 phy_ppn;        // ���ݵ�����ppn
    int ret;            // ppn�ؿ����
    int res;            // ppn�ؿ����
} cmd_ret_info;

typedef struct {
    u32 sn;
    u32 data_len;
    cmd_ret_info main;   // ����
    cmd_ret_info back;   // ����
} cmd_meta_result;

typedef struct {
    u32 start_flg;       // ��ʼ��־
    u32 cmd_type;        // ��������
    u64 sn;              // sn�ŷ���������
    u64 addr;            // �ؿ���ַ
} cmd_metadata_info_req;

/*****************************************************************************
 Description  :
 command      : toolname cx set bp_dqs_deflect dir={left|right}
 Direction    : ADM_RAID_SET SUBOPCODE:ADM_CMD_SET_BP_DQS_DEFLECT
*****************************************************************************/
struct cmd_bp_dqs_deflect_dir {
    u8 dir;
    u8 rsv[3];
};

enum CMD_DQS_DEFLECT_DIR {
    CMD_DQS_DEFLECT_LEFT,
    CMD_DQS_DEFLECT_RIGHT
};

#define CHIP_NUM_MAX 5
#define CHIP_DIEID_WORD_NUM 5
#define CHIP_NAME_LEN_MAX 7
#define CHIP_VERSION_LEN_MAX 8
#define CHIP_LOCATION_LEN_MAX 8
/*****************************************************************************
 Description  : ��ȡоƬdieid����
 command      : toolname cx show dieid
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_DIEID
*****************************************************************************/
struct chip_dieid {
    u8 chip_id;
    u8 chip_name[CHIP_NAME_LEN_MAX];
    u8 chip_version[CHIP_VERSION_LEN_MAX];
    u8 chip_location[CHIP_LOCATION_LEN_MAX];
    u32 dieid_data[CHIP_DIEID_WORD_NUM];
};
struct cmd_chip_dieid {
    u32 chip_num;
    struct chip_dieid chip_data[CHIP_NUM_MAX];
};

/*****************************************************************************
 Description  : ���ж�λ�ṹ��
*****************************************************************************/
struct cmd_rg_id {
    u16 rg_id;
    u8 rsvd[2];
};

struct cmd_vd_id {
    u16 vd_id;
    u8 rsvd[2];
};

struct cmd_expander_id {
    u16 exp_id;
    u8 rsvd[2];
};

enum disk_locate_flag_value {
    DISK_LOC_ENC_SLOT,
    DISK_LOC_DID
};

struct disk_location {
    u16 enc_id;   // ���
    u16 slot_id;  // ��λ��
};

struct multi_disk_location {
    u16 enc_id;   //     ���
    u16 slot_id;  //     ��λ��
    u16 did;      //     �߼���
    u8 flag;      //     0:DISK_LOC_ENC_SLOT 1:DISK_LOC_DID, �����·�ʱȷ�ϱ��ζ�λ��ʽ
    u8 rsvd;
};

/*****************************************************************************
 Description  : ��ȡ��������б�
 command      : toolname cx show pdlist
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_PDLIST
*****************************************************************************/
struct cmd_pdlist_idx {
    u16 start_idx;  // ���λ�ȡ������Ϣ��ʼindex
    u16 count;      // ��ѯ����
    u32 rsvd;       // ����Ԥ��
};

struct cmd_pdlist_entry {
    u16 enc_id;     // ���̿��
    u16 slot_id;    // ���̲�λ��
    u8 interface;   // �����̵Ľӿ����ͣ�SAS EXP,SATA,PLANE,NVME
    u8 media_type;  // ���̽�������: HDD,SSD
    u8 logi_type;
    u8 logi_status;
    u16 did;  // ����ID
    u8 physi_status;
    u8 link_speed;     // ʵ�������ٶ�
    u32 lbs;           // �������߼�������С
    u32 pbs;           // ����������������С
    u64 raw_capacity;  // �������ԭʼ����
    u8 model[40 + 1];  // �����ͺţ������ַ���������
    u8 sed_disk;       // �������Ƿ�Ϊ������:YES,NO
    u8 dif_info;       // Bit3-bit0:dif���ͣ�   Bit7-bit4:  dif size
    u8 sp;             // ��������ת״̬ spin down/spin up
    u8 fw[32];         // �̼��汾��Ϣ
    u8 halflife;
    u8 support_flag;    // ����֧�ֵ�����   bit0:fua֧��, bit1:�Ƿ�����, bit2:�Ƿ�Ϊsmr��
    u8 rsvd0[2];
};
#define CMD_PDS_MAX_NUM 256U
#define CMD_PDLIST_ONCE_NUM ((ADM_MAX_DATA_SIZE - 64) / sizeof(struct cmd_pdlist_entry))
/*****************************************************************************
 Description  : ��������б��ѯ���
 command      : toolname cx show pdlist
 Direction    : opcode=0xC2  subopcode=ADM_CMD_GET_PDLIST_NUM
*****************************************************************************/
struct cmd_show_pdlist {
    u16 num;                                             // ��������
    u16 rsvd0;                                           // ����Ԥ��
    u32 rsvd1[15];                                       // Ԥ��
    struct cmd_pdlist_entry disks[CMD_PDLIST_ONCE_NUM];  // ����4Kҳ������ܻ�ȡ��entry(��ͷ)
};

/*****************************************************************************
 Description  : ��ȡ�����������
 command      :
 Direction    : opcode=0xC2  subopcode=ADM_CMD_GET_PDLIST_NUM
*****************************************************************************/
struct cmd_disk_num {
    u16 num;     // ��λ��������
    u8 rsvd[2];  // ����Ԥ��
};

/*****************************************************************************
 Description  : ��ȡ���̵���ϸ��Ϣ
 command      : toolname cx:ex:sx show
 Direction    : opcode=0xC2  subopcode=0x47
*****************************************************************************/
struct cmd_show_disk {
    u16 enc_id;
    u16 slot_id;
    u16 did;
    u16 asso_enc_id;
    u16 asso_slot_id;
    u16 asso_did;
    u8 rg_id;
    u8 rg_level;           // Ӳ������raid�鼶��
    u8 logi_type;          // �߼�����
    u8 logi_status;        // ������״̬
    u64 rg_capacity;       // Disk������raid���������С
    u64 disk_WWN;          // �̱�ʶ,16����չʾ
    u8 hibernated_status;  // Ӳ������״̬��
    u8 hsp_status;         // �ȱ�״̬��0:���ȱ��̣�1:ȫ���ȱ���2:�ֲ��ȱ�
    u8 is_boot;            // �Ƿ�������
    u8 interface;          // Ӳ�̽ӿ����ͣ�
    u16 physi_id;
    u8 media_type;
    u8 physi_status;
    u32 lbs;
    u32 pbs;
    u8 disk_max_speed;  // �̵����֧������
    u8 device_speed;    // �̵�Э�����ʣ�
    u8 is_sed;          // �Ƿ�Ϊ������
    u8 dif_info;
    u8 connected_port_id;  // ���ӵĶ˿ں�
    u8 linked_port_speed;  // ���ֱ���̣������̵�Э�����ʣ�����expander�����ǿ���expander��Э������
    u8 disk_cache;
    u8 drive_temperature;  // Ӳ�̵�ǰ�¶�
    u8 halflife;
    u8 media_error_count;
    u8 other_error_count;
    u8 sed_enable;   // �Ƿ���������
    u8 drive_size;   // Ӳ�̳ߴ磬
    u8 locate_led;   // 1 on�� 0 off
    u8 ncq_support;  // Ӳ�̶���֧�����
    u8 ncq_enable;   // Ӳ�̶������û��߽���
    u64 raw_capacity;
    u64 sas_address;
    u64 power_on_time[2];
    u8 model[40 + 1];
    u8 vendor[32];
    u8 serial_num[32];
    u8 fw_version[32];
    u16 rotation_rate;  // 0:δ�ϱ�, 1 non-rotating medium ����ת����, ����ֵ��ʾת��
    u8 wearout;         // SSD������Ԥ��
    u8 action;          // bit0:�ع���bit1:Ѳ�죬bit2:�ؿ���bit3:����
    u16 left_time;      // ����ʣ��ʱ��
    u8 progress;        // ������ȣ�0-100
    u8 support_flag;    // ����֧�ֵ�����   bit0:fua֧��, bit1:�Ƿ����̣�bit2:�Ƿ�Ϊsmr��
};

/*****************************************************************************
 Description  : ���õ������̵�Ѳ�쿪�غͲ���
 command      : toolname cx:ex:sx show phy
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SET_DISK_PATROLREAD
*****************************************************************************/
struct cmd_set_disk_patrolread {
    struct multi_disk_location loc;
    u8 sw;  // 0: off  1:on ��start��
    u8 rate;
    u8 rsvd[2];
    u32 delay;  // ��λСʱ
};

/*****************************************************************************
 Description  : ��ѯ�������̵�Ѳ��״̬
 command      : toolname cx:ex:sx show phy
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SET_DISK_PATROLREAD
*****************************************************************************/
struct cmd_show_disk_patrolread {
    u16 left_time;  // ʣ��Ԥ��ʱ�䣬��λ��min
    u8 rate;
    u8 run_status;
    u8 progress;
    u8 delay;   // �ӳ�ʱ�䣬��λh
    u16 magic;  // CMD_MAGIC_FLAG ����֮ǰ�汾�������־
};

/*****************************************************************************
 Description  : ��ʾ��\expander�ض��˿ڵ�phy��Ϣ
 command      : toolname cx:ex:sx show phy
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_PHY_STATUS
*****************************************************************************/
struct cmd_show_phy_entry {
    u8 phy_id;
    u8 phy_enable;
    u8 link_speed;  // �˿���������
    u8 attach_dev_type;
    u32 rsvd0;
    u64 attach_sas_addr;
    u64 invalid_dword_cont;
    u64 run_disparity_err_cont;
    u64 loss_of_dword_syn_cont;
    u64 phy_reset_pro_cont;
    u32 rsvd[2];
};

/*****************************************************************************
 Description  : ��ʾ�̲��������Ϣ
 command      : toolname cx:ex:sx show phy
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_DISK_PHY_EEROR
*****************************************************************************/
struct cmd_show_disk_sas_error {
    u64 invalid_dword_cont;
    u64 run_disparity_err_cont;
    u64 loss_of_dword_syn_cont;
    u64 phy_reset_pro_cont;
};

#define MAX_SATA_PHY_ERR_EVENT 16
struct cmd_show_disk_sata_error {
    u64 err_val[MAX_SATA_PHY_ERR_EVENT];
};

struct cmd_show_disk_error {
    u8 disk_type;
    u8 dev_present;
    u8 link_speed;  // Э������
    union {
        struct cmd_show_disk_sas_error sas_error;
        struct cmd_show_disk_sata_error sata_error;
    };
};

/*****************************************************************************
 Description  : ��ʾĳ��expander��phy״̬
 command      : toolname cx:ex show phy
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_EXP_PHY_STATUS
*****************************************************************************/
struct cmd_show_exp_phy_status {
    u8 phy_num;  // ��expander��phy������
    u8 rsvd[7];
    struct cmd_show_phy_entry phys[MAX_PHY_NUM];
};

/*****************************************************************************
 Description  : ��ѯ�����̰�ȫ��������
 command      : toolname cx:ex:sx show sanitize
 Direction    : opcode=0xC0  subopcode=ADM_CMD_SHOW_SANITIZE_PROGRESS
*****************************************************************************/
struct cmd_sanitize_progress {
    u8 progress;  // 0-100
    u16 left_time;
    u8 rsvd[5];
};

/*****************************************************************************
 Description  : ���ô��̰�ȫ����
 command      : toolname cx:ex:sx start sanitize mode={cryptoerase|overwrite|blockerase}
 Direction    : opcode=0xC0  subopcode=ADM_CMD_SET_DISK_SANITIZE
*****************************************************************************/
struct cmd_set_disk_sanitize {
    struct multi_disk_location loc;
    u8 mode;        // ����ģʽ, overwrite - 1, blockerase - 2, cryptoerase - 3
    u8 rsvd[7];     // ����Ԥ��
};

/*****************************************************************************
 Description  : ���ô��̰�ȫ����
 command      : toolname cx:ex:sx start erase mode={simple| normal| thorough} [pa=x] [pb=x]
 command      : toolname cx:ex:sx stop erase
 Direction    : opcode=0xC0  subopcode=ADM_CMD_SET_DISK_ERASE
*****************************************************************************/
struct cmd_set_disk_erase {
    struct multi_disk_location loc;
    u8 sw;          // �򿪻�ر� 1 ADM_NEW_SWITCH_ON ����2 ��
    u8 mode;        // simple -1 | normal -2| thorough -3
    u8 pattern[2];  // ��������
    u8 rsvd[12];    // ����Ԥ��
};

/*****************************************************************************
 Description  : ����Ӳ�̵Ƶ�״̬
 command      : toolname cx:ex:sx set led
 Direction    : opcode=0xC0  subopcode=ADM_CMD_SET_DISK_LED
*****************************************************************************/
struct cmd_set_disk_led {
    struct multi_disk_location loc;
    u8 led_type;  // ������� Locate:1 /err:0
    u8 sw;        // 0:�أ�1:��
    u8 rsvd[2];   // ����Ԥ��
};

enum cmd_disk_led_type {
    CMD_LED_TYPE_ERR,
    CMD_LED_TYPE_LOCATE
};

enum cmd_led_light_state {
    CMD_LED_STATUES_OFF,
    CMD_LED_STATUES_ON
};

/*****************************************************************************
 Description  : ��ȡ����did
 command      : toolname cx:ex:sx show did
 Direction    : opcode=READ  subopcode=ADM_CMD_GET_DISK_DID
*****************************************************************************/
struct cmd_disk_did {
    u16 did;
    u16 rsvd;
};
#define ADM_CREATE_RG_BIT_PDCACHE_UNSUP 1 /* LUN����PDcacheͬ�����в�֧�ֵ��̣�ͨ��result0�ĵڶ���bit���� */
/*****************************************************************************
 Description  : �����߼���
 command      : toolname cx create vd
 Direction    : opcode=0xC0  subopcode=ADM_CMD_CREATE_VD
*****************************************************************************/
struct cmd_create_vd {
    u8 name[MAX_VD_NAME_LEN];  // ���ƣ�����������
    u8 rsvd1[8];
    u64 size;  // �߼��̴�С����λ MB���������ݱ�����������ȡֵ��Χ:0MB��RAID��ʣ��������ȫF��ʾȫ��ʣ��������
    u32 strip_unit_size;  // ������С����λKB��Ĭ��ֵ32KB��enum adm_susize
    u8 init;  // ��ʼ�����ͣ�enum adm_init_type,fast:���ٳ�ʼ����Ĭ��ֵ����front:ǰ̨��ʼ����background:��̨��ʼ��
    u8 cold_hot;  // VD��������: 2:Ĭ��ֵ�����������ԣ�1:��lun��0:��LUN
    u8 wcache;  // VDдcache���ԡ�ȡֵΪ:��1��WB д�أ���2��WT д����Ĭ��ֵ������3��WBE ǿ��д�أ�
    u8 rcache;    // VDԤ�����ԡ�ȡֵΪ:��0��No_Ahead ��Ԥ����Ĭ��ֵ������1��Ahead Ԥ��
    u8 map_type;  // SCSI: ADM_RAID_SCSI(0) NVMe : ADM_RAID_NVME(1)
    u8 rg_id;  // RAID��ID�����֧��64��RAID�顣���������һ��RG֮�󣬿���ͨ��ָ��RAID�������LUN
    u8 rg_id_used;       // ture: rd_id used, false: rg_id unused
    u8 level;            // vd����RAID�鼶��ȡֵΪenum raid_group_level
    u8 member_disk_num;  // ��Ա�����������,
    u8 sg_disk_num;      // RAID����ĳ�Ա������
    u8 disk_cache_sw;    // VD����Ӳ��дCache����:enum adm_disk_cahe_switch
    u8 pf;               // AUTO = 2; PF0 = 0; PF1 = 1;�����ָ��RAID���ϴ�������PF���Ը���raid������ԣ���1��RAID�ϣ����е�VD���ϱ���ͬһ��  Port��
    u8 locationtype;  // 0: ʹ��EID/SID��λ 1 :ʹ��DID��λ��
    u8 rsvd[7];
    union {
        struct disk_location disks[CMD_MEMB_DISK_MAX_NUM];  // ��Ա��λ����Ϣ��
                                                                    // ���location_type = 0, ��ʹ��struct disk_location,
                                                                    // ����ʹ��u 16 did[80];
        u16 did[CMD_MEMB_DISK_MAX_NUM];
    };
};

/*****************************************************************************
 Description  : ɾ���߼���
 command      : toolname cx:vdx delete
 Direction    : opcode=0xC0  subopcode=ADM_CMD_DELETE_VD
*****************************************************************************/
struct cmd_del_vd {
    struct cmd_vd_id loc;
    u8 discardcache;  // 0: default 1: force 2: discardcache
    u8 rsvd[3];       // ����Ԥ��
};

/*****************************************************************************
 Description  : �߼����б�չʾ
 command      : toolname cx show vdlist
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_VDLIST
*****************************************************************************/
struct cmd_vd_list_idx {
    u16 start_idx;  // ��ʼvd index
    u16 count;      // ��ѯ���������
    u32 rsvd;       // ����Ԥ��
};

struct cmd_vdlist_entry {
    u8 rg_id;
    u8 vd_status;
    u8 map_type;
    u8 vd_type;
    u16 vd_id;
    u16 su_size;
    u8 vd_name[32];
    u64 capacity;
    u32 sec_size;
    u8 cold_hot;
    u8 rcache;
    u8 wcache;
    u8 pf;
    u8 realwcache;
    u8 access_policy;  // ��д���� enum adm_vd_access_policy
    u16 magic;         // ���߰汾����ħ���ֶ�
    u8 rsvd[4];        // ����Ԥ��
};
#define CMD_VDS_MAX_NUM 512U     // ��ǰ����raidģʽ���Ϊ512��hbaģʽΪ4
#define CMD_VDLIST_ONCE_NUM ((ADM_MAX_DATA_SIZE - 64) / sizeof(struct cmd_vdlist_entry))
struct cmd_show_vdlist {
    u16 num;                                           // ��������
    u16 rsvd0;                                         // ����Ԥ��
    u32 rsvd1[15];                                     // Ԥ��
    struct cmd_vdlist_entry vds[CMD_VDLIST_ONCE_NUM];  // ����4Kҳ������ܻ�ȡ��entry
};

/*****************************************************************************
 Description  : ��ȡ�߼�������
 command      :
 Direction    : opcode=0xC2  subopcode=ADM_CMD_GET_VDLIST_NUM
*****************************************************************************/
struct cmd_get_vd_num {
    u16 num;     // �߼���vd����
    u8 rsvd[2];  // ����Ԥ��
};

/*****************************************************************************
 Description  : ����raid���ؿ�
 command      : toolname cx set copyback
 Direction    : opcode=0xC0  subopcode=ADM_CMD_SET_COPYBACK_SW
                opcode=0xC0  subopcode=ADM_CMD_SHOW_COPYBACK_SW
*****************************************************************************/
struct cmd_copyback_status {
    u8 copyback_status;  // ����״̬��0�أ�1��
    u8 rsvd[3];          // ����Ԥ��
};

/*****************************************************************************
 Description  : ��ѯ������bootwithpinnedcache����
 command      : toolname cx set bootwithpinnedcache sw={on|off}
 Direction    : opcode=0xC0  subopcode=ADM_CMD_SET_BOOT_PINNEDCACHE
                opcode=0xC0  subopcode=ADM_CMD_SHOW_BOOT_PINNEDCACHE
*****************************************************************************/
struct cmd_set_pinnedcache {
    u8 sw;               // ����״̬��0�أ�1��
    u8 rsvd[3];          // ����Ԥ��
};

/*****************************************************************************
 Description  : ���ÿ��ز������ʺϵ�һobj��λ��ʽ
*****************************************************************************/
struct cmd_comn_switch_status {
    u32 rsvd0;     // ����һobjʹ��
    u8 sw_status;  // ����״̬��0�أ�1��
    u8 rsvd[3];    // ����Ԥ��
};

/*****************************************************************************
 Description  : Ԥ��������
 command      : toolname cx set precopy
 Direction    : opcode=0xC0  subopcode=ADM_CMD_SET_PRECOPY_SW
                opcode=0xC0  subopcode=ADM_CMD_SHOW_PRECOPY_SW
*****************************************************************************/
struct cmd_precopy_status {
    u8 precopy_status;  // ����״̬��0�أ�1��
    u8 rsvd[3];         // ����Ԥ��
};

/*****************************************************************************
 Description  : ��ѯϵͳѲ��״̬
 command      : toolname cx show patrolread
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_PATROLREAD
*****************************************************************************/
struct cmd_show_patrolread {
    u8 period;            // ���ڣ���λ:�죬ȡֵ��Χ[1:60]
    u8 sw;                // ����״̬
    u8 rate;              // ���ʣ�low|middle|high
    u8 disk_num;          // ��������������
    u8 run_status;        // �Ƿ�������Ѳ��,run��stop
    u8 finish_disks_sum;  // ����Ѳ���������
    u8 run_disks_sum;     // ����Ѳ���������
    u8 delay;             // �ӳ�ʱ�䣬��λh
    u16 magic;            // CMD_MAGIC_FLAG ����֮ǰ�汾�������־
    u8 rsv[2];            // ����Ԥ��
};

/*****************************************************************************
 Description  : �����ȱ�����״̬
 command      :
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SET_EHSP_SW
                opcode=0xC2  subopcode=ADM_CMD_SHOW_EHSP_SW
*****************************************************************************/
struct cmd_emergency_hsp {
    u8 ehsp_status;  // ����״̬��0:�أ�1:��
    u8 rsvd[3];      // ����Ԥ��
};

/*****************************************************************************
 Description  : raid������ֱͨ����
 command      :
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SET_PASSTHRU_TYPE
                opcode=0xC2  subopcode=ADM_CMD_SHOW_PASSTHRU_TYPE
*****************************************************************************/
struct cmd_passthru_type {
    u8 passthru_type;  // 0:�� ADM_CARD_PASSTHRU_OFF��1:JBOD ADM_CARD_PASSTHRU_JBOD 2:RAWDRIVE ADM_CARD_PASSTHRU_RAWDRIVE
    u8 rsvd[3];        //  ����Ԥ��
};

/*****************************************************************************
 Description  : DDMA����
 command      :
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SET_DDMA
                opcode=0xC2  subopcode=ADM_CMD_SHOW_DDMA
*****************************************************************************/
struct cmd_ddma {
    u8 ddma_status;  //  0:�أ�1:��
    u8 rsvd[3];      //  ����Ԥ��
};

/*****************************************************************************
 Description  : ��ʾraid��������Ϊ�����̵��������Ϣ
 command      :
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_BOOT
*****************************************************************************/
struct cmd_show_boot_entry {
    u16 id;   //  vid��DID
    u8 type;  //  vd/pd
    u8 rsvd0;
    struct disk_location disk;  //  �����������ʹ��
    u8 rsvd1[4];
};

#define BOOT_ENTRY_MAX 20
struct cmd_show_boot {
    u16 num;   // ����������
    u16 rsvd;  // ����Ԥ��
    struct cmd_show_boot_entry disks[BOOT_ENTRY_MAX];
};

/*****************************************************************************
 Description  : ��ѯ��ǰϵͳ�ĵ�������������Ϣ
 command      :
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_FNCFG
                opcode=0xC2  subopcode=ADM_CMD_CLEAR_FNCFG
                opcode=0xC2  subopcode=ADM_CMD_IMPORT_FNCFG
*****************************************************************************/
struct cmd_fncfg_param {
    u64 rg_guid;    // ���� all (ȫF) / guid ͨ��raid��Ψһ��ʶguid��λ��������
    u64 rsvd[2];
};

/*****************************************************************************
 Description  : ��ѯ��ǰϵͳ��������ϸ����������Ϣ
 command      :
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_FNCFG
*****************************************************************************/
struct cmd_show_fncfg_entry {
    u16 enc;         //  ��ǰforeign������Ч��Ӳ�̿��
    u16 slot;        //  ��ǰforeign������Ч��Ӳ�̲�λ��
    u8 status;       //  ��ǰ���ô��̵�״̬
    u8 type;         //  ��ǰ���̵����ͣ�free,member,spare,PaThrough,RawDisk
    u8 rg_id;        //  ��ǰforeign���ô������ڵ�raid��ID
    u8 rg_level;     //  ��ǰforeign���ô������ڵ�raid��ȼ�
    u8 raid_status;  //  ��ǰforeign���ô������ڵ�raid��״̬��normal,fault,degrade
    u8 rsvd[3];
    u64 rg_guid;     //  ����raid���Ψһ��ʶ
    u16 vd_count;    //  ����raid�鴴�����߼�������
    u16 pd_count;    //  ����raid��ʹ�õ�����������
    u8 rsvd1[16];
};

/*****************************************************************************
 Description  : ��ѯ��ǰϵͳ����������������Ϣ�б�
 command      :
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_FNCFGLIST
*****************************************************************************/
#define CMD_FNCFGLIST_MAX_NUM 240U
#define CMD_FNCFGLIST_ONCE_NUM ((ADM_MAX_DATA_SIZE - 64U) / sizeof(struct cmd_show_fncfg_entry))
struct cmd_fncfglist_idx {
    u16 start_idx;      // ���λ�ȡ��Ϣ��ʼindex
    u16 count;          // ��ѯ����
    u32 rsvd;           // ����Ԥ��
};

struct cmd_show_fncfglist {
    u16 num;                                                // ��������
    u16 rsvd0;                                              // ����Ԥ��
    u32 rsvd1[15];                                          // Ԥ��
    struct cmd_show_fncfg_entry cfgs[CMD_FNCFGLIST_ONCE_NUM];
};

/*****************************************************************************
 Description  : BMCʹ�ã�һ�λ�ȡ���������̵Ŀ�Ų�λ�ź��߼��ţ�����ȡ��ϸ��Ϣ
 command      :
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_PDS_ID
*****************************************************************************/
struct pds_disk_location {
    u16 enc_id;                         // ���
    u16 slot_id;                        // ��λ��
    u16 did;                            // �߼���
    u16 rsvd;
};

#define PDS_ID_NODE_NUM         256
struct cmd_pds_id {
    struct pds_disk_location ids[PDS_ID_NODE_NUM];
    u16 num;                            //  ��ǰ�����������
    u8 rsvd[2];
};

/*****************************************************************************
 Description  : ��ȡ��̨������Ϣ
 command      : toolname cx show bgtask
 Direction    : opcode=ADM_RAID_READ  subopcode=ADM_CMD_SHOW_BGTASK
*****************************************************************************/
#define CMD_MAX_REALTIME_BGTASK_NUM 32
struct bgtask_info {
    u8 type;
    u8 progress;  // �������н���,������ʾ�ٷֱ�
    u8 rate;
    u8 rsvd0;  // ����Ԥ��
    u16 vd_id;
    u16 time_left;
    u8 rsvd1[4];  // ����Ԥ��
};

struct cmd_show_bgtask {
    u8 sw;
    u8 task_num;
    u8 rsvd[6];  // ����Ԥ��
    struct bgtask_info bgtask[CMD_MAX_REALTIME_BGTASK_NUM];
};

/*****************************************************************************
 Description  : raid���б�չʾ
 command      : toolname cx show rglist
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_RGLIST
*****************************************************************************/
struct cmd_rglist_idx {
    u16 start_idx;  // ��ʼrg index
    u16 count;      // ���λ�ȡ��������
    u32 rsvd;       // ����Ԥ��
};

struct cmd_rglist_entry {
    u8 name[32];         // ����
    u8 rg_id;            // raid��ID
    u8 level;            // raid��ȼ�
    u8 status;           // raid��״̬
    u8 member_disk_num;  // ��Ա������
    u64 total_capacity;  // raid��������
    u64 free_capacity;   // raid��ʣ������
    u8 rsvd[8];  // ����Ԥ��
};

#define CMD_RGS_MAX_NUM 64U     // ��ǰ����raidģʽ���Ϊ64��hbaģʽΪ4
#define CMD_RGLIST_ONCE_NUM ((ADM_MAX_DATA_SIZE - 64) / sizeof(struct cmd_rglist_entry))
struct cmd_show_rglist {
    u16 num;
    u16 rsvd0;                                         // ����Ԥ��
    u32 rsvd1[15];                                     // ����Ԥ��
    struct cmd_rglist_entry rgs[CMD_RGLIST_ONCE_NUM];  // ����4Kҳ������ܻ�ȡ��entry
};

/*****************************************************************************
 Description  : ��ʾ��ǰraid�����ϸ��Ϣ
 command      : toolname cx:rgx show
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_RG_INFO
*****************************************************************************/
struct rg_block_info {
    u64 capacity;   // ����
    u64 start_lba;  // �ڵ�����������������ʼλ��
    u64 end_lba;    // �ڵ�������������������λ��
    u16 vd_id;      // vd��ID��Ϣ(��ʹ�õ�ʱ����Ч)
    u8 used_flag;   // �Ƿ���� 0:���У�1:used
    u8 rsvd;
};

struct member_disk_location {
    u16 enc_id;   // ���
    u16 slot_id;  // ��λ��
    u16 did;      // ����id
    u8 sg_index;  // �������� �������ȱ��� 0xff��ʾ��Ч��
    u8 rsvd0;
};

#define BLOCK_INFO_MAX 36
struct cmd_rg_info {
    u8 rg_id;            // Raid��ID
    u8 level;            // Raid����
    u8 status;           // Raid��״̬
    u8 sg_num;           // ��������
    u8 sg_disk_num;      // ���������
    u8 member_disk_num;  // ��Ա������
    u8 copyback_sw;      // �ؿ�����
    u8 disk_cache_sw;    // on off deault
    u16 vd_list[32];                                                         // vd�б�
    u32 sec_size;                                                            // ������С��B��
    u16 aligned_size;                                                        // �����С��MB��
    u16 vd_num;                                                              // VD����
    u8 name[32];                                                             // Raid������
    u64 total_capacity;                                                      // ������
    u64 free_capacity;                                                       // ��������
    u64 sg_free_capacity;                                                    // ����ʣ����������λB
    u16 rsvd2[3];
    u8 hsp_num;                                                              // �ȱ�������
    u8 background_task;                                                      // ��ǰ��̨����
    struct member_disk_location disks[CMD_MEMB_DISK_MAX_NUM];        // ��Ա����Ϣ
    struct member_disk_location hsp_disks[MAX_RG_HOTSPARE_DISK_NUM];  // ��Ա����Ϣ
    u8 free_block_num;                                                       // raid���Ͽ�����������
    u8 block_num;                   // raid���ϵ�block���������ڴ�����ɾ��VD����RAID�������µķ������ռ䣩
    u8 sec_type;                          // �������ͣ�enum adm_vd_sec_type_value
    u8 rsvd0;
    struct rg_block_info block_info[BLOCK_INFO_MAX];  // RG �ϵ�block��Ϣ
    u32 rsvd1[5];                         // Ԥ��
};

/*****************************************************************************
 Description  : ��ʾ�߼���VD����ϸ��Ϣ
 command      : toolname cx:vdx show
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_VD_INFO
*****************************************************************************/
struct vd_block_info {
    u16 id;                // lun id��Ϣ
    u16 next_id;           //  ��һ��lun id��Ϣ�����id��VD�Ƿ���ͳһ��ţ�
    u16 rsvd[2];           // ���뱣��
    u64 capacity;          // ÿ��lun������
    u64 start_lba_ondisk;  // �ڵ����������ϵ���ʼLBA��ַ
    u64 sec_num_ondisk;    // �ڵ����������ϵ���������
};

#define BLOCK_NUM_MAX 16
struct cmd_vd_info {
    u8 name[32];  // vd ����
    u16 id;
    u8 rg_id;
    u8 rg_level;
    u8 sg_num;       // ��������
    u8 sg_disk_num;  // ÿ�������������
    u8 vd_status;
    u8 vd_type;   // NORMAL_LUN = 0,PRIVATE_LUN = 1, EXPAND_LUN = 2
    u8 cold_hot;  // VD��������:��1��2:Ĭ��ֵ�����������ԣ���2��1:��lun����3��0:��LUN
    u8 map_type;  // ST NT NA
    u8 config_port;                       // �����ϱ�PF����
    u8 work_port;                         // ��ǰ�ϱ���PF����
    u16 stripe_size;                      // ������С��KB
    u16 su_size;                          // ������С��KB
    u64 capacity;                         // ��λMB��
    u32 sec_size;                         // ������С
    u8 sec_type;                          // �������ͣ�enum adm_vd_sec_type_value
    u8 rcache;                            // No_Ahead Ahead NA
    u8 wcache;                            // ��ǰ����״̬: WB WT WBE NA
    u8 real_wcache;                       // ʵ�ʹ���״̬: WB WT WBE NA
    u8 init_rate;                         // high medium low
    u8 pr_rate;                           // patrolread high medium low
    u8 init_progress;                     // 0-100
    u8 sanitize_progress;                 // 0-100
    struct vd_block_info block_info[BLOCK_NUM_MAX];  // 1��VD��������չ7�Σ�Ԥ����16��
    u8 block_num;                         // vd�����ݿ�������normal ֻ��1����expander LUN�����ж��.
    u8 dirty_cache;                       // no,yes
    u8 is_boot;                           // �Ƿ�����Ϊ�����߼���
    u8 access_policy;                     // ��д���� enum adm_vd_access_policy
    u8 rsvd[2];                           // ����Ԥ��
    u16 magic;                            // ���߰汾����ħ���ֶ�
    u64 wwn_low;                          // LUN��WWN��8���ֽڣ��ɳ����Զ���
    u64 wwn_high;  // LUN��WWN��8���ֽڣ����4bit��ʾNAA���ͣ�ʣ��60bit��ʾIEEE�г���ID
};

/*****************************************************************************
 Description  : ��ʾRG����VD�ĳ�Ա������
 Direction    : opcode=ADM_RAID_READ_FROM_CQE  subopcode=ADM_CMD_MEMBER_DISK_NUM
*****************************************************************************/
enum cmd_member_id_type {
    CMD_RG_MEMBER_TYPE,
    CMD_VD_MEMBER_TYPE
};

struct cmd_member_id {
    u16 id;   // RG �� VDͨ�ã�ͨ��type����
    u8 type;  // 0:CMD_RG_MEMBER_TYPE 1:CMD_VD_MEMBER_TYPE
    u8 rsvd;
};
/*****************************************************************************
 Description  : ��ʾRG��VD�ĳ�Ա����Ϣ
 command      : toolname cx:vdx / cx:rgx show pdarray
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_MEMBER_DISKS
*****************************************************************************/
struct cmd_member_disk_idx {
    struct cmd_member_id loc;
    u16 start_idx;  // ���λ�ȡ������Ϣ��ʼindex,��Χ:0-max_pdlist_n-1
    u16 count;      // ��ѯ����
    u32 rsvd;       // ����Ԥ��
};

struct cmd_member_disk_entry {
    u16 enc_id;
    u16 slot_id;
    u16 did;
    u16 asso_enc_id;   //  �����ȱ��̿�λ��
    u16 asso_slot_id;  //  �����ȱ��̲�λ��
    u16 asso_did;
    u64 capacity;  //  ������������B
    u32 sec_size;  //  ������С��B
    u8 model[40 + 1];
    u8 intf_type;
    u8 media_type;
    u8 logi_status;
    u8 asso_status;
    u8 rsvd[3];
};
#define CMD_BST_MAX_NUM 8192U
#define CMD_MEMB_DISKS_ONCE_NUM ((ADM_MAX_DATA_SIZE - 64) / sizeof(struct cmd_member_disk_entry))
struct cmd_show_member_disks {
    u16 num;
    u16 rsvd0;                                                    // ����Ԥ��
    u32 rsvd1[15];                                                // 64�ֽ�
    struct cmd_member_disk_entry disks[CMD_MEMB_DISKS_ONCE_NUM];  // ����4Kҳ������ܻ�ȡ��entry
};

#define RGS_ID_NODE_NUM         128
/*****************************************************************************
 Description  : BMCʹ�ã�һ�λ�ȡ����raid���id������ȡ��ϸ��Ϣ
 command      :
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_RGS_ID
*****************************************************************************/
struct cmd_rgs_id {
    u16 ids[RGS_ID_NODE_NUM];
    u16 num;  //  ��ǰ���е�raid������
    u8 rsvd[2];
};

/*****************************************************************************
 Description  : �����߼���VDΪ������
 command      : toolname cx:vdx set bootdrive
 Direction    : opcode=0xC0  subopcode=0x2A
*****************************************************************************/
struct cmd_vd_boot {
    struct cmd_vd_id loc;
    u8 boot_status;  // 0:�أ�1:��
    u8 rsvd[3];      // ����Ԥ��
};

/*****************************************************************************
 Description  : �����߼�������
 command      : toolname cx:vdx set attr
 Direction    : opcode=0xC0  subopcode=ADM_CMD_SET_VD_ATTR
*****************************************************************************/
struct cmd_set_vd_attr {
    struct cmd_vd_id loc;
    u8 type;     // �߼����������ͣ�0:wcache/1:rcache/2:name/3:map_type/4:pf/5:access
    u8 rsvd[3];  // ���뱣��
    union {
        u32 value;    // ����ֵ������������ʱ��Ч
        u8 name[MAX_VD_NAME_LEN];  // �߼������ƣ���������Ϊ����ʱ��Ч
    };
    u8 rsvd1[8];
};

/*****************************************************************************
 Description  : ���þֲ��ȱ��̻���ȫ���ȱ���
 command      : toolname cx:ex:sx set hotspare
 Direction    : opcode=0xC0  subopcode=ADM_CMD_SET_DISK_HSP
*****************************************************************************/
struct cmd_disk_hotspare {
    struct multi_disk_location loc;
    u8 hsp_type;  // 0 �ֲ��ȱ��̣�1 ȫ�� �ȱ��� 2 ��λ�ȱ�
    u8 rg_id;     // �ֲ��ȱ�ʱʹ��
    u8 rsvd[2];
};

/*****************************************************************************
 Description  : ���ò�λ�ȱ�
 command      : toolname cx:ex:sx set hotspare
 Direction    : opcode=0xC0  subopcode=ADM_CMD_SET_DISK_HSP
*****************************************************************************/
struct cmd_hspslot_param {
    struct multi_disk_location loc;
    u8 sw;        // �� 0:ADM_GENERAL_SWITCH_OFF  �� 1:ADM_GENERAL_SWITCH_ON
    u8 rsvd[7];
};

/*****************************************************************************
 Description  : ��ѯ��λ�ȱ��б�
 command      : toolname cx show hspslot
 Direction    : opcode=ADM_RAID_READ  subopcode=ADM_CMD_SHOW_BGTASK
*****************************************************************************/
#define CMD_MAX_HSPSLOTS_NUM 511
struct cmd_show_hspslot {
    u8 num;
    u8 rsvd[7];
    struct {
        u16 enc;
        u16 slot;
        u8 rsvd1[4];
    } slots[CMD_MAX_HSPSLOTS_NUM]; // 511����4K
};

struct sense_info {
    u8 sense_len;     // sense ���ȣ�<=96���ֽ�
    u8 sense_offset;  // ����ƫ��
    u16 ret;          // ����ִ�н��
};

/*****************************************************************************
 Description  : ���õ�����Ϊֱͨ��
 command      : toolname cx:ex:sx set passthru
 Direction    : opcode=ADM_RAID_SET  subopcode=ADM_CMD_SET_DISK_PASSTHRU
*****************************************************************************/
struct cmd_disk_passthru {
    struct multi_disk_location loc;
    u8 map_type;  // ���nvme�̵�ӳ������:SCSI��NVMe��Ԥ���ӿڣ�֧��˫PF��nvme�̺�ʹ��
    u8 pf;        // Ӳ�̹�����PF,Ԥ���ӿڣ�֧��˫PF��ʹ��
    u8 rsvd[2];   // ����Ԥ��
};

/*****************************************************************************
 Description  : ���ô���Ϊ������
 command      : toolname cx:ex:sx set bootdrive
 Direction    : opcode=0xC0  subopcode=0x43 ADM_CMD_SET_DISK_BOOT
*****************************************************************************/
struct cmd_disk_boot {
    struct multi_disk_location loc;
    u8 boot_status;  // 0:�أ�1:��
    u8 rsvd[3];      // ����Ԥ��
};

/*****************************************************************************
 Description  : ����������������
 command      : toolname cx:ex:sx set online/offline
 Direction    : opcode=0xC0  subopcode=KSTORE_CMD_SET_DISK_ON_OFFLINE
*****************************************************************************/
struct cmd_disk_on_offline {
    struct multi_disk_location loc;
    u8 online_status;  // ADM_DISK_ONLINE: online��ADM_DISK_OFFLINE: offline
    u8 rsvd[3];        // ����Ԥ��
};

/*****************************************************************************
 Description  : ����/��ѯȫ�����̿���״̬
 Command      : toolname cx set/show rawdisk
 Direction    : opcode=0xC0  subopcode=0x17/0x18
*****************************************************************************/
struct cmd_glb_rawdisk {
    u8 rawdisk_state;  // 0:�أ�1:��
    u8 rsvd[3];        // ����Ԥ��
};

/*****************************************************************************
 Description  : ����/��ѯȫ��DDMA����״̬
 command      : toolname cx set/show ddma
 Direction    : opcode=0xC0  subopcode=0x19/0x1A
*****************************************************************************/
struct cmd_glb_ddma {
    u8 ddma_state;  // 0:�أ�1:��
    u8 rsvd[3];     // ����Ԥ��
};

/*****************************************************************************
 Description  : SCSIЭ������͸��
 command      :
 Direction    : opcode=0xC0/0xC1/0xC2  subopcode=ADM_CMD_SCSI_PASSTHROUGH
*****************************************************************************/
struct cmd_scsi_passthrough {
    struct multi_disk_location loc;
    u8 lun;               // Ŀ�����ϵ�lun id����ǰ��Ʒֻ֧�ֵ��̵�lun,Ŀǰ��ʹ��
    u8 cdb_len;           // cdb����ĳ���
    u8 sense_buffer_len;  // sense buffer�Ĵ�С������96���ֽ�
    u8 rsvd0;             // �����ֶ�
    u8 cdb[32];           // cdb������
    u32 rsvd1;            // �����ֽڶ���
    u8 *sense_buffer;     // sense_buffer ���ݱ����ַ
};

/*****************************************************************************
 Description  : ��ȡ��ǰ��������������Ϣ��������Ϣ��hba��raid��ͬ.
 command      :
 Direction    : opcode=0xC2  subopcode=0x17 ADM_CMD_GET_CTRL_INFO
*****************************************************************************/
struct cmd_controller_info {
    u16 msg_len;                    // ��ǰ��Ϣ��Ч����
    u8  ctrl_mod;                   /*  ��ʾ��ǰ�忨�Ĺ���ģʽ
                                        0: HBA
                                        1: RAID
                                        2: HBA for Storage �洢ģʽHBA��
                                        3: JOBD������JBOD������ſ���
                                        4: other
                                    */
    u8  rgtask;                     /*  raid����غ�̨֧��:
                                        bit0:�ع�
                                        bit1 Ԥ����
                                        bit2 �ؿ�
                                        bit3 Ѳ��
                                        bit4 ���� ����ǰ�汾��֧�֣�
                                    */
    u8  mod_support;                /*  bit0: raid�Ƿ�֧��
                                        bit1:ֱͨģʽ�Ƿ�֧��
                                        bit2:����ģʽ�Ƿ�֧��
                                        ��������
                                    */
    u8 raid_level;                  /*  ֧�ֵ�raid����1��ʾ֧�֣�0��ʾ��֧��
                                        bit0 raid0
                                        bit1 raid1
                                        bit2 raid10
                                        bit3 raid5
                                        bit4 raid50
                                        bit5 raid6
                                        bit6 raid60
                                    */
    u8  expander_support;           // ֧�ֵ�expander������0 ��ʾ��ǰ�汾��֧�ֶԽ�expander
    u8  rsv;                        // ����
    u16 max_pdnum;                  // ֧���������������
    u16 max_hddnum;                 // ֧�����HDD������ 0��֧��
    u16 max_ssdnum;                 // ֧�����SSD������ 0��֧��
    u16 max_nvmenum;                // ֧�����NVMe������ 0��֧��
    u8  min_hddnum_level0;          // raid0��С��������(hdd)
    u8  max_hddnum_level0;          // raid0�����������(hdd)
    u8  min_hddnum_level1;          // raid1��С��������(hdd)
    u8  max_hddnum_level1;          // raid1�����������(hdd)
    u8  min_hddnum_level5;          // raid5��С��������(hdd)
    u8  max_hddnum_level5;          // raid5�����������(hdd)
    u8  min_hddnum_level6;          // raid6��С��������(hdd)
    u8  max_hddnum_level6;          // raid6�����������(hdd)
    u8  min_hddnum_level10;         // raid10��С��������(hdd)
    u8  max_hddnum_level10;         // raid10�����������(hdd)
    u8  max_hdd_subrgnum_level10;   // raid10��������(hdd)
    u8  min_hddnum_level50;         // raid50��С��������(hdd)
    u8  max_hddnum_level50;         // raid50�����������(hdd)
    u8  max_hddsubrgnum_level50;    // raid50��������(hdd)
    u8  min_hddnum_level60;         // raid10��С��������(hdd)
    u8  max_hddnum_level60;         // raid10�����������(hdd)
    u8  max_hddsubrgnum_level60;    // raid10��������(hdd)
    u8  pd_op_support;              // bit0 �����̰�ȫ���� sanitize
                                    // bit1 �����̰�ȫ���� erase
                                    // bit2 ���� smart_polling
    u16 vd_op_support;              // �߼��̲�������
                                    /*  bit0 �����޸�
                                        bit1 ��lun��hba��֧��
                                        bit2 ����
                                        bit3 ɾ��
                                        bit4 ǰ̨��ʼ��
                                        bit5 ��̨��ʼ��
                                        bit6 ���ٳ�ʼ��
                                        bit7 ��ȫ����
                                        bit8 ����һ����У��
                                    */
    u8  min_ssdnum_level0;          // raid0��С��������(ssd)
    u8  max_ssdnum_level0;          // raid0�����������(ssd)
    u8  min_ssdnum_level1;          // raid1��С��������(hdd)
    u8  max_ssdnum_level1;          // raid1�����������(hdd)
    u8  min_ssdnum_level5;          // raid5��С��������(hdd)
    u8  max_ssdnum_level5;          // raid5�����������(hdd)
    u8  min_ssdnum_level6;          // raid6��С��������(hdd)
    u8  max_ssdnum_level6;          // raid6�����������(hdd)
    u8  min_ssdnum_level10;         // raid10��С��������(hdd)
    u8  max_ssdnum_level10;         // raid10�����������(hdd)
    u8  max_ssd_subrgnum_level10;   // raid10��������(hdd)
    u8  min_ssdnum_level50;         // raid50��С��������(hdd)
    u8  max_ssdnum_level50;         // raid50�����������(hdd)
    u8  max_ssdsubrgnum_level50;    // raid50��������(hdd)
    u8  min_ssdnum_level60;         // raid10��С��������(hdd)
    u8  max_ssdnum_level60;         // raid10�����������(hdd)
    u8  max_ssdsubrgnum_level60;    // raid10��������(hdd)
    u8  max_expandlun;              // ÿ��vd֧�ֵ������չlun����
    u8  max_vdnum;                  // ֧�ֵ����vd����(64��)
    u8  spares_support;             /*  �Ƿ�֧���ȱ�
                                        bit0:ȫ���ȱ�֧��
                                        bit1:�ֲ��ȱ�֧��
                                        bit2:�����ȱ�֧��
                                    */
    u8  hsp_per_raid;               // ÿ��raid��֧�ֵ����ֲ��ȱ�������(2��)
    u8  max_globhps;                // ȫ���ȱ�������
    u8  foreign_import;             // �������õ���:1��ʾ֧�֣�0��ʾ��֧�֣�bit0 : ֧���������õ��룬����λ����
    u8  phy_number;                 // ������ʹ�õ����phy������40�����������β��ã�
    u8  drive_support;              // ֧�ֵ����������ͣ�bit0:SCSI �豸��bit1:NVMe�豸
    u8  rsvpf[3];                   // ����
    u8  pf_number;                  // pf֧�ֵ���������ǰоƬ���֧��2������    1:֧��1��PF��    2: ֧��2��PF
    u8  pf_type;                    /*  pf����ģʽ
                                        00:δʹ��
                                        01:SCSIģʽ
                                        02:NVMEģʽ
                                        bit0-bit3: PF0����ģʽ
                                        bit4-bit7: PF1����ģʽ
                                    */
    u8  ep_pcie_width;              // ����PCIE�˿����ԣ����:    1 x4��    2 x8��3 x16
    u8  ep_pcie_speed;              // PCIe4.0 16G
    u32 rsvd0;                      // ����
    u16 vdnum_per_rg;               // ÿ��raid����󴴽���vd����
    u16 rg_num;                     // raid��������
    u8  bp_support;                 // �Ƿ�֧�ֱ���ģ��:    bit0:����ģ���Ƿ�֧��
    u8  rsv_bp[3];                  // ����
    u8  rsv0[4];                    // ����
    u8  sas_ctl_speed;              // sas������֧�ֵ����� 12G
    u8  pcie_rp_speed;              // ֧������pcie�豸���ʣ�pcie4.0 16G
    u8  rsvd1[2];                   // ����
    u32 max_hddstripe_size;         // ���������С(K)   (���80M��
    u32 max_ssdstripe_size;         // ���������С(K)   (���16M��
    u16 min_hddstripe_size;         // ��С������С(K)   32 ��ʾ32K
    u16 min_ssdstripe_size;         // ��С������С(K)   32 ��ʾ32K
    u16 max_su_size;                // ���������Ԫ��С(K)  1024 (1M)
    u16 min_su_size;                // ��С������Ԫ��С(K)  32
    u8  pad[];                      // ��䵽128�ֽ�
};

#define CONTROLLER_STATUS_BOARD_NAME_LEN 24
/*****************************************************************************
 Description  : ��ȡ��ǰ��������״̬��Ϣ
 command      :
 Direction    : opcode=0xC2  subopcode=ADM_CMD_GET_CTRL_STATUS
*****************************************************************************/
struct cmd_controller_status {
    u16 msg_len;                // ��ǰ��Ϣ��Ч����
    u16 vid;                    // pcie vendor id
    u16 did;                    // pcie device ID
    u16 svid;                   // pcie subsystem Vendor ID
    u16 ssid;                   // subsystem ID
    u8 bus;                     // pci���ߺţ�0��
    u8 device;                  // pci�豸�ţ�0��
    u8 pf0_enable;              // ��pf0ʹ��ʱ,����Ϊ1���豸
    u8 pf1_enable;              // ��pf1ʹ��ʱ,����Ϊ2���豸
    u8 ep_pcie_width;           // Э�̿��:1,2,4,8,16  ������x1��x2��x4��x8��
    u8 ep_pcie_speed;           // ��ǰЭ������
    u64 sas_address;            // ��������sas��ַ
    char chip_name[16];         // ��ʾоƬ���� 40bytes
    char chip_version[8];       // оƬ�汾��Ϣ ��V100��
    u8 die_id[20];              // ��ʾоƬID (20λ��16������) ��0��
    u8 rsv[4];
    char board_sn[32];          // Ԥ��31���ֽڣ���ǰ20���ֽڣ�,�����׶μ��أ���barcode��ȡ ��0��
    char fw_version0[16];       // ����0��FW�汾 //Ԥ��15���ֽ� ��1.5.00��
    char fw_version1[16];       // ����1��FW�汾 ��1.5.01��136bytes
    u8 current_fw;              // ��ǰ���з���: 0 ����0�� 1:����1 ��0��
    u8 ctrl_mod;                // ��������ǰ����ģʽ RAID��HBA
    short chip_tempature;       // -40 ?C 125
    u16 disk_num;               // ��λ����������������phy��online��offline��
    u16 vd_num;                 // �߼�������
    u16 vd_degrade_num;         // ������vd����
    u16 vd_fault_num;           // ���ϵ�vd����
    u16 rg_num;                 // raid������
    u16 disk_prefail_num;       // ��ԤʧЧ����
    u16 disk_fail_num;          // ��ʧЧ����
    u16 ddr_size;               // ��λMB
    u16 ddr_ecc_count;          // DDRͳ��ֵ
    u16 ddr_ecc_bucket_size;    // �ɾ����������� 160bytes
    u16 ddr_ce_count;
    u16 ddr_uce_count;          // �����⵽���ɾ����������ӣ�(������bit ECC)���������ʱϵͳ�ع���
    u16 sram_ecc_count;         // ϵͳ��sram ��bit ecc����ͳ��
    u16 sram_uce_count;         // sram���ɾ�������ͳ��
    u8 cache_pinned;            // �Ƿ���pinned cache ��cache�ṩ״̬����0��
    u8 need_check;              // Errors need fw to check
    u16 board_hw_err;           /* �ڲ�ģ������ϣ�����IOֹͣ��
                                 * bit0: TSP�쳣
                                 * bit1: NAC�쳣
                                 * bit2: FW�쳣
                                 * bit3: IMU�쳣
                                 * bit4: ����ʧ��
                                 * bit5: SOC_FATAL
                                 * bit6: DDR_MBIT_ERR
                                 * bit7: SRAM_ERR
                                 * bit8: DDRC_ERR
                                 * bit9: RDE_ERR
                                 * bit10: SAS_CONTROLLER_ERR
                                 * bit11 - bit15: rsved */
    u8 board_clk_err;           /* �ڲ�ģ��ʱ�ӹ���
                                 * bit0: PCIe EP�ο�ʱ�ӹ���
                                 * bit1: PCIe RP�ο�ʱ�ӹ���
                                 * bit2: SAS �˿ڲο�ʱ�ӹ���
                                 * bit3 - bit7: rsved */
    u8 board_power_err;         /* bit0: ��ԴоƬ�쳣��SYH664������״̬
                                 * bit1: �ڲ���ԴоƬ�����ѹ����
                                 * bit2: �ڲ���ԴоƬ�����ѹ����
                                 * bit3 - bit7: rsved */
    u8 board_capacity_err;      /* �ڲ�����ģ����ݹ���
                                 * bit0: оƬ�ڲ�ģ�������
                                 * bit1: �������ݹ�����ѹ����
                                 * bit2: �������ݹ�����ѹ����
                                 * bit3: С������ֵ�쳣 (S_CAPACITY_ERR)
                                 * bit4: ����������ֵ�쳣
                                 * bit5: С���ݹ�����ѹ����
                                 * bit6: С���ݹ�����ѹ����
                                 * bit7: rsved */
    u8 board_flash_err;         /* �ڲ�flashģ�����
                                 * bit0: SPI Nor Flash����
                                 * bit1: SPI Nand Flash���� (hba bp_Flash_err)
                                 * bit2: NFC Nand Flash���� (raid bp_Flash_err)
                                 * bit3 - bit7: rsed */
    char board_name[CONTROLLER_STATUS_BOARD_NAME_LEN];        // �����ͺ�����
};

/*****************************************************************************
 Description  : ��ȡ�������ݵ�״̬��Ϣ
 command      : toolname cx:sc show supercap
 Direction    : opcode=0xC2  subopcode=ADM_CMD_GET_SC_INFO
*****************************************************************************/
struct cmd_supercapacitor_info {
    u8 voltage;                   // ����ģ���ѹ����λ0.1V��
    u8 pcb_version;               // ����ģ��PCB�汾�ţ�0000 0000=A��0000 0001=B��
    char temperature;             // ����ģ���¶ȣ��������϶ȣ�
    char alarm_temp;              // �¶�����
    u32 design_capacity;          // ����ģ���������(0.01FΪ��λ)
    char module_version[8];       // ����ģ��汾��(000001 ~ 065535)
    char cap_version[8];          // �������ݰ汾��(ABCD),��ǰʹ��4���ֽ�
    char device_name[16];         // ����ģ���豸��
    char vendor_name[8];          // ����ģ�鳧������д
    char cap_sn[16];              // ��������SN���к�
    char device_sn[16];           // ����ģ��SN��
    char cap_manufacture[12];     // �������ݳ�������
    char device_manufacture[12];  // ����ģ����������
    char pack_assm_no[8];         // ����ģ�������к�
    char pcb_assm_no[16];         // ����ģ��PCBװ���
    u8 present;                   // ��λ״̬(0-����λ��1-��λ)
    u8 learn_status;              /* ��ѧϰ״̬:
                         0: ������ѧϰ��bitλ��1��Ч��
                         1: ��ѧϰ�ɹ���bitλ��1��Ч��
                         2: ��ѧϰʧ�ܣ�bitλ��1��Ч��
                         3: ��ѧϰ��ʱ��bitλ��1��Ч��
                         ȫ0:û����ѧϰ
                     */
    u8 vendor_id;                 // ����ģ�鳧��ID����ʾΪ01��
    u8 flash_erase_status;  // flash����״̬��0-û�в�����1-�����ɹ���2-����ʧ�ܣ�3-���ڲ�����
    u8 sc_status;           /* ����bit��ʾ״̬:
                      0: mos_status   ����MOS�Ĵ򿪺͹ر�״̬��1-�򿪣�
                      1: high_temp_alarm����ģ���¶ȹ��ߣ�1��Ч��
                      2:low_vol_alarm ����ģ���ѹ���ͣ�1��Ч��
                      3:low_capacity_alarm    ����ģ��ʣ�������ͣ�1��Ч��
                      4: replace_required ��Ҫ������1��Ч��
                      5: nospace_offload   û�пռ䱸��cache��1��Ч��
                      6: pack_missing ���ݲ���λ��1��Ч��
                      7: ����
                      ˵��:bit 2��bit 3����ѧϰʱ������λ����ѧϰ��ɺ���ݽ����λ
                  */
    u8 status;              // ����ģ������״̬��0-FAILED��1-OK��
    u8 signal_alarm;        // ����iicͨ���쳣 (1-FAILED, 0-OK)
    u8 rsvd0[9];           // �����ֶΣ��������䣬Ĭ����0
};

enum cmd_channal_type {
    CHANNAL_TYPE_INBAND_SCSI = 0,
    CHANNAL_TYPE_OUTBAND_PCIE,
    CHANNAL_TYPE_OUTBAND_I2C,
    CHANNAL_TYPE_INBAND_NVME,
    CHANNAL_TYPE_INVALID,
};

enum cmd_pf_type {
    PF_TYPE_PF0 = 0,
    PF_TYPE_PF1,
    PF_TYPE_INVALID,
};

struct outband_cmd_req_header {
    u8 opcode;              // ������
    u8 cmd_flags;           // Ԥ��lib���fw�����ݿ�����Ϣ
    u16 cid;                // ��Ϣ���кţ����ڽ�������CQE��Ӧ����lib�����
    u16 subopcode;          // �������֣����ڶ�����������
    u8 obj_flag;            // ���obj�����ͣ���ǰ��Ҫ������������EID/SID��ʾ�����̣�����DID��ʾ������
    u8 Rsvd1;
    u16 obj;                // �������������id����������ID��VD ��ID�� RG��ID��
    u16 obj_ex;             // ���ʹ��EID/SID��ʾ������ʱ���������EID
    u16 param_len;          // ָ����������
    u16 data_len;           // ָ�����ݳ��ȣ�������4K��
    u16 data_offset;        // ������� opcode��ʼ�����ʼλ��
    u16 version;            // ����lib��͹̼��汾ƥ��
    u32 timeout_ms;         // ���ʱʱ�䣬��λ:����
    u8 data[0];             /* �����������ݡ����ǵ��ʹ��������ּ��ݣ������������С��20������Դ��������ֶΣ������ܴ��������ֶ�
                                ���ݵ���ʼ��ַ������8�ֽڶ��룬��������룬ʹ��pad��䣬����ܳ��Ȳ������ֽڶ��룬���pad��0������
                            */
};

struct outband_cmd_rsp_header {
    u32 cqedw0;             // ������������ķ��ؽ���������ѯ��ϢС��32bit����ʹ������ֶη��ؽ����
    u32 cqedw1;             // Ԥ����Ϊ�˼���NVMe��Դ�������еİ汾ֻ�ܷ���32bitֵ���û�̬���������ֶξ�����ʹ�ã�
    u16 data_len;           // ָ�����ݳ��ȣ�������4K��
    u16 data_offset;        // ������� cqedw0��ʼ�����ʼλ��
    u16 cid;                // ��Ϣ���кţ����ڽ�������CQE��Ӧ����lib�����
    u16 status;             // ��������ͨ�õ�ִ��״̬���ɹ���ʧ�ܣ�
    u8 data[0];
};

#define MAX_HOST_EVENT_NUMBER 14
struct event_info {
    u8 type;                // ��ע���¼�����
    u8 level;               // ��ע���¼���ͼ��𣨱����ע����ͼ�����info�����߼����warning��errorҲ���ϱ���
} __attribute__((packed));

struct host_event_info {
    u8 eid;                 // ����mctp��Ӧ��eid
    u8 event_num;           // ��ע���¼�����������events������
    u16 rsv;
    struct event_info events[MAX_HOST_EVENT_NUMBER];    // ��ע�ľ����¼�
};

struct host_powercycle_result {
    u16 enc_id;     // ���
    u16 slot_id;    // ��λ��
    int ret;        // �µ����ϵ��Ƿ�ɹ���0:�ɹ�����0:ʧ��
};

/*****************************************************************************
 Description  : set raid bgtask speed
 command      : toolname cx set bgrate function={rebuild|sanitize|bginit|expandrg|copyback|precopy
                                                 |precopy|consistencycheck(ccheck)|patrolread|all
                                        rate={high|medium|low}
 Direction    : ADM_RAID_SET SUBOPCODE:ADM_CMD_SET_BGRATE
*****************************************************************************/
struct cmd_set_bgrate {
    u8 bg_type;
    u8 rate;
    u8 rsvd[2];
};

/*****************************************************************************
 Description  : ��ʾ�߼���һ����У����Ϣ
 command      : toolname cx:vdx show ccheck (consistencycheck)
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_CONSISTENCY_CHECK
*****************************************************************************/
struct cmd_get_vd_cc {
    u16 vd_id;
    u8 sw;
    u8 period;
    u8 rate;
    u8 repair;
    u8 run_status;
    u8 progress;
    u16 left_time;
    u8 rsv;
    u8 delay;   // �ӳ�ʱ�䣬��λh
    u16 magic;  // CMD_MAGIC_FLAG ����֮ǰ�汾�������־
};

/*****************************************************************************
 Description  : ��ʾraid��һ����У����Ϣ
 command      : toolname cx show ccheck (consistencycheck)
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_CONSISTENCY_CHECK
*****************************************************************************/
struct cmd_get_rd_cc {
    u8 sw;           // 0Ϊoff 1Ϊon
    u8 period;       // ����  :��λΪday      ȡֵ��Χ[1:60] Period
    u8 rate;         // ADM_BGTASK_RATE_LOW = 1 ADM_BGTASK_RATE_MIDDLE = 2 ADM_BGTASK_RATE_HIGH = 3
    u8 repair;       // ADM_AUTOCC_REPAIRE_AUTO =0��ʾΪon      ADM_AUTOCC_REPAIRE_MANU = 1 ��ʾΪoff
    u8 run_status;   // 0 stop     1 run
    u8 total_vd;     // ��ҪУ���������
    u8 complete_vd;  // �Ѿ���ɵ�����
    u8 delay;        // �ӳ�ʱ�䣬��λh
    u16 magic;       // CMD_MAGIC_FLAG ����֮ǰ�汾�������־
    u8 rsvd[54];     // �� 64
};

/*****************************************************************************
 Description  : ��ʼ\ֹͣ�߼���һ����У����Ϣ
 command      : toolname cx:vdx start ccheck delay=x
                toolname cx:vdx stop ccheck
 Direction    : ADM_RAID_SET  SUBOPCODE:ADM_CMD_SET_VD_CONSIS_CHECK
*****************************************************************************/
struct cmd_set_vd_cc {
    struct cmd_vd_id loc;
    u8 sw;      // 0: off (stop)  1:on ��start�� 2: set
    u8 rsvd0[3];
    u32 delay;  // ����ĵȴ���ʼʱ�䣬��λСʱ
    u8 rsvd[4];
};

/*****************************************************************************
 Description  : ����/��ʼ/ֹͣ����һ����У��
 command      : toolname cx start ccheck period=x repair={on|off} rate={low|medium|high}
                toolname cx stop ccheck
 Direction    : ADM_RAID_SET  SUBOPCODE:ADM_CMD_SET_CONSIS_CHECK
*****************************************************************************/
struct cmd_set_cc {
    u8 sw;      // 0: off (stop)  1:on ��start�� 2: set
    u8 period;  // ����  :��λΪday      ȡֵ��Χ[1:60] Period
    u8 rate;    // ADM_BGTASK_RATE_LOW = 1 ADM_BGTASK_RATE_MIDDLE = 2 ADM_BGTASK_RATE_HIGH = 3
    u8 repair;  // ADM_AUTOCC_REPAIRE_AUTO =0��ʾΪon      ADM_AUTOCC_REPAIRE_MANU = 1 ��ʾΪoff
    u32 delay;  // ����ĵȴ���ʼʱ�䣬��λСʱ
    u8 mask;
    u8 rsvd[3];
};

/*****************************************************************************
 Description  : ����raid���Զ�Ѳ��
 command      : toolname cx start patrolread period=x drivenum=x rate={low|medium|high}
                toolname cx stop patrolread
 Direction    : ADM_RAID_SET  SUBOPCODE:ADM_CMD_SET_PATROLREAD
*****************************************************************************/
struct cmd_start_patrolread {
    u8 sw;        // 0: off (stop)  1:on ��start�� 2: set
    u8 period;    // ���ڣ���λ:�죬ȡֵ��Χ[1:60]
    u8 rate;      // ADM_BGTASK_RATE_LOW = 1 ADM_BGTASK_RATE_MIDDLE = 2 ADM_BGTASK_RATE_HIGH = 3
    u8 disk_num;  // ����ͬʱѲ���Ӳ��������1-8��Ĭ��4��
    u32 delay;    // ����ĵȴ���ʼʱ�䣬��λСʱ
    u8 mask;      // bit���Ʊ����������ز���ֵ�Ƿ���Ч����˳��bitλ��1��Ч�����ڱ����û���ʷ�������ã����������޸ġ�
    u8 rsvd[3];   // ����Ԥ��
};

/*****************************************************************************
 Description  : ���ý����ȱ�����״̬
 command      : toolname cx set ehsp sw={on|off}
 Direction    : ADM_RAID_SET  SUBOPCODE:ADM_CMD_SET_EHSP
*****************************************************************************/
struct cmd_glb_ehsp {
    u8 ehsp_status;
    u8 rsvd[3];
};

/*****************************************************************************
 Description  : ���ú�̨���񿪹�״̬
 command      : toolname cx set bgtask sw={on|off}
 Direction    : ADM_RAID_SET  SUBOPCODE:ADM_CMD_SET_BGTASK
*****************************************************************************/
struct cmd_glb_bgtask {
    u8 bgtask_status;
    u8 rsvd[3];
};

/*****************************************************************************
 Description  : ��չvd����
 command      : toolname cx:vdx expand size=x
 Direction    : ADM_RAID_SET  SUBOPCODE:ADM_CMD_EXPAND_VD_SIZE
*****************************************************************************/
struct cmd_expand_vd {
    struct cmd_vd_id loc;
    u64 capacity;  // ��չ����MB��λ
    u32 rsvd;
};

/*****************************************************************************
 Description  : ��չrg��������
 command      : toolname cx:rgx expand {drives=ex:sx,ey:sy OR drives=x,y (dids)}
 Direction    : ADM_RAID_SET  SUBOPCODE:ADM_CMD_EXPAND_RG_DISK
*****************************************************************************/
struct cmd_expand_rg_disk {
    struct cmd_rg_id loc;
    u16 num;
    u8 locate_type;
    u8 rsvd;
    union {  // ���location_typeΪ0,��ʹ��struct disk_location,����ʹ��u16 did[80]
        struct disk_location disks[CMD_MEMB_DISK_MAX_NUM];
        u16 did[CMD_MEMB_DISK_MAX_NUM];
    };
};

/*****************************************************************************
 Description  : ���̽��Ȳ�ѯ
 command      : toolname cx:rgx show expand
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_RG_EXP_PROGRESS
*****************************************************************************/
struct cmd_expand_disk_info {
    u16 rg_id;
    u16 left_time;  //  ʣ��Ԥ��ʱ�䣬��λ��min
    u8 progress;    //  0-100
    u8 rsvd[3];
};

/*****************************************************************************
 Description  : ���߼��̽��а�ȫ����
 command      : toolname cx:vdx start sanitize mode={simple|normal|thorough}
                toolname cx:vdx stop sanitize
 Direction    : ADM_RAID_SET SUBOPCODE:ADM_CMD_SET_VD_SANITIZE
*****************************************************************************/
struct cmd_set_vd_sanitize {
    struct cmd_vd_id loc;
    u8 sw;
    u8 mode;  // simple��һ��normal ������ thorough ִ�о���
    u8 pattern[2];
    u8 is_valid;
    u8 rsvd[3];
};

/*****************************************************************************
 Description  : ��ѯ�߼���VD��ȫ������Ϣ
 command      : toolname cx:vdx show sanitize
 Direction    : SUBOPCODE:ADM_CMD_SHOW_VD_SANITIZE
*****************************************************************************/
struct cmd_vd_sanitize_info {
    u16 id;
    u8 status;
    u8 rate;
    u8 sanitize_times;
    u8 current_step; /* delete */
    u8 progress;
    u8 rsvd0;
    u8 name[31 + 1];
    u64 capacity;
    u16 left_time;
    u8 sanitize_result;
    u8 rsvd1;
};

/*****************************************************************************
 Description  : ��ѯ�߼���VD�ķ�����һ����Ϣ
 command      : toolname cx:vdx show inconsistent
 Direction    : SUBOPCODE:ADM_CMD_SHOW_VD_INCONSISTENT
*****************************************************************************/
struct cmd_vd_inconsistent_info {
    u32 stripe_num;
    struct {
        u16 vd_id;
        u16 rsvd;
        u32 stripe_id;
        u64 start_lba;
        u64 len;
    } stripe[MAX_INCONSISTENT_STRIPE_NUM];  // 100
};

/*****************************************************************************
 Description  : ��ѯ�洢�豸����������
 command      : toolname cx/cx:ex:sx show bst slba=xx (512, 512kB,512M, 512G) num=xx
 Direction    : SUBOPCODE:ADM_CMD_SHOW_BST
*****************************************************************************/

struct cmd_bst_num_param {
    struct multi_disk_location loc;  // �ڻ�ȡ����bstʱ�����õ���bst�Ļ�ȡ��ʽ��ֻ�ǽ�flag��һ����did��Ϊȫf.
    u64 start_lba;
    u32 len;
};

struct cmd_bst_data_idx {
    struct multi_disk_location loc;  // �ڻ�ȡ����bstʱ�����õ���bst�Ļ�ȡ��ʽ��ֻ�ǽ�flag��һ����did��Ϊȫf.
    u64 start_lba;
    u32 len;
    u16 start_idx;
    u16 count;
};

struct cmd_bst_data_entry {
    u16 did;
    u64 start_lba;  // ��ʼ����λ��
    u32 nlb;        // ��������
    u8 type;        // 1:Logical     0:Physical
    u8 rsvd[3];
    u32 index;
};
#define CMD_BST_MAX_NUM 8192U
#define CMD_BST_DATA_ONCE_NUM ((ADM_MAX_DATA_SIZE - 64U) / sizeof(struct cmd_bst_data_entry))
struct cmd_show_bst {
    u16 num;
    u16 rsvd0;
    u32 rsvd1[15];
    struct cmd_bst_data_entry bsts[CMD_BST_DATA_ONCE_NUM];
};

/*****************************************************************************
 Description  : ����Ӳ����������
 command      : toolname cx:ex:sx set routine sw={on|off}
                toolname cx show jbod/rawdrive
 Direction    : opcode=0xC0  subopcode=0x15/0x16 ADM_CMD_SET_PASSTHRU_TYPE
                                                 ADM_CMD_SHOW_PASSTHRU_TYPE
*****************************************************************************/
struct cmd_glb_passthru_type {
    u8 passthru_type;  // 0:�أ�1:JBODģʽ 2:RAWDISKģʽ
    u8 rsvd[3];        // ����Ԥ��
};

/*****************************************************************************
 Description  : SMARTɨ���������úͲ�ѯ
 command      : toolname cx set routine_cycle period=x
 Direction    : opcode:SET  ADM_CMD_SET_SMART_POLLING
                            ADM_CMD_SHOW_SMART_POLLING
*****************************************************************************/
struct cmd_smart_polling {
    u32 period;  // ����ʱ��,��λs��Ĭ��ֵ:60
    u32 rsvd;
};

/*****************************************************************************
 Description  : ����phy������
 Command      : toolname cx:ex:sx set phy func={hardreset|linkreset|enable|disable|speed}
 Direction    : none  ADM_CMD_SET_DISK_PHY
*****************************************************************************/
struct cmd_set_phy {
    struct multi_disk_location loc;
    u8 func;  // ���ܺ�����Ӧ�����
    u8 speed;
    u8 rsvd[6];
};

/*****************************************************************************
 Description  : ����phy������
 Command      : toolname cx:ex:sx set phy_cfg sw={enable|disable} [sp=speed(12G)]
 Direction    : none  ADM_CMD_SET_PHY_CFG
*****************************************************************************/
struct cmd_set_phy_cfg {
    struct multi_disk_location loc;
    u8 sw;
    u8 speed;
    u8 rsvd[6];
};

/*****************************************************************************
 Description  : ��ѯ phy cfg
 Command      : toolname cx show phy_cfg
 Direction    : none  ADM_CMD_SHOW_PHY_CFG
*****************************************************************************/
typedef struct _flash_phy_cfg_info { /* 12B */
    u8 flag;  // 1�����ã�0xffδ����
    u8 slot_id;
    u8 phy_link_rate;
    u8 phy_switch;
    u32 rsv[2];
} flash_phy_cfg_info;

/*****************************************************************************
 Description  : ���ô��̶������
 Command      : toolname cx:ex:sx set phy func={hardreset|linkreset|enable|disable|speed}
 Direction    : none  ADM_CMD_SET_DISK_QUEUE_DEPTH
*****************************************************************************/
struct cmd_set_disk_queue_depth {
    struct multi_disk_location loc;
    u16 depth;
    u16 rsvd;
};

/*****************************************************************************
 Description  : ��ȡӲ��IO����ͳ��
 command      : toolname cx:ex:sx diskIO
 Direction    : opcode: ADM_CMD_SHOW_DISK_IO_SUM
*****************************************************************************/
struct cmd_show_disk_io_sum {
    u64 io_sum;
    u64 failed_io_sum;
    u8 rsvd[4];
};

/*****************************************************************************
 Description  : ���á���ѯӲ�̵�дcache״̬
 command      : toolname cx:ex:sx diskIO
 Direction    : opcode:  ADM_CMD_SET_DISK_CACHE
                         ADM_CMD_SHOW_DISK_CACHE
*****************************************************************************/
struct cmd_disk_cache {
    struct multi_disk_location loc;
    u8 cache_status;
    u8 rsvd[3];
};

#define VDS_ID_NODE_NUM         128
/*****************************************************************************
 Description  : BMCʹ�ã�һ�λ�ȡ�����߼���vd��id
 command      : toolname cx:ex:sx diskIO
 Direction    : opcode:  ADM_CMD_SHOW_VDS_ID
*****************************************************************************/
struct cmd_vds_id {
    u16 ids[VDS_ID_NODE_NUM];
    u16 num;  // ��ǰ���е��߼�������
    u8 rsvd[2];
};

/*****************************************************************************
 Description  : pcie��·�����ѯ
 command      : toolname cx
 Direction    : opcode:
*****************************************************************************/
union pcie_mac_int_status {
    struct {
        u32 reg_linkup_intr_status : 1;                  /* [0] */
        u32 reg_linkdown_intr_status : 1;                /* [1] */
        u32 reg_enter_l0_intr_status : 1;                /* [2] */
        u32 reg_leave_l0_intr_status : 1;                /* [3] */
        u32 reg_enter_disable_intr_status : 1;           /* [4] */
        u32 reg_leave_disable_intr_status : 1;           /* [5] */
        u32 reg_hot_reset_intr_status : 1;               /* [6] */
        u32 rxl0s_to_recovery_intr_status : 1;           /* [7] */
        u32 reg_phystatus_timeout_intr_status : 1;       /* [8] */
        u32 reg_deskew_unlock_intr_status : 1;           /* [9] */
        u32 reg_symbol_unlock_intr_status : 1;           /* [10] */
        u32 reg_loopback_unlock_intr_status : 1;         /* [11] */
        u32 reg_deskew_fifo_overflow_intr_status : 1;    /* [12] */
        u32 reg_enter_l1_timeout_intr_status : 1;        /* [13] */
        u32 reg_enter_l2_timeout_intr_status : 1;        /* [14] */
        u32 reg_enter_recovery_lock_intr_status : 1;     /* [15] */
        u32 reg_enter_recovery_cfg_intr_status : 1;      /* [16] */
        u32 reg_enter_eq_p3_intr_status : 1;             /* [17] */
        u32 pl_eco_rsv_intr_status : 4;                  /* [21:18] */
        u32 ltssm_tracer_sram_ecc_1bit_err : 1;          /* [22] */
        u32 ltssm_tracer_sram_ecc_2bit_err : 1;          /* [23] */
        u32 reg_enter_g1_recovery_speed_intr_status : 1; /* [24] */
        u32 reg_enter_g2_recovery_speed_intr_status : 1; /* [25] */
        u32 reg_enter_g3_recovery_speed_intr_status : 1; /* [26] */
        u32 reg_ltssm_intr_status : 1;                   /* [27] */
        u32 reg_enter_lpbk_lock_intr_status : 1;         /* [28] */
        u32 reg_enter_g4_recovery_speed_intr_status : 1; /* [29] */
        u32 reserved : 2;                                /* [31:30] */
    } bits;
    u32 val;
};  // 0x054

union pcie_mac_reg_test_counter {
    struct {
        u32 phy_lane_err_counter : 16; /* [15:0] */
        u32 reserved : 16;             /* [31:16] */
    } bits;
    u32 val;
};  // 0x05c

union pcie_mac_reg_debug_pipe2 {
    struct {
        u32 mac_powerdown_1 : 24; /* [23:0] */
        u32 mac_ltssm_st : 6;     /* [29:24] */
        u32 reserved : 2;         /* [31:30] */
    } bits;
    u32 val;
};  // 0x068

union pcie_mac_reg_debug_pipe3 {
    struct {
        u32 phy_phystatus : 16; /* [15:0] */
        u32 mac_ltssm_st : 6;   /* [21:16] */
        u32 reserved : 10;      /* [31:22] */
    } bits;
    u32 val;
};  // 0x06c

union pcie_mac_symbol_unlock_counter {
    struct {
        u32 symbol_unlock_counter : 16; /* [15:0] */
        u32 reserved : 16;              /* [31:16] */
    } bits;
    u32 val;
};  // 0x08c

union pcie_mac_reg_debug_pipe10 {
    struct {
        u32 phy_rxdata_valid : 16; /* [15:0] */
        u32 mac_ltssm_st : 6;      /* [21:16] */
        u32 reserved : 10;         /* [31:22] */
    } bits;
    u32 val;
};  // 0x0254

union pcie_mac_pcs_rx_err_cnt {
    struct {
        u32 pcs_rx_err_cnt : 16; /* [15:0] */
        u32 reserved : 16;       /* [31:16] */
    } bits;
    u32 val;
};  // 0x02BC

union pcie_mac_level_l0_info {
    struct {
        u32 up_retrain_redo_req : 1;     /* [0] */
        u32 dp_redp_eq_enter_rcv : 1;    /* [1] */
        u32 cfg_link_disable : 1;        /* [2] */
        u32 enter_loop_back : 1;         /* [3] */
        u32 framing_err_retrain : 1;     /* [4] */
        u32 dl_retran : 1;               /* [5] */
        u32 skp_infter_idle_timeout : 1; /* [6] */
        u32 hot_reset : 1;               /* [7] */
        u32 cfg_retrain_req : 1;         /* [8] */
        u32 reg_retrain_req : 1;         /* [9] */
        u32 det_eieos_128b130b : 1;      /* [10] */
        u32 det_ts_train : 1;            /* [11] */
        u32 direct_speed_change : 1;     /* [12] */
        u32 tl_ap_req_link_down : 1;     /* [13] */
        u32 dl_req_link_down : 1;        /* [14] */
        u32 enter_l2_req : 1;            /* [15] */
        u32 enter_l1_req : 1;            /* [16] */
        u32 enter_l0s_req : 1;           /* [17] */
        u32 enter_l1l2_timeout : 1;      /* [18] */
        u32 rcv_eios : 1;                /* [19] */
        u32 resvered : 1;                /* [31:20] */
    } bits;
    u32 val;
};  // 0x3b4

union pcie_apb_lane_error_status0 {
    struct {
        u32 symbol_unlock_err_status : 16; /* [15:0] */
        u32 pcs_rcv_err_status : 16;       /* [31:16] */
    } bits;
    u32 val;
};  // 0x3bc

union pcie_apb_lane_error_status1 {
    struct {
        u32 phy_lane_err_status : 16;           /* [15:0] */
        u32 loopback_link_data_err_status : 16; /* [31:16] */
    } bits;
    u32 val;
};  // 0x3c0

union pcie_dl_dfx_lcrc_err {
    struct {
        u32 dl_lcrc_err_num : 8; /* [7:0] */
        u32 resvered : 24;       /* [31:8] */
    } bits;
    u32 val;
};  // 0x50

union pcie_dl_dfx_dcrc_err {
    struct {
        u32 dl_dcrc_err_num : 8; /* [7:0] */
        u32 resvered : 24;       /* [31:8] */
    } bits;
    u32 val;
};  // 0x54

union pcie_dl_retry_cnt {
    struct {
        u32 dfx_retry_cnt : 16; /* [15:0] */
        u32 resvered : 16;      /* [31:16] */
    } bits;
    u32 val;
};  // 0xb8

union pcie_dl_mac_retry_cnt {
    struct {
        u32 dl_mac_retrain_cnt : 8; /* [7:0] */
        u32 resvered : 24;          /* [31:8] */
    } bits;
    u32 val;
};  // 0xd4

union pcie_dl_rx_nak_count {
    struct {
        u32 rx_nak_count : 16; /* [15:0] */
        u32 resvered : 16;     /* [31:16] */
    } bits;
    u32 val;
};  // 0x00F8

union pcie_cfg_cor_err_status {
    struct {
        u32 rx_err_st : 1;                 /* [0] */
        u32 reserved0 : 5;                 /* [5:1] */
        u32 bad_tlp_st : 1;                /* [6] */
        u32 bad_dllp_st : 1;               /* [7] */
        u32 reply_num_rollover_st : 1;     /* [8] */
        u32 reserved1 : 3;                 /* [11:9] */
        u32 reply_timer_timout_st : 1;     /* [12] */
        u32 advisory_non_fatal_err_st : 1; /* [13] */
        u32 cor_int_err_st : 1;            /* [14] */
        u32 header_log_overflow_st : 1;    /* [15] */
        u32 reserved2 : 16;                /* [31:16] */
    } bits;
    u32 val;
};  // 0x110

struct cmd_pcie_link_error {
    union pcie_mac_int_status mac_int_status;
    union pcie_mac_reg_test_counter test_counter;
    union pcie_mac_reg_debug_pipe2 dbg_pipe2;
    union pcie_mac_reg_debug_pipe3 dbg_pipe3;
    union pcie_mac_symbol_unlock_counter sys_unlock;
    union pcie_mac_reg_debug_pipe10 dbg_pipe10;
    union pcie_mac_pcs_rx_err_cnt pcs_rx_err;
    union pcie_mac_level_l0_info mac_level_10_info;
    union pcie_apb_lane_error_status0 apb_lane_err_st0;
    union pcie_apb_lane_error_status1 apb_lane_err_st1;
    union pcie_dl_dfx_lcrc_err lcrc;
    union pcie_dl_dfx_dcrc_err dcrc;
    union pcie_dl_retry_cnt dl_retry;
    union pcie_dl_mac_retry_cnt dl_mac_retry;
    union pcie_dl_rx_nak_count rx_nak_count;
    union pcie_cfg_cor_err_status cfg_cor_err;
};

#define MAX_EXMBIST_BURST 2
#define MAX_EXMBIST_FAULT_RECORD 10

typedef union _U_EXMBIST_ROWCOL {
    struct {
        u32 row : 18;    // ����Row
        u32 col : 10;    // ����Col
        u32 bank : 4;      // [31:28] ����Bank
    } bits;
    /* Define an unsigned member */
    u32    value;
}U_EXMBIST_ROWCOL;
typedef union _U_EXMBIST_ERRINFO {
    struct {
        u32 type : 2;      // [1:0] ��ʾ�������� 00 -> �޹���, 01 -> CE, 10 -> UCE
        u32 socket : 2;    // [3:2] ����Socket 00 -> Socket0, 01 -> Socket1
        u32 dimm : 1;      // [4] ����DIMM 0 -> DIMM0, 1-> DIMM1
        u32 rank : 2;      // [6:5] ����Rank 0 -> Rank0, 1-> Rank1
        u32 channel : 3;   // [9:7] 000 -> Ch0, ... ,110 -> Ch6
        u32 dramWidth : 2; // [11:10] �ڴ�λ��
        u32 alo : 5;       // [16:12] �����㷨
        u32 patternId : 5; // [21:17] ����pattern
        u32 reserve : 10;   // [31:22] ����λ
    } bits;
    /* Define an unsigned member */
    u32    value;
}U_EXMBIST_ERRINFO;
typedef struct _exmbist_error_recode {
    u64 err_bit[MAX_EXMBIST_BURST];   // ����bitλ
    U_EXMBIST_ROWCOL row_col;
    U_EXMBIST_ERRINFO err_info;
    u16 err_cnt;                      // �������
    u8 err_burst[MAX_EXMBIST_BURST];  // ����Burst
    u8 temperature;                   // �¶�
    u8 rsv[3];
}exmbist_error_recode;

struct exmbist_sum {
    u32 idx;  // ��¼ record ��ǰ�� idx
    u32 rsv1;
    exmbist_error_recode fault_record[MAX_EXMBIST_FAULT_RECORD];
    u8 status;  // 4:��Ҫ�� 3:Running 2:fail, 1:pass, 0:Ĭ��
    u8 cur_pattern; // ��¼��ǰ����ʲôpattern�׶Σ�for debug
    u8 cur_algorithm; // ��¼��ǰ����ʲô�㷨�׶�, for debug
    u8 rsv2;
    u32 magic;
};

typedef struct exmbist_sum hw_exmbist_sum;

struct set_exmbist_param {
    u8 status;  // 4:��Ҫ�� 3:Running 2:fail, 1:pass, 0:Ĭ��
    u8 rsv[3];
};

enum set_exmbist_status {
    EXMBIST_STATUS_DEFALUT = 0,
    EXMBIST_STATUS_PASS,
    EXMBIST_STATUS_FAIL,
    EXMBIST_STATUS_RUNNING,
    EXMBIST_STATUS_TEST,
};

/*****************************************************************************
 Description  : ��ȡecc��Ϣ
 command      : toolname cx show ecc_info
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SRAM_ECC_INFO
*****************************************************************************/
struct sram_ecc_info {
    u16 peri_single_ecc;
    u16 peri_mutil_ecc;
    u16 imu_single_ecc;
    u16 imu_mutil_ecc;
};

/*****************************************************************************
 Description  : ��ȡddr��Ϣ
 command      : toolname cx show ddr_info
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_DDR_INFO
*****************************************************************************/
struct cmd_ddr_info_rslt {
    u8 channel;
    u8 ddr_size;
    u8 ddr_num;
    u8 ddr_total_size;
};

enum ddr_info_total_size {
    DDR_INFO_TOTAL_SIZE_256MB = 0,
    DDR_INFO_TOTAL_SIZE_512MB,
    DDR_INFO_TOTAL_SIZE_1GB,
    DDR_INFO_TOTAL_SIZE_2GB,
    DDR_INFO_TOTAL_SIZE_4GB,
    DDR_INFO_TOTAL_SIZE_8GB,
    DDR_INFO_TOTAL_SIZE_16GB,
    DDR_INFO_TOTAL_SIZE_BUTT
};

enum ddr_info_size {
    DDR_INFO_SIZE_256MB = 0,
    DDR_INFO_SIZE_512MB,
    DDR_INFO_SIZE_1GB,
    DDR_INFO_SIZE_2GB
};

/*****************************************************************************
 Description  : ��доƬefuse����
 command      : toolname cx show efuse id=x index=x count=x
 command      : toolname cx set efuse file=xxx
 Direction    : SUBOPCODE:ADM_CMD_SHOW_EFUSE ADM_CMD_SET_EFUSE
*****************************************************************************/
struct efuse_param_s {
    u8 efuse_id;
    u8 efuse_index;
    u8 efuse_count;
    u8 rsv;
};

#define CHIP_ADC_CHANNEL 5
struct cmd_chip_adc {
    int adc_val[CHIP_ADC_CHANNEL];
};

struct cmd_chip_tsensor {
    int cpu_temperature;
};

/*****************************************************************************
 Description  : ����sas��ַ
 command      : toolname cx set sas addr=xxx
 Direction    : SUBOPCODE:ADM_CMD_SET_SAS_ADDR
*****************************************************************************/
struct cmd_sas_addr {
    u64 sas_addr;
};

/*****************************************************************************
 Description  : ���õ��ӱ�ǩeLable
 command      : toolname cx set elable file=xxx
 Direction    : SUBOPCODE:ADM_CMD_SET_ELECTRON_LABEL
*****************************************************************************/
struct cmd_electron_label {
    u8 board_type[32];
    u8 board_sn[32];
    u8 item[32];
    u8 description[300];
    u8 manufactured[32];
    u8 vendor_name[32];
    u8 issue_number[32];
    u8 clei_code[32];
    u8 bom[32];
    u8 model[32];
    u8 pcb[32];
};

struct sds_com_con_para {
    u8 phy_sds_id;
    u8 lane_num;  // ��phy_sds_id��ʼ������ȡ��lane_num��
};
struct cmd_sds_com_con_para {
    struct sds_com_con_para con_para;
    u8 rsv[2];
};

/*****************************************************************************
 Description  : ������������
 command      : toolname cx set loop id=xx num=xx type=xx
                type:0-ALL_DISABLE,1-PARALLEL_TX_TO_RX,2-PARALLEL_RX_TO_TX,
                3-SERIAL_TX_TO_RX,4-SERIAL_RX_TO_TX,5-SERIAL_PRE_DRV_TX_TO_RX
 Direction    : ADM_RAID_SET SUBOPCODE:ADM_CMD_SET_LOOP_TYPE
*****************************************************************************/
enum CMD_SERDES_LOOPBACK_TYPE {
    CMD_SERDES_LOOPBACL_ALL_DISABLE = 0,             /* <ȥʹ�����л��� */
    CMD_SERDES_LOOPBACK_PARALLEL_TX_TO_RX = 1,       /* <Serdes���нӿ��ڻ� */
    CMD_SERDES_LOOPBACK_PARALLEL_RX_TO_TX = 2,       /* <Serdes���нӿ��⻷ */
    CMD_SERDES_LOOPBACK_SERIAL_TX_TO_RX = 3,         /* <Serdes���нӿ��ڻ� */
    CMD_SERDES_LOOPBACK_SERIAL_RX_TO_TX = 4,         /* <оƬ��֧�֣����нӿ��⻷ */
    CMD_SERDES_LOOPBACK_SERIAL_PRE_DRV_TX_TO_RX = 5, /* <Serdes���нӿ��ڻ�(����˽�) ����H30��Ч */
    CMD_SERDES_LOOPBACK_END
};
struct cmd_sds_set_loop_para {
    struct sds_com_con_para con_para;
    u8 loop_type;
    u8 rsv;
};

struct cmd_sds_set_loop_rlt {
    u64 rlt_bit_mask;  // ��ӦbitΪ1�����Ӧphysdsidִ��ʧ�ܣ����Դ�������id��Ч
};

/*****************************************************************************
 Description  : �������ͻ�ȡ
 command      : toolname cx show loop_type id=xx
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_LOOP_TYPE
*****************************************************************************/
struct cmd_sds_get_loop_type {
    u8 loop_type;
    u8 rsv[3];
};

/*****************************************************************************
 Description  : ��ȡprbs״̬
 command      : toolname cx show prbs_sta id=xx num=xx
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_PRBS_STA
*****************************************************************************/
struct cmd_serdes_prbs_status_s {
    u32 check_en;     /* <prbs���ʹ�� */
    u32 check_type;   /* <prbs������� */
    u32 error_status; /* <prbs��鷢�ִ��� */
    u64 error_cnt;    /* <prbs��鷢�ִ������ */
    u32 alos_status;  /* <0:δlos     1:los */
};

/*****************************************************************************
 Description  : ����prbs
 command      : toolname cx set prbs id=xx num=xx dir={tx|rx|tx_rx} type={disable|prbs7|prbs9|prbs10|
 prbs11|prbs15|prbs20|prbs23|prbs31|prbs58|prbs63|define} [data0=xx data1=xx data2=xx]
 Direction    : ADM_RAID_SET SUBOPCODE:ADM_CMD_SET_PRBS ADM_CMD_SET_PRBS_CTRL
*****************************************************************************/
enum cmd_serdes_prbs_type {
    CMD_SERDES_PRBS_DISABLE = 0,
    CMD_SERDES_PRBS_PRBS7 = 1,  /* <X^7+X^6+1 */
    CMD_SERDES_PRBS_PRBS9 = 2,  /* <X^9+X^5+1 */
    CMD_SERDES_PRBS_PRBS10 = 3, /* <X^10+X^7+1 */
    CMD_SERDES_PRBS_PRBS11 = 4, /* <X^11+X^9+1 */
    CMD_SERDES_PRBS_PRBS15 = 5, /* <X^15+X^14+1 */
    CMD_SERDES_PRBS_PRBS20 = 6, /* <X^20+X^17+1 */
    CMD_SERDES_PRBS_PRBS23 = 7, /* <X^23+X^28+1 */
    CMD_SERDES_PRBS_PRBS31 = 8, /* <X^31+X^28+1 */
    CMD_SERDES_PRBS_PRBS58 = 9,
    CMD_SERDES_PRBS_PRBS63 = 10,
    CMD_SERDES_PRBS_USER_DEFINE = 11, /* <RX USER_DEFINE   H16V200: RX Pattern0101 */
    CMD_SERDES_PRBS_END
};
struct cmd_sds_set_prbs {
    struct sds_com_con_para con_para;
    u8 dir;  // 0-tx,1-rx,2(rsv)-tx and rx
    u8 prbs_type;
    u32 cus_data[3];  // ֻ����prbs_typeΪusrdefineʱ����Ч
};

/*****************************************************************************
 Description  : ��ѯprbs����
 command      : toolname cx show prbs_type id=xx
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_PRBS_TYPE
*****************************************************************************/
struct cmd_sds_prbs_rlt {
    u16 tx_prbs_type;
    u16 rx_prbs_type;
};

/*****************************************************************************
 Description  : ��ѹ��ȡ
 command      : toolname cx show vol id=xx type={ss|cs|ds|cs_hvcc|cs_lvcc|ds_hvcc|all}
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_VOL
*****************************************************************************/
enum cmd_adc_type {
    CMD_ADC_TYPE_SS = 0,      /* <ss��ѹ */
    CMD_ADC_TYPE_CS = 1,      /* <cs��ѹ */
    CMD_ADC_TYPE_DS = 2,      /* <ds��ѹ */
    CMD_ADC_TYPE_CS_HVCC = 3, /* <HVCC��ѹ */
    CMD_ADC_TYPE_CS_LVCC = 4, /* <LVCC��ѹ */
    CMD_ADC_TYPE_DS_HVCC = 5, /* <HVCC��ѹ */
    CMD_ADC_TYPE_ALL = 6,     /* <��ѹ */
    CMD_ADC_TYPE_END
};
struct cmd_sds_vol_para {
    struct sds_com_phy_para com_para;
    u8 adc_type;
    u8 rsv[3];
};

struct cmd_sds_vol_rlt {
    u16 vol_mv;
    u8 rsv[2];
};

/*****************************************************************************
 Description  : keyinfo��ȡ ����num��
 command      : toolname cx show keyinfo id=xx num=xx
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_KEY_INFO
*****************************************************************************/
struct cmd_sds_key_info_data {
    struct sds_tx_fir_data ffe;
    struct sds_ctle_data ctle_cfg;
    struct sds_rx_dfe tap_cfg;
    u32 alos;
    u32 dsapi12;
    u32 dsapi13;
};

/*****************************************************************************
 Description  : cdr��ȡ cdr����
 command      : toolname cx show cdr id=xx
 command      : toolname cx set cdr id=xx num=xx cphs=xx ephs=xx phs=xx freq=xx
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_CDR
                ADM_RAID_SET SUBOPCODE:ADM_CMD_SET_CDR
*****************************************************************************/
struct cmd_serdes_cdr {
    u8 cphsofst;
    u8 ephsofst;
    u8 phsgn;
    u8 freqgn_cdrmode;
};

/*****************************************************************************
 Description  : ����pn
 command      : toolname cx set pn id=xx num=xx dir={tx|rx|tx_rx} pn_val={0|1}
 Direction    : ADM_RAID_SET SUBOPCODE:ADM_CMD_SET_PN
*****************************************************************************/
struct cmd_sds_set_pn {
    struct sds_com_con_para con_para;
    u8 dir;     // 0-tx,1-rx,2-tx and rx
    u8 pn_val;  // 0 or 1
};

/*****************************************************************************
 Description  : ��ȡpn
 command      : toolname cx show pn id=xx
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_PN
*****************************************************************************/
struct cmd_sds_pn_rlt {
    u16 tx_pn_val;
    u16 rx_pn_val;
};

/*****************************************************************************
 Description  : ��ȡ�̼��汾��Ϣ
 command      : toolname cx show fw_version
 Direction    : SUBOPCODE:ADM_CMD_SHOW_FW_VERSION
*****************************************************************************/
#define FW_VERSION_COUNT    10
#define FW_VERSION_VERS_MAX 16
#define FW_VERSION_TIME_MAX 20
struct fw_version_s {
    u8 build_vers[FW_VERSION_VERS_MAX];
    u8 build_time[FW_VERSION_TIME_MAX];
};
struct version_query_s {
    struct fw_version_s region_master[FW_VERSION_COUNT];
    struct fw_version_s region_slave[FW_VERSION_COUNT];
    u8 master_vers[FW_VERSION_VERS_MAX];
    u8 slave_vers[FW_VERSION_VERS_MAX];
};

/*****************************************************************************
 Description  : ��ȡ���صĹ̼��汾��Ϣ
 command      : toolname cx show inactivefw
 Direction    : SUBOPCODE:ADM_CMD_SHOW_LOAD_FW_VERSION
*****************************************************************************/
struct load_version_query_s {
    struct fw_version_s region_load[FW_VERSION_COUNT];
    u32 valid_status;  // 0:��Ч��1:��Ч
};

/*****************************************************************************
 Description  : ����log level
 command      : toolname cx set loglevel val={fatal|error|warn|print|info|debug}
 Direction    : ADM_RAID_SET SUBOPCODE:ADM_CMD_SET_LOG_LEVEL
*****************************************************************************/
struct cmd_set_log_level {
    u16 log_level;
    u16 rsv;
};

/*****************************************************************************
 Description  : ��ȡlog level
 command      : toolname cx show loglevel
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_LOG_LEVEL
*****************************************************************************/
struct cmd_get_log_level {
    u16 log_level;
    u16 rsv;
};

/*****************************************************************************
 Description  : ��ȡraid����־��Ϣ
 command      : toolname cx get log path=xxx
 Direction    : SUBOPCODE:ADM_CMD_GET_LOG
*****************************************************************************/
enum log_file_type {
    AP_LOG_TYPE,
    IMU_LOG_TYPE,
    AP_LOG_INDEX,
    IMU_LOG_INDEX,
    AP_LASTWORD_TYPE,
    LOG_BUFF_TYPE
};

struct cmd_log_info_req {
    u8 collect_type;
    u8 flag;  // ��ǰ��Ƭ����(ǰ3λ�ֱ��ʾ��ʼbit0���в�bit1��β��bit2����Ƭ�ܴ�����ʼ��β����λ)
    u16 req_seq;
    u32 req_offset;
    u32 req_len;
};

struct log_rsp_head {
    u32 total_size;
    u32 max_pay_load;  // ����֧����󷵻ص����ݳ���
    u8 collect_type;
    u8 rsv;
    u16 seq;
    u32 rsp_len;
    u32 data_slice_crc;
};

#define CHIP_MAX_PAY_LOAD_CAP 64 * 1024
struct cmd_log_info_data {
    struct log_rsp_head rsp_head;
    u8 data[CHIP_MAX_PAY_LOAD_CAP];  // ռλ����ʵ�������ڴ�
};

#define LOG_MGT_MAGIC   0x1b1c1d1e  // imu��ap��apkey��־��magic
#define FLASH_CNT_VALID 0x12345678
#define LOG_TAG_ID      0x7fff

enum LOG_TAG_TYPE {
    LOG_MAGIC_TAG,
    LOG_VER_TAG
};

typedef struct {
    u32 tag_type;
    u32 area;
    u32 magic;
    u32 rsv;
} magic_tag;  // user_val

typedef struct {
    u32 tag_type;
    u32 area;
    u16 ver[4];
} version_tag;  // user_val

/*****************************************************************************
 Description  :
 command      : toolname cx set mbist [flag=xx]
                toolname cx show mbist
 Direction    : SUBOPCODE:ADM_CMD_SET_MBIST
                SUBOPCODE:ADM_CMD_SHOW_MBIST
*****************************************************************************/
#define MBIST_REQ_MAGIC     0x5a5a5a5a
struct cmd_mbist_request {
    u8 verify_flag;
    u8 rsved[3];
};

struct cmd_mbist_status {
    u8 fail_count;
    u8 pass_count;
    u8 rsved[2];
};

/*****************************************************************************
 Description  :
 command      : toolname cx show ddr_eye_diagram
 Direction    : SUBOPCODE:ADM_CMD_SHOW_DDR_EYE
*****************************************************************************/
#define DDR_CHANNEL_NUM 2
#define DDR_BIT_NUM 72
struct ddr_eye_bit {
    short RxDly[2];
    short RxVrf[2];
    short TxDly[2];
    short TxVrf[2];
};

struct cmd_ddr_eye_info {
    struct ddr_eye_bit ddr_eye_bit[DDR_CHANNEL_NUM][DDR_BIT_NUM];  // DDR����channel, ��72bit
};

/*****************************************************************************
 Description  :
 command      : toolname cx set loopback sw={on|off} phyid={0-39} rate=[1.5|3|6|12]
 codetype=[0|1|2|3|4|5|6|7|8|9|10] mode=[0|1]
 Direction    : SUBOPCODE:ADM_CMD_SET_DISK_LOOPBACK
*****************************************************************************/

enum loop_back_mode {
    LOOP_BACK_MODE_IN = 0,
    LOOP_BACK_MODE_OUT,
};

enum loop_back_type {
    LOOP_BACK_TYPE_PRBS7 = 0,
    LOOP_BACK_TYPE_PRBS23,
    LOOP_BACK_TYPE_PRBS31,
    LOOP_BACK_TYPE_JTPAT,
    LOOP_BACK_TYPE_CJTPAT,
    LOOP_BACK_TYPE_SCRAMBED_0,
    LOOP_BACK_TYPE_TRAIN,
    LOOP_BACK_TYPE_TRAIN_DONE,
    LOOP_BACK_TYPE_HFTP,
    LOOP_BACK_TYPE_MFTP,
    LOOP_BACK_TYPE_LFTP,
};

enum loop_back_rate {
    LOOP_BACK_RATE_1P5G = 8,
    LOOP_BACK_RATE_3G,
    LOOP_BACK_RATE_6G,
    LOOP_BACK_RATE_12G,
};

struct cmd_start_disk_loopback {
    u8 sw;  // 0�� 1��
    u8 phyid;
    u8 rsv[2];
    u32 rate;       // ����
    u32 code_type;  // ����
    u32 mode;       // 0:�ڻ� 1 �⻷
};

struct cmd_loopback_rslt {
    u8 sw;  // 0�� 1��
    u8 rsv[3];
    u32 code_type;
    u32 status;
};

/*****************************************************************************
 Description  : SAS DISK Firmware actice
 command      : SAS EXPANDER��PCIE SW�̼������Ƿ���Ҫ�赲IO
 Direction    : opcode:ADM_CMD_ACTIVE_DISK_FW;
*****************************************************************************/
struct adm_disk_fw_active {
    struct multi_disk_location loc;
    u16 io_break;  // 1:���ж�IOҵ��0:�ж�IOҵ��
    u16 rsvd;
};

/*****************************************************************************
 Description  :
 command      : toolname cx:ex:sx set ncq sw={on|off}
 Direction    : subopcode:ADM_CMD_SET_NCQ;
*****************************************************************************/
struct cmd_set_ncq {
    struct multi_disk_location loc;
    u32 sw;  // 0�� 1��
};

enum set_slow_disk_type {
    SET_SLOW_DISK_TYPE_RAID,
    SET_SLOW_DISK_TYPE_RAWDRIVE
};
/*****************************************************************************
 Description  :
 command      : toolname cx set slowdrive type={raid|rawdrive} [sw={on|off}] [ratio=x] [media={ssd|hdd}]
                [susize={256|1024}] [l1=x] [l2=x] [l3=x] [default]
 Direction    : subopcode:ADM_CMD_SET_SLOWDISK;
*****************************************************************************/
struct cmd_set_slow_disk {
    u8 sw;           // 0 �� 1�� 2default
    u8 type;         // enum set_slow_disk_type
    u16 su_size;     // ADM_SUSZ_256, ADM_SUSZ_1024, 0 unknown
    u8 ratio;        // 0 unknown
    u8 default_set;  // 0 unknown, 1 set default
    u8 media;        // 0 hdd, 1 ssd, 2 unknown
    u8 rsv[3];
    u16 l1;  // 0 unknown
    u16 l2;  // 0 unknown
    u16 l3;  // 0 unknown
};

struct raid_threshold {
    u8 media;  // 0 hdd, 1 ssd, 2 unknown
    u8 rsv[3];
    u16 su_size;  // ADM_SUSZ_256, ADM_SUSZ_1024, 0 unknown
    u16 l1;       // 0 unknown
    u16 l2;       // 0 unknown
    u16 l3;       // 0 unknown
};

struct rawdrive_threshold {
    u8 media;  // 0 hdd, 1 ssd, 2 unknown
    u8 rsv;
    u16 l2;  // 0 unknown
};

struct slow_disk_config {
    u8 ratio;  // ֻ������raid
    u8 rsv[3];
    struct raid_threshold raid_info[3];
    struct rawdrive_threshold rawdrive_info[2];
};

/*****************************************************************************
 Description  :
 command      : toolname cx show slowdrive
 Direction    : subopcode:ADM_CMD_SHOW_SLOWDISK;
*****************************************************************************/
struct cmd_show_slow_disk {
    u8 raid_sw;  // 0�� 1��
    u8 raw_sw;   // 0�� 1��
    u8 rsv[2];
    struct slow_disk_config current_config;
    struct slow_disk_config next_config;
};

/*****************************************************************************
 Description  :
 command      : toolname cx set bpselflearn time=xx volta_vc=xx volta_vc_to_vd=xx cap=xx
 Direction    : subopcode:ADM_CMD_SET_SELF_LEARN;
*****************************************************************************/
struct cmd_bp_cap_operation {
    u32 va_time;
    u32 volta_vc;
    u32 volta_vc_to_vd;
    u32 cap_i_ma;
};

struct cmd_bp_cap_learn_time {
    u32 total_time;  // ��λ����
};

struct cmd_bp_cap_retrive {
    u64 super_cap_esr;  // super_cap_esr/100
    u64 super_cap_cf;   // super_cap_cf/100
    u8 status;          // ѧϰ״̬
    u8 rsv;
    u16 cap_voltage;    // �ߵ�λ��ѹֵ mV
};

struct cmd_set_backupcap {
    u64 backup_cap_cf;
};

/*****************************************************************************
 Description  :
 command      : toolname cx show smcapv
 Direction    : subopcode:ADM_CMD_SHOW_SM_CAP_V;
*****************************************************************************/
struct cmd_bp_samll_cap_volta {
    u32 small_cap_volta; //   small_cap_volta/1000 V
};

struct cmd_map_ppn_rslt {
    u8 ch[2];
    u8 ce[2];
    u8 die[2];
    u8 plane[2];
    u16 page[2];
    u16 block[2];
};

/*****************************************************************************
 Description  : dump���мĴ���
 command      : toolname cx show allreg type={sds|fe|bi|cache|raid|all}
                [macroid=0-7|0xff] outfile=xx
 Direction    : subopcode:ADM_CMD_SHOW_ALLREG;
*****************************************************************************/
enum {
    GET_DUMP_SUB_TYPE,       // ������type��ȡ֧����type
    GET_SUB_TYPE_ADDR_INFO,  // ���ص�ַ num���Լ�title
    GET_REG_DATA_BY_ADDR,    // ���ݵ�ַ�ĳ��ȷ����λ�ȡ
};

enum reg_dump_type {
    DUMP_SDS_REG,
    DUMP_FE_REG,
    DUMP_BE_REG,
    DUMP_CACHE_REG,
    DUMP_RAID_REG,
    DUMP_SOC_REG,
    DUMP_BUTT
};

typedef struct {
    u8 macro_id;  // SDSר��0-7 ����macroid 0xff����ȫ�� macroid
    u8 rsv[3];
} sds_type_req;

typedef struct {
    u8 rsv[4];
} extra_type_req;

typedef struct {
    u64 reg_start_addr;
    u32 reg_num;
} reg_req_info;

#pragma pack(4)
struct cmd_reg_dump_param {
    u8 op_type;
    u8 rsv1;
    u16 reg_type;
    union {
        extra_type_req extra_req;
        struct {
            u16 sub_type;
        } sub_addr_req;
        struct {
            reg_req_info reg_req;
            u16 sub_type;
        } addr_data_req;
        u64 data[2];
    };
};
#pragma pack()

#define MAX_DUMP_SUB_TYPE_NUM 512  // ������Ҫ����
struct dump_sub_type_info {
    u16 sub_type_num;
    u16 dump_sub_type[MAX_DUMP_SUB_TYPE_NUM];
};

struct dump_sub_type_addr_info {
    u8 sub_title[32];
    reg_req_info reg_req;
};

/*****************************************************************************
 Description  : ����Ӳ�̵�ʡ��ģʽ
 command      : toolname cx set powersave sw={on|off} [time=xx]
 Direction    : subopcode:ADM_CMD_SET_POWERSAVE;
*****************************************************************************/
struct cmd_set_powersave {
    u32 time;       // [30,120] ����
    u8 sw;          // 0�� 1��
    u8 rsvd[3];
};

/*****************************************************************************
 Description  :
 command      : toolname cx:ex:sx set powersave sw={on|off} [pc=xx] [pcmod=xx]
 Direction    : subopcode:ADM_CMD_SET_DISK_POWERSAVE;
*****************************************************************************/
struct cmd_set_power_save {
    struct multi_disk_location loc;
    u32 sw;      // 0�� 1��
    u32 pc;      // 2          3
    u32 pc_mod;  // 0 1 2      1
};

/*****************************************************************************
 Description  : ��ѯ����������ѧϰ��Ϣ
 command      : toolname cx:sc show selflearn
 Direction    : subopcode:ADM_CMD_SHOW_SELFLEARN;
*****************************************************************************/
struct cmd_bp_selfleran_rslt {
    u8 present;           // ��λ״̬(0-����λ��1-��λ)
    u8 last_learn_status; /* �ϴ���ѧϰ���״̬ ��ѧϰ״̬:
                        0: ������ѧϰ��bitλ��1��Ч��
                        1: ��ѧϰ�ɹ���bitλ��1��Ч��
                        2: ��ѧϰʧ�ܣ�bitλ��1��Ч��
                        3: ��ѧϰ��ʱ��bitλ��1��Ч��
                        ȫ0:û����ѧϰ
                    */
    u8 rsv[2];
    u32 learn_cycle;      // relearn���ڣ���λ��
    u64 last_learn_time;  // �ϴ���ѧϰʱ��
    u64 next_learn_time;  // �´�relearn��ʼʱ�䣬�뼶ʱ���
};

/*****************************************************************************
 Description  : ���ó���������ѧϰ����
 command      : toolname cx:sc set learn cycle=xx starttime=yyyy-MM-dd,hh:mm:ss
 Direction    : subopcode:ADM_CMD_SET_SELFLEARN_CYCLE;
*****************************************************************************/
struct cmd_bp_selfleran_cycle {
    u32 learn_cycle;      // relearn���ڣ���λ��
    u32 next_learn_time;       // ��λ:��
};

/*****************************************************************************
 Description  : ���ñ���ģ���¶�����
 command      : toolname cx:sc set alarm temp=xx
 Direction    : subopcode:ADM_CMD_SET_ALARMTEMP;
*****************************************************************************/
struct cmd_set_bp_alarmtemp {
    int temperature;
};

/*****************************************************************************
 Description  : ����UEFI��BIOS����ϱ���������
 command      :
 Direction    : subopcode:ADM_CMD_SET_REPORT_DRIVE_NUM;
*****************************************************************************/
struct cmd_set_report_drive_num {
    u8 max_drive_for_uefi;
    u8 rsvd[3];
};

typedef union {
    /* Define the struct bits */
    struct {
        /* DWORD0 */
        unsigned int event_type      : 3;   /* [0..2] �Զ�����Ϣ */
        unsigned int rsv             : 5;   /* [3..7] Ԥ�� */
        unsigned int event_code      : 8;   /* [8..15] �¼��� */
        unsigned int event_location  : 4;   /* [16..19] �¼�λ�� */
        unsigned int event_level     : 4;   /* [20..23] �¼����� */
        unsigned int parame0         : 8;   /* [24..31] ����0 */
        /* DWORD1 */
        unsigned int parame1         : 32;  /* ����1 */
    } bits;

    unsigned long long result;
} event_report_info;

#define EVENT_VENDOR_SPECIFIC    0x7

enum event_code {
    FLASH_ERR = 139,
    CPU_OVERTEMP = 140,
    CPU_OVERTEMP_ALARM,
    CPU_TEMP_REVER,
    SDS_DOWN_REFCLK_WARN = 143,
    SDS_DOWN_REFCLK_RECOVER = 144,
    PCIE_CTRL_WARN = 145,
    PCIE_CTRL_RECOVER = 146
};

enum event_location {
    EVENT_CONTROLLER = 0,
    EVENT_PD,
    EVENT_VD,
    EVENT_RD,
    EVENT_ENCL,
    EVENT_SC
};

enum event_level {
    EVENT_FATAL = 0,
    EVENT_CRITICAL,
    EVENT_WARNING,
    EVENT_INFO,
};

enum file_type {
    DUMP_FILE_TYPE,      // cx get dump path=xx [type=0] ��norflashȡ
    DUMP_RAM_FILE_TYPE,  // cx get dump path=xx [type=1] ��ramȡ�������֧��ram�����norflashȡ
    NAND_FILE_TYPE       // cx get nandlog path=xx [id=x]
};

/*****************************************************************************
 Description  : �ռ�dump����
 command      : toolname cx get dump path=xx
 Direction    : SUBOPCODE:ADM_CMD_GET_DUMP
*****************************************************************************/
#define REG_DUMP_MAGIC  0x5b5c5d5e
#define DATA_DUMP_MAGIC 0x5e5f5051
#define RAS_DUMP_TYPE 0x1
#define REG_DUMP_TYPE 0x2
struct reg_dump_header {
    u32 magic;
    u8 type;
    u8 rsv1[3];
    u64 time_ms;
};

/*****************************************************************************
 Description  : nand�ϻ�����
 command      : toolname cx set retention turn=x
 Direction    : SUBOPCODE:ADM_CMD_SET_RETENTION
******************************************************************************/
typedef enum {
    NO_CAUSE_LOG_T = 0,
    CAUSE_UNC_T,
    CAUSE_PSF_T,
    CAUSE_ESF_T,
    CAUSE_PSF_UNC_T,
    RD_RISK_FLAG_T,
    HIGH_ERROR_BIT_T,
    ERASE_TIMEOUT_T,
    PROG_TIMEOUT_T,
    OTHER_CAUSE_T
} GBB_CAUSE_T;

struct bp_gbb_log {
    u8 turn;
    u8 ch;
    u8 ce;
    u8 lun;
    u8 plane;
    u8  rsv;
    u16 block;
    u16 page;
    u16 temperature;
    GBB_CAUSE_T cause;
};

struct bp_retention_info {
    u8 success;
    u8 rsv[3];
    u32 data_len;
};

struct bp_retention_req {
    u8 idx;
    u8 rsv[3];
    u32 data_len;
};

/*****************************************************************************
 Description  : ��ѯ����pinnedcache��vd
 command      : toolname cx show pinnedcache
 Direction    : SUBOPCODE:ADM_CMD_SHOW_PINNEDCACHE_LIST
*****************************************************************************/

struct cmd_pinnedcache_info {
    u8 vdid;
    u8 status;
};

#define PINNEDCACHE_LIST_MAX 127
struct cmd_pinnedcache_list {
    u32 cnt;
    struct cmd_pinnedcache_info info[PINNEDCACHE_LIST_MAX];
};

/*****************************************************************************
 Description  : ������������
 command      : toolname cx set kickdrive [period=x(1-90)day] [sum=x(1-8)]
 Direction    : SUBOPCODE:ADM_CMD_SET_KICK_DISK_CYCLE
*****************************************************************************/
struct cmd_set_kickdisk {
    u8 period;   // MASK_OF_STRUCT_INDEX_FIRST
    u8 sum;      // MASK_OF_STRUCT_INDEX_SECOND
    u8 rsvd;
    u8 mask;
    u32 rsvd1[3];
};

/**
 * @brief ��ѯ��ǰ��������״��
 * @command toolname cx show kickdrive
 * @subopcode ADM_CMD_SHOW_KICK_DRIVE_STATUS
 */
struct cmd_show_kickdrive {
    u8 period; /* ��ǰ���������������� */
    u8 sum; /* ��ǰ�������������������� */
    u8 kick_sum; /* �������������� */
    u8 rsvd[53];
    u64 reset_remaining_time; /* ����������������ʣ��ʱ�䣬��λ��min */
};

/*****************************************************************************
 Description  : �û��ָ����쳣�Ĳ���
 command      :
 Direction    : SUBOPCODE:ADM_CMD_USER_RECOVERY ADM_CMD_USER_RECOVERY_PROCESS
*****************************************************************************/
enum {
    RECOVER_WRITE_HOLE = 1,         // WRITE_HOLE_CLEAR
    RECOVER_PINNED_CACHE = 2,       // PINNED_CACHE_CLEAR
    RECOVER_BAD_CARD = 3,           // NONE
    RECOVER_RESTORE_DATA = 4,       // RESTORE_DATA | DROP_DATA
    RECOVER_DROP_DATA = 5,          // DROP_DATA
    RECOVER_RESETTING_REQUIRED = 6, // RESETTING_REQUIRED
    MAX_RECOVERY_TYPE
};

enum {
    WRITE_HOLE_CLEAR = 1,
    PINNED_CACHE_CLEAR = 2,
    RESTORE_DATA = 3,
    DROP_DATA = 4,
};

struct cmd_user_recovery {
    u16 opcode;
    u16 operation;
    u8 param[20];
};

struct cmd_user_recovery_default {
    u32 default_on;
};

/*****************************************************************************
 Description  : �������̵��������ޣ�fc -> flow control)
 command      : toolname cx set drivefc mode={high|low} [media=hdd|ssd] Ĭ��hdd
                toolname cx set vdfc mode={high|low|medium}
 Direction    : SUBOPCODE:ADM_CMD_SET_DRIVER_FC, ADM_CMD_SET_VD_FC
*****************************************************************************/
enum drivefc_mode {
    DRIVEFC_LOW = 0,
    DRIVEFC_HIGH = 1,
    DRIVEFC_MEDIUM = 2,
    DRIVEFC_MAX
};
struct cmd_set_drive_fc {
    u8 drivefc_mode;
    u8 media;  // 0 hdd, 1 ssd
    u8 rsv[2];
};
struct cmd_drive_fc_cfg {
    u32 current_mode;
    u32 next_mode;
    u32 rsv[6];
};

/*****************************************************************************
 Description  : ��ѯ���̵��������ޣ�fc -> flow control) ������ѯ��Ϣ
 command      : toolname cx show drivefc
 Direction    : SUBOPCODE:ADM_CMD_SHOW_DRIVER_FC_EXPAND
*****************************************************************************/
struct cmd_show_drive_fc_rslt {
    struct cmd_drive_fc_cfg config[8];  // 0 hdd, 1 ssd ����Ԥ��
};

/*****************************************************************************
 Description  : ��ѯvd���������ޣ�fc -> flow control) ������ѯ��Ϣ
 command      : toolname cx show vdfc
 Direction    : SUBOPCODE:ADM_CMD_SHOW_VD_FC
*****************************************************************************/
struct cmd_show_vd_fc_cfg {
    struct cmd_drive_fc_cfg config[8];  // 0 vd ����Ԥ��
};

/*****************************************************************************
 Description  : �����ռ�nandlog������
 command      :
 Direction    : SUBOPCODE:ADM_CMD_SET_NAND_LOG_IDX
*****************************************************************************/
struct cmd_set_nand_log_idx {
    u8 idx;  // 0xFFΪȫ���ռ�
    u8 rsv[3];
};

enum area_type {
    APLOG_ATTR,
    APKEYLOG_ATTR,
    IMULOG_ATTR,
    DUMP_ATTR,
    LASTWORD_ATTR,
    AREA_ATTR_NUM
};
struct nandlog_desc {
    u8 area_type;  // enum area_type
    u8 rsv[3];
    u32 area_len;  // ����ĳ���
};
#define NANDLOG_DESC_MAX_LEN 7
#define NANDLOG_MAGIC        0x5A5B5C5D
struct nandlog_bin_header {
    u32 nand_log_magic;  // NANDLOG_MAGIC
    u8 nand_log_cnt;     // һ��nandlog.bin��nandlog������
    u8 rsv[2];
    u8 desc_valid_num;  // ÿһ��nandlog���ݵ�����
    struct nandlog_desc desc[NANDLOG_DESC_MAX_LEN];
};

#define OPER_FLASH_LOG_MAGIC 0x1b1c1d1e
typedef struct {
    u32 magic;  // OPER_FLASH_LOG_MAGIC
    u32 rsv[7];
} flash_log_header;

/*****************************************************************************
 Description  : ����оƬ��SAS��·������ַ���ģʽ
 command      : toolname cx set dphstartmod  [sas0={0|1}]  [sas1={0|1}] [sas2={0|1}]  [sas3={0|1} ] [sas4={0|1}]
 Direction    : SUBOPCODE:ADM_CMD_SET_DPHSTARTMOD
*****************************************************************************/
#define SAS_IP_MAX 5
struct cmd_set_dph_mod_para {
    u8 sas_dph_mod[SAS_IP_MAX];
    u8 mask;  // bit0-bit4��ʾ�û�����sas0-sas4
    u8 rsv0[2];
};

/*****************************************************************************
 Description  : ��ѯоƬ��SAS��·������ַ���ģʽ
 command      : toolname cx show dphstartmod
 Direction    : SUBOPCODE:ADM_CMD_SHOW_DPHSTARTMOD
*****************************************************************************/
struct cmd_show_dph_mod_rslt {
    u8 cur_sas_ip_num;            // ��ǰ��Чsas ip����
    u8 rsv0[3];
    u8 cur_dph_mod[SAS_IP_MAX];   // ��ǰ����
    u8 rsv1[3];
    u8 next_dph_mod[SAS_IP_MAX];  // �´���Ч����
    u8 rsv2[3];
};

struct cmd_rg_vd_pdcache_param {
    struct cmd_member_id loc;
    u8 pdcache; // ֵ�����enum adm_disk_cahe_switch
    u8 rsvd[3];
};

/*****************************************************************************
 Description  : ����dha����
 command      : toolname cx set dha sw={on|off}
 Direction    : SUBOPCODE:ADM_CMD_SET_DHA_SW
*****************************************************************************/
struct cmd_set_dha_sw {
    u8 dha_sw;  // 0:close 1:open
    u8 rsv[3];
};

/*****************************************************************************
 Description  : ����quedepth
 command      : toolname cx set quedepth type={rawdrive|raid} [sashdd=xx] [sasssd=xx] [satahdd=xx] [satassd=xx]
 Direction    : SUBOPCODE:ADM_CMD_SET_QUEUE_DEPTH
*****************************************************************************/
enum que_depth_type {
    QUE_DEPTH_TYPE_RAID,
    QUE_DEPTH_TYPE_RAWDRIVE,
    QUE_DEPTH_TYPE_BUTT
};

struct cmd_que_depth_cfg {
    u8 sas_hdd;
    u8 sas_ssd;
    u8 sata_hdd;
    u8 sata_ssd;
    u8 rsv[4];
};

struct cmd_set_que_depth {
    u8 type;  // enum que_depth_type
    u8 bitmask;  // ֧���û�ͬʱ����һ��������������ӦbitΪ1��ʾ�û����ã�Ϊ0��ʾ�û�δ����
                 // bit0: sas_hdd, bit1: sas_ssd, bit2: sata_hdd, bit3: sata_ssd
    u8 rsv[2];
    struct cmd_que_depth_cfg set_cfg;
};

/*****************************************************************************
 Description  : ��ѯquedepth
 command      : toolname cx show quedepth
 Direction    : SUBOPCODE:ADM_CMD_SHOW_QUEUE_DEPTH
*****************************************************************************/
struct cmd_show_que_depth {
    struct cmd_que_depth_cfg current_cfg[QUE_DEPTH_TYPE_BUTT];
    struct cmd_que_depth_cfg next_cfg[QUE_DEPTH_TYPE_BUTT];
    u32 rsv[16];  // Ԥ��
};

/*****************************************************************************
 Description  : ����io�ϲ�����
 command      : toolname cx set merge [sw=on|off] [timeout=default|x] [count=default|x]
 Direction    : SUBOPCODE:ADM_CMD_SET_MERGE
*****************************************************************************/
struct cmd_set_merge_cfg {
    u8 sw;  // 0:off 1:on
    u8 rsv[7];
    u32 timeout;  // U32_MAX:default  ��λ��us
    u32 count;    // U32_MAX:default
    u32 bitmask;  // bit0:sw, bit1:timeout, bit2:count
};

/*****************************************************************************
 Description  : ��ѯio�ϲ�����
 command      : toolname cx show merge
 Direction    : SUBOPCODE:ADM_CMD_SHOW_MERGE
*****************************************************************************/
struct cmd_show_merge_cfg {
    u8 sw;  // 0:off 1:on
    u8 rsv[7];
    u32 timeout;
    u32 count;
    u32 rsv2[4];
};

/*****************************************************************************
 Description  : ����vd io�ϲ�����
 command      : toolname cx set vdmerge vd=x [stripe_len=default|x] [start_len=default|x] [merge_len=default|x]
 Direction    : SUBOPCODE:ADM_CMD_SET_VD_MERGE
*****************************************************************************/
struct cmd_vd_merge_cfg {
    u8 vdid;
    u8 rsv[3];
    u32 stripe_len;  // U32_MAX:default ��λ��4kb
    u32 start_len;   // U32_MAX:default ��λ��4kb
    u32 merge_len;   // U32_MAX:default ��λ��4kb
};

struct cmd_set_vd_merge_cfg {
    u8 bitmask;  // bit0:stripe_len ,bit1: start_len, bit2:merge_len
    u8 rsv[3];
    struct cmd_vd_merge_cfg set_cfg;
};

/*****************************************************************************
 Description  : ��ѯvd io�ϲ�����
 command      : toolname cx show vdmerge [vd=x|all]
 Direction    : SUBOPCODE:ADM_CMD_SHOW_VD_MERGE
*****************************************************************************/
struct cmd_show_vd_merge_param {
    u8 vdid;  // U8_MAX:all
    u8 rsv[3];
};

#define VD_MAX_NUM 127
struct cmd_show_vd_merge_cfg {
    u8 vd_num;  // ��ѯ����vd����
    u8 rsv[3];
    struct cmd_vd_merge_cfg show_cfg[VD_MAX_NUM];
};

/*****************************************************************************
 Description  : set tsp freq
 command      : toolname cx set tspfreq mode={low|high}
 Direction    : SUBOPCODE:ADM_CMD_SET_TSP_FREQ
*****************************************************************************/
enum tsp_freq_mode {
    TSP_FREQ_LOW = 0,  /* 426MHz */
    TSP_FREQ_HIGH = 1, /* 800MHz */
    TSP_FREQ_BUTT
};

struct cmd_set_tsp {
    u32 freq_mode;
};

/*****************************************************************************
 Description  : show tsp freq
 command      : toolname cx show tspfreq
 Direction    : SUBOPCODE:ADM_CMD_SHOW_TSP_FREQ
*****************************************************************************/
struct cmd_show_tsp {
    u32 cur_freq_mode;
    u32 next_freq_mode;
};

#pragma pack(pop)
#endif  // INCLUDE_OPEN_API_LIB_API_H
