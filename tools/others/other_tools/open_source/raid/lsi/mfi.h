/*
 * MegaRAID MFI Interface Header File
 *
 * © 2004-2018 Broadcom.  All Rights Reserved.  Broadcom Confidential
 * The term “Broadcom” refers to Broadcom Inc. and/or its subsidiaries.
 *
 * This header file defines the typedefs, unions, and structures used by the
 * MegaRAID Firmware Interface (MFI).
 *
 * Note:  There are two versions of each MFI frame declared.  One has a common
 *        header (MFI_CMD_*), and the other has integrated header fields (MFI_FRAME_*).
 *        This is done to maximize compatibility between previously developed modules.
 */
#ifndef INCLUDE_MFI
#define INCLUDE_MFI

/*
 * define macros for MFI
 */
#define MFI_FRAME_SIZE      64          /* frame size if 64 bytes */

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

#define mfiControlReg           mfiControlReg
#define mfiMsgFlags             mfiMsgFlags
#define idInStatusOut           idInStatusOut
#define mfiContextRegs          mfiContextRegs
#define mfiAbortRegs            mfiAbortRegs
#define mfiOOBCmdTL             mfiOOBCmdTL
#define mfiCapabilities         mfiCapabilities

#else                           /* unnamed struct are acceptable */

#define mfiControlReg
#define mfiMsgFlags
#define idInStatusOut
#define mfiContextRegs
#define mfiAbortRegs
#define mfiOOBCmdTL
#define mfiCapabilities

#endif  /* NO_UNNAMED_STRUCT */

/*
 * define the firmware states
 */
typedef enum _MFI_STATE {
    MFI_STATE_UNDEFINED         = 0,    /* firmware state not initialize yet */
    MFI_STATE_BB_INIT           = 1,    /* Boot Block is initializing */
    MFI_STATE_RESERVED_2        = 2,
    MFI_STATE_RESERVED_3        = 3,
    MFI_STATE_FW_INIT           = 4,    /* firmware is initializing */
    MFI_STATE_RESERVED_5        = 5,
    MFI_STATE_WAIT_HANDSHAKE    = 6,    /* firmware waiting for handshake to continue */
    MFI_STATE_FW_INIT_2         = 7,    /* firmware is initializing (post handshake) */
    MFI_STATE_DEVICE_SCAN       = 8,    /* firmware is scanning and initializing attached devices */
    MFI_STATE_BOOT_MSG_PENDING  = 9,    /* firmware has boot messages pending for BIOS to process */
    MFI_STATE_FLUSH_CACHE       = 10,   /* firmware is flushing dirty data to cache */
    MFI_STATE_READY             = 11,   /* firmware ready to receive cmds - response queue not valid */
    MFI_STATE_OPERATIONAL       = 12,   /* firmware operational - response queue valid */
    MFI_STATE_RESERVED_13       = 13,
    MFI_STATE_RESERVED_14       = 14,
    MFI_STATE_FAULT             = 15,   /* fault condition occurred */
} MFI_STATE;

/*
 * define the FW fault codes
 */
typedef enum _MFI_FAULT {
    MFI_FAULT_BREAK             = 1,    /* unexpected FW trap */
    MFI_FAULT_ECC               = 2,    /* uncorrectable ECC error */
    MFI_FAULT_MEMORY            = 3,    /* insufficient memory */
    MFI_FAULT_DIRTY_CACHE       = 4,    /* dirty cache can't be flushed - only occurs when hot-plug and user can't be notified */
    MFI_FAULT_INTERNAL_USE      = 0xfe, /* Used for routine coordinated resets.  No user notification necessary */
    MFI_FAULT_INIT              = 0xff, /* FW fault during initialization */
} MFI_FAULT;

/*
 * defines for FW fault sub codes
 */
#define MFI_FAULT_INIT_UNSUPPORTED_MEMORY   (3)     /* Controller memory is unsupported */
#define MFI_FAULT_INIT_BAD_MEMORY           (4)     /* Memory is bad, or not connected */

/*
 * define the waitData for MFI_STATE_WAIT_HANDSHAKE
 */
typedef enum _MFI_WAIT_DATA {
    MFI_WAIT_DATA_HOST_MEM_REQUIRED  = 0x0000001,    /* Provide FW with host memory */
} MFI_WAIT_DATA;

/*
 * define the firmware state field format - placed in the OMR0 register
 */
typedef union _MFI_REG_STATE {
    /*
     * single U32 register definition
     */
    U32     reg;

    /*
     * common register definition
     */
    struct {
#ifndef MFI_BIG_ENDIAN
        U32     stateSpecificData  : 28;
        U32     state              : 4;
#else
        U32     state              : 4;
        U32     stateSpecificData  : 28;
#endif  // MFI_BIG_ENDIAN
    } common;

    /*
     * state bits for Boot Block
     */
    struct {
#ifndef MFI_BIG_ENDIAN
        U32     bbData             : 28;
        U32     state              : 4;
#else
        U32     state              : 4;
        U32     bbData             : 28;
#endif  // MFI_BIG_ENDIAN
    } bb;

    /*
     * state bits for INIT state
     */
    struct {
#ifndef MFI_BIG_ENDIAN
        U32     initData    : 28;
        U32     state       : 4;
#else
        U32     state       : 4;
        U32     initData    : 28;
#endif  // MFI_BIG_ENDIAN
    } init;

    /*
     * define bits for handshake state
     */
    struct {
#ifndef MFI_BIG_ENDIAN
        U32     waitData    : 28;
        U32     state       : 4;
#else
        U32     state       : 4;
        U32     waitData    : 28;
#endif  // MFI_BIG_ENDIAN
    } wait;

    /*
     * define bits for DEVICE SCAN state
     */
    struct {
#ifndef MFI_BIG_ENDIAN
        U32     devID       : 8;    /* current device being scanned */
        U32     percent     : 7;    /* percent of scan completion */
        U32     devIDMsb    : 5;    /* MSB of devID field */
        U32     reserved    : 8;    /* reserved for future use */
        U32     state       : 4;
#else
        U32     state       : 4;
        U32     reserved    : 8;    /* reserved for future use */
        U32     devIDMsb    : 5;    /* MSB of devID field */
        U32     percent     : 7;    /* percent of scan completion */
        U32     devID       : 8;    /* current device being scanned */
#endif  // MFI_BIG_ENDIAN
    } scan;

    /*
     * define bits for boot message pending state
     */
    struct {
#ifndef MFI_BIG_ENDIAN
        U32     needsEKM        : 1;   /* EKM is required */
        U32     doubleDip       : 1;
        U32     needsEKMOverOOB : 1;   /* EKM is required over OOB */
        U32     reserved        : 25;
        U32     state           : 4;
#else
        U32     state           : 4;
        U32     reserved        : 25;
        U32     needsEKMOverOOB : 1;   /* EKM is required over OOB */
        U32     doubleDip       : 1;
        U32     needsEKM        : 1;   /* EKM is required */
#endif  // MFI_BIG_ENDIAN
    } bootMsgPending;

    /*
     * define bits for READY state
     */
    struct {
#ifndef MFI_BIG_ENDIAN
        U32     maxCmds     : 16;   /* maximum number of concurrent outstanding commands */
        U32     maxSGEs     : 8;    /* maximum number of SGEs in SGL */
        U32     mode64      : 1;    /* 1=mode is 64-bit (MFAs and Contexts), 0=32-bit mode */
        U32     msiEnable   : 2;    /* 0=No MSI/MSI-X (old FW), 1=MSI support, 2=MSI-X support, 3=Invalid */
        U32     hostMemRequired  : 1; /* 1=driver needs to provide host mem to controller before INIT */
        U32     state       : 4;
#else
        U32     state       : 4;
        U32     hostMemRequired  : 1; /* 1=driver needs to provide host mem to controller before INIT */
        U32     msiEnable   : 2;    /* 0=No MSI/MSI-X (old FW), 1=MSI support, 2=MSI-X support, 3=Invalid */
        U32     mode64      : 1;    /* 1=mode is 64-bit (MFAs and Contexts), 0=32-bit mode */
        U32     maxSGEs     : 8;    /* maximum number of SGEs in SGL */
        U32     maxCmds     : 16;   /* maximum number of concurrent outstanding commands */
#endif  // MFI_BIG_ENDIAN
    } ready;

    /*
     * define bits for OPERATIONAL state
     */
    struct {
#ifndef MFI_BIG_ENDIAN
        U32     maxCmds     : 16;   /* maximum number of concurrent outstanding commands */
        U32     maxSGEs     : 8;    /* maximum number of SGEs in SGL */
        U32     mode64      : 1;    /* 1=mode is 64-bit (MFAs and contexts), 0=32-bit mode */
                                    /* Note: context is DEPRECATED */
        U32     useMfiContextRegsAndMfiAbortRegs : 1;
                                    /* For MSI/MIS-X: 0=use msgId (DEPRECATED), 1=use msgVector */
                                    /* For context: 0=use MFI_ADDRESS(context) (DEPRECATED) */
                                    /* 1=use mfiContextRegs and mfiAbortRegs, i.e cmdContext etc. */
        U32     supportIEEE : 1;    /* IEEE SGL is supported*/
        U32     reserved    : 1;    /* reserved bits (must be zero) */
        U32     state       : 4;
#else
        U32     state       : 4;
        U32     reserved    : 1;    /* reserved bits (must be zero) */
        U32     supportIEEE : 1;    /* IEEE SGL is supported*/
        U32     useMfiContextRegsAndMfiAbortRegs : 1;
                                    /* For MSI/MIS-X: 0=use msgId (DEPRECATED), 1=use msgVector */
                                    /* For context: 0=use MFI_ADDRESS(context) (DEPRECATED) */
                                    /* 1=use mfiContextRegs and mfiAbortRegs, i.e cmdContext etc. */
        U32     mode64      : 1;    /* 1=mode is 64-bit (MFAs and Contexts), 0=32-bit mode */
                                    /* Note: context is DEPRECATED */
        U32     maxSGEs     : 8;    /* maximum number of SGEs in SGL */
        U32     maxCmds     : 16;   /* maximum number of concurrent outstanding commands */
#endif  // MFI_BIG_ENDIAN
    } operational;

    /*
     * define bits for FAULT state
     */
    struct {
#ifndef MFI_BIG_ENDIAN
        U32     resetRequired : 1;  /* controller requires reset; driver - set MFI_CONTROL_REG.resetAdapter when ready */
        U32     resetAdapter  : 1;  /* controller requires reset; driver - initiate chip reset per the controller spec */
        U32     crashDumpDone : 1;  /* FW/driver indicates crash dump operation was either completed or was terminated */
        U32     crashDumpDMADone:1; /*
                                     * Set by FW indicating DMA of crash buffer chunk was completed
                                     * Reset by driver indicating DMA buffer was received
                                     */
        U32     reserved    : 4;
        U32     subCode     : 8;    /* minor fault code */
        U32     code        : 12;   /* major fault code */
        U32     state       : 4;
#else
        U32     state       : 4;
        U32     code        : 12;   /* major fault code */
        U32     subCode     : 8;    /* minor fault code */
        U32     reserved    : 4;
        U32     crashDumpDMADone:1; /*
                                     * Set by FW indicating DMA of crash buffer chunk was completed
                                     * Reset by driver indicating DMA buffer was received
                                     */
        U32     crashDumpDone : 1;  /* FW/driver indicates crash dump was either completed or was terminated */
        U32     resetAdapter  : 1;  /* controller requires reset; driver - initiate chip reset per the controller spec */
        U32     resetRequired : 1;  /* controller requires reset; driver - set MFI_CONTROL_REG.resetAdapter when ready */
#endif  // MFI_BIG_ENDIAN
    } fault;
} MFI_REG_STATE;

