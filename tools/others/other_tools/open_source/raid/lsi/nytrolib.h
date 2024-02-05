/****************************************************************  
 * FILE        : nytrolib.h
 * DESCRIPTION : Nytro specific structures and definition
 * AUTHOR      : Debabrata Bardhan (DB)
 * DATE        : 07-10-2013
 ********************************************************************************* 
    Copyright 2008-2014 Avago Technologies.  All rights reserved.
 *********************************************************************************

 *********************************************************************************
 * Revision History:
 * 07-10-2013 : DB      : Initial Release.
 * 08-21-2013 : DB      : Change in following Structures - 
 *                          SL_NYTRO_GET_CTRL_INFO_T
 *                          SL_NY_SATA_CURRENT_ATTRIBUTES
 *                          SL_NY_SATA_CUMULATIVE_ATTRIBUTES
 *                          SL_NY_SAS_CURRENT_ATTRIBUTES
 *                          SL_NY_SAS_CUMULATIVE_ATTRIBUTES
 *                          SL_Power_Data_T
 * 08-21-2013 : DB      : Added new enum SL_AUX_POWER_STATUS
 * 08-21-2013 : DB      : Added new structure SL_NYTRO_BOARD_INFO_S
 * 09-19-2013 : DB		: Description added for SL_NYTRO_SET_SSD_STATE.
 * 09-19-2013 : DB		: Replacing SL_NYTRO_RESET_CONTROLLER 
 *							with SL_NYTRO_GET_DEFAULT_CONFIG_DETAILS.
 * 09-24-2013 : DB		: Added Supported for 
 *							SL_NYTRO_GET_DEFAULT_CONFIG_DETAILS
 *							SL_NYTRO_RAID_SUSPEND_TASK
 *							SL_NYTRO_RAID_RESTORE_TASK
 * 10-23-2013 : DB		: Removed deprecated command SL_NYTRO_SSD_CHANGE_STATE 
 * 10-23-2013 : DB		: Added Supported for Power Monitoring
 * 11-12-2013 : DB		: Added field
 *							SL_NY_SATA_CUMULATIVE_ATTRIBUTES.CacheDataAge
 * 11-29-2013 : DB		: Change in SL_NYTRO_SSD_STATE_T - Change SlotId to U8 from U16.
 * 12-02-2013 : DB		: Change in DEFAULT_WD_CONFIG_DETAILS to support 
 *							Cage, Location and TrimCount.
 * 12-04-2013 : DB		: Add field SL_NY_SATA_CUMULATIVE_ATTRIBUTES.POMS
 * 23-04-2013 : DB		: Add field SL_NYTRO_CTRL_PROP_T.EnableDLC
 *						  Changed the size of SL_NYTRO_CTRL_PROP_T
 * 24-01-2014 : PS		: Changed description of SL_NYTRO_GET_SSD_INFO (Adding FUA comment)
 * 24-01-2014 : RT		: Adding support for ReadOnlyMode 
 * 17-02-2014 : SB      : Added BoardType in SL_NYTRO_BOARD_INFO_T.
 *********************************************************************************/


#ifndef __NYTROLIB_H__
#define __NYTROLIB_H__

#include "storelib.h"

#pragma pack(1)


/*********************************************************************************
 *  
 *         Nytro Commands
 *  
 *********************************************************************************/
enum SL_NYTRO_CMD
{
    // Begin
   /*
    * SL_NYTRO_GET_CTRL_INFO
    * Desc:
    *   Get Controller level Health information for the selected Nytro Controller.
    *   This will give details about the Nytro controller
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of the type SL_NYTRO_GET_CTRL_INFO_T
    *   dataSize: SL_NYTRO_GET_CTRL_INFO_S
    * Output:
    *   pData: SL_NYTRO_GET_CTRL_INFO_T
    *   dataSize: SL_NYTRO_GET_CTRL_INFO_S
    * StoreLib Error Codes:
    *   SL_ERR_INCORRECT_DATA_SIZE
    *   SL_ERR_MEMORY_ALLOC_FAILED
    *   SL_ERR_INVALID_CTRL
    */
    SL_NYTRO_GET_CTRL_INFO,

   /*
    * SL_NYTRO_GET_SSD_INFO
    * Desc:
    *   Get onboard SSD drive information for the selected SSD on Nytro Controller
    * Input:
    *   ctrlId: Controller Id
	*	libCmdParam.cmdParam_1b[0] = 1 for forced unit access (FUA), 0 otherwise
    *   pdRef.deviceID: Device ID of the physical drive
    *   pData: Pointer to buffer of the type SL_NYTRO_GET_SSD_INFO_T
    *   dataSize: SL_NYTRO_GET_SSD_INFO_S
    * Output:
    *   pData: Pointer to buffer of the type SL_NYTRO_GET_SSD_INFO_T
    *   dataSize: SL_NYTRO_GET_SSD_INFO_S
    * StoreLib Error Codes:
    *   SL_ERR_INCORRECT_DATA_SIZE
    *   SL_ERR_MEMORY_ALLOC_FAILED
    *   SL_ERR_IR_DEVICE_NOT_FOUND
    *   SL_ERR_INVALID_CTRL
    */
    SL_NYTRO_GET_SSD_INFO,

    /**
     * Following are Config command exposed for WarpDrive. 
     * The original intention was to make it available as WarpDrive. 
     * It is exposed as a Nytro command to ensure zero change in 
     * WarpDrive commands due to Nytro requirement. 
     * NMR and Raptor controller need support these commands to 
     * maintain consistency. 
     */

#if 0

