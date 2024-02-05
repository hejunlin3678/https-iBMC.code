/*
 * Scsi-t10.h - Header file for SCSI structures.
 *
 * This file contains only items that are defined by the SCSI standard.  Any
 * internal structures must be defined in the scsi/inc/scsiint.h header file.
 *
 * Written by Gerry Smith 1/9/2013
 *          - Content copied from previous scsidef.h header file
 */
#ifndef INCLUDE_SCSI
#define INCLUDE_SCSI
#define _PACKED             __attribute((packed))               /* GNU compiler mechanism  */
#pragma GCC diagnostic ignored  "-Wpacked"
#pragma GCC diagnostic ignored  "-Wattributes"
// Following SCSI constants are needed in pd.c; Also defined in scsi.h
#define SCSI_LOG_SENSE_CUMULATIVE_VALUES        0x01
#define SCSI_LOG_SENSE_PC_SHIFT                 0x06
#define SCSI_LOG_SENSE_SELF_TEST_LOG_PAGE       0x10
#define SCSI_MODE_CACHING_FLAG_RCD             (0x01)   /* Read Cache Disable */
#define SCSI_MODE_CACHING_FLAG_WCE             (0x04)   /* Write Cache Enable */
#define SCSI_MODE_CACHING_FLAG2_DRA            (0x20)
#define SCSI_MODE_CACHING_PAGE_LENGTH          (0x12)

#define SCSI_LOG_SENSE_TEMPERATURE_PAGE                 0x0D
#define SCSI_LOG_SENSE_INFORMATIONAL_EXCEPTIONS_PAGE    0x2F
#define SCSI_LOG_SENSE_PC_CURRENT                       0x01

#define SCSI_UNMAP_ONE_CMD_PARAM_LENGTH        (24)
#define SCSI_UNMAP_ONE_DATA_DESC_LENGTH        (22)
#define SCSI_UNMAP_ONE_BLK_DESC_LENGTH         (16)
typedef enum _Provisioning_Type {
    PROVISION_NO_REPORT,
    RESOURCE_PROVISIONED,
    THIN_PROVISIONED,
}PROVISION_TYPE;


/**
 * SCSI Peripheral device types
 */
/*typedef enum _SCSI_DEV_TYPE {
    SCSI_DAD            = 0x00,
    SCSI_SAD            = 0x01,
    SCSI_PRINTER        = 0x02,
    SCSI_PROCESSOR      = 0x03,
    SCSI_WRITE_ONCE     = 0x04,
    SCSI_CDROM          = 0x05,
    SCSI_SCANNER        = 0x06,
    SCSI_OPTICAL        = 0x07,
    SCSI_MEDIUM_CHANGER = 0x08,
    SCSI_ARRAY_CTRL     = 0x0c,
    SCSI_SES            = 0x0d,
    SCSI_SDAD           = 0x0e,
    SCSI_OPTICAL_CARD   = 0x0f,
    SCSI_BRIDGE         = 0x10,
    SCSI_OBJECT_DEV     = 0x11,
    SCSI_AUTOMATION     = 0x12,
    SCSI_WELL_KNOWN     = 0x1e,
    SCSI_NOT_PRESENT    = 0x1f,
} SCSI_DEV_TYPE;
*/
/**
 * Define the SCSI command codes
 */
typedef enum    _SCSI_COMMAND {
    SC_TEST_UNIT_READY              = 0x00,
    SC_REZERO_UNIT                  = 0x01,
    SC_REQUEST_SENSE                = 0x03,
    SC_FORMAT_UNIT                  = 0x04,
    SC_REASSIGN_BLOCKS              = 0x07,
    SC_READ_6                       = 0x08,
    SC_WRITE_6                      = 0x0A,
    SC_SEEK_6                       = 0x0B,
    SC_INQUIRY                      = 0x12,
    SC_MODE_SELECT_6                = 0x15,
    SC_RESERVE_6                    = 0x16,
    SC_RELEASE_6                    = 0x17,
    SC_COPY                         = 0x18,
    SC_MODE_SENSE_6                 = 0x1A,
    SC_START_STOP_UNIT              = 0x1B,
    SC_RECEIVE_DIAGNOSTIC_RESULTS   = 0x1C,
    SC_SEND_DIAGNOSTIC              = 0x1D,
    SC_PREVENT_ALLOW_MEDIUM_REMOVAL = 0x1E,
    SC_READ_CAPACITY                = 0x25,
    SC_READ                         = 0x28,
    SC_WRITE                        = 0x2A,
    SC_SEEK                         = 0x2B,
    SC_WRITE_AND_VERIFY             = 0x2E,
    SC_VERIFY                       = 0x2F,
    SC_SET_LIMITS                   = 0x33,
    SC_PRE_FETCH                    = 0x34,
    SC_SYNCHRONIZE_CACHE            = 0x35,
    SC_LOCK_UNLOCK_CACHE            = 0x36,
    SC_READ_DEFECT_DATA             = 0x37,
    SC_COMPARE                      = 0x39,
    SC_COPY_AND_VERIFY              = 0x3A,
    SC_WRITE_BUFFER                 = 0x3B,
    SC_READ_BUFFER                  = 0x3C,
    SC_READ_LONG                    = 0x3E,
    SC_WRITE_LONG                   = 0x3F,
    SC_CHANGE_DEFINITION            = 0x40,
    SC_WRITE_SAME                   = 0x41,
    SC_UNMAP                        = 0x42,
    SC_SANITIZE                     = 0x48,
    SC_LOG_SELECT                   = 0x4C,
    SC_LOG_SENSE                    = 0x4D,
    SC_XDWRITE                      = 0x50,
    SC_XPWRITE                      = 0x51,
    SC_XDREAD                       = 0x52,
    SC_MODE_SELECT                  = 0x55,
    SC_RESERVE                      = 0x56,
    SC_RELEASE                      = 0x57,
    SC_MODE_SENSE                   = 0x5A,
    SC_PERSISTENT_RESERVE_IN        = 0x5E,
    SC_PERSISTENT_RESERVE_OUT       = 0x5F,
    SC_VARIABLE_OP_CODE             = 0x7F,
    SC_XDWRITE_EXTENDED             = 0x80,
    SC_REBUILD                      = 0x81,
    SC_REGENERATE                   = 0x82,
    SC_ATA_PASSTHROUGH_16           = 0x85,
    SC_READ_16                      = 0x88,
    SC_WRITE_16                     = 0x8A,
    SC_WRITE_AND_VERIFY_16          = 0x8E,
    SC_VERIFY_16                    = 0x8F,
    SC_SYNCHRONIZE_CACHE_16         = 0x91,
    SC_WRITE_SAME_16                = 0x93,
    SC_SERVICE_ACTION_BIDIRECTIONAL = 0x9D,
    SC_SERVICE_ACTION_IN_16         = 0x9E,
    SC_READ_CAPACITY_16             = 0x9E,     // psuedo-name for legacy code - users should be updated to use SC_SERVICE_ACTION_IN_16
    SC_READ_LONG_16                 = 0x9E,     // psuedo-name for legacy code - users should be updated to use SC_SERVICE_ACTION_IN_16
    SC_SERVICE_ACTION_OUT_16        = 0x9F,
    SC_WRITE_LONG_16                = 0x9F,     // psuedo-name for legacy code - users should be updated to use SC_SERVICE_ACTION_OUT_16
    SC_REPORT_LUNS                  = 0xA0,
    SC_ATA_PASSTHROUGH_12           = 0xA1,
    SC_SECURITY_PROTOCOL_IN         = 0xA2,
    SC_REPORT_SUPPORTED_OP_CODES    = 0xA3,
    SC_MOVE_MEDIUM_ATTACHED         = 0xA7,
    SC_READ_12                      = 0xA8,
    SC_WRITE_12                     = 0xAA,
    SC_WRITE_AND_VERIFY_12          = 0xAE,
    SC_VERIFY_12                    = 0xAF,
    SC_READ_ELEMENT_STATUS_ATTACHED = 0xB4,
    SC_SECURITY_PROTOCOL_OUT        = 0xB5,
} SCSI_COMMAND;

/**
 * Define SCSI Service Actions
 */
typedef enum _SCSI_SERVICE_ACTION {
    SSA_OVERWRITE_ERASE             = 0x0001,
    SSA_BLOCK_ERASE                 = 0x0002,
    SSA_CRYPTO_ERASE                = 0x0003,
    SSA_XPWRITE_32                  = 0x0006,
    SSA_XDWRITEREAD_32              = 0x0007,
    SSA_READ_32                     = 0x0009,
    SSA_VERIFY_32                   = 0x000A,
    SSA_WRITE_32                    = 0x000B,
    SSA_WRITE_AND_VERIFY_32         = 0x000C,
    SSA_WRITE_SAME_32               = 0x000D,
    SSA_ORWRITE                     = 0x000E,
    SSA_RECEIVE_CREDENTIAL          = 0x1800,
} SCSI_SERVICE_ACTION;

/**
 * Define the SCSI status codes
 */
typedef enum    _SCSI_STATUS {
    SS_GOOD                         = 0x00,
    SS_CHECK                        = 0x02,
    SS_CONDITION_MET                = 0x04,
    SS_BUSY                         = 0x08,
    SS_INTERMEDIATE                 = 0x10,
    SS_INTERMEDIATE_CONDITION_MET   = 0x14,
    SS_RESERVATION_CONFLICT         = 0x18,
    SS_COMMAND_TERMINATED           = 0x22,
    SS_TASK_SET_FULL                = 0x28,
    SS_ACA_ACTIVE                   = 0x30
} SCSI_STATUS;

/**
 * Define the SCSI SENSE codes
 */
typedef enum    _SCSI_SENSE_KEY {
    SN_NO_SENSE             = 0x00,
    SN_RECOVERED_ERROR      = 0x01,
    SN_NOT_READY            = 0x02,
    SN_MEDIUM_ERROR         = 0x03,
    SN_HARDWARE_ERROR       = 0x04,
    SN_ILLEGAL_REQUEST      = 0x05,
    SN_UNIT_ATTENTION       = 0x06,
    SN_DATA_PROTECT         = 0x07,
    SN_BLANK_CHECK          = 0x08,
    SN_VENDOR_SPECIFIC      = 0x09,
    SN_COPY_ABORTED         = 0x0A,
    SN_ABORTED_COMMAND      = 0x0B,
    SN_VOLUME_OVERFLOW      = 0x0D,
    SN_MISCOMPARE           = 0x0E,
    SN_COMPLETED            = 0x0F,
} SCSI_SENSE_KEY;

/**
 * Define the SCSI Additional Sense Codes
 */