typedef union _MFI_REG_STATE_EXT1 {
    /*
     * single U32 register definition
     */
    U32     reg;

    /*
     * common register definition
     */
    struct {
#ifndef MFI_BIG_ENDIAN
        U32     stateSpecificData  : 28;
        U32     state              : 4;
#else
        U32     state              : 4;
        U32     stateSpecificData  : 28;
#endif  // MFI_BIG_ENDIAN
    } common;

    /*
     * define bits for READY state
     */
    struct {
#ifndef MFI_BIG_ENDIAN
        U32     maxReplyQueues   : 5;   /* maximum number of reply queues - 1(and associated MSIX vectors) */
        U32     maxChainSize     : 5;   /* maximum size of chain buffer in units of 128 bytes */
        U32     maxChainBuffers  : 4;   /* maximum number of chain buffers */
        U32     maxReplyQueuesExt: 8;   /* extended reply queues */
        U32     maxChainSizeUnits: 1;   /* 1=maximum size of chain buffer in units of 512 bytes
                                           0=maximum size of chain buffer in units of 128 bytes*/
        U32     RDPQArrayMode    : 1;   /* RDPQ array mode. Driver shall indicate usage via MPI2_IOCINIT_MSGFLAG_RDPQ_ARRAY_MODE  */
        U32     canHandleSyncCache : 1; /* 1=Driver should forward sync cache command to FW */
        U32     canHandle64BitMode : 1; /* 1=FW is capable of handling 64bit DMA mode*/
        U32     intCoalescing    : 1;   /* 1=High IOPs queue support enabled
                                           FW only sets this if in 128 vector mode */
        U32     reserved1        : 1;
        U32     state            : 4;
#else
        U32     state            : 4;
        U32     reserved1        : 1;
        U32     intCoalescing    : 1;   /* 1=High IOPs queue support enabled
                                           FW only sets this if in 128 vector mode */
        U32     canHandle64BitMode : 1; /* 1=FW is capable of handling 64bit DMA mode*/
        U32     canHandleSyncCache : 1; /* 1=Driver should forward sync cache command to FW */
        U32     RDPQArrayMode    : 1;   /* RDPQ array mode */
        U32     maxChainSizeUnits: 1;   /* 1=maximum size of chain buffer in units of 512 bytes
                                           0=maximum size of chain buffer in units of 128 bytes*/
        U32     maxReplyQueuesExt: 8;   /* extended reply queues */
        U32     maxChainBuffers  : 4;   /* maximum number of chain buffers */
        U32     maxChainSize     : 5;   /* maximum size of chain buffer in units of 128 bytes */
        U32     maxReplyQueues   : 5;   /* maximum number of reply queues - 1(and associated MSIX vectors) */
#endif  // MFI_BIG_ENDIAN
    } ready;

    /*
     * define bits for OPERATIONAL state
     */
    struct {
#ifndef MFI_BIG_ENDIAN
        U32     maxReplyQueues   : 5;   /* maximum number of reply queues - 1(and associated MSIX vectors) */
        U32     maxChainSize     : 5;   /* maximum size of chain buffer in units of 128 bytes */
        U32     maxChainBuffers  : 4;   /* maximum number of chain buffers */
        U32     ldSyncRequired   : 1;
        U32     maxReplyQueuesExt: 8;   /* extended reply queues */
        U32     canHandleSyncCache : 1; /* 1=Driver should forward sync cache command to FW */
        U32     reserved1        : 4;
        U32     state            : 4;
#else
        U32     state            : 4;
        U32     reserved1        : 4;
        U32     canHandleSyncCache : 1; /* 1=Driver should forward sync cache command to FW */
        U32     maxReplyQueuesExt: 8;   /* extended reply queues */
        U32     ldSyncRequired   : 1;
        U32     maxChainBuffers  : 4;   /* maximum number of chain buffers */
        U32     maxChainSize     : 5;   /* maximum size of chain buffer in units of 128 bytes */
        U32     maxReplyQueues   : 5;   /* maximum number of reply queues - 1(and associated MSIX vectors) */
#endif  // MFI_BIG_ENDIAN
    } operational;

} MFI_REG_STATE_EXT1;

typedef union _MFI_REG_STATE_EXT2 {
    /*
     * single U32 register definition
     */
    U32     reg;

    /*
     * common register definition
     */
    struct {
#ifndef MFI_BIG_ENDIAN
        U32     maxAdapterQdepth   : 16; /* This will hold the max Ctrl qdepth value (sysPD + VD qdepth) */
        U32     maxRaidMapSize     : 9;  /* In 64K units, Supports upto 32M size */
        U32     reserved1          : 7;
#else
        U32     reserved1          : 7;
        U32     maxRaidMapSize     : 9;  /* In 64K units, Supports upto 32M size */
        U32     maxAdapterQdepth   : 16; /* This will hold the max Ctrl qdepth value (sysPD + VD qdepth) */
#endif  // MFI_BIG_ENDIAN
    } common;
} MFI_REG_STATE_EXT2;

typedef union _MFI_REG_STATE_EXT3 {
    /*
     * single U32 register definition
     */
    U32     reg;

    /*
     * common register definition
     */
    struct {
#ifndef MFI_BIG_ENDIAN
        U32     reserved1          : 32;
#else
        U32     reserved1          : 32;
#endif  // MFI_BIG_ENDIAN
    } common;

    /*
     * define bits for OPERATIONAL state
     */
    struct {
#ifndef MFI_BIG_ENDIAN
        U32     CurrentFwNvmePageSize   : 8;    /* Firmware provide current NVME Device page size in terms of power of two */
        U32     reserved1           : 24;
#else
        U32     reserved1           : 24;
        U32     CurrentFwNvmePageSize   : 8;    /* Firmware provide current NVME Device page size in terms of power of two */
#endif  // MFI_BIG_ENDIAN
    } operational;

} MFI_REG_STATE_EXT3;


/*
 * define the firmware control bits - placed in the IDR register
 */
typedef union _MFI_CONTROL_REG {
    struct {
#ifndef MFI_BIG_ENDIAN
        U32     abortAllCmds        : 1;    /* abort all outstanding commands */
        U32     transitionToReady   : 1;    /* transition from OPERATIONAL to READY state */
        U32     discardLowMFA       : 1;    /* discard (possible) low MFA posted in 64-bit mode */
        U32     releaseFirmware     : 1;    /* set to release FW to continue running (i.e. BIOS handshake) */
        U32     hotPlug             : 1;    /* set by drivers when Hot Plug startup is detected - must be set BEFORE or with releaseFirmware bit */
        U32     killAdapter         : 1;    /* set by drivers to kill controller - FW drops to mon task.  use 'rp' to dump pending commands */
        U32     resetAdapter        : 1;    /* set by drivers when ready for controller reset (in response to MFI_REG_STATE.fault.resestRequired) */
        U32     triggerCrashDump    : 1;    /* set by drivers to inform FW to initiate crash dump collection process */
        U32     triggerSnapDump     : 1;    /* set by drivers to inform FW to initiate snap dump collection process */
        U32     reserved            : 23;   /* reserved for future use */
#else
        U32     reserved            : 23;   /* reserved for future use */
        U32     triggerSnapDump     : 1;    /* set by drivers to inform FW to initiate snap dump collection process */
        U32     triggerCrashDump    : 1;    /* set by drivers to inform FW to initiate crash dump collection process */
        U32     resetAdapter        : 1;    /* set by drivers when ready for controller reset (in response to MFI_REG_STATE.fault.resestRequired) */
        U32     killAdapter         : 1;    /* set by drivers to kill controller - FW drops to mon task.  use 'rp' to dump pending commands */
        U32     hotPlug             : 1;    /* set by drivers when Hot Plug startup is detected - must be set BEFORE or with releaseFirmware bit */
        U32     releaseFirmware     : 1;    /* set to release FW to continue running (i.e. BIOS handshake) */
        U32     discardLowMFA       : 1;    /* discard (possible) low MFA posted in 64-bit mode */
        U32     transitionToReady   : 1;    /* transition from OPERATIONAL to READY state */
        U32     abortAllCmds        : 1;    /* abort all outstanding commands */
#endif  // MFI_BIG_ENDIAN
    } mfiControlReg;                        /* unnamed struct:  search for NO_UNNAMED_STRUCT for usage */
    U32     reg;
} MFI_CONTROL_REG;

/*
 * define the MFI transfer direction flags
 */
typedef enum _MFI_DIR {
    MFI_DIR_NONE        = 0,            /* no data transfer */
    MFI_DIR_WRITE       = 1,            /* transfer is from host */
    MFI_DIR_READ        = 2,            /* transfer is to host  */
    MFI_DIR_BOTH        = 3,            /* transfer is both read and write, or unknown */
} MFI_DIR;

/*
 * define the MFI command opcodes
 */
typedef enum _MFI_CMD_OP {
    MFI_CMD_OP_INIT        = 0,         /* initialize MFI completion queues */
    MFI_CMD_OP_LD_READ     = 1,         /* READ from a logical drive */
    MFI_CMD_OP_LD_WRITE    = 2,         /* WRITE to a logical drive */
    MFI_CMD_OP_LD_SCSI     = 3,         /* issue a SCSI CDB to a logical drive */
    MFI_CMD_OP_PD_SCSI     = 4,         /* issue a SCSI CDB to a physical drive */
    MFI_CMD_OP_DCMD        = 5,         /* issue a MegaRAID DCMD */
    MFI_CMD_OP_ABORT       = 6,         /* ABORT an outstanding command */
    MFI_CMD_OP_SMP         = 7,         /* SMP request */
    MFI_CMD_OP_STP         = 8,         /* STP request */
    MFI_CMD_OP_NVME        = 9,         /* NVMe request */
    MFI_CMD_OP_COUNT                    /* count of valid MFI commands */
} MFI_CMD_OP;

/*
 * define the MFI message frame FLAGS field
 */
typedef union _MFI_FLAGS {
    struct {
#ifndef MFI_BIG_ENDIAN
        U16     doNotPostInReplyQueue   : 1;        /* 1 = do not post in reply queue (i.e. don't generate an interrupt) */
        U16     SGLIs64Bit              : 1;        /* 1 = SGL contains 64-bit entries */
        U16     sensePtrIs64Bit         : 1;        /* 1 = sense data buffer is 64-bit pointer */
        U16     dir                     : 2;        /* transfer direction (type MFI_DIR) */
        U16     SGLIsIEEE               : 1;        /* 1 = IEEE SGL format, 0 = MFI SGL format */
        U16     reserved                : 10;
#else
        U16     reserved                : 10;
        U16     SGLIsIEEE               : 1;        /* 1 = IEEE SGL format, 0 = MFI SGL format */
        U16     dir                     : 2;        /* transfer direction (type MFI_DIR) */
        U16     sensePtrIs64Bit         : 1;        /* 1 = sense data buffer is 64-bit pointer */
        U16     SGLIs64Bit              : 1;        /* 1 = SGL contains 64-bit entries */
        U16     doNotPostInReplyQueue   : 1;        /* 1 = do not post in reply queue (i.e. don't generate an interrupt) */
#endif  // MFI_BIG_ENDIAN
    } mfiMsgFlags;                                  /* unnamed struct:  search for NO_UNNAMED_STRUCT for usage */
    U16     reg;
} MFI_FLAGS;

