/*
 * SLEVENT.H - Header file for Event management from API -- SCD Warhawk Specific
 *
 *     Copyright 2008-2014 Avago Technologies.  All rights reserved.
 * Written by Rajendra Singh
 */
#ifndef INCLUDE_SL_EVENT
#define INCLUDE_SL_EVENT

#include "event.h"

typedef enum _SL_EVENT_ARG_COUNT
{
    SL_EVENT_ARG_COUNT_ZERO,
    SL_EVENT_ARG_COUNT_ONE,
    SL_EVENT_ARG_COUNT_TWO,
    SL_EVENT_ARG_COUNT_THREE,
    SL_EVENT_ARG_COUNT_FOUR,
    SL_EVENT_ARG_COUNT_FIVE,
    SL_EVENT_ARG_COUNT_SIX,
    SL_EVENT_ARG_COUNT_SEVEN,
    SL_EVENT_ARG_COUNT_EIGHT,
    SL_EVENT_ARG_COUNT_NINE,
    SL_EVENT_ARG_COUNT_TEN
}SL_EVENT_ARG_COUNT;

/*
 * define the argument types -- WH Specific
 */
typedef enum _MR_EVT_WH_ARGS {
    MR_EVT_WH_ARGS_NONE,
    MR_EVT_WH_ARGS_SLOT_DRIVELIFE,
    MR_EVT_WH_ARGS_SLOT_THROTTLE,
    MR_EVT_WH_ARGS_TEMP_THRESHOLD,
    MR_EVT_WH_ARGS_POWER_THROTTLE,
    MR_EVT_WH_ARGS_DFF_TEMP_THRESHOLD,
    MR_EVT_WH_ARGS_DIAG_TRIGGER,
    MR_EVT_WH_ARGS_SSD_SECURITY,
    MR_EVT_WH_ARGS_CTRL_PFK,
    MR_EVT_WH_ARGS_REC_VOL
} MR_EVT_WH_ARGS;


/*
 * this structure describes the format of an event returned to the application
 */
typedef struct _MR_EVT_WH_DETAIL {
    U32                 seqNum;             // event sequence enumber
    U32                 timeStamp;          // seconds past Jan 1, 2000 (if FF in bits 24-32, then bits 0-23 are seconds since boot)
    U32                 code;               // event code
    MR_EVT_CLASS_LOCALE cl;                 // class/locale information for this event
    U8                  argType;            // argument type (MR_EVT_WH_ARGS)
    U8                  reserved1[15];      // reserved for future use

    /*
     * Define the arguments unions.  Each log event has a specific 'argument' type
     * which is used to reference this union.
     */
    union {

        struct
        {
            U16             Slot;
            U16             DriveLife;
            U16             WarningLevel;
            U16             ErrorLevel;
            MR_EVT_ARG_PD   Pd;
        } SlotDriveLife;

        struct
        {
            U16             Slot;
            U16             Throttle;
            MR_EVT_ARG_PD   Pd;
        } SlotThrottle;

        struct
        {
            U8              SensorNum;
            U16             ThresholdsExceeded;
            U16             Temperature;
        }WHTemperature;

        struct
        {
            U16             PercentThrottle;
            U16             PCISlotAvailPower;
        } Throttle;

        struct
        {
            U8              Flags;
            U8              SensorOrSlot;   /* SensorNumber or Slot Number, depending on flags value*/
            U8              CurrTemp;       /* Celcius */
            U8              ThresholdTemp;  /* Celcius */
            MR_EVT_ARG_PD   Pd;
        } Temperature;

        struct
        {
            U32 TriggerType;

            union 
            {
                U32 Master;

                struct 
                {
                    U16 EventValue;
                    U16 LogEntryQualifier;
                } Event;
                
                struct
                {
                    U8 ASCQ;
                    U8 ASC;
                    U8 SenseKey;
                    U8 Reserved;
                } Scsi;
                
                struct
                {
                    U16 IOCStatus;
                    U16 Reserved;
                    U32 IOCLogInfo;
                } Mpi;
            } u;
        } Trigger;

        struct
        {
            MR_EVT_ARG_PD   Pd;
            U64             Wwid;           /* World Wide Identifier */
            MR_EVT_ARG_LD   Ld;
        } PdWwidLd;                       /* IOPI_LOG_CODE_SSD_SECURITY_LOCKED */

        struct
        {
            U64             Wwid;           /* World Wide Identifier */
            MR_EVT_ARG_LD   Ld;
        } WwidLd;

        struct
        {
            MR_EVT_ARG_PD  Pd;
            U16            Reserved;          /* Reserved */
            U8             EnclIndex;         /* From SAS Device Page 0 */
            U8             SlotNumber;
        } PdArg;               

        struct
        {
            U16             Slot;            /*Slot Number of the card */
            U16             RomIdSNMismatch; /*If ROMID mismatch romIdSNMismatch = true, else romIdSNMismatch = false*/
            U16             SasAddrMismatch; /*If SASADDR mismatch sasAddrMismatch = true, else sasAddrMismatch = false*/	
        } pfkRomIdSasAddr;

        struct
        {
            U16 Slot;          /*Slot Number of the card */
            U16 LicenseActive; /* Set True if License Activation request successful */
        } pfkLicenseActive;    
        
        // alternate argument access via byte/short/word/dword
        U8          b[96];
        U16         s[48];
        U32         w[24];
        U64         d[12];

    } args;

    char    description[128];   // Null-terminated ASCII text description of event

    /* Size = 256 bytes */
} MR_EVT_WH_DETAIL;

