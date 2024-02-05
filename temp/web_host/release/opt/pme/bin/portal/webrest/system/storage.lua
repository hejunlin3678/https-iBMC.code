local utils = require 'utils'
local dflib = require 'dflib'
local c = require 'dflib.core'
local cjson = require 'cjson'
local http = require 'http'
local bit = require 'bit'
local logging = require 'logging'
local math = require 'math'
local defs = require 'define'
local cfg = require 'config'
local download = require 'download'
local tools = require 'tools'
local C = dflib.class
local O = dflib.object
local null = cjson.null
local GVariant = c.GVariant
local gbyte = GVariant.new_byte
local guint16 = GVariant.new_uint16
local guint32 = GVariant.new_uint32
local gstring = GVariant.new_string
local gvstring = GVariant.new_vstring
local new_fixed_array = GVariant.new_fixed_array
local call_method = utils.call_method
local object_name = dflib.object_name
local reply_ok_encode_json = http.reply_ok_encode_json
local reply_bad_request = http.reply_bad_request
local reply_ok = http.reply_ok
local safe_call = http.safe_call
local dal_set_file_owner = utils.dal_set_file_owner
local dal_set_file_mode = utils.dal_set_file_mode
local CTRL_OPTION2_SUPPORT_CRYPTO_ERASE = 0x20000

local CTRL_VENDER_INDEX = {
    VENDER_LSI = 1,
    VENDER_PMC = 2,
    VENDER_HUAWEI = 3
}

local RAID_CTRL_MODE_NAME = {
    CTRL_MODE_RAID_STR = 'RAID',
    CTRL_MODE_HBA_STR = 'HBA',
    CTRL_MODE_JBOD_STR = 'JBOD',
    CTRL_MODE_MIXED_STR = 'Mixed',
    CTRL_MODE_UNKNOWN_STR = 'N/A'
}

local CTRL_MODE_E = {
    CTRL_MODE_RAID = 0,
    CTRL_MODE_HBA = 1,
    CTRL_MODE_JBOD = 2,
    CTRL_MODE_MIXED = 3,
    CTRL_MODE_UNKNOWN = 0xff -- 获取失败时候的默认值
}
local CTRL_DRIVE_WCP_NAME = {
    CTRL_DRIVE_WCP_DEFAULT_STR = 'Default',
    CTRL_DRIVE_WCP_ENABLED_STR = 'Enabled',
    CTRL_DRIVE_WCP_DISABLED_STR = 'Disabled',
    CTRL_DRIVE_WCP_INVALID_STR = 'N/A'
}

local CTRL_DRIVE_WCP_E = {
    CTRL_DRIVE_WCP_DEFAULT = 0,
    CTRL_DRIVE_WCP_ENABLED = 1,
    CTRL_DRIVE_WCP_DISABLED = 2,
    CTRL_DRIVE_WCP_INVALID = 3
}

local STRIPE_SIZE_SCHE = {
    STRIPE_SIZE_16K_SCHE = 16 * 1024,
    STRIPE_SIZE_32K_SCHE = 32 * 1024,
    STRIPE_SIZE_64K_SCHE = 64 * 1024,
    STRIPE_SIZE_128K_SCHE = 128 * 1024,
    STRIPE_SIZE_256K_SCHE = 256 * 1024,
    STRIPE_SIZE_512K_SCHE = 512 * 1024,
    STRIPE_SIZE_1M_SCHE = 1024 * 1024
}

local STRIPE_SIZE_BMC = {
    STRIPE_SIZE_16K_BMC = 5,
    STRIPE_SIZE_32K_BMC = 6,
    STRIPE_SIZE_64K_BMC = 7,
    STRIPE_SIZE_128K_BMC = 8,
    STRIPE_SIZE_256K_BMC = 9,
    STRIPE_SIZE_512K_BMC = 10,
    STRIPE_SIZE_1M_BMC = 11,
    STRIPE_SIZE_DEFAULT_BMC = 255
}

local STOTAGE_INFO_INVALID = {
    STORAGE_INFO_INVALID_BYTE = 0xFF,
    STORAGE_INFO_INVALID_WORD = 0xFFFF,
    STORAGE_INFO_INVALID_DWORD = 0xFFFFFFFF
}

local LOGENTRY_SEVERITY = {
    LOGENTRY_SEVERITY_OK = 'OK',
    LOGENTRY_SEVERITY_WARNING = 'Warning',
    LOGENTRY_SEVERITY_CRITICAL = 'Critical'
}

local ALARM_LEVEL = {
    MAJOR_STRING = 'Major',
    MINOR_STRING = 'Minor',
    CRITICAL_STRING = 'Critical',
    INFORMATIONAL_STRING = 'Informational'
}

local BYTE_START_BIT = {
    FIRST_BYTE_START_BIT = 0,
    SECOND_BYTE_START_BIT = 8,
    THIRD_BYTE_START_BIT = 16,
    FOURTH_BYTE_START_BIT = 24
}

-- 一致性校验的信息转换
local CONSIS_CHECK_PARAM = {
    CC_DISABLED_N = 0,
    CC_EANBLED_N = 1,
    CC_ENABLED_SET_N = 2,
    CC_PERIOD_MIN = 24,
    CC_PERIOD_MAX = 1440,
    CC_RATE_LOW_N = 1,
    CC_RATE_MIDDLE_N = 2,
    CC_RATE_HIGH_N = 3,
    CC_RATE_LOW_S = 'Low',
    CC_RATE_MIDDLE_S = 'Medium',
    CC_RATE_HIGH_S = 'High',
    CC_MASK_PERIOD = 0x01,
    CC_MASK_RATE = 0x02,
    CC_MASK_REPAIR = 0x04,
    CC_MASK_DELAY = 0x08,
    CC_DELAY_MAX = 24
}

local SUPORTED_POLICE_E = {
    LD_WRITE_THROUGH = 'WriteThrough',
    LD_WRITE_BACKBBU = 'WriteBackWithBBU',
    LD_WRITE_BACK = 'WriteBack',
    LD_WRITE_RO = 'ReadOnly',
    LD_READ_NO = 'NoReadAhead',
    LD_READ_AHEAD = 'ReadAhead',
    LD_IO_CACHE = 'CachedIO',
    LC_IO_DIRECT = 'DirectIO',
    LD_ACC_RW = 'ReadWrite',
    LD_ACC_READ = 'ReadOnly',
    LD_ACC_BLOCK = 'Blocked',
    LD_ACC_HIDDEN = 'Hidden',
    LD_DCACHE_DEF = 'Unchanged',
    LD_DCACHE_ENABLE = 'Enabled',
    LD_DCACHE_DISABLE = 'Disabled',
    LD_INITMODE_UN = 'UnInit',
    LD_INITMODE_QUICK = 'QuickInit',
    LD_INITMODE_FULL = 'FullInit',
    LD_INITMODE_RPI = 'RPI',
    LD_INITMODE_OPO = 'OPO',
    LD_INITMODE_FRONT = 'Front',
    LD_INITMODE_BACKGROUND = 'Background',
    LD_DCACHE_DISK_DEF = 'Disk\'sDefault'
}

local SML_RAID_LEVEL_E = {
    SML_RAID_LEVEL_0 = 'RAID0',
    SML_RAID_LEVEL_1 = 'RAID1',
    SML_RAID_LEVEL_5 = 'RAID5',
    SML_RAID_LEVEL_6 = 'RAID6',
    SML_RAID_LEVEL_10 = 'RAID10',
    SML_RAID_LEVEL_50 = 'RAID50',
    SML_RAID_LEVEL_60 = 'RAID60',
    SML_RAID_LEVEL_1ADM = 'RAID1(ADM)',
    SML_RAID_LEVEL_10ADM = 'RAID10(ADM)',
    SML_RAID_LEVEL_1TRIPLE = 'RAID1Triple',
    SML_RAID_LEVEL_10TRIPLE = 'RAID10Triple'
}

local CONTROLLER_TYPE_ID_E = {
    LSI_3108_WITH_MR = 0,
    LSI_3108_WITH_IR = 1,
    LSI_3108_WITH_IT = 2,
    LSI_3008_WITH_MR = 3,
    LSI_3008_WITH_IR = 4,
    LSI_3008_WITH_IT = 5,
    LSI_3508_WITH_MR = 6,
    LSI_3516_WITH_MR = 7,
    LSI_3408_WITH_IMR = 8,
    LSI_3416_WITH_IMR = 9,
    LSI_3408_WITH_IT = 10,
    LSI_3416_WITH_IT = 11,
    LSI_3004_WITH_IMR = 12,
    LSI_3008_WITH_IT_PCIE = 13,
    LSI_3908_WITH_MR = 14,
    LSI_3916_WITH_MR = 15,
    LSI_3808_WITH_MR = 16,
    LSI_3816_WITH_IMR = 17,
    LSI_3808_WITH_IMR = 18,
    PMC_3152_8I_SMART_RAID = 64,
    PMC_2100_8I_SMART_HBA = 65,
    HI1880_SP186_M_16i = 96,
    HI1880_SP186_M_32i = 97,
    HI1880_SP186_M_40i = 98,
    HI1880_SP686C_M_16i_2G = 99,
    HI1880_SP686C_M_16i_4G = 100,
    HI1880_SP686C_MH_32i_4G = 101,
    HI1880_SP686C_M_40i_2G = 102, 
    HI1880_SP686C_M_40i_4G = 103,
    HI1880_SPR120 = 104,
    HI1880_SPR130 = 105,
    HI1880_SPR140 = 106,
    HI1880_SPR180 = 107,
    HI1880_SPR185 = 108,
    HI1880_SPR190 = 109,
    HI1880_SPR1A0 = 110,
    HI1880_SPR1A5 = 111,
    HI1880_SPR110 = 112,
    HI1880_SP186_M_8i = 113
}

local CONTROLLER_TYPE_MAP = {
    [CONTROLLER_TYPE_ID_E.LSI_3108_WITH_MR] = 'LSI SAS3108MR',
    [CONTROLLER_TYPE_ID_E.LSI_3108_WITH_IR] = 'LSI SAS3108IR',
    [CONTROLLER_TYPE_ID_E.LSI_3108_WITH_IT] = 'LSI SAS3108IT',
    [CONTROLLER_TYPE_ID_E.LSI_3008_WITH_MR] = 'LSI SAS3008MR',
    [CONTROLLER_TYPE_ID_E.LSI_3008_WITH_IR] = 'LSI SAS3008IR',
    [CONTROLLER_TYPE_ID_E.LSI_3008_WITH_IT] = 'LSI SAS3008IT',
    [CONTROLLER_TYPE_ID_E.LSI_3508_WITH_MR] = 'Avago SAS3508MR',
    [CONTROLLER_TYPE_ID_E.LSI_3516_WITH_MR] = 'Avago SAS3516MR',
    [CONTROLLER_TYPE_ID_E.LSI_3408_WITH_IMR] = 'Avago SAS3408iMR',
    [CONTROLLER_TYPE_ID_E.LSI_3416_WITH_IMR] = 'Avago SAS3416iMR',
    [CONTROLLER_TYPE_ID_E.LSI_3408_WITH_IT] = 'Avago SAS3408IT',
    [CONTROLLER_TYPE_ID_E.LSI_3416_WITH_IT] = 'Avago SAS3416IT',
    [CONTROLLER_TYPE_ID_E.LSI_3004_WITH_IMR] = 'Avago SAS3004iMR',
    [CONTROLLER_TYPE_ID_E.LSI_3008_WITH_IT_PCIE] = 'LSI SAS3008IT(PCIe)',
    [CONTROLLER_TYPE_ID_E.LSI_3908_WITH_MR] = 'BROADCOM MegaRAID SAS 9560-8i',
    [CONTROLLER_TYPE_ID_E.LSI_3916_WITH_MR] = 'BROADCOM MegaRAID SAS 9560-16i',
    [CONTROLLER_TYPE_ID_E.LSI_3808_WITH_MR] = 'BROADCOM MegaRAID SAS 9540-8i',
    [CONTROLLER_TYPE_ID_E.LSI_3816_WITH_IMR] = 'BROADCOM SAS3816iMR',
    [CONTROLLER_TYPE_ID_E.LSI_3808_WITH_IMR] = 'BROADCOM SAS3808iMR',
    [CONTROLLER_TYPE_ID_E.PMC_3152_8I_SMART_RAID] = 'MSCC SmartRAID 3152-8i',
    [CONTROLLER_TYPE_ID_E.PMC_2100_8I_SMART_HBA] = 'MSCC SmartHBA 2100-8i',
    [CONTROLLER_TYPE_ID_E.HI1880_SP186_M_16i] = 'SP186-M-16i',
    [CONTROLLER_TYPE_ID_E.HI1880_SP186_M_32i] = 'SP186-M-32i',
    [CONTROLLER_TYPE_ID_E.HI1880_SP186_M_40i] = 'SP186-M-40i',
    [CONTROLLER_TYPE_ID_E.HI1880_SP186_M_8i] = 'SP186-M-8i',
    [CONTROLLER_TYPE_ID_E.HI1880_SP686C_M_16i_2G] = 'SP686C-M-16i 2G',
    [CONTROLLER_TYPE_ID_E.HI1880_SP686C_M_16i_4G] = 'SP686C-M-16i 4G',
    [CONTROLLER_TYPE_ID_E.HI1880_SP686C_MH_32i_4G] = 'SP686C-MH-32i 4G',
    [CONTROLLER_TYPE_ID_E.HI1880_SP686C_M_40i_2G] = 'SP686C-M-40i 2G',
    [CONTROLLER_TYPE_ID_E.HI1880_SP686C_M_40i_4G] = 'SP686C-M-40i 4G',
    [CONTROLLER_TYPE_ID_E.HI1880_SPR110] = 'SPR110',
    [CONTROLLER_TYPE_ID_E.HI1880_SPR120] = 'SPR120',
    [CONTROLLER_TYPE_ID_E.HI1880_SPR130] = 'SPR130',
    [CONTROLLER_TYPE_ID_E.HI1880_SPR140] = 'SPR140',
    [CONTROLLER_TYPE_ID_E.HI1880_SPR180] = 'SPR180',
    [CONTROLLER_TYPE_ID_E.HI1880_SPR185] = 'SPR185',
    [CONTROLLER_TYPE_ID_E.HI1880_SPR190] = 'SPR190',
    [CONTROLLER_TYPE_ID_E.HI1880_SPR1A0] = 'SPR1A0',
    [CONTROLLER_TYPE_ID_E.HI1880_SPR1A5] = 'SPR1A5'
}

local MAX_SLOT_NUMBER = 255 --  SlotNumber可能为255，防止出错
local SPAN_DEPTH = {
    SPAN_DEPTH_MINI = 2,
    SPAN_DEPTH_INVALID = 255
}

local SML_LOGIC_DRIVE_RAID_LEVEL_E = {
    RAID_LEVEL_0 = 0,
    RAID_LEVEL_1 = 1,
    RAID_LEVEL_2 = 2,
    RAID_LEVEL_3 = 3,
    RAID_LEVEL_4 = 4,
    RAID_LEVEL_5 = 5,
    RAID_LEVEL_6 = 6,
    RAID_LEVEL_10 = 10,
    RAID_LEVEL_1ADM = 11,
    RAID_LEVEL_10ADM = 12,
    RAID_LEVEL_1TRIPLE = 13,
    RAID_LEVEL_10TRIPLE = 14,
    RAID_LEVEL_1E = 17,
    RAID_LEVEL_20 = 20,
    RAID_LEVEL_30 = 30,
    RAID_LEVEL_40 = 40,
    RAID_LEVEL_50 = 50,
    RAID_LEVEL_60 = 60,
    RAID_LEVEL_DELETED = 254,
    RAID_LEVEL_UNKNOWN = 255
}

local SML_LD_STATE_E = {
    LD_STATE_OFFLINE = 0,
    LD_STATE_PARTIALLY_DEGRADED = 1,
    LD_STATE_DEGRADED = 2,
    LD_STATE_OPTIMAL = 3,
    LD_STATE_FAILED = 4,
    LD_STATE_NOT_CONFIGURED = 5,
    LD_STATE_INTERIM_RECOVERY = 6,
    LD_STATE_READY_FOR_RECOVERY = 7,
    LD_STATE_RECOVERYING = 8,
    LD_STATE_WRONG_DRIVE_REPLACED = 9,
    LD_STATE_DRVIE_IMPROPERLY_CONNECTED = 10,
    LD_STATE_EXPANDING = 11,
    LD_STATE_NOT_YET_AVAILABLE = 12,
    LD_STATE_QUEUED_FOR_EXPANSION = 13,
    LD_STATE_DISABLED_FROM_SCSIID_CONFLICT = 14,
    LD_STATE_EJECTED = 15,
    LD_STATE_ERASE_IN_PROGRESS = 16,
    LD_STATE_UNUSED = 17,
    LD_STATE_READY_TO_PERFORM_PREDICTIVE_SPARE_REBUILD = 18,
    LD_STATE_RPI_IN_PROGRESS = 19,
    LD_STATE_RPI_QUEUED = 20,
    LD_STATE_ENCRYPTED_VOLUME_WITHOUT_KEY = 21,
    LD_STATE_ENCRYPTION_MIGRATION = 22,
    LD_STATE_ENCRYPTED_VOLUME_REKEYING = 23,
    LD_STATE_ENCRYPTED_VOLUME_ENCRYPTION_OFF = 24,
    LD_STATE_VOLUME_ENCODE_REQUESTED = 25,
    LD_STATE_ENCRYPTED_VOLUME_REKEY_REQUESTED = 26,
    LD_STATE_UNSUPPORTED_ON_THIS_CONTROLLER = 27,
    LD_STATE_NOT_FORMATTED = 28,
    LD_STATE_FORMATTING = 29,
    LD_STATE_SANITIZING = 30,
    LD_STATE_INITIALIZING = 31,
    LD_STATE_INITIALIZEFAIL = 32,
    LD_STATE_DELETING = 33,
    LD_STATE_DELETEFAIL = 34,
    LD_STATE_WRITE_PROTECT = 35, 
}

local RFPROP_VOLUME_STATE = {
    RFPROP_VOLUME_STATE_OFFLINE = 'Offline',
    RFPROP_VOLUME_STATE_PARTTIALLY_DEGRADED = 'Partially degraded',
    RFPROP_VOLUME_STATE_DEGRADED = 'Degraded',
    RFPROP_VOLUME_STATE_OPTIMAL = 'Optimal',
    RFPROP_VOLUME_STATE_FAILED = "Failed",
    RFPROP_VOLUME_STATE_NOT_CONFIGURED = "Not Configured",
    RFPROP_VOLUME_STATE_INTERIM_RECOVERY = "Interim recovery",
    RFPROP_VOLUME_STATE_READY_FOR_RECOVERY = "Ready for recovery",
    RFPROP_VOLUME_STATE_RECOVERYING = "Recovering",
    RFPROP_VOLUME_STATE_WRONG_DRIVE_REPLACED = "Wrong drive replaced",
    RFPROP_VOLUME_STATE_DRVIE_IMPROPERLY_CONNECTED = "Drive improperly connected",
    RFPROP_VOLUME_STATE_EXPANDING = "Expanding",
    RFPROP_VOLUME_STATE_NOT_YET_AVAILABLE = "Not available",
    RFPROP_VOLUME_STATE_QUEUED_FOR_EXPANSION = "Queued for expansion",
    RFPROP_VOLUME_STATE_DISABLED_FROM_SCSIID_CONFLICT = "Disabled from SCSI conflict",
    RFPROP_VOLUME_STATE_EJECTED = "Ejected",
    RFPROP_VOLUME_STATE_ERASE_IN_PROGRESS = "Erase in progress",
    RFPROP_VOLUME_STATE_UNUSED = "Unused",
    RFPROP_VOLUME_STATE_READY_TO_PERFORM_PREDICTIVE_SPARE_REBUILD = "Ready to perform predictive spare rebuild",
    RFPROP_VOLUME_STATE_RPI_IN_PROGRESS = "RPI in progress",
    RFPROP_VOLUME_STATE_RPI_QUEUED = "RPI queued",
    RFPROP_VOLUME_STATE_ENCRYPTED_VOLUME_WITHOUT_KEY = "Encrypted without key",
    RFPROP_VOLUME_STATE_ENCRYPTION_MIGRATION = "Encryption migration",
    RFPROP_VOLUME_STATE_ENCRYPTED_VOLUME_REKEYING = "Encrypted & re-keying",
    RFPROP_VOLUME_STATE_ENCRYPTED_VOLUME_ENCRYPTION_OFF = "Encrypted with encryption off",
    RFPROP_VOLUME_STATE_VOLUME_ENCODE_REQUESTED = "Volume encode requested",
    RFPROP_VOLUME_STATE_ENCRYPTED_VOLUME_REKEY_REQUESTED = "Encrypted with re-key request",
    RFPROP_VOLUME_STATE_UNSUPPORTED_ON_THIS_CONTROLLER = "Unsupported",
    RFPROP_VOLUME_STATE_NOT_FORMATTED = "Not Formatted",
    RFPROP_VOLUME_STATE_FORMATTING = "Formatting",
    RFPROP_VOLUME_STATE_SANITIZING = "Sanitizing",
    RFPROP_VOLUME_STATE_INITIALIZING = "Initializing",
    RFPROP_VOLUME_STATE_INITIALIZEFAIL = "Initialize fail",
    RFPROP_VOLUME_STATE_DELETING = "Deleting",
    RFPROP_VOLUME_STATE_DELETEFAIL = "Delete fail",
    RFPROP_VOLUME_STATE_WRITE_PROTECT = "Write protect",
}

local SML_LD_READ_POLICY_E = {LD_CACHE_NO_READ_AHEAD = 0, LD_CACHE_READ_AHEAD = 1}

local SML_LD_WRITE_POLICY_E = {
    LD_CACHE_WRITE_THROUGH = 0,
    LD_CACHE_WRITE_BACK = 1,
    LD_CACHE_WRITE_CACHE_IF_BAD_BBU = 2
}

local SML_LD_CACHE_POLICY_E = {LD_CACHE_CACHED_IO = 0, LD_CACHE_DIRECT_IO = 1}

local SML_LD_ACCESS_POLICY_E = {
    LD_ACCESS_RW = 0,
    LD_ACCESS_READ_ONLY = 1,
    LD_ACCESS_BLOCKED = 2,
    LD_ACCESS_HIDDEN = 3
}