/*
 * define the capabilitiies field
 */
typedef union _MFI_CAPABILITIES {
    struct {
#ifndef MFI_BIG_ENDIAN
        U32     supportFPRemoteLun      : 1;        /* 1 = driver can issue FP IO requests to remote LUNs */
        U32     supportAdditionalMSIX   : 1;        /* 1 = driver can support >8 MSIX queues (Invader C0 and beyond) */
        U32     supportFastPathWB       : 1;        /* 1 = driver can support Raptor FastPath WriteBack (Latency Sensitive I/O)*/
        U32     supportMaxExtLDs        : 1;        /* 1 = driver can support MAX_API_LOGICAL_DRIVES_EXT lds */
        U32     supportNDrvR1LoadBalance: 1;        /* 1 = driver can load balance R1 reads for VDs with more than 2 drives */
        U32     supportCoreAffinity     : 1;        /* 1 = driver can issue IO to a core based on MR_IO_AFFINITY  */
        U32     SecurityProtocolCmdsFW  : 1;        /* 1 = For JBODs, driver always sends security protocol commands to FW */
        U32     supportExtQueueDepth    : 1;        /* 1 = driver supports extended queue depth for the adapter */
        U32     supportExtIOSize        : 1;        /* 1 = driver supports extended IO Size for the adapter */
        U32     supportVFIDinIOFrame    : 1;        /* 1 = driver supports insertion of VF ID in the IO frame */
        U32     supportFPRLBypass       : 1;        /* 1 = driver supports region lock bypass for FP I/O and routes
                                                           non FP I/O directly to FW via region lock bypass queue */
        U32     supportQDThrottling     : 1;        /* 1 = Driver supports queue depth throttling if dual QD is enabled */
        U32     supportPdMapTargetId    : 1;        /* 1 = Driver supports using pdTargetId field in MR_PD_CFG_SEQ */
        U32     support64BitMode        : 1;        /* 1 = Driver supports and is currently operating in 64bit DMA mode */
        U32     supportNVMePassthru     : 1;        /* 1 = Driver supports NVMe passthru commands */
        U32     reserved                : 17;
#else
        U32     reserved                : 17;
        U32     supportNVMePassthru     : 1;        /* 1 = Driver supports NVMe passthru commands */
        U32     support64BitMode        : 1;        /* 1 = Driver supports and is currently operating in 64bit DMA mode */
        U32     supportPdMapTargetId    : 1;        /* 1 = Driver supports using pdTargetId field in MR_PD_CFG_SEQ */
        U32     supportQDThrottling     : 1;        /* 1 = Driver supports queue depth throttling if dual QD is enabled */
        U32     supportFPRLBypass       : 1;        /* 1 = driver supports region lock bypass for FP I/O and routes
                                                           non FP I/O directly to FW via region lock bypass queue */
        U32     supportVFIDinIOFrame    : 1;        /* 1 = driver supports insertion of VF ID in the IO frame */
        U32     supportExtIOSize        : 1;        /* 1 = driver supports extended IO Size for the adapter */
        U32     supportExtQueueDepth    : 1;        /* 1 = driver supports extended queue depth for the adapter */
        U32     SecurityProtocolCmdsFW  : 1;        /* 1 = For JBODs, driver always sends security protocol commands to FW */
        U32     supportCoreAffinity     : 1;        /* 1 = driver can issue IO to a core based on MR_IO_AFFINITY  */
        U32     supportNDrvR1LoadBalance: 1;        /* 1 = driver can load balance R1 reads for VDs with more than 2 drives */
        U32     supportMaxExtLDs        : 1;        /* 1 = driver can support MAX_API_LOGICAL_DRIVES_EXT lds */
        U32     supportFastPathWB       : 1;        /* 1 = driver can support Raptor FastPath WriteBack (Latency Sensitive I/O)*/
        U32     supportAdditionalMSIX   : 1;        /* 1 = driver can support >8 MSIX queues (Invader C0 and beyond) */
        U32     supportFPRemoteLun      : 1;        /* 1 = driver can issue FP IO requests to remote LUNs */
#endif  // MFI_BIG_ENDIAN
    } mfiCapabilities;                              /* unnamed struct:  search for NO_UNNAMED_STRUCT for usage */
    U32     reg;
} MFI_CAPABILITIES;

/*
 * define MFI Address Context union
 */
#ifdef MFI_ADDRESS_IS_U64
    typedef U64     MFI_ADDRESS;
#else
    typedef union _MFI_ADDRESS {
        struct {
            U32     addressLow;
            U32     addressHigh;
        } u;
        U64     address;
    } MFI_ADDRESS, *PMFI_ADDRESS;
#endif

/*
 * define the Scatter-Gather-Entry formats
 */
typedef struct  _MFI_SGE_32 {
    U32     addr;
    U32     count;
} MFI_SGE_32, *PMFI_SGE_32;

typedef struct  _MFI_SGE_64 {
    U32     addrLow;
    U32     addrHigh;
    U32     count;
} MFI_SGE_64, *PMFI_SGE_64;

typedef union _MFI_SGE {
    MFI_SGE_32  s32;
    MFI_SGE_64  s64;
} MFI_SGE, *PMFI_SGE;

typedef struct _IEEE_SGE_32 {
    U32         address;
    U32         length;
    union {
        struct {
            U32     reserved    :   31;
            U32     extension   :   1;  // Address denotes next SGL if set
        } flags;
        U32         w;
    };
} IEEE_SGE_32, *PIEEE_SGE_32;

typedef struct _IEEE_SGE_64 {
    union {
        struct {
            U32     addrLow;
            U32     addrHigh;
        } u;
        U64 address;
    };
    U32         length;
    union {
        struct {
            U32     reserved    :   31;
            U32     extension   :   1;  // Address denotes next SGL if set
        } flags;
        U32         w;
    };
} IEEE_SGE_64, *PIEEE_SGE_64;

/*
 * define the MFI frame header
 */
typedef struct _MFI_HDR {
    U8      cmd;                        /* MFI command (type MFI_CMD_OP) */
    U8      requestSenseLength;         /* input (and returned) length of request sense buffer */
    U8      cmdStatus;                  /* returned MFI command status (MFI_STATUS) */
    union {
        U8      targetIDMsb;                /* Input: MSB of target ID (overwritten by extStatus on command completion) */
        U8      extStatus;                  /* Output: returned SCSI status for CDB commands (or MFI_SMP_STAT status for STP/SMP) */
        U8      msgId;                      /* DEPRECATED Input: MSI/MSI-X Message Id sent by host */
    } idInStatusOut;
    U8      targetID;                   /* target ID (LD) */
    U8      LUN;                        /* target ID LUN */
    U8      CDBLength;                  /* SCSI CDB length */
    U8      numSGE;                     /* number of SG Elements */

    union {
        MFI_ADDRESS context;           /* DEPRECATED driver context field low word */
        struct {
            U32     cmdContext;        /* driver context field */
            U8      requestorId;       /* requestor Id */
            U8      msgVector;         /* MSI/MSI-X message Vector sent by host; replaces msgId */
            U8      pad[2];            /* reserved for future use */
        } mfiContextRegs;              /* unnamed struct:  search for NO_UNNAMED_STRUCT for usage */
    };

    MFI_FLAGS   flags;                  /* command flags */
    U16     timeout;                    /* command timeout (in seconds) */
    U32     length;                     /* length of this command's data */
} MFI_HDR, *PMFI_HDR;

/*
 * define the INIT command structure and queue data structure
 */
typedef struct _MFI_CMD_INIT {
    MFI_HDR     hdr;                    /* standard command header */
    MFI_ADDRESS queueInfoNew;           /* input queue info phys addr */
    MFI_ADDRESS queueInfoOld;           /* output queue info phys addr */
    MFI_ADDRESS driverVersion;          /* driver version phys addr (MR_DRV_DRIVER_VERSION) */
    MFI_ADDRESS systemInformation;      /* system information sent to controller (MR_DRV_SYSTEM_INFO) */
    U32         reserved[2];            /* reserved for expansion */
} MFI_CMD_INIT, *PMFI_CMD_INIT;

typedef struct _MFI_FRAME_INIT {
    U8          cmd;                    /* MFI command (type MFI_CMD_OP) */
    U8          reserved;               /* set to 0 */
    U8          cmdStatus;              /* returned MFI command status (MFI_STATUS) */
    union {
        U8      msgId;                  /* DEPRECATED Input: MSI/MSI-X Message Id sent by host */
        U8      reserved1;              /* For future use */
    };
    MFI_CAPABILITIES driverOperations;  /* capabilities/operations supported by driver */

    union {
        MFI_ADDRESS context;           /* DEPRECATED driver context field low word */
        struct {
            U32     cmdContext;        /* driver context field */
            U8      requestorId;       /* requestor Id */
            U8      msgVector;         /* MSI/MSI-X message Vector sent by host; replaces msgId */
            U8      pad[2];            /* reserved for future use */
        } mfiContextRegs;              /* unnamed struct:  search for NO_UNNAMED_STRUCT for usage */
    };

    MFI_FLAGS   flags;                  /* command flags */
    U16         replyQueueMask;         /* Provide reply queue group mask for interrupt coalescing.
                                           Each bit represents a group of vectors in each function. */
    U32         length;                 /* length of this command's data */

    MFI_ADDRESS queueInfoNew;           /* input queue info phys addr */
    MFI_ADDRESS queueInfoOld;           /* output queue info phys addr */
    MFI_ADDRESS driverVersion;          /* driver version phys addr (MR_DRV_DRIVER_VERSION) */
    MFI_ADDRESS systemInformation;      /* system information sent to controller (MR_DRV_SYSTEM_INFO) */
    U32         reserved4[2];           /* reserved for expansion */
} MFI_FRAME_INIT, *PMFI_FRAME_INIT;

typedef struct _MFI_CMD_INIT_QUEUE {
    struct  {
#ifndef MFI_BIG_ENDIAN
        U32     mfaIs64Bits    : 1;     /* 1 = 64-bit MFAs must be issued (i.e. wo 32-bit writes) */
        U32     contextIs64Bits: 1;     /* 1 = 64-bit contexts are in use (reply queue entries are 64-bit) */
        U32     reserved       :30;     /* reserved for future use */
#else
        U32     reserved       :30;     /* reserved for future use */
        U32     contextIs64Bits: 1;     /* 1 = 64-bit contexts are in use (reply queue entries are 64-bit) */
        U32     mfaIs64Bits    : 1;     /* 1 = 64-bit MFAs must be issued (i.e. wo 32-bit writes) */
#endif  // MFI_BIG_ENDIAN
    } flags;
    U32         responseQueueEntries;   /* number of entries in response queue (32 or 64-bit) */
    MFI_ADDRESS responseQueueStart;     /* response queue phys addr */
    MFI_ADDRESS producerIndexPtr;       /* producer (firmware) queue pointer phys addr */
    MFI_ADDRESS consumerIndexPtr;       /* consumer (driver) queue pointer phys addr */
    U32         reserved[8];            /* reserved for expansion */
} MFI_CMD_INIT_QUEUE, *PMFI_CMD_INIT_QUEUE;

/*
 * define the Logical Drive READ/WRITE command structure
 */