   /*
    * SL_NYTRO_SET_LD_PROPERTIES
    * Desc:
    *   Sets ld properties.
    * Input:
    *   ctrlId: Controller Id, MR_CTRL_PROP
    *   ldRef.seqNum: Logical drive sequence number
    *   ldRef.targetId: Logical drive Id
    *   pData: Pointer to buffer of MR_LD_PROPERTIES struct with data to be set
    *   dataSize: sizeof(MR_LD_PROPERTIES)
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_NYTRO_SET_LD_PROPERTIES,

   /*
    * SL_NYTRO_DELETE_LD
    * Desc:
    *   Attempts to delete the ld.
    * Input:
    *   ctrlId: Controller Id
    *   ldRef.targetId: Logical drive Id
    *   ldRef.seqNum: Sequence Number of Logical drive Id
    *   pData: None
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_NYTRO_DELETE_LD,

   /*
    * SL_NYTRO_ADD_CONFIG
    * Desc:
    *   Adds specified config to current config.
    *   If new config is desired, then existing config needs to be cleared first
    *   prior to using this command.
    * Input:
    *   ctrlId: Controller Id
    *   pData: MR_CONFIG_DATA buffer containing config data.
    *   dataSize: sizeof(MR_CONFIG_DATA)
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_NYTRO_ADD_CONFIG,

   /*
    * SL_NYTRO_CLEAR_CONFIG
    * Desc:
    *   Clears existing config
    * Input:
    *   ctrlId: Controller Id
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_NYTRO_CLEAR_CONFIG,

#endif

    /**
     * Above are Config command exposed for WarpDrive. 
     * The original intention was to make it available as WarpDrive. 
     * It is exposed as a Nytro command to ensure zero change in 
     * WarpDrive commands due to Nytro requirement. 
     * NMR and Raptor controller need support these commands to 
     * maintain consistency. 
     */
   /*
    * SL_NYTRO_FLASH_SSD_FIRMWARE
    * Desc:
    *   Flashes DFF firmware.
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.deviceID: Device ID of the physical drive
    *   pData: Pointer to buffer containing binary image
    *   dataSize: size of binary image
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_INVALID_FW_FLASH_IMAGE
    */
    SL_NYTRO_FLASH_SSD_FIRMWARE,

   /*
    * SL_NYTRO_GET_VPD
    * Desc:
    *   Get VPD for WarpDrive.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer containing binary VPD Data
    *   WD  -   MAN PAGE 1
    *   NMR -   Unknown
    *   dataSize: size of VPD Data
    * Output:
    *   pData: Pointer to buffer containing binary VPD Data
    *   dataSize: Size of the VPD Page.
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_INVALID_CMD
    */
    SL_NYTRO_GET_VPD,

   /*
    * SL_NYTRO_SET_VPD
    * Desc:
    *   Set VPD for WarpDrive.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer containing binary VPD Data
    *   WD  -   MAN PAGE 1
    *   NMR -   Unknown
    *   dataSize: size of VPD Data
    * Output: 
    *   None 
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_INVALID_CMD
    */
    SL_NYTRO_SET_VPD,
  
    /*
    * SL_NYTRO_GET_POWER_MONITORING_INFO
    * Desc:
    *   Get Power Monitoring Info of Nytro Controller.
    * Input:
    *   ctrlId: Controller Id
    *   pData: SL_NYTRO_POWER_MONITORING_INFO_T buffer containing state data.
    *   dataSize: sizeof(SL_NYTRO_POWER_MONITORING_INFO_T)
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_INVALID_CMD
    *   SL_ERR_IR_DEVICE_NOT_FOUND
    */
    SL_NYTRO_GET_POWER_MONITORING_INFO,

#if 0
    
    /*
    * SL_FLASH_SSD_FIRMWARE
    * Desc:
    *   Flashes SSD firmware.
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.deviceID: Device ID of the physical drive
    *   pData: Pointer to buffer containing binary image
    *   dataSize: size of binary image
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_INVALID_FW_FLASH_IMAGE
    * AVAGO SAS IT/IR Controllers: Flashing BIOS as a separate image is supported for this ctrl
    */
    SL_FLASH_SSD_FIRMWARE,
    
#endif

    /*
    * SL_NYTRO_QUERY_PANIC
    * Desc:
    *   Query a specific SSD to determine if it has any Panic Logs. There can
    *   be up to 16 Panic Logs stored in the SSD by the SSD Firmware. The Panic
    *   Logs are a Binary File that can be collected and sent to SandForce for
    *   analysis.
    *
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.deviceID: Slot number of the physical drive
    *   pData: Pointer to empty buffer of size SL_NYTRO_PANIC_DUMP_HEADER_S
    *   dataSize: SL_NYTRO_PANIC_DUMP_HEADER_S
    * Output:
    *   pData: Pointer to buffer of the type SL_NYTRO_PANIC_DUMP_HEADER_T
    *   dataSize: SL_NYTRO_PANIC_DUMP_HEADER_S
    * StoreLib Error Codes:
    *   SL_SUCCESS
    *   SL_ERR_MEMORY_ALLOC_FAILED
    */
    SL_NYTRO_QUERY_PANIC,
    
    /*
    * SL_NYTRO_EXTRACT_PANIC
    * Desc:
    *   Extract the specified Panic Log
    *
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.deviceID: Slot number of the physical drive
    *   cmdParam_1b[0]: Panic Entry to extract.  There are up to 16 panic entries.
    *                   This parameter will be 0-15
    *   pData: Pointer to empty buffer, at least as large as what is returned
    *          by the SL_NYTRO_QUERY_PANIC command
    *   dataSize: Size of the Panic Buffer(pData).
    * Output:
    *   pData: Pointer to data buffer allocated by application to hold the
    *          panic information
    *   dataSize: Size of the Panic Buffer. This buffer should be at least as
    *             large as what is returned by the SL_NYTRO_QUERY_PANIC command
    * StoreLib Error Codes:
    *   SL_SUCCESS
    *   SL_ERR_MEMORY_ALLOC_FAILED
    */
    SL_NYTRO_EXTRACT_PANIC,
    
    /*
    * SL_NYTRO_ERASE_PANIC
    * Desc:
    *   Erase the specified Panic Log. Panics should be erased from highest
    *   slot to lowest because of SF firmware bug
    *
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.deviceID: Slot number of the physical drive
    *   cmdParam_1b[0]: Panic Entry to erase.  There are up to 16 panic entries.
    *                   This parameter will be 0-15
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_SUCCESS
    */
    SL_NYTRO_ERASE_PANIC,
      
    /*
    * SL_NYTRO_MST_START
    * Desc:
    *   Erase the specified Panic Log. Panics should be erased from highest
    *   slot to lowest because of SF firmware bug
    *
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.deviceID: Slot number of the physical drive
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_SUCCESS
    */
    SL_NYTRO_MST_START,

