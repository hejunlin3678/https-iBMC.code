/******************************************************************************

                  版权所有 (C), 2016-2021, 华为技术有限公司

 ******************************************************************************
  文 件 名   : sml_common.h
  版 本 号   : 初稿
  作    者   : gongxiongtao (gwx455466)
  生成日期   : 2017年6月14日
  最近修改   :
  功能描述   : 华为自研的存储设备管理库(Storage Management Library)的基本信息,
               主要包含业界标准或协议的定义
  函数列表   :
  修改历史   :
  1.日    期   : 2016年6月14日
    作    者   : gongxiongtao (gwx455466)
    修改内容   : DTS2017061205611  提取出sml的公共声明给redfish和agentless用。

******************************************************************************/
#ifndef __SML_COMMON_H__
#define __SML_COMMON_H__
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#pragma pack(1)

/*
 * SFF 8035i Revision 2
 * Bit 0 (pre-failure/advisory bit)
 * Bit 1 (on-line data collection)
 * Bit 2 (Performance bit)
 * Bit 3 (Errorrate bit)
 * Bit 4 (Eventcount bit)
 * Bit 5 (Selfpereserving bit)
 * Bit 2, 3, 4, 5 are vendor-specific bits:
 * https://www.linux-mips.org/pub/linux/mips/people/macro/S.M.A.R.T./SFF-8035i.pdf
 */
#define ATA_SMART_ATTRIBUTE_FLAGS_PREFAILURE(x)       (x & 0x01)
#define ATA_SMART_ATTRIBUTE_FLAGS_ONLINE(x)           (x & 0x02)
#define ATA_SMART_ATTRIBUTE_FLAGS_PERFORMANCE(x)      (x & 0x04)
#define ATA_SMART_ATTRIBUTE_FLAGS_ERRORRATE(x)        (x & 0x08)
#define ATA_SMART_ATTRIBUTE_FLAGS_EVENTCOUNT(x)       (x & 0x10)
#define ATA_SMART_ATTRIBUTE_FLAGS_SELFPRESERVING(x)   (x & 0x20)

/*
 * SMART Attribute ID
 * Table 1 - Legacy Attribute IDs
 * Table 2 - Attributes defined by T13
 * http://www.t13.org/documents/uploadeddocuments/docs2005/e05148r0-acs-smartattributesannex.pdf
 */
#define ATA_SMART_ATTRIBUTE_ID_RAW_READ_ERROR_RATE              1
#define ATA_SMART_ATTRIBUTE_ID_THROUGHPUT_PERFORMANCE           2
#define ATA_SMART_ATTRIBUTE_ID_SPIN_UP_TIME                     3
#define ATA_SMART_ATTRIBUTE_ID_START_STOP_COUNT                 4
#define ATA_SMART_ATTRIBUTE_ID_REALLOCATED_SECTORS_COUNT        5
#define ATA_SMART_ATTRIBUTE_ID_READ_CHANNEL_MARGIN              6
#define ATA_SMART_ATTRIBUTE_ID_SEEK_ERROR_RATE                  7
#define ATA_SMART_ATTRIBUTE_ID_SEEK_TIME_PERFORMANCE            8
#define ATA_SMART_ATTRIBUTE_ID_POWER_ON_HOURS                   9
#define ATA_SMART_ATTRIBUTE_ID_SPIN_RETRY_COUNT                 10
#define ATA_SMART_ATTRIBUTE_ID_CALIBRATION_RETRY_COUNT          11
#define ATA_SMART_ATTRIBUTE_ID_POWER_CYCLE_COUNT                12
#define ATA_SMART_ATTRIBUTE_ID_SOFT_READ_ERROR_RATE_13          13
#define ATA_SMART_ATTRIBUTE_ID_RESERVED_BLOCK_COUNT             170
#define ATA_SMART_ATTRIBUTE_ID_PROGRAM_FAIL_COUNT               171
#define ATA_SMART_ATTRIBUTE_ID_ERASE_FAIL_COUNT                 172
#define ATA_SMART_ATTRIBUTE_ID_UNEXPECT_POWER_LOSS_COUNT        174
#define ATA_SMART_ATTRIBUTE_ID_PROGRAM_FAIL_COUNT_CHIP          175
#define ATA_SMART_ATTRIBUTE_ID_ERASE_FAIL_COUNT_CHIP            176
#define ATA_SMART_ATTRIBUTE_ID_WEAR_LEVELING_COUNT              177    //Samsung SSD Wearout
#define ATA_SMART_ATTRIBUTE_ID_USED_RSVD_BLK_CNT_CHIP           178
#define ATA_SMART_ATTRIBUTE_ID_USED_RSVD_BLK_CNT_TOTAL          179
#define ATA_SMART_ATTRIBUTE_ID_UNUSED_RSVD_BLK_CNT_TOTAL        180
#define ATA_SMART_ATTRIBUTE_ID_PROGRAM_FAIL_CNT_TOTAL           181
#define ATA_SMART_ATTRIBUTE_ID_ERASE_FAIL_COUNT_TOTAL           182
#define ATA_SMART_ATTRIBUTE_ID_RUNTIME_BAD_BLOCK                183
#define ATA_SMART_ATTRIBUTE_ID_END_TO_END_ERROR                 184
#define ATA_SMART_ATTRIBUTE_ID_HEAD_STABILITY                   185    //Western Digital attribute.
#define ATA_SMART_ATTRIBUTE_ID_INDUCED_OP_VIBRATION_DETECTION   186    //Western Digital attribute.
#define ATA_SMART_ATTRIBUTE_ID_REPORTED_UNCORRECTABLE_ERRORS    187
#define ATA_SMART_ATTRIBUTE_ID_COMMAND_TIMEOUT                  188
#define ATA_SMART_ATTRIBUTE_ID_HIGH_FLY_WRITES                  189
#define ATA_SMART_ATTRIBUTE_ID_AIRFLOW_TEMPERATURE_CELSIUS      190
#define ATA_SMART_ATTRIBUTE_ID_G_SENSE_ERROR_RATE_191           191
#define ATA_SMART_ATTRIBUTE_ID_POWER_OFF_RETRACT_COUNT_192      192
#define ATA_SMART_ATTRIBUTE_ID_LOAD_CYCLE_COUNT_193             193
#define ATA_SMART_ATTRIBUTE_ID_TEMPERATURE_CELSIUS              194
#define ATA_SMART_ATTRIBUTE_ID_HARDWARE_ECC_RECOVERED           195
#define ATA_SMART_ATTRIBUTE_ID_REALLOCATED_EVENT_COUNT          196
#define ATA_SMART_ATTRIBUTE_ID_CURRENT_PENDING_SECTOR           197
#define ATA_SMART_ATTRIBUTE_ID_OFFLINE_UNCORRECTABLE            198
#define ATA_SMART_ATTRIBUTE_ID_UDMA_CRC_ERROR_COUNT             199
#define ATA_SMART_ATTRIBUTE_ID_MULTI_ZONE_ERROR_RATE            200
#define ATA_SMART_ATTRIBUTE_ID_SOFT_READ_ERROR_RATE_201         201
#define ATA_SMART_ATTRIBUTE_ID_PERCENT_LIFETIME_REMAINING       202    //Mircon SSD Wearout
#define ATA_SMART_ATTRIBUTE_ID_RUN_OUT_CANCEL                   203
#define ATA_SMART_ATTRIBUTE_ID_SOFT_ECC_CORRECTION              204
#define ATA_SMART_ATTRIBUTE_ID_THERMAL_ASPERITY_RATE            205
#define ATA_SMART_ATTRIBUTE_ID_FLYING_HEIGHT                    206
#define ATA_SMART_ATTRIBUTE_ID_SPIN_HIGH_CURRENT                207
#define ATA_SMART_ATTRIBUTE_ID_SPIN_BUZZ                        208
#define ATA_SMART_ATTRIBUTE_ID_OFFLINE_SEEK_PERFORMNCE          209
#define ATA_SMART_ATTRIBUTE_ID_DISK_SHIFT                       220
#define ATA_SMART_ATTRIBUTE_ID_G_SENSE_ERROR_RATE_221           221
#define ATA_SMART_ATTRIBUTE_ID_LOADED_HOURS                     222
#define ATA_SMART_ATTRIBUTE_ID_LOAD_RETRY_COUNT                 223
#define ATA_SMART_ATTRIBUTE_ID_LOAD_FRICTION                    224
#define ATA_SMART_ATTRIBUTE_ID_LOAD_CYCLE_COUNT_225             225
#define ATA_SMART_ATTRIBUTE_ID_LOAD_IN_TIME                     226
#define ATA_SMART_ATTRIBUTE_ID_TORQUE_AMPLIFICATION_COUNT       227
#define ATA_SMART_ATTRIBUTE_ID_POWER_OFF_RETRACT_COUNT_228      228
#define ATA_SMART_ATTRIBUTE_ID_HEAD_AMPLITUDE                   230
#define ATA_SMART_ATTRIBUTE_ID_DRIVE_TEMPERATURE                231
#define ATA_SMART_ATTRIBUTE_ID_AVAILABLE_RESERVED_SPACE         232
#define ATA_SMART_ATTRIBUTE_ID_MEDIA_WEAROUT_INDICATOR          233    //Intel SSD Wearout
#define ATA_SMART_ATTRIBUTE_ID_HEAD_FLYING_HOURS                240
#define ATA_SMART_ATTRIBUTE_ID_TRANSFER_ERROR_RATE              240    //Fujitsu attribute , Count of times the link is reset during a data transfer.
#define ATA_SMART_ATTRIBUTE_ID_TOTAL_LBAS_WRITTEN               241
#define ATA_SMART_ATTRIBUTE_ID_TOTAL_LBAS_READ                  242
#define ATA_SMART_ATTRIBUTE_ID_DRIVE_LIFETIME_REMAINNING        245    //SanDisk SSD Wearout
#define ATA_SMART_ATTRIBUTE_ID_PERCENT_WEAROUT_REMAINING        248    // DERA SSD Wearout
#define ATA_SMART_ATTRIBUTE_ID_READ_ERROR_RETRY_RATE            250
#define ATA_SMART_ATTRIBUTE_ID_FREE_FALL_SENSOR                 254