typedef struct _MFI_CMD_LD_READ_WRITE {
    MFI_HDR     hdr;
    MFI_ADDRESS requestSense;           /* request sense buffer pointer */
    MFI_ADDRESS startBlock;             /* starting block address */
    MFI_SGE     sge[2];                 /* two entries is 6 words */
} MFI_CMD_LD_READ_WRITE, *PMFI_CMD_LD_READ_WRITE;

typedef struct _MFI_FRAME_LD_READ_WRITE {
    U8          cmd;                    /* MFI command (type MFI_CMD_OP) */
    U8          requestSenseLength;     /* input (and returned) length of request sense buffer */
    U8          cmdStatus;              /* returned MFI command status (MFI_STATUS) */
    union {
        U8          targetIDMsb;            /* Input: MSB of target ID (overwritten by extStatus on command completion) */
        U8          scsiStatus;             /* Output: returned SCSI status for CDB commands */
        U8          msgId;                  /* DEPRECATED Input: MSI/MSI-X Message Id sent by host */
    } idInStatusOut;
    U8          targetID;               /* target ID (LD) */
    U8          accessByte;             /* access byte of CDB[1] */
    U8          reserved;               /* set to 0 */
    U8          numSGE;                 /* number of SG Elements */

    union {
        MFI_ADDRESS context;           /* DEPRECATED driver context field low word */
        struct {
            U32     cmdContext;        /* driver context field */
            U8      requestorId;       /* requestor Id */
            U8      msgVector;         /* MSI/MSI-X message Vector sent by host; replaces msgId */
            U8      pad[2];            /* reserved for future use */
        } mfiContextRegs;              /* unnamed struct:  search for NO_UNNAMED_STRUCT for usage */
    };

    MFI_FLAGS   flags;                  /* command flags */
    U16         timeout;                /* command timeout (in seconds) */
    U32         numberOfBlocks;         /* number of Blcoks to transfer data */

    MFI_ADDRESS requestSense;           /* request sense buffer pointer */
    MFI_ADDRESS startBlock;             /* starting block address */
    MFI_SGE     sge[2];                 /* two entries is 6 words */
} MFI_FRAME_LD_READ_WRITE, *PMFI_FRAME_LD_READ_WRITE;

/*
 * declare the LD/PD SCSI IO frame
 */
typedef struct _MFI_CMD_SCSI {
    MFI_HDR     hdr;                    /* message frame header */
    MFI_ADDRESS requestSense;           /* request sense buffer pointer */
    U8          cdb[16];
    MFI_SGE     sge[1];                 /* only declare 1 SGE - number is actually variable */
} MFI_CMD_SCSI, *PMFI_CMD_SCSI;

typedef struct _MFI_FRAME_SCSI {
    U8          cmd;                    /* MFI command (type MFI_CMD_OP) */
    U8          requestSenseLength;     /* input (and returned) length of request sense buffer */
    U8          cmdStatus;              /* returned MFI command status (MFI_STATUS) */
    union {
        U8          targetIDMsb;            /* Input: MSB of target ID (overwritten by extStatus on command completion) */
        U8          scsiStatus;             /* Output: returned SCSI status for CDB commands */
        U8          msgId;                  /* DEPRECATED Input: MSI/MSI-X Message Id sent by host */
    } idInStatusOut;
    U8          targetID;               /* target ID (LD) */
    U8          lun;                    /* target ID LUN */
    U8          cdbLength;              /* SCSI CDB length */
    U8          numSGE;                 /* number of SG Elements */

    union {
        MFI_ADDRESS context;           /* DEPRECATED driver context field low word */
        struct {
            U32     cmdContext;        /* driver context field */
            U8      requestorId;       /* requestor Id */
            U8      msgVector;         /* MSI/MSI-X message Vector sent by host; replaces msgId */
            U8      pad[2];            /* reserved for future use */
        } mfiContextRegs;              /* unnamed struct:  search for NO_UNNAMED_STRUCT for usage */
    };

    MFI_FLAGS   flags;                  /* command flags */
    U16         timeout;                /* command timeout (in seconds) */
    U32         length;                 /* length of this command's data */

    MFI_ADDRESS requestSense;           /* request sense buffer pointer */
    U8          cdb[16];
    MFI_SGE     sge[1];                 /* only declare 1 SGE - number is actually variable */
} MFI_FRAME_SCSI, *PMFI_FRAME_SCSI;

/*
 * declare the MegaRAID DCMD structure
 */
typedef struct _MFI_CMD_DCMD {
    MFI_HDR hdr;                        /* message frame header */
    U32     opcode;                     /* 32-bit opcode for DCMDs */
    union {
        U8      b[12];                  /* byte reference to mailbox */
        U16     s[6];                   /* short reference to mailbox */
        U32     w[3];                   /* word access to mailbox data */
    } mbox;
    MFI_SGE sge[2];                     /* SGE entries */
} MFI_CMD_DCMD, *PMFI_CMD_DCMD;

typedef struct _MFI_FRAME_DCMD {
    U8          cmd;                    /* MFI command (type MFI_CMD_OP) */
    U8          reserved;               /* set to 0 */
    U8          cmdStatus;              /* returned MFI command status (MFI_STATUS) */
    union {
        U8      msgId;                  /* DEPRECATED Input: MSI/MSI-X Message Id sent by host */
        U8      reserved1;              /* For future use */
    };
    U8          reserved2[3];           /* set to 0 */
    U8          numSGE;                 /* number of SG Elements */

    union {
        MFI_ADDRESS context;           /* DEPRECATED driver context field low word */
        struct {
            U32     cmdContext;        /* driver context field */
            U8      requestorId;       /* requestor Id */
            U8      msgVector;         /* MSI/MSI-X message Vector sent by host; replaces msgId */
            U8      pad[2];            /* reserved for future use */
        } mfiContextRegs;              /* unnamed struct:  search for NO_UNNAMED_STRUCT for usage */
    };

    MFI_FLAGS   flags;                  /* command flags */
    U16         timeout;                /* command timeout (in seconds) */
    U32         dataTransferlength;     /* length of this command's data */

    U32        opcode;                  /* 32-bit opcode for DCMDs */
    union {
        U8      b[12];                  /* byte reference to mailbox */
        U16     s[6];                   /* short reference to mailbox */
        U32     w[3];                   /* word access to mailbox data */
    } mbox;
    MFI_SGE    sge[2];                  /* SGE entries */
} MFI_FRAME_DCMD, *PMFI_FRAME_DCMD;

/*
 * define ABORT structure
 */
typedef struct _MFI_CMD_ABORT {
    MFI_HDR hdr;                        /* message frame header */
    union {
        MFI_ADDRESS abortContext;       /* DEPRECATED context of command to abort */
        struct {
            U32     abortCmdContext;    /* context of command to abort */
            U8      reserved[4];        /* reserved for future use */
        } mfiAbortRegs;                 /* unnamed struct:  search for NO_UNNAMED_STRUCT for usage */
    };
    MFI_ADDRESS abortMfa;               /* MFA of command to abort */
} MFI_CMD_ABORT, *PMFI_CMD_ABORT;

typedef struct _MFI_FRAME_ABORT {
    U8          cmd;                    /* MFI command (type MFI_CMD_OP) */
    U8          reserved1;              /* set to 0 */
    U8          cmdStatus;              /* returned MFI command status (MFI_STATUS) */
    union {
        U8      msgId;                  /* DEPRECATED Input: MSI/MSI-X Message Id sent by host */
        U8      reserved4;              /* For future use */
    };
    U8          reserved2[4];           /* set to 0 */

    union {
        MFI_ADDRESS context;           /* DEPRECATED driver context field low word */
        struct {
            U32     cmdContext;        /* driver context field */
            U8      requestorId;       /* requestor Id */
            U8      msgVector;         /* MSI/MSI-X message Vector sent by host; replaces msgId */
            U8      pad[2];            /* reserved for future use */
        } mfiContextRegs;              /* unnamed struct:  search for NO_UNNAMED_STRUCT for usage */
    };

    MFI_FLAGS   flags;                  /* command flags */
    U8          reserved3[6];           /* set to 0 */

    union {
        MFI_ADDRESS abortContext;       /* DEPRECATED context of command to abort */
        struct {
            U32     abortCmdContext;    /* context of command to abort */
            U8      reserved[4];        /* reserved for future use */
        } mfiAbortRegs;                 /* unnamed struct:  search for NO_UNNAMED_STRUCT for usage */
    };
    MFI_ADDRESS abortMfa;               /* MFA of command to abort */
} MFI_FRAME_ABORT, *PMFI_FRAME_ABORT;

/*
 * define the SMP structure
 */
typedef struct _MFI_CMD_SMP {
    MFI_HDR     hdr;                    /* message frame header (extStatus will contain MFI_SMP_STAT) */
    U64         sasAddr;                /* device SAS address (0 = use hdr.targetID) */
    MFI_SGE     sge[2];                 /* SGE entries: [0]=SMP Response, [1]=SMP Request */
} MFI_CMD_SMP, *PMFI_CMD_SMP;

typedef struct _MFI_FRAME_SMP {
    U8          cmd;                    /* MFI command (type MFI_CMD_OP) */
    U8          reserved;               /* set to 0 */
    U8          cmdStatus;              /* returned MFI command status (MFI_STATUS) */
    union {
        U8          connectionStatus;       /* Output: SAS Connection Status (MFI_SMP_STAT) */
        U8          msgId;                  /* DEPRECATED Input: MSI/MSI-X Message Id sent by host */
    } idInStatusOut;
    U8          reserved2[3];           /* set to 0 */
    U8          numSGE;                 /* number of SG Elements */

    union {
        MFI_ADDRESS context;           /* DEPRECATED driver context field low word */
        struct {
            U32     cmdContext;        /* driver context field */
            U8      requestorId;       /* requestor Id */
            U8      msgVector;         /* MSI/MSI-X message Vector sent by host; replaces msgId */
            U8      pad[2];            /* reserved for future use */
        } mfiContextRegs;              /* unnamed struct:  search for NO_UNNAMED_STRUCT for usage */
    };

    MFI_FLAGS   flags;                  /* command flags */
    U16         timeout;                /* command timeout (in seconds) */
    U32         dataTransferlength;     /* length of this command's data */
    U64         sasAddr;                /* device SAS address (0 = use hdr.targetID) */
    MFI_SGE     sge[2];                 /* SGE entries: [0]=SMP Response, [1]=SMP Request */
} MFI_FRAME_SMP, *PMFI_FRAME_SMP;