   /*
    * SL_NYTRO_GET_SMART_LOG
    * Desc:
    *   Get SMART Log for the selected SSD on Nytro Controller
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.deviceID: Device ID of the physical drive
    *   pData: Pointer to buffer of the type SL_NYTRO_SMART_LOG_T
    *   dataSize: SL_NYTRO_SMART_LOG_S + SL_NYTRO_SMART_LOG_T.BufferSize
    * Output:
    *   pData: Pointer to buffer of the type SL_NYTRO_SMART_LOG_T
    *   dataSize: SL_NYTRO_SMART_LOG_S + SL_NYTRO_SMART_LOG_T.BufferSize
    * StoreLib Error Codes:
    *   SL_ERR_INCORRECT_DATA_SIZE
    *   SL_ERR_MEMORY_ALLOC_FAILED
    *   SL_ERR_IR_DEVICE_NOT_FOUND
    *   SL_ERR_INVALID_CTRL
    */
    SL_NYTRO_GET_SMART_LOG,


   /*
    * SL_NYTRO_GET_BOARD_DETAILS
    * Desc:
    *   Get following details of WarpDrive controller.
    *   ChipName, ChipRevision, BoardName, BoardAssembly, BoardTracerNumber, BoardType
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of the type SL_NYTRO_BOARD_INFO_T
    *   dataSize: SL_NYTRO_BOARD_INFO_S
    * Output:
    *   pData: Pointer to buffer of the type SL_NYTRO_BOARD_INFO_T
    *   dataSize: SL_NYTRO_BOARD_INFO_S
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_INVALID_CMD
    */
    SL_NYTRO_GET_BOARD_DETAILS,

    /*
    * SL_NYTRO_SET_BOARD_DETAILS
    * Desc:
    *   Set following details of WarpDrive controller.
    *   ChipName, ChipRevision, BoardName, BoardAssembly, BoardTracerNumber
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of the type SL_NYTRO_BOARD_INFO_T
    *   dataSize: SL_NYTRO_BOARD_INFO_S
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_INVALID_CMD
    */
    SL_NYTRO_SET_BOARD_DETAILS,


    /* 
    * -------- Hold on it can be Factory command --------
    * SL_NYTRO_GET_CTRL_PROP
    * Desc:
    *   Get Controller FW to IT/IR Mode.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of the type SL_NYTRO_CTRL_PROP_T
    *   dataSize: SL_NYTRO_CTRL_PROP_S
    * Output:
    *   PData: Pointer to buffer of the type SL_NYTRO_CTRL_PROP_T
    *   dataSize: SL_NYTRO_CTRL_PROP_S
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_INVALID_CMD  
    */
    SL_NYTRO_GET_CTRL_PROP,


    /* 
    * -------- Hold on it can be Factory command -------- 
    * SL_NYTRO_SET_CTRL_PROP
    * Desc:
    *   Set Controller FW to IT/IR Mode.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of the type SL_NYTRO_CTRL_PROP_T
    *   dataSize: SL_NYTRO_CTRL_PROP_S
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_INVALID_CMD  
    */
    SL_NYTRO_SET_CTRL_PROP,


   /*
    * SL_NYTRO_FORMAT_SSD
    * Desc:
    *   Format a specific SSD. SSD drive will be formatted in user mode
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.deviceID: Device ID of the physical drive
    * Output:
    *   NA
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_SSD_FORMAT_FAILED
    */
    SL_NYTRO_FORMAT_SSD,

   /*
    * SL_NYTRO_DISPLAY_SET_POWER
    * Desc:
    *   Set the power of the controller in watts.
    * Input:
    *   ctrlId: Controller Id
    *   pData: SL_Power_Data_T buffer containing config data.
    *   dataSize: sizeof(SL_Power_Data_T)
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    */
    SL_NYTRO_DISPLAY_SET_POWER,

	/*
    * SL_NYTRO_GET_DEFAULT_CONFIG_DETAILS
    * Desc:
    *   Get config details of Nytro controller.
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of the type SL_NYTRO_CONFIG_DETAILS_T
    *   dataSize: SL_NYTRO_CONFIG_DETAILS_S
    * Output:
    *   pData: Pointer to buffer of the type SL_NYTRO_CONFIG_DETAILS_T
    *   dataSize: SL_NYTRO_CONFIG_DETAILS_S
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_INVALID_CMD
    */
    SL_NYTRO_GET_DEFAULT_CONFIG_DETAILS,

    /*
    * SL_NYTRO_SET_SSD_STATE
    * Desc:
    *   Set SSD State.
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.deviceID: Device ID of the physical drive
    *   pData: Pointer to buffer of the type SL_NYTRO_SSD_STATE_T
    *   dataSize: SL_NYTRO_SSD_STATE_S
	* Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_INVALID_CMD
    */
	SL_NYTRO_SET_SSD_STATE,

    /*
    * SL_NYTRO_RESET_TARGET
    * Desc:
    *   Reset DFF.
    * Input:
    *   ctrlId: Controller Id
    *   pdRef.deviceID: Device ID of the physical drive
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_INVALID_CMD
    *   SL_ERR_IR_DEVICE_NOT_FOUND
    */
	SL_NYTRO_RESET_TARGET,
   /*
    * SL_NYTRO_LOCATE
    * Desc:
    *   Locate Nytro Controller by initiating
    *   specific LED blink pattern
    * Input:
    *   ctrlId: Controller Id
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_INVALID_CMD_TYPE
    *   SL_ERR_INVALID_CMD
    */
    SL_NYTRO_LOCATE,
    SL_NYTRO_SMART_EVENT_LOG,

   /*
    * SL_NYTRO_RAID_SUSPEND_TASK
    * Desc:
    *   Locate Nytro Controller by initiating
    *   specific LED blink pattern
    * Input:
    *   ctrlId: Controller Id
    *   pData: Pointer to buffer of the type SL_NYTRO_RAID_SUSPEND_PARAM_T
    *   dataSize: SL_NYTRO_RAID_SUSPEND_PARAM_S
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_INVALID_CMD_TYPE
    *   SL_ERR_INVALID_CMD
    */
    SL_NYTRO_RAID_SUSPEND_TASK,

   /*
    * SL_NYTRO_RAID_RESTORE_TASK
    * Desc:
    *   Locate Nytro Controller by initiating
    *   specific LED blink pattern
    * Input:
    *   ctrlId: Controller Id
    * Output:
    *   None
    * StoreLib Error Codes:
    *   SL_ERR_INVALID_CTRL
    *   SL_ERR_INVALID_CMD_TYPE
    *   SL_ERR_INVALID_CMD
    */
    SL_NYTRO_RAID_RESTORE_TASK,