local SML_BOOT_PRIORITY_E = {
    BOOT_PRIORITY_NONE = 0,
    BOOT_PRIORITY_PRIMARY = 1,
    BOOT_PRIORITY_SECONDARY = 2,
    BOOT_PRIORITY_ALL = 3,
    BOOT_PRIORITY_UNKNOWN = 0xFF
}

local SML_BOOT_PRIORITY_STR_E = {
    BOOT_PRIORITY_NONE = 'None',
    BOOT_PRIORITY_PRIMARY = 'Primary',
    BOOT_PRIORITY_SECONDARY = 'Secondary',
    BOOT_PRIORITY_ALL = 'All'
}

local SML_DISK_CACHE_POLICY_E = {PD_CACHE_UNCHANGED = 0, PD_CACHE_ENABLE = 1, PD_CACHE_DISABLE = 2}

local LD_INIT_STATE_E = {
    LD_INIT_STATE_NO_INIT = 0, 
    LD_INIT_STATE_QUICK_INIT = 1, 
    LD_INIT_STATE_FULL_INIT = 2,
    LD_INIT_STATE_RPI = 3,
    LD_INIT_STATE_OPO = 4,
    LD_INIT_STATE_FRONT = 5,
    LD_INIT_STATE_BACKGROUND = 6
}

local SML_LD_ACCELERATION_METHOD_E = {
    LD_ACCELERATOR_UNKNOWN = 0,
    LD_ACCELERATOR_NONE = 1,
    LD_ACCELERATOR_CACHE = 2,
    LD_ACCELERATOR_IOBYPASS = 3,
    LD_ACCELERATOR_MAXCACHE = 4
}

local SML_LD_ACCELERATION_METHOD_STR_E = {
    LD_ACCELERATOR_UNKNOWN =  'Unknown',
    LD_ACCELERATOR_NONE = 'None',
    LD_ACCELERATOR_CACHE = 'ControllerCache',
    LD_ACCELERATOR_IOBYPASS  = 'IOBypass',
    LD_ACCELERATOR_MAXCACHE = 'maxCache'
}

local ldAccelerationMethodMap = {
    [SML_LD_ACCELERATION_METHOD_E.LD_ACCELERATOR_UNKNOWN] = SML_LD_ACCELERATION_METHOD_STR_E.LD_ACCELERATOR_UNKNOWN,
    [SML_LD_ACCELERATION_METHOD_E.LD_ACCELERATOR_NONE] = SML_LD_ACCELERATION_METHOD_STR_E.LD_ACCELERATOR_NONE,
    [SML_LD_ACCELERATION_METHOD_E.LD_ACCELERATOR_CACHE] = SML_LD_ACCELERATION_METHOD_STR_E.LD_ACCELERATOR_CACHE,
    [SML_LD_ACCELERATION_METHOD_E.LD_ACCELERATOR_IOBYPASS] = SML_LD_ACCELERATION_METHOD_STR_E.LD_ACCELERATOR_IOBYPASS,
    [SML_LD_ACCELERATION_METHOD_E.LD_ACCELERATOR_MAXCACHE] = SML_LD_ACCELERATION_METHOD_STR_E.LD_ACCELERATOR_MAXCACHE,
}

local SML_LOGENTRY_STATUS_E = {
    LOGENTRY_STATUS_INFORMATIONAL_CODE = 0,
    LOGENTRY_STATUS_MINOR_CODE = 1,
    LOGENTRY_STATUS_MAJOR_CODE = 2,
    LOGENTRY_STATUS_CRITICAL_CODE = 3
}

local RF_DEVIES_MEDIATYPE = {
    RF_DEVIES_MEDIATYPE_HDD_VALUE = 0,
    RF_DEVIES_MEDIATYPE_SSD_VALUE = 1,
    RF_DEVIES_MEDIATYPE_SSM_VALUE = 2
}

local SML_PD_STATE_E = {
    PD_STATE_UNCONFIGURED_GOOD = 0, -- Unconfigured good drive
    PD_STATE_UNCONFIGURED_BAD = 1, -- Unconfigured bad drive
    PD_STATE_HOT_SPARE = 2, -- Hot spare drive
    PD_STATE_OFFLINE = 3, -- Configured - good drive (data invalid)
    PD_STATE_FAILED = 4, -- Configured - bad drive (data invalid)
    PD_STATE_REBUILD = 5, -- Configured - drive is rebuilding
    PD_STATE_ONLINE = 6, -- Configured - drive is online
    PD_STATE_COPYBACK = 7, -- drive is getting copied
    PD_STATE_SYSTEM = 8, -- drive is exposed and controlled by host
    PD_STATE_SHIELD_UNCONFIGURED = 9, -- UnConfigured - shielded
    PD_STATE_SHIELD_HOT_SPARE = 10, -- Hot Spare - shielded
    PD_STATE_SHIELD_CONFIGURED = 11, -- Configured - shielded
    PD_STATE_FOREIGN = 12, -- With foreign configuration
    PD_STATE_ACTIVE = 13,
    PD_STATE_STANDBY = 14,
    PD_STATE_SLEEP = 15,
    PD_STATE_DST = 16,
    PD_STATE_SMART = 17,
    PD_STATE_SCT = 18,
    PD_STATE_EPD = 19,
    PD_STATE_RAW = 20,
    PD_STATE_READY = 21,
    PD_STATE_NOT_SUPPORTED = 22,
    PD_STATE_PREDICTIVE_FAILURE = 23,
    PD_STATE_DIAGNOSING = 24,
    PD_STATE_INCOMPATIBLE = 25,
    PD_STATE_ERASING = 26,
    PD_STATE_UNKNOWN = 255 -- Unknown state
}

local RF_PHYSICALDISKSTATE_E = {
    RF_PHYSICALDISKSTATE_UNCONFIGUREDGOOD = 'UnconfiguredGood',
    RF_PHYSICALDISKSTATE_UNCONFIGUREDBAD = 'UnconfiguredBad',
    RF_PHYSICALDISKSTATE_HOTSPARE = 'HotSpareDrive',
    RF_PHYSICALDISKSTATE_CONFIGUREDGOOD = 'Offline',
    RF_PHYSICALDISKSTATE_CONFIGUREDBAD = 'Failed',
    RF_PHYSICALDISKSTATE_FOREIGN = 'Foreign',
    RF_PHYSICALDISKSTATE_ACTIVE = 'Active',
    RF_PHYSICALDISKSTATE_STANDBY = 'Standby',
    RF_PHYSICALDISKSTATE_SLEEP = 'Sleep',
    RF_PHYSICALDISKSTATE_DST = 'DSTInProgress',
    RF_PHYSICALDISKSTATE_SMART = 'SMARTOfflineDataCollection',
    RF_PHYSICALDISKSTATE_SCT = 'SCTCommand',
    RF_PHYSICALDISKSTATE_REBUILDING = 'Rebuilding',
    RF_PHYSICALDISKSTATE_ONLINE = 'Online',
    RF_PHYSICALDISKSTATE_GETTINGCOPIED = 'GettingCopied',
    RF_PHYSICALDISKSTATE_CONTROLLEDBYHOST = 'JBOD',
    RF_PHYSICALDISKSTATE_UNCONFIGUREDSHIELDED = 'UnconfiguredShielded',
    RF_PHYSICALDISKSTATE_HOTSPARESHIELDED = 'HotSpareShielded',
    RF_PHYSICALDISKSTATE_CONFIGUREDSHIELDED = 'ConfiguredShielded', 
    RF_PHYSICALDISKSTATE_RAW = "Raw",
    RF_PHYSICALDISKSTATE_READY = "Ready",
    RF_PHYSICALDISKSTATE_NOTSUPPORTED = "NotSupported",
    RF_PHYSICALDISKSTATE_PREDICTIVE_FAILURE = "PredictiveFailure",
    RF_PHYSICALDISKSTATE_DIAGNOSING = "Diagnosing",
    RF_PHYSICALDISKSTATE_INCOMPATIBLE = "Incompatible",
    RF_PHYSICALDISKSTATE_ERASING = "EraseInProgress"
}

local RFPROP_DRIVE_POWER_STATE = {
    RFPROP_DRIVE_POWER_STATE_V_SPUNUP_NUM = 0,
    RFPROP_DRIVE_POWER_STATE_V_SPUNDOWN_NUM = 1,
    RFPROP_DRIVE_POWER_STATE_V_TRANSITION_NUM = 2
}

local RFPROP_DRIVE_POWER_STATE_V_SPUNUP = 'Spun Up'
local RFPROP_DRIVE_POWER_STATE_V_SPUNDOWN = 'Spun Down'
local RFPROP_DRIVE_POWER_STATE_V_TRANSITION = 'Transition'

local SML_PHYSICAL_DRIVE_HOT_SPARE_E = {
    PD_HOT_SPARE_NONE = 0,
    PD_HOT_SPARE_GLOBAL = 1,
    PD_HOT_SPARE_DEDICATED = 2,
    PD_HOT_SPARE_AUTO_REPLACE = 3,
    PD_HOT_SPARE_UNKNOWN = 255
}

local RF_HOTSPARETYPE = {
    RF_HOTSPARETYPE_NONE = 'None',
    RF_HOTSPARETYPE_GLOBAL = 'Global',
    RF_HOTSPARETYPE_DEDICATED = 'Dedicated',
    RF_HOTSPARETYPE_AUTO_REPLACE = 'Auto Replace'
}

local SML_PD_PATROL_STATE = {
    SML_PD_PATROL_STATE_OFF = 'DoneOrNotPatrolled',
    SML_PD_PATROL_STATE_ON = 'Patrolling'
}
-- PCIe资源ID归属定义
local RESOURCE_ID_PCIE_DEVICE_BASE = 0x80 -- PCIe资源归属于其它PCIe设备时, 资源ID的基数, 实际的资源ID=基数 + PCIe设备FRU ID
local RESOURCE_ID_PCIE_DEVICE_MAX = 0xC0 -- PCIe资源归属于其它PCIe设备时, 资源ID的最大值, 实际的资源ID=基数 + PCIe设备FRU ID
local RESOURCE_ID_CPU1_AND_CPU2 = 0xFD -- PCIe资源归属于CPU1和CPU2
local RESOURCE_ID_PCH = 0xFE -- PCIe资源归属于PCH
local RESOURCE_ID_PCIE_SWTICH = 0xFF -- PCIe资源归属于PCIe Switch, 可动态切换, 不固定

local DRIVE_INDICATORLED_OFF = 0
local DRIVE_INDICATORLED_BLINKING = 1

local DRIVE_FAULT_OK = 0
local DRIVE_FAULT_YES = 1

local RF_INDICATORLED_OFF = 'Off'
local RF_INDICATORLED_BLINKING = 'Blinking'

-- 表示创建逻辑盘的类型
local CREATE_LD_TYPE_E = {CREATE_LD_ON_NEW_ARRAY = 0, ADD_LD_ON_EXIST_ARRAY = 1, CREATE_LD_AS_CACHECADE = 2}

local DRIVE_SMART_INVALID_U16_VALUE = 0xFFFF
local DRIVE_SMART_INVALID_U8_VALUE = 0xFF
local DRIVE_REBUILTING_VALUE = 1

local RFERR_INSUFFICIENT_PRIVILEGE = 403
local MFI_STAT_WRONG_STATE = 0x32

local SM_CODE_E = {
    SM_CODE_INVALID_RAID_LEVEL = 0x2080,
    SM_CODE_INVALID_SPAN_DEPTH = 0x2081,
    SM_CODE_INVALID_PD_COUNT = 0x2082,
    SM_CODE_INVALID_PD_ID = 0x2085,
    SM_CODE_CAPACITY_OUT_OF_RANGE = 0x2086,
    SM_CODE_STRIP_SIZE_OUT_OF_RANGE = 0x2087,
    SM_CODE_READ_POLCIY_OUT_OF_RANGE = 0x2088,
    SM_CODE_WRITE_POLICY_OUT_OF_RANGE = 0x2089,
    SM_CODE_IO_POLICY_OUT_OF_RANGE = 0x208A,
    SM_CODE_LD_NAME_INVALID_ASCII = 0x208E,
    SM_CODE_LD_NAME_EXCEED_MAX_LEN = 0x208F,
    SM_CODE_UPGRADE_IN_PROGRESS = 0x2090,
    SM_CODE_NO_SPACE = 0x2091,
    SM_CODE_OPERATION_IN_PROGRESS = 0x2092,
    SM_CODE_PD_NOT_IDENTIFIED = 0x20A1, -- 物理盘没有在控制器下
    SM_CODE_BUSY = 0x20C0,
    SM_CODE_PARA_DATA_ILLEGAL = 0x20CC,
    SM_CODE_NOT_SUPPORT_IN_PRESENT_STATE = 0x20D5 -- 控制器没初始化好
}

local SML_ERR_CODE_E = {
    SML_ERR_I2C_READ_WRITE_FAILED = 0x1001,
    SML_ERR_REBOOT_REQUIRED = 0x1074, -- 操作已完成 需要重启
    SML_ERR_CONFIG_PRESENT_ERROR = 0x1098,

    SML_ERR_CTRL_INIT_NOT_COMPLETED = 0x1104, -- 控制器初始化未完成
    SML_ERR_CTRL_SET_PROP_UPLEVEL_FUNC_DISABLED = 0x1110,
    SML_ERR_CTRL_STATUS_INVALID = 0x11D5, -- 控制器状态无效
    SML_ERR_CTRL_OPERATION_NOT_SUPPORT = 0x11D7, -- 控制器不支持操作
    SML_ERR_CTRL_BBU_STATUS_ABNORMAL = 0x11DA,
    SML_ERR_CTRL_NO_EDITABLE_LD = 0x11DB,
    SML_ERR_CTRL_RCP_NOT_IN_RANGE = 0x11DC,

    SML_ERR_LD_INIT_IN_PROGRESS = 0x1218,
    SML_ERR_LD_PROPERTY_SET_NOT_ALLOWED = 0x1220,
    SML_ERR_LD_PROPERTY_SET_ONLY_DEFAULT_ALLOWED = 0x1221,
    SML_ERR_LD_NO_SSCD_OR_INVALID_NUM_OF_SSCD = 0x1222,
    SML_ERR_LD_SET_CACHING_ENABLE_FOR_LD_WITH_SSD = 0x1223,
    SML_ERR_LD_SIZE_SHRINK_NOT_ALLOWED = 0x1224,
    SML_ERR_LD_STATE_UNSUPPORTED_TO_SET = 0x1225,
    SML_ERR_LD_OPERATION_NOT_SUPPORT = 0x12D5,

    SML_ERR_PD_MAKESPARE_NOT_ALLOWED = 0x1310, -- 物理盘不允许设置热备
    SML_ERR_PD_SPARE_SDD_HDD_MIXED = 0x1312,
    SML_ERR_PD_SPARE_FOR_RAID0_LD = 0x1314, -- 试图将物理盘设置为RAID0逻辑盘的热备盘
    SML_ERR_PD_OPERATION_NOT_SUPPORT = 0x13D5, -- 物理盘不支持

    SML_ERR_CONFIG_INVALID_PARAM_RAID_LEVEL = 0x1413, -- 创建逻辑盘的RAID级别参数无效
    SML_ERR_CONFIG_INVALID_PARAM_CAPACITY_TOO_SMALL = 0x1415, -- 创建逻辑盘指定的容量空间太小
    SML_ERR_CONFIG_INVALID_PARAM_CAPACITY_TOO_LARGE = 0x1416, -- 创建逻辑盘指定的容量空间太大
    SML_ERR_CONFIG_ARRAY_NO_AVAILABLE_SPACE = 0x1432, -- 指定用于创建逻辑盘的Array上没有可用空间
    SML_ERR_CONFIG_INVALID_PD_NON_SUPPORTED = 0x1442, -- 用于创建逻辑盘的物理盘ID指向的是不支持的物理设备
    SML_ERR_CONFIG_INVALID_PD_IN_USE = 0x1444, -- 用于创建逻辑盘的物理盘已经被使用
    SML_ERR_CONFIG_INVALID_PD_NON_SDD_FOR_CACHECADE = 0x1448, -- 用于创建CacheCade逻辑盘的物理盘不是SSD硬盘
    SML_ERR_CONFIG_INVALID_PD_OTHER_ERROR = 0x144F, -- 用于创建逻辑盘的物理盘有其他不适于创建的错误
    SML_ERR_CONFIG_OPERATION_NOT_SUPPORT = 0x14D5,
    SML_ERR_CONFIG_NO_FOREIGN_CONFIG = 0x14D6,
    SML_ERR_CONFIG_INCOMPLETE_FOREIGN_CONFIG = 0x14D7,
    SML_ERR_CONFIG_ASSOCIATED_LD_SIZE_OUT_OF_RANGE = 0x14D8
}

local errLDMap = {
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request('PropertyModificationNeedPrivilege'),
    [SML_ERR_CODE_E.SML_ERR_CTRL_STATUS_INVALID] = reply_bad_request('CurrentStatusNotSupportOperation'),
    [SML_ERR_CODE_E.SML_ERR_CTRL_INIT_NOT_COMPLETED] = reply_bad_request('CurrentStatusNotSupportOperation'),
    [SML_ERR_CODE_E.SML_ERR_I2C_READ_WRITE_FAILED] = reply_bad_request('CurrentStatusNotSupportOperation'),
    [SML_ERR_CODE_E.SML_ERR_CTRL_SET_PROP_UPLEVEL_FUNC_DISABLED] = reply_bad_request('CurrentStatusNotSupportOperation'),
    [SML_ERR_CODE_E.SML_ERR_CTRL_OPERATION_NOT_SUPPORT] = reply_bad_request('RAIDControllerNotSupported'),
    [SML_ERR_CODE_E.SML_ERR_LD_INIT_IN_PROGRESS] = reply_bad_request('LDInitInProgress'),
    [SM_CODE_E.SM_CODE_LD_NAME_EXCEED_MAX_LEN] = reply_bad_request('InvalidVolumeName'),
    [SML_ERR_CODE_E.SML_ERR_CTRL_BBU_STATUS_ABNORMAL] = reply_bad_request('PropertyModificationNotSupportedByBBUStatus'),
    [SML_ERR_CODE_E.SML_ERR_CTRL_NO_EDITABLE_LD] = reply_bad_request('CurrentStatusNotSupportOperation'),
    [SML_ERR_CODE_E.SML_ERR_LD_SIZE_SHRINK_NOT_ALLOWED] = reply_bad_request('VolumeShrinkNotAllowed'),
    [SML_ERR_CODE_E.SML_ERR_LD_STATE_UNSUPPORTED_TO_SET] = reply_bad_request('OperationFailed'),
    [SML_ERR_CODE_E.SML_ERR_CONFIG_INVALID_PARAM_RAID_LEVEL] = reply_bad_request('RaidControllerLevelInvalid'),
    [SML_ERR_CODE_E.SML_ERR_CONFIG_INVALID_PD_NON_SDD_FOR_CACHECADE] = reply_bad_request('NonSSDExist'),
    [SML_ERR_CODE_E.SML_ERR_CONFIG_ASSOCIATED_LD_SIZE_OUT_OF_RANGE] = reply_bad_request('AssociatedVolumeCapacityOutRange'),
    [SM_CODE_E.SM_CODE_INVALID_SPAN_DEPTH] = reply_bad_request('NumberOfSpansInvalid'),
    [SML_ERR_CODE_E.SML_ERR_CONFIG_INVALID_PARAM_CAPACITY_TOO_SMALL] = reply_bad_request('VolumeCapacityOutRange'),
    [SML_ERR_CODE_E.SML_ERR_CONFIG_INVALID_PARAM_CAPACITY_TOO_LARGE] = reply_bad_request('VolumeCapacityOutRange'),
    [SML_ERR_CODE_E.SML_ERR_CONFIG_OPERATION_NOT_SUPPORT] = reply_bad_request('OperationNotSupported'),
    [SM_CODE_E.SM_CODE_NOT_SUPPORT_IN_PRESENT_STATE] = reply_bad_request('CurrentStatusNotSupport'),
    [SML_ERR_CODE_E.SML_ERR_CONFIG_INVALID_PD_IN_USE] = reply_bad_request('DriveStatusNotSupported'),
    [SML_ERR_CODE_E.SML_ERR_CONFIG_INVALID_PD_OTHER_ERROR] = reply_bad_request('DriveStatusNotSupported'),
    [SM_CODE_E.SM_CODE_OPERATION_IN_PROGRESS] = reply_bad_request('OperationInProcess'),
    [SML_ERR_CODE_E.SML_ERR_CONFIG_INVALID_PD_NON_SUPPORTED] = reply_bad_request('DriveStatusNotSupported'),
    [SM_CODE_E.SM_CODE_INVALID_PD_ID] = reply_bad_request('PhysicalDiskIdInvalid'),
    [SM_CODE_E.SM_CODE_INVALID_PD_COUNT] = reply_bad_request('PhysicalDiskIdCountError'),
    [SM_CODE_E.SM_CODE_INVALID_RAID_LEVEL] = reply_bad_request('RaidControllerLevelInvalid'),
    [SM_CODE_E.SM_CODE_CAPACITY_OUT_OF_RANGE] = reply_bad_request('VolumeCapacityOutRange'),
    [SM_CODE_E.SM_CODE_STRIP_SIZE_OUT_OF_RANGE] = reply_bad_request('StripeSizeError'),
    [SM_CODE_E.SM_CODE_READ_POLCIY_OUT_OF_RANGE] = reply_bad_request('InvalidValue'),
    [SM_CODE_E.SM_CODE_WRITE_POLICY_OUT_OF_RANGE] = reply_bad_request('InvalidValue'),
    [SM_CODE_E.SM_CODE_IO_POLICY_OUT_OF_RANGE] = reply_bad_request('InvalidValue'),
    [SML_ERR_CODE_E.SML_ERR_CONFIG_ARRAY_NO_AVAILABLE_SPACE] = reply_bad_request('ArraySpaceNoFree')
}

local lDAccMap = {
    [SML_ERR_CODE_E.SML_ERR_REBOOT_REQUIRED] = reply_bad_request('VolumeSetSuccessfully'),
    [SM_CODE_E.SM_CODE_OPERATION_IN_PROGRESS] = reply_bad_request('OperationInProcess'),
}