typedef enum    _SCSI_ASENSE {
    SA_NO_SENSE                                 = 0x00,
    SA_PERIPHERAL_DEVICE_WRITE_FAULT            = 0x03,
    SA_LUN_NOT_READY                            = 0x04,
    SA_WRITE_ERROR                              = 0x0C,
    SA_INVALID_IU                               = 0x0E,
    SA_PI_ERROR                                 = 0x10,
    SA_UNRECOVERED_READ                         = 0x11,
    SA_ADDRESS_MARK_NOT_FOUND                   = 0x12,
    SA_RECORD_NOT_FOUND                         = 0x14,
    SA_RECOVERED_ECC                            = 0x18,
    SA_DEFECT_LIST_ERROR                        = 0x19,
    SA_PARAMETER_LIST_LENGTH_ERROR              = 0x1A,
    SA_DEFECT_LIST_NOT_FOUND                    = 0x1C,
    SA_INVALID_COMMAND_OPCODE                   = 0x20,
    SA_ACCESS_DENIED                            = 0x20,         // sense=0x7
    SA_LBA_OUT_OF_RANGE                         = 0x21,
    SA_INVALID_FIELD_IN_CDB                     = 0x24,
    SA_LOGICAL_UNIT_NOT_SUPPORTED               = 0x25,
    SA_INVALID_FIELD_IN_PARAMETER_LIST          = 0x26,
    SA_WRITE_PROTECTED                          = 0x27,
    SA_NOT_READY_TO_READY_CHANGE                = 0x28,
    SA_POWER_OR_RESET_OCCURED                   = 0x29,
    SA_PARAMETERS_CHANGED                       = 0x2A,
    SA_COMMAND_SEQUENCE_ERROR                   = 0x2C,
    SA_COMMANDS_CLEARED_BY_ANOTHER              = 0x2F,
    SA_FORMAT_COMMAND_FAILED                    = 0x31,
    SA_NO_DEFECT_SPARE_LOCATION_AVAILABLE       = 0x32,
    SA_ROUNDED_PARAMETER                        = 0x37,
    SA_SAVING_PARAMETERS_NOT_SUPPORTED          = 0x39,
    SA_MEDIUM_NOT_PRESENT                       = 0x3A,
    SA_LOGICAL_UNIT_FAILURE                     = 0x3E,
    SA_INFO_CHANGED_OR_DELETED                  = 0x3F,
    SA_DIAG_FAILURE                             = 0x40,
    SA_MESSAGE_ERROR                            = 0x43,
    SA_INTERNAL_TARGET_FAILURE                  = 0x44,
    SA_UNSUCCESSFUL_SOFT_RESET                  = 0x46,
    SA_SCSI_PARITY                              = 0x47,
    SA_INITIATOR_DETECTED_ERROR                 = 0x48,
    SA_INVALID_MESSAGE_ERROR                    = 0x49,
    SA_DATA_PHASE_ERROR                         = 0x4B,
    SA_LOGICAL_UNIT_FAILED_SELF_CONFIGURATION   = 0x4C,
    SA_TAGGED_OVERLAPPED_COMMANDS               = 0x4D,
    SA_OVERLAPPED_COMMANDS_ATTEMPTED            = 0x4E,
    SA_INSUFFICIENT_RESOURCES                   = 0x55,
    SA_LOG_CONDITION                            = 0x5B,
    SA_SERVICE_RPL                              = 0x5C,
    SA_FAILURE_THRESHOLD_EXCEEDED               = 0x5D,
    SA_LOW_POWER_CONDITION                      = 0x5E,
} SCSI_ASENSE;

/**
 * Define the SCSI ASCQ code for NOT_READY (02/04/ASCQ)
 * See http://www.t10.org/lists/asc-num.htm
 */
typedef enum _SCSI_ASCQ_NOT_READY {
    SCSI_ASCQ_NR_CAUSE_NOT_REPORTABLE               = 0x00,     // 04/00 - LOGICAL UNIT NOT READY, CAUSE NOT REPORTABLE
    SCSI_ASCQ_NR_BECOMING_READY                     = 0x01,     // 04/01 - LOGICAL UNIT IS IN PROCESS OF BECOMING READY
    SCSI_ASCQ_NR_INITIALIZING_CMD_REQUIRED          = 0x02,     // 04/02 - LOGICAL UNIT NOT READY, INITIALIZING COMMAND REQUIRED
    SCSI_ASCQ_NR_MANUAL_INTERVENTION_REQUIRED       = 0x03,     // 04/03 - LOGICAL UNIT NOT READY, MANUAL INTERVENTION REQUIRED
    SCSI_ASCQ_NR_FORMAT_IN_PROGRESS                 = 0x04,     // 04/04 - LOGICAL UNIT NOT READY, FORMAT IN PROGRESS
    SCSI_ASCQ_NR_REBUILD_IN_PROGRESS                = 0x05,     // 04/05 - LOGICAL UNIT NOT READY, REBUILD IN PROGRESS
    SCSI_ASCQ_NR_RECALCULATION_IN_PROGRESS          = 0x06,     // 04/06 - LOGICAL UNIT NOT READY, RECALCULATION IN PROGRESS
    SCSI_ASCQ_NR_OPERATION_IN_PROGRESS              = 0x07,     // 04/07 - LOGICAL UNIT NOT READY, OPERATION IN PROGRESS
    SCSI_ASCQ_NR_LONG_WRITE_IN_PROGRESS             = 0x08,     // 04/08 - LOGICAL UNIT NOT READY, LONG WRITE IN PROGRESS
    SCSI_ASCQ_NR_SELF_TEST_IN_PROGRESS              = 0x09,     // 04/09 - LOGICAL UNIT NOT READY, SELF-TEST IN PROGRESS
    SCSI_ASCQ_NA_ASYMETRIC_ACCESS_STATE_TRANSITION  = 0x0a,     // 04/0a - LOGICAL UNIT NOT ACCESSIBLE, ASYMMETRIC ACCESS STATE TRANSITION
    SCSI_ASCQ_NA_TARGET_PORT_ON_STANDBY_STATE       = 0x0b,     // 04/0b - LOGICAL UNIT NOT ACCESSIBLE, TARGET PORT IN STANDBY STATE
    SCSI_ASCQ_NA_TARGET_PORT_UNAVAILABLE            = 0x0c,     // 04/0c - LOGICAL UNIT NOT ACCESSIBLE, TARGET PORT IN UNAVAILABLE STATE
    SCSI_ASCQ_NR_STRUCTURE_CHECK_REQUIRED           = 0x0d,     // 04/0d - LOGICAL UNIT NOT READY, STRUCTURE CHECK REQUIRED
    SCSI_ASCQ_NR_CONFIGURATION_CHANGED              = 0x0E,     // 3F/0Eh CONFIGURATION HAS BEEN CHANGED
    SCSI_ASCQ_NR_AUXILIARY_MEMORY_NOT_ACCESSIBLE    = 0x10,     // 04/10 - LOGICAL UNIT NOT READY, AUXILIARY MEMORY NOT ACCESSIBLE
    SCSI_ASCQ_NR_NOTIFY_ENABLE_SPINUP_REQUIRED      = 0x11,     // 04/11 - LOGICAL UNIT NOT READY, NOTIFY (ENABLE SPINUP) REQUIRED
    SCSI_ASCQ_NR_OFFLINE                            = 0x12,     // 04/12 - LOGICAL UNIT NOT READY, OFFLINE
    SCSI_ASCQ_NR_SA_CREATION_IN_PROGRESS            = 0x13,     // 04/13 - LOGICAL UNIT NOT READY, SA CREATION IN PROGRESS
    SCSI_ASCQ_NR_SPACE_ALLOCATION_IN_PROGRESS       = 0x14,     // 04/14 - LOGICAL UNIT NOT READY, SPACE ALLOCATION IN PROGRESS
    SCSI_ASCQ_NR_ROBOTICS_DISABLED                  = 0x15,     // 04/15 - LOGICAL UNIT NOT READY, ROBOTICS DISABLED
    SCSI_ASCQ_NR_CONFIGURATION_REQUIRED             = 0x16,     // 04/16 - LOGICAL UNIT NOT READY, CONFIGURATION REQUIRED
    SCSI_ASCQ_NR_CALIBRATION_REQUIRED               = 0x17,     // 04/17 - LOGICAL UNIT NOT READY, CALIBRATION REQUIRED
    SCSI_ASCQ_NR_A_DOOR_IS_OPEN                     = 0x18,     // 04/18 - LOGICAL UNIT NOT READY, A DOOR IS OPEN
    SCSI_ASCQ_NR_OPERATING_IN_SEQUENTIAL_MODE       = 0x19,     // 04/19 - LOGICAL UNIT NOT READY, OPERATING IN SEQUENTIAL MODE
    SCSI_ASCQ_NR_START_STOP_IN_PROGRESS             = 0x1a,     // 04/1a - LOGICAL UNIT NOT READY, START STOP UNIT COMMAND IN PROGRESS
    SCSI_ASCQ_NR_SANITIZE_IN_PROGRESS               = 0x1b,     // 04/1b - LOGICAL UNIT NOT READY, SANITIZE IN PROGRESS
    SCSI_ASCQ_NR_ADDITIONAL_POWER_USE_NOT_YET_GRANTED=0x1c,     // 04/1c - LOGICAL UNIT NOT READY, ADDITIONAL POWER USE NOT YET GRANTED
} SCSI_ASCQ_NOT_READY;

/**
 * Define the SCSI ASCQ for SA_PI_ERROR 0A/10/ASCQ
 * See http://www.t10.org/lists/asc-num.htm
 */
typedef enum _SCSI_ASCQ_PI {
    SCSI_ASCQ_PI_ID_CRC_OR_ECC_ERROR            = 0,    // 10/00    ID CRC or ECC error
    SCSI_ASCQ_PI_GUARD_CHECK_FAILED             = 1,    // 10/01    LOGICAL BLOCK GUARD CHECK FAILED
    SCSI_ASCQ_PI_APPLICATION_TAG_CHECK_FAILED   = 2,    // 10/02    LOGICAL BLOCK APPLICATION TAG CHECK FAILED
    SCSI_ASCQ_PI_REFERENCE_TAG_CHECK_FAILED     = 3,    // 10/03    LOGICAL BLOCK REFERENCE TAG CHECK FAILED
} SCSI_ASCQ_PI;

/*
 ******************************************************************************
 *
 * Define the SCSI sense data structures
 *
 ******************************************************************************
 */

/**
 * Define SCSI sense response codes
 */
typedef enum _SCSI_SENSE_RESPONSE_CODE {
    SCSI_SENSE_RESPONSE_CODE_CURRENT        = 0x70,
    SCSI_SENSE_RESPONSE_CODE_DEFERRED       = 0x71,
    SCSI_SENSE_RESPONSE_CODE_DESC_CURRENT   = 0x72,
    SCSI_SENSE_RESPONSE_CODE_DESC_DEFERRED  = 0x73,
} SCSI_SENSE_RESPONSE_CODE;

/**
 * Define the fixed sense data format
 */