    //End
    //@@@@@ Always add new commands at the end @@@@@//
};




/*############################################################################
*
*        NYTRO Specific definitions
*
############################################################################*/

/****************************************************************************
*
*        NYTRO Controller Level INFO Structure
*
****************************************************************************/

#if 0

typedef struct _SL_NY_CTRL_HEALTH
{
#ifdef SPARC
    U8          ErrCode;                
    U8          State;
#else
    U8          State;              /*  Current overall health  */
    U8          ErrCode;            /*  ErrorCode in case Warning or Critical   */
#endif

} SL_NY_CTRL_HEALTH, *PTR_SL_NY_CTRL_HEALTH;

#endif 


typedef struct _SL_NYTRO_GET_CTRL_INFO_T
{
    /*  Byte - 0-3 */

    //SL_NY_CTRL_HEALTH         OverAllHealth;

#ifdef SPARC
    U8          Reserved1;
	U8			AuxPowerStatus;				/*	This provide the status of the Auxilary Power	*/

    U8          ThrottlingStatus;           /*  100 - No throttling being done
                                             *  90 - One (or more) of the SSDs are being throttled */           
    U8          AuxPowerPresent;            /* This confirms the presence of Auxiliary power.
                                             *          0 - Aux Power not present
                                             *          1 - Aux Power Present
                                             *          0xFF - Aux Power not supported */
#else
    U8          AuxPowerPresent;            /* This confirms the presence of Auxiliary power.
                                             *          0 - Aux Power not present
                                             *          1 - Aux Power Present
                                             *          0xFF - Aux Power not supported */
    U8          ThrottlingStatus;           /*  100 - No throttling being done
                                             *  90 - One (or more) of the SSDs are being throttled */
	U8			AuxPowerStatus;				/*	This provide the status of the Auxilary Power	*/
    U8	        Reserved1;

#endif

    /*  Byte - 4-7 */
#ifdef SPARC
    U8          MaxPCISlotPower;            /* Maximum threshold of PCISlotPower */
    U8          MinPCISlotPower;            /* Minimum threshold of PCISlotPower */
    U8          PCISlotPower;               /* Current power allocated to the controller 
                                             * through PCI Slot */
    U8          LifeLeft;                   /* Percentage of life left.
                                             * This field reports worst case of all the SSD drives attached to the controller */
#else
    U8          LifeLeft;                   /* Percentage of life left.
                                             * This field reports worst case of all the SSD drives attached to the controller */
    U8          PCISlotPower;               /* Current power allocated to the controller 
                                             * through PCI Slot */
    U8          MinPCISlotPower;            /* Minimum threshold of PCISlotPower */
    U8          MaxPCISlotPower;            /* Maximum threshold of PCISlotPower */
#endif

    /*  Byte - 8-31 */
    U32         Reserved2[6];

} SL_NYTRO_GET_CTRL_INFO_T, *PTR_SL_NYTRO_GET_CTRL_INFO_T;

#define SL_NYTRO_GET_CTRL_INFO_S sizeof(SL_NYTRO_GET_CTRL_INFO_T)


/****************************************************************************
*
*        NYTRO SSD INFO Structure
*
****************************************************************************/


typedef struct _SL_NY_SSD_BASIC_INFO
{
    /*  Byte - 0-3 */
#ifdef SPARC
    U8          FlashType;                                  /* Flash Type being used (SLC / MLC / EMLC) */
    U8          Temperature;                                /* Temperature of the onboard SSD */
    U8          Location;                                   /* Current location of SSD 0 - Upper, 1 - Lower. 
                                                             * This is applicable only for onboard SSDs */
    U8          Cage;                                       /* Current Cage that holds the SSD */
#else
    U8          Cage;                                       /* Current Cage that holds the SSD */
    U8          Location;                                   /* Current location of SSD 0 - Upper, 1 - Lower. 
                                                             * This is applicable only for onboard SSDs */
    U8          Temperature;                                /* Temperature of the onboard SSD */
    U8          FlashType;                                  /* Flash Type being used (SLC / MLC / EMLC) */
#endif

    /*  Byte - 4-7 */
#ifdef SPARC
	U8			Reserved1;
	U8			AccessStatus;								/* if 1 then in ReadOnlyMode */
    U8          LockedUp;									/* 1-SSD is locked up, 0-SSD is not locked up */
    U8          DevProtocol;                                /* 0-Unknown, 1-parallel SCSI, 2-SAS, 3-SATA, 4-FC
                                                             						  * derived from   MR_PD_INFO.interfaceType */
#else
    U8          DevProtocol;                                /* 0-Unknown, 1-parallel SCSI, 2-SAS, 3-SATA, 4-FC
                                                             						  * derived from   MR_PD_INFO.interfaceType */
	U8			LockedUp;                                   /* 1-SSD is locked up, 0-SSD is not locked up */
	U8			AccessStatus;								/* if 1 then in ReadOnlyMode */
	U8			Reserved1;
#endif

    /*  Byte - 8-31 */
    U32         Reserved2[6];

} SL_NY_SSD_BASIC_INFO, *PTR_SL_NY_SSD_BASIC_INFO;


typedef struct _SL_NY_SATA_CURRENT_ATTRIBUTES
{
        /*  Byte - 0-15 */
    U64  RawReadErrorRate;       /* Bytes Read */
    U32  SATACRCErrorCount;      /* SATA R-Errors (CRC) Error Count */
    U32  ECCOnFlyCount;          /* Uncorrectable ECC Errors */

        /*  Byte - 16-19 */
#ifdef SPARC
    U8   LifeCurveStatus;        /* Drive Life Protection Status */
    U8   WearRangeDelta;         /* Wear Range Delta */
    U8   UncorrectableErrors;    /* Uncorrectable RAISE Errors */
    U8   Reserved1;            
#else
    U8   Reserved1;		         
    U8   UncorrectableErrors;    /* Uncorrectable RAISE Errors */
    U8   WearRangeDelta;         /* Wear Range Delta */
    U8   LifeCurveStatus;        /* Drive Life Protection Status */
#endif

        /*  Byte - 20-47 */
    U32 Reserved2[7];

} SL_NY_SATA_CURRENT_ATTRIBUTES, *PTR_SL_NY_SATA_CURRENT_ATTRIBUTES;

