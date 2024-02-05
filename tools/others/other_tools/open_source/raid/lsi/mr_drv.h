/****************************************************************
 * FILE        : MR_DRV.H
 * MODULE      : SAS
 * DESCRIPTION : SAS DRIVER IOCTL DETAILS Header File
 * AUTHOR      : Parag Maharana
 * DATE        : 12-20-2004
 *****************************************************************
    Copyright (c) 2004 LSI Logic Corporation.
 *****************************************************************

 *****************************************************************
 * Revision History:
 * 12-20-2004 : Parag Maharana : Initial Release.
 * 12-30-2004 : Parag Maharana : Added AdapterId call for windows.
 * 01-07-2005 : HNR/SB         : Added Linux IOCTLS in discussion with Linux
 *                               Driver Group
 * 01-12-2005 : PRM/SB         : Removed MFI_EVENT_DATA, added SIZE
 *                               for Version and Statistics
 * 05-20-2005 : PRM            : Added New IOCTL for Show/Hide ALL
 *                               Disk PDs when LD present.
 * 05-25-2005 : Gurpreet/SB    : Added MR_DRV_ADAPTER_ID_INFO to return
 *                               ControllerId for MR_WIN_GET_ADAPTER_ID
 * 12-02-2010 : PM/SB          : Added Driver Parameters for Poll mode
 *                               and rescan
 * 10-01-2013 : BH             : Added MR_DRV_SYSTEM_INFO
 *
 * 12-03-2013 : SM             : Added dcmd.mbox.b[0] definition
 *                               for MR_DRIVER_IOCTL_MEGARAID_STATISTICS
 *                             : Add new structures/unions
 *                               MR_DRV_MEGARAID_LD_STATISTICS_EXT
 *
 * 12-12-2013 : NK             : Added ioctl MR_DRIVER_SET_APP_CRASHDUMP_MODE
 * 02-12-2014 : BH/SteveHagan  : Added Raptor ioctls as follows:
 *                                   MR_WIN_RAPTOR_GET_STATS, MR_WIN_RAPTOR_GET_THRESHOLDS,
 *                                   MR_WIN_RAPTOR_SET_THRESHOLDS, MR_WIN_RAPTOR_CLEAR_STATS
 *                                   MR_WIN_RAPTOR_STATS_CONTROL
 *                               Added structure MR_RAPTOR_STATUS
 *
 * 07-25-2014 : NK             : Added domainID to struct MR_DRV_PCI_INFORMATION
 *
 * 04-05-2018 : SH             : Added RTTrace IOCTL MR_DRIVER_CTRL_RTT_READ
 *                               Added structure    MR_RTTRACE_HEADER
 *                               Added structure    MR_RTT_DATA
 *
 *
 * 06-29-2018 : SH            : Added RTTrace IOCTL  MR_DRIVER_CTRL_RTT_PARAMS
 *								Added Structure MR_RTT_PARAMS
 *
 *
 *
 ******************************************************************/


#ifndef _MR_DRV_H
#define _MR_DRV_H