typedef struct  _SCSI_SENSE_FIXED {
    uchar   responseCode    : 7;
    uchar   valid           : 1;
    uchar   obsolete;
    uint    senseKey        : 4;
    uint    sdat_ovfl       : 1;    // sense data overflow
    uint    ili             : 1;
    uint    eom             : 1;
    uint    filemark        : 1;
    uint    information;            // load/store in Big Endian format
    uchar   additionalSenseLength;
    uchar   commandSepcificInfo[4];
    uchar   asc;
    uchar   ascq;
    uchar   fruCode;
    uchar   sksv;
    ushort  fieldPointer;
} _PACKED SCSI_SENSE_FIXED;       /* declare as _PACKED so compiler will generate unaligned loads */

/*
 *-----------------------------------------------------------------------------
 *
 * Define the sub-structures and the main structure for descriptor-based sense
 * data
 */

/**
 * SCSI sense descriptor types
 */
typedef enum _SCSI_SENSE_DESC_TYPE {
    SCSI_SENSE_DESC_TYPE_INFORMATION                = 0,
    SCSI_SENSE_DESC_TYPE_CMD_SPEC_INFO              = 1,
    SCSI_SENSE_DESC_TYPE_SENSE_KEY_SPEC             = 2,
    SCSI_SENSE_DESC_TYPE_FRU                        = 3,
    SCSI_SENSE_DESC_TYPE_STREAM_CMDS                = 4,
    SCSI_SENSE_DESC_TYPE_BLK_CMDS                   = 5,
    SCSI_SENSE_DESC_TYPE_OSD_OBJ_IDT                = 6,
    SCSI_SENSE_DESC_TYPE_OSD_INT_CHK                = 7,
    SCSI_SENSE_DESC_TYPE_OSD_ATT_IDT                = 8,
    SCSI_SENSE_DESC_TYPE_ATA                        = 9,
    SCSI_SENSE_DESC_TYPE_ANOTHER_PROGRESS_INDICATION= 0x0a,
    SCSI_SENSE_DESC_TYPE_USER_DATA_SEGMENT_REFERAL  = 0x0b,
    SCSI_SENSE_DESC_TYPE_FORWARDED_SENSE_DATA       = 0x0c,
} SCSI_SENSE_DESC_TYPE;

/**
 * SCSI sense descriptor for fields common to all descriptors
 */
typedef struct _SCSI_SENSE_DESC_COMMON {
    uchar    descType;
    uchar    additionalLength;  //length-1
} _PACKED SCSI_SENSE_DESC_COMMON;

/**
 * SCSI sense descriptor for INFORMATION data
 */
typedef struct _SCSI_SENSE_DESC_INFORMATION {
    uchar    descType;
    uchar    addLen;
    uint8    reserved   :7;
    uint8    valid      :1;
    uint8    reserved1;
    uint64   information;
} _PACKED SCSI_SENSE_DESC_INFORMATION;

/**
 * SCSI sense descriptor for COMMAND SPECIFIC data
 */
typedef struct _SCSI_SENSE_DESC_CMD_SPECIFIC {
    uchar    descType;
    uchar    addLen;
    uint8    reserved;
    uint8    reserved1;
    uchar    cmdSpecificInformation[8];
} _PACKED SCSI_SENSE_DESC_CMD_SPECIFIC;

/**
 * SCSI sense descriptor for sense-key-specific data
 */
typedef struct _SCSI_SENSE_DESC_SENSE_KEY_SPECIFIC {
    uchar    descType;
    uchar    addLen;
    uint8    reserved[2];
    union {
        struct {
            uchar       bitPointer  :3;
            uchar       bpv         :1;
            uchar       reserved    :2;
            uchar       cmd_data    :1;
            uchar       sksv        :1;
            ushort      fieldPointer;
        } _PACKED fieldPointer;
        struct {
            uchar       reserved    :7;
            uchar       sksv        :1;
            ushort      actualRetryCount;
        } _PACKED actualRetryCount;
        struct {
            uchar       reserved    :7;
            uchar       sksv        :1;
            ushort      progressIndication;
        } _PACKED progressIndication;
        struct {
            uchar       bitPointer  :3;
            uchar       bpv         :1;
            uchar       reserved    :2;
            uchar       cmd_data    :1;
            uchar       sksv        :1;
            ushort      segmentPointer;
        } _PACKED segmentPointer;
        uchar   sksv;
        ushort  reserved1;
    } sense_key_specific;
    uint8       reserved2;
} _PACKED SCSI_SENSE_DESC_SENSE_KEY_SPECIFIC;

/**
 * SCSI sense descriptor for FRU data
 */
typedef struct _SCSI_SENSE_DESC_FRU {
    uchar   descType;
    uchar   addLen;
    uchar   reserved;
    uchar   fru;
} _PACKED SCSI_SENSE_DESC_FRU;

/**
 * SCSI sense descriptor for block device data
 */
typedef struct _SCSI_SENSE_DESC_BLOCK_DEV {
    uchar   descType;
    uchar   addLen;
    uchar   reserved1;
    uchar   reserved2   : 5;
    uchar   ili         : 1;
    uchar   reserved3   : 2;
} _PACKED SCSI_SENSE_DESC_BLOCK_DEV;

/**
 * SCSI sense descriptor for "another progress indication"
 */
typedef struct _SCSI_SENSE_DESC_ANOTHER_PROGRESS_INDICATION {
    uchar   descType;
    uchar   addLen;
    uchar   anotherSenseKey;
    uchar   anotherASC;
    uchar   anotherASCQ;
    uchar   reserved;
    uchar   anotherProgressIndication[2];
} _PACKED SCSI_SENSE_DESC_ANOTHER_PROGRESS_INDICATION;

/**
 * SCSI sense descriptor for forwarded sense data
 */
typedef struct _SCSI_SENSE_DESC_FORWARDED_SENSE_DATA {
    uchar   descType;
    uchar   addLen;
    uchar   senseDataSource : 4;
    uchar   reserved1       : 3;
    uchar   fsdt            : 1;
    uchar   forwardedStatus;
    uchar   forwardedSenseData[0];
} _PACKED SCSI_SENSE_DESC_FORWARDED_SENSE_DATA;

/**
 * SCSI sense desceriptor for vendor-specific data
 */
typedef struct _SCSI_SENSE_DESC_VENDOR_SPECIFIC {
    uchar   descType;
    uchar   addLen;
    uchar   vendorSpecific[0];
} _PACKED SCSI_SENSE_DESC_VENDOR_SPECIFIC;

/**
 * SCSI sense desceriptor for used data segment referral data
 */
typedef struct _SCSI_SENSE_DESC_USER_DATA_SEGMENT_REFERRAL {
    uchar   descType;
    uchar   addLen;
    uchar   not_all_r   : 1;
    uchar   reserved2   : 7;
    uchar   reserved3;
    struct {
        uchar   asymetricAccessState    : 4;
        uchar   reserved0               : 4;
        uchar   reserved1;
        uchar   targetPortGroup[2];
    } targetProtGroupDescriptor[0];
} _PACKED SCSI_SENSE_DESC_USER_DATA_SEGMENT_REFERRAL;

/**
 * SCSI sense descriptor union for all descriptor data types
 */
typedef union _SCSI_SENSE_DESC_UNION {
    SCSI_SENSE_DESC_COMMON                      commonDesc;
    SCSI_SENSE_DESC_INFORMATION                 infoDesc;
    SCSI_SENSE_DESC_CMD_SPECIFIC                cmdSpecDesc;
    SCSI_SENSE_DESC_SENSE_KEY_SPECIFIC          snskeySpecDesc;
    SCSI_SENSE_DESC_FRU                         fruDataDesc;
    SCSI_SENSE_DESC_BLOCK_DEV                   blkDev;
    SCSI_SENSE_DESC_ANOTHER_PROGRESS_INDICATION anotherProgressIndication;
    SCSI_SENSE_DESC_USER_DATA_SEGMENT_REFERRAL  userDataSegmentReferral;
    SCSI_SENSE_DESC_FORWARDED_SENSE_DATA        forwardedSenseData;
    SCSI_SENSE_DESC_VENDOR_SPECIFIC             vendorSpecific;
} _PACKED SCSI_SENSE_DESC_UNION;


/**
 * Structure to define sense data that is in the descriptor format.
 */
typedef struct _SCSI_SENSE_DESC {
    uchar   responseCode    : 7;
    uchar   reserved1       : 1;
    uchar   senseKey        : 4;
    uchar   reserved2       : 4;
    uchar   asc;
    uchar   ascq;
    uchar   reserved3       : 7;
    uchar   sdat_ovfl       : 1;
    uchar   reserved4[2];
    uchar   additionalSenseLength;
    SCSI_SENSE_DESC_UNION       firstDesc;  //desc are not a fixed size so cannot index as array
} _PACKED SCSI_SENSE_DESC;

/**
 * Define a union for both fixed and descriptor-based sense data
 */
typedef union _SCSI_SENSE {
    struct {  //fields common to fixed and desc formats
        uchar   responseCode    : 7;
        uchar   reserved1       : 1;
    };
    SCSI_SENSE_DESC     descFormat;
    SCSI_SENSE_FIXED    fixedFormat;
} _PACKED SCSI_SENSE;


/*
 ******************************************************************************
 ******************************************************************************
 *
 * Define structures for SCSI command data
 *
 ******************************************************************************
 ******************************************************************************
 */

/*
 * define the cdb length macros
 */
#define CDB_LENGTH_6    6
#define CDB_LENGTH_10   10
#define CDB_LENGTH_16   16
#define CDB_LENGTH_32   32
/**
 * Define INQUIRY CDB structure
 */

typedef struct _SCSI_CDB_INQUIRY {
    uchar  opcode;              /* 0x12                                           */
    uchar  evpd :1;             /* EVPD = bit 0                                   */
    uchar  reserved :7;         /* reserved                                       */
    uchar  pageCode;            /* 0x00, 0x80 = Unit Serial Number Page if EVPD   */
    ushort allocationLength; /*                                                    */
    uchar  control;             /* 0x00 - only for linked commands.  */
} _PACKED SCSI_CDB_INQUIRY;

/**
 * Define RECEIVE DIAGNOSTIC RESULTS CDB structure
 */
typedef struct _SCSI_CDB_RECEIVE_DIAGNOSTIC_RESULTS
{
    U8  Opcode;                 /* 0x1D                                           */
    U8  Pcv;                    /* Reserved = [7..1] PCV(Page code valid bit) = 0 */
    U8  Pagecode;               /* Page Code Field                                */
    U8  Length1;                /* bits [15..8] Parameter List Length             */
    U8  Length0;                /* bits [7..0] Parameter List Length              */
    U8  Control;                /* 0x00 - only for linked commands.               */
    U16 Pad;                    /* Padding for 32 bit alignment                   */
} SCSI_CDB_RECEIVE_DIAGNOSTIC_RESULTS, *PTR_SCSI_CDB_RECEIVE_DIAGNOSTIC_RESULTS;