typedef struct _SL_NY_SATA_CUMULATIVE_ATTRIBUTES
{
    /*  Byte - 0-35 */
    U32   RetiredBlockCount;                  /* Retired Block Count */
    U32   POH;                                /* Power-On Hours */
    U32   DevPowerCycleCount;                 /* Device Power Cycle Count */
    U32   UnxepectedPowerLossCount;           /* Unexpected Power Loss Count */
    U32   IOErrorDetectionRate;               /* I/O Error Detection Code Rate */
    U32   ReservedBlockCount;                 /* Reserved (over-provisioned) Blocks */
    U32   ProgramFailCount1;                  /* Program Fail Count */
    U32   EraseFailCount1;                    /* Erase Fail Count */
    U32   SoftReadErrorRate;                  /* Soft Read Error Count */

        /*  Byte - 36-39 */
#ifdef SPARC
    U8    Reserved1;
	U8	  MaxTemperature;					  /* Highest temperature sampled over the lifetime of the device */
    U8    LifeLeft;                           /* SSD Life Left */
    U8    UncorrectableErrors;                /* Uncorrectable RAISE Errors */
#else
    U8    UncorrectableErrors;                /* Uncorrectable RAISE Errors */
    U8    LifeLeft;                           /* SSD Life Left */
	U8	  MaxTemperature;					  /* Highest temperature sampled over the lifetime of the device */
    U8	  Reserved1;
#endif

        /*  Byte - 40-59 */
    U32   LifeTimeWrites;                     /* Total Writes From Host */
    U32   LifeTimeReads;                      /* Total Reads To Host */
    U32   PECycles;                           /* Gigabytes Erased */
    float WriteAmplification;                 /* Write amplification is not applicable if the value in this field is 0 */
	U32	  CacheDataAge;						  /* Cached SMART Data Age in seconds */
	U32	  POMS;								  /* Power-On MilliSecond */

        /*  Byte - 60-99 */
    U32   Reserved2[8]  ;
} SL_NY_SATA_CUMULATIVE_ATTRIBUTES, *PTR_SL_NY_SATA_CUMULATIVE_ATTRIBUTES;

typedef struct _SL_NY_SATA_SSD_HEALTH
{
        /*  Byte - 0-143 */
    SL_NY_SATA_CURRENT_ATTRIBUTES       Current;
    SL_NY_SATA_CUMULATIVE_ATTRIBUTES    Cumulative;
} SL_NY_SATA_SSD_HEALTH, *PTR_SL_NY_SATA_SSD_HEALTH;

typedef struct _SL_NY_SAS_CURRENT_ATTRIBUTES
{
        /*  Byte - 0-23 */
    U64  RawReadErrorRate;       /* Bytes Read */
    U32  eccRate;                /* ECC Rate */
    U32  errPrimCount;           /* Error Primitive Count */
    U32  uncorrectableErrCnt;    /* Uncorrectable Error Count */
    U32  Reserved1;            /* Current Temperature */

        /*  Byte - 24-47 */
    U32   Reserved2[6];
} SL_NY_SAS_CURRENT_ATTRIBUTES, *PTR_SL_NY_SAS_CURRENT_ATTRIBUTES;

typedef struct _SL_NY_SAS_CUMULATIVE_ATTRIBUTES
{
        /*  Byte - 0-27 */
    U32   reallocBlkCnt;                      /* Reallocated Block Count */
    U32   POH;                                /* Power-On Hours */
    U32   accuStartStopCycles;                /* Accumulated Start Stop Cycles */
    U32   ReservedBlockCount;                 /* Unused Reserved Block Count */
    U32   totalUncorrectedWriteErr;           /* Total Uncorrected Write Errors */
    U32   eraseFailCnt;                       /* Erase Fail Count */
    U32   maxLifetimeTemp;                    /* Maximum Lifetime Temperature */

        /*  Byte - 28-31 */
#ifdef SPARC
    U8    Reserved1[2];
	U8	  MaxTemperature;					  /* Highest temperature sampled over the lifetime of the device */
    U8    LifeLeft;                           /* SSD Life Left */
#else
    U8    LifeLeft;                           /* SSD Life Left */
	U8	  MaxTemperature;					  /* Highest temperature sampled over the lifetime of the device */
    U8    Reserved1[2];
#endif

        /*  Byte - 32-59 */
    U64   gbErased;                           /* Gigabytes Erased */
    U64   LifeTimeWrites;                     /* Total Writes From Host */
    U64   LifeTimeReads;                      /* Total Reads To Host */
    float WriteAmplification;                 /* Write amplification is not applicable if the value in this field is 0 */

        /*  Byte - 60-95 */
    U32   Reserved2[9];

} SL_NY_SAS_CUMULATIVE_ATTRIBUTES, *PTR_SL_NY_SAS_CUMULATIVE_ATTRIBUTES;


typedef struct _SL_NY_SAS_SSD_HEALTH
{
        /*  Byte - 0-143 */
    SL_NY_SAS_CURRENT_ATTRIBUTES    Current;
    SL_NY_SAS_CUMULATIVE_ATTRIBUTES Cumulative;
} SL_NY_SAS_SSD_HEALTH, *PTR_SL_NY_SAS_SSD_HEALTH;


typedef union _SL_NY_SSD_HEALTH
{
    SL_NY_SATA_SSD_HEALTH       sataSSDHealth;
    SL_NY_SAS_SSD_HEALTH        sasSSDHealth;
} SL_NY_SSD_HEALTH, *PTR_SL_NY_SSD_HEALTH;


typedef struct _SL_NYTRO_GET_SSD_INFO_T
{
    SL_NY_SSD_BASIC_INFO        ssdBasicInfo;
    SL_NY_SSD_HEALTH            ssdHealthInfo;
} SL_NYTRO_GET_SSD_INFO_T, *PTR_SL_NYTRO_GET_SSD_INFO_T;

#define SL_NYTRO_GET_SSD_INFO_S sizeof(SL_NYTRO_GET_SSD_INFO_T)


typedef enum _SL_SSD_CAGE
{
    SL_ONBOARD_SSD_CAGE_1        = 1,
    SL_ONBOARD_SSD_CAGE_2        = 2,
    SL_ONBOARD_SSD_CAGE_3        = 3,
    SL_ONBOARD_SSD_CAGE_4        = 4,
    SL_ONBOARD_SSD_CAGE_INVALID  = 0xFF

}SL_SSD_CAGE;

