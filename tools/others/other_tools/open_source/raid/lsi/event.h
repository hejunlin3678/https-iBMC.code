/*
 * EVENT.H - Header file for Event management from API
 *
 * Copyright (C) LSI Logic Corporation, 2004
 * Written by Gerry Smith
 */
#ifndef INCLUDE_EVENT
#define INCLUDE_EVENT

#include "mr.h"

/*
 * define macros to convert unnamed structs to named structs
 *
 * Provides compatbility with compilers that do no allow unnamed structs.
 * Consumer application must compile with NO_UNNAMED_STRUCT defined.
 * search for "unnamed" to find usage in this file.
 * If NO_UNNAMED_STRUCT is defined, use structure names to access members.
 * If unnamed structs are allowed, do not use these structure names.
 *
 * If possible ensure all new structures are named.
 */
#ifdef NO_UNNAMED_STRUCT        /* unnamed struct are not acceptable */

#define mrEventClassLocale      mrEventClassLocale
#define mrEvtArgLd              mrEvtArgLd
#define mrEvtArgPd              mrEvtArgPd

#else                           /* unnamed struct are acceptable */

#define mrEventClassLocale
#define mrEvtArgLd
#define mrEvtArgPd

#endif  /* NO_UNNAMED_STRUCT */

/*
 * define the event class values
 *
 * Definitions:
 *  DEBUG   - Event is for debugging only.  This event is not saved in NVRAM.
 *  PROGRESS- Event is a progress posting event.  This event is not saved in NVRAM.
 *  INFO    - Informational message
 *  WARNING - Some component may be close to a failure point
 *  CRITICAL- A component has failed but the system has not lost data
 *  FATAL   - A component has failed and data loss has (or will) occur
 *  DEAD    - A catastrophic error has occurred and the controller has dies.  This event
 *            will only be seen after the controller has been restarted.
 */
typedef enum _MR_EVT_CLASS {
    MR_EVT_CLASS_DEBUG       = -2,       /* debug information */
    MR_EVT_CLASS_PROGRESS    = -1,       /* rebuild/bgi/cc/fgi/reconstruct progress posting */
    MR_EVT_CLASS_INFO        = 0,
    MR_EVT_CLASS_WARNING     = 1,
    MR_EVT_CLASS_CRITICAL    = 2,
    MR_EVT_CLASS_FATAL       = 3,
    MR_EVT_CLASS_DEAD        = 4,        /* only visible after next controller boot */
} MR_EVT_CLASS;
#define MR_EVT_CLASS_MAX       4        /* maximum permissible value */

/*
 * define event locale codes
 */
typedef enum _MR_EVT_LOCALE {
    MR_EVT_LOCALE_LD        = 0x0001,     /* LD field valid */
    MR_EVT_LOCALE_PD        = 0x0002,     /* PD field valid */
    MR_EVT_LOCALE_ENCL      = 0x0004,     /* ENCL Index valid */
    MR_EVT_LOCALE_BBU       = 0x0008,     /* BBU events */
    MR_EVT_LOCALE_SAS       = 0x0010,     /* SAS events codes */
    MR_EVT_LOCALE_CTRL      = 0x0020,     /* BOOT/SHUTDOWN events, etc */
    MR_EVT_LOCALE_CONFIG    = 0x0040,     /* configuration changes */
    MR_EVT_LOCALE_CLUSTER   = 0x0080,     /* cluster events */
    MR_EVT_LOCALE_ALL       = 0xffff,     /* bitmap for all event locales */
} MR_EVT_LOCALE;

/*
 * define event OOB class codes
 */
typedef enum _MR_EVT_OOB_LOCALE {
    MR_EVT_OOB_LOCALE_INVENTORY        = 0x01,     /* Inventory related */
    MR_EVT_OOB_LOCALE_HWR              = 0x02,     /* HWR related */
    MR_EVT_OOB_LOCALE_MISC_BASIC       = 0x04,     /* Basic set of miscellaneous events */
                                                   /* reserved for future */
    MR_EVT_OOB_LOCALE_MISC_SET1        = 0x10,     /* Misc SET1 */
    MR_EVT_OOB_LOCALE_MISC_SET2        = 0x20,     /* Misc SET2 */
                                                   /* reserved for future */
    MR_EVT_OOB_LOCALE_NEVER            = 0x80,     /* Don't expose these events via OOB poll */

} MR_EVT_OOB_LOCALE;

#define MR_EVT_OOB_LOCALE_HWR_ALL        0x03     /* A complete set of HWR related events to monitor controller (HWR + INVENTORY) */
#define MR_EVT_OOB_LOCALE_BASIC          0x07     /* A complete set of basic events to monitor controller (BASIC + HWR+ INVENTORY) */

/*
 * Define the class/locale structure.  This is used as an input to some DCMD opcodes.
 * See MR_DCMD_CTRL_EVENT_GET for detail on usage of this structure to filter events.
 */
typedef union _MR_EVT_CLASS_LOCALE {
    struct {
        U16     locale;                 // locale of the event (MR_EVT_LOCALE bit map)
        union {
            U8      oobLocale;          // applicable only for OOB interface (MR_EVT_OOB_LOCALE)
            U8      reserved;           // reserved for future use
        };
        S8      eventClass;             // class of event (MR_EVT_CLASS)
    } mrEventClassLocale;               // unnamed struct:  search for NO_UNNAMED_STRUCT for usage
    U32     w;                          // alternate name for full 32-bit value
} MR_EVT_CLASS_LOCALE;

/*
 * define the argument types
 */
typedef enum _MR_EVT_ARGS {
    MR_EVT_ARGS_NONE,
    MR_EVT_ARGS_CDB_SENSE,
    MR_EVT_ARGS_LD,
    MR_EVT_ARGS_LD_COUNT,
    MR_EVT_ARGS_LD_LBA,
    MR_EVT_ARGS_LD_OWNER,
    MR_EVT_ARGS_LD_LBA_PD_LBA,
    MR_EVT_ARGS_LD_PROG,
    MR_EVT_ARGS_LD_STATE,
    MR_EVT_ARGS_LD_STRIP,
    MR_EVT_ARGS_PD,
    MR_EVT_ARGS_PD_ERR,
    MR_EVT_ARGS_PD_LBA,
    MR_EVT_ARGS_PD_LBA_LD,
    MR_EVT_ARGS_PD_PROG,
    MR_EVT_ARGS_PD_STATE,
    MR_EVT_ARGS_PCI,
    MR_EVT_ARGS_RATE,
    MR_EVT_ARGS_STR,
    MR_EVT_ARGS_TIME,
    MR_EVT_ARGS_ECC,
    MR_EVT_ARGS_LD_PROP,
    MR_EVT_ARGS_PD_SPARE,
    MR_EVT_ARGS_PD_INDEX,
    MR_EVT_ARGS_DIAG_PASS,
    MR_EVT_ARGS_DIAG_FAIL,
    MR_EVT_ARGS_PD_LBA_LBA,
    MR_EVT_ARGS_PORT_PHY,
    MR_EVT_ARGS_PD_MISSING,
    MR_EVT_ARGS_PD_ADDRESS,
    MR_EVT_ARGS_BITMAP,
    MR_EVT_ARGS_CONNECTOR,
    MR_EVT_ARGS_PD_PD,
    MR_EVT_ARGS_PD_FRU,
    MR_EVT_ARGS_PD_PATHINFO,
    MR_EVT_ARGS_PD_POWER_STATE,
    MR_EVT_ARGS_GENERIC,
    MR_EVT_ARGS_KEYINFO,
    MR_EVT_ARGS_LD_LD,
    MR_EVT_ARGS_LD_PIT,
    MR_EVT_ARGS_LD_VIEW,
    MR_EVT_ARGS_LD_SNAP_PROG,
    MR_EVT_ARGS_PFINFO,
    MR_EVT_ARGS_LD_PROP_PS,
    MR_EVT_ARGS_PD_TEMP,
    MR_EVT_ARGS_PD_LIFE,
    MR_EVT_ARGS_MODE,
    MR_EVT_ARGS_LD_INCOMPATIBLE,
    MR_EVT_ARGS_PD_INCOMPATIBLE,
    MR_EVT_ARGS_PDLD_OPERATION_DELAYED,
    MR_EVT_ARGS_LD_VF_MAP,
    MR_EVT_ARGS_PD_DEGRADED_MEDIA,
    MR_EVT_ARGS_BBU_SCAP_INFO,
    MR_EVT_ARGS_PD_INQ,
    MR_EVT_ARGS_AUTO_CFG_OPTION,
    MR_EVT_ARGS_LD_EPD,
    MR_EVT_ARGS_CTRL_FRU_STATE,
    MR_EVT_ARGS_PD_ERROR_STATS,
} MR_EVT_ARGS;

/*
 * define assorted argument structures
 */
typedef union _MR_EVT_ARG_LD {
    struct {
        U16     targetId;           // logical drive target ID
        U8      ldIndex;            // FW LD number, valid only if !isRemote nor !isEPD
        U8      isRemote      :1;   // HA cluster locale: 0=local, 1=remote (at peer)
        U8      isEPD         :1;   // LD is an EPD. ldIndex is not valid.
        U8      ldIndexMsb    :5;   // MSB of ldIndex
        U8      reserved      :1;
    } mrEvtArgLd;                   // unnamed struct:  search for NO_UNNAMED_STRUCT for usage
    U32     w;                      // alternate name for full 32-bit value
} MR_EVT_ARG_LD;

/*
* Event PD structure applicable for MR_EVT_LOCALE_PD and MR_EVT_LOCALE_ENCL - both physical drives and enclosures.
*
* For physical drives the following fields will be applicable.
*     deviceId (of the physical drive)
*     enclDeviceId
*     slotNumber
*
* For enclosures the following fields will be applicable.
*     deviceId (of the enclosure)
*     enclPosition
*     enclConnectorIndex
*
* Refer MR_EVT_ARG_PD_EXT for additional information
*/
typedef union _MR_EVT_ARG_PD {
    struct {
        U16     deviceId;               // physical device ID

        union {
            U8      enclIndex;          // DEPRECATED. For enclosures, 0 means not within an enclosure

            // For MR_EVT_LOCALE_PD, if MR_CTRL_INF0.adapterOperations.supportEnclEnumeration is set
            U8      enclDeviceId;       // (U8)MR_PD_INVALID(FF) = not within an enclosure
                                        // HACK: 2-Byte deviceId, is shoe horned into a byte here,
                                        // notice the cast required to compare with MR_PD_INVALID.
                                        // This is ok as long as MAX_PHYSICAL_DEVICES is 256.

            // For MR_EVT_LOCALE_ENCL, if MR_CTRL_INF0.adapterOperations.supportEnclEnumeration is set
            U8      enclPosition;       // Position of enclosure in cascaded chain
        };

        union {
            // For legacy usage and for MR_EVT_LOCALE_PD, if MR_CTRL_INF0.adapterOperations.supportEnclEnumeration is set
            U8      slotNumber;         // slot number in encloure

            // For MR_EVT_LOCALE_ENCL, if MR_CTRL_INF0.adapterOperations.supportEnclEnumeration is set
            U8      enclConnectorIndex; // Index of the connector to which this enclosure (chain) is attached.
        };
    } mrEvtArgPd;                       // unnamed struct:  search for NO_UNNAMED_STRUCT for usage
    U32     w;                          // alternate name for full 32-bit value
} MR_EVT_ARG_PD;