#define ATA_LOG_STRING_LENGTH                512
#define ATA_LOG_DATA_MAX_LEN                 512
#define ATA_LOG_EXT_ERR_LEN                  19
#define ATA_LOG_ERR_CMD_NUM                  5
#define ATA_LOG_ERR_ERR_NUM                  5
#define ATA_LOG_EXT_ERR_NUM                  4
#define ATA_LOG_EXT_SELF_TEST_LOG_NUM        19
#define ATA_LOG_EXT_SELF_TEST_LOG_MAX_NUM    25
#define ATA_LOG_SEAGATE_GLIST_NUM            32
#define ATA_LOG_SEAGATE_PLIST_NUM            31
#define ATA_LOG_SEAGATE_CE_NUM               16

/*
 * ata command
 * Tables B.3 and B.4 of T13/2161-D (ACS-3) Revision 4
 * http://www.t13.org/Documents/UploadedDocuments/docs2012/d2161r4-ATAATAPI_Command_Set_-_3.pdf
 */
#define ATA_CMD_NOP                             0x00
#define ATA_CMD_RECALIBRATE                     0x10   // RECALIBRATE
#define ATA_CMD_READ_SECTORS                    0x20   // READ SECTOR(S)
#define ATA_CMD_READ_SECTORS_NEW                0x21   // READ SECTOR(S)
#define ATA_CMD_READ_LONG_RETRY                 0x22   // READ LONG (with retries)
#define ATA_CMD_READ_LONG                       0x23   // READ LONG (without retries)
#define ATA_CMD_READ_SECTORS_EXT                0x24   // READ SECTOR(S) EXT
#define ATA_CMD_READ_DMA_EXT                    0x25
#define ATA_CMD_READ_DMA_QUEUED_EXT             0x26
#define ATA_CMD_READ_MULTIPLE_EXT               0x29
#define ATA_CMD_READ_STREAM_DMA                 0x2A   // READ STREAM DMA
#define ATA_CMD_READ_STREAM_PIO                 0x2B   // READ STREAM PIO
#define ATA_CMD_READ_LOG_EXT                    0x2F
#define ATA_CMD_WRITE_SECTORS                   0x30
#define ATA_CMD_WRITE_SECTORS_NEW               0x31   // WRITE SECTOR(S)
#define ATA_CMD_WRITE_LONG_RETRY                0x32   // WRITE LONG (with retries)
#define ATA_CMD_WRITE_LONG                      0x33   // WRITE LONG (without retries)
#define ATA_CMD_WRITE_SECTORS_EXT               0x34
#define ATA_CMD_WRITE_DMA_EXT                   0x35
#define ATA_CMD_WRITE_DMA_QUEUED_EXT            0x36
#define ATA_CMD_WRITE_MULTIPLE_EXT              0x39
#define ATA_CMD_WRITE_STREAM_DMA                0x3A   // WRITE STREAM DMA
#define ATA_CMD_WRITE_STREAM_PIO                0x3B   // WRITE STREAM PIO
#define ATA_CMD_WRITE_VERIFY                    0x3C   // WRITE VERIFY
#define ATA_CMD_WRITE_DMA_FUA_EXT               0x3D   // WRITE DMA FUA EXT
#define ATA_CMD_WRITE_DMA_QUEUED_FUA_EXT        0x3E   // WRITE DMA QUEUED FUA EXT
#define ATA_CMD_WRITE_LOG_EXT                   0x3F   // WRITE LOG EXT
#define ATA_CMD_READ_VERIFY_SECTORS             0x40
#define ATA_CMD_READ_VERIFY_SECTORS_NEW         0x41   // READ VERIFY SECTOR(S) without retries
#define ATA_CMD_READ_VERIFY_SECTORS_EXT         0x42
#define ATA_CMD_READ_FPDMA_QUEUED               0x60
#define ATA_CMD_WRITE_FPDMA_QUEUED              0x61
#define ATA_CMD_DOWNLOAD_MICROCODE              0x92
#define ATA_CMD_PACKET                          0xA0
#define ATA_CMD_IDENTIFY_PACKET_DEVICE          0xA1
#define ATA_CMD_SMART                           0xB0
#define ATA_CMD_DEVICE_CONFIGURATION            0xB1
#define ATA_CMD_DEVICE_CONFIGURATION_RESTORE    0xC0   // DEVICE CONFIGURATION RESTORE
#define ATA_CMD_READ_MULTIPLE                   0xC4
#define ATA_CMD_WRITE_MULTIPLE                  0xC5
#define ATA_CMD_SET_MULTIPLE_MODE               0xC6
#define ATA_CMD_READ_DMA_QUEUED                 0xC7
#define ATA_CMD_READ_DMA                        0xC8
#define ATA_CMD_READ_DMA_RETRY                  0xC9   // READ DMA (without retries, obsolete since ATA-5)
#define ATA_CMD_WRITE_DMA                       0xCA
#define ATA_CMD_WRITE_DMA_NEW                   0xCB   // WRITE DMA (without retries, obsolete since ATA-5)
#define ATA_CMD_WRITE_DMA_QUEUED                0xCC
#define ATA_CMD_WRITE_MULTIPLE_FUA_EXT          0xCE   // WRITE MULTIPLE FUA EXT
#define ATA_CMD_SMART_READ_DATA                 0xD0
#define ATA_CMD_READ_ATTRIBUTE_THRESHOLDS       0xD1   // SMART READ ATTRIBUTE THRESHOLDS
#define ATA_CMD_SMART_ENABLE_DISABLE_AUTOSAVE   0xD2
#define ATA_CMD_SMART_SAVE_ATTRIBUTE_VALUES     0xD3
#define ATA_CMD_SMART_EXECUTE_OFFLINE_IMMEDIATE 0xD4
#define ATA_CMD_SMART_READ_LOG                  0xD5
#define ATA_CMD_SMART_WRITE_LOG                 0xD6
#define ATA_CMD_SMART_ENABLE_OPERATIONS         0xD8
#define ATA_CMD_SMART_DISABLE_OPERATIONS        0xD9
#define ATA_CMD_SMART_RETURN_STATUS             0xDA
#define ATA_CMD_ENABLE_DISABLE_AUTO_OFFLINE     0xDB   // Enable/Disable Auto Offline (SFF)
#define ATA_CMD_READ_BUFFER                     0xE4
#define ATA_CMD_WRITE_BUFFER                    0xE8
#define ATA_CMD_IDENTIFY_DEVICE                 0xEC
#define ATA_CMD_SET_FEATURES                    0xEF
#define ATA_CMD_SET_MAX                         0xF9