typedef enum _SL_SSD_LOCATION
{
    SL_ONBOARD_SSD_LOCATION_LOWER    = 0,
    SL_ONBOARD_SSD_LOCATION_UPPER    = 1,
    SL_ONBOARD_SSD_LOCATION_INVALID  = 0xFF

}SL_SSD_LOCATION;

typedef enum _SL_AUX_POWER
{
    SL_AUX_POWER_NOT_PRESENT = 0,
    SL_AUX_POWER_PRESENT = 1,
    SL_AUX_POWER_NOT_SUPPORTED = 0xFF

}SL_AUX_POWER;

typedef enum _SL_AUX_POWER_STATUS
{
    SL_AUX_POWER_STATUS_GOOD		= 0,
    SL_AUX_POWER_STATUS_ERROR		= 1,
    SL_AUX_POWER_STATUS_UNKNOWN		= 0xFF

}SL_AUX_POWER_STATUS;


#define SSD_THROTTLING_OFF    (100)      /* Throttling disabled */
#define SSD_THROTTLING_ON     (90)       /* Throttling enabled  */

typedef enum _SL_SSD_FLASH_TYPE
{
    SL_SSD_FLASHTYPE_SLC,
    SL_SSD_FLASHTYPE_MLC,
    SL_SSD_FLASHTYPE_EMLC,
    SL_SSD_FLASHTYPE_CMLC,
    SL_SSD_FLASHTYPE_UNKNOWN = 0xFF

}SL_SSD_FLASH_TYPE;

typedef enum _SL_NY_CTRL_STATUS
{
    SL_NY_STATUS_GOOD,
    SL_NY_STATUS_WARNING,
    SL_NY_STATUS_ERROR,
    SL_NY_STATUS_UNKNOWN = 0xFF

}SL_NY_CTRL_STATUS;


#define MAX_POWER_VALUE 60

typedef struct _SL_NYTRO_POW_CONF_T
{
#ifdef SPARC
    U8      Reserved1[3];
    U8      Power;
#else
    U8      Power;
    U8      Reserved1[3];
#endif
} SL_NYTRO_POW_CONF_T, *PTR_SL_NYTRO_POW_CONF_T;

#define SL_NYTRO_POW_CONF_S sizeof(SL_NYTRO_POW_CONF_T)

typedef enum _SL_NY_SLOT_STATE
{
    SL_NY_SLOT_STATE_DISABLE,
    SL_NY_SLOT_STATE_ENABLE,
    SL_NY_SLOT_STATE_UNKNOWN = 0xFF

}SL_NY_SLOT_STATE;

typedef enum _SL_NY_SLOT_SETTING
{
    SL_NY_SLOT_SETTING_CURRENT,
    SL_NY_SLOT_SETTING_PERSISTENT,
    SL_NY_SLOT_SETTING_UNKNOWN = 0xFF

}SL_NY_SLOT_SETTING;


typedef struct _SL_NYTRO_SSD_STATE_T
{
#ifdef SPARC
	U8		Reserved0;
	U8		SlotID;
	U16		EnclID;
#else
	U16		EnclID;
	U8		SlotID;
	U8		Reserved0;
#endif

#ifdef SPARC
    U8      Reserved1[2];
    U8      Setting;
    U8      State;
#else
    U8      State;
    U8      Setting;
    U8      Reserved1[2];
#endif

	U32		Reserved2[6];

} SL_NYTRO_SSD_STATE_T, *PTR_SL_NYTRO_SSD_STATE_T;

#define SL_NYTRO_SSD_STATE_S    sizeof(SL_NYTRO_SSD_STATE_T)


/****************************************************************************
*
*        NYTRO SSD Panic Log Structure
*
****************************************************************************/
#define MAX_PANIC_HEADERS                       16

#define NYTRO_PANIC_EXTRACT_FUNCTION_SEEK       0
#define NYTRO_PANIC_EXTRACT_FUNCTION_EXTRACT    1
#define NYTRO_PANIC_EXTRACT_FUNCTION_ERASE      2

#define NYTRO_PANIC_EXTRACT_SEEK_SCAN           0
#define NYTRO_PANIC_EXTRACT_SEEK_ROOT           1


typedef struct _SL_NYTRO_PANIC_DUMP_HEADER_T
{
    U32     HeaderPrefixLow;    /* From spec: Constant Magic Number */
                                /* 0x50 0x41 0x4e 0x49 0x44 0x55 0x4d 0x50*/
    U32     HeaderPrefixHigh;
    U32     TransactionNumber;  /* sourc code control transaction number*/
    U32     SyncCode;           /* sync code to match panic dump with firmware version*/
    U32     UnlockLevel;        /* Unlock level at time of panic dump*/
    U32     HeaderSize;         /* Size of the header in the panic dump*/
    U32     DataSize;           /* size of panic dump in bytes*/
    U32     ePageAddress;       /* ePage address location of the panic dump*/
#ifdef SPARC
    U16     CRC;                /* CRC check on panic dump header 1=valid 0=invalid*/  
    U16     Valid;              /* 1 = Panic Dump is valid, 0= panic dump is invalid*/ 
#else
    U16     Valid;              /* 1 = Panic Dump is valid, 0= panic dump is invalid*/ 
    U16     CRC;                /* CRC check on panic dump header 1=valid 0=invalid*/  
#endif

} SL_NYTRO_PANIC_DUMP_HEADER_T, *PTR_SL_NYTRO_PANIC_DUMP_HEADER_T;

#define SL_NYTRO_PANIC_DUMP_HEADER_S sizeof(SL_NYTRO_PANIC_DUMP_HEADER_T)

typedef struct _SL_NYTRO_PANIC_INFO_T
{
    SL_NYTRO_PANIC_DUMP_HEADER_T  PanicInfo[MAX_PANIC_SLOTS];
} SL_NYTRO_PANIC_INFO_T;

#define SL_NYTRO_PANIC_INFO_S sizeof(SL_NYTRO_PANIC_INFO_T)