/*
* This is additional information for MR_EVT_LOCALE_PD and MR_EVT_LOCALE_ENCL. This structure will
* always go along with MR_EVT_ARG_PD as an event argument.
*/
typedef union _MR_EVT_ARG_PD_EXT {
    struct {
#ifndef MFI_BIG_ENDIAN
        U8             isValid  :  1;
        U8             boxNumber : 7;
#else
        U8             boxNumber : 7;
        U8             isValid  :  1;
#endif
    } mrEvtArgPdExt;
    U8     b;
} MR_EVT_ARG_PD_EXT;

typedef struct _MR_EVT_ARG_PD_INFO {
                                                                                    //           Packed bytes     Description
                                                                                    //                                  123456789-123456789-
                                                                                    //                               6 'Info- '
    char            vendor[4];          // First four characters of Vendor name                      4               5 'SEAG '
    char            model[16];          // Drive model (product identification)                     16              17 'STxxxxxxxxxxxxxx '
    char            serialNumber[10];   // Drive serial number, first 10 characters                 10              17 'mnmnmnmnmnmnmnmn '
#ifndef MFI_BIG_ENDIAN
    U16             unitsTB  :  1;      // 1=capacity in TB, 0=capacity in GB                        2               7 'xxx GB' (or TB)
    U16             capacity : 15;      // Capacity of the drive
#else
    U16             capacity : 15;      // Capacity of the drive
    U16             unitsTB  :  1;      // 1=capacity in TB, 0=capacity in GB                        2               7 'xxx GB' (or TB)
#endif
                                                                                    // Totals       32              52 characters to print
                                                                                    // Use sprintf("Info- %.4s %.16s %.10s %d %s"),
                                                                                    //              vendor, model, serialNumber, capacity, (unitsTB ? "TB" : "GB");
} MR_EVT_ARG_PD_INFO;

/*
 * this structure describes the format of an event returned to the application
 */
typedef struct _MR_EVT_DETAIL {
    U32     seqNum;             // event sequence enumber
    U32     timeStamp;          // seconds past Jan 1, 2000 (if FF in bits 24-32, then bits 0-23 are seconds since boot)
    U32     code;               // event code
    MR_EVT_CLASS_LOCALE cl;     // class/locale information for this event
    U8      argType;            // argument type (MR_EVT_ARGS)
    U8      enclDeviceIdMsb;    // valid only MR_EVT_LOCALE_PD (for MR_EVT_ARGS_PD_PD this is the primary PD)
    U8      reserved1[14];      // reserved for future use

    /*
     * Define the arguments unions.  Each log event has a specific 'argument' type
     * which is used to reference this union.
     */
    union {
        struct {                // MR_EVT_ARGS_CDB_SENSE
            MR_EVT_ARG_PD   pd;         // device ID
            U8              cdbLength;  // length of CDB
            U8              senseLength;// length of request sense data
            MR_EVT_ARG_PD_EXT   pdExt;
            U8              reserved;
            U8              cdb[16];    // CDB
            U8              sense[64];  // sense data
            U64             sasAddr;    // sasAddr of the path where CDB (and SENSE) is encountered.
                                        // Ignore if sasAddr = MR_SAS_ADDRESS_INVALID
        } cdbSense;

        MR_EVT_ARG_LD   ld;     // MR_EVT_ARGS_LD

        /*
         * For EventDescription[code].argType == MR_EVT_ARGS_LD AND where the LDs is an EPD,
         * i.e. MR_EVT_ARG_LD.isEPD=1, the argType in the actual MR_EVT_DETAIL will be substituted with MR_EVT_ARGS_LD_EPD.
         * FW shall provide the argument string as "EPD <tgtID> for PD <devID> (e<encDevID>/s<slot#>)"
         *
         * Note -  for all other event arguments that include MR_EVT_ARGS_LD,
         *         FW will only replace "VD" with "EPD" in the event string and only reference the targetID.
         */
        struct {                // MR_EVT_ARGS_LD_EPD substituted for MR_EVT_ARGS_LD for EPDs
            MR_EVT_ARG_LD   ld;
            MR_EVT_ARG_PD   pd;
            MR_EVT_ARG_PD_EXT   pdExt;
        } ldEpd;

        struct {                // MR_EVT_ARGS_LD_COUNT
            MR_EVT_ARG_LD   ld;
            U32             count;
        } ldCount;

        struct {                // MR_EVT_ARGS_LD_LBA
            U64             lba;
            MR_EVT_ARG_LD   ld;
        } ldLba;

        struct {                // ME_EVT_ARGS_LD_OWNER
            MR_EVT_ARG_LD   ld;
            U32             prevOwner;
            U32             newOwner;
        } ldOwner;

        struct {                // MR_EVT_ARGS_LD_LBA_PD_LBA
            U64             ldLba;
            U64             pdLba;
            MR_EVT_ARG_LD   ld;
            MR_EVT_ARG_PD   pd;
            MR_EVT_ARG_PD_EXT   pdExt;
        } ldLbaPdLba;

        struct {                // MR_EVT_ARGS_PD_ERROR_STATS
            MR_EVT_ARG_PD pd;
            U16 driveErrorCounter;  // driveErrorCounter for this PD
            U16 slotErrorCounter;   // slotErrorCounter for this slot in the enclosure
            char probableIssue [24];// string indicating "drive" or "cable" as probable issue
            MR_EVT_ARG_PD_EXT   pdExt;
        } pdErrorStats;

        struct {                // MR_EVT_ARGS_LD_PROG
            MR_EVT_ARG_LD   ld;
            MR_PROGRESS     prog;
            U32             totalElapsedSecs;   // 0 = unintialized
        } ldProg;

        struct {                // MR_EVT_ARGS_LD_PROP, MR_EVT_ARGS_LD_PROP_PS
            MR_EVT_ARG_LD   ld;
            MR_LD_PROPERTIES prevProp;
            MR_LD_PROPERTIES newProp;
        } ldProp, ldPropPs;

        struct {                // MR_EVT_ARGS_LD_STATE
            MR_EVT_ARG_LD   ld;
            U32             prevState;  // type MR_LD_STATE
            U32             newState;   // type MR_LD_STATE
        } ldState;

        struct {                // MR_EVT_ARGS_LD_STRIP
            U64             strip;
            MR_EVT_ARG_LD   ld;
        } ldStrip;

        MR_EVT_ARG_PD   pd;     // MR_EVT_ARGS_PD

        struct {                // MR_EVT_ARGS_PD
            MR_EVT_ARG_PD   pd;
            MR_EVT_ARG_PD_EXT   pdExt;
        } pdPdExt;

        struct {                // MR_EVT_ARGS_PD_ERR, MR_EVT_ARGS_PD_PATHINFO
            MR_EVT_ARG_PD   pd;
            union {
                U32         err;        // Used for MR_EVT_ARGS_PD_ERR
                U32         path;       // Used for MR_EVT_ARGS_PD_PATHINFO
            };
            U64             sasAddr;    // sasAddr of the path where err was encountered.
                                        // Ignore if sasAddr = MR_SAS_ADDRESS_INVALID
            MR_EVT_ARG_PD_EXT   pdExt;
        } pdErr, pdPathInfo;

        struct {                // MR_EVT_ARGS_PD_LBA
            U64             lba;
            MR_EVT_ARG_PD   pd;
            MR_EVT_ARG_PD_EXT   pdExt;
        } pdLba;

        struct {                // MR_EVT_ARGS_PD_LBA_LD
            U64             lba;
            MR_EVT_ARG_PD   pd;
            MR_EVT_ARG_LD   ld;
            MR_EVT_ARG_PD_EXT   pdExt;
        } pdLbaLd;

        struct {                // MR_EVT_ARGS_PD_PROG
            MR_EVT_ARG_PD   pd;
            MR_PROGRESS     prog;
            U32             totalElapsedSecs;   // 0 = unintialized
            MR_EVT_ARG_PD_EXT   pdExt;
        } pdProg;

        struct {                // MR_EVT_ARGS_PD_STATE, MR_EVT_ARGS_PD_POWER_STATE
            MR_EVT_ARG_PD   pd;
            U32             prevState;  // type MR_PD_STATE, MR_PD_POWER_STATE
            U32             newState;   // type MR_PD_STATE, MR_PD_POWER_STATE
            MR_EVT_ARG_PD_EXT   pdExt;
        } pdState, pdPowerState;

        struct {                // MR_EVT_ARGS_PCI
            U16     vendorId;
            U16     deviceId;
            U16     subVendorId;
            U16     subDeviceId;
        } pci;

        U32         rate;       // MR_EVT_ARGS_RATE

        char        str[96];    // MR_EVT_ARGS_STR  (NULL terminated)

        struct {                // MR_EVT_ARGS_TIME
            U32     rtc;            // time in seconds since January 1, 2000
            U32     elapsedSeconds; // elapsed seconds since power-on, ignore if 0xFFFFFFFF
        } time;

        struct {
            U32     ecar;           // ECAR register
            U32     elog;           // ELOG register
            char    str[64];        // descriptive string of error
        } ecc;

        struct {                // MR_EVT_ARGS_PD_SPARE
            MR_EVT_ARG_PD   pd;
            MR_SPARE        spare;
            MR_EVT_ARG_PD_EXT   pdExt;
        } pdSpare;

        struct {                // MR_EVT_ARGS_PD_INDEX, MR_EVT_ARGS_PD_TEMP
            MR_EVT_ARG_PD   pd;
            union {
                U32             index;       // index number (for enclosure element indexes)
                U32             temperature; // current temperature
            };
            MR_EVT_ARG_PD_EXT   pdExt;
        } pdIndex, pdTemp;


        struct {                // MR_EVT_ARGS_DIAG_PASS
            U32     passCount;      // pass count
            char    str[64];        // test name
        } diagPass;

        struct {                // MR_EVT_ARGS_DIAG_FAIL
            U32     passCount;      // pass count
            U32     offset;         // offset of failure
            U8      goodData;       // good data
            U8      badData;        // bad data
            U8      reserved[2];    // reserved
            char    str[64];        // test name
        } diagFail;

        struct {                // MR_EVT_ARGS_PD_LBA_LBA
            U64             pdLba1; // 'bad' LBA
            U64             pdLba2; // 'remapped' LBA
            MR_EVT_ARG_PD   pd;
            MR_EVT_ARG_PD_EXT   pdExt;
        } pdLbaLba;

        struct {                // MR_EVT_ARGS_PORT_PHY
            U32         port;       // port number
            U32         phy;        // PHY number
        } portPhy;

        struct {                // MR_EVT_ARGS_PD_MISSING
            MR_EVT_ARG_PD   pd;
            U32             ar;     // array number
            U32             row;    // row number
            MR_EVT_ARG_PD_EXT   pdExt;
        } pdMissing;

        MR_PD_ADDRESS   pdAddr; // MR_EVT_ARGS_PD_ADDRESS

        struct {                // MR_EVT_ARGS_PD_MISSING
            MR_PD_ADDRESS       pdAddr; // MR_EVT_ARGS_PD_ADDRESS
            MR_EVT_ARG_PD_EXT   pdExt;
        } pdAddrExt;

        struct {                // MR_EVT_ARGS_BITMAP
            U8      count;          // Number of bytes in bitmap
            U8      reserved[3];
            U8      evtBitmap[92];  // LSB of bitmap is bit 0 of evtBitmap[0].
                                    // MSB of bitmap is bit 8 of evtBitmap[count-1].
        } bitmap;

        MR_SAS_CONNECTOR_INFO   connector;  // MR_EVT_ARGS_CONNECTOR;

        struct {                // MR_EVT_ARGS_PD_PD
            MR_EVT_ARG_PD   destPd;
            MR_EVT_ARG_PD   srcPd;
            U8              enclDeviceIdMsb;    // valid for src PD
            MR_EVT_ARG_PD_EXT   destPdExt;
            MR_EVT_ARG_PD_EXT   srcPdExt;
        } pdPair;

        struct {                // MR_EVT_ARGS_PD_FRU
            MR_EVT_ARG_PD   pd;
            char            FRU[16];
            MR_EVT_ARG_PD_EXT   pdExt;
        } pdFRU;

        U64         genericArg; // MR_EVT_ARGS_GENERIC

        struct {                // MR_EVT_ARGS_KEYINFO
            MR_EVT_ARG_PD        pd;
            MR_SECURITY_KEY_TYPE keyType;
            U8                   keyIdLength;
            MR_EVT_ARG_PD_EXT    pdExt; // Since this is after variable length array the offset needs to be calculated
            U8                   reserved;
            char                 str[1];  // variable length keyId
        } keyInfo;

        struct {                // MR_EVT_ARGS_LD_LD
            MR_EVT_ARG_LD   ld;         // LD consuming a service or dependent LD
            MR_EVT_ARG_LD   providerLd; // LD providing the service
        } ldpair;

        struct {                // MR_EVT_ARGS_LD_PIT
            MR_EVT_ARG_LD   ld;      // source LD
            U32             pitRtc;  // time in seconds since January 1, 2000
            U8              autoPit; // indicates if this is a auto-snapshot pit
            U8              pad[3];
        } ldPiT;

        struct {                // MR_EVT_ARGS_LD_VIEW
            MR_EVT_ARG_LD   ld;      // source LD or snapshot repository
            U32             viewRtc; // VIEW: time in seconds since January 1, 2000
            U32             pitRtc;  // PiT: time in seconds since January 1, 2000
        } ldView;

        struct {                // MR_EVT_ARGS_LD_SNAP_PROG
            MR_EVT_ARG_LD   ld;     // source LD
            MR_PROGRESS     prog;   // progress
            U32             pitRtc; // time in seconds since January 1, 2000
        } ldSnapProg;

        MR_PF_INFO      pf;     // MR_EVT_ARGS_PFINFO

        struct {                // MR_EVT_ARGS_PD_LIFE
            MR_EVT_ARG_PD   pd;
            U8              life;           // current remaining life %
            U8              warningLevel;   // warning threshold (%)
            U8              criticalLevel;  // critical threshold (%)
            U8              pad;
            MR_EVT_ARG_PD_EXT   pdExt;
        } pdLife;

        U32         mode;       // MR_EVT_ARGS_MODE

        struct {                // MR_EVT_ARGS_LD_INCOMPATIBLE
            MR_EVT_ARG_LD   ld;
            U32             mode;       // MR_EVT_ARGS_MODE
            char            str[64];    // Incompatible area
        } ldComp;

        struct {                // MR_EVT_ARGS_PD_INCOMPATIBLE
            MR_EVT_ARG_PD   pd;
            U32             mode;       // MR_EVT_ARGS_MODE
            char            str[64];    // Incompatible area
            MR_EVT_ARG_PD_EXT   pdExt;
        } pdComp;

        struct {                // PD or LD operation delay, MR_EVT_ARGS_PDLD_OPERATION_DELAYED
            union {
                MR_EVT_ARG_PD   pd;
                MR_EVT_ARG_LD   ld;
            };
            char            strOp[24];    // PD or LD operation
            char            strDr[68];    // Delay Reason
        } PdLdOpDelay; // This event will not be available for every OEM

        MR_LD_VF_MAP        ldVfMap;            // MR_EVT_ARGS_MR_LD_VF_MAP
        MR_IOV_CTRL_FRU_STATE fruState;         // MR_IOV_CTRL_FRU_STATE

        struct {                // MR_EVT_ARGS_PD_DEGRADED_MEDIA                                        // Packed bytes           Description
                                                                                                        //                             123456789-123456789-123456789-123456789-
                                                                                                        //       32                34 'EVT#nnnnn-mm/dd/yy hh:mm:ss: eeee='
            MR_EVT_ARG_PD       pd;                                                                     //        4                17 'PD ID(e0xID/sXX) '
            MR_EVT_ARG_PD_INFO  pdInfo;                                                                 //       32                52 'Info-.....'
#ifndef MFI_BIG_ENDIAN
            U16                 errorRate :  1; // 1= argument is bad media error rate.
                                                // 0= argument is numErrors and locality info.
            U16                 reserved  : 15;                                                         //        2                   Not displayed
                                                                                                        // Net   70               103
#else
            U16                 reserved  : 15;
            U16                 errorRate :  1;
#endif
            union {
                struct {
                    U16         badMediaErrCount;// number of occurances of degraded media in this period         2                23 'NNNNN bad media events.'
                                                 // 0xFFFF means more than 64K, apps may replace NNNNN with 'MANY'
                                                                                                        //                          1 NULL Termination
                    U8          locality[16];    // Occurances across 16 regions of the drive                    16               Not part of event description
                                                 // 0xFF indicates 256 or more errors were encountered
                                                                                                        // Total 88               127
                };
                struct {
                    U16         numErrors;       // total poor performance errors                                 2                    0 Not displayed
                    U16         timePeriod;      // minutes                                                       2                23 'has bad perf. (XXmin/hr).'
                                                                                                        // For 'XX min/hr' use sprintf("(%2d%s)",
                                                                                                        //                            timePeriod < 60 : timePeriod : timePeriod/60),
                                                                                                        //                            timePeriod < 60 : "min" : "hr");
                                                                                                        // Total 74               126
                                                                                                        // Note - FW is restricted to 128 char description.  Apps may provide a more
                                                                                                        //        descriptive message - "has poor performance at least 1 per XX min/hr"
                };
            };

            MR_EVT_ARG_PD_EXT   pdExt;

        } pdDegradedMedia;

        struct {                // MR_EVT_ARGS_BBU_SCAP_INFO
            U16      sCapCapacitance;       // superCap capacitance
            U16      sCapEsr;               // superCap resistance
            U16      sCapVoltage;           // superCap voltage
            U16      sCapTemperature;       // superCap current temperature
            U32      over55DegBucketCount;  // count of howmany times above 55 Deg temperature is reached
            U32      over60DegBucketCount;  // count of howmany times above 60 Deg temperature is reached
            U32      over65DegBuccketCount; // count of howmany times above 65 Deg temperature is reached
            U32      over70DegBucketCount;  // count of howmany times above 70 Deg temperature is reached
            U32      over75DegBucketCount;  // count of howmany times above 75 Deg temperature is reached
        } bbuScapInfo;

        struct {                // MR_EVT_ARGS_PD_INQ
            MR_EVT_ARG_PD       pd;
            MR_EVT_ARG_PD_INFO  pdInfo;
            MR_EVT_ARG_PD_EXT   pdExt;
        } pdInqInfo;

        MR_AUTO_CFG_OPTIONS     autoCfgOption;  // MR_EVT_ARGS_AUTO_CFG_OPTION

        // alternate argument access via byte/short/word/dword
        U8          b[96];
        U16         s[48];
        U32         w[24];
        U64         d[12];

    } args;

    char    description[128];   // Null-terminated ASCII text description of event

    /* size = 256 bytes */
} MR_EVT_DETAIL;