typedef enum _MFI_SMP_STAT {
    MFI_SMP_STAT_OK                                = 0x00,  /* Command completed successfully */
    MFI_SMP_STAT_UNKNOWN_ERROR                     = 0x01,  /* Unknown error */
    MFI_SMP_STAT_INVALID_FRAME                     = 0x02,  /* Invalid frame */
    MFI_SMP_STAT_UTC_BAD_DEST                      = 0x03,  /* Unable to Connect (Bad Destination) */
    MFI_SMP_STAT_UTC_BREAK_RECEIVED                = 0x04,  /* Unable to Connect (Break Received) */
    MFI_SMP_STAT_UTC_CONNECT_RATE_NOT_SUPPORTED    = 0x05,  /* Unable to Connect (Connect Rate Not Supported) */
    MFI_SMP_STAT_UTC_PORT_LAYER_REQUEST            = 0x06,  /* Unable to Connect (Port Layer Request) */
    MFI_SMP_STAT_UTC_PROTOCOL_NOT_SUPPORTED        = 0x07,  /* Unable to Connect (Protocol Not Supported) */
    MFI_SMP_STAT_UTC_STP_RESOURCES_BUSY            = 0x08,  /* Unable to Connect (STP Resources Busy) */
    MFI_SMP_STAT_UTC_WRONG_DESTINATION             = 0x09,  /* Unable to Connect (Wrong Destination) */
    MFI_SMP_STAT_SHORT_INFORMATION_UNIT            = 0x0A,  /* Information Unit too short */
    MFI_SMP_STAT_LONG_INFORMATION_UNIT             = 0x0B,  /* Information Unit too long */
    MFI_SMP_STAT_XFER_RDY_INCORRECT_WRITE_DATA     = 0x0C,  /* XFER_RDY Incorrect Write Data */
    MFI_SMP_STAT_XFER_RDY_REQUEST_OFFSET_ERROR     = 0x0D,  /* XFER_RDY Request Offset Error */
    MFI_SMP_STAT_XFER_RDY_NOT_EXPECTED             = 0x0E,  /* XFER_RDY Not Expected */
    MFI_SMP_STAT_DATA_INCORRECT_DATA_LENGTH        = 0x0F,  /* DATA Incorrect Data Length */
    MFI_SMP_STAT_DATA_TOO_MUCH_READ_DATA           = 0x10,  /* DATA Too much read data */
    MFI_SMP_STAT_DATA_OFFSET_ERROR                 = 0x11,  /* DATA Offset Error */
    MFI_SMP_STAT_SDSF_NAK_RECEIVED                 = 0x12,  /* Service Delivery Subsystem Failure - NAK received */
    MFI_SMP_STAT_SDSF_CONNECTION_FAILED            = 0x13,  /* Service Delivery Subsystem Failure - Connection failed */
    MFI_SMP_STAT_INITIATOR_RESPONSE_TIMEOUT        = 0x14   /* Initiator response timeout */
} MFI_SMP_STAT;

/*
 * define the STP structure (SATA Tunneling Protocol)
 */
typedef struct _MFI_STP_FIS {
    U16         fis[10];
    U32         stpFlags;               /* STP flags - CSMI_SAS_STP_FLAGS */
} MFI_STP_FIS;

typedef struct _MFI_CMD_STP_STATUS {    /* define the STP response data */
    U16         fis[10];                /* status FIS as defined by SATA specification */
    U32         StatusControlReg;       /* status/control register at command completion as defined by SATA */
} MFI_CMD_STP_STATUS;

typedef struct _MFI_CMD_STP {
    MFI_HDR     hdr;                    /* message frame header */
    U16         fis[10];                /* STP FIS block */
    U32         stpFlags;               /* STP flags - CSMI_SAS_STP_FLAGS */
    MFI_SGE     sge[1];                 /* SGE entries: [0]=FIS Response (type MFI_CMD_STP_STATUS), [1..]=Data */
} MFI_CMD_STP, *PMFI_CMD_STP;

typedef struct _MFI_FRAME_STP {
    U8          cmd;                    /* MFI command (type MFI_CMD_OP) */
    U8          reserved;               /* set to 0 */
    U8          cmdStatus;              /* returned MFI command status (MFI_STATUS) */
    union {
        U8          connectionStatus;       /* Output: SAS Connection Status (MFI_SMP_STAT) */
        U8          msgId;                  /* DEPRECATED Input: MSI/MSI-X Message Id sent by host */
    } idInStatusOut;
    U8          targetId;               /* targetId, valid when SASAddress is zero */
    U8          targetIdMsb;            /* MSB of targetId field */
    U8          reserved3;              /* set to 0 */
    U8          numSGE;                 /* number of SG Elements */

    union {
        MFI_ADDRESS context;           /* DEPRECATED driver context field low word */
        struct {
            U32     cmdContext;        /* driver context field */
            U8      requestorId;       /* requestor Id */
            U8      msgVector;         /* MSI/MSI-X message Vector sent by host; replaces msgId */
            U8      pad[2];            /* reserved for future use */
        } mfiContextRegs;              /* unnamed struct:  search for NO_UNNAMED_STRUCT for usage */
    };

    MFI_FLAGS   flags;                  /* command flags */
    U16         timeout;                /* command timeout (in seconds) */
    U32         dataTransferlength;     /* length of this command's data */
    U16         fis[10];                /* STP FIS block */
    U32         stpFlags;               /* STP flags - CSMI_SAS_STP_FLAGS */
    MFI_SGE     sge[1];                 /* SGE entries: [0]=FIS Response (type MFI_CMD_STP_STATUS), [1..]=Data */
} MFI_FRAME_STP, *PMFI_FRAME_STP;

#define MAX_NVME_ENCAP_SIZE            128  /* Provides room to grow , current max from NVME Spec is 64 */
#define MAX_ERROR_RESPONSE_DATA_SIZE    32  /* Provides room to grow, current max from NVMe spec is 16*/
#define MAX_NVME_ENCAP_ERROR_REPLY_SIZE 48  /* Provides room to grow, current max from MPT is 24 */
#define NVME_ENCAP_REVISION              1 /* tracks changes to MFI_NVME_ENCAP structure */
                                           /* Increment value by 1 for changes to below structure*/

/* This structure wants to be less than the minimum size of a host page, so that it will consume only 1 SGE */
/* At this point the structure must be less that 4K in size */

typedef struct _MFI_NVME_ENCAPSULATED {
    U8          revision;
    U8          NVMeEncapSize;            /* actual bytes in NVMeEncapCommand that holds valid data*/
    U8          errorResponseDataSize;    /* actual bytes in errorResponseData that holds valid data*/
    U8          NVMeEncapErrorReplySize;  /* actual bytes in NVMeEncapErrorReply that holds valid data*/
    U8          NVMeEncapCommand[MAX_NVME_ENCAP_SIZE]; /* fill with NVMe_Command of MPT Message*/
    U8          errorResponseData[MAX_ERROR_RESPONSE_DATA_SIZE]; /* fill with ErorResponseBaseAddress of MPT message*/
    U8          NVMeEncapErrorReply[MAX_NVME_ENCAP_ERROR_REPLY_SIZE]; /* MPT NVme Encapsulated Error Reply Message*/
} MFI_NVME_ENCAPSULATED, *PMFI_NVME_ENCAPSULATED;



typedef struct _MFI_FRAME_NVME {
    U8          cmd;                    /* MFI command (type MFI_CMD_OP) */
    U8          reserved;               /* set to 0 */
    U8          cmdStatus;              /* returned MFI command status (MFI_STATUS) */
    U8          reserved1;              /* for common definitions */
    U16         targetId;               /* targetId */
    U8          reserved2;              /* set to 0 */
    U8          numSGE;                 /* number of SG Elements */

    union {
        MFI_ADDRESS context;            /* DEPRECATED driver context field low word */
        struct {
            U32     cmdContext;        /* driver context field */
            U8      requestorId;       /* requestor Id */
            U8      msgVector;         /* MSI/MSI-X message Vector sent by host; replaces msgId */
            U8      pad[2];            /* reserved for future use */
        } mfiContextRegs;              /* unnamed struct:  search for NO_UNNAMED_STRUCT for usage */
    };

    MFI_FLAGS   flags;                  /* command flags */
    U16         timeout;                /* command timeout (in seconds) */
    U16         encapsulatedNVMeFlags;  /* "flags" field of MPT NVMe Encapsulated Request */
    U16         pad0;                   /* padding for alignment of sge address */
    U32         dataTransferLength;     /* length of data (sge[1..]*/
    MFI_SGE     sge[1];                 /* SGE entries: */
                                        /* [0]= Pointer to MFI_NVME_ENCAP */
                                        /* [1..]=Data */
} MFI_FRAME_NVME, *PMFI_FRAME_NVME;

/*
 * define a generic MFI command union
 */
typedef union _MFI_CMD {
    MFI_HDR                 hdr;
    MFI_CMD_INIT            init;
    MFI_CMD_LD_READ_WRITE   rw;
    MFI_CMD_SCSI            scsi;
    MFI_CMD_DCMD            dcmd;
    MFI_CMD_ABORT           abort;
    MFI_CMD_SMP             smp;        /* SAS Management Protocol */
    MFI_CMD_STP             stp;        /* SATA Tunneling Protocol */
    MFI_FRAME_NVME          nvme;       /* NVME passthrough Protocol */
    U8                      b[64];      /* standard frame is 64 bytes */
    U16                     s[32];      /* short access to structure */
    U32                     w[16];      /* word access to the frame */
    U64                     d[8];       /* dword access */
} MFI_CMD, *PMFI_CMD;

typedef union _MFI_CMD_FRAMES {
    MFI_HDR                 hdr;        /* MFI Header */
    MFI_FRAME_INIT          init;
    MFI_FRAME_LD_READ_WRITE rw;
    MFI_FRAME_SCSI          scsi;
    MFI_FRAME_DCMD          dcmd;
    MFI_FRAME_ABORT         abort;
    MFI_FRAME_SMP           smp;        /* SAS Management Protocol */
    MFI_FRAME_STP           stp;        /* SATA Tunneling Protocol */
    MFI_FRAME_NVME          nvme;       /* NVME passthrough Protocol */
    U8                      b[64];      /* standard frame is 64 bytes */
    U16                     w[32];      /* word access to structure */
    U32                     d[16];      /* double word access to the frame */
    U64                     l[8];       /* large access */
} MFI_CMD_FRAMES, *PMFI_CMD_FRAMES;

/*
 * Declare generic MFI frames with 7 additional SGE frames.  Firmware does
 * support a larger MFI frame count, based on the supported SGE count.
 */
typedef struct _MFI_CMD_WITH_SGL {
    MFI_CMD     cmd;
    union {
        MFI_SGE     sge[37];
        MFI_SGE_32  sge32[56];
        MFI_SGE_64  sge64[37];
        U8          b[MFI_FRAME_SIZE*7];  /* standard frame is 64 bytes */
    } sgl;
} MFI_CMD_WITH_SGL, *PMFI_CMD_WITH_SGL;

typedef struct _MFI_CMD_FRAMES_WITH_SGL {
    MFI_CMD_FRAMES  cmd;
    union {
        MFI_SGE     sge[37];
        MFI_SGE_32  sge32[56];
        MFI_SGE_64  sge64[37];
        U8          b[MFI_FRAME_SIZE*7];  /* standard frame is 64 bytes */
    } sgl;
} MFI_CMD_FRAMES_WITH_SGL, *PMFI_CMD_FRAMES_WITH_SGL;


/*
 * MR-MPI2 function codes.  These function codes utilize the vendor
 * specific range of the MPT2 spec
 */
#define MR_MPI2_FUNCTION_MFI_PASSTHRU_REQUEST      0xF0  /* MFI request passed through MPI2 queuing */
#define MR_MPI2_FUNCTION_LD_IO_REQUEST             0xF1  /* MPI2 formatted IO request targetting LD */

/*
 * MR-MPI2 Request descriptor types.  These descriptor types are
 * posted to the hw's inbound queues and used by the fw to
 * determine the type of frame being sent.
 */
#define MR_MPI2_REQ_DESCRIPT_FLAGS_MPT             0x0
#define MR_MPI2_REQ_DESCRIPT_FLAGS_MFA             0x1
#define MR_MPI2_REQ_DESCRIPT_FLAGS_RL_BYPASS_IO    0x2
#define MR_MPI2_REQ_DESCRIPT_FLAGS_HI_PRIORITY     0x3
#define MR_MPI2_REQ_DESCRIPT_FLAGS_FP_IO           0x6
#define MR_MPI2_REQ_DESCRIPT_FLAGS_LD_IO           0x7

