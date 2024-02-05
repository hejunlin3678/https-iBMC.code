/*
 * SL_EVENTMSG.H - Header file for Event message definition -- SCD Warhawk Specific
 *
 *     Copyright 2008-2014 Avago Technologies.  All rights reserved.
 * Written by Rajendra Singh
 */
#ifndef INCLUDE_SL_EVENTMSG
#define INCLUDE_SL_EVENTMSG

#ifdef EVT_DESCRIPTION_DEFINE                   // used to conditionally enable event detail definition
#define EV(str)     MR_EVT_##str                // short-hand reference to MR_EVT_*
#define EC(str)     MR_EVT_CLASS_##str          // short-hand reference to MR_EVT_CLASS_*
#define EL(str)     MR_EVT_LOCALE_##str         // short-hand reference to MR_EVT_LOCALE_*
#define EA(str)     MR_EVT_ARGS_##str           // argument type
#define EA_WH(str)  MR_EVT_WH_ARGS_##str        // argument type -- Warhawk Specific

#define EC_DBG      MR_EVT_CLASS_DEBUG
#define EC_P        MR_EVT_CLASS_PROGRESS
#define EC_I        MR_EVT_CLASS_INFO
#define EC_W        MR_EVT_CLASS_WARNING
#define EC_C        MR_EVT_CLASS_CRITICAL
#define EC_F        MR_EVT_CLASS_FATAL
#define EC_D        MR_EVT_CLASS_DEAD

#define SLEV(str)	SL_EVT_##str				// short-hand reference to SL_EVT_*
#define SLEA(str)   SL_EVT_ARGS_##str			// argument type

/*
 * define the event message strings
 *
 * Each entry in this array must correspond 1:1 with the MR_EVT_WH entries defined in slevent.h
 */
typedef struct _MR_EVT_WH_DESCRIPTION {
    MR_EVT_WH   code;       // event code
    U16         locale;     // event locale (MR_EVT_LOCALE)
    S8          eventClass; // event class (MR_EVT_CLASS)
    U8          Reserved;   // Required to match the structure size with _MR_EVT_DESCRIPTION for seamless typecasting
    U32         argType;    // event argument type (MR_EVT_ARGS)
    const char  *desc;      // description
} MR_EVT_WH_DESCRIPTION;

typedef struct _SL_EVT_DESCRIPTION {
	 SL_EVENTS   code;       // event code
    U16         locale;     // event locale (MR_EVT_LOCALE)
    S8          eventClass; // event class (MR_EVT_CLASS)
    U8          oobLocale;  // event OOB locale (MR_EVT_OOB_LOCALE)
    U32         argType;    // event argument type (SL_EVT_ARGS)
    const char  *desc;      // description
}SL_EVT_DESCRIPTION;
/*
 * define the event detail structure
 */

/*
 * define the event detail info
 */