/*
 * define event codes
 */
typedef enum _MR_EVT_WH {

    /* Do  not add anything here */

    MR_EVT_WH_THROTTLING_OCCURING                       = 0xFFFF,
    MR_EVT_WH_THROTTLING_REMOVED                        = 0xFFFE,
    MR_EVT_WH_SSD_LIFE_WARNING                          = 0xFFFD,
    MR_EVT_WH_SSD_LIFE_EXHAUSTED                        = 0xFFFC,
    MR_EVT_WH_BACKUP_RAIL_MONITOR_FAILURE               = 0xFFFB,
    MR_EVT_WH_TEMP_EXCEEDED_WARNING                     = 0xFFFA,
    MR_EVT_WH_TEMP_EXCEEDED_CRITICAL                    = 0xFFF9,
    MR_EVT_WH_POWER_THROTTLING_OCCURING                 = 0xFFF8,
    MR_EVT_WH_POWER_THROTTLING_REMOVED                  = 0xFFF7,
    MR_EVT_WH_DFF_TEMP_EXCEEDED_WARNING                 = 0xFFF6,
    MR_EVT_WH_DFF_TEMP_EXCEEDED_CRITICAL                = 0xFFF5,
    MR_EVT_WH_DIAGNOSTIC_TRIGGER_FIRED                  = 0xFFF4,
    MR_EVT_WH_SSD_SECURITY_LOCKED                       = 0xFFF3,
    MR_EVT_WH_SSD_SECURITY_UNLOCKED                     = 0xFFF2,
    MR_EVT_WH_CTRL_PFK_SOD_SASADDR_MISMATCH             = 0xFFF1,
    MR_EVT_WH_CTRL_PFK_SOD_ROMID_MISMATCH               = 0xFFF0,
    MR_EVT_WH_CTRL_PFK_LICENSE_APPLIED                  = 0xFFEF,
    MR_EVT_WH_CTRL_PFK_INVALID_LICENSE_KEY              = 0xFFEE,
    MR_EVT_WH_RECOVER_VOLUME_INITIATED                  = 0xFFED,
    MR_EVT_WH_RECOVER_VOLUME_SUCCESS                    = 0xFFEC,
    MR_EVT_WH_RECOVER_VOLUME_FAIL                       = 0xFFEB,
    MR_EVT_WH_BACKUP_RAIL_MONITOR_WARNING               = 0xFFEA,

    /* Add new Warhawk Events here */

    /* Update the count of WarpDrive events when new events are added */
    MR_EVT_WH_COUNT                                     = 0x0016

    /* Do  not add anything here */
} MR_EVT_WH;

//Events generated by Storelib
typedef enum _SL_EVENTS
{
	SL_EVT_CTRL_HOTPLUG_ADD,
	SL_EVT_CTRL_HOTPLUG_REMOVED,
	SL_EVT_CTRL_CRC_FAILED,
}SL_EVENTS;

typedef enum _SL_EVT_ARGS {
    SL_EVT_ARGS_NONE,
    SL_EVT_ARGS_CRC_FAILED,
}SL_EVT_ARGS;

typedef struct _SL_EVT_DETAIL {
    U32     seqNum;             // event sequence enumber
    U32     timeStamp;          // seconds past Jan 1, 2000 (if FF in bits 24-32, then bits 0-23 are seconds since boot)
    U32     code;               // event code
    MR_EVT_CLASS_LOCALE cl;     // class/locale information for this event
    U8      argType;            // argument type (SL_EVT_ARGS)
    U8      reserved1[15];      // reserved for future use

    union 
    {
        struct 
        {
            U32 totalCRCCount;
            U32 totalRecoveryCount;
            U32 opCode;			
        } crc;
		
        // alternate argument access via byte/short/word/dword
        U8 		 b[96];
        U16		 s[48];
        U32		 w[24];
        U64		 d[12];		
	} args;
		
    char    description[128];	 // Null-terminated ASCII text description of event

}SL_EVT_DETAIL;
#endif /* INCLUDE_SL_EVENT */