/*
 * SCSI Sense Keys
 * https://www.t10.org/lists/2sensekey.htm
 */
#define     SCSI_KEY_NO_SENSE             0x00
#define     SCSI_KEY_RECOVERED_ERROR      0x01
#define     SCSI_KEY_NOT_READY            0x02
#define     SCSI_KEY_MEDIUM_ERROR         0x03
#define     SCSI_KEY_HARDWARE_ERROR       0x04
#define     SCSI_KEY_ILLEGAL_REQUEST      0x05
#define     SCSI_KEY_UNIT_ATTENTION       0x06
#define     SCSI_KEY_DATA_PROTECT         0x07
#define     SCSI_KEY_BLANK_CHECK          0x08
#define     SCSI_KEY_VENDOR_SPECIFIC      0x09
#define     SCSI_KEY_COPY_ABORTED         0x0A
#define     SCSI_KEY_ABORTED_COMMAND      0x0B
#define     SCSI_KEY_VOLUME_OVERFLOW      0x0D
#define     SCSI_KEY_MISCOMPARE           0x0E
#define     SCSI_KEY_COMPLETED            0x0F

/*
 * from General Condition ASC Assignments
 * https://www.t10.org/lists/asc-num.htm
 */
#define     SCSI_ASC_NO_SENSE                                 0x00
#define     SCSI_ASC_PERIPHERAL_DEVICE_WRITE_FAULT            0x03
#define     SCSI_ASC_LUN_NOT_READY                            0x04
#define     SCSI_ASC_WRITE_ERROR                              0x0C
#define     SCSI_ASC_INVALID_IU                               0x0E
#define     SCSI_ASC_PI_ERROR                                 0x10
#define     SCSI_ASC_UNRECOVERED_READ                         0x11
#define     SCSI_ASC_ADDRESS_MARK_NOT_FOUND                   0x12
#define     SCSI_ASC_RECORD_NOT_FOUND                         0x14
#define     SCSI_ASC_DATA_SYNC_MARK_ERROR                     0X16
#define     SCSI_ASC_RECOVERED_ECC                            0x18
#define     SCSI_ASC_DEFECT_LIST_ERROR                        0x19
#define     SCSI_ASC_PARAMETER_LIST_LENGTH_ERROR              0x1A
#define     SCSI_ASC_DEFECT_LIST_NOT_FOUND                    0x1C
#define     SCSI_ASC_DURING_VERIFY_BYTE_CHECK_OPEARTION       0X1D
#define     SCSI_ASC_INVALID_COMMAND_OPCODE                   0x20
#define     SCSI_ASC_ACCESS_DENIED                            0x20
#define     SCSI_ASC_LBA_OUT_OF_RANGE                         0x21
#define     SCSI_ASC_INVALID_FIELD_IN_CDB                     0x24
#define     SCSI_ASC_LOGICAL_UNIT_NOT_SUPPORTED               0x25
#define     SCSI_ASC_INVALID_FIELD_IN_PARAMETER_LIST          0x26
#define     SCSI_ASC_WRITE_PROTECTED                          0x27
#define     SCSI_ASC_NOT_READY_TO_READY_CHANGE                0x28
#define     SCSI_ASC_POWER_OR_RESET_OCCURED                   0x29
#define     SCSI_ASC_PARAMETERS_CHANGED                       0x2A
#define     SCSI_ASC_COMMAND_SEQUENCE_ERROR                   0x2C
#define     SCSI_ASC_COMMANDS_CLEARED_BY_ANOTHER              0x2F
#define     SCSI_ASC_FORMAT_COMMAND_FAILED                    0x31
#define     SCSI_ASC_NO_DEFECT_SPARE_LOCATION_AVAILABLE       0x32
#define     SCSI_ASC_ROUNDED_PARAMETER                        0x37
#define     SCSI_ASC_SAVING_PARAMETERS_NOT_SUPPORTED          0x39
#define     SCSI_ASC_MEDIUM_NOT_PRESENT                       0x3A
#define     SCSI_ASC_LOGICAL_UNIT_FAILURE                     0x3E
#define     SCSI_ASC_INFO_CHANGED_OR_DELETED                  0x3F
#define     SCSI_ASC_DIAG_FAILURE                             0x40
#define     SCSI_ASC_MESSAGE_ERROR                            0x43
#define     SCSI_ASC_INTERNAL_TARGET_FAILURE                  0x44
#define     SCSI_ASC_UNSUCCESSFUL_SOFT_RESET                  0x46
#define     SCSI_ASC_SCSI_PARITY                              0x47
#define     SCSI_ASC_INITIATOR_DETECTED_ERROR                 0x48
#define     SCSI_ASC_INVALID_MESSAGE_ERROR                    0x49
#define     SCSI_ASC_DATA_PHASE_ERROR                         0x4B
#define     SCSI_ASC_LOGICAL_UNIT_FAILED_SELF_CONFIGURATION   0x4C
#define     SCSI_ASC_TAGGED_OVERLAPPED_COMMANDS               0x4D
#define     SCSI_ASC_OVERLAPPED_COMMANDS_ATTEMPTED            0x4E
#define     SCSI_ASC_INSUFFICIENT_RESOURCES                   0x55
#define     SCSI_ASC_LOG_CONDITION                            0x5B
#define     SCSI_ASC_SERVICE_RPL                              0x5C
#define     SCSI_ASC_FAILURE_THRESHOLD_EXCEEDED               0x5D
#define     SCSI_ASC_LOW_POWER_CONDITION                      0x5E

/*
 * SENSE ASCQ NOT READY
 * from Complete ASC/ASCQ Assignments Listing
 * https://www.t10.org/lists/asc-num.htm
 */