MR_EVT_WH_DESCRIPTION WarHawkEventDescription[] = {

{ EV(WH_THROTTLING_OCCURING),               EL(CTRL),   EC_I,   0, EA_WH(SLOT_THROTTLE),            "SSD is being throttled Slot Number %04x (Cage %04x Location %s) Throttle %04x" },
{ EV(WH_THROTTLING_REMOVED),                EL(CTRL),   EC_I,   0, EA_WH(SLOT_THROTTLE),            "SSD throttling is now removed Slot Number %04x (Cage %04x Location %s) Throttle %04x" },
{ EV(WH_SSD_LIFE_WARNING),                  EL(CTRL),   EC_W,   0, EA_WH(SLOT_DRIVELIFE),           "SSD Life is at warning level Slot Number %04x (Cage %04x Location %s) Drive Life %04x Warning Level %04x Error Level %04x" },
{ EV(WH_SSD_LIFE_EXHAUSTED),                EL(CTRL),   EC_C,   0, EA_WH(SLOT_DRIVELIFE),           "SSD Life is exhausted Slot Number %04x (Cage %04x Location %s) Drive Life %04x Warning Level %04x Error Level %04x" },
{ EV(WH_BACKUP_RAIL_MONITOR_FAILURE),       EL(CTRL),   EC_C,   0, EA_WH(NONE),                     "Backup Rail Monitor has failed on %s. Check %s documentation for additional details" },
{ EV(WH_TEMP_EXCEEDED_WARNING),             EL(CTRL),   EC_W,   0, EA_WH(TEMP_THRESHOLD),           "Temperature %04x on sensor %04x has exceeded warning temperature threshold %04x" },
{ EV(WH_TEMP_EXCEEDED_CRITICAL),            EL(CTRL),   EC_C,   0, EA_WH(TEMP_THRESHOLD),           "Temperature %04x on sensor %04x has exceeded critical temperature threshold %04x" },
{ EV(WH_POWER_THROTTLING_OCCURING),         EL(CTRL),   EC_C,   0, EA_WH(POWER_THROTTLE),           "Percent Power Throttled %04x PCI Slot Available Power %04x" },
{ EV(WH_POWER_THROTTLING_REMOVED),          EL(CTRL),   EC_C,   0, EA_WH(POWER_THROTTLE),           "Power throttling is now removed Percent Power Throttled %04x PCI Slot Available Power %04x" },
{ EV(WH_DFF_TEMP_EXCEEDED_WARNING),         EL(CTRL),   EC_W,   0, EA_WH(DFF_TEMP_THRESHOLD),       "Temperature %04x on %s %04x has exceeded warning temperature threshold %04x" },
{ EV(WH_DFF_TEMP_EXCEEDED_CRITICAL),        EL(CTRL),   EC_C,   0, EA_WH(DFF_TEMP_THRESHOLD),       "Temperature %04x on %s %04x has exceeded critical temperature threshold %04x" },
{ EV(WH_DIAGNOSTIC_TRIGGER_FIRED),          EL(CTRL),   EC_C,   0, EA_WH(DIAG_TRIGGER),                     "Diagnostic trigger fired" },
{ EV(WH_SSD_SECURITY_LOCKED ),              EL(PD),     EC_F,   0, EA_WH(SSD_SECURITY),             "SSD is in locked state. Encl Index %04x Slot Number %04x Device Id %04x volume's WWID %s target Id %04x" },
{ EV(WH_SSD_SECURITY_UNLOCKED ),            EL(PD),     EC_F,   0, EA_WH(SSD_SECURITY),             "SSD is in unlocked state. Encl Index %04x Slot Number %04x Device Id %04x" },
{ EV(WH_CTRL_PFK_SOD_SASADDR_MISMATCH),     EL(CTRL),   EC_W,   0, EA_WH(CTRL_PFK),                 "PFK - SAS Address mismatch. Slot Number %04x" },
{ EV(WH_CTRL_PFK_SOD_ROMID_MISMATCH),       EL(CTRL),   EC_W,   0, EA_WH(CTRL_PFK),                 "PFK - ROM Id mismatch. Slot Number %04x" },
{ EV(WH_CTRL_PFK_LICENSE_APPLIED),          EL(CTRL),   EC_I,   0, EA_WH(CTRL_PFK),                 "PFK - License Applied. Slot Number %04x" },
{ EV(WH_CTRL_PFK_INVALID_LICENSE_KEY),      EL(CTRL),   EC_I,   0, EA_WH(CTRL_PFK),                 "PFK - Invalid License Key." },
{ EV(WH_RECOVER_VOLUME_INITIATED),          EL(LD),     EC_W,   0, EA_WH(REC_VOL),                  "Volume recovery is initiated on volume with WWID %s & target Id %04x." },
{ EV(WH_RECOVER_VOLUME_SUCCESS),            EL(LD),     EC_I,   0, EA_WH(REC_VOL),                  "Volume recovery is successful on volume with WWID %s & target Id %04x." },
{ EV(WH_RECOVER_VOLUME_FAIL),               EL(LD),     EC_F,   0, EA_WH(REC_VOL),                  "Volume recovery has failed on volume with WWID %s & target Id %04x." },
{ EV(WH_BACKUP_RAIL_MONITOR_WARNING),       EL(CTRL),   EC_W,   0, EA_WH(NONE),                     "Backup Rail Monitor is in warning state on %s. Check %s documentation for additional details" }

};
SL_EVT_DESCRIPTION SlEventDescription[] = {
{ SLEV(CTRL_HOTPLUG_ADD),          EL(CTRL),   EC_I,   0, SLEA(NONE),                 "Controller with channel = 0x%02x, device address = 0x%02x has been unplugged" },
{ SLEV(CTRL_HOTPLUG_REMOVED),      EL(CTRL),   EC_I,   0, SLEA(NONE),                 "Controller with channel = 0x%02x, device address = 0x%02x has been unplugged" },
{ SLEV(CTRL_CRC_FAILED), 	       EL(CTRL),   EC_I,   0, SLEA(CRC_FAILED),           "CRC failed for opcode 0x%x, total CRC failure count = %d, total recovery count = %d" }
};

#undef EV
#undef EC
#undef EL
#undef EA

#undef EC_DBG
#undef EC_P
#undef EC_I
#undef EC_W
#undef EC_C
#undef EC_F
#undef EC_D

#else
    extern  MR_EVT_WH_DESCRIPTION  WarHawkEventDescription[];
    extern  SL_EVT_DESCRIPTION     SlEventDescription[];
#endif

#endif /* INCLUDE_EVENTMSG */