/*
 * define event LOG INFO structure
 */
typedef struct _MR_EVT_LOG_INFO {
    U32     newestSeqNum;           /* seq of newest event in log */
    U32     oldestSeqNum;           /* seq of oldest event in log */
    U32     clearSeqNum;            /* seq of last log clear */
    U32     shutdownSeqNum;         /* seq of last clean shutdown */
    U32     bootSeqNum;             /* seq of this session boot */
} MR_EVT_LOG_INFO;

/*
 * structure for returning a list of events
 */
typedef struct _MR_EVT_LIST {
    U32             count;
    U32             reserved;       /* reserved for future use - also aligns structure */
    MR_EVT_DETAIL   event[1];       /* list of returned events (app to declare large enough) */
} MR_EVT_LIST;

/*
 * define EVENT detail PACKED structure
 * Primarily for OOB usage
 *
 * Use MR_EVT_DESCRIPTION to convert this structure to its corresponding MR_EVT_DETAIL
 * Use event code to index into EventDescription.
 */
typedef struct _MR_EVT_PACKED {
    U8      checksum;               // 000 - checksum of full record
    U8      length;                 // 001 - byte length of event record (including checksum and length fields)
    U16     code;                   // 002 - event code (MR_EVT)
    U32     seqNum;                 // 004 - event sequence enumber
    U32     timeStamp;              // 008 - seconds past Jan 1, 2000
    U8      args[4];                // 00c - variable list of arguments (max 96 bytes)
} MR_EVT_PACKED;

/*
 * structure for returning a list of packed events
 * Primarily for OOB usage
 *
 */
typedef struct _MR_EVT_PACKED_LIST {
    U32             count;
    MR_EVT_PACKED   event[1];       /* list of returned events (app to declare large enough) */
} MR_EVT_PACKED_LIST;

/*
 * define event codes
 */