#define     SCSI_ASCQ_CAUSE_NOT_REPORTABLE               0x00
#define     SCSI_ASCQ_BECOMING_READY                     0x01
#define     SCSI_ASCQ_INITIALIZING_CMD_REQUIRED          0x02
#define     SCSI_ASCQ_MANUAL_INTERVENTION_REQUIRED       0x03
#define     SCSI_ASCQ_FORMAT_IN_PROGRESS                 0x04
#define     SCSI_ASCQ_REBUILD_IN_PROGRESS                0x05
#define     SCSI_ASCQ_RECALCULATION_IN_PROGRESS          0x06
#define     SCSI_ASCQ_OPERATION_IN_PROGRESS              0x07
#define     SCSI_ASCQ_LONG_WRITE_IN_PROGRESS             0x08
#define     SCSI_ASCQ_SELF_TEST_IN_PROGRESS              0x09
#define     SCSI_ASCQ_STRUCTURE_CHECK_REQUIRED           0x0d
#define     SCSI_ASCQ_CONFIGURATION_CHANGED              0x0E
#define     SCSI_ASCQ_AUXILIARY_MEMORY_NOT_ACCESSIBLE    0x10
#define     SCSI_ASCQ_NOTIFY_ENABLE_SPINUP_REQUIRED      0x11
#define     SCSI_ASCQ_OFFLINE                            0x12
#define     SCSI_ASCQ_SA_CREATION_IN_PROGRESS            0x13
#define     SCSI_ASCQ_SPACE_ALLOCATION_IN_PROGRESS       0x14
#define     SCSI_ASCQ_ROBOTICS_DISABLED                  0x15
#define     SCSI_ASCQ_CONFIGURATION_REQUIRED             0x16
#define     SCSI_ASCQ_CALIBRATION_REQUIRED               0x17
#define     SCSI_ASCQ_A_DOOR_IS_OPEN                     0x18
#define     SCSI_ASCQ_OPERATING_IN_SEQUENTIAL_MODE       0x19
#define     SCSI_ASCQ_START_STOP_IN_PROGRESS             0x1a
#define     SCSI_ASCQ_SANITIZE_IN_PROGRESS               0x1b
#define     SCSI_ASCQ_ADD_POWER_USE_NOT_YET_GRANTED      0x1c     //  ADDITIONAL POWER USE NOT YET GRANTED

/*
 * SENSE ASCQ THRESHOLD EXCEEDED
 * from Complete ASC/ASCQ Assignments Listing
 * https://www.t10.org/lists/asc-num.htm
 */
#define     SCSI_ASCQ_GENERAL_HARD_DRIVE_FAILURE        0x10
#define     SCSI_ASCQ_HW_TOO_MANY_BLOCK_REASSIGNED      0x14
#define     SCSI_ASCQ_FAILURE_CONTROLLER_DETECTED       0x18
#define     SCSI_ASCQ_FW_TOO_MANY_BLOCK_REASSIGN        0xfc
#define     SCSI_ASCQ_HARD_DIRVE_FAILURE                0xfd
#define     SCSI_ASCQ_THROUGHPUT_PERFORMANCE            0xfe

/*
 * SENS ASCQ NOT ACCESSIBLE
 * from Complete ASC/ASCQ Assignments Listing
 * https://www.t10.org/lists/asc-num.htm
 */
#define     SCSI_ASCQ_ASYMETRIC_ACCESS_STATE_TRANSITION  0x0a
#define     SCSI_ASCQ_TARGET_PORT_ON_STANDBY_STATE       0x0b
#define     SCSI_ASCQ_TARGET_PORT_UNAVAILABLE            0x0c

/*
 * ata smart attribute data structure, vendor specific
 * SFF 8035 spec *
 * https://www.linux-mips.org/pub/linux/mips/people/macro/S.M.A.R.T./SFF-8035i.pdf
 */
typedef struct _ATA_SMART_ATTRIBUTE
{
    guint8  id;
    guint16 flags;
    guint8  current;
    guint8  worst;
    guint8  raw[6]; //属性原始值
    guint8  reserved;
} ATA_SMART_ATTRIBUTE_S;

/*
 * ata smart data structure
 * Table 62 of T13/1699-D (ATA8-ACS) Revision 6a
 * http://www.t13.org/documents/uploadeddocuments/docs2008/d1699r6a-ata8-acs.pdf
 */
typedef struct _ATA_SMART_DATA //refers to ACS-3 Table-130
{
    guint16           revNumber;
    ATA_SMART_ATTRIBUTE_S smartAttribute[30];
    guint8            offLineDataCollectionStatus;
    guint8            selftestExecStatus;
    guint16           totalTimeToCompleteOffLine;
    guint8            vendorSpecific366;
    guint8            offLineDataCollectionCapability;
    guint16           smartCapability;
    guint8            errorlogCapability;
    guint8            vendorSpecific371;
    guint8            shortSelfTestPollingTime;
    guint8            extendSelfTestPollingTimeinByte;
    guint8            conveyanceSelfTestPollingTime;
    guint16           extendSelfTestPollingTimeinWord;
    guint8            reserved377_385[9];
    guint8            vendorSpecific386_510[125];
    guint8            chkSum;
} ATA_SMART_DATA_S;

/*
 * ata smart threshold data structure, vendor specific
 * 4.2. of SFF 8035i, page 13
 * https://www.linux-mips.org/pub/linux/mips/people/macro/S.M.A.R.T./SFF-8035i.pdf
 */
typedef struct _ATA_SMART_THRESHOLD_ENTRY
{
    guint8 id;
    guint8 threshold;
    guint8 reserved[10];
} ATA_SMART_THRESHOLD_ENTRY_S;

/*
 * Device attribute threshold data structure
 * 4. of SFF 8035i, page 13
 * https://www.linux-mips.org/pub/linux/mips/people/macro/S.M.A.R.T./SFF-8035i.pdf
 */
typedef struct _ATA_SMART_THRESHOLDS
{
    guint16                     revNumber;
    ATA_SMART_THRESHOLD_ENTRY_S thresholdEntries[30];
    guint8                      reserved[149];
    guint8                      chkSum;
} ATA_SMART_THRESHOLDS_S;

typedef struct _ATA_SMART_ERRLOG_INPUT
{
    guint8 commandReg;
    guint8 featuresReg;
    guint8 status;
    guint8 errorRegister;
    guint16 sectorCount;
} ATA_SMART_ERRLOG_INPUT_S;

/*
 * Error log data structure
 * Table 42 of T13/1321D Rev 1 spec
 * https://www.seagate.com/support/disc/manuals/ata/d1153r17.pdf
 */
typedef struct _ATA_SMART_ERRLOG_ERR
{
    guint8 reserved;
    guint8 errorRegister;
    guint8 sectorCount;
    guint8 sectorNumber;
    guint8 cylinderLow;
    guint8 cylinderHigh;
    guint8 driveHead;
    guint8 status;
    guint8 extendedError[ATA_LOG_EXT_ERR_LEN];
    guint8 state;
    guint16 timestamp;
} ATA_SMART_ERRLOG_ERR_S;

/*
 * Command Data Structure
 * Table 41 of T13/1321D Rev 1 spec
 * https://www.seagate.com/support/disc/manuals/ata/d1153r17.pdf
 */
typedef struct _ATA_SMART_ERRLOG_CMD
{
    guint8 deviceControlReg;
    guint8 featuresReg;
    guint8 sectorCount;
    guint8 sectorNumber;
    guint8 cylinderLow;
    guint8 cylinderHigh;
    guint8 driveHead;
    guint8 commandReg;
    guint32 timestamp;
} ATA_SMART_ERRLOG_CMD_S;