/*
 * Queue 0 - not used
 * Queue 1 - for descriptor type 1, MFA (no autopull, same as HiPriority Q in 2208/2108/2008)
 * Queue 2 - for descriptor type 6, Fast Path IO and all other IO types going to FPE/Cuda
 * Queue 3 - not used
 */
#define MR_MPI2_REQ_Q_DESC_DEFAULT                  (0)  // Default - not used
#define MR_MPI2_REQ_Q_DESC_MFA_HIPRI                (1)  // for desc type 1 (MFA)
#define MR_MPI2_REQ_Q_DESC_FPE                      (2)  // for all other desc types going to FPE/Cuda
#define MR_MPI2_REQ_Q_DESC_RL_BYPASS                (3)  // for IOs which bypasses the region lock

/*
 * Region Lock Grant Desitinations
 */
typedef enum _MR_RL_FLAGS_GRANT_DESTINATION {
    MR_MPI2_RL_FLAGS_GRANT_DESTINATION_CPU0  = 0x00,
    MR_MPI2_RL_FLAGS_GRANT_DESTINATION_CPU1  = 0x10,
    MR_MPI2_RL_FLAGS_GRANT_DESTINATION_CUDA  = 0x80,
} MR_RL_FLAGS_GRANT_DESTINATION;

/*
 * Define the boost priority range of Application/File System priority to firmware for caching
 */
typedef enum _MR_BOOST_PRIORITY_RANGE {
    MR_BOOST_PRIORITY_NONE          = 0,     /* boost priority none or not required */
    MR_BOOST_PRIORITY_MIN           = 1,     /* boost priority minimum value */
    MR_BOOST_PRIORITY_MAX           = 15,    /* boost priority maximum value */
} MR_BOOST_PRIORITY_RANGE;

/*
 * Config Sequence Number enable
 */
#define MR_MPI2_RL_FLAGS_SEQ_NUM_ENABLE             (0x08)


/*
 * MR-MPI2 MFA Descriptor format.  This is used to pass MFI frames prior
 * to the queues being initialized(MFI_CMD_OP_INIT).  The PCI address
 * must be 256 byte aligned due to only the upper 56 bits being used.
 */
typedef struct _MR_MPI2_RAID_MFA_DESCRIPTOR {
    U32     RequestFlags    : 8;
    U32     MessageAddress1 : 24; /*  PCI address bits 31:8*/
    U32     MessageAddress2;      /*  PCI address bits 61:32 */
} MR_MPI2_MFA_REQUEST_DESCRIPTOR,*PMR_MPI2_MFA_REQUEST_DESCRIPTOR;

/*
 * MR-MPI2 RAID Context Area.  When MPI2_SCSI_IO_VENDOR_UNIQUE is defined, the
 * MPI headers will add struct MPI2_SCSI_IO_VENDOR_UNIQUE into the MPI2 SCSI IO
 * structure between the CDB and SGL.
 */

typedef struct _MR_RAID_CTX_G35 {
    struct {
#ifndef MFI_BIG_ENDIAN
        U16     contextType  :4;            // 0x00 -0x01
        U16     numSegments  :4;
        U16     rsvd         :8;
#else
        U16     rsvd         :8;
        U16     numSegments  :4;
        U16     contextType  :4;
#endif
    };
        U16     timeoutValue;               // 0x02 -0x03
        union {                             // 0x04 -0x05 routing flags
            struct {
#ifndef MFI_BIG_ENDIAN
                U16     reserved     :1;
                U16     sld          :1;
                U16     c2f          :1;
                U16     fwn          :1;
                U16     sqn          :1;
                U16     sbs          :1;
                U16     rw           :1;
                U16     log          :1;
                U16     cpuSel       :4;
                U16     setDivert    :4;
#else
                U16     setDivert    :4;
                U16     cpuSel       :4;
                U16     log          :1;
                U16     rw           :1;
                U16     sbs          :1;
                U16     sqn          :1;
                U16     fwn          :1;
                U16     c2f          :1;
                U16     sld          :1;
                U16     reserved     :1;
#endif
            } bits;
            U16 s;
        } routingFlags;                 // 0x04 -0x05 routing flags
        U16     ldTargetId;             // 0x06 - 0x07
        union {
            U64     rowLBA;
            U64     lba;                // 0x08 - 0x0f, SCSI or ROW LBA used by divert engine
        };
        union {
            U32     regLockLength;
            U32     length;             // 0x10 - 0x13 SCSI length
        };
        union {                     // flow specific
            U16 rmwOpIndex;         // 0x14 - 0x15, R5/6 RMW: rmw operation index
            U16 peerSmid;           // 0x14 - 0x15, R1 Write: peer smid
            U16 r56ArmMap;          // 0x14 - 0x15, Unused [15], LogArm[14:10], P-Arm[9:5], Q-Arm[4:0]
        } flowSpecific;
        U8      extStatus;          // 0x16, output - Ext Status  set by firmware
        U8      status;             // 0x17, status

        U8      RAIDFlags;          // 0x18, ioSubType[7:4], RCExt[3], preferredCpu[2:0]
        U8      spanArm;            // 0x19, span[7:5], arm[4:0]
        U16     configSeqNum;       // 0x1A -0x1B

#ifndef MFI_BIG_ENDIAN              // 0x1C - 0x1D
        U16     numSGE         : 12;
        U16     reserved       : 3;
        U16     streamDetected : 1;
#else
        U16     streamDetected : 1;
        U16     reserved       : 3;
        U16     numSGE         : 12;
#endif
        U8      resvd2[2];          // 0x1E-0x1F
} MR_RAID_CTX_G35; // 0x20 bytes

typedef struct _MR_RAID_CTX_G2 {
#ifndef MFI_BIG_ENDIAN
    U16     contextType  :4;     // 0x00 -0x01
    U16     numSegments  :4;
    U16     streamDetected :1;
    U16     divertedFPIO :1;     // Driver fills this bit to indicate diverted FP IO
    U16     IOPInitiated :1;     // FW sets this bit when FW reads the SCSIO IO Request. Cleared by driver
    U16     rsvd         :5;
#else
    U16     rsvd         :5;
    U16     IOPInitiated :1;     // FW sets this bit when FW reads the SCSIO IO Request. Cleared by driver
    U16     divertedFPIO :1;     // Driver fills this bit, to indicate diverted FP IO
    U16     streamDetected :1;
    U16     numSegments  :4;
    U16     contextType  :4;
#endif
    U16     timeoutValue;       // 0x02 -0x03
    U8      regLockFlags;       // 0x04
    U8      reservedForHw1;     // 0x05
    U16     ldTargetId;         // 0x06 - 0x07
    U64     regLockRowLBA;      // 0x08 - 0x0F
    U32     regLockLength;      // 0x10 - 0x13
    U16     nextLMId;           // 0x14 - 0x15
    union {                     /*
                                 * Location 0x16 is overloaded.
                                 * logArm is valid while command issue if driver set Raptor Latency sensitive fastpath *
                                 * extStatus is valid while command completion
                                 */
        U8      extStatus;      // 0x16 - output - Ext Status  set by firmware
        U8      logArm;         // 0x16 - input  - Logical Arm set by driver
    };
    U8      status;             // 0x17 status
    U8      RAIDFlags;          // 0x18 resvd[7:6], ioSubType[5:4], resvd[3:1], preferredCpu[0]
    U8      numSGE;             // 0x19 numSge; not including chain entries
    U16     configSeqNum;       // 0x1A -0x1B
    U8      spanArm;            // 0x1C span[7:5], arm[4:0]
    U8      boostPriority;      // 0x1D - MR_BOOST_PRIORITY_RANGE
    U8      numSGEExt;          // 0x1E - To support larger IO sizes, numSGE and numSGEExt will be used as two parts
    U8      resvd2;          // 0x1F
} MR_RAID_CTX_G2;

/*
 * If MPI_VENDOR_UNIQUE_TYPE_NONE is defined and SAS Generation is 3.5,
 * both MPI2_SCSI_IO_VENDOR_UNIQUE & MPI25_SCSI_IO_VENDOR_UNIQUE will
 * correspond to raid context definitions from generation 3.5.
 * Else, both will correspond to raid context definitions from generation 2.
 */
#if !defined(MPI_VENDOR_UNIQUE_TYPE_NONE)
#if defined(MPI_MR_GEN35_RAID_CTX)
typedef  MR_RAID_CTX_G35 MPI2_SCSI_IO_VENDOR_UNIQUE;
typedef  MR_RAID_CTX_G35 MPI25_SCSI_IO_VENDOR_UNIQUE;
#define MR_RAID_CTX_RAID_FLAGS_IO_SUB_TYPE_SHIFT       (4)
#define MR_RAID_CTX_RAID_FLAGS_IO_SUB_TYPE_MASK        (0xF0)
#define MR_RAID_CTX_RAID_FLAGS_PREFFRED_CPU_SHIFT      (0)
#define MR_RAID_CTX_RAID_FLAGS_PREFFRED_CPU_MASK       (0x7)


/*
 * These are the values that will be sent by the driver in the raidFlags field of the RAID_CONTEXT
 */
typedef enum MR_RAID_FLAGS_IO_SUB_TYPE {
    MR_RAID_FLAGS_IO_SUB_TYPE_NONE         = 0,
    MR_RAID_FLAGS_IO_SUB_TYPE_SYSTEM_PD    = 1,
    MR_RAID_FLAGS_IO_SUB_TYPE_RMW_DATA     = 2,
    MR_RAID_FLAGS_IO_SUB_TYPE_RMW_P        = 3,
    MR_RAID_FLAGS_IO_SUB_TYPE_RMW_Q        = 4,
    MR_RAID_FLAGS_IO_SUB_TYPE_CACHE_BYPASS = 6,
    MR_RAID_FLAGS_IO_SUB_TYPE_LDIO_BW_LIMIT = 7,
    MR_RAID_FLAGS_IO_SUB_TYPE_R56_DIV_OFFLOAD = 8,

} MR_RAID_FLAGS_IO_SUB_TYPE;

typedef struct _MR_RAID_FLAGS {
#ifndef MFI_BIG_ENDIAN
    U8 preferredCPU : 3;
    U8 rcExt : 1;
    U8 ioSubType : 4;
#else
    U8 ioSubType : 4;
    U8 rcExt : 1;
    U8 preferredCPU : 3;
#endif
} MR_RAID_FLAGS;

#else // #if defined(MPI_MR_GEN35_RAID_CTX)
typedef  MR_RAID_CTX_G2  MPI2_SCSI_IO_VENDOR_UNIQUE;
typedef  MR_RAID_CTX_G2  MPI25_SCSI_IO_VENDOR_UNIQUE;
#define MR_RAID_CTX_RAID_FLAGS_IO_SUB_TYPE_SHIFT       (4)
#define MR_RAID_CTX_RAID_FLAGS_IO_SUB_TYPE_MASK        (0x30)
#define MR_RAID_CTX_RAID_FLAGS_PREFFRED_CPU_SHIFT      (0)
#define MR_RAID_CTX_RAID_FLAGS_PREFFRED_CPU_MASK       (0x1)

/*
 * These are the values that will be sent by the driver in the raidFlags field of the RAID_CONTEXT
 */