/**
 * Define the SCSI INQUIRY data format
 */
typedef struct  _SCSI_INQUIRY {
    uint    peripheralDeviceType :  5;      /* byte 0 */
    uint    peripheralQualifier  :  3;

    uint    reserved1            :  7;      /* byte 1 */
    uint    rmb                  :  1;

    uchar   version;                        /* byte 2 */

    uint    responseDataFormat   :  4;      /* byte 3 */
    uint    hiSup                :  1;
    uint    normAca              :  1;
    uint    obsolete1            :  2;

    uchar   additionalLength;               /* byte 4 */

    uchar   protect              :  1;      /* byte 5 */
    uchar   reserved3            :  2;
    uchar   thirdPartyCopy       :  1;
    uchar   alua                 :  2;
    uchar   acc                  :  1;
    uchar   sccs                 :  1;

    uint    addr16               :  1;      /* byte 6 */
    uint    obsolete6            :  2;
    uint    mChngr               :  1;
    uint    multiP               :  1;
    uint    vs1                  :  1;
    uint    encServ              :  1;
    uint    bQue                 :  1;

    uint    vs2                  :  1;      /* byte 7 */
    uint    cmdQue               :  1;
    uint    obsolete7a           :  1;
    uint    linked               :  1;
    uint    sync                 :  1;
    uint    wBus16               :  1;
    uint    obsolete7b           :  2;

    char    vendorIdentification[8];        /* byte 8 */
    char    productIdentification[16];      /* byte 16 */
    char    productRevisionLevel[4];        /* byte 32 */
    char    vendorSpecific[20];             /* byte 36 */

    uint    ius                 :  1;       /* byte 56 */
    uint    qas                 :  1;
    uint    clocking            :  2;
    uint    reserved5           :  4;

    uchar   reserved6;                      /* byte 57 */
    ushort  versionDescriptor[8];           /* byte 58 */
    uchar   reserved7[22];                  /* byte 74 */
                                            /* byte 96 */
} SCSI_INQUIRY;

/**
 * Define SCSI inquiry VPD descriptor
 */
typedef struct _SCSI_VPD_DEVID {
    U8      codeSet             : 4;    // SCSI_VPD_DEVID_CODE_SET
    U8      protocolIdentifier  : 4;    // SCSI_VPD_DEVID_PROTOCOL
    U8      identifierType      : 4;    // SCSI_VPD_DEVID_TYPE
    U8      association         : 2;    // SCSI_VPD_DEVID_ASSOCIATION
    U8      reserved1           : 1;
    U8      piv                 : 1;
    U8      reserved2;
    U8      identifierLength;
    U8      identifier[0];
} SCSI_VPD_DEVID;

typedef struct _SCSI_INQUIRY_VPD_LOGICAL_BLK_PROV {
    uchar   thresholdExponent;      // byte 4
    uchar   dp              : 1;    // byte 5
    uchar   anc_sup         : 1;
    uchar   lbprz           : 1;
    uchar   reserved5       : 2;
    uchar   lbpws10         : 1;
    uchar   lbpws           : 1;
    uchar   lbpu            : 1;
    uchar   provisioningType: 3;// byte 6
    uchar   reserved6       : 5;
    uchar   reserved7;              // byte 7
} SCSI_INQUIRY_VPD_LOGICAL_BLK_PROV;
#define VPD_IDENTIFIER_LENGTH 16        //16 bytes is a TYPICAL size but SCSI does not limit size here
typedef struct _SCSI_VPD_DESC {
    U8      codeSet             : 4;    // SCSI_VPD_DEVID_CODE_SET
    U8      protocolIdentifier  : 4;    // SCSI_VPD_DEVID_PROTOCOL
    U8      identifierType      : 4;    // SCSI_VPD_DEVID_TYPE
    U8      association         : 2;    // SCSI_VPD_DEVID_ASSOCIATION
    U8      reserved1           : 1;
    U8      piv                 : 1;
    U8      reserved2;
    U8      identifierLength;
    U8      identifier[VPD_IDENTIFIER_LENGTH];
} SCSI_VPD_DESC;
/**
 * Define VPD DeviceId code set
 */
typedef enum _SCSI_VPD_DEVID_CODE_SET {
    SCSI_VPD_DEVID_CODE_SET_RESERVED    = 0,
    SCSI_VPD_DEVID_CODE_SET_BINARY      = 1,
    SCSI_VPD_DEVID_CODE_SET_ASCII       = 2,
    SCSI_VPD_DEVID_CODE_SET_UTF8        = 3
} SCSI_VPD_DEVID_CODE_SET;

/**
 * Define SCSI VPD DeviceId protocol identifier
 */
typedef enum _SCSI_VPD_DEVID_PROTOCOL {
    SCSI_VPD_DEVID_PROTOCOL_FIBRE_CHANNEL       = 0,    //  FCP-2
    SCSI_VPD_DEVID_PROTOCOL_PARALLEL_SCSI       = 1,    // SPI-5
    SCSI_VPD_DEVID_PROTOCOL_SSA                 = 2,    // SSA-S3P
    SCSI_VPD_DEVID_PROTOCOL_1394                = 3,    // SBP-3
    SCSI_VPD_DEVID_PROTOCOL_SCSIRdma            = 4,    // RemoteDirectMemoryAccess SRP
    SCSI_VPD_DEVID_PROTOCOL_iSCSI               = 5,    // Internet SCSI
    SCSI_VPD_DEVID_PROTOCOL_SAS                 = 6,    // SerialSCSI
    SCSI_VPD_DEVID_PROTOCOL_ADT                 = 7,    // Automation/Drive Interface Transport Protocol
    SCSI_VPD_DEVID_PROTOCOL_ATAPI               = 8,    // ATA Packet Interface  ATA/ATAPI-7
    SCSI_VPD_DEVID_PROTOCOL_Reserved1           = 9,
    SCSI_VPD_DEVID_PROTOCOL_Reserved2           = 10,
    SCSI_VPD_DEVID_PROTOCOL_Reserved3           = 11,
    SCSI_VPD_DEVID_PROTOCOL_Reserved4           = 12,
    SCSI_VPD_DEVID_PROTOCOL_Reserved5           = 13,
    SCSI_VPD_DEVID_PROTOCOL_Reserved6           = 14,
    SCSI_VPD_DEVID_PROTOCOL_NoSpecificProtocol = 15
} SCSI_VPD_DEVID_PROTOCOL;

/**
 * Define SCSI VPD DeviceId Type
 */
typedef enum _SCSI_VPD_DEVID_TYPE {
    SCSI_VPD_DEVID_TYPE_VENDOR_SPECIFIC     = 0,
    SCSI_VPD_DEVID_TYPE_VENDOR_ID           = 1,
    SCSI_VPD_DEVID_TYPE_EUI64               = 2,
    SCSI_VPD_DEVID_TYPE_NAA                 = 3,
    SCSI_VPD_DEVID_TYPE_PORT_RELATIVE       = 4,
    SCSI_VPD_DEVID_TYPE_TARGET_PORT_GROUP   = 5,
    SCSI_VPD_DEVID_TYPE_LOGICAL_UNIT_GROUP  = 6,
    SCSI_VPD_DEVID_TYPE_MD5                 = 7,
    SCSI_VPD_DEVID_TYPE_SCSI                = 8
} SCSI_VPD_DEVID_TYPE;

/**
 * Define SCSI VPD DeviceId Association
 */
typedef enum _SCSI_VPD_DEVID_ASSOCIATION {
    SCSI_VPD_DEVID_ASSOCIATION_LUN          = 0,
    SCSI_VPD_DEVID_ASSOCIATION_TARGET_PORT  = 1,
    SCSI_VPD_DEVID_ASSOCIATION_TARGET_DEVICE= 2,
    SCSI_VPD_DEVID_ASSOCIATION_RESERVED     = 3
} SCSI_VPD_DEVID_ASSOCIATION;

/**
 * Define SCSI VPD relative port identifiers
 */
typedef enum _SCSI_VPD_RELATIVE_PORT {
    SCSI_VPD_RELATIVE_PORT_RESERVED     = 0,
    SCSI_VPD_RELATIVE_PORT_A            = 1,
    SCSI_VPD_RELATIVE_PORT_B            = 2
} SCSI_VPD_RELATIVE_PORT;

/**
 * Define SCSI INQUIRY VPD data format
 */
#define SCSI_VPD_BLOCK_LIMIT_PAGE_LENGTH 60 /* Block Limit VPD Page Page Length is 60 bytes. */
typedef struct _SCSI_INQUIRY_VPD {
    uint    peripheralDeviceType :  5;      /* byte 0 */
    uint    peripheralQualifier  :  3;
    uchar   pageCode;                       /* byte 1 */
    uchar   reserved;                       /* byte 2 */
    uchar   pageLength;                     /* byte 3 */

    union {
        struct {                            // page 0x00
            uchar   supportedPageList[0];
        } supportedPages;

        struct {                            // page 0x80
            uchar   sn[0];
        } serialNumber;

        SCSI_VPD_DEVID   deviceIdentification;   // page 0x83

        struct _SCSI_INQUIRY_VPD_EXTENDED { // page 0x86
            uchar   ref_chk         : 1;    // byte 4
            uchar   app_chk         : 1;
            uchar   grd_chk         : 1;
            uchar   spt             : 3;
            uchar   reserved4       : 2;

            uchar   simpsup         : 1;    // byte 5
            uchar   ordsup          : 1;
            uchar   headsup         : 1;
            uchar   prior_sup       : 1;
            uchar   group_sup       : 1;
            uchar   uask_sup        : 1;
            uchar   reserved5       : 2;

            uchar   v_sup           : 1;    // byte 6
            uchar   nv_sup          : 1;
            uchar   crd_sup         : 1;
            uchar   wu_sup          : 1;
            uchar   reserved6       : 4;

            uchar   luiclr          : 1;    // byte 7
            uchar   reserved7_1     : 3;
            uchar   p_i_i_sup       : 1;
            uchar   reserved7_2     : 3;

            uchar   cbcs            : 1;    // byte 8
            uchar   reserved8       : 7;

            uchar   multi_download  : 4;    // byte 9
            uchar   reserved9       : 4;
        } extended;

        SCSI_INQUIRY_VPD_LOGICAL_BLK_PROV logicalBlkProv;
        struct _SCSI_INQUIRY_VPD_BLOCK_LIMITS {
            uchar   wsnz                        : 1;
            uchar   reserved                    : 7;    // byte 4
            uchar   maxCompareAndWriteLength;           // byte 5
            ushort  optimalTransferLengthGranularity;   // byte 6,7
            uint    maxTransferLength;                  // byte 8  - 11
            uint    optimalTransferLength;              // byte 12 - 15
            uint    maxPrefetchLength;                  // byte 16 - 19
            uint    maxUnmapLBACount;                   // byte 20 - 23
            uint    maxUnmapBlockDescCount;             // byte 24 - 27
            uint    optimalUnmapGranularity;            // byte 28 - 31
            uint    unmapGranularityAlignment   : 31;
            uint    ugavalid                    : 1;    // byte 32 - 35
            uint    maxWriteSameLength[2];              // byte 36 - 43
            uchar   reserved2[20];
        } blockLimits;
    } page;

} SCSI_INQUIRY_VPD, *PSCSI_INQUIRY_VPD;