/*
 * Error log data structure
 * Table 40 of T13/1321D Rev 1 spec
 * https://www.seagate.com/support/disc/manuals/ata/d1153r17.pdf
 */
typedef struct _ATA_SMART_ERRLOG_ENTRY
{
    ATA_SMART_ERRLOG_CMD_S commands[ATA_LOG_ERR_CMD_NUM];
    ATA_SMART_ERRLOG_ERR_S error;
} ATA_SMART_ERRLOG_ENTRY_S;

/*
 * SMART error log sector
 * Table 39 of T13/1321D Rev 1 spec
 * https://www.seagate.com/support/disc/manuals/ata/d1153r17.pdf
 */
typedef struct _ATA_SMART_ERRLOG
{
    guint8 revNumber;
    guint8 errorLogPointer;
    ATA_SMART_ERRLOG_ENTRY_S errorlog[ATA_LOG_ERR_ERR_NUM];
    guint16 errorCount;
    guint8 reserved[57];
    guint8 chkSum;
} ATA_SMART_ERRLOG_S;

/*
 * Error data structure
 * Table A.10 T13/1699-D Revision 6a
 * http://www.t13.org/documents/uploadeddocuments/docs2008/d1699r6a-ata8-acs.pdf
 */
typedef struct _ATA_SMART_EXTERRLOG_ERROR
{
    guint8 deviceControlReg;
    guint8 errorReg;
    guint8 countReg;
    guint8 countRegHi;
    guint8 lbaLowReg;
    guint8 lbaLowRegHi;
    guint8 lbaMidReg;
    guint8 lbaMidRegHi;
    guint8 lbaHighReg;
    guint8 lbaHighRegHi;
    guint8 deviceReg;
    guint8 statusReg;
    guint8 extendedError[ATA_LOG_EXT_ERR_LEN];
    guint8 state;
    guint16 timestamp;
} ATA_SMART_EXTERRLOG_ERROR_S;

/*
 * Command data structure
 * Table A.9 of T13/1699-D Revision 6a
 * http://www.t13.org/documents/uploadeddocuments/docs2008/d1699r6a-ata8-acs.pdf
 */
typedef struct _ATA_SMART_EXTERRLOG_CMD
{
    guint8 deviceControlReg;
    guint8 featuresReg;
    guint8 featuresRegHi;
    guint8 countReg;
    guint8 countRegHi;
    guint8 lbaLowReg;
    guint8 lbaLowRegHi;
    guint8 lbaMidReg;
    guint8 lbaMidRegHi;
    guint8 lbaHighReg;
    guint8 lbaHighRegHi;
    guint8 deviceReg;
    guint8 commandReg;
    guint8 reserved;
    guint32 timestamp;
} ATA_SMART_EXTERRLOG_CMD_S;

/*
 * Error log data structure
 * Table A.8 of T13/1699-D Revision 6a
 * http://www.t13.org/documents/uploadeddocuments/docs2008/d1699r6a-ata8-acs.pdf
 */
typedef struct _ATA_SMART_EXTERRLOG_ENTRY
{
    ATA_SMART_EXTERRLOG_CMD_S commands[ATA_LOG_ERR_CMD_NUM];
    ATA_SMART_EXTERRLOG_ERROR_S error;
} ATA_SMART_EXTERRLOG_ENTRY_S;

/*
 * Ext. Comprehensive SMART error log
 * Table A.7 of T13/1699-D Revision 6a
 * http://www.t13.org/documents/uploadeddocuments/docs2008/d1699r6a-ata8-acs.pdf
 */
typedef struct _ATA_SMART_EXTERRLOG
{
    guint8 version;
    guint8 reserved1;
    guint16 errorLogIndex;
    ATA_SMART_EXTERRLOG_ENTRY_S errorLogs[ATA_LOG_EXT_ERR_NUM];
    guint16 deviceErrorCount;
    guint8 reserved2[9];
    guint8 chkSum;
} ATA_SMART_EXTERRLOG_S;

/*
 * Self-test log descriptor entry
 * Table 45 of T13/1321D Rev 1 spec
 * http://www.t13.org/documents/uploadeddocuments/docs2008/d1699r6a-ata8-acs.pdf
 */
typedef struct _ATA_SMART_EXTSELFTESTLOG_DESC
{
    guint8 selfTestType;
    guint8 selfTestStatus;
    guint16 timestamp;
    guint8 chkPoint;
    guint8 failingLba[6];
    guint8 vendorSpecific[15];
} ATA_SMART_EXTSELFTESTLOG_DESC_S;

/*
 * Self-test log data structure
 * Table 44 of T13/1321D Rev 1 spec
 * http://www.t13.org/documents/uploadeddocuments/docs2008/d1699r6a-ata8-acs.pdf
 */
typedef struct _ATA_SMART_EXTSELFTESTLOG
{
    guint8 version;
    guint8 reserved1;
    guint16 logDescIndex;
    ATA_SMART_EXTSELFTESTLOG_DESC_S logDescs[ATA_LOG_EXT_SELF_TEST_LOG_NUM];
    guint8 vendorSpecifc[2];
    guint8 reserved2[11];
    guint8 chksum;
} ATA_SMART_EXTSELFTESTLOG_S;

typedef struct _ATA_SMART_SEAGATEGLIST_ITEM
{
    guint16 R;    //硬盘上的物理地址Radius
    guint16 T;    //硬盘上的物理地址Theta
    guint16 Z;    //硬盘上的物理地址Z
    guint32 LBA;  //逻辑块地址
    guint16 POT;  //上电时间 (Hours)
    guint8 a[4];
} ATA_SMART_SEAGATEGLIST_ITEM_S;

typedef struct _ATA_SMART_SEAGATEGLIST
{
    guint8 reserved0[16];
    ATA_SMART_SEAGATEGLIST_ITEM_S items[ATA_LOG_SEAGATE_GLIST_NUM];
    guint8 reserved1[16];
} ATA_SMART_SEAGATEGLIST_S;

typedef struct _ATA_SMART_SEAGATEPLIST_ITEM
{
    guint16 R;
    guint16 T;
    guint16 Z;
    guint32 LBA;
    guint16 POT;
    guint8 a[4];
} ATA_SMART_SEAGATEPLIST_ITEM_S;

typedef struct _ATA_SMART_SEAGATEPLIST
{
    guint8 reserved0[16];
    ATA_SMART_SEAGATEPLIST_ITEM_S items[ATA_LOG_SEAGATE_PLIST_NUM];
} ATA_SMART_SEAGATEPLIST_S;

typedef struct _ATA_SMART_CRITICALEVENT_ITEM
{
    guint16 type;           //类型
    guint16 POT;            //上电时间 (Hours)
    guint32 timestamp;      //时间戳
    guint32 LBA;            //逻辑块地址
    guint32 EC;             //错误码
    guint16 temp;           //温度
    guint16 unknow2;        //ATA CMD
    guint32 R;
    guint16 Z;
    guint16 T;
    guint16 index;
    guint16 reserved;
} ATA_SMART_CRITICALEVENT_ITEM_S;

typedef struct _ATA_SMART_DISKSTATUS_DESC
{
    guint16 status;
    gchar* desc;
} ATA_SMART_DISKSTATUS_DESC_S;

typedef struct _ATA_SMART_CMD_DESC
{
    guint8 reg;
    gchar* desc;
} ATA_SMART_CMD_DESC_S;