#ifdef  __cplusplus
extern "C" {
#endif


/****************************************************************************
*
*        IOCTL MACROS
*
****************************************************************************/


#define MR_DRIVER_IOCTL_MAIN                0xF0000000

#define MR_DRIVER_IOCTL_COMMON              0xF0010000
#define MR_DRIVER_IOCTL_WINDOWS             0xF0020000
#define MR_DRIVER_IOCTL_OTHER               0xF0040000
#define MR_DRIVER_IOCTL_LINUX               0xF0040000
#define MR_DRIVER_IOCTL_NETWARE             0xF0040000


/****************************************************************************
*
*        COMMON DRIVER IOCTL OPCODE
*
****************************************************************************/

/* Input  : ioctlHeader.win.controlCode  - MR_DRIVER_IOCTL_COMMON
 *          mfiFrame.dcmd.opcode         - MR_DRIVER_IOCTL_DRIVER_VERSION
 *          dataBuffer                   - DRIVER_VERSION struct
 * Desc   : Driver returns DRIVER_VERSION information to application
 */
#define MR_DRIVER_IOCTL_DRIVER_VERSION       (0xF0010100)

/* Input  : ioctlHeader.win.controlCode  - MR_DRIVER_IOCTL_COMMON
 *          mfiFrame.dcmd.opcode         - MR_DRIVER_IOCTL_PCI_INFORMATION
 *          dataBuffer                   - PCI_INFORMATION struct
 * Desc   : Driver returns PCI_INFORMATION information to application
 */
#define MR_DRIVER_IOCTL_PCI_INFORMATION      (0xF0010200)

/* Input  : ioctlHeader.win.controlCode  - MR_DRIVER_IOCTL_COMMON
 *          mfiFrame.dcmd.opcode         - MR_DRIVER_IOCTL_MEGARAID_STATISTICS
 *          dcmd.mbox.b[0]               - 0 - supports MAX_API_LOGICAL_DRIVES_LEGACY lds and uses legacy MR_DRV_MEGARAID_LD_STATISTICS
 *                                       - 1 - supports MAX_API_LOGICAL_DRIVES_EXT lds and uses extended struct MR_DRV_MEGARAID_LD_STATISTICS_EXT
 *          dataBuffer                   - MR_DRV_MEGARAID_LD_STATISTICS/MR_DRV_MEGARAID_LD_STATISTICS_EXT struct
 * Desc   : Driver returns MEGARAID_LD_STATISTICS information to application
 */
#define MR_DRIVER_IOCTL_MEGARAID_STATISTICS  (0xF0010300)



/* Input  : ioctlHeader.win.controlCode  - MR_DRIVER_IOCTL_COMMON
 *          mfiFrame.dcmd.opcode         - MR_DRIVER_GET_LD_PD_MODE
 *          mfiFrame.dcmd.mbox           - reserved
 *          dataBuffer                   - MR_DRV_LD_PD_MODE (U32)
 * Desc   : Driver will send 4 byte MR_DRV_LD_PD_MODE mode to application
 * Status : Return success if driver supports else failure
 */
#define MR_DRIVER_GET_LD_PD_MODE             (0xF0010400)

/* Input  : ioctlHeader.win.controlCode  - MR_DRIVER_IOCTL_COMMON
 *          mfiFrame.dcmd.opcode         - MR_DRIVER_SET_LD_PD_MODE
 *          mfiFrame.dcmd.mbox           - reserved
 *          dataBuffer                   - MR_DRV_LD_PD_MODE (U32)
 * Desc   : Driver will receive 4 bytes MR_DRV_LD_PD_MODE mode to application
 * Status : Return success if driver supports else failure
 */
#define MR_DRIVER_SET_LD_PD_MODE             (0xF0010500)

/* Input  : ioctlHeader.win.controlCode - MR_DRIVER_IOCTL_COMMON
 *          mfiFrame.dcmd.opcode        - MR_DRIVER_IOCTL_SUPPORT_CRASHDUMP
 *          dcmd.mbox.b[0]              -  1: support crash dump
 *                                      -  0: remove crash dump support
 * Desc   : Application will inform driver about crash dump collection capability
 * Status : Return success if driver supports else failure
 */

#define MR_DRIVER_SET_APP_CRASHDUMP_MODE (MR_DRIVER_IOCTL_COMMON | 0x0600)

/****************************************************************************
*
*        WINDOWS DRIVER IOCTL OPCODE
*
****************************************************************************/

/* Input  : ioctlHeader.win.controlCode  - MR_DRIVER_IOCTL_WINDOWS
 *          mfiFrame.dcmd.opcode         - MR_WIN_RESCAN_CHANNEL
 *          mfiFrame.dcmd.mbox0          - Channel
 *                                         1 : Shared Channel,
 *                                         2 : Dedicated Channel
 * Desc   : Application sends rescan request to Driver for particular channel
 *          specified in MAILBOX0
 * Status : Return success if driver supports else failure
 */

#define MR_WIN_RESCAN_CHANNEL             (MR_DRIVER_IOCTL_WINDOWS | 0x0100)

/* Input  : ioctlHeader.win.controlCode  - MR_DRIVER_IOCTL_WINDOWS
 *          mfiFrame.dcmd.opcode         - MR_WIN_AEN_MASK
 *          mfiFrame.dcmd.mbox           - reserved
 *          dataBuffer                   - MR_EVT_CLASS_LOCALE struct
 * Desc   : Application sends MASK information in MR_EVT_CLASS_LOCALE struct
 *          to driver.
 * Status : Return success if driver supports else failure
 */
#define MR_WIN_AEN_MASK                   (MR_DRIVER_IOCTL_WINDOWS | 0x0200)

/* Input  : ioctlHeader.win.controlCode  - MR_DRIVER_IOCTL_WINDOWS
 *          mfiFrame.dcmd.opcode         - MR_WIN_GET_ADAPTER_ID
 *          mfiFrame.dcmd.mbox           - reserved
 *          dataBuffer                   - AdapterId (U32)
 * Desc   : Driver will send 4 byte adapter Id (instance Id) to application
 * Status : Return success if driver supports else failure
 */
#define MR_WIN_GET_ADAPTER_ID             (MR_DRIVER_IOCTL_WINDOWS | 0x0300)

/* Input  : ioctlHeader.win.controlCode  - MR_DRIVER_IOCTL_WINDOWS
 *          mfiFrame.dcmd.opcode         - MR_WIN_RAPTOR_GET_STATS
 *          mfiFrame.dcmd.mbox           - reserved
 *          dataBuffer                   - MR_RAPTOR_STATS
 * Desc   : Application sends MR_RAPTOR_STATS structure to driver
 * Status : Return success if driver supports else failure
 */
#define MR_WIN_RAPTOR_GET_STATS           (MR_DRIVER_IOCTL_WINDOWS | 0x0600)

/* Input  : ioctlHeader.win.controlCode  - MR_DRIVER_IOCTL_WINDOWS
 *          mfiFrame.dcmd.opcode         - MR_WIN_RAPTOR_GET_THRESHOLDS
 *          mfiFrame.dcmd.mbox           - reserved
 *          dataBuffer                   - MR_BUCKET_LIST
 * Desc   : Application sends MR_BUCKET_LIST structure to driver
 * Status : Return success if driver supports else failure
 */
#define MR_WIN_RAPTOR_GET_THRESHOLDS      (MR_DRIVER_IOCTL_WINDOWS | 0x0700)

/* Input  : ioctlHeader.win.controlCode  - MR_DRIVER_IOCTL_WINDOWS
 *          mfiFrame.dcmd.opcode         - MR_WIN_RAPTOR_SET_THRESHOLDS
 *          mfiFrame.dcmd.mbox           - reserved
 *          dataBuffer                   - MR_BUCKET_LIST
 * Desc   : Application sends MR_BUCKET_LIST structure to driver
 * Status : Return success if driver supports else failure
 */
#define MR_WIN_RAPTOR_SET_THRESHOLDS      (MR_DRIVER_IOCTL_WINDOWS | 0x0800)

/* Input  : ioctlHeader.win.controlCode  - MR_DRIVER_IOCTL_WINDOWS
 *          mfiFrame.dcmd.opcode         - MR_WIN_RAPTOR_CLEAR_STATS
 *          mfiFrame.dcmd.mbox           - reserved
 * Status : Return success if driver supports else failure
 */
#define MR_WIN_RAPTOR_CLEAR_STATS         (MR_DRIVER_IOCTL_WINDOWS | 0x0900)

/* Input  : ioctlHeader.win.controlCode  - MR_DRIVER_IOCTL_WINDOWS
 *          mfiFrame.dcmd.opcode         - MR_WIN_RAPTOR_STATS_CONTROL
 *          mfiFrame.dcmd.mbox           - reserved
 * Status : Return success if driver supports else failure
 */
#define MR_WIN_RAPTOR_STATS_CONTROL       (MR_DRIVER_IOCTL_WINDOWS | 0x0A00)

/* Input  :ioctlHeader.win.controlCode   - MR_DRIVER_IOCTL_WINDOWS
 *         mfiFrame.dcmd.opcode          - MR_DRIVER_CTRL_RTT_READ
 *         mfiFrame.dcmd.mbox.b[0]       - 0 :  Driver will copy only RTTRACE_HEADER
 *         mfiFrame.dcmd.mbox.b[0]       - 1 :  Driver will copy the entire RTTRACE buffer
 *         dataBuffer                    - for mbox.b[0] = 0, it will be RTTRACE_HEADER strucuture.
 *                                       - for mbox.b[0] = 1, it will be MR_RTT_DATA structure.
 * Desc   : will copy the buffer from driver to application.
 * Status : MFI_STAT_MEMORY_NOT_AVAILABLE  - Memory is not available for RTTrace.
 *          MFI_STAT_OK                    - SUCCESS
 *          MFI_STAT_INVALID_PARAMETER     - For any sanity check failure.
 */
#define MR_DRIVER_CTRL_RTT_READ           (MR_DRIVER_IOCTL_WINDOWS | 0x0B00)

 /* Input :ioctlHeader.win.controlCode   - MR_DRIVER_IOCTL_WINDOWS
 *         mfiFrame.dcmd.opcode          - MR_DRIVER_CTRL_RTT_PARAMS
 *         mfiFrame.dcmd.mbox.b[0]       - 0 :  StorCLI will read PARAMS
 *         mfiFrame.dcmd.mbox.b[0]       - 1 :  StorCLI will set PARAMS
 *         dataBuffer                    - MR_RTT_PARAMS structure defined below.
 *
 * Desc   : will READ/SET PARAMS from the application.
 * Status :
 *          MFI_STAT_OK- SUCCESS
 *          MFI_STAT_INVALID_PARAMETER     -For any sanity check failure.
 */

#define MR_DRIVER_CTRL_RTT_PARAMS         (MR_DRIVER_IOCTL_WINDOWS | 0x0C00)


/****************************************************************************
*
*        LINUX/NETWARE DRIVER IOCTL OPCODE
*
****************************************************************************/

/* Input  : ioctlHeader.other.controlCode   - MR_DRIVER_IOCTL_LINUX
 *          mfiFrame.dcmd.opcode            - MR_LINUX_GET_ADAPTER_COUNT
 *          mfiFrame.dcmd.mbox              - reserved
 *          dataBuffer                      - Adapter count (U32)
 * Desc   : Driver will send 4 byte adapter count to application
 * Status : Return success if driver supports else failure
 */
#define MR_LINUX_GET_ADAPTER_COUNT         (MR_DRIVER_IOCTL_LINUX | 0x0100)

/* Input  : ioctlHeader.other.controlCode   - MR_DRIVER_IOCTL_LINUX
 *          mfiFrame.dcmd.opcode            - MR_LINUX_GET_ADAPTER_MAP
 *          mfiFrame.dcmd.mbox              - reserved
 *          dataBuffer                      - list of adapter Ids (U16) * adapter count
 * Desc   : Driver will send list of adapter ids to application
 * Status : Return success if driver supports else failure
 */
#define MR_LINUX_GET_ADAPTER_MAP           (MR_DRIVER_IOCTL_LINUX | 0x0200)

/* Input  : ioctlHeader.other.controlCode   - MR_DRIVER_IOCTL_LINUX
 *          ioctlHeader.other.controllerId  - adapter Id
 *          mfiFrame.dcmd.opcode            - MR_LINUX_GET_AEN
 *          mfiFrame.dcmd.mbox.w[0]         - starting sequence number
 *          mfiFrame.dcmd.mbox.w[1]         - MR_EVT_CLASS_LOCALE desired class/locale event types wanted
 *          dataBuffer                      - None
 * Desc   : Driver will wait for the specified event to happen from firmware.
 *          IOCTL will return immediately.
 *          When the event occurs, driver will invoke AEN signal.
 * Status : Return success if driver supports else failure
 */
#define MR_LINUX_GET_AEN                   (MR_DRIVER_IOCTL_LINUX | 0x0300)

/* Input  : ioctlHeader.other.controlCode   - MR_DRIVER_IOCTL_LINUX
 *          ioctlHeader.other.controllerId  - adapter Id
 *          mfiFrame.dcmd.opcode            - MR_LINUX_GET_GROUNDED_AEN
 *          mfiFrame.dcmd.mbox.w[0]         - starting sequence number
 *          mfiFrame.dcmd.mbox.w[1]         - MR_EVT_CLASS_LOCALE desired class/locale event types wanted
 *          dataBuffer                      - None
 * Desc   : Driver will wait for the specified event to happen from firmware.
 *          The IOCTL will not return till the event occurs.
 * Status : Return success if driver supports else failure
 */
#define MR_LINUX_GET_GROUNDED_AEN          (MR_DRIVER_IOCTL_LINUX | 0x0400)

/* Input  : ioctlHeader.other.controlCode   - MR_DRIVER_IOCTL_LINUX
 *          ioctlHeader.other.controllerId  - adapter Id
 *          mfiFrame.dcmd.opcode            - MR_LINUX_GET_DRIVER_PARAMETERS
 *          mfiFrame.dcmd.mbox              - reserved
 *          dataBuffer                      - MR_DRIVER_PARAMETERS
 * Desc   : Returns driver specific parameters
 * Status : Return success if driver supports else failure
 */
#define MR_LINUX_GET_DRIVER_PARAMETERS     (MR_DRIVER_IOCTL_LINUX | 0x0500)


/****************************************************************************
*
*        DRIVER VERSION INFORMATION
*
****************************************************************************/

typedef struct _MR_DRV_DRIVER_VERSION
{
    U8    driverSignature[12];
    U8    osName[16];
    U8    osVersion[12];
    U8    driverName[20];
    U8    driverVersion[32];
    U8    driverReleaseDate[20];
} MR_DRV_DRIVER_VERSION, *PMR_DRV_DRIVER_VERSION;

#define MR_DRV_DRIVER_VERSION_SIZE sizeof(MR_DRV_DRIVER_VERSION)

/****************************************************************************
*
*        PCI INFORMATION
*
****************************************************************************/


#define PCI_TYPE0_ADDRESSES             6
#define PCI_TYPE1_ADDRESSES             2
#define PCI_TYPE2_ADDRESSES             5

typedef struct _MR_DRV_PCI_COMMON_HEADER
{
    U16  vendorID;                   // (ro)
    U16  deviceID;                   // (ro)
    U16  command;                    // Device control
    U16  status;
    U8   revisionID;                 // (ro)
    U8   progIf;                     // (ro)
    U8   subClass;                   // (ro)
    U8   baseClass;                  // (ro)
    U8   cacheLineSize;              // (ro+)
    U8   latencyTimer;               // (ro+)
    U8   headerType;                 // (ro)
    U8   bist;                       // Built in self test

    union
    {
        struct _MR_DRV_PCI_HEADER_TYPE_0
        {
            U32   baseAddresses[PCI_TYPE0_ADDRESSES];
            U32   cis;
            U16   subVendorID;
            U16   subSystemID;
            U32   romBaseAddress;
            U8    capabilitiesPtr;
            U8    reserved1[3];
            U32   reserved2;
            U8    interruptLine;      //
            U8    interruptPin;       // (ro)
            U8    minimumGrant;       // (ro)
            U8    maximumLatency;     // (ro)
        } type0;

        //
        // PCI to PCI Bridge
        //

        struct _MR_DRV_PCI_HEADER_TYPE_1
        {
            U32   baseAddresses[PCI_TYPE1_ADDRESSES];
            U8    primaryBus;
            U8    secondaryBus;
            U8    subordinateBus;
            U8    secondaryLatency;
            U8    ioBase;
            U8    ioLimit;
            U16   secondaryStatus;
            U16   memoryBase;
            U16   memoryLimit;
            U16   prefetchBase;
            U16   prefetchLimit;
            U32   prefetchBaseUpper32;
            U32   prefetchLimitUpper32;
            U16   ioBaseUpper16;
            U16   ioLimitUpper16;
            U8    capabilitiesPtr;
            U8    reserved1[3];
            U32   romBaseAddress;
            U8    interruptLine;
            U8    interruptPin;
            U16   bridgeControl;
        } type1;

        //
        // PCI to CARDBUS Bridge
        //

        struct _MR_DRV_PCI_HEADER_TYPE_2
        {
            U32   socketRegistersBaseAddress;
            U8    capabilitiesPtr;
            U8    reserved;
            U16   secondaryStatus;
            U8    primaryBus;
            U8    secondaryBus;
            U8    subordinateBus;
            U8    secondaryLatency;
            struct
            {
                U32   base;
                U32   limit;
            }    range[PCI_TYPE2_ADDRESSES-1];
            U8   interruptLine;
            U8   interruptPin;
            U16  bridgeControl;
        } type2;
    } u;

} MR_DRV_PCI_COMMON_HEADER, *PMR_DRV_PCI_COMMON_HEADER;

#define MR_DRV_PCI_COMMON_HEADER_SIZE sizeof(MR_DRV_PCI_COMMON_HEADER)   //64 bytes

typedef struct _MR_DRV_PCI_LINK_CAPABILITY
{
    union
    {
        struct
        {
            U32 linkSpeed         :4;
            U32 linkWidth         :6;
            U32 aspmSupport       :2;
            U32 losExitLatency    :3;
            U32 l1ExitLatency     :3;
            U32 rsvdp             :6;
            U32 portNumber        :8;
        }bits;

        U32 asUlong;
    }u;

}MR_DRV_PCI_LINK_CAPABILITY, *PMR_DRV_PCI_LINK_CAPABILITY;

#define MR_DRV_PCI_LINK_CAPABILITY_SIZE sizeof(MR_DRV_PCI_LINK_CAPABILITY)

typedef struct _MR_DRV_PCI_LINK_STATUS_CAPABILITY
{
    union
    {
        struct
        {
            U16 linkSpeed             :4;
            U16 negotiatedLinkWidth   :6;
            U16 linkTrainingError     :1;
            U16 linkTraning           :1;
            U16 slotClockConfig       :1;
            U16 rsvdZ                  :3;
        }bits;

        U16 asUshort;
    }u;

    U16 reserved;

} MR_DRV_PCI_LINK_STATUS_CAPABILITY, *PMR_DRV_PCI_LINK_STATUS_CAPABILITY;

#define MR_DRV_PCI_LINK_STATUS_CAPABILITY_SIZE sizeof(MR_DRV_PCI_LINK_STATUS_CAPABILITY)


typedef struct _MR_DRV_PCI_CAPABILITIES
{
    MR_DRV_PCI_LINK_CAPABILITY         linkCapability;
    MR_DRV_PCI_LINK_STATUS_CAPABILITY  linkStatusCapability;

}MR_DRV_PCI_CAPABILITIES, *PMR_DRV_PCI_CAPABILITIES;

#define MR_DRV_PCI_CAPABILITIES_SIZE sizeof(MR_DRV_PCI_CAPABILITIES)

typedef struct _MR_DRV_PCI_INFORMATION
{
    U32                        busNumber;
    U8                         deviceNumber;
    U8                         functionNumber;
    U8                         interruptVector;
    U8                         reserved;
    MR_DRV_PCI_COMMON_HEADER   pciHeaderInfo;
    MR_DRV_PCI_CAPABILITIES    capability;
    U32                        domainID;
    U8                         reserved2[28];
}MR_DRV_PCI_INFORMATION, *PMR_DRV_PCI_INFORMATION;


#define MR_DRV_PCI_INFORMATION_SIZE sizeof(MR_DRV_PCI_INFORMATION)

/****************************************************************************
*
*        LOGICAL DRIVE IO STATISTICS
*
****************************************************************************/

typedef struct _MR_DRV_MEGARAID_LD_STATISTICS
{
    U32   numberOfIoReads[MAX_API_LOGICAL_DRIVES_LEGACY];
    U32   numberOfIoWrites[MAX_API_LOGICAL_DRIVES_LEGACY];
    U32   numberOfBlocksRead[MAX_API_LOGICAL_DRIVES_LEGACY];
    U32   numberOfBlocksWritten[MAX_API_LOGICAL_DRIVES_LEGACY];
    U32   numberOfReadFailures[MAX_API_LOGICAL_DRIVES_LEGACY];
    U32   numberOfWriteFailures[MAX_API_LOGICAL_DRIVES_LEGACY];

} MR_DRV_MEGARAID_LD_STATISTICS, *PMR_DRV_MEGARAID_LD_STATISTICS;

#define MR_DRV_MEGARAID_LD_STATISTICS_SIZE sizeof(MR_DRV_MEGARAID_LD_STATISTICS)

/****************************************************************************
*
*        EXTENDED LOGICAL DRIVE IO STATISTICS
*
****************************************************************************/

typedef struct _MR_DRV_MEGARAID_LD_STATISTICS_EXT
{
    U16             ldCount;        // number of LDs
    U16             reserved;       // pad to 8-byte boundary
    struct {
        U32   numberOfIoReads;
        U32   numberOfIoWrites;
        U32   numberOfBlocksRead;
        U32   numberOfBlocksWritten;
        U32   numberOfReadFailures;
        U32   numberOfWriteFailures;
        U32   reserved[6];
    } ldList[MAX_API_LOGICAL_DRIVES_EXT];

} MR_DRV_MEGARAID_LD_STATISTICS_EXT, *PMR_DRV_MEGARAID_LD_STATISTICS_EXT;

#define MR_DRV_MEGARAID_LD_STATISTICS_EXT_SIZE sizeof(MR_DRV_MEGARAID_LD_STATISTICS_EXT)

/****************************************************************************
*
*        FLAGS FOR SHOW/HIDE PDS
*
****************************************************************************/

typedef enum _MR_DRV_LD_PD_MODE
{
    MR_DRV_SHOW_ALL_LD_MODE = 0,  /*By default this mode is set, this will show all LDs and all PDs which are not disk*/
    MR_DRV_SHOW_ALL_PD_MODE = 1,  /*Show All PDs No LD*/
    MR_DRV_SHOW_ALL_MODE    = 3,  /*Show ALL LDs and ALL PDs but all disk pds  will not support read/write*/
    MR_DRV_SHOW_MAX_MODE    = 4
}MR_DRV_LD_PD_MODE;

/****************************************************************************
*
*  PCI DEVICENUMBER/FUNCTIONNUMBER/BUSNUMBER TO IDENTIFY THE CONTROLLERID
*  IN RESPONSE TO MR_WIN_GET_ADAPTER_ID.
*
****************************************************************************/
typedef struct _MR_DRV_ADAPTER_ID_INFO
{
    union
    {
        struct
        {
            U32 DeviceNumber:5;
            U32 FunctionNumber:3;
            U32 BusNumber:24;
        }bits;

        U32   asUlong;
    } u;

} MR_DRV_ADAPTER_ID_INFO, *PMR_DRV_ADAPTER_ID_INFO;



/****************************************************************************
*
*        DRIVER PARAMETERS
*
****************************************************************************/

typedef struct _MR_DRIVER_PARAMETERS
{
    U8   supportPollMode;     //supports poll mode of listening for events
    U8   supportDevChange;    //supports initiating bus rescan when devices arrive/depart
    U16  supportNvmeEncap:1;  //supports NVMe encapsulation
    U16  supportSGL64:1;      //supports 64 bit interface
    U16  reserved1:14;        //for future use
    U8   reserved[4];         //for future use
} MR_DRIVER_PARAMETERS;

typedef struct _MR_RAPTOR_STATS
{
    U32 FwWriteOutstanding;
    U64 RaptorLdIo;
    U64 RaptorPdIo;
    U64 RaptorUnalignedWriteIos;
    U64 RaptorAlignedWriteIos;
}MR_RAPTOR_STATS, *PMR_RAPTOR_STATS;

/****************************************************************************
*
*        DRIVER SYSTEM INFORMATION
*
****************************************************************************/
typedef struct _MR_DRV_SYSTEM_INFO
{
    U8    infoVersion;      /* Version of MR_DRV_SYSTEM_INFO structure. 0 for this structure. Other values reserved for future. */
    U8    systemIdLength;   /* Length of systemId */
    U16   reserved0;
    U8    systemId[64];     /* Unique ID of system or non-controller component.  If unique value not available, use
                             * high-resolution time, true random number, or other value that is unlikely to be duplicated
                             * by another system in the cluster. */

    U8    reserved[1980];
} MR_DRV_SYSTEM_INFO, *PMR_DRV_SYSTEM_INFO;

#define MR_DRV_SYSTEM_INFO_SIZE sizeof(MR_DRV_SYSTEM_INFO)

/*******************************************************************************
*
*        RTTrace INFORMATION
*
********************************************************************************/
typedef struct _MR_RTTRACE_HEADER
{
    U8       DriverName[32];         // Name of device driver from which trace was uploaded
    U8       PortName[32];           // Name of port for which trace was uploaded
    U8       RttVersion[4];          // Version supported by driver for RTTRACE IOCTLs
    U32      TraceBufferSize;        // Trace buffer size in bytes (used or unused)
    U32      NumberEvents;           // Number events in trace buffer at upload time
    U32      TracelogMask;           // Mask of enabled trace event types
    U64      DateTime[2];            // Date and Time at which Trace was uploaded
    U8       TraceEnabled;           // TRUE when trace enabled
    U8       reserved[31];           // reserved for future use
} MR_RTTRACE_HEADER, *PMR_RTTRACE_HEADER;

#define RTTRACE_HEADER_SIZE   sizeof(MR_RTTRACE_HEADER)

typedef struct _MR_RTT_DATA
{
    U32     bufferlength;                 // Set by application in submission path. Set by driver in IOCTL return path.
                                          // Driver will set actual bytes written to this field.
    U32     flags;                        // Used for driver/application handshake.
    U8      data[4];                      // Variable-size returned data buffer.
} MR_RTT_DATA, *PMR_RTT_DATA;

#define MPI_RTT_FLAG_NULL           0x00000000      // All flags cleared (FALSE)
#define MPI_RTT_FLAG_READ_FIRST     0x00000001     // Sent by Application to read from start of trace
#define MPI_RTT_FLAG_LAST_EVENT     0x00000002     // Sent by driver to notify application last record

// RTTRACE Parameter Mask Flags
//  Used to indicate which parameters are being sent between application and driver
#define MPI_RTT_PARAM_TRACE_ENABLED         0x00000001
#define MPI_RTT_PARAM_TRACE_LOG_MASK        0x00000002
#define MPI_RTT_PARAM_NUMBER_EVENTS         0x00000004
#define MPI_RTT_PARAM_TRACE_BUFFER_SIZE     0x00000008



typedef struct _MR_RTT_PARAMS
{
	U32                     TraceBufferSize;    // READ ONLY - Trace buffer size in bytes (used or unused)
	U32                     NumberEvents;       // READ ONLY - Number events currently in trace buffer
	U32                     TraceLogMask;       // READ/WRITE - Mask of enabled trace event types
	U32                     ParamMask;          // Array of flags to indicate which params are being transferred
	U8                      TraceEnabled;       // READ/WRITE - TRUE when trace enabled
	U8                      reserved1;
	U16                     reserved2;
} MR_RTT_PARAMS, *PMR_RTT_PARAMS;



#ifdef  __cplusplus
}
#endif

#endif //_MR_DRV_H