typedef enum MR_RAID_FLAGS_IO_SUB_TYPE {
    MR_RAID_FLAGS_IO_SUB_TYPE_NONE         = 0,
    MR_RAID_FLAGS_IO_SUB_TYPE_SYSTEM_PD    = 1,
} MR_RAID_FLAGS_IO_SUB_TYPE;
#endif  // #if defined(MPI_MR_GEN35_RAID_CTX)
#endif  // #if !defined(MPI_VENDOR_UNIQUE_TYPE_NONE)


#define MR_RAID_CTX_SPANARM_ARM_SHIFT      (0)
#define MR_RAID_CTX_SPANARM_ARM_MASK       (0x1f)

#define MR_RAID_CTX_SPANARM_SPAN_SHIFT     (5)
#define MR_RAID_CTX_SPANARM_SPAN_MASK      (0xE0)

/* LogArm[14:10], P-Arm[9:5], Q-Arm[4:0] */
#define RAID_CTX_R56_Q_ARM_MASK            (0x1F)
#define RAID_CTX_R56_P_ARM_SHIFT           (5)
#define RAID_CTX_R56_P_ARM_MASK            (0x3E0)
#define RAID_CTX_R56_LOG_ARM_SHIFT         (10)
#define RAID_CTX_R56_LOG_ARM_MASK          (0x7C00)

typedef enum _MR_RAID_CTX_REGION_TYPE {
    MR_REGION_TYPE_UNUSED       = 0,    /* lock is currently not used */
    MR_REGION_TYPE_SHARED_READ  = 1,    /* shared lock for reads */
    MR_REGION_TYPE_SHARED_WRITE = 2,    /* shared lock for writes */
    MR_REGION_TYPE_EXCLUSIVE    = 3,    /* exclusive lock */
} MR_RAID_CTX_REGION_TYPE;

typedef enum _MR_RAID_CTX_CONTEXT_TYPE {
    MR_RAID_CTX_TYPE_UNINTERPRETED  = 0,    /* HW will not interpret the RAID context */
    MR_RAID_CTX_TYPE_TIMEOUT        = 1,    /* HW will time this IO using the timeout value in RAID context */
    MR_RAID_CTX_TYPE_MR_REGION_LOCK = 2,    /* HW will time this IO and take RL using values in RAID context */
    MR_RAID_CTX_TYPE_MFI_I2O        = 3     /* Not used for MR */
} MR_RAID_CTX_CONTEXT_TYPE;

/*
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 *
 * Structures, macros and defines related to the OOB Interface
 *
 *---------------------------------------------------------------------------------------------------
 *---------------------------------------------------------------------------------------------------
 */

/*
 * defines the enum for LSI packet type
 */
typedef enum _MFI_OOB_CONTROL_CMD {
    MFI_OOB_CONTROL_MESSAGE      = 0x0,    // Packet is an LSI message and not a control command
                                           // control commands
    MFI_OOB_CONTROL_RESET        = 0x8,    // Control Command reset the interface and FW OOB service
    MFI_OOB_CONTROL_RESUME       = 0x9,    // Control Command resume, payload is MFI_OOB_CMD_RESPONSE.packetSeqNum
    MFI_OOB_CONTROL_RESPONSE     = 0xA,    // Control Command response, payload is MFI_OOB_STATUS
    MFI_OOB_CONTROL_OOB_STATUS   = 0xB,    // Control Command OOB status - a quick way to check status of OOB service
    MFI_OOB_CONTROL_STATUS_POLL  = 0xC,    // Control Command status poll - a quick way to poll controller status
    MFI_OOB_CONTROL_MFI_STAT     = 0xD,    // MFI STAT response for DCMD, payload is MFI_STAT
    MFI_OOB_CONTROL_GENERIC_INFO = 0xE,    // Control Command to provide certain controller information.
                                           // Payload field should be set to one of the enum values in MFI_OOB_CTRL_INO_PARAMS
} MFI_OOB_CONTROL_CMD;

/*
 * defines the enum for LSI CONTROL STATUS
 */
typedef enum _MFI_OOB_STATUS {
    MFI_OOB_STATUS_OK                   = 0x00,  // command completed successfully
    MFI_OOB_STATUS_INVALID_CMD          = 0x01,  // invalid control command
    MFI_OOB_STATUS_INVALID_PARAMETER    = 0x02,  // invalid control command paramter
    MFI_OOB_STATUS_INVALID_CHECKSUM     = 0x03,  // invalid packet checksum
    MFI_OOB_STATUS_NOT_READY            = 0x04,  // FW OOB service is not ready, retry later
    MFI_OOB_STATUS_WRONG_STATE          = 0x05,  // FW OOB service cannot process this CMD at this time
    MFI_OOB_STATUS_CANNOT_RESUME        = 0x06,  // FW OOB service cannot resume, command is not active
    MFI_OOB_STATUS_INVALID              = 0xFF,  // invalid status - used for polling command completion
} MFI_OOB_STATUS;

/*
 * define LSI proprietary packet header
 */
typedef struct _MFI_OOB_LSI_HDR {
#ifndef MFI_BIG_ENDIAN
    U8  srcSlaveAddressRW   : 1;    /* Byte0 : read without prior write is not allowed  */
    U8  srcSlaveAddress     : 7;    /* Byte0 : i2c slave address (BMC for commands, controller for response) */
#else
    U8  srcSlaveAddress     : 7;    /* Byte0 : i2c slave address (BMC for commands, controller for response) */
    U8  srcSlaveAddressRW   : 1;    /* Byte0 : read without prior write is not allowed  */
#endif
    U8  applicationMsgTag;          /* Byte1 : optional context for BMC to tag each message
                                               0xFF is reserved */
    union {
        struct {
#ifndef MFI_BIG_ENDIAN
            U16 controlCmd  : 4;    /* Byte2 : MFI_OOB_CONTROL_CMD */
            U16 payload     :12;    /* Byte2&3 : payload of control command */
#else
            U16 payload     :12;    /* Byte2&3 : payload of control command */
            U16 controlCmd  : 4;    /* Byte2 : MFI_OOB_CONTROL_CMD */
#endif
        } controlCommand;

        struct {
#ifndef MFI_BIG_ENDIAN
            U8  controlCmd  : 4;    /* Byte2 : 0x0 - Pkt is an LSI message (MFI_OOB_CMD_PACKET) */
            U8  ic          : 1;    /* Byte2: integrity check. 1-following byte is checksum */
            U8  resvd       : 3;    /* for future use */
#else
            U8  resvd       : 3;    /* for future use */
            U8  ic          : 1;    /* Byte2: integrity check. 1-following byte is checksum */
            U8  controlCmd  : 4;    /* Byte2 : 0x0 - Pkt is an LSI message (MFI_OOB_CMD_PACKET) */
#endif
            union {                 /* Byte3 */
                U8  checksum;       /*         CRC-8 of the rest of the bytes in the packet, if checksum bit is set */
                U8  payload;        /*         for future use */
            };
        } message;
    };
} MFI_OOB_LSI_HDR;

typedef struct _MFI_OOB_PCI_MSG_HDR {
#ifndef MFI_BIG_ENDIAN
    U8  msgType        : 7;         /* Msg type should be 0x7E for PCI VDM messages once the VDM driver is initialized
                                       VDM driver can send the msgType as 0x0 during end point discovery */
    U8  ic             : 1;         /* Integrity check - checksum at the end of packet */
#else
    U8  ic             : 1;         /* Integrity check - checksum at the end of packet */
    U8  msgType        : 7;         /* Msg type should be 0x7E for PCI VDM messages once the VDM driver is initialized
                                       VDM driver can send the msgType as 0x0 during end point discovery */
#endif
    U8  vendorID[2];                /* Vendor ID of the end point PCIe device */
    U8  controlMsg;                 /* One of the enum values from MFI_OOB_MCTP_CONTROL_MSG */
    U16 payload;                    /* payload of control message */
    U8  applicationMsgTag;          /* optional context for BMC to tag each message */
    U8  reserved[5];                /* for future use */
} MFI_OOB_PCI_MSG_HDR;

/*
 * define MFI OOB Message payload - COMMAND packets
 */

typedef struct _MFI_OOB_CMD_PACKET1 {
#ifndef MFI_BIG_ENDIAN
    U16 packetSeqNum    :12;        /* Byte0&1: 1 = indicates this is MFI_OOB_CMD_PACKET1 */
    U16 reserved        : 4;        /* Byte1 : for future use */
#else
    U16 reserved        : 4;        /* Byte1 : for future use */
    U16 packetSeqNum    :12;        /* Byte0&1: 1 = indicates this is MFI_OOB_CMD_PACKET1 */
#endif
    U8  applicationMsgTag;          /* Byte2 : optional context for BMC to tag each message
                                               0xFF is reserved */
    U8  mfiCmd;             /* Byte3 : supported MFI_CMD_OP_PD_SCSI=4, MFI_CMD_OP_DCMD=5, & MFI_CMD_OP_ABORT=6
                             *         MFI_CMD_OP_SMP=7, MFI_CMD_OP_STP=8
                             */
    U32 lengthOfCmdPayload; /* Byte4 : Length of Paylod in DWORDs */
    U16 responseDataLength; /* Byte8 : Expected response length in DWORDs of the data */

    union {
        U8  reserved1[2];   /* Byte10: for future use (MFI_CMD_DCMD) */
        struct {            /* MFI_CMD_OP_PD_SCSI */
            U8  targetID;   /* Byte10: target ID */
            U8  LUN;        /* Byte11: target ID LUN */
        } mfiOOBCmdTL;      /* unnamed struct:  search for NO_UNNAMED_STRUCT for usage */
        /* other commands may be defined in future */
    };

    union {
        struct {            /* MFI_CMD_OP_DCMD */
            U32 dcmdOpcode; /* Byte12: DCMD opcode */
            union {
                U8  mbox[12];        /* Byte16: DCMD mbox arguments */
                U8  endOfCmdPacket;  /* Byte16: CMD packet may be terminated at this point
                                      *         if there are no mbox values.
                                      */
            };
        } dcmd;
        struct {            /* MFI_CMD_OP_PD_SCSI */
            U8  cdb[16];    /* Byte12: SCSI opcode shall also indicate length of CDB */
        } dcdb;
        struct {            /* MFI_CMD_OP_STP */
            U8  endOfCmdPacket;  /* Byte12: Terminate the command packet at offsetof(stp.ndOfCmdPacket) bytes
                                  * if FIS and any data will follow in the next packet.
                                  * If packetSize can accomodate FIS, then it may begin at Byte28, and be included
                                  * in the first cmd packet itself.
                                  *
                                  * Note:
                                  *
                                  * STP Command: Use MFI_STP_FIS for the FIS, and
                                  *              include it at the start of the command payload.
                                  *              Use the subsequent packets to deliver the FIS and data if any.
                                  *              lengthOfCmdPayload (byte 4) must account for MFI_STP_FIS and
                                  *              data if any.
                                  *
                                  * STP Response: Use MFI_CMD_STP_STATUS for the response FIS, and
                                  *               include it at the start of response payload.
                                  *               MFI_OOB_RESP_PACKET1.responseDataLength must account for
                                  *               MFI_CMD_STP_STATUS and response data if any.
                                  */
        } stp;
        struct {                  /* MFI_CMD_OP_SMP */
            U8  sasaddr[8];       /* Byte12: SAS address of the target expander device */  
            U8  endOfCmdPacket;   /* Byte20: Terminate the command packet at offsetof(stp.ndOfCmdPacket) bytes
                                   * and SMP request will start from packet1 in the data */
        } smp;
        /* other commands may be defined in future */
    };

    U8  data[4];            /* Byte28: Variable length data for command (data is optional)
                             *         Cmd packet may be terminated at this point, if no data.
                             */
} MFI_OOB_CMD_PACKET1;