/**
 * Define SCSI READ_CAPACITY(10) data structure
 */
typedef struct _SCSI_READ_CAPACITY10_DATA {
    U32     LogicalBlockAddress;
    U32     BytesPerBlock;
} SCSI_READ_CAPACITY10_DATA;
/**
 * Define SCSI inq BLKS LIMIT B0 page data sturcture
 */
typedef struct _SCSI_BLKS_LIMIT_DATA {
    uint    peripheralDeviceType            : 5;  // byte 0
    uint    peripheralQualifier             : 3;
    uchar   pageCode;                             // byte 1
    uint    pageLength                      : 16; // byte2 - byte 3
    uchar   wsnz                            : 1;
    uchar   reserved                        : 7;  // byte 4
    uchar   maxCompareAndWriteLength;             // byte 5
    ushort  optimalTransferLengthGranularity;     // byte 6,7
    uint    maxTransferLength;                    // byte 8  - 11
    uint    optimalTransferLength;                // byte 12 - 15
    uint    maxPrefetchLength;                    // byte 16 - 19
    uint    maxUnmapLBACount;                     // byte 20 - 23
    uint    maxUnmapBlockDescCount;               // byte 24 - 27
    uint    optimalUnmapGranularity;              // byte 28 - 31
    uint    unmapGranularityAlignment        : 31;
    uint    ugavalid                         : 1; // byte 32 - 35
    uint    maxWriteSameLength[2];                // byte 36 - 43
    uchar   reserved2[20];

} SCSI_BLK_LIMIT_DATA;

/**
 * Define SCSI inq LOGICAL BLK PROBV B2 page data sturcture
 */
typedef struct _SCSI_LOGICAL_BLK_PROV_DATA {
    uint    peripheralDeviceType : 5;  // byte 0
    uint    peripheralQualifier  : 3;
    uchar   pageCode;                  // byte 1
    uint    pageLength           : 16; // byte2 - byte 3
    uchar   threaholdExponent;         // byte 4
    uchar   dp                   : 1;  // byte 5
    uchar   anc_sup              : 1;
    uchar   lbprz                : 1;
    uchar   reserved5            : 2;
    uchar   lbpws10              : 1;
    uchar   lbpws                : 1;
    uchar   lbpu                 : 1;
    uchar   provisioningType     : 3;  // byte 6
    uchar   reserved6            : 5;
    uchar   reserved7;                 // byte 7
} SCSI_LOGICAL_BLK_PROV_DATA;

/**
 * Define SCSI READ_CAPACITY(16) data sturcture
 */
typedef struct _SCSI_READ_CAPACITY16_DATA {
    U64     LogicalBlockAddress;
    U32     BytesPerBlock;
    union {
        U32 w;
        struct {
            U32     prot_en             : 1;
            U32     p_type              : 3;
            U32     reserved1           : 4;
            U32     logicalBlocksPerPhysicalBlockExponent    : 4;
            U32     p_i_exponent        : 4;
            U32     lowestAlignedLogicalBlockAddress_H   : 6;
            U32     lbprz               : 1;
            U32     lbpme               : 1;
            U32     lowestAlignedLogicalBlockAddress_L   : 8;
        };
    };
    uchar   reserved[16];
} SCSI_READ_CAPACITY16_DATA;

/**
 * Define SCSI READ_CAPACITY(16) p_type field values.
 */
typedef enum _SCSI_READ_CAPACITY16_PTYPE {
    SCSI_READ_CAPACITY16_PTYPE_0            = 0,
    SCSI_READ_CAPACITY16_PTYPE_1            = 1,
    SCSI_READ_CAPACITY16_PTYPE_2            = 2,
    SCSI_READ_CAPACITY16_PTYPE_3            = 3,
    SCSI_READ_CAPACITY16_PTYPE_RESERVED4    = 4,
    SCSI_READ_CAPACITY16_PTYPE_RESERVED5    = 5,
    SCSI_READ_CAPACITY16_PTYPE_RESERVED6    = 6,
    SCSI_READ_CAPACITY16_PTYPE_RESERVED7    = 7,
} SCSI_READ_CAPACITY16_PTYPE;


/*
 * define the SCSI REPORT LUN data format
 */
typedef struct _LUN_ENTRY {
    U8  lun;
    U8  reserved[7];
} LUN_ENTRY;

typedef struct _LUN_LIST_HEADER {
    U32  lunListLen;
    U32  reserved;
} LUN_LIST_HEADER;

typedef struct _SCSI_REPORT_LUNS12_DATA {
    LUN_LIST_HEADER lunListHeader;
    LUN_ENTRY       lunList[MAX_LOGICAL_DRIVES];
} SCSI_REPORT_LUNS12_DATA;

/*
 *-----------------------------------------------------------------------------
 *
 * Define mode sense command and pages
 *
 *-----------------------------------------------------------------------------
 */

/**
 * Define MODE_SENSE(6) CDB structure
 */
typedef struct _SCSI_CDB_MODE_SENSE_6 {
    uchar   cmd;

    uchar   reserved1_0 : 3;
    uchar   dbd         : 1;
    uchar   reserved1_4 : 4;

    union {                 // pagecode byte (include pc)
        uchar   pageCodeByte;   // to allow byte access to the entire field
        struct {
            uchar   pageCode    : 6;
            uchar   pc          : 2;
        };
    };

    uchar   subPageCode;
    uchar   allocationLength;
    uchar   control;
} SCSI_CDB_MODE_SENSE_6;

/**
 * Define MODE_SENSE(10) CDB structure
 */
typedef struct _SCSI_CDB_MODE_SENSE_10 {
    uchar   cmd;

    uchar   reserved1_0 : 3;
    uchar   dbd         : 1;
    uchar   llbaa       : 1;
    uchar   reserved1_4 : 3;

    union {                 // pagecode byte (include pc)
        uchar   pageCodeByte;   // to allow byte access to the entire field
        struct {
            uchar   pageCode    : 6;
            uchar   pc          : 2;
        };
    };

    uchar   subPageCode;
    uchar   reserved4[3];
    ushort  allocationLength;
    uchar   control;
} _PACKED SCSI_CDB_MODE_SENSE_10;

/**
 * Define MODE_SELECT(6) CDB structure
 */
typedef struct _SCSI_CDB_MODE_SELECT_6 {
    uchar   cmd;

    uchar   sp          : 1;
    uchar   reserved1_1 : 3;
    uchar   pf          : 1;
    uchar   reserved1_5 : 3;

    uchar   reserved2[2];
    uchar   parameterListLength;
    uchar   control;
} SCSI_CDB_MODE_SELECT_6;

/**
 * Define MODE_SELECT(10) CDB structure
 */
typedef struct _SCSI_CDB_MODE_SELECT_10 {
    uchar   cmd;

    uchar   sp          : 1;
    uchar   reserved1_1 : 3;
    uchar   pf          : 1;
    uchar   reserved1_5 : 3;

    uchar   reserved2[5];
    ushort  parameterListLength;
    uchar   control;
} _PACKED SCSI_CDB_MODE_SELECT_10;

/**
 * Define SCSI mode page control field
 */
typedef enum _SMP_PAGECONTROL {
    SMP_PAGECONTROL_CURRENT     = 0x00,
    SMP_PAGECONTROL_CHANGEABLE  = 0x40,
    SMP_PAGECONTROL_DEFAULT     = 0x80,
    SMP_PAGECONTROL_SAVED       = 0xC0,
} SMP_PAGECONTROL;

/**
 * Define the SCSI mode pages we use
 */
typedef enum _SMP_PAGECODE {
    SMP_PAGECODE_RECOVERY_PAGE          = 0x01,
    SMP_PAGECODE_CACHING_PAGE           = 0x08,
    SMP_PAGECODE_CONTROL_PAGE           = 0x0A,
    SMP_PAGECODE_PSP_PAGE               = 0x19,
    SMP_PAGECODE_POWER_CONDITION_PAGE   = 0x1A,
    SMP_PAGECODE_IEC_PAGE               = 0x1C,
    SMP_PAGECODE_ALL                    = 0x3F,
} SMP_PAGECODE;

/**
 * Define SCSI mode subpage values
 */
typedef enum _SMP_SUBPAGE {
    SMP_SUBPAGE_ZERO                        = 0,
    SMP_SUBPAGE_PSP_SAS_PHY_CONTROL         = 0x01,
    SMP_SUBPAGE_PSP_SAS_SHARED_PORT_CONTROL = 0x02,
    SMP_SUBPAGE_PSP_SAS_ENHANCED_PHY_CONTROL= 0x03,
} SMP_SUBPAGE;


/**
 * Define the device-specific parameter byte for different device types
 */
typedef union _SCSI_MODE_DATA_DEVICE_SPECIFIC_PARAMETER {
    uchar   b;          // to allow full-byte access

    struct {            // define for block devices
        uchar   reserved0_0 : 4;
        uchar   dpofua      : 1;
        uchar   reserved0_5 : 2;
        uchar   wp          : 1;
    } block;

    struct {
        uchar   reserved;   // field is reserved for SES devices
    } ses;
} SCSI_MODE_DATA_DEVICE_SPECIFIC_PARAMETER;

/**
 * Define SCSI MODE(6) SENSE/SELECT data structures
 */
typedef struct _SCSI_MODE_6_DATA {
    struct  {                           /* mode parameter header */
        uchar   modeDataLength;         /* mode data length */
        uchar   mediumType;             /* medium type */
        SCSI_MODE_DATA_DEVICE_SPECIFIC_PARAMETER    deviceSpecificParameter;/* device specific parameter */
        uchar   blockDescriptorLength;  /* block descriptor length */
    } mph;
    uchar   data[256];                  /* data buffer */
} SCSI_MODE_6_DATA;

/**
 * Define SCSI MODE(10) SENSE/SELECT data structures
 */
typedef struct _SCSI_MODE_10_DATA {
    struct  {                           /* mode parameter header */
        ushort  modeDataLength;         /* mode data length */
        uchar   mediumType;             /* medium type */
        SCSI_MODE_DATA_DEVICE_SPECIFIC_PARAMETER    deviceSpecificParameter;/* device specific parameter */
        ushort  reserved;
        ushort  blockDescriptorLength;  /* block descriptor length */
    } mph;
    uchar   data[512];                  /* data buffer */
} _PACKED SCSI_MODE_10_DATA;

typedef struct _SCSI_UNMAP_BLK_DESC {
    U64     lba;
    U32     noOfLogicalBlks;
    U32     reserved;
}_PACKED SCSI_UNMAP_BLK_DESC;