local errCtlMap = {
    [SM_CODE_E.SM_CODE_NOT_SUPPORT_IN_PRESENT_STATE] = reply_bad_request('ModifyfailedWithRAIDCardState'),
    [SML_ERR_CODE_E.SML_ERR_CTRL_STATUS_INVALID] = reply_bad_request('ModifyfailedWithRAIDCardState'),
    [SML_ERR_CODE_E.SML_ERR_LD_PROPERTY_SET_NOT_ALLOWED] = reply_bad_request('ModifyfailedWithCachecade'),
    [SML_ERR_CODE_E.SML_ERR_LD_OPERATION_NOT_SUPPORT] = reply_bad_request('ModifyfailedWithRaidControllerNotSupport'),
    [SM_CODE_E.SM_CODE_PARA_DATA_ILLEGAL] = reply_bad_request('PropertyValueNotInList'),
    [SML_ERR_CODE_E.SML_ERR_LD_NO_SSCD_OR_INVALID_NUM_OF_SSCD] = reply_bad_request('NoCachecadeVolume'),
    [SML_ERR_CODE_E.SML_ERR_LD_PROPERTY_SET_ONLY_DEFAULT_ALLOWED] = reply_bad_request('ModifyfailedWithSSD'),
    [SML_ERR_CODE_E.SML_ERR_LD_SET_CACHING_ENABLE_FOR_LD_WITH_SSD] = reply_bad_request('ModifyfailedWithSSD'),
    [SM_CODE_E.SM_CODE_LD_NAME_INVALID_ASCII] = reply_bad_request('InvalidVolumeName'),
    [SML_ERR_CODE_E.SML_ERR_REBOOT_REQUIRED] = reply_bad_request('RAIDControllerSetSuccessfully'),
    [SML_ERR_CODE_E.SML_ERR_CONFIG_PRESENT_ERROR] = reply_bad_request('ConfigurationExist'),
    [SML_ERR_CODE_E.SML_ERR_CTRL_BBU_STATUS_ABNORMAL] = reply_bad_request('PropertyModificationNotSupportedByBBUStatus'),
    [SML_ERR_CODE_E.SML_ERR_CTRL_NO_EDITABLE_LD] = reply_bad_request('CurrentStatusNotSupportOperation'),
    [SML_ERR_CODE_E.SML_ERR_CTRL_RCP_NOT_IN_RANGE] = reply_bad_request('ModifyfailedWithRCPNotInRange'),
    [SM_CODE_E.SM_CODE_UPGRADE_IN_PROGRESS] = reply_bad_request('UpgradingErr'),
    [SM_CODE_E.SM_CODE_NO_SPACE] = reply_bad_request('InsufficientMemoryErr'),
    [SM_CODE_E.SM_CODE_OPERATION_IN_PROGRESS] = reply_bad_request('OperationInProcess'),
}

local errDriveMap = {
    [SM_CODE_E.SM_CODE_NOT_SUPPORT_IN_PRESENT_STATE] = reply_bad_request('PropertyNotSupportedModificationByCurrentStatus'),
    [SM_CODE_E.SM_CODE_BUSY] = reply_bad_request('LocatedFailedByPDRebuilding'),
    [SML_ERR_CODE_E.SML_ERR_LD_PROPERTY_SET_NOT_ALLOWED] = reply_bad_request('ModifyfailedWithCachecade'),
    [SML_ERR_CODE_E.SML_ERR_LD_OPERATION_NOT_SUPPORT] = reply_bad_request('ModifyfailedWithRaidControllerNotSupport'),
    [SM_CODE_E.SM_CODE_PARA_DATA_ILLEGAL] = reply_bad_request('PropertyValueNotInList'),
    [SML_ERR_CODE_E.SML_ERR_LD_NO_SSCD_OR_INVALID_NUM_OF_SSCD] = reply_bad_request('NoCachecadeVolume'),
    [SML_ERR_CODE_E.SML_ERR_LD_PROPERTY_SET_ONLY_DEFAULT_ALLOWED] = reply_bad_request('ModifyfailedWithSSD'),
    [SML_ERR_CODE_E.SML_ERR_LD_SET_CACHING_ENABLE_FOR_LD_WITH_SSD] = reply_bad_request('ModifyfailedWithSSD'),
    [SM_CODE_E.SM_CODE_LD_NAME_INVALID_ASCII] = reply_bad_request('InvalidVolumeName'),
    [SML_ERR_CODE_E.SML_ERR_PD_MAKESPARE_NOT_ALLOWED] = reply_bad_request('PropertyModificationNotSupportedByPDStatus'),
    [SML_ERR_CODE_E.SML_ERR_PD_SPARE_FOR_RAID0_LD] = reply_bad_request('PropertyModificationFailedByRAIDlevelNotSupported'),
    [MFI_STAT_WRONG_STATE] = reply_bad_request('PropertyModificationNotSupportedByPDStatus'),
    [SML_ERR_CODE_E.SML_ERR_PD_SPARE_SDD_HDD_MIXED] = reply_bad_request('PropertyModificationFailedByMediaTypeConflict'),
    [SML_ERR_CODE_E.SML_ERR_CONFIG_ASSOCIATED_LD_SIZE_OUT_OF_RANGE] = reply_bad_request('AssociatedVolumeCapacityOutRange'),
    [SM_CODE_E.SM_CODE_OPERATION_IN_PROGRESS] = reply_bad_request('OperationInProcess'),
}

local restoreMap = {
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request('InsufficientPrivilege'),
    [SM_CODE_E.SM_CODE_NOT_SUPPORT_IN_PRESENT_STATE] = reply_bad_request('CurrentStatusNotSupportOperation'),
    [SML_ERR_CODE_E.SML_ERR_CTRL_STATUS_INVALID] = reply_bad_request('CurrentStatusNotSupportOperation'),
    [SML_ERR_CODE_E.SML_ERR_CTRL_INIT_NOT_COMPLETED] = reply_bad_request('CurrentStatusNotSupportOperation'),
    [SML_ERR_CODE_E.SML_ERR_I2C_READ_WRITE_FAILED] = reply_bad_request('CurrentStatusNotSupportOperation'),
    [SML_ERR_CODE_E.SML_ERR_CONFIG_NO_FOREIGN_CONFIG] = reply_bad_request('NoForeignConfig'),
    [SML_ERR_CODE_E.SML_ERR_CONFIG_INCOMPLETE_FOREIGN_CONFIG] = reply_bad_request('IncompleteForeignConfig'),
    [SML_ERR_CODE_E.SML_ERR_REBOOT_REQUIRED] = reply_bad_request('RAIDControllerSetSuccessfully'),
    [SM_CODE_E.SM_CODE_OPERATION_IN_PROGRESS] = reply_bad_request('OperationInProcess'),
}

local cryptoEraseeMap = {
    [RFERR_INSUFFICIENT_PRIVILEGE] = reply_bad_request('PropertyModificationNeedPrivilege'),
    [SM_CODE_E.SM_CODE_PD_NOT_IDENTIFIED] = reply_bad_request('OperationFailed'),
    [SM_CODE_E.SM_CODE_NOT_SUPPORT_IN_PRESENT_STATE] = reply_bad_request('OperationFailed'),
    [SML_ERR_CODE_E.SML_ERR_CTRL_STATUS_INVALID] = reply_bad_request('OperationFailed'),
    [SML_ERR_CODE_E.SML_ERR_CTRL_INIT_NOT_COMPLETED] = reply_bad_request('OperationFailed'),
    [SML_ERR_CODE_E.SML_ERR_PD_OPERATION_NOT_SUPPORT] = reply_bad_request('CryptoEraseNotSupported'),
    [SML_ERR_CODE_E.SML_ERR_CTRL_OPERATION_NOT_SUPPORT] = reply_bad_request('CryptoEraseNotSupported'),
    [SM_CODE_E.SM_CODE_OPERATION_IN_PROGRESS] = reply_bad_request('OperationInProcess'),
}

local function IntToBool(value)
    if value == 1 then
        return true
    elseif value == 0 then
        return false
    else
        return null
    end
end

local function BoolToInt(value)
    if value == false then
        return 0
    else
        return 1
    end
end

local function BootPriorityToBool(boot)
    if boot == SML_BOOT_PRIORITY_E.BOOT_PRIORITY_NONE then
        return false
    elseif boot == SML_BOOT_PRIORITY_E.BOOT_PRIORITY_PRIMARY then
        return true
    elseif boot == SML_BOOT_PRIORITY_E.BOOT_PRIORITY_SECONDARY then
        return true
    elseif boot == SML_BOOT_PRIORITY_E.BOOT_PRIORITY_ALL then
        return true
    else
        return null
    end
end

local function BootPriorityToStr(boot)
    if boot == SML_BOOT_PRIORITY_E.BOOT_PRIORITY_NONE then
        return SML_BOOT_PRIORITY_STR_E.BOOT_PRIORITY_NONE
    elseif boot == SML_BOOT_PRIORITY_E.BOOT_PRIORITY_PRIMARY then
        return SML_BOOT_PRIORITY_STR_E.BOOT_PRIORITY_PRIMARY
    elseif boot == SML_BOOT_PRIORITY_E.BOOT_PRIORITY_SECONDARY then
        return SML_BOOT_PRIORITY_STR_E.BOOT_PRIORITY_SECONDARY
    elseif boot == SML_BOOT_PRIORITY_E.BOOT_PRIORITY_ALL then
        return SML_BOOT_PRIORITY_STR_E.BOOT_PRIORITY_ALL
    else
        return null
    end
end

local function BootPriorityToNum(str)
    if str == SML_BOOT_PRIORITY_STR_E.BOOT_PRIORITY_NONE then
        return SML_BOOT_PRIORITY_E.BOOT_PRIORITY_NONE
    elseif str == SML_BOOT_PRIORITY_STR_E.BOOT_PRIORITY_PRIMARY then
        return SML_BOOT_PRIORITY_E.BOOT_PRIORITY_PRIMARY
    elseif str == SML_BOOT_PRIORITY_STR_E.BOOT_PRIORITY_SECONDARY then
        return SML_BOOT_PRIORITY_E.BOOT_PRIORITY_SECONDARY
    elseif str == SML_BOOT_PRIORITY_STR_E.BOOT_PRIORITY_ALL then
        return SML_BOOT_PRIORITY_E.BOOT_PRIORITY_ALL
    else
        return 0xff
    end
end

local function CheckStrEffective(str)
    if str == 'N/A' or str == 'UnKnown' or str == 'Undefined' or str == '' or str == nil then
        logging:error('check str effective fail.')
        return null
    else
        return str
    end
end

local function HealthStatusCodeToStr(status)
    if status >= STOTAGE_INFO_INVALID.STORAGE_INFO_INVALID_BYTE then
        return null
    elseif status == SML_LOGENTRY_STATUS_E.LOGENTRY_STATUS_INFORMATIONAL_CODE then
        return LOGENTRY_SEVERITY.LOGENTRY_SEVERITY_OK
    else
        return LOGENTRY_SEVERITY.LOGENTRY_SEVERITY_WARNING
    end
end

local function GetFaultDetails(status)
    local healthStatusStr = ''
    local healthStatusArray = {
        'Memory correctable errors', 'Memory uncorrectable errors', 'Memory ECC errors',
        'NVRAM uncorrectable errors', 'Raid communication loss', 'Card power abnormal'
    }
    if status == SML_LOGENTRY_STATUS_E.LOGENTRY_STATUS_INFORMATIONAL_CODE then
        return null
    end

    local firstFlag = true
    for i = 1, #healthStatusArray, 1 do
        if bit.band(status, bit.lshift(1, i - 1)) ~= 0 then
            if firstFlag then
                healthStatusStr = healthStatusStr .. healthStatusArray[i]
                firstFlag = false
            else
                healthStatusStr = healthStatusStr .. ',' .. healthStatusArray[i]
            end
        end
    end

    return healthStatusStr
end