typedef enum _MR_EVT {
    MR_EVT_BOOT                                             = 0x0000,
    MR_EVT_VERSION                                          = 0x0001,
    MR_EVT_BBU_TBBU_DIRTY_CACHE_CONFIG_MISMATCH             = 0x0002,
    MR_EVT_BBU_TBBU_DIRTY_CACHE_PROCESSED                   = 0x0003,
    MR_EVT_CFG_CLEARED                                      = 0x0004,
    MR_EVT_CLUST_DOWN                                       = 0x0005,
    MR_EVT_CLUST_OWNERSHIP_CHANGE                           = 0x0006,
    MR_EVT_CTRL_ALARM_DISABLED                              = 0x0007,
    MR_EVT_CTRL_ALARM_ENABLED                               = 0x0008,
    MR_EVT_CTRL_BGI_RATE_CHANGED                            = 0x0009,
    MR_EVT_CTRL_CACHE_DISCARDED                             = 0x000a,
    MR_EVT_CTRL_CACHE_REBOOT_CANT_RECOVER                   = 0x000b,
    MR_EVT_CTRL_CACHE_REBOOT_RECOVER                        = 0x000c,
    MR_EVT_CTRL_CACHE_VERSION_MISMATCH                      = 0x000d,
    MR_EVT_CTRL_CC_RATE_CHANGED                             = 0x000e,
    MR_EVT_CTRL_CRASH                                       = 0x000f,
    MR_EVT_CTRL_FACTORY_DEFAULTS                            = 0x0010,
    MR_EVT_CTRL_FLASH_BAD_IMAGE                             = 0x0011,
    MR_EVT_CTRL_FLASH_ERASE_ERROR                           = 0x0012,
    MR_EVT_CTRL_FLASH_ERASE_TIMEOUT                         = 0x0013,
    MR_EVT_CTRL_FLASH_GENERAL_ERROR                         = 0x0014,
    MR_EVT_CTRL_FLASH_IMAGE                                 = 0x0015,
    MR_EVT_CTRL_FLASH_OK                                    = 0x0016,
    MR_EVT_CTRL_FLASH_PROGRAM_ERROR                         = 0x0017,
    MR_EVT_CTRL_FLASH_PROGRAM_TIMEOUT                       = 0x0018,
    MR_EVT_CTRL_FLASH_UNKNOWN_CHIP_TYPE                     = 0x0019,
    MR_EVT_CTRL_FLASH_UNKNOWN_CMD_SET                       = 0x001a,
    MR_EVT_CTRL_FLASH_VERIFY_FAILURE                        = 0x001b,
    MR_EVT_CTRL_FLUSH_RATE_CHANGED                          = 0x001c,
    MR_EVT_CTRL_HIBERNATE                                   = 0x001d,
    MR_EVT_CTRL_LOG_CLEARED                                 = 0x001e,
    MR_EVT_CTRL_LOG_WRAPPED                                 = 0x001f,
    MR_EVT_CTRL_MEM_ECC_MULTI_BIT                           = 0x0020,
    MR_EVT_CTRL_MEM_ECC_SINGLE_BIT                          = 0x0021,
    MR_EVT_CTRL_NOT_ENOUGH_MEMORY                           = 0x0022,
    MR_EVT_CTRL_PR_DONE                                     = 0x0023,
    MR_EVT_CTRL_PR_PAUSE                                    = 0x0024,
    MR_EVT_CTRL_PR_RATE_CHANGED                             = 0x0025,
    MR_EVT_CTRL_PR_RESUME                                   = 0x0026,
    MR_EVT_CTRL_PR_START                                    = 0x0027,
    MR_EVT_CTRL_REBUILD_RATE_CHANGED                        = 0x0028,
    MR_EVT_CTRL_RECON_RATE_CHANGED                          = 0x0029,
    MR_EVT_CTRL_SHUTDOWN                                    = 0x002a,
    MR_EVT_CTRL_TEST                                        = 0x002b,
    MR_EVT_CTRL_TIME_SET                                    = 0x002c,
    MR_EVT_CTRL_USER_ENTERED_DEBUGGER                       = 0x002d,
    MR_EVT_LD_BGI_ABORTED                                   = 0x002e,
    MR_EVT_LD_BGI_CORRECTED_MEDIUM_ERROR                    = 0x002f,
    MR_EVT_LD_BGI_DONE                                      = 0x0030,
    MR_EVT_LD_BGI_DONE_ERRORS                               = 0x0031,
    MR_EVT_LD_BGI_DOUBLE_MEDIUM_ERRORS                      = 0x0032,
    MR_EVT_LD_BGI_FAILED                                    = 0x0033,
    MR_EVT_LD_BGI_PROGRESS                                  = 0x0034,
    MR_EVT_LD_BGI_START                                     = 0x0035,
    MR_EVT_LD_CACHE_POLICY_CHANGE                           = 0x0036,
    MR_EVT_OBSOLETE_1                                       = 0x0037,       // obsolete - may be used in the future
    MR_EVT_LD_CC_ABORTED                                    = 0x0038,
    MR_EVT_LD_CC_CORRECTED_MEDIUM_ERROR                     = 0x0039,
    MR_EVT_LD_CC_DONE                                       = 0x003a,
    MR_EVT_LD_CC_DONE_INCON                                 = 0x003b,
    MR_EVT_LD_CC_DOUBLE_MEDIUM_ERRORS                       = 0x003c,
    MR_EVT_LD_CC_FAILED                                     = 0x003d,
    MR_EVT_LD_CC_FAILED_UNCOR                               = 0x003e,
    MR_EVT_LD_CC_INCONSISTENT_PARITY                        = 0x003f,
    MR_EVT_LD_CC_INCONSISTENT_PARITY_LOGGING_DISABLED       = 0x0040,
    MR_EVT_LD_CC_PROGRESS                                   = 0x0041,
    MR_EVT_LD_CC_START                                      = 0x0042,
    MR_EVT_LD_INIT_ABORTED                                  = 0x0043,
    MR_EVT_LD_INIT_FAILED                                   = 0x0044,
    MR_EVT_LD_INIT_PROGRESS                                 = 0x0045,
    MR_EVT_LD_INIT_START_FAST                               = 0x0046,
    MR_EVT_LD_INIT_START_FULL                               = 0x0047,
    MR_EVT_LD_INIT_SUCCESSFUL                               = 0x0048,
    MR_EVT_LD_PROP_CHANGE                                   = 0x0049,
    MR_EVT_LD_RECON_DONE                                    = 0x004a,
    MR_EVT_LD_RECON_DONE_ERRORS                             = 0x004b,
    MR_EVT_LD_RECON_DOUBLE_MEDIUM_ERRORS                    = 0x004c,
    MR_EVT_LD_RECON_PROGRESS                                = 0x004d,
    MR_EVT_LD_RECON_RESUME                                  = 0x004e,
    MR_EVT_LD_RECON_RESUME_FAILED                           = 0x004f,
    MR_EVT_LD_RECON_START                                   = 0x0050,
    MR_EVT_LD_STATE_CHANGE                                  = 0x0051,
    MR_EVT_PD_CLEAR_ABORTED                                 = 0x0052,
    MR_EVT_PD_CLEAR_FAILED                                  = 0x0053,
    MR_EVT_PD_CLEAR_PROGRESS                                = 0x0054,
    MR_EVT_PD_CLEAR_STARTED                                 = 0x0055,
    MR_EVT_PD_CLEAR_SUCCESSFUL                              = 0x0056,
    MR_EVT_PD_ERR                                           = 0x0057,
    MR_EVT_PD_FORMAT_COMPLETE                               = 0x0058,
    MR_EVT_PD_FORMAT_STARTED                                = 0x0059,
    MR_EVT_PD_HS_SMART_POLL_FAILED                          = 0x005a,
    MR_EVT_PD_INSERTED                                      = 0x005b,
    MR_EVT_PD_NOT_SUPPORTED                                 = 0x005c,
    MR_EVT_PD_PR_CORRECTED                                  = 0x005d,
    MR_EVT_PD_PR_PROGRESS                                   = 0x005e,
    MR_EVT_PD_PR_UNCORRECTABLE                              = 0x005f,
    MR_EVT_PD_PREDICTIVE_THRESHOLD_EXCEEDED                 = 0x0060,
    MR_EVT_PD_PUNCTURE                                      = 0x0061,
    MR_EVT_PD_RBLD_ABORT_BY_USER                            = 0x0062,
    MR_EVT_PD_RBLD_DONE_LD                                  = 0x0063,
    MR_EVT_PD_RBLD_DONE_PD                                  = 0x0064,
    MR_EVT_PD_RBLD_FAILED_BAD_SOURCE                        = 0x0065,
    MR_EVT_PD_RBLD_FAILED_BAD_TARGET                        = 0x0066,
    MR_EVT_PD_RBLD_PROGRESS                                 = 0x0067,
    MR_EVT_PD_RBLD_RESUME                                   = 0x0068,
    MR_EVT_PD_RBLD_START                                    = 0x0069,
    MR_EVT_PD_RBLD_START_AUTO                               = 0x006a,
    MR_EVT_PD_RBLD_STOP_BY_OWNERSHIP_LOSS                   = 0x006b,
    MR_EVT_PD_REASSIGN_WRITE_FAILED                         = 0x006c,
    MR_EVT_PD_REBUILD_MEDIUM_ERROR                          = 0x006d,
    MR_EVT_PD_REC_CORRECTING                                = 0x006e,
    MR_EVT_PD_RECOVER_MEDIUM_ERROR                          = 0x006f,
    MR_EVT_PD_REMOVED                                       = 0x0070,
    MR_EVT_PD_SENSE                                         = 0x0071,
    MR_EVT_PD_STATE_CHANGE                                  = 0x0072,
    MR_EVT_PD_STATE_CHANGE_BY_USER                          = 0x0073,
    MR_EVT_PD_REDUNDANT_PATH_BROKEN                         = 0x0074,       // DEPRECATED
    MR_EVT_PD_REDUNDANT_PATH_RESTORED                       = 0x0075,       // DEPRECATED
    MR_EVT_PD_DEDICATED_SPARE_NO_LONGER_USEFUL              = 0x0076,
    MR_EVT_SAS_TOPOLOGY_LOOP_DETECTED                       = 0x0077,
    MR_EVT_SAS_TOPOLOGY_UNADDRESSABLE_DEVICE                = 0x0078,
    MR_EVT_SAS_TOPOLOGY_MULTIPLE_PORTS_TO_SAME_ADDR         = 0x0079,
    MR_EVT_SAS_TOPOLOGY_EXPANDER_ERR                        = 0x007a,
    MR_EVT_SAS_TOPOLOGY_SMP_TIMEOUT                         = 0x007b,
    MR_EVT_SAS_TOPOLOGY_OUT_OF_ROUTE_ENTRIES                = 0x007c,
    MR_EVT_SAS_TOPOLOGY_INDEX_NOT_FOUND                     = 0x007d,
    MR_EVT_SAS_TOPOLOGY_SMP_FUNCTION_FAILED                 = 0x007e,
    MR_EVT_SAS_TOPOLOGY_SMP_CRC_ERROR                       = 0x007f,
    MR_EVT_SAS_TOPOLOGY_MULITPLE_SUBTRACTIVE                = 0x0080,
    MR_EVT_SAS_TOPOLOGY_TABLE_TO_TABLE                      = 0x0081,
    MR_EVT_SAS_TOPOLOGY_MULTIPLE_PATHS                      = 0x0082,
    MR_EVT_PD_UNUSABLE                                      = 0x0083,
    MR_EVT_PD_SPARE_DEDICATED_CREATED                       = 0x0084,
    MR_EVT_PD_SPARE_DEDICATED_DISABLED                      = 0x0085,
    MR_EVT_PD_SPARE_DEDICATED_NOT_USEFUL_FOR_ALL_ARRAYS     = 0x0086,
    MR_EVT_PD_SPARE_GLOBAL_CREATED                          = 0x0087,
    MR_EVT_PD_SPARE_GLOBAL_DISABLED                         = 0x0088,
    MR_EVT_PD_SPARE_GLOBAL_NOT_COVERING_ALL_ARRAYS          = 0x0089,
    MR_EVT_LD_CREATED                                       = 0x008a,
    MR_EVT_LD_DELETED                                       = 0x008b,
    MR_EVT_LD_INCONSISTENT_DUE_AT_STARTUP                   = 0x008c,
    MR_EVT_BBU_PRESENT                                      = 0x008d,
    MR_EVT_BBU_NOT_PRESENT                                  = 0x008e,
    MR_EVT_BBU_NEW_BATTERY_DETECTED                         = 0x008f,
    MR_EVT_BBU_REPLACED                                     = 0x0090,
    MR_EVT_BBU_TEMPERATURE_HIGH                             = 0x0091,
    MR_EVT_BBU_VOLTAGE_LOW                                  = 0x0092,
    MR_EVT_BBU_CHARGING                                     = 0x0093,
    MR_EVT_BBU_DISCHARGING                                  = 0x0094,
    MR_EVT_BBU_TEMPERATURE_NORMAL                           = 0x0095,
    MR_EVT_BBU_REPLACEMENT_NEEDED_SOH_BAD                   = 0x0096,
    MR_EVT_BBU_RELEARN_STARTED                              = 0x0097,
    MR_EVT_BBU_RELEARN_IN_PROGRESS                          = 0x0098,
    MR_EVT_BBU_RELEARN_COMPLETE                             = 0x0099,
    MR_EVT_BBU_RELEARN_TIMEOUT                              = 0x009a,
    MR_EVT_BBU_RELEARN_PENDING                              = 0x009b,
    MR_EVT_BBU_RELEARN_POSTPONED                            = 0x009c,
    MR_EVT_BBU_RELEARN_WILL_START_IN_4_DAYS                 = 0x009d,
    MR_EVT_BBU_RELEARN_WILL_START_IN_2_DAYS                 = 0x009e,
    MR_EVT_BBU_RELEARN_WILL_START_IN_1_DAYS                 = 0x009f,
    MR_EVT_BBU_RELEARN_WILL_START_IN_5_HOURS                = 0x00a0,
    MR_EVT_BBU_BATTERY_REMOVED                              = 0x00a1,
    MR_EVT_BBU_BATTERY_CAP_BELOW_SOH_THRESHOLD              = 0x00a2,
    MR_EVT_BBU_BATTERY_CAP_ABOVE_SOH_THRESHOLD              = 0x00a3,
    MR_EVT_ENCL_DISCOVERED_SES                              = 0x00a4,
    MR_EVT_ENCL_DISCOVERED_SAFTE                            = 0x00a5,
    MR_EVT_ENCL_COMMUNICATION_LOST                          = 0x00a6,
    MR_EVT_ENCL_COMMUNICATION_RESTORED                      = 0x00a7,
    MR_EVT_ENCL_FAN_FAILED                                  = 0x00a8,
    MR_EVT_ENCL_FAN_INSERTED                                = 0x00a9,
    MR_EVT_ENCL_FAN_REMOVED                                 = 0x00aa,
    MR_EVT_ENCL_POWER_FAILED                                = 0x00ab,
    MR_EVT_ENCL_POWER_INSERTED                              = 0x00ac,
    MR_EVT_ENCL_POWER_REMOVED                               = 0x00ad,
    MR_EVT_ENCL_SIM_FAILED                                  = 0x00ae,
    MR_EVT_ENCL_SIM_INSERTED                                = 0x00af,
    MR_EVT_ENCL_SIM_REMOVED                                 = 0x00b0,
    MR_EVT_ENCL_TEMPERATURE_BELOW_WARNING                   = 0x00b1,
    MR_EVT_ENCL_TEMPERATURE_BELOW_ERROR                     = 0x00b2,
    MR_EVT_ENCL_TEMPERATURE_ABOVE_WARNING                   = 0x00b3,
    MR_EVT_ENCL_TEMPERATURE_ABOVE_ERROR                     = 0x00b4,
    MR_EVT_ENCL_SHUTDOWN                                    = 0x00b5,
    MR_EVT_ENCL_MAX_PER_PORT_EXCEEDED                       = 0x00b6,
    MR_EVT_ENCL_FIRMWARE_MISMATCH                           = 0x00b7,
    MR_EVT_ENCL_BAD_SENSOR                                  = 0x00b8,
    MR_EVT_ENCL_BAD_PHY                                     = 0x00b9,
    MR_EVT_ENCL_UNSTABLE                                    = 0x00ba,
    MR_EVT_ENCL_HARDWARE_ERROR                              = 0x00bb,
    MR_EVT_ENCL_NOT_RESPONDING                              = 0x00bc,
    MR_EVT_ENCL_SAS_SATA_MIXING_DETECTED                    = 0x00bd,
    MR_EVT_ENCL_SES_HOTPLUG_DETECTED                        = 0x00be,
    MR_EVT_CLUSTER_ENABLED                                  = 0x00bf,
    MR_EVT_CLUSTER_DISABLED                                 = 0x00c0,
    MR_EVT_PD_TOO_SMALL_FOR_AUTO_REBUILD                    = 0x00c1,
    MR_EVT_BBU_GOOD                                         = 0x00c2,
    MR_EVT_BBU_BAD                                          = 0x00c3,
    MR_EVT_PD_BBM_LOG_80_PERCENT_FULL                       = 0x00c4,
    MR_EVT_PD_BBM_LOG_FULL                                  = 0x00c5,
    MR_EVT_LD_CC_OWNERSHIP_LOSS_ABORT                       = 0x00c6,
    MR_EVT_LD_BGI_OWNERSHIP_LOSS_ABORT                      = 0x00c7,
    MR_EVT_BBU_BATTERY_SOH_INVALID                          = 0x00c8,
    MR_EVT_CTRL_MEM_ECC_SINGLE_BIT_WARNING                  = 0x00c9,
    MR_EVT_CTRL_MEM_ECC_SINGLE_BIT_CRITICAL                 = 0x00ca,
    MR_EVT_CTRL_MEM_ECC_SINGLE_BIT_DISABLED                 = 0x00cb,
    MR_EVT_ENCL_POWER_SUPPLY_OFF                            = 0x00cc,
    MR_EVT_ENCL_POWER_SUPPLY_ON                             = 0x00cd,
    MR_EVT_ENCL_POWER_SUPPLY_CABLE_REMOVED                  = 0x00ce,
    MR_EVT_ENCL_POWER_SUPPLY_CABLE_INSERTED                 = 0x00cf,
    MR_EVT_ENCL_FAN_RETURNED_TO_NORMAL                      = 0x00d0,
    MR_EVT_DIAG_BBU_RETENTION_TEST_STARTED_ON_PREV_BOOT     = 0x00d1,
    MR_EVT_DIAG_BBU_RETENTION_PASSED                        = 0x00d2,
    MR_EVT_DIAG_BAT_RETENTION_TEST_FAILED                   = 0x00d3,
    MR_EVT_DIAG_NVRAM_RET_TEST_STARTED_ON_PREV_BOOT         = 0x00d4,
    MR_EVT_DIAG_NVRAM_RENTION_TEST_SUCCESS                  = 0x00d5,
    MR_EVT_DIAG_NVRAM_RENTION_TEST_FAILED                   = 0x00d6,
    MR_EVT_DIAG_SELF_CHECK_TEST_PASS                        = 0x00d7,
    MR_EVT_DIAG_SELF_CHECK_TEST_FAIL                        = 0x00d8,
    MR_EVT_DIAG_SELF_CHECK_DONE                             = 0x00d9,
    MR_EVT_FOREIGN_CFG_DETECTED                             = 0x00da,
    MR_EVT_FOREIGN_CFG_IMPORTED                             = 0x00db,
    MR_EVT_FOREIGN_CFG_CLEARED                              = 0x00dc,
    MR_EVT_NVRAM_CORRUPT                                    = 0x00dd,
    MR_EVT_NVRAM_MISMATCH                                   = 0x00de,
    MR_EVT_SAS_WIDE_PORT_LINK_LOST                          = 0x00df,
    MR_EVT_SAS_WIDE_PORT_LINK_RESTORED                      = 0x00e0,
    MR_EVT_SAS_PHY_ERROR_RATE_EXCEEDED                      = 0x00e1,
    MR_EVT_SATA_BAD_BLOCK_REMAPED                           = 0x00e2,
    MR_EVT_CTRL_HOTPLUG_DETECTED                            = 0x00e3,
    MR_EVT_ENCL_TEMPERATURE_DIFFERENTIAL                    = 0x00e4,
    MR_EVT_DIAG_DISK_TEST_CANNOT_START                      = 0x00e5,
    MR_EVT_DIAG_TIME_NOT_SUFFICIENT                         = 0x00e6,
    MR_EVT_PD_MARK_MISSING                                  = 0x00e7,
    MR_EVT_PD_REPLACE_MISSING                               = 0x00e8,
    MR_EVT_ENCL_TEMPERATURE_RETURNED_TO_NORMAL              = 0x00e9,
    MR_EVT_ENCL_FIRMWARE_FLASH_IN_PROGRESS                  = 0x00ea,
    MR_EVT_ENCL_FIRMWARE_DOWNLOAD_FAILED                    = 0x00eb,
    MR_EVT_PD_NOT_CERTIFIED                                 = 0x00ec,
    MR_EVT_CTRL_CACHE_DISCARD_BY_USER                       = 0x00ed,
    MR_EVT_CTRL_BOOT_MISSING_PDS                            = 0x00ee,
    MR_EVT_CTRL_BOOT_LDS_WILL_GO_OFFLINE                    = 0x00ef,
    MR_EVT_CTRL_BOOT_LDS_MISSING                            = 0x00f0,
    MR_EVT_CTRL_BOOT_CONFIG_MISSING                         = 0x00f1,
    MR_EVT_BBU_CHARGE_COMPLETE                              = 0x00f2,
    MR_EVT_ENCL_FAN_SPEED_CHANGED                           = 0x00f3,
    MR_EVT_PD_SPARE_DEDICATED_IMPORTED_AS_GLOBAL            = 0x00f4,
    MR_EVT_PD_NO_REBUILD_SAS_SATA_MIX_NOT_ALLOWED_IN_LD     = 0x00f5,
    MR_EVT_SEP_IS_BEING_REBOOTED                            = 0x00f6,
    MR_EVT_PD_INSERTED_EXT                                  = 0x00f7,
    MR_EVT_PD_REMOVED_EXT                                   = 0x00f8,
    MR_EVT_LD_OPTIMAL                                       = 0x00f9,
    MR_EVT_LD_PARTIALLY_DEGRADED                            = 0x00fa,
    MR_EVT_LD_DEGRADED                                      = 0x00fb,
    MR_EVT_LD_OFFLINE                                       = 0x00fc,
    MR_EVT_BBU_RELEARN_REQUESTED                            = 0x00fd,
    MR_EVT_LD_DISABLED_NO_SUPPORT_FOR_RAID5                 = 0x00fe,
    MR_EVT_LD_DISABLED_NO_SUPPORT_FOR_RAID6                 = 0x00ff,
    MR_EVT_LD_DISABLED_NO_SUPPORT_FOR_SAS                   = 0x0100,
    MR_EVT_CTRL_BOOT_MISSING_PDS_EXT                        = 0x0101,
    MR_EVT_CTRL_PUNCTURE_ENABLED                            = 0x0102,
    MR_EVT_CTRL_PUNCTURE_DISABLED                           = 0x0103,
    MR_EVT_ENCL_SIM_NOT_INSTALLED                           = 0x0104,
    MR_EVT_PACKAGE_VERSION                                  = 0x0105,
    MR_EVT_PD_SPARE_AFFINITY_IGNORED                        = 0x0106,
    MR_EVT_FOREIGN_CFG_TABLE_OVERFLOW                       = 0x0107,
    MR_EVT_FOREIGN_CFG_PARTIAL_IMPORT                       = 0x0108,
    MR_EVT_SAS_MUX_ACTIVATED_CONNECTOR                      = 0x0109,
    MR_EVT_CTRL_FRU                                         = 0x010a,
    MR_EVT_PD_COMMAND_TIMEOUT                               = 0x010b,
    MR_EVT_PD_RESET                                         = 0x010c,
    MR_EVT_LD_BBM_LOG_80_PERCENT_FULL                       = 0x010d,
    MR_EVT_LD_BBM_LOG_FULL                                  = 0x010e,
    MR_EVT_LD_LOG_MEDIUM_ERROR                              = 0x010f,
    MR_EVT_LD_CORRECTED_MEDIUM_ERROR                        = 0x0110,
    MR_EVT_PD_BBM_LOG_100_PERCENT_FULL                      = 0x0111,
    MR_EVT_LD_BBM_LOG_100_PERCENT_FULL                      = 0x0112,
    MR_EVT_DIAG_FAULTY_IOP_DETECTED                         = 0x0113,
    MR_EVT_PD_COPYBACK_START                                = 0x0114,
    MR_EVT_PD_COPYBACK_ABORT                                = 0x0115,
    MR_EVT_PD_COPYBACK_DONE                                 = 0x0116,
    MR_EVT_PD_COPYBACK_PROGRESS                             = 0x0117,
    MR_EVT_PD_COPYBACK_RESUME                               = 0x0118,
    MR_EVT_PD_COPYBACK_AUTO                                 = 0x0119,
    MR_EVT_PD_COPYBACK_FAILED                               = 0x011a,
    MR_EVT_CTRL_EPOW_UNSUCCESSFUL                           = 0x011b,
    MR_EVT_BBU_FRU                                          = 0x011c,
    MR_EVT_PD_FRU                                           = 0x011d,
    MR_EVT_CTRL_HW_REVISION                                 = 0x011e,
    MR_EVT_FOREIGN_CFG_UPGRADE_REQUIRED                     = 0x011f,
    MR_EVT_PD_SAS_REDUNDANT_PATH_RESTORED                   = 0x0120,
    MR_EVT_PD_SAS_REDUNDANT_PATH_BROKEN                     = 0x0121,
    MR_EVT_ENCL_REDUNDANT_ENCL_MODULE_INSERTED              = 0x0122,
    MR_EVT_ENCL_REDUNDANT_ENCL_MODULE_REMOVED               = 0x0123,
    MR_EVT_CTRL_PR_CANT_START                               = 0x0124,
    MR_EVT_PD_COPYBACK_ABORT_BY_USER                        = 0x0125,
    MR_EVT_PD_COPYBACK_ABORT_FOR_SPARE                      = 0x0126,
    MR_EVT_PD_COPYBACK_ABORT_FOR_REBUILD                    = 0x0127,
    MR_EVT_LD_CACHE_DISCARDED                               = 0x0128,
    MR_EVT_PD_TOO_SMALL_FOR_COPYBACK                        = 0x0129,
    MR_EVT_PD_NO_COPYBACK_SAS_SATA_MIX_NOT_ALLOWED_IN_LD    = 0x012a,
    MR_EVT_PD_FW_DOWNLOAD_START                             = 0x012b,
    MR_EVT_PD_FW_DOWNLOAD_COMPLETE                          = 0x012c,
    MR_EVT_PD_FW_DOWNLOAD_TIMEOUT                           = 0x012d,
    MR_EVT_PD_FW_DOWNLOAD_FAILED                            = 0x012e,
    MR_EVT_CTRL_PROP_CHANGED                                = 0x012f,
    MR_EVT_CTRL_PR_PROP_CHANGED                             = 0x0130,
    MR_EVT_CTRL_CC_SCHEDULE_PROP_CHANGED                    = 0x0131,
    MR_EVT_BBU_PROP_CHANGED                                 = 0x0132,
    MR_EVT_BBU_PERIODIC_RELEARN_PENDING                     = 0x0133,
    MR_EVT_CTRL_LOCK_KEY_CREATED                            = 0x0134,
    MR_EVT_CTRL_LOCK_KEY_BACKEDUP                           = 0x0135,
    MR_EVT_CTRL_LOCK_KEY_VERIFIED_ESCROW                    = 0x0136,
    MR_EVT_CTRL_LOCK_KEY_REKEYED                            = 0x0137,
    MR_EVT_CTRL_LOCK_KEY_REKEY_FAILED                       = 0x0138,
    MR_EVT_CTRL_LOCK_KEY_INVALID                            = 0x0139,
    MR_EVT_CTRL_LOCK_KEY_DESTROYED                          = 0x013a,
    MR_EVT_CTRL_ESCROW_KEY_INVALID                          = 0x013b,
    MR_EVT_LD_SECURED                                       = 0x013c,
    MR_EVT_LD_PARTIALLY_SECURED                             = 0x013d,
    MR_EVT_PD_SECURED                                       = 0x013e,
    MR_EVT_PD_UNSECURED                                     = 0x013f,
    MR_EVT_PD_REPROVISIONED                                 = 0x0140,
    MR_EVT_PD_LOCK_KEY_REKEYED                              = 0x0141,
    MR_EVT_PD_SECURITY_FAILURE                              = 0x0142,
    MR_EVT_LD_CACHE_PINNED                                  = 0x0143,
    MR_EVT_CTRL_BOOT_LDS_CACHE_PINNED                       = 0x0144,
    MR_EVT_CTRL_LDS_CACHE_DISCARDED_BY_USER                 = 0x0145,
    MR_EVT_LD_CACHE_DESTAGED                                = 0x0146,
    MR_EVT_LD_CC_STARTED_ON_INCONSISTENT_LD                 = 0x0147,
    MR_EVT_CTRL_LOCK_KEY_FAILED                             = 0x0148,
    MR_EVT_CTRL_SECRET_KEY_INVALID                          = 0x0149,
    MR_EVT_BBU_REMOTE_CONNECTOR_ERROR                       = 0x014a,
    MR_EVT_PD_POWER_STATE_CHANGE                            = 0x014b,
    MR_EVT_ENCL_ELEMENT_STATUS_CHANGED                      = 0x014c,
    MR_EVT_PD_NO_REBUILD_HDD_SSD_MIX_NOT_ALLOWED_IN_LD      = 0x014d,
    MR_EVT_PD_NO_COPYBACK_HDD_SSD_MIX_NOT_ALLOWED_IN_LD     = 0x014e,
    MR_EVT_LD_BBM_LOG_CLEARED                               = 0x014f,
    MR_EVT_SAS_TOPOLOGY_ERROR                               = 0x0150,
    MR_EVT_LD_CORRECTED_CLUSTER_OF_MEDIUM_ERRORS            = 0x0151,
    MR_EVT_CTRL_HOST_BUS_SCAN_REQUESTED                     = 0x0152,
    MR_EVT_CTRL_FACTORY_REPURPOSED                          = 0x0153,
    MR_EVT_CTRL_LOCK_KEY_BINDING_UPDATED                    = 0x0154,
    MR_EVT_CTRL_LOCK_KEY_EKM_MODE                           = 0x0155,
    MR_EVT_CTRL_LOCK_KEY_EKM_FAILURE                        = 0x0156,
    MR_EVT_PD_LOCK_KEY_REQUIRED                             = 0x0157,
    MR_EVT_LD_SECURE_FAILED                                 = 0x0158,
    MR_EVT_CTRL_ONLINE_RESET                                = 0x0159,
    MR_EVT_LD_SNAPSHOT_ENABLED                              = 0x015a,
    MR_EVT_LD_SNAPSHOT_DISABLED_BY_USER                     = 0x015b,
    MR_EVT_LD_SNAPSHOT_DISABLED_INTERNALLY                  = 0x015c,
    MR_EVT_LD_SNAPSHOT_PIT_CREATED                          = 0x015d,
    MR_EVT_LD_SNAPSHOT_PIT_DELETED                          = 0x015e,
    MR_EVT_LD_SNAPSHOT_VIEW_CREATED                         = 0x015f,
    MR_EVT_LD_SNAPSHOT_VIEW_DELETED                         = 0x0160,
    MR_EVT_LD_SNAPSHOT_ROLLBACK_STARTED                     = 0x0161,
    MR_EVT_LD_SNAPSHOT_ROLLBACK_ABORTED                     = 0x0162,
    MR_EVT_LD_SNAPSHOT_ROLLBACK_COMPLETED                   = 0x0163,
    MR_EVT_LD_SNAPSHOT_ROLLBACK_PROGRESS                    = 0x0164,
    MR_EVT_LD_SNAPSHOT_REPOSITORY_80_PERCENT_FULL           = 0x0165,
    MR_EVT_LD_SNAPSHOT_REPOSITORY_FULL                      = 0x0166,
    MR_EVT_LD_SNAPSHOT_VIEW_80_PERCENT_FULL                 = 0x0167,
    MR_EVT_LD_SNAPSHOT_VIEW_FULL                            = 0x0168,
    MR_EVT_LD_SNAPSHOT_REPOSITORY_LOST                      = 0x0169,
    MR_EVT_LD_SNAPSHOT_REPOSITORY_RESTORED                  = 0x016a,
    MR_EVT_LD_SNAPSHOT_INTERNAL_ERROR                       = 0x016b,
    MR_EVT_LD_SNAPSHOT_AUTO_ENABLED                         = 0x016c,
    MR_EVT_LD_SNAPSHOT_AUTO_DISABLED                        = 0x016d,
    MR_EVT_CFG_CMD_LOST                                     = 0x016e,
    MR_EVT_PD_COD_STALE                                     = 0x016f,
    MR_EVT_PD_POWER_STATE_CHANGE_FAILED                     = 0x0170,
    MR_EVT_LD_NOT_READY                                     = 0x0171,
    MR_EVT_LD_IS_READY                                      = 0x0172,
    MR_EVT_LD_IS_SSC                                        = 0x0173,
    MR_EVT_LD_IS_USING_SSC                                  = 0x0174,
    MR_EVT_LD_IS_NOT_USING_SSC                              = 0x0175,
    MR_EVT_LD_SNAPSHOT_FREED_RESOURCE                       = 0x0176,
    MR_EVT_LD_SNAPSHOT_AUTO_FAILED                          = 0x0177,
    MR_EVT_CTRL_RESET_EXPANDER                              = 0x0178,
    MR_EVT_LD_SSC_SIZE_CHANGED                              = 0x0179,
    MR_EVT_BBU_CANNOT_DO_TRANSPARENT_LEARN                  = 0x017a,
    MR_EVT_CTRL_PFK_APPLIED                                 = 0x017b,
    MR_EVT_LD_SNAPSHOT_SCHEDULE_PROP_CHANGE                 = 0x017c,
    MR_EVT_LD_SNAPSHOT_SCHEDULED_ACTION_DUE                 = 0x017d,
    MR_EVT_CTRL_PERF_COLLECTION                             = 0x017e,
    MR_EVT_CTRL_PFK_TRANSFERRED                             = 0x017f,
    MR_EVT_CTRL_PFK_SERIALNUM                               = 0x0180,
    MR_EVT_CTRL_PFK_SERIALNUM_MISMATCH                      = 0x0181,
    MR_EVT_BBU_REPLACEMENT_NEEDED_SOH_NOT_OPTIMAL           = 0x0182,
    MR_EVT_LD_POWER_STATE_CHANGE                            = 0x0183,
    MR_EVT_LD_POWER_STATE_MAX_UNAVAILABLE                   = 0x0184,
    MR_EVT_CTRL_HOST_DRIVER_LOADED                          = 0x0185,
    MR_EVT_LD_MIRROR_BROKEN                                 = 0x0186,
    MR_EVT_LD_MIRROR_JOINED                                 = 0x0187,
    MR_EVT_PD_SAS_WIDE_PORT_LINK_FAILURE                    = 0x0188,
    MR_EVT_PD_SAS_WIDE_PORT_LINK_RESTORED                   = 0x0189,
    MR_EVT_TMM_FRU                                          = 0x018a,
    MR_EVT_BBU_REPLACEMENT_NEEDED                           = 0x018b,
    MR_EVT_FOREIGN_CFG_AUTO_IMPORT_NONE                     = 0x018c,
    MR_EVT_BBU_MICROCODE_UPDATE_REQUIRED                    = 0x018d,
    MR_EVT_LD_SSC_SIZE_EXCEEDED                             = 0x018e,
    MR_EVT_LD_PI_LOST                                       = 0x018f,
    MR_EVT_PD_SHIELD_DIAG_PASS                              = 0x0190,
    MR_EVT_PD_SHIELD_DIAG_FAIL                              = 0x0191,
    MR_EVT_CTRL_SERVER_POWER_DIAG_STARTED                   = 0x0192,
    MR_EVT_PD_RBLD_DRIVE_CACHE_ENABLED                      = 0x0193,
    MR_EVT_PD_RBLD_DRIVE_CACHE_RESTORED                     = 0x0194,
    MR_EVT_PD_EMERGENCY_SPARE_COMMISSIONED                  = 0x0195,
    MR_EVT_PD_EMERGENCY_SPARE_COMMISSIONED_REMINDER         = 0x0196,
    MR_EVT_LD_CC_SUSPENDED                                  = 0x0197,
    MR_EVT_LD_CC_RESUMED                                    = 0x0198,
    MR_EVT_LD_BGI_SUSPENDED                                 = 0x0199,
    MR_EVT_LD_BGI_RESUMED                                   = 0x019a,
    MR_EVT_LD_RECON_SUSPENDED                               = 0x019b,
    MR_EVT_PD_RBLD_SUSPENDED                                = 0x019c,
    MR_EVT_PD_COPYBACK_SUSPENDED                            = 0x019d,
    MR_EVT_LD_CC_SUSPENDED_REMINDER                         = 0x019e,
    MR_EVT_LD_BGI_SUSPENDED_REMINDER                        = 0x019f,
    MR_EVT_LD_RECON_SUSPENDED_REMINDER                      = 0x01a0,
    MR_EVT_PD_RBLD_SUSPENDED_REMINDER                       = 0x01a1,
    MR_EVT_PD_COPYBACK_SUSPENDED_REMINDER                   = 0x01a2,
    MR_EVT_CTRL_PR_SUSPENDED_REMINDER                       = 0x01a3,
    MR_EVT_PD_ERASE_ABORTED                                 = 0x01a4,
    MR_EVT_PD_ERASE_FAILED                                  = 0x01a5,
    MR_EVT_PD_ERASE_PROGRESS                                = 0x01a6,
    MR_EVT_PD_ERASE_STARTED                                 = 0x01a7,
    MR_EVT_PD_ERASE_SUCCESSFUL                              = 0x01a8,
    MR_EVT_LD_ERASE_ABORTED                                 = 0x01a9,
    MR_EVT_LD_ERASE_FAILED                                  = 0x01aa,
    MR_EVT_LD_ERASE_PROGRESS                                = 0x01ab,
    MR_EVT_LD_ERASE_START                                   = 0x01ac,
    MR_EVT_LD_ERASE_SUCCESSFUL                              = 0x01ad,
    MR_EVT_LD_ERASE_POTENTIAL_LEAKAGE                       = 0x01ae,
    MR_EVT_BBU_CHARGE_DISABLED_OTC                          = 0x01af,
    MR_EVT_BBU_MICROCODE_UPDATED                            = 0x01b0,
    MR_EVT_BBU_MICROCODE_UPDATE_FAILED                      = 0x01b1,
    MR_EVT_LD_ACCESS_BLOCKED_SSC_OFFLINE                    = 0x01b2,
    MR_EVT_LD_SSC_DISASSOCIATE_START                        = 0x01b3,
    MR_EVT_LD_SSC_DISASSOCIATE_DONE                         = 0x01b4,
    MR_EVT_LD_SSC_DISASSOCIATE_FAILED                       = 0x01b5,
    MR_EVT_LD_SSC_DISASSOCIATE_PROGRESS                     = 0x01b6,
    MR_EVT_LD_SSC_DISASSOCIATE_ABORT_BY_USER                = 0x01b7,
    MR_EVT_SAS_PHY_LINK_SPEED_UPDATED                       = 0x01b8,
    MR_EVT_CTRL_PFK_DEACTIVATED                             = 0x01b9,
    MR_EVT_LD_ACCESS_UNBLOCKED                              = 0x01ba,
    MR_EVT_LD_IS_USING_SSC_2                                = 0x01bb,
    MR_EVT_LD_IS_NOT_USING_SSC_2                            = 0x01bc,
    MR_EVT_PD_PR_ABORTED                                    = 0x01bd,
    MR_EVT_PD_TRANSIENT_ERROR_DETECTED                      = 0x01be,
    MR_EVT_LD_PI_ERR_IN_CACHE                               = 0x01bf,
    MR_EVT_CTRL_FLASH_IMAGE_UNSUPPORTED                     = 0x01c0,
    MR_EVT_BBU_MODE_SET                                     = 0x01c1,
    MR_EVT_BBU_PERIODIC_RELEARN_RESCHEDULED                 = 0x01c2,
    MR_EVT_CTRL_RESETNOW_START                              = 0x01c3,
    MR_EVT_CTRL_RESETNOW_DONE                               = 0x01c4,
    MR_EVT_L3_CACHE_ERROR                                   = 0x01c5,
    MR_EVT_L2_CACHE_ERROR                                   = 0x01c6,
    MR_EVT_CTRL_BOOT_HEADLESS_MODE_HAD_ERRORS               = 0x01c7,
    MR_EVT_CTRL_BOOT_SAFE_MODE_FOR_ERRORS                   = 0x01c8,
    MR_EVT_CTRL_BOOT_ERROR_WARNING                          = 0x01c9,
    MR_EVT_CTRL_BOOT_ERROR_CRITICAL                         = 0x01ca,
    MR_EVT_CTRL_BOOT_ERROR_FATAL                            = 0x01cb,
    MR_EVT_HA_NODE_JOIN                                     = 0x01cc,
    MR_EVT_HA_NODE_BREAK                                    = 0x01cd,
    MR_EVT_HA_PD_IS_REMOTE                                  = 0x01ce,
    MR_EVT_HA_PD_IS_LOCAL                                   = 0x01cf,
    MR_EVT_HA_LD_IS_REMOTE                                  = 0x01d0,
    MR_EVT_HA_LD_IS_LOCAL                                   = 0x01d1,
    MR_EVT_HA_LD_TARGET_ID_CONFLICT                         = 0x01d2,
    MR_EVT_HA_LD_ACCESS_IS_SHARED                           = 0x01d3,
    MR_EVT_HA_LD_ACCESS_IS_EXCLUSIVE                        = 0x01d4,
    MR_EVT_HA_LD_INCOMPATIBLE                               = 0x01d5,
    MR_EVT_HA_PEER_INCOMPATIBLE                             = 0x01d6,
    MR_EVT_HA_HW_INCOMPATIBLE                               = 0x01d7,
    MR_EVT_HA_CTRLPROP_INCOMPATIBLE                         = 0x01d8,
    MR_EVT_HA_FW_VERSION_MISMATCH                           = 0x01d9,
    MR_EVT_HA_FEATURES_MISMATCH                             = 0x01da,
    MR_EVT_HA_CACHE_MIRROR_ONLINE                           = 0x01db,
    MR_EVT_HA_CACHE_MIRROR_OFFLINE                          = 0x01dc,
    MR_EVT_LD_ACCESS_BLOCKED_PEER_UNAVAILABLE               = 0x01dd,
    MR_EVT_BBU_UNSUPPORTED                                  = 0x01de,
    MR_EVT_PD_TEMP_HIGH                                     = 0x01df,
    MR_EVT_PD_TEMP_CRITICAL                                 = 0x01e0,
    MR_EVT_PD_TEMP_NORMAL                                   = 0x01e1,
    MR_EVT_PD_IO_THROTTLED                                  = 0x01e2,
    MR_EVT_PD_IO_NOT_THROTTLED                              = 0x01e3,
    MR_EVT_PD_LIFE                                          = 0x01e4,
    MR_EVT_PD_LIFE_NOT_OPTIMAL                              = 0x01e5,
    MR_EVT_PD_LIFE_CRITICAL                                 = 0x01e6,
    MR_EVT_PD_FAILURE_LOCKEDUP                              = 0x01e7,
    MR_EVT_CTRL_HOST_DRIVER_UPDATE_NEEDED                   = 0x01e8,
    MR_EVT_HA_POSSIBLE_PEER_COMM_LOSS                       = 0x01e9,
    MR_EVT_CTRL_FLASH_SIGNED_COMPONENT_NOT_PRESENT          = 0x01ea,
    MR_EVT_CTRL_FLASH_AUTHENTICATION_FAILURE                = 0x01eb,
    MR_EVT_LD_SET_BOOT_DEVICE                               = 0x01ec,
    MR_EVT_PD_SET_BOOT_DEVICE                               = 0x01ed,
    MR_EVT_BBU_TEMPERATURE_CHANGED                          = 0x01ee,
    MR_EVT_CTRL_TEMPERATURE_CHANGED                         = 0x01ef,
    MR_EVT_NVCACHE_BACKUP_UNAVAILABLE                       = 0x01f0,
    MR_EVT_NVCACHE_CONSIDER_REPLACEMENT                     = 0x01f1,
    MR_EVT_NVCACHE_INVALID                                  = 0x01f2,
    MR_EVT_BOOT_DEVICE_INVALID                              = 0x01f3,
    MR_EVT_HA_SSC_WB_POOL_SIZE_MISMATCH                     = 0x01f4,
    MR_EVT_HA_SSC_NONSHARED_VD_WB_ASSOCIATION               = 0x01f5,
    MR_EVT_CTRL_POWER_IO_THROTTLE_START                     = 0x01f6,
    MR_EVT_CTRL_POWER_IO_THROTTLE_STOP                      = 0x01f7,
    MR_EVT_CTRL_TUNABLE_PARAMETERS_CHANGED                  = 0x01f8,
    MR_EVT_CTRL_TEMP_WITHIN_OPTIMAL_RANGE                   = 0x01f9,
    MR_EVT_CTRL_TEMP_ABOVE_OPTIMAL_RANGE                    = 0x01fa,
    MR_EVT_HA_MODE_HA_FEATURE_SET                           = 0x01fb,
    MR_EVT_HA_MODE_SC_FEATURE_SET                           = 0x01fc,
    MR_EVT_NVCACHE_COMPONENT_MISMATCH                       = 0x01fd,
    MR_EVT_CTRL_ENTER_MAINTENANCE_MODE                      = 0x01fe,
    MR_EVT_CTRL_EXIT_MAINTENANCE_MODE                       = 0x01ff,
    MR_EVT_HA_TOPOLOGY_MODE                                 = 0x0200,
    MR_EVT_HA_TOPOLOGY_LD_INCOMPATIBLE                      = 0x0201,
    MR_EVT_HA_TOPOLOGY_PD_INCOMPATIBLE                      = 0x0202,
    MR_EVT_CTRL_CACHE_FLUSH_START                           = 0x0203,
    MR_EVT_CTRL_CACHE_FLUSH_DONE                            = 0x0204,
    MR_EVT_CTRL_CACHE_FLUSH_ABORT                           = 0x0205,
    MR_EVT_CTRL_PERSONALITY_CHANGE                          = 0x0206,
    MR_EVT_CFG_AUTO_CREATED                                 = 0x0207,
    MR_EVT_CTRL_SWZONE_ENABLED                              = 0x0208,
    MR_EVT_CTRL_SWZONE_DISABLED                             = 0x0209,
    MR_EVT_LD_INIT_ABORTED_DUE_TO_RESET                     = 0x020a,
    MR_EVT_HA_CTRL_LOCK_KEY_MISMATCH                        = 0x020b,
    MR_EVT_HA_CTRL_LOCK_KEY_MATCH                           = 0x020c,
    MR_EVT_HA_LD_COMPATIBLE                                 = 0x020d,
    MR_EVT_PD_OPERATION_DELAYED                             = 0x020e,
    MR_EVT_LD_OPERATION_DELAYED                             = 0x020f,
    MR_EVT_CTRL_GENERIC_MESSAGE                             = 0x0210,
    MR_EVT_LD_VF_MAP                                        = 0x0211,
    MR_EVT_CTRL_TEMP_CRITICAL                               = 0x0212,
    MR_EVT_CTRL_CHASSIS_SHUTDOWN                            = 0x0213,
    MR_EVT_CTRL_CHASSIS_RESTART                             = 0x0214,
    MR_EVT_CTRL_INFO_CHANGED                                = 0x0215,
    MR_EVT_LD_PARTIALLY_HIDDEN                              = 0x0216,
    MR_EVT_PD_NOT_ENTERPRISE_SELF_ENCRYPTING_DRIVE          = 0x0217,
    MR_EVT_CTRL_FORCE_FLASH                                 = 0x0218,
    MR_EVT_LD_ACCESS_POLICY_SET_TO_RW                       = 0x0219,
    MR_EVT_CTRL_FLASH_TRANSIENT_ERROR                       = 0x021a,
    MR_EVT_CTRL_FLASH_FAILED                                = 0x021b,
    MR_EVT_PD_LOCATE_START                                  = 0x021c,
    MR_EVT_PD_LOCATE_STOP                                   = 0x021d,
    MR_EVT_PD_PR_CONFLICT_ABORT                             = 0x021e,
    MR_EVT_PD_DEGRADED_MEDIA                                = 0x021f,
    MR_EVT_PD_DEGRADED_MEDIA_POOR_PERFORMANCE               = 0x0220,
    MR_EVT_BBU_SCAP_INFO                                    = 0x0221,
    MR_EVT_CTRL_PERSONALITY_PCI_CHANGE                      = 0x0222,
    MR_EVT_PD_INQUIRY_INFO                                  = 0x0223,
    MR_EVT_LD_TRANSPORT_READY                               = 0x0224,
    MR_EVT_LD_TRANSPORT_CLEAR                               = 0x0225,
    MR_EVT_SYSTEM_RESET_REQUIRED                            = 0x0226,
    MR_EVT_NVCACHE_BLOCK_RECOVERY_SKIPPED                   = 0x0227,
    MR_EVT_CTRL_AUTO_CFG_OPTION                             = 0x0228,
    MR_EVT_CTRL_AUTO_CFG_PARAMETERS_CHANGED                 = 0x0229,
    MR_EVT_LD_TRANSPORT_READY_PRESENT_REMINDER              = 0x022a,
    MR_EVT_CTRL_PROFILE_UPDATE_SUCCESSFUL                   = 0x022b,
    MR_EVT_CTRL_PROFILE_AUTOSELECTED                        = 0x022c,
    MR_EVT_CTRL_PROFILE_PHYSICAL_DEVICEID_CHANGE            = 0x022d,
    MR_EVT_CTRL_FRU_SAVE_STATE                              = 0x022e,
    MR_EVT_CTRL_FRU_RESTORE_STATE                           = 0x022f,
    MR_EVT_CTRL_FRU_DELETE_STATE                            = 0x0230,
    MR_EVT_PD_ERROR_STATS                                   = 0x0231,
    MR_EVT_CTRL_MFC_BAD_SASADDRESS                          = 0x0232,
    MR_EVT_SAS_TOPOLOGY_ERROR_STR                           = 0x0233,
    MR_EVT_CTRL_NVDATA_INVALID                              = 0x0234,
    MR_EVT_CFG_NOT_CONFIGURED                               = 0x0235,
    MR_EVT_CFG_UNSUPPORTED                                  = 0x0236,
    MR_EVT_CTRL_FLASH_BACKUP_ROM_PGM_ERROR                  = 0x0237,
    MR_EVT_CTRL_FLASH_CRC_ERR_BACKUP_ACTIVATED              = 0x0238,
    MR_EVT_PD_NOT_GENUINE                                   = 0x0239,
    MR_EVT_PD_FAILED_AT_BOOT                                = 0x023a,
    MR_EVT_CTRL_CACHE_DATA_FOUND                            = 0x023b,
    MR_EVT_LD_DEGRADED_WRITE_HOLE                           = 0x023c,
    MR_EVT_CTRL_FEATURE_KEY_COMM_ERROR                      = 0x023d,
    MR_EVT_CTRL_FLASH_IMAGE_ACTIVE                          = 0x023e,
    MR_EVT_CTRL_FLASH_IMAGE_BACKUP                          = 0x023f,
    MR_EVT_CTRL_DUAL_FW_VERSION_MISMATCH                    = 0x0240,
    MR_EVT_PACKAGE_VERSION_BACKUP                           = 0x0241,
    MR_EVT_CTRL_CACHE_RESTORE_ERROR                         = 0x0242,
    MR_EVT_PD_LOCK_KEY_UNLOCKED_ESCROW                      = 0x0243,
    MR_EVT_PD_TEMP_FATAL                                    = 0x0244,
    MR_EVT_CTRL_MEMORY_ERROR                                = 0x0245,
    MR_EVT_CTRL_FW_INCOMPATIBLE_WITH_PROFILE                = 0x0246,
    MR_EVT_PD_ARRAY_DEVICE_SLOT_FAILED                      = 0x0247,
    MR_EVT_ENCL_TEMPERATURE_ABOVE_CRITICAL                  = 0x0248,
    MR_EVT_ENCL_SERVICE_CTRL_EMM_CRITICAL                   = 0x0249,
    MR_EVT_ENCL_SERVICE_CTRL_EMM_WARNING                    = 0x024a,
    MR_EVT_ENCL_ELEMENT_CRITICAL                            = 0x024b,
    MR_EVT_ENCL_ELEMENT_WARNING                             = 0x024c,
    MR_EVT_ENCL_SAS_CONNECTOR_ERROR                         = 0x024d,
    MR_EVT_CTRL_FW_VALID_SNAPDUMP_AVAILABLE                 = 0x024e,
    MR_EVT_CTRL_BOOT_LDS_WILL_GO_OFFLINE_EXT                = 0x024f,
    MR_EVT_CTRL_BOOT_LDS_MISSING_EXT                        = 0x0250,
    MR_EVT_CTRL_LDS_CACHE_DISCARDED_AUTO                    = 0x0251,
    MR_EVT_PD_SANITIZE_FAILED                               = 0x0252,
    MR_EVT_PD_SANITIZE_PROGRESS                             = 0x0253,
    MR_EVT_PD_SANITIZE_STARTED                              = 0x0254,
    MR_EVT_PD_SANITIZE_SUCCESSFUL                           = 0x0255,
    MR_EVT_LD_CANNOT_BE_SECURED_IN_FUTURE                   = 0x0256,
    MR_EVT_SYSTEM_SHUTDOWN_REQUIRED                         = 0x0257,
    MR_EVT_LD_NON_UNMAP_CAPABLE_PD_USED                     = 0x0258,
    MR_EVT_LD_UNMAP_NOT_ENABLED                             = 0x0259,
    MR_EVT_PD_SSD_WEAR_GAUGE_CHANGED                        = 0x025a,
    MR_EVT_CTRL_SB_KEY_UPDATE_POWER_CYCLE                   = 0x025b,
    MR_EVT_PD_DEDICATED_NON_UNMAP_SPARE_NO_LONGER_USEFUL    = 0x025c,
    MR_EVT_PD_NO_UNMAP_CAPABLE_PD_FOR_REBUILD               = 0x025d,
    MR_EVT_PD_NO_UNMAP_CAPABLE_PD_FOR_COPYBACK              = 0x025e,
    MR_EVT_COUNT
} MR_EVT;

#endif /* INCLUDE_EVENT */