typedef struct _ATA_SEAGATE_FARM_HEADER
{
	guint64 signature;
	guint64 majorRev;
	guint64 minorRev;
	guint64 pageNum;
	guint64 logSize;
	guint64 pageSize;
	guint64 headSupport;
	guint64 copiesNum;
	gchar reserved[16320];
} ATA_SEAGATE_FARM_HEADER_S;

typedef struct _ATA_SEAGATE_FARM_DRIVE_INFO
{
	guint64 pageNum;
	guint64 copyNum;
	gchar serialNumHi[8];
	gchar serialNumLow[8];
	guint64 wwnHi;
	guint64 wwnLow;
	gchar interfaceType[8];
	guint64 capacity;
	guint64 pSectorSize;
	guint64 lSectorSize;
	guint64 bufferSize;
	guint64 headNum;
	guint64 formFactor;
	guint64 rotationRate;
	gchar fwRevHi[8];
	gchar fwRevLow[8];
	guint64 ataSecurityState;
	guint64 ataFeaturesSupported;
	guint64 ataFeaturesEnabled;
	guint64 powerOnHour;
	guint64 spindlePowerOnHour;
	guint64 headFlightHour;
	guint64 headLoadEvent;
	guint64 powerCycleCount;
	guint64 hardwareResetCount;
	guint64 spinupTime;
	guint64 NVCStatusPowerOn;
	guint64 timeAvailableToSave;
	guint64 startTimeStamp;
	guint64 endTimeStamp;
	gchar reserved[16144];
} ATA_SEAGATE_FARM_DRIVE_INFO_S;

typedef struct _ATA_SEAGATE_FARM_WORKLOAD_STAT
{
	guint64 pageNum;
	guint64 copyNum;
	guint64 ratedWP;
	guint64 readCmdNum;
	guint64 writeCmdNum;
	guint64 rReadCmdNum;
	guint64 rWriteCmdNum;
	guint64 otherCmdNum;
	guint64 lSectorWritten;
	guint64 lSectorRead;
	gchar reserved[16304];
} ATA_SEAGATE_FARM_WORKLOAD_STAT_S;

typedef struct _ATA_SEAGATE_FARM_ERROR_STAT
{
	guint64 pageNum;
	guint64 copyNum;
	guint64 unrecoverReadErrors;
	guint64 unrecoverWriteErrors;
	guint64 reallocatedSectors;
	guint64 readRecoveryAttempts;
	guint64 mechanicalStartFailures;
	guint64 reallocatedCandidateSectors;
	guint64 ASREvents;
	guint64 interfaceCRCErrors;
	guint64 spinRetryCount;
	guint64 spinRetryCountNormalized;
	guint64 spinRetryCountWorst;
	guint64 IOEDCErrors;
	guint64 CTOTotal;
	guint64 CTOOver5s;
	guint64 CTOOver7_5s;
	guint64 flashLEDEvents;
	guint64 flashLEDCodeAddress;
    guint64 uncorrectableErrors;
    guint64 FruCodeSmartTrip;
    guint64 infoFlashLEDEvent;
    guint64 infoReadWriteRetryEvent;
    guint64 superParityFlyRecovery;
    guint64 ReallocatedSectorsByCause;
	gchar reserved[15952];
} ATA_SEAGATE_FARM_ERROR_STAT_S;

typedef struct _ATA_SEAGATE_FARM_ENV_STAT
{
	guint64 pageNum;
	guint64 copyNum;
	guint64 currentTemp;
	guint64 highestTemp;
	guint64 lowestTemp;
	guint64 avgShortTermTemp;
	guint64 avgLongTermTemp;
	guint64 highestAvgShortTermTemp;
	guint64 lowestAvgShortTermTemp;
	guint64 highestAvgLongTermTemp;
	guint64 lowestAvgLongTermTemp;
	guint64 overTempTime;
	guint64 underTempTime;
	guint64 maxOperatTemp;
	guint64 minOperatTemp;
	guint64 overLimitShockEvents;
	guint64 highFlyWriteCount;
	guint64 currentRelativeHumidity;
	guint64 humidityMixedRatio;
	guint64 currentMotorPower;
	gchar reserved[16224];
} ATA_SEAGATE_FARM_ENV_STAT_S;

typedef struct _ATA_SEAGATE_FARM_RELIA_STAT
{
    guint64 pageNum;
    guint64 copyNum;
    guint64 lastIDDTimeStamp;
    guint64 lastIDDSubCmd;
    guint64 discSlip[24];
    guint64 errorRateOfZone0[24];
    guint64 GListReclamationsNum;
    guint64 servoStatus;
    guint64 altsListBeforeIDD;
    guint64 altsListAfterIDD;
    guint64 residentGlistBeforeIDD;
    guint64 residentGlistAfterIDD;
    guint64 scrubsListBeforeIDD;
    guint64 scrubsListAfterIDD;
    guint64 DOSScansPerformedNum;
    guint64 LBAsCorrectedByISPNum;
    guint64 validParitySectorsNum;
    guint64 DOSWriteRefreshCount[24];
    guint64 RAWOperationsNum;
    guint64 DVGASkipWriteDetect[24];
    guint64 RVGASkipWriteDetect[24];
    guint64 FVGASkipWriteDetect[24];
    guint64 skipWriteDetectThred[24];
    guint64 errorRateRaw;
    guint64 errorRateNormalized;
    guint64 errorRateWorst;
    guint64 seekErrorRateRaw;
    guint64 seekErrorRateNormalized;
    guint64 seekErrorRateWorst;
    guint64 highPriorityUnloadEvents;
    guint64 microActuatorLockoutAccumulated;
    guint64 ACFFSine1X[24];
    guint64 ACFFCosine1X[24];
    guint64 PZTCalibration[24];
    guint64 MRHeadResistance[24];
    guint64 TMDNum[24];
    guint64 velocityObserver[24];
    guint64 velocityObserverNum[24];
    guint64 currentH2SATTrimmedMeanBitsInError[24][3];
    guint64 currentH2SATIterationsToConverge[24][3];
    guint64 currentH2SATCodewordsAtIterationLevel[24];
    guint64 H2SATAmplitude[24];
    guint64 H2SATAsymmetry[24];
    guint64 flyheightClearanceDelta[24][3];
    guint64 discSlipRecalibrationPerformed;
    guint64 reallocatedSectorsNum[24];
    guint64 reallocationCandidateSectorsNum[24];
    guint64 heliumPressureThreshold;
    guint64 DOSOughtScansCount[24];
    guint64 DOSNeedScansCount[24];
    guint64 DOSWriteScansCount[24];
    guint64 WritePOHInSeconds[24];
    guint64 RVAbsoluteMean;
    guint64 MaxRVAbsoluteMean;
    guint64 IdleTime;
    guint64 DOSWriteCountThreshold[24];
    guint64 SecondMRHeadResistance[24];
    gchar reserved[9624];
}ATA_SEAGATE_FARM_RELIA_STAT_S;

#define ATA_GENERAL_PURPOSE_LOG_DIRECTORY_LENGTH            512