typedef struct _MFI_OOB_CMD_PACKETX {
#ifndef MFI_BIG_ENDIAN
    U16 packetSeqNum    :12;        /* Byte0&1: value > 1 indicates this is MFI_OOB_CMD_PACKETX */
    U16 reserved        : 4;        /* Byte1 : for future use */
#else
    U16 reserved        : 4;        /* Byte1 : for future use */
    U16 packetSeqNum    :12;        /* Byte0&1: value > 1 indicates this is MFI_OOB_CMD_PACKETX */
#endif
    U8  applicationMsgTag;          /* Byte2 : optional context for BMC to tag each message
                                               0xFF is reserved */
    U8  data[1];                    /* Byte3 : Variable length data for subsequent packets */
                                    /* Data length is determined by STOP bit in i2c transaction */
} MFI_OOB_CMD_PACKETX;

/*
 * define MFI OOB Message payload - RESPONSE packets
 */

typedef struct _MFI_OOB_RESP_PACKET1 {
#ifndef MFI_BIG_ENDIAN
    U16 packetSeqNum    :12;        /* Byte0&1: 1 = indicates this is MFI_OOB_RESP_PACKET1 */
    U16 reserved        : 4;        /* Byte1 : for future use */
#else
    U16 reserved        : 4;        /* Byte1 : for future use */
    U16 packetSeqNum    :12;        /* Byte0&1: 1 = indicates this is MFI_OOB_RESP_PACKET1 */
#endif
    U8  applicationMsgTag;          /* Byte2 : optional context for BMC to tag each message */

    U8  cmdStatus;                  /* Byte3 : MFI_STAT */
    U16 responseDataLength;         /* Byte4 : actual response length in DWORDs of data */
    U8  extStatus;                  /* Byte6 : reserved for DCMDs.  SCSI_STATUS for DCDBs                       */
                                    /*         If cmdStatus=MFI_STAT_SCSI_DONE_WITH_ERROR, data[]=request sense */
    U8  data[1];                    /* Byte7 : Variable length data for response (data is optional) */
                                    /* Data length is determined by STOP bit in i2c transaction */
} MFI_OOB_RESP_PACKET1;

typedef struct _MFI_OOB_RESP_PACKETX {
#ifndef MFI_BIG_ENDIAN
    U16 packetSeqNum    :12;        /* Byte0&1: value > 1 indicates this is MFI_OOB_RESP_PACKETX */
    U16 reserved        : 4;        /* Byte1 : for future use */
#else
    U16 reserved        : 4;        /* Byte1 : for future use */
    U16 packetSeqNum    :12;        /* Byte0&1: value > 1 indicates this is MFI_OOB_RESP_PACKETX */
#endif
    U8  applicationMsgTag;          /* Byte2 : optional context for BMC to tag each message */

    U8  data[1];                    /* Byte3 : Variable length data for subsequent packets */
                                    /* Data length is determined by STOP bit in i2c transaction */
} MFI_OOB_RESP_PACKETX;

/*
 * define the Packet structure
 * Following is for illustration purpose only.
 * Actual bytes serialized over I2C will depend on the contents of the packet
 */
typedef struct _MFI_OOB_LSI_PKT {
    MFI_OOB_LSI_HDR     header;         // not returned for pure slave reads
    union {                             // optional based on HDR information
        MFI_OOB_CMD_PACKET1     commandPkt1;
        MFI_OOB_CMD_PACKETX     commandPktX;
        MFI_OOB_RESP_PACKET1    responsePkt1;
        MFI_OOB_RESP_PACKETX    responsePktX;
    } message;
} MFI_OOB_LSI_PKT;

typedef struct _MFI_OOB_PCI_MSG_PKT {
    MFI_OOB_PCI_MSG_HDR     pciHeader;
    union {                             // optional based on HDR information
        MFI_OOB_CMD_PACKET1     commandPkt1;
        MFI_OOB_CMD_PACKETX     commandPktX;
        MFI_OOB_RESP_PACKET1    responsePkt1;
        MFI_OOB_RESP_PACKETX    responsePktX;
    } message;
} MFI_OOB_PCI_MSG_PKT;

/*
 * define MCTP packet header
 */
typedef struct _MFI_OOB_MCTP_HDR {
    U8  destSlaveAddressRW  : 1;    /* Byte0 : always set to 0, MCTP only issues writes in both direction */
    U8  destSlaveAddress    : 7;    /* Byte0 : i2c slave address (controller address) */
    U8  cmdCode;                    /* Byte1 : 0x0F-MCTP over SMBus */
    U8  byteCount;                  /* Byte2 : bytes in this packet starting at byte3, not including PEC */
    U8  srcSlaveAddressRW   : 1;    /* Byte3 : LSB is 1 to separate from IPMI/IPMB */
    U8  srcSlaveAddress     : 7;    /* Byte3 : i2c slave address of BMC */

    U8  hdrVersion          : 4;    /* Byte4 : 0x1 - MCTP spec version */
    U8  reserved            : 4;    /* Byte4 : reserved by MCTP spec */
    U8  destinationEID;             /* Byte5 : Destination endpoint ID per MCTP base spec */
    U8  sourceEID;                  /* Byte6 : Source endpoint ID per MCTP base spec */

    U8  msgTag              : 3;    /* Byte7 : MCTP msg tag  */
    U8  tagOwner            : 1;    /* Byte7 : 1-Source sets the msgTag */
    U8  seqNumber           : 2;    /* Byte7 : packet sequence number for out of order detection */
    U8  EOM                 : 1;    /* Byte7 : End of message */
    U8  SOM                 : 1;    /* Byte7 : Start of message */
} MFI_OOB_MCTP_HDR;

/*
 * define first MCTP packet
 */
typedef struct _MFI_OOB_MCTP_PACKET {
    MFI_OOB_MCTP_HDR    header;                 /* Byte0 */

    union {
        struct {
            U8  msgType        : 7;             /* Byte8 : 0x7E - Vendor defined message */
            U8  integrityCheck : 1;             /* Byte8 : IC is present at end of packet */
            U8  vendorID[2];                    /* Byte9 : PCI Vendor ID */
            U8  controlCmd;                     /* Byte11: MFI_OOB_CONTROL_CMD */

            union {                             /* Byte12 */
                MFI_OOB_CMD_PACKET1  packet1;
                MFI_OOB_RESP_PACKET1 responsePkt1;
                U8                  data[1];    /*         variable length payload for non-LSI defined message */
            };
        } packet1;

        MFI_OOB_CMD_PACKETX     packetX;        /* Byte8 : variable length payload */
        MFI_OOB_RESP_PACKETX    responsePktX;
    };
} MFI_OOB_MCTP_PACKET;

/*
* defines the enum for MCTP payload identifier
*/
typedef enum _MFI_OOB_MCTP_CONTROL_MSG {

    MFI_OOB_MCTP_CONTROL_RESPONSE               = 0xA,     // Control Command response, payload is MFI_OOB_STATUS
    MFI_OOB_MCTP_CONTROL_OOB_STATUS             = 0xB,     // Control Command OOB status - a quick way to check status of OOB service
    MFI_OOB_MCTP_CONTROL_STATUS_POLL            = 0xC,     // Control Command status poll - a quick way to poll controller status
    MFI_OOB_MCTP_CONTROL_MFI_STAT               = 0xD,     // MFI STAT response for DCMD, payload is MFI_STAT
    MFI_OOB_MCTP_CONTROL_MRAPI                  = 0x10,    // MR API message
    // control commands
    MFI_OOB_MCTP_CONTROL_PAUSE                  = 0x81,    // pause the transfer
    MFI_OOB_MCTP_CONTROL_RESUME                 = 0x82,    // resume, payload is packetCount from where to resume
    // exception commands
    MFI_OOB_MCTP_CONTROL_EXCEPTION_GENERAL      = 0x90,    // exception  general packet exception.
    MFI_OOB_MCTP_CONTROL_EXCEPTION_ORDER        = 0x91,    // exception  out of order packet exception.
    MFI_OOB_MCTP_CONTROL_EXCEPTION_PEC          = 0x92,    // exception  PEC failure, payload is packetCount on which occured.
    MFI_OOB_MCTP_CONTROL_EXCEPTION_INV_PARAM    = 0x93,    // exception  payload with invalid parameter.
    MFI_OOB_MCTP_CONTROL_EXCEPTION_BUSY_RETRY   = 0x94,    // exception  Busy try later.
    MFI_OOB_MCTP_CONTROL_EXCEPTION_TARGET       = 0x95,    // exception  Target unavailable.
    MFI_OOB_MCTP_CONTROL_EXCEPTION_TAG_INUSE    = 0x96,    // exception  app msg tag already in use.
    MFI_OOB_MCTP_CONTROL_EXCEPTION_PENDING      = 0x98,    // exception  indicates the command is still being processed
    // recovery commands
    MFI_OOB_MCTP_CONTROL_ABORT                  = 0xA0,    // recovery  abort all the commands.
    MFI_OOB_MCTP_CONTROL_RESET                  = 0xA1,    // recovery  reset the interface and FW OOB service
} MFI_OOB_MCTP_CONTROL_MSG;

typedef enum _MFI_OOB_METHOD {
    MFI_OOB_BINDING_LSI_PROPRIETARY     = 0x0,
    MFI_OOB_BINDING_MCTP_OVER_I2C       = 0x1,
    MFI_OOB_BINDING_MCTP_OVER_PCIE      = 0x2,
    MFI_OOB_BINDING_LSI_SMBUS           = 0x3,
} MFI_OOB_METHOD;

typedef struct   _MFI_OOB_METHOD_INFO {
    U16     OOBMaxPayloadSize;      // Max Payload Size
    U16     OOBMaxPacketSize;       // Max Packet Size
    U8      OOBCommunicationMode;   // OOB Binding Method
    U8      reserved[3];
} MFI_OOB_METHOD_INFO;

typedef enum _MFI_OOB_CTRL_INFO_PARAMS {
    MFI_OOB_MANUFACTURING_CODE = 0x0,  // Device ID of the controller
    MFI_OOB_CTRL_ID            = 0x1,  // PnP IDs
    MFI_OOB_MAX_ROC_TEMP       = 0x2,  // Max allowed ROC temperature
    MFI_OOB_CURRENT_ROC_TEMP   = 0x3,  // Current ROC temperature
} MFI_OOB_CTRL_INFO_PARAMS;

typedef union _MFI_OOB_CTRL_GENERIC_INFO {
    struct {
        U16     subVendorId;            // sub-vendor ID for this board
        U16     subDeviceId;            // sub-device ID for this board
        U16     vendorId;               // vendor ID for this board
        U16     deviceId;               // device ID for this board
    };
    struct{
        U8      currentROCTemp;         // current ROC temperature
        U8      reserved1[7];
    };
    struct{
        U8      maxROCTemp;             // max allowed ROC temperature
        U8      reserved2[7];
    };
    struct{
        U16     mfgId;                  // device ID for this board
        U8      reserved[6];
    };
    U64 data;
}MFI_OOB_CTRL_GENERIC_INFO;

#endif /* INCLUDE_MFI */