#define SMART_LOGS_SSD_EVENT_LOG                0xB7
#define SMART_LOGS_SYSTEM_EVENT_LOG             0xFB
#define SMART_LOGS_GROWN_DEFECT_LOG             0xA8
#define MST_WRITE_LOG_ADDRESS                   0xF8
#define MST_READ_LOG_ADDRESS                    0xF9

#define SENTINEL_VALUE                          0x8888CAFE

/**
 * Used for MST Status
 */

typedef struct _SL_MST_Results {
    U32 length_of_results_1;
    U32 result_type;
    U32 format_version;
    U32 rblock_capacity;
    U32 test_state;
    U8 reserved[108];
    U32 input_offset;
    U32 current_command;
    U32 cycles_required;
    U32 cycles_completed;
    U32 rblocks_tested;
    U32 remaining_rblocks;
    U32 elapsed_time;
    U32 start_temperature;
    U32 max_temperature;
    U32 correctable_eccx_errors;
    U32 uncorrectable_eccx_errors;
    U32 program_errors;
    U32 erase_errors;
    U32 grown_bad_blocks;
    U32 remapped_blocks;
    U32 pages_with_0_errors;
    U32 _1bit_errors;
    U32 _2bit_errors;
    U32 _3bit_errors;
    U32 _4bit_errors;
    U32 _5bit_errors;
    U32 _6bit_errors;
    U32 _7bit_errors;
    U32 _8bit_errors;
    U32 _9bit_errors;
    U32 _10bit_errors;
    U32 _11bit_errors;
    U32 _12bit_errors;
    U32 _13bit_errors;
    U32 _14bit_errors;
    U32 _15bit_errors;
    U32 _16bit_errors;
    U32 _17bit_errors;
    U32 _18bit_errors;
    U32 _19bit_errors;
    U32 _20bit_errors;
    U32 _21bit_errors;
    U32 _22bit_errors;
    U32 _23bit_errors;
    U32 _24bit_errors;
    U32 _25bit_errors;
    U32 _26bit_errors;
    U32 _27bit_errors;
    U32 _28bit_errors;
    U32 _29bit_errors;
    U32 _30bit_errors;
    U32 _31bit_errors;
    U32 pages_with_0_errors_hi;
    U32 _1bit_errors_hi;
    U32 _2bit_errors_hi;
    U32 _3bit_errors_hi;
    U32 _4bit_errors_hi;
    U32 _5bit_errors_hi;
    U32 _6bit_errors_hi;
    U32 _7bit_errors_hi;
    U8 reserved2[164];
} SL_MST_Results;


typedef struct _SL_NYTRO_SMART_LOG_T
{
#ifdef SPARC
    U16     PageNumber;
    U16     NumSectors;

    U8      Reserved[3];
    U8      LogAddress;
#else
    U16     NumSectors;
    U16     PageNumber;

    U8      LogAddress;
    U8      Reserved[3];
#endif
    U32     BufferSize;
    U8      *buff;
} SL_NYTRO_SMART_LOG_T;

#define SL_NYTRO_SMART_LOG_S    sizeof(SL_NYTRO_SMART_LOG_T)


typedef struct _SL_NYTRO_CTRL_PROP_T
{
#ifdef SPARC
    U32 Reserved1   : 31;
    U32 FWMode      : 1;    // 0 - IT FW, 1 - IR FW
#else
    U32 FWMode      : 1;    // 0 - IT FW, 1 - IR FW
    U32 Reserved1   : 31;
#endif

	struct {
#ifdef SPARC
		U32	Reserved2	: 30;
		U32 ReadOnlyMode: 1;	
		U32	EnableDLC	: 1;	
#else
		U32 EnableDLC   : 1;    // Action bit for DLC. 0 - No change in DLC state, 1 - Change in DLC state
		U32 ReadOnlyMode: 1;	// Action bit for ReadOnlyMode.  0 - no change in readOnlyMode, 1 - Change in readOnlyMode
		U32 Reserved2   : 30;
#endif
		U32 Reserved3[3];
	} OnOffProperty;

#ifdef SPARC
		U8 ReadOnlyModeLifeLeftPercentage;
		U8 ReadOnlyModeExitFreeSpacePercent;
		U8 ReadOnlyModeEnterFreeSpacePercent;
		U8 Reserve4 			: 6;
		U8 ReadOnlyModeValue 	: 1;
		U8 DLCValue				: 1;
		
		
#else
		U8 DLCValue				: 1;	// if enableDLC bit is 1, then 0 - disable DLC, 1 - enable DLC
		U8 ReadOnlyModeValue 	: 1;	// if ReadOnlyMode bit is 1, then 0 - disable readOnlyMode, 1 - enable readOnlyMode
		U8 Reserve4 			: 6;
		U8 ReadOnlyModeEnterFreeSpacePercent;	// free space threshold when ReadOnlyMode enter
		U8 ReadOnlyModeExitFreeSpacePercent;	// free space threshold when ReadOnlyMode exit
		U8 ReadOnlyModeLifeLeftPercentage;		// life left threshold when ReadOnlyMode enter
#endif
    U32     Reserved5[32];
} SL_NYTRO_CTRL_PROP_T;


#define SL_NYTRO_CTRL_PROP_S    sizeof(SL_NYTRO_CTRL_PROP_T)


typedef struct _SL_Power_Data
{
#ifdef SPARC
	U16		Reserved1;
	U8		writePowerValue;	// 0 - Read only, 1 - Write
	U8		powerValue;
#else
    U8		powerValue;
    U8		writePowerValue;	// 0 - Read only, 1 - Write
	U16		Reserved1;
#endif
	U32		Reserved2[3];
}SL_Power_Data_T;

#define SL_Power_Data_S    sizeof(SL_Power_Data_T)

typedef struct _SL_NYTRO_BOARD_INFO_T
{
	struct {
	#ifdef SPARC
		U32 Reserved1			: 28;
		U32 SasAddress			: 1;
		U32 BoardTracerNumber	: 1;
		U32 BoardAssembly		: 1;
		U32 BoardName			: 1;    
	#else
		U32 BoardName			: 1;    
		U32 BoardAssembly		: 1;
		U32 BoardTracerNumber	: 1;
		U32 SasAddress			: 1;
		U32	Reserved1			: 28;
	#endif

		U32	Reserved2[3];

	} onOffProperties;

    U8                      BoardName[16];              /* 0x10 */
    U8                      BoardAssembly[16];          /* 0x20 */
    U8                      BoardTracerNumber[16];      /* 0x30 */
    U64                     SASAddress;                 /* 0x40 */
    U8                      BoardType;                  /* 0x48 */
    U8                      Reserved3[3];               /* 0x49 */
    U32                     Reserved4[29];              /* 0x52 */

} SL_NYTRO_BOARD_INFO_T;