/**
 * Define SCSI MODE(10) SENSE/SELECT data structures
 */
typedef struct _SCSI_UNMAP_DATA {
    U16  unmapDataLength;              /* mode data length */
    U16  blockDescriptorLength;        /* block descriptor length */
    U32  reserved;
    SCSI_UNMAP_BLK_DESC   data[1];   /* data buffer */
} _PACKED SCSI_UNMAP_DATA;
/**
 * Define structures for generic mode sense data
 */
typedef struct _SCSI_MODE_PARAMS_BLOCK_DESC {
    uint    numBlocks;                  /* Number of blocks in LD */
    uint    reserved4;
    uchar   blockLenHi;                 /* MS byte of block lenghth in bytes */
    ushort  blockLenLo;                 /* LS word of block lenghth in bytes */
} _PACKED SCSI_MODE_PARAMS_BLOCK_DESC;

/**
 * Define SCSI mode page header
 */
typedef struct _SCSI_MODE_PAGE_HEADER {
    union {
        struct {
            uchar   pageCode    : 6;            // Page Code
            uchar   spf         : 1;
            uchar   ps          : 1;
        };
        uchar   pageCodeByte;           // accessd to full page code byte, including bit fields
    };
    uchar   pageLength;                 // Page length (excluding this header)
} SCSI_MODE_PAGE_HEADER;

/**
 * Define SCSI mode sub-page header
 */
typedef struct _SCSI_MODE_SUBPAGE_HEADER {
    union {
        struct {
            uchar   pageCode    : 6;            // Page Code
            uchar   spf         : 1;
            uchar   ps          : 1;
        };
        uchar   pageCodeByte;           // accessd to full page code byte, including bit fields
    };
    uchar   subPageCode;
    ushort  pageLength;                 // Page length (excluding this header)
} _PACKED SCSI_MODE_SUBPAGE_HEADER;

/**
 * Define SCSI recovery mode page structure
 */
typedef struct _SCSI_MODE_PAGE_RECOVERY {
    SCSI_MODE_PAGE_HEADER   hdr;        /* The header */

    uchar   dcr     : 1;
    uchar   dte     : 1;
    uchar   per     : 1;
    uchar   eer     : 1;
    uchar   rc      : 1;
    uchar   tb      : 1;
    uchar   arre    : 1;
    uchar   awre    : 1;

    uchar   readRetryCount;
    uchar   obsolete4;
    uchar   obsolete5;
    uchar   obsolete6;

    uchar   restrictedForMmc6   : 2;
    uchar   reserved7_2         : 5;
    uchar   lbpere              : 1;

    uchar   writeRetryCount;
    uchar   reserved9;
    ushort  recoveryTimeLimit;
} SCSI_MODE_PAGE_RECOVERY;

/**
 * Define SCSI caching mode page byte2 bits.
 */
typedef union _SCSI_MODE_PAGE_CACHING_CONTROL_FLAGS {
    uchar   b;          // permit full byte access to this field
    struct {
        uchar   rcd     : 1;
        uchar   mf      : 1;
        uchar   wce     : 1;
        uchar   size    : 1;
        uchar   disc    : 1;
        uchar   cap     : 1;
        uchar   abpf    : 1;
        uchar   ic      : 1;
    };
} SCSI_MODE_PAGE_CACHING_CONTROL_FLAGS;

/**
 * Define SCSI caching mode page structure
 */
typedef struct _SCSI_MODE_PAGE_CACHING_t {
    SCSI_MODE_PAGE_HEADER   hdr;        /* The header */

    SCSI_MODE_PAGE_CACHING_CONTROL_FLAGS    cf; // control flags

    uchar   writeRetentionPriority      : 4;
    uchar   demandReadRetentionPriority : 4;
    ushort  disablePreFetchTransferLength;
    ushort  minPreFetch;
    ushort  maxPreFetch;
    ushort  maxPreFetchCeiling;

    /* Control flags */
    uchar   nv_dis          : 1;
    uchar   sync_prog       : 2;
    uchar   vendorSpecific  : 2;
    uchar   dra             : 1;
    uchar   lbcss           : 1;
    uchar   fsw             : 1;

    uchar   numCacheSeg;                /* Number of cache segments */
    ushort  cacheSegSize;               /* Size of each Cache segment */
    uchar   rsvd;                       /* Reserved */
    uchar   obsolete[3];
} _PACKED SCSI_MODE_PAGE_CACHING_t;

/**
 * Define SCSI CONTROL mode page structures
 */
typedef struct _SCSI_MODE_PAGE_CONTROL {
    SCSI_MODE_PAGE_HEADER   hdr;        // mode page header

    uchar   rlec        : 1;
    uchar   gltsd       : 1;
    uchar   d_sense     : 1;
    uchar   dpicz       : 1;
    uchar   tmf_only    : 1;
    uchar   tst         : 3;

    uchar   obsolete3_0 : 1;
    uchar   qerr        : 2;
    uchar   nuar        : 1;
    uchar   queueAlgorithmModifier  : 4;

    uchar   obsolete4_0 : 3;
    uchar   swp         : 1;
    uchar   ua_intlck_ctrl : 2;
    uchar   rac         : 1;
    uchar   vs          : 1;

    uchar   autoloadMode: 3;
    uchar   reserved5_3 : 1;
    uchar   rwwp        : 1;
    uchar   atmpe       : 1;
    uchar   tas         : 1;
    uchar   ato         : 1;

    ushort  obsolete6;
    ushort  busyTimeoutPeriod;
    ushort  extendedSelfTestCompletionTime;
} _PACKED SCSI_MODE_PAGE_CONTROL;

/**
 * Define SCSI IEC mode page structure
 */
typedef struct _SCSI_MODE_PAGE_IEC {
    SCSI_MODE_PAGE_HEADER   hdr;        // mode page header

    uchar   logerr      : 1;
    uchar   ebackerr    : 1;
    uchar   test        : 1;
    uchar   dexcpt      : 1;
    uchar   ewasc       : 1;
    uchar   ebf         : 1;
    uchar   reserved2_6 : 1;
    uchar   perf        : 1;

    uchar   mrie        : 4;
    uchar   reserved3   : 4;

    uint    intervalTimer;
    uint    reportCount;
} _PACKED SCSI_MODE_PAGE_IEC;

/**
 * Define SCSI PSP SAS mode page structure
 */
typedef struct _SCSI_MODE_PAGE_PSP_SAS {
    SCSI_MODE_PAGE_HEADER   hdr;        // mode page header

    uchar   protocolIdentifier  : 4;
    uchar   readyLEDmeaning     : 1;
    uchar   broadcastAsyncEvent : 1;
    uchar   continueAWT         : 1;
    uchar   reserved2_7         : 1;

    uchar   reserved3;
    ushort  itNexusLossTime;
    ushort  initiatorResponseTime;
    ushort  rejectToOpenLimit;
    uchar   reserved10[6];
} _PACKED SCSI_MODE_PAGE_PSP_SAS;

/**
 * Define SCSI PSP SAS subpage 1 mode page structure
 */
typedef struct _SCSI_MODE_SUBPAGE_PSP_SAS_PHY_CONTROL {
    SCSI_MODE_SUBPAGE_HEADER    hdr;        // mode page header

    uchar   reserved4;

    uchar   protocolIdentifier  : 4;
    uchar   reserved5           : 4;

    uchar   generationCode;
    uchar   numberOfPhys;

    struct _SCSI_MODE_SUBPAGE_PSP_SAS_PHY_CONTROL_PHY_DESCRIPTOR {
        uchar   reserved0;
        uchar   phyIdentifier;
        ushort  reserved2;

        uchar   attachedReason      : 4;
        uchar   attachedDeviceType  : 3;
        uchar   reserved4_7         : 1;

        uchar   negotiatedLogicalLinkRate   : 4;
        uchar   reason              : 4;

        struct {
            uchar   reserved0       : 1;
            uchar   attachedSmpPort : 1;
            uchar   attachedStpPort : 1;
            uchar   attachedSspPort : 1;
            uchar   reserved4       : 4;
        } initiator, target;

        uint64  sasAddress;
        uint64  attachedSasAddress;
        uchar   attachedPhyIdentifier;

        uchar   attachedBreakReplyCapable       : 1;
        uchar   attachedRequestedInsideZpsds    : 1;
        uchar   attachedInsideZpsdsPersistent   : 1;
        uchar   attachedPartialCapable          : 1;
        uchar   attachedSlumberCapable          : 1;
        uchar   attachedPowerCapable            : 2;
        uchar   reserved25_7                    : 1;

        uchar   reserved26[6];

        struct {
            uchar   hwPhysicalLinkRate          : 4;
            uchar   programmedPhysicalLinkRate  : 4;
        } minimum, maximum;

        uchar       reserved34[8];
        uchar       vendorSpecific[2];
        uchar       reserved44[4];
    } _PACKED phy[0];
} _PACKED SCSI_MODE_SUBPAGE_PSP_SAS_PHY_CONTROL;

/**
 * Define SCSI Disconnect-Reconnect mode page structure
 */
typedef struct _SCSI_MODE_PAGE_CONNECTION {
    SCSI_MODE_PAGE_HEADER   hdr;        // mode page header
    U8      Reserved0;
    U8      Reserved1;
    U8      BusInactiveTime[2];
    U8      Reserved2[2];
    U8      MaxConnectTime[2];
    U8      MaxBurstSize[2];
    U8      Reserved3[2];
    U8      FirstBurstSize[2];
} _PACKED SCSI_MODE_PAGE_CONNECTION;

/**
 * Define SCSI Verify Error Recovery mode page structure
 */
typedef struct _SCSI_MODE_PAGE_VERIFY_ERROR_RECOVERY {
    SCSI_MODE_PAGE_HEADER   hdr;        // mode page header

    U8      dcr         : 1;
    U8      dte         : 1;
    U8      per         : 1;
    U8      eer         : 1;
    U8      reserved2_4 : 4;

    U8      verifyRetryCount;
    U8      obsolete;
    U8      reserved5[5];
    U16     verifyRecoveryTimeLimit;
} _PACKED SCSI_MODE_PAGE_VERIFY_ERROR_RECOVERY;

/**
 * Define SCSI Protocol specific LUN mode page structure
 */
typedef struct _SCSI_MODE_PAGE_PROTOCOL_LU {
    SCSI_MODE_PAGE_HEADER   hdr;        // mode page header
    U8      ProtocolId              : 4;
    U8      transportLayerRetries   : 1;
    U8      Reserved0               : 3;
    U8      Reserved1[5];
} _PACKED SCSI_MODE_PAGE_PROTOCOL_LU;

/**
 * Define SCSI Shared Port Control mode page structure
 */