local function SupportedRAIDTypesToStr(status)
    local strArray = {}
    local levelArray = {
        SML_RAID_LEVEL_E.SML_RAID_LEVEL_0, SML_RAID_LEVEL_E.SML_RAID_LEVEL_1, SML_RAID_LEVEL_E.SML_RAID_LEVEL_5,
        SML_RAID_LEVEL_E.SML_RAID_LEVEL_6, SML_RAID_LEVEL_E.SML_RAID_LEVEL_10, SML_RAID_LEVEL_E.SML_RAID_LEVEL_50,
        SML_RAID_LEVEL_E.SML_RAID_LEVEL_60, SML_RAID_LEVEL_E.SML_RAID_LEVEL_1ADM, SML_RAID_LEVEL_E.SML_RAID_LEVEL_10ADM,
        SML_RAID_LEVEL_E.SML_RAID_LEVEL_1TRIPLE, SML_RAID_LEVEL_E.SML_RAID_LEVEL_10TRIPLE
    }
    -- RAID级别存储在第二个字节中，移位检查从第九位开始，结束于第19位
    for i = 8, 19, 1 do
        if bit.band(bit.lshift(1, i), status) ~= 0 then
            strArray[#strArray + 1] = levelArray[i - 8 + 1]
        end
    end
    return strArray
end

local function SupportedModesToStr(status)
    local strArray = {}

    -- RAID模式在第25bit
    if bit.band(bit.lshift(1, 25), status) ~= 0 then
        strArray[#strArray + 1] = RAID_CTRL_MODE_NAME.CTRL_MODE_RAID_STR
    end

    -- JBOD模式在第26bit
    if bit.band(bit.lshift(1, 26), status) ~= 0 then
        strArray[#strArray + 1] = RAID_CTRL_MODE_NAME.CTRL_MODE_JBOD_STR
    end

    -- HBA模式在第27bit
    if bit.band(bit.lshift(1, 27), status) ~= 0 then
        strArray[#strArray + 1] = RAID_CTRL_MODE_NAME.CTRL_MODE_HBA_STR
    end

    -- Mixed模式在第28bit
    if bit.band(bit.lshift(1, 28), status) ~= 0 then
        strArray[#strArray + 1] = RAID_CTRL_MODE_NAME.CTRL_MODE_MIXED_STR
    end

    return strArray
end

local function RaidCtrlModeToStr(mode)
    if mode == CTRL_MODE_E.CTRL_MODE_RAID then
        return RAID_CTRL_MODE_NAME.CTRL_MODE_RAID_STR
    elseif mode == CTRL_MODE_E.CTRL_MODE_HBA then
        return RAID_CTRL_MODE_NAME.CTRL_MODE_HBA_STR
    elseif mode == CTRL_MODE_E.CTRL_MODE_JBOD then
        return RAID_CTRL_MODE_NAME.CTRL_MODE_JBOD_STR
    elseif mode == CTRL_MODE_E.CTRL_MODE_MIXED then
        return RAID_CTRL_MODE_NAME.CTRL_MODE_MIXED_STR
    end
end

local function RaidCtrlModeToNum(mode)
    if mode == RAID_CTRL_MODE_NAME.CTRL_MODE_RAID_STR then
        return CTRL_MODE_E.CTRL_MODE_RAID
    elseif mode == RAID_CTRL_MODE_NAME.CTRL_MODE_HBA_STR then
        return CTRL_MODE_E.CTRL_MODE_HBA
    elseif mode == RAID_CTRL_MODE_NAME.CTRL_MODE_JBOD_STR then
        return CTRL_MODE_E.CTRL_MODE_JBOD
    elseif mode == RAID_CTRL_MODE_NAME.CTRL_MODE_MIXED_STR then
        return CTRL_MODE_E.CTRL_MODE_MIXED
    else
        return CTRL_MODE_E.CTRL_MODE_UNKNOWN
    end
end

local function StrToArrayBySpace(str)
    if str == null then
        return null
    end
    local array = utils.split(str, ' ')
    return array[1]
end

local function SpeedgbpsFormat(data)
    if data == 1 then
        return 3
    elseif data == 2 then
        return 1.5
    elseif data == 3 then
        return 3
    elseif data == 4 then
        return 6
    elseif data == 5 then
        return 12
    else
        return null
    end
end

local function StripsizeFormat(size)
    local sizeBytes

    if size == 0 then
        sizeBytes = 512
    elseif size < 21 then
        sizeBytes = 1024 * bit.lshift(1, size - 1)
    else
        logging:error('invalid strip size is %d', size)
        return null
    end
    return sizeBytes
end

local function StripsizeToNum(size)
    if size == nil then
        return STRIPE_SIZE_BMC.STRIPE_SIZE_DEFAULT_BMC  -- 不存在时返回默认值
    end

    local sizeBytes
    if size == STRIPE_SIZE_SCHE.STRIPE_SIZE_16K_SCHE then
        sizeBytes = STRIPE_SIZE_BMC.STRIPE_SIZE_16K_BMC
    elseif size == STRIPE_SIZE_SCHE.STRIPE_SIZE_32K_SCHE then
        sizeBytes = STRIPE_SIZE_BMC.STRIPE_SIZE_32K_BMC
    elseif size == STRIPE_SIZE_SCHE.STRIPE_SIZE_64K_SCHE then
        sizeBytes = STRIPE_SIZE_BMC.STRIPE_SIZE_64K_BMC
    elseif size == STRIPE_SIZE_SCHE.STRIPE_SIZE_128K_SCHE then
        sizeBytes = STRIPE_SIZE_BMC.STRIPE_SIZE_128K_BMC
    elseif size == STRIPE_SIZE_SCHE.STRIPE_SIZE_256K_SCHE then
        sizeBytes = STRIPE_SIZE_BMC.STRIPE_SIZE_256K_BMC
    elseif size == STRIPE_SIZE_SCHE.STRIPE_SIZE_512K_SCHE then
        sizeBytes = STRIPE_SIZE_BMC.STRIPE_SIZE_512K_BMC
    elseif size == STRIPE_SIZE_SCHE.STRIPE_SIZE_1M_SCHE then
        sizeBytes = STRIPE_SIZE_BMC.STRIPE_SIZE_1M_BMC
    else
        logging:error('invalid strip size is %d', size)
        return STRIPE_SIZE_BMC.STRIPE_SIZE_DEFAULT_BMC
    end
    return sizeBytes
end

local function ParseStoragePolicy(ctrlOption, supportedPolicyStr1, supportedPolicyStr2, supportedPolicyStr3, offset)
    local result = {}
    local policy = {}
    if bit.band(ctrlOption, bit.lshift(1, offset + 7)) ~= 0 then
        result['Configurable'] = true
    else
        result['Configurable'] = false
    end
    if bit.band(ctrlOption, bit.lshift(1, offset)) ~= 0 and supportedPolicyStr1 ~= nil then
        policy[#policy + 1] = supportedPolicyStr1
    end
    if bit.band(ctrlOption, bit.lshift(1, offset + 1)) ~= 0 and supportedPolicyStr2 ~= nil then
        policy[#policy + 1] = supportedPolicyStr2
    end
    if bit.band(ctrlOption, bit.lshift(1, offset + 2)) ~= 0 and supportedPolicyStr3 ~= nil then
        policy[#policy + 1] = supportedPolicyStr3
    end

    result['SupportedPolicy'] = policy
    return result
end

local function VolumeStateToStr(state)
    local stateResultMap = {}
    stateResultMap[SML_LD_STATE_E.LD_STATE_OFFLINE] = RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_OFFLINE
    stateResultMap[SML_LD_STATE_E.LD_STATE_PARTIALLY_DEGRADED] =
        RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_PARTTIALLY_DEGRADED
    stateResultMap[SML_LD_STATE_E.LD_STATE_DEGRADED] = RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_DEGRADED
    stateResultMap[SML_LD_STATE_E.LD_STATE_OPTIMAL] = RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_OPTIMAL
    stateResultMap[SML_LD_STATE_E.LD_STATE_FAILED] = RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_FAILED
    stateResultMap[SML_LD_STATE_E.LD_STATE_NOT_CONFIGURED] = RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_NOT_CONFIGURED
    stateResultMap[SML_LD_STATE_E.LD_STATE_INTERIM_RECOVERY] = RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_INTERIM_RECOVERY
    stateResultMap[SML_LD_STATE_E.LD_STATE_READY_FOR_RECOVERY] =
        RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_READY_FOR_RECOVERY
    stateResultMap[SML_LD_STATE_E.LD_STATE_RECOVERYING] = RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_RECOVERYING
    stateResultMap[SML_LD_STATE_E.LD_STATE_WRONG_DRIVE_REPLACED] =
        RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_WRONG_DRIVE_REPLACED
    stateResultMap[SML_LD_STATE_E.LD_STATE_DRVIE_IMPROPERLY_CONNECTED] =
        RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_DRVIE_IMPROPERLY_CONNECTED
    stateResultMap[SML_LD_STATE_E.LD_STATE_EXPANDING] = RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_EXPANDING
    stateResultMap[SML_LD_STATE_E.LD_STATE_NOT_YET_AVAILABLE] =
        RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_NOT_YET_AVAILABLE
    stateResultMap[SML_LD_STATE_E.LD_STATE_QUEUED_FOR_EXPANSION] =
        RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_QUEUED_FOR_EXPANSION
    stateResultMap[SML_LD_STATE_E.LD_STATE_DISABLED_FROM_SCSIID_CONFLICT] =
        RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_DISABLED_FROM_SCSIID_CONFLICT
    stateResultMap[SML_LD_STATE_E.LD_STATE_EJECTED] = RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_EJECTED
    stateResultMap[SML_LD_STATE_E.LD_STATE_ERASE_IN_PROGRESS] =
        RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_ERASE_IN_PROGRESS
    stateResultMap[SML_LD_STATE_E.LD_STATE_UNUSED] = RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_UNUSED
    stateResultMap[SML_LD_STATE_E.LD_STATE_READY_TO_PERFORM_PREDICTIVE_SPARE_REBUILD] =
        RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_READY_TO_PERFORM_PREDICTIVE_SPARE_REBUILD
    stateResultMap[SML_LD_STATE_E.LD_STATE_RPI_IN_PROGRESS] = RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_RPI_IN_PROGRESS
    stateResultMap[SML_LD_STATE_E.LD_STATE_RPI_QUEUED] = RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_RPI_QUEUED
    stateResultMap[SML_LD_STATE_E.LD_STATE_ENCRYPTED_VOLUME_WITHOUT_KEY] =
        RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_ENCRYPTED_VOLUME_WITHOUT_KEY
    stateResultMap[SML_LD_STATE_E.LD_STATE_ENCRYPTION_MIGRATION] =
        RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_ENCRYPTION_MIGRATION
    stateResultMap[SML_LD_STATE_E.LD_STATE_ENCRYPTED_VOLUME_REKEYING] =
        RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_ENCRYPTED_VOLUME_REKEYING
    stateResultMap[SML_LD_STATE_E.LD_STATE_ENCRYPTED_VOLUME_ENCRYPTION_OFF] =
        RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_ENCRYPTED_VOLUME_ENCRYPTION_OFF
    stateResultMap[SML_LD_STATE_E.LD_STATE_VOLUME_ENCODE_REQUESTED] =
        RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_VOLUME_ENCODE_REQUESTED
    stateResultMap[SML_LD_STATE_E.LD_STATE_ENCRYPTED_VOLUME_REKEY_REQUESTED] =
        RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_ENCRYPTED_VOLUME_REKEY_REQUESTED
    stateResultMap[SML_LD_STATE_E.LD_STATE_UNSUPPORTED_ON_THIS_CONTROLLER] =
        RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_UNSUPPORTED_ON_THIS_CONTROLLER
    stateResultMap[SML_LD_STATE_E.LD_STATE_NOT_FORMATTED] =
        RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_NOT_FORMATTED
    stateResultMap[SML_LD_STATE_E.LD_STATE_FORMATTING] =
        RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_FORMATTING
    stateResultMap[SML_LD_STATE_E.LD_STATE_SANITIZING] =
        RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_SANITIZING
    stateResultMap[SML_LD_STATE_E.LD_STATE_INITIALIZING] =
        RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_INITIALIZING
    stateResultMap[SML_LD_STATE_E.LD_STATE_INITIALIZEFAIL] =
        RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_INITIALIZEFAIL
    stateResultMap[SML_LD_STATE_E.LD_STATE_DELETING] =
        RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_DELETING
    stateResultMap[SML_LD_STATE_E.LD_STATE_DELETEFAIL] =
        RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_DELETEFAIL
    stateResultMap[SML_LD_STATE_E.LD_STATE_WRITE_PROTECT] =
        RFPROP_VOLUME_STATE.RFPROP_VOLUME_STATE_WRITE_PROTECT
    return stateResultMap[state] or null
end

local function RaidLevelToStr(level)
    if level == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_1E then
        return 'RAID1E'
    elseif level == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_1ADM then
        return 'RAID1(ADM)'
    elseif level == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_10ADM then
        return 'RAID10(ADM)'
    elseif level == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_1TRIPLE then
        return 'RAID1Triple'
    elseif level == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_10TRIPLE then
        return 'RAID10Triple'
    elseif level <= SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_60 then
        return 'RAID' .. level
    else
        logging:error('invalid raid level is %d', level)
        return null
    end
end

local function ReadPolicyToStr(policy)
    if policy == SML_LD_READ_POLICY_E.LD_CACHE_NO_READ_AHEAD then
        return SUPORTED_POLICE_E.LD_READ_NO
    elseif policy == SML_LD_READ_POLICY_E.LD_CACHE_READ_AHEAD then
        return SUPORTED_POLICE_E.LD_READ_AHEAD
    else
        logging:error('invalid read policy is %d', policy)
        return null
    end
end

local function ReadPolicyToNum(policy)
    if policy == SUPORTED_POLICE_E.LD_READ_NO then
        return SML_LD_READ_POLICY_E.LD_CACHE_NO_READ_AHEAD
    elseif policy == SUPORTED_POLICE_E.LD_READ_AHEAD then
        return SML_LD_READ_POLICY_E.LD_CACHE_READ_AHEAD
    else
        logging:error('invalid read policy, set to defualt')
        return 0xff
    end
end

local function WritePolicyToStr(policy)
    if policy == SML_LD_WRITE_POLICY_E.LD_CACHE_WRITE_THROUGH then
        return SUPORTED_POLICE_E.LD_WRITE_THROUGH
    elseif policy == SML_LD_WRITE_POLICY_E.LD_CACHE_WRITE_BACK then
        return SUPORTED_POLICE_E.LD_WRITE_BACKBBU
    elseif policy == SML_LD_WRITE_POLICY_E.LD_CACHE_WRITE_CACHE_IF_BAD_BBU then
        return SUPORTED_POLICE_E.LD_WRITE_BACK
    elseif policy == SML_LD_WRITE_POLICY_E.LD_CACHE_READ_ONLY then
        return SUPORTED_POLICE_E.LD_WRITE_RO
    else
        logging:error('invalid write policy is %d', policy)
        return null
    end
end

local function WritePolicyToNum(policy)
    if policy == SUPORTED_POLICE_E.LD_WRITE_THROUGH then
        return SML_LD_WRITE_POLICY_E.LD_CACHE_WRITE_THROUGH
    elseif policy == SUPORTED_POLICE_E.LD_WRITE_BACKBBU then
        return SML_LD_WRITE_POLICY_E.LD_CACHE_WRITE_BACK
    elseif policy == SUPORTED_POLICE_E.LD_WRITE_BACK then
        return SML_LD_WRITE_POLICY_E.LD_CACHE_WRITE_CACHE_IF_BAD_BBU
    else
        logging:error('invalid write policy, set to defualt')
        return 0xff
    end
end

local function CachePolicyToStr(policy)
    if policy == SML_LD_CACHE_POLICY_E.LD_CACHE_CACHED_IO then
        return SUPORTED_POLICE_E.LD_IO_CACHE
    elseif policy == SML_LD_CACHE_POLICY_E.LD_CACHE_DIRECT_IO then
        return SUPORTED_POLICE_E.LC_IO_DIRECT
    else
        logging:error('invalid cache policy is %d', policy)
        return null
    end
end

local function CachePolicyToNum(policy)
    if policy == SUPORTED_POLICE_E.LD_IO_CACHE then
        return SML_LD_CACHE_POLICY_E.LD_CACHE_CACHED_IO
    elseif policy == SUPORTED_POLICE_E.LC_IO_DIRECT then
        return SML_LD_CACHE_POLICY_E.LD_CACHE_DIRECT_IO
    else
        logging:error('invalid cache policy, set to defualt')
        return 0xff
    end
end

local function AccessPolicyToStr(policy)
    if policy == SML_LD_ACCESS_POLICY_E.LD_ACCESS_RW then
        return SUPORTED_POLICE_E.LD_ACC_RW
    elseif policy == SML_LD_ACCESS_POLICY_E.LD_ACCESS_READ_ONLY then
        return SUPORTED_POLICE_E.LD_ACC_READ
    elseif policy == SML_LD_ACCESS_POLICY_E.LD_ACCESS_BLOCKED then
        return SUPORTED_POLICE_E.LD_ACC_BLOCK
    elseif policy == SML_LD_ACCESS_POLICY_E.LD_ACCESS_HIDDEN then
        return SUPORTED_POLICE_E.LD_ACC_HIDDEN
    else
        logging:error('invalid access policy is %d', policy)
        return null
    end
end

local function AccessPolicyToNum(policy)
    if policy == SUPORTED_POLICE_E.LD_ACC_RW then
        return SML_LD_ACCESS_POLICY_E.LD_ACCESS_RW
    elseif policy == SUPORTED_POLICE_E.LD_ACC_READ then
        return SML_LD_ACCESS_POLICY_E.LD_ACCESS_READ_ONLY
    elseif policy == SUPORTED_POLICE_E.LD_ACC_BLOCK then
        return SML_LD_ACCESS_POLICY_E.LD_ACCESS_BLOCKED
    elseif policy == SUPORTED_POLICE_E.LD_ACC_HIDDEN then
        return SML_LD_ACCESS_POLICY_E.LD_ACCESS_HIDDEN
    else
        logging:error('invalid access policy, set to defualt')
        return STOTAGE_INFO_INVALID.STORAGE_INFO_INVALID_BYTE
    end
end

local function DriveCachePolicyToStr(policy)
    if policy == SML_DISK_CACHE_POLICY_E.PD_CACHE_UNCHANGED then
        return SUPORTED_POLICE_E.LD_DCACHE_DEF
    elseif policy == SML_DISK_CACHE_POLICY_E.PD_CACHE_ENABLE then
        return SUPORTED_POLICE_E.LD_DCACHE_ENABLE
    elseif policy == SML_DISK_CACHE_POLICY_E.PD_CACHE_DISABLE then
        return SUPORTED_POLICE_E.LD_DCACHE_DISABLE
    else
        logging:error('invalid drive cache policy is %d', policy)
        return null
    end
end

local function DriveCachePolicyToNum(policy)
    if policy == SUPORTED_POLICE_E.LD_DCACHE_DEF then
        return SML_DISK_CACHE_POLICY_E.PD_CACHE_UNCHANGED
    elseif policy == SUPORTED_POLICE_E.LD_DCACHE_ENABLE then
        return SML_DISK_CACHE_POLICY_E.PD_CACHE_ENABLE
    elseif policy == SUPORTED_POLICE_E.LD_DCACHE_DISABLE then
        return SML_DISK_CACHE_POLICY_E.PD_CACHE_DISABLE
    else
        logging:error('invalid cache policy, set to defualt')
        return 0xff
    end
end

local function InitModeToStr(state)
    if state == LD_INIT_STATE_E.LD_INIT_STATE_NO_INIT then
        return SUPORTED_POLICE_E.LD_INITMODE_UN
    elseif state == LD_INIT_STATE_E.LD_INIT_STATE_QUICK_INIT then
        return SUPORTED_POLICE_E.LD_INITMODE_QUICK
    elseif state == LD_INIT_STATE_E.LD_INIT_STATE_FULL_INIT then
        return SUPORTED_POLICE_E.LD_INITMODE_FULL
    else
        logging:error('invalid ld init state is %d', state)
        return null
    end
end

local function InitModeToNum(state)
    if state == SUPORTED_POLICE_E.LD_INITMODE_UN then
        return LD_INIT_STATE_E.LD_INIT_STATE_NO_INIT
    elseif state == SUPORTED_POLICE_E.LD_INITMODE_QUICK then
        return LD_INIT_STATE_E.LD_INIT_STATE_QUICK_INIT
    elseif state == SUPORTED_POLICE_E.LD_INITMODE_FULL then
        return LD_INIT_STATE_E.LD_INIT_STATE_FULL_INIT
    elseif state == SUPORTED_POLICE_E.LD_INITMODE_RPI then
        return LD_INIT_STATE_E.LD_INIT_STATE_RPI
    elseif state == SUPORTED_POLICE_E.LD_INITMODE_OPO then
        return LD_INIT_STATE_E.LD_INIT_STATE_OPO
    elseif state == SUPORTED_POLICE_E.LD_INITMODE_FRONT then
        return LD_INIT_STATE_E.LD_INIT_STATE_FRONT
    elseif state == SUPORTED_POLICE_E.LD_INITMODE_BACKGROUND then
        return LD_INIT_STATE_E.LD_INIT_STATE_BACKGROUND
    else
        logging:error('invalid ld init state is %s, set to defualt', state)
        return STOTAGE_INFO_INVALID.STORAGE_INFO_INVALID_BYTE
    end
end

local function AccelerationMethodToStr(acc)

    if  acc > SML_LD_ACCELERATION_METHOD_E.LD_ACCELERATOR_MAXCACHE then
        return null
    end

    return ldAccelerationMethodMap[acc]
end

local function AccelerationMethodToNum(acc)
    if acc == SML_LD_ACCELERATION_METHOD_STR_E.LD_ACCELERATOR_NONE then
        return SML_LD_ACCELERATION_METHOD_E.LD_ACCELERATOR_NONE
    elseif acc == SML_LD_ACCELERATION_METHOD_STR_E.LD_ACCELERATOR_CACHE then
        return SML_LD_ACCELERATION_METHOD_E.LD_ACCELERATOR_CACHE
    elseif acc == SML_LD_ACCELERATION_METHOD_STR_E.LD_ACCELERATOR_IOBYPASS then
        return SML_LD_ACCELERATION_METHOD_E.LD_ACCELERATOR_IOBYPASS
    else
        logging:error('invalid acceleration method, set to defualt')
        return STOTAGE_INFO_INVALID.STORAGE_INFO_INVALID_BYTE
    end
end

local function CacheLineSizeToStr(cls)

    if cls == STOTAGE_INFO_INVALID.STORAGE_INFO_INVALID_BYTE then
        return null
    end

    return cls * 64 .. 'K'
end

local function CacheLineSizeToNum(cls)
    if cls == '64K' then
        return 1
    elseif cls == '256K' then
        return 4
    else
        logging:error('invalid cache line size, set to defualt')
        return STOTAGE_INFO_INVALID.STORAGE_INFO_INVALID_BYTE
    end
end

local function HealthToStr(health)
    if health == SML_LOGENTRY_STATUS_E.LOGENTRY_STATUS_INFORMATIONAL_CODE then
        return ALARM_LEVEL.INFORMATIONAL_STRING
    elseif health == SML_LOGENTRY_STATUS_E.LOGENTRY_STATUS_MINOR_CODE then
        return ALARM_LEVEL.MINOR_STRING
    elseif health == SML_LOGENTRY_STATUS_E.LOGENTRY_STATUS_MAJOR_CODE then
        return ALARM_LEVEL.MAJOR_STRING
    elseif health == SML_LOGENTRY_STATUS_E.LOGENTRY_STATUS_CRITICAL_CODE then
        return ALARM_LEVEL.CRITICAL_STRING
    else
        return null
    end
end

local function GetBBU(RAIDStorageObj)
    local BBU = {}
    local objName = RAIDStorageObj.RefObjectBBUStatus:fetch_or()
    if objName == nil or objName == '' then
        return null
    end
    local bbu_handle = O[objName]
    if bbu_handle == nil then
        return null
    end

    if bbu_handle.Present:fetch() == 0 then
        BBU['State'] = 'Absent'
        BBU['Health'] = bbu_handle.Health:next(HealthToStr):fetch()
    elseif bbu_handle.Present:fetch() == 1 then
        BBU['Name'] = bbu_handle.Type:next(CheckStrEffective):fetch()
        BBU['State'] = 'Enabled'
        BBU['Health'] = bbu_handle.Health:next(HealthToStr):fetch()
    else
        return null
    end

    return BBU
end

local function MediaTypeStr(type)
    if type == RF_DEVIES_MEDIATYPE.RF_DEVIES_MEDIATYPE_HDD_VALUE then
        return 'HDD'
    elseif type == RF_DEVIES_MEDIATYPE.RF_DEVIES_MEDIATYPE_SSD_VALUE then
        return 'SSD'
    elseif type == RF_DEVIES_MEDIATYPE.RF_DEVIES_MEDIATYPE_SSM_VALUE then
        return 'SSM'
    else
        logging:error('get mediatype unknown.')
        return null
    end
end

local function FirmwareStatusToStr(status)
    local stateResultMap = {}
    stateResultMap[SML_PD_STATE_E.PD_STATE_UNCONFIGURED_GOOD] =
        RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_UNCONFIGUREDGOOD
    stateResultMap[SML_PD_STATE_E.PD_STATE_UNCONFIGURED_BAD] =
        RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_UNCONFIGUREDBAD
    stateResultMap[SML_PD_STATE_E.PD_STATE_HOT_SPARE] = RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_HOTSPARE
    stateResultMap[SML_PD_STATE_E.PD_STATE_OFFLINE] = RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_CONFIGUREDGOOD
    stateResultMap[SML_PD_STATE_E.PD_STATE_FAILED] = RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_CONFIGUREDBAD
    stateResultMap[SML_PD_STATE_E.PD_STATE_REBUILD] = RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_REBUILDING
    stateResultMap[SML_PD_STATE_E.PD_STATE_ONLINE] = RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_ONLINE
    stateResultMap[SML_PD_STATE_E.PD_STATE_EPD] = RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_ONLINE
    stateResultMap[SML_PD_STATE_E.PD_STATE_COPYBACK] = RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_GETTINGCOPIED
    stateResultMap[SML_PD_STATE_E.PD_STATE_SYSTEM] = RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_CONTROLLEDBYHOST
    stateResultMap[SML_PD_STATE_E.PD_STATE_SHIELD_UNCONFIGURED] =
        RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_UNCONFIGUREDSHIELDED
    stateResultMap[SML_PD_STATE_E.PD_STATE_SHIELD_HOT_SPARE] =
        RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_HOTSPARESHIELDED
    stateResultMap[SML_PD_STATE_E.PD_STATE_SHIELD_CONFIGURED] =
        RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_CONFIGUREDSHIELDED
    stateResultMap[SML_PD_STATE_E.PD_STATE_FOREIGN] = RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_FOREIGN
    stateResultMap[SML_PD_STATE_E.PD_STATE_ACTIVE] = RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_ACTIVE
    stateResultMap[SML_PD_STATE_E.PD_STATE_STANDBY] = RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_STANDBY
    stateResultMap[SML_PD_STATE_E.PD_STATE_SLEEP] = RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_SLEEP
    stateResultMap[SML_PD_STATE_E.PD_STATE_DST] = RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_DST
    stateResultMap[SML_PD_STATE_E.PD_STATE_SMART] = RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_SMART
    stateResultMap[SML_PD_STATE_E.PD_STATE_SCT] = RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_SCT
    stateResultMap[SML_PD_STATE_E.PD_STATE_RAW] = RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_RAW
    stateResultMap[SML_PD_STATE_E.PD_STATE_READY] = RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_READY
    stateResultMap[SML_PD_STATE_E.PD_STATE_NOT_SUPPORTED] = RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_NOTSUPPORTED
    stateResultMap[SML_PD_STATE_E.PD_STATE_PREDICTIVE_FAILURE] = RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_PREDICTIVE_FAILURE
    stateResultMap[SML_PD_STATE_E.PD_STATE_DIAGNOSING] = RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_DIAGNOSING
    stateResultMap[SML_PD_STATE_E.PD_STATE_INCOMPATIBLE] = RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_INCOMPATIBLE
    stateResultMap[SML_PD_STATE_E.PD_STATE_ERASING] = RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_ERASING
    return stateResultMap[status]
end

local function FirmwareStatusToNum(status)
    local ret
    if status == RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_ONLINE then
        ret = SML_PD_STATE_E.PD_STATE_ONLINE
    elseif status == RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_CONFIGUREDGOOD then
        ret = SML_PD_STATE_E.PD_STATE_OFFLINE
    elseif status == RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_CONTROLLEDBYHOST then
        ret = SML_PD_STATE_E.PD_STATE_SYSTEM
    elseif status == RF_PHYSICALDISKSTATE_E.RF_PHYSICALDISKSTATE_UNCONFIGUREDGOOD then
        ret = SML_PD_STATE_E.PD_STATE_UNCONFIGURED_GOOD
    else
        logging:error('FirmwareStatus value invalid')
    end

    return ret
end

local function GetStorageLogicalRsc(controllerObj)
    local controllerName = object_name(controllerObj)
    local ldArray = C.LogicalDrive():fold(function(ldObj, array)
        local drivesArray = {}
        if ldObj.RaidLevel:fetch() == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_DELETED then
            return array
        end

        if ldObj.RefRAIDController:fetch() ~= controllerName then
            return array
        end
        local participatedPDEnclosuresArray = ldObj.ParticipatedPDEnclosure:fetch()
        local participatedPDSlotsArray = ldObj.ParticipatedPDSlot:fetch()
        if participatedPDEnclosuresArray == nil or participatedPDSlotsArray == nil then
            return array
        end
        for i = 1, #participatedPDEnclosuresArray, 1 do
            local driveObj = C.Hdd():fold(function(obj)
                local refRaidObjName = obj.RefRAIDController:fetch()
                local slotNumber = obj.SlotNumber:fetch()
                local enclosureDeviceId = obj.EnclosureDeviceId:fetch()
                if refRaidObjName == controllerName and slotNumber == participatedPDSlotsArray[i] and
                    enclosureDeviceId == participatedPDEnclosuresArray[i] then
                    return obj, false
                end
            end):fetch_or()
            if driveObj ~= nil then
                local drivesID = string.gsub(driveObj.PhysicalLocation:fetch(), '%s+', '') ..
                                     string.gsub(driveObj.DeviceName:fetch(), '%s+', '')
                drivesArray[#drivesArray + 1] = drivesID
            end
        end
        array[#array + 1] = {Members = drivesArray, LogicalDriveHandle = ldObj}
        return array
    end, {}):fetch_or()

    return ldArray
end

local function GetControllerRelatedDrivesMembers(obj, ldArray)
    local ldObj
    local drivesMembers = {}

    if ldArray == nil then
        return nil, nil
    end

    local drivesID = string.gsub(obj.PhysicalLocation:fetch(), '%s+', '') ..
                         string.gsub(obj.DeviceName:fetch(), '%s+', '')
    for i = 1, #ldArray, 1 do
        local memberArray = ldArray[i]['Members']
        for j = 1, #memberArray, 1 do
            if drivesID == memberArray[j] then
                drivesMembers = memberArray
                ldObj = ldArray[i]['LogicalDriveHandle']
                return drivesMembers, ldObj
            end
        end
    end
end

local function recalculatePdCount(arrayPdCount, raidLevel)
    if raidLevel == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_1 or raidLevel ==
        SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_10 then
        if arrayPdCount % 2 ~= 0 then
            return nil
        end
        return arrayPdCount / 2
    elseif raidLevel == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_5 or raidLevel ==
        SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_50 then
        if arrayPdCount < 3 then
            return nil
        end
        return arrayPdCount - 1
    elseif raidLevel == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_6 or raidLevel ==
        SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_60 then
        if arrayPdCount < 3 then
            return nil
        end
        return arrayPdCount - 2
    elseif raidLevel == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_1ADM or 
           raidLevel == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_10ADM or 
           raidLevel == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_1TRIPLE or 
           raidLevel == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_10TRIPLE then
        if arrayPdCount % 3 ~= 0 then
            return nil
        end
        return arrayPdCount / 3
    else
        return arrayPdCount
    end
end

local function UseSubArray(refRaidController)
    local raidObj = O[refRaidController]

    if raidObj == nil then
        logging:error('refRaidController = %s', refRaidController)
        return false
    end

    if raidObj.TypeID:fetch() == CONTROLLER_TYPE_ID_E.PMC_3152_8I_SMART_RAID or
       raidObj.TypeID:fetch() == CONTROLLER_TYPE_ID_E.PMC_2100_8I_SMART_HBA then
        return true
    elseif raidObj.TypeID:fetch() >= CONTROLLER_TYPE_ID_E.HI1880_SP186_M_16i and
           raidObj.TypeID:fetch() <= CONTROLLER_TYPE_ID_E.HI1880_SP186_M_8i then
        return true
    else
        return false
    end
end

local function GetControllerRelatedDrivesSpaceData(ldObj)
    local totalFreeSpaceMiB
    local freeSpaceMiBPerDrive
    local freeBlocksSpaceMiB = {}
    local spanDepth = ldObj.SpanDepth:fetch()
    local ldRefArrayList = ldObj.RefArray:fetch()

    if ldRefArrayList == nil or #ldRefArrayList == 0 then
        return null, null
    end
    local ldRefRaidName = ldObj.RefRAIDController:fetch()
    if UseSubArray(ldRefRaidName) == true then
        spanDepth = 1;  --对于启用DiskSubArray的控制器来说，Array的子组数固定为1
    end

    local array_obj_handle = C.DiskArray():fold(function(obj)
        if ldRefArrayList[1] == obj.ArrayID:fetch() and ldRefRaidName == obj.RefRAIDController:fetch() then
            return obj, false
        end
    end):fetch()

    local freeSpace = array_obj_handle.TotalFreeSpace:fetch()
    local arrayPdCount = array_obj_handle.PDCount:fetch()
    if arrayPdCount == 0 then
        return null, null
    end

    if string.find(object_name(array_obj_handle), 'DiskArrayDynamic', 1) then
        freeSpaceMiBPerDrive = freeSpace / arrayPdCount
        totalFreeSpaceMiB = null
    else
        local raidLevel = ldObj.RaidLevel:fetch_or(0)
        arrayPdCount = recalculatePdCount(arrayPdCount, raidLevel)
        if arrayPdCount == nil then
            freeSpaceMiBPerDrive = null
        else
            freeSpaceMiBPerDrive = freeSpace / arrayPdCount
        end
        totalFreeSpaceMiB = freeSpace * spanDepth
    end

    local freeBlocksSpace = array_obj_handle.FreeBlocksSpace:fetch_or({})
    for i = 1, #freeBlocksSpace, 1 do
        if freeBlocksSpace[i] ~= STOTAGE_INFO_INVALID.STORAGE_INFO_INVALID_DWORD then
            freeBlocksSpaceMiB[#freeBlocksSpaceMiB + 1] = spanDepth * freeBlocksSpace[i]
        end
    end

    return totalFreeSpaceMiB, freeSpaceMiBPerDrive, freeBlocksSpaceMiB
end

local function checkIfEPD(FirmwareStatus)
    if FirmwareStatus == SML_PD_STATE_E.PD_STATE_EPD then
        return true
    else
        return false
    end
end

local function CapacityMBToBytes(Size)
    if Size < 0 or Size == STOTAGE_INFO_INVALID.STORAGE_INFO_INVALID_DWORD then
        return null
    else
        return Size * 1024 * 1024
    end
end

local function GetControllerDrives(controllerObj)
    local controllerObjName = object_name(controllerObj)
    local drivesOutputArray = C.Hdd():fold(function(obj, array)
        if obj.RefRAIDController:fetch() ~= controllerObjName then
            return array
        end

        if obj.Presence:fetch() ~= 1 then
            return array
        end
        local drivesID = string.gsub(obj.PhysicalLocation:fetch(), '%s+', '') ..
                             string.gsub(obj.DeviceName:fetch(), '%s+', '')
        local ldArray = GetStorageLogicalRsc(controllerObj)
        local totalFreeSpaceMiB, freeSpaceMiBPerDrive, freeBlocksSpaceMiB, raidLevel, numDrivePerSpan, isSSCD, cachePolicy
        local membersArray, ldObj = GetControllerRelatedDrivesMembers(obj, ldArray)
        if ldObj ~= nil then
            raidLevel = ldObj.RaidLevel:next(RaidLevelToStr):fetch_or(null)
            numDrivePerSpan = ldObj.NumDrivePerSpan:neq(STOTAGE_INFO_INVALID.STORAGE_INFO_INVALID_BYTE):fetch_or(null)
            cachePolicy = ldObj.DiskCachePolicy:next(DriveCachePolicyToStr):fetch_or(null)
            totalFreeSpaceMiB, freeSpaceMiBPerDrive, freeBlocksSpaceMiB =
                GetControllerRelatedDrivesSpaceData(ldObj)
            isSSCD = ldObj.IsSSCD:next(IntToBool):fetch()
        end
        array[#array + 1] = {
            IsEPD = obj.FirmwareStatus:next(checkIfEPD):fetch_or(),
            ID = obj.Id:fetch(),
            DriveID = drivesID,
            Name = obj.DeviceName:next(CheckStrEffective):fetch(),
            CapacityBytes = obj.CapacityMB:next(CapacityMBToBytes):fetch(),
            MediaType = obj.MediaType:next(MediaTypeStr):fetch(),
            Protocol = obj.InterfaceTypeString:next(CheckStrEffective):next(ProtocolStrFormat):fetch_or(),
            FirmwareStatus = obj.FirmwareStatus:next(FirmwareStatusToStr):fetch(),
            VolumeRaidLevel = raidLevel,
            DiskCachePolicy = cachePolicy,
            NumDrivePerSpan = numDrivePerSpan,
            Members = membersArray,
            TotalFreeSpaceMiB = totalFreeSpaceMiB,
            FreeSpaceMiBPerDrive = freeSpaceMiBPerDrive,
            FreeBlocksSpaceMiB = freeBlocksSpaceMiB,
            SSDCachecadeVolume = isSSCD,
        }
        return array
    end, {}):fetch_or()
    return drivesOutputArray
end

local function GetCtrlBootDevices(controllerObj)
    local result = {}
    local BootDevices = controllerObj.BootDevices:fetch_or({})

    for i = 1, #BootDevices, 1 do
        result[#result + 1] = BootDevices[i]
    end

    return result
end

local function DriveWriteCachePolicyToNum(policy)
    if policy == CTRL_DRIVE_WCP_NAME.CTRL_DRIVE_WCP_DEFAULT_STR then
        return CTRL_DRIVE_WCP_E.CTRL_DRIVE_WCP_DEFAULT
    elseif policy == CTRL_DRIVE_WCP_NAME.CTRL_DRIVE_WCP_ENABLED_STR then
        return CTRL_DRIVE_WCP_E.CTRL_DRIVE_WCP_ENABLED
    elseif policy == CTRL_DRIVE_WCP_NAME.CTRL_DRIVE_WCP_DISABLED_STR then
        return CTRL_DRIVE_WCP_E.CTRL_DRIVE_WCP_DISABLED
    else
        return CTRL_DRIVE_WCP_E.CTRL_DRIVE_WCP_INVALID
    end
end

local function DriveWriteCachePolicyToStr(wcp)
    if wcp == CTRL_DRIVE_WCP_E.CTRL_DRIVE_WCP_DEFAULT then
        return "Default"
    elseif wcp == CTRL_DRIVE_WCP_E.CTRL_DRIVE_WCP_ENABLED then
        return "Enabled"
    elseif wcp == CTRL_DRIVE_WCP_E.CTRL_DRIVE_WCP_DISABLED then
        return "Disabled"
    else
        return "Invalid"
    end
end

local function GetCtrlDriveWriteCachePolicy(opt)
    local result = {}
    result['ConfiguredDrive'] = DriveWriteCachePolicyToStr(bit.band(opt, 0x3))
    result['UnconfiguredDrive'] = DriveWriteCachePolicyToStr(bit.rshift(bit.band(opt, 0xC), 2))
    result['HBADrive'] = DriveWriteCachePolicyToStr(bit.rshift(bit.band(opt, 0x30), 4))
    return result
end

local function GetVSPcieDrives(drivesArray)
    C.PCIeDisk():fold(function(obj)
        if obj.Presence:fetch_or() ~= 1 then
            return
        end

        table.insert(drivesArray, {
            Drives_ID = string.gsub(obj.PhysicalLocation:fetch_or(''), '%s+', '') ..
                string.gsub(obj.DeviceName:fetch_or(''), '%s+', ''),
            Name = obj.DeviceName:fetch_or()
        })
    end):fetch_or()
    return drivesArray
end

local function GetVSPhysicalDrives()
    local startSlot = C.HDDBackplane[0].StartSlot:fetch_or(0)
    local drivesOutputArray = C.Hdd():fold(function(obj, array)
        if obj.Presence:fetch() ~= 1 or obj.Id:fetch() < startSlot then
            return array
        end

        local hddRefRaidController = obj.RefRAIDController:fetch()
        if #hddRefRaidController ~= 0 and hddRefRaidController ~= 'N/A' and hddRefRaidController ~= 'PCH' then
            if rawget(O[hddRefRaidController], "__handle") then
                return array
            end
        end

        array[#array + 1] = {
            Drives_ID = string.gsub(obj.PhysicalLocation:fetch(), '%s+', '') ..
                string.gsub(obj.DeviceName:fetch(), '%s+', ''),
            Name = obj.DeviceName:fetch()
        }
        return array
    end, {}):fetch_or({})

    return GetVSPcieDrives(drivesOutputArray)
end

local function RaidCtrlModeTostr(mode)
    local raidCtrlModeName = {}
    raidCtrlModeName[CTRL_MODE_E.CTRL_MODE_RAID] = RAID_CTRL_MODE_NAME.CTRL_MODE_RAID_STR
    raidCtrlModeName[CTRL_MODE_E.CTRL_MODE_HBA] = RAID_CTRL_MODE_NAME.CTRL_MODE_HBA_STR
    raidCtrlModeName[CTRL_MODE_E.CTRL_MODE_JBOD] = RAID_CTRL_MODE_NAME.CTRL_MODE_JBOD_STR
    raidCtrlModeName[CTRL_MODE_E.CTRL_MODE_MIXED] = RAID_CTRL_MODE_NAME.CTRL_MODE_MIXED_STR
    if mode >= #raidCtrlModeName then
        return RAID_CTRL_MODE_NAME.CTRL_MODE_UNKNOWN_STR
    end
    return raidCtrlModeName[mode]
end

local function TypeIDToBoolean(typeID)
    if typeID == CONTROLLER_TYPE_ID_E.LSI_3108_WITH_IT or typeID == CONTROLLER_TYPE_ID_E.LSI_3008_WITH_IT or typeID ==
        CONTROLLER_TYPE_ID_E.LSI_3408_WITH_IT or typeID == CONTROLLER_TYPE_ID_E.LSI_3416_WITH_IT then
        return false
    else
        return true
    end
end

local function TypeIDToString(typeID)
    return CONTROLLER_TYPE_MAP[typeID] or null
end

local function GetVSSpanPhysicalDrives(diskArrayObj, spanHotspareHddSlotNumbers, hotspareHddMaxSize)
    local drivesOutputArray
    local refRaidController = diskArrayObj.RefRAIDController:fetch()
    local refPDSlotsArray = diskArrayObj.RefPDSlots:fetch_or({})
    if #refPDSlotsArray == 0 or refPDSlotsArray[1] == 255 then
        return drivesOutputArray, spanHotspareHddSlotNumbers
    end

    local refPDEnclosuresArray = diskArrayObj.RefPDEnclosures:fetch()
    if #refPDEnclosuresArray == 0 or refPDEnclosuresArray == nil then
        return drivesOutputArray, spanHotspareHddSlotNumbers
    end

    drivesOutputArray = C.Hdd():fold(function(obj, array)
        if obj.RefRAIDController:fetch() ~= refRaidController then
            return array
        end

        if obj.Presence:fetch() ~= 1 then
            return array
        end

        local hddId = obj.Id:fetch()
        local slotNumber = obj.SlotNumber:fetch()
        local enclosureDeviceId = obj.EnclosureDeviceId:fetch()

        if hddId >= hotspareHddMaxSize then
            return array
        end

        local flag = false
        for i = 1, math.max(#refPDSlotsArray, #refPDEnclosuresArray), 1 do
            if slotNumber == refPDSlotsArray[i] and enclosureDeviceId == refPDEnclosuresArray[i] then
                flag = true
                break
            end
        end

        if flag == false then
            return array
        end

        array[#array + 1] = {
            Drives_ID = string.gsub(obj.PhysicalLocation:fetch(), '%s+', '') ..
                string.gsub(obj.DeviceName:fetch(), '%s+', ''),
            Name = obj.DeviceName:fetch()
        }
        spanHotspareHddSlotNumbers[hddId] = 1
        return array
    end, {}):fetch_or()
    return drivesOutputArray, spanHotspareHddSlotNumbers
end

local function GetVSVolumesSpans(volumeObj, spanHotspareHddSlotNumbers, hotspareHddMaxSize)
    local driveArray
    local spanDepth = volumeObj.SpanDepth:fetch()
    if spanDepth < SPAN_DEPTH.SPAN_DEPTH_MINI or spanDepth == SPAN_DEPTH.SPAN_DEPTH_INVALID then
        return null, spanHotspareHddSlotNumbers
    end
    local refRaidController = volumeObj.RefRAIDController:fetch()
    local refListArray = volumeObj.RefArray:fetch()

    local spanNum = 0
    local diskArrayClass
    if UseSubArray(refRaidController) == true then
        diskArrayClass = "DiskSubArray"
    else 
        diskArrayClass = "DiskArray"
    end
    local spansOutputArray = C[diskArrayClass]():fold(function(obj, array)
        if obj.RefRAIDController:fetch() ~= refRaidController then
            return array
        end

        local flag = false
        for i = 1, #refListArray, 1 do
            if obj.ArrayID:fetch() == refListArray[i] then
                flag = true
                break
            end
        end
        if flag == false then
            return array
        end
        driveArray, spanHotspareHddSlotNumbers = GetVSSpanPhysicalDrives(obj, spanHotspareHddSlotNumbers,
            hotspareHddMaxSize)
        array[#array + 1] = {Name = 'Span' .. spanNum, Drives = driveArray}
        spanNum = spanNum + 1
        return array
    end, {}):fetch()
    return spansOutputArray, spanHotspareHddSlotNumbers
end

local function GetVSHotSparesPhysicalDrives(volumesObj, spanHotspareHddSlotNumbers, hotspareHddMaxSize)
    local drivesOutputArray
    local refRaidController = volumesObj.RefRAIDController:fetch()
    local dedicatedSparedPDSlotsArray = volumesObj.DedicatedSparedPDSlots:fetch()
    if #dedicatedSparedPDSlotsArray == 0 or dedicatedSparedPDSlotsArray == nil then
        return drivesOutputArray, spanHotspareHddSlotNumbers
    end
    local dedicatedSparedPDEnclosuresArray = volumesObj.DedicatedSparedPDEnclosures:fetch()
    if #dedicatedSparedPDEnclosuresArray == 0 or dedicatedSparedPDEnclosuresArray == nil then
        return drivesOutputArray, spanHotspareHddSlotNumbers
    end

    drivesOutputArray = C.Hdd():fold(function(obj, array)
        if obj.RefRAIDController:fetch() ~= refRaidController then
            return array
        end

        if obj.Presence:fetch() ~= 1 then
            return array
        end

        local hddId = obj.Id:fetch()
        local slotNumber = obj.SlotNumber:fetch()
        local enclosureDeviceId = obj.EnclosureDeviceId:fetch()

        if hddId >= hotspareHddMaxSize then
            return array
        end

        local flag = false
        for i = 1, math.max(#dedicatedSparedPDSlotsArray, #dedicatedSparedPDEnclosuresArray), 1 do
            if slotNumber == dedicatedSparedPDSlotsArray[i] and enclosureDeviceId ==
                dedicatedSparedPDEnclosuresArray[i] then
                flag = true
                break
            end
        end

        if flag == false then
            return array
        end

        array[#array + 1] = {
            Drives_ID = string.gsub(obj.PhysicalLocation:fetch(), '%s+', '') ..
                string.gsub(obj.DeviceName:fetch(), '%s+', ''),
            Name = obj.DeviceName:fetch()
        }
        spanHotspareHddSlotNumbers[hddId] = 1
        return array
    end, {}):fetch_or()
    return drivesOutputArray, spanHotspareHddSlotNumbers
end

local function GetVSVolumesHotSpares(volumesObj, spanHotspareHddSlotNumbers, hotspareHddMaxSize)
    local driveArray = {}
    local spanHotspare
    local dedicatedSparedPDSlots = volumesObj.DedicatedSparedPDSlots:fetch()
    if #dedicatedSparedPDSlots == 0 or dedicatedSparedPDSlots[1] == 255 then
        return spanHotspare, spanHotspareHddSlotNumbers
    end

    driveArray, spanHotspareHddSlotNumbers = GetVSHotSparesPhysicalDrives(volumesObj, spanHotspareHddSlotNumbers,
        hotspareHddMaxSize)
    if driveArray == nil or #driveArray == 0 then
        return null, spanHotspareHddSlotNumbers
    end
    spanHotspare = {Name = 'Hot Spare', Drives = driveArray}

    return spanHotspare, spanHotspareHddSlotNumbers
end

local function is_hdd_crypto_erase_supported(obj)
    return obj.FDECapable:eq(1):next(function()
        local raidObj = O[obj.RefRAIDController:fetch()]
        local ctrlOption2 = raidObj.CtrlOption2:fetch_or()
        return ctrlOption2 ~= STORAGE_INFO_INVALID_DWORD and
                   bit.band(ctrlOption2, CTRL_OPTION2_SUPPORT_CRYPTO_ERASE) == CTRL_OPTION2_SUPPORT_CRYPTO_ERASE
    end):fetch_or(false)
end

local function GetVSVolumesPhysicalDrives(volumeObj, hotspareHddMaxSize,
    hddSlotNumbers, _)
    local drivesArray
    local startSlot = C.HDDBackplane[0].StartSlot:fetch_or(0)
    local refRaidController = volumeObj.RefRAIDController:fetch()
    local participatedPDSlotsArray = volumeObj.ParticipatedPDSlot:fetch_or({})
    if #participatedPDSlotsArray == 0 or participatedPDSlotsArray == 255 then
        return drivesArray, hddSlotNumbers
    end
    local participatedPDEnclosuresArray = volumeObj.ParticipatedPDEnclosure:fetch_or({})
    if #participatedPDEnclosuresArray == 0 or participatedPDEnclosuresArray == 255 then
        return drivesArray, hddSlotNumbers
    end

    local drivesOutputArray = C.Hdd():fold(function(obj, array)
        if obj.RefRAIDController:fetch() ~= refRaidController then
            return array
        end

        if obj.Presence:fetch() ~= 1 then
            return array
        end

        local hddId = obj.Id:fetch()
        -- 针对某个硬盘，允许既出现在LD0的Spans也出现在LD1的Drives，因此此处需要删去对Spans的过滤，使所有硬盘都能继续往下判断是否为当前LD的成员盘
        if hddId < startSlot or hddId >= hotspareHddMaxSize then
            return array
        end

        local slotNumber = obj.SlotNumber:fetch()
        local enclosureDeviceId = obj.EnclosureDeviceId:fetch()
        local flag = false
        for i = 1, math.max(#participatedPDSlotsArray, #participatedPDEnclosuresArray), 1 do
            if slotNumber == participatedPDSlotsArray[i] and enclosureDeviceId == participatedPDEnclosuresArray[i] then
                flag = true
                break
            end
        end

        if flag == false then
            return array
        end

        array[#array + 1] = {
            Drives_ID = string.gsub(obj.PhysicalLocation:fetch(), '%s+', '') ..
                string.gsub(obj.DeviceName:fetch(), '%s+', ''),
            Name = obj.DeviceName:fetch(),
            CryptoEraseSupported = is_hdd_crypto_erase_supported(obj)
        }
        hddSlotNumbers[hddId] = 1
        return array
    end, {}):fetch_or()
    return drivesOutputArray, hddSlotNumbers
end

local function GetVSControllerVolumes(controllerObj, spanHotspareHddSlotNumbers, hotspareHddMaxSize,
    hddSlotNumbers, hddMaxSize)
    local spansArray = null
    local hotSparesArray
    if hotspareHddMaxSize > MAX_SLOT_NUMBER + 1 or hddMaxSize > MAX_SLOT_NUMBER + 1 then
        logging:error('GetVSControllerVolumes, Input param error.')
        return null
    end

    local controllerObjName = object_name(controllerObj)

    local volumesArray = C.LogicalDrive():fold(function(volumeObj, array)
        local refRaidController = volumeObj.RefRAIDController:fetch()

        if refRaidController ~= controllerObjName then
            return array
        end
        local volumeId = volumeObj.TargetID:fetch()
        spansArray, spanHotspareHddSlotNumbers = GetVSVolumesSpans(volumeObj, spanHotspareHddSlotNumbers,
            hotspareHddMaxSize)
        hotSparesArray, spanHotspareHddSlotNumbers = GetVSVolumesHotSpares(volumeObj, spanHotspareHddSlotNumbers,
            hotspareHddMaxSize)
        -- 作为某个逻辑盘的所有成员盘，要么全部出现在Spans，要么全部出现在Drives，即Spans和Drives这两项当中只可能有一项有效
        local driveArray = null
        if spansArray == null then
            driveArray, hddSlotNumbers = GetVSVolumesPhysicalDrives(volumeObj,
            hotspareHddMaxSize, hddSlotNumbers, hddMaxSize)
        end
        array[#array + 1] = {
            Volumes_ID = 'LogicalDrive' .. volumeId,
            Name = 'Logical Drive ' .. volumeId,
            Drives = driveArray,
            Spans = spansArray,
            HotSpares = hotSparesArray,
            VolumeRaidLevel = volumeObj.RaidLevel:next(RaidLevelToStr):fetch_or(),
            SSDCachecadeVolume = volumeObj.IsSSCD:next(IntToBool):fetch(),
            SSDCachingEnabled = volumeObj.CacheCadeLD:neq(0xff):next(IntToBool):fetch_or(null),
            CapacityBytes = volumeObj.Size:next(CapacityMBToBytes):fetch()
        }
        return array
    end, {}):fetch_or()

    return volumesArray, spanHotspareHddSlotNumbers, hddSlotNumbers
end

local function GetVSControllerPhysicalDrives(controllerObj, spanHotspareHddSlotNumbers, hotspareHddMaxSize,
    hddSlotNumbers, hddMaxSize)
    local startSlot = C.HDDBackplane[0].StartSlot:fetch_or(0)
    local controllerObjName = object_name(controllerObj)
    local drivesOutputArray = C.Hdd():fold(function(obj, array)
        if obj.RefRAIDController:fetch() ~= controllerObjName then
            return array
        end

        if obj.Presence:fetch() ~= 1 then
            return array
        end

        local hddId = obj.Id:fetch()
        if hddId < startSlot or hddId >= hotspareHddMaxSize or hddId >= hddMaxSize or
            (spanHotspareHddSlotNumbers and spanHotspareHddSlotNumbers[hddId]) or (hddSlotNumbers and hddSlotNumbers[hddId]) then
            return array
        end

        array[#array + 1] = {
            CryptoEraseSupported = is_hdd_crypto_erase_supported(obj),
            Drives_ID = string.gsub(obj.PhysicalLocation:fetch(), '%s+', '') ..
                string.gsub(obj.DeviceName:fetch(), '%s+', ''),
            Name = obj.DeviceName:fetch()
        }
        return array
    end, {}):fetch_or()
    return drivesOutputArray
end

local function is_jbod_state_supported(ctrlOption2)
    --第19bit位表示是否支持JBOD功能
    if bit.band(bit.lshift(1, 19), ctrlOption2) ~= 0 then
        return true
    else 
        return false
    end
end

local function is_epd_supported(ctrlOption2)
    --第18bit位表示是否支持创建或者删除EPD
    if bit.band(bit.lshift(1, 18), ctrlOption2) ~= 0 then
        return true
    else 
        return false
    end
end

local function GetVSRaidControllers()
    local drivesArray
    local volumesArray
    local controllersArray = C.RaidController():fold(function(controllerObj, array)
    local hddSlotNumbers = {}
    local spanHotspareHddSlotNumbers = {}
        volumesArray, spanHotspareHddSlotNumbers, hddSlotNumbers =
            GetVSControllerVolumes(controllerObj, spanHotspareHddSlotNumbers, MAX_SLOT_NUMBER + 1, hddSlotNumbers,
                MAX_SLOT_NUMBER + 1)
        drivesArray = GetVSControllerPhysicalDrives(controllerObj, spanHotspareHddSlotNumbers, MAX_SLOT_NUMBER + 1,
            hddSlotNumbers, MAX_SLOT_NUMBER + 1)
        local ctrlOption2 = controllerObj.CtrlOption2:fetch_or()
        array[#array + 1] = {
            RaidController_ID = 'RAIDStorage' .. controllerObj.Id:fetch(),
            Name = controllerObj.ComponentName:fetch(),
            Mode = controllerObj.Mode:next(RaidCtrlModeTostr):fetch(),
            VolumeSupported = controllerObj.TypeID:next(TypeIDToBoolean):fetch(),
            Type = controllerObj.TypeID:next(TypeIDToString):fetch(),
            Drives = drivesArray,
            Volumes = volumesArray,
            JbodStateSupported  = is_jbod_state_supported(ctrlOption2),
            EpdSupported = is_epd_supported(ctrlOption2)
        }
        return array
    end, {}):fetch_or()
    return controllersArray
end

local function GetFreeBlocksSpaceMiB(obj, spanDepth)
    local outputArray = {}
    local freeBlockArray = obj.FreeBlocksSpace:fetch_or()
    if not freeBlockArray or #freeBlockArray == 0 then
        return outputArray
    end

    for i = 1, #freeBlockArray, 1 do
        if freeBlockArray[i] ~= STOTAGE_INFO_INVALID.STORAGE_INFO_INVALID_DWORD and freeBlockArray[i] ~= -1 then
            outputArray[#outputArray + 1] = freeBlockArray[i] * spanDepth
        end
    end
    return outputArray
end

local function GetVolumeSpans(volumeObj)
    local spansArray = {}
    local spanNum = volumeObj.SpanDepth:fetch()
    if spanNum == STOTAGE_INFO_INVALID.STORAGE_INFO_INVALID_BYTE then
        logging:error('invalid span number is %d', spanNum)
        return null
    end
    if spanNum == 1 then
        return spansArray
    end

    local redArray = volumeObj.RefArray:fetch_or({})
    local refRaidControllerObjName = volumeObj.RefRAIDController:fetch()

    local diskArrayClass
    if UseSubArray(refRaidControllerObjName) == true then
        diskArrayClass = "DiskSubArray"
    else 
        diskArrayClass = "DiskArray"
    end

    local spanIndex = 0
    for i = 1, #redArray, 1 do
        spansArray = C[diskArrayClass]():fold(function(arrayObj, array)
            local arrayRefRaidCTRObjName = arrayObj.RefRAIDController:fetch()
            local arrayID = arrayObj.ArrayID:fetch()
            if refRaidControllerObjName == arrayRefRaidCTRObjName and redArray[i] == arrayID then
                array[#array + 1] = {
                    ArrayID = bit.band(arrayID, 0x7FFF),    --确保bit15为0
                    SpanName = 'Span' .. spanIndex,
                    UsedSpaceMiB = arrayObj.UsedSpace:fetch(),
                    FreeSpaceMiB = arrayObj.TotalFreeSpace:fetch(),
                    FreeBlocksSpaceMiB = GetFreeBlocksSpaceMiB(arrayObj, 1)
                }
                spanIndex = spanIndex + 1
            end
            return array
        end, spansArray):fetch()
    end
    return spansArray
end

local function GetRelatedLDs(volumeObj)
    local result = {}
    local AssociatedLDs = volumeObj.AssociatedLDs:fetch_or({})

    for i = 1, #AssociatedLDs, 1 do
        if AssociatedLDs[i] ~= STOTAGE_INFO_INVALID.STORAGE_INFO_INVALID_WORD then
            result[#result + 1] = 'Logical Drive ' .. AssociatedLDs[i]
        end
    end

    return result
end

local function ProtocolStrFormat(str)
    if str == 'parallel SCSI' then
        return 'iSCSI'
    elseif str == 'Fiber Channel' then
        return 'FCoE'
    else
        return str
    end
end

local function TrimStr(str)
    local strBuf = str:match '^%s*(.-)%s*$'
    return strBuf
end

local function CapableSpeedGbsFormat(speedgbs)
    local speedgbsArray = {}
    speedgbsArray[1] = 1.5
    speedgbsArray[2] = 3
    speedgbsArray[3] = 6
    speedgbsArray[4] = 12
    speedgbsArray[5] = 2.5
    speedgbsArray[6] = 5
    speedgbsArray[7] = 8
    speedgbsArray[8] = 10
    speedgbsArray[9] = 16
    speedgbsArray[10] = 20
    speedgbsArray[11] = 30
    speedgbsArray[12] = 32
    speedgbsArray[13] = 40
    speedgbsArray[14] = 64
    speedgbsArray[15] = 80
    speedgbsArray[16] = 96
    speedgbsArray[17] = 128
    speedgbsArray[18] = 160
    speedgbsArray[19] = 256
    speedgbsArray[20] = 22.5
    return speedgbsArray[speedgbs]
end

local function GetPciefunctionAssociatedresource(obj)
    if obj.InterfaceType:fetch() ~= 6 then -- PD_INTERFACE_TYPE_PCIE
        return null
    end

    local resource_num = obj.ResId:fetch()
    if resource_num == 0 then
        logging:error('resource ID is invalid')
        return null
    end
    if resource_num == RESOURCE_ID_PCIE_SWTICH then
        return 'PCIeSwitch'
    elseif resource_num == RESOURCE_ID_PCH then
        return 'PCH'
    elseif resource_num == RESOURCE_ID_CPU1_AND_CPU2 then
        return 'CPU1,CPU2'
    elseif resource_num > RESOURCE_ID_PCIE_DEVICE_BASE and resource_num <= RESOURCE_ID_PCIE_DEVICE_MAX then
        return C.CLASS_COMPONENT('FruId', resource_num - RESOURCE_ID_PCIE_DEVICE_BASE).DeviceName:fetch()
    else
        return 'CPU' .. resource_num
    end
end

local function GetArrayId(controllerObj, data)
    local slot
    local enclosure
    local arrayId = STOTAGE_INFO_INVALID.STORAGE_INFO_INVALID_WORD
    local raidObjName = object_name(controllerObj)
    local diskId = data.Drives[1]
    local objHandle = C.Hdd("Id", diskId)

    if objHandle.Presence:fetch_or(0) == 1 then
        slot = objHandle.SlotNumber:fetch_or()
        enclosure = objHandle.EnclosureDeviceId:fetch_or()
    end

    C.DiskArray():fold(function(obj)
        if raidObjName ~= obj.RefRAIDController:fetch_or() then
            return STOTAGE_INFO_INVALID.STORAGE_INFO_INVALID_WORD
        end

        local pdEnclosuresArray = obj.RefPDEnclosures:fetch_or({})
        local pdsArray = obj.RefPDSlots:fetch_or({})
        for i = 1, #pdEnclosuresArray, 1 do
            if pdEnclosuresArray[i] == enclosure and pdsArray[i] == slot then
                arrayId = obj.ArrayID:fetch_or()
            end
        end
    end):fetch_or()
    return arrayId
end

local function GetSASAddress(obj)
    local address
    local addressArray = {}
    address = obj.SASAddress1:next(CheckStrEffective):fetch()
    if address ~= null then
        addressArray[#addressArray + 1] = address
    end

    address = obj.SASAddress2:next(CheckStrEffective):fetch()
    if address ~= null then
        addressArray[#addressArray + 1] = address
    end
    return addressArray
end

local function PowerStateToStr(state)
    if state == RFPROP_DRIVE_POWER_STATE.RFPROP_DRIVE_POWER_STATE_V_SPUNUP_NUM then
        return RFPROP_DRIVE_POWER_STATE_V_SPUNUP
    elseif state == RFPROP_DRIVE_POWER_STATE.RFPROP_DRIVE_POWER_STATE_V_SPUNDOWN_NUM then
        return RFPROP_DRIVE_POWER_STATE_V_SPUNDOWN
    elseif state == RFPROP_DRIVE_POWER_STATE.RFPROP_DRIVE_POWER_STATE_V_TRANSITION_NUM then
        return RFPROP_DRIVE_POWER_STATE_V_TRANSITION
    else
        logging:error('get PowerState unknown.')
        return null
    end
end

local function HotspareTypeToStr(type)
    if type == SML_PHYSICAL_DRIVE_HOT_SPARE_E.PD_HOT_SPARE_NONE then
        return RF_HOTSPARETYPE.RF_HOTSPARETYPE_NONE
    elseif type == SML_PHYSICAL_DRIVE_HOT_SPARE_E.PD_HOT_SPARE_GLOBAL then
        return RF_HOTSPARETYPE.RF_HOTSPARETYPE_GLOBAL
    elseif type == SML_PHYSICAL_DRIVE_HOT_SPARE_E.PD_HOT_SPARE_DEDICATED then
        return RF_HOTSPARETYPE.RF_HOTSPARETYPE_DEDICATED
    elseif type == SML_PHYSICAL_DRIVE_HOT_SPARE_E.PD_HOT_SPARE_AUTO_REPLACE then
        return RF_HOTSPARETYPE.RF_HOTSPARETYPE_AUTO_REPLACE
    else
        logging:error('hotsparetype value is error.')
        return null
    end
end

local function RebuildStateToStr(state)
    if state == 0 then
        return 'DoneOrNotRebuilt'
    elseif state == 1 then
        return 'Rebuilding'
    else
        logging:error('the value of rebuildstate is error.')
        return null
    end
end

local function PatrolStateToStr(state)
    if state == 0 then
        return SML_PD_PATROL_STATE.SML_PD_PATROL_STATE_OFF
    elseif state == 1 then
        return SML_PD_PATROL_STATE.SML_PD_PATROL_STATE_ON
    else
        logging:error('the value of patrolstate is error.')
        return null
    end
end

local function PatrolStateToNum(str)
    if str == SML_PD_PATROL_STATE.SML_PD_PATROL_STATE_OFF then
        return 0
    elseif str == SML_PD_PATROL_STATE.SML_PD_PATROL_STATE_ON then
        return 1
    else
        logging:error('the value of patrolstate is error.')
        return null
    end
end

local function GetIndicatorLED(obj)
    local indicatorled = obj.Location:fetch()
    local fault = obj.Fault:fetch()
    if (indicatorled == DRIVE_INDICATORLED_OFF) or
        ((indicatorled == DRIVE_INDICATORLED_BLINKING) and (fault == DRIVE_FAULT_YES)) then
        return RF_INDICATORLED_OFF
    elseif (indicatorled == DRIVE_INDICATORLED_BLINKING) and (fault == DRIVE_FAULT_OK) then
        return RF_INDICATORLED_BLINKING
    else
        logging:error('indicatorled value error.')
        return null
    end
end

local M = {}

function M.GetViewsSummary()
    logging:info(' GetViewsSummary ')
    local drivesArray = {}
    drivesArray = GetVSPhysicalDrives()

    if drivesArray and #drivesArray == 0 then
        drivesArray = nil
    end

    return {
        Drives = drivesArray,
        BmaEnabled = (C.SMS[0] and C.SMS[0].SmsEnabled:fetch_or()) or 0,
        RaidControllers = GetVSRaidControllers()
    }
end

local function getRaidControllerState(value)
    if value == 0 then
        return false
    elseif value == 1 then
        return true
    else
        return null
    end
end

local function GetSpareActivationMode(Mode)
    if Mode == 0 then
        return 'Failure'
    elseif Mode == 1 then
        return 'Predictive'
    else
        logging:info('the value of SpareActivationMode is error.')
        return null
    end
end

local function ConsisCheckRateToStr(rate)
    if rate == CONSIS_CHECK_PARAM.CC_RATE_LOW_N then
        return CONSIS_CHECK_PARAM.CC_RATE_LOW_S
    elseif rate == CONSIS_CHECK_PARAM.CC_RATE_MIDDLE_N then
        return CONSIS_CHECK_PARAM.CC_RATE_MIDDLE_S
    elseif rate == CONSIS_CHECK_PARAM.CC_RATE_HIGH_N then
        return CONSIS_CHECK_PARAM.CC_RATE_HIGH_S
    else
        return null
    end
end

local function GetCCInfo(controllerObj)
    local result = {}
    result['Enabled'] = controllerObj.LDCCEnabled:next(getRaidControllerState):fetch()
    result['Period'] = controllerObj.LDCCPeriod:fetch()
    result['Rate'] = controllerObj.LDCCRate:next(ConsisCheckRateToStr):fetch()
    result['AutoRepairEnabled'] = controllerObj.LDCCAutoRepairEnabled:next(getRaidControllerState):fetch()
    result['RunningStatus'] = controllerObj.LDCCStatus:next(getRaidControllerState):fetch()
    result['TotalVolumeCount'] = controllerObj.LDCCTotalVD:neq(STOTAGE_INFO_INVALID.STORAGE_INFO_INVALID_BYTE):fetch_or(null)
    result['CompletedVolumeCount'] = controllerObj.LDCCCompletedVD:neq(STOTAGE_INFO_INVALID.STORAGE_INFO_INVALID_BYTE):fetch_or(null)
    result['DelayForStart'] = controllerObj.LDCCDelay:neq(-1):neq(STOTAGE_INFO_INVALID.STORAGE_INFO_INVALID_DWORD):fetch_or(0)
    return result
end

function M.GetRaidController(idStr, _)
    logging:info(' GetRaidController idStr:%s', idStr)
    local controllerObj

    if #idStr > 32 then
        return reply_bad_request('InvalidIndex')
    end

    local id = tonumber(idStr:match('RAIDStorage(%d+)$'))
    if not id then
        logging:error('check raid controlelr uri fail.')
        return reply_bad_request('InvalidIndex')
    end
    C.RaidController():fold(function(obj)
        if obj.Id:fetch() == id then
            controllerObj = obj
            return obj, false
        end
    end)
    if controllerObj == nil then
        logging:error('check raid controlelr uri fail.')
        return reply_bad_request('InvalidIndex')
    end

    local CtrlOption = controllerObj.CtrlOption1:fetch_or(0)
    local CtrlOption2 = controllerObj.CtrlOption2:fetch_or(0)
    local CtrlOption3 = controllerObj.CtrlOption3:fetch_or(0)
    if C.StorageDiagnose[0].RaidOOBEnable:next(getRaidControllerState):fetch() == true then
        OOBSupport = controllerObj.OOBSupport:next(getRaidControllerState):fetch()
    else
        OOBSupport = C.StorageDiagnose[0].RaidOOBEnable:next(getRaidControllerState):fetch()
    end
    return {
        ID = idStr,
        Name = controllerObj.Name:fetch(),
        Manufacturer = controllerObj.Manufacturer:fetch(),
        FirmwareVersion = controllerObj.FirmwareVersion:next(CheckStrEffective):fetch(),
        OOBSupport = OOBSupport,
        Health = controllerObj.HealthStatusCode:next(HealthStatusCodeToStr):fetch(),
        FaultDetails = GetFaultDetails(controllerObj.HealthStatusCode:fetch_or(0)),
        SupportedRAIDTypes = controllerObj.CtrlOption3:next(SupportedRAIDTypesToStr):fetch(),
        SupportedModes = controllerObj.CtrlOption2:next(SupportedModesToStr):fetch(),
        Mode = controllerObj.Mode:next(RaidCtrlModeToStr):fetch(),
        Type = controllerObj.Type:next(CheckStrEffective):fetch(),
        VolumeSupported = controllerObj.TypeID:next(TypeIDToBoolean):fetch(),
        ConfigurationVersion = controllerObj.NVDataVersion:next(CheckStrEffective):fetch(),
        SpareActivationMode = controllerObj.SpareActivationMode:next(GetSpareActivationMode):fetch(),
        NoBatteryWriteCacheEnabled = controllerObj.NoBatteryWriteCache:next(getRaidControllerState):fetch(),
        ReadCachePercent = controllerObj.ReadCachePercent:next(function(percent)
            if percent == DRIVE_SMART_INVALID_U8_VALUE then
                return null
            else
                return percent
            end
        end):fetch(),
        PCIeLinkWidth = controllerObj.PCIeLinkWidth:next(function(width)
            if width == DRIVE_SMART_INVALID_U8_VALUE then
                return null
            else
                return 'x' .. width
            end
        end):fetch(),
        MemorySizeMiB = controllerObj.MemorySize:next(function(size)
            if size == DRIVE_SMART_INVALID_U16_VALUE then
                return null
            else
                return size
            end
        end):fetch(),
        SupportedDeviceProtocols = controllerObj.DeviceInterfaceString:next(CheckStrEffective):next(
            StrToArrayBySpace):fetch(),
        SpeedGbps = controllerObj.DeviceInterface:next(SpeedgbpsFormat):fetch(),
        SASAddress = controllerObj.SASAddr:next(CheckStrEffective):fetch(),
        MaxStripeSizeBytes = controllerObj.MaxStripSize:next(StripsizeFormat):fetch(),
        MinStripeSizeBytes = controllerObj.MinStripSize:next(StripsizeFormat):fetch(),
        CachePinnedState = controllerObj.CachePinned:next(getRaidControllerState):fetch(),
        MaintainPDFailHistory = controllerObj.MaintPDFailHistory:next(getRaidControllerState):fetch(),
        BBU = GetBBU(controllerObj),
        Drives = GetControllerDrives(controllerObj),
        DriverInfo = {
            DriverName = controllerObj.DriverName:next(CheckStrEffective):fetch(),
            DriverVersion = controllerObj.DriverVersion:next(CheckStrEffective):fetch()
        },
        CopyBackState = controllerObj.CopybackEnabled:next(getRaidControllerState):fetch(),
        SmarterCopyBackState = controllerObj.SMARTerCopybackEnabled:next(getRaidControllerState):fetch(),
        JBODState = controllerObj.JBODEnabled:next(getRaidControllerState):fetch(),
        ConsisCheckInfo = GetCCInfo(controllerObj),
        WritePolicy = ParseStoragePolicy(CtrlOption, SUPORTED_POLICE_E.LD_WRITE_THROUGH,
            SUPORTED_POLICE_E.LD_WRITE_BACKBBU, SUPORTED_POLICE_E.LD_WRITE_BACK, BYTE_START_BIT.SECOND_BYTE_START_BIT),
        ReadPolicy = ParseStoragePolicy(CtrlOption, SUPORTED_POLICE_E.LD_READ_NO, SUPORTED_POLICE_E.LD_READ_AHEAD,
            nil, BYTE_START_BIT.FIRST_BYTE_START_BIT),
        CachePolicy = ParseStoragePolicy(CtrlOption, SUPORTED_POLICE_E.LD_IO_CACHE, SUPORTED_POLICE_E.LC_IO_DIRECT,
            nil, BYTE_START_BIT.THIRD_BYTE_START_BIT),
        AccessPolicy = ParseStoragePolicy(CtrlOption, SUPORTED_POLICE_E.LD_ACC_RW, SUPORTED_POLICE_E.LD_ACC_READ,
            SUPORTED_POLICE_E.LD_ACC_BLOCK, BYTE_START_BIT.FOURTH_BYTE_START_BIT),
        DriveCachePolicy = ParseStoragePolicy(CtrlOption2, SUPORTED_POLICE_E.LD_DCACHE_DEF,
            SUPORTED_POLICE_E.LD_DCACHE_ENABLE, SUPORTED_POLICE_E.LD_DCACHE_DISABLE, BYTE_START_BIT.FIRST_BYTE_START_BIT),
        BootDevices = GetCtrlBootDevices(controllerObj),
        WriteCachePolicy = GetCtrlDriveWriteCachePolicy(CtrlOption3)
    }
end

function M.GetLogicalDrive(controllerId, volumeId, _)
    logging:info(' GetLogicalDrive controllerId:%s volumeId:%s', controllerId, volumeId)

    if #controllerId > 32 then
        return reply_bad_request('Invalid controllerId')
    end

    if #volumeId > 32 then
        return reply_bad_request('Invalid volumeId')
    end

    local volumeObj = C.LogicalDrive():fold(function(volumeObj)
        if volumeObj.RaidLevel:fetch() == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_DELETED then
            return
        end
        local refRaidControllerObjName = volumeObj.RefRAIDController:fetch()
        local refRaidControllerObj = O[refRaidControllerObjName]
        local refRaidControllerId = 'RAIDStorage' .. refRaidControllerObj.Id:fetch()
        local volumeObjId = 'LogicalDrive' .. volumeObj.TargetID:fetch()
        if controllerId == refRaidControllerId and volumeId == volumeObjId then
            return volumeObj, false
        end
    end):fetch_or()

    if volumeObj == nil then
        return reply_bad_request('Invalid volumeId')
    end

    return {
        ID = volumeId,
        Name = volumeObj.DriveName:fetch(),
        State = volumeObj.DriveStatus:next(VolumeStateToStr):fetch(),
        VolumeRaidLevel = volumeObj.RaidLevel:next(RaidLevelToStr):fetch(),
        CapacityBytes = volumeObj.Size:next(CapacityMBToBytes):fetch(),
        OptimumIOSizeBytes = volumeObj.StripSize:next(StripsizeFormat):fetch(),
        CurrentReadPolicy = volumeObj.CurrentReadPolicy:next(ReadPolicyToStr):fetch(),
        CurrentWritePolicy = volumeObj.CurrentWritePolicy:next(WritePolicyToStr):fetch(),
        CurrentCachePolicy = volumeObj.CurrentCachePolicy:next(CachePolicyToStr):fetch(),
        DefaultReadPolicy = volumeObj.DefaultReadPolicy:next(ReadPolicyToStr):fetch(),
        DefaultWritePolicy = volumeObj.DefaultWritePolicy:next(WritePolicyToStr):fetch(),
        DefaultCachePolicy = volumeObj.DefaultCachePolicy:next(CachePolicyToStr):fetch(),
        AccessPolicy = volumeObj.AccessPolicy:next(AccessPolicyToStr):fetch(),
        BGIEnabled = volumeObj.BGIEnabled:next(IntToBool):fetch(),
        DriveCachePolicy = volumeObj.DiskCachePolicy:next(DriveCachePolicyToStr):fetch(),
        InitializationMode = volumeObj.InitState:next(InitModeToStr):fetch(),
        FGIProgress = volumeObj.FGIProgress:fetch(),
        BootEnabled = volumeObj.IsBootable:next(BootPriorityToBool):fetch(),
        BootPriority = volumeObj.BootPriority:next(BootPriorityToStr):fetch(),
        SSDCachecadeVolume = volumeObj.IsSSCD:next(IntToBool):fetch(),
        SSDCachingEnabled = volumeObj.CacheCadeLD:neq(0xff):next(IntToBool):fetch_or(null),
        ConsistencyCheck = volumeObj.ConsistencyCheck:next(IntToBool):fetch(),
        OSDriveName = volumeObj.OSDriveName:next(CheckStrEffective):fetch(),
        Spans = GetVolumeSpans(volumeObj),
        AccelerationMethod = volumeObj.AccelerationMethod:next(AccelerationMethodToStr):fetch(),
        CacheLineSize = volumeObj.CacheLineSize:next(CacheLineSizeToStr):fetch(),
        AssociatedVolumes = GetRelatedLDs(volumeObj),
        RebuildState = volumeObj.RebuildState:next(RebuildStateToStr):fetch_or(),
        RebuildProgress = volumeObj.RebuildState:eq(DRIVE_REBUILTING_VALUE):fetch_or() and
        volumeObj.RebuildProgress:neq(0xff):next(
            function(progress)
                return progress .. '%'
            end):fetch_or(''),
        MaxResizableBytes = volumeObj.MaxResizableSize:next(CapacityMBToBytes):fetch(),
    }
end

function M.GetPhysicalDrives(id, _)
    logging:info(' GetPhysicalDrives id:%s', id)

    if #id > 128 then
        return reply_bad_request('Invalid id')
    end

    local driveObj = C.Hdd():fold(function(obj)
        if obj.Presence:fetch() ~= 1 then
            return
        end
        local drivesID = string.gsub(obj.PhysicalLocation:fetch(), '%s+', '') ..
                             string.gsub(obj.DeviceName:fetch(), '%s+', '')
        if id == drivesID then
            return obj, false
        end
    end):fetch_or()

    if not driveObj then
        driveObj = C.PCIeDisk():fold(function(obj)
            if obj.Presence:fetch() ~= 1 then
                return
            end
            local drivesID = string.gsub(obj.PhysicalLocation:fetch(), '%s+', '') ..
                                 string.gsub(obj.DeviceName:fetch(), '%s+', '')
            if id == drivesID then
                return obj, false
            end
        end):fetch_or()

        if not driveObj then
            return null
        end
    end

    local hddNvmeTemp = driveObj.Temperature:neq(0xff):fetch_or(null)
         if hddNvmeTemp ~= null and hddNvmeTemp > 0x7F then
            hddNvmeTemp = -(bit.band(bit.bnot(hddNvmeTemp), 0xFF) + 1)
         end

    return {
        ID = id,
        Name = driveObj.DeviceName:next(CheckStrEffective):fetch_or(),
        Protocol = driveObj.InterfaceTypeString:next(CheckStrEffective):next(ProtocolStrFormat):fetch_or(),
        Health = driveObj.HddHealth:next(HealthToStr):fetch_or(),
        Manufacturer = driveObj.Manufacturer:next(TrimStr):next(CheckStrEffective):fetch_or(),
        Model = driveObj.ModelNumber:next(TrimStr):next(CheckStrEffective):fetch_or(),
        SerialNumber = driveObj.SerialNumber:next(TrimStr):next(CheckStrEffective):fetch_or(),
        Revision = driveObj.FirmwareVersion:next(TrimStr):next(CheckStrEffective):fetch_or(),
        MediaType = driveObj.MediaType:next(MediaTypeStr):fetch_or(),
        TemperatureCelsius = hddNvmeTemp,
        FirmwareStatus = driveObj.FirmwareStatus:next(FirmwareStatusToStr):fetch_or(),
        IsEPD = driveObj.FirmwareStatus:next(checkIfEPD):fetch_or(),
        SASAddress = GetSASAddress(driveObj),
        CapacityBytes = driveObj.CapacityMB:next(CapacityMBToBytes):fetch_or(),
        CapableSpeedGbs = driveObj.InterfaceSpeed:next(CapableSpeedGbsFormat):fetch_or(null),
        PowerState = driveObj.PowerState:next(PowerStateToStr):fetch_or(),
        HoursOfPoweredUp = driveObj.HoursOfPoweredUp:next(
            function(value)
                if value >= DRIVE_SMART_INVALID_U16_VALUE then
                    return null
                else
                    return value
                end
            end):fetch_or(),
        RotationSpeedRPM = driveObj.RotationSpeed:neq(STOTAGE_INFO_INVALID.STORAGE_INFO_INVALID_WORD):fetch_or(null), 
        HotspareType = driveObj.HotSpare:next(HotspareTypeToStr):fetch_or(),
        RebuildState = driveObj.RebuildState:next(RebuildStateToStr):fetch_or(),
        PatrolState = driveObj.PatrolState:next(PatrolStateToStr):fetch_or(),
        IndicatorLED = GetIndicatorLED(driveObj),
        NegotiatedSpeedGbs = driveObj.LinkSpeed:next(CapableSpeedGbsFormat):fetch_or(null),
        PredictedMediaLifeLeftPercent = driveObj.RemnantMediaWearout:neq(0xff):fetch_or(null),
        AssociatedResource = GetPciefunctionAssociatedresource(driveObj),
        RebuildProgress = driveObj.RebuildState:eq(DRIVE_REBUILTING_VALUE):fetch_or() and
            driveObj.RebuildProgress:neq(0xff):next(
                function(progress)
                    return progress .. '%'
                end):fetch_or(''),
        BootEnabled = driveObj.BootPriority:next(BootPriorityToBool):fetch_or(),
        BootPriority = driveObj.BootPriority:next(BootPriorityToStr):fetch_or()
    }
end

local function SetDriveWriteCachePolicy(user, controllerObj, errMap, data)

    if data.ConfiguredDrive then
        local policy = DriveWriteCachePolicyToNum(data.ConfiguredDrive)
        local ok, ret = call_method(user, controllerObj, 'SetDriveWriteCachePolicy', errMap, gbyte(0), gbyte(policy))
        if not ok then
            return ret
        end
    end
    if data.UnconfiguredDrive then
        local policy = DriveWriteCachePolicyToNum(data.UnconfiguredDrive)
        local ok, ret = call_method(user, controllerObj, 'SetDriveWriteCachePolicy', errMap, gbyte(1), gbyte(policy))
        if not ok then
            return ret
        end
    end
    if data.HBADrive then
        local policy = DriveWriteCachePolicyToNum(data.HBADrive)
        local ok, ret = call_method(user, controllerObj, 'SetDriveWriteCachePolicy', errMap, gbyte(2), gbyte(policy))
        if not ok then
            return ret
        end
    end
end

local function SetConsisCheck(user, controllerObj, errMap, data)
    local info = {
        enable = 0x00,
        period = 0x0000,
        rate = 0x01,
        repair = 0x00,
        delay = 0x00000000,
        mask = 0x00
    }

    if data.Enabled ~= nil then
        info.enable = BoolToInt(data.Enabled)
    else
        info.enable = CONSIS_CHECK_PARAM.CC_ENABLED_SET_N
    end

    if data.Period ~= nil then
        if (data.Period >= CONSIS_CHECK_PARAM.CC_PERIOD_MIN) and (data.Period <= CONSIS_CHECK_PARAM.CC_PERIOD_MAX) then
            info.period = data.Period
            info.mask = bit.bor(info.mask, CONSIS_CHECK_PARAM.CC_MASK_PERIOD)
        end
    else
        info.period = CONSIS_CHECK_PARAM.CC_PERIOD_MIN
    end

    if data.Rate ~= nil then
        if data.Rate == CONSIS_CHECK_PARAM.CC_RATE_LOW_S then
            info.rate = CONSIS_CHECK_PARAM.CC_RATE_LOW_N
            info.mask = bit.bor(info.mask, CONSIS_CHECK_PARAM.CC_MASK_RATE)
        elseif data.Rate == CONSIS_CHECK_PARAM.CC_RATE_MIDDLE_S then
            info.rate = CONSIS_CHECK_PARAM.CC_RATE_MIDDLE_N
            info.mask = bit.bor(info.mask, CONSIS_CHECK_PARAM.CC_MASK_RATE)
        elseif data.Rate == CONSIS_CHECK_PARAM.CC_RATE_HIGH_S then
            info.rate = CONSIS_CHECK_PARAM.CC_RATE_HIGH_N
            info.mask = bit.bor(info.mask, CONSIS_CHECK_PARAM.CC_MASK_RATE)
        end
    else
        info.rate = CONSIS_CHECK_PARAM.CC_RATE_LOW_N
    end

    if data.AutoRepairEnabled ~= nil then
        info.repair = BoolToInt(data.AutoRepairEnabled)
        info.mask = bit.bor(info.mask, CONSIS_CHECK_PARAM.CC_MASK_REPAIR)
    else
        info.repair = CONSIS_CHECK_PARAM.CC_DISABLED_N
    end

    if data.DelayForStart ~= nil then
        if (data.Enabled ~= nil) and (data.Enabled == true) then
            if (data.DelayForStart >= 0) and (data.DelayForStart <= CONSIS_CHECK_PARAM.CC_DELAY_MAX) then
                info.delay = data.DelayForStart
                info.mask = bit.bor(info.mask, CONSIS_CHECK_PARAM.CC_MASK_DELAY)
            end
        end
    else
        info.delay = 0
    end
    
    return call_method(user, controllerObj, 'SetConsisCheck', errMap, gbyte(info.enable),
        guint16(info.period), gbyte(info.rate), gbyte(info.repair), guint32(info.delay), gbyte(info.mask))
end

local function SetBootDevices(user, controllerObj, errMap, driveId)
    logging:info(' SetBootDevices')

    local bootDevices = {}

    for i = 1, #driveId do
        logging:info('driveId[%d]:%s', i, driveId[i])
        if driveId[i] == 'None' then
            bootDevices[i] = driveId[i]
        end

        if string.find(driveId[i], 'Logical') then
            --逻辑盘
            bootDevices[i] = driveId[i]
        else
            --物理盘
            C.Hdd():fold(function(obj)
                if obj.Presence:fetch() ~= 1 then
                    return
                end
                local objDriveId = obj.DeviceName:fetch()
                if driveId[i] == objDriveId then
                    bootDevices[i] = obj.DeviceName:fetch()
                    return obj, false
                end
            end):fetch_or()
        end
    end
    
    return call_method(user, controllerObj, 'SetBootDevices', errMap, gvstring(bootDevices))
end

function M.SetRaidController(controllerId, user, data)
    logging:info(' SetRaidController controllerId:%s', controllerId)
    if #controllerId > 32 then
        return reply_bad_request('Invalid id')
    end
    local controllerObj
    local id = tonumber(controllerId:match('RAIDStorage(%d+)$'))
    if not id then
        logging:error('check raid controlelr uri fail.')
        return reply_bad_request('InvalidIndex')
    end
    C.RaidController():fold(function(obj)
        if obj.Id:fetch() == id then
            controllerObj = obj
            return obj, false
        end
    end)
    if controllerObj == nil then
        logging:error('check raid controlelr uri fail.')
        return reply_bad_request('InvalidIndex')
    end

    local result = reply_ok()
    result:join(safe_call(function()
        if data.Mode then
            local profileId = 0xff
            local mode = RaidCtrlModeToNum(data.Mode)
            local ok, ret = call_method(user, controllerObj, 'SetMode', errCtlMap, gbyte(mode), gbyte(profileId))
            if not ok then
                return ret
            end
        end

        if data.CopyBackState ~= nil then
            local state = BoolToInt(data.CopyBackState)
            local ok, ret = call_method(user, controllerObj, 'SetCopyback', errCtlMap, gbyte(state))
            if not ok then
                return ret
            end
        end

        if data.SmarterCopyBackState ~= nil then
            local state = BoolToInt(data.SmarterCopyBackState)
            local ok, ret = call_method(user, controllerObj, 'SetSMARTerCopyback', errCtlMap, gbyte(state))
            if not ok then
                return ret
            end
        end

        if data.JBODState ~= nil then
            local state = BoolToInt(data.JBODState)
            local ok, ret = call_method(user, controllerObj, 'SetJBOD', errCtlMap, gbyte(state))
            if not ok then
                return ret
            end
        end

        if data.ConsisCheckInfo ~= nil then
            local ok, ret = SetConsisCheck(user, controllerObj, errCtlMap, data.ConsisCheckInfo)
            if not ok then
                return ret
            end
        end

        if data.NoBatteryWriteCacheEnabled ~= nil then
            local state = BoolToInt(data.NoBatteryWriteCacheEnabled)
            local ok, ret = call_method(user, controllerObj, 'SetNBWC', errCtlMap, gbyte(state))
            if not ok then
                return ret
            end
        end

        if data.ReadCachePercent ~= nil then
            local ok, ret = call_method(user, controllerObj, 'SetReadCachePercent', errCtlMap, gbyte(data.ReadCachePercent))
            if not ok then
                return ret
            end
        end

        if data.WriteCachePolicy ~= nil then
            local ok, ret = SetDriveWriteCachePolicy(user, controllerObj, errCtlMap, data.WriteCachePolicy)
            if not ok then
                return ret
            end
        end

        if data.BootDevices ~= nil then
            local ok, ret = SetBootDevices(user, controllerObj, errCtlMap, data.BootDevices)
            if not ok then
                return ret
            end
        end
    end))

    if result:isOk() then
        return reply_ok_encode_json(M.GetRaidController(controllerId, user))
    end
    result:appendErrorData(M.GetRaidController(controllerId, user))
    return result
end

function M.SetLogicalDrive(controllerId, volumeId, user, data)
    logging:info(' SetLogicalDrive controllerId:%s volumeId:%s', controllerId, volumeId)

    if #controllerId > 32 then
        return reply_bad_request('Invalid controllerId')
    end

    if #volumeId > 32 then
        return reply_bad_request('Invalid volumeId')
    end

    local volumeObj = C.LogicalDrive():fold(function(volumeObj)
        if volumeObj.RaidLevel:fetch() == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_DELETED then
            return
        end
        local refRaidControllerObjName = volumeObj.RefRAIDController:fetch()
        local refRaidControllerObj = O[refRaidControllerObjName]
        local refRaidControllerId = 'RAIDStorage' .. refRaidControllerObj.Id:fetch()
        local volumeObjId = 'LogicalDrive' .. volumeObj.TargetID:fetch()

        if controllerId == refRaidControllerId and volumeId == volumeObjId then
            return volumeObj, false
        end
    end):fetch_or()
    if volumeObj == nil then
        logging:error('check raid volume uri fail.')
        return reply_bad_request('InvalidIndex')
    end

    local result = reply_ok()

    result:join(safe_call(function()
        if data.VolumeRaidLevel then
            return reply_bad_request('PropertyNotWritable', 'VolumeRaidLevel')
        end

        if data.InitializationMode then
            return reply_bad_request('PropertyNotWritable', 'InitializationMode')
        end

        if data.Name then
            local ok, ret = call_method(user, volumeObj, 'SetName', errLDMap, gstring(data.Name))
            if not ok then
                return ret
            end
        end

        if data.DefaultReadPolicy then
            local policy = ReadPolicyToNum(data.DefaultReadPolicy)
            local ok, ret = call_method(user, volumeObj, 'SetReadPolicy', errLDMap, gbyte(policy))
            if not ok then
                return ret
            end
        end

        if data.DefaultWritePolicy then
            local policy = WritePolicyToNum(data.DefaultWritePolicy)
            local ok, ret = call_method(user, volumeObj, 'SetWritePolicy', errLDMap, gbyte(policy))
            if not ok then
                return ret
            end
        end

        if data.DefaultCachePolicy then
            local policy = CachePolicyToNum(data.DefaultCachePolicy)
            local ok, ret = call_method(user, volumeObj, 'SetIOPolicy', errLDMap, gbyte(policy))
            if not ok then
                return ret
            end
        end

        if data.AccessPolicy then
            local policy = AccessPolicyToNum(data.AccessPolicy)
            local ok, ret = call_method(user, volumeObj, 'SetAccessPolicy', lDAccMap, gbyte(policy))
            if not ok then
                return ret
            end
        end

        if data.DriveCachePolicy then
            local policy = DriveCachePolicyToNum(data.DriveCachePolicy)
            local ok, ret = call_method(user, volumeObj, 'SetDiskCachePolicy', errLDMap, gbyte(policy))
            if not ok then
                return ret
            end
        end

        if data.BGIEnabled ~= nil then
            local policy = BoolToInt(data.BGIEnabled)
            local ok, ret = call_method(user, volumeObj, 'SetBGI', errLDMap, gbyte(policy))
            if not ok then
                return ret
            end
        end

        if data.BootEnabled ~= nil then
            local boot = BoolToInt(data.BootEnabled)
            local ok, ret = call_method(user, volumeObj, 'SetBootable', errLDMap, gbyte(boot))
            if not ok then
                return ret
            end
        end

        if data.BootPriority ~= nil then
            local boot = BootPriorityToNum(data.BootPriority)
            local ok, ret = call_method(user, volumeObj, 'SetBootable', errLDMap, gbyte(boot))
            if not ok then
                return ret
            end
        end

        if data.SSDCachingEnabled ~= nil then
            local policy = BoolToInt(data.SSDCachingEnabled)
            local ok, ret = call_method(user, volumeObj, 'SetCachecade', errLDMap, gbyte(policy))
            if not ok then
                return ret
            end
        end
        
        if data.AccelerationMethod ~= nil then
            local method = AccelerationMethodToNum(data.AccelerationMethod)
            local ok, ret = call_method(user, volumeObj, 'SetAccelerationMethod', errLDMap, gbyte(method))
            if not ok then
                return ret
            end
        end

        if data.CapacityMBytes ~= nil then
            local ok, ret = call_method(user, volumeObj, 'SetCapacity', errLDMap, guint32(data.CapacityMBytes), gbyte(0))
            if not ok then
                return ret
            end
        end

        if data.OptimumIOSizeBytes ~= nil then
            local stripSize = StripsizeToNum(data.OptimumIOSizeBytes)
            local ok, ret = call_method(user, volumeObj, 'SetStripSize', errLDMap, gbyte(stripSize))
            if not ok then
                return ret
            end
        end
    end))

    if result:isOk() then
        return reply_ok_encode_json(M.GetLogicalDrive(controllerId, volumeId, user))
    end
    result:appendErrorData(M.GetLogicalDrive(controllerId, volumeId, user))
    return result
end

function M.SetPhysicalDrives(driveId, user, data)
    logging:info(' GetPhysicalDrives driveId:%s', driveId)

    if #driveId > 128 then
        return reply_bad_request('Invalid driveId')
    end

    local driveObj = C.Hdd():fold(function(obj)
        if obj.Presence:fetch() ~= 1 then
            return
        end
        local objDriveId = string.gsub(obj.PhysicalLocation:fetch(), '%s+', '') ..
                               string.gsub(obj.DeviceName:fetch(), '%s+', '')
        if driveId == objDriveId then
            return obj, false
        end
    end):fetch_or()
    if driveObj == nil then
        logging:error('check raid drive uri fail.')
        return reply_bad_request('InvalidIndex')
    end

    local result = reply_ok()
    result:join(safe_call(function()
        if data.IndicatorLED ~= nil then
            local inputList
            if data.IndicatorLED == RF_INDICATORLED_OFF then
                inputList = {gbyte(DRIVE_INDICATORLED_OFF), gbyte(0)}
            elseif data.IndicatorLED == RF_INDICATORLED_BLINKING then
                inputList = {gbyte(DRIVE_INDICATORLED_BLINKING), gbyte(0)}
            else
                logging:error('indicatorled value error.')
                return reply_bad_request('PropertyValueNotInList')
            end

            local ok, ret = call_method(user, driveObj, 'SetLocateState', errDriveMap, inputList)
            if not ok then
                return ret
            end
        end

        if data.FirmwareStatus ~= nil and data.SpareforLogicalDrive ~= nil then
            return reply_bad_request('PropertyConflict')
        end

        if data.FirmwareStatus ~= nil and data.HotspareType ~= nil then
            return reply_bad_request('PropertyConflict')
        end

        if data.HotspareType ~= nil then
            local inputList
            local logicalId = {}
            if data.HotspareType == RF_HOTSPARETYPE.RF_HOTSPARETYPE_NONE then
                if data.SpareforLogicalDrive ~= nil then
                    return reply_bad_request('PropertyConflict')
                end
                logicalId[1] = guint16(0)
                inputList = {gbyte(SML_PHYSICAL_DRIVE_HOT_SPARE_E.PD_HOT_SPARE_NONE), new_fixed_array(logicalId)}
            elseif data.HotspareType == RF_HOTSPARETYPE.RF_HOTSPARETYPE_GLOBAL then
                if data.SpareforLogicalDrive ~= nil then
                    return reply_bad_request('PropertyConflict')
                end
                logicalId[1] = guint16(0)
                inputList = {gbyte(SML_PHYSICAL_DRIVE_HOT_SPARE_E.PD_HOT_SPARE_GLOBAL), new_fixed_array(logicalId)}
            elseif data.HotspareType == RF_HOTSPARETYPE.RF_HOTSPARETYPE_DEDICATED and data.SpareforLogicalDrive ~= nil then
                local logicalIdCnt = #(data.SpareforLogicalDrive)
                if logicalIdCnt > 4 then
                    return reply_bad_request('PropertyValueFormatError')
                end
                for i = 1, logicalIdCnt, 1 do
                    logicalId[i] = guint16(tonumber(data.SpareforLogicalDrive[i]:match('(%d+)$')))
                end
                inputList = {gbyte(SML_PHYSICAL_DRIVE_HOT_SPARE_E.PD_HOT_SPARE_DEDICATED), new_fixed_array(logicalId)}
            elseif data.HotspareType == RF_HOTSPARETYPE.RF_HOTSPARETYPE_AUTO_REPLACE and data.SpareforLogicalDrive ~= nil then
                local logicalIdCnt = #(data.SpareforLogicalDrive)
                if logicalIdCnt ~= 1 then
                    return reply_bad_request('PropertyValueFormatError')
                end
                logicalId[1] = guint16(tonumber(data.SpareforLogicalDrive[1]:match('(%d+)$')))
                inputList = {gbyte(SML_PHYSICAL_DRIVE_HOT_SPARE_E.PD_HOT_SPARE_AUTO_REPLACE), new_fixed_array(logicalId)}
            else
                logging:error('hotspare value error.')
                return reply_bad_request('PropertyValueNotInList')
            end
            local ok, ret = call_method(user, driveObj, 'SetHotspare', errDriveMap, inputList)
            if not ok then
                return ret
            end
        end

        if data.FirmwareStatus ~= nil then
            local inputvalue = FirmwareStatusToNum(data.FirmwareStatus)
            if inputvalue == nil then
                return reply_bad_request('PropertyModificationNotSupported')
            end
            local ok, ret = call_method(user, driveObj, 'SetFirmwareState', errDriveMap, gbyte(inputvalue))
            if not ok then
                return ret
            end
        end

        if data.BootEnabled ~= nil then
            local boot = BoolToInt(data.BootEnabled)
            local ok, ret = call_method(user, driveObj, 'SetBootable', errDriveMap, gbyte(boot))
            if not ok then
                return ret
            end
        end

        if data.BootPriority ~= nil then
            local boot = BootPriorityToNum(data.BootPriority)
            local ok, ret = call_method(user, driveObj, 'SetBootable', errDriveMap, gbyte(boot))
            if not ok then
                return ret
            end
        end

        if data.PatrolState ~= nil then
            local patrol = PatrolStateToNum(data.PatrolState)
            local ok, ret = call_method(user, driveObj, 'SetPatrolState', errDriveMap, gbyte(patrol))
            if not ok then
                return ret
            end
        end
    end))

    if result:isOk() then
        return reply_ok_encode_json() -- to do IndicatorLED ret value
    end
    result:appendErrorData(M.GetPhysicalDrives(driveId, user))
    return result
end

function testControllerVendor(typeId, vendorId)
    if typeId < CONTROLLER_TYPE_ID_E.PMC_3152_8I_SMART_RAID and vendorId == CTRL_VENDER_INDEX.VENDER_LSI then
        return true
    elseif (typeId == CONTROLLER_TYPE_ID_E.PMC_3152_8I_SMART_RAID or
            typeId == CONTROLLER_TYPE_ID_E.PMC_2100_8I_SMART_HBA) and
            vendorId == CTRL_VENDER_INDEX.VENDER_PMC then
        return true
    elseif (typeId >= CONTROLLER_TYPE_ID_E.HI1880_SP186_M_16i and
            typeId <= CONTROLLER_TYPE_ID_E.HI1880_SP186_M_8i) and
            vendorId == CTRL_VENDER_INDEX.VENDER_HUAWEI then
        return true
    end

    return false
end

function GetCreateType(typeId, arrayId, cacheCadeFlag)
    if arrayId >= 0 and arrayId ~= STOTAGE_INFO_INVALID.STORAGE_INFO_INVALID_WORD then
        if cacheCadeFlag == true then
            if testControllerVendor(typeId, CTRL_VENDER_INDEX.VENDER_PMC) == true then
                return CREATE_LD_TYPE_E.CREATE_LD_AS_CACHECADE    -- arrayId为有效值且CacheCadeFlag为true且为PMC卡：创建CacheCade
            else
                return nil    -- arrayId为有效值且CacheCadeFlag为true且不为PMC卡：没这种场景
            end
        else
            return CREATE_LD_TYPE_E.ADD_LD_ON_EXIST_ARRAY    -- arrayId为有效值且CacheCadeFlag为false：在已有阵列上创建
        end
    else
        if cacheCadeFlag == true then
            return CREATE_LD_TYPE_E.CREATE_LD_AS_CACHECADE    -- arrayId为无效值且CacheCadeFlag为true：创建CacheCade
        else
            return CREATE_LD_TYPE_E.CREATE_LD_ON_NEW_ARRAY    -- arrayId为无效值且CacheCadeFlag为false：创建全新阵列
        end
    end
end

function GetRaidLevel(typeId, createType, VolumeRaidLevel)
    if testControllerVendor(typeId, CTRL_VENDER_INDEX.VENDER_LSI) == true and
        createType == CREATE_LD_TYPE_E.ADD_LD_ON_EXIST_ARRAY then
        return STOTAGE_INFO_INVALID.STORAGE_INFO_INVALID_BYTE -- 如果是博通卡且在已有阵列上创建的场景，不需要指定RAID级别
    end

    if VolumeRaidLevel == SML_RAID_LEVEL_E.SML_RAID_LEVEL_1ADM then
        return SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_1ADM
    elseif VolumeRaidLevel == SML_RAID_LEVEL_E.SML_RAID_LEVEL_10ADM then
        return SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_10ADM
    elseif VolumeRaidLevel == SML_RAID_LEVEL_E.SML_RAID_LEVEL_1TRIPLE then
        return SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_1TRIPLE
    elseif VolumeRaidLevel == SML_RAID_LEVEL_E.SML_RAID_LEVEL_10TRIPLE then
        return SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_10TRIPLE
    else
        return tonumber(VolumeRaidLevel:match('(%d+)$'))
    end
end

function CheckCacheCadeRaidLevel(typeId, raidLevel)
    if testControllerVendor(typeId, CTRL_VENDER_INDEX.VENDER_PMC) == true then
        if raidLevel ~= SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_0 and raidLevel ~= SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_1 and
           raidLevel ~= SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_5 and raidLevel ~= SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_10 then
            return false
        end
    else
        if raidLevel ~= SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_0 and raidLevel ~= SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_1 then
            return false
        end
    end

    return true
end

function GetSpanNumber(typeId, raidLevel, SpanNumber, DriveNum)

    if SpanNumber ~= nil then
        return SpanNumber -- 有值，直接返回
    end

    if raidLevel == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_0 or
       raidLevel == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_1 or
       raidLevel == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_5 or
       raidLevel == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_6 or
       raidLevel == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_1ADM or
       raidLevel == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_1TRIPLE then
        return 1;
    end

    if raidLevel == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_10 then
        -- 如果硬盘数量大于等于4，且能整除2(每个span最少的物理盘数是2)。span_num = 硬盘个数 / 2
        if DriveNum >= 4 and DriveNum % 2 == 0 then
            return DriveNum / 2
        else
            return 2
        end
    end

    if raidLevel == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_50 or
       raidLevel == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_10ADM or
       raidLevel == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_10TRIPLE then
        -- 如果硬盘数量大于等于6，且能整除3(每个span最少的物理盘数是3)。span_num = 硬盘个数 / 3
        if DriveNum >= 6 and DriveNum % 3 == 0 then
            return DriveNum / 3
        else
            return 2
        end
    end

    if raidLevel == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_60 then
        local minDiskNum
        local minDiskNumPerSpan
        if testControllerVendor(typeId, CTRL_VENDER_INDEX.VENDER_PMC) == true or
            typeId == CONTROLLER_TYPE_ID_E.LSI_3908_WITH_MR or
            typeId == CONTROLLER_TYPE_ID_E.LSI_3916_WITH_MR then
            minDiskNum = 8    -- PMC卡和3908、3916的RAID 6至少4盘
            minDiskNumPerSpan = 4
        else
            minDiskNum = 6    -- 博通卡的RAID 6至少3盘
            minDiskNumPerSpan = 3
        end

        if DriveNum >= minDiskNum and DriveNum % minDiskNumPerSpan == 0 then
            return DriveNum / minDiskNumPerSpan
        else
            return 2
        end
    end

    return 1
end

function GetName(Name)
    if Name == nil then
        return ''
    else
        return Name
    end
end

function GetCapacity(CapacityMBytes)
    if CapacityMBytes == nil then
        return -1 -- 实际传的是0xFFFFFFFF，按默认的最大值创建
    else
        return CapacityMBytes
    end
end

function GetBlockIndex(BlockIndex)
    if BlockIndex == nil then
        return STOTAGE_INFO_INVALID.STORAGE_INFO_INVALID_BYTE
    else
        return BlockIndex
    end
end

function GetAssociatedLD(lds)
    if lds == nil then
        return STOTAGE_INFO_INVALID.STORAGE_INFO_INVALID_WORD
    end

    return tonumber(lds[1]:match('LogicalDrive(%d+)$'))
end

function CheckLDNameTooLong(typeId, len)
    if testControllerVendor(typeId, CTRL_VENDER_INDEX.VENDER_LSI) and len > 15 then
        return true
    end
    if testControllerVendor(typeId, CTRL_VENDER_INDEX.VENDER_HUAWEI) and len > 20 then
        return true
    end
    if testControllerVendor(typeId, CTRL_VENDER_INDEX.VENDER_PMC) and len > 64 then
        return true
    end
    return false
end

function M.AddLogicalDrive(controllerId, user, data)
    logging:info(' AddLogicalDrive controllerId:%s', controllerId)
    local controllerObj
    local id = tonumber(controllerId:match('RAIDStorage(%d+)$'))
    if #controllerId > 32 or not id then
        logging:error('check raid volume uri fail.')
        return reply_bad_request('InvalidIndex')
    end
    C.RaidController():fold(function(obj)
        if obj.Id:fetch() == id then
            controllerObj = obj
            return obj, false
        end
    end)
    if controllerObj == nil then
        logging:error('check raid volume uri fail.')
        return reply_bad_request('InvalidIndex')
    end

    if data.Drives == nil then
        return reply_bad_request('PropertyMissing', 'Drives')
    end
    local typeId = controllerObj.TypeID:fetch_or()
    local inputList = {}
    local methodName
    local arrayId = GetArrayId(controllerObj, data)
    local createType = GetCreateType(typeId, arrayId, data.CreateCacheCadeFlag)
    if createType == nil then
        return reply_bad_request('UnnecessaryPropWhenCreateVol', 'CreateCacheCadeFlag')
    end

    if CheckLDNameTooLong(typeId, #data.Name) then
        return reply_bad_request('PropertyValueExceedsMaxLength')
    end

    local raidLevel = GetRaidLevel(typeId, createType, data.VolumeRaidLevel)
    local drivesArray = {}
    if createType == CREATE_LD_TYPE_E.CREATE_LD_ON_NEW_ARRAY or createType ==
        CREATE_LD_TYPE_E.ADD_LD_ON_EXIST_ARRAY then
        if createType == CREATE_LD_TYPE_E.CREATE_LD_ON_NEW_ARRAY then
            methodName = 'CreateCommonLD'
            for i, _ in ipairs(data.Drives) do
                drivesArray[i] = gbyte(data.Drives[i])
            end
            inputList[#inputList + 1] = new_fixed_array(drivesArray)
        else
            methodName = 'AddLD'
            inputList[#inputList + 1] = guint16(arrayId)
            inputList[#inputList + 1] = gbyte(GetBlockIndex(data.BlockIndex))
        end

        inputList[#inputList + 1] = gbyte(raidLevel)
        inputList[#inputList + 1] = gbyte(GetSpanNumber(typeId, raidLevel, data.SpanNumber, #data.Drives))
        inputList[#inputList + 1] = gstring(GetName(data.Name))
        inputList[#inputList + 1] = guint32(GetCapacity(data.CapacityMBytes))
        inputList[#inputList + 1] = gbyte(0)
        -- OptimumIOSizeBytes strip_size_as_para
        local stripSizeNum = StripsizeToNum(data.OptimumIOSizeBytes)
        if stripSizeNum == nil then
            return reply_bad_request('StripeSizeError')
        end
        inputList[#inputList + 1] = gbyte(stripSizeNum)
        inputList[#inputList + 1] = gbyte(ReadPolicyToNum(data.DefaultReadPolicy))
        inputList[#inputList + 1] = gbyte(WritePolicyToNum(data.DefaultWritePolicy))
        inputList[#inputList + 1] = gbyte(CachePolicyToNum(data.DefaultCachePolicy))
        inputList[#inputList + 1] = gbyte(AccessPolicyToNum(data.AccessPolicy))
        inputList[#inputList + 1] = gbyte(DriveCachePolicyToNum(data.DriveCachePolicy))
        inputList[#inputList + 1] = gbyte(InitModeToNum(data.InitializationMode))
        inputList[#inputList + 1] = gbyte(AccelerationMethodToNum(data.AccelerationMethod))
    elseif createType == CREATE_LD_TYPE_E.CREATE_LD_AS_CACHECADE then
        if CheckCacheCadeRaidLevel(typeId, raidLevel) == false then
            return reply_bad_request('PropertyValueNotInList', {'VolumeRaidLevel'})
        end
        methodName = 'CreateCachecadeLD'
        for i, _ in ipairs(data.Drives) do
            drivesArray[i] = gbyte(data.Drives[i])
        end
        inputList[#inputList + 1] = new_fixed_array(drivesArray)
        inputList[#inputList + 1] = gbyte(raidLevel)
        inputList[#inputList + 1] = gstring(GetName(data.Name))
        inputList[#inputList + 1] = gbyte(WritePolicyToNum(data.DefaultWritePolicy))
        inputList[#inputList + 1] = guint16(arrayId)
        inputList[#inputList + 1] = guint32(GetCapacity(data.CapacityMBytes))
        inputList[#inputList + 1] = gbyte(0)
        inputList[#inputList + 1] = guint16(GetAssociatedLD(data.AssociatedVolumes))
        inputList[#inputList + 1] = gbyte(CacheLineSizeToNum(data.CacheLineSize))
    end
    local ok, ret = call_method(user, controllerObj, methodName, errLDMap, inputList)
    if not ok then
        return ret
    end
    local volumeId = 'LogicalDrive' .. ret[1]

    logging:info(' AddLogicalDrive sucess, volumeId:%s', volumeId)
    return reply_ok()
end

function M.DeleteLogicalDrive(controllerId, volumeId, user)
    logging:info(' DeleteLogicalDrive controllerId:%s volumeId:%s', controllerId, volumeId)

    if #controllerId > 32 or #volumeId > 32 then
        logging:error('check raid volume uri fail.')
        return reply_bad_request('InvalidIndex')
    end

    local raidControllerObj
    local volumeNumId
    C.LogicalDrive():fold(function(volumeObj)
        if volumeObj.RaidLevel:fetch() == SML_LOGIC_DRIVE_RAID_LEVEL_E.RAID_LEVEL_DELETED then
            return
        end
        local refRaidControllerObjName = volumeObj.RefRAIDController:fetch()
        local refRaidControllerObj = O[refRaidControllerObjName]
        local refRaidControllerId = 'RAIDStorage' .. refRaidControllerObj.Id:fetch()
        local id = tonumber(volumeId:match('LogicalDrive(%d+)$'))
        if controllerId == refRaidControllerId and id == volumeObj.TargetID:fetch() then
            raidControllerObj = refRaidControllerObj
            volumeNumId = id
            return false, false
        end
    end):fetch_or()
    if raidControllerObj == nil then
        logging:error('check raid volume uri fail.')
        return reply_bad_request('InvalidIndex')
    end

    local ok, ret = call_method(user, raidControllerObj, 'DeleteLD', errLDMap, guint16(volumeNumId))
    if not ok then
        return ret
    end

    logging:info(' DeleteLogicalDrive sucess')
    return reply_ok()
end

function M.RestoreDefaultSettings(controllerId, user)
    logging:info(' RestoreDefaultSettings, controllerId:', controllerId)

    if #controllerId > 32 then
        logging:error('check raid volume uri fail.')
        return reply_bad_request('InvalidIndex')
    end

    local controllerObj
    local id = tonumber(controllerId:match('RAIDStorage(%d+)$'))
    if not id then
        logging:error('check raid controlelr uri fail.')
        return reply_bad_request('InvalidIndex')
    end
    C.RaidController():fold(function(obj)
        if obj.Id:fetch() == id then
            controllerObj = obj
            return obj, false
        end
    end)
    if controllerObj == nil then
        logging:error('check raid controlelr uri fail.')
        return reply_bad_request('InvalidIndex')
    end

    local ok, ret = call_method(user, controllerObj, 'RestoreSettings', restoreMap)
    if not ok then
        return ret
    end
    return reply_ok()
end

function M.ImportForeignConfig(controllerId, user)
    logging:info(' ImportForeignConfig, controllerId:', controllerId)

    if #controllerId > 32 then
        return reply_bad_request('Invalid controllerId')
    end
    local controllerObj
    local id = tonumber(controllerId:match('RAIDStorage(%d+)$'))
    if not id then
        logging:error('check raid controlelr uri fail.')
        return reply_bad_request('InvalidIndex')
    end
    C.RaidController():fold(function(obj)
        if obj.Id:fetch() == id then
            controllerObj = obj
            return obj, false
        end
    end)
    if controllerObj == nil then
        logging:error('check raid controlelr uri fail.')
        return reply_bad_request('InvalidIndex')
    end

    local ok, ret = call_method(user, controllerObj, 'ImportForeignConfig', restoreMap)
    if not ok then
        return ret
    end
    return reply_ok()
end

function M.ClearForeignConfig(controllerId, user)
    logging:info(' ClearForeignConfig, controllerId:', controllerId)

    if #controllerId > 32 then
        return reply_bad_request('Invalid controllerId')
    end
    local controllerObj
    local id = tonumber(controllerId:match('RAIDStorage(%d+)$'))
    if not id then
        logging:error('check raid controlelr uri fail.')
        return reply_bad_request('InvalidIndex')
    end
    C.RaidController():fold(function(obj)
        if obj.Id:fetch() == id then
            controllerObj = obj
            return obj, false
        end
    end)
    if controllerObj == nil then
        logging:error('check raid controlelr uri fail.')
        return reply_bad_request('InvalidIndex')
    end

    local ok, ret = call_method(user, controllerObj, 'ClearForeignConfig', restoreMap)
    if not ok then
        return ret
    end
    return reply_ok()
end

function M.PhysicalDrivesCryptoErase(driveId, user)
    logging:info(' PhysicalDrivesCryptoErase, controllerId:%s', driveId)

    if #driveId > 128 then
        logging:error('check raid volume uri fail.')
        return reply_bad_request('InvalidIndex')
    end

    local driveObj = C.Hdd():fold(function(obj)
        if obj.Presence:fetch() ~= 1 then
            return
        end
        local objDriveId = string.gsub(obj.PhysicalLocation:fetch(), '%s+', '') ..
                               string.gsub(obj.DeviceName:fetch(), '%s+', '')
        if driveId == objDriveId then
            return obj, false
        end
    end):fetch_or()
    if driveObj == nil then
        logging:error('check raid drive uri fail.')
        return reply_bad_request('InvalidIndex')
    end

    local ok, ret = call_method(user, driveObj, 'CryptoErase', cryptoEraseeMap)
    if not ok then
        return ret
    end
    return reply_ok()
end

function M.DumpControllerLog(controllerId, user)
    logging:info(' DumpControllerLog controllerId:%s', controllerId)
    local controllerObj
    local id = tonumber(controllerId:match('RAIDStorage(%d+)$'))
    if not id then
        logging:error('check raid controller uri fail.')
        return reply_bad_request('InvalidIndex')
    end
    C.RaidController():fold(function(obj)
        if obj.Id:fetch() == id then
            controllerObj = obj
            return obj, false
        end
    end)
    if controllerObj == nil then
        logging:error('check raid controller uri fail.')
        return reply_bad_request('InvalidIndex')
    end

    return controllerObj:next(function(obj)
        local collect_cb = function()
            local ok, ret = utils.call_method_async(user, obj, 'DumpLog', errCtlMap, nil)
            if not ok then
                return ret
            end
            return reply_ok()
        end

        local component_name = controllerObj.ComponentName:fetch();
        local tar_name = '/tmp/' .. string.gsub(component_name, " ", "_") .. '.tar.gz'

        local progress_cb = function(file_path)
            local ok, ret = call_method(user, obj, 'GetDumpPhase', nil, nil)
            if not ok then
                logging:error('progress_cb GetDumpPhase failed')
                return 0
            end
            if ret[1] == 100 then
                if not dal_set_file_owner(tar_name, cfg.REDFISH_USER_UID, cfg.APPS_USER_GID) then
                    logging:error('chown %s failed', tar_name)
                end
                if not dal_set_file_mode(tar_name, cfg.FILE_MODE_600) then
                    logging:error('chmod %s failed', tar_name)
                end
                tools.movefile(tar_name, file_path)
            end
            return ret[1]
        end

        return download(user, tar_name, 'RAID Controller ' .. id .. ' log', defs.DOWNLOAD_DUMP, collect_cb, progress_cb)
    end):catch(function(err)
        return http.reply_bad_request('ActionNotSupported', err)
    end):fetch()
end

return M