#define SL_NYTRO_BOARD_INFO_S	sizeof(SL_NYTRO_BOARD_INFO_T)



#define MAX_ONBOARD_SLOTS	8


typedef struct _WD_SSD_CAGE_LOCATION_TRIM_INFO
{
	#ifdef SPARC
		U16				Reserved;
		U8				Location;
		U8				Cage;
	#else
		U8				Cage;
		U8				Location;
		U16				Reserved;
	#endif

		U64				SlotTrimCount;
} WD_SSD_CAGE_LOCATION_TRIM_INFO;


typedef struct _DEFAULT_WD_CONFIG_DETAILS
{
	U32						Man10OEMSpecificFlags0;		/* 0x00 */
    U32                     Man4R0Settings;				/* 0x04 */
    U32                     Man4R1ESettings;			/* 0x08 */
    U32                     Man4R1Settings;				/* 0x0C */
    U32                     Man4R10Settings;			/* 0x10 */

#ifdef SPARC	
    U8                      Man4MaxVolumes;             /* 0x17 */
    U8                      Man4MaxPhysDisks;           /* 0x16 */
    U8                      Man4MaxPhysDisksPerVol;     /* 0x15 */
	U8                      Man4MaxHotSpares;           /* 0x14 */
#else
	U8                      Man4MaxHotSpares;           /* 0x14 */
    U8                      Man4MaxPhysDisksPerVol;     /* 0x15 */
    U8                      Man4MaxPhysDisks;           /* 0x16 */
    U8                      Man4MaxVolumes;             /* 0x17 */
#endif

#ifdef SPARC	
    U8                      Ioc6MaxDrivesR10;           /* 0x1B */
    U8                      Ioc6MaxDrivesR1E;           /* 0x1A */
    U8                      Ioc6MaxDrivesR1;            /* 0x19 */
    U8                      Ioc6MaxDrivesR0;            /* 0x18 */
#else
    U8                      Ioc6MaxDrivesR0;            /* 0x18 */
    U8                      Ioc6MaxDrivesR1;            /* 0x19 */
    U8                      Ioc6MaxDrivesR1E;           /* 0x1A */
    U8                      Ioc6MaxDrivesR10;           /* 0x1B */
#endif    

#ifdef SPARC	
    U8                      Ioc6MinDrivesR10;           /* 0x1F */
    U8                      Ioc6MinDrivesR1E;           /* 0x1E */
    U8                      Ioc6MinDrivesR1;            /* 0x1D */
	U8                      Ioc6MinDrivesR0;            /* 0x1C */
#else
	U8                      Ioc6MinDrivesR0;            /* 0x1C */
    U8                      Ioc6MinDrivesR1;            /* 0x1D */
    U8                      Ioc6MinDrivesR1E;           /* 0x1E */
    U8                      Ioc6MinDrivesR10;           /* 0x1F */
#endif 

	/* WarpDrive support 8 onboard slot */
	WD_SSD_CAGE_LOCATION_TRIM_INFO		CLTInfo[MAX_ONBOARD_SLOTS];		/* 0x20 - 0x7F */

	U32						Reserved1[32];				/* 0x80 - 0xFF */

} DEFAULT_WD_CONFIG_DETAILS;

typedef struct _DEFAULT_NMR_CONFIG_DETAILS
{
	U32						Reserved1[64];
} DEFAULT_NMR_CONFIG_DETAILS;


typedef union _DEFAULT_CONFIG_DETAILS
{
	DEFAULT_WD_CONFIG_DETAILS			WDConfigDetails;
	DEFAULT_NMR_CONFIG_DETAILS			NMRConfigDetails;
} DEFAULT_CONFIG_DETAILS;


typedef struct _SL_NYTRO_CONFIG_DETAILS_T
{
	DEFAULT_CONFIG_DETAILS			DefaultConfigDetails;
	U32								Reserved1[32];
} SL_NYTRO_CONFIG_DETAILS_T;

#define SL_NYTRO_CONFIG_DETAILS_S	sizeof(SL_NYTRO_CONFIG_DETAILS_T)


/* RAID ACTION SUSPEND TASKS macros */
#define MPI2_RAID_ACTION_SUSPEND_TASK  (0x83)
#define SUSPEND_TASK                   (0x1)
#define RESTORE_TASK                   (0x0)
#define TASK_SUSPEND_MINUTES_PER_SSD   (6)

typedef struct _SL_NYTRO_RAID_SUSPEND_PARAM_T
{
#ifdef SPARC	
    U16                     Reserved2;				/* 0x02 */
    U8                      Reserved1;				/* 0x1D */
	U8                      SSDCount;				/* 0x00 */
#else
	U8                      SSDCount;				/* 0x00 */
    U8                      Reserved1;				/* 0x01 */
    U16                     Reserved2;				/* 0x02 */
#endif 
	
	U32						Reserved3[7];

} SL_NYTRO_RAID_SUSPEND_PARAM_T;


typedef struct _WD_POWER_MONITORING_INFO
{
	U32 LTC12VPowerInfo; 
	U32 LTC33VPowerInfo; 
	U32 TotalBoardPowerInfo; 

	U32	Reserved1[29];
} WD_POWER_MONITORING_INFO;

typedef struct _NMR_POWER_MONITORING_INFO
{
	U32						Reserved1[32];
} NMR_POWER_MONITORING_INFO;


typedef union _POWER_MONITORING_INFO
{
	WD_POWER_MONITORING_INFO			WDPowerMontoringInfo;
	NMR_POWER_MONITORING_INFO			NMRPowerMonitoringInfo;
} POWER_MONITORING_INFO;

typedef struct _SL_NYTRO_POWER_MONITORING_INFO_T
{
	POWER_MONITORING_INFO			PowerMontoringInfo;
	U32								Reserved1[32];
} SL_NYTRO_POWER_MONITORING_INFO_T;

#pragma pack() //@@@ Add all data structures above this inside pragma pack decl @@@@


#endif //__NYTROLIB_H__