typedef struct _SCSI_MODE_SUBPAGE_PSP_SHARED_PORT_CONTROL {
    SCSI_MODE_SUBPAGE_HEADER    hdr;        // mode page header
    U8      Reserved0;

    U8      ProtocolId  : 4;
    U8      Reserved1   : 4;

    U16     powerLossTimeout;
    U8      reserved8;
    U8      powerGrantTimeout;
    U8      Reserved[6];
} _PACKED SCSI_MODE_SUBPAGE_PSP_SHARED_PORT_CONTROL;

/**
 * Define SCSI Power Condition mode page structure
 */
typedef struct _SCSI_MODE_PAGE_POWER_CONDITION {
    SCSI_MODE_PAGE_HEADER   hdr;        // mode page header
    U8      standby_y       : 1;
    U8      reserved2_1     : 5;
    U8      pm_bg_precedence: 2;

    U8      standby_z       : 1;
    U8      idle_a          : 1;
    U8      idle_b          : 1;
    U8      idle_c          : 1;
    U8      reserved3_4     : 4;

    U32     idle_a_timer;
    U32     standby_z_timer;
    U32     idle_b_timer;
    U32     idle_c_timer;
    U32     standby_y_timer;
    U8      Reserved24[15];

    U8      reserved39_0    : 2;
    U8      ccf_stopped     : 2;
    U8      ccf_standby     : 2;
    U8      ccf_idle        : 2;
}   SCSI_MODE_PAGE_POWER_CONDITION;


/*
 *-----------------------------------------------------------------------------
 *
 * Define log page structures
 *
 *-----------------------------------------------------------------------------
 */

/**
 * Define the SCSI log page header structure
 */
typedef struct _SCSI_LOG_PAGE_HDR {     /* log page header */
    uchar   pageCode;
    uchar   reserved;
    ushort  pageLength;
} SCSI_LOG_PAGE_HDR;

/**
 * Define SCSI log parameter header structure
 */
typedef struct _SCSI_LOG_PARAMETER_HDR {     /* log parameter header */
    ushort  parameterCode;
    uchar   lp      : 1;
    uchar   lbin    : 1;
    uchar   tmc     : 2;
    uchar   etc     : 1;
    uchar   tsd     : 1;
    uchar   ds      : 1;
    uchar   du      : 1;
    uchar   parameterLength;
} SCSI_LOG_PARAMETER_HDR;

/**
 * Define SCSI log parameters 10 structure
 */
typedef struct _SCSI_LOG_PARAMETERS_10 {     /* log parameters for self test results page(0x10) */
    uchar   selfTestResults : 4;
    uchar   reserved1       : 1;
    uchar   selfTestCode    : 3;
    uchar   selfTestNumber;
    ushort  timeStamp;
    uchar   addrOfFirstFailure[8];
    uchar   senseKey        : 4;
    uchar   reserved2       : 4;
    uchar   additionalsenseCode;
    uchar   additionalSenseQualifier;
    uchar   vendorSepcific;
} SCSI_LOG_PARAMETERS_10;

/**
 * Define SCSI log page 10 structure
 */
typedef struct _SCSI_LOG_PAGE_10 {      /* log page 10 format */
    SCSI_LOG_PAGE_HDR       hdr;
    SCSI_LOG_PARAMETER_HDR  hdr1;
    SCSI_LOG_PARAMETERS_10  params[20];
} SCSI_LOG_PAGE_10;

/**
 * Define log sense page codes
 */
typedef enum _SCSI_LOG_SENSE_PAGE {
    SCSI_LOG_SENSE_PG_CODE_MASK                   = 0x3f,
    SCSI_LOG_SENSE_PG_TEMPERATURE                 = 0x0d,
    SCSI_LOG_SENSE_PG_SELF_TEST_LOG               = 0x10,
    SCSI_LOG_SENSE_PG_INFORMATIONAL_EXCEPTIONS    = 0x2f,
} SCSI_LOG_SENSE_PAGE;


/*
 ******************************************************************************
 ******************************************************************************
 *
 * Define structures for SCSI commands
 *
 ******************************************************************************
 ******************************************************************************
 */

/**
 * Define SCSI IO flags for R/W opcodes
 */
typedef union _SCSI_RW_IOFLAGS {
    uchar   b;          // to allow for byte manipulation
    struct {
        uchar   obsolete    : 1;
        uchar   fua_nv      : 1;
        uchar   rarc        : 1;
        uchar   fua         : 1;
        uchar   dpo         : 1;
        uchar   rwProtect   : 3;    // SCSI_RWPROTECT - protect fields for read/write (i.e DIF/PI)
    };
    struct {
        uchar   ndob_reserved   : 1;
        uchar   lbdata          : 1;
        uchar   pbdata          : 1;
        uchar   unmap           : 1;
        uchar   anchor          : 1;
        uchar   wrprotect       : 3;
    };     //unmap flags for write same command
} SCSI_RW_IOFLAGS;

/**
 * Define SCSI RW_PROTECT fields for read/write DIF/PI support
 */
typedef enum    _SCSI_RWPROTECT {
        SCSI_RWPROTECT_STRIP                   = 0,    // Drive removes PI fields
        SCSI_RWPROTECT_CHECK_ALL               = 1,    // Drive checks all PI fields
        SCSI_RWPROTECT_CHECK_APP_REF           = 2,    // Drive checks application and reference tags
        SCSI_RWPROTECT_CHECK_NONE              = 3,    // Drive transfers PI fields without any checking
        SCSI_RWPROTECT_CHECK_GUARD             = 4,    // Drive checks only Guard field
        SCSI_RWPROTECT_CHECK_ALL_MAY_REF       = 5,    // Drive may check Reference tag during write
        SCSI_RWPROTECT_RESERVED6               = 6,
        SCSI_RWPROTECT_RESERVED7               = 7,
} SCSI_RWPROTECT;

#define SCSI_APP_TAG_MASK_ALL                (0xFFFF)    // Check or replace all bits of Application Tag



#define ScsiCmdIsReadIo(_cdb)  ((_cdb & 0xF) == 0x8)
#define ScsiCmdIsWriteIo(_cdb) ((_cdb & 0xF) == 0xA && _cdb!=SC_MODE_SENSE && _cdb!=SC_MODE_SENSE_6)
#define ScsiCmdIsRWIo(_cdb) (ScsiCmdIsReadIo(_cdb) || ScsiCmdIsWriteIo(_cdb))

/**
 * Define SCSI CDB format for READ(6)/WRITE(6) commands
 */
typedef struct _SCSI_RW_6 {
    uchar   cmd;
    uint    lba    : 24;
    uchar   count;
    uchar   control;
} _PACKED SCSI_RW_6;

/**
 * Define SCSI CDB format for Read(10)/WRITE(10) commands
 */
typedef struct _SCSI_RW_10 {
    uchar   cmd;
    SCSI_RW_IOFLAGS ioFlags;
    uint    lba;
    uchar   resv6;
    ushort  count;
    uchar   control;
} _PACKED SCSI_RW_10;

/**
 * Define SCSI CDB format for Read(12)/WRITE(12) commands
 */
typedef struct _SCSI_RW_12 {
    uchar   cmd;
    SCSI_RW_IOFLAGS ioFlags;
    uint    lba;
    uint    count;
    uchar   resv10;
    uchar   control;
} _PACKED SCSI_RW_12;

/**
 * Define SCSI CDB format for Read(16)/WRITE(16) commands
 */
typedef struct _SCSI_RW_16 {
    uchar   cmd;
    SCSI_RW_IOFLAGS ioFlags;
    uint64  lba;
    uint    count;
    uchar   resv14;
    uchar   control;
} _PACKED SCSI_RW_16;

/**
 * Define SCSI CDB format for Read(32)/WRITE(32) commands
 */
typedef struct _SCSI_RW_32 {
    uchar   cmd;
    uchar   control;
    uchar   reserved2_5[4];
    uchar   groupNumber;
    uchar   additionalCdbLen;
    ushort  serviceAction;
    SCSI_RW_IOFLAGS ioFlags;
    uchar   resv11;
    uint64  lba;
    uint    initRefTag;
    ushort  appTag;
    ushort  appTagMask;
    uint    count;
} _PACKED SCSI_RW_32;

/**
 * Define SCSI CDB format for READ_CAPACITY(10) command
 */
typedef struct _SCSI_READ_CAPACITY {
    uchar   cmd;
    uchar   resv1;
    uint    lba;
    uchar   resv6;
    uchar   resv7;
    uchar   pmi     : 1;
    uchar   resv8   : 7;
    uchar   control;
}   SCSI_READ_CAPACITY;

/**
 * Define SCSI CDB format for READ_CAPACITY(16) command
 */
typedef struct _SCSI_READ_CAPACITY_16 {
    uchar   cmd;
    uchar   service_action  : 5;
    uchar   resv1           : 3;
    uint64  lba;
    uint    allocationLength;
    uchar   pmi     : 1;
    uchar   resv14  : 7;
    uchar   control;
}   SCSI_READ_CAPACITY_16;

/**
 * Define SCSI CDB format for UNMAP command
 */
typedef struct _SCSI_UNMAP {
    uchar   cmd;
    uchar   anchor          : 1;
    uchar   resv1           : 7;
    uchar   resv2;
    uchar   resv3;
    uchar   resv4;
    uchar   resv5;
    uchar   groupNumber     :5;
    U16     paramListLength;
    uchar   control;
} _PACKED SCSI_UNMAP;
typedef struct _SCSI_WRITE_SAME_10 {
    uchar   cmd;
    SCSI_RW_IOFLAGS        unmapFlags;
    U32     lba;
    uchar   groupnumber     : 5;
    uchar   resvd6          : 3;
    U16     nooflogicalblks;
    uchar   control;
}   SCSI_WRITE_SAME_10;
typedef struct _SCSI_WRITE_SAME_16 {
        uchar   cmd;
        SCSI_RW_IOFLAGS        unmapFlags;
        U64     lba;
        U32     nooflogicalblks;
        uchar   groupnumber     : 5;
        uchar   resvd14         : 3;
        uchar   control;
}   SCSI_WRITE_SAME_16;
/**
 * Define SCSI CDB format for SECURITY PROTOCOL IN command
 */
typedef struct _SCSI_SECURITY_PROTOCOL_IN {
    uchar   cmd;
    uchar   securityProtocol;
    ushort  securityProtocolSpecific;
    uchar   reserved4   : 7;
    uchar   inc_512     : 1;
    uchar   reserved5;
    uint    allocationLength;
    uchar   reserved10;
    uchar   control;
}   SCSI_SECURITY_PROTOCOL_IN;

/**
 * Define SCSI CDB format for SECURITY PROTOCOL OUT command
 */
typedef struct _SCSI_SECURITY_PROTOCOL_OUT {
    uchar   cmd;
    uchar   securityProtocol;
    ushort  securityProtocolSpecific;
    uchar   reserved4   : 7;
    uchar   inc_512     : 1;
    uchar   reserved5;
    uint    transferLength;
    uchar   reserved10;
    uchar   control;
} SCSI_SECURITY_PROTOCOL_OUT;