typedef union _IDE_IDENTIFY_DEVICE_T
{
    guint16  word[256];      /* raw version of data */
    struct                  /* formatted version of data */
    {
        guint16  generalConfiguration;
        guint16  obsolete;
        guint16  specificConfiguration;
        guint16  obsolete1;
        guint16  retired1[2];
        guint16  obsolete2;
        guint16  reserved2[2];
        guint16  retired3;
        guint16  serialNumber[10];
        guint16  retired4[2];
        guint16  obsolete5;
        guint16  firmwareRevision[4];
        guint16  modelNumber[20];
        guint16  maxMultipleSize;
        guint16  reserved6;
        guint16  capabilities[2];
        guint16  obsolete6[2];
        guint16  validWords;
        guint16  obsolete7[5];
        guint16  totalUserSectors[2];
        guint16  obsolete8;
        guint16  dmaMode;
        guint16  pioModesSupported;
        guint16  minimumMultiwordDmaTransferCycleTime;
        guint16  manufacturerRecommendedMultiwordDmaTransferCycleTime;
        guint16  minumumPioTransferCycleTimeWithoutFlowControl;
        guint16  minumumPioTransferCycleTimeWithFlowControl;
        guint16  reserved9[2];
        guint16  reserved10[4];
        guint16  queueDepth;
        guint16  reserved11[4];
        guint16  majorVersionNumber;
        guint16  minorVersionNumber;
        guint16  commandSetsSupported[3];
        guint16  commandSetsEnabled[2];
        guint16  commandSetDefaults;
        guint16  ultraDmaModes;
        guint16  timeForSecurityErase;
        guint16  timeForEnahncedSecurityErase;
        guint16  currentAdvancedPowerManagementValue;
        guint16  masterPasswordRevisionCode;
        guint16  hardwareResetResult;
        guint16  acousticManagement;
        guint16  streamMinReqSize;
        guint16  streamTransferTime;
        guint16  streamAccessLatency;
        guint16  streamPerformanceGranularity[2];
        guint16  maximumLBAfor48bitAddressing[4];
        guint16  streamingTransferTime;
        guint16  reserved12;
        guint16  sectorSize;
        guint16  interSeekDelay;
        guint16  reserved13[9];
        guint16  logicalSectorWords[2];
        guint16  reserved14[8];
        guint16  removableMediaStatusNotification;
        guint16  securityStatus;
        guint16  vendorSpecific[31];
        guint16  CFApowerMode;
        guint16  reserved15[15];
        guint16  mediaSerialNumber[30];
        guint16  reserves16[49];
        guint16  integrityWord;
    } data;
} IDE_IDENTIFY_DEVICE_T;
#define IDE_IDENTIFY_DEVICE_S sizeof(IDE_IDENTIFY_DEVICE_T)

typedef struct _SCSI_SENSE_DISECT
{
    guint8 error_code;
    guint8 sense_key;
    guint8 asc;
    guint8 ascq;
} SCSI_SENSE_DISECT_S;

typedef struct _SCSI_ERROR_COUNTER {
    guint8 got_pc[7];
    guint8 got_extra_pc;
    guint64 counter[8];
}SCSI_ERROR_COUNTER_S;

typedef struct _SCSI_NON_MEDIUM_ERROR {
    guint8 got_pc0;
    guint8 got_extra_pc;
    guint64 counter_pc0;
    guint8 got_tfe_h;
    guint64 counter_tfe_h; /* Track following errors [Hitachi] */
    guint8 got_pe_h;
    guint64 counter_pe_h;  /* Positioning errors [Hitachi] */
}SCSI_NON_MEDIUM_ERROR_S;

typedef struct _SCSI_ADDITIONAL_CODES_DESC {
    gchar* desc;
    guint8 asc;
    guint8 ascq;
}SCSI_ADDITIONAL_CODES_DESC_S;

//------------模拟RAID卡诊断事件
typedef struct tag_sml_mock_ctrl_event
{
    guint32 event_code;
    guint16 pd_device_id;
    guint16 encl_device_id;

    /* 硬盘状态变化事件 */
    guint8  pd_prev_state;
    guint8  pd_new_state;

    /* 硬盘sense code事件 */
    guint8  pd_sense_key;
    guint8  pd_sense_asc;
    guint8  pd_sense_ascq;

} SML_MOCK_CTRL_EVENT_S;

typedef struct PD_VENDOR_NAME_ATTR_ID
{
    const gchar * vendor_name;
    guint8 wearout_attr_id;
}PD_VENDOR_NAME_ATTR_ID_S;

/* BEGIN:2022/7/26 PN:AR20220719595805 */
typedef struct PD_VENDOR_ID_VENDOE_NAME {
    guint32 vendor_id;
    const gchar *vendor_name;
}PD_VENDOR_ID_VENDOE_NAME_S;
/* END */

typedef struct PD_VENDOR_PN_VENDOR_NAME {
    const gchar *vendor_pn;
    const gchar *vendor_name;
}PD_VENDOR_PN_VENDOR_NAME_S;

#define SCSI_CMD_TIME_OUT               10
#define SCSI_LOG_PAGE_RESP_LENGTH       252
#define SCSI_CDB_LENGTH16               16
#define SCSI_CDB_LENGTH12               12
#define SCSI_CDB_LENGTH10               10
#define SCSI_CDB_LENGTH6                6

#define SCSI_SELF_TEST_TIMEOUT          (5 * 60 * 60)

#define SCSI_UNIT_SERIAL_NUMBER_VPD_PAGE_LEN 0x2c // SPC-5 max serial number length 40 byte + header 4 byte size
#define SCSI_LOG_PAGE_HEADER_SIZE                  4
#define SCSI_DEFECT_LIST_HEADER_SIZE               8
//SCSI Operation codes, refers to SPC-4 Table E.2
#define SCSI_CMD_INQUIRY                        0x12
#define SCSI_CMD_LOG_SENSE                      0x4d
#define SCSI_CMD_REQUEST_SENSE                  0x03
#define SCSI_CMD_LOG_SELECT                     0x4c
#define SCSI_CMD_MODE_SENSE                     0x1a
#define SCSI_CMD_MODE_SENSE_10                  0x5a
#define SCSI_CMD_MODE_SELECT                    0x15
#define SCSI_CMD_MODE_SELECT_10                 0x55
#define SCSI_CMD_SEND_DIAGNOSTIC                0x1d
#define SCSI_CMD_RECEIVE_DIAGNOSTIC             0x1c
#define SCSI_CMD_READ_DEFECT_10                 0x37
#define SCSI_CMD_DIAG_BG_SHORT_SELF_TEST        0x01
#define SCSI_CMD_DIAG_BG_EXTENDED_SELF_TEST     0x02
#define SCSI_CMD_DIAG_FG_SHORT_SELF_TEST        0x05
#define SCSI_CMD_DIAG_FG_EXTENDED_SELF_TEST     0x06
#define SCSI_CMD_DIAG_ABORT_SELF_TEST           0x04
#define SCSI_CMD_READ_DEFECT_DATA               0xb7

// ANSI SCSI Log Page code, refers to SPC-4 Table E.11
#define SCSI_LOG_PAGE_SUPPORTED_LOG_PAGES           0x00
#define SCSI_LOG_PAGE_BUFFER_OVERRUN                0x01
#define SCSI_LOG_PAGE_WRITE_ERROR_COUNTER           0x02
#define SCSI_LOG_PAGE_READ_ERROR_COUNTER            0x03
#define SCSI_LOG_PAGE_READ_REVERSE_ERROR_COUNTER    0x04
#define SCSI_LOG_PAGE_VERIFY_ERROR_COUNTER          0x05
#define SCSI_LOG_PAGE_NON_MEDIUM_ERROR              0x06
#define SCSI_LOG_PAGE_LAST_N_ERROR                  0x07
#define SCSI_LOG_PAGE_FORMAT_STATUS                 0x08
#define SCSI_LOG_PAGE_TEMPERATURE                   0x0d
#define SCSI_LOG_PAGE_STARTSTOP_CYCLE_COUNTER       0x0e
#define SCSI_LOG_PAGE_APPLICATION_CLIENT            0x0f
#define SCSI_LOG_PAGE_SELFTEST_RESULTS              0x10
#define SCSI_LOG_PAGE_SOLID_STATE_MEDIA             0x11
#define SCSI_LOG_PAGE_BACKGROUND_RESULTS            0x15
#define SCSI_LOG_PAGE_PROTOCOL_SPECIFIC             0x18
#define SCSI_LOG_PAGE_GENERAL_STATISTICS            0x19
#define SCSI_LOG_PAGE_TAPE_ALERTS                   0x2e
#define SCSI_LOG_PAGE_IE                            0x2f
#define SCSI_LOG_PAGE_HUAWEI_SPECIFIC32H            0x32
#define SCSI_LOG_PAGE_HUAWEI_SPECIFIC34H            0x34
#define SCSI_LOG_PAGE_HUAWEI_SPECIFIC35H            0x35
#define SCSI_LOG_PAGE_KIOXIA_SSD_SPECIFIC           0x30
#define SCSI_LOG_PAGE_KIOXIA_EOPT                   0x30
#define SCSI_LOG_PAGE_KIOXIA_SMART_DATA             0x38