/**
 * Define SCSI CDB format for WRITE_LONG(10) command
 */
typedef struct _SCSI_WRITE_LONG {
    uchar   cmd;
    uchar   obsolete    : 1;
    uchar   reserved1   : 4;
    uchar   pblock      : 1;
    uchar   wr_uncor    : 1;
    uchar   cor_dis     : 1;
    uint    lba;
    uchar   reserved6;
    ushort  transferLength;
    uchar   control;
}   SCSI_WRITE_LONG;

/**
 * Define a structure to define WRITE_LONG(16) "byte 1" fields.
 */
typedef union _SCSI_WRITE_LONG_BYTE1 {
    uchar   b;  // for byte access to this structure
    struct {
        uchar   serviceAction   : 5;
        uchar   pblock          : 1;
        uchar   wr_uncor        : 1;
        uchar   cor_dis         : 1;
    };
} SCSI_WRITE_LONG_BYTE1;

/**
 * Define SCSI CDB format for WRITE_LONG(16) command
 */
typedef struct _SCSI_WRITE_LONG_16 {
    uchar   cmd;
    SCSI_WRITE_LONG_BYTE1   byte1;
    uint64  lba;
    uchar   reserved10[2];
    ushort  transferLength;
    uchar   reserved14;
    uchar   control;
}   SCSI_WRITE_LONG_16;


/*
 *-----------------------------------------------------------------------------
 *
 * Define SCSI FORMAT command CDB structure and parameter structures
 */
/**
 * Define SCSI CDB format for FORMAT_UNIT command
 */
typedef struct _SCSI_FORMAT_UNIT {
    uchar   opcode;                     // Byte 0

    uchar   defectListFormat    : 3;    // Byte 1
    uchar   cmplst              : 1;
    uchar   fmtdata             : 1;
    uchar   longlist            : 1;
    uchar   fmtpinfo            : 2;

    uchar   vendoSpecific;              // Byte 2
    uchar   obsolete[2];                // Byte 3-4
    uchar   control;                    // Byte 5
} SCSI_FORMAT_UNIT;

/**
 * Define SCSI FORMAT_UNIT short parameter list header
 */
typedef struct _SCSI_FORMAT_UNIT_SHORT_PARAMETER_HEADER {
    uchar   protection_field_usage      : 3;    // Byte 0
    uchar   reserved0                   : 5;

    uchar   vendorSpecific              : 1;    // Byte 1
    uchar   immed                       : 1;
    uchar   obsolete                    : 1;
    uchar   ip                          : 1;
    uchar   stpf                        : 1;
    uchar   dcrt                        : 1;
    uchar   dpry                        : 1;
    uchar   fov                         : 1;

    ushort  defectListLength;
} SCSI_FORMAT_UNIT_SHORT_PARAMETER_HEADER;

/**
 * Define SCSI FORMAT_UNIT long parameter list header
 */
typedef struct _SCSI_FORMAT_UNIT_LONG_PARAMETER_HEADER {
    uchar   protection_field_usage      : 3;    // Byte 0
    uchar   reserved0                   : 5;

    uchar   vendorSpecific              : 1;    // Byte 1
    uchar   immed                       : 1;
    uchar   obsolete                    : 1;
    uchar   ip                          : 1;
    uchar   stpf                        : 1;
    uchar   dcrt                        : 1;
    uchar   dpry                        : 1;
    uchar   fov                         : 1;

    uchar   reserved2;                          // Byte 2

    uchar   protectionIntervalExponent  : 4;    // Byte 3
    uchar   p_i_information             : 4;

    uint    defectListLength;                   // Byte 4
} SCSI_FORMAT_UNIT_LONG_PARAMETER_HEADER;

/**
 * Define SCSI CDB format for REQUEST SENSE (6) command
 */
typedef struct _SCSI_REQUEST_SENSE {
    uchar   opcode;                             // Byte 0

    uchar   desc                        : 1;    // Byte 1
    uchar   reserved0                   : 7;

    uchar   reserved1;                          // Byte 2
    uchar   reserved2;                          // Byte 3
    uchar   allocationLength;                   // Byte 4
    uchar   control;                            // Byte 5
} _PACKED SCSI_REQUEST_SENSE;


/*
 *-----------------------------------------------------------------------------
 *
 * Define FDE feature structures
 *
 *-----------------------------------------------------------------------------
 */

typedef struct _FDE_GET_FEATURE_HDR {          // from byte 0 of returned buffer for "discovery level 0" cmd
    uint  LengthofParameterData;               // ttl # of bytes that are valid in the parameter = 2Ch
    uint  DatastructureRevision;               // ver# describes the format of the data returned = 00000001h
    uint  Reserved[2];
    uchar VendorVersion;                       // Vendor Specific field starts (follwoings are Seagate Data)
    uchar DeviceSecurityLifeCycleState;        // current value of the device security life cycle
    uchar Reserved_1;
    struct {
        uchar FDE     : 1;        // Full Disk Encryption supported
        uchar Locking : 1;        // LBA band locking supported
        uchar FDPL    : 1;        // Firmware download access control supported (Firmware Download PortLocking object)
        uchar DPL     : 1;        // Diagnostic port access control supported
        uchar UDSPL   : 1;        // UDS access control supported
        uchar CDPL    : 1;        // CHANGEDEF access control supported (PortLocking)
        uchar MB      : 1;        // Multiple Bands supported
        uchar MM      : 1;        // Multiple TCG Methods supported = 0
    } supported;
    uchar Reserved_2;
    struct {
        uchar FDE     : 1;        // Full Disk Encryption enabled
        uchar Locking : 1;        // LBA band locking enabled and locked
        uchar FDPL    : 1;        // Firmware download access control enabled
        uchar DPL     : 1;        // Diagnostic access control enabled
        uchar UDSPL   : 1;        // UDS access control enabled
        uchar CDPL    : 1;        // CHANGEDEF access control enabled
        uchar MB      : 1;        // Multiple Bands enabled
        uchar MM      : 1;        // Multiple TCG Methods enabled = 0
    } enabled;
    uchar Reserved_3[26];
} FDE_GET_FEATURE_HDR;


typedef struct _FDE_GET_FEATURE_DESCRIPTORS {     // from byte 48 of returned buffer for "discovery level 0" cmd
    ushort featureCode;
    uchar  reserved : 4;
    uchar  ver      : 4;
    uchar  len;
    union {
        struct {
            uchar syncSupported       : 1;      // TPer supports the Synchronous Protocol
            uchar ssynchSupported     : 1;      // TPer supports the Asynchronous Protocol
            uchar sckNakSupported     : 1;      // TPer supports transmission ACK/NAK flow control
            uchar bufferMgmtSupported : 1;      // TPer supports buffer management flow control
            uchar streamingSupported  : 1;      // TPer supports the streaming protocol
            uchar rsvd0               : 1;      //
            uchar comIDMgmtSupported  : 1;      // TPer supports ComID management using Protocol ID 02h
            uchar rsvd1               : 1;
        } TperFeature;
        struct {
            uchar lockingSupported : 1;     // TPer supports the Locking template
            uchar lockingEnabled   : 1;     // SP that incorporates the Locking template is in any state
            uchar locked           : 1;     // Write/ReadLockEnabled=True and Write/ReadLocked=True
            uchar mediaEncryption  : 1;     // TPer supports media encryption
            uchar MBREnabled       : 1;     // LockingEnabled==1 and MBRControl & MBR tables are implemented
            uchar MBRDone          : 1;     // MBREnabled is set to one, and the MBRControl table’s Done
            uchar rvd              : 2;
        } lockingFeature;
        uchar reserved[12];
    } fdeFeatureDate;
} FDE_GET_FEATURE_DESCRIPTORS;



/*
 ******************************************************************************
 *
 * define sectorformats with DIF/PI data
 *
 ******************************************************************************
 */

/**
 * Define SCSI DIF/PI structure
 */
typedef struct _SCSI_DIF {
    U16     crc;
    U16     appTag;
    U32     refTag;
} SCSI_DIF;

/**
 * Define SCSI 512 byte block with DIF/PI fields.
 */
typedef struct _SCSI_DIF_BLOCK_512 {
    uchar       data[512];
    SCSI_DIF    dif;
} SCSI_DIF_BLOCK_512;

/**
 * Define SCSI 4096 byte block with DIF/PI fields.
 */
typedef struct _SCSI_DIF_BLOCK_4K {
    uchar       data[4096];
    SCSI_DIF    dif;
} SCSI_DIF_BLOCK_4K;

#define SCSI_IU_TYPE_CMD        0x06
#define SCSI_IU_TYPE_TASK       0x16

#define SCSI_RESP_IU_SENSE_DATA_PRES           2
#define SCSI_RESP_IU_NO_DATA_PRES              0

/* Defines for TM */
#define SCSI_TMMGMT_TYPE_ABORT_TASK             0x01
#define SCSI_TMMGMT_TYPE_ABORT_TASK_SET         0x02
#define SCSI_TMMGMT_TYPE_ABORT_CLR_TASK_SET     0x04
#define SCSI_TMMGMT_TYPE_LUN_RESET              0x08
#define SCSI_TMMGMT_TYPE_QUERY_TASK             0x80

// SSP Frame TLR Controle values.
#define SSP_FRAME_TLR_CONTROL_SHIFT             3
#define SSP_FRAME_TLR_CONTROL_MASK              3
#define SSP_FRAME_TLR_CONTROL_ENABLE00          0
#define SSP_FRAME_TLR_CONTROL_ALLOW             1
#define SSP_FRAME_TLR_CONTROL_DISABLE           2
#define SSP_FRAME_TLR_CONTROL_ENABLE11          3

typedef struct _SCSI_CMD_IU {
    U64     LogicalUnitNumber;      // stored big endian - may need to swap
    U8      Reserved3;
    U8      TaskAttribute;          // lower 3 bits
    U8      Reserved4;
    U8      AdditionalCDBLength;
    U8      CDB[16];
} SCSI_CMD_IU;

typedef struct _SCSI_TASK_IU {
    U64     LogicalUnitNumber;      // stored big endian - may need to swap
    U16     Reserved1;
    U8      TaskManagementFunction;
    U8      Reserved2;
    U16     ManagedTaskTag;
    U16     Reserved3;
    U32     Reserved4[3];
} SCSI_TASK_IU;

// Task Management Response codes
typedef enum    _SCSI_TM_RESP_CODE {
    SCSI_TM_FUNC_COMPLETE         = 0x00,
    SCSI_TM_INVALID_FRAME         = 0x02,
    SCSI_TM_FUNC_NOT_SUPPORTED    = 0x04,
    SCSI_TM_FUNC_FAILED           = 0x05,
    SCSI_TM_FUNC_SUCCEEDED        = 0x08,
    SCSI_TM_INVALID_LUN           = 0x09,
} SCSI_TM_RESP_CODE;

#endif  /* INCLUDE_SCSI */