/* SCSI IO response status (SAM-2 status codes) */
#define SCSI_STATUS_SUCCESS                     0x00
#define SCSI_STATUS_CHECK_CONDITION             0x02
#define SCSI_STATUS_CONDITION_MET               0x04
#define SCSI_STATUS_BUSY                        0x08
#define SCSI_STATUS_INTERMEDIATE                0x10  /* SAM-4中已不再使用 */
#define SCSI_STATUS_INTERMEDIATE_CONDMET        0x14  /* SAM-4中已不再使用 */
#define SCSI_STATUS_RESERVATION_CONFLICT        0x18
#define SCSI_STATUS_COMMAND_TERMINATED          0x22  /* SAM-4中已不再使用 */
#define SCSI_STATUS_TASK_SET_FULL               0x28
#define SCSI_STATUS_ACA_ACTIVE                  0x30
#define SCSI_STATUS_TASK_ABORT                  0x40  /* SAM-4中新增 */

//response code of sense data
#define SCSI_FIXED_SENSE_DATA_RESP_CUR_INFO         0x70
#define SCSI_FIXED_SENSE_DATA_RESP_DEFERRD_ERR      0x71
#define SCSI_DESC_SENSE_DATA_RESP_CUR_INFO          0x72
#define SCSI_DESC_SENSE_DATA_RESP_DEFERRD_ERR       0x73

/* BEGIN: Added by z00352904, 2016/12/1 17:22:5  PN:AR-0000264509-001-001 */
#define MEDIA_TYPE_HDD                               "HDD"
#define MEDIA_TYPE_SSD                               "SSD"
#define MEDIA_TYPE_SSM                               "SSM"

#define INTERFACE_STR_SCSI                           "parallel SCSI"
#define INTERFACE_STR_SAS                            "SAS"
#define INTERFACE_STR_SATA                           "SATA"
#define INTERFACE_STR_FC                             "FC"
#define INTERFACE_STR_SATA_SAS                       "SATA/SAS"
#define INTERFACE_STR_PCIE                           "PCIe"
/* END:   Added by z00352904, 2016/12/1 17:22:11*/

/* BEGIN: Added by h00371221, 2017/12/25   AR.SR.SFEA02109385.002.001*/
#define PD_VENDOR_ID_SEAGATE     0x000C50
#define PD_VENDOR_ID_WDC         0x0014EE
#define PD_VENDOR_ID_HGST        0x000CCA
#define PD_VENDOR_ID_SANDISK     0x001173
#define PD_VENDOR_ID_TOSHIBA     0x000039
#define PD_VENDOR_ID_INTEL_1     0x5CD2E4
#define PD_VENDOR_ID_INTEL_2     0x001517
#define PD_VENDOR_ID_SMI         0x486834
#define PD_VENDOR_ID_SAMSUNG     0x002538
#define PD_VENDOR_ID_MICRON      0x00A075
/* BEGIN: Added by TanXin TWX555928, 2018/4/18   PN:AR.SR.SFEA02130837.004.002-006/007 */
#define PD_VENDOR_ID_LITE_ON     0x002303
#define PD_VENDOR_ID_SSSTC       0x38F601
#define PD_VENDOR_ID_HYNIX       0xACE42E
#define PD_VENDOR_ID_HUAWEI_1    0x6CEBB6
#define PD_VENDOR_ID_HUAWEI_2    0x708CB6
#define PD_VENDOR_ID_HUAWEI_3    0x48DC2D
#define PD_VENDOR_ID_YANGTZE_MEMORY    0xA428B7
#define PD_VENDOR_ID_RAMAXEL     0x044A50
#define PD_VENDOR_ID_UMIS     0x5CDFB8
#define PD_VENDOR_ID_DERA        0x9CBD6E
#define PD_VENDOR_ID_AL        0x3A5A27
#define PD_VENDOR_NAME_DERA          "DERA"
#define PD_VENDOR_NAME_UMIS          "UMIS"
#define PD_VENDOR_NAME_RAMAXEL       "RAMAXEL"
#define PD_VENDOR_NAME_YANGTZE_MEMORY  "YANGTZE MEMORY"
#define PD_VENDOR_NAME_SSSTC       "SSSTC"
#define PD_VENDOR_NAME_HYNIX       "HYNIX"
#define PD_VENDOR_NAME_LITE_ON     "LITEON"
#define PD_VENDOR_NAME_LITE_ON_ER2    "LITEON ER2"
#define PD_VENDOR_NAME_LITE_ON_CVZ    "LITEON CVZ"
#define PD_VENDOR_NAME_LITE_ON_EGT    "LITEON EGT"
/* END:   Added by TanXin TWX555928, 2018/4/18 */
#define PD_VENDOR_NAME_INTEL       "INTEL"
#define PD_VENDOR_NAME_SAMSUNG     "SAMSUNG"
#define PD_VENDOR_NAME_MICRON      "MICRON"
#define PD_VENDOR_NAME_SANDISK     "SANDISK"
#define PD_VENDOR_NAME_FUJITSU     "FUJITSU"
#define PD_VENDOR_NAME_SEAGATE     "SEAGATE"
#define PD_VENDOR_NAME_WDC         "WDC"
#define PD_VENDOR_NAME_HITACHI     "HGST"
#define PD_VENDOR_NAME_TOSHIBA     "TOSHIBA"
#define PD_VENDOR_NAME_SMI         "SMI"
#define PD_VENDOR_NAME_HUAWEI      "HUAWEI"
#define PD_VENDOR_NAME_KIOXIA      "KIOXIA"
#define PD_VENDOR_NAME_RAMAXEL     "RAMAXEL"
#define PD_VENDOR_NAME_UMIS        "UMIS"
#define PD_VENDOR_NAME_AL          "LONGSYS"

#define PD_MODEL_NUMBER_SMI_M2       "MD619GXBFCD4S"
/* END:    Added by h00371221, 2017/12/25 */

#define PD_VENDOR_ID_PHISON      0x6479A7
#define PD_VENDOR_NAME_PHISON     "PHISON"

#define PD_VENDOR_ID_DATSSD      0x9C1C6D
#define PD_VENDOR_NAME_DATSSD    "DATSSD"

#define PD_VENDOR_NAME_HUADIAN                   "HUADIAN"
#define PD_MODEL_NUMBER_HUADIAN__H10I_IY_256G    "DGC63256S3I27SD"

#define PD_VENDOR_NAME_DEFAULT   "ATA"

#pragma pack()

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif
